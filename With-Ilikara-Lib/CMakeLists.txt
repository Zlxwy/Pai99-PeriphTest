cmake_minimum_required(VERSION 3.16)    # 限制最低CMake版本

set(CROSS_COMPILER_DIR /opt/loongson-gnu-toolchain-8.3-x86_64-loongarch64-linux-gnu-rc1.6)
set(CMAKE_C_COMPILER ${CROSS_COMPILER_DIR}/bin/loongarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILER_DIR}/bin/loongarch64-linux-gnu-g++)
set(TARGET_NAME Emain) # 制定生成的可执行文件名称

project(main) # 项目名称

# 列举所用的源文件路径
aux_source_directory(${PROJECT_SOURCE_DIR}/Lib/src/ SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/TestCode/src/ SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/User/ SRC)

# 列举所用的头文件路径
include_directories(${PROJECT_SOURCE_DIR}/Lib/inc/)
include_directories(${PROJECT_SOURCE_DIR}/TestCode/inc/)
include_directories(${PROJECT_SOURCE_DIR}/User/)
include_directories(/home/root/opencv/include/opencv4/)

# 找到opencv所需的库文件并保存到变量中
find_library(OPENCV_CORE      opencv_core      /home/root/opencv/lib/)
find_library(OPENCV_IMGPROC   opencv_imgproc   /home/root/opencv/lib/)
find_library(OPENCV_HIGHGUI   opencv_highgui   /home/root/opencv/lib/)
find_library(OPENCV_VIDEOIO   opencv_videoio   /home/root/opencv/lib/)
find_library(OPENCV_IMGCODECS opencv_imgcodecs /home/root/opencv/lib/)

# 若需使用静态库，可带上下面两行程序，具体可参考龙邱B站视频教程
# link_directories(${PROJECT_SOURCE_DIR}/a)
# link_libraries(ss)

# 由源文件生成一个可执行程序，并链接到所需的库文件
add_executable(${TARGET_NAME} ${SRC})
target_link_libraries(${TARGET_NAME} PUBLIC
    ${OPENCV_CORE}
    ${OPENCV_IMGPROC}
    ${OPENCV_HIGHGUI}
    ${OPENCV_VIDEOIO}
    ${OPENCV_IMGCODECS}
    pthread
)
