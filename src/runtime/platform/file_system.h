#pragma once

#include <runtime/core/instance_util.h>
#include <filesystem>
#include <string>
#include <assert.h>
#include <vector>

namespace fs = std::filesystem;
class FileSystem : public Singleton<FileSystem>
{
public:
    FileSystem();

    void InitializeSystem(const std::string runtime_path);
    static const fs::path GetProjectPath()      { return GetRuntimePath().parent_path().parent_path().parent_path(); }
    static const fs::path GetContentPath()      
    {
        auto path =  GetProjectPath() / "content";
        if (!fs::exists(path)) {
            std::cerr << "Content path does not exist: " << path << std::endl;
            assert(false);
        }
        return path; 
    }
    static const fs::path GetEditorPath()       
    { 
        auto path =  GetProjectPath() / "editor";
        assert(fs::exists(path));
        return path; 
    }
    static const fs::path GetRuntimePath(){ 
        return GetInstance()->run_path; 
    };
    static std::vector<fs::path> GetRootPaths();
private:
    bool isInitialized = false; 
    static std::vector<fs::path> root_paths;
    fs::path run_path;
};