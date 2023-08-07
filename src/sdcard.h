#include <hbirdv2.h>
#include <hbirdv2_spi.h>

#define CMD0 0x40
#define CMD0_CRC 0x95
#define CMD8 0x48
#define CMD8_ARG 0x000001AA
#define CMD8_CRC 0x86
#define CMD58 58
#define CMD58_ARG 0x00000000
#define CMD58_CRC 0x00
#define CMD55 55
#define CMD55_ARG 0x00000000
#define CMD55_CRC 0x00
#define ACMD41 41
#define ACMD41_ARG 0x40000000
#define ACMD41_CRC 0x00
#define CMD12 12
#define CMD12_CRC 0x00
#define CMD17 0x51
#define CMD17_CRC 0x00
#define CMD18 0x52
#define CMD18_CRC 0x00
#define CMD24 24
#define CMD24_CRC 0x00

#define PARAM_ERROR(X) X & 0b01000000
#define ADDR_ERROR(X) X & 0b00100000
#define ERASE_SEQ_ERROR(X) X & 0b00010000
#define CRC_ERROR(X) X & 0b00001000
#define ILLEGAL_CMD(X) X & 0b00000100
#define ERASE_RESET(X) X & 0b00000010
#define IN_IDLE(X) X & 0b00000001

#define CMD_VER(X) ((X >> 4) & 0xF0)
#define VOL_ACC(X) (X & 0x1F)

#define VOLTAGE_ACC_27_33 0b00000001
#define VOLTAGE_ACC_LOW 0b00000010
#define VOLTAGE_ACC_RES1 0b00000100
#define VOLTAGE_ACC_RES2 0b00001000

#define POWER_UP_STATUS(X) X & 0x40
#define CCS_VAL(X) X & 0x40
#define VDD_2728(X) X & 0b10000000
#define VDD_2829(X) X & 0b00000001
#define VDD_2930(X) X & 0b00000010
#define VDD_3031(X) X & 0b00000100
#define VDD_3132(X) X & 0b00001000
#define VDD_3233(X) X & 0b00010000
#define VDD_3334(X) X & 0b00100000
#define VDD_3435(X) X & 0b01000000
#define VDD_3536(X) X & 0b10000000

#define SD_SUCCESS 0
#define SD_ERROR 1
#define SD_IN_IDLE_STATE 0x01
#define SD_READY 0x00
#define SD_R1_NO_ERROR(X) X < 0x02

/**
 * read timeout = 100ms
 * write timeout = 250ms
 * (0.1s * 16 MHz)/(10 * 8 bytes) = 20000
 */
#define SD_MAX_READ_ATTEMPTS 20000
#define SD_MAX_WRITE_ATTEMPTS 50000

#define SD_TOKEN_OOR(X) X & 0b00001000
#define SD_TOKEN_CECC(X) X & 0b00000100
#define SD_TOKEN_CC(X) X & 0b00000010
#define SD_TOKEN_ERROR(X) X & 0b00000001

#define SD_START_TOKEN 0xFE
#define SD_ERROR_TOKEN 0x00

#define SD_DATA_ACCEPTED 0x05
#define SD_DATA_REJECTED_CRC 0x0B
#define SD_DATA_REJECTED_WRITE 0x0D

#define SD_BLOCK_LEN 512

void gpio_config();
void sd_powerUpSeq(SPI_TypeDef *spi);
int sd_init(SPI_TypeDef *spi);
void spi_emptyClock8(SPI_TypeDef *spi);
void spi_send8(SPI_TypeDef *spi, uint8_t data);
void spi_send16(SPI_TypeDef *spi, uint16_t data);
void spi_send32(SPI_TypeDef *spi, uint32_t data);
uint8_t spi_receive8(SPI_TypeDef *spi);
void sd_sendCmd(SPI_TypeDef *spi, uint8_t cmd, uint32_t arg, uint8_t crc);
uint8_t sd_readRes1(SPI_TypeDef *spi);
uint8_t sd_readRes1b(SPI_TypeDef *spi);
void sd_readRes3_7(SPI_TypeDef *spi, uint8_t *res);
void sd_sendIfCond(SPI_TypeDef *spi, uint8_t *res);
void sd_printR1(uint8_t res);
void sd_printR3(uint8_t *res);
void sd_printR7(uint8_t *res);
void sd_readOCR(SPI_TypeDef *spi, uint8_t *res);
uint8_t sd_sendApp(SPI_TypeDef *spi);
uint8_t sd_sendOpCond(SPI_TypeDef *spi);
uint8_t sd_goIdleState(SPI_TypeDef *spi);
uint8_t sd_readSingleBlock(SPI_TypeDef *spi, uint32_t addr, uint8_t *buf, uint8_t *token);
void sd_printDataErrToken(uint8_t token);
uint8_t sd_writeSingleBlock(SPI_TypeDef *spi, uint32_t addr, uint8_t *buf, uint8_t *token);
uint8_t sd_readMultiBlock(SPI_TypeDef *spi, uint32_t addr, uint8_t count, uint8_t *buf, uint8_t *token);
uint8_t sd_writeMultiBlock(SPI_TypeDef *spi, uint32_t addr, uint8_t count, uint8_t *buf, uint8_t *token);
void sd_stopTransmission(SPI_TypeDef *spi);
void _test();
