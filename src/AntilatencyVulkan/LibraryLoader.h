#pragma once

#include <string>

#if defined(_WIN32)
#   include <Windows.h>
#elif defined(__ANDROID__) || defined(__linux__)
#   include "dlfcn.h"
#else
#   error unsupported os
#endif



class Library {
private:
	void* handle = nullptr;

public:
	Library() {}

	Library(const Library&) = delete;
	Library& operator=(const Library&) = delete;
	Library(Library&& r){
		handle = r.handle;
		r.handle = nullptr;
    }
	
	Library& operator=(Library&& r) {
		handle = r.handle;
		r.handle = nullptr;
		return *this;
	}


	Library(const char* name) {
		std::string sName = name;
	#if defined(_WIN32)
		sName += ".dll";
		handle = (void*)LoadLibraryA(sName.c_str());
	#else
		sName += ".so";
        //auto iMode = RTLD_LAZY; //this mode allows deffered symbol resolving
        auto iMode = RTLD_NOW | RTLD_LOCAL;
        handle = reinterpret_cast<void*>( dlopen(sName.c_str(), iMode) );
	#endif
	}

	void* getFunction(const char *name) const {
	#if defined(_WIN32) // Microsoft compiler
		return (void*)GetProcAddress((HINSTANCE)handle, name);
	#else
		return dlsym(handle, name);
	#endif
	}

	~Library(){
		if (handle == nullptr) return;
	#if defined(_WIN32)
		FreeLibrary((HINSTANCE)handle);
	#else
		dlclose(handle);
	#endif
		handle = nullptr;
	}
};
