/**
 * @file         liblava/app/imgui.hpp
 * @brief        ImGui integration
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/block/graphics_pipeline.hpp>
#include <liblava/file.hpp>
#include <liblava/frame/input.hpp>
#include <liblava/resource/texture.hpp>

// fwd
struct GLFWwindow;
struct GLFWcursor;
struct ImDrawData;
struct ImGuiStyle;

namespace lava {

/// Default ImGui font size
constexpr const r32 default_imgui_font_size = 18.f;

/// ImGui state file
constexpr name _imgui_file_ = "imgui.ini";

/// Font icon name (default)
constexpr name _font_icon_ = "font_icon";

/**
 * @brief ImGui integration
 */
struct imgui {
    /**
     * @brief Construct a new ImGui
     */
    explicit imgui() = default;

    /**
     * @brief Construct a new ImGui
     *
     * @param window    Window for ImGui
     */
    explicit imgui(GLFWwindow* window) {
        setup(window);
    }

    /**
     * @brief Destroy the ImGui
     */
    ~imgui() {
        destroy();
    }

    /**
     * @brief ImGui icon font settings
     */
    struct icon_font {
        /// Icon font data
        data font_data;

        /// Icon range begin
        ui16 range_begin = 0;

        /// Icon range end
        ui16 range_end = 0;

        /// Default icon font size
        r32 size = default_imgui_font_size;
    };

    /**
     * @brief ImGui font settings
     */
    struct font {
        /// Const font reference
        using ref = font const&;

        /// Font file
        string file;

        /// Font size
        r32 size = 21.f;

        /// Font icon file
        string icon_file;

        /// Font icon size
        r32 icon_size = 21.f;

        /// Font range begin
        ui16 icon_range_begin = 0;

        /// Font range end
        ui16 icon_range_end = 0;
    };

    /**
     * @brief ImGui configuration
     */
    struct config {
        /// Font data
        data font_data;

        /// Font size
        r32 font_size = default_imgui_font_size;

        /// Font style
        std::shared_ptr<ImGuiStyle> style;

        /// Font icon settings
        icon_font icon;

        /// ImGui state file path
        std::filesystem::path ini_file_dir;
    };

    /**
     * @brief Set up ImGui with configuration
     *
     * @param window    Target window
     * @param config    Configuration
     */
    void setup(GLFWwindow* window, config config);

    /**
     * @brief Set up default ImGui
     *
     * @param window    Target window
     */
    void setup(GLFWwindow* window) {
        setup(window, config());
    }

    /**
     * @brief Create ImGui
     *
     * @param pipeline      Graphics pipeline
     * @param max_frames    Number of frames
     *
     * @return true         Create was successful
     * @return false        Create failed
     */
    bool create(graphics_pipeline::ptr pipeline, index max_frames);

    /**
     * @brief Create ImGui with device
     *
     * @param device        Vulkan device
     * @param max_frames    Number of frames
     *
     * @return true         Create was successful
     * @return false        Create failed
     */
    bool create(device_p device, index max_frames) {
        return create(make_graphics_pipeline(device), max_frames);
    }

    /**
     * @brief Create ImGui with device and render pass
     *
     * @param device        Vulkan device
     * @param max_frames    Number of frames
     * @param pass          Render pass
     *
     * @return true         Create was successful
     * @return false        Create failed
     */
    bool create(device_p device, index max_frames, VkRenderPass pass) {
        if (!create(device, max_frames))
            return false;

        return pipeline->create(pass);
    }

    /**
     * @brief Upload font texture
     *
     * @param texture    Texture to upload
     *
     * @return true      Upload was successful
     * @return false     Upload failed
     */
    bool upload_fonts(texture::ptr texture);

    /**
     * @brief Destroy the ImGui
     */
    void destroy();

    /**
     * @brief Check if ImGui is ready
     *
     * @return true     ImGui is ready
     * @return false    ImGui is not ready
     */
    bool ready() const {
        return initialized;
    }

    /**
     * @brief Get the pipeline
     *
     * @return graphics_pipeline::ptr    Graphics pipeline
     */
    graphics_pipeline::ptr get_pipeline() {
        return pipeline;
    }

    /// Draw function
    using draw_func = std::function<void()>;

    /// Function called on ImGui draw
    draw_func on_draw;

    /**
     * @brief Check if mouse capture is active
     *
     * @return true     Capture is active
     * @return false    Capture is not active
     */
    bool capture_mouse() const;

    /**
     * @brief Check if keyboard capture is active
     *
     * @return true     Capture is active
     * @return false    Capture is not active
     */
    bool capture_keyboard() const;

    /**
     * @brief Set ImGui active
     *
     * @param value    Active state
     */
    void set_active(bool value = true) {
        active = value;
    }

    /**
     * @brief Check if ImGui is activated
     *
     * @return true     ImGui is active
     * @return false    ImGui is not active
     */
    bool activated() const {
        return active;
    }

    /**
     * @brief Togge active state
     */
    void toggle() {
        active = !active;
    }

    /**
     * @brief Set the ini file
     *
     * @param dir    Path for file
     */
    void set_ini_file(std::filesystem::path dir);

    /**
     * @brief Get the ini file
     *
     * @return fs::path    Path of file
     */
    std::filesystem::path get_ini_file() const {
        return std::filesystem::path(ini_file);
    }

    /**
     * @brief Convert style to sRGB
     */
    void convert_style_to_srgb();

    /**
     * @brief Get the input callback
     *
     * @return input_callback const&    Input callback
     */
    input_callback const& get_input_callback() const {
        return callback;
    }

private:
    /**
     * @brief Handle key event
     *
     * @param key         Key
     * @param scancode    Scan code
     * @param action      Action
     * @param mods        Mods
     */
    void handle_key_event(i32 key, i32 scancode, i32 action, i32 mods);

    /**
     * @brief Handle mouse button event
     *
     * @param button    Button
     * @param action    Action
     * @param mods      Mods
     */
    void handle_mouse_button_event(i32 button, i32 action, i32 mods);

    /**
     * @brief Handle scroll event
     *
     * @param x_offset    X offset
     * @param y_offset    Y offset
     */
    void handle_scroll_event(r64 x_offset, r64 y_offset);

    /**
     * @brief Prepare draw lists
     *
     * @param draw_data    Draw data
     */
    void prepare_draw_lists(ImDrawData* draw_data);

    /**
     * @brief Render draw lists
     *
     * @param cmd_buf    Vulkan command buffer
     */
    void render_draw_lists(VkCommandBuffer cmd_buf);

    /**
     * @brief Invalidate device objects
     */
    void invalidate_device_objects();

    /**
     * @brief Update mouse position and buttons
     */
    void update_mouse_pos_and_buttons();

    /**
     * @brief Update mouse cursor
     */
    void update_mouse_cursor();

    /**
     * @brief Start new frame
     */
    void new_frame();

    /**
     * @brief Render ImGui
     *
     * @param cmd_buf    Vulkan command buffer
     */
    void render(VkCommandBuffer cmd_buf);

    /// Vulkan device
    device_p device = nullptr;

    // Initialized state
    bool initialized = false;

    /// Graphics pipeline
    graphics_pipeline::ptr pipeline;

    /// Pipeline layout
    pipeline_layout::ptr layout;

    /// Buffer memory alignment
    size_t buffer_memory_alignment = 256;

    /// Current frame index
    index frame = 0;

    /// Number of frames
    index max_frames = 4;

    /// Vertex buffer list
    buffer::list vertex_buffers;

    /// Index buffer list
    buffer::list index_buffers;

    /// Vulkan descriptor
    descriptor_ptr descriptor;

    /// Vulkan descriptor pool
    descriptor::pool::ptr descriptor_pool;

    /// Vulkan descriptor set
    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;

    /// Target window
    GLFWwindow* window = nullptr;

    /// Mouse just pressed state
    bool mouse_just_pressed[5] = { false, false, false, false, false };

    /// Current time
    r64 current_time = 0.0;

    /// Mouse cursors
    std::vector<GLFWcursor*> mouse_cursors;

    /// File for state (path)
    string ini_file;

    /// Active state
    bool active = true;

    /// Input callback
    input_callback callback;
};

/// Imgui type
using imgui_t = imgui;

/**
 * @brief Set up ImGui font
 *
 * @param config    ImGui configuration
 * @param font      ImGui font
 */
void setup_imgui_font(imgui::config& config,
                      imgui::font::ref font);

/**
 * @brief Set up imgui font icons
 *
 * @param font        Imgui font
 * @param filename    Font icon file name
 * @param min         Min range
 * @param max         Max range
 */
void setup_imgui_font_icons(imgui::font& font,
                            string filename,
                            ui16 min, ui16 max);

/**
 * @brief ImGui left spacing with top offset
 *
 * @param top    Top offset
 */
void imgui_left_spacing(ui32 top = 1);

} // namespace lava
