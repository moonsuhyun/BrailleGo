file(GLOB BSP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_BOARD}/*.c")

add_library(bsp OBJECT ${BSP_SOURCES})

target_include_directories(bsp
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_BOARD}/Inc
)

target_link_libraries(bsp PUBLIC cmsis stm32hal)