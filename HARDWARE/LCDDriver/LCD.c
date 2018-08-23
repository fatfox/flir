/******************************************************************************

*重要说明！
在.h文件中，#define Immediately时是立即显示当前画面
而如果#define Delay，则只有在执行了LCD_WR_REG(0x0007,0x0173);
之后才会显示，执行一次LCD_WR_REG(0x0007,0x0173)后，所有写入数
据都立即显示。
#define Delay一般用在开机画面的显示，防止显示出全屏图像的刷新过程
******************************************************************************/
#include "stm32f10x.h"
#include "LCD.h"
#include "stm32f10x_fsmc.h"
#include "key.h"
#include "pic.h"
/*
 * 函数名：LCD_GPIO_Config
 * 描述  ：根据FSMC配置LCD的I/O
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用        
 */
 u8 color_mod=Iron;
 u8 test_mod=none;

extern long data[40][40];
extern long ext[3];
extern u8 ext_add[2];


void LCD_GPIO_Config(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable the FSMC Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    
    /* config lcd gpio clock base on FSMC */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    
    /* config tft rst gpio */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    /* config tft back_light gpio base on the PT4101 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 
    GPIO_Init(GPIOA, &GPIO_InitStructure);  		   
    
    /* config tft data lines base on FSMC
	 * data lines,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10
	 */	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
    /* config tft control lines base on FSMC
	 * PD4-FSMC_NOE  :LCD-RD
   * PD5-FSMC_NWE  :LCD-WR
	 * PD7-FSMC_NE1  :LCD-CS
   * PD11-FSMC_A16 :LCD-DC
	 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    /* tft control gpio init */	 

		GPIO_SetBits(GPIOD, GPIO_Pin_13);		 // RST = 1 
    GPIO_SetBits(GPIOD, GPIO_Pin_4);		 // RD = 1  
    GPIO_SetBits(GPIOD, GPIO_Pin_5);		 // WR = 1 
    GPIO_SetBits(GPIOD, GPIO_Pin_7);		 //	CS = 1 

}

/*
 * 函数名：LCD_FSMC_Config
 * 描述  ：LCD  FSMC 模式配置
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用        
 */

static void LCD_FSMC_Config(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p; 
    
    
    p.FSMC_AddressSetupTime = 0x02;	 //地址建立时间
    p.FSMC_AddressHoldTime = 0x00;	 //地址保持时间
    p.FSMC_DataSetupTime = 0x05;		 //数据建立时间
    p.FSMC_BusTurnAroundDuration = 0x00;
    p.FSMC_CLKDivision = 0x00;
    p.FSMC_DataLatency = 0x00;

    p.FSMC_AccessMode = FSMC_AccessMode_B;	 // 一般使用模式B来控制LCD
    
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p; 
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    
    /* Enable FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}

volatile static void Delay(__IO u32 nCount)
{	
	volatile int i;
	for(i=0;i<7200;i++)
    for(; nCount != 0; nCount--);
}  
  
u16 ssd1289_GetPoint(u16 x,u8 y)
{
	 u16 a = 0;
	
	*(__IO u16 *) (Bank1_LCD_C) = 0x4f;	
	*(__IO u16 *) (Bank1_LCD_D) = x;

	*(__IO u16 *) (Bank1_LCD_C) = 0x4e;	
	*(__IO u16 *) (Bank1_LCD_D) = y;

	*(__IO u16 *) (Bank1_LCD_C) = 0x22;

	 a = *(__IO u16 *) (Bank1_LCD_D); 
   return(a);	  
}
volatile static void LCD_Rst(void)
{			
    Clr_Rst;
    Delay(1000);					   
    Set_Rst;		 	 
    Delay(1000);	
}
static void WriteComm(u16 CMD)
{			
//	CMD=(CMD<<4)|(CMD>>4);

//CMD=((CMD<<2)&0xcc)|((CMD>>2)&0x33);

//CMD=((CMD<<1)&0xaa)|((CMD>>1)&0x55);

	*(__IO u16 *) (Bank1_LCD_C) = CMD;
}
static void WriteData(u16 tem_data)
{			
//	tem_data=(tem_data<<4)|(tem_data>>4);

//tem_data=((tem_data<<2)&0xcc)|((tem_data>>2)&0x33);

//tem_data=((tem_data<<1)&0xaa)|((tem_data>>1)&0x55);
	*(__IO u16 *) (Bank1_LCD_D) = tem_data;
}
u16 ReadPixel(u16 x,u8 y)
{
	u16 dat,temp;
// 	BlockWrite(x,x,y,y);
	WriteComm(0x2a);   
	WriteData(x>>8);
	WriteData(x&0xff);
	WriteComm(0x2b);   
	WriteData(y>>8);
	WriteData(y&0xff);
	WriteComm(0x2e);
	dat = *(__IO u16 *) (Bank1_LCD_D);
	dat = *(__IO u16 *) (Bank1_LCD_D);
	temp = *(__IO u16 *) (Bank1_LCD_D);
	dat = (dat&0xf800)|((dat&0x00fc)<<3)|(temp&0xFF00)>>11;
	return dat;
	
// 	dat = (dat&0xf800)|((dat&0x00fc)<<3)|((*(__IO u16 *) (Bank1_LCD_D))>>11);
}
/**********************************************
Lcd初始化函数
***********************************************/

void Lcd_Clear(void)
{
	u16 temp;
	BlockWrite(0,127,0,159);
	for (temp=0; temp<128*160*2; temp++)
	{
		*(__IO u16 *) (Bank1_LCD_D) = 0;
	}
}

void Lcd_Initialize(void)
{	
LCD_GPIO_Config();
LCD_FSMC_Config();
Delay(20);
LCD_Rst();

WriteComm(0x11); //Sleep out
Delay(120); //Delay 120ms
//------------------------------------ST7735S Frame Rate-----------------------------------------//
WriteComm(0xB1);
WriteData(0x05);
WriteData(0x3A);
WriteData(0x3A);
WriteComm(0xB2);
WriteData(0x05);
WriteData(0x3A);
WriteData(0x3A);
WriteComm(0xB3);
WriteData(0x05);
WriteData(0x3A);
WriteData(0x3A);
WriteData(0x05);
WriteData(0x3A);
WriteData(0x3A);
//------------------------------------End ST7735S Frame Rate-----------------------------------------//
WriteComm(0xB4); //Dot inversion
WriteData(0x03);

//------------------------------------ST7735S Power Sequence-----------------------------------------//
WriteComm(0xC0);
WriteData(0x62);
WriteData(0x02);
WriteData(0x04);
WriteComm(0xC1);
WriteData(0XC0);
WriteComm(0xC2);
WriteData(0x0D);
WriteData(0x00);
WriteComm(0xC3);
WriteData(0x8D);
WriteData(0xEA);
WriteComm(0xC4);
WriteData(0x8D);
WriteData(0xEE);
//---------------------------------End ST7735S Power Sequence-------------------------------------//
WriteComm(0xC5); //VCOM
WriteData(0x0D);
WriteComm(0x36); //MX, MY, RGB mode
WriteData(0xC8);
//------------------------------------ST7735S Gamma Sequence-----------------------------------------//
WriteComm(0xE0);
WriteData(0x0A);
WriteData(0x1F);
WriteData(0x0E);
WriteData(0x17);
WriteData(0x37);
WriteData(0x31);
WriteData(0x2B);
WriteData(0x2E);
WriteData(0x2C);
WriteData(0x29);
WriteData(0x31);
WriteData(0x3C);
WriteData(0x00);

WriteData(0x05);
WriteData(0x03);
WriteData(0x0D);
WriteComm(0xE1);
WriteData(0x0B);
WriteData(0x1F);
WriteData(0x0E);
WriteData(0x12);
WriteData(0x28);
WriteData(0x24);
WriteData(0x1F);
WriteData(0x25);
WriteData(0x25);
WriteData(0x26);
WriteData(0x30);
WriteData(0x3C);
WriteData(0x00);
WriteData(0x05);
WriteData(0x03);
WriteData(0x0D);
//------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
WriteComm(0x3A); //65k mode
WriteData(0x55);


WriteComm(0x29); //Display on
	
Lcd_Clear();

Lcd_Light_ON;
//Lcd_ColorBox(0,0,240,320,Yellow);


// printf("ReadPixel=%04x\r\n",ReadPixel(10, 9));
// DrawPixel(10, 10, 0xaaaa);
// printf("ReadPixel=%04x\r\n",ReadPixel(10, 10));
// DrawPixel(10, 11, 0XFFFF);
// printf("ReadPixel=%04x\r\n",ReadPixel(10, 11));
// DrawPixel(10, 12, 0X00);
// printf("ReadPixel=%04x\r\n",ReadPixel(10, 12));

// DrawPixel(10, 13, 0Xf800);
// printf("ReadPixel=%04x\r\n",ReadPixel(10, 13));
// DrawPixel(10, 14, 0X3e0);
// printf("ReadPixel=%04x\r\n",ReadPixel(10, 14));
// DrawPixel(10, 15, 0X1f);
// printf("ReadPixel=%04x\r\n",ReadPixel(10, 15));
// WriteComm(0x20);


// while(1);
}
/******************************************
函数名：Lcd写命令函数
功能：向Lcd指定位置写入应有命令或数据
入口参数：Index 要寻址的寄存器地址
          ConfigTemp 写入的数据或命令值
******************************************/
void LCD_WR_REG(u16 Index,u16 CongfigTemp)
{
	*(__IO u16 *) (Bank1_LCD_C) = Index;	
	*(__IO u16 *) (Bank1_LCD_D) = CongfigTemp;
}
/************************************************
函数名：Lcd写开始函数
功能：控制Lcd控制引脚 执行写操作
************************************************/
void Lcd_WR_Start(void)
{
*(__IO u16 *) (Bank1_LCD_C) = 0x2C;
}
/**********************************************
函数名：Lcd块选函数
功能：选定Lcd上指定的矩形区域

注意：xStart、yStart、Xend、Yend随着屏幕的旋转而改变，位置是矩形框的四个角

入口参数：xStart x方向的起始点
          ySrart y方向的起始点
          Xend   y方向的终止点
          Yend   y方向的终止点
返回值：无
***********************************************/
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend) 
{
	Xstart += 2;
	Xend += 2;
	Ystart += 1;
	Yend += 1;
	
	WriteComm(0x2a);   
	WriteData(Xstart>>8);
	WriteData(Xstart&0xff);
	WriteData(Xend>>8);
	WriteData(Xend&0xff);

	WriteComm(0x2b);   
	WriteData(Ystart>>8);
	WriteData(Ystart&0xff);
	WriteData(Yend>>8);
	WriteData(Yend&0xff);


	WriteComm(0x2c);
}
/**********************************************
函数名：Lcd块选函数
功能：选定Lcd上指定的矩形区域

注意：xStart和 yStart随着屏幕的旋转而改变，位置是矩形框的四个角

入口参数：xStart x方向的起始点
          ySrart y方向的终止点
          xLong 要选定矩形的x方向长度
          yLong  要选定矩形的y方向长度
返回值：无
***********************************************/
void Lcd_ColorBox(u16 xStart,u16 yStart,u16 xLong,u16 yLong,u16 Color)
{
	u32 temp;
	BlockWrite(xStart,xStart+xLong-1,yStart,yStart+yLong-1);
	for (temp=0; temp<xLong*yLong; temp++)
	{
//		WriteData(Color>>8);
//		WriteData(Color);
		*(__IO u16 *) (Bank1_LCD_D) = Color>>8;
		*(__IO u16 *) (Bank1_LCD_D) = Color;
	}
}

/******************************************
函数名：Lcd图像填充100*100
功能：向Lcd指定位置填充图像
入口参数：Index 要寻址的寄存器地址
          ConfigTemp 写入的数据或命令值
******************************************/
void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic)
{
  unsigned long i;

//	WriteComm(0x36); //Set_address_mode
//	WriteData(0x00); //竖屏
	
	BlockWrite(x,x+pic_H-1,y,y+pic_V-1);
	for (i = 0; i < pic_H*pic_V*2; i++)
	{
//		WriteData(pic[i]);
		*(__IO u16 *) (Bank1_LCD_D) = pic[i];
	}
//	WriteComm(0x36); //Set_address_mode
//	WriteData(0xC0); //横屏
}



void LCD_Pic2(u16 x, u8* pic)
{
  unsigned long i;
	BlockWrite(x,x,0,159);
	for (i = 0; i < 160*2; i++)
	{
		*(__IO u16 *) (Bank1_LCD_D) = pic[319-i];
	}
}


void DrawPixel(u16 x, u16 y, u16 Color)
{
	BlockWrite(x,x+1,y,y+1);
	*(__IO u16 *) (Bank1_LCD_D) = Color;
}


void DrawBack(void){
	LCD_Fill_Pic(0,0,128,160,gImage_back);
}



u16 To_HSB(u8 num){
	u8 R=0,G=0,B=0;
	float a;
	u32 b;
	if(color_mod==Iron){
		a=0.7*num;
		a+=20;
		num=(u8)a;
		if (num < 64) {
			B = (unsigned char)(num * 4);
			G = 0;
			R = 0;
		}
		else if (num < 96) {
			B = 255;
			G = 0;
			R = (unsigned char)(4 * (num - 64));
		}
		else if (num < 128) {
			B = (unsigned char)(256 - 8 * (num - 95));
			G = 0;
			R = (unsigned char)(4 * (num - 64) - 1);
		}
		else if (num < 191) {
			B = 0;
			G = (unsigned char)(4 * (num - 128));
			R = 255;
		}
		else {
			B = (unsigned char)(4 * (num - 191));
			G = 255;
			R = 255;
		}
	}else if(color_mod==BW){
		R=G=B=num;
	}else if(color_mod==RB){
		b=240*num;
		b/=255;
		num=(u8)b;
		if (num < 60) {
			B = 255;
			G = num*4;
			R = 0;
		}
		else if (num < 120) {
			B = (120-num)*4;
			G = 255;
			R = 0;
		}
		else if (num < 180) {
			B = 0;
			G = 255;
			R = (num-119)*4;
		}else{
			B = 0;
			G = (240-num)*4;
			R = 255;
		}
	}
	// RRRRR GGGGG ? BBBBB//
	
	return 0xFFFF&((B&0xf8)>>3|(G&0xf8)<<3|(R&0xf8)<<8);
}	


void logo_move(void){
	static u8 step=0;
	u8 i;
	step++;
	if(step==8)step=0;
  BlockWrite(0,2,152,159);
	for (i = 0; i < 3*8*2; i++){
		*(__IO u16 *) (Bank1_LCD_D) = gImage_logo[i+(7-step)*3*2];
	}
}



void Draw_battery(u8 num){
	u16 i;
	u16 color;
	u8 bat=Battery_Scan();
	if(bat==0){
		num/=5;
		if(num<4)color=Red;
		else if(num<8)color=Yellow;
		else color=Green;
		BlockWrite(24,30,5,24);
		for (i = 0; i < 7*num; i++){
			*(__IO u16 *) (Bank1_LCD_D) = color>>8;
			*(__IO u16 *) (Bank1_LCD_D) = color;
		}
		for (i = 0; i < 7*(20-num); i++){
			*(__IO u16 *) (Bank1_LCD_D) = 0;
			*(__IO u16 *) (Bank1_LCD_D) = 0;
		}
	}else if(bat==1){
		LCD_Fill_Pic(24,5,7,20,gImage_battery1);
	}else if(bat==2){
		LCD_Fill_Pic(24,5,7,20,gImage_battery2);
	}
}


void blowup(void) {
	int i;
	for (i = 0; i < 8 * 7; i++) {
		data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 1] = 1+ data[i / 7 * 5 + 2][i % 7 * 5 + 2] * t4 + data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 5] * t1;
		data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 2] = 1+ data[i / 7 * 5 + 2][i % 7 * 5 + 2] * t3 + data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 5] * t2;
		data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 3] = 1+ data[i / 7 * 5 + 2][i % 7 * 5 + 2] * t2 + data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 5] * t3;
		data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 4] = 1+ data[i / 7 * 5 + 2][i % 7 * 5 + 2] * t1 + data[i / 7 * 5 + 2][i % 7 * 5 + 2 + 5] * t4;
	}
	for (i = 0; i < 7 * 36; i++) {
		data[i % 7 * 5 + 2 + 1][i / 7 + 2] = 1+ data[i % 7 * 5 + 2][i / 7 + 2] * t4 + data[i % 7 * 5 + 2 + 5][i / 7 + 2] * t1;
		data[i % 7 * 5 + 2 + 2][i / 7 + 2] = 1+ data[i % 7 * 5 + 2][i / 7 + 2] * t3 + data[i % 7 * 5 + 2 + 5][i / 7 + 2] * t2;
		data[i % 7 * 5 + 2 + 3][i / 7 + 2] = 1+ data[i % 7 * 5 + 2][i / 7 + 2] * t2 + data[i % 7 * 5 + 2 + 5][i / 7 + 2] * t3;
		data[i % 7 * 5 + 2 + 4][i / 7 + 2] = 1+ data[i % 7 * 5 + 2][i / 7 + 2] * t1 + data[i % 7 * 5 + 2 + 5][i / 7 + 2] * t4;
	}
	for (i = 0; i < 36; i++) {
		data[0][i + 2] = data[1][i + 2] = data[2][i + 2];
		data[39][i + 2] = data[38][i + 2] = data[37][i + 2];
	}
	for (i = 0; i < 40; i++) {
		data[i][0] = data[i][1] = data[i][2];
		data[i][39] = data[i][38] = data[i][37];
	}
	ext[2]=data[19][19];
}

void get_img(void){
	u16 i;
	long diff=ext[0]-ext[1]+2;
	for(i=0;i<40*40;i++){
		data[i/40][i%40]=To_HSB(0xff&((data[i/40][i%40]-ext[1]+1)*0xff/diff));
	}
}

void Draw_img(void){
	u16 i;
	//BlockWrite(4,123,40,159);
	for (i = 0; i < 40*40; i++){
		Lcd_ColorBox(4+i/40*3,40+i%40*3,3,3,data[i/40][i%40]);
	}
}

void Draw_A_num(u16 x, u16 y,u8 size,u8 back,u8 num){
  u16 i;
	switch (size){
		case 5:{
			BlockWrite(x,x+9-1,y,y+5-1);
			if(back==0){
				for(i=0;i<45;i++){
					*(__IO u16 *) (Bank1_LCD_D) = gImage_num5[(num*45+i)*2];
					*(__IO u16 *) (Bank1_LCD_D) = gImage_num5[(num*45+i)*2+1];
				}
			}else{
				for(i=0;i<45;i++){
					*(__IO u16 *) (Bank1_LCD_D) = ~gImage_num5[(num*45+i)*2];
					*(__IO u16 *) (Bank1_LCD_D) = ~gImage_num5[(num*45+i)*2+1];
				}
			}
			break;
		}
		
		case 9:{
			BlockWrite(x,x+18-1,y,y+9-1);
			if(back==0){
				for(i=0;i<162;i++){
					*(__IO u16 *) (Bank1_LCD_D) = gImage_num9[(num*162+i)*2];
					*(__IO u16 *) (Bank1_LCD_D) = gImage_num9[(num*162+i)*2+1];
				}
			}else{
				for(i=0;i<162;i++){
					*(__IO u16 *) (Bank1_LCD_D) = ~gImage_num9[(num*162+i)*2];
					*(__IO u16 *) (Bank1_LCD_D) = ~gImage_num9[(num*162+i)*2+1];
				}
			}
			break;
		}
		
		default:
			break;
	}
	
}


void Draw_menu(void){
	u16 i;
	if(test_mod==none){
		BlockWrite(35,69,0,16);
		for (i = 0; i < 1190; i++){
			*(__IO u16 *) (Bank1_LCD_D) = gImage_menu[i];
		}
	}else if(test_mod==midd){
		BlockWrite(35,69,0,16);
		for (i = 0; i < 1190; i++){
			*(__IO u16 *) (Bank1_LCD_D) = gImage_menu[1190+i];
		}
	}else if(test_mod==exts){
		BlockWrite(35,69,0,16);
		for (i = 0; i < 1190; i++){
			*(__IO u16 *) (Bank1_LCD_D) = gImage_menu[2380+i];
		}
	}
}

void Draw_Num(u16 x,u16 y,u8 bk,int num){    //右上角的位置
	if(num>=1000){
		if(bk){
			Lcd_ColorBox(x,y+9,18,2,White);
			Lcd_ColorBox(x,y+20,18,2,White);
		}else{
			Lcd_ColorBox(x,y+9,18,2,Black);
			Lcd_ColorBox(x,y+20,18,2,Black);
		}
		Draw_A_num(x,y+22,9,bk,num/1000%10);     //max
		Draw_A_num(x,y+11,9,bk,num/100%10);
		Draw_A_num(x,y,9,bk,num/10%10);
	}else if(num>=0){
		if(bk){
			Lcd_ColorBox(x,y,9,5,White);
			Lcd_ColorBox(x,y+5,18,1,White);
			Lcd_ColorBox(x,y+8,18,2,White);
			Lcd_ColorBox(x,y+6,16,2,White);
			Lcd_ColorBox(x+16,y+6,2,2,Black);
			Lcd_ColorBox(x,y+19,18,2,White);
			Lcd_ColorBox(x,y+30,18,1,White);
		}else{ 
			Lcd_ColorBox(x,y,9,5,Black);
			Lcd_ColorBox(x,y+5,18,1,Black);
			Lcd_ColorBox(x,y+8,18,2,Black);
			Lcd_ColorBox(x,y+6,16,2,Black);
			Lcd_ColorBox(x+16,y+6,2,2,White);
			Lcd_ColorBox(x,y+19,18,2,Black);
			Lcd_ColorBox(x,y+30,18,1,Black);
		}
		Draw_A_num(x,y+21,9,bk,num/100%10);     //max
		Draw_A_num(x,y+10,9,bk,num/10%10);
		Draw_A_num(x+9,y,5,bk,num%10);
	}else if(num>=-99){
		num=-num;
		if(bk){
			Lcd_ColorBox(x,y,9,5,White);
			Lcd_ColorBox(x,y+5,18,1,White);
			Lcd_ColorBox(x,y+8,18,2,White);
			Lcd_ColorBox(x,y+6,16,2,White);
			Lcd_ColorBox(x+16,y+6,2,2,Black);
			Lcd_ColorBox(x,y+19,18,2,White);
			LCD_Fill_Pic(x,y+21,18,10,gImage_neg_W);
		}else{
			Lcd_ColorBox(x,y,9,5,Black);
			Lcd_ColorBox(x,y+5,18,1,Black);
			Lcd_ColorBox(x,y+8,18,2,Black);
			Lcd_ColorBox(x,y+6,16,2,Black);
			Lcd_ColorBox(x+16,y+6,2,2,White);
			Lcd_ColorBox(x,y+19,18,2,Black);
			LCD_Fill_Pic(x,y+21,18,10,gImage_neg_B);
		}
		Draw_A_num(x,y+10,9,bk,num/10%10);
		Draw_A_num(x+9,y,5,bk,num%10);
	}else{
		num=-num;
		if(bk){
			Lcd_ColorBox(x,y+9,18,2,White);
			LCD_Fill_Pic(x,y+21,18,10,gImage_neg_W);
		}else{
			Lcd_ColorBox(x,y+9,18,2,Black);
			LCD_Fill_Pic(x,y+21,18,10,gImage_neg_B);
		}
		Draw_A_num(x,y+11,9,bk,num/100%10);     //max
		Draw_A_num(x,y,9,bk,num/10%10);
	}
}


void Draw_data(void){
	int max=(int)(ext[0])*10/4;
	int min=(int)(ext[1])*10/4;
	int mid=(int)(ext[2])*10/4;
	if(test_mod==midd){            //mid
		Lcd_ColorBox(61,100,3,3,Black);
		Lcd_ColorBox(66,82,24,35,White);
		
  	Draw_Num(69,84,1,mid);
	}else if(test_mod==exts){
		Lcd_ColorBox(10+(7-ext_add[0]/8)*15,46+(ext_add[0]%8)*15,3,3,Black);
		Lcd_ColorBox(10+(7-ext_add[1]/8)*15,46+(ext_add[1]%8)*15,3,3,White);
	}
	Draw_Num(0,0,0,max);
	Draw_Num(110,0,0,min);
}



void Draw_color(void){
	if(color_mod==Iron)LCD_Fill_Pic(4,33,120,7,gImage_Iron);
	else if(color_mod==RB)LCD_Fill_Pic(4,33,120,7,gImage_RB);
	else if(color_mod==BW)LCD_Fill_Pic(4,33,120,7,gImage_BW);
}


void Draw_Camera(void){
	LCD_Fill_Pic(91,8,13,16,gImage_camera);
}

void Draw_Wait(void){
	LCD_Fill_Pic(91,8,13,16,gImage_wait);
}





















