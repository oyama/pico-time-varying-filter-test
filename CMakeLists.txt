cmake_minimum_required(VERSION 3.13...3.27)

include(pico_sdk_import.cmake)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMSISCORE ${CMAKE_CURRENT_LIST_DIR}/lib/CMSIS_6/CMSIS/Core)
project(time_varying_filter_test C CXX ASM)

pico_sdk_init()
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/CMSIS-DSP)

add_executable(${CMAKE_PROJECT_NAME}
  src/main.c
  src/filter_df2.c
  src/filter_gr.c
  src/filter_svf.c
  src/filter_tdf2rc.c
)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/lib/CMSIS_6/CMSIS/Core/Include
  ${CMAKE_CURRENT_LIST_DIR}/lib/CMSIS-DSP/Include
)
target_link_libraries(${CMAKE_PROJECT_NAME}
  pico_stdlib
  CMSISDSP
)
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
  -DARM_ALL_FFT_TABLES
  -DARM_TABLE_TWIDDLECOEF_F32_4096
  -DARM_TABLE_BITREVIDX_FLT_4096
)
add_dependencies(${CMAKE_PROJECT_NAME} CMSISDSP)
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})


find_program(OPENOCD openocd)
if(OPENOCD)
  add_custom_target(run
    COMMAND ${OPENOCD} -f interface/cmsis-dap.cfg -f target/rp2350.cfg -c "adapter speed 5000" -c "program ${CMAKE_PROJECT_NAME}.elf verify reset exit"
    DEPENDS ${CMAKE_PROJECT_NAME}
  )
endif()
