# boards/risc-v/gd32vw55x/gd32vw553h-eval/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
#

set_property(GLOBAL PROPERTY LD_SCRIPT "${NUTTX_BOARD_DIR}/scripts/ld.script")

if(CONFIG_ARCH_BOARD_GD32VW553H_EVAL)
  add_subdirectory(src)
endif()
