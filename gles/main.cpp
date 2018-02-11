//
//  Copyright (c) 8:18 AM mutter
//
//  This file is part of opengles.
//
//  opengles is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  opengles is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with opengles.  If not, see <http://www.gnu.org/licenses/>.
//

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

#include <iostream>
#include <iomanip>
#include <cstring>
#include <fstream>

#include <png.h>
#include "common.h"

using namespace std;

void eglCheckError();

void glCheckError() {
    std::cerr << "GLES error code " << std::hex << glGetError() << std::endl;
}

void XCheckError(Display* pDisplay, int errorCode);

void XEventLoop(Display* display);

void eglDraw();

void eglLoadShader(GLuint program, GLenum type, std::string file);

void eglLoadTexture(const char* file_path);

static EGLDisplay eglDisplay;
static EGLSurface eglSurface;

static GLuint program;
static GLuint vertexAttributePositionIndex = 0;
static GLuint vertexAttributeTexCoordIndex = 1;

static const GLfloat vertices[] = { 1.0f, 1.0f, 0.0f,
                                    1.0f, -1.0f, 0.0f,
                                    -1.0f, 1.0f, 0.0f,
                                    -1.0f, -1.0f, 0.0f };
static const GLfloat texcoords[] = { 1.0f, 1.0f,
                                     1.0f, 0.0f,
                                     0.0f, 1.0f,
                                     0.0f, 0.0f };

int main() {

    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
        std::cerr << "X open display error" << '\n';
        exit(EXIT_FAILURE);
    }

    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display),
                                        0, 0, 800, 640, 0,
                                        BlackPixel(display, DefaultScreen(display)),
                                        WhitePixel(display, DefaultScreen(display)));

    if (window == BadAlloc || window == BadColor || window == BadCursor
        || window == BadMatch || window == BadValue || window == BadWindow) {
        std::cerr << "X create simple window" << '\n';
        XCheckError(display, (int) window);
    }

    XSelectInput(display, window, ExposureMask | ButtonPressMask | KeyPressMask |
                                  KeymapStateMask | KeyReleaseMask | StructureNotifyMask);
    // XMapWindow will return BadRequest, don't understand why?
    // we just ignore the returned value
    XMapWindow(display, window);
    XStoreName(display, window, "VERY SIMPLE EGL APPLICATION");

    // below is very important, otherwise eglCreateWindowSurface will fail
    Atom wm_state = XInternAtom(display, "_NET_WM_STATE", false);
    XEvent xEvent;
    memset(&xEvent, 0, sizeof(xEvent));
    xEvent.type                 = ClientMessage;
    xEvent.xclient.window       = window;
    xEvent.xclient.message_type = wm_state;
    xEvent.xclient.format       = 32;
    xEvent.xclient.data.l[0]    = 1;
    xEvent.xclient.data.l[1]    = false;
    XSendEvent(
            display,
            DefaultRootWindow(display),
            false,
            SubstructureNotifyMask,
            &xEvent);

    // setup for EGL
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (eglDisplay == EGL_NO_DISPLAY) {
        std::cerr << "Unable to open connection to local window system" << '\n';
        eglCheckError();
        exit(EXIT_FAILURE);
    }

    EGLint major, minor;
    if (!eglInitialize(eglDisplay, &major, &minor)) {
        std::cerr << "Unable to initialize EGL" << '\n';
        eglCheckError();
    }

    EGLint configAttributeList[] {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, EGL_DONT_CARE,
            EGL_DEPTH_SIZE, EGL_DONT_CARE,
            EGL_STENCIL_SIZE, EGL_DONT_CARE,
            EGL_SAMPLE_BUFFERS, 0,
            EGL_NONE
    };

    const EGLint MAX_CONFIGS = 1;
    EGLConfig eglConfigs[MAX_CONFIGS];
    EGLint numConfigs;
    if (!eglChooseConfig(eglDisplay, configAttributeList, eglConfigs, MAX_CONFIGS, &numConfigs)) {
        std::cerr << "EGL choose config error" << std::endl;
        eglCheckError();
    } else {
        const EGLint windowSurfaceAttributeList[] {
                EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
                EGL_NONE
        };
        eglSurface = eglCreateWindowSurface(eglDisplay, eglConfigs[0], window, windowSurfaceAttributeList);
        if (eglSurface == EGL_NO_SURFACE) {
            std::cerr << "EGL create window surface error" << '\n';
            eglCheckError();
        }

        const EGLint contextAttributeList[] {
                EGL_CONTEXT_CLIENT_VERSION, 3,
                EGL_NONE
        };
        EGLContext eglContext = eglCreateContext(eglDisplay, eglConfigs[0], EGL_NO_CONTEXT, contextAttributeList);
        if (eglContext == EGL_NO_CONTEXT) {
            std::cerr << "EGL create context error" << '\n';
            eglCheckError();
        }

        if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
            std::cerr << "EGL make current context error" << '\n';
            eglCheckError();
        }

        program = glCreateProgram();
        if (program == 0) {
            std::cerr << "GLES create program" << std::endl;
            glCheckError();
        }

        const char* VERTEX_SHADER_PATH = "/home/mutter/ClionProjects/opengles/vertex.glsl";
        const char* FRAGMENT_SHADER_PATH = "/home/mutter/ClionProjects/opengles/fragment.glsl";

        eglLoadShader(program, GL_VERTEX_SHADER, VERTEX_SHADER_PATH);
        eglLoadShader(program, GL_FRAGMENT_SHADER, FRAGMENT_SHADER_PATH);

        glLinkProgram(program);

        GLint linked;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) {
            std::cerr << "GLES link program" << std::endl;
            glCheckError();
            glDeleteProgram(program);
        }

        glUseProgram(program);
        glBindAttribLocation(program, vertexAttributePositionIndex, "aPosition");
        glBindAttribLocation(program, vertexAttributeTexCoordIndex, "aTexCoord");
        png_texture_load("/home/mutter/186050022.png", NULL, NULL);
    }

    XEventLoop(display);
    return 0;
}

void XEventLoop(Display* display) {
    while (1) {
        XEvent event;
        XNextEvent(display, &event);
        switch (event.type) {
            case KeymapNotify:
                break;
            case KeyRelease:
                std::cerr << "key release" << '\n';
                break;
            case ButtonPress:
                XCloseDisplay(display);
                return;
            default:
                break;
        }

        eglDraw();
    }
}

void XCheckError(Display* pDisplay, int errorCode) {
    char error_message[2048] = { '0' };
    XGetErrorText(pDisplay, errorCode, error_message, sizeof(error_message));
    std::cerr << error_message << std::endl;
}

void eglCheckError() {
    std::cerr << "EGL error code " << std::hex << eglGetError() << std::endl;
}

void eglLoadTexture(const char* file_path) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    png_structp pngStructp = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop pngInfop = png_create_info_struct(pngStructp);
    FILE* file = fopen(file_path, "r");
    png_init_io(pngStructp, file);
    png_read_info(pngStructp, pngInfop);
    png_bytep* row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * pngInfop->height);

    for (png_uint_32 y = 0; y < pngInfop->height; y++) {
        row_pointers[y] = (png_bytep) malloc(png_get_rowbytes(pngStructp, pngInfop));
    }

    std::cout << "image wxh " << pngInfop->width << "x" << pngInfop->height << std::endl;
    png_read_image(pngStructp, row_pointers);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei) pngInfop->width, (GLsizei) pngInfop->height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, row_pointers);
    fclose(file);
}

void eglLoadShader(GLuint program, GLenum type, std::string file) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        std::cerr << "GLES create shader" << std::endl;
        glCheckError();
        return;
    }
    ifstream shaderStream;
    shaderStream.open(file);
    char buffer[8192 * 10] = { 0 };
    if (shaderStream.is_open()) {
        shaderStream.read(buffer, sizeof(buffer));
    } else {
        std::cerr << "open " << file << " error\n";
    }

    char* shaderStr = buffer;
    glShaderSource(shader, 1, (const GLchar* const*) (&shaderStr), NULL);
    glCompileShader(shader);

    GLint compile;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile);
    if (!compile) {
        std::cerr << "GLES compile shader" << std::endl;
        glCheckError();
        glDeleteShader(shader);
        return;
    }
    glAttachShader(program, shader);

    shaderStream.close();
}

void eglDraw() {
    glClear(GL_COLOR_BUFFER_BIT);

    glVertexAttribPointer(vertexAttributePositionIndex, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(vertexAttributePositionIndex);
    glVertexAttribPointer(vertexAttributeTexCoordIndex, 2, GL_FLOAT, GL_FALSE, 0, texcoords);
    glEnableVertexAttribArray(vertexAttributeTexCoordIndex);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    eglSwapBuffers(eglDisplay, eglSurface);
}
