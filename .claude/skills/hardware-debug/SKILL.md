# 嵌入式硬件调试技能

## 描述
通过串口日志、寄存器转储、GPIO 状态分析等手段，排查嵌入式系统中的硬件驱动问题。

## 适用场景
- 外设无响应（UART/SPI/I2C/LCD/WiFi/BLE）
- 启动卡死或异常
- 中断不触发
- 引脚复用冲突

## 调试方法论

### 第一步：确认基本通信链路
```bash
# 1. 串口是否有输出？→ UART 基本链路正常
# 2. NSH 是否响应？→ 系统启动完成
# 3. ps 命令确认任务状态
nsh> ps
```

### 第二步：读取寄存器状态
当外设无响应时，直接读取硬件寄存器：
```c
// SPI 调试示例
syslog(LOG_ERR, "SPI CTL0: 0x%08lx, STAT: 0x%08lx\n",
       getreg32(GD32_SPI_CTL0(SPI0)),
       getreg32(GD32_SPI_STAT(SPI0)));

// GPIO AF 调试
syslog(LOG_ERR, "GPIOA AFSEL1: 0x%08lx\n",
       getreg32(GD32_GPIO_AFSEL1(GPIOA)));
```

### 第三步：对比官方 Demo
当问题难以定位时，对比厂商官方 Demo 代码：
1. 确认引脚配置（AF 编号、GPIO 模式）
2. 确认外设初始化顺序
3. 确认时钟使能顺序
4. 用逻辑分析仪抓波形对比

## 常见问题排查

### UART 无串口输出
**排查步骤**：
1. 确认 TX/RX 引脚 AF 编号正确（查数据手册 AF 表）
2. 确认时钟使能（RCU_USART0 等）
3. 确认波特率配置（与终端一致，通常 115200）
4. 确认引脚没有与其他外设冲突（如 IR_OUT 与 USART_TX 共用 PB15）

**典型错误**：
```
PB15 可用功能：RTC_REFIN, TIMER0_CH2_ON, TIMER2_CH0,
               I2C0_SCL, I2C1_SCL, UART1_TX, USART0_TX,
               IFRP_OUT, EVENTOUT
```
PB15 同时是 USART0_TX 和 IFRP_OUT，需要在 board Kconfig 中管理冲突。

### SPI 外设无响应
**排查步骤**：
1. 读取 GPIO AFSEL 寄存器确认 AF 配置
2. 确认 CS 引脚是 GPIO 输出（非 SPI_NSS AF）
3. 确认 SPI 时钟使能
4. 用逻辑分析仪抓 SCK/MOSI/MISO/CS 波形
5. 读取 SPI STAT 寄存器确认 TX/RX 缓冲区状态

**代码模板**：
```c
// SPI 原始测试
gd32_gpio_write(GPIO_LCD_CS, false);  // CS 拉低
SPI_SEND(SPI0, 0xDA);                 // 发送 ILI9341 读 ID 命令
SPI_SEND(SPI0, 0x00);
uint8_t id = SPI_SEND(SPI0, 0xFF);    // 读回
gd32_gpio_write(GPIO_LCD_CS, true);   // CS 拉高
syslog(LOG_ERR, "LCD ID: 0x%02x\n", id);
```

### 启动卡死
**排查步骤**：
1. 确认 `board_early_initialize()` 中没有阻塞操作
2. 确认 `board_late_initialize()` 中的初始化函数没有死锁
3. 检查 WiFi/BLE 初始化是否阻塞（SDK 初始化可能需要等待硬件就绪）

**WiFi/BLE 初始化卡死的典型原因**：
- 射频硬件复位未完成
- NVDS（非易失数据存储）区域损坏
- 中断未正确配置（BLE IRQ 未 attach）
- 共存模式下 WiFi 未先初始化

**临时解决方案**：添加超时机制或移到 `board_early_initialize`（调度器启动前执行）

### 中断不触发
**排查步骤**：
1. 确认 ECLIC 中断使能
2. 确认中断号正确（与芯片手册一致）
3. 确认 `irq_attach()` 调用成功
4. 检查 ECLIC MTH（阈值）是否屏蔽了目标中断级别

## 寄存器调试速查

### GD32VW55x 常用调试寄存器
```c
// 系统时钟
getreg32(GD32_RCU_CFG0)           // 时钟配置
getreg32(GD32_RCU_AHBEN)          // AHB 外设使能
getreg32(GD32_RCU_APB1EN)         // APB1 外设使能
getreg32(GD32_RCU_APB2EN)         // APB2 外设使能

// GPIO
getreg32(GD32_GPIO_CTL0(GPIOA))   // 端口配置低寄存器
getreg32(GD32_GPIO_OMOD(GPIOA))   // 输出模式
getreg32(GD32_GPIO_AFSEL0(GPIOA)) // AF 选择低寄存器
getreg32(GD32_GPIO_OCTL(GPIOA))   // 输出数据

// SPI
getreg32(GD32_SPI_CTL0(SPI0))     // 控制寄存器 0
getreg32(GD32_SPI_STAT(SPI0))     // 状态寄存器

// ECLIC
getreg32(GD32_ECLIC_MTH)          // 中断阈值
```

## 工具链
- **串口终端**：minicom / picocom / screen (115200 8N1)
- **逻辑分析仪**：Saleae / DSLogic（抓 SPI/UART 波形）
- **调试器**：J-Link / OpenOCD + GDB
- **NSH 命令**：ps / free / cat /dev/xxx / i2c / spi
