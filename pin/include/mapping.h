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
 * SOURCE FILE NAME = MAPPING.H
 *
 * DESCRIPTIVE NAME =
 *
 *
 * VERSION = V5.0
 *
 * DATE        : 10 August 1998
 *
 * DESCRIPTION : Mapping between Adobe character names, Presentation Manager
 *               multi-codepage support code point values, and Adobe
 *               numeric values. Note that many different code points
 *               may map to the same Adobe character name.
 *
 * FUNCTIONS
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
*/

#pragma pack(1)
#ifndef NULL
#define NULL           0
#endif



/*
** Character code mapping table for use with fonts that have the
** Adobe standard encoding.  These fonts will provide Winthorn
** multi-codepage support.
**
**                       IBM Code      Code to output to Postscript
**                       page 850      printer in a show command. If
**                       word that     zero, then a font remap is needed
**                       this char     in order to show this character.
**                       represents    After font remap, output the char
**                         1-316       code used by the caller.
**                           |            |
**                           |            |
*/

#if AFM2BIN
CharData charmapAdobeStandard[] = {
      "space",             0 ,         32 ,
      "space",             1 ,         32 ,
      "space",             2 ,         32 ,
      "space",             3 ,         32 ,
      "space",             4 ,         32 ,
      "space",             5 ,         32 ,
      "space",             6 ,         32 ,
      "bullet",            7 ,         183,
      "space",             8 ,         32 ,
      "space",             9 ,         32 ,
      "space",            10 ,         32 ,
      "space",            11 ,         32 ,
      "space",            12 ,         32 ,
      "space",            13 ,         32 ,
      "space",            14 ,         32 ,
      "space",            15 ,         32 ,
      "space",            16 ,         32 ,
      "space",            17 ,         32 ,
      "space",            18 ,         32 ,
      "space",            19 ,         32 ,
      "paragraph",        20 ,         182,
      "section",          21 ,         167,
      "space",            22 ,         32 ,
      "space",            23 ,         32 ,
      "space",            24 ,         32 ,
      "space",            25 ,         32 ,
      "space",            26 ,         32 ,
      "space",            27 ,         32 ,
      "space",            28 ,         32 ,
      "space",            29 ,         32 ,
      "space",            30 ,         32 ,
      "space",            31 ,         32 ,
      "space",            32 ,         32 ,
      "exclam",           33 ,         33 ,
      "quotedbl",         34 ,         34 ,
      "numbersign",       35 ,         35 ,
      "dollar",           36 ,         36 ,
      "percent",          37 ,         37 ,
      "ampersand",        38 ,         38 ,
      "quotesingle",      39 ,         169,
      "parenleft",        40 ,         40 ,
      "parenright",       41 ,         41 ,
      "asterisk",         42 ,         42 ,
      "plus",             43 ,         43 ,
      "comma",            44 ,         44 ,
      "hyphen",           45 ,         45 ,
      "period",           46 ,         46 ,
      "slash",            47 ,         47 ,
      "zero",             48 ,         48 ,
      "one",              49 ,         49 ,
      "two",              50 ,         50 ,
      "three",            51 ,         51 ,
      "four",             52 ,         52 ,
      "five",             53 ,         53 ,
      "six",              54 ,         54 ,
      "seven",            55 ,         55 ,
      "eight",            56 ,         56 ,
      "nine",             57 ,         57 ,
      "colon",            58 ,         58 ,
      "semicolon",        59 ,         59 ,
      "less",             60 ,         60 ,
      "equal",            61 ,         61 ,
      "greater",          62 ,         62 ,
      "question",         63 ,         63 ,
      "at",               64 ,         64 ,
      "A",                65 ,         65 ,
      "B",                66 ,         66 ,
      "C",                67 ,         67 ,
      "D",                68 ,         68 ,
      "E",                69 ,         69 ,
      "F",                70 ,         70 ,
      "G",                71 ,         71 ,
      "H",                72 ,         72 ,
      "I",                73 ,         73 ,
      "J",                74 ,         74 ,
      "K",                75 ,         75 ,
      "L",                76 ,         76 ,
      "M",                77 ,         77 ,
      "N",                78 ,         78 ,
      "O",                79 ,         79 ,
      "P",                80 ,         80 ,
      "Q",                81 ,         81 ,
      "R",                82 ,         82 ,
      "S",                83 ,         83 ,
      "T",                84 ,         84 ,
      "U",                85 ,         85 ,
      "V",                86 ,         86 ,
      "W",                87 ,         87 ,
      "X",                88 ,         88 ,
      "Y",                89 ,         89 ,
      "Z",                90 ,         90 ,
      "bracketleft",      91 ,         91 ,
      "backslash",        92 ,         92 ,
      "bracketright",     93 ,         93 ,
      "circumflex",       94 ,         195,
      "underscore",       95 ,         95 ,
      "grave",            96 ,         193,
      "a",                97 ,         97 ,
      "b",                98 ,         98 ,
      "c",                99 ,         99 ,
      "d",                100,         100,
      "e",                101,         101,
      "f",                102,         102,
      "g",                103,         103,
      "h",                104,         104,
      "i",                105,         105,
      "j",                106,         106,
      "k",                107,         107,
      "l",                108,         108,
      "m",                109,         109,
      "n",                110,         110,
      "o",                111,         111,
      "p",                112,         112,
      "q",                113,         113,
      "r",                114,         114,
      "s",                115,         115,
      "t",                116,         116,
      "u",                117,         117,
      "v",                118,         118,
      "w",                119,         119,
      "x",                120,         120,
      "y",                121,         121,
      "z",                122,         122,
      "braceleft",        123,         123,
      "bar",              124,         124,
      "braceright",       125,         125,
      "tilde",            126,         196,
      "space",            127,         32 ,
      "Ccedilla",         128,         000,
      "udieresis",        129,         000,
      "eacute",           130,         000,
      "acircumflex",      131,         000,
      "adieresis",        132,         000,
      "agrave",           133,         000,
      "aring",            134,         000,
      "ccedilla",         135,         000,
      "ecircumflex",      136,         000,
      "edieresis",        137,         000,
      "egrave",           138,         000,
      "idieresis",        139,         000,
      "icircumflex",      140,         000,
      "igrave",           141,         000,
      "Adieresis",        142,         000,
      "Aring",            143,         000,
      "Eacute",           144,         000,
      "ae",               145,         241,
      "AE",               146,         225,
      "ocircumflex",      147,         000,
      "odieresis",        148,         000,
      "ograve",           149,         000,
      "ucircumflex",      150,         000,
      "ugrave",           151,         000,
      "ydieresis",        152,         000,
      "Odieresis",        153,         000,
      "Udieresis",        154,         000,
      "oslash",           155,         249,
      "sterling",         156,         163,
      "Oslash",           157,         233,
      "multiply",         158,         0  ,
      "florin",           159,         166,
      "aacute",           160,         000,
      "iacute",           161,         000,
      "oacute",           162,         000,
      "uacute",           163,         000,
      "ntilde",           164,         000,
      "Ntilde",           165,         000,
      "ordfeminine",      166,         227,
      "ordmasculine",     167,         235,
      "questiondown",     168,         191,
      "registered",       169,         0  ,
      "logicalnot",       170,         0  ,
      "onehalf",          171,         0  ,
      "onequarter",       172,         0  ,
      "exclamdown",       173,         161,
      "guillemotleft",    174,         171,
      "guillemotright",   175,         187,
      "space",            176,         32 ,
      "space",            177,         32 ,
      "space",            178,         32 ,
      "space",            179,         32 ,
      "space",            180,         32 ,
      "Aacute",           181,         000,
      "Acircumflex",      182,         000,
      "Agrave",           183,         000,
      "copyright",        184,         00 ,
      "space",            185,         32 ,
      "space",            186,         32 ,
      "space",            187,         32 ,
      "space",            188,         32 ,
      "cent",             189,         162,
      "yen",              190,         165,
      "space",            191,         32 ,
      "space",            192,         32 ,
      "space",            193,         32 ,
      "space",            194,         32 ,
      "space",            195,         32 ,
      "space",            196,         32 ,
      "space",            197,         32 ,
      "atilde",           198,         000,
      "Atilde",           199,         000,
      "space",            200,         32 ,
      "space",            201,         32 ,
      "space",            202,         32 ,
      "space",            203,         32 ,
      "space",            204,         32 ,
      "space",            205,         32 ,
      "space",            206,         32 ,
      "currency",         207,         168,
      "eth",              208,         00 ,
      "Eth",              209,         00 ,
      "Ecircumflex",      210,         000,
      "Edieresis",        211,         000,
      "Egrave",           212,         000,
      "dotlessi",         213,         245,
      "Iacute",           214,         000,
      "Icircumflex",      215,         000,
      "Idieresis",        216,         000,
      "space",            217,         32 ,
      "space",            218,         32 ,
      "space",            219,         32 ,
      "space",            220,         32 ,
      "brokenbar",        221,         00 ,
      "Igrave",           222,         000,
      "space",            223,         32 ,
      "Oacute",           224,         000,
      "germandbls",       225,         251,
      "Ocircumflex",      226,         000,
      "Ograve",           227,         000,
      "otilde",           228,         000,
      "Otilde",           229,         000,
      "mu",               230,         000,
      "thorn",            231,         000,
      "Thorn",            232,         000,
      "Uacute",           233,         000,
      "Ucircumflex",      234,         000,
      "Ugrave",           235,         000,
      "yacute",           236,         000,
      "Yacute",           237,         000,
      "macron",           238,         197,
      "acute",            239,         194,
      "hyphen",           240,         45 ,
      "plusminus",        241,         00 ,
      "space",            242,         32 ,
      "threequarters",    243,         00 ,
      "paragraph",        244,         182,
      "section",          245,         167,
      "divide",           246,         00 ,
      "cedilla",          247,         203,
      "ring",             248,         202,
      "dieresis",         249,         200,
      "periodcentered",   250,         180,
      "onesuperior",      251,         00 ,
      "threesuperior",    252,         00 ,
      "twosuperior",      253,         00 ,
      "space",            254,         32 ,
      "space",            255,         00 ,
      "space",            256,         32 ,
      "space",            257,         32 ,
      "space",            258,         32 ,
      "space",            259,         32 ,
      "space",            260,         32 ,
      "space",            261,         32 ,
      "space",            262,         32 ,
      "space",            263,         32 ,
      "space",            264,         32 ,
      "space",            265,         32 ,
      "space",            266,         32 ,
      "space",            267,         32 ,
      "space",            268,         32 ,
      "space",            269,         32 ,
      "space",            270,         32 ,
      "space",            271,         32 ,
      "space",            272,         32 ,
      "space",            273,         32 ,
      "space",            274,         32 ,
      "space",            275,         32 ,
      "space",            276,         32 ,
      "space",            277,         32 ,
      "space",            278,         32 ,
      "space",            279,         32 ,
      "space",            280,         32 ,
      "space",            281,         32 ,
      "space",            282,         32 ,
      "space",            283,         32 ,
      "space",            284,         32 ,
      "space",            285,         32 ,
      "space",            286,         32 ,
      "space",            287,         32 ,
      "space",            288,         32 ,
      "space",            289,         32 ,
      "space",            290,         32 ,
      "space",            291,         32 ,
      "space",            292,         32 ,
      "space",            293,         32 ,
      "space",            294,         32 ,
      "space",            295,         32 ,
      "space",            296,         32 ,
      "space",            297,         32 ,
      "bullet",           298,         183,
      "space",            299,         32 ,
      "space",            300,         32 ,
      "macron",           301,         197,
      "breve",            302,         198,
      "dotaccent",        303,         199,
      "ring",             304,         202,
      "hungarumlaut",     305,         205,
      "ogonek",           306,         206,
      "caron",            307,         207,
      "quoteleft",        308,         96 ,
      "quoteright",       309,         39 ,
      "quotedblleft",     310,         170,
      "quotedblright",    311,         186,
      "endash",           312,         177,
      "emdash",           313,         208,
      "asciicircum",      314,         94 ,
      "asciitilde",       315,         126,
      "quotesinglbase",   316,         000,
      "quotedblbase",     317,         000,
      "ellipsis",         318,         000,
      "dagger",           319,         000,
      "daggerdbl",        320,         000,
      "circumflex",       321,         000,
      "perthousand",      322,         000,
      "Scaron",           323,         000,
      "guilsinglleft",    324,         000,
      "OE",               325,         000,
      "tilde",            326,         000,
      "trademark",        327,         000,
      "scaron",           328,         000,
      "guilsinglright",   329,         000,
      "oe",               330,         000,
      "Ydierisis",        331,         000,
      NULL,               000,         000
} ;


/*
** Character code mapping table for use with fonts that have the
** Adobe font-specific encoding.  These fonts will not provide
** Winthorn multi-codepage support.
**
**                                     Code to output to Postscript
**                       Code point    printer in a show command. With
**                       application   font-specific encoding, this code
**                       provides to   is always the same as the code
**                       engine        point provided by the app, since
**                         1-255       the engine provides no remapping.
**                           |            |
**                           |            |
*/
CharData charmapFontSpecific[] = {
      "space",            32 ,         32 ,
      "exclam",           33 ,         33 ,
      "universal",        34 ,         34 ,
      "numbersign",       35 ,         35 ,
      "existential",      36 ,         36 ,
      "percent",          37 ,         37 ,
      "ampersand",        38 ,         38 ,
      "suchthat",         39 ,         39 ,
      "parenleft",        40 ,         40 ,
      "parenright",       41 ,         41 ,
      "asteriskmath",     42 ,         42 ,
      "plus",             43 ,         43 ,
      "comma",            44 ,         44 ,
      "minus",            45 ,         45 ,
      "period",           46 ,         46 ,
      "slash",            47 ,         47 ,
      "zero",             48 ,         48 ,
      "one",              49 ,         49 ,
      "two",              50 ,         50 ,
      "three",            51 ,         51 ,
      "four",             52 ,         52 ,
      "five",             53 ,         53 ,
      "six",              54 ,         54 ,
      "seven",            55 ,         55 ,
      "eight",            56 ,         56 ,
      "nine",             57 ,         57 ,
      "colon",            58 ,         58 ,
      "semicolon",        59 ,         59 ,
      "less",             60 ,         60 ,
      "equal",            61 ,         61 ,
      "greater",          62 ,         62 ,
      "question",         63 ,         63 ,
      "congruent",        64 ,         64 ,
      "Alpha",            65 ,         65 ,
      "Beta",             66 ,         66 ,
      "Chi",              67 ,         67 ,
      "Delta",            68 ,         68 ,
      "Epsilon",          69 ,         69 ,
      "Phi",              70 ,         70 ,
      "Gamma",            71 ,         71 ,
      "Eta",              72 ,         72 ,
      "Iota",             73 ,         73 ,
      "theta1",           74 ,         74 ,
      "Kappa",            75 ,         75 ,
      "Lambda",           76 ,         76 ,
      "Mu",               77 ,         77 ,
      "Nu",               78 ,         78 ,
      "Omicron",          79 ,         79 ,
      "Pi",               80 ,         80 ,
      "Theta",            81 ,         81 ,
      "Rho",              82 ,         82 ,
      "Sigma",            83 ,         83 ,
      "Tau",              84 ,         84 ,
      "Upsilon",          85 ,         85 ,
      "sigma1",           86 ,         86 ,
      "Omega",            87 ,         87 ,
      "Xi",               88 ,         88 ,
      "Psi",              89 ,         89 ,
      "Zeta",             90 ,         90 ,
      "bracketleft",      91 ,         91 ,
      "therefore",        92 ,         92 ,
      "bracketright",     93 ,         93 ,
      "perpendicular",    94 ,         94 ,
      "underscore",       95 ,         95 ,
      "radicalex",        96 ,         96 ,
      "alpha",            97 ,         97 ,
      "beta",             98 ,         98 ,
      "chi",              99 ,         99 ,
      "delta",           100,          100,
      "epsilon",         101,          101,
      "phi",             102,          102,
      "gamma",           103,          103,
      "eta",             104,          104,
      "iota",            105,          105,
      "phi1",            106,          106,
      "kappa",           107,          107,
      "lambda",          108,          108,
      "mu",              109,          109,
      "nu",              110,          110,
      "omicron",         111,          111,
      "pi",              112,          112,
      "theta",           113,          113,
      "rho",             114,          114,
      "sigma",           115,          115,
      "tau",             116,          116,
      "upsilon",         117,          117,
      "omega1",          118,          118,
      "omega",           119,          119,
      "xi",              120,          120,
      "psi",             121,          121,
      "zeta",            122,          122,
      "braceleft",       123,          123,
      "bar",             124,          124,
      "braceright",      125,          125,
      "similar",         126,          126,
      "Upsilon1",        161,          161,
      "minute",          162,          162,
      "lessequal",       163,          163,
      "fraction",        164,          164,
      "infinity",        165,          165,
      "florin",          166,          166,
      "club",            167,          167,
      "diamond",         168,          168,
      "heart",           169,          169,
      "spade",           170,          170,
      "arrowboth",       171,          171,
      "arrowleft",       172,          172,
      "arrowup",         173,          173,
      "arrowright",      174,          174,
      "arrowdown",       175,          175,
      "degree",          176,          176,
      "plusminus",       177,          177,
      "second",          178,          178,
      "greaterequal",    179,          179,
      "multiply",        180,          180,
      "proportional",    181,          181,
      "partialdiff",     182,          182,
      "bullet",          183,          183,
      "divide",          184,          184,
      "notequal",        185,          185,
      "equivalence",     186,          186,
      "approxequal",     187,          187,
      "ellipsis",        188,          188,
      "arrowvertex",     189,          189,
      "arrowhorizex",    190,          190,
      "carriagereturn",  191,          191,
      "aleph",           192,          192,
      "Ifraktur",        193,          193,
      "Rfraktur",        194,          194,
      "weierstrass",     195,          195,
      "circlemultiply",  196,          196,
      "circleplus",      197,          197,
      "emptyset",        198,          198,
      "intersection",    199,          199,
      "union",           200,          200,
      "propersuperset",  201,          201,
      "reflexsuperset",  202,          202,
      "notsubset",       203,          203,
      "propersubset",    204,          204,
      "reflexsubset",    205,          205,
      "element",         206,          206,
      "notelement",      207,          207,
      "angle",           208,          208,
      "gradient",        209,          209,
      "registerserif",   210,          210,
      "copyrightserif",  211,          211,
      "trademarkserif",  212,          212,
      "product",         213,          213,
      "radical",         214,          214,
      "dotmath",         215,          215,
      "logicalnot",      216,          216,
      "logicaland",      217,          217,
      "logicalor",       218,          218,
      "arrowdblboth",    219,          219,
      "arrowdblleft",    220,          220,
      "arrowdblup",      221,          221,
      "arrowdblright",   222,          222,
      "arrowdbldown",    223,          223,
      "lozenge",         224,          224,
      "angleleft",       225,          225,
      "registersans",    226,          226,
      "copyrightsans",   227,          227,
      "trademarksans",   228,          228,
      "summation",       229,          229,
      "parenlefttp",     230,          230,
      "parenleftex",     231,          231,
      "parenleftbt",     232,          232,
      "bracketlefttp",   233,          233,
      "bracketleftex",   234,          234,
      "bracketleftbt",   235,          235,
      "bracelefttp",     236,          236,
      "braceleftmid",    237,          237,
      "braceleftbt",     238,          238,
      "braceex",         239,          239,
      "angleright",      241,          241,
      "integral",        242,          242,
      "integraltp",      243,          243,
      "integralex",      244,          244,
      "integralbt",      245,          245,
      "parenrighttp",    246,          246,
      "parenrightex",    247,          247,
      "parenrightbt",    248,          248,
      "bracketrighttp",  249,          249,
      "bracketrightex",  250,          250,
      "bracketrightbt",  251,          251,
      "bracerighttp",    252,          252,
      "bracerightmid",   253,          253,
      "bracerightbt",    254,          254,
      "apple",           000,          000,
      "a1",               33,          33 ,
      "a2",               34,          34 ,
      "a202",             35,          35 ,
      "a3",               36,          36 ,
      "a4",               37,          37 ,
      "a5",               38,          38 ,
      "a119",             39,          39 ,
      "a118",             40,          40 ,
      "a117",             41,          41 ,
      "a11",              42,          42 ,
      "a12",              43,          43 ,
      "a13",              44,          44 ,
      "a14",              45,          45 ,
      "a15",              46,          46 ,
      "a16",              47,          47 ,
      "a105",             48,          48 ,
      "a17",              49,          49 ,
      "a18",              50,          50 ,
      "a19",              51,          51 ,
      "a20",              52,          52 ,
      "a21",              53,          53 ,
      "a22",              54,          54 ,
      "a23",              55,          55 ,
      "a24",              56,          56 ,
      "a25",              57,          57 ,
      "a26",              58,          58 ,
      "a27",              59,          59 ,
      "a28",              60,          60 ,
      "a6",               61,          61 ,
      "a7",               62,          62 ,
      "a8",               63,          63 ,
      "a9",               64,          64 ,
      "a10",              65,          65 ,
      "a29",              66,          66 ,
      "a30",              67,          67 ,
      "a31",              68,          68 ,
      "a32",              69,          69 ,
      "a33",              70,          70 ,
      "a34",              71,          71 ,
      "a35",              72,          72 ,
      "a36",              73,          73 ,
      "a37",              74,          74 ,
      "a38",              75,          75 ,
      "a39",              76,          76 ,
      "a40",              77,          77 ,
      "a41",              78,          78 ,
      "a42",              79,          79 ,
      "a43",              80,          80 ,
      "a44",              81,          81 ,
      "a45",              82,          82 ,
      "a46",              83,          83 ,
      "a47",              84,          84 ,
      "a48",              85,          85 ,
      "a49",              86,          86 ,
      "a50",              87,          87 ,
      "a51",              88,          88 ,
      "a52",              89,          89 ,
      "a53",              90,          90 ,
      "a54",              91,          91 ,
      "a55",              92,          92 ,
      "a56",              93,          93 ,
      "a57",              94,          94 ,
      "a58",              95,          95 ,
      "a59",              96,          96 ,
      "a60",              97,          97 ,
      "a61",              98,          98 ,
      "a62",              99,          99 ,
      "a63",             100,          100,
      "a64",             101,          101,
      "a65",             102,          102,
      "a66",             103,          103,
      "a67",             104,          104,
      "a68",             105,          105,
      "a69",             106,          106,
      "a70",             107,          107,
      "a71",             108,          108,
      "a72",             109,          109,
      "a73",             110,          110,
      "a74",             111,          111,
      "a203",            112,          112,
      "a75",             113,          113,
      "a204",            114,          114,
      "a76",             115,          115,
      "a77",             116,          116,
      "a78",             117,          117,
      "a79",             118,          118,
      "a81",             119,          119,
      "a82",             120,          120,
      "a83",             121,          121,
      "a84",             122,          122,
      "a97",             123,          123,
      "a98",             124,          124,
      "a99",             125,          125,
      "a100",            126,          126,
      "a101",            161,          161,
      "a102",            162,          162,
      "a103",            163,          163,
      "a104",            164,          164,
      "a106",            165,          165,
      "a107",            166,          166,
      "a108",            167,          167,
      "a112",            168,          168,
      "a111",            169,          169,
      "a110",            170,          170,
      "a109",            171,          171,
      "a120",            172,          172,
      "a121",            173,          173,
      "a122",            174,          174,
      "a123",            175,          175,
      "a124",            176,          176,
      "a125",            177,          177,
      "a126",            178,          178,
      "a127",            179,          179,
      "a128",            180,          180,
      "a129",            181,          181,
      "a130",            182,          182,
      "a131",            183,          183,
      "a132",            184,          184,
      "a133",            185,          185,
      "a134",            186,          186,
      "a135",            187,          187,
      "a136",            188,          188,
      "a137",            189,          189,
      "a138",            190,          190,
      "a139",            191,          191,
      "a140",            192,          192,
      "a141",            193,          193,
      "a142",            194,          194,
      "a143",            195,          195,
      "a144",            196,          196,
      "a145",            197,          197,
      "a146",            198,          198,
      "a147",            199,          199,
      "a148",            200,          200,
      "a149",            201,          201,
      "a150",            202,          202,
      "a151",            203,          203,
      "a152",            204,          204,
      "a153",            205,          205,
      "a154",            206,          206,
      "a155",            207,          207,
      "a156",            208,          208,
      "a157",            209,          209,
      "a158",            210,          210,
      "a159",            211,          211,
      "a160",            212,          212,
      "a161",            213,          213,
      "a163",            214,          214,
      "a164",            215,          215,
      "a196",            216,          216,
      "a165",            217,          217,
      "a192",            218,          218,
      "a166",            219,          219,
      "a167",            220,          220,
      "a168",            221,          221,
      "a169",            222,          222,
      "a170",            223,          223,
      "a171",            224,          224,
      "a172",            225,          225,
      "a173",            226,          226,
      "a162",            227,          227,
      "a174",            228,          228,
      "a175",            229,          229,
      "a176",            230,          230,
      "a177",            231,          231,
      "a178",            232,          232,
      "a179",            233,          233,
      "a193",            234,          234,
      "a180",            235,          235,
      "a199",            236,          236,
      "a181",            237,          237,
      "a200",            238,          238,
      "a182",            239,          239,
      "a201",            241,          241,
      "a183",            242,          242,
      "a184",            243,          243,
      "a197",            244,          244,
      "a185",            245,          245,
      "a194",            246,          246,
      "a198",            247,          247,
      "a186",            248,          248,
      "a195",            249,          249,
      "a187",            250,          250,
      "a188",            251,          251,
      "a189",            252,          252,
      "a190",            253,          253,
      "a191",            254,          254,
      "a205",            000,          000,
      "a206",            000,          000,
      "a85",             000,          000,
      "a86",             000,          000,
      "a87",             000,          000,
      "a88",             000,          000,
      "a89",             000,          000,
      "a90",             000,          000,
      "a91",             000,          000,
      "a92",             000,          000,
      "a93",             000,          000,
      "a94",             000,          000,
      "a95",             000,          000,
      "a96",             000,          000,
      NULL,              000,          000
} ;
#endif

/*****************************************************************************/
/*                                                                           */
/* PostScript Name             PM Glyph number                               */
/* |                           |   Adobe encoding number                     */
/* |                           |   | Defined in Adobe red book appx E.5 Y/N  */
/* |                           |   | | IBM GCID  Unicode                     */
/* |                           |   | | |         |      Unicode Description  */
/* |                           |   | | |         |      |                    */
 char *PMGlyphName[] = {
  "     ",                //   0   0 0 00000000  U0000  null
  "SS000000",             //   1   0 0 SS000000  U263A  white smiling face
  "SS010000",             //   2   0 0 SS010000  U263B  black smiling face
  "heart",                //   3   0 0 SS020000  U2665  black heart suit
  "diamond",              //   4   0 0 SS030000  U2666  black diamond suit
  "club",                 //   5   0 0 SS040000  U2663  black club suit
  "spade",                //   6   0 0 SS050000  U2660  black spade suit
  "bullet",               //   7 183 1 SM570000  U2022  bullet
  "SM570001",             //   8   0 0 SM570001  U25D8  inverse bullet
  "SM750000",             //   9   0 0 SM750000  U25CB  white circle
  "SM750002",             //  10   0 0 SM750002  U25D9  inverse white circle
  "male",                 //  11   0 0 SM280000  U2642  male sign
  "female",               //  12   0 0 SM290000  U2640  female sign
  "musicalnote",          //  13   0 0 SM930000  U266A  eighth note
  "musicalnotedbl",       //  14   0 0 SM910000  U266B  beamed eighth notes
  "SM690000",             //  15   0 0 SM690000  U263C  white sun with rays
  "SM590000",             //  16   0 0 SM590000  U25BA  black right-pointing pointer
  "SM630000",             //  17   0 0 SM630000  U25C4  black left-pointing pointer
  "SM760000",             //  18   0 0 SM760000  U2195  up down arrow
  "exclamdbl",            //  19   0 0 SP330000  U203C  double exclamation mark
  "paragraph",            //  20 182 1 SM250000         paragraph -- duplicate
  "section",              //  21 167 1 SM240000         section -- duplicate
  "SM700000",             //  22   0 0 SM700000  U25AC  black rectangle
  "SM770000",             //  23   0 0 SM770000  U21A8  up down arrow with base
  "arrowup",              //  24   0 0 SM320000  U2191  upwards arrow
  "arrowdown",            //  25   0 0 SM330000  U2193  downwards arrow
  "arrowright",           //  26   0 0 SM310000  U2192  rightwards arrow
  "arrowleft",            //  27   0 0 SM300000  U2190  leftwards arrow
  "rightangle",           //  28   0 0 SA420000  U221F  right angle
  "arrowboth",            //  29   0 0 SM780000  U2194  left right arrow
  "SM600000",             //  30   0 0 SM600000  U25B2  black up-pointing triangle
  "SV040000",             //  31   0 0 SV040000  U25BC  black down-pointing triangle
  "space",                //  32  32 1 SP010000  U0020  space
  "exclam",               //  33  33 1 SP020000  U0021  exclamation mark
  "quotedbl",             //  34  34 1 SP040000  U0022  quotation mark
  "numbersign",           //  35  35 1 SM010000  U0023  number sign
  "dollar",               //  36  36 1 SC030000  U0024  dollar sign
  "percent",              //  37  37 1 SM020000  U0025  percent sign
  "ampersand",            //  38  38 1 SM030000  U0026  ampersand
  "quotesingle",          //  39 169 1 SP050000  U0027  apostrophe
  "parenleft",            //  40  40 1 SP060000  U0028  left parenthesis
  "parenright",           //  41  41 1 SP070000  U0029  right parenthesis
  "asterisk",             //  42  42 1 SM040000  U002A  asterisk
  "plus",                 //  43  43 1 SA010000  U002B  plus sign
  "comma",                //  44  44 1 SP080000  U002C  comma
  "hyphen",               //  45  45 1 SP100000  U002D  hyphen-minus
  "period",               //  46  46 1 SP110000  U002E  full stop
  "slash",                //  47  47 1 SP120000  U002F  solidus
  "zero",                 //  48  48 1 ND100000  U0030  digit zero
  "one",                  //  49  49 1 ND010000  U0031  digit one
  "two",                  //  50  50 1 ND020000  U0032  digit two
  "three",                //  51  51 1 ND030000  U0033  digit three
  "four",                 //  52  52 1 ND040000  U0034  digit four
  "five",                 //  53  53 1 ND050000  U0035  digit five
  "six",                  //  54  54 1 ND060000  U0036  digit six
  "seven",                //  55  55 1 ND070000  U0037  digit seven
  "eight",                //  56  56 1 ND080000  U0038  digit eight
  "nine",                 //  57  57 1 ND090000  U0039  digit nine
  "colon",                //  58  58 1 SP130000  U003A  colon
  "semicolon",            //  59  59 1 SP140000  U003B  semicolon
  "less",                 //  60  60 1 SA030000  U003C  less-than sign
  "equal",                //  61  61 1 SA040000  U003D  equals sign
  "greater",              //  62  62 1 SA050000  U003E  greater-than sign
  "question",             //  63  63 1 SP150000  U003F  question mark
  "at",                   //  64  64 1 SM050000  U0040  commercial at
  "A",                    //  65  65 1 LA020000  U0041  Latin capital a
  "B",                    //  66  66 1 LB020000  U0042  Latin capital b
  "C",                    //  67  67 1 LC020000  U0043  Latin capital c
  "D",                    //  68  68 1 LD020000  U0044  Latin capital d
  "E",                    //  69  69 1 LE020000  U0045  Latin capital e
  "F",                    //  70  70 1 LF020000  U0046  Latin capital f
  "G",                    //  71  71 1 LG020000  U0047  Latin capital g
  "H",                    //  72  72 1 LH020000  U0048  Latin capital h
  "I",                    //  73  73 1 LI020000  U0049  Latin capital i
  "J",                    //  74  74 1 LJ020000  U004A  Latin capital j
  "K",                    //  75  75 1 LK020000  U004B  Latin capital k
  "L",                    //  76  76 1 LL020000  U004C  Latin capital l
  "M",                    //  77  77 1 LM020000  U004D  Latin capital m
  "N",                    //  78  78 1 LN020000  U004E  Latin capital n
  "O",                    //  79  79 1 LO020000  U004F  Latin capital o
  "P",                    //  80  80 1 LP020000  U0050  Latin capital p
  "Q",                    //  81  81 1 LQ020000  U0051  Latin capital q
  "R",                    //  82  82 1 LR020000  U0052  Latin capital r
  "S",                    //  83  83 1 LS020000  U0053  Latin capital s
  "T",                    //  84  84 1 LT020000  U0054  Latin capital t
  "U",                    //  85  85 1 LU020000  U0055  Latin capital u
  "V",                    //  86  86 1 LV020000  U0056  Latin capital v
  "W",                    //  87  87 1 LW020000  U0057  Latin capital w
  "X",                    //  88  88 1 LX020000  U0058  Latin capital x
  "Y",                    //  89  89 1 LY020000  U0059  Latin capital y
  "Z",                    //  90  90 1 LZ020000  U005A  Latin capital z
  "bracketleft",          //  91  91 1 SM060000  U005B  left square bracket
  "backslash",            //  92  92 1 SM070000  U005C  reverse solidus
  "bracketright",         //  93  93 1 SM080000  U005D  right square bracket
  "asciicircum",          //  94  94 1 SD150000  U005E  circumflex accent
  "underscore",           //  95  95 1 SP090000  U005F  low line
  "grave",                //  96 193 1 SD130000  U0060  grave accent
  "a",                    //  97  97 1 LA010000  U0061  Latin small a
  "b",                    //  98  98 1 LB010000  U0062  Latin small b
  "c",                    //  99  99 1 LC010000  U0063  Latin small c
  "d",                    // 100 100 1 LD010000  U0064  Latin small d
  "e",                    // 101 101 1 LE010000  U0065  Latin small e
  "f",                    // 102 102 1 LF010000  U0066  Latin small f
  "g",                    // 103 103 1 LG010000  U0067  Latin small g
  "h",                    // 104 104 1 LH010000  U0068  Latin small h
  "i",                    // 105 105 1 LI010000  U0069  Latin small i
  "j",                    // 106 106 1 LJ010000  U006A  Latin small j
  "k",                    // 107 107 1 LK010000  U006B  Latin small k
  "l",                    // 108 108 1 LL010000  U006C  Latin small l
  "m",                    // 109 109 1 LM010000  U006D  Latin small m
  "n",                    // 110 110 1 LN010000  U006E  Latin small n
  "o",                    // 111 111 1 LO010000  U006F  Latin small o
  "p",                    // 112 112 1 LP010000  U0070  Latin small p
  "q",                    // 113 113 1 LQ010000  U0071  Latin small q
  "r",                    // 114 114 1 LR010000  U0072  Latin small r
  "s",                    // 115 115 1 LS010000  U0073  Latin small s
  "t",                    // 116 116 1 LT010000  U0074  Latin small t
  "u",                    // 117 117 1 LU010000  U0075  Latin small u
  "v",                    // 118 118 1 LV010000  U0076  Latin small v
  "w",                    // 119 119 1 LW010000  U0077  Latin small w
  "x",                    // 120 120 1 LX010000  U0078  Latin small x
  "y",                    // 121 121 1 LY010000  U0079  Latin small y
  "z",                    // 122 122 1 LZ010000  U007A  Latin small z
  "braceleft",            // 123 123 1 SM110000  U007B  left curly bracket
  "bar",                  // 124 124 1 SM130000  U007C  vertical line
  "braceright",           // 125 125 1 SM140000  U007D  right curly bracket
  "asciitilde",           // 126 126 1 SD190000  U007E  tilde
  "SM790000",             // 127   0 0 SM790000  U2302  house
  "Ccedilla",             // 128   0 1 LC420000  U00C7  Latin capital c cedilla
  "udieresis",            // 129   0 1 LU170000  U00FC  Latin small u diaeresis
  "eacute",               // 130   0 1 LE110000  U00E9  Latin small e acute
  "acircumflex",          // 131   0 1 LA150000  U00E2  Latin small a circumflex
  "adieresis",            // 132   0 1 LA170000  U00E4  Latin small a diaeresis
  "agrave",               // 133   0 1 LA130000  U00E0  Latin small a grave
  "aring",                // 134   0 1 LA270000  U00E5  Latin small a ring above
  "ccedilla",             // 135   0 1 LC410000  U00E7  Latin small c cedilla
  "ecircumflex",          // 136   0 1 LE150000  U00EA  Latin small e circumflex
  "edieresis",            // 137   0 1 LE170000  U00EB  Latin small e diaeresis
  "egrave",               // 138   0 1 LE130000  U00E8  Latin small e grave
  "idieresis",            // 139   0 1 LI170000  U00EF  Latin small i diaeresis
  "icircumflex",          // 140   0 1 LI150000  U00EE  Latin small i circumflex
  "igrave",               // 141   0 1 LI130000  U00EC  Latin small i grave
  "Adieresis",            // 142   0 1 LA180000  U00C4  Latin capital a diaeresis
  "Aring",                // 143   0 1 LA280000  U00C5  Latin capital a ring above
  "Eacute",               // 144   0 1 LE120000  U00C9  Latin capital e acute
  "ae",                   // 145 241 1 LA510000  U00E6  Latin small ae
  "AE",                   // 146 225 1 LA520000  U00C6  Latin capital ae
  "ocircumflex",          // 147   0 1 LO150000  U00F4  Latin small o circumflex
  "odieresis",            // 148   0 1 LO170000  U00F6  Latin small o diaeresis
  "ograve",               // 149   0 1 LO130000  U00F2  Latin small o grave
  "ucircumflex",          // 150   0 1 LU150000  U00FB  Latin small u circumflex
  "ugrave",               // 151   0 1 LU130000  U00F9  Latin small u grave
  "ydieresis",            // 152   0 1 LY170000  U00FF  Latin small y diaeresis
  "Odieresis",            // 153   0 1 LO180000  U00D6  Latin capital o diaeresis
  "Udieresis",            // 154   0 1 LU180000  U00DC  Latin capital u diaeresis
  "oslash",               // 155 249 1 LO610000  U00F8  Latin small o stroke
  "sterling",             // 156 163 1 SC020000  U00A3  pound sign
  "Oslash",               // 157 233 1 LO620000  U00D8  Latin capital o stroke
  "multiply",             // 158   0 1 SA070000  U00D7  multiplication sign
  "florin",               // 159 166 1 SC070000  U0192  Latin small f hook
  "aacute",               // 160   0 1 LA110000  U00E1  Latin small a acute
  "iacute",               // 161   0 1 LI110000  U00ED  Latin small i acute
  "oacute",               // 162   0 1 LO110000  U00F3  Latin small o acute
  "uacute",               // 163   0 1 LU110000  U00FA  Latin small u acute
  "ntilde",               // 164   0 1 LN190000  U00F1  Latin small n tilde
  "Ntilde",               // 165   0 1 LN200000  U00D1  Latin capital n tilde
  "ordfeminine",          // 166 227 1 SM210000  U00AA  feminine ordinal indicator
  "ordmasculine",         // 167 235 1 SM200000  U00BA  masculine ordinal indicator
  "questiondown",         // 168 191 1 SP160000  U00BF  inverted question mark
  "registered",           // 169   0 1 SM530000  U00AE  registered sign
  "logicalnot",           // 170   0 1 SM660000  U00AC  not sign
  "onehalf",              // 171   0 1 NF010000  U00BD  vulgar fraction one half
  "onequarter",           // 172   0 1 NF040000  U00BC  vulgar fraction one quarter
  "exclamdown",           // 173 161 1 SP030000  U00A1  inverted exclamation mark
  "guillemotleft",        // 174 171 1 SP170000  U00AB  left-pointing double angle quotation
  "guillemotright",       // 175 187 1 SP180000  U00BB  right-pointing double angle quotation
  "SF140000",             // 176   0 0 SF140000  U2591  light shade
  "SF150000",             // 177   0 0 SF150000  U2592  medium shade
  "SF160000",             // 178   0 0 SF160000  U2593  dark shade
  "SF110000",             // 179   0 0 SF110000  U2502  box drawings light vertical
  "SF090000",             // 180   0 0 SF090000  U2524  box drawings light vertical and left
  "Aacute",               // 181   0 1 LA120000  U00C1  Latin capital a acute
  "Acircumflex",          // 182   0 1 LA160000  U00C2  Latin capital a circumflex
  "Agrave",               // 183   0 1 LA140000  U00C0  Latin capital a grave
  "copyright",            // 184   0 1 SM520000  U00A9  copyright sign
  "SF230000",             // 185   0 0 SF230000  U2563  box drawings double vertical and left
  "SF240000",             // 186   0 0 SF240000  U2551  box drawings double vertical
  "SF250000",             // 187   0 0 SF250000  U2557  box drawings double down and left
  "SF260000",             // 188   0 0 SF260000  U255D  box drawings double up and left
  "cent",                 // 189 162 1 SC040000  U00A2  cent sign
  "yen",                  // 190 165 1 SC050000  U00A5  yen sign
  "SF030000",             // 191   0 0 SF030000  U2510  box drawings light down and left
  "SF020000",             // 192   0 0 SF020000  U2514  box drawings light up and right
  "SF070000",             // 193   0 0 SF070000  U2534  box drawings light up and horizontal
  "SF060000",             // 194   0 0 SF060000  U252C  box drawings light down and horizonta
  "SF080000",             // 195   0 0 SF080000  U251C  box drawings light vertical and right
  "SF100000",             // 196   0 0 SF100000  U2500  box drawings light horizontal
  "SF050000",             // 197   0 0 SF050000  U253C  box drawings light vertical and horiz
  "atilde",               // 198   0 1 LA190000  U00E3  Latin small a tilde
  "Atilde",               // 199   0 1 LA200000  U00C3  Latin capital a tilde
  "SF380000",             // 200   0 0 SF380000  U255A  box drawings double up and right
  "SF390000",             // 201   0 0 SF390000  U2554  box drawings double down and right
  "SF400000",             // 202   0 0 SF400000  U2569  box drawings double up and horizontal
  "SF410000",             // 203   0 0 SF410000  U2566  box drawings double down and horizont
  "SF420000",             // 204   0 0 SF420000  U2560  box drawings double vertical and righ
  "SF430000",             // 205   0 0 SF430000  U2550  box drawings double horizontal
  "SF440000",             // 206   0 0 SF440000  U256C  box drawings double vertical and hori
  "currency",             // 207 168 1 SC010000  U00A4  currency sign
  "eth",                  // 208   0 1 LD630000  U00F0  Latin small eth
  "Eth",                  // 209   0 1 LD620000  U00D0  Latin capital eth
  "Ecircumflex",          // 210   0 1 LE160000  U00CA  Latin capital e circumflex
  "Edieresis",            // 211   0 1 LE180000  U00CB  Latin capital e diaeresis
  "Egrave",               // 212   0 1 LE140000  U00C8  Latin capital e grave
  "dotlessi",             // 213 245 1 SC200000  U20AC  euro sign
  "Iacute",               // 214   0 1 LI120000  U00CD  Latin capital i acute
  "Icircumflex",          // 215   0 1 LI160000  U00CE  Latin capital i circumflex
  "Idieresis",            // 216   0 1 LI180000  U00CF  Latin capital i diaeresis
  "SF040000",             // 217   0 0 SF040000  U2518  box drawings light up and left
  "SF010000",             // 218   0 0 SF010000  U250C  box drawings light down and right
  "SF610000",             // 219   0 0 SF610000  U2588  full block
  "SF570000",             // 220   0 0 SF570000  U2584  lower half block
  "brokenbar",            // 221   0 1 SM650000  U00A6  broken bar
  "Igrave",               // 222   0 1 LI140000  U00CC  Latin capital i grave
  "SF600000",             // 223   0 0 SF600000  U2580  upper half block
  "Oacute",               // 224   0 1 LO120000  U00D3  Latin capital o acute
  "germandbls",           // 225 251 1 LS610000  U00DF  Latin small sharp s
  "Ocircumflex",          // 226   0 1 LO160000  U00D4  Latin capital o circumflex
  "Ograve",               // 227   0 1 LO140000  U00D2  Latin capital o grave
  "otilde",               // 228   0 1 LO190000  U00F5  Latin small o tilde
  "Otilde",               // 229   0 1 LO200000  U00D5  Latin capital o tilde
  "micro",                // 230   0 0 SM170000  U00B5  micro sign
  "thorn",                // 231   0 1 LT630000  U00FE  Latin small thorn
  "Thorn",                // 232   0 1 LT640000  U00DE  Latin capital thorn
  "Uacute",               // 233   0 1 LU120000  U00DA  Latin capital u acute
  "Ucircumflex",          // 234   0 1 LU160000  U00DB  Latin capital u circumflex
  "Ugrave",               // 235   0 1 LU140000  U00D9  Latin capital u grave
  "yacute",               // 236   0 1 LY110000  U00FD  Latin small y acute
  "Yacute",               // 237   0 1 LY120000  U00DD  Latin capital y acute
  "overline",             // 238   0 0 SM150000  U00AF  macron
  "acute",                // 239 194 1 SD110000  U00B4  acute accent
  "hyphen",               // 240  45 1 SP320000  U00AD  soft hyphen -- duplicate
  "plusminus",            // 241   0 1 SA020000  U00B1  plus-minus sign
  "underscoredbl",        // 242   0 0 SM100000  U2017  double low line
  "threequarters",        // 243   0 1 NF050000  U00BE  fraction three quarters
  "paragraph",            // 244 182 1 SM250000  U00B6  pilcrow sign
  "section",              // 245 167 1 SM240000  U00A7  section sign
  "divide",               // 246   0 1 SA060000  U00F7  division sign
  "cedilla",              // 247 203 1 SD410000  U00B8  cedilla
  "degree",               // 248   0 1 SM190000  U00B0  degree sign
  "dieresis",             // 249 200 1 SD170000  U00A8  diaeresis
  "periodcentered",       // 250 180 1 SD630000  U00B7  middle dot
  "onesuperior",          // 251   0 1 ND011000  U00B9  superscript one
  "threesuperior",        // 252   0 1 ND031000  U00B3  superscript three
  "twosuperior",          // 253   0 1 ND021000  U00B2  superscript two
  "filledbox",            // 254   0 0 SM470000  U25A0  black square
  "space",                // 255   0 0 SP300000  U00A0  no-break space -- duplicate
  "Pts",                  // 256   0 0 SC060000  U20A7  peseta sign
  "SM680000",             // 257   0 0 SM680000  U2310  reversed not sign
  "SF190000",             // 258   0 0 SF190000  U2561  box drawings vertical single and left double
  "SF200000",             // 259   0 0 SF200000  U2562  box drawings vertical double and left single
  "SF210000",             // 260   0 0 SF210000  U2556  box drawings down double and left single
  "SF220000",             // 261   0 0 SF220000  U2555  box drawings down single and left double
  "SF270000",             // 262   0 0 SF270000  U255C  box drawings up double and left single
  "SF280000",             // 263   0 0 SF280000  U255B  box drawings up single and left double
  "SF360000",             // 264   0 0 SF360000  U255E  box drawings vertical single and right double
  "SF370000",             // 265   0 0 SF370000  U255F  box drawings vertical double and right single
  "SF450000",             // 266   0 0 SF450000  U2567  box drawings up single and horizontal double
  "SF460000",             // 267   0 0 SF460000  U2568  box drawings up double and horizontal single
  "SF470000",             // 268   0 0 SF470000  U2564  box drawings down single and horizontal double
  "SF480000",             // 269   0 0 SF480000  U2565  box drawings down double and horizontal single
  "SF490000",             // 270   0 0 SF490000  U2559  box drawings up double and right single
  "SF500000",             // 271   0 0 SF500000  U2558  box drawings up single and right double
  "SF510000",             // 272   0 0 SF510000  U2552  box drawings down single and right double
  "SF520000",             // 273   0 0 SF520000  U2553  box drawings down double and right single
  "SF530000",             // 274   0 0 SF530000  U256B  box drawings vertical double and horizontal single
  "SF540000",             // 275   0 0 SF540000  U256A  box drawings vertical single and horizontal double
  "SF580000",             // 276   0 0 SF580000  U258C  left half block
  "SF590000",             // 277   0 0 SF590000  U2590  right half block
  "alpha",                // 278   0 0 GA010000  U03B1  Greek small alpha
  "Gamma",                // 279   0 0 GG020000  U0393  Greek capital gamma
  "pi",                   // 280   0 0 GP010000  U03C0  Greek small pi
  "Sigma",                // 281   0 0 GS020000  U03A3  Greek capital sigma
  "sigma",                // 282   0 0 GS010000  U03C3  Greek small sigma
  "tau",                  // 283   0 0 GT010000  U03C4  Greek small tau
  "Phi",                  // 284   0 0 GF020000  U03A6  Greek capital phi
  "Theta",                // 285   0 0 GT620000  U0398  Greek capital theta
  "Omega",                // 286   0 0 GO320000  U03A9  Greek capital omega
  "delta",                // 287   0 0 GD010000  U03B4  Greek small delta
  "infinity",             // 288   0 0 SA450000  U221E  infinity
  "phi",                  // 289   0 0 GF010000  U03C6  Greek small phi
  "epsilon",              // 290   0 0 GE010000  U03B5  Greek small epsilon
  "intersection",         // 291   0 0 SA380000  U2229  intersection
  "equivalence",          // 292   0 0 SA480000  U2261  identical to
  "greaterequal",         // 293   0 0 SA530000  U2265  greater-than or equal to
  "lessequal",            // 294   0 0 SA520000  U2264  less-than or equal to
  "SS260000",             // 295   0 0 SS260000  U2320  top half integral
  "SS270000",             // 296   0 0 SS270000  U2321  bottom half integral
  "approxequal",          // 297   0 0 SA700000  U2248  almost equal to
  "productdot",           // 298   0 0 SA790000  U2219  bullet operator
  "radical",              // 299   0 0 SA800000  U221A  square root
  "nsuperior",            // 300   0 0 LN011000  U207F  superscript Latin small n
  "macron",               // 301 197 1 SD310000  U02C9  modifier macron
  "breve",                // 302 198 1 SD230000  U02D8  breve
  "dotaccent",            // 303 199 1 SD290000  U02D9  dot above
  "ring",                 // 304 202 1 SD270000  U02DA  ring above
  "hungarumlaut",         // 305 205 1 SD250000  U02DD  double acute accent
  "ogonek",               // 306 206 1 SD430000  U02DB  ogonek
  "caron",                // 307 207 1 SD210000  U02C7  caron
  "quoteleft",            // 308  96 1 SP190000  U2018  left single quotation mark
  "quoteright",           // 309  39 1 SP200000  U2019  right single quotation mark
  "quotedblleft",         // 310 170 1 SP210000  U201C  left double quotation mark
  "quotedblright",        // 311 186 1 SP220000  U201D  right double quotation mark
  "endash",               // 312 177 1 SS680000  U2013  en dash
  "emdash",               // 313 208 1 SM900000  U2014  em dash
  "yuan",                 // 314  94 1 SC120000  UFFE5  fullwidth yen sign
  "Euro",                 // 315 126 1 LI610000  U0131  Latin small dotless i
  "quotesinglbase",       // 316 184 1 SP260000  U201A  single low-9 quotation mark
  "quotedblbase",         // 317 185 1 SP230000  U201E  double low-9 quotation mark
  "ellipsis",             // 318 188 1 SV520000  U2026  horizontal ellipsis
  "dagger",               // 319 178 1 SM340000  U2020  dagger
  "daggerdbl",            // 320 179 1 SM350000  U2021  double dagger
  "circumflex",           // 321 195 1 SD150100  U02C6  modifier circumflex accent
  "perthousand",          // 322 189 1 SM560000  U2030  per mille sign
  "Scaron",               // 323   0 1 LS220000  U0160  Latin capital s caron
  "guilsinglleft",        // 324 172 1 SP270000  U2039  single left-pointing angle quotation mark
  "OE",                   // 325 234 1 LO520000  U0152  Latin capital ligature oe
  "tilde",                // 326 196 1 SD190100  U02DC  small tilde
  "trademark",            // 327   0 1 SM540000  U2122  trade mark sign
  "scaron",               // 328   0 1 LS210000  U0161  Latin small s caron
  "guilsinglright",       // 329 173 1 SP280000  U203A  single right-pointing angle quotation mark
  "oe",                   // 330 250 1 LO510000  U0153  Latin small ligature oe
  "Ydieresis",            // 331   0 1 LY180000  U0178  Latin capital y diaeresis
  "gbreve",               // 332   0 0 LG230000  U011F  Latin small g breve
  "Gbreve",               // 333   0 0 LG240000  U011E  Latin capital g breve
  "Idotaccent",           // 334   0 0 LI300000  U0130  Latin capital i dot above
  "scedilla",             // 335   0 0 LS410000  U015F  Latin small s cedilla
  "Scedilla",             // 336   0 0 LS420000  U015E  Latin capital s cedilla
  "abreve",               // 337   0 0 LA230000  U0103  Latin small a breve
  "Abreve",               // 338   0 0 LA240000  U0102  Latin capital a breve
  "aogonek",              // 339   0 0 LA430000  U0105  Latin small a ogonek
  "Aogonek",              // 340   0 0 LA440000  U0104  Latin capital a ogonek
  "cacute",               // 341   0 0 LC110000  U0107  Latin small c acute
  "Cacute",               // 342   0 0 LC120000  U0106  Latin capital c acute
  "ccaron",               // 343   0 0 LC210000  U010D  Latin small c caron
  "Ccaron",               // 344   0 0 LC220000  U010C  Latin capital c caron
  "dcaron",               // 345   0 0 LD210000  U010F  Latin small d caron
  "Dcaron",               // 346   0 0 LD220000  U010E  Latin capital d caron
  "dbar",                 // 347   0 0 LD610000  U0111  Latin small d stroke
  "ecaron",               // 348   0 0 LE210000  U011B  Latin small e caron
  "Ecaron",               // 349   0 0 LE220000  U011A  Latin capital e caron
  "eogonek",              // 350   0 0 LE430000  U0119  Latin small e ogonek
  "Eogonek",              // 351   0 0 LE440000  U0118  Latin capital e ogonek
  "lacute",               // 352   0 0 LL110000  U013A  Latin small l acute
  "Lacute",               // 353   0 0 LL120000  U0139  Latin capital l acute
  "lcaron",               // 354   0 0 LL210000  U013E  Latin small l caron
  "Lcaron",               // 355   0 0 LL220000  U013D  Latin capital l caron
  "lslash",               // 356   0 1 LL610000  U0142  Latin small l stroke
  "Lslash",               // 357   0 1 LL620000  U0141  Latin capital l stroke
  "nacute",               // 358   0 0 LN110000  U0144  Latin small n acute
  "Nacute",               // 359   0 0 LN120000  U0143  Latin capital n acute
  "ncaron",               // 360   0 0 LN210000  U0148  Latin small n caron
  "Ncaron",               // 361   0 0 LN220000  U0147  Latin capital n caron
  "ohungarumlaut",        // 362   0 0 LO250000  U0151  Latin small o double acute
  "Ohungarumlaut",        // 363   0 0 LO260000  U0150  Latin capital o double acute
  "racute",               // 364   0 0 LR110000  U0155  Latin small r acute
  "Racute",               // 365   0 0 LR120000  U0154  Latin capital r acute
  "rcaron",               // 366   0 0 LR210000  U0159  Latin small r caron
  "Rcaron",               // 367   0 0 LR220000  U0158  Latin capital r caron
  "sacute",               // 368   0 0 LS110000  U015B  Latin small s acute
  "Sacute",               // 369   0 0 LS120000  U015A  Latin capital s acute
  "tcaron",               // 370   0 0 LT210000  U0165  Latin small t caron
  "Tcaron",               // 371   0 0 LT220000  U0164  Latin capital t caron
  "tcedilla",             // 372   0 0 LT410000  U0163  Latin small t cedilla
  "Tcedilla",             // 373   0 0 LT420000  U0162  Latin capital t cedilla
  "uhungarumlaut",        // 374   0 0 LU250000  U0171  Latin small u double acute
  "Uhungarumlaut",        // 375   0 0 LU260000  U0170  Latin capital u double acute
  "uring",                // 376   0 0 LU270000  U016F  Latin small u ring above
  "Uring",                // 377   0 0 LU280000  U016E  Latin capital u ring above
  "zacute",               // 378   0 0 LZ110000  U017A  Latin small z acute
  "Zacute",               // 379   0 0 LZ120000  U0179  Latin capital z acute
  "zcaron",               // 380   0 1 LZ210000  U017E  Latin small z caron
  "Zcaron",               // 381   0 1 LZ220000  U017D  Latin capital z caron
  "zdotaccent",           // 382   0 0 LZ290000  U017C  Latin small z dot above
  "Zdotaccent",           // 383   0 0 LZ300000  U017B  Latin capital z dot above

  /*
   * Warp 3 ended at this point.  Add Cyriilic and Baltic characters
   */
  "afii10023",            // 384       KE180000  U0401  Cyrillic capital io
  "afii10051",            // 385       KD620000  U0402  Cyrillic capital dje
  "afii10052",            // 386       KG120000  U0403  Cyrillic capital gje
  "afii10053",            // 387       KE160000  U0404  Cyrillic capital ukrainian ie
  "afii10054",            // 388       KZ160000  U0405  Cyrillic capital dze
  "afii10055",            // 389       KI120000  U0406  Cyrillic capital ukrainian i
  "afii10056",            // 390       KI180000  U0407  Cyrillic capital yi
  "afii10057",            // 391       KJ020000  U0408  Cyrillic capital je
  "afii10058",            // 392       KL420000  U0409  Cyrillic capital lje
  "afii10059",            // 393       KN120000  U040A  Cyrillic capital nje
  "afii10060",            // 394       KC120000  U040B  Cyrillic capital tshe
  "afii10061",            // 395       KK120000  U040C  Cyrillic capital kje
  "afii10062",            // 396       KU240000  U040E  Cyrillic capital short u
  "afii10145",            // 397       KG220000  U040F  Cyrillic capital dzhe
  "afii10017",            // 398       KA020000  U0410  Cyrillic capital a
  "afii10018",            // 399       KB020000  U0411  Cyrillic capital be
  "afii10019",            // 400       KV020000  U0412  Cyrillic capital ve
  "afii10020",            // 401       KG020000  U0413  Cyrillic capital ghe
  "afii10021",            // 402       KD020000  U0414  Cyrillic capital de
  "afii10022",            // 403       KE020000  U0415  Cyrillic capital ie
  "afii10024",            // 404       KZ220000  U0416  Cyrillic capital zhe
  "afii10025",            // 405       KZ020000  U0417  Cyrillic capital ze
  "afii10026",            // 406       KI020000  U0418  Cyrillic capital i
  "afii10027",            // 407       KJ120000  U0419  Cyrillic capital short i
  "afii10028",            // 408       KK020000  U041A  Cyrillic capital ka
  "afii10029",            // 409       KL020000  U041B  Cyrillic capital el
  "afii10030",            // 410       KM020000  U041C  Cyrillic capital em
  "afii10031",            // 411       KN020000  U041D  Cyrillic capital en
  "afii10032",            // 412       KO020000  U041E  Cyrillic capital o
  "afii10033",            // 413       KP020000  U041F  Cyrillic capital pe
  "afii10034",            // 414       KR020000  U0420  Cyrillic capital er
  "afii10035",            // 415       KS020000  U0421  Cyrillic capital es
  "afii10036",            // 416       KT020000  U0422  Cyrillic capital te
  "afii10037",            // 417       KU020000  U0423  Cyrillic capital u
  "afii10038",            // 418       KF020000  U0424  Cyrillic capital ef
  "afii10039",            // 419       KH020000  U0425  Cyrillic capital ha
  "afii10040",            // 420       KC020000  U0426  Cyrillic capital tse
  "afii10041",            // 421       KC220000  U0427  Cyrillic capital che
  "afii10042",            // 422       KS220000  U0428  Cyrillic capital sha
  "afii10043",            // 423       KS160000  U0429  Cyrillic capital shcha
  "afii10044",            // 424       KU220000  U042A  Cyrillic capital hard sign
  "afii10045",            // 425       KY020000  U042B  Cyrillic capital yeru
  "afii10046",            // 426       KX120000  U042C  Cyrillic capital soft sign
  "afii10047",            // 427       KE140000  U042D  Cyrillic capital e
  "afii10048",            // 428       KU160000  U042E  Cyrillic capital yu
  "afii10049",            // 429       KA160000  U042F  Cyrillic capital ya
  "afii10065",            // 430       KA010000  U0430  Cyrillic small a
  "afii10066",            // 431       KB010000  U0431  Cyrillic small be
  "afii10067",            // 432       KV010000  U0432  Cyrillic small ve
  "afii10068",            // 433       KG010000  U0433  Cyrillic small ghe
  "afii10069",            // 434       KD010000  U0434  Cyrillic small de
  "afii10070",            // 435       KE010000  U0435  Cyrillic small ie
  "afii10072",            // 436       KZ210000  U0436  Cyrillic small zhe
  "afii10073",            // 437       KZ010000  U0437  Cyrillic small ze
  "afii10074",            // 438       KI010000  U0438  Cyrillic small i
  "afii10075",            // 439       KJ110000  U0439  Cyrillic small short i
  "afii10076",            // 440       KK010000  U043A  Cyrillic small ka
  "afii10077",            // 441       KL010000  U043B  Cyrillic small el
  "afii10078",            // 442       KM010000  U043C  Cyrillic small em
  "afii10079",            // 443       KN010000  U043D  Cyrillic small en
  "afii10080",            // 444       KO010000  U043E  Cyrillic small o
  "afii10081",            // 445       KP010000  U043F  Cyrillic small pe
  "afii10082",            // 446       KR010000  U0440  Cyrillic small er
  "afii10083",            // 447       KS010000  U0441  Cyrillic small es
  "afii10084",            // 448       KT010000  U0442  Cyrillic small te
  "afii10085",            // 449       KU010000  U0443  Cyrillic small u
  "afii10086",            // 450       KF010000  U0444  Cyrillic small ef
  "afii10087",            // 451       KH010000  U0445  Cyrillic small ha
  "afii10088",            // 452       KC010000  U0446  Cyrillic small tse
  "afii10089",            // 453       KC210000  U0447  Cyrillic small che
  "afii10090",            // 454       KS210000  U0448  Cyrillic small sha
  "afii10091",            // 455       KS150000  U0449  Cyrillic small shcha
  "afii10092",            // 456       KU210000  U044A  Cyrillic small hard sign
  "afii10093",            // 457       KY010000  U044B  Cyrillic small yeru
  "afii10094",            // 458       KX110000  U044C  Cyrillic small soft sign
  "afii10095",            // 459       KE130000  U044D  Cyrillic small e
  "afii10096",            // 460       KU150000  U044E  Cyrillic small yu
  "afii10097",            // 461       KA150000  U044F  Cyrillic small ya
  "afii61352",            // 462       SM000000  U2116  numero sign
  "afii10071",            // 463       KE170000  U0451  Cyrillic small io
  "afii10099",            // 464       KD610000  U0452  Cyrillic small dje
  "afii10100",            // 465       KG110000  U0453  Cyrillic small gje
  "afii10101",            // 466       KE150000  U0454  Cyrillic small ukrainian ie
  "afii10102",            // 467       KZ150000  U0455  Cyrillic small dze
  "afii10103",            // 468       KI110000  U0456  Cyrillic small ukrainian i
  "afii10104",            // 469       KI170000  U0457  Cyrillic small yi
  "afii10105",            // 470       KJ010000  U0458  Cyrillic small je
  "afii10106",            // 471       KL410000  U0459  Cyrillic small lje
  "afii10107",            // 472       KN110000  U045A  Cyrillic small nje
  "afii10108",            // 473       KC110000  U045B  Cyrillic small tshe
  "afii10109",            // 474       KK110000  U045C  Cyrillic small kje
  "afii10110",            // 475       KU230000  U045E  Cyrillic small short u
  "afii10193",            // 476       KG210000  U045F  Cyrillic small dzhe
  "afii10050",            // 477       KG300000  U0490  Cyrillic capital ghe upturn
  "afii10098",            // 478       KG290000  U0491  Cyrillic small ghe upturn
  "Rcedilla",             // 479       LR420000  U0156  Latin capital r cedilla
  "Iogonek",              // 480       LI440000  U012E  Latin capital i ogonek
  "Amacron",              // 481       LA320000  U0100  Latin capital a macron
  "Emacron",              // 482       LE320000  U0112  Latin capital e macron
  "Edotaccent",           // 483       LE300000  U0116  Latin capital e dot above
  "Gcedilla",             // 484       LG420000  U0122  Latin capital g cedilla
  "Kcedilla",             // 485       LK420000  U0136  Latin capital k cedilla
  "Imacron",              // 486       LI320000  U012A  Latin capital i macron
  "Lcedilla",             // 487       LL420000  U01EB  Latin small o ogonek
  "Ncedilla",             // 488       LN420000  U0145  Latin capital n cedilla
  "Omacron",              // 489       LO320000  U014C  Latin capital o macron
  "Uogonek",              // 490       LU440000  U0172  Latin capital u ogonek
  "Umacron",              // 491       LU320000  U016A  Latin capital u macron
  "rcedilla",             // 492       LR410000  U0157  Latin small r cedilla
  "iogonek",              // 493       LI430000  U012F  Latin small i ogonek
  "amacron",              // 494       LA310000  U0101  Latin small a macron
  "emacron",              // 495       LE310000  U0113  Latin small e macron
  "edotaccent",           // 496       LE290000  U0117  Latin small e dot above
  "gcedilla",             // 497       LG410000  U0123  Latin small g cedilla
  "kcedilla",             // 498       LK410000  U0137  Latin small k cedilla
  "imacron",              // 499       LI310000  U012B  Latin small i macron
  "lcedilla",             // 500       LL410000  U013C  Latin small l cedilla
  "ncedilla",             // 501       LN410000  U0146  Latin small n cedilla
  "omacron",              // 502       LO310000  U014D  Latin small o macron
  "uogonek",              // 503       LU430000  U0173  Latin small u ogonek
  "umacron",              // 504       LU310000  U016B  Latin small u macron

  /*
   * Warp 4 ended at this point.  Add Apple characters
   */
  "replace",              // 505       SV320000  UFFFD  replacement character
  "notequal",             // 506       SA540000  U2260  not equal to
  "lozenge",              // 507       SA660000  U25CA  lozenge
  "option",               // 508       SS300000  U2318  place of interest sign
  "liter",                // 509       SM160000  U2113  script small l
  "fraction",             // 510       SP120001  U2044  fraction slash
  "partialdiff",          // 511       SA490000  U2022  partial differential

  /*
   * These dead keys are used by keyboard processing, and are not expected
   * to be printed.
   */
  "acute",                // 512       SD118000  U0300  combining grave accent -- duplicate
  "grave",                // 513       SD138000  U0301  combining acute accent -- duplicate
  "circumflex",           // 514       SD158000  U0302  combining circumflex accent  -- duplicate
  "dieresis",             // 515       SD178000  U0308  combining diaeresis -- duplicate
  "tilde",                // 516       SD198000  U0303  combining tilde  -- duplicate
  "cedilla",              // 517       SD418000  U0327  combining cedilla -- duplicate
  "caron",                // 518       SD218000  U030C  combining caron -- duplicate
  "hungarumlaut",         // 519       SD258000  U030B  combining double acute accent -- duplicate
  "ring",                 // 520       SD278000  U030A  combining ring above -- duplicate
  "breve",                // 521       SD238000  U0306  combining breve -- duplicate
  "ogonek",               // 522       SD438000  U0328  combining ogonek -- duplicate
  "dotaccent",            // 523       SD398000  U0307  combining dot above  -- duplicate
  ".notdef",              // 524       SD118001  U034F  NOTUNI
  ".notdef",              // 525       SD138001  U034E  NOTUNI
  ".notdef",              // 526       SD118007  U034D  NOTUNI
  ".notdef",              // 527       SD178007  U034C  NOTUNI
  ".notdef",              // 528       SD738000  U0344  combining Greek dialytika tonos
  ".notdef",              // 529       SD318000  U0304  combining macron
  ".notdef",              // 530       -

  /*
   * New characters added for Warp Server 99.  These include Hebrew, Greek,
   * Arabic, and Publishing.
   */
  "nonjoiner",            // 531       SP5300Z0  U200C  zero width non-joiner
  "joiner",               // 532       SP5400Z0  U200D  zero width joiner
  "lefttoright",          // 533       SP5500Z0  U200E  left-to-right mark
  "righttoleft",          // 534       SP5600Z0  U200F  right-to-left mark
  "afii57636",            // 535       SC170000  U20AA  new sheqel sign
  "afii57664",            // 536       HX330000  U05D0  Hebrew alef
  "afii57665",            // 537       HB010000  U05D1  Hebrew bet
  "afii57666",            // 538       HG010000  U05D2  Hebrew gimel
  "afii57667",            // 539       HD010000  U05D3  Hebrew dalet
  "afii57668",            // 540       HH010000  U05D4  Hebrew he
  "afii57669",            // 541       HW010000  U05D5  Hebrew vav
  "afii57670",            // 542       HZ010000  U05D6  Hebrew zayin
  "afii57671",            // 543       HH450000  U05D7  Hebrew het
  "afii57672",            // 544       HT450000  U05D8  Hebrew tet
  "afii57673",            // 545       HY010000  U05D9  Hebrew yod
  "afii57674",            // 546       HK610000  U05DA  Hebrew final kaf
  "afii57675",            // 547       HK010000  U05DB  Hebrew kaf
  "afii57676",            // 548       HL010000  U05DC  Hebrew lamed
  "afii57677",            // 549       HM610000  U05DD  Hebrew final mem
  "afii57678",            // 550       HM010000  U05DE  Hebrew mem
  "afii57679",            // 551       HN610000  U05DF  Hebrew final nun
  "afii57680",            // 552       HN010000  U05E0  Hebrew nun
  "afii57681",            // 553       HS010000  U05E1  Hebrew samekh
  "afii57682",            // 554       HX350000  U05E2  Hebrew ayin
  "afii57683",            // 555       HP610000  U05E3  Hebrew final pe
  "afii57684",            // 556       HP010000  U05E4  Hebrew pe
  "afii57685",            // 557       HS610000  U05E5  Hebrew final tsadi
  "afii57686",            // 558       HS450000  U05E6  Hebrew tsadi
  "afii57687",            // 559       HQ010000  U05E7  Hebrew qof
  "afii57688",            // 560       HR010000  U05E8  Hebrew resh
  "afii57689",            // 561       HS210000  U05E9  Hebrew shin
  "afii57690",            // 562       HT010000  U05EA  Hebrew tav
  "afii57799",            // 563       SD808000  U05B0  Hebrew point sheva
  "afii57801",            // 564       SD818000  U05B1  Hebrew point hataf segol
  "afii57800",            // 565       SD828000  U05B2  Hebrew point hataf patah
  "afii57802",            // 566       SD838000  U05B3  Hebrew point hataf qamats
  "afii57793",            // 567       SD848000  U05B4  Hebrew point hiriq
  "afii57794",            // 568       SD858000  U05B5  Hebrew point tsere
  "afii57795",            // 569       SD868000  U05B6  Hebrew point segol
  "afii57798",            // 570       SD878000  U05B7  Hebrew point patah
  "afii57797",            // 571       SD888000  U05B8  Hebrew point qamats
  "afii57806",            // 572       SD898000  U05B9  Hebrew point holam
  "afii57796",            // 573       SD928000  U05BB  Hebrew point qubuts
  "afii57807",            // 574       SD938000  U05BC  Hebrew point dagesh or mapiq
  "afii57839",            // 575       SD948000  U05BD  Hebrew point meteg
  "afii57645",            // 576       SD640000  U05BE  Hebrew punctuation maqaf
  "afii57841",            // 577       SD958000  U05BF  Hebrew point rafe
  "afii57842",            // 578       SD650000  U05C0  Hebrew punctuation paseq
  "afii57804",            // 579       SD968000  U05C1  Hebrew point shin dot
  "afii57803",            // 580       SD978000  U05C2  Hebrew point sin dot
  "afii57658",            // 581       SD660000  U05C3  Hebrew punctuation sof pasuq
  "afii57716",            // 582       HW610000  U05F0  Hebrew ligature yiddish double vav
  "afii57717",            // 583       HW630000  U05F1  Hebrew ligature yiddish vav yod
  "afii57718",            // 584       HY610000  U05F2  Hebrew ligature yiddish double yod
  "longdash",             // 585       SM120000  U2015  horizontal bar
  "tonos",                // 586       SD110001  U0384  Greek tonos
  "dieresistonos",        // 587       SD730000  U0385  Greek dialytika tonos
  "Alpha",                // 588       GA020000  U0391  Greek capital alpha
  "Beta",                 // 589       GB020000  U0392  Greek capital beta
  "Delta",                // 590       GD020000  U0394  Greek capital delta
  "Epsilon",              // 591       GE020000  U0395  Greek capital epsilon
  "Zeta",                 // 592       GZ020000  U0396  Greek capital zeta
  "Eta",                  // 593       GE320000  U0397  Greek capital eta
  "theta",                // 594       GT610001  U03d1  Greek theta symbol -- duplicate
  "Iota",                 // 595       GI020000  U0399  Greek capital iota
  "Kappa",                // 596       GK020000  U039A  Greek capital kappa
  "Lambda",               // 597       GL020000  U039B  Greek capital lamda
  "Mu",                   // 598       GM020000  U039C  Greek capital mu
  "Nu",                   // 599       GN020000  U039D  Greek capital nu
  "Xi",                   // 600       GX020000  U039E  Greek capital xi
  "Omicron",              // 601       GO020000  U039F  Greek capital omicron
  "Pi",                   // 602       GP020000  U03A0  Greek capital pi
  "Rho",                  // 603       GR020000  U03A1  Greek capital rho
  "Tau",                  // 604       GT020000  U03A4  Greek capital tau
  "Upsilon",              // 605       GU020000  U03A5  Greek capital upsilon
  "Phi",                  // 606       GP620000  U03A6  Greek capital phi
  "Chi",                  // 607       GH020000  U03A7  Greek capital chi
  "beta",                 // 608       GB010000  U03B2  Greek small beta
  "gamma",                // 609       GG010000  U03B3  Greek small gamma
  "zeta",                 // 610       GZ010000  U03B6  Greek small zeta
  "eta",                  // 611       GE310000  U03B7  Greek small eta
  "theta",                // 612       GT610000  U03B8  Greek small theta
  "iota",                 // 613       GI010000  U03B9  Greek small iota
  "kappa",                // 614       GK010000  U03BA  Greek small kappa
  "lambda",               // 615       GL010000  U03BB  Greek small lamda
  "mu",                   // 616       GM010000  U03BC  Greek small mu
  "nu",                   // 617       GN010000  U03BD  Greek small nu
  "xi",                   // 618       GX010000  U03BE  Greek small xi
  "omicron",              // 619       GO010000  U03BF  Greek small omicron
  "rho",                  // 620       GR010000  U03C1  Greek small rho
  "sigma1",               // 621       GS610000  U03C2  Greek small final sigma
  "upsilon",              // 622       GU010000  U03C4  Greek small tau
  "phi",                  // 623       GF010000  U03C6  Greek small phi
  "chi",                  // 624       GH010000  U03C7  Greek small chi
  "psi",                  // 625       GP610000  U03C8  Greek small psi
  "omega",                // 626       GO310000  U03C9  Greek small omega
  "Alphatonos",           // 627       GA120000  U0386  Greek capital alpha tonos
  "Epsilontonos",         // 628       GE120000  U0388  Greek capital epsilon tonos
  "Etatonos",             // 629       GE720000  U0389  Greek capital eta tonos
  "Iotatonos",            // 630       GI120000  U038A  Greek capital iota tonos
  "Omicrontonos",         // 631       GO120000  U038C  Greek capital omicron tonos
  "Upsilontonos",         // 632       GU120000  U038E  Greek capital upsilon tonos
  "Omegatonos",           // 633       GO720000  U038F  Greek capital omega tonos
  "Iotadieresis",         // 634       GI180000  U03AA  Greek capital iota dialytika
  "Upsilondieresis",      // 635       GU180000  U03AB  Greek Upsilon dia
  "alphatonos",           // 636       GA110000  U03AC  Greek small alpha tonos
  "epsilontonos",         // 637       GE110000  U03AD  Greek small epsilon tonos
  "etatonos",             // 638       GE710000  U03AE  Greek small eta tonos
  "iotatonos",            // 639       GI110000  U03AF  Greek small iota tonos
  "omicrontonos",         // 640       GO110000  U03CC  Greek small omicron tonos
  "upsilontonos",         // 641       GU110000  U03CD  Greek small upsilon tonos
  "omegatonos",           // 642       GO710000  U03CE  Greek small omega tonos
  "iotadieresis",         // 643       GI170000  U03CA  Greek small iota dialytika
  "upsilondieresis",      // 644       GU170000  U03CB  Greek upsilon dia
  "iotadieresistonos",    // 645       GI730000  U0390  Greek small iota dialytika and tonos
  "upsilondieresistonnos",// 646       GU730000  U03B0  Greek small upsilon dialytika and tonos
  "afii62836",            // 647       AL320000  UFEF7  Arabic ligature lam alef hamza above isolated form
  "afii62837",            // 648       AL320003  UFEF8  Arabic ligature lam alef hamza above final form
  "afii62840",            // 649       AL020000  UFEFB  Arabic ligature lam alef isolated form
  "afii62841",            // 650       AL020003  UFEFC  Arabic ligature lam alef final form
  "afii63167",            // 651       SM870000  UF8FC  Arabic kasseh (tail)
  "afii62753",            // 652       AA210002  UFE82  Arabic alef madda above final form
  "afii62754",            // 653       AA310002  UFE84  Arabic alef hamza above final form
  "afii62760",            // 654       AA010002  UFE8E  Arabic alef final form
  "afii57416",            // 655       AB010000  U0628  Arabic beh
  "afii57418",            // 656       AT010000  U062A  Arabic teh
  "afii57419",            // 657       AT470000  U062B  Arabic theh
  "afii57388",            // 658       SP080007  U060C  Arabic comma
  "afii57420",            // 659       AG230000  U062C  Arabic jeem
  "afii57421",            // 660       AH450000  U062D  Arabic hah
  "afii57422",            // 661       AH470000  U062E  Arabic khah
  "afii57392",            // 662       ND100001  U0660  Arabic-indic digit zero
  "afii57393",            // 663       ND010001  U0661  Arabic-indic digit one
  "afii57394",            // 664       ND020001  U0662  Arabic-indic digit two
  "afii57395",            // 665       ND030001  U0663  Arabic-indic digit three
  "afii57396",            // 666       ND040001  U0664  Arabic-indic digit four
  "afii57397",            // 667       ND050001  U0665  Arabic-indic digit five
  "afii57398",            // 668       ND060001  U0666  Arabic-indic digit six
  "afii57399",            // 669       ND070001  U0667  Arabic-indic digit seven
  "afii57400",            // 670       ND080001  U0668  Arabic-indic digit eight
  "afii57401",            // 671       ND090001  U0669  Arabic-indic digit nine
  "afii57441",            // 672       AF010000  U0641  Arabic feh
  "afii57403",            // 673       SP140007  U061B  Arabic semicolon
  "AS010000",             // 674       AS010000  UF8F6  Arabic seen (1st part) (a) - isolated-final
  "AS230000",             // 675       AS230000  UF8F5  Arabic sheen (1st part) (a) - isolated-final
  "AS450000",             // 676       AS450000  UF8F4  Arabic sad (1st part) (a) - isolated-final
  "afii57407",            // 677       SP150007  U061F  Arabic question mark
  "afii57409",            // 678       AX300000  U0621  Arabic hamza
  "afii57410",            // 679       AA210000  U0622  Arabic alef madda above
  "afii57411",            // 680       AA310000  U0623  Arabic alef hamza above
  "afii57412",            // 681       AW310000  U0624  Arabic waw hamza above
  "afii62804",            // 682       AC470002  UFECA  Arabic ain final form
  "afii57414",            // 683       AY310000  U0626  Arabic yeh hamza above
  "afii57415",            // 684       AA010000  U0627  Arabic alef
  "afii62761",            // 685       AB010003  UFE91  Arabic beh initial form
  "afii57417",            // 686       AT020000  U0629  Arabic teh marbuta
  "afii62765",            // 687       AT010003  UFE97  Arabic teh initial form
  "afii62768",            // 688       AT470003  UFE9B  Arabic theh initial form
  "afii62771",            // 689       AG230003  UFE9F  Arabic jeem initial form
  "afii62774",            // 690       AH450003  UFEA3  Arabic hah initial form
  "afii62777",            // 691       AH470003  UFEA7  Arabic khah initial form
  "afii57423",            // 692       AD010000  U062F  Arabic dal
  "afii57424",            // 693       AD470000  U0630  Arabic thal
  "afii57425",            // 694       AR010000  U0631  Arabic reh
  "afii57426",            // 695       AZ010000  U0632  Arabic zain
  "afii62784",            // 696       AS010003  UFEB3  Arabic seen initial form
  "afii62787",            // 697       AS230003  UFEB7  Arabic sheen initial form
  "afii62790",            // 698       AS450003  UFEBB  Arabic sad initial form
  "afii62794",            // 699       AD450000  UFEC0  Arabic dad medial form
  "afii57431",            // 700       AT450000  U0637  Arabic tah
  "afii57432",            // 701       AZ450000  U0638  Arabic zah
  "afii62802",            // 702       AC470003  UFECB  Arabic ain initial form
  "afii62805",            // 703       AG310003  UFECF  Arabic ghain initial form
  "afii57433",            // 704       AC470000  U0639  Arabic ain
  "afii57440",            // 705       SM860000  U0640  Arabic tatweel
  "afii62808",            // 706       AF010003  UFED3  Arabic feh initial form
  "afii62811",            // 707       AQ010003  UFED7  Arabic qaf initial form
  "afii62814",            // 708       AK010003  UFED8  Arabic qaf medial form
  "afii62817",            // 709       AL010003  UFEDF  Arabic lam initial form
  "afii62820",            // 710       AM010003  UFEE3  Arabic meem initial form
  "afii62823",            // 711       AN010003  UFEE7  Arabic noon initial form
  "afii62826",            // 712       AH010003  UFEEB  Arabic heh initial form
  "afii57448",            // 713       AW010000  U0648  Arabic waw
  "afii57449",            // 714       AA020000  U0649  Arabic alef maksura
  "afii62831",            // 715       AY010003  UFEF3  Arabic yeh initial form
  "AD450000",             // 716       AD450003  UF8F7  Arabic dud (1st part) (a) - isolated-final
  "afii62803",            // 717       AC470004  UFECC  Arabic ain medial form
  "afii62807",            // 718       AG310002  UFECE  Arabic ghain final form
  "afii57434",            // 719       AG310000  U063A  Arabic ghain
  "afii57445",            // 720       AM010000  U0645  Arabic meem
  "afii52396",            // 721       AX100004  UFE7D  Arabic shadda medial form
  "afii57457",            // 722       AX100000  U0651  Arabic shadda
  "afii57446",            // 723       AN010000  U0646  Arabic noon
  "afii57447",            // 724       AH010000  U0647  Arabic heh
  "afii62827",            // 725       AH010004  UFEEC  Arabic heh medial form
  "afii62830",            // 726       AA020002  UFEF0  Arabic alef maksura final form
  "afii62833",            // 727       AY010002  UFEF2  Arabic yeh final form
  "afii62806",            // 728       AG310004  UFED0  Arabic ghain medial form
  "afii57442",            // 729       AQ010000  U0642  Arabic qaf
  "afii62834",            // 730       AL220000  UFEF5  Arabic ligature lam alef madda above isolated form
  "afii62835",            // 731       AL220003  UFEF6  Arabic ligature lam alef madda above final form
  "afii57444",            // 732       AL010000  U0644  Arabic lam
  "afii57443",            // 733       AK010000  U0643  Arabic kaf
  "afii57450",            // 734       AY010000  U064A  Arabic yeh
  "afii57507",            // 735       AC210000  U0686  Arabic tcheh
  "afii57509",            // 736       AG010000  U06AF  Arabic gaf
  "afii57506",            // 737       AP010000  U067E  Arabic peh
  "afii57508",            // 738       AX210000  U0698  Arabic jeh
  "afii57413",            // 739       AA310400  U0625  Arabic alef hamza below
  "afii57451",            // 740       AA070000  U064B  Arabic fathatan
  "afii57452",            // 741       AU070000  U064C  Arabic dammatan
  "afii57453",            // 742       AI070000  U064D  Arabic kasratan isolated form
  "afii57454",            // 743       AA050000  U064E  Arabic fatha
  "afii57455",            // 744       AU050000  U064F  Arabic damma
  "afii57456",            // 745       AI050000  U0650  Arabic kasra
  "afii57458",            // 746       AE050000  U0652  Arabic sukun
  "afii57508",            // 747       AZ210000  U0698  Arabic jeh
  "lefttorightembed",     // 748       SP5700Z0  U202A  left-to-right embedding
  "righttoleftembed",     // 749       SP5800Z0  U202B  right-to-left embedding
  "popformatdirection",   // 750       SP5900Z0  U202C  pop directional formatting
  "lefttorightoverride",  // 751       SP6000Z0  U202D  left-to-right override
  "righttoleftoverride",  // 752       SP6100Z0  U202E  right-to-left override
  "SM580000",             // 753       SM580000  U25C5  large bullet
  "spacenumeric",         // 754       SP310000  U2007  numeric space
  "logo",                 // 755       SV640000  UF8FF  corporate logo
  "ff",                   // 756       LF510000  UFB00  Latin small ligature ff
  "fi",                   // 757       LF530000  UFB01  Latin small ligature fi
  "fl",                   // 758       LF550000  UFB02  Latin small ligature fl
  "ffi",                  // 759       LF570000  UFB03  Latin small ligature ffi
  "ffl",                  // 760       LF590000  UFB04  Latin small ligature ffl
  "ij",                   // 761       LI510000  U0133  Latin small ligature ij
  "integral",             // 762       SA510000  U222B  integral
  "oneeighth",            // 763       NF180000  U215B  fraction one eighth
  "threeeighths",         // 764       NF190000  U215C  fraction three eighths
  "fiveeighths",          // 765       NF200000  U215D  fraction five eighths
  "seveneighths",         // 766       NF210000  U215E  fraction seven eighths
  "zeroslash",            // 767       ND100008  UF8B3  zero slash
  /*
   * The OS/2 Warp ATM fonts do not contain the Japanese, Korean, or Thai
   * characters ranges of UGL.  If it is desirable to add them then the
   * following names should be enabled.
   */
#ifdef THAI_UGL
  "SP500000",             // 768       SP500000  U303F  ideographic half fill space
  "carriagereturn",       // 769       SM720000  U21B5  downwards arrow corner leftwards
  "periodjapan",          // 770       JQ700000  U3002  ideographic full stop
  "leftquotejapan",       // 771       JQ710000  U300C  left corner bracket
  "rightquotejapan",      // 772       JQ720000  U300D  right corner bracket
  "commajapan",           // 773       JQ730000  U3001  ideographic comma
  "middotkana",           // 774       JQ740000  U30FB  katakana middle dot
  "Wokana",               // 775       JW500000  U30F2  katakana wo
  "Akanasmall",           // 776       JA010000  U30A1  katakana small a
  "Ikanasmall",           // 777       JI010000  U30A3  katakana small i
  "Ukanasmall",           // 778       JU010000  U30A5  katakana small u
  "Ekanasmall",           // 779       JE010000  U30A7  katakana small e
  "Okanasmall",           // 780       JO010000  U30A9  katakana small o
  "Yakanasmall",          // 781       JY110000  U30E3  katakana small ya
  "Yukanasmall",          // 782       JY310000  U30E5  katakana small yu
  "Yokanasmall",          // 783       JY510000  U30E7  katakana small yo
  "Tukanasmall",          // 784       JT310000  U30C3  katakana small tu
  "Longsoundkana",        // 785       JX700000  U30FC  katakana-hiragana prolonged sound mark
  "Akana",                // 786       JA000000  U30A2  katakana a
  "Ikana",                // 787       JI000000  U30A4  katakana i
  "Ukana",                // 788       JU000000  U30A6  katakana u
  "Ekana",                // 789       JE000000  U30A8  katakana e
  "Okana",                // 790       JO000000  U30AA  katakana o
  "Kakana",               // 791       JK100000  U30AB  katakana ka
  "Kikana",               // 792       JK200000  U30AD  katakana ki
  "Kukana",               // 793       JK300000  U30AF  katakana ku
  "Kekana",               // 794       JK400000  U30B1  katakana ke
  "Kokana",               // 795       JK500000  U30B3  katakana ko
  "Sakana",               // 796       JS100000  U30B5  katakana sa
  "Sikana",               // 797       JS200000  U30B7  katakana si
  "Sukana",               // 798       JS300000  U30B9  katakana su
  "Sekana",               // 799       JS400000  U30BB  katakana se
  "Sokana",               // 800       JS500000  U30BD  katakana so
  "Takana",               // 801       JT100000  U30BF  katakana ta
  "Tikana",               // 802       JT200000  U30C1  katakana ti
  "Tukana",               // 803       JT300000  U30C4  katakana tu
  "Tekana",               // 804       JT400000  U30C6  katakana te
  "Tokana",               // 805       JT500000  U30C8  katakana to
  "Nakana",               // 806       JN100000  U30CA  katakana na
  "Nikana",               // 807       JN200000  U30CB  katakana ni
  "Nukana",               // 808       JN300000  U30CC  katakana nu
  "Nekana",               // 809       JN400000  U30CD  katakana ne
  "Nokana",               // 810       JN500000  U30CE  katakana no
  "Hakana",               // 811       JH100000  U30CF  katakana ha
  "Hikana",               // 812       JH200000  U30D2  katakana hi
  "Hukana",               // 813       JH300000  U30D5  katakana hu
  "Hekana",               // 814       JH400000  U30D8  katakana he
  "Hokana",               // 815       JH500000  U30DB  katakana ho
  "Makana",               // 816       JM100000  U30DE  katakana ma
  "Mikana",               // 817       JM200000  U30DF  katakana mi
  "Mukana",               // 818       JM300000  U30E0  katakana mu
  "Mekana",               // 819       JM400000  U30E1  katakana me
  "Mokana",               // 820       JM500000  U30E2  katakana mo
  "Yakana",               // 821       JY100000  U30E4  katakana ya
  "Yukana",               // 822       JY300000  U30E6  katakana yu
  "Yokana",               // 823       JY500000  U30E8  katakana yo
  "Rakana",               // 824       JR100000  U30E9  katakana ra
  "Rikana",               // 825       JR200000  U30EA  katakana ri
  "Rukana",               // 826       JR300000  U30EB  katakana ru
  "Rekana",               // 827       JR400000  U30EC  katakana re
  "Rokana",               // 828       JR500000  U30ED  katakana ro
  "Wakana",               // 829       JW100000  U30EF  katakana wa
  "Nkana",                // 830       JN000000  U30F3  katakana n
  "Voicedjapan",          // 831       JX710000  U309B  katakana-hiragana voiced sound mark
  "Semivoicedjapan",      // 832       JX720000  U309C  katakana-hiragana semi-voiced sound mark
  "ahiraganasmall",       // 833       RA010000  U3041  hiragana small a
  "ihiraganasmall",       // 834       RI010000  U3043  hiragana small i
  "uhiraganasmall",       // 835       RU010000  U3045  hiragana small u
  "ehiraganasmall",       // 836       RE010000  U3047  hiragana small e
  "ohiraganasmall",       // 837       RO010000  U3049  hiragana small o
  "yahiraganasmall",      // 838       RY110000  U3083  hiragana small ya
  "yuhiraganasmall",      // 839       RY310000  U3085  hiragana small yu
  "ohiraganasmall",       // 840       RO010000  U3049  hiragana small o
  "tuhiraganasmall",      // 841       RT310000  U3063  hiragana small tu
  "ahiragana",            // 842       RA000000  U3042  hiragana a
  "ihiragana",            // 843       RI000000  U3044  hiragana i
  "uhiragana",            // 844       RU000000  U3046  hiragana u
  "ehiragana",            // 845       RE000000  U3048  hiragana e
  "ohiragana",            // 846       RO000000  U304A  hiragana o
  "kahiragana",           // 847       RK100000  U304B  hiragana ka
  "kihiragana",           // 848       RK200000  U304D  hiragana ki
  "kuhiragana",           // 849       RK300000  U304F  hiragana ku
  "kehiragana",           // 850       RK400000  U3051  hiragana ke
  "kohiragana",           // 851       RK500000  U3053  hiragana ko
  "sahiragana",           // 852       RS100000  U3055  hiragana sa
  "sihiragana",           // 853       RS200000  U3057  hiragana si
  "suhiragana",           // 854       RS300000  U3059  hiragana su
  "sehiragana",           // 855       RS400000  U305B  hiragana se
  "sohiragana",           // 856       RS500000  U305D  hiragana so
  "tahiragana",           // 857       RT100000  U305F  hiragana ta
  "tihiragana",           // 858       RT200000  U3061  hiragana ti
  "tuhiragana",           // 859       RT300000  U3064  hiragana tu
  "tehiragana",           // 860       RT400000  U3066  hiragana te
  "tohiragana",           // 861       RT500000  U3068  hiragana to
  "nahiragana",           // 862       RN100000  U306A  hiragana na
  "nihiragana",           // 863       RN200000  U306B  hiragana ni
  "nuhiragana",           // 864       RN300000  U306C  hiragana nu
  "nehiragana",           // 865       RN400000  U306D  hiragana ne
  "nohiragana",           // 866       RN500000  U306E  hiragana no
  "hahiragana",           // 867       RH100000  U306F  hiragana ha
  "hihiragana",           // 868       RH200000  U3072  hiragana hi
  "huhiragana",           // 869       RH300000  U3075  hiragana hu
  "hehiragana",           // 870       RH400000  U3078  hiragana he
  "hohiragana",           // 871       RH500000  U307B  hiragana ho
  "mahiragana",           // 872       RM100000  U307E  hiragana ma
  "mihiragana",           // 873       RM200000  U307F  hiragana mi
  "muhiragana",           // 874       RM300000  U3080  hiragana mu
  "mehiragana",           // 875       RM400000  U3081  hiragana me
  "mohiragana",           // 876       RM500000  U3082  hiragana mo
  "yahiragana",           // 877       RY100000  U3084  hiragana ya
  "yuhiragana",           // 878       RY300000  U3086  hiragana yu
  "yohiragana",           // 879       RY500000  U3088  hiragana yo
  "rahiragana",           // 880       RR100000  U3089  hiragana ra
  "rihiragana",           // 881       RR200000  U308A  hiragana ri
  "ruhiragana",           // 882       RR300000  U308B  hiragana ru
  "rehiragana",           // 883       RR400000  U308C  hiragana re
  "rohiragana",           // 884       RR500000  U308D  hiragana ro
  "wahiragana",           // 885       RW100000  U308F  hiragana wa
  "wohiragana",           // 886       RW500000  U3092  hiragana wo
  "nhiragana",            // 887       RN000000  U3093  hiragana n
  "leftquoteopenjapan",   // 888       JQ710001  U300E  left white corner bracket
  "rightquoteopenjapan",  // 889       JQ720001  U300F  right white corner bracket
  "kekanasmall",          // 890       JK410000  U30F6  katakana small ke
  "iterate",              // 891       SS770000  U3005  ideographic iteration mark
  ".notdef",              // 892       -
  ".notdef",              // 893       -
  ".notdef",              // 894       -
  "Won",                  // 895       SC140000  U20A9  won sign
  "Giyeog",               // 896       OG000000  U3131  hangul kiyeok
  "Ssanggiyeog",          // 897       OG100000  U3132  hangul ssangkiyeok
  "Giyeogsios",           // 898       OG200000  U3133  hangul kiyeok-sios
  "Nieun",                // 899       ON000000  U3134  hangul nieun
  "Nieunjieuj",           // 900       ON150000  U3135  hangul nieun-cieuc
  "Nieunhieuh",           // 901       ON100000  U3136  hangul nieun-hieuh
  "Digeud",               // 902       OD000000  U3137  hangul tikeut
  "Ssangdigeud",          // 903       OD100000  U3138  hangul ssangtikeut
  "Lieul",                // 904       OL000000  U3139  hangul rieul
  "Lieulgiyeog",          // 905       OL200000  U313A  hangul rieul-kiyeok
  "Lieulmieum",           // 906       OL400000  U313B  hangul rieul-mieum
  "Lieulbieub",           // 907       OL100000  U313C  hangul rieul-pieup
  "Lieulsios",            // 908       OL600000  U313D  hangul rieul-sios
  "Lieultieut",           // 909       OL700000  U313E  hangul rieul-thieuth
  "Lieulpieup",           // 910       OL500000  U313F  hangul rieul-phieuph
  "Lieulhieuh",           // 911       OL300000  U3140  hangul rieul-hieuh
  "Mieum",                // 912       OM000000  U3141  hangul mieum
  "Bieub",                // 913       OB000000  U3142  hangul pieup
  "Ssangbieub",           // 914       OB100000  U3143  hangul ssangpieup
  "Bieubsios",            // 915       OB200000  U3144  hangul pieup-sios
  "Sios",                 // 916       OS000000  U3145  hangul sios
  "Ssangsios",            // 917       OS100000  U3146  hangul ssangsios
  "Ieung",                // 918       ON200000  U3147  hangul ieung
  "Jieuj",                // 919       OJ000000  U3148  hangul cieuc
  "Ssangjieuj",           // 920       OJ100000  U3149  hangul ssangcieuc
  "Cieuc",                // 921       OC200000  U314A  hangul chieuch
  "Kiyeok",               // 922       OK000000  U314B  hangul khieukh
  "Tieut",                // 923       OT000000  U314C  hangul thieuth
  "Pieup",                // 924       OP000000  U314D  hangul phieuph
  "Hieuh",                // 925       OH000000  U314E  hangul hieuh
  "Ahangul",              // 926       OA000000  U314F  hangul a
  "Aehangul",             // 927       OA200000  U3150  hangul ae
  "Yahangul",             // 928       OY200000  U3151  hangul ya
  "Yaehangul",            // 929       OY250000  U3152  hangul yae
  "Eohangul",             // 930       OE200000  U3153  hangul eo
  "Ehangul",              // 931       OE000000  U3154  hangul e
  "Yeohangul",            // 932       OY400000  U3155  hangul yeo
  "Yehangul",             // 933       OY300000  U3156  hangul ye
  "Ohangul",              // 934       OO000000  U3157  hangul o
  "Wahangul",             // 935       OO100000  U3158  hangul wa
  "Waehangul",            // 936       OO200000  U3159  hangul wae
  "Oehangul",             // 937       OO300000  U315A  hangul oe
  "Yohangul",             // 938       OY500000  U315B  hangul yo
  "Uhangul",              // 939       OU000000  U315C  hangul u
  "Weohangul",            // 940       OU300000  U315D  hangul weo
  "Wehangul",             // 941       OU200000  U315E  hangul we
  "Wihangul",             // 942       OU400000  U315F  hangul wi
  "Yuhangul",             // 943       OY600000  U3160  hangul yu
  "Euhangul",             // 944       OE300000  U3161  hangul eu
  "Yihangul",             // 945       OE400000  U3162  hangul yi
  "Ihangul",              // 946       OI000000  U3163  hangul i
  "Won",                  // 947       SC140000  U20A9  won sign
  ".notdef",              // 948
  "Caeomhangul",          // 949       SP490000  U3164  hangul filler
  "U0E3F",                // 950       SC130000  U0E3F  currency symbol baht
  "U0E01",                // 951       BK100000  U0E01  thai character ko kai
  "U0E02",                // 952       BK200000  U0E02  thai character kho khai
  "U0E03",                // 953       BK300000  U0E03  thai character kho khuat
  "U0E04",                // 954       BK400000  U0E04  thai character kho khwai
  "U0E05",                // 955       BK500000  U0E05  thai character kho khon
  "U0E06",                // 956       BK600000  U0E06  thai character kho rakhang
  "U0E07",                // 957       BN100000  U0E07  thai character ngo ngu
  "U0E08",                // 958       BC100000  U0E08  thai character cho chan
  "U0E09",                // 959       BX100000  U0E09  thai character cho ching
  "U0E0A",                // 960       BS100000  U0E0A  thai character cho chang
  "U0E0B",                // 961       BX200000  U0E0B  thai character so so
  "U0E0C",                // 962       BX300000  U0E0C  thai character cho choe
  "U0E0D",                // 963       BY100000  U0E0D  thai character yo ying
  "U0E0E",                // 964       BD100000  U0E0E  thai character do chada
  "U0E0F",                // 965       BT100000  U0E0F  thai character to patak
  "U0E10",                // 966       BT200000  U0E10  thai character tho than
  "U0E11",                // 967       BT300000  U0E11  thai character tho nangmontho
  "U0E12",                // 968       BT400000  U0E12  thai character tho phuthao
  "U0E13",                // 969       BN200000  U0E13  thai character no nen
  "U0E14",                // 970       BD200000  U0E14  thai character do dek
  "U0E15",                // 971       BT500000  U0E15  thai character to tao
  "U0E16",                // 972       BT600000  U0E16  thai character tho thung
  "U0E17",                // 973       BT700000  U0E17  thai character tho thahan
  "U0E18",                // 974       BT800000  U0E18  thai character tho thong
  "U0E19",                // 975       BN300000  U0E19  thai character no nu
  "U0E1A",                // 976       BB100000  U0E1A  thai character bo baimai
  "U0E1B",                // 977       BP100000  U0E1B  thai character po pla
  "U0E1C",                // 978       BP200000  U0E1C  thai character pho phung
  "U0E1D",                // 979       BF100000  U0E1D  thai character fo fa
  "U0E1E",                // 980       BP300000  U0E1E  thai character pho phan
  "U0E1F",                // 981       BF200000  U0E1F  thai character fo fan
  "U0E20",                // 982       BP400000  U0E20  thai character pho samphao
  "U0E21",                // 983       BM100000  U0E21  thai character mo ma
  "U0E22",                // 984       BY200000  U0E22  thai character yo yak
  "U0E23",                // 985       BR100000  U0E23  thai character ro rua
  "U0E24",                // 986       BR200000  U0E24  thai character ru
  "U0E25",                // 987       BL100000  U0E25  thai character lo ling
  "U0E26",                // 988       BL200000  U0E26  thai character lu
  "U0E27",                // 989       BW100000  U0E27  thai character wo waen
  "U0E28",                // 990       BS200000  U0E28  thai character so sala
  "U0E29",                // 991       BS300000  U0E29  thai character so rusi
  "U0E2A",                // 992       BS400000  U0E2A  thai character so sua
  "U0E2B",                // 993       BH100000  U0E2B  thai character ho hip
  "U0E2C",                // 994       BL300000  U0E2C  thai character lo chula
  "U0E2D",                // 995       BO100000  U0E2D  thai character o ang
  "U0E2E",                // 996       BH200000  U0E2E  thai character ho nokhuk
  "U0E2F",                // 997       BQ200000  U0E2F  thai character paiyannoi
  "U0E30",                // 998       BA200000  U0E30  thai character sara a
  "U0E31",                // 999       BA100000  U0E31  thai character mai han-akat
  "U0E32",                // 1000      BA300000  U0E32  thai character sara aa
  "U0E33",                // 1001      BA400000  U0E33  thai character sara am
  "U0E34",                // 1002      BI100000  U0E34  thai character sara i
  "U0E35",                // 1003      BI200000  U0E35  thai character sara ii
  "U0E36",                // 1004      BU100000  U0E36  thai character sara ue
  "U0E37",                // 1005      BU200000  U0E37  thai character sara uee
  "U0E38",                // 1006      BU300000  U0E38  thai character sara u
  "U0E39",                // 1007      BU400000  U0E39  thai character sara uu
  "U0E3A",                // 1008      BQ300000  U0E3A  thai character phinthu
  "U0E40",                // 1009      BE200000  U0E40  thai character sara e
  "U0E41",                // 1010      BE300000  U0E41  thai character sara ae
  "U0E42",                // 1011      BO200000  U0E42  thai character sara o
  "U0E43",                // 1012      BA500000  U0E43  thai character sara ai maimuan
  "U0E44",                // 1013      BA600000  U0E44  thai character sara ai maimalai
  "U0E45",                // 1014      BA700000  U0E45  thai character lakkhangyao
  "U0E46",                // 1015      BQ100000  U0E46  thai character maiyamok
  "U0E47",                // 1016      BE100000  U0E47  thai character maitaikhu
  "U0E48",                // 1017      BZ100000  U0E48  thai character mai ek
  "U0E49",                // 1018      BZ200000  U0E49  thai character mai tho
  "U0E4A",                // 1019      BZ300000  U0E4A  thai character mai tri
  "U0E4B",                // 1020      BZ400000  U0E4B  thai character mai chattawa
  "U0E4C",                // 1021      BZ500000  U0E4C  thai character thanthakhat
  "U0E4D",                // 1022      BN400000  U0E4D  thai character nikhahit
  "U0E4E",                // 1023      BE400000  U0E4E  thai character yamakkan
  "U0E4F",                // 1024      BQ400000  U0E4F  thai character fongman
  "U0E5A",                // 1025      BQ500000  U0E5A  thai character angkhankhu
  "U0E5B",                // 1026      BQ600000  U0E5B  thai character khomut
  "U0E50",                // 1027      ND100002  U0E50  thai digit zero
  "U0E51",                // 1028      ND010002  U0E51  thai digit one
  "U0E52",                // 1029      ND020002  U0E52  thai digit two
  "U0E53",                // 1030      ND030002  U0E53  thai digit three
  "U0E54",                // 1031      ND040002  U0E54  thai digit four
  "U0E55",                // 1032      ND050002  U0E55  thai digit five
  "U0E56",                // 1033      ND060002  U0E56  thai digit six
  "U0E57",                // 1034      ND070002  U0E57  thai digit seven
  "U0E58",                // 1035      ND080002  U0E58  thai digit eight
  "U0E59",                // 1036      ND090002  U0E59  thai digit nine
  "TH874001",             // 1037      BK100001  U2E01  NOTUNI
  "TH874002",             // 1038      BK100002  U2E02  NOTUNI
  "TH874003",             // 1039      BK100003  U2E03  NOTUNI
  "TH874004",             // 1040      BK100004  U2E04  NOTUNI
  "TH874005",             // 1041      BK100005  U2E05  NOTUNI
  "TH874006",             // 1042      BK100006  U2E06  NOTUNI
  "TH874007",             // 1043      BK100007  U2E07  NOTUNI
  "TH874008",             // 1044      BK100008  U2E08  NOTUNI
  "TH874009",             // 1045      BK100009  U2E09  NOTUNI
  "TH874010",             // 1046      BK100010  U2E0A  NOTUNI
  "TH874011",             // 1047      BK100011  U2E0B  NOTUNI
  "TH874012",             // 1048      BK100012  U2E0C  NOTUNI
  "TH874013",             // 1049      BK100013  U2E0D  NOTUNI
  "TH874014",             // 1050      BK100014  U2E0E  NOTUNI
  "TH874015",             // 1051      BK100015  U2E0F  NOTUNI
  "TH874016",             // 1052      BK100016  U2E10  NOTUNI
  "TH874017",             // 1053      BK100017  U2E11  NOTUNI
  "TH874018",             // 1054      BK100018  U2E12  NOTUNI
  "TH874019",             // 1055      BK100019  U2E13  NOTUNI
  "TH874020",             // 1056      BK100020  U2E14  NOTUNI
  "TH874021",             // 1057      BK100021  U2E15  NOTUNI
  "TH874022",             // 1058      BK100022  U2E16  NOTUNI
  "TH874023",             // 1059      BK100023  U2E17  NOTUNI
  "TH874024",             // 1060      BK100024  U2E18  NOTUNI
  "TH874025",             // 1061      BK100025  U2E19  NOTUNI
  "TH874026",             // 1062      BK100026  U2E1A  NOTUNI
  "TH874027",             // 1063      BK100027  U2E1B  NOTUNI
  "TH874028",             // 1064      BK100028  U2E1C  NOTUNI
  "TH874029",             // 1065      BK100029  U2E1D  NOTUNI
  "TH874030",             // 1066      BK100030  U2E1E  NOTUNI
  "TH874031",             // 1067      BK100031  U2E1F  NOTUNI
  "TH874127",             // 1068      BK100127  U2E7F  NOTUNI
  "TH874128",             // 1069      BK100128  U2E80  NOTUNI
  "TH874129",             // 1070      BK100129  U2E81  NOTUNI
  "TH874130",             // 1071      BK100130  U2E82  NOTUNI
  "TH874131",             // 1072      BK100131  U2E83  NOTUNI
  "TH874132",             // 1073      BK100132  U2E84  NOTUNI
  "TH874133",             // 1074      BK100133  U2E85  NOTUNI
  "TH874134",             // 1075      BK100134  U2E86  NOTUNI
  "TH874135",             // 1076      BK100135  U2E87  NOTUNI
  "TH874136",             // 1077      BK100136  U2E88  NOTUNI
  "TH874137",             // 1078      BK100137  U2E89  NOTUNI
  "TH874138",             // 1079      BK100138  U2E8A  NOTUNI
  "TH874139",             // 1080      BK100139  U2E8B  NOTUNI
  "TH874140",             // 1081      BK100140  U2E8C  NOTUNI
  "TH874141",             // 1082      BK100141  U2E8D  NOTUNI
  "TH874142",             // 1083      BK100142  U2E8E  NOTUNI
  "TH874143",             // 1084      BK100143  U2E8F  NOTUNI
  "TH874144",             // 1085      BK100144  U2E90  NOTUNI
  "TH874145",             // 1086      BK100145  U2E91  NOTUNI
  "TH874146",             // 1087      BK100146  U2E92  NOTUNI
  "TH874147",             // 1088      BK100147  U2E93  NOTUNI
  "TH874148",             // 1089      BK100148  U2E94  NOTUNI
  "TH874149",             // 1090      BK100149  U2E95  NOTUNI
  "TH874150",             // 1091      BK100150  U2E96  NOTUNI
  "TH874151",             // 1092      BK100151  U2E97  NOTUNI
  "TH874152",             // 1093      BK100152  U2E98  NOTUNI
  "TH874153",             // 1094      BK100153  U2E99  NOTUNI
  "TH874154",             // 1095      BK100154  U2E9A  NOTUNI
  "TH874155",             // 1096      BK100155  U2E9B  NOTUNI
  "TH874156",             // 1097      BK100156  U2E9C  NOTUNI
  "TH874157",             // 1098      BK100157  U2E9D  NOTUNI
  "TH874158",             // 1099      BK100158  U2E9E  NOTUNI
  "TH874159",             // 1100      BK100159  U2E9F  NOTUNI
  "TH874160",             // 1101      BZ100300  U2EA0  NOTUNI
  "TH874219",             // 1102      BZ200300  U2EDB  NOTUNI
  "TH874220",             // 1103      BZ300300  U2EDC  NOTUNI
  "TH874221",             // 1104      BZ400300  U2EDD  NOTUNI
  "TH874222",             // 1105      BZ500300  U2EDE  NOTUNI
#endif      /* THAI_UGL */
  NULL };
