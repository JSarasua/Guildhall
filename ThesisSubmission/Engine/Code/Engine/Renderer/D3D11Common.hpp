#pragma once
#define RENDER_DEBUG
#define DX_SAFE_RELEASE(ptr) if(nullptr != ptr) {ptr->Release(); ptr = nullptr;}


#define INITGUID
#include <d3d11.h>  // d3d11 specific objects
#include <dxgi.h>   // shared library used across multiple dx graphical interfaces
#include <dxgidebug.h>  // debug utility (mostly used for reporting and analytics)
