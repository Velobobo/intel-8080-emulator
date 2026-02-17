#include "cpu/opcodes.h"

Opcode opcode_table[256] = {
    /* 00 */ {"NOP",1,4}, {"LXI B,d16",3,10}, {"STAX B",1,7}, {"INX B",1,5},
    {"INR B",1,5}, {"DCR B",1,5}, {"MVI B,d8",2,7}, {"RLC",1,4},
    {"NOP",1,4}, {"DAD B",1,10}, {"LDAX B",1,7}, {"DCX B",1,5},
    {"INR C",1,5}, {"DCR C",1,5}, {"MVI C,d8",2,7}, {"RRC",1,4},

    /* 10 */ {"NOP",1,4}, {"LXI D,d16",3,10}, {"STAX D",1,7}, {"INX D",1,5},
    {"INR D",1,5}, {"DCR D",1,5}, {"MVI D,d8",2,7}, {"RAL",1,4},
    {"NOP",1,4}, {"DAD D",1,10}, {"LDAX D",1,7}, {"DCX D",1,5},
    {"INR E",1,5}, {"DCR E",1,5}, {"MVI E,d8",2,7}, {"RAR",1,4},

    /* 20 */ {"NOP",1,4}, {"LXI H,d16",3,10}, {"SHLD adr",3,16}, {"INX H",1,5},
    {"INR H",1,5}, {"DCR H",1,5}, {"MVI H,d8",2,7}, {"DAA",1,4},
    {"NOP",1,4}, {"DAD H",1,10}, {"LHLD adr",3,16}, {"DCX H",1,5},
    {"INR L",1,5}, {"DCR L",1,5}, {"MVI L,d8",2,7}, {"CMA",1,4},

    /* 30 */ {"NOP",1,4}, {"LXI SP,d16",3,10}, {"STA adr",3,13}, {"INX SP",1,5},
    {"INR M",1,10}, {"DCR M",1,10}, {"MVI M,d8",2,10}, {"STC",1,4},
    {"NOP",1,4}, {"DAD SP",1,10}, {"LDA adr",3,13}, {"DCX SP",1,5},
    {"INR A",1,5}, {"DCR A",1,5}, {"MVI A,d8",2,7}, {"CMC",1,4},

    /* 40 */ {"MOV B,B",1,5}, {"MOV B,C",1,5}, {"MOV B,D",1,5}, {"MOV B,E",1,5},
    {"MOV B,H",1,5}, {"MOV B,L",1,5}, {"MOV B,M",1,7}, {"MOV B,A",1,5},
    {"MOV C,B",1,5}, {"MOV C,C",1,5}, {"MOV C,D",1,5}, {"MOV C,E",1,5},
    {"MOV C,H",1,5}, {"MOV C,L",1,5}, {"MOV C,M",1,7}, {"MOV C,A",1,5},

    /* 50 */ {"MOV D,B",1,5}, {"MOV D,C",1,5}, {"MOV D,D",1,5}, {"MOV D,E",1,5},
    {"MOV D,H",1,5}, {"MOV D,L",1,5}, {"MOV D,M",1,7}, {"MOV D,A",1,5},
    {"MOV E,B",1,5}, {"MOV E,C",1,5}, {"MOV E,D",1,5}, {"MOV E,E",1,5},
    {"MOV E,H",1,5}, {"MOV E,L",1,5}, {"MOV E,M",1,7}, {"MOV E,A",1,5},

    /* 60 */ {"MOV H,B",1,5}, {"MOV H,C",1,5}, {"MOV H,D",1,5}, {"MOV H,E",1,5},
    {"MOV H,H",1,5}, {"MOV H,L",1,5}, {"MOV H,M",1,7}, {"MOV H,A",1,5},
    {"MOV L,B",1,5}, {"MOV L,C",1,5}, {"MOV L,D",1,5}, {"MOV L,E",1,5},
    {"MOV L,H",1,5}, {"MOV L,L",1,5}, {"MOV L,M",1,7}, {"MOV L,A",1,5},

    /* 70 */ {"MOV M,B",1,7}, {"MOV M,C",1,7}, {"MOV M,D",1,7}, {"MOV M,E",1,7},
    {"MOV M,H",1,7}, {"MOV M,L",1,7}, {"HLT",1,7}, {"MOV M,A",1,7},
    {"MOV A,B",1,5}, {"MOV A,C",1,5}, {"MOV A,D",1,5}, {"MOV A,E",1,5},
    {"MOV A,H",1,5}, {"MOV A,L",1,5}, {"MOV A,M",1,7}, {"MOV A,A",1,5},

    /* 80 */ {"ADD B",1,4}, {"ADD C",1,4}, {"ADD D",1,4}, {"ADD E",1,4},
    {"ADD H",1,4}, {"ADD L",1,4}, {"ADD M",1,7}, {"ADD A",1,4},
    {"ADC B",1,4}, {"ADC C",1,4}, {"ADC D",1,4}, {"ADC E",1,4},
    {"ADC H",1,4}, {"ADC L",1,4}, {"ADC M",1,7}, {"ADC A",1,4},

    /* 90 */ {"SUB B",1,4}, {"SUB C",1,4}, {"SUB D",1,4}, {"SUB E",1,4},
    {"SUB H",1,4}, {"SUB L",1,4}, {"SUB M",1,7}, {"SUB A",1,4},
    {"SBB B",1,4}, {"SBB C",1,4}, {"SBB D",1,4}, {"SBB E",1,4},
    {"SBB H",1,4}, {"SBB L",1,4}, {"SBB M",1,7}, {"SBB A",1,4},

    /* A0 */ {"ANA B",1,4}, {"ANA C",1,4}, {"ANA D",1,4}, {"ANA E",1,4},
    {"ANA H",1,4}, {"ANA L",1,4}, {"ANA M",1,7}, {"ANA A",1,4},
    {"XRA B",1,4}, {"XRA C",1,4}, {"XRA D",1,4}, {"XRA E",1,4},
    {"XRA H",1,4}, {"XRA L",1,4}, {"XRA M",1,7}, {"XRA A",1,4},

    /* B0 */ {"ORA B",1,4}, {"ORA C",1,4}, {"ORA D",1,4}, {"ORA E",1,4},
    {"ORA H",1,4}, {"ORA L",1,4}, {"ORA M",1,7}, {"ORA A",1,4},
    {"CMP B",1,4}, {"CMP C",1,4}, {"CMP D",1,4}, {"CMP E",1,4},
    {"CMP H",1,4}, {"CMP L",1,4}, {"CMP M",1,7}, {"CMP A",1,4},

    /* C0 */ {"RNZ",1,5}, {"POP B",1,10}, {"JNZ adr",3,10}, {"JMP adr",3,10},
    {"CNZ adr",3,11}, {"PUSH B",1,11}, {"ADI d8",2,7}, {"RST 0",1,11},
    {"RZ",1,5}, {"RET",1,10}, {"JZ adr",3,10}, {"JMP adr",3,10},
    {"CZ adr",3,11}, {"CALL adr",3,17}, {"ACI d8",2,7}, {"RST 1",1,11},

    /* D0 */ {"RNC",1,5}, {"POP D",1,10}, {"JNC adr",3,10}, {"OUT d8",2,10},
    {"CNC adr",3,11}, {"PUSH D",1,11}, {"SUI d8",2,7}, {"RST 2",1,11},
    {"RC",1,5}, {"IN d8",2,10}, {"JC adr",3,10}, {"IN d8",2,10},
    {"CC adr",3,11}, {"CALL adr",3,17}, {"SBI d8",2,7}, {"RST 3",1,11},

    /* E0 */ {"RPO",1,5}, {"POP H",1,10}, {"JPO adr",3,10}, {"XTHL",1,18},
    {"CPO adr",3,11}, {"PUSH H",1,11}, {"ANI d8",2,7}, {"RST 4",1,11},
    {"RPE",1,5}, {"PCHL",1,5}, {"JPE adr",3,10}, {"XCHG",1,5},
    {"CPE adr",3,11}, {"CALL adr",3,17}, {"XRI d8",2,7}, {"RST 5",1,11},

    /* F0 */ {"RP",1,5}, {"POP PSW",1,10}, {"JP adr",3,10}, {"DI",1,4},
    {"CP adr",3,11}, {"PUSH PSW",1,11}, {"ORI d8",2,7}, {"RST 6",1,11},
    {"RM",1,5}, {"SPHL",1,5}, {"JM adr",3,10}, {"EI",1,4},
    {"CM adr",3,11}, {"CALL adr",3,17}, {"CPI d8",2,7}, {"RST 7",1,11},
};