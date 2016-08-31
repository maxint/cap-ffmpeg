#pragma once

#include <windows.h>

class DynamicLibraryLoader
{
public:
    DynamicLibraryLoader() : _hModule(NULL) {}
    DynamicLibraryLoader(const char* libname)
    {
        load(libname);
    }
    ~DynamicLibraryLoader()
    {
        free();
    }
    template <class T> T getProcAddress(const char* funcname) const
    {
        return (T) GetProcAddress(_hModule, funcname);
    }
    bool isLoaded() { return _hModule != NULL; }
    void load(const char* libname)
    {
        free();
        _hModule = LoadLibrary(libname);
    }
    void free()
    {
        if (_hModule)
        {
            FreeLibrary(_hModule);
            _hModule = NULL;
        }
    }
private:
    HMODULE _hModule;
};

#define CV_GET_PROC_ADDRESS(loader, func) (loader).getProcAddress<decltype(&func)>(#func)