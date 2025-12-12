/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RadarAlarmDisplay.cpp
 * Author: tron
 * Function: 雷达报警立体色块显示
 * Created on 2023年5月8日, 上午9:33
 */

/**************************************************************************************************
 * @brief 索引表
 * 
 * 索引	位置(顺时针旋转后的位置)	等级
 * 0	右前中	                   6
 * 1	左后边	                   4
 * 2	左前边	                   4
 * 3	左前角	                   4
 * 4	左前中	                   4
 * 5	右前中	                   4
 * 6	右前角	                   4
 * 7	右前边	                   4
 * 8	右后边	                   4
 * 9	右后角	                   4
 * 10	右后中	                   4
 * 11	左后中	                   4
 * 12	左后角	                   4
 * 13	右后边	                   6
 * 14	右后边	                   5
 * 15	右后边	                   3
 * 16	右后边	                   2
 * 17	右后边	                   1
 * 18	左前边	                   6
 * 19	左前边	                   5
 * 20	左前边	                   3
 * 21	左前边	                   2
 * 22	左前边	                   1
 * 23	右前角	                   5
 * 24	右前边	                   5
 * 25	右后角	                   5
 * 26	右后中	                   5
 * 27	左后中	                   5
 * 28	左后角	                   5
 * 29	左后边	                   5
 * 30	左前角	                   5
 * 31	左后边	                   3
 * 32	左前角	                   3
 * 33	左前中	                   3
 * 34	右前中	                   3
 * 35	右前角	                   3
 * 36	右前边	                   3
 * 37	右后角	                   3
 * 38	右后中	                   3
 * 39	左后中	                   3
 * 40	左后角	                   3
 * 41	左前中	                   5
 * 42	右前中	                   5
 * 43	左前中	                   6
 * 44	左前角	                   6
 * 45	左后边	                   6
 * 46	左后角	                   6
 * 47	左后中	                   6
 * 48	右后中	                   6
 * 49	右后角	                   6
 * 50	左后边	                   2
 * 51	左前角	                   2
 * 52	左前中	                   2
 * 53	右前中	                   2
 * 54	右前角	                   2
 * 55	右前边	                   2
 * 56	右后角	                   2
 * 57	右后中	                   2
 * 58	左后中	                   2
 * 59	左后角	                   2
 * 60	右前边	                   6
 * 61	右前角	                   6
 * 62	左后边	                   1
 * 63	左后角	                   1
 * 64	左后中	                   1
 * 65	右后中	                   1
 * 66	右后角	                   1
 * 67	右前边	                   1
 * 68	右前角	                   1
 * 69	右前中	                   1
 * 70	左前角	                   1
 * 71	左前中	                   1
 * 
 *************************************************************************************************/

#include "../tool/TimeHelper.hpp"

#include "RadarAlarmDisplay.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "CarModelAnimation3D.hpp"

namespace VirtualView3D {

#define VV_RADAR_ALARM_BLOCK_TRANS_X     (VV_CAR_MODEL_COORD_CENTER_OFFSET * 2.80f)
#define VV_RADAR_ALARM_BLOCK_TRANS_Y     1280.f

/** @brief YAW PITCH for OpengGL coordinate */
#define RADAR_ALARM_YAW_GL_COORD           0.f, 1.f, 0.f
#define RADAR_ALARM_PITCH_GL_COORD         0.f, 0.f, -1.f

// #define RADAR_ALARM_INDEX_TIPS

struct PdcMeshIndex
{
    std::uint8_t index;
    AreaType     type;
    LevelType    level;
};

static const PdcMeshIndex PDC_MESH_INDEX_ARRAY[] = {
/// index, type,         level    
    {0	, AreaType::RFM, LevelType::LEVEL_6}, // 右前中
    {1	, AreaType::LRS, LevelType::LEVEL_4}, // 左后边
    {2	, AreaType::LFS, LevelType::LEVEL_4}, // 左前边
    {3	, AreaType::LFC, LevelType::LEVEL_4}, // 左前角
    {4	, AreaType::LFM, LevelType::LEVEL_4}, // 左前中
    {5	, AreaType::RFM, LevelType::LEVEL_4}, // 右前中
    {6	, AreaType::RFC, LevelType::LEVEL_4}, // 右前角
    {7	, AreaType::RFS, LevelType::LEVEL_4}, // 右前边
    {8	, AreaType::RRS, LevelType::LEVEL_4}, // 右后边
    {9	, AreaType::RRC, LevelType::LEVEL_4}, // 右后角
    {10	, AreaType::RRM, LevelType::LEVEL_4}, // 右后中
    {11	, AreaType::LRM, LevelType::LEVEL_4}, // 左后中
    {12	, AreaType::LRC, LevelType::LEVEL_4}, // 左后角
    {13	, AreaType::RRS, LevelType::LEVEL_6}, // 右后边
    {14	, AreaType::RRS, LevelType::LEVEL_5}, // 右后边
    {15	, AreaType::RRS, LevelType::LEVEL_3}, // 右后边
    {16	, AreaType::RRS, LevelType::LEVEL_2}, // 右后边
    {17	, AreaType::RRS, LevelType::LEVEL_1}, // 右后边
    {18	, AreaType::LFS, LevelType::LEVEL_6}, // 左前边
    {19	, AreaType::LFS, LevelType::LEVEL_5}, // 左前边
    {20	, AreaType::LFS, LevelType::LEVEL_3}, // 左前边
    {21	, AreaType::LFS, LevelType::LEVEL_2}, // 左前边
    {22	, AreaType::LFS, LevelType::LEVEL_1}, // 左前边
    {23	, AreaType::RFC, LevelType::LEVEL_5}, // 右前角
    {24	, AreaType::RFS, LevelType::LEVEL_5}, // 右前边
    {25	, AreaType::RRC, LevelType::LEVEL_5}, // 右后角
    {26	, AreaType::RRM, LevelType::LEVEL_5}, // 右后中
    {27	, AreaType::LRM, LevelType::LEVEL_5}, // 左后中
    {28	, AreaType::LRC, LevelType::LEVEL_5}, // 左后角
    {29	, AreaType::LRS, LevelType::LEVEL_5}, // 左后边
    {30	, AreaType::LFC, LevelType::LEVEL_5}, // 左前角
    {31	, AreaType::LRS, LevelType::LEVEL_3}, // 左后边
    {32	, AreaType::LFC, LevelType::LEVEL_3}, // 左前角
    {33	, AreaType::LFM, LevelType::LEVEL_3}, // 左前中
    {34	, AreaType::RFM, LevelType::LEVEL_3}, // 右前中
    {35	, AreaType::RFC, LevelType::LEVEL_3}, // 右前角
    {36	, AreaType::RFS, LevelType::LEVEL_3}, // 右前边
    {37	, AreaType::RRC, LevelType::LEVEL_3}, // 右后角
    {38	, AreaType::RRM, LevelType::LEVEL_3}, // 右后中
    {39	, AreaType::LRM, LevelType::LEVEL_3}, // 左后中
    {40	, AreaType::LRC, LevelType::LEVEL_3}, // 左后角
    {41	, AreaType::LFM, LevelType::LEVEL_5}, // 左前中
    {42	, AreaType::RFM, LevelType::LEVEL_5}, // 右前中
    {43	, AreaType::LFM, LevelType::LEVEL_6}, // 左前中
    {44	, AreaType::LFC, LevelType::LEVEL_6}, // 左前角
    {45	, AreaType::LRS, LevelType::LEVEL_6}, // 左后边
    {46	, AreaType::LRC, LevelType::LEVEL_6}, // 左后角
    {47	, AreaType::LRM, LevelType::LEVEL_6}, // 左后中
    {48	, AreaType::RRM, LevelType::LEVEL_6}, // 右后中
    {49	, AreaType::RRC, LevelType::LEVEL_6}, // 右后角
    {50	, AreaType::LRS, LevelType::LEVEL_2}, // 左后边
    {51	, AreaType::LFC, LevelType::LEVEL_2}, // 左前角
    {52	, AreaType::LFM, LevelType::LEVEL_2}, // 左前中
    {53	, AreaType::RFM, LevelType::LEVEL_2}, // 右前中
    {54	, AreaType::RFC, LevelType::LEVEL_2}, // 右前角
    {55	, AreaType::RFS, LevelType::LEVEL_2}, // 右前边
    {56	, AreaType::RRC, LevelType::LEVEL_2}, // 右后角
    {57	, AreaType::RRM, LevelType::LEVEL_2}, // 右后中
    {58	, AreaType::LRM, LevelType::LEVEL_2}, // 左后中
    {59	, AreaType::LRC, LevelType::LEVEL_2}, // 左后角
    {60	, AreaType::RFS, LevelType::LEVEL_6}, // 右前边
    {61	, AreaType::RFC, LevelType::LEVEL_6}, // 右前角
    {62	, AreaType::LRS, LevelType::LEVEL_1}, // 左后边
    {63	, AreaType::LRC, LevelType::LEVEL_1}, // 左后角
    {64	, AreaType::LRM, LevelType::LEVEL_1}, // 左后中
    {65	, AreaType::RRM, LevelType::LEVEL_1}, // 右后中
    {66	, AreaType::RRC, LevelType::LEVEL_1}, // 右后角
    {67	, AreaType::RFS, LevelType::LEVEL_1}, // 右前边
    {68	, AreaType::RFC, LevelType::LEVEL_1}, // 右前角
    {69	, AreaType::RFM, LevelType::LEVEL_1}, // 右前中
    {70	, AreaType::LFC, LevelType::LEVEL_1}, // 左前角
    {71	, AreaType::LFM, LevelType::LEVEL_1}, // 左前中
};

static const size_t PDC_MESH_INDEX_ARRAY_SIZE = sizeof(PDC_MESH_INDEX_ARRAY) / sizeof(PdcMeshIndex);

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
std::array<uint32_t, 3> RadarAlarmDisplay::GetPdcMeshIndexArray(const std::vector<PdcInfo> &pdcInfo)
{
    std::array<uint32_t, 3> indexArray = {0, 0, 0};
    
    for (std::vector<PdcInfo>::const_iterator it = pdcInfo.begin(); it != pdcInfo.end(); it++)
    {   
        PdcMeshIndex PdcMeshIndex;
        bool isFound = false;
        for (size_t meshArrayIndex = 0; meshArrayIndex < PDC_MESH_INDEX_ARRAY_SIZE; meshArrayIndex++)
        {
            if ((it->type == PDC_MESH_INDEX_ARRAY[meshArrayIndex].type) && (it->level == PDC_MESH_INDEX_ARRAY[meshArrayIndex].level))
            {
                PdcMeshIndex = PDC_MESH_INDEX_ARRAY[meshArrayIndex];
                isFound = true;
                break;
            }
        }

        if (isFound)
        {
            if (PdcMeshIndex.index < 32)
            {
                indexArray[0] = indexArray[0] | (1 << PdcMeshIndex.index);
            }
            else if (PdcMeshIndex.index < 64)
            {
                indexArray[1] = indexArray[1] | (1 << PdcMeshIndex.index);
            }
            else
            {
                indexArray[2] = indexArray[2] | (1 << PdcMeshIndex.index);
            }
        }
    }

    return indexArray;
}

RadarAlarmDisplay::RadarAlarmDisplay()
{
}

RadarAlarmDisplay::~RadarAlarmDisplay()
{
    /** @brief delete VAO VBO ...*/
    for (std::vector<BaseLib::Mesh>::iterator iter = this->modelspair.first.meshes.begin(); iter != this->modelspair.first.meshes.end(); iter++) {
        glDeleteVertexArrays(1, &iter->VAO);
        glDeleteBuffers(1, &iter->VBO);
		glDeleteBuffers(1, &iter->EBO);
    }

    /** @brief delete texture */
    for (std::vector<Texture>::iterator iter = this->modelspair.first.texturesLoaded.begin(); iter != this->modelspair.first.texturesLoaded.end(); iter++) {
        glDeleteTextures(1, &iter->id);
    }
}

void RadarAlarmDisplay::Initialization()
{
    BaseLib::Model  model;
    Shader shader;

    /** init for car 3d model */
    long timeus = TimeHelper::GetTimestampMicros();
    // model.Initialization("resource/junlian/virtualview/models/RadarAlarm/Radar.obj");
    model.Initialization("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/models/RadarAlarm/Radar.obj");
    model.LoadModel();
    // shader.FromSourceFile("resource/junlian/virtualview/shader/transform.vs", "resource/junlian/virtualview/shader/transform.fs");
    shader.FromSourceFile("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.vs", "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.fs");
    this->modelspair = std::pair<Model, Shader>(model, shader);

    this->mainScaleFactor = this->modelspair.first.GetFrustumFitScale();
    this->mainScaleFactor = VV_CAMERA_DISTANCE_FACTOR / (mainScaleFactor + 1e-6); 
    this->mainScaleFactor = this->mainScaleFactor * 1.800f;

    LOGGER_I("RadarAlarmDisplay LoadModel duration:%ld(us)", TimeHelper::GetTimestampMicros() - timeus);
}


/*调用一次, 传入 vector类型pdcInfo 一帧所有 信息 */
void RadarAlarmDisplay::DoRender(const VisualAngleControl &visualAngleControl, std::vector<PdcInfo> &pdcInfo){

#ifdef RADAR_ALARM_INDEX_TIPS
    static int tipIndex           = 0;
    static bool isShow            = false;
    static long currentIndexCount = 0;

    /// 300 frames
    if (currentIndexCount >= 400)
    {
        tipIndex++;
        if (tipIndex == this->modelspair.first.meshsCenter.size())
        // if (tipIndex > 60)
        {
            tipIndex = 0;
        }
        
        currentIndexCount = 0;
    }

    currentIndexCount++;

    LOGGER_I("tipIndex=%d, currentIndexCount=%d, isShow=%d", tipIndex, currentIndexCount, isShow);

#endif

    glEnable(GL_DEPTH_TEST);
    // GLX_AA_BLEND_ENABLE();

    float scalexyz     = this->mainScaleFactor * visualAngleControl.GetScaleVisualAngle(); 
    float radiansPitch = visualAngleControl.GetPitchRotationVisualAngle();
    float radiansYaw   = visualAngleControl.GetCarModelLeftRightVisualAngle();

    this->modelspair.second.UseProgram();

    glm::mat4 pvMat4 = visualAngleControl.projectMat4 * visualAngleControl.viewMat4;

    std::array<uint32_t, 3> indexArray = GetPdcMeshIndexArray(pdcInfo);

    for (size_t i=0; i < this->modelspair.first.meshsCenter.size(); i++)
    {
         /**************************************************************************************************
         * @brief  mesh index bit check
         *         indexArray[0] indicates: index range [0,  31]
         *         indexArray[1] indicates: index range [32, 63]
         *         indexArray[2] indicates: index range [64, 71]
         * 
         *************************************************************************************************/
#ifndef RADAR_ALARM_INDEX_TIPS
        if (((indexArray[i/32] >> i) & 0x01) == 0x00)
        {
            continue;
        }
#endif //RADAR_ALARM_INDEX_TIPS
        glm::mat4 modelMat4 = glm::mat4(1.0f);
        
        modelMat4 = glm::scale(modelMat4, glm::vec3(scalexyz, scalexyz, scalexyz));

        modelMat4 = glm::rotate(modelMat4, radiansPitch, glm::vec3(RADAR_ALARM_PITCH_GL_COORD));
        modelMat4 = glm::rotate(modelMat4, radiansYaw, glm::vec3(RADAR_ALARM_YAW_GL_COORD));

        modelMat4 = glm::translate(modelMat4, glm::vec3(VV_RADAR_ALARM_BLOCK_TRANS_X,  VV_RADAR_ALARM_BLOCK_TRANS_Y, 0.f));
        modelMat4 = glm::rotate(modelMat4, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.0f));

        modelMat4 = glm::scale(modelMat4, glm::vec3(VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR));

        // glm::mat4 mvpMatrix = visualAngleControl.projectMat4 * visualAngleControl.viewMat4 * modelMat4;
        glm::mat4 mvpMatrix = pvMat4 * modelMat4;

        // draw
        // this->modelspair.second.SetMat4("u_Model", modelMat4);
        // this->modelspair.second.SetMat4("u_View", visualAngleControl.viewMat4);
        // this->modelspair.second.SetMat4("u_Projection", visualAngleControl.projectMat4);
        this->modelspair.second.SetMat4("u_MVP", mvpMatrix);
    
    #ifdef RADAR_ALARM_INDEX_TIPS
        if((tipIndex == i))
        {
            if (currentIndexCount % 2 == 0) isShow = !isShow;

            if (isShow) this->modelspair.first.DrawMesh(i, this->modelspair.second);
            
        }
        else
        {
            this->modelspair.first.DrawMesh(i, this->modelspair.second);
        }
    #else
  
        this->modelspair.first.DrawMesh(i, this->modelspair.second);

    #endif
            
    }

    glDisable(GL_DEPTH_TEST);
    // GLX_AA_BLEND_DISABLE();
}

};
