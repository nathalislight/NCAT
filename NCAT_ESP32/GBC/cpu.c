



#include "defs.h"
#include "regs.h"
#include "hw.h"
#include "cpu.h"
#include "mem.h"
#include "fastmem.h"
#include "cpuregs.h"
#include "cpucore.h"
#include "lcdc.h"
#include "debug.h"
#include <string.h>

#include "esp_attr.h"

#ifdef USE_ASM
#include "asm.h"
#endif


struct cpu_GBC cpu__GBC;




#define ZFLAG(n) ( (n) ? 0 : FZ )


#define PUSH(w) ( (SP_ -= 2), (writew(xSP, (w))) )
#define POP(w) ( ((w) = readw(SP_)), (xSP += 2) )


#define FETCH_OLD ( mbc.rmap[PC_>>12] \
? mbc.rmap[PC_>>12][PC_++] \
: mem_read(PC_++) )

#define FETCH (readb(PC_++))


#define INC(r) { ((r)++); \
GBC_F = (GBC_F & (FL|FC)) | incflag_table[(r)]; }

#define DEC(r) { ((r)--); \
GBC_F = (GBC_F & (FL|FC)) | decflag_table[(r)]; }

#define INCW(r) ( (r)++ )

#define DECW(r) ( (r)-- )

#define ADD(n) { \
W__(acc) = (un16)GBC_A + (un16)(n); \
GBC_F = (ZFLAG(LB(acc))) \
| (FH & ((GBC_A ^ (n) ^ LB(acc)) << 1)) \
| (HB(acc) << 4); \
GBC_A = LB(acc); }

#define ADC(n) { \
W__(acc) = (un16)GBC_A + (un16)(n) + (un16)((GBC_F&FC)>>4); \
GBC_F = (ZFLAG(LB(acc))) \
| (FH & ((GBC_A ^ (n) ^ LB(acc)) << 1)) \
| (HB(acc) << 4); \
GBC_A = LB(acc); }

#define ADDW(n) { \
DW(acc) = (un32)HL + (un32)(n); \
GBC_F = (GBC_F & (FZ)) \
| (FH & ((GBC_H ^ ((n)>>8) ^ HB(acc)) << 1)) \
| (acc.b[HI][LO] << 4); \
HL = W__(acc); }

#define ADDSP(n) { \
DW(acc) = (un32)SP_ + (un32)(n8)(n); \
GBC_F = (FH & (((SP_>>8) ^ ((n)>>8) ^ HB(acc)) << 1)) \
| (acc.b[HI][LO] << 4); \
SP_ = W__(acc); }

#define LDHLSP(n) { \
DW(acc) = (un32)SP_ + (un32)(n8)(n); \
GBC_F = (FH & (((SP_>>8) ^ ((n)>>8) ^ HB(acc)) << 1)) \
| (acc.b[HI][LO] << 4); \
HL = W__(acc); }

#define CP(n) { \
W__(acc) = (un16)GBC_A - (un16)(n); \
GBC_F = FN \
| (ZFLAG(LB(acc))) \
| (FH & ((GBC_A ^ (n) ^ LB(acc)) << 1)) \
| ((un8)(-(n8)HB(acc)) << 4); }

#define SUB(n) { CP((n)); GBC_A = LB(acc); }

#define SBC(n) { \
W__(acc) = (un16)GBC_A - (un16)(n) - (un16)((GBC_F&FC)>>4); \
GBC_F = FN \
| (ZFLAG((n8)LB(acc))) \
| (FH & ((GBC_A ^ (n) ^ LB(acc)) << 1)) \
| ((un8)(-(n8)HB(acc)) << 4); \
GBC_A = LB(acc); }

#define AND(n) { GBC_A &= (n); \
GBC_F = ZFLAG(GBC_A) | FH; }

#define XOR(n) { GBC_A ^= (n); \
GBC_F = ZFLAG(GBC_A); }

#define OR(n) { GBC_A |= (n); \
GBC_F = ZFLAG(GBC_A); }

#define RLCA(r) { (r) = ((r)>>7) | ((r)<<1); \
GBC_F = (((r)&0x01)<<4); }

#define RRCA(r) { (r) = ((r)<<7) | ((r)>>1); \
GBC_F = (((r)&0x80)>>3); }

#define RLA(r) { \
LB(acc) = (((r)&0x80)>>3); \
(r) = ((r)<<1) | ((GBC_F&FC)>>4); \
GBC_F = LB(acc); }

#define RRA(r) { \
LB(acc) = (((r)&0x01)<<4); \
(r) = ((r)>>1) | ((GBC_F&FC)<<3); \
GBC_F = LB(acc); }

#define RLC(r) { RLCA(r); GBC_F |= ZFLAG(r); }
#define RRC(r) { RRCA(r); GBC_F |= ZFLAG(r); }
#define RL(r) { RLA(r); GBC_F |= ZFLAG(r); }
#define RR(r) { RRA(r); GBC_F |= ZFLAG(r); }

#define SLA(r) { \
LB(acc) = (((r)&0x80)>>3); \
(r) <<= 1; \
GBC_F = ZFLAG((r)) | LB(acc); }

#define SRA(r) { \
LB(acc) = (((r)&0x01)<<4); \
(r) = (un8)(((n8)(r))>>1); \
GBC_F = ZFLAG((r)) | LB(acc); }

#define SRL(r) { \
LB(acc) = (((r)&0x01)<<4); \
(r) >>= 1; \
GBC_F = ZFLAG((r)) | LB(acc); }

#define CPL(r) { \
(r) = ~(r); \
GBC_F |= (FH|FN); }

#define SCF { GBC_F = (GBC_F & (FZ)) | FC; }

#define CCF { GBC_F = (GBC_F & (FZ|FC)) ^ FC; }

#define DAA { \
GBC_A += (LB(acc) = daa_table[((((int)GBC_F)&0x70)<<4) | GBC_A]); \
GBC_F = (GBC_F & (FN)) | ZFLAG(GBC_A) | daa_carry_table[LB(acc)>>2]; }

#define SWAP(r) { \
(r) = swap_table[(r)]; \
GBC_F = ZFLAG((r)); }

#define BIT(n,r) { GBC_F = (GBC_F & FC) | ZFLAG(((r) & (1 << (n)))) | FH; }
#define RES(n,r) { (r) &= ~(1 << (n)); }
#define SET(n,r) { (r) |= (1 << (n)); }

#define CB_REG_CASES(r, n) \
case 0x00|(n): RLC(r); break; \
case 0x08|(n): RRC(r); break; \
case 0x10|(n): RL(r); break; \
case 0x18|(n): RR(r); break; \
case 0x20|(n): SLA(r); break; \
case 0x28|(n): SRA(r); break; \
case 0x30|(n): SWAP(r); break; \
case 0x38|(n): SRL(r); break; \
case 0x40|(n): BIT(0, r); break; \
case 0x48|(n): BIT(1, r); break; \
case 0x50|(n): BIT(2, r); break; \
case 0x58|(n): BIT(3, r); break; \
case 0x60|(n): BIT(4, r); break; \
case 0x68|(n): BIT(5, r); break; \
case 0x70|(n): BIT(6, r); break; \
case 0x78|(n): BIT(7, r); break; \
case 0x80|(n): RES(0, r); break; \
case 0x88|(n): RES(1, r); break; \
case 0x90|(n): RES(2, r); break; \
case 0x98|(n): RES(3, r); break; \
case 0xA0|(n): RES(4, r); break; \
case 0xA8|(n): RES(5, r); break; \
case 0xB0|(n): RES(6, r); break; \
case 0xB8|(n): RES(7, r); break; \
case 0xC0|(n): SET(0, r); break; \
case 0xC8|(n): SET(1, r); break; \
case 0xD0|(n): SET(2, r); break; \
case 0xD8|(n): SET(3, r); break; \
case 0xE0|(n): SET(4, r); break; \
case 0xE8|(n): SET(5, r); break; \
case 0xF0|(n): SET(6, r); break; \
case 0xF8|(n): SET(7, r); break;


#define ALU_CASES(base, imm, op, label) \
case (imm): b = FETCH; goto label; \
case (base): b = GBC_B; goto label; \
case (base)+1: b = GBC_C; goto label; \
case (base)+2: b = GBC_D; goto label; \
case (base)+3: b = GBC_E; goto label; \
case (base)+4: b = GBC_H; goto label; \
case (base)+5: b = GBC_L; goto label; \
case (base)+6: b = readb(HL); goto label; \
case (base)+7: b = GBC_A; \
label: op(b); break;








#define JR ( PC_ += 1+(n8)readb(PC_) )
#define JP ( PC_ = readw(PC_) )

#define CALL ( PUSH(PC_+2), JP )

#define NOJR ( clen--, PC_++ )
#define NOJP ( clen--, PC_+=2 )
#define NOCALL ( clen-=3, PC_+=2 )
#define NORET ( clen-=3 )

#define RST(n) { PUSH(PC_); PC_ = (n); }

#define RET ( POP(PC_) )

#define EI ( IMA = 1 )
#define DI ( cpu__GBC.halt = IMA = IME = 0 )



#define PRE_INT ( DI, PUSH(PC_) )
#define THROW_INT(n) ( (IF &= ~(1<<(n))), (PC_ = 0x40+((n)<<3)) )




void cpu_reset()
{
	memset(&cpu__GBC, 0, sizeof(cpu__GBC));
	cpu__GBC.speed = 0;
	cpu__GBC.halt = 0;
	cpu__GBC.div = 0;
	cpu__GBC.tim = 0;
	cpu__GBC.lcdc = 40;

	IME = 0;
	IMA = 0;
	
//--------------------------------------------------------------------------------
	if (bootromLoaded==0) {
		PC_ = 0x0100; //for direct cartridge boot
	} else {
    PC_ = 0x0000; //for bootROM boot
	}
		//printf("PC REG is set to: %d\n",PC_);
//--------------------------------------------------------------------------------
///  PC_ = 0x0100;
  /*if (hw.gbbootromdata==NULL) {
		PC_ = 0x0100; //for direct cartridge boot
	} else {
		PC_ = 0x0000; //for bootROM boot
	}*/
//--------------------------------------------------------------------------------

	SP_ = 0xFFFE;
	AF = 0x01B0;
	BC = 0x0013;
	DE = 0x00D8;
	HL = 0x014D;
	
	if (hw.cgb) GBC_A = 0x11;
	if (hw.gba) GBC_B = 0x01;
}

/* cnt - time to emulate, expressed in 2MHz units in
	single-speed and 4MHz units in double speed mode
	
	FIXME: employ common unit to drive whatever_advance(),
	(double-speed machine cycles (2MHz) is a good candidate)
	handle differences in place */
void div_advance(int cnt)
{
	cpu__GBC.div += (cnt<<1);
	if (cpu__GBC.div >= 256)
	{
		R_DIV += (cpu__GBC.div >> 8);
		cpu__GBC.div &= 0xff;
	}
}

/* cnt - time to emulate, expressed in 2MHz units in
	single-speed and 4MHz units in double speed mode
	
	FIXME: employ common unit to drive whatever_advance(),
	(double-speed machine cycles (2MHz) is a good candidate)
	handle differences in place */
void timer_advance(int cnt)
{
	int unit, tima;
	
	if (!(R_TAC & 0x04)) return;

	unit = ((-R_TAC) & 3) << 1;
	cpu__GBC.tim += (cnt<<unit);

	if (cpu__GBC.tim >= 512)
	{
		tima = R_TIMA + (cpu__GBC.tim >> 9);
		cpu__GBC.tim &= 0x1ff;
		if (tima >= 256)
		{
			hw_interrupt(IF_TIMER, IF_TIMER);
			hw_interrupt(0, IF_TIMER);
		}
		while (tima >= 256)
			tima = tima - 256 + R_TMA;
		R_TIMA = tima;
	}
}

void lcdc_advance(int cnt)
{
	cpu__GBC.lcdc -= cnt;
	if (cpu__GBC.lcdc <= 0) lcdc_trans();
}

void sound_advance(int cnt)
{
	cpu__GBC.snd += cnt;
}

void cpu_timers(int cnt)
{
	div_advance(cnt << cpu__GBC.speed);
	timer_advance(cnt << cpu__GBC.speed);
	lcdc_advance(cnt);
	sound_advance(cnt);
}

int cpu_idle(int max)
{
	int cnt, unit;

	if (!(cpu__GBC.halt && IME)) return 0;
	if (R_IF & R_IE)
	{
		cpu__GBC.halt = 0;
		return 0;
	}

	/* Make sure we don't miss lcdc status events! */
	if ((R_IE & (IF_VBLANK | IF_STAT)) && (max > cpu__GBC.lcdc))
		max = cpu__GBC.lcdc;
	
	/* If timer interrupt cannot happen, this is very simple! */
	if (!((R_IE & IF_TIMER) && (R_TAC & 0x04)))
	{
		cpu_timers(max);
		return max;
	}

	/* Figure out when the next timer interrupt will happen */
	unit = ((-R_TAC) & 3) << 1;
	cnt = (511 - cpu__GBC.tim + (1<<unit)) >> unit;
	cnt += (255 - R_TIMA) << (9 - unit);

	if (max < cnt)
		cnt = max;
	
	cpu_timers(cnt);
	return cnt;
}

#ifndef ASM_CPU_EMULATE

extern int debug_trace;







//--------------------------------------------------------------------------------
#define __ADD ADD(b);
#define __ADC ADC(b);
#define __SUB SUB(b);
#define __SBC SBC(b);
#define __AND AND(b);
#define __XOR XOR(b);
#define  __OR OR(b);
#define __CP CP(b);
//--------------------------------------------------------------------------------




IRAM_ATTR
int cpu_emulate(int cycles)
{
	int i;
	byte op, cbop;
	int clen;
	static union reg acc;
	static byte b;
	static word w;

	i = cycles;
next:
	if ((clen = cpu_idle(i)))
	{
		i -= clen;
		if (i > 0) goto next;
		return cycles-i;
	}

	if (IME && (IF & IE))
	{
		PRE_INT;
		switch ((byte)(IF & IE))
		{
		case 0x01: case 0x03: case 0x05: case 0x07:
		case 0x09: case 0x0B: case 0x0D: case 0x0F:
		case 0x11: case 0x13: case 0x15: case 0x17:
		case 0x19: case 0x1B: case 0x1D: case 0x1F:
			THROW_INT(0); break;
		case 0x02: case 0x06: case 0x0A: case 0x0E:
		case 0x12: case 0x16: case 0x1A: case 0x1E:
			THROW_INT(1); break;
		case 0x04: case 0x0C: case 0x14: case 0x1C:
			THROW_INT(2); break;
		case 0x08: case 0x18:
			THROW_INT(3); break;
		case 0x10:
			THROW_INT(4); break;
		}
	}
	IME = IMA;
	
	if (debug_trace) debug_disassemble(PC_, 1);
	op = FETCH;
	clen = cycles_table[op];





/*
		ALU_CASES(0x80, 0xC6, ADD, __ADD)
		ALU_CASES(0x88, 0xCE, ADC, __ADC)
		ALU_CASES(0x90, 0xD6, SUB, __SUB)
		ALU_CASES(0x98, 0xDE, SBC, __SBC)
		ALU_CASES(0xA0, 0xE6, AND, __AND)
		ALU_CASES(0xA8, 0xEE, XOR, __XOR)
		ALU_CASES(0xB0, 0xF6, OR, __OR)
		ALU_CASES(0xB8, 0xFE, CP, __CP)

    
#define ALU_CASES(base, imm, op, label) \
case (imm): b = FETCH; goto label; \
case (base): b = GBC_B; goto label; \
case (base)+1: b = GBC_C; goto label; \
case (base)+2: b = GBC_D; goto label; \
case (base)+3: b = GBC_E; goto label; \
case (base)+4: b = GBC_H; goto label; \
case (base)+5: b = GBC_L; goto label; \
case (base)+6: b = readb(HL); goto label; \
case (base)+7: b = GBC_A; \
label: op(b); break;
*/








    
    
    
    			

    
    
    			



















    


switch(op>>4) {
  case 0x04:  
	  switch(op){
      case 0x40: /* LD GBC_B,GBC_B */
        break;
      case 0x41: /* LD GBC_B,GBC_C */
      	GBC_B = GBC_C; break;
      case 0x42: /* LD GBC_B,GBC_D */
      	GBC_B = GBC_D; break;
      case 0x43: /* LD GBC_B,GBC_E */
      	GBC_B = GBC_E; break;
      case 0x44: /* LD GBC_B,GBC_H */
      	GBC_B = GBC_H; break;
      case 0x45: /* LD GBC_B,GBC_L */
      	GBC_B = GBC_L; break;
      case 0x46: /* LD GBC_B,(HL) */
      	GBC_B = readb(xHL); break;
      case 0x47: /* LD GBC_B,GBC_A */
      	GBC_B = GBC_A; break;
      case 0x48: /* LD GBC_C,GBC_B */
      	GBC_C = GBC_B; break;
      case 0x49: /* LD GBC_C,GBC_C */
        break;
      case 0x4A: /* LD GBC_C,GBC_D */
      	GBC_C = GBC_D; break;
      case 0x4B: /* LD GBC_C,GBC_E */
      	GBC_C = GBC_E; break;
      case 0x4C: /* LD GBC_C,GBC_H */
      	GBC_C = GBC_H; break;
      case 0x4D: /* LD GBC_C,GBC_L */
      	GBC_C = GBC_L; break;
      case 0x4E: /* LD GBC_C,(HL) */
      	GBC_C = readb(xHL); break;
      case 0x4F: /* LD GBC_C,GBC_A */
      	GBC_C = GBC_A; break;
    }  
		break;    
  case 0x05:
    switch(op){
      case 0x50: /* LD GBC_D,GBC_B */
      	GBC_D = GBC_B; break;
      case 0x51: /* LD GBC_D,GBC_C */
      	GBC_D = GBC_C; break;
	    case 0x52: /* LD GBC_D,GBC_D */
        break;
      case 0x53: /* LD GBC_D,GBC_E */
      	GBC_D = GBC_E; break;
      case 0x54: /* LD GBC_D,GBC_H */
      	GBC_D = GBC_H; break;
      case 0x55: /* LD GBC_D,GBC_L */
      	GBC_D = GBC_L; break;
      case 0x56: /* LD GBC_D,(HL) */
      	GBC_D = readb(xHL); break;
      case 0x57: /* LD GBC_D,GBC_A */
      	GBC_D = GBC_A; break;
       case 0x58: /* LD GBC_E,GBC_B */
      	GBC_E = GBC_B; break;
      case 0x59: /* LD GBC_E,GBC_C */
      	GBC_E = GBC_C; break;
      case 0x5A: /* LD GBC_E,GBC_D */
      	GBC_E = GBC_D; break;
    	case 0x5B: /* LD GBC_E,GBC_E */
        break;
      case 0x5C: /* LD GBC_E,GBC_H */
      	GBC_E = GBC_H; break;
      case 0x5D: /* LD GBC_E,GBC_L */
      	GBC_E = GBC_L; break;
      case 0x5E: /* LD GBC_E,(HL) */
      	GBC_E = readb(xHL); break;
      case 0x5F: /* LD GBC_E,GBC_A */
      	GBC_E = GBC_A; break;
    }
		break;    
  case 0x06:
	  switch(op){
    	case 0x60: /* LD GBC_H,GBC_B */
    		GBC_H = GBC_B; break;
    	case 0x61: /* LD GBC_H,GBC_C */
    		GBC_H = GBC_C; break;
    	case 0x62: /* LD GBC_H,GBC_D */
    		GBC_H = GBC_D; break;
    	case 0x63: /* LD GBC_H,GBC_E */
    		GBC_H = GBC_E; break;
	    case 0x64: /* LD GBC_H,GBC_H */
    		break;
    	case 0x65: /* LD GBC_H,GBC_L */
    		GBC_H = GBC_L; break;
    	case 0x66: /* LD GBC_H,(HL) */
    		GBC_H = readb(xHL); break;
    	case 0x67: /* LD GBC_H,GBC_A */
    		GBC_H = GBC_A; break;

    	case 0x68: /* LD GBC_L,GBC_B */
    		GBC_L = GBC_B; break;
    	case 0x69: /* LD GBC_L,GBC_C */
    		GBC_L = GBC_C; break;
    	case 0x6A: /* LD GBC_L,GBC_D */
    		GBC_L = GBC_D; break;
    	case 0x6B: /* LD GBC_L,GBC_E */
    		GBC_L = GBC_E; break;
    	case 0x6C: /* LD GBC_L,GBC_H */
    		GBC_L = GBC_H; break;
	    case 0x6D: /* LD GBC_L,GBC_L */
    		break;
    	case 0x6E: /* LD GBC_L,(HL) */
    		GBC_L = readb(xHL); break;
    	case 0x6F: /* LD GBC_L,GBC_A */
    		GBC_L = GBC_A; break;

    }
		break;    
  case 0x07:
	  switch(op){
    	case 0x70: /* LD (HL),GBC_B */
    		b = GBC_B; goto __LD_HL;
    	case 0x71: /* LD (HL),GBC_C */
    		b = GBC_C; goto __LD_HL;
    	case 0x72: /* LD (HL),GBC_D */
    		b = GBC_D; goto __LD_HL;
    	case 0x73: /* LD (HL),GBC_E */
    		b = GBC_E; goto __LD_HL;
    	case 0x74: /* LD (HL),GBC_H */
    		b = GBC_H; goto __LD_HL;
    	case 0x75: /* LD (HL),GBC_L */
    		b = GBC_L; goto __LD_HL;
    	case 0x76: /* HALT */
    		cpu__GBC.halt = 1;
    		break;
    	case 0x77: /* LD (HL),GBC_A */
    		b = GBC_A;
    	__LD_HL:
    		writeb(xHL,b);
    		break;    			
    	case 0x78: /* LD GBC_A,GBC_B */
    		GBC_A = GBC_B; break;
    	case 0x79: /* LD GBC_A,GBC_C */
    		GBC_A = GBC_C; break;
    	case 0x7A: /* LD GBC_A,GBC_D */
    		GBC_A = GBC_D; break;
    	case 0x7B: /* LD GBC_A,GBC_E */
    		GBC_A = GBC_E; break;
    	case 0x7C: /* LD GBC_A,GBC_H */
    		GBC_A = GBC_H; break;
    	case 0x7D: /* LD GBC_A,GBC_L */
    		GBC_A = GBC_L; break;
    	case 0x7E: /* LD GBC_A,(HL) */
    		GBC_A = readb(xHL); break;
    	case 0x7F: /* LD GBC_A,GBC_A */
    		break;
    }  
		break;    


  case 0x00:
	  switch(op){
  	  case 0x00: /* NOP */
        break;
    	case 0x01: /* LD BC,imm */
    		BC = readw(xPC); PC_ += 2; break;
    	case 0x02: /* LD (BC),GBC_A */
    		writeb(xBC, GBC_A); break;
    	case 0x03: // INC BC 
    		INCW(BC); break;
    	case 0x04: /* INC GBC_B */
    		INC(GBC_B); break;
      case 0x05: /* DEC GBC_B */
    		DEC(GBC_B); break;
     	case 0x06: /* LD GBC_B,imm */
     		GBC_B = FETCH; break;
			case 0x07: // RLCA 
				RLCA(GBC_A); break;
     	case 0x08: /* LD (imm),SP_ */
     		writew(readw(xPC), SP_); PC_ += 2; break;
    	case 0x09: /* ADD HL,BC */
    		w = BC; goto __ADDW;
    	case 0x0A: /* LD GBC_A,(BC) */
   	  	GBC_A = readb(xBC); break;
    	case 0x0B: /* DEC BC */
    		DECW(BC); break;
	    case 0x0C: /* INC GBC_C */
    		INC(GBC_C); break;
    	case 0x0D: /* DEC GBC_C */
    		DEC(GBC_C); break;
     	case 0x0E: /* LD GBC_C,imm */
     		GBC_C = FETCH; break;
    	case 0x0F: // RRCA 
				RRCA(GBC_A); break;

    } 
		break;    

  case 0x01:
  	switch(op){
    	case 0x10: /* STOP */
    		PC_++;
    		if (R_KEY1 & 1)
    		{
    			cpu__GBC.speed = cpu__GBC.speed ^ 1;
    			R_KEY1 = (R_KEY1 & 0x7E) | (cpu__GBC.speed << 7);
    			break;
    		}
    		/* NOTE - we do not implement dmg STOP whatsoever */
    		break;
    	case 0x11: /* LD DE,imm */
    		DE = readw(xPC); PC_ += 2; break;
    	case 0x12: /* LD (DE),GBC_A */
    		writeb(xDE, GBC_A); break;
    	case 0x13: // INC DE 
    		INCW(DE); break;
			case 0x14: /* INC GBC_D */
    		INC(GBC_D); break;
    	case 0x15: /* DEC GBC_D */
    		DEC(GBC_D); break;
    	case 0x16: /* LD GBC_D,imm */
    		GBC_D = FETCH; break;
	    case 0x17: // RLA 
	    	RLA(GBC_A); break;
    	case 0x18: /* JR */
    	__JR:
    		JR; break;
    	case 0x19: /* ADD HL,DE */
    		w = DE; goto __ADDW;
    	case 0x1A: /* LD GBC_A,(DE) */
    		GBC_A = readb(xDE); break;
    	case 0x1B: /* DEC DE */
    		DECW(DE); break;
    	case 0x1C: /* INC GBC_E */
    		INC(GBC_E); break;
    	case 0x1D: /* DEC GBC_E */
    		DEC(GBC_E); break;
    	case 0x1E: /* LD GBC_E,imm */
    		GBC_E = FETCH; break;
	    case 0x1F: // RRA 
	    	RRA(GBC_A); break;
    }  
		break;    
  case 0x02:
 	  switch(op){
    	case 0x20: /* JR NZ */
    		if (!(GBC_F&FZ)) goto __JR; NOJR; break;
    	case 0x21: /* LD HL,imm */
    		HL = readw(xPC); PC_ += 2; break;
	    case 0x22: // LDI (HL),GBC_A 
	    	writeb(xHL, GBC_A); HL++; break;
    	case 0x23: // INC HL 
    		INCW(HL); break;
    	case 0x24: /* INC GBC_H */
    		INC(GBC_H); break;
    	case 0x25: /* DEC GBC_H */
    		DEC(GBC_H); break;
    	case 0x26: /* LD GBC_H,imm */
    		GBC_H = FETCH; break;
    	case 0x27: /* DAA */
#if 0
		DAA
#else
		{
			//http://forums.nesdev.com/viewtopic.php?t=9088

			int a = GBC_A;
			if (!(GBC_F & FN))
			{
				if ((GBC_F & FH) || ((a & 0x0f) > 9)) a += 0x06;

				if ((GBC_F & FC) || (a > 0x9f)) a += 0x60;
			}
			else
			{
				if (GBC_F & FH)	a = (a - 6) & 0xff;

				if (GBC_F & FC) a -= 0x60;
			}

			GBC_F &= ~(FH | FZ);

			if (a & 0x100) GBC_F |= FC;

			a &= 0xff;

			if (!a) GBC_F |= FZ;

			GBC_A = (byte)a;
		}
#endif
        break;
    	case 0x28: /* JR Z */
    		if (GBC_F&FZ) goto __JR; NOJR; break;
    	case 0x29: /* ADD HL,HL */
    		w = HL;
    	__ADDW:
    		ADDW(w);
    		break;
	    case 0x2A: // LDI GBC_A,(HL) 
	    	GBC_A = readb(xHL); HL++; break;   
    	case 0x2B: /* DEC HL */
    		DECW(HL); break;
    	case 0x2C: /* INC GBC_L */
    		INC(GBC_L); break;
    	case 0x2D: /* DEC GBC_L */
    		DEC(GBC_L); break;
    	case 0x2E: /* LD GBC_L,imm */
    		GBC_L = FETCH; break;
    	case 0x2F: /* CPL */
    		CPL(GBC_A); break;
    } 
		break;    
  case 0x03:
 	  switch(op){
    	case 0x30: /* JR NC */
    		if (!(GBC_F&FC)) goto __JR; NOJR; break;
    	case 0x31: /* LD SP_,imm */
    		SP_ = readw(xPC); PC_ += 2; break;
	    case 0x32: // LDD (HL),GBC_A 
	    	writeb(xHL, GBC_A); HL--; break;
    	case 0x33: // INC SP_ 
    		INCW(SP_); break;
    	case 0x34: /* INC (HL) */
    		b = readb(xHL);
    		INC(b);
    		writeb(xHL, b);
    		break;
    	case 0x35: /* DEC (HL) */
    		b = readb(xHL);
    		DEC(b);
    		writeb(xHL, b);
    		break;
    	case 0x36: /* LD (HL),imm */
    		b = FETCH; writeb(xHL, b); break;
    	case 0x37: /* SCF */
    		SCF; break;
    	case 0x38: /* JR GBC_C */
    		if (GBC_F&FC) goto __JR; NOJR; break;
    	case 0x39: /* ADD HL,SP_ */
    		w = SP_; goto __ADDW;
	    case 0x3A: // LDD GBC_A,(HL) 
	    	GBC_A = readb(xHL); HL--; break;
    	case 0x3B: /* DEC SP_ */
      		DECW(SP_); break;
    	case 0x3C: /* INC GBC_A */
    		INC(GBC_A); break;
    	case 0x3D: /* DEC GBC_A */
    		DEC(GBC_A); break;
       	case 0x3E: /* LD GBC_A,imm */
    		GBC_A = FETCH; break;
    	case 0x3F: /* CCF */
    		CCF; break;
    }
		break;    


  case 0x08:
	  switch(op){
      case 0x80:
        b = GBC_B;
        __ADD
        break;
      case 0x81:
        b = GBC_C;
        __ADD
        break;
      case 0x82:
        b = GBC_D;
        __ADD
        break;
      case 0x83:
        b = GBC_E;
        __ADD
        break;
      case 0x84:
        b = GBC_H;
        __ADD
        break;
      case 0x85:
        b = GBC_L;
        __ADD
        break;
      case 0x86:
        b = readb(HL);
        __ADD
        break;
      case 0x87:
        b = GBC_A;
        __ADD
        break;
      //----------------------------------------
      case 0x88:
        b = GBC_B;
        __ADC
        break;
      case 0x89:
        b = GBC_C;
        __ADC
        break;
      case 0x8A:
        b = GBC_D;
        __ADC
        break;
      case 0x8B:
        b = GBC_E;
        __ADC
        break;
      case 0x8C:
        b = GBC_H;
        __ADC
        break;
      case 0x8D:
        b = GBC_L;
        __ADC
        break;
      case 0x8E:
        b = readb(HL);
        __ADC
        break;
      case 0x8F:
        b = GBC_A;
        __ADC
        break;
    }    
		break;    
  case 0x09:
	  switch(op){
      case 0x90:
        b = GBC_B;
        __SUB
        break;
      case 0x91:
        b = GBC_C;
        __SUB
        break;
      case 0x92:
        b = GBC_D;
        __SUB
        break;
      case 0x93:
        b = GBC_E;
        __SUB
        break;
      case 0x94:
        b = GBC_H;
        __SUB
        break;
      case 0x95:
        b = GBC_L;
        __SUB
        break;
      case 0x96:
        b = readb(HL);
        __SUB
        break;
      case 0x97:
        b = GBC_A;
        __SUB
        break;
      //----------------------------------------

      case 0x98:
        b = GBC_B;
        __SBC
        break;
      case 0x99:
        b = GBC_C;
        __SBC
        break;
      case 0x9A:
        b = GBC_D;
        __SBC
        break;
      case 0x9B:
        b = GBC_E;
        __SBC
        break;
      case 0x9C:
        b = GBC_H;
        __SBC
        break;
      case 0x9D:
        b = GBC_L;
        __SBC
        break;
      case 0x9E:
        b = readb(HL);
        __SBC
        break;
      case 0x9F:
        b = GBC_A;
        __SBC
        break;

    }    
		break;    
  case 0x0A:
	  switch(op){
      case 0xA0:
        b = GBC_B;
        __AND
        break;
      case 0xA1:
        b = GBC_C;
        __AND
        break;
      case 0xA2:
        b = GBC_D;
        __AND
        break;
      case 0xA3:
        b = GBC_E;
        __AND
        break;
      case 0xA4:
        b = GBC_H;
        __AND
        break;
      case 0xA5:
        b = GBC_L;
        __AND
        break;
      case 0xA6:
        b = readb(HL);
        __AND
        break;
      case 0xA7:
        b = GBC_A;
        __AND
        break;
      //----------------------------------------
      case 0xA8:
        b = GBC_B;
        __XOR
        break;
      case 0xA9:
        b = GBC_C;
        __XOR
        break;
      case 0xAA:
        b = GBC_D;
        __XOR
        break;
      case 0xAB:
        b = GBC_E;
        __XOR
        break;
      case 0xAC:
        b = GBC_H;
        __XOR
        break;
      case 0xAD:
        b = GBC_L;
        __XOR
        break;
      case 0xAE:
        b = readb(HL);
        __XOR
        break;
      case 0xAF:
        b = GBC_A;
        __XOR
        break;

    }    
		break;    
  case 0x0B:
	  switch(op){
      case 0xB0:
        b = GBC_B;
        __OR
        break;
      case 0xB1:
        b = GBC_C;
        __OR
        break;
      case 0xB2:
        b = GBC_D;
        __OR
        break;
      case 0xB3:
        b = GBC_E;
        __OR
        break;
      case 0xB4:
        b = GBC_H;
        __OR
        break;
      case 0xB5:
        b = GBC_L;
        __OR
        break;
      case 0xB6:
        b = readb(HL);
        __OR
        break;
      case 0xB7:
        b = GBC_A;
        __OR
        break;
      case 0xB8:
        b = GBC_B;
        __CP
        break;
      case 0xB9:
        b = GBC_C;
        __CP
        break;
      case 0xBA:
        b = GBC_D;
        __CP
        break;
      case 0xBB:
        b = GBC_E;
        __CP
        break;
      case 0xBC:
        b = GBC_H;
        __CP
        break;
      case 0xBD:
        b = GBC_L;
        __CP
        break;
      case 0xBE:
        b = readb(HL);
        __CP
        break;
      case 0xBF:
        b = GBC_A;
        __CP
        break;			
    }    
		break;    
  case 0x0C:
	  switch(op){
    	case 0xC0: /* RET NZ */
    		if (!(GBC_F&FZ)) goto __RET; NORET; break;
    	case 0xC1: /* POP BC */
    		POP(BC); break;
    	case 0xC2: /* JP NZ */
    		if (!(GBC_F&FZ)) goto __JP; NOJP; break;
    	case 0xC3: /* JP */
    	__JP:
    		JP; break;
    	case 0xC4: /* CALL NZ */
    		if (!(GBC_F&FZ)) goto __CALL; NOCALL; break;
    	case 0xC5: /* PUSH BC */
    		PUSH(BC); break;
      case 0xC6:
        b = FETCH;
        __ADD
        break;
    	case 0xC7: /* RST 0 */
    		b = 0x00; goto __RST;
    	case 0xC8: /* RET Z */
    		if (GBC_F&FZ) goto __RET; NORET; break;
    	case 0xC9: /* RET */
    	__RET:
    		RET; break;
    	case 0xCA: /* JP Z */
    		if (GBC_F&FZ) goto __JP; NOJP; break;
	    case 0xCB: /* CB prefix */
	    	cbop = FETCH;
	    	clen = cb_cycles_table[cbop];
	    	switch (cbop)
	    	{
	    		CB_REG_CASES(GBC_B, 0);
	    		CB_REG_CASES(GBC_C, 1);
	    		CB_REG_CASES(GBC_D, 2);
	    		CB_REG_CASES(GBC_E, 3);
	    		CB_REG_CASES(GBC_H, 4);
	    		CB_REG_CASES(GBC_L, 5);
	    		CB_REG_CASES(GBC_A, 7);
	    	default:
	    		b = readb(xHL);
	    		switch(cbop)
	    		{
	    			CB_REG_CASES(b, 6);
	    		}
	    		if ((cbop & 0xC0) != 0x40) /* exclude BIT */
	    			writeb(xHL, b);
	    		break;
	    	}
	    	break;
    	case 0xCC: /* CALL Z */
    		if (GBC_F&FZ) goto __CALL; NOCALL; break;
    	case 0xCD: /* CALL */
    	__CALL:
    		CALL; break;
      case 0xCE:
        b = FETCH;
        __ADC
        break;
    	case 0xCF: /* RST 8 */
    		b = 0x08; goto __RST;


    }    
		break;    
  case 0x0D:
	  switch(op){
    	case 0xD0: /* RET NC */
    		if (!(GBC_F&FC)) goto __RET; NORET; break;
    	case 0xD1: /* POP DE */
    		POP(DE); break;
    	case 0xD2: /* JP NC */
    		if (!(GBC_F&FC)) goto __JP; NOJP; break;

    	case 0xD3: // INVALID OPCODE!
			  break;
			  


    	case 0xD4: /* CALL NC */
    		if (!(GBC_F&FC)) goto __CALL; NOCALL; break;

    	case 0xD5: /* PUSH DE */
    		PUSH(DE); break;
      case 0xD6:
        b = FETCH;
        __SUB
        break;
    	case 0xD7: /* RST 10 */
    		b = 0x10; goto __RST;
    	case 0xD8: /* RET GBC_C */
    		if (GBC_F&FC) goto __RET; NORET; break;
    	case 0xD9: /* RETI */
    		IME = IMA = 1; goto __RET;
    	case 0xDA: /* JP GBC_C */
    		if (GBC_F&FC) goto __JP; NOJP; break;

    	case 0xDB: // INVALID OPCODE!
			  break;

    	case 0xDC: /* CALL GBC_C */
    		if (GBC_F&FC) goto __CALL; NOCALL; break;

    	case 0xDD: // INVALID OPCODE!
			  break;

      case 0xDE:
        b = FETCH;
        __SBC
        break;

    	case 0xDF: /* RST 18 */
    		b = 0x18; goto __RST;

    }    
		break;    
  case 0x0E:
	  switch(op){
    
    	case 0xE0: /* LDH (imm),GBC_A */
    		writehi(FETCH, GBC_A); break;
    	case 0xE1: /* POP HL */
    		POP(HL); break;
    	case 0xE2: /* LDH (GBC_C),GBC_A */
    		writehi(GBC_C, GBC_A); break;

    	case 0xE3: // INVALID OPCODE!
			  break;
    	case 0xE4: // INVALID OPCODE!
			  break;

    	case 0xE5: /* PUSH HL */
    		PUSH(HL); break;
      case 0xE6:
        b = FETCH;
        __AND
        break;
    	case 0xE7: /* RST 20 */
    		b = 0x20; goto __RST;
    	case 0xE8: /* ADD SP_,imm */
#if 0
		b = FETCH; ADDSP(b); break;
#else
		{
			// https://gammpei.github.io/blog/posts/2018-03-04/how-to-write-a-game-boy-emulator-part-8-blarggs-cpu-test-roms-1-3-4-5-7-8-9-10-11.html
			signed char v = (signed char) FETCH;
			int temp = (int)(SP_) + (int)v;

			byte half_carry = ((SP_ & 0xff) ^ v ^ temp) & 0x10;

			GBC_F &= ~(FZ | FN | FH | FC);

			if (half_carry) GBC_F |= FH;
			if ((SP_ & 0xff) + (byte)v > 0xff) GBC_F |= FC;

			SP_ = temp & 0xffff;
		}
		break;
#endif
    	case 0xE9: /* JP HL */
    		PC_ = HL; break;    

    	case 0xEA: /* LD (imm),GBC_A */
    		writeb(readw(xPC), GBC_A); PC_ += 2; break;

    	case 0xEB: // INVALID OPCODE!
			  break;
    	case 0xEC: // INVALID OPCODE!
			  break;
    	case 0xED: // INVALID OPCODE!
			  break;

      case 0xEE:
        b = FETCH;
        __XOR
        break;
    	case 0xEF: /* RST 28 */
    		b = 0x28; goto __RST;

    }    
		break;    
  case 0x0F:
	  switch(op) {
    	case 0xF0: /* LDH GBC_A,(imm) */
    		GBC_A = readhi(FETCH); break;
    	case 0xF1: /* POP AF */
    		POP(AF); break;
    	case 0xF2: /* LDH GBC_A,(GBC_C) (undocumented) */
    		GBC_A = readhi(GBC_C); break;  			
    	case 0xF3: /* DI */
    		DI; break;
    	
			case 0xF4: // INVALID OPCODE!
			  break;
    	case 0xF5: /* PUSH AF */
    		PUSH(AF); break;

      case 0xF6:
        b = FETCH;
        __OR
        break;

    	case 0xF7: /* RST 30 */
    		b = 0x30; goto __RST;

    	case 0xF8: /* LD HL,SP_+imm */
//--------------------------------------------------------------------------------
#if 0
		b = FETCH; LDHLSP(b); break;
#else
		{
			// https://gammpei.github.io/blog/posts/2018-03-04/how-to-write-a-game-boy-emulator-part-8-blarggs-cpu-test-roms-1-3-4-5-7-8-9-10-11.html
			signed char v = (signed char) FETCH;
			int temp = (int)(SP_) + (int)v;

			byte half_carry = ((SP_ & 0xff) ^ v ^ temp) & 0x10;

			GBC_F &= ~(FZ | FN | FH | FC);

			if (half_carry) GBC_F |= FH;
			if ((SP_ & 0xff) + (byte)v > 0xff) GBC_F |= FC;

			HL = temp & 0xffff;
		}
		break;
#endif
//--------------------------------------------------------------------------------
    	case 0xF9: /* LD SP_,HL */
    		SP_ = HL; break;
    	case 0xFA: /* LD GBC_A,(imm) */
    		GBC_A = readb(readw(xPC)); PC_ += 2; break;

    	case 0xFB: /* EI */
    		EI; break;

			case 0xFC: // INVALID OPCODE!
			  break;
			case 0xFD: // INVALID OPCODE!
			  break;
				
      case 0xFE:
        b = FETCH;
        __CP
        break;
    	case 0xFF: /* RST 38 */
    		b = 0x38;
    	__RST:
    		RST(b); break;		
		}
		break;    
  
}
//--------------------------------------------------------------------------------
/*	switch(op)
	{
	case 0x00: // NOP 
	case 0x40: // LD GBC_B,GBC_B 
	case 0x49: // LD GBC_C,GBC_C 
	case 0x52: // LD GBC_D,GBC_D 
	case 0x5B: // LD GBC_E,GBC_E 
	case 0x64: // LD GBC_H,GBC_H 
	case 0x6D: // LD GBC_L,GBC_L 
	case 0x7F: // LD GBC_A,GBC_A 
		break;
			
	case 0x41: // LD GBC_B,GBC_C 
		GBC_B = GBC_C; break;
	case 0x42: // LD GBC_B,GBC_D 
		GBC_B = GBC_D; break;
	case 0x43: // LD GBC_B,GBC_E 
		GBC_B = GBC_E; break;
	case 0x44: // LD GBC_B,GBC_H 
		GBC_B = GBC_H; break;
	case 0x45: // LD GBC_B,GBC_L 
		GBC_B = GBC_L; break;
	case 0x46: // LD GBC_B,(HL) 
		GBC_B = readb(xHL); break;
	case 0x47: // LD GBC_B,GBC_A 
		GBC_B = GBC_A; break;

	case 0x48: // LD GBC_C,GBC_B 
		GBC_C = GBC_B; break;
	case 0x4A: // LD GBC_C,GBC_D 
		GBC_C = GBC_D; break;
	case 0x4B: // LD GBC_C,GBC_E 
		GBC_C = GBC_E; break;
	case 0x4C: // LD GBC_C,GBC_H 
		GBC_C = GBC_H; break;
	case 0x4D: // LD GBC_C,GBC_L 
		GBC_C = GBC_L; break;
	case 0x4E: // LD GBC_C,(HL) 
		GBC_C = readb(xHL); break;
	case 0x4F: // LD GBC_C,GBC_A 
		GBC_C = GBC_A; break;

	case 0x50: // LD GBC_D,GBC_B 
		GBC_D = GBC_B; break;
	case 0x51: // LD GBC_D,GBC_C 
		GBC_D = GBC_C; break;
	case 0x53: // LD GBC_D,GBC_E 
		GBC_D = GBC_E; break;
	case 0x54: // LD GBC_D,GBC_H 
		GBC_D = GBC_H; break;
	case 0x55: // LD GBC_D,GBC_L 
		GBC_D = GBC_L; break;
	case 0x56: // LD GBC_D,(HL) 
		GBC_D = readb(xHL); break;
	case 0x57: // LD GBC_D,GBC_A 
		GBC_D = GBC_A; break;

	case 0x58: // LD GBC_E,GBC_B 
		GBC_E = GBC_B; break;
	case 0x59: // LD GBC_E,GBC_C 
		GBC_E = GBC_C; break;
	case 0x5A: // LD GBC_E,GBC_D 
		GBC_E = GBC_D; break;
	case 0x5C: // LD GBC_E,GBC_H 
		GBC_E = GBC_H; break;
	case 0x5D: // LD GBC_E,GBC_L 
		GBC_E = GBC_L; break;
	case 0x5E: // LD GBC_E,(HL) 
		GBC_E = readb(xHL); break;
	case 0x5F: // LD GBC_E,GBC_A 
		GBC_E = GBC_A; break;

	case 0x60: // LD GBC_H,GBC_B 
		GBC_H = GBC_B; break;
	case 0x61: // LD GBC_H,GBC_C 
		GBC_H = GBC_C; break;
	case 0x62: // LD GBC_H,GBC_D 
		GBC_H = GBC_D; break;
	case 0x63: // LD GBC_H,GBC_E 
		GBC_H = GBC_E; break;
	case 0x65: // LD GBC_H,GBC_L 
		GBC_H = GBC_L; break;
	case 0x66: // LD GBC_H,(HL) 
		GBC_H = readb(xHL); break;
	case 0x67: // LD GBC_H,GBC_A 
		GBC_H = GBC_A; break;
			
	case 0x68: // LD GBC_L,GBC_B 
		GBC_L = GBC_B; break;
	case 0x69: // LD GBC_L,GBC_C 
		GBC_L = GBC_C; break;
	case 0x6A: // LD GBC_L,GBC_D 
		GBC_L = GBC_D; break;
	case 0x6B: // LD GBC_L,GBC_E 
		GBC_L = GBC_E; break;
	case 0x6C: // LD GBC_L,GBC_H 
		GBC_L = GBC_H; break;
	case 0x6E: // LD GBC_L,(HL) 
		GBC_L = readb(xHL); break;
	case 0x6F: // LD GBC_L,GBC_A 
		GBC_L = GBC_A; break;
			
	case 0x70: // LD (HL),GBC_B 
		b = GBC_B; goto __LD_HL;
	case 0x71: // LD (HL),GBC_C 
		b = GBC_C; goto __LD_HL;
	case 0x72: // LD (HL),GBC_D 
		b = GBC_D; goto __LD_HL;
	case 0x73: // LD (HL),GBC_E 
		b = GBC_E; goto __LD_HL;
	case 0x74: // LD (HL),GBC_H 
		b = GBC_H; goto __LD_HL;
	case 0x75: // LD (HL),GBC_L 
		b = GBC_L; goto __LD_HL;
	case 0x77: // LD (HL),GBC_A 
		b = GBC_A;
	__LD_HL:
		writeb(xHL,b);
		break;
			
	case 0x78: // LD GBC_A,GBC_B 
		GBC_A = GBC_B; break;
	case 0x79: // LD GBC_A,GBC_C 
		GBC_A = GBC_C; break;
	case 0x7A: // LD GBC_A,GBC_D 
		GBC_A = GBC_D; break;
	case 0x7B: // LD GBC_A,GBC_E 
		GBC_A = GBC_E; break;
	case 0x7C: // LD GBC_A,GBC_H 
		GBC_A = GBC_H; break;
	case 0x7D: // LD GBC_A,GBC_L 
		GBC_A = GBC_L; break;
	case 0x7E: // LD GBC_A,(HL) 
		GBC_A = readb(xHL); break;

	case 0x01: // LD BC,imm 
		BC = readw(xPC); PC_ += 2; break;
	case 0x11: // LD DE,imm 
		DE = readw(xPC); PC_ += 2; break;
	case 0x21: // LD HL,imm 
		HL = readw(xPC); PC_ += 2; break;
	case 0x31: // LD SP_,imm 
		SP_ = readw(xPC); PC_ += 2; break;

	case 0x02: // LD (BC),GBC_A 
		writeb(xBC, GBC_A); break;
	case 0x0A: // LD GBC_A,(BC) 
		GBC_A = readb(xBC); break;
	case 0x12: // LD (DE),GBC_A 
		writeb(xDE, GBC_A); break;
	case 0x1A: // LD GBC_A,(DE) 
		GBC_A = readb(xDE); break;

	case 0x22: // LDI (HL),GBC_A 
		writeb(xHL, GBC_A); HL++; break;
	case 0x2A: // LDI GBC_A,(HL) 
		GBC_A = readb(xHL); HL++; break;
	case 0x32: // LDD (HL),GBC_A 
		writeb(xHL, GBC_A); HL--; break;
	case 0x3A: // LDD GBC_A,(HL) 
		GBC_A = readb(xHL); HL--; break;

	case 0x06: // LD GBC_B,imm 
		GBC_B = FETCH; break;
	case 0x0E: // LD GBC_C,imm 
		GBC_C = FETCH; break;
	case 0x16: // LD GBC_D,imm 
		GBC_D = FETCH; break;
	case 0x1E: // LD GBC_E,imm 
		GBC_E = FETCH; break;
	case 0x26: // LD GBC_H,imm 
		GBC_H = FETCH; break;
	case 0x2E: // LD GBC_L,imm 
		GBC_L = FETCH; break;
	case 0x36: // LD (HL),imm 
		b = FETCH; writeb(xHL, b); break;
	case 0x3E: // LD GBC_A,imm 
		GBC_A = FETCH; break;

	case 0x08: // LD (imm),SP_ 
		writew(readw(xPC), SP_); PC_ += 2; break;
	case 0xEA: // LD (imm),GBC_A 
		writeb(readw(xPC), GBC_A); PC_ += 2; break;

	case 0xE0: // LDH (imm),GBC_A 
		writehi(FETCH, GBC_A); break;
	case 0xE2: // LDH (GBC_C),GBC_A 
		writehi(GBC_C, GBC_A); break;
	case 0xF0: // LDH GBC_A,(imm) 
		GBC_A = readhi(FETCH); break;
	case 0xF2: // LDH GBC_A,(GBC_C) (undocumented) 
		GBC_A = readhi(GBC_C); break;
			

	case 0xF8: // LD HL,SP_+imm 
		b = FETCH; LDHLSP(b); break;
	case 0xF9: // LD SP_,HL 
		SP_ = HL; break;
	case 0xFA: // LD GBC_A,(imm) 
		GBC_A = readb(readw(xPC)); PC_ += 2; break;

		ALU_CASES(0x80, 0xC6, ADD, __ADD)
		ALU_CASES(0x88, 0xCE, ADC, __ADC)
		ALU_CASES(0x90, 0xD6, SUB, __SUB)
		ALU_CASES(0x98, 0xDE, SBC, __SBC)
		ALU_CASES(0xA0, 0xE6, AND, __AND)
		ALU_CASES(0xA8, 0xEE, XOR, __XOR)
		ALU_CASES(0xB0, 0xF6, OR, __OR)
		ALU_CASES(0xB8, 0xFE, CP, __CP)

	case 0x09: // ADD HL,BC 
		w = BC; goto __ADDW;
	case 0x19: // ADD HL,DE 
		w = DE; goto __ADDW;
	case 0x39: // ADD HL,SP_ 
		w = SP_; goto __ADDW;
	case 0x29: // ADD HL,HL 
		w = HL;
	__ADDW:
		ADDW(w);
		break;

	case 0x04: // INC GBC_B 
		INC(GBC_B); break;
	case 0x0C: // INC GBC_C 
		INC(GBC_C); break;
	case 0x14: // INC GBC_D 
		INC(GBC_D); break;
	case 0x1C: // INC GBC_E 
		INC(GBC_E); break;
	case 0x24: // INC GBC_H 
		INC(GBC_H); break;
	case 0x2C: // INC GBC_L 
		INC(GBC_L); break;
	case 0x34: // INC (HL) 
		b = readb(xHL);
		INC(b);
		writeb(xHL, b);
		break;
	case 0x3C: // INC GBC_A 
		INC(GBC_A); break;
			
	case 0x03: // INC BC 
		INCW(BC); break;
	case 0x13: // INC DE 
		INCW(DE); break;
	case 0x23: // INC HL 
		INCW(HL); break;
	case 0x33: // INC SP_ 
		INCW(SP_); break;
			
	case 0x05: // DEC GBC_B 
		DEC(GBC_B); break;
	case 0x0D: // DEC GBC_C 
		DEC(GBC_C); break;
	case 0x15: // DEC GBC_D 
		DEC(GBC_D); break;
	case 0x1D: // DEC GBC_E 
		DEC(GBC_E); break;
	case 0x25: // DEC GBC_H 
		DEC(GBC_H); break;
	case 0x2D: // DEC GBC_L 
		DEC(GBC_L); break;
	case 0x35: // DEC (HL) 
		b = readb(xHL);
		DEC(b);
		writeb(xHL, b);
		break;
	case 0x3D: // DEC GBC_A 
		DEC(GBC_A); break;

	case 0x0B: // DEC BC 
		DECW(BC); break;
	case 0x1B: // DEC DE 
		DECW(DE); break;
	case 0x2B: // DEC HL 
		DECW(HL); break;
	case 0x3B: // DEC SP_ 
		DECW(SP_); break;

	case 0x07: // RLCA 
		RLCA(GBC_A); break;
	case 0x0F: // RRCA 
		RRCA(GBC_A); break;
	case 0x17: // RLA 
		RLA(GBC_A); break;
	case 0x1F: // RRA 
		RRA(GBC_A); break;

	case 0x27: // DAA 
		DAA; break;
	case 0x2F: // CPL 
		CPL(GBC_A); break;

	case 0x18: // JR 
	__JR:
		JR; break;
	case 0x20: // JR NZ 
		if (!(GBC_F&FZ)) goto __JR; NOJR; break;
	case 0x28: // JR Z 
		if (GBC_F&FZ) goto __JR; NOJR; break;
	case 0x30: // JR NC 
		if (!(GBC_F&FC)) goto __JR; NOJR; break;
	case 0x38: // JR GBC_C 
		if (GBC_F&FC) goto __JR; NOJR; break;

	case 0xC3: // JP 
	__JP:
		JP; break;
	case 0xC2: // JP NZ 
		if (!(GBC_F&FZ)) goto __JP; NOJP; break;
	case 0xCA: // JP Z 
		if (GBC_F&FZ) goto __JP; NOJP; break;
	case 0xD2: // JP NC 
		if (!(GBC_F&FC)) goto __JP; NOJP; break;
	case 0xDA: // JP GBC_C 
		if (GBC_F&FC) goto __JP; NOJP; break;
	case 0xE9: // JP HL 
		PC_ = HL; break;

	case 0xC9: // RET 
	__RET:
		RET; break;
	case 0xC0: // RET NZ 
		if (!(GBC_F&FZ)) goto __RET; NORET; break;
	case 0xC8: // RET Z 
		if (GBC_F&FZ) goto __RET; NORET; break;
	case 0xD0: // RET NC 
		if (!(GBC_F&FC)) goto __RET; NORET; break;
	case 0xD8: // RET GBC_C 
		if (GBC_F&FC) goto __RET; NORET; break;
	case 0xD9: // RETI 
		IME = IMA = 1; goto __RET;

	case 0xCD: // CALL 
	__CALL:
		CALL; break;
	case 0xC4: // CALL NZ 
		if (!(GBC_F&FZ)) goto __CALL; NOCALL; break;
	case 0xCC: // CALL Z 
		if (GBC_F&FZ) goto __CALL; NOCALL; break;
	case 0xD4: // CALL NC 
		if (!(GBC_F&FC)) goto __CALL; NOCALL; break;
	case 0xDC: // CALL GBC_C 
		if (GBC_F&FC) goto __CALL; NOCALL; break;

	case 0xC7: // RST 0 
		b = 0x00; goto __RST;
	case 0xCF: // RST 8 
		b = 0x08; goto __RST;
	case 0xD7: // RST 10 
		b = 0x10; goto __RST;
	case 0xDF: // RST 18 
		b = 0x18; goto __RST;
	case 0xE7: // RST 20 
		b = 0x20; goto __RST;
	case 0xEF: // RST 28 
		b = 0x28; goto __RST;
	case 0xF7: // RST 30 
		b = 0x30; goto __RST;
	case 0xFF: // RST 38 
		b = 0x38;
	__RST:
		RST(b); break;
			
	case 0xC1: // POP BC 
		POP(BC); break;
	case 0xC5: // PUSH BC 
		PUSH(BC); break;
	case 0xD1: // POP DE 
		POP(DE); break;
	case 0xD5: // PUSH DE 
		PUSH(DE); break;
	case 0xE1: // POP HL 
		POP(HL); break;
	case 0xE5: // PUSH HL 
		PUSH(HL); break;
	case 0xF1: // POP AF 
		POP(AF); break;
	case 0xF5: // PUSH AF 
		PUSH(AF); break;

	case 0xE8: // ADD SP_,imm 
		b = FETCH; ADDSP(b); break;

	case 0xF3: // DI 
		DI; break;
	case 0xFB: // EI 
		EI; break;

	case 0x37: // SCF 
		SCF; break;
	case 0x3F: // CCF 
		CCF; break;

	case 0x10: // STOP 
		PC_++;
		if (R_KEY1 & 1)
		{
			cpu__GBC.speed = cpu__GBC.speed ^ 1;
			R_KEY1 = (R_KEY1 & 0x7E) | (cpu__GBC.speed << 7);
			break;
		}
		// NOTE - we do not implement dmg STOP whatsoever 
		break;
			
	case 0x76: // HALT 
		cpu__GBC.halt = 1;
		break;

	case 0xCB: // CB prefix 
		cbop = FETCH;
		clen = cb_cycles_table[cbop];
		switch (cbop)
		{
			CB_REG_CASES(GBC_B, 0);
			CB_REG_CASES(GBC_C, 1);
			CB_REG_CASES(GBC_D, 2);
			CB_REG_CASES(GBC_E, 3);
			CB_REG_CASES(GBC_H, 4);
			CB_REG_CASES(GBC_L, 5);
			CB_REG_CASES(GBC_A, 7);
		default:
			b = readb(xHL);
			switch(cbop)
			{
				CB_REG_CASES(b, 6);
			}
			if ((cbop & 0xC0) != 0x40) // exclude BIT 
				writeb(xHL, b);
			break;
		}
		break;
			
	default:
		die(
			"invalid opcode 0x%02X at address 0x%04X, rombank = %d\n",
			op, (PC_-1) & 0xffff, mbc.rombank);
		break;
	}
*/


	clen <<= 1;
	div_advance(clen);
	timer_advance(clen);
	clen >>= cpu__GBC.speed;
	lcdc_advance(clen);
	sound_advance(clen);

	i -= clen;
	if (i > 0) goto next;
	return cycles-i;
}

#endif /* ASM_CPU_EMULATE */


#ifndef ASM_CPU_STEP

int cpu_step(int max)
{
	int cnt;
	if ((cnt = cpu_idle(max))) return cnt;
	return cpu_emulate(1);
}

#endif /* ASM_CPU_STEP */












