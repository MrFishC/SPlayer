cmake_minimum_required(VERSION 3.22.1)

#添加头文件路径（相对于本文件路径）
include_directories(include)

project("splayer")

find_library(
        log-lib
        log)

#设置环境变量         结合include_directories(include)，最后在target_link_libraries中添加静态库或动态库的名字，就可以实现引入第三方静态库或动态库
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}")

#创建并命名一个库（构建一个原生库）
#add_library中的第一个参数不能随便定义
#在这个顶级CMakeLists.txt中，${CMAKE_PROJECT_NAME}用于定义目标库名称；
add_library(${CMAKE_PROJECT_NAME} SHARED
        native-lib.cpp
        IDemux.cpp
        XData.cpp
        FFDemux.cpp
        XLog.cpp
        XThread.cpp
        IObserver.cpp
        IDecode.cpp
        FFDecode.cpp
        XParameter.cpp
        IVideoView.cpp
        GLVideoView.cpp
        XTexture.cpp
        XEGL.cpp
        XShader.cpp
        FFResample.cpp
        IResample.cpp
        SLAudioPlay.cpp
        IAudioPlay.cpp
        IPlayer.cpp
        IPlayerBuilder.cpp
        FFPlayerBuilder.cpp
        IPlayerPorxy.cpp
)

target_link_libraries(${CMAKE_PROJECT_NAME}
        avcodec
        avformat
        avfilter
        avutil
        swresample
        swscale
        android
        OpenSLES
        GLESv2
        EGL
        log)