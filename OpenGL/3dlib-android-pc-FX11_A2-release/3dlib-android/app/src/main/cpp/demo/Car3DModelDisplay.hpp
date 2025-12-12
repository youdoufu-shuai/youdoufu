/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Car3DModelDisplay.hpp
 * Author: tron
 * Function: 四门两盖3D动效
 * Created on 2023年5月6日, 下午5:42
 */

#ifndef CAR3DMODELDISPLAY_HPP
#define CAR3DMODELDISPLAY_HPP

#include "Include.hpp"
#include "../model/Model.hpp"
#include "VisualAngleControl.hpp"

namespace VirtualView3D
{

#define CFG_KEY_VIRTUAL_VIEW_MODEL_FILE      "virtual_view_model_file"
#define CFG_KEY_VIRTUAL_VIEW_COMPONENT       "component"
#define CFG_KEY_VIRTUAL_VIEW_MAX_VALUE       "max_value"
#define CFG_KEY_VIRTUAL_VIEW_ANIMATION_RATE  "animation_rate"
#define CFG_KEY_VIRTUAL_VIEW_ANIMATION_DELTA "animation_delta"
#define CFG_KEY_VIRTUAL_VIEW_DOOR            "door"
#define CFG_KEY_VIRTUAL_VIEW_HOOD            "hood"
#define CFG_KEY_VIRTUAL_VIEW_TRUNK           "trunk"
#define CFG_KEY_VIRTUAL_VIEW_WHEEL           "wheel"


    using namespace BaseLib;

/* 
* 功能：
*    1、自车车模。
*    2、四门两盖。
*    3、车轮运动方向。
*/

/*
* 四门两盖 轮子按照速度显示 开启关闭过程动效
*/
    struct CarDoorType {
        uint8_t doorDrvrSts;    /*!< 左前车门: 0 车门状态未知; 1 车门打开; 2 车门关闭 */
        uint8_t doorLeReSts;    /*!< 左后车门: 0 车门状态未知; 1 车门打开; 2 车门关闭 */
        uint8_t doorPassSts;    /*!< 右前车门: 0 车门状态未知; 1 车门打开; 2 车门关闭 */
        uint8_t doorRiReSts;    /*!< 右后车门：0 车门状态未知; 1 车门打开; 2 车门关闭 */
        uint8_t hoodSts;        /*!< 引擎盖状态: 0 未知; 1 打开; 2 关闭		  */
        uint8_t trSts;          /*!< 后备箱开关状态: 0 未知; 1  打开; 2 关闭	  */
    };


/* 
* 车辆运动状态 
*/
    enum class CarMotionStatu : std::int8_t {
        STOP     = 0,
        FORWARD  = 1,
        BACKWARD = 2,
    };

/*
 * 四门两盖3D动效
 */
    class Car3DModelDisplay
    {
    public:
        Car3DModelDisplay();
        virtual ~Car3DModelDisplay();
        void Initialization(const std::shared_ptr<YAML::Node> & cfgNodePtr, VisualAngleControl &visualAngleControl);
    private:
        std::pair<Model, Shader> modelspair; /**< model shader pair */
        float rotationAngle[29]    = {0};
        std::shared_ptr<YAML::Node> configNodePtr;  /**< yaml config node */

        /** @brief animation parameters in config file*/
        float doorAnimationMaxValue;
        float doorAnimationDelta;

        float hoodAnimationMaxValue;
        float hoodAnimationDelta;

        float trunkAnimationMaxValue;
        float trunkAnimationDelta;

        float wheelAnimationDelta;

        float mainScaleFactor;               /**< main scale factor */

        glm::mat4 extraRotationMatrix;       /**< 额外的旋转矩阵 */

    public:
        /*设置四门两盖 轮子按照速度显示 开启关闭过程动效*/
        void DoRender(const VisualAngleControl &visualAngleControl, const CarDoorType &doorType, const CarMotionStatu &carMotionStatu);

        /**
         * @brief 设置额外的旋转矩阵
         * @param angle 旋转角度（弧度）
         * @param axis 旋转轴
         */
        void SetExtraRotation(float angle, const glm::vec3& axis);

        /**
         * @brief 重置额外的旋转矩阵为单位矩阵
         */
        void ResetExtraRotation();
    } ;

};

#endif /* CAR3DMODELDISPLAY_HPP */

