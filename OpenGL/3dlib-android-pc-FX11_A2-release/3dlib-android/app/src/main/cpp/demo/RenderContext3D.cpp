#include "RenderContext3D.h"

/**
 * @brief 清屏颜色定义 (R, G, B, A)
 */
#define CLEAR_COLOR 1.f, 1., 1.f, 1.0f

/**
 * @brief 窗口默认宽度
 */
#define WIN_WIDTH  1024

/**
 * @brief 窗口默认高度
 */
#define WIN_HEIGHT 1800

using namespace std;
using namespace VirtualView3D;

/**
 * @brief 汽车模型视口定义 (x, y, width, height)
 * @note 修改屏幕尺寸的时候，UIDisplay.cpp也要相应修改
 */
#define VIEWPORT_TUPLE_CAR_MODEL    0, 0, 1080, 2400
//  #define VIEWPORT_TUPLE_CAR_MODEL    0, 0, 1280, 2856

/**
 * @brief 超采样抗锯齿比例
 */
#define SSAA_SCALE 2

/**
 * @brief 多重采样抗锯齿最大采样数
 */
#define MSAA_MAX   4

/**
 * @brief 启用停车位显示功能
 */
#define VV_AA_PARKING_DISPLAY

/**
 * @brief 启用雷达水波纹效果
 */
#define VV_AA_RADAR_WATER_RIPPLE

/**
 * @brief 启用车辆移动方向箭头
 */
#define VV_AA_MOVE_DIRECTION_ARROW

/**
 * @brief 启用车辆出口方向指示
 */
#define VV_AA_CAR_EXIT_DIRECTION

/**
 * @brief 启用停车进度条
 */
#define VV_AA_PARKING_PROGRESS_BAR

/**
 * @brief 启用停车进度显示
 */
#define VV_AA_PARKING_PROGRESS

/**
 * @brief 启用雷达警报距离显示
 */
#define VV_AA_RADAR_ALARM_DISTANCE

/**
 * @brief 启用3D车辆模型显示
 */
#define VV_AA_CAR_3D_MODEL_DISPLAY

/**
 * @brief 启用障碍物显示
 */
#define VV_AA_OBSTACLE_DISPLAY

/**
 * @brief 启用雷达警报显示
 */
#define VV_AA_RADAR_ALARM_DISPLAY

/**
 * @brief 程序运行状态标志
 */
static bool isRunning = true;

/**
 * @brief 信号处理函数，用于处理程序终止信号
 * @param signal 接收到的信号类型
 */
void signalStop(int signal) {
    __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "接收到终止信号: %d", signal);
    isRunning = false;
}

/**
 * @brief 注册信号处理函数，用于捕获SIGINT和SIGTERM信号
 */
void registerStopSignal() {
    signal(SIGINT, signalStop);   // 注册中断信号处理
    signal(SIGTERM, signalStop);  // 注册终止信号处理
}

// 静态成员变量初始化
RenderContext3D *RenderContext3D::context = nullptr;  // 单例实例指针
float RenderContext3D::slotOffset = 0.f;              // 停车位X轴偏移量
float RenderContext3D::slotOffset1 = 0.f;             // 停车位Y轴偏移量
float RenderContext3D::rotationSpeed = 0.f;           // 旋转速度
float RenderContext3D::moveSpeed = 0.02f;             // 默认移动速度
float RenderContext3D::defaultRotationAngle = 0.f;    // 默认旋转角度
glm::vec3 RenderContext3D::defaultRotationAxis = glm::vec3(0.f, 1.f, 0.f);  // 默认旋转轴(Y轴)

/**
 * @brief 获取RenderContext3D单例实例
 * @return RenderContext3D单例指针
 */
RenderContext3D *RenderContext3D::GetInstance() {
    if (context == nullptr) {
        __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "创建RenderContext3D单例实例");
        context = new RenderContext3D();
    }
    return context;
}

/**
 * @brief 销毁RenderContext3D单例实例，释放资源
 */
void RenderContext3D::DestroyInstance() {
    if (context != nullptr) {
        __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "销毁RenderContext3D单例实例");
        delete context;
        context = nullptr;
    }
}

/**
 * @brief RenderContext3D构造函数，初始化基本参数
 * @note 私有构造函数，只能通过GetInstance()方法获取实例
 */
RenderContext3D::RenderContext3D()
        : width(WIN_WIDTH), height(WIN_HEIGHT),                // 初始化窗口尺寸
          carPosVec3(0.0, 0.0, 0.0), carDoorType(),            // 初始化车辆位置和车门状态
          carWheelSts(VirtualView3D::CarMotionStatu::STOP), i(0), // 初始化车轮状态
          currentRotationAngle(defaultRotationAngle) {         // 初始化当前旋转角度
    __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "RenderContext3D构造函数被调用");
}

/**
 * @brief 初始化渲染上下文
 * @param assetsDir 资源目录路径
 */
void RenderContext3D::Init(std::string assetsDir) {
    __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "初始化渲染上下文，资源目录: %s", assetsDir.c_str());
    resourceDir = assetsDir;

    // 加载简单着色器
    BaseLib::Shader simpleShader;
    simpleShader.FromSourceFile("resource/junlian/virtualview/shader/simple.vs",
                                "resource/junlian/virtualview/shader/simple.fs");

    // 生成矩形顶点数组
    unsigned int VBO, VAO, EBO;
    GLHelper::GenRectangleVertexArray(glm::ivec4(VIEWPORT_TUPLE_CAR_MODEL),
                                      glm::ivec4(VIEWPORT_TUPLE_CAR_MODEL), false, VAO, VBO, EBO);

    // 初始化虚拟视图
    virtualView.Initialization();

    // 设置视角控制视口
    virtualView.visualAngleControl.SetViewport(VIEWPORT_TUPLE_CAR_MODEL);

    // 初始化UI显示
    uiDisplay.init();
    uiDisplay.setOnClickCallback([]() {
        // 点击回调函数，当前为空
    });
}

/**
 * @brief RenderContext3D析构函数，释放OpenGL资源
 */
RenderContext3D::~RenderContext3D() {
    __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "释放OpenGL资源");
    ::glDeleteVertexArrays(1, &VAO);
    ::glDeleteBuffers(1, &VBO);
    ::glDeleteBuffers(1, &EBO);
}

/**
 * @brief 当OpenGL表面创建时调用，初始化OpenGL资源
 */
void RenderContext3D::OnSurfaceCreated() {
    __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "OpenGL表面创建");

    // 设置清屏颜色
    glClearColor(CLEAR_COLOR);

    // 生成顶点数组和缓冲区
    ::glGenVertexArrays(1, &VAO);
    ::glGenBuffers(1, &VBO);
    ::glGenBuffers(1, &EBO);
}

/**
 * @brief 当OpenGL表面尺寸变化时调用，更新视口
 * @param width 新的宽度
 * @param height 新的高度
 */
void RenderContext3D::OnSurfaceChanged(int width, int height) {
    __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "OpenGL表面尺寸变化: %d x %d", width, height);
    this->width = width;
    this->height = height;

    // 设置OpenGL视口
    glViewport(0, 0, width, height);

    // 更新虚拟视图的视口
    virtualView.visualAngleControl.SetViewport(0, 0, width, height);
}

/**
 * @brief 开始车辆旋转
 * @param speed 旋转速度，如果为负值则使用当前设置的rotationSpeed
 * @param axis 旋转轴，默认为Y轴(0,1,0)
 * @param resetRotation 是否重置当前旋转角度，默认为false
 */
void RenderContext3D::StartRotate(float speed, const glm::vec3 &axis, bool resetRotation) {
    __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "开始旋转，速度: %f, 重置: %d", speed, resetRotation);
    static auto lastTime = now;
    float deltaTime = std::chrono::duration<float>(now - lastTime).count();
    lastTime = now;

    // 如果需要重置旋转角度
    if (resetRotation) {
        __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "重置旋转角度为0");
        currentRotationAngle = 0.0f;
    }

    // 使用提供的速度或默认速度
    float useSpeed = (speed < 0.0f) ? rotationSpeed : speed;

    // 计算目标角度
    float targetAngle = currentRotationAngle + useSpeed * deltaTime * 60.0f;

    // 使用线性插值平滑旋转
    float lerpFactor = 0.2f;
    currentRotationAngle = currentRotationAngle * (1.0f - lerpFactor) + targetAngle * lerpFactor;

    // 应用旋转到相关组件
    ApplyRotation(currentRotationAngle, axis);
}

/**
 * @brief 设置车辆的绝对旋转角度
 * @param angle 旋转角度（弧度）
 * @param axis 旋转轴，默认为Z轴(0,0,1)
 */
void RenderContext3D::SetRotationAngle(float angle, const glm::vec3 &axis) {
    __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "设置旋转角度: %f", angle);
    currentRotationAngle = -angle;

    ApplyRotation(angle, axis);
}

/**
 * @brief 应用旋转到所有需要旋转的组件
 * @param angle 旋转角度（弧度）
 * @param axis 旋转轴
 */
void RenderContext3D::ApplyRotation(float angle, const glm::vec3 &axis) {
    // 应用旋转到停车位显示组件
    virtualView.parkingDisplay.SetExtraRotation(angle, axis);
    // 应用旋转到障碍物显示组件
    virtualView.obstacleDisplay.SetExtraRotation(angle, axis);
}

/**
 * @brief 绘制每一帧
 * @note 此方法在每一帧渲染时被调用
 */
void RenderContext3D::OnDrawFrame() {
    // 清除颜色和深度缓冲
    glClearColor(CLEAR_COLOR);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 设置视口
    glViewport(VIEWPORT_TUPLE_CAR_MODEL);

    // 更新当前时间
    now = std::chrono::steady_clock::now();

    // 更新车辆移动状态
    UpdateMovement();

    // 更新停车进度百分比（每20帧更新一次）
    if (nframes % 20 == 0) {
        switch (progressPercentage) {
            case 0:
                progressPercentage = 1;  // 从0开始进度
                break;
            case 101:
                progressPercentage = 0;  // 进度达到101%时重置为0
                break;
            default:
                progressPercentage += 1;  // 正常情况下每次增加1%
                break;
        }
    }

    // 切换停车出口方向（左右交替）
    if (parkoutType == ParkoutType::LEFT) parkoutType = ParkoutType::RIGHT;
    else parkoutType = ParkoutType::LEFT;

    // 重置车门状态
    switchSts = 0x0;  // 所有车门关闭状态
    carDoorType.doorDrvrSts = switchSts;  // 驾驶员门状态
    carDoorType.doorLeReSts = switchSts;  // 左后门状态
    carDoorType.doorPassSts = switchSts;  // 乘客门状态
    carDoorType.doorRiReSts = switchSts;  // 右后门状态
    carDoorType.hoodSts = switchSts;      // 引擎盖状态
    carDoorType.trSts = switchSts;        // 后备箱状态

    // 更新APA（自动泊车辅助）剩余距离
    apaStatus.remainDistance += 0.05f;
    if (apaStatus.remainDistance >= 20.f) {
        apaStatus.remainDistance = 0.f;  // 距离达到20米时重置
    }

    // 设置切换状态
    toggleState = 1;

    // 根据切换状态生成障碍物数据
    if (toggleState == 1) {
        obstacles = MakeObstacleSampleData();  // 使用样例障碍物数据
    } else {
        obstacles = MakeObstacleDataByPark(parkingSlots, 1);  // 根据停车位生成障碍物数据
    }

    // 生成PDC（泊车距离控制）信息和距离数据
    std::vector<PdcInfo> pdcInfo = MakePdcInfoSampleData();
    std::vector<VirtualView3D::PdcDistance> pdcDistance = MakePdcDistance();

    // 生成停车位数据
    parkingSlots = MakeParkingSlotsSampleData();
    // 如果只显示选中的停车位
    if (showOnlySelectedSlot) {
        std::vector<Slot> selectedSlots;
        for (const auto &slot: parkingSlots) {
            if (slot.parkingID == selectedParkingSlotId) {
                selectedSlots.push_back(slot);
                break;
            }
        }
        parkingSlots = selectedSlots;
    }

    // 处理第二阶段移动（倒车入库）的旋转逻辑
    if (isSecondPhase) {
        // 第二阶段的第一帧特殊处理
        if (isSecondPhaseFirstFrame) {
            __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "第二阶段第一帧处理");
            // 如果当前旋转角度接近0，使用默认旋转角度
            if (fabs(currentRotationAngle) < 0.001f) {
                currentRotationAngle = defaultRotationAngle;
            }

            // 记录初始旋转角度
            initialRotationAngle = currentRotationAngle;
            isSecondPhaseFirstFrame = false;
        }

        // 计算当前到目标位置的总距离
        float currentTotalDistance = sqrt(
            pow(targetPositionX - carPosVec3.x, 2) +
            pow(targetPositionY - carPosVec3.y, 2)
        );
        // 计算移动进度（0-1之间）
        float progress = 1.0f - (currentTotalDistance / totalInitialDistance);

        // 确保进度在0-1范围内
        progress = std::max(0.0f, std::min(1.0f, progress));

        // 计算旋转进度，使用缓动函数实现平滑过渡
        float rotationProgress;
        if (progress < 0.5f) {
            // 前半段使用二次缓入函数
            rotationProgress = 2.0f * progress * progress;
        } else {
            // 后半段使用二次缓出函数
            float t = 2.0f * (progress - 0.5f);
            rotationProgress = 0.5f + 0.5f * (2.0f * t - t * t);
        }

        // 计算目标旋转角度
        float targetAngle = initialRotationAngle + (finalRotationAngle - initialRotationAngle) * rotationProgress;

        // 使用线性插值平滑旋转
        float lerpFactor = 0.1f;
        currentRotationAngle = currentRotationAngle * (1.0f - lerpFactor) + targetAngle * lerpFactor;

        // 确保旋转角度不超过最终角度
        if (finalRotationAngle > initialRotationAngle) {
            currentRotationAngle = std::min(currentRotationAngle, finalRotationAngle);
        } else {
            currentRotationAngle = std::max(currentRotationAngle, finalRotationAngle);
        }

        // 根据车辆位置应用正确方向的旋转
        if (carPosVec3.y > targetPositionY) {
            ApplyRotation(-currentRotationAngle, glm::vec3(0.f, 1.0f, 0.f));
        } else {
            ApplyRotation(currentRotationAngle, glm::vec3(0.f, 1.0f, 0.f));
        }
    } else {
        // 非第二阶段移动时的旋转处理
        if (carPosVec3.y > targetPositionY) {
            ApplyRotation(-currentRotationAngle, glm::vec3(0.f, 1.0f, 0.f));
        } else {
            ApplyRotation(currentRotationAngle, glm::vec3(0.f, 1.0f, 0.f));
        }
    }

    // 根据条件编译宏渲染各个组件

#ifdef VV_AA_PARKING_DISPLAY
    // 渲染停车位
    virtualView.parkingDisplay.DoRender(virtualView.visualAngleControl, parkingSlots);
    // 渲染虚拟停车位
    virtualView.parkingDisplay.DoRenderVirtual(virtualView.visualAngleControl, carPosVec3,
                                               isVirtualParkStatic);
#endif

#ifdef VV_AA_RADAR_WATER_RIPPLE
    // 渲染雷达水波纹效果
    virtualView.radarWaterRipple.DoRender(virtualView.visualAngleControl, searchParkingWave);
#endif

#ifdef VV_AA_MOVE_DIRECTION_ARROW
    // 渲染车辆移动方向箭头（前进方向）
    virtualView.vehicleMoveDirectionArrow.DoRender(virtualView.visualAngleControl,
                                                   MotionDir::MOTION_FORWARD);
#endif

#ifdef VV_AA_CAR_EXIT_DIRECTION
    // 渲染车辆出口方向（如果启用）
    if (isVehicleExitDoRender) {
        virtualView.carExitDirection.DoRender(virtualView.visualAngleControl, parkoutType);
    }
#endif

#ifdef VV_AA_PARKING_PROGRESS_BAR
    // 渲染停车进度条
    virtualView.parkingProgressBar.DoRender(virtualView.visualAngleControl, progressPercentage,
                                            gear);
#endif

#ifdef VV_AA_PARKING_PROGRESS
    // 渲染停车进度
    virtualView.parkingProgress.DoRender(virtualView.visualAngleControl, apaStatus);
#endif

#ifdef VV_AA_RADAR_ALARM_DISTANCE
    // 渲染雷达警报距离
    virtualView.radarAlarmDistance.DoRender(virtualView.visualAngleControl, pdcDistance);
#endif

#ifdef VV_AA_CAR_3D_MODEL_DISPLAY
    // 渲染3D车辆模型
    virtualView.car3DModelDisplay.DoRender(virtualView.visualAngleControl, carDoorType,
                                           carWheelSts);
#endif

#ifdef VV_AA_OBSTACLE_DISPLAY
    // 渲染障碍物
    virtualView.obstacleDisplay.DoRender(virtualView.visualAngleControl, obstacles);
#endif

#ifdef VV_AA_RADAR_ALARM_DISPLAY
    // 渲染雷达警报
    virtualView.radarAlarmDisplay.DoRender(virtualView.visualAngleControl, pdcInfo);
#endif

    // 如果正在选择停车位，渲染UI
    if (isChooseParkingSlot) {
        uiDisplay.render(0.45f, 0.65f, 0.1f, 0.1f);
    }

    // 帧计数器递增
    nframes++;
}

std::vector<VirtualView3D::Slot> RenderContext3D::MakeParkingSlotsSampleData() {
    std::vector<Slot> parkingSlots;
    Slot parkSlot;

    const float COMMON_PARK_WIDTH_HALF = 1.25f;
    const float COMMON_PARK_LENGTH_HALF = 2.5f;

    float parkCenterX;
    float parkCenterY;
    float parkCenterZ;

    float offset = slotOffset;
    float offset1 = slotOffset1;
    uint32_t currentSelectedId = lastSelectedSlotId;

    parkCenterX = -9.f - offset;
    parkCenterY = -4.5f - offset1;
    parkCenterZ = 0.f;

    parkSlot.parkingID = 1;
    parkSlot.displayNum = 1;
    parkSlot.parkingSpaceShowContents = (parkSlot.parkingID == currentSelectedId) ?
                                        ParkingSpaceShowContents::SHOW_BALLOON_P
                                                                                  : ParkingSpaceShowContents::SHOW_P;
    parkSlot.parkingType = ParkingType::PARKING_IS_POSSIBLE;
    parkSlot.parkingSpaceType = ParkingSpaceType::NORMAL_SPACE;
    parkSlot.slotType = SlotType::SLOT_RIGHT_VERT;
    parkSlot.parkingPoint[0] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[1] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[2] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[3] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkingSlots.push_back(parkSlot);

    parkCenterX = -6.f - offset;
    parkCenterY = -4.5f - offset1;
    parkCenterZ = 0.f;

    parkSlot.parkingID = 2;
    parkSlot.displayNum = 2;
    parkSlot.parkingSpaceShowContents = (parkSlot.parkingID == currentSelectedId) ?
                                        ParkingSpaceShowContents::SHOW_BALLOON_P
                                                                                  : ParkingSpaceShowContents::SHOW_P;
    parkSlot.parkingType = ParkingType::PARKING_IS_POSSIBLE;
    parkSlot.parkingSpaceType = ParkingSpaceType::NORMAL_SPACE;
    parkSlot.slotType = SlotType::SLOT_RIGHT_VERT;
    parkSlot.parkingPoint[0] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[1] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[2] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[3] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkingSlots.push_back(parkSlot);

    parkCenterX = -3.f - offset;
    parkCenterY = -4.5f - offset1;
    parkCenterZ = 0.f;

    parkSlot.parkingID = 3;
    parkSlot.displayNum = 3;
    parkSlot.parkingSpaceShowContents = (parkSlot.parkingID == currentSelectedId) ?
                                        ParkingSpaceShowContents::SHOW_BALLOON_P
                                                                                  : ParkingSpaceShowContents::SHOW_P;
    parkSlot.parkingType = ParkingType::PARKING_IS_POSSIBLE;
    parkSlot.parkingSpaceType = ParkingSpaceType::NORMAL_SPACE;
    parkSlot.slotType = SlotType::SLOT_RIGHT_VERT;
    parkSlot.parkingPoint[0] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[1] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[2] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[3] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkingSlots.push_back(parkSlot);

    parkCenterX = 0.f - offset;
    parkCenterY = -4.5f - offset1;
    parkCenterZ = 0.f;

    parkSlot.parkingID = 4;
    parkSlot.displayNum = 4;
    parkSlot.parkingSpaceShowContents = (parkSlot.parkingID == currentSelectedId) ?
                                        ParkingSpaceShowContents::SHOW_BALLOON_P
                                                                                  : ParkingSpaceShowContents::SHOW_P;
    parkSlot.parkingType = ParkingType::PARKING_IS_POSSIBLE;
    parkSlot.parkingSpaceType = ParkingSpaceType::NORMAL_SPACE;
    parkSlot.slotType = SlotType::SLOT_RIGHT_VERT;
    parkSlot.parkingPoint[0] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[1] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[2] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[3] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkingSlots.push_back(parkSlot);

    parkCenterX = 3.f - offset;
    parkCenterY = -4.5 - offset1;
    parkCenterZ = 0.f;

    parkSlot.parkingID = 5;
    parkSlot.displayNum = 5;
    parkSlot.parkingSpaceShowContents = (parkSlot.parkingID == currentSelectedId) ?
                                        ParkingSpaceShowContents::SHOW_BALLOON_P
                                                                                  : ParkingSpaceShowContents::SHOW_P;
    parkSlot.parkingType = ParkingType::PARKING_IS_POSSIBLE;
    parkSlot.parkingSpaceType = ParkingSpaceType::NORMAL_SPACE;
    parkSlot.slotType = SlotType::SLOT_RIGHT_VERT;
    parkSlot.parkingPoint[0] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[1] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[2] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[3] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkingSlots.push_back(parkSlot);

    parkCenterX = 6.f - offset;
    parkCenterY = -4.5f - offset1;
    parkCenterZ = 0.f;

    parkSlot.parkingID = 6;
    parkSlot.displayNum = 6;
    parkSlot.parkingSpaceShowContents = (parkSlot.parkingID == currentSelectedId) ?
                                        ParkingSpaceShowContents::SHOW_BALLOON_P
                                                                                  : ParkingSpaceShowContents::SHOW_P;
    parkSlot.parkingType = ParkingType::PARKING_IS_POSSIBLE;
    parkSlot.parkingSpaceType = ParkingSpaceType::NORMAL_SPACE;
    parkSlot.slotType = SlotType::SLOT_RIGHT_VERT;
    parkSlot.parkingPoint[0] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[1] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[2] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[3] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkingSlots.push_back(parkSlot);

    parkCenterX = 9.f - offset;
    parkCenterY = -4.5f - offset1;
    parkCenterZ = 0.f;

    parkSlot.parkingID = 7;
    parkSlot.displayNum = 7;
    parkSlot.parkingSpaceShowContents = (parkSlot.parkingID == currentSelectedId) ?
                                        ParkingSpaceShowContents::SHOW_BALLOON_P
                                                                                  : ParkingSpaceShowContents::SHOW_P;
    parkSlot.parkingType = ParkingType::PARKING_IS_POSSIBLE;
    parkSlot.parkingSpaceType = ParkingSpaceType::NORMAL_SPACE;
    parkSlot.slotType = SlotType::SLOT_RIGHT_VERT;
    parkSlot.parkingPoint[0] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[1] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[2] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[3] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkingSlots.push_back(parkSlot);

    parkCenterX = -9.f - offset;
    parkCenterY = 4.5f - offset1;
    parkCenterZ = 0.f;

    parkSlot.parkingID = 8;
    parkSlot.displayNum = 1;
    parkSlot.parkingSpaceShowContents = (parkSlot.parkingID == currentSelectedId) ?
                                        ParkingSpaceShowContents::SHOW_BALLOON_P
                                                                                  : ParkingSpaceShowContents::SHOW_P;
    parkSlot.parkingType = ParkingType::PARKING_IS_POSSIBLE;
    parkSlot.parkingSpaceType = ParkingSpaceType::NORMAL_SPACE;
    parkSlot.slotType = SlotType::SLOT_LEFT_VERT;
    parkSlot.parkingPoint[0] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[1] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[2] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[3] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkingSlots.push_back(parkSlot);

    parkCenterX = -6.f - offset;
    parkCenterY = 4.5f - offset1;
    parkCenterZ = 0.f;

    parkSlot.parkingID = 9;
    parkSlot.displayNum = 2;
    parkSlot.parkingSpaceShowContents = (parkSlot.parkingID == currentSelectedId) ?
                                        ParkingSpaceShowContents::SHOW_BALLOON_P
                                                                                  : ParkingSpaceShowContents::SHOW_P;
    parkSlot.parkingType = ParkingType::PARKING_IS_POSSIBLE;
    parkSlot.parkingSpaceType = ParkingSpaceType::NORMAL_SPACE;
    parkSlot.slotType = SlotType::SLOT_LEFT_VERT;
    parkSlot.parkingPoint[0] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[1] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[2] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[3] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkingSlots.push_back(parkSlot);

    parkCenterX = -3.f - offset;
    parkCenterY = 4.5f - offset1;
    parkCenterZ = 0.f;

    parkSlot.parkingID = 10;
    parkSlot.displayNum = 3;
    parkSlot.parkingSpaceShowContents = (parkSlot.parkingID == currentSelectedId) ?
                                        ParkingSpaceShowContents::SHOW_BALLOON_P
                                                                                  : ParkingSpaceShowContents::SHOW_P;
    parkSlot.parkingType = ParkingType::PARKING_IS_POSSIBLE;
    parkSlot.parkingSpaceType = ParkingSpaceType::NORMAL_SPACE;
    parkSlot.slotType = SlotType::SLOT_LEFT_VERT;
    parkSlot.parkingPoint[0] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[1] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[2] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[3] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkingSlots.push_back(parkSlot);

    parkCenterX = 0.f - offset;
    parkCenterY = 4.5f - offset1;
    parkCenterZ = 0.f;

    parkSlot.parkingID = 11;
    parkSlot.displayNum = 4;
    parkSlot.parkingSpaceShowContents = (parkSlot.parkingID == currentSelectedId) ?
                                        ParkingSpaceShowContents::SHOW_BALLOON_P
                                                                                  : ParkingSpaceShowContents::SHOW_P;
    parkSlot.parkingType = ParkingType::PARKING_IS_POSSIBLE;
    parkSlot.parkingSpaceType = ParkingSpaceType::NORMAL_SPACE;
    parkSlot.slotType = SlotType::SLOT_LEFT_VERT;
    parkSlot.parkingPoint[0] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[1] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[2] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[3] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkingSlots.push_back(parkSlot);

    parkCenterX = 3.f - offset;
    parkCenterY = 4.5f - offset1;
    parkCenterZ = 0.f;

    parkSlot.parkingID = 12;
    parkSlot.displayNum = 5;
    parkSlot.parkingSpaceShowContents = (parkSlot.parkingID == currentSelectedId) ?
                                        ParkingSpaceShowContents::SHOW_BALLOON_P
                                                                                  : ParkingSpaceShowContents::SHOW_P;
    parkSlot.parkingType = ParkingType::PARKING_IS_POSSIBLE;
    parkSlot.parkingSpaceType = ParkingSpaceType::NORMAL_SPACE;
    parkSlot.slotType = SlotType::SLOT_LEFT_VERT;
    parkSlot.parkingPoint[0] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[1] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[2] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[3] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkingSlots.push_back(parkSlot);

    parkCenterX = 6.f - offset;
    parkCenterY = 4.5f - offset1;
    parkCenterZ = 0.f;

    parkSlot.parkingID = 13;
    parkSlot.displayNum = 6;
    parkSlot.parkingSpaceShowContents = (parkSlot.parkingID == currentSelectedId) ?
                                        ParkingSpaceShowContents::SHOW_BALLOON_P
                                                                                  : ParkingSpaceShowContents::SHOW_P;
    parkSlot.parkingType = ParkingType::PARKING_IS_POSSIBLE;
    parkSlot.parkingSpaceType = ParkingSpaceType::NORMAL_SPACE;
    parkSlot.slotType = SlotType::SLOT_LEFT_VERT;
    parkSlot.parkingPoint[0] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[1] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[2] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[3] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkingSlots.push_back(parkSlot);

    parkCenterX = 9.f - offset;
    parkCenterY = 4.5f - offset1;
    parkCenterZ = 0.f;

    parkSlot.parkingID = 14;
    parkSlot.displayNum = 7;
    parkSlot.parkingSpaceShowContents = (parkSlot.parkingID == currentSelectedId) ?
                                        ParkingSpaceShowContents::SHOW_BALLOON_P
                                                                                  : ParkingSpaceShowContents::SHOW_P;
    parkSlot.parkingType = ParkingType::PARKING_IS_POSSIBLE;
    parkSlot.parkingSpaceType = ParkingSpaceType::NORMAL_SPACE;
    parkSlot.slotType = SlotType::SLOT_LEFT_VERT;
    parkSlot.parkingPoint[0] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[1] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY -
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[2] = {.x= parkCenterX - COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkSlot.parkingPoint[3] = {.x= parkCenterX + COMMON_PARK_WIDTH_HALF, .y=parkCenterY +
                                                                             COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
    parkingSlots.push_back(parkSlot);

    return parkingSlots;
}

std::vector<VirtualView3D::Obstacle> RenderContext3D::MakeObstacleSampleData() {
    std::vector<Obstacle> obstacles;

    auto baseObstacles = MakeObstacleDataByPark(parkingSlots, 1);
    if (baseObstacles.empty()) {
        return obstacles;
    }

    const size_t requiredIndices[] = {0, 1, 2, 3, 6, 8, 9};
    for (size_t index: requiredIndices) {
        if (index >= baseObstacles.size()) {
            return obstacles;
        }
    }

    Obstacle obstacle;

    // 使用前4个障碍物
    for (int i = 0; i < 4 && i < baseObstacles.size(); i++) {
        obstacle.id = baseObstacles[i].id;
        obstacle.type = ObstacleType::OBSTACLE_TYPE_CAR;
        obstacle.coordinate = {
                .x = baseObstacles[i].coordinate.x,
                .y = baseObstacles[i].coordinate.y,
                .z = baseObstacles[i].coordinate.z,
                .angle = glm::radians(90.f)
        };
        obstacle.statu = ObstacleMotionStatu::STOP;
        obstacles.push_back(obstacle);
    }

    const size_t movingIndices[] = {6, 8, 9};
    for (int i = 0; i < 3; i++) {
        if (movingIndices[i] < baseObstacles.size()) {
            obstacle.id = baseObstacles[movingIndices[i]].id; // 保持与车位ID的映射关系
            obstacle.type = ObstacleType::OBSTACLE_TYPE_TRAFFICCONE;
            obstacle.coordinate = {
                    .x = baseObstacles[movingIndices[i]].coordinate.x,
                    .y = baseObstacles[movingIndices[i]].coordinate.y,
                    .z = baseObstacles[movingIndices[i]].coordinate.z,
                    .angle = glm::radians(-90.f)
            };
            obstacle.statu = ObstacleMotionStatu::SPORT;
            obstacles.push_back(obstacle);
        }
    }

    return obstacles;
}

std::vector<VirtualView3D::PdcInfo> RenderContext3D::MakePdcInfoSampleData() {
    std::vector<VirtualView3D::PdcInfo> pdcInfoVec;
    PdcInfo pdcInfo;

    pdcInfo.type = AreaType::LFM;
    pdcInfo.level = LevelType::LEVEL_1;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LFM;
    pdcInfo.level = LevelType::LEVEL_2;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LFM;
    pdcInfo.level = LevelType::LEVEL_3;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LFM;
    pdcInfo.level = LevelType::LEVEL_4;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RFM;
    pdcInfo.level = LevelType::LEVEL_1;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RFM;
    pdcInfo.level = LevelType::LEVEL_2;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RFM;
    pdcInfo.level = LevelType::LEVEL_3;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RFM;
    pdcInfo.level = LevelType::LEVEL_4;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LFC;
    pdcInfo.level = LevelType::LEVEL_1;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LFC;
    pdcInfo.level = LevelType::LEVEL_2;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LFC;
    pdcInfo.level = LevelType::LEVEL_3;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RFC;
    pdcInfo.level = LevelType::LEVEL_1;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RFC;
    pdcInfo.level = LevelType::LEVEL_2;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RFC;
    pdcInfo.level = LevelType::LEVEL_3;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LFS;
    pdcInfo.level = LevelType::LEVEL_1;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RFS;
    pdcInfo.level = LevelType::LEVEL_1;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LRS;
    pdcInfo.level = LevelType::LEVEL_1;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RRS;
    pdcInfo.level = LevelType::LEVEL_1;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LRC;
    pdcInfo.level = LevelType::LEVEL_1;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LRC;
    pdcInfo.level = LevelType::LEVEL_2;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LRC;
    pdcInfo.level = LevelType::LEVEL_3;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RRC;
    pdcInfo.level = LevelType::LEVEL_1;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RRC;
    pdcInfo.level = LevelType::LEVEL_2;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RRC;
    pdcInfo.level = LevelType::LEVEL_3;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LRM;
    pdcInfo.level = LevelType::LEVEL_1;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LRM;
    pdcInfo.level = LevelType::LEVEL_2;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LRM;
    pdcInfo.level = LevelType::LEVEL_3;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LRM;
    pdcInfo.level = LevelType::LEVEL_4;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LRM;
    pdcInfo.level = LevelType::LEVEL_5;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::LRM;
    pdcInfo.level = LevelType::LEVEL_6;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RRM;
    pdcInfo.level = LevelType::LEVEL_1;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RRM;
    pdcInfo.level = LevelType::LEVEL_2;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RRM;
    pdcInfo.level = LevelType::LEVEL_3;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RRM;
    pdcInfo.level = LevelType::LEVEL_4;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RRM;
    pdcInfo.level = LevelType::LEVEL_5;
    pdcInfoVec.push_back(pdcInfo);

    pdcInfo.type = AreaType::RRM;
    pdcInfo.level = LevelType::LEVEL_6;
    pdcInfoVec.push_back(pdcInfo);

    return pdcInfoVec;
}

std::vector<VirtualView3D::PdcDistance> RenderContext3D::MakePdcDistance() {
    std::vector<VirtualView3D::PdcDistance> pdcDistanceVector;

    std::srand(std::time(nullptr));
    uint16_t distance = std::rand() % 998;

    PdcDistance pdcDist;
    pdcDist.position = PdcDistancePosition::FRONT;
    pdcDist.distance = distance;
    pdcDistanceVector.push_back(pdcDist);

    pdcDist.position = PdcDistancePosition::REAR;
    pdcDist.distance = distance + 1;
    pdcDistanceVector.push_back(pdcDist);

    return pdcDistanceVector;
}

std::vector<VirtualView3D::Obstacle>
RenderContext3D::MakeObstacleDataByPark(std::vector<VirtualView3D::Slot> &slots, float yaw) {

    std::vector<Obstacle> obstacles;

    for (int i = 0; i < slots.size(); i++) {
        if (i == 6) continue;
        VirtualView3D::Slot &slot = slots[i];
        float centerX = 0.f, centerY = 0.f, centerZ = 0.f;

        for (int j = 0; j < 4; j++) {
            centerX += slot.parkingPoint[j].x;
            centerY += slot.parkingPoint[j].y;
            centerZ += slot.parkingPoint[j].z;
        }

        centerX = centerX / 4;
        centerY = centerY / 4;
        centerZ = centerZ / 4;

        float parkYaw = 0.f;
        glm::vec2 vecDirCA = glm::vec2(slot.parkingPoint[0].x - slot.parkingPoint[2].x,
                                       slot.parkingPoint[0].y - slot.parkingPoint[2].y);
        glm::vec2 vecDirAB = glm::vec2(slot.parkingPoint[1].x - slot.parkingPoint[0].x,
                                       slot.parkingPoint[1].y - slot.parkingPoint[0].y);

        glm::vec2 vecDir = vecDirCA;

        if (glm::length(vecDirAB) > glm::length(vecDirCA)) vecDir = vecDirAB;

        if (fabs(vecDir.y) < 1e-6) {
            parkYaw = M_PI_2;
        } else {
            parkYaw = atan2(vecDir.y, vecDir.x);
        }

        Obstacle obstacle;
        obstacle.id = slot.parkingID;
        obstacle.type = ObstacleType::OBSTACLE_TYPE_BUS;
        obstacle.coordinate = {.x=centerX, .y=centerY, .z=centerZ, .angle=parkYaw};
        obstacle.statu = ObstacleMotionStatu::STOP;
        obstacles.push_back(obstacle);
    }

    return obstacles;
}

/**
 * @brief 处理触摸事件，调整视角
 * @param deltaX X轴移动距离
 * @param deltaY Y轴移动距离
 */
void RenderContext3D::OnTouchEvent(float deltaX, float deltaY) {
    __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "触摸事件: deltaX=%f, deltaY=%f", deltaX, deltaY);

    // 旋转灵敏度系数
    float rotationSensitivity = 0.0015f;

    // 计算偏航角和俯仰角变化量
    float yawAngle = -deltaX * rotationSensitivity;
    float pitchAngle = -deltaY * rotationSensitivity;

    // 获取当前视角
    float currentYaw = virtualView.visualAngleControl.GetLeftRightVisualAngle();
    float currentPitch = virtualView.visualAngleControl.GetPitchRotationVisualAngle();

    // 计算新的俯仰角并限制在合理范围内
    float newPitch = currentPitch + pitchAngle;
    if (newPitch > 0.5f) newPitch = 0.5f;  // 上限
    if (newPitch < -0.9) newPitch = -0.9;  // 下限

    // 更新视角控制
    // 注释掉的代码：virtualView.visualAngleControl.SetLeftRightVisualAngle(currentYaw + yawAngle);
    virtualView.visualAngleControl.SetPitchRotationVisualAngle(newPitch);
}

/**
 * @brief 处理缩放事件，调整视图缩放比例
 * @param scaleFactor 缩放因子
 */
void RenderContext3D::OnScale(float scaleFactor) {
    __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "缩放事件: scaleFactor=%f", scaleFactor);

    // 获取当前缩放比例
    float currentScale = virtualView.visualAngleControl.GetScaleVisualAngle();

    // 计算新的缩放比例
    float newScale = currentScale * scaleFactor;

    // 更新视角控制的缩放比例
    virtualView.visualAngleControl.SetScaleVisualAngle(newScale);
}

/**
 * @brief 处理点击事件，用于选择停车位
 * @param x 点击位置的X坐标
 * @param y 点击位置的Y坐标
 * @return 选中的停车位ID，如果没有选中则返回0
 */
int32_t RenderContext3D::Touch(float x, float y) {
    __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "点击事件: x=%f, y=%f", x, y);

    // 创建触摸点结构
    TouchPoint touchPoint;
    touchPoint.pixelX = x;
    touchPoint.pixelY = y;

    // 计算相对坐标（0-1范围）
    float relativeX = x / width;
    float relativeY = y / height;

    // 检查是否点击了UI
    if (uiDisplay.checkClick(relativeX, relativeY)) {
        __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "点击了UI确认按钮");
        isChooseParkingSlot = false;
        // 如果有上次选中的停车位，确认选择并开始寻路
        if (lastSelectedSlotId > 0) {
            selectedParkingSlotId = lastSelectedSlotId;
            showOnlySelectedSlot = true;
            __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "确认选择停车位ID: %d", selectedParkingSlotId);
            StartWayFinding(carPosVec3, parkingSlots, selectedParkingSlotId);
        }
        return 0;
    }

    // 检查是否点击了停车位
    int32_t selectedId = virtualView.parkingDisplay.Touch(touchPoint);
    __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "点击检测返回停车位ID: %d", selectedId);

    // 检查选中的停车位是否有障碍物
    bool hasObstacle = false;
    if (selectedId > 0) {
        for (const auto &obstacle: obstacles) {
            if (obstacle.id == selectedId) {
                hasObstacle = true;
                selectedId = 0;
                __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "停车位有障碍物，取消选择");
                break;
            }
        }
    }

    // 重置上次选中停车位的显示状态
    if (lastSelectedSlotId > 0 && lastSelectedSlotId <= parkingSlots.size()) {
        parkingSlots[lastSelectedSlotId - 1].parkingSpaceShowContents = ParkingSpaceShowContents::SHOW_P;
    }

    // 设置新选中停车位的显示状态
    if (selectedId > 0 && selectedId <= parkingSlots.size()) {
        // 设置为气球显示模式
        parkingSlots[selectedId - 1].parkingSpaceShowContents = ParkingSpaceShowContents::SHOW_BALLOON_P;
        lastSelectedSlotId = selectedId;
        isChooseParkingSlot = true;
        __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "选中停车位ID: %d", selectedId);
    }

    return selectedId;
}

/**
 * @brief 开始寻路，规划从当前位置到选定停车位的路径
 * @param carPosVec3 当前车辆位置
 * @param parkingSlots 停车位列表
 * @param selectedParkingSlotId 选定的停车位ID
 */
void RenderContext3D::StartWayFinding(glm::dvec3 carPosVec3, std::vector<Slot> &parkingSlots,
                                      u_int32_t selectedParkingSlotId) {
    __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "开始寻路到停车位ID: %d", selectedParkingSlotId);

    // 获取选中的停车位
    const auto& slot = parkingSlots[selectedParkingSlotId - 1];

    // 计算停车位中心点坐标
    double slotCenterPointX = (slot.parkingPoint[0].x + slot.parkingPoint[1].x +
                               slot.parkingPoint[2].x + slot.parkingPoint[3].x) / 4.0;
    double slotCenterPointY = (slot.parkingPoint[0].y + slot.parkingPoint[1].y +
                               slot.parkingPoint[2].y + slot.parkingPoint[3].y) / 4.0;

    // 创建停车位中心点和车辆当前位置的向量
    glm::vec3 slotCenterPoint(slotCenterPointX, slotCenterPointY, 0.f);
    glm::vec3 carPos(carPosVec3.x, carPosVec3.y, carPosVec3.z);

    // 设置第二阶段目标位置为停车位中心
    secondPhaseTargetPos = slotCenterPoint;

    // 第一阶段目标：先水平移动到停车位的X坐标位置（保持Y坐标不变）
    glm::vec3 firstPhaseTarget(slotCenterPoint.x, carPos.y, carPos.z);

    // 设置需要进行第二阶段移动（倒车入库）
    needSecondPhaseMove = true;

    // 开始第一阶段移动，带有X轴偏移
    __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "开始第一阶段移动，目标: x=%f, y=%f",
                        firstPhaseTarget.x, firstPhaseTarget.y);
    MoveToPosition(carPos, firstPhaseTarget, 0.03f, 2.f, 0.f);
}

/**
 * @brief 移动车辆到指定位置
 * @param carPosVec3 当前车辆位置
 * @param PositionVec3 目标位置
 * @param moveSpeed 移动速度
 * @param xOffset X轴偏移量
 * @param yOffset Y轴偏移量
 */
void RenderContext3D::MoveToPosition(glm::vec3 carPosVec3, glm::vec3 PositionVec3, float moveSpeed, float xOffset, float yOffset) {
    __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "移动到位置: 目标(x=%f, y=%f), 偏移(x=%f, y=%f), 速度=%f",
                        PositionVec3.x, PositionVec3.y, xOffset, yOffset, moveSpeed);

    // 设置目标位置（加上偏移量）
    targetPositionX = PositionVec3.x + xOffset;
    targetPositionY = PositionVec3.y + yOffset;

    // 设置移动速度
    RenderContext3D::moveSpeed = moveSpeed;

    // 更新当前车辆位置
    this->carPosVec3 = glm::dvec3(carPosVec3.x, carPosVec3.y, carPosVec3.z);

    // 开始移动
    isMoving = true;
}

/**
 * @brief 更新车辆移动状态
 * @return 是否正在移动
 */
bool RenderContext3D::UpdateMovement() {
    // 如果不在移动状态，直接返回
    if (!isMoving) {
        return false;
    }

    // 计算到目标位置的距离
    float distanceToTargetX = targetPositionX - carPosVec3.x;
    float distanceToTargetY = targetPositionY - carPosVec3.y;
    float absDistanceX = abs(distanceToTargetX);
    float absDistanceY = abs(distanceToTargetY);

    // 位置阈值，小于此值认为已到达目标
    const float POSITION_THRESHOLD = 0.001f;

    // 如果已经非常接近目标位置，认为第一阶段移动完成
    if (absDistanceX < POSITION_THRESHOLD && absDistanceY < POSITION_THRESHOLD) {
        isMoving = false;

        // 如果需要进行第二阶段移动（倒车入库）
        if (needSecondPhaseMove) {
            __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "第一阶段移动完成，开始第二阶段移动");
            needSecondPhaseMove = false;
            glm::vec3 currentPos(carPosVec3.x, carPosVec3.y, carPosVec3.z);

            // 开始第二阶段移动，目标是停车位中心，带有偏移
            MoveToPosition(currentPos, secondPhaseTargetPos, moveSpeed, 1.5f, 1.5f);

            // 计算初始距离，用于后续计算移动进度
            initialDistanceX = secondPhaseTargetPos.x + 1.5f - currentPos.x;
            initialDistanceY = secondPhaseTargetPos.y + 1.5f - currentPos.y;
            totalInitialDistance = sqrt(initialDistanceX * initialDistanceX + initialDistanceY * initialDistanceY);

            // 设置为第二阶段移动
            isSecondPhase = true;
            isSecondPhaseFirstFrame = true;

            // 如果当前旋转角度接近0，使用默认旋转角度
            if (fabs(currentRotationAngle) < 0.001f) {
                currentRotationAngle = defaultRotationAngle;
            }

            // 记录初始旋转角度
            initialRotationAngle = currentRotationAngle;
        }

        return false;
    }

    // 计算总距离和移动比例
    float totalDistance = sqrt(absDistanceX * absDistanceX + absDistanceY * absDistanceY);
    float moveRatio = moveSpeed / totalDistance;

    // 计算X和Y方向的移动量
    float moveX = distanceToTargetX * moveRatio;
    float moveY = distanceToTargetY * moveRatio;

    // 如果是第二阶段移动（倒车入库），使用曲线轨迹
    if (!needSecondPhaseMove && isSecondPhase) {
        // 计算当前移动进度
        float currentTotalDistance = sqrt(absDistanceX * absDistanceX + absDistanceY * absDistanceY);
        float progress = 1.0f - (currentTotalDistance / totalInitialDistance);

        // 确保进度在0-1范围内
        progress = std::max(0.0f, std::min(1.0f, progress));

        // 使用二次函数调整X和Y方向的移动因子，实现曲线轨迹
        // X方向：开始快，结束慢
        float xFactor = 0.95f - 0.9f * progress * progress;
        // Y方向：开始慢，结束快
        float yFactor = 0.15f + 0.9f * progress * progress;

        // 应用移动因子
        moveX = distanceToTargetX * moveRatio * xFactor;
        moveY = distanceToTargetY * moveRatio * yFactor;
    }

    // 确保移动量不超过剩余距离（X方向）
    if (abs(moveX) > absDistanceX) {
        float adjustRatio = absDistanceX / abs(moveX);
        moveX = distanceToTargetX * adjustRatio;
        moveY = distanceToTargetY * adjustRatio;
    }

    // 确保移动量不超过剩余距离（Y方向）
    if (abs(moveY) > absDistanceY) {
        float adjustRatio = absDistanceY / abs(moveY);
        moveX = distanceToTargetX * adjustRatio;
        moveY = distanceToTargetY * adjustRatio;
    }

    // 更新X坐标
    if (absDistanceX > POSITION_THRESHOLD) {
        carPosVec3.x += moveX;
        slotOffset += moveX;  // 同步更新停车位偏移
    }

    // 更新Y坐标
    if (absDistanceY > POSITION_THRESHOLD) {
        carPosVec3.y += moveY;
        slotOffset1 += moveY;  // 同步更新停车位偏移
    }

    // 重新计算到目标的距离
    absDistanceX = abs(targetPositionX - carPosVec3.x);
    absDistanceY = abs(targetPositionY - carPosVec3.y);

    // 如果已到达目标位置
    if (absDistanceX <= POSITION_THRESHOLD && absDistanceY <= POSITION_THRESHOLD) {
        // 精确设置到目标位置
        carPosVec3.x = targetPositionX;
        carPosVec3.y = targetPositionY;

        // 如果需要进行第二阶段移动
        if (needSecondPhaseMove) {
            __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "第一阶段移动完成，开始第二阶段移动");
            needSecondPhaseMove = false;
            glm::vec3 currentPos(carPosVec3.x, carPosVec3.y, carPosVec3.z);

            // 根据停车位位置决定Y轴偏移方向
            if (secondPhaseTargetPos.y > currentPos.y) {
                // 停车位在上方，向上偏移
                MoveToPosition(currentPos, secondPhaseTargetPos, moveSpeed, 0.f, 1.5f);
                initialDistanceX = secondPhaseTargetPos.x - currentPos.x;
                initialDistanceY = secondPhaseTargetPos.y + 1.5f - currentPos.y;
                totalInitialDistance = sqrt(initialDistanceX * initialDistanceX + initialDistanceY * initialDistanceY);

                isSecondPhase = true;
                isSecondPhaseFirstFrame = true;
            } else {
                // 停车位在下方，向下偏移
                MoveToPosition(currentPos, secondPhaseTargetPos, moveSpeed, 0.f, -1.5f);
                initialDistanceX = secondPhaseTargetPos.x - currentPos.x;
                initialDistanceY = secondPhaseTargetPos.y - 1.5f - currentPos.y;
                totalInitialDistance = sqrt(initialDistanceX * initialDistanceX + initialDistanceY * initialDistanceY);

                isSecondPhase = true;
                isSecondPhaseFirstFrame = true;
            }
        } else {
            // 移动完成
            isMoving = false;

            // 如果是第二阶段移动完成，设置最终旋转角度
            if (isSecondPhase) {
                __android_log_print(ANDROID_LOG_INFO, "RenderContext3D", "第二阶段移动完成，设置最终旋转角度");
                if (carPosVec3.y > targetPositionY) {
                    // 车辆在目标上方
                    currentRotationAngle = finalRotationAngle;
                    ApplyRotation(-currentRotationAngle, glm::vec3(0.f, 1.0f, 0.f));
                } else {
                    // 车辆在目标下方
                    currentRotationAngle = -finalRotationAngle;
                    ApplyRotation(currentRotationAngle, glm::vec3(0.f, 1.0f, 0.f));
                }
            }

            // 重置第二阶段标志
            isSecondPhase = false;
        }
    }
    return isMoving;
}