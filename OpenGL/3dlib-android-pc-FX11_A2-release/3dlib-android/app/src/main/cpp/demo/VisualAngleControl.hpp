/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VisualAngleControl.hpp
 * Author: tron
 * Function: 视角控制
 * Created on 2023年5月6日, 下午5:37
 */

#ifndef VISUALANGLECONTROL_HPP
#define VISUALANGLECONTROL_HPP

#include "Include.hpp"

namespace VirtualView3D {

#define ZNEAR		                     (5.f)
#define ZFAR                             (2000.f)
#define SCR_WIDTH                        (1800)
#define SCR_HEIGHT                       (720)
#define COLOR_CHANNEL                    (4)
#define SHARED_MEM_ALIEN                 (1024)

#define VV_CAMERA_DISTANCE_FACTOR        (2.95f) //(2.825f)
#define VV_MODEL_SCALE_FACTOR            (0.95f)

#define VV_VISUAL_SCALE_MIN              (0.3f)
#define VV_VISUAL_SCALE_MAX              (2.f)

#define VIRTUAL_VIEW_FPS                 (20)

#define VV_CAR_MODEL_COORD_CENTER_OFFSET (1023.f)

/*
 *缩放功能： float 比例 0.3~2
 *水平视角 0-360度
 *垂直视角 -70-  70
 *泊车完成标志
 */
class VisualAngleControl
{
public:
    VisualAngleControl();
    virtual ~VisualAngleControl();
private:
    float carModelYaw;                     /**< left right angle, [-PI, PI] radians */
    float yaw;                             /**< left right angle,     [-PI, PI], radians */
    float virtualParkYaw;                  /**< left right angle for virtual park under car model, [-PI, PI], radians */
    float pitch;                           /**< pitch rotation,       angle range:[-70°, 70° ], rad */
    float pitchParkingCompleted;           /**< pitch for parking completed perspective, rad. */
    float pitchDelta;                      /**< pitch delta, rad */
    float scale;                           /**< scale of visual angle [0.3, 2  ] */
    float parkingCompletionAngle;          /**< parking completion view angle*/
    bool  isParkingCompletedMode;          /**< flag for parking completed mode */
    bool  isParkingOutMode;                /**< flag for parking out mode */
    long  parkCompletedTMillisecondsStart; /**< ms timestamp for parking completed mode */

    float cameraDistanceFactor;            /**< camera distance factor */

    glm::ivec4 viewportVec4;               /**< viewport vec4 GLint x, GLint y, GLsizei width, GLsizei height */

public:
    int winWidth      = SCR_WIDTH;
    int winHeight     = SCR_HEIGHT;
    int colorChannels = COLOR_CHANNEL;

    glm::mat4 viewMat4 = glm::lookAt(
        glm::vec3(-16.f, 16.f, 0.f),        // Camera position
        glm::vec3(0, 0, 0),         // looks at the origin
        glm::vec3(0, 1, 0)          // Head up
    );

    glm::mat4 projectMat4;          // glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 2000.f);
public:
    /** @brief 设置车模视角左右旋转度参考范围[-PI,PI]  单位：弧度 */
    void SetCarModelLeftRightVisualAngle(const float angle);

    /** @brief 设置视角左右旋转度参考范围[-PI,PI]  单位：弧度 */
    void SetLeftRightVisualAngle(const float angle);

    /** @brief 设置自车下虚拟车位视角左右旋转度参考范围[-PI,PI]  单位：弧度 */
    void SetVirtualParkLeftRightVisualAngle(const float angle);

    /** @brief 设置视角俯仰旋转 -70°~70°, -45°（-PI/4）为俯视, 单位：弧度 */
    void SetPitchRotationVisualAngle(const float rad);

    /** @brief 设置视角缩放 float 比例 0.3~2 */
    void SetScaleVisualAngle(const float ratio);

    /**************************************************************************************************
     * @brief 设置泊车完成视角,自车辆模型下显示车位
     * @details  90:车模下不画车位，将视角从当前位置2s时间变为俯视
     *          -90:显示自己车及车下车位，也显示其他感知输入如车位、障碍物
     *        
     * @param angle 单位：度
    **************************************************************************************************/
    void SetParkingCompletionPerspective(const float angle);

    /**************************************************************************************************
     * @brief 设置虚拟视图的viewport参数
     * @details 虚拟视图的viewpor改变时调用一次即可，若无改变则不用调用。
     * @note    不会调用glViewport接口。
     * 
     * @param x      GLint   x of glViewport
     * @param y      GLint   y of glViewport
     * @param width  GLsizei width of glViewport
     * @param height GLsizei height of glViewport
    **************************************************************************************************/
    void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height);

    void SetCameraDistanceFactor(float value);

    void SetMainModelScaleFactor(float value);

    /**************************************************************************************************
     * @brief 设置泊车完成视角pitch.
     * @details 该接口由Car3DModelDisplay类的DoRender接口在IsParkingCompletedMode接口为true时调用。
     * @note 仅由库调用
    **************************************************************************************************/
    void TriggerParkingCompletedPerspectivePitch();

    /**************************************************************************************************
     * @brief Get the viewportVec4 content
     * 
     * @return glm::ivec4 
    **************************************************************************************************/
    glm::ivec4 GetViewportVec4() const;

    /** @brief 获取车模视角左右旋转度参考范围[-PI,PI]  单位：弧度 */
    float GetCarModelLeftRightVisualAngle() const; // 不行

    /** @brief 获取视角左右旋转度参考范围[-PI,PI]  单位：弧度 */
    float GetLeftRightVisualAngle() const; // 不行

    /** @brief 获取自车下虚拟车位视角左右旋转度参考范围[-PI,PI]  单位：弧度 */
    float GetVirtualParkLeftRightVisualAngle() const;

    /** @brief 获取视角俯仰旋转 -70-70°    单位：弧度 */
    float GetPitchRotationVisualAngle() const;

    /** @brief 获取视角缩放 float 比例 0.3~2   */
    float GetScaleVisualAngle() const;

    /** @brief 获取泊车完成视角 显示单车位及自己车辆模型 90度显示自己车在当前车位上  单位：度 */
    float GetParkingCompletionPerspective() const;

    float GetCameraDistanceFactor() const;

    bool IsParkingCompletedMode() const;

    bool IsParkingOutMode() const;



} ;
};

#endif /* VISUALANGLECONTROL_HPP */

