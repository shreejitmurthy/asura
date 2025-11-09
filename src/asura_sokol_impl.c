#define SOKOL_IMPL

#if defined(_WIN32) || defined(_WIN64)  // Windows
    #define SOKOL_D3D11
#elif defined(__linux__)  // Linux
    #define SOKOL_GLCORE
#endif

#define SOKOL_NO_ENTRY
#define SOKOL_TRACE_HOOKS

// #define FONTSTASH_IMPLEMENTATION
// #include "fontstash.h"

#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_time.h"
// #include "sokol/sokol_audio.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_glue.h"
#define SOKOL_DEBUGTEXT_IMPL
#include "sokol/util/sokol_debugtext.h"
#define SOKOL_GL_IMPL
#include "sokol/util/sokol_gl.h"
// #define SOKOL_FONTSTASH_IMPL
// #include "sokol/util/sokol_fontstash.h"