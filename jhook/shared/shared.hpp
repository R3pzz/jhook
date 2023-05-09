#pragma once

#ifndef USE_CUSTOM_ENVS
#	define USE_CUSTOM_ENVS 0
#endif

#ifndef SUPPORT_ASM_INLINING
# define SUPPORT_ASM_INLINING 0
#endif

#ifndef USE_IMGUI_PROVIDER
# define USE_IMGUI_PROVIDER 1
#endif

#include <array>
#include <cstdint>
#include <string>
#include <thread>
#include <functional>
#include <type_traits>
#include <omp_llvm.h>
#include <windows.h>
#include <jni.h>

#include <libs/glm/glm.hpp>
#include <libs/minhook/minhook.h>
#if USE_IMGUI_PROVIDER
#include <libs/imgui/imgui.h>
#include <libs/imgui/imgui_impl_win32.h>
#include <libs/imgui/imgui_impl_opengl2.h>
#endif
#if !USE_ASM_INLINING
#include <libs/asmjit/asmjit.h>
#define __ a.
#define JIT_START \
using namespace asmjit; \
 \
JitRuntime jrt{}; \
CodeHolder code{}; \
x86::Assembler a{ &code }; \
 \
code.init( jrt.environment( ) );
#endif

namespace shared {
	using ulong_t = unsigned long;
}

#include "mem/address.hpp"
#include "mem/native.hpp"
#include "mem/bytes.hpp"

#include "java/miscObjects.hpp"
#include "java/oopHandle.hpp"
#include "java/constPool.hpp"
#include "java/javaMethod.hpp"
#include "java/instanceKlass.hpp"
#if USE_CUSTOM_ENVS
//#include "java/nativeThread.hpp"
//#include "java/javaThread.hpp"
//#include "java/threadList.hpp"
//#include "java/threadLocalStorage.hpp"
#endif

#include "java/jniWrangler.hpp"

#include "util/dllAwaiter.hpp"
#include "util/javaMethodHook.hpp"
#include "util/nativeMethodHook.hpp"

#include "runtime/runtimeContext.hpp"

#include "renderer/color.hpp"
#include "renderer/rendererContext.hpp"

namespace shd = shared;