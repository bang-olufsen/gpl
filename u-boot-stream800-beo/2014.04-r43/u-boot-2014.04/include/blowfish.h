#ifndef _BLOWFISH_H_
#define _BLOWFISH_H_

#define MAXKEYBYTES 56		/* 448 bits */

unsigned long F(unsigned long x);
void Blowfish_encipher(unsigned long *xl, unsigned long *xr);
void Blowfish_decipher(unsigned long *xl, unsigned long *xr);
void InitializeBlowfish(char key[], short keybytes);

#endif	/* _BLOWFISH_H_ */
