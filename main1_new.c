#include <stc89c51.h>
/****小小调度器开始**********************************************/
#define MAXTASKS 3
volatile unsigned char timers[MAXTASKS];

#define _SS static unsigned char _lc=0; switch(_lc){default:
#define _EE ;}; _lc=0; return 255;
#define WaitX(tickets) do {_lc=(__LINE__%255)+1; return tickets ;} while(0); case (__LINE__%255)+1:
#define RunTask(TaskName,TaskID) do { if (timers[TaskID]==0) timers[TaskID]=TaskName(); } while(0);

/*
1. 首先 ProtoThread 实现的多任务是“协作式”多任务，也即多个任务间通过“友好合作”的方法共享 CPU 资源，当一个任务不需要 CPU 时，主动让出 CPU。
2. ProtoThread 任务调用 WaitX 主动让出 CPU 时，作了两件事：记录断点（代码当前行记录到_lc 变量里），让出 CPU（return 实现）
3. 任务的恢复运行，当一个任务等待的条件发生时，恢复运行，这时应该要恢复到原来的断点处，这通过_SS 宏里的 switch(_lc)和 WaitX 宏里的 case
__LINE__+((__LINE__%256)==0): 实现。
2. 所以，恢复运行的本质是通过 switch 找到原来有 WaitX 留下来的断点。
4. 至于 RunTask 宏是加了定时服务，当任务定时时间到后，去运行任务函数，也即充当了调度器的角色。
*/

#define RunTaskA(TaskName,TaskID) { if (timers[TaskID]==0) {timers[TaskID]=TaskName(); continue;} } //前面的任务优先保证执行
#define CallSub(SubTaskName) do {unsigned char currdt; _lc=(__LINE__%255)+1; return 0; case (__LINE__%255)+1: currdt=SubTaskName();if(currdt!=255) return currdt;} while(0); //和上面是同一行，因为一行显示不下了，里面有 return 0，在执行子函数前释放了一下 CPU
#define InitTasks() {unsigned char i; for(i=MAXTASKS;i>0 ;i--) timers[i-1]=0; }
#define UpdateTimers() {unsigned char i; for(i=MAXTASKS;i>0 ;i--){if((timers[i-1]!=0)&&(timers[i-1]!=255)) timers[i-1]--;}}
#define SEM unsigned int
//初始化信号量
#define InitSem(sem) sem=0;
//等待信号量
#define WaitSem(sem) do{ sem=1; WaitX(0); if (sem>0) return 1;} while(0); //里面有 WaitX(0)，执行的时候释放了一下 CPU
//等待信号量或定时器溢出， 定时器 tickets 最大为 0xFFFE
#define WaitSemX(sem,tickets) do { sem=tickets+1; WaitX(0); if(sem>1){ sem--; return 1;} } while(0); //里面有 WaitX(0)，执行的时候释放了一下 CPU
//发送信号量
#define SendSem(sem) do {sem=0;} while(0);
/*****小小调度器结束**********************************************************************/
/*****下面是示例程序**********************************************************************/
sbit LED1 = P2^1;
sbit LED2 = P2^2;
sbit LED0 = P2^5;
unsigned char task0(){
    static unsigned char _lc=0;
    switch(_lc)
    {
        default:
            while(1){
                do {
                        _lc=(__LINE__%255)+1;
                        return 50 ;
                    }
                while(0);
                case (__LINE__%255)+1:;
            LED0=!LED0;
            };
    };
    _lc=0;
    return 255;
}

unsigned char task1(){
    _SS
    while(1){
        WaitX(100);     //wait 1s
        //Function1
    }
    _EE
}
unsigned char task2(){
    _SS
    while(1){
        WaitX(100);     //wait 1s
        //Function2
    }
    _EE
}

void InitT0()
{
    TMOD = 0x21;
    IE |= 0x82; // 12t
    TL0=0Xff;
    TH0=0XDB;
    TR0 = 1;
}

void INTT0(void) interrupt 1 using 1
{
    TL0=0Xff; //10ms 重装
    TH0=0XDB;//b7;
    /*
    TL0=0X3c; //100ms 重装     Detail: http://www.21ic.com/jichuzhishi/mcu/timer/2014-05-05/345616.html
    TH0=0XB0;
    */
    UpdateTimers();
    RunTask(task0,0);//任务 0 具有精确按时获得执行的权限，要求：task0 每次执行消耗时间<0.5 个 ticket
}

void main()
{
    InitT0();
    InitTasks(); //初始化任务，实际上是给 timers 清零
    while(1){
        // RunTask(task0,0);
        RunTaskA(task1,1);//任务 1 具有比任务 2 高的运行权限   没有WAITX(0)
        RunTaskA(task2,2);//任务 2 具有低的运行权限
    }
}

//定义： 收到task2(1s)的信号后， 走task1
#define SEM unsigned int；信号量的类型是 int，可以定义 65536 个，是个全局变量。
SEM sm1;
void task1(){
    _SS
    InitSem(sm1);
    while(1){
        WaiSem(sm1);
        LED1=!LED1; }
    _EE
}
void task2(){
    _SS
    while(1){
        WaitX(100);
        SendSem(sm1);
        }
    _EE
}

/* Calll SubFunction */
unsigned char task11(){
static int i;
    _SS
    for(i=0;i<11;i++){
        WaitX(10U);
        LED2=!LED2;
    }
    _EE
}

unsigned char task1(){
    _SS
    while(1){
        WaitX(100U);
        LED0=!LED0;
        CallSub(task11);
    }
    _EE
}
