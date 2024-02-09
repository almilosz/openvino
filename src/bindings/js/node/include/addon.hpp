// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <napi.h>

typedef Napi::Function (*Prototype)(Napi::Env);

/** @brief A structure with data that will be associated with the instance of the ov.js node-addon. */
struct AddonData {
    Napi::FunctionReference core;
    Napi::FunctionReference tensor;
    Napi::FunctionReference basic_wrap;
};

void init_class(Napi::Env env,
                Napi::Object exports,
                std::string class_name,
                Prototype func,
                Napi::FunctionReference& reference);

Napi::Object init_module(Napi::Env env, Napi::Object exports);
