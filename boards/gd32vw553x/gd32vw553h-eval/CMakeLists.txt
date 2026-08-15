# boards/risc-v/gd32vw55x/gd32vw553h-eval/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
#

add_subdirectory(src)

file(GLOB EXTRA_LIBS
     ${NUTTX_BOARD_ABS_DIR}/libs/${CONFIG_ARCH_BOARD_CUSTOM_NAME}/*.a)

set_property(GLOBAL APPEND PROPERTY NUTTX_EXTRA_LIBRARIES ${EXTRA_LIBS})

set_property(GLOBAL PROPERTY LD_SCRIPT ${NUTTX_BOARD_ABS_DIR}/scripts/ld.script)

# Disable .note.gnu.build-id section to ensure .text starts at 0x08000000
target_link_options(nuttx PRIVATE -Wl,--build-id=none)
