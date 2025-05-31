#!/bin/bash

RED_TEXT="\033[31m"
GREEN_TEXT="\033[32m"
YELLOW_TEXT="\033[33m"
BLUE_TEXT="\033[34m"
COLOR_RST="\033[0m"

# 指定构建的文件夹名称、要发送的可执行文件的名称、发送到久久派后的名称、久久派的IP地址
MAKED_PATH="./"
MAKED_NAME="Emain"
PAI99_RECV_NAME="Eocv_ctrl_servo"
PAI99_IP_ADDR="192.168.43.245"

if [ -f "${MAKED_PATH}${MAKED_NAME}" ]; then
    rm "${MAKED_PATH}${MAKED_NAME}"
    echo -e "${YELLOW_TEXT}MyInfo > 原可执行文件 ${MAKED_PATH}${MAKED_NAME} 已被删除。${COLOR_RST}"
else
    echo -e "${YELLOW_TEXT}MyInfo > 原可执行文件 ${MAKED_PATH}${MAKED_NAME} 不存在，将直接进行构建。${COLOR_RST}"
fi

echo -e "${GREEN_TEXT}MyInfo > 开始进行构建。${COLOR_RST}"
make TAGET_PATH=${MAKED_PATH} TARGET_NAME=${MAKED_NAME}
if [ $? -ne 0 ]; then
    echo -e "${RED_TEXT}MyInfo > 未能构建成功。${COLOR_RST}"
    exit 1
else
    echo -e "${GREEN_TEXT}MyInfo > 构建完成，开始传输。${COLOR_RST}"
fi

scp ${MAKED_PATH}${MAKED_NAME} root@${PAI99_IP_ADDR}:~/myExecs/${PAI99_RECV_NAME}
if [ $? -ne 0 ]; then
    echo -e "${RED_TEXT}MyInfo > SSH传输失败，请检查网络连接或IP地址。${COLOR_RST}"
    exit 1
else
    echo -e "${GREEN_TEXT}MyInfo > SSH传输成功，已将 ${MAKED_NAME} 传输为久久派的 ~/myExecs/${PAI99_RECV_NAME} 文件${COLOR_RST}"
fi

# 脚本结束