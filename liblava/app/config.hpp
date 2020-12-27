// file      : liblava/app/config.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/app/gui.hpp>
#include <liblava/frame/window.hpp>

namespace lava {

    struct app_config {
        bool save_window = true;
        bool handle_key_events = true;

        bool v_sync = false;
        index physical_device = 0;

        lava::font font;
    };

    window::state::optional load_window_state(name save_name);
    void save_window_file(window::ref window);

} // namespace lava
