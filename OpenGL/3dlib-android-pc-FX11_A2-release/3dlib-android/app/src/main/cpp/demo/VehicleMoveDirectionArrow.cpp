/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VehicleMoveDirectionArrow.cpp
 * Author: tron
 * Function: 自车运动方向箭头
 * Created on 2023年5月8日, 上午9:25
 */

#include "../tool/TimeHelper.hpp"

#include "VehicleMoveDirectionArrow.hpp"
namespace VirtualView3D {

#define VEHICLE_MOVE_YAW_GL_COORD           0.f, 1.f, 0.f
#define VEHICEL_MOVE_PITCH_GL_COORD         0.f, 0.f, -1.f

VehicleMoveDirectionArrow::VehicleMoveDirectionArrow():mainScaleFactor(1.0f)
{
}

VehicleMoveDirectionArrow::~VehicleMoveDirectionArrow()
{
    glDeleteVertexArrays(1,&vaoarrow);
    glDeleteBuffers(1,&vboarrow);
    glDeleteTextures(6, this->textureIDs);
}

void VehicleMoveDirectionArrow::Initialization()
{
    this->InitTextures();
    this->InitArraw();

    // this->arrowShader.FromSourceFile("resource/junlian/virtualview/shader/arrow.vs", "resource/junlian/virtualview/shader/arrow.fs");
    this->arrowShader.FromSourceFile("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/arrow.vs", "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/arrow.fs");

    this->mainScaleFactor = VV_CAMERA_DISTANCE_FACTOR * 4.125f;
}

void VehicleMoveDirectionArrow::InitTextures()
{
    long timeus = TimeHelper::GetTimestampMicros();

    /**************************************************************************************************
     * @brief select
     *        1_00095.png,1_00109.png,1_00123.png,1_00137.png,1_00151.png,1_00165.png 
     * 
    **************************************************************************************************/

	// std::string directory = "resource/junlian/virtualview/image/arrow";
    std::string directory = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/image/arrow";
    std::string filename  = "1_000" + std::to_string(95) + ".png";
    this->textureIDs[0]   = Model::TextureFromFile(filename.c_str(), directory);

    for(int i=0;i<5;i++){
        std::string filename  = "1_00" + std::to_string(14*(i+1)+95) + ".png";
        this->textureIDs[i+1] = Model::TextureFromFile(filename.c_str(), directory);
    }

    LOGGER_I("VehicleMoveDirectionArrow InitTextures done, duration:%ld(us)", TimeHelper::GetTimestampMicros() - timeus);
}

void VehicleMoveDirectionArrow::InitArraw()
{
	float arrowWidth = 0.05f;
	float arrowZ     = 0.01f;
	const float vertices[] = {
		  arrowWidth,  arrowZ, -arrowWidth, 0.0f, 0.0f,
		  arrowWidth,  arrowZ,  arrowWidth, 1.0f, 0.0f,
        2*arrowWidth,  arrowZ,  arrowWidth, 1.0f, 1.0f,
	    2*arrowWidth,  arrowZ, -arrowWidth, 0.0f, 1.0f,
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
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	
}

/*设置自车运动方向箭头 :0 前进 1：后退*/
void VehicleMoveDirectionArrow::DoRender(const VisualAngleControl &visualAngleControl, const MotionDir dir)
{
    glDisable(GL_DEPTH_TEST);
    GLX_AA_BLEND_ENABLE();

    static unsigned int elapsedTime  = 0; 
    elapsedTime                     += 1;
    int textureIndex                 = (int)(elapsedTime/3) % 6;

	float radiansPitch = visualAngleControl.GetPitchRotationVisualAngle();  
	float radiansYaw   = visualAngleControl.GetCarModelLeftRightVisualAngle();

    float scalexyz = this->mainScaleFactor * visualAngleControl.GetScaleVisualAngle();

    if( dir == MotionDir::MOTION_FORWARD)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->textureIDs[textureIndex]);

        this->arrowShader.UseProgram();
        this->arrowShader.SetInt("textureDiffuse1", 0);

        // Model matrix
        glm::mat4 arrowMat4 = glm::mat4(1.0f);
        arrowMat4 = glm::scale(arrowMat4, glm::vec3(scalexyz, scalexyz, scalexyz));
        arrowMat4 = glm::rotate(arrowMat4, radiansPitch, glm::vec3(VEHICEL_MOVE_PITCH_GL_COORD));
        arrowMat4 = glm::rotate(arrowMat4, radiansYaw, glm::vec3(VEHICLE_MOVE_YAW_GL_COORD));

        arrowMat4 = glm::translate(arrowMat4, glm::vec3(0.25f, 0.0f, 0.0f));

        arrowMat4 = glm::scale(arrowMat4, glm::vec3(VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR));

        this->arrowShader.SetMat4("u_Model", arrowMat4);
        this->arrowShader.SetMat4("u_View", visualAngleControl.viewMat4);
        this->arrowShader.SetMat4("u_Projection", visualAngleControl.projectMat4);

        glBindVertexArray(this->vaoarrow);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    if( dir == MotionDir::MOTION_BACK)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->textureIDs[textureIndex]);

        this->arrowShader.UseProgram();
        this->arrowShader.SetInt("textureDiffuse1", 0);

        // Model matrix
        glm::mat4 arrowMat4 = glm::mat4(1.0f);
        arrowMat4 = glm::scale(arrowMat4, glm::vec3(scalexyz, scalexyz, scalexyz));
        arrowMat4 = glm::rotate(arrowMat4, radiansPitch, glm::vec3(VEHICEL_MOVE_PITCH_GL_COORD));
        arrowMat4 = glm::rotate(arrowMat4, radiansYaw, glm::vec3(VEHICLE_MOVE_YAW_GL_COORD));
        arrowMat4 = glm::translate(arrowMat4, glm::vec3(0.03f,0.f,0.f));
        arrowMat4 = glm::rotate(arrowMat4, glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
        arrowMat4 = glm::translate(arrowMat4, glm::vec3(0.1f, 0.0f, 0.0f));


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
    // LOGGER_GLE("VehicleMoveDirectionArrow::DoRender done");
}

};
