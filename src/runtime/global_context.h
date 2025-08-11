#pragma once

#include <string>

class RuntimeGlobalContext
{
public:
    // create all global systems and initialize these systems
    void startSystems(const std::string& config_file_path);
    // destroy all global systems
    void shutdownSystems();

public:
    std::string config_file_path;
};

extern RuntimeGlobalContext g_runtime_global_context;