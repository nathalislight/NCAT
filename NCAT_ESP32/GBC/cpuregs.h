

#ifndef __CPUREGS_H__

#define __CPUREGS_H__



#include "defs.h"
#include "cpu.h"

#define LB(r) ((r).b[LO][LO])
#define HB(r) ((r).b[LO][HI])
#define W__(r) ((r).w[LO])
#define DW(r) ((r).d)

#define GBC_A HB(cpu__GBC.af)
#define GBC_F LB(cpu__GBC.af)
#define GBC_B HB(cpu__GBC.bc)
#define GBC_C LB(cpu__GBC.bc)  
#define GBC_D HB(cpu__GBC.de)
#define GBC_E LB(cpu__GBC.de)
#define GBC_H HB(cpu__GBC.hl)
#define GBC_L LB(cpu__GBC.hl)

#define AF W__(cpu__GBC.af)
#define BC W__(cpu__GBC.bc)
#define DE W__(cpu__GBC.de)
#define HL W__(cpu__GBC.hl)

#define PC_ W__(cpu__GBC.pc)
#define SP_ W__(cpu__GBC.sp)

#define xAF DW(cpu__GBC.af)
#define xBC DW(cpu__GBC.bc)
#define xDE DW(cpu__GBC.de)
#define xHL DW(cpu__GBC.hl)

#define xPC DW(cpu__GBC.pc)
#define xSP DW(cpu__GBC.sp)

#define IMA cpu__GBC.ima
#define IME cpu__GBC.ime
#define IF R_IF
#define IE R_IE

#define FZ 0x80
#define FN 0x40
#define FH 0x20
#define FC 0x10
#define FL 0x0F /* low unused portion of flags */


#endif /* __CPUREGS_H__ */
