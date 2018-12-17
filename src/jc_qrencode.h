// references for the Reed-Solomon error correction
// https://en.wikiversity.org/wiki/Reed%E2%80%93Solomon_codes_for_coders
// https://github.com/qsantos/qrcode/blob/master/rs.c
// https://github.com/kazuhikoarase/qrcode-generator/blob/master/java/src/java/com/d_project/qrcode/QRUtil.java



#ifndef JC_QRENCODE_H
#define JC_QRENCODE_H

#include <stdint.h>

#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) &&     !defined(__ILP32__) ) || defined(_M_X64) || defined(__ia64) || defined (_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
    #define JC_QRE_IS64BIT 1
    #define JC_QRE_PAD(_X_) uint8_t _pad[_X_]
#else
    #define JC_QRE_PAD(_X_)
#endif

const static uint32_t JC_QRE_ERROR_CORRECTION_LEVEL_LOW        = 0;
const static uint32_t JC_QRE_ERROR_CORRECTION_LEVEL_MEDIUM     = 1;
const static uint32_t JC_QRE_ERROR_CORRECTION_LEVEL_QUARTILE   = 2;
const static uint32_t JC_QRE_ERROR_CORRECTION_LEVEL_HIGH       = 3;

typedef struct _JCQRCode
{
    uint8_t* data;      // The modules of the qrcode
    uint32_t size;      // Size (in modules) of one side of the qrcode
    uint32_t version;   // Version [1,40]
    uint32_t ecl;       // Error correction level
    uint32_t _pad;
} JCQRCode;

/**
*/
JCQRCode* jc_qrencode(const uint8_t* input, uint32_t inputlength, uint32_t ecl);


#if defined(JC_QRENCODE_IMPLEMENTATION)

static void print_bits(const char* tag, uint32_t v, uint32_t count)
{
    printf("%s 0b", tag);
    for( int32_t i = 0; i < count; ++i)
    {
        printf("%u", (v >> (count - 1 - i)) & 1);
        if( i == 31 )
        {
            printf(" ...");
            break;
        }
    }
    printf("\n");
}


static const uint32_t JC_QRE_MIN_VERSION = 1;
static const uint32_t JC_QRE_MAX_VERSION = 40;

static const uint8_t JC_QRE_INPUT_TYPE_NUMERIC         = 0;
static const uint8_t JC_QRE_INPUT_TYPE_ALPHANUMERIC    = 1;
static const uint8_t JC_QRE_INPUT_TYPE_BYTE            = 2;
static const uint8_t JC_QRE_INPUT_TYPE_KANJI           = 3;


static uint32_t JC_QRE_TYPE_BITS[] = {1, 2, 4, 8};

static int32_t JC_QRE_ALPHANUMERIC_MAPPINGS[] = {
// ' ',  !,  ",  #,  $,  %,  &,  ',  (,  ),  *,  +,  ,,  -,  .,  /,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  :,  ;,  <,  =,  >,  ?,  @,  A,  B,  C,  D,  E,  F,  G,  H,  I,  J,  K,  L,  M,  N,  O,  P,  Q,  R,  S,  T,  U,  V,  W,  X,  Y,  Z
    36, -1, -1, -1, 37, 38, -1, -1, -1, -1, 39, 40, -1, 41, 42, 43,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 44, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
};

// Each array has 4 levels of error correction, and one extra (first) element in each
#define JC_QRE_INDEX(_ECL, _I) ((_ECL)*(JC_QRE_MAX_VERSION+1) + (_I))

static uint32_t JC_QRE_DATA_CODEWORD_COUNT[] = {
    0,   19,   34,   55,   80,  108,  136,  156,  194,  232,  274,  324,  370,  428,  461,  523,  589,  647,  721,  795,  861,  932, 1006, 1094, 1174, 1276, 1370, 1468, 1531, 1631, 1735, 1843, 1955, 2071, 2191, 2306, 2434, 2566, 2702, 2812, 2956,
    0,   16,   28,   44,   64,   86,  108,  124,  154,  182,  216,  254,  290,  334,  365,  415,  453,  507,  563,  627,  669,  714,  782,  860,  914, 1000, 1062, 1128, 1193, 1267, 1373, 1455, 1541, 1631, 1725, 1812, 1914, 1992, 2102, 2216, 2334,
    0,   13,   22,   34,   48,   62,   76,   88,  110,  132,  154,  180,  206,  244,  261,  295,  325,  367,  397,  445,  485,  512,  568,  614,  664,  718,  754,  808,  871,  911,  985, 1033, 1115, 1171, 1231, 1286, 1354, 1426, 1502, 1582, 1666,
    0,    9,   16,   26,   36,   46,   60,   66,   86,  100,  122,  140,  158,  180,  197,  223,  253,  283,  313,  341,  385,  406,  442,  464,  514,  538,  596,  628,  661,  701,  745,  793,  845,  901,  961,  986, 1054, 1096, 1142, 1222, 1276,
};
static const uint32_t JC_QRE_ERROR_CORRECTION_CODEWORD_COUNT[] = {
       0,    7,   10,   15,   20,   26,   18,   20,   24,   30,   18,   20,   24,   26,   30,   22,   24,   28,   30,   28,   28,   28,   28,   30,   30,   26,   28,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,
       0,   10,   16,   26,   18,   24,   16,   18,   22,   22,   26,   30,   22,   22,   24,   24,   28,   28,   26,   26,   26,   26,   28,   28,   28,   28,   28,   28,   28,   28,   28,   28,   28,   28,   28,   28,   28,   28,   28,   28,   28,
       0,   13,   22,   18,   26,   18,   24,   18,   22,   20,   24,   28,   26,   24,   20,   30,   24,   28,   28,   26,   30,   28,   30,   30,   30,   30,   28,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,
       0,   17,   28,   22,   16,   22,   28,   26,   26,   24,   28,   24,   28,   22,   24,   24,   30,   28,   28,   26,   28,   30,   24,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,
};
static const uint32_t JC_QRE_CAPACITY_NUMERIC[] = {
       0,   41,   77,  127,  187,  255,  322,  370,  461,  552,  652,  772,  883, 1022, 1101, 1250, 1408, 1548, 1725, 1903, 2061, 2232, 2409, 2620, 2812, 3057, 3283, 3517, 3669, 3909, 4158, 4417, 4686, 4965, 5253, 5529, 5836, 6153, 6479, 6743, 7089,
       0,   34,   63,  101,  149,  202,  255,  293,  365,  432,  513,  604,  691,  796,  871,  991, 1082, 1212, 1346, 1500, 1600, 1708, 1872, 2059, 2188, 2395, 2544, 2701, 2857, 3035, 3289, 3486, 3693, 3909, 4134, 4343, 4588, 4775, 5039, 5313, 5596,
       0,   27,   48,   77,  111,  144,  178,  207,  259,  312,  364,  427,  489,  580,  621,  703,  775,  876,  948, 1063, 1159, 1224, 1358, 1468, 1588, 1718, 1804, 1933, 2085, 2181, 2358, 2473, 2670, 2805, 2949, 3081, 3244, 3417, 3599, 3791, 3993,
       0,   17,   34,   58,   82,  106,  139,  154,  202,  235,  288,  331,  374,  427,  468,  530,  602,  674,  746,  813,  919,  969, 1056, 1108, 1228, 1286, 1425, 1501, 1581, 1677, 1782, 1897, 2022, 2157, 2301, 2361, 2524, 2625, 2735, 2927, 3057,
};
static const uint32_t JC_QRE_CAPACITY_ALPHANUMERIC[] = {
       0,   25,   47,   77,  114,  154,  195,  224,  279,  335,  395,  468,  535,  619,  667,  758,  854,  938, 1046, 1153, 1249, 1352, 1460, 1588, 1704, 1853, 1990, 2132, 2223, 2369, 2520, 2677, 2840, 3009, 3183, 3351, 3537, 3729, 3927, 4087, 4296,
       0,   20,   38,   61,   90,  122,  154,  178,  221,  262,  311,  366,  419,  483,  528,  600,  656,  734,  816,  909,  970, 1035, 1134, 1248, 1326, 1451, 1542, 1637, 1732, 1839, 1994, 2113, 2238, 2369, 2506, 2632, 2780, 2894, 3054, 3220, 3391,
       0,   16,   29,   47,   67,   87,  108,  125,  157,  189,  221,  259,  296,  352,  376,  426,  470,  531,  574,  644,  702,  742,  823,  890,  963, 1041, 1094, 1172, 1263, 1322, 1429, 1499, 1618, 1700, 1787, 1867, 1966, 2071, 2181, 2298, 2420,
       0,   10,   20,   35,   50,   64,   84,   93,  122,  143,  174,  200,  227,  259,  283,  321,  365,  408,  452,  493,  557,  587,  640,  672,  744,  779,  864,  910,  958, 1016, 1080, 1150, 1226, 1307, 1394, 1431, 1530, 1591, 1658, 1774, 1852,
};
static const uint32_t JC_QRE_CAPACITY_BYTE[] = {
       0,   17,   32,   53,   78,  106,  134,  154,  192,  230,  271,  321,  367,  425,  458,  520,  586,  644,  718,  792,  858,  929, 1003, 1091, 1171, 1273, 1367, 1465, 1528, 1628, 1732, 1840, 1952, 2068, 2188, 2303, 2431, 2563, 2699, 2809, 2953,
       0,   14,   26,   42,   62,   84,  106,  122,  152,  180,  213,  251,  287,  331,  362,  412,  450,  504,  560,  624,  666,  711,  779,  857,  911,  997, 1059, 1125, 1190, 1264, 1370, 1452, 1538, 1628, 1722, 1809, 1911, 1989, 2099, 2213, 2331,
       0,   11,   20,   32,   46,   60,   74,   86,  108,  130,  151,  177,  203,  241,  258,  292,  322,  364,  394,  442,  482,  509,  565,  611,  661,  715,  751,  805,  868,  908,  982, 1030, 1112, 1168, 1228, 1283, 1351, 1423, 1499, 1579, 1663,
       0,    7,   14,   24,   34,   44,   58,   64,   84,   98,  119,  137,  155,  177,  194,  220,  250,  280,  310,  338,  382,  403,  439,  461,  511,  535,  593,  625,  658,  698,  742,  790,  842,  898,  958,  983, 1051, 1093, 1139, 1219, 1273,
};
static const uint32_t JC_QRE_CAPACITY_KANJI[] = {
       0,   10,   20,   32,   48,   65,   82,   95,  118,  141,  167,  198,  226,  262,  282,  320,  361,  397,  442,  488,  528,  572,  618,  672,  721,  784,  842,  902,  940, 1002, 1066, 1132, 1201, 1273, 1347, 1417, 1496, 1577, 1661, 1729, 1817,
       0,    8,   16,   26,   38,   52,   65,   75,   93,  111,  131,  155,  177,  204,  223,  254,  277,  310,  345,  384,  410,  438,  480,  528,  561,  614,  652,  692,  732,  778,  843,  894,  947, 1002, 1060, 1113, 1176, 1224, 1292, 1362, 1435,
       0,    7,   12,   20,   28,   37,   45,   53,   66,   80,   93,  109,  125,  149,  159,  180,  198,  224,  243,  272,  297,  314,  348,  376,  407,  440,  462,  496,  534,  559,  604,  634,  684,  719,  756,  790,  832,  876,  923,  972, 1024,
       0,    4,    8,   15,   21,   27,   36,   39,   52,   60,   74,   85,   96,  109,  120,  136,  154,  173,  191,  208,  235,  248,  270,  284,  315,  330,  365,  385,  405,  430,  457,  486,  518,  553,  590,  605,  647,  673,  701,  750,  784,
};
static const uint32_t JC_QRE_CHARACTER_COUNT_BIT_SIZE[] = {
       0,   10,   10,   10,   10,   10,   10,   10,   10,   10,   12,   12,   12,   12,   12,   12,   12,   12,   12,   12,   12,   12,   12,   12,   12,   12,   12,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,   14,
       0,    9,    9,    9,    9,    9,    9,    9,    9,    9,   11,   11,   11,   11,   11,   11,   11,   11,   11,   11,   11,   11,   11,   11,   11,   11,   11,   13,   13,   13,   13,   13,   13,   13,   13,   13,   13,   13,   13,   13,   13,
       0,    8,    8,    8,    8,    8,    8,    8,    8,    8,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,   16,
       0,    8,    8,    8,    8,    8,    8,    8,    8,    8,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   10,   12,   12,   12,   12,   12,   12,   12,   12,   12,   12,   12,   12,   12,   12,
};
static const uint32_t JC_QRE_BLOCK_COUNT_GROUP1[] = {
       0,    1,    1,    1,    1,    1,    2,    2,    2,    2,    2,    4,    2,    4,    3,    5,    5,    1,    5,    3,    3,    4,    2,    4,    6,    8,   10,    8,    3,    7,    5,   13,   17,   17,   13,   12,    6,   17,    4,   20,   19,
       0,    1,    1,    1,    2,    2,    4,    4,    2,    3,    4,    1,    6,    8,    4,    5,    7,   10,    9,    3,    3,   17,   17,    4,    6,    8,   19,   22,    3,   21,   19,    2,   10,   14,   14,   12,    6,   29,   13,   40,   18,
       0,    1,    1,    2,    2,    2,    4,    2,    4,    4,    6,    4,    4,    8,   11,    5,   15,    1,   17,   17,   15,   17,    7,   11,   11,    7,   28,    8,    4,    1,   15,   42,   10,   29,   44,   39,   46,   49,   48,   43,   34,
       0,    1,    1,    2,    4,    2,    4,    4,    4,    4,    6,    3,    7,   12,   11,   11,    3,    2,    2,    9,   15,   19,   34,   16,   30,   22,   33,   12,   11,   19,   23,   23,   19,   11,   59,   22,    2,   24,   42,   10,   20,
};
static const uint32_t JC_QRE_BLOCK_COUNT_GROUP2[] = {
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    2,    0,    2,    0,    1,    1,    1,    5,    1,    4,    5,    4,    7,    5,    4,    4,    2,    4,   10,    7,   10,    3,    0,    1,    6,    7,   14,    4,   18,    4,    6,
       0,    0,    0,    0,    0,    0,    0,    0,    2,    2,    1,    4,    2,    1,    5,    5,    3,    1,    4,   11,   13,    0,    0,   14,   14,   13,    4,    3,   23,    7,   10,   29,   23,   21,   23,   26,   34,   14,   32,    7,   31,
       0,    0,    0,    0,    0,    2,    0,    4,    2,    4,    2,    4,    6,    4,    5,    7,    2,   15,    1,    4,    5,    6,   16,   14,   16,   22,    6,   26,   31,   37,   25,    1,   35,   19,    7,   14,   10,   10,   14,   22,   34,
       0,    0,    0,    0,    0,    2,    0,    1,    2,    4,    2,    8,    4,    4,    5,    7,   13,   17,   19,   16,   10,    6,    0,   14,    2,   13,    4,   28,   31,   26,   25,   28,   35,   46,    1,   41,   64,   46,   32,   67,   61,
};
static const uint32_t JC_QRE_CODEWORDS_PER_BLOCK_GROUP1[] = {
       0,   19,   34,   55,   80,  108,   68,   78,   97,  116,   68,   81,   92,  107,  115,   87,   98,  107,  120,  113,  107,  116,  111,  121,  117,  106,  114,  122,  117,  116,  115,  115,  115,  115,  115,  121,  121,  122,  122,  117,  118,
       0,   16,   28,   44,   32,   43,   27,   31,   38,   36,   43,   50,   36,   37,   40,   41,   45,   46,   43,   44,   41,   42,   46,   47,   45,   47,   46,   45,   45,   45,   47,   46,   46,   46,   46,   47,   47,   46,   46,   47,   47,
       0,   13,   22,   17,   24,   15,   19,   14,   18,   16,   19,   22,   20,   20,   16,   24,   19,   22,   22,   21,   24,   22,   24,   24,   24,   24,   22,   23,   24,   23,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,   24,
       0,    9,   16,   13,    9,   11,   15,   13,   14,   12,   15,   12,   14,   11,   12,   12,   15,   14,   14,   13,   15,   16,   13,   15,   16,   15,   16,   15,   15,   15,   15,   15,   15,   15,   16,   15,   15,   15,   15,   15,   15,
};
static const uint32_t JC_QRE_CODEWORDS_PER_BLOCK_GROUP2[] = {
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,   69,    0,   93,    0,  116,   88,   99,  108,  121,  114,  108,  117,  112,  122,  118,  107,  115,  123,  118,  117,  116,  116,    0,  116,  116,  122,  122,  123,  123,  118,  119,
       0,    0,    0,    0,    0,    0,    0,    0,   39,   37,   44,   51,   37,   38,   41,   42,   46,   47,   44,   45,   42,    0,    0,   48,   46,   48,   47,   46,   46,   46,   48,   47,   47,   47,   47,   48,   48,   47,   47,   48,   48,
       0,    0,    0,    0,    0,   16,    0,   15,   19,   17,   20,   23,   21,   21,   17,   25,   20,   23,   23,   22,   25,   23,   25,   25,   25,   25,   23,   24,   25,   24,   25,   25,   25,   25,   25,   25,   25,   25,   25,   25,   25,
       0,    0,    0,    0,    0,   12,    0,   14,   15,   13,   16,   13,   15,   12,   13,   13,   16,   15,   15,   14,   16,   17,    0,   16,   17,   16,   17,   16,   16,   16,   16,   16,   16,   16,   17,   16,   16,   16,   16,   16,   16,
};
// Each error correction level can have 8 format strings depending on what mask was applied
static const uint32_t JC_QRE_FORMAT_BITS[] = {
    30660, 29427, 32170, 30877, 26159, 25368, 27713, 26998,
    21522, 20773, 24188, 23371, 17913, 16590, 20375, 19104,
    13663, 12392, 16177, 14854, 9396, 8579, 11994, 11245,
    5769, 5054, 7399, 6608, 1890,  597, 3340, 2107,
};

// Each version has its version string
static const uint32_t JC_QRE_VERSION_BITS[] = {
       0,    0,    0,    0,    0,    0,    0, 31892, 34236, 39577, 42195, 48118, 51042, 55367, 58893, 63784, 68472, 70749, 76311, 79154, 84390, 87683, 92361, 96236, 102084, 102881, 110507, 110734, 117786, 119615, 126325, 127568, 133589, 136944, 141498, 145311, 150283, 152622, 158308, 161089, 167017,
};


typedef struct _JCQRCodeBitBuffer
{
    uint8_t* bits;      // pointer to payload
    uint32_t numbits;   // current offset (in bits)
    JC_QRE_PAD(4);
} JCQRCodeBitBuffer;

typedef struct _JCQRCodeSegment
{
    JCQRCodeBitBuffer   data;
    uint32_t            offset;
    uint32_t            elementcount;   // character, bytes, or kanji characters
    uint8_t             type;           // numeric, alphanumeric, byte, kanji
    JC_QRE_PAD(7);
} JCQRCodeSegment;

typedef struct _JCQRCodeInternal
{
    JCQRCode qrcode; // the output

    uint8_t  bitbuffer[4096];   // storage for all the segments. Each segment starts at a byte boundary
    uint8_t  databuffer[4096];  // all segments merged into one buffer (appended one after each other)
    uint8_t  errorcorrection[4096];  // storage for the error correction code words
    uint8_t  interleaved[4096];  // all interleaved blocks, including error correction
    uint8_t  image[32768];       // Enough to store the largest version (177*177)
    uint8_t  image_fun[32768];   // Holds info about whether a module is a function module or not

    JCQRCodeSegment segments[8];
    uint32_t num_segments;
    uint32_t datasize;          // number of bytes used in databuffer
    uint32_t interleavedsize;   // number of bytes used in interleaved
} JCQRCodeInternal;


// Store the integer with the most significant bit first
static inline uint32_t _jc_qre_bitbuffer_write(uint8_t* buffer, uint32_t buffersize, uint32_t* cursor, uint32_t input, uint32_t numbits)
{
    uint32_t num_bytes_to_traverse = (numbits + 7) / 8;
    if( *cursor + num_bytes_to_traverse >= buffersize )
        return 0;
    if( numbits > 32 )
        return 0;

    uint32_t pos = *cursor; // in bits

//print_bits("encode:", input, numbits);

    for( uint32_t i = 0; i < numbits; ++i, ++pos)
    {
        //uint8_t currentvalue = buffer[pos / 8];

        uint32_t targetindex = 7 - pos & 0x7;

        uint32_t ii = numbits - 1 - i;
        uint8_t inputbit = (input >> ii) & 1;

        buffer[pos/8] |= (uint8_t)(inputbit << targetindex);
//printf(" %u  %u    %u: %u\n", pos, pos/8, i, (inputbit << targetindex));

//print_bits("byte:", buffer[pos/8], 8);

    }
    *cursor += numbits;

//printf("\n");

    return 1;
}

// Appends a previously encoded segment into another
static inline uint32_t _jc_qre_bitbuffer_append(uint8_t* buffer, uint32_t buffersize, uint32_t* cursor, uint8_t* input, uint32_t numbits)
{
    uint32_t num_bytes_to_traverse = (numbits + 7) / 8;
    if( *cursor + num_bytes_to_traverse >= buffersize )
        return 0;

    uint32_t pos = *cursor; // in bits
    for( uint32_t i = 0; i < numbits; ++i, ++pos )
    {
        //uint8_t currentvalue = buffer[pos / 8];

        uint32_t targetindex = 7 - pos & 0x7;

        //uint32_t inputindex = i & 0x7;
        uint8_t inputbit = input[i / 8] >> (7 - (i & 7)) & 1;

        // clear the target bit and OR in the input bit
        buffer[pos/8] |= (uint8_t)(inputbit << targetindex);
    }

    *cursor += numbits;
    return 1;
}

static inline uint32_t _jc_qre_bitbuffer_read(uint8_t* buffer, uint32_t buffersize, uint32_t* cursor, uint32_t numbits)
{
    uint32_t pos = *cursor; // in bits
    uint32_t value = 0;
    for( uint32_t i = 0; i < numbits; ++i, ++pos)
    {
        if( pos / 8 >= buffersize )
            break;

        uint8_t currentvalue = buffer[pos / 8];

        uint32_t srcindex = 7 - pos & 0x7;

        uint32_t srcbit = (currentvalue & (1 << srcindex)) ? 1 : 0;

        value = (value & ~(1 << i)) | (srcbit << i);
    }

    *cursor = pos;
    return value;
}

/*
static void _jc_qre_dbg_print(const uint8_t* buffer, uint32_t numbits)
{
    printf("encoded: ");
    for( uint32_t i = 0; i < numbits; ++i)
    {
        uint8_t currentvalue = buffer[i / 8];
        uint32_t srcindex = i & 0x7;
        uint32_t srcbit = (currentvalue & (1 << srcindex)) ? 1 : 0;

        printf("%d ", srcbit);
    }
    printf("\n");
}
*/

static inline uint32_t _jc_qre_guess_type_numeric(const uint8_t* input, uint32_t inputlength)
{
    for( uint32_t i = 0; i < inputlength; ++i )
    {
        if( input[i] < '0' || input[i] > '9' )
        {
            return 0;
        }
    }
    return 1;
}


static inline uint32_t _jc_qre_guess_type_alphanumeric(const uint8_t* input, uint32_t inputlength)
{
    for( uint32_t i = 0; i < inputlength; ++i )
    {
        uint8_t c = input[i];
        if( c < ' ' || c > 'Z' || JC_QRE_ALPHANUMERIC_MAPPINGS[c - ' '] == -1 )
        {
            return 0;
        }
    }
    return 1;
}

static inline uint8_t _jc_qre_guess_type(const uint8_t* input, uint32_t inputlength)
{
    if( _jc_qre_guess_type_numeric(input, inputlength) )
        return JC_QRE_INPUT_TYPE_NUMERIC;
    else if( _jc_qre_guess_type_alphanumeric(input, inputlength) )
        return JC_QRE_INPUT_TYPE_ALPHANUMERIC;

    return JC_QRE_INPUT_TYPE_BYTE;
}

static uint32_t _jc_qre_encode_numeric(JCQRCodeSegment* seg, uint32_t maxsize, const uint8_t* input, uint32_t inputlength)
{
    if( inputlength >= maxsize )
        return 0xFFFFFFFF;
    uint32_t accum = 0;
    for( uint32_t i = 0; i < inputlength; ++i, ++input)
    {
        char c = *(const char*)input;
        accum = accum * 10 + (uint32_t)(c - '0');
        if( i % 3 == 2 )
        {
            _jc_qre_bitbuffer_write(seg->data.bits, maxsize, &seg->data.numbits, accum, 10);
            accum = 0;
        }
    }
    if( accum )
    {
        uint32_t num = inputlength % 3;
        _jc_qre_bitbuffer_write(seg->data.bits, maxsize, &seg->data.numbits, accum, num * 3 + 1);
    }
    seg->elementcount = inputlength;
    seg->type = JC_QRE_INPUT_TYPE_NUMERIC;
    return seg->elementcount;
}

static uint32_t _jc_qre_encode_alphanumeric(JCQRCodeSegment* seg, uint32_t maxsize, const uint8_t* input, uint32_t inputlength)
{
    if( inputlength >= maxsize )
        return 0xFFFFFFFF;
    const char* characters = (const char*)input;
    for( uint32_t i = 0; i < inputlength/2; ++i)
    {
        uint32_t v1 = (uint32_t)JC_QRE_ALPHANUMERIC_MAPPINGS[characters[i*2+0] - ' '];
        uint32_t v2 = (uint32_t)JC_QRE_ALPHANUMERIC_MAPPINGS[characters[i*2+1] - ' '];
        uint32_t value = (v1 * 45) + v2;

        _jc_qre_bitbuffer_write(seg->data.bits, maxsize, &seg->data.numbits, value, 11);

        //printf("hello: %c %c - > %u %u   %u bits\n", characters[i*2+0], characters[i*2+1], v1, v2, seg->data.numbits );
    }
    if( inputlength & 1 )
    {
        uint32_t v = (uint32_t)JC_QRE_ALPHANUMERIC_MAPPINGS[characters[inputlength-1] - ' '];
        _jc_qre_bitbuffer_write(seg->data.bits, maxsize, &seg->data.numbits, v, 6);
    }
    seg->elementcount = inputlength;
    seg->type = JC_QRE_INPUT_TYPE_ALPHANUMERIC;

    //printf("_jc_qre_encode_alphanumeric: %u chars   %u bits\n", inputlength, seg->data.numbits);

    return seg->elementcount;
}

static uint32_t _jc_qre_encode_bytes(JCQRCodeSegment* seg, uint32_t maxsize, const uint8_t* input, uint32_t inputlength)
{
    if( inputlength >= maxsize )
        return 0xFFFFFFFF;
    memcpy(seg->data.bits, input, inputlength);
    seg->data.numbits = inputlength * 8;
    seg->elementcount = inputlength;
    seg->type = JC_QRE_INPUT_TYPE_BYTE;
    return seg->elementcount;
}

static uint32_t _jc_qre_add_segment(JCQRCodeInternal* qr, const uint8_t* input, uint32_t inputlength)
{
    if( qr->num_segments >= sizeof(qr->segments)/sizeof(JCQRCodeSegment) )
        return 0xFFFFFFFF;

    JCQRCodeSegment* seg = &qr->segments[qr->num_segments];
    uint32_t offset = 0;
    for( uint32_t i = 0; i < qr->num_segments; ++i )
    {
        offset += qr->segments[i].offset;
    }
    seg->offset = offset;
    seg->data.bits = &qr->bitbuffer[offset];
    qr->num_segments++;

    seg->type = _jc_qre_guess_type(input, inputlength);

    printf("  _jc_qre_guess_type: %d\n", seg->type);


    uint32_t max_size = sizeof(qr->bitbuffer) - offset;
    uint32_t result = 0;
    if( seg->type == JC_QRE_INPUT_TYPE_NUMERIC ) {
        result = _jc_qre_encode_numeric(seg, max_size, input, inputlength);
    } else if( seg->type == JC_QRE_INPUT_TYPE_ALPHANUMERIC ) {
        result = _jc_qre_encode_alphanumeric(seg, max_size, input, inputlength);
    } else if( seg->type == JC_QRE_INPUT_TYPE_BYTE ) {
        result = _jc_qre_encode_bytes(seg, max_size, input, inputlength);
    }

    return result;
}

// Reed-Solomon functions
static uint8_t _jc_qre_rs_multiply(uint8_t x, uint8_t y)
{
    int32_t v = 0;
    for( int32_t i = 7; i >= 0; --i )
    {
        v = (v << 1) ^ ( (v >> 7) * 0x11D ); // Do modulo 0x11D if it overflows
        v ^= ((y >> i) & 1) * x;            // Only if y is odd
    }
    return (uint8_t)v;
}

// the generator [out] must have the size 'length'
static void _jc_qre_rs_create_generator(uint32_t length, uint8_t* coefficients)
{
    for( uint32_t i = 0; i < length; ++i )
    {
        coefficients[i] = 0;
    }
    coefficients[length-1] = 1;

    uint32_t power = 1;
    for( uint32_t i = 0; i < length; ++i )
    {
        for( uint32_t j = 0; j < length; ++j )
        {
            coefficients[j] = _jc_qre_rs_multiply(coefficients[j], (uint8_t)power);
            if( (j + 1) < length )
                coefficients[j] ^= coefficients[j+1];
        }
        uint32_t overflow = (power >> 7);           // If it became > 255, this is 1...
        power = (power << 1) ^ (overflow * 0x11D);  // ...in which case, we should do the modulo
    }
}

// length: length of coefficients
// coefficients: the rs generator coefficients
// datasize: the number of data code words
// data: the data code words
// out: the remainder coefficients
static void _jc_qre_rs_encode(uint32_t length, const uint8_t* coefficients, uint32_t datasize, const uint8_t* data, uint8_t* out)
{
    for( uint32_t i = 0; i < length; ++i )
    {
        out[i] = 0;
    }

    for( uint32_t i = 0; i < datasize; ++i )
    {
        uint32_t value = data[i] ^ out[0];
        // shift all elements one step
        for( uint32_t j = 1; j < length; ++j )
        {
            out[j-1] = out[j];
        }
        out[length-1] = 0;
        for( uint32_t j = 0; j < length; ++j )
        {
            out[j] ^= _jc_qre_rs_multiply(coefficients[j], (uint8_t)value);
        }
    }
}

/*
static void dbg_array(const uint8_t* a, uint32_t count)
{
    printf("\tlen: %u: ", count);
    for( uint32_t i = 0; i < count; ++i)
    {
        printf("%d, ", a[i]);
    }
    printf("\n");
}
*/

static void _jc_qre_calc_error_correction(JCQRCodeInternal* qr)
{
    uint32_t index = JC_QRE_INDEX(qr->qrcode.ecl, qr->qrcode.version);
    uint32_t num_ec_codewords_per_block = JC_QRE_ERROR_CORRECTION_CODEWORD_COUNT[index];

    uint8_t coefficients[64]; // generator
    _jc_qre_rs_create_generator(num_ec_codewords_per_block, coefficients);

    uint32_t num_ec_codewords_total = 0;
    uint32_t data_offset = 0;

    uint32_t num_data_codewords_per_block = JC_QRE_CODEWORDS_PER_BLOCK_GROUP1[index];
    for( uint32_t i = 0; i < JC_QRE_BLOCK_COUNT_GROUP1[index]; ++i )
    {
        _jc_qre_rs_encode(num_ec_codewords_per_block, coefficients, num_data_codewords_per_block, &qr->databuffer[data_offset], &qr->errorcorrection[num_ec_codewords_total]);

        data_offset += num_data_codewords_per_block;
        num_ec_codewords_total += num_ec_codewords_per_block;
    }

    num_data_codewords_per_block = JC_QRE_CODEWORDS_PER_BLOCK_GROUP2[index];
    for( uint32_t i = 0; i < JC_QRE_BLOCK_COUNT_GROUP2[index]; ++i )
    {
        _jc_qre_rs_encode(num_ec_codewords_per_block, coefficients, num_data_codewords_per_block, &qr->databuffer[data_offset], &qr->errorcorrection[num_ec_codewords_total]);

        data_offset += num_data_codewords_per_block;
        num_ec_codewords_total += num_ec_codewords_per_block;
    }

    for( uint32_t i = 0; i < num_ec_codewords_total; ++i )
    {
        //printf("EC %u: 0x%02x\n", i, qr->errorcorrection[i]);
    }
}

static void _jc_qre_interleave_codewords(JCQRCodeInternal* qr)
{
    uint32_t index = JC_QRE_INDEX(qr->qrcode.ecl, qr->qrcode.version);
    uint32_t num_blocks = JC_QRE_BLOCK_COUNT_GROUP1[index] + JC_QRE_BLOCK_COUNT_GROUP2[index];

    uint32_t num_data_cw_block1 = JC_QRE_CODEWORDS_PER_BLOCK_GROUP1[index];
    uint32_t num_data_cw_block2 = JC_QRE_CODEWORDS_PER_BLOCK_GROUP2[index];
    uint32_t num_data_cw_block = num_data_cw_block1 > num_data_cw_block2 ? num_data_cw_block1 : num_data_cw_block2;

    uint32_t block_count_group1 = JC_QRE_BLOCK_COUNT_GROUP1[index];

    // where each group starts
    uint32_t groupoffset1 = 0;
    uint32_t groupoffset2 = num_data_cw_block1 * block_count_group1;

    // We have data code words in consecutive order, block after block
    // need to interleave those into final buffer
    // The blocks in group 1 have smaller size than those in group 2
    uint32_t total = 0;
    for( uint32_t cw = 0; cw < num_data_cw_block; ++cw)
    {
        for( uint32_t i = 0; i < num_blocks; ++i)
        {
            // are we in group 1 or 2?
            uint32_t group1 = i < block_count_group1;
            // number of data codewords in group
            uint32_t data_cw_count = group1 ? num_data_cw_block1 : num_data_cw_block2;
            if( cw >= data_cw_count )
            {
                // since the blocks in group 1 are shorter than those in block 2
                continue;
            }

            uint32_t offset = group1 ? groupoffset1 : groupoffset2;
            uint32_t blocknum = group1 ? i : i - block_count_group1;

            qr->interleaved[total++] = qr->databuffer[offset + blocknum * data_cw_count + cw];
        }
    }

    // Interleave the error correction codewords
    uint32_t num_ec_codewords_per_block = JC_QRE_ERROR_CORRECTION_CODEWORD_COUNT[index];
    for( uint32_t cw = 0; cw < num_ec_codewords_per_block; ++cw)
    {
        for( uint32_t i = 0; i < num_blocks; ++i)
        {
            qr->interleaved[total++] = qr->errorcorrection[i * num_ec_codewords_per_block + cw];
        }
    }
    qr->interleavedsize = total;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline int32_t _jc_qre_get_image_size(uint32_t version)
{
    return (version-1)*4 + 21;
}


static inline void _jc_qre_draw_module(JCQRCodeInternal* qr, int32_t x, int32_t y, uint8_t black)
{
    qr->image[y * 256 + x] = black ? 0 : 255;
}
static inline void _jc_qre_draw_function_module(JCQRCodeInternal* qr, int32_t x, int32_t y, uint8_t black)
{
    qr->image[y * 256 + x] = black ? 0 : 255;
    qr->image_fun[y * 256 + x] = 1;
}
static inline uint32_t _jc_qre_is_function_module(JCQRCodeInternal* qr, int32_t x, int32_t y)
{
    return qr->image_fun[y * 256 + x] != 0 ? 1 : 0;
}

static void _jc_qre_draw_finder_pattern(JCQRCodeInternal* qr, int32_t x, int32_t y)
{
    int size = _jc_qre_get_image_size(qr->qrcode.version);

    for( int j = -4; j <= 4; ++j )
    {
        int yy = y + j;
        if( yy < 0 || yy >= size )
            continue;
        for( int i = -4; i <= 4; ++i )
        {
            int xx = x + i;
            if( xx < 0 || xx >= size )
                continue;
            int iabs = i < 0 ? -i : i;
            int jabs = j < 0 ? -j : j;
            int max = iabs > jabs ? iabs : jabs;

            _jc_qre_draw_function_module(qr, xx, yy, max != 2 && max != 4);
        }
    }
}


static void _jc_qre_draw_data(JCQRCodeInternal* qr)
{
    int32_t size = qr->qrcode.size;

    uint32_t bitindex = 0;

    int upwards = 1;
    int x = size - 1;

    printf("qr->interleavedsize * 8: %u\n", qr->interleavedsize * 8);

    print_bits("byte 0: ", qr->interleaved[0], 8);
    print_bits("byte 1: ", qr->interleaved[1], 8);
    print_bits("byte 2: ", qr->interleaved[2], 8);
    print_bits("byte 3: ", qr->interleaved[3], 8);


    while( bitindex < qr->interleavedsize * 8 )
    {
        for( int i = 0; i < size * 2; ++i )
        {
            uint32_t xx = x - (i & 1);
            uint32_t y = upwards ? size - 1 - (i >> 1) : (i >> 1);

            if(!_jc_qre_is_function_module(qr, xx, y))
            {
                uint32_t bit = _jc_qre_bitbuffer_read(qr->interleaved, qr->interleavedsize, &bitindex, 1);
                _jc_qre_draw_module(qr, xx, y, bit != 0);
            }
        }
        upwards ^= 1;
        x -= 2;
        if( x == 6 )
            x = 5;
    }
}


static void _jc_qre_draw_finder_patterns(JCQRCodeInternal* qr)
{
    int32_t size = qr->qrcode.size;

    for( uint32_t i = 0; i < size; ++i )
    {
        _jc_qre_draw_function_module(qr, i, 6, i % 2 == 0);
        _jc_qre_draw_function_module(qr, 6, i, i % 2 == 0);
    }

    _jc_qre_draw_finder_pattern(qr, 3, 3);
    _jc_qre_draw_finder_pattern(qr, size-4, 3);
    _jc_qre_draw_finder_pattern(qr, 3, size-4);

    if( qr->qrcode.version > 1 )
    {
        // alignment patterns
        //_jc_qre_draw_finder_pattern(qr, 3, size-4);
    }

/*
    // Reserve format info areas
    for( uint32_t i = 0; i < 8; ++i )
    {
        qr->image_fun[(size - i - 1) * 256 + 8] = 1; // bottom left
        qr->image_fun[8 * 256 + (size - i - 1)] = 1; // top right
        qr->image_fun[8 * 256 + i] = 1; // topleft, lower
        qr->image_fun[i * 256 + 8] = 1; // topleft, lower
    }*/

    // the dark module
    _jc_qre_draw_function_module(qr, 8, (4 * qr->qrcode.version) + 9, 1);
}

static void _jc_qre_draw_format(JCQRCodeInternal* qr, uint32_t pattern_mask)
{
    int32_t size = qr->qrcode.size;
    uint32_t format = JC_QRE_FORMAT_BITS[qr->qrcode.ecl * 8 + pattern_mask];

printf("version: %u  ecl: %d\n", qr->qrcode.version, qr->qrcode.ecl);
printf("pattern_mask: %u\n", pattern_mask);
    printf("format: %u, 0x%08x\n", format, format);
    print_bits("format:", format, 15);

    // flip the bits, for the benefit of the draw function
    //format ^= 0xFFFFFFFF;

    // top right
    for( uint32_t i = 0; i < 8; ++i )
    {
        _jc_qre_draw_function_module(qr, size - 1 - i, 8, (format >> i) & 1);
    }
    // bottom left
    for( uint32_t i = 0; i < 7; ++i )
    {
        _jc_qre_draw_function_module(qr, 8, size - 7 + i, (format >> (8+i)) & 1);
    }

    // top left -- right side
    for( uint32_t i = 0; i < 6; ++i )
    {
        _jc_qre_draw_function_module(qr, 8, i, (format >> i) & 1);
    }

    _jc_qre_draw_function_module(qr, 8, 7, (format >> 6) & 1);
    _jc_qre_draw_function_module(qr, 8, 8, (format >> 7) & 1);
    _jc_qre_draw_function_module(qr, 7, 8, (format >> 8) & 1);

    for( uint32_t i = 0; i < 6; ++i )
    {
        _jc_qre_draw_function_module(qr, 5 - i, 8, (format >> (9 + i))  & 1);
    }
}

static inline uint32_t _jc_qre_is_masked(uint32_t x, uint32_t y, uint32_t pattern_mask)
{
    switch(pattern_mask)
    {
        case 0: return ((x + y) & 1) == 0;
        case 1: return (y & 1) == 0;
        case 2: return (x % 3) == 0;
        case 3: return ((x + y) % 3) == 0;
        case 4: return ((x / 3 + y / 2) & 1) == 0;
        case 5: return ((x * y) & 1 + (x * y) % 3) == 0;
        case 6: return (((x * y) & 1 + (x * y) % 3) & 1) == 0;
        case 7: return (((x + y) & 1 + (x * y) % 3) & 1) == 0;
        default: return 0;
    }
}

static void _jc_qre_draw_mask(JCQRCodeInternal* qr, uint32_t pattern_mask)
{
    int32_t size = qr->qrcode.size;
    for( uint32_t y = 0; y < size; ++y )
    {
        for( uint32_t x = 0; x < size; ++x )
        {
            if(_jc_qre_is_function_module(qr, x, y))
            {
                continue;
            }
            if(_jc_qre_is_masked(x, y, pattern_mask))
                qr->image[y * 256 + x] ^= 255;
        }
    }
}

static inline uint32_t _jc_qre_calc_penalty(JCQRCodeInternal* qr)
{
    return 0;
}

static void _jc_qre_draw_version(JCQRCodeInternal* qr)
{
    int32_t size = qr->qrcode.size;
    if( qr->qrcode.version >= 7 )
    {
        for( uint32_t x = 0; x < 6; ++x )
        {
            for( uint32_t y = 0; y < 3; ++y )
            {
                uint32_t yy = size - 11 + y;
                // Bottom left
                qr->image_fun[yy * 256 + x] = 1;
                // Top right
                qr->image_fun[x * 256 + yy] = 1;
            }
        }
    }
}

static void _jc_qre_draw_image(JCQRCodeInternal* qr)
{
    memset(qr->image, 255, sizeof(qr->image));
    memset(qr->image_fun, 0, sizeof(qr->image_fun));
    qr->qrcode.data = qr->image;
    qr->qrcode.size = _jc_qre_get_image_size(qr->qrcode.version);

    _jc_qre_draw_finder_patterns(qr);
    _jc_qre_draw_format(qr, 0); // reserve area
    _jc_qre_draw_version(qr); // reserve area

    _jc_qre_draw_data(qr);

    // apply masks and find the best one
    uint32_t best_mask = 0;
    /*
    uint32_t lowest_score = 0xFFFFFFFF;
    for( uint32_t i = 0; i < 8; ++i )
    {
        _jc_qre_draw_format(qr, i);
        _jc_qre_draw_mask(qr, i);
        uint32_t score = _jc_qre_calc_penalty(qr);


    printf("mask: %u   score: %u\n", i, score);
        if( score < lowest_score )
        {
            best_mask = i;
            lowest_score = score;
        }
        _jc_qre_draw_mask(qr, i); // undo the mask (using xor)
    }
    */

    printf("best mask: %u\n", best_mask);

    _jc_qre_draw_format(qr, best_mask);
    _jc_qre_draw_mask(qr, best_mask);
    _jc_qre_draw_version(qr);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


JCQRCode* jc_qrencode(const uint8_t* input, uint32_t inputlength, uint32_t ecl)
{
    JCQRCodeInternal* qr = (JCQRCodeInternal*)malloc( sizeof(JCQRCodeInternal) );
    memset(qr, 0, sizeof(JCQRCodeInternal) );
    qr->qrcode.ecl = ecl;

    uint32_t numbits = _jc_qre_add_segment(qr, input, inputlength);

    if( numbits == 0 || numbits == 0xFFFFFFFF )
    {
        // todo: error codes
        free(qr);
        return 0;
    }

    uint32_t version = 0;
    uint32_t capacity_bits = 0;
    for( uint32_t i = JC_QRE_MIN_VERSION; i <= JC_QRE_MAX_VERSION; ++i )
    {
        capacity_bits = JC_QRE_DATA_CODEWORD_COUNT[ JC_QRE_INDEX(ecl, i) ] * 8;
        if( numbits < capacity_bits )
        {
            version = i;
            break;
        }
    }
    qr->qrcode.version = version;


//printf("Writing segments\n");
    uint32_t datasize = 0;
    for( uint32_t i = 0; i < qr->num_segments; ++i )
    {
        JCQRCodeSegment* seg = &qr->segments[i];
        _jc_qre_bitbuffer_write(qr->databuffer, sizeof(qr->databuffer), &datasize, JC_QRE_TYPE_BITS[seg->type], 4);

print_bits("mode:", JC_QRE_TYPE_BITS[seg->type], 4);

//printf("line: %d datasize %u\n", __LINE__, datasize);
        _jc_qre_bitbuffer_write(qr->databuffer, sizeof(qr->databuffer), &datasize, seg->elementcount, JC_QRE_CHARACTER_COUNT_BIT_SIZE[JC_QRE_INDEX(seg->type, version)]);

print_bits("numchars:", seg->elementcount, JC_QRE_CHARACTER_COUNT_BIT_SIZE[JC_QRE_INDEX(seg->type, version)]);

//printf("line: %d datasize %u\n", __LINE__, datasize);
        _jc_qre_bitbuffer_append(qr->databuffer, sizeof(qr->databuffer), &datasize, seg->data.bits, seg->data.numbits);


printf("num bits:  %u\n", datasize);

print_bits("byte 0:", qr->databuffer[0], 8);
print_bits("byte 1:", qr->databuffer[1], 8);
print_bits("byte 2:", qr->databuffer[2], 8);
print_bits("byte 3:", qr->databuffer[3], 8);

    }

//printf("line: %d datasize %u\n", __LINE__, datasize);

/*
//printf("Finding a better ecl...\n");

    // see if we can get a higher error correction level and still fit in the smallest version
    for( uint32_t newecl = ecl+1; newecl <= JC_QRE_ERROR_CORRECTION_LEVEL_HIGH; ++newecl )
    {
uint32_t index = JC_QRE_INDEX(newecl, version);
//printf("ecl: %d  index: %u  bytes: %u\n", newecl, index, JC_QRE_DATA_CODEWORD_COUNT[ JC_QRE_INDEX(newecl, version) ]);
        uint32_t new_capacity_bits = JC_QRE_DATA_CODEWORD_COUNT[ JC_QRE_INDEX(newecl, version) ] * 8;
        if( datasize < new_capacity_bits )
        {
            ecl = newecl;
            capacity_bits = new_capacity_bits;
        }
    }
*/

//printf("VERSION: %d   ECL: %d  capacity_bits: %d\n", qr->qrcode.version, qr->qrcode.ecl, capacity_bits);

//printf("Writing terminator\n");

    // add terminator (max 4 zeros)
    uint32_t terminator_length = capacity_bits - datasize;
    if( terminator_length > 4 )
        terminator_length = 4;
    uint32_t zeros = 0;
    if( terminator_length )
    {
        _jc_qre_bitbuffer_write(qr->databuffer, sizeof(qr->databuffer), &datasize, zeros, terminator_length);
    }

//printf("line: %d datasize %u\n", __LINE__, datasize);

    // make it 8 bit aligned
    uint32_t numpadzeros = (8 - (datasize & 0x7)) & 0x7;
    _jc_qre_bitbuffer_write(qr->databuffer, sizeof(qr->databuffer), &datasize, zeros, numpadzeros);

//printf("Writing pad zeroes: %d\n", numpadzeros);
//printf("datasize %u   capacity_bits %u\n", datasize, capacity_bits);

    // pad with bytes
    uint8_t padding[2] = { 236, 17 };
    uint32_t numpadbytes = (capacity_bits - datasize) / 8;

//printf("Padding with %u bytes\n", numpadbytes);
    for( uint32_t i = 0; i < numpadbytes; ++i )
    {
        _jc_qre_bitbuffer_write(qr->databuffer, sizeof(qr->databuffer), &datasize, padding[i&1], 8);
    }

    qr->datasize = datasize / 8;

    _jc_qre_calc_error_correction(qr);
    _jc_qre_interleave_codewords(qr);
    _jc_qre_draw_image(qr);

    return (JCQRCode*)&qr->qrcode;
}

#undef JC_QRE_INDEX

#endif // JC_QRENCODE_IMPLEMENTATION
#endif // JC_QRENCODE_H
