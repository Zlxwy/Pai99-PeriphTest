#!/bin/bash

RED_TEXT="\033[31m"
GREEN_TEXT="\033[32m"
YELLOW_TEXT="\033[33m"
BLUE_TEXT="\033[34m"
COLOR_RST="\033[0m"

# 指定构建的文件夹名称、要发送的可执行文件的名称(必须和CMakeLists.txt文件中的保持一致)、发送到久久派后的名称、久久派的IP地址
BUILD_FOLDER="Build"
CMAKED_NAME="Emain"
PAI99_RECV_NAME="Epwm_out"
PAI99_IP_ADDR="192.168.0.198"

# 检查构建文件夹是否存在，不存在的话就创建这个文件夹
if [ ! -d "./${BUILD_FOLDER}" ]; then
    echo -e "${YELLOW_TEXT}MyInfo > 构建文件夹不存在，将创建 ${BUILD_FOLDER} 文件夹。${COLOR_RST}"
    mkdir ${BUILD_FOLDER}
fi

# 在构建目录下开始构建，传输到久久派
cmake -B ./${BUILD_FOLDER}/
echo -e "${GREEN_TEXT}MyInfo > 已创建 ${BUILD_FOLDER} 文件夹，开始进行构建。${COLOR_RST}"
cmake --build ./${BUILD_FOLDER}/
if [ $? -ne 0 ]; then
    echo -e "${RED_TEXT}MyInfo > 未能构建成功。${COLOR_RST}"
    exit 1
else
    echo -e "${GREEN_TEXT}MyInfo > 构建完成，开始传输。${COLOR_RST}"
fi

scp ./${BUILD_FOLDER}/${CMAKED_NAME} root@${PAI99_IP_ADDR}:~/myExecs/${PAI99_RECV_NAME}
if [ $? -ne 0 ]; then
    echo -e "${RED_TEXT}MyInfo > SSH传输失败，请检查网络连接或IP地址。${COLOR_RST}"
    exit 1
else
    echo -e "${GREEN_TEXT}MyInfo > SSH传输成功，已将 ${CMAKED_NAME} 传输为久久派的 ~/myExecs/${PAI99_RECV_NAME} 文件。${COLOR_RST}"
fi

# 脚本结束
