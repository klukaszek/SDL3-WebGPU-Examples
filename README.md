# SDL3 WebGPU Examples

Collection of WebGPU examples for SDL3 based on the examples provided by: 

- https://github.com/TheSpydog/SDL_gpu_examples/


## SDL3 WebGPU Fork
You can acquire the WebGPU fork of SDL3 from my repo here: 

- https://github.com/klukaszek/SDL

## Building 

**You will require Emscripten to be installed to make use of SDL3 WebGPU.**

To clone and build SDL from source:
```
git clone https://github.com/klukaszek/SDL
cd SDL
git submodule update --init
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
mkdir build
cd build
emcmake cmake .. -DSDL3_DIR="full/path/to/SDL/build"
make
```

You can also provide the `-GNinja` flag to any of the `emcmake` commands to use Ninja instead of Make.
See CMakeLists.txt for more info on the build process.

# Troubleshooting

It is important to not forget to install the git submodule so that we can compile a WASM compatible SPIRV-Cross library. We need SPIRV-Cross to perform reflection for building valid BindGroupLayouts.

If you encounter any problems with WASM-LD not finding SPIRV-Cross functions, then perform the following:
```
cd SDL 
cd SPIRV-Cross
mkdir build
cd build
emcmake cmake ..
make
```
