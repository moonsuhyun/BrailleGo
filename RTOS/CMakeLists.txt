file(GLOB RTOS_SOURCES
        "kernel/src/*.c"
        "kernel/src/*.cpp"
        "lib/*.c"
)

if(TARGET_PROCESSOR STREQUAL "cortex-m3")
    file(GLOB PORT_SOURCES "port/cortex-m3/*.c")
    list(APPEND RTOS_SOURCES ${PORT_SOURCES})
    add_library(rtos_handler OBJECT port/cortex-m3/PortHandler.c)
endif ()

if(TARGET_MCU STREQUAL "stm32f103")
    file(GLOB BSP_SOURCES "bsp/stm32f103/*.c")
    list(APPEND RTOS_SOURCES ${BSP_SOURCES})
endif ()

add_library(rtos STATIC ${RTOS_SOURCES})

target_include_directories(rtos
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/kernel/include
            ${CMAKE_CURRENT_SOURCE_DIR}/lib
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/kernel/internal
            ${CMAKE_CURRENT_SOURCE_DIR}/bsp
            ${CMAKE_CURRENT_SOURCE_DIR}/port
)

target_link_libraries(rtos PUBLIC cmsis stm32hal)

target_include_directories(rtos_handler
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/kernel/include
            ${CMAKE_CURRENT_SOURCE_DIR}/lib
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/kernel/internal
            ${CMAKE_CURRENT_SOURCE_DIR}/bsp
            ${CMAKE_CURRENT_SOURCE_DIR}/port
)

target_link_libraries(rtos_handler PUBLIC cmsis stm32hal)
