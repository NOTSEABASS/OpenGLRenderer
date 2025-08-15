#include <runtime/platform/file_system.h>

FileSystem::FileSystem()
{
}

void FileSystem::InitializeSystem(const std::string runtime_path) {
    if (!isInitialized)
        run_path = runtime_path;
};

std::vector<fs::path> FileSystem::root_paths;

std::vector<fs::path> FileSystem::GetRootPaths()
{
    if (root_paths.size() == 0)
    {
        for(int i = 0; i < 26; i++)
        {
            char root_symbol = 'A' + i;
            std::string tmp_path = "";
            tmp_path = tmp_path + root_symbol + "://";

            fs::path p(tmp_path);
            if (fs::exists(p))
            {
                root_paths.push_back(p);
            }
        }
    }
    return root_paths;
}