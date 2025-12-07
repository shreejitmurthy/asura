#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#endif

#define SOKOL_IMPL

#if defined(_WIN32) || defined(_WIN64)  // Windows
    #define SOKOL_D3D11
#elif defined(__linux__)  // Linux
    #define SOKOL_GLCORE
#endif

#define SOKOL_NO_ENTRY
#define SOKOL_TRACE_HOOKS

#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_time.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_glue.h"
#define SOKOL_DEBUGTEXT_IMPL
#include "sokol/util/sokol_debugtext.h"
#define SOKOL_GL_IMPL
#include "sokol/util/sokol_gl.h"
