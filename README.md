# Working with ATWINC1500 on the Nucleo F401RE

This repository contains firmware intended for a Nucleo F401RE board,
communicating over SPI with the Microchip ATWINC1500 module to enable WiFi
connectivity.

## Contents
- Vendor SDK: `winc1500/`
- Middleware: `winc1500/bsp/include/nm_bsp_stm32f401xe.h`, `winc1500/bsp/source/nm_bsp_stm32f401xe.c`, `winc1500/bus_wrapper/source/nm_bus_wrapper_stm32f401xe.c`, `core/include/conf_winc.h`
- User Application: `core/`

## Usage
To build the firmware, run:
```
$ cmake --build <build-dir> --target atwinc1500_first_contact -j $(nproc)
```

Depending on your board, you can then adapt the OpenOCD script
`nucleo-f401re-jlink.cfg` to flash to it.

## Documentation
[Middleware Software Requirements Specification](docs/middleware/SRS.md)