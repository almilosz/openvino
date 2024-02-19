#include <napi.h>
#include <addon.hpp>

Napi::Value MethodA(const Napi::CallbackInfo& info) {
    printf("MethodA\n");
    Napi::FunctionReference& constructor = info.Env().GetInstanceData<AddonData>()->basic_wrap;
    return constructor.New({});
}

Napi::Value MethodB(const Napi::CallbackInfo& info) {
    printf("MethodB\n");
    Napi::HandleScope scope(info.Env());
    Napi::FunctionReference& constructor = info.Env().GetInstanceData<AddonData>()->basic_wrap;
    
    // https://github.com/nodejs/node-addon-api/blob/main/doc/object_lifetime_management.md#making-handle-lifespan-shorter-than-that-of-the-native-method
    for (int i = 0; i < 50000; i++) {
        Napi::HandleScope scope(info.Env()); // Case 
        auto b = constructor.New({});
        std::string name = std::string("inner-scope") + std::to_string(i);
        Napi::Value newValue = Napi::String::New(info.Env(), name.c_str());
        // do something with newValue
        
    };

    // auto sentence1 = Napi::String::New(info.Env(), "no 1");
    // auto sentence2 = Napi::String::New(scope.Env(), "no 2");
    return constructor.New({});
}

Napi::Value MethodC(const Napi::CallbackInfo& info) {
   auto sentence1 = Napi::Array::New(info.Env(), "no 1"); 
   // TEST: assign some file content and return it as string
   return constructor.New({});
}


class BasicClass {
public:
    BasicClass(): _msg("New message.\n"){
        counter++;
        printf("%d New BasicClass has been initialized.\n", counter);
                
    }
    ~BasicClass() {
        printf("%d BasicClass has been destroyed.\n", counter);
        counter--;
    }
    std::string _msg;
    static int counter;
};
int BasicClass::counter = 0;

class BasicWrap : public Napi::ObjectWrap<BasicWrap> {
public:
    BasicWrap(const Napi::CallbackInfo& info) : Napi::ObjectWrap<BasicWrap>(info) {
        this->_basic_class._msg = "Basic Class constructor\n";
        printf("BasicWrap created.\n");
    }

    ~BasicWrap() {
        printf("BasicWrap destroyed.\n");
    }

    static Napi::Function get_class(Napi::Env env) {
        // Napi::HandleScope scope(env); // error. BasicWrap constructor doesn't exists on js side
        return DefineClass(env, "BasicWrap", {
            InstanceMethod("getMessage", &BasicWrap::get_message)
        });
    }

    static Napi::Object init(Napi::Env env, Napi::Object exports) {
        Napi::HandleScope scope(env);

        const auto& prototype =  DefineClass(env, "BasicWrap", {
            InstanceMethod("getMessage", &BasicWrap::get_message)
        });
        printf("Basic was initialized.\n");
        auto& reference = env.GetInstanceData<AddonData>()->basic_wrap;
        reference = Napi::Persistent(prototype);
        exports.Set("Basic", prototype);
        return exports;
    }

    Napi::Value get_message(const Napi::CallbackInfo& info) {
        return Napi::String::New(info.Env(), _basic_class._msg);
    }

private:
    BasicClass _basic_class;
};
