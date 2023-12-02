#ifndef SPI_LCD_H_
#define SPI_LCD_H_

#include "main.h"

// 16 비트 RGB 코드
#define RGB_BLACK		0x0000
#define RGB_WHITE		0xFFFF
#define RGB_RED		0x001F
#define RGB_GREEN		0x07E0
#define RGB_BLUE		0xF800
#define RGB_YELLOW	0x07FF

void spi_init(void);
int spi_tx_rx(byte bTxData, byte *pbRxData);
int spi_tx(byte bTxData);
int spi_st7735_wr_cmd(byte bCmd);
int spi_st7735_wr_data(byte bData);
void spi_st7735_init(void);
void spi_st7735_set_area(int iX1, int iX2, int iY1, int iY2);
void spi_st7735_set_bg(int iBgRGB);
void spi_st7735_putchar(int iX, int iY, int iBgRGB, int iFgRGB, char cCh);
void spi_st7735_putstr(int iX, int iY, int iBgRGB, int iFgRGB, char* pcStr);

#endif /* SPI_LCD_H_ */ 