#include "main.h"
#include "spi_lcd.h"
#define DISP_LEN	14 //표시 가능한 최대 문자 갯수를 14개로 설정
void main(void)
{
	char caDisp[] = "Kunsan National University";
	int pos, startPos, cnt, strLen = strlen(caDisp);
	spi_init();			// ATmega128 SPI 초기 설정
	spi_st7735_init();// LCD(ST7735) 모듈 관련 초기 설정
	spi_st7735_set_bg(RGB_BLACK);
	
	while(1){
		cnt = 0; pos = startPos;
		while (cnt++ < DISP_LEN){
			if(pos <= strLen){
				spi_st7735_putstr(3, 8*cnt,1, RGB_BLACK, RGB_WHITE, caDisp[pos]);
			}
			else {
				spi_st7735_putstr(3, 8*cnt,1, RGB_BLACK, RGB_WHITE, " ");
			}
			pos++;
		}
		if(++startPos > strLen){
			startPos = 0;
		}
		_delay_ms(30);
	}
}
