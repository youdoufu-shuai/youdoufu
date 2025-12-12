/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VehicleMoveDirectionArrow.hpp
 * Author: tron
 * Function: 自车运动方向箭头
 * Created on 2023年5月8日, 上午9:25
 */

#ifndef VEHICLEMOVEDIRECTIONARROW_HPP
#define VEHICLEMOVEDIRECTIONARROW_HPP

#include "Include.hpp"
#include "VisualAngleControl.hpp"
#include "../model/Model.hpp"

namespace VirtualView3D {

    using namespace BaseLib;

/*
 * 功能：显示自车运动方向
 */
enum class MotionDir : std::int8_t
{
    MOTION_FORWARD = 0,
    MOTION_BACK    = 1,
} ;

/*
 * 自车运动方向箭头显示
 */
class VehicleMoveDirectionArrow
{
public:
    VehicleMoveDirectionArrow();
    virtual ~VehicleMoveDirectionArrow();

    void Initialization();
private:
    void InitTextures();
    void InitArraw();

    unsigned int textureIDs[6] = {0};
    GLuint vaoarrow             = 0;
    GLuint vboarrow             = 0;
    Shader arrowShader;
    float mainScaleFactor;               /**< main scale factor */
public:
    /*设置自车运动方向箭头 :0 前进 1：后退*/
    void DoRender(const VisualAngleControl &visualAngleControl, const MotionDir dir);
} ;
};
#endif /* VEHICLEMOVEDIRECTIONARROW_HPP */

