/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */



/* 
 * File:   Shader.cpp
 * Author: tron
 * Function: parse and comile shader source.
 * Created on 2023年5月9日, 上午11:21
 */

/*
 *  HISTORY
 *  -----------------------------------------------------------------------------------------------
 *  Version   Date        Author    Description
 *  -----------------------------------------------------------------------------------------------
 *  0.1       2023-04-20  TDZ       Migrate from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader.h
 *
 * 
 **************************************************************************************************/

 #include "Shader.hpp"

 namespace BaseLib{
 
 Shader::Shader()
 {
 }
 
 Shader::~Shader()
 {
 }
 /**
  * @brief build shader from shader source file
  * 
  * @param vertexPath   const char *  vertext shader source file
  * @param fragmentPath const char *  fragment shader source file
  * @param geometryPath const char *  geometry shader source file
  */
 void Shader::FromSourceFile(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
 {
     // 保存着色器文件路径，以便在其他地方使用
     this->vertexPath = vertexPath ? vertexPath : "";
     this->fragmentPath = fragmentPath ? fragmentPath : "";
     this->geometryPath = geometryPath ? geometryPath : "";
     
     // 1. retrieve the vertex/fragment source code from filePath
     std::string vertexCode;
     std::string fragmentCode;
     std::string geometryCode;
     std::ifstream vShaderFile;
     std::ifstream fShaderFile;
     std::ifstream gShaderFile;
     // ensure ifstream objects can throw exceptions:
     vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
     fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
     gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
     try
     {
         // open files
         vShaderFile.open(vertexPath);
         fShaderFile.open(fragmentPath);
         std::stringstream vShaderStream, fShaderStream;
 
         // read file's buffer contents into streams
         if(vShaderFile.is_open())
         {
             vShaderStream << vShaderFile.rdbuf();
         }
         else
         {
             std::cout<<"vShaderFile open error"<<std::endl;
         }
 
         if(fShaderFile.is_open())
         {
             fShaderStream << fShaderFile.rdbuf();
         }
         else
         {
             std::cout<<"fShaderStream open error"<<std::endl;
         }
 
         // close file handlers
         vShaderFile.close();
         fShaderFile.close();
 
         // convert stream into string
         vertexCode = vShaderStream.str();
         fragmentCode = fShaderStream.str();
 
         // if geometry shader path is present, also load a geometry shader
         if ( geometryPath != nullptr )
         {
             gShaderFile.open(geometryPath);
             std::stringstream gShaderStream;
             gShaderStream << gShaderFile.rdbuf();
             gShaderFile.close();
             geometryCode = gShaderStream.str();
         }
     }
     catch ( std::ifstream::failure& e )
     {
         std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
     }
 
     const char *vShaderCode = vertexCode.c_str();
     const char *fShaderCode = fragmentCode.c_str();
     // 2. compile shaders
     unsigned int vertex, fragment;
     // vertex shader
     vertex = glCreateShader(GL_VERTEX_SHADER);
     glShaderSource(vertex, 1, &vShaderCode, NULL);
     glCompileShader(vertex);
     CheckCompileErrors(vertex, "VERTEX");
 
     // fragment Shader
     fragment = glCreateShader(GL_FRAGMENT_SHADER);
     glShaderSource(fragment, 1, &fShaderCode, NULL);
     glCompileShader(fragment);
     CheckCompileErrors(fragment, "FRAGMENT");
 
     // if geometry shader is given, compile geometry shader
     unsigned int geometry = GL_NONE;
     if ( geometryPath != nullptr )
     {
         const char * gShaderCode = geometryCode.c_str();
         geometry = glCreateShader(GL_GEOMETRY_SHADER);
         glShaderSource(geometry, 1, &gShaderCode, NULL);
         glCompileShader(geometry);
         CheckCompileErrors(geometry, "GEOMETRY");
     }
     
     // shader Program
     ID = glCreateProgram();
     glAttachShader(ID, vertex);
     glAttachShader(ID, fragment);
     if ( geometryPath != nullptr )
         glAttachShader(ID, geometry);
     glLinkProgram(ID);
     CheckCompileErrors(ID, "PROGRAM");
 
     // delete the shaders as they're linked into our program now and no longer necessary
     glDeleteShader(vertex);
     glDeleteShader(fragment);
     if ((geometryPath != nullptr) && (geometry != GL_NONE))
     {
         glDeleteShader(geometry);
     }
         
 }
 
 // activate the shader
 // ------------------------------------------------------------------------
 void Shader::UseProgram()
 {
     glUseProgram(ID);
 }
 
 
 // utility uniform functions
 // ------------------------------------------------------------------------
 void Shader::SetBool(const std::string &name, bool value) const
 {
     glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
 }
 
 
 // ------------------------------------------------------------------------
 void Shader::SetInt(const std::string &name, int value) const
 {
     glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
 }
 
 // ------------------------------------------------------------------------
 void Shader::SetFloat(const std::string &name, float value) const
 {
     glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
 }
 
 // ------------------------------------------------------------------------
 void Shader::SetVec2(const std::string &name, const glm::vec2 &value) const
 {
     glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
 }
 void Shader::SetVec2(const std::string &name, float x, float y) const
 {
     glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
 }
 
 
 // ------------------------------------------------------------------------
 void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const
 {
     glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
 }
 
 
 void Shader::SetVec3(const std::string &name, float x, float y, float z) const
 {
     glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
 }
 
 
 // ------------------------------------------------------------------------
 void Shader::SetVec4(const std::string &name, const glm::vec4 &value) const
 {
     glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
 }
 
 
 void Shader::SetVec4(const std::string &name, float x, float y, float z, float w)
 {
     glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
 }
 
 
 // ------------------------------------------------------------------------
 void Shader::SetMat2(const std::string &name, const glm::mat2 &mat) const
 {
     glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
 }
 
 
 // ------------------------------------------------------------------------
 void Shader::SetMat3(const std::string &name, const glm::mat3 &mat) const
 {
     glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
 }
 // ------------------------------------------------------------------------
 void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const
 {
     glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
 }
 
 
 // utility function for checking shader compilation/linking errors.
 // ------------------------------------------------------------------------
 GLint Shader::CheckCompileErrors(GLuint shader, std::string type)
 {
     GLint success;
     GLchar infoLog[1024];
     if ( type != "PROGRAM" )
     {
         glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
         if ( !success )
         {
             glGetShaderInfoLog(shader, 1024, NULL, infoLog);
             std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n------------- -- " << std::endl;
         }
     }
     else
     {
         glGetProgramiv(shader, GL_LINK_STATUS, &success);
         if ( !success )
         {
             glGetProgramInfoLog(shader, 1024, NULL, infoLog);
             std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n----------------- -- " << std::endl;
         }
     }
 
     return success;
 }
 
 };
 
 