# Setting up app build
set(APP_NAME ${CMAKE_PROJECT_NAME})

add_executable(${APP_NAME}
    main.cpp
    utils/source/bitarray.cpp
    utils/source/canvas.cpp
    utils/source/intrusive-list.cpp
)

target_include_directories(${APP_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    utils/include
    ui/include
    tamagotchi/include
)

target_link_libraries(${APP_NAME}
    PRIVATE
        mik32_hal
        mik32_runtime
        mik32_dht11
        mik32_ssd1315
)

target_link_directories(${APP_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/hardware/mik32-runtime/ldscripts
)

# Convert output to hex and binary
add_custom_command(TARGET ${APP_NAME}
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${TOOLCHAIN_PREFIX}-objcopy ARGS -O binary ${APP_NAME}${CMAKE_EXECUTABLE_SUFFIX_C} ${APP_NAME}.bin
    COMMAND ${TOOLCHAIN_PREFIX}-objcopy ARGS -O ihex ${APP_NAME}${CMAKE_EXECUTABLE_SUFFIX_C} ${APP_NAME}.hex
    COMMAND ${TOOLCHAIN_PREFIX}-objdump -w -x -s -S ${APP_NAME}.elf > ${APP_NAME}.dump
)