/***********************************************************
              北京航空航天大学
  文件名       ：main.c
  说明         ：摄像头整车程序
************************************************************/

#include "include.h"  // Include.h 包含用户自定义的头文件 中值
#include  "Landzo_Calculation.h"

/***********************定义变量***************************/
uint8 TIME0flag_5ms=0;
uint8 TIME0flag_10ms=0;
uint8 TIME0flag_20ms=0;
uint8 TIME0flag_50ms=0;
uint8 TIME0flag_80ms=0;
uint8 TIME0flag_100ms=0;
uint8 TIME0flag_1s =0;
uint8 TIME1flag_20ms=0;

extern uint8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
extern uint16 USART_RX_STA;    

extern  uint8  DMA_Over_Flg ;            //采集完成标志位
extern  uint8  LinADCout ;
extern  uint8_t  LandzoRAM[ ];
extern  vuint8 key5,key4,key3,key2,key1 ;

uint16 TurnPWM_Value ;
uint16 LinCout =0;
uint8 Atem8B0 = 0 ;
int8  Station ;
uint8 ALineOverCout = 0 ;                 //采集计数完成行数
uint8 ALineCal = 0 ;    
uint8  MIDSTATION = 71 ;                 //CCD传感器表示的黑线的中间位置
uint16 Sum_Station = 0 ;
uint8  Rightblackedge[DATALINE]={0};
uint8  Leftblackedge[DATALINE]={0};
uint8  CameraStation[DATALINE]={0};
uint8 ADdata[DATALINE][DATACOUNT] ={0} ;        //黑线AD数组存储
uint16 Atemp0 ;
uint8  checkflg = 0 ;     
uint16 i=0,j=0;
uint8  ii ;

/*********************NRF模块变量******************/
uint8  bksartflag = 1 ;
uint8  reflag = 0 ;
uint8  rebufer[32] ={36};
uint16 trnbufer[8] ;
uint8  releth;

extern uint8 frameflag ;
/****************速度变量****************/
int16 speedcout1,speedcount2 ;
uint16 Set_Speed=0 ;                                  //目标速度
uint16 Speed_Count =0 ;                          //脉冲计数器
uint16 SpeedPWM_Value=0 ;                          //PID计算的PWM 

uint8 Speed_P;
uint8 Speed_I;
uint8 Speed_D;

extern vuint8 LPT_INT_count ;  //速度2的count
extern vuint8 DMA_count_Flg  ; //速度1的count


/***********************定义变量***************************/

/************************主函数***************************/
void main(void)
{    
  DisableInterrupts;   //禁止总中断 
  gpio_init(PTA28,GPO,1) ;  //LED GPIO引脚初始化配置 推挽输出 输出为高电平
  gpio_init(PTC13,GPO,0) ;  //Buzzer ,高电平响 
  uart_init(UART0 , 115200);//串口初始化 串口：UART0 波特率：115200
  uart_init(UART5 , 115200);//串口初始化 串口：UART0 波特率：115200
  pit_init_ms(PIT0,5); //周期中断定时器PIT0 定时5ms  
  set_irq_priority((INT_PIT0+INC_IRQ),2);//设置中断优先级 2
  setIRQadrr(INT_PIT0,PIT0_IRQHandler);  //设置PIT1定时器中断地址函数
  
  IICWriteGpio_inint();
  //  while(LandzoIICEEROM_INIT()!= 1);

  /***********************初始化摄像头采样***************************/
  COMS_INC ();    //场中断为  14，行中断为13，数据为PTE0-PTE7，DMA触发为PTD12，采集数据的阵列为160 X100，
                  //在使用程序的时候需要查看 calculation.h 中的具体定义 
                  /* #define  DATALINE   100     //采样行数
                    #define  DATACOUNT  160 //采样点数 */ 
  
  OLED_Init();    //OELD显示
  Servo_motor_SD5_init();   //舵机初始
  Servo_motor_SD5_centre();
  Motor_double_PWM_init();   //电机初始化

  speed_count_Init();

  for(i = 0;i < DATALINE;i ++) //ADdata[i][j] 初始化赋值
  {
    for(j = 0;j < DATACOUNT;j ++)
    {
      ADdata[i][j] = 0;     
    }
  } 

  //速度变量初始化
  Set_Speed = 80 ;  //64
  Speed_P=5;
  Speed_I=0;
  Speed_D=0;
  
  EnableInterrupts;   //开启总中断  
  while(1)
  {
    if(TIME0flag_5ms)
    {
      TIME0flag_5ms = 0 ;
      // uart_putchar(UART0,0xff);
      // uart_putchar(UART0,0xff);
      // uart_putchar(UART5,0xff);
      // uart_putchar(UART0,0xff);
    }
    if(TIME0flag_20ms)
    {
      TIME0flag_20ms = 0 ;
      
      speed1_count_read(&DMA_count_Flg,&speedcout1,&LPT_INT_count,&speedcount2) ;
      Speed_Count = (speedcout1+speedcount2)/2 ;
      SpeedPWM_Value =SpeedControl(Speed_Count,Set_Speed,Speed_P,Speed_I,Speed_D) ;    //PID函数计算PWM值      
      // SpeedPWM_Value = 0 ;
      FTM_PWM_Duty(FTM0 , CH0,0);                                  //FTM0 CH0 PWM输出 ；电机控制
      FTM_PWM_Duty(FTM0 , CH1,SpeedPWM_Value);                     //FTM0 CH1 PWM输出 ；电机控制  
      FTM_PWM_Duty(FTM0 , CH2,0);                                  //FTM0 CH2 PWM输出 ；电机控制
      FTM_PWM_Duty(FTM0 , CH3,SpeedPWM_Value);                     //FTM0 CH3 PWM输出 ；电机控制  

      // PTA28_OUT = ~PTA28_OUT ;
    }
    
    if(DMA_Over_Flg==1)
    {
      DMA_Over_Flg=0;
      if(ALineOverCout < LinADCout)
      {
        Camera_Black(&ADdata[ALineOverCout][0],&Rightblackedge[ALineOverCout],
                  &Leftblackedge[ALineOverCout],&CameraStation[ALineOverCout]);
        ALineOverCout ++ ;
      }
      
    }
    
    if((frameflag)&&(ALineOverCout == DATALINE))
    {
      frameflag = 0;
      ALineOverCout = 0 ;
      for(int i=0;i<DATALINE;i++)
      {
        Sum_Station+= CameraStation[i];
      }       
      Station=(int8)(Sum_Station/DATALINE);
      Sum_Station=0;
      TurnPWM_Value = TurnPWM(Station, MIDSTATION) ;               //根据CCD位置值计算转向PWM值函数
      //TurnPWM_Value = MIDSTRING2 ;
      FTM_CnV_REG(FTMx[FTM1], CH1) = TurnPWM_Value ;               //转向赋值   舵机2   
      
      COMS_print();   //串口发数及显示
    }
      
    if(TIME0flag_100ms)   //LED指示灯闪烁提示程序在运行
    {
      TIME0flag_100ms=0;
    }
  }
}