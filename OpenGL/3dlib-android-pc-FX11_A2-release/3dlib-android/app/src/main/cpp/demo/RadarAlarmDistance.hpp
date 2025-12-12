/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RadarAlarmDistance.hpp
 * Author: tron
 * Function: 雷达报警立体色块显示
 * Created on 2023年5月8日, 上午9:33
 */

#pragma once

#include <array>
#include "Include.hpp"
#include "../model/Model.hpp"
#include "VisualAngleControl.hpp"

namespace VirtualView3D {

#define VV_RADAR_ALARM_DISTANCE_TEX_LEN (11U)
#define VV_RADAR_ALARM_DISTANCE_VAO_LEN (4U)

using namespace BaseLib;


/** @brief Alarm Distance text position */
enum class PdcDistancePosition : std::int8_t
{
    FRONT = 0, /**< front */
    REAR,      /**< rear */
};

/** @brief Alarm Distance */
struct PdcDistance
{
    PdcDistancePosition position; /**< position */
    uint16_t            distance; /**< distance in cm */
};


/*
 * 雷达报警立体色块显示
 */
class RadarAlarmDistance
{
public:
    RadarAlarmDistance();
    void Initialization();
    virtual ~RadarAlarmDistance();
private:
    Shader shaderAlarmDistance;
    GLuint alarmDistanceTextures[VV_RADAR_ALARM_DISTANCE_TEX_LEN]; /**< textures for alarm distance, 0~9 and cm */
    GLuint vaosDistanceFront[VV_RADAR_ALARM_DISTANCE_VAO_LEN];     /**< VAO array for front distance */
    GLuint vbosDistanceFront[VV_RADAR_ALARM_DISTANCE_VAO_LEN];     /**< VBO array for front distance */
    GLuint vaosDistanceRear[VV_RADAR_ALARM_DISTANCE_VAO_LEN];      /**< VAO array for rear distance */
    GLuint vbosDistanceRear[VV_RADAR_ALARM_DISTANCE_VAO_LEN];      /**< VBO array for rear distance */    
public:
    /** 调用一次, 传入 vector类型pdcInfo 一帧所有 信息 */
    void DoRender(const VisualAngleControl &visualAngleControl, std::vector<PdcDistance> &pdcDistanceVector);

};
};

