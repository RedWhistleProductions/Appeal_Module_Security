#pragma once
#include <cstring>
#include <stdexcept>
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
class Plugin {
#ifdef _WIN32
    HMODULE Handle=nullptr;
#else
    void *Handle=nullptr;
#endif
    void (*Destroy)()=nullptr;
public:
    Plugin()=default;
    Plugin(const Plugin&)=delete;
    Plugin &operator=(const Plugin&)=delete;
    ~Plugin(){Unload();}
    template<class T> void Assign(const char *name,T &function){
        function=nullptr;if(!Handle)return;
#ifdef _WIN32
        auto raw=GetProcAddress(Handle,name);
        static_assert(sizeof(raw)==sizeof(function));std::memcpy(&function,&raw,sizeof(raw));
#else
        function=reinterpret_cast<T>(dlsym(Handle,name));
#endif
    }
    void Load(const std::string &file){
        Unload();
#ifdef _WIN32
        // Module paths follow the host's existing UTF-8 resource naming.
        int size=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,file.data(),int(file.size()),nullptr,0);
        if(size<=0)throw std::runtime_error("Invalid Security plugin path");
        std::wstring wide(size,L'\0');MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,file.data(),int(file.size()),wide.data(),size);
        Handle=LoadLibraryW(wide.c_str());
#else
        Handle=dlopen(file.c_str(),RTLD_NOW|RTLD_LOCAL);
#endif
        if(!Handle)throw std::runtime_error("Could not load Security plugin");
        void (*create)()=nullptr;Assign("Constructor",create);Assign("Destructor",Destroy);
        if(!create || !Destroy){Unload();throw std::runtime_error("Security plugin is missing lifecycle exports");}
        try{create();}catch(...){Unload();throw;}
    }
    void Unload() noexcept {
        if(!Handle)return;
        if(Destroy){try{Destroy();}catch(...){}}
#ifdef _WIN32
        FreeLibrary(Handle);
#else
        dlclose(Handle);
#endif
        Handle=nullptr;Destroy=nullptr;
    }
};
