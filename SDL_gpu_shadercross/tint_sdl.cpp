// File: tint_sdl.cpp
// Author: Kyle Lukaszek
// Email: kylelukaszek [at] gmail [dot] com
// Date: 2024-09-26
// -------------------------------------------------------------
// Special thanks to the Tint contributors for their work on the Tint compiler.
// -------------------------------------------------------------
//
// License: Apache 2.0 (Follows the same license as Tint)
//
// Requirements:
// - git
// - emscripten toolchain
// - make
//
// Description:
// This file is a simple C++ wrapper around the Tint compiler for converting SPIR-V to WGSL.
// The main function is tint_spv_to_wgsl which takes in a SPIR-V shader as a uint8_t array from
// SDL and returns a char* to the WGSL shader.
//
// This .cpp file is meant to be assigned as an extension to libtint.a which can be found
// at https://www.github.com/klukaszek/tint-wasm.
//
// Build:
// To build this file, simply run 'make' in the same directory as this file. This will
// compile the file and create a static library called libtint.a which will be linked
// to the main program if it is compiled using Emscripten.
//
// Making Changes:
// To update libtint.a with any changes made to this file, simply run 'make clean', then 'make' 
// again in this directory. This will recompile the object files and update the precompiled 
// static library pulled from github. This library is precompiled as it is WASM32 and saves 
// a lot of time not having to compile Tint from source.
//
// This file really doesn't need to be changed though as it only serves to convert SPIR-V to WGSL
// and vice-versa.
// ---------------------------------------------------------------

#include "api/tint.h"
#include "lang/wgsl/common/allowed_features.h"
#include "utils/diagnostic/formatter.h"
#include <sys/types.h>
#define TINT_BUILD_WGSL_WRITER 1
#define TINT_BUILD_WGSL_READER 1
#define TINT_BUILD_SPV_READER  1
#define TINT_BUILD_SPV_WRITER  1

#include "cmd/common/helper.h"
#include "tint.h"
#include "utils/diagnostic/source.h"
#include <charconv>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <optional>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

std::vector<uint32_t> convertUint8ArrayToUint32Array(const uint8_t *data, size_t size)
{
    std::vector<uint32_t> result;
    result.reserve((size / 4));
    for (size_t i = 0; i < size; i += 4) {
        uint32_t word = 0;
        word |= data[i + 0] << 0;
        word |= data[i + 1] << 8;
        word |= data[i + 2] << 16;
        word |= data[i + 3] << 24;
        result.push_back(word);
    }
    return result;
}

tint::cmd::ProgramInfo LoadProgramInfo(const tint::cmd::LoadProgramOptions &opts, const std::vector<uint32_t> shader_code)
{
    auto load = [&]() -> tint::cmd::ProgramInfo {
        return tint::cmd::ProgramInfo{
            /* program */ tint::spirv::reader::Read(shader_code, {}),
            /* source_file */ nullptr,
        };
    };

    tint::cmd::ProgramInfo info = load();

    if (info.program.Diagnostics().Count() > 0) {

        std::cerr << info.program.Diagnostics().Str() << std::endl;

        fflush(stderr);
    }

    if (!info.program.IsValid()) {
        std::cerr << "Program is not valid." << std::endl;
        return info;
    }

    return info;
}

extern "C" {

void tint_initialize(void)
{
    tint::Initialize();
}

char *tint_spv_to_wgsl(const uint8_t *shader_data, const size_t shader_size)
{
    tint::cmd::LoadProgramOptions opts;

    opts.filename = "spv-shader";
    opts.use_ir = false;
    opts.printer = nullptr;

    // Properly convert SDL's uint8_t SPIRV array to a uint32_t array
    std::vector<uint32_t> shader_code = convertUint8ArrayToUint32Array(shader_data, shader_size);

    auto info = LoadProgramInfo(opts, shader_code);

    tint::wgsl::writer::Options options;
    auto result = tint::wgsl::writer::Generate(info.program, options);

    // Malloc the "str" pointer to the size of the WGSL string
    char *wgsl = (char *)malloc(result->wgsl.size() + 1);

    // Copy the WGSL string to the provided buffer
    std::strcpy(wgsl, result->wgsl.c_str());

    return wgsl;
}
}
