/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VisualAngleControl.cpp
 * Author: tron
 * Function: 视角控制
 * Created on 2023年5月6日, 下午5:37
 */

#include "../tool/Logger.hpp"
#include "../tool/TimeHelper.hpp"

#include "VisualAngleControl.hpp"

namespace VirtualView3D {

#define VV_PERSPECTIVE_FOV                              (45.f)

#define VV_PARKING_COMPLETED_PERSPECTIVE_ANIMATION_LAST (2000.f)  ///< ms, float here
#define VV_PARKING_COMPLETED_PERSPECTIVE_PITCH          (-M_PI_4)

#define VV_PARKING_COMPLETED_PERSPECTIVE_ANGLE_LOWER    (89.99f)
#define VV_PARKING_COMPLETED_PERSPECTIVE_ANGLE_UPPER    (90.01f)

#define VV_PARKING_OUT_PERSPECTIVE_ANGLE_LOWER          (-90.01f)
#define VV_PARKING_OUT_PERSPECTIVE_ANGLE_UPPER          (-89.99f)

VisualAngleControl::VisualAngleControl():carModelYaw(0.f),
                                         yaw(0.f),
                                         virtualParkYaw(0.f),
                                         pitch(0.f),
                                         pitchParkingCompleted(0.f),
                                         pitchDelta(0.f),
                                         scale(1.0f),
                                         parkingCompletionAngle(0.f),
                                         isParkingCompletedMode(false),
                                         isParkingOutMode(false),
                                         parkCompletedTMillisecondsStart(0),
                                         cameraDistanceFactor(VV_CAMERA_DISTANCE_FACTOR),
                                         viewportVec4(0,0, SCR_WIDTH, SCR_HEIGHT)
{
    this->projectMat4 = glm::perspective(glm::radians(VV_PERSPECTIVE_FOV), (float)viewportVec4[2] / viewportVec4[3], ZNEAR, ZFAR);
}

VisualAngleControl::~VisualAngleControl()
{
}

/** @brief 设置视角左右旋转度参考范围[-PI,PI]  单位：弧度 */
void VisualAngleControl::SetCarModelLeftRightVisualAngle(const float angle) 
{
    this->carModelYaw = angle;
}

/** @brief 设置视角左右旋转 0-360度  单位：度*/
void VisualAngleControl::SetLeftRightVisualAngle(const float angle)
{
    this->yaw = angle;
}

/** @brief 设置自车下虚拟车位视角左右旋转度参考范围[-PI,PI]  单位：弧度 */
void VisualAngleControl::SetVirtualParkLeftRightVisualAngle(const float angle) 
{
    this->virtualParkYaw = angle;
}


/** @brief 设置视角俯仰旋转 -70°~70°, -45°（-PI/4）为俯视, 单位：弧度 */
void VisualAngleControl::SetPitchRotationVisualAngle(const float rad)
{
    this->pitch = rad;
}


/*设置视角缩放 float 比例 0.3~2   */
void VisualAngleControl::SetScaleVisualAngle(const float ratio)
{
    if (ratio < VV_VISUAL_SCALE_MIN)
    {
        this->scale = VV_VISUAL_SCALE_MIN;
    } 
    else if(ratio > VV_VISUAL_SCALE_MAX)
    {
        this->scale = VV_VISUAL_SCALE_MAX;
    } 
    else
    {
        this->scale = ratio;
    }
   
}


/**************************************************************************************************
 * @brief 设置泊车完成视角,自车辆模型下显示车位
 * @details  90:车模下不画车位，将视角从当前位置2s时间变为俯视
 *          -90:显示自己车及车下车位，也显示其他感知输入如车位、障碍物
 *        
 * @param angle 单位：度
**************************************************************************************************/
void VisualAngleControl::SetParkingCompletionPerspective(const float angle)
{
    this->parkingCompletionAngle = angle;

    this->isParkingOutMode       = ((this->parkingCompletionAngle > VV_PARKING_OUT_PERSPECTIVE_ANGLE_LOWER) && 
                                    (this->parkingCompletionAngle < VV_PARKING_OUT_PERSPECTIVE_ANGLE_UPPER));

    if (!this->isParkingCompletedMode && 
        (this->parkingCompletionAngle > VV_PARKING_COMPLETED_PERSPECTIVE_ANGLE_LOWER) && 
        (this->parkingCompletionAngle < VV_PARKING_COMPLETED_PERSPECTIVE_ANGLE_UPPER))
    {
        this->isParkingCompletedMode = true;
        this->parkCompletedTMillisecondsStart = BaseLib::TimeHelper::GetTimestampMillis();
        this->pitchParkingCompleted           = this->pitch;

        this->pitchDelta                      = (VV_PARKING_COMPLETED_PERSPECTIVE_PITCH - this->pitchParkingCompleted) / 
                                                VV_PARKING_COMPLETED_PERSPECTIVE_ANIMATION_LAST;

    }
  
    if (this->isParkingCompletedMode && ((this->parkingCompletionAngle <= VV_PARKING_COMPLETED_PERSPECTIVE_ANGLE_LOWER) || 
                                         (this->parkingCompletionAngle >= VV_PARKING_COMPLETED_PERSPECTIVE_ANGLE_UPPER)))
    {
        this->isParkingCompletedMode = false;
    }
   
}


/**************************************************************************************************
 * @brief 设置虚拟视图的viewport
 * @details 虚拟视图viewpor改变时调用一次即可，若无改变则不用调用。
 * @note    不会调用glViewport接口。
 * 
 * @param x      GLint   x of glViewport
 * @param y      GLint   y of glViewport
 * @param width  GLsizei width of glViewport
 * @param height GLsizei height of glViewport
**************************************************************************************************/
void VisualAngleControl::SetViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    assert((width > 0) && (height > 0));

    this->viewportVec4[0] = x;
    this->viewportVec4[1] = y;
    this->viewportVec4[2] = width;
    this->viewportVec4[3] = height;

    this->projectMat4 = glm::perspective(glm::radians(VV_PERSPECTIVE_FOV), (float)viewportVec4[2] / viewportVec4[3], ZNEAR, ZFAR);
}

/**************************************************************************************************
 * @brief Get the viewportVec4 content
 * 
 * @return glm::ivec4 
**************************************************************************************************/
glm::ivec4 VisualAngleControl::GetViewportVec4() const
{
    return this->viewportVec4;
}


void VisualAngleControl::SetCameraDistanceFactor(float value)
{
    this->cameraDistanceFactor = value;
}


/**************************************************************************************************
 * @brief 设置泊车完成视角pitch.
 * @details 该接口由Car3DModelDisplay类的DoRender接口在IsParkingCompletedMode接口为true时调用。
 * @note 仅由库调用
**************************************************************************************************/
void VisualAngleControl::TriggerParkingCompletedPerspectivePitch()
{

    if (this->isParkingCompletedMode)
    {
        long msDu = (BaseLib::TimeHelper::GetTimestampMillis() - this->parkCompletedTMillisecondsStart);

        if (msDu >= VV_PARKING_COMPLETED_PERSPECTIVE_ANIMATION_LAST)
        {
            msDu = VV_PARKING_COMPLETED_PERSPECTIVE_ANIMATION_LAST;
        }

        this->pitchParkingCompleted = this->pitchDelta * msDu;
    }
}


float VisualAngleControl::GetCameraDistanceFactor() const
{
    return this->cameraDistanceFactor;
}


/** @brief 设置车模视角左右旋转度参考范围[-PI,PI]  单位：弧度  */
float VisualAngleControl::GetCarModelLeftRightVisualAngle() const
{
    return this->carModelYaw;
}

/** @brief 设置视角左右旋转度参考范围[-PI,PI]  单位：弧度 */
float VisualAngleControl::GetLeftRightVisualAngle() const
{
    return this->yaw;
}

/** @brief 获取自车下虚拟车位视角左右旋转度参考范围[-PI,PI]  单位：弧度 */
float VisualAngleControl::GetVirtualParkLeftRightVisualAngle() const
{
    return this->virtualParkYaw;
}

 /** @brief 设置视角俯仰旋转 -70°~70°, -45°（-PI/4）为俯视, 单位：弧度 */
float VisualAngleControl::GetPitchRotationVisualAngle() const
{
    if (this->isParkingCompletedMode)
    {
        return this->pitchParkingCompleted;
    }
    else 
    {
        return this->pitch;
    }
    
}
/** @brief 设置视角缩放 float 比例 0.3~2   */
float VisualAngleControl::GetScaleVisualAngle() const
{
    return this->scale;
}

/** @brief 设置泊车完成视角 显示单车位及自己车辆模型 90度显示自己车在当前车位上  单位：度 */
float VisualAngleControl::GetParkingCompletionPerspective() const
{
    return this->parkingCompletionAngle;
}

bool VisualAngleControl::IsParkingCompletedMode() const 
{
    return this->isParkingCompletedMode;
}

bool VisualAngleControl::IsParkingOutMode() const 
{
    return this->isParkingOutMode;
}


};

