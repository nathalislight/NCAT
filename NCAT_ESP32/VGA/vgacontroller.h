/*
  Created by Fabrizio Di Vittorio (fdivitto2013@gmail.com) - <http://www.fabgl.com>
  Copyright (c) 2019-2021 Fabrizio Di Vittorio.
  All rights reserved.


* Please contact fdivitto2013@gmail.com if you need a commercial license.


* This library and related software is available under GPL v3.

  FabGL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FabGL is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with FabGL.  If not, see <http://www.gnu.org/licenses/>.
 */


#pragma once



/**
 * @file
 *
 * @brief This file contains fabgl::VGAController definition.
 */


#include <stdint.h>
#include <stddef.h>
#include <atomic>

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "fabglconf.h"
///#include "fabutils.h"
#include "swgenerator.h"
///#include "displaycontroller.h"
#include "vgabasecontroller.h"




namespace fabgl {




/**
* @brief Represents the VGA bitmapped controller
*
* Use this class to set screen resolution and to associate VGA signals to ESP32 GPIO outputs.
*
* This example initializes VGA Controller with 64 colors at 640x350:
*
*     fabgl::VGAController VGAController;
*     // the default assigns GPIO22 and GPIO21 to Red, GPIO19 and GPIO18 to Green, GPIO5 and GPIO4 to Blue, GPIO23 to HSync and GPIO15 to VSync
*     VGAController.begin();
*     VGAController.setResolution(VGA_640x350_70Hz);
*
* This example initializes VGA Controller with 8 colors (5 GPIOs used) and 640x350 resolution:
*
*     // Assign GPIO22 to Red, GPIO21 to Green, GPIO19 to Blue, GPIO18 to HSync and GPIO5 to VSync
*     VGAController.begin(GPIO_NUM_22, GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5);
*
*     // Set 640x350@70Hz resolution
*     VGAController.setResolution(VGA_640x350_70Hz);
*
* This example initializes VGA Controller with 64 colors (8 GPIOs used) and 640x350 resolution:
*
*     // Assign GPIO22 and GPIO21 to Red, GPIO19 and GPIO18 to Green, GPIO5 and GPIO4 to Blue, GPIO23 to HSync and GPIO15 to VSync
*     VGAController.begin(GPIO_NUM_22, GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5, GPIO_NUM_4, GPIO_NUM_23, GPIO_NUM_15);
*
*     // Set 640x350@70Hz resolution
*     VGAController.setResolution(VGA_640x350_70Hz);
*/
class VGAController : public VGABaseController {

public:

  VGAController();

  // unwanted methods
  VGAController(VGAController const&)   = delete;
  void operator=(VGAController const&)  = delete;


  /**
   * @brief Returns the singleton instance of VGAController class
   *
   * @return A pointer to VGAController singleton object
   */
  static VGAController * instance() { return s_instance; }

  // abstract method of BitmappedDisplayController
  void suspendBackgroundPrimitiveExecution();

  // abstract method of BitmappedDisplayController
  void resumeBackgroundPrimitiveExecution();

   // import "modeline" version of setResolution
  using VGABaseController::setResolution;

  void setResolution(VGATimings const& timings, int viewPortWidth = -1, int viewPortHeight = -1, bool doubleBuffered = false);

  
  /**
   * @brief Sets a raw pixel prepared using VGAController.createRawPixel.
   *
   * A raw pixel (or raw color) is a byte (uint8_t) that contains color information and synchronization signals.
   *
   * @param x Horizontal pixel position
   * @param y Vertical pixel position
   * @param rgb Raw pixel color
   *
   * Example:
   *
   *     // Set color of pixel at 100, 100
   *     VGAController.setRawPixel(100, 100, VGAController.createRawPixel(RGB222(3, 0, 0));
   */
  void setRawPixel(int x, int y, uint8_t rgb)    { VGA_PIXEL(x, y) = rgb; }


///private:
public:

  void init();

  void allocateViewPort();
  void onSetupDMABuffer(lldesc_t volatile * buffer, bool isStartOfVertFrontPorch, int scan, bool isVisible, int visibleRow);

  static void VSyncInterrupt(void * arg);

  static VGAController * s_instance;

  volatile int16_t       m_maxVSyncISRTime; // Maximum us VSync interrupt routine can run

};



} // end of namespace







