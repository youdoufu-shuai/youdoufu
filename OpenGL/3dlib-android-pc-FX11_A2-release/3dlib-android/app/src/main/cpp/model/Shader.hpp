/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Shader.hpp
 * Author: tron
 * Function: parse and comile shader source.
 * Created on 2023年5月9日, 上午11:21
 */

#ifndef SHADER_HPP
#define SHADER_HPP

#include "Include.hpp"
#include "Logger.hpp"

namespace BaseLib{

class Shader
{
public:

    unsigned int ID;
    std::string vertexPath;
    std::string fragmentPath;
    std::string geometryPath;

    Shader();
    ~Shader();

    /**
     * @brief build shader from shader source file
     * 
     * @param vertexPath   const char *  vertext shader source file
     * @param fragmentPath const char *  fragment shader source file
     * @param geometryPath const char *  geometry shader source file
     */
    void FromSourceFile(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    
    /**
     * @brief build shader from shader source code in memory
     * 
     * @param vertexSource   const char *  vertext shader source code
     * @param fragmentSource const char *  fragment shader source code
     * @param geometrySource const char *  geometry shader source code
     */
    void FromMemory(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);

    // activate the shader
    // ------------------------------------------------------------------------
    void UseProgram();

    // utility uniform functions
    // ------------------------------------------------------------------------
    void SetBool(const std::string &name, bool value) const;

    // ------------------------------------------------------------------------
    void SetInt(const std::string &name, int value) const;

    // ------------------------------------------------------------------------
    void SetFloat(const std::string &name, float value) const;

    // ------------------------------------------------------------------------
    void SetVec2(const std::string &name, const glm::vec2 &value) const;

    void SetVec2(const std::string &name, float x, float y) const;

    // ------------------------------------------------------------------------
    void SetVec3(const std::string &name, const glm::vec3 &value) const;

    // ------------------------------------------------------------------------
    void SetVec3(const std::string &name, float x, float y, float z) const;

    // ------------------------------------------------------------------------
    void SetVec4(const std::string &name, const glm::vec4 &value) const;

    void SetVec4(const std::string &name, float x, float y, float z, float w);

    // ------------------------------------------------------------------------
    void SetMat2(const std::string &name, const glm::mat2 &mat) const;

    // ------------------------------------------------------------------------
    void SetMat3(const std::string &name, const glm::mat3 &mat) const;

    // ------------------------------------------------------------------------
    void SetMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    GLint CheckCompileErrors(GLuint shader, std::string type);
} ;

};
#endif /* SHADER_HPP */

