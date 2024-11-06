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