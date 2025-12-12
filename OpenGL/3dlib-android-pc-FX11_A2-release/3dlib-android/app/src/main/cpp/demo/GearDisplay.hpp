/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GearDisplay.hpp
 * Author: tron
 * Function: 档位显示
 * Created on 2023年5月8日, 上午9:28
 */

#ifndef GEARDISPLAY_HPP
#define GEARDISPLAY_HPP

#include "Include.hpp"
// #include "ModelInfo.hpp"

namespace VirtualView3D {

/*
 * 功能：显示挡位
 */
enum class GearPosition : std::int8_t
{
    GEAR_P = 0, /*!< 0: P档；*/
    GEAR_R = 1, /*!< 1: R档；*/
    GEAR_N = 2, /*!< 2: N档；*/
    GEAR_D = 3, /*!< 3: D档；*/
    GEAR_M = 4, /*!< 4: M档；*/
} ;

/*
 *档位显示
 */
class GearDisplay
{
public:
    GearDisplay();
    virtual ~GearDisplay();
private:
public:
    void DoRender(const GearPosition gearPosition);
} ;
};
#endif /* GEARDISPLAY_HPP */

