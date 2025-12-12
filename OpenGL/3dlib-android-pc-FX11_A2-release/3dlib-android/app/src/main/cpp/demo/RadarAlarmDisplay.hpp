/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RadarAlarmDisplay.hpp
 * Author: tron
 * Function: 雷达报警立体色块显示
 * Created on 2023年5月8日, 上午9:33
 */

#ifndef RADARALARMDISPLAY_HPP
#define RADARALARMDISPLAY_HPP

#include <array>
#include "Include.hpp"
#include "../model/Model.hpp"
#include "VisualAngleControl.hpp"

namespace VirtualView3D {

    using namespace BaseLib;

/*
 * 功能：显示雷达报警色块
 * PDC 区域类型
 */
enum class AreaType : std::int8_t
{
    LFM = 0, /*!< 左前中    */
    RFM,     /*!< 右前中    */
    LFC,     /*!< 左前角    */
    RFC,     /*!< 右前角    */
    LRM,     /*!< 左后中    */
    RRM,     /*!< 右后中    */
    LRC,     /*!< 左后角    */
    RRC,     /*!< 右后角    */
    LFS,     /*!< 左前边    */
    RFS,     /*!< 右前边    */
    LRS,     /*!< 左后边    */
    RRS,     /*!< 右后边    */
} ;

/* 
 * PDC 报警等级 
 */
enum class LevelType : std::int8_t
{
    LEVEL_NONE = 0,  /*!< 等级NONE*/
    LEVEL_1,         /*!< 等级1*/
    LEVEL_2,         /*!< 等级2*/
    LEVEL_3,         /*!< 等级3*/
    LEVEL_4,         /*!< 等级4*/
    LEVEL_5,         /*!< 等级5*/
    LEVEL_6,         /*!< 等级6*/
} ;

/* 
 * 数据字段
 */
struct PdcInfo
{
    AreaType type;
    LevelType level;
} ;

/*
 * 雷达报警立体色块显示
 */
class RadarAlarmDisplay
{
public:
    RadarAlarmDisplay();
    void Initialization();
    virtual ~RadarAlarmDisplay();
private:
    std::pair<Model, Shader> modelspair; /**< model shader pair */
    float mainScaleFactor;               /**< main scale factor */
public:
    /*调用一次, 传入 vector类型pdcInfo 一帧所有 信息 */
    void DoRender(const VisualAngleControl &visualAngleControl, std::vector<PdcInfo> &pdcInfo);

    /**************************************************************************************************
     * @brief Get the Pdc Mesh Index Array object
     *        The mesh index bit is 1 means drawing the mesh.
     * 
     * @param pdcInfo 
     * @return std::array<uint32_t, 3> indexArray
     *         indexArray[0] indicates: index range [0,  31]
     *         indexArray[1] indicates: index range [32, 63]
     *         indexArray[2] indicates: index range [64, 71]
    **************************************************************************************************/
    static std::array<uint32_t, 3> GetPdcMeshIndexArray(const std::vector<PdcInfo> &pdcInfo);
} ;
};
#endif /* RADARALARMDISPLAY_HPP */

