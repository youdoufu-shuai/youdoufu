/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Include.hpp
 * Author: tron
 *
 * Created on 2023年5月8日, 上午9:42
 */

#ifndef INCLUDE_HPP
#define INCLUDE_HPP

#define GL_ES_TDA

#ifdef GL_ES_TDA
    #include <EGL/egl.h>
    #include <EGL/eglext.h>
    #include <GLES3/gl32.h>
    #include <GLES2/gl2ext.h>
    #ifndef GL_GEOMETRY_SHADER
        #define GL_GEOMETRY_SHADER GL_GEOMETRY_SHADER_OES
    #endif // GL_GEOMETRY_SHADER

#else
    #include <GL/glew.h>
    #include <GL/freeglut.h>

#endif //GL_ES_TDA

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <map>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <string>
#include <memory>

#include <yaml-cpp/yaml.h>


    
#define GLX_AA_BLEND_ENABLE()  \
                                glEnable(GL_BLEND); \
                                glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
                                

#define GLX_AA_BLEND_DISABLE()  \
                                    glDisable(GL_BLEND);


#endif /* INCLUDE_HPP */
