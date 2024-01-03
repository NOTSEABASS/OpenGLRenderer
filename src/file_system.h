#pragma once

#include <instance_util.h>
#include <filesystem>
#include <string>

class FileSystem : public Singleton<FileSystem>
{
public:
    FileSystem()                                    { run_path = std::filesystem::current_path(); }
    static std::filesystem::path GetProjectPath()   { return run_path.parent_path().parent_path(); }
    static std::filesystem::path GetContentPath()   { return GetProjectPath() / "content"; }
private:
    static std::filesystem::path run_path;
};