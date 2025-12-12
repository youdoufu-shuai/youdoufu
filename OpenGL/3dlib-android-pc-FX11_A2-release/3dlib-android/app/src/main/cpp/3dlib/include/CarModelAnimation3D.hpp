/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   CarModelAnimation3D.hpp
 * Author: tron
 *
 * Created on 2023年5月6日, 下午4:59
 */

#pragma once


#ifdef HEADER_SPLIT
    #include "carAnimation3D/CarModel3D.hpp"
#endif // HEADER_SPLIT


//#ifndef HEADER_SPLIT

    /**************************************************************************************************
     * @brief COMMON INCLUDE AND MACROS
     *
    **************************************************************************************************/
    #include <map>
    #include <cstdlib>
    #include <iostream>
    #include <fstream>
    #include <sstream>
    #include <vector>
    #include <cstdio>
    #include <string>
    #include <memory>
    #include <sys/time.h>
    #include <ctime>

    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>

    #include <assimp/Importer.hpp>
    #include <assimp/scene.h>
    #include <assimp/postprocess.h>
    #include <assimp/cimport.h>


#if defined(USE_GLES32)
    #include <EGL/egl.h>
    #include <EGL/eglext.h>
    #include <GLES3/gl32.h>
    #include <GLES2/gl2ext.h>


    #define EBO_INDICES_TYPE unsigned int
    #define EBO_GL_TYPE      GL_UNSIGNED_INT

#elif defined(GL_ES_TDA)
    #include <EGL/egl.h>
    #include <EGL/eglext.h>
    #include <GLES3/gl32.h>
    #include <GLES2/gl2ext.h>
    #ifndef GL_GEOMETRY_SHADER
        #define GL_GEOMETRY_SHADER GL_GEOMETRY_SHADER_OES
    #endif // GL_GEOMETRY_SHADER


    #define EBO_INDICES_TYPE unsigned int
    #define EBO_GL_TYPE      GL_UNSIGNED_INT
#elif defined(USE_GLES20)
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
    #include <EGL/eglext.h>

    #ifndef glGenVertexArrays
        #define glGenVertexArrays glGenVertexArraysOES
    #endif // glGenVertexArrays

    #ifndef glBindVertexArray
        #define glBindVertexArray glBindVertexArrayOES
    #endif // glBindVertexArray

    #ifndef glDeleteVertexArrays
        #define glDeleteVertexArrays glDeleteVertexArraysOES
    #endif // glDeleteVertexArrays

    #ifndef GL_GEOMETRY_SHADER
        #define GL_GEOMETRY_SHADER GL_GEOMETRY_SHADER_EXT
    #endif // GL_GEOMETRY_SHADER

    #ifndef GL_RGBA8
        #define GL_RGBA8 GL_RGBA8_OES
    #endif // GL_RGBA8

    #ifndef GL_RED
        #define GL_RED GL_RED_EXT
    #endif // GL_RED

    #ifndef GL_TEXTURE_WRAP_R
        #define GL_TEXTURE_WRAP_R GL_TEXTURE_WRAP_R_OES
    #endif

    #ifndef GL_RGBA16F
        #define GL_RGBA16F GL_RGBA16F_EXT
    #endif

    #ifndef GL_RGB16F
        #define GL_RGB16F GL_RGB16F_EXT
    #endif

    #ifndef GL_R16F
        #define GL_R16F GL_R16F_EXT
    #endif

    #ifndef GL_DEPTH24_STENCIL8
        #define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
    #endif // GL_DEPTH24_STENCIL8

    #ifndef GL_MAX_SAMPLES
        #define GL_MAX_SAMPLES GL_MAX_SAMPLES_EXT
    #endif // GL_MAX_SAMPLES


    #define EBO_INDICES_TYPE unsigned short
    #define EBO_GL_TYPE      GL_UNSIGNED_SHORT
#else
    #include <GL/glew.h>
    #include <GL/freeglut.h>


    #define EBO_INDICES_TYPE unsigned int
    #define EBO_GL_TYPE      GL_UNSIGNED_INT
#endif


    #define GLX_AA_BLEND_ENABLE() glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    #define GLX_AA_BLEND_DISABLE() glDisable(GL_BLEND);

//
//    /**************************************************************************************************
//     * @brief HEADER FOR LOGGER
//     *
//    **************************************************************************************************/
//    namespace BaseLib{
//
//    #if defined(BUILD_TARGET_OS_ANDROID)
//        #if defined(USE_ANDROID_LOG)
//            #include<android/log.h>
//            #define LOG_TAG "libCarAnimation3D.so"
//            #define LOGGER_E(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
//            #define LOGGER_V(...)  __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG,__VA_ARGS__)
//            #define LOGGER_D(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
//            #define LOGGER_I(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
//
//            #define LOGGER_GLE(...) {GLint error;for (error = glGetError(); error; error = glGetError()) {LOGGER_E("[GL error] glError(0x%x):", error); __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__);}}
//        #else
//            #define LOGGER_E(...)
//            #define LOGGER_V(...)
//            #define LOGGER_D(...)
//            #define LOGGER_I(...)
//
//            #define LOGGER_GLE(...)
//
//            // #define LOGGER_E(format, ...) {printf("[error] @file:%s @line:%d @function:%s ", __BASE_FILE__, __LINE__, __FUNCTION__);printf(format, ##__VA_ARGS__);printf("\n");}
//            // #define LOGGER_I(format, ...) {printf("[ info] ");printf(format, ##__VA_ARGS__);printf("\n");}
//            // #define LOGGER_D(format, ...) {printf("[debug] @file:%s @line:%d @function:%s ", __BASE_FILE__, __LINE__, __FUNCTION__);printf(format, ##__VA_ARGS__);printf("\n");}
//
//            // #define LOGGER_GLE(format, ...) {GLint error;for (error = glGetError(); error; error = glGetError()) {printf("[GL error] glError(0x%x):", error); printf(format, ##__VA_ARGS__);printf("\n");}}
//       #endif
//    #else
//        #define LOGGER_E(format, ...) {printf("[error] @file:%s @line:%d @function:%s ", __BASE_FILE__, __LINE__, __FUNCTION__);printf(format, ##__VA_ARGS__);printf("\n");}
//        #define LOGGER_I(format, ...) {printf("[ info] ");printf(format, ##__VA_ARGS__);printf("\n");}
//        #define LOGGER_D(format, ...) {printf("[debug] @file:%s @line:%d @function:%s ", __BASE_FILE__, __LINE__, __FUNCTION__);printf(format, ##__VA_ARGS__);printf("\n");}
//
//        #define LOGGER_GLE(format, ...) {GLint error;for (error = glGetError(); error; error = glGetError()) {printf("[GL error] glError(0x%x):", error); printf(format, ##__VA_ARGS__);printf("\n");}}
//    #endif // BUILD_TARGET_OS_ANDROID
//    }
//
//    /**************************************************************************************************
//     * @brief HEADER FOR GLHelper
//     *
//    **************************************************************************************************/
//    namespace BaseLib
//    {
//        struct Framebuffer
//        {
//            /** @brief input */
//            GLuint colorFormat          = GL_RGBA8;              ///< GL_RGBA8
//            GLuint depthFormat          = GL_DEPTH24_STENCIL8;   ///< GL_DEPTH24_STENCIL8
//            glm::ivec2 size             = glm::ivec2(0, 0);       ///< size x:width, y:height
//            GLuint msaaMax              = 0;                     ///< max multisamples size, 0: No MSAA
//
//            /** @brief output */
//            GLuint id                   = GL_NONE;
//            GLuint colorTextId          = GL_NONE;
//            GLuint depthRenderbufferId  = GL_NONE;
//        };
//
//        class GLHelper
//        {
//        public:
//
//            static bool IsExtensionSupported(const char *extension)
//            {
//                const GLubyte *extensions = NULL;
//                const GLubyte *start;
//                GLubyte *where, *terminator;
//
//                /* Extension names should not have spaces. */
//                where = (GLubyte *) strchr(extension, ' ');
//                if (where || *extension == '\0')
//                    return false;
//
//                extensions = glGetString(GL_EXTENSIONS);
//
//                /* It takes a bit of care to be fool-proof about parsing the
//                OpenGL extensions string. Don't be fooled by sub-strings,
//                etc.
//                */
//                start = extensions;
//                for (;;) {
//                    where = (GLubyte *) strstr((const char *) start, extension);
//
//                    if (!where) break;
//
//                    terminator = where + strlen(extension);
//
//                    if (where == start || *(where - 1) == ' ')
//                        if (*terminator == ' ' || *terminator == '\0') return true;
//
//                    start = terminator;
//                }
//
//                return false;
//            }
//
//
//            static bool CreateFramebuffer(Framebuffer &framebuffer)
//            {
//            #if defined(GL_ES_TDA) || defined(USE_GLES20) || defined(USE_GLES32)
//                PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC glRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC)eglGetProcAddress( "glRenderbufferStorageMultisampleEXT");
//                PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC glFramebufferTexture2DMultisampleEXT = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC)eglGetProcAddress( "glFramebufferTexture2DMultisampleEXT");
//            #endif // GL_ES_TDA USE_GLES20
//
//                LOGGER_I("CreateFramebuffer size(%d,%d)", framebuffer.size.x, framebuffer.size.y);
//
//                GLint samples = 0;
//                glGetIntegerv(GL_MAX_SAMPLES, &samples);
//                LOGGER_I("GL_MAX_SAMPLES:%d, msaaMax=%d", samples, framebuffer.msaaMax);
//
//                glGenFramebuffers(1, &framebuffer.id);
//                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
//
//                if (framebuffer.msaaMax > 0)
//                {
//                    assert (static_cast<GLint>(framebuffer.msaaMax) <= samples);
//                    /** @brief MSAA */
//                #if defined(GL_ES_TDA) || defined(USE_GLES20) || defined(USE_GLES32)
//                    glGenTextures(1, &framebuffer.colorTextId);
//                    glBindTexture(GL_TEXTURE_2D, framebuffer.colorTextId);
//                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//                    glTexImage2D(GL_TEXTURE_2D, 0, framebuffer.colorFormat, framebuffer.size.x, framebuffer.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
//                    glBindTexture(GL_TEXTURE_2D, 0);
//                    LOGGER_GLE("CreateFramebuffer glGenTextures done");
//                #else // GL_ES_TDA
//                    glGenTextures(1, &framebuffer.colorTextId);
//                    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebuffer.colorTextId);
//                    glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, framebuffer.msaaMax, framebuffer.colorFormat, framebuffer.size.x, framebuffer.size.y, GL_TRUE);
//                    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
//                    LOGGER_GLE("CreateFramebuffer glGenTextures done");
//                #endif // GL_ES_TDA
//                }
//                else
//                {
//                    /** @brief no MSAA */
//                    glGenTextures(1, &framebuffer.colorTextId);
//                    glBindTexture(GL_TEXTURE_2D, framebuffer.colorTextId);
//                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//                    glTexImage2D(GL_TEXTURE_2D, 0, framebuffer.colorFormat, framebuffer.size.x, framebuffer.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
//                    glBindTexture(GL_TEXTURE_2D, 0);
//                    LOGGER_GLE("CreateFramebuffer glGenTextures done");
//                }
//
//                glGenRenderbuffers(1, &framebuffer.depthRenderbufferId);
//                glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.depthRenderbufferId);
//
//                if (framebuffer.msaaMax > 0)
//                {
//                #if defined(GL_ES_TDA) || defined(USE_GLES20) || defined(USE_GLES32)
//                    glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, framebuffer.msaaMax, framebuffer.depthFormat, framebuffer.size.x, framebuffer.size.y);
//                #else // GL_ES_TDA
//                    glRenderbufferStorageMultisample(GL_RENDERBUFFER, framebuffer.msaaMax, framebuffer.depthFormat, framebuffer.size.x, framebuffer.size.y);
//                #endif // GL_ES_TDA
//                }
//                else
//                {
//                    /** @brief no MSAA */
//                    glRenderbufferStorage(GL_RENDERBUFFER, framebuffer.depthFormat, framebuffer.size.x, framebuffer.size.y);
//                }
//                glBindRenderbuffer(GL_RENDERBUFFER, 0);
//                LOGGER_GLE("CreateFramebuffer glRenderbufferStorage depth done");
//
//                if (framebuffer.msaaMax > 0)
//                {
//                #if defined(GL_ES_TDA) || defined(USE_GLES20) || defined(USE_GLES32)
//                    glFramebufferTexture2DMultisampleEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.colorTextId, 0, framebuffer.msaaMax);
//                #else
//                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebuffer.colorTextId, 0);
//                #endif // GL_ES_TDA
//                }
//                else
//                {
//                    /** @brief no MSAA */
//                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.colorTextId, 0);
//                }
//
//                LOGGER_GLE("CreateFramebuffer GL_COLOR_ATTACHMENT0 done");
//
//                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer.depthRenderbufferId);
//                // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.depthRenderbufferId);
//
//                LOGGER_GLE("CreateFramebuffer GL_DEPTH_ATTACHMENT done");
//
//                GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//                if (status != GL_FRAMEBUFFER_COMPLETE) {
//                    LOGGER_GLE("CreateFramebuffer Framebuffer incomplete! (GL Error: %x)\n", status);
//                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//                    return false;
//                }
//
//                glBindFramebuffer(GL_FRAMEBUFFER, 0);
//                LOGGER_GLE("CreateFramebuffer done");
//
//                return true;
//            }
//
//            static void FreeFramebuffer(Framebuffer &framebuffer)
//            {
//                glDeleteTextures(1, &framebuffer.colorTextId);
//                glDeleteRenderbuffers(1, &framebuffer.depthRenderbufferId);
//                glDeleteFramebuffers(1, &framebuffer.id);
//
//                framebuffer.colorTextId         = GL_NONE;
//                framebuffer.depthRenderbufferId = GL_NONE;
//                framebuffer.id                  = GL_NONE;
//            }
//
//            /**************************************************************************************************
//             * @brief generate rectangle vertex array and buffer
//             *
//             * @param[in] vertices input vertex array
//             * @param[out] VAO
//             * @param[out] VBO
//            **************************************************************************************************/
//            static void GenRectangleVertexArrayBuffer(const float vertices[], size_t verticeSize, GLuint &VAO, GLuint &VBO)
//            {
//                glGenVertexArrays(1, &VAO);
//                glGenBuffers(1, &VBO);
//
//                glBindVertexArray(VAO);
//
//                glBindBuffer(GL_ARRAY_BUFFER, VBO);
//                glBufferData(GL_ARRAY_BUFFER, verticeSize, vertices, GL_STATIC_DRAW);
//
//                // position attribute
//                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//                glEnableVertexAttribArray(0);
//
//                // texture coord attribute
//                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//                glEnableVertexAttribArray(1);
//                glBindVertexArray(0);
//            }
//
//            /**************************************************************************************************
//             * @brief GenRectangleVertexArray
//             *
//             * @param v1 Parent viewport
//             * @param v2 Current Viewport
//             * @param isTextureFlipT flip t
//             * @param VAO
//             * @param VBO
//             * @param EBO
//            **************************************************************************************************/
//            static void GenRectangleVertexArray(glm::ivec4 v1, glm::ivec4 v2, bool isTextureFlipT, GLuint &VAO, GLuint &VBO, GLuint &EBO)
//            {
//                glm::vec2 v1BL = glm::vec2(v1.x,      v1.y);      // bottom left
//
//                glm::vec2 v2BL = glm::vec2(v2.x,      v2.y);      // bottom left
//                glm::vec2 v2TR = glm::vec2(v2.x+v2.z, v2.y+v2.w); // top right
//
//                glm::vec2 v2DeltaBL = (v2BL - v1BL);  // bottom left of v2
//                glm::vec2 v2DeltaTR = (v2TR - v1BL);  // top right of v2
//
//                float vertices[] = {
//                    2.0f * v2DeltaTR.x / v1.z - 1.0f, 2.f * v2DeltaTR.y / v1.w - 1.0f, 0.0f, 1.0f, isTextureFlipT ? 0.f : 1.0f, // top right
//                    2.0f * v2DeltaTR.x / v1.z - 1.0f, 2.f * v2DeltaBL.y / v1.w - 1.0f, 0.0f, 1.0f, isTextureFlipT ? 1.f : 0.0f, // bottom right
//                    2.0f * v2DeltaBL.x / v1.z - 1.0f, 2.f * v2DeltaBL.y / v1.w - 1.0f, 0.0f, 0.0f, isTextureFlipT ? 1.f : 0.0f, // bottom left
//                    2.0f * v2DeltaBL.x / v1.z - 1.0f, 2.f * v2DeltaTR.y / v1.w - 1.0f, 0.0f, 0.0f, isTextureFlipT ? 0.f : 1.0f, // top left
//                };
//
//                EBO_INDICES_TYPE indices[] = {
//                    0, 1, 3, // first triangle
//                    1, 2, 3  // second triangle
//                };
//
//                glGenVertexArrays(1, &VAO);
//                glGenBuffers(1, &VBO);
//                glGenBuffers(1, &EBO);
//
//                glBindVertexArray(VAO);
//
//                glBindBuffer(GL_ARRAY_BUFFER, VBO);
//                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//                // position attribute
//                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//                glEnableVertexAttribArray(0);
//
//                // texture coord attribute
//                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//                glEnableVertexAttribArray(1);
//                glBindVertexArray(0);
//            }
//
//        };
//
//    } // namespace BaseLib
//
//
//    /**************************************************************************************************
//     * @brief HEADER FOR Shader
//     *
//    **************************************************************************************************/
////    namespace BaseLib{
////
////    class Shader
////    {
////    public:
////
////        unsigned int ID;
////
////        Shader();
////        ~Shader();
////
////        /**
////         * @brief build shader from shader source file
////         *
////         * @param vertexPath   const char *  vertext shader source file
////         * @param fragmentPath const char *  fragment shader source file
////         * @param geometryPath const char *  geometry shader source file
////         */
////        void FromSourceFile(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
////
////        // activate the shader
////        // ------------------------------------------------------------------------
////        void UseProgram();
////
////        // utility uniform functions
////        // ------------------------------------------------------------------------
////        void SetBool(const std::string &name, bool value) const;
////
////        // ------------------------------------------------------------------------
////        void SetInt(const std::string &name, int value) const;
////
////        // ------------------------------------------------------------------------
////        void SetFloat(const std::string &name, float value) const;
////
////        // ------------------------------------------------------------------------
////        void SetVec2(const std::string &name, const glm::vec2 &value) const;
////
////        void SetVec2(const std::string &name, float x, float y) const;
////
////        // ------------------------------------------------------------------------
////        void SetVec3(const std::string &name, const glm::vec3 &value) const;
////
////        // ------------------------------------------------------------------------
////        void SetVec3(const std::string &name, float x, float y, float z) const;
////
////        // ------------------------------------------------------------------------
////        void SetVec4(const std::string &name, const glm::vec4 &value) const;
////
////        void SetVec4(const std::string &name, float x, float y, float z, float w);
////
////        // ------------------------------------------------------------------------
////        void SetMat2(const std::string &name, const glm::mat2 &mat) const;
////
////        // ------------------------------------------------------------------------
////        void SetMat3(const std::string &name, const glm::mat3 &mat) const;
////
////        // ------------------------------------------------------------------------
////        void SetMat4(const std::string &name, const glm::mat4 &mat) const;
////
////    private:
////        // utility function for checking shader compilation/linking errors.
////        // ------------------------------------------------------------------------
////        GLint CheckCompileErrors(GLuint shader, std::string type);
////    } ;
////
////    }
//
//
//    /**************************************************************************************************
//     * @brief HEADER FOR Mesh
//     *
//     **************************************************************************************************/
///*
//    namespace BaseLib{
//
//    struct Vertex
//    {
//        // position
//        glm::vec3 position;
//
//    #ifdef ENABLE_VERTEX_NORMAL
//        // normal
//        glm::vec3 normal;
//
//        // texCoords
//        glm::vec2 texCoords;
//    #else
//        // texCoords
//        glm::vec2 texCoords;
//    #endif // ENABLE_VERTEX_NORMAL
//
//
//    #ifdef ENABLE_VERTEX_TANGENT
//        // tangent
//        glm::vec3 tangent;
//        // bitangent
//        glm::vec3 bitangent;
//    #endif // ENABLE_VERTEX_TANGENT
//
//        // //bone indexes which will influence this vertex
//        // int mBoneIDs[4];  //< MAX_BONE_INFLUENCE = 4;
//        // //weights from each bone
//        // float mWeights[4];  //< MAX_BONE_INFLUENCE = 4;
//    } ;
//
//    struct Texture
//    {
//        unsigned int id;
//        std::string type;
//        std::string path;
//    } ;
//
//    class Mesh
//    {
//    public:
//        Mesh();
//        ~Mesh();
//    public:
//        // mesh Data
//        // std::vector<Vertex>       vertices;
//
//        std::vector<EBO_INDICES_TYPE> indices;
//
//        std::vector<Texture>      textures;
//        unsigned int VAO;
//        // render data
//        unsigned int VBO, EBO;
//
//        Mesh(std::vector<Vertex> vertices, std::vector<EBO_INDICES_TYPE> indices, std::vector<Texture> textures);
//
//        void Draw(Shader &shader, std::vector<Texture> &textures, GLuint texOffset=GL_NONE);
//
//    private:
//        // initializes all the buffer objects/arrays
//        static void SetupMesh(unsigned int *VAO, unsigned int *VBO, unsigned int *EBO, std::vector<Vertex> & vertices, std::vector<EBO_INDICES_TYPE> & indices);
//    } ;
//    }
//*/
//
//
//    /**************************************************************************************************
//     * @brief HEADER FOR Model
//     *
//    **************************************************************************************************/
////    namespace BaseLib{
////
////    using namespace BaseLib;
////
////    typedef bool (*LogPtrType)(std::string);
////
////    class Model
////    {
////    public:
////
////        // model data
////        std::vector<Texture> texturesLoaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
////        std::vector<Mesh>    meshes;
////        std::string directory;
////        std::string path;
////
////        static LogPtrType logPtr;
////
////        /* the global Assimp scene object */
////        // const C_STRUCT aiScene* scene = NULL;
////        C_STRUCT aiVector3D sceneMin, sceneMax, sceneCenter;
////
////        std::vector<glm::vec3> meshsCenter;
////
////        Model();
////        // constructor, expects a filepath to a 3D model.
////        Model(std::string const &path) : path(path){};
////
////        ~Model();
////
////        //传递路径参数
////        void Initialization(std::string const &path, bool (*LOG)(std::string));
////
////        // draws the model, and thus all its meshes
////        void Draw(Shader &shader);
////
////        void Draw(Shader &shader, std::vector<Texture> &textures);
////
////        /**
////         * @brief draw mesh by specified mesh index with given shader
////         *
////         * @param meshIdx unsigned int mesh index
////         * @param shader  Shader       compiled shader
////         */
////        void DrawMesh(unsigned int meshIdx, Shader &shader, GLuint texOffset=GL_NONE);
////
////        void DrawMesh(unsigned int meshIdx, Shader &shader, std::vector<Texture> &textures, GLuint texOffset=GL_NONE);
////
////        /**
////         * @brief Get the frustum fit scale value
////         *
////         * @return float
////         */
////        float GetFrustumFitScale();
////
////        float GetMaxViewDistance();
////
////        glm::vec3 GetAdjustModelPosVec();
////
////    public:
////        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
////        void LoadModel();
////
////
////        static unsigned int TextureFromFile(const char *path, const std::string &directory);
////
////        /**************************************************************************************************
////         * @brief Load Texture
////         *
////         * @param typeName  std::string  type name, one of:
////         *                               {texture_diffuse, texture_specular, texture_normal, texture_height}
////         * @param path      std::string file path
////         * @param directory std::string directory
////         * @return Texture
////         **************************************************************************************************/
////        static Texture LoadTexture(std::string typeName, std::string path, const std::string &directory);
////
////        static GLuint GenImageTexture(std::string filename);
////
////
////        /**************************************************************************************************
////         * @brief parse .pvr format file and generate a texture
////         *
////         * @param filename pvr format file.
////         * @param textureID generated texture, a valid texture id would be > 0
////        **************************************************************************************************/
////        static void ParsePVR(const char *filename, unsigned int* textureID);
////
////    private:
////        // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
////        void ProcessNode(aiNode *node, const aiScene *scene);
////
////        Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
////
////
////        // checks all material textures of a given type and loads the textures if they're not loaded yet.
////        // the required info is returned as a Texture struct.
////        std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
////
////
////        void GetBoundBox(C_STRUCT aiVector3D* min, C_STRUCT aiVector3D* max, const C_STRUCT aiScene* scene);
////
////        glm::vec3 GetMeshCenter(unsigned int meshIndex, C_STRUCT aiMesh* mesh);
////
////        /**
////         * @brief Get the Bound Box for node object
////         *
////         * @param nd
////         * @param min
////         * @param max
////         * @param trafo
////         */
////        void GetBoundBox4node (const C_STRUCT aiNode* nd,
////                C_STRUCT aiVector3D* min,
////                C_STRUCT aiVector3D* max,
////                C_STRUCT aiMatrix4x4* trafo,
////                const C_STRUCT aiScene* scene
////                );
////
////    } ;
////
////    }
//
//
////     /**************************************************************************************************
////     * @brief HEADER FOR TimeHelper
////     *
////    **************************************************************************************************/
////    namespace BaseLib{
////
////    class TimeHelper
////    {
////    public:
////        static long GetTimestampMillis()
////        {
////            struct timeval tv;
////            gettimeofday(&tv, NULL);
////            return tv.tv_sec * 1000 + tv.tv_usec / 1000;
////        }
////
////        static long GetTimestampMicros()
////        {
////            struct timeval tv;
////            gettimeofday(&tv, NULL);
////            return tv.tv_sec * 1000000 + tv.tv_usec;
////        }
////    };
////
////    }
//
//
//    /**************************************************************************************************
//     * @brief HEADER FOR CarModel3D
//     *
//    **************************************************************************************************/
//
//    #define CAR_MODEL_3D_HPP_VERSION (2024052101)
//
//    namespace CarModel3D{
//
//    #define CFG_KEY_ANIMATION_3D_MODEL_FILE        "animation_3d_model_file"
//    #define CFG_KEY_ANIMATION_3D_SHADER            "shader_source"
//    #define CFG_KEY_ANIMATION_3D_VERTEX            "vertex"
//    #define CFG_KEY_ANIMATION_3D_FRAGMENT          "fragment"
//
//
//    #define WHEEL_DIRECTION_STATIC   0
//    #define WHEEL_DIRECTION_FORWARD  2
//    #define WHEEL_DIRECTION_BACKWARD 3
//
//
//    using namespace BaseLib;
//
//
//    /*
//    | 变量类型  | 变量名称                  | 类型说明                                                     |
//    | :-------- | :------------------------ | ------------------------------------------------------------ |
//    | uint8     | cm3DFrontLeftDoorState    | 左前门状态， 1表示开， 2表示关                               |
//    | uint8     | cm3DFrontRightDoorState   | 右前门状态， 1表示开， 2表示关                               |
//    | uint8     | cm3DRearLeftDoorState     | 左后门状态，1表示开， 2表示关                                |
//    | uint8     | cm3DRearRightDoorState    | 右后门状态， 1表示开， 2表示关                               |
//    | uint8     | cm3DEngineCoverState      | 引擎盖状态， 1表示开， 2表示关                               |
//    | uint8     | cm3DRearTailBoxState      | 后备箱状态， 1表示开， 2表示关                               |
//    | WheelInfo | cm3DFrontLeftWheelState   | 左前轮滚动方向以及旋转速度,cm3DFrontLeftWheelState.speed 代表车轮滚动速度， cm3DFrontLeftWheelState.direction 代表车轮滚动方向。cm3DFrontLeftWheelState.angle 表示车轮转动幅度（rad），值大于零表示左转向， 值小于零表示右转向。 |
//    | WheelInfo | cm3DFrontRightWheelState    | 右前轮滚动方向以及旋转速度，cm3DFrontLeftWheelState.direction 代表车轮滚动方向，cm3DFrontLeftWheelState.speed代表车轮滚动速度。cm3DFrontLeftWheelState.angle 表示车轮转动幅度（rad），值大于零表示左转向， 值小于零表示右转向。 |
//    | WheelInfo | cm3DRearLeftWheelState    | 左后轮滚动方向以及旋转速度，，cm3DFrontLeftWheelState.speed 代表车轮滚动速度， cm3DFrontLeftWheelState.direction 代表车轮滚动方向。cm3DFrontLeftWheelState.angle 表示车轮转动幅度（rad），值大于零表示左转向， 值小于零表示右转向。 |
//    | WheelInfo | cm3DRearRightWheelState   | 右后轮滚动方向以及旋转速度，cm3DFrontLeftWheelState.speed 代表车轮滚动速度，cm3DFrontLeftWheelState.direction 代表车轮滚动方向。cm3DFrontLeftWheelState.angle 表示车轮转动幅度（rad/s），值大于零表示左转向， 值小于零表示右转向。 |
//    | uint8     | cm3DLeftMirrorsState      | @deprecated 1表示打开状态， 2表示折叠状态， 3表示打开动画， 4表示折叠动画。 |
//    | uint8     | cm3DRightMirrorsState     | @deprecated 1表示打开状态， 2表示折叠状态， 3表示打开动画， 4表示折叠动画。 |
//    | uint8     | cm3DSunFloorState         | 天窗状态， 0x01表示全关， 0x02代表打开4%, 0x03代表打开8% ....   0x19代表打开96%， 0x1A代表全开。 |
//    | uint8     | cm3DFrontLeftWindowState  | 左前车窗状态， 0x01表示全关， 0x02代表打开4%, 0x03代表打开8% ....   0x19代表打开96%，0x1A代表全开。 |
//    | uint8     | cm3DFrontRightWindowState | 右前车窗状态， 0x01表示全关， 0x02代表打开4%, 0x03代表打开8% ....   0x19代表打开96%，0x1A代表全开。 |
//    | uint8     | cm3DRearLeftWindowState   | 左后车窗状态， 0x01表示全关， 0x02代表打开4%, 0x03代表打开8% ....   0x19代表打开96%，0x1A代表全开。 |
//    | uint8     | cm3DRearRightWindowState  | 右后车窗状态    0x01表示全关， 0x02代表打开4%, 0x03代表打开8% ....   0x19代表打开96%，0x1A代表全开。 |
//    | uint8     | cm3DNearFarLightState     | 远近灯光状态， 0代表关， 1 代表近光灯， 2 代表远光灯。3 代表近光灯和远光灯全开 。       |
//    | uint8     | cm3DBrakeLightState       | 刹车灯状态，  0 代表刹车状态， 1 代表没刹车状态。            |
//    | uint8     | cm3DRuingLightState       | 日间行车灯状态， 1 代表 （模型默认打开。）                            |
//    | uint8     | cm3DTurnLightState        | 转向灯状态，  1 代表左转， 2 代表右转，3 左右都闪。                      |
//    | double    | seatAngle                 | 主驾座椅角度  数据类型待定  （正常驾驶默认110度 可以设置0-180度）单位：角度（deg），范围0~180度。|
//    |           |                           |                                                              |
//    */
//
//    /**
//     * 2023-06-06
//    | 变量类型  | 变量名称                 | 类型说明                                                     |
//    | :-------- | :----------------------- | ------------------------------------------------------------ |
//    | WheelInfo | cm3DFrontLeftWheelState  | cm3DFrontLeftWheelState.direction 代表车轮滚动方向， 1-静止，2-前进，3-后退。 |
//    | WheelInfo | cm3DFrontRightWheelState | cm3DFrontLeftWheelState.direction 代表车轮滚动方向， 1-静止，2-前进，3-后退。 |
//    | WheelInfo | cm3DRearLeftWheelState   | cm3DFrontLeftWheelState.direction 代表车轮滚动方向， 1-静止，2-前进，3-后退。 |
//    | WheelInfo | cm3DRearRightWheelState  | cm3DFrontLeftWheelState.direction 代表车轮滚动方向， 1-静止，2-前进，3-后退。 |
//    | uint8_t   | cm3DLeftMirrorsState     | 左后视镜状态， 1表示打开状态， 2表示折叠状态。               |
//    | uint8_t   | cm3DRightMirrorsState    | 右后视镜状态， 1表示打开状态， 2表示折叠状态。               |
//    */
//
//
//    /**
//     * 车轮信息
//     */
//    struct WheelInfo
//    {
//        float speed;          /**< 车轮滚动速度m/s*/
//        uint8_t direction;    /**< 代表车轮滚动方向， 1-静止，2-前进，3-后退。*/
//        /**
//         * 车轮转向角度， 值大于零表示左转向， 值小于零表示右转向。
//         *（前轮偏角）（方向盘角度范围：-14.5 rad到14.5 rad）  单位：弧度
//        */
//        float angle;
//    } ;
//
//    /*
//    * 3D 动画参数
//    */
//    struct  Animation3D
//    {
//        uint8_t   cm3DFrontLeftDoorState;       //左前门状态， 1表示开， 2表示关
//        uint8_t   cm3DFrontRightDoorState;    //右前门状态， 1表示开， 2表示关
//        uint8_t   cm3DRearLeftDoorState;         //左后门状态，1表示开， 2表示关
//        uint8_t   cm3DRearRightDoorState;      //右后门状态， 1表示开， 2表示关
//        uint8_t   cm3DEngineCoverState;         //引擎盖状态， 1表示开， 2表示关
//        uint8_t   cm3DRearTailBoxState;           //后备箱状态， 1表示开， 2表示关
//        WheelInfo cm3DFrontLeftWheelState;      //左前轮
//        WheelInfo cm3DFrontRightWheelState;   //右前轮
//        WheelInfo cm3DRearLeftWheelState;        //左后轮
//        WheelInfo cm3DRearRightWheelState;       //右后轮
//        uint8_t   cm3DLeftMirrorsState;          ///< 左后视镜状态， 1表示打开状态， 2表示折叠状态
//        uint8_t   cm3DRightMirrorsState;         ///< 右后视镜状态， 1表示打开状态， 2表示折叠状态
//        uint8_t   cm3DSunFloorState;
//        uint8_t   cm3DFrontLeftWindowState;
//        uint8_t   cm3DFrontRightWindowState;
//        uint8_t   cm3DRearLeftWindowState;
//        uint8_t   cm3DRearRightWindowState;
//
//        /**************************************************************************************************
//         * @brief LIGHT
//        **************************************************************************************************/
//        uint8_t   cm3DLowBeamState;              ///< 近光灯 0:OFF, 1:ON
//        uint8_t   cm3DNearFarLightState;         ///< 远光灯 0:OFF, 1:ON
//        uint8_t   cm3DBrakeLightState;           ///< 刹车灯 0代表刹车状态, 1代表没刹车状态。
//        uint8_t   cm3DRuingLightState;           ///< 前日行灯信号 0:OFF, 1:ON
//        uint8_t   cm3DRuingSecondaryLightState;  ///< 前日行转向灯共用信号 0:OFF, 1:left ON, 2:right ON, 3:both ON
//        uint8_t   cm3DRearPositionLightState;    ///< 后位置灯信号 E371(与转向共用 0:OFF, 1:left ON, 2:right ON, 3:both ON), E171/E245(单独 0:OFF, 1:ON)
//        uint8_t   cm3DTurnLightState;            ///< 转向灯 1:左转， 2:右转，3:全亮
//        uint8_t   cm3DWelcomeLightState;         ///< 迎宾灯 0:OFF,1:ON
//        uint8_t   cm3DReverseLightState;         ///< 倒车灯 0:OFF,1:ON
//        uint8_t   cm3DFogLampState;              ///< 雾灯 0:OFF,1:ON
//
//        double    seatAngle;
//        /**************************************************************************************************
//         * @brief ALPHA SETTING
//        **************************************************************************************************/
//        float     carModelAlpha;             ///< 车身内饰等（不包括底盘）透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
//        float     chassisAlpha;              ///< 车辆底盘(不包括车轮、框架)透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
//        float     windshieldAlpha;           ///< 前挡风玻璃透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
//        float     windowAlpha;               ///< 主副驾车窗透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
//        float     passengerWindowAlpha;      ///< 乘员车窗透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
//        float     sunFloorAlpha;             ///< 天窗和尾窗透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
//        float     wheelAlpha;                ///< 车轮透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
//        float     carFrameAlpha;             ///< 车框透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
//        float     shadowAlpha;               ///< 车模阴影透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
//        bool      isTextureAlphaEnable;      ///< 车身内饰等（不包括底盘）贴图对应透明度是否生效, true:生效， false:不生效
//        /**************************************************************************************************
//         * @brief ANIMATION RATE
//        **************************************************************************************************/
//        float     doorAnimationRate;           ///< 四门动画速率，单位:角度/s
//        float     hoodTrunkAnimatinoRate;      ///< 引擎盖行李箱动画速率，单位:角度/s
//        float     windowAnimationRate;         ///< 车窗开度动画速率，单位:开度/s
//        float     rearMirrorAnimationRate;     ///< 后视镜动画速率，单位:角度/s
//        float     sunFloorAnimationRate;       ///< 天窗动画速率， 单位:开度/s
//        float     frontWheelYawAnimationRate;  ///< 前轮摆动速率， 单位:角度/s
//    };
//
//
//    /*
//    * 视角相机参数
//    */
//    struct  CameraView
//    {
//        /*虚拟相机变化时，mode、view、projection矩阵 */
//        glm::mat4 view;          /*glm::mat4;*/
//        glm::mat4 model;         /*glm::mat4 */
//        glm::mat4 projection;    /*glm::mat4;*/
//
//        glm::vec3 cameraPosition; /* campera position */
//    } ;
//
//    /*
//    * VM 3D 动效
//    */
//    class CarModel3D
//    {
//    public:
//        CarModel3D();
//        CarModel3D(const CarModel3D& orig);
//        virtual ~CarModel3D();
//    private:
//
//        /*模型数据*/
//        Model dynamicModel3D;
//
//        /* shader */
//        Shader dynamicShader3D;
//
//        std::string resourceRoot;  ///< resource root directory
//
//        float rotationAngle[20]    = {0};
//
//		std::map<int, std::vector<Texture> > textrueMap; ///< textures map
//
//        int maxMeshIndex;                         ///< max mesh index.
//
//        bool InitModel();
//
//        bool InitTextures();
//
//    public:
////        static BaseLib::LogPtrType logPtr;
//
//        /**************************************************************************************************
//         * @brief 初始化接口
//         *
//         * @param resourceRoot std::string root directory of junlian
//        **************************************************************************************************/
//        bool Initialization(std::string resourceRoot, bool (*LOG)(std::string));
//
//        /*渲染部分接口，1：根据传入的结构体，渲染各自对应的状态。2：输入视角相机参数*/
//        bool DoRender(const Animation3D &animation3D, const CameraView &viewParams);
//
//        bool ResetStatus(const Animation3D &animation3D);
//
//    } ;
//    }
//
//#endif // HEADER_SPLIT
