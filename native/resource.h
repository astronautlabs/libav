#ifndef __RESOURCE_H__
#   define __RESOURCE_H__

#include <napi.h>
#include "libavaddon.h"

#define R_GETTER(name, getter) InstanceAccessor((name), (getter), nullptr)
#define R_SETTER(name, setter) InstanceAccessor((name), nullptr, (setter))
#define R_ACCESSOR(name, getter, setter) InstanceAccessor((name), (getter), (setter))
#define R_METHOD(name, impl) InstanceMethod<impl>((name))

/**
 * The default implementation of GetRegisterableHandle when a more specific one is not provided.
 * This implementation simply returns the given handle.
 */
void* DefaultGetRegisterableHandle(void *handle);

/**
 * Base class for all resource classes in libav-node. Responsible for managing the resource instance's
 * status in the addon's resource map, registering the class as an export of the addon, registering the class's
 * JS constructor in the addon's constructor map, providing native construction services, and more.
 * 
 * Notable template parameters:
 * - SelfT: The subclass itself
 * - HandleT: The type of the handle, as stored on this object.
 * - GetRegisterableHandle: used to translate the handle that gets associated with the NAVResource* object
 *   into the one that is used for registering the resource into the resource map. This is useful for example 
 *   for AVBuffer and AVBufferRef- AVBufferRef* is the handle on the NAVBuffer resource class, but AVBuffer*
 *   is the pointer used for the resource registration map.
 * 
 * Subclass MUST implement the following two static methods:
 * - static Napi::Function ClassDefinition();
 * - static std::string ExportName();
 */
template<class SelfT, class HandleT, void *(*GetRegisterableHandle)(void *) = DefaultGetRegisterableHandle>
class NAVResource : public Napi::ObjectWrap<SelfT> {

    public:
        NAVResource(const Napi::CallbackInfo& info):
            Napi::ObjectWrap<SelfT>(info)
        {
            handle = nullptr;
        }

        /**
         * Called from subclass constructors to handle creating a resource from its corresponding handle.
         * If true is returned, then the Javascript call consists of one argument which is the Napi::External
         * handle pointer, and adopting that handle has been completed. The subclass may wish to perform additional
         * initialization in this case, but should certainly early-return the constructor and avoid processing the 
         * call further.
         */
        bool ConstructFromHandle(const Napi::CallbackInfo& info) {
            if (info.Length() == 1 && info[0].IsExternal()) {
                this->handle = info[0].As<Napi::External<HandleT>>().Data();

                if (handle) {
                    RegisterResource(info.Env());
                }

                return true;
            }

            return false;
        }

        static void Register(Napi::Env env, Napi::Object exports) {
            Napi::Function ctor = SelfT::ClassDefinition(env);
            std::string exportName = SelfT::ExportName();

            LibAvAddon::Self(env)->RegisterConstructor(exportName, ctor);
            exports.Set(exportName, ctor);
            
        }

        /**
         * Finalize this instance when it becomes unreachable via the garbage collector.
         * This will disassociate the current handle within the resource map and call the 
         * Free() method to free it (or unreference it, depending on the type of resource the 
         * subclass represents)
         */
        virtual void Finalize(Napi::Env env) {
            if (this->handle) {
                LibAvAddon::Self(env)->UnregisterResource(GetRegisterableHandle((void*)handle));
                Free();
            }
        }

        /**
         * Free (or decrease reference count) of the associated handle.
         * Subclass *must* implement this method.
         */
        virtual void Free() = 0;

        /**
         * Whether resource mapping is enabled (default true).
         * When false is returned, the handles of the subclass are never entered into the 
         * resource map. Allows the subclass to take advantage of the other capabilities 
         * of inheriting from NAVResource without enabling this behavior. This is particularly 
         * useful when the handles are never owned by the NAVResource subclass 
         * (such as AVDictionaryEntry).
         */
        virtual bool IsResourceMappingEnabled() { return true; }

        /**
         * Increase the reference count of the associated handle. By default this does nothing,
         * assuming that the underlying handle is not reference counted. 
         */
        virtual void RefHandle() {
            // Do nothing by default.
        }

        /**
         * Acquire an instance of the subclass given a specific native handle that it should hold.
         * If an existing instance exists in the addon's resource map, it will be returned. If 
         * no instance exists, one will be created.
         * 
         * @param refIsOwned Whether the handle was created specifically for this instance or not.
         *                   If it was not, then Ref() will be called to increase the reference count 
         *                   of the underlying handle (if that is supported by the subclass)
         */
        static SelfT *FromHandle(const Napi::Env &env, HandleT *handle, bool refIsOwned) {
            if (!handle)
                return nullptr;
            
            SelfT *instance = LibAvAddon::Self(env)->GetResource<SelfT>(
                GetRegisterableHandle((void*)handle)
            );

            if (instance)
                return instance;

            instance = LibAvAddon::Self(env)->Construct<SelfT>(env, SelfT::ExportName(), { 
                Napi::External<HandleT>::New(env, (HandleT*)handle)
            });

            if (!refIsOwned) {
                instance->RefHandle();
            }

            return instance;
        }

        static Napi::Value FromHandleWrapped(const Napi::Env env, HandleT *ref, bool refIsOwned) {
            if (!ref)
                return env.Null();
            
            return FromHandle(env, ref, refIsOwned)->Value();
        }
        
        static std::vector<SelfT *> FromHandles(const Napi::Env &env, HandleT *items, int count, bool refsAreOwned) {
            std::vector<SelfT*> vec;
            for (int i = 0, max = count; i < max; ++i)
                vec.push_back(FromHandle(env, items + i, refsAreOwned));
            return vec;
        }
        
        static Napi::Array FromHandlesWrapped(const Napi::Env &env, HandleT *items, int count, bool refsAreOwned) {
            auto array = Napi::Array::New(env, count);
            std::vector<SelfT*> vec = FromHandles(env, items, count, refsAreOwned);

            for (int i = 0, max = count; i < max; ++i)
                array.Set(i, vec[i] ? vec[i]->Value() : env.Null());
            
            return array;
        }

        HandleT *GetHandle() {
            return handle;
        }

        /**
         * Change this instance's handle to another one. If the handle was previously set,
         * it will be unregistered from the addon's resource map. The previous handle will not 
         * be automatically freed, that should be handled by the caller. If the passed handle
         * is already the active handle, nothing will happen. The new handle will be registered
         * with the resource map.
         */
        void SetHandle(HandleT *handle) {
            if (this->handle == handle)
                return;
            UnregisterResource(this->Env());
            this->handle = handle;
            RegisterResource(this->Env());
        }

    private:
        HandleT *handle = nullptr;

        /**
         * Register this instance and its handle into the addon's resource map as the 
         * sole instance responsible for the given handle.
         */
        void RegisterResource(const Napi::Env &env) {
            if (!handle || !IsResourceMappingEnabled())
                return;
            
            LibAvAddon::Self(env)->RegisterResource(
                GetRegisterableHandle((void*)handle), 
                this
            );
        }

        /**
         * Unregister this instance and its handle in the addon's resource map as the 
         * sole instance responsible for the given handle
         */
        void UnregisterResource(const Napi::Env &env) {
            if (!handle)
                return;
            
            LibAvAddon::Self(env)->UnregisterResource(
                GetRegisterableHandle((void*)handle)
            );
        }
};

#endif // #ifndef __RESOURCE_H__