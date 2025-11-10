// Required for macOS.

#if defined(__APPLE__) && defined(__MACH__)
    #define SOKOL_IMPL
    #define SOKOL_METAL
    #define SOKOL_NO_ENTRY
    #define SOKOL_TRACE_HOOKS
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
    #define FONTSTASH_IMPLEMENTATION
    #if defined(_MSC_VER )
        #pragma warning(disable:4996)   // strncpy use in fontstash.h
    #endif
    #if defined(__GNUC__) || defined(__clang__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wunused-function"
        #pragma GCC diagnostic ignored "-Wsign-conversion"
    #endif
     #include "fontstash/src/fontstash.h"
    #if defined(__GNUC__) || defined(__clang__)
        #pragma GCC diagnostic pop
    #endif
    #define SOKOL_FONTSTASH_IMPL
    #include "sokol/util/sokol_fontstash.h"
#endif
