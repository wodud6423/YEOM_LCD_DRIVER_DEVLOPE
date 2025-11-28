# Beaglebone Black LCD Driver Project

## Target Board
**Beaglebone Black**

## Module
**1602A LCD**

## Project Overview
This project demonstrates a **platform-based LCD driver** development for the Beaglebone Black.  
It is designed to enhance device driver development capabilities by providing an example of GPIO-based LCD control.

- Utilizes the **Beaglebone Black Expansion Header pins**.
- Based on the **Linux GPIO consumer interface** (`linux/gpio/consumer.h`) for **Linux kernel version 5.4**.

---

## Directory Structure
├── Makefile
├── README.md
├── dts
│ └── am335x-beonblack-lddcourse_copy.dtsi
├── include
│ ├── char_copy.h
│ ├── gpio_copy.h
│ └── lcd_copy.h
├── output
│ ├── gpio-sysfs_copy.ko
│ ├── gpio-sysfs_copy.mod
│ ├── gpio-sysfs_copy.mod.o
│ ├── gpio-sysfs_copy.o
│ ├── gpio.o
│ ├── lcd.o
│ ├── lcd_app.elf
│ └── lcd_app.o
└── source
├── Makefile
├── Module.symvers
├── gpio-sysfs_copy.c
├── gpio-sysfs_copy.mod.c
├── gpio_copy.c
├── lcd_app_copy.c
├── lcd_copy.c
└── modules.order

---

## Description
The project provides a basic framework for controlling the **1602A LCD** via GPIO from the Linux kernel space.  

- **lcd_copy.c / lcd_app_copy.c**: Implements low-level LCD functions and sample application.
- **gpio_copy.c / gpio-sysfs_copy.c**: Provides GPIO interface for the LCD driver.
- **include/**: Header files for function declarations and macros.
- **output/**: Compiled kernel modules (`.ko`), object files (`.o`), and application binaries (`.elf`).
- **dts/**: Device tree source files for configuring pins on the Beaglebone Black.

---

## Notes
- Detailed information and usage instructions will be provided in the respective `README.md` files in each directory.  
- The project will be further documented after the entire codebase has been refactored.

---

## License
This project is licensed under the **GPL License**. See individual source files and the `LICENSE` file for details.

