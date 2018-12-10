/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : sys_cfg.c
    Project name : 公共可移植模块。
    Module name  : 
    Date created : 2017年8月17日   10时19分43秒
    Author       : Ning.JianLi
    Description  : STM32系统配置接口。
*******************************************************************************/

#include "sys_cfg.h"

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

/* 该函数的定义使得printf函数可以将信息输出在USART端口上 */
INT fputc(INT iChar, FILE *pstFile)
{
    USART_SendData(USART1, (UCHAR)iChar);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    return iChar;
}


VOID __SYS_SystemClockConfig(VOID)
{
    /* 初始化系统时钟 */
    RCC_DeInit() ;
    RCC_HSEConfig(RCC_HSE_ON) ;
    if(ERROR == RCC_WaitForHSEStartUp())
    {
        return ;
    }

    /* 锁相环各个系数设置 */
    RCC_PLLConfig(RCC_PLLSource_HSE, 
                  SYS_PLL_CONFIG_INFO_PLLM ,
                  SYS_PLL_CONFIG_INFO_PLLN ,
                  SYS_PLL_CONFIG_INFO_PLLP ,
                  SYS_PLL_CONFIG_INFO_PLLQ) ;

    /* AHB时钟和SYSCLK一致 */
    RCC_HCLKConfig(RCC_SYSCLK_Div1) ;

    /* APB1总线时钟工作在AHB总线时钟的1/4 */
    RCC_PCLK1Config(RCC_HCLK_Div4) ;

    /* APB1总线时钟工作在AHB总线时钟的1/2 */
    RCC_PCLK2Config(RCC_HCLK_Div2) ;

    /* 启动锁相环 */
    RCC_PLLCmd(ENABLE) ;

    /* 等待锁相环锁定 */
    while(RESET == RCC_GetFlagStatus(RCC_FLAG_PLLRDY)) ;

    /* 对于运行在168MHz的STM32F4，FLASH等待周期应设置为5 */
    FLASH_SetLatency(FLASH_Latency_5) ;

    /* 允许预取 */
    FLASH_PrefetchBufferCmd(ENABLE) ;

    /* 允许指令Cache */
    FLASH_InstructionCacheCmd(ENABLE) ;

    /* 允许数据Cache */
    FLASH_DataCacheCmd(ENABLE) ;

    /* 选择PLL输出作为系统时钟 */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK) ;

    /* 等待系统时钟切换完成 */
    while(0x08 != RCC_GetSYSCLKSource()) ;

    return ;
}

VOID __SYS_SystemAllGPIOConfig(VOID)
{
    GPIO_InitTypeDef stGPIOInitInfo ;

    /* LED GPIO配置 */
    /* LED使用GPIOH组的10、11、12引脚 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE) ;
    
    stGPIOInitInfo.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 ;
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_OUT ;
    stGPIOInitInfo.GPIO_Speed = GPIO_Speed_50MHz ;
    stGPIOInitInfo.GPIO_OType = GPIO_OType_PP ;
    stGPIOInitInfo.GPIO_PuPd  = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOH, &stGPIOInitInfo) ;

    /* 熄灭所有的LED灯 */
    GPIO_SetBits(GPIOH, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12) ;
    
    /* 系统串口CPIO配置 */
    /* 系统串口使用GPIOA组的9、10引脚 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE) ;

    stGPIOInitInfo.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10 ;
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_AF ;
    stGPIOInitInfo.GPIO_Speed = GPIO_Speed_50MHz ;
    stGPIOInitInfo.GPIO_OType = GPIO_OType_PP ;
    stGPIOInitInfo.GPIO_PuPd  = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &stGPIOInitInfo) ;

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1) ;
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1) ;
}

VOID __SYS_SystemSerialPortConfig(VOID)
{
    USART_InitTypeDef stUSARTInitInfo ;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE) ;

    /* 115200-8-1-N-N */
    stUSARTInitInfo.USART_BaudRate            = 115200 ;
    stUSARTInitInfo.USART_WordLength          = USART_WordLength_8b ;
    stUSARTInitInfo.USART_StopBits            = USART_StopBits_1 ;
    stUSARTInitInfo.USART_Parity              = USART_Parity_No ;
    stUSARTInitInfo.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx ;
    stUSARTInitInfo.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;

    USART_Init(USART1, &stUSARTInitInfo) ;
    USART_Cmd(USART1, ENABLE) ;
}


VOID SYS_SystemConfig(VOID)
{
    /* 系统时钟配置 */
    __SYS_SystemClockConfig() ;

    /* GPIO配置 */
    __SYS_SystemAllGPIOConfig() ;

    /* 系统串口配置 */ 
    __SYS_SystemSerialPortConfig() ;

    /* 中断处理函数表初始化 */
    __SYS_SystemInterruptInit() ;

    return ;
}

VOID SYS_SystemTickInit(VOID)
{
    RCC_ClocksTypeDef stRCCClockFreqInfo ;
    UINT              uiCnts ;

    /* 获取系统时钟频率 */
    RCC_GetClocksFreq(&stRCCClockFreqInfo) ;

    uiCnts = stRCCClockFreqInfo.HCLK_Frequency / OSCfg_TickRate_Hz ;

    /* 以计算出的systick运行重装载值来初始化systick */
    OS_CPU_SysTickInit(uiCnts) ;

    return ;
}

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
VOID  CPU_TS_TmrInit(VOID)
{
    RCC_ClocksTypeDef stRCCClockFreqInfo ;
    
    /* 获取系统时钟频率 */
    RCC_GetClocksFreq(&stRCCClockFreqInfo) ;

    SYS_REG_DEM_CR     |= (CPU_INT32U)SYS_BIT_DEM_CR_TRCENA;    /* Enable Cortex-M4's DWT CYCCNT reg.                   */
    SYS_REG_DWT_CYCCNT  = (CPU_INT32U)0u;
    SYS_REG_DWT_CR     |= (CPU_INT32U)SYS_BIT_DWT_CR_CYCCNTENA;
    
    CPU_TS_TmrFreqSet(stRCCClockFreqInfo.HCLK_Frequency) ;

    return ;
}

CPU_TS_TMR  CPU_TS_TmrRd (void)
{
    CPU_TS_TMR  ts_tmr_cnts;

    ts_tmr_cnts = (CPU_TS_TMR)SYS_REG_DWT_CYCCNT;

    return (ts_tmr_cnts);
}

CPU_INT64U  CPU_TS32_to_uSec (CPU_TS32  ts_cnts)
{
    RCC_ClocksTypeDef stRCCClockFreqInfo ;

    RCC_GetClocksFreq(&stRCCClockFreqInfo) ;

    return(ts_cnts / (stRCCClockFreqInfo.HCLK_Frequency / DEF_TIME_NBR_uS_PER_SEC)) ;
}

CPU_INT64U  CPU_TS64_to_uSec (CPU_TS64  ts_cnts)
{
    RCC_ClocksTypeDef stRCCClockFreqInfo ;

    RCC_GetClocksFreq(&stRCCClockFreqInfo) ;

    return(ts_cnts / (stRCCClockFreqInfo.HCLK_Frequency / DEF_TIME_NBR_uS_PER_SEC)) ;
}

#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file sys_cfg.c. *******/  
