# 3D动效库示例 3dlib-demo

## 关于版本释放说明
- 暂无

## demo运行
 - android studio 工具

## 库怎样使用
1. 资源路径**3dlib-android/app/src/main/assets**下
2. 3d动效库路径**3dlib-android/app/src/main/cpp/3dlib/lib/android/arm64-v8a/libCarAnimation3D.so**
3. 第三方库目录**3dlib-android/app/src/main/cpp/thirdparty**，其中的libassimp.so库由**assimp-5.3.1**构建

## 如果修改了demo程序，如何重新编译demo


## demo依赖环境


## 3D动效库更新记录【FX11-A2】
#### **替换inclue、lib目录**以及**resource下junlian目录**
### 2025-01-08
1. 外饰相关纹理更新
2. 车轮相关纹理更新
3. HDR环境纹理使用hdr0108r90v1版本


### 2025-01-07
1. 外饰相关纹理更新
2. 车轮、玻璃、灯罩相关纹理更新
3. HDR环境纹理使用hdr_7版本


### 2025-01-06
1. 车模优化，
2. 车门添加车门旋转连接件和动画
3. 尾门添加支撑杆结构及动画
4. 灯光、内饰纹理更新
5. 外饰、车轮、玻璃相关纹理更新
6. HDR环境纹理使用hdr_6版本


### 2024-12-26
1. 车模优化，添加车门连接件
2. 车门动画参数更新
3. 外饰、玻璃相关贴图更新
4. 内饰、灯光贴图更新
5. HDR环境纹理更新


### 2024-12-12
1. 车模优化
2. 内饰纹理更新、灯光纹理更新
3. 车轮相关纹理更新、外饰相关纹理更新、玻璃相关纹理更新
4. HDR环境纹理优化


### 2024-11-29
1. 车模优化
2. 模型灯光结构调整
3. 车轮纹理、灯光纹理、内外饰等纹理更新
4. 支持天窗开度调节
```c++
   /** @brief 用例:天窗开度，范围：0x01~0x1A */
   animation3D.cm3DSunFloorState = 0x01;
```
5. 支持灯光效果，相关用例如下(详见README.md更新记录)：
```c++
   /** @brief 用例:日行灯亮 */
   animation3D.cm3DBrakeLightState           = 1; // 刹车灯 0:开启 1:关闭
   animation3D.cm3DTurnLightState            = 0; // 转向灯 1:左转， 2:右转，3:全亮
   animation3D.cm3DRuingLightState           = 1; // 车头中间贯穿灯:0:OFF, 1:ON
   animation3D.cm3DRuingSecondaryLightState  = 3; // 前左右日行灯信号 0:OFF, 1:left ON, 2:right ON, 3:both ON
   animation3D.cm3DRearPositionLightState    = 1; // 后位置灯灯信号 0:OFF, 1:ON （默认点亮）

   /** @brief 用例:双灯闪烁ON */
   animation3D.cm3DBrakeLightState           = 1; // 刹车灯 0:开启 1:关闭
   animation3D.cm3DTurnLightState            = 3; // 转向灯 1:左转， 2:右转，3:全亮
   animation3D.cm3DRuingLightState           = 1; // 车头中间贯穿灯:0:OFF, 1:ON
   animation3D.cm3DRuingSecondaryLightState  = 3; // 前左右日行灯信号 0:OFF, 1:left ON, 2:right ON, 3:both ON
   animation3D.cm3DRearPositionLightState    = 1; // 后位置灯灯信号 0:OFF, 1:ON （默认点亮）

   /** @brief 用例:双灯闪烁OFF */
   animation3D.cm3DBrakeLightState           = 1; // 刹车灯 0:开启 1:关闭
   animation3D.cm3DTurnLightState            = 0; // 转向灯 1:左转， 2:右转，3:全亮
   animation3D.cm3DRuingLightState           = 1; // 车头中间贯穿灯:0:OFF, 1:ON
   animation3D.cm3DRuingSecondaryLightState  = 3; // 前左右日行灯信号 0:OFF, 1:left ON, 2:right ON, 3:both ON
   animation3D.cm3DRearPositionLightState    = 1; // 后位置灯灯信号 0:OFF, 1:ON （默认点亮）

   /** @brief 用例:刹车灯亮 */
   animation3D.cm3DBrakeLightState           = 0; // 刹车灯 0:开启 1:关闭
   animation3D.cm3DRuingLightState           = 1; // 车头中间贯穿灯:0:OFF, 1:ON
   animation3D.cm3DRuingSecondaryLightState  = 3; // 前左右日行灯信号 0:OFF, 1:left ON, 2:right ON, 3:both ON
   animation3D.cm3DRearPositionLightState    = 1; // 后位置灯灯信号 0:OFF, 1:ON （默认点亮）

   /** @brief 用例:近光灯亮 */
   animation3D.cm3DLowBeamState              = 1; // 近光灯 0:OFF, 1:ON
   animation3D.cm3DNearFarLightState         = 0; // 远光灯 0:OFF, 1:ON

   /** @brief 用例:远近大灯亮 */
   animation3D.cm3DLowBeamState              = 1; // 近光灯 0:OFF, 1:ON
   animation3D.cm3DNearFarLightState         = 1; // 远光灯 0:OFF, 1:ON
```

### 2024-11-20
1. FX11-A2车型库构建环境和E335车型平台保持一致
2. CarModel3D::Initialization接口初始化资源目录传入*avm_resource/image/carmodel3d/junlian/FX11_A2*
```c++
   /** @brief 用例:FX11-A2初始化 */
   carModel3D.Initialization("avm_resource/image/carmodel3d/junlian/FX11_A2", funcLog);
```
1. 车模使用glTF格式加载，纹理转为PVR格式文件
2. 支持四轮转动动画，前轮转向动画
3. 支持四门两盖开关动画，后视镜折叠动画
4. 支持车窗开度动画，开度范围和E335一致
5. 支持车模基本透明效果，透明相关设置项和E335保持一致
<font color="red">注：当前版本暂不支持灯光设置</font>