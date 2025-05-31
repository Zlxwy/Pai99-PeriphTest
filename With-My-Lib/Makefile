CROSS_COMPILE	:= loongarch64-linux-gnu-
CC				:= $(CROSS_COMPILE)gcc
CXX				:= $(CROSS_COMPILE)g++
TARGET_PATH		?= ./
TARGET_NAME		?= Exxx

MAIN_FUNC_FILE	:= ./User/main.cpp

# 在这里列举所有包含hpp头文件的目录，必须以斜杆'/'结尾
INC_DIRS := \
./Lib/inc/ \
./TestCode/inc/ \
./User/ \
/home/root/opencv_4_10_build/include/opencv4/
# end list

# 在这里列举所有包含cpp源文件的目录，必须以斜杆'/'结尾（就不用输入main函数文件的路径了）
SRC_DIRS := \
./Lib/src/ \
./TestCode/src/
# end list

# 在这里列举库文件的搜索路径（不区分动态库和静态库）
LIB_DIRS := \
/home/root/opencv_4_10_build/lib/
# end list

# 在这里列举要链接的库文件（不区分动态库和静态库）
LIB_LINKS := \
opencv_core \
opencv_imgproc \
opencv_highgui \
opencv_videoio \
opencv_imgcodecs \
pthread
# end list

# 函数$(patsubst)会把" -I %"中的%替换成$(INC_DIRS)的条目
# 函数$(patsubst)会对$(INC_DIRS)中的每个条目单独执行此操作。
# 结果: INCLUDE := -I ./Lib  -I ./User
INCLUDE := $(patsubst %, -I %, $(INC_DIRS))

# 函数$(foreach var,list,text): var会遍历list的每一个条目，并扩展text，
# 即$(list1)$(text) $(list2)$(text) ......
# 函数$(wildcard pattern)：查找出所有符合pattern的文件名。
# 结果: SOURCE := ./Lib/pwm_led_breath.cpp  ./User/main.cpp
SOURCE := $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)*.cpp))

# 函数$(patsubst)会把" -L%"中的%替换成$(LIB_DIRS)的条目
# 函数$(patsubst)会对$(LIB_DIRS)中的每个条目单独执行此操作。
# 结果: LIBRARY := -L/opt/opencv_4_10_build/lib/
LIBRARY := $(patsubst %, -L%, $(LIB_DIRS))

# 函数$(patsubst)会把" -l%"中的%替换成$(LIB_LINKS)的条目
# 函数$(patsubst)会对$(LIB_LINKS)中的每个条目单独执行此操作。
# 结果: LINK := -lopencv_core  -lopencv_imgproc  -lopencv_highgui  -lopencv_videoio  -lopencv_imgcodecs  -lpthread
LINK := $(patsubst %, -l%, $(LIB_LINKS))

$(TARGET_PATH)$(TARGET_NAME): $(SOURCE)
	$(CXX)  $(MAIN_FUNC_FILE)  $(SOURCE)  $(INCLUDE)  $(LIBRARY)  $(LINK)  -o $(TARGET_PATH)$(TARGET_NAME)
# 龙芯编译器  主函数文件.cpp      其他源文件.cpp... -I 头文件搜索路径 -L库文件搜索路径 -l库文件名称 -o 输出路径/可执行文件名

clean:
	rm $(TARGET_PATH)$(TARGET_NAME)