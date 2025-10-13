# Learn OpenGL (Version 3.3)

这是学习OpenGL所做的练习，Win10+OpenGL3.3+Visual Studio 2022，内容主要源自于：[LearnOpenGL CN](https://learnopengl-cn.github.io/)  

## Attention  

使用VS运行项目的时候，请设置 `解决方案->属性->配置启动项目` 为 `当前选择`  

## How projects organized  

我使用visual studio 2022，将所有知识点的练习都放在一个solution下，这样方便整合全部内容，所有的练习工程都以`OpenGL*`作为前缀（可运行），所有的外部依赖都以`dep`作为前缀（不可运行）。  

项目目录下已经包含开发所需要的库文件（OpenGL_lib），项目中的`GLFW`和`glad`库有可能不适配你的电脑显卡，项目中使用的是3.3版本，如果你是40系显卡可能需要升级，如若无法运行项目需要自行下载替换：  

GLFW和glad的详细说明可以参考：[LearnOpenGL CN - 创建窗口](https://learnopengl-cn.github.io/01%20Getting%20started/02%20Creating%20a%20window/#glfw)  

[GLFW下载页](http://www.glfw.org/download.html)（建议自己按照教程自行编译一个静态库使用，替换OpenGL_lib/lib下的glfw3.lib和GLFW内的文件）  
[glad在线服务](http://glad.dav1d.de/)  
[glm官网(0.9.8)](https://glm.g-truc.net/0.9.8/index.html)  

数学库（glm）使用的是和教程一致的0.9.8版本，从0.9.9版本开始，glm默认创建的矩阵就是零矩阵了，而不是单位矩阵，如果想创建单位矩阵需要这样初始化：  

```c++
// glm version >= 0.9.9
glm::mat4 mat = glm::mat4(1.0f);
```

正常来说，`glad.c`和`stb_image.h`应该包含在项目源码中，我只是图个方便，将一些东西分离出来了（比如将glad编译为静态库）。  

## How to add a new project  

在解决方案`LearnOpenGL.sln`上新增一个项目，配置`头文件路径`和`库文件路径`，添加需要链接的库，同时要设置好项目依赖，这些都可以参考别的项目配置怎么写的。注意配置中用到的一个宏，`$(solutionDir)`，它代表当前的解决方案路径，即：`xxx\xxx\LearnOpenGL\`。  

## Camera  

在入门部分，制作摄像机章节，涉及到的推导比较多（MVP矩阵），不好理解，可以观看B站games101课程，在此也推荐几篇博客：  
[[图形学笔记]推导投影矩阵（知乎@小猫耳朵）](https://zhuanlan.zhihu.com/p/122411512)  
[视图变换和投影变换矩阵的原理及推导，以及OpenGL，DirectX和Unity的对应矩阵（知乎@江玉荣）](https://zhuanlan.zhihu.com/p/362713511)  
[深入理解透视矩阵（By zhyingkun）](https://www.zhyingkun.com/perspective/perspective/)  

## assimp  

转换模型数据文件为opengl可以识别的形式  

也许你需要自行编译库文件：[assimp](https://github.com/assimp/assimp/blob/master/Build.md)

添加.lib链接库，然后将动态库`assimp-vc142-mt.dll`添加到vs的资源文件目录下，就不用手动拷贝到可执行文件的目录了。  

### OpenGL Build-in variables

reference:  
[Advanced GLSL-CN](https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/08%20Advanced%20GLSL/)  
[Advanced GLSL-EN(origin)](https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL)

### Uniform Buffer Object (UBO)

```c++
layout (std140) uniform ExampleBlock
{
                     // 基准对齐量       // 对齐偏移量
    float value;     // 4               // 0 
    vec3 vector;     // 16              // 16  (必须是16的倍数，所以 4->16)
    mat4 matrix;     // 16              // 32  (列 0)
                     // 16              // 48  (列 1)
                     // 16              // 64  (列 2)
                     // 16              // 80  (列 3)
    float values[3]; // 16              // 96  (values[0])
                     // 16              // 112 (values[1])
                     // 16              // 128 (values[2])
    bool boolean;    // 4               // 144
    int integer;     // 4               // 148
}; 
```

对齐偏移量是基准对齐量的整数倍，是变量的起始位置，在std140布局中递增。  

只要记住整数倍，递增，和基准对齐量这三点，就能计算出每个变量位置和接口块总大小，上面的示例接口块总大小为152字节（148+4）。

使用方式参考篇章图解：[使用Uniform缓冲](https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/08%20Advanced%20GLSL/#uniform_2)

大体流程是绑定shader uniform block和UBO到同一个绑定点，然后将数据拷贝到（设置到）UBO中。
