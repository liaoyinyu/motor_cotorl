#ifndef BUJINDIANJI_H
#define BUJINDIANJI_H
#include <stdlib.h>
#include <conio.h>

typedef unsigned int uint;
typedef unsigned char uchar;
#define Start (outportb(0x640,0x00))            //启动AD
#define Read (inportb(0x640))                   //读AD采样结果

static uchar btab1[] = {0x10,0x30,0x20,0x60,0x40,0xc0,0x80,0x90 };  //逆时针转
static uchar btab[]  = {0x90,0x80,0xc0,0x40,0x60,0x20,0x30,0x10 }; //顺时针转

void dis(void);             //根据set_mode 数值显示 数码管
void ccscan(void);          //检测是否有按键按下
void clear(void);           //清屏
void putbuf(void);          //根据按键值情况进行相应的处理
void getkey(void);          //获取按键值
void delay(int time);       //延时
void dianjizhuandong(void);    //步进电机转动
void keyget(void);
void ADget(void);

int MY8255_A = 0x0600;      //8255地址
int MY8255_B = 0x0602;
int MY8255_C = 0x0604;
int MY8255_MODE = 0x0606;

char a[]={0x30,0x60,0x5b,0x4f,0x66,0x6d,0x7d,0x07,  //数码管显示数据
         0x7f,0x6f,0x77,0x39,0x5e,0x79,0x71};

char b[]={0x00,0x00,0x00,0x00,0x00,0x00};           //mov buf
char b1[]={0x00,0x08,0x01,0x00,0x00,0x000};         //set buf ，初始值为180
int cc;             //有按键按下为1
int n;              //按键值
int set_mode=0;     //设置模式标志
int position=0;     //步进电机的位置
int angle_mov=0;    //步进电机转动角度
int angle_max=180;    //最大角度值
int ADval=0;      //ad转换的值
int step_a=0;
int step_b=0;
int step_c=0;
uchar step_flag=0;


void getkey(void)
{
    int i=0;
    int j=0xfe;
    for(i=0;i<=3;i++)
    {
        outp(MY8255_A,j);
        if(!((inp(MY8255_C))&0x01))
        {
            n=i+0;
            putbuf();
            return;
        }
        if(!((inp(MY8255_C))&0x02))
        {
            n=i+4;
            putbuf();
            return;
        }
        if(!((inp(MY8255_C))&0x04))
        {
            n=i+8;
            putbuf();
            return;
        }
        if(!((inp(MY8255_C))&0x08))
        {
            n=i+12;
            putbuf();
            return;
        }
        j<<=1;

    }
}

void ccscan(void)
{
    outp(MY8255_A,0x00);
    cc=inp(MY8255_C);
    cc=(~cc) & 0x0f;
}

void dis()
{
    int i;
    int j=0xdf;
    if(set_mode==0)
    {
        for(i=0;i<=3;i++)
        {
            outp(MY8255_A,0xff);
            outp(MY8255_B,a[b[i]]);
            outp(MY8255_A,j);
            delay(0x05);
            j=(j>>1)| (j<<7);
        }
    }
    else
    {
        for(i=0;i<=3;i++)
        {
            outp(MY8255_A,0xff);
            outp(MY8255_B,a[b1[i]]);
            outp(MY8255_A,j);
            delay(0x05);
            j=(j>>1)| (j<<7);
        }
    }
}

void clear(void)
{
    outp(MY8255_B,0x00);
}

void putbuf()
{
    uchar i,j;
    if((n==0) & (set_mode==1))              //按set键后，使用键盘设置最大角度
    {
        b1[0]++;                            //按一次相应位加一
        if(b1[0]>9) b1[0]=0;
    }
    if((n==1) & (set_mode==1))
    {
        b1[1]++;
        if(b1[1]>9) b1[1]=0;
    }
    if((n==6) & (set_mode==1))
    {
        b1[2]++;
        if(b1[2]>9) b1[2]=0;
    }
    if((n==7) & (set_mode==1))
    {
        b1[3]++;
        if(b1[3]>9) b1[3]=0;
    }

    if(n==2)                                //按set键
    {
        set_mode=1;
    }

    if(n==5)                                //按start键载入
    {
        angle_max=b1[0]+b1[1]*10+b1[2]*100+b1[3]*1000;          //
        set_mode=0;
        delay(0x05);
    }

    if((n==4)&(set_mode==1))                //按clear键清零
    {
        clear();
        b1[0]=0;
        b1[1]=0;
        b1[2]=0;
        b1[3]=0;
    }
    dis();
    clear();
    ccscan();
    while(cc)
    {
        dis();
        clear();
        ccscan();
    }

}

void dianjizhuandong()
{
   int i,j;
   step_c=ADval/15;                         //target position

    if(position<step_c)                      //有差值才转，正差正转，反差反转
    {
        angle_mov=step_c-position; 
        if(step_flag)
            step_a=4;
        else step_a=0;
        for(j=1;j<=angle_mov;j++)
        {
            for(i=0;i<4;i++)
            {
                outportb(MY8255_C,btab1[step_a]); //正转,顺时针
                step_a++;
                if(step_a>=8)   step_a=0;
                dis();
                delay(0x0fa);
             }
            step_flag=!step_flag;
        }
        position=step_c;

    }
    else if(position>step_c)
    {
        angle_mov=position-step_c;
        if(step_flag)
            step_b=4;
        else step_b=0;
        for(j=0;j<=angle_mov;j++)
        {
            for(i=0;i<4;i++)
            {
                outportb(MY8255_C,btab[step_b]); //反转，逆时针
                step_b++;
                if(step_b>=8)   step_b=0;
                dis();
                delay(0x0fa);
             }
            step_flag=!step_flag;
        }
        position=step_c;
    }
}

void delay(int time)
{
    int i;
    int j;
    for(i=0;i<=time;i++)
    {
        for(j=0;j<=0x100;j++)
        {

        }
    }
    return;
}

void keyget(void)
{
    dis();
    clear();
    ccscan();
    if(cc)
    {
        dis();
        delay(0x05);
        clear();
        ccscan();
        if(cc)
        {
            getkey();
        }
    }
}

void ADget(void)
{
    Start;
    delay(0x05);
    ADval=Read;                             //转动电位器，读取电位器的数值，
    Adval=(ADval*45)>>5;    //cal angle
    if(ADval>=angle_max)
    {
        ADval=angle_max;                    //需要不大于最大角度值下调整
    }
    b[0]=ADval%10;                          //在数码管上显示电位器转动的数值
    b[1]=ADval/10%10;
    b[2]=ADval/100%10;
    b[3]=ADval/1000%10;
}

#endif // BUJINDIANJI_H
