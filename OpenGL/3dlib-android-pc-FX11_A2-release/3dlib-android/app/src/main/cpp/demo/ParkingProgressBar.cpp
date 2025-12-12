/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ParkingProgressBar.cpp
 * Author: tron
 * Function: 泊车进度显示
 * Created on 2023年5月8日, 上午9:26
 */

#include "../tool/TimeHelper.hpp"

#include "ParkingProgressBar.hpp"
#include "Model.hpp"
#include "CarModelAnimation3D.hpp"


namespace VirtualView3D {
ParkingProgressBar::ParkingProgressBar():mainScaleFactor(1.0f)
{
}

ParkingProgressBar::~ParkingProgressBar()
{
    glDeleteVertexArrays(1,&vaobar);
    glDeleteBuffers(1,&vbobar);
    glDeleteTextures(24, this->textureIDs);
}

void ParkingProgressBar::Initialization()
{
    this->InitTextures();
    this->InitBar();

    // this->barShader.FromSourceFile("resource/junlian/virtualview/shader/bar.vs", "resource/junlian/virtualview/shader/bar.fs");
    this->barShader.FromSourceFile("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/bar.vs", "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/bar.fs");

    this->mainScaleFactor = 4.0f;
}

void ParkingProgressBar::InitTextures()
{
    long timeus = TimeHelper::GetTimestampMicros();
	// std::string directory = "resource/junlian/virtualview/image/parking_bar";
    std::string directory = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/image/parking_bar";
	for(int i=0;i<12;i++){
		std::string filename = "D_ic_loading_" + std::to_string(i) + ".png";
		this->textureIDs[i] = BaseLib::Model::TextureFromFile(filename.c_str(), directory);
	}
    for(int i=0;i<12;i++){
		std::string filename = "R_ic_loading_" + std::to_string(i) + ".png";
		this->textureIDs[i+12] = Model::TextureFromFile(filename.c_str(), directory);
	}

	LOGGER_I("ParkingProgressBar InitTextures done, duration=%ld(us)", TimeHelper::GetTimestampMicros() - timeus);
}

void ParkingProgressBar::InitBar(){
    float barWidth = 0.2f;
	float barZ     = 0.01f;
	const float vertices[] = {  
		  barWidth, barZ, -barWidth, 0.0f, 0.0f,
		  barWidth, barZ,  barWidth, 1.0f, 0.0f,
		 -barWidth, barZ,  barWidth, 1.0f, 1.0f,
		 -barWidth, barZ, -barWidth, 0.0f, 1.0f,
	};

	glGenVertexArrays(1, &this->vaobar);
    glGenBuffers(1, &this->vbobar);
	glBindVertexArray(this->vaobar);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbobar);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

/*传入参数 进度百分比 progressPercentage = 0-100%*/
void ParkingProgressBar::DoRender(const VisualAngleControl &visualAngleControl, const uint8_t progressPercentage, const GearPosition gear) {

    /** @brief only suppor gear R and D */
    if ((gear != GearPosition::GEAR_D) && (gear != GearPosition::GEAR_R)) return;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float scalexyz = this->mainScaleFactor;

    glActiveTexture(GL_TEXTURE0);
    
    int textureIndex = (int)(fmin(progressPercentage, 100) * 11/100);
    if(gear == GearPosition::GEAR_D) glBindTexture(GL_TEXTURE_2D, this->textureIDs[textureIndex]);
    if(gear == GearPosition::GEAR_R) glBindTexture(GL_TEXTURE_2D, this->textureIDs[textureIndex+12]);
    
    this->barShader.UseProgram();
    this->barShader.SetInt("textureDiffuse1", 0);

    // Model matrix
    glm::mat4 barMat4 = glm::mat4(1.0f);
    barMat4 = glm::scale(barMat4, glm::vec3(scalexyz, scalexyz, scalexyz));
    barMat4 = glm::translate(barMat4, glm::vec3(-2.f, 0.5f, 0.0f));

    barMat4 = glm::scale(barMat4, glm::vec3(VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR));

    this->barShader.SetMat4("u_Model", barMat4);
    this->barShader.SetMat4("u_View", visualAngleControl.viewMat4);
    this->barShader.SetMat4("u_Projection", visualAngleControl.projectMat4);

    glBindVertexArray(this->vaobar);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);

    // glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

};
