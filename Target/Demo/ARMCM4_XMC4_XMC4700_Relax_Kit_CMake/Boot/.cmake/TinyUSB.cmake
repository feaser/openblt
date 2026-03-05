# Create interface library for the hardware independent TinyUSB stack.
add_library(tinyusb-core INTERFACE)

target_sources(tinyusb-core INTERFACE
    "${CMAKE_SOURCE_DIR}/lib/tinyusb/src/tusb.c"
    "${CMAKE_SOURCE_DIR}/lib/tinyusb/src/common/tusb_fifo.c"
    "${CMAKE_SOURCE_DIR}/lib/tinyusb/src/device/usbd.c"
    "${CMAKE_SOURCE_DIR}/lib/tinyusb/src/device/usbd_control.c"
    "${CMAKE_SOURCE_DIR}/lib/tinyusb/src/class/vendor/vendor_device.c"
)

target_include_directories(tinyusb-core INTERFACE 
    "${CMAKE_SOURCE_DIR}/lib/tinyusb/src"
)

# Create interface library for the hardware dependent Infineon XMC4000 TinyUSB port.
add_library(tinyusb-port-xmc4000 INTERFACE)

target_sources(tinyusb-port-xmc4000 INTERFACE
    "${CMAKE_SOURCE_DIR}/lib/tinyusb/port/infineon/xmc4000/dcd_dwc2.c"
)

