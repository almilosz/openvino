// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "async_manager.hpp"

#include "compiled_model.hpp"
#include "node_output.hpp"
#include "tensor.hpp"

AsyncManager::AsyncManager(const Napi::CallbackInfo& info) : Napi::ObjectWrap<AsyncManager>(info) {
    if (info.Length() != 1) {
        reportError(info.Env(), "Invalid number of arguments for AsyncManager constructor.");
        return;
    }
    try {
        auto compiledModelWrap = Napi::ObjectWrap<CompiledModelWrap>::Unwrap(info[0].ToObject());
        _compiled_model = compiledModelWrap->get_compiled_model();
        _infer_request = _compiled_model.create_infer_request();
    } catch (std::exception& e) {
        reportError(info.Env(), e.what());
    }
}

Napi::Function AsyncManager::GetClassConstructor(Napi::Env env) {
    return DefineClass(env,
                       "AsyncManager",
                       {
                           InstanceMethod("infer", &AsyncManager::infer_dispatch),
                       });
}

Napi::Object AsyncManager::Init(Napi::Env env, Napi::Object exports) {
    auto func = GetClassConstructor(env);

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("AsyncManager", func);
    return exports;
}

// Data structure representing our thread-safe function context.
struct TsfnContext {
    TsfnContext(Napi::Env env)
        : deferred(Napi::Promise::Deferred::New(env)){

          };

    Napi::Promise::Deferred deferred;
    // std::thread nativeThread;

    Napi::ThreadSafeFunction tsfn;
    std::map<std::string, ov::Tensor> result;
};

// The thread-safe function finalizer callback. This callback executes
// at destruction of thread-safe function, taking as arguments the finalizer
// data and threadsafe-function context.
void FinalizerCallback(Napi::Env env, void* finalizeData, TsfnContext* context) {
    // Join the thread
    //   context->nativeThread.join();

    delete context;
};

Napi::Value AsyncManager::infer_dispatch(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    // info[0] = tensor like data
    if (info.Length() == 1 && info[0].IsTypedArray()) {
        reportError(info.Env(), "TypedArray cannot be passed directly into infer() method.");
        return env.Null();
    } else if (info.Length() == 1 && info[0].IsArray()) {
        try {
            infer(info[0].As<Napi::Array>());
        } catch (std::exception& e) {
            reportError(env, e.what());
            return env.Null();
        }
    } else if (info.Length() == 1 && info[0].IsObject()) {
        try {
            infer(info[0].As<Napi::Object>());
        } catch (std::exception& e) {
            reportError(env, e.what());
            return env.Null();
        }
    } else {
        reportError(env, "Infer method takes as an argument an array or an object.");
    }

    // Construct context data
    auto context_data = new TsfnContext(env);
    context_data->result = get_output_tensors();

    context_data->tsfn = Napi::ThreadSafeFunction::New(env,                // Environment
                                                       Napi::Function(),   // we use promise to return result
                                                       "TSFN",             // Resource name
                                                       0,                  // Max queue size (0 = unlimited).
                                                       1,                  // Initial thread count
                                                       context_data,       // Context,
                                                       FinalizerCallback,  // Finalizer
                                                       (void*)nullptr      // Finalizer data
    );

    auto callback = [](Napi::Env env, Napi::Function _, TsfnContext* data) {
        auto m = data->result;

        auto outputs_obj = Napi::Object::New(env);

        for (const auto& [key, tensor] : m) {
            auto new_tensor = ov::Tensor(tensor.get_element_type(), tensor.get_shape());
            tensor.copy_to(new_tensor);
            outputs_obj.Set(key, TensorWrap::Wrap(env, new_tensor));
        }
        data->deferred.Resolve({outputs_obj});
    };

    napi_status status = context_data->tsfn.BlockingCall(context_data, callback);

    if (status != napi_ok) {
        Napi::Error::Fatal("ThreadEntry", "Napi::ThreadSafeNapi::Function.BlockingCall() failed");
    }

    context_data->tsfn.Release();
    return context_data->deferred.Promise();
}

void AsyncManager::infer(const Napi::Array& inputs) {
    for (size_t i = 0; i < inputs.Length(); ++i) {
        auto tensor = value_to_tensor(inputs[i], _infer_request, i);
        _infer_request.set_input_tensor(i, tensor);
    }
    _infer_request.infer();
}

void AsyncManager::infer(const Napi::Object& inputs) {
    auto keys = inputs.GetPropertyNames();

    for (size_t i = 0; i < keys.Length(); ++i) {
        auto input_name = static_cast<Napi::Value>(keys[i]).ToString().Utf8Value();
        auto value = inputs.Get(input_name);
        auto tensor = value_to_tensor(value, _infer_request, input_name);

        _infer_request.set_tensor(input_name, tensor);
    }
    _infer_request.infer();
}

Napi::Value AsyncManager::get_output_tensors(const Napi::CallbackInfo& info) {
    auto compiled_model = _infer_request.get_compiled_model().outputs();
    auto outputs_obj = Napi::Object::New(info.Env());

    for (auto& node : compiled_model) {
        auto tensor = _infer_request.get_tensor(node);
        auto new_tensor = ov::Tensor(tensor.get_element_type(), tensor.get_shape());
        tensor.copy_to(new_tensor);
        outputs_obj.Set(node.get_any_name(), TensorWrap::Wrap(info.Env(), new_tensor));
    }
    return outputs_obj;
}

std::map<std::string, ov::Tensor> AsyncManager::get_output_tensors() {
    auto compiled_model = _infer_request.get_compiled_model().outputs();
    std::map<std::string, ov::Tensor> outputs;

    for (auto& node : compiled_model) {
        auto tensor = _infer_request.get_tensor(node);
        auto new_tensor = ov::Tensor(tensor.get_element_type(), tensor.get_shape());
        tensor.copy_to(new_tensor);
        outputs.insert({node.get_any_name(), new_tensor});
    }
    return outputs;
}

Napi::Value AsyncManager::get_compiled_model(const Napi::CallbackInfo& info) {
    return CompiledModelWrap::Wrap(info.Env(), _infer_request.get_compiled_model());
}
