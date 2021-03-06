#ifndef __NLAV_LIBAV_H
#define __NLAV_LIBAV_H

#include "common.h"
#include "helpers.h"
#include <napi.h>
#include <map>
#include <assert.h>

class LibAvAddon : public Napi::Addon<LibAvAddon>
{
public:
    LibAvAddon(Napi::Env env, Napi::Object exports);
    static LibAvAddon *Self(const Napi::Env &env);
    static LibAvAddon *Self(const Napi::CallbackInfo &call);

    Napi::FunctionReference *AVBufferPool;
    Napi::FunctionReference *AVFrame;
    Napi::FunctionReference *AVFrameSideData;

    template <class ResourceT, class HandleT>
    ResourceT *GetResource(const HandleT *handle) {
         auto iter = resourceMap.find((void*)handle);
         if (iter == resourceMap.end())
            return nullptr;
        
        return (ResourceT*) (*iter).second;
    }

    template <class ResourceT, class HandleT>
    void RegisterResource(const HandleT *handle, const ResourceT *resource) {
        resourceMap[(void*)handle] = (void*)resource;
    }

    template <class HandleT>
    void UnregisterResource(const HandleT *handle) {
        resourceMap.erase((void*)handle);
    }

    inline Napi::FunctionReference *GetConstructor(std::string name) {
        auto iter = constructorMap.find(name);
        if (iter == constructorMap.end())
            return nullptr;

        return (*iter).second;
    }

    inline void RegisterConstructor(std::string name, Napi::Function ctor) {
        auto ctorRef = new Napi::FunctionReference();
        *ctorRef = Napi::Persistent(ctor);
        constructorMap[name] = ctorRef;
    }

    template<class ResourceT>
    static ResourceT* Construct(
        const Napi::Env &env, 
        const std::initializer_list<napi_value> &args
    ) {
        return Construct<ResourceT>(env, ResourceT::ExportName(), args);
    }

    template<class ResourceT>
    static ResourceT* Construct(
        const Napi::Env &env
    ) {
        return Construct<ResourceT>(env, std::initializer_list<napi_value>());
    }

    template<class ResourceT>
    static ResourceT* Construct(
        const Napi::Env &env, 
        const std::string ctorName, 
        const std::initializer_list<napi_value> &args
    ) {
        return ResourceT::Unwrap(
            ConstructWrapped(env, ctorName, args)
        );
    }

    inline static Napi::Object ConstructWrapped(
        const Napi::Env &env, 
        const std::string ctorName, 
        const std::initializer_list<napi_value> &args
    ) {
        return assert_valid(Self(env)->GetConstructor(ctorName))->New(args);
    }
    
private: 
    std::map<void*, void*> resourceMap;
    std::map<std::string, Napi::FunctionReference*> constructorMap;
};

#endif // __NLAV_LIBAV_H