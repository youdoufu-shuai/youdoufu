/**************************************************************************************************
 * @file ParkingProgress.cpp
 * @brief 泊车进度显示
 * @version 0.1
 * @date 2023-07-05
 * 
 * @copyright Copyright (c) 2023
 * 
**************************************************************************************************/
#include "../tool/TimeHelper.hpp"

#include "ParkingProgress.hpp"
#include "CarModelAnimation3D.hpp"


namespace VirtualView3D {


ParkingProgress::ParkingProgress()
{
}

ParkingProgress::~ParkingProgress()
{
    glDeleteVertexArrays(1,&this->vaoDistancePrefix);
    glDeleteBuffers(1,&this->vboDistancePrefix);
    glDeleteTextures(1, &this->distancePrefixTextureID);

    glDeleteVertexArrays(1,&this->vaoDistanceInteger);
    glDeleteBuffers(1,&this->vboDistanceInteger);
    glDeleteTextures(20, this->distanceIntegerTextureIDs);

    glDeleteVertexArrays(1,&this->vaoDistanceFractional);
    glDeleteBuffers(1,&this->vboDistanceFractional);
    glDeleteTextures(10, this->distanceFractionalTextureIDs);

    LOGGER_GLE("ParkingProgress::~ParkingProgress done");

}

void ParkingProgress::Initialization()
{
    this->InitTextures();
    this->InitVao();

    // this->shader.FromSourceFile("resource/junlian/virtualview/shader/bar.vs", "resource/junlian/virtualview/shader/bar.fs");
    this->shader.FromSourceFile("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/bar.vs", "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/bar.fs");

}

void ParkingProgress::InitTextures()
{
    long timeus = TimeHelper::GetTimestampMicros();
	
    // std::string directoryParkingDis = "resource/junlian/virtualview/image/parkingDistance";
    std::string directoryParkingDis = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/image/parkingDistance";
    /** @brief distance prefix text */
    std::string filename = "left.png";
    this->distancePrefixTextureID = Model::TextureFromFile(filename.c_str(), directoryParkingDis);

    /** @brief interger part */
    for (int i = 0; i < 20; i++) {
        std::string filename = std::to_string(i) + ".png";
        this->distanceIntegerTextureIDs[i] = Model::TextureFromFile(filename.c_str(), directoryParkingDis);
    }

    /** @brief fractional part */
    for (int i = 0; i < 10; i++) {
        std::string filename = "decimal" + std::to_string(i) + ".png";
        this->distanceFractionalTextureIDs[i] = Model::TextureFromFile(filename.c_str(), directoryParkingDis);
    }

	LOGGER_I("ParkingProgress InitTextures done, duration=%ld(us)", TimeHelper::GetTimestampMicros() - timeus);
}

void ParkingProgress::InitVao(){
    
    /**************************************************************************************************
     * @brief init VAO for distance priefix
    **************************************************************************************************/
    const float verticesDistancePrefix[] = {
        -0.165694f,3.479580f,0.f, 1.f, 1.f,
        -0.938934f,3.479580f,0.f, 0.f, 1.f,
        -0.938934f,3.995073f,0.f, 0.f, 0.f,
        -0.165694f,3.995073f,0.f, 1.f, 0.f
    };

    glGenVertexArrays(1, &this->vaoDistancePrefix);
    glGenBuffers(1, &this->vboDistancePrefix);
    glBindVertexArray(this->vaoDistancePrefix);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboDistancePrefix);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesDistancePrefix), verticesDistancePrefix, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    /**************************************************************************************************
     * @brief init VAO for distance integer
    **************************************************************************************************/
    const float verticesDistanceInteger[] = {
        0.276157f,3.479580f,0.f, 1.f, 1.f,
        -0.165694f,3.479580f,0.f, 0.f, 1.f,
        -0.165694f,3.995073f,0.f, 0.f, 0.f,
        0.276157f,3.995073f,0.f, 1.f, 0.f
    };

    glGenVertexArrays(1, &this->vaoDistanceInteger);
    glGenBuffers(1, &this->vboDistanceInteger);
    glBindVertexArray(this->vaoDistanceInteger);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboDistanceInteger);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesDistanceInteger), verticesDistanceInteger, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    /**************************************************************************************************
     * @brief init VAO for distance fractional part
    **************************************************************************************************/
    const float verticesDistanceFractional[] = {
        0.938934f,3.479580f,0.f, 1.f, 1.f,
        0.276157f,3.479580f,0.f, 0.f, 1.f,
        0.276157f,3.995073f,0.f, 0.f, 0.f,
        0.938934f,3.995073f,0.f, 1.f, 0.f

    };

    glGenVertexArrays(1, &this->vaoDistanceFractional);
    glGenBuffers(1, &this->vboDistanceFractional);
    glBindVertexArray(this->vaoDistanceFractional);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboDistanceFractional);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesDistanceFractional), verticesDistanceFractional, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}


void ParkingProgress::DoRender(const VisualAngleControl &visualAngleControl, const ApaStatus& apaStatus) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);

    glm::mat4 viewMat4 = glm::lookAt(
        glm::vec3(0., 0.f, 16.f),        // Camera position
        glm::vec3(0, 0, 0),         // looks at the origin
        glm::vec3(0, 1, 0)          // Head up
    );

    int textureIndex = this->distancePrefixTextureID;

    int integerPart    = (int)apaStatus.remainDistance;
    int fractionalPart = (int)((apaStatus.remainDistance - integerPart) * 10);
    if (integerPart < 0)  integerPart = 0;
    if (integerPart > 19) integerPart = 19;
    if (fractionalPart < 0) fractionalPart = 0;
    if (fractionalPart > 9) fractionalPart = 9;

    glActiveTexture(GL_TEXTURE0);

    this->shader.UseProgram();
    this->shader.SetInt("textureDiffuse1", 0);

    /**************************************************************************************************
     * @brief render distance prefix
    **************************************************************************************************/
    glBindTexture(GL_TEXTURE_2D, textureIndex);
    
    // Model matrix
    glm::mat4 modelMat4 = glm::mat4(1.0f);

    this->shader.SetMat4("u_Model", modelMat4);
    this->shader.SetMat4("u_View", viewMat4);
    this->shader.SetMat4("u_Projection", visualAngleControl.projectMat4);

    glBindVertexArray(this->vaoDistancePrefix);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    /**************************************************************************************************
     * @brief render distance integer
    **************************************************************************************************/
    textureIndex = this->distanceIntegerTextureIDs[integerPart];
    glBindTexture(GL_TEXTURE_2D, textureIndex);

    // Model matrix
    modelMat4 = glm::mat4(1.0f);

    this->shader.SetMat4("u_Model", modelMat4);
    this->shader.SetMat4("u_View", viewMat4);
    this->shader.SetMat4("u_Projection", visualAngleControl.projectMat4);

    glBindVertexArray(this->vaoDistanceInteger);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    /**************************************************************************************************
     * @brief render distance fraction
    **************************************************************************************************/
    textureIndex = this->distanceFractionalTextureIDs[fractionalPart];
    glBindTexture(GL_TEXTURE_2D, textureIndex);

    // Model matrix
    modelMat4 = glm::mat4(1.0f);

    /// indent for one number when a single integer display.
    if (integerPart < 10) {
        modelMat4 = glm::translate(modelMat4, glm::vec3(-0.2, 0, 0));
    } else {
        modelMat4 = glm::translate(modelMat4, glm::vec3(-0.05, 0, 0));
    }

    this->shader.SetMat4("u_Model", modelMat4);
    this->shader.SetMat4("u_View", viewMat4);
    this->shader.SetMat4("u_Projection", visualAngleControl.projectMat4);

    glBindVertexArray(this->vaoDistanceFractional);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

}

};
