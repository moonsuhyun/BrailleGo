file(GLOB_RECURSE CORE_SOURCES "Src/*.*")

list(APPEND CORE_SOURCES "${CMAKE_SOURCE_DIR}/BSP/${TARGET_BOARD}/Startup/startup_stm32f103rbtx.s")

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/BSP/${TARGET_BOARD}/Ld/STM32F103RBTX_FLASH.ld)

add_executable(${PROJECT_NAME}.elf ${CORE_SOURCES} ${LINKER_SCRIPT})

target_include_directories(${PROJECT_NAME}.elf PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Inc)

target_link_libraries(${PROJECT_NAME}.elf
        PRIVATE
            bsp
            rtos
#            rtos_handler
)

target_link_options(${PROJECT_NAME}.elf PRIVATE -T ${LINKER_SCRIPT})
target_link_options(${PROJECT_NAME}.elf PRIVATE -Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
target_link_options(${PROJECT_NAME}.elf PRIVATE -mcpu=cortex-m3 -mthumb -mthumb-interwork)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
