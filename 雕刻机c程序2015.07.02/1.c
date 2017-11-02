/*z地址定义
50 1=x+，2=x-，3=y+,4=y-
51 前进后退步数高
52 前进后退步数低
53 
54/55 字宽 
56 
57 弱光开关
58/59 激光强度
60 x轴速度
61 y轴速度
62 开始打印0,57
63 暂停
64 停止标志
65
66  左右标记 
100开始时灰度图数据
*/

#include <reg52.h>
#define uint unsigned int 
#define uchar unsigned char 
#define N z[60]	   //X速度
#define M z[61]	   //Y速度
sbit a=P1^0;//步进电机接线定义
sbit a_=P1^1;
sbit b=P1^2;
sbit b_=P1^3;	   //纵向电机驱动

sbit xa=P2^0;
sbit xa_=P2^1;	
sbit xb=P2^2;
sbit xb_=P2^3;   //横向电机驱动

sbit jg=P3^7;	//激光接线
sbit led=P3^6;//指示灯

uchar xdata z[500]={0};//缓存
uchar buff[3];//串口缓存
uchar x1,x0,y1,y0,cont2=0;
uchar xfb=4,yfb=4;//走步标志位


unsigned char HighRH = 0;  //高电平重载值的高字节
unsigned char HighRL = 0;  //高电平重载值的低字节
unsigned char LowRH  = 0;  //低电平重载值的高字节
unsigned char LowRL  = 0;  //低电平重载值的低字节



void delayms(uint xms)				
{   
	uint i,j;
	for(i=xms;i>0;i--)		      //i=xms即延时约xms毫秒
		for(j=110;j>0;j--);
}

/* 配置并启动PWM，fr-频率，dc-占空比 */
void ConfigPWM(unsigned int fr, unsigned char dc)
{
    unsigned int  high, low;
    unsigned long tmp;
    
    tmp  = (11059200/12) / fr;  //计算一个周期所需的计数值
    high = (tmp*dc) / 100;      //计算高电平所需的计数值
    low  = tmp - high;          //计算低电平所需的计数值
    high = 65536 - high + 12;   //计算高电平的重载值并补偿中断延时
    low  = 65536 - low  + 12;   //计算低电平的重载值并补偿中断延时
    HighRH = (unsigned char)(high>>8); //高电平重载值拆分为高低字节
    HighRL = (unsigned char)high;
    LowRH  = (unsigned char)(low>>8);  //低电平重载值拆分为高低字节
    LowRL  = (unsigned char)low;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = HighRH;   //加载T0重载值
    TL0 = HighRL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
    jg = 0;     //输出低电平，关闭激光
}
/* 关闭PWM */
void ClosePWM()
{
    TR0 = 0;     //停止定时器0
    ET0 = 0;     //禁止定时器0中断
    jg = 0;  //输出低电平，关闭激光
}
/* T0中断服务函数，产生PWM输出 */
void InterruptTimer0() interrupt 1
{
    if (jg == 0)  //当前输出为低电平时，装载高电平值并输出高电平
    {
        TH0 = LowRH;
        TL0 = LowRL;
        jg = 1;
    }
    else              //当前输出为高电平时，装载低电平值并输出低电平
    {
        TH0 = HighRH;
        TL0 = HighRL;
        jg = 0;
    }
}



void xfor(uint i)		//x轴前进函数，前进多少步
{   
  while(1) 
  {
		if(xfb==4)
		  {
         xa=xb=1;
	       xb_=xa_=0;
		     xfb=1;
		     i--;			
	       delayms(N);
		     if(i==0){xa=xb=0; break;}
		  }
		   if(xfb==1)
		  {
         xb=xa_=1;
	       xa=xb_=0;
		     xfb=2;
		     i--;	
	       delayms(N);
		     if(i==0){xa_=xb=0; break;}
		  }
		   if(xfb==2)
		  {
         xa_=xb_=1;
         xb=xa=0;
		     xfb=3;	//走步标志位
		     i--;
	       delayms(N);
		     if(i==0){xa_=xb_=0; break;}
		   }
		   if(xfb==3)
		  {
         xa_=xb=0;
	       xb_=xa=1;
		     xfb=4;
		     i--;	
	       delayms(N);
		     if(i==0){xa=xb_=0; break;}
		  }


		}

}


void xbac(uint i)		//xxx后退函数
{   
     while(1) 
         {
		if(xfb==1)
		  {
         xa_=xb=0;
	       xb_=xa=1;
		     xfb=4;
		     i--;				//走步标志位
	       delayms(N);
		     if(i==0){xa=xb_=0; break;}
		   }
		   if(xfb==4)
		  {
         xa_=xb_=1;
         xb=xa=0;
		     xfb=3;
		     i--;	
	       delayms(N);
		     if(i==0){xa_=xb_=0; break;}
		   }
		   if(xfb==3)
		  {
         xb=xa_=1;
	       xa=xb_=0;
		     xfb=2;	//走步标志位
		     i--;	
	       delayms(N);
		     if(i==0){xa_=xb=0; break;}
		   }
		   if(xfb==2)
		  {
         xa=xb=1;
	       xb_=xa_=0;
		     xfb=1;
		     i--;	
	       delayms(N);
		     if(i==0){xa=xb=0; break;}
		  }
		}
}



void yfor(uint i)		//y轴前进函数
{   
     while(1) 
     {
	       switch(yfb)
		   {
		     case 4:{a=b=1; b_=a_=0; yfb=1; i--; delayms(M); if(i==0){a=b=0;break;}}
				 case 1:{b=a_=1; a=b_=0; yfb=2; i--; delayms(M); if(i==0){a_=b=0;break;}}
				 case 2:{a_=b_=1; b=a=0; yfb=3; i--; delayms(M); if(i==0){a_=b_=0;break;}}
				 case 3:{b_=a=1; a_=b=0; yfb=4; i--; delayms(M); if(i==0){a=b_=0;break;}}
		   }
		   if(i==0)  break;
      }
}

void ybac(uint i)		//yy后退函数
{   
     while(1) 
     {
	       switch(yfb)
		   {
		     case 1:{a=b_=1; b=a_=0; yfb=4; i--; delayms(M); if(i==0){a=b_=0;break;}}
				 case 4:{b_=a_=1; a=b=0; yfb=3; i--; delayms(M); if(i==0){a_=b_=0;break;}}
				 case 3:{a_=b=1; b_=a=0; yfb=2; i--; delayms(M); if(i==0){a_=b=0;break;}}
				 case 2:{b=a=1; a_=b_=0; yfb=1; i--; delayms(M); if(i==0){a=b=0;break;}}
		   }
		   if(i==0)  break;
      }
}

void dazi(uint zik)//打印函数		   ******打印函数已更改******
{
    uint x;
		jg=0;
 
			for(x=0;x<zik;x++)//执行zik个循环，x轴右移zik步
			{
			  while(z[63]);//暂停等待
				if(z[64]==1) break;//停止标志 跳出循环
				jg=1;//开激光
				delayms((z[99+x]*(z[58]*256+z[59]))/100);
			    jg=0;  //关闭激光
			if(z[66]==1)
			{
			xbac(1);
			}	
             else
			 {
			 xfor(1);
			 }			 
			}	  
		if(z[64==1])    z[64]=0;
	    else{yfor(1);} //y轴进一行
    	z[62]=0;//一行打印完成
		SBUF=1;//发送信息，表示打印完成 
		
}

 /* 串口配置函数，baud-通信波特率 */
void ConfigUART(unsigned int baud)
{
    SCON  = 0x50;  //配置串口为模式1
    TMOD &= 0x0F;  //清零T1的控制位
    TMOD |= 0x20;  //配置T1为模式2
    TH1 = 256 - (11059200/12/32)/baud;  //计算T1重载值
    TL1 = TH1;     //初值等于重载值
    ET1 = 0;       //禁止T1中断
    ES  = 1;       //使能串口中断
    TR1 = 1;       //启动T1
}


void chuanlo() interrupt 4
{
  if(RI)
  {
     buff[cont2]=SBUF;//每次3字节，地址高，地址低，数据，，
    
	   cont2++;
     if(cont2==3)//每收3个字节，把数据写入地址中
	   {
	    z[(buff[0]*256)+buff[1]]=buff[2];
		  cont2=0;
		 SBUF=0;  //*****************在这里加入这行代码测试一下*******************************
	   }
	 RI=0;


    
  }
  if(TI)
   {
   TI=0;

   }
}

main()
{  
   EA=1;
   P0=0xff;
   ConfigUART(9600)	;
	z[60]=15;//默认的参数
	z[61]=15;
	z[56]=1;
	z[62]=0;
	jg=0;
	cont2=0;
	
   while(1)
   {
   														   //	*****此处已更改******

    if(z[57]==100){ConfigPWM(100, 98);delayms(10);}	 //上位机指令处理，打开弱光定位，频率100Hz，占空比98%  	  							  
    else if(z[57]==1)   jg=1; 						  //打开强光
	else   { ClosePWM(); delayms(10);}
	 
	  if(cont2!=0) led=0;  else led=1;//指示通信是否可用
      if(z[50]==1){xfor(z[51]*256+z[52]);z[50]=0;}//x+
	  if(z[50]==2){xbac(z[51]*256+z[52]);z[50]=0;}//x-
	  if(z[50]==3){yfor(z[51]*256+z[52]);z[50]=0;}//y+
	  if(z[50]==4){ybac(z[51]*256+z[52]);z[50]=0;}//y-

	  
      if(z[62])//开始打印标志
	  {
	    dazi(z[54]*256+z[55]);
	  }
   
  }

}