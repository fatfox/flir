#ifndef _VarDefine_
#define _VarDefine_
#include "sys.h"


#define APP_ADDR  0x08004000

// #define SIZEx5 5

#define SIZEx8 8  //插值数量选择


#define LcdOldVersion 		//新版的屏幕注释掉这一行。


#ifdef SIZEx5

#define PixLg 40   //軟件分辨率
#define PixGain 5    //軟件分辨率

#define FileBase 54414	//文件bmp區域容量	
#define FileNow 60830		//當前版本適配容量

#define t1 1000/5000     //gain
#define t2 2000/5000
#define t3 3000/5000
#define t4 4000/5000

#endif



#ifdef SIZEx8

#define PixLg 59   //軟件分辨率
#define PixGain 8    //軟件分辨率

#define FileBase 54414	//文件bmp區域容量	
#define FileNow 68354		//當前版本適配容量

#define t1 1000/8000     //gain
#define t2 2000/8000
#define t3 3000/8000
#define t4 4000/8000
#define t5 5000/8000
#define t6 6000/8000
#define t7 7000/8000

#endif


typedef enum DispStep{
  Normal = 1,
  Pause,
  Play,
}DispStep;


typedef enum ColrMode{
  Iron = 1,
  RB,
  BW,
}ColrMode;

typedef enum DispMeas{
  none = 1,
  midd,
  exts,
}DispMeas;


struct SysFlag_BITS{
	u8 SaveFlag:1;
	u8 RefreshFlag:1;
};

union SysFlag_REG {
   u16                   	all;
   struct SysFlag_BITS  	bit;
};

struct SysState_REG{
	u8 DispStep;
	u8 ColrMode;
	u8 ColrModeBak;
	u8 DispMeas;
	u8 LightLV;
	u8 LightLVBak;
	u16 PlayNum;
	u16 SaveNum;
	union SysFlag_REG SysFlag;
};


/*----------------------------*/

struct SysTime_REG
{
	volatile u16 SysTimeCNT10ms;
	volatile u16 SysTimeCNT100ms;
	volatile u16 SysTimeCNT1s;
	volatile u16 SysTimeCNT1min;
	volatile u16 SysTimeCNT1h;
		
	volatile u8 SysTimeFLG10ms;
	volatile u8 SysTimeFLG100ms;
	volatile u8 SysTimeFLG1s;
	volatile u8 SysTimeFLG1min;
	volatile u8 SysTimeFLG1h;
};  


/*----------------------------*/


struct KeyFlag_BITS{
	u16 IOActu:1;	//实时
	u16 IOLast:1;	//上次状态，用于滤波
	u16 IOFilt:1;	//滤波后的值
	u16 KeyOut:2;	//单次触发位  		//1:按下首次 2:短按弹起 3:长按弹起
	u16 KeyErr:1;	//按键长按忽略
};

union KeyFlag_REG{
	u16											all;
	struct KeyFlag_BITS 		bit;
};

struct AKeyState_REG{
	union KeyFlag_REG Flag;
	u16 PressTime;
	u16 HoldTime;
};

struct KeyState_REG{
	struct AKeyState_REG Key1;
	struct AKeyState_REG Key2;
	struct AKeyState_REG Key3;
	struct AKeyState_REG Key4;
};

/*----------------------------*/


extern FATFS fs;
extern DIR dr;
extern FIL fdst_f;
extern FRESULT res_f;


extern long data[PixLg][PixLg];
extern long ext[3];
extern u8 ext_add[2];

extern u8 SAVE_NUM[2];

extern char name_buf[64];
extern char dir_buf[64];

extern u8 RW_Buf[600];


extern struct SysState_REG SysState;
extern struct SysTime_REG SysTime;
extern struct KeyState_REG KeyState;



#endif
