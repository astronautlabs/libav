#ifndef __RESOURCE_H__
#   define __RESOURCE_H__

#include <napi.h>
#include "libav.h"

void* DefaultGetRegisterableHandle(void *handle);

/**
 * Base class for all resource classes in libav-node.
 * - GetRegisterableHandle is used to translate the handle that gets associated with the NAVResource* object
 *   into the one that is used for registering the resource into the resource map. This is useful for example 
 *   for AVBuffer and AVBufferRef- AVBufferRef* is the handle on the NAVBuffer resource class, but AVBuffer*
 *   is the pointer used for the resource registration map.
 */
template<class SelfT, class HandleT, void *(*GetRegisterableHandle)(void *) = DefaultGetRegisterableHandle>
class NAVResource : public Napi::ObjectWrap<SelfT> {
    public:
        NAVResource(const Napi::CallbackInfo& info):
            Napi::ObjectWrap<SelfT>(info)
        {
        }

        bool ConstructFromHandle(const Napi::CallbackInfo& info) {
            if (info.Length() == 1 && info[0].IsExternal()) {
                this->handle = info[0].As<Napi::External<HandleT>>().Data();

                LibAvAddon::Self(info.Env())->RegisterResource(
                    GetRegisterableHandle((void*)handle), 
                    this
                );

                return true;
            }

            return false;
        }

        // static virtual Napi::Function ClassDefinition() = 0;
        // static virtual std::string ExportName() = 0;

        static void Register(Napi::Env env, Napi::Object exports) {
            Napi::Function ctor = SelfT::template ClassDefinition(env);
            std::string exportName = SelfT::template ExportName();
            
            LibAvAddon::Self(env)->RegisterConstructor(exportName, ctor);
            exports.Set(exportName, ctor);
            
        }

        virtual void Finalize(Napi::Env env) {
            if (this->handle) {
                LibAvAddon::Self(env)->UnregisterResource(GetRegisterableHandle((void*)handle));
                Free();
            }
        }

        /**
         * Free (or decrease reference count) of the associated handle.
         */
        virtual void Free() = 0;

        /**
         * Increase the reference count of the associated handle.
         */
        virtual void RefHandle() = 0;

        static SelfT *FromHandle(const Napi::Env env, HandleT *handle, bool refIsOwned) {
            SelfT *instance = LibAvAddon::Self(env)->GetResource<SelfT>(
                GetRegisterableBufferHandle(handle)
            );

            if (instance)
                return instance;

            instance = LibAvAddon::Self(env)->Construct<SelfT>(env, SelfT::template ExportName(), { 
                Napi::External<HandleT>::New(env, handle)
            });

            if (!refIsOwned)
                instance->Ref();

            return instance;
        }

        static Napi::Value FromHandleWrapped(const Napi::Env env, HandleT *ref, bool refIsOwned) {
            return FromHandle(env, ref, refIsOwned)->Value();
        }
        
        HandleT *handle;
};

#endif // #ifndef __RESOURCE_H__