cmake_minimum_required(VERSION 3.10)
project(SDL_gpu_examples)
find_package(SDL3 REQUIRED)

add_executable(SDL_gpu_examples
    Examples/main.c
    Examples/Common.h
    stb_image.h
    Examples/Common.c
    Examples/ClearScreen.c
    Examples/BasicTriangle.c
    Examples/BasicVertexBuffer.c
    Examples/CullMode.c
    Examples/BasicStencil.c
    Examples/InstancedIndexed.c
    Examples/TexturedQuad.c
    Examples/TexturedAnimatedQuad.c
    Examples/Clear3DSlice.c
    Examples/BasicCompute.c
    Examples/ComputeUniforms.c
    Examples/ToneMapping.c
    Examples/CustomSampling.c
    Examples/DrawIndirect.c
    Examples/ComputeSpriteBatch.c
    Examples/CopyAndReadback.c
    Examples/CopyConsistency.c
    Examples/Texture2DArray.c
    Examples/TriangleMSAA.c
    Examples/Cubemap.c
    Examples/WindowResize.c
    Examples/Blit2DArray.c
    Examples/BlitCube.c
    Examples/BlitMirror.c
    Examples/GenerateMipmaps.c
    Examples/ClearScreenMultiWindow.c
    Examples/ComputeSampler.c
)

target_include_directories(SDL_gpu_examples PRIVATE SDL_gpu_shadercross)

add_custom_command(TARGET SDL_gpu_examples PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Content $<TARGET_FILE_DIR:SDL_gpu_examples>/Content
)

if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    # # If SDL_gpu_shadercross does not have libtint.a, build it
    # if(NOT EXISTS ${CMAKE_SOURCE_DIR}/SDL_gpu_shadercross/libtint.a)
        add_custom_command(TARGET SDL_gpu_examples PRE_BUILD
            COMMAND make VERBOSE=1
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/SDL_gpu_shadercross
        )
    # endif()

    # Compiler and linker flags for Emscripten
    set_target_properties(SDL_gpu_examples PROPERTIES
    COMPILE_FLAGS "${EMCC_COMPILER_FLAGS}"
    LINK_FLAGS "${EMCC_LINKER_FLAGS} -g -o SDL_gpu_examples.html --shell-file ../shell.html -O3 -sWASM=1 -sUSE_WEBGPU -sASYNCIFY -sSTACK_SIZE=262144 -sNO_DISABLE_EXCEPTION_CATCHING -sALLOW_TABLE_GROWTH=1 -sALLOW_MEMORY_GROWTH=1 -sASSERTIONS=1")
    
    # Add the Content/ directory to Emscripten's virtual filesystem
    target_link_options(SDL_gpu_examples PUBLIC "SHELL: --preload-file Content/")
    
    # Link libtint.a to the project
    target_link_directories(SDL_gpu_examples PRIVATE SDL_gpu_shadercross)
    target_link_libraries(SDL_gpu_examples PRIVATE SDL3::SDL3 SDL3::Headers tint)

    set(CLEAN_FILES ${CLEAN_FILES} 
        ./SDL_gpu_shadercross/libtint.a
    )

else()

    target_link_libraries(SDL_gpu_examples
    SDL3::SDL3
    SDL3::Headers)

endif()
