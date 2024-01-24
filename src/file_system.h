#pragma once

#include <singleton_util.h>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;
class FileSystem : public Singleton<FileSystem>
{
public:
    FileSystem();
    static const fs::path GetProjectPath()      { return run_path.parent_path(); }
    static const fs::path GetContentPath()      { return GetProjectPath() / "content"; }
    static const fs::path GetEditorPath()       { return GetProjectPath() / "editor"; }
    static std::vector<fs::path> GetRootPaths();
private:
    static std::vector<fs::path> root_paths;
    static const fs::path run_path;
};