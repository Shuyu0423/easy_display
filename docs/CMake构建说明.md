# CMake 构建说明

## Host 测试

Windows 下推荐安装：

- CMake
- Ninja
- GCC，当前 preset 默认使用 `C:/MinGW/bin/gcc.exe`

```powershell
cmake --preset host --fresh
cmake --build --preset host
ctest --preset host
```

Host 构建会编译并运行触摸控制器的 smoke test，同时对 RTT log 模块做语法检查。

## STM32F4 对象编译

```powershell
cmake --preset stm32f4-objects --fresh
cmake --build --preset stm32f4-objects
```

该目标用于检查 demo 工程的源文件、头文件和宏依赖关系。当前还不负责生成可烧录固件。

如果 CMake 报 `unable to find a build program corresponding to "Ninja"`，说明当前环境缺少 Ninja，需要先安装 Ninja 或改用本机已有的生成器。

## 后续迁移方向

## STM32F4 固件构建

```powershell
cmake --preset stm32f4-gcc --fresh
cmake --build --preset stm32f4-gcc
```

该目标使用 `arm-none-eabi-gcc` 生成：

- `easy_display_stm32f4.elf`
- `easy_display_stm32f4.hex`
- `easy_display_stm32f4.bin`
- `easy_display_stm32f4.map`

生成路径：

```text
build/stm32f4-gcc/demo/stm32_f4/example_pro/
```

## STM32F4 烧录

使用 OpenOCD：

```powershell
cmake --build --preset stm32f4-gcc --target flash-openocd
```

也可以使用默认烧录目标，目前默认指向 OpenOCD：

```powershell
cmake --build --preset stm32f4-gcc --target flash
```

OpenOCD 默认配置：

```text
interface/stlink.cfg
target/stm32f4x.cfg
```

如果你使用 J-Link，并且 `JLink.exe` 在 PATH 中，可以使用：

```powershell
cmake --build --preset stm32f4-gcc --target flash-jlink
```

当前 toolchain 默认路径是：

```text
C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/12.2 mpacbti-rel1/bin
```

## 后续迁移方向

1. 添加 JLink 或 OpenOCD 烧录目标。
2. 根据实际芯片型号拆分 linker script。
3. 保留 Keil 作为调试入口，CMake 作为依赖关系和 CI 入口。
