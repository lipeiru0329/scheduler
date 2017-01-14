/************************************************************
***********************************************************/
#include <stc89c51.h>
#include <stdio.h>

#define  _SS   static char lc=0; switch(lc){   case 0: lc=0;
#define  _EE   }; lc=0;
#define  WaitX(a,b)  settimer(&lc,__LINE__,a,b); return ; case __LINE__:

struct TASK {
  char td;
  void (*fp)();
};

#define MAXTASKS 10
struct TASK tasks[MAXTASKS];

//���ö�ʱ��
void settimer(char *lc,char  line,char  tmrid,int d){
  *lc=line;
  tasks[tmrid].td=d;
}

//�߼���ʱ�������ڶ�ʱ���ж������
void dectimers() {
    unsigned char i;
    for (i=0;i<maxtasks;i++){
       if (tasks.td>0)  tasks.td--;
    }
}

//������Ⱥ�������main��������
void runtasks() {
   unsigned char i;
   for(i=0;i<maxtasks;i++)
   {
     if (tasks.fp!=0){
           if (tasks.td==0){
             tasks.td=-1;
             tasks.fp();
                }
         }
        }
}

/****************СС���������ֽ���*******************************************************/


sbit KEY = P3^2;
unsigned char code numtab[16]={0x24,0x6F,0xE0,0x62,0x2B,0x32,0x30,0x67,0x20,0x22,0x21,0x38,0xB4,0x68,0xB0,0xB1};


sfr IAP_CONTR = 0xC7;
sfr WDT_CONTR = 0xC1;

//������Ź�
void clr_wdt()
{
    WDT_CONTR =0x3C;
}

//��ʼ����ʱ��
void InitT0()
{
        TMOD = 0x21;
        IE |= 0x82;  // 12t
        TL0=0Xff;
        TH0=0Xb7;
        /*
        TL0=0X3c; //100ms ��װ     Detail: http://www.21ic.com/jichuzhishi/mcu/timer/2014-05-05/345616.html
        TH0=0XB0;
        */
        TR0 = 1;
}
//��ʱ���ж�
void INTT0(void) interrupt 1 using 1
{
        TL0=0Xff;    //10ms ��װ
        TH0=0Xb7;
        /*
        TL0=0X3c; //100ms ��װ     Detail: http://www.21ic.com/jichuzhishi/mcu/timer/2014-05-05/345616.html
        TH0=0XB0;
        */
        dectimers();
}

sbit LED1= P2^4;

//����һ��״̬��д��
void ontimer0(){

    //function

  //��װ��ʱ��
  if ( /*     */  ) tasks[0].td=45;  //����450ms
  else tasks[0].td=5;  //����50ms
}

//�������״̬��д��
char keycount=0;
void task1(){
    if(KEY==0) {
       keycount++;
       if (keycount>20) //function;  //����1s�������ߡ�������
    }
    else{
        keycount=0;
    }
    //��װ��ʱ��
    tasks[1].td=5;  // ÿ50ms���
}


//��������α�߳�д��
void  task2()
{
    static char i;
    _SS

    while(1){

        for(i=0;i<=9;i++){   //��0--9������ʾ�����200mS
           WaitX(2,20);         //    �ȴ�200mS
           //function
        }
        for(i=0;i<=9;i++){ //��0--9������ʾ�����500mS
           WaitX(2,50);       //    �ȴ�500mS
           //function
        }
    }

    _EE
}



void main()
{
        unsigned char         KeyNum;
        P3M0 = 0x00;
        P3M1 =0x00;
        //WDT_CONTR= 0x00;   //�رտ��Ź�
        P1 = 0xff;         //����ʾ

        clr_wdt();

        InitT0();

        KEY =1;                                //����IO��
        KeyNum=0;                        //���´���

    //װ������:
        tasks[0].fp=ontimer0;
        tasks[1].fp=task1;
        tasks[2].fp=task2;

    //ѭ������
        while(1){
             runtasks();
             clr_wdt();
        }
}

/*

    Reminder:  Delay 2ms
        for(i=0;i<20;i++){
            delay_us(100);
            waitx(0);
            }

*/
