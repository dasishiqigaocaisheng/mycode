#ifndef _CHARLIB_H_
#define _CHARLIB_H_

#include "BMP.h"

#define VACANCY         0

#define BASIC_WIDTH		16
#define BASIC_HEIGHT	30

typedef struct CharMoldClass
{
	uint16_t Width;
	uint16_t Height;
	const uint16_t* Blocks;
} charmold;

extern const charmold BasicChar_Courier_New[];
extern const charmold Courier_New_7x15[];
extern const charmold SongTi_9x21[];
#endif
