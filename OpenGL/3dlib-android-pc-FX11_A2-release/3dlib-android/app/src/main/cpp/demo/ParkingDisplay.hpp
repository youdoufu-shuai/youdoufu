/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ParkingDisplay.hpp
 * Author: tron
 * Function: 泊车位置显示
 * Created on 2023年5月8日, 上午9:10
 */

#ifndef PARKINGDISPLAY_HPP
#define PARKINGDISPLAY_HPP

#include "Include.hpp"
#include "../model/Model.hpp"
#include "VisualAngleControl.hpp"

namespace VirtualView3D {

#define CFG_KEY_VIRTUAL_VIEW_PARKING_SLOT       "parking_slot"
#define CFG_KEY_VIRTUAL_VIEW_SHAPE              "shape"
#define CFG_KEY_VIRTUAL_VIEW_ANGLE              "angle"
#define CFG_KEY_VIRTUAL_VIEW_LENGTH             "length"
#define CFG_KEY_VIRTUAL_VIEW_WIDTH              "width"
#define CFG_KEY_VIRTUAL_SHAPE_ANGLE_TOLERANCE   "shape_angle_tolerance"
#define CFG_KEY_VIRTUAL_FILTER_K_ANGLE          "filter_k_angle"
#define CFG_KEY_VV_LENGTH_TOLERANCE             "length_tolerance"
#define CFG_KEY_VV_WIDTH_TOLERANCE              "width_tolerance"
#define CFG_KEY_VV_SLOT_SIZE_SCALE              "slot_size_scale"
#define CFG_KEY_VV_SLOT_NUM_MAX                 "slot_num_max"

#define VV_PARKING_EXTRA_VAO_SIZE      (1)

#define VV_PARKING_MARKERS_SIZE_COMMON (8)
#define VV_PARKING_MARKERS_SIZE_LEFT   (9)
#define VV_PARKING_MARKERS_SIZE_RIGHT  (9)
#define VV_PARKING_MARKERS_SIZE_MIDDLE (7)

    using namespace BaseLib;

/* 
*功能：
*    1、根据输出的数据渲染显示车位，车位类型。
*    2、输入点击坐标，判断响应。
*    3、选择车位后，渲染更新选中车位纹理等。
*/
    enum class SlotType : std::int8_t {
        SLOT_UNKNOW_TYPE       = 0,
        SLOT_LEFT_RAPA         = 1,
        SLOT_RIGHT_RAPA        = 2,
        SLOT_LEFT_VERT         = 3,
        SLOT_RIGHT_VERT        = 4,
        SLOT_LEFT_ANG_FORWARD  = 5,
        SLOT_LEFT_ANG_REVERSE  = 6,
        SLOT_RIGHT_ANG_FORWARD = 7,
        SLOT_RIGHT_ANG_REVERSE = 8,
        SLOT_VIRTUAL           = 9, /*!< virtual parking slot, internal use */
    };

/*
* 车位坐标  单位：m
*/
    struct ParkingPoint {
        double x;
        double y;
        double z;
    };

/*
* 屏幕点击坐标位置 -> 单位：像素坐标
*/
    struct TouchPoint {
        float pixelX;
        float pixelY;
    };

/*
* 车位空间 值为 0-1
*/
    enum class ParkingSpaceType : std::int8_t {
        NARROW_SPACE   = 0,    /*!< 窄车位 	*/
        NORMAL_SPACE   = 1,    /*!< 正常车位 	*/
    };

/**
 * @brief 车位显示内容 值为 0-3
 */
    enum class ParkingSpaceShowContents : std::int8_t {
        SHOW_NONE      = 0,    /*!< 不显示，NONE */
        SHOW_NUMBERS   = 1,    /*!< 显示数字  */
        SHOW_P         = 2,    /*!< 显示P     */
        SHOW_BALLOON_P = 3,    /*!< 显示气球P */
    };

/*
* 车位类型 值为 0-1
*/
    enum class ParkingType : std::int8_t {
        NO_PARKING_ALLOWED  = 0,		/*!< 不可泊车位   */
        PARKING_IS_POSSIBLE = 1,		/*!< 可泊车位 	  */
    };

/*
* 泊车位信息
*/
    struct Slot {
        SlotType slotType;

        /*按照0:A 1:B 2:C 3:D 单位m*/
        ParkingPoint parkingPoint[4];

        /*车位id   唯一性  */
        int32_t parkingID;
        int32_t displayNum; ///< 车位显示数字

        /*
        *具体显示选择项： 数字； P ；气球P;
        *每个车位： 数字 1-1000
        *每个车位真实唯一ID,绑定了该车位
        *车位显示内容
        */
        ParkingSpaceShowContents parkingSpaceShowContents;

        /* 车位空间：正常车位， 窄车位*/
        ParkingSpaceType parkingSpaceType;

        /*车位类型：可泊车位，不可泊车位*/
        ParkingType parkingType;
    };

    struct ParkSlotVao
    {
        int32_t parkingID   = 0; /*!< parkingID */
        float rad           = 0.f;
        GLuint vao          = GL_NONE;
        GLuint vbo          = GL_NONE;
        float length        = 0.f;
        float width         = 0.f;
    };

/*
* 泊车位置显示
*/

    class ParkingDisplay
    {
    public:
        ParkingDisplay();
        virtual ~ParkingDisplay();

        void Initialization(const std::shared_ptr<YAML::Node> & cfgNodePtr);
    public:

        /*选定泊车车位ID */
        void SelectedSlot(const int32_t id);

        /*根据传入的触摸屏幕坐标 反推实际坐标  -  返回车位真实 ID*/
        int32_t Touch(const TouchPoint &point);

        void DoRender(const VisualAngleControl &visualAngleControl, const std::vector<Slot>  &slots);

        /***************************************************************************************************
         * @brief render virtual parking slot
         *
         * @param visualAngleControl VisualAngleControl  common visual angle control
         * @param carPosition        glm::dvec3          car position, double value for (x,y,z)
         * @param isStatic           bool                whether static or moving
         ***************************************************************************************************/
        void DoRenderVirtual(const VisualAngleControl &visualAngleControl, const glm::dvec3 &carPosition, bool isStatic);

        static glm::vec3 GetParkingSlotCenter(const Slot& slot);
        static float GetParkingSlotDirYawRad(const Slot& slot, glm::vec2 &vecDirAB, glm::vec2 &vecDirAC, float &interRad, float &length, float &width);

        void GenParkingVao(float theta, ParkSlotVao &parkVao, bool isCacheVertices);

        void SetExtraRotation(float angle, const glm::vec3& axis);

    private:
        void InitTextures();
        void InitParking();

        GLuint parkingTextIdsCommon[VV_PARKING_MARKERS_SIZE_COMMON] = {0};
        GLuint parkingTextIdsLeft[VV_PARKING_MARKERS_SIZE_LEFT]     = {0};
        GLuint parkingTextIdsRight[VV_PARKING_MARKERS_SIZE_RIGHT]   = {0};
        GLuint parkingTextIdsMiddle[VV_PARKING_MARKERS_SIZE_MIDDLE] = {0};

        glm::ivec4 viewportVec4; /**< GLint x, GLint y, GLsizei width, GLsizei height */

        GLuint vaosParking[VV_PARKING_EXTRA_VAO_SIZE] = {GL_NONE};
        GLuint vbosParking[VV_PARKING_EXTRA_VAO_SIZE] = {GL_NONE};

        Shader parkingShader;
        Shader balloonShader;
        int selectedSlotId        = 0;

        std::map<int, glm::mat4> slotsModelMat4Map;
        glm::mat4 slotsViewMat4;
        glm::mat4 slotsProjectMat4;

        glm::vec3 selectSlotVec3;

        glm::mat4 ExtraRotationMat4;
        glm::mat4 parentMatrix;

        std::map<int32_t, std::vector<glm::vec3> > verticesVec3CacheMap;
        std::vector<Slot>                          slotsCacheVector;

        float mainScaleFactor;               /*!< main scale factor */

        glm::dvec4 carPositionBase;          /*!< car position (x,y,z,yaw) base for virtual parking slot */
        bool isRenderTouchable;              /*!< render touchable parking slots or not */

        std::shared_ptr<YAML::Node> configNodePtr;  /**< yaml config node */
        std::vector<ParkSlotVao> parkSlotVaoVector; /**< vector of parking slot vao */

        float shapeRadTolerance;
        float lengthTolerance;
        float widthTolerance;
        float slotSizeScale;
        size_t slotNumMax;
    } ;

};
#endif /* PARKINGDISPLAY_HPP */

