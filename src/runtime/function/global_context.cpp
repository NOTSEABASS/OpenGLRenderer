#include "global_context.h"
#include <runtime/platform/file_system.h>
#include <iostream>

void RuntimeGlobalContext::startSystems(const std::string &config_file_path)
{
    std::cout << "Starting runtime global context..." << std::endl;
    this->config_file_path = config_file_path;
    FileSystem::GetInstance()->InitializeSystem(config_file_path);
}

void RuntimeGlobalContext::shutdownSystems()
{
}
