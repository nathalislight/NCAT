//--------------------------------------------------------------------------------
//MAPPERS !!!
//................................................................................
//MAPPER0
mapintf_t map0_intf =
{
  0, /* mapper number */
  (char*)"None", /* mapper name */
  NULL, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  NULL, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER1
/* TODO: roll this into something... */
 int bitcount = 0;
 uint8_t latch = 0;
 uint8_t regs[4];
 int bank_select;
 uint8_t lastreg;

 void map1_write(uint32_t address, uint8_t value)
{
  int regnum = (address >> 13) - 4;

  if (value & 0x80)
  {
    regs[0] |= 0x0C;
    bitcount = 0;
    latch = 0;
    return;
  }

  if (lastreg != regnum)
  {
    bitcount = 0;
    latch = 0;
    lastreg = regnum;
  }
  //lastreg = regnum;

  latch |= ((value & 1) << bitcount++);

  /* 5 bit registers */
  if (5 != bitcount)
    return;

  regs[regnum] = latch;
  value = latch;
  bitcount = 0;
  latch = 0;

  switch (regnum)
  {
    case 0:
      {
        if (0 == (value & 2))
        {
          int mirror = value & 1;
          ppu_mirror(mirror, mirror, mirror, mirror);
        }
        else
        {
          if (value & 1)
            ppu_mirror(0, 0, 1, 1);
          else
            ppu_mirror(0, 1, 0, 1);
        }
      }
      break;

    case 1:
      if (regs[0] & 0x10)
        mmc_bankvrom(4, 0x0000, value);
      else
        mmc_bankvrom(8, 0x0000, value >> 1);
      break;

    case 2:
      if (regs[0] & 0x10)
        mmc_bankvrom(4, 0x1000, value);
      break;

    case 3:
      if (mmc_getinfo()->rom_banks == 0x20)
      {
        bank_select = (regs[1] & 0x10) ? 0 : 0x10;
      }
      else if (mmc_getinfo()->rom_banks == 0x40)
      {
        if (regs[0] & 0x10)
          bank_select = (regs[1] & 0x10) | ((regs[2] & 0x10) << 1);
        else
          bank_select = (regs[1] & 0x10) << 1;
      }
      else
      {
        bank_select = 0;
      }

      if (0 == (regs[0] & 0x08))
        mmc_bankrom(32, 0x8000, ((regs[3] >> 1) + (bank_select >> 1)));
      else if (regs[0] & 0x04)
        mmc_bankrom(16, 0x8000, ((regs[3] & 0xF) + bank_select));
      else
        mmc_bankrom(16, 0xC000, ((regs[3] & 0xF) + bank_select));

    default:
      break;
  }
}

 void map1_init(void)
{
  bitcount = 0;
  latch = 0;

  memset(regs, 0, sizeof(regs));

  if (mmc_getinfo()->rom_banks == 0x20)
    mmc_bankrom(16, 0xC000, 0x0F);

  map1_write(0x8000, 0x80);
}

 void map1_getstate(SnssMapperBlock *state);
 void map1_getstate(SnssMapperBlock *state)
{
  state->extraData.mapper1.registers[0] = regs[0];
  state->extraData.mapper1.registers[1] = regs[1];
  state->extraData.mapper1.registers[2] = regs[2];
  state->extraData.mapper1.registers[3] = regs[3];
  state->extraData.mapper1.latch = latch;
  state->extraData.mapper1.numberOfBits = bitcount;
}


 void map1_setstate(SnssMapperBlock *state);
 void map1_setstate(SnssMapperBlock *state)
{
  regs[1] = state->extraData.mapper1.registers[0];
  regs[1] = state->extraData.mapper1.registers[1];
  regs[2] = state->extraData.mapper1.registers[2];
  regs[3] = state->extraData.mapper1.registers[3];
  latch = state->extraData.mapper1.latch;
  bitcount = state->extraData.mapper1.numberOfBits;
}

 map_memwrite map1_memwrite[] =
{
  { 0x8000, 0xFFFF, map1_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map1_intf =
{
  1, /* mapper number */
  (char*)"MMC1", /* mapper name */
  map1_init, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  map1_getstate, /* get state (snss) */
  map1_setstate, /* set state (snss) */
  NULL, /* memory read structure */
  map1_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER2
/* mapper 2: UNROM */
 void map2_write(uint32_t address, uint8_t value)
{
  UNUSED(address);

  mmc_bankrom(16, 0x8000, value);
}

 map_memwrite map2_memwrite[] =
{
  { 0x8000, 0xFFFF, map2_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map2_intf =
{
  2, /* mapper number */
  (char*)"UNROM", /* mapper name */
  NULL, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map2_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER3
/* mapper 3: CNROM */
 void map3_write(uint32_t address, uint8_t value)
{
  UNUSED(address);

  mmc_bankvrom(8, 0x0000, value);
}

 map_memwrite map3_memwrite[] =
{
  { 0x8000, 0xFFFF, map3_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map3_intf =
{
  3, /* mapper number */
  (char*)"CNROM", /* mapper name */
  NULL, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map3_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER4
 struct
{
  int counter, latch;
  bool enabled, reset;
} irq;

 uint8_t reg;
 uint8_t command;
 uint16_t vrombase;

/* mapper 4: MMC3 */
 void map4_write(uint32_t address, uint8_t value)
{
  switch (address & 0xE001)
  {
    case 0x8000:
      command = value;
      vrombase = (command & 0x80) ? 0x1000 : 0x0000;

      if (reg != (value & 0x40))
      {
        if (value & 0x40)
          mmc_bankrom(8, 0x8000, (mmc_getinfo()->rom_banks * 2) - 2);
        else
          mmc_bankrom(8, 0xC000, (mmc_getinfo()->rom_banks * 2) - 2);
      }
      reg = value & 0x40;
      break;

    case 0x8001:
      switch (command & 0x07)
      {
        case 0:
          value &= 0xFE;
          mmc_bankvrom(1, vrombase ^ 0x0000, value);
          mmc_bankvrom(1, vrombase ^ 0x0400, value + 1);
          break;

        case 1:
          value &= 0xFE;
          mmc_bankvrom(1, vrombase ^ 0x0800, value);
          mmc_bankvrom(1, vrombase ^ 0x0C00, value + 1);
          break;

        case 2:
          mmc_bankvrom(1, vrombase ^ 0x1000, value);
          break;

        case 3:
          mmc_bankvrom(1, vrombase ^ 0x1400, value);
          break;

        case 4:
          mmc_bankvrom(1, vrombase ^ 0x1800, value);
          break;

        case 5:
          mmc_bankvrom(1, vrombase ^ 0x1C00, value);
          break;

        case 6:
          mmc_bankrom(8, (command & 0x40) ? 0xC000 : 0x8000, value);
          break;

        case 7:
          mmc_bankrom(8, 0xA000, value);
          break;
      }
      break;

    case 0xA000:
      /* four screen mirroring crap */
      if (0 == (mmc_getinfo()->flags & ROM_FLAG_FOURSCREEN))
      {
        if (value & 1)
          ppu_mirror(0, 0, 1, 1); /* horizontal */
        else
          ppu_mirror(0, 1, 0, 1); /* vertical */
      }
      break;

    case 0xA001:
      /* Save RAM enable / disable */
      /* Messes up Startropics I/II if implemented -- bah */
      break;

    case 0xC000:
      irq.latch = value;
      //      if (irq.reset)
      //         irq.counter = irq.latch;
      break;

    case 0xC001:
      irq.reset = true;
      irq.counter = irq.latch;
      break;

    case 0xE000:
      irq.enabled = false;
      //      if (irq.reset)
      //         irq.counter = irq.latch;
      break;

    case 0xE001:
      irq.enabled = true;
      //      if (irq.reset)
      //         irq.counter = irq.latch;
      break;

    default:
      break;
  }

  if (true == irq.reset)
    irq.counter = irq.latch;
}

 void map4_hblank(int vblank)
{
  if (vblank)
    return;

  if (ppu_enabled())
  {
    if (irq.counter >= 0)
    {
      irq.reset = false;
      irq.counter--;

      if (irq.counter < 0)
      {
        if (irq.enabled)
        {
          irq.reset = true;
          nes_irq();
        }
      }
    }
  }
}

 void map4_getstate(SnssMapperBlock *state);
 void map4_getstate(SnssMapperBlock *state)
{
  state->extraData.mapper4.irqCounter = irq.counter;
  state->extraData.mapper4.irqLatchCounter = irq.latch;
  state->extraData.mapper4.irqCounterEnabled = irq.enabled;
  state->extraData.mapper4.last8000Write = command;
}

 void map4_setstate(SnssMapperBlock *state);
 void map4_setstate(SnssMapperBlock *state)
{
  irq.counter = state->extraData.mapper4.irqCounter;
  irq.latch = state->extraData.mapper4.irqLatchCounter;
  irq.enabled = state->extraData.mapper4.irqCounterEnabled;
  command = state->extraData.mapper4.last8000Write;
}

 void map4_init(void)
{
  irq.counter = irq.latch = 0;
  irq.enabled = irq.reset = false;
  reg = command = 0;
  vrombase = 0x0000;
}

 map_memwrite map4_memwrite[] =
{
  { 0x8000, 0xFFFF, map4_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map4_intf =
{
  4, /* mapper number */
  (char*)"MMC3", /* mapper name */
  map4_init, /* init routine */
  NULL, /* vblank callback */
  map4_hblank, /* hblank callback */
  map4_getstate, /* get state (snss) */
  map4_setstate, /* set state (snss) */
  NULL, /* memory read structure */
  map4_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER5
///mmc5_snd.c

/* TODO: encapsulate apu/mmc5 rectangle */

#define  APU_OVERSAMPLE
#define  APU_VOLUME_DECAY(x)  ((x) -= ((x) >> 7))

 const int duty_lut[4] = { 2, 4, 8, 12 };

#define  MMC5_WRA0    0x5000
#define  MMC5_WRA1    0x5001
#define  MMC5_WRA2    0x5002
#define  MMC5_WRA3    0x5003
#define  MMC5_WRB0    0x5004
#define  MMC5_WRB1    0x5005
#define  MMC5_WRB2    0x5006
#define  MMC5_WRB3    0x5007
#define  MMC5_SMASK   0x5015

typedef struct mmc5rectangle_s
{
  uint8_t regs[4];

  bool enabled;

  float accum;
  int32_t freq;
  int32_t output_vol;
  bool fixed_envelope;
  bool holdnote;
  uint8_t volume;

  int32_t env_phase;
  int32_t env_delay;
  uint8_t env_vol;

  int vbl_length;
  uint8_t adder;
  int duty_flip;
} mmc5rectangle_t;

typedef struct mmc5dac_s
{
  int32_t output;
  bool enabled;
} mmc5dac_t;


 struct
{
  float incsize;
  uint8_t mul[2];
  mmc5rectangle_t rect[2];
  mmc5dac_t dac;
} mmc5;


#define  MMC5_RECTANGLE_OUTPUT   chan->output_vol

 int32_t mmc5_rectangle(mmc5rectangle_t *chan);
 int32_t mmc5_rectangle(mmc5rectangle_t *chan)
{
  int32_t output;

#ifdef APU_OVERSAMPLE
  int num_times;
  int32_t total;
#endif /* APU_OVERSAMPLE */

  /* reg0: 0-3=volume, 4=envelope, 5=hold, 6-7=duty cycle
  ** reg1: 0-2=sweep shifts, 3=sweep inc/dec, 4-6=sweep length, 7=sweep on
  ** reg2: 8 bits of freq
  ** reg3: 0-2=high freq, 7-4=vbl length counter
  */

  APU_VOLUME_DECAY(chan->output_vol);

  if (false == chan->enabled || 0 == chan->vbl_length)
    return MMC5_RECTANGLE_OUTPUT;

  /* vbl length counter */
  if (false == chan->holdnote)
    chan->vbl_length--;

  /* envelope decay at a rate of (env_delay + 1) / 240 secs */
  chan->env_phase -= 4; /* 240/60 */
  while (chan->env_phase < 0)
  {
    chan->env_phase += chan->env_delay;

    if (chan->holdnote)
      chan->env_vol = (chan->env_vol + 1) & 0x0F;
    else if (chan->env_vol < 0x0F)
      chan->env_vol++;
  }

  if (chan->freq < 4)
    return MMC5_RECTANGLE_OUTPUT;

  chan->accum -= mmc5.incsize; /* # of cycles per sample */
  if (chan->accum >= 0)
    return MMC5_RECTANGLE_OUTPUT;

#ifdef APU_OVERSAMPLE
  num_times = total = 0;

  if (chan->fixed_envelope)
    output = chan->volume << 8; /* fixed volume */
  else
    output = (chan->env_vol ^ 0x0F) << 8;
#endif

  while (chan->accum < 0)
  {
    chan->accum += chan->freq;
    chan->adder = (chan->adder + 1) & 0x0F;

#ifdef APU_OVERSAMPLE
    if (chan->adder < chan->duty_flip)
      total += output;
    else
      total -= output;

    num_times++;
#endif
  }

#ifdef APU_OVERSAMPLE
  chan->output_vol = total / num_times;
#else
  if (chan->fixed_envelope)
    output = chan->volume << 8; /* fixed volume */
  else
    output = (chan->env_vol ^ 0x0F) << 8;

  if (0 == chan->adder)
    chan->output_vol = output;
  else if (chan->adder == chan->duty_flip)
    chan->output_vol = -output;
#endif

  return MMC5_RECTANGLE_OUTPUT;
}

 uint8_t mmc5_read(uint32_t address)
{
  uint32_t retval;

  retval = (uint32_t) (mmc5.mul[0] * mmc5.mul[1]);

  switch (address)
  {
    case 0x5205:
      return (uint8_t) retval;

    case 0x5206:
      return (uint8_t) (retval >> 8);

    default:
      return 0xFF;
  }
}

/* mix vrcvi sound channels together */
 int32_t mmc5_process(void)
{
  int32_t accum;

  accum = mmc5_rectangle(&mmc5.rect[0]);
  accum += mmc5_rectangle(&mmc5.rect[1]);
  if (mmc5.dac.enabled)
    accum += mmc5.dac.output;

  return accum;
}

/* write to registers */
 void mmc5_write(uint32_t address, uint8_t value)
{
  int chan;

  switch (address)
  {
    /* rectangles */
    case MMC5_WRA0:
    case MMC5_WRB0:
      chan = (address & 4) ? 1 : 0;
      mmc5.rect[chan].regs[0] = value;

      mmc5.rect[chan].volume = value & 0x0F;
      mmc5.rect[chan].env_delay = decay_lut[value & 0x0F];
      mmc5.rect[chan].holdnote = (value & 0x20) ? true : false;
      mmc5.rect[chan].fixed_envelope = (value & 0x10) ? true : false;
      mmc5.rect[chan].duty_flip = duty_lut[value >> 6];
      break;

    case MMC5_WRA1:
    case MMC5_WRB1:
      break;

    case MMC5_WRA2:
    case MMC5_WRB2:
      chan = (address & 4) ? 1 : 0;
      mmc5.rect[chan].regs[2] = value;
      if (mmc5.rect[chan].enabled)
        mmc5.rect[chan].freq = (((mmc5.rect[chan].regs[3] & 7) << 8) + value) + 1;
      break;

    case MMC5_WRA3:
    case MMC5_WRB3:
      chan = (address & 4) ? 1 : 0;
      mmc5.rect[chan].regs[3] = value;

      if (mmc5.rect[chan].enabled)
      {
        mmc5.rect[chan].vbl_length = vbl_lut[value >> 3];
        mmc5.rect[chan].env_vol = 0;
        mmc5.rect[chan].freq = (((value & 7) << 8) + mmc5.rect[chan].regs[2]) + 1;
        mmc5.rect[chan].adder = 0;
      }
      break;

    case MMC5_SMASK:
      if (value & 0x01)
      {
        mmc5.rect[0].enabled = true;
      }
      else
      {
        mmc5.rect[0].enabled = false;
        mmc5.rect[0].vbl_length = 0;
      }

      if (value & 0x02)
      {
        mmc5.rect[1].enabled = true;
      }
      else
      {
        mmc5.rect[1].enabled = false;
        mmc5.rect[1].vbl_length = 0;
      }

      break;

    case 0x5010:
      if (value & 0x01)
        mmc5.dac.enabled = true;
      else
        mmc5.dac.enabled = false;
      break;

    case 0x5011:
      mmc5.dac.output = (value ^ 0x80) << 8;
      break;

    case 0x5205:
      mmc5.mul[0] = value;
      break;

    case 0x5206:
      mmc5.mul[1] = value;
      break;

    case 0x5114:
    case 0x5115:
      /* ???? */
      break;

    default:
      break;
  }
}

/* reset state of vrcvi sound channels */
 void mmc5_reset(void)
{
  int i;
  apu_t apu;


  /* get the phase period from the apu */
  apu_getcontext(&apu);
  mmc5.incsize = apu.cycle_rate;

  for (i = 0x5000; i < 0x5008; i++)
    mmc5_write(i, 0);

  mmc5_write(0x5010, 0);
  mmc5_write(0x5011, 0);
}

 int mmc5_init(void)
{
  int i, num_samples;
  apu_t apu;

  apu_getcontext(&apu);
  num_samples = apu.num_samples;

  /* lut used for enveloping and frequency sweeps */
  for (i = 0; i < 16; i++)
    decay_lut[i] = num_samples * (i + 1);

  /* used for note length, based on vblanks and size of audio buffer */
  for (i = 0; i < 32; i++)
    vbl_lut[i] = vbl_length[i] * num_samples;

  return 0;
}

 apu_memread mmc5_memread[] =
{
  { 0x5205, 0x5206, mmc5_read },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

 apu_memwrite mmc5_memwrite[] =
{
  { 0x5000, 0x5015, mmc5_write },
  { 0x5114, 0x5115, mmc5_write },
  { 0x5205, 0x5206, mmc5_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

apuext_t mmc5_ext =
{
  mmc5_init,
  NULL, /* no shutdown */
  mmc5_reset,
  mmc5_process,
  mmc5_memread,
  mmc5_memwrite
};
///////////////////////////////////////

///
/*
   struct
  {
   int counter, enabled;
   int reset, latch;
  } irq;*/

/* MMC5 - Castlevania III, etc */
 void map5_hblank(int vblank)
{
  UNUSED(vblank);

  if (irq.counter == nes_getcontextptr()->scanline)
  {
    if (true == irq.enabled)
    {
      nes_irq();
      irq.reset = true;
    }
    //else
    //   irq.reset = false;
    irq.counter = irq.latch;
  }
}

 void map5_write(uint32_t address, uint8_t value)
{
   int page_size = 8;

  /* ex-ram memory-- bleh! */
  if (address >= 0x5C00 && address <= 0x5FFF)
    return;

  switch (address)
  {
    case 0x5100:
      /* PRG page size setting */
      /* 0:32k 1:16k 2,3:8k */
      switch (value & 3)
      {
        case 0:
          page_size = 32;
          break;

        case 1:
          page_size = 16;
          break;

        case 2:
        case 3:
          page_size = 8;
          break;
      }
      break;

    case 0x5101:
      /* CHR page size setting */
      /* 0:8k 1:4k 2:2k 3:1k */
      break;

    case 0x5104:
      /* GFX mode setting */
      /*
        00:split mode
        01:split & exgraffix
        10:ex-ram
        11:exram + write protect
      */
      break;

    case 0x5105:
      /* TODO: exram needs to fill in nametables 2-3 */
      ppu_mirror(value & 3, (value >> 2) & 3, (value >> 4) & 3, value >> 6);
      break;

    case 0x5106:
    case 0x5107:
      /* ex-ram fill mode stuff */
      break;

    case 0x5113:
      /* ram page for $6000-7FFF? bit 2*/
      break;

    case 0x5114:
      mmc_bankrom(8, 0x8000, value);
      //if (page_size == 8)
      //   mmc_bankrom(8, 0x8000, value);
      break;

    case 0x5115:
      mmc_bankrom(8, 0x8000, value);
      mmc_bankrom(8, 0xA000, value + 1);
      //if (page_size == 8)
      //   mmc_bankrom(8, 0xA000, value);
      //else if (page_size == 16)
      //   mmc_bankrom(16, 0x8000, value >> 1);
      //mmc_bankrom(16, 0x8000, value & 0xFE);
      break;

    case 0x5116:
      mmc_bankrom(8, 0xC000, value);
      //if (page_size == 8)
      //   mmc_bankrom(8, 0xC000, value);
      break;

    case 0x5117:
      //if (page_size == 8)
      //   mmc_bankrom(8, 0xE000, value);
      //else if (page_size == 16)
      //   mmc_bankrom(16, 0xC000, value >> 1);
      //mmc_bankrom(16, 0xC000, value & 0xFE);
      //else if (page_size == 32)
      //   mmc_bankrom(32, 0x8000, value >> 2);
      //mmc_bankrom(32, 0x8000, value & 0xFC);
      break;

    case 0x5120:
      mmc_bankvrom(1, 0x0000, value);
      break;

    case 0x5121:
      mmc_bankvrom(1, 0x0400, value);
      break;

    case 0x5122:
      mmc_bankvrom(1, 0x0800, value);
      break;

    case 0x5123:
      mmc_bankvrom(1, 0x0C00, value);
      break;

    case 0x5124:
    case 0x5125:
    case 0x5126:
    case 0x5127:
      /* more VROM shit? */
      break;

    case 0x5128:
      mmc_bankvrom(1, 0x1000, value);
      break;

    case 0x5129:
      mmc_bankvrom(1, 0x1400, value);
      break;

    case 0x512A:
      mmc_bankvrom(1, 0x1800, value);
      break;

    case 0x512B:
      mmc_bankvrom(1, 0x1C00, value);
      break;

    case 0x5203:
      irq.counter = value;
      irq.latch = value;
      //      irq.reset = false;
      break;

    case 0x5204:
      irq.enabled = (value & 0x80) ? true : false;
      //      irq.reset = false;
      break;

    default:
      break;
  }
}

 uint8_t map5_read(uint32_t address)
{
  /* Castlevania 3 IRQ counter */
  if (address == 0x5204)
  {
    /* if reset == 1, we've hit scanline */
    return (irq.reset ? 0x40 : 0x00);
  }
  else
  {
    return 0xFF;
  }
}

 void map5_init(void)
{
  mmc_bankrom(8, 0x8000, MMC_LASTBANK);
  mmc_bankrom(8, 0xA000, MMC_LASTBANK);
  mmc_bankrom(8, 0xC000, MMC_LASTBANK);
  mmc_bankrom(8, 0xE000, MMC_LASTBANK);

  irq.counter = irq.enabled = 0;
  irq.reset = irq.latch = 0;
}

/* incomplete SNSS definition */
 void map5_getstate(SnssMapperBlock *state);
 void map5_getstate(SnssMapperBlock *state)
{
  state->extraData.mapper5.dummy = 0;
}

 void map5_setstate(SnssMapperBlock *state);
 void map5_setstate(SnssMapperBlock *state)
{
  UNUSED(state);
}

 map_memwrite map5_memwrite[] =
{
  /* $5000 - $5015 handled by sound */
  { 0x5016, 0x5FFF, map5_write },
  { 0x8000, 0xFFFF, map5_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

 map_memread map5_memread[] =
{
  { 0x5204, 0x5204, map5_read },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map5_intf =
{
  5, /* mapper number */
  (char*)"MMC5", /* mapper name */
  map5_init, /* init routine */
  NULL, /* vblank callback */
  map5_hblank, /* hblank callback */
  map5_getstate, /* get state (snss) */
  map5_setstate, /* set state (snss) */
  map5_memread, /* memory read structure */
  map5_memwrite, /* memory write structure */
  &mmc5_ext /* external sound device */
};
//................................................................................
//MAPPER7
/* mapper 7: AOROM */
 void map7_write(uint32_t address, uint8_t value)
{
  int mirror;
  UNUSED(address);

  mmc_bankrom(32, 0x8000, value);
  mirror = (value & 0x10) >> 4;
  ppu_mirror(mirror, mirror, mirror, mirror);
}

 void map7_init(void)
{
  mmc_bankrom(32, 0x8000, 0);
}

 map_memwrite map7_memwrite[] =
{
  { 0x8000, 0xFFFF, map7_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map7_intf =
{
  7, /* mapper number */
  (char*)"AOROM", /* mapper name */
  map7_init, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map7_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER8
/* mapper 8: FFE F3xxx -- what the hell uses this? */
 void map8_write(uint32_t address, uint8_t value)
{
  UNUSED(address);

  mmc_bankrom(16, 0x8000, value >> 3);
  mmc_bankvrom(8, 0x0000, value & 7);
}

 void map8_init(void)
{
  mmc_bankrom(16, 0x8000, 0);
  mmc_bankrom(16, 0xC000, 1);
  mmc_bankvrom(8, 0x0000, 0);
}

 map_memwrite map8_memwrite[] =
{
  { 0x8000, 0xFFFF, map8_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map8_intf =
{
  8, /* mapper number */
  (char*)"FFE F3xxx", /* mapper name */
  map8_init, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map8_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER9
 uint8_t latch9[2];
 uint8_t regs9[4];

/* Used when tile $FD/$FE is accessed */
 void mmc9_latchfunc(uint32_t address, uint8_t value)
{
  if (0xFD == value || 0xFE == value)
  {
    int reg;

    if (address)
    {
      latch9[1] = value;
      reg = 2 + (value - 0xFD);
    }
    else
    {
      latch9[0] = value;
      reg = value - 0xFD;
    }

    mmc_bankvrom(4, address, regs9[reg]);
  }
}

/* mapper 9: MMC2 */
/* MMC2: Punch-Out! */
 void map9_write(uint32_t address, uint8_t value)
{
  switch ((address & 0xF000) >> 12)
  {
    case 0xA:
      mmc_bankrom(8, 0x8000, value);
      break;

    case 0xB:
      regs9[0] = value;
      if (0xFD == latch9[0])
        mmc_bankvrom(4, 0x0000, value);
      break;

    case 0xC:
      regs9[1] = value;
      if (0xFE == latch9[0])
        mmc_bankvrom(4, 0x0000, value);
      break;

    case 0xD:
      regs9[2] = value;
      if (0xFD == latch9[1])
        mmc_bankvrom(4, 0x1000, value);
      break;

    case 0xE:
      regs9[3] = value;
      if (0xFE == latch9[1])
        mmc_bankvrom(4, 0x1000, value);
      break;

    case 0xF:
      if (value & 1)
        ppu_mirror(0, 0, 1, 1); /* horizontal */
      else
        ppu_mirror(0, 1, 0, 1); /* vertical */
      break;

    default:
      break;
  }
}

 void map9_init(void)
{
  memset(regs9, 0, sizeof(regs9));

  mmc_bankrom(8, 0x8000, 0);
  mmc_bankrom(8, 0xA000, (mmc_getinfo()->rom_banks * 2) - 3);
  mmc_bankrom(8, 0xC000, (mmc_getinfo()->rom_banks * 2) - 2);
  mmc_bankrom(8, 0xE000, (mmc_getinfo()->rom_banks * 2) - 1);

  latch9[0] = 0xFE;
  latch9[1] = 0xFE;

  ppu_setlatchfunc(mmc9_latchfunc);
}

 void map9_getstate(SnssMapperBlock *state);
 void map9_getstate(SnssMapperBlock *state)
{
  state->extraData.mapper9.latch[0] = latch9[0];
  state->extraData.mapper9.latch[1] = latch9[1];
  state->extraData.mapper9.lastB000Write = regs9[0];
  state->extraData.mapper9.lastC000Write = regs9[1];
  state->extraData.mapper9.lastD000Write = regs9[2];
  state->extraData.mapper9.lastE000Write = regs9[3];
}

 void map9_setstate(SnssMapperBlock *state);
 void map9_setstate(SnssMapperBlock *state)
{
  latch9[0] = state->extraData.mapper9.latch[0];
  latch9[1] = state->extraData.mapper9.latch[1];
  regs9[0] = state->extraData.mapper9.lastB000Write;
  regs9[1] = state->extraData.mapper9.lastC000Write;
  regs9[2] = state->extraData.mapper9.lastD000Write;
  regs9[3] = state->extraData.mapper9.lastE000Write;
}

 map_memwrite map9_memwrite[] =
{
  { 0x8000, 0xFFFF, map9_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map9_intf =
{
  9, /* mapper number */
  (char*)"MMC2", /* mapper name */
  map9_init, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  map9_getstate, /* get state (snss) */
  map9_setstate, /* set state (snss) */
  NULL, /* memory read structure */
  map9_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER11
/* mapper 11: Color Dreams, Wisdom Tree */
 void map11_write(uint32_t address, uint8_t value)
{
  UNUSED(address);

  mmc_bankrom(32, 0x8000, value & 0x0F);
  mmc_bankvrom(8, 0x0000, value >> 4);
}

 void map11_init(void)
{
  mmc_bankrom(32, 0x8000, 0);
  mmc_bankvrom(8, 0x0000, 0);
}

 map_memwrite map11_memwrite[] =
{
  { 0x8000, 0xFFFF, map11_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map11_intf =
{
  11, /* mapper number */
  (char*)"Color Dreams", /* mapper name */
  map11_init, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map11_memwrite, /* memory write structure */
  NULL /* external sound device */
};

//................................................................................
//MAPPER15
/* mapper 15: Contra 100-in-1 */
 void map15_write(uint32_t address, uint8_t value)
{
  int bank = value & 0x3F;
  uint8_t swap = (value & 0x80) >> 7;

  switch (address & 0x3)
  {
    case 0:
      mmc_bankrom(8, 0x8000, (bank << 1) + swap);
      mmc_bankrom(8, 0xA000, (bank << 1) + (swap ^ 1));
      mmc_bankrom(8, 0xC000, ((bank + 1) << 1) + swap);
      mmc_bankrom(8, 0xE000, ((bank + 1) << 1) + (swap ^ 1));

      if (value & 0x40)
        ppu_mirror(0, 0, 1, 1); /* horizontal */
      else
        ppu_mirror(0, 1, 0, 1); /* vertical */
      break;

    case 1:
      mmc_bankrom(8, 0xC000, (bank << 1) + swap);
      mmc_bankrom(8, 0xE000, (bank << 1) + (swap ^ 1));
      break;

    case 2:
      if (swap)
      {
        mmc_bankrom(8, 0x8000, (bank << 1) + 1);
        mmc_bankrom(8, 0xA000, (bank << 1) + 1);
        mmc_bankrom(8, 0xC000, (bank << 1) + 1);
        mmc_bankrom(8, 0xE000, (bank << 1) + 1);
      }
      else
      {
        mmc_bankrom(8, 0x8000, (bank << 1));
        mmc_bankrom(8, 0xA000, (bank << 1));
        mmc_bankrom(8, 0xC000, (bank << 1));
        mmc_bankrom(8, 0xE000, (bank << 1));
      }
      break;

    case 3:
      mmc_bankrom(8, 0xC000, (bank << 1) + swap);
      mmc_bankrom(8, 0xE000, (bank << 1) + (swap ^ 1));

      if (value & 0x40)
        ppu_mirror(0, 0, 1, 1); /* horizontal */
      else
        ppu_mirror(0, 1, 0, 1); /* vertical */
      break;

    default:
      break;
  }
}

 void map15_init(void)
{
  mmc_bankrom(32, 0x8000, 0);
}

 map_memwrite map15_memwrite[] =
{
  { 0x8000, 0xFFFF, map15_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map15_intf =
{
  15, /* mapper number */
  (char*)"Contra 100-in-1", /* mapper name */
  map15_init, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map15_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER16
 struct
{
  int counter;
  bool enabled;
} irq16;

/* mapper 16: Bandai */

 void map16_init(void)
{
  mmc_bankrom(16, 0x8000, 0);
  mmc_bankrom(16, 0xC000, MMC_LASTBANK);
  irq16.counter = 0;
  irq16.enabled = false;
}

 void map16_write(uint32_t address, uint8_t value)
{
  int reg = address & 0xF;

  if (reg < 8)
  {
    mmc_bankvrom(1, reg << 10, value);
  }
  else
  {
    switch (address & 0x000F)
    {
      case 0x8:
        mmc_bankrom(16, 0x8000, value);
        break;

      case 0x9:
        switch (value & 3)
        {
          case 0:
            ppu_mirror(0, 0, 1, 1); /* horizontal */
            break;

          case 1:
            ppu_mirror(0, 1, 0, 1); /* vertical */
            break;

          case 2:
            ppu_mirror(0, 0, 0, 0);
            break;

          case 3:
            ppu_mirror(1, 1, 1, 1);
            break;
        }
        break;

      case 0xA:
        irq16.enabled = (value & 1) ? true : false;
        break;

      case 0xB:
        irq16.counter = (irq16.counter & 0xFF00) | value;
        break;

      case 0xC:
        irq16.counter = (value << 8) | (irq16.counter & 0xFF);
        break;

      case 0xD:
        /* eeprom I/O port? */
        break;
    }
  }
}

 void map16_hblank(int vblank)
{
  UNUSED(vblank);

  if (irq16.enabled)
  {
    if (irq16.counter)
    {
      if (0 == --irq16.counter)
        nes_irq();
    }
  }
}

 void map16_getstate(SnssMapperBlock *state);
 void map16_getstate(SnssMapperBlock *state)
{
  state->extraData.mapper16.irqCounterLowByte = irq16.counter & 0xFF;
  state->extraData.mapper16.irqCounterHighByte = irq16.counter >> 8;
  state->extraData.mapper16.irqCounterEnabled = irq16.enabled;
}

 void map16_setstate(SnssMapperBlock *state);
 void map16_setstate(SnssMapperBlock *state)
{
  irq.counter = (state->extraData.mapper16.irqCounterHighByte << 8)
                | state->extraData.mapper16.irqCounterLowByte;
  irq.enabled = state->extraData.mapper16.irqCounterEnabled;
}

 map_memwrite map16_memwrite[] =
{
  { 0x6000, 0x600D, map16_write },
  { 0x7FF0, 0x7FFD, map16_write },
  { 0x8000, 0x800D, map16_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map16_intf =
{
  16, /* mapper number */
  (char*)"Bandai", /* mapper name */
  map16_init, /* init routine */
  NULL, /* vblank callback */
  map16_hblank, /* hblank callback */
  map16_getstate, /* get state (snss) */
  map16_setstate, /* set state (snss) */
  NULL, /* memory read structure */
  map16_memwrite, /* memory write structure */
  NULL /* external sound device */
};

//................................................................................
//MAPPER18
/* mapper 18: Jaleco SS8806 */
#define  VRC_PBANK(bank, value, high) \
  do { \
    if ((high)) \
      highprgnybbles[(bank)] = (value) & 0x0F; \
    else \
      lowprgnybbles[(bank)] = (value) & 0x0F; \
    mmc_bankrom(8, 0x8000 + ((bank) << 13), (highprgnybbles[(bank)] << 4)+lowprgnybbles[(bank)]); \
  } while (0)

#define VRC_VBANK(bank, value, high) \
  { \
    if ((high)) \
      highnybbles[(bank)] = (value) & 0x0F; \
    else \
      lownybbles[(bank)] = (value) & 0x0F; \
    mmc_bankvrom(1, (bank) << 10, (highnybbles[(bank)] << 4)+lownybbles[(bank)]); \
  }

 struct
{
  int counter, enabled;
  uint8_t nybbles[4];
  int clockticks;
} irq18;

 void map18_init(void)
{
  irq18.counter = irq18.enabled = 0;
}

 uint8_t lownybbles[8];
 uint8_t highnybbles[8];
 uint8_t lowprgnybbles[3];
 uint8_t highprgnybbles[3];


 void map18_write(uint32_t address, uint8_t value)
{
  switch (address)
  {
    case 0x8000: VRC_PBANK(0, value, 0); break;
    case 0x8001: VRC_PBANK(0, value, 1); break;
    case 0x8002: VRC_PBANK(1, value, 0); break;
    case 0x8003: VRC_PBANK(1, value, 1); break;
    case 0x9000: VRC_PBANK(2, value, 0); break;
    case 0x9001: VRC_PBANK(2, value, 1); break;
    case 0xA000: VRC_VBANK(0, value, 0); break;
    case 0xA001: VRC_VBANK(0, value, 1); break;
    case 0xA002: VRC_VBANK(1, value, 0); break;
    case 0xA003: VRC_VBANK(1, value, 1); break;
    case 0xB000: VRC_VBANK(2, value, 0); break;
    case 0xB001: VRC_VBANK(2, value, 1); break;
    case 0xB002: VRC_VBANK(3, value, 0); break;
    case 0xB003: VRC_VBANK(3, value, 1); break;
    case 0xC000: VRC_VBANK(4, value, 0); break;
    case 0xC001: VRC_VBANK(4, value, 1); break;
    case 0xC002: VRC_VBANK(5, value, 0); break;
    case 0xC003: VRC_VBANK(5, value, 1); break;
    case 0xD000: VRC_VBANK(6, value, 0); break;
    case 0xD001: VRC_VBANK(6, value, 1); break;
    case 0xD002: VRC_VBANK(7, value, 0); break;
    case 0xD003: VRC_VBANK(7, value, 1); break;
    case 0xE000:
      irq18.nybbles[0] = value & 0x0F;
      irq18.clockticks = (irq18.nybbles[0]) | (irq18.nybbles[1] << 4) |
                         (irq18.nybbles[2] << 8) | (irq18.nybbles[3] << 12);
      irq18.counter = (uint8_t)(irq18.clockticks / 114);
      if (irq18.counter > 15) irq18.counter -= 16;
      break;
    case 0xE001:
      irq18.nybbles[1] = value & 0x0F;
      irq18.clockticks = (irq18.nybbles[0]) | (irq18.nybbles[1] << 4) |
                         (irq18.nybbles[2] << 8) | (irq18.nybbles[3] << 12);
      irq18.counter = (uint8_t)(irq18.clockticks / 114);
      if (irq18.counter > 15) irq18.counter -= 16;
      break;
    case 0xE002:
      irq18.nybbles[2] = value & 0x0F;
      irq18.clockticks = (irq18.nybbles[0]) | (irq18.nybbles[1] << 4) |
                         (irq18.nybbles[2] << 8) | (irq18.nybbles[3] << 12);
      irq18.counter = (uint8_t)(irq18.clockticks / 114);
      if (irq18.counter > 15) irq18.counter -= 16;
      break;
    case 0xE003:
      irq18.nybbles[3] = value & 0x0F;
      irq18.clockticks = (irq18.nybbles[0]) | (irq18.nybbles[1] << 4) |
                         (irq18.nybbles[2] << 8) | (irq18.nybbles[3] << 12);
      irq18.counter = (uint8_t)(irq18.clockticks / 114);
      if (irq18.counter > 15) irq18.counter -= 16;
      break;
    case 0xF000:
      if (value & 0x01) irq18.enabled = true;
      break;
    case 0xF001:
      irq18.enabled = value & 0x01;
      break;
    case 0xF002:
      switch (value & 0x03)
      {
        case 0:  ppu_mirror(0, 0, 1, 1); break;
        case 1:  ppu_mirror(0, 1, 0, 1); break;
        case 2:  ppu_mirror(1, 1, 1, 1); break;
        case 3:  ppu_mirror(1, 1, 1, 1); break; // should this be zero?
        default: break;
      }
      break;
    default:
      break;
  }
}


 map_memwrite map18_memwrite[] =
{
  { 0x8000, 0xFFFF, map18_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

 void map18_getstate(SnssMapperBlock *state);
 void map18_getstate(SnssMapperBlock *state)
{
  state->extraData.mapper18.irqCounterLowByte = irq18.counter & 0xFF;
  state->extraData.mapper18.irqCounterHighByte = irq18.counter >> 8;
  state->extraData.mapper18.irqCounterEnabled = irq18.enabled;
}

 void map18_setstate(SnssMapperBlock *state);
 void map18_setstate(SnssMapperBlock *state)
{
  irq18.counter = (state->extraData.mapper18.irqCounterHighByte << 8)
                  | state->extraData.mapper18.irqCounterLowByte;
  irq18.enabled = state->extraData.mapper18.irqCounterEnabled;
}

mapintf_t map18_intf =
{
  18, /* mapper number */
  (char*)"Jaleco SS8806", /* mapper name */
  map18_init, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  map18_getstate, /* get state (snss) */
  map18_setstate, /* set state (snss) */
  NULL, /* memory read structure */
  map18_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER19
/* Special mirroring macro for mapper 19 */
#define N_BANK1(table, value) \
  { \
    if ((value) < 0xE0) \
      ppu_setpage(1, (table) + 8, &mmc_getinfo()->vrom[((value) % (mmc_getinfo()->vrom_banks * 8)) << 10] - (0x2000 + ((table) << 10))); \
    else \
      ppu_setpage(1, (table) + 8, &mmc_getinfo()->vram[((value) & 7) << 10] - (0x2000 + ((table) << 10))); \
    ppu_mirrorhipages(); \
  }

 struct
{
  int counter, enabled;
} irq19;

 void map19_init(void)
{
  irq19.counter = irq19.enabled = 0;
}

/* mapper 19: Namcot 106 */
 void map19_write(uint32_t address, uint8_t value)
{
  int reg = address >> 11;
  switch (reg)
  {
    case 0xA:
      irq19.counter &= ~0xFF;
      irq19.counter |= value;
      break;

    case 0xB:
      irq19.counter = ((value & 0x7F) << 8) | (irq19.counter & 0xFF);
      irq19.enabled = (value & 0x80) ? true : false;
      break;

    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
      mmc_bankvrom(1, (reg & 7) << 10, value);
      break;

    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
      N_BANK1(reg & 3, value);
      break;

    case 0x1C:
      mmc_bankrom(8, 0x8000, value);
      break;

    case 0x1D:
      mmc_bankrom(8, 0xA000, value);
      break;

    case 0x1E:
      mmc_bankrom(8, 0xC000, value);
      break;

    default:
      break;
  }
}

 void map19_getstate(SnssMapperBlock *state);
 void map19_getstate(SnssMapperBlock *state)
{
  state->extraData.mapper19.irqCounterLowByte = irq19.counter & 0xFF;
  state->extraData.mapper19.irqCounterHighByte = irq19.counter >> 8;
  state->extraData.mapper19.irqCounterEnabled = irq19.enabled;
}

 void map19_setstate(SnssMapperBlock *state);
 void map19_setstate(SnssMapperBlock *state)
{
  irq19.counter = (state->extraData.mapper19.irqCounterHighByte << 8)
                  | state->extraData.mapper19.irqCounterLowByte;
  irq19.enabled = state->extraData.mapper19.irqCounterEnabled;
}

 map_memwrite map19_memwrite[] =
{
  { 0x5000, 0x5FFF, map19_write },
  { 0x8000, 0xFFFF, map19_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map19_intf =
{
  19, /* mapper number */
  (char*)"Namcot 106", /* mapper name */
  map19_init, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  map19_getstate, /* get state (snss) */
  map19_setstate, /* set state (snss) */
  NULL, /* memory read structure */
  map19_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER24
typedef struct vrcvirectangle_s
{
  bool enabled;

  uint8_t reg[3];

  float accum;
  uint8_t adder;

  int32_t freq;
  int32_t volume;
  uint8_t duty_flip;
} vrcvirectangle_t;

typedef struct vrcvisawtooth_s
{
  bool enabled;

  uint8_t reg[3];

  float accum;
  uint8_t adder;
  uint8_t output_acc;

  int32_t freq;
  uint8_t volume;
} vrcvisawtooth_t;

typedef struct vrcvisnd_s
{
  vrcvirectangle_t rectangle[2];
  vrcvisawtooth_t saw;
  float incsize;
} vrcvisnd_t;


 vrcvisnd_t vrcvi;

/* VRCVI rectangle wave generation */
 int32_t vrcvi_rectangle(vrcvirectangle_t *chan);
 int32_t vrcvi_rectangle(vrcvirectangle_t *chan)
{
  /* reg0: 0-3=volume, 4-6=duty cycle
  ** reg1: 8 bits of freq
  ** reg2: 0-3=high freq, 7=enable
  */

  chan->accum -= vrcvi.incsize; /* # of clocks per wave cycle */
  while (chan->accum < 0)
  {
    chan->accum += chan->freq;
    chan->adder = (chan->adder + 1) & 0x0F;
  }

  /* return if not enabled */
  if (false == chan->enabled)
    return 0;

  if (chan->adder < chan->duty_flip)
    return -(chan->volume);
  else
    return chan->volume;
}

/* VRCVI sawtooth wave generation */
 int32_t vrcvi_sawtooth(vrcvisawtooth_t *chan);
 int32_t vrcvi_sawtooth(vrcvisawtooth_t *chan)
{
  /* reg0: 0-5=phase accumulator bits
  ** reg1: 8 bits of freq
  ** reg2: 0-3=high freq, 7=enable
  */

  chan->accum -= vrcvi.incsize; /* # of clocks per wav cycle */
  while (chan->accum < 0)
  {
    chan->accum += chan->freq;
    chan->output_acc += chan->volume;

    chan->adder++;
    if (7 == chan->adder)
    {
      chan->adder = 0;
      chan->output_acc = 0;
    }
  }

  /* return if not enabled */
  if (false == chan->enabled)
    return 0;

  return (chan->output_acc >> 3) << 9;
}

/* mix vrcvi sound channels together */
 int32_t vrcvi_process(void)
{
  int32_t output;

  output = vrcvi_rectangle(&vrcvi.rectangle[0]);
  output += vrcvi_rectangle(&vrcvi.rectangle[1]);
  output += vrcvi_sawtooth(&vrcvi.saw);

  return output;
}

/* write to registers */
 void vrcvi_write(uint32_t address, uint8_t value)
{
  int chan = (address >> 12) - 9;

  switch (address & 0xB003)
  {
    case 0x9000:
    case 0xA000:
      vrcvi.rectangle[chan].reg[0] = value;
      vrcvi.rectangle[chan].volume = (value & 0x0F) << 8;
      vrcvi.rectangle[chan].duty_flip = (value >> 4) + 1;
      break;

    case 0x9001:
    case 0xA001:
      vrcvi.rectangle[chan].reg[1] = value;
      vrcvi.rectangle[chan].freq = ((vrcvi.rectangle[chan].reg[2] & 0x0F) << 8) + value + 1;
      break;

    case 0x9002:
    case 0xA002:
      vrcvi.rectangle[chan].reg[2] = value;
      vrcvi.rectangle[chan].freq = ((value & 0x0F) << 8) + vrcvi.rectangle[chan].reg[1] + 1;
      vrcvi.rectangle[chan].enabled = (value & 0x80) ? true : false;
      break;

    case 0xB000:
      vrcvi.saw.reg[0] = value;
      vrcvi.saw.volume = value & 0x3F;
      break;

    case 0xB001:
      vrcvi.saw.reg[1] = value;
      vrcvi.saw.freq = (((vrcvi.saw.reg[2] & 0x0F) << 8) + value + 1) << 1;
      break;

    case 0xB002:
      vrcvi.saw.reg[2] = value;
      vrcvi.saw.freq = (((value & 0x0F) << 8) + vrcvi.saw.reg[1] + 1) << 1;
      vrcvi.saw.enabled = (value & 0x80) ? true : false;
      break;

    default:
      break;
  }
}

/* reset state of vrcvi sound channels */
 void vrcvi_reset(void)
{
  int i;
  apu_t apu;

  /* get the phase period from the apu */
  apu_getcontext(&apu);
  vrcvi.incsize = apu.cycle_rate;

  /* preload regs */
  for (i = 0; i < 3; i++)
  {
    vrcvi_write(0x9000 + i, 0);
    vrcvi_write(0xA000 + i, 0);
    vrcvi_write(0xB000 + i, 0);
  }
}

 apu_memwrite vrcvi_memwrite[] =
{
  { 0x9000, 0x9002, vrcvi_write }, /* vrc6 */
  { 0xA000, 0xA002, vrcvi_write },
  { 0xB000, 0xB002, vrcvi_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

apuext_t vrcvi_ext =
{
  NULL, /* no init */
  NULL, /* no shutdown */
  vrcvi_reset,
  vrcvi_process,
  NULL, /* no reads */
  vrcvi_memwrite
};

/////////////////////////////

 struct
{
  int counter, enabled;
  int latch, wait_state;
} irq24;

 void map24_init(void)
{
  irq24.counter = irq24.enabled = 0;
  irq24.latch = irq24.wait_state = 0;
}

 void map24_hblank(int vblank)
{
  UNUSED(vblank);

  if (irq24.enabled)
  {
    if (256 == ++irq24.counter)
    {
      irq24.counter = irq24.latch;
      nes_irq();
      //irq.enabled = false;
      irq24.enabled = irq24.wait_state;
    }
  }
}

 void map24_write(uint32_t address, uint8_t value)
{
  switch (address & 0xF003)
  {
    case 0x8000:
      mmc_bankrom(16, 0x8000, value);
      break;

    case 0x9003:
      /* ??? */
      break;

    case 0xB003:
      switch (value & 0x0C)
      {
        case 0x00:
          ppu_mirror(0, 1, 0, 1); /* vertical */
          break;

        case 0x04:
          ppu_mirror(0, 0, 1, 1); /* horizontal */
          break;

        case 0x08:
          ppu_mirror(0, 0, 0, 0);
          break;

        case 0x0C:
          ppu_mirror(1, 1, 1, 1);
          break;

        default:
          break;
      }
      break;


    case 0xC000:
      mmc_bankrom(8, 0xC000, value);
      break;

    case 0xD000:
      mmc_bankvrom(1, 0x0000, value);
      break;

    case 0xD001:
      mmc_bankvrom(1, 0x0400, value);
      break;

    case 0xD002:
      mmc_bankvrom(1, 0x0800, value);
      break;

    case 0xD003:
      mmc_bankvrom(1, 0x0C00, value);
      break;

    case 0xE000:
      mmc_bankvrom(1, 0x1000, value);
      break;

    case 0xE001:
      mmc_bankvrom(1, 0x1400, value);
      break;

    case 0xE002:
      mmc_bankvrom(1, 0x1800, value);
      break;

    case 0xE003:
      mmc_bankvrom(1, 0x1C00, value);
      break;

    case 0xF000:
      irq24.latch = value;
      break;

    case 0xF001:
      irq24.enabled = (value >> 1) & 0x01;
      irq24.wait_state = value & 0x01;
      if (irq24.enabled)
        irq24.counter = irq24.latch;
      break;

    case 0xF002:
      irq24.enabled = irq24.wait_state;
      break;

    default:
      break;
  }
}

 void map24_getstate(SnssMapperBlock *state);
 void map24_getstate(SnssMapperBlock *state)
{
  state->extraData.mapper24.irqCounter = irq24.counter;
  state->extraData.mapper24.irqCounterEnabled = irq24.enabled;
}

 void map24_setstate(SnssMapperBlock *state);
 void map24_setstate(SnssMapperBlock *state)
{
  irq24.counter = state->extraData.mapper24.irqCounter;
  irq24.enabled = state->extraData.mapper24.irqCounterEnabled;
}

 map_memwrite map24_memwrite[] =
{
  { 0x8000, 0xF002, map24_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map24_intf =
{
  24, /* mapper number */
  (char*)"Konami VRC6", /* mapper name */
  map24_init, /* init routine */
  NULL, /* vblank callback */
  map24_hblank, /* hblank callback */
  map24_getstate, /* get state (snss) */
  map24_setstate, /* set state (snss) */
  NULL, /* memory read structure */
  map24_memwrite, /* memory write structure */
  &vrcvi_ext /* external sound device */
};

//................................................................................
//MAPPER32
 int select_c000 = 0;

/* mapper 32: Irem G-101 */
 void map32_write(uint32_t address, uint8_t value)
{
  switch (address >> 12)
  {
    case 0x08:
      if (select_c000)
        mmc_bankrom(8, 0xC000, value);
      else
        mmc_bankrom(8, 0x8000, value);
      break;

    case 0x09:
      if (value & 1)
        ppu_mirror(0, 0, 1, 1); /* horizontal */
      else
        ppu_mirror(0, 1, 0, 1); /* vertical */

      select_c000 = (value & 0x02);
      break;

    case 0x0A:
      mmc_bankrom(8, 0xA000, value);
      break;

    case 0x0B:
      {
        int loc = (address & 0x07) << 10;
        mmc_bankvrom(1, loc, value);
      }
      break;

    default:
      break;
  }
}

 map_memwrite map32_memwrite[] =
{
  { 0x8000, 0xFFFF, map32_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map32_intf =
{
  32, /* mapper number */
  (char*)"Irem G-101", /* mapper name */
  NULL, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map32_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER33
/* mapper 33: Taito TC0190*/
 void map33_write(uint32_t address, uint8_t value)
{
  int page = (address >> 13) & 3;
  int reg = address & 3;

  switch (page)
  {
    case 0: /* $800X */
      switch (reg)
      {
        case 0:
          mmc_bankrom(8, 0x8000, value);
          break;

        case 1:
          mmc_bankrom(8, 0xA000, value);
          break;

        case 2:
          mmc_bankvrom(2, 0x0000, value);
          break;

        case 3:
          mmc_bankvrom(2, 0x0800, value);
          break;
      }
      break;

    case 1: /* $A00X */
      {
        int loc = 0x1000 + (reg << 10);
        mmc_bankvrom(1, loc, value);
      }
      break;

    case 2: /* $C00X */
    case 3: /* $E00X */
      switch (reg)
      {
        case 0:
        /* irqs maybe ? */
        //break;

        case 1:
          /* this doesn't seem to work just right */
          if (value & 1)
            ppu_mirror(0, 0, 1, 1); /* horizontal */
          else
            ppu_mirror(0, 1, 0, 1);
          break;

        default:
          break;
      }
      break;
  }
}


 map_memwrite map33_memwrite[] =
{
  { 0x8000, 0xFFFF, map33_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map33_intf =
{
  33, /* mapper number */
  (char*)"Taito TC0190", /* mapper name */
  NULL, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map33_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER34
 void map34_init(void)
{
  mmc_bankrom(32, 0x8000, MMC_LASTBANK);
}

 void map34_write(uint32_t address, uint8_t value)
{
  if ((address & 0x8000) || (0x7FFD == address))
  {
    mmc_bankrom(32, 0x8000, value);
  }
  else if (0x7FFE == address)
  {
    mmc_bankvrom(4, 0x0000, value);
  }
  else if (0x7FFF == address)
  {
    mmc_bankvrom(4, 0x1000, value);
  }
}

 map_memwrite map34_memwrite[] =
{
  { 0x7FFD, 0xFFFF, map34_write },
  { (uint32_t)-1, (uint32_t)-1, NULL }
};

mapintf_t map34_intf =
{
  34, /* mapper number */
  (char*)"Nina-1", /* mapper name */
  map34_init, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map34_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER40
//mapper40:
#define  MAP40_IRQ_PERIOD  (4096 / 113.666666)

 struct
{
  int enabled, counter;
} irq40;

/* mapper 40: SMB 2j (hack) */
 void map40_init(void)
{
  mmc_bankrom(8, 0x6000, 6);
  mmc_bankrom(8, 0x8000, 4);
  mmc_bankrom(8, 0xA000, 5);
  mmc_bankrom(8, 0xE000, 7);

  irq40.enabled = false;
  irq40.counter = (int) MAP40_IRQ_PERIOD;
}

 void map40_hblank(int vblank)
{
  UNUSED(vblank);

  if (irq40.enabled && irq40.counter)
  {
    irq40.counter--;
    if (0 == irq40.counter)
    {
      nes_irq();
      irq40.enabled = false;
    }
  }
}

 void map40_write(uint32_t address, uint8_t value)
{
  int range = (address >> 13) - 4;

  switch (range)
  {
    case 0: /* 0x8000-0x9FFF */
      irq40.enabled = false;
      irq40.counter = (int) MAP40_IRQ_PERIOD;
      break;

    case 1: /* 0xA000-0xBFFF */
      irq40.enabled = true;
      break;

    case 3: /* 0xE000-0xFFFF */
      mmc_bankrom(8, 0xC000, value & 7);
      break;

    default:
      break;
  }
}

 void map40_getstate(SnssMapperBlock *state);
 void map40_getstate(SnssMapperBlock *state)
{
  state->extraData.mapper40.irqCounter = irq40.counter;
  state->extraData.mapper40.irqCounterEnabled = irq40.enabled;
}

 void map40_setstate(SnssMapperBlock *state);
 void map40_setstate(SnssMapperBlock *state)
{
  irq40.counter = state->extraData.mapper40.irqCounter;
  irq40.enabled = state->extraData.mapper40.irqCounterEnabled;
}

 map_memwrite map40_memwrite[] =
{
  { 0x8000, 0xFFFF, map40_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map40_intf =
{
  40, /* mapper number */
  (char*)"SMB 2j (pirate)", /* mapper name */
  map40_init, /* init routine */
  NULL, /* vblank callback */
  map40_hblank, /* hblank callback */
  map40_getstate, /* get state (snss) */
  map40_setstate, /* set state (snss) */
  NULL, /* memory read structure */
  map40_memwrite, /* memory write structure */
  NULL /* external sound device */
};

//................................................................................
//MAPPER41
 uint8_t register_low;
 uint8_t register_high;

/*****************************************************/
/* Set 8K CHR bank from the combined register values */
/*****************************************************/
 void map41_set_chr (void)
{
  /* Set the CHR bank from the appropriate register bits */
  mmc_bankvrom (8, 0x0000, ((register_low >> 1) & 0x0C) | (register_high));

  /* Done */
  return;
}

/******************************/
/* Mapper #41: Caltron 6 in 1 */
/******************************/
 void map41_init (void)
{
  /* Both registers set to zero at power on */
  /* TODO: Registers should also be cleared on a soft reset */
  register_low = 0x00;
  register_high = 0x00;
  mmc_bankrom (32, 0x8000, 0x00);
  map41_set_chr ();

  /* Done */
  return;
}

/******************************************/
/* Mapper #41 write handler ($6000-$67FF) */
/******************************************/
 void map41_low_write (uint32_t address, uint8_t value)
{
  /* Within this range the value written is irrelevant */
  UNUSED (value);

  /* $6000-$67FF: A5    = mirroring (1=horizontal, 0=vertical)      */
  /*              A4-A3 = high two bits of 8K CHR bank              */
  /*              A2    = register 1 enable (0=disabled, 1=enabled) */
  /*              A2-A0 = 32K PRG bank                              */
  register_low = (uint8_t) (address & 0x3F);
  mmc_bankrom (32, 0x8000, register_low & 0x07);
  map41_set_chr ();
  if (register_low & 0x20) ppu_mirror(0, 0, 1, 1); /* horizontal */
  else                     ppu_mirror(0, 1, 0, 1); /* vertical */

  /* Done */
  return;
}

/******************************************/
/* Mapper #41 write handler ($8000-$FFFF) */
/******************************************/
 void map41_high_write (uint32_t address, uint8_t value)
{
  /* Address doesn't matter within this range */
  UNUSED (address);

  /* $8000-$FFFF: D1-D0 = low two bits of 8K CHR bank */
  if (register_low & 0x04)
  {
    register_high = value & 0x03;
    map41_set_chr ();
  }

  /* Done */
  return;
}

/****************************************************/
/* Shove extra mapper information into a SNSS block */
/****************************************************/
 void map41_setstate (SnssMapperBlock *state);
 void map41_setstate (SnssMapperBlock *state)
{
  /* TODO: Store SNSS information */
  UNUSED (state);

  /* Done */
  return;
}

/*****************************************************/
/* Pull extra mapper information out of a SNSS block */
/*****************************************************/
 void map41_getstate (SnssMapperBlock *state);
 void map41_getstate (SnssMapperBlock *state)
{
  /* TODO: Retrieve SNSS information */
  UNUSED (state);

  /* Done */
  return;
}

 map_memwrite map41_memwrite [] =
{
  { 0x6000, 0x67FF, map41_low_write },
  { 0x8000, 0xFFFF, map41_high_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map41_intf =
{
  41,                               /* Mapper number */
  (char*)"Caltron 6 in 1",                 /* Mapper name */
  map41_init,                       /* Initialization routine */
  NULL,                             /* VBlank callback */
  NULL,                             /* HBlank callback */
  map41_getstate,                   /* Get state (SNSS) */
  map41_setstate,                   /* Set state (SNSS) */
  NULL,                             /* Memory read structure */
  map41_memwrite,                   /* Memory write structure */
  NULL                              /* External sound device */
};
//................................................................................
//MAPPER42
 struct
{
  bool enabled;
  uint32_t counter;
} irq42;

/********************************/
/* Mapper #42 IRQ reset routine */
/********************************/
 void map42_irq_reset (void)
{
  /* Turn off IRQs */
  irq42.enabled = false;
  irq42.counter = 0x0000;

  /* Done */
  return;
}

/********************************************/
/* Mapper #42: Baby Mario bootleg cartridge */
/********************************************/
 void map42_init (void)
{
  /* Set the hardwired pages */
  mmc_bankrom (8, 0x8000, 0x0C);
  mmc_bankrom (8, 0xA000, 0x0D);
  mmc_bankrom (8, 0xC000, 0x0E);
  mmc_bankrom (8, 0xE000, 0x0F);

  /* Reset the IRQ counter */
  map42_irq_reset ();

  /* Done */
  return;
}

/****************************************/
/* Mapper #42 callback for IRQ handling */
/****************************************/
 void map42_hblank (int vblank)
{
  /* Counter is M2 based so it doesn't matter whether */
  /* the PPU is in its VBlank period or not           */
  UNUSED(vblank);

  /* Increment the counter if it is enabled and check for strike */
  if (irq42.enabled)
  {
    /* Is there a constant for cycles per scanline? */
    /* If so, someone ought to substitute it here   */
    irq42.counter = irq42.counter + 114;

    /* IRQ is triggered after 24576 M2 cycles */
    if (irq42.counter >= 0x6000)
    {
      /* Trigger the IRQ */
      nes_irq ();

      /* Reset the counter */
      map42_irq_reset ();
    }
  }
}

/******************************************/
/* Mapper #42 write handler ($E000-$FFFF) */
/******************************************/
 void map42_write (uint32_t address, uint8_t value)
{
  switch (address & 0x03)
  {
    /* Register 0: Select ROM page at $6000-$7FFF */
    case 0x00: mmc_bankrom (8, 0x6000, value & 0x0F);
      break;

    /* Register 1: mirroring */
    case 0x01: if (value & 0x08) ppu_mirror(0, 0, 1, 1); /* horizontal */
      else              ppu_mirror(0, 1, 0, 1); /* vertical   */
      break;

    /* Register 2: IRQ */
    case 0x02: if (value & 0x02) irq42.enabled = true;
      else              map42_irq_reset ();
      break;

    /* Register 3: unused */
    default:   break;
  }

  /* Done */
  return;
}

/****************************************************/
/* Shove extra mapper information into a SNSS block */
/****************************************************/
 void map42_setstate (SnssMapperBlock *state);
 void map42_setstate (SnssMapperBlock *state)
{
  /* TODO: Store SNSS information */
  UNUSED (state);

  /* Done */
  return;
}

/*****************************************************/
/* Pull extra mapper information out of a SNSS block */
/*****************************************************/
 void map42_getstate (SnssMapperBlock *state);
 void map42_getstate (SnssMapperBlock *state)
{
  /* TODO: Retrieve SNSS information */
  UNUSED (state);

  /* Done */
  return;
}

 map_memwrite map42_memwrite [] =
{
  { 0xE000, 0xFFFF, map42_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map42_intf =
{
  42,                               /* Mapper number */
  (char*)"Baby Mario (bootleg)",           /* Mapper name */
  map42_init,                       /* Initialization routine */
  NULL,                             /* VBlank callback */
  map42_hblank,                     /* HBlank callback */
  map42_getstate,                   /* Get state (SNSS) */
  map42_setstate,                   /* Set state (SNSS) */
  NULL,                             /* Memory read structure */
  map42_memwrite,                   /* Memory write structure */
  NULL                              /* External sound device */
};
//................................................................................
//MAPPER46
 uint8_t prg_low_bank;
 uint8_t chr_low_bank;
 uint8_t prg_high_bank;
 uint8_t chr_high_bank;

/*************************************************/
/* Set banks from the combined register values   */
/*************************************************/
 void map46_set_banks (void)
{
  /* Set the PRG and CHR pages */
  mmc_bankrom (32, 0x8000, (prg_high_bank << 1) | (prg_low_bank));
  mmc_bankvrom (8, 0x0000, (chr_high_bank << 3) | (chr_low_bank));

  /* Done */
  return;
}

/*********************************************************/
/* Mapper #46: Pelican Game Station (aka Rumble Station) */
/*********************************************************/
 void map46_init (void)
{
  /* High bank switch register is set to zero on reset */
  prg_high_bank = 0x00;
  chr_high_bank = 0x00;
  map46_set_banks ();

  /* Done */
  return;
}

/******************************************/
/* Mapper #46 write handler ($6000-$FFFF) */
/******************************************/
 void map46_write (uint32_t address, uint8_t value)
{
  /* $8000-$FFFF: D6-D4 = lower three bits of CHR bank */
  /*              D0    = low bit of PRG bank          */
  /* $6000-$7FFF: D7-D4 = high four bits of CHR bank   */
  /*              D3-D0 = high four bits of PRG bank   */
  if (address & 0x8000)
  {
    prg_low_bank = value & 0x01;
    chr_low_bank = (value >> 4) & 0x07;
    map46_set_banks ();
  }
  else
  {
    prg_high_bank = value & 0x0F;
    chr_high_bank = (value >> 4) & 0x0F;
    map46_set_banks ();
  }

  /* Done */
  return;
}

/****************************************************/
/* Shove extra mapper information into a SNSS block */
/****************************************************/
 void map46_setstate (SnssMapperBlock *state);
 void map46_setstate (SnssMapperBlock *state)
{
  /* TODO: Store SNSS information */
  UNUSED (state);

  /* Done */
  return;
}

/*****************************************************/
/* Pull extra mapper information out of a SNSS block */
/*****************************************************/
 void map46_getstate (SnssMapperBlock *state);
 void map46_getstate (SnssMapperBlock *state)
{
  /* TODO: Retrieve SNSS information */
  UNUSED (state);

  /* Done */
  return;
}

 map_memwrite map46_memwrite [] =
{
  { 0x6000, 0xFFFF, map46_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map46_intf =
{
  46,                               /* Mapper number */
  (char*)"Pelican Game Station",           /* Mapper name */
  map46_init,                       /* Initialization routine */
  NULL,                             /* VBlank callback */
  NULL,                             /* HBlank callback */
  map46_getstate,                   /* Get state (SNSS) */
  map46_setstate,                   /* Set state (SNSS) */
  NULL,                             /* Memory read structure */
  map46_memwrite,                   /* Memory write structure */
  NULL                              /* External sound device */
};
//................................................................................
//MAPPER50
 struct
{
  bool enabled;
  uint32_t counter;
} irq50;

/********************************/
/* Mapper #50 IRQ reset routine */
/********************************/
 void map50_irq_reset (void)
{
  /* Turn off IRQs */
  irq50.enabled = false;
  irq50.counter = 0x0000;

  /* Done */
  return;
}

/**************************************************************/
/* Mapper #50: 3rd discovered variation of SMB2j cart bootleg */
/**************************************************************/
 void map50_init (void)
{
  /* Set the hardwired pages */
  mmc_bankrom (8, 0x6000, 0x0F);
  mmc_bankrom (8, 0x8000, 0x08);
  mmc_bankrom (8, 0xA000, 0x09);
  mmc_bankrom (8, 0xE000, 0x0B);

  /* Reset the IRQ counter */
  map50_irq_reset ();

  /* Done */
  return;
}

/****************************************/
/* Mapper #50 callback for IRQ handling */
/****************************************/
 void map50_hblank (int vblank)
{
  /* Counter is M2 based so it doesn't matter whether */
  /* the PPU is in its VBlank period or not           */
  UNUSED(vblank);

  /* Increment the counter if it is enabled and check for strike */
  if (irq50.enabled)
  {
    /* Is there a constant for cycles per scanline? */
    /* If so, someone ought to substitute it here   */
    irq50.counter = irq50.counter + 114;

    /* IRQ line is hooked to Q12 of the counter */
    if (irq50.counter & 0x1000)
    {
      /* Trigger the IRQ */
      nes_irq ();

      /* Reset the counter */
      map50_irq_reset ();
    }
  }
}

/******************************************/
/* Mapper #50 write handler ($4000-$5FFF) */
/******************************************/
 void map50_write (uint32_t address, uint8_t value)
{
  uint8_t selectable_bank;

  /* For address to be decoded, A5 must be high and A6 low */
  if ((address & 0x60) != 0x20) return;

  /* A8 low  = $C000-$DFFF page selection */
  /* A8 high = IRQ timer toggle */
  if (address & 0x100)
  {
    /* IRQ settings */
    if (value & 0x01) irq50.enabled = true;
    else              map50_irq_reset ();
  }
  else
  {
    /* Stupid data line swapping */
    selectable_bank = 0x00;
    if (value & 0x08) selectable_bank |= 0x08;
    if (value & 0x04) selectable_bank |= 0x02;
    if (value & 0x02) selectable_bank |= 0x01;
    if (value & 0x01) selectable_bank |= 0x04;
    mmc_bankrom (8, 0xC000, selectable_bank);
  }

  /* Done */
  return;
}

/****************************************************/
/* Shove extra mapper information into a SNSS block */
/****************************************************/
 void map50_setstate (SnssMapperBlock *state);
 void map50_setstate (SnssMapperBlock *state)
{
  /* TODO: Store SNSS information */
  UNUSED (state);

  /* Done */
  return;
}

/*****************************************************/
/* Pull extra mapper information out of a SNSS block */
/*****************************************************/
 void map50_getstate (SnssMapperBlock *state);
 void map50_getstate (SnssMapperBlock *state)
{
  /* TODO: Retrieve SNSS information */
  UNUSED (state);

  /* Done */
  return;
}

 map_memwrite map50_memwrite [] =
{
  { 0x4000, 0x5FFF, map50_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map50_intf =
{
  50,                               /* Mapper number */
  (char*)"SMB2j (3rd discovered variant)", /* Mapper name */
  map50_init,                       /* Initialization routine */
  NULL,                             /* VBlank callback */
  map50_hblank,                     /* HBlank callback */
  map50_getstate,                   /* Get state (SNSS) */
  map50_setstate,                   /* Set state (SNSS) */
  NULL,                             /* Memory read structure */
  map50_memwrite,                   /* Memory write structure */
  NULL                              /* External sound device */
};
//................................................................................
//MAPPER64
 struct
{
  int counter, latch;
  bool enabled, reset;
} irq64;

 uint8_t _command = 0;
 uint16_t _vrombase = 0x0000;

 void map64_hblank(int vblank)
{
  if (vblank)
    return;

  irq64.reset = false;

  if (ppu_enabled())
  {
    if (0 == irq64.counter--)
    {
      irq64.counter = irq64.latch;

      if (true == irq64.enabled)
        nes_irq();

      irq64.reset = true;
    }
  }
}

/* mapper 64: Tengen RAMBO-1 */
 void map64_write(uint32_t address, uint8_t value)
{
  switch (address & 0xE001)
  {
    case 0x8000:
      _command = value;
      _vrombase = (value & 0x80) ? 0x1000 : 0x0000;
      break;

    case 0x8001:
      switch (_command & 0xF)
      {
        case 0:
          mmc_bankvrom(1, 0x0000 ^ _vrombase, value);
          mmc_bankvrom(1, 0x0400 ^ _vrombase, value);
          break;

        case 1:
          mmc_bankvrom(1, 0x0800 ^ _vrombase, value);
          mmc_bankvrom(1, 0x0C00 ^ _vrombase, value);
          break;

        case 2:
          mmc_bankvrom(1, 0x1000 ^ _vrombase, value);
          break;

        case 3:
          mmc_bankvrom(1, 0x1400 ^ _vrombase, value);
          break;

        case 4:
          mmc_bankvrom(1, 0x1800 ^ _vrombase, value);
          break;

        case 5:
          mmc_bankvrom(1, 0x1C00 ^ _vrombase, value);
          break;

        case 6:
          mmc_bankrom(8, (_command & 0x40) ? 0xA000 : 0x8000, value);
          break;

        case 7:
          mmc_bankrom(8, (_command & 0x40) ? 0xC000 : 0xA000, value);
          break;

        case 8:
          mmc_bankvrom(1, 0x0400, value);
          break;

        case 9:
          mmc_bankvrom(1, 0x0C00, value);
          break;

        case 15:
          mmc_bankrom(8, (_command & 0x40) ? 0x8000 : 0xC000, value);
          break;

        default:
          break;
      }
      break;

    case 0xA000:
      if (value & 1)
        ppu_mirror(0, 0, 1, 1);
      else
        ppu_mirror(0, 1, 0, 1);
      break;

    case 0xC000:
      //irq64.counter = value;
      irq64.latch = value;
      break;

    case 0xC001:
      //irq64.latch = value;
      irq64.reset = true;
      break;

    case 0xE000:
      //irq64.counter = irq64.latch;
      irq64.enabled = false;
      break;

    case 0xE001:
      irq64.enabled = true;
      break;

    default:
      break;
  }

  if (true == irq64.reset)
    irq64.counter = irq64.latch;
}

 void map64_init(void)
{
  mmc_bankrom(8, 0x8000, MMC_LASTBANK);
  mmc_bankrom(8, 0xA000, MMC_LASTBANK);
  mmc_bankrom(8, 0xC000, MMC_LASTBANK);
  mmc_bankrom(8, 0xE000, MMC_LASTBANK);

  irq64.counter = irq64.latch = 0;
  irq64.reset = irq64.enabled = false;
}

 map_memwrite map64_memwrite[] =
{
  { 0x8000, 0xFFFF, map64_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map64_intf =
{
  64, /* mapper number */
  (char*)"Tengen RAMBO-1", /* mapper name */
  map64_init, /* init routine */
  NULL, /* vblank callback */
  map64_hblank, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map64_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER65
 struct
{
  int counter;
  bool enabled;
  int cycles;
  uint8_t low, high;
} irq65;

 void map65_init(void)
{
  irq65.counter = 0;
  irq65.enabled = false;
  irq65.low = irq65.high = 0;
  irq65.cycles = 0;
}

/* TODO: shouldn't there be some kind of HBlank callback??? */

/* mapper 65: Irem H-3001*/
 void map65_write(uint32_t address, uint8_t value)
{
  int range = address & 0xF000;
  int reg = address & 7;

  switch (range)
  {
    case 0x8000:
    case 0xA000:
    case 0xC000:
      mmc_bankrom(8, range, value);
      break;

    case 0xB000:
      mmc_bankvrom(1, reg << 10, value);
      break;

    case 0x9000:
      switch (reg)
      {
        case 4:
          irq65.enabled = (value & 0x01) ? false : true;
          break;

        case 5:
          irq65.high = value;
          irq65.cycles = (irq65.high << 8) | irq65.low;
          irq65.counter = (uint8_t)(irq65.cycles / 128);
          break;

        case 6:
          irq65.low = value;
          irq65.cycles = (irq65.high << 8) | irq65.low;
          irq65.counter = (uint8_t)(irq65.cycles / 128);
          break;

        default:
          break;
      }
      break;

    default:
      break;
  }
}

 map_memwrite map65_memwrite[] =
{
  { 0x8000, 0xFFFF, map65_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map65_intf =
{
  65, /* mapper number */
  (char*)"Irem H-3001", /* mapper name */
  map65_init, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map65_memwrite, /* memory write structure */
  NULL /* external sound device */
};

//................................................................................
//MAPPER66
/* mapper 66: GNROM */
 void map66_write(uint32_t address, uint8_t value)
{
  UNUSED(address);

  mmc_bankrom(32, 0x8000, (value >> 4) & 3);
  mmc_bankvrom(8, 0x0000, value & 3);
}

 void map66_init(void)
{
  mmc_bankrom(32, 0x8000, 0);
  mmc_bankvrom(8, 0x0000, 0);
}


 map_memwrite map66_memwrite[] =
{
  { 0x8000, 0xFFFF, map66_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map66_intf =
{
  66, /* mapper number */
  (char*)"GNROM", /* mapper name */
  map66_init, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map66_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER70
/* mapper 70: Arkanoid II, Kamen Rider Club, etc. */
/* ($8000-$FFFF) D6-D4 = switch $8000-$BFFF */
/* ($8000-$FFFF) D3-D0 = switch PPU $0000-$1FFF */
/* ($8000-$FFFF) D7 = switch mirroring */
 void map70_write(uint32_t address, uint8_t value)
{
  UNUSED(address);

  mmc_bankrom(16, 0x8000, (value >> 4) & 0x07);
  mmc_bankvrom(8, 0x0000, value & 0x0F);

  /* Argh! FanWen used the 4-screen bit to determine
  ** whether the game uses D7 to switch between
  ** horizontal and vertical mirroring, or between
  ** one-screen mirroring from $2000 or $2400.
  */
  if (mmc_getinfo()->flags & ROM_FLAG_FOURSCREEN)
  {
    if (value & 0x80)
      ppu_mirror(0, 0, 1, 1); /* horiz */
    else
      ppu_mirror(0, 1, 0, 1); /* vert */
  }
  else
  {
    int mirror = (value & 0x80) >> 7;
    ppu_mirror(mirror, mirror, mirror, mirror);
  }
}

 map_memwrite map70_memwrite[] =
{
  { 0x8000, 0xFFFF, map70_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map70_intf =
{
  70, /* mapper number */
  (char*)"Mapper 70", /* mapper name */
  NULL, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map70_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER73
 struct
{
  bool enabled;
  uint32_t counter;
} irq73;

/**************************/
/* Mapper #73: Salamander */
/**************************/
 void map73_init (void)
{
  /* Turn off IRQs */
  irq73.enabled = false;
  irq73.counter = 0x0000;

  /* Done */
  return;
}

/****************************************/
/* Mapper #73 callback for IRQ handling */
/****************************************/
 void map73_hblank (int vblank)
{
  /* Counter is M2 based so it doesn't matter whether */
  /* the PPU is in its VBlank period or not           */
  UNUSED (vblank);

  /* Increment the counter if it is enabled and check for strike */
  if (irq73.enabled)
  {
    /* Is there a constant for cycles per scanline? */
    /* If so, someone ought to substitute it here   */
    irq73.counter = irq73.counter + 114;

    /* Counter triggered on overflow into Q16 */
    if (irq73.counter & 0x10000)
    {
      /* Clip to sixteen-bit word */
      irq73.counter &= 0xFFFF;

      /* Trigger the IRQ */
      nes_irq ();

      /* Shut off IRQ counter */
      irq73.enabled = false;
    }
  }
}

/******************************************/
/* Mapper #73 write handler ($8000-$FFFF) */
/******************************************/
 void map73_write (uint32_t address, uint8_t value)
{
  switch (address & 0xF000)
  {
    case 0x8000: irq73.counter &= 0xFFF0;
      irq73.counter |= (uint32_t) (value);
      break;
    case 0x9000: irq73.counter &= 0xFF0F;
      irq73.counter |= (uint32_t) (value << 4);
      break;
    case 0xA000: irq73.counter &= 0xF0FF;
      irq73.counter |= (uint32_t) (value << 8);
      break;
    case 0xB000: irq73.counter &= 0x0FFF;
      irq73.counter |= (uint32_t) (value << 12);
      break;
    case 0xC000: if (value & 0x02) irq73.enabled = true;
      else              irq73.enabled = false;
      break;
    case 0xF000: mmc_bankrom (16, 0x8000, value);
    default:     break;
  }

  /* Done */
  return;
}

/****************************************************/
/* Shove extra mapper information into a SNSS block */
/****************************************************/
 void map73_setstate (SnssMapperBlock *state);
 void map73_setstate (SnssMapperBlock *state)
{
  /* TODO: Store SNSS information */
  UNUSED (state);

  /* Done */
  return;
}

/*****************************************************/
/* Pull extra mapper information out of a SNSS block */
/*****************************************************/
 void map73_getstate (SnssMapperBlock *state);
 void map73_getstate (SnssMapperBlock *state)
{
  /* TODO: Retrieve SNSS information */
  UNUSED (state);

  /* Done */
  return;
}

 map_memwrite map73_memwrite [] =
{
  { 0x8000, 0xFFFF, map73_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map73_intf =
{
  73,                               /* Mapper number */
  (char*)"Konami VRC3",                    /* Mapper name */
  map73_init,                       /* Initialization routine */
  NULL,                             /* VBlank callback */
  map73_hblank,                     /* HBlank callback */
  map73_getstate,                   /* Get state (SNSS) */
  map73_setstate,                   /* Set state (SNSS) */
  NULL,                             /* Memory read structure */
  map73_memwrite,                   /* Memory write structure */
  NULL                              /* External sound device */
};
//................................................................................
//MAPPER75
 uint8_t latch75[2];
 uint8_t hibits;

/* mapper 75: Konami VRC1 */
 void map75_write(uint32_t address, uint8_t value)
{
  switch ((address & 0xF000) >> 12)
  {
    case 0x8:
      mmc_bankrom(8, 0x8000, value);
      break;

    case 0x9:
      hibits = (value & 0x06);

      mmc_bankvrom(4, 0x0000, ((hibits & 0x02) << 3) | latch75[0]);
      mmc_bankvrom(4, 0x1000, ((hibits & 0x04) << 2) | latch75[1]);

      if (value & 1)
        ppu_mirror(0, 1, 0, 1); /* vert */
      else
        ppu_mirror(0, 0, 1, 1); /* horiz */

      break;

    case 0xA:
      mmc_bankrom(8, 0xA000, value);
      break;

    case 0xC:
      mmc_bankrom(8, 0xC000, value);
      break;

    case 0xE:
      latch75[0] = (value & 0x0F);
      mmc_bankvrom(4, 0x0000, ((hibits & 0x02) << 3) | latch75[0]);
      break;

    case 0xF:
      latch75[1] = (value & 0x0F);
      mmc_bankvrom(4, 0x1000, ((hibits & 0x04) << 2) | latch75[1]);
      break;

    default:
      break;
  }
}

 map_memwrite map75_memwrite[] =
{
  { 0x8000, 0xFFFF, map75_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map75_intf =
{
  75, /* mapper number */
  (char*)"Konami VRC1", /* mapper name */
  NULL, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map75_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER78
/* mapper 78: Holy Diver, Cosmo Carrier */
/* ($8000-$FFFF) D2-D0 = switch $8000-$BFFF */
/* ($8000-$FFFF) D7-D4 = switch PPU $0000-$1FFF */
/* ($8000-$FFFF) D3 = switch mirroring */
 void map78_write(uint32_t address, uint8_t value)
{
  UNUSED(address);

  mmc_bankrom(16, 0x8000, value & 7);
  mmc_bankvrom(8, 0x0000, (value >> 4) & 0x0F);

  /* Ugh! Same abuse of the 4-screen bit as with Mapper #70 */
  if (mmc_getinfo()->flags & ROM_FLAG_FOURSCREEN)
  {
    if (value & 0x08)
      ppu_mirror(0, 1, 0, 1); /* vert */
    else
      ppu_mirror(0, 0, 1, 1); /* horiz */
  }
  else
  {
    int mirror = (value >> 3) & 1;
    ppu_mirror(mirror, mirror, mirror, mirror);
  }
}

 map_memwrite map78_memwrite[] =
{
  { 0x8000, 0xFFFF, map78_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map78_intf =
{
  78, /* mapper number */
  (char*)"Mapper 78", /* mapper name */
  NULL, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map78_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER79
/* mapper 79: NINA-03/06 */
 void map79_write(uint32_t address, uint8_t value)
{
  if ((address & 0x5100) == 0x4100)
  {
    mmc_bankrom(32, 0x8000, (value >> 3) & 1);
    mmc_bankvrom(8, 0x0000, value & 7);
  }
}

 void map79_init(void)
{
  mmc_bankrom(32, 0x8000, 0);
  mmc_bankvrom(8, 0x0000, 0);
}

 map_memwrite map79_memwrite[] =
{
  { 0x4100, 0x5FFF, map79_write }, /* ????? incorrect range ??? */
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map79_intf =
{
  79, /* mapper number */
  (char*)"NINA-03/06", /* mapper name */
  map79_init, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map79_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER85
 struct
{
  int counter, latch;
  int wait_state;
  bool enabled;
} irq85;

/* mapper 85: Konami VRC7 */
 void map85_write(uint32_t address, uint8_t value)
{
  uint8_t bank = address >> 12;
  uint8_t reg = (address & 0x10) | ((address & 0x08) << 1);

  switch (bank)
  {
    case 0x08:
      if (0x10 == reg)
        mmc_bankrom(8, 0xA000, value);
      else
        mmc_bankrom(8, 0x8000, value);
      break;

    case 0x09:
      /* 0x10 & 0x30 should be trapped by sound emulation */
      mmc_bankrom(8, 0xC000, value);
      break;

    case 0x0A:
      if (0x10 == reg)
        mmc_bankvrom(1, 0x0400, value);
      else
        mmc_bankvrom(1, 0x0000, value);
      break;

    case 0x0B:
      if (0x10 == reg)
        mmc_bankvrom(1, 0x0C00, value);
      else
        mmc_bankvrom(1, 0x0800, value);
      break;

    case 0x0C:
      if (0x10 == reg)
        mmc_bankvrom(1, 0x1400, value);
      else
        mmc_bankvrom(1, 0x1000, value);
      break;

    case 0x0D:
      if (0x10 == reg)
        mmc_bankvrom(1, 0x1C00, value);
      else
        mmc_bankvrom(1, 0x1800, value);
      break;

    case 0x0E:
      if (0x10 == reg)
      {
        irq85.latch = value;
      }
      else
      {
        switch (value & 3)
        {
          case 0:
            ppu_mirror(0, 1, 0, 1); /* vertical */
            break;

          case 1:
            ppu_mirror(0, 0, 1, 1); /* horizontal */
            break;

          case 2:
            ppu_mirror(0, 0, 0, 0);
            break;

          case 3:
            ppu_mirror(1, 1, 1, 1);
            break;
        }
      }
      break;

    case 0x0F:
      if (0x10 == reg)
      {
        irq85.enabled = irq85.wait_state;
      }
      else
      {
        irq85.wait_state = value & 0x01;
        irq85.enabled = (value & 0x02) ? true : false;
        if (true == irq85.enabled)
          irq85.counter = irq85.latch;
      }
      break;

    default:
      break;
  }
}

 void map85_hblank(int vblank)
{
  UNUSED(vblank);

  if (irq85.enabled)
  {
    if (++irq85.counter > 0xFF)
    {
      irq85.counter = irq85.latch;
      nes_irq();

      //return;
    }
    //irq85.counter++;
  }
}

 map_memwrite map85_memwrite[] =
{
  { 0x8000, 0xFFFF, map85_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

 void map85_init(void)
{
  mmc_bankrom(16, 0x8000, 0);
  mmc_bankrom(16, 0xC000, MMC_LASTBANK);

  mmc_bankvrom(8, 0x0000, 0);

  irq85.counter = irq85.latch = 0;
  irq85.wait_state = 0;
  irq85.enabled = false;
}

mapintf_t map85_intf =
{
  85, /* mapper number */
  (char*)"Konami VRC7", /* mapper name */
  map85_init, /* init routine */
  NULL, /* vblank callback */
  map85_hblank, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map85_memwrite, /* memory write structure */
  NULL
};
//................................................................................
//MAPPER87
/******************************************/
/* Mapper #87 write handler ($6000-$7FFF) */
/******************************************/
 void map87_write (uint32_t address, uint8_t value)
{
  /* Within range, address written to is irrelevant */
  UNUSED (address);

  /* Very simple: 8K CHR page is selected by D1 */
  if (value & 0x02) mmc_bankvrom (8, 0x0000, 0x01);
  else              mmc_bankvrom (8, 0x0000, 0x00);

  /* Done */
  return;
}

 map_memwrite map87_memwrite [] =
{
  { 0x6000, 0x7FFF, map87_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map87_intf =
{
  87,                               /* Mapper number */
  (char*)"16K VROM switch",                /* Mapper name */
  NULL,                             /* Initialization routine */
  NULL,                             /* VBlank callback */
  NULL,                             /* HBlank callback */
  NULL,                             /* Get state (SNSS) */
  NULL,                             /* Set state (SNSS) */
  NULL,                             /* Memory read structure */
  map87_memwrite,                   /* Memory write structure */
  NULL                              /* External sound device */
};
//................................................................................
//MAPPER93
 void map93_write(uint32_t address, uint8_t value)
{
  UNUSED(address);

  /* ($8000-$FFFF) D7-D4 = switch $8000-$BFFF D0: mirror */
  mmc_bankrom(16, 0x8000, value >> 4);

  if (value & 1)
    ppu_mirror(0, 1, 0, 1); /* vert */
  else
    ppu_mirror(0, 0, 1, 1); /* horiz */
}

 map_memwrite map93_memwrite[] =
{
  { 0x8000, 0xFFFF, map93_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map93_intf =
{
  93, /* mapper number */
  (char*)"Mapper 93", /* mapper name */
  NULL, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map93_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER94
/* mapper 94: Senjou no Ookami */
 void map94_write(uint32_t address, uint8_t value)
{
  UNUSED(address);

  /* ($8000-$FFFF) D7-D2 = switch $8000-$BFFF */
  mmc_bankrom(16, 0x8000, value >> 2);
}

 map_memwrite map94_memwrite[] =
{
  { 0x8000, 0xFFFF, map94_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map94_intf =
{
  94, /* mapper number */
  (char*)"Mapper 94", /* mapper name */
  NULL, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map94_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER99
/* Switch VROM for VS games */
 void map99_vromswitch(uint8_t value)
{
  int bank = (value & 0x04) >> 2;
  mmc_bankvrom(8, 0x0000, bank);
}

/* mapper 99: VS. System */
 void map99_init(void)
{
  ppu_mirror(0, 1, 2, 3);
  ppu_setvromswitch(map99_vromswitch);
}

mapintf_t map99_intf =
{
  99, /* mapper number */
  (char*)"VS. System", /* mapper name */
  map99_init, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  NULL, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER160
 struct
{
  bool enabled, expired;
  int counter;
  int latch_c005, latch_c003;
} irq160;

 void map160_write(uint32_t address, uint8_t value)
{
  if (address >= 0x8000 && address <= 0x8003)
  {
    mmc_bankrom(8, 0x8000 + 0x2000 * (address & 3), value);
  }
  else if (address >= 0x9000 && address <= 0x9007)
  {
    mmc_bankvrom(1, 0x400 * (address & 7), value);
  }
  else if (0xC002 == address)
  {
    irq160.enabled = false;
    irq160.latch_c005 = irq160.latch_c003;
  }
  else if (0xC003 == address)
  {
    if (false == irq160.expired)
    {
      irq160.counter = value;
    }
    else
    {
      irq160.expired = false;
      irq160.enabled = true;
      irq160.counter = irq160.latch_c005;
    }
  }
  else if (0xC005 == address)
  {
    irq160.latch_c005 = value;
    irq160.counter = value;
  }
}

 void map160_hblank(int vblank)
{
  if (!vblank)
  {
    if (ppu_enabled() && irq160.enabled)
    {
      if (0 == irq160.counter && false == irq160.expired)
      {
        irq160.expired = true;
        nes_irq();
      }
      else
      {
        irq160.counter--;
      }
    }
  }
}

 void map160_init(void)
{
  irq160.enabled = false;
  irq160.expired = false;
  irq160.counter = 0;
  irq160.latch_c003 = irq160.latch_c005 = 0;
}

 map_memwrite map160_memwrite[] =
{
  { 0x8000, 0xFFFF, map160_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map160_intf =
{
  160, /* mapper number */
  (char*)"Aladdin (pirate)", /* mapper name */
  map160_init, /* init routine */
  NULL, /* vblank callback */
  map160_hblank, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map160_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//MAPPER229
/************************/
/* Mapper #229: 31 in 1 */
/************************/
 void map229_init (void)
{
  /* On reset, PRG is set to first 32K and CHR to first 8K */
  mmc_bankrom (32, 0x8000, 0x00);
  mmc_bankvrom (8, 0x0000, 0x00);

  /* Done */
  return;
}

/*******************************************/
/* Mapper #229 write handler ($8000-$FFFF) */
/*******************************************/
 void map229_write (uint32_t address, uint8_t value)
{
  /* Value written is irrelevant */
  UNUSED (value);

  /* A4-A0 sets 8K CHR page */
  mmc_bankvrom (8, 0x0000, (uint8_t) (address & 0x1F));

  /* If A4-A1 are all low then select the first 32K,     */
  /* otherwise select a 16K bank at both $8000 and $C000 */
  if ((address & 0x1E) == 0x00)
  {
    mmc_bankrom (32, 0x8000, 0x00);
  }
  else
  {
    mmc_bankrom (16, 0x8000, (uint8_t) (address & 0x1F));
    mmc_bankrom (16, 0xC000, (uint8_t) (address & 0x1F));
  }

  /* A5: mirroring (low = vertical, high = horizontal) */
  if (address & 0x20) ppu_mirror(0, 0, 1, 1);
  else                ppu_mirror(0, 1, 0, 1);

  /* Done */
  return;
}

 map_memwrite map229_memwrite [] =
{
  { 0x8000, 0xFFFF, map229_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map229_intf =
{
  229,                              /* Mapper number */
  (char*)"31 in 1 (bootleg)",              /* Mapper name */
  map229_init,                      /* Initialization routine */
  NULL,                             /* VBlank callback */
  NULL,                             /* HBlank callback */
  NULL,                             /* Get state (SNSS) */
  NULL,                             /* Set state (SNSS) */
  NULL,                             /* Memory read structure */
  map229_memwrite,                  /* Memory write structure */
  NULL                              /* External sound device */
};

//................................................................................
//MAPPER231
/* mapper 231: NINA-07, used in Wally Bear and the NO! Gang */

 void map231_init(void)
{
  mmc_bankrom(32, 0x8000, MMC_LASTBANK);
}

 void map231_write(uint32_t address, uint8_t value)
{
  int bank, vbank;
  UNUSED(address);

  bank = ((value & 0x80) >> 5) | (value & 0x03);
  vbank = (value >> 4) & 0x07;

  mmc_bankrom(32, 0x8000, bank);
  mmc_bankvrom(8, 0x0000, vbank);
}

 map_memwrite map231_memwrite[] =
{
  { 0x8000, 0xFFFF, map231_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

mapintf_t map231_intf =
{
  231, /* mapper number */
  (char*)"NINA-07", /* mapper name */
  map231_init, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map231_memwrite, /* memory write structure */
  NULL /* external sound device */
};
//................................................................................
//................................................................................
//................................................................................
//................................................................................
//MAPVRC.C =
//MAPPER21
//MAPPER22
//MAPPER23
//MAPPER25
#define VRC_VBANK(bank, value, high) \
  { \
    if ((high)) \
      highnybbles[(bank)] = (value) & 0x0F; \
    else \
      lownybbles[(bank)] = (value) & 0x0F; \
    mmc_bankvrom(1, (bank) << 10, (highnybbles[(bank)] << 4)+lownybbles[(bank)]); \
  }

 struct
{
  int counter, enabled;
  int latch, wait_state;
} irq_;

/// int select_c000 = 0;
/// uint8_t lownybbles[8];
/// uint8_t highnybbles[8];

 void vrc_init(void)
{
  irq_.counter = irq_.enabled = 0;
  irq_.latch = irq_.wait_state = 0;
}

 void map21_write(uint32_t address, uint8_t value)
{
  switch (address)
  {
    case 0x8000:
      if (select_c000)
        mmc_bankrom(8, 0xC000, value);
      else
        mmc_bankrom(8, 0x8000, value);
      break;

    case 0x9000:
      switch (value & 3)
      {
        case 0:
          ppu_mirror(0, 1, 0, 1); /* vertical */
          break;

        case 1:
          ppu_mirror(0, 0, 1, 1); /* horizontal */
          break;

        case 2:
          ppu_mirror(0, 0, 0, 0);
          break;

        case 3:
          ppu_mirror(1, 1, 1, 1);
          break;

        default:
          break;
      }
      break;
    case 0x9002: select_c000 = (value & 0x02) >> 1; break;
    case 0xA000: mmc_bankrom(8, 0xA000, value); break;

    case 0xB000: VRC_VBANK(0, value, 0); break;
    case 0xB002:
    case 0xB040: VRC_VBANK(0, value, 1); break;
    case 0xB001:
    case 0xB004:
    case 0xB080: VRC_VBANK(1, value, 0); break;
    case 0xB003:
    case 0xB006:
    case 0xB0C0: VRC_VBANK(1, value, 1); break;
    case 0xC000: VRC_VBANK(2, value, 0); break;
    case 0xC002:
    case 0xC040: VRC_VBANK(2, value, 1); break;
    case 0xC001:
    case 0xC004:
    case 0xC080: VRC_VBANK(3, value, 0); break;
    case 0xC003:
    case 0xC006:
    case 0xC0C0: VRC_VBANK(3, value, 1); break;
    case 0xD000: VRC_VBANK(4, value, 0); break;
    case 0xD002:
    case 0xD040: VRC_VBANK(4, value, 1); break;
    case 0xD001:
    case 0xD004:
    case 0xD080: VRC_VBANK(5, value, 0); break;
    case 0xD003:
    case 0xD006:
    case 0xD0C0: VRC_VBANK(5, value, 1); break;
    case 0xE000: VRC_VBANK(6, value, 0); break;
    case 0xE002:
    case 0xE040: VRC_VBANK(6, value, 1); break;
    case 0xE001:
    case 0xE004:
    case 0xE080: VRC_VBANK(7, value, 0); break;
    case 0xE003:
    case 0xE006:
    case 0xE0C0: VRC_VBANK(7, value, 1); break;

    case 0xF000:
      irq_.latch &= 0xF0;
      irq_.latch |= (value & 0x0F);
      break;
    case 0xF002:
    case 0xF040:
      irq_.latch &= 0x0F;
      irq_.latch |= ((value & 0x0F) << 4);
      break;
    case 0xF004:
    case 0xF001:
    case 0xF080:
      irq_.enabled = (value >> 1) & 0x01;
      irq_.wait_state = value & 0x01;
      irq_.counter = irq_.latch;
      break;
    case 0xF006:
    case 0xF003:
    case 0xF0C0:
      irq_.enabled = irq_.wait_state;
      break;

    default:
      break;
  }
}

 void map22_write(uint32_t address, uint8_t value)
{
  int reg = address >> 12;

  switch (reg)
  {
    case 0x8:
      mmc_bankrom(8, 0x8000, value);
      break;

    case 0xA:
      mmc_bankrom(8, 0xA000, value);
      break;

    case 0x9:
      switch (value & 3)
      {
        case 0:
          ppu_mirror(0, 1, 0, 1); /* vertical */
          break;

        case 1:
          ppu_mirror(0, 0, 1, 1); /* horizontal */
          break;

        case 2:
          ppu_mirror(1, 1, 1, 1);
          break;

        case 3:
          ppu_mirror(0, 0, 0, 0);
          break;
      }
      break;

    case 0xB:
    case 0xC:
    case 0xD:
    case 0xE:
      {
        int loc = (((reg - 0xB) << 1) + (address & 1)) << 10;
        mmc_bankvrom(1, loc, value >> 1);
      }
      break;

    default:
      break;
  }
}

 void map23_write(uint32_t address, uint8_t value)
{
  switch (address)
  {
    case 0x8000:
    case 0x8FFF:
      mmc_bankrom(8, 0x8000, value);
      break;

    case 0xA000:
    case 0xAFFF:
      mmc_bankrom(8, 0xA000, value);
      break;

    case 0x9000:
    case 0x9004:
    case 0x9008:
      switch (value & 3)
      {
        case 0:
          ppu_mirror(0, 1, 0, 1); /* vertical */
          break;

        case 1:
          ppu_mirror(0, 0, 1, 1); /* horizontal */
          break;

        case 2:
          ppu_mirror(0, 0, 0, 0);
          break;

        case 3:
          ppu_mirror(1, 1, 1, 1);
          break;
      }
      break;

    case 0xB000: VRC_VBANK(0, value, 0); break;
    case 0xB001:
    case 0xB004: VRC_VBANK(0, value, 1); break;
    case 0xB002:
    case 0xB008: VRC_VBANK(1, value, 0); break;
    case 0xB003:
    case 0xB00C: VRC_VBANK(1, value, 1); break;
    case 0xC000: VRC_VBANK(2, value, 0); break;
    case 0xC001:
    case 0xC004: VRC_VBANK(2, value, 1); break;
    case 0xC002:
    case 0xC008: VRC_VBANK(3, value, 0); break;
    case 0xC003:
    case 0xC00C: VRC_VBANK(3, value, 1); break;
    case 0xD000: VRC_VBANK(4, value, 0); break;
    case 0xD001:
    case 0xD004: VRC_VBANK(4, value, 1); break;
    case 0xD002:
    case 0xD008: VRC_VBANK(5, value, 0); break;
    case 0xD003:
    case 0xD00C: VRC_VBANK(5, value, 1); break;
    case 0xE000: VRC_VBANK(6, value, 0); break;
    case 0xE001:
    case 0xE004: VRC_VBANK(6, value, 1); break;
    case 0xE002:
    case 0xE008: VRC_VBANK(7, value, 0); break;
    case 0xE003:
    case 0xE00C: VRC_VBANK(7, value, 1); break;

    case 0xF000:
      irq_.latch &= 0xF0;
      irq_.latch |= (value & 0x0F);
      break;

    case 0xF004:
      irq_.latch &= 0x0F;
      irq_.latch |= ((value & 0x0F) << 4);
      break;

    case 0xF008:
      irq_.enabled = (value >> 1) & 0x01;
      irq_.wait_state = value & 0x01;
      irq_.counter = irq_.latch;
      break;

    case 0xF00C:
      irq_.enabled = irq_.wait_state;
      break;

    default:
      break;
  }
}

 void vrc_hblank(int vblank)
{
  UNUSED(vblank);

  if (irq_.enabled)
  {
    if (256 == ++irq_.counter)
    {
      irq_.counter = irq_.latch;
      nes_irq();
      //irq.enabled = false;
      irq_.enabled = irq_.wait_state;
    }
  }
}



 map_memwrite map21_memwrite[] =
{
  { 0x8000, 0xFFFF, map21_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

 map_memwrite map22_memwrite[] =
{
  { 0x8000, 0xFFFF, map22_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

 map_memwrite map23_memwrite[] =
{
  { 0x8000, 0xFFFF, map23_write },
  {     (uint32_t)-1,     (uint32_t)-1, NULL }
};

 void map21_getstate(SnssMapperBlock *state);
 void map21_getstate(SnssMapperBlock *state)
{
  state->extraData.mapper21.irqCounter = irq_.counter;
  state->extraData.mapper21.irqCounterEnabled = irq_.enabled;
}

 void map21_setstate(SnssMapperBlock *state);
 void map21_setstate(SnssMapperBlock *state)
{
  irq_.counter = state->extraData.mapper21.irqCounter;
  irq_.enabled = state->extraData.mapper21.irqCounterEnabled;
}

mapintf_t map21_intf =
{
  21, /* mapper number */
  (char*)"Konami VRC4 A", /* mapper name */
  vrc_init, /* init routine */
  NULL, /* vblank callback */
  vrc_hblank, /* hblank callback */
  map21_getstate, /* get state (snss) */
  map21_setstate, /* set state (snss) */
  NULL, /* memory read structure */
  map21_memwrite, /* memory write structure */
  NULL /* external sound device */
};

mapintf_t map22_intf =
{
  22, /* mapper number */
  (char*)"Konami VRC2 A", /* mapper name */
  vrc_init, /* init routine */
  NULL, /* vblank callback */
  NULL, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map22_memwrite, /* memory write structure */
  NULL /* external sound device */
};

mapintf_t map23_intf =
{
  23, /* mapper number */
  (char*)"Konami VRC2 B", /* mapper name */
  vrc_init, /* init routine */
  NULL, /* vblank callback */
  vrc_hblank, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map23_memwrite, /* memory write structure */
  NULL /* external sound device */
};

mapintf_t map25_intf =
{
  25, /* mapper number */
  (char*)"Konami VRC4 B", /* mapper name */
  NULL, /* init routine */
  NULL, /* vblank callback */
  vrc_hblank, /* hblank callback */
  NULL, /* get state (snss) */
  NULL, /* set state (snss) */
  NULL, /* memory read structure */
  map21_memwrite, /* memory write structure */
  NULL /* external sound device */
};
