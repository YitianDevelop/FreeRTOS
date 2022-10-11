## FREE_RTOS 源码分析

### 关于

仓库克隆自FreeRTOS官方仓库 [GitHub - FreeRTOS](https://github.com/FreeRTOS/FreeRTOS)，为方便阅读分析，做了大量精简，主要精简在以下方面

1. 移除大量FreeRTOS对于不同平台的适配文件

2. 移除大量Demo示例文件，只保留了Posix_Gcc以及WIN32-MingW以及WIN32-MSVC(除了Posix_GCC保证能编译通过外，其他平台不保证能编译通过)

### Build&Run

FreeRTOS的目标设备为小型CPU设备，但对于实现了POSIX接口的操作系统(不限于)，可以模拟运行。

#### Linux系或者MACOS

```shell
cd FreeRTOS/Demo/Posix_GCC
make
```

官方文档见: FreeRTOS/Demo/Posix_GCC/FreeRTOS-simulator-for-Linux.url

#### Windows

理论上利用MingW GCC编译器可以编译通过，或者利用MSVC编译

##### MingW- GCC

```shell
cd FreeRtOS/Demo/Posix_GCC
make 
```

##### MSVC

用Visval Studio打开  FreeRTOS/Demo/WIN32-MSVC/WIN32.vcxproj 项目即可