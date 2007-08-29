/*DDK*************************************************************************/
/*                                                                           */
/* COPYRIGHT    Copyright (C) 1991, 2003 IBM Corporation                     */
/*                                                                           */
/*    The following IBM OS/2 source code is provided to you solely for       */
/*    the purpose of assisting you in your development of OS/2 device        */
/*    drivers. You may use this code in accordance with the IBM License      */
/*    Agreement provided in the IBM Developer Connection Device Driver       */
/*    Source Kit for OS/2. This Copyright statement may not be removed.      */
/*                                                                           */
/*****************************************************************************/
/**************************************************************************
 *
 * SOURCE FILE NAME = PPDENC.C
 *
 * DESCRIPTIVE NAME = Convert PPD encoding to OS/2 codepage
 *
 *
 * VERSION = V1.0
 *
 * DATE
 *
 * DESCRIPTION  PPD Encoding converter main file.
 *
 *
 * FUNCTIONS
 *
 *
 *
 * NOTES
 *
 *
 * STRUCTURES
 *
 * EXTERNAL REFERENCES
 *
 * EXTERNAL FUNCTIONS
 *

 * bvl: Added UTF-8 Support, we simply substitute the ISOLatin table for it, supposedly the first 256 chars of UTF are ISOLatin, it at least solves our problems
*/


#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "listfls.h"

//#define FALSE 0
//#define TRUE  1



#define CONVERT_ISOLATIN1      0
#define CONVERT_WINDOWSANSI    1
#define CONVERT_MAC            2
#define CONVERT_UTF8           3
#define CONVERT_UNKNOWN        1000


char *MRI_LANG = NULL;


// copy pathname from source to dest, and add trailing slash, if missing
static
void copy_pathname( char *dest, char *src)
{
    strcpy(dest,src);

    // add slash as last character, if it's not present
    if(dest[strlen(dest)-1]!='\\')
        strcat(dest,"\\");
}




// charset conversion tables


char FromIso[256] =
{
  /* Code */ /* source character name */  /* destionation code */
  /*   0  */ /* -unused- */                  0x00,
  /*   1  */ /* -unused- */                  0x01,
  /*   2  */ /* -unused- */                  0x02,
  /*   3  */ /* -unused- */                  0x03,
  /*   4  */ /* -unused- */                  0x04,
  /*   5  */ /* -unused- */                  0x05,
  /*   6  */ /* -unused- */                  0x06,
  /*   7  */ /* -unused- */                  0x07,
  /*   8  */ /* -unused- */                  0x08,
  /*   9  */ /* -unused- */                  0x09,
  /*  10  */ /* -unused- */                  0x0A,
  /*  11  */ /* -unused- */                  0x0B,
  /*  12  */ /* -unused- */                  0x0C,
  /*  13  */ /* -unused- */                  0x0D,
  /*  14  */ /* -unused- */                  0x0E,
  /*  15  */ /* -unused- */                  0x0F,
  /*  16  */ /* -unused- */                  0x10,
  /*  17  */ /* -unused- */                  0x11,
  /*  18  */ /* -unused- */                  0x12,
  /*  19  */ /* -unused- */                  0x13,
  /*  20  */ /* -unused- */                  0x14,
  /*  21  */ /* -unused- */                  0x15,
  /*  22  */ /* -unused- */                  0x16,
  /*  23  */ /* -unused- */                  0x17,
  /*  24  */ /* -unused- */                  0x18,
  /*  25  */ /* -unused- */                  0x19,
  /*  26  */ /* -unused- */                  0x1A,
  /*  27  */ /* -unused- */                  0x1B,
  /*  28  */ /* -unused- */                  0x1C,
  /*  29  */ /* -unused- */                  0x1D,
  /*  30  */ /* -unused- */                  0x1E,
  /*  31  */ /* -unused- */                  0x1F,
  /*  32  */ /* space */                     0x20,
  /*  33  */ /* exclam */                    0x21,
  /*  34  */ /* quotedbl */                  0x22,
  /*  35  */ /* numbersign */                0x23,
  /*  36  */ /* dollar */                    0x24,
  /*  37  */ /* percent */                   0x25,
  /*  38  */ /* ampersand */                 0x26,
  /*  39 x*/ /* quoteright */                0x27,
  /*  40  */ /* parenleft */                 0x28,
  /*  41  */ /* parenright */                0x29,
  /*  42  */ /* asterisk */                  0x2A,
  /*  43  */ /* plus */                      0x2B,
  /*  44  */ /* comma */                     0x2C,
  /*  45 x*/ /* minus */                     0x2D,
  /*  46  */ /* period */                    0x2E,
  /*  47  */ /* slash */                     0x2F,
  /*  48  */ /* zero */                      0x30,
  /*  49  */ /* one  */                      0x31,
  /*  50  */ /* two */                       0x32,
  /*  51  */ /* three */                     0x33,
  /*  52  */ /* four */                      0x34,
  /*  53  */ /* five */                      0x35,
  /*  54  */ /* six */                       0x36,
  /*  55  */ /* seven */                     0x37,
  /*  56  */ /* eight */                     0x38,
  /*  57  */ /* nine */                      0x39,
  /*  58  */ /* colon */                     0x3A,
  /*  59  */ /* semicolon */                 0x3B,
  /*  60  */ /* less */                      0x3C,
  /*  61  */ /* equal */                     0x3D,
  /*  62  */ /* greater */                   0x3E,
  /*  63  */ /* question */                  0x3F,
  /*  64  */ /* at */                        0x40,
  /*  65  */ /* A */                         0x41,
  /*  66  */ /* B */                         0x42,
  /*  67  */ /* C */                         0x43,
  /*  68  */ /* D */                         0x44,
  /*  69  */ /* E */                         0x45,
  /*  70  */ /* F */                         0x46,
  /*  71  */ /* G */                         0x47,
  /*  72  */ /* H */                         0x48,
  /*  73  */ /* I */                         0x49,
  /*  74  */ /* J */                         0x4A,
  /*  75  */ /* K */                         0x4B,
  /*  76  */ /* L */                         0x4C,
  /*  77  */ /* M */                         0x4D,
  /*  78  */ /* N */                         0x4E,
  /*  79  */ /* O */                         0x4F,
  /*  80  */ /* P */                         0x50,
  /*  81  */ /* Q */                         0x51,
  /*  82  */ /* R */                         0x52,
  /*  83  */ /* S */                         0x53,
  /*  84  */ /* T */                         0x54,
  /*  85  */ /* U */                         0x55,
  /*  86  */ /* V */                         0x56,
  /*  87  */ /* W */                         0x57,
  /*  88  */ /* X */                         0x58,
  /*  89  */ /* Y */                         0x59,
  /*  90  */ /* Z */                         0x5A,
  /*  91  */ /* bracketleft */               0x5B,
  /*  92  */ /* backslash */                 0x5C,
  /*  93  */ /* bracketright */              0x5D,
  /*  94  */ /* asciicircum */               0x5E,
  /*  95  */ /* underscore */                0x5F,
  /*  96 x*/ /* quoteleft */                 0x60,
  /*  97  */ /* a */                         0x61,
  /*  98  */ /* b */                         0x62,
  /*  99  */ /* c */                         0x63,
  /* 100  */ /* d */                         0x64,
  /* 101  */ /* e */                         0x65,
  /* 102  */ /* f */                         0x66,
  /* 103  */ /* g */                         0x67,
  /* 104  */ /* h */                         0x68,
  /* 105  */ /* i */                         0x69,
  /* 106  */ /* j */                         0x6A,
  /* 107  */ /* k */                         0x6B,
  /* 108  */ /* l */                         0x6C,
  /* 109  */ /* m */                         0x6D,
  /* 110  */ /* n */                         0x6E,
  /* 111  */ /* o */                         0x6F,
  /* 112  */ /* p */                         0x70,
  /* 113  */ /* q */                         0x71,
  /* 114  */ /* r */                         0x72,
  /* 115  */ /* s */                         0x73,
  /* 116  */ /* t */                         0x74,
  /* 117  */ /* u */                         0x75,
  /* 118  */ /* v */                         0x76,
  /* 119  */ /* w */                         0x77,
  /* 120  */ /* x */                         0x78,
  /* 121  */ /* y */                         0x79,
  /* 122  */ /* z */                         0x7A,
  /* 123  */ /* braceleft */                 0x7B,
  /* 124  */ /* bar */                       0x7C,
  /* 125  */ /* braceright */                0x7D,
  /* 126  */ /* asciitilde */                0x7E,
  /* 127  */ /* -unused- */                  0x7F,
  /* 128  */ /* -unused- */                  0x80,
  /* 129  */ /* -unused- */                  0x81,
  /* 130  */ /* -unused- */                  0x82,
  /* 131  */ /* -unused- */                  0x83,
  /* 132  */ /* -unused- */                  0x84,
  /* 133  */ /* -unused- */                  0x85,
  /* 134  */ /* -unused- */                  0x86,
  /* 135  */ /* -unused- */                  0x87,
  /* 136  */ /* -unused- */                  0x88,
  /* 137  */ /* -unused- */                  0x89,
  /* 138  */ /* -unused- */                  0x90,
  /* 139  */ /* -unused- */                  0x91,
  /* 140  */ /* -unused- */                  0x92,
  /* 141  */ /* -unused- */                  0x93,
  /* 142  */ /* -unused- */                  0x94,
  /* 143  */ /* -unused- */                  0x95,

// unclear
  /* 144 x*/ /* dotlessi */                  0x96,
  /* 145  */ /* grave */                     0x60,
  /* 146  */ /* acute */                     0xEF,
  /* 147 x*/ /* circumflex */                0x99,
  /* 148 x*/ /* tilde */                     0x9A,
  /* 149 x*/ /* macron */                    0x9B,
  /* 150 x*/ /* breve */                     0x9C,
  /* 151 x*/ /* dotaccent */                 0x9E,
  /* 152  */ /* dieresis */                  0xF9,
  /* 153  */ /* -unused- */                  0xA0,
  /* 154 x*/ /* ring */                      0xA1,
  /* 155  */ /* cedilla */                   0xF7,
  /* 156  */ /* -unused- */                  0xA3,
  /* 157 x*/ /* hungarumlaut */              0xA4,
  /* 158 x*/ /* ogonek */                    0xA5,
  /* 159 x*/ /* caron */                     0xA6,
  /* 160  */ /* space [reqspace] */          0xFF,
// unclear

  /* 161  */ /* exclamdown */                0xAD,
  /* 162  */ /* cent */                      0xBD,
  /* 163  */ /* sterling */                  0x9C,
  /* 164  */ /* currency */                  0xCF,
  /* 165  */ /* yen */                       0xBE,
  /* 166  */ /* brokenbar */                 0xDD,
  /* 167  */ /* section */                   0xF5,
  /* 168  */ /* dieresis */                  0xF9,
  /* 169  */ /* copyright */                 0xB8,
  /* 170  */ /* ordfeminine */               0xA6,
  /* 171  */ /* guillemotleft */             0xAE,
  /* 172  */ /* logicalnot */                0xAA,
  /* 173  */ /* hyphen [syllable] */         0xF0,
  /* 174  */ /* registered */                0xA9,
  /* 175  */ /* macron [overline] */         0xEE,
  /* 176  */ /* degree */                    0xF8,
  /* 177  */ /* plusminus */                 0xF1,
  /* 178  */ /* twosuperior */               0xFD,
  /* 179  */ /* threesuperior */             0xFC,
  /* 180  */ /* acute */                     0xEF,
  /* 181  */ /* mu */                        0xE6,
  /* 182  */ /* paragraph */                 0xF4,
  /* 183  */ /* periodcentered */            0xFA,
  /* 184  */ /* cedilla */                   0xF7,
  /* 185  */ /* onesuperior */               0xFB,
  /* 186  */ /* ordmasculine */              0xA7,
  /* 187  */ /* guillemotright */            0xAF,
  /* 188  */ /* onequarter */                0xAC,
  /* 189  */ /* onehalf */                   0xAB,
  /* 190  */ /* threequarters */             0xF3,
  /* 191  */ /* questiondown */              0xA8,
  /* 192  */ /* Agrave */                    0xB7,
  /* 193  */ /* Aacute */                    0xB5,
  /* 194  */ /* Acircumflex */               0xB6,
  /* 195  */ /* Atilde */                    0xC7,
  /* 196  */ /* Adieresis */                 0x8E,
  /* 197  */ /* Aring */                     0x8F,
  /* 198  */ /* AE */                        0x92,
  /* 199  */ /* Ccedilla */                  0x80,
  /* 200  */ /* Egrave */                    0xD4,
  /* 201  */ /* Eacute */                    0x90,
  /* 202  */ /* Ecircumflex */               0xD2,
  /* 203  */ /* Edieresis */                 0xD3,
  /* 204  */ /* Igrave */                    0xDE,
  /* 205  */ /* Iacute */                    0xD6,
  /* 206  */ /* Icircumflex */               0xD7,
  /* 207  */ /* Idieresis */                 0xD8,
  /* 208  */ /* Eth */                       0xD1,
  /* 209  */ /* Ntilde */                    0xA5,
  /* 210  */ /* Ograve */                    0xE3,
  /* 211  */ /* Oacute */                    0xE0,
  /* 212  */ /* Ocircumflex */               0xE2,
  /* 213  */ /* Otilde */                    0xE5,
  /* 214  */ /* Odieresis */                 0x99,
  /* 215  */ /* multiply */                  0x9E,
  /* 216  */ /* Oslash */                    0x9D,
  /* 217  */ /* Ugrave */                    0xEB,
  /* 218  */ /* Uacute */                    0xE9,
  /* 219  */ /* Ucircumflex */               0xEA,
  /* 220  */ /* Udieresis */                 0x9A,
  /* 221  */ /* Yacute */                    0xED,
  /* 222  */ /* Thorn */                     0xE8,
  /* 223  */ /* germandbls */                0xE1,
  /* 224  */ /* agrave */                    0x85,
  /* 225  */ /* aacute */                    0xA0,
  /* 226  */ /* acircumflex */               0x83,
  /* 227  */ /* atilde */                    0xC6,
  /* 228  */ /* adieresis */                 0x84,
  /* 229  */ /* aring */                     0x86,
  /* 230  */ /* ae */                        0x91,
  /* 231  */ /* ccedilla */                  0x87,
  /* 232  */ /* egrave */                    0x8A,
  /* 233  */ /* eacute */                    0x82,
  /* 234  */ /* ecircumflex */               0x88,
  /* 235  */ /* edieresis */                 0x89,
  /* 236  */ /* igrave */                    0x8D,
  /* 237  */ /* iacute */                    0xA1,
  /* 238  */ /* icircumflex */               0x8C,
  /* 239  */ /* idieresis */                 0x8B,
  /* 240  */ /* eth */                       0xD0,
  /* 241  */ /* ntilde */                    0xA4,
  /* 242  */ /* ograve */                    0x95,
  /* 243  */ /* oacute */                    0xA2,
  /* 244  */ /* ocircumflex */               0x93,
  /* 245  */ /* otilde */                    0xE4,
  /* 246  */ /* odieresis */                 0x94,
  /* 247  */ /* divide */                    0xF6,
  /* 248  */ /* oslash */                    0x9B,
  /* 249  */ /* ugrave */                    0x97,
  /* 250  */ /* uacute */                    0xA3,
  /* 251  */ /* ucircumflex */               0x96,
  /* 252  */ /* udieresis */                 0x81,
  /* 253  */ /* yacute */                    0xEC,
  /* 254  */ /* thorn */                     0xE7,
  /* 255  */ /* ydieresis */                 0x98
};

char FromUTF8[256] =
{
  /* Code */ /* source character name */  /* destionation code */
  /*   0  */ /* -unused- */                  0x00,
  /*   1  */ /* -unused- */                  0x01,
  /*   2  */ /* -unused- */                  0x02,
  /*   3  */ /* -unused- */                  0x03,
  /*   4  */ /* -unused- */                  0x04,
  /*   5  */ /* -unused- */                  0x05,
  /*   6  */ /* -unused- */                  0x06,
  /*   7  */ /* -unused- */                  0x07,
  /*   8  */ /* -unused- */                  0x08,
  /*   9  */ /* -unused- */                  0x09,
  /*  10  */ /* -unused- */                  0x0A,
  /*  11  */ /* -unused- */                  0x0B,
  /*  12  */ /* -unused- */                  0x0C,
  /*  13  */ /* -unused- */                  0x0D,
  /*  14  */ /* -unused- */                  0x0E,
  /*  15  */ /* -unused- */                  0x0F,
  /*  16  */ /* -unused- */                  0x10,
  /*  17  */ /* -unused- */                  0x11,
  /*  18  */ /* -unused- */                  0x12,
  /*  19  */ /* -unused- */                  0x13,
  /*  20  */ /* -unused- */                  0x14,
  /*  21  */ /* -unused- */                  0x15,
  /*  22  */ /* -unused- */                  0x16,
  /*  23  */ /* -unused- */                  0x17,
  /*  24  */ /* -unused- */                  0x18,
  /*  25  */ /* -unused- */                  0x19,
  /*  26  */ /* -unused- */                  0x1A,
  /*  27  */ /* -unused- */                  0x1B,
  /*  28  */ /* -unused- */                  0x1C,
  /*  29  */ /* -unused- */                  0x1D,
  /*  30  */ /* -unused- */                  0x1E,
  /*  31  */ /* -unused- */                  0x1F,
  /*  32  */ /* space */                     0x20,
  /*  33  */ /* exclam */                    0x21,
  /*  34  */ /* quotedbl */                  0x22,
  /*  35  */ /* numbersign */                0x23,
  /*  36  */ /* dollar */                    0x24,
  /*  37  */ /* percent */                   0x25,
  /*  38  */ /* ampersand */                 0x26,
  /*  39 x*/ /* quoteright */                0x27,
  /*  40  */ /* parenleft */                 0x28,
  /*  41  */ /* parenright */                0x29,
  /*  42  */ /* asterisk */                  0x2A,
  /*  43  */ /* plus */                      0x2B,
  /*  44  */ /* comma */                     0x2C,
  /*  45 x*/ /* minus */                     0x2D,
  /*  46  */ /* period */                    0x2E,
  /*  47  */ /* slash */                     0x2F,
  /*  48  */ /* zero */                      0x30,
  /*  49  */ /* one  */                      0x31,
  /*  50  */ /* two */                       0x32,
  /*  51  */ /* three */                     0x33,
  /*  52  */ /* four */                      0x34,
  /*  53  */ /* five */                      0x35,
  /*  54  */ /* six */                       0x36,
  /*  55  */ /* seven */                     0x37,
  /*  56  */ /* eight */                     0x38,
  /*  57  */ /* nine */                      0x39,
  /*  58  */ /* colon */                     0x3A,
  /*  59  */ /* semicolon */                 0x3B,
  /*  60  */ /* less */                      0x3C,
  /*  61  */ /* equal */                     0x3D,
  /*  62  */ /* greater */                   0x3E,
  /*  63  */ /* question */                  0x3F,
  /*  64  */ /* at */                        0x40,
  /*  65  */ /* A */                         0x41,
  /*  66  */ /* B */                         0x42,
  /*  67  */ /* C */                         0x43,
  /*  68  */ /* D */                         0x44,
  /*  69  */ /* E */                         0x45,
  /*  70  */ /* F */                         0x46,
  /*  71  */ /* G */                         0x47,
  /*  72  */ /* H */                         0x48,
  /*  73  */ /* I */                         0x49,
  /*  74  */ /* J */                         0x4A,
  /*  75  */ /* K */                         0x4B,
  /*  76  */ /* L */                         0x4C,
  /*  77  */ /* M */                         0x4D,
  /*  78  */ /* N */                         0x4E,
  /*  79  */ /* O */                         0x4F,
  /*  80  */ /* P */                         0x50,
  /*  81  */ /* Q */                         0x51,
  /*  82  */ /* R */                         0x52,
  /*  83  */ /* S */                         0x53,
  /*  84  */ /* T */                         0x54,
  /*  85  */ /* U */                         0x55,
  /*  86  */ /* V */                         0x56,
  /*  87  */ /* W */                         0x57,
  /*  88  */ /* X */                         0x58,
  /*  89  */ /* Y */                         0x59,
  /*  90  */ /* Z */                         0x5A,
  /*  91  */ /* bracketleft */               0x5B,
  /*  92  */ /* backslash */                 0x5C,
  /*  93  */ /* bracketright */              0x5D,
  /*  94  */ /* asciicircum */               0x5E,
  /*  95  */ /* underscore */                0x5F,
  /*  96 x*/ /* quoteleft */                 0x60,
  /*  97  */ /* a */                         0x61,
  /*  98  */ /* b */                         0x62,
  /*  99  */ /* c */                         0x63,
  /* 100  */ /* d */                         0x64,
  /* 101  */ /* e */                         0x65,
  /* 102  */ /* f */                         0x66,
  /* 103  */ /* g */                         0x67,
  /* 104  */ /* h */                         0x68,
  /* 105  */ /* i */                         0x69,
  /* 106  */ /* j */                         0x6A,
  /* 107  */ /* k */                         0x6B,
  /* 108  */ /* l */                         0x6C,
  /* 109  */ /* m */                         0x6D,
  /* 110  */ /* n */                         0x6E,
  /* 111  */ /* o */                         0x6F,
  /* 112  */ /* p */                         0x70,
  /* 113  */ /* q */                         0x71,
  /* 114  */ /* r */                         0x72,
  /* 115  */ /* s */                         0x73,
  /* 116  */ /* t */                         0x74,
  /* 117  */ /* u */                         0x75,
  /* 118  */ /* v */                         0x76,
  /* 119  */ /* w */                         0x77,
  /* 120  */ /* x */                         0x78,
  /* 121  */ /* y */                         0x79,
  /* 122  */ /* z */                         0x7A,
  /* 123  */ /* braceleft */                 0x7B,
  /* 124  */ /* bar */                       0x7C,
  /* 125  */ /* braceright */                0x7D,
  /* 126  */ /* asciitilde */                0x7E,
  /* 127  */ /* -unused- */                  0x7F,
  /* 128  */ /* -unused- */                  0x80,
  /* 129  */ /* -unused- */                  0x81,
  /* 130  */ /* -unused- */                  0x82,
  /* 131  */ /* -unused- */                  0x83,
  /* 132  */ /* -unused- */                  0x84,
  /* 133  */ /* -unused- */                  0x85,
  /* 134  */ /* -unused- */                  0x86,
  /* 135  */ /* -unused- */                  0x87,
  /* 136  */ /* -unused- */                  0x88,
  /* 137  */ /* -unused- */                  0x89,
  /* 138  */ /* -unused- */                  0x90,
  /* 139  */ /* -unused- */                  0x91,
  /* 140  */ /* -unused- */                  0x92,
  /* 141  */ /* -unused- */                  0x93,
  /* 142  */ /* -unused- */                  0x94,
  /* 143  */ /* -unused- */                  0x95,

// unclear
  /* 144 x*/ /* dotlessi */                  0x96,
  /* 145  */ /* grave */                     0x60,
  /* 146  */ /* acute */                     0xEF,
  /* 147 x*/ /* circumflex */                0x99,
  /* 148 x*/ /* tilde */                     0x9A,
  /* 149 x*/ /* macron */                    0x9B,
  /* 150 x*/ /* breve */                     0x9C,
  /* 151 x*/ /* dotaccent */                 0x9E,
  /* 152  */ /* dieresis */                  0xF9,
  /* 153  */ /* -unused- */                  0xA0,
  /* 154 x*/ /* ring */                      0xA1,
  /* 155  */ /* cedilla */                   0xF7,
  /* 156  */ /* -unused- */                  0xA3,
  /* 157 x*/ /* hungarumlaut */              0xA4,
  /* 158 x*/ /* ogonek */                    0xA5,
  /* 159 x*/ /* caron */                     0xA6,
  /* 160  */ /* space [reqspace] */          0xFF,
// unclear

  /* 161  */ /* exclamdown */                0xAD,
  /* 162  */ /* cent */                      0xBD,
  /* 163  */ /* sterling */                  0x9C,
  /* 164  */ /* currency */                  0xCF,
  /* 165  */ /* yen */                       0xBE,
  /* 166  */ /* brokenbar */                 0xDD,
  /* 167  */ /* section */                   0xF5,
  /* 168  */ /* dieresis */                  0xF9,
  /* 169  */ /* copyright */                 0xB8,
  /* 170  */ /* ordfeminine */               0xA6,
  /* 171  */ /* guillemotleft */             0xAE,
  /* 172  */ /* logicalnot */                0xAA,
  /* 173  */ /* hyphen [syllable] */         0xF0,
  /* 174  */ /* registered */                0xA9,
  /* 175  */ /* macron [overline] */         0xEE,
  /* 176  */ /* degree */                    0xF8,
  /* 177  */ /* plusminus */                 0xF1,
  /* 178  */ /* twosuperior */               0xFD,
  /* 179  */ /* threesuperior */             0xFC,
  /* 180  */ /* acute */                     0xEF,
  /* 181  */ /* mu */                        0xE6,
  /* 182  */ /* paragraph */                 0xF4,
  /* 183  */ /* periodcentered */            0xFA,
  /* 184  */ /* cedilla */                   0xF7,
  /* 185  */ /* onesuperior */               0xFB,
  /* 186  */ /* ordmasculine */              0xA7,
  /* 187  */ /* guillemotright */            0xAF,
  /* 188  */ /* onequarter */                0xAC,
  /* 189  */ /* onehalf */                   0xAB,
  /* 190  */ /* threequarters */             0xF3,
  /* 191  */ /* questiondown */              0xA8,
  /* 192  */ /* Agrave */                    0xB7,
  /* 193  */ /* Aacute */                    0xB5,
  /* 194  */ /* Acircumflex */               0xB6,
  /* 195  */ /* Atilde */                    0xC7,
  /* 196  */ /* Adieresis */                 0x8E,
  /* 197  */ /* Aring */                     0x8F,
  /* 198  */ /* AE */                        0x92,
  /* 199  */ /* Ccedilla */                  0x80,
  /* 200  */ /* Egrave */                    0xD4,
  /* 201  */ /* Eacute */                    0x90,
  /* 202  */ /* Ecircumflex */               0xD2,
  /* 203  */ /* Edieresis */                 0xD3,
  /* 204  */ /* Igrave */                    0xDE,
  /* 205  */ /* Iacute */                    0xD6,
  /* 206  */ /* Icircumflex */               0xD7,
  /* 207  */ /* Idieresis */                 0xD8,
  /* 208  */ /* Eth */                       0xD1,
  /* 209  */ /* Ntilde */                    0xA5,
  /* 210  */ /* Ograve */                    0xE3,
  /* 211  */ /* Oacute */                    0xE0,
  /* 212  */ /* Ocircumflex */               0xE2,
  /* 213  */ /* Otilde */                    0xE5,
  /* 214  */ /* Odieresis */                 0x99,
  /* 215  */ /* multiply */                  0x9E,
  /* 216  */ /* Oslash */                    0x9D,
  /* 217  */ /* Ugrave */                    0xEB,
  /* 218  */ /* Uacute */                    0xE9,
  /* 219  */ /* Ucircumflex */               0xEA,
  /* 220  */ /* Udieresis */                 0x9A,
  /* 221  */ /* Yacute */                    0xED,
  /* 222  */ /* Thorn */                     0xE8,
  /* 223  */ /* germandbls */                0xE1,
  /* 224  */ /* agrave */                    0x85,
  /* 225  */ /* aacute */                    0xA0,
  /* 226  */ /* acircumflex */               0x83,
  /* 227  */ /* atilde */                    0xC6,
  /* 228  */ /* adieresis */                 0x84,
  /* 229  */ /* aring */                     0x86,
  /* 230  */ /* ae */                        0x91,
  /* 231  */ /* ccedilla */                  0x87,
  /* 232  */ /* egrave */                    0x8A,
  /* 233  */ /* eacute */                    0x82,
  /* 234  */ /* ecircumflex */               0x88,
  /* 235  */ /* edieresis */                 0x89,
  /* 236  */ /* igrave */                    0x8D,
  /* 237  */ /* iacute */                    0xA1,
  /* 238  */ /* icircumflex */               0x8C,
  /* 239  */ /* idieresis */                 0x8B,
  /* 240  */ /* eth */                       0xD0,
  /* 241  */ /* ntilde */                    0xA4,
  /* 242  */ /* ograve */                    0x95,
  /* 243  */ /* oacute */                    0xA2,
  /* 244  */ /* ocircumflex */               0x93,
  /* 245  */ /* otilde */                    0xE4,
  /* 246  */ /* odieresis */                 0x94,
  /* 247  */ /* divide */                    0xF6,
  /* 248  */ /* oslash */                    0x9B,
  /* 249  */ /* ugrave */                    0x97,
  /* 250  */ /* uacute */                    0xA3,
  /* 251  */ /* ucircumflex */               0x96,
  /* 252  */ /* udieresis */                 0x81,
  /* 253  */ /* yacute */                    0xEC,
  /* 254  */ /* thorn */                     0xE7,
  /* 255  */ /* ydieresis */                 0x98
};


// WindowsAnsi to Ibm850
char FromWindows[256] =
{
  /* Code */ /* source character name */  /* destionation code */
  /*   0  */ /* -unused- */                  0x00,
  /*   1  */ /* -unused- */                  0x01,
  /*   2  */ /* -unused- */                  0x02,
  /*   3  */ /* -unused- */                  0x03,
  /*   4  */ /* -unused- */                  0x04,
  /*   5  */ /* -unused- */                  0x05,
  /*   6  */ /* -unused- */                  0x06,
  /*   7  */ /* -unused- */                  0x07,
  /*   8  */ /* -unused- */                  0x08,
  /*   9  */ /* -unused- */                  0x09,
  /*  10  */ /* -unused- */                  0x0A,
  /*  11  */ /* -unused- */                  0x0B,
  /*  12  */ /* -unused- */                  0x0C,
  /*  13  */ /* -unused- */                  0x0D,
  /*  14  */ /* -unused- */                  0x0E,
  /*  15  */ /* -unused- */                  0x0F,
  /*  16  */ /* -unused- */                  0x10,
  /*  17  */ /* -unused- */                  0x11,
  /*  18  */ /* -unused- */                  0x12,
  /*  19  */ /* -unused- */                  0x13,
  /*  20  */ /* -unused- */                  0x14,
  /*  21  */ /* -unused- */                  0x15,
  /*  22  */ /* -unused- */                  0x16,
  /*  23  */ /* -unused- */                  0x17,
  /*  24  */ /* -unused- */                  0x18,
  /*  25  */ /* -unused- */                  0x19,
  /*  26  */ /* -unused- */                  0x1A,
  /*  27  */ /* -unused- */                  0x1B,
  /*  28  */ /* -unused- */                  0x1C,
  /*  29  */ /* -unused- */                  0x1D,
  /*  30  */ /* -unused- */                  0x1E,
  /*  31  */ /* -unused- */                  0x1F,
  /*  32  */ /* space */                     0x20,
  /*  33  */ /* exclam */                    0x21,
  /*  34  */ /* quotedbl */                  0x22,
  /*  35  */ /* numbersign */                0x23,
  /*  36  */ /* dollar */                    0x24,
  /*  37  */ /* percent */                   0x25,
  /*  38  */ /* ampersand */                 0x26,
  /*  39 x*/ /* quoteright */                0x27,
  /*  40  */ /* parenleft */                 0x28,
  /*  41  */ /* parenright */                0x29,
  /*  42  */ /* asterisk */                  0x2A,
  /*  43  */ /* plus */                      0x2B,
  /*  44  */ /* comma */                     0x2C,
  /*  45 x*/ /* hyphen */                    0x2D,
  /*  46  */ /* period */                    0x2E,
  /*  47  */ /* slash */                     0x2F,
  /*  48  */ /* zero */                      0x30,
  /*  49  */ /* one  */                      0x31,
  /*  50  */ /* two */                       0x32,
  /*  51  */ /* three */                     0x33,
  /*  52  */ /* four */                      0x34,
  /*  53  */ /* five */                      0x35,
  /*  54  */ /* six */                       0x36,
  /*  55  */ /* seven */                     0x37,
  /*  56  */ /* eight */                     0x38,
  /*  57  */ /* nine */                      0x39,
  /*  58  */ /* colon */                     0x3A,
  /*  59  */ /* semicolon */                 0x3B,
  /*  60  */ /* less */                      0x3C,
  /*  61  */ /* equal */                     0x3D,
  /*  62  */ /* greater */                   0x3E,
  /*  63  */ /* question */                  0x3F,
  /*  64  */ /* at */                        0x40,
  /*  65  */ /* A */                         0x41,
  /*  66  */ /* B */                         0x42,
  /*  67  */ /* C */                         0x43,
  /*  68  */ /* D */                         0x44,
  /*  69  */ /* E */                         0x45,
  /*  70  */ /* F */                         0x46,
  /*  71  */ /* G */                         0x47,
  /*  72  */ /* H */                         0x48,
  /*  73  */ /* I */                         0x49,
  /*  74  */ /* J */                         0x4A,
  /*  75  */ /* K */                         0x4B,
  /*  76  */ /* L */                         0x4C,
  /*  77  */ /* M */                         0x4D,
  /*  78  */ /* N */                         0x4E,
  /*  79  */ /* O */                         0x4F,
  /*  80  */ /* P */                         0x50,
  /*  81  */ /* Q */                         0x51,
  /*  82  */ /* R */                         0x52,
  /*  83  */ /* S */                         0x53,
  /*  84  */ /* T */                         0x54,
  /*  85  */ /* U */                         0x55,
  /*  86  */ /* V */                         0x56,
  /*  87  */ /* W */                         0x57,
  /*  88  */ /* X */                         0x58,
  /*  89  */ /* Y */                         0x59,
  /*  90  */ /* Z */                         0x5A,
  /*  91  */ /* bracketleft */               0x5B,
  /*  92  */ /* backslash */                 0x5C,
  /*  93  */ /* bracketright */              0x5D,
  /*  94  */ /* asciicircum */               0x5E,
  /*  95  */ /* underscore */                0x5F,
  /*  96  */ /* grave */                     0x60,
  /*  97  */ /* a */                         0x61,
  /*  98  */ /* b */                         0x62,
  /*  99  */ /* c */                         0x63,
  /* 100  */ /* d */                         0x64,
  /* 101  */ /* e */                         0x65,
  /* 102  */ /* f */                         0x66,
  /* 103  */ /* g */                         0x67,
  /* 104  */ /* h */                         0x68,
  /* 105  */ /* i */                         0x69,
  /* 106  */ /* j */                         0x6A,
  /* 107  */ /* k */                         0x6B,
  /* 108  */ /* l */                         0x6C,
  /* 109  */ /* m */                         0x6D,
  /* 110  */ /* n */                         0x6E,
  /* 111  */ /* o */                         0x6F,
  /* 112  */ /* p */                         0x70,
  /* 113  */ /* q */                         0x71,
  /* 114  */ /* r */                         0x72,
  /* 115  */ /* s */                         0x73,
  /* 116  */ /* t */                         0x74,
  /* 117  */ /* u */                         0x75,
  /* 118  */ /* v */                         0x76,
  /* 119  */ /* w */                         0x77,
  /* 120  */ /* x */                         0x78,
  /* 121  */ /* y */                         0x79,
  /* 122  */ /* z */                         0x7A,
  /* 123  */ /* braceleft */                 0x7B,
  /* 124  */ /* bar */                       0x7C,
  /* 125  */ /* braceright */                0x7D,
  /* 126  */ /* asciitilde */                0x7E,
  /* 127  */ /* -unused- */                  0x7F,
  /* 128  */ /* -unused- [euro] */           0xD5,
  /* 129  */ /* -unused- */                  0x81,
  /* 130  */ /* quotesinglbase */            0x2C, // unsupported, comm
  /* 131  */ /* florin */                    0x9F,
  /* 132  */ /* quotedblbase */              0xFF, // unsupported
  /* 133  */ /* ellipsis */                  0xFF, // unsupported
  /* 134  */ /* dagger */                    0xFF, // unsupported
  /* 135  */ /* daggerdbl */                 0xFF, // unsupported
  /* 136  */ /* circumflex */                0xFF, // unsupported
  /* 137  */ /* perthousand */               0xFF, // unsupported
  /* 138  */ /* Scaron */                    0x53, // unsupported, S
  /* 139  */ /* guilsinglleft */             0x3C, // unsupported, less
  /* 140  */ /* OE */                        0xFF, // unsupported
  /* 141  */ /* -unused- */                  0x8D,
  /* 142  */ /* -unused- [zcaron] */         0x5A, // unsupported, Z
  /* 143  */ /* -unused- */                  0x8F,
  /* 144  */ /* -unused- */                  0x90,
  /* 145  */ /* quoteleft */                 0x60, // unsupported, grave
  /* 146  */ /* quoteright */                0x27, // unsupported, quotesingle
  /* 147  */ /* quotedblleft */              0x22, // unsupported, quotedbl
  /* 148  */ /* quotedblright */             0x22, // unsupported, quotedbl
  /* 149  */ /* bullet [bullet1] */          0x07,
  /* 150  */ /* endash */                    0x2D, // unsupported, hyphen
  /* 151  */ /* emdash */                    0x2D, // unsupported, hyphen
  /* 152  */ /* tilde */                     0x7E, // unsupported, asciitilde
  /* 153  */ /* trademark */                 0xFF, // unsupported
  /* 154  */ /* scaron */                    0x73, // unsupported, s
  /* 155  */ /* guilsinglright */            0x3E, // unsupported, greater
  /* 156  */ /* oe */                        0xFF, // unsupported
  /* 157  */ /* -unused- */                  0x9D, // unsupported
  /* 158  */ /* -unused- [zcaron] */         0x7A, // unsupported, z
  /* 159  */ /* Ydieresis */                 0x98, // unsupported, ydieresis

// 160-255 - same as in IsoLatin1

  /* 160  */ /* space [reqspace] */          0xFF,
  /* 161  */ /* exclamdown */                0xAD,
  /* 162  */ /* cent */                      0xBD,
  /* 163  */ /* sterling */                  0x9C,
  /* 164  */ /* currency */                  0xCF,
  /* 165  */ /* yen */                       0xBE,
  /* 166  */ /* brokenbar */                 0xDD,
  /* 167  */ /* section */                   0xF5,
  /* 168  */ /* dieresis */                  0xF9,
  /* 169  */ /* copyright */                 0xB8,
  /* 170  */ /* ordfeminine */               0xA6,
  /* 171  */ /* guillemotleft */             0xAE,
  /* 172  */ /* logicalnot */                0xAA,
  /* 173  */ /* hyphen [syllable] */         0xF0,
  /* 174  */ /* registered */                0xA9,
  /* 175  */ /* macron [overline] */         0xEE,
  /* 176  */ /* degree */                    0xF8,
  /* 177  */ /* plusminus */                 0xF1,
  /* 178  */ /* twosuperior */               0xFD,
  /* 179  */ /* threesuperior */             0xFC,
  /* 180  */ /* acute */                     0xEF,
  /* 181  */ /* mu */                        0xE6,
  /* 182  */ /* paragraph */                 0xF4,
  /* 183  */ /* periodcentered */            0xFA,
  /* 184  */ /* cedilla */                   0xF7,
  /* 185  */ /* onesuperior */               0xFB,
  /* 186  */ /* ordmasculine */              0xA7,
  /* 187  */ /* guillemotright */            0xAF,
  /* 188  */ /* onequarter */                0xAC,
  /* 189  */ /* onehalf */                   0xAB,
  /* 190  */ /* threequarters */             0xF3,
  /* 191  */ /* questiondown */              0xA8,
  /* 192  */ /* Agrave */                    0xB7,
  /* 193  */ /* Aacute */                    0xB5,
  /* 194  */ /* Acircumflex */               0xB6,
  /* 195  */ /* Atilde */                    0xC7,
  /* 196  */ /* Adieresis */                 0x8E,
  /* 197  */ /* Aring */                     0x8F,
  /* 198  */ /* AE */                        0x92,
  /* 199  */ /* Ccedilla */                  0x80,
  /* 200  */ /* Egrave */                    0xD4,
  /* 201  */ /* Eacute */                    0x90,
  /* 202  */ /* Ecircumflex */               0xD2,
  /* 203  */ /* Edieresis */                 0xD3,
  /* 204  */ /* Igrave */                    0xDE,
  /* 205  */ /* Iacute */                    0xD6,
  /* 206  */ /* Icircumflex */               0xD7,
  /* 207  */ /* Idieresis */                 0xD8,
  /* 208  */ /* Eth */                       0xD1,
  /* 209  */ /* Ntilde */                    0xA5,
  /* 210  */ /* Ograve */                    0xE3,
  /* 211  */ /* Oacute */                    0xE0,
  /* 212  */ /* Ocircumflex */               0xE2,
  /* 213  */ /* Otilde */                    0xE5,
  /* 214  */ /* Odieresis */                 0x99,
  /* 215  */ /* multiply */                  0x9E,
  /* 216  */ /* Oslash */                    0x9D,
  /* 217  */ /* Ugrave */                    0xEB,
  /* 218  */ /* Uacute */                    0xE9,
  /* 219  */ /* Ucircumflex */               0xEA,
  /* 220  */ /* Udieresis */                 0x9A,
  /* 221  */ /* Yacute */                    0xED,
  /* 222  */ /* Thorn */                     0xE8,
  /* 223  */ /* germandbls */                0xE1,
  /* 224  */ /* agrave */                    0x85,
  /* 225  */ /* aacute */                    0xA0,
  /* 226  */ /* acircumflex */               0x83,
  /* 227  */ /* atilde */                    0xC6,
  /* 228  */ /* adieresis */                 0x84,
  /* 229  */ /* aring */                     0x86,
  /* 230  */ /* ae */                        0x91,
  /* 231  */ /* ccedilla */                  0x87,
  /* 232  */ /* egrave */                    0x8A,
  /* 233  */ /* eacute */                    0x82,
  /* 234  */ /* ecircumflex */               0x88,
  /* 235  */ /* edieresis */                 0x89,
  /* 236  */ /* igrave */                    0x8D,
  /* 237  */ /* iacute */                    0xA1,
  /* 238  */ /* icircumflex */               0x8C,
  /* 239  */ /* idieresis */                 0x8B,
  /* 240  */ /* eth */                       0xD0,
  /* 241  */ /* ntilde */                    0xA4,
  /* 242  */ /* ograve */                    0x95,
  /* 243  */ /* oacute */                    0xA2,
  /* 244  */ /* ocircumflex */               0x93,
  /* 245  */ /* otilde */                    0xE4,
  /* 246  */ /* odieresis */                 0x94,
  /* 247  */ /* divide */                    0xF6,
  /* 248  */ /* oslash */                    0x9B,
  /* 249  */ /* ugrave */                    0x97,
  /* 250  */ /* uacute */                    0xA3,
  /* 251  */ /* ucircumflex */               0x96,
  /* 252  */ /* udieresis */                 0x81,
  /* 253  */ /* yacute */                    0xEC,
  /* 254  */ /* thorn */                     0xE7,
  /* 255  */ /* ydieresis */                 0x98
};


//
// ConvertCharset
//
// convert string charset
//

void ConvertCharset( char *str_in, int in_len , char *str_out, int *out_len, int iSrcCharset )
{
    int i,j;
    char *convtab = NULL;

    int fInHex; // flag, if we currently are processing hex string

    // these variables keep state in case we have to abort hex string conversion
    // (if it's actually not hex string, just <some string>...
    int iHexRollbackSrc;
    int iHexRollbackDest;
    int iHexDigits;

    char chHex;


    // validate params
    if( str_in ==  NULL || str_out == NULL )
        return;

    // here we process the src/dest charset requests
    // should create something more elaborate, but for now...

    if( iSrcCharset == CONVERT_ISOLATIN1 )
       convtab = FromIso;
    else if( iSrcCharset == CONVERT_WINDOWSANSI )
       convtab = FromWindows;
    else if( iSrcCharset == CONVERT_UTF8 )
       convtab = FromUTF8;
    else
    {
        // just copy over the data and pretend it's OK
        memcpy( str_out, str_in, in_len );
        *out_len = in_len;
        return;
    }

    //
    // convert the string (main loop)
    //

    i = 0;
    j = 0;
    fInHex = FALSE;

    while( i < in_len && j < *out_len )
    {

        //
        // note: very similar code is countained in p2putil.ch::CopyString
        // if you fix a     here, don't forget to fix it there too.
        //

        if( !fInHex && str_in[i]=='<' )
        {
            // this might be the beginning of <hex> string

            fInHex = TRUE;
            iHexRollbackSrc = i+1;
            iHexRollbackDest = j+1;
            iHexDigits = 0;
        }
        else if( fInHex && str_in[i]=='>' )
        {
            // hex string is terminated

            if( iHexDigits )
            {
                // this string is error, rollback
                i = iHexRollbackSrc;
                j = iHexRollbackDest;
                // just go on copying
            }

            fInHex = FALSE;
            iHexDigits = 0;
        }
        else if( fInHex )
        {
            // processing the hex string

            char ch = str_in[i];

            // check if it's acceptable hex char

            if( ch >= '0' && ch<='9' )
            {
                chHex = (chHex << 4) | (ch - '0');
                iHexDigits++;
            }
            else if( ch >= 'a' && ch<='f' )
            {
                chHex = (chHex << 4) | (ch - 'a' + 0xA);
                iHexDigits++;
            }
            else if( ch >= 'A' && ch<='F' )
            {
                chHex = (chHex << 4) | (ch - 'A' + 0xA);
                iHexDigits++;
            }
            else if( ch == ' ' || ch == '\t' )
            {
                // these are acceptable chars
                // just copy them over verbatim
                // (note: this is different from p2putil.ch processing!)
                str_out[j]=convtab[ str_in[i] ];
                i++;
                j++;
                continue;
            }
            else
            {
                // illegal character encountered!
                // just bail, return to copying the string as if hex mode never happened
                i = iHexRollbackSrc;
                j = iHexRollbackDest;
                fInHex = FALSE;
                iHexDigits = 0;
                continue;
            }

            // must be 2 or more(even nr) hex characters
            if( iHexDigits == 2 )
            {
                char tch;
                // convert and output the char
                ch = convtab[ chHex ];

                // convert this to hex and output
                tch = (ch & 0xF0) >> 4;
                if(tch > 9) tch += 'A' - 0xA; else tch+= '0';

                str_out[j] = tch;
                j++;


                tch = (ch & 0xF);
                if(tch > 9) tch += 'A' - 0xA; else tch+= '0';
                str_out[j] = tch;
                j++;

                iHexDigits = 0;
            }

            i++;
            continue;
        }

        // copy current character over to destination string
        // (this is done almost everytime, after processing code is executed)
        str_out[j]=convtab[ str_in[i] ];
        i++;
        j++;
    }

    *out_len = j;

    // and we're done...
    return;
}

//
// reading file to memory
//
// text line structure
//

typedef struct LIST_LINE_
{
    char  *pStr;
    int   iStrSize;
    struct LIST_LINE_ *pNext;
} LIST_LINE, *PLIST_LINE;

PLIST_LINE list_head = NULL;
PLIST_LINE list_tail = NULL;

char *PpdBuf = NULL;
int  biggest_line_size;

//
// read in file and parse lines into list
//
int ReadFileToList( FILE *in )
{
    char *buf = NULL;
    int buf_size = 0;
    int line_length = 0;
    int line_begin_offset = 0;

    int i = 0, j = 0;
    int fForever = TRUE;

    biggest_line_size = 0;

    PpdBuf = NULL;

    // read everything
    fseek( in, 0L, SEEK_END);
    buf_size = ftell( in );
    fseek( in, 0L, SEEK_SET);

    buf = malloc ( buf_size + 4 ); // 4 kb
    if( buf == NULL )
    {
        printf("  out of memory reading input file (asked for %d bytes)\n", buf_size );
        return FALSE; // fail
    }

    if( !fread(buf, buf_size, 1, in) )
    {
        printf("  error reading input file\n" );
        free( buf );
        return FALSE; // fail
    }

    line_length = 0;
    line_begin_offset = 0;

    while( i < buf_size )
    {
        while( buf[i] != '\n' && i < buf_size ) i++;

        if( buf[i] == '\n' || i == buf_size ) // found the sacred end of line
        {
            PLIST_LINE node;
            // put the line in list!
            // i is length of line

            if( i < buf_size )
                i++; // include the \n

            line_length = i - line_begin_offset;

            if( line_length > biggest_line_size ) biggest_line_size = line_length; // find the biggest buffer we will need

            node = malloc( sizeof( LIST_LINE ) );

            node->iStrSize = line_length;
            node->pStr = &buf[ line_begin_offset ];

//debug
//            node->pStr[ node->iStrSize ] = 0;
//            printf( "%s", node->pStr );
//debug

            node->pNext = NULL;

            if( list_tail != NULL )
                list_tail->pNext = node;

            list_tail = node;
            if( list_head == NULL ) list_head = node;

            // reset to fill buffer
            line_length = 0;
            line_begin_offset = i;

        }
    }


    PpdBuf = buf;

    return TRUE;
}

//
// Go through lines to find LanguageEncoding of PPD
// Also change LanguageEncoding to OS2-850, so that when PPD is converted
// we can see it.
//
int GetCharset( void )
{
    PLIST_LINE node = NULL;
    int iCharset = CONVERT_UNKNOWN;
    char key[19] = "*LanguageEncoding:";
    char word[19] = "";
    int  enc_offset;
    char os2cp_str[] = " OS2-850\r\n";

    for( node = list_head ; node != NULL ; node = node->pNext )
    {
        // check if string
        int i = 0, j = 0;
        char *str = node->pStr;

        while( (str[i] == ' ' || str[i] == '\t') && i < node->iStrSize ) i++;
        if( i == node->iStrSize ) continue; // next

        while(  i < node->iStrSize && j < 18 )
        {
            word[j] = str[i];
            i++;
            j++;
        }
        word[j] = 0;

        if( i == node->iStrSize ) continue; // next

        enc_offset = i;

        if(!stricmp( key, word ))
        {
            // gee, found just the string!

            // read the keyword again
            while( (str[i] == ' ' || str[i] == '\t' || str[i]=='\"') && i < node->iStrSize ) i++;
            if( i == node->iStrSize ) continue; // next

            j = 0;
            while(  i < node->iStrSize && j < 18 && str[i]!='\"' && str[i]!=' ' && str[i]!='\t' && str[i]!='\r' && str[i]!='\n' )
            {
                word[j] = str[i];
                i++;
                j++;
            }
            word[j] = 0;

            if( !stricmp( word, "IsoLatin1") )
            {
                iCharset = CONVERT_ISOLATIN1;
                printf( "(IsoLatin1)" );

                strcpy( &str[ enc_offset ], os2cp_str );
                node->iStrSize = enc_offset + strlen( os2cp_str );
                break;
            }
            else if( !stricmp( word, "WindowsANSI") )
            {
                iCharset = CONVERT_WINDOWSANSI;
                printf( "(WindowsAnsi)" );

                strcpy( &str[ enc_offset ], os2cp_str );
                node->iStrSize = enc_offset + strlen( os2cp_str ) - 1;
                break;
            }
            else if( !stricmp( word, "UTF-8") )
            {
                iCharset = CONVERT_UTF8;
                printf( "(UTF-8)" );

                strcpy( &str[ enc_offset ], os2cp_str );
                node->iStrSize = enc_offset + strlen( os2cp_str ) - 1;
                break;
            }
            else if( !stricmp( word, os2cp_str ) )
            {
                printf( "(%s already)", os2cp_str );
                break;
            }
            else
            {
                printf( "(%s: unsupported)", word );
                break;
            }
        }
    }

    if( iCharset == CONVERT_UNKNOWN ) printf( "(copying only)" );

    return iCharset;
}


// debug: dump statistics
int DumpList( void )
{
    PLIST_LINE node = NULL;
    int line_number  = 0;
    int byte_count = 0;

    node = list_head;

    while( node )
    {
//        printf( "line %04d: %s (%d)\n",line_number,node->pStr,node->iStrSize );

        line_number++;
        byte_count+=node->iStrSize;

        node = node->pNext;
    }

    printf( "read %d lines, %d bytes\n",line_number, byte_count );

    return TRUE;
}


// clean list, free all memory, delete all nodes

int FreeList( void )
{
    PLIST_LINE node1 = NULL;
    PLIST_LINE node2 = NULL;

    node1 = list_head;

    while( node1 )
    {
        node2 = node1->pNext;

        free( node1 );

        node1 = node2;
    }

    list_head = NULL;
    list_tail = NULL;

    if( PpdBuf )
    {
        free( PpdBuf );
        PpdBuf = NULL;
    }
    biggest_line_size = 0;

    return TRUE;
}

//
// ConvertFile
//
// convert charset in source file, create destination file
// reads in file line-by-line, converts charset, output the lines
//

int ConvertFile( char *file_in, char *file_out )
{
    FILE *in = NULL;
    FILE *out = NULL;
    int iCharset = CONVERT_UNKNOWN;
    PLIST_LINE node = NULL;
    char *buf;

    // validate params
    if( file_in == NULL || file_out == NULL )
        return FALSE;

    in = fopen( file_in,"rb" );
    if( in == NULL )
    {
        printf("  could not open input file '%s' for reading\n", file_in );
        return FALSE;
    }

    // read in the whole file
    if( !ReadFileToList( in ) )
    {
        printf("  %s: error reading file\n", file_in );

        FreeList();
        return FALSE;
    }

    fclose( in );

    // find out it's encoding
    iCharset = GetCharset();

    buf = malloc( biggest_line_size * 2 );  // yeah, paranoia...
    if( buf == NULL )
    {
         printf("  out of memory writing output file\n" );
         FreeList();
         return FALSE;
    }

//debug
//  DumpList();

    out = fopen( file_out,"wb" );
    if( out == NULL )
    {
        printf("  could not open output file '%s' for writing\n", file_in );

        free( buf );
        FreeList();
        return FALSE;
    }

    node = list_head;
    while(node)
    {
        int out_len = biggest_line_size * 2; // yeah, paranoia...

        ConvertCharset( node->pStr, node->iStrSize, buf, &out_len, iCharset);

        fwrite(buf, out_len, 1, out);

        node = node->pNext;
    }

    free( buf );
    FreeList();

    fclose(out);

    // success
    return TRUE;
}

void print_usage( void )
{
    printf( "Usage:\n"
    "  * to convert one file:\n"
    "    ppdenc <input_ppd_file> <output_ppd_file>\n"
    "  * do convert all *.PPD files in directory\n"
    "    ppdenc -d <source_directory> <dest_directory>\n"
    "  (destination directory must exist!)\n\n"
    "Example:\n"
    "  * to convert all files in directory c:\\src_ppd run:\n"
    "    ppdenc -d c:\\src_ppd c:\\dest_ppd\n"
    "    (diretory c:\\dest_ppd must exist)\n"
    );
}

int main(int argc, char *argv[])
{
    char SOURCE_PATH[CCHMAXPATH];
    char DEST_PATH[CCHMAXPATH];
    char *pszInFile;
    char szInFilePath[CCHMAXPATH];
    char szOutFilePath[CCHMAXPATH];
    int  iCharset;

    printf("PPD encoding conversion utility version 1.00\n");
    printf("Copyright (c) IBM Corp. 2001. All rights reserved.\n");


    // check that arg count is right
    if( argc<3 || argc>4 )
    {
        print_usage();
        return 1;
    }

    // check what exactly we should convert
    if( argc == 4 && !strcmp( argv[1], "-d" ) )
    {
        // user is asking us to convert directory full of PPD files

        copy_pathname( SOURCE_PATH, argv[2] );
        copy_pathname( DEST_PATH, argv[3] );

        printf("  searching PPD files in directory %s\n", SOURCE_PATH );
        FilenameList_ReadFromDirectory( SOURCE_PATH, "*.ppd" );

        pszInFile = FilenameList_GetName(1);
        while(pszInFile)
        {
            strcpy( szInFilePath, SOURCE_PATH );
            strcat( szInFilePath, pszInFile );

            strcpy( szOutFilePath, DEST_PATH );
            strcat( szOutFilePath, pszInFile );

            printf("  converting file %s: ", pszInFile );

            if( !ConvertFile( szInFilePath, szOutFilePath ) )
               printf("   FAILED\n");
            else
               printf(" OK\n");

            pszInFile = FilenameList_GetName(0);
       }

    }
    else if( argc == 3 )
    {
        // user is asking us to convert only one file

        strcpy( szInFilePath, argv[1] );
        strcpy( szOutFilePath, argv[2] );

        printf("  converting file %s: ", szInFilePath );

        if( !ConvertFile( szInFilePath, szOutFilePath ) )
           printf("   FAILED\n");
        else
           printf(" OK\n");
    }
    else
    {
        print_usage();
        return 1;
    }

    printf( "done\n" );


    return 0; // success
}
