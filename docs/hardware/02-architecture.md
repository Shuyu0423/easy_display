# easy_display 当前架构图

日期：2026-08-09  
阶段：现有设计复盘（快速模式）

> 本图依据当前仓库代码与配置生成。实线表示当前默认启用或实际调用路径；虚线表示代码已具备、但当前默认配置未启用或在默认启动流程中不可达的能力。

## 1. 系统总览

```mermaid
flowchart TB
    USER["用户 / 上位机"]
    TOUCH["用户触摸"]

    subgraph APP["应用与验证层"]
        MAIN["app/main.c<br/>启动与主循环"]
        BOARDTEST["ILI9341 + XPT2046<br/>板级联调测试（默认启用）"]
        CMD["命令框架<br/>SC_VERSION / SC_HELLO"]
        UI["LCD UI<br/>图片、文字、像素绘制"]
    end

    subgraph MIDDLEWARE["easy_display 可移植中间层"]
        LCDAPI["LCD_DRIVER 抽象接口<br/>init / clear / pixel / window / pixels"]
        LCDCTRL["屏幕控制器驱动<br/>ILI9341（启用）"]
        LCDALT["ST7789 / EPD / SSD1306<br/>可选实现（未启用）"]
        TOUCHAPI["触摸核心抽象<br/>TOUCH_DEVICE + TOUCH_CONTROLLER"]
        XPT["XPT2046 电阻触摸驱动<br/>默认测试路径"]
        CST["CST816 电容触摸驱动<br/>已实现、未默认启用"]
        LOG["日志模块 easy_log<br/>RTT（当前后端）"]
        RING["UART 环形缓冲 + 命令分发"]
        FLASHLOG["Flash 日志后端<br/>已实现、未启用"]
    end

    subgraph BSP["STM32F407VET6 板级适配层"]
        STDLIB["STM32F4 标准外设库 / CMSIS"]
        LCDSPI["LCD 软件 SPI GPIO<br/>PE7 SCL · PE8 SDA<br/>PE9 RST · PE10 DC · PE11 CS<br/>PE12 BUSY · PE13 BL"]
        TSPI["触摸软件 SPI GPIO<br/>PB13 SCL · PB15 MOSI · PB14 MISO<br/>PB12 CS · PB10 IRQ"]
        TI2C["触摸软件 I²C GPIO<br/>PB6 SCL · PB7 SDA<br/>PB8 RST · PB9 INT"]
        UART["USART1 + DMA2 Stream5<br/>PA9 TX · PA10 RX · 115200"]
        IFLASH["片内 Flash 读写/擦除适配"]
        TICK["SysTick / TIM 时基与延时"]
        RTT["SEGGER RTT / SWD 调试通道"]
    end

    subgraph HW["实体硬件 / 当前参考板"]
        MCU["STM32F407VET6<br/>嘉立创天空星开发板"]
        PANEL["ILI9341 TFT LCD<br/>240 × 320 · RGB565"]
        TP["XPT2046 电阻触摸"]
        CTP["CST816 电容触摸（可选）"]
        HOST["串口终端 / 上位机"]
        DEBUGGER["J-Link / ST-Link + RTT"]
        NVM["MCU 内部 Flash"]
    end

    MAIN --> BOARDTEST
    BOARDTEST --> LCDAPI
    BOARDTEST --> TOUCHAPI
    BOARDTEST --> LOG
    MAIN -.->|"测试关闭后才可到达"| UI
    MAIN -.-> RING
    RING -.-> CMD
    USER -.->|"串口命令"| HOST
    TOUCH --> TP

    UI --> LCDAPI
    LCDAPI --> LCDCTRL
    LCDAPI -.-> LCDALT
    LCDCTRL --> LCDSPI
    TOUCHAPI --> XPT
    TOUCHAPI -.-> CST
    XPT --> TSPI
    CST -.-> TI2C
    LOG --> RTT
    LOG -.-> FLASHLOG
    LOG -.-> UART
    FLASHLOG -.-> IFLASH
    RING -.-> UART

    LCDSPI --> STDLIB
    TSPI --> STDLIB
    TI2C -.-> STDLIB
    UART -.-> STDLIB
    IFLASH -.-> STDLIB
    TICK --> STDLIB
    STDLIB --> MCU

    MCU --> LCDSPI
    LCDSPI --> PANEL
    TSPI --> TP
    TI2C -.-> CTP
    UART -.-> HOST
    IFLASH -.-> NVM
    RTT --> DEBUGGER
```

## 2. 当前默认启动路径

```mermaid
flowchart LR
    RESET["复位"] --> INIT["board_init<br/>uart1_init(115200)<br/>tick_timer_init"]
    INIT --> TEST["ili9341_xpt2046_test()"]
    TEST --> LCD["初始化 ILI9341<br/>红/绿/蓝与四色块测试"]
    LCD --> REG["注册 XPT2046<br/>注册触摸总线回调"]
    REG --> LOOP["永久轮询触摸<br/>RTT 输出坐标 + 屏幕画十字"]
    LOOP --> LOOP
    LOOP -.->|"当前不会返回"| UNREACHABLE["buffer_init / ui_init<br/>UART 命令主循环"]
```

## 3. 当前设计结论

- 核心设计思想是“上层统一 API + 控制器驱动 + 板级回调适配”。显示和触摸控制器不直接绑定 STM32 外设，移植时主要替换 `mcu_spi`、`mcu_touch_spi`、`mcu_touch_iic` 等板级实现。
- 当前实际生效组合是 `STM32F407VET6 + ILI9341 240×320 + XPT2046 + SEGGER RTT`。
- LCD 与 XPT2046 分别使用一套软件 SPI GPIO，没有共享同一 SPI 总线。
- CST816 的驱动和软件 I²C 适配已经存在，但默认校准测试关闭；ST7789、EPD、SSD1306 也属于可选能力，当前配置未启用。
- UART1 DMA 环形接收、命令框架和普通 UI 主循环已经实现，但默认开启的 ILI9341/XPT2046 测试含永久循环，因此这些代码在当前固件启动路径中不可达。
- 日志框架支持 RTT、UART 和片内 Flash；当前 `DEBUG_TARGETS` 只启用 RTT。Flash 日志配置和底层读写代码存在，但不是当前运行后端。

## 4. 需要后续确认的边界

- 仓库没有完整原理图或电源设计资料，因此电源输入、稳压、电平兼容、ESD/EMI、连接器和背光驱动电流不在本图中推断。
- `PE12 BUSY` 是通用显示适配保留信号；ILI9341 常规 SPI 显示路径是否实际接入该脚，需要以接线或原理图确认。
- 当前架构更接近“裸机轮询 + 中间件回调”，尚未看到 RTOS 任务、事件队列或统一显示/触摸事件循环。
- 显示总线为 GPIO 模拟 SPI，适合联调和移植验证；刷新带宽是否满足最终 GUI，需要用目标帧率和局部刷新比例实测。

## 5. 主要证据文件

- `driver_pack/user_conf.h`：当前板级依赖及测试开关。
- `driver_pack/lcd/inc/lcd_conf.h`：ILI9341、分辨率和显示接口配置。
- `driver_pack/lcd/inc/lcd_driver.h`：LCD 抽象接口。
- `driver_pack/input/touch/core/inc/touch_driver.h`：触摸设备、总线和控制器抽象。
- `demo/stm32_f4/example_pro/app/main.c`：实际启动顺序。
- `tests/target/touch/ili9341_xpt2046_test.c`：默认板级测试及永久轮询路径。
- `demo/stm32_f4/example_pro/board/`：LCD、触摸、Flash 与时基适配。
- `demo/stm32_f4/example_pro/bsp/uart/`：USART1 + DMA 环形接收。
- `driver_pack/log/inc/sc_log_conf.h`：当前 RTT 日志后端选择。
