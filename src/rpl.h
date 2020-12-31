/*
 *  This file is part of x48, an emulator of the HP-48sx Calculator.
 *  Copyright (C) 1994  Eddie C. Dost  (ecd@dressler.de)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* $Log: rpl.h,v $
 * Revision 1.3  1995/01/11  18:20:01  ecd
 * major update to support HP48 G/GX
 *
 * Revision 1.2  1994/12/07  20:16:41  ecd
 * more functions added
 *
 * Revision 1.2  1994/12/07  20:16:41  ecd
 * more functions added
 *
 * Revision 1.1  1994/12/07  10:16:15  ecd
 * Initial revision
 *
 *
 * $Id: rpl.h,v 1.3 1995/01/11 18:20:01 ecd Exp ecd $
 */

#ifndef _RPL_H
#define _RPL_H 1

#include "global.h"
#include "hp48.h"

/*
 * Addresses in SX ROM
 */
#define ROMPTAB_SX	0x707d9
#define ROMPTAB_GX	0x809a3

/*
 *
 */
//| 38G  | 39G  | 40G  | 48SX | 48GX | 49G  | Name
//#F0688 #806E9 #806E9 #7056A #806E9 #806E9 =TEMPOB
//#F068D #806EE #806EE #7056F #806EE #806EE =TEMPTOP
//#F0692 #806F3 #806F3 #70574 #806F3 #806F3 =RSKTOP   (B)
//#F0697 #806F8 #806F8 #70579 #806F8 #806F8 =DSKTOP   (D1)
//#F0DEA #80E9B #80E9B #7066E #807ED #80E9B =AVMEM    (D)
//#F0705 #8076B #8076B #705B0 #8072F #8076B =INTRPPTR (D0)
//       #80F02 #80F02 #706C5 #80843 #80F02 =SystemFlags


# if 1 //HP48GX
#define TEMPOB      0x806E9
#define TEMPTOP     0x806EE
#define RSKTOP      0x806F3
#define DSKTOP      0x806F8
#define AVMEM       0x807ED
#define INTRPPTR    0x8072F
#define SYSTEMFLAGS 0x80843
# else
#define TEMPOB      0x7056A
#define TEMPTOP     0x7056F
#define RSKTOP      0x70574
#define DSKTOP      0x70579
#define AVMEM       0x7066E
#define INTRPPTR    0x705B0
#define SYSTEMFLAGS 0x706C5
# endif

/*
 * Object Prologs
 */
#define DOBINT		0x02911		/* System Binary	*/
#define DOREAL		0x02933		/* Real			*/
#define DOEREAL	0x02955		/* Extended Real	*/
#define DOCMP		0x02977		/* Complex		*/
#define DOECMP		0x0299d		/* Extended Complex	*/
#define DOCHAR		0x029bf		/* Character		*/
#define DOARRY 		0x029e8		/* Array		*/
#define DOLNKARRY	0x02a0a		/* Linked Array		*/
#define DOCSTR		0x02a2c		/* String		*/
#define DOHSTR		0x02a4e		/* Binary Integer	*/
#define DOLIST		0x02a74		/* List			*/
#define DORRP		0x02a96		/* Directory		*/
#define DOSYMB		0x02ab8		/* Algebraic		*/
#define DOEXT		0x02ada		/* Unit			*/
#define DOTAG		0x02afc		/* Tagged		*/
#define DOGROB		0x02b1e		/* Graphic Object	*/
#define DOLIB		0x02b40		/* Library		*/
#define DOBAK		0x02b62		/* Backup		*/
#define DOEXT0		0x02b88		/* Library Data		*/
#define DOACPTR		0x02baa		/*			*/
#define DOEXT1		0x02BAA		/* Extended Pointer */
#define DOEXT2		0x02bcc		/*			*/
#define DOEXT3		0x02bee		/*			*/
#define DOEXT4		0x02c10		/*			*/
#define DOCOL		0x02d9d		/* Program		*/
#define DOCODE		0x02dcc		/* Code			*/
#define DOIDNT		0x02e48		/* Global Name		*/
#define DOLAM		0x02e6d		/* Local Name		*/
#define DOROMP		0x02e92		/* XLib Name		*/

/*
 * Terminates composite objects
 */
#define SEMI		0x0312b		/* Semi			*/

/*
 * Unit Operators
 */
#define UM_MUL		0x10b5e		/* Unit Operator *	*/
#define UM_DIV		0x10b68		/* Unit Operator /	*/
#define UM_POW		0x10b72		/* Unit Operator ^	*/
#define UM_PRE		0x10b7c		/* Unit Operator prefix */
#define UM_END		0x10b86		/* Unit Operator _	*/

typedef struct hp_real {
  word_20	x;
  word_32	ml;
  word_32	mh;
  word_4	m;
  word_1	s;
} hp_real;

extern char *decode_rpl_obj	(word_20 addr, char *buf);

extern char *skip_ob		(word_20 *addr, char *string);
extern char *dec_rpl_obj	(word_20 *addr, char *string);
extern char *dec_bin_int	(word_20 *addr, char *string);
extern char *dec_real		(word_20 *addr, char *string);
extern char *dec_long_real	(word_20 *addr, char *string);
extern char *dec_complex	(word_20 *addr, char *string);
extern char *dec_long_complex	(word_20 *addr, char *string);
extern char *dec_char		(word_20 *addr, char *string);
extern char *dec_array		(word_20 *addr, char *string);
extern char *dec_lnk_array	(word_20 *addr, char *string);
extern char *dec_string		(word_20 *addr, char *string);
extern char *dec_hex_string	(word_20 *addr, char *string);
extern char *dec_list		(word_20 *addr, char *string);
extern char *dec_symb		(word_20 *addr, char *string);
extern char *dec_unit		(word_20 *addr, char *string);
extern char *dec_library	(word_20 *addr, char *string);
extern char *dec_library_data	(word_20 *addr, char *string);
extern char *dec_acptr		(word_20 *addr, char *string);
extern char *dec_prog		(word_20 *addr, char *string);
extern char *dec_code		(word_20 *addr, char *string);
extern char *dec_global_ident	(word_20 *addr, char *string);
extern char *dec_local_ident	(word_20 *addr, char *string);
extern char *dec_xlib_name	(word_20 *addr, char *string);
extern char *dec_unit_op	(word_20 *addr, char *string);

#endif /* !_RPL_H */
