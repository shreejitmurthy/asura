// Required for macOS.

#if defined(__APPLE__) && defined(__MACH__)
    /*
     * We'll include all relevant sokol headers even though we don't use some of them in code.
     * This is so we doesn't have to do this step, and can just #include <sokol_app.h> if we use it for
     * windowing.
     */
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
#endif
