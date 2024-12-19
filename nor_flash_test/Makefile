default: hello.elf

hello.o: hello.c
	riscv32-unknown-elf-gcc -c -g -O0 -ffreestanding -march=rv32i -mabi=ilp32 -o hello.o hello.c

start.o: start.s
	riscv32-unknown-elf-as -g -march=rv32i -mabi=ilp32 -o start.o start.s

s29gl256.o: s29gl256.c
	riscv32-unknown-elf-gcc -c -g -O0 -ffreestanding -march=rv32i -mabi=ilp32 -o s29gl256.o s29gl256.c

hello.elf: hello.o start.o s29gl256.o baremetal.ld
	riscv32-unknown-elf-ld -T baremetal.ld -m elf32lriscv -o hello.elf hello.o s29gl256.o start.o

run: hello.elf
	@echo "Ctrl-A C for QEMU console, then quit to exit"
	qemu-system-riscv32 -nographic -serial mon:stdio -machine virt -bios hello.elf

debug: hello.elf
	@echo "Ctrl-A C for QEMU console, then quit to exit"
	qemu-system-riscv32 -nographic -serial mon:stdio -machine virt -s -S -bios hello.elf

clean:
	rm -f hello.o start.o hello.elf
