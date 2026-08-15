# Software Requirements Specification: STM32F401xE-WINC1500 Middleware
Version 1.0.

## Table of Contents
* [1. Introduction](#1-introduction)
    * [1.1 Purpose](#11-purpose)
    * [1.2 Scope](#12-scope)
    * [1.3 Component Overview](#13-component-overview)
        * [1.3.1 Component Perspective](#131-component-perspective)
            * [1.3.1.1 User Interface](#1311-user-interface)
        * [1.3.2 Component Functions](#131-component-perspective)
    * [1.4 Definitions](#14-definitions)
* [2. References](#2-references)
* [3. Requirements](#3-requirements)
    * [3.1 Functions](#31-functions)
    * [3.2 Performance](#32-performance)
    * [3.3 Usability](#33-usability)
    * [3.4 Interface](#34-interface)
    * [3.5 Design Constraints](#35-design-constraints)
    * [3.6 Software System Attributes](#36-software-system-attributes)
* [4. Verification](#4-verification)
* [5. Appendices](#5-appendices)
    * [5.1 Assumptions and dependencies](#51-assumptions-and-dependencies)
    * [5.2 Acronyms and abbreviations](#52-acronyms-and-abbreviations)

## 1. Introduction

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

![Inter-Component Interfaces](assets/middleware-component-perspective-2.png)

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

- Microchip ATWINC1500 datasheet (DS70005304F).
- ATWINC1500 19.7.11 Software API reference manual.
- ST STM32F401xE reference manual (RM0368).

## 3. Requirements

This section lists the various requirements that an implementation of this
middleware shall meet. Each requirement is a single, verifiable capability and
is uniquely identified by a `REQ-<section>` number that is never reused.

### 3.1 Functions

This subsection lists the functional requirements that the middleware shall
meet.

#### 3.1.1 BSP Functions

The BSP implements the `nm_bsp_*` API declared in
`winc1500/bsp/include/nm_bsp.h`. It is responsible for module power control,
module-to-host interrupt management and putting the driver to sleep.

| ID         | Requirement                                                                                                                                                   | Rationale                                                                                                                                                                    |
|------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| REQ-FUN-01 | On invocation, `nm_bsp_init` shall hold the CHIP_EN and RESET_N signals low when it returns.                                                                  | The module is powered off by default until a call is made to explicitly start it. CHIP_EN is active-high and RESET_N is active-low (ATWINC1500 datasheet, sections 7.4-7.6). |
| REQ-FUN-02 | `nm_bsp_init` shall configure the IRQN signal to generate an interrupt on the falling edge, with the internal pull-up resistor enabled.                       | IRQN is active-low; the pull-up forces the line high while the module is powered off.                                                                                        |
| REQ-FUN-03 | `nm_bsp_init` shall invoke the porting API to ensure that no ISR callback is registered for the IRQN signal.                                                  | If the function is invoked as part of a re-initialisation procedure, no leftover configuration from past state should remain.                                                |
| REQ-FUN-04 | `nm_bsp_init` shall return `M2M_SUCCESS` on success, and a negative integer on failure to fulfill any of the above requirements.                              | The vendor contract requires it.                                                                                                                                             |
| REQ-FUN-05 | On invocation, `nm_bsp_deinit` shall deinitialise the CHIP_EN, RESET_N and IRQN control pins.                                                                 | Every successful `nm_bsp_init` is matched by a `nm_bsp_deinit` per the vendor contract.                                                                                      |
| REQ-FUN-06 | `nm_bsp_deinit` shall return `M2M_SUCCESS` on success, and a negative integer on failure to fulfill the above requirement.                                    | The vendor contract requires it.                                                                                                                                             |
| REQ-FUN-07 | On invocation, `nm_bsp_reset` shall begin by holding the CHIP_EN and RESET_N signals low for at least 2 &mu;s.                                                | The datasheet specifies a minimum RESET_N pulse of 1 &mu;s. We double that value as a safety margin for timing correctness.                                                  |
| REQ-FUN-08 | `nm_bsp_reset` shall follow by setting the CHIP_EN signal high and shall hold it high for at least 10 ms before setting the RESET_N signal high.              | The datasheet specifies a minimum of 5 ms between the CHIP_EN and RESET_N rises. We double that value for safety.                                                            |
| REQ-FUN-09 | On invocation, `nm_bsp_sleep` shall block the invoking context for the specified number of milliseconds.                                                      | &mdash;                                                                                                                                                                      |
| REQ-FUN-10 | On invocation, `nm_bsp_register_isr` shall invoke the porting API to register the specified callback as ISR for the IRQN signal.                              | &mdash;                                                                                                                                                                      |
| REQ-FUN-11 | On invocation, `nm_bsp_interrupt_ctrl` shall enable the IRQN interrupt when invoked with an input of 1, and shall disable it when invoked with an input of 0. | &mdash;                                                                                                                                                                      |

#### 3.1.2 Bus wrapper functions

The bus wrapper implements the `nm_bus_*` and `nm_spi_rw` interfaces declared
in `winc1500/bus_wrapper/include/nm_bus_wrapper.h`, restricted to SPI transport.

For the purposes of this subsection:

- The *module-required SPI configuration* and the *module-required DMA
  parameters* are defined in section 3.4.
- An *SPI event* is a transmit-complete, receive-complete, transmit-receive-
  complete or error IRQ.

| ID         | Requirement                                                                                                                                                                                   | Rationale                                                                                                                                                                                                            |
|------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| REQ-FUN-12 | On invocation, `nm_bus_init` shall configure the SPI interface to the module-required SPI configuration, replacing any configuration in effect at entry.                                      | The SPI configuration in its entirety is crucial to establishing communication with the module. Ensuring its compliance with the module requirements becomes the middleware's responsibility rather than the user's. |
| REQ-FUN-13 | When the DMA option is enabled, `nm_bus_init` shall invoke the the porting API to initialise SPI TX and RX DMA streams.                                                                       | Since DMA configuration is largely inconsequential to the driver functionality, more liberty is given to the user then with the SPI configuration.                                                                   |
| REQ-FUN-14 | When the DMA option is enabled, `nm_bus_init` shall ensure that each DMA stream holds the module-required DMA parameters, and shall reconfigure any stream that does not.                     | A minimal set of DMA parameters remains module-critical, so the wrapper validates and self-heals them.                                                                                                               |
| REQ-FUN-15 | When the DMA option is enabled, `nm_bus_init` shall link SPI interface and DMA controller, and shall enable DMA interrupts in the NVIC.                                                       | &mdash;                                                                                                                                                                                                              |
| REQ-FUN-16 | `nm_bus_init` shall invoke the porting API to register a handler for each SPI event, and shall enable SPI interrupts in the NVIC.                                                             | Interrupt-driven transfers enable power-efficient synchronisation mechanisms, compared to timeout-based transfers which force polling.                                                                               |
| REQ-FUN-17 | `nm_bus_init` shall leave the Slave Select signal deasserted on completion.                                                                                                                   | The module is disabled by default until a call is made to explicity enable it.                                                                                                                                       |
| REQ-FUN-18 | `nm_bus_init` shall invoke the BSP API to perform a module reset.                                                                                                                             | The module is powered on only once the interface is ready.                                                                                                                                                           |
| REQ-FUN-19 | `nm_bus_init` shall return `M2M_SUCCESS` on success, and a negative integer on failure to fulfill any of the above requirements.                                                              | The vendor contract requires it.                                                                                                                                                                                     |
| REQ-FUN-20 | On invocation, `nm_bus_deinit` shall deinitialise the Slave Select pin and deregister any SPI event handlers.                                                                                 | &mdash;                                                                                                                                                                                                              |
| REQ-FUN-21 | `nm_bus_deinit` shall not affect the SPI and DMA interfaces in any respect.                                                                                                                   | Ownership of the SPI and DMA peripherals stays with the user.                                                                                                                                                        |
| REQ-FUN-22 | `nm_bus_deinit` shall return `M2M_SUCCESS` on success, and a negative integer on failure to fulfill any of the above requirements.                                                            | The vendor contract requires it.                                                                                                                                                                                     |
| REQ-FUN-23 | On invocation, `nm_bus_ioctl` shall use its first parameter to select the bus I/O function to which it redirects.                                                                             | The first parameter acts as a dispatch key selecting among the bus operations exposed by the interface (`nm_bus_wrapper.h`).                                                                                         |
| REQ-FUN-24 | `nm_bus_ioctl` shall return `M2M_ERR_INVALID_ARG` when called with a first parameter that has no corresponding bus I/O function.                                                              | The SPI read/write command is the only operation the driver issues in SPI mode; unrecognised commands are rejected per the vendor contract.                                                                          |
| REQ-FUN-25 | `nm_bus_ioctl` shall repurpose its second parameter to pass to the selected bus I/O function as that function's parameters.                                                                   | The second parameter is forwarded so each bus operation receives its own argument structure.                                                                                                                         |
| REQ-FUN-26 | `nm_bus_ioctl` shall return the return value of the selected bus I/O function.                                                                                                                | The vendor contract requires propagating the result of the performed operation.                                                                                                                                      |
| REQ-FUN-27 | On invocation, `nm_spi_rw` shall return `M2M_ERR_BUS_FAIL` when invoked with a transfer size of zero.                                                                                         | The vendor contract requires it.                                                                                                                                                                                     |
| REQ-FUN-28 | `nm_spi_rw` shall return `M2M_ERR_BUS_FAIL` when invoked with both the transmit and receive buffers NULL.                                                                                     | The vendor contract requires it.                                                                                                                                                                                     |
| REQ-FUN-29 | When invoked with a transmit buffer and a NULL receive buffer, `nm_spi_rw` shall transmit the specified number of bytes and shall return `M2M_SUCCESS` on completion.                         | Half-duplex transmit path.                                                                                                                                                                                           |
| REQ-FUN-30 | When invoked with a receive buffer and a NULL transmit buffer, `nm_spi_rw` shall receive the specified number of bytes and shall return `M2M_SUCCESS` on completion.                          | Half-duplex receive path.                                                                                                                                                                                            |
| REQ-FUN-31 | When invoked with both non-NULL transmit and receive buffers, `nm_spi_rw` shall transmit and receive the specified number of bytes concurrently and shall return `M2M_SUCCESS` on completion. | Full-duplex transmit and receive path.                                                                                                                                                                               |
| REQ-FUN-32 | `nm_spi_rw` shall assert the Slave Select signal before starting each SPI transfer and shall deassert it after the transfer completes.                                                        | &mdash;                                                                                                                                                                                                              |
| REQ-FUN-33 | `nm_spi_rw` shall return `M2M_ERR_BUS_FAIL` when a transfer fails.                                                                                                                            | The vendor contract requires the wrapper to report transfer failure.                                                                                                                                                 |

No requirement is made on `nm_bus_reinit`, as it is unused in the current driver
version.

#### 3.1.3 Synchronisation and bus ownership

The middleware does not own the SPI bus, the SPI peripheral or the DMA
peripheral; these may be shared with the user application. It therefore relies
on user-specified mechanisms, exposed through the porting interface, for bus
exclusivity and operation synchronisation.

| ID         | Requirement                                                                                                                                               | Rationale                                                                                                                      |
|------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------|
| REQ-FUN-34 | `nm_spi_rw` shall invoke the porting API to acquire the SPI bus before asserting the Slave Select signal.                                                 | The SPI bus may be shared, so the wrapper claims it before any transfer.                                                       |
| REQ-FUN-35 | `nm_spi_rw` shall invoke the porting API to release the SPI bus after deasserting the Slave Select signal.                                                | The bus is freed to allow for any pending accesses to proceed.                                                                 |
| REQ-FUN-36 | `nm_spi_rw` shall invoke the porting API to prepare the synchronisation mechanism, before starting each SPI transfer.                                     | &mdash;                                                                                                                        |
| REQ-FUN-37 | `nm_spi_rw` shall invoke the porting API to block middleware execution until transfer completion or failure, before it deasserts the Slave Select signal. | The vendor contract requires the wrapper to report transfer success and failure, hence it must block until either takes place. |
| REQ-FUN-38 | On each SPI event, the handler shall invoke the porting API to signal transfer completion or failure.                                                     | Prevents `nm_spi_rw` from deadlocking.                                                                                         |

### 3.2 Performance

This subsection specifies the measurable performance characteristics that the
middleware shall meet.

| ID          | Requirement                                                                                               | Rationale                                                                  |
|-------------|-----------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------|
| REQ-PERF-01 | The SPI clock frequency shall not exceed 48 MHz.                                                          | The datasheet specifies this as the maximum supported SPI clock frequency. |
| REQ-PERF-02 | The middleware shall define a maximum SPI transfer size, `egstrNmBusCapabilities `, of at least 16 bytes. | The vendor contract requires it (`nm_bus_wrapper.h`)                       |

### 3.3 Usability

This subsection specifies the requirements that ease the adaptation of the
middleware to a target application by the user (defined
[earlier](#1311-user-interface)). The target platform is the STM32F401xE family
of microcontrollers; the middleware is not required to function on any other
platform.

| ID         | Requirement                                                                                                                                                 | Rationale                                                                                                           |
|------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------|
| REQ-USE-01 | Adapting the middleware to a target application shall not require changes to the vendor driver or to the board and transport abstraction layer.             | All application-specific behaviour is confined to the porting interface and its implementation.                     |
| REQ-USE-02 | The middleware shall obtain SPI, DMA and EXTI interrupt delivery through the porting interface, and shall not claim exclusive ownership of those resources. | As there is no reason to make an exclusive claim to either of SPI, DMA and EXTI peripherals, they remain shareable. |
| REQ-USE-03 | Selecting between DMA-based and interrupt-based SPI transfers shall require a single configuration change in the porting interface.                         | &mdash;                                                                                                             |
| REQ-USE-04 | The middleware shall determine transfer-completion blocking through the porting interface without imposing a particular blocking mechanism.                 | The blocking mechanism is user-defined in the porting interface, so the user may adopt any waiting strategy.        |

### 3.4 Interface

This subsection specifies the requirements that apply to the hardware interface
exposed by the middleware to the WINC1500 module on one side, and the software
interface exposed to the user application &mdash; the porting interface &mdash;
on the other.

#### 3.4.1 Hardware Interface

|    ID     | Requirement                                                                                                                                                                                                                                                                                            | Rationale                                                                        |
|:---------:|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------|
| REQ-IF-01 | The board &amp; transport layer shall expose the module control signals &mdash; CHIP_EN, RESET_N, IRQN &mdash; through the user-defined pins.                                                                                                                                                          | &mdash;                                                                          |
| REQ-IF-02 | The board &amp; transport layer shall expose the SPI data interface &mdash; SCK, MOSI, MISO &mdash; and SS through the user-defined pins.                                                                                                                                                              | &mdash;                                                                          |
| REQ-IF-03 | The board &amp; transport layer shall configure the SPI interface as a full-duplex master, with 8-bit data, most-significant-bit-first, clock polarity low and phase first-edge, software-controlled SS, and an SPI clock within the limit set by REQ-PERF-01.                                         | This configuration is required by the module for SPI communication.              |
| REQ-IF-04 | When the DMA option is enabled, the board &amp; transport layer shall require the SPI TX and RX DMA streams to hold the module-required DMA parameters: memory-to-peripheral / peripheral-to-memory direction (respectively), peripheral-increment disabled, memory-increment enabled and normal mode. | These configurations ensure data flow correctness and prevent memory corruption. |

#### 3.4.2 Porting Interface

|    ID     | Requirement                                                                                                                                                                                                                                                                                                                                                                                    |
|:---------:|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| REQ-IF-05 | The porting interface shall specify the GPIO port and pin for each module control and communication signal: IRQN, CHIP_EN, RESET_N, SS, and the SPI data lines (SCK, MOSI, MISO).                                                                                                                                                                                                              |
| REQ-IF-06 | The porting interface shall provide mechanisms to register and deregister an ISR callback for the IRQN EXTI signal.                                                                                                                                                                                                                                                                            |
| REQ-IF-07 | The porting interface shall provide mechanisms to register and deregister an ISR callback for each SPI event: transmit-complete, receive-complete, transmit-receive-complete, and error.                                                                                                                                                                                                       |
| REQ-IF-08 | The porting interface shall provide an acquire operation that blocks the invoking thread until the SPI bus is exclusively available, and a corresponding release operation.                                                                                                                                                                                                                    |
| REQ-IF-09 | The porting interface shall provide: a prepare operation that resets transfer-completion state before each transfer; a wait operation that blocks the invoking thread until completion or failure; a notify-success operation and a notify-failure operation that unblock a pending wait and record the outcome; and a means to query whether the last completed transfer succeeded or failed. |
| REQ-IF-10 | When the DMA option is enabled, the porting interface shall additionally identify the SPI TX and RX DMA streams and provide an initialisation function for each.                                                                                                                                                                                                                               |
| REQ-IF-11 | The porting interface shall provide a system clock configuration function.                                                                                                                                                                                                                                                                                                                     |

### 3.5 Design Constraints

This subsection lists the design constraints imposed on the middleware by the
vendor contract, the target platform and the project limitations. They restrict
the design freedom of the middleware, but do not of themselves describe
behaviour in response to an invocation.

| ID         | Requirement                                                                                                                                                                                      |
|------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| REQ-DES-01 | The middleware shall implement the BSP and bus wrapper interfaces exactly as declared in the vendor headers `winc1500/bsp/include/nm_bsp.h` and `winc1500/bus_wrapper/include/nm_bus_wrapper.h`. |
| REQ-DES-02 | The middleware shall not require modifications to the vendor driver files.                                                                                                                       |
| REQ-DES-03 | The middleware shall only be required to implement the SPI transport of the bus wrapper; the UART and I2C transports shall be optional to implement.                                             |
| REQ-DES-04 | The middleware shall require a configuration setting that selects a supported bus transport; a configuration selecting no transport, or an unsupported one, shall be rejected at compile time.   |
| REQ-DES-05 | The middleware shall target the STM32F401xE family of microcontrollers. Hence, it is encouraged to take advantage of platform-specific behaviour during implementation.                          |
| REQ-DES-06 | The middleware shall be implemented in the C11 / GNU11 language standard and shall have the STM32 HAL and the STM32 CMSIS headers as platform dependencies.                                      |
| REQ-DES-07 | The middleware shall be developed against the WINC1500 driver and module firmware version 19.7.11.                                                                                               |

### 3.6 Software System Attributes

This subsection specifies the attributes that the middleware implementation shall
possess beyond its functional behaviour, namely reliability, maintainability and
portability. Each attribute is stated as a single, verifiable requirement.

#### 3.6.1 Reliability

| ID          | Requirement                                                                                                                                                                                                                       | Rationale                                                                                                                          |
|-------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------|
| REQ-ATTR-01 | The middleware shall perform SPI data transfers from the invoking thread context only; it shall not perform any SPI transfer from within an interrupt handler.                                                                    | Transfers block the invoking context until completion (REQ-FUN-37), whereas ISRs should reduce execution time as much as possible. |
| REQ-ATTR-02 | The middleware may implement recovery from SPI transfer failures internally, provided that failures not resolved by the recovery mechanism are reported to the invoking driver as error return values, as required by REQ-FUN-33. | Recovery is a design freedom of the middleware, but the driver depends on accurate failure reporting to run its own retry logic.   |
| REQ-ATTR-03 | The board and transport layer shall make regulate access to its internally shared state in a thread-safe manner.                                                                                                                  | &mdash;                                                                                                                            |

#### 3.6.2 Maintainability

| ID          | Requirement                                                                                                       | Rationale                                                                                                      |
|-------------|-------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------|
| REQ-ATTR-04 | The middleware shall document the functions of its public API with Doxygen.                                       | Consistent API documentation supports maintainers in understanding the middleware contract.                    |

#### 3.6.3 Portability

| ID          | Requirement                                                                                                                                                                  |
|-------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| REQ-ATTR-05 | Porting the middleware shall not require changes to the board and transport layer implementation, but shall only require an appropriate implementation of porting interface. |

## 5. Appendices

### 5.1 Assumptions and dependencies

The middleware assumes a WINC1500 driver and module firmware of version 19.7.11.

The middleware depends on the STM32 HAL, and on the user to supply a porting
implementation.