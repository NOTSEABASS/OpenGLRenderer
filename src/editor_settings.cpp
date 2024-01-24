#include <editor_settings.h>

bool EditorSettings::UsePostProcess = true;
bool EditorSettings::UsePolygonMode = false;
bool EditorSettings::DrawGizmos     = true;
std::vector<WindowSize> EditorSettings::window_size_list = {    WindowSize(800, 600),
                                                                WindowSize(1024, 768),
                                                                WindowSize(1200, 900),
                                                                WindowSize(1280, 720),
                                                                WindowSize(1280, 800),
                                                                WindowSize(1280, 960),
                                                                WindowSize(1280, 1024),
                                                                WindowSize(1440, 1080),
                                                                WindowSize(1600, 900),
                                                                WindowSize(1600, 1200),
                                                                WindowSize(1920, 1080),
                                                                WindowSize(1920, 1200),
                                                                WindowSize(1920, 1440),
                                                                WindowSize(2560, 1440),
                                                                WindowSize(2560, 1600),
                                                                WindowSize(2880, 1620),
                                                                WindowSize(3840, 2160) };
                                                    