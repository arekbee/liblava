/**
 * @file         liblava/block/block.cpp
 * @brief        Command buffer model
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/block/block.hpp>

namespace lava {

//-----------------------------------------------------------------------------
bool command::create(device_p device,
                     index frame_count,
                     VkCommandPools cmd_pools) {
    buffers.resize(frame_count);

    for (auto i = 0u; i < frame_count; ++i) {
        VkCommandBufferAllocateInfo const allocate_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = cmd_pools.at(i),
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };
        if (failed(device->call().vkAllocateCommandBuffers(device->get(),
                                                           &allocate_info,
                                                           &buffers.at(i)))) {
            log()->error("create command buffers");
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
void command::destroy(device_p device,
                      VkCommandPools cmd_pools) {
    for (auto i = 0u; i < buffers.size(); ++i)
        device->call().vkFreeCommandBuffers(device->get(),
                                            cmd_pools.at(i),
                                            1,
                                            &buffers.at(i));
}

//-----------------------------------------------------------------------------
bool block::create(device_p d,
                   index frame_count,
                   index queue_family) {
    device = d;

    current_frame = 0;

    cmd_pools.resize(frame_count);

    for (auto i = 0u; i < frame_count; ++i) {
        VkCommandPoolCreateInfo const create_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = 0,
            .queueFamilyIndex = queue_family,
        };
        if (failed(device->call().vkCreateCommandPool(device->get(),
                                                      &create_info,
                                                      memory::alloc(),
                                                      &cmd_pools.at(i)))) {
            log()->error("create block command pool");
            return false;
        }
    }

    for (auto& [id, command] : commands)
        if (!command->create(device, frame_count, cmd_pools))
            return false;

    return true;
}

//-----------------------------------------------------------------------------
void block::destroy() {
    for (auto& [id, command] : commands)
        command->destroy(device, cmd_pools);

    for (auto i = 0u; i < cmd_pools.size(); ++i)
        device->call().vkDestroyCommandPool(device->get(),
                                            cmd_pools.at(i),
                                            memory::alloc());

    cmd_pools.clear();
    cmd_order.clear();
    commands.clear();
}

//-----------------------------------------------------------------------------
id block::add_cmd(command::process_func func, bool active) {
    auto cmd = make_command();
    cmd->on_process = func;
    cmd->active = active;

    if (device && !cmd_pools.empty())
        if (!cmd->create(device, get_frame_count(), cmd_pools))
            return undef_id;

    auto result = cmd->get_id();

    commands.emplace(result, cmd);
    cmd_order.push_back(commands.at(result).get());

    return result;
}

//-----------------------------------------------------------------------------
void block::remove_cmd(id::ref cmd) {
    if (!commands.count(cmd))
        return;

    auto command = commands.at(cmd);
    command->destroy(device, cmd_pools);

    remove(cmd_order, (command::cptr)(command.get()));

    commands.erase(cmd);
}

//-----------------------------------------------------------------------------
bool block::process(index frame) {
    current_frame = frame;

    if (failed(device->call().vkResetCommandPool(device->get(),
                                                 cmd_pools.at(frame),
                                                 0))) {
        log()->error("block reset command pool");
        return false;
    }

    for (auto& command : cmd_order) {
        if (!command->active)
            continue;

        auto& cmd_buf = command->buffers.at(frame);

        VkCommandBufferBeginInfo const begin_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        if (failed(device->call().vkBeginCommandBuffer(cmd_buf, &begin_info)))
            return false;

        if (command->on_process)
            command->on_process(cmd_buf);

        if (failed(device->call().vkEndCommandBuffer(cmd_buf)))
            return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
bool block::activated(id::ref command) {
    if (!commands.count(command))
        return false;

    return commands.at(command)->active;
}

//-----------------------------------------------------------------------------
bool block::set_active(id::ref command, bool active) {
    if (!commands.count(command))
        return false;

    commands.at(command)->active = active;
    return true;
}

} // namespace lava
