// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "addon.hpp"

#include <napi.h>

#include "basic_class.hpp"
#include "core_wrap.hpp"
#include "tensor.hpp"
#include "openvino/openvino.hpp"



void init_class(Napi::Env env,
                Napi::Object exports,
                std::string class_name,
                Prototype func,
                Napi::FunctionReference& reference) {
    const auto& prototype = func(env);
    printf("%s was initialized.\n", class_name.c_str());

    reference = Napi::Persistent(prototype);
    exports.Set(class_name, prototype);
}

/** @brief Initialize native add-on */
Napi::Object init_module(Napi::Env env, Napi::Object exports) {
    auto addon_data = new AddonData();
    env.SetInstanceData<AddonData>(addon_data);

    init_class(env, exports, "Core", &CoreWrap::get_class, addon_data->core);
    init_class(env, exports, "Tensor", &TensorWrap::get_class, addon_data->tensor);
    init_class(env, exports, "Basic", &BasicWrap::get_class, addon_data->basic_wrap);
   
    exports.Set(Napi::String::New(env, "methodA"), Napi::Function::New(env, MethodA));
    exports.Set(Napi::String::New(env, "methodB"), Napi::Function::New(env, MethodB));

    return exports;
}

/** @brief Register and initialize native add-on */
NODE_API_MODULE(addon_openvino, init_module)


