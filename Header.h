//定时开关及显示
//浙江工业大学之江学院 信息工程学院 徐超凡
//

#include <reg51.h>
#include <intrins.h>
#include <string.h>

#define uchar unsigned char
#define uint unsigned int
#define GPIO_DIG P2

sbit MoveAim = P1^1;
sbit NumUp = P1^2;
sbit ConditionChange = P1^3;
sbit SetEdit = P1^7;
sbit NumDown = P1^5;
sbit Check = P1^6;
sbit TimeEdit = P1^4;
sbit Power = P1^0;

//0 的 ASCII 码值为 48

uchar Time[3][6] = { 0 };//用来存放要显示的时间
char DisplayData[2][17];
uchar Hour = 0;
uchar Minute = 0;
uchar Second = 0;
uchar Aim = 0;
uchar Aiming[4] = {14,11,6,3};
uchar Timing[4] = { 4,3,1,0 };
uchar Aiming2[3] = { 14,11,8 };
uchar Condition = 0;//正在编辑的 Conditon 的序号
uint MS40 = 0;
uint BreakTime = 0;
uchar State = 0;//0 为未开启，1、2、3 分别对应同时开启的 Condition 的数量
uchar Editing = 0;//2 时为预设值设定，1 时为时间设定
uchar RecDisplay = 0;
uchar Combo = 0;
uchar CommonCounter;
uchar BlinkTime = 0;
uchar Blank = 0;
uchar Blanking[2];
uchar AimChanged = 0;
uchar on = 1;
uchar GotKey = 1;

//LCD Display
#define LCD1602_DATAPINS P0
sbit LCD1602_E = P2 ^ 7;
sbit LCD1602_RW = P2 ^ 5;
sbit LCD1602_RS = P2 ^ 6;
void Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;
	_nop_();
	i = 2;
	j = 199;
	do
	{
		while (--j);
	} while (--i);
}
void LcdWriteCom(uchar com)	  //写入命令
{
	LCD1602_E = 0;     //使能
	LCD1602_RS = 0;	   //选择发送命令
	LCD1602_RW = 0;	   //选择写入

	LCD1602_DATAPINS = com;     //放入命令
	Delay1ms();		//等待数据稳定

	LCD1602_E = 1;	          //写入时序
	_nop_();
	_nop_();
	_nop_();
	_nop_();//保持时间
	LCD1602_E = 0;
}
void LcdWriteData(uchar dat)			//写入数据
{
	LCD1602_E = 0;	//使能清零
	LCD1602_RS = 1;	//选择输入数据
	LCD1602_RW = 0;	//选择写入

	LCD1602_DATAPINS = dat; //写入数据
	Delay1ms();

	LCD1602_E = 1;   //写入时序
	_nop_();
	_nop_();
	_nop_();
	_nop_();//保持时间
	LCD1602_E = 0;
}
void LcdInit()						  //LCD初始化子程序
{
	LcdWriteCom(0x38);  //开显示
	LcdWriteCom(0x0c);  //开显示不显示光标
	LcdWriteCom(0x06);  //写一个指针加1
	LcdWriteCom(0x01);  //清屏
	LcdWriteCom(0x80);  //设置数据指针起点
}
//LCD Display

uchar TimeSelect()
{
	switch(Aim)
	{
		case (0): return Second;
		case (1): return Minute;
		case (2): return Hour;
	}
}

uchar GetKey()
{
	if (!MoveAim)
		CommonCounter = 1;
	else if (!ConditionChange)
		CommonCounter = 2;
	else if (!NumUp)
		CommonCounter = 3;
	else if (!NumDown)
		CommonCounter = 4;
	else if (!SetEdit)
		CommonCounter = 5;
	else if (!TimeEdit)
		CommonCounter = 6;
	else if (!Check)
		CommonCounter = 7;
	else
		CommonCounter = 0;
	if(CommonCounter == 0)
		GotKey = 1;
	else
		GotKey = 5;
	return CommonCounter;
}

void ResponseKey()
{
	switch (GetKey())
	{
	case (0):
		return;
	case (1) :
		if(Editing == 2)
		{
			Blank = 0;
			BlinkTime = 0;
			if (Aim < 3)
				++Aim;
			else
				Aim = 0;
			Blanking[0] = Time[Condition - 1][Timing[Aim]]/10+48;
			Blanking[1] = Time[Condition - 1][Timing[Aim]]%10+48;
		}
		else if (Editing == 1)
		{
			Blank = 0;
			BlinkTime = 0;
			if (Aim <2)
				++Aim;
			else 
				Aim = 0;
			Blanking[0] = TimeSelect()/10+48;
			Blanking[1] = TimeSelect()%10+48;
		}
		else
		{
			Blank = 0;
			BlinkTime = 0;
			Aim = 0;
		}
		return;
	case (2) :
		if (Condition < 3)
			++Condition;
		else
			Condition = 1;
		if(Editing == 0)
			RecDisplay == 0;
		else if(Editing == 2)
			Aim = 0;
		return;
	case (3) :
		Blank == 0;
		BlinkTime == 0;
		if (Editing == 2)
		{
			if(Aim % 2)
				if (Time[Condition - 1][Timing[Aim]] < 23)
					++Time[Condition - 1][Timing[Aim]];
				else
					Time[Condition - 1][Timing[Aim]] = 0;
			else
				if (Time[Condition - 1][Timing[Aim]] < 59)
					++Time[Condition - 1][Timing[Aim]];
				else
					Time[Condition - 1][Timing[Aim]] = 0;
			Blanking[0] = Time[Condition - 1][Timing[Aim]]/10+48;
			Blanking[1] = Time[Condition - 1][Timing[Aim]]%10+48;
		}
		else if(Editing == 1)
		{
			switch (Aim)
			{
				case 0: if (Second < 59)
									++Second;
								else
									Second = 0;
								break;
				case 1: if (Minute < 59)
										++Minute;
								else
										Minute = 0;
								break;
				case 2: if (Hour < 23)
										++Hour;
									else
										Hour = 0;
								break;
			}
			Blanking[0] = TimeSelect()/10+48;
			Blanking[1] = TimeSelect()%10+48;
		}
		return;
	case (4) :
		Blank == 0;
		BlinkTime == 0;
		if (Editing == 2)
		{
			if(Aim % 2)
				if (Time[Condition - 1][Timing[Aim]] > 0)
					--Time[Condition - 1][Timing[Aim]];
				else
					Time[Condition - 1][Timing[Aim]] = 23;
			else
				if (Time[Condition - 1][Timing[Aim]] > 0)
					--Time[Condition - 1][Timing[Aim]];
				else
					Time[Condition - 1][Timing[Aim]] = 59;	
			Blanking[0] = Time[Condition - 1][Timing[Aim]]/10+48;
			Blanking[1] = Time[Condition - 1][Timing[Aim]]%10+48;	
		}				
		else if(Editing == 1)
		{
			switch (Aim)
			{
				case 0: if (Second > 0)
									--Second;
								else
									Second = 59;
								break;
				case 1: if (Minute > 0)
									--Minute;
								else
									Minute = 59;
								break;
				case 2: if (Hour > 0)
									--Hour;
								else
									Hour = 23;
								break;
			}
			Blanking[0] = TimeSelect()/10+48;
			Blanking[1] = TimeSelect()%10+48;
		}
		return;
	case (5) :
		Condition = 1;
		if(Editing != 2)
		{
			Condition = 1;
			on = 1;
			Editing = 2;
			Aim = 0;
			Blanking[0] = Time[Condition - 1][Timing[Aim]]/10+48;
			Blanking[1] = Time[Condition - 1][Timing[Aim]]%10+48;
		}
		else
		{
			Condition = 0;
			Editing = 0;
		}
		return;
	case (6) :
		if(Editing != 1)
		{
			Editing = 1;
			on = 0;
			Aim = 0;
			Blanking[0] = TimeSelect()/10+48;
			Blanking[1] = TimeSelect()%10+48;
		}
		else
		{
			Editing = 0;
			on = 1;
			DisplayData[0][Aiming2[Aim]] = TimeSelect()/10;
			DisplayData[0][Aiming2[Aim] + 1] = TimeSelect()%10;
		}
		return;
	case (7) :
		RecDisplay = 0;
		if(Condition > 0)
			Condition = 0;
		else
			Condition = 1;
		return;
	}
}

void Blink()
{
	uchar A;
	if(Editing == 2)
		A = Aiming[Aim];
	else if(Editing == 1)
		A = Aiming2[Aim];
	if(++BlinkTime == 15)
	{
		BlinkTime = 0;
		if(Editing == 2)
		{
			if(Blank)
			{
				Blanking[0] = Time[Condition - 1][Timing[Aim]]/10+48;
				Blanking[1] = Time[Condition - 1][Timing[Aim]]%10+48;
			}
			else
			{
				Blanking[0] = ' ';
				Blanking[1] = ' ';
			}
			Blank = !Blank;
		}
		else if (Editing == 1)
		{
			if(Blank)
			{
				Blanking[0] = TimeSelect()/10+48;
				Blanking[1] = TimeSelect()%10+48;
			}
			else
			{
				Blanking[0] = ' ';
				Blanking[1] = ' ';
			}
			Blank = !Blank;
		}
	}
	else
		if(Editing == 2)
		{
			DisplayData[1][A] = Blanking[0];
			DisplayData[1][A + 1] = Blanking[1];
		}
		else if(Editing == 1)
		{
			DisplayData[0][A] = Blanking[0];
			DisplayData[0][A + 1] = Blanking[1];
		}
}

void Show()
{
	if(Editing == 2 || Editing == 0)
	{
		if(Condition > 0)
		{
			strcpy(DisplayData[1], "A: 00:00 ~ 00:00");
			switch(Condition)
			{
				case 1:
					DisplayData[1][0] = 'A'; break;
				case 2:
					DisplayData[1][0] = 'B'; break;
				case 3:
					DisplayData[1][0] = 'C'; break;
			}
			if(Editing == 0)
				Aim = 4;
			if(Aim != 0)
			{
				DisplayData[1][3] = Time[Condition - 1][0]/10 + 48;
				DisplayData[1][4] = Time[Condition - 1][0]%10 + 48;
			}
			if(Aim != 1)
			{
				DisplayData[1][6] = Time[Condition - 1][1]/10 + 48;
				DisplayData[1][7] = Time[Condition - 1][1]%10 + 48;
			}
			if(Aim != 2)
			{
				DisplayData[1][11] = Time[Condition - 1][3]/10 + 48;
				DisplayData[1][12] = Time[Condition - 1][3]%10 + 48;
			}
			if(Aim != 3)
			{
				DisplayData[1][14] = Time[Condition - 1][4]/10 + 48;
				DisplayData[1][15] = Time[Condition - 1][4]%10 + 48;
			}
			if(Editing == 0)
				Aim = 0;
			if(Editing == 2)
				Blink();
		}
	}
	else if(Editing == 1)
	{
		if(Aim != 2)
			DisplayData[0][8] = Hour/10 + 48;
			DisplayData[0][9] = Hour%10 + 48;
		if(Aim != 1)
			DisplayData[0][11] = Minute/10 + 48;
			DisplayData[0][12] = Minute%10 + 48;
		if(Aim != 0)
			DisplayData[0][14] = Second/10 + 48;
			DisplayData[0][15] = Second%10 + 48;
		Blink();
	}
	if(Editing != 2 && Condition == 0)
		if(State)
			strcpy(DisplayData[1], "Status:       ON");
		else
			strcpy(DisplayData[1], "Status:      OFF");
	if(Editing != 1)
	{
		DisplayData[0][14] = Second/10 + 48;
		DisplayData[0][15] = Second%10 + 48;
		DisplayData[0][11] = Minute/10 + 48;
		DisplayData[0][12] = Minute%10 + 48;
		DisplayData[0][8] = Hour/10 + 48;
		DisplayData[0][9] = Hour%10 + 48;
	}
	LcdWriteCom(0x80);
	for (CommonCounter = 0; CommonCounter < 16; ++CommonCounter)
		LcdWriteData(DisplayData[0][CommonCounter]);
	LcdWriteCom(0xC0);
	for (CommonCounter = 0; CommonCounter < 16; ++CommonCounter)
		LcdWriteData(DisplayData[1][CommonCounter]);
		
}

void AddSecond()
{
	if (Second < 59)
		++Second;
	else
	{
		Second = 0;
		if (Minute < 59)
			++Minute;
		else
		{
			Minute = 0;
			if (Hour < 23)
				++Hour;
			else
				Hour = 0;
		}
	}
}

void timer0()interrupt 1
{
	TH0 = 0x70;
	TL0 = 0x00;
	if (++MS40 == 25)
	{
		MS40 = 0;
		if(on)
		{
			AddSecond();
			CommonCounter = 3;
			while (CommonCounter--)
			{
				if (Hour == Time[2 - CommonCounter][0] && Minute == Time[2 - CommonCounter][1] && Second == Time[2 - CommonCounter][2])
					++State;
				if (Hour == Time[2 - CommonCounter][3] && Minute == Time[2 - CommonCounter][4] && Second == Time[2 - CommonCounter][5])
					--State;
			}
		}
		if (++RecDisplay == 10)
			Condition = 0;
	}
	if(--GotKey == 0)
		ResponseKey();
	Show();
}