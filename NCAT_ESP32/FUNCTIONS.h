//********************************************************************************
//   VIDEO DRAW SYSTEM:
//********************************************************************************
void screenmemory_fillscreen(uint8_t COLOR) {
#if VGA_OUTPUT
  for (uint16_t Ypos = 0; Ypos < LCD_HEIGHT; Ypos++)
    for (uint16_t Xpos = 0; Xpos < SCREEN_WIDTH-2; Xpos++) {
#else    
  for (uint16_t Ypos = 0; Ypos < SCREEN_HEIGHT; Ypos++)
    for (uint16_t Xpos = 0; Xpos < 256 /*< SCREEN_WIDTH*/; Xpos++) {
#endif      

#if VGA_OUTPUT
  if (VGA_INITED==1)
  if (Ypos>=0 && Ypos < LCD_HEIGHT && Xpos>=0 && Xpos < SCREEN_WIDTH-2) {
    if (Xpos%4==0) VGA.m_viewPort[Ypos][Xpos+2] = 0b11000000 | default_6bit[COLOR];
    if (Xpos%4==1) VGA.m_viewPort[Ypos][Xpos+2] = 0b11000000 | default_6bit[COLOR];
    if (Xpos%4==2) VGA.m_viewPort[Ypos][Xpos-2] = 0b11000000 | default_6bit[COLOR];
    if (Xpos%4==3) VGA.m_viewPort[Ypos][Xpos-2] = 0b11000000 | default_6bit[COLOR]; 
  }
  SCREENMEMORY_LINE[Ypos][Xpos] = COLOR;

#else 
      SCREENMEMORY_LINE[Ypos][Xpos] = COLOR;
#endif  
    }
  if (LCD_ENABLED) xSemaphoreGive(draw_mutex); //refresh LCD
}
//--------------------------------------------------------------------------------
void screenmemory_drawpixel(int16_t X, int16_t Y, uint8_t COLOR) {
#if VGA_OUTPUT
  if (VGA_INITED==1) {
    if (Y>=0 && Y < LCD_HEIGHT && X>=0 && X < SCREEN_WIDTH && DRAWMODE!=2) {
      if (X%4==0) VGA.m_viewPort[Y][X+2] = 0b11000000 | default_6bit[COLOR];
      if (X%4==1) VGA.m_viewPort[Y][X+2] = 0b11000000 | default_6bit[COLOR];
      if (X%4==2) VGA.m_viewPort[Y][X-2] = 0b11000000 | default_6bit[COLOR];
      if (X%4==3) VGA.m_viewPort[Y][X-2] = 0b11000000 | default_6bit[COLOR]; 
    }
    else if (Y>=0 && Y < LCD_HEIGHT && X>=0 && X < SCREEN_WIDTH && DRAWMODE==2) {
      if (X%4==0) VGA.m_viewPort[Y][X+2] = 0b11000000 | nes_6bit[COLOR];
      if (X%4==1) VGA.m_viewPort[Y][X+2] = 0b11000000 | nes_6bit[COLOR];
      if (X%4==2) VGA.m_viewPort[Y][X-2] = 0b11000000 | nes_6bit[COLOR];
      if (X%4==3) VGA.m_viewPort[Y][X-2] = 0b11000000 | nes_6bit[COLOR]; 
    }
  }
  if (Y>=0 && Y < LCD_HEIGHT && X>=0 && X < SCREEN_WIDTH) SCREENMEMORY_LINE[Y][X] = COLOR; 
#else 
  if (Y>=0 && Y < SCREEN_HEIGHT && X>=0 && X < SCREEN_WIDTH) SCREENMEMORY_LINE[Y][X] = COLOR;
#endif  
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void screenmemory_drawpixel_static(uint16_t X, uint16_t Y, uint16_t COLOR) {
#if VGA_OUTPUT
#if LCD_ENABLED
  if (Y < SCREEN_HEIGHT && X < LCD_HEIGHT) tft.drawPixel(X, Y, COLOR); //draw 1 pixel into LCD
#endif
  uint8_t R = (COLOR & 0b1111100000000000) >> (11 + 2);
  uint8_t G = (COLOR & 0b0000011111100000) >> (6 + 3);
  uint8_t B = (COLOR & 0b0000000000011111) >> (0 + 2);
  //0bGGGGGGGGYYYYYYYYRRRRRRRRBBBBBBBB
  uint8_t c = ((G << 6) | (R << 3) | (B << 0)); //RGB
  if (Y < LCD_HEIGHT && X < SCREEN_WIDTH) SCREENMEMORY_LINE[Y][X] = c; //draw 1 pixel into MEMORY

  if (VGA_INITED==1) {
    if (Y>=0 && Y < LCD_HEIGHT-1 && X>=2 && X < SCREEN_WIDTH ) {
      if (X%4==0) VGA.m_viewPort[Y][X+2] = 0b11000000 | default_6bit[c];
      if (X%4==1) VGA.m_viewPort[Y][X+2] = 0b11000000 | default_6bit[c];
      if (X%4==2) VGA.m_viewPort[Y][X-2] = 0b11000000 | default_6bit[c];
      if (X%4==3) VGA.m_viewPort[Y][X-2] = 0b11000000 | default_6bit[c]; 
    }
  }
#else   
#if LCD_ENABLED
  if (Y < SCREEN_HEIGHT && X < SCREEN_WIDTH) tft.drawPixel(X, Y, COLOR); //draw 1 pixel into LCD
#endif
  uint8_t R = (COLOR & 0b1111100000000000) >> (11 + 2);
  uint8_t G = (COLOR & 0b0000011111100000) >> (6 + 3);
  uint8_t B = (COLOR & 0b0000000000011111) >> (0 + 2);
  //0bGGGGGGGGYYYYYYYYRRRRRRRRBBBBBBBB
  uint8_t c = ((G << 6) | (R << 3) | (B << 0)); //RGB
  if (Y < SCREEN_HEIGHT && X < SCREEN_WIDTH) SCREENMEMORY_LINE[Y][X] = c; //draw 1 pixel into MEMORY
#endif  
}
//--------------------------------------------------------------------------------
void screenmemory_line(int startx, int starty, int endx, int endy, uint8_t color) {
  int t, distance;
  int xerr = 0, yerr = 0, delta_x, delta_y;
  int incx, incy;
  // compute the distances in both directions
  delta_x = endx - startx;
  delta_y = endy - starty;
  // Compute the direction of the increment, an increment of 0 means either a horizontal or vertical line.
  if (delta_x > 0) incx = 1;
  else if (delta_x == 0) incx = 0;
  else incx = -1;

  if (delta_y > 0) incy = 1;
  else if (delta_y == 0) incy = 0;
  else incy = -1;

  // determine which distance is greater
  delta_x = abs(delta_x);
  delta_y = abs(delta_y);
  if (delta_x > delta_y) distance = delta_x;
  else distance = delta_y;

  // draw the line
  for (t = 0; t <= distance + 1; t++) {
    screenmemory_drawpixel(startx, starty, color);

    xerr += delta_x;
    yerr += delta_y;
    if (xerr > distance) {
      xerr -= distance;
      startx += incx;
    }
    if (yerr > distance) {
      yerr -= distance;
      starty += incy;
    }
  }
}
//--------------------------------------------------------------------------------
void screenmemory_line_static(int startx, int starty, int endx, int endy, uint16_t color) {
  int t, distance;
  int xerr = 0, yerr = 0, delta_x, delta_y;
  int incx, incy;
  // compute the distances in both directions
  delta_x = endx - startx;
  delta_y = endy - starty;
  // Compute the direction of the increment, an increment of 0 means either a horizontal or vertical line.
  if (delta_x > 0) incx = 1;
  else if (delta_x == 0) incx = 0;
  else incx = -1;

  if (delta_y > 0) incy = 1;
  else if (delta_y == 0) incy = 0;
  else incy = -1;

  // determine which distance is greater
  delta_x = abs(delta_x);
  delta_y = abs(delta_y);
  if (delta_x > delta_y) distance = delta_x;
  else distance = delta_y;

  // draw the line
  for (t = 0; t <= distance + 1; t++) {
    screenmemory_drawpixel_static(startx, starty, color);

    xerr += delta_x;
    yerr += delta_y;
    if (xerr > distance) {
      xerr -= distance;
      startx += incx;
    }
    if (yerr > distance) {
      yerr -= distance;
      starty += incy;
    }
  }
}
//--------------------------------------------------------------------------------
void screenmemory_drawrectangle(int16_t X, int16_t Y, int16_t Width, int16_t Height, uint8_t COLOR) {
  screenmemory_line(X, Y, X + Width, Y, COLOR);
  screenmemory_line(X, Y, X, Y + Height, COLOR);
  screenmemory_line(X + Width, Y, X + Width, Y + Height, COLOR);
  screenmemory_line(X, Y + Height, X + Width, Y + Height, COLOR);
}
//--------------------------------------------------------------------------------
void screenmemory_drawrectangle_static(int16_t X, int16_t Y, int16_t Width, int16_t Height, uint16_t COLOR) {
  screenmemory_line_static(X, Y, X + Width, Y, COLOR);
  screenmemory_line_static(X, Y, X, Y + Height, COLOR);
  screenmemory_line_static(X + Width, Y, X + Width, Y + Height, COLOR);
  screenmemory_line_static(X, Y + Height, X + Width, Y + Height, COLOR);
}
//--------------------------------------------------------------------------------
void screenmemory_drawfillrectangle(int16_t X, int16_t Y, int16_t Width, int16_t Height, uint8_t COLOR) {
  for (uint16_t Ypos = Y; Ypos < Y + Height; Ypos++)
    for (uint16_t Xpos = X; Xpos < X + Width; Xpos++) {
      screenmemory_drawpixel(Xpos, Ypos, COLOR);
    }
  if (LCD_ENABLED) xSemaphoreGive(draw_mutex);
}
//--------------------------------------------------------------------------------
void screenmemory_drawfillrectangle_static(int16_t X, int16_t Y, int16_t Width, int16_t Height, uint16_t COLOR) {
  uint8_t R = (COLOR & 0b1111100000000000) >> (11 + 2);
  uint8_t G = (COLOR & 0b0000011111100000) >> (6 + 3);
  uint8_t B = (COLOR & 0b0000000000011111) >> (0 + 2);
  //0bGGGGGGGGYYYYYYYYRRRRRRRRBBBBBBBB
  uint8_t c = ((G << 6) | (R << 3) | (B << 0)); //RGB

  for (int16_t Ypos = Y; Ypos < Y + Height; Ypos++)
    for (int16_t Xpos = X; Xpos < X + Width; Xpos++) {
      screenmemory_drawpixel(Xpos, Ypos, c);
#if VGA_OUTPUT
  if (VGA_INITED==1) {
    if (Ypos>=0 && Ypos < LCD_HEIGHT-1 && Xpos>=0 && Xpos < SCREEN_WIDTH ) {
      if (Xpos%4==0) VGA.m_viewPort[Ypos][Xpos+2] = 0b11000000 | default_6bit[c];
      if (Xpos%4==1) VGA.m_viewPort[Ypos][Xpos+2] = 0b11000000 | default_6bit[c];
      if (Xpos%4==2) VGA.m_viewPort[Ypos][Xpos-2] = 0b11000000 | default_6bit[c];
      if (Xpos%4==3) VGA.m_viewPort[Ypos][Xpos-2] = 0b11000000 | default_6bit[c]; 
    }
  }
#endif
      
    }
  #if LCD_ENABLED
    tft.fillRect(X, Y, Width , Height, COLOR);
  #endif
}
//--------------------------------------------------------------------------------
void screenmemory_fillscreen_static(uint16_t COLOR) {
  screenmemory_drawfillrectangle_static(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
}
//--------------------------------------------------------------------------------
uint8_t draw_char_xy(uint16_t Main_x, uint16_t Main_y, char Main_char, const char* font, uint8_t color = 255, uint8_t bgcolor = 0) {
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint8_t CHAROFFSET = font[2]; //char start offset

  font[3]; //char count
  if (Main_char != '\n' && Main_char != '\r')
    for (uint16_t Ypos = 0; Ypos < YcharSize; Ypos++)
      for (uint16_t Xpos = 0; Xpos < XcharSize; Xpos += 8) {
        uint8_t CHARLINE = 0;
        if (XcharSize < 8)
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * 1) + (Ypos) * 1 + Xpos / 8  + 4];
        else
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * (XcharSize / 8)) +   (Ypos) * (XcharSize / 8) + Xpos / 8  + 4];

        if ((Xpos + 0 < XcharSize) && (CHARLINE & 0b10000000) != 0) screenmemory_drawpixel(Main_x + Xpos + 0, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 0, Main_y + Ypos, bgcolor);
        if ((Xpos + 1 < XcharSize) && (CHARLINE & 0b01000000) != 0) screenmemory_drawpixel(Main_x + Xpos + 1, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 1, Main_y + Ypos, bgcolor);
        if ((Xpos + 2 < XcharSize) && (CHARLINE & 0b00100000) != 0) screenmemory_drawpixel(Main_x + Xpos + 2, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 2, Main_y + Ypos, bgcolor);
        if ((Xpos + 3 < XcharSize) && (CHARLINE & 0b00010000) != 0) screenmemory_drawpixel(Main_x + Xpos + 3, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 3, Main_y + Ypos, bgcolor);
        if ((Xpos + 4 < XcharSize) && (CHARLINE & 0b00001000) != 0) screenmemory_drawpixel(Main_x + Xpos + 4, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 4, Main_y + Ypos, bgcolor);
        if ((Xpos + 5 < XcharSize) && (CHARLINE & 0b00000100) != 0) screenmemory_drawpixel(Main_x + Xpos + 5, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 5, Main_y + Ypos, bgcolor);
        if ((Xpos + 6 < XcharSize) && (CHARLINE & 0b00000010) != 0) screenmemory_drawpixel(Main_x + Xpos + 6, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 6, Main_y + Ypos, bgcolor);
        if ((Xpos + 7 < XcharSize) && (CHARLINE & 0b00000001) != 0) screenmemory_drawpixel(Main_x + Xpos + 7, Main_y + Ypos, color);
        else screenmemory_drawpixel(Main_x + Xpos + 7, Main_y + Ypos, bgcolor);
      }
  return XcharSize;
}
//--------------------------------------------------------------------------------
uint8_t draw_char_xy_static(uint16_t Main_x, uint16_t Main_y, char Main_char, const char* font, uint16_t color = 0xffff, uint16_t bgcolor = 0) {
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint8_t CHAROFFSET = font[2]; //char start offset

  font[3]; //char count
  if (Main_char != '\n' && Main_char != '\r')
    for (uint16_t Ypos = 0; Ypos < YcharSize; Ypos++)
      for (uint16_t Xpos = 0; Xpos < XcharSize; Xpos += 8) {
        uint8_t CHARLINE = 0;
        if (XcharSize < 8)
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * 1) + (Ypos) * 1 + Xpos / 8  + 4];
        else
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * (XcharSize / 8)) +   (Ypos) * (XcharSize / 8) + Xpos / 8  + 4];

        if ((Xpos + 0 < XcharSize) && (CHARLINE & 0b10000000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 0, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 0, Main_y + Ypos, bgcolor);
        if ((Xpos + 1 < XcharSize) && (CHARLINE & 0b01000000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 1, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 1, Main_y + Ypos, bgcolor);
        if ((Xpos + 2 < XcharSize) && (CHARLINE & 0b00100000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 2, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 2, Main_y + Ypos, bgcolor);
        if ((Xpos + 3 < XcharSize) && (CHARLINE & 0b00010000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 3, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 3, Main_y + Ypos, bgcolor);
        if ((Xpos + 4 < XcharSize) && (CHARLINE & 0b00001000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 4, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 4, Main_y + Ypos, bgcolor);
        if ((Xpos + 5 < XcharSize) && (CHARLINE & 0b00000100) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 5, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 5, Main_y + Ypos, bgcolor);
        if ((Xpos + 6 < XcharSize) && (CHARLINE & 0b00000010) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 6, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 6, Main_y + Ypos, bgcolor);
        if ((Xpos + 7 < XcharSize) && (CHARLINE & 0b00000001) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 7, Main_y + Ypos, color);
        else screenmemory_drawpixel_static(Main_x + Xpos + 7, Main_y + Ypos, bgcolor);
      }
  return XcharSize;
}
//--------------------------------------------------------------------------------
uint8_t draw_char_xy_static_transparent(uint16_t Main_x, uint16_t Main_y, char Main_char, const char* font, uint16_t color = 0xffff) {
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint8_t CHAROFFSET = font[2]; //char start offset

  font[3]; //char count
  if (Main_char != '\n' && Main_char != '\r')
    for (uint16_t Ypos = 0; Ypos < YcharSize; Ypos++)
      for (uint16_t Xpos = 0; Xpos < XcharSize; Xpos += 8) {
        uint8_t CHARLINE = 0;
        if (XcharSize < 8)
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * 1) + (Ypos) * 1 + Xpos / 8  + 4];
        else
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * (XcharSize / 8)) +   (Ypos) * (XcharSize / 8) + Xpos / 8  + 4];

        if ((Xpos + 0 < XcharSize) && (CHARLINE & 0b10000000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 0, Main_y + Ypos, color);
        if ((Xpos + 1 < XcharSize) && (CHARLINE & 0b01000000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 1, Main_y + Ypos, color);
        if ((Xpos + 2 < XcharSize) && (CHARLINE & 0b00100000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 2, Main_y + Ypos, color);
        if ((Xpos + 3 < XcharSize) && (CHARLINE & 0b00010000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 3, Main_y + Ypos, color);
        if ((Xpos + 4 < XcharSize) && (CHARLINE & 0b00001000) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 4, Main_y + Ypos, color);
        if ((Xpos + 5 < XcharSize) && (CHARLINE & 0b00000100) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 5, Main_y + Ypos, color);
        if ((Xpos + 6 < XcharSize) && (CHARLINE & 0b00000010) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 6, Main_y + Ypos, color);
        if ((Xpos + 7 < XcharSize) && (CHARLINE & 0b00000001) != 0) screenmemory_drawpixel_static(Main_x + Xpos + 7, Main_y + Ypos, color);
      }
  return XcharSize;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t draw_char_xy_transparent_double(uint16_t Main_x, uint16_t Main_y, char Main_char,
  const char * font, uint32_t color) {
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint8_t CHAROFFSET = font[2]; //char start offset

  if (Main_char != '\n' && Main_char != '\r')
    for (uint16_t Ypos = 0; Ypos < YcharSize; Ypos += 1)
      for (uint16_t Xpos = 0; Xpos < XcharSize; Xpos += 8) {
        uint8_t CHARLINE = 0;
        if (XcharSize < 8)
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * 1) + (Ypos) * 1 + Xpos / 8 + 4];
        else
          CHARLINE = font[(Main_char - CHAROFFSET) * (YcharSize * (XcharSize / 8)) + (Ypos) * (XcharSize / 8) + Xpos / 8 + 4];

        if ((Xpos + 0 < XcharSize) && (CHARLINE & 0b10000000) != 0) {
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 0, Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 0 + 1 , Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 0, Main_y + Ypos*2 + 1, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 0 + 1, Main_y + Ypos*2 + 1, color);
        }    
        if ((Xpos + 1 < XcharSize) && (CHARLINE & 0b01000000) != 0) {
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 2, Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 2 + 1, Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 2, Main_y + Ypos*2 + 1, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 2 + 1, Main_y + Ypos*2 + 1, color);
        }    
        if ((Xpos + 2 < XcharSize) && (CHARLINE & 0b00100000) != 0) {
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 4, Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 4 + 1, Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 4, Main_y + Ypos*2 + 1, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 4 + 1, Main_y + Ypos*2 + 1, color);
        }    
        if ((Xpos + 3 < XcharSize) && (CHARLINE & 0b00010000) != 0) {
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 6, Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 6 + 1, Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 6, Main_y + Ypos*2 + 1, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 6 + 1, Main_y + Ypos*2 + 1, color);
        }    
        if ((Xpos + 4 < XcharSize) && (CHARLINE & 0b00001000) != 0) {
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 8, Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 8 + 1, Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 8, Main_y + Ypos*2 + 1, color);           
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 8 + 1, Main_y + Ypos*2 + 1, color);
        }    
        if ((Xpos + 5 < XcharSize) && (CHARLINE & 0b00000100) != 0) {            
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 10, Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 10 + 1, Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 10, Main_y + Ypos*2 + 1, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 10 + 1, Main_y + Ypos*2 + 1, color);
        }    
        if ((Xpos + 6 < XcharSize) && (CHARLINE & 0b00000010) != 0) {            
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 12, Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 12 + 1, Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 12, Main_y + Ypos*2 + 1, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 12 + 1, Main_y + Ypos*2 + 1, color);
        }    
        if ((Xpos + 7 < XcharSize) && (CHARLINE & 0b00000001) != 0) {            
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 14, Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 14 + 1, Main_y + Ypos*2, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 14, Main_y + Ypos*2 + 1, color);
            screenmemory_drawpixel_static(Main_x + Xpos*2 + 14 + 1, Main_y + Ypos*2 + 1, color);
        }    
      }
  return XcharSize*2;
}
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
uint16_t draw_string_xy(uint16_t x, uint16_t y, const char *c, const char* font, uint8_t color = 0xff, uint8_t bgcolor = 0)
{
  uint8_t width;
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint16_t textwidth = 0;
  while (*c) {
    width = draw_char_xy(x, y, *c, font, color, bgcolor);
    textwidth += (width);
    x += (width);
    c++;
  }
  if (LCD_ENABLED) xSemaphoreGive(draw_mutex);
  return textwidth;
}
//--------------------------------------------------------------------------------
uint16_t draw_string_xy_static(uint16_t x, uint16_t y, const char *c, const char* font, uint16_t color = 0xffff, uint16_t bgcolor = 0)
{
  uint8_t width;
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint16_t textwidth = 0;
  while (*c) {
    width = draw_char_xy_static(x, y, *c, font, color, bgcolor);
    textwidth += (width);
    x += (width);
    c++;
  }
  return textwidth;
}

//--------------------------------------------------------------------------------
uint16_t draw_string_xy_transparent(uint16_t x, uint16_t y, const char *c, const char* font, uint16_t color = 0xffff) {
  uint8_t width;
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint16_t textwidth = 0;
  while (*c) {
    width = draw_char_xy_static_transparent(x, y, *c, font, color);
    textwidth += (width);
    x += (width);
    c++;
  }
  return textwidth;
}
//--------------------------------------------------------------------------------
uint16_t draw_string_xy_transparent_double(uint16_t x, uint16_t y, const char *c, const char* font, uint16_t color = 0xffff) {
  uint8_t width;
  uint8_t XcharSize = font[0]; //x char size
  uint8_t YcharSize = font[1]; //y char size
  uint16_t textwidth = 0;
  while (*c) {
    width = draw_char_xy_transparent_double(x, y, *c, font, color);
    textwidth += (width);
    x += (width);
    c++;
  }
  return textwidth;
}
//--------------------------------------------------------------------------------
const char* DisplayFontSet = NULL;
uint16_t XPOS_CHAR = 0;
uint16_t YPOS_CHAR = 0;
//--------------------------------------------------------------------------------
void set_font(const char* font) {
  DisplayFontSet = font;
}
//--------------------------------------------------------------------------------
void set_font_XY(uint16_t x, uint16_t y) {
  XPOS_CHAR = x;
  YPOS_CHAR = y;
}
//--------------------------------------------------------------------------------
void draw_string(char *c, uint8_t color = 255, uint8_t bgcolor = 0) {
  if (c[strlen(c) - 1] == '\n') {
    draw_string_xy(XPOS_CHAR, YPOS_CHAR, c, DisplayFontSet, color);
    YPOS_CHAR += (uint8_t)(DisplayFontSet[1]);
    XPOS_CHAR = 0;
  } else {
    XPOS_CHAR += draw_string_xy(XPOS_CHAR, YPOS_CHAR, c, DisplayFontSet, color, bgcolor);
  }
}

//--------------------------------------------------------------------------------
void draw_string_static(char *c, uint16_t color = 0xffff, uint16_t bgcolor = 0) {
  if (c[strlen(c) - 1] == '\n') {
    draw_string_xy_static(XPOS_CHAR, YPOS_CHAR, c, DisplayFontSet, color);
    YPOS_CHAR += (uint8_t)(DisplayFontSet[1]);
    XPOS_CHAR = 0;
  } else {
    XPOS_CHAR += draw_string_xy_static(XPOS_CHAR, YPOS_CHAR, c, DisplayFontSet, color, bgcolor);
  }
}

//--------------------------------------------------------------------------------
void DrawPalette() {
  uint8_t COLOR = 0;
  for (uint16_t YPOS = 0; YPOS < 16; YPOS++)
    for (uint16_t XPOS = 0; XPOS < 16; XPOS++) {
      screenmemory_drawfillrectangle( (240 - 192) / 2 + XPOS * 12, (240 - 192) / 2 +  YPOS * 12, 12, 12, COLOR);
      COLOR++;
    }
}
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
void RESTORE_BACKGROUND(int XPOS, int YPOS, int WIDTH, int HEIGHT ) {
  DRAWMODE = 0; //this is important!
#ifndef NCAT_NO_PSRAM
  uint16_t bg_COLOR = 0;
  for (int16_t bgtmpY = 0; bgtmpY < HEIGHT; bgtmpY++) {
    for (int16_t bgtmpX = 0; bgtmpX < WIDTH; bgtmpX++) {

      if (BACKGROUND_BUFFER) { //load baground pixel from BACKGROUND_BUFFER if exist (65536colors)
        if (((YPOS +  bgtmpY) * LCD_WIDTH + (XPOS + bgtmpX)) * 2 <= BACKGROUND_BUFFER_SIZE)
          bg_COLOR = BACKGROUND_BUFFER[(YPOS +  bgtmpY) * LCD_WIDTH + (XPOS + bgtmpX) ];
      } else {                 //load baground pixel from SCREENMEMORY (256 colors)
        bg_COLOR = default_16bit[(SCREENMEMORY_LINE[YPOS +  bgtmpY ][XPOS + bgtmpX ]) /*& 0b00111111*/];
      }
      ((uint16_t*)SCREENBUFFER)[bgtmpX % 512] = bg_COLOR;

      //draw bacground on SCREENMOMORY 64 colors
      uint16_t bg_COLOR_swap = ((bg_COLOR & 0xff) << 8) | ((bg_COLOR & 0xff00) >> 8);
      uint8_t bg_Red = (bg_COLOR_swap & 0b1111100000000000) >> 11;
      uint8_t bg_Green = (bg_COLOR_swap & 0b0000011111000000) >> 6;
      uint8_t bg_Blue = (bg_COLOR_swap & 0b0000000000011111) >> 0;
      //0bGGGGGGGGYYYYYYYYRRRRRRRRBBBBBBBB
      uint8_t reducedCOLOR = (((bg_Green >> 3) << 6) | ((bg_Red >> 2) << 3) | ((bg_Blue >> 2) << 0)); //RGB
      if (XPOS + bgtmpX > 0 && XPOS + bgtmpX < LCD_WIDTH)
        SCREENMEMORY_LINE[YPOS +  bgtmpY ][XPOS + bgtmpX ] = reducedCOLOR /*& 0b00111111*/;
    }

    if (WIDTH > LCD_WIDTH && YPOS + bgtmpY < LCD_HEIGHT) {
      #if LCD_ENABLED
        tft.pushImage(XPOS , YPOS + bgtmpY , LCD_WIDTH, 1, &SCREENBUFFER[0]); //draw line
      #endif
    } else {
      #if LCD_ENABLED
        tft.pushImage(XPOS , YPOS + bgtmpY , WIDTH, 1, &SCREENBUFFER[0]);        //draw line
      #endif
    }
  }
#else 
  uint16_t bg_COLOR = 0;
  for (int16_t bgtmpY = 0; bgtmpY < HEIGHT; bgtmpY++) {
    for (int16_t bgtmpX = 0; bgtmpX < WIDTH; bgtmpX++) {

      ((uint16_t*)SCREENBUFFER)[bgtmpX % 512] = bg_COLOR;

      //draw bacground on SCREENMOMORY 64 colors
      uint16_t bg_COLOR_swap = ((bg_COLOR & 0xff) << 8) | ((bg_COLOR & 0xff00) >> 8);
      uint8_t bg_Red = (bg_COLOR_swap & 0b1111100000000000) >> 11;
      uint8_t bg_Green = (bg_COLOR_swap & 0b0000011111000000) >> 6;
      uint8_t bg_Blue = (bg_COLOR_swap & 0b0000000000011111) >> 0;
      //0bGGGGGGGGYYYYYYYYRRRRRRRRBBBBBBBB
      uint8_t reducedCOLOR = (((bg_Green >> 3) << 6) | ((bg_Red >> 2) << 3) | ((bg_Blue >> 2) << 0)); //RGB
      if (XPOS + bgtmpX > 0 && XPOS + bgtmpX < LCD_WIDTH)
        SCREENMEMORY_LINE[YPOS +  bgtmpY ][XPOS + bgtmpX ] = reducedCOLOR /*& 0b00111111*/;
    }

    if (WIDTH > LCD_WIDTH && YPOS + bgtmpY < LCD_HEIGHT) {
      #if LCD_ENABLED
        tft.pushImage(XPOS , YPOS + bgtmpY , LCD_WIDTH, 1, &SCREENBUFFER[0]); //draw line
      #endif
    } else {
      #if LCD_ENABLED
        tft.pushImage(XPOS , YPOS + bgtmpY , WIDTH, 1, &SCREENBUFFER[0]);        //draw line
      #endif
    }
  }

#endif  

}
//--------------------------------------------------------------------------------









#if JPG_DECODER
//********************************************************************************
// JPG RENDER FUNCTIONS
//********************************************************************************
uint32_t dataIdx = 0; //actual input buffer read index
uint32_t dataLen = 0; //size of input data
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint32_t loadMem(JDEC* jdec, uint8_t* buf, uint32_t len) {
  vPortYield();
  if (dataIdx + len > dataLen) len = dataLen - dataIdx;
  if (buf) memcpy(buf, JPGDATA + dataIdx, len);
  //if (buf) printf("JPG BUF %d: %d\n", dataIdx, ((uint8_t*)buf)[dataIdx]);
  dataIdx += len;
  return len;
}
//--------------------------------------------------------------------------------
uint32_t renderindex = 0;
uint16_t jpgXPOS = 0;
uint16_t jpgYPOS = 0;
uint16_t jpgtmpX = 0;
uint16_t jpgtmpY = 0;
//--------------------------------------------------------------------------------
uint8_t jpg_Red;
uint8_t jpg_Green;
uint8_t jpg_Blue;
uint16_t jpg_COLOR;
//********************************************************************************
int renderLCD(JDEC* jdec, uint16_t* bitmap, JRECT* jrect)
{
  // uint16_t jpgwidth = jdec->width;
  // uint16_t jpgheight = jdec->height;
  uint16_t  x = jrect->left + 0;
  uint16_t  y = jrect->top  + 0;
  uint16_t w = jrect->right  + 1 - jrect->left;
  uint16_t h = jrect->bottom + 1 - jrect->top;
#if DEBUG>5
  printf("JPG renderindex: %d\n", renderindex);
#endif
  for (jpgtmpY = 0; jpgtmpY < h; jpgtmpY++) {
    for (jpgtmpX = 0; jpgtmpX < w; jpgtmpX++) {
      jpg_Red = ((uint8_t*)bitmap)[(jpgtmpY * w + jpgtmpX) * 3 + 0];
      jpg_Green = ((uint8_t*)bitmap)[(jpgtmpY * w + jpgtmpX)  * 3 + 1];
      jpg_Blue = ((uint8_t*)bitmap)[(jpgtmpY * w + jpgtmpX)  * 3 + 2];

      jpg_Red = (jpg_Red & 0b11111000) >> 3;
      jpg_Green = (jpg_Green & 0b11111000) >> 3;
      jpg_Blue = (jpg_Blue & 0b11111000) >> 3;

      jpg_COLOR = ((jpg_Red << 11) | (jpg_Green << 6) | (jpg_Blue << 0)); //RGB

      SCREENMEMORY_LINE[jpgYPOS + y + jpgtmpY ][jpgXPOS + x + jpgtmpX ] = (((jpg_Green >> 3) << 6) | ((jpg_Red >> 2) << 3) | ((jpg_Blue >> 3) << 0)); //RGB 256colors
      ((uint16_t*)SCREENBUFFER)[jpgtmpX % 512] = ((jpg_COLOR & 0xff) << 8) | ((jpg_COLOR & 0xff00) >> 8); //max 512 WORDS pre line
    }
    if (w < 512) {
      //schema: 0bGGBBBBBRRRRRGGG, R=RED, G=GREEN, B=BLUE
      #if LCD_ENABLED
        tft.pushImage(x + jpgXPOS, y + jpgYPOS + jpgtmpY , w, 1, &SCREENBUFFER[0]); //draw line
      #endif
    }
  }
  renderindex++;
  return 1;
}
//--------------------------------------------------------------------------------
void renderJPG(int XPOS, int YPOS, const uint8_t* inputdata, uint32_t inputdataLen, uint8_t* decodeBuffer, uint32_t decodeBufferLen, int jpgScale) {
  dataIdx = 0;
  dataLen = inputdataLen;
  jpgXPOS = XPOS;
  jpgYPOS = YPOS;

  JPGDATA = (uint8_t*)inputdata;

  DRAWMODE = 0; //this is important!

  jpgres = jd_prepare(&jpgdec, loadMem, decodeBuffer, decodeBufferLen, 0);
  #if DEBUG>2
    printf("JPG PREPARE STATUS: %d\n", jpgres);
  #endif
  if (jpgres == JDR_OK) jpgres = jd_decomp(&jpgdec, renderLCD, jpgScale);
  #if DEBUG>2
    printf("JPG DECODE STATUS: %d\n", jpgres);
  #endif
}
//--------------------------------------------------------------------------------
uint16_t* tmpJPGRenderbuffer;
int renderBUFFER(JDEC* jdec, uint16_t* bitmap, JRECT* jrect)
{
  //uint16_t jpgwidth = jdec->width;
  //uint16_t jpgheight = jdec->height;
  uint16_t  x = jrect->left + 0;
  uint16_t  y = jrect->top  + 0;
  uint16_t w = jrect->right  + 1 - jrect->left;
  uint16_t h = jrect->bottom + 1 - jrect->top;

  DRAWMODE = 0; //this is important!

  #if DEBUG>5
    printf("JPG renderindex: %d\n", renderindex);
  #endif
  for (jpgtmpY = 0; jpgtmpY < h; jpgtmpY++) {
    for (jpgtmpX = 0; jpgtmpX < w; jpgtmpX++) {

      jpg_Red = ((uint8_t*)bitmap)[(jpgtmpY * w + jpgtmpX) * 3 + 0];
      jpg_Green = ((uint8_t*)bitmap)[(jpgtmpY * w + jpgtmpX)  * 3 + 1];
      jpg_Blue = ((uint8_t*)bitmap)[(jpgtmpY * w + jpgtmpX)  * 3 + 2];

      jpg_Red = (jpg_Red & 0b11111000) >> 3;
      jpg_Green = (jpg_Green & 0b11111000) >> 3;
      jpg_Blue = (jpg_Blue & 0b11111000) >> 3;

      jpg_COLOR = ((jpg_Red << 11) | (jpg_Green << 6) | (jpg_Blue << 0)); //RGB
      SCREENMEMORY_LINE[jpgYPOS + y + jpgtmpY ][jpgXPOS + x + jpgtmpX ] = (((jpg_Green >> 3) << 6) | ((jpg_Red >> 2) << 3) | ((jpg_Blue >> 2) << 0)); //RGB
      if (tmpJPGRenderbuffer) { //save to BUFFER pixel
        ((uint16_t*)tmpJPGRenderbuffer)[(jpgYPOS + y + jpgtmpY)*LCD_WIDTH  + (jpgXPOS + x + jpgtmpX)] = ((jpg_COLOR & 0xff) << 8) | ((jpg_COLOR & 0xff00) >> 8);
      }
      ((uint16_t*)SCREENBUFFER)[jpgtmpX % 512] = ((jpg_COLOR & 0xff) << 8) | ((jpg_COLOR & 0xff00) >> 8); //max 512 WORDS pre line
    }
    if (w < 512) {
      //schema: 0bGGBBBBBRRRRRGGG, R=RED, G=GREEN, B=BLUE
      #if LCD_ENABLED
        tft.pushImage(x + jpgXPOS, y + jpgYPOS + jpgtmpY , w, 1, &SCREENBUFFER[0]); //draw line
      #endif
    }
  }
  renderindex++;
  return 1;
}
//----------------------------------------------------------------------------------------
void renderJPG_toBUFFER(uint16_t* renderJPGbuf, int XPOS, int YPOS, const uint8_t* inputdata, uint32_t inputdataLen, uint8_t* decodeBuffer, uint32_t decodeBufferLen, int jpgScale) {
  dataIdx = 0;
  dataLen = inputdataLen;
  jpgXPOS = XPOS;
  jpgYPOS = YPOS;

  JPGDATA = (uint8_t*)inputdata;

  DRAWMODE = 0; //this is important!

  tmpJPGRenderbuffer = renderJPGbuf; //set pointer to output buffer

  jpgres = jd_prepare(&jpgdec, loadMem, decodeBuffer, decodeBufferLen, 0);
  #if DEBUG>2
    printf("JPG PREPARE STATUS: %d\n", jpgres);
  #endif
  if (jpgres == JDR_OK) {
  #if DEBUG>2
    printf("jpgdec.width: %d jpgdec.height: %d\n", jpgdec.width, jpgdec.height);
  #endif

    if (jpgdec.height >= LCD_HEIGHT) jpgdec.height = LCD_HEIGHT;
    if (jpgdec.width >= LCD_WIDTH) jpgdec.width = LCD_WIDTH;
    jpgres = jd_decomp(&jpgdec, renderBUFFER, jpgScale);
  }
  #if DEBUG>2
    printf("JPG DECODE STATUS: %d\n", jpgres);
  #endif
}
#endif
//********************************************************************************





//********************************************************************************
//  PNG RENDER FUNCTIONS
//********************************************************************************
#if PNG_DECODER
//--------------------------------------------------------------------------------
upng_t* upng_;
//--------------------------------------------------------------------------------
uint16_t pngXPOS = 0;
uint16_t pngYPOS = 0;
uint16_t pngtmpX = 0;
uint16_t pngtmpY = 0;
uint8_t png_Red;
uint8_t png_Green;
uint8_t png_Blue;
uint16_t png_COLOR;
//--------------------------------------------------------------------------------
void renderPNG(int XPOS, int YPOS, const uint8_t* inputdata, uint32_t inputdataLen, uint8_t* decodeBuffer, uint32_t decodeBufferLen, uint16_t transparentCOLOR) {
  DRAWMODE = 0; //this is important!


  upng_ = upng_new_from_bytes((unsigned char*)inputdata, inputdataLen);
  upng_decode(upng_);
  if (upng_get_error(upng_) != UPNG_EOK) {
    printf("PNG error: %u, LINE in CODE: %u\n", upng_get_error(upng_), upng_get_error_line(upng_));
  }
  uint16_t WIDTH = upng_get_width(upng_);
  uint16_t HEIGHT = upng_get_height(upng_);
  #if DEBUG>2
    printf("PNG width: %d, height: %d \n", WIDTH, HEIGHT);
  #endif

  decodeBuffer = (uint8_t*)upng_get_buffer(upng_);

  for (pngtmpY = 0; pngtmpY < HEIGHT; pngtmpY++) {
    for (pngtmpX = 0; pngtmpX < WIDTH; pngtmpX++) {

      png_Red = ((uint8_t*)decodeBuffer)[(pngtmpY * WIDTH + pngtmpX) * 3 + 0];
      png_Green = ((uint8_t*)decodeBuffer)[(pngtmpY * WIDTH + pngtmpX)  * 3 + 1];
      png_Blue = ((uint8_t*)decodeBuffer)[(pngtmpY * WIDTH + pngtmpX)  * 3 + 2];

      uint8_t reducedCOLOR = ( ((png_Green >> 6) << 6) | ((png_Red >> 5) << 3) | ((png_Blue >> 5) << 0) ); //RGB

      png_Red = (png_Red & 0b11111000) >> 3;
      png_Green = (png_Green & 0b11111100) >> 2;
      png_Blue = (png_Blue & 0b11111000) >> 3;

      png_COLOR = ((png_Red << 11) | (png_Green << 5) | (png_Blue << 0)); //RGB

      //0bGGGGGGGGYYYYYYYYRRRRRRRRBBBBBBBB
      if (png_COLOR != transparentCOLOR) { //not transparent color
        SCREENMEMORY_LINE[YPOS +  pngtmpY ][XPOS + pngtmpX ] = reducedCOLOR /*& 0b00111111*/; //256 colors
        ((uint16_t*)SCREENBUFFER)[pngtmpX % 512] = ((png_COLOR & 0xff) << 8) | ((png_COLOR & 0xff00) >> 8); //max 512 WORDS pre line
      }  else { //transparent color
        if (BACKGROUND_BUFFER) { //load baground pixel from BACKGROUND_BUFFER if exist (65536colors)
          if (((YPOS +  pngtmpY) * LCD_WIDTH + (XPOS + pngtmpX)) * 2 <= BACKGROUND_BUFFER_SIZE)
            png_COLOR = BACKGROUND_BUFFER[(YPOS +  pngtmpY) * LCD_WIDTH + (XPOS + pngtmpX) ];
        } else {                 //load baground pixel from SCREENMEMORY (64/256 colors)
          png_COLOR = default_16bit[(SCREENMEMORY_LINE[YPOS +  pngtmpY ][XPOS + pngtmpX ]) /*& 0b00111111*/];
        }
        ((uint16_t*)SCREENBUFFER)[pngtmpX % 512] = png_COLOR;

        //draw bacground on SCREENMOMORY 64 colors
        uint16_t png_COLOR_swap = ((png_COLOR & 0xff) << 8) | ((png_COLOR & 0xff00) >> 8);
        png_Red = (png_COLOR_swap & 0b1111100000000000) >> 11;
        png_Green = (png_COLOR_swap & 0b0000011111000000) >> 6;
        png_Blue = (png_COLOR_swap & 0b0000000000011111) >> 0;
        //0bGGGGGGGGYYYYYYYYRRRRRRRRBBBBBBBB
        reducedCOLOR = (((png_Green >> 3) << 6) | ((png_Red >> 2) << 3) | ((png_Blue >> 2) << 0)); //RGB 256colors

        if (XPOS + pngtmpX > 0 && XPOS + pngtmpX < LCD_WIDTH)
          SCREENMEMORY_LINE[YPOS +  pngtmpY ][XPOS + pngtmpX ] = reducedCOLOR /*& 0b00111111*/;
        }
      }
      if (WIDTH > LCD_WIDTH && YPOS + pngtmpY < LCD_HEIGHT) {
        #if LCD_ENABLED
          tft.pushImage(XPOS , YPOS + pngtmpY , LCD_WIDTH, 1, &SCREENBUFFER[0]); //draw line
        #endif
      } else {
        #if LCD_ENABLED
          tft.pushImage(XPOS , YPOS + pngtmpY , WIDTH, 1, &SCREENBUFFER[0]); //draw line
        #endif
      }
  }
  upng_free(upng_);
}
//--------------------------------------------------------------------------------
void animatePNG(int offset_direction, int stopXpos, int XPOS, int YPOS, const uint8_t* inputdata, uint32_t inputdataLen, uint8_t* decodeBuffer, uint32_t decodeBufferLen, uint16_t transparentCOLOR) {

  DRAWMODE = 0; //this is important!

  upng_ = upng_new_from_bytes((unsigned char*)inputdata, inputdataLen);
  upng_decode(upng_);
  if (upng_get_error(upng_) != UPNG_EOK) {
    printf("PNG error: %u, LINE in CODE: %u\n", upng_get_error(upng_), upng_get_error_line(upng_));
    return;
  }
  uint16_t WIDTH = upng_get_width(upng_);
  uint16_t HEIGHT = upng_get_height(upng_);
  #if DEBUG>2
    printf("PNG width: %d, height: %d \n", WIDTH, HEIGHT);
  #endif

  decodeBuffer = (uint8_t*)upng_get_buffer(upng_);

  int16_t XPOS_save = XPOS;
  int16_t XoffsetINC = offset_direction; //-8 to left //+8 to right

  for (int16_t animateX = XPOS_save; ((animateX <= stopXpos && XoffsetINC > 0) || (animateX >= stopXpos && XoffsetINC < 0)) && (animateX >= 0 - LCD_WIDTH) && (animateX <= LCD_WIDTH * 2); animateX += XoffsetINC) {
    vPortYield(); 
    #if DEBUG>5
      printf("animateX:%d\n", animateX);
    #endif
    XPOS = animateX;

    for (pngtmpY = 0; pngtmpY < HEIGHT; pngtmpY++) {
      vPortYield(); 
      for (pngtmpX = 0; pngtmpX < WIDTH; pngtmpX++) {
        png_Red = ((uint8_t*)decodeBuffer)[(pngtmpY * WIDTH + pngtmpX) * 3 + 0];
        png_Green = ((uint8_t*)decodeBuffer)[(pngtmpY * WIDTH + pngtmpX)  * 3 + 1];
        png_Blue = ((uint8_t*)decodeBuffer)[(pngtmpY * WIDTH + pngtmpX)  * 3 + 2];

        uint8_t reducedCOLOR = ( ((png_Green >> 6) << 6) | ((png_Red >> 5) << 3) | ((png_Blue >> 5) << 0) ); //RGB

        png_Red = (png_Red & 0b11111000) >> 3;
        png_Green = (png_Green & 0b11111100) >> 2;
        png_Blue = (png_Blue & 0b11111000) >> 3;

        png_COLOR = ((png_Red << 11) | (png_Green << 5) | (png_Blue << 0)); //RGB

        //0bGGGGGGGGYYYYYYYYRRRRRRRRBBBBBBBB
        if (png_COLOR != transparentCOLOR) { //not transparent color
          if (XPOS + pngtmpX > 0 && XPOS + pngtmpX < LCD_WIDTH)
            SCREENMEMORY_LINE[YPOS +  pngtmpY ][XPOS + pngtmpX ] = reducedCOLOR /*& 0b00111111*/;
          ((uint16_t*)SCREENBUFFER)[pngtmpX % 512] = ((png_COLOR & 0xff) << 8) | ((png_COLOR & 0xff00) >> 8); //max 512 WORDS pre line

        }  else { //transparent color
          if (BACKGROUND_BUFFER) { //load baground pixel from BACKGROUND_BUFFER if exist (65536colors)
            if (((YPOS +  pngtmpY) * LCD_WIDTH + (XPOS + pngtmpX)) * 2 <= BACKGROUND_BUFFER_SIZE)
              png_COLOR = BACKGROUND_BUFFER[(YPOS +  pngtmpY) * LCD_WIDTH + (XPOS + pngtmpX) ];
          } else {                 //load baground pixel from SCREENMEMORY (64 colors)
            png_COLOR = default_16bit[(SCREENMEMORY_LINE[YPOS +  pngtmpY ][XPOS + pngtmpX ]) /*& 0b00111111*/];
          }
          ((uint16_t*)SCREENBUFFER)[pngtmpX % 512] = png_COLOR;

          //draw bacground on SCREENMOMORY 64 colors
          uint16_t png_COLOR_swap = ((png_COLOR & 0xff) << 8) | ((png_COLOR & 0xff00) >> 8);
          png_Red = (png_COLOR_swap & 0b1111100000000000) >> 11;
          png_Green = (png_COLOR_swap & 0b0000011111000000) >> 6;
          png_Blue = (png_COLOR_swap & 0b0000000000011111) >> 0;
          //0bGGGGGGGGYYYYYYYYRRRRRRRRBBBBBBBB
          reducedCOLOR = ( ((png_Green >> 3) << 6) | ((png_Red >> 2) << 3) | ((png_Blue >> 2) << 0) ); //RGB

          if (XPOS + pngtmpX > 0 && XPOS + pngtmpX < LCD_WIDTH)
            SCREENMEMORY_LINE[YPOS +  pngtmpY ][XPOS + pngtmpX ] = reducedCOLOR /*& 0b00111111*/;
        }
      }



      //draw pixels to LCD screen 65536 colors
      if (WIDTH > LCD_WIDTH && YPOS + pngtmpY < LCD_HEIGHT) {
        #if LCD_ENABLED
          tft.pushImage(XPOS , YPOS + pngtmpY , LCD_WIDTH, 1, &SCREENBUFFER[0]); //draw line
        #endif
      } else {
        #if LCD_ENABLED
          tft.pushImage(XPOS , YPOS + pngtmpY , WIDTH, 1, &SCREENBUFFER[0]); //draw line
        #endif
        delayMicroseconds___(100);
      }
    }
  }
  upng_free(upng_);
}
//--------------------------------------------------------------------------------




void renderTRANSPARENTRECT(uint16_t* TMP_BACKGROUND_BUFFER, uint16_t XPOS, uint16_t YPOS, uint16_t WIDTH, uint16_t HEIGHT) {
  DRAWMODE = 0; //this is important!

  for (uint8_t tmpY = 0; tmpY < HEIGHT; tmpY++) {
    for (uint8_t tmpX = 0; tmpX < WIDTH; tmpX++) {
      uint16_t TMP_COLOR = TMP_BACKGROUND_BUFFER[(tmpY + YPOS) * LCD_WIDTH + (tmpX + XPOS)];
      uint16_t TMP_COLOR_REORDER = ((TMP_COLOR & 0xff) << 8) | ((TMP_COLOR & 0xff00) >> 8);

      png_Red = (TMP_COLOR_REORDER & 0b1111100000000000) >> 11;
      png_Green = (TMP_COLOR_REORDER & 0b0000011111100000) >> 6;
      png_Blue = (TMP_COLOR_REORDER & 0b0000000000011111) >> 0;

      png_Red = ((png_Red >> 1) | 0b00010000) & 0b00011111;
      png_Green = ((png_Green >> 1) | 0b00010000) & 0b00011111;
      png_Blue = ((png_Blue >> 1) | 0b00010000) & 0b00011111;

      TMP_COLOR_REORDER = (((png_Green >> 0) << 6) | ((png_Red >> 0) << 11) | ((png_Blue >> 0) << 0)); //RGB
      TMP_COLOR = ((TMP_COLOR_REORDER & 0xff) << 8) | ((TMP_COLOR_REORDER & 0xff00) >> 8);

      //0bGGGGGGGGYYYYYYYYRRRRRRRRBBBBBBBB
      uint8_t reducedCOLOR = (((png_Green >> 3) << 6) | ((png_Red >> 2) << 3) | ((png_Blue >> 2) << 0)); //RGB
      SCREENMEMORY_LINE[YPOS +  tmpY ][XPOS + tmpX ] = reducedCOLOR; //256 colors

      TMP_COLOR = ((TMP_COLOR_REORDER & 0xff) << 8) | ((TMP_COLOR_REORDER & 0xff00) >> 8);
      TMP_BACKGROUND_BUFFER[(tmpY + YPOS)*LCD_WIDTH + (tmpX + XPOS)] = TMP_COLOR;
    }
    #if LCD_ENABLED
      tft.pushImage(XPOS , (tmpY + YPOS) , WIDTH, 1, TMP_BACKGROUND_BUFFER + ((tmpY + YPOS)*LCD_WIDTH)); //draw line
    #endif
  }
}
//--------------------------------------------------------------------------------
#endif



//--------------------------------------------------------------------------------
void animateTEXT(int offset_direction, int stopXpos, int XPOS, int YPOS, char* TEXT, uint16_t COLOR) { 
  DRAWMODE = 0; //this is important!
  int16_t XPOS_save = XPOS;
  int16_t XoffsetINC = offset_direction; //-8 to left //+8 to right
  for (int16_t animateX = XPOS_save; ((animateX <= stopXpos && XoffsetINC > 0) || (animateX >= stopXpos && XoffsetINC < 0)) && (animateX >= 0 - LCD_WIDTH) && (animateX <= LCD_WIDTH * 2); animateX += XoffsetINC) {
    vPortYield(); 
    #if DEBUG>5
      printf("animateX:%d\n", animateX);
    #endif
    XPOS = animateX;   
    screenmemory_drawfillrectangle_static(XPOS-16, YPOS, strlen(TEXT)*16 + 32*2 , 32, 0);
    draw_string_xy_transparent_double(XPOS, YPOS, (char*)TEXT, font_Ishmeria8x16, COLOR);
    delay___(40);
  }  
}
//--------------------------------------------------------------------------------



#if HIMEM
#include "soc/dport_reg.h" //FOR HIMEM support

//********************************************************************************
//HIMEM FUNCTION...
//********************************************************************************
//--------------------------------------------------------------------------------
esp_himem_handle_t mh; //Handle for the address space we're using
esp_himem_rangehandle_t rh; //Handle for the actual RAM.

uint8_t *init_ptr = NULL;
#endif
//--------------------------------------------------------------------------------
void himem_malloc(uint32_t himem_size) {
#if HIMEM
  //Allocate the memory we're going to check.
  ESP_ERROR_CHECK(esp_himem_alloc(himem_size, &mh));
  //Allocate a block of address range
  ESP_ERROR_CHECK(esp_himem_alloc_map_range(ESP_HIMEM_BLKSZ, &rh));
#endif
}
//--------------------------------------------------------------------------------
void himem_free() {
#if HIMEM
  ESP_ERROR_CHECK(esp_himem_free(mh));
  ESP_ERROR_CHECK(esp_himem_free_map_range(rh));
#endif
}
//--------------------------------------------------------------------------------
#if HIMEM
int32_t HIMEMOFFSET = ESP_HIMEM_BLKSZ; // himem offset 32kB
#endif

void himem_write(uint32_t ADDRESS, uint32_t SIZE, uint8_t* DATABUF) {
#if HIMEM
  uint8_t *ptr = NULL;

  vPortYield(); // BT need this!!!
  vTaskDelay(1);

  if (SIZE <= ESP_HIMEM_BLKSZ) {
    int32_t STARTADDRESS = HIMEMOFFSET +  ADDRESS - (ADDRESS % ESP_HIMEM_BLKSZ)  - ESP_HIMEM_BLKSZ ;
    int32_t STARTOFFSET = ADDRESS % ESP_HIMEM_BLKSZ;
    int32_t STARTSIZE = (SIZE % ESP_HIMEM_BLKSZ);   
    if (SIZE == ESP_HIMEM_BLKSZ) STARTSIZE += ESP_HIMEM_BLKSZ;

    int32_t NEXTADDRESS = HIMEMOFFSET + ADDRESS - (ADDRESS % ESP_HIMEM_BLKSZ)  ;
    int32_t NEXTOFFSET = 0;
    int32_t NEXTSIZE = SIZE - STARTSIZE;

    if (STARTSIZE > 0) {
      #if DEBUG>4
        printf("     STARTADDRESS: %d    STARTOFFSET: %d    STARTSIZE: %d    \n", STARTADDRESS, STARTOFFSET, STARTSIZE);
      #endif
      if (init_ptr) ESP_ERROR_CHECK(esp_himem_unmap(rh, init_ptr, ESP_HIMEM_BLKSZ));
      init_ptr = NULL;
      ESP_ERROR_CHECK(esp_himem_map(mh, rh, STARTADDRESS, 0, ESP_HIMEM_BLKSZ, 0, &init_ptr)); // round to ESP_HIMEM_BLKSZ
      ptr = init_ptr;

      memcpy( &ptr[STARTOFFSET], &DATABUF[0], STARTSIZE );

      if (init_ptr) ESP_ERROR_CHECK(esp_himem_unmap(rh, init_ptr, ESP_HIMEM_BLKSZ));
    }

    if (NEXTSIZE > 0) {
      #if DEBUG>4
        printf("     NEXTADDRESS: %d    NEXTOFFSET: %d    NEXTSIZE: %d    \n", NEXTADDRESS, NEXTOFFSET, NEXTSIZE);
      #endif
      if (init_ptr) ESP_ERROR_CHECK(esp_himem_unmap(rh, init_ptr, ESP_HIMEM_BLKSZ));
      init_ptr = NULL;
      ESP_ERROR_CHECK(esp_himem_map(mh, rh, NEXTADDRESS, 0, ESP_HIMEM_BLKSZ, 0, &init_ptr)); // round to ESP_HIMEM_BLKSZ
      ptr = init_ptr;

      memcpy(&ptr[NEXTOFFSET], &DATABUF[STARTOFFSET],  NEXTSIZE );
      if (init_ptr) ESP_ERROR_CHECK(esp_himem_unmap(rh, init_ptr, ESP_HIMEM_BLKSZ));
    }
  }
#endif
}

void himem_read(uint32_t ADDRESS, uint32_t SIZE, uint8_t* DATABUF) {
#if HIMEM
  uint8_t *ptr = NULL;

  vPortYield(); // BT need this!!!
  vTaskDelay(1);

  if (SIZE <= ESP_HIMEM_BLKSZ) {

    #if DEBUG>4
      printf("READ:     ADDRESS: %d --------\n", ADDRESS);
    #endif
    int32_t STARTADDRESS = HIMEMOFFSET +  ADDRESS - (ADDRESS % ESP_HIMEM_BLKSZ)  - ESP_HIMEM_BLKSZ ;
    int32_t STARTOFFSET = ADDRESS % ESP_HIMEM_BLKSZ ;
    int32_t STARTSIZE = (SIZE % ESP_HIMEM_BLKSZ);    // ESP_HIMEM_BLKSZ - (ADDRESS%ESP_HIMEM_BLKSZ) ;
    if (SIZE == ESP_HIMEM_BLKSZ) STARTSIZE += ESP_HIMEM_BLKSZ;

    int32_t NEXTADDRESS = HIMEMOFFSET + ADDRESS - (ADDRESS % ESP_HIMEM_BLKSZ)  ;
    int32_t NEXTOFFSET = 0;
    int32_t NEXTSIZE = SIZE - STARTSIZE;

    if (STARTSIZE > 0) {
      #if DEBUG>4
        printf("     STARTADDRESS: %d    STARTOFFSET: %d    STARTSIZE: %d    \n", STARTADDRESS, STARTOFFSET, STARTSIZE);
      #endif
      if (init_ptr) ESP_ERROR_CHECK(esp_himem_unmap(rh, init_ptr, ESP_HIMEM_BLKSZ));
      init_ptr = NULL;
      ESP_ERROR_CHECK(esp_himem_map(mh, rh, STARTADDRESS, 0, ESP_HIMEM_BLKSZ, 0, &init_ptr)); // round to ESP_HIMEM_BLKSZ
      ptr = init_ptr;

      memcpy( &DATABUF[0], &ptr[STARTOFFSET], STARTSIZE );
      if (init_ptr) ESP_ERROR_CHECK(esp_himem_unmap(rh, init_ptr, ESP_HIMEM_BLKSZ));
    }

    if (NEXTSIZE > 0) {
      #if DEBUG>4  
        printf("     NEXTADDRESS: %d    NEXTOFFSET: %d    NEXTSIZE: %d    \n", NEXTADDRESS, NEXTOFFSET, NEXTSIZE);
      #endif
      if (init_ptr) ESP_ERROR_CHECK(esp_himem_unmap(rh, init_ptr, ESP_HIMEM_BLKSZ));
      init_ptr = NULL;
      ESP_ERROR_CHECK(esp_himem_map(mh, rh, NEXTADDRESS, 0, ESP_HIMEM_BLKSZ, 0, &init_ptr)); // round to ESP_HIMEM_BLKSZ
      ptr = init_ptr;

      memcpy( &DATABUF[STARTOFFSET], &ptr[NEXTOFFSET], NEXTSIZE );
      if (init_ptr) ESP_ERROR_CHECK(esp_himem_unmap(rh, init_ptr, ESP_HIMEM_BLKSZ));
    }
  }
#endif
}
//--------------------------------------------------------------------------------


















//--------------------------------------------------------------------------------
#define BITMAPMAGICIDENTIFIER 0x4D42

typedef struct {
   uint16_t type;                  // Magic identifier 0x4D42     
   uint32_t size;                  // File size in bytes          
   uint16_t reserved1;             // Reversed stuff              
   uint16_t reserved2;             // Reversed stuff              
   uint32_t offset;                // Offset to image data, bytes
} BMPHEADER;

typedef struct {
   uint32_t size;                  // Header size in bytes      
   int32_t width,height;           // Width and height of image 
   uint16_t planes;                // Number of colour planes   
   uint16_t bits;                  // Bits per pixel            
   uint32_t compression;           // Compression type          
   uint32_t imagesize;             // Image size in bytes       
   int32_t xres,yres;              // Pixels per meter          
   uint32_t ncolours;              // Number of colours         
   uint32_t importantcolours;      // Important colours         
} BMPINFOHEADER;
//--------------------------------------------------------------------------------
  BMPHEADER* bmpheader;
  BMPINFOHEADER* infoheader;
//--------------------------------------------------------------------------------
uint8_t* decode_1bit_bmp(uint8_t* DATAIN, int *_w, int *_h, char *_order, char *_invert) {
  unsigned char *img; 
  uint8_t* bmp_data;
  
  int i, j, rev_j, pos, ipos, tmp_w;

  // Read header information
  ///fread(&header, sizeof(BMPHEADER), 1, fp);
  bmpheader=(BMPHEADER*)malloc(sizeof(BMPHEADER));
  infoheader=(BMPINFOHEADER*)malloc(sizeof(BMPINFOHEADER));

  
  memcpy(&bmpheader->type, &DATAIN[0], 2);
  memcpy(&bmpheader->size, &DATAIN[2], 4);
  memcpy(&bmpheader->offset, &DATAIN[10], 4);

  printf("bmpheader->offset: %u\n",bmpheader->offset);
  // Check that this is BMP.
  if (bmpheader->type != BITMAPMAGICIDENTIFIER) {
    printf("File isn't bitmap image. Only BMP files are supported.\n");
    return(NULL);
  }

  memcpy(infoheader, &DATAIN[14], sizeof(BMPINFOHEADER));

  if (infoheader->bits != 1) {
    printf("Only 1-bit monochrome bitmap files are supported.\n");
    return(NULL);
  }

  printf("bits: %d, width: %d, height: %d\n",infoheader->bits,infoheader->width,infoheader->height);


  // In 1-bit image rows have 4 bytes border.
  // 2-bit images and more have colour in every byte.
  int lineSize = (infoheader->width / 32) * 4;
  if (infoheader->width % 32) {
    lineSize += 4;
  }

  // Is this wrong?
  //int lineSize = (infoheader->width / 8 + (infoheader->width / 8) % 4);
  int fileSize = lineSize * infoheader->height;
  img = (unsigned char *)ps_malloc(infoheader->width * infoheader->height);

  printf("bmpheader->offset: %d\n",bmpheader->offset);
  
  bmp_data=&DATAIN[bmpheader->offset];

  // Here you can specity byte order.
  ipos = 0;
  if (infoheader->width < 8) {
    tmp_w = 8;
  } else {
    tmp_w = infoheader->width;
  }
  for (j = 0, rev_j = infoheader->height-1;j < infoheader->height; j++, rev_j--) {
    for (i = tmp_w/8-1; i >= 0; i--) {

      // Revere byte order or not
      if (_order == NULL) {
        pos = rev_j * lineSize + i;
      } else {
        pos = j * lineSize + i;
      }

      // Invert image or not
      if (_invert == NULL) {
        img[ipos] = bmp_data[pos];
      } else {
        img[ipos] = 0xFF - bmp_data[pos];
      }

      ipos += 1;
    }
  }

  
  
  *_w = tmp_w;
  *_h = infoheader->height;

  free(infoheader);
  free(bmpheader);
  
  return img;

  
}


//--------------------------------------------------------------------------------
// ESP32's Xtal frequency 
const double fXtal = 40000000.0;

const enum {
  REV0,
  REV1
} rev = REV0;  //REV1 default

int bestSdm0 = 0, bestSdm1 = 0, bestSdm2 = 0, bestOdiv = 0;
//--------------------------------------------------------------------------------
double APLL_calc(double fTarget, uint8_t SET_APLL, uint8_t precise_LEVEL, uint8_t* bestclkDiv) { //mode=0 basic mode=1 precise 
  double fOut, temp;
  double bestDelta = 100000000.0;    
  int sdm0 = 0, sdm1 = 0, sdm2 = 0, odiv = 0;
  uint8_t clkDiv=0;
  uint8_t LOADING_CHAR=0;

  bestSdm0 = 0;
  bestSdm1 = 0;
  bestSdm2 = 0;
  bestOdiv = 0;

  for (sdm0 = 0; sdm0 <= 255; sdm0++) {  
    for (sdm1 = 0; sdm1 <= 255; sdm1++) {
//................................................................................
      if (INPUTREAD()) {
        if ((INPUT_START && INPUT_SELECT) || (INPUT_SYSTEM)) {
          ///POWER = 0; //exit SIGNALGEN
          return 0;
        }
      }
      if (LOADING_CHAR==0) draw_string_xy_static(0,0, (char*)"/", font_Ishmeria8x16, 0xffff, 0);
      if (LOADING_CHAR==1) draw_string_xy_static(0,0, (char*)"-", font_Ishmeria8x16, 0xffff, 0);
      if (LOADING_CHAR==2) draw_string_xy_static(0,0, (char*)"\\", font_Ishmeria8x16, 0xffff, 0);
      if (LOADING_CHAR==3) draw_string_xy_static(0,0, (char*)"|", font_Ishmeria8x16, 0xffff, 0);
      LOADING_CHAR++;
      if (LOADING_CHAR==4) LOADING_CHAR=0;
//................................................................................

      for (sdm2 = 0; sdm2 <= 63; sdm2++) {
        for (clkDiv=0;clkDiv<7;clkDiv++) {
          // Checking for frequency range of the numerator 
          temp = fXtal * ((double)sdm2 + (double)sdm1/256.0 + (double)sdm0/65536.0 + 4);
          if (temp > 500000000.0 || temp < 350000000.0) {
            continue;
          }
          for (odiv = 0; odiv <= 31; odiv++) {
            fOut = temp/(double)(2*(odiv+2));
            fOut=fOut/4; // NCX: correction

            if (clkDiv==1) fOut=fOut/2.0;
            else if (clkDiv==2) fOut=fOut/4.0;
            else if (clkDiv==3) fOut=fOut/8.0;
            else if (clkDiv==4) fOut=fOut/16.0;
            else if (clkDiv==5) fOut=fOut/32.0;
            else if (clkDiv==6) fOut=fOut/64.0;

            if (fabs(fOut-fTarget) < bestDelta) {
              bestDelta = fabs(fOut-fTarget);
              bestSdm0 = sdm0;
              bestSdm1 = sdm1;
              bestSdm2 = sdm2;
              bestOdiv = odiv;

              printf("CALC: sdm0=%d, sdm1=%d, sdm2=%d, odiv=%d\n",bestSdm0, bestSdm1, bestSdm2, bestOdiv);  
              printf("bestDelta: %lf\n",bestDelta);

              if (clkDiv==0) *bestclkDiv=1;
              else if (clkDiv==1) *bestclkDiv=2;
              else if (clkDiv==2) *bestclkDiv=4;
              else if (clkDiv==3) *bestclkDiv=8;
              else if (clkDiv==4) *bestclkDiv=16;
              else if (clkDiv==5) *bestclkDiv=32;
              else if (clkDiv==6) *bestclkDiv=64;
            }
            if (precise_LEVEL==4 && bestDelta==0.000) break; //done here
            else if (precise_LEVEL==3 && bestDelta<10.000) break; //done here
            else if (precise_LEVEL==2 && bestDelta<100.000) break; //done here
            else if (precise_LEVEL==1 && bestDelta<1000.000) break; //done here
            else if (precise_LEVEL==0 && bestDelta<10000.000) break; //done here
          }
          if (precise_LEVEL==4 && bestDelta==0.000) break; //done here
          else if (precise_LEVEL==3 && bestDelta<10.000) break; //done here
          else if (precise_LEVEL==2 && bestDelta<100.000) break; //done here
          else if (precise_LEVEL==1 && bestDelta<1000.000) break; //done here
          else if (precise_LEVEL==0 && bestDelta<10000.000) break; //done here
        }
        if (precise_LEVEL==4 && bestDelta==0.000) break; //done here
        else if (precise_LEVEL==3 && bestDelta<10.000) break; //done here
        else if (precise_LEVEL==2 && bestDelta<100.000) break; //done here
        else if (precise_LEVEL==1 && bestDelta<1000.000) break; //done here
        else if (precise_LEVEL==0 && bestDelta<10000.000) break; //done here
      }
      if (precise_LEVEL==4 && bestDelta==0.000) break; //done here
      else if (precise_LEVEL==3 && bestDelta<10.000) break; //done here
      else if (precise_LEVEL==2 && bestDelta<100.000) break; //done here
      else if (precise_LEVEL==1 && bestDelta<1000.000) break; //done here
      else if (precise_LEVEL==0 && bestDelta<10000.000) break; //done here
    }
    if (precise_LEVEL==4 && bestDelta==0.000) break; //done here
    else if (precise_LEVEL==3 && bestDelta<10.000) break; //done here
    else if (precise_LEVEL==2 && bestDelta<100.000) break; //done here
    else if (precise_LEVEL==1 && bestDelta<1000.000) break; //done here
    else if (precise_LEVEL==0 && bestDelta<10000.000) break; //done here
  }
  fOut = fXtal * (bestSdm2 + bestSdm1/256.0 + bestSdm0/65536.0 + 4) / (double)(2*(bestOdiv+2));
  fOut/=4;

  if (*bestclkDiv==2) fOut=fOut/2;
  else if (*bestclkDiv==4) fOut=fOut/4;
  else if (*bestclkDiv==8) fOut=fOut/8;
  else if (*bestclkDiv==16) fOut=fOut/16;
  else if (*bestclkDiv==32) fOut=fOut/32;
  else if (*bestclkDiv==64) fOut=fOut/64;
  
  printf("f=%.01f Hz, ∆f=%.01f Hz (relative: %.04f%%)\n", fOut, fabs(fOut-fTarget), 100*fabs(fOut-fTarget)/fTarget);
  printf("sdm0=%d, sdm1=%d, sdm2=%d, odiv=%d   [clkDIV=%d]\n", bestSdm0, bestSdm1, bestSdm2, bestOdiv, *bestclkDiv);

  return fOut;
}













//********************************************************************************
#ifdef NCAT_NO_PSRAM
//================================================================================
// LOAD DATA TO FLASH (FALLBACK SOLUTION FOR NO PSRAM DEVICES)
//================================================================================
char loadmessage[64];
uint8_t* FLASH_load_sdcard(char * filename, uint32_t* len) {
  fp = SD.open(filename);
  if (DEBUG) Serial.print("FILE SIZE: ");
  if (DEBUG) Serial.println(fp.size());
  FILE_ROM_SIZE = fp.size();
  *len = FILE_ROM_SIZE;

  if (FILE_ROM_SIZE>MAX_FLASH_FILESIZE) {
    printf("Selected file size is out of range: %d Bytes");
    return NULL;
  }
  uint16_t BLOCKCOUNT = (FILE_ROM_SIZE + SPI_FLASH_SECTOR_SIZE) / SPI_FLASH_SECTOR_SIZE;
  uint16_t BLOCKSIZEPX = LCD_WIDTH / BLOCKCOUNT;
  Serial.print("BLOCKCOUNT: ");
  Serial.print(BLOCKCOUNT);
  Serial.print(" BLOCKSIZEPX: ");
  Serial.print(BLOCKSIZEPX);
  Serial.println();
  BLOCKSIZEPX++; 
//********************************************************************************
  // Read NES rom to SPI FLASH!
  uint32_t i = 0;
  for (i = 0; i < fp.size() + SPI_FLASH_SECTOR_SIZE; i++) {
    if (DEBUG>3 && i % SPI_FLASH_SECTOR_SIZE == 0) {
      Serial.print("ROM loaded 4kB:");
      Serial.println(i / 0x1000);
    }
    if (i > 0 && i % SPI_FLASH_SECTOR_SIZE == 0) {
      //spi_flash_erase_sector(SPI_FLASH_ADDRESS/SPI_FLASH_SECTOR_SIZE + (i/SPI_FLASH_SECTOR_SIZE)-SPI_FLASH_SECTOR_SIZE);

      delayMicroseconds(300);
      spi_flash_erase_range(SPI_FLASH_ADDRESS + i - SPI_FLASH_SECTOR_SIZE, sizeof(flashdata));
      delayMicroseconds(300);
      spi_flash_write(SPI_FLASH_ADDRESS + i - SPI_FLASH_SECTOR_SIZE, flashdata, sizeof(flashdata));
      delayMicroseconds(300);

      sprintf(loadmessage, " %d / %d", i, FILE_ROM_SIZE);
      set_font_XY(8, 8);
      draw_string("Loaded:");
      draw_string(loadmessage);
      screenmemory_drawfillrectangle(((i / SPI_FLASH_SECTOR_SIZE) - 1)* BLOCKSIZEPX, 24, BLOCKSIZEPX, 16, 0xff);
    }
    delayMicroseconds(50);
    if (fp.available()) flashdata[i % SPI_FLASH_SECTOR_SIZE] = fp.read();
    delayMicroseconds(50);
  }
  fp.close();

  FLASH_ROM_SIZE = i; //Size of File and Offset Align

  if (DEBUG) Serial.print("FLASH SIZE: ");
  if (DEBUG) Serial.println(FLASH_ROM_SIZE);

  ROM = 0;
  //if (handle1) spi_flash_munmap(handle1);

  printf("Mapping %x (+%x)\n", SPI_FLASH_ADDRESS, FLASH_ROM_SIZE);
  ESP_ERROR_CHECK( spi_flash_mmap(SPI_FLASH_ADDRESS, FLASH_ROM_SIZE, SPI_FLASH_MMAP_DATA, &ROM, &handle1) );
  printf("mmap_res: handle=%d ptr=%p\n", handle1, ROM);
  Serial.println("[ROM MAPPED ON FLASH!]");
  return (unsigned char *)ROM;    
}
#endif
//================================================================================
