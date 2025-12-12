/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RadarWaterRipple.cpp
 * Author: tron
 * Function: 雷达水波纹显示
 * Created on 2023年5月8日, 上午9:14
 */

#include "../tool/TimeHelper.hpp"

#include "RadarWaterRipple.hpp"

namespace VirtualView3D {

/** @brief YAW PITCH for OpengGL coordinate */
#define RADAR_WATER_YAW_GL_COORD           0.f, 1.f, 0.f
#define RADAR_WATER_PITCH_GL_COORD         0.f, 0.f, -1.f

#define VV_RADAR_SATER_MODEL_SCALE         (VV_MODEL_SCALE_FACTOR*1.1f)

#define VV_RADAR_WATER_TEX_NUM             (44)
#define VV_RADAR_WATER_TEX_IDX_MAX         (VV_RADAR_WATER_TEX_NUM-1)

RadarWaterRipple::RadarWaterRipple():mainScaleFactor(1.0f),textureIndex(0)
{
}

RadarWaterRipple::~RadarWaterRipple()
{
    glDeleteVertexArrays(1, &vaowave);
    glDeleteBuffers(1, &vbowave);
    glDeleteTextures(45, this->textureIDs);
}

void RadarWaterRipple::Initialization()
{
    this->InitTextures();
    this->InitWave();

    // this->waveShader.FromSourceFile("resource/junlian/virtualview/shader/waterRipple.vs", "resource/junlian/virtualview/shader/waterRipple.fs");
    this->waveShader.FromSourceFile("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/waterRipple.vs", "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/waterRipple.fs");

    this->mainScaleFactor = VV_CAMERA_DISTANCE_FACTOR * 4.125f;
}

void RadarWaterRipple::InitTextures()
{
    long timeus = TimeHelper::GetTimestampMicros();
	// std::string directory = "resource/junlian/virtualview/image/wave";
    std::string directory = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/image/wave";

	for(int i=0; i<10; i++){
		std::string filename = "1_0000" + std::to_string(i) + ".png";
		this->textureIDs[i] = Model::TextureFromFile(filename.c_str(), directory);
	}

	for(int i=10; i<VV_RADAR_WATER_TEX_NUM; i++){
		std::string filename = "1_000" + std::to_string(i) + ".png";
		this->textureIDs[i] = Model::TextureFromFile(filename.c_str(), directory);
	}

	LOGGER_I("RadarWaterRipple InitTextures done, duration:%ld(us)", TimeHelper::GetTimestampMicros() - timeus);
}

void RadarWaterRipple::InitWave(){
    float waveWidth = 0.2f;
	float waveZ     = 0.01f;
	const float vertices[] = {  
		  waveWidth, waveZ, -waveWidth, 0.0f, 0.0f,
		  waveWidth, waveZ,  waveWidth, 1.0f, 0.0f,
		 -waveWidth, waveZ,  waveWidth, 1.0f, 1.0f,
		 -waveWidth, waveZ, -waveWidth, 0.0f, 1.0f,
	};

	glGenVertexArrays(1, &this->vaowave);
    glGenBuffers(1, &this->vbowave);
	glBindVertexArray(this->vaowave);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbowave);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}


/*设置雷达水波纹显示*/
void RadarWaterRipple::DoRender(const VisualAngleControl &visualAngleControl, const bool flag){
    glDisable(GL_DEPTH_TEST);
    // glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_ONE, GL_ZERO);

    static unsigned int elapsedTime  = 0;
    elapsedTime                     += 1;

    if (elapsedTime > 22)
    {
        this->textureIndex = (unsigned int)(elapsedTime/3) % 21 + 23;
    } 
    else
    {
        this->textureIndex = (unsigned int)(elapsedTime/3) % 44;
    }

    float radiansPitch = visualAngleControl.GetPitchRotationVisualAngle(); 
    float radiansY     = visualAngleControl.GetCarModelLeftRightVisualAngle();
    float scalexyz     = this->mainScaleFactor * visualAngleControl.GetScaleVisualAngle();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->textureIDs[this->textureIndex]);
    this->waveShader.UseProgram();
    this->waveShader.SetInt("textureDiffuse1", 0);

    // Model matrix
    glm::mat4 waveMat4 = glm::mat4(1.0f);
    waveMat4 = glm::scale(waveMat4, glm::vec3(scalexyz, scalexyz, scalexyz));
    waveMat4 = glm::rotate(waveMat4, radiansPitch, glm::vec3(RADAR_WATER_PITCH_GL_COORD));
    waveMat4 = glm::rotate(waveMat4, radiansY, glm::vec3(RADAR_WATER_YAW_GL_COORD));
    
    waveMat4 = glm::translate(waveMat4, glm::vec3(0.08f, 0.0f, 0.f));

    waveMat4 = glm::scale(waveMat4, glm::vec3(VV_RADAR_SATER_MODEL_SCALE, VV_RADAR_SATER_MODEL_SCALE, VV_RADAR_SATER_MODEL_SCALE));

    this->waveShader.SetMat4("u_Model", waveMat4);
    this->waveShader.SetMat4("u_View", visualAngleControl.viewMat4);
    this->waveShader.SetMat4("u_Projection", visualAngleControl.projectMat4);

    glBindVertexArray(this->vaowave);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);

    // glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    // glDepthMask(GL_TRUE);
    // LOGGER_GLE("RadarWaterRipple::DoRender done");
}

};

