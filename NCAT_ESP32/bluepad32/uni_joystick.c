
/****************************************************************************
http://retro.moe/unijoysticle2

Copyright 2019 Ricardo Quesada

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
****************************************************************************/

#include "uni_joystick.h"

#include "uni_gamepad.h"

static void to_single_joy(const uni_gamepad_t* gp, uni_joystick_t* out_joy) {
    // Button A is "fire"
    if (gp->updated_states & GAMEPAD_STATE_BUTTON_A) {
        out_joy->fire |= ((gp->buttons & BUTTON_A_) != 0);
    }
    // Thumb left is "fire"
    if (gp->updated_states & GAMEPAD_STATE_BUTTON_THUMB_L) {
        out_joy->fire |= ((gp->buttons & BUTTON_THUMB_L) != 0);
    }

    // Shoulder right is "auto fire"
    if (gp->updated_states & GAMEPAD_STATE_BUTTON_SHOULDER_R) {
        out_joy->auto_fire |= ((gp->buttons & BUTTON_SHOULDER_R) != 0);
    }

    // Dpad
    if (gp->updated_states & GAMEPAD_STATE_DPAD) {
        if (gp->dpad & DPAD_UP)
            out_joy->up |= 1;
        if (gp->dpad & DPAD_DOWN)
            out_joy->down |= 1;
        if (gp->dpad & DPAD_RIGHT)
            out_joy->right |= 1;
        if (gp->dpad & DPAD_LEFT)
            out_joy->left |= 1;
    }

    // Axis: X and Y
    if (gp->updated_states & GAMEPAD_STATE_AXIS_X) {
        out_joy->left |= (gp->axis_x < -AXIS_THRESHOLD);
        out_joy->right |= (gp->axis_x > AXIS_THRESHOLD);
    }
    if (gp->updated_states & GAMEPAD_STATE_AXIS_Y) {
        out_joy->up |= (gp->axis_y < -AXIS_THRESHOLD);
        out_joy->down |= (gp->axis_y > AXIS_THRESHOLD);
    }

    // Pots
    if (gp->updated_states & GAMEPAD_STATE_BRAKE) {
        out_joy->pot_x = (gp->brake >> 2);  // convert from 1024 to 256
    }
    if (gp->updated_states & GAMEPAD_STATE_THROTTLE) {
        out_joy->pot_y = (gp->throttle >> 2);  // convert from 1024 to 256
    }
}

// Basic Mode: One gamepad controls one joystick
void uni_joy_to_single_joy_from_gamepad(const uni_gamepad_t* gp, uni_joystick_t* out_joy) {
    to_single_joy(gp, out_joy);

    // Buttom B is "jump". Good for C64 games
    if (gp->updated_states & GAMEPAD_STATE_BUTTON_B) {
        out_joy->up |= ((gp->buttons & BUTTON_B_) != 0);
    }

    // 2nd Button for Atari ST / Amiga
    if (gp->updated_states & GAMEPAD_STATE_BUTTON_X) {
        out_joy->pot_y |= ((gp->buttons & BUTTON_X) != 0);
    }

    // 3rd Button for Amiga
    if (gp->updated_states & GAMEPAD_STATE_BUTTON_Y) {
        out_joy->pot_x |= ((gp->buttons & BUTTON_Y) != 0);
    }
}

// Enhanced mode: One gamepad controls two joysticks
void uni_joy_to_combo_joy_joy_from_gamepad(const uni_gamepad_t* gp,
                                           uni_joystick_t* out_joy1,
                                           uni_joystick_t* out_joy2) {
    to_single_joy(gp, out_joy2);

    // Buttom B is "fire"
    if (gp->updated_states & GAMEPAD_STATE_BUTTON_B) {
        out_joy1->fire |= ((gp->buttons & BUTTON_B_) != 0);
    }
    // Thumb right is "fire"
    if (gp->updated_states & GAMEPAD_STATE_BUTTON_THUMB_R) {
        out_joy1->fire |= ((gp->buttons & BUTTON_THUMB_R) != 0);
    }

    // Swap "auto fire" in Combo Joy Joy.
    // "left" belongs to joy1 while "right" to joy2.
    if (gp->updated_states & GAMEPAD_STATE_BUTTON_SHOULDER_L) {
        out_joy2->auto_fire = ((gp->buttons & BUTTON_SHOULDER_L) != 0);
    }
    if (gp->updated_states & GAMEPAD_STATE_BUTTON_SHOULDER_R) {
        out_joy1->auto_fire = ((gp->buttons & BUTTON_SHOULDER_R) != 0);
    }

    // Axis: RX and RY
    if (gp->updated_states & GAMEPAD_STATE_AXIS_RX) {
        out_joy1->left |= (gp->axis_rx < -AXIS_THRESHOLD);
        out_joy1->right |= (gp->axis_rx > AXIS_THRESHOLD);
    }
    if (gp->updated_states & GAMEPAD_STATE_AXIS_RY) {
        out_joy1->up |= (gp->axis_ry < -AXIS_THRESHOLD);
        out_joy1->down |= (gp->axis_ry > AXIS_THRESHOLD);
    }
}

// One gamepad controls one mouse: Unfinished
void uni_joy_to_single_mouse_from_gamepad(const uni_gamepad_t* gp, uni_joystick_t* out_mouse) {
    to_single_joy(gp, out_mouse);
}

// One gamepad controls one mouse and one joystick: Unfinished
void uni_joy_to_combo_joy_mouse_from_gamepad(const uni_gamepad_t* gp,
                                             uni_joystick_t* out_joy,
                                             uni_joystick_t* out_mouse) {
    to_single_joy(gp, out_joy);

    // Axis: RX and RY
    if (gp->updated_states & GAMEPAD_STATE_AXIS_RX) {
        out_mouse->left |= (gp->axis_rx < -AXIS_THRESHOLD);
        out_mouse->right |= (gp->axis_rx > AXIS_THRESHOLD);
    }
    if (gp->updated_states & GAMEPAD_STATE_AXIS_RY) {
        out_mouse->up |= (gp->axis_ry < -AXIS_THRESHOLD);
        out_mouse->down |= (gp->axis_ry > AXIS_THRESHOLD);
    }

    // Buttom B is "mouse left button"
    if (gp->updated_states & GAMEPAD_STATE_BUTTON_B) {
        out_mouse->fire |= ((gp->buttons & BUTTON_B_) != 0);
    }

    // Buttom X is "mouse middle button"
    if (gp->updated_states & GAMEPAD_STATE_BUTTON_X) {
        out_mouse->pot_x |= ((gp->buttons & BUTTON_X) != 0);
    }

    // Buttom Y is "mouse right button"
    if (gp->updated_states & GAMEPAD_STATE_BUTTON_Y) {
        out_mouse->pot_y |= ((gp->buttons & BUTTON_Y) != 0);
    }
}
