SET(CMAKE_POLICY_DEFAULT_CMP0012 NEW)
cmake_minimum_required(VERSION 3.16.0)

project(3dlib-demo-pc)

message(STATUS "camke souce for pc virtual view lib demo")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/bin)


###############################################################################
# BUILD FOR PLATFORM linux x86_64
###############################################################################
# important macros
add_definitions(-DENABLE_VERTEX_NORMAL)
add_definitions(-DUSE_GLES32)

add_compile_definitions(GL_GLEXT_PROTOTYPES)

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

###############################################################################
# headers and lib
###############################################################################
include_directories(${ASSIMP_INCLUDE_DIRS})
link_directories(${ASSIMP_LIBRARY_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/../3dlib-android/app/src/main/cpp/3dlib/include)
link_directories(${CMAKE_SOURCE_DIR}/3dlib/lib/linux/x86_64)

link_directories(${CMAKE_SOURCE_DIR}/thirdparty/opengl/angle)


# demo 
include_directories(${CMAKE_SOURCE_DIR}/source)
include_directories(${CMAKE_SOURCE_DIR}/source/x86_64)

###############################################################################
# set virtualview 3D lib name
###############################################################################
set(LIB_VV3D    CarAnimation3D)
# set(LIBS_GL     GL GLU GLEW glut m ${ASSIMP_LIBRARIES})
set(LIBS_GL     GLESv2 EGL X11 m ${ASSIMP_LIBRARIES})


set(SRC_MAIN  
   ${CMAKE_SOURCE_DIR}/source/EGLScreen.hpp 
   ${CMAKE_SOURCE_DIR}/source/main_3dfbo_msaa_pc_egles.cpp
  )

# 3d animation test exe
add_executable(3dlib-demo-pc 
               ${SRC_MAIN}
              )

target_link_libraries(3dlib-demo-pc
                      ${LIB_VV3D}
                      ${LIBS_GL}
                     )
