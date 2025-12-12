/**************************************************************************************************
 * @file main.cpp
 * @brief demo for main
 * 
 * @details 
 * 
 * 
 *  HISTORY
 *  -----------------------------------------------------------------------------------------------
 *  Version   Date        Author    Description
 *  -----------------------------------------------------------------------------------------------
 *  0.1       2023-04-18  TDZ       Initial. 
 *  0.2       2023-04-20  TDZ       Referenced freeglut source code.
 *  0.3       2023-04-24  TDZ       Support wheels rotation feature.
 *  0.4       2023-05-12  TDZ       Add demo for TDA4 virtual view offscreen rendering.
 *  0.5       2023-05-16  TDZ       Updateed demo for TDA4 virtual view offscreen rendering 
 *                                  with optional init method.
 *  0.6       2023-05-17  TDZ       Add usage for viewMat4, and spilt the Initialization function. 
 *  0.7       2023-07-30  TDZ       Add SSAA framebuffer for 3D animation DoRender. 
 *  0.8       2024-11-08  TDZ       Add EGLScreen demo for x86_64. 
 * 
 **************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <thread>
#include <chrono>
#include <sys/time.h>
#include <signal.h>

#ifdef HEADER_SPLIT 
	#include <tool/ImageHelper.hpp>
	#include <tool/GLHelper.hpp>
	#include <tool/TimeHelper.hpp>

	#include <CarModelAnimation3D.hpp>
#else
	#include <CarModelAnimation3D.hpp>
#endif // HEADER_SPLIT

#include <EGLScreen.hpp>

#define ENABLE_WINDOW_TEST

#define DRAW_3D_VIEW_CAR_MODEL
// #define RENDER_3D_VIEW_BACKGROUND
// #define SAVE_RENDER_PICTURE


#define WIN_WIDTH  1800
#define WIN_HEIGHT 1024

#define VIEWPORT_TUPLE_CAR_MODEL    0, 0, 1800, 1024
#define VIEWPORT_TUPLE_WHOLE_SCREEN 0, 0, WIN_WIDTH, WIN_HEIGHT

// #define CLEAR_COLOR             1.0f, 1.0f, 1.0f, 0.0f
// #define CLEAR_COLOR_FBO_DEFAULT 1.0f, 1.0f, 1.0f, 1.0f
// #define CLEAR_COLOR             0.5f, 0.5f, 0.5f, 0.0f
// #define CLEAR_COLOR_FBO_DEFAULT 0.5f, 0.5f, 0.5f, 1.0f
// #define CLEAR_COLOR             0.3f, 0.3f, 0.3f, 0.0f
// #define CLEAR_COLOR_FBO_DEFAULT 0.3f, 0.3f, 0.3f, 1.0f
#define CLEAR_COLOR             0.0f, 0.0f, 0.0f, 0.0f
#define CLEAR_COLOR_FBO_DEFAULT 0.0f, 0.0f, 0.0f, 1.0f

#define CAR_MODEL "FX11_A2"
static std::string carModeNo = CAR_MODEL;

/**************************************************************************************************
 * @brief AA
 * 
**************************************************************************************************/
#define SSAA_SCALE 2.f // SSAA scale sqrt(1.5)=1.225f
#define MSAA_MAX   0    // MSAA MAX {0, 2, 4}, 0: No MSAA



using  namespace  std;

static bool isRunning = true;

void signalStop(int signal)
{
	LOGGER_I("signalStop: signal=%d", signal);
	isRunning = false;
}

void registerStopSignal()
{
    signal(SIGINT, signalStop);  // press ctrl c
    signal(SIGTERM, signalStop); // termination
}

bool funcLog(std::string message)
{
	std::cout << "funcLog:" << message << std::endl;

	return true;
}


/**************************************************************************************************
 * @brief AA
 * 
**************************************************************************************************/
BaseLib::Framebuffer fboMSAA;  ///< fbo for MSAA
BaseLib::Framebuffer fboSSAA;  ///< fbo for MSAA resolve


int main(int argc, char **argv)
{
	registerStopSignal();

#ifdef ENABLE_WINDOW_TEST
	int   argcExt   = 3;
	std::string swidth  = (std::to_string(WIN_WIDTH));
	std::string sheight = (std::to_string(WIN_HEIGHT));
	char *argvExt[] = {
		argv[0],
		(char*)swidth.c_str(),
		(char*)sheight.c_str()
	};
	BaseLib::EGLScreen screenRendering;
    screenRendering.Initialization(argcExt, argvExt);   /* mandatory: common init */
#endif // ENABLE_WINDOW_TEST



	/**************************************************************************************************
	 * @brief AA FRAMEBUFFER FOR MSAA
	 * 
	**************************************************************************************************/
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

	/**************************************************************************************************
	 * @brief AA FRAMBUFFER for SSAA
	 * 
	**************************************************************************************************/
	fboSSAA.id          = GL_NONE;
	fboSSAA.colorFormat = GL_RGBA8;
	fboSSAA.depthFormat = GL_DEPTH24_STENCIL8;
	fboSSAA.size = glm::ivec2(tmpViewportVec4.z*SSAA_SCALE, tmpViewportVec4.w*SSAA_SCALE);
	BaseLib::GLHelper::CreateFramebuffer(fboSSAA);
	LOGGER_GLE("BaseLib::GLHelper::CreateFramebuffer(fboSSAA)");

	BaseLib::Shader simpleShader;
	{
		std::string vspath = std::string("./../3dlib-android/app/src/main/assets/avm_resource/image/carmodel3d/junlian/")+carModeNo+ std::string("/animation3d/shader/simple.vs");
		std::string fspath = std::string("./../3dlib-android/app/src/main/assets/avm_resource/image/carmodel3d/junlian/")+carModeNo+ std::string("/animation3d/shader/simple.fs");
		simpleShader.FromSourceFile(vspath.c_str(), fspath.c_str());
	}
	
	/**************************************************************************************************
	 * @brief VAO for car model
	 * 
	**************************************************************************************************/
    unsigned int VBO, VAO, EBO;
	BaseLib::GLHelper::GenRectangleVertexArray(glm::ivec4(VIEWPORT_TUPLE_CAR_MODEL), glm::ivec4(VIEWPORT_TUPLE_CAR_MODEL), false, VAO, VBO , EBO);

	/**************************************************************************************************
	 * @brief VAO for background
	 * 
	**************************************************************************************************/
    unsigned int VBOAvmBackground, VAOAvmBackground, EBOAvmBackground;
	BaseLib::GLHelper::GenRectangleVertexArray(glm::ivec4(VIEWPORT_TUPLE_WHOLE_SCREEN), glm::ivec4(VIEWPORT_TUPLE_WHOLE_SCREEN), true, VAOAvmBackground, VBOAvmBackground , EBOAvmBackground);

	/**************************************************************************************************
	 * @brief BACKGROUND texture
	 * 
	**************************************************************************************************/
	// std::string filename = "AVM-screenshot2.png";
	// std::string directory = "avm_resource/image/carmodel3d/junlian/"CAR_MODEL"/animation3d/models/Geely";
	// BaseLib::Texture textureBg = BaseLib::Model::LoadTexture("texture_diffuse", filename, directory);
	
	/* [3D animation] declare start */
	CarModel3D::CarModel3D  carModel3D;
	CarModel3D::Animation3D animation3D;
	CarModel3D::CameraView  viewParams;
	/* [3D animation] declare done */

	/* [3D animation] init */
	carModel3D.Initialization("./../3dlib-android/app/src/main/assets/avm_resource/image/carmodel3d/junlian/"CAR_MODEL, funcLog);

	LOGGER_GLE("carModel3D.Initialization");

	/* [3D animation] done */
	glm::ivec4 viewportVec4 = glm::ivec4(VIEWPORT_TUPLE_CAR_MODEL);
	float ratio  = (float)viewportVec4.z / viewportVec4.w;

	int nframes                    = 0;
	uint8_t wheelDir               = 0; // stop
	uint8_t doorSwitchSts          = 0;
	uint8_t sunFloorSts            = 0x01;
	uint8_t flWindow               = 0x01;
	uint8_t mirror                 = 0;
	float   wheelSpeed             = 0;
	int    carModelAlphaFlag       = 0;
	uint8_t nearFarLightState      = 0;
	uint8_t brakeLightState        = 0;
	uint8_t turnLightState         = 0;
	uint8_t ruingLightState        = 0;
	float seatAngle1   		       = 150.f;

	/** @brief frame count */
	uint32_t frameCount    = 0;
	timeval timeCurrent;
	long usecEnd           = 0;
	long usecStart         = 0;
	gettimeofday(&timeCurrent, NULL);
	usecStart = (timeCurrent.tv_sec * 1000000) + timeCurrent.tv_usec;

	long doRenderUsStart = BaseLib::TimeHelper::GetTimestampMicros();
	long doRenderUsEnd   = BaseLib::TimeHelper::GetTimestampMicros();
	std::chrono::time_point<std::chrono::steady_clock> now;
	while(isRunning)
	{
		now = std::chrono::steady_clock::now();

		if (nframes % 50 == 0)
		{
			/* wheel */
			switch (wheelDir)
			{
				case 0:
					wheelDir = 3;
					break;
				case 3:
					wheelDir = 2;
					break;
				case 2:
					default:
					wheelDir = 0;
					break;
			}

			/* door status */
			switch (doorSwitchSts)
			{
				case 0:
					doorSwitchSts = 1;
					break;
				case 1:
					doorSwitchSts = 2;
					break;
				case 2:
				default:
					doorSwitchSts = 0;
					break;
			}
			switch (sunFloorSts)
			{
				case 0x1A:
					sunFloorSts = 0x01;
					break;
				default:
					sunFloorSts += 5;
					break;
			}
			
			switch (mirror)
			{
				case 0:
					mirror = 1;
					break;
				case 1:
					mirror = 2;
					break;
				case 2:
					mirror = 1;
					break;
				default:
					mirror = 1;
					break;
			}

			switch(nearFarLightState)
			{
				case 0:
					nearFarLightState = 1;
					break;
				case 1:
					nearFarLightState = 2;
					break;
				case 2:
					nearFarLightState = 3;
					break;
				case 3:
					nearFarLightState = 0;
					break;
			}

			switch(brakeLightState)
			{
				case 0:
					brakeLightState = 1;
					break;
				case 1:
					brakeLightState = 0;
					break;
			}

			switch(turnLightState)
			{
				case 0:
					turnLightState = 1;
					break;
				case 1:
					turnLightState = 2;
					break;
				case 2:
					turnLightState = 3;
					break;
				case 3:
					turnLightState = 0;
					break;
			}

			switch(ruingLightState)
			{
				case 0:
					ruingLightState = 1;
					break;
				case 1:
					ruingLightState = 0;
					break;
			}
		
		}

		if (nframes % 10 == 0)
		{
			switch (flWindow)
			{
				case 0x1A:
					flWindow = 0x01;
					break;
				default:
					flWindow += 1;
					break;
			}

			if (seatAngle1 > 180.0f)
			{
				seatAngle1 = 0.0f;
			}
			else 
			{
				seatAngle1 += 10;
			}

			if (wheelSpeed < 10.f)
			{
				wheelSpeed += 0.01;
			}
			else
			{
				wheelSpeed = 0.0f;
			}
		}

		if (nframes % 150 == 0)
		{
			switch (carModelAlphaFlag)
			{
				case 0:
					carModelAlphaFlag = 1;
					break;
				case 1:
					carModelAlphaFlag = 2;
					break;
				default:
				case 2:
					carModelAlphaFlag = 0;
					break;
			}
		}


		/* [3D animation] set data start */
		wheelDir = WHEEL_DIRECTION_FORWARD;
		// wheelDir = WHEEL_DIRECTION_BACKWARD;
		// wheelDir = WHEEL_DIRECTION_STATIC;
		animation3D.cm3DFrontLeftWheelState.direction  = wheelDir;      // wheel: front left
		animation3D.cm3DRearLeftWheelState.direction   = wheelDir;      // wheel: rear left
		animation3D.cm3DFrontRightWheelState.direction = wheelDir;      // wheel: front right
		animation3D.cm3DRearRightWheelState.direction  = wheelDir;      // wheel: rear right

		// animation3D.cm3DFrontLeftWheelState.direction  = 0;      // wheel: front left
		// animation3D.cm3DRearLeftWheelState.direction   = 0;      // wheel: rear left
		// animation3D.cm3DFrontRightWheelState.direction = 0;      // wheel: front right
		// animation3D.cm3DRearRightWheelState.direction  = 0;      // wheel: rear right


		wheelSpeed = 0.01;
		wheelSpeed = 1.4363f / 4.0;
		animation3D.cm3DFrontLeftWheelState.speed  = wheelSpeed;
		animation3D.cm3DRearLeftWheelState.speed   = wheelSpeed;
		animation3D.cm3DFrontRightWheelState.speed = wheelSpeed;
		animation3D.cm3DRearRightWheelState.speed  = wheelSpeed;

		///
		// doorSwitchSts = 1;
		// doorSwitchSts = 2;
		animation3D.cm3DFrontLeftDoorState    = doorSwitchSts; // door: front left
		animation3D.cm3DFrontRightDoorState   = doorSwitchSts; // door: front right
		animation3D.cm3DRearLeftDoorState     = doorSwitchSts; // door: rear left
		animation3D.cm3DRearRightDoorState    = doorSwitchSts; // door: rear right
		animation3D.cm3DEngineCoverState      = doorSwitchSts; // engine cover
		animation3D.cm3DRearTailBoxState      = doorSwitchSts; // rear trunk


		// flWindow = 0x1;
		// flWindow = 0x1A;
		animation3D.cm3DSunFloorState		  = flWindow; //sunFloorSts;
		// animation3D.cm3DSunFloorState		  = 0;
		
		animation3D.cm3DFrontLeftWindowState      = flWindow;
		animation3D.cm3DFrontRightWindowState     = flWindow;
		animation3D.cm3DRearLeftWindowState       = flWindow;
		animation3D.cm3DRearRightWindowState      = flWindow;

		animation3D.cm3DFrontLeftWheelState.angle  = 30.f * std::sin(glm::radians(nframes * 1.0f));  // wheel angle
		animation3D.cm3DFrontRightWheelState.angle = 30.f * std::sin(glm::radians(nframes * 1.0f));  // wheel angle

		// animation3D.cm3DFrontLeftWheelState.angle  = 0.f;  // wheel angle
		// animation3D.cm3DFrontRightWheelState.angle = 0.f; 

		animation3D.cm3DLeftMirrorsState          = mirror;
		animation3D.cm3DRightMirrorsState         = mirror;

	#if 0
		/** @brief 灯光 */
		animation3D.cm3DLowBeamState              = 1; // 近光灯 0:OFF, 1:ON
		animation3D.cm3DNearFarLightState         = 1; // 远光灯 0:OFF, 1:ON
		animation3D.cm3DBrakeLightState           = 0;//brakeLightState;
		animation3D.cm3DTurnLightState            = 3;
		animation3D.cm3DRuingLightState           = 1;
		animation3D.cm3DRuingSecondaryLightState  = 3;
		animation3D.cm3DRearPositionLightState	  = 1;
		animation3D.cm3DWelcomeLightState         = 1;
		animation3D.cm3DReverseLightState         = 1;
		animation3D.cm3DFogLampState              = 1;
	#endif 

	#if 1
		animation3D.cm3DLowBeamState              = 0; // 近光灯 0:OFF, 1:ON
		animation3D.cm3DNearFarLightState         = 0; // 远光灯 0:OFF, 1:ON
		animation3D.cm3DBrakeLightState           = 1;//brakeLightState;
		animation3D.cm3DTurnLightState            = 0;
		animation3D.cm3DRuingLightState           = 0;
		animation3D.cm3DRuingSecondaryLightState  = 0;
		animation3D.cm3DRearPositionLightState    = 0;
		animation3D.cm3DWelcomeLightState         = 0;
		animation3D.cm3DReverseLightState         = 0;
		animation3D.cm3DFogLampState              = 0;
	#endif 

	#if 1
		/** @brief 用例:日行灯亮 */
		animation3D.cm3DBrakeLightState           = 1; // 刹车灯 0:开启 1:关闭
		animation3D.cm3DTurnLightState            = 0; // 转向灯 1:左转， 2:右转，3:全亮
		animation3D.cm3DRuingLightState           = 1; // 车头中间贯穿灯:0:OFF, 1:ON
		animation3D.cm3DRuingSecondaryLightState  = 3; // 前左右日行灯信号 0:OFF, 1:left ON, 2:right ON, 3:both ON
		animation3D.cm3DRearPositionLightState    = 1; // 后位置灯灯信号 0:OFF, 1:ON （默认点亮）
	#endif 

	#if 0
		/** @brief 用例:左转向灯闪烁ON */
		animation3D.cm3DBrakeLightState           = 1; // 刹车灯 0:开启 1:关闭
		animation3D.cm3DTurnLightState            = 1; // 转向灯 1:左转， 2:右转，3:全亮
		animation3D.cm3DRuingLightState           = 1; // 车头中间贯穿灯:0:OFF, 1:ON
		animation3D.cm3DRuingSecondaryLightState  = 3; // 前左右日行灯信号 0:OFF, 1:left ON, 2:right ON, 3:both ON
		animation3D.cm3DRearPositionLightState    = 1; // 后位置灯灯信号 0:OFF, 1:ON （默认点亮）
	#endif

	#if 0
		/** @brief 用例:左转向灯闪烁OFF */
		animation3D.cm3DBrakeLightState           = 1; // 刹车灯 0:开启 1:关闭
		animation3D.cm3DTurnLightState            = 0; // 转向灯 1:左转， 2:右转，3:全亮
		animation3D.cm3DRuingLightState           = 1; // 车头中间贯穿灯:0:OFF, 1:ON
		animation3D.cm3DRuingSecondaryLightState  = 3; // 前左右日行灯信号 0:OFF, 1:left ON, 2:right ON, 3:both ON
		animation3D.cm3DRearPositionLightState    = 1; // 后位置灯灯信号 0:OFF, 1:ON （默认点亮）
	#endif 

	#if 0
		/** @brief 用例:右转向灯闪烁ON */
		animation3D.cm3DBrakeLightState           = 1; // 刹车灯 0:开启 1:关闭
		animation3D.cm3DTurnLightState            = 2; // 转向灯 1:左转， 2:右转，3:全亮
		animation3D.cm3DRuingLightState           = 1; // 车头中间贯穿灯:0:OFF, 1:ON
		animation3D.cm3DRuingSecondaryLightState  = 3; // 前左右日行灯信号 0:OFF, 1:left ON, 2:right ON, 3:both ON
		animation3D.cm3DRearPositionLightState    = 1; // 后位置灯灯信号 0:OFF, 1:ON （默认点亮）
	#endif

	#if 0
		/** @brief 用例:右转向灯闪烁OFF */
		animation3D.cm3DBrakeLightState           = 1; // 刹车灯 0:开启 1:关闭
		animation3D.cm3DTurnLightState            = 0; // 转向灯 1:左转， 2:右转，3:全亮
		animation3D.cm3DRuingLightState           = 1; // 车头中间贯穿灯:0:OFF, 1:ON
		animation3D.cm3DRuingSecondaryLightState  = 3; // 前左右日行灯信号 0:OFF, 1:left ON, 2:right ON, 3:both ON
		animation3D.cm3DRearPositionLightState    = 1; // 后位置灯灯信号 0:OFF, 1:ON （默认点亮）
	#endif 

	static long turnFlashUsStart = BaseLib::TimeHelper::GetTimestampMicros();
	static int turnLightSt    = 0;
	static int turnLightStPre = 0;

	if ((BaseLib::TimeHelper::GetTimestampMicros() - turnFlashUsStart) >= 360000)
	{
		turnFlashUsStart = BaseLib::TimeHelper::GetTimestampMicros();

		if (turnLightSt == 0)
		{
			turnLightSt    = turnLightStPre+1;
			if(turnLightSt > 3) 
			{
				turnLightSt = 1;
			}
		}
		else
		{
			turnLightStPre = turnLightSt;
			turnLightSt = 0;
		}
	}

	#if 1
		/** @brief 用例:双灯闪烁ON */
		animation3D.cm3DBrakeLightState           = 1; // 刹车灯 0:开启 1:关闭
		animation3D.cm3DTurnLightState            = turnLightSt; // 转向灯 1:左转， 2:右转，3:全亮
		animation3D.cm3DRuingLightState           = 1; // 车头中间贯穿灯:0:OFF, 1:ON
		animation3D.cm3DRuingSecondaryLightState  = 3; // 前左右日行灯信号 0:OFF, 1:left ON, 2:right ON, 3:both ON
		animation3D.cm3DRearPositionLightState    = 1; // 后位置灯灯信号 0:OFF, 1:ON （默认点亮）
	#endif

	#if 0
		/** @brief 用例:双灯闪烁OFF */
		animation3D.cm3DBrakeLightState           = 0; // 刹车灯 0:开启 1:关闭
		animation3D.cm3DTurnLightState            = 0; // 转向灯 1:左转， 2:右转，3:全亮
		animation3D.cm3DRuingLightState           = 1; // 车头中间贯穿灯:0:OFF, 1:ON
		animation3D.cm3DRuingSecondaryLightState  = 3; // 前左右日行灯信号 0:OFF, 1:left ON, 2:right ON, 3:both ON
		animation3D.cm3DRearPositionLightState    = 1; // 后位置灯灯信号 0:OFF, 1:ON （默认点亮）
	#endif 

	#if 0
		/** @brief 用例:刹车灯亮 */
		animation3D.cm3DBrakeLightState           = 0; // 刹车灯 0:开启 1:关闭
		animation3D.cm3DRuingLightState           = 1; // 车头中间贯穿灯:0:OFF, 1:ON
		animation3D.cm3DRuingSecondaryLightState  = 3; // 前左右日行灯信号 0:OFF, 1:left ON, 2:right ON, 3:both ON
		animation3D.cm3DRearPositionLightState    = 1; // 后位置灯灯信号 0:OFF, 1:ON （默认点亮）
	#endif

	#if 1
		/** @brief 用例:近光灯亮 */
		animation3D.cm3DLowBeamState              = 1; // 近光灯 0:OFF, 1:ON
		animation3D.cm3DNearFarLightState         = 0; // 远光灯 0:OFF, 1:ON
	#endif

	#if 0
		/** @brief 用例:远近大灯亮 */
		animation3D.cm3DLowBeamState              = 1; // 近光灯 0:OFF, 1:ON
		animation3D.cm3DNearFarLightState         = 1; // 远光灯 0:OFF, 1:ON
	#endif


		// seatAngle1 = 110.0f;
		animation3D.seatAngle                     = seatAngle1;

		/**************************************************************************************************
		 * @brief 
		 * 【3D车模透明度设置】
			1. 不透明状态下：
				内饰：1.0
				底盘：1.0
				车窗：0.8
				车轮：1.0
				车框：1.0
			2. 透明状态下：
				内饰：0.0
				底盘：0.2
				车窗：0.2
				车轮：0.65
				车框：0.15
		 * 
		**************************************************************************************************/

		/** @brief  Alpha: 透明度[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明 */
		// carModelAlphaFlag = 1;
		carModelAlphaFlag = 2;
		if (carModelAlphaFlag == 1)
		{
			/** @brief 透明 */
			animation3D.carModelAlpha        = 0.15f;  ///< 车身内饰等（不包括底盘）透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.chassisAlpha         = 0.35f;  ///< 车辆底盘透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.windshieldAlpha      = 0.32f; ///< 前挡风玻璃透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.windowAlpha          = 0.32f;  ///< 前排车窗透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.passengerWindowAlpha = 0.32f; ///< 乘员车窗透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.sunFloorAlpha        = 0.32f;  ///< 天窗和尾窗透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.wheelAlpha           = 1.0f;  ///< 车轮透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.carFrameAlpha        = 0.35f;  ///< 车框透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.shadowAlpha          = 0.95f;  ///< 阴影透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明

			// animation3D.carModelAlpha        = 0.01f;  ///< 车身内饰等（不包括底盘）透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			// animation3D.chassisAlpha         = 0.60f;  ///< 车辆底盘透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			// animation3D.windshieldAlpha      = 0.60f; ///< 前挡风玻璃透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			// animation3D.windowAlpha          = 0.60f;  ///< 前排车窗透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			// animation3D.passengerWindowAlpha = 0.60f; ///< 乘员车窗透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			// animation3D.sunFloorAlpha        = 0.60f;  ///< 天窗和尾窗透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			// animation3D.wheelAlpha           = 1.00f;  ///< 车轮透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			// animation3D.carFrameAlpha        = 0.60f;  ///< 车框透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			// animation3D.shadowAlpha          = 0.60f;  ///< 阴影透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
		}
		else if (carModelAlphaFlag == 2) 
		{
			/** @brief 窗透明 */
			animation3D.carModelAlpha        = 1.0f;  ///< 车身内饰等（不包括底盘）透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.chassisAlpha         = 1.0f;  ///< 车辆底盘透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.windshieldAlpha      = 0.65f; ///< 前挡风玻璃透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.windowAlpha          = 0.8f;  ///< 前排车窗透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.passengerWindowAlpha = 0.85f; ///< 乘员车窗透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.sunFloorAlpha        = 0.98f;  ///< 天窗和尾窗透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.wheelAlpha           = 1.0f;  ///< 车轮透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.carFrameAlpha        = 1.0f;  ///< 车框透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.shadowAlpha          = 0.93f;  ///< 阴影透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
		}
		else
		{
			/** @brief 不透明 */
			animation3D.carModelAlpha        = 1.0f;  ///< 车身内饰等（不包括底盘）透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.chassisAlpha         = 1.0f;  ///< 车辆底盘透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.windshieldAlpha      = 1.00f; ///< 前挡风玻璃透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.windowAlpha          = 1.00f;  ///< 前排车窗透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.passengerWindowAlpha = 1.0f; ///< 乘员车窗透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.sunFloorAlpha        = 1.00f;  ///< 天窗和尾窗透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.wheelAlpha           = 1.0f;  ///< 车轮透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.carFrameAlpha        = 1.0f;  ///< 车框透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
			animation3D.shadowAlpha          = 1.0f;  ///< 阴影透明度，范围[0.f, 1.f] 0.0f:完全透明， 1.f:完全不透明
		}


		/** @brief 动画速率 */
		animation3D.doorAnimationRate          = 90.f; ///< 四门动画速率，单位:角度/s
        animation3D.hoodTrunkAnimatinoRate     = 45.0f; ///< 引擎盖行李箱动画速率，单位:角度/s
        animation3D.windowAnimationRate        = 25.f; ///< 车窗开度动画速率，单位:开度/s
        animation3D.rearMirrorAnimationRate    = 30.f; ///< 后视镜动画速率，单位:角度/s
        animation3D.sunFloorAnimationRate      = 60.f; ///< 天窗动画速率， 单位:开度/s
		animation3D.frontWheelYawAnimationRate = 30.f; ///< 前轮摆动速率， 单位:角度/s

		viewParams.model = glm::mat4(1.f); 
		viewParams.model = glm::scale(viewParams.model, glm::vec3(1.25f, 1.25f, 1.25f));
		viewParams.model = glm::translate(viewParams.model, glm::vec3(0.18f, 0.0f, 0.0f));
		// viewParams.model = glm::rotate(viewParams.model, glm::radians(0.25f * nframes), glm::vec3(0.f, 1.f, 0.0f)); // rotation 
		viewParams.model = glm::rotate(viewParams.model, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.0f)); // rotation 


		glm::vec3 cameraBasicPosition = glm::vec3(0.0f, 3.5f, 5.5f);

		float camTheta = nframes * 0.5f;
		float camPhi   = 0.0f;


		float R = 5.5f;
		// float camX = R * glm::cos(glm::radians(windowRendering.camPhi)) * glm::sin(glm::radians(windowRendering.camTheta));
		// float camZ = -R * glm::cos(glm::radians(windowRendering.camPhi)) * glm::cos(glm::radians(windowRendering.camTheta));
		// float camY = fmax(R * glm::cos(glm::radians(windowRendering.camPhi)), 1e-3);

		float camX =  R * glm::sin(glm::radians(camTheta));
		float camZ =  R * glm::cos(glm::radians(camTheta));
		float camY =  2.5f;
		camY       = fmax(R * glm::cos(glm::radians(camPhi)), 1e-3);
		camY       = R * glm::cos(glm::radians(camPhi));

		// LOGGER_I("camPhi=%f, camTheta=%f", camPhi, camTheta);
		// viewParams.cameraPosition = glm::vec3(0.f, 3.5f, 5.5f);
		viewParams.cameraPosition = glm::vec3(camX, camY, camZ);


		viewParams.view  = glm::lookAt(
			viewParams.cameraPosition,  // Camera position
			glm::vec3(0, 0, 0),               // looks at the origin
			glm::vec3(0, 1, 0)                // Head up
		);


		viewParams.projection = glm::perspective(glm::radians(45.f), ratio, 1.5f, 1000.f);
		/* [3D animation] set data done */
	

		if (nframes % 300 == 1) 
		{
			/** @brief mirrors fold */
			animation3D.cm3DLeftMirrorsState  = 2;
			animation3D.cm3DRightMirrorsState = 2;
			animation3D.seatAngle             = 110.f;

			/** @brief window closed */
			animation3D.cm3DFrontLeftWindowState    = 0x01;
			animation3D.cm3DFrontRightWindowState   = 0x01;
			animation3D.cm3DRearLeftWindowState     = 0x01;
			animation3D.cm3DRearRightWindowState    = 0x01;

			/** @brief doors closed */
			animation3D.cm3DFrontLeftDoorState    = 2; // door: front left
			animation3D.cm3DFrontRightDoorState   = 2; // door: front right
			animation3D.cm3DRearLeftDoorState     = 2; // door: rear left
			animation3D.cm3DRearRightDoorState    = 2; // door: rear right

			doorSwitchSts = 2;

			carModel3D.ResetStatus(animation3D);
			LOGGER_I("carModel3D.ResetStatus"); 
		}

		
		/**< step: do render here */
		/**< step: !important, call before rendering */

		/**************************************************************************************************
		 * @brief AA BIND TO TARGET framebuffer
		 * 
		**************************************************************************************************/
		if (fboMSAA.id > 0 && fboSSAA.id > 0)
		{
			/** @brief MSAA framebuffer */
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboMSAA.id);
			glClearColor(CLEAR_COLOR);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glViewport(0, 0, fboMSAA.size.x, fboMSAA.size.y);
		}
		else if (fboSSAA.id > 0)
		{
			/** @brief SSAA framebuffer */
			glBindFramebuffer(GL_FRAMEBUFFER, fboSSAA.id);
			glClearColor(CLEAR_COLOR);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glViewport(0, 0, fboSSAA.size.x, fboSSAA.size.y);
		}
		else 
		{
			/** @brief DEFAULT framebuffer */
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(CLEAR_COLOR_FBO_DEFAULT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		#ifdef RENDER_3D_VIEW_BACKGROUND
			/**************************************************************************************************
			 * @brief RENDER BACKGROUND
			 * 
			**************************************************************************************************/
			glViewport(VIEWPORT_TUPLE_WHOLE_SCREEN);
			glEnable(GL_BLEND);
			simpleShader.UseProgram();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureBg.id);
			glBindVertexArray(VAOAvmBackground);
			glDrawElements(GL_TRIANGLES, 6, EBO_GL_TYPE, 0);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		#endif // RENDER_3D_VIEW_BACKGROUND

			glViewport(VIEWPORT_TUPLE_CAR_MODEL); /* optional: specified viewport */
		}

		

		/**************************************************************************************************
		 * @brief AA RENDER TO target framebuffer
		 * 
		**************************************************************************************************/
	#ifdef DRAW_3D_VIEW_CAR_MODEL
		/* [3D animation] DoRender */
		carModel3D.DoRender(animation3D, viewParams);
		LOGGER_GLE("carModel3D.DoRender");
	#endif // DRAW_3D_VIEW_CAR_MODEL


		/**************************************************************************************************
		 * @brief AA glBlitFramebuffer
		 * 
		**************************************************************************************************/
		if (fboMSAA.id > 0 && fboSSAA.id > 0)
		{
			/** @brief BLIT TO SSAA framebuffer */
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

		/**************************************************************************************************
		 * @brief AA MIPMAP
		 * 
		**************************************************************************************************/
		if (fboSSAA.colorTextId > 0)
		{
			glBindTexture(GL_TEXTURE_2D, fboSSAA.colorTextId);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}


	#ifdef SAVE_RENDER_PICTURE
		if (fboSSAA.id > 0)
		{
			cv::Mat image(fboSSAA.size.x, fboSSAA.size.y, CV_8UC4); ///< origin data mat
			glReadPixels(0, 0, fboSSAA.size.x, fboSSAA.size.y, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
			std::string filename = "./logimg/" + std::to_string(nframes) + ".png";
			ImageHelper::Save2image(filename,  image.data, fboSSAA.size.x, fboSSAA.size.y, 4, false);
		}
		
	#endif // SAVE_RENDER_PICTURE


#ifdef ENABLE_WINDOW_TEST

		if (fboSSAA.colorTextId > 0)
		{
			/* [offscreen] !important bind, call before rendering */
			// offScreenRendering.BindFramebuffer(); /* optional: bind framebuffer */
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glClearColor(CLEAR_COLOR_FBO_DEFAULT);
			glClear(GL_COLOR_BUFFER_BIT);

			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		#ifdef RENDER_3D_VIEW_BACKGROUND
			/**************************************************************************************************
			 * @brief RENDER BACKGROUND
			 * 
			**************************************************************************************************/
			glViewport(VIEWPORT_TUPLE_WHOLE_SCREEN);
			simpleShader.UseProgram();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureBg.id);
			glBindVertexArray(VAOAvmBackground);
			glDrawElements(GL_TRIANGLES, 6, EBO_GL_TYPE, 0);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		#endif // RENDER_3D_VIEW_BACKGROUND


			/**************************************************************************************************
		     * @brief AA RENDER CAR MODE FBO TEXTURE
		     * 
		     **************************************************************************************************/
			glViewport(VIEWPORT_TUPLE_CAR_MODEL);
			simpleShader.UseProgram();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, fboSSAA.colorTextId);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, EBO_GL_TYPE, 0);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glDisable(GL_BLEND);
		}
		
		#endif // ENABLE_WINDOW_TEST

#ifdef ENABLE_WINDOW_TEST
		/* [offscreen] send data here, call after rendering */
		// windowRendering.WindowDisplay();  /* optional: display */
		screenRendering.SwapBuffers();

		#ifdef SAVE_RENDER_PICTURE
		{
			std::string filename = "./log/" + std::to_string(nframes) + ".png";
			ImageHelper::Save2image(filename, offScreenRendering.fboCurrent.fboBufData.data, offScreenRendering.window.width, offScreenRendering.window.height, offScreenRendering.window.channels, false);
		}
		#endif // SAVE_RENDER_PICTURE
#endif // ENABLE_WINDOW_TEST
		
		nframes++;

		/** @brief frame count */
		frameCount++;
		gettimeofday(&timeCurrent, NULL);
		usecEnd = (timeCurrent.tv_sec * 1000000) + timeCurrent.tv_usec;
		long duration = (usecEnd - usecStart);
		/** calculate FPS every 1s */
		if (duration >= 1000000)
		{
			double fps = frameCount * 1000000.0f / duration;
			LOGGER_I("FPS=%lf frameCount=%u usecEnd=%ld(us) usecStart=%ld(us) duration=%ld(us)", fps, frameCount, usecEnd, usecStart, duration);
			frameCount = 0;
			usecStart = usecEnd;
		}

		std::this_thread::sleep_until(now + std::chrono::milliseconds(40));

	}

	LOGGER_I("done");

	return 0;
}
