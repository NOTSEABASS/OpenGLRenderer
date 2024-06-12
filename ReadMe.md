This is a tiny-renderer project written in c++ based on OpenGL

<img src="/img/sample_gif_1.gif" style="zoom:50%;" />

<img src="/img/renderer-preview.gif" style="zoom:50%;" />

## UpdateInfo
Update 1.2.4
- A totally usable SSAO
- Move render result to a imgui panel
- fix some problems

Update 1.2.3
- Add a simple SSAO

Update 1.2.2
- Add PostProcess Move Up/Down Button
  
Update 1.2.1
- Add a bloom post process
- Remove the console window

Update 1.2.0
- Add simple console system
- Replace debug log from std::cout to console log item
- Fix shadow bias
- Fix gizmos in polygonmode

Update 1.1.0
- Add infinite grid as gizmos
- Add light debug cube
- Add scene object coordinate axes
- Add material choosing combo

<img src="/img/grid.jpg" style="zoom: 50%;" />

## Build

### windows

build with cmake

```bash
cd build
cmake ..
cmake --build . --config Release/Debug
```

tips: this project is based on openGL and Assimp, so you need to build them first

## Problems
### Flashback
If you cannot run the application, please check whether your renderer can find the 'content' or 'editor' path.
The renderer will get root path based on std::filesystem::currentpath() which may not find your content
path correctly(For now, there won't be any installation steps for the renderer, so the)

## Reference
Render Engine:

https://learnopengl.com

https://learnopengl-cn.github.io

Inifinite-grid:

https://zhuanlan.zhihu.com/p/647926704
