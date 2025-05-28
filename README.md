# 目录
* [如何构建这个项目？](#如何构建这个项目?)
  * [构建方式一 (推荐)](#构建方式一 (推荐))
  * [构建方式二](#构建方式二)
* [如何传输至板卡](#如何传输至板卡)

# 如何构建这个项目？

## 构建方式一、使用 CMake 进行构建 {#构建方式一 (推荐)}

> doCmake.sh 脚本文件依赖于 Cmake 进行构建，构建完成后会直接传输到久久派上。

### CMakeList.txt 设置
1. 打开 <font color="red">*CMakeLists.txt*</font> 文件
2. 在 **CROSS_COMPILER** 指定所用的编译器
3. 在 **TARGET_NAME** 处指定生成的可执行文件名称
4. 列举所有源文件、头文件存放的路径
5. 链接库文件

### doCmake.sh 设置
6. 打开 <font color="red">*doCmake.sh*</font> 文件
7. 在 **BUILD_FOLDER** 指定 Cmake 构建文件夹的名称
8. 在 **CMAKED_NAME** 指定即将发送的可执行文件的名称，和 <font color="red">*CMakeLists.txt*</font> 中的 **TARGET_NAME** 保持一致
9. 在 **PAI99_RECV_NAME** 指定发送到久久派后的可执行文件的名称（改个名）
10. 电脑和久久派要连接在同一个网段下，在 **PAI99_IP_ADDR** 指定久久派的 IP 地址

### 最终操作
11. 在项目根目录的终端下运行命令即可
```bash
./doCmake.sh
```

## 构建方式二、使用 Makefile 进行构建 {#构建方式二}

> domake.sh 脚本文件依赖于 Makefile 进行构建，构建完成后也会直接传输到久久派上。

### Makefile 设置
1. 打开 <font color="red">*Makefile*</font> 文件
2. 在 **CROSS_COMPILE** 指定使用的编译器
3. 在 **MAIN_FUNC_FILE** 指定 main 函数所在的 CPP 文件
4. 列举所有头文件路径、源文件路径（不用再包含 main 函数所在的源文件了）、库文件搜索路径、链接库文件名

### domake.sh 设置
5. 在 **MAKED_PATH** 指定生成可执行文件的路径
6. 在 **MAKED_NAME** 指定生成可执行文件的名称
7. 在 **PAI99_RECV_NAME** 指定发送到久久派后的可执行文件的名称（改个名）
8. 电脑和久久派要连接在同一个网段下，在 **PAI99_IP_ADDR** 指定久久派的 IP 地址

### 最终操作
9. 在项目根目录的终端下运行命令即可
```bash
./domake.sh
```

# 如何传输至板卡
在执行 ./doCmake.sh 或 ./domake.sh 后，就已经包含传输的操作了。