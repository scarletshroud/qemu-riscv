# qemu-riscv

## Пререквезиты для установки эмулятора QEMU и RISC-V toolchain 


```
sudo apt-get install autoconf automake autotools-dev curl python3 libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build device-tree-compiler```  
```

### Сборка RISC-V toolchain

1. Клонируем репозиторий с RISC-V toolchain:  
```bash
git clone https://github.com/riscv/riscv-gnu-toolchain
```

2. Далее осуществляем переход в склонированный репозиторий:  
```bash 
cd riscv-gnu-toolchain
```

3. Осуществляем конфигурацию и сборку проекта:
```bash
./configure --prefix=/opt/riscv --with-arch=rv32i --with-multilib-generator="rv32i-ilp32--;rv32ima-ilp32-- rv32imafd-ilp32--"
make -j$(nproc)
```

4. Добавляем собранный toolchain в PATH для запуска из терминала:
```bash
export PATH="/opt/riscv/bin:$PATH"
```

5. Осуществляем проверку корректности установки RISC-V toolchain:

```bash
which riscv32-unknown-elf-as
riscv32-unknown-elf-as --version
riscv32-unknown-elf-gcc --version
```

6. Получаем следующий вывод:
```
/opt/riscv/bin/riscv32-unknown-elf-as
GNU assembler (GNU Binutils) 2.43.1
Copyright (C) 2024 Free Software Foundation, Inc.
This program is free software; you may redistribute it under the terms of
the GNU General Public License version 3 or later.
This program has absolutely no warranty.
This assembler was configured for a target of `riscv32-unknown-elf'.
riscv32-unknown-elf-gcc (g04696df0963) 14.2.0
Copyright (C) 2024 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

### Сборка QEMU

1. Клонируем репозиторий с QEMU: 
```bash
git clone https://gitlab.com/qemu-project/qemu.git
```

2. Осуществляем конфигурацию и сборку проекта:  
```bash
./configure --target-list=riscv32-softmmu --prefix=/opt/qemu32
make -j$(nproc)  
make install
```

3. Добавляем собранный toolchain в PATH для запуска из терминала:
```bash
export PATH="/opt/qemu32/bin:$PATH"
```

4. Осуществляем проверку корректности установки эмулятора QEMU:
```bash
which qemu-system-riscv32
qemu-system-riscv32 --version
qemu-system-riscv32 -machine help
```

5. Получаем следующий вывод:
```
/opt/qemu32/bin/qemu-system-riscv32
QEMU emulator version 9.1.50 (v9.1.0-1334-gdaaf51001a-dirty)
Copyright (c) 2003-2024 Fabrice Bellard and the QEMU Project developers
Supported machines are:
none                 empty machine
opentitan            RISC-V Board compatible with OpenTitan
sifive_e             RISC-V Board compatible with SiFive E SDK
sifive_u             RISC-V Board compatible with SiFive U SDK
spike                RISC-V Spike board (default)
virt                 RISC-V VirtIO board
```

qemu-system-riscv32 -machine virt -m 128M -bios none -device loader,file=./prog -nographic -s

export PATH="/opt/riscv/bin:$PATH"
export PATH="/opt/qemu32/bin:$PATH"


### Добавление периферийного устройства в QEMU

Создаем папку nor_flash в директории hw

В ней создаем файл meson.build, в котором указываем 

```system_ss.add(when: 'CONFIG_NOR_FLASH', if_true: files('nor_flash.c'))```

И файл Kconfig:

```
config NOR_FLASH
    bool
```

В папке hw/Kconfig указываем под Kconfig
``` source nor_flash/Kconfig ```

В meson.build:
```
subdir('nor_flash')
```

В inlclude/hw/riscv/virt.h:

```c
enum {
    VIRT_DEBUG,
    VIRT_MROM,
    VIRT_TEST,
    VIRT_RTC,
    VIRT_CLINT,
    VIRT_ACLINT_SSWI,
    VIRT_PLIC,
    VIRT_APLIC_M,
    VIRT_APLIC_S,
    VIRT_UART0,
    VIRT_VIRTIO,
    VIRT_FW_CFG,
    VIRT_IMSIC_M,
    VIRT_IMSIC_S,
    VIRT_FLASH,
    VIRT_DRAM,
    VIRT_PCIE_MMIO,
    VIRT_PCIE_PIO,
    VIRT_PLATFORM_BUS,
    VIRT_PCIE_ECAM,

    /* Добавляем */

    VIRT_NOR_FLASH
};
```

В файле hw/riscv/virt.c:

1. Выполним подключение заголовочного файла:

```c
#include "include/hw/nor_flash/nor_flash.h" 
```

2. Определим диапазон используемых адресов для устройства:

```c
static const MemMapEntry virt_memmap[] = {
    [VIRT_DEBUG] =        {        0x0,         0x100 },
    [VIRT_MROM] =         {     0x1000,        0xf000 },
    [VIRT_TEST] =         {   0x100000,        0x1000 },
    [VIRT_RTC] =          {   0x101000,        0x1000 },
    [VIRT_CLINT] =        {  0x2000000,       0x10000 },
    [VIRT_ACLINT_SSWI] =  {  0x2F00000,        0x4000 },
    [VIRT_PCIE_PIO] =     {  0x3000000,       0x10000 },
    [VIRT_PLATFORM_BUS] = {  0x4000000,     0x2000000 },
    [VIRT_PLIC] =         {  0xc000000, VIRT_PLIC_SIZE(VIRT_CPUS_MAX * 2) },
    [VIRT_APLIC_M] =      {  0xc000000, APLIC_SIZE(VIRT_CPUS_MAX) },
    [VIRT_APLIC_S] =      {  0xd000000, APLIC_SIZE(VIRT_CPUS_MAX) },
    [VIRT_UART0] =        { 0x10000000,         0x100 },
    [VIRT_VIRTIO] =       { 0x10001000,        0x1000 },
    [VIRT_FW_CFG] =       { 0x10100000,          0x18 },
    [VIRT_FLASH] =        { 0x20000000,     0x4000000 },
    [VIRT_IMSIC_M] =      { 0x24000000, VIRT_IMSIC_MAX_SIZE },
    [VIRT_IMSIC_S] =      { 0x28000000, VIRT_IMSIC_MAX_SIZE },
    [VIRT_PCIE_ECAM] =    { 0x30000000,    0x10000000 },
    [VIRT_PCIE_MMIO] =    { 0x40000000,    0x40000000 },
    
    /* Область памяти для созданного устройства */
    [VIRT_NOR_FLASH] =    { 0x60000000,         0x2048},
    [VIRT_DRAM] =         { 0x80000000,           0x0 },
}; 
```

В данном случае устройство отображается на диапазон адресов 0x60000000. Для него зарезервировано 2 Кб. 

3. Вызываем функцию инициализации перифирийного устройства в функции ```virt_machine_init```:

```c
flash_create(system_memory, memmap[VIRT_NOR_FLASH].base);
```


В файле hw/riscv/Kconfig добавим наше устройство:

```c
config RISCV_VIRT
    bool
    default y
    depends on RISCV32 || RISCV64
    imply PCI_DEVICES
    imply VIRTIO_VGA
    imply TEST_DEVICES
    imply TPM_TIS_SYSBUS
    select DEVICE_TREE
    select RISCV_NUMA
    select GOLDFISH_RTC
    select PCI
    select PCI_EXPRESS_GENERIC_BRIDGE
    select PFLASH_CFI01
    select SERIAL_MM
    select RISCV_ACLINT
    select RISCV_APLIC
    select RISCV_IMSIC
    select SIFIVE_PLIC
    select SIFIVE_TEST
    select SMBIOS
    select VIRTIO_MMIO
    select FW_CFG_DMA
    select PLATFORM_BUS
    select ACPI
    select ACPI_PCI

    /*Добавляем устройство */
    select NOR_FLASH
```
