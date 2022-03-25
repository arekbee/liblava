<a href="https://git.io/liblava">
    <img align="left" src="res/doc/liblava_logo_200px.png" width="100">
</a>

<img src="res/doc/liblava.svg" style="margin:21px 15px 0px 0px">

**A modern and easy-to-use library for the Vulkan® API**

[![version](https://img.shields.io/badge/2022_preview-0.7.1-cf1020)](https://git.io/liblava) [![License](https://img.shields.io/github/license/liblava/liblava)](#license) [![CodeFactor](https://img.shields.io/codefactor/grade/github/liblava/liblava)](https://www.codefactor.io/repository/github/liblava/liblava) [![Discord](https://img.shields.io/discord/439508141722435595)](https://discord.lava-block.com) [![Donate](https://img.shields.io/badge/donate-PayPal-3b7bbf.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=JHWEUZ7ZTTV2G) [![Twitter Follow](https://img.shields.io/twitter/follow/liblava?style=flat&color=00acee)](https://twitter.com/liblava)

**lava** is a lean framework that provides **essentials** for **low-level graphics** - specially well suited for **prototyping**, **tooling**, **profiling** and **education**. It is written in **modern C++20** and strives for a **modular rolling release** as far as possible. We don't want to promise too much, but it runs really smoothly on **Windows** and **Linux**.

➜ **[Download](https://github.com/liblava/liblava/releases)** &nbsp; [Tutorial](#tutorial) &nbsp; [Guide](#guide) &nbsp; [Build](#build) &nbsp; [Install](#install) &nbsp; [Collaborate](#collaborate) &nbsp; [Third-Party](#third-party)

[![core](https://img.shields.io/badge/lava-core-blue.svg)](#lava-core) [![util](https://img.shields.io/badge/lava-util-blue.svg)](#lava-util) [![file](https://img.shields.io/badge/lava-file-orange.svg)](#lava-file) [![base](https://img.shields.io/badge/lava-base-orange.svg)](#lava-base) [![resource](https://img.shields.io/badge/lava-resource-orange.svg)](#lava-resource) [![asset](https://img.shields.io/badge/lava-asset-red.svg)](#lava-asset) [![frame](https://img.shields.io/badge/lava-frame-red.svg)](#lava-frame) [![block](https://img.shields.io/badge/lava-block-red.svg)](#lava-block) [![app](https://img.shields.io/badge/lava-app-brightgreen.svg)](#lava-app) [![engine](https://img.shields.io/badge/lava-engine-brightgreen.svg)](#lava-engine)
# Demos

| lava light | lava spawn | 
|:---------|---------:|
| <a href="liblava-demo/light.cpp">![light](res/light/screenshot.png)</a> | <a href="liblava-demo/spawn.cpp">![spawn](res/spawn/screenshot.png)</a> | 
| deferred shading + offscreen rendering | uniform buffer + camera | 

<br />

| lava lamp | lava shapes | 
|:---------|---------:|
| <a href="liblava-demo/lamp.cpp">![lamp](res/lamp/screenshot.png)</a> | <a href="liblava-demo/shapes.cpp">![shapes](res/shapes/screenshot.png)</a> | 
| push constants ➜ shader | generating primitives | 

<br />

| lava generics | lava spawn | 
|:---------|---------:|
| <a href="liblava-demo/generics.cpp">![generics](res/generics/screenshot.png)</a> | <a href="liblava-demo/triangle.cpp">![triangle](res/triangle/screenshot.png)</a> | 
| float, double & int meshes | unique classic mesh | 

<br />

# Projects

| lava raytracing cubes | 
|:---------|
| <a href="https://github.com/pezcode/lava-rt/blob/main/demo/cubes.cpp">![generics](https://raw.githubusercontent.com/pezcode/lava-rt/main/demo/res/cubes/screenshot.png)</a> | 
| raytraced reflecting cubes ➜ [pezcode/lava-rt](https://github.com/pezcode/lava-rt) |

<br />

# In a nutshell

<a href="https://git.io/liblava">
    <img align="right" src="res/doc/liblava_logo_200px.png" width="170"  style="margin:-30px 100px 0px 0px">
</a>

* written in **modern C++** with latest **Vulkan support**
* **run loop** abstraction for **window** and **input** handling
* plain **renderer** and  **command buffer** model
* **texture** and **mesh** loading from **virtual file system**
* **camera**, **imgui**, **logger** and more...

<br />

# Tutorial

<a href="https://www.vulkan.org/" target="_blank"><img align="right" hspace="20" src="res/doc/Vulkan_170px_Dec16.png" width="300" style="margin:0px 50px 0px 0px"></a>

**Hello World** in **Vulkan**? &nbsp; ➜  Let's go!

*a simple app that renders a colored window*

<br />

All we need is a `window` + `device` and `renderer`

<br />

**Vulkan** is a low-level, verbose graphics API and such a program can take several hundred lines of code.

The good news is that **liblava** can help you.

```c++
#include <liblava/lava.hpp>

using namespace lava;
```
<br />

Here are a few **examples** to get to know `lava`:

<br />

## 1. frame init

```c++
int main(int argc, char* argv[]) {

    lava::frame frame( {argc, argv} );
    
    return frame.ready() ? 0 : error::not_ready;
}
```

This is how to initialize `lava frame` with command line arguments.

<br />

## 2. run loop

```c++
lava::frame frame(argh);
if (!frame.ready())
    return error::not_ready;

ui32 count = 0;

frame.add_run([&](id::ref run) {
    sleep(one_second);
    count++;

    log()->debug("{} - running {} sec", count, frame.get_running_time_sec());

    if (count == 3)
        return frame.shut_down();

    return run_continue;
});

return frame.run();
```

The last line performs a loop with the **run** we added before - If *count* reaches 3 that **loop** will exit.

<br />

## 3. window input

Here is another example that shows how to create a `lava window` and handle `lava input`:

```c++
lava::frame frame(argh);
if (!frame.ready())
    return error::not_ready;

lava::window window;
if (!window.create())
    return error::create_failed;

lava::input input;
window.assign(&input);

input.key.listeners.add([&](key_event::ref event) {
    if (event.pressed(key::escape))
        return frame.shut_down();
    
    return input_ignore;
});

frame.add_run([&](id::ref run) {
    input.handle_events();

    if (window.close_request())
        return frame.shut_down();

    return run_continue;
});

return frame.run();
```

<br />

Straightforward ➜ With this knowledge in hand let's write our **Hello World**...

## 4. clear color

```c++
lava::frame frame(argh);
if (!frame.ready())
    return error::not_ready;

lava::window window;
if (!window.create())
    return error::create_failed;

lava::input input;
window.assign(&input);

input.key.listeners.add([&](key_event::ref event) {
    if (event.pressed(key::escape))
        return frame.shut_down();

    return input_ignore;
});

lava::device_ptr device = frame.create_device();
if (!device)
    return error::create_failed;

lava::render_target::ptr render_target = create_target(&window, device);
if (!render_target)
    return error::create_failed;

lava::renderer renderer;
if (!renderer.create(render_target->get_swapchain()))
    return error::create_failed;

ui32 frame_count = render_target->get_frame_count();

VkCommandPool cmd_pool;
VkCommandBuffers cmd_bufs(frame_count);

auto build_cmd_bufs = [&]() {
    if (!device->vkCreateCommandPool(device->graphics_queue().family, &cmd_pool))
        return build_failed;

    if (!device->vkAllocateCommandBuffers(cmd_pool, frame_count, cmd_bufs.data()))
        return build_failed;

    VkCommandBufferBeginInfo const begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    };

    VkClearColorValue const clear_color = { random(1.f), random(1.f), random(1.f), 0.f };

    VkImageSubresourceRange const image_range{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .levelCount = 1,
        .layerCount = 1,
    };

    for (auto i = 0u; i < frame_count; ++i) {
        VkCommandBuffer cmd_buf = cmd_bufs[i];
        VkImage frame_image = render_target->get_image(i);

        if (failed(device->call().vkBeginCommandBuffer(cmd_buf, &begin_info)))
            return build_failed;

        insert_image_memory_barrier(device, cmd_buf, frame_image,
                                    VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                    image_range);

        device->call().vkCmdClearColorImage(cmd_buf, frame_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                            &clear_color, 1, &image_range);

        insert_image_memory_barrier(device, cmd_buf, frame_image,
                                    VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                    image_range);

        if (failed(device->call().vkEndCommandBuffer(cmd_buf)))
            return build_failed;
    }

    return build_done;
};

auto clean_cmd_bufs = [&]() {
    device->vkFreeCommandBuffers(cmd_pool, frame_count, cmd_bufs.data());
    device->vkDestroyCommandPool(cmd_pool);
};

if (!build_cmd_bufs())
    return error::create_failed;

render_target->on_swapchain_start = build_cmd_bufs;
render_target->on_swapchain_stop = clean_cmd_bufs;

frame.add_run([&](id::ref run) {
    input.handle_events();

    if (window.close_request())
        return frame.shut_down();

    if (window.resize_request())
        return window.handle_resize();

    optional_index current_frame = renderer.begin_frame();
    if (!current_frame.has_value())
        return run_continue;

    return renderer.end_frame({ cmd_bufs[*current_frame] });
});

frame.add_run_end([&]() {
    clean_cmd_bufs();

    renderer.destroy();
    render_target->destroy();
});

return frame.run();
```

Welcome on **Planet Vulkan** - That's a lot to display a colored window!

<br />
 
Take a closer look at the `build_cmd_bufs` function:

* We **create** a **command pool** and **command buffers** for each **frame** of the **render target**
* And **set** each **command buffer** to **clear the frame image** with some **random color**

<br />

`clean_cmd_bufs` **frees all buffers** and **destroys** the **command pool**.

In case of **swap chain** restoration we simply **recreate command buffers** with a **new random color** - This happens for example on `window` resize.

The flag *VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT* specifies the usage of **command buffers** in such a way that it can *no longer* be changed. And therefore it is a very *static* example.

---

**Vulkan** supports a more *dynamic* and common usage by **resetting a command pool** before **recording new commands**.

<br />

Ok, it's time for ➜ `lava block`

## 5. color block

```c++
lava::block block;

if (!block.create(device, frame_count, device->graphics_queue().family))
    return error::create_failed;

block.add_command([&](VkCommandBuffer cmd_buf) {
    VkClearColorValue const clear_color = { random(1.f), random(1.f), random(1.f), 0.f };

    VkImageSubresourceRange const image_range{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .levelCount = 1,
        .layerCount = 1,
    };

    VkImage frame_image = render_target->get_image(block.get_current_frame());

    insert_image_memory_barrier(device, cmd_buf, frame_image,
                                VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                image_range);

    device->call().vkCmdClearColorImage(cmd_buf, frame_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                        &clear_color, 1, &image_range);

    insert_image_memory_barrier(device, cmd_buf, frame_image,
                                VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                image_range);
});
```

This is much more simpler than before!

<br />

We create a `lava block` with a `lava command` that only **clears** the **current frame image**.

<br />

All we need to do now is to process the `lava block` in the run loop:

```c++
if (!block.process(*current_frame))
    return run_abort;

return renderer.end_frame(block.get_buffers());
```

And call the `renderer` with our recorded **command buffers**.

<br />

**NOTE** &nbsp; Don't forget to *clean* it up when the **run ends**:

```c++
block.destroy();
```

<br />

**New to Vulkan?** Take a look at this [Vulkan Guide](https://github.com/KhronosGroup/Vulkan-Guide)

Check [Awesome Vulkan ecosystem](http://www.vinjn.com/awesome-vulkan/) for tutorials, samples and books

<br />

## 8. imgui demo

Out of blocks, `lava app` supports [Dear ImGui](https://github.com/ocornut/imgui) for **tooling** and **easy prototyping**.

```c++
int main(int argc, char* argv[]) {

    lava::app app("demo", { argc, argv });
    if (!app.setup())
        return error::not_ready;

    app.imgui.on_draw = []() {

        ImGui::ShowDemoWindow();
    };

    return app.run();
}
```

<br />

**What's next?** ➜ Check some [demo](liblava-demo) or use the [template](#template) to try it out!


<br />

# Guide

[Lifetime of an Object](#lifetime-of-an-object) &nbsp; &nbsp; [Making meshes](#making-meshes) &nbsp; &nbsp; [Keyboard shortcuts](#keyboard-shortcuts) &nbsp; &nbsp; [Command-Line Arguments](#command-line-arguments)

---

<br />

## Lifetime of an Object

Before you create new objects or use existing ones, you should get familiar with the lifetime of objects.

It is basically possible to create all objects in **liblava** on the stack or on the heap.

But be careful. You have to take care of the lifetime yourself.

<br />

### make &nbsp; ➜ &nbsp; create &nbsp; ➜ &nbsp; destroy

This is the general pattern that is used in this library:

1. **make** &nbsp; Use constructor or factory method *(static function to get a shared pointer)*
2. **create** &nbsp; Build the respective object
3. **destroy** &nbsp; Discard it after your use

The destructor calls the **destroy** method if it was *not* called before.

<br />

### For example: buffer object

```c++
void use_buffer_on_stack() {

    buffer buf; // make

    auto created = buf.create(device, data, size, usage);
    if (created) {
        // ...

        buf.destroy();
    }
}
```

Or look at this method where it is returned as a *shared pointer*:

```c++
buffer::ptr use_buffer_on_heap() {

    auto buf = make_buffer();

    if (buf->create(device, data, size, usage))
        return buf;

    return nullptr;
}
```

<br />

## Making meshes

**liblava** provides a `mesh` struct that contains a list of vertices and, optionally, a list of indices.

By default, vertices in a `mesh` are of type `vertex`, which has the following **layout**:

```c++
struct vertex {
    v3 position;
    v4 color;
    v2 uv;
    v3 normal;
}
```

It is made like this:

```c++
mesh::ptr my_mesh = make_mesh();

my_mesh->add_data( /* Pass in a lava::mesh_data object */ );
my_mesh->create(device);
```

<br />

**liblava** provides a `create_mesh()` function to simplify the **creation of primitives**.

It takes a `mesh_type` argument to specify what kind of primitive to build - Its values are:

`none` &nbsp; `cube` &nbsp; `triangle` &nbsp; `quad` &nbsp; `hexagon`

The function is called in this way:

```c++
mesh::ptr cube;
cube = create_mesh(device, mesh_type::cube);
```

<br />

**Meshes** are templated, and can represent **any vertex struct** definition.

```c++
struct int_vertex {
    std::array<i32, 3> position;
    v4 color;
};
mesh_template<int_vertex>::ptr int_triangle;
```

`create_mesh()` can generate primitives for arbitrary vertex structs too,
provided that the struct contains an array or vector member named `position`:

```c++
int_triangle = create_mesh<int_vertex>(device, mesh_type::triangle);
```

<br />

`create_mesh()` may also initialize **Color**, **Normal**, and **UV** data automatically.

However, it will only initialize these if there are corresponding `color`,
`normal`, and/or `uv` fields defined in the vertex struct.

By default, all data that can be initialized will be, but if generating any 
of this data is not desired, the fields can be individually disabled by template arguments in this order:

- **Color**
- **Normal**
- **UV**

It is done in this way:

```c++
struct custom_vertex {
    v3 position;
    v3 color;
    v3 normal;
    v2 uv;
};
mesh_template<custom_vertex>::ptr triangle;

// Generate three vertices with positions and uvs, but not colors or normals
triangle = create_mesh<custom_vertex, false, false, true>
                      (device, mesh_type::triangle);
```

Cubes generated this way have a special case. If they are initialized with normal data, they will be represented by 24 vertices. Otherwise, only 8 vertices will be initialized.

<br />

## Keyboard shortcuts

`lava app` defines some **shortcuts** for practical and common **actions**:

<br />

| shortcut             |               action |   default   | config.json |  window.json |
| :------------------- | -------------------: | :---------: | ----------: | -----------: |
| *alt + enter*        |       **fullscreen** |     off     |             | "fullscreen" |
| *alt + backspace*    |           **v-sync** |     off     |    "v-sync" |              |
| *control + tab*      |              **gui** |     on      |     "imgui" |              |
| *control + space*    |            **pause** |     off     |    "paused" |              |
| *control + b*        |    **run benchmark** |             |             |              |
| *control + q*        | **quit application** |             |             |              |

<br />

**NOTE** &nbsp; You can disable these actions by simply turning them off:

`app.config.handle_key_events = false;`

<br />

## Command-Line Arguments

### app

```
--clean, -c
```

* **clean** preferences folder

<br />

```
--v_sync={0|1}, -vs={0|1}
```

* **0** &nbsp; vertical sync off
* **1** &nbsp; vertical sync on

<br />

```
--physical_device={n}, -pd={n}
```

* **n** &nbsp; physical device index &nbsp; *default: n = 0*

<br />

```
--identification={str}, -id={str}
```

* **str** &nbsp; window save name &nbsp; *supports punctuation marks*

<br />

#### Benchmark

**liblava** can write frame times into a `json` file to analyze them further in automated workflows like benchmarks:

```json
{
  "benchmark": {
    "avg": 16.02839111337229,
    "count": 622,
    "max": 45,
    "min": 12,
    "offset": 5000,
    "time": 10000
  },
  "frames": [
    12,
    14,
    16,
    16
  ],
  "timestamps": [
    5,
    17,
    31,
    47,
    63
  ]
}
```

<br />

```
--frames, -fs
```

* **activate** benchmark mode

<br />

```
--frames_time={n}, -fst={n}
```

* **n** &nbsp; benchmark duration in milliseconds &nbsp; *default: n = 10000 ms*

<br />

```
--frames_offset={n}, -fso={n}
```

* **n** &nbsp; warm up time in milliseconds &nbsp; *default: n = 5000 ms*

<br />

```
--frames_file={str}, -fsf={str}
```

* **str** &nbsp; output file &nbsp; *default: str = frames.json*

<br />

```
--frames_path={str}, -fsp={str}
```

* **str** &nbsp; output path &nbsp; *default: preferences folder*

<br />

```
--frames_exit={0|1}, -fsx={0|1}
```

* **0** &nbsp; keep running after benchmark
* **1** &nbsp; close app after benchmark &nbsp; *default*

<br />

```
--frames_buffer={n}, -fsb={n}
```

* **n** &nbsp; pre-allocated buffer size for results &nbsp; *default: n = 100000*

<br />

### frame

```
--debug, -d
```

* **enable** validation layer &nbsp; *[VK_LAYER_KHRONOS_validation](https://github.com/KhronosGroup/Vulkan-ValidationLayers/blob/master/docs/khronos_validation_layer.md)*

<br />

```
--utils, -u
```

* **enable** debug utils extension &nbsp; *[VK_EXT_debug_utils](https://www.lunarg.com/wp-content/uploads/2018/05/Vulkan-Debug-Utils_05_18_v1.pdf)*

<br />

```
--renderdoc, -r
```

* **enable** [RenderDoc](https://renderdoc.org/) capture layer &nbsp; *VK_LAYER_RENDERDOC_Capture*

<br />

```
--verbose, -v
```

*  **enable** verbose logging

<br />

```
--log={0|...|6}, -l={0|...|6}
```

* level **0** &nbsp; trace
* level **1** &nbsp; debug
* level **2** &nbsp; info
* level **3** &nbsp; warn
* level **4** &nbsp; error
* level **5** &nbsp; critical
* level **6** &nbsp; *logging off*

<br />

# Modules

## lava [engine](liblava/engine)

[![driver](https://img.shields.io/badge/lava-driver-brightgreen.svg)](liblava/engine/driver.hpp)

## lava [app](liblava/app)

[![app](https://img.shields.io/badge/lava-app-brightgreen.svg)](liblava/app/app.hpp) [![camera](https://img.shields.io/badge/lava-camera-brightgreen.svg)](liblava/app/camera.hpp) [![forward_shading](https://img.shields.io/badge/lava-forward_shading-brightgreen.svg)](liblava/app/forward_shading.hpp)

[![benchmark](https://img.shields.io/badge/lava-benchmark-brightgreen.svg)](liblava/app/benchmark.hpp) [![config](https://img.shields.io/badge/lava-config-brightgreen.svg)](liblava/app/config.hpp) [![imgui](https://img.shields.io/badge/lava-imgui-brightgreen.svg)](liblava/app/imgui.hpp)

<br />

## lava [block](liblava/block)

[![attachment](https://img.shields.io/badge/lava-attachment-red.svg)](liblava/block/attachment.hpp) [![block](https://img.shields.io/badge/lava-block-red.svg)](liblava/block/block.hpp) [![descriptor](https://img.shields.io/badge/lava-descriptor-red.svg)](liblava/block/descriptor.hpp) [![render_pass](https://img.shields.io/badge/lava-render_pass-red.svg)](liblava/block/render_pass.hpp) [![subpass](https://img.shields.io/badge/lava-subpass-red.svg)](liblava/block/subpass.hpp)

[![compute_pipeline](https://img.shields.io/badge/lava-compute_pipeline-red.svg)](liblava/block/compute_pipeline.hpp) [![graphics_pipeline](https://img.shields.io/badge/lava-graphics_pipeline-red.svg)](liblava/block/graphics_pipeline.hpp) [![pipeline](https://img.shields.io/badge/lava-pipeline-red.svg)](liblava/block/pipeline.hpp) [![pipeline_layout](https://img.shields.io/badge/lava-pipeline_layout-red.svg)](liblava/block/pipeline_layout.hpp)

## lava [frame](liblava/frame)

[![frame](https://img.shields.io/badge/lava-frame-red.svg)](liblava/frame/frame.hpp) [![gamepad](https://img.shields.io/badge/lava-gamepad-red.svg)](liblava/frame/gamepad.hpp) [![input](https://img.shields.io/badge/lava-input-red.svg)](liblava/frame/input.hpp) [![window](https://img.shields.io/badge/lava-window-red.svg)](liblava/frame/window.hpp)

[![render_target](https://img.shields.io/badge/lava-render_target-red.svg)](liblava/frame/render_target.hpp) [![renderer](https://img.shields.io/badge/lava-renderer-red.svg)](liblava/frame/renderer.hpp) [![swapchain](https://img.shields.io/badge/lava-swapchain-red.svg)](liblava/frame/swapchain.hpp)

## lava [asset](liblava/asset)

[![image_data](https://img.shields.io/badge/lava-image_data-red.svg)](liblava/asset/image_data.hpp) [![mesh_loader](https://img.shields.io/badge/lava-mesh_loader-red.svg)](liblava/asset/mesh_loader.hpp) [![texture_loader](https://img.shields.io/badge/lava-texture_loader-red.svg)](liblava/asset/texture_loader.hpp)

<br />

## lava [resource](liblava/resource)

[![buffer](https://img.shields.io/badge/lava-buffer-orange.svg)](liblava/resource/buffer.hpp) [![mesh](https://img.shields.io/badge/lava-mesh-orange.svg)](liblava/resource/mesh.hpp) [![primitive](https://img.shields.io/badge/lava-primitive-orange.svg)](liblava/resource/primitive.hpp)

[![format](https://img.shields.io/badge/lava-format-orange.svg)](liblava/resource/format.hpp) [![image](https://img.shields.io/badge/lava-image-orange.svg)](liblava/resource/image.hpp) [![texture](https://img.shields.io/badge/lava-texture-orange.svg)](liblava/resource/texture.hpp)

## lava [base](liblava/base)

[![base](https://img.shields.io/badge/lava-base-orange.svg)](liblava/base/base.hpp) [![instance](https://img.shields.io/badge/lava-instance-orange.svg)](liblava/base/instance.hpp) [![profile](https://img.shields.io/badge/lava-profile-orange.svg)](liblava/base/profile.hpp) [![memory](https://img.shields.io/badge/lava-memory-orange.svg)](liblava/base/memory.hpp)

[![device](https://img.shields.io/badge/lava-device-orange.svg)](liblava/base/device.hpp) [![physical_device](https://img.shields.io/badge/lava-physical_device-orange.svg)](liblava/base/physical_device.hpp) [![queue](https://img.shields.io/badge/lava-queue-orange.svg)](liblava/base/queue.hpp)

## lava [file](liblava/file)

[![file](https://img.shields.io/badge/lava-file-orange.svg)](liblava/file/file.hpp) [![file_system](https://img.shields.io/badge/lava-file_system-orange.svg)](liblava/file/file_system.hpp) [![file_utils](https://img.shields.io/badge/lava-file_utils-orange.svg)](liblava/file/file_utils.hpp) [![json_file](https://img.shields.io/badge/lava-json_file-orange.svg)](liblava/file/json_file.hpp)

<br />

## lava [util](liblava/util)

[![log](https://img.shields.io/badge/lava-log-blue.svg)](liblava/util/log.hpp) [![random](https://img.shields.io/badge/lava-random-blue.svg)](liblava/util/random.hpp) [![telegram](https://img.shields.io/badge/lava-telegram-blue.svg)](liblava/util/telegram.hpp) [![thread](https://img.shields.io/badge/lava-thread-blue.svg)](liblava/util/thread.hpp) [![utility](https://img.shields.io/badge/lava-utility-blue.svg)](liblava/util/utility.hpp)

## lava [core](liblava/core)

[![data](https://img.shields.io/badge/lava-data-blue.svg)](liblava/core/data.hpp) [![hex](https://img.shields.io/badge/lava-hex-blue.svg)](liblava/core/hex.hpp) [![id](https://img.shields.io/badge/lava-id-blue.svg)](liblava/core/id.hpp) [![math](https://img.shields.io/badge/lava-math-blue.svg)](liblava/core/math.hpp) [![time](https://img.shields.io/badge/lava-time-blue.svg)](liblava/core/time.hpp) [![types](https://img.shields.io/badge/lava-types-blue.svg)](liblava/core/types.hpp) [![version](https://img.shields.io/badge/lava-version-blue.svg)](liblava/core/version.hpp)

<br />

---

| [![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=JHWEUZ7ZTTV2G) | [![GitHub Stars](https://img.shields.io/github/stars/liblava/liblava?style=social)](https://github.com/liblava/liblava/stargazers) &nbsp; [![Twitter URL](https://img.shields.io/twitter/follow/liblava?style=social)](https://twitter.com/liblava) |
|:---------|---------:|
| **liblava** needs help for **maintenance**, **support** and further **development**  |  even *without* spending anything, **we would be very happy about a star or follow**  | 

<br />

<center>:heart: &nbsp; Thank you very much!</center>

---

<br />

# Tests

Run the `lava` executable to test our [Tutorial](#tutorial) examples ➜ so called [stages](liblava/engine/tutorial.cpp).

<br />

## List all stages

```bash
lava -ls

lava --stages
```

1. [frame init](#1-frame-init)
2. [run loop](#2-run-loop)
3. [window input](#3-window-input)
4. [clear color](#4-clear-color)
5. [color block](#5-color-block)
6. forward shading
7. gamepad
8. [imgui demo](#8-imgui-demo)

<br />

### Run *window input* example

```bash
lava -s=3

lava --stage=3
```


**NOTE** &nbsp; If you run `lava` without arguments - the *last* stage will be started.

<br />

## Unit testing

In addition run `lava-test` to check some **unit tests** with [Catch2](https://github.com/catchorg/Catch2).

<br />

If you need any help, please feel free to ask us on ➜ [Discord](https://discord.lava-block.com)

<br />

# Build

[![CMake (Linux, Windows)](https://github.com/liblava/liblava/actions/workflows/cmake.yml/badge.svg)](https://github.com/liblava/liblava/actions/workflows/cmake.yml)
## Requirements

* **C++20** compatible compiler
* CMake **3.22+**
* [Vulkan SDK](https://vulkan.lunarg.com)

<br />

```bash
git clone https://github.com/liblava/liblava.git
cd liblava

git submodule update --init --recursive

mkdir build
cd build

cmake ..
make
```

<br />

# Install

You can use **liblava** as a *git submodule* in your project:

```bash
git submodule add https://github.com/liblava/liblava.git

git submodule update --init --recursive
```

Add this to your `CMakeLists.txt`

```cmake
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/liblava ${CMAKE_CURRENT_BINARY_DIR}/liblava)
target_link_libraries(${PROJECT_NAME} PRIVATE lava::app)
```

<br />

## Package setup

Alternatively ➜ compile and install a specific version for multiple projects:

```bash
mkdir build
cd build

cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=../lava-install ..
cmake --build . --config Release --target install
```

First find the package in your `CMakeLists.txt`

```cmake
find_package(lava 0.7.1 REQUIRED)

...

add_executable(test main.cpp)
target_link_libraries(test lava::app)
```

And then build your project with install path ➜ *lava_DIR*

```bash
mkdir build
cd build

cmake -D lava_DIR=path/to/lava-install/lib/cmake/lava ..
cmake --build .
```

<br />

## Installing and using Vcpkg

[Vcpkg](https://github.com/microsoft/vcpkg) integration with 2 options ➜ use this [registry and port](https://github.com/liblava/vcpkg-liblava).

<br />

## Conan Package Manager

If you are familiar with [Conan](https://conan.io/) ➜ build this [package recipe](https://github.com/liblava/conan-liblava).

<br />

# Template

Put your code in the [src](src) folder and begin to code in [main.cpp](src/main.cpp).

<br />

**NOTE** &nbsp; You can change the project name in **CMake** ➜ `LIBLAVA_TEMPLATE_NAME` 

```bash
cmake -DLIBLAVA_TEMPLATE_NAME="My-Project" ..
```

<br />

# Reference

To generate the documentation with [Doxygen](https://www.doxygen.nl) run:

```bash
doxygen doxygen.conf
```

Here you can find the **latest** ➜ [doc.lava-block.com](https://doc.lava-block.com)

<br />

# Collaborate

Use the [issue tracker](https://github.com/liblava/liblava/issues) to report any bug or compatibility issue.

:heart: &nbsp; Thanks to all **[contributors](https://github.com/liblava/liblava/graphs/contributors)** making **liblava** flow...

<br />

## Support

If you want to contribute - we suggest the following:

1. Fork the [official repository](https://github.com/liblava/liblava/fork)
2. Apply your changes to your fork
3. Submit a [pull request](https://github.com/liblava/liblava/pulls) describing the changes you have made

<br />

# Third-Party

* [argh](https://github.com/adishavit/argh) &nbsp; **Argh! A minimalist argument handler** &nbsp; *3-clause BSD*

  [![frame](https://img.shields.io/badge/lava-frame-red.svg)](liblava/frame/frame.hpp)

* [Catch2](https://github.com/catchorg/Catch2) &nbsp; **A modern, C++-native, header-only, test framework for unit-tests, TDD and BDD** &nbsp; *BSL 1.0*

* [glfw](https://github.com/glfw/glfw) &nbsp; **A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input** &nbsp; *zlib*

  [![frame](https://img.shields.io/badge/lava-frame-red.svg)](liblava/frame/frame.cpp) [![gamepad](https://img.shields.io/badge/lava-gamepad-red.svg)](liblava/frame/gamepad.cpp) [![input](https://img.shields.io/badge/lava-input-red.svg)](liblava/frame/input.cpp) [![window](https://img.shields.io/badge/lava-window-red.svg)](liblava/frame/window.cpp)

* [gli](https://github.com/g-truc/gli) &nbsp; **OpenGL Image (GLI)** &nbsp; *MIT*

  [![texture_loader](https://img.shields.io/badge/lava-texture_loader-red.svg)](liblava/asset/texture_loader.cpp)

* [glm](https://github.com/g-truc/glm) &nbsp; **OpenGL Mathematics (GLM)** &nbsp; *MIT*

  [![math](https://img.shields.io/badge/lava-math-blue.svg)](liblava/core/math.hpp)

* [IconFontCppHeaders](https://github.com/juliettef/IconFontCppHeaders) &nbsp; **C, C++ headers and C# classes for icon fonts** &nbsp; *zlib*

  [![imgui](https://img.shields.io/badge/lava-imgui-brightgreen.svg)](liblava/app/imgui.cpp)

* [imgui](https://github.com/ocornut/imgui) &nbsp; **Dear ImGui - Bloat-free Graphical User interface for C++ with minimal dependencies** &nbsp; *MIT*

  [![imgui](https://img.shields.io/badge/lava-imgui-brightgreen.svg)](liblava/app/imgui.cpp)

* [json](https://github.com/nlohmann/json) &nbsp; **JSON for Modern C++** &nbsp; *MIT*

  [![json_file](https://img.shields.io/badge/lava-json_file-orange.svg)](liblava/file/json_file.hpp)

* [physfs](https://github.com/icculus/physfs) &nbsp; **A portable, flexible file i/o abstraction** &nbsp; *zlib*

  [![file](https://img.shields.io/badge/lava-file-orange.svg)](liblava/file/file.cpp) [![file_system](https://img.shields.io/badge/lava-file_system-orange.svg)](liblava/file/file_system.cpp)

* [spdlog](https://github.com/gabime/spdlog) &nbsp; **Fast C++ logging library** &nbsp; *MIT*

  [![log](https://img.shields.io/badge/lava-log-blue.svg)](liblava/util/log.hpp)

* [stb](https://github.com/nothings/stb) &nbsp; **Single-file public domain libraries for C/C++** &nbsp; *MIT*

  [![image_data](https://img.shields.io/badge/lava-image_data-red.svg)](liblava/asset/image_data.cpp) [![texture_loader](https://img.shields.io/badge/lava-texture_loader-red.svg)](liblava/asset/texture_loader.cpp)

* [tinyobjloader](https://github.com/syoyo/tinyobjloader) &nbsp; **Tiny but powerful single file wavefront obj loader** &nbsp; *MIT*

  [![mesh_loader](https://img.shields.io/badge/lava-mesh_loader-red.svg)](liblava/asset/mesh_loader.hpp)

* [volk](https://github.com/zeux/volk) &nbsp; **Meta loader for Vulkan API** &nbsp; *MIT*

  [![base](https://img.shields.io/badge/lava-base-orange.svg)](liblava/base/base.hpp)

* [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers) &nbsp; **Vulkan Header files and API registry** &nbsp; *Apache 2.0*

  [![base](https://img.shields.io/badge/lava-base-orange.svg)](liblava/base/base.hpp) [![window](https://img.shields.io/badge/lava-window-red.svg)](liblava/frame/window.hpp)

* [Vulkan-Profiles](https://github.com/KhronosGroup/Vulkan-Profiles) &nbsp; **Vulkan Profiles Tools** &nbsp; *Apache 2.0*

  [![profile](https://img.shields.io/badge/lava-profile-orange.svg)](liblava/base/profile.hpp)

* [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) &nbsp; **Easy to integrate Vulkan memory allocation library** &nbsp; *MIT*

  [![memory](https://img.shields.io/badge/lava-memory-orange.svg)](liblava/base/memory.hpp)

<br />

# Demo

You can find the demonstration projects in the `liblava-demo` folder.

## Roboto

[GitHub](https://github.com/google/fonts/tree/main/apache/roboto) &nbsp; [Website](https://fonts.google.com/specimen/Roboto) &nbsp; *Apache License, Version 2.0*

* [Roboto-Regular.ttf](res/font/Roboto-Regular.ttf)

## Font Awesome

[GitHub](https://github.com/FortAwesome/Font-Awesome) &nbsp; [Website](https://fontawesome.com) &nbsp; *Font Awesome Free License*

* [fa-solid-900.ttf](res/font/icon/fa-solid-900.ttf)

## Barbarella

[Website](https://www.shadertoy.com/view/XdfGDr) &nbsp; *Shader by Weyland Yutani*

* [lamp.frag](res/lamp/lamp.frag)

## Spawn Model

[Website](https://opengameart.org/content/lava-spawn) &nbsp; *CC BY-SA 3.0*

* [lava-spawn-game.mtl](res/spawn/lava-spawn-game.mtl)
* [lava-spawn-game.obj](res/spawn/lava-spawn-game.obj)

<br />

# License

<a href="https://opensource.org" target="_blank"><img align="right" width="90" src="res/doc/OSI-Approved-License-100x137.png" style="margin:0px 0px 0px 80px"></a>

**liblava** is licensed under [MIT License](LICENSE) which allows you to use the software <br />for any purpose you might like - including commercial and for-profit use.

<br />

However - this library includes several [Third-Party](doc/Third-Party.md) libraries which are licensed under their own respective **Open Source** licenses ➜ They all allow static linking with closed source software.

<br />

**All copies of liblava must include a copy of the MIT License terms and the copyright notice**

Vulkan and the Vulkan logo are trademarks of the <a href="http://www.khronos.org" target="_blank">Khronos Group Inc.</a>

Copyright (c) 2018-present - <a href="https://lava-block.com">Lava Block OÜ</a> and [contributors](https://github.com/liblava/liblava/graphs/contributors)

<br />

<a href="https://git.io/liblava"><img src="res/doc/liblava_logo_200px.png" width="50"></a>
