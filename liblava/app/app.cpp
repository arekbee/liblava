/**
 * @file         liblava/app/app.cpp
 * @brief        Application with basic functionality
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <imgui.h>
#include <liblava/app/app.hpp>
#include <liblava/app/def.hpp>
#include <liblava/asset/write_image.hpp>
#include <liblava/base/debug_utils.hpp>

namespace lava {

//-----------------------------------------------------------------------------
app::app(frame_env env)
: frame(env), window(env.info.app_name) {
    parse_config(env.cmd_line);
}

//-----------------------------------------------------------------------------
app::app(name name, argh::parser cmd_line)
: frame(frame_env(name, cmd_line)), window(name) {
    parse_config(cmd_line);
}

//-----------------------------------------------------------------------------
void app::parse_config(argh::parser cmd_line) {
    if (auto id = cmd_line({ "-id", "--identification" })) {
        config.id = id.str();
        remove_chars(config.id, _punctuation_marks_);
    }

    if (auto fullscreen = -1; cmd_line({ "-wf", "--fullscreen" }) >> fullscreen)
        config.window_state->fullscreen = fullscreen == 1;

    if (auto x_pos = -1; cmd_line({ "-wx", "--x_pos" }) >> x_pos)
        config.window_state->x = x_pos;

    if (auto y_pos = -1; cmd_line({ "-wy", "--y_pos" }) >> y_pos)
        config.window_state->y = y_pos;

    if (auto width = -1; cmd_line({ "-ww", "--width" }) >> width)
        config.window_state->width = width;

    if (auto height = -1; cmd_line({ "-wh", "--height" }) >> height)
        config.window_state->height = height;

    cmd_line({ "-vs", "--v_sync" }) >> config.v_sync;
    cmd_line({ "-pd", "--physical_device" }) >> config.physical_device;
}

//-----------------------------------------------------------------------------
void app::handle_config() {
    config.context = this;

    config_callback.on_load = [&](json_ref j) {
        if (!j.count(config.id))
            return false;

        config.set_config(j[config.id]);
        return true;
    };

    config_callback.on_save = [&]() {
        json j;
        j[config.id] = config.get_config();
        return j;
    };

    config_file.add(&config_callback);
    config_file.load();
}

//-----------------------------------------------------------------------------
bool app::create_block() {
    if (!block.create(device,
                      target->get_frame_count(),
                      device->graphics_queue().family))
        return false;

    block_command = block.add_cmd([&](VkCommandBuffer cmd_buf) {
        scoped_label block_mark(cmd_buf,
                                _lava_block_,
                                { default_color, 1.f });

        auto current_frame = block.get_current_frame();

        {
            scoped_label stage_mark(cmd_buf,
                                    _lava_texture_staging_,
                                    { 0.f, 0.13f, 0.4f, 1.f });

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

    if (!setup_file_system())
        return false;

    handle_config();

    auto& cmd_line = get_cmd_line();

    if (auto paused = -1; cmd_line({ "-p", "--paused" }) >> paused)
        run_time.paused = paused == 1;

    if (auto delta = -1; cmd_line({ "-d", "--delta" }) >> delta)
        run_time.fix_delta = ms(delta);

    cmd_line({ "-s", "--speed" }) >> run_time.speed;

    if (on_setup && !on_setup())
        return false;

    if (!setup_window())
        return false;

    if (!setup_device())
        return false;

    if (!setup_render())
        return false;

    setup_run();

    if (parse(cmd_line, frames))
        benchmark(*this, frames);

    return true;
}

//-----------------------------------------------------------------------------
bool app::setup_file_system() {
    log()->debug("physfs {}", to_string(fs.get_version()));

    auto& cmd_line = get_cmd_line();

    if (!fs.initialize(cmd_line[0],
                       config.org,
                       get_name(),
                       config.ext)) {
        log()->error("init file system");
        return false;
    }

    fs.mount_res(log());

    if (cmd_line[{ "-c", "--clean" }]) {
        fs.clean_pref_dir();
        log()->info("clean preferences");
    }

    if (cmd_line[{ "-cc", "--clean_cache" }]) {
        std::filesystem::remove_all(fs.get_pref_dir() + "cache/");

        log()->info("clean cache");
    }

    return true;
}

//-----------------------------------------------------------------------------
bool app::setup_window() {
    if (config.id != _default_) {
        window.set_save_name(config.id);
        window.show_save_title();
    }

    if (!window.create(config.window_state))
        return false;

    config.update_window_state();

    log()->trace("{}: {}", _fullscreen_, config.window_state->fullscreen);

    set_window_icon(window);

    if (get_cmd_line()[{ "-wc", "--center" }])
        window.center();

    return true;
}

//-----------------------------------------------------------------------------
bool app::setup_device() {
    if (!device) {
        device = platform.create_device(config.physical_device);
        if (!device)
            return false;
    }

    auto physical_device = device->get_physical_device();

    auto device_name = trim_copy(physical_device->get_device_name());
    auto device_type = physical_device->get_device_type_string();
    auto device_driver_version = physical_device->get_driver_version();

    log()->info("device: {} ({}) - driver: {}",
                device_name, device_type,
                to_string(device_driver_version));

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
        config.update_window_state();

        if (!config_file.save())
            log()->error("save config file {}", config_file.get());

        config_file.clear();

        camera.destroy();

        destroy_imgui();

        block.destroy();

        destroy_target();

        window.destroy();

        fs.terminate();
    });

    add_run_once([&]() {
        return on_create ? on_create() : run_continue;
    });

    frame_counter = 0;
}

//-----------------------------------------------------------------------------
bool app::create_imgui() {
    if (config.imgui_font.file.empty()) {
        auto font_files = fs.enumerate_files(_font_path_);
        if (!font_files.empty())
            config.imgui_font.file = fmt::format("{}{}",
                                                 _font_path_, font_files.front());
    }

    setup_imgui_font(imgui_config, config.imgui_font);

    imgui_config.ini_file_dir = fs.get_pref_dir();

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

    if (auto imgui_active = -1;
        get_cmd_line()({ "-ig", "--imgui" }) >> imgui_active)
        imgui.set_active(imgui_active == 1);

    return true;
}

//-----------------------------------------------------------------------------
void app::destroy_imgui() {
    imgui.destroy();
    imgui_fonts->destroy();
}

//-----------------------------------------------------------------------------
bool app::create_target() {
    target = lava::create_target(&window, device,
                                 config.v_sync, config.surface);
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

/**
 * @brief Add app tooltips
 *
 * @param app    Target app
 */
void add_tooltips(app* app) {
    app->add_tooltip(_pause_, key::space, mod::control);
    app->add_tooltip(_imgui_, key::tab, mod::control);
    app->add_tooltip(_v_sync_, key::backspace, mod::alt);
    app->add_tooltip(_fullscreen_, key::enter, mod::alt);
    app->add_tooltip(_benchmark_, key::b, mod::control);
    app->add_tooltip(_screenshot_, key::p, mod::control);
    app->add_tooltip(_quit_, key::q, mod::control);
}

//-----------------------------------------------------------------------------
void app::handle_keys() {
    input.key.listeners.add([&](key_event::ref event) {
        if (imgui.capture_keyboard()) {
            camera.stop();
            return input_ignore;
        }

        if (config.handle_key_events) {
            if (event.mod == mod::control) {
                if (event.pressed(key::q))
                    return shut_down();

                if (event.pressed(key::tab)) {
                    imgui.toggle();
                    return input_done;
                }

                if (event.pressed(key::b)) {
                    frames.exit = false;
                    benchmark(*this, frames);
                    return input_done;
                }

                if (event.pressed(key::space)) {
                    run_time.paused = !run_time.paused;
                    return input_done;
                }

                if (event.pressed(key::p)) {
                    screenshot();
                    return input_done;
                }
            } else if (event.mod == mod::alt) {
                if (event.pressed(key::enter)) {
                    window.set_fullscreen(!window.fullscreen());
                    return input_done;
                }

                if (event.pressed(key::backspace)) {
                    toggle_v_sync = true;
                    return input_done;
                }
            }
        }

        if (camera.activated())
            return camera.handle(event);

        return input_ignore;
    });
}

//-----------------------------------------------------------------------------
void app::handle_input() {
    input.add(&imgui.get_input_callback());

    add_tooltips(this);

    handle_keys();

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

        if (window.switch_mode_request()
            || toggle_v_sync
            || target->reload_request()) {
            device->wait_for_idle();

            log()->info("- {}", _reload_);

            destroy_target();
            destroy_imgui();

            if (window.switch_mode_request()) {
                config.update_window_state();

                config.window_state->fullscreen = !config.window_state->fullscreen;

                log()->debug("{}: {}", _fullscreen_,
                             config.window_state->fullscreen ? _on_ : _off_);

                if (!window.switch_mode(config.window_state))
                    return false;

                config.update_window_state();

                set_window_icon(window);
            }

            if (toggle_v_sync) {
                config.v_sync = !config.v_sync;

                log()->debug("{}: {}", _v_sync_,
                             config.v_sync ? _on_ : _off_);

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
            if (run_time.fix_delta != ms(0))
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
string app::screenshot() {
    auto backbuffer_image = target->get_backbuffer(renderer.get_frame());
    if (!backbuffer_image)
        return {};

    auto image = grab_image(backbuffer_image);
    if (!image)
        return {};

    string screenshot_path = "screenshot/";
    fs.create_folder(screenshot_path);

    auto path = fs.get_pref_dir() + screenshot_path
                + get_current_time() + ".png";

    auto swizzle = !support_blit(device->get_vk_physical_device(),
                                 backbuffer_image->get_format())
                   && format_bgr(backbuffer_image->get_format());

    auto saved = write_image_png(device, image, path, swizzle);

    image->destroy();

    if (saved) {
        log()->info("screenshot: {}", path);
        return path;
    } else {
        log()->error("screenshot failed: {}", path);
        return {};
    }
}

//-----------------------------------------------------------------------------
void app::add_tooltip(string_ref name, key_t key, mod_t mod) {
    tooltips.emplace_back(name, key, mod);
}

//-----------------------------------------------------------------------------
void app::draw_about(bool separator,
                     bool fps,
                     bool spacing) const {
    if (separator)
        ImGui::Separator();

    if (spacing) {
        ImGui::Spacing();

        imgui_left_spacing(2);
    }

    ImGui::Text("%s %s", _liblava_, str(version_string()));

    if (config.handle_key_events && ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", str(to_string(tooltips)));
    }

    if (fps) {
        if (spacing)
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
}

} // namespace lava
