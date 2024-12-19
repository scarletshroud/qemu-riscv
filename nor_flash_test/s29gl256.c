/*!
*   \file S29GL256.c
*   \brief Данный файл содержит определения функций по работе с памятью S29GL256 в параллельном режиме.
*   \author Zinovichev E.S.
*/

#include "s29gl256.h"

#define FLASH_BASE_VADDR        0x60000000
#define FLASH_START_VADDR       0xBE000000

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

#define MANUFACTURER_ID_ADDR        0x00
#define CHIP_ID_ADDR                0x01
#define SECTOR_SECURITY_STATUS_ADDR 0x02

static const uint32_t const sector_addresses[] = {
    [S29GL256_SA1] = 0x0000000000,
    [S29GL256_SA2] = 0x0000040000,
    [S29GL256_SA3] = 0x0000080000,
    [S29GL256_SA4] = 0x00000C0000,
    [S29GL256_SA5] = 0x0000100000,
    [S29GL256_SA6] = 0x0000140000,
    [S29GL256_SA7] = 0x0000180000,
    [S29GL256_SA8] = 0x00001C0000,
};

static void delay() {
    for (uint32_t i = 0; i < 100; ++i) {
    }
}

/*!
    \brief              Функция программирования байта во Flash-память
    \param addr[in]     Адрес записи байта
    \param value[in]    Значение байта для записи
    \return             Статус операции
*/
static S29GL256_status_t program(const uint32_t addr, const uint8_t value, const uint8_t bypass_mode) {
    uint8_t mem_value;

    if (!bypass_mode) {
        *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = F_UNLOCK_CMD;
        *((volatile uint8_t*)FLASH_BASE_VADDR + (S_UNLOCK_ADDR)) = S_UNLOCK_CMD;
    }

    *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = PROGRAM_CMD;

    *((volatile uint8_t*)addr) = value;

    delay();

    mem_value = S29GL256_read_byte(addr);

    if (mem_value != value) {
        return S29GL256_STATUS_PROGRAM_ERR;
    }

    return S29GL256_STATUS_OK;
}

/*!
    \brief              Функция включения режима "Auto Select"
*/
static void enable_autoselect_mode() {
    *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = F_UNLOCK_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (S_UNLOCK_ADDR)) = S_UNLOCK_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = ENABLE_AUTOSELECT_CMD;
}

/*!
    \brief              Функция разблокировки режима bypass
*/
static void unlock_bypass() {
    *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = F_UNLOCK_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (S_UNLOCK_ADDR)) = S_UNLOCK_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = UNLOCK_BYPASS_CMD;
}

/*!
    \brief              Функция выхода из режима bypass
*/
static void bypass_reset() {
    *((volatile unsigned char*)(FLASH_BASE_VADDR)) = ENABLE_AUTOSELECT_CMD;
    *((volatile unsigned char*)(FLASH_BASE_VADDR)) = UNLOCK_BYPASS_RESET_CMD;
}

/*!
    \brief                  Функция ожидания стирания Flash-памяти
    \param flash_addr[in]   Адрес стирания
    \return                 Статус операции
*/
#define TIMEOUT_COUNTER_LIMIT 1000000
static S29GL256_status_t wait_flash_erasing(const uint32_t flash_addr) {
    uint8_t erase_status = 0;
    uint32_t timeout_counter = 0;

    erase_status = S29GL256_read_byte(flash_addr);

    while (!(erase_status >> 7)) { /* READ D7 bit */

        if (timeout_counter >= TIMEOUT_COUNTER_LIMIT) {
            return S29GL256_STATUS_ERASE_ERR;
        }

        erase_status = S29GL256_read_byte(FLASH_START_VADDR);

        timeout_counter++;
    }

    return S29GL256_STATUS_OK;
}

uint8_t S29GL256_read_manufacturer_id() {
    enable_autoselect_mode();
    return *((volatile uint8_t*)FLASH_BASE_VADDR + MANUFACTURER_ID_ADDR);
}

uint8_t S29GL256_read_chip_id() {
    enable_autoselect_mode();
    return *((volatile uint8_t*)FLASH_BASE_VADDR + CHIP_ID_ADDR);
}

uint8_t S29GL256_verify_sector_security(const uint32_t sector_addr) {
    enable_autoselect_mode();
    return *((volatile uint8_t*)sector_addr + MANUFACTURER_ID_ADDR);
}

uint8_t S29GL256_read_byte(const uint32_t addr) {
    return *((volatile uint8_t*)addr);
}

uint32_t S29GL256_read_word(const uint32_t addr) {
    return *((volatile uint32_t*)addr);
}

void S29GL256_read(uint8_t* const buffer, const uint32_t addr, const uint32_t size) {
    for (uint32_t i = 0; i < size; ++i) {
        buffer[i] = *((volatile uint8_t*)addr + i);
    }
}

#define MAX_WRITE_ATTEMPTS 3
S29GL256_status_t S29GL256_write(const uint8_t* const data, const uint32_t addr, const uint32_t size, const uint8_t bypass_mode) {
    S29GL256_status_t program_status;

    // if (addr < FLASH_START_VADDR) {
    //     return S29GL256_STATUS_ADDR_OUT_OF_RANGE;
    // }

    if (bypass_mode) {
        unlock_bypass();
    }

    for (uint32_t i = 0; i < size; ++i) {
        program_status = program(addr + i, data[i], bypass_mode);

        if (program_status != S29GL256_STATUS_OK) {
            uint8_t attempts = 0;

            S29GL256_reset();

            while (attempts < MAX_WRITE_ATTEMPTS) {
                program_status = program(addr + i, data[i], bypass_mode);

                if (program_status != S29GL256_STATUS_OK) {
                    ++attempts;

                    S29GL256_reset();

                    continue;
                }

                break;
            }

            if (attempts >= MAX_WRITE_ATTEMPTS) {
                return S29GL256_STATUS_PROGRAM_ERR;
            }

            return program_status;
        }
    }

    if (bypass_mode) {
        bypass_reset();
    }

    return 0;
}

S29GL256_status_t S29GL256_chip_erase() {
    *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = F_UNLOCK_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (S_UNLOCK_ADDR)) = S_UNLOCK_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = ERASE_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = F_UNLOCK_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (S_UNLOCK_ADDR)) = S_UNLOCK_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = CHIP_ERASE_CMD;

    return wait_flash_erasing(FLASH_START_VADDR);
}

S29GL256_status_t S29GL256_sector_erase(const S29GL256_sector_t sector) {
    uint32_t offset_address = sector_addresses[sector];

    if (sector < S29GL256_SA1 || sector > S29GL256_SA8) {
        return S29GL256_STATUS_ADDR_OUT_OF_RANGE;
    }

    *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = F_UNLOCK_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (S_UNLOCK_ADDR)) = S_UNLOCK_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = ERASE_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = F_UNLOCK_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (S_UNLOCK_ADDR)) = S_UNLOCK_CMD;
    *((volatile uint8_t*)FLASH_START_VADDR + (offset_address)) = SECTOR_ERASE_CMD;

    return wait_flash_erasing(FLASH_START_VADDR + offset_address);
}

S29GL256_status_t S29GL256_page_erase(const uint32_t page) {

    if (page < FLASH_START_VADDR) {
        return S29GL256_STATUS_ADDR_OUT_OF_RANGE;
    }

    *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = F_UNLOCK_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (S_UNLOCK_ADDR)) = S_UNLOCK_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = ERASE_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (F_UNLOCK_ADDR)) = F_UNLOCK_CMD;
    *((volatile uint8_t*)FLASH_BASE_VADDR + (S_UNLOCK_ADDR)) = S_UNLOCK_CMD;
    *((volatile uint8_t*)page) = PAGE_ERASE_CMD;

    return wait_flash_erasing(page);
}

void S29GL256_reset() {
    *((volatile unsigned char*)(FLASH_BASE_VADDR)) = RESET_CMD;
}
