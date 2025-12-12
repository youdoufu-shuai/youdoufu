/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CarExitDirection.hpp
 * Author: tron
 *
 * Created on 2023年5月8日, 上午9:37
 */

#ifndef CAREXITDIRECTION_HPP
#define CAREXITDIRECTION_HPP

#include "Include.hpp"
#include "VisualAngleControl.hpp"
#include "../model/Model.hpp"

namespace VirtualView3D {
    using namespace BaseLib;

/*
 * 功能：车辆泊出方向提示
 */
enum class ParkoutType : std::int8_t
{
    DEFAULT = 0, /*!< Default, show nothing */
    LEFT    = 1, /*!< Left, show left arrow */
    RIGHT   = 2, /*!< Right, show right arrow */
} ;

/*
 * 自车泊出方向指示箭头显示
 */
class CarExitDirection
{
public:
    CarExitDirection();

    virtual ~CarExitDirection();

    void Initialization();
private:
    unsigned int textureIDs[2]  = {0};
    GLuint vaoarrow             = 0;
    GLuint vboarrow             = 0;
    Shader arrowShader;
    float mainScaleFactor;               /**< main scale factor */
public:
    void DoRender(const VisualAngleControl &visualAngleControl, const ParkoutType parkoutType);
} ;
};
#endif /* CAREXITDIRECTION_HPP */

