/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VirtualView.hpp
 * Author: tron
 * Function: 3D虚拟视图
 * Created on 2023年5月6日, 下午5:27
 */

#ifndef VIRTUALVIEW_HPP
#define VIRTUALVIEW_HPP

#include "../model/YamlLoader.hpp"

#include "Car3DModelDisplay.hpp"
#include "CarExitDirection.hpp"
#include "GearDisplay.hpp"
#include "ObstacleDisplay.hpp"
#include "ParkingDisplay.hpp"
#include "ParkingProgressBar.hpp"
#include "ParkingProgress.hpp"
#include "RadarAlarmDisplay.hpp"
#include "RadarAlarmDistance.hpp"
#include "RadarWaterRipple.hpp"
#include "VehicleMoveDirectionArrow.hpp"
#include "VisualAngleControl.hpp"

// #include "virtualView/ModelInfo.hpp"

namespace VirtualView3D {

/*
 * 虚拟视图
 */
class VirtualView
{
public:

    VirtualView();
    VirtualView(const VirtualView& orig);
    virtual ~VirtualView();

    void Initialization();
private:
    std::shared_ptr<YAML::Node> configNodePtr;  ///< yaml config node

public: 
    /*1. 窗口渲染 removed */
 
    /*2. 离屏渲染 removed */

    /*3. 视角控制*/
    VisualAngleControl visualAngleControl;

    /*4. 自车-车模*/
    Car3DModelDisplay car3DModelDisplay;

    /*5. 障碍物*/
    ObstacleDisplay obstacleDisplay;

    /*6. 车位*/
    ParkingDisplay parkingDisplay;

    /*7. 雷达水波纹*/
    RadarWaterRipple radarWaterRipple;

    /*8. 自车运动方向箭头*/
    VehicleMoveDirectionArrow vehicleMoveDirectionArrow;

    /*9. 泊车进度条*/
    ParkingProgressBar parkingProgressBar;

    /*10. 档位提示*/
    GearDisplay gearDisplay;

    /*11. 雷达报警立体色块显示*/
    RadarAlarmDisplay radarAlarmDisplay;

    /*12. 泊出方向指示箭头*/
    CarExitDirection carExitDirection;

    ParkingProgress parkingProgress;

    RadarAlarmDistance radarAlarmDistance;
} ;
};

#endif /* VIRTUALVIEW_HPP */

