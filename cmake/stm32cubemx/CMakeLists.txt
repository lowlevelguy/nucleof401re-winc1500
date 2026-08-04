cmake_minimum_required(VERSION 3.22)
# Enable CMake support for ASM and C languages
enable_language(C ASM)
# STM32CubeMX generated symbols (macros)
set(MX_Defines_Syms 
	USE_HAL_DRIVER 
	STM32F401xE
    $<$<CONFIG:Debug>:DEBUG>
)

# STM32CubeMX generated Include paths
set(MX_Include_Dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Include

	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/bsp/include
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/bus_wrapper/include
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/common/include
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/include
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/socket/include
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/spi_flash/include
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/config
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/programmer
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/root_cert
)

# STM32CubeMX generated application sources
set(MX_Application_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/src/stm32f4xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/src/stm32f4xx_hal_msp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/src/sysmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/src/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../startup_stm32f401xe.s
)

# STM32 HAL/LL Drivers
set(STM32_Drivers_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/src/system_stm32f4xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
)

# Drivers Middlewares
set(WINC1500_Driver_Src
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/common/source/nm_common.c

	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/bsp/source/nm_bsp_stm32f401xe.c

	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/bus_wrapper/source/nm_bus_wrapper_sam4s.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/bus_wrapper/source/nm_bus_wrapper_samd21.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/bus_wrapper/source/nm_bus_wrapper_same70.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/bus_wrapper/source/nm_bus_wrapper_samg53.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/bus_wrapper/source/nm_bus_wrapper_samg55.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/bus_wrapper/source/nm_bus_wrapper_saml21.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/bus_wrapper/source/nm_bus_wrapper_saml22.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/bus_wrapper/source/nm_bus_wrapper_samr21.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/bus_wrapper/source/nm_bus_wrapper_samr30.c

	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/nmspi.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/nmi2c.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/nmuart.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/nmdrv.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/nmdrv_crt.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/nmbus.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/nmbus_crt.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/nmasic.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/nmasic_crt.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/m2m_wifi.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/m2m_ssl.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/m2m_periph.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/m2m_ota.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/m2m_hif.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/m2m_hif_crt.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/m2m_crypto.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/driver/source/m2m_ate_mode.c

	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/socket/source/socket.c

	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/spi_flash/source/spi_flash.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../winc1500/spi_flash/source/flexible_flash.c
)


# Link directories setup
set(MX_LINK_DIRS

)
# Project static libraries
set(MX_LINK_LIBS 
    STM32_Drivers
    ${TOOLCHAIN_LINK_LIBRARIES}
    
)
# Interface library for Includes and symbols
add_library(stm32cubemx INTERFACE)
target_Include_directories(stm32cubemx INTERFACE ${MX_Include_Dirs})
target_compile_definitions(stm32cubemx INTERFACE ${MX_Defines_Syms})

# Create STM32_Drivers static library
add_library(STM32_Drivers OBJECT)
target_sources(STM32_Drivers PRIVATE ${STM32_Drivers_Src})
target_link_libraries(STM32_Drivers PUBLIC stm32cubemx)

# Create WINC1500_Driver static library
add_library(WINC1500_Driver OBJECT)
target_sources(WINC1500_Driver PRIVATE ${WINC1500_Driver_Src})
target_link_libraries(WINC1500_Driver PUBLIC stm32cubemx)

# Add STM32CubeMX generated application sources to the project
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${MX_Application_Src})

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${MX_LINK_DIRS})

# Add libraries to the project
target_link_libraries(${CMAKE_PROJECT_NAME} ${MX_LINK_LIBS})

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

# Validate that STM32CubeMX code is compatible with C standard
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()
