/**************************************************************************************************
 * @file GLHelper.hpp
 * @brief OpenGL helper
 * 
 * @details
 * 
 *  HISTORY
 *  -----------------------------------------------------------------------------------------------
 *  Version   Date        Author    Description
 *  -----------------------------------------------------------------------------------------------
 *  0.1       2023-07-28  TDZ       Initial.
 * 
 * 
**************************************************************************************************/
#pragma once

#include "../model/Include.hpp"
#include "Logger.hpp"

namespace BaseLib
{
    struct Framebuffer 
    {
        /** @brief input */
        GLuint colorFormat          = GL_RGBA8;              ///< GL_RGBA8
        GLuint depthFormat          = GL_DEPTH24_STENCIL8;   ///< GL_DEPTH24_STENCIL8
        glm::ivec2 size             = glm::ivec2(0, 0);       ///< size x:width, y:height
        GLuint msaaMax              = 0;                     ///< max multisamples size, 0: No MSAA

        /** @brief output */
        GLuint id                   = GL_NONE;
        GLuint colorTextId          = GL_NONE;
        GLuint depthRenderbufferId  = GL_NONE;
    };

    class GLHelper 
    {
        public:

        static bool CreateFramebuffer(Framebuffer &framebuffer)
        {
        #ifdef GL_ES_TDA
            PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC glRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC)eglGetProcAddress( "glRenderbufferStorageMultisampleEXT");
            PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC glFramebufferTexture2DMultisampleEXT = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC)eglGetProcAddress( "glFramebufferTexture2DMultisampleEXT");
        #else
             PFNGLTEXSTORAGE2DMULTISAMPLEPROC glTexStorage2DMultisample    = (PFNGLTEXSTORAGE2DMULTISAMPLEPROC)glutGetProcAddress("glTexStorage2DMultisample");
        #endif // GL_ES_TDA

            LOGGER_I("CreateFramebuffer size(%d,%d)", framebuffer.size.x, framebuffer.size.y);

            GLint samples = 0;
            glGetIntegerv(GL_MAX_SAMPLES, &samples);
            LOGGER_I("GL_MAX_SAMPLES:%d, msaaMax=%d", samples, framebuffer.msaaMax);

            glGenFramebuffers(1, &framebuffer.id);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
          
            if (framebuffer.msaaMax > 0)
            {
                assert (static_cast<GLint>(framebuffer.msaaMax) <= samples);
                /** @brief MSAA */
            #ifdef GL_ES_TDA
                glGenTextures(1, &framebuffer.colorTextId);
                glBindTexture(GL_TEXTURE_2D, framebuffer.colorTextId);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexImage2D(GL_TEXTURE_2D, 0, framebuffer.colorFormat, framebuffer.size.x, framebuffer.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                LOGGER_GLE("CreateFramebuffer glGenTextures done");
            #else // GL_ES_TDA
                glGenTextures(1, &framebuffer.colorTextId);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebuffer.colorTextId);
                glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, framebuffer.msaaMax, framebuffer.colorFormat, framebuffer.size.x, framebuffer.size.y, GL_TRUE);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                LOGGER_GLE("CreateFramebuffer glGenTextures done");
            #endif // GL_ES_TDA
            }
            else
            {
                /** @brief no MSAA */
                glGenTextures(1, &framebuffer.colorTextId);
                glBindTexture(GL_TEXTURE_2D, framebuffer.colorTextId);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexImage2D(GL_TEXTURE_2D, 0, framebuffer.colorFormat, framebuffer.size.x, framebuffer.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                LOGGER_GLE("CreateFramebuffer glGenTextures done");
            }

            glGenRenderbuffers(1, &framebuffer.depthRenderbufferId);
            glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.depthRenderbufferId);

            if (framebuffer.msaaMax > 0)
            {
            #ifdef GL_ES_TDA 
                glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, framebuffer.msaaMax, framebuffer.depthFormat, framebuffer.size.x, framebuffer.size.y);
            #else // GL_ES_TDA
                glRenderbufferStorageMultisample(GL_RENDERBUFFER, framebuffer.msaaMax, framebuffer.depthFormat, framebuffer.size.x, framebuffer.size.y);
            #endif // GL_ES_TDA
            }
            else
            {
                /** @brief no MSAA */
                glRenderbufferStorage(GL_RENDERBUFFER, framebuffer.depthFormat, framebuffer.size.x, framebuffer.size.y);
            }
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            LOGGER_GLE("CreateFramebuffer glRenderbufferStorage depth done");

            if (framebuffer.msaaMax > 0)
            {
            #ifdef GL_ES_TDA
                 glFramebufferTexture2DMultisampleEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.colorTextId, 0, framebuffer.msaaMax);
            #else
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebuffer.colorTextId, 0);
            #endif // GL_ES_TDA
            }
            else
            {
                /** @brief no MSAA */
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.colorTextId, 0);
            }

            LOGGER_GLE("CreateFramebuffer GL_COLOR_ATTACHMENT0 done");
           
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer.depthRenderbufferId);
            // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.depthRenderbufferId);

            LOGGER_GLE("CreateFramebuffer GL_DEPTH_ATTACHMENT done");

            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
                LOGGER_GLE("CreateFramebuffer Framebuffer incomplete! (GL Error: %x)\n", status);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                return false;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            LOGGER_GLE("CreateFramebuffer done");

            return true;
        }

        static void FreeFramebuffer(Framebuffer &framebuffer)
        {
            glDeleteTextures(1, &framebuffer.colorTextId);
            glDeleteRenderbuffers(1, &framebuffer.depthRenderbufferId);
            glDeleteFramebuffers(1, &framebuffer.id);

            framebuffer.colorTextId         = GL_NONE;
            framebuffer.depthRenderbufferId = GL_NONE;
            framebuffer.id                  = GL_NONE;
        }

        /**************************************************************************************************
         * @brief generate rectangle vertex array and buffer
         * 
         * @param[in] vertices input vertex array
         * @param[out] VAO 
         * @param[out] VBO 
        **************************************************************************************************/
        static void GenRectangleVertexArrayBuffer(const float vertices[], size_t verticeSize, GLuint &VAO, GLuint &VBO)
        {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, verticeSize, vertices, GL_STATIC_DRAW);

            // position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        
            // texture coord attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);
        }

        /**************************************************************************************************
         * @brief GenRectangleVertexArray
         * 
         * @param v1 Parent viewport 
         * @param v2 Current Viewport 
         * @param isTextureFlipT flip t
         * @param VAO 
         * @param VBO 
         * @param EBO
        **************************************************************************************************/
        static void GenRectangleVertexArray(glm::ivec4 v1, glm::ivec4 v2, bool isTextureFlipT, GLuint &VAO, GLuint &VBO, GLuint &EBO)
        {
            glm::vec2 v1BL = glm::vec2(v1.x,      v1.y);      // bottom left
            // glm::vec2 v1TR = glm::vec2(v1.x+v1.z, v1.y+v1.w); // top right

            glm::vec2 v2BL = glm::vec2(v2.x,      v2.y);      // bottom left
            glm::vec2 v2TR = glm::vec2(v2.x+v2.z, v2.y+v2.w); // top right

            glm::vec2 v2DeltaBL = (v2BL - v1BL);  // bottom left of v2
            glm::vec2 v2DeltaTR = (v2TR - v1BL);  // top right of v2

            float vertices[] = {
                2.0f * v2DeltaTR.x / v1.z - 1.0f, 2.f * v2DeltaTR.y / v1.w - 1.0f, 0.0f, 1.0f, isTextureFlipT ? 0.f : 1.0f, // top right
                2.0f * v2DeltaTR.x / v1.z - 1.0f, 2.f * v2DeltaBL.y / v1.w - 1.0f, 0.0f, 1.0f, isTextureFlipT ? 1.f : 0.0f, // bottom right
                2.0f * v2DeltaBL.x / v1.z - 1.0f, 2.f * v2DeltaBL.y / v1.w - 1.0f, 0.0f, 0.0f, isTextureFlipT ? 1.f : 0.0f, // bottom left
                2.0f * v2DeltaBL.x / v1.z - 1.0f, 2.f * v2DeltaTR.y / v1.w - 1.0f, 0.0f, 0.0f, isTextureFlipT ? 0.f : 1.0f, // top left
            };

            // for (int i=0; i < sizeof(vertices)/sizeof(float); i++) {
            //     printf("%f ", vertices[i]);
            //     if (i > 0 && (i+1) % 5 == 0) printf("\n");
            // }

            unsigned int indices[] = {  
                0, 1, 3, // first triangle
                1, 2, 3  // second triangle
            };

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            // position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        
            // texture coord attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            glBindVertexArray(0);
        }

    };

} // namespace BaseLib

/**************************************************************************************************
 * END OF FILE
**************************************************************************************************/
