/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Mesh.hpp
 * Author: tron
 * Function: make mesh for model
 * Created on 2023年5月9日, 上午10:20
 */

#ifndef MESH_HPP
#define MESH_HPP

#include "Shader.hpp"

namespace BaseLib{


struct Vertex
{
    // position
    glm::vec3 position;
    // normal
    glm::vec3 normal;
    // texCoords
    glm::vec2 texCoords;
    // tangent
    glm::vec3 tangent;
    // bitangent
    glm::vec3 bitangent;
    //bone indexes which will influence this vertex
    int mBoneIDs[4];  //< MAX_BONE_INFLUENCE = 4;
    //weights from each bone
    float mWeights[4];  //< MAX_BONE_INFLUENCE = 4;
} ;

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
} ;

class Mesh
{
public:
    Mesh();
    ~Mesh();
public:
    // mesh Data
    // std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int VAO;
    // render data 
    unsigned int VBO, EBO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    void Draw(Shader &shader, std::vector<Texture> &textures);

private:
    // initializes all the buffer objects/arrays
    static void SetupMesh(unsigned int *VAO, unsigned int *VBO, unsigned *EBO, std::vector<Vertex> & vertices, std::vector<unsigned int> & indices);
} ;
};

#endif /* MESH_HPP */

