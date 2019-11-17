/***********************************************************
                  北京航空航天大学
  文件名       ：main.c
  说明         ：电磁整车程序
************************************************************/

#include "common.h"
#include "include.h"

void data_handle();

int32 duoji_PWM=0;
float P1=70,D1=320;

int speed_set=200,right_speed=0,left_speed=0;
int speed_error=0,speed_error_pre=0,speed_error_accum=0;
int SP=23,SD=10;
int speed_PWM=0,speed_PWM_pre=0,Speed_PWM_OUT_times=0;
int Speed_PWM_OUT=0;

int32 dianji_PWM;
uint8 read_times=0;

uint32 time1=0;

int Float_Dir_Mid=7450,Float_Dir_Left=8750,Float_Dir_Right=6250;
int D_error=0,D_error_pre=0,D_error_pre_pre=0,D_error_pre_pre_pre=0,D_error_pre_pre_pre_pre=0;
float D_P=8,D_D=20;
int dir_PWM=0;
int adc_1_ADresult=0,adc_2_ADresult=0,adc_3_ADresult=0;

void PIT1_IRQHandler(void);
void data_init(void);
uint8 get_mid(uint8 a,uint8 b,uint8 c);

void ysz_delay(unsigned int ms);
void PWM_OUT();

void speed_PID();
void speed_PWM_OUT(void);

void steering_control(void);


/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       FTM PWM 测试
 */ 
void main(void)
{
  DisableInterrupts;
  LCD_Init();//液晶初始化
  gpio_init (PTB3, GPI,1);//采样按键    
  adc_init(ADC1_SE5a); //左 E1
  adc_init(ADC0_SE8); //右 B0   
  //adc_init(ADC0_SE9); //中 B1
  
  LCD_Init();
  FTM_PWM_init(FTM0,FTM_CH0,10000,0);//电机PWM初始化
  FTM_PWM_init(FTM0,FTM_CH1,10000,0);
  FTM_PWM_init(FTM0,FTM_CH2,10000,0);//电机PWM初始化
  FTM_PWM_init(FTM0,FTM_CH3,10000,0);
  
  FTM_PWM_init(FTM2,FTM_CH1,50,Float_Dir_Mid);//57800    50450  65150
  
  lptmr_pulse_init(LPT0_ALT2,0xFFFF,LPT_Rising);//测速初始化//PC5
  LPTMR0_CNR=0;
  EnableInterrupts; 
  
  while(PTB3_IN==1);//等待触发
  pit_init_ms(PIT1,20);                               //初始化PIT0，定时时间为： 5ms
  set_vector_handler(PIT1_VECTORn ,PIT1_IRQHandler);      //设置PIT0的中断复位函数为 PIT0_IRQHandler  
  
  while(1)
  {      
    PWM_OUT();
  }    
}

void ysz_delay(unsigned int ms)//为防止time_delay_ms();与lpt冲突编写的延时
{
  unsigned int j1,k_1;
  int i1;
  i1=ms;
  for(j1=0;j1<i1;j1++)   
    for(k_1=0;k_1<14120;k_1++);
}



void PIT1_IRQHandler(void)//4MS定时中断
{     
  time1++;
  adc_1_ADresult=ad_ave(ADC1_SE5a,ADC_8bit,20);//左 PTE1
  adc_2_ADresult=ad_ave(ADC0_SE8,ADC_8bit,20);//右 PTB0
  //adc_3_ADresult=ad_ave(ADC0_SE9,ADC_8bit,20);// PTB1
  
  steering_control();
  left_speed=LPTMR0_CNR;
  LPTMR0_CNR=0;
  lptmr_pulse_init(LPT0_ALT2,0xFFFF,LPT_Rising);//PC5
  LCD_BL(8,2,adc_1_ADresult);// PTE1
  //LCD_BL(56,2,adc_3_ADresult);//PTB1 
  LCD_BL(100,2,adc_2_ADresult);//PTB0
  LCD_BL(56,4,left_speed);//速度
  
  speed_PID();
  PWM_OUT();  
    
  PIT_Flag_Clear(PIT1);
  
}

uint8 get_mid(uint8 a,uint8 b,uint8 c)
{
  uint8 x;
  if(a>b) {x=b;b=a;a=x;}
  if(b>c) {x=c;c=b;b=x;}
  if(a>b) {x=b;b=a;a=x;}
  return b;
}

int error_jifen=0,error_jifenxsi;

void speed_PID()
{ 
  speed_error=speed_set-left_speed;//(right_speed+left_speed)/
  /*error_jifen=+speed_error;
  error_jifenxsi=SI*error_jifen;
  if(error_jifenxsi>=20000)
  error_jifenxsi=20000;
  if(error_jifenxsi<=-20000)
  error_jifenxsi=-20000;*/
  speed_PWM = SP * speed_error+SD * (speed_error - speed_error_pre);   
  speed_error_pre=speed_error;

  if(speed_PWM<-8000)
  speed_PWM=-8000;
  else if(speed_PWM>8000)
  speed_PWM=8000;
}

void PWM_OUT()
{ 
  if(speed_PWM>=0)
  {
    FTM_PWM_Duty(FTM0,FTM_CH0,0);
    FTM_PWM_Duty(FTM0,FTM_CH1,speed_PWM);//左反
    FTM_PWM_Duty(FTM0,FTM_CH2,0);
    FTM_PWM_Duty(FTM0,FTM_CH3,speed_PWM);//左反
  }
  else if(speed_PWM<0)
  {
    FTM_PWM_Duty(FTM0,FTM_CH0,-speed_PWM);
    FTM_PWM_Duty(FTM0,FTM_CH1,0);//左反
    FTM_PWM_Duty(FTM0,FTM_CH2,-speed_PWM);
    FTM_PWM_Duty(FTM0,FTM_CH3,0);//左反
  }
}

void steering_control(void)//舵机控制
{  
  D_error=((float)(adc_1_ADresult-adc_2_ADresult)/(float)(adc_2_ADresult+adc_1_ADresult))*100;
  if(D_error<30&&D_error>-30)
  D_P=1;   
  else 
  D_P=8; 
  dir_PWM=D_P*D_error+D_D*(D_error-D_error_pre);
  D_error_pre=D_error;

  dir_PWM=Float_Dir_Mid+dir_PWM;

  if(dir_PWM<=Float_Dir_Right)
    dir_PWM=Float_Dir_Right;
  else if(dir_PWM>=Float_Dir_Left)
    dir_PWM=Float_Dir_Left;
  FTM_PWM_Duty(FTM2,FTM_CH1,dir_PWM);
}