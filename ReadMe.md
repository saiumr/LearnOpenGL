# Learn OpenGL (Version 3.3)

这是学习OpenGL所做的练习，内容主要源自于：[LearnOpenGL CN](https://learnopengl-cn.github.io/)  

## How projects organized  

我使用visual studio 2022，将所有知识点的练习都放在一个solution下，这样方便整合全部内容，所有的练习工程都以`OpenGL*`作为前缀（可运行），所有的外部依赖都以`dep`作为前缀（不可运行）。  

项目目录下已经包含开发所需要的库文件（OpenGL_lib），项目中的`GLFW`和`glad`库有可能不适配你的电脑，如若无法运行项目需要自行下载替换：  

[GLFW下载页](http://www.glfw.org/download.html)  
[glad在线服务](http://glad.dav1d.de/)  

详细说明可以参考：[LearnOpenGL CN - 创建窗口](https://learnopengl-cn.github.io/01%20Getting%20started/02%20Creating%20a%20window/)  

正常来说，`glad.c`和`stb_image.h`应该包含在项目源码中，我只是图个方便，将一些东西分离出来了（比如将glad编译为静态库）。  

## How to add a new project  

在解决方案`LearnOpenGL.sln`上新增一个项目，配置`头文件路径`和`库文件路径`，添加需要链接的库，同时要设置好项目依赖，这些都可以参考别的项目配置怎么写的。注意配置中用到的一个宏，`$(solutionDir)`，它代表当前的解决方案路径，即：`xxx\xxx\OpenGL_lib`。  
