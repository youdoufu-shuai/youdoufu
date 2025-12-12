/**************************************************************************************************
 * @file ParkingProgress.hpp
 * @brief 泊车进度显示
 * @version 0.1
 * @date 2023-07-05
 * 
 * @copyright Copyright (c) 2023
 * 
**************************************************************************************************/
#pragma once

#include "Include.hpp"
#include "VisualAngleControl.hpp"
#include "../model/Model.hpp"
#include "GearDisplay.hpp"

namespace VirtualView3D {

    using namespace BaseLib;

    /** @brief APA 状态 */
    struct ApaStatus {
        float remainDistance;  ///< 剩余距离，单位米，1位小数
    };

class ParkingProgress
{
public:
    ParkingProgress();
    void Initialization();
    virtual ~ParkingProgress();
private:
    void InitTextures();
    void InitVao();
    
    Shader shader;

    /**************************************************************************************************
     * @brief remain distance
    **************************************************************************************************/
    GLuint vaoDistancePrefix                = 0;     ///< VAO 剩余
    GLuint vboDistancePrefix                = 0;     ///< VBO 剩余

    GLuint vaoDistanceInteger               = 0;     ///< VAO 整数
    GLuint vboDistanceInteger               = 0;     ///< VBO 整数

    GLuint vaoDistanceFractional            = 0;     ///< VAO 小数
    GLuint vboDistanceFractional            = 0;     ///< VBO 小数

    GLuint distancePrefixTextureID          = 0;     ///< 纹理ID, 距离前缀提示文字，显示“剩余”
    GLuint distanceIntegerTextureIDs[20]    = {0};   ///< 纹理ID数组，显示整数, [0,19]
    GLuint distanceFractionalTextureIDs[10] = {0};   ///< 纹理ID, 显示小数和单位,[.0m,.9m]
    
public:
    void DoRender(const VisualAngleControl &visualAngleControl, const ApaStatus& parkStatus);
} ;
};


