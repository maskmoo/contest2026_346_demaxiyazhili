############################################################################
# boards/gd32vw553x/gd32vw553h-eval/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to you under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
############################################################################

add_subdirectory(src)

# ===========================================================================
# 板级编译参数 (对应 scripts/Make.defs)
# ===========================================================================

# 链接脚本 (Make.defs: LDSCRIPT = ld.script → ARCHSCRIPT += ...)
set_property(GLOBAL PROPERTY LD_SCRIPT
             ${NUTTX_BOARD_DIR}/scripts/ld.script)

# 禁用链接器松弛优化 (Make.defs: ARCHCPUFLAGS += -mno-relax)
# 确保 Wi-Fi/BLE 预编译库链接兼容性
target_compile_options(arch PRIVATE -mno-relax)

# RISC-V 架构标识宏 (Make.defs: ARCHDEFINES += -DARCH_RISCV)
target_compile_definitions(arch PRIVATE ARCH_RISCV)

# 栈溢出检测 (Make.defs: ARCHCFLAGS += -finstrument-functions -ffixed-s11)
# -finstrument-functions: 编译器在每个函数入口/出口插入回调
# -ffixed-s11: 固定 s11 寄存器用于栈溢出检测 (不被编译器分配)
if(CONFIG_STACK_OVERFLOW_CHECK)
    target_compile_options(arch PRIVATE -finstrument-functions -ffixed-s11)
endif()

# 禁用 .note.gnu.build-id section
# 不禁用的话，该 section 会将 .text 从 0x08000000 推到 0x08000100，
# 导致 CPU 从 0x08000000 取不到有效代码，固件无法启动
target_link_options(nuttx PRIVATE -Wl,--build-id=none)
