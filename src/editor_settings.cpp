#include <editor_settings.h>

bool EditorSettings::UsePostProcess = false;
bool EditorSettings::UsePolygonMode = false;
WindowSize EditorSettings::window_size_list[5] = {   WindowSize(800, 600),
                                                    WindowSize(1200, 900),
                                                    WindowSize(1920, 1080),
                                                    WindowSize(2880, 1620),
                                                    WindowSize(3840, 2160) };
                                                    