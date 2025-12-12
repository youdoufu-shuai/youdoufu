/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ObstacleDisplay.hpp
 * Author: tron
 * Function: 障碍物显示
 * Created on 2023年5月6日, 下午5:57
 */

#ifndef OBSTACLEDISPLAY_HPP
#define OBSTACLEDISPLAY_HPP

#include "Include.hpp"
#include "../model/Model.hpp"
#include "VisualAngleControl.hpp"


namespace VirtualView3D {

    using namespace BaseLib;

/* 
 *功能：
 *    1、障碍物类型，显示具体的障碍物（例如：轿车、SUV、卡车、公交车，锥桶(雪糕筒)），和障碍物朝向。
 *    2、障碍物运动状态。
 *    3、障碍物大小比例与自车辆相同 ，只看类型，大小固定显示，
 *    4、行人模型：静态、动态
 *    5、二轮车模型：自行车、摩托车  ---带上人模型
 */
    enum class ObstacleType : std::int8_t
    {
        OBSTACLE_TYPE_UNKOWN            = 0,
        OBSTACLE_TYPE_CAR               = 1,
        OBSTACLE_TYPE_TRUCK             = 2,
        OBSTACLE_TYPE_BUS               = 3,
        OBSTACLE_TYPE_TRAM              = 4,
        OBSTACLE_TYPE_ENGINEERCAR       = 5,
        OBSTACLE_TYPE_CART              = 6,
        OBSTACLE_TYPE_TRICYCLE          = 7,
        OBSTACLE_TYPE_MOTOCYCLE         = 8,
        OBSTACLE_TYPE_ELECTRIC_BICYCLE  = 9,
        OBSTACLE_TYPE_BICYCLE           = 10,
        OBSTACLE_TYPE_PEDESTRIAN        = 11,
        OBSTACLE_TYPE_TRAFFICCONE       = 12, /// TODO
    } ;

/*
* 障碍运动状态
*/
    enum class ObstacleMotionStatu : std::int8_t {
        STOP        = 0,
        SPORT       = 1,
    };

/*
 * 位置坐标  单位：m
 */
    struct Coordinate
    {
        double x;
        double y;
        double z;
        /*目标朝向0度，车头朝向，弧度yaw  单位：弧度*/
        double angle;
    } ;

/*
 *障碍物属性
 */
    struct Obstacle
    {
        int32_t id;   /*唯一性  融合追踪ID*/
        ObstacleType type;
        ObstacleMotionStatu statu;
        Coordinate coordinate;
    } ;

/*
 *障碍物显示
 */

    class ObstacleDisplay
    {
    public:
        ObstacleDisplay();
        virtual ~ObstacleDisplay();
    public:

        /**************************************************************************************************
         * @brief Init
         * @details load 3d models here
        **************************************************************************************************/
        void Initialization();

        /*设置显示障碍物 传入障碍物vector列表*/
        void DoRender(const VisualAngleControl &visualAngleControl, std::vector<Obstacle>  &obstacles);

        void SetExtraRotation(float angle, const glm::vec3& axis);
    private:
        std::map<ObstacleType, std::pair<Model, Shader> >  modelsmap; /**< model shader map*/
        float mainScaleFactor;                                        /**< main scale factor */
        float mainTransFactor;                                        /**< main transform factor */

        glm::mat4 extraRotationMat4;
    };
};

#endif /* OBSTACLEDISPLAY_HPP */

