#include <napi.h>

#include <addon.hpp>

class BasicClass {
public:
    BasicClass() : _msg("New message.\n") {
        counter++;
        // printf("%d New BasicClass has been initialized.\n", counter);
    }
    ~BasicClass() {
        // printf("%d BasicClass has been destroyed.\n", counter);
        counter--;
    }
    std::string _msg;
    static int counter;
};
int BasicClass::counter = 0;

class BasicWrap : public Napi::ObjectWrap<BasicWrap> {
public:
    BasicWrap(const Napi::CallbackInfo& info) : Napi::ObjectWrap<BasicWrap>(info) {
        // printf("BasicWrap created.\n");
    }

    ~BasicWrap() {
        printf("BasicWrap destroyed.\n");
    }

    static Napi::Function get_class(Napi::Env env) {
        return DefineClass(env, "BasicWrap", {InstanceMethod("getMessage", &BasicWrap::get_message)});
    }

    Napi::Value get_message(const Napi::CallbackInfo& info) {
        return Napi::String::New(info.Env(), _basic_class._msg);
    }

private:
    BasicClass _basic_class;
};
