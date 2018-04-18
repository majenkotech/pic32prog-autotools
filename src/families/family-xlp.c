/*
 * Routines specific for PIC32 MXx7x XLP family.
 *
 * Copyright (C) 2013 Serge Vakulenko
 * Copyright (C) 2015-2017 Majenko Technologies
 *
 * This file is part of PIC32PROG project, which is distributed
 * under the terms of the GNU General Public License (GPL).
 * See the accompanying file "COPYING" for more details.
 */
#include <stdio.h>
#include "pic32.h"

/*
 * Print configuration for XLP/2 XLP family.
 */
void print_xlp(unsigned cfg0, unsigned cfg1, unsigned cfg2, unsigned cfg3)
{
    /*--------------------------------------
     * Configuration register 0
     */
    printf("    DEVCFG0 = %08x\n", cfg0);
    if ((~cfg0 & XLP_CFG0_DEBUG_MASK) == XLP_CFG0_DEBUG_ENABLED)
        printf("                     %u Debugger enabled\n",
            cfg0 & XLP_CFG0_DEBUG_MASK);
    else
        printf("                     %u Debugger disabled\n",
            cfg0 & XLP_CFG0_DEBUG_MASK);

    if (~cfg0 & XLP_CFG0_JTAG_DISABLE)
        printf("                     %u JTAG disabled\n",
            cfg0 & XLP_CFG0_JTAG_DISABLE);

    switch (~cfg0 & XLP_CFG0_ICESEL_MASK) {
    case XLP_CFG0_ICESEL_PAIR1:
        printf("                    %02x Use PGC1/PGD1\n", cfg0 & XLP_CFG0_ICESEL_MASK);
        break;
    case XLP_CFG0_ICESEL_PAIR2:
        printf("                    %02x Use PGC2/PGD2\n", cfg0 & XLP_CFG0_ICESEL_MASK);
        break;
    case XLP_CFG0_ICESEL_PAIR3:
        printf("                    %02x Use PGC3/PGD3\n", cfg0 & XLP_CFG0_ICESEL_MASK);
        break;
    case XLP_CFG0_ICESEL_PAIR4:
        printf("                    %02x Use PGC4/PGD4\n", cfg0 & XLP_CFG0_ICESEL_MASK);
        break;
    }

    if (~cfg0 & XLP_CFG0_PWP_MASK)
        printf("                 %05x Program flash write protect\n",
            cfg0 & XLP_CFG0_PWP_MASK);

    if (~cfg0 & XLP_CFG0_SMCLR)
        printf("                       MCLR generates POR\n");
    if (~cfg0 & XLP_CFG0_BWP)
        printf("                       Boot flash write protect\n");
    if (~cfg0 & XLP_CFG0_CP)
        printf("                       Code protect\n");


    /*--------------------------------------
     * Configuration register 1
     */
    printf("    DEVCFG1 = %08x\n", cfg1);
    switch (cfg1 & XLP_CFG1_FNOSC_MASK) {
    case XLP_CFG1_FNOSC_FRC:
        printf("                     %u Fast RC Oscillator\n", XLP_CFG1_FNOSC_FRC);
        break; 
    
    case XLP_CFG1_FNOSC_FRCPLL:
        printf("                     %u Fast RC Oscillator with divide-by-N with PLL\n", XLP_CFG1_FNOSC_FRCPLL);
        break;

    case XLP_CFG1_FNOSC_PRI:
        printf("                     %u Primary Oscillator\n", XLP_CFG1_FNOSC_PRI);
        break;

    case XLP_CFG1_FNOSC_PRIPLL:
        printf("                     %u Primary Oscillator with PLL\n", XLP_CFG1_FNOSC_PRIPLL);
        break;
   
    case XLP_CFG1_FNOSC_SOSC:
        printf("                     %u Secondary Oscillator\n", XLP_CFG1_FNOSC_SOSC);
        break;

    case XLP_CFG1_FNOSC_LPRC:
        printf("                     %u Low-Power RC Oscillator\n", XLP_CFG1_FNOSC_LPRC);
        break;

    case XLP_CFG1_FNOSC_FRCDIV16:
        printf("                     %u Fast RC Oscillator with fixed divide-by-16 postscaler\n", XLP_CFG1_FNOSC_FRCDIV16);
        break;

    case XLP_CFG1_FNOSC_FRCDIV:
        printf("                     %u Fast RC Oscillator with divide-by-N\n", XLP_CFG1_FNOSC_FRCDIV);
        break;
    }

    if (cfg1 & XLP_CFG1_FSOSCEN)
        printf("                    %u  Secondary oscillator enabled\n",
            XLP_CFG1_FSOSCEN >> 4);
    if (cfg1 & XLP_CFG1_IESO)
        printf("                    %u  Internal-external switch over enabled\n",
            XLP_CFG1_IESO >> 4);

    switch (cfg1 & XLP_CFG1_POSCMOD_MASK) {
    case XLP_CFG1_POSCMOD_EXT:
        printf("                   %u   Primary oscillator: External\n", XLP_CFG1_POSCMOD_EXT >> 8);
        break;
    case XLP_CFG1_POSCMOD_XT:
        printf("                   %u   Primary oscillator: XT\n", XLP_CFG1_POSCMOD_XT >> 8);
        break;
    case XLP_CFG1_POSCMOD_HS:
        printf("                   %u   Primary oscillator: HS\n", XLP_CFG1_POSCMOD_HS >> 8);
        break;
    case XLP_CFG1_POSCMOD_DISABLE:
        printf("                   %u   Primary oscillator: disabled\n", XLP_CFG1_POSCMOD_DISABLE >> 8);
        break;
    }
    if (cfg1 & XLP_CFG1_CLKO_DISABLE)
        printf("                   %u   CLKO output disabled\n",
            XLP_CFG1_CLKO_DISABLE >> 8);

    switch (cfg1 & XLP_CFG1_FPBDIV_MASK) {
    case XLP_CFG1_FPBDIV_1:
        printf("                  %u    Peripheral bus clock: SYSCLK / 1\n", XLP_CFG1_FPBDIV_1 >> 12);
        break;
    case XLP_CFG1_FPBDIV_2:
        printf("                  %u    Peripheral bus clock: SYSCLK / 2\n", XLP_CFG1_FPBDIV_2 >> 12);
        break;
    case XLP_CFG1_FPBDIV_4:
        printf("                  %u    Peripheral bus clock: SYSCLK / 4\n", XLP_CFG1_FPBDIV_4 >> 12);
        break;
    case XLP_CFG1_FPBDIV_8:
        printf("                  %u    Peripheral bus clock: SYSCLK / 8\n", XLP_CFG1_FPBDIV_8 >> 12);
        break;
    }
    if (cfg1 & XLP_CFG1_FCKM_ENABLE)
        printf("                  %u    Fail-safe clock monitor enabled\n", XLP_CFG1_FCKM_ENABLE >> 12);
    if (cfg1 & XLP_CFG1_FCKS_ENABLE)
        printf("                  %u    Clock switching enabled\n", XLP_CFG1_FCKS_ENABLE >> 12);

    switch (cfg1 & XLP_CFG1_WDTPS_MASK) {
        case XLP_CFG1_WDTPS_1:
            printf("                %2x     Watchdog postscale: 1/1\n", XLP_CFG1_WDTPS_1 >> 16);
            break;
        case XLP_CFG1_WDTPS_2:
            printf("                %2x     Watchdog postscale: 1/2\n", XLP_CFG1_WDTPS_2 >> 16);
            break;
        case XLP_CFG1_WDTPS_4:
            printf("                %2x     Watchdog postscale: 1/4\n", XLP_CFG1_WDTPS_4 >> 16);
            break;
        case XLP_CFG1_WDTPS_8:
            printf("                %2x     Watchdog postscale: 1/8\n", XLP_CFG1_WDTPS_8 >> 16);
            break;
        case XLP_CFG1_WDTPS_16:
            printf("                %2x     Watchdog postscale: 1/16\n", XLP_CFG1_WDTPS_16 >> 16);
            break;
        case XLP_CFG1_WDTPS_32:
            printf("                %2x     Watchdog postscale: 1/32\n", XLP_CFG1_WDTPS_32 >> 16);
            break;
        case XLP_CFG1_WDTPS_64:
            printf("                %2x     Watchdog postscale: 1/64\n", XLP_CFG1_WDTPS_64 >> 16);
            break;
        case XLP_CFG1_WDTPS_128:
            printf("                %2x     Watchdog postscale: 1/128\n", XLP_CFG1_WDTPS_128 >> 16);
            break;
        case XLP_CFG1_WDTPS_256:
            printf("                %2x     Watchdog postscale: 1/256\n", XLP_CFG1_WDTPS_256 >> 16);
            break;
        case XLP_CFG1_WDTPS_512:
            printf("                %2x     Watchdog postscale: 1/512\n", XLP_CFG1_WDTPS_512 >> 16);
            break;
        case XLP_CFG1_WDTPS_1024:
            printf("                %2x     Watchdog postscale: 1/1024\n", XLP_CFG1_WDTPS_1024 >> 16);
            break;
        case XLP_CFG1_WDTPS_2048:
            printf("                %2x     Watchdog postscale: 1/2048\n", XLP_CFG1_WDTPS_2048 >> 16);
            break;
        case XLP_CFG1_WDTPS_4096:
            printf("                %2x     Watchdog postscale: 1/4096\n", XLP_CFG1_WDTPS_4096 >> 16);
            break;
        case XLP_CFG1_WDTPS_8192:
            printf("                %2x     Watchdog postscale: 1/8192\n", XLP_CFG1_WDTPS_8192 >> 16);
            break;
        case XLP_CFG1_WDTPS_16384:
            printf("                %2x     Watchdog postscale: 1/16384\n", XLP_CFG1_WDTPS_16384 >> 16);
            break;
        case XLP_CFG1_WDTPS_32768:
            printf("                %2x     Watchdog postscale: 1/32768\n", XLP_CFG1_WDTPS_32768 >> 16);
            break;
        case XLP_CFG1_WDTPS_65536:
            printf("                %2x     Watchdog postscale: 1/65536\n", XLP_CFG1_WDTPS_65536 >> 16);
            break;
        case XLP_CFG1_WDTPS_131072:
            printf("                %2x     Watchdog postscale: 1/131072\n", XLP_CFG1_WDTPS_131072 >> 16);
            break;
        case XLP_CFG1_WDTPS_262144:
            printf("                %2x     Watchdog postscale: 1/262144\n", XLP_CFG1_WDTPS_262144 >> 16);
            break;
        case XLP_CFG1_WDTPS_524288:
            printf("                %2x     Watchdog postscale: 1/524288\n", XLP_CFG1_WDTPS_524288 >> 16);
            break;
        case XLP_CFG1_WDTPS_1048576:
            printf("                %2x     Watchdog postscale: 1/1048576\n", XLP_CFG1_WDTPS_1048576 >> 16);
            break;
    }

    if (cfg1 & XLP_CFG1_WDTSPGM)
        printf("                %u      Watchdog timer stops during flash programming\n", XLP_CFG1_WDTSPGM >> 20);

    if (cfg1 & XLP_CFG1_WINDIS)
        printf("                %u      Watchdog in non-Window mode\n", XLP_CFG1_WINDIS >> 20);
    else {
        printf("                %u      Watchdog in Window mode\n", XLP_CFG1_WINDIS >> 20);
        switch(cfg1 & XLP_CFG1_FWDTWINSZ_MASK) {
            case XLP_CFG1_FWDTWINSZ_75:
                printf("              %u        Window size is 75%%\n", XLP_CFG1_FWDTWINSZ_75 >> 24);
                break;
            case XLP_CFG1_FWDTWINSZ_50:
                printf("              %u        Window size is 50%%\n", XLP_CFG1_FWDTWINSZ_50 >> 24);
                break;
            case XLP_CFG1_FWDTWINSZ_37:
                printf("              %u        Window size is 37.5%%\n", XLP_CFG1_FWDTWINSZ_37 >> 24);
                break;
            case XLP_CFG1_FWDTWINSZ_25:
                printf("              %u        Window size is 25%%\n", XLP_CFG1_FWDTWINSZ_25 >> 24);
                break;
        }
    }

    if (cfg1 & XLP_CFG1_FWDTEN) 
        printf("                %u      Watchdog enable\n", XLP_CFG1_FWDTEN >> 20);
    

    /*--------------------------------------
     * Configuration register 2
     */
    printf("    DEVCFG2 = %08x\n", cfg2);
    switch (cfg2 & XLP_CFG2_FPLLIDIV_MASK) {
    case XLP_CFG2_FPLLIDIV_1:
        printf("                     %u PLL divider: 1/1\n", XLP_CFG2_FPLLIDIV_1);
        break;
    case XLP_CFG2_FPLLIDIV_2:
        printf("                     %u PLL divider: 1/2\n", XLP_CFG2_FPLLIDIV_2);
        break;
    case XLP_CFG2_FPLLIDIV_3:
        printf("                     %u PLL divider: 1/3\n", XLP_CFG2_FPLLIDIV_3);
        break;
    case XLP_CFG2_FPLLIDIV_4:
        printf("                     %u PLL divider: 1/4\n", XLP_CFG2_FPLLIDIV_4);
        break;
    case XLP_CFG2_FPLLIDIV_5:
        printf("                     %u PLL divider: 1/5\n", XLP_CFG2_FPLLIDIV_5);
        break;
    case XLP_CFG2_FPLLIDIV_6:
        printf("                     %u PLL divider: 1/6\n", XLP_CFG2_FPLLIDIV_6);
        break;
    case XLP_CFG2_FPLLIDIV_10:
        printf("                     %u PLL divider: 1/10\n", XLP_CFG2_FPLLIDIV_10);
        break;
    case XLP_CFG2_FPLLIDIV_12:
        printf("                     %u PLL divider: 1/12\n", XLP_CFG2_FPLLIDIV_12);
        break;
    }
    switch (cfg2 & XLP_CFG2_FPLLMUL_MASK) {
    case XLP_CFG2_FPLLMUL_15:
        printf("                    %u  PLL multiplier: 15x\n", XLP_CFG2_FPLLMUL_15 >> 4);
        break;
    case XLP_CFG2_FPLLMUL_16:
        printf("                    %u  PLL multiplier: 16x\n", XLP_CFG2_FPLLMUL_16 >> 4);
        break;
    case XLP_CFG2_FPLLMUL_17:
        printf("                    %u  PLL multiplier: 17x\n", XLP_CFG2_FPLLMUL_17 >> 4);
        break;
    case XLP_CFG2_FPLLMUL_18:
        printf("                    %u  PLL multiplier: 18x\n", XLP_CFG2_FPLLMUL_18 >> 4);
        break;
    case XLP_CFG2_FPLLMUL_19:
        printf("                    %u  PLL multiplier: 19x\n", XLP_CFG2_FPLLMUL_19 >> 4);
        break;
    case XLP_CFG2_FPLLMUL_20:
        printf("                    %u  PLL multiplier: 20x\n", XLP_CFG2_FPLLMUL_20 >> 4);
        break;
    case XLP_CFG2_FPLLMUL_21:
        printf("                    %u  PLL multiplier: 21x\n", XLP_CFG2_FPLLMUL_21 >> 4);
        break;
    case XLP_CFG2_FPLLMUL_24:
        printf("                    %u  PLL multiplier: 24x\n", XLP_CFG2_FPLLMUL_24 >> 4);
        break;
    }
    if (cfg2 & XLP_CFG2_FPLLICLK) {
        printf("                    %x  FRC is selected as input to the System PLL\n", XLP_CFG2_FPLLICLK >> 4);
    } else {
        printf("                    %x  POSC is selected as input to the System PLL\n", 0);
    }

    switch (cfg2 & XLP_CFG2_UPLLIDIV_MASK) {
    case XLP_CFG2_UPLLIDIV_1:
        printf("                   %u   USB PLL divider: 1/1\n", XLP_CFG2_UPLLIDIV_1 >> 8);
        break;
    case XLP_CFG2_UPLLIDIV_2:
        printf("                   %u   USB PLL divider: 1/2\n", XLP_CFG2_UPLLIDIV_2 >> 8);
        break;
    case XLP_CFG2_UPLLIDIV_3:
        printf("                   %u   USB PLL divider: 1/3\n", XLP_CFG2_UPLLIDIV_3 >> 8);
        break;
    case XLP_CFG2_UPLLIDIV_4:
        printf("                   %u   USB PLL divider: 1/4\n", XLP_CFG2_UPLLIDIV_4 >> 8);
        break;
    case XLP_CFG2_UPLLIDIV_5:
        printf("                   %u   USB PLL divider: 1/5\n", XLP_CFG2_UPLLIDIV_5 >> 8);
        break;
    case XLP_CFG2_UPLLIDIV_6:
        printf("                   %u   USB PLL divider: 1/6\n", XLP_CFG2_UPLLIDIV_6 >> 8);
        break;
    case XLP_CFG2_UPLLIDIV_10:
        printf("                   %u   USB PLL divider: 1/10\n", XLP_CFG2_UPLLIDIV_10 >> 8);
        break;
    case XLP_CFG2_UPLLIDIV_12:
        printf("                   %u   USB PLL divider: 1/12\n", XLP_CFG2_UPLLIDIV_12 >> 8);
        break;
    }
    if (cfg2 & XLP_CFG2_UPLL_DISABLE)
        printf("                  %u    Disable USB PLL\n",
            XLP_CFG2_UPLL_DISABLE >> 12);
    else
        printf("                       Enable USB PLL\n");

    switch (cfg2 & XLP_CFG2_FPLLODIV_MASK) {
    case XLP_CFG2_FPLLODIV_1:
        printf("                 %u     PLL postscaler: 1/1\n", XLP_CFG2_FPLLODIV_1 >> 16);
        break;
    case XLP_CFG2_FPLLODIV_2:
        printf("                 %u     PLL postscaler: 1/2\n", XLP_CFG2_FPLLODIV_2 >> 16);
        break;
    case XLP_CFG2_FPLLODIV_4:
        printf("                 %u     PLL postscaler: 1/4\n", XLP_CFG2_FPLLODIV_4 >> 16);
        break;
    case XLP_CFG2_FPLLODIV_8:
        printf("                 %u     PLL postscaler: 1/8\n", XLP_CFG2_FPLLODIV_8 >> 16);
        break;
    case XLP_CFG2_FPLLODIV_16:
        printf("                 %u     PLL postscaler: 1/16\n", XLP_CFG2_FPLLODIV_16 >> 16);
        break;
    case XLP_CFG2_FPLLODIV_32:
        printf("                 %u     PLL postscaler: 1/32\n", XLP_CFG2_FPLLODIV_32 >> 16);
        break;
    case XLP_CFG2_FPLLODIV_64:
        printf("                 %u     PLL postscaler: 1/64\n", XLP_CFG2_FPLLODIV_64 >> 16);
        break;
    case XLP_CFG2_FPLLODIV_256:
        printf("                 %u     PLL postscaler: 1/128\n", XLP_CFG2_FPLLODIV_256 >> 16);
        break;
    }

    if (cfg2 & XLP_CFG2_BOREN)
        printf("                %x      Brown-Out Reset Enabled\n", XLP_CFG2_BOREN >> 20);
  
    if (cfg2 & XLP_CFG2_VBATBOREN)
        printf("                %x      VBAT BOR Enabled\n", XLP_CFG2_VBATBOREN >> 20);
 
    if (cfg2 & XLP_CFG2_DSBOREN)
        printf("                %x      Deep Sleep BOR Enabled\n", XLP_CFG2_DSBOREN >> 20);

    if (cfg2 & XLP_CFG2_DSWDTEN) {
        printf("              %2x       Deep Sleep Watchdog Postscale 1:2^%d\n", (cfg2 & XLP_CFG2_DSWDTPS_MASK) >> 24, 5 + ((cfg2 & XLP_CFG2_DSWDTPS_MASK) >> 24));
        if (cfg2 & XLP_CFG2_DSWDTOSC) {
            printf("              %x        Deep Sleep Watchdog Clock is LPRC\n", XLP_CFG2_DSWDTOSC >> 28);
        } else {
            printf("              %x        Deep Sleep Watchdog Clock is SOSC\n", 0);
        }
        printf("              %x        Deep Sleep Watchdog Enabled\n", XLP_CFG2_DSWDTEN >> 28);
    }
    if (cfg2 & XLP_CFG2_FDSEN) {
        printf("              %x        Deep Sleep Enabled\n", XLP_CFG2_FDSEN >> 28);
    }

    

    /*--------------------------------------
     * Configuration register 3
     */
    printf("    DEVCFG3 = %08x\n", cfg3);
    if (~cfg3 & XLP_CFG3_USERID_MASK)
        printf("                  %04x User-defined ID\n",
            cfg3 & XLP_CFG3_USERID_MASK);

    if (cfg3 & XLP_CFG3_PMDL1WAY)
        printf("              %u        Peripheral Module Disable - only 1 reconfig\n",
            XLP_CFG3_PMDL1WAY >> 28);
    else
        printf("                       USBID pin: controlled by port\n");

    if (cfg3 & XLP_CFG3_IOL1WAY)
        printf("              %u        Peripheral Pin Select - only 1 reconfig\n",
            XLP_CFG3_IOL1WAY >> 28);
    else
        printf("                       USBID pin: controlled by port\n");

    if (cfg3 & XLP_CFG3_FUSBIDIO)
        printf("              %u        USBID pin: controlled by USB\n",
            XLP_CFG3_FUSBIDIO >> 28);
    else
        printf("                       USBID pin: controlled by port\n");

    if (cfg3 & XLP_CFG3_FVBUSONIO)
        printf("              %u        VBuson pin: controlled by USB\n",
            XLP_CFG3_FVBUSONIO >> 28);
    else
        printf("                       VBuson pin: controlled by port\n");
}

unsigned word_mask_xlp(unsigned address, unsigned word) {
    // DEVCFG0's highest bit doesn't exist.
    if (address == 0x9FC02FFC) {
        return word & 0x7FFFFFFF;
    }
    return word;
}
