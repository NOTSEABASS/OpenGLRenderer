#include <runtime/function/input_management.h>

InputInfo::InputInfo() 
{
    mouse_offset_x      = 0;
    mouse_offset_y      = 0;
    mouse_scroll_x      = 0;
    mouse_scroll_y      = 0;
    mouse_button_left   = 0;
    mouse_button_right  = 0;
    mouse_last_x        = 0;
    mouse_last_y        = 0;
}

void InputInfo::Update()
{
    mouse_offset_x  = mouse_pos_x - mouse_last_x;
    mouse_offset_y  = mouse_last_y - mouse_pos_y;
    mouse_last_x    = mouse_pos_x;
    mouse_last_y    = mouse_pos_y;
}