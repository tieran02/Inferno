#pragma once
#include "core/Assert.h"

#define VerifySuccess(hr) INF_VERIFY(SUCCEEDED(hr), "D3D12 Call did not succeed");