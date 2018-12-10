/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : sys_cfg.h
    Project name : ��������ֲģ�顣
    Module name  : 
    Date created : 2017��8��17��   10ʱ21��23��
    Author       : Ning.JianLi
    Description  : STM32ϵͳ���ýӿ�
*******************************************************************************/

#ifndef __SYS_CFG_H__
#define __SYS_CFG_H__

#include <stdio.h>
#include "cpu.h"
#include "stm32f4xx.h"
#include "pub_type.h"
#include "sys_interrupt.h"

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

/*  PLL_M = 25, PLL_N = 336, PLL_P = 2, PLL_Q = 7
 *  PLLCLK    = HSE * (PLLN / PLLM)      = 336MHz.
 *  SYSCLK    = PLLCLK / PLLP            = 168MHz.
 *  OTG_FSCLK = PLLCLK / PLLQ            =  48MHz.
 */

/* VCO��Ƶϵ�� */
#define SYS_PLL_CONFIG_INFO_PLLM              25

/* VCO��Ƶϵ�� */
#define SYS_PLL_CONFIG_INFO_PLLN              336

/* ����Ƶϵ��(ϵͳʱ��) */
#define SYS_PLL_CONFIG_INFO_PLLP              2

/* ����Ƶϵ��(����USB OTG FS��SDIO��RNG) */
#define SYS_PLL_CONFIG_INFO_PLLQ              7

#define SYS_REG_DEM_CR                        (*(CPU_REG32 *)0xE000EDFC)
#define SYS_REG_DWT_CR                        (*(CPU_REG32 *)0xE0001000)
#define SYS_REG_DWT_CYCCNT                    (*(CPU_REG32 *)0xE0001004)

#define SYS_BIT_DEM_CR_TRCENA                 DEF_BIT_24
#define SYS_BIT_DWT_CR_CYCCNTENA              DEF_BIT_00

INT fputc(INT iChar, FILE *pstFile) ;
VOID __SYS_SystemClockConfig(VOID) ;
VOID __SYS_SystemAllGPIOConfig(VOID) ;
VOID __SYS_SystemSerialPortConfig(VOID) ;
VOID SYS_SystemConfig(VOID) ;
VOID SYS_SystemTickInit(VOID) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __SYS_CFG_H__ */

/******* End of file sys_cfg.h. *******/  
