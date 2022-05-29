#ifndef EMU_H
#define EMU_H

#ifdef __cplusplus
extern "C" {
#endif

void emu_run();
void emu_reset();

void do_audio_frame_gbc();
void do_audio_soundmix_gbc();

#ifdef __cplusplus
};
#endif

#endif
