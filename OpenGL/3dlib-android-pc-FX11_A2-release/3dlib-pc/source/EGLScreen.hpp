/**************************************************************************************************
 * @file EGLScreen.hpp
 * @brief egl screen display
 * @version 0.1
 * @date 2023-11-16
 * 
 * @copyright Copyright (c) 2023
 * 
**************************************************************************************************/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl32.h>
#include <GLES2/gl2ext.h>

#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>

namespace BaseLib 
{

    class EGLScreen
    {
    public:
        EGLScreen():surfaceWidth(0),surfaceHeight(0),width(0),height(0)
        {

        }

        ~EGLScreen()
        {
            eglMakeCurrent(this->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglDestroySurface(this->eglDisplay, this->eglSurface);
            eglDestroyContext(this->eglDisplay, this->eglContext);
            eglTerminate(this->eglDisplay);
            eglReleaseThread();

        }


        int Initialization(int argc, char **argv)
        {
            if (argc == 1)
            {
                width  = 1800;
                height = 720;
            }

            if (argc == 3)
            {
                width  = std::stoi(argv[1]);
                height = std::stoi(argv[2]);
            }

            this->width  = width;
            this->height = height;

            printf("setting wind width=%d, height=%d\n", this->width, this->height);

            EGLBoolean rc = WinCreate();

            if (rc == EGL_TRUE)
            {
                rc = this->InitEGL();
            }

            if (rc == EGL_TRUE)
            {
                eglQuerySurface(this->eglDisplay, this->eglSurface, EGL_WIDTH, &this->surfaceWidth);
	            eglQuerySurface(this->eglDisplay, this->eglSurface, EGL_HEIGHT, &this->surfaceHeight);

                printf("surface width=%d, height=%d\n", this->surfaceWidth, this->surfaceHeight);

                const GLubyte* info = glGetString(GL_VENDOR);
                printf("GL_VENDOR:%s\n", info);

                info = glGetString(GL_VERSION);
                printf("GL_VERSION:%s\n", info);

                info = glGetString(GL_RENDERER);
                printf("GL_RENDER:%s\n", info);

                info = glGetString(GL_SHADING_LANGUAGE_VERSION);
                printf("GL_SHADING_LANGUAGE_VERSION:%s\n", info);

                info = glGetString(GL_EXTENSIONS);
                printf("GL_EXTENSIONS:%s\n", info);

                int count = 0;
                glGetIntegerv(GL_MAX_TEXTURE_SIZE, &count);
                printf("GL_MAX_TEXTURE_SIZE=%d\n", count);

                glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &count);
                printf("GL_MAX_TEXTURE_IMAGE_UNITS=%d\n", count);
            }
           
            return rc;
        }

      
        void SwapBuffers()
        {
            int rc = eglSwapBuffers(this->eglDisplay, this->eglSurface);
            if (rc != EGL_TRUE) {
                fprintf(stderr, "eglSwapBuffers failed\n");
            }
        }


    private:
       
        int InitEGL(void)
        {
            EGLBoolean rc;
            EGLint majorVersion;
            EGLint minorVersion;
            EGLConfig egl_conf = (EGLConfig)0;
            EGLint num_confs = 0;

            const EGLint egl_ctx_attr[] = {
                EGL_CONTEXT_CLIENT_VERSION, 3,
                EGL_NONE
            };

            const EGLint egl_attrib_list[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_DEPTH_SIZE, 16,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                EGL_NONE
            };

            this->eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            if (this->eglDisplay == EGL_NO_DISPLAY) {
                fprintf(stderr, "eglGetDisplay failed\n");
                return EGL_FALSE;
            }

            rc = eglInitialize(this->eglDisplay, &majorVersion, &minorVersion);
            printf("majorVersion=%d, minorVersion=%d\n", majorVersion, minorVersion);
            if (rc != EGL_TRUE) {
                fprintf(stderr, "eglInitialize failed\n");
                return EGL_FALSE;
            }

            if (!eglBindAPI(EGL_OPENGL_ES_API)) {
                printf("EGL: ERROR: eglBindAPI(EGL_OPENGL_ES_API) failed !!!\n");
                return EGL_FALSE;
            }

            rc = eglChooseConfig(this->eglDisplay, egl_attrib_list, &egl_conf, 1, &num_confs);
            if ((rc != EGL_TRUE) || (num_confs == 0)) {
                fprintf(stderr, "eglChooseConfig failed\n");
                return EGL_FALSE;
            }

            this->eglContext = eglCreateContext(this->eglDisplay, egl_conf, EGL_NO_CONTEXT, (EGLint*)&egl_ctx_attr);
            if (this->eglContext == EGL_NO_CONTEXT) {
                fprintf(stderr, "eglCreateContext failed\n");
                return EGL_FALSE;
            }

            //Create the EGL surface from the screen window
            this->eglSurface = eglCreateWindowSurface(this->eglDisplay, egl_conf, this->eglNativeWindow, NULL);
            if (this->eglSurface == EGL_NO_SURFACE) {
                fprintf(stderr, "eglCreateWindowSurface failed\n");
                return EGL_FALSE;
            }

            rc = eglMakeCurrent(this->eglDisplay, this->eglSurface, this->eglSurface, this->eglContext);
            if (rc != EGL_TRUE) {
                fprintf(stderr, "eglMakeCurrent failed\n");
                return EGL_FALSE;
            }

            return EGL_TRUE;
        }

    private:

        EGLBoolean WinCreate()
        {

            Window root;
            XSetWindowAttributes swa;
            XSetWindowAttributes  xattr;
            Atom wm_state;
            XWMHints hints;
            XEvent xev;
            EGLConfig ecfg;
            EGLint num_config;
            Window win;

            /*
            * X11 native display initialization
            */

            x_display = XOpenDisplay(NULL);
            if ( x_display == NULL )
            {
                return EGL_FALSE;
            }

            root = DefaultRootWindow(x_display);

            swa.event_mask  =  ExposureMask | PointerMotionMask | KeyPressMask;
            win = XCreateWindow(
                    x_display, root,
                    0, 0, this->width, this->height, 0,
                    CopyFromParent, InputOutput,
                    CopyFromParent, CWEventMask,
                    &swa );
            s_wmDeleteMessage = XInternAtom(x_display, "WM_DELETE_WINDOW", False);
            XSetWMProtocols(x_display, win, &s_wmDeleteMessage, 1);

            xattr.override_redirect = EGL_FALSE;
            XChangeWindowAttributes ( x_display, win, CWOverrideRedirect, &xattr );

            hints.input = EGL_TRUE;
            hints.flags = InputHint;
            XSetWMHints(x_display, win, &hints);

            // make the window visible on the screen
            XMapWindow (x_display, win);
            XStoreName (x_display, win, title.c_str());

            // get identifiers for the provided atom name strings
            wm_state = XInternAtom (x_display, "_NET_WM_STATE", EGL_FALSE);

            memset ( &xev, 0, sizeof(xev) );
            xev.type                 = ClientMessage;
            xev.xclient.window       = win;
            xev.xclient.message_type = wm_state;
            xev.xclient.format       = 32;
            xev.xclient.data.l[0]    = 1;
            xev.xclient.data.l[1]    = EGL_FALSE;
            XSendEvent (
                x_display,
                DefaultRootWindow ( x_display ),
                EGL_FALSE,
                SubstructureNotifyMask,
                &xev );

            this->eglNativeWindow  = (EGLNativeWindowType) win;
            this->eglNativeDisplay = (EGLNativeDisplayType) x_display;
            return EGL_TRUE;
        }

    public:
        EGLint surfaceWidth;
        EGLint surfaceHeight;
        std::string title;

        Display *x_display;
        Atom s_wmDeleteMessage;

    private:
        /// Window width
        GLint       width;
        /// Window height
        GLint       height;

        /// Display handle
        EGLNativeDisplayType eglNativeDisplay;

        /// Window handle
        EGLNativeWindowType  eglNativeWindow;

        /// EGL display
        EGLDisplay  eglDisplay;

        /// EGL context
        EGLContext  eglContext;

        /// EGL surface
        EGLSurface  eglSurface;

    };

}
