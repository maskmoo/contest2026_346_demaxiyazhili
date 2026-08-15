# GD32VW55x 移植修复计划

对照 [porting_guide.md](https://github.com/open-vela/docs/blob/dev-ai-contest-2026/zh-cn/chip_porting/porting_guide.md) 检查发现以下问题：

## 待修复项

### P0 — 严重（导致编译/链接失败）

- [ ] **manifest 链接路径拼写错误**：`contest2026_346_demaxiyazhili.xml` 第 14 行 `board/` 应为 `boards/`
- [x] **芯片层缺少公共 `include/` 头文件**：缺少 `include/chip.h` 和 `include/irq.h`，架构层公共代码通过 `<arch/chip/chip.h>` 和 `<arch/irq.h>` 引用

### P1 — 中等

- [ ] **链接脚本缺少 `.vectors` 显式段**：`ld.script` 中 `.text` 段未显式放置 `*(.vectors)`
- [ ] **缺少 `board_app_initialize` 函数**：`gd32_boot.c` 只有 `board_late_initialize`，缺少 nsh 上下文的 `board_app_initialize`
- [ ] **`scripts/Make.defs` 缺少编译器标志**：仅设置了 `LDSCRIPT`，缺少 `CFLAGS`/`AFLAGS`/`EXTRA_LIBS` 等

### P2 — 建议

- [ ] **定时器应迁移到 `arch_alarm` 模型**：当前使用 `arch_timer`，指南推荐 `arch_alarm`（oneshot driver）
- [ ] **board Kconfig 内容不足**：仅有一个 board tag 配置

### P3 — 可选

- [ ] **缺少 ETCROMFS 构建配置**：无 `nsh_romfsimg.h` 和 ROMFS 配置（当前用 procfs 替代）

## 进度记录

- 2026-08-15: 开始修复，处理 P0 芯片层缺少公共 include 头文件
