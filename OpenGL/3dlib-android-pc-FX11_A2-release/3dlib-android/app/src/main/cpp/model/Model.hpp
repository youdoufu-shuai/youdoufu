/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Model.hpp
 * Author: tron
 * Function: oad model
 * Created on 2023年5月9日, 上午11:00
 */

#ifndef MODEL_HPP
#define MODEL_HPP

#include "Mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

namespace BaseLib{

using namespace BaseLib;


class Model
{
public:

    // model data 
    std::vector<Texture> texturesLoaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh>    meshes;
    std::string directory;
    std::string path;

    /* the global Assimp scene object */
    // const C_STRUCT aiScene* scene = NULL;
    C_STRUCT aiVector3D sceneMin, sceneMax, sceneCenter;

    std::vector<glm::vec3> meshsCenter;

    Model();
    // constructor, expects a filepath to a 3D model.
    // Model(string const &path, bool gamma = false);
    Model(std::string const &path) : path(path){};
    
    ~Model();

    //传递路径参数
    void Initialization(std::string const &path, bool gamma = false);

    // draws the model, and thus all its meshes
    void Draw(Shader &shader);

    void Draw(Shader &shader, std::vector<Texture> &textures);

    /**
     * @brief draw mesh by specified mesh index with given shader
     * 
     * @param meshIdx unsigned int mesh index
     * @param shader  Shader       compiled shader
     */
    void DrawMesh(unsigned int meshIdx, Shader &shader);

    void DrawMesh(unsigned int meshIdx, Shader &shader, std::vector<Texture> &textures);

    /**
     * @brief Get the frustum fit scale value
     * 
     * @return float 
     */
    float GetFrustumFitScale();

    float GetMaxViewDistance();

    glm::vec3 GetAdjustModelPosVec();

public:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void LoadModel();


    static unsigned int TextureFromFile(const char *path, const std::string &directory);
    
    /**************************************************************************************************
     * @brief Load Texture
     * 
     * @param typeName  std::string  type name, one of:
     *                               {texture_diffuse, texture_specular, texture_normal, texture_height}
     * @param path      std::string file path
     * @param directory std::string directory
     * @return Texture 
     **************************************************************************************************/
    static Texture LoadTexture(std::string typeName, std::string path, const std::string &directory);


    /**************************************************************************************************
     * @brief parse .pvr format file and generate a texture
     * 
     * @param filename pvr format file.
     * @param textureID generated texture, a valid texture id would be > 0
    **************************************************************************************************/
    static void ParsePVR(const char *filename, unsigned int* textureID);

private:
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void ProcessNode(aiNode *node, const aiScene *scene);

    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);


    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);


    void GetBoundBox(C_STRUCT aiVector3D* min, C_STRUCT aiVector3D* max, const C_STRUCT aiScene* scene);

    glm::vec3 GetMeshCenter(unsigned int meshIndex, C_STRUCT aiMesh* mesh);

    /**
     * @brief Get the Bound Box for node object
     * 
     * @param nd 
     * @param min 
     * @param max 
     * @param trafo 
     */
    void GetBoundBox4node (const C_STRUCT aiNode* nd,
            C_STRUCT aiVector3D* min,
            C_STRUCT aiVector3D* max,
            C_STRUCT aiMatrix4x4* trafo,
            const C_STRUCT aiScene* scene
            );

} ;

};

#endif /* MODEL_HPP */

