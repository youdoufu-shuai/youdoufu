//
// Created on 2024/1/11.
//

#pragma once
#include<android/log.h>
#include <GLES3/gl32.h>
#include <sys/stat.h>
#include <error.h>
#include <thread>
#include <chrono>
#include <android/log.h>

//#include <CarModelAnimation3D.hpp>
#include "Shader.hpp"
#include "Include.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "YamlLoader.hpp"
#include "GLHelper.hpp"
#include "GlprocLoader.hpp"
#include "ImageHelper.hpp"
#include "Logger.hpp"
#include "TimeHelper.hpp"
#include "VirtualView3D.hpp"
#include "Car3DModelDisplay.hpp"
#include "CarExitDirection.hpp"
#include "GearDisplay.hpp"
#include "ObstacleDisplay.hpp"
#include "ParkingDisplay.hpp"
#include "ParkingProgress.hpp"
#include "ParkingProgressBar.hpp"
#include "RadarAlarmDisplay.hpp"
#include "RadarAlarmDistance.hpp"
#include "RadarWaterRipple.hpp"
#include "VehicleMoveDirectionArrow.hpp"
#include "VirtualView.hpp"
#include "VisualAngleControl.hpp"
#include "UIDisplay.hpp"

using namespace std;
using namespace VirtualView3D;

class RenderContext3D {
public:
    ~RenderContext3D();

    static RenderContext3D* GetInstance();

    static void DestroyInstance();

    void Init(std::string assetsDir);

    void OnSurfaceCreated();

    void OnSurfaceChanged(int width, int height);

    void OnDrawFrame();

    void OnTouchEvent(float deltaX, float deltaY);

    void OnScale(float scaleFactor);

    int32_t Touch(float x, float y);

    /**
     * @brief 开始车辆旋转
     * @param speed 旋转速度，默认使用当前设置的rotationSpeed
     * @param axis 旋转轴，默认为y轴(0,1,0)
     * @param resetRotation 是否重置当前旋转角度，默认为false
     */
    void StartRotate(float speed = -1.0f, const glm::vec3& axis = glm::vec3(0.f, 1.f, 0.f), bool resetRotation = false);

    /**
     * @brief 设置车辆的绝对旋转角度
     * @param angle 旋转角度（弧度）
     * @param axis 旋转轴，默认为z轴(0,0,1)
     */
    void SetRotationAngle(float angle, const glm::vec3& axis = glm::vec3(0.f, 0.f, 1.f));
    
    /**
     * @brief 应用旋转到所有需要旋转的组件
     * @param angle 旋转角度（弧度）
     * @param axis 旋转轴
     */
    void ApplyRotation(float angle, const glm::vec3& axis);

    void StartWayFinding(glm::dvec3 carPosVec3, std::vector<Slot> &parkingSlot, u_int32_t selectedParkingSlotId);

    /**
     * @brief 移动到指定位置
     * @param carPosVec3 当前位置
     * @param PositionVec3 目标位置
     * @param moveSpeed 移动速度，默认为0.02f
     * @param xOffset X轴临时偏移量，默认为0.0f
     * @param yOffset Y轴临时偏移量，默认为0.0f
     */
    void MoveToPosition(glm::vec3 carPosVec3, glm::vec3 PositionVec3, float moveSpeed = 0.02f, float xOffset = 0.0f, float yOffset = 0.0f);

    /**
     * @brief 更新移动状态
     * @return 是否正在移动
     */
    bool UpdateMovement();

    /**
     * @brief 预设的移动
     * @param flag 用于判断向左还是向右预设，如果flag为正则向右，flag为左则向左
     */
    void PerfabMove(bool flag);

public:
    VirtualView virtualView;
    std::vector<Slot> parkingSlots;
    int i;

private:
    static RenderContext3D *context;

    RenderContext3D();

    // 基础渲染相关
    BaseLib::Shader simpleShader;
    unsigned int VBO, VAO, EBO;
    int width;
    int height;
    UIDisplay uiDisplay;

    CarDoorType carDoorType;
    std::chrono::time_point<std::chrono::steady_clock> now;
    int toggleState            = 0;
    int nframes                = 0;
    uint8_t switchSts          = 0; // undefined
    bool searchParkingWave     = true; // search parking slot radar wave
    CarMotionStatu carWheelSts = CarMotionStatu::STOP;
    uint8_t progressPercentage = 0;
    GearPosition gear          = GearPosition::GEAR_R;
    ParkoutType parkoutType      = ParkoutType::LEFT;
    VirtualView3D::ApaStatus apaStatus  = {.remainDistance = 0.f};
    bool isVehicleExitDoRender   = false;
    bool isVirtualParkStatic     = true;
    glm::dvec3 carPosVec3        = glm::dvec3(0.0, 0.0, 0.0); // car position (x,y,z)
    ParkingDisplay parkingDisplay;
    uint32_t lastSelectedSlotId  = 0;
    static float slotOffset;
    static float slotOffset1;
    static float rotationSpeed;
    static float moveSpeed; // 移动速度
    static float defaultRotationAngle; // 默认旋转角度
    static glm::vec3 defaultRotationAxis; // 默认旋转轴
    bool isChooseParkingSlot = false;
    uint32_t selectedParkingSlotId = 0; // ID of the confirmed selected slot
    bool showOnlySelectedSlot = false; // Flag to control rendering only the selected slot

    // 用于平滑移动的变量
    float targetPositionX = 0.0f; // 目标X位置
    float targetPositionY = 0.0f; // 目标Y位置
    bool isMoving = false; // 是否正在移动

    // 用于分两次移动的变量
    bool needSecondPhaseMove = false; // 是否需要执行第二阶段移动
    bool isSecondPhase = false; // 是否处于第二阶段移动（倒车入库阶段）
    bool isSecondPhaseFirstFrame = false; // 是否是第二阶段的第一帧
    glm::vec3 secondPhaseTargetPos; // 第二阶段移动的目标位置

    // 用于曲线运动的变量
    float initialDistanceX = 0.0f; // 第二阶段开始时到目标的X距离
    float initialDistanceY = 0.0f; // 第二阶段开始时到目标的Y距离
    float totalInitialDistance = 0.0f; // 第二阶段开始时到目标的总距离

    // 用于车身旋转的变量
    float targetRotationAngle = 0.0f; // 目标旋转角度（90度）
    float currentRotationAngle = 0.0f; // 当前旋转角度
    float initialRotationAngle = 0.0f; // 第二阶段开始时的初始旋转角度
    float finalRotationAngle = glm::radians(90.0f); // 最终旋转角度（90度）

    std::vector<PdcInfo> pdcInfo = MakePdcInfoSampleData();
    std::vector<VirtualView3D::PdcDistance> pdcDistance = MakePdcDistance();
    std::vector<Obstacle>  obstacles;

    // 帧缓冲对象
    BaseLib::Framebuffer fboMSAA;
    BaseLib::Framebuffer fboSSAA;

    // 资源目录
    std::string resourceDir;

    // 辅助函数声明
    std::vector<VirtualView3D::Slot> MakeParkingSlotsSampleData();
    std::vector<VirtualView3D::Obstacle> MakeObstacleSampleData();
    static std::vector<VirtualView3D::PdcInfo> MakePdcInfoSampleData();
    static std::vector<VirtualView3D::PdcDistance> MakePdcDistance();
    static std::vector<VirtualView3D::Obstacle> MakeObstacleDataByPark(std::vector<VirtualView3D::Slot> &slots, float yaw);
};