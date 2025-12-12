/**************************************************************************************************
 * @file glproc_loader.hpp
 * @brief glutGetProcAddress
 * @details
 * 
 *  HISTORY
 *  -----------------------------------------------------------------------------------------------
 *  Version   Date        Author    Description
 *  -----------------------------------------------------------------------------------------------
 *  0.1       2023-04-19  TDZ       Initial.
 * 
**************************************************************************************************/

#ifndef GLPROCLOADER_HPP
#define GLPROCLOADER_HPP

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stddef.h>

#define GL_GLEXT_PROTOTYPES

#ifdef GL_ES_TDA
    #include <EGL/egl.h>
    #include <EGL/eglext.h>
    #include <GLES3/gl32.h>
    #include <GLES2/gl2ext.h>
    #ifndef GL_GEOMETRY_SHADER
        #define GL_GEOMETRY_SHADER GL_GEOMETRY_SHADER_OES
    #endif // GL_GEOMETRY_SHADER
#endif //GL_ES_TDA

namespace BaseLib{

// 声明OpenGL函数指针变量
#ifdef GL_ES_TDA
// Android平台上的OpenGL函数指针声明
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLDETACHSHADERPROC glDetachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLUNIFORM3FVPROC glUniform3fv;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLGETINTERNALFORMATIVPROC glGetInternalformativ;
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
extern PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
extern PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
extern PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
extern PFNGLINVALIDATEFRAMEBUFFERPROC glInvalidateFramebuffer;
extern PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
#endif // GL_ES_TDA

class GLProcLoader
{
    private:
    GLProcLoader();

    public:
    static void InitExtensionEntries(void)
    {
#ifdef GL_ES_TDA
        // 在Android平台上，大多数OpenGL ES 3.x函数已经由系统提供，不需要手动加载
        // 如果需要加载扩展函数，应该使用eglGetProcAddress
        // 例如：glRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)eglGetProcAddress("glRenderbufferStorageMultisampleEXT");
        glGenerateMipmap          = (PFNGLGENERATEMIPMAPPROC)eglGetProcAddress("glGenerateMipmap");
        glCreateShader            = (PFNGLCREATESHADERPROC)eglGetProcAddress("glCreateShader");
        glShaderSource            = (PFNGLSHADERSOURCEPROC)eglGetProcAddress("glShaderSource");
        glCompileShader           = (PFNGLCOMPILESHADERPROC)eglGetProcAddress("glCompileShader");
        glCreateProgram           = (PFNGLCREATEPROGRAMPROC)eglGetProcAddress("glCreateProgram");
        glAttachShader            = (PFNGLATTACHSHADERPROC)eglGetProcAddress("glAttachShader");
        glDetachShader            = (PFNGLDETACHSHADERPROC)eglGetProcAddress("glDetachShader");
        glLinkProgram             = (PFNGLLINKPROGRAMPROC)eglGetProcAddress("glLinkProgram");
        glDeleteShader            = (PFNGLDELETESHADERPROC)eglGetProcAddress("glDeleteShader");
        glUseProgram              = (PFNGLUSEPROGRAMPROC)eglGetProcAddress("glUseProgram");
        glGetShaderiv             = (PFNGLGETSHADERIVPROC)eglGetProcAddress("glGetShaderiv");
        glGetShaderInfoLog        = (PFNGLGETSHADERINFOLOGPROC)eglGetProcAddress("glGetShaderInfoLog");
        glGetProgramiv            = (PFNGLGETPROGRAMIVPROC)eglGetProcAddress("glGetProgramiv");
        glGetProgramInfoLog       = (PFNGLGETPROGRAMINFOLOGPROC)eglGetProcAddress("glGetProgramInfoLog");
        glGenVertexArrays         = (PFNGLGENVERTEXARRAYSPROC)eglGetProcAddress("glGenVertexArrays");
        glGenBuffers              = (PFNGLGENBUFFERSPROC)eglGetProcAddress("glGenBuffers");
        glDeleteBuffers           = (PFNGLDELETEBUFFERSPROC)eglGetProcAddress("glDeleteBuffers");
        glBindVertexArray         = (PFNGLBINDVERTEXARRAYPROC)eglGetProcAddress("glBindVertexArray");
        glDeleteVertexArrays      = (PFNGLDELETEVERTEXARRAYSPROC)eglGetProcAddress("glDeleteVertexArrays");
        glBindBuffer              = (PFNGLBINDBUFFERPROC)eglGetProcAddress("glBindBuffer");
        glBufferData              = (PFNGLBUFFERDATAPROC)eglGetProcAddress("glBufferData");
        glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)eglGetProcAddress("glEnableVertexAttribArray");
        glVertexAttribPointer     = (PFNGLVERTEXATTRIBPOINTERPROC)eglGetProcAddress("glVertexAttribPointer");
        glVertexAttribIPointer    = (PFNGLVERTEXATTRIBIPOINTERPROC)eglGetProcAddress("glVertexAttribIPointer");
        glUniformMatrix4fv        = (PFNGLUNIFORMMATRIX4FVPROC)eglGetProcAddress("glUniformMatrix4fv");
        glGetUniformLocation      = (PFNGLGETUNIFORMLOCATIONPROC)eglGetProcAddress("glGetUniformLocation");
        glActiveTexture           = (PFNGLACTIVETEXTUREPROC)eglGetProcAddress("glActiveTexture");
        glUniform1i               = (PFNGLUNIFORM1IPROC)eglGetProcAddress("glUniform1i");
        glUniform3fv              = (PFNGLUNIFORM3FVPROC)eglGetProcAddress("glUniform3fv");
        glUniform1f               = (PFNGLUNIFORM1FPROC)eglGetProcAddress("glUniform1f");
        glGetInternalformativ     = (PFNGLGETINTERNALFORMATIVPROC)eglGetProcAddress("glGetInternalformativ");
        glGenFramebuffers         = (PFNGLGENFRAMEBUFFERSPROC)eglGetProcAddress("glGenFramebuffers");
        glGenRenderbuffers        = (PFNGLGENRENDERBUFFERSPROC)eglGetProcAddress("glGenRenderbuffers");
        glBindBuffer              = (PFNGLBINDBUFFERPROC)eglGetProcAddress("glBindBuffer");
        glBindFramebuffer         = (PFNGLBINDFRAMEBUFFERPROC)eglGetProcAddress("glBindFramebuffer");
        glFramebufferTexture2D    = (PFNGLFRAMEBUFFERTEXTURE2DPROC)eglGetProcAddress("glFramebufferTexture2D");
        glBindRenderbuffer        = (PFNGLBINDRENDERBUFFERPROC)eglGetProcAddress("glBindRenderbuffer");
        glRenderbufferStorage     = (PFNGLRENDERBUFFERSTORAGEPROC)eglGetProcAddress("glRenderbufferStorage");
        glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)eglGetProcAddress("glFramebufferRenderbuffer");
        glCheckFramebufferStatus  = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)eglGetProcAddress("glCheckFramebufferStatus");
        glDeleteRenderbuffers     = (PFNGLDELETERENDERBUFFERSPROC)eglGetProcAddress("glDeleteRenderbuffers");
        glDeleteFramebuffers      = (PFNGLDELETEFRAMEBUFFERSPROC)eglGetProcAddress("glDeleteFramebuffers");
        glInvalidateFramebuffer   = (PFNGLINVALIDATEFRAMEBUFFERPROC)eglGetProcAddress("glInvalidateFramebuffer");
        glBlitFramebuffer         = (PFNGLBLITFRAMEBUFFERPROC)eglGetProcAddress("glBlitFramebuffer");
        glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)eglGetProcAddress("glRenderbufferStorageMultisample");
        // PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample    = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)glutGetProcAddress("glTexImage2DMultisample");
#endif // GL_ES_TDA
    }

};

};
#endif /* GLPROCLOADER_HPP */
