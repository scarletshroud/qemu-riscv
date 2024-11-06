/*!
    \file S29GL256.h
    \brief Данный файл содержит объявления основных функций по работе с памятью S29GL256 в параллельном режиме.
    \author Zinovichev E.S.
*/

#ifndef _S29GL256_H_
#define _S29GL256_H_

#include <stdint.h>

typedef enum {
    S29GL256_STATUS_OK,
    S29GL256_STATUS_PROGRAM_ERR,
    S29GL256_STATUS_ERASE_ERR,
    S29GL256_STATUS_ADDR_OUT_OF_RANGE
} S29GL256_status_t;

typedef enum {
    S29GL256_SA1,
    S29GL256_SA2,
    S29GL256_SA3,
    S29GL256_SA4,
    S29GL256_SA5,
    S29GL256_SA6,
    S29GL256_SA7,
    S29GL256_SA8
} S29GL256_sector_t;

/*!
    \brief              Функция чтения одного 32-х разрядного слова
    \return             ID производителя: 0x01
*/
uint8_t     S29GL256_read_manufacturer_id();

/*!
    \brief              Функция чтения одного 32-х разрядного слова
    \return             ID микросхемы: 0xc8
*/
uint8_t     S29GL256_read_chip_id();

/*!
    \brief              Функция чтения одного 32-х разрядного слова
    \param addr[in]     Адрес сектора для верификации
    \return             Статус защищенности сектора (0x00 или 0x01)
*/
uint8_t     S29GL256_verify_sector_security(const uint32_t sector_addr);

/*!
    \brief              Функция чтения одного 32-х разрядного слова
    \param addr[in]     Адрес для чтения
    \return             32-х разрядное слово
*/
uint8_t     S29GL256_read_byte(const uint32_t addr);

/*!
    \brief              Функция чтения одного 32-х разрядного слова
    \param addr[in]     Адрес для чтения
    \return             32-х разрядное слово
*/
uint32_t    S29GL256_read_word(const uint32_t addr);

/*!
    \brief              Функция чтения массива байт
    \param buffer[out]  Указатель на буфер для хранения прочитанного массива
    \param addr[in]     Адрес начала диапазона памяти для чтения
    \param size[in]     Количество байт, которые необходимо прочитать
*/
void        S29GL256_read(uint8_t* const buffer, const uint32_t addr, const uint32_t size);

/*!
    \brief              Функция записи массива байт
    \param data[in]     Указатель на массив данных, который необходимо записать
    \param addr[in]     Адрес начала диапазона памяти для записи
    \param size[in]     Количество байт, которые необходимо записать
    \return             Статус операции
*/
S29GL256_status_t   S29GL256_write(const uint8_t* const data, const uint32_t addr, const uint32_t size, const uint8_t bypass_mode);

/*!
    \brief              Функция полного стирания памяти
    \return             Статус операции
*/
S29GL256_status_t   S29GL256_chip_erase();

/*!
    \brief              Функция стирания выбранного сектора памяти
    \param sector[in]   Адрес сектора для стирания
    \return             Статус операции
*/
S29GL256_status_t   S29GL256_sector_erase(const S29GL256_sector_t sector);

/*!
    \brief              Функция стирания выбранной страницы памяти
    \param page[in]     Адрес страницы для стирания
    \return             Статус операции
*/
S29GL256_status_t   S29GL256_page_erase(const uint32_t page);

/*!
    \brief              Функция инициации сброса памяти
*/
void                    S29GL256_reset();

#endif
