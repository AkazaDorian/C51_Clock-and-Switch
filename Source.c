#include "Header.h"

void delay(unsigned int i)
{
  char j;
  for(i; i > 0; i--)
    for(j = 200; j > 0; j--);
}
int main()
{
	TMOD = 0x01;//设定工作方式为16位定时器
	TH0 = 0x70;
	TL0 = 0x00;
	EA = 1;//开总中断
	ET0 = 1;//开定时器T0中断  
	TR0 = 1;//启动定时器T0
	LcdInit();
	for (CommonCounter = 0; CommonCounter < 3; ++CommonCounter)
	{
		Time[CommonCounter][2] = 0;
		Time[CommonCounter][5] = 0;
	}
	strcpy(DisplayData[0], "Time:   00:00:00");
	strcpy(DisplayData[1], "Status:      OFF");
//	strcpy(DisplayData[1], "Status:       ON");
//	strcpy(DisplayData[1], "A: 00:00 ~ 00:00");
	LcdInit();
	for (CommonCounter = 0; CommonCounter < 16; ++CommonCounter)
		LcdWriteData(DisplayData[0][CommonCounter]);
	LcdWriteCom(0xC0);
	for (CommonCounter = 0; CommonCounter < 16; ++CommonCounter)
		LcdWriteData(DisplayData[1][CommonCounter]);
	while (1)
		if(State)
		{
			Power = 1;
			delay(5);
			Power = 0;
			delay(5);
		}
}