/*z��ַ����
50 1=x+��2=x-��3=y+,4=y-
51 ǰ�����˲�����
52 ǰ�����˲�����
53 
54/55 �ֿ� 
56 
57 ���⿪��
58/59 ����ǿ��
60 x���ٶ�
61 y���ٶ�
62 ��ʼ��ӡ0,57
63 ��ͣ
64 ֹͣ��־
65
66  ���ұ�� 
100��ʼʱ�Ҷ�ͼ����
*/

#include <reg52.h>
#define uint unsigned int 
#define uchar unsigned char 
#define N z[60]	   //X�ٶ�
#define M z[61]	   //Y�ٶ�
sbit a=P1^0;//����������߶���
sbit a_=P1^1;
sbit b=P1^2;
sbit b_=P1^3;	   //����������

sbit xa=P2^0;
sbit xa_=P2^1;	
sbit xb=P2^2;
sbit xb_=P2^3;   //����������

sbit jg=P3^7;	//�������
sbit led=P3^6;//ָʾ��

uchar xdata z[500]={0};//����
uchar buff[3];//���ڻ���
uchar x1,x0,y1,y0,cont2=0;
uchar xfb=4,yfb=4;//�߲���־λ


unsigned char HighRH = 0;  //�ߵ�ƽ����ֵ�ĸ��ֽ�
unsigned char HighRL = 0;  //�ߵ�ƽ����ֵ�ĵ��ֽ�
unsigned char LowRH  = 0;  //�͵�ƽ����ֵ�ĸ��ֽ�
unsigned char LowRL  = 0;  //�͵�ƽ����ֵ�ĵ��ֽ�



void delayms(uint xms)				
{   
	uint i,j;
	for(i=xms;i>0;i--)		      //i=xms����ʱԼxms����
		for(j=110;j>0;j--);
}

/* ���ò�����PWM��fr-Ƶ�ʣ�dc-ռ�ձ� */
void ConfigPWM(unsigned int fr, unsigned char dc)
{
    unsigned int  high, low;
    unsigned long tmp;
    
    tmp  = (11059200/12) / fr;  //����һ����������ļ���ֵ
    high = (tmp*dc) / 100;      //����ߵ�ƽ����ļ���ֵ
    low  = tmp - high;          //����͵�ƽ����ļ���ֵ
    high = 65536 - high + 12;   //����ߵ�ƽ������ֵ�������ж���ʱ
    low  = 65536 - low  + 12;   //����͵�ƽ������ֵ�������ж���ʱ
    HighRH = (unsigned char)(high>>8); //�ߵ�ƽ����ֵ���Ϊ�ߵ��ֽ�
    HighRL = (unsigned char)high;
    LowRH  = (unsigned char)(low>>8);  //�͵�ƽ����ֵ���Ϊ�ߵ��ֽ�
    LowRL  = (unsigned char)low;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = HighRH;   //����T0����ֵ
    TL0 = HighRL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
    jg = 0;     //����͵�ƽ���رռ���
}
/* �ر�PWM */
void ClosePWM()
{
    TR0 = 0;     //ֹͣ��ʱ��0
    ET0 = 0;     //��ֹ��ʱ��0�ж�
    jg = 0;  //����͵�ƽ���رռ���
}
/* T0�жϷ�����������PWM��� */
void InterruptTimer0() interrupt 1
{
    if (jg == 0)  //��ǰ���Ϊ�͵�ƽʱ��װ�ظߵ�ƽֵ������ߵ�ƽ
    {
        TH0 = LowRH;
        TL0 = LowRL;
        jg = 1;
    }
    else              //��ǰ���Ϊ�ߵ�ƽʱ��װ�ص͵�ƽֵ������͵�ƽ
    {
        TH0 = HighRH;
        TL0 = HighRL;
        jg = 0;
    }
}



void xfor(uint i)		//x��ǰ��������ǰ�����ٲ�
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
		     xfb=3;	//�߲���־λ
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


void xbac(uint i)		//xxx���˺���
{   
     while(1) 
         {
		if(xfb==1)
		  {
         xa_=xb=0;
	       xb_=xa=1;
		     xfb=4;
		     i--;				//�߲���־λ
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
		     xfb=2;	//�߲���־λ
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



void yfor(uint i)		//y��ǰ������
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

void ybac(uint i)		//yy���˺���
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

void dazi(uint zik)//��ӡ����		   ******��ӡ�����Ѹ���******
{
    uint x;
		jg=0;
 
			for(x=0;x<zik;x++)//ִ��zik��ѭ����x������zik��
			{
			  while(z[63]);//��ͣ�ȴ�
				if(z[64]==1) break;//ֹͣ��־ ����ѭ��
				jg=1;//������
				delayms((z[99+x]*(z[58]*256+z[59]))/100);
			    jg=0;  //�رռ���
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
	    else{yfor(1);} //y���һ��
    	z[62]=0;//һ�д�ӡ���
		SBUF=1;//������Ϣ����ʾ��ӡ��� 
		
}

 /* �������ú�����baud-ͨ�Ų����� */
void ConfigUART(unsigned int baud)
{
    SCON  = 0x50;  //���ô���Ϊģʽ1
    TMOD &= 0x0F;  //����T1�Ŀ���λ
    TMOD |= 0x20;  //����T1Ϊģʽ2
    TH1 = 256 - (11059200/12/32)/baud;  //����T1����ֵ
    TL1 = TH1;     //��ֵ��������ֵ
    ET1 = 0;       //��ֹT1�ж�
    ES  = 1;       //ʹ�ܴ����ж�
    TR1 = 1;       //����T1
}


void chuanlo() interrupt 4
{
  if(RI)
  {
     buff[cont2]=SBUF;//ÿ��3�ֽڣ���ַ�ߣ���ַ�ͣ����ݣ���
    
	   cont2++;
     if(cont2==3)//ÿ��3���ֽڣ�������д���ַ��
	   {
	    z[(buff[0]*256)+buff[1]]=buff[2];
		  cont2=0;
		 SBUF=0;  //*****************������������д������һ��*******************************
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
	z[60]=15;//Ĭ�ϵĲ���
	z[61]=15;
	z[56]=1;
	z[62]=0;
	jg=0;
	cont2=0;
	
   while(1)
   {
   														   //	*****�˴��Ѹ���******

    if(z[57]==100){ConfigPWM(100, 98);delayms(10);}	 //��λ��ָ��������ⶨλ��Ƶ��100Hz��ռ�ձ�98%  	  							  
    else if(z[57]==1)   jg=1; 						  //��ǿ��
	else   { ClosePWM(); delayms(10);}
	 
	  if(cont2!=0) led=0;  else led=1;//ָʾͨ���Ƿ����
      if(z[50]==1){xfor(z[51]*256+z[52]);z[50]=0;}//x+
	  if(z[50]==2){xbac(z[51]*256+z[52]);z[50]=0;}//x-
	  if(z[50]==3){yfor(z[51]*256+z[52]);z[50]=0;}//y+
	  if(z[50]==4){ybac(z[51]*256+z[52]);z[50]=0;}//y-

	  
      if(z[62])//��ʼ��ӡ��־
	  {
	    dazi(z[54]*256+z[55]);
	  }
   
  }

}