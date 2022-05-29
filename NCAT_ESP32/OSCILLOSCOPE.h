#if OSCILLOSCOPE

//OSC MAIN DEFINITIONS:
#define NUM_SAMPLES (1000)
#define I2S_NUM       (0)
#define BUFF_SIZE 20*1000
#define B_MULT (BUFF_SIZE/NUM_SAMPLES)

#define BUFFERS_COUNT 20

#if COMPOSITE_VIDEO  
DMA_ATTR lldesc_t dma_descriptor[BUFFERS_COUNT];
DMA_ATTR uint16_t* dma_buff[BUFFERS_COUNT];
#endif


float mean = 0;
float max_v, min_v;
float freq = 0;
float period = 0;
uint32_t trigger0 = 0;
uint32_t trigger1 = 0;
float sample_rate = 28636 / 2; // 28.636/2 Mhz

int shift = 125; //info text shift X
bool info = true;
bool ADC_EXIT = false;
uint16_t spritecolor = 0b0000011111000000;
bool timebase_switched=false;

//===============================================================================
void REINIT_ADC(){
#if COMPOSITE_VIDEO  
  // Switch on Slave mode.
  // I2S_CONF_REG -> I2S_RX_SLAVE_MOD
  // Set to 1 to enable slave mode.
  I2S0.conf.rx_slave_mod = 0; //not 1 !!!


  // Enable parallel mode
  // I2S_CONF2_REG -> I2S_LCD_END
  // Set to 1 to enable LCD mode.
  I2S0.conf2.lcd_en = 1;

  // Use HSYNC/VSYNC/HREF to control sampling
  // I2S_CONF2_REG -> I2S_CAMERA_EN
  // Set to 1 to enable camera mode.
  //I2S0.conf2.camera_en = 1;


  // Configure clock divider
  I2S0.clkm_conf.clkm_div_a   = 1;
  I2S0.clkm_conf.clkm_div_b   = 0;
  I2S0.clkm_conf.clkm_div_num = 1;

  // I2S_FIFO_CONF_REG -> I2S_DSCR_EN
  // FIFO will sink data to DMA
  I2S0.fifo_conf.dscr_en = 1;

  // FIFO configuration
  // I2S_FIFO_CONF_REG -> RX_FIFO_MOD
  I2S0.fifo_conf.rx_fifo_mod          = 1; // 0-3???

  // I2S_FIFO_CONF_REG -> RX_FIFO_MOD_FORCE_EN
  I2S0.fifo_conf.rx_fifo_mod_force_en = 1;

  // I2S_CONF_CHAN_REG -> I2S_RX_CHAN_MOD
  I2S0.conf_chan.rx_chan_mod          = 1;

  // Clear flags which are used in I2S serial mode
  // I2S_SAMPLE_RATE_CONF_REG -> I2S_RX_BITS_MOD
  I2S0.sample_rate_conf.rx_bits_mod = 0;

  // I2S_CONF_REG -> I2S_RX_RIGHT_FIRST
  I2S0.conf.rx_right_first = 0;
  //I2S0.conf.rx_right_first = 0;

  // I2S_CONF_REG -> I2S_RX_MSB_RIGHT
  I2S0.conf.rx_msb_right   = 0;
  //I2S0.conf.rx_msb_right   = 1;

  // I2S_CONF_REG -> I2S_RX_MSB_SHIFT
  I2S0.conf.rx_msb_shift   = 0;
  //I2S0.conf.rx_msb_shift   = 1;

  // I2S_CONF_REG -> I2S_RX_MSB_MONO
  I2S0.conf.rx_mono        = 1;

  // I2S_CONF_REG -> I2S_RX_SHORT_SYNC
  I2S0.conf.rx_short_sync  = 0;

  I2S0.timing.val          = 0;

  for (uint32_t buff_idx = 0; buff_idx < BUFFERS_COUNT; buff_idx++) {
    dma_descriptor[buff_idx].length = 0; //number of byte written to the buffer
    dma_descriptor[buff_idx].size = 2000;  //max size of the buffer in bytes
    dma_descriptor[buff_idx].owner = 1;
    dma_descriptor[buff_idx].sosf = 1;
    dma_descriptor[buff_idx].buf = (uint8_t*)&dma_buff[buff_idx][0];
    dma_descriptor[buff_idx].offset = 0;
    dma_descriptor[buff_idx].empty = 0;
    dma_descriptor[buff_idx].eof = 1;
    //pointer to the next descriptor
    if (buff_idx == BUFFERS_COUNT-1) dma_descriptor[buff_idx].qe.stqe_next = &dma_descriptor[0];
    else dma_descriptor[buff_idx].qe.stqe_next = &dma_descriptor[buff_idx + 1];
  }     
  I2S0.in_link.addr = (uint32_t)&dma_descriptor[0];

  // I2S_RX_EOF_NUM_REG
  I2S0.rx_eof_num   = 1000 * 20;

  // I2S_IN_LINK_REG -> I2S_INLINK_ADDR
  //I2S0.in_link.addr = (uint32_t) pFirst;

  // I2S_IN_LINK_REG -> I2S_INLINK_START
  I2S0.in_link.start   = 1;
#endif      
}      
//===============================================================================

#if COMPOSITE_VIDEO  
//------------------------
uint8_t digital_wave_option = 0; //0-auto | 1-analog | 2-digital data (SERIAL/SPI/I2C/etc)
//------------------------
float v_offset = 0; // vertical offset is zero in VOLTS
float v_div = 412.5;  // 0.825V/div 4divs!!! -> converted to 8divs (0.412.5V/div)
float v_scale = 0.833333333; //scale 83.3333% verticaly
float h_scale = 0.833333333; //scale 83.3333% horizontaly

uint32_t TIMEBASE = 2; // 2us or more
//--------------------------------------------------------------------------------
//********************************************************************************
//*  OSCILLOSCOPE FUNCTIONS:                                                     *
//********************************************************************************
class mean_filter {
  public:
    mean_filter(int values) {
      _values = values;
    }

    void init(float value) {
      for (int i = 0; i < _values; i++) {
        _data[i] = value;
      }
    }
    float filter(float reading) {
      float temp = 0;
      _data[_values - 1] = reading;
      for (int i = 0; i < _values - 1; i++) {
        temp += _data[i];
        _data[i] = _data[i + 1];
      }
      temp += reading;
      return temp / float(_values);
    }
    int _values = 5;
    float _data[100] = {0};
};
//--------------------------------------------------------------------------------
float to_scale(float reading) {
  float temp = 
    LCD_HEIGHT -
    (
      (
        (
          (float)((reading) / 4095.0)
          + (float)(v_offset / 3.3) // vertical offset is zero in VOLTS
        ) * 3300 /
        (float)(v_div * 8) // vertically 8divs
      )
    ) * (LCD_HEIGHT - 1) - 1;
  temp = temp * (float)(v_scale); //vertical scale to %
  temp = temp + (float)(LCD_HEIGHT * ((1 - v_scale) / 2)); // vertical offset from scale/2 %
  return temp;
}
//--------------------------------------------------------------------------------
float to_voltage(float reading) {
  return  (reading) / 4095.0 * 3.3;
}
//--------------------------------------------------------------------------------
//********************************************************************************
void draw_grid() {
  float px_v_per_div = (float)( LCD_HEIGHT * (float)(v_scale)) / 8;
  float px_v_offset = (float)(LCD_HEIGHT * ((1 - v_scale) / 2)); // vertical offset from scale/2 %
  float px_h_per_div = (float)( LCD_WIDTH * (float)(h_scale)) / 8;
  float px_h_offset = (float)(LCD_WIDTH * ((1 - h_scale) / 2)); // vertical offset from scale/2 %

  for (int y = 0; y <= 8; y++) { // 8 divs
    for (int x = 0; x <= 8; x++) { // 8 divs
      //horizontal lines
      if (x < 8)
        for (int i = 0; i < px_h_per_div; i++)  {
          if (i % 8 == 0) screenmemory_drawpixel(x * px_h_per_div + i + px_v_offset, y * px_v_per_div +  px_h_offset , 0b10010100);
        }
      if (y < 8)
        //vertical lines
        for (int j = 0; j < px_v_per_div; j++)  {
          if (j % 8 == 0) screenmemory_drawpixel(x * px_h_per_div + px_v_offset, y * px_v_per_div + j +  px_h_offset  , 0b10010100);
        }
    }
  }

  //draw horizontal center line:
  screenmemory_line(px_h_offset , LCD_HEIGHT / 2, LCD_WIDTH - px_h_offset, LCD_HEIGHT / 2, 0b10010100);

  //draw vertical center line:
  screenmemory_line(LCD_WIDTH / 2 , px_v_offset, LCD_WIDTH / 2, LCD_HEIGHT - px_v_offset, 0b10010100);
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void peak_mean(uint32_t len, float * max_value, float * min_value, float *pt_mean) {
  max_value[0] = (uint16_t)dma_buff[0][0];
  min_value[0] = (uint16_t)dma_buff[0][0];


  mean_filter filter(5);
  filter.init(dma_buff[0][0]);

  float mean = 0;
  for (uint32_t i = 1; i < len; i++) {
    float value = filter.filter((float)((uint16_t)dma_buff[i / NUM_SAMPLES][i % NUM_SAMPLES]));

    if (value == 0) continue; // Fix zero values

    if (value > max_value[0]) max_value[0] = value;
    if (value < min_value[0]) min_value[0] = value;

    mean += (uint16_t)dma_buff[i / NUM_SAMPLES][i % NUM_SAMPLES];
  }
  mean /= float(BUFF_SIZE);
  mean = to_voltage(mean);
  pt_mean[0] = mean;
}
//--------------------------------------------------------------------------------
//true if digital/ false if analog
bool digital_analog(uint32_t max_v, uint32_t min_v) {
  uint32_t upper_threshold = max_v - 0.05 * (max_v - min_v);
  uint32_t lower_threshold = min_v + 0.05 * (max_v - min_v);
  uint32_t digital_data = 0;
  uint32_t analog_data = 0;
  for (uint32_t i = 0; i < BUFF_SIZE; i++) {
    if (dma_buff[i / NUM_SAMPLES][i % NUM_SAMPLES] > lower_threshold) {
      if (dma_buff[i / NUM_SAMPLES][i % NUM_SAMPLES] > upper_threshold) {
        //HIGH DIGITAL
        digital_data++;
      } else {
        //ANALOG/TRANSITION
        analog_data++;
      }
    } else {
      //LOW DIGITAL
      digital_data++;
    }
  }
  //more than 50% of data is analog
  if (analog_data < digital_data) return true;
  return false;
}
//--------------------------------------------------------------------------------
void trigger_freq_analog(float sample_rate,
                         float mean,
                         uint32_t max_v,
                         uint32_t min_v,
                         float *pt_freq,
                         float *pt_period,
                         uint32_t *pt_trigger0,
                         uint32_t *pt_trigger1) {
  float freq = 0;
  float period = 0;
  bool signal_side = false;
  uint32_t trigger_count = 0;
  uint32_t trigger_num = 10;
  uint32_t trigger_temp[trigger_num] = {0};
  uint32_t trigger_index = 0;

  //get initial signal relative to the mean
  if (to_voltage(dma_buff[0][0]) > mean) {
    signal_side = true;
  }

  //waveform repetitions calculation + get triggers time
  uint32_t wave_center = (max_v + min_v) / 2;
  for (uint32_t i = 1 ; i < BUFF_SIZE; i++) {
    if (signal_side && dma_buff[i / NUM_SAMPLES][i % NUM_SAMPLES] < wave_center - (wave_center - min_v) * 0.2) {

      signal_side = false;
    } else if (!signal_side && dma_buff[i / NUM_SAMPLES][i % NUM_SAMPLES] > wave_center + (max_v - wave_center) * 0.2) {
      
      freq++;
      if (trigger_count < trigger_num) {
        trigger_temp[trigger_count] = i;
        trigger_count++;
      }
      signal_side = true;
    }
  }
  //frequency calculation
  if (trigger_count < 2) {
    trigger_temp[0] = 0;
    trigger_index = 0;
    freq = 0;
    period = 0;
  } else {
    //simple frequency calculation fair enough for frequencies over 2khz (20hz resolution)
    freq = freq * NUM_SAMPLES / B_MULT;
    period = (float)(sample_rate * 1000.0) / freq; //us
  }

  // setting triggers offset and getting second trigger for debug cursor on drawn_channel1

  // The trigger function uses a rise porcentage (5%) obove the mean, thus,
  // the real waveform starting point is some datapoints back.
  // The resulting trigger gets a negative offset of 5% of the calculated period

  uint32_t trigger2 = 0;
  if (trigger_temp[0] - period * 0.05 > 0 && trigger_count > 1) {
    trigger_index = trigger_temp[0] - period * 0.05;
    trigger2 = trigger_temp[1] - period * 0.05;
  } else if (trigger_count > 2) {
    trigger_index = trigger_temp[1] - period * 0.05;
    if (trigger_count > 2) trigger2 = trigger_temp[2] - period * 0.05;
  }

  pt_trigger0[0] = trigger_index;
  pt_trigger1[0] = trigger2;
  pt_freq[0] = freq;
  pt_period[0] = period;
}
//--------------------------------------------------------------------------------
void trigger_freq_digital(float sample_rate,
                          float mean,
                          uint32_t max_v,
                          uint32_t min_v,
                          float *pt_freq,
                          float *pt_period,
                          uint32_t *pt_trigger0) {

  float freq = 0;
  float period = 0;
  bool signal_side = false;
  uint32_t trigger_count = 0;
  uint32_t trigger_num = 10;
  uint32_t trigger_temp[trigger_num] = {0};
  uint32_t trigger_index = 0;

  //get initial signal relative to the mean
  if (to_voltage(dma_buff[0][0]) > mean) {
    
    signal_side = true;
  }

  //waveform repetitions calculation + get triggers time
  uint32_t wave_center = (max_v + min_v) / 2;
  bool normal_high = (mean > to_voltage(wave_center)) ? true : false;
  if (max_v - min_v > 4095 * (0.4 / 3.3)) {
    for (uint32_t i = 1 ; i < BUFF_SIZE; i++) {
      if (signal_side && dma_buff[i / NUM_SAMPLES][i % NUM_SAMPLES] < wave_center - (wave_center - min_v) * 0.2) {
        //signal was high, fell -> trigger if normal high
        if (trigger_count < trigger_num && normal_high) {
          trigger_temp[trigger_count] = i;
          trigger_count++;
        }
        signal_side = false;
      } else if (!signal_side && dma_buff[i / NUM_SAMPLES][i % NUM_SAMPLES] > wave_center + (max_v - wave_center) * 0.2) {

        freq++;
        //signal was low, rose -> trigger if normal low
        if (trigger_count < trigger_num && !normal_high) {
          trigger_temp[trigger_count] = i;
          trigger_count++;
        }
        signal_side = true;
      }
    }

    //Digital frequency calculation:
    freq = 281.7133902265831 * freq;
    period = ((float)(sample_rate * 1000.0) / freq) / (28.636 / 2); //us

    if (trigger_count > 1) {
      //from 2000 to 80 hz -> uses mean of the periods for precision
      if (freq < 2000 && freq > 80) {
        period = 0;
        for (uint32_t i = 1; i < trigger_count; i++) {
          period += trigger_temp[i] - trigger_temp[i - 1];
        }
        period /= (trigger_count - 1);
        freq = sample_rate * (NUM_SAMPLES) / period;
      }
      //under 80hz, single period for frequency calculation
      else if (trigger_count > 1 && freq <= 80) {
        period = trigger_temp[1] - trigger_temp[0];
        freq = sample_rate * (2800 / 2) / period;
      }
    }
    trigger_index = trigger_temp[0];

    if (trigger_index > 10) trigger_index -= 10;
    else trigger_index = 0;
  }
  pt_trigger0[0] = trigger_index;
  pt_freq[0] = freq;
  pt_period[0] = period;
}
//********************************************************************************

#endif

#endif
