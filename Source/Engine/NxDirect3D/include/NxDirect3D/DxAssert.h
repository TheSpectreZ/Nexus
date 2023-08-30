#pragma once
#include "NxCore/Assertion.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl.h>
#include <system_error>

template<typename T>
using Cptr = Microsoft::WRL::ComPtr<T>;

static HRESULT _HSR;

#define CLEAR_HSR _HSR = S_OK
#define CHECK_LOG_HSR NEXUS_ASSERT( (_HSR < 0),"HResult: %s",std::system_category().message(_HSR).c_str()); CLEAR_HSR
#define CHECK_CALL(func) _HSR = func; CHECK_LOG_HSR
#define CHECK_HANDLEX(handle,type) CHECK_LOG_HSR; NEXUS_ASSERT( (handle == nullptr),"%s:%s is Null",#type,#handle)