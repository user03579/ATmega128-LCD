#include "spi_lcd.h"
#include "spi_lcd_font.h"	// 8x16 ASCII 문자 폰트

// ATmega128 SPI 초기 설정
void spi_init(void)
{
	// 포트 PB3 - 0 관련 초기화
	DDRB |= 0x07;	// 0000_0111, PB3(MISO) 입력, PB2 - 0 출력
	PORTB |= 0x08;	// 0000_1000, PB3(MISO) Pull-up 저항 연결
	SFIOR &= ~(1 << PUD); // Pull-up 저항 연결
	// No SPI interrupt, SPI enable, MSB first, Master mode
	// Rising edge lead, Sample at Leading edge, SCK=fosc/16
	SPCR = 0x51;	// 0101_0001
}

// ATmega128 SPI 데이터 송수신
int spi_tx_rx(byte bTxData, byte *pbRxData)
{
	int i = 0;

	SPDR = bTxData;	// Data 송신
	while(!(SPSR & 0x80)) if(++i > 100000) return FALSE;	// SPIF, 송신 완료 확인
	*pbRxData = SPDR;	// 수신된 데이터
	return TRUE;
}

// ATmega128 SPI 데이터 송신
int spi_tx(byte bTxData)
{
	int i = 0;

	SPDR = bTxData;				// Data 송신
	while(!(SPSR & 0x80)) if(++i > 100000) return FALSE;	// SPIF, 송신 완료 확인
	return TRUE;
}

//////////////////////////////////////////////
// ATmega128에 연결된 TFT LCD(ST7735) 모듈 관련 함수들
// LCD 모듈의 신호들이 아래와 같이 ATmega128 키트에 연결된 것으로 가정
// LCD 모듈	-->	ATmega128 키트
// Reset 	-->	포트 PD7
// A0(D/CX)	-->	포트 PD6
// LED 		-->	3.3V
// SPI		-->	SPI

// ATmega128 SPI를 통해서 LCD(ST7735)에 Command 전송
int spi_st7735_wr_cmd(byte bCmd)
{
	int iReturn;

	PORTD &= 0xA0;	// 1010_0000, PD6(A0(D/CX)) --> L
	iReturn = spi_tx(bCmd);
	PORTD |= 0x40;	// 0100_0000, PD6(A0(D/CX)) --> H
	return iReturn;
}

// ATmega128 SPI를 통해서 LCD(ST7735)에 Data 전송
int spi_st7735_wr_data(byte bData)
{
	int iReturn;

	PORTD |= 0x40;	// 0100_0000, PD6(A0(D/CX)) --> H
	iReturn = spi_tx(bData);
	return iReturn;
}

// LCD(ST7735) 모듈 관련 초기 설정
void spi_st7735_init(void)
{
	// 포트 PD7, PD6 관련 초기화 
	DDRD |= 0xC0;	// 1100_0000, PD7, PD6 출력
	PORTD |= 0xC0;	// 1100_0000, PD7(Reset) --> H, PD6(A0(D/CX)) --> H

	// LCD 모듈 리셋
	_delay_ms(50);
	PORTD &= ~0x80;// 1000_0000, PD7(Reset) --> L
	_delay_ms(50);
	PORTD |= 0x80;	// 1000_0000, PD7(Reset) --> H
	_delay_ms(50);

	spi_st7735_wr_cmd(0x01);	// ST7735 Reset
	_delay_ms(50);
	spi_st7735_wr_cmd(0x11);	// Sleep out
	_delay_ms(50);
	spi_st7735_wr_cmd(0x26);	// Gamma curve select
	spi_st7735_wr_data(0x04);	// Gamma curve 4
	spi_st7735_wr_cmd(0xB1);	// Frame rate (in normal mode, full colors)
	spi_st7735_wr_data(0x0B);
	spi_st7735_wr_data(0x14);
	spi_st7735_wr_cmd(0xC0);	// Power control 1
	spi_st7735_wr_data(0x08);	// 4.4V
	spi_st7735_wr_data(0x00);	// 2.5uA
	spi_st7735_wr_cmd(0xC1);	// Power control 2
	spi_st7735_wr_data(0x05);	// VGH: 6X(14.7), VGL: -3X(-7.35)
	spi_st7735_wr_cmd(0xC5);	// VCOM control 1
	spi_st7735_wr_data(0x42);	// VCOMH = 4.15
	spi_st7735_wr_data(0x30);	// VML = -1.30
	spi_st7735_wr_cmd(0xC7);	// VCOM offset
	spi_st7735_wr_data(0xC1);
	spi_st7735_wr_cmd(0x3A);	// Interface pixel format
	spi_st7735_wr_data(0x05);	// 16-bit/pixel
	spi_st7735_wr_cmd(0x36);	// Memory data access control
	spi_st7735_wr_data(0xA0);	// MY = 1, MX = 0, MV = 1, ML = 0, RGB = 0, MH = 0
	spi_st7735_wr_cmd(0x2A);	// Column address
	spi_st7735_wr_data(0x00);	// XS[15:8]
	spi_st7735_wr_data(0x00);	// XS[7:0]
	spi_st7735_wr_data(0x00);	// XE[15:8]
	spi_st7735_wr_data(0x9F);	// XE[7:0]
	spi_st7735_wr_cmd(0x2B);	// Row address
	spi_st7735_wr_data(0x00);	// YS[15:8]
	spi_st7735_wr_data(0x00);	// YS[7:0]
	spi_st7735_wr_data(0x00);	// YE[15:8]
	spi_st7735_wr_data(0x7F);	// YE[7:0]
	spi_st7735_wr_cmd(0xE0);	// Gamma adjustment (+)
	spi_st7735_wr_data(0x28);
	spi_st7735_wr_data(0x24);
	spi_st7735_wr_data(0x22);
	spi_st7735_wr_data(0x31);
	spi_st7735_wr_data(0x2B);
	spi_st7735_wr_data(0x0E);
	spi_st7735_wr_data(0x53);
	spi_st7735_wr_data(0xA5);
	spi_st7735_wr_data(0x42);
	spi_st7735_wr_data(0x16);
	spi_st7735_wr_data(0x18);
	spi_st7735_wr_data(0x12);
	spi_st7735_wr_data(0x1A);
	spi_st7735_wr_data(0x14);
	spi_st7735_wr_data(0x03);
	spi_st7735_wr_cmd(0xE1);	// Gamma adjustment (-)
	spi_st7735_wr_data(0x17);
	spi_st7735_wr_data(0x1B);
	spi_st7735_wr_data(0x1D);
	spi_st7735_wr_data(0x0E);
	spi_st7735_wr_data(0x14);
	spi_st7735_wr_data(0x11);
	spi_st7735_wr_data(0x2C);
	spi_st7735_wr_data(0xA5);
	spi_st7735_wr_data(0x3D);
	spi_st7735_wr_data(0x09);
	spi_st7735_wr_data(0x27);
	spi_st7735_wr_data(0x2D);
	spi_st7735_wr_data(0x25);
	spi_st7735_wr_data(0x2B);
	spi_st7735_wr_data(0x3C);
	spi_st7735_wr_cmd(0x13);	// Partial off (normal)
	spi_st7735_wr_cmd(0x29);	// Display on
}

// LCD에 출력할 영역 지정
void spi_st7735_set_area(int iX1, int iX2, int iY1, int iY2)
{
	const byte bX1_c = iX1;  // save only the least 8 bits
	const byte bX2_c = iX2;
	const byte bY1_c = iY1;  // save only the least 8 bits
	const byte bY2_c = iY2;

	spi_st7735_wr_cmd(0x2A);	// Column address
	spi_st7735_wr_data(0x00);	// XS[15:8]
	spi_st7735_wr_data(bX1_c);	// XS[7:0]
	spi_st7735_wr_data(0x00);	// XE[15:8]
	spi_st7735_wr_data(bX2_c);	// XE[7:0]
	spi_st7735_wr_cmd(0x2B);	// Row address
	spi_st7735_wr_data(0x00);	// YS[15:8]
	spi_st7735_wr_data(bY1_c);	// YS[7:0]
	spi_st7735_wr_data(0x00);	// YE[15:8]
	spi_st7735_wr_data(bY2_c);	// YE[7:0]
}

// LCD의 배경색 설정
void spi_st7735_set_bg(int iBgRGB)
{
	const byte bBgRGB_c0 = iBgRGB;
	const byte bBgRGB_c1 = iBgRGB >> 8;

	spi_st7735_set_area(0, 159, 0, 127);
	spi_st7735_wr_cmd(0x2C);	// memory write

	for(int i = 0; i < (160 * 128); i++) {
		spi_st7735_wr_data(bBgRGB_c1);
		spi_st7735_wr_data(bBgRGB_c0);
	}
}

// LCD에 문자 출력
void spi_st7735_putchar(int iX, int iY, int iBgRGB, int iFgRGB, char cCh)
{
	const byte bBgRGB_c0 = iBgRGB;
	const byte bBgRGB_c1 = iBgRGB >> 8;
	const byte bFgRGB_c0 = iFgRGB;
	const byte bFgRGB_c1 = iFgRGB >> 8;
	const int iSizeX = 8;
	const int iSizeY = 16;

	spi_st7735_set_area(iX, iX + iSizeX - 1, iY, iY + iSizeY - 1);  // set output area: 8 x 16
	spi_st7735_wr_cmd(0x2C);	// memory write

	for(int j = 0; j < iSizeY; j++) {// coordinate in y
		for(int i = 0; i < iSizeX; i++) { // coordinate in x
			if(j < 8) { // upper half
				if(*(*(caLcdFont + cCh) + i) & (1 << j)) { // foreground color
					spi_st7735_wr_data(bFgRGB_c1);
					spi_st7735_wr_data(bFgRGB_c0);
				} else { // background color
					spi_st7735_wr_data(bBgRGB_c1);
					spi_st7735_wr_data(bBgRGB_c0);
				}
			} else { // lower half
				if(*(*(caLcdFont + cCh) + i + 8) & (1 << (j - 8))) { // foreground color
					spi_st7735_wr_data(bFgRGB_c1);
					spi_st7735_wr_data(bFgRGB_c0);
				} else { // background color
					spi_st7735_wr_data(bBgRGB_c1);
					spi_st7735_wr_data(bBgRGB_c0);
				}
			}
		}
	}
}

// LCD(ST7735)에 문자열 출력
// (iX, iY): 첫번째 문자가 표시될 위치
// iBgRGB: 배경색
// iFgRGB: 전경색
// pcStr: 표시될 문자열을 가리키는 포인터
void spi_st7735_putstr(int iX, int iY, int iBgRGB, int iFgRGB, char* pcStr)
{
	int i = 0;

	while(*pcStr != NULL) {
		spi_st7735_putchar(iX + 8 * i, iY, iBgRGB, iFgRGB, *pcStr);
		pcStr++; i++;
	}
}
 