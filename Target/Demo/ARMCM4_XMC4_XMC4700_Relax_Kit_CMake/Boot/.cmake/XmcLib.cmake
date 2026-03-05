# Create interface library for the Infineon XMC library.
add_library(infineon-xmc-lib INTERFACE)

target_sources(infineon-xmc-lib INTERFACE
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc4_eru.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc4_flash.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc4_gpio.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc4_rtc.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc4_scu.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_can.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_ccu4.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_ccu8.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_common.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_dac.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_dma.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_dsd.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_ebu.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_ecat.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_eru.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_eth_mac.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_eth_phy_dp83848.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_eth_phy_ksz8031rnl.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_eth_phy_ksz8081rnb.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_fce.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_gpio.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_hrpwm.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_i2c.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_i2s.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_ledts.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_posif.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_rtc.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_sdmmc.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_spi.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_uart.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_usbd.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_usbh.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_usic.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_vadc.c"
    "${CMAKE_SOURCE_DIR}/lib/xmclib/src/xmc_wdt.c"
)

target_include_directories(infineon-xmc-lib INTERFACE 
    "${CMAKE_SOURCE_DIR}/lib/xmclib/inc"
)
