/*
 • To change this license header, choose License Headers in Project Properties.

 • To change this template file, choose Tools | Templates

 • and open the template in the editor.

 */

/*
 • File:   ParkingDisplay.cpp

 • Author: tron

 • Function: 自动泊车车位显示

 • Created on 2023年5月6日, 下午5:52

 */

#include <sys/stat.h>
#include <math.h>
#include <android/log.h>

#include "Include.hpp"
#include "../tool/TimeHelper.hpp"
#include "../tool/GLHelper.hpp"

#include "ParkingDisplay.hpp"
#include "Model.hpp"

namespace VirtualView3D {

#define VV_PARKING_VERTICES_SIZE      (80)       // 20 * sizeof(float)

    /** @brief vao index */
#define VV_VAO_IDX_PARK_BALOON    (0)
#define VV_VAO_IDX_PARK_NORMAL    (1)
#define VV_VAO_IDX_PARK_OBLIQUE   (2)
#define VV_VAO_IDX_PARK_LF_RR     (3)

    /** @brief left and right */
#define PARK_TEX_IMG_ID_NO_OFFSET       (-1)
#define PARK_NO_MIN                     (1)
#define PARK_NO_MAX                     (6)
#define PARK_TEX_IMG_ID_NO_1            (1+PARK_TEX_IMG_ID_NO_OFFSET)
#define PARK_TEX_IMG_ID_NO_6            (6+PARK_TEX_IMG_ID_NO_OFFSET)
#define PARK_TEX_IMG_ID_P               (7+PARK_TEX_IMG_ID_NO_OFFSET)
#define PARK_TEX_IMG_ID_P_NARROW        (8+PARK_TEX_IMG_ID_NO_OFFSET)
#define PARK_TEX_IMG_ID_NARROW          (9+PARK_TEX_IMG_ID_NO_OFFSET)

    /** @brief common*/
#define PARK_TEX_IMG_ID_UNAVAILABLE                (0)
#define PARK_TEX_IMG_ID_EMPTY                      (1)
#define PARK_TEX_IMG_ID_P_HORIZONTAL_LEFT          (2)
#define PARK_TEX_IMG_ID_BALLOON_P                  (3)
#define PARK_TEX_IMG_ID_SELECTED_NORMAL_LEFT       (4)
#define PARK_TEX_IMG_ID_SELECTED_NARROW_LEFT       (5)
#define PARK_TEX_IMG_ID_SELECTED_NORMAL_RIGHT      (6)
#define PARK_TEX_IMG_ID_SELECTED_NARROW_RIGHT      (7)


    /** @brief YAW PITCH for OpengGL coordinate */
#define PARK_YAW_GL_COORD           0.f, 1.f, 0.f
#define PARK_PITCH_GL_COORD         0.f, 0.f, -1.f


#define VV_PARKING_SLOT_Y             0.f
#define VV_PARKING_SLOT_WIDTH         2.5f
#define VV_PARKING_SLOT_LENGTH        5.3f
#define VV_PARKING_SLOT_WIDTH_2       (VV_PARKING_SLOT_WIDTH * 0.5f)
#define VV_PARKING_SLOT_LENGTH_2      (VV_PARKING_SLOT_LENGTH * 0.5f)
#define VV_PARK_SLOT_OFFSET_Z_OBLIQUE 0.5f

#define VV_PARK_OFFSET_X_VIRTUAL      1.5f
#define VV_PARK_TRANS_FACTOR          2.125f

    // #define VV_PARK_DEBUG
#ifdef VV_PARK_DEBUG
    static size_t debugCounter    = 0;
     static size_t debugCounterPre = 0;
     static bool isLog = false;
#endif // VV_PARK_DEBUG


    ParkingDisplay::ParkingDisplay():mainScaleFactor(1.0f),isRenderTouchable(true)
    {

    }

    ParkingDisplay::~ParkingDisplay()
    {
        glDeleteVertexArrays(VV_PARKING_EXTRA_VAO_SIZE, this->vaosParking);
        glDeleteBuffers(VV_PARKING_EXTRA_VAO_SIZE, this->vbosParking);

        for (auto & parkVao : this->parkSlotVaoVector)
        {
            glDeleteVertexArrays(1, &parkVao.vao);
            glDeleteBuffers(1, &parkVao.vbo);
        }

        glDeleteTextures(VV_PARKING_MARKERS_SIZE_COMMON, this->parkingTextIdsCommon);
        glDeleteTextures(VV_PARKING_MARKERS_SIZE_LEFT, this->parkingTextIdsLeft);
        glDeleteTextures(VV_PARKING_MARKERS_SIZE_RIGHT, this->parkingTextIdsRight);
    }

    /**
     • @brief Get parking slot center.

     • @details Slot's coordinates are Car-coordinate. return the OpenGL coordinate center.

     •

     • @param slot const Slot&

     • @return glm::vec3 return the OpenGL coordinate center.

    **/
    glm::vec3 ParkingDisplay::GetParkingSlotCenter(const Slot& slot)
    {
        glm::vec3 result(0.0f, 0.0f, 0.0f);

        for (int i = 0; i < 4; i++)
        {
            result.x += slot.parkingPoint[i].x;
            result.y += slot.parkingPoint[i].z;
            result.z -= slot.parkingPoint[i].y;
        }

        result = result / 4.0f;

        return result;
    }

    void ParkingDisplay::SetExtraRotation(float angle, const glm::vec3 &axis)
    {
        ExtraRotationMat4 = glm::rotate(glm::mat4(1.f), angle, axis);
    }

    /**
     • @brief Get parking slot direction yaw by radians.

     •

     • @param slot const Slot&

     • @return float

    **/
    float ParkingDisplay::GetParkingSlotDirYawRad(const Slot& slot, glm::vec2 &vecDirAB, glm::vec2 &vecDirAC, float &interRad, float &length, float &width)
    {
        float result = 0.f;

        vecDirAB = glm::vec2(slot.parkingPoint[1].x - slot.parkingPoint[0].x, slot.parkingPoint[1].y - slot.parkingPoint[0].y);
        vecDirAC = glm::vec2(slot.parkingPoint[2].x - slot.parkingPoint[0].x, slot.parkingPoint[2].y - slot.parkingPoint[0].y);

        glm::vec2 vecDir = -vecDirAC; // CA

        length = glm::length(vecDirAB);
        width  = glm::length(vecDirAC);

        if (length > width)
        {
            vecDir = vecDirAB;
        }
        else
        {
            float tmp = length;
            length    = width;
            width     = tmp;
        }

        float dotVal  = glm::dot(vecDirAC, vecDirAB);
        interRad      = acos(dotVal / ((length * width) + 1e-6));

        result = atan2(vecDir.y, vecDir.x);

        return result;
    }


    void ParkingDisplay::Initialization(const std::shared_ptr<YAML::Node> & cfgNodePtr)
    {
        this->configNodePtr = cfgNodePtr; // 保留，但不使用

        this->shapeRadTolerance = glm::radians(1.5f);
        this->lengthTolerance = 0.35f;
        this->widthTolerance = 0.35f;
        this->slotSizeScale = 1.06f;
        this->slotNumMax = 30;

        this->parkSlotVaoVector.reserve(this->slotNumMax);
        this->slotsCacheVector.reserve(this->slotNumMax);

        this->InitTextures();
        this->InitParking();

        // 使用外部存储路径
        std::string vsPath = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/parking.vs";
        std::string fsPath = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/parking.fs";
        std::string balloonVsPath = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/balloon.vs";
        std::string balloonFsPath = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/balloon.fs";

        struct stat buffer;
        bool vsExists = (stat(vsPath.c_str(), &buffer) == 0);
        bool fsExists = (stat(fsPath.c_str(), &buffer) == 0);
        bool balloonVsExists = (stat(balloonVsPath.c_str(), &buffer) == 0);
        bool balloonFsExists = (stat(balloonFsPath.c_str(), &buffer) == 0);

        this->parkingShader.FromSourceFile(vsPath.c_str(), fsPath.c_str());
        this->balloonShader.FromSourceFile(balloonVsPath.c_str(), balloonFsPath.c_str());

        this->mainScaleFactor = VV_CAMERA_DISTANCE_FACTOR * 0.1158f;
    }


    void ParkingDisplay::InitTextures(void)
    {
        long timeus = TimeHelper::GetTimestampMicros();
        std::string directory = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/image/parking2";
        std::string filename;
        int i;

        struct stat buffer;
        bool dirExists = (stat(directory.c_str(), &buffer) == 0);

        /** @brief common, start from 0 */
        for (i = 0; i < VV_PARKING_MARKERS_SIZE_COMMON; i++)
        {
            filename = std::to_string(i) + ".png";
            this->parkingTextIdsCommon[i] = BaseLib::Model::TextureFromFile(filename.c_str(), directory);
        }

        /** @brief left, start from 1 */
        std::string leftDir = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/image/parking2/left";
        for (i = 0; i < VV_PARKING_MARKERS_SIZE_LEFT; i++)
        {
            filename = std::to_string(i+1) + ".png";
            this->parkingTextIdsLeft[i] = Model::TextureFromFile(filename.c_str(), leftDir);
        }

        /** @brief right, start from 1 */
        std::string rightDir = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/image/parking2/right";
        for (i = 0; i < VV_PARKING_MARKERS_SIZE_RIGHT; i++)
        {
            filename = std::to_string(i+1) + ".png";
            this->parkingTextIdsRight[i] = Model::TextureFromFile(filename.c_str(), rightDir);
        }

        /** @brief middle, start from 1 */
        std::string middleDir = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/image/parking2/middle";
        for (i = 0; i < VV_PARKING_MARKERS_SIZE_MIDDLE; i++)
        {
            filename = std::to_string(i+1) + ".png";
            this->parkingTextIdsMiddle[i] = Model::TextureFromFile(filename.c_str(), middleDir);
        }

        LOGGER_I("ParkingDisplay InitTextures done, duration:%ld(us)", TimeHelper::GetTimestampMicros() - timeus);
    }

    void ParkingDisplay::GenParkingVao(float theta, ParkSlotVao &parkVao, bool isCacheVertices)
    {
        glm::vec3 dirCA_2 = (this->slotSizeScale * parkVao.length) * glm::vec3(sin(theta), 0, cos(theta));
        glm::vec3 aHat    = glm::vec3(0, 0.0,  this->slotSizeScale * parkVao.width);
        glm::vec3 bHat    = glm::vec3(0, 0.0, -aHat.z);

        glm::vec3 A = aHat + dirCA_2;
        glm::vec3 B = bHat + dirCA_2;
        glm::vec3 C = aHat - dirCA_2;
        glm::vec3 D = bHat - dirCA_2;

        float vertices[20] =
                {
                        A.x , A.y, A.z,  1.0f, 1.0f, // A
                        B.x , B.y, B.z,  1.0f, 0.0f, // B
                        D.x , D.y, D.z,  0.0f, 0.0f, // D
                        C.x , C.y, C.z,  0.0f, 1.0f, // C
                };

        BaseLib::GLHelper::GenRectangleVertexArrayBuffer(vertices, VV_PARKING_VERTICES_SIZE, parkVao.vao, parkVao.vbo);

        if (isCacheVertices)
        {
            std::vector<glm::vec3> verticesVec3;
            verticesVec3.reserve(4);
            verticesVec3.push_back(glm::vec3(vertices[0],  vertices[1],  vertices[2]));
            verticesVec3.push_back(glm::vec3(vertices[5],  vertices[6],  vertices[7]));
            verticesVec3.push_back(glm::vec3(vertices[10], vertices[11], vertices[12]));
            verticesVec3.push_back(glm::vec3(vertices[15], vertices[16], vertices[17]));

            this->verticesVec3CacheMap[parkVao.parkingID] = verticesVec3;
        }

    }

    void ParkingDisplay::InitParking(void)
    {
        /** @brief BALLOON */
        const float verticesBalloonP[] = {
                0.65f, 0.000f,  VV_PARKING_SLOT_Y, 1.0f, 1.0f,
                0.65f, 1.664f,  VV_PARKING_SLOT_Y, 1.0f, 0.0f,
                -0.65f, 1.664f,  VV_PARKING_SLOT_Y, 0.0f, 0.0f,
                -0.65f, 0.000f,  VV_PARKING_SLOT_Y, 0.0f, 1.0f,
        };

        BaseLib::GLHelper::GenRectangleVertexArrayBuffer(verticesBalloonP, VV_PARKING_VERTICES_SIZE, this->vaosParking[0], this->vbosParking[0]);

    }


    /*选定泊车车位ID */
    void ParkingDisplay::SelectedSlot(const int32_t id)
    {
        this->selectedSlotId = id;
    }

    int32_t ParkingDisplay::Touch(const TouchPoint &point){
        int32_t parkingID = 0;

        for (size_t i = 0; i < slotsCacheVector.size(); i++)
        {
            Slot slot = slotsCacheVector[i];

            if (slot.parkingType == ParkingType::NO_PARKING_ALLOWED)
            {
                continue;
            }

            glm::mat4 modelViewMatrix = this->slotsViewMat4 * this->slotsModelMat4Map[slot.parkingID];

            std::vector<glm::vec3> verticesVec3 = verticesVec3CacheMap[slot.parkingID];
            std::vector<glm::vec3> quardVec3Vector;
            quardVec3Vector.reserve(4);
            /**
             ▪ @brief A B D C

             ▪

            **/
            for (size_t j = 0; j < 4; j++)
            {
                glm::vec3 screenVec3 = glm::project(verticesVec3[j], modelViewMatrix, this->slotsProjectMat4, this->viewportVec4);

                screenVec3.y = this->viewportVec4[3] - screenVec3.y;

                quardVec3Vector.push_back(screenVec3);
            }

            glm::vec3 sAB = quardVec3Vector[1] - quardVec3Vector[0];
            glm::vec3 sBD = quardVec3Vector[2] - quardVec3Vector[1];
            glm::vec3 sDC = quardVec3Vector[3] - quardVec3Vector[2];
            glm::vec3 sCA = quardVec3Vector[0] - quardVec3Vector[3];

            glm::vec2 AP  = glm::vec2((point.pixelX - quardVec3Vector[0].x), (point.pixelY - quardVec3Vector[0].y));
            glm::vec2 BP  = glm::vec2((point.pixelX - quardVec3Vector[1].x), (point.pixelY - quardVec3Vector[1].y));
            glm::vec2 DP  = glm::vec2((point.pixelX - quardVec3Vector[2].x), (point.pixelY - quardVec3Vector[2].y));
            glm::vec2 CP  = glm::vec2((point.pixelX - quardVec3Vector[3].x), (point.pixelY - quardVec3Vector[3].y));

            float crossVal1 = sAB.x * AP.y - sAB.y * AP.x;
            float crossVal2 = sBD.x * BP.y - sBD.y * BP.x;
            float crossVal3 = sDC.x * DP.y - sDC.y * DP.x;
            float crossVal4 = sCA.x * CP.y - sCA.y * CP.x;

            bool isInside = (crossVal1 < 0.f) && (crossVal2 < 0.f) && (crossVal3 < 0.f) && (crossVal4 < 0.f);

            if (isInside) {
                parkingID = slot.parkingID;

#ifdef VV_PARK_DEBUG
                LOGGER_I("found parking slot inside, parkingID=%d", parkingID);
                 debugCounter++;
#endif

                break;
            }
        }

#ifdef VV_PARK_DEBUG
        if (parkingID > 0) {
             LOGGER_I("final selected parking slot, parkingID=%d", parkingID);
             debugCounter++;
         }
#endif

        return parkingID;
    }


    void ParkingDisplay::DoRender(const VisualAngleControl &visualAngleControl, const std::vector<Slot> &slots)
    {
        this->viewportVec4 =  visualAngleControl.GetViewportVec4();

        float radiansPitch =  visualAngleControl.GetPitchRotationVisualAngle();
        float radiansYaw   = -visualAngleControl.GetLeftRightVisualAngle();

        float scalexyz                 = this->mainScaleFactor * visualAngleControl.GetScaleVisualAngle();
        bool isDrawBalloon             = false;  ///< draw balloon

        /** @brief check size */
        if ((slots.size() == 0))
        {
            if (this->isRenderTouchable)
            {
                this->slotsCacheVector.clear();
            }

            return;
        }

        glDisable(GL_DEPTH_TEST);
        GLX_AA_BLEND_ENABLE();

        if (this->isRenderTouchable)
        {
            this->slotsViewMat4    = visualAngleControl.viewMat4;
            this->slotsProjectMat4 = visualAngleControl.projectMat4;
            this->slotsModelMat4Map.clear();
            this->slotsCacheVector.clear();
        }

        /**
         ◦ @brief VAO cache vector check

         ◦

        **/
        if (this->isRenderTouchable)
        {
            if (this->parkSlotVaoVector.size() > this->slotNumMax)
            {
                for (ParkSlotVao &parkVao : this->parkSlotVaoVector)
                {
                    glDeleteVertexArrays(1, &parkVao.vao);
                    glDeleteBuffers(1, &parkVao.vbo);
                }

                this->parkSlotVaoVector.clear();
                verticesVec3CacheMap.clear();
            }
        }


        size_t loopMax = std::min<size_t>(slots.size(), this->slotNumMax);

#ifdef VV_PARK_DEBUG
        isLog = false;
         if (debugCounterPre != debugCounter)
         {
             debugCounterPre = debugCounter;
             isLog = true;
         }
#endif // VV_PARK_DEBUG

        parentMatrix = glm::mat4(1.0f);
        parentMatrix = glm::scale(parentMatrix, glm::vec3(scalexyz, scalexyz, scalexyz));
        parentMatrix = glm::rotate(parentMatrix, radiansPitch, glm::vec3(PARK_PITCH_GL_COORD));
        parentMatrix = glm::rotate(parentMatrix, radiansYaw, glm::vec3(PARK_YAW_GL_COORD));
        parentMatrix *= ExtraRotationMat4;

        for (size_t i = 0; i < loopMax; i++)
        {
            Slot slot              = slots[i];
            GLuint vao             = GL_NONE;
            GLuint textureId       = this->parkingTextIdsCommon[PARK_TEX_IMG_ID_UNAVAILABLE]; // GL_NONE;

            float interRad = 0.f;
            float length   = 0.f;
            float width    = 0.f;
            glm::vec2 vecDirAB(0.0f, 0.0f);
            glm::vec2 vecDirAC(0.0f, 0.0f);
            float slotDir  = GetParkingSlotDirYawRad(slot, vecDirAB, vecDirAC, interRad, length, width);
            float cmpRad   = interRad;
            ParkSlotVao tmpParkVao;
            bool isFoundVao = false;

            /**
             ▪ @brief filter invalid parking slots

             ▪

            **/
            if (isnan(interRad) || fabs(interRad) < 1e-3 || length < 1e-3 || width < 1e-3)
            {
                continue;
            }

            float crossDir = vecDirAB.x * vecDirAC.y - vecDirAC.x * vecDirAB.y;

            /**
             ▪ @brief add to touch cache

             ▪

            **/
            if (this->isRenderTouchable)
            {
                this->slotsCacheVector.push_back(slot);
            }

            /**
             ▪ @brief rectification of abnormal slot type for rendering, ignore slotType of SLOT_X_RAPA

             ▪

            **/
            if ((crossDir > 0.f) && (interRad < (M_PI_2 - this-> shapeRadTolerance)) && (slot.slotType != SlotType::SLOT_LEFT_ANG_FORWARD) && (slot.slotType != SlotType::SLOT_LEFT_RAPA))
            {
                slot.slotType = SlotType::SLOT_LEFT_ANG_FORWARD;
            }

            if ((crossDir < 0.f) && (interRad < (M_PI_2 - this-> shapeRadTolerance)) && (slot.slotType != SlotType::SLOT_RIGHT_ANG_FORWARD) && (slot.slotType != SlotType::SLOT_RIGHT_RAPA))
            {
                slot.slotType = SlotType::SLOT_RIGHT_ANG_FORWARD;
            }

            if ((crossDir > 0.f) && (interRad > (M_PI_2 + this-> shapeRadTolerance)) && (slot.slotType != SlotType::SLOT_LEFT_ANG_REVERSE) && (slot.slotType != SlotType::SLOT_LEFT_RAPA))
            {
                slot.slotType = SlotType::SLOT_LEFT_ANG_REVERSE;
            }

            if ((crossDir < 0.f) && (interRad > (M_PI_2 + this-> shapeRadTolerance)) && (slot.slotType != SlotType::SLOT_RIGHT_ANG_REVERSE) && (slot.slotType != SlotType::SLOT_RIGHT_RAPA))
            {
                slot.slotType = SlotType::SLOT_RIGHT_ANG_REVERSE;
            }

            /**
             ▪ @brief correct interset rad

             ▪

            **/
            if ((slot.slotType == SlotType::SLOT_RIGHT_RAPA) ||
                (slot.slotType == SlotType::SLOT_RIGHT_VERT) ||
                (slot.slotType == SlotType::SLOT_RIGHT_ANG_FORWARD) ||
                (slot.slotType == SlotType::SLOT_RIGHT_ANG_REVERSE)
                    )
            {
                cmpRad = M_PI - cmpRad;
            }

            for (std::vector<ParkSlotVao>::iterator parkVaoIt = this->parkSlotVaoVector.begin(); parkVaoIt != this->parkSlotVaoVector.end(); parkVaoIt++)
            {
                ParkSlotVao parkVao = (*parkVaoIt);
                if (parkVao.parkingID == slot.parkingID)
                {
                    float dRad    = fabs(parkVao.rad - cmpRad);
                    float dWdith  = fabs(width - parkVao.width);
                    float dLength = fabs(length - parkVao.length);
                    if ((dRad    < this->shapeRadTolerance) &&
                        (dLength < this->lengthTolerance) &&
                        (dWdith  < this->widthTolerance))
                    {
                        tmpParkVao = parkVao;
                        isFoundVao = true;
                    }

#ifdef VV_PARK_DEBUG
                    if (isLog)
                 {
                     LOGGER_I("for slot.id=%d, comRadDeg=%f, current: parkVao.radDeg=%f, width=%f, length=%f, dRadReg=%f, dWidth=%f, dLength=%f, isFound=%d", slot.parkingID, glm::degrees(cmpRad), glm::degrees(parkVao.rad), parkVao.width, parkVao.length, glm::degrees(dRad), dWdith, dLength, isFoundVao);
                 }
#endif // VV_PARK_DEBUG

                    if (!isFoundVao)
                    {
                        glDeleteVertexArrays(1, &parkVao.vao);
                        glDeleteBuffers(1, &parkVao.vbo);
                        this->parkSlotVaoVector.erase(parkVaoIt);
                    }

                    break;
                }
            }

            if (!isFoundVao)
            {
                tmpParkVao.parkingID = slot.parkingID;
                tmpParkVao.rad       = cmpRad;
                tmpParkVao.length    = length;
                tmpParkVao.width     = width;

                bool isCacheVertices = (slot.slotType != SlotType::SLOT_VIRTUAL);

                this->GenParkingVao(cmpRad, tmpParkVao, isCacheVertices);
                this->parkSlotVaoVector.emplace_back(tmpParkVao);
            }

            vao = tmpParkVao.vao;

            if (vao == GL_NONE)
            {
                LOGGER_I("VAO invalid for parking slot id=%d, continue", slot.parkingID);
                continue;
            }

#ifdef VV_PARK_DEBUG
            if (isLog)
         {
             LOGGER_I("slot id=%d, slotType=%d, slotDir=%f, slotDirDeg=%f, interDeg=%f, cmpDeg=%f, width=%f, length=%f, parkAngle=%f, slotSize=%u, vaoSize=%u, isFoundVao=%d", slot.parkingID, slot.slotType, slotDir, glm::degrees(slotDir), glm::degrees(interRad), glm::degrees(cmpRad), width, length, glm::degrees(tmpParkVao.rad), slots.size(), this->parkSlotVaoVector.size(), isFoundVao);
         }
#endif // VV_PARK_DEBUG

            switch (slot.slotType)
            {
                case SlotType::SLOT_UNKNOW_TYPE:
                    break;
                case SlotType::SLOT_LEFT_RAPA:
                    slotDir += (M_PI_2 - interRad);
                    break;
                case SlotType::SLOT_RIGHT_RAPA:
                    slotDir -= (M_PI_2 - interRad);
                    break;
                case SlotType::SLOT_LEFT_VERT:
                    slotDir += (M_PI_2 - interRad);
                    break;
                case SlotType::SLOT_RIGHT_VERT:
                    slotDir -= (M_PI_2 - interRad);
                    break;
                case SlotType::SLOT_LEFT_ANG_FORWARD: // 5
                    slotDir += (M_PI_2 - interRad);
                    slotDir += M_PI;
                    break;
                case SlotType::SLOT_LEFT_ANG_REVERSE: // 6
                    slotDir += (M_PI_2 - interRad);
                    slotDir += M_PI;
                    break;
                case SlotType::SLOT_RIGHT_ANG_FORWARD: // 7
                    slotDir -= (M_PI_2 - interRad);
                    break;
                case SlotType::SLOT_RIGHT_ANG_REVERSE: // 8
                    slotDir -= (M_PI_2 - interRad);
                    break;
                case SlotType::SLOT_VIRTUAL:
                    break;
                default:
                    break;
            }

            textureId = this->parkingTextIdsCommon[PARK_TEX_IMG_ID_UNAVAILABLE];

            if (slot.slotType == SlotType::SLOT_VIRTUAL)
            {
                textureId = this->parkingTextIdsCommon[PARK_TEX_IMG_ID_SELECTED_NORMAL_LEFT];
            }
            else if (slot.parkingID == this->selectedSlotId)
            {
                /**
                 ▪ @brief SELECTED

                 *
                 **/

                if ((slot.slotType == SlotType::SLOT_LEFT_VERT) || (slot.slotType == SlotType::SLOT_LEFT_ANG_FORWARD) || (slot.slotType == SlotType::SLOT_LEFT_ANG_REVERSE))
                {
                    /** @brief left */

                    textureId = this->parkingTextIdsCommon[PARK_TEX_IMG_ID_SELECTED_NORMAL_LEFT];
                    if (slot.parkingSpaceType == ParkingSpaceType::NARROW_SPACE) {
                        textureId = this->parkingTextIdsCommon[PARK_TEX_IMG_ID_SELECTED_NARROW_LEFT];
                    }
                }
                else if ((slot.slotType == SlotType::SLOT_RIGHT_VERT) || (slot.slotType == SlotType::SLOT_RIGHT_ANG_FORWARD) || (slot.slotType == SlotType::SLOT_RIGHT_ANG_REVERSE))
                {
                    /** @brief right */

                    textureId = this->parkingTextIdsCommon[PARK_TEX_IMG_ID_SELECTED_NORMAL_RIGHT];
                    if (slot.parkingSpaceType == ParkingSpaceType::NARROW_SPACE) {
                        textureId = this->parkingTextIdsCommon[PARK_TEX_IMG_ID_SELECTED_NARROW_RIGHT];
                    }

                    // textureId = this->parkingTextIdsCommon[PARK_TEX_IMG_ID_SELECTED_NORMAL_LEFT];
                    // if (slot.parkingSpaceType == ParkingSpaceType::NARROW_SPACE) {
                    //     textureId = this->parkingTextIdsCommon[PARK_TEX_IMG_ID_SELECTED_NARROW_LEFT];
                    // }
                }
                else
                {
                    /** @brief no narrow for SlotType::SLOT_UNKNOW_TYPE SlotType::SLOT_LEFT_RAPA SlotType::SLOT_RIGHT_RAPA */
                    textureId = this->parkingTextIdsCommon[PARK_TEX_IMG_ID_EMPTY];
                }
            }
            else if ((slot.slotType != SlotType::SLOT_VIRTUAL) && (slot.parkingType == ParkingType::PARKING_IS_POSSIBLE))
            {
                /**
                 ▪ @brief UNSELECTED

                 ▪

                 **/

                if ((slot.slotType == SlotType::SLOT_LEFT_VERT) || (slot.slotType == SlotType::SLOT_LEFT_ANG_FORWARD) || (slot.slotType == SlotType::SLOT_LEFT_ANG_REVERSE))
                {
                    /** @brief left */

                    if (slot.parkingSpaceType == ParkingSpaceType::NARROW_SPACE)
                    {
                        /**
                         ▪ @brief PARK_TEX_IMG_ID_P_NARROW or PARK_TEX_IMG_ID_NARROW

                         ▪ @note number or balloon check out here.

                         ▪

                         **/
                        textureId = this->parkingTextIdsLeft[PARK_TEX_IMG_ID_P_NARROW];
                        if (slot.parkingSpaceShowContents == ParkingSpaceShowContents::SHOW_NONE)
                        {
                            textureId = this->parkingTextIdsLeft[PARK_TEX_IMG_ID_NARROW];
                        }
                    }
                    else
                    {
                        /**
                         ▪ @brief PARK_TEX_IMG_ID_P or PARK_TEX_IMG_ID_EMPTY

                         ▪ @note number or balloon check out here.

                         **/

                        textureId = this->parkingTextIdsLeft[PARK_TEX_IMG_ID_P];
                        if (slot.parkingSpaceShowContents == ParkingSpaceShowContents::SHOW_NONE)
                        {
                            textureId = this->parkingTextIdsCommon[PARK_TEX_IMG_ID_EMPTY];
                        }
                    }

                    /**
                     ▪ @brief SHOW NUMBER

                     ▪ @note when the narrow parking slots need to show number, it uses the same number textures the

                     ▪ normal ones used.

                     ▪

                     **/
                    if (slot.parkingSpaceShowContents == ParkingSpaceShowContents::SHOW_NUMBERS)
                    {
                        if (slot.displayNum >= PARK_NO_MIN && slot.displayNum <= PARK_NO_MAX)
                        {
                            textureId = this->parkingTextIdsLeft[slot.displayNum + PARK_TEX_IMG_ID_NO_OFFSET];
                        }
                        // else
                        // {
                        //     textureId = this->parkingTextIdsLeft[PARK_TEX_IMG_ID_P];
                        // }
                    }

                }
                else if ((slot.slotType == SlotType::SLOT_RIGHT_VERT) || (slot.slotType == SlotType::SLOT_RIGHT_ANG_FORWARD) || (slot.slotType == SlotType::SLOT_RIGHT_ANG_REVERSE))
                {
                    /** @brief right */

                    if (slot.parkingSpaceType == ParkingSpaceType::NARROW_SPACE)
                    {
                        /**
                         ▪ @brief PARK_TEX_IMG_ID_P_NARROW or PARK_TEX_IMG_ID_NARROW

                         ▪ @note number or balloon check out here.

                         ▪

                         **/
                        textureId = this->parkingTextIdsRight[PARK_TEX_IMG_ID_P_NARROW];
                        if (slot.parkingSpaceShowContents == ParkingSpaceShowContents::SHOW_NONE)
                        {
                            textureId = this->parkingTextIdsRight[PARK_TEX_IMG_ID_NARROW];
                        }
                    }
                    else
                    {
                        /**
                         ▪ @brief PARK_TEX_IMG_ID_P or PARK_TEX_IMG_ID_EMPTY

                         ▪ @note number or balloon check out here.

                         **/

                        textureId = this->parkingTextIdsRight[PARK_TEX_IMG_ID_P];
                        if (slot.parkingSpaceShowContents == ParkingSpaceShowContents::SHOW_NONE)
                        {
                            textureId = this->parkingTextIdsCommon[PARK_TEX_IMG_ID_EMPTY];
                        }
                    }

                    /**
                     ▪ @brief SHOW NUMBER

                     ▪ @note when the narrow parking slots need to show number, it uses the same number textures the

                     ▪ normal ones used.

                     ▪

                     **/
                    if (slot.parkingSpaceShowContents == ParkingSpaceShowContents::SHOW_NUMBERS)
                    {
                        if (slot.displayNum >= PARK_NO_MIN && slot.displayNum <= PARK_NO_MAX)
                        {
                            textureId = this->parkingTextIdsRight[slot.displayNum + PARK_TEX_IMG_ID_NO_OFFSET];
                        }
                        // else
                        // {
                        //     textureId = this->parkingTextIdsRight[PARK_TEX_IMG_ID_P];
                        // }
                    }
                }
                else
                {
                    /**
                     ▪ @brief SlotType::SLOT_UNKNOW_TYPE SlotType::SLOT_LEFT_RAPA SlotType::SLOT_RIGHT_RAPA

                     ▪        PARK_TEX_IMG_ID_P or PARK_TEX_IMG_ID_EMPTY

                     ▪ @note number or balloon check out here.

                     **/

                    textureId = this->parkingTextIdsMiddle[PARK_TEX_IMG_ID_P];
                    if (slot.parkingSpaceShowContents == ParkingSpaceShowContents::SHOW_NONE)
                    {
                        textureId = this->parkingTextIdsCommon[PARK_TEX_IMG_ID_EMPTY];
                    }

                    /**
                     ▪ @brief SHOW NUMBER

                     ▪ @note when the narrow parking slots need to show number, it uses the same number textures the

                     ▪ normal ones used.

                     ▪

                     **/
                    if (slot.parkingSpaceShowContents == ParkingSpaceShowContents::SHOW_NUMBERS)
                    {
                        if (slot.displayNum >= PARK_NO_MIN && slot.displayNum <= PARK_NO_MAX)
                        {
                            textureId = this->parkingTextIdsMiddle[slot.displayNum + PARK_TEX_IMG_ID_NO_OFFSET];
                        }
                    }
                }
            }

            glActiveTexture(GL_TEXTURE0);

            glBindTexture(GL_TEXTURE_2D, textureId);

            this->parkingShader.UseProgram();
            this->parkingShader.SetInt("textureDiffuse1", 0);

            glm::vec3 transvec3 = ParkingDisplay::GetParkingSlotCenter(slot);

            transvec3.x = transvec3.x * VV_PARK_TRANS_FACTOR;
            transvec3.z = transvec3.z * VV_PARK_TRANS_FACTOR;
            transvec3.y = 0.f;

            glm::mat4 parkingMat4 = glm::mat4(1.0f);
            parkingMat4 = glm::scale(parkingMat4, glm::vec3(scalexyz, scalexyz, scalexyz));

            parkingMat4 = glm::rotate(parkingMat4, radiansPitch, glm::vec3(PARK_PITCH_GL_COORD)); // 0.f, 0.f, 1.f
            parkingMat4 = glm::rotate(parkingMat4, radiansYaw, glm::vec3(PARK_YAW_GL_COORD));

            parkingMat4 = glm::translate(parkingMat4, transvec3);
            parkingMat4 = glm::rotate(parkingMat4, float(slotDir), glm::vec3(0.0f, 1.0f, 0.0f));

            glm::mat4 localMatrix = glm::mat4(1.0f);
            localMatrix = glm::translate(localMatrix, transvec3);  // 车位位置平移
            localMatrix = glm::rotate(localMatrix, float(slotDir), glm::vec3(0.0f, 1.0f, 0.0f));  // 车位自身方向

            /**
             ▪ @brief Textures direction correction

             ▪

             **/
            switch (slot.slotType)
            {
                case SlotType::SLOT_UNKNOW_TYPE:
                    break;
                case SlotType::SLOT_LEFT_RAPA:
                    break;
                case SlotType::SLOT_RIGHT_RAPA:
                    parkingMat4 = glm::rotate(parkingMat4, float(M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
                    break;
                case SlotType::SLOT_LEFT_VERT:
                    break;
                case SlotType::SLOT_RIGHT_VERT:
                    parkingMat4 = glm::rotate(parkingMat4, float(M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
                    break;
                case SlotType::SLOT_LEFT_ANG_FORWARD:
                    parkingMat4 = glm::rotate(parkingMat4, float(M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
                    break;
                case SlotType::SLOT_LEFT_ANG_REVERSE:
                    parkingMat4 = glm::rotate(parkingMat4, float(M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
                    break;
                case SlotType::SLOT_RIGHT_ANG_FORWARD:
                    parkingMat4 = glm::rotate(parkingMat4, float(M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
                    break;
                case SlotType::SLOT_RIGHT_ANG_REVERSE:
                    parkingMat4 = glm::rotate(parkingMat4, float(M_PI), glm::vec3(0.0f, 1.0f, 0.0f));
                    break;
                case SlotType::SLOT_VIRTUAL:
                    break;
                default:
                    break;
            }

            parkingMat4 = glm::scale(parkingMat4, glm::vec3(VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR));
            localMatrix = glm::scale(localMatrix, glm::vec3(VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR));

            parkingMat4 = parentMatrix * localMatrix;  // 关键修改：矩阵层级

            if (this->isRenderTouchable)
            {
                this->slotsModelMat4Map[slot.parkingID] = parkingMat4;
            }

            this->parkingShader.SetMat4("u_Model", parkingMat4);
            this->parkingShader.SetMat4("u_View", visualAngleControl.viewMat4);
            this->parkingShader.SetMat4("u_Projection", visualAngleControl.projectMat4);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        for (size_t i = 0; i < loopMax; i++)
        {
            Slot slot              = slots[i];
            isDrawBalloon          = false;

            if (slot.parkingID == this->selectedSlotId)
            {

                if (slot.parkingSpaceShowContents != ParkingSpaceShowContents::SHOW_NONE)
                {
                    isDrawBalloon = true;
                }
            }
            else if ((slot.slotType != SlotType::SLOT_VIRTUAL) && (slot.parkingType == ParkingType::PARKING_IS_POSSIBLE))
            {
                /**
                 ▪ @brief BALLOON P

                 ▪

                **/
                if (slot.parkingSpaceShowContents == ParkingSpaceShowContents::SHOW_BALLOON_P)
                {
                    isDrawBalloon = true;
                }
            }
            else
            {
                /// pass
            }

            /**
             ▪ @brief SHOW BALLOON P

            **/
            if (isDrawBalloon)
            {
                glm::mat4 parkingMat4 = this->slotsModelMat4Map[slot.parkingID];

                // 在车位矩阵基础上添加气球的本地变换
                glm::mat4 balloonMat4 = glm::translate(parkingMat4, glm::vec3(0.0f, 0.f, 0.0f)); // Y轴偏移（高度）
                balloonMat4 = glm::rotate(balloonMat4, float(M_PI), glm::vec3(0.0f, 1.0f, 0.0f));   // 调整朝向
                balloonMat4 = glm::scale(balloonMat4, glm::vec3(2.0f, 2.0f, 2.0f));                 // 缩放

                // 绑定纹理和着色器...
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, this->parkingTextIdsCommon[PARK_TEX_IMG_ID_BALLOON_P]);

                this->balloonShader.UseProgram();
                this->balloonShader.SetInt("textureDiffuse1", 0);

                // 传递矩阵到着色器
                this->balloonShader.SetMat4("u_Model", balloonMat4);
                this->balloonShader.SetMat4("u_View", visualAngleControl.viewMat4);
                this->balloonShader.SetMat4("u_Projection", visualAngleControl.projectMat4);

                // 绘制...
                glBindVertexArray(this->vaosParking[VV_VAO_IDX_PARK_BALOON]);
                glEnable(GL_DEPTH_TEST);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                glBindVertexArray(0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glDisable(GL_DEPTH_TEST);

            }
        }

        GLX_AA_BLEND_DISABLE();
        // glDisable(GL_DEPTH_TEST);
    }

    /***
     • @brief render virtual parking slot

     •

     • @param visualAngleControl VisualAngleControl  common visual angle control

     • @param carPosition        glm::dvec3          car position, double value for (x,y,z)

     • @param isStatic           bool                whether static or moving

     ***/
    void ParkingDisplay::DoRenderVirtual(const VisualAngleControl &visualAngleControl, const glm::dvec3 &carPosition, bool isStatic)
    {
        std::vector<Slot> slots;
        slots.reserve(1);
        Slot parkSlot;

        if (isStatic)
        {
            carPositionBase    = glm::dvec4(carPosition, visualAngleControl.GetLeftRightVisualAngle());
        }

        glm::dvec3 parkCenter = glm::dvec3(carPositionBase) - carPosition;

        /**
         ◦ @brief VIRTUAL PARKING SLOT

        **/

        parkSlot.parkingID                = 0;
        parkSlot.parkingSpaceShowContents = ParkingSpaceShowContents::SHOW_NONE;
        parkSlot.displayNum               = 0;
        parkSlot.parkingType              = ParkingType::PARKING_IS_POSSIBLE;
        parkSlot.parkingSpaceType         = ParkingSpaceType::NORMAL_SPACE;
        parkSlot.slotType                 = SlotType::SLOT_VIRTUAL;

        glm::vec4 rec[4] =
                {
                        glm::vec4(+VV_PARKING_SLOT_LENGTH_2, -VV_PARKING_SLOT_WIDTH_2, 0.f, 0.f), // A
                        glm::vec4(+VV_PARKING_SLOT_LENGTH_2, +VV_PARKING_SLOT_WIDTH_2, 0.f, 0.f), // B
                        glm::vec4(-VV_PARKING_SLOT_LENGTH_2, -VV_PARKING_SLOT_WIDTH_2, 0.f, 0.f), // C
                        glm::vec4(-VV_PARKING_SLOT_LENGTH_2, +VV_PARKING_SLOT_WIDTH_2, 0.f, 0.f), // D
                };

        for (int i = 0; i < 4; i++)
        {
            glm::mat4 mat4Vertex = glm::mat4(1.0f);
            glm::vec4 vertex     = glm::vec4(rec[i].x, rec[i].y, rec[i].z, 1.0f);
            mat4Vertex           = glm::rotate(mat4Vertex, static_cast<float>(carPositionBase.w), glm::vec3(0.f, 0.f, 1.f));
            mat4Vertex           = glm::translate(mat4Vertex, glm::vec3(VV_PARK_OFFSET_X_VIRTUAL, 0.f, 0.f));

            vertex               = mat4Vertex * vertex;

            rec[i].x = vertex.x;
            rec[i].y = vertex.y;
            rec[i].z = vertex.z;

            parkSlot.parkingPoint[i] = {.x=parkCenter.x + rec[i].x, .y=parkCenter.y + rec[i].y, .z=parkCenter.z + rec[i].z};
        }

        slots.push_back(parkSlot);

        this->isRenderTouchable = false;
//     this->DoRender(visualAngleControl, slots);
        this->isRenderTouchable = true;
    }

};