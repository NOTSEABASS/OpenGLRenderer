#pragma once
#include <string>
#include <vector>

struct WindowSize
{
    unsigned int width;
    unsigned int height;

    WindowSize(int x, int y)
    {
        width = x;
        height = y;
    }

    std::string to_string()
    {
        return std::to_string(width) + "x" + std::to_string(height);
    }
};

enum EGBuffer
{
    Fragpos = 0,
    Normal = 1,
    Depth = 2
};

static class EditorSettings
{
public:
    static bool UseWireframeMode;
    static bool UsePostProcess;
    static bool DrawGizmos;
    static std::vector<WindowSize> window_size_list;
    static std::vector<std::string> GBuffers;
    static EGBuffer CurrentRenderBuffer;
    static bool UsePreviewGBuffer;
};