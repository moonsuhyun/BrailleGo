
add_library(hal_conf INTERFACE)
target_include_directories(hal_conf
        INTERFACE
            ${CMAKE_SOURCE_DIR}/RTOS/BSP/${TARGET_BOARD}/HalConf
)

add_library(cmsis INTERFACE)
target_include_directories(cmsis
        INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Device/ST/STM32F1xx/Include
            ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Include
)

file(GLOB_RECURSE STM32HAL_SOURCES "STM32F1xx_HAL_Driver/*.*")
add_library(stm32hal STATIC ${STM32HAL_SOURCES})
target_include_directories(stm32hal
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/STM32F1xx_HAL_Driver/Inc
            ${CMAKE_CURRENT_SOURCE_DIR}/STM32F1xx_HAL_Driver/Inc/Legacy
)

target_link_libraries(stm32hal PUBLIC cmsis hal_conf)