/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RadarAlarmDistance.cpp
 * Author: tron
 * Function: 雷达报警立体色块显示
 * Created on 2023年5月8日, 上午9:33
 */


#include "../tool/TimeHelper.hpp"
#include "../tool/GLHelper.hpp"

#include "RadarAlarmDistance.hpp"

namespace VirtualView3D {

/** @brief YAW PITCH for OpengGL coordinate */
#define RADAR_ALARM_DISTANCE_YAW_GL_COORD           -1.f, 1.f, 0.f
#define RADAR_ALARM_DISTANCE_PITCH_GL_COORD          0.f, 1.f, -1.f
// #define RADAR_ALARM_DISTANCE_TRANS_PADDING_LEFT      0.f, 0.f, 0.f
// #define RADAR_ALARM_DISTANCE_TRANS_PADDING_RIGHT     0.f, 0.f, 0.f


RadarAlarmDistance::RadarAlarmDistance()
{
}

RadarAlarmDistance::~RadarAlarmDistance()
{
    /** @brief delete VAO VBO ...*/
    for (size_t i = 0; i < VV_RADAR_ALARM_DISTANCE_VAO_LEN; i++)
    {
        glDeleteVertexArrays(1, &vaosDistanceFront[i]);
        glDeleteBuffers(1, &vbosDistanceFront[i]);

        glDeleteVertexArrays(1, &vaosDistanceRear[i]);
        glDeleteBuffers(1, &vbosDistanceRear[i]);
    }

    for (size_t j = 0; j < VV_RADAR_ALARM_DISTANCE_TEX_LEN; j++)
    {
        glDeleteTextures(1, &alarmDistanceTextures[j]);
    }

}

void RadarAlarmDistance::Initialization()
{
    Model  model;
    Shader shader;

    /** init for car 3d model */
    long timeus = TimeHelper::GetTimestampMicros();

    /**************************************************************************************************
     * @brief SHADER
     * 
    **************************************************************************************************/
    // this->shaderAlarmDistance.FromSourceFile("resource/junlian/virtualview/shader/alarmDistance.vs", "resource/junlian/virtualview/shader/alarmDistance.fs");	
    this->shaderAlarmDistance.FromSourceFile("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/alarmDistance.vs", "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/alarmDistance.fs");
    
    /**************************************************************************************************
     * @brief LOAD TEXTURES
     * 
    **************************************************************************************************/
    // std::string directoryAlarmDistance = "resource/junlian/virtualview/image/alarmDistance";
    std::string directoryAlarmDistance = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/image/alarmDistance";
    std::string filename               = "";
    for (uint8_t i = 0; i < VV_RADAR_ALARM_DISTANCE_TEX_LEN - 1; i++)
    {
        filename                       = std::to_string(i) + ".png";
        this->alarmDistanceTextures[i] = Model::TextureFromFile(filename.c_str(), directoryAlarmDistance);
    }
    filename                                                         = "cm.png";
    this->alarmDistanceTextures[VV_RADAR_ALARM_DISTANCE_TEX_LEN - 1] = Model::TextureFromFile(filename.c_str(), directoryAlarmDistance);


    const size_t verticeSize = 20 * sizeof(float);
   /**************************************************************************************************
    * @brief INIT VAO FOR FRONT
    * 
   **************************************************************************************************/
    short verticesFrontIdx = 0;
    const float verticesFront[] =
    {

        -10.973531f,12.193157f,-0.201228f, 1.f, 1.f,
        -10.973531f,12.193157f,-0.373709f, 0.f, 1.f,
        -10.831241f,12.335446f,-0.373709f, 0.f, 0.f,
        -10.831241f,12.335446f,-0.201228f, 1.f, 0.f,

        -10.973531f,12.193157f,-0.028747f, 1.f, 1.f,
        -10.973531f,12.193157f,-0.201228f, 0.f, 1.f,
        -10.831241f,12.335446f,-0.201228f, 0.f, 0.f,
        -10.831241f,12.335446f,-0.028747f, 1.f, 0.f,

        -10.973531f,12.193157f, 0.143734f, 1.f, 1.f,
        -10.973531f,12.193157f,-0.028747f, 0.f, 1.f,
        -10.831241f,12.335446f,-0.028747f, 0.f, 0.f,
        -10.831241f,12.335446f, 0.143734f, 1.f, 0.f,

        -10.973531f,12.193157f, 0.373709f, 1.f, 1.f,
        -10.973531f,12.193157f, 0.143734f, 0.f, 1.f,
        -10.831241f,12.335446f, 0.143734f, 0.f, 0.f,
        -10.831241f,12.335446f, 0.373709f, 1.f, 0.f,

    };

    BaseLib::GLHelper::GenRectangleVertexArrayBuffer(verticesFront, verticeSize, this->vaosDistanceFront[verticesFrontIdx], this->vbosDistanceFront[verticesFrontIdx]);
    verticesFrontIdx++;

    BaseLib::GLHelper::GenRectangleVertexArrayBuffer(&verticesFront[20 * verticesFrontIdx], verticeSize, this->vaosDistanceFront[verticesFrontIdx], this->vbosDistanceFront[verticesFrontIdx]);
    verticesFrontIdx++;

    BaseLib::GLHelper::GenRectangleVertexArrayBuffer(&verticesFront[20 * verticesFrontIdx], verticeSize, this->vaosDistanceFront[verticesFrontIdx], this->vbosDistanceFront[verticesFrontIdx]);
    verticesFrontIdx++;

    BaseLib::GLHelper::GenRectangleVertexArrayBuffer(&verticesFront[20 * verticesFrontIdx], verticeSize, this->vaosDistanceFront[verticesFrontIdx], this->vbosDistanceFront[verticesFrontIdx]);
    verticesFrontIdx++;

    assert(verticesFrontIdx == VV_RADAR_ALARM_DISTANCE_VAO_LEN);

    /**************************************************************************************************
    * @brief INIT VAO FOR REAR
    * 
    **************************************************************************************************/
    short verticesRearIdx = 0;
    const float verticesRear[] = 
    {
        -12.055948f,11.110739f,-0.201228f, 1.f, 1.f,
        -12.055948f,11.110739f,-0.373709f, 0.f, 1.f,
        -11.913659f,11.253028f,-0.373709f, 0.f, 0.f,
        -11.913659f,11.253028f,-0.201228f, 1.f, 0.f,

        -12.055948f,11.110739f,-0.028747f, 1.f, 1.f,
        -12.055948f,11.110739f,-0.201228f, 0.f, 1.f,
        -11.913659f,11.253028f,-0.201228f, 0.f, 0.f,
        -11.913659f,11.253028f,-0.028747f, 1.f, 0.f,

        -12.055948f,11.110739f, 0.143734f, 1.f, 1.f,
        -12.055948f,11.110739f,-0.028747f, 0.f, 1.f,
        -11.913659f,11.253028f,-0.028747f, 0.f, 0.f,
        -11.913659f,11.253028f, 0.143734f, 1.f, 0.f,

        -12.055948f,11.110739f, 0.373709f, 1.f, 1.f,
        -12.055948f,11.110739f, 0.143734f, 0.f, 1.f,
        -11.913659f,11.253028f, 0.143734f, 0.f, 0.f,
        -11.913659f,11.253028f, 0.373709f, 1.f, 0.f,
    };

    BaseLib::GLHelper::GenRectangleVertexArrayBuffer(verticesRear, verticeSize, this->vaosDistanceRear[verticesRearIdx], this->vbosDistanceRear[verticesRearIdx]);
    verticesRearIdx++;

    BaseLib::GLHelper::GenRectangleVertexArrayBuffer(&verticesRear[20 * verticesRearIdx], verticeSize, this->vaosDistanceRear[verticesRearIdx], this->vbosDistanceRear[verticesRearIdx]);
    verticesRearIdx++;

    BaseLib::GLHelper::GenRectangleVertexArrayBuffer(&verticesRear[20 * verticesRearIdx], verticeSize, this->vaosDistanceRear[verticesRearIdx], this->vbosDistanceRear[verticesRearIdx]);
    verticesRearIdx++;

    BaseLib::GLHelper::GenRectangleVertexArrayBuffer(&verticesRear[20 * verticesRearIdx], verticeSize, this->vaosDistanceRear[verticesRearIdx], this->vbosDistanceRear[verticesRearIdx]);
    verticesRearIdx++;

    assert(verticesRearIdx == VV_RADAR_ALARM_DISTANCE_VAO_LEN);

    LOGGER_I("RadarAlarmDistance LoadModel duration:%ld(us)", TimeHelper::GetTimestampMicros() - timeus);

    LOGGER_GLE("RadarAlarmDistance::Initialization done");
}


/*调用一次, 传入 vector类型pdcInfo 一帧所有 信息 */
void RadarAlarmDistance::DoRender(const VisualAngleControl &visualAngleControl, std::vector<PdcDistance> &pdcDistanceVector) 
{
    glDisable(GL_DEPTH_TEST);
    GLX_AA_BLEND_ENABLE();

    // float radiansPitch = visualAngleControl.GetPitchRotationVisualAngle();
    float radiansYaw   = visualAngleControl.GetCarModelLeftRightVisualAngle();

    this->shaderAlarmDistance.UseProgram();
    this->shaderAlarmDistance.SetInt("textureDiffuse1", 0);

    GLuint textureIdHundreds = GL_NONE;
    GLuint textureIdTens     = GL_NONE;
    GLuint textureIdOnes     = GL_NONE;

    GLuint vaoIdHundreds     = GL_NONE;
    GLuint vaoIdTens         = GL_NONE;
    GLuint vaoIdOnes         = GL_NONE;
    GLuint vaoIdCm           = GL_NONE;

    glm::mat4 modelMat4;

    float offsetZ = 0.f;

    for (auto & pdcDistance : pdcDistanceVector)
    {
        textureIdHundreds = GL_NONE;
        textureIdTens     = GL_NONE;
        textureIdOnes     = GL_NONE;

        vaoIdHundreds     = GL_NONE;
        vaoIdTens         = GL_NONE;
        vaoIdOnes         = GL_NONE;
        vaoIdCm           = GL_NONE;

        modelMat4 = glm::mat4(1.0f);

        uint16_t distance = pdcDistance.distance;

        textureIdHundreds = (distance / 100);
        
        if (textureIdHundreds > 0)
        {
           if (textureIdHundreds > 9) textureIdHundreds = 9;
           
           textureIdHundreds = this->alarmDistanceTextures[textureIdHundreds];
        }
        else
        {
            textureIdHundreds = GL_NONE;
        }
       
        textureIdTens     = ((distance / 10) % 10);
        if (textureIdHundreds == GL_NONE && (textureIdTens == 0))
        {
            textureIdTens = GL_NONE;
        }
        else
        {
            textureIdTens = this->alarmDistanceTextures[textureIdTens];
        }

        textureIdOnes     = this->alarmDistanceTextures[(distance % 10)];


        if (pdcDistance.position == PdcDistancePosition::FRONT)
        {
            vaoIdHundreds = this->vaosDistanceFront[0];
            vaoIdTens     = this->vaosDistanceFront[1];
            vaoIdOnes     = this->vaosDistanceFront[2];
            vaoIdCm       = this->vaosDistanceFront[3];
        }

        if (pdcDistance.position == PdcDistancePosition::REAR)
        {
            vaoIdHundreds = this->vaosDistanceRear[0];
            vaoIdTens     = this->vaosDistanceRear[1];
            vaoIdOnes     = this->vaosDistanceRear[2];
            vaoIdCm       = this->vaosDistanceRear[3];
        }

        /**************************************************************************************************
         * @brief DRAW ALARM DISTANCE
         * 
         **************************************************************************************************/
        if (textureIdHundreds != GL_NONE)
        {
            /** @brief hundreds */
            glBindTexture(GL_TEXTURE_2D, textureIdHundreds);
            modelMat4 = glm::mat4(1.0f);
            modelMat4 = glm::rotate(modelMat4, radiansYaw, glm::vec3(RADAR_ALARM_DISTANCE_YAW_GL_COORD));
            offsetZ   = 0.18f;

            modelMat4 = glm::translate(modelMat4, glm::vec3(0.f, 0.f, offsetZ));

            this->shaderAlarmDistance.SetMat4("u_Model", modelMat4);
            this->shaderAlarmDistance.SetMat4("u_View", visualAngleControl.viewMat4);
            this->shaderAlarmDistance.SetMat4("u_Projection", visualAngleControl.projectMat4);

            glBindVertexArray(vaoIdHundreds);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        if (textureIdTens != GL_NONE)
        {
            /** @brief tens */
            glBindTexture(GL_TEXTURE_2D, textureIdTens);
            modelMat4 = glm::mat4(1.0f);
            modelMat4 = glm::rotate(modelMat4, radiansYaw, glm::vec3(RADAR_ALARM_DISTANCE_YAW_GL_COORD));

            if (textureIdHundreds != GL_NONE)
            {
                offsetZ   = 0.09f;
                modelMat4 = glm::translate(modelMat4, glm::vec3(0.f, 0.f, offsetZ));
            }
           
            this->shaderAlarmDistance.SetMat4("u_Model", modelMat4);
            this->shaderAlarmDistance.SetMat4("u_View", visualAngleControl.viewMat4);
            this->shaderAlarmDistance.SetMat4("u_Projection", visualAngleControl.projectMat4);

            glBindVertexArray(vaoIdTens);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        if (textureIdOnes != GL_NONE)
        {
            /** @brief ones */
            glBindTexture(GL_TEXTURE_2D, textureIdOnes);
            modelMat4 = glm::mat4(1.0f);
            modelMat4 = glm::rotate(modelMat4, radiansYaw, glm::vec3(RADAR_ALARM_DISTANCE_YAW_GL_COORD));

            if (textureIdHundreds == GL_NONE)
            {
                offsetZ   = -0.09f;
                modelMat4 = glm::translate(modelMat4, glm::vec3(0.f, 0.f, offsetZ));
            }

            this->shaderAlarmDistance.SetMat4("u_Model", modelMat4);
            this->shaderAlarmDistance.SetMat4("u_View", visualAngleControl.viewMat4);
            this->shaderAlarmDistance.SetMat4("u_Projection", visualAngleControl.projectMat4);

            glBindVertexArray(vaoIdOnes);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);

            /** @brief cm */
            glBindTexture(GL_TEXTURE_2D, this->alarmDistanceTextures[10]);
            modelMat4 = glm::mat4(1.0f);
            modelMat4 = glm::rotate(modelMat4, radiansYaw, glm::vec3(RADAR_ALARM_DISTANCE_YAW_GL_COORD));

            if (textureIdHundreds == GL_NONE)
            {
                offsetZ = -0.18f;
            }
            else
            {
                offsetZ = -0.09f;
            }

            modelMat4 = glm::translate(modelMat4, glm::vec3(0.f, 0.f, offsetZ));

            this->shaderAlarmDistance.SetMat4("u_Model", modelMat4);
            this->shaderAlarmDistance.SetMat4("u_View", visualAngleControl.viewMat4);
            this->shaderAlarmDistance.SetMat4("u_Projection", visualAngleControl.projectMat4);

            glBindVertexArray(vaoIdCm);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

    }
   
    // glDisable(GL_DEPTH_TEST);
    GLX_AA_BLEND_DISABLE();
}

};
