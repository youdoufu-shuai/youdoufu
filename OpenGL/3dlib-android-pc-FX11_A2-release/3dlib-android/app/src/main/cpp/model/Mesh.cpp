/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Mesh.cpp
 * Author: tron
 * Function: make mesh for model
 * Created on 2023年5月9日, 上午10:20
 */

/**************************************************************************************************
 * @file shader.hpp
 * @brief make mesh for model.
 * 
 * @details 
 * 
 * 
 *  HISTORY
 *  -----------------------------------------------------------------------------------------------
 *  Version   Date        Author    Description
 *  -----------------------------------------------------------------------------------------------
 *  0.1       2023-04-20  TDZ       Migrate from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/mesh.h
 *
 * 
**************************************************************************************************/

#include <android/log.h>
#include "Mesh.hpp"

namespace BaseLib {

Mesh::Mesh()
{
}
Mesh::~Mesh()
{
}

/*
 *  constructor
 */
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->indices = indices;
    this->textures = textures;

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    Mesh::SetupMesh(&VAO, &VBO, &EBO, vertices, indices);
}

void Mesh::Draw(Shader &shader, std::vector<Texture> &textures)
{
    // bind appropriate textures
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;
    unsigned int luminNr    = 1;
    for ( unsigned int i = 0; i < textures.size(); i++ )
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].type;
        if ( name == "texture_diffuse" )
            number = std::to_string(diffuseNr++);
        else if ( name == "texture_specular" )
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if ( name == "texture_normal" )
            number = std::to_string(normalNr++);   // transfer unsigned int to string
        else if ( name == "texture_height" )
            number = std::to_string(heightNr++);   // transfer unsigned int to string

        else if ( name == "texture_lumin" )
            number = std::to_string(luminNr++);   // transfer unsigned int to string

        std::string yes = "texture_diffuse1";

        // shader.UseProgram();
        // 获取着色器文件路径信息
        const char* vertexPath = shader.vertexPath.c_str();
        const char* fragmentPath = shader.fragmentPath.c_str();
        int size = textures.size();

        GLint uniformLocOffset = glGetUniformLocation(shader.ID, "offset");
        glUniform1f(uniformLocOffset, 0.5f);

        // now set the sampler to the correct texture unit
        GLint uniformLoc = glGetUniformLocation(shader.ID, (name + number).c_str());
        // GLint uniformLoc = glGetUniformLocation(shader.ID, yes.c_str());
        glUniform1i(uniformLoc, i);
        // glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // draw mesh
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, (void *)0);

    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}


/*
 * initializes all the buffer objects/arrays
 */
void Mesh::SetupMesh(unsigned int *VAO, unsigned int *VBO, unsigned *EBO, std::vector<Vertex> & vertices, std::vector<unsigned int> & indices)
{
    // create buffers/arrays
    glGenVertexArrays(1, VAO);

    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);

    glBindVertexArray(*VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, mBoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mWeights));
    glBindVertexArray(0);

}
};