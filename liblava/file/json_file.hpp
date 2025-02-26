/**
 * @file         liblava/file/json_file.hpp
 * @brief        Json file
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/core/types.hpp>
#include <liblava/file/json.hpp>

namespace lava {

/// Configuration json file
constexpr name _config_file_ = "config.json";

/**
 * @brief Json file
 */
struct json_file {
    /**
     * @brief Construct a new json file
     *
     * @param path    Name of file
     */
    explicit json_file(string_ref path = _config_file_);

    /**
     * @brief Json file callback
     */
    struct callback {
        /// List of callbacks
        using list = std::vector<callback*>;

        /// Load function
        using load_func = std::function<void(json_ref)>;

        /// Called on load
        load_func on_load;

        /// Save function
        using save_func = std::function<json()>;

        /// Called on save
        save_func on_save;
    };

    /**
     * @brief Add callback to json file
     *
     * @param callback    Callback to add
     */
    void add(callback* callback);

    /**
     * @brief Remove callback from json file
     *
     * @param callback    Callback to remove
     */
    void remove(callback* callback);

    /**
     * @brief Clear all callbacks
     */
    void clear() {
        callbacks.clear();
    }

    /**
     * @brief Set path of the json file
     *
     * @param value    Name of file
     */
    void set(string_ref value) {
        path = value;
    }

    /**
     * @brief Get path of the json file
     *
     * @return name    Name of file
     */
    string_ref get() const {
        return path;
    }

    /**
     * @brief Load the json file
     *
     * @return true     Load was successful
     * @return false    Load failed
     */
    bool load();

    /**
     * @brief Save the json file
     *
     * @return true     Save was successful
     * @return false    Save failed
     */
    bool save();

private:
    /// Name of file
    string path;

    /// List of callbacks
    callback::list callbacks;
};

} // namespace lava
