# SDL3 WebGPU Examples

Collection of WebGPU examples for SDL3 based on the examples provided by: 

- https://github.com/TheSpydog/SDL_gpu_examples/


## SDL3 WebGPU Fork
You can acquire the WebGPU fork of SDL3 from my repo here: 

- https://github.com/klukaszek/SDL

## Building 

**Emscripten version 3.1.69+ is required to make use of SDL3 WebGPU.**
**There is an issue with Emscripten versions (>=3.1.65 & < 3.1.69) that breaks buffer copying**

Native testing and support will be done later. For the time being I only really care about web since SDL3 already has DX, Metal, and VK.

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

# Example Checklist

- [x] ClearScreen.c
- [x] BasicTriangle.c
- [x] BasicVertexBuffer.c
- [x] CullMode.c
- [x] BasicStencil.c
- [x] InstancedIndexed.c
- [x] TexturedQuad.c
- [x] Texture2DArray.c
- [ ] TexturedAnimatedQuad.c
    - Example loads with no warnings, but nothing draws.
    - Needs to be investigated.
- [x] Clear3DSlice.c,
- [x] Blit2DArray.c 
    - Sampler issue on right texture with no warnings, but draws.
    - Needs to be investigated.
- [x] BlitCube.c
- [ ] BlitMirror.c
    - Example loads with no warnings, but nothing draws.
    - Needs to be investigated.
- [x] Cubemap.c
- [ ] CopyAndReadback.c
- [ ] CopyConsistency.c
- [ ] BasicCompute.c
- [ ] ComputeUniforms.c
- [ ] ToneMapping.c
- [ ] CustomSampling.c
- [x] DrawIndirect.c
- [ ] ComputeSpriteBatch.c
- [x] TriangleMSAA.c 
    - Draws properly, but no visible change occurs when changing the sample count.
    - Needs to be investigated.
- [x] WindowResize.c (Resizes browser canvas. Have not tested anything natively.
- [ ] GenerateMipmaps.c
