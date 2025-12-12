/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VirtualView.cpp
 * Author: tron
 * Function: 3D虚拟视图
 * Created on 2023年5月6日, 下午5:27
 */

#include <android/log.h>
#include "VirtualView.hpp"
namespace VirtualView3D {

VirtualView::VirtualView()
{
}

VirtualView::~VirtualView()
{
}

void VirtualView::Initialization()
{
    // 注释掉YAML配置加载
    // YamlLoader yamlLoader("avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/config/config-vv.yaml");
    // this->configNodePtr = std::make_shared<YAML::Node>(yamlLoader.loadYamlNode());
    
    this->configNodePtr = std::make_shared<YAML::Node>();

    this->car3DModelDisplay.Initialization(this->configNodePtr, this->visualAngleControl);
    this->obstacleDisplay.Initialization();
    this->vehicleMoveDirectionArrow.Initialization();
    this->parkingDisplay.Initialization(this->configNodePtr);
    this->radarWaterRipple.Initialization();
    this->parkingProgressBar.Initialization();
    this->radarAlarmDisplay.Initialization();
    this->carExitDirection.Initialization();
    this->parkingProgress.Initialization();
    this->radarAlarmDistance.Initialization();
}

};

