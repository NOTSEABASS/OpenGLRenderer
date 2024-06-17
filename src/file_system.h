#pragma once

#include <singleton_util.h>
#include <filesystem>
#include <string>
#include <assert.h>
#include <vector>

namespace fs = std::filesystem;
class FileSystem : public Singleton<FileSystem>
{
public:
    FileSystem();
    static const fs::path GetProjectPath()      { return run_path.parent_path().parent_path(); }
    static const fs::path GetContentPath()      
    {
        auto path =  GetProjectPath() / "content";
        assert(fs::exists(path));
        return path; 
    }
    static const fs::path GetEditorPath()       
    { 
        auto path =  GetProjectPath() / "editor";
        assert(fs::exists(path));
        return path; 
    }
    static std::vector<fs::path> GetRootPaths();
private:
    static std::vector<fs::path> root_paths;
    static const fs::path run_path;
};