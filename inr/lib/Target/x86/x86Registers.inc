// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/Target/Register.h>
namespace inr::x86 {
#ifdef TARGET_REGISTER_DEFINITIONS
constexpr Register target_registers[] = {
    {"al", 0},
    {"ah", 1},
    {"bl", 2},
    {"bh", 3},
    {"cl", 4},
    {"ch", 5},
    {"dl", 6},
    {"dh", 7},
    {"sil", 8},
    {"dil", 9},
    {"spl", 10},
    {"bpl", 11},
    {"r8b", 12},
    {"r9b", 13},
    {"r10b", 14},
    {"r11b", 15},
    {"r12b", 16},
    {"r13b", 17},
    {"r14b", 18},
    {"r15b", 19},
    {"ax", 20},
    {"bx", 21},
    {"cx", 22},
    {"dx", 23},
    {"si", 24},
    {"di", 25},
    {"sp", 26},
    {"bp", 27},
    {"r8w", 28},
    {"r9w", 29},
    {"r10w", 30},
    {"r11w", 31},
    {"r12w", 32},
    {"r13w", 33},
    {"r14w", 34},
    {"r15w", 35},
    {"eax", 36},
    {"ebx", 37},
    {"ecx", 38},
    {"edx", 39},
    {"esi", 40},
    {"edi", 41},
    {"esp", 42},
    {"ebp", 43},
    {"r8d", 44},
    {"r9d", 45},
    {"r10d", 46},
    {"r11d", 47},
    {"r12d", 48},
    {"r13d", 49},
    {"r14d", 50},
    {"r15d", 51},
    {"rax", 52},
    {"rbx", 53},
    {"rcx", 54},
    {"rdx", 55},
    {"rsi", 56},
    {"rdi", 57},
    {"rsp", 58},
    {"rbp", 59},
    {"r8", 60},
    {"r9", 61},
    {"r10", 62},
    {"r11", 63},
    {"r12", 64},
    {"r13", 65},
    {"r14", 66},
    {"r15", 67},
    {"xmm0", 68},
    {"xmm1", 69},
    {"xmm2", 70},
    {"xmm3", 71},
    {"xmm4", 72},
    {"xmm5", 73},
    {"xmm6", 74},
    {"xmm7", 75},
    {"xmm8", 76},
    {"xmm9", 77},
    {"xmm10", 78},
    {"xmm11", 79},
    {"xmm12", 80},
    {"xmm13", 81},
    {"xmm14", 82},
    {"xmm15", 83},
    {"xmm16", 84},
    {"xmm17", 85},
    {"xmm18", 86},
    {"xmm19", 87},
    {"xmm20", 88},
    {"xmm21", 89},
    {"xmm22", 90},
    {"xmm23", 91},
    {"xmm24", 92},
    {"xmm25", 93},
    {"xmm26", 94},
    {"xmm27", 95},
    {"xmm28", 96},
    {"xmm29", 97},
    {"xmm30", 98},
    {"xmm31", 99},
    {"ymm0", 100},
    {"ymm1", 101},
    {"ymm2", 102},
    {"ymm3", 103},
    {"ymm4", 104},
    {"ymm5", 105},
    {"ymm6", 106},
    {"ymm7", 107},
    {"ymm8", 108},
    {"ymm9", 109},
    {"ymm10", 110},
    {"ymm11", 111},
    {"ymm12", 112},
    {"ymm13", 113},
    {"ymm14", 114},
    {"ymm15", 115},
    {"ymm16", 116},
    {"ymm17", 117},
    {"ymm18", 118},
    {"ymm19", 119},
    {"ymm20", 120},
    {"ymm21", 121},
    {"ymm22", 122},
    {"ymm23", 123},
    {"ymm24", 124},
    {"ymm25", 125},
    {"ymm26", 126},
    {"ymm27", 127},
    {"ymm28", 128},
    {"ymm29", 129},
    {"ymm30", 130},
    {"ymm31", 131},
    {"zmm0", 132},
    {"zmm1", 133},
    {"zmm2", 134},
    {"zmm3", 135},
    {"zmm4", 136},
    {"zmm5", 137},
    {"zmm6", 138},
    {"zmm7", 139},
    {"zmm8", 140},
    {"zmm9", 141},
    {"zmm10", 142},
    {"zmm11", 143},
    {"zmm12", 144},
    {"zmm13", 145},
    {"zmm14", 146},
    {"zmm15", 147},
    {"zmm16", 148},
    {"zmm17", 149},
    {"zmm18", 150},
    {"zmm19", 151},
    {"zmm20", 152},
    {"zmm21", 153},
    {"zmm22", 154},
    {"zmm23", 155},
    {"zmm24", 156},
    {"zmm25", 157},
    {"zmm26", 158},
    {"zmm27", 159},
    {"zmm28", 160},
    {"zmm29", 161},
    {"zmm30", 162},
    {"zmm31", 163},
};
constexpr uint32_t A_FAMILY_regs[] = {
    0,
    1,
    20,
    36,
    52,
};
constexpr uint32_t B_FAMILY_regs[] = {
    2,
    3,
    21,
    37,
    53,
};
constexpr uint32_t C_FAMILY_regs[] = {
    4,
    5,
    22,
    38,
    54,
};
constexpr uint32_t D_FAMILY_regs[] = {
    6,
    7,
    23,
    39,
    55,
};
constexpr uint32_t SI_FAMILY_regs[] = {
    8,
    24,
    40,
    56,
};
constexpr uint32_t DI_FAMILY_regs[] = {
    9,
    25,
    41,
    57,
};
constexpr uint32_t SP_FAMILY_regs[] = {
    10,
    26,
    42,
    58,
};
constexpr uint32_t BP_FAMILY_regs[] = {
    11,
    27,
    43,
    59,
};
constexpr uint32_t R8_FAMILY_regs[] = {
    12,
    28,
    44,
    60,
};
constexpr uint32_t R9_FAMILY_regs[] = {
    13,
    29,
    45,
    61,
};
constexpr uint32_t R10_FAMILY_regs[] = {
    14,
    30,
    46,
    62,
};
constexpr uint32_t R11_FAMILY_regs[] = {
    15,
    31,
    47,
    63,
};
constexpr uint32_t R12_FAMILY_regs[] = {
    16,
    32,
    48,
    64,
};
constexpr uint32_t R13_FAMILY_regs[] = {
    17,
    33,
    49,
    65,
};
constexpr uint32_t R14_FAMILY_regs[] = {
    18,
    34,
    50,
    66,
};
constexpr uint32_t R15_FAMILY_regs[] = {
    19,
    35,
    51,
    67,
};
constexpr uint32_t MM0_FAMILY_regs[] = {
    68,
    100,
    132,
};
constexpr uint32_t MM1_FAMILY_regs[] = {
    69,
    101,
    133,
};
constexpr uint32_t MM2_FAMILY_regs[] = {
    70,
    102,
    134,
};
constexpr uint32_t MM3_FAMILY_regs[] = {
    71,
    103,
    135,
};
constexpr uint32_t MM4_FAMILY_regs[] = {
    72,
    104,
    136,
};
constexpr uint32_t MM5_FAMILY_regs[] = {
    73,
    105,
    137,
};
constexpr uint32_t MM6_FAMILY_regs[] = {
    74,
    106,
    138,
};
constexpr uint32_t MM7_FAMILY_regs[] = {
    75,
    107,
    139,
};
constexpr uint32_t MM8_FAMILY_regs[] = {
    76,
    108,
    140,
};
constexpr uint32_t MM9_FAMILY_regs[] = {
    77,
    109,
    141,
};
constexpr uint32_t MM10_FAMILY_regs[] = {
    78,
    110,
    142,
};
constexpr uint32_t MM11_FAMILY_regs[] = {
    79,
    111,
    143,
};
constexpr uint32_t MM12_FAMILY_regs[] = {
    80,
    112,
    144,
};
constexpr uint32_t MM13_FAMILY_regs[] = {
    81,
    113,
    145,
};
constexpr uint32_t MM14_FAMILY_regs[] = {
    82,
    114,
    146,
};
constexpr uint32_t MM15_FAMILY_regs[] = {
    83,
    115,
    147,
};
constexpr uint32_t MM16_FAMILY_regs[] = {
    84,
    116,
    148,
};
constexpr uint32_t MM17_FAMILY_regs[] = {
    85,
    117,
    149,
};
constexpr uint32_t MM18_FAMILY_regs[] = {
    86,
    118,
    150,
};
constexpr uint32_t MM19_FAMILY_regs[] = {
    87,
    119,
    151,
};
constexpr uint32_t MM20_FAMILY_regs[] = {
    88,
    120,
    152,
};
constexpr uint32_t MM21_FAMILY_regs[] = {
    89,
    121,
    153,
};
constexpr uint32_t MM22_FAMILY_regs[] = {
    90,
    121,
    154,
};
constexpr uint32_t MM23_FAMILY_regs[] = {
    91,
    123,
    155,
};
constexpr uint32_t MM24_FAMILY_regs[] = {
    92,
    124,
    156,
};
constexpr uint32_t MM25_FAMILY_regs[] = {
    93,
    125,
    157,
};
constexpr uint32_t MM26_FAMILY_regs[] = {
    94,
    126,
    158,
};
constexpr uint32_t MM27_FAMILY_regs[] = {
    95,
    127,
    159,
};
constexpr uint32_t MM28_FAMILY_regs[] = {
    96,
    128,
    160,
};
constexpr uint32_t MM29_FAMILY_regs[] = {
    97,
    129,
    161,
};
constexpr uint32_t MM30_FAMILY_regs[] = {
    98,
    130,
    162,
};
constexpr uint32_t MM31_FAMILY_regs[] = {
    99,
    131,
    163,
};
constexpr RegisterFamily target_register_families[] = {
    {0, A_FAMILY_regs},
    {1, B_FAMILY_regs},
    {2, C_FAMILY_regs},
    {3, D_FAMILY_regs},
    {4, SI_FAMILY_regs},
    {5, DI_FAMILY_regs},
    {6, SP_FAMILY_regs},
    {7, BP_FAMILY_regs},
    {8, R8_FAMILY_regs},
    {9, R9_FAMILY_regs},
    {10, R10_FAMILY_regs},
    {11, R11_FAMILY_regs},
    {12, R12_FAMILY_regs},
    {13, R13_FAMILY_regs},
    {14, R14_FAMILY_regs},
    {15, R15_FAMILY_regs},
    {16, MM0_FAMILY_regs},
    {17, MM1_FAMILY_regs},
    {18, MM2_FAMILY_regs},
    {19, MM3_FAMILY_regs},
    {20, MM4_FAMILY_regs},
    {21, MM5_FAMILY_regs},
    {22, MM6_FAMILY_regs},
    {23, MM7_FAMILY_regs},
    {24, MM8_FAMILY_regs},
    {25, MM9_FAMILY_regs},
    {26, MM10_FAMILY_regs},
    {27, MM11_FAMILY_regs},
    {28, MM12_FAMILY_regs},
    {29, MM13_FAMILY_regs},
    {30, MM14_FAMILY_regs},
    {31, MM15_FAMILY_regs},
    {32, MM16_FAMILY_regs},
    {33, MM17_FAMILY_regs},
    {34, MM18_FAMILY_regs},
    {35, MM19_FAMILY_regs},
    {36, MM20_FAMILY_regs},
    {37, MM21_FAMILY_regs},
    {38, MM22_FAMILY_regs},
    {39, MM23_FAMILY_regs},
    {40, MM24_FAMILY_regs},
    {41, MM25_FAMILY_regs},
    {42, MM26_FAMILY_regs},
    {43, MM27_FAMILY_regs},
    {44, MM28_FAMILY_regs},
    {45, MM29_FAMILY_regs},
    {46, MM30_FAMILY_regs},
    {47, MM31_FAMILY_regs},
};
constexpr uint32_t GP8_regs[] = {
    0,
    2,
    4,
    6,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    17,
    18,
    19,
};
constexpr uint32_t GP16_regs[] = {
    20,
    21,
    22,
    23,
    24,
    25,
    26,
    27,
    28,
    29,
    30,
    31,
    32,
    33,
    34,
    35,
};
constexpr uint32_t GP32_regs[] = {
    36,
    37,
    38,
    39,
    40,
    41,
    42,
    43,
    44,
    45,
    46,
    47,
    48,
    49,
    50,
    51,
};
constexpr uint32_t GP64_regs[] = {
    52,
    53,
    54,
    55,
    56,
    57,
    58,
    59,
    60,
    61,
    62,
    63,
    64,
    65,
    66,
    67,
};
constexpr uint32_t XMM_regs[] = {
    68,
    69,
    70,
    71,
    72,
    73,
    74,
    75,
    76,
    77,
    78,
    79,
    80,
    81,
    82,
    83,
    84,
    85,
    86,
    87,
    88,
    89,
    90,
    91,
    92,
    93,
    94,
    95,
    96,
    97,
    98,
    99,
};
constexpr uint32_t YMM_regs[] = {
    100,
    101,
    102,
    103,
    104,
    105,
    106,
    107,
    108,
    109,
    110,
    111,
    112,
    113,
    114,
    115,
    116,
    117,
    118,
    119,
    120,
    121,
    122,
    123,
    124,
    125,
    126,
    127,
    128,
    129,
    130,
    131,
};
constexpr uint32_t ZMM_regs[] = {
    132,
    133,
    134,
    135,
    136,
    137,
    138,
    139,
    140,
    141,
    142,
    143,
    144,
    145,
    146,
    147,
    148,
    149,
    150,
    151,
    152,
    153,
    154,
    155,
    156,
    157,
    158,
    159,
    160,
    161,
    162,
    163,
};
constexpr RegisterClass target_register_classes[] = {
    {0, GP8_regs, inr::TIRT::createBit(8)},
    {1, GP16_regs, inr::TIRT::createBit(16)},
    {2, GP32_regs, inr::TIRT::createBit(32)},
    {3, GP64_regs, inr::TIRT::createBit(64)},
    {4, XMM_regs, inr::TIRT::createBit(128)},
    {5, YMM_regs, inr::TIRT::createBit(256)},
    {6, ZMM_regs, inr::TIRT::createBit(512)},
};
#endif
constexpr uint32_t AL = 0;
constexpr uint32_t AH = 1;
constexpr uint32_t BL = 2;
constexpr uint32_t BH = 3;
constexpr uint32_t CL = 4;
constexpr uint32_t CH = 5;
constexpr uint32_t DL = 6;
constexpr uint32_t DH = 7;
constexpr uint32_t SIL = 8;
constexpr uint32_t DIL = 9;
constexpr uint32_t SPL = 10;
constexpr uint32_t BPL = 11;
constexpr uint32_t R8B = 12;
constexpr uint32_t R9B = 13;
constexpr uint32_t R10B = 14;
constexpr uint32_t R11B = 15;
constexpr uint32_t R12B = 16;
constexpr uint32_t R13B = 17;
constexpr uint32_t R14B = 18;
constexpr uint32_t R15B = 19;
constexpr uint32_t AX = 20;
constexpr uint32_t BX = 21;
constexpr uint32_t CX = 22;
constexpr uint32_t DX = 23;
constexpr uint32_t SI = 24;
constexpr uint32_t DI = 25;
constexpr uint32_t SP = 26;
constexpr uint32_t BP = 27;
constexpr uint32_t R8W = 28;
constexpr uint32_t R9W = 29;
constexpr uint32_t R10W = 30;
constexpr uint32_t R11W = 31;
constexpr uint32_t R12W = 32;
constexpr uint32_t R13W = 33;
constexpr uint32_t R14W = 34;
constexpr uint32_t R15W = 35;
constexpr uint32_t EAX = 36;
constexpr uint32_t EBX = 37;
constexpr uint32_t ECX = 38;
constexpr uint32_t EDX = 39;
constexpr uint32_t ESI = 40;
constexpr uint32_t EDI = 41;
constexpr uint32_t ESP = 42;
constexpr uint32_t EBP = 43;
constexpr uint32_t R8D = 44;
constexpr uint32_t R9D = 45;
constexpr uint32_t R10D = 46;
constexpr uint32_t R11D = 47;
constexpr uint32_t R12D = 48;
constexpr uint32_t R13D = 49;
constexpr uint32_t R14D = 50;
constexpr uint32_t R15D = 51;
constexpr uint32_t RAX = 52;
constexpr uint32_t RBX = 53;
constexpr uint32_t RCX = 54;
constexpr uint32_t RDX = 55;
constexpr uint32_t RSI = 56;
constexpr uint32_t RDI = 57;
constexpr uint32_t RSP = 58;
constexpr uint32_t RBP = 59;
constexpr uint32_t R8 = 60;
constexpr uint32_t R9 = 61;
constexpr uint32_t R10 = 62;
constexpr uint32_t R11 = 63;
constexpr uint32_t R12 = 64;
constexpr uint32_t R13 = 65;
constexpr uint32_t R14 = 66;
constexpr uint32_t R15 = 67;
constexpr uint32_t XMM0 = 68;
constexpr uint32_t XMM1 = 69;
constexpr uint32_t XMM2 = 70;
constexpr uint32_t XMM3 = 71;
constexpr uint32_t XMM4 = 72;
constexpr uint32_t XMM5 = 73;
constexpr uint32_t XMM6 = 74;
constexpr uint32_t XMM7 = 75;
constexpr uint32_t XMM8 = 76;
constexpr uint32_t XMM9 = 77;
constexpr uint32_t XMM10 = 78;
constexpr uint32_t XMM11 = 79;
constexpr uint32_t XMM12 = 80;
constexpr uint32_t XMM13 = 81;
constexpr uint32_t XMM14 = 82;
constexpr uint32_t XMM15 = 83;
constexpr uint32_t XMM16 = 84;
constexpr uint32_t XMM17 = 85;
constexpr uint32_t XMM18 = 86;
constexpr uint32_t XMM19 = 87;
constexpr uint32_t XMM20 = 88;
constexpr uint32_t XMM21 = 89;
constexpr uint32_t XMM22 = 90;
constexpr uint32_t XMM23 = 91;
constexpr uint32_t XMM24 = 92;
constexpr uint32_t XMM25 = 93;
constexpr uint32_t XMM26 = 94;
constexpr uint32_t XMM27 = 95;
constexpr uint32_t XMM28 = 96;
constexpr uint32_t XMM29 = 97;
constexpr uint32_t XMM30 = 98;
constexpr uint32_t XMM31 = 99;
constexpr uint32_t YMM0 = 100;
constexpr uint32_t YMM1 = 101;
constexpr uint32_t YMM2 = 102;
constexpr uint32_t YMM3 = 103;
constexpr uint32_t YMM4 = 104;
constexpr uint32_t YMM5 = 105;
constexpr uint32_t YMM6 = 106;
constexpr uint32_t YMM7 = 107;
constexpr uint32_t YMM8 = 108;
constexpr uint32_t YMM9 = 109;
constexpr uint32_t YMM10 = 110;
constexpr uint32_t YMM11 = 111;
constexpr uint32_t YMM12 = 112;
constexpr uint32_t YMM13 = 113;
constexpr uint32_t YMM14 = 114;
constexpr uint32_t YMM15 = 115;
constexpr uint32_t YMM16 = 116;
constexpr uint32_t YMM17 = 117;
constexpr uint32_t YMM18 = 118;
constexpr uint32_t YMM19 = 119;
constexpr uint32_t YMM20 = 120;
constexpr uint32_t YMM21 = 121;
constexpr uint32_t YMM22 = 122;
constexpr uint32_t YMM23 = 123;
constexpr uint32_t YMM24 = 124;
constexpr uint32_t YMM25 = 125;
constexpr uint32_t YMM26 = 126;
constexpr uint32_t YMM27 = 127;
constexpr uint32_t YMM28 = 128;
constexpr uint32_t YMM29 = 129;
constexpr uint32_t YMM30 = 130;
constexpr uint32_t YMM31 = 131;
constexpr uint32_t ZMM0 = 132;
constexpr uint32_t ZMM1 = 133;
constexpr uint32_t ZMM2 = 134;
constexpr uint32_t ZMM3 = 135;
constexpr uint32_t ZMM4 = 136;
constexpr uint32_t ZMM5 = 137;
constexpr uint32_t ZMM6 = 138;
constexpr uint32_t ZMM7 = 139;
constexpr uint32_t ZMM8 = 140;
constexpr uint32_t ZMM9 = 141;
constexpr uint32_t ZMM10 = 142;
constexpr uint32_t ZMM11 = 143;
constexpr uint32_t ZMM12 = 144;
constexpr uint32_t ZMM13 = 145;
constexpr uint32_t ZMM14 = 146;
constexpr uint32_t ZMM15 = 147;
constexpr uint32_t ZMM16 = 148;
constexpr uint32_t ZMM17 = 149;
constexpr uint32_t ZMM18 = 150;
constexpr uint32_t ZMM19 = 151;
constexpr uint32_t ZMM20 = 152;
constexpr uint32_t ZMM21 = 153;
constexpr uint32_t ZMM22 = 154;
constexpr uint32_t ZMM23 = 155;
constexpr uint32_t ZMM24 = 156;
constexpr uint32_t ZMM25 = 157;
constexpr uint32_t ZMM26 = 158;
constexpr uint32_t ZMM27 = 159;
constexpr uint32_t ZMM28 = 160;
constexpr uint32_t ZMM29 = 161;
constexpr uint32_t ZMM30 = 162;
constexpr uint32_t ZMM31 = 163;
constexpr uint32_t A_FAMILY = 0;
constexpr uint32_t B_FAMILY = 1;
constexpr uint32_t C_FAMILY = 2;
constexpr uint32_t D_FAMILY = 3;
constexpr uint32_t SI_FAMILY = 4;
constexpr uint32_t DI_FAMILY = 5;
constexpr uint32_t SP_FAMILY = 6;
constexpr uint32_t BP_FAMILY = 7;
constexpr uint32_t R8_FAMILY = 8;
constexpr uint32_t R9_FAMILY = 9;
constexpr uint32_t R10_FAMILY = 10;
constexpr uint32_t R11_FAMILY = 11;
constexpr uint32_t R12_FAMILY = 12;
constexpr uint32_t R13_FAMILY = 13;
constexpr uint32_t R14_FAMILY = 14;
constexpr uint32_t R15_FAMILY = 15;
constexpr uint32_t MM0_FAMILY = 16;
constexpr uint32_t MM1_FAMILY = 17;
constexpr uint32_t MM2_FAMILY = 18;
constexpr uint32_t MM3_FAMILY = 19;
constexpr uint32_t MM4_FAMILY = 20;
constexpr uint32_t MM5_FAMILY = 21;
constexpr uint32_t MM6_FAMILY = 22;
constexpr uint32_t MM7_FAMILY = 23;
constexpr uint32_t MM8_FAMILY = 24;
constexpr uint32_t MM9_FAMILY = 25;
constexpr uint32_t MM10_FAMILY = 26;
constexpr uint32_t MM11_FAMILY = 27;
constexpr uint32_t MM12_FAMILY = 28;
constexpr uint32_t MM13_FAMILY = 29;
constexpr uint32_t MM14_FAMILY = 30;
constexpr uint32_t MM15_FAMILY = 31;
constexpr uint32_t MM16_FAMILY = 32;
constexpr uint32_t MM17_FAMILY = 33;
constexpr uint32_t MM18_FAMILY = 34;
constexpr uint32_t MM19_FAMILY = 35;
constexpr uint32_t MM20_FAMILY = 36;
constexpr uint32_t MM21_FAMILY = 37;
constexpr uint32_t MM22_FAMILY = 38;
constexpr uint32_t MM23_FAMILY = 39;
constexpr uint32_t MM24_FAMILY = 40;
constexpr uint32_t MM25_FAMILY = 41;
constexpr uint32_t MM26_FAMILY = 42;
constexpr uint32_t MM27_FAMILY = 43;
constexpr uint32_t MM28_FAMILY = 44;
constexpr uint32_t MM29_FAMILY = 45;
constexpr uint32_t MM30_FAMILY = 46;
constexpr uint32_t MM31_FAMILY = 47;
constexpr uint32_t GP8 = 0;
constexpr uint32_t GP16 = 1;
constexpr uint32_t GP32 = 2;
constexpr uint32_t GP64 = 3;
constexpr uint32_t XMM = 4;
constexpr uint32_t YMM = 5;
constexpr uint32_t ZMM = 6;
}
