/**************************************************************************************************
 * @file YamlLoader.hpp
 * @brief 
 * @version 0.1
 * @date 2023-07-13
 * 
 * @copyright Copyright (c) 2023
 * 
**************************************************************************************************/
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace BaseLib
{
    class YamlLoader {
    public:
        YamlLoader();
        YamlLoader(std::string filename);
        virtual ~YamlLoader();
    public:
        YAML::Node loadYamlNode();
        std::vector<YAML::Node> loadYamlAllNodes();
    public:
        std::string filename;  /*!< config yaml filename */
    };

} // namespace BaseLib

