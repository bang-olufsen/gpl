/*
 * pwm.h
 *
 * Copyright (C) 2013, StreamUnlimited Engineering GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _ARCH_AM33XX_PWM_H
#define _ARCH_AM33XX_PWM_H

#define EHRPWM0_BASE_ADDR 0x48300200
#define EHRPWM1_BASE_ADDR 0x48302200
#define EHRPWM2_BASE_ADDR 0x48304200

#define PWMSS_BASE_ADDR 0x48300000

struct pwmss {
	unsigned int idver;
	unsigned int sysconfig;
	unsigned int clkconfig;
	unsigned int clkstatus;
};

 /* Time base module registers */
#define TBCTL                   0x00
#define TBPRD                   0x0A

#define TBCTL_RUN_MASK          (BIT(15) | BIT(14))
#define TBCTL_STOP_NEXT         0
#define TBCTL_STOP_ON_CYCLE     BIT(14)
#define TBCTL_FREE_RUN          (BIT(15) | BIT(14))
#define TBCTL_PRDLD_MASK        BIT(3)
#define TBCTL_PRDLD_SHDW        0
#define TBCTL_PRDLD_IMDT        BIT(3)
#define TBCTL_CLKDIV_MASK       (BIT(12) | BIT(11) | BIT(10) | BIT(9) | \
                                BIT(8) | BIT(7))
#define TBCTL_CTRMODE_MASK      (BIT(1) | BIT(0))
#define TBCTL_CTRMODE_UP        0
#define TBCTL_CTRMODE_DOWN      BIT(0)
#define TBCTL_CTRMODE_UPDOWN    BIT(1)
#define TBCTL_CTRMODE_FREEZE    (BIT(1) | BIT(0))

#define TBCTL_HSPCLKDIV_SHIFT   7
#define TBCTL_CLKDIV_SHIFT      10

#define CLKDIV_MAX              7
#define HSPCLKDIV_MAX           7
#define PERIOD_MAX              0xFFFF

/* compare module registers */
#define CMPA                    0x12
#define CMPB                    0x14

/* Action qualifier module registers */
#define AQCTLA                  0x16
#define AQCTLB                  0x18
#define AQSFRC                  0x1A
#define AQCSFRC                 0x1C

#define AQCTL_CBU_MASK          (BIT(9) | BIT(8))
#define AQCTL_CBU_FRCLOW        BIT(8)
#define AQCTL_CBU_FRCHIGH       BIT(9)
#define AQCTL_CBU_FRCTOGGLE     (BIT(9) | BIT(8))
#define AQCTL_CAU_MASK          (BIT(5) | BIT(4))
#define AQCTL_CAU_FRCLOW        BIT(4)
#define AQCTL_CAU_FRCHIGH       BIT(5)
#define AQCTL_CAU_FRCTOGGLE     (BIT(5) | BIT(4))
#define AQCTL_PRD_MASK          (BIT(3) | BIT(2))
#define AQCTL_PRD_FRCLOW        BIT(2)
#define AQCTL_PRD_FRCHIGH       BIT(3)
#define AQCTL_PRD_FRCTOGGLE     (BIT(3) | BIT(2))
#define AQCTL_ZRO_MASK          (BIT(1) | BIT(0))
#define AQCTL_ZRO_FRCLOW        BIT(0)
#define AQCTL_ZRO_FRCHIGH       BIT(1)
#define AQCTL_ZRO_FRCTOGGLE     (BIT(1) | BIT(0))

#define AQCTL_CHANA_POLNORMAL   (AQCTL_CAU_FRCLOW | AQCTL_PRD_FRCHIGH | \
                                AQCTL_ZRO_FRCHIGH)
#define AQCTL_CHANA_POLINVERSED (AQCTL_CAU_FRCHIGH | AQCTL_PRD_FRCLOW | \
                                AQCTL_ZRO_FRCLOW)
#define AQCTL_CHANB_POLNORMAL   (AQCTL_CBU_FRCLOW | AQCTL_PRD_FRCHIGH | \
                                AQCTL_ZRO_FRCHIGH)
#define AQCTL_CHANB_POLINVERSED (AQCTL_CBU_FRCHIGH | AQCTL_PRD_FRCLOW | \
                                AQCTL_ZRO_FRCLOW)

#define AQSFRC_RLDCSF_MASK      (BIT(7) | BIT(6))
#define AQSFRC_RLDCSF_ZRO       0
#define AQSFRC_RLDCSF_PRD       BIT(6)
#define AQSFRC_RLDCSF_ZROPRD    BIT(7)
#define AQSFRC_RLDCSF_IMDT      (BIT(7) | BIT(6))

#define AQCSFRC_CSFB_MASK       (BIT(3) | BIT(2))
#define AQCSFRC_CSFB_FRCDIS     0
#define AQCSFRC_CSFB_FRCLOW     BIT(2)
#define AQCSFRC_CSFB_FRCHIGH    BIT(3)
#define AQCSFRC_CSFB_DISSWFRC   (BIT(3) | BIT(2))
#define AQCSFRC_CSFA_MASK       (BIT(1) | BIT(0))
#define AQCSFRC_CSFA_FRCDIS     0
#define AQCSFRC_CSFA_FRCLOW     BIT(0)
#define AQCSFRC_CSFA_FRCHIGH    BIT(1)
#define AQCSFRC_CSFA_DISSWFRC   (BIT(1) | BIT(0))

#define NUM_PWM_CHANNEL         2       /* EHRPWM channels */

#ifndef __ASSEMBLY__

struct am33xx_pwm {
	u16 tbctl;
	u16 tbprd;
	u16 cmpa;
	u16 cmpb;
	u16 aqctla;
	u16 aqctlb;
	u16 aqsfrc;
	u16 aqcsfrc;
};

struct ehrpwm_chip {
	int polarity;
	int div;
	unsigned long period_cycles[NUM_PWM_CHANNEL];
};


#endif /* __ASSEMBLY__ */

#endif
