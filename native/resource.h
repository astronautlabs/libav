#ifndef __RESOURCE_H__
#   define __RESOURCE_H__

#include <napi.h>

template<class SelfT, class HandleT>
class NAVResource : public Napi::ObjectWrap<SelfT> {
    public:
        NAVResource(const Napi::CallbackInfo& info):
            Napi::ObjectWrap<SelfT>(info)
        {
        }

        bool ConstructFromHandle(const Napi::CallbackInfo& info) {
            if (info.Length() === 1 && info[0].IsExternal()) {
                this->handle = info[0].As<Napi::External<HandleT>>().Data();
                return true;
            }

            return false;
        }

        // static virtual Napi::Function ClassDefinition() = 0;
        // static virtual std::string ExportName() = 0;

        static void Register(Napi::Env env, Napi::Object exports) {
            auto defn = ClassDefinition();

            auto constructor = new Napi::FunctionReference();
            *constructor = Napi::Persistent(func);
            
            LibAvAddon::Self(env)->AVBuffer = constructor;

            exports.Set(SelfT::template ExportName(), constructor->Value());

        }

        virtual void Finalize(Napi::Env env) {
            Free();
        }

        /**
         * Free (or decrease reference count) of the associated handle.
         */
        virtual void Free() = 0;

        /**
         * Increase the reference count of the associated handle.
         */
        virtual void RefHandle() = 0;

        static SelfT *FromHandle(const Napi::Env env, HandleT *ref, bool refIsOwned) {
            SelfT *instance = LibAvAddon::Self(env)->Construct(env, SelfT::template ExportName(), { 
                Napi::External<HandleT>::New(env, ref)
            });

            if (!refIsOwned)
                instance->Ref();

            return instance;
        }

        static SelfT *FromHandleWrapped(const Napi::Env env, HandleT *ref, bool refIsOwned) {
            return FromHandle(env, ref, refIsOwned)->Value();
        }
        
        HandleT *handle;
};

#endif // #ifndef __RESOURCE_H__