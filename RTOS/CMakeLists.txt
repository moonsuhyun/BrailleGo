file(GLOB RTOS_SOURCES
        "kernel/src/*.c"
        "kernel/src/*.cpp"
        "lib/*.c"
)

file(GLOB PORT_SOURCES "port/${TARGET_PROCESSOR}/*.c")
list(APPEND RTOS_SOURCES ${PORT_SOURCES})
#add_library(rtos_handler OBJECT port/${TARGET_PROCESSOR}/PortHandler.c)

add_library(rtos OBJECT ${RTOS_SOURCES})

target_include_directories(rtos
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/kernel/include
            ${CMAKE_CURRENT_SOURCE_DIR}/lib
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/kernel/internal
            ${CMAKE_CURRENT_SOURCE_DIR}/port
            ${CMAKE_CURRENT_SOURCE_DIR}/port/${TARGET_PROCESSOR}
)

target_link_libraries(rtos PUBLIC cmsis bsp)

#target_include_directories(rtos_handler
#        PUBLIC
#            ${CMAKE_CURRENT_SOURCE_DIR}/kernel/include
#            ${CMAKE_CURRENT_SOURCE_DIR}/lib
#        PRIVATE
#            ${CMAKE_CURRENT_SOURCE_DIR}/kernel/internal
#            ${CMAKE_CURRENT_SOURCE_DIR}/port
#            ${CMAKE_CURRENT_SOURCE_DIR}/port/${TARGET_PROCESSOR}
#)
#
#target_link_libraries(rtos_handler PUBLIC cmsis bsp)
