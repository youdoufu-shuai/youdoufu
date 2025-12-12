/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Model.cpp
 * Author: tron
 * Function: oad model
 * Created on 2023年5月9日, 上午11:00
 */

/**************************************************************************************************
 * @file model.hpp
 * @brief load model
 * 
 * @details 
 * 
 *  HISTORY
 *  -----------------------------------------------------------------------------------------------
 *  Version   Date        Author    Description
 *  -----------------------------------------------------------------------------------------------
 *  0.1       2023-04-20  TDZ       Migrate from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h
 *
 *          
**************************************************************************************************/
#include <sys/stat.h>
#include <android/log.h>
#include "Model.hpp"
//#include <../thirdparty/OpenCV-android-sdk/sdk/native/jni/include/opencv2/opencv.hpp>
#include "../thirdparty/stb/include/stb_image.h"
#include "../thirdparty/stb/include/stb_image_write.h"


namespace BaseLib{

/** @brief check define for PVRTC */
#ifndef GL_IMG_texture_compression_pvrtc
#define GL_IMG_texture_compression_pvrtc 1
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03
#endif /* GL_IMG_texture_compression_pvrtc */

#ifndef GL_IMG_texture_compression_pvrtc2
#define GL_IMG_texture_compression_pvrtc2 1
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG 0x9137
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG 0x9138
#endif /* GL_IMG_texture_compression_pvrtc2 */

#ifndef GL_EXT_pvrtc_sRGB
#define GL_EXT_pvrtc_sRGB 1
#define GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT 0x8A54
#define GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT 0x8A55
#define GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT 0x8A56
#define GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT 0x8A57
#define GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG 0x93F0
#define GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG 0x93F1
#endif /* GL_EXT_pvrtc_sRGB */

#ifndef GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG
    #define GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG 0x93F0
#endif /* GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG */

#ifndef GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG
    #define GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG 0x93F1
#endif /* GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG */


/**
 * @brief Compression pixel format
 * more info see
 * https://docs.imgtec.com/specifications/pvr-file-format-specification/topics/pvr-header-format.html
 */
enum class CompressedPixelFormat:std::uint64_t
{
    PVRTCI_2bpp_RGB = 0,
    PVRTCI_2bpp_RGBA,
    PVRTCI_4bpp_RGB,
    PVRTCI_4bpp_RGBA,
    PVRTCII_2bpp,
    PVRTCII_4bpp,
};

Model::Model()
{
}

Model::~Model()
{
}

void Model::Initialization(std::string const &path, bool gamma)
{
    this->path = path;
}

// draws the model, and thus all its meshes
void Model::Draw(Shader &shader)
{
    for ( unsigned int i = 0; i < meshes.size(); i++ )
        meshes[i].Draw(shader, meshes[i].textures);
}

// draws the model, and thus all its meshes
void Model::Draw(Shader &shader, std::vector<Texture> &textures)
{
    for ( unsigned int i = 0; i < meshes.size(); i++ )
        meshes[i].Draw(shader, textures);
}

/**
 * @brief draw mesh by specified mesh index with given shader
 *
 * @param meshIdx unsigned int mesh index
 * @param shader  Shader       compiled shader
 */
void Model::DrawMesh(unsigned int meshIdx, Shader &shader)
{
    assert(meshIdx < this->meshes.size());
    this->meshes[meshIdx].Draw(shader, meshes[meshIdx].textures);
}

void Model::DrawMesh(unsigned int meshIdx, Shader &shader, std::vector<Texture> &textures)
{
    assert(meshIdx < this->meshes.size());
    this->meshes[meshIdx].Draw(shader, textures);
}


/**
 * @brief Get the frustum fit scale value
 *
 * @return float
 */
float Model::GetFrustumFitScale()
{
    float result = 0;
    result = sceneMax.x - sceneMin.x;
    result = fmax((sceneMax.y - sceneMin.y), result);
    result = fmax((sceneMax.z - sceneMin.z), result);

    return result;
}
float Model::GetMaxViewDistance()
{
    glm::vec3 vec3 = (abs(glm::vec3(sceneMin.x, sceneMin.y, sceneMin.z)) + abs(glm::vec3(sceneMax.x, sceneMax.y, sceneMax.z))) / 2.0f;
    float maxDis = fmax(vec3.x, fmax(vec3.y, vec3.z));
    return maxDis;
}
glm::vec3 Model::GetAdjustModelPosVec()
{
    glm::vec3 vec3(sceneCenter.x, sceneCenter.y, sceneCenter.z);

    return vec3;
}

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::LoadModel()
{
    // read file via ASSIMP
    Assimp::Importer importer;
    //aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
    // aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // const aiScene* scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    // check for errors
    if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    GetBoundBox(&sceneMin, &sceneMax, scene);
    sceneCenter.x = (sceneMin.x + sceneMax.x) / 2.0f;
    sceneCenter.y = (sceneMin.y + sceneMax.y) / 2.0f;
    sceneCenter.z = (sceneMin.z + sceneMax.z) / 2.0f;

    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    ProcessNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for ( unsigned int i = 0; i < node->mNumMeshes; i++ )
    {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));

        meshsCenter.push_back(GetMeshCenter(i, mesh));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for ( unsigned int i = 0; i < node->mNumChildren; i++ )
    {
        ProcessNode(node->mChildren[i], scene);
    }

}
Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // walk through each of the mesh's vertices
    for ( unsigned int i = 0; i < mesh->mNumVertices; i++ )
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        for(int k = 0; k < 4; k++)
        {
            vertex.mBoneIDs[k] = 0;
            vertex.mWeights[k] = 0.f;
        }
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        // normals
        if ( mesh->HasNormals() )
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // texture coordinates
        if ( mesh->mTextureCoords[0] ) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        }
        else
            vertex.texCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for ( unsigned int i = 0; i < mesh->mNumFaces; i++ )
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for ( unsigned int j = 0; j < face.mNumIndices; j++ )
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN


    // 1. diffuse maps
    std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());


    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<Texture> Model::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    // static int count = 0;
    std::vector<Texture> textures;
    unsigned int textureCount = mat->GetTextureCount(type);
    // __android_log_print(ANDROID_LOG_INFO, "Model", "Loading material textures of type %s, count=%u", typeName.c_str(), textureCount);

    for ( unsigned int i = 0; i < textureCount; i++ )
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for ( unsigned int j = 0; j < texturesLoaded.size(); j++ )
        {
            if ( std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0 )
            {
                textures.push_back(texturesLoaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                // __android_log_print(ANDROID_LOG_INFO, "Model", "Reusing existing texture %s", str.C_Str());
                break;
            }
        }
        if ( !skip )
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            texturesLoaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
            // __android_log_print(ANDROID_LOG_INFO, "Model", "Loaded new texture %s", str.C_Str());
        }
    }
    return textures;
}


/**************************************************************************************************
 * @brief Load Texture
 *
 * @param typeName  std::string  type name, one of:
 *                               {texture_diffuse, texture_specular, texture_normal, texture_height}
 * @param path      std::string file path
 * @param directory std::string directory
 * @return Texture
**************************************************************************************************/
Texture Model::LoadTexture(std::string typeName, std::string path, const std::string &directory)
{
    Texture texture;
    texture.id   = TextureFromFile(path.c_str(), directory);
    texture.type = typeName;
    texture.path = path;

    // printf("\n texture.type=%s\n, texture.path = %s \n", texture.type.c_str(), texture.path.c_str());

    return texture;
}


void Model::GetBoundBox(C_STRUCT aiVector3D* min, C_STRUCT aiVector3D* max, const C_STRUCT aiScene* scene)
{
    C_STRUCT aiMatrix4x4 trafo;
    aiIdentityMatrix4(&trafo);

    min->x = min->y = min->z =  1e10f;
    max->x = max->y = max->z = -1e10f;
    GetBoundBox4node(scene->mRootNode, min, max, &trafo, scene);
}
glm::vec3 Model::GetMeshCenter(unsigned int meshIndex, C_STRUCT aiMesh* mesh)
{
    unsigned int i;
    glm::vec3 tmp;
    glm::vec3 tmpMin(1e10f, 1e10f, 1e10f);
    glm::vec3 tmpMax(-1e10f, -1e10f, -1e10f);
    glm::vec3 center;

    for ( i = 0; i < mesh->mNumVertices; i++ )
    {
        tmp      = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        tmpMin.x = fmin(tmp.x, tmpMin.x);
        tmpMin.y = fmin(tmp.y, tmpMin.y);
        tmpMin.z = fmin(tmp.z, tmpMin.z);

        tmpMax.x = fmax(tmp.x, tmpMax.x);
        tmpMax.y = fmax(tmp.y, tmpMax.y);
        tmpMax.z = fmax(tmp.z, tmpMax.z);
    }

    center = (tmpMin + tmpMax) / 2.0f;

    return center;
}
/**
 * @brief Get the Bound Box for node object
 *
 * @param nd
 * @param min
 * @param max
 * @param trafo
 */
void Model::GetBoundBox4node (const C_STRUCT aiNode* nd,
        C_STRUCT aiVector3D* min,
        C_STRUCT aiVector3D* max,
        C_STRUCT aiMatrix4x4* trafo,
        const C_STRUCT aiScene* scene
        )
{
    C_STRUCT aiMatrix4x4 prev;
    unsigned int n = 0, t;

    prev = *trafo;
    aiMultiplyMatrix4(trafo, &nd->mTransformation);

    for ( ; n < nd->mNumMeshes; ++n )
    {
        const C_STRUCT aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
        for ( t = 0; t < mesh->mNumVertices; ++t )
        {

            C_STRUCT aiVector3D tmp = mesh->mVertices[t];
            aiTransformVecByMatrix4(&tmp, trafo);

            min->x = fmin(min->x, tmp.x);
            min->y = fmin(min->y, tmp.y);
            min->z = fmin(min->z, tmp.z);

            max->x = fmax(max->x, tmp.x);
            max->y = fmax(max->y, tmp.y);
            max->z = fmax(max->z, tmp.z);
        }
    }

    for ( n = 0; n < nd->mNumChildren; ++n )
    {
        GetBoundBox4node(nd->mChildren[n], min, max, trafo, scene);
    }

    *trafo = prev;
}

unsigned int Model::TextureFromFile(const char *path, const std::string &directory)
{
    // __android_log_print(ANDROID_LOG_INFO, "Model","TextureFromFile start path=%s, dir=%s", path, directory.c_str());
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID = 0;

#if 0
    std::string filenamePvr = filename + ".pvr";
    struct stat fileStat;
    int statRet = stat(filenamePvr.c_str(), &fileStat);
    if (statRet == 0)
    {
        /** @brief find texture .pvr file and gen texture */
        ParsePVR(filenamePvr.c_str(), &textureID);

        // __android_log_print(ANDROID_LOG_INFO, "Model","TextureFromFile ParsePVR filename=%s, textureID=%u", filenamePvr.c_str(), textureID);
    }

    /** @brief invalid textureID, use the origin texture image file */
    if(textureID == 0)
    {
        /** @brief original texture image file, read by stb_image */
        glGenTextures(1, &textureID);
        int width, height, channels;
        // 加载图像，强制使用RGBA格式
        stbi_set_flip_vertically_on_load(false); // 保持与OpenCV相同的图像方向
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
        GLenum format         = GL_RGBA;
        GLint  internalformat = GL_RGBA;

        if (data)
        {
            // 根据通道数确定格式
            if (channels == 4)
            {
                // 对于4通道图像，需要将BGRA转换为RGBA
                for (int i = 0; i < width * height; i++)
                {
                    // 交换R和B通道
                    unsigned char temp = data[i * 4];
                    data[i * 4] = data[i * 4 + 2];
                    data[i * 4 + 2] = temp;
                }
                format = GL_RGBA;
                internalformat = GL_RGBA;
            }
            else if (channels == 3)
            {
                // 对于3通道图像，需要将BGR转换为RGB
                for (int i = 0; i < width * height; i++)
                {
                    // 交换R和B通道
                    unsigned char temp = data[i * 3];
                    data[i * 3] = data[i * 3 + 2];
                    data[i * 3 + 2] = temp;
                }
                format = GL_RGB;
                internalformat = GL_RGB;
            }

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // 释放图像数据
            stbi_image_free(data);
        }
        else
        {
            // __android_log_print(ANDROID_LOG_ERROR, "Model", "Failed to load texture: %s", filename.c_str());
        }
    }

#else // GL_ES_TDA

    /** @brief original texture image file, read by stb_image */
    glGenTextures(1, &textureID);
    int width, height, channels;
    // 加载图像，强制使用RGBA格式
    stbi_set_flip_vertically_on_load(false); // 保持与OpenCV相同的图像方向
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    GLenum format         = GL_RGBA;
    GLint  internalformat = GL_RGBA;

    if (data)
    {
        // 根据通道数确定格式
        if (channels == 4)
        {
            // 对于4通道图像，需要将BGRA转换为RGBA
            for (int i = 0; i < width * height; i++)
            {
                // 交换R和B通道
                unsigned char temp = data[i * 4];
                data[i * 4] = data[i * 4 + 2];
                data[i * 4 + 2] = temp;
            }
            format = GL_RGBA;
            internalformat = GL_RGBA;
        }
        else if (channels == 3)
        {
            // 对于3通道图像，需要将BGR转换为RGB
            for (int i = 0; i < width * height; i++)
            {
                // 交换R和B通道
                unsigned char temp = data[i * 3];
                data[i * 3] = data[i * 3 + 2];
                data[i * 3 + 2] = temp;
            }
            format = GL_RGB;
            internalformat = GL_RGB;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 释放图像数据
        stbi_image_free(data);
    }
    else
    {
        // __android_log_print(ANDROID_LOG_ERROR, "Model","Failed to load texture: %s", filename.c_str());
    }

#endif // GL_ES_TDA

    // __android_log_print(ANDROID_LOG_INFO, "Model","TextureFromFile done, filename:%s", filename.c_str());

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

/**************************************************************************************************
 * @brief parse .pvr format file and generate a texture
 *
 * @param filename pvr format file.
 * @param textureID generated texture, a valid texture id would be > 0
**************************************************************************************************/
void Model::ParsePVR(const char *filename, unsigned int* textureID)
{
#ifdef GL_ES_TDA
    // __android_log_print(ANDROID_LOG_INFO, "Model","ParsePVR: %s", filename);
    *textureID = GL_NONE;

    bool checkResult = true;

    FILE* mpFile = fopen(filename, "rb");

    unsigned char *mpData        = nullptr;
    unsigned char *originDataPtr = nullptr;
    unsigned char *pCurrentPos   = nullptr;

    uint32_t uiSmallestWidth  = 1;
    uint32_t uiSmallestHeight = 1;
    uint32_t uiSmallestDepth  = 1;
    uint32_t bitsPerPixel     = 0;

    GLenum internalformat = GL_RGBA;

    uint64_t nFormat;
    uint32_t nColorSpace;
    uint32_t nPicWid;
    uint32_t nPicHgt;
    uint32_t nPicDept;
    uint32_t nSurface;
    // uint32_t nFace;
    // uint32_t nSize;
    uint32_t nLevel;
    uint32_t nMetaData;

    /** @brief file length, PVR header size=52 */
    fseek(mpFile, 0, SEEK_END);
    int nFileLen = ftell(mpFile);

    if(nFileLen < 52)
    {
        LOGGER_I("invalid PowerVR file format, cause: fileLen=%d", nFileLen);
        checkResult = false;
    }

    if (checkResult)
    {
        mpData        = (unsigned char *)malloc(nFileLen * sizeof(unsigned char));
        originDataPtr = mpData;

        memset(mpData, 0, nFileLen);
        rewind(mpFile);

        int nError = fread(mpData, sizeof(mpData[0]), nFileLen, mpFile);

        if (nError != nFileLen)
        {
            // __android_log_print(ANDROID_LOG_INFO, "Model","invalid PowerVR file format, cause: fread IO error");
            checkResult = false;
        }
    }

    if (checkResult)
    {
        if (mpData[0] != 'P' || mpData[1] != 'V' || mpData[2] != 'R')
        {
            // __android_log_print(ANDROID_LOG_INFO, "Model", "invalid PowerVR file format, cause: first 3 characters are not PVR");
            checkResult = false;
        }
    }

    fclose(mpFile);

    if (checkResult)
    {
         /** @brief PVR header info */
        nFormat     = *(uint64_t*)(mpData + 8);
        nColorSpace = *(uint32_t*)(mpData + 16);
        nPicHgt     = *(uint32_t*)(mpData + 24);
        nPicWid     = *(uint32_t*)(mpData + 28);
        nPicDept    = *(uint32_t*)(mpData + 32);
        nSurface    = *(uint32_t*)(mpData + 36);
        // nFace    = *(uint32_t*)(mpData + 40);
        nLevel      = *(uint32_t*)(mpData + 44);
        nMetaData   = *(uint32_t*)(mpData + 48);

        /** @note don't not support PVR texture array */
        assert(nSurface == 1);

        mpData    += nMetaData + 52;

        /** @brief use lRGB better  */
        nColorSpace = 0;

        switch (nFormat)
        {
            /** @brief PVRTCI 2bpp*/
            case static_cast<uint64_t>(CompressedPixelFormat::PVRTCI_2bpp_RGB):
                bitsPerPixel     = 2;
                if (nColorSpace == 0) internalformat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
                else                  internalformat = GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT;
                uiSmallestWidth  = 16;
                uiSmallestHeight = 8;
                uiSmallestDepth  = 1;
                break;
            case static_cast<uint64_t>(CompressedPixelFormat::PVRTCI_2bpp_RGBA):
                bitsPerPixel     = 2;
                if (nColorSpace == 0) internalformat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
                else                  internalformat = GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT;
                uiSmallestWidth  = 16;
                uiSmallestHeight = 8;
                uiSmallestDepth  = 1;
                break;
            /** @brief PVRTCI 4bpp*/
            case static_cast<uint64_t>(CompressedPixelFormat::PVRTCI_4bpp_RGB):
                bitsPerPixel     = 4;
                if (nColorSpace == 0) internalformat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
                else                  internalformat = GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT;
                uiSmallestWidth  = 8;
                uiSmallestHeight = 8;
                uiSmallestDepth  = 1;
                break;
            case static_cast<uint64_t>(CompressedPixelFormat::PVRTCI_4bpp_RGBA):
                bitsPerPixel     = 4;
                if (nColorSpace == 0) internalformat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
                else                  internalformat = GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT;
                uiSmallestWidth  = 8;
                uiSmallestHeight = 8;
                uiSmallestDepth  = 1;
                break;
            /** @brief PVRTCII 2bpp */
            case static_cast<uint64_t>(CompressedPixelFormat::PVRTCII_2bpp):
                bitsPerPixel     = 2;
                if (nColorSpace == 0) internalformat = GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG;
                else                  internalformat = GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG;
                uiSmallestWidth  = 8;
                uiSmallestHeight = 4;
                uiSmallestDepth  = 1;
                break;
            /** @brief PVRTCII 4bpp */
            case static_cast<uint64_t>(CompressedPixelFormat::PVRTCII_4bpp):
                bitsPerPixel     = 4;
                if (nColorSpace == 0) internalformat = GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG;
                else                  internalformat = GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG;
                uiSmallestWidth  = 4;
                uiSmallestHeight = 4;
                uiSmallestDepth  = 1;
                break;
            default:
                bitsPerPixel = 0;
                break;
        }

        if (bitsPerPixel == 0)
        {
            // __android_log_print(ANDROID_LOG_INFO, "Model", "invalid PowerVR file format, cause: unsupport format %lu", nFormat);
            checkResult = false;
        }
    }

    if (checkResult)
    {
        pCurrentPos = mpData;

        glGenTextures(1, textureID);
        glBindTexture(GL_TEXTURE_2D, *textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        uint32_t dataSizePre = 0;

        /** @brief parse for texture data */
        for (uint32_t i=0; i < nLevel; i++)
        {
            /** @brief level and padding for min */
            uint32_t uiWidth      = std::max<uint32_t>(nPicWid >> i, 1);
            uint32_t uiHeight     = std::max<uint32_t>(nPicHgt >> i, 1);
            uint32_t uiDepth      = std::max<uint32_t>(nPicDept >> i, 1);
            uint32_t uiSizeWidth  = uiWidth  + ((-1 * uiWidth)  % uiSmallestWidth);
            uint32_t uiSizeHeight = uiHeight + ((-1 * uiHeight) % uiSmallestHeight);
            uint32_t uiSizeDepth  = uiDepth  + ((-1 * uiDepth)  % uiSmallestDepth);

            /** @brief calcaulte data size */
            uint64_t uiDataSize = static_cast<uint64_t>(bitsPerPixel) * static_cast<uint64_t>(uiSizeWidth) * static_cast<uint64_t>(uiSizeHeight) * static_cast<uint64_t>(uiSizeDepth);
            uint32_t dataSize   = static_cast<uint32_t>(uiDataSize / 8);

            glCompressedTexImage2D(GL_TEXTURE_2D, i, internalformat, uiWidth, uiHeight, 0 , dataSize, (void*)pCurrentPos);

            if (dataSizePre != dataSize)
            {
                pCurrentPos = pCurrentPos + dataSize;
                dataSizePre = dataSize;
            }

            LOGGER_GLE("glCompressedTexImage2D");
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        LOGGER_GLE("ParsePVR glTexParameteri");
    }

   if (originDataPtr)
   {
       free(originDataPtr);
   }
#else
    // Non-GL_ES_TDA implementation
    *textureID = 0;
//    __android_log_print(ANDROID_LOG_INFO, "Model", "ParsePVR not implemented for non-GL_ES_TDA platforms");
#endif

    return;
}

};
