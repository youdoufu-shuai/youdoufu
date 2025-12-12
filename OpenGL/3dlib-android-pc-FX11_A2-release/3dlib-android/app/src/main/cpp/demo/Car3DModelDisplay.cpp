/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Car3DModelDisplay.cpp
 * Author: tron
 * Function: 四门两盖3D动效
 * Created on 2023年5月6日, 下午5:42
 */

#include <sys/stat.h>
#include <android/log.h>

#include "../tool/TimeHelper.hpp"

#include "Car3DModelDisplay.hpp"
#include "CarModelAnimation3D.hpp"
#include "glm/gtc/type_ptr.inl"


namespace VirtualView3D {

    /** @brief YAW PITCH for OpengGL coordinate */
#define CAR_MODEL_YAW_GL_COORD           0.f, 1.f, 0.f
#define CAR_MODEL_PITCH_GL_COORD         0.f, 0.f, -1.f


#define VIRTUAL_VIEW_DOOR_OPEN      1
#define VIRTUAL_VIEW_COVER_OPEN     1
#define VIRTUAL_VIEW_DOOR_CLOSE     2


    Car3DModelDisplay::Car3DModelDisplay():mainScaleFactor(1.0f), extraRotationMatrix(1.0f)
    {
    }

    void Car3DModelDisplay::SetExtraRotation(float angle, const glm::vec3& axis)
    {
        extraRotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);
    }

    void Car3DModelDisplay::ResetExtraRotation()
    {
        extraRotationMatrix = glm::mat4(1.0f);
    }

    Car3DModelDisplay::~Car3DModelDisplay()
    {
        /** @brief delete VAO VBO ... */
        for (std::vector<Mesh>::iterator iter = this->modelspair.first.meshes.begin(); iter != this->modelspair.first.meshes.end(); iter++) {
            glDeleteVertexArrays(1, &iter->VAO);
            glDeleteBuffers(1, &iter->VBO);
            glDeleteBuffers(1, &iter->EBO);

            LOGGER_GLE("Car3DModelDisplay::~Car3DModelDisplay glDeleteBuffers done");
        }

        /** @brief delete textures */
        for (std::vector<Texture>::iterator iter = this->modelspair.first.texturesLoaded.begin(); iter != this->modelspair.first.texturesLoaded.end(); iter++) {
            glDeleteTextures(1, &iter->id);

            LOGGER_GLE("Car3DModelDisplay::~Car3DModelDisplay glDeleteTextures done");
        }

    }

    void Car3DModelDisplay::Initialization(const std::shared_ptr<YAML::Node> & cfgNodePtr, VisualAngleControl &visualAngleControl)
    {
        this->configNodePtr = cfgNodePtr;

        Model  model;
        Shader shader;

        /** init for car 3d model */
        long timeus = TimeHelper::GetTimestampMicros();

        // 硬编码模型文件路径，不再从YAML配置中读取
        std::string modelFilename = "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/models/Geely/Geely1.obj";

        LOGGER_I("virtual view model file:%s", modelFilename.c_str());

        model.Initialization(modelFilename);

        model.LoadModel();
        shader.FromSourceFile("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transformHp.vs",
                              "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transformHp.fs");
        this->modelspair = std::pair<Model, Shader>(model, shader);

        LOGGER_I("Car3DModel model info:");
        LOGGER_I("model texture size=%lu", this->modelspair.first.texturesLoaded.size());
        LOGGER_I("model mesh    size=%lu", this->modelspair.first.meshes.size());
        // LOGGER_I("model box center aiVector3D(%f,%f,%f)", this->modelspair.first.sceneCenter.x, this->modelspair.first.sceneCenter.y, this->modelspair.first.sceneCenter.z);

        this->doorAnimationMaxValue = 90.0f;
        this->doorAnimationDelta = 5.0f;

        this->hoodAnimationMaxValue = 45.0f;
        this->hoodAnimationDelta = 3.0f;

        this->trunkAnimationMaxValue = 60.0f;
        this->trunkAnimationDelta = 4.0f;

        this->wheelAnimationDelta = 2.0f;

        /** @brief set car model scale factor */
        mainScaleFactor = this->modelspair.first.GetFrustumFitScale();
        mainScaleFactor = (VV_CAMERA_DISTANCE_FACTOR * 1.08f) / (mainScaleFactor + 1e-6);

        LOGGER_I("Car3DModel LoadModel duration:%ld(us)", TimeHelper::GetTimestampMicros() - timeus);
    }


    /*设置四门两盖 轮子按照速度显示 开启关闭过程动效*/
    void Car3DModelDisplay::DoRender(const VisualAngleControl &visualAngleControl, const CarDoorType &doorType, const CarMotionStatu &carMotionStatu)
    {
        static float  MODEL_YMIN = this->modelspair.first.sceneMin.y;

        glEnable(GL_DEPTH_TEST);
        // GLX_AA_BLEND_ENABLE();
        // glEnable(GL_CULL_FACE);

        float scalexyz = mainScaleFactor * visualAngleControl.GetScaleVisualAngle();

        /**************************************************************************************************
         * @brief parking completed mode, trigger the pitch
         *
        **************************************************************************************************/
        if (visualAngleControl.IsParkingCompletedMode())
        {
            const_cast<VisualAngleControl &>(visualAngleControl).TriggerParkingCompletedPerspectivePitch();
        }

        float radiansPitch = visualAngleControl.GetPitchRotationVisualAngle();
        float radiansYaw   = visualAngleControl.GetCarModelLeftRightVisualAngle();

        this->modelspair.second.UseProgram();

        glm::mat4 pvMat4 = visualAngleControl.projectMat4 * visualAngleControl.viewMat4;

        for (size_t i=0; i < this->modelspair.first.meshsCenter.size(); i++)
        {
            glm::vec3 centerWheeli = this->modelspair.first.meshsCenter[i];
            glm::mat4 egoCarWheelMat4 = glm::mat4(1.0f);
            egoCarWheelMat4 = glm::scale(egoCarWheelMat4, glm::vec3(scalexyz, scalexyz, scalexyz));

            egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, radiansPitch, glm::vec3(CAR_MODEL_PITCH_GL_COORD));
            egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, radiansYaw, glm::vec3(CAR_MODEL_YAW_GL_COORD));

            // 应用额外的旋转矩阵
            egoCarWheelMat4 = egoCarWheelMat4 * extraRotationMatrix;

            egoCarWheelMat4 = glm::translate(egoCarWheelMat4, glm::vec3(VV_CAR_MODEL_COORD_CENTER_OFFSET, -MODEL_YMIN, 0.f));
            egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.0f));  ///< coord rotation.


            /**************************************************************************************************
             * @brief WHEEL
             *
            **************************************************************************************************/
            if( i >= 40 && i <= 43 && (CarMotionStatu::STOP != carMotionStatu)){
                if (CarMotionStatu::FORWARD == carMotionStatu){
                    this->rotationAngle[6] += this->wheelAnimationDelta;

                    if (this->rotationAngle[6] > 360.f) this->rotationAngle[6] = this->rotationAngle[6] - 360.f;

                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[6]), glm::vec3(1.f, 0.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                } else {
                    this->rotationAngle[6] -= this->wheelAnimationDelta;
                    if (this->rotationAngle[6] < -360.f) this->rotationAngle[6] = this->rotationAngle[6] + 360.f;

                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[6]), glm::vec3(1.f, 0.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                }
            }

            /**************************************************************************************************
             * @brief FRONT LEFT DOOR
             *        doorDrvrSts 左前车门: 0 车门状态未知; 1 车门打开; 2 车门关闭
             *
            **************************************************************************************************/
            if( i >= 31 && i <= 34 && (doorType.doorDrvrSts > 0) ){
                centerWheeli.x = 583.687;
                centerWheeli.z = 580.351;
                centerWheeli.y = -629.6;

                int frontLeftDoor = 0;
                if(doorType.doorDrvrSts == 1)frontLeftDoor = 1;
                else frontLeftDoor = -1;

                this->rotationAngle[10] += frontLeftDoor * this->doorAnimationDelta;
                if((this->rotationAngle[10] >= this->doorAnimationMaxValue) && (frontLeftDoor==1)){
                    this->rotationAngle[10] = this->doorAnimationMaxValue;
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[10]), glm::vec3(0.f, -1.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                }
                else if(this->rotationAngle[10] <=0 && frontLeftDoor==-1){
                    this->rotationAngle[10]=0.f;
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[10]), glm::vec3(0.f, -1.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                }
                else{
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[10]), glm::vec3(0.f, -1.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                }
            }

            /**************************************************************************************************
             * @brief FRONT RIGHT DOOR
             * 		 doorPassSts  右前车门: 0 车门状态未知; 1 车门打开; 2 车门关闭
            **************************************************************************************************/
            if( i >= 27 && i <= 30 && doorType.doorPassSts > 0){
                centerWheeli.x = -583.689;
                centerWheeli.z = 580.351;
                centerWheeli.y = -629.6;

                int frontRightDoor = 0;
                if (doorType.doorPassSts == 1) frontRightDoor = 1;
                else frontRightDoor = -1;

                this->rotationAngle[16] += frontRightDoor * this->doorAnimationDelta;
                if((this->rotationAngle[16] >= this->doorAnimationMaxValue) && (frontRightDoor==1)){
                    this->rotationAngle[16] = this->doorAnimationMaxValue;
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[16]), glm::vec3(0.f, 1.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                }
                else if(this->rotationAngle[16]<=0&& frontRightDoor==-1){
                    this->rotationAngle[16]=0.f;
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[16]), glm::vec3(0.f, 1.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                }
                else{
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[16]), glm::vec3(0.f, 1.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                }
            }

            /**************************************************************************************************
             * @brief REAR LEFT DOOR
             *        doorLeReSts 左后车门: 0 车门状态未知; 1 车门打开; 2 车门关闭
            **************************************************************************************************/
            if( ((i >= 37 && i <= 38)||i==49) && doorType.doorLeReSts > 0){
                centerWheeli.x = 575.398;
                centerWheeli.z = -74.987;
                centerWheeli.y = 616.324;

                int backLeftDoor = 0;
                if (doorType.doorLeReSts == 1) backLeftDoor = 1;
                else backLeftDoor = -1;

                this->rotationAngle[22] += backLeftDoor * this->doorAnimationDelta;
                if((this->rotationAngle[22] >= this->doorAnimationMaxValue) && (backLeftDoor == 1)){
                    this->rotationAngle[22] = this->doorAnimationMaxValue;
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[22]), glm::vec3(0.f, -1.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                }
                else if(this->rotationAngle[22]<=0 && backLeftDoor==-1){
                    this->rotationAngle[22]=0.f;
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[22]), glm::vec3(0.f, -1.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                }
                else{
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[22]), glm::vec3(0.f, -1.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                }
            }

            /**************************************************************************************************
             * @brief REAR RIGHT DOOR
             *        doorRiReSts 右后车门：0 车门状态未知; 1 车门打开; 2 车门关闭
            **************************************************************************************************/
            if( ((i >= 35 && i <= 36)||i==50) && doorType.doorRiReSts > 0){
                centerWheeli.x = -575.399;
                centerWheeli.z = -74.985;
                centerWheeli.y = 616.324;

                int backRightDoor = 0;
                if (doorType.doorRiReSts == 1) backRightDoor = 1;
                else backRightDoor = -1;

                this->rotationAngle[24] += backRightDoor * this->doorAnimationDelta;
                if((this->rotationAngle[24] >= this->doorAnimationMaxValue) && (backRightDoor==1)){
                    this->rotationAngle[24] = this->doorAnimationMaxValue;
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[24]), glm::vec3(0.f, 1.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                }
                else if(this->rotationAngle[24]<=0&& backRightDoor==-1){
                    this->rotationAngle[24]=0.f;
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[24]), glm::vec3(0.f, 1.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                }
                else{
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[24]), glm::vec3(0.f, 1.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                }
            }

            /**************************************************************************************************
             * @brief TRUNK
             *        trSts 后备箱开关状态: 0 未知; 1  打开; 2 关闭
            **************************************************************************************************/
            if(((i <= 25 && i >= 19)||i==44) && doorType.trSts > 0) {
                centerWheeli.x = 0;
                centerWheeli.z = -1030.64;
                centerWheeli.y = 1027.27;

                int tailgate = 0;
                if (doorType.trSts == 1) tailgate = 1;
                else tailgate = -1;

                this->rotationAngle[26] += tailgate * this->trunkAnimationDelta;
                if((this->rotationAngle[26] >= this->trunkAnimationMaxValue) && (tailgate==1)){
                    this->rotationAngle[26] = this->trunkAnimationMaxValue;
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(50.f), glm::vec3(1.f, 0.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);

                } else if(this->rotationAngle[26]<=0 && tailgate==-1) {
                    this->rotationAngle[26]=0.f;
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(0.f), glm::vec3(1.f, 0.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                } else{
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[26]), glm::vec3(1.f, 0.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                }
            }

            /**************************************************************************************************
             * @brief HOOD, ENGINE COVER
             *        hoodSts 引擎盖状态: 0 未知; 1 打开; 2 关闭
            **************************************************************************************************/
            if( i == 26 && doorType.hoodSts > 0) {

                centerWheeli.x = 0;
                centerWheeli.y = 739.778;
                centerWheeli.z = 792.449;

                int engineCompartmentCover = 0;
                if (doorType.hoodSts == 1) engineCompartmentCover = 1;
                else engineCompartmentCover = -1;

                this->rotationAngle[28] += engineCompartmentCover * this->hoodAnimationDelta;
                if((this->rotationAngle[28]>= this->hoodAnimationMaxValue) &&  (engineCompartmentCover==1)) {
                    this->rotationAngle[28] = this->hoodAnimationMaxValue;
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(50.f), glm::vec3(-1.f, 0.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                } else if(this->rotationAngle[28] <= 0 && engineCompartmentCover==-1) {
                    this->rotationAngle[28]=0.f;
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(0.f), glm::vec3(-1.f, 0.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                } else{
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, centerWheeli);
                    egoCarWheelMat4 = glm::rotate(egoCarWheelMat4, glm::radians(this->rotationAngle[28]), glm::vec3(-1.f, 0.f, 0.0f));
                    egoCarWheelMat4 = glm::translate(egoCarWheelMat4, -centerWheeli);
                }
            }

            egoCarWheelMat4 = glm::scale(egoCarWheelMat4, glm::vec3(VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR, VV_MODEL_SCALE_FACTOR));

            glm::mat4 mvpMatrix = pvMat4 * egoCarWheelMat4;

            glEnable(GL_CULL_FACE);
            if (((i == 26) && (doorType.hoodSts == 1)) ||
                ((i == 28) || (i == 32) || (i == 36) || (i == 38)) || // for windows
                ((i == 27) || (i == 31) || (i == 35) || (i == 37)) || // for doors
                ((i >= 40) && (i <= 43))                              // for wheels
                    )
            {
                glDisable(GL_CULL_FACE);
            };

            // draw
            this->modelspair.second.SetMat4("u_MVP", mvpMatrix);
            this->modelspair.first.DrawMesh(i, this->modelspair.second);
        }

        glDisable(GL_DEPTH_TEST);
        // GLX_AA_BLEND_DISABLE();
        glDisable(GL_CULL_FACE);
        // LOGGER_GLE("Car3DModelDisplay::DoRender done");
    }
};
 
 