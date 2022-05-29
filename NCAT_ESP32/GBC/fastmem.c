

#include "fastmem.h"

#include <esp_attr.h>

#define D_GBC 0 /* direct */
#define C_GBC 1 /* direct cgb-only */
#define R_GBC 2 /* io register */
#define S_GBC 3 /* sound register */
#define W_GBC 4 /* wave pattern */

#define F_GBC 0xFF /* fail */

//DRAM_ATTR const byte himask[256];

const byte hi_rmap[256] =
{
	0, 0, R_GBC, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC,
	S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, C_GBC, 0, C_GBC,
	0, C_GBC, C_GBC, C_GBC, C_GBC, C_GBC, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, C_GBC, C_GBC, C_GBC, C_GBC, 0, 0, 0, 0,
	C_GBC, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const byte hi_wmap[256] =
{
	R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC,
	S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC,
	S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC,
	S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC, S_GBC,
	R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, R_GBC, 0, R_GBC, 0, R_GBC,
	0, C_GBC, C_GBC, C_GBC, C_GBC, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, R_GBC, R_GBC, R_GBC, R_GBC, 0, 0, 0, 0,
	R_GBC, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, R_GBC
};


void sound_write();
static void no_write()
{
}
