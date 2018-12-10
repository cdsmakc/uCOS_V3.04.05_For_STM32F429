/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                       IAR Development Kits
*                                              on the
*
*                                    STM32F429II-SK KICKSTART KIT
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : YS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <includes.h>
#include "sys_cfg.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  APP_TASK_EQ_0_ITERATION_NBR              16u


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

/* ----------------- APPLICATION GLOBALS -------------- */
static  OS_TCB   Thread1TaskTCB ;
static  CPU_STK  Thread1TaskStk[APP_CFG_THREAD1_TASK_STK_SIZE] ;

static  OS_TCB   Thread2TaskTCB ;
static  CPU_STK  Thread2TaskStk[APP_CFG_THREAD2_TASK_STK_SIZE] ;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppThread1Task          (void     *p_arg);
static  void  AppThread2Task          (void     *p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int main(void)
{
    OS_ERR  err;

    /* ��ֹ�����ж� */
    CPU_IntDis() ;
    
    /* ϵͳ���� */
    SYS_SystemConfig() ;

    /* CPU��ʼ�� */
    CPU_Init() ;

    /* �ڴ����Ԫ��ʼ�� */
    Mem_Init() ;

    /* ��ѧ���㵥Ԫ��ʼ�� */
    Math_Init() ;                                                

    /* uC/OS-III����ϵͳ��ʼ�� */
    OSInit(&err) ; 

    /* SysTick��ʱ����ʼ�� */
    SYS_SystemTickInit() ;

    OSTaskCreate((OS_TCB       *)&Thread1TaskTCB,
                 (CPU_CHAR     *)"App Task Thread 1",
                 (OS_TASK_PTR   )AppThread1Task,
                 (void         *)0u,
                 (OS_PRIO       )APP_CFG_THREAD1_TASK_PRIO,
                 (CPU_STK      *)&Thread1TaskStk[0u],
                 (CPU_STK_SIZE  )Thread1TaskStk[APP_CFG_THREAD1_TASK_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_THREAD1_TASK_STK_SIZE,
                 (OS_MSG_QTY    )0u,
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    OSTaskCreate((OS_TCB       *)&Thread2TaskTCB,
                 (CPU_CHAR     *)"App Task Thread 2",
                 (OS_TASK_PTR   )AppThread2Task,
                 (void         *)0u,
                 (OS_PRIO       )APP_CFG_THREAD2_TASK_PRIO,
                 (CPU_STK      *)&Thread2TaskStk[0u],
                 (CPU_STK_SIZE  )Thread2TaskStk[APP_CFG_THREAD2_TASK_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_THREAD2_TASK_STK_SIZE,
                 (OS_MSG_QTY    )0u,
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

    (void)&err;

    return (0u);
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppThread1Task (void *p_arg __unused)
{
    OS_ERR  err;

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif


    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */

        printf("This is thread 1. Avg usage is %f%%. Max usage is %f%%.\n",
               (FLOAT)(OSStatTaskCPUUsage/100.0),
               (FLOAT)(OSStatTaskCPUUsageMax/100.0)) ;

        GPIO_SetBits(GPIOH, GPIO_Pin_10) ;
			
        OSTimeDlyHMSM(0u, 0u, 0u, 500u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);

        GPIO_ResetBits(GPIOH, GPIO_Pin_10) ;
            
        OSTimeDlyHMSM(0u, 0u, 0u, 500u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
    }
}

static  void  AppThread2Task (void *p_arg __unused)
{
    OS_ERR  err;

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif


    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */

        printf("This is thread 2. Avg usage is %f%%. Max usage is %f%%.\n",
               (FLOAT)(OSStatTaskCPUUsage/100.0),
               (FLOAT)(OSStatTaskCPUUsageMax/100.0)) ;

        GPIO_SetBits(GPIOH, GPIO_Pin_11) ;
			
        OSTimeDlyHMSM(0u, 0u, 0u, 250u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);

        GPIO_ResetBits(GPIOH, GPIO_Pin_11) ;
            
        OSTimeDlyHMSM(0u, 0u, 0u, 250u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
    }
}



