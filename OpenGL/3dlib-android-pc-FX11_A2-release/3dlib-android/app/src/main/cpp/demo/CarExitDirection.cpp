/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CarExitDirection.cpp
 * Author: tron
 * 
 * Created on 2023年5月8日, 上午9:37
 */

#include "../tool/TimeHelper.hpp"

#include "CarExitDirection.hpp"
#include "Model.hpp"
#include "CarModelAnimation3D.hpp"

namespace VirtualView3D {

/** @brief YAW PITCH for OpengGL coordinate */
#define CAR_EXIT_YAW_GL_COORD           0.f, 1.f, 0.f
#define CAR_EXIT_PITCH_GL_COORD         0.f, 0.f, -1.f

CarExitDirection::CarExitDirection():mainScaleFactor(1.0f)
{
}

CarExitDirection::~CarExitDirection()
{
    glDeleteVertexArrays(1,&this->vaoarrow);
    glDeleteBuffers(1,&this->vboarrow);
    glDeleteTextures(2, this->textureIDs);
}

void CarExitDirection::Initialization()
{
    /** @brief init texture */
    long timeus = TimeHelper::GetTimestampMicros();
    // std::string directory = "resource/junlian/virtualview/image/parkout";
    std::string directory = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/image/parkout";
    for(int i=0;i<2;i++){
        std::string filename = std::to_string(i+1) + ".png";
        this->textureIDs[i] = Model::TextureFromFile(filename.c_str(), directory);
    }
    
    

    /** @brief init vao */
    float arrowWidth = 0.05f;
    float arrowZ     = 0.01f;

    const float vertices[] = {  
        -1.f*arrowWidth, arrowZ, -1.6f*arrowWidth, 0.0f, 1.0f,
        -1.f*arrowWidth, arrowZ,  1.6f*arrowWidth, 1.0f, 1.0f,
         4.f*arrowWidth, arrowZ,  1.6f*arrowWidth, 1.0f, 0.0f,
         4.f*arrowWidth, arrowZ, -1.6f*arrowWidth, 0.0f, 0.0f,
    };

    glGenVertexArrays(1, &this->vaoarrow);
    glGenBuffers(1, &this->vboarrow);
    glBindVertexArray(this->vaoarrow);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboarrow);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    this->arrowShader.FromSourceFile("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/arrow.vs", "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/arrow.fs");

    this->mainScaleFactor = VV_CAMERA_DISTANCE_FACTOR * 4.125f;

    LOGGER_I("CarExitDirection Initialization done, duration:%ld(us)", TimeHelper::GetTimestampMicros() - timeus);
}

void CarExitDirection::DoRender(const VisualAngleControl &visualAngleControl, const ParkoutType parkoutType) {
    glDisable(GL_DEPTH_TEST);
    GLX_AA_BLEND_ENABLE();

    float radiansPitch = visualAngleControl.GetPitchRotationVisualAngle();
    float radiansYaw   = visualAngleControl.GetCarModelLeftRightVisualAngle();

    float scalexyz = this->mainScaleFactor * visualAngleControl.GetScaleVisualAngle(); 

    if(parkoutType == ParkoutType::LEFT)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->textureIDs[0]);
        this->arrowShader.UseProgram();
        this->arrowShader.SetInt("textureDiffuse1", 0);

        // Model matrix
        glm::mat4 arrowMat4 = glm::mat4(1.0f);
        
        arrowMat4 = glm::scale(arrowMat4, glm::vec3(scalexyz, scalexyz, scalexyz));
        arrowMat4 = glm::rotate(arrowMat4, radiansPitch, glm::vec3(CAR_EXIT_PITCH_GL_COORD));
        arrowMat4 = glm::rotate(arrowMat4, radiansYaw, glm::vec3(CAR_EXIT_YAW_GL_COORD));

        arrowMat4 = glm::translate(arrowMat4, glm::vec3(0.20f, 0.0f, -0.05f));

        arrowMat4 = glm::scale(arrowMat4, glm::vec3(VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR));

        this->arrowShader.SetMat4("u_Model", arrowMat4);
        this->arrowShader.SetMat4("u_View", visualAngleControl.viewMat4);
        this->arrowShader.SetMat4("u_Projection", visualAngleControl.projectMat4);

        glBindVertexArray(this->vaoarrow);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    if(parkoutType == ParkoutType::RIGHT)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->textureIDs[1]);

        this->arrowShader.UseProgram();
        this->arrowShader.SetInt("textureDiffuse1", 0);
      
        // Model matrix
        glm::mat4 arrowMat4 = glm::mat4(1.0f);
        arrowMat4 = glm::scale(arrowMat4, glm::vec3(scalexyz, scalexyz, scalexyz));
        arrowMat4 = glm::rotate(arrowMat4, radiansPitch, glm::vec3(CAR_EXIT_PITCH_GL_COORD));
        arrowMat4 = glm::rotate(arrowMat4, radiansYaw, glm::vec3(CAR_EXIT_YAW_GL_COORD));
        arrowMat4 = glm::translate(arrowMat4, glm::vec3(0.20f, 0.0f, 0.05f));

        arrowMat4 = glm::scale(arrowMat4, glm::vec3(VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR));

        this->arrowShader.SetMat4("u_Model", arrowMat4);
        this->arrowShader.SetMat4("u_View", visualAngleControl.viewMat4);
        this->arrowShader.SetMat4("u_Projection", visualAngleControl.projectMat4);

        glBindVertexArray(this->vaoarrow);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // glDisable(GL_DEPTH_TEST);
    GLX_AA_BLEND_DISABLE();
}

};
