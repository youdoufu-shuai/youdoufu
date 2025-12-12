# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("lib3d")

set(TARGET_OS android)

###############################################################################
# set 3D animation lib name
###############################################################################
set(LIB_3D      CarAnimation3D)

###############################################################################
# defines
###############################################################################
add_definitions(-DUSE_GLES32)
add_definitions(-DENABLE_VERTEX_NORMAL)
add_definitions(-DBUILD_TARGET_OS_ANDROID)
add_definitions(-DENABLE_IBL_FEATURE)
add_definitions(-DUSE_ANDROID_LOG)


###############################################################################
# include
###############################################################################
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/thirdparty/android/llvm/include)
include_directories(${CMAKE_SOURCE_DIR}/tool)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/assimp/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/yaml-cpp-0.8.0/include)
include_directories(${CMAKE_SOURCE_DIR}/3dlib/include)
include_directories(${CMAKE_SOURCE_DIR}/demo)
include_directories(${CMAKE_SOURCE_DIR}/model)
include_directories(${CMAKE_SOURCE_DIR}/include)

###############################################################################
# link directories
###############################################################################
link_directories(${CMAKE_SOURCE_DIR}/3dlib/lib/${TARGET_OS}/${ANDROID_ABI})
link_directories(${CMAKE_SOURCE_DIR}/thirdparty/assimp/lib/${TARGET_OS}/${ANDROID_ABI})

link_directories(${CMAKE_SOURCE_DIR}/thirdparty/stl/lib/${TARGET_OS}/${ANDROID_ABI})

###############################################################################
# yaml-cpp configuration
###############################################################################
# 设置yaml-cpp选项
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Disable yaml-cpp tests" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Disable yaml-cpp tools" FORCE)
set(YAML_CPP_BUILD_CONTRIB ON CACHE BOOL "Enable yaml-cpp contrib" FORCE)
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "Build yaml-cpp as static library" FORCE)

# 添加yaml-cpp子目录
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/yaml-cpp-0.8.0)


add_library( # Sets the name of the library.
        lib3d

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp
        ${CMAKE_SOURCE_DIR}/demo/RenderContext3D.cpp
        ${CMAKE_SOURCE_DIR}/demo/Car3DModelDisplay.cpp
        ${CMAKE_SOURCE_DIR}/demo/CarExitDirection.cpp
        ${CMAKE_SOURCE_DIR}/demo/GearDisplay.cpp
        ${CMAKE_SOURCE_DIR}/demo/ObstacleDisplay.cpp
        ${CMAKE_SOURCE_DIR}/demo/ParkingDisplay.cpp
        ${CMAKE_SOURCE_DIR}/demo/ParkingProgress.cpp
        ${CMAKE_SOURCE_DIR}/demo/ParkingProgressBar.cpp
        ${CMAKE_SOURCE_DIR}/demo/RadarAlarmDisplay.cpp
        ${CMAKE_SOURCE_DIR}/demo/RadarAlarmDistance.cpp
        ${CMAKE_SOURCE_DIR}/demo/RadarWaterRipple.cpp
        ${CMAKE_SOURCE_DIR}/demo/VehicleMoveDirectionArrow.cpp
        ${CMAKE_SOURCE_DIR}/demo/VirtualView.cpp
        ${CMAKE_SOURCE_DIR}/demo/VisualAngleControl.cpp
        ${CMAKE_SOURCE_DIR}/demo/UIDisplay.cpp
        ${CMAKE_SOURCE_DIR}/model/Mesh.cpp
        ${CMAKE_SOURCE_DIR}/model/Model.cpp
        ${CMAKE_SOURCE_DIR}/model/Shader.cpp
        ${CMAKE_SOURCE_DIR}/model/YamlLoader.cpp
        ${CMAKE_SOURCE_DIR}/tool/GlprocLoader.cpp
)

target_link_libraries( # Specifies the target library.
        lib3d
        opencv_java4
        log
        EGL
        GLESv3
        c++
        assimp
        yaml-cpp::yaml-cpp

        ${LIB_3D}
)
