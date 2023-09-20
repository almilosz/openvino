// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include <napi.h>

#include <openvino/runtime/compiled_model.hpp>
#include <openvino/runtime/infer_request.hpp>

class AsyncManager : public Napi::ObjectWrap<AsyncManager> {
public:
    AsyncManager(const Napi::CallbackInfo& info);

    static Napi::Function GetClassConstructor(Napi::Env env);

    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    /** @brief  Checks incoming Napi::Value and calls overloaded infer() method */
    Napi::Value infer_dispatch(const Napi::CallbackInfo& info);

    /** @brief Infers specified inputs in synchronous mode.
     * @param inputs  An object with a collection of pairs key (input_name) and a value (tensor, tensor's data)
     */
    void infer(const Napi::Object& inputs);

    /** @brief Infers specified inputs in synchronous mode.
     * @param inputs  An Array with values (tensors, tensors' data)
     */
    void infer(const Napi::Array& inputs);

    /** @return A Javascript object with model outputs. */
    std::map<std::string, ov::Tensor> get_output_tensors();

    /** @return A Javascript object with model outputs. */
    Napi::Value get_output_tensors(const Napi::CallbackInfo& info);

    /** @return A Javascript CompiledModel. */
    Napi::Value get_compiled_model(const Napi::CallbackInfo& info);

private:
    ov::CompiledModel _compiled_model;
    ov::InferRequest _infer_request;
};
