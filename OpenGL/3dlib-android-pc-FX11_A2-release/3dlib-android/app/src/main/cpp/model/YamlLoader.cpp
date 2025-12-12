/**************************************************************************************************
 * @file YamlLoader.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-13
 * 
 * @copyright Copyright (c) 2023
 * 
**************************************************************************************************/

#include "YamlLoader.hpp"

namespace BaseLib
{

    YamlLoader::YamlLoader() {

    }

    YamlLoader::YamlLoader(std::string filename):filename(filename) {

    }

    YamlLoader::~YamlLoader() {

    }

    YAML::Node YamlLoader::loadYamlNode() {
        YAML::Node node;
        try {
            node = YAML::LoadFile(this->filename);
        } catch (YAML::Exception& e) {
            std::cerr << "YAML Exception: " << e.what() << std::endl;
        }

        return node;
    }

    std::vector<YAML::Node> YamlLoader::loadYamlAllNodes() {
        std::vector<YAML::Node> nodes;
        try {
            nodes = YAML::LoadAllFromFile(this->filename);
        } catch (YAML::Exception& e) {
            std::cerr << "YAML Exception: " << e.what() << std::endl;
        }

        return nodes;
    }
    
} // namespace BaseLib
