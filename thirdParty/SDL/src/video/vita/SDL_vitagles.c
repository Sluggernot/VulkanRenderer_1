/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2023 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_VITA && SDL_VIDEO_VITA_PIB
#include <stdlib.h>
#include <string.h>

#include "SDL_error.h"
#include "SDL_log.h"
#include "SDL_vitavideo.h"
#include "SDL_vitagles_c.h"

/*****************************************************************************/
/* SDL OpenGL/OpenGL ES functions                                            */
/*****************************************************************************/
#define EGLCHK(stmt)                           \
    do {                                       \
        EGLint err;                            \
                                               \
        stmt;                                  \
        err = eglGetError();                   \
        if (err != EGL_SUCCESS) {              \
            SDL_SetError("EGL error %d", err); \
            return 0;                          \
        }                                      \
    } while (0)

void VITA_GLES_KeyboardCallback(ScePigletPreSwapData *data)
{
    SceCommonDialogUpdateParam commonDialogParam;
    SDL_zero(commonDialogParam);
    commonDialogParam.renderTarget.colorFormat = data->colorFormat;
    commonDialogParam.renderTarget.surfaceType = data->surfaceType;
    commonDialogParam.renderTarget.colorSurfaceData = data->colorSurfaceData;
    commonDialogParam.renderTarget.depthSurfaceData = data->depthSurfaceData;
    commonDialogParam.renderTarget.width = data->width;
    commonDialogParam.renderTarget.height = data->height;
    commonDialogParam.renderTarget.strideInPixels = data->strideInPixels;
    commonDialogParam.displaySyncObject = data->displaySyncObject;

    sceCommonDialogUpdate(&commonDialogParam);
}

int VITA_GLES_LoadLibrary(_THIS, const char *path)
{
    pibInit(PIB_SHACCCG | PIB_GET_PROC_ADDR_CORE);
    return 0;
}

void *VITA_GLES_GetProcAddress(_THIS, const char *proc)
{
    return eglGetProcAddress(proc);
}

void VITA_GLES_UnloadLibrary(_THIS)
{
    eglTerminate(_this->gl_data->display);
}

static EGLint width = 960;
static EGLint height = 544;

SDL_GLContext VITA_GLES_CreateContext(_THIS, SDL_Window *window)
{

    SDL_WindowData *wdata = (SDL_WindowData *)window->driverdata;

    EGLint attribs[32];
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
    EGLConfig config;
    EGLint num_configs;
    PFNEGLPIGLETVITASETPRESWAPCALLBACKSCEPROC preSwapCallback;
    int i;

    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLCHK(display = eglGetDisplay(0));

    EGLCHK(eglInitialize(display, NULL, NULL));
    wdata->uses_gles = SDL_TRUE;
    window->flags |= SDL_WINDOW_FULLSCREEN;

    EGLCHK(eglBindAPI(EGL_OPENGL_ES_API));

    i = 0;
    attribs[i++] = EGL_RED_SIZE;
    attribs[i++] = 8;
    attribs[i++] = EGL_GREEN_SIZE;
    attribs[i++] = 8;
    attribs[i++] = EGL_BLUE_SIZE;
    attribs[i++] = 8;
    attribs[i++] = EGL_DEPTH_SIZE;
    attribs[i++] = 0;
    attribs[i++] = EGL_ALPHA_SIZE;
    attribs[i++] = 8;
    attribs[i++] = EGL_STENCIL_SIZE;
    attribs[i++] = 0;

    attribs[i++] = EGL_SURFACE_TYPE;
    attribs[i++] = 5;

    attribs[i++] = EGL_RENDERABLE_TYPE;
    attribs[i++] = EGL_OPENGL_ES2_BIT;

    attribs[i++] = EGL_CONFORMANT;
    attribs[i++] = EGL_OPENGL_ES2_BIT;

    attribs[i++] = EGL_NONE;

    EGLCHK(eglChooseConfig(display, attribs, &config, 1, &num_configs));

    if (num_configs == 0) {
        SDL_SetError("No valid EGL configs for requested mode");
        return 0;
    }

    EGLCHK(surface = eglCreateWindowSurface(display, config, VITA_WINDOW_960X544, NULL));

    EGLCHK(context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs));

    EGLCHK(eglMakeCurrent(display, surface, surface, context));

    EGLCHK(eglQuerySurface(display, surface, EGL_WIDTH, &width));
    EGLCHK(eglQuerySurface(display, surface, EGL_HEIGHT, &height));

    _this->gl_data->display = display;
    _this->gl_data->context = context;
    _this->gl_data->surface = surface;

    preSwapCallback = (PFNEGLPIGLETVITASETPRESWAPCALLBACKSCEPROC)eglGetProcAddress("eglPigletVitaSetPreSwapCallbackSCE");
    preSwapCallback(VITA_GLES_KeyboardCallback);

    return context;
}

int VITA_GLES_MakeCurrent(_THIS, SDL_Window *window, SDL_GLContext context)
{
    if (!eglMakeCurrent(_this->gl_data->display, _this->gl_data->surface,
                        _this->gl_data->surface, _this->gl_data->context)) {
        return SDL_SetError("Unable to make EGL context current");
    }
    return 0;
}

int VITA_GLES_SetSwapInterval(_THIS, int interval)
{
    EGLBoolean status;
    status = eglSwapInterval(_this->gl_data->display, interval);
    if (status == EGL_TRUE) {
        /* Return success to upper level */
        _this->gl_data->swapinterval = interval;
        return 0;
    }
    /* Failed to set swap interval */
    return SDL_SetError("Unable to set the EGL swap interval");
}

int VITA_GLES_GetSwapInterval(_THIS)
{
    return _this->gl_data->swapinterval;
}

int VITA_GLES_SwapWindow(_THIS, SDL_Window *window)
{
    if (!eglSwapBuffers(_this->gl_data->display, _this->gl_data->surface)) {
        return SDL_SetError("eglSwapBuffers() failed");
    }
    return 0;
}

void VITA_GLES_DeleteContext(_THIS, SDL_GLContext context)
{
    SDL_VideoData *phdata = (SDL_VideoData *)_this->driverdata;
    EGLBoolean status;

    if (phdata->egl_initialized != SDL_TRUE) {
        SDL_SetError("VITA: GLES initialization failed, no OpenGL ES support");
        return;
    }

    /* Check if OpenGL ES connection has been initialized */
    if (_this->gl_data->display != EGL_NO_DISPLAY) {
        if (context != EGL_NO_CONTEXT) {
            status = eglDestroyContext(_this->gl_data->display, context);
            if (status != EGL_TRUE) {
                /* Error during OpenGL ES context destroying */
                SDL_SetError("VITA: OpenGL ES context destroy error");
                return;
            }
        }
    }

    return;
}

#endif /* SDL_VIDEO_DRIVER_VITA */

/* vi: set ts=4 sw=4 expandtab: */
