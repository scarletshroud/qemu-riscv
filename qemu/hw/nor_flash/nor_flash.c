#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/log.h"
#include "hw/sysbus.h"
#include "chardev/char.h"
#include "hw/hw.h"
#include "hw/irq.h"
#include "hw/nor_flash/nor_flash.h"

#define MANUFACTURER_ID_ADDR        0x00
#define CHIP_ID_ADDR                0x01
#define SECTOR_SECURITY_STATUS_ADDR 0x02

#define MANUFACTURER_ID 0x01;
#define CHIP_ID         0x22;

#define F_UNLOCK_ADDR           0x555
#define S_UNLOCK_ADDR           0x2AA

#define F_UNLOCK_CMD            0xAA
#define S_UNLOCK_CMD            0x55

#define PROGRAM_CMD             0xA0
#define ENABLE_AUTOSELECT_CMD   0x90
#define RESET_CMD               0xF0
#define ERASE_CMD               0x80
#define CHIP_ERASE_CMD          0x10
#define SECTOR_ERASE_CMD        0x30
#define PAGE_ERASE_CMD          0x50
#define UNLOCK_BYPASS_CMD       0x20
#define UNLOCK_BYPASS_RESET_CMD 0x00

static uint64_t flash_read(void *opaque, hwaddr addr, unsigned int size)
{
    flashState *s = opaque;

    if (s->autoselect_mode) {

        if (addr == MANUFACTURER_ID_ADDR) {
            return MANUFACTURER_ID;
        }

        if (addr == CHIP_ID_ADDR) {
            return CHIP_ID;
        }

        if (addr == SECTOR_SECURITY_STATUS_ADDR) {

        }

    }

    return s->mem[addr];
}

static void flash_write(void *opaque, hwaddr addr, uint64_t value, unsigned int size)
{
    flashState *s = opaque;
    unsigned char byte = value;

    switch (s->lock_status) {
        case LOCKED:
            if (addr == F_UNLOCK_ADDR && byte == F_UNLOCK_CMD) {
                s->lock_status = WAIT_FOR_UNLOCK;
            }
            break;
            
        case WAIT_FOR_UNLOCK:
            if (addr == S_UNLOCK_ADDR && byte == S_UNLOCK_CMD) {
                s->lock_status = FULLY_UNLOCKED;
            }
            break;

        case FULLY_UNLOCKED:
            if (addr == F_UNLOCK_ADDR && byte == ENABLE_AUTOSELECT_CMD) {
                s->autoselect_mode = 1;
            }

            if (addr == F_UNLOCK_ADDR && byte == UNLOCK_BYPASS_CMD) {
                s->bypass_mode = 1;
            }

            if (addr == F_UNLOCK_ADDR && byte == PROGRAM_CMD) {
                s->lock_status = PROGRAM;
            }
            break;

        case PROGRAM:

            if (s->mem[addr] == 0xFF) {
                
            }

            s->mem[addr] = byte;
            s->lock_status = LOCKED;
            break;
    }
}

static void flash_init(flashState *s) {
    s->bypass_mode = 0;
    s->autoselect_mode = 0;
    s->status_reg = 0;
    s->lock_status = LOCKED;
    for (uint32_t i = 0; i < NOR_FLASH_MEMORY_SIZE; ++i) {
        s->mem[i] = 0xFF;
    }
}

static const MemoryRegionOps flash_ops = {
    .read = flash_read,
    .write = flash_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 1,
        .max_access_size = 6}
};

flashState *flash_create(MemoryRegion *address_space, hwaddr base)
{
    flashState *s = g_malloc0(sizeof(flashState));
    flash_init(s);
    memory_region_init_io(&s->mmio, NULL, &flash_ops, s, TYPE_NOR_FLASH, NOR_FLASH_MEMORY_SIZE);
    memory_region_add_subregion(address_space, base, &s->mmio);
    return s;
}