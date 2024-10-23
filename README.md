# SDL3 WebGPU Examples

Collection of WebGPU examples for SDL3 based on the examples provided by: 

- https://github.com/TheSpydog/SDL_gpu_examples/


## SDL3 WebGPU Fork
You can acquire the WebGPU fork of SDL3 from my repo here: 

- https://github.com/klukaszek/SDL

## Building 

**Emscripten 3.1.64 is required to make use of SDL3 WebGPU.**
**There is currently an issue with Emscripten versions >=3.1.65 that breaks buffer copying**

To clone and build SDL from source:
```
git clone https://github.com/klukaszek/SDL
cd SDL
mkdir build
cd build
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release -DSDL_WEBGPU=ON -DSDL_RENDER_WEBGPU=ON
cmake --build . --config Release --parallel
sudo cmake --install . --config Release
```

To clone and build examples:
```
git clone https://github.com/klukaszek/SDL3-WebGPU-Examples
cd SDL3-WebGPU-Examples
git submodule update --init
git submodule update --remote
mkdir build
cd build
emcmake cmake .. -DSDL3_DIR="full/path/to/SDL/build"
make
```

You can also provide the `-GNinja` flag to any of the `emcmake` commands to use Ninja instead of Make.
See CMakeLists.txt for more info on the build process.

On your first compilation, a precompiled static library for Google's Tint compiler will be downloaded once so that we can cross-compile between SPIRV and WGSL.
