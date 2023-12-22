#pragma once
#ifdef _WIN32
#ifdef BUILD_DLL
#define DLL_API __declspec(dllimport)
#else
#define DLL_API __declspec(dllexport)
#endif
#else
#define DLL_API
#endif