/*	
 * DES algorithm implementation.
 *
 * Revised by Justin Xia @2021/10/17
 */

#ifndef _WIN32
#include <endian.h>
#endif
#include "des.h"
#include "memory.h"

#define	S	static
typedef unsigned char	uchar;
typedef unsigned int	uint;

S void	basedes(uchar *inpvect, uchar *outvect, uint way);
#if !defined(_WIN32) && __BYTE_ORDER == __BIG_ENDIAN
#define byteswap(cp)
#else
S void	byteswap(uchar *cp);
#endif
S void	calc_keys(uchar *ckey, uchar *dkey);
S void	expandl(uchar *vect, uchar *store, uchar *vectk);
S void	func_des(long *lenga, long *lungb, uchar *iterk);
S void	iter_des(int decr,long *lunga, long *lungb);
S void	makematr(uchar *vect, uchar *matr, uint n, uint bits);
S void	make_lshift(uchar *ckey, uchar *dkey, uint n, uint m);
S void	moltb(const uchar *base, const uchar *oper, uchar *result, uint n, uint m);
S void	oper_t1(uchar *matr1, uchar *matr2, uint n, uint m);
S void	oper_t1v(uchar *vect1, uchar *vect2, uint n);
S void	oper_t2v(uchar *vect1, uchar *vect2, uint n);
S void	perm_choi_1(uchar *totkey, uchar *ckey, uchar *dkey);
S void	perm_choi_2(uchar *ckey, uchar *dkey, uchar *iterk);

S uchar itermatr[8];
S uchar matrk[8 * 7],	/* [8][7] */
		appok1[7 * 8],	/* [7][8] */
		ckey[4 * 7],	/* [4][7] */
		dkey[4 * 7];	/* [4][7] */
S uchar iterkey[16][8];
S const uchar iperm[8] = { 0x40, 0x10, 0x04, 0x01, 0x80, 0x20, 0x08, 0x02 };
S const uchar ipermt[8] = { 0x08, 0x80, 0x04, 0x40, 0x02, 0x20, 0x01, 0x10 };
S const uchar kperm2[8][6] = {
	{ 14, 17, 11, 24,  1,  5},
	{  3, 28, 15,  6, 21, 10},
	{ 23, 19, 12,  4, 26,  8},
	{ 16,  7, 27, 20, 13,  2},
	{ 41, 52, 31, 37, 47, 55},
	{ 30, 40, 51, 45, 33, 48},
	{ 44, 49, 39, 56, 34, 53},
	{ 46, 42, 50, 36, 29, 32}};

#ifdef	USE_TRASF_INIT
S void	trasf_init(void);
S int	desIsInited = 0;	/* flag di DES gia' inizializzato (vedi trasf_init()) */
S long	trasfp[8][64];
S const uchar selfun[8][64] = {
	{
		14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
		 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
		 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
		15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
	},
	{
		15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
		 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
		 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
		13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
	},
	{
		10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
		13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
		13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
		 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
	},
	{
		 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
		13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
		10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
		 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
	},
	{
		 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
		14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
		 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
		11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
	},
	{
		12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
		10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
		 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
		 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
	},
	{
		 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
		13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
		 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
		 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
	},
	{
		13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
		 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
		 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
		 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
	}
};
#else //USE_TRASF_INIT
S const unsigned int trasfp[8][64] = {
	{  0x00808200,  0x00000000,  0x00008000,  0x00808202,  0x00808002,  0x00008202,  0x00000002,  0x00008000,  0x00000200,  0x00808200,  0x00808202,  0x00000200,  0x00800202,  0x00808002,  0x00800000,  0x00000002,  0x00000202,  0x00800200,  0x00800200,  0x00008200,  0x00008200,  0x00808000,  0x00808000,  0x00800202,  0x00008002,  0x00800002,  0x00800002,  0x00008002,  0x00000000,  0x00000202,  0x00008202,  0x00800000,  0x00008000,  0x00808202,  0x00000002,  0x00808000,  0x00808200,  0x00800000,  0x00800000,  0x00000200,  0x00808002,  0x00008000,  0x00008200,  0x00800002,  0x00000200,  0x00000002,  0x00800202,  0x00008202,  0x00808202,  0x00008002,  0x00808000,  0x00800202,  0x00800002,  0x00000202,  0x00008202,  0x00808200,  0x00000202,  0x00800200,  0x00800200,  0x00000000,  0x00008002,  0x00008200,  0x00000000,  0x00808002  },
	{  0x40084010,  0x40004000,  0x00004000,  0x00084010,  0x00080000,  0x00000010,  0x40080010,  0x40004010,  0x40000010,  0x40084010,  0x40084000,  0x40000000,  0x40004000,  0x00080000,  0x00000010,  0x40080010,  0x00084000,  0x00080010,  0x40004010,  0x00000000,  0x40000000,  0x00004000,  0x00084010,  0x40080000,  0x00080010,  0x40000010,  0x00000000,  0x00084000,  0x00004010,  0x40084000,  0x40080000,  0x00004010,  0x00000000,  0x00084010,  0x40080010,  0x00080000,  0x40004010,  0x40080000,  0x40084000,  0x00004000,  0x40080000,  0x40004000,  0x00000010,  0x40084010,  0x00084010,  0x00000010,  0x00004000,  0x40000000,  0x00004010,  0x40084000,  0x00080000,  0x40000010,  0x00080010,  0x40004010,  0x40000010,  0x00080010,  0x00084000,  0x00000000,  0x40004000,  0x00004010,  0x40000000,  0x40080010,  0x40084010,  0x00084000  },
	{  0x00000104,  0x04010100,  0x00000000,  0x04010004,  0x04000100,  0x00000000,  0x00010104,  0x04000100,  0x00010004,  0x04000004,  0x04000004,  0x00010000,  0x04010104,  0x00010004,  0x04010000,  0x00000104,  0x04000000,  0x00000004,  0x04010100,  0x00000100,  0x00010100,  0x04010000,  0x04010004,  0x00010104,  0x04000104,  0x00010100,  0x00010000,  0x04000104,  0x00000004,  0x04010104,  0x00000100,  0x04000000,  0x04010100,  0x04000000,  0x00010004,  0x00000104,  0x00010000,  0x04010100,  0x04000100,  0x00000000,  0x00000100,  0x00010004,  0x04010104,  0x04000100,  0x04000004,  0x00000100,  0x00000000,  0x04010004,  0x04000104,  0x00010000,  0x04000000,  0x04010104,  0x00000004,  0x00010104,  0x00010100,  0x04000004,  0x04010000,  0x04000104,  0x00000104,  0x04010000,  0x00010104,  0x00000004,  0x04010004,  0x00010100  },
	{  0x80401000,  0x80001040,  0x80001040,  0x00000040,  0x00401040,  0x80400040,  0x80400000,  0x80001000,  0x00000000,  0x00401000,  0x00401000,  0x80401040,  0x80000040,  0x00000000,  0x00400040,  0x80400000,  0x80000000,  0x00001000,  0x00400000,  0x80401000,  0x00000040,  0x00400000,  0x80001000,  0x00001040,  0x80400040,  0x80000000,  0x00001040,  0x00400040,  0x00001000,  0x00401040,  0x80401040,  0x80000040,  0x00400040,  0x80400000,  0x00401000,  0x80401040,  0x80000040,  0x00000000,  0x00000000,  0x00401000,  0x00001040,  0x00400040,  0x80400040,  0x80000000,  0x80401000,  0x80001040,  0x80001040,  0x00000040,  0x80401040,  0x80000040,  0x80000000,  0x00001000,  0x80400000,  0x80001000,  0x00401040,  0x80400040,  0x80001000,  0x00001040,  0x00400000,  0x80401000,  0x00000040,  0x00400000,  0x00001000,  0x00401040  },
	{  0x00000080,  0x01040080,  0x01040000,  0x21000080,  0x00040000,  0x00000080,  0x20000000,  0x01040000,  0x20040080,  0x00040000,  0x01000080,  0x20040080,  0x21000080,  0x21040000,  0x00040080,  0x20000000,  0x01000000,  0x20040000,  0x20040000,  0x00000000,  0x20000080,  0x21040080,  0x21040080,  0x01000080,  0x21040000,  0x20000080,  0x00000000,  0x21000000,  0x01040080,  0x01000000,  0x21000000,  0x00040080,  0x00040000,  0x21000080,  0x00000080,  0x01000000,  0x20000000,  0x01040000,  0x21000080,  0x20040080,  0x01000080,  0x20000000,  0x21040000,  0x01040080,  0x20040080,  0x00000080,  0x01000000,  0x21040000,  0x21040080,  0x00040080,  0x21000000,  0x21040080,  0x01040000,  0x00000000,  0x20040000,  0x21000000,  0x00040080,  0x01000080,  0x20000080,  0x00040000,  0x00000000,  0x20040000,  0x01040080,  0x20000080  },
	{  0x10000008,  0x10200000,  0x00002000,  0x10202008,  0x10200000,  0x00000008,  0x10202008,  0x00200000,  0x10002000,  0x00202008,  0x00200000,  0x10000008,  0x00200008,  0x10002000,  0x10000000,  0x00002008,  0x00000000,  0x00200008,  0x10002008,  0x00002000,  0x00202000,  0x10002008,  0x00000008,  0x10200008,  0x10200008,  0x00000000,  0x00202008,  0x10202000,  0x00002008,  0x00202000,  0x10202000,  0x10000000,  0x10002000,  0x00000008,  0x10200008,  0x00202000,  0x10202008,  0x00200000,  0x00002008,  0x10000008,  0x00200000,  0x10002000,  0x10000000,  0x00002008,  0x10000008,  0x10202008,  0x00202000,  0x10200000,  0x00202008,  0x10202000,  0x00000000,  0x10200008,  0x00000008,  0x00002000,  0x10200000,  0x00202008,  0x00002000,  0x00200008,  0x10002008,  0x00000000,  0x10202000,  0x10000000,  0x00200008,  0x10002008  },
	{  0x00100000,  0x02100001,  0x02000401,  0x00000000,  0x00000400,  0x02000401,  0x00100401,  0x02100400,  0x02100401,  0x00100000,  0x00000000,  0x02000001,  0x00000001,  0x02000000,  0x02100001,  0x00000401,  0x02000400,  0x00100401,  0x00100001,  0x02000400,  0x02000001,  0x02100000,  0x02100400,  0x00100001,  0x02100000,  0x00000400,  0x00000401,  0x02100401,  0x00100400,  0x00000001,  0x02000000,  0x00100400,  0x02000000,  0x00100400,  0x00100000,  0x02000401,  0x02000401,  0x02100001,  0x02100001,  0x00000001,  0x00100001,  0x02000000,  0x02000400,  0x00100000,  0x02100400,  0x00000401,  0x00100401,  0x02100400,  0x00000401,  0x02000001,  0x02100401,  0x02100000,  0x00100400,  0x00000000,  0x00000001,  0x02100401,  0x00000000,  0x00100401,  0x02100000,  0x00000400,  0x02000001,  0x02000400,  0x00000400,  0x00100001  },
	{  0x08000820,  0x00000800,  0x00020000,  0x08020820,  0x08000000,  0x08000820,  0x00000020,  0x08000000,  0x00020020,  0x08020000,  0x08020820,  0x00020800,  0x08020800,  0x00020820,  0x00000800,  0x00000020,  0x08020000,  0x08000020,  0x08000800,  0x00000820,  0x00020800,  0x00020020,  0x08020020,  0x08020800,  0x00000820,  0x00000000,  0x00000000,  0x08020020,  0x08000020,  0x08000800,  0x00020820,  0x00020000,  0x00020820,  0x00020000,  0x08020800,  0x00000800,  0x00000020,  0x08020020,  0x00000800,  0x00020820,  0x08000800,  0x00000020,  0x08000020,  0x08020000,  0x08020020,  0x08000000,  0x00020000,  0x08000820,  0x00000000,  0x08020820,  0x00020020,  0x08000020,  0x08020000,  0x08000800,  0x08000820,  0x00000000,  0x08020820,  0x00020800,  0x00020800,  0x00000820,  0x00000820,  0x00020020,  0x08000000,  0x08020800  },
};
#endif//USE_TRASF_INIT


S const uchar perm_fin[32] = {
		16,  7, 20, 21,
		29, 12, 28, 17,
		 1, 15, 23, 26,
		 5, 18, 31, 10,
		 2,  8, 24, 14,
		32, 27,  3,  9,
		19, 13, 30,  6,
		22, 11,  4, 25 };


/**************************************************************************
		DES
ROUTINE BASE PER LA CRITTOGRAFAZIONE CON L'ALGORITMO DES
puchCodeIn = vettore input;
puchCodeOut = vettore output;
puchKey = chiave di crittografazione;
**************************************************************************/
void DES (uchar *puchCodeIn, uchar *puchCodeOut, uchar *puchKey)
{
#ifdef	USE_TRASF_INIT
	trasf_init();							/* inizializza "matrice trasformazioni */
#endif//USE_TRASF_INIT
	makematr(puchKey,matrk,8,7);    		/* forma matrice key */
	perm_choi_1(matrk, ckey, dkey);			/* calcola ckey e dkey del passo 0 */
	calc_keys(ckey, dkey);					/* calcola le "chiavi" dei 16 passi */
	basedes(puchCodeIn,puchCodeOut,0);
}

/**************************************************************************
		UNDES
ROUTINE BASE PER LA DECRITTOGRAFAZIONE CON L'ALGORITMO DES
puchCodeIn = vettore input;
puchCodeOut = vettore output;
puchKey = chiave di decrittografazione;
**************************************************************************/
void UNDES (uchar *puchCodeIn, uchar *puchCodeOut, uchar *puchKey)
{
#ifdef	USE_TRASF_INIT
	trasf_init();							/* inizializza "matrice trasformazioni */
#endif//USE_TRASF_INIT
	makematr(puchKey,matrk,8,7);    		/* forma matrice key */
	perm_choi_1(matrk, ckey, dkey);			/* calcola ckey e dkey del passo 0 */
	calc_keys(ckey, dkey);					/* calcola le "chiavi" dei 16 passi */
	basedes(puchCodeIn,puchCodeOut,1);
}


/**************************************************************************
ROUTINE BASE PER LA CRITTOGRAFAZIONE/DECRITTOGRAFAZIONE CON L'ALGORITMO DES
inpvect = vettore input;
outvect = vettore output;
way = 0 -> modo encrypt, = 1 -> modo decrypt;
**************************************************************************/

S void basedes(uchar *inpvect, uchar *outvect, uint way)
{
 uchar	done1[8],
		done2[8],
		appo[8];

 moltb(inpvect, iperm, appo, 8, 8);		/* effettua AA = IP(A) perm. ini. */
 oper_t1v(appo, done1, 8);				/* applica l'operatore T1 */
 iter_des(way, (long *)&done1[0], (long *)&done1[4]); /* esegue le iterazioni base del DES */
 oper_t2v(done1, done2, 8);				/* effettua ZZ = IP^(B) perm. fin. */
 moltb(done2, ipermt, outvect, 8, 8);	/* crimes = stringa crittografata */
}

/*****************************************************************
ROUTINE CHE MOLTIPLICA 2 VETTORI DI CUI UNO E' UN VETTORE DI PERMUTAZIONE
base = puntatore al vettore iniziale
oper = puntatore al vettore di permutazione
result = puntatore al vettore risultato
n = numero elementi vettore iniziale
m = numero elementi vettore di permutazione
*****************************************************************/

S void moltb(const uchar *base, const uchar *oper, uchar *result, uint n, uint m)
{
 uint	i,
		j;
 uchar	mask,
		b,
		r;

 for (i = 0; i < n; i++)
	{
	b = *base++;
	for (j = r = 0, mask = 0x80; j < m; j++, mask >>= 1)
		if (b & oper[j])
			r |= mask;					/* moltiplica come se riga x colonna */
	*result++ = r;
	}
}

/*****************************************************************
ROUTINE CHE APPLICA L' OPERATORE T1 AD UNA MATRICE
T1 E' TALE CHE Y[i][j] = X[(n-1)-j][i]
matr1 = puntatore alla matrice da trasporre
matr2 = puntatore alla matrice trasposta
n = numero righe matrice da trasporre
m = numero colonne matrice da trasporre
*****************************************************************/

S void oper_t1(uchar *matr1, uchar *matr2, uint n, uint m)
{
 uint	i,
		j;

 for (i = 0; i < m; i++)
	for (j = 0; j < n; j++)
		*matr2++ = matr1[(n - 1 - j) * m + i];
}

/*****************************************************************
ROUTINE CHE APPLICA L' OPERATORE T1 AD UN VETTORE
vect1 = puntatore al vettore da trasporre
vect2 = puntatore al vettore trasposto
n = numero elementi vettore da trasporre
*****************************************************************/

S void oper_t1v(uchar *vect1, uchar *vect2, uint n)
{
 uint   i;
 int    j;
 uchar  mask1,
		mask2;

 for (i = 0; i < 8; i++, vect2++)
	{
	*vect2 = 0;
	mask2 = (uchar)(0x80 >> i);			/* maschera per la selezione del bit */
	for (j = n - 1, mask1 = 0x80; j >= 0; j--, mask1 >>= 1)
		if (vect1[j] & mask2)
			*vect2 |= mask1;
	}
}

/*****************************************************************
ROUTINE CHE APPLICA L' OPERATORE T2 AD UN VETTORE
vect1 = puntatore al vettore da trasporre
vect2 = puntatore al vettore trasposta
n = numero elementi vettore da trasporre
*****************************************************************/

S void oper_t2v(uchar *vect1, uchar *vect2, uint n)
{
 int	i;
 uint	j;
 uchar	mask1,
		mask2;

 for (i = 0; i < 8; i++, vect2++)
	{
	*vect2 = 0;
	mask2 = (uchar)(0x01 << i);			/* maschera per la selezione del bit */
	for (j = 0, mask1 = 0x80; j < n; j++, mask1 >>= 1)
		if (vect1[j] & mask2)
			*vect2 |= mask1;
	}
}

/*****************************************************************
ROUTINE CHE EFFETTUA LA TRASFORMAZIONE DI n BYTES IN UNA MATRICE nXbits
matr = puntatore alla matrice da definire
vect = puntatore al vettore da trasformare
n = numero di righe della matrice (o elementi della stringa finale )
bits = mumero di bits da trasformare
*****************************************************************/

S void makematr(uchar *vect, uchar *matr, uint n, uint bits)
{
 uint	i,
		j;
 uchar	a,
		b;

 for (i = 0; i < n; i++)
	{
	a = 128;							/* a="esaminatore bits" */
	b = *vect++;						/* b=byte in esame */
	for (j = 0; j < bits; j++)
		{
		if (b & a)						/* se bit=1 elemento matrice=1 */
			*matr++ = 1;
		else							/* se bit=0 elemento matrice=0 */
			*matr++ = 0;
		a >>= 1;
		}
	}
}

/**********************************************************************
ROUTINE CHE EFFETTUA LA PERMUTED CHOICE 1 PER IL CALCOLO DI CKEY E DKEY
totkey = puntatore alla matrice "chiave totale" (input)
ckey = puntatore alla matrice "chiave sinistra" (output)
dkey = puntatore alla matrice "chiave destra" (output)
**********************************************************************/

S void perm_choi_1(uchar *totkey, uchar *ckey, uchar *dkey)
{
 uint	i,
		j;

 oper_t1(totkey, appok1, 8, 7);			/* applica operatore T2 alla chiave */
 for (i = 0; i < 3; i++)
	for(j = 0; j < 8; j++)				/* applica operatore 1/2 SHdown */
		*ckey++ = appok1[i * 8 + j];	/* per definire CKEY */
 for (i = 6; i > 3; i--)
	for (j = 0; j < 8; j++)				/* applica operatore 1/2 SHup */
		*dkey++ = appok1[i * 8 + j];	/* per definire DKEY */
 for (j = 0; j < 4; j++)
	{
	*ckey++ = appok1[3 * 8 + j];		/* completa le 2 matrici */
	*dkey++ = appok1[3 * 8 + j + 4];
	}
}

/**********************************************************************
ROUTINE CHE EFFETTUA LA PERMUTED CHOICE 2 PER IL CALCOLO DI K(6x8)
totkey = puntatore alla matrice "chiave totale" (output)
ckey = puntatore alla matrice "chiave sinistra" (input)
dkey = puntatore alla matrice "chiave destra" (input)
**********************************************************************/

S void perm_choi_2(uchar *ckey, uchar *dkey, uchar *iterk)
{
 uint	i,
		j;
 char	mask = 0x20;

 for (i = 0; i < 8; i++, iterk++, mask = 0x20)
	for (j = 0, *iterk = 0; j < 6; j++, mask >>= 1)
		{								/* la permutazione e' */
		if (kperm2[i][j] < 29)			/* effettuata esplicitamente */
			*iterk |= ckey[kperm2[i][j] - 1] ? mask : 0;
		else
			*iterk |= dkey[kperm2[i][j] - 29] ? mask : 0;
		}
}

/**********************************************************************
ROUTINE CHE EFFETTUA IL CALCOLO DELLE MATRICI ITERKEY(6x8)
ckey = puntatore alla matrice "chiave sinistra" (input)
dkey = puntatore alla matrice "chiave destra" (input)
**********************************************************************/

S void calc_keys(uchar *ckey, uchar *dkey)
{
 uint	i,
		j;

 for (i = 0; i < 16; i++)
	{
	switch (i)							/* crea la matrice Ki */
		{
		case 0:
		case 1:
		case 8:
		case 15:
			make_lshift(ckey, dkey, 4, 7); /* un solo left shift */
			break;
		default:						/* due left shift */
			for (j = 0; j < 2; j++)
				make_lshift(ckey, dkey, 4, 7);
			break;
		}
	perm_choi_2(ckey, dkey, iterkey[i]); /* effettua permuted choice 2 */
	}
}

/*****************************************************************
ROUTINE CHE EFFETTUA IL VERO E PROPRIO CUORE DEL DES
decr = identificatore crypt\decrypt
lunga = puntatore al vettore su cui si effettuano meno operazioni
lungb = puntatore al vettore su cui si effettuano piu' operazioni
*****************************************************************/

S void iter_des(int decr,long *lunga, long *lungb)
{
 long	appol;
 uint	i,
		j;

 for (i = 0; i < 16; i++)
	{
	appol = *lungb;
	j = decr ? 15 - i : i;				/* crypt \ decrypt */
	func_des(lunga, lungb, iterkey[j]);
	*lunga = appol;
	}
 appol = *lungb;						/* scambio finale */
 *lungb = *lunga;
 *lunga = appol;
}

/*********************************************************************
ROUTINE CHE EFFETTUA IL CALCOLO DELLA FUNZIONE R = L ^ f(R,K)
lunga = puntatore al vettore operando
lungb = puntatore al vettore operando e risultato
iterk = matrice key da usare
*********************************************************************/

S void func_des(long *lunga, long *lungb, uchar *iterk)
{
 int	i;

 expandl((uchar *) lungb, itermatr, iterk); /* "espande" vettore */
 *lungb = 0;
 for (i = 0; i < 8; i++)				/* effettua le operazioni base del des */
	*lungb |= trasfp[i][itermatr[i]];	/* usando la matrice TRASF costruita */
 byteswap((uchar *) lungb);				/* all'inizializzazione */
 (*lungb) ^= (*lunga);
}

/*********************************************************************
ROUTINE CHE EFFETTUA IL LEFT SHIFT DI CKEY E DKEY SECONDO LA TABELLA DES
ckey = puntatore alla matrice "chiave sinistra"
dkey = puntatore alla matrice "chiave destra"
n = numero righe
m = numero colonne
*********************************************************************/

S void make_lshift(uchar *ckey, uchar *dkey, uint n, uint m)
{
 uint	j;
 uchar	appc,
		appd;

 appc = ckey[0];						/* salva i primi elementi */
 appd = dkey[0];
 for (j = 0; j < n * m - 1; j++)		/* shifta a sinistra */
	{
	ckey[j] = ckey[j + 1];
	dkey[j] = dkey[j + 1];
	}
 ckey[n * m - 1] = appc;				/* mette gli elementi salvati */
 dkey[n * m - 1] = appd;				/* in fondo alla matrice */
}

/*********************************************************************
ROUTINE CHE ESPANDE IL VETTORE DESTRO SECONDO UN PRECISO ALGORITMO
E POI NE CALCOLA L'OR ESCLUSIVO CON IL VETTORE "CHIAVE"
matr = puntatore al vettore da espandere
store = puntatore al vettore espanso
matrk = puntatore al vettore chiave
lo schema dell'espansione e':
ABCDEFGH,ILMNOPQR,abcdefgh,ilmnopqr
00DEFGHI 00HILMNO 00NOPQRa 00Rabcde 00defghi 00hilmno 00nopqrA
*********************************************************************/

S void expandl(uchar *vect, uchar *store, uchar *vectk)
{
 uint	i;

 for (i = 0; i < 4; i++)
	{
	*store = (uchar) ((vect[i] & 0xf0) >> 3);
	if (vect[(i - 1) & 3] & 0x01)
		*store |= 0x20;
	if (vect[i] & 0x08)
		*store |= 0x01;
	*store++ ^= *vectk++;
	*store = (uchar) ((vect[i] & 0x0f) << 1);
	if (vect[i] & 0x10)
		*store |= 0x20;
	if (vect[(i + 1) & 3] & 0x80)
		*store |= 0x01;
	*store++ ^= *vectk++;				/* exor con vettore derivato dalla chiave */
	}
}

/*****************************************************************
ROUTINE CHE INIZIALIZZA LA MATRICE DI TRASFORMAZIONE DERIVATA
DALLE MATRICI SELFUN
*****************************************************************/

#ifdef	USE_TRASF_INIT
S void trasf_init()
{
 uchar	pbox[32],
		i,
		p,
		s,
		j,
		rowcol;
 long	val;

 if (desIsInited)						/* esce se DES gia' inizializzato */
	return;

/* pbox e' tale che in pbox[i] c'e' la posizione che il carattere 'i+1' */
/* occupa in perm_fin, tale artificio semplifica il laroro successivo.	*/

 for (p = 0; p < 32; p++)
	for (i = 0; i < 32; i++)
		if (perm_fin[i] == (uchar) (p + 1))
			{
			pbox[p] = i;
			break;
			}
 for (s = 0; s < 8; s++)				/* per ogni S-box */
	for (i = 0; i < 64; i++)			/* per ogni possibile input */
		{
		val = 0;						/* la riga e' data da msb e lsb */
					/* la colonna dai 4 bits centrali */
		rowcol = (uchar) ((i & 32) | ((i & 1) ? 16 : 0) | ((i >> 1) & 0xf));
		for (j = 0; j < 4; j++)			/* per ogni possibile output */
			if (selfun[s][rowcol] & (8 >> j))
				val |= 1L << (31 - pbox[4 * s + j]);
		trasfp[s][i] = val;
		}
 desIsInited = 1;						/* indica DES inizializzato !!! */
}
#endif//USE_TRASF_INIT

///*****************************************************************
//ROUTINE CHE TRASFORMA UN LONG IN UNA RAPPRESENTAZIONE "NATURALE"
//*****************************************************************/
#if defined(_WIN32) || __BYTE_ORDER != __BIG_ENDIAN
S void byteswap(uchar *cp)
{
 uchar	*cp1,
		*cp2,
		tmp;
 int	j;

 cp2 = (cp1 = cp) + 3;
 for (j = 0; j < 2; j++)
	{
	tmp = *cp1;
	*cp1++ = *cp2;
	*cp2-- = tmp;
	}
}
#endif

///*****************************************************************
//ROUTINE for 3DES
//*****************************************************************/
bool triple_des(unsigned char *inputData, unsigned long ulInputDataLen, 
				  unsigned char *outputData, unsigned char *key)
{
	if ((ulInputDataLen % 8) != 0)
		return false;

	unsigned long i = 0;
	while (i < ulInputDataLen)
	{
		DES(&inputData[i],  &outputData[i], &key[0]);
		UNDES(&outputData[i],  &outputData[i], &key[8]);
		DES(&outputData[i],  &outputData[i], &key[0]);
		i += 8;
	}
	return true;
}

///*****************************************************************
//ROUTINE for reversed 3DES
//*****************************************************************/
bool reversed_triple_des(unsigned char *inputData,  unsigned long ulInputDataLen,  
						   unsigned char *outputData, unsigned char *key)
{
	if ((ulInputDataLen % 8) != 0)
		return false;

	unsigned long i = 0;
	while (i < ulInputDataLen)
	{
		UNDES(&inputData[i],  &outputData[i], &key[0]);
		DES(&outputData[i],  &outputData[i], &key[8]);
		UNDES(&outputData[i],  &outputData[i], &key[0]);
		i += 8;
	}

	return true;
}

