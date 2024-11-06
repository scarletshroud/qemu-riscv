default: hello.elf

hello.o: hello.c
	riscv32-unknown-elf-gcc -c -g -O0 -ffreestanding -march=rv32i -mabi=ilp32 -o hello.o hello.c

start.o: start.s
	riscv32-unknown-elf-as -g -march=rv32i -mabi=ilp32 -o start.o start.s

hello.elf: hello.o start.o linker.ld
	riscv32-unknown-elf-ld -T linker.ld -m elf32lriscv -o hello.elf hello.o start.o

run: hello.elf
	qemu-system-riscv32 -nographic -serial mon:stdio -machine virt -bios hello.elf

clean:
	rm -f hello.o start.o hello.elf