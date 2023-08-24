
#include <stdio.h>
#include "hbird_sdk_hal.h"
#include <sdcard.h>

void spi_emptyClock8(SPI_TypeDef *spi)
{
    spi_set_datalen(spi, 8);
    spi_start_transaction(spi, SPI_CMD_RD, SPI_CSN1);
    while ((spi_get_status(spi) & 0xFFFF) != 1)
    {
    };
}

void gpio_config()
{
    gpio_iof_config(GPIOA, IOF_SPI_MASK);
}

void _test()
{
    // func for debug
}

int sd_init(SPI_TypeDef *spi)
{
    uint8_t res[5];
    int max_try = 24;

    // 切换到 SPI 模式
    do
    {
        if (max_try-- <= 1)
        {
            return SD_ERROR;
        }
        res[0] = sd_goIdleState(spi);
    } while (res[0] != SD_IN_IDLE_STATE);

    // 发送电气参数
    sd_sendIfCond(spi, res);

    if (res[0] != SD_IN_IDLE_STATE)
    {
        return SD_ERROR;
    }
    if (res[4] != 0xAA)
    {
        return SD_ERROR;
    }

    max_try = 24;
    do
    {
        if (max_try-- <= 1)
        {
            return SD_ERROR;
        }
        res[0] = sd_sendApp(spi);
        if (res[0] < 2)
        {
            res[0] = sd_sendOpCond(spi);
        }
        delay_1ms(10);
    } while (res[0] != SD_READY);

    // 查询 OCR(operation conditions register) 内容
    sd_readOCR(SPI1, res);

    if (!(res[1] & 0x80))
    {
        return SD_ERROR;
    }

    return SD_SUCCESS;
}

void sd_powerUpSeq(SPI_TypeDef *spi)
{
    // 不要用
    delay_1ms(1);
    for (uint8_t i = 0; i < 10; i++)
    {
        spi_emptyClock8(spi);
    }
    spi_send8(spi, 0xFF);
}

uint8_t sd_goIdleState(SPI_TypeDef *spi)
{
    spi_send8(spi, 0xFF);
    sd_sendCmd(spi, CMD0, 0x00000000, CMD0_CRC);
    uint8_t res = sd_readRes1(spi);
    return res;
}

void sd_sendCmd(SPI_TypeDef *spi, uint8_t cmd, uint32_t arg, uint8_t crc)
{
    // transmit command to sd card
    spi_send8(spi, cmd | 0x40);
    // transmit argument
    spi_send32(spi, arg);
    // transmit crc
    spi_send8(spi, crc | 0x01);
}

void spi_send8(SPI_TypeDef *spi, uint8_t data)
{
    uint32_t wdata = ((uint32_t)data) << 24;
    spi_set_datalen(spi, 8);
    spi_write_fifo(spi, &wdata, 8);
    spi_start_transaction(spi, SPI_CMD_WR, SPI_CSN0);
    while ((spi_get_status(spi) & 0xFFFF) != 1)
    {
    };
}

void spi_send16(SPI_TypeDef *spi, uint16_t data)
{
    uint32_t wdata = ((uint32_t)data) << 16;
    spi_set_datalen(spi, 16);
    spi_write_fifo(spi, &wdata, 16);
    spi_start_transaction(spi, SPI_CMD_WR, SPI_CSN0);
    while ((spi_get_status(spi) & 0xFFFF) != 1)
    {
    };
}

void spi_send32(SPI_TypeDef *spi, uint32_t data)
{
    spi_set_datalen(spi, 32);
    spi_write_fifo(spi, &data, 32);
    spi_start_transaction(spi, SPI_CMD_WR, SPI_CSN0);
    while ((spi_get_status(spi) & 0xFFFF) != 1)
    {
    };
}

uint8_t spi_receive8(SPI_TypeDef *spi)
{
    // TODO: MOSI = 0b1
    spi_set_datalen(spi, 8);
    spi_start_transaction(spi, SPI_CMD_RD, SPI_CSN0);
    while ((spi_get_status(spi) & 0xFFFF) != 1)
    {
    };
    uint32_t fifo = 0;
    spi_read_fifo(spi, &fifo, 8);
    return fifo & 0xFF;
}

uint8_t sd_readRes1(SPI_TypeDef *spi)
{
    spi_send8(spi, 0xFF);

    uint8_t i = 0, res;
    do
    {
        if (i++ > 8)
        {
            break;
        }
        res = spi_receive8(spi);
    } while (res == 0xFF);

    return res;
}

uint8_t sd_readRes1b(SPI_TypeDef *spi)
{
    spi_send8(spi, 0xFF);

    uint16_t i = 0, res;
    do
    {
        if (i++ > 100)
        {
            break;
        }
        res = spi_receive8(spi);
    } while (res == 0x00); // mean busy

    return res;
}

void sd_readRes3_7(SPI_TypeDef *spi, uint8_t *res)
{
    res[0] = sd_readRes1(spi);
    if (res[0] > 1)
    {
        return;
    }

    for (int i = 1; i < 5; i++)
    {
        res[i] = spi_receive8(spi);
    }
}

void sd_sendIfCond(SPI_TypeDef *spi, uint8_t *res)
{
    spi_send8(spi, 0xFF);
    // send CMD8
    sd_sendCmd(spi, CMD8, CMD8_ARG, CMD8_CRC);

    // read response
    sd_readRes3_7(spi, res);
    // deassert chip select
    spi_send8(spi, 0xFF);
}

void sd_readOCR(SPI_TypeDef *spi, uint8_t *res)
{
    spi_send8(spi, 0xFF);

    // send CMD58
    sd_sendCmd(spi, CMD58, CMD58_ARG, CMD58_CRC);
    // read response
    sd_readRes3_7(spi, res);

    spi_send8(spi, 0xFF);
}

void sd_printR1(uint8_t res)
{
    if (res & 0b10000000)
    {
        printf("\tError: MSB = 1\r\n");
        return;
    }
    if (res == 0)
    {
        printf("\tCard Ready\r\n");
        return;
    }
    if (PARAM_ERROR(res))
        printf("\tParameter Error\r\n");
    if (ADDR_ERROR(res))
        printf("\tAddress Error\r\n");
    if (ERASE_SEQ_ERROR(res))
        printf("\tErase Sequence Error\r\n");
    if (CRC_ERROR(res))
        printf("\tCRC Error\r\n");
    if (ILLEGAL_CMD(res))
        printf("\tIllegal Command\r\n");
    if (ERASE_RESET(res))
        printf("\tErase Reset Error\r\n");
    if (IN_IDLE(res))
        printf("\tIn Idle State\r\n");
}

void sd_printR1b(uint8_t res)
{
    if (res == 0)
    {
        printf("Warning: card is busy\r\n");
    }
    else
    {
        printf("Ready for the next command\r\n");
    }
}

void sd_printR7(uint8_t *res)
{
    sd_printR1(res[0]);

    if (res[0] > 1)
        return;

    printf("\tCommand Version: %X \r\n", CMD_VER(res[1]));

    printf("\tVoltage Accepted: ");
    if (VOL_ACC(res[3]) == VOLTAGE_ACC_27_33)
        printf("2.7-3.6V\r\n");
    else if (VOL_ACC(res[3]) == VOLTAGE_ACC_LOW)
        printf("LOW VOLTAGE\r\n");
    else if (VOL_ACC(res[3]) == VOLTAGE_ACC_RES1)
        printf("RESERVED\r\n");
    else if (VOL_ACC(res[3]) == VOLTAGE_ACC_RES2)
        printf("RESERVED\r\n");
    else
        printf("NOT DEFINED\r\n");

    printf("\tEcho: %X", res[4]);
    printf("\r\n");
}

void sd_printR3(uint8_t *res)
{
    sd_printR1(res[0]);

    if (res[0] > 1)
        return;

    printf("\tCard Power Up Status: ");
    if (POWER_UP_STATUS(res[1]))
    {
        printf("READY\r\n");
        printf("\tCCS Status: ");
        if (CCS_VAL(res[1]))
        {
            printf("1\r\n");
        }
        else
            printf("0\r\n");
    }
    else
    {
        printf("BUSY\r\n");
    }

    printf("\tVDD Window: ");
    if (VDD_2728(res[3]))
        printf("2.7-2.8, ");
    if (VDD_2829(res[2]))
        printf("2.8-2.9, ");
    if (VDD_2930(res[2]))
        printf("2.9-3.0, ");
    if (VDD_3031(res[2]))
        printf("3.0-3.1, ");
    if (VDD_3132(res[2]))
        printf("3.1-3.2, ");
    if (VDD_3233(res[2]))
        printf("3.2-3.3, ");
    if (VDD_3334(res[2]))
        printf("3.3-3.4, ");
    if (VDD_3435(res[2]))
        printf("3.4-3.5, ");
    if (VDD_3536(res[2]))
        printf("3.5-3.6");
    printf("\r\n");
}

uint8_t sd_sendApp(SPI_TypeDef *spi)
{
    spi_send8(spi, 0xFF);

    sd_sendCmd(spi, CMD55, CMD55_ARG, CMD55_CRC);
    uint8_t res = sd_readRes1(spi);

    spi_send8(spi, 0xFF);
    return res;
}

uint8_t sd_sendOpCond(SPI_TypeDef *spi)
{
    spi_send8(spi, 0xFF);

    sd_sendCmd(spi, ACMD41, ACMD41_ARG, ACMD41_CRC);
    uint8_t res = sd_readRes1(spi);

    spi_send8(spi, 0xFF);
    return res;
}

/*******************************************************************************
 Read single 512 byte block
 token = 0xFE - Successful read
 token = 0x0X - Data error
 token = 0xFF - Timeout
*******************************************************************************/
uint8_t sd_readSingleBlock(SPI_TypeDef *spi, uint32_t addr, uint8_t *buf, uint8_t *token)
{
    uint8_t res1, read;
    uint16_t readAttempts;

    // set token to none
    *token = 0xFF;

    spi_send8(spi, 0xFF);

    // send CMD17
    sd_sendCmd(spi, CMD17, addr, CMD17_CRC);

    // read R1
    res1 = sd_readRes1(spi);

    // if response received from card
    if (res1 != 0xFF)
    {
        // wait for a response token (timeout = 100ms)
        readAttempts = 0;
        while (++readAttempts != SD_MAX_READ_ATTEMPTS)
        {
            if ((read = spi_receive8(spi)) != 0xFF)
            {
                break;
            }
        }

        // set token to card response
        *token = read;

        // if response token is 0xFE
        if (read == 0xFE)
        {
            // read 512 byte block
            for (uint16_t i = 0; i < 512; i++)
                *buf++ = spi_receive8(spi);

            // read 16-bit CRC
            spi_receive8(spi);
            spi_receive8(spi);
        }
    }

    spi_send8(spi, 0xFF);

    return res1;
}

void sd_printDataErrToken(uint8_t token)
{
    if (SD_TOKEN_OOR(token))
        printf("\tData out of range\r\n");
    if (SD_TOKEN_CECC(token))
        printf("\tCard ECC failed\r\n");
    if (SD_TOKEN_CC(token))
        printf("\tCC Error\r\n");
    if (SD_TOKEN_ERROR(token))
        printf("\tError\r\n");
}

/*******************************************************************************
 Write single 512 byte block
 token = 0x00 - busy timeout
 token = 0x05 - data accepted
 token = 0xFF - response timeout
*******************************************************************************/
uint8_t sd_writeSingleBlock(SPI_TypeDef *spi, uint32_t addr, uint8_t *buf, uint8_t *token)
{
    uint16_t writeAttempts;
    uint8_t read, res;

    // set token to none
    *token = 0xFF;

    spi_send8(spi, 0xFF);

    // send CMD24
    sd_sendCmd(spi, CMD24, addr, CMD24_CRC);

    // read response
    res = sd_readRes1(spi);

    // if no error
    if (res == SD_READY)
    {
        // send start token
        spi_send8(spi, SD_START_TOKEN);

        // write buffer to card
        for (uint16_t i = 0; i < SD_BLOCK_LEN; i++)
            spi_send8(spi, buf[i]);

        // wait for a response (timeout = 250ms)
        writeAttempts = 0;
        while (++writeAttempts != SD_MAX_WRITE_ATTEMPTS)
        {
            if ((read = spi_receive8(spi)) != 0xFF)
            {
                *token = 0xFF;
                break;
            }
        }

        // if data accepted
        if ((read & 0x1F) == 0x05)
        {
            // set token to data accepted
            *token = 0x05;

            // wait for write to finish (timeout = 250ms)
            writeAttempts = 0;
            while (spi_receive8(spi) == 0x00)
            {
                if (++writeAttempts == SD_MAX_WRITE_ATTEMPTS)
                {
                    *token = 0x00;
                    break;
                }
            }
        }
    }

    spi_send8(spi, 0xFF);

    return res;
}

uint8_t sd_readMultiBlock(SPI_TypeDef *spi, uint32_t addr, uint8_t count, uint8_t *buf, uint8_t *token)
{
    uint16_t readAttempts;
    uint8_t read, res1, index = 0;
    *token = 0xFF;
    spi_send8(spi, 0xFF);

    // send CMD18
    sd_sendCmd(spi, CMD18, addr, CMD18_CRC);

    // read R1
    res1 = sd_readRes1(spi);

    // if response received from card
    if (res1 == 0x00)
    {
        while (index++ < count)
        {
            // wait for a response token (timeout = 100ms)
            readAttempts = 0;
            while (++readAttempts != SD_MAX_READ_ATTEMPTS)
            {
                if ((read = spi_receive8(spi)) != 0xFF)
                {
                    break;
                }
            }

            // set token to card response
            *token = read;

            // if response token is 0xFE
            if (read == 0xFE)
            {
                // read 512 byte block
                for (uint16_t i = 0; i < 512; i++)
                    *buf++ = spi_receive8(spi);

                // read 16-bit CRC
                spi_receive8(spi);
                spi_receive8(spi);
            }
            else
            {
                break; // error msg in token
            }
        }
        sd_stopTransmission(spi);
    }
    else
    {
        // do nothing, error msg in res1
    }

    spi_send8(spi, 0xFF);

    return res1;
}

uint8_t sd_writeMultiBlock(SPI_TypeDef *spi, uint32_t addr, uint8_t count, uint8_t *buf, uint8_t *token)
{
    uint16_t writeAttempts;
    uint8_t read, res;

    // set token to none
    *token = 0xFF;

    spi_send8(spi, 0xFF);

    // send CMD25
    sd_sendCmd(spi, CMD25, addr, CMD25_CRC);

    // read response
    res = sd_readRes1(spi);

    // if no error
    if (res == SD_READY)
    {

        for (uint8_t index = 0; index < count; index++)
        {
            // send start token
            spi_send8(spi, SD_START_TOKEN_FOR_MULTI_WR);

            // write buffer to card
            for (uint16_t i = 0; i < SD_BLOCK_LEN; i++)
            {
                if (SD_BLOCK_LEN - i < 4)
                {
                    spi_send8(spi, *buf);
                    buf++;
                }
                else
                {
                    uint32_t data = ntohl(*((uint32_t *)buf));
                    spi_send32(spi, data);
                    buf += 4;
                    i += 3;
                }
            }

            // wait for a response (timeout = 250ms)
            writeAttempts = 0;
            while (++writeAttempts != SD_MAX_WRITE_ATTEMPTS)
            {
                read = spi_receive8(spi);
                if (read != 0xFF)
                {
                    *token = 0xFF;
                    break;
                }
            }

            // if data accepted
            if ((read & 0x1F) == 0x05)
            {
                // set token to data accepted
                *token = 0x05;

                // wait for write to finish (timeout = 250ms)
                writeAttempts = 0;
                while (spi_receive8(spi) == 0x00)
                {
                    if (++writeAttempts == SD_MAX_WRITE_ATTEMPTS)
                    {
                        *token = 0x00;
                        break;
                    }
                }
            }
            else
            {
                break; // error msg in read
            }
        }

        // send stop token
        spi_send8(spi, SD_STOP_TRAN_TOKEN);
        // wait for write to finish (timeout = 250ms)
        writeAttempts = 0;
        while (spi_receive8(spi) != 0xFF)
        {
            if (++writeAttempts == SD_MAX_WRITE_ATTEMPTS)
            {
                *token = 0x00;
                break;
            }
        }
    }

    spi_send8(spi, 0xFF);

    return res;
}

void sd_stopTransmission(SPI_TypeDef *spi)
{
    uint8_t res1b;
    sd_sendCmd(spi, CMD12, 0x00000000, CMD12_CRC);
    res1b = sd_readRes1b(spi);
    spi_send8(spi, 0xFF);
}