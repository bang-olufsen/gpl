#include <common.h>

#include "blowfish.h"

#define N		16
#define KEYBYTES	8

unsigned long P[N + 2];
unsigned long S[4][256];

#include "blowfish_dat.inc"

unsigned long F(unsigned long x)
{
	unsigned short a;
	unsigned short b;
	unsigned short c;
	unsigned short d;
	unsigned long  y;

	d = x & 0x00FF;
	x >>= 8;
	c = x & 0x00FF;
	x >>= 8;
	b = x & 0x00FF;
	x >>= 8;
	a = x & 0x00FF;
	//y = ((S[0][a] + S[1][b]) ^ S[2][c]) + S[3][d];
	y = S[0][a] + S[1][b];
	y = y ^ S[2][c];
	y = y + S[3][d];

	return y;
}

void Blowfish_encipher(unsigned long *xl, unsigned long *xr)
{
	unsigned long  Xl;
	unsigned long  Xr;
	unsigned long  temp;
	short          i;

	Xl = *xl;
	Xr = *xr;

	for (i = 0; i < N; ++i) {
		Xl = Xl ^ P[i];
		Xr = F(Xl) ^ Xr;

		temp = Xl;
		Xl = Xr;
		Xr = temp;
	}

	temp = Xl;
	Xl = Xr;
	Xr = temp;

	Xr = Xr ^ P[N];
	Xl = Xl ^ P[N + 1];

	*xl = Xl;
	*xr = Xr;
}

void Blowfish_decipher(unsigned long *xl, unsigned long *xr)
{
	unsigned long  Xl;
	unsigned long  Xr;
	unsigned long  temp;
	short          i;

	Xl = *xl;
	Xr = *xr;

	for (i = N + 1; i > 1; --i) {
		Xl = Xl ^ P[i];
		Xr = F(Xl) ^ Xr;

		/* Exchange Xl and Xr */
		temp = Xl;
		Xl = Xr;
		Xr = temp;
	}

	/* Exchange Xl and Xr */
	temp = Xl;
	Xl = Xr;
	Xr = temp;

	Xr = Xr ^ P[1];
	Xl = Xl ^ P[0];

	*xl = Xl;
	*xr = Xr;
}

void InitializeBlowfish(char key[], short keybytes)
{
	short          i;
	short          j;
	short          k;
	unsigned long  data;
	unsigned long  datal;
	unsigned long  datar;
	unsigned long  offset;

	/* Use the array initialization data (BLOWFISH_DAT[]) */
	for (i = 0, offset = 0; i < N + 2; ++i, offset+=4) {
		// BLOWFISH_DAT is in big-endian format
		P[i] = ((BLOWFISH_DAT[offset+3]<<0) |
				(BLOWFISH_DAT[offset+2]<<8) |
				(BLOWFISH_DAT[offset+1]<<16) |
				(BLOWFISH_DAT[offset+0]<<24));
	}

	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 256; ++j, offset+=4) {
			// BLOWFISH_DAT is in big-endian format
			S[i][j] = ((BLOWFISH_DAT[offset+3]<<0) |
					   (BLOWFISH_DAT[offset+2]<<8) |
					   (BLOWFISH_DAT[offset+1]<<16) |
					   (BLOWFISH_DAT[offset+0]<<24));
		}
	}


	j = 0;
	for (i = 0; i < N + 2; ++i) {
		data = 0x00000000;
		for (k = 0; k < 4; ++k) {
			data <<= 8;
			data |= (unsigned long)key[j] & 0xff;
			j = j + 1;

			if (j >= keybytes)
				j = 0;
		}
		P[i] = P[i] ^ data;
	}

	datal = 0x00000000;
	datar = 0x00000000;

	for (i = 0; i < N + 2; i += 2) {
		Blowfish_encipher(&datal, &datar);

		P[i] = datal;
		P[i + 1] = datar;
	}

	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 256; j += 2) {
			Blowfish_encipher(&datal, &datar);

			S[i][j] = datal;
			S[i][j + 1] = datar;
		}
	}
}
