/**************************************************************************************************
 * @file ImageHelper.hpp
 * @brief image helper.
 * 
 * @details
 * 
 *  HISTORY
 *  -----------------------------------------------------------------------------------------------
 *  Version   Date        Author    Description
 *  -----------------------------------------------------------------------------------------------
 *  0.1       2023-05-11  TDZ       Initial.
 * 
 * 
**************************************************************************************************/

#pragma once

#include <string>
#include <cstring>

#include <Logger.hpp>

#include "../thirdparty/stb/include/stb_image.h"
#include "../thirdparty/stb/include/stb_image_write.h"

namespace BaseLib{

class ImageHelper
{
    public:
    static void Save2image(std::string filepath, unsigned char *data, int width, int height, int channel, bool isFlip = true)
    {
        LOGGER_I("Save2image begin filepath=%s, width=%d, height=%d, channel=%d, isFlip=%d", filepath.c_str(), width, height, channel, isFlip);
        
        // 创建一个临时缓冲区来存储RGB数据（stb_image_write需要连续的RGB数据）
        unsigned char* rgb_data = (unsigned char*)malloc(width * height * 3);
        
        // 将输入数据转换为RGB格式
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                int src_idx = channel * (i * width + j); // 源数据索引
                int dst_idx = 3 * (i * width + j);      // 目标RGB数据索引
                
                // 复制RGB数据（原数据是RGBA格式，我们只需要RGB）
                rgb_data[dst_idx]     = data[src_idx];     // R
                rgb_data[dst_idx + 1] = data[src_idx + 1]; // G
                rgb_data[dst_idx + 2] = data[src_idx + 2]; // B
            }
        }
        
        // 如果需要翻转Y轴
        if (isFlip) {
            flipYAxis(rgb_data, width, height, 3);
        }
        
        // 使用stb_image_write保存为PNG格式
        stbi_write_png(filepath.c_str(), width, height, 3, rgb_data, width * 3);
        
        // 释放临时缓冲区
        free(rgb_data);
    }

    static void flipYAxis(uint8_t * buf, int width, int height, int channels)
    {
        // LOGGER_I("flipYAxis begin width=%d, height=%d", width, height);
        int totalLength   = height * width * channels;
        int oneLineLength = width * channels;
        uint8_t* tmp      = (uint8_t*)malloc(height * width * channels);

        memcpy(tmp, buf, width*height*channels);
        memset(buf,0,sizeof(uint8_t)*height*width*channels);


        for(int i = 0; i < height;i++) {
            memcpy(buf + oneLineLength * i, tmp + totalLength - oneLineLength * (i+1), oneLineLength);
        }

        free(tmp);
    }
};

};
