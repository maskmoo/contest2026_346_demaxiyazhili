# boards/risc-v/gd32vw55x/gd32vw553h-eval/src/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
#

target_sources(board PRIVATE
    gd32_boot.c
    gd32_bringup.c
    gd32_reset.c
    gd32_userleds.c
    gd32_buttons.c
)
