
/*!
*   \file example.c
*   \brief Данный файл содержит пример работы с памятью S29GL256, установленной в параллельном режиме.
*   \author Zinovichev E.S.
*/

#include "s29gl256.h"

#include <stdint.h>

#define MANUFACTURER_ID 0x1
#define CHIP_ID 0x22

#define ARRAY_SIZE 512
#define TEST_ADDR 0xbe000000

#define _CHIP_ERASE_SCENARIO_
//#define _PAGE_ERASE_SCENARIO_
//#define _SECTOR_ERASE_SCENARIO_

static void fill_array(uint8_t* array, uint32_t size) {
    for (uint32_t i = 0; i < size; ++i) {
        array[i] = i;
    }
}

static uint8_t verify_result(uint8_t* array, uint8_t* result_array, uint32_t size) {
    for (uint32_t i = 0; i < size; ++i) {
        if (array[i] != result_array[i]) {
            return 1;
        }
    }

    return 0;
}

int main() {
    uint8_t manufacturer_id = 0;
    uint8_t chip_id = 0;
    uint8_t bypass_mode = 1;
    uint8_t verify_status = 0;
    uint8_t array, read_buffer[ARRAY_SIZE];

    S29GL256_status_t write_status, erase_status;

    fill_array(array, ARRAY_SIZE);

	/* Read IDs */

	manufacturer_id = s29gl256_read_manufacturer_id();

	if (manufacturer_id != MANUFACTURER_ID) {
		return -1;
	}

	chip_id = s29gl256_read_chip_id();

	if (chip_id != CHIP_ID) {
		return -1;
	}

    /* Simple array write & read */

    write_status = s29gl256_write(array, 0x00000000, ARRAY_SIZE, bypass_mode);

    if (write_status != S29GL256_STATUS_OK) {
        return -1;
    }

   s29gl256_read(read_buffer, 0xbfbfffa, 512);

   verify_status = verify_result(array, read_buffer, ARRAY_SIZE);

   if (verify_status) {
       return -1;
   }

#ifdef _PAGE_ERASE_SCENARIO_
    /* Page Erase */
    erase_status = s29gl256_page_erase(TEST_ADDR);

    if (erase_status != s29gl256_STATUS_OK) {
        return -1;
    }
#endif

#ifdef _SECTOR_ERASE_SCENARIO_
    /* Sector Erase */
    erase_status = s29gl256_sector_erase(s29gl256_SA1);

    if (erase_status != s29gl256_STATUS_OK) {
        return -1;
    }
#endif

#ifdef _CHIP_ERASE_SCENARIO_
    /* Chip Erase */
    erase_status = s29gl256_chip_erase();

    if (erase_status != S29GL256_STATUS_OK) {
        return -1;
    }
#endif

    /*  Make sure that data is cleared  */

    s29gl256_read(read_buffer, TEST_ADDR, ARRAY_SIZE);

    verify_status = verify_result(array, read_buffer, ARRAY_SIZE);

    if (!verify_status) {
        return -1;
    }

    return 0;
}
