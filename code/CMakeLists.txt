# CMakeLists.txt for SW63 STM32L052K8
cmake_minimum_required(VERSION 3.16)

# Device configuration
set(DEVICE_NAME STM32L052K8)

# Set C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()
message("Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_PATH "")

set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PATH}arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}arm-none-eabi-g++)
set(CMAKE_LINKER ${TOOLCHAIN_PATH}arm-none-eabi-g++)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PATH}arm-none-eabi-objcopy)
set(CMAKE_SIZE ${TOOLCHAIN_PATH}arm-none-eabi-size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

# Disables tests
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(TARGET_FLAGS "-mcpu=cortex-m0plus -mthumb -mfloat-abi=soft")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -fdata-sections -ffunction-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -fdata-sections -ffunction-sections -Wno-volatile")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -g3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -g3")
endif()
if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -g0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -g0")
endif()

set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")

# Setup linker flags
set(CMAKE_C_LINK_FLAGS "${TARGET_FLAGS}")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --specs=nano.specs --specs=nosys.specs")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,-Map=sw63.map -Wl,--gc-sections")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -lc -lm -Wl,--end-group")

project(sw63)

# File path variables
set(BIN_FILE sw63.bin)
set(ELF_FILE sw63.elf)
set(HEX_FILE sw63.hex)
set(HEX_FILE_WITH_BOOTLOADER sw63-with-bootloader.hex)
set(UF2_FILE sw63.uf2)
set(JLINK_FILE sw63.jlink)
set(JLINK_FILE_WITH_BOOTLOADER sw63-with-bootloader.jlink)
set(LD_FILE ${CMAKE_SOURCE_DIR}/ld/stm32l052k8_bootloader.ld)

enable_language(C CXX ASM)

# Define STM32L052xx for preprocessor
add_definitions(-DSTM32L052xx)
add_definitions(-DUSE_HAL_DRIVER)
add_definitions(-DMSI_VALUE=2097000U)

# Paths to core libraries
set(HAL_DRIVER_PATH ${CMAKE_SOURCE_DIR}/lib/stm32l0xx-hal-driver)
set(CMSIS_6_PATH ${CMAKE_SOURCE_DIR}/lib/CMSIS_6)
set(CMSIS_DEVICE_L0_PATH ${CMAKE_SOURCE_DIR}/lib/cmsis-device-l0)

# All source files
set(SOURCES
    # System sources
    ${CMSIS_DEVICE_L0_PATH}/Source/Templates/system_stm32l0xx.c
    ${CMSIS_DEVICE_L0_PATH}/Source/Templates/gcc/startup_stm32l052xx.s
    
    # HAL driver sources
    ${HAL_DRIVER_PATH}/Src/stm32l0xx_hal.c
    ${HAL_DRIVER_PATH}/Src/stm32l0xx_hal_cortex.c
    ${HAL_DRIVER_PATH}/Src/stm32l0xx_hal_dma.c
    ${HAL_DRIVER_PATH}/Src/stm32l0xx_hal_flash.c
    ${HAL_DRIVER_PATH}/Src/stm32l0xx_hal_flash_ex.c
    ${HAL_DRIVER_PATH}/Src/stm32l0xx_hal_gpio.c
    ${HAL_DRIVER_PATH}/Src/stm32l0xx_hal_pwr.c
    ${HAL_DRIVER_PATH}/Src/stm32l0xx_hal_pwr_ex.c
    ${HAL_DRIVER_PATH}/Src/stm32l0xx_hal_rcc.c
    ${HAL_DRIVER_PATH}/Src/stm32l0xx_hal_rcc_ex.c
    ${HAL_DRIVER_PATH}/Src/stm32l0xx_hal_tim.c
    ${HAL_DRIVER_PATH}/Src/stm32l0xx_hal_i2c.c
    ${HAL_DRIVER_PATH}/Src/stm32l0xx_hal_i2c_ex.c
    ${HAL_DRIVER_PATH}/Src/stm32l0xx_hal_adc.c
    ${HAL_DRIVER_PATH}/Src/stm32l0xx_hal_adc_ex.c
    
    # Application sources
    src/main.cpp
    src/app/App.cpp
    src/app/AnimationRunner.cpp
    src/app/Locale.cpp
    src/app/animations/AnimationTime.cpp
    src/app/animations/AnimationIntro.cpp
    src/app/animations/AnimationCharge.cpp
    src/app/ui/LayerNormal.cpp
    src/app/ui/LayerSettings.cpp
    src/app/ui/LayerSecret.cpp
    src/dev/Button.cpp
    src/dev/Display.cpp
    src/dev/System.cpp
    src/dev/DS3231.cpp
)

# Include directories
set(INCLUDES
    ${CMSIS_6_PATH}/CMSIS/Core/Include
    ${CMSIS_DEVICE_L0_PATH}/Include
    ${HAL_DRIVER_PATH}/Inc
    src
)

# Create the sw63 target
add_executable(sw63 ${SOURCES})

target_compile_definitions(sw63 PRIVATE
    USE_HAL_DRIVER 
    STM32L052xx
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(sw63 PRIVATE ${INCLUDES})

set_target_properties(sw63 PROPERTIES 
    LINK_FLAGS ${CMAKE_C_LINK_FLAGS}
)

# Add linker script via target_link_options
target_link_options(sw63 PRIVATE "-T${LD_FILE}")

# Post build commands
add_custom_command(TARGET sw63 POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${ELF_FILE} ${BIN_FILE}
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${ELF_FILE} ${HEX_FILE}
    COMMAND ${CMAKE_SIZE} ${ELF_FILE}
    COMMAND python3 ${CMAKE_SOURCE_DIR}/lib/uf2/utils/uf2conv.py -c -f STM32L0 ${HEX_FILE} -o ${UF2_FILE}
    COMMAND srec_cat ${CMAKE_SOURCE_DIR}/bootloader/sw63-bootloader.hex -Intel ${HEX_FILE} -Intel -o ${HEX_FILE_WITH_BOOTLOADER} -Intel
    COMMENT "Building ${BIN_FILE}, ${HEX_FILE}, ${UF2_FILE}, and merged ${HEX_FILE_WITH_BOOTLOADER}"
)

# Generate J-Link script
add_custom_command(TARGET sw63 POST_BUILD
    COMMAND echo "device ${DEVICE_NAME}" > ${JLINK_FILE}
    COMMAND echo "si SWD" >> ${JLINK_FILE}
    COMMAND echo "speed 4000" >> ${JLINK_FILE}
    COMMAND echo "connect" >> ${JLINK_FILE}
    COMMAND echo "halt" >> ${JLINK_FILE}
    COMMAND echo "loadfile ${HEX_FILE}" >> ${JLINK_FILE}
    COMMAND echo "r" >> ${JLINK_FILE}
    COMMAND echo "g" >> ${JLINK_FILE}
    COMMAND echo "exit" >> ${JLINK_FILE}
    COMMENT "Generating J-Link script for sw63"
)

# Create flash target
add_custom_target(flash
    COMMAND JLinkExe -CommanderScript ${JLINK_FILE}
    DEPENDS sw63
    COMMENT "Flashing sw63"
)

# Generate J-Link script for merged hex with bootloader
add_custom_command(TARGET sw63 POST_BUILD
    COMMAND echo "device ${DEVICE_NAME}" > ${JLINK_FILE_WITH_BOOTLOADER}
    COMMAND echo "si SWD" >> ${JLINK_FILE_WITH_BOOTLOADER}
    COMMAND echo "speed 4000" >> ${JLINK_FILE_WITH_BOOTLOADER}
    COMMAND echo "connect" >> ${JLINK_FILE_WITH_BOOTLOADER}
    COMMAND echo "halt" >> ${JLINK_FILE_WITH_BOOTLOADER}
    COMMAND echo "loadfile ${HEX_FILE_WITH_BOOTLOADER}" >> ${JLINK_FILE_WITH_BOOTLOADER}
    COMMAND echo "r" >> ${JLINK_FILE_WITH_BOOTLOADER}
    COMMAND echo "g" >> ${JLINK_FILE_WITH_BOOTLOADER}
    COMMAND echo "exit" >> ${JLINK_FILE_WITH_BOOTLOADER}
    COMMENT "Generating J-Link script for merged sw63-with-bootloader"
)

# Create flash target for merged hex with bootloader
add_custom_target(flash-with-bootloader
    COMMAND JLinkExe -CommanderScript ${JLINK_FILE_WITH_BOOTLOADER}
    DEPENDS sw63
    COMMENT "Flashing sw63 with bootloader"
)

# Print build information
message(STATUS "Device: ${DEVICE_NAME}")
message(STATUS "Target: sw63")
message(STATUS "Linker script: ${LD_FILE}")
message(STATUS "")
message(STATUS "Usage:")
message(STATUS "  Configure: cmake ..")
message(STATUS "  Build: make")
message(STATUS "  Flash: make flash")
message(STATUS "  Flash with bootloader: make flash-with-bootloader")