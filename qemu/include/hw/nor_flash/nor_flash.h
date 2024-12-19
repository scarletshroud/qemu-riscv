#ifndef HW_NOR_FLASH_H
#define HW_NOR_FLASH_H

#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_NOR_FLASH "NOR_FLASH"

#define NOR_FLASH_MEMORY_SIZE 2048

typedef struct flashState flashState;
DECLARE_INSTANCE_CHECKER(flashState, NOR_FLASH, TYPE_NOR_FLASH)

typedef enum {
    LOCKED,
    WAIT_FOR_UNLOCK, 
    FULLY_UNLOCKED,
    PROGRAM
} lockStatus;

struct flashState
{
    MemoryRegion mmio;
    uint8_t autoselect_mode;
    uint8_t bypass_mode;
    lockStatus lock_status;
    uint32_t status_reg;
    unsigned char mem[NOR_FLASH_MEMORY_SIZE];
};

flashState *flash_create(MemoryRegion *address_space, hwaddr base);

#endif 