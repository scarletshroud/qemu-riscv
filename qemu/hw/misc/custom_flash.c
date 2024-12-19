#include "hw/sysbus.h"
#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "hw/misc/custom_flash.h"

#define FLASH_SIZE (64 * 1024)  // 64 KB of flash memory

typedef struct {
    uint8_t memory[FLASH_SIZE];
    uint32_t status;
} CustomFlashState;

static void custom_flash_reset(DeviceState *dev) {
    CustomFlashState *s = CUSTOM_FLASH(dev);
    memset(s->memory, 0xFF, FLASH_SIZE);  // Flash memory initialized to 0xFF (erased state)
    s->status = 0;
}

static uint64_t custom_flash_read(void *opaque, hwaddr addr, unsigned size) {
    CustomFlashState *s = opaque;
    if (addr + size > FLASH_SIZE) {
        qemu_log_mask(LOG_GUEST_ERROR, "custom_flash: Read out of bounds: 0x%" HWADDR_PRIx "\n", addr);
        return 0;
    }
    return *(uint32_t *)(s->memory + addr);
}

static void custom_flash_write(void *opaque, hwaddr addr, uint64_t value, unsigned size) {
    CustomFlashState *s = opaque;

    if (addr + size > FLASH_SIZE) {
        qemu_log_mask(LOG_GUEST_ERROR, "custom_flash: Write out of bounds: 0x%" HWADDR_PRIx "\n", addr);
        return;
    }

    // Simulate flash write (only allows changing bits from 1 to 0, not 0 to 1)
    uint32_t *flash_ptr = (uint32_t *)(s->memory + addr);
    *flash_ptr &= value;
}

static const MemoryRegionOps custom_flash_ops = {
    .read = custom_flash_read,
    .write = custom_flash_write,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4,
    },
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void custom_flash_realize(DeviceState *dev, Error **errp) {
    CustomFlashState *s = CUSTOM_FLASH(dev);
    memory_region_init_io(&s->mmio, OBJECT(dev), &custom_flash_ops, s, "custom_flash", FLASH_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(dev), &s->mmio);
}

static void custom_flash_class_init(ObjectClass *klass, void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = custom_flash_reset;
}

static const TypeInfo custom_flash_info = {
    .name          = TYPE_CUSTOM_FLASH,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(CustomFlashState),
    .class_init    = custom_flash_class_init,
};

static void custom_flash_register_types(void) {
    type_register_static(&custom_flash_info);
}

type_init(custom_flash_register_types);
