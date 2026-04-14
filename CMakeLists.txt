cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME CableLength)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
# 1. 告诉 CMake 这是一个没有操作系统的裸机环境
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 2. 【核心修复指令】强制 CMake 在测试编译器时只编译静态库，跳过链接阶段！
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 3. 这行是您原本就有的项目名称声明，上面那三行必须在它前面！
project(CableLength C CXX ASM)
project(${CMAKE_PROJECT_NAME})
# 告诉 GCC 编译器：输入的源码文件是 GB2312 编码，且编译进单片机的执行字符集也是 GB2312
add_compile_options(-finput-charset=GB2312 -fexec-charset=GB2312)
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        Core/users/tjcHMI.h
        Core/users/tjcHMI.c)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        Core/users/delay.c
        Core/users/FDC2214.c
        Core/users/myiic.c
        Core/users/sys.c

    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        Core/users
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)




# 设置浮点运算模式（根据MCU选择）
# 对于没有硬件FPU的MCU使用soft
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=soft")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfloat-abi=soft")

# 对于有硬件FPU的MCU（如F4系列）使用
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfloat-abi=hard -mfpu=fpv4-sp-d16")

# 使能printf和scanf浮点数支持
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -u_printf_float -u_scanf_float)

# 可选：使用nano库减小代码体积
# target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
#     --specs=nano.specs
#     --specs=nosys.specs
#)
