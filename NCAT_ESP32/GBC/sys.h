#ifndef VID_H
#define VID_H

#include <sys/time.h>

/* stuff implemented by the different sys/ backends */
#ifdef __cplusplus
extern "C" {
#endif

///void vid_begin();
///void vid_end();
void vid_init();
void vid_preinit();
void vid_close();
void vid_setpal(int i, int r, int g, int b);
void vid_settitle(char *title);

void pcm_init();
////extern int pcm_submit();
void pcm_close();

void ev_poll();

void sys_checkdir(char *path, int wr);
void sys_sleep(int us);
void sys_sanitize(char *s);

void joy_init();
void joy_poll();
void joy_close();

void kb_init();
void kb_poll();
void kb_close();


/* FIXME these have different prototype for obsolete ( == M$ ) platforms */
int sys_elapsed_(uint32_t *prev);
void sys_initpath();
unsigned long sys_micros();

#ifdef __cplusplus
};
#endif
#endif
