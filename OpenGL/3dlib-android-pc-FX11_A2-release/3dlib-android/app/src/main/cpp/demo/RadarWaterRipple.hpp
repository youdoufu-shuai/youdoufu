/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RadarWaterRipple.hpp
 * Author: tron
 * Function: 雷达水波纹显示
 * Created on 2023年5月8日, 上午9:14
 */

#ifndef RADARWATERRIPPLE_HPP
#define RADARWATERRIPPLE_HPP

#include "Include.hpp"
#include "VisualAngleControl.hpp"
#include "../model/Model.hpp"

namespace VirtualView3D {

    using namespace BaseLib;

/*
* 水波纹特效只是 显示和不显示
*/
class RadarWaterRipple
{
public:
    RadarWaterRipple();
    virtual ~RadarWaterRipple();

    void Initialization();
private:
    void InitTextures();
    void InitWave();

    unsigned int textureIDs[45] = {0};
    GLuint vaowave              =   0;
    GLuint vbowave              =   0;
    Shader waveShader;
    float mainScaleFactor;               /**< main scale factor */

    GLuint textureIndex;
public:
    /*设置雷达水波纹显示*/
    void DoRender(const VisualAngleControl &visualAngleControl, const bool flag);
} ;
};
#endif /* RADARWATERRIPPLE_HPP */

