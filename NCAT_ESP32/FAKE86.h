#if FAKE86_EMULATOR

#define FAKE86_NO_AUDIO 1

//--------------------------------------------------------------------------------
#define FAKE86_USE_JUMPTABLE 1

uint8_t didbootstrap = 0;

uint16_t* VGA_SC;
uint16_t* VGA_CRTC;
uint16_t* VGA_ATTR;
uint16_t* VGA_GC;

uint32_t* palettecga; 
uint32_t* palettevga;

//#define DISK_CONTROLLER_ATA true

//████████████████████████████████████████
//cpu.h
//  Fake86: A portable, open-source 8086 PC emulator.
//  Copyright (C)2010-2012 Mike Chambers

//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


#define regax 0
#define regcx 1
#define regdx 2
#define regbx 3
#define regsp 4
#define regbp 5
#define regsi 6
#define regdi 7
#define reges 0
#define regcs 1
#define regss 2
#define regds 3

#ifdef __BIG_ENDIAN__
#define regal 1
#define regah 0
#define regcl 3
#define regch 2
#define regdl 5
#define regdh 4
#define regbl 7
#define regbh 6
#else
#define regal 0
#define regah 1
#define regcl 2
#define regch 3
#define regdl 4
#define regdh 5
#define regbl 6
#define regbh 7
#endif

union _bytewordregs_ 
{
  uint16_t wordregs[8];
  uint8_t byteregs[8];
};

#define StepIP(x)  (ip+= x)
#define getmem8(x, y) read86(segbase(x) + y)
#define getmem16(x, y)  readw86(segbase(x) + y)
#define putmem8(x, y, z)  write86(segbase(x) + y, z)
#define putmem16(x, y, z) writew86(segbase(x) + y, z)
#define signext(value)  (int16_t)(int8_t)(value)
#define signext32(value)  (int32_t)(int16_t)(value)
#define getreg16(regid) regs_.wordregs[regid]
#define getreg8(regid)  regs_.byteregs[byteregtable[regid]]
#define putreg16(regid, writeval) (regs_.wordregs[regid] = writeval)
#define putreg8(regid, writeval)  (regs_.byteregs[byteregtable[regid]] = writeval)
#define getsegreg(regid)  (segregs[regid])
#define putsegreg(regid, writeval)  (segregs[regid] = writeval)
#define segbase(x)  ((uint32_t)x << 4)

void  exec86 (uint32_t execloops); 

//████████████████████████████████████████
//i8253.h
#define PIT_MODE_LATCHCOUNT  0
#define PIT_MODE_LOBYTE 1
#define PIT_MODE_HIBYTE 2
#define PIT_MODE_TOGGLE 3

struct i8253_s {
  uint16_t chandata[3];
  uint8_t accessmode[3];
  uint8_t bytetoggle[3];
  uint32_t effectivedata[3];
  float chanfreq[3];
  uint8_t active[3];
  uint16_t counter[3];
};
//████████████████████████████████████████
//i8259.h
struct structpic {
  uint8_t imr; //mask register
  uint8_t irr; //request register
  uint8_t isr; //service register
  uint8_t icwstep; //used during initialization to keep track of which ICW we're at
  uint8_t icw[5];
  uint8_t intoffset; //interrupt vector offset
  uint8_t priority; //which IRQ has highest priority
  uint8_t autoeoi; //automatic EOI mode
  uint8_t readmode; //remember what to return on read register from OCW3
  uint8_t enabled;
};
//████████████████████████████████████████
//i8237.h
struct dmachan_s {
  uint32_t page;
  uint32_t addr;
  uint32_t reload;
  uint32_t count;
  uint8_t direction;
  uint8_t autoinit;
  uint8_t writemode;
  uint8_t masked;
};
//████████████████████████████████████████
struct blaster_s {
  uint8_t mem[1024];
  uint16_t memptr;
  uint16_t samplerate;
  uint8_t dspmaj;
  uint8_t dspmin;
  uint8_t speakerstate;
  uint8_t lastresetval;
  uint8_t lastcmdval;
  uint8_t lasttestval;
  uint8_t waitforarg;
  uint8_t paused8;
  uint8_t paused16;
  uint8_t sample;
  uint8_t sbirq;
  uint8_t sbdma;
  uint8_t usingdma;
  uint8_t maskdma;
  uint8_t useautoinit;
  uint32_t blocksize;
  uint32_t blockstep;
  uint64_t sampleticks;
  struct mixer_s {
    uint8_t index;
    uint8_t reg[256];
  } mixer;
}; 

blaster_s *blaster;


//████████████████████████████████████████



void FAKE86_KEYBOARD();

uint8_t hltstate=0;
static uint16_t trap_toggle = 0;

//████████████████████████████████████████
//cpu.c
extern struct i8253_s i8253;

extern struct structpic i8259;
uint64_t curtimer, lasttimer, timerfreq;

uint8_t byteregtable[8] = { regal, regcl, regdl, regbl, regah, regch, regdh, regbh };

static const uint8_t parity[0x100] = {
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
};

uint8_t opcode, segoverride, reptype, bootdrive = 0, hdcount = 0;
uint16_t segregs[4], savecs, saveip, ip, useseg, oldsp;
uint8_t tempcf, oldcf, cf, pf, af, zf, sf, tf, ifl, df, of, mode_, reg_, rm;
uint16_t oper1, oper2, res16, disp16, temp16, dummy, stacksize, frametemp;
uint8_t oper1b, oper2b, res8, disp8, temp8, nestlev, addrbyte;
uint32_t temp1, temp2, temp3, temp4, temp5, temp32, tempaddr32, ea;
int32_t result;
uint64_t totalexec;

extern uint8_t updatedscreen;

union _bytewordregs_ regs_;

uint8_t ethif;

extern uint8_t  vidmode;
extern void vidinterrupt();
extern uint8_t readVGA (uint32_t addr32);
extern void writeVGA (uint32_t addr32, uint8_t value);
extern void portout (uint16_t portnum, uint8_t value);
extern void portout16 (uint16_t portnum, uint16_t value);
extern uint8_t  portin (uint16_t portnum);
extern uint16_t portin16 (uint16_t portnum);

void intcall86 (uint8_t intnum);

#define modregrm() { \
  addrbyte = getmem8(segregs[regcs], ip); \
  StepIP(1); \
  mode_ = addrbyte >> 6; \
  reg_ = (addrbyte >> 3) & 7; \
  rm = addrbyte & 7; \
  switch(mode_) \
  { \
  case 0: \
  if(rm == 6) { \
  disp16 = getmem16(segregs[regcs], ip); \
  StepIP(2); \
  } \
  if(((rm == 2) || (rm == 3)) && !segoverride) { \
  useseg = segregs[regss]; \
  } \
  break; \
 \
  case 1: \
  disp16 = signext(getmem8(segregs[regcs], ip)); \
  StepIP(1); \
  if(((rm == 2) || (rm == 3) || (rm == 6)) && !segoverride) { \
  useseg = segregs[regss]; \
  } \
  break; \
 \
  case 2: \
  disp16 = getmem16(segregs[regcs], ip); \
  StepIP(2); \
  if(((rm == 2) || (rm == 3) || (rm == 6)) && !segoverride) { \
  useseg = segregs[regss]; \
  } \
  break; \
 \
  default: \
  disp8 = 0; \
  disp16 = 0; \
  } \
  }

#define makeflagsword() \
  ( \
  2 | (uint16_t) cf | ((uint16_t) pf << 2) | ((uint16_t) af << 4) | ((uint16_t) zf << 6) | ((uint16_t) sf << 7) | \
  ((uint16_t) tf << 8) | ((uint16_t) ifl << 9) | ((uint16_t) df << 10) | ((uint16_t) of << 11) \
  )

#define decodeflagsword(x) { \
  temp16 = x; \
  cf = temp16 & 1; \
  pf = (temp16 >> 2) & 1; \
  af = (temp16 >> 4) & 1; \
  zf = (temp16 >> 6) & 1; \
  sf = (temp16 >> 7) & 1; \
  tf = (temp16 >> 8) & 1; \
  ifl = (temp16 >> 9) & 1; \
  df = (temp16 >> 10) & 1; \
  of = (temp16 >> 11) & 1; \
  }


 static inline void write86 (uint32_t addr32, uint8_t value) {
  tempaddr32 = addr32 & 0xFFFFF;
  if (tempaddr32 >= 0xC0000) {
      return;
  }

  if ( (tempaddr32 >= 0xA0000) && (tempaddr32 <= 0xBFFFF) ) {
      if ( (vidmode != 0x13) && (vidmode != 0x12) && (vidmode != 0xD) && (vidmode != 0x10) ) {
          RAM[tempaddr32] = value;
          updatedscreen = 1;
        }
      else if ( ( (VGA_SC[4] & 6) == 0) && (vidmode != 0xD) && (vidmode != 0x10) && (vidmode != 0x12) ) {
          RAM[tempaddr32] = value;
          updatedscreen = 1;
        }
      else {
          writeVGA (tempaddr32 - 0xA0000, value);
        }

      updatedscreen = 1;
    }
  else {
      RAM[tempaddr32] = value;
    }
}

 static inline void writew86 (uint32_t addr32, uint16_t value) {
  write86 (addr32, (uint8_t) value);
  write86 (addr32 + 1, (uint8_t) (value >> 8) );
}

static inline uint8_t read86 (uint32_t addr32) {
  addr32 &= 0xFFFFF;
  if ( (addr32 >= 0xA0000) && (addr32 <= 0xBFFFF) ) {
    if ( (vidmode == 0xD) || (vidmode == 0xE) || (vidmode == 0x10) || (vidmode == 0x12)  || (vidmode == 0x13)  ) return (readVGA (addr32 - 0xA0000) );
    if ( (vidmode != 0x13) && (vidmode != 0x12) && (vidmode != 0xD) ) return (RAM[addr32]);
    if ( (VGA_SC[4] & 6) == 0)
      return (RAM[addr32]);
    else
      return (readVGA (addr32 - 0xA0000) );
  }

//   if (!didbootstrap) {
//      RAM[0x410] = 0x41; //ugly hack to make BIOS always believe we have an EGA/VGA card installed
//      RAM[0x475] = hdcount; //the BIOS doesn't have any concept of hard drives, so here's another hack
//    }
  return (RAM[addr32]);
}

static inline uint16_t readw86 (uint32_t addr32) {
  return ( (uint16_t) read86 (addr32) | (uint16_t) (read86 (addr32 + 1) << 8) );
}

inline static void flag_szp8(uint8_t value) {
  if (!value) {
     zf = 1;
  }
  else {
     zf = 0;  // set or clear zero flag 
  }

  if (value & 0x80) {
     sf = 1;
  }
  else {
     sf = 0;  // set or clear sign flag 
  }

   pf = parity[value]; // retrieve parity state from lookup table 
}

inline static void flag_szp16(uint16_t value) {
  if (!value) {
     zf = 1;
  } else {
     zf = 0;  // set or clear zero flag 
  }

  if (value & 0x8000) {
     sf = 1;
  } else {
     sf = 0;  // set or clear sign flag 
  }
  pf = parity[value & 255];  // retrieve parity state from lookup table 
}

inline static void flag_log8(uint8_t value) {
  flag_szp8(  value);
   cf = 0;
   of = 0; // bitwise logic ops always clear carry and overflow 
}

 inline static void flag_log16(uint16_t value) {
  flag_szp16(  value);
   cf = 0;
   of = 0; // bitwise logic ops always clear carry and overflow 
}

inline static void flag_adc8(uint8_t v1, uint8_t v2, uint8_t v3) {

  // v1 = destination operand, v2 = source operand, v3 = carry flag 
  uint16_t  dst;

  dst = (uint16_t)v1 + (uint16_t)v2 + (uint16_t)v3;
  flag_szp8(  (uint8_t)dst);
  if (((dst ^ v1) & (dst ^ v2) & 0x80) == 0x80) {
     of = 1;
  }
  else {
     of = 0; // set or clear overflow flag 
  }

  if (dst & 0xFF00) {
     cf = 1;
  }
  else {
     cf = 0; // set or clear carry flag 
  }

  if (((v1 ^ v2 ^ dst) & 0x10) == 0x10) {
     af = 1;
  }
  else {
     af = 0; // set or clear auxilliary flag 
  }
}

 inline static void flag_adc16(    uint16_t v1, uint16_t v2, uint16_t v3) {

  uint32_t  dst;

  dst = (uint32_t)v1 + (uint32_t)v2 + (uint32_t)v3;
  flag_szp16(  (uint16_t)dst);
  if ((((dst ^ v1) & (dst ^ v2)) & 0x8000) == 0x8000) {
     of = 1;
  }
  else {
     of = 0;
  }

  if (dst & 0xFFFF0000) {
     cf = 1;
  }
  else {
     cf = 0;
  }

  if (((v1 ^ v2 ^ dst) & 0x10) == 0x10) {
     af = 1;
  }
  else {
     af = 0;
  }
}

 inline static void flag_add8(    uint8_t v1, uint8_t v2) {
  // v1 = destination operand, v2 = source operand 
  uint16_t  dst;

  dst = (uint16_t)v1 + (uint16_t)v2;
  flag_szp8(  (uint8_t)dst);
  if (dst & 0xFF00) {
     cf = 1;
  }
  else {
     cf = 0;
  }

  if (((dst ^ v1) & (dst ^ v2) & 0x80) == 0x80) {
     of = 1;
  }
  else {
     of = 0;
  }

  if (((v1 ^ v2 ^ dst) & 0x10) == 0x10) {
     af = 1;
  }
  else {
     af = 0;
  }
}

 inline static void flag_add16(    uint16_t v1, uint16_t v2) {
  // v1 = destination operand, v2 = source operand 
  uint32_t  dst;

  dst = (uint32_t)v1 + (uint32_t)v2;
  flag_szp16(  (uint16_t)dst);
  if (dst & 0xFFFF0000) {
     cf = 1;
  }
  else {
     cf = 0;
  }

  if (((dst ^ v1) & (dst ^ v2) & 0x8000) == 0x8000) {
     of = 1;
  }
  else {
     of = 0;
  }

  if (((v1 ^ v2 ^ dst) & 0x10) == 0x10) {
     af = 1;
  }
  else {
     af = 0;
  }
}

 inline static void flag_sbb8(    uint8_t v1, uint8_t v2, uint8_t v3) {

  // v1 = destination operand, v2 = source operand, v3 = carry flag 
  uint16_t  dst;

  v2 += v3;
  dst = (uint16_t)v1 - (uint16_t)v2;
  flag_szp8(  (uint8_t)dst);
  if (dst & 0xFF00) {
     cf = 1;
  }
  else {
     cf = 0;
  }

  if ((dst ^ v1) & (v1 ^ v2) & 0x80) {
     of = 1;
  }
  else {
     of = 0;
  }

  if ((v1 ^ v2 ^ dst) & 0x10) {
     af = 1;
  }
  else {
     af = 0;
  }
}

 inline static void flag_sbb16(    uint16_t v1, uint16_t v2, uint16_t v3) {

  // v1 = destination operand, v2 = source operand, v3 = carry flag 
  uint32_t  dst;

  v2 += v3;
  dst = (uint32_t)v1 - (uint32_t)v2;
  flag_szp16(  (uint16_t)dst);
  if (dst & 0xFFFF0000) {
     cf = 1;
  }
  else {
     cf = 0;
  }

  if ((dst ^ v1) & (v1 ^ v2) & 0x8000) {
     of = 1;
  }
  else {
     of = 0;
  }

  if ((v1 ^ v2 ^ dst) & 0x10) {
     af = 1;
  }
  else {
     af = 0;
  }
}

 inline static void flag_sub8(    uint8_t v1, uint8_t v2) {

  // v1 = destination operand, v2 = source operand 
  uint16_t  dst;

  dst = (uint16_t)v1 - (uint16_t)v2;
  flag_szp8(  (uint8_t)dst);
  if (dst & 0xFF00) {
     cf = 1;
  }
  else {
     cf = 0;
  }

  if ((dst ^ v1) & (v1 ^ v2) & 0x80) {
     of = 1;
  }
  else {
     of = 0;
  }

  if ((v1 ^ v2 ^ dst) & 0x10) {
     af = 1;
  }
  else {
     af = 0;
  }
}

 inline static void flag_sub16(    uint16_t v1, uint16_t v2) {

  // v1 = destination operand, v2 = source operand 
  uint32_t  dst;

  dst = (uint32_t)v1 - (uint32_t)v2;
  flag_szp16(  (uint16_t)dst);
  if (dst & 0xFFFF0000) {
     cf = 1;
  }
  else {
     cf = 0;
  }

  if ((dst ^ v1) & (v1 ^ v2) & 0x8000) {
     of = 1;
  }
  else {
     of = 0;
  }

  if ((v1 ^ v2 ^ dst) & 0x10) {
     af = 1;
  }
  else {
     af = 0;
  }
}

 inline static void op_adc8(   ) {
   res8 =  oper1b +  oper2b +  cf;
  flag_adc8(   oper1b,  oper2b,  cf);
}

 inline static void op_adc16(   ) {
   res16 =  oper1 +  oper2 +  cf;
  flag_adc16(   oper1,  oper2,  cf);
}

 inline static void op_add8(   ) {
   res8 =  oper1b +  oper2b;
  flag_add8(   oper1b,  oper2b);
}

 inline static void op_add16(   ) {
   res16 =  oper1 +  oper2;
  flag_add16(   oper1,  oper2);
}

 inline static void op_and8(   ) {
   res8 =  oper1b &  oper2b;
  flag_log8(   res8);
}

 inline static void op_and16(   ) {
   res16 =  oper1 &  oper2;
  flag_log16(   res16);
}

 inline static void op_or8(   ) {
   res8 =  oper1b |  oper2b;
  flag_log8(   res8);
}

 inline static void op_or16(   ) {
   res16 =  oper1 |  oper2;
  flag_log16(   res16);
}

 inline static void op_xor8(   ) {
   res8 =  oper1b ^  oper2b;
  flag_log8(   res8);
}

 inline static void op_xor16(   ) {
   res16 =  oper1 ^  oper2;
  flag_log16(   res16);
}

 inline static void op_sub8(   ) {
   res8 =  oper1b -  oper2b;
  flag_sub8(   oper1b,  oper2b);
}

 inline static void op_sub16(   ) {
   res16 =  oper1 -  oper2;
  flag_sub16(   oper1,  oper2);
}

 inline static void op_sbb8(   ) {
   res8 =  oper1b - ( oper2b +  cf);
  flag_sbb8(   oper1b,  oper2b,  cf);
}

 inline static void op_sbb16(   ) {
   res16 =  oper1 - ( oper2 +  cf);
  flag_sbb16(   oper1,  oper2,  cf);
}

 inline static void getea(    uint8_t rmval) {
  uint32_t  tempea;

  tempea = 0;
  switch ( mode_) {
  case 0:
    switch (rmval) {
    case 0:
      tempea =  regs_.wordregs[regbx] +  regs_.wordregs[regsi];
      break;
    case 1:
      tempea =  regs_.wordregs[regbx] +  regs_.wordregs[regdi];
      break;
    case 2:
      tempea =  regs_.wordregs[regbp] +  regs_.wordregs[regsi];
      break;
    case 3:
      tempea =  regs_.wordregs[regbp] +  regs_.wordregs[regdi];
      break;
    case 4:
      tempea =  regs_.wordregs[regsi];
      break;
    case 5:
      tempea =  regs_.wordregs[regdi];
      break;
    case 6:
      tempea =  disp16;
      break;
    case 7:
      tempea =  regs_.wordregs[regbx];
      break;
    }
    break;

  case 1:
  case 2:
    switch (rmval) {
    case 0:
      tempea =  regs_.wordregs[regbx] +  regs_.wordregs[regsi] +  disp16;
      break;
    case 1:
      tempea =  regs_.wordregs[regbx] +  regs_.wordregs[regdi] +  disp16;
      break;
    case 2:
      tempea =  regs_.wordregs[regbp] +  regs_.wordregs[regsi] +  disp16;
      break;
    case 3:
      tempea =  regs_.wordregs[regbp] +  regs_.wordregs[regdi] +  disp16;
      break;
    case 4:
      tempea =  regs_.wordregs[regsi] +  disp16;
      break;
    case 5:
      tempea =  regs_.wordregs[regdi] +  disp16;
      break;
    case 6:
      tempea =  regs_.wordregs[regbp] +  disp16;
      break;
    case 7:
      tempea =  regs_.wordregs[regbx] +  disp16;
      break;
    }
    break;
  }

   ea = (tempea & 0xFFFF) + ( useseg << 4);
}

 inline static void push(    uint16_t pushval) {
   regs_.wordregs[regsp] =  regs_.wordregs[regsp] - 2;
  putmem16(   segregs[regss],  regs_.wordregs[regsp], pushval);
}

 inline static uint16_t pop(   ) {

  uint16_t  tempval;

  tempval = getmem16(   segregs[regss],  regs_.wordregs[regsp]);
   regs_.wordregs[regsp] =  regs_.wordregs[regsp] + 2;
  return tempval;
}

void reset86(   ) {
  uint16_t i;
  //for (i = 0; i < 256; i++) {
  //   int_callback[i] = NULL;
  //}
   segregs[regcs] = 0xFFFF;
   ip = 0x0000;
   hltstate = 0;
   trap_toggle = 0;
}

 inline static uint16_t readrm16(    uint8_t rmval) {
  if ( mode_ < 3) {
    getea(  rmval);
    return read86(   ea) | ((uint16_t)read86(   ea + 1) << 8);
  }
  else {
    return getreg16(  rmval);
  }
}

 inline static uint8_t readrm8(    uint8_t rmval) {
  if ( mode_ < 3) {
    getea(  rmval);
    return read86(   ea);
  }
  else {
    return getreg8(  rmval);
  }
}

 inline static void writerm16(    uint8_t rmval, uint16_t value) {
  if ( mode_ < 3) {
    getea(  rmval);
    write86(   ea, value & 0xFF);
    write86(   ea + 1, value >> 8);
  }
  else {
    putreg16(  rmval, value);
  }
}

 inline static void writerm8(    uint8_t rmval, uint8_t value) {
  if ( mode_ < 3) {
    getea(  rmval);
    write86(   ea, value);
  }
  else {
    putreg8(  rmval, value);
  }
}

 inline static uint8_t op_grp2_8(    uint8_t cnt) {

  uint16_t  s;
  uint16_t  shift;
  uint16_t  oldcf;
  uint16_t  msb;

  s =  oper1b;
  oldcf =  cf;
#ifdef CPU_LIMIT_SHIFT_COUNT
  cnt &= 0x1F;
#endif
  switch ( reg_) {
  case 0: // ROL r/m8 
    for (shift = 1; shift <= cnt; shift++) {
      if (s & 0x80) {
         cf = 1;
      }
      else {
         cf = 0;
      }

      s = s << 1;
      s = s |  cf;
    }

    if (cnt == 1) {
      // of =  cf ^ ( (s >> 7) & 1);
      if ((s & 0x80) &&  cf)  of = 1; else  of = 0;
    }
    else  of = 0;
    break;

  case 1: // ROR r/m8 
    for (shift = 1; shift <= cnt; shift++) {
       cf = s & 1;
      s = (s >> 1) | ( cf << 7);
    }

    if (cnt == 1) {
       of = (s >> 7) ^ ((s >> 6) & 1);
    }
    break;

  case 2: // RCL r/m8 
    for (shift = 1; shift <= cnt; shift++) {
      oldcf =  cf;
      if (s & 0x80) {
         cf = 1;
      }
      else {
         cf = 0;
      }

      s = s << 1;
      s = s | oldcf;
    }

    if (cnt == 1) {
       of =  cf ^ ((s >> 7) & 1);
    }
    break;

  case 3: // RCR r/m8 
    for (shift = 1; shift <= cnt; shift++) {
      oldcf =  cf;
       cf = s & 1;
      s = (s >> 1) | (oldcf << 7);
    }

    if (cnt == 1) {
       of = (s >> 7) ^ ((s >> 6) & 1);
    }
    break;

  case 4:
  case 6: // SHL r/m8 
    for (shift = 1; shift <= cnt; shift++) {
      if (s & 0x80) {
         cf = 1;
      }
      else {
         cf = 0;
      }

      s = (s << 1) & 0xFF;
    }

    if ((cnt == 1) && ( cf == (s >> 7))) {
       of = 0;
    }
    else {
       of = 1;
    }

    flag_szp8(  (uint8_t)s);
    break;

  case 5: // SHR r/m8 
    if ((cnt == 1) && (s & 0x80)) {
       of = 1;
    }
    else {
       of = 0;
    }

    for (shift = 1; shift <= cnt; shift++) {
       cf = s & 1;
      s = s >> 1;
    }

    flag_szp8(  (uint8_t)s);
    break;

  case 7: // SAR r/m8 
    for (shift = 1; shift <= cnt; shift++) {
      msb = s & 0x80;
       cf = s & 1;
      s = (s >> 1) | msb;
    }

     of = 0;
    flag_szp8(  (uint8_t)s);
    break;
  }

  return s & 0xFF;
}

 inline static uint16_t op_grp2_16(    uint8_t cnt) {

  uint32_t  s;
  uint32_t  shift;
  uint32_t  oldcf;
  uint32_t  msb;

  s =  oper1;
  oldcf =  cf;
#ifdef CPU_LIMIT_SHIFT_COUNT
  cnt &= 0x1F;
#endif
  switch ( reg_) {
  case 0: // ROL r/m8 
    for (shift = 1; shift <= cnt; shift++) {
      if (s & 0x8000) {
         cf = 1;
      }
      else {
         cf = 0;
      }

      s = s << 1;
      s = s |  cf;
    }

    if (cnt == 1) {
       of =  cf ^ ((s >> 15) & 1);
    }
    break;

  case 1: // ROR r/m8 
    for (shift = 1; shift <= cnt; shift++) {
       cf = s & 1;
      s = (s >> 1) | ( cf << 15);
    }

    if (cnt == 1) {
       of = (s >> 15) ^ ((s >> 14) & 1);
    }
    break;

  case 2: // RCL r/m8 
    for (shift = 1; shift <= cnt; shift++) {
      oldcf =  cf;
      if (s & 0x8000) {
         cf = 1;
      }
      else {
         cf = 0;
      }

      s = s << 1;
      s = s | oldcf;
    }

    if (cnt == 1) {
       of =  cf ^ ((s >> 15) & 1);
    }
    break;

  case 3: // RCR r/m8 
    for (shift = 1; shift <= cnt; shift++) {
      oldcf =  cf;
       cf = s & 1;
      s = (s >> 1) | (oldcf << 15);
    }

    if (cnt == 1) {
       of = (s >> 15) ^ ((s >> 14) & 1);
    }
    break;

  case 4:
  case 6: // SHL r/m8 
    for (shift = 1; shift <= cnt; shift++) {
      if (s & 0x8000) {
         cf = 1;
      }
      else {
         cf = 0;
      }

      s = (s << 1) & 0xFFFF;
    }

    if ((cnt == 1) && ( cf == (s >> 15))) {
       of = 0;
    }
    else {
       of = 1;
    }

    flag_szp16(  (uint16_t)s);
    break;

  case 5: // SHR r/m8 
    if ((cnt == 1) && (s & 0x8000)) {
       of = 1;
    }
    else {
       of = 0;
    }

    for (shift = 1; shift <= cnt; shift++) {
       cf = s & 1;
      s = s >> 1;
    }

    flag_szp16(  (uint16_t)s);
    break;

  case 7: // SAR r/m8 
    for (shift = 1; shift <= cnt; shift++) {
      msb = s & 0x8000;
       cf = s & 1;
      s = (s >> 1) | msb;
    }

     of = 0;
    flag_szp16(  (uint16_t)s);
    break;
  }

  return (uint16_t)s & 0xFFFF;
}

 inline static void op_div8(    uint16_t valdiv, uint8_t divisor) {
  if (divisor == 0) {
    intcall86(  0);
    return;
  }

  if ((valdiv / (uint16_t)divisor) > 0xFF) {
    intcall86(  0);
    return;
  }

   regs_.byteregs[regah] = valdiv % (uint16_t)divisor;
   regs_.byteregs[regal] = valdiv / (uint16_t)divisor;
}

 inline static void op_idiv8(    uint16_t valdiv, uint8_t divisor) {
  //TODO: Rewrite IDIV code, I wrote this in 2011. It can be made far more efficient.
  uint16_t  s1;
  uint16_t  s2;
  uint16_t  d1;
  uint16_t  d2;
  int sign;

  if (divisor == 0) {
    intcall86(  0);
    return;
  }

  s1 = valdiv;
  s2 = divisor;
  sign = (((s1 ^ s2) & 0x8000) != 0);
  s1 = (s1 < 0x8000) ? s1 : ((~s1 + 1) & 0xffff);
  s2 = (s2 < 0x8000) ? s2 : ((~s2 + 1) & 0xffff);
  d1 = s1 / s2;
  d2 = s1 % s2;
  if (d1 & 0xFF00) {
    intcall86(  0);
    return;
  }

  if (sign) {
    d1 = (~d1 + 1) & 0xff;
    d2 = (~d2 + 1) & 0xff;
  }

   regs_.byteregs[regah] = (uint8_t)d2;
   regs_.byteregs[regal] = (uint8_t)d1;
}

 inline static void op_grp3_8(   ) {
   oper1 = signext( oper1b);
   oper2 = signext( oper2b);
  switch ( reg_) {
  case 0:
  case 1: // TEST 
    flag_log8(   oper1b & getmem8(   segregs[regcs],  ip));
    StepIP(  1);
    break;

  case 2: // NOT 
     res8 = ~ oper1b;
    break;

  case 3: // NEG 
     res8 = (~ oper1b) + 1;
    flag_sub8(  0,  oper1b);
    if ( res8 == 0) {
       cf = 0;
    }
    else {
       cf = 1;
    }
    break;

  case 4: // MUL 
     temp1 = (uint32_t) oper1b * (uint32_t) regs_.byteregs[regal];
     regs_.wordregs[regax] =  temp1 & 0xFFFF;
    flag_szp8(  (uint8_t) temp1);
    if ( regs_.byteregs[regah]) {
       cf = 1;
       of = 1;
    }
    else {
       cf = 0;
       of = 0;
    }
#ifdef CPU_CLEAR_ZF_ON_MUL
     zf = 0;
#endif
    break;

  case 5: // IMUL 
     oper1 = signext( oper1b);
     temp1 = signext( regs_.byteregs[regal]);
     temp2 =  oper1;
    if (( temp1 & 0x80) == 0x80) {
       temp1 =  temp1 | 0xFFFFFF00;
    }

    if (( temp2 & 0x80) == 0x80) {
       temp2 =  temp2 | 0xFFFFFF00;
    }

     temp3 = ( temp1 *  temp2) & 0xFFFF;
     regs_.wordregs[regax] =  temp3 & 0xFFFF;
    if ( regs_.byteregs[regah]) {
       cf = 1;
       of = 1;
    }
    else {
       cf = 0;
       of = 0;
    }
#ifdef CPU_CLEAR_ZF_ON_MUL
     zf = 0;
#endif
    break;

  case 6: // DIV 
    op_div8(   regs_.wordregs[regax],  oper1b);
    break;

  case 7: // IDIV 
    op_idiv8(   regs_.wordregs[regax],  oper1b);
    break;
  }
}

 inline static void op_div16(    uint32_t valdiv, uint16_t divisor) {
  if (divisor == 0) {
    intcall86(  0);
    return;
  }

  if ((valdiv / (uint32_t)divisor) > 0xFFFF) {
    intcall86(  0);
    return;
  }

   regs_.wordregs[regdx] = valdiv % (uint32_t)divisor;
   regs_.wordregs[regax] = valdiv / (uint32_t)divisor;
}

 inline static void op_idiv16(    uint32_t valdiv, uint16_t divisor) {
  //TODO: Rewrite IDIV code, I wrote this in 2011. It can be made far more efficient.
  uint32_t  d1;
  uint32_t  d2;
  uint32_t  s1;
  uint32_t  s2;
  int sign;

  if (divisor == 0) {
    intcall86(  0);
    return;
  }

  s1 = valdiv;
  s2 = divisor;
  s2 = (s2 & 0x8000) ? (s2 | 0xffff0000) : s2;
  sign = (((s1 ^ s2) & 0x80000000) != 0);
  s1 = (s1 < 0x80000000) ? s1 : ((~s1 + 1) & 0xffffffff);
  s2 = (s2 < 0x80000000) ? s2 : ((~s2 + 1) & 0xffffffff);
  d1 = s1 / s2;
  d2 = s1 % s2;
  if (d1 & 0xFFFF0000) {
    intcall86(  0);
    return;
  }

  if (sign) {
    d1 = (~d1 + 1) & 0xffff;
    d2 = (~d2 + 1) & 0xffff;
  }

   regs_.wordregs[regax] = d1;
   regs_.wordregs[regdx] = d2;
}

 inline static void op_grp3_16(   ) {
  switch ( reg_) {
  case 0:
  case 1: // TEST 
    flag_log16(   oper1 & getmem16(   segregs[regcs],  ip));
    StepIP(  2);
    break;

  case 2: // NOT 
     res16 = ~ oper1;
    break;

  case 3: // NEG 
     res16 = (~ oper1) + 1;
    flag_sub16(  0,  oper1);
    if ( res16) {
       cf = 1;
    }
    else {
       cf = 0;
    }
    break;

  case 4: // MUL 
     temp1 = (uint32_t) oper1 * (uint32_t) regs_.wordregs[regax];
     regs_.wordregs[regax] =  temp1 & 0xFFFF;
     regs_.wordregs[regdx] =  temp1 >> 16;
    flag_szp16(  (uint16_t) temp1);
    if ( regs_.wordregs[regdx]) {
       cf = 1;
       of = 1;
    }
    else {
       cf = 0;
       of = 0;
    }
#ifdef CPU_CLEAR_ZF_ON_MUL
     zf = 0;
#endif
    break;

  case 5: // IMUL 
     temp1 =  regs_.wordregs[regax];
     temp2 =  oper1;
    if ( temp1 & 0x8000) {
       temp1 |= 0xFFFF0000;
    }

    if ( temp2 & 0x8000) {
       temp2 |= 0xFFFF0000;
    }

     temp3 =  temp1 *  temp2;
     regs_.wordregs[regax] =  temp3 & 0xFFFF;  // into register ax 
     regs_.wordregs[regdx] =  temp3 >> 16; // into register dx 
    if ( regs_.wordregs[regdx]) {
       cf = 1;
       of = 1;
    }
    else {
       cf = 0;
       of = 0;
    }
#ifdef CPU_CLEAR_ZF_ON_MUL
     zf = 0;
#endif
    break;

  case 6: // DIV 
    op_div16(  ((uint32_t) regs_.wordregs[regdx] << 16) +  regs_.wordregs[regax],  oper1);
    break;

  case 7: // DIV 
    op_idiv16(  ((uint32_t) regs_.wordregs[regdx] << 16) +  regs_.wordregs[regax],  oper1);
    break;
  }
}

 inline static void op_grp5(   ) {
  switch ( reg_) {
  case 0: // INC Ev 
     oper2 = 1;
     tempcf =  cf;
    op_add16( );
     cf =  tempcf;
    writerm16(   rm,  res16);
    break;

  case 1: // DEC Ev 
     oper2 = 1;
     tempcf =  cf;
    op_sub16( );
     cf =  tempcf;
    writerm16(   rm,  res16);
    break;

  case 2: // CALL Ev 
    push(   ip);
     ip =  oper1;
    break;

  case 3: // CALL Mp 
    push(   segregs[regcs]);
    push(   ip);
    getea(   rm);
     ip = (uint16_t)read86(   ea) + (uint16_t)read86(   ea + 1) * 256;
     segregs[regcs] = (uint16_t)read86(   ea + 2) + (uint16_t)read86(   ea + 3) * 256;
    break;

  case 4: // JMP Ev 
     ip =  oper1;
    break;

  case 5: // JMP Mp 
    getea(   rm);
     ip = (uint16_t)read86(   ea) + (uint16_t)read86(   ea + 1) * 256;
     segregs[regcs] = (uint16_t)read86(   ea + 2) + (uint16_t)read86(   ea + 3) * 256;
    break;

  case 6: // PUSH Ev 
    push(   oper1);
    break;
  }
}

















uint8_t dolog = 0, didintr = 0;
FILE  *logout;
uint8_t printops = 0;

#ifdef NETWORKING_ENABLED
extern void nethandler();
#endif
extern void diskhandler();
extern void readdisk (uint8_t drivenum, uint16_t dstseg, uint16_t dstoff, uint16_t cyl, uint16_t sect, uint16_t head, uint16_t sectcount);

 void intcall86 (uint8_t intnum) {
  static uint16_t lastint10ax;
  uint16_t oldregax;
  didintr = 1;

  if (intnum == 0x19) { didbootstrap = 1;
  }

  switch (intnum) {
      case 0x10:
        updatedscreen = 1;
        if ( (regs_.byteregs[regah]==0x00) || (regs_.byteregs[regah]==0x10) ) {
            oldregax = regs_.wordregs[regax];
            vidinterrupt();
            regs_.wordregs[regax] = oldregax;
            if (regs_.byteregs[regah]==0x10) return;
            if (vidmode==9) return;
          }
        if ( (regs_.byteregs[regah]==0x1A) && (lastint10ax!=0x0100) ) { //the 0x0100 is a cheap hack to make it not do this if DOS EDIT/QBASIC
            regs_.byteregs[regal] = 0x1A;
            regs_.byteregs[regbl] = 0x8;
            return;
          }
        lastint10ax = regs_.wordregs[regax];
        break;

#ifndef DISK_CONTROLLER_ATA
      case 0x19: //bootstrap
        if (bootdrive<255) { //read first sector of boot drive into 07C0:0000 and execute it
            regs_.byteregs[regdl] = bootdrive;
            readdisk (regs_.byteregs[regdl], 0x07C0, 0x0000, 0, 1, 0, 1);
            segregs[regcs] = 0x0000;
            ip = 0x7C00;
          }
        else {
            segregs[regcs] = 0xF600;  //start ROM BASIC at bootstrap if requested
            ip = 0x0000;
          }
        return;

      case 0x13:
      case 0xFD:
        diskhandler();
        return;
#endif
#ifdef NETWORKING_OLDCARD
      case 0xFC:
#ifdef NETWORKING_ENABLED
        nethandler();
#endif
        return;
#endif
    }

  push (makeflagsword() );
  push (segregs[regcs]);
  push (ip);
  segregs[regcs] = getmem16 (0, (uint16_t) intnum * 4 + 2);
  ip = getmem16 (0, (uint16_t) intnum * 4);
  ifl = 0;
  tf = 0;
}

#if defined(NETWORKING_ENABLED)
extern struct netstruct {
  uint8_t enabled;
  uint8_t canrecv;
  uint16_t  pktlen;
} net;
#endif



uint64_t  frametimer = 0, didwhen = 0, didticks = 0;
uint32_t  makeupticks = 0;
extern float  timercomp;
uint64_t  timerticks = 0, realticks = 0;
uint64_t  lastcountertimer = 0, counterticks = 10000;
extern uint8_t  nextintr();
extern void timing();



uint32_t  loopcount;
uint8_t docontinue;
static uint16_t firstip;

static inline void opcode00 (void) { 
  // 00 ADD Eb Gb 
  modregrm();
  oper1b = readrm8 (rm);
  oper2b = getreg8 (reg_);
  op_add8();
  writerm8 (rm, res8);
}
static inline void opcode01 (void) { 
  // 01 ADD Ev Gv 
  modregrm();
  oper1 = readrm16 (rm);
  oper2 = getreg16 (reg_);
  op_add16();
  writerm16 (rm, res16);
}
static inline void opcode02 (void) { 
  // 02 ADD Gb Eb 
  modregrm();
  oper1b = getreg8 (reg_);
  oper2b = readrm8 (rm);
  op_add8();
  putreg8 (reg_, res8);
}
static inline void opcode03 (void) { 
  // 03 ADD Gv Ev 
  modregrm();
  oper1 = getreg16 (reg_);
  oper2 = readrm16 (rm);
  op_add16();
  putreg16 (reg_, res16);
}
static inline void opcode04 (void) { 
  // 04 ADD regs_.byteregs[regal] Ib 
  oper1b = regs_.byteregs[regal];
  oper2b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  op_add8();
  regs_.byteregs[regal] = res8;
}
static inline void opcode05 (void) { 
  // 05 ADD eAX Iv 
  oper1 = (getreg16 (regax) );
  oper2 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  op_add16();
  putreg16 (regax, res16);
}
static inline void opcode06 (void) { 
  // 06 PUSH segregs[reges] 
  push (segregs[reges]);
}
static inline void opcode07 (void) { 
  // 07 POP segregs[reges] 
  segregs[reges] = pop();
}
static inline void opcode08 (void) { 
  // 08 OR Eb Gb 
  modregrm();
  oper1b = readrm8 (rm);
  oper2b = getreg8 (reg_);
  op_or8();
  writerm8 (rm, res8);
}
static inline void opcode09 (void) { 
  // 09 OR Ev Gv 
  modregrm();
  oper1 = readrm16 (rm);
  oper2 = getreg16 (reg_);
  op_or16();
  writerm16 (rm, res16);
}
static inline void opcode0A (void) { 
  // 0A OR Gb Eb 
  modregrm();
  oper1b = getreg8 (reg_);
  oper2b = readrm8 (rm);
  op_or8();
  putreg8 (reg_, res8);
}
static inline void opcode0B (void) { 
  // 0B OR Gv Ev 
  modregrm();
  oper1 = getreg16 (reg_);
  oper2 = readrm16 (rm);
  op_or16();
  if ( (oper1 == 0xF802) && (oper2 == 0xF802) ) {
    sf = 0; // cheap hack to make Wolf 3D think we're a 286 so it plays 
  }
  putreg16 (reg_, res16);
}
static inline void opcode0C (void) { 
  // 0C OR regs_.byteregs[regal] Ib 
  oper1b = regs_.byteregs[regal];
  oper2b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  op_or8();
  regs_.byteregs[regal] = res8;
}
static inline void opcode0D (void) { 
  // 0D OR eAX Iv 
  oper1 = getreg16 (regax);
  oper2 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  op_or16();
  putreg16 (regax, res16);
}
static inline void opcode0E (void) { 
  // 0E PUSH segregs[regcs] 
  push (segregs[regcs]);
}
static inline void opcode0F (void) { 
  //0F POP CS
#ifndef CPU_V20
  segregs[regcs] = pop();
#endif
}
static inline void opcode10 (void) { 
  // 10 ADC Eb Gb 
  modregrm();
  oper1b = readrm8 (rm);
  oper2b = getreg8 (reg_);
  op_adc8();
  writerm8 (rm, res8);
}
static inline void opcode11 (void) { 
  // 11 ADC Ev Gv 
  modregrm();
  oper1 = readrm16 (rm);
  oper2 = getreg16 (reg_);
  op_adc16();
  writerm16 (rm, res16);
}
static inline void opcode12 (void) { 
  // 12 ADC Gb Eb 
  modregrm();
  oper1b = getreg8 (reg_);
  oper2b = readrm8 (rm);
  op_adc8();
  putreg8 (reg_, res8);
}
static inline void opcode13 (void) { 
  // 13 ADC Gv Ev 
  modregrm();
  oper1 = getreg16 (reg_);
  oper2 = readrm16 (rm);
  op_adc16();
  putreg16 (reg_, res16);
}
static inline void opcode14 (void) { 
  // 14 ADC regs_.byteregs[regal] Ib 
  oper1b = regs_.byteregs[regal];
  oper2b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  op_adc8();
  regs_.byteregs[regal] = res8;
}
static inline void opcode15 (void) { 
  //15 ADC eAX Iv 
  oper1 = getreg16 (regax);
  oper2 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  op_adc16();
  putreg16 (regax, res16);
}
static inline void opcode16 (void) { 
  // 16 PUSH segregs[regss] 
  push (segregs[regss]);
}
static inline void opcode17 (void) { 
  // 17 POP segregs[regss] 
  segregs[regss] = pop();
}
static inline void opcode18 (void) { 
  // 18 SBB Eb Gb 
  modregrm();
  oper1b = readrm8 (rm);
  oper2b = getreg8 (reg_);
  op_sbb8();
  writerm8 (rm, res8);
}
static inline void opcode19 (void) { 
  // 19 SBB Ev Gv 
  modregrm();
  oper1 = readrm16 (rm);
  oper2 = getreg16 (reg_);
  op_sbb16();
  writerm16 (rm, res16);
}
static inline void opcode1A (void) { 
  // 1A SBB Gb Eb 
  modregrm();
  oper1b = getreg8 (reg_);
  oper2b = readrm8 (rm);
  op_sbb8();
  putreg8 (reg_, res8);
}
static inline void opcode1B (void) { 
  // 1B SBB Gv Ev 
  modregrm();
  oper1 = getreg16 (reg_);
  oper2 = readrm16 (rm);
  op_sbb16();
  putreg16 (reg_, res16);
}
static inline void opcode1C (void) { 
  // 1C SBB regs_.byteregs[regal] Ib 
  oper1b = regs_.byteregs[regal];
  oper2b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  op_sbb8();
  regs_.byteregs[regal] = res8;
}
static inline void opcode1D (void) { 
  // 1D SBB eAX Iv 
  oper1 = getreg16 (regax);
  oper2 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  op_sbb16();
  putreg16 (regax, res16);
}
static inline void opcode1E (void) { 
  // 1E PUSH segregs[regds] 
  push (segregs[regds]);
}
static inline void opcode1F (void) { 
  // 1F POP segregs[regds] 
  segregs[regds] = pop();
}
static inline void opcode20 (void) { 
  // 20 AND Eb Gb 
  modregrm();
  oper1b = readrm8 (rm);
  oper2b = getreg8 (reg_);
  op_and8();
  writerm8 (rm, res8);
}
static inline void opcode21 (void) { 
  // 21 AND Ev Gv 
  modregrm();
  oper1 = readrm16 (rm);
  oper2 = getreg16 (reg_);
  op_and16();
  writerm16 (rm, res16);
}
static inline void opcode22 (void) { 
  // 22 AND Gb Eb 
  modregrm();
  oper1b = getreg8 (reg_);
  oper2b = readrm8 (rm);
  op_and8();
  putreg8 (reg_, res8);
}
static inline void opcode23 (void) { 
  // 23 AND Gv Ev 
  modregrm();
  oper1 = getreg16 (reg_);
  oper2 = readrm16 (rm);
  op_and16();
  putreg16 (reg_, res16);
}
static inline void opcode24 (void) { 
  // 24 AND regs_.byteregs[regal] Ib 
  oper1b = regs_.byteregs[regal];
  oper2b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  op_and8();
  regs_.byteregs[regal] = res8;
}
static inline void opcode25 (void) { 
  // 25 AND eAX Iv 
  oper1 = getreg16 (regax);
  oper2 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  op_and16();
  putreg16 (regax, res16);
}
static inline void opcode27 (void) { 
  // 27 DAA 
  if ( ( (regs_.byteregs[regal] & 0xF) > 9) || (af == 1) ) {
    oper1 = regs_.byteregs[regal] + 6;
    regs_.byteregs[regal] = oper1 & 255;
    if (oper1 & 0xFF00) {
      cf = 1;
    } else {
      cf = 0;
    }
    af = 1;
  } else {
    af = 0;
  }

  if ( ( (regs_.byteregs[regal] & 0xF0) > 0x90) || (cf == 1) ) {
    regs_.byteregs[regal] = regs_.byteregs[regal] + 0x60;
    cf = 1;
  } else {
    cf = 0;
  }

  regs_.byteregs[regal] = regs_.byteregs[regal] & 255;
  flag_szp8 (regs_.byteregs[regal]);
}
static inline void opcode28 (void) { 
  // 28 SUB Eb Gb 
  modregrm();
  oper1b = readrm8 (rm);
  oper2b = getreg8 (reg_);
  op_sub8();
  writerm8 (rm, res8);
}
static inline void opcode29 (void) { 
  // 29 SUB Ev Gv 
  modregrm();
  oper1 = readrm16 (rm);
  oper2 = getreg16 (reg_);
  op_sub16();
  writerm16 (rm, res16);
}
static inline void opcode2A (void) { 
  // 2A SUB Gb Eb 
  modregrm();
  oper1b = getreg8 (reg_);
  oper2b = readrm8 (rm);
  op_sub8();
  putreg8 (reg_, res8);
}
static inline void opcode2B (void) { 
  // 2B SUB Gv Ev 
  modregrm();
  oper1 = getreg16 (reg_);
  oper2 = readrm16 (rm);
  op_sub16();
  putreg16 (reg_, res16);
}
static inline void opcode2C (void) { 
  // 2C SUB regs_.byteregs[regal] Ib 
  oper1b = regs_.byteregs[regal];
  oper2b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  op_sub8();
  regs_.byteregs[regal] = res8;
}
static inline void opcode2D (void) { 
  // 2D SUB eAX Iv 
  oper1 = getreg16 (regax);
  oper2 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  op_sub16();
  putreg16 (regax, res16);
}
static inline void opcode2F (void) { 
  // 2F DAS 
  if ( ( (regs_.byteregs[regal] & 15) > 9) || (af == 1) ) {
    oper1 = regs_.byteregs[regal] - 6;
    regs_.byteregs[regal] = oper1 & 255;
    if (oper1 & 0xFF00) {
      cf = 1;
    } else {
      cf = 0;
    }
    af = 1;
  } else {
    af = 0;
  }
  if ( ( (regs_.byteregs[regal] & 0xF0) > 0x90) || (cf == 1) ) {
    regs_.byteregs[regal] = regs_.byteregs[regal] - 0x60;
    cf = 1;
  } else {
    cf = 0;
  }
  flag_szp8 (regs_.byteregs[regal]);
}
static inline void opcode30 (void) { 
  // 30 XOR Eb Gb 
  modregrm();
  oper1b = readrm8 (rm);
  oper2b = getreg8 (reg_);
  op_xor8();
  writerm8 (rm, res8);
}
static inline void opcode31 (void) { 
  // 31 XOR Ev Gv 
  modregrm();
  oper1 = readrm16 (rm);
  oper2 = getreg16 (reg_);
  op_xor16();
  writerm16 (rm, res16);
}
static inline void opcode32 (void) { 
  // 32 XOR Gb Eb 
  modregrm();
  oper1b = getreg8 (reg_);
  oper2b = readrm8 (rm);
  op_xor8();
  putreg8 (reg_, res8);
}
static inline void opcode33 (void) { 
  // 33 XOR Gv Ev 
  modregrm();
  oper1 = getreg16 (reg_);
  oper2 = readrm16 (rm);
  op_xor16();
  putreg16 (reg_, res16);
}
static inline void opcode34 (void) { 
  // 34 XOR regs_.byteregs[regal] Ib 
  oper1b = regs_.byteregs[regal];
  oper2b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  op_xor8();
  regs_.byteregs[regal] = res8;
}
static inline void opcode35 (void) { 
  // 35 XOR eAX Iv 
  oper1 = getreg16 (regax);
  oper2 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  op_xor16();
  putreg16 (regax, res16);
}
static inline void opcode37 (void) { 
  // 37 AAA ASCII 
  if ( ( (regs_.byteregs[regal] & 0xF) > 9) || (af == 1) ) {
    regs_.byteregs[regal] = regs_.byteregs[regal] + 6;
    regs_.byteregs[regah] = regs_.byteregs[regah] + 1;
    af = 1;
    cf = 1;
  } else {
    af = 0;
    cf = 0;
  } 
  regs_.byteregs[regal] = regs_.byteregs[regal] & 0xF;
}
static inline void opcode38 (void) { 
  // 38 CMP Eb Gb 
  modregrm();
  oper1b = readrm8 (rm);
  oper2b = getreg8 (reg_);
  flag_sub8 (oper1b, oper2b);
}
static inline void opcode39 (void) { 
  // 39 CMP Ev Gv 
  modregrm();
  oper1 = readrm16 (rm);
  oper2 = getreg16 (reg_);
  flag_sub16 (oper1, oper2);
}
static inline void opcode3A (void) { 
  // 3A CMP Gb Eb 
  modregrm();
  oper1b = getreg8 (reg_);
  oper2b = readrm8 (rm);
  flag_sub8 (oper1b, oper2b);
}
static inline void opcode3B (void) { 
  // 3B CMP Gv Ev 
  modregrm();
  oper1 = getreg16 (reg_);
  oper2 = readrm16 (rm);
  flag_sub16 (oper1, oper2);
}
static inline void opcode3C (void) { 
  // 3C CMP regs_.byteregs[regal] Ib 
  oper1b = regs_.byteregs[regal];
  oper2b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  flag_sub8 (oper1b, oper2b);
}
static inline void opcode3D (void) { 
  // 3D CMP eAX Iv 
  oper1 = getreg16 (regax);
  oper2 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  flag_sub16 (oper1, oper2);
}
static inline void opcode3F (void) { 
  // 3F AAS ASCII 
  if ( ( (regs_.byteregs[regal] & 0xF) > 9) || (af == 1) ) {
    regs_.byteregs[regal] = regs_.byteregs[regal] - 6;
    regs_.byteregs[regah] = regs_.byteregs[regah] - 1;
    af = 1;
    cf = 1;
  } else {
    af = 0;
    cf = 0;
  }
  regs_.byteregs[regal] = regs_.byteregs[regal] & 0xF;
}
static inline void opcode40 (void) { 
  // 40 INC eAX 
  oldcf = cf;
  oper1 = getreg16 (regax);
  oper2 = 1;
  op_add16();
  cf = oldcf;
  putreg16 (regax, res16);
}
static inline void opcode41 (void) { 
  // 41 INC eCX 
  oldcf = cf;
  oper1 = getreg16 (regcx);
  oper2 = 1;
  op_add16();
  cf = oldcf;
  putreg16 (regcx, res16);
}
static inline void opcode42 (void) { 
  // 42 INC eDX 
  oldcf = cf;
  oper1 = getreg16 (regdx);
  oper2 = 1;
  op_add16();
  cf = oldcf;
  putreg16 (regdx, res16);
}
static inline void opcode43 (void) { 
  // 43 INC eBX 
  oldcf = cf;
  oper1 = getreg16 (regbx);
  oper2 = 1;
  op_add16();
  cf = oldcf;
  putreg16 (regbx, res16);
}
static inline void opcode44 (void) { 
  // 44 INC eSP 
  oldcf = cf;
  oper1 = getreg16 (regsp);
  oper2 = 1;
  op_add16();
  cf = oldcf;
  putreg16 (regsp, res16);
}
static inline void opcode45 (void) { 
  // 45 INC eBP 
  oldcf = cf;
  oper1 = getreg16 (regbp);
  oper2 = 1;
  op_add16();
  cf = oldcf;
  putreg16 (regbp, res16);
}
static inline void opcode46 (void) { 
  // 46 INC eSI 
  oldcf = cf;
  oper1 = getreg16 (regsi);
  oper2 = 1;
  op_add16();
  cf = oldcf;
  putreg16 (regsi, res16);
}
static inline void opcode47 (void) { 
  // 47 INC eDI 
  oldcf = cf;
  oper1 = getreg16 (regdi);
  oper2 = 1;
  op_add16();
  cf = oldcf;
  putreg16 (regdi, res16);
}
static inline void opcode48 (void) { 
  // 48 DEC eAX *
  oldcf = cf;
  oper1 = getreg16 (regax);
  oper2 = 1;
  op_sub16();
  cf = oldcf;
  putreg16 (regax, res16);
}
static inline void opcode49 (void) { 
  // 49 DEC eCX 
  oldcf = cf;
  oper1 = getreg16 (regcx);
  oper2 = 1;
  op_sub16();
  cf = oldcf;
  putreg16 (regcx, res16);
}
static inline void opcode4A (void) { 
  // 4A DEC eDX 
  oldcf = cf;
  oper1 = getreg16 (regdx);
  oper2 = 1;
  op_sub16();
  cf = oldcf;
  putreg16 (regdx, res16);
}
static inline void opcode4B (void) { 
  // 4B DEC eBX 
  oldcf = cf;
  oper1 = getreg16 (regbx);
  oper2 = 1;
  op_sub16();
  cf = oldcf;
  putreg16 (regbx, res16);
}
static inline void opcode4C (void) { 
  // 4C DEC eSP 
  oldcf = cf;
  oper1 = getreg16 (regsp);
  oper2 = 1;
  op_sub16();
  cf = oldcf;
  putreg16 (regsp, res16);
}
static inline void opcode4D (void) { 
  // 4D DEC eBP 
  oldcf = cf;
  oper1 = getreg16 (regbp);
  oper2 = 1;
  op_sub16();
  cf = oldcf;
  putreg16 (regbp, res16);
}
static inline void opcode4E (void) { 
  // 4E DEC eSI 
  oldcf = cf;
  oper1 = getreg16 (regsi);
  oper2 = 1;
  op_sub16();
  cf = oldcf;
  putreg16 (regsi, res16);
}
static inline void opcode4F (void) { 
  // 4F DEC eDI 
  oldcf = cf;
  oper1 = getreg16 (regdi);
  oper2 = 1;
  op_sub16();
  cf = oldcf;
  putreg16 (regdi, res16);
}
static inline void opcode50 (void) { 
  // 50 PUSH eAX 
  push (getreg16 (regax) );
}
static inline void opcode51 (void) { 
  // 51 PUSH eCX 
  push (getreg16 (regcx) );
}
static inline void opcode52 (void) { 
  // 52 PUSH eDX 
  push (getreg16 (regdx) );
}
static inline void opcode53 (void) { 
  // 53 PUSH eBX 
  push (getreg16 (regbx) );
}
static inline void opcode54 (void) { 
  // 54 PUSH eSP 
  push (getreg16 (regsp) - 2);
}
static inline void opcode55 (void) { 
  // 55 PUSH eBP 
  push (getreg16 (regbp) );
}
static inline void opcode56 (void) { 
  // 56 PUSH eSI 
  push (getreg16 (regsi) );
}
static inline void opcode57 (void) { 
  // 57 PUSH eDI 
  push (getreg16 (regdi) );
}
static inline void opcode58 (void) { 
  // 58 POP eAX 
  putreg16 (regax, pop() );
}
static inline void opcode59 (void) { 
  // 59 POP eCX 
  putreg16 (regcx, pop() );
}
static inline void opcode5A (void) { 
  // 5A POP eDX 
  putreg16 (regdx, pop() );
}
static inline void opcode5B (void) { 
  // 5B POP eBX 
  putreg16 (regbx, pop() );
}
static inline void opcode5C (void) { 
  // 5C POP eSP 
  putreg16 (regsp, pop() );
}
static inline void opcode5D (void) { 
  // 5D POP eBP 
  putreg16 (regbp, pop() );
}
static inline void opcode5E (void) { 
  // 5E POP eSI 
  putreg16 (regsi, pop() );
}
static inline void opcode5F (void) { 
  // 5F POP eDI 
  putreg16 (regdi, pop() );
}

#ifdef CPU_V20
static inline void opcode60 (void) { 
  // 60 PUSHA (80186+) 
  oldsp = getreg16 (regsp);
  push (getreg16 (regax) );
  push (getreg16 (regcx) );
  push (getreg16 (regdx) );
  push (getreg16 (regbx) );
  push (oldsp);
  push (getreg16 (regbp) );
  push (getreg16 (regsi) );
  push (getreg16 (regdi) );
}
static inline void opcode61 (void) { 
  // 61 POPA (80186+) 
  putreg16 (regdi, pop() );
  putreg16 (regsi, pop() );
  putreg16 (regbp, pop() );
  dummy = pop();
  putreg16 (regbx, pop() );
  putreg16 (regdx, pop() );
  putreg16 (regcx, pop() );
  putreg16 (regax, pop() );
}
static inline void opcode62 (void) { 
  // 62 BOUND Gv, Ev (80186+) 
  modregrm();
  getea (rm);
  if (signext32 (getreg16 (reg_) ) < signext32 ( getmem16 (ea >> 4, ea & 15) ) ) {
    intcall86 (5); //bounds check exception
  } else {
    ea += 2;
    if (signext32 (getreg16 (reg_) ) > signext32 ( getmem16 (ea >> 4, ea & 15) ) ) {
      intcall86(5); //bounds check exception
    }
  }
}
static inline void opcode68 (void) { 
  // 68 PUSH Iv (80186+) 
  push (getmem16 (segregs[regcs], ip) );
  StepIP (2);
}
static inline void opcode69 (void) { 
  // 69 IMUL Gv Ev Iv (80186+) 
  modregrm();
  temp1 = readrm16 (rm);
  temp2 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  if ( (temp1 & 0x8000L) == 0x8000L) {
    temp1 = temp1 | 0xFFFF0000L;
  }
  if ( (temp2 & 0x8000L) == 0x8000L) {
    temp2 = temp2 | 0xFFFF0000L;
  }
  temp3 = temp1 * temp2;
  putreg16 (reg_, temp3 & 0xFFFFL);
  if (temp3 & 0xFFFF0000L) {
    cf = 1;
    of = 1;
  } else {
    cf = 0;
    of = 0;
  }
}
static inline void opcode6A (void) { 
  // 6A PUSH Ib (80186+) 
  push (getmem8 (segregs[regcs], ip) );
  StepIP (1);
}
static inline void opcode6B (void) { 
  // 6B IMUL Gv Eb Ib (80186+) 
  modregrm();
  temp1 = readrm16 (rm);
  temp2 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if ( (temp1 & 0x8000L) == 0x8000L) {
    temp1 = temp1 | 0xFFFF0000L;
  }
  if ( (temp2 & 0x8000L) == 0x8000L) {
    temp2 = temp2 | 0xFFFF0000L;
  }
  temp3 = temp1 * temp2;
  putreg16 (reg_, temp3 & 0xFFFFL);
  if (temp3 & 0xFFFF0000L) {
    cf = 1;
    of = 1;
  } else {
    cf = 0;
    of = 0;
  }
}
static inline void opcode6C (void) { 
  // 6E INSB 
  if (reptype && (getreg16 (regcx) == 0) ) {
    return;
  }

  putmem8 (useseg, getreg16 (regsi) , portin (regs_.wordregs[regdx]) );
  if (df) {
    putreg16 (regsi, getreg16 (regsi) - 1);
    putreg16 (regdi, getreg16 (regdi) - 1);
  } else {
    putreg16 (regsi, getreg16 (regsi) + 1);
    putreg16 (regdi, getreg16 (regdi) + 1);
  }
  if (reptype) {
    putreg16 (regcx, getreg16 (regcx) - 1);
  }
  totalexec++;
  loopcount++;
  if (!reptype) {
    return;
  }
  ip = firstip;
}
static inline void opcode6D (void) { 
  // 6F INSW 
  if (reptype && (getreg16 (regcx) == 0) ) {
    return;
  }
  putmem16 (useseg, getreg16 (regsi) , portin16 (regs_.wordregs[regdx]) );
  if (df) {
    putreg16 (regsi, getreg16 (regsi) - 2);
    putreg16 (regdi, getreg16 (regdi) - 2);
  } else {
    putreg16 (regsi, getreg16 (regsi) + 2);
    putreg16 (regdi, getreg16 (regdi) + 2);
  }
  if (reptype) {
    putreg16 (regcx, getreg16 (regcx) - 1);
  }
  totalexec++;
  loopcount++;
  if (!reptype) {
    return;
  }
  ip = firstip;
}
static inline void opcode6E (void) { 
  // 6E OUTSB 
  if (reptype && (getreg16 (regcx) == 0) ) {
    return;
  }
  portout (regs_.wordregs[regdx], getmem8 (useseg, getreg16 (regsi) ) );
  if (df) {
    putreg16 (regsi, getreg16 (regsi) - 1);
    putreg16 (regdi, getreg16 (regdi) - 1);
  } else {
    putreg16 (regsi, getreg16 (regsi) + 1);
    putreg16 (regdi, getreg16 (regdi) + 1);
  }
  if (reptype) {
    putreg16 (regcx, getreg16 (regcx) - 1);
  }
  totalexec++;
  loopcount++;
  if (!reptype) {
    return;
  }
  ip = firstip;
}
static inline void opcode6F (void) { 
  // 6F OUTSW 
  if (reptype && (getreg16 (regcx) == 0) ) {
    return;
  }
  portout16 (regs_.wordregs[regdx], getmem16 (useseg, getreg16 (regsi) ) );
  if (df) {
    putreg16 (regsi, getreg16 (regsi) - 2);
    putreg16 (regdi, getreg16 (regdi) - 2);
  } else {
    putreg16 (regsi, getreg16 (regsi) + 2);
    putreg16 (regdi, getreg16 (regdi) + 2);
  }
  if (reptype) {
    putreg16 (regcx, getreg16 (regcx) - 1);
  }
  totalexec++;
  loopcount++;
  if (!reptype) {
    return;
  }
  ip = firstip;
}
#endif
static inline void opcode70 (void) { 
  // 70 JO Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (of) {
    ip = ip + temp16;
  }
}
static inline void opcode71 (void) { 
  // 71 JNO Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (!of) {
    ip = ip + temp16;
  }
}
static inline void opcode72 (void) { 
  // 72 JB Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (cf) {
    ip = ip + temp16;
  }
}
static inline void opcode73 (void) { 
  // 73 JNB Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (!cf) {
    ip = ip + temp16;
  }
}
static inline void opcode74 (void) { 
  // 74 JZ Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (zf) {
    ip = ip + temp16;
  }
}
static inline void opcode75 (void) { 
  // 75 JNZ Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (!zf) {
    ip = ip + temp16;
  }
}
static inline void opcode76 (void) { 
  // 76 JBE Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (cf || zf) {
    ip = ip + temp16;
  }
}
static inline void opcode77 (void) {
  // 77 JA Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (!cf && !zf) {
    ip = ip + temp16;
  }
}
static inline void opcode78 (void) { 
  // 78 JS Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (sf) {
    ip = ip + temp16;
  }
}
static inline void opcode79 (void) { 
  // 79 JNS Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (!sf) {
    ip = ip + temp16;
  }
}
static inline void opcode7A (void) { 
  // 7A JPE Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (pf) {
    ip = ip + temp16;
  }
}
static inline void opcode7B (void) { 
  // 7B JPO Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (!pf) {
    ip = ip + temp16;
  }
}
static inline void opcode7C (void) { 
  // 7C JL Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (sf != of) {
    ip = ip + temp16;
  }
}
static inline void opcode7D (void) { 
  // 7D JGE Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (sf == of) {
    ip = ip + temp16;
  }
}
static inline void opcode7E (void) { 
  // 7E JLE Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if ( (sf != of) || zf) {
    ip = ip + temp16;
  }
}
static inline void opcode7F (void) { 
    // 7F JG Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (!zf && (sf == of) ) {
    ip = ip + temp16;
  }
}
static inline void opcode80 (void) { 
  // 80/82 GRP1 Eb Ib 
  modregrm();
  oper1b = readrm8 (rm);
  oper2b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  switch (reg_) {
    case 0:
      op_add8();
      break;
    case 1:
      op_or8();
      break;
    case 2:
      op_adc8();
      break;
    case 3:
      op_sbb8();
      break;
    case 4:
      op_and8();
      break;
    case 5:
      op_sub8();
      break;
    case 6:
      op_xor8();
      break;
    case 7:
      flag_sub8 (oper1b, oper2b);
      break;
    default:
      break;  // to avoid compiler warnings 
  }
  if (reg_ < 7) {
    writerm8 (rm, res8);
  }
}
static inline void opcode82 (void) { 
  // 80/82 GRP1 Eb Ib 
  modregrm();
  oper1b = readrm8 (rm);
  oper2b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  switch (reg_) {
    case 0:
      op_add8();
      break;
    case 1:
      op_or8();
      break;
    case 2:
      op_adc8();
      break;
    case 3:
      op_sbb8();
      break;
    case 4:
      op_and8();
      break;
    case 5:
      op_sub8();
      break;
    case 6:
      op_xor8();
      break;
    case 7:
      flag_sub8 (oper1b, oper2b);
      break;
    default:
      break;  // to avoid compiler warnings 
  }

  if (reg_ < 7) {
    writerm8 (rm, res8);
  }
}
static inline void opcode81 (void) { 
  // 81 GRP1 Ev Iv 
  modregrm();
  oper1 = readrm16 (rm);
  if (opcode == 0x81) {
    oper2 = getmem16 (segregs[regcs], ip);
    StepIP (2);
  } else {
    oper2 = signext (getmem8 (segregs[regcs], ip) );
    StepIP (1);
  }
  switch (reg_) {
    case 0:
      op_add16();
      break;
    case 1:
      op_or16();
      break;
    case 2:
      op_adc16();
      break;
    case 3:
      op_sbb16();
      break;
    case 4:
      op_and16();
      break;
    case 5:
      op_sub16();
      break;
    case 6:
      op_xor16();
      break;
    case 7:
      flag_sub16 (oper1, oper2);
      break;
    default:
      break;  // to avoid compiler warnings */
  }
  if (reg_ < 7) {
    writerm16 (rm, res16);
  }
}
static inline void opcode83 (void) { 
  // 83 GRP1 Ev Ib 
  modregrm();
  oper1 = readrm16 (rm);
  if (opcode == 0x81) {
    oper2 = getmem16 (segregs[regcs], ip);
    StepIP (2);
  } else {
    oper2 = signext (getmem8 (segregs[regcs], ip) );
    StepIP (1);
  }
  switch (reg_) {
    case 0:
      op_add16();
      break;
    case 1:
      op_or16();
      break;
    case 2:
      op_adc16();
      break;
    case 3:
      op_sbb16();
      break;
    case 4:
      op_and16();
      break;
    case 5:
      op_sub16();
      break;
    case 6:
      op_xor16();
      break;
    case 7:
      flag_sub16 (oper1, oper2);
      break;
    default:
      break;  // to avoid compiler warnings 
  }
  if (reg_ < 7) {
    writerm16 (rm, res16);
  }
}
static inline void opcode84 (void) { 
  // 84 TEST Gb Eb 
  modregrm();
  oper1b = getreg8 (reg_);
  oper2b = readrm8 (rm);
  flag_log8 (oper1b & oper2b);
}
static inline void opcode85 (void) { 
  // 85 TEST Gv Ev 
  modregrm();
  oper1 = getreg16 (reg_);
  oper2 = readrm16 (rm);
  flag_log16 (oper1 & oper2);
}
static inline void opcode86 (void) { 
  // 86 XCHG Gb Eb 
  modregrm();
  oper1b = getreg8 (reg_);
  putreg8 (reg_, readrm8 (rm) );
  writerm8 (rm, oper1b);
}
static inline void opcode87 (void) { 
  // 87 XCHG Gv Ev 
  modregrm();
  oper1 = getreg16 (reg_);
  putreg16 (reg_, readrm16 (rm) );
  writerm16 (rm, oper1);
}
static inline void opcode88 (void) { 
  // 88 MOV Eb Gb 
  modregrm();
  writerm8 (rm, getreg8 (reg_) );
}

static inline void opcode89 (void) { 
  // 89 MOV Ev Gv 
  modregrm();
  writerm16 (rm, getreg16 (reg_) );
}

static inline void opcode8A (void) { 
  // 8A MOV Gb Eb 
  modregrm();
  putreg8 (reg_, readrm8 (rm) );
}

static inline void opcode8B (void) { 
  // 8B MOV Gv Ev 
  modregrm();
  putreg16 (reg_, readrm16 (rm) );
}

static inline void opcode8C (void) { 
  // 8C MOV Ew Sw 
  modregrm();
  writerm16 (rm, getsegreg (reg_) );
}

static inline void opcode8D (void) { 
  // 8D LEA Gv M 
  modregrm();
  getea (rm);
  putreg16 (reg_, ea - segbase (useseg) );
}

static inline void opcode8E (void) { 
  // 8E MOV Sw Ew 
  modregrm();
  putsegreg (reg_, readrm16 (rm) );
}

static inline void opcode8F (void) { 
  // 8F POP Ev 
  modregrm();
  writerm16 (rm, pop() );
}

static inline void opcode90 (void) { 
  // 90 NOP 
}

static inline void opcode91 (void) { 
  // 91 XCHG eCX eAX 
  oper1 = getreg16 (regcx);
  putreg16 (regcx, getreg16 (regax) );
  putreg16 (regax, oper1);
}

static inline void opcode92 (void) { 
  // 92 XCHG eDX eAX 
  oper1 = getreg16 (regdx);
  putreg16 (regdx, getreg16 (regax) );
  putreg16 (regax, oper1);
}

static inline void opcode93 (void) { 
  // 93 XCHG eBX eAX 
  oper1 = getreg16 (regbx);
  putreg16 (regbx, getreg16 (regax) );
  putreg16 (regax, oper1);
}

static inline void opcode94 (void) { 
  // 94 XCHG eSP eAX 
  oper1 = getreg16 (regsp);
  putreg16 (regsp, getreg16 (regax) );
  putreg16 (regax, oper1);
}

static inline void opcode95 (void) { 
  // 95 XCHG eBP eAX 
  oper1 = getreg16 (regbp);
  putreg16 (regbp, getreg16 (regax) );
  putreg16 (regax, oper1);
}

static inline void opcode96 (void) { 
  // 96 XCHG eSI eAX 
  oper1 = getreg16 (regsi);
  putreg16 (regsi, getreg16 (regax) );
  putreg16 (regax, oper1);
}

static inline void opcode97 (void) { 
  // 97 XCHG eDI eAX 
  oper1 = getreg16 (regdi);
  putreg16 (regdi, getreg16 (regax) );
  putreg16 (regax, oper1);
}

static inline void opcode98 (void) { 
  // 98 CBW 
  if ( (regs_.byteregs[regal] & 0x80) == 0x80) {
    regs_.byteregs[regah] = 0xFF;
  } else {
    regs_.byteregs[regah] = 0;
  }
}

static inline void opcode99 (void) { 
  // 99 CWD 
  if ( (regs_.byteregs[regah] & 0x80) == 0x80) {
    putreg16 (regdx, 0xFFFF);
  } else {
    putreg16 (regdx, 0);
  }
}

static inline void opcode9A (void) { 
  // 9A CALL Ap 
  oper1 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  oper2 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  push (segregs[regcs]);
  push (ip);
  ip = oper1;
  segregs[regcs] = oper2;
}

static inline void opcode9B (void) { 
  // 9B WAIT 
}

static inline void opcode9C (void) { 
  // 9C PUSHF 
  push (makeflagsword() | 0xF800);
}

static inline void opcode9D (void) { 
  // 9D POPF 
  temp16 = pop();
  decodeflagsword (temp16);
}

static inline void opcode9E (void) { 
  // 9E SAHF 
  decodeflagsword ( (makeflagsword() & 0xFF00) | regs_.byteregs[regah]);
}

static inline void opcode9F (void) { 
  // 9F LAHF 
  regs_.byteregs[regah] = makeflagsword() & 0xFF;
}

static inline void opcodeA0 (void) { 
  // A0 MOV regs_.byteregs[regal] Ob 
  regs_.byteregs[regal] = getmem8 (useseg, getmem16 (segregs[regcs], ip) );
  StepIP (2);
}

static inline void opcodeA1 (void) { 
  // A1 MOV eAX Ov 
  oper1 = getmem16 (useseg, getmem16 (segregs[regcs], ip) );
  StepIP (2);
  putreg16 (regax, oper1);
}

static inline void opcodeA2 (void) { 
  // A2 MOV Ob regs_.byteregs[regal] 
  putmem8 (useseg, getmem16 (segregs[regcs], ip), regs_.byteregs[regal]);
  StepIP (2);
}

static inline void opcodeA3 (void) { 
  // A3 MOV Ov eAX 
  putmem16 (useseg, getmem16 (segregs[regcs], ip), getreg16 (regax) );
  StepIP (2);
}

static inline void opcodeA4 (void) { 
  // A4 MOVSB 
  if (reptype && (getreg16 (regcx) == 0) ) {
    return;
  }
  putmem8 (segregs[reges], getreg16 (regdi), getmem8 (useseg, getreg16 (regsi) ) );
  if (df) {
    putreg16 (regsi, getreg16 (regsi) - 1);
    putreg16 (regdi, getreg16 (regdi) - 1);
  } else {
    putreg16 (regsi, getreg16 (regsi) + 1);
    putreg16 (regdi, getreg16 (regdi) + 1);
  }
  if (reptype) {
    putreg16 (regcx, getreg16 (regcx) - 1);
  }
  totalexec++;
  loopcount++;
  if (!reptype) {
    return;
  }
  ip = firstip;
}

static inline void opcodeA5 (void) { 
  // A5 MOVSW 
  if (reptype && (getreg16 (regcx) == 0) ) {
    return;
  }

  putmem16 (segregs[reges], getreg16 (regdi), getmem16 (useseg, getreg16 (regsi) ) );
  if (df) {
    putreg16 (regsi, getreg16 (regsi) - 2);
    putreg16 (regdi, getreg16 (regdi) - 2);
  } else {
    putreg16 (regsi, getreg16 (regsi) + 2);
    putreg16 (regdi, getreg16 (regdi) + 2);
   }

  if (reptype) {
    putreg16 (regcx, getreg16 (regcx) - 1);
  }
  totalexec++;
  loopcount++;
  if (!reptype) {
    return;
  }
  ip = firstip;
}

static inline void opcodeA6 (void) { 
  // A6 CMPSB 
  if (reptype && (getreg16 (regcx) == 0) ) {
    return;
  }

  oper1b = getmem8 (useseg, getreg16 (regsi) );
  oper2b = getmem8 (segregs[reges], getreg16 (regdi) );
  if (df) {
    putreg16 (regsi, getreg16 (regsi) - 1);
    putreg16 (regdi, getreg16 (regdi) - 1);
  } else {
    putreg16 (regsi, getreg16 (regsi) + 1);
    putreg16 (regdi, getreg16 (regdi) + 1);
  }
  flag_sub8 (oper1b, oper2b);
  if (reptype) {
    putreg16 (regcx, getreg16 (regcx) - 1);
  }
  if ( (reptype == 1) && !zf) {
    return;
  } else if ( (reptype == 2) && (zf == 1) ) {
    return;
  }
  totalexec++;
  loopcount++;
  if (!reptype) {
    return;
  }
  ip = firstip;
}

static inline void opcodeA7 (void) { 
  // A7 CMPSW 
  if (reptype && (getreg16 (regcx) == 0) ) {
    return;
  }
  oper1 = getmem16 (useseg, getreg16 (regsi) );
  oper2 = getmem16 (segregs[reges], getreg16 (regdi) );
  if (df) {
    putreg16 (regsi, getreg16 (regsi) - 2);
    putreg16 (regdi, getreg16 (regdi) - 2);
  } else {
    putreg16 (regsi, getreg16 (regsi) + 2);
    putreg16 (regdi, getreg16 (regdi) + 2);
  }
  flag_sub16 (oper1, oper2);
  if (reptype) {
    putreg16 (regcx, getreg16 (regcx) - 1);
  }

  if ( (reptype == 1) && !zf) {
    return;
  }

  if ( (reptype == 2) && (zf == 1) ) {
    return;
  }

  totalexec++;
  loopcount++;
  if (!reptype) {
    return;
  }
  ip = firstip;
}

static inline void opcodeA8 (void) { 
  // A8 TEST regs_.byteregs[regal] Ib 
  oper1b = regs_.byteregs[regal];
  oper2b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  flag_log8 (oper1b & oper2b);
}

static inline void opcodeA9 (void) { 
  // A9 TEST eAX Iv 
  oper1 = getreg16 (regax);
  oper2 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  flag_log16 (oper1 & oper2);
}

static inline void opcodeAA (void) { 
  // AA STOSB 
  if (reptype && (getreg16 (regcx) == 0) ) {
    return;
  }
  putmem8 (segregs[reges], getreg16 (regdi), regs_.byteregs[regal]);
  if (df) {
    putreg16 (regdi, getreg16 (regdi) - 1);
  } else {
    putreg16 (regdi, getreg16 (regdi) + 1);
  }
  if (reptype) {
    putreg16 (regcx, getreg16 (regcx) - 1);
  }
  totalexec++;
  loopcount++;
  if (!reptype) {
    return;
  }
  ip = firstip;
}

static inline void opcodeAB (void) { 
  // AB STOSW 
  if (reptype && (getreg16 (regcx) == 0) ) {
    return;
  }
  putmem16 (segregs[reges], getreg16 (regdi), getreg16 (regax) );
  if (df) {
    putreg16 (regdi, getreg16 (regdi) - 2);
  } else {
    putreg16 (regdi, getreg16 (regdi) + 2);
  }

  if (reptype) {
    putreg16 (regcx, getreg16 (regcx) - 1);
  }

  totalexec++;
  loopcount++;
  if (!reptype) {
    return;
  }
  ip = firstip;
}

static inline void opcodeAC (void) { 
  // AC LODSB 
  if (reptype && (getreg16 (regcx) == 0) ) {
    return;
  }
  regs_.byteregs[regal] = getmem8 (useseg, getreg16 (regsi) );
  if (df) {
    putreg16 (regsi, getreg16 (regsi) - 1);
  } else {
    putreg16 (regsi, getreg16 (regsi) + 1);
  }
  if (reptype) {
    putreg16 (regcx, getreg16 (regcx) - 1);
  }
  totalexec++;
  loopcount++;
  if (!reptype) {
    return;
  }
  ip = firstip;
}

static inline void opcodeAD (void) { 
  // AD LODSW 
  if (reptype && (getreg16 (regcx) == 0) ) {
    return;
  }
  oper1 = getmem16 (useseg, getreg16 (regsi) );
  putreg16 (regax, oper1);
  if (df) {
    putreg16 (regsi, getreg16 (regsi) - 2);
  } else {
    putreg16 (regsi, getreg16 (regsi) + 2);
  }
  if (reptype) {
    putreg16 (regcx, getreg16 (regcx) - 1);
  }

  totalexec++;
  loopcount++;
  if (!reptype) {
    return;
  }

  ip = firstip;
}

static inline void opcodeAE (void) { 
  // AE SCASB 
  if (reptype && (getreg16 (regcx) == 0) ) {
    return;
  }
  oper1b = getmem8 (segregs[reges], getreg16 (regdi) );
  oper2b = regs_.byteregs[regal];
  flag_sub8 (oper1b, oper2b);
  if (df) {
    putreg16 (regdi, getreg16 (regdi) - 1);
  } else {
    putreg16 (regdi, getreg16 (regdi) + 1);
  }
  if (reptype) {
    putreg16 (regcx, getreg16 (regcx) - 1);
  }
  if ( (reptype == 1) && !zf) {
    return;
  } else if ( (reptype == 2) && (zf == 1) ) {
    return;
  }
  totalexec++;
  loopcount++;
  if (!reptype) {
      return;
  }
  ip = firstip;
}

static inline void opcodeAF (void) { 
  // AF SCASW 
  if (reptype && (getreg16 (regcx) == 0) ) {
    return;
  }
  oper1 = getmem16 (segregs[reges], getreg16 (regdi) );
  oper2 = getreg16 (regax);
  flag_sub16 (oper1, oper2);
  if (df) {
    putreg16 (regdi, getreg16 (regdi) - 2);
  } else {
    putreg16 (regdi, getreg16 (regdi) + 2);
  }
  if (reptype) {
    putreg16 (regcx, getreg16 (regcx) - 1);
  }
  if ( (reptype == 1) && !zf) {
    return;
  } else if ( (reptype == 2) & (zf == 1) ) {
    return;
  }
  totalexec++;
  loopcount++;
  if (!reptype) {
    return;
  }
  ip = firstip;
}

static inline void opcodeB0 (void) { 
  // B0 MOV regs_.byteregs[regal] Ib 
  regs_.byteregs[regal] = getmem8 (segregs[regcs], ip);
  StepIP (1);
}

static inline void opcodeB1 (void) { 
  // B1 MOV regs_.byteregs[regcl] Ib 
  regs_.byteregs[regcl] = getmem8 (segregs[regcs], ip);
  StepIP (1);
}

static inline void opcodeB2 (void) { 
  // B2 MOV regs_.byteregs[regdl] Ib 
  regs_.byteregs[regdl] = getmem8 (segregs[regcs], ip);
  StepIP (1);
}

static inline void opcodeB3 (void) { 
  // B3 MOV regs_.byteregs[regbl] Ib 
  regs_.byteregs[regbl] = getmem8 (segregs[regcs], ip);
  StepIP (1);
}

static inline void opcodeB4 (void) { 
  // B4 MOV regs_.byteregs[regah] Ib 
  regs_.byteregs[regah] = getmem8 (segregs[regcs], ip);
  StepIP (1);
}

static inline void opcodeB5 (void) { 
  // B5 MOV regs_.byteregs[regch] Ib 
  regs_.byteregs[regch] = getmem8 (segregs[regcs], ip);
  StepIP (1);
}

static inline void opcodeB6 (void) { 
  // B6 MOV regs_.byteregs[regdh] Ib 
  regs_.byteregs[regdh] = getmem8 (segregs[regcs], ip);
  StepIP (1);
}

static inline void opcodeB7 (void) { 
  // B7 MOV regs_.byteregs[regbh] Ib 
  regs_.byteregs[regbh] = getmem8 (segregs[regcs], ip);
  StepIP (1);
}

static inline void opcodeB8 (void) { 
  // B8 MOV eAX Iv 
  oper1 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  putreg16 (regax, oper1);
}

static inline void opcodeB9 (void) { 
  // B9 MOV eCX Iv 
  oper1 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  putreg16 (regcx, oper1);
}

static inline void opcodeBA (void) { 
  // BA MOV eDX Iv 
  oper1 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  putreg16 (regdx, oper1);
}

static inline void opcodeBB (void) { 
  // BB MOV eBX Iv 
  oper1 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  putreg16 (regbx, oper1);
}

static inline void opcodeBC (void) { 
  // BC MOV eSP Iv 
  putreg16 (regsp, getmem16 (segregs[regcs], ip) );
  StepIP (2);
}

static inline void opcodeBD (void) { 
  // BD MOV eBP Iv 
  putreg16 (regbp, getmem16 (segregs[regcs], ip) );
  StepIP (2);
}

static inline void opcodeBE (void) { 
  // BE MOV eSI Iv 
  putreg16 (regsi, getmem16 (segregs[regcs], ip) );
  StepIP (2);
}

static inline void opcodeBF (void) { 
  // BF MOV eDI Iv 
  putreg16 (regdi, getmem16 (segregs[regcs], ip) );
  StepIP (2);
}

static inline void opcodeC0 (void) { 
  // C0 GRP2 byte imm8 (80186+) 
  modregrm();
  oper1b = readrm8 (rm);
  oper2b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  writerm8 (rm, op_grp2_8 (oper2b) );
}

static inline void opcodeC1 (void) { 
  // C1 GRP2 word imm8 (80186+) 
  modregrm();
  oper1 = readrm16 (rm);
  oper2 = getmem8 (segregs[regcs], ip);
  StepIP (1);
  writerm16 (rm, op_grp2_16 ( (uint8_t) oper2) );
}

static inline void opcodeC2 (void) { 
  // C2 RET Iw 
  oper1 = getmem16 (segregs[regcs], ip);
  ip = pop();
  putreg16 (regsp, getreg16 (regsp) + oper1);
}

static inline void opcodeC3 (void) { 
  // C3 RET 
  ip = pop();
}

static inline void opcodeC4 (void) { 
  // C4 LES Gv Mp 
  modregrm();
  getea (rm);
  putreg16 (reg_, read86 (ea) + read86 (ea + 1) * 256);
  segregs[reges] = read86 (ea + 2) + read86 (ea + 3) * 256;
}

static inline void opcodeC5 (void) { 
  // C5 LDS Gv Mp 
  modregrm();
  getea (rm);
  putreg16 (reg_, read86 (ea) + read86 (ea + 1) * 256);
  segregs[regds] = read86 (ea + 2) + read86 (ea + 3) * 256;
}

static inline void opcodeC6 (void) { 
  // C6 MOV Eb Ib 
  modregrm();
  writerm8 (rm, getmem8 (segregs[regcs], ip) );
  StepIP (1);
}

static inline void opcodeC7 (void) { 
  // C7 MOV Ev Iv 
  modregrm();
  writerm16 (rm, getmem16 (segregs[regcs], ip) );
  StepIP (2);
}

static inline void opcodeC8 (void) { 
  // C8 ENTER (80186+) 
  stacksize = getmem16 (segregs[regcs], ip);
  StepIP (2);
  nestlev = getmem8 (segregs[regcs], ip);
  StepIP (1);
  push (getreg16 (regbp) );
  frametemp = getreg16 (regsp);
  if (nestlev) {
    for (temp16 = 1; temp16 < nestlev; temp16++) {
      putreg16 (regbp, getreg16 (regbp) - 2);
      push (getreg16 (regbp) );
    }
    push (getreg16 (regsp) );
  }
  putreg16 (regbp, frametemp);
  putreg16 (regsp, getreg16 (regbp) - stacksize);
}

static inline void opcodeC9 (void) { 
  // C9 LEAVE (80186+) 
  putreg16 (regsp, getreg16 (regbp) );
  putreg16 (regbp, pop() );
}

static inline void opcodeCA (void) { 
  // CA RETF Iw 
  oper1 = getmem16 (segregs[regcs], ip);
  ip = pop();
  segregs[regcs] = pop();
  putreg16 (regsp, getreg16 (regsp) + oper1);
}

static inline void opcodeCB (void) { 
  // CB RETF 
  ip = pop();;
  segregs[regcs] = pop();
}

static inline void opcodeCC (void) { 
  // CC INT 3 
  intcall86 (3);
}

static inline void opcodeCD (void) { 
  // CD INT Ib 
  oper1b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  intcall86 (oper1b);
}

static inline void opcodeCE (void) { 
  // CE INTO 
  if (of) {
    intcall86 (4);
  }
}

static inline void opcodeCF (void) { 
  // CF IRET 
  ip = pop();
  segregs[regcs] = pop();
  decodeflagsword (pop() );            
   // if (net.enabled) net.canrecv = 1;
             
}

static inline void opcodeD0 (void) { 
  // D0 GRP2 Eb 1 
  modregrm();
  oper1b = readrm8 (rm);
  writerm8 (rm, op_grp2_8 (1) );
}

static inline void opcodeD1 (void) { 
  // D1 GRP2 Ev 1 
  modregrm();
  oper1 = readrm16 (rm);
  writerm16 (rm, op_grp2_16 (1) );
}

static inline void opcodeD2 (void) { 
  // D2 GRP2 Eb regs_.byteregs[regcl] 
  modregrm();
  oper1b = readrm8 (rm);
  writerm8 (rm, op_grp2_8 (regs_.byteregs[regcl]) );
}

static inline void opcodeD3 (void) { 
  // D3 GRP2 Ev regs_.byteregs[regcl] 
  modregrm();
  oper1 = readrm16 (rm);
  writerm16 (rm, op_grp2_16 (regs_.byteregs[regcl]) );
}

static inline void opcodeD4 (void) { 
  // D4 AAM I0 
  oper1 = getmem8 (segregs[regcs], ip);
  StepIP (1);
  if (!oper1) {
    intcall86 (0);
      return;
  } // division by zero 

  regs_.byteregs[regah] = (regs_.byteregs[regal] / oper1) & 255;
  regs_.byteregs[regal] = (regs_.byteregs[regal] % oper1) & 255;
  flag_szp16 (getreg16 (regax) );
}

static inline void opcodeD5 (void) { 
  // D5 AAD I0 
  oper1 = getmem8 (segregs[regcs], ip);
  StepIP (1);
  regs_.byteregs[regal] = (regs_.byteregs[regah] * oper1 + regs_.byteregs[regal]) & 255;
  regs_.byteregs[regah] = 0;
  flag_szp16 (regs_.byteregs[regah] * oper1 + regs_.byteregs[regal]);
  sf = 0;
}

static inline void opcodeD6 (void) { 
  // D6 XLAT on V20/V30, SALC on 8086/8088 
#ifndef CPU_V20
  regs_.byteregs[regal] = cf ? 0xFF : 0x00;
#endif
}

static inline void opcodeD7 (void) { 
  // D7 XLAT 
  regs_.byteregs[regal] = read86(useseg * 16 + (regs_.wordregs[regbx]) + regs_.byteregs[regal]);
}

static inline void opcodeD8 (void) {  
  // escape to x87 FPU (unsupported) 
  modregrm();
}
static inline void opcodeD9 (void) { 
  // escape to x87 FPU (unsupported) 
  modregrm();
}
static inline void opcodeDA (void) { 
  // escape to x87 FPU (unsupported) 
  modregrm();
}
static inline void opcodeDB (void) { 
  // escape to x87 FPU (unsupported) 
  modregrm();
}
static inline void opcodeDC (void) { 
  // escape to x87 FPU (unsupported) 
  modregrm();
}
static inline void opcodeDD (void) { 
  // escape to x87 FPU (unsupported) 
  modregrm();
}
static inline void opcodeDE (void) { 
  // escape to x87 FPU (unsupported) 
  modregrm();
}
static inline void opcodeDF (void) { 
  // escape to x87 FPU (unsupported) 
  modregrm();
}

static inline void opcodeE0 (void) { 
  // E0 LOOPNZ Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  putreg16 (regcx, getreg16 (regcx) - 1);
  if ( (getreg16 (regcx) ) && !zf) {
    ip = ip + temp16;
  }
}

static inline void opcodeE1 (void) { 
  // E1 LOOPZ Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  putreg16 (regcx, (getreg16 (regcx) ) - 1);
  if ( (getreg16 (regcx) ) && (zf == 1) ) {
    ip = ip + temp16;
  }
}

static inline void opcodeE2 (void) { 
  // E2 LOOP Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  putreg16 (regcx, (getreg16 (regcx) ) - 1);
  if (getreg16 (regcx) ) {
    ip = ip + temp16;
  }
}

static inline void opcodeE3 (void) { 
  // E3 JCXZ Jb 
  temp16 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  if (! (getreg16 (regcx) ) ) {
    ip = ip + temp16;
  }
}

static inline void opcodeE4 (void) { 
  // E4 IN regs_.byteregs[regal] Ib 
  oper1b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  regs_.byteregs[regal] = (uint8_t) portin (oper1b);
}

static inline void opcodeE5 (void) { 
  // E5 IN eAX Ib 
  oper1b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  putreg16 (regax, portin16 (oper1b) );
}

static inline void opcodeE6 (void) { 
  // E6 OUT Ib regs_.byteregs[regal] 
  oper1b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  portout (oper1b, regs_.byteregs[regal]);
}

static inline void opcodeE7 (void) { 
  // E7 OUT Ib eAX 
  oper1b = getmem8 (segregs[regcs], ip);
  StepIP (1);
  portout16 (oper1b, (getreg16 (regax) ) );
}

static inline void opcodeE8 (void) { 
  // E8 CALL Jv 
  oper1 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  push (ip);
  ip = ip + oper1;
}

static inline void opcodeE9 (void) { 
  // E9 JMP Jv 
  oper1 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  ip = ip + oper1;
}

static inline void opcodeEA (void) { 
  // EA JMP Ap 
  oper1 = getmem16 (segregs[regcs], ip);
  StepIP (2);
  oper2 = getmem16 (segregs[regcs], ip);
  ip = oper1;
  segregs[regcs] = oper2;
}

static inline void opcodeEB (void) { 
  // EB JMP Jb 
  oper1 = signext (getmem8 (segregs[regcs], ip) );
  StepIP (1);
  ip = ip + oper1;
}

static inline void opcodeEC (void) { 
  // EC IN regs_.byteregs[regal] regdx 
  oper1 = (getreg16 (regdx) );
  regs_.byteregs[regal] = (uint8_t) portin (oper1);
}

static inline void opcodeED (void) { 
  // ED IN eAX regdx 
  oper1 = (getreg16 (regdx) );
  putreg16 (regax, portin16 (oper1) );
}

static inline void opcodeEE (void) { 
  // EE OUT regdx regs_.byteregs[regal] 
  oper1 = (getreg16 (regdx) );
  portout (oper1, regs_.byteregs[regal]);
}

static inline void opcodeEF (void) { 
  // EF OUT regdx eAX 
  oper1 = (getreg16 (regdx) );
  portout16 (oper1, (getreg16 (regax) ) );
}

static inline void opcodeF0 (void) { 
  // F0 LOCK 
}

static inline void opcodeF4 (void) { 
  // F4 HLT 
  ip--;
}

static inline void opcodeF5 (void) { 
  // F5 CMC 
  if (!cf) {
      cf = 1;
  } else {
    cf = 0;
  }
}

static inline void opcodeF6 (void) { 
  // F6 GRP3a Eb 
  modregrm();
  oper1b = readrm8 (rm);
  op_grp3_8();
  if ( (reg_ > 1) && (reg_ < 4) ) {
    writerm8 (rm, res8);
  }
}

static inline void opcodeF7 (void) { 
  // F7 GRP3b Ev 
  modregrm();
  oper1 = readrm16 (rm);
  op_grp3_16();
  if ( (reg_ > 1) && (reg_ < 4) ) {
    writerm16 (rm, res16);
  }
}

static inline void opcodeF8 (void) { 
  // F8 CLC 
  cf = 0;
}

static inline void opcodeF9 (void) { 
  // F9 STC 
  cf = 1;
}

static inline void opcodeFA (void) { 
  // FA CLI 
  ifl = 0;
}

static inline void opcodeFB (void) { 
  // FB STI 
  ifl = 1;
}

static inline void opcodeFC (void) { 
  // FC CLD 
  df = 0;
}

static inline void opcodeFD (void) { 
  // FD STD 
  df = 1;
}

static inline void opcodeFE (void) { 
  // FE GRP4 Eb 
  modregrm();
  oper1b = readrm8 (rm);
  oper2b = 1;
  if (!reg_) {
    tempcf = cf;
    res8 = oper1b + oper2b;
    flag_add8 (oper1b, oper2b);
    cf = tempcf;
    writerm8 (rm, res8);
  } else {
    tempcf = cf;
    res8 = oper1b - oper2b;
    flag_sub8 (oper1b, oper2b);
    cf = tempcf;
    writerm8 (rm, res8);
  }
}

static inline void opcodeFF (void) { 
  // FF GRP5 Ev 
  modregrm();
  oper1 = readrm16 (rm);
  op_grp5();
}

#ifndef CPU_V20
static inline void opcode60 (void) {  //unknown 
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode61 (void) {  //unknown 
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode62 (void) {  //unknown 
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode68 (void) {  //unknown
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode69 (void) {  //unknown
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode6A (void) {  //unknown
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode6B (void) {  //unknown
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode6C (void) {  //unknown
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode6D (void) {  //unknown
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode6E (void) {  //unknown
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode6F (void) {  //unknown 
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
#endif

static inline void opcode26 (void) {  //unknown
#ifdef CPU_V20
  intcall86 (6);  // trip invalid opcode exception (this occurs on the 80186+, 8086/8088 CPUs treat them as NOPs. 
                  // technically they aren't exactly like NOPs in most cases, but for our pursoses, that's accurate enough. 
#endif
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode2E (void) {  ///unknown
#ifdef CPU_V20
  intcall86 (6);  // trip invalid opcode exception (this occurs on the 80186+, 8086/8088 CPUs treat them as NOPs. 
                  // technically they aren't exactly like NOPs in most cases, but for our pursoses, that's accurate enough. 
#endif
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode36 (void) {  ///unknown
#ifdef CPU_V20
  intcall86 (6);  // trip invalid opcode exception (this occurs on the 80186+, 8086/8088 CPUs treat them as NOPs. 
                  // technically they aren't exactly like NOPs in most cases, but for our pursoses, that's accurate enough. 
#endif
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode3E (void) {  ///unknown
#ifdef CPU_V20
  intcall86 (6);  // trip invalid opcode exception (this occurs on the 80186+, 8086/8088 CPUs treat them as NOPs. 
                  // technically they aren't exactly like NOPs in most cases, but for our pursoses, that's accurate enough. 
#endif
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode63 (void) {  ///unknown
#ifdef CPU_V20
  intcall86 (6);  // trip invalid opcode exception (this occurs on the 80186+, 8086/8088 CPUs treat them as NOPs. 
                  // technically they aren't exactly like NOPs in most cases, but for our pursoses, that's accurate enough. 
#endif
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode64 (void) {  ///unknown
#ifdef CPU_V20
  intcall86 (6);  // trip invalid opcode exception (this occurs on the 80186+, 8086/8088 CPUs treat them as NOPs. 
                  // technically they aren't exactly like NOPs in most cases, but for our pursoses, that's accurate enough. 
#endif
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode65 (void) {  ///unknown
#ifdef CPU_V20
  intcall86 (6);  // trip invalid opcode exception (this occurs on the 80186+, 8086/8088 CPUs treat them as NOPs. 
                  // technically they aren't exactly like NOPs in most cases, but for our pursoses, that's accurate enough. 
#endif
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode66 (void) {  ///unknown
#ifdef CPU_V20
  intcall86 (6);  // trip invalid opcode exception (this occurs on the 80186+, 8086/8088 CPUs treat them as NOPs. 
                  // technically they aren't exactly like NOPs in most cases, but for our pursoses, that's accurate enough. 
#endif
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcode67 (void) {  ///unknown
#ifdef CPU_V20
  intcall86 (6);  // trip invalid opcode exception (this occurs on the 80186+, 8086/8088 CPUs treat them as NOPs. 
                  // technically they aren't exactly like NOPs in most cases, but for our pursoses, that's accurate enough. 
#endif
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcodeF1 (void) {  ///unknown
#ifdef CPU_V20
  intcall86 (6);  // trip invalid opcode exception (this occurs on the 80186+, 8086/8088 CPUs treat them as NOPs. 
                  // technically they aren't exactly like NOPs in most cases, but for our pursoses, that's accurate enough. 
#endif
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcodeF2 (void) {  ///unknown
#ifdef CPU_V20
  intcall86 (6);  // trip invalid opcode exception (this occurs on the 80186+, 8086/8088 CPUs treat them as NOPs. 
                  // technically they aren't exactly like NOPs in most cases, but for our pursoses, that's accurate enough. 
#endif
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}
static inline void opcodeF3 (void) {  ///unknown
#ifdef CPU_V20
  intcall86 (6);  // trip invalid opcode exception (this occurs on the 80186+, 8086/8088 CPUs treat them as NOPs. 
                  // technically they aren't exactly like NOPs in most cases, but for our pursoses, that's accurate enough. 
#endif
#if DEBUG > 0   
    printf ("Illegal opcode: %02X @ %04X:%04X\n", opcode, savecs, saveip);
#endif    
}

typedef void (*Handler)(void);    // A pointer to a handler function 

#if FAKE86_USE_JUMPTABLE

static Handler jump_table[256] = {
  opcode00,opcode01,opcode02,opcode03,opcode04,opcode05,opcode06,opcode07,opcode08,opcode09,opcode0A,opcode0B,opcode0C,opcode0D,opcode0E,opcode0F,
  opcode10,opcode11,opcode12,opcode13,opcode14,opcode15,opcode16,opcode17,opcode18,opcode19,opcode1A,opcode1B,opcode1C,opcode1D,opcode1E,opcode1F,
  opcode20,opcode21,opcode22,opcode23,opcode24,opcode25,opcode26,opcode27,opcode28,opcode29,opcode2A,opcode2B,opcode2C,opcode2D,opcode2E,opcode2F,
  opcode30,opcode31,opcode32,opcode33,opcode34,opcode35,opcode36,opcode37,opcode38,opcode39,opcode3A,opcode3B,opcode3C,opcode3D,opcode3E,opcode3F,
  opcode40,opcode41,opcode42,opcode43,opcode44,opcode45,opcode46,opcode47,opcode48,opcode49,opcode4A,opcode4B,opcode4C,opcode4D,opcode4E,opcode4F,
  opcode50,opcode51,opcode52,opcode53,opcode54,opcode55,opcode56,opcode57,opcode58,opcode59,opcode5A,opcode5B,opcode5C,opcode5D,opcode5E,opcode5F,
  opcode60,opcode61,opcode62,opcode63,opcode64,opcode65,opcode66,opcode67,opcode68,opcode69,opcode6A,opcode6B,opcode6C,opcode6D,opcode6E,opcode6F,
  opcode70,opcode71,opcode72,opcode73,opcode74,opcode75,opcode76,opcode77,opcode78,opcode79,opcode7A,opcode7B,opcode7C,opcode7D,opcode7E,opcode7F,
  opcode80,opcode81,opcode82,opcode83,opcode84,opcode85,opcode86,opcode87,opcode88,opcode89,opcode8A,opcode8B,opcode8C,opcode8D,opcode8E,opcode8F,
  opcode90,opcode91,opcode92,opcode93,opcode94,opcode95,opcode96,opcode97,opcode98,opcode99,opcode9A,opcode9B,opcode9C,opcode9D,opcode9E,opcode9F,
  opcodeA0,opcodeA1,opcodeA2,opcodeA3,opcodeA4,opcodeA5,opcodeA6,opcodeA7,opcodeA8,opcodeA9,opcodeAA,opcodeAB,opcodeAC,opcodeAD,opcodeAE,opcodeAF,
  opcodeB0,opcodeB1,opcodeB2,opcodeB3,opcodeB4,opcodeB5,opcodeB6,opcodeB7,opcodeB8,opcodeB9,opcodeBA,opcodeBB,opcodeBC,opcodeBD,opcodeBE,opcodeBF,
  opcodeC0,opcodeC1,opcodeC2,opcodeC3,opcodeC4,opcodeC5,opcodeC6,opcodeC7,opcodeC8,opcodeC9,opcodeCA,opcodeCB,opcodeCC,opcodeCD,opcodeCE,opcodeCF,
  opcodeD0,opcodeD1,opcodeD2,opcodeD3,opcodeD4,opcodeD5,opcodeD6,opcodeD7,opcodeD8,opcodeD9,opcodeDA,opcodeDB,opcodeDC,opcodeDD,opcodeDE,opcodeDF,
  opcodeE0,opcodeE1,opcodeE2,opcodeE3,opcodeE4,opcodeE5,opcodeE6,opcodeE7,opcodeE8,opcodeE9,opcodeEA,opcodeEB,opcodeEC,opcodeED,opcodeEE,opcodeEF,
  opcodeF0,opcodeF1,opcodeF2,opcodeF3,opcodeF4,opcodeF5,opcodeF6,opcodeF7,opcodeF8,opcodeF9,opcodeFA,opcodeFB,opcodeFC,opcodeFD,opcodeFE,opcodeFF
};

void DOOPCODE(uint8_t opcode) {
  jump_table[opcode]();
}


#endif


//********************************************************************************
// CPU CORE
//********************************************************************************
void exec86 (uint32_t execloops) {
  counterticks = (uint64_t) ( (double) timerfreq / (double) 65536.0);

  for (loopcount = 0; loopcount < execloops; loopcount++) {
    if ( (totalexec & 31) == 0) timing();
    if (tf) {
      trap_toggle = 1;
    } else {
      trap_toggle = 0;
    }
    if (!trap_toggle && (ifl && (i8259.irr & (~i8259.imr) ) ) ) {
      intcall86 (nextintr() );  // get next interrupt from the i8259, if any 
    }
    reptype = 0;
    segoverride = 0;
    useseg = segregs[regds];
    docontinue = 0;
    firstip = ip;

    if ( (segregs[regcs] == 0xF000) && (ip == 0xE066) ) didbootstrap = 0; //detect if we hit the BIOS entry point to clear didbootstrap because we've rebooted
//--------
    if (!didbootstrap) {
      RAM[0x410] = 0x41; //ugly hack to make BIOS always believe we have an EGA/VGA card installed
      RAM[0x475] = hdcount; //the BIOS doesn't have any concept of hard drives, so here's another hack
    }
//--------  
    while (!docontinue) {
      segregs[regcs] = segregs[regcs] & 0xFFFF;
      ip = ip & 0xFFFF;
      savecs = segregs[regcs];
      saveip = ip;
      opcode = getmem8 (segregs[regcs], ip);
      StepIP (1);

      switch (opcode) {
          // segment prefix check 
        case 0x2E:  // segment segregs[regcs] 
          useseg = segregs[regcs];
          segoverride = 1;
          break;
        case 0x3E:  // segment segregs[regds] 
          useseg = segregs[regds];
          segoverride = 1;
          break;
        case 0x26:  // segment segregs[reges] 
          useseg = segregs[reges];
          segoverride = 1;
          break;
        case 0x36:  // segment segregs[regss] 
          useseg = segregs[regss];
          segoverride = 1;
          break;
          // repetition prefix check 
        case 0xF3:  // REP/REPE/REPZ 
          reptype = 1;
          break;
        case 0xF2:  // REPNE/REPNZ 
          reptype = 2;
          break;
        default:
          docontinue = 1;
          break;
      }
    }
    totalexec++;

//--------------------------------------------------------------------------------
#if FAKE86_USE_JUMPTABLE
    ///DOOPCODE(opcode);
    jump_table[opcode]();
//--------------------------------------------------------------------------------
#else
//--------------------------------------------------------------------------------
  switch (opcode) {
    case 0x0: // 00 ADD Eb Gb 
      modregrm();
      oper1b = readrm8( rm);
      oper2b = getreg8( reg);
      op_add8();
      writerm8( rm, res8);
      break;

    case 0x1: // 01 ADD Ev Gv 
      modregrm();
      oper1 = readrm16( rm);
      oper2 = getreg16( reg);
      op_add16();
      writerm16( rm, res16);
      break;

    case 0x2: // 02 ADD Gb Eb 
      modregrm();
      oper1b = getreg8( reg);
      oper2b = readrm8( rm);
      op_add8();
      putreg8( reg, res8);
      break;

    case 0x3: // 03 ADD Gv Ev 
      modregrm();
      oper1 = getreg16( reg);
      oper2 = readrm16( rm);
      op_add16();
      putreg16( reg, res16);
      break;

    case 0x4: // 04 ADD regs_.byteregs[regal] Ib 
      oper1b = regs_.byteregs[regal];
      oper2b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      op_add8();
      regs_.byteregs[regal] = res8;
      break;

    case 0x5: // 05 ADD eAX Iv 
      oper1 = regs_.wordregs[regax];
      oper2 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      op_add16();
      regs_.wordregs[regax] = res16;
      break;

    case 0x6: // 06 PUSH segregs[reges] 
      push( segregs[reges]);
      break;

    case 0x7: // 07 POP segregs[reges] 
      segregs[reges] = pop();
      break;

    case 0x8: // 08 OR Eb Gb 
      modregrm();
      oper1b = readrm8( rm);
      oper2b = getreg8( reg);
      op_or8();
      writerm8( rm, res8);
      break;

    case 0x9: // 09 OR Ev Gv 
      modregrm();
      oper1 = readrm16( rm);
      oper2 = getreg16( reg);
      op_or16();
      writerm16( rm, res16);
      break;

    case 0xA: // 0A OR Gb Eb 
      modregrm();
      oper1b = getreg8( reg);
      oper2b = readrm8( rm);
      op_or8();
      putreg8( reg, res8);
      break;

    case 0xB: // 0B OR Gv Ev 
      modregrm();
      oper1 = getreg16( reg);
      oper2 = readrm16( rm);
      op_or16();
      if ((oper1 == 0xF802) && (oper2 == 0xF802)) {
        sf = 0;  // cheap hack to make Wolf 3D think we're a 286 so it plays 
      }

      putreg16( reg, res16);
      break;

    case 0xC: // 0C OR regs_.byteregs[regal] Ib 
      oper1b = regs_.byteregs[regal];
      oper2b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      op_or8();
      regs_.byteregs[regal] = res8;
      break;

    case 0xD: // 0D OR eAX Iv 
      oper1 = regs_.wordregs[regax];
      oper2 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      op_or16();
      regs_.wordregs[regax] = res16;
      break;

    case 0xE: // 0E PUSH segregs[regcs] 
      push( segregs[regcs]);
      break;

#ifdef CPU_ALLOW_POP_CS //only the 8086/8088 does this.
    case 0xF: //0F POP CS
      segregs[regcs] = pop();
      break;
#endif

    case 0x10:  // 10 ADC Eb Gb 
      modregrm();
      oper1b = readrm8( rm);
      oper2b = getreg8( reg);
      op_adc8();
      writerm8( rm, res8);
      break;

    case 0x11:  // 11 ADC Ev Gv 
      modregrm();
      oper1 = readrm16( rm);
      oper2 = getreg16( reg);
      op_adc16();
      writerm16( rm, res16);
      break;

    case 0x12:  // 12 ADC Gb Eb 
      modregrm();
      oper1b = getreg8( reg);
      oper2b = readrm8( rm);
      op_adc8();
      putreg8( reg, res8);
      break;

    case 0x13:  // 13 ADC Gv Ev 
      modregrm();
      oper1 = getreg16( reg);
      oper2 = readrm16( rm);
      op_adc16();
      putreg16( reg, res16);
      break;

    case 0x14:  // 14 ADC regs_.byteregs[regal] Ib 
      oper1b = regs_.byteregs[regal];
      oper2b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      op_adc8();
      regs_.byteregs[regal] = res8;
      break;

    case 0x15:  // 15 ADC eAX Iv 
      oper1 = regs_.wordregs[regax];
      oper2 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      op_adc16();
      regs_.wordregs[regax] = res16;
      break;

    case 0x16:  // 16 PUSH segregs[regss] 
      push( segregs[regss]);
      break;

    case 0x17:  // 17 POP segregs[regss] 
      segregs[regss] = pop();
      break;

    case 0x18:  // 18 SBB Eb Gb 
      modregrm();
      oper1b = readrm8( rm);
      oper2b = getreg8( reg);
      op_sbb8();
      writerm8( rm, res8);
      break;

    case 0x19:  // 19 SBB Ev Gv 
      modregrm();
      oper1 = readrm16( rm);
      oper2 = getreg16( reg);
      op_sbb16();
      writerm16( rm, res16);
      break;

    case 0x1A:  // 1A SBB Gb Eb 
      modregrm();
      oper1b = getreg8( reg);
      oper2b = readrm8( rm);
      op_sbb8();
      putreg8( reg, res8);
      break;

    case 0x1B:  // 1B SBB Gv Ev 
      modregrm();
      oper1 = getreg16( reg);
      oper2 = readrm16( rm);
      op_sbb16();
      putreg16( reg, res16);
      break;

    case 0x1C:  // 1C SBB regs_.byteregs[regal] Ib 
      oper1b = regs_.byteregs[regal];
      oper2b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      op_sbb8();
      regs_.byteregs[regal] = res8;
      break;

    case 0x1D:  // 1D SBB eAX Iv 
      oper1 = regs_.wordregs[regax];
      oper2 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      op_sbb16();
      regs_.wordregs[regax] = res16;
      break;

    case 0x1E:  // 1E PUSH segregs[regds] 
      push( segregs[regds]);
      break;

    case 0x1F:  // 1F POP segregs[regds] 
      segregs[regds] = pop();
      break;

    case 0x20:  // 20 AND Eb Gb 
      modregrm();
      oper1b = readrm8( rm);
      oper2b = getreg8( reg);
      op_and8();
      writerm8( rm, res8);
      break;

    case 0x21:  // 21 AND Ev Gv 
      modregrm();
      oper1 = readrm16( rm);
      oper2 = getreg16( reg);
      op_and16();
      writerm16( rm, res16);
      break;

    case 0x22:  // 22 AND Gb Eb 
      modregrm();
      oper1b = getreg8( reg);
      oper2b = readrm8( rm);
      op_and8();
      putreg8( reg, res8);
      break;

    case 0x23:  // 23 AND Gv Ev 
      modregrm();
      oper1 = getreg16( reg);
      oper2 = readrm16( rm);
      op_and16();
      putreg16( reg, res16);
      break;

    case 0x24:  // 24 AND regs_.byteregs[regal] Ib 
      oper1b = regs_.byteregs[regal];
      oper2b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      op_and8();
      regs_.byteregs[regal] = res8;
      break;

    case 0x25:  // 25 AND eAX Iv 
      oper1 = regs_.wordregs[regax];
      oper2 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      op_and16();
      regs_.wordregs[regax] = res16;
      break;

    case 0x27:  // 27 DAA 
    {
      uint8_t old_al;
      old_al = regs_.byteregs[regal];
      if (((regs_.byteregs[regal] & 0x0F) > 9) || af) {
        oper1 = (uint16_t)regs_.byteregs[regal] + 0x06;
        regs_.byteregs[regal] = oper1 & 0xFF;
        if (oper1 & 0xFF00) cf = 1;
        if ((oper1 & 0x000F) < (old_al & 0x0F)) af = 1;
      }
      if (((regs_.byteregs[regal] & 0xF0) > 0x90) || cf) {
        oper1 = (uint16_t)regs_.byteregs[regal] + 0x60;
        regs_.byteregs[regal] = oper1 & 0xFF;
        if (oper1 & 0xFF00) cf = 1; else cf = 0;
      }
      flag_szp8( regs_.byteregs[regal]);
      break;
    }

    case 0x28:  // 28 SUB Eb Gb 
      modregrm();
      oper1b = readrm8( rm);
      oper2b = getreg8( reg);
      op_sub8();
      writerm8( rm, res8);
      break;

    case 0x29:  // 29 SUB Ev Gv 
      modregrm();
      oper1 = readrm16( rm);
      oper2 = getreg16( reg);
      op_sub16();
      writerm16( rm, res16);
      break;

    case 0x2A:  // 2A SUB Gb Eb 
      modregrm();
      oper1b = getreg8( reg);
      oper2b = readrm8( rm);
      op_sub8();
      putreg8( reg, res8);
      break;

    case 0x2B:  // 2B SUB Gv Ev 
      modregrm();
      oper1 = getreg16( reg);
      oper2 = readrm16( rm);
      op_sub16();
      putreg16( reg, res16);
      break;

    case 0x2C:  // 2C SUB regs_.byteregs[regal] Ib 
      oper1b = regs_.byteregs[regal];
      oper2b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      op_sub8();
      regs_.byteregs[regal] = res8;
      break;

    case 0x2D:  // 2D SUB eAX Iv 
      oper1 = regs_.wordregs[regax];
      oper2 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      op_sub16();
      regs_.wordregs[regax] = res16;
      break;

    case 0x2F:  // 2F DAS 
    {
      uint8_t old_al;
      old_al = regs_.byteregs[regal];
      if (((regs_.byteregs[regal] & 0x0F) > 9) || af) {
        oper1 = (uint16_t)regs_.byteregs[regal] - 0x06;
        regs_.byteregs[regal] = oper1 & 0xFF;
        if (oper1 & 0xFF00) cf = 1;
        if ((oper1 & 0x000F) >= (old_al & 0x0F)) af = 1;
      }
      if (((regs_.byteregs[regal] & 0xF0) > 0x90) || cf) {
        oper1 = (uint16_t)regs_.byteregs[regal] - 0x60;
        regs_.byteregs[regal] = oper1 & 0xFF;
        if (oper1 & 0xFF00) cf = 1; else cf = 0;
      }
      flag_szp8( regs_.byteregs[regal]);
      break;
    }

    case 0x30:  // 30 XOR Eb Gb 
      modregrm();
      oper1b = readrm8( rm);
      oper2b = getreg8( reg);
      op_xor8();
      writerm8( rm, res8);
      break;

    case 0x31:  // 31 XOR Ev Gv 
      modregrm();
      oper1 = readrm16( rm);
      oper2 = getreg16( reg);
      op_xor16();
      writerm16( rm, res16);
      break;

    case 0x32:  // 32 XOR Gb Eb 
      modregrm();
      oper1b = getreg8( reg);
      oper2b = readrm8( rm);
      op_xor8();
      putreg8( reg, res8);
      break;

    case 0x33:  // 33 XOR Gv Ev 
      modregrm();
      oper1 = getreg16( reg);
      oper2 = readrm16( rm);
      op_xor16();
      putreg16( reg, res16);
      break;

    case 0x34:  // 34 XOR regs_.byteregs[regal] Ib 
      oper1b = regs_.byteregs[regal];
      oper2b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      op_xor8();
      regs_.byteregs[regal] = res8;
      break;

    case 0x35:  // 35 XOR eAX Iv 
      oper1 = regs_.wordregs[regax];
      oper2 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      op_xor16();
      regs_.wordregs[regax] = res16;
      break;

    case 0x37:  // 37 AAA ASCII 
      if (((regs_.byteregs[regal] & 0xF) > 9) || (af == 1)) {
        regs_.wordregs[regax] = regs_.wordregs[regax] + 0x106;
        af = 1;
        cf = 1;
      }
      else {
        af = 0;
        cf = 0;
      }

      regs_.byteregs[regal] = regs_.byteregs[regal] & 0xF;
      break;

    case 0x38:  // 38 CMP Eb Gb 
      modregrm();
      oper1b = readrm8( rm);
      oper2b = getreg8( reg);
      flag_sub8( oper1b, oper2b);
      break;

    case 0x39:  // 39 CMP Ev Gv 
      modregrm();
      oper1 = readrm16( rm);
      oper2 = getreg16( reg);
      flag_sub16( oper1, oper2);
      break;

    case 0x3A:  // 3A CMP Gb Eb 
      modregrm();
      oper1b = getreg8( reg);
      oper2b = readrm8( rm);
      flag_sub8( oper1b, oper2b);
      break;

    case 0x3B:  // 3B CMP Gv Ev 
      modregrm();
      oper1 = getreg16( reg);
      oper2 = readrm16( rm);
      flag_sub16( oper1, oper2);
      break;

    case 0x3C:  // 3C CMP regs_.byteregs[regal] Ib 
      oper1b = regs_.byteregs[regal];
      oper2b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      flag_sub8( oper1b, oper2b);
      break;

    case 0x3D:  // 3D CMP eAX Iv 
      oper1 = regs_.wordregs[regax];
      oper2 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      flag_sub16( oper1, oper2);
      break;

    case 0x3F:  // 3F AAS ASCII 
      if (((regs_.byteregs[regal] & 0xF) > 9) || (af == 1)) {
        regs_.wordregs[regax] = regs_.wordregs[regax] - 6;
        regs_.byteregs[regah] = regs_.byteregs[regah] - 1;
        af = 1;
        cf = 1;
      }
      else {
        af = 0;
        cf = 0;
      }

      regs_.byteregs[regal] = regs_.byteregs[regal] & 0xF;
      break;

    case 0x40:  // 40 INC eAX 
      oldcf = cf;
      oper1 = regs_.wordregs[regax];
      oper2 = 1;
      op_add16();
      cf = oldcf;
      regs_.wordregs[regax] = res16;
      break;

    case 0x41:  // 41 INC eCX 
      oldcf = cf;
      oper1 = regs_.wordregs[regcx];
      oper2 = 1;
      op_add16();
      cf = oldcf;
      regs_.wordregs[regcx] = res16;
      break;

    case 0x42:  // 42 INC eDX 
      oldcf = cf;
      oper1 = regs_.wordregs[regdx];
      oper2 = 1;
      op_add16();
      cf = oldcf;
      regs_.wordregs[regdx] = res16;
      break;

    case 0x43:  // 43 INC eBX 
      oldcf = cf;
      oper1 = regs_.wordregs[regbx];
      oper2 = 1;
      op_add16();
      cf = oldcf;
      regs_.wordregs[regbx] = res16;
      break;

    case 0x44:  // 44 INC eSP 
      oldcf = cf;
      oper1 = regs_.wordregs[regsp];
      oper2 = 1;
      op_add16();
      cf = oldcf;
      regs_.wordregs[regsp] = res16;
      break;

    case 0x45:  // 45 INC eBP 
      oldcf = cf;
      oper1 = regs_.wordregs[regbp];
      oper2 = 1;
      op_add16();
      cf = oldcf;
      regs_.wordregs[regbp] = res16;
      break;

    case 0x46:  // 46 INC eSI 
      oldcf = cf;
      oper1 = regs_.wordregs[regsi];
      oper2 = 1;
      op_add16();
      cf = oldcf;
      regs_.wordregs[regsi] = res16;
      break;

    case 0x47:  // 47 INC eDI 
      oldcf = cf;
      oper1 = regs_.wordregs[regdi];
      oper2 = 1;
      op_add16();
      cf = oldcf;
      regs_.wordregs[regdi] = res16;
      break;

    case 0x48:  // 48 DEC eAX 
      oldcf = cf;
      oper1 = regs_.wordregs[regax];
      oper2 = 1;
      op_sub16();
      cf = oldcf;
      regs_.wordregs[regax] = res16;
      break;

    case 0x49:  // 49 DEC eCX 
      oldcf = cf;
      oper1 = regs_.wordregs[regcx];
      oper2 = 1;
      op_sub16();
      cf = oldcf;
      regs_.wordregs[regcx] = res16;
      break;

    case 0x4A:  // 4A DEC eDX 
      oldcf = cf;
      oper1 = regs_.wordregs[regdx];
      oper2 = 1;
      op_sub16();
      cf = oldcf;
      regs_.wordregs[regdx] = res16;
      break;

    case 0x4B:  // 4B DEC eBX 
      oldcf = cf;
      oper1 = regs_.wordregs[regbx];
      oper2 = 1;
      op_sub16();
      cf = oldcf;
      regs_.wordregs[regbx] = res16;
      break;

    case 0x4C:  // 4C DEC eSP 
      oldcf = cf;
      oper1 = regs_.wordregs[regsp];
      oper2 = 1;
      op_sub16();
      cf = oldcf;
      regs_.wordregs[regsp] = res16;
      break;

    case 0x4D:  // 4D DEC eBP 
      oldcf = cf;
      oper1 = regs_.wordregs[regbp];
      oper2 = 1;
      op_sub16();
      cf = oldcf;
      regs_.wordregs[regbp] = res16;
      break;

    case 0x4E:  // 4E DEC eSI 
      oldcf = cf;
      oper1 = regs_.wordregs[regsi];
      oper2 = 1;
      op_sub16();
      cf = oldcf;
      regs_.wordregs[regsi] = res16;
      break;

    case 0x4F:  // 4F DEC eDI 
      oldcf = cf;
      oper1 = regs_.wordregs[regdi];
      oper2 = 1;
      op_sub16();
      cf = oldcf;
      regs_.wordregs[regdi] = res16;
      break;

    case 0x50:  // 50 PUSH eAX 
      push( regs_.wordregs[regax]);
      break;

    case 0x51:  // 51 PUSH eCX 
      push( regs_.wordregs[regcx]);
      break;

    case 0x52:  // 52 PUSH eDX 
      push( regs_.wordregs[regdx]);
      break;

    case 0x53:  // 53 PUSH eBX 
      push( regs_.wordregs[regbx]);
      break;

    case 0x54:  // 54 PUSH eSP 
#ifdef USE_286_STYLE_PUSH_SP
      push( regs_.wordregs[regsp]);
#else
      push( regs_.wordregs[regsp] - 2);
#endif
      break;

    case 0x55:  // 55 PUSH eBP 
      push( regs_.wordregs[regbp]);
      break;

    case 0x56:  // 56 PUSH eSI 
      push( regs_.wordregs[regsi]);
      break;

    case 0x57:  // 57 PUSH eDI 
      push( regs_.wordregs[regdi]);
      break;

    case 0x58:  // 58 POP eAX 
      regs_.wordregs[regax] = pop();
      break;

    case 0x59:  // 59 POP eCX 
      regs_.wordregs[regcx] = pop();
      break;

    case 0x5A:  // 5A POP eDX 
      regs_.wordregs[regdx] = pop();
      break;

    case 0x5B:  // 5B POP eBX 
      regs_.wordregs[regbx] = pop();
      break;

    case 0x5C:  // 5C POP eSP 
      regs_.wordregs[regsp] = pop();
      break;

    case 0x5D:  // 5D POP eBP 
      regs_.wordregs[regbp] = pop();
      break;

    case 0x5E:  // 5E POP eSI 
      regs_.wordregs[regsi] = pop();
      break;

    case 0x5F:  // 5F POP eDI 
      regs_.wordregs[regdi] = pop();
      break;

#ifndef CPU_8086
    case 0x60:  // 60 PUSHA (80186+) 
      oldsp = regs_.wordregs[regsp];
      push( regs_.wordregs[regax]);
      push( regs_.wordregs[regcx]);
      push( regs_.wordregs[regdx]);
      push( regs_.wordregs[regbx]);
      push( oldsp);
      push( regs_.wordregs[regbp]);
      push( regs_.wordregs[regsi]);
      push( regs_.wordregs[regdi]);
      break;

    case 0x61:  // 61 POPA (80186+) 
      regs_.wordregs[regdi] = pop();
      regs_.wordregs[regsi] = pop();
      regs_.wordregs[regbp] = pop();
      regs_.wordregs[regsp] += 2;
      regs_.wordregs[regbx] = pop();
      regs_.wordregs[regdx] = pop();
      regs_.wordregs[regcx] = pop();
      regs_.wordregs[regax] = pop();
      break;

    case 0x62: // 62 BOUND Gv, Ev (80186+) 
      modregrm();
      getea( rm);
      if (signext32(getreg16( reg)) < signext32(getmem16( ea >> 4, ea & 15))) {
        intcall86( 5); //bounds check exception
      }
      else {
        ea += 2;
        if (signext32(getreg16( reg)) > signext32(getmem16( ea >> 4, ea & 15))) {
          intcall86( 5); //bounds check exception
        }
      }
      break;

    case 0x68:  // 68 PUSH Iv (80186+) 
      push( getmem16( segregs[regcs], ip));
      StepIP( 2);
      break;

    case 0x69:  // 69 IMUL Gv Ev Iv (80186+) 
      modregrm();
      temp1 = readrm16( rm);
      temp2 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      if ((temp1 & 0x8000L) == 0x8000L) {
        temp1 = temp1 | 0xFFFF0000L;
      }

      if ((temp2 & 0x8000L) == 0x8000L) {
        temp2 = temp2 | 0xFFFF0000L;
      }

      temp3 = temp1 * temp2;
      putreg16( reg, temp3 & 0xFFFFL);
      if (temp3 & 0xFFFF0000L) {
        cf = 1;
        of = 1;
      }
      else {
        cf = 0;
        of = 0;
      }
      break;

    case 0x6A:  // 6A PUSH Ib (80186+) 
      push( (uint16_t)getmem8( segregs[regcs], ip));
      StepIP( 1);
      break;

    case 0x6B:  // 6B IMUL Gv Eb Ib (80186+) 
      modregrm();
      temp1 = readrm16( rm);
      temp2 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if ((temp1 & 0x8000L) == 0x8000L) {
        temp1 = temp1 | 0xFFFF0000L;
      }

      if ((temp2 & 0x8000L) == 0x8000L) {
        temp2 = temp2 | 0xFFFF0000L;
      }

      temp3 = temp1 * temp2;
      putreg16( reg, temp3 & 0xFFFFL);
      if (temp3 & 0xFFFF0000L) {
        cf = 1;
        of = 1;
      }
      else {
        cf = 0;
        of = 0;
      }
      break;

    case 0x6C:  // 6E INSB 
      if (reptype && (regs_.wordregs[regcx] == 0)) {
        break;
      }

      putmem8( segregs[reges], regs_.wordregs[regdi], portin( regs_.wordregs[regdx]));
      if (df) {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] - 1;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] - 1;
      }
      else {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] + 1;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] + 1;
      }

      if (reptype) {
        regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      }

      loopcount++;
      if (!reptype) {
        break;
      }

      ip = firstip;
      break;

    case 0x6D:  // 6F INSW 
      if (reptype && (regs_.wordregs[regcx] == 0)) {
        break;
      }

      putmem16( segregs[reges], regs_.wordregs[regdi], portin16( regs_.wordregs[regdx]));
      if (df) {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] - 2;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] - 2;
      }
      else {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] + 2;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] + 2;
      }

      if (reptype) {
        regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      }

      loopcount++;
      if (!reptype) {
        break;
      }

      ip = firstip;
      break;

    case 0x6E:  // 6E OUTSB 
      if (reptype && (regs_.wordregs[regcx] == 0)) {
        break;
      }

      portout( regs_.wordregs[regdx], getmem8( useseg, regs_.wordregs[regsi]));
      if (df) {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] - 1;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] - 1;
      }
      else {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] + 1;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] + 1;
      }

      if (reptype) {
        regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      }

      loopcount++;
      if (!reptype) {
        break;
      }

      ip = firstip;
      break;

    case 0x6F:  // 6F OUTSW 
      if (reptype && (regs_.wordregs[regcx] == 0)) {
        break;
      }

      portout16( regs_.wordregs[regdx], getmem16( useseg, regs_.wordregs[regsi]));
      if (df) {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] - 2;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] - 2;
      }
      else {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] + 2;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] + 2;
      }

      if (reptype) {
        regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      }

      loopcount++;
      if (!reptype) {
        break;
      }

      ip = firstip;
      break;
#endif

    case 0x70:  // 70 JO Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (of) {
        ip = ip + temp16;
      }
      break;

    case 0x71:  // 71 JNO Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (!of) {
        ip = ip + temp16;
      }
      break;

    case 0x72:  // 72 JB Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (cf) {
        ip = ip + temp16;
      }
      break;

    case 0x73:  // 73 JNB Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (!cf) {
        ip = ip + temp16;
      }
      break;

    case 0x74:  // 74 JZ Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (zf) {
        ip = ip + temp16;
      }
      break;

    case 0x75:  // 75 JNZ Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (!zf) {
        ip = ip + temp16;
      }
      break;

    case 0x76:  // 76 JBE Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (cf || zf) {
        ip = ip + temp16;
      }
      break;

    case 0x77:  // 77 JA Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (!cf && !zf) {
        ip = ip + temp16;
      }
      break;

    case 0x78:  // 78 JS Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (sf) {
        ip = ip + temp16;
      }
      break;

    case 0x79:  // 79 JNS Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (!sf) {
        ip = ip + temp16;
      }
      break;

    case 0x7A:  // 7A JPE Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (pf) {
        ip = ip + temp16;
      }
      break;

    case 0x7B:  // 7B JPO Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (!pf) {
        ip = ip + temp16;
      }
      break;

    case 0x7C:  // 7C JL Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (sf != of) {
        ip = ip + temp16;
      }
      break;

    case 0x7D:  // 7D JGE Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (sf == of) {
        ip = ip + temp16;
      }
      break;

    case 0x7E:  // 7E JLE Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if ((sf != of) || zf) {
        ip = ip + temp16;
      }
      break;

    case 0x7F:  // 7F JG Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (!zf && (sf == of)) {
        ip = ip + temp16;
      }
      break;

    case 0x80:
    case 0x82:  // 80/82 GRP1 Eb Ib 
      modregrm();
      oper1b = readrm8( rm);
      oper2b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      switch (reg) {
      case 0:
        op_add8();
        break;
      case 1:
        op_or8();
        break;
      case 2:
        op_adc8();
        break;
      case 3:
        op_sbb8();
        break;
      case 4:
        op_and8();
        break;
      case 5:
        op_sub8();
        break;
      case 6:
        op_xor8();
        break;
      case 7:
        flag_sub8( oper1b, oper2b);
        break;
      default:
        break;  // to avoid compiler warnings 
      }

      if (reg < 7) {
        writerm8( rm, res8);
      }
      break;

    case 0x81:  // 81 GRP1 Ev Iv 
    case 0x83:  // 83 GRP1 Ev Ib 
      modregrm();
      oper1 = readrm16( rm);
      if (opcode == 0x81) {
        oper2 = getmem16( segregs[regcs], ip);
        StepIP( 2);
      }
      else {
        oper2 = signext(getmem8( segregs[regcs], ip));
        StepIP( 1);
      }

      switch (reg) {
      case 0:
        op_add16();
        break;
      case 1:
        op_or16();
        break;
      case 2:
        op_adc16();
        break;
      case 3:
        op_sbb16();
        break;
      case 4:
        op_and16();
        break;
      case 5:
        op_sub16();
        break;
      case 6:
        op_xor16();
        break;
      case 7:
        flag_sub16( oper1, oper2);
        break;
      default:
        break;  // to avoid compiler warnings 
      }

      if (reg < 7) {
        writerm16( rm, res16);
      }
      break;

    case 0x84:  // 84 TEST Gb Eb 
      modregrm();
      oper1b = getreg8( reg);
      oper2b = readrm8( rm);
      flag_log8( oper1b & oper2b);
      break;

    case 0x85:  // 85 TEST Gv Ev 
      modregrm();
      oper1 = getreg16( reg);
      oper2 = readrm16( rm);
      flag_log16( oper1 & oper2);
      break;

    case 0x86:  // 86 XCHG Gb Eb 
      modregrm();
      oper1b = getreg8( reg);
      putreg8( reg, readrm8( rm));
      writerm8( rm, oper1b);
      break;

    case 0x87:  // 87 XCHG Gv Ev 
      modregrm();
      oper1 = getreg16( reg);
      putreg16( reg, readrm16( rm));
      writerm16( rm, oper1);
      break;

    case 0x88:  // 88 MOV Eb Gb 
      modregrm();
      writerm8( rm, getreg8( reg));
      break;

    case 0x89:  // 89 MOV Ev Gv 
      modregrm();
      writerm16( rm, getreg16( reg));
      break;

    case 0x8A:  // 8A MOV Gb Eb 
      modregrm();
      putreg8( reg, readrm8( rm));
      break;

    case 0x8B:  // 8B MOV Gv Ev 
      modregrm();
      putreg16( reg, readrm16( rm));
      break;

    case 0x8C:  // 8C MOV Ew Sw 
      modregrm();
      writerm16( rm, getsegreg( reg));
      break;

    case 0x8D:  // 8D LEA Gv M 
      modregrm();
      getea( rm);
      putreg16( reg, ea - segbase(useseg));
      break;

    case 0x8E:  // 8E MOV Sw Ew 
      modregrm();
      putsegreg( reg, readrm16( rm));
      break;

    case 0x8F:  // 8F POP Ev 
      modregrm();
      writerm16( rm, pop());
      break;

    case 0x90:  // 90 NOP 
      break;

    case 0x91:  // 91 XCHG eCX eAX 
      oper1 = regs_.wordregs[regcx];
      regs_.wordregs[regcx] = regs_.wordregs[regax];
      regs_.wordregs[regax] = oper1;
      break;

    case 0x92:  // 92 XCHG eDX eAX 
      oper1 = regs_.wordregs[regdx];
      regs_.wordregs[regdx] = regs_.wordregs[regax];
      regs_.wordregs[regax] = oper1;
      break;

    case 0x93:  // 93 XCHG eBX eAX 
      oper1 = regs_.wordregs[regbx];
      regs_.wordregs[regbx] = regs_.wordregs[regax];
      regs_.wordregs[regax] = oper1;
      break;

    case 0x94:  // 94 XCHG eSP eAX 
      oper1 = regs_.wordregs[regsp];
      regs_.wordregs[regsp] = regs_.wordregs[regax];
      regs_.wordregs[regax] = oper1;
      break;

    case 0x95:  // 95 XCHG eBP eAX 
      oper1 = regs_.wordregs[regbp];
      regs_.wordregs[regbp] = regs_.wordregs[regax];
      regs_.wordregs[regax] = oper1;
      break;

    case 0x96:  // 96 XCHG eSI eAX 
      oper1 = regs_.wordregs[regsi];
      regs_.wordregs[regsi] = regs_.wordregs[regax];
      regs_.wordregs[regax] = oper1;
      break;

    case 0x97:  // 97 XCHG eDI eAX 
      oper1 = regs_.wordregs[regdi];
      regs_.wordregs[regdi] = regs_.wordregs[regax];
      regs_.wordregs[regax] = oper1;
      break;

    case 0x98:  // 98 CBW 
      if ((regs_.byteregs[regal] & 0x80) == 0x80) {
        regs_.byteregs[regah] = 0xFF;
      }
      else {
        regs_.byteregs[regah] = 0;
      }
      break;

    case 0x99:  // 99 CWD 
      if ((regs_.byteregs[regah] & 0x80) == 0x80) {
        regs_.wordregs[regdx] = 0xFFFF;
      }
      else {
        regs_.wordregs[regdx] = 0;
      }
      break;

    case 0x9A:  // 9A CALL Ap 
      oper1 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      oper2 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      push( segregs[regcs]);
      push( ip);
      ip = oper1;
      segregs[regcs] = oper2;
      break;

    case 0x9B:  // 9B WAIT 
      break;

    case 0x9C:  // 9C PUSHF 
#ifdef CPU_SET_HIGH_FLAGS
      push( makeflagsword() | 0xF800);
#else
      push( makeflagsword() | 0x0800);
#endif
      break;

    case 0x9D:  // 9D POPF 
      temp16 = pop();
      decodeflagsword( temp16);
      break;

    case 0x9E:  // 9E SAHF 
      decodeflagsword( (makeflagsword() & 0xFF00) | regs_.byteregs[regah]);
      break;

    case 0x9F:  // 9F LAHF 
      regs_.byteregs[regah] = makeflagsword() & 0xFF;
      break;

    case 0xA0:  // A0 MOV regs_.byteregs[regal] Ob 
      regs_.byteregs[regal] = getmem8( useseg, getmem16( segregs[regcs], ip));
      StepIP( 2);
      break;

    case 0xA1:  // A1 MOV eAX Ov 
      oper1 = getmem16( useseg, getmem16( segregs[regcs], ip));
      StepIP( 2);
      regs_.wordregs[regax] = oper1;
      break;

    case 0xA2:  // A2 MOV Ob regs_.byteregs[regal] 
      putmem8( useseg, getmem16( segregs[regcs], ip), regs_.byteregs[regal]);
      StepIP( 2);
      break;

    case 0xA3:  // A3 MOV Ov eAX 
      putmem16( useseg, getmem16( segregs[regcs], ip), regs_.wordregs[regax]);
      StepIP( 2);
      break;

    case 0xA4:  // A4 MOVSB 
      if (reptype && (regs_.wordregs[regcx] == 0)) {
        break;
      }

      putmem8( segregs[reges], regs_.wordregs[regdi], getmem8( useseg, regs_.wordregs[regsi]));
      if (df) {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] - 1;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] - 1;
      }
      else {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] + 1;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] + 1;
      }

      if (reptype) {
        regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      }

      loopcount++;
      if (!reptype) {
        break;
      }

      ip = firstip;
      break;

    case 0xA5:  // A5 MOVSW 
      if (reptype && (regs_.wordregs[regcx] == 0)) {
        break;
      }

      putmem16( segregs[reges], regs_.wordregs[regdi], getmem16( useseg, regs_.wordregs[regsi]));
      if (df) {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] - 2;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] - 2;
      }
      else {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] + 2;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] + 2;
      }

      if (reptype) {
        regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      }

      loopcount++;
      if (!reptype) {
        break;
      }

      ip = firstip;
      break;

    case 0xA6:  // A6 CMPSB 
      if (reptype && (regs_.wordregs[regcx] == 0)) {
        break;
      }

      oper1b = getmem8( useseg, regs_.wordregs[regsi]);
      oper2b = getmem8( segregs[reges], regs_.wordregs[regdi]);
      if (df) {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] - 1;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] - 1;
      }
      else {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] + 1;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] + 1;
      }

      flag_sub8( oper1b, oper2b);
      if (reptype) {
        regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      }

      if ((reptype == 1) && !zf) {
        break;
      }
      else if ((reptype == 2) && (zf == 1)) {
        break;
      }

      loopcount++;
      if (!reptype) {
        break;
      }

      ip = firstip;
      break;

    case 0xA7:  // A7 CMPSW 
      if (reptype && (regs_.wordregs[regcx] == 0)) {
        break;
      }

      oper1 = getmem16( useseg, regs_.wordregs[regsi]);
      oper2 = getmem16( segregs[reges], regs_.wordregs[regdi]);
      if (df) {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] - 2;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] - 2;
      }
      else {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] + 2;
        regs_.wordregs[regdi] = regs_.wordregs[regdi] + 2;
      }

      flag_sub16( oper1, oper2);
      if (reptype) {
        regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      }

      if ((reptype == 1) && !zf) {
        break;
      }

      if ((reptype == 2) && (zf == 1)) {
        break;
      }

      loopcount++;
      if (!reptype) {
        break;
      }

      ip = firstip;
      break;

    case 0xA8:  // A8 TEST regs_.byteregs[regal] Ib 
      oper1b = regs_.byteregs[regal];
      oper2b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      flag_log8( oper1b & oper2b);
      break;

    case 0xA9:  // A9 TEST eAX Iv 
      oper1 = regs_.wordregs[regax];
      oper2 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      flag_log16( oper1 & oper2);
      break;

    case 0xAA:  // AA STOSB 
      if (reptype && (regs_.wordregs[regcx] == 0)) {
        break;
      }

      putmem8( segregs[reges], regs_.wordregs[regdi], regs_.byteregs[regal]);
      if (df) {
        regs_.wordregs[regdi] = regs_.wordregs[regdi] - 1;
      }
      else {
        regs_.wordregs[regdi] = regs_.wordregs[regdi] + 1;
      }

      if (reptype) {
        regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      }

      loopcount++;
      if (!reptype) {
        break;
      }

      ip = firstip;
      break;

    case 0xAB:  // AB STOSW 
      if (reptype && (regs_.wordregs[regcx] == 0)) {
        break;
      }

      putmem16( segregs[reges], regs_.wordregs[regdi], regs_.wordregs[regax]);
      if (df) {
        regs_.wordregs[regdi] = regs_.wordregs[regdi] - 2;
      }
      else {
        regs_.wordregs[regdi] = regs_.wordregs[regdi] + 2;
      }

      if (reptype) {
        regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      }

      loopcount++;
      if (!reptype) {
        break;
      }

      ip = firstip;
      break;

    case 0xAC:  // AC LODSB 
      if (reptype && (regs_.wordregs[regcx] == 0)) {
        break;
      }

      regs_.byteregs[regal] = getmem8( useseg, regs_.wordregs[regsi]);
      if (df) {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] - 1;
      }
      else {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] + 1;
      }

      if (reptype) {
        regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      }

      loopcount++;
      if (!reptype) {
        break;
      }

      ip = firstip;
      break;

    case 0xAD:  // AD LODSW 
      if (reptype && (regs_.wordregs[regcx] == 0)) {
        break;
      }

      oper1 = getmem16( useseg, regs_.wordregs[regsi]);
      regs_.wordregs[regax] = oper1;
      if (df) {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] - 2;
      }
      else {
        regs_.wordregs[regsi] = regs_.wordregs[regsi] + 2;
      }

      if (reptype) {
        regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      }

      loopcount++;
      if (!reptype) {
        break;
      }

      ip = firstip;
      break;

    case 0xAE:  // AE SCASB 
      if (reptype && (regs_.wordregs[regcx] == 0)) {
        break;
      }

      oper1b = regs_.byteregs[regal];
      oper2b = getmem8( segregs[reges], regs_.wordregs[regdi]);
      flag_sub8( oper1b, oper2b);
      if (df) {
        regs_.wordregs[regdi] = regs_.wordregs[regdi] - 1;
      }
      else {
        regs_.wordregs[regdi] = regs_.wordregs[regdi] + 1;
      }

      if (reptype) {
        regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      }

      if ((reptype == 1) && !zf) {
        break;
      }
      else if ((reptype == 2) && (zf == 1)) {
        break;
      }

      loopcount++;
      if (!reptype) {
        break;
      }

      ip = firstip;
      break;

    case 0xAF:  // AF SCASW 
      if (reptype && (regs_.wordregs[regcx] == 0)) {
        break;
      }

      oper1 = regs_.wordregs[regax];
      oper2 = getmem16( segregs[reges], regs_.wordregs[regdi]);
      flag_sub16( oper1, oper2);
      if (df) {
        regs_.wordregs[regdi] = regs_.wordregs[regdi] - 2;
      }
      else {
        regs_.wordregs[regdi] = regs_.wordregs[regdi] + 2;
      }

      if (reptype) {
        regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      }

      if ((reptype == 1) && !zf) {
        break;
      }
      else if ((reptype == 2) && (zf == 1)) { //did i fix a typo bug? this used to be & instead of &&
        break;
      }

      loopcount++;
      if (!reptype) {
        break;
      }

      ip = firstip;
      break;

    case 0xB0:  // B0 MOV regs_.byteregs[regal] Ib 
      regs_.byteregs[regal] = getmem8( segregs[regcs], ip);
      StepIP( 1);
      break;

    case 0xB1:  // B1 MOV regs_.byteregs[regcl] Ib 
      regs_.byteregs[regcl] = getmem8( segregs[regcs], ip);
      StepIP( 1);
      break;

    case 0xB2:  // B2 MOV regs_.byteregs[regdl] Ib 
      regs_.byteregs[regdl] = getmem8( segregs[regcs], ip);
      StepIP( 1);
      break;

    case 0xB3:  // B3 MOV regs_.byteregs[regbl] Ib 
      regs_.byteregs[regbl] = getmem8( segregs[regcs], ip);
      StepIP( 1);
      break;

    case 0xB4:  // B4 MOV regs_.byteregs[regah] Ib 
      regs_.byteregs[regah] = getmem8( segregs[regcs], ip);
      StepIP( 1);
      break;

    case 0xB5:  // B5 MOV regs_.byteregs[regch] Ib 
      regs_.byteregs[regch] = getmem8( segregs[regcs], ip);
      StepIP( 1);
      break;

    case 0xB6:  // B6 MOV regs_.byteregs[regdh] Ib 
      regs_.byteregs[regdh] = getmem8( segregs[regcs], ip);
      StepIP( 1);
      break;

    case 0xB7:  // B7 MOV regs_.byteregs[regbh] Ib 
      regs_.byteregs[regbh] = getmem8( segregs[regcs], ip);
      StepIP( 1);
      break;

    case 0xB8:  // B8 MOV eAX Iv 
      oper1 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      regs_.wordregs[regax] = oper1;
      break;

    case 0xB9:  // B9 MOV eCX Iv 
      oper1 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      regs_.wordregs[regcx] = oper1;
      break;

    case 0xBA:  // BA MOV eDX Iv 
      oper1 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      regs_.wordregs[regdx] = oper1;
      break;

    case 0xBB:  // BB MOV eBX Iv 
      oper1 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      regs_.wordregs[regbx] = oper1;
      break;

    case 0xBC:  // BC MOV eSP Iv 
      regs_.wordregs[regsp] = getmem16( segregs[regcs], ip);
      StepIP( 2);
      break;

    case 0xBD:  // BD MOV eBP Iv 
      regs_.wordregs[regbp] = getmem16( segregs[regcs], ip);
      StepIP( 2);
      break;

    case 0xBE:  // BE MOV eSI Iv 
      regs_.wordregs[regsi] = getmem16( segregs[regcs], ip);
      StepIP( 2);
      break;

    case 0xBF:  // BF MOV eDI Iv 
      regs_.wordregs[regdi] = getmem16( segregs[regcs], ip);
      StepIP( 2);
      break;

    case 0xC0:  // C0 GRP2 byte imm8 (80186+) 
      modregrm();
      oper1b = readrm8( rm);
      oper2b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      writerm8( rm, op_grp2_8( oper2b));
      break;

    case 0xC1:  // C1 GRP2 word imm8 (80186+) 
      modregrm();
      oper1 = readrm16( rm);
      oper2 = getmem8( segregs[regcs], ip);
      StepIP( 1);
      writerm16( rm, op_grp2_16( (uint8_t)oper2));
      break;

    case 0xC2:  // C2 RET Iw 
      oper1 = getmem16( segregs[regcs], ip);
      ip = pop();
      regs_.wordregs[regsp] = regs_.wordregs[regsp] + oper1;
      break;

    case 0xC3:  // C3 RET 
      ip = pop();
      break;

    case 0xC4:  // C4 LES Gv Mp 
      modregrm();
      getea( rm);
      putreg16( reg, read86( ea) + read86( ea + 1) * 256);
      segregs[reges] = read86( ea + 2) + read86( ea + 3) * 256;
      break;

    case 0xC5:  // C5 LDS Gv Mp 
      modregrm();
      getea( rm);
      putreg16( reg, read86( ea) + read86( ea + 1) * 256);
      segregs[regds] = read86( ea + 2) + read86( ea + 3) * 256;
      break;

    case 0xC6:  // C6 MOV Eb Ib 
      modregrm();
      writerm8( rm, getmem8( segregs[regcs], ip));
      StepIP( 1);
      break;

    case 0xC7:  // C7 MOV Ev Iv 
      modregrm();
      writerm16( rm, getmem16( segregs[regcs], ip));
      StepIP( 2);
      break;

    case 0xC8:  // C8 ENTER (80186+) 
      stacksize = getmem16( segregs[regcs], ip);
      StepIP( 2);
      nestlev = getmem8( segregs[regcs], ip);
      StepIP( 1);
      push( regs_.wordregs[regbp]);
      frametemp = regs_.wordregs[regsp];
      if (nestlev) {
        for (temp16 = 1; temp16 < nestlev; ++temp16) {
          regs_.wordregs[regbp] = regs_.wordregs[regbp] - 2;
          push( regs_.wordregs[regbp]);
        }

        push( frametemp); //regs_.wordregs[regsp]);
      }

      regs_.wordregs[regbp] = frametemp;
      regs_.wordregs[regsp] = regs_.wordregs[regbp] - stacksize;

      break;

    case 0xC9:  // C9 LEAVE (80186+) 
      regs_.wordregs[regsp] = regs_.wordregs[regbp];
      regs_.wordregs[regbp] = pop();
      break;

    case 0xCA:  // CA RETF Iw 
      oper1 = getmem16( segregs[regcs], ip);
      ip = pop();
      segregs[regcs] = pop();
      regs_.wordregs[regsp] = regs_.wordregs[regsp] + oper1;
      break;

    case 0xCB:  // CB RETF 
      ip = pop();
      segregs[regcs] = pop();
      break;

    case 0xCC:  // CC INT 3 
      intcall86( 3);
      break;

    case 0xCD:  // CD INT Ib 
      oper1b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      intcall86( oper1b);
      break;

    case 0xCE:  // CE INTO 
      if (of) {
        intcall86( 4);
      }
      break;

    case 0xCF:  // CF IRET 
      ip = pop();
      segregs[regcs] = pop();
      decodeflagsword( pop());

      
       // if (net.enabled) net.canrecv = 1;
       
      break;

    case 0xD0:  // D0 GRP2 Eb 1 
      modregrm();
      oper1b = readrm8( rm);
      writerm8( rm, op_grp2_8( 1));
      break;

    case 0xD1:  // D1 GRP2 Ev 1 
      modregrm();
      oper1 = readrm16( rm);
      writerm16( rm, op_grp2_16( 1));
      break;

    case 0xD2:  // D2 GRP2 Eb regs_.byteregs[regcl] 
      modregrm();
      oper1b = readrm8( rm);
      writerm8( rm, op_grp2_8( regs_.byteregs[regcl]));
      break;

    case 0xD3:  // D3 GRP2 Ev regs_.byteregs[regcl] 
      modregrm();
      oper1 = readrm16( rm);
      writerm16( rm, op_grp2_16( regs_.byteregs[regcl]));
      break;

    case 0xD4:  // D4 AAM I0 
      oper1 = getmem8( segregs[regcs], ip);
      StepIP( 1);
      if (!oper1) {
        intcall86( 0);
        break;
      } // division by zero 

      regs_.byteregs[regah] = (regs_.byteregs[regal] / oper1) & 255;
      regs_.byteregs[regal] = (regs_.byteregs[regal] % oper1) & 255;
      flag_szp16( regs_.wordregs[regax]);
      break;

    case 0xD5:  // D5 AAD I0 
      oper1 = getmem8( segregs[regcs], ip);
      StepIP( 1);
      regs_.byteregs[regal] = (regs_.byteregs[regah] * oper1 + regs_.byteregs[regal]) & 255;
      regs_.byteregs[regah] = 0;
      flag_szp16( regs_.byteregs[regah] * oper1 + regs_.byteregs[regal]);
      sf = 0;
      break;

    case 0xD6:  // D6 XLAT on V20/V30, SALC on 8086/8088 
#ifndef CPU_NO_SALC
      regs_.byteregs[regal] = cf ? 0xFF : 0x00;
      break;
#endif

    case 0xD7:  // D7 XLAT 
      regs_.byteregs[regal] = read86( useseg * 16 + (regs_.wordregs[regbx]) + regs_.byteregs[regal]);
      break;

    case 0xD8:
    case 0xD9:
    case 0xDA:
    case 0xDB:
    case 0xDC:
    case 0xDE:
    case 0xDD:
    case 0xDF:  // escape to x87 FPU (unsupported) 
      modregrm();
      break;

    case 0xE0:  // E0 LOOPNZ Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      if ((regs_.wordregs[regcx]) && !zf) {
        ip = ip + temp16;
      }
      break;

    case 0xE1:  // E1 LOOPZ Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      if (regs_.wordregs[regcx] && (zf == 1)) {
        ip = ip + temp16;
      }
      break;

    case 0xE2:  // E2 LOOP Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      regs_.wordregs[regcx] = regs_.wordregs[regcx] - 1;
      if (regs_.wordregs[regcx]) {
        ip = ip + temp16;
      }
      break;

    case 0xE3:  // E3 JCXZ Jb 
      temp16 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      if (!regs_.wordregs[regcx]) {
        ip = ip + temp16;
      }
      break;

    case 0xE4:  // E4 IN regs_.byteregs[regal] Ib 
      oper1b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      regs_.byteregs[regal] = (uint8_t)portin( oper1b);
      break;

    case 0xE5:  // E5 IN eAX Ib 
      oper1b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      regs_.wordregs[regax] = portin16( oper1b);
      break;

    case 0xE6:  // E6 OUT Ib regs_.byteregs[regal] 
      oper1b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      portout( oper1b, regs_.byteregs[regal]);
      break;

    case 0xE7:  // E7 OUT Ib eAX 
      oper1b = getmem8( segregs[regcs], ip);
      StepIP( 1);
      portout16( oper1b, regs_.wordregs[regax]);
      break;

    case 0xE8:  // E8 CALL Jv 
      oper1 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      push( ip);
      ip = ip + oper1;
      break;

    case 0xE9:  // E9 JMP Jv 
      oper1 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      ip = ip + oper1;
      break;

    case 0xEA:  // EA JMP Ap 
      oper1 = getmem16( segregs[regcs], ip);
      StepIP( 2);
      oper2 = getmem16( segregs[regcs], ip);
      ip = oper1;
      segregs[regcs] = oper2;
      break;

    case 0xEB:  // EB JMP Jb 
      oper1 = signext(getmem8( segregs[regcs], ip));
      StepIP( 1);
      ip = ip + oper1;
      break;

    case 0xEC:  // EC IN regs_.byteregs[regal] regdx 
      oper1 = regs_.wordregs[regdx];
      regs_.byteregs[regal] = (uint8_t)portin( oper1);
      break;

    case 0xED:  // ED IN eAX regdx 
      oper1 = regs_.wordregs[regdx];
      regs_.wordregs[regax] = portin16( oper1);
      break;

    case 0xEE:  // EE OUT regdx regs_.byteregs[regal] 
      oper1 = regs_.wordregs[regdx];
      portout( oper1, regs_.byteregs[regal]);
      break;

    case 0xEF:  // EF OUT regdx eAX 
      oper1 = regs_.wordregs[regdx];
      portout16( oper1, regs_.wordregs[regax]);
      break;

    case 0xF0:  // F0 LOCK 
      break;

    case 0xF4:  // F4 HLT 
      hltstate = 1;
      break;

    case 0xF5:  // F5 CMC 
      if (!cf) {
        cf = 1;
      }
      else {
        cf = 0;
      }
      break;

    case 0xF6:  // F6 GRP3a Eb 
      modregrm();
      oper1b = readrm8( rm);
      op_grp3_8();
      if ((reg > 1) && (reg < 4)) {
        writerm8( rm, res8);
      }
      break;

    case 0xF7:  // F7 GRP3b Ev 
      modregrm();
      oper1 = readrm16( rm);
      op_grp3_16();
      if ((reg > 1) && (reg < 4)) {
        writerm16( rm, res16);
      }
      break;

    case 0xF8:  // F8 CLC 
      cf = 0;
      break;

    case 0xF9:  // F9 STC 
      cf = 1;
      break;

    case 0xFA:  // FA CLI 
      ifl = 0;
      break;

    case 0xFB:  // FB STI 
      ifl = 1;
      break;

    case 0xFC:  // FC CLD 
      df = 0;
      break;

    case 0xFD:  // FD STD 
      df = 1;
      break;

    case 0xFE:  // FE GRP4 Eb 
      modregrm();
      oper1b = readrm8( rm);
      oper2b = 1;
      if (!reg) {
        tempcf = cf;
        res8 = oper1b + oper2b;
        flag_add8( oper1b, oper2b);
        cf = tempcf;
        writerm8( rm, res8);
      }
      else {
        tempcf = cf;
        res8 = oper1b - oper2b;
        flag_sub8( oper1b, oper2b);
        cf = tempcf;
        writerm8( rm, res8);
      }
      break;

    case 0xFF:  // FF GRP5 Ev 
      modregrm();
      oper1 = readrm16( rm);
      op_grp5();
      break;

    default:
#ifdef CPU_ALLOW_ILLEGAL_OP_EXCEPTION
      intcall86( 6); // trip invalid opcode exception. this occurs on the 80186+, 8086/8088 CPUs treat them as NOPs. 
               // technically they aren't exactly like NOPs in most cases, but for our pursoses, that's accurate enough. 
      debug_log(DEBUG_INFO, "[CPU] Invalid opcode exception at %04X:%04X\r\n", segregs[regcs], firstip);
#endif
      break;
    }

//--------------------------------------------------------------------------------    
#endif

















      

      if (POWER != APP_EMU_FAKE86) {
          return;
        }
    }
}
 
//████████████████████████████████████████
 
//████████████████████████████████████████
//████████████████████████████████████████
//i8253.c
// i8253.c: functions to emulate the Intel 8253 programmable interval timer.
//   these are required for the timer interrupt and PC speaker to be
//   properly emulated! 

extern void set_portout_redirector (uint16_t startport, uint16_t endport, void *callback);
extern void set_portin_redirector (uint16_t startport, uint16_t endport, void *callback);

struct i8253_s i8253;


extern uint64_t hostfreq, lasttick, curtick, tickgap, totalexec;

void out8253 (uint16_t portnum, uint8_t value) 
{
  uint8_t curbyte;
  portnum &= 3;
  switch (portnum) 
        {
      case 0:
      case 1:
      case 2: //channel data
        if ( (i8253.accessmode[portnum] == PIT_MODE_LOBYTE) || ( (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) && (i8253.bytetoggle[portnum] == 0) ) ) curbyte = 0;
        else if ( (i8253.accessmode[portnum] == PIT_MODE_HIBYTE) || ( (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) && (i8253.bytetoggle[portnum] == 1) ) ) curbyte = 1;
        if (curbyte == 0) { //low byte
            i8253.chandata[portnum] = (i8253.chandata[portnum] & 0xFF00) | value;
          }
        else {   //high byte
            i8253.chandata[portnum] = (i8253.chandata[portnum] & 0x00FF) | ( (uint16_t) value << 8);
          }
        
        ///i8253.chandata[portnum]=0; ///NCX???
        
        if (i8253.chandata[portnum] == 0) i8253.effectivedata[portnum] = 65536;
        else i8253.effectivedata[portnum] = i8253.chandata[portnum];
        i8253.active[portnum] = 1;
        tickgap = (uint64_t) ( (float) hostfreq / (float) ( (float) 1193182 / (float) i8253.effectivedata[0]) );       
        if (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) i8253.bytetoggle[portnum] = (~i8253.bytetoggle[portnum]) & 1;
        i8253.chanfreq[portnum] = (float) ( (uint32_t) ( ( (float) 1193182.0 / (float) i8253.effectivedata[portnum]) * (float) 1000.0) ) / (float) 1000.0;     
        
        ///printf("[DEBUG] PIT channel %u counter changed to %u (%f Hz)\n", portnum, i8253.chandata[portnum], i8253.chanfreq[portnum]);
        
        break;
      case 3: //mode/command
        i8253.accessmode[value>>6] = (value >> 4) & 3;
        if (i8253.accessmode[value>>6] == PIT_MODE_TOGGLE) i8253.bytetoggle[value>>6] = 0;
        break;
  }
}

uint8_t in8253 (uint16_t portnum) {
  uint8_t curbyte;
  portnum &= 3;
  switch (portnum) {
      case 0:
      case 1:
      case 2: //channel data
        if ( (i8253.accessmode[portnum] == 0) || (i8253.accessmode[portnum] == PIT_MODE_LOBYTE) || ( (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) && (i8253.bytetoggle[portnum] == 0) ) ) curbyte = 0;
        else if ( (i8253.accessmode[portnum] == PIT_MODE_HIBYTE) || ( (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) && (i8253.bytetoggle[portnum] == 1) ) ) curbyte = 1;
        if ( (i8253.accessmode[portnum] == 0) || (i8253.accessmode[portnum] == PIT_MODE_LOBYTE) || ( (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) && (i8253.bytetoggle[portnum] == 0) ) ) curbyte = 0;
        else if ( (i8253.accessmode[portnum] == PIT_MODE_HIBYTE) || ( (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) && (i8253.bytetoggle[portnum] == 1) ) ) curbyte = 1;
        if ( (i8253.accessmode[portnum] == 0) || (i8253.accessmode[portnum] == PIT_MODE_TOGGLE) ) i8253.bytetoggle[portnum] = (~i8253.bytetoggle[portnum]) & 1;
        if (curbyte == 0) { //low byte
            return ( (uint8_t) i8253.counter[portnum]);
          }
        else {   //high byte
            return ( (uint8_t) (i8253.counter[portnum] >> 8) );
          }
        break;
    }
  return (0);
}

void init8253(void) {
  memset (&i8253, 0, sizeof (i8253_s) );
  set_portout_redirector (0x40, 0x43, (void*)&out8253);
  set_portin_redirector (0x40, 0x43, (void*)&in8253);
} 
//████████████████████████████████████████









//i8259.c
// i8259.c: functions to emulate the Intel 8259 prioritized interrupt controller.
// note: this is not a very complete 8259 implementation, but for the purposes
// of a PC, it's all we need. 

struct structpic i8259;

extern void set_portout_redirector(uint16_t startport, uint16_t endport, void *callback);
extern void set_portin_redirector(uint16_t startport, uint16_t endport, void *callback);

uint8_t in8259(uint16_t portnum) 
{
  switch (portnum & 1) {
       case 0:
      if (i8259.readmode==0) return(i8259.irr); else return(i8259.isr);
       case 1: //read mask register
      return(i8259.imr);
  }
  return (0);
}

extern uint32_t makeupticks;
void out8259(uint16_t portnum, uint8_t value) 
{
   uint8_t i;
   switch (portnum & 1) {
    case 0:
                  if (value & 0x10) { //begin initialization sequence
                    i8259.icwstep = 1;
                    i8259.imr = 0; //clear interrupt mask register
                    i8259.icw[i8259.icwstep++] = value;
                    return;
                  }
     if ((value & 0x98)==8) { //it's an OCW3
      if (value & 2) i8259.readmode = value & 2;
     }
     if (value & 0x20) 
                 { 
                   //EOI command
                   //keyboardwaitack = 0;
                   for (i=0; i<8; i++)
                     if ((i8259.isr >> i) & 1) {
                       i8259.isr ^= (1 << i);
                       if ((i==0) && (makeupticks>0)) { makeupticks = 0; i8259.irr |= 1; }
                       return;
                     }
     }
     break;
    case 1:
     if ((i8259.icwstep==3) && (i8259.icw[1] & 2)) i8259.icwstep = 4; //single mode, so don't read ICW3
     if (i8259.icwstep<5) { i8259.icw[i8259.icwstep++] = value; return; }
     //if we get to this point, this is just a new IMR value
     i8259.imr = value;
     break;
   }
}

uint8_t nextintr(void) 
{
  uint8_t i, tmpirr;
  tmpirr = i8259.irr & (~i8259.imr); //XOR request register with inverted mask register
  for (i=0; i<8; i++)
    if ((tmpirr >> i) & 1) {
       i8259.irr ^= (1 << i);
       i8259.isr |= (1 << i);
       return(i8259.icw[2] + i);
    }
  return(0); //this won't be reached, but without it the compiler gives a warning
}

void doirq(uint8_t irqnum)
{
   i8259.irr |= (1 << irqnum);
   //if (irqnum == 1) keyboardwaitack = 1;
}

void init8259(void) 
{
  memset((void *)&i8259, 0, sizeof(structpic));
  set_portout_redirector(0x20, 0x21, (void*)&out8259);
  set_portin_redirector(0x20, 0x21, (void*)&in8259);
} 
//████████████████████████████████████████
//i8237.c
// i8237.c: functions to emulate the Intel 8237 DMA controller.
// the Sound Blaster Pro emulation functions rely on this! 

///extern struct blaster_s blaster;

struct dmachan_s dmachan[4];
uint8_t flipflop = 0;

extern void set_portout_redirector (uint16_t startport, uint16_t endport, void *callback);
extern void set_portin_redirector (uint16_t startport, uint16_t endport, void *callback);
extern uint8_t read86 (uint32_t addr32);

///extern uint8_t RAM[0x100000];
uint8_t read8237 (uint8_t channel) {
  uint8_t ret;
  if (dmachan[channel].masked) return (128);
  if (dmachan[channel].autoinit && (dmachan[channel].count > dmachan[channel].reload) ) dmachan[channel].count = 0;
  if (dmachan[channel].count > dmachan[channel].reload) return (128);
  //if (dmachan[channel].direction) ret = RAM[dmachan[channel].page + dmachan[channel].addr + dmachan[channel].count];
  //  else ret = RAM[dmachan[channel].page + dmachan[channel].addr - dmachan[channel].count];
  if (dmachan[channel].direction == 0) ret = RAM[dmachan[channel].page + dmachan[channel].addr + dmachan[channel].count];
  else ret = RAM[dmachan[channel].page + dmachan[channel].addr - dmachan[channel].count];
  dmachan[channel].count++;
  return (ret);
}

void out8237 (uint16_t addr, uint8_t value) {
  uint8_t channel;
#ifdef DEBUG_DMA
  printf ("out8237(0x%X, %X);\n", addr, value);
#endif
  switch (addr) {
      case 0x2: //channel 1 address register
        if (flipflop == 1) dmachan[1].addr = (dmachan[1].addr & 0x00FF) | ( (uint32_t) value << 8);
        else dmachan[1].addr = (dmachan[1].addr & 0xFF00) | value;
#ifdef DEBUG_DMA
        if (flipflop == 1) printf ("[NOTICE] DMA channel 1 address register = %04X\n", dmachan[1].addr);
#endif
        flipflop = ~flipflop & 1;
        break;
      case 0x3: //channel 1 count register
        if (flipflop == 1) dmachan[1].reload = (dmachan[1].reload & 0x00FF) | ( (uint32_t) value << 8);
        else dmachan[1].reload = (dmachan[1].reload & 0xFF00) | value;
        if (flipflop == 1) {
            if (dmachan[1].reload == 0) dmachan[1].reload = 65536;
            dmachan[1].count = 0;
#ifdef DEBUG_DMA
            printf ("[NOTICE] DMA channel 1 reload register = %04X\n", dmachan[1].reload);
#endif
          }
        flipflop = ~flipflop & 1;
        break;
      case 0xA: //write single mask register
        channel = value & 3;
        dmachan[channel].masked = (value >> 2) & 1;
#ifdef DEBUG_DMA
        printf ("[NOTICE] DMA channel %u masking = %u\n", channel, dmachan[channel].masked);
#endif
        break;
      case 0xB: //write mode register
        channel = value & 3;
        dmachan[channel].direction = (value >> 5) & 1;
        dmachan[channel].autoinit = (value >> 4) & 1;
        dmachan[channel].writemode = (value >> 2) & 1; //not quite accurate
#ifdef DEBUG_DMA
        printf ("[NOTICE] DMA channel %u write mode reg: direction = %u, autoinit = %u, write mode = %u\n",
                channel, dmachan[channel].direction, dmachan[channel].autoinit, dmachan[channel].writemode);
#endif
        break;
      case 0xC: //clear byte pointer flip-flop
#ifdef DEBUG_DMA
        printf ("[NOTICE] DMA cleared byte pointer flip-flop\n");
#endif
        flipflop = 0;
        break;
      case 0x83: //DMA channel 1 page register
        dmachan[1].page = (uint32_t) value << 16;
#ifdef DEBUG_DMA
        printf ("[NOTICE] DMA channel 1 page base = %05X\n", dmachan[1].page);
#endif
        break;
    }
}

uint8_t in8237 (uint16_t addr) {
#ifdef DEBUG_DMA
  printf ("in8237(0x%X);\n", addr);
#endif
  return (0);
}

void init8237() {
  memset (&dmachan[0], 0, sizeof (dmachan_s) );
  memset (&dmachan[1], 0, sizeof (dmachan_s) );
  memset (&dmachan[2], 0, sizeof (dmachan_s) );
  memset (&dmachan[3], 0, sizeof (dmachan_s) );
  set_portout_redirector (0x00, 0x0F, (void*) &out8237); ///
  set_portin_redirector (0x00, 0x0F, (void*) &in8237); ///
  set_portout_redirector (0x80, 0x8F, (void*) &out8237); ///
  set_portin_redirector (0x80, 0x8F, (void*) &in8237); ///
}
//████████████████████████████████████████























//████████████████████████████████████████
uint32_t loadbinary (uint32_t addr32, uint8_t *filename, uint8_t roflag) 
{
  uint32_t readsize = 0;
  
  if (memcmp((char*)filename,"rombasic", 8) == 0) {
    readsize = sizeof(rombasic_data);
    memcpy(&RAM[addr32], &rombasic_data[0], readsize); ///
  } else if (memcmp((char*)filename,"videorom", 8) == 0) {
    readsize = sizeof(videorom_data);
    memcpy(&RAM[addr32], &videorom_data[0], readsize); ///
  } else if (memcmp((char*)filename,"pcxtbios", 8) == 0) {
    readsize = sizeof(pcxtbios_data);
    memcpy(&RAM[addr32], &pcxtbios_data[0], readsize); ///
  } else {
    printf ("ROM load eror.\n");
    while (1) {}
  }
  return (readsize);
} 
//--------------------------------------------------------------------------------
uint32_t loadrom (uint32_t addr32, uint8_t *filename, uint8_t failure_fatal) 
{  
  uint32_t readsize;
  readsize = loadbinary (addr32, filename, 1);
  printf ("Loaded %s at 0x%05X (%lu KB)\n", filename, addr32, readsize >> 10); //
  return (readsize);
}

//████████████████████████████████████████
//disk.h
struct struct_drive {
///  File *diskfile; ///
  
  uint32_t filesize;
  uint16_t cyls;
  uint16_t sects;
  uint16_t heads;
  uint8_t inserted;
  char *filename;
};
//████████████████████████████████████████
//disk.c
///struct struct_drive disk[255]; //0=floppy (A:), 1=floppy (B:), 0x80=disk (C:) ...

struct struct_drive disk[2]; //0=floppy (A:), 1=floppy (B:), 0x80=disk (C:) = DISABLED...

uint8_t * DISKIMAGE; ///NCX: data...

//********************************************************************************  
uint8_t insertdisk (uint8_t drivenum, char *filename) 
{
  if (drivenum==1) { //FLOPPY B in HIMEM ... 2.88MB max.!
     disk[drivenum].inserted = 0;
     uint32_t disksize=0;
///     HIMEM_load_file(DATAPATH2,&disksize); ///2.88MB floppy 
//--------------------------------------------------------------------------------
          if (LOADED_ROM==3) {
            screenmemory_fillscreen(0); //black color
            HIMEM_load_file(DATAPATH2, &disksize);
            LOADED_ROM=4;
          }   
          if (LOADED_ROM==13) {
            screenmemory_fillscreen(0); //black color
            HIMEM_load_flash(DATAPATH2, &disksize);
            LOADED_ROM=14;
          }        
          if (LOADED_ROM==23) {
            screenmemory_fillscreen(0); //black color
            HIMEM_load_usbstorage(DATAPATH2, &disksize);
            LOADED_ROM=24;
          }
//--------------------------------------------------------------------------------


///     if (disksize==0) return(0);
     
///printf("\n\ndrivenum1 disksize: %d\n\n",disksize);
     
     if (disksize) {
        disk[drivenum].inserted = 1; 
        disk[drivenum].filesize = disksize; ///
     } else return(0);
  } else if (drivenum==0)  { //FLOPPY A in PSRAM ... 1.44MB max.!
     disk[drivenum].inserted = 0;
     uint32_t disksize=0; ///
     ///DISKIMAGE = PSRAM_load_file(filename,&disksize); ///  

//--------------------------------------------------------------------------------
          if (LOADED_ROM==3) {
            screenmemory_fillscreen(0); //black color
            DISKIMAGE = PSRAM_load_file(DATAPATH1, &disksize);
            LOADED_ROM=4;
          }   
          if (LOADED_ROM==13) {
            screenmemory_fillscreen(0); //black color
            DISKIMAGE = PSRAM_load_flash(DATAPATH1, &disksize);
            LOADED_ROM=14;
          }        
          if (LOADED_ROM==23) {
            screenmemory_fillscreen(0); //black color
            DISKIMAGE = PSRAM_load_usbstorage(DATAPATH1, &disksize);
            LOADED_ROM=24;
          }
//--------------------------------------------------------------------------------

     if (disksize==0) return(0);
     
     if (DISKIMAGE) {
        disk[drivenum].inserted = 1; 
        disk[drivenum].filesize = disksize; ///      
     } else return(0);
  } else { //other drives is NOT SUPPORTED... 
    return(0);
  }
  
//--------------------------------------------------------------------------------  
//  if (drivenum >= 0x80) //it's a HARD DISK image
//  { 
//    //it's a hard disk image
//    disk[drivenum].sects = 63;
//    disk[drivenum].heads = 16;
//    disk[drivenum].cyls = disk[drivenum].filesize / (disk[drivenum].sects * disk[drivenum].heads * 512);
//    hdcount++;
//  } else {   
  
  
  //it's a floppy image
  if (drivenum < 2) { //ONLY FLOPPY IMAGES we SUPPORT here
    //2.88MB floppy (3.5")  
    disk[drivenum].cyls = 80;
    disk[drivenum].sects = 36;
    disk[drivenum].heads = 2;
    ///1.44MB floppy (3.5")  
    if (disk[drivenum].filesize <= 1474560) disk[drivenum].sects = 18;
    ///1.2MB floppy (5.25")
    if (disk[drivenum].filesize <= 1228800) disk[drivenum].sects = 15;
    //720kB floppy (3.5")    
    if (disk[drivenum].filesize <= 737280) disk[drivenum].sects = 9;
    ///360kBB floppy (5.25")
    if (disk[drivenum].filesize <= 368640) {
      disk[drivenum].cyls = 40;
      disk[drivenum].sects = 9;
    }
    ///??? floppy     
    if (disk[drivenum].filesize <= 163840) {
      disk[drivenum].cyls = 40;
      disk[drivenum].sects = 8;
      disk[drivenum].heads = 1;
    }
    //printf("\n\ndrivenum %d:   disksize: %d  cyl: %d sect: %d heads: %d \n\n",drivenum,disk[drivenum].filesize,disk[drivenum].cyls,disk[drivenum].sects,disk[drivenum].heads);  
    return(1);
  }
  return 0;
}  
//********************************************************************************  
void ejectdisk (uint8_t drivenum)
{
  disk[drivenum].inserted = 0;
  if (drivenum==0) free(DISKIMAGE); ///
}
//********************************************************************************  
//0=floppy (A:), 1=floppy (B:), 0x80=disk (C:) = DISABLED...
void readdisk (uint8_t drivenum, uint16_t dstseg, uint16_t dstoff, uint16_t cyl, uint16_t sect, uint16_t head, uint16_t sectcount) {

//--------------------------------------------------------------------------------  
  if (drivenum==1) {
//--------------------------------------------------------------------------------  
#if HIMEM  
    uint32_t bytes_read = 0;
    uint32_t memdest, lba, fileoffset, cursect, sectoffset;
    if (!sect || !disk[drivenum].inserted) return;
    lba = ( (uint32_t) cyl * (uint32_t) disk[drivenum].heads + (uint32_t) head) * (uint32_t) disk[drivenum].sects + (uint32_t) sect - 1;
    fileoffset = lba * 512;

    if (fileoffset > disk[drivenum].filesize) return; ///NCX: fatal problem!
   
    memdest = ( (uint32_t) dstseg << 4) + (uint32_t) dstoff;
  
    //for the readdisk function, we need to use write86 instead of directly fread'ing into
    //the RAM array, so that read-only flags are honored. otherwise, a program could load
    //data from a disk over BIOS or other ROM code that it shouldn't be able to.

     for (cursect=0; cursect<sectcount; cursect++) {
      memset(HIMEM_READBUFFER,0,512); //NCX: important clear buffer
#if MICROSD
      himem_read( fileoffset + (cursect * 512),512,HIMEM_READBUFFER);
#endif
      for (sectoffset=0; sectoffset<512; sectoffset++) {
        write86 (memdest++, HIMEM_READBUFFER[sectoffset]); /// Chyba bola v HIMEM_READBUFFER musi byt malloc!
      }
    }
    regs_.byteregs[regal] = cursect;
    cf = 0;
    regs_.byteregs[regah] = 0;
#endif  
//--------------------------------------------------------------------------------  
  } else if (drivenum==0) {
//--------------------------------------------------------------------------------  
    uint32_t bytes_read = 0;
    uint32_t memdest, lba, fileoffset, cursect, sectoffset;
    if (!sect || !disk[drivenum].inserted) return;
    lba = ( (uint32_t) cyl * (uint32_t) disk[drivenum].heads + (uint32_t) head) * (uint32_t) disk[drivenum].sects + (uint32_t) sect - 1;
    fileoffset = lba * 512;

    if (fileoffset > disk[drivenum].filesize) {
      printf("SERIOUS ERROR R0 \n");
    }
  
    if (fileoffset > disk[drivenum].filesize) return;
    memdest = ( (uint32_t) dstseg << 4) + (uint32_t) dstoff;
  
    //for the readdisk function, we need to use write86 instead of directly fread'ing into
    //the RAM array, so that read-only flags are honored. otherwise, a program could load
    //data from a disk over BIOS or other ROM code that it shouldn't be able to.
    for (cursect=0; cursect<sectcount; cursect++) {
      for (sectoffset=0; sectoffset<512; sectoffset++) {
        write86 (memdest++, DISKIMAGE[fileoffset + (cursect * 512) + sectoffset]); ///
      }
    }
    regs_.byteregs[regal] = cursect;
    cf = 0;
    regs_.byteregs[regah] = 0;
  }
}
//--------------------------------------------------------------------------------  

void writedisk (uint8_t drivenum, uint16_t dstseg, uint16_t dstoff, uint16_t cyl, uint16_t sect, uint16_t head, uint16_t sectcount) {
//--------------------------------------------------------------------------------  
  if (drivenum==1) {
//--------------------------------------------------------------------------------  
#if HIMEM
    uint32_t bytes_written = 0;
    uint32_t memdest, lba, fileoffset, cursect, sectoffset;
    if (!sect || !disk[drivenum].inserted) return;

    lba = ( (uint32_t) cyl * (uint32_t) disk[drivenum].heads + (uint32_t) head) * (uint32_t) disk[drivenum].sects + (uint32_t) sect - 1;
    fileoffset = lba * 512;

    if (fileoffset > disk[drivenum].filesize) {
      printf("SERIOUS ERROR W1\n");
    }
  
    memdest = ( (uint32_t) dstseg << 4) + (uint32_t) dstoff;
  
    for (cursect=0; cursect<sectcount; cursect++) {
      memset(HIMEM_READBUFFER,0,512);  // important: clear buffer
      for (sectoffset=0; sectoffset < 512; sectoffset++) {
        HIMEM_READBUFFER[sectoffset]= read86 (memdest++); 
      }
#if MICROSD    
      himem_write( fileoffset + (cursect * 512),512,HIMEM_READBUFFER);
#endif
      if (bytes_written != 512) {
        asm("nop");
      }
    }
    regs_.byteregs[regal] = (uint8_t) sectcount;
    cf = 0;
    regs_.byteregs[regah] = 0;
#endif
//--------------------------------------------------------------------------------  
  } else if (drivenum==0) {
//--------------------------------------------------------------------------------  
   
    uint32_t bytes_written = 0;
    uint32_t memdest, lba, fileoffset, cursect, sectoffset;
    if (!sect || !disk[drivenum].inserted) return;

    lba = ( (uint32_t) cyl * (uint32_t) disk[drivenum].heads + (uint32_t) head) * (uint32_t) disk[drivenum].sects + (uint32_t) sect - 1;
    fileoffset = lba * 512;

    if (fileoffset > disk[drivenum].filesize) {
      printf("SERIOUS ERROR W0 \n");
    }
   
    if (fileoffset > disk[drivenum].filesize) return;
    memdest = ( (uint32_t) dstseg << 4) + (uint32_t) dstoff;

    for (cursect=0; cursect<sectcount; cursect++) {
      for (sectoffset=0; sectoffset < 512; sectoffset++) {
        DISKIMAGE[fileoffset + (cursect * 512) + sectoffset] = read86 (memdest++); ///
      }
      if (bytes_written != 512) {
        asm("nop");
      }
    }
    regs_.byteregs[regal] = (uint8_t) sectcount;
    cf = 0;
    regs_.byteregs[regah] = 0;
  }
}
//--------------------------------------------------------------------------------  

void diskhandler() {
  static uint8_t lastdiskah[256], lastdiskcf[256];
  switch (regs_.byteregs[regah]) {
    case 0: //reset disk system
      regs_.byteregs[regah] = 0;
      cf = 0; //useless function in an emulator. say success and return.
      break;
    case 1: //return last status
      regs_.byteregs[regah] = lastdiskah[regs_.byteregs[regdl]];
      cf = lastdiskcf[regs_.byteregs[regdl]];
      return;
    case 2: //read sector(s) into memory
//................................................................................      
      if (regs_.byteregs[regdl] < 2) { // we not use HDD in this case
//................................................................................             
        if (disk[regs_.byteregs[regdl]].inserted) {
          readdisk (regs_.byteregs[regdl], segregs[reges], getreg16 (regbx), regs_.byteregs[regch] + (regs_.byteregs[regcl]/64) *256, regs_.byteregs[regcl] & 63, regs_.byteregs[regdh], regs_.byteregs[regal]);
          cf = 0;
          regs_.byteregs[regah] = 0;
        } else {
          cf = 1;
          regs_.byteregs[regah] = 1;
        }
      }  
      break;
    case 3: //write sector(s) from memory
//................................................................................      
      if (regs_.byteregs[regdl] < 2) {// we not use HDD in this case
//................................................................................      
        if (disk[regs_.byteregs[regdl]].inserted) {
          writedisk (regs_.byteregs[regdl], segregs[reges], getreg16 (regbx), regs_.byteregs[regch] + (regs_.byteregs[regcl]/64) *256, regs_.byteregs[regcl] & 63, regs_.byteregs[regdh], regs_.byteregs[regal]);
          cf = 0;
          regs_.byteregs[regah] = 0;
        } else {
          cf = 1;
          regs_.byteregs[regah] = 1;
        }
      }  
      break;
    case 4:
    case 5: //format track
      cf = 0;
      regs_.byteregs[regah] = 0;
      break;
    case 8: //get drive parameters
//................................................................................      
      if (regs_.byteregs[regdl] < 2) {// we not use HDD in this case
//................................................................................            
        if (disk[regs_.byteregs[regdl]].inserted) {                                 
          cf = 0;
          regs_.byteregs[regah] = 0;
          regs_.byteregs[regch] = disk[regs_.byteregs[regdl]].cyls - 1;
          regs_.byteregs[regcl] = disk[regs_.byteregs[regdl]].sects & 63;
          regs_.byteregs[regcl] = regs_.byteregs[regcl] + (disk[regs_.byteregs[regdl]].cyls/256) *64;
          regs_.byteregs[regdh] = disk[regs_.byteregs[regdl]].heads - 1;
          
          //segregs[reges] = 0; regs_.wordregs[regdi] = 0x7C0B; //floppy parameter table
          if (regs_.byteregs[regdl]<0x80) {
            regs_.byteregs[regbl] = 4; //else regs_.byteregs[regbl] = 0;
            regs_.byteregs[regdl] = 2;
          } else regs_.byteregs[regdl] = hdcount;
        } else {
          cf = 1;
          regs_.byteregs[regah] = 0xAA;
        }
      }  
      break;
    default:
      cf = 1;
  }//end of switch
  lastdiskah[regs_.byteregs[regdl]] = regs_.byteregs[regah];
  lastdiskcf[regs_.byteregs[regdl]] = cf;
  if (regs_.byteregs[regdl] & 0x80) RAM[0x474] = regs_.byteregs[regah];
} 
//████████████████████████████████████████




//████████████████████████████████████████
// video.c: many various functions to emulate bits of the video controller.
//   a lot of this code is inefficient, and just plain ugly. i plan to rework
//   large sections of it soon. 

extern union _bytewordregs_ regs_;
extern uint16_t segregs[4];
extern uint8_t scrmodechange;

uint8_t vidmode, cgabg, blankattr, vidgfxmode, vidcolor;
uint16_t cursx, cursy, cols = 80, rows = 25, vgapage, cursorposition, cursorvisible;
uint8_t updatedscreen, clocksafe, port3da, port6 /*, portout16*/;
uint32_t videobase= 0xB8000, textbase = 0xB8000, x, y;
uint32_t usefullscreen = 0, usegrabmode = 1;
uint8_t latchRGB = 0, latchPal = 0, VGA_latch[4], stateDAC = 0;
uint8_t latchReadRGB = 0, latchReadPal = 0;
uint32_t tempRGB;
uint16_t oldw, oldh; //used when restoring screen mode

uint32_t rgb(uint32_t r, uint32_t g, uint32_t b) {
#ifdef __BIG_ENDIAN__
  return ( (r<<24) | (g<<16) | (b<<8) );
#else
  return (r | (g<<8) | (b<<16) );
#endif
}

extern uint32_t nw, nh;
void vidinterrupt() {
  uint32_t tempcalc, memloc, n;
  updatedscreen = 1;
  switch (regs_.byteregs[regah]) { //what video interrupt function?
      case 0: //set video mode
#if DEBUG > 0
        printf ("Set video mode %02Xh\n", regs_.byteregs[regal]);
#endif
        VGA_SC[0x4] = 0; //VGA modes are in chained mode by default after a mode switch
        //regs_.byteregs[regal] = 3;
        switch (regs_.byteregs[regal] & 0x7F) {
          case 0: //40x25 mono text
            videobase = textbase;
            cols = 40;
            rows = 25;
            vidcolor = 0;
            vidgfxmode = 0;
            blankattr = 7;
            for (tempcalc = videobase; tempcalc<videobase+16384; tempcalc+=2) {
              RAM[tempcalc] = 0;
              RAM[tempcalc+1] = blankattr;
            }
            break;
          case 1: //40x25 color text
            videobase = textbase;
            cols = 40;
            rows = 25;
            vidcolor = 1;
            vidgfxmode = 0;
            blankattr = 7;
            for (tempcalc = videobase; tempcalc<videobase+16384; tempcalc+=2) {
              RAM[tempcalc] = 0;
              RAM[tempcalc+1] = blankattr;
            }
            portram[0x3D8] = portram[0x3D8] & 0xFE;
            break;
          case 2: //80x25 mono text
            videobase = textbase;
            cols = 80;
            rows = 25;
            vidcolor = 1;
            vidgfxmode = 0;
            blankattr = 7;
            for (tempcalc = videobase; tempcalc<videobase+16384; tempcalc+=2) {
              RAM[tempcalc] = 0;
              RAM[tempcalc+1] = blankattr;
            }
            portram[0x3D8] = portram[0x3D8] & 0xFE;
            break;
          case 3: //80x25 color text
            videobase = textbase;
            cols = 80;
            rows = 25;
            vidcolor = 1;
            vidgfxmode = 0;
            blankattr = 7;
            for (tempcalc = videobase; tempcalc<videobase+16384; tempcalc+=2) {
              RAM[tempcalc] = 0;
              RAM[tempcalc+1] = blankattr;
            }
            portram[0x3D8] = portram[0x3D8] & 0xFE;
            break;
          case 4:
          case 5: //80x25 color text
            videobase = textbase;
            cols = 40;
            rows = 25;
            vidcolor = 1;
            vidgfxmode = 1;
            blankattr = 7;
            for (tempcalc = videobase; tempcalc<videobase+16384; tempcalc+=2) {
              RAM[tempcalc] = 0;
              RAM[tempcalc+1] = blankattr;
            }
            if (regs_.byteregs[regal]==4) portram[0x3D9] = 48;
            else portram[0x3D9] = 0;
            break;
          case 6:
            videobase = textbase;
            cols = 80;
            rows = 25;
            vidcolor = 0;
            vidgfxmode = 1;
            blankattr = 7;
            for (tempcalc = videobase; tempcalc<videobase+16384; tempcalc+=2) {
              RAM[tempcalc] = 0;
              RAM[tempcalc+1] = blankattr;
            }
            portram[0x3D8] = portram[0x3D8] & 0xFE;
            break;
          case 127:
            videobase = 0xB8000;
            cols = 90;
            rows = 25;
            vidcolor = 0;
            vidgfxmode = 1;
            for (tempcalc = videobase; tempcalc<videobase+16384; tempcalc++) {
              RAM[tempcalc] = 0;
            }
            portram[0x3D8] = portram[0x3D8] & 0xFE;
            break;
          case 0x9: //320x200 16-color
            videobase = 0xB8000;
            cols = 40;
            rows = 25;
            vidcolor = 1;
            vidgfxmode = 1;
            blankattr = 0;
            if ( (regs_.byteregs[regal]&0x80) ==0) for (tempcalc = videobase; tempcalc<videobase+65535; tempcalc+=2) {
                RAM[tempcalc] = 0;
                RAM[tempcalc+1] = blankattr;
            }
            portram[0x3D8] = portram[0x3D8] & 0xFE;
            break;
          case 0xD: //320x200 16-color
          case 0x12: //640x480 16-color
          case 0x13: //320x200 256-color
            videobase = 0xA0000;
            cols = 40;
            rows = 25;
            vidcolor = 1;
            vidgfxmode = 1;
            blankattr = 0;
            for (tempcalc = videobase; tempcalc<videobase+65535; tempcalc+=2) {
              RAM[tempcalc] = 0;
              RAM[tempcalc+1] = blankattr;
            }
            portram[0x3D8] = portram[0x3D8] & 0xFE;
            break;
        }
        vidmode = regs_.byteregs[regal] & 0x7F;
        RAM[0x449] = vidmode;
        RAM[0x44A] = (uint8_t) cols;
        RAM[0x44B] = 0;
        RAM[0x484] = (uint8_t) (rows - 1);
        cursx = 0;
        cursy = 0;
        if ( (regs_.byteregs[regal] & 0x80) == 0x00) {
          memset (&RAM[0xA0000], 0, 0x1FFFF);
          memset (VRAM, 0, 262144);
        }
        switch (vidmode) {
          case 127: //hercules
            nw = oldw = 720;
            nh = oldh = 348;
            scrmodechange = 1;
            break;
          case 0x12:
            nw = oldw = 640;
            nh = oldh = 480;
            scrmodechange = 1;
            break;
          case 0x13:
            oldw = 640;
            oldh = 400;
            nw = 320;
            nh = 200;
            scrmodechange = 1;
            break;
          default:
            nw = oldw = 640;
            nh = oldh = 400;
            scrmodechange = 1;
            break;
          }
          break;
        case 0x10: //VGA DAC functions
          switch (regs_.byteregs[regal]) {
            case 0x10: //set individual DAC register
              palettevga[getreg16 (regbx) ] = rgb((regs_.byteregs[regdh] & 63) << 2, (regs_.byteregs[regch] & 63) << 2, (regs_.byteregs[regcl] & 63) << 2);
              break;
            case 0x12: //set block of DAC registers
              memloc = segregs[reges]*16+getreg16 (regdx);
              for (n=getreg16 (regbx); n< (uint32_t) (getreg16 (regbx) +getreg16 (regcx) ); n++) {
                palettevga[n] = rgb(read86(memloc) << 2, read86(memloc + 1) << 2, read86(memloc + 2) << 2);
                memloc += 3;
              }
          }
      break;
    case 0x1A: //get display combination code (ps, vga/mcga)
        regs_.byteregs[regal] = 0x1A;
        regs_.byteregs[regbl] = 0x8;
      break;
  }
}

void initcga() {
  palettecga[0] = 0;
  palettecga[1] = rgb (0, 0, 0xAA);
  palettecga[2] = rgb (0, 0xAA, 0);
  palettecga[3] = rgb (0, 0xAA, 0xAA);
  palettecga[4] = rgb (0xAA, 0, 0);
  palettecga[5] = rgb (0xAA, 0, 0xAA);
  palettecga[6] = rgb (0xAA, 0x55, 0);
  palettecga[7] = rgb (0xAA, 0xAA, 0xAA);
  palettecga[8] = rgb (0x55, 0x55, 0x55);
  palettecga[9] = rgb (0x55, 0x55, 0xFF);
  palettecga[10] = rgb (0x55, 0xFF, 0x55);
  palettecga[11] = rgb (0x55, 0xFF, 0xFF);
  palettecga[12] = rgb (0xFF, 0x55, 0x55);
  palettecga[13] = rgb (0xFF, 0x55, 0xFF);
  palettecga[14] = rgb (0xFF, 0xFF, 0x55);
  palettecga[15] = rgb (0xFF, 0xFF, 0xFF);
  palettevga[0] = rgb (0, 0, 0);
  palettevga[1] = rgb (0, 0, 169);
  palettevga[2] = rgb (0, 169, 0);
  palettevga[3] = rgb (0, 169, 169);
  palettevga[4] = rgb (169, 0, 0);
  palettevga[5] = rgb (169, 0, 169);
  palettevga[6] = rgb (169, 169, 0);
  palettevga[7] = rgb (169, 169, 169);
  palettevga[8] = rgb (0, 0, 84);
  palettevga[9] = rgb (0, 0, 255);
  palettevga[10] = rgb (0, 169, 84);
  palettevga[11] = rgb (0, 169, 255);
  palettevga[12] = rgb (169, 0, 84);
  palettevga[13] = rgb (169, 0, 255);
  palettevga[14] = rgb (169, 169, 84);
  palettevga[15] = rgb (169, 169, 255);
  palettevga[16] = rgb (0, 84, 0);
  palettevga[17] = rgb (0, 84, 169);
  palettevga[18] = rgb (0, 255, 0);
  palettevga[19] = rgb (0, 255, 169);
  palettevga[20] = rgb (169, 84, 0);
  palettevga[21] = rgb (169, 84, 169);
  palettevga[22] = rgb (169, 255, 0);
  palettevga[23] = rgb (169, 255, 169);
  palettevga[24] = rgb (0, 84, 84);
  palettevga[25] = rgb (0, 84, 255);
  palettevga[26] = rgb (0, 255, 84);
  palettevga[27] = rgb (0, 255, 255);
  palettevga[28] = rgb (169, 84, 84);
  palettevga[29] = rgb (169, 84, 255);
  palettevga[30] = rgb (169, 255, 84);
  palettevga[31] = rgb (169, 255, 255);
  palettevga[32] = rgb (84, 0, 0);
  palettevga[33] = rgb (84, 0, 169);
  palettevga[34] = rgb (84, 169, 0);
  palettevga[35] = rgb (84, 169, 169);
  palettevga[36] = rgb (255, 0, 0);
  palettevga[37] = rgb (255, 0, 169);
  palettevga[38] = rgb (255, 169, 0);
  palettevga[39] = rgb (255, 169, 169);
  palettevga[40] = rgb (84, 0, 84);
  palettevga[41] = rgb (84, 0, 255);
  palettevga[42] = rgb (84, 169, 84);
  palettevga[43] = rgb (84, 169, 255);
  palettevga[44] = rgb (255, 0, 84);
  palettevga[45] = rgb (255, 0, 255);
  palettevga[46] = rgb (255, 169, 84);
  palettevga[47] = rgb (255, 169, 255);
  palettevga[48] = rgb (84, 84, 0);
  palettevga[49] = rgb (84, 84, 169);
  palettevga[50] = rgb (84, 255, 0);
  palettevga[51] = rgb (84, 255, 169);
  palettevga[52] = rgb (255, 84, 0);
  palettevga[53] = rgb (255, 84, 169);
  palettevga[54] = rgb (255, 255, 0);
  palettevga[55] = rgb (255, 255, 169);
  palettevga[56] = rgb (84, 84, 84);
  palettevga[57] = rgb (84, 84, 255);
  palettevga[58] = rgb (84, 255, 84);
  palettevga[59] = rgb (84, 255, 255);
  palettevga[60] = rgb (255, 84, 84);
  palettevga[61] = rgb (255, 84, 255);
  palettevga[62] = rgb (255, 255, 84);
  palettevga[63] = rgb (255, 255, 255);
  palettevga[64] = rgb (255, 125, 125);
  palettevga[65] = rgb (255, 157, 125);
  palettevga[66] = rgb (255, 190, 125);
  palettevga[67] = rgb (255, 222, 125);
  palettevga[68] = rgb (255, 255, 125);
  palettevga[69] = rgb (222, 255, 125);
  palettevga[70] = rgb (190, 255, 125);
  palettevga[71] = rgb (157, 255, 125);
  palettevga[72] = rgb (125, 255, 125);
  palettevga[73] = rgb (125, 255, 157);
  palettevga[74] = rgb (125, 255, 190);
  palettevga[75] = rgb (125, 255, 222);
  palettevga[76] = rgb (125, 255, 255);
  palettevga[77] = rgb (125, 222, 255);
  palettevga[78] = rgb (125, 190, 255);
  palettevga[79] = rgb (125, 157, 255);
  palettevga[80] = rgb (182, 182, 255);
  palettevga[81] = rgb (198, 182, 255);
  palettevga[82] = rgb (218, 182, 255);
  palettevga[83] = rgb (234, 182, 255);
  palettevga[84] = rgb (255, 182, 255);
  palettevga[85] = rgb (255, 182, 234);
  palettevga[86] = rgb (255, 182, 218);
  palettevga[87] = rgb (255, 182, 198);
  palettevga[88] = rgb (255, 182, 182);
  palettevga[89] = rgb (255, 198, 182);
  palettevga[90] = rgb (255, 218, 182);
  palettevga[91] = rgb (255, 234, 182);
  palettevga[92] = rgb (255, 255, 182);
  palettevga[93] = rgb (234, 255, 182);
  palettevga[94] = rgb (218, 255, 182);
  palettevga[95] = rgb (198, 255, 182);
  palettevga[96] = rgb (182, 255, 182);
  palettevga[97] = rgb (182, 255, 198);
  palettevga[98] = rgb (182, 255, 218);
  palettevga[99] = rgb (182, 255, 234);
  palettevga[100] = rgb (182, 255, 255);
  palettevga[101] = rgb (182, 234, 255);
  palettevga[102] = rgb (182, 218, 255);
  palettevga[103] = rgb (182, 198, 255);
  palettevga[104] = rgb (0, 0, 113);
  palettevga[105] = rgb (28, 0, 113);
  palettevga[106] = rgb (56, 0, 113);
  palettevga[107] = rgb (84, 0, 113);
  palettevga[108] = rgb (113, 0, 113);
  palettevga[109] = rgb (113, 0, 84);
  palettevga[110] = rgb (113, 0, 56);
  palettevga[111] = rgb (113, 0, 28);
  palettevga[112] = rgb (113, 0, 0);
  palettevga[113] = rgb (113, 28, 0);
  palettevga[114] = rgb (113, 56, 0);
  palettevga[115] = rgb (113, 84, 0);
  palettevga[116] = rgb (113, 113, 0);
  palettevga[117] = rgb (84, 113, 0);
  palettevga[118] = rgb (56, 113, 0);
  palettevga[119] = rgb (28, 113, 0);
  palettevga[120] = rgb (0, 113, 0);
  palettevga[121] = rgb (0, 113, 28);
  palettevga[122] = rgb (0, 113, 56);
  palettevga[123] = rgb (0, 113, 84);
  palettevga[124] = rgb (0, 113, 113);
  palettevga[125] = rgb (0, 84, 113);
  palettevga[126] = rgb (0, 56, 113);
  palettevga[127] = rgb (0, 28, 113);
  palettevga[128] = rgb (56, 56, 113);
  palettevga[129] = rgb (68, 56, 113);
  palettevga[130] = rgb (84, 56, 113);
  palettevga[131] = rgb (97, 56, 113);
  palettevga[132] = rgb (113, 56, 113);
  palettevga[133] = rgb (113, 56, 97);
  palettevga[134] = rgb (113, 56, 84);
  palettevga[135] = rgb (113, 56, 68);
  palettevga[136] = rgb (113, 56, 56);
  palettevga[137] = rgb (113, 68, 56);
  palettevga[138] = rgb (113, 84, 56);
  palettevga[139] = rgb (113, 97, 56);
  palettevga[140] = rgb (113, 113, 56);
  palettevga[141] = rgb (97, 113, 56);
  palettevga[142] = rgb (84, 113, 56);
  palettevga[143] = rgb (68, 113, 56);
  palettevga[144] = rgb (56, 113, 56);
  palettevga[145] = rgb (56, 113, 68);
  palettevga[146] = rgb (56, 113, 84);
  palettevga[147] = rgb (56, 113, 97);
  palettevga[148] = rgb (56, 113, 113);
  palettevga[149] = rgb (56, 97, 113);
  palettevga[150] = rgb (56, 84, 113);
  palettevga[151] = rgb (56, 68, 113);
  palettevga[152] = rgb (80, 80, 113);
  palettevga[153] = rgb (89, 80, 113);
  palettevga[154] = rgb (97, 80, 113);
  palettevga[155] = rgb (105, 80, 113);
  palettevga[156] = rgb (113, 80, 113);
  palettevga[157] = rgb (113, 80, 105);
  palettevga[158] = rgb (113, 80, 97);
  palettevga[159] = rgb (113, 80, 89);
  palettevga[160] = rgb (113, 80, 80);
  palettevga[161] = rgb (113, 89, 80);
  palettevga[162] = rgb (113, 97, 80);
  palettevga[163] = rgb (113, 105, 80);
  palettevga[164] = rgb (113, 113, 80);
  palettevga[165] = rgb (105, 113, 80);
  palettevga[166] = rgb (97, 113, 80);
  palettevga[167] = rgb (89, 113, 80);
  palettevga[168] = rgb (80, 113, 80);
  palettevga[169] = rgb (80, 113, 89);
  palettevga[170] = rgb (80, 113, 97);
  palettevga[171] = rgb (80, 113, 105);
  palettevga[172] = rgb (80, 113, 113);
  palettevga[173] = rgb (80, 105, 113);
  palettevga[174] = rgb (80, 97, 113);
  palettevga[175] = rgb (80, 89, 113);
  palettevga[176] = rgb (0, 0, 64);
  palettevga[177] = rgb (16, 0, 64);
  palettevga[178] = rgb (32, 0, 64);
  palettevga[179] = rgb (48, 0, 64);
  palettevga[180] = rgb (64, 0, 64);
  palettevga[181] = rgb (64, 0, 48);
  palettevga[182] = rgb (64, 0, 32);
  palettevga[183] = rgb (64, 0, 16);
  palettevga[184] = rgb (64, 0, 0);
  palettevga[185] = rgb (64, 16, 0);
  palettevga[186] = rgb (64, 32, 0);
  palettevga[187] = rgb (64, 48, 0);
  palettevga[188] = rgb (64, 64, 0);
  palettevga[189] = rgb (48, 64, 0);
  palettevga[190] = rgb (32, 64, 0);
  palettevga[191] = rgb (16, 64, 0);
  palettevga[192] = rgb (0, 64, 0);
  palettevga[193] = rgb (0, 64, 16);
  palettevga[194] = rgb (0, 64, 32);
  palettevga[195] = rgb (0, 64, 48);
  palettevga[196] = rgb (0, 64, 64);
  palettevga[197] = rgb (0, 48, 64);
  palettevga[198] = rgb (0, 32, 64);
  palettevga[199] = rgb (0, 16, 64);
  palettevga[200] = rgb (32, 32, 64);
  palettevga[201] = rgb (40, 32, 64);
  palettevga[202] = rgb (48, 32, 64);
  palettevga[203] = rgb (56, 32, 64);
  palettevga[204] = rgb (64, 32, 64);
  palettevga[205] = rgb (64, 32, 56);
  palettevga[206] = rgb (64, 32, 48);
  palettevga[207] = rgb (64, 32, 40);
  palettevga[208] = rgb (64, 32, 32);
  palettevga[209] = rgb (64, 40, 32);
  palettevga[210] = rgb (64, 48, 32);
  palettevga[211] = rgb (64, 56, 32);
  palettevga[212] = rgb (64, 64, 32);
  palettevga[213] = rgb (56, 64, 32);
  palettevga[214] = rgb (48, 64, 32);
  palettevga[215] = rgb (40, 64, 32);
  palettevga[216] = rgb (32, 64, 32);
  palettevga[217] = rgb (32, 64, 40);
  palettevga[218] = rgb (32, 64, 48);
  palettevga[219] = rgb (32, 64, 56);
  palettevga[220] = rgb (32, 64, 64);
  palettevga[221] = rgb (32, 56, 64);
  palettevga[222] = rgb (32, 48, 64);
  palettevga[223] = rgb (32, 40, 64);
  palettevga[224] = rgb (44, 44, 64);
  palettevga[225] = rgb (48, 44, 64);
  palettevga[226] = rgb (52, 44, 64);
  palettevga[227] = rgb (60, 44, 64);
  palettevga[228] = rgb (64, 44, 64);
  palettevga[229] = rgb (64, 44, 60);
  palettevga[230] = rgb (64, 44, 52);
  palettevga[231] = rgb (64, 44, 48);
  palettevga[232] = rgb (64, 44, 44);
  palettevga[233] = rgb (64, 48, 44);
  palettevga[234] = rgb (64, 52, 44);
  palettevga[235] = rgb (64, 60, 44);
  palettevga[236] = rgb (64, 64, 44);
  palettevga[237] = rgb (60, 64, 44);
  palettevga[238] = rgb (52, 64, 44);
  palettevga[239] = rgb (48, 64, 44);
  palettevga[240] = rgb (44, 64, 44);
  palettevga[241] = rgb (44, 64, 48);
  palettevga[242] = rgb (44, 64, 52);
  palettevga[243] = rgb (44, 64, 60);
  palettevga[244] = rgb (44, 64, 64);
  palettevga[245] = rgb (44, 60, 64);
  palettevga[246] = rgb (44, 52, 64);
  palettevga[247] = rgb (44, 48, 64);
  palettevga[248] = rgb (0, 0, 0);
  palettevga[249] = rgb (0, 0, 0);
  palettevga[250] = rgb (0, 0, 0);
  palettevga[251] = rgb (0, 0, 0);
  palettevga[252] = rgb (0, 0, 0);
  palettevga[253] = rgb (0, 0, 0);
  palettevga[254] = rgb (0, 0, 0);
  palettevga[255] = rgb (0, 0, 0);
}

uint16_t vtotal = 0;
void outVGA (uint16_t portnum, uint8_t value) {
  static uint8_t oldah, oldal;
  uint8_t flip3c0 = 0;
  updatedscreen = 1;
  switch (portnum) {
    case 0x3B8: //hercules support
      if ( ( (value & 2) == 2) && (vidmode != 127) ) {
          oldah = regs_.byteregs[regah];
          oldal = regs_.byteregs[regal];
          regs_.byteregs[regah] = 0;
          regs_.byteregs[regal] = 127;
          vidinterrupt();
          regs_.byteregs[regah] = oldah;
          regs_.byteregs[regal] = oldal;
        }
      if (value & 0x80) videobase = 0xB8000;
      else videobase = 0xB0000;
      break;
    case 0x3C0:
      if (flip3c0) {
          flip3c0 = 0;
          portram[0x3C0] = value & 255;
          return;
        }
      else {
          flip3c0 = 1;
          VGA_ATTR[portram[0x3C0]] = value & 255;
          return;
        }
    case 0x3C4: //sequence controller index
      portram[0x3C4] = value & 255;
      //if (portout16) VGA_SC[value & 255] = value >> 8;
      break;
    case 0x3C5: //sequence controller data
      VGA_SC[portram[0x3C4]] = value & 255;
      /*if (portram[0x3C4] == 2) {
      printf("VGA_SC[2] = %02X\n", value);
      }*/
      break;
    case 0x3D4: //CRT controller index
      portram[0x3D4] = value & 255;
      //if (portout16) VGA_CRTC[value & 255] = value >> 8;
      break;
    case 0x3C7: //color index register (read operations)
      latchReadPal = value & 255;
      latchReadRGB = 0;
      stateDAC = 0;
      break;
    case 0x3C8: //color index register (write operations)
      latchPal = value & 255;
      latchRGB = 0;
      tempRGB = 0;
      stateDAC = 3;
      break;
    case 0x3C9: //RGB data register
      value = value & 63;
      switch (latchRGB) {
#ifdef __BIG_ENDIAN__
        case 0: //red
          tempRGB = value << 26;
          break;
        case 1: //green
          tempRGB |= value << 18;
          break;
        case 2: //blue
          tempRGB |= value << 10;
          palettevga[latchPal] = tempRGB;
          latchPal = latchPal + 1;
          break;
#else
        case 0: //red
          tempRGB = value << 2;
          break;
        case 1: //green
          tempRGB |= value << 10;
          break;
        case 2: //blue
          tempRGB |= value << 18;
          palettevga[latchPal] = tempRGB;
          latchPal = latchPal + 1;
          break;
#endif
      }
      latchRGB = (latchRGB + 1) % 3;
      break;
    case 0x3D5: //cursor position latch
      VGA_CRTC[portram[0x3D4]] = value & 255;
      if (portram[0x3D4]==0xE) cursorposition = (cursorposition&0xFF) | (value<<8);
      else if (portram[0x3D4]==0xF) cursorposition = (cursorposition&0xFF00) |value;
      cursy = cursorposition/cols;
      cursx = cursorposition%cols;
      if (portram[0x3D4] == 6) {
          vtotal = value | ( ( (uint16_t) VGA_GC[7] & 1) << 8) | ( ( (VGA_GC[7] & 32) ? 1 : 0) << 9);
          //printf("Vertical total: %u\n", vtotal);
        }
      break;
    case 0x3CF:
      VGA_GC[portram[0x3CE]] = value;
      break;
    default:
      portram[portnum] = value;
  }
}

uint8_t inVGA (uint16_t portnum) {
  switch (portnum) {
    case 0x3C1:
      return ( (uint8_t) VGA_ATTR[portram[0x3C0]]);
    case 0x3C5:
      return ( (uint8_t) VGA_SC[portram[0x3C4]]);
    case 0x3D5:
      return ( (uint8_t) VGA_CRTC[portram[0x3D4]]);
    case 0x3C7: //DAC state
      return (stateDAC);
    case 0x3C8: //palette index
      return (latchReadPal);
    case 0x3C9: //RGB data register
      switch (latchReadRGB++) {
#ifdef __BIG_ENDIAN__
        case 0: //blue
          return ( (palettevga[latchReadPal] >> 26) & 63);
        case 1: //green
          return ( (palettevga[latchReadPal] >> 18) & 63);
        case 2: //red
          latchReadRGB = 0;
          return ( (palettevga[latchReadPal++] >> 10) & 63);
#else
        case 0: //blue
          return ( (palettevga[latchReadPal] >> 2) & 63);
        case 1: //green
          return ( (palettevga[latchReadPal] >> 10) & 63);
        case 2: //red
          latchReadRGB = 0;
          return ( (palettevga[latchReadPal++] >> 18) & 63);
#endif
      }
    case 0x3DA:
      return (port3da);
  }
  return (portram[portnum]); //this won't be reached, but without it the compiler gives a warning
}

#define shiftVGA(value) {\
  for (cnt=0; cnt<(VGA_GC[3] & 7); cnt++) {\
    value = (value >> 1) | ((value & 1) << 7);\
  }\
}

#define logicVGA(curval, latchval) {\
  switch ((VGA_GC[3]>>3) & 3) {\
       case 1: curval &= latchval; break;\
       case 2: curval |= latchval; break;\
       case 3: curval ^= latchval; break;\
  }\
}

uint8_t lastmode = 0, tempvalue;
void writeVGA (uint32_t addr32, uint8_t value) {
  uint32_t planesize;
  uint8_t curval, tempand, cnt;
  updatedscreen = 1;
  planesize = 0x10000;
  //if (lastmode != VGA_GC[5] & 3) printf("write mode %u\n", VGA_GC[5] & 3);
  //lastmode = VGA_GC[5] & 3;
  switch (VGA_GC[5] & 3) { //get write mode
      case 0:
        shiftVGA (value);
        if (VGA_SC[2] & 1) {
            if (VGA_GC[1] & 1)
              if (VGA_GC[0] & 1) curval = 255;
              else curval = 0;
            else curval = value;
            logicVGA (curval, VGA_latch[0]);
            curval = (~VGA_GC[8] & curval) | (VGA_GC[8] & VGA_latch[0]);
            VRAM[addr32] = curval;
          }
        if (VGA_SC[2] & 2) {
            if (VGA_GC[1] & 2)
              if (VGA_GC[0] & 2) curval = 255;
              else curval = 0;
            else curval = value;
            logicVGA (curval, VGA_latch[1]);
            curval = (~VGA_GC[8] & curval) | (VGA_GC[8] & VGA_latch[1]);
            VRAM[addr32+planesize] = curval;
          }
        if (VGA_SC[2] & 4) {
            if (VGA_GC[1] & 4)
              if (VGA_GC[0] & 4) curval = 255;
              else curval = 0;
            else curval = value;
            logicVGA (curval, VGA_latch[2]);
            curval = (~VGA_GC[8] & curval) | (VGA_GC[8] & VGA_latch[2]);
            VRAM[addr32+planesize*2] = curval;
          }
        if (VGA_SC[2] & 8) {
            if (VGA_GC[1] & 8)
              if (VGA_GC[0] & 8) curval = 255;
              else curval = 0;
            else curval = value;
            logicVGA (curval, VGA_latch[3]);
            curval = (~VGA_GC[8] & curval) | (VGA_GC[8] & VGA_latch[3]);
            VRAM[addr32+planesize*3] = curval;
          }
        break;
      case 1:
        if (VGA_SC[2] & 1) VRAM[addr32] = VGA_latch[0];
        if (VGA_SC[2] & 2) VRAM[addr32+planesize] = VGA_latch[1];
        if (VGA_SC[2] & 4) VRAM[addr32+planesize*2] = VGA_latch[2];
        if (VGA_SC[2] & 8) VRAM[addr32+planesize*3] = VGA_latch[3];
        break;
      case 2:
        if (VGA_SC[2] & 1) {
            if (VGA_GC[1] & 1)
              if (value & 1) curval = 255;
              else curval = 0;
            else curval = value;
            logicVGA (curval, VGA_latch[0]);
            curval = (~VGA_GC[8] & curval) | (VGA_GC[8] & VGA_latch[0]);
            VRAM[addr32] = curval;
          }
        if (VGA_SC[2] & 2) {
            if (VGA_GC[1] & 2)
              if (value & 2) curval = 255;
              else curval = 0;
            else curval = value;
            logicVGA (curval, VGA_latch[1]);
            curval = (~VGA_GC[8] & curval) | (VGA_GC[8] & VGA_latch[1]);
            VRAM[addr32+planesize] = curval;
          }
        if (VGA_SC[2] & 4) {
            if (VGA_GC[1] & 4)
              if (value & 4) curval = 255;
              else curval = 0;
            else curval = value;
            logicVGA (curval, VGA_latch[2]);
            curval = (~VGA_GC[8] & curval) | (VGA_GC[8] & VGA_latch[2]);
            VRAM[addr32+planesize*2] = curval;
          }
        if (VGA_SC[2] & 8) {
            if (VGA_GC[1] & 8)
              if (value & 8) curval = 255;
              else curval = 0;
            else curval = value;
            logicVGA (curval, VGA_latch[3]);
            curval = (~VGA_GC[8] & curval) | (VGA_GC[8] & VGA_latch[3]);
            VRAM[addr32+planesize*3] = curval;
          }
        break;
      case 3:
        tempand = value & VGA_GC[8];
        shiftVGA (value);
        if (VGA_SC[2] & 1) {
            if (VGA_GC[0] & 1) curval = 255;
            else curval = 0;
            //logicVGA (curval, VGA_latch[0]);
            curval = (~tempand & curval) | (tempand & VGA_latch[0]);
            VRAM[addr32] = curval;
          }
        if (VGA_SC[2] & 2) {
            if (VGA_GC[0] & 2) curval = 255;
            else curval = 0;
            //logicVGA (curval, VGA_latch[1]);
            curval = (~tempand & curval) | (tempand & VGA_latch[1]);
            VRAM[addr32+planesize] = curval;
          }
        if (VGA_SC[2] & 4) {
            if (VGA_GC[0] & 4) curval = 255;
            else curval = 0;
            //logicVGA (curval, VGA_latch[2]);
            curval = (~tempand & curval) | (tempand & VGA_latch[2]);
            VRAM[addr32+planesize*2] = curval;
          }
        if (VGA_SC[2] & 8) {
            if (VGA_GC[0] & 8) curval = 255;
            else curval = 0;
            //logicVGA (curval, VGA_latch[3]);
            curval = (~tempand & curval) | (tempand & VGA_latch[3]);
            VRAM[addr32+planesize*3] = curval;
          }
        break;
    }
}

uint8_t readmode;
uint32_t readmap;
uint8_t readVGA (uint32_t addr32) {
  uint32_t planesize;
  planesize = 0x10000;

  VGA_latch[0] = VRAM[addr32];
  VGA_latch[1] = VRAM[addr32+planesize];
  VGA_latch[2] = VRAM[addr32+planesize*2];
  VGA_latch[3] = VRAM[addr32+planesize*3];
  if (VGA_SC[2] & 1) return (VRAM[addr32]);
  if (VGA_SC[2] & 2) return (VRAM[addr32+planesize]);
  if (VGA_SC[2] & 4) return (VRAM[addr32+planesize*2]);
  if (VGA_SC[2] & 8) return (VRAM[addr32+planesize*3]);
  return (0); //this won't be reached, but without it some compilers give a warning
}

void initVideoPorts() {
  set_portout_redirector (0x3B0, 0x3DA, (void*)&outVGA);
  set_portin_redirector (0x3B0, 0x3DA, (void*)&inVGA);
}

//████████████████████████████████████████
//text_render.c

#define SYMBOL_W  8
#define SYMBOL_H  16

extern uint32_t videobase, textbase, x, y;
extern uint16_t cursx, cursy, cols, rows, vgapage, cursorposition, cursorvisible;

volatile uint32_t render_duration = 0;

void old_text_mode_render(void);
void draw_symbol(uint8_t symb_code, uint8_t xpos, uint8_t ypos, uint32_t back_color, uint32_t pixel_color,  uint8_t divx, uint8_t divy);

void text_mode_render(void)
{
  uint32_t start_time = millis___();
  
  //all text  modes are N x 25 symbols
  uint8_t char_x, char_y;
  uint8_t divx, divy;
  uint32_t pointer_offset = 0;
  uint32_t vidptr, back_color, pixel_color, vgapage;
  uint8_t curchar;
  
  vgapage = ( (uint32_t) VGA_CRTC[0xC]<<8) + (uint32_t) VGA_CRTC[0xD];
  if ((portram[0x3D8]==9) && (portram[0x3D4]==9)) {
    pointer_offset = vgapage + videobase;
    divy = 4;
  } else {
    pointer_offset = videobase;
    divy = 16;
  }
  if (cols==80) divx = 1;
  else divx = 2;

  for (char_y = 0; char_y < rows; char_y++ )  { 
    for (char_x = 0; char_x < cols; char_x++) {
      vidptr = pointer_offset + char_y*cols*2 + char_x*2;
      curchar = RAM[vidptr];
     
      //find color of the symbol
      if (vidcolor) {
        if (portram[0x3D8] & 128) 
          back_color = palettecga[ (RAM[vidptr+1]/16) & 7];
        else 
          back_color = palettecga[RAM[vidptr+1]/16]; //high intensity background
        
        pixel_color = palettecga[RAM[vidptr+1]&15];
      } else {
        if ( (RAM[vidptr+1] & 0x70) ) 
        {
          back_color = palettecga[7];
          pixel_color = palettecga[0];
        }
        else 
        {
          back_color = palettecga[0];
          pixel_color = palettecga[7];
        }
      }
      //draw_symbol(curchar, char_x, char_y, back_color, pixel_color,  divx, divy);
      
      if (char_y<25 && char_x<60) {
        //256 colors avaliable:
        uint32_t color =(((pixel_color&0b111000000000000000000000 )>>21) | ((pixel_color&0b000000000001100000000000000)>>8 ) | ((pixel_color&0b0000000000000000011100000)>>2)); // B G R
        uint32_t bg_color =(((back_color&0b111000000000000000000000 )>>21) | ((back_color&0b000000000001100000000000000)>>8 ) | ((back_color&0b0000000000000000011100000)>>2)); 
        draw_char_xy(char_x*4, char_y*8, curchar, DisplayFontSet, color,bg_color);
      }
    }   
  }
  render_duration = millis___() - start_time;
}

/*
void draw_symbol(uint8_t symb_code, uint8_t xpos, uint8_t ypos, uint32_t back_color, uint32_t pixel_color,  uint8_t divx, uint8_t divy)
{
  uint16_t x, y;
  uint16_t x_start = xpos*SYMBOL_W;
  uint16_t x_stop  = x_start + SYMBOL_W;
  
  uint16_t y_start = ypos*SYMBOL_H;
  uint16_t y_stop  = y_start + SYMBOL_H;
  uint8_t pixel_state;
  uint16_t font_pos = 0;
  
  if((divy == 16) && (divx == 1))
  {
    font_pos = symb_code*128;
    for (y=y_start; y < y_stop; y++)
      for (x=x_start; x < x_stop; x++)
      {
        pixel_state = fontcga[font_pos];
        if (!pixel_state)//off
          prestretch[y][x] = back_color;
        else
          prestretch[y][x] = pixel_color;
        font_pos++;
      }
  } else {
    
    for (y=y_start; y < y_stop; y++)
      for (x=x_start; x < x_stop; x++)
      {
        pixel_state = fontcga[symb_code*128 + (y %  divy) * 8 + ( (x / divx) % 8) ];
        if (!pixel_state)//off
          prestretch[y][x] = back_color;
        else
          prestretch[y][x] = pixel_color;
      }
  } 
}
*/
//████████████████████████████████████████

//████████████████████████████████████████
// render.c: functions for SDL initialization, as well as video scaling/rendering.
//   it is a bit messy. i plan to rework much of this in the future. i am also
//   going to add hardware accelerated scaling soon. */

uint32_t *scalemap = NULL;

extern uint16_t cursx, cursy, cols, rows, vgapage, cursorposition, cursorvisible;
extern uint8_t updatedscreen, clocksafe, port3da, port6 /*, portout16_*/;
extern uint32_t videobase, textbase, x, y;
extern uint32_t usefullscreen, usegrabmode;
extern uint16_t vtotal;

uint64_t totalframes = 0;
uint32_t framedelay = 20;
uint8_t scrmodechange = 0, noscale = 0, renderbenchmark = 0, doaudio = 0;

void fake86_draw (void);

uint8_t initscreen (uint8_t *ver) {
  initcga(); 
  return (1);
}

void doscrmodechange(void) 
{
  if (scrmodechange) {
    screenmemory_fillscreen(0); //black color
  }
  scrmodechange = 0;
}

void fake86_draw (void) { 
  
  uint32_t planemode, vgapage, chary, charx, vidptr, divx, divy, curchar, curpixel, usepal, intensity, blockw, curheight, x1, y1;      
  uint32_t color;//uint32_t earlier
      
  switch (vidmode) {
    case 0:
    case 1:
    case 2: //text modes
    case 3:
    case 7:
    case 0x82:
      {
        nw = 640;
        nh = 400;
        text_mode_render();
        break;
      }                        
    case 4:
    case 5: //4 colors
      {
        nw = 320;
        nh = 200;
        usepal = (portram[0x3D9]>>5) & 1;
        intensity = ( (portram[0x3D9]>>4) & 1) << 3;
        for (y=0; y<200; y++) {
          for (x=0; x<320; x++) {
            charx = x;
            chary = y;
            vidptr = videobase + ( (chary>>1) * 80) + ( (chary & 1) * 8192) + (charx >> 2);
            curpixel = RAM[vidptr];
            switch (charx & 3)
            {
            case 3:
              curpixel = curpixel & 3;
              break;
            case 2:
              curpixel = (curpixel>>2) & 3;
              break;
            case 1:
              curpixel = (curpixel>>4) & 3;
              break;
            case 0:
              curpixel = (curpixel>>6) & 3;
              break;
            }//end of switch
            if (vidmode==4) {
              curpixel = curpixel * 2 + usepal + intensity;
              if (curpixel == (usepal + intensity) )  curpixel = cgabg;
              color = palettecga[curpixel];
              ///prestretch[y][x] = color;
//................................................................................
              uint32_t c = color;          
              uint32_t color =(((c&0b111000000000000000000000 )>>21) | ((c&0b000000000001100000000000000)>>8) | ((c&0b0000000000000000011100000)>>2)); 
              SCREENMEMORY_LINE[ (y)+ 20][((x)*3/4)]=color;
//................................................................................              
            } else {
              curpixel = curpixel * 63;
              color = palettecga[curpixel];
//................................................................................              
              uint32_t c = color;
              //c |= (fb_ram[(Ytmp*160+Xtmp)*2+1]<<16);
              uint32_t color =(((c&0b111000000000000000000000 )>>21) | ((c&0b000000000001100000000000000)>>8) | ((c&0b0000000000000000011100000)>>2)); 
              SCREENMEMORY_LINE[ (y)+ 20][((x)*3/4)]=color;
//................................................................................                          
            }
          }//for x
        }//for y
        break;
      }//mode 5
    case 6: //2 colors
      {
        nw = 640;
        nh = 200;
        for (y=0; y<200; y++) 
        {
          for (x=0; x<640; x++) 
          {
            charx = x;
            chary = y;
            vidptr = videobase + ( (chary>>1) * 80) + ( (chary&1) * 8192) + (charx>>3);
            curpixel = (RAM[vidptr]>> (7- (charx&7) ) ) &1;
            color = palettecga[curpixel*15];
            //prestretch[y][x] = color;
//................................................................................
            uint32_t c = color;
            uint32_t color =(((c&0b111000000000000000000000 )>>21) | ((c&0b000000000001100000000000000)>>8) | ((c&0b0000000000000000011100000)>>2)); 
            SCREENMEMORY_LINE[ (y)+ 20][((x/2)*3/4)]=color;
//................................................................................
          }
        }
        break;
      }
                                
    case 0x8: //160x200 16-color (PCjr)
        nw = 640; //fix this
        nh = 400; //part later
        for (y=0; y<400; y++)
          for (x=0; x<640; x++) {
              vidptr = 0xB8000 + (y>>2) *80 + (x>>3) + ( (y>>1) &1) *8192;
              if ( ( (x>>1) &1) ==0) color = palettecga[RAM[vidptr] >> 4];
              else color = palettecga[RAM[vidptr] & 15];
              ///prestretch[y][x] = color;
//................................................................................
              uint32_t c = color;
              //c |= (fb_ram[(Ytmp*160+Xtmp)*2+1]<<16);
              uint32_t color =(((c&0b111000000000000000000000 )>>21) | ((c&0b000000000001100000000000000)>>8) | ((c&0b0000000000000000011100000)>>2)); 
              SCREENMEMORY_LINE[ (y/2)+ 20][((x/2)*3/4)]=color;
//................................................................................
          }
      break;
    case 0x9: //320x200 16-color (Tandy/PCjr)
      nw = 640; //fix this
      nh = 400; //part later
      for (y=0; y<400; y++)
        for (x=0; x<640; x++) {
          vidptr = 0xB8000 + (y>>3) *160 + (x>>2) + ( (y>>1) &3) *8192;
          if ( ( (x>>1) &1) ==0) color = palettecga[RAM[vidptr] >> 4];
          else color = palettecga[RAM[vidptr] & 15];
          //prestretch[y][x] = color;
//................................................................................
            uint32_t c = color;
            //c |= (fb_ram[(Ytmp*160+Xtmp)*2+1]<<16);
            uint32_t color =(((c&0b111000000000000000000000 )>>21) | ((c&0b000000000001100000000000000)>>8) | ((c&0b0000000000000000011100000)>>2)); 
            SCREENMEMORY_LINE[ (y/2)+ 20][((x/2)*3/4)]=color;
//................................................................................            
        }
      break;
    case 0xD:
    case 0xE:
      
      //TESTED ON GAME COMMANDER KEEN1:
        
      nw = 360;
      nh = 240;
      for (y=0; y<240; y++)
        for (x=20; x<340; x++) { 
          vidptr = y*48 + x/8; /// 1 iba    pri X<360 Y<240
           
          color  = (VRAM[vidptr] >> (~x & 7) ) & 1;
          color |= ( (VRAM[vidptr+0x10000] >> (~x & 7) ) & 1) << 1;
          color |= ( (VRAM[vidptr+0x20000] >> (~x & 7) ) & 1) << 2;
          color |= ( (VRAM[vidptr+0x30000] >> (~x & 7) ) & 1) << 3;
          //prestretch[y][x] = palettevga[color];             
//................................................................................
          uint32_t c = palettevga[color];                                        
          //c |= (fb_ram[(Ytmp*160+Xtmp)*2+1]<<16);
          uint32_t color =(((c&0b111000000000000000000000 )>>21) | ((c&0b000000000001100000000000000)>>8) | ((c&0b0000000000000000011100000)>>2)); 
          SCREENMEMORY_LINE[ (y)][((x*2)/3)+1]=color;
          SCREENMEMORY_LINE[ (y)][((x*2)/3)]=color;
//................................................................................
        }
      break;
    case 0x10:
      nw = 640;
      nh = 350;
      for (y=0; y<nh; y++)
        for (x=0; x<nw; x++) {
          vidptr = y*80 + (x>>3);
          x1 = 7 - (x & 7);
          color = (VRAM[vidptr] >> x1) & 1;
          color += ( ( (VRAM[0x10000 + vidptr] >> x1) & 1) << 1);
          color += ( ( (VRAM[0x20000 + vidptr] >> x1) & 1) << 2);
          color += ( ( (VRAM[0x30000 + vidptr] >> x1) & 1) << 3);
          color = palettevga[color];
          //prestretch[y][x] = color;
//................................................................................
          uint32_t c = color;
          //c |= (fb_ram[(Ytmp*160+Xtmp)*2+1]<<16);
          uint32_t color =(((c&0b111000000000000000000000 )>>21) | ((c&0b000000000001100000000000000)>>8) | ((c&0b0000000000000000011100000)>>2)); 
          SCREENMEMORY_LINE[ (y/2)+ 20][((x/2)*3/4)]=color;
//................................................................................
        }
      break;
    case 0x12:
      nw = 640;
      nh = 480;
      vgapage = ( (uint32_t) VGA_CRTC[0xC]<<8) + (uint32_t) VGA_CRTC[0xD];
      for (y=0; y<nh; y++)
        for (x=0; x<nw; x++) {
            vidptr = y*80 + (x/8);
            color  = (VRAM[vidptr] >> (~x & 7) ) & 1;
            color |= ( (VRAM[vidptr+0x10000] >> (~x & 7) ) & 1) << 1;
            color |= ( (VRAM[vidptr+0x20000] >> (~x & 7) ) & 1) << 2;
            color |= ( (VRAM[vidptr+0x30000] >> (~x & 7) ) & 1) << 3;
            //prestretch[y][x] = palettevga[color];
//................................................................................
            uint32_t c = palettevga[color];
            //c |= (fb_ram[(Ytmp*160+Xtmp)*2+1]<<16);
            uint32_t color =(((c&0b111000000000000000000000 )>>21) | ((c&0b000000000001100000000000000)>>8) | ((c&0b0000000000000000011100000)>>2));            
            SCREENMEMORY_LINE[ (y/2)+ 20][((x/2)*3/4)]=color;
//................................................................................
        }
      break;
    case 0x13:
      {
        if (vtotal == 11) { //ugly hack to show Flashback at the proper resolution
          nw = 256;
          nh = 224;
        } else {
          nw = 320;
          nh = 200;
        }
        if (VGA_SC[4] & 6) planemode = 1;
        else planemode = 0;

        vgapage = ( (uint32_t) VGA_CRTC[0xC]<<8) + (uint32_t) VGA_CRTC[0xD];
        for (y=0; y<nh; y++) { 
          for (x=0; x<nw; x++) {
            if (!planemode) 
              color = palettevga[RAM[videobase + y*nw + x]];
            else {
              vidptr = y*nw + x;
              vidptr = vidptr/4 + (x & 3) *0x10000;
              vidptr = vidptr + vgapage - (VGA_ATTR[0x13] & 15);
              color = palettevga[VRAM[vidptr]];
            }
            //prestretch[y][x] = color;
//................................................................................
            uint32_t c = color;
            //c |= (fb_ram[(Ytmp*160+Xtmp)*2+1]<<16);
            uint32_t color =(((c&0b111000000000000000000000 )>>21) | ((c&0b000000000001100000000000000)>>8) | ((c&0b0000000000000000011100000)>>2)); 
            SCREENMEMORY_LINE[ (y)+ 20][((x)*3/4)]=color;
//................................................................................
          }
        }
        break;
      }                
  }   
}
//████████████████████████████████████████


//████████████████████████████████████████
// timing.c: critical functions to provide accurate timing for the
// system timer interrupt, and to generate new audio output samples. 

extern struct i8253_s i8253;
extern void doirq (uint8_t irqnum);
extern void tickaudio();
extern void tickssource();
extern void tickadlib();
extern void tickBlaster();

uint64_t hostfreq = 1000000, lasttick = 0, curtick = 0, tickgap, i8253tickgap, lasti8253tick, scanlinetiming, lastscanlinetick, curscanline = 0;
uint64_t sampleticks, lastsampletick, ssourceticks, lastssourcetick, adlibticks, lastadlibtick, lastblastertick, gensamplerate;
uint16_t pit0counter = 65535;

extern uint64_t totalexec;
extern uint32_t speed;
extern uint8_t port3da, doaudio, slowsystem;


void inittiming(void) 
{
  hostfreq = 1000000; //default: 1000000
  curtick = (uint64_t)micros___();
        
  lasti8253tick = lastblastertick = lastadlibtick = lastssourcetick = lastsampletick = lastscanlinetick = lasttick = curtick;
  scanlinetiming = HERTZ___(hostfreq / 31500);
  ssourceticks = HERTZ___(hostfreq / 8000);
  adlibticks = HERTZ___(hostfreq / 48000);
  //if (doaudio) sampleticks = hostfreq / gensamplerate; //todo
  //else sampleticks = -1;
  sampleticks = -1;
  i8253tickgap = HERTZ___(hostfreq / 119318);
  scanlinetiming=0; ///NCX: significantly increase speed of Graphics... 
  
  //i8253tickgap=0;
}

void timing(void) 
{
  uint8_t i8253chan;
  curtick = (uint64_t)micros___() ;
        
  if (curtick >= (lastscanlinetick + scanlinetiming) ) {
    curscanline = (curscanline + 1) % 525;
    if (curscanline > 479) port3da = 8;
    else port3da = 0;
    if (curscanline & 1) port3da |= 1;
    pit0counter++;
    lastscanlinetick = curtick;
  }
  if (i8253.active[0]) { //timer interrupt channel on i8253
    if (curtick >= (lasttick + tickgap) ) {
      lasttick = curtick;
      doirq (0);
    }
  }

  if (curtick >= (lasti8253tick + i8253tickgap) ) {
    for (i8253chan=0; i8253chan<3; i8253chan++) {
      if (i8253.active[i8253chan]) {
        if (i8253.counter[i8253chan] < 10) i8253.counter[i8253chan] = i8253.chandata[i8253chan];
        i8253.counter[i8253chan] -= 10;

      }
    }
    lasti8253tick = curtick;
  }

#if !FAKE86_NO_AUDIO

  if (curtick >= (lastssourcetick + ssourceticks) ) {
    tickssource();
    lastssourcetick = curtick - (curtick - (lastssourcetick + ssourceticks) );
  }

  if (blaster->samplerate > 0) {
    if (curtick >= (lastblastertick + blaster->sampleticks) ) {
      tickBlaster();
      lastblastertick = curtick - (curtick - (lastblastertick + blaster->sampleticks) );
    }
  }  
  if (curtick >= (lastsampletick + sampleticks) ) {
    tickaudio();
    if (slowsystem) {
      tickaudio();
      tickaudio();
      tickaudio();
    }
    lastsampletick = curtick - (curtick - (lastsampletick + sampleticks) );
  }

  if (curtick >= (lastadlibtick + adlibticks) ) {
    tickadlib();
    lastadlibtick = curtick - (curtick - (lastadlibtick + adlibticks) );
  }
  #endif      
} 
//████████████████████████████████████████
// ports.c: functions to handle port I/O from the CPU module, as well
//   as functions for emulated hardware components to register their
//   read/write callback functions across the port address range. 

extern uint8_t speakerenabled;

void (*do_callback_write) (uint16_t portnum, uint8_t value) = NULL;
uint8_t (*do_callback_read) (uint16_t portnum) = NULL;

void portout (uint16_t portnum, uint8_t value) {
  if (portnum > PORTS_MEM_SIZE) return;
  
  portram[portnum] = value;
  //printf("portout(0x%X, 0x%02X);\n", portnum, value);
  switch (portnum) {
  case 0x61:
    if ( (value & 3) == 3) speakerenabled = 1;
    else speakerenabled = 0;
    return;
  }
  do_callback_write = (void (*) (uint16_t portnum, uint8_t value) ) portcallback_->portout_callback[portnum];
  if (do_callback_write != (void *) 0) (*do_callback_write) (portnum, value);
}

uint8_t portin (uint16_t portnum)  {
  if (portnum > PORTS_MEM_SIZE) return (0xFF);
  
  //printf("portin(0x%X);\n", portnum);
  switch (portnum) {
  case 0x62:
    return (0x00);
  case 0x60:
  case 0x61:
  case 0x63:
  case 0x64:
    return (portram[portnum]);
  }
  do_callback_read = (uint8_t (*) (uint16_t portnum) ) portcallback_->portin_callback[portnum];
  if (do_callback_read != (void *) 0) return ( (*do_callback_read) (portnum) );
  return (0xFF);
}

void portout16 (uint16_t portnum, uint16_t value) {
  portout (portnum, (uint8_t) value);
  portout (portnum + 1, (uint8_t) (value >> 8) );
}

uint16_t portin16 (uint16_t portnum) {
  uint16_t ret;
  
  ret = (uint16_t) portin (portnum);
  ret |= (uint16_t) portin (portnum+1) << 8;
  return (ret);
}

void set_portout_redirector (uint16_t startport, uint16_t endport, void *callback) {
  uint16_t i;
  for (i=startport; i<=endport; i++) {
    portcallback_->portout_callback[i] = callback;
  }
}

void set_portin_redirector (uint16_t startport, uint16_t endport, void *callback) {
  uint16_t i;
  for (i=startport; i<=endport; i++) {
      portcallback_->portin_callback[i] = callback;
    }
}

//████████████████████████████████████████
//adlib.c
extern void set_portout_redirector (uint16_t startport, uint16_t endport, void *callback);
extern void set_portin_redirector (uint16_t startport, uint16_t endport, void *callback);

extern int32_t usesamplerate;

double samprateadjust = 1.0;
uint8_t optable[0x16] = { 0, 0, 0, 1, 1, 1, 255, 255, 0, 0, 0, 1, 1, 1, 255, 255, 0, 0, 0, 1, 1, 1 };
uint16_t adlibregmem[0xFF], adlibaddr = 0;

const int8_t waveform[4][64] = {
  { 1, 8, 13, 20, 26, 31, 37, 41, 47, 49, 54, 58, 58, 62, 63, 63, 64, 63, 62, 61, 58, 55, 52, 47, 45, 38, 34, 27, 23, 17, 10, 4,-2,-8,-15,-21,-26,-34,-36,-42,-48,-51,-54,-59,-60,-62,-64,-65,-65,-63,-64,-61,-59,-56,-53,-48,-46,-39,-36,-28,-24,-17,-11,-6 },
  { 1, 8, 13, 20, 25, 32, 36, 42, 46, 50, 54, 57, 60, 61, 62, 64, 63, 65, 61, 61, 58, 55, 51, 49, 44, 38, 34, 28, 23, 16, 11, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 8, 13, 21, 25, 31, 36, 43, 45, 50, 54, 57, 59, 62, 63, 63, 63, 64, 63, 59, 59, 55, 52, 48, 44, 38, 34, 28, 23, 16, 10, 4, 2, 7, 14, 20, 26, 31, 36, 42, 45, 51, 54, 56, 60, 62, 62, 63, 65, 63, 62, 60, 58, 55, 52, 48, 44, 38, 34, 28, 23, 17, 10, 3 },
  { 1, 8, 13, 20, 26, 31, 36, 42, 46, 51, 53, 57, 60, 62, 61, 66, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 13, 21, 25, 32, 36, 41, 47, 50, 54, 56, 60, 62, 61, 67, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

const int8_t oplwave[4][256] = {
  {
    0, 1, 3, 4, 6, 7, 9, 11, 12, 14, 15, 17, 18, 20, 22, 23, 24, 26, 27, 29, 30, 31, 33, 34, 36, 37, 38, 40, 40, 42, 43, 44, 46, 46, 48, 49, 50, 51, 51, 53,
    53, 54, 55, 56, 57, 57, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 63, 64, 64, 64, 116, 116, 116, 116, 116, 116, 116, 116, 116, 64, 64, 64, 63, 63, 63, 62, 62, 61, 61, 60,
    59, 59, 58, 57, 57, 56, 55, 54, 53, 53, 51, 51, 50, 49, 48, 46, 46, 44, 43, 42, 40, 40, 38, 37, 36, 34, 33, 31, 30, 29, 27, 26, 24, 23, 22, 20, 18, 17, 15, 14,
    12, 11, 9, 7, 6, 4, 3, 1, 0, -1, -3, -4, -6, -7, -9, -11, -12, -14, -15, -17, -18, -20, -22, -23, -24, -26, -27, -29, -30, -31, -33, -34, -36, -37, -38, -40, -40, -42, -43, -44,
    -46, -46, -48, -49, -50, -51, -51, -53, -53, -54, -55, -56, -57, -57, -58, -59, -59, -60, -61, -61, -62, -62, -63, -63, -63, -64, -64, -64, -116, -116, -116, -116, -116, -116, -116, -116, -116, -64, -64, -64,
    -63, -63, -63, -62, -62, -61, -61, -60, -59, -59, -58, -57, -57, -56, -55, -54, -53, -53, -51, -51, -50, -49, -48, -46, -46, -44, -43, -42, -40, -40, -38, -37, -36, -34, -33, -31, -30, -29, -27, -26,
    -24, -23, -22, -20, -18, -17, -15, -14, -12, -11, -9, -7, -6, -4, -3, -1
  },

  {
    0, 1, 3, 4, 6, 7, 9, 11, 12, 14, 15, 17, 18, 20, 22, 23, 24, 26, 27, 29,30, 31, 33, 34, 36, 37, 38, 40, 40, 42, 43, 44, 46, 46, 48, 49, 50, 51, 51, 53,
    53, 54, 55, 56, 57, 57, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 63, 64, 64, 64, 116, 116, 116, 116, 116, 116, 116, 116, 116, 64, 64, 64, 63, 63, 63, 62, 62, 61, 61, 60,
    59, 59, 58, 57, 57, 56, 55, 54, 53, 53, 51, 51, 50, 49, 48, 46, 46, 44, 43, 42, 40, 40, 38, 37, 36, 34, 33, 31, 30, 29, 27, 26, 24, 23, 22, 20, 18, 17, 15, 14,
    12, 11, 9, 7, 6, 4, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },


  {
    0, 1, 3, 4, 6, 7, 9, 11, 12, 14, 15, 17, 18, 20, 22, 23, 24, 26, 27, 29, 30, 31, 33, 34, 36, 37, 38, 40, 40, 42, 43, 44, 46, 46, 48, 49, 50, 51, 51, 53,
    53, 54, 55, 56, 57, 57, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 63, 64, 64, 64, 116, 116, 116, 116, 116, 116, 116, 116, 116, 64, 64, 64, 63, 63, 63, 62, 62, 61, 61, 60,
    59, 59, 58, 57, 57, 56, 55, 54, 53, 53, 51, 51, 50, 49, 48, 46, 46, 44, 43, 42, 40, 40, 38, 37, 36, 34, 33, 31, 30, 29, 27, 26, 24, 23, 22, 20, 18, 17, 15, 14,
    12, 11, 9, 7, 6, 4, 3, 1, 0, 1, 3, 4, 6, 7, 9, 11, 12, 14, 15, 17, 18, 20, 22, 23, 24, 26, 27, 29, 30, 31, 33, 34, 36, 37, 38, 40, 40, 42, 43, 44,
    46, 46, 48, 49, 50, 51, 51, 53, 53, 54, 55, 56, 57, 57, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 63, 64, 64, 64, 116, 116, 116, 116, 116, 116, 116, 116, 116, 64, 64, 64,
    63, 63, 63, 62, 62, 61, 61, 60, 59, 59, 58, 57, 57, 56, 55, 54, 53, 53, 51, 51, 50, 49, 48, 46, 46, 44, 43, 42, 40, 40, 38, 37, 36, 34, 33, 31, 30, 29, 27, 26,
    24, 23, 22, 20, 18, 17, 15, 14, 12, 11, 9, 7, 6, 4, 3, 1
  },


  {
    0, 1, 3, 4, 6, 7, 9, 11, 12, 14, 15, 17, 18, 20, 22, 23, 24, 26, 27, 29, 30, 31, 33, 34, 36, 37, 38, 40, 40, 42, 43, 44, 46, 46, 48, 49, 50, 51, 51, 53,
    53, 54, 55, 56, 57, 57, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 63, 64, 64, 64, 116, 116, 116, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 4, 6, 7, 9, 11, 12, 14, 15, 17, 18, 20, 22, 23, 24, 26, 27, 29, 30, 31, 33, 34, 36, 37, 38, 40, 40, 42, 43, 44,
    46, 46, 48, 49, 50, 51, 51, 53, 53, 54, 55, 56, 57, 57, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 63, 64, 64, 64, 116, 116, 116, 116, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  }

};

uint8_t oplstep[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

struct structadlibop {
  uint8_t wave;
} adlibop[9][2];

struct structadlibchan {
  uint16_t freq;
  double convfreq;
  uint8_t keyon;
  uint16_t octave;
  uint8_t wavesel;
} adlibch[9];

const double attacktable[16] = { 1.0003, 1.00025, 1.0002, 1.00015, 1.0001, 1.00009, 1.00008, 1.00007, 1.00006, 1.00005, 1.00004, 1.00003, 1.00002, 1.00001, 1.000005 }; //1.003, 1.05, 1.01, 1.015, 1.02, 1.025, 1.03, 1.035, 1.04, 1.045, 1.05, 1.055, 1.06, 1.065, 1.07, 1.075 };
const double decaytable[16] = { 0.99999, 0.999985, 0.99998, 0.999975, 0.99997, 0.999965, 0.99996, 0.999955, 0.99995, 0.999945, 0.99994, 0.999935, 0.99994, 0.999925, 0.99992, 0.99991 };
double adlibenv[9], adlibdecay[9], adlibattack[9];
uint8_t adlibdidattack[9], adlibpercussion = 0, adlibstatus = 0;

uint16_t adlibport = 0x388;

void outadlib (uint16_t portnum, uint8_t value) {
  if (portnum==adlibport) {
      adlibaddr = value;
      return;
    }
  portnum = adlibaddr;
  adlibregmem[portnum] = value;
  switch (portnum) {
      case 4: //timer control
        if (value&0x80) {
            adlibstatus = 0;
            adlibregmem[4] = 0;
          }
        break;
      case 0xBD:
        if (value & 0x10) adlibpercussion = 1;
        else adlibpercussion = 0;
        break;
    }
  if ( (portnum >= 0x60) && (portnum <= 0x75) ) { //attack/decay
      portnum &= 15;
      adlibattack[portnum] = attacktable[15- (value>>4) ]*1.006;
      adlibdecay[portnum] = decaytable[value&15];
    }
  else if ( (portnum >= 0xA0) && (portnum <= 0xB8) ) { //octave, freq, key on
      portnum &= 15;
      if (!adlibch[portnum].keyon && ( (adlibregmem[0xB0+portnum]>>5) &1) ) {
          adlibdidattack[portnum] = 0;
          adlibenv[portnum] = 0.0025;
        }
      adlibch[portnum].freq = adlibregmem[0xA0+portnum] | ( (adlibregmem[0xB0+portnum]&3) <<8);
      adlibch[portnum].convfreq = ( (double) adlibch[portnum].freq * 0.7626459);
      adlibch[portnum].keyon = (adlibregmem[0xB0+portnum]>>5) &1;
      adlibch[portnum].octave = (adlibregmem[0xB0+portnum]>>2) &7;
    }
  else if ( (portnum >= 0xE0) && (portnum <= 0xF5) ) { //waveform select
      portnum &= 15;
      if (portnum<9) adlibch[portnum].wavesel = value&3;
    }
}

uint8_t inadlib (uint16_t portnum) {
  if (!adlibregmem[4]) adlibstatus = 0;
  else adlibstatus = 0x80;
  adlibstatus = adlibstatus + (adlibregmem[4]&1) *0x40 + (adlibregmem[4]&2) *0x10;
  return (adlibstatus);
}

uint16_t adlibfreq (uint8_t chan) {
  //uint8_t downoct[4] = { 3, 2, 1, 0 };
  //uint8_t upoct[3] = { 1, 2, 3 };
  uint16_t tmpfreq;
  if (!adlibch[chan].keyon) return (0);
  tmpfreq = (uint16_t) adlibch[chan].convfreq;
  //if (adlibch[chan].octave<4) tmpfreq = tmpfreq>>1;
  //if (adlibch[chan].octave>4) tmpfreq = tmpfreq<<1;
  switch (adlibch[chan].octave) {
      case 0:
        tmpfreq = tmpfreq >> 4;
        break;
      case 1:
        tmpfreq = tmpfreq >> 3;
        break;
      case 2:
        tmpfreq = tmpfreq >> 2;
        break;
      case 3:
        tmpfreq = tmpfreq >> 1;
        break;
        //case 4: tmpfreq = tmpfreq >> 1; break;
      case 5:
        tmpfreq = tmpfreq << 1;
        break;
      case 6:
        tmpfreq = tmpfreq << 2;
        break;
      case 7:
        tmpfreq = tmpfreq << 3;
    }

  return (tmpfreq);
}

uint64_t fullstep, adlibstep[9];

int32_t adlibsample (uint8_t curchan) {
  int32_t tempsample;
  double tempstep;

  if (adlibpercussion && (curchan>=6) && (curchan<=8) ) return (0);

  fullstep = usesamplerate/adlibfreq (curchan);

  tempsample = (int32_t) oplwave[adlibch[curchan].wavesel][ (uint8_t) ( (double) adlibstep[curchan]/ ( (double) fullstep/ (double) 256) ) ];
  tempstep = adlibenv[curchan];
  if (tempstep>1.0) tempstep = 1;
  tempsample = (int32_t) ( (double) tempsample * tempstep * 2.0);

  adlibstep[curchan]++;
  if (adlibstep[curchan]>fullstep) adlibstep[curchan] = 0;
  return (tempsample);
}

int16_t adlibgensample() {
  uint8_t curchan;
  int16_t adlibaccum;
  adlibaccum = 0;
  for (curchan=0; curchan<9; curchan++) {
      if (adlibfreq (curchan) !=0) {
          adlibaccum += (int16_t) adlibsample (curchan);
        }
    }
  return (adlibaccum);
}

void tickadlib() {
  uint8_t curchan;
  for (curchan=0; curchan<9; curchan++) {
      if (adlibfreq (curchan) !=0) {
          if (adlibdidattack[curchan]) {
              adlibenv[curchan] *= adlibdecay[curchan];
            }
          else {
              adlibenv[curchan] *= adlibattack[curchan];
              if (adlibenv[curchan]>=1.0) adlibdidattack[curchan] = 1;
            }
        }
    }
}

void initadlib (uint16_t baseport) {
  //samprateadjust = (double)44100.0 / usesamplerate;
  set_portout_redirector (baseport, baseport + 1, (void*) &outadlib);
  set_portin_redirector (baseport, baseport + 1, (void*) &inadlib);
} 
//████████████████████████████████████████
extern void set_portout_redirector (uint16_t startport, uint16_t endport, void *callback);
extern void set_portin_redirector (uint16_t startport, uint16_t endport, void *callback);
extern void doirq (uint8_t irqnum);
extern uint8_t read8237 (uint8_t channel);

extern void outadlib (uint16_t portnum, uint8_t value); //on the Sound Blaster Pro, ports (base+0) and (base+1) are for
extern uint8_t inadlib (uint16_t portnum); //the OPL FM music chips, and are also mirrored at (base+8) (base+9)
//as well as 0x388 and 0x389 to remain compatible with the older adlib cards

void bufNewData (uint8_t value) {
  if (blaster->memptr >= sizeof (blaster->mem) ) return;
  blaster->mem[blaster->memptr] = value;
  blaster->memptr++;
}

extern uint64_t hostfreq;
void setsampleticks() {
  if (blaster->samplerate == 0) {
      blaster->sampleticks = 0;
      return;
    }
  blaster->sampleticks = hostfreq / (uint64_t) blaster->samplerate;
}

void cmdBlaster (uint8_t value) {
  if (blaster->waitforarg) {
      switch (blaster->lastcmdval) {
          case 0x10: //direct 8-bit sample output
            blaster->sample = value;
            break;
          case 0x14: //8-bit single block DMA output
          case 0x24:
            if (blaster->waitforarg == 2) {
                blaster->blocksize = (blaster->blocksize & 0xFF00) | (uint32_t) value;
                blaster->waitforarg = 3;
                return;
              }
            else {
                blaster->blocksize = (blaster->blocksize & 0x00FF) | ( (uint32_t) value << 8);
#if DEBUG > 2
                printf ("[NOTICE] Sound Blaster DSP block transfer size set to %u\n", blaster->blocksize);
#endif
                blaster->usingdma = 1;
                blaster->blockstep = 0;
                blaster->useautoinit = 0;
                blaster->paused8 = 0;
                blaster->speakerstate = 1;
              }
            break;
          case 0x40: //set time constant
            blaster->samplerate = (uint16_t) ( (uint32_t) 1000000 / (uint32_t) (256 - (uint32_t) value) );
            setsampleticks();
#if DEBUG > 2
            printf ("[DEBUG] Sound Blaster time constant received, sample rate = %u\n", blaster->samplerate);
#endif
            break;
          case 0x48: //set DSP block transfer size
            if (blaster->waitforarg == 2) {
                blaster->blocksize = (blaster->blocksize & 0xFF00) | (uint32_t) value;
                blaster->waitforarg = 3;
                return;
              }
            else {
                blaster->blocksize = (blaster->blocksize & 0x00FF) | ( (uint32_t) value << 8);
                //if (blaster->blocksize == 0) blaster->blocksize = 65536;
                blaster->blockstep = 0;
#if DEBUG > 2
                printf ("[NOTICE] Sound Blaster DSP block transfer size set to %u\n", blaster->blocksize);
#endif
              }
            break;
          case 0xE0: //DSP identification for Sound Blaster 2.0 and newer (invert each bit and put in read buffer)
            bufNewData (~value);
            break;
          case 0xE4: //DSP write test, put data value into read buffer
            bufNewData (value);
            blaster->lasttestval = value;
            break;
        }
      blaster->waitforarg = 0;
      return;
    }

  switch (value) {
      case 0x10:
      case 0x40:
      case 0xE0:
      case 0xE4:
        blaster->waitforarg = 1;
        break;

      case 0x14: //8-bit single block DMA output
      case 0x24:
      case 0x48:
        blaster->waitforarg = 2;
        break;

      case 0x1C: //8-bit auto-init DMA output
      case 0x2C:
        blaster->usingdma = 1;
        blaster->blockstep = 0;
        blaster->useautoinit = 1;
        blaster->paused8 = 0;
        blaster->speakerstate = 1;
        break;

      case 0xD0: //pause 8-bit DMA I/O
        blaster->paused8 = 1;
      case 0xD1: //speaker output on
        blaster->speakerstate = 1;
        break;
      case 0xD3: //speaker output off
        blaster->speakerstate = 0;
        break;
      case 0xD4: //continue 8-bit DMA I/O
        blaster->paused8 = 0;
        break;
      case 0xD8: //get speaker status
        if (blaster->speakerstate) bufNewData (0xFF);
        else bufNewData (0x00);
        break;
      case 0xDA: //exit 8-bit auto-init DMA I/O mode
        blaster->usingdma = 0;
        break;
      case 0xE1: //get DSP version info
        blaster->memptr = 0;
        bufNewData (blaster->dspmaj);
        bufNewData (blaster->dspmin);
        break;
      case 0xE8: //DSP read test
        blaster->memptr = 0;
        bufNewData (blaster->lasttestval);
        break;
      case 0xF2: //force 8-bit IRQ
        doirq (blaster->sbirq);
        break;
      case 0xF8: //undocumented command, clears in-buffer and inserts a null byte
        blaster->memptr = 0;
        bufNewData (0);
        break;
      default:
#if DEBUG > 0      
        printf ("[NOTICE] Sound Blaster received unhandled command %02Xh\n", value);
#endif        
        break;
    }
}

uint8_t mixer[256], mixerindex = 0;
void outBlaster (uint16_t portnum, uint8_t value) {
#if DEBUG > 2
  printf ("[DEBUG] outBlaster: port %Xh, value %02X\n", portnum, value);
#endif
  portnum &= 0xF;
  switch (portnum) {
      case 0x0:
      case 0x8:
        outadlib (0x388, value);
        break;
      case 0x1:
      case 0x9:
        outadlib (0x389, value);
        break;
      case 0x4: //mixer address port
        mixerindex = value;
        break;
      case 0x5: //mixer data
        mixer[mixerindex] = value;
        break;
      case 0x6: //reset port
        if ( (value == 0x00) && (blaster->lastresetval == 0x01) ) {
            blaster->speakerstate = 0;
            blaster->sample = 128;
            blaster->waitforarg = 0;
            blaster->memptr = 0;
            blaster->usingdma = 0;
            blaster->blocksize = 65535;
            blaster->blockstep = 0;
            bufNewData (0xAA);
            memset (mixer, 0xEE, sizeof (mixer) );
#if DEBUG > 2
            printf ("[DEBUG] Sound Blaster received reset!\n");
#endif
          }
        blaster->lastresetval = value;
        break;
      case 0xC: //write command/data
        cmdBlaster (value);
        if (blaster->waitforarg != 3) blaster->lastcmdval = value;
        break;
    }
}

uint8_t inBlaster (uint16_t portnum) {
  uint8_t ret = 0;
#if DEBUG > 2
  static uint16_t lastread = 0;
#endif
  portnum &= 0xF;
#if DEBUG > 2
  if (lastread != portnum) printf ("[DEBUG] inBlaster: port %Xh, value ", 0x220 + portnum);
#endif
  switch (portnum) {
      case 0x0:
      case 0x8:
        ret = inadlib (0x388);
        break;
      case 0x1:
      case 0x9:
        ret = inadlib (0x389);
        break;
      case 0x5: //mixer data
        ret = mixer[mixerindex];
        break;
      case 0xA: //read data
        if (blaster->memptr == 0) {
            ret = 0;
          }
        else {
            ret = blaster->mem[0];
            memmove (&blaster->mem[0], &blaster->mem[1], sizeof (blaster->mem) - 1);
            blaster->memptr--;
          }
        break;
      case 0xE: //read-buffer status
        if (blaster->memptr > 0) ret = 0x80;
        else ret = 0x00;
        break;
      default:
        ret = 0x00;
    }
#if DEBUG > 2
  if (lastread != portnum) printf ("%02X\n", ret);
  lastread = portnum;
#endif
  return (ret);
}

//FILE *sbout = NULL;
void tickBlaster() {
  if (!blaster->usingdma) return;
  //if (blaster->paused8) {
  //blaster->sample = 128;
  //return;
  //}
  //printf("tickBlaster();\n");
  blaster->sample = read8237 (blaster->sbdma);
  //if (sbout != NULL) fwrite(&blaster->sample, 1, 1, sbout);
  blaster->blockstep++;
  if (blaster->blockstep > blaster->blocksize) {
      doirq (blaster->sbirq);
#if DEBUG > 1
      printf ("[NOTICE] Sound Blaster did IRQ\n");
#endif
      if (blaster->useautoinit) {
          blaster->blockstep = 0;
        }
      else {
          blaster->usingdma = 0;
        }
    }
}

int16_t getBlasterSample() {
  if (blaster->speakerstate == 0) return (0);
  else return ( (int16_t) blaster->sample - 128);
}

void mixerReset() {
  memset (blaster->mixer.reg, 0, sizeof (blaster->mixer.reg) );
  blaster->mixer.reg[0x22] = blaster->mixer.reg[0x26] = blaster->mixer.reg[0x04] = (4 << 5) | (4 << 1);
}

void initBlaster (uint16_t baseport, uint8_t irq) {
  //sbout = fopen("sbout.raw", "wb");
  memset (&blaster, 0, sizeof (blaster) );
  blaster->dspmaj = 3; //emulate a Sound Blaster Pro
  blaster->dspmin = 0;
  blaster->sbirq = irq;
  blaster->sbdma = 1;
  mixerReset();
  set_portout_redirector (baseport, baseport + 0xE, (void*) &outBlaster);
  set_portin_redirector (baseport, baseport + 0xE, (void*) &inBlaster);
} 
//████████████████████████████████████████
//sndsource.c
extern void set_portout_redirector (uint16_t startport, uint16_t endport, void *callback);
extern void set_portin_redirector (uint16_t startport, uint16_t endport, void *callback);

///extern uint8_t portram[0x10000];
uint8_t ssourcebuf[16], ssourceptr = 0, ssourceactive = 0;
int16_t ssourcecursample = 0;

int16_t getssourcebyte() {
  return (ssourcecursample);
}

void tickssource() {
  uint8_t rotatefifo;
  if ( (ssourceptr==0) || (!ssourceactive) ) {
      ssourcecursample = 0;
      return;
    }
  ssourcecursample = ssourcebuf[0];
  for (rotatefifo=1; rotatefifo<16; rotatefifo++)
    ssourcebuf[rotatefifo-1] = ssourcebuf[rotatefifo];
  ssourceptr--;
  portram[0x379] = 0;
}

void putssourcebyte (uint8_t value) {
  if (ssourceptr==16) return;
  ssourcebuf[ssourceptr++] = value;
  if (ssourceptr==16) portram[0x379] = 0x40;
}

uint8_t ssourcefull() {
  if (ssourceptr==16) return (0x40);
  else return (0x00);
}

void outsoundsource (uint16_t portnum, uint8_t value) {
  static uint8_t last37a = 0;
  switch (portnum) {
      case 0x378:
        putssourcebyte (value);
        break;
      case 0x37A:
        if ( (value & 4) && ! (last37a & 4) ) putssourcebyte (portram[0x378]);
        last37a = value;
        break;
    }
}

uint8_t insoundsource (uint16_t portnum) {
  return (ssourcefull() );
}

void initsoundsource() {
  set_portout_redirector (0x378, 0x378, (void*) &outsoundsource);
  set_portout_redirector (0x37A, 0x37A, (void*) &outsoundsource);
  set_portin_redirector (0x379, 0x379, (void*) &insoundsource);
  ssourceactive = 1;
} 
//████████████████████████████████████████



// KEYBOARD:
extern uint8_t translatescancode (uint8_t keyval) {
  switch (keyval) {
    case 0x3c:
      return (0x1E);//A
      break;
    case 0x50:
      return (0x30);//B
      break;
    case 0x4e:
      return (0x2E);//C
      break;
    case 0x3e:
      return (0x20);//D
      break;
    case 0x29:
      return (0x12);//E
      break;
    case 0x3f:
      return (0x21);//F
      break;
    case 0x40: //0x0a
      return (0x22);//G
      break;          
    case 0x41:
      return (0x23);//H
      break;
    case 0x2e:
      return (0x17);//I
      break;
    case 0x42:
      return (0x24);//J
      break;
    case 0x43:
      return (0x25);//K
      break;
    case 0x44:
      return (0x26);//L
      break;
    case 0x52: //0x10
      return (0x32);//M
      break;
    case 0x51:
      return (0x31);//N
      break;          
    case 0x2f://0X12
      return (0x18);//O
      break;
    case 0x30:
      return (0x19);//P
      break;
    case 0x27:
      return (0x10);//Q
      break;
    case 0x2a:
      return (0x13);//R
      break;
    case 0x3d:
      return (0x1F);//S
      break;
    case 0x2b:
      return (0x14);//T
      break;
    case 0x2d:
      return (0x16);//U
      break;
    case 0x4f:
      return (0x2F);//V
      break;
    case 0x28:
      return (0x11);//W
      break;
    case 0x4d:
      return (0x2D);//X
      break;
    case 0x2c:
      return (0x15);//Y
      break;
    case 0x4c:
      return (0x2C);//Z
      break;          
    case 0x1b://0
      return (0x0B);
      break;        
    case 0x12://1
      return (0x02);
      break;         
    case 0x13://2
      return (0x03);
      break;           
    case 0x14://3
      return (0x04);
      break;      
    case 0x15://4
      return (0x05);
      break;
    case 0x16://5
      return (0x06);
      break;
    case 0x17://6
      return (0x07);
      break;          
    case 0x18://7
      return (0x08);
      break;
    case 0x19://8
      return (0x09);
      break;
    case 0x1a://9
      return (0x0A);
      break;
      
    case 0x47:
    case 0x5b:
      return (0x1C);
      break; //enter
      
    case 0x5e:
      return (0x39);
      break; //space
      
    case 0x01:
      return (0x01);
      break; //escape
      
    case 0x34:
      return (0x53);//del
      break; 
      
    case 0x1e:
      return (0xE);
      break; //backspace
      
    case 0x26:
      return (0x0F);
      break; //tab
      
    case 0x57://up
      return (0x48);
      break; 
      
    case 0x61://left
      return (0x4B);
      break;
      
    case 0x62://down
      return (0x50);
      break; 
      
    case 0x63://right
      return (0x4d);
      break;
      
    case 0x1c://minus
      return (0x0c);
      break; 
      
    case 0x1d://equal =
      return (0x0d);
      break;
      
    case 0x31:// [
      return (0x1a);
      break; 
      
    case 0x32://]
      return (0x1b);
      break; 
      
    case 0x33:// "\"
      return (0x2b);
      break;
      
    case 0x45: //";"
      return (0x27);
      break; 
      
    case 0x46:// "'"
      return (0x28);
      break;
      
    case 0x11: //`
      return (0x29);
      break; 
      
    case 0x53: //comma
      return (0x33);
      break; 
      
    case 0x54://dot
      return (0x34);
      break;
      
    case 0x55://slash
      return (0x35);
      break; 
      
    case 0x4b ://my internal code
    case 0x56 ://my internal code
      return (0x2A);//left shift
      break;
      
    case 0x5d://my internal code
    case 0x5f://my internal code

      return (0x38);//left alt
      break; 
      
    case 0x5c://my internal code
    case 0x60://my internal code
      return (0x1d);//left ctrl
      break; 
      
    case 0x02: //F1
        return (0+1 + 0x3A); ///
        break;
    case 0x03://F2
        return (0+2 + 0x3A); ///
        break;
    case 0x04://F3
        return (0+3 + 0x3A); ///
        break;
    case 0x05://F4
        return (0+4 + 0x3A); ///
        break;
    case 0x06://F5
        return (0+5 + 0x3A); ///
        break;
    case 0x07://F6
        return (0+6 + 0x3A); ///
        break;
    case 0x88://F7
        return (0+7 + 0x3A); ///
        break;
    case 0x09://F8
        return (0+8 + 0x3A); ///
        break;
    case 0x0a://F9
        return (0+9 + 0x3A); ///
        break;
    case 0x0b://F10
        return (0+10 + 0x3A); ///
        break;            
    case 0x7c: //F11
      return (0x57);
      break;           
    case 0x0d: //F12
      return (0x58);
      break;       
    default:
      return (0);
      break; 
  }//end of switch
} 

//KEYBOARD:
extern uint8_t translatescancode_directly (uint8_t keyval) {
  switch (keyval) {
    case 0x04:
      return (0x1E);//A
      break;
    case 0x05:
      return (0x30);//B
      break;
    case 0x06:
      return (0x2E);//C
      break;
    case 0x07:
      return (0x20);//D
      break;
    case 0x08:
      return (0x12);//E
      break;
    case 0x09:
      return (0x21);//F
      break;
    case 0x0a:
      return (0x22);//G
      break;          
    case 0x0b:
      return (0x23);//H
      break;
    case 0x0c:
      return (0x17);//I
      break;
    case 0x0d:
      return (0x24);//J
      break;
    case 0x0e:
      return (0x25);//K
      break;
    case 0x0f:
      return (0x26);//L
      break;
    case 0x10:
      return (0x32);//M
      break;
    case 0x11:
      return (0x31);//N
      break;          
    case 0x12://0X12
      return (0x18);//O
      break;
    case 0x13:
      return (0x19);//P
      break;
    case 0x14:
      return (0x10);//Q
      break;
    case 0x15:
      return (0x13);//R
      break;
    case 0x16:
      return (0x1F);//S
      break;
    case 0x17:
      return (0x14);//T
      break;
    case 0x18:
      return (0x16);//U
      break;
    case 0x19:
      return (0x2F);//V
      break;
    case 0x1a:
      return (0x11);//W
      break;
    case 0x1b:
      return (0x2D);//X
      break;
    case 0x1c:
      return (0x15);//Y
      break;
    case 0x1d:
      return (0x2C);//Z
      break;          
    case 0x27://0
      return (0x0B);
      break;
      
    case 0x1e://1
      return (0x02);
      break;
      
    case 0x1f://2
      return (0x03);
      break;  
      
    case 0x20://3
      return (0x04);
      break;
      
    case 0x21://4
      return (0x05);
      break;
    case 0x22://5
      return (0x06);
      break;
    case 0x23://6
      return (0x07);
      break;          
    case 0x24://7
      return (0x08);
      break;
    case 0x25://8
      return (0x09);
      break;
    case 0x26://9
      return (0x0A);
      break;
      
    case 0x58: 
    case 0x28: 
      return (0x1C);
      break; //enter
      
    case 0x2c:
      return (0x39);
      break; //space
      
    case 0x29:
      return (0x01);
      break; //escape
      
    case 0x4c:
      return (0x53);//del
      break; 
      
    case 0x2a:
      return (0xE);
      break; //backspace
      
    case 0x2b:
      return (0x0F);
      break; //tab
          
    case 0x52://up
      return (0x48);
      break; 
      
    case 0x50://left
      return (0x4B);
      break;
      
    case 0x51://down
      return (0x50);
      break; 
      
    case 0x4f://right
      return (0x4d);
      break;
      
    case 0x2d://minus
      return (0x0c);
      break; 
      
    case 0x2e://equal =
      return (0x0d);
      break;
      
    case 0x2f:// [
      return (0x1a);
      break; 
      
    case 0x30://]
      return (0x1b);
      break; 
      
    case 0x32:// "\"
      return (0x2b);
      break;
      
    case 0x33: //";"
      return (0x27);
      break; 
      
    case 0x34:// "'"
      return (0x28);
      break;
      
    case 0x35: //`
      return (0x29);
      break; 
      
    case 0x36: //comma
      return (0x33);
      break; 
      
    case 0x37://dot
      return (0x34);
      break;
      
    case 0x38://slash
      return (0x35);
      break; 
//--------------------------------------------------------------------------------
    case 0x4b ://my internal code
    case 0x56 ://my internal code
        return (0x2A);//left shift
        break;
        
    case 0x5d://my internal code
    case 0x5f://my internal code

      return (0x38);//left alt
      break; 
      
    case 0x5c://my internal code
    case 0x60://my internal code
      return (0x1d);//left ctrl
      break; 
//--------------------------------------------------------------------------------
          
    case 0x3a: //F1
      return (0+1 + 0x3A); ///
      break;
    case 0x3b://F2
      return (0+2 + 0x3A); ///
      break;
    case 0x3c://F3
      return (0+3 + 0x3A); ///
      break;
    case 0x3d://F4
      return (0+4 + 0x3A); ///
      break;
    case 0x3e://F5
      return (0+5 + 0x3A); ///
      break;
    case 0x3f://F6
      return (0+6 + 0x3A); ///
      break;
    case 0x40://F7
      return (0+7 + 0x3A); ///
      break;
    case 0x41://F8
      return (0+8 + 0x3A); ///
      break;
    case 0x42://F9
      return (0+9 + 0x3A); ///
      break;
    case 0x43://F10
      return (0+10 + 0x3A); ///
      break;            
    case 0x44: //F11
      return (0x57);
      break;           
    case 0x45: //F12
      return (0x58);
      break;       
    default:
      return (0);
      break; 
  }//end of switch
} 


//********************************************************************************
//  KEYBOARD FAKE86 FUNCTIONS
//********************************************************************************
//Key kode = usb scan code
extern void emulate_key_down(uint8_t key_code) {
  if (POWER == APP_EMU_FAKE86) {///FAKE86 runnung
#if (DEBUG > 2) && !PS2_KEYBOARD //can not printf if is used PS_KEYBOARD
    printf("emulate_key_down %d\n",key_code);
#endif
    portram[0x60] = (key_code);
    portram[0x64] |= 2;
    doirq(1);
  }  
}
//--------------------------------------------------------------------------------
//Key kode = usb scan code
extern void emulate_key_up(uint8_t key_code) {
  if (POWER == APP_EMU_FAKE86) { ///FAKE86 runnung
#if (DEBUG > 2) && !PS2_KEYBOARD //can not printf if is used PS_KEYBOARD   
    printf("emulate_key_up\n");
#endif  
    portram[0x60] = (key_code) | 0x80;
    portram[0x64] |= 2;
    doirq(1);
  }  
}
//--------------------------------------------------------------------------------
extern void FAKE86_KEYBOARD() {
  if (POWER == APP_EMU_FAKE86) {
    for (uint16_t tmp=0;tmp<256;tmp++) {
      if (BTKEYBOARD[tmp]==1)
      keymap[tmp]=BTKEYBOARD[tmp];  // keymap bridge...
    }
#if USBHOST    
    for (uint16_t tmp=0;tmp<256;tmp++) {
      if (USBKEYBOARD[tmp]==1)
      keymap[tmp]=USBKEYBOARD[tmp]; // keymap bridge...  
    }
#endif
#if PS2_KEYBOARD    
    for (uint16_t tmp=0;tmp<256;tmp++) {
      if (USBKEYBOARD[tmp]==1)
      keymap[tmp]=PS2KEYBOARD[tmp]; // keymap bridge...
    }
#endif 

//--------------------------------------------------------------------------------
    for (uint16_t tmp=0;tmp<256;tmp++) {
      if (keymap[tmp]==0 && ((uint32_t*)prevkeytime)[tmp]!=0) { ///released KEY
        prevkeytime[tmp]=0;
        emulate_key_up(translatescancode(tmp));
      }   

      if (keymap[tmp]==0) ((uint32_t*)prevkeytime)[tmp]=0;
    
      if (keymap[tmp]==1 && ((uint32_t*)prevkeytime)[tmp]==0) { //pressed KEY
        prevkeytime[tmp]=millis___();
        emulate_key_down(translatescancode(tmp));
      } else if (keymap[tmp]==1 && ((uint32_t*)prevkeytime)[tmp]!=0 && (millis___() - ((uint32_t*)prevkeytime)[tmp] > KEYBOARD_REPEAT_PAUSE)) {
        emulate_key_down(translatescancode(tmp));
      }   
    }
//--------------------------------------------------------------------------------    


    for (uint16_t tmp=0;tmp<256;tmp++) {
      if (BTKEYBOARD[tmp]==0)
      keymap[tmp]=BTKEYBOARD[tmp];  // keymap bridge...
    }
#if USBHOST    
    for (uint16_t tmp=0;tmp<256;tmp++) {
      if (USBKEYBOARD[tmp]==0)
      keymap[tmp]=USBKEYBOARD[tmp]; // keymap bridge...  
    }
#endif
#if PS2_KEYBOARD    
    for (uint16_t tmp=0;tmp<256;tmp++) {
      if (USBKEYBOARD[tmp]==0)
      keymap[tmp]=PS2KEYBOARD[tmp]; // keymap bridge...
    }
#endif   
  }
}
//********************************************************************************







void fake86_inithardware(void)
{
  printf ("Init emulated hardware:\n");

  memset(portcallback_, 0, sizeof(portcallback_t)); ///  

  init8253();
  init8259();
  
  printf ("  - Intel 8237 DMA controller: ");
  init8237();
  printf ("OK\n");
  
  initVideoPorts();

#if !FAKE86_NO_AUDIO
  printf ("  - Adlib FM music card: ");
  initadlib (0x388);
  printf ("OK\n");
  printf ("  - Creative Labs Sound Blaster Pro: ");
  initBlaster (0x220, 7);
  printf ("OK\n");
#endif
 
  inittiming();
  initcga();
}


//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
uint32_t nw, nh; //native width and height, pre-stretching (i.e. 320x200 for mode 13h)
extern uint16_t oldw, oldh, constantw, constanth;

extern void Fake86Video (void) {
  uint32_t time_now = millis___();
  static uint32_t last_display_update_time = 0;
  
  if (POWER == APP_EMU_FAKE86) {
    time_now = millis___();
    if ((updatedscreen || renderbenchmark) && ((time_now - last_display_update_time) > 30)) //limit FPS
    {
      updatedscreen = 0;
      fake86_draw();
      drawready=1;

      time_now = millis___();     
      last_display_update_time = time_now;
 
      Serial.println((uint32_t)totalframes);
      totalframes++;
    }
  }
}

//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
#define CPU_STEPS_COUNT 10000 //default: 10000

uint32_t cpu_ticks = 0;
uint8_t speakerenabled=0;

void fake86_run_task(void) {
  exec86(CPU_STEPS_COUNT);  
  cpu_ticks+= CPU_STEPS_COUNT;
  if (scrmodechange) doscrmodechange();
}
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣

#endif
