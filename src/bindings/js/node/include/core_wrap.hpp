// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <napi.h>

#include <thread>

#include "openvino/runtime/core.hpp"

class CoreWrap : public Napi::ObjectWrap<CoreWrap> {
public:
   
    CoreWrap(const Napi::CallbackInfo& info);
   
    static Napi::Function get_class(Napi::Env env);

    /** @brief Returns devices available for inference. */
    Napi::Value get_available_devices(const Napi::CallbackInfo& info);

private:
    ov::Core _core;
};


