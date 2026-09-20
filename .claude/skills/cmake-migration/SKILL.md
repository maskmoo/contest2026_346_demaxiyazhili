# NuttX/openvela CMake 构建迁移技能

## 描述
将 NuttX/openvela 的 Make 构建系统迁移到 CMake，或维护双构建系统的一致性。

## 适用场景
- 为新芯片/板卡同时维护 Make 和 CMake 构建
- 排查 Make 与 CMake 编译产物差异
- 理解 openvela CMake 构建框架

## openvela CMake 构建框架

### 构建入口
```bash
# Make 方式
./build.sh <board>/configs/<config> -j8

# CMake 方式
./build.sh <board>/configs/<config> --cmake -j8
```

### 层级结构
```
nuttx/CMakeLists.txt              # 顶层
├── arch/risc-v/src/CMakeLists.txt # 架构层
├── boards/CMakeLists.txt          # 板级
└── vendor/<vendor>/               # 厂商代码
    ├── chips/<chip>/CMakeLists.txt
    └── boards/<board>/CMakeLists.txt
```

## 迁移检查清单

### 1. 芯片层 CMakeLists.txt
对标 `Make.defs`，确保源文件列表一致：

```cmake
# 对标 Make.defs 中的 CSRCS
set(SRCS
    gd32vw55x_start.c
    gd32vw55x_irq.c
    gd32vw55x_serial.c
    gd32vw55x_clockconfig.c
    # ...
)

# 条件编译（对标 ifeq ($(CONFIG_xxx),y)）
if(CONFIG_GD32VW55X_SPI)
    list(APPEND SRCS gd32vw55x_spi.c)
endif()

target_sources(chip PRIVATE ${SRCS})
```

### 2. 板级 CMakeLists.txt
```cmake
# 对标 board Make.defs
set(BOARD_SRCS
    gd32_boot.c
    gd32_bringup.c
    gd32_appinit.c
    gd32_spi.c
    gd32_lcd.c
)

target_sources(board PRIVATE ${BOARD_SRCS})
```

### 3. SDK 集成（CMake 特殊处理）
SDK 头文件需要延迟引入，使用 `cmake_language(DEFER)`：

```cmake
# Sdk.cmake
cmake_language(DEFER DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    CALL _include_sdk_headers)

macro(_include_sdk_headers)
    target_include_directories(chip PRIVATE
        ${SDK_PATH}/MAC/Include
        ${SDK_PATH}/platform/GD32VW55x
    )
endmacro()
```

### 4. 预编译库链接
```cmake
# 使用 --start-group/--end-group 解决循环依赖
target_link_libraries(chip INTERFACE
    -Wl,--start-group
    ${SDK_PATH}/lib/libwifi.a
    ${SDK_PATH}/lib/libble_max.a
    -Wl,--end-group
)
```

## 常见差异排查

### 产物大小不同
```bash
# 比较 .config
diff nuttx/.config cmake_build/.config

# 比较 map 文件
grep "\.text" nuttx/nuttx.map | head
grep "\.text" cmake_build/nuttx.map | head
```

常见原因：
1. **缺少 `-Wstrict-prototypes`**：CMake 默认不加此标志，隐式声明不会报错
2. **源文件列表不一致**：CMakeLists.txt 遗漏了 Make.defs 中的文件
3. **条件编译差异**：Kconfig 宏在两个系统中未同步
4. **链接顺序不同**：`--start-group`/`--end-group` 未正确使用

### CMake 编译通过但运行异常
排查步骤：
1. 对比 `.config` 文件确认配置一致
2. 对比 `nuttx.map` 找出缺失的符号
3. 检查链接脚本是否正确引用
4. 检查 `CFLAGS`/`AFLAGS` 是否完整传递

### Include 路径问题
```bash
# 查看实际 include 路径
cmake --build build --verbose 2>&1 | grep "\-I"

# 对比 Make 的 include 路径
make -C nuttx V=1 2>&1 | grep "\-I"
```

## 关键注意事项

1. **`$(ARCH_SRCDIR)` 在 CMake 中**：对应 `${CMAKE_CURRENT_SOURCE_DIR}` 或架构相关路径
2. **`$(INCDIR_PREFIX)`**：Make 中的 `-I` 前缀，CMake 中用 `target_include_directories` 替代
3. **`-D` 宏定义**：确保 `CONFIG_*` 宏通过 `target_compile_definitions` 正确传递
4. **deferred include**：SDK 头文件必须延迟引入，否则依赖顺序错误
5. **链接脚本**：CMake 中通过 `target_link_options` 指定 `-T ld.script`
