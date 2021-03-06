/*
 * Interface to PIC32 microcontroller via debug port.
 *
 * Copyright (C) 2011-2013 Serge Vakulenko
 * Copyright (C) 2015-2017 Majenko Technologies
 *
 * This file is part of PIC32PROG project, which is distributed
 * under the terms of the GNU General Public License (GPL).
 * See the accompanying file "COPYING" for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include "target.h"
#include "adapter.h"
#include "localize.h"
#include "pic32.h"
#include "console.h"

#include "config.h"

extern print_func_t print_mx1;
extern print_func_t print_mx3;
extern print_func_t print_mz;
extern print_func_t print_xlp;

extern word_mask_func_t word_mask_mx1;
extern word_mask_func_t word_mask_mx3;
extern word_mask_func_t word_mask_xlp;
extern word_mask_func_t word_mask_mz;

extern unsigned long open_retries;

/*
 * PIC32 families.
 */
                    /*-Boot-Devcfg--Row---Print------Code--------Nwords-Version-*/
static const
family_t family_mx1 = { "mx1",
                        3,  0x0bf0, 128,  print_mx1, word_mask_mx1, pic32_pemx1, 422,  0x0301 };
static const
family_t family_xlp = { "xlp",
                        12, 0x2ff0, 512,  print_xlp, word_mask_xlp, pic32_pemx3, 1044, 0x0201 };
static const
family_t family_mx3 = { "mx3",
                        12, 0x2ff0, 512,  print_mx3, word_mask_mx3, pic32_pemx3, 1044, 0x0201 };
static const
family_t family_mz  = { "mz",
                        80, 0xffc0, 2048, print_mz,  word_mask_mz,  pic32_pemz,  1052, 0x0502 };
/*
 * This one is a special one for the bootloader. We have no idea what we're
 * programming, so set the values to the maximum out of all the others.
 * We don't really care at the end of the day.
 */
static const
family_t family_bl  = { "bootloader",
                        80, 0,      1024, 0,         0,           0,    0      };

/*
 * Table of PIC32 chip variants.
 * This list can be extended at run time from pic32prog.conf file.
 */
#define TABSZ   1000

static variant_t pic32_tab[TABSZ] = {

    /* MX1/2 family-------------Flash---Family */
    {0x4A07053, "MX110F016B",     16,   &family_mx1},
    {0x4A09053, "MX110F016C",     16,   &family_mx1},
    {0x4A0B053, "MX110F016D",     16,   &family_mx1},
    {0x4A06053, "MX120F032B",     32,   &family_mx1},
    {0x4A08053, "MX120F032C",     32,   &family_mx1},
    {0x4A0A053, "MX120F032D",     32,   &family_mx1},
    {0x6A50053, "MX120F064H",     64,   &family_mx1},
    {0x4D07053, "MX130F064B",     64,   &family_mx1},
    {0x4D09053, "MX130F064C",     64,   &family_mx1},
    {0x4D0B053, "MX130F064D",     64,   &family_mx1},
    {0x6A00053, "MX130F128H",    128,   &family_mx1},
    {0x6A01053, "MX130F128L",    128,   &family_mx1},
    {0x4D06053, "MX150F128B",    128,   &family_mx1},
    {0x4D08053, "MX150F128C",    128,   &family_mx1},
    {0x4D0A053, "MX150F128D",    128,   &family_mx1},
    {0x6A10053, "MX150F256H",    256,   &family_mx1},
    {0x6A11053, "MX150F256L",    256,   &family_mx1},
    {0x6610053, "MX170F256B",    256,   &family_mx1},
    {0x661A053, "MX170F256D",    256,   &family_mx1},
    {0x6A30053, "MX170F512H",    512,   &family_mx1},
    {0x6A31053, "MX170F512L",    512,   &family_mx1},
    {0x4A01053, "MX210F016B",     16,   &family_mx1},
    {0x4A03053, "MX210F016C",     16,   &family_mx1},
    {0x4A05053, "MX210F016D",     16,   &family_mx1},
    {0x4A00053, "MX220F032B",     32,   &family_mx1},
    {0x4A02053, "MX220F032C",     32,   &family_mx1},
    {0x4A04053, "MX220F032D",     32,   &family_mx1},
    {0x4D01053, "MX230F064B",     64,   &family_mx1},
    {0x4D03053, "MX230F064C",     64,   &family_mx1},
    {0x4D05053, "MX230F064D",     64,   &family_mx1},
    {0x6A02053, "MX230F128H",    128,   &family_mx1},
    {0x6A03053, "MX230F128L",    128,   &family_mx1},
    {0x4D00053, "MX250F128B",    128,   &family_mx1},
    {0x4D02053, "MX250F128C",    128,   &family_mx1},
    {0x4D04053, "MX250F128D",    128,   &family_mx1},
    {0x6A12053, "MX250F256H",    256,   &family_mx1},
    {0x6A13053, "MX250F256L",    256,   &family_mx1},
    {0x6600053, "MX270F256B",    256,   &family_mx1},
    {0x660A053, "MX270F256D",    256,   &family_mx1},
    {0x6A32053, "MX270F512H",    512,   &family_mx1},
    {0x6A33053, "MX270F512L",    512,   &family_mx1},
    {0x6A04053, "MX530F128H",    128,   &family_mx1},
    {0x6A05053, "MX530F128L",    128,   &family_mx1},
    {0x6A14053, "MX550F256H",    256,   &family_mx1},
    {0x6A15053, "MX550F256L",    256,   &family_mx1},
    {0x6A34053, "MX570F512H",    512,   &family_mx1},
    {0x6A35053, "MX570F512L",    512,   &family_mx1},

    /* XLP Family---------------Flash---Family */
    {0x7800053, "MX154F128B",    128,   &family_xlp},
    {0x7804053, "MX154F128D",    128,   &family_xlp},
    {0x7808053, "MX155F128B",    128,   &family_xlp},
    {0x780C053, "MX155F128D",    128,   &family_xlp},
    {0x7801053, "MX174F256B",    256,   &family_xlp},
    {0x7805053, "MX174F256D",    256,   &family_xlp},
    {0x7809053, "MX175F256B",    256,   &family_xlp},
    {0x780D053, "MX175F256D",    256,   &family_xlp},
    {0x7802053, "MX254F128B",    128,   &family_xlp},
    {0x7806053, "MX254F128D",    128,   &family_xlp},
    {0x780A053, "MX255F128B",    128,   &family_xlp},
    {0x780E053, "MX255F128D",    128,   &family_xlp},
    {0x7803053, "MX274F256B",    256,   &family_xlp},
    {0x7807053, "MX274F256D",    256,   &family_xlp},
    {0x780B053, "MX275F256B",    256,   &family_xlp},
    {0x780F053, "MX275F256D",    256,   &family_xlp},

    /* MX3/4/5/6/7 family-------Flash---Family */
    {0x0902053, "MX320F032H",     32,   &family_mx3},
    {0x0906053, "MX320F064H",     64,   &family_mx3},
    {0x090A053, "MX320F128H",    128,   &family_mx3},
    {0x092A053, "MX320F128L",    128,   &family_mx3},
    {0x5600053, "MX330F064H",     64,   &family_mx3},
    {0x5601053, "MX330F064L",     64,   &family_mx3},
    {0x090D053, "MX340F128H",    128,   &family_mx3},
    {0x092D053, "MX340F128L",    128,   &family_mx3},
    {0x0912053, "MX340F256H",    256,   &family_mx3},
    {0x0916053, "MX340F512H",    512,   &family_mx3},
    {0x570C053, "MX350F128H",    128,   &family_mx3},
    {0x570D053, "MX350F128L",    128,   &family_mx3},
    {0x5704053, "MX350F256H",    256,   &family_mx3},
    {0x5705053, "MX350F256L",    256,   &family_mx3},
    {0x0934053, "MX360F256L",    256,   &family_mx3},
    {0x0938053, "MX360F512L",    512,   &family_mx3},
    {0x5808053, "MX370F512H",    512,   &family_mx3},
    {0x5809053, "MX370F512L",    512,   &family_mx3},
    {0x0942053, "MX420F032H",     32,   &family_mx3},
    {0x5602053, "MX430F064H",     64,   &family_mx3},
    {0x5603053, "MX430F064L",     64,   &family_mx3},
    {0x094D053, "MX440F128H",    128,   &family_mx3},
    {0x096D053, "MX440F128L",    128,   &family_mx3},
    {0x0952053, "MX440F256H",    256,   &family_mx3},
    {0x0956053, "MX440F512H",    512,   &family_mx3},
    {0x570E053, "MX450F128H",    128,   &family_mx3},
    {0x570F053, "MX450F128L",    128,   &family_mx3},
    {0x5706053, "MX450F256H",    256,   &family_mx3},
    {0x5707053, "MX450F256L",    256,   &family_mx3},
    {0x0974053, "MX460F256L",    256,   &family_mx3},
    {0x0978053, "MX460F512L",    512,   &family_mx3},
    {0x580A053, "MX470F512H",    512,   &family_mx3},
    {0x580B053, "MX470F512L",    512,   &family_mx3},
    {0x4400053, "MX534F064H",     64,   &family_mx3},
    {0x440C053, "MX534F064L",     64,   &family_mx3},
    {0x4401053, "MX564F064H",     64,   &family_mx3},
    {0x440D053, "MX564F064L",     64,   &family_mx3},
    {0x4403053, "MX564F128H",    128,   &family_mx3},
    {0x440F053, "MX564F128L",    128,   &family_mx3},
    {0x4317053, "MX575F256H",    256,   &family_mx3},
    {0x4333053, "MX575F256L",    256,   &family_mx3},
    {0x4309053, "MX575F512H",    512,   &family_mx3},
    {0x430F053, "MX575F512L",    512,   &family_mx3},
    {0x4405053, "MX664F064H",     64,   &family_mx3},
    {0x4411053, "MX664F064L",     64,   &family_mx3},
    {0x4407053, "MX664F128H",    128,   &family_mx3},
    {0x4413053, "MX664F128L",    128,   &family_mx3},
    {0x430B053, "MX675F256H",    256,   &family_mx3},
    {0x4305053, "MX675F256L",    256,   &family_mx3},
    {0x430C053, "MX675F512H",    512,   &family_mx3},
    {0x4311053, "MX675F512L",    512,   &family_mx3},
    {0x4325053, "MX695F512H",    512,   &family_mx3},
    {0x4341053, "MX695F512L",    512,   &family_mx3},
    {0x440B053, "MX764F128H",    128,   &family_mx3},
    {0x4417053, "MX764F128L",    128,   &family_mx3},
    {0x4303053, "MX775F256H",    256,   &family_mx3},
    {0x4312053, "MX775F256L",    256,   &family_mx3},
    {0x430D053, "MX775F512H",    512,   &family_mx3},
    {0x4306053, "MX775F512L",    512,   &family_mx3},
    {0x430E053, "MX795F512H",    512,   &family_mx3},
    {0x4307053, "MX795F512L",    512,   &family_mx3},

    /* MZ family----------------Flash---Family */
    {0x5100053, "MZ0256ECE064",  256,   &family_mz},
    {0x510A053, "MZ0256ECE100",  256,   &family_mz},
    {0x5114053, "MZ0256ECE124",  256,   &family_mz},
    {0x511E053, "MZ0256ECE144",  256,   &family_mz},
    {0x5105053, "MZ0256ECF064",  256,   &family_mz},
    {0x510F053, "MZ0256ECF100",  256,   &family_mz},
    {0x5119053, "MZ0256ECF124",  256,   &family_mz},
    {0x5123053, "MZ0256ECF144",  256,   &family_mz},
    {0x5101053, "MZ0512ECE064",  512,   &family_mz},
    {0x510B053, "MZ0512ECE100",  512,   &family_mz},
    {0x5115053, "MZ0512ECE124",  512,   &family_mz},
    {0x511F053, "MZ0512ECE144",  512,   &family_mz},
    {0x5106053, "MZ0512ECF064",  512,   &family_mz},
    {0x5110053, "MZ0512ECF100",  512,   &family_mz},
    {0x511A053, "MZ0512ECF124",  512,   &family_mz},
    {0x5124053, "MZ0512ECF144",  512,   &family_mz},
    {0x5102053, "MZ1024ECE064", 1024,   &family_mz},
    {0x510C053, "MZ1024ECE100", 1024,   &family_mz},
    {0x5116053, "MZ1024ECE124", 1024,   &family_mz},
    {0x5120053, "MZ1024ECE144", 1024,   &family_mz},
    {0x5107053, "MZ1024ECF064", 1024,   &family_mz},
    {0x5111053, "MZ1024ECF100", 1024,   &family_mz},
    {0x511B053, "MZ1024ECF124", 1024,   &family_mz},
    {0x5125053, "MZ1024ECF144", 1024,   &family_mz},
    {0x5103053, "MZ1024ECG064", 1024,   &family_mz},
    {0x510D053, "MZ1024ECG100", 1024,   &family_mz},
    {0x5117053, "MZ1024ECG124", 1024,   &family_mz},
    {0x5121053, "MZ1024ECG144", 1024,   &family_mz},
    {0x5108053, "MZ1024ECH064", 1024,   &family_mz},
    {0x5112053, "MZ1024ECH100", 1024,   &family_mz},
    {0x511C053, "MZ1024ECH124", 1024,   &family_mz},
    {0x5126053, "MZ1024ECH144", 1024,   &family_mz},
    {0x5130053, "MZ1024ECM064", 1024,   &family_mz},
    {0x513A053, "MZ1024ECM100", 1024,   &family_mz},
    {0x5144053, "MZ1024ECM124", 1024,   &family_mz},
    {0x514E053, "MZ1024ECM144", 1024,   &family_mz},
    {0x5104053, "MZ2048ECG064", 2048,   &family_mz},
    {0x510E053, "MZ2048ECG100", 2048,   &family_mz},
    {0x5118053, "MZ2048ECG124", 2048,   &family_mz},
    {0x5122053, "MZ2048ECG144", 2048,   &family_mz},
    {0x5109053, "MZ2048ECH064", 2048,   &family_mz},
    {0x5113053, "MZ2048ECH100", 2048,   &family_mz},
    {0x511D053, "MZ2048ECH124", 2048,   &family_mz},
    {0x5127053, "MZ2048ECH144", 2048,   &family_mz},
    {0x5131053, "MZ2048ECM064", 2048,   &family_mz},
    {0x513B053, "MZ2048ECM100", 2048,   &family_mz},
    {0x5145053, "MZ2048ECM124", 2048,   &family_mz},
    {0x514F053, "MZ2048ECM144", 2048,   &family_mz},

    /* MZ family with FPU-------Flash---Family */
    {0x7201053, "MZ0512EFE064",  512,   &family_mz},
    {0x7206053, "MZ0512EFF064",  512,   &family_mz},
    {0x722E053, "MZ0512EFK064",  512,   &family_mz},
    {0x7202053, "MZ1024EFE064", 1024,   &family_mz},
    {0x7207053, "MZ1024EFF064", 1024,   &family_mz},
    {0x722F053, "MZ1024EFK064", 1024,   &family_mz},
    {0x7203053, "MZ1024EFG064", 1024,   &family_mz},
    {0x7208053, "MZ1024EFH064", 1024,   &family_mz},
    {0x7230053, "MZ1024EFM064", 1024,   &family_mz},
    {0x7204053, "MZ2048EFG064", 2048,   &family_mz},
    {0x7209053, "MZ2048EFH064", 2048,   &family_mz},
    {0x7231053, "MZ2048EFM064", 2048,   &family_mz},

    {0x720B053, "MZ0512EFE100",  512,   &family_mz},
    {0x7210053, "MZ0512EFF100",  512,   &family_mz},
    {0x7238053, "MZ0512EFK100",  512,   &family_mz},
    {0x720C053, "MZ1024EFE100", 1024,   &family_mz},
    {0x7211053, "MZ1024EFF100", 1024,   &family_mz},
    {0x7239053, "MZ1024EFK100", 1024,   &family_mz},
    {0x720D053, "MZ1024EFG100", 1024,   &family_mz},
    {0x7212053, "MZ1024EFH100", 1024,   &family_mz},
    {0x723A053, "MZ1024EFM100", 1024,   &family_mz},
    {0x720E053, "MZ2048EFG100", 2048,   &family_mz},
    {0x7213053, "MZ2048EFH100", 2048,   &family_mz},
    {0x723B053, "MZ2048EFM100", 2048,   &family_mz},

    {0x7215053, "MZ0512EFE124",  512,   &family_mz},
    {0x721A053, "MZ0512EFF124",  512,   &family_mz},
    {0x7242053, "MZ0512EFK124",  512,   &family_mz},
    {0x7216053, "MZ1024EFE124", 1024,   &family_mz},
    {0x721B053, "MZ1024EFF124", 1024,   &family_mz},
    {0x7243053, "MZ1024EFK124", 1024,   &family_mz},
    {0x7217053, "MZ1024EFG124", 1024,   &family_mz},
    {0x721C053, "MZ1024EFH124", 1024,   &family_mz},
    {0x7244053, "MZ1024EFM124", 1024,   &family_mz},
    {0x7218053, "MZ2048EFG124", 2048,   &family_mz},
    {0x721D053, "MZ2048EFH124", 2048,   &family_mz},
    {0x7245053, "MZ2048EFM124", 2048,   &family_mz},

    {0x721F053, "MZ0512EFE144",  512,   &family_mz},
    {0x7224053, "MZ0512EFF144",  512,   &family_mz},
    {0x724C053, "MZ0512EFK144",  512,   &family_mz},
    {0x7220053, "MZ1024EFE144", 1024,   &family_mz},
    {0x7225053, "MZ1024EFF144", 1024,   &family_mz},
    {0x724D053, "MZ1024EFK144", 1024,   &family_mz},
    {0x7221053, "MZ1024EFG144", 1024,   &family_mz},
    {0x7226053, "MZ1024EFH144", 1024,   &family_mz},
    {0x724E053, "MZ1024EFM144", 1024,   &family_mz},
    {0x7222053, "MZ2048EFG144", 2048,   &family_mz},
    {0x7227053, "MZ2048EFH144", 2048,   &family_mz},
    {0x724F053, "MZ2048EFM144", 2048,   &family_mz},

    {0x5f69053, "MZ2064DAA288", 2048,   &family_mz},
    {0x5F0C053, "MZ1025DAA169", 1024,   &family_mz},
    {0x5F0D053, "MZ1025DAB169", 1024,   &family_mz},
    {0x5F0F053, "MZ1064DAA169", 1024,   &family_mz},
    {0x5F10053, "MZ1064DAB169", 1024,   &family_mz},
    {0x5F15053, "MZ2025DAA169", 2048,   &family_mz},
    {0x5F16053, "MZ2025DAB169", 2048,   &family_mz},
    {0x5F18053, "MZ2064DAA169", 2048,   &family_mz},
    {0x5F19053, "MZ2064DAB169", 2048,   &family_mz},
    {0x5F42053, "MZ1025DAG169", 1024,   &family_mz},
    {0x5F43053, "MZ1025DAH169", 1024,   &family_mz},
    {0x5F45053, "MZ1064DAG169", 1024,   &family_mz},
    {0x5F46053, "MZ1064DAH169", 1024,   &family_mz},
    {0x5F4B053, "MZ2025DAG169", 2048,   &family_mz},
    {0x5F4C053, "MZ2025DAH169", 2048,   &family_mz},
    {0x5F4E053, "MZ2064DAG169", 2048,   &family_mz},
    {0x5F4F053, "MZ2064DAH169", 2048,   &family_mz},
    {0x5F78053, "MZ1025DAA176", 1024,   &family_mz},
    {0x5F79053, "MZ1025DAB176", 1024,   &family_mz},
    {0x5F7B053, "MZ1064DAA176", 1024,   &family_mz},
    {0x5F7C053, "MZ1064DAB176", 1024,   &family_mz},
    {0x5F81053, "MZ2025DAA176", 2048,   &family_mz},
    {0x5F82053, "MZ2025DAB176", 2048,   &family_mz},
    {0x5F84053, "MZ2064DAA176", 2048,   &family_mz},
    {0x5F85053, "MZ2064DAB176", 2048,   &family_mz},
    {0x5FAE053, "MZ1025DAG176", 1024,   &family_mz},
    {0x5FAF053, "MZ1025DAH176", 1024,   &family_mz},
    {0x5FB1053, "MZ1064DAG176", 1024,   &family_mz},
    {0x5FB2053, "MZ1064DAH176", 1024,   &family_mz},
    {0x5FB7053, "MZ2025DAG176", 2048,   &family_mz},
    {0x5FB8053, "MZ2025DAH176", 2048,   &family_mz},
    {0x5FBA053, "MZ2064DAG176", 2048,   &family_mz},
    {0x5FBB053, "MZ2064DAH176", 2048,   &family_mz},
    {0x5F5D053, "MZ1025DAA288", 1024,   &family_mz},
    {0x5F5E053, "MZ1025DAB288", 1024,   &family_mz},
    {0x5F60053, "MZ1064DAA288", 1024,   &family_mz},
    {0x5F61053, "MZ1064DAB288", 1024,   &family_mz},
    {0x5F66053, "MZ2025DAA288", 2048,   &family_mz},
    {0x5F67053, "MZ2025DAB288", 2048,   &family_mz},
    {0x5F69053, "MZ2064DAA288", 2048,   &family_mz},
    {0x5F6A053, "MZ2064DAB288", 2048,   &family_mz},

    /* USB bootloader */
    {0xEAFB00B, "Bootloader",   0,      &family_bl},
    {0}
};

/*
 * Table of supported serial protocols.
 */
static const struct {
    const char *prefix;
    adapter_t *(*func)(const char *port, int baud);
} serial_tab[] = {
#ifdef ENABLE_STK500V2
    { "stk500",     adapter_open_stk500v2       },  /* Default */
#endif
#ifdef ENABLE_AN1388_UART
    { "an1388",     adapter_open_an1388_uart    },
#endif
#ifdef ENABLE_BITBANG
    { "ascii",      adapter_open_bitbang        },
#endif
    { 0 },
};

/*
 * Table of supported USB protocols.
 */
static const struct {
    const char *prefix;
    adapter_t *(*func)(int vid, int pid, const char *serial, int report);
} usb_tab[] = {
#ifdef ENABLE_PICKIT2
    { "pickit2",    adapter_open_pickit2        },
    { "pickit3",    adapter_open_pickit3        },
#endif
#ifdef ENABLE_HIDBOOT
    { "hidboot",    adapter_open_hidboot        },
#endif
#ifdef ENABLE_AN1388
    { "an1388",     adapter_open_an1388         },
#endif
#ifdef ENABLE_UHB
    { "uhb",        adapter_open_uhb            },
#endif
    { 0 },
};

#if defined(__CYGWIN32__) || defined(MINGW32)
/*
 * Delay in milliseconds: Windows.
 */
#include <windows.h>

void mdelay(unsigned msec)
{
    Sleep(msec);
}

int __ms_vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
    // Needed to link the libusb-win32/libusb-1.0.a library.
    return 0;
}
#else
/*
 * Delay in milliseconds: Unix.
 */
void mdelay(unsigned msec)
{
    usleep(msec * 1000);
}
#endif

/*
 * Open USB adapter, detected by vendor/product ID.
 * Return a pointer to adapter structure, or 0 when not found.
 */
static adapter_t *open_usb_adapter(const char *port_name, int report)
{
    char *delimiter;
    const char *serial = 0;
    int prefix_len, i, vid, pid;

    if (!port_name) {
        /* Autodetect the device from a list of known adapters. */
        adapter_t *a = NULL;
#ifdef ENABLE_PICKIT2
        if (! a)
            a = adapter_open_pickit2(0, 0, 0, report);
        if (! a)
            a = adapter_open_pickit3(0, 0, 0, report);
#endif
#ifdef ENABLE_MPSSE
        if (! a)
            a = adapter_open_mpsse(0, 0, 0, report);
#endif
#ifdef ENABLE_HIDBOOT
        if (! a)
            a = adapter_open_hidboot(0, 0, 0, report);
#endif
#ifdef ENABLE_AN1388
        if (! a)
            a = adapter_open_an1388(0, 0, 0, report);
#endif
#ifdef ENABLE_UHB
        if (! a)
            a = adapter_open_uhb(0, 0, 0, report);
#endif
        return a;
    }

    /* Get protocol prefix. */
    delimiter = strchr(port_name, ':');
    if (! delimiter)
        return 0;
    prefix_len = delimiter - port_name;

    /* Find prefix in the protocol table. */
    for (i=0; usb_tab[i].prefix; i++) {
        int len = strlen(usb_tab[i].prefix);
        if (prefix_len == len &&
            strncasecmp(port_name, usb_tab[i].prefix, len) == 0) {
            goto found;
        }
    }
    fprintf(stderr, "%s: Unknown USB protocol\n", port_name);
    return 0;

found:
    vid = strtoul(delimiter+1, &delimiter, 16);
    if (*delimiter != ':') {
        fprintf(stderr, "%s: Incorrect VID:PID value\n", port_name);
        return 0;
    }
    pid = strtoul(delimiter+1, &delimiter, 16);
    if (*delimiter == ':')
        serial = delimiter+1;

    return usb_tab[i].func(vid, pid, serial, report);
}

/*
 * Open serial adapter.
 * Use Arduino-compatible protocol (stk500v2) by default.
 * To select other protocols, add a prefix to the port name,
 * like "bitbang:COM5".
 */
static adapter_t *open_serial_adapter(const char *port_name, int baud_rate)
{
    const char *prefix, *delimiter;
    int prefix_len, len, i;

    /* Get protocol prefix. */
    delimiter = strchr(port_name, ':');
    if (! delimiter) {
        /* Use stk500v2 protocol by default. */
        return serial_tab[0].func(port_name, baud_rate);
    }
    prefix_len = delimiter - port_name;
    prefix = port_name;
    port_name = delimiter + 1;

    /* Find prefix in the protocol table. */
    for (i=0; serial_tab[i].prefix; i++) {
        len = strlen(serial_tab[i].prefix);
        if (prefix_len == len &&
            strncasecmp(prefix, serial_tab[i].prefix, len) == 0) {
            return serial_tab[i].func(port_name, baud_rate);
        }
    }
    return 0;
}

/*
 * Parse the name of the device.
 * Return 1 in case of USB device (0 for serial).
 */
static int is_usb_device(const char *port_name)
{
    const char *delimiter;

    /* No device name specified - search for a known USB device. */
    if (!port_name)
        return 1;

    /* Get protocol prefix. */
    delimiter = strchr(port_name, ':');
    if (!delimiter) {
        /* No protocol prefix - use serial protocol. */
        return 0;
    }

    /* Get VID. */
    delimiter = strchr(delimiter + 1, ':');
    if (!delimiter) {
        /* No VID - assume serial protocol. */
        return 0;
    }

    /* Protocol prefix, VID and PID are present - use USB protocol. */
    return 1;
}

/*
 * Connect to JTAG adapter.
 */
target_t *target_open(const char *port_name, int baud_rate)
{
    target_t *t;

    t = calloc(1, sizeof(target_t));
    if (! t) {
        fprintf(stderr, _("Out of memory\n"));
        exit(-1);
    }
    t->cpu_name = "Unknown";

    /* Update pic2_tab[] array from the pic32prog.conf file. */
    target_configure();

    /* Find adapter. */
    if (open_retries == 0) open_retries = 1;

    if (open_retries > 1) {
        conprintf("\n*** Enter programming mode now. ***\n\n");
        fflush(stdout);
    }

    while(open_retries > 0) {
        if (is_usb_device(port_name)) {
            t->adapter = open_usb_adapter(port_name, open_retries == 1);
        } else {
            t->adapter = open_serial_adapter(port_name, baud_rate);
        }

        if (t->adapter) break;

        open_retries--;
        if (open_retries > 0) {
            usleep(500000);
        }
    }
    if (! t->adapter) {
        fprintf(stderr, "\n");
        fprintf(stderr, _("No target found.\n"));
        exit(-1);
    }

    /* Check CPU identifier. */
    t->cpuid = t->adapter->get_idcode(t->adapter);
    if (t->cpuid == 0) {
        /* Device not responding. */
        fprintf(stderr, _("Unknown CPUID=%08x.\n"), t->cpuid);
        t->adapter->close(t->adapter, 0);
        exit(1);
    }

    unsigned i;
    for (i=0; (t->cpuid ^ pic32_tab[i].devid) & 0x0fffffff; i++) {
        if (pic32_tab[i].devid == 0) {
            /* Device not detected. */
            fprintf(stderr, _("Unknown CPUID=%08x.\n"), t->cpuid);
            t->adapter->close(t->adapter, 0);
            exit(1);
        }
    }
    t->family = pic32_tab[i].family;
    t->cpu_name = pic32_tab[i].name;
    t->flash_addr = 0x1d000000;
    t->flash_bytes = pic32_tab[i].flash_kbytes * 1024;
    if (! t->flash_bytes) {
        t->flash_addr = t->adapter->user_start;
        t->flash_bytes = t->adapter->user_nbytes;
        t->boot_bytes = t->adapter->boot_nbytes;
    }
    t->adapter->family_name = t->family->name;
    return t;
}

/*
 * Close the device.
 */
void target_close(target_t *t, int power_on)
{
    t->adapter->close(t->adapter, power_on);
}

const char *target_cpu_name(target_t *t)
{
    return t->cpu_name;
}

unsigned target_idcode(target_t *t)
{
    return t->cpuid;
}

unsigned target_flash_bytes(target_t *t)
{
    return t->flash_bytes;
}

unsigned target_boot_bytes(target_t *t)
{
    return t->family->boot_kbytes * 1024;
}

unsigned target_devcfg_offset(target_t *t)
{
    return t->family->devcfg_offset;
}

unsigned target_block_size(target_t *t)
{
    return t->family->bytes_per_row;
}

/*
 * Add an entry to the pic32_tab[] array.
 */
void target_add_variant(char *name, unsigned id,
    char *family, unsigned flash_kbytes)
{
    int i;

    for (i=0; i<TABSZ; i++) {
        if (pic32_tab[i].devid == 0 ||
            id == pic32_tab[i].devid) {
            /* Add a new entry or update an existing one
             * with new data from config file. */
            pic32_tab[i].name = strdup(name);
            pic32_tab[i].flash_kbytes = flash_kbytes;
            if (strcmp(family, "MX1") == 0)
                pic32_tab[i].family = &family_mx1;
            else if (strcmp(family, "MX3") == 0)
                pic32_tab[i].family = &family_mx3;
            else if (strcmp(family, "MZ") == 0)
                pic32_tab[i].family = &family_mz;
            else {
                fprintf(stderr, "%s: Unknown family=%s.\n", name, family);
            }
            break;
        }
    }
}

/*
 * Use PE for reading/writing/erasing memory.
 */
void target_use_executive(target_t *t)
{
    if (t->adapter->load_executive != 0 && t->family->pe_nwords != 0)
        t->adapter->load_executive(t->adapter, t->family->pe_code,
            t->family->pe_nwords, t->family->pe_version);
}

/*
 * Print configuration registers of the target CPU.
 */
void target_print_devcfg(target_t *t)
{
    if (! t->family->devcfg_offset)
        return;

    unsigned devcfg_addr = 0x1fc00000 + target_devcfg_offset(t);
    unsigned devcfg3 = t->adapter->read_word(t->adapter, devcfg_addr);
    unsigned devcfg2 = t->adapter->read_word(t->adapter, devcfg_addr + 4);
    unsigned devcfg1 = t->adapter->read_word(t->adapter, devcfg_addr + 8);
    unsigned devcfg0 = t->adapter->read_word(t->adapter, devcfg_addr + 12);

    if (devcfg3 == 0xffffffff && devcfg2 == 0xffffffff &&
        devcfg1 == 0xffffffff && devcfg0 == 0x7fffffff)
        return;
    if (devcfg3 == 0 && devcfg2 == 0 && devcfg1 == 0 && devcfg0 == 0)
        return;

    conprintf(_("Configuration:\n"));
    t->family->print_devcfg(devcfg0, devcfg1, devcfg2, devcfg3);
}

/*
 * Translate virtual to physical address.
 */
static unsigned virt_to_phys(unsigned addr)
{
    if (addr >= 0x80000000 && addr < 0xA0000000)
        return addr - 0x80000000;
    if (addr >= 0xA0000000 && addr < 0xC0000000)
        return addr - 0xA0000000;
    return addr;
}

/*
 * Read data from memory.
 */
void target_read_block(target_t *t, unsigned addr,
    unsigned nwords, unsigned *data)
{
    if (! t->adapter->read_data) {
        conprintf(_("\nData reading not supported by the adapter.\n"));
        exit(1);
    }

    addr = virt_to_phys(addr);
    //fprintf(stderr, "target_read_block(addr = %x, nwords = %d)\n", addr, nwords);
    while (nwords > 0) {
        unsigned n = nwords;
        if (n > 256)
            n = 256;
        t->adapter->read_data(t->adapter, addr, n, data);
        addr += n<<2;
        data += n;
        nwords -= n;
    }
    //fprintf(stderr, "    done (addr = %x)\n", addr);
}

/*
 * Verify data.
 */
void target_verify_block(target_t *t, unsigned addr,
    unsigned nwords, unsigned *data)
{
    unsigned i, word, expected, block[512];

    //fprintf(stderr, "%s: addr=%08x, nwords=%u, data=%08x...\n", __func__, addr, nwords, data[0]);
    if (t->adapter->verify_data != 0) {
        t->adapter->verify_data(t->adapter, virt_to_phys(addr), nwords, data);
        return;
    }

    t->adapter->read_data(t->adapter, addr, nwords, block);
    for (i=0; i<nwords; i++) {
        expected = data [i];
        expected = t->family->word_mask(addr + (i<<2), expected);
        word = block [i];
        if (word != expected) {
            conprintf(_("\nerror at address %08X: file=%08X, mem=%08X\n"),
                addr + i*4, expected, word);
            exit(1);
        }
    }
}

/*
 * Erase all Flash memory.
 */
int target_erase(target_t *t)
{
    if (t->adapter->erase_chip) {
        conprintf(_("        Erase: "));
        fflush(stdout);
        t->adapter->erase_chip(t->adapter);
        conprintf(_("done\n"));
    }
    return 1;
}

/*
 * Test block for non 0xFFFFFFFF value
 */
static int target_test_empty_block(unsigned *data, unsigned nwords)
{
    while (nwords--)
        if (*data++ != 0xFFFFFFFF)
            return 0;
    return 1;
}

/*
 * Write to flash memory.
 */
void target_program_block(target_t *t, unsigned addr,
    unsigned nwords, unsigned *data)
{
    addr = virt_to_phys(addr);
    //fprintf(stderr, "target_program_block(addr = %x, nwords = %d)\n", addr, nwords);

    if (! t->adapter->program_block) {
        unsigned words_per_row = t->family->bytes_per_row / 4;
        while (nwords > 0) {
            unsigned n = nwords;
            if (n > words_per_row)
                n = words_per_row;
	    if (! target_test_empty_block(data, words_per_row))
                t->adapter->program_row(t->adapter, addr, data, words_per_row);
            addr += n<<2;
            data += n;
            nwords -= n;
        }
    }
    while (nwords > 0) {
        unsigned n = nwords;
        if (n > 256)
            n = 256;
        t->adapter->program_block(t->adapter, addr, data);
        addr += n<<2;
        data += n;
        nwords -= n;
    }
}

/*
 * Program the configuration registers.
 */
void target_program_devcfg(target_t *t, unsigned devcfg0,
        unsigned devcfg1, unsigned devcfg2, unsigned devcfg3)
{
    if (! t->family->devcfg_offset)
        return;

    unsigned addr = 0x1fc00000 + t->family->devcfg_offset;

    //fprintf(stderr, "%s: devcfg0-3 = %08x %08x %08x %08x\n", __func__, devcfg0, devcfg1, devcfg2, devcfg3);
    if (t->family->pe_version >= 0x0500) {
        /* Since pic32mz, the programming executive */
        t->adapter->program_quad_word(t->adapter, addr, devcfg3,
            devcfg2, devcfg1, devcfg0);
        return;
    }

    t->adapter->program_word(t->adapter, addr, devcfg3);
    t->adapter->program_word(t->adapter, addr + 4, devcfg2);
    t->adapter->program_word(t->adapter, addr + 8, devcfg1);
    t->adapter->program_word(t->adapter, addr + 12, devcfg0);
}
