/***********************************************************
              北京航空航天大学
  文件名       ：main.c
  说明         ：CCD整车程序
************************************************************/

#include "include.h"
#include "calculation.h"

/*************************
设置系统的全局变量
*************************/
extern u8 LPT_INT_count ;
extern u8 TIME0flag_5ms ;
extern u8 TIME0flag_10ms ;
extern u8 TIME0flag_15ms ;
extern u8 TIME0flag_20ms ;
extern u8 TIME0flag_80ms ;
extern u8 TIME1flag_1s ;
extern u8 TIME1flag_20ms ;
/********
全局
********/
u8 Atep8B0 ;
u16 ASPeed1 ;         //目标车速
/*********
PWM变量
*********/
u16 PWMCount ;        //计算车速
u16 PWMC ;
/*********
CCD变量
*********/
uint8_t TurnPeriodCount;
uint8_t Atem8B0;
uint8_t ALeftLineAryy[50], ARightLineAryy[50],ALastLeftLinAryy[50],ALastRightLinAryy[50];
uint8_t Pixel[138];
uint8_t send_data_cnt = 0;
uint8_t *pixel_pt;
uint8_t CCDStation;
uint8_t CCDMIDSTATION = 64;
uint16_t CCDTurnPWM;
uint8_t i;
u16 count = 0;                    				//采集车速

void main()
{
  DisableInterrupts;                    //禁止总中断
  /*********************************************************
  初始化全局变量
  *********************************************************/
  ASPeed1 = 10 ;
  pixel_pt = Pixel;
  for(Atep8B0=0; Atep8B0<128+10; Atep8B0++) {
    *pixel_pt++ = 0;
  }
  for(Atem8B0=0 ; Atem8B0 < 50; Atem8B0 ++) {
    ALeftLineAryy[Atem8B0]  = 0X11;
  }
  for(Atem8B0=0 ; Atem8B0 < 50; Atem8B0 ++) {
    ALastLeftLinAryy[Atem8B0]  = 0X11;
  }
  for(Atem8B0=0 ; Atem8B0 < 50; Atem8B0 ++) {
    ARightLineAryy[Atem8B0]  = 0X6b;
  }
  for(Atem8B0=0 ; Atem8B0 < 50; Atem8B0 ++) {
    ALastRightLinAryy[Atem8B0]  = 0X6b;
  }

  /*********************************************************
  初始化程序
  *********************************************************/
  uart_init (UART0 , 115200);               //初始化UART0，输出脚PTA15，输入脚PTA14，串口频率 15200
  gpio_init (PORTA , 16, GPO,HIGH);
  pit_init_ms(PIT0, 5);                      //初始化PIT0，定时时间为： 5ms
  pit_init(PIT1, 10000);                     //初始化PIT1，定时时间为： 0.2ms
  CCD_init();                                //初始化CCD传感器
  MOTORPWM_init();                                         //电机初始化
  TURNPWM_init();                                         //转向初始化
  EnableInterrupts;			                    //开总中断

  /******************************************
    执行程序
  ******************************************/
  while(1)
  {
    /*********************
      5ms程序执行代码段.0
    *********************/
    if(TIME0flag_5ms == 1)
    {
      TIME0flag_5ms = 0 ;
    }

    /*********************
      10ms程序执行代码段
    *********************/
    if(TIME0flag_10ms == 1)
    {
      TIME0flag_10ms = 0 ;
      count = LPTMR0_CNR;                                  //保存脉冲计数器计算值
      lptmr_counter_clean();                              //清空脉冲计数器计算值（马上清空，这样才能保证计数值准确）
      count = LPT_INT_count * LIN_COUT +  count;         //间隔10ms的脉冲次数
      LPT_INT_count = 0;                                //清空LPT中断次数

      PWMCount = SpeedPID( count,ASPeed1);    //Count采集车速，ASPeed1目标车速
      PWMC =  PWMCount*100/313 ;
      if(PWMC > 50)                       //限速
        PWMC = 50 ;
        //PWMC = 0 ;
      FTM_PWM_Duty(FTM0 , CH0,0);         //输出速度
      FTM_PWM_Duty(FTM0 , CH1,PWMC);
    }

    /*********************
      15ms程序执行代码段
    *********************/
    if(TIME0flag_15ms == 1)
    {
      TIME0flag_15ms = 0 ;
      //uart_putchar(UART0,0xff);
    }

    /*********************
      20ms程序执行代码段
    *********************/
    if(TIME1flag_20ms == 1)
    {
      TIME1flag_20ms = 0 ;
      ImageCapture(Pixel);    //CCD采样程序
      /* Calculate Integration Time */
      CalculateIntegrationTime();  //计算曝光时间
      if(++send_data_cnt >= 5) {
        send_data_cnt = 0;
      }

      CCD_Point(Pixel,0,ALeftLineAryy,ARightLineAryy,ALastLeftLinAryy,ALastRightLinAryy);  //采集黑线位置进行黑线点数计算

      CCDStation = CCD_PointOUT(1 ,ALeftLineAryy , ARightLineAryy) ;//根据CCD采集的位置，拟合转向值

      CCDTurnPWM = TurnPWM(CCDStation,CCDMIDSTATION) ;

      FTM_CnV_REG(FTMx[FTM1], CH1) = CCDTurnPWM ;     //转向赋值   舵机2
    }

    /*********************
      1s程序执行代码段
    *********************/
    if(TIME0flag_80ms == 1)
    {
      TIME0flag_80ms = 0 ;
      PTA16_OUT = ~PTA16_OUT ;
    }
  }
}
