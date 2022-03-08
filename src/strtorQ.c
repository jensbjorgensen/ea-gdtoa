/****************************************************************

The author of this software is David M. Gay.

Copyright (C) 1998, 2000 by Lucent Technologies
All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the name of Lucent or any of its entities
not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior
permission.

LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.

****************************************************************/

/* Please send bug reports to David M. Gay (dmg at acm dot org,
 * with " at " changed at "@" and " dot " changed to ".").	*/

#include "gdtoaimp.h"

#undef _0
#undef _1

#ifdef IEEE_8087
#define _0 3
#define _1 2
#define _2 1
#define _3 0
#else
#error Something went wrong, IEEE8087 is not defined
#endif

void ULtoQ(ULong *L, const ULong *bits, Long exp, int k)
{
	switch(k & STRTOG_Retmask)
	{
		case STRTOG_NoNumber:
		case STRTOG_Zero:
			L[0] = L[1] = L[2] = L[3] = 0;
			break;

		case STRTOG_Normal:
		case STRTOG_NaNbits:
			L[_3] = bits[0];
			L[_2] = bits[1];
			L[_1] = bits[2];
			L[_0] = (bits[3] & (unsigned)~0x10000) | (unsigned)((exp + 0x3fff + 112) << 16);
			break;

		case STRTOG_Denormal:
			L[_3] = bits[0];
			L[_2] = bits[1];
			L[_1] = bits[2];
			L[_0] = bits[3];
			break;

		case STRTOG_Infinite:
			L[_0] = 0x7fff0000;
			L[_1] = L[_2] = L[_3] = 0;
			break;

		case STRTOG_NaN:
			L[0] = ld_QNAN0;
			L[1] = ld_QNAN1;
			L[2] = ld_QNAN2;
			L[3] = ld_QNAN3;
			break;

		default:
			break;
	}
	if(k & STRTOG_Neg)
	{
		{
			L[_0] |= 0x80000000L;
		}
	}
}

int strtorQ(CONST char *s, char **sp, int rounding, void *L)
{
	static FPI fpi0 = {113, 1 - 16383 - 113 + 1, 32766 - 16383 - 113 + 1, 1, SI};
	FPI *fpi;
	FPI fpi1;
	ULong bits[4];
	Long exp;
	int k;

	fpi = &fpi0;
	if(rounding != FPI_Round_near)
	{
		fpi1 = fpi0;
		fpi1.rounding = rounding;
		fpi = &fpi1;
	}
	k = strtodg(s, sp, fpi, &exp, bits);
	ULtoQ((ULong*)L, bits, exp, k);
	return k;
}
