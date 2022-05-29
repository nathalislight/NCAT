#ifndef __LOADER_H__
#define __LOADER_H__


typedef struct loader_s
{
	char *rom;
	char *base;
	char *sram;
	char *state;
	int ramloaded;
} loader_t;

#ifdef __cplusplus
extern "C" {
#endif

extern loader_t loader;


int rom_load(const byte * data, int len);
int sram_load();
int sram_save();

void loader_init(char *s);
void state_save(int n);
void state_load(int n);

void loader_unload();

#ifdef __cplusplus
};
#endif

#endif
