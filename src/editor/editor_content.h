#pragma once

#include <map>
#include <string>

class Texture2D;

using namespace std;

static class EditorContent
{
public:
    static map<string, Texture2D*> editor_tex;
};