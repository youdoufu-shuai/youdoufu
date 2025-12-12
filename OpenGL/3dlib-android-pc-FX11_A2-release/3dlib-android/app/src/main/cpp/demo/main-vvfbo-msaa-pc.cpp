```cpp
/**
 * @file main.cpp
 * @brief 3D虚拟视图渲染程序
 * 
 * 该程序模拟了一个停车场的场景，包括车位、障碍物、车辆等元素的渲染。
 * 使用OpenGL进行图形渲染，并支持多采样抗锯齿（MSAA）和超级采样抗锯齿（SSAA）。
 * 
 * @author [Your Name]
 * @version 1.0
 * @date 2023-10-12
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <thread>
#include <chrono>
#include <signal.h>

#include <tool/GLHelper.hpp>
#include <tool/ImageHelper.hpp>

#include <VirtualView3D.hpp>
#include <WindowRendering.hpp>

/**
 * @def VIEWPORT_TUPLE_CAR_MODEL
 * @brief 车辆模型视口参数
 * 
 * 定义车辆模型的视口位置和大小。
 */
#define VIEWPORT_TUPLE_CAR_MODEL    0, 0, 520, 720

/**
 * @def VIEWPORT_TUPLE_WHOLE_SCREEN
 * @brief 整个屏幕视口参数
 * 
 * 定义整个屏幕的视口位置和大小。
 */
#define VIEWPORT_TUPLE_WHOLE_SCREEN 0, 0, 1800, 720

/**
 * @def SSAA_SCALE
 * @brief 超级采样抗锯齿缩放因子
 * 
 * 定义超级采样抗锯齿的缩放因子。
 */
#define SSAA_SCALE 2

/**
 * @def MSAA_MAX
 * @brief 多采样抗锯齿最大值
 * 
 * 定义多采样抗锯齿的最大值。
 */
#define MSAA_MAX   4

/**
 * @def VV_AA_PARKING_DISPLAY
 * @brief 停车场显示功能宏
 * 
 * 控制是否启用停车场显示功能。
 */
#define VV_AA_PARKING_DISPLAY

/**
 * @def VV_AA_RADAR_WATER_RIPPLE
 * @brief 雷达水波纹功能宏
 * 
 * 控制是否启用雷达水波纹功能。
 */
#define VV_AA_RADAR_WATER_RIPPLE

/**
 * @def VV_AA_MOVE_DIRECTION_ARROW
 * @brief 移动方向箭头功能宏
 * 
 * 控制是否启用移动方向箭头功能。
 */
#define VV_AA_MOVE_DIRECTION_ARROW

/**
 * @def VV_AA_CAR_EXIT_DIRECTION
 * @brief 车辆退出方向功能宏
 * 
 * 控制是否启用车辆退出方向功能。
 */
#define VV_AA_CAR_EXIT_DIRECTION

/**
 * @def VV_AA_PARKING_PROGRESS_BAR
 * @brief 停车进度条功能宏
 * 
 * 控制是否启用停车进度条功能。
 */
#define VV_AA_PARKING_PROGRESS_BAR

/**
 * @def VV_AA_PARKING_PROGRESS
 * @brief 停车进度功能宏
 * 
 * 控制是否启用停车进度功能。
 */
#define VV_AA_PARKING_PROGRESS

/**
 * @def VV_AA_RADAR_ALARM_DISTANCE
 * @brief 雷达报警距离功能宏
 * 
 * 控制是否启用雷达报警距离功能。
 */
#define VV_AA_RADAR_ALARM_DISTANCE

/**
 * @def VV_AA_CAR_3D_MODEL_DISPLAY
 * @brief 3D车辆模型显示功能宏
 * 
 * 控制是否启用3D车辆模型显示功能。
 */
#define VV_AA_CAR_3D_MODEL_DISPLAY

/**
 * @def VV_AA_OBSTACLE_DISPLAY
 * @brief 障碍物显示功能宏
 * 
 * 控制是否启用障碍物显示功能。
 */
#define VV_AA_OBSTACLE_DISPLAY

/**
 * @def VV_AA_RADAR_ALARM_DISPLAY
 * @brief 雷达报警显示功能宏
 * 
 * 控制是否启用雷达报警显示功能。
 */
#define VV_AA_RADAR_ALARM_DISPLAY

/**
 * @brief 生成示例车位数据
 * 
 * 生成一组示例车位数据，包括车位ID、显示编号、显示内容、车位类型、车位空间类型和槽位类型。
 * 
 * @return std::vector<VirtualView3D::Slot> 示例车位数据
 */
static std::vector<VirtualView3D::Slot> MakeParkingSlotsSampleData();

/**
 * @brief 生成示例障碍物数据
 * 
 * 生成一组示例障碍物数据，包括障碍物ID、类型、坐标和状态。
 * 
 * @return std::vector<VirtualView3D::Obstacle> 示例障碍物数据
 */
static std::vector<VirtualView3D::Obstacle> MakeObstacleSampleData();

/**
 * @brief 生成示例PDC信息数据
 * 
 * 生成一组示例PDC（停车距离控制）信息数据，包括类型和级别。
 * 
 * @return std::vector<VirtualView3D::PdcInfo> 示例PDC信息数据
 */
static std::vector<VirtualView3D::PdcInfo> MakePdcInfoSampleData();

/**
 * @brief 生成示例PDC距离数据
 * 
 * 生成一组示例PDC距离数据，包括位置和距离值。
 * 
 * @return std::vector<VirtualView3D::PdcDistance> 示例PDC距离数据
 */
static std::vector<VirtualView3D::PdcDistance> MakePdcDistance();

/**
 * @brief 根据车位数据生成障碍物数据
 * 
 * 根据车位数据生成障碍物数据，每个障碍物的位置是对应车位的中心位置，角度是车位的方向。
 * 
 * @param slots 车位数据
 * @param yaw 车位方向
 * @return std::vector<VirtualView3D::Obstacle> 障碍物数据
 */
static std::vector<VirtualView3D::Obstacle> MakeObstacleDataByPark(std::vector<VirtualView3D::Slot> &slots, float yaw); 

/**
 * @brief 程序运行标志
 * 
 * 控制程序的运行状态，当设置为false时程序将退出。
 */
static bool isRunning = true;

/**
 * @brief 信号处理函数
 * 
 * 处理程序的终止信号，设置isRunning标志为false。
 * 
 * @param signal 信号值
 */
void signalStop(int signal)
{
	LOGGER_I("signalStop: signal=%d", signal);
	isRunning = false;
}

/**
 * @brief 注册信号处理函数
 * 
 * 注册SIGINT和SIGTERM信号的处理函数。
 */
void registerStopSignal()
{
    signal(SIGINT, signalStop);
    signal(SIGTERM, signalStop);
}

using namespace VirtualView3D;
using namespace std;

/**
 * @brief 主函数
 * 
 * 程序的入口函数，初始化窗口、帧缓冲、着色器等资源，并进入主循环。
 * 
 * @param argc 参数个数
 * @param argv 参数列表
 * @return int 程序返回值
 */
int main(int argc, char **argv)
{
	registerStopSignal(); // 注册信号处理函数

	// 扩展参数列表，设置窗口大小
	int     argcExt  = 3;
	char  *argvExt[] = {
		argv[0],
		(char *)"1800",
		(char *)"720"
	};

	VirtualView3D::WindowRendering windowRendering;

	// 设置窗口标题
	windowRendering.windowTitle = "virtualview window";
    // 初始化窗口
	windowRendering.WindowInitialization(argcExt, argvExt);

	// 创建帧缓冲对象
	BaseLib::Framebuffer fboMSAA;
	BaseLib::Framebuffer fboSSAA;

	// 初始化多采样抗锯齿帧缓冲
	fboMSAA.id          = GL_NONE;
	fboMSAA.colorFormat = GL_RGBA8;
	fboMSAA.depthFormat = GL_DEPTH24_STENCIL8;
	fboMSAA.msaaMax     = MSAA_MAX;
	glm::ivec4 tmpViewportVec4(VIEWPORT_TUPLE_CAR_MODEL);
	fboMSAA.size = glm::ivec2(tmpViewportVec4.z*SSAA_SCALE, tmpViewportVec4.w*SSAA_SCALE);
	if (fboMSAA.msaaMax > 0)
	{
		BaseLib::GLHelper::CreateFramebuffer(fboMSAA);
	}
	
	// 初始化超级采样抗锯齿帧缓冲
	fboSSAA.id          = GL_NONE;
	fboSSAA.colorFormat = GL_RGBA8;
	fboSSAA.depthFormat = GL_DEPTH24_STENCIL8;
	fboSSAA.size = glm::ivec2(tmpViewportVec4.z*SSAA_SCALE, tmpViewportVec4.w*SSAA_SCALE);
	BaseLib::GLHelper::CreateFramebuffer(fboSSAA);
	LOGGER_GLE("BaseLib::GLHelper::CreateFramebuffer(fboSSAA)");

	// 加载着色器
	BaseLib::Shader simpleShader;
	simpleShader.FromSourceFile("resource/junlian/virtualview/shader/simple.vs", "resource/junlian/virtualview/shader/simple.fs");

	// 创建顶点数组对象、顶点缓冲对象和元素缓冲对象
	unsigned int VBO, VAO, EBO;
	GLHelper::GenRectangleVertexArray(glm::ivec4(VIEWPORT_TUPLE_CAR_MODEL), glm::ivec4(VIEWPORT_TUPLE_CAR_MODEL), false, VAO, VBO , EBO);

	// 初始化虚拟视图
	VirtualView virtualView;
	virtualView.Initialization();
	virtualView.visualAngleControl.SetViewport(VIEWPORT_TUPLE_CAR_MODEL);

	// 初始化车辆门状态
	CarDoorType carDoorType;

	// 初始化变量
	int mouseEventCounterPre   = windowRendering.mouseEventCounter;
	int toggleState            = 0;
	int nframes                = 0;
	uint8_t switchSts          = 0;
	bool searchParkingWave     = true;
	CarMotionStatu carWheelSts = CarMotionStatu::STOP;
	uint8_t progressPercentage = 0;
	GearPosition gear          = GearPosition::GEAR_R;
	float parkingCompletionAngle = 0.f;
	ParkoutType parkoutType      = ParkoutType::LEFT;
	VirtualView3D::ApaStatus apaStatus  = {.remainDistance = 0.f};
	bool isVehicleExitDoRender   = false;
	bool isVirtualParkStatic     = true;
	glm::dvec3 carPosVec3        = glm::dvec3(0.0, 0.0, 0.0);

	uint32_t frameCount    = 0;
	timeval timeCurrent;
	long usecEnd           = 0;
	long usecStart         = 0;
	gettimeofday(&timeCurrent, NULL);
	usecStart = (timeCurrent.tv_sec * 1000000) + timeCurrent.tv_usec;

	std::chrono::time_point<std::chrono::steady_clock> now;
	while(isRunning)
	{
		now = std::chrono::steady_clock::now();

		// 定期更新车辆状态
		if (nframes % 500 == 0)
		{
			switch (switchSts)
			{
				case 1:
					switchSts = 2;
					break;
				case 0:
				case 2:
				default:
					switchSts = 1;
					break;
			}

			switch(carWheelSts)
			{
				case CarMotionStatu::STOP:
					carWheelSts = CarMotionStatu::FORWARD;
					break;
				case CarMotionStatu::FORWARD:
					carWheelSts = CarMotionStatu::BACKWARD;
					break;
				case CarMotionStatu::BACKWARD:
				default:
					carWheelSts = CarMotionStatu::STOP;
					break;
			}

			if(gear==GearPosition::GEAR_D)gear=GearPosition::GEAR_R;
			else gear=GearPosition::GEAR_D;
			
		}

		// 定期更新停车进度
		if (nframes % 20 == 0)
		{
			switch (progressPercentage)
			{
				case 0:
					progressPercentage  = 1;
					break;
				case 101:
					progressPercentage  = 0;
					break;
				default:
					progressPercentage += 1; 
					break;
			}

		}

		// 更新停车完成角度和车辆退出状态
		if (nframes > 800 && nframes < 1200)
		{
			parkingCompletionAngle = 90.f;
		} 
		else if (nframes > 1200 && nframes < 1800)
		{
			isVehicleExitDoRender  = true;
			parkingCompletionAngle = -90.f;
		}
		else
		{
			parkingCompletionAngle = 0.f;
			isVehicleExitDoRender  = false;
		}

		// 更新车辆位置
		if (nframes < 2000)
		{
			isVirtualParkStatic = true;
			carPosVec3 = glm::dvec3(0.0, 0.0, 0.0);
		}
		else if (nframes < 5000)
		{
			isVirtualParkStatic = false;
			carPosVec3 = glm::dvec3(nframes*0.001, 0.0, 0.0);
		}
		else
		{
			isVirtualParkStatic = true;
			carPosVec3 = glm::dvec3(0.0, 0.0, 0.0);
		}

		// 更新停车出口类型
		if (parkoutType == ParkoutType::LEFT) parkoutType = ParkoutType::RIGHT;
		else parkoutType = ParkoutType::LEFT;

		// 更新车辆门状态
		switchSts = 0x0;
		carDoorType.doorDrvrSts = switchSts;
		carDoorType.doorLeReSts = switchSts;
		carDoorType.doorPassSts = switchSts;
		carDoorType.doorRiReSts = switchSts;
		carDoorType.hoodSts     = switchSts;
		carDoorType.trSts       = switchSts;

		// 更新APA状态
		apaStatus.remainDistance += 0.05f;
		if (apaStatus.remainDistance >=20.f) {
			apaStatus.remainDistance = 0.f;
		}

		// 生成障碍物数据
		std::vector<Obstacle>  obstacles;
		std::vector<Slot> parkingSlots;
		if (nframes % 80 == 0)
		{
			if (toggleState == 0) 
			{
				toggleState = 1;
			}
			else
			{
				toggleState = 0;
			}
		}
		
		parkingSlots = MakeParkingSlotsSampleData();
		if (toggleState == 1)
		{
			obstacles = MakeObstacleSampleData();
		}
		else
		{
			obstacles = MakeObstacleDataByPark(parkingSlots, glm::radians(windowRendering.camTheta));
		}

		// 生成PDC信息和距离数据
		std::vector<PdcInfo> pdcInfo = MakePdcInfoSampleData();
		std::vector<VirtualView3D::PdcDistance> pdcDistance = MakePdcDistance();

		// 绑定帧缓冲对象
		if (fboMSAA.id > 0 && fboSSAA.id > 0)
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboMSAA.id);
			glClearColor(0.92941176f, 0.94117647f, 0.96078431f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glViewport(0, 0, fboMSAA.size.x, fboMSAA.size.y);
		}
		else if (fboSSAA.id > 0)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, fboSSAA.id);
			glClearColor(0.92941176f, 0.94117647f, 0.96078431f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glViewport(0, 0, fboSSAA.size.x, fboSSAA.size.y);
		}
		else 
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(0.92941176f, 0.94117647f, 0.96078431f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		// 渲染停车场显示
	#ifdef VV_AA_PARKING_DISPLAY
		virtualView.parkingDisplay.DoRender(virtualView.visualAngleControl, parkingSlots);
		virtualView.parkingDisplay.DoRenderVirtual(virtualView.visualAngleControl, carPosVec3, isVirtualParkStatic);
	#endif

		// 渲染雷达水波纹
	#ifdef VV_AA_RADAR_WATER_RIPPLE
		virtualView.radarWaterRipple.DoRender(virtualView.visualAngleControl, searchParkingWave);
	#endif

		// 渲染移动方向箭头
	#ifdef VV_AA_MOVE_DIRECTION_ARROW
		virtualView.vehicleMoveDirectionArrow.DoRender(virtualView.visualAngleControl, MotionDir::MOTION_FORWARD);
	#endif

		// 渲染车辆退出方向
	#ifdef VV_AA_CAR_EXIT_DIRECTION
		if (isVehicleExitDoRender) 
		{
			virtualView.carExitDirection.DoRender(virtualView.visualAngleControl, parkoutType);
		}
	#endif

		// 渲染停车进度条
	#ifdef VV_AA_PARKING_PROGRESS_BAR
		virtualView.parkingProgressBar.DoRender(virtualView.visualAngleControl, progressPercentage, gear);
	#endif

		// 渲染停车进度
	#ifdef VV_AA_PARKING_PROGRESS
		virtualView.parkingProgress.DoRender(virtualView.visualAngleControl, apaStatus);
	#endif

		// 渲染雷达报警距离
	#ifdef VV_AA_RADAR_ALARM_DISTANCE
		virtualView.radarAlarmDistance.DoRender(virtualView.visualAngleControl, pdcDistance);
	#endif
	
		// 渲染3D车辆模型
	#ifdef VV_AA_CAR_3D_MODEL_DISPLAY
		virtualView.car3DModelDisplay.DoRender(virtualView.visualAngleControl, carDoorType, carWheelSts);
	#endif

		// 渲染障碍物
	#ifdef VV_AA_OBSTACLE_DISPLAY
		virtualView.obstacleDisplay.DoRender(virtualView.visualAngleControl, obstacles);
	#endif

		// 渲染雷达报警
	#ifdef VV_AA_RADAR_ALARM_DISPLAY
		virtualView.radarAlarmDisplay.DoRender(virtualView.visualAngleControl, pdcInfo);
	#endif
		
		// 帧缓冲对象之间的数据传输
		if (fboMSAA.id > 0 && fboSSAA.id > 0)
		{
			static GLenum depthAttachments[1] = {GL_DEPTH_ATTACHMENT};
			static GLenum colorAttachments[1] = {GL_COLOR_ATTACHMENT0};
			glBindFramebuffer(GL_READ_FRAMEBUFFER, fboMSAA.id);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboSSAA.id);
			glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, 1, depthAttachments);
			glBlitFramebuffer(0, 0, (GLuint)fboMSAA.size.x, (GLuint)fboMSAA.size.y,
							  0, 0, (GLuint)fboSSAA.size.x, (GLuint)fboSSAA.size.y,
							  GL_COLOR_BUFFER_BIT,
							  GL_LINEAR
							  );
			glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, 1, colorAttachments);
		}

		// 生成纹理的mipmap
		if (fboSSAA.colorTextId > 0)
		{
			glBindTexture(GL_TEXTURE_2D, fboSSAA.colorTextId);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// 绑定默认帧缓冲对象
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.92941176f, 0.94117647f, 0.96078431f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// 禁用深度测试，启用混合
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glViewport(VIEWPORT_TUPLE_CAR_MODEL);

		// 使用着色器程序
        simpleShader.UseProgram();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fboSSAA.colorTextId);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_BLEND);

		// 如果未定义停车场显示功能宏，则渲染停车场显示
	#ifndef VV_AA_PARKING_DISPLAY
		virtualView.parkingDisplay.DoRender(virtualView.visualAngleControl, parkingSlots);
		virtualView.parkingDisplay.DoRenderVirtual(virtualView.visualAngleControl, carPosVec3, isVirtualParkStatic);
	#endif

		// 如果未定义雷达水波纹功能宏，则渲染雷达水波纹
	#ifndef VV_AA_RADAR_WATER_RIPPLE
		virtualView.radarWaterRipple.DoRender(virtualView.visualAngleControl, searchParkingWave);
	#endif

		// 如果未定义移动方向箭头功能宏，则渲染移动方向箭头
	#ifndef VV_AA_MOVE_DIRECTION_ARROW
		virtualView.vehicleMoveDirectionArrow.DoRender(virtualView.visualAngleControl, MotionDir::MOTION_FORWARD);
	#endif

		// 如果未定义车辆退出方向功能宏，则渲染车辆退出方向
	#ifndef VV_AA_CAR_EXIT_DIRECTION
		if (isVehicleExitDoRender) 
		{
			virtualView.carExitDirection.DoRender(virtualView.visualAngleControl, parkoutType);
		}
	#endif

		// 如果未定义停车进度条功能宏，则渲染停车进度条
	#ifndef VV_AA_PARKING_PROGRESS_BAR
		virtualView.parkingProgressBar.DoRender(virtualView.visualAngleControl, progressPercentage, gear);
	#endif

		// 如果未定义停车进度功能宏，则渲染停车进度
	#ifndef VV_AA_PARKING_PROGRESS
		virtualView.parkingProgress.DoRender(virtualView.visualAngleControl, apaStatus);
	#endif

		// 如果未定义雷达报警距离功能宏，则渲染雷达报警距离
	#ifndef VV_AA_RADAR_ALARM_DISTANCE
		virtualView.radarAlarmDistance.DoRender(virtualView.visualAngleControl, pdcDistance);
	#endif
	
		// 如果未定义3D车辆模型显示功能宏，则渲染3D车辆模型
	#ifndef VV_AA_CAR_3D_MODEL_DISPLAY
		virtualView.car3DModelDisplay.DoRender(virtualView.visualAngleControl, carDoorType, carWheelSts);
	#endif

		// 如果未定义障碍物显示功能宏，则渲染障碍物
	#ifndef VV_AA_OBSTACLE_DISPLAY
		virtualView.obstacleDisplay.DoRender(virtualView.visualAngleControl, obstacles);
	#endif

		// 如果未定义雷达报警显示功能宏，则渲染雷达报警
	#ifndef VV_AA_RADAR_ALARM_DISPLAY
		virtualView.radarAlarmDisplay.DoRender(virtualView.visualAngleControl, pdcInfo);
	#endif
		
		// 处理鼠标事件
		if (windowRendering.mouseEventCounter != mouseEventCounterPre)
		{
			VirtualView3D::TouchPoint touchPoint = {.pixelX = (float)windowRendering.mouseX, .pixelY = (float)windowRendering.mouseY};
			int parkingSlotId = virtualView.parkingDisplay.Touch(touchPoint);
			virtualView.parkingDisplay.SelectedSlot(parkingSlotId);

			mouseEventCounterPre = windowRendering.mouseEventCounter;
		}

		// 显示窗口
		windowRendering.WindowDisplay();
		
		nframes++;

		frameCount++;
		gettimeofday(&timeCurrent, NULL);
		usecEnd = (timeCurrent.tv_sec * 1000000) + timeCurrent.tv_usec;
		long duration = (usecEnd - usecStart);
		if (duration >= 1000000)
		{
			double fps = frameCount * 1000000.0f / duration;
			LOGGER_I("FPS=%lf frameCount=%u usecEnd=%ld(us) usecStart=%ld(us) duration=%ld(us)", fps, frameCount, usecEnd, usecStart, duration);
			frameCount = 0;
			usecStart = usecEnd;
		}

		// 限制帧率
		std::this_thread::sleep_until(now + std::chrono::milliseconds(50));

	}

	LOGGER_I("done");
	return 0;
}

/**
 * @brief 生成示例车位数据
 * 
 * 生成一组示例车位数据，包括车位ID、显示编号、显示内容、车位类型、车位空间类型和槽位类型。
 * 
 * @return std::vector<VirtualView3D::Slot> 示例车位数据
 */
static std::vector<VirtualView3D::Slot> MakeParkingSlotsSampleData()
{
	std::vector<Slot> parkingSlots;
	Slot parkSlot;

	const float COMMON_PARK_WIDTH_HALF  = 1.25f; // 车位宽度的一半
	const float COMMON_PARK_LENGTH_HALF = 2.5f;  // 车位长度的一半

	float parkCenterX = 0.f; // 车位中心X坐标
	float parkCenterY = 0.f; // 车位中心Y坐标
	float parkCenterZ = 0.f; // 车位中心Z坐标

	// 生成多个车位数据
	parkCenterX =  0.f;
	parkCenterY = -4.5f;
	parkCenterZ =  0.f;

	parkSlot.parkingID                = 2; // 车位ID
	parkSlot.displayNum               = 1; // 显示编号
	parkSlot.parkingSpaceShowContents = ParkingSpaceShowContents::SHOW_NUMBERS; // 显示内容
	parkSlot.parkingType              = ParkingType::PARKING_IS_POSSIBLE; // 车位类型
	parkSlot.parkingSpaceType         = ParkingSpaceType::NORMAL_SPACE; // 车位空间类型
	parkSlot.slotType                 = SlotType::SLOT_RIGHT_VERT; // 槽位类型
	parkSlot.parkingPoint[0] = {.x= parkCenterX-COMMON_PARK_WIDTH_HALF, .y=parkCenterY+COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
	parkSlot.parkingPoint[1] = {.x= parkCenterX+COMMON_PARK_WIDTH_HALF, .y=parkCenterY+COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
	parkSlot.parkingPoint[2] = {.x= parkCenterX-COMMON_PARK_WIDTH_HALF, .y=parkCenterY-COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
	parkSlot.parkingPoint[3] = {.x= parkCenterX+COMMON_PARK_WIDTH_HALF, .y=parkCenterY-COMMON_PARK_LENGTH_HALF, .z=parkCenterZ};
	parkingSlots.push_back(parkSlot);

	// 生成其他车位数据...
	
	return parkingSlots;
}

/**
 * @brief 生成示例障碍物数据
 * 
 * 生成一组示例障碍物数据，包括障碍物ID、类型、坐标和状态。
 * 
 * @return std::vector<VirtualView3D::Obstacle> 示例障碍物数据
 */
static std::vector<VirtualView3D::Obstacle> MakeObstacleSampleData()
{
	std::vector<Obstacle>  obstacles;

	Obstacle obstacle;
	obstacle.id = 1;
	obstacle.type = ObstacleType::OBSTACLE_TYPE_CAR;
	obstacle.coordinate = {.x=9.f, .y=4.5f, .z=0.f, .angle=glm::radians(90.f)};
	obstacle.statu = ObstacleMotionStatu::STOP;
	obstacles.push_back(obstacle);

	// 生成其他障碍物数据...

	return obstacles;
}

/**
 * @brief 生成示例PDC信息数据
 * 
 * 生成一组示例PDC（停车距离控制）信息数据，包括类型和级别。
 * 
 * @return std::vector<VirtualView3D::PdcInfo> 示例PDC信息数据
 */
static std::vector<VirtualView3D::PdcInfo> MakePdcInfoSampleData()
{
	std::vector<VirtualView3D::PdcInfo> pdcInfoVec;
	PdcInfo pdcInfo;

	pdcInfo.type  = AreaType::LFM;
	pdcInfo.level = LevelType::LEVEL_1;
	pdcInfoVec.push_back(pdcInfo);

	// 生成其他PDC信息数据...

	return pdcInfoVec;
}

/**
 * @brief 生成示例PDC距离数据
 * 
 * 生成一组示例PDC距离数据，包括位置和距离值。
 * 
 * @return std::vector<VirtualView3D::PdcDistance> 示例PDC距离数据
 */
static std::vector<VirtualView3D::PdcDistance> MakePdcDistance()
{
	std::vector<VirtualView3D::PdcDistance> pdcDistanceVector;

	std::srand(std::time(nullptr));
    uint16_t distance = std::rand() % 998;

	PdcDistance pdcDist;
	pdcDist.position = PdcDistancePosition::FRONT;
	pdcDist.distance = distance;
	pdcDistanceVector.push_back(pdcDist);

	pdcDist.position = PdcDistancePosition::REAR;
	pdcDist.distance = distance+1;
	pdcDistanceVector.push_back(pdcDist);

	return pdcDistanceVector;
}

/**
 * @brief 根据车位数据生成障碍物数据
 * 
 * 根据车位数据生成障碍物数据，每个障碍物的位置是对应车位的中心位置，角度是车位的方向。
 * 
 * @param slots 车位数据
 * @param yaw 车位方向
 * @return std::vector<VirtualView3D::Obstacle> 障碍物数据
 */
static std::vector<VirtualView3D::Obstacle> MakeObstacleDataByPark(std::vector<VirtualView3D::Slot> &slots, float yaw) {

	std::vector<Obstacle>  obstacles;

	for (int i=0; i<slots.size(); i++) {
		if(i==6) continue;
		VirtualView3D::Slot& slot = slots[i];
		float centerX = 0.f, centerY = 0.f, centerZ = 0.f;

		// 计算车位中心坐标
		for (int j = 0; j < 4; j++)
		{
			centerX += slot.parkingPoint[j].x;
			centerY += slot.parkingPoint[j].y;
			centerZ += slot.parkingPoint[j].z;

			if (isnan(slot.parkingPoint[j].x)) {
				LOGGER_D("not a number");
			}
		}

		centerX = centerX/4;
		centerY = centerY/4;
		centerZ = centerZ/4;

		// 计算车位方向
		float parkYaw = 0.f;
		glm::vec2 vecDirCA = glm::vec2(slot.parkingPoint[0].x - slot.parkingPoint[2].x, slot.parkingPoint[0].y - slot.parkingPoint[2].y);
		glm::vec2 vecDirAB = glm::vec2(slot.parkingPoint[1].x - slot.parkingPoint[0].x, slot.parkingPoint[1].y - slot.parkingPoint[0].y);

		glm::vec2 vecDir = vecDirCA;

		if (glm::length(vecDirAB) > glm::length(vecDirCA)) vecDir = vecDirAB;

		if (fabs(vecDir.y) < 1e-6)
		{
			parkYaw = M_PI_2;
		}
		else
		{
			parkYaw = atan2(vecDir.y, vecDir.x);
		}

		// 创建障碍物
		Obstacle obstacle;
		obstacle.id = i;
		obstacle.type = ObstacleType::OBSTACLE_TYPE_CAR;
		obstacle.coordinate = {.x=centerX, .y=centerY, .z=centerZ, .angle=parkYaw};
		obstacle.statu = ObstacleMotionStatu::STOP;
		obstacles.push_back(obstacle);
	}

	return obstacles;
}
```