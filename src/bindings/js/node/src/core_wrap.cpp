// SPDX-License-Identifier: Apache-2.0

#include "core_wrap.hpp"

#include "addon.hpp"
#include "errors.hpp"
#include "helper.hpp"

CoreWrap::CoreWrap(const Napi::CallbackInfo& info) : Napi::ObjectWrap<CoreWrap>(info), _core{} {}

Napi::Function CoreWrap::get_class(Napi::Env env) {
    return DefineClass(env, "Core", {InstanceMethod("getAvailableDevices", &CoreWrap::get_available_devices)});
}

Napi::Value CoreWrap::get_available_devices(const Napi::CallbackInfo& info) {
    const auto& devices = _core.get_available_devices();
    Napi::Array js_devices = Napi::Array::New(info.Env(), devices.size());

    uint32_t i = 0;
    for (const auto& dev : devices)
        js_devices[i++] = dev;

    return js_devices;
}
