/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ParkingProgressBar.hpp
 * Author: tron
 * Function: 泊车进度显示
 * Created on 2023年5月8日, 上午9:26
 */

#ifndef PARKINGPROGRESSBAR_HPP
#define PARKINGPROGRESSBAR_HPP

#include "Include.hpp"
#include "VisualAngleControl.hpp"
#include "../model/Model.hpp"
#include "GearDisplay.hpp"

namespace VirtualView3D {

    using namespace BaseLib;

class ParkingProgressBar
{
public:
    ParkingProgressBar();
    void Initialization();
    virtual ~ParkingProgressBar();
private:
    void InitTextures();
    void InitBar();
    unsigned int textureIDs[24] =  {0};
    GLuint vaobar               =   0;
    GLuint vbobar               =   0;
    Shader barShader;
    float mainScaleFactor;               /**< main scale factor */
public:
    /*传入参数 进度百分比 progressPercentage = 0-100%*/
    void DoRender(const VisualAngleControl &visualAngleControl, const uint8_t progressPercentage, const GearPosition gear);
} ;
};
#endif /* PARKINGPROGRESSBAR_HPP */

