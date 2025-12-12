/*
 • To change this license header, choose License Headers in Project Properties.

 • To change this template file, choose Tools | Templates

 • and open the template in the editor.

 */

/*
 • File:   ObstacleDisplay.cpp

 • Author: tron

 • Function: 障碍物显示

 • Created on 2023年5月6日, 下午5:57

 */

#include "../tool/TimeHelper.hpp"

#include "ObstacleDisplay.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "CarModelAnimation3D.hpp"

namespace VirtualView3D {

/** @brief YAW PITCH for OpengGL coordinate */
#define OBSTACLE_YAW_GL_COORD           0.f, 1.f, 0.f
#define OBSTACLE_PITCH_GL_COORD         0.0f, 0.0f, -1.0f

#define VV_OBSTACLE_TRANS_FACTOR        0.668f // 0.64618f // 0.65f

    ObstacleDisplay::ObstacleDisplay():mainScaleFactor(1.0f),mainTransFactor(1.0f)
    {
    }

    ObstacleDisplay::~ObstacleDisplay()
    {
        for (auto &msPair : this->modelsmap) {
            /** @brief  delete VAO VBO ... */
            for (std::vector<Mesh>::iterator iter = msPair.second.first.meshes.begin(); iter != msPair.second.first.meshes.end(); iter++) {
                glDeleteVertexArrays(1, &iter->VAO);
                glDeleteBuffers(1, &iter->VBO);
                glDeleteBuffers(1, &iter->EBO);
            }

            /** @brief delete texture */
            for (std::vector<Texture>::iterator iter = msPair.second.first.texturesLoaded.begin(); iter != msPair.second.first.texturesLoaded.end(); iter++) {
                glDeleteTextures(1, &iter->id);
            }
        }
    }

/**
 • @brief Init

 • @details load 3d models here

**/
    void ObstacleDisplay::Initialization()
    {
        Model  model_car;
        Model  model_truck;
        Model  model_bus;
        Model  model_tram;
        Model  model_engineercar;
        Model  model_cart;
        Model  model_tricycle;
        Model  model_motocycle;
        Model  model_electriccycle;
        Model  model_bicycle;
        Model  model_pedestrian;
        Model  model_cone;

        Shader shader_car;
        Shader shader_truck;
        Shader shader_bus;
        Shader shader_tram;
        Shader shader_engineercar;
        Shader shader_cart;
        Shader shader_tricycle;
        Shader shader_motocycle;
        Shader shader_electriccycle;
        Shader shader_bicycle;
        Shader shader_pedestrian;
        Shader shader_cone;

        long timeus = TimeHelper::GetTimestampMicros();
        /** init for obstacle car */
        model_car.Initialization("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/models/Geely/Geely1.obj");
        model_car.LoadModel();

        shader_car.FromSourceFile("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.vs", "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.fs");
        this->modelsmap[ObstacleType::OBSTACLE_TYPE_CAR] = std::pair<Model, Shader>(model_car, shader_car);

        /** init for obstacle truck */
        model_truck.Initialization("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/models/Obstacle/Truck.obj");
        model_truck.LoadModel();
        shader_truck.FromSourceFile("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.vs", "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.fs");
        this->modelsmap[ObstacleType::OBSTACLE_TYPE_TRUCK] = std::pair<Model, Shader>(model_truck, shader_truck);

        /** init for obstacle bus */
        model_bus.Initialization("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/models/Obstacle/Bus.obj");
        model_bus.LoadModel();
        shader_bus.FromSourceFile("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.vs", "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.fs");
        this->modelsmap[ObstacleType::OBSTACLE_TYPE_BUS] = std::pair<Model, Shader>(model_bus, shader_bus);

        /** init for obstacle motocycle */
        model_motocycle.Initialization("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/models/Obstacle/MOTO.obj");
        model_motocycle.LoadModel();
        shader_motocycle.FromSourceFile("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.vs", "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.fs");
        this->modelsmap[ObstacleType::OBSTACLE_TYPE_MOTOCYCLE] = std::pair<Model, Shader>(model_motocycle, shader_motocycle);

        /** init for obstacle bicycle */
        model_bicycle.Initialization("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/models/Obstacle/bicycle.obj");
        model_bicycle.LoadModel();
        shader_bicycle.FromSourceFile("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.vs", "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.fs");
        this->modelsmap[ObstacleType::OBSTACLE_TYPE_BICYCLE] = std::pair<Model, Shader>(model_bicycle, shader_bicycle);

        /** init for obstacle pedestrian */
        model_pedestrian.Initialization("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/models/Obstacle/human.obj");
        model_pedestrian.LoadModel();
        shader_pedestrian.FromSourceFile("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.vs", "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.fs");
        this->modelsmap[ObstacleType::OBSTACLE_TYPE_PEDESTRIAN] = std::pair<Model, Shader>(model_pedestrian, shader_pedestrian);

        /** init for obstacle cone */
        model_cone.Initialization("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/models/Obstacle/ConeBarrel.obj"); // TODO: add ConeBarrel obj file to junlian's resource dir, and change this path to /storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/models/Obstacle/ConeBarrel.obj
        model_cone.LoadModel();
        shader_cone.FromSourceFile("/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.vs", "/storage/emulated/0/Android/data/com.demo.lib3d/files/Download/avm_resource/image/carmodel3d/junlian/FX11_A2/animation3d/shader/new/transform.fs");
        this->modelsmap[ObstacleType::OBSTACLE_TYPE_TRAFFICCONE] = std::pair<Model, Shader>(model_cone, shader_cone);

        this->mainScaleFactor = this->modelsmap[ObstacleType::OBSTACLE_TYPE_CAR].first.GetFrustumFitScale();
        this->mainScaleFactor = VV_CAMERA_DISTANCE_FACTOR * 1.08f / (this->mainScaleFactor + 1e-6);

        this->mainTransFactor = this->modelsmap[ObstacleType::OBSTACLE_TYPE_CAR].first.GetFrustumFitScale() / VV_CAMERA_DISTANCE_FACTOR;

        LOGGER_I("Obstacles LoadModel duration:%ld(us)", TimeHelper::GetTimestampMicros() - timeus);

    }

    void ObstacleDisplay::SetExtraRotation(float angle, const glm::vec3 &axis)
    {
        extraRotationMat4 = glm::rotate(glm::mat4(1.f), angle, axis);
    }


/*设置显示障碍物 传入障碍物vector列表*/
    void ObstacleDisplay::DoRender(const VisualAngleControl &visualAngleControl, std::vector<Obstacle> &obstacles)
    {
        if (obstacles.size() == 0) return;
        bool isDraw = false;

        glEnable(GL_DEPTH_TEST);
        // GLX_AA_BLEND_ENABLE();
        glEnable(GL_CULL_FACE);

        Model  obstacleModel;
        Shader obstacleShader;

        float scale1;

        // 获取视角控制参数
        float radiansPitch = visualAngleControl.GetPitchRotationVisualAngle();
        float radiansYaw = -visualAngleControl.GetLeftRightVisualAngle();

        // 创建父矩阵，与ParkingDisplay中的方式相同
        glm::mat4 parentMatrix = glm::mat4(1.0f);
        float scalexyz = this->mainScaleFactor * visualAngleControl.GetScaleVisualAngle();
        parentMatrix = glm::scale(parentMatrix, glm::vec3(scalexyz, scalexyz, scalexyz));
        parentMatrix = glm::rotate(parentMatrix, radiansPitch, glm::vec3(OBSTACLE_PITCH_GL_COORD));
        parentMatrix = glm::rotate(parentMatrix, radiansYaw, glm::vec3(OBSTACLE_YAW_GL_COORD));
        parentMatrix = parentMatrix * extraRotationMat4; // 应用额外旋转

        // 计算投影视图矩阵
        glm::mat4 pvMat4 = visualAngleControl.projectMat4 * visualAngleControl.viewMat4;

        for (size_t i = 0; i < obstacles.size(); i++)
        {
            isDraw       = false;
            scale1       = 1.0f;  /// default

            glm::vec3 coordFactorVec3 = glm::vec3(VV_OBSTACLE_TRANS_FACTOR, 0.f, VV_OBSTACLE_TRANS_FACTOR);

            Obstacle obstacle          = obstacles[i];
            obstacle.coordinate.angle -= M_PI_2;
            glm::vec3 transVec3(0.f);

            switch (obstacle.type)
            {
                case ObstacleType::OBSTACLE_TYPE_CAR:
                    isDraw = true;
                    obstacleModel     = this->modelsmap[ObstacleType::OBSTACLE_TYPE_CAR].first;
                    obstacleShader    = this->modelsmap[ObstacleType::OBSTACLE_TYPE_CAR].second;
                    break;
                case ObstacleType::OBSTACLE_TYPE_TRUCK:
                    isDraw = true;
                    obstacleModel  = this->modelsmap[ObstacleType::OBSTACLE_TYPE_TRUCK].first;
                    obstacleShader = this->modelsmap[ObstacleType::OBSTACLE_TYPE_TRUCK].second;
                    break;
                case ObstacleType::OBSTACLE_TYPE_BUS:
                    isDraw = true;
                    obstacleModel  = this->modelsmap[ObstacleType::OBSTACLE_TYPE_BUS].first;
                    obstacleShader = this->modelsmap[ObstacleType::OBSTACLE_TYPE_BUS].second;
                    scale1 = 5.f;
                    break;
                case ObstacleType::OBSTACLE_TYPE_TRAM:
                    isDraw = true;
                    obstacleModel  = this->modelsmap[ObstacleType::OBSTACLE_TYPE_TRAM].first;
                    obstacleShader = this->modelsmap[ObstacleType::OBSTACLE_TYPE_TRAM].second;
                    break;
                case ObstacleType::OBSTACLE_TYPE_ENGINEERCAR:
                    isDraw = true;
                    obstacleModel  = this->modelsmap[ObstacleType::OBSTACLE_TYPE_ENGINEERCAR].first;
                    obstacleShader = this->modelsmap[ObstacleType::OBSTACLE_TYPE_ENGINEERCAR].second;
                    scale1 = 5.f;
                    break;
                case ObstacleType::OBSTACLE_TYPE_CART:
                    isDraw = true;
                    obstacleModel  = this->modelsmap[ObstacleType::OBSTACLE_TYPE_CART].first;
                    obstacleShader = this->modelsmap[ObstacleType::OBSTACLE_TYPE_CART].second;
                    break;
                case ObstacleType::OBSTACLE_TYPE_TRICYCLE:
                    isDraw = true;
                    obstacleModel  = this->modelsmap[ObstacleType::OBSTACLE_TYPE_TRICYCLE].first;
                    obstacleShader = this->modelsmap[ObstacleType::OBSTACLE_TYPE_TRICYCLE].second;
                    break;
                case ObstacleType::OBSTACLE_TYPE_MOTOCYCLE:
                    isDraw = true;
                    obstacleModel  = this->modelsmap[ObstacleType::OBSTACLE_TYPE_MOTOCYCLE].first;
                    obstacleShader = this->modelsmap[ObstacleType::OBSTACLE_TYPE_MOTOCYCLE].second;
                    break;
                case ObstacleType::OBSTACLE_TYPE_ELECTRIC_BICYCLE:
                    isDraw = true;
                    obstacleModel  = this->modelsmap[ObstacleType::OBSTACLE_TYPE_ELECTRIC_BICYCLE].first;
                    obstacleShader = this->modelsmap[ObstacleType::OBSTACLE_TYPE_ELECTRIC_BICYCLE].second;
                    break;
                case ObstacleType::OBSTACLE_TYPE_BICYCLE:
                    isDraw = true;
                    obstacleModel  = this->modelsmap[ObstacleType::OBSTACLE_TYPE_BICYCLE].first;
                    obstacleShader = this->modelsmap[ObstacleType::OBSTACLE_TYPE_BICYCLE].second;
                    scale1 = 12.f;
                    break;
                case ObstacleType::OBSTACLE_TYPE_PEDESTRIAN:
                    isDraw = true;
                    obstacleModel  = this->modelsmap[ObstacleType::OBSTACLE_TYPE_PEDESTRIAN].first;
                    obstacleShader = this->modelsmap[ObstacleType::OBSTACLE_TYPE_PEDESTRIAN].second;
                    scale1 = 12.f;
                    break;
                case ObstacleType::OBSTACLE_TYPE_TRAFFICCONE:
                    isDraw = true;
                    obstacleModel  = this->modelsmap[ObstacleType::OBSTACLE_TYPE_TRAFFICCONE].first;
                    obstacleShader = this->modelsmap[ObstacleType::OBSTACLE_TYPE_TRAFFICCONE].second;
                    scale1 = 30.f;
                    break;
                default:
                case ObstacleType::OBSTACLE_TYPE_UNKOWN:
                    break;
            }

            if (isDraw)
            {
                // 坐标转换
                transVec3.x = obstacle.coordinate.x * this->mainTransFactor * coordFactorVec3.x;
                transVec3.y = obstacle.coordinate.z * this->mainTransFactor * coordFactorVec3.y;
                transVec3.z = -obstacle.coordinate.y * this->mainTransFactor * coordFactorVec3.z;
                transVec3.y = 0.f; // 强制 y=0（地面）

                obstacleShader.UseProgram();

                // 构建局部模型矩阵
                glm::mat4 localMatrix = glm::mat4(1.0f);
                localMatrix = glm::translate(localMatrix, transVec3);  // 障碍物位置平移
                localMatrix = glm::rotate(localMatrix, float(obstacle.coordinate.angle), glm::vec3(OBSTACLE_YAW_GL_COORD));  // 障碍物自身方向
                localMatrix = glm::scale(localMatrix, glm::vec3(VV_MODEL_SCALE_FACTOR * scale1, VV_MODEL_SCALE_FACTOR * scale1, VV_MODEL_SCALE_FACTOR * scale1));

                // 将局部矩阵与父矩阵结合，实现与车位同步旋转
                glm::mat4 modelMatrix = parentMatrix * localMatrix;

                // 计算 MVP 矩阵
                glm::mat4 mvpMatrix = pvMat4 * modelMatrix;
                obstacleShader.SetMat4("u_MVP", mvpMatrix);

                obstacleModel.Draw(obstacleShader);
            }
        }

        glDisable(GL_DEPTH_TEST);
        // GLX_AA_BLEND_DISABLE();
        glDisable(GL_CULL_FACE);
    }

};