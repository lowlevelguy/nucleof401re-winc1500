# Software Requirements Specification: STM32F401xE-WINC1500 Middleware
Version 1.0.

---

## Table of Contents
* [1. Introduction](#1-introduction)
    * [1.1 Document Purpose](#11-purpose)
    * [1.2 Scope](#12-scope)
    * [1.3 Component Overview](#13-component-overview)
        * [1.3.1 Component Perspective](#131-component-perspective)
            * [1.3.1.1 User Interface](#1311-user-interface)
        * [1.3.2 Component Functions](#131-component-perspective)
    * [1.4 Definitions](#14-definitions)
* [2. References](#2-references)
* [5. Appendices](#5-appendices)

## 1. Introduction

---

### 1.1 Purpose

This middleware serves the purpose of allowing STM32F401xE firmware to interface
with the Microchip ATWINC1500 Wi-Fi module official driver, to enable Wi-Fi
connectivity via an SPI connection.

### 1.2 Scope

The middleware covers the implementations of the two Hardware Abstraction Layer
interfaces expected by the vendor driver: the Board Support Package (BSP)
(`winc1500/bsp/include/nm_bsp.h`) and bus wrapper
(`winc1500/bus_wrapper/include/nm_bus_wrapper.h`) interfaces. In particular,
only the part of the latter that's relevant for SPI-based communication is
covered.

Additionally, it defines a porting interface for the user to implement. The
purpose of this interface is to allow the firmware flexibility in terms of
peripheral configuration, bus availability and runtime environment (namely,
baremetal or RTOS).

### 1.3 Component Overview
#### 1.3.1 Component Perspective

The middleware lies at the second-most bottom layer of four-stratum stack:
1) the application firmware calls
2) the vendor driver to execute networking functions, which in turn invokes
3) the middleware to convert them into platform-specific read-write commands,
   which in turn invokes
4) the STM32 HAL and the porting implementation to perform low-level SPI, DMA
   and GPIO operations

<img src="assets/middleware-component-perspective-2.png" width="100%" alt="Inter-Component Interfaces">

##### 1.3.1.1 User Interface

The user of the middleware is the STM32F401xE firmware developer. The user
interface is the porting interface part of the middleware. In their
implementation, the user is expected to define:
- GPIO connections mappings
- SPI peripheral mappings
- optionally, DMA peripheral mappings and configuration functions
- acquire/release semantics for SPI bus access
- a notification-based synchronisation mechanism for SPI operations
- EXTI, SPI and DMA ISR callback registration/de-registration mechanisms

#### 1.3.2 Component Functions

The middleware performs four groups of functions.

- As an implementation of the BSP interface, it performs **module power
  control**: power on, power off and reset sequences executed in a manner
  compliant with datasheet specifications.
- As an implementation of the BSP interface, it is charged with **managing 
  module-to-host interrupt**: the module exposes a specialised pin to transmit
  interrupt requests to the board &mdash;; the middleware configures said pin
  appropriately.
- As an implementation of the bus wrapper interface, it **handles WINC-relevant
  SPI transport**: peripheral configuration, thread-safe full-duplex and
  half-duplex communication, optionally leveraged by a DMA peripheral.
 
**Note, on peripheral ownership**: the bus wrapper makes use of, but does not
own, either of the SPI or &mdash; optional &mdash; DMA peripherals. All the
more, it does not own the SPI bus. The application firmware does. Hence, all SPI
bus and SPI and DMA peripheral access is done in a manner that is thread-safe
*and* agnostic to whether they are a shared resource.

### 1.4 Definitions

| Term                                  | Meaning                                                                                                        |
|---------------------------------------|----------------------------------------------------------------------------------------------------------------|
| Driver layer                          | The vendor source under `winc1500/`.                                                                           |
| Board and transport abstraction layer | The complex of the BSP and the bus wrapper implementations. Also called board &amp; transport layer for short. |
| Application layer                     | The application firmware.                                                                                      |
| User application                      | The application layer plus the porting implementation.                                                         |

When the phrase "user-specified" is used, it means that the object of interest
is exposed to the board &amp; transport layer via the integration interface, and
defined by the user in their implementation.

## 2. References

---

- Microchip ATWINC1500 datasheet (DS70005304).
- ATWINC1500 19.7.11 Software API reference manual.

## 5. Appendices

---

### 5.1 Assumptions and dependencies

The middleware assumes a WINC1500 driver and module firmware of version 19.7.11.

The middleware depends on the STM32 HAL, and on the user to supply a porting
implementation.