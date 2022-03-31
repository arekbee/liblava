/**
 * @file         liblava/app/app.cpp
 * @brief        Application with basic functionality
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <imgui.h>
#include <liblava/app/app.hpp>
#include <liblava/app/def.hpp>
#include <liblava/base/debug_utils.hpp>

namespace lava {

//-----------------------------------------------------------------------------
app::app(frame_config config)
: frame(config), window(config.info.app_name) {}

//-----------------------------------------------------------------------------
app::app(name name, argh::parser cmd_line)
: frame(frame_config(name, cmd_line)), window(name) {}

//-----------------------------------------------------------------------------
void app::handle_config() {
    config_file.add(&config_callback);

    config_callback.on_load = [&](json_ref j) {
        if (!j.count(config.id))
            return;

        auto j_config = j[config.id];

        if (j_config.count(_paused_))
            run_time.paused = j_config[_paused_];

        if (j_config.count(_speed_))
            run_time.speed = j_config[_speed_];

        if (j_config.count(_fixed_delta_))
            run_time.use_fix_delta = j_config[_fixed_delta_];

        if (j_config.count(_delta_))
            run_time.fix_delta = ms(j_config[_delta_]);

        if (j_config.count(_imgui_))
            imgui.set_active(j_config[_imgui_]);

        if (j_config.count(_v_sync_))
            config.v_sync = j_config[_v_sync_];

        if (j_config.count(_physical_device_))
            config.physical_device = j_config[_physical_device_];
    };

    config_callback.on_save = [&]() {
        json j;

        j[config.id][_paused_] = run_time.paused;
        j[config.id][_speed_] = run_time.speed;
        j[config.id][_fixed_delta_] = run_time.use_fix_delta;
        j[config.id][_delta_] = run_time.fix_delta.count();
        j[config.id][_imgui_] = imgui.activated();
        j[config.id][_v_sync_] = config.v_sync;
        j[config.id][_physical_device_] = config.physical_device;

        return j;
    };

    config_file.load();
}

//-----------------------------------------------------------------------------
bool app::create_block() {
    if (!block.create(device, target->get_frame_count(), device->graphics_queue().family))
        return false;

    block_command = block.add_cmd([&](VkCommandBuffer cmd_buf) {
        scoped_label block_label(cmd_buf, _lava_block_, { default_color, 1.f });

        auto current_frame = block.get_current_frame();

        {
            scoped_label stage_label(cmd_buf, _lava_texture_staging_, { 0.f, 0.13f, 0.4f, 1.f });
            staging.stage(cmd_buf, current_frame);
        }

        if (on_process)
            on_process(cmd_buf, current_frame);

        shading.get_pass()->process(cmd_buf, current_frame);
    });

    return true;
}

//-----------------------------------------------------------------------------
bool app::setup() {
    if (!frame::ready())
        return false;

    auto& cmd_line = get_cmd_line();

    if (!setup_file_system(cmd_line))
        return false;

    handle_config();

    if (!setup_window(cmd_line))
        return false;

    if (!setup_device(cmd_line))
        return false;

    if (!setup_render())
        return false;

    setup_run();

    if (parse(cmd_line, frames))
        benchmark(*this, frames);

    return true;
}

//-----------------------------------------------------------------------------
bool app::setup_file_system(cmd_line cmd_line) {
    log()->debug("physfs {}", str(to_string(file_system::get_version())));

    if (!file_system::instance().initialize(str(get_cmd_line()[0]), config.org, get_name(), config.ext)) {
        log()->error("init file system");
        return false;
    }

    file_system::instance().mount_res(log());

    if (cmd_line[{ "-c", "--clean" }])
        file_system::instance().clean_pref_dir();

    return true;
}

//-----------------------------------------------------------------------------
bool app::setup_window(cmd_line cmd_line) {
    if (auto id = cmd_line({ "-id", "--identification" })) {
        config.id = id.str();
        remove_chars(config.id, _punctuation_marks_);
        window.set_save_name(str(config.id));
        window.show_save_title();
    }

    if (!window.create(load_window_state(window.get_save_name())))
        return false;

    log()->trace("{}: {}", _fullscreen_, window.fullscreen());

    set_window_icon(window);

    return true;
}

//-----------------------------------------------------------------------------
bool app::setup_device(cmd_line cmd_line) {
    cmd_line({ "-vs", "--v_sync" }) >> config.v_sync;
    cmd_line({ "-pd", "--physical_device" }) >> config.physical_device;

    if (!device) {
        device = create_device(config.physical_device);
        if (!device)
            return false;
    }

    auto physical_device = device->get_physical_device();

    auto device_name = trim_copy(physical_device->get_device_name());
    auto device_type = physical_device->get_device_type_string();
    auto device_driver_version = physical_device->get_driver_version();

    log()->info("device: {} ({}) - driver: {}",
                str(device_name), str(device_type), str(to_string(device_driver_version)));

    return true;
}

//-----------------------------------------------------------------------------
bool app::setup_render() {
    if (!create_target())
        return false;

    log()->trace("{}: {}", _v_sync_, target->get_swapchain()->v_sync());

    if (!camera.create(device))
        return false;

    camera.aspect_ratio = window.get_aspect_ratio();
    camera.update_projection();

    if (!create_imgui())
        return false;

    return create_block();
}

//-----------------------------------------------------------------------------
void app::setup_run() {
    handle_input();
    handle_window();

    update();
    render();

    add_run_end([&]() {
        camera.destroy();

        destroy_imgui();

        block.destroy();

        destroy_target();

        if (config.save_window)
            save_window_file(window);

        window.destroy();

        if (!config_file.save())
            log()->error("cannot save config file {}", config_file.get());

        config_file.clear();

        file_system::instance().terminate();
    });

    add_run_once([&]() {
        return on_create ? on_create() : run_continue;
    });

    frame_counter = 0;
}

//-----------------------------------------------------------------------------
bool app::create_imgui() {
    if (config.imgui_font.file.empty()) {
        auto font_files = file_system::enumerate_files(_font_path_);
        if (!font_files.empty())
            config.imgui_font.file = fmt::format("{}{}", _font_path_, str(font_files.front()));
    }

    setup_imgui_font(imgui_config, config.imgui_font);

    imgui_config.ini_file_dir = file_system::get_pref_dir();

    imgui.setup(window.get(), imgui_config);
    if (!imgui.create(device, target->get_frame_count(), shading.get_vk_pass()))
        return false;

    if (format_srgb(target->get_format()))
        imgui.convert_style_to_srgb();

    shading.get_pass()->add(imgui.get_pipeline());

    imgui_fonts = make_texture();
    if (!imgui.upload_fonts(imgui_fonts))
        return false;

    staging.add(imgui_fonts);

    return true;
}

//-----------------------------------------------------------------------------
void app::destroy_imgui() {
    imgui.destroy();
    imgui_fonts->destroy();
}

//-----------------------------------------------------------------------------
bool app::create_target() {
    target = lava::create_target(&window, device, config.v_sync, config.surface);
    if (!target)
        return false;

    if (!shading.create(target))
        return false;

    if (!renderer.create(target->get_swapchain()))
        return false;

    window.assign(&input);

    return on_create ? on_create() : true;
}

//-----------------------------------------------------------------------------
void app::destroy_target() {
    if (on_destroy)
        on_destroy();

    renderer.destroy();

    shading.destroy();
    target->destroy();
}

//-----------------------------------------------------------------------------
void app::handle_input() {
    input.add(&imgui.get_input_callback());

    input.key.listeners.add([&](key_event::ref event) {
        if (imgui.capture_keyboard()) {
            camera.stop();
            return input_ignore;
        }

        if (config.handle_key_events) {
            if (event.pressed(key::q, mod::control))
                return shut_down();

            if (event.pressed(key::tab, mod::control)) {
                imgui.toggle();
                return input_done;
            }

            if (event.pressed(key::b, mod::control)) {
                frames.exit = false;
                benchmark(*this, frames);
                return input_done;
            }

            if (event.pressed(key::enter, mod::alt)) {
                window.set_fullscreen(!window.fullscreen());
                return input_done;
            }

            if (event.pressed(key::backspace, mod::alt)) {
                toggle_v_sync = true;
                return input_done;
            }

            if (event.pressed(key::space, mod::control)) {
                run_time.paused = !run_time.paused;
                return input_done;
            }
        }

        if (camera.activated())
            return camera.handle(event);

        return input_ignore;
    });

    input.mouse_button.listeners.add([&](mouse_button_event::ref event) {
        if (imgui.capture_mouse())
            return input_ignore;

        if (camera.activated())
            return camera.handle(event, input.get_mouse_position());

        return input_ignore;
    });

    input.scroll.listeners.add([&](scroll_event::ref event) {
        if (imgui.capture_mouse())
            return input_ignore;

        if (camera.activated())
            return camera.handle(event);

        return input_ignore;
    });

    add_run([&](id::ref run) {
        input.handle_events();
        input.set_mouse_position(window.get_mouse_position());

        return run_continue;
    });

    add_run_end([&]() {
        input.remove(&imgui.get_input_callback());
    });
}

//-----------------------------------------------------------------------------
void app::handle_window() {
    add_run([&](id::ref run) {
        if (window.close_request())
            return shut_down();

        if (window.switch_mode_request() || toggle_v_sync || target->reload_request()) {
            device->wait_for_idle();

            log()->info("- {}", _reload_);

            destroy_target();
            destroy_imgui();

            if (window.switch_mode_request()) {
                if (config.save_window)
                    save_window_file(window);

                auto window_state = load_window_state(window.get_save_name());
                if (window_state)
                    window_state->fullscreen = !window.fullscreen();

                log()->debug("{}: {}", _fullscreen_, window_state->fullscreen ? _on_ : _off_);

                if (!window.switch_mode(window_state))
                    return false;

                set_window_icon(window);
            }

            if (toggle_v_sync) {
                config.v_sync = !config.v_sync;

                log()->debug("{}: {}", _v_sync_, config.v_sync ? _on_ : _off_);

                toggle_v_sync = false;
            }

            if (!create_target())
                return run_abort;

            return create_imgui();
        }

        if (window.resize_request()) {
            camera.aspect_ratio = window.get_aspect_ratio();
            camera.update_projection();

            return window.handle_resize();
        }

        return run_continue;
    });
}

//-----------------------------------------------------------------------------
void app::update() {
    run_time.system = now();

    add_run([&](id::ref run) {
        auto dt = ms(0);
        auto time = now();

        if (run_time.system != time) {
            dt = time - run_time.system;
            run_time.system = time;
        }

        run_time.delta = dt;

        if (!run_time.paused) {
            if (run_time.use_fix_delta)
                dt = run_time.fix_delta;

            dt = to_ms(to_sec(dt) * run_time.speed);
            run_time.current += dt;
        } else
            dt = ms(0);

        return on_update ? on_update(to_delta(dt)) : run_continue;
    });
}

//-----------------------------------------------------------------------------
void app::render() {
    add_run([&](id::ref run) {
        if (window.iconified()) {
            sleep(one_ms);
            return run_continue;
        }

        auto frame_index = renderer.begin_frame();
        if (!frame_index.has_value())
            return run_continue;

        frame_counter++;

        if (!block.process(*frame_index))
            return run_abort;

        return renderer.end_frame(block.get_buffers());
    });
}

//-----------------------------------------------------------------------------
void app::draw_about(bool separator) const {
    if (separator)
        ImGui::Separator();

    ImGui::Spacing();

    imgui_left_spacing(2);

    ImGui::Text("%s %s", _liblava_, str(version_string()));

    if (config.handle_key_events && ImGui::IsItemHovered())
        ImGui::SetTooltip("pause = control + space\ngui = control + tab\nv-sync = alt + backspace\nfullscreen = alt + enter");

    imgui_left_spacing();

    if (v_sync())
        ImGui::Text("%.f fps (v-sync)", ImGui::GetIO().Framerate);
    else
        ImGui::Text("%.f fps", ImGui::GetIO().Framerate);

    if (run_time.paused) {
        ImGui::SameLine();
        ImGui::TextUnformatted(_paused_);
    }
}

} // namespace lava
