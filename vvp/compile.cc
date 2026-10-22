/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */


# include  "config.h"
# include  "delay.h"
# include  "arith.h"
# include  "compile.h"
# include  "logic.h"
# include  "resolv.h"
# include  "udp.h"
# include  "symbols.h"
# include  "codes.h"
# include  "schedule.h"
# include  "vpi_priv.h"
# include  "parse_misc.h"
# include  "statistics.h"
# include  "schedule.h"
# include  <iostream>
# include  <list>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>

#ifdef __MINGW32__
#অন্তর্ভাব <windows.h>
#endif

# include  "ivl_alloc.h"

unsigned compile_errors = 0;

/*
 * The opcode table lists all the code mnemonics, along with their
 * opcode and operand types. The table must be sorted lexicographically
 * by mnemonic string: compile_code() uses bsearch() on this array.
 * If the order is wrong, lookup fails and the assembler reports
 * "Invalid opcode" for otherwise valid instructions (e.g. class
 * property queue ops must sort among all %delete*, %qpop*, %store*
 * names, not grouped by feature).
 * The opcode_compare function is a helper for that lookup.
 */

গণনা operand_e {
	/* Place holder for unused operand */
      OA_NONE,
	/* The operand is a number, an immediate unsigned integer */
      OA_সংখ্যা,
	/* The operand is a pointer to an array. */
      OA_ARR_PTR,
	/* The operand is a thread bit index or short integer */
      OA_BIT1,
      OA_BIT2,
	/* The operand is a pointer to code space */
      OA_CODE_PTR,
      OA_CODE_PTR2,
	/* The operand is a variable or net pointer */
      OA_FUNC_PTR,
	/* The operand is a second functor pointer */
      OA_FUNC_PTR2,
	/* The operand is a VPI handle */
      OA_VPI_PTR,
	/* String */
      OA_STRING
};

কাঠামো opcode_table_s {
      ধ্রুবক অক্ষর*mnemonic;
      vvp_code_fun opcode;

      unsigned argc;
      গণনা operand_e argt[OPERAND_MAX];
};

স্থির ধ্রুবক কাঠামো opcode_table_s opcode_table[] = {
      { "%abs/wr", of_ABS_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add",    of_ADD,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add/wr", of_ADD_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%addi",   of_ADDI,   3,  {OA_BIT1,     OA_BIT2,     OA_সংখ্যা} },
      { "%alloc",  of_ALLOC,  1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%and",    of_AND,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%and/r",  of_ANDR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%assign/ar",of_ASSIGN_AR,2,{OA_ARR_PTR,OA_BIT1,     OA_NONE} },
      { "%assign/ar/d",of_ASSIGN_ARD,2,{OA_ARR_PTR,OA_BIT1,  OA_NONE} },
      { "%assign/ar/e",of_ASSIGN_ARE,1,{OA_ARR_PTR,OA_NONE,  OA_NONE} },
      { "%assign/vec4",      of_ASSIGN_VEC4,       2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/a/d",  of_ASSIGN_VEC4_A_D,   3, {OA_ARR_PTR,  OA_BIT1, OA_BIT2} },
      { "%assign/vec4/a/e",  of_ASSIGN_VEC4_A_E,   2, {OA_ARR_PTR,  OA_BIT1, OA_NONE} },
      { "%assign/vec4/d",    of_ASSIGN_VEC4D,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/e",    of_ASSIGN_VEC4E,      1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%assign/vec4/off/d",of_ASSIGN_VEC4_OFF_D, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%assign/vec4/off/e",of_ASSIGN_VEC4_OFF_E, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr",  of_ASSIGN_WR, 2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/d",of_ASSIGN_WRD,2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/e",of_ASSIGN_WRE,1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%blend",     of_BLEND,     0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/str", of_BLEND_STR, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/wr",  of_BLEND_WR,  0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%breakpoint", of_BREAKPOINT, 0,  {OA_NONE, OA_NONE, OA_NONE} },
      { "%callf/obj",       of_CALLF_OBJ,       2,{OA_CODE_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/real",      of_CALLF_REAL,      2,{OA_CODE_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/str",       of_CALLF_STR,       2,{OA_CODE_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/vec4",      of_CALLF_VEC4,      2,{OA_CODE_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/void",      of_CALLF_VOID,      2,{OA_CODE_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%cassign/link",    of_CASSIGN_LINK,    2,{OA_FUNC_PTR,OA_FUNC_PTR2,OA_NONE} },
      { "%cassign/vec4",    of_CASSIGN_VEC4,    1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cassign/vec4/off",of_CASSIGN_VEC4_OFF,2,{OA_FUNC_PTR,OA_BIT1,     OA_NONE} },
      { "%cassign/wr",  of_CASSIGN_WR,  1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cast/vec2/dar", of_CAST_VEC2_DAR, 1,  {OA_সংখ্যা,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/dar", of_CAST_VEC4_DAR, 1,  {OA_সংখ্যা,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/str", of_CAST_VEC4_STR, 1,  {OA_সংখ্যা,   OA_NONE,     OA_NONE} },
      { "%cast2",   of_CAST2,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/e",   of_CMPE,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/ne",  of_CMPNE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/s",   of_CMPS,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/str", of_CMPSTR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/u",   of_CMPU,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/we",  of_CMPWE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wne", of_CMPWNE, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wr",  of_CMPWR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/x",   of_CMPX,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/z",   of_CMPZ,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmpi/e",  of_CMPIE,  3,  {OA_BIT1,     OA_BIT2,     OA_সংখ্যা} },
      { "%cmpi/ne", of_CMPINE, 3,  {OA_BIT1,     OA_BIT2,     OA_সংখ্যা} },
      { "%cmpi/s",  of_CMPIS,  3,  {OA_BIT1,     OA_BIT2,     OA_সংখ্যা} },
      { "%cmpi/u",  of_CMPIU,  3,  {OA_BIT1,     OA_BIT2,     OA_সংখ্যা} },
      { "%cmpix/ltu", of_CMPIX_LTU, 2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%cmpix/slt0", of_CMPIX_SLT0, 1, {OA_সংখ্যা, OA_NONE, OA_NONE} },
      { "%concat/str",  of_CONCAT_STR,  0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concat/vec4", of_CONCAT_VEC4, 0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concati/str", of_CONCATI_STR, 1,{OA_STRING,OA_NONE,  OA_NONE} },
      { "%concati/vec4",of_CONCATI_VEC4,3,{OA_BIT1,  OA_BIT2,  OA_সংখ্যা} },
      { "%cvt/rv",   of_CVT_RV,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/rv/s", of_CVT_RV_S,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/sr", of_CVT_SR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/ur", of_CVT_UR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/vr", of_CVT_VR, 1,  {OA_সংখ্যা,   OA_NONE,     OA_NONE} },
      { "%deassign",of_DEASSIGN,3,{OA_FUNC_PTR, OA_BIT1,     OA_BIT2} },
      { "%deassign/wr",of_DEASSIGN_WR,1,{OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%debug/thr",  of_DEBUG_THR,  1,{OA_STRING,   OA_NONE,     OA_NONE} },
      { "%delay",  of_DELAY,  2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%delayx", of_DELAYX, 1,  {OA_সংখ্যা,   OA_NONE,     OA_NONE} },
      { "%delete/elem",of_DELETE_ELEM,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%delete/obj",of_DELETE_OBJ,1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%delete/prop/elem", of_DELETE_PROP_ELEM, 1, {OA_সংখ্যা, OA_NONE, OA_NONE} },
      { "%delete/prop/obj",  of_DELETE_PROP_OBJ,  1, {OA_সংখ্যা, OA_NONE, OA_NONE} },
      { "%delete/tail",of_DELETE_লেজ,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%disable",  of_DISABLE, 1, {OA_VPI_PTR,OA_NONE,     OA_NONE} },
      { "%disable/flow", of_DISABLE_FLOW, 1, {OA_VPI_PTR,OA_NONE, OA_NONE} },
      { "%disable/fork",of_DISABLE_FORK,0,{OA_NONE,OA_NONE,  OA_NONE} },
      { "%div",      of_DIV,     0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/s",    of_DIV_S,   0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/wr",   of_DIV_WR,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/obj",  of_DUP_OBJ, 0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/real", of_DUP_REAL,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/vec4", of_DUP_VEC4,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%end",    of_END,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl",  of_EVCTL,  2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/c",of_EVCTLC, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl/i",of_EVCTLI, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/s",of_EVCTLS, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%event",    of_EVENT,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%event/nb", of_EVENT_NB, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%flag_get/vec4", of_FLAG_GET_VEC4, 1, {OA_সংখ্যা, OA_NONE, OA_NONE} },
      { "%flag_inv",      of_FLAG_INV,      1, {OA_BIT1,   OA_NONE, OA_NONE} },
      { "%flag_mov",      of_FLAG_MOV,      2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_or",       of_FLAG_OR,       2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_set/imm",  of_FLAG_SET_IMM,  2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%flag_set/vec4", of_FLAG_SET_VEC4, 1, {OA_সংখ্যা, OA_NONE, OA_NONE} },
      { "%force/link",    of_FORCE_LINK,2,{OA_FUNC_PTR, OA_FUNC_PTR2, OA_NONE} },
      { "%force/vec4",    of_FORCE_VEC4,    1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%force/vec4/off",of_FORCE_VEC4_OFF,2,{OA_FUNC_PTR, OA_BIT1,      OA_NONE} },
      { "%force/vec4/off/d",of_FORCE_VEC4_OFF_D,3,{OA_FUNC_PTR, OA_BIT1,  OA_BIT2} },
      { "%force/wr",      of_FORCE_WR,      1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%fork",   of_FORK,   2,  {OA_CODE_PTR2,OA_VPI_PTR,  OA_NONE} },
      { "%free",   of_FREE,   1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%inv",    of_INV,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%ix/add", of_IX_ADD, 3,  {OA_সংখ্যা,   OA_BIT1,     OA_BIT2} },
      { "%ix/getv",of_IX_GETV,2,  {OA_BIT1,     OA_FUNC_PTR, OA_NONE} },
      { "%ix/getv/s",of_IX_GETV_S,2, {OA_BIT1,   OA_FUNC_PTR, OA_NONE} },
      { "%ix/load",of_IX_LOAD,3,  {OA_সংখ্যা,   OA_BIT1,     OA_BIT2} },
      { "%ix/mov", of_IX_MOV, 2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%ix/mul", of_IX_MUL, 3,  {OA_সংখ্যা,   OA_BIT1,     OA_BIT2} },
      { "%ix/sub", of_IX_SUB, 3,  {OA_সংখ্যা,   OA_BIT1,     OA_BIT2} },
      { "%ix/vec4",  of_IX_VEC4,  1,  {OA_সংখ্যা,   OA_NONE, OA_NONE} },
      { "%ix/vec4/s",of_IX_VEC4_S,1,  {OA_সংখ্যা,   OA_NONE, OA_NONE} },
      { "%jmp",    of_JMP,    1,  {OA_CODE_PTR, OA_NONE,     OA_NONE} },
      { "%jmp/0",  of_JMP0,   2,  {OA_CODE_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/0xz",of_JMP0XZ, 2,  {OA_CODE_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1",  of_JMP1,   2,  {OA_CODE_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1xz",of_JMP1XZ, 2,  {OA_CODE_PTR, OA_BIT1,     OA_NONE} },
      { "%join",   of_JOIN,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%join/detach",of_JOIN_DETACH,1,{OA_সংখ্যা,OA_NONE,  OA_NONE} },
      { "%load/ar",of_LOAD_AR,2,  {OA_ARR_PTR,  OA_BIT1,     OA_NONE} },
      { "%load/dar/r",  of_LOAD_DAR_R,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE}},
      { "%load/dar/str",of_LOAD_DAR_STR,  1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/dar/vec4",of_LOAD_DAR_VEC4,1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/obj",   of_LOAD_OBJ,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/obja",  of_LOAD_OBJA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/prop/dar/vec4", of_LOAD_PROP_DAR_VEC4, 2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%load/real",  of_LOAD_REAL, 1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%load/str",   of_LOAD_STR,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/stra",  of_LOAD_STRA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/vec4",  of_LOAD_VEC4, 1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%load/vec4a", of_LOAD_VEC4A,2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%max/wr", of_MAX_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%min/wr", of_MIN_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod",    of_MOD,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/s",  of_MOD_S,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/wr", of_MOD_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul",    of_MUL,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul/wr", of_MUL_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%muli",   of_MULI,   3,  {OA_BIT1,     OA_BIT2,     OA_সংখ্যা} },
      { "%nand",   of_NAND,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nand/r", of_NANDR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%neg/wr", of_NEG_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%new/cobj",  of_NEW_COBJ,  1, {OA_VPI_PTR,OA_NONE,  OA_NONE} },
      { "%new/darray",of_NEW_DARRAY,2, {OA_BIT1,   OA_STRING,OA_NONE} },
      { "%noop",   of_NOOP,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor",    of_NOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor/r",  of_NORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%null",   of_NULL,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or",     of_OR,     0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or/r",   of_ORR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%pad/s",  of_PAD_S,  1,  {OA_সংখ্যা,   OA_NONE,     OA_NONE} },
      { "%pad/u",  of_PAD_U,  1,  {OA_সংখ্যা,   OA_NONE,     OA_NONE} },
      { "%part/s", of_PART_S, 1,  {OA_সংখ্যা,   OA_NONE,     OA_NONE} },
      { "%part/u", of_PART_U, 1,  {OA_সংখ্যা,   OA_NONE,     OA_NONE} },
      { "%parti/s",of_PARTI_S,3,  {OA_সংখ্যা,   OA_BIT1,     OA_BIT2} },
      { "%parti/u",of_PARTI_U,3,  {OA_সংখ্যা,   OA_BIT1,     OA_BIT2} },
      { "%pop/obj", of_বের_করো_OBJ, 2, {OA_BIT1,    OA_BIT2,     OA_NONE} },
      { "%pop/real",of_বের_করো_REAL,1, {OA_সংখ্যা,  OA_NONE,     OA_NONE} },
      { "%pop/str", of_বের_করো_STR, 1, {OA_সংখ্যা,  OA_NONE,     OA_NONE} },
      { "%pop/vec4",of_বের_করো_VEC4,1, {OA_সংখ্যা,  OA_NONE,     OA_NONE} },
      { "%pow",     of_POW,     0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/s",   of_POW_S,   0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/wr",  of_POW_WR,  0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%prop/obj",of_PROP_OBJ,2,  {OA_সংখ্যা,   OA_BIT1,     OA_NONE} },
      { "%prop/queue/size", of_PROP_QUEUE_আকার,  1, {OA_সংখ্যা, OA_NONE, OA_NONE} },
      { "%prop/r",  of_PROP_R,  1,  {OA_সংখ্যা,   OA_NONE,     OA_NONE} },
      { "%prop/str",of_PROP_STR,1,  {OA_সংখ্যা,   OA_NONE,     OA_NONE} },
      { "%prop/v",  of_PROP_V,  1,  {OA_সংখ্যা,   OA_NONE,     OA_NONE} },
      { "%push/ix/vec4", of_ঠেলো_IX_VEC4, 3, {OA_সংখ্যা, OA_BIT1, OA_BIT2} },
      { "%pushi/real",of_PUSHI_REAL,2,{OA_BIT1,   OA_BIT2,   OA_NONE} },
      { "%pushi/str", of_PUSHI_STR, 1,{OA_STRING, OA_NONE,   OA_NONE} },
      { "%pushi/vec4",of_PUSHI_VEC4,3,{OA_BIT1,   OA_BIT2,   OA_সংখ্যা} },
      { "%pushv/str", of_PUSHV_STR, 0,{OA_NONE,   OA_NONE,   OA_NONE} },
      { "%putc/str/vec4",of_PUTC_STR_VEC4,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/prop/r",   of_QINSERT_PROP_R,   2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/str", of_QINSERT_PROP_STR, 2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/v",   of_QINSERT_PROP_V,   3, {OA_সংখ্যা, OA_BIT1, OA_BIT2} },
      { "%qinsert/real",of_QINSERT_REAL,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/str", of_QINSERT_STR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/v",   of_QINSERT_V,   3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%qpop/b/real",of_QPOP_B_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/str", of_QPOP_B_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/v",   of_QPOP_B_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/f/real",of_QPOP_F_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/str", of_QPOP_F_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/v",   of_QPOP_F_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/prop/b/r",   of_QPOP_PROP_B_REAL, 1, {OA_সংখ্যা, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/str", of_QPOP_PROP_B_STR, 1, {OA_সংখ্যা, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/v",   of_QPOP_PROP_B_V,    2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%qpop/prop/f/r",   of_QPOP_PROP_F_REAL, 1, {OA_সংখ্যা, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/str", of_QPOP_PROP_F_STR, 1, {OA_সংখ্যা, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/v",   of_QPOP_PROP_F_V,    2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%queue/append_word/v", of_QUEUE_APPEND_WORD_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/find/index/prop/v", of_QUEUE_FIND_সূচক_PROP_V, 2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%queue/find/index/v", of_QUEUE_FIND_সূচক_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find/prop/v", of_QUEUE_FIND_PROP_V, 2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%queue/find/v", of_QUEUE_FIND_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/prop/v", of_QUEUE_FIND_FIRST_সূচক_PROP_V, 2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/v", of_QUEUE_FIND_FIRST_সূচক_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/prop/v", of_QUEUE_FIND_FIRST_PROP_V, 2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%queue/find_first/v", of_QUEUE_FIND_FIRST_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/prop/v", of_QUEUE_FIND_LAST_সূচক_PROP_V, 2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/v", of_QUEUE_FIND_LAST_সূচক_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/prop/v", of_QUEUE_FIND_LAST_PROP_V, 2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%queue/find_last/v", of_QUEUE_FIND_LAST_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/max/obj/v", of_QUEUE_MAX_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/max/prop/v", of_QUEUE_MAX_PROP_V, 2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%queue/max/v", of_QUEUE_MAX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/min/obj/v", of_QUEUE_MIN_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/min/prop/v", of_QUEUE_MIN_PROP_V, 2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%queue/min/v", of_QUEUE_MIN_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/new_empty/v", of_QUEUE_NEW_EMPTY_V, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%queue/size/v", of_QUEUE_আকার_V, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%queue/unique/index/obj/v", of_QUEUE_UNIQUE_সূচক_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/index/prop/v", of_QUEUE_UNIQUE_সূচক_PROP_V, 2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%queue/unique/index/v", of_QUEUE_UNIQUE_সূচক_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/unique/obj/v", of_QUEUE_UNIQUE_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/prop/v", of_QUEUE_UNIQUE_PROP_V, 2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%queue/unique/v", of_QUEUE_UNIQUE_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/word/prop/v", of_QUEUE_WORD_PROP_V, 3, {OA_সংখ্যা, OA_BIT1, OA_BIT2} },
      { "%queue/word/v", of_QUEUE_WORD_V, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%release/net",of_RELEASE_NET,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/reg",of_RELEASE_REG,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/wr", of_RELEASE_WR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%replicate", of_REPLICATE,   1,{OA_সংখ্যা,  OA_NONE,OA_NONE} },
      { "%ret/real",  of_RET_REAL,    1,{OA_সংখ্যা,  OA_NONE,OA_NONE} },
      { "%ret/str",   of_RET_STR,     1,{OA_সংখ্যা,  OA_NONE,OA_NONE} },
      { "%ret/vec4",  of_RET_VEC4,    3,{OA_সংখ্যা,  OA_BIT1,OA_BIT2} },
      { "%retload/real",of_RETLOAD_REAL,1,{OA_সংখ্যা,  OA_NONE,OA_NONE} },
      { "%retload/str", of_RETLOAD_STR, 1,{OA_সংখ্যা,  OA_NONE,OA_NONE} },
      { "%retload/vec4",of_RETLOAD_VEC4,1,{OA_সংখ্যা,  OA_NONE,OA_NONE} },
      { "%reverse/obj", of_REVERSE_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%reverse/prop/obj", of_REVERSE_PROP_OBJ, 1, {OA_সংখ্যা, OA_NONE, OA_NONE} },
      { "%rsort/obj", of_RSORT_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%rsort/prop/obj", of_RSORT_PROP_OBJ, 1, {OA_সংখ্যা, OA_NONE, OA_NONE} },
      { "%scopy",  of_SCOPY,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%set/dar/obj/real",of_SET_DAR_OBJ_REAL,1,{OA_সংখ্যা,OA_NONE,OA_NONE} },
      { "%set/dar/obj/str", of_SET_DAR_OBJ_STR, 1,{OA_সংখ্যা,OA_NONE,OA_NONE} },
      { "%set/dar/obj/vec4",of_SET_DAR_OBJ_VEC4,1,{OA_সংখ্যা,OA_NONE,OA_NONE} },
      { "%shiftl",   of_SHIFTL,   1, {OA_সংখ্যা, OA_NONE,   OA_NONE} },
      { "%shiftr",   of_SHIFTR,   1, {OA_সংখ্যা, OA_NONE,   OA_NONE} },
      { "%shiftr/s", of_SHIFTR_S, 1, {OA_সংখ্যা, OA_NONE,   OA_NONE} },
      { "%shuffle/obj", of_SHUFFLE_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%shuffle/prop/obj", of_SHUFFLE_PROP_OBJ, 1, {OA_সংখ্যা, OA_NONE, OA_NONE} },
      { "%sort/obj", of_SORT_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%sort/prop/obj", of_SORT_PROP_OBJ, 1, {OA_সংখ্যা, OA_NONE, OA_NONE} },
      { "%split/vec4",    of_SPLIT_VEC4,    1,{OA_সংখ্যা,   OA_NONE, OA_NONE} },
      { "%store/dar/r",   of_STORE_DAR_R,   1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/str", of_STORE_DAR_STR, 1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/vec4",of_STORE_DAR_VEC4,1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/obj",   of_STORE_OBJ,   1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/obja",  of_STORE_OBJA,  2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/prop/obj",of_STORE_PROP_OBJ,2, {OA_সংখ্যা,  OA_BIT1, OA_NONE} },
      { "%store/prop/qb/r",   of_STORE_PROP_QB_R,   2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/str", of_STORE_PROP_QB_STR, 2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/v",   of_STORE_PROP_QB_V,   3, {OA_সংখ্যা, OA_BIT1, OA_BIT2} },
      { "%store/prop/qf/r",   of_STORE_PROP_QF_R,   2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/str", of_STORE_PROP_QF_STR, 2, {OA_সংখ্যা, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/v",   of_STORE_PROP_QF_V,   3, {OA_সংখ্যা, OA_BIT1, OA_BIT2} },
      { "%store/prop/r",  of_STORE_PROP_R,  1, {OA_সংখ্যা,  OA_NONE, OA_NONE} },
      { "%store/prop/str",of_STORE_PROP_STR,1, {OA_সংখ্যা,  OA_NONE, OA_NONE} },
      { "%store/prop/v",  of_STORE_PROP_V,  2, {OA_সংখ্যা,  OA_BIT1, OA_NONE} },
      { "%store/qb/r",   of_STORE_QB_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/str", of_STORE_QB_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/v",   of_STORE_QB_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qdar/r",  of_STORE_QDAR_R,  2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/str",of_STORE_QDAR_STR,2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/v",  of_STORE_QDAR_V,  3,{OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qf/r",   of_STORE_QF_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/str", of_STORE_QF_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/v",   of_STORE_QF_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qobj/r",  of_STORE_QOBJ_R,  2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/str",of_STORE_QOBJ_STR,2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/v",  of_STORE_QOBJ_V,  3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/real",    of_STORE_REAL,    1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/reala",   of_STORE_REALA,   2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/str",     of_STORE_STR,     1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/stra",    of_STORE_STRA,    2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/vec4",    of_STORE_VEC4,    3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/vec4a",   of_STORE_VEC4A,   3, {OA_ARR_PTR, OA_BIT1, OA_BIT2} },
      { "%sub",    of_SUB,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%sub/wr", of_SUB_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%subi",   of_SUBI,   3,  {OA_BIT1,     OA_BIT2,     OA_সংখ্যা} },
      { "%substr",     of_SUBSTR,     2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%substr/vec4",of_SUBSTR_VEC4,2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%test_nul",     of_TEST_NUL,     1,{OA_FUNC_PTR,OA_NONE,    OA_NONE} },
      { "%test_nul/a",   of_TEST_NUL_A,   2,{OA_ARR_PTR, OA_BIT1,    OA_NONE} },
      { "%test_nul/obj", of_TEST_NUL_OBJ, 0,{OA_NONE,    OA_NONE,    OA_NONE} },
      { "%test_nul/prop",of_TEST_NUL_PROP,2,{OA_সংখ্যা,  OA_BIT1,    OA_NONE} },
      { "%uparr/and/v",          of_UPARR_AND_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/or/v",           of_UPARR_OR_V,           2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/product/obj/v",  of_UPARR_PRODUCT_OBJ_V,  1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/product/prop/v", of_UPARR_PRODUCT_PROP_V, 2, {OA_সংখ্যা,   OA_BIT1, OA_NONE} },
      { "%uparr/product/r",      of_UPARR_PRODUCT_REAL,   1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/product/v",      of_UPARR_PRODUCT_V,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/sum/obj/v",      of_UPARR_যোগ_OBJ_V,      1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/sum/prop/v",     of_UPARR_যোগ_PROP_V,     2, {OA_সংখ্যা,   OA_BIT1, OA_NONE} },
      { "%uparr/sum/r",          of_UPARR_যোগ_REAL,       1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/sum/v",          of_UPARR_যোগ_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/xor/v",          of_UPARR_XOR_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%wait",   of_WAIT,   1,  {OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%wait/fork",of_WAIT_FORK,0,{OA_NONE,   OA_NONE,     OA_NONE} },
      { "%xnor",   of_XNOR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xnor/r", of_XNORR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor",    of_XOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor/r",  of_XORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { 0, of_NOOP, 0, {OA_NONE, OA_NONE, OA_NONE} }
};

স্থির ধ্রুবক unsigned opcode_গুনতি =
                    আকার(opcode_table)/আকার(*opcode_table) - 1;

স্থির পূর্ণসংখ্যা opcode_compare(ধ্রুবক শূন্য*k, ধ্রুবক শূন্য*r)
{
      ধ্রুবক অক্ষর*kp = static_cast<ধ্রুবক অক্ষর*>(k);
      ধ্রুবক কাঠামো opcode_table_s*rp = static_cast<ধ্রুবক কাঠামো opcode_table_s*>(r);
      ফেরত strcmp(kp, rp->mnemonic);
}

/*
 * Keep a symbol table of addresses within code space. Labels on
 * executable opcodes are mapped to their address here.
 */
স্থির প্রতীক_table_t sym_codespace = 0;

/*
 * Keep a symbol table of functors mentioned in the source. This table
 * is used to resolve references as they come.
 */
স্থির প্রতীক_table_t sym_functors = 0;

/*
 * VPI objects are indexed during compile time so that they can be
 * linked together as they are created. This symbol table matches
 * labels to vpiHandles.
 */
স্থির প্রতীক_table_t sym_vpi = 0;


/*
 * If a functor parameter makes a forward reference to a functor, then
 * I need to save that reference and resolve it after the functors are
 * created. Use this structure to keep the unresolved references in an
 * unsorted singly linked list.
 *
 * The postpone_functor_input arranges for a functor input to be
 * resolved and connected at cleanup. This is used if the symbol is
 * defined after its use in a functor. The ptr parameter is the
 * complete vvp_input_t for the input port.
 */


/*
 *  Add a functor to the symbol table
 */

শূন্য define_functor_প্রতীক(ধ্রুবক অক্ষর*label, vvp_net_t*net)
{
      প্রতীক_মান_t val;
      val.net = net;
      sym_set_মান(sym_functors, label, val);
}

স্থির vvp_net_t*lookup_functor_প্রতীক(ধ্রুবক অক্ষর*label)
{
      প্রতিজ্ঞা(sym_functors);
      প্রতীক_মান_t val = sym_get_মান(sym_functors, label);
      ফেরত val.net;
}

vpiHandle vvp_lookup_handle(ধ্রুবক অক্ষর*label)
{
      প্রতীক_মান_t val = sym_get_মান(sym_vpi, label);
      যদি (val.ptr) ফেরত (vpiHandle) val.ptr;
      ফেরত 0;
}

vvp_net_t* vvp_net_lookup(ধ্রুবক অক্ষর*label)
{
      স্থির যৌক্তিক t0_trigger_generated = মিথ্যা;

        /* First, look to see if the symbol is a vpi object of some
	   sort. If it is, then get the vvp_ipoint_t pointer out of
	   the vpiHandle. */
      প্রতীক_মান_t val = sym_get_মান(sym_vpi, label);
      যদি (val.ptr) {
	    vpiHandle vpi = static_cast<vpiHandle>(val.ptr);
	    নির্বাচন (vpi->get_type_code()) {
		ক্ষেত্র vpiNet:
		ক্ষেত্র vpiReg:
		ক্ষেত্র vpiBitVar:
		ক্ষেত্র vpiByteVar:
		ক্ষেত্র vpiShortIntVar:
		ক্ষেত্র vpiIntVar:
		ক্ষেত্র vpiLongIntVar:
		ক্ষেত্র vpiIntegerVar: {
		      ধ্রুবক __vpiSignal*sig = dynamic_cast<__vpiSignal*>(vpi);
		      ফেরত sig->গ্রন্থি;
		}

		ক্ষেত্র vpiRealVar: {
		      ধ্রুবক __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(vpi);
		      ফেরত sig->net;
		}

		ক্ষেত্র vpiStringVar:
		ক্ষেত্র vpiArrayVar:
		ক্ষেত্র vpiClassVar: {
		      ধ্রুবক __vpiBaseVar*sig = dynamic_cast<__vpiBaseVar*>(vpi);
		      ফেরত sig->get_net();
		}

		ক্ষেত্র vpiNamedEvent: {
		      ধ্রুবক __vpiNamedEvent*tmp = dynamic_cast<__vpiNamedEvent*>(vpi);
		      ফেরত tmp->funct;
		}

		পূর্বনির্ধারিত:
		  fprintf(stderr, "Unsupported type %d.\n",
		          vpi->get_type_code());
		  প্রতিজ্ঞা(0);
	    }
      }


	/* Failing that, look for a general functor. */
      vvp_net_t*tmp = lookup_functor_প্রতীক(label);

	// This is a special label used to create a T0 trigger for the
	// always_comb/latch processes.
     যদি (! t0_trigger_generated && (strcmp(label, "E_0x0") == 0)) {
	      // This should never happen, but if it does then the E_0x0
	      // event generation may need to be explicitly generated in
	      // the compiler output instead of implicitly in this code.
	    প্রতিজ্ঞা(! vpip_peek_current_scope()->is_automatic());
	    t0_trigger_generated = সত্য;
	      // Create a local event with no name for the T0 trigger
	    compile_named_event(strdup(label), strcpy(নতুন অক্ষর [1],""), সত্য);
	    tmp = vvp_net_lookup(label);
	    প্রতিজ্ঞা(tmp);
	      // Create a trigger for the T0 event.
	    vvp_net_ptr_t ptr (tmp, 0);
	    schedule_t0_trigger(ptr);
     }

      ফেরত tmp;
}

/*
 * The resolv_list_s is the base class for a symbol resolve action, and
 * the resolv_list is an unordered list of these resolve actions. Some
 * function creates an instance of a resolv_list_s object that
 * contains the data pertinent to that resolution request, and
 * executes it with the resolv_submit function. If the operation can
 * complete, then the resolv_submit deletes the object. Otherwise, it
 * pushes it onto the resolv_list for later processing.
 *
 * Derived classes implement the resolve function to perform the
 * actual binding or resolution that the instance requires. If the
 * function succeeds, the resolve method returns true and the object
 * can be deleted any time.
 *
 * The mes parameter of the resolve method tells the resolver that
 * this call is its last chance. If it cannot complete the operation,
 * it must print an error message and return false.
 */
স্থির resolv_list_s*resolv_list = 0;

resolv_list_s::~resolv_list_s()
{
      মুক্ত(label_);
}

শূন্য resolv_submit(resolv_list_s*cur)
{
      যদি (cur->resolve()) {
	    delete cur;
	    ফেরত;
      }

      cur->পরবর্তী_2 = resolv_list;
      resolv_list = cur;
}


/*
 * Look up vvp_nets in the symbol table. The "source" is the label for
 * the net that I want to feed, and net->port[port] is the vvp_net
 * input that I want that node to feed into. When the name is found,
 * put net->port[port] into the fan-out list for that node.
 */
কাঠামো vvp_net_resolv_list_s: সর্বজনীন resolv_list_s {

      explicit vvp_net_resolv_list_s(অক্ষর*l) : resolv_list_s(l) { }
	// port to be driven by the located node.
      vvp_net_ptr_t port;
      আভাসী যৌক্তিক resolve(যৌক্তিক mes) override;
};

যৌক্তিক vvp_net_resolv_list_s::resolve(যৌক্তিক mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      যদি (tmp) {
	      // Link the input port to the located output.
	    tmp->link(port);
	    ফেরত সত্য;
      }

      যদি (mes)
	    fprintf(stderr, "unresolved vvp_net reference: %s\n", label());

      ফেরত মিথ্যা;
}

অন্তঃরেখ স্থির
শূন্য postpone_functor_প্রবেশ(vvp_net_ptr_t port, অক্ষর*lab)
{
      কাঠামো vvp_net_resolv_list_s*res = নতুন কাঠামো vvp_net_resolv_list_s(lab);
      res->port   = port;

      resolv_submit(res);
}


/*
 *  Generic functor reference lookup.
 */

কাঠামো functor_gen_resolv_list_s: সর্বজনীন resolv_list_s {
      explicit functor_gen_resolv_list_s(অক্ষর*txt) : resolv_list_s(txt) {
	    ref = 0;
      }
      vvp_net_t**ref;
      আভাসী যৌক্তিক resolve(যৌক্তিক mes) override;
};

যৌক্তিক functor_gen_resolv_list_s::resolve(যৌক্তিক mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      যদি (tmp) {
	    *ref = tmp;
	    ফেরত সত্য;
      }

      যদি (mes)
	    fprintf(stderr, "unresolved functor reference: %s\n", label());

      ফেরত মিথ্যা;
}

শূন্য functor_ref_lookup(vvp_net_t**ref, অক্ষর*lab)
{
      কাঠামো functor_gen_resolv_list_s*res =
	    নতুন কাঠামো functor_gen_resolv_list_s(lab);

      res->ref    = ref;

      resolv_submit(res);
}

/*
 *  vpiHandle lookup
 */

কাঠামো vpi_handle_resolv_list_s: সর্বজনীন resolv_list_s {
      explicit vpi_handle_resolv_list_s(অক্ষর*lab) : resolv_list_s(lab) {
	    handle = ফাঁকা;
      }
      আভাসী যৌক্তিক resolve(যৌক্তিক mes) override;
      vpiHandle *handle;
};

যৌক্তিক vpi_handle_resolv_list_s::resolve(যৌক্তিক mes)
{
      প্রতীক_মান_t val = sym_get_মান(sym_vpi, label());
      যদি (!val.ptr) {
	    // check for thread access symbols
	    unsigned base, wid;
	    size_t n = 0;
	    অক্ষর ss[32];
	    যদি (2 == sscanf(label(), "W<%u,%31[r]>%zn", &base, ss, &n)
		       && n == strlen(label())) {

		  val.ptr = vpip_make_vthr_word(base, ss);
		  sym_set_মান(sym_vpi, label(), val);

	    } অন্যথা যদি (1 == sscanf(label(), "S<%u,str>%zn", &base, &n)
		       && n == strlen(label())) {

		  val.ptr = vpip_make_vthr_str_স্তূপ(base);
		  sym_set_মান(sym_vpi, label(), val);

	    } অন্যথা যদি (3 == sscanf(label(), "S<%u,vec4,%31[su]%u>%zn", &base, ss, &wid, &n)
		       && n == strlen(label())) {

		  যৌক্তিক signed_flag = মিথ্যা;
		  জন্য (অক্ষর*fp = ss ;  *fp ;  fp += 1) নির্বাচন (*fp) {
		      ক্ষেত্র 's':
			signed_flag = সত্য;
			বিরতি;
		      ক্ষেত্র 'u':
			signed_flag = মিথ্যা;
			বিরতি;
		      পূর্বনির্ধারিত:
			বিরতি;
		  }
		  val.ptr = vpip_make_vthr_vec4_স্তূপ(base, signed_flag, wid);
		  sym_set_মান(sym_vpi, label(), val);
	    }

      }

      যদি (!val.ptr) {
	    // check for memory word  M<mem,base,wid>
      }

      যদি (strcmp(label(), "null") == 0) {
	    val.ptr = vpip_make_null_const();
	    sym_set_মান(sym_vpi, label(), val);
      }

      যদি (val.ptr) {
	    *handle = static_cast<vpiHandle>(val.ptr);
	    ফেরত সত্য;
      }

      যদি (mes)
	    fprintf(stderr, "unresolved vpi name lookup: %s\n", label());

      ফেরত মিথ্যা;
}

শূন্য compile_vpi_lookup(vpiHandle *handle, অক্ষর*label)
{
      যদি (strcmp(label, "$time") == 0) {
	    *handle = vpip_sim_time(vpip_peek_current_scope(), মিথ্যা);
	    মুক্ত(label);
	    ফেরত;
      }

      যদি (strcmp(label, "$stime") == 0) {
	    *handle = vpip_sim_time(vpip_peek_current_scope(), সত্য);
	    মুক্ত(label);
	    ফেরত;
      }

      যদি (strcmp(label, "$realtime") == 0) {
	    *handle = vpip_sim_realtime(vpip_peek_current_scope());
	    মুক্ত(label);
	    ফেরত;
      }

      যদি (strcmp(label, "$simtime") == 0) {
	    *handle = vpip_sim_time(0, মিথ্যা);
	    মুক্ত(label);
	    ফেরত;
      }

      কাঠামো vpi_handle_resolv_list_s*res
	    = নতুন কাঠামো vpi_handle_resolv_list_s(label);

      res->handle = handle;
      resolv_submit(res);
}

/*
 * Code Label lookup
 */

কাঠামো code_label_resolv_list_s: সর্বজনীন resolv_list_s {
      explicit code_label_resolv_list_s(অক্ষর*lab, যৌক্তিক cptr2) : resolv_list_s(lab) {
	    code = ফাঁকা;
	    cptr2_flag = cptr2;
      }
      কাঠামো vvp_code_s *code;
      যৌক্তিক cptr2_flag;
      আভাসী যৌক্তিক resolve(যৌক্তিক mes) override;
};

যৌক্তিক code_label_resolv_list_s::resolve(যৌক্তিক mes)
{
      প্রতীক_মান_t val = sym_get_মান(sym_codespace, label());
      যদি (val.ptr) {
	    যদি (cptr2_flag)
		  code->cptr2 = reinterpret_cast<vvp_code_t>(val.ptr);
	    অন্যথা
		  code->cptr = reinterpret_cast<vvp_code_t>(val.ptr);
	    ফেরত সত্য;
      }

      যদি (mes)
	    fprintf(stderr, "unresolved code label: %s\n", label());

      ফেরত মিথ্যা;
}

শূন্য code_label_lookup(কাঠামো vvp_code_s *code, অক্ষর *label, যৌক্তিক cptr2)
{
      কাঠামো code_label_resolv_list_s *res
	    = নতুন কাঠামো code_label_resolv_list_s(label, cptr2);

      res->code  = code;

      resolv_submit(res);
}

কাঠামো code_array_resolv_list_s: সর্বজনীন resolv_list_s {
      explicit code_array_resolv_list_s(অক্ষর*lab) : resolv_list_s(lab) {
	    code = ফাঁকা;
      }
      কাঠামো vvp_code_s *code;
      আভাসী যৌক্তিক resolve(যৌক্তিক mes) override;
};

যৌক্তিক code_array_resolv_list_s::resolve(যৌক্তিক mes)
{
      code->array = array_find(label());
      যদি (code->array != 0) {
	    ফেরত সত্য;
      }

      যদি (mes)
	    fprintf(stderr, "Array unresolved: %s\n", label());

      ফেরত মিথ্যা;
}

স্থির শূন্য compile_array_lookup(কাঠামো vvp_code_s*code, অক্ষর*label)
{
      কাঠামো code_array_resolv_list_s *res
	    = নতুন কাঠামো code_array_resolv_list_s(label);

      res->code  = code;

      resolv_submit(res);
}

স্থির std::list<কাঠামো __vpiSysTaskCall*> scheduled_compiletf;

শূন্য compile_compiletf(কাঠামো __vpiSysTaskCall*obj)
{
      যদি (obj->defn->info.compiletf == 0)
	    ফেরত;

      scheduled_compiletf.push_back(obj);
}

/*
 * When parsing is otherwise complete, this function is called to do
 * the final stuff. Clean up deferred linking here.
 */

শূন্য compile_cleanup(শূন্য)
{
      পূর্ণসংখ্যা lnerrs = -1;
      পূর্ণসংখ্যা nerrs = 0;
      পূর্ণসংখ্যা last;

      যদি (verbose_flag) {
	    fprintf(stderr, " ... Linking\n");
	    fflush(stderr);
      }

      করো {
	    resolv_list_s *res = resolv_list;
	    resolv_list = 0x0;
	    last = nerrs == lnerrs;
	    lnerrs = nerrs;
	    nerrs = 0;
	    যখন (res) {
		  resolv_list_s *cur = res;
		  res = res->পরবর্তী_2;
		  যদি (cur->resolve(last))
			delete cur;
		  অন্যথা {
			nerrs++;
			cur->পরবর্তী_2 = resolv_list;
			resolv_list = cur;
		  }
	    }
	    যদি (nerrs && last)
		  fprintf(stderr,
			  "compile_cleanup: %d unresolved items\n",
			  nerrs);
      } যখন (nerrs && !last);

      compile_errors += nerrs;

      যদি (verbose_flag) {
	    fprintf(stderr, " ... Removing symbol tables\n");
	    fflush(stderr);
      }

	/* After compile is complete, the vpi symbol table is no
	   longer needed. VPI objects are located by following
	   scopes. */
      delete_প্রতীক_table(sym_vpi);
      sym_vpi = 0;

	/* Don't need the code labels. The instructions have numeric
	   pointers in them, the symbol table is no longer needed. */
      delete_প্রতীক_table(sym_codespace);
      sym_codespace = 0;

      delete_প্রতীক_table(sym_functors);
      sym_functors = 0;

      delete_udp_symbols();

      compile_island_cleanup();
      compile_array_cleanup();

      যদি (verbose_flag) {
	    fprintf(stderr, " ... Compiletf functions\n");
	    fflush(stderr);
      }

      প্রতিজ্ঞা(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_COMPILETF;

      যখন (! scheduled_compiletf.empty()) {
	    কাঠামো __vpiSysTaskCall*obj = scheduled_compiletf.front();
	    scheduled_compiletf.pop_front();
	    vpip_cur_task = obj;
	    obj->defn->info.compiletf (obj->defn->info.user_তথ্য);
	    vpip_cur_task = 0;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

শূন্য compile_vpi_প্রতীক(ধ্রুবক অক্ষর*label, vpiHandle obj)
{
      প্রতীক_মান_t val;
      val.ptr = obj;
      sym_set_মান(sym_vpi, label, val);
}

/*
 * Initialize the compiler by allocation empty symbol tables and
 * initializing the various address spaces.
 */
শূন্য compile_init(শূন্য)
{
      sym_vpi = new_প্রতীক_table();

      sym_functors = new_প্রতীক_table();

      sym_codespace = new_প্রতীক_table();
      codespace_init();
}

শূন্য compile_load_vpi_module(অক্ষর*name)
{
      vpip_load_module(name);
      delete[] name;
}

শূন্য compile_vpi_time_precision(long pre)
{
      vpip_set_time_precision(pre);
}

/*
 * Convert a Cr string value to double.
 *
 * The format is broken into mantissa and exponent.
 * The exponent in turn includes a sign bit.
 *
 * The mantissa is a 64bit integer value (encoded in hex).
 *
 * The exponent included the sign bit (0x4000) and the binary
 * exponent offset by 0x1000. The actual exponent is the
 * encoded exponent - 0x1000.
 *
 * The real value is sign * (mant ** exp).
 */
স্থির যৌক্তিক crstring_header_test(ধ্রুবক অক্ষর*str)
{
      যদি ((str[0] != 'C') && (str[0] != 'c'))
	    ফেরত মিথ্যা;
      যদি ((str[1] != 'r') || (str[2] != '<'))
	    ফেরত মিথ্যা;

      ফেরত সত্য;
}

যৌক্তিক crstring_test(ধ্রুবক অক্ষর*str)
{
      যদি (!crstring_header_test(str))
	    ফেরত মিথ্যা;
      ধ্রুবক অক্ষর*tp = strchr(str, '>');
      যদি (tp == 0) ফেরত মিথ্যা;
      যদি (tp[1] != 0) ফেরত মিথ্যা;

      যদি ((strspn(str+3, "0123456789abcdefmg")+3) != (size_t)(tp - str))
	  ফেরত মিথ্যা;

      ফেরত সত্য;
}

দ্বিগুণ crstring_to_double(ধ্রুবক অক্ষর*label)
{
      প্রতিজ্ঞা(crstring_header_test(label));

      ধ্রুবক অক্ষর*cp = label+3;
      প্রতিজ্ঞা(*cp == 'm');
      cp += 1;
      অক্ষর*ep;
      uint64_t mant = strtoull(cp, &ep, 16);
      cp = ep;
      প্রতিজ্ঞা(*cp == 'g');
      cp += 1;
      পূর্ণসংখ্যা exp = strtoul(cp, 0, 16);

      দ্বিগুণ tmp;
      যদি (mant == 0 && exp == 0x3fff) {
	    tmp = INFINITY;
      } অন্যথা যদি (mant == 0 && exp == 0x7fff) {
	    tmp = -INFINITY;
      } অন্যথা যদি (exp == 0x3fff) {
	    tmp = nan("");
      } অন্যথা {
	    দ্বিগুণ sign = (exp & 0x4000)? -1.0 : 1.0;
	    exp &= 0x1fff;

	    tmp = sign * ldexp((দ্বিগুণ)mant, exp - 0x1000);
      }

      ফেরত tmp;
}

/*
 * Run through the arguments looking for the nodes that are
 * connected to my input ports. For each source functor that I
 * find, connect the output of that functor to the indexed
 * input by inserting myself (complete with the port number in
 * the vvp_ipoint_t) into the list that the source heads.
 *
 * If the source functor is not declared yet, then don't do
 * the link yet. Save the reference to be resolved later.
 *
 * If the source is a constant value, then set the ival of the functor
 * and skip the symbol lookup.
 */

শূন্য প্রবেশ_connect(vvp_net_t*fdx, unsigned port, অক্ষর*label)
{
      vvp_net_ptr_t ifdx = vvp_net_ptr_t(fdx, port);

	/* Is this a vvp_vector4_t constant value? */
      যদি (c4string_test(label)) {

	    vvp_vector4_t tmp = c4string_to_vector4(label);

	      // Inputs that are constants are scheduled to execute as
	      // soon at the simulation starts. In Verilog, constants
	      // start propagating when the simulation starts, just
	      // like any other signal value. But letting the
	      // scheduler distribute the constant value has the
	      // additional advantage that the constant is not
	      // propagated until the network is fully linked.
	      // For constants that initialise an undriven net, we
	      // schedule execution before time 0, to make sure it
	      // occurs before any sensitive processes are started
	      // or VPI callbacks are executed.
	    যদি (label[0] == 'c')
		  schedule_init_vector(ifdx, tmp);
	    অন্যথা
		  schedule_set_vector(ifdx, tmp);

	    মুক্ত(label);
	    ফেরত;
      }

	/* Is this a vvp_vector8_t constant value? */
      যদি (c8string_test(label)) {

	    vvp_vector8_t tmp = c8string_to_vector8(label);
	    যদি (label[0] == 'c')
		  schedule_init_vector(ifdx, tmp);
	    অন্যথা
		  schedule_set_vector(ifdx, tmp);

	    মুক্ত(label);
	    ফেরত;
      }

	/* Handle the Cr<> constant driver, which is a real-value
	   driver. */
      যদি (crstring_test(label)) {

	    দ্বিগুণ tmp = crstring_to_double(label);

	    যদি (label[0] == 'c')
		  schedule_init_vector(ifdx, tmp);
	    অন্যথা
		  schedule_set_vector(ifdx, tmp);
	    মুক্ত(label);
	    ফেরত;
      }

	/* Handle the general case that this is a label for a node in
	   the vvp net. This arranges for the label to be preserved in
	   a linker list, and linked when the symbol table is
	   complete. */
      postpone_functor_প্রবেশ(ifdx, label);
}

শূন্য inputs_connect(vvp_net_t*fdx, unsigned argc, কাঠামো symb_s*argv)
{
      যদি (argc > 4) {
	    std::cerr << "XXXX argv[0] = " << argv[0].text << std::endl;
      }
      প্রতিজ্ঞা(argc <= 4);

      জন্য (unsigned idx = 0;  idx < argc;  idx += 1) {

	    প্রবেশ_connect(fdx, idx, argv[idx].text);
      }
}

শূন্য wide_inputs_connect(vvp_wide_fun_core*core,
			 unsigned argc, কাঠামো symb_s*argv)
{
	/* Create input functors to receive values from the
	   network. These functors pass the data to the core. */
      unsigned প্রবেশ_functors = (argc+3) / 4;
      জন্য (unsigned idx = 0 ;  idx < প্রবেশ_functors ;  idx += 1) {
	    unsigned base = idx*4;
	    unsigned trans = 4;
	    যদি (base+trans > argc)
		  trans = argc - base;

	    vvp_wide_fun_t*cur = নতুন vvp_wide_fun_t(core, base);
	    vvp_net_t*ptr = নতুন vvp_net_t;
	    ptr->fun = cur;

	    inputs_connect(ptr, trans, argv+base);
      }
}

template <শ্রেণী T_> শূন্য make_arith(T_ *arith, অক্ষর*label,
				    unsigned argc, কাঠামো symb_s*argv)
{
      vvp_net_t* ptr = নতুন vvp_net_t;
      ptr->fun = arith;

      define_functor_প্রতীক(label, ptr);
      মুক্ত(label);

      প্রতিজ্ঞা(argc == 2);
      inputs_connect(ptr, argc, argv);

      মুক্ত(argv);
}

শূন্য compile_arith_cast_int(অক্ষর*label, long width,
                            unsigned argc, কাঠামো symb_s*argv)
{
      vvp_arith_cast_int*arith = নতুন vvp_arith_cast_int((unsigned) width);

      vvp_net_t* ptr = নতুন vvp_net_t;
      ptr->fun = arith;

      define_functor_প্রতীক(label, ptr);
      মুক্ত(label);

      প্রতিজ্ঞা(argc == 1);
      inputs_connect(ptr, argc, argv);
      মুক্ত(argv);
}

শূন্য compile_arith_cast_vec2(অক্ষর*label, long width,
			     unsigned argc, কাঠামো symb_s*argv)
{
      vvp_arith_cast_vec2*arith = নতুন vvp_arith_cast_vec2((unsigned) width);

      vvp_net_t* ptr = নতুন vvp_net_t;
      ptr->fun = arith;

      define_functor_প্রতীক(label, ptr);
      মুক্ত(label);

      প্রতিজ্ঞা(argc == 1);
      inputs_connect(ptr, argc, argv);
      মুক্ত(argv);
}

শূন্য compile_arith_cast_real(অক্ষর*label, যৌক্তিক signed_flag,
                             unsigned argc, কাঠামো symb_s*argv)
{
      vvp_arith_cast_real*arith = নতুন vvp_arith_cast_real(signed_flag);

      vvp_net_t* ptr = নতুন vvp_net_t;
      ptr->fun = arith;

      define_functor_প্রতীক(label, ptr);
      মুক্ত(label);

      প্রতিজ্ঞা(argc == 1);
      inputs_connect(ptr, argc, argv);
      মুক্ত(argv);
}

শূন্য compile_arith_abs(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv)
{
      vvp_arith_abs*arith = নতুন vvp_arith_abs;

      vvp_net_t* ptr = নতুন vvp_net_t;
      ptr->fun = arith;

      define_functor_প্রতীক(label, ptr);
      মুক্ত(label);

      প্রতিজ্ঞা(argc == 1);
      inputs_connect(ptr, argc, argv);
      মুক্ত(argv);
}

শূন্য compile_arith_div(অক্ষর*label, long wid, যৌক্তিক signed_flag,
		       unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    ধ্রুবক অক্ষর *suffix = "";
	    যদি (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s; .arith/div%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_arith_div(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

শূন্য compile_arith_div_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv)
{
      যদি (argc != 2) {
	    fprintf(stderr, "%s; .arith/divr has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_real_ *arith = নতুন vvp_arith_div_real;
      make_arith(arith, label, argc, argv);
}

শূন্য compile_arith_mod(অক্ষর*label, long wid, যৌক্তিক signed_flag,
		       unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    fprintf(stderr, "%s .arith/mod has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_arith_mod(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

শূন্য compile_arith_mod_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv)
{
      যদি (argc != 2) {
	    fprintf(stderr, "%s .arith/mod.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_real_ *arith = নতুন vvp_arith_mod_real;
      make_arith(arith, label, argc, argv);
}

শূন্য compile_arith_mult(অক্ষর*label, long wid,
			unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    fprintf(stderr, "%s .arith/mult has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_arith_mult(wid);
      make_arith(arith, label, argc, argv);
}

শূন্য compile_arith_mult_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv)
{
      যদি (argc != 2) {
	    fprintf(stderr, "%s .arith/mult.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_real_ *arith = নতুন vvp_arith_mult_real;
      make_arith(arith, label, argc, argv);
}


শূন্য compile_arith_pow(অক্ষর*label, long wid, যৌক্তিক signed_flag,
		       unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    ধ্রুবক অক্ষর *suffix = "";
	    যদি (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s .arith/pow%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_arith_pow(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

শূন্য compile_arith_pow_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv)
{
      যদি (argc != 2) {
	    fprintf(stderr, "%s .arith/pow.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_real_ *arith = নতুন vvp_arith_pow_real;
      make_arith(arith, label, argc, argv);
}

শূন্য compile_arith_sub(অক্ষর*label, long wid, unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    fprintf(stderr, "%s .arith/sub has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_arith_sub(wid);
      make_arith(arith, label, argc, argv);
}

শূন্য compile_arith_sub_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv)
{
      যদি (argc != 2) {
	    fprintf(stderr, "%s; .arith/sub.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_real_ *arith = নতুন vvp_arith_sub_real;
      make_arith(arith, label, argc, argv);
}

শূন্য compile_arith_যোগ(অক্ষর*label, long wid, unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    fprintf(stderr, "%s .arith/sum has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_arith_যোগ(wid);
      make_arith(arith, label, argc, argv);
}

শূন্য compile_arith_যোগ_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv)
{
      যদি (argc != 2) {
	    fprintf(stderr, "%s .arith/sum.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_real_ *arith = নতুন vvp_arith_যোগ_real;
      make_arith(arith, label, argc, argv);
}

শূন্য compile_cmp_eeq(অক্ষর*label, long wid,
		     unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_cmp_eeq(wid);

      make_arith(arith, label, argc, argv);
}

শূন্য compile_cmp_nee(অক্ষর*label, long wid,
		     unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_cmp_nee(wid);

      make_arith(arith, label, argc, argv);
}

শূন্য compile_cmp_eq(অক্ষর*label, long wid, unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_cmp_eq(wid);
      make_arith(arith, label, argc, argv);
}

শূন্য compile_cmp_eqx(অক্ষর*label, long wid, unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqx has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_cmp_eqx(wid);
      make_arith(arith, label, argc, argv);
}

শূন্য compile_cmp_eqz(অক্ষর*label, long wid, unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqz has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_cmp_eqz(wid);
      make_arith(arith, label, argc, argv);
}

শূন্য compile_cmp_eq_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv)
{
      যদি (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_real_ *arith = নতুন vvp_cmp_eq_real;
      make_arith(arith, label, argc, argv);
}

শূন্য compile_cmp_ne(অক্ষর*label, long wid, unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_cmp_ne(wid);
      make_arith(arith, label, argc, argv);
}

শূন্য compile_cmp_ne_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv)
{
      যদি (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_real_ *arith = নতুন vvp_cmp_ne_real;
      make_arith(arith, label, argc, argv);
}

শূন্য compile_cmp_ge(অক্ষর*label, long wid, যৌক্তিক signed_flag,
		    unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_cmp_ge(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

শূন্য compile_cmp_ge_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv)
{
      যদি (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_real_ *arith = নতুন vvp_cmp_ge_real;
      make_arith(arith, label, argc, argv);
}

শূন্য compile_cmp_gt(অক্ষর*label, long wid, যৌক্তিক signed_flag,
		    unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_cmp_gt(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

শূন্য compile_cmp_gt_r(অক্ষর*label, unsigned argc, কাঠামো symb_s*argv)
{
      যদি (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_real_ *arith = নতুন vvp_cmp_gt_real;
      make_arith(arith, label, argc, argv);
}

শূন্য compile_cmp_weq(অক্ষর*label, long wid,
		     unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    fprintf(stderr, "%s .cmp/weq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_cmp_weq(wid);

      make_arith(arith, label, argc, argv);
}

শূন্য compile_cmp_wne(অক্ষর*label, long wid,
		     unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      যদি (argc != 2) {
	    fprintf(stderr, "%s .cmp/wne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ফেরত;
      }

      vvp_arith_ *arith = নতুন vvp_cmp_wne(wid);

      make_arith(arith, label, argc, argv);
}


শূন্য compile_delay(অক্ষর*label, unsigned width,
                   vvp_delay_t*delay, কাঠামো symb_s arg)
{
      vvp_net_t*net = নতুন vvp_net_t;
      vvp_fun_delay*obj = নতুন vvp_fun_delay(net, width, *delay);
      net->fun = obj;

      delete delay;

      প্রবেশ_connect(net, 0, arg.text);

      define_functor_প্রতীক(label, net);
      মুক্ত(label);
}

শূন্য compile_delay(অক্ষর*label, unsigned width,
                   unsigned argc, কাঠামো symb_s*argv, যৌক্তিক ignore_decay)
{
      vvp_delay_t stub (0, 0, 0);
      যদি (ignore_decay) stub.set_ignore_decay();
      vvp_net_t*net = নতুন vvp_net_t;
      vvp_fun_delay*obj = নতুন vvp_fun_delay(net, width, stub);
      net->fun = obj;

      inputs_connect(net, argc, argv);
      মুক্ত(argv);

      define_functor_প্রতীক(label, net);
      মুক্ত(label);
}

/*
 * Extend nodes.
 */
শূন্য compile_extend_signed(অক্ষর*label, long wid, কাঠামো symb_s arg)
{
      প্রতিজ্ঞা(wid >= 0);

      vvp_fun_extend_signed*fun = নতুন vvp_fun_extend_signed(wid);
      vvp_net_t*ptr = নতুন vvp_net_t;
      ptr->fun = fun;

      define_functor_প্রতীক(label, ptr);
      মুক্ত(label);

      প্রবেশ_connect(ptr, 0, arg.text);
}

কাঠামো __vpiModPath* compile_modpath(অক্ষর*label, unsigned width,
                                     কাঠামো symb_s drv, কাঠামো symb_s dest)
{
      vvp_net_t*net = নতুন vvp_net_t;
      vvp_fun_modpath*obj = নতুন vvp_fun_modpath(net, width);
      net->fun = obj;

      প্রবেশ_connect(net, 0, drv.text);

      define_functor_প্রতীক(label, net);

      __vpiModPath*modpath = vpip_make_modpath(net);

      compile_vpi_lookup(&modpath->path_term_out.রাশিমালা, dest.text);

      মুক্ত(label);

      modpath->modpath = obj;
      ফেরত modpath;
}

স্থির কাঠামো __vpiModPathSrc*make_modpath_src(কাঠামো __vpiModPath*path,
					       অক্ষর edge_c,
					       ধ্রুবক কাঠামো symb_s&src,
					       কাঠামো numbv_s&vals,
					       যৌক্তিক ifnone)
{
      vvp_fun_modpath*dst = path->modpath;

      vvp_time64_t use_delay[12];
      প্রতিজ্ঞা(vals.cnt == 12);
      জন্য (unsigned idx = 0 ; idx < vals.cnt ;  idx += 1) {
	    use_delay[idx] = vals.nvec[idx];
      }

      numbv_clear(&vals);

      vvp_fun_modpath_src*obj = 0;

      পূর্ণসংখ্যা vpi_edge = vpiNoEdge;
      যদি (edge_c == 0) {
	    obj = নতুন vvp_fun_modpath_src(use_delay);

      } অন্যথা {
	    যৌক্তিক posedge, negedge;
	    নির্বাচন (edge_c) {
		ক্ষেত্র '+':
		  vpi_edge = vpiPosedge;
		  posedge = সত্য;
		  negedge = মিথ্যা;
		  বিরতি;
		ক্ষেত্র '-':
		  vpi_edge = vpiNegedge;
		  posedge = মিথ্যা;
		  negedge = সত্য;
		  বিরতি;
#যদি 0
		ক্ষেত্র '*':
		  posedge = সত্য;
		  negedge = সত্য;
		  বিরতি;
#endif
		পূর্বনির্ধারিত:
		  posedge = মিথ্যা;
		  negedge = মিথ্যা;
		  fprintf(stderr, "Unknown edge identifier %c(%d).\n", edge_c,
		          edge_c);
		  প্রতিজ্ঞা(0);
	    }
	    obj = নতুন vvp_fun_modpath_edge(use_delay, posedge, negedge);
      }

      vvp_net_t*net = নতুন vvp_net_t;
      কাঠামো __vpiModPathSrc* srcobj = vpip_make_modpath_src(path, net) ;
      vpip_attach_to_current_scope(srcobj);
      net->fun = obj;

	/* Save the vpiEdge directory into the input path term. */
      srcobj->path_term_in.edge = vpi_edge;
      প্রবেশ_connect(net, 0, src.text);
      dst->add_modpath_src(obj, ifnone);

      ফেরত srcobj;
}

শূন্য compile_modpath_src(কাঠামো __vpiModPath*dst, অক্ষর edge_c,
			 ধ্রুবক কাঠামো symb_s&src,
			 কাঠামো numbv_s&vals,
			 ধ্রুবক কাঠামো symb_s&condit_src,
			 ধ্রুবক কাঠামো symb_s&path_term_in)
{
      কাঠামো __vpiModPathSrc*obj =
	    make_modpath_src(dst, edge_c, src, vals, মিথ্যা);
      প্রবেশ_connect(obj->net, 1, condit_src.text);
      compile_vpi_lookup(&obj->path_term_in.রাশিমালা, path_term_in.text);
}

শূন্য compile_modpath_src(কাঠামো __vpiModPath*dst, অক্ষর edge_c,
			 ধ্রুবক কাঠামো symb_s&src,
			 কাঠামো numbv_s&vals,
			 পূর্ণসংখ্যা condit_src,
			 ধ্রুবক কাঠামো symb_s&path_term_in,
			 যৌক্তিক ifnone)
{
      প্রতিজ্ঞা(condit_src == 0);
      কাঠামো __vpiModPathSrc*obj =
	    make_modpath_src(dst, edge_c, src, vals, ifnone);
      compile_vpi_lookup(&obj->path_term_in.রাশিমালা, path_term_in.text);
}

/*
 * A .shift/l statement creates an array of functors for the
 * width. The 0 input is the data vector to be shifted and the 1 input
 * is the amount of the shift. An unconnected shift amount is set to 0.
 */
শূন্য compile_shiftl(অক্ষর*label, long wid, unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      vvp_arith_ *arith = নতুন vvp_shiftl(wid);
      make_arith(arith, label, argc, argv);
}

শূন্য compile_shiftr(অক্ষর*label, long wid, যৌক্তিক signed_flag,
		    unsigned argc, কাঠামো symb_s*argv)
{
      প্রতিজ্ঞা( wid > 0 );

      vvp_arith_ *arith = নতুন vvp_shiftr(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

শূন্য compile_resolver(অক্ষর*label, অক্ষর*type, unsigned argc, কাঠামো symb_s*argv)
{
      vvp_net_t*net = নতুন vvp_net_t;

      resolv_core*core = 0;

      যদি (strcmp(type,"tri") == 0) {
	    core = নতুন resolv_tri(argc, net, vvp_scalar_t(BIT4_Z, 0,0));

      } অন্যথা যদি (strcmp(type,"tri0") == 0) {
	    core = নতুন resolv_tri(argc, net, vvp_scalar_t(BIT4_0, 5,5));

      } অন্যথা যদি (strcmp(type,"tri1") == 0) {
	    core = নতুন resolv_tri(argc, net, vvp_scalar_t(BIT4_1, 5,5));

      } অন্যথা যদি (strcmp(type,"triand") == 0) {
	    core = নতুন resolv_triand(argc, net);

      } অন্যথা যদি (strcmp(type,"trior") == 0) {
	    core = নতুন resolv_trior(argc, net);

      } অন্যথা {
	    fprintf(stderr, "invalid resolver type: %s\n", type);
	    compile_errors += 1;
            delete net;
      }

      যদি (core) {
	    net->fun = core;
	    define_functor_প্রতীক(label, net);

            জন্য (unsigned base = 0 ;  base < argc ;  base += 4) {
	          unsigned nports = argc - base;
                  যদি (nports > 4)
                        nports = 4;

                  যদি (base > 0) {
                        net = নতুন vvp_net_t;
                        net->fun = নতুন resolv_extend(core, base);
                  }
                  inputs_connect(net, nports, argv+base);
            }
      }
      মুক্ত(type);
      মুক্ত(label);
      মুক্ত(argv);
}

শূন্য compile_udp_def(পূর্ণসংখ্যা sequ, অক্ষর *label, অক্ষর *name,
		     unsigned nin, unsigned init, অক্ষর **table)
{
      যদি (sequ) {
	    vvp_bit4_t init4;
	    যদি (init == 0)
		  init4 = BIT4_0;
	    অন্যথা যদি (init == 1)
		  init4 = BIT4_1;
	    অন্যথা
		  init4 = BIT4_X;

	    vvp_udp_seq_s *u = নতুন vvp_udp_seq_s(label, name, nin, init4);
	    u->compile_table(table);
      } অন্যথা {
	    vvp_udp_comb_s *u = নতুন vvp_udp_comb_s(label, name, nin);
	    u->compile_table(table);
      }
      মুক্ত(label);
}

অক্ষর **compile_udp_table(অক্ষর **table, অক্ষর *row)
{
  যদি (table)
    প্রতিজ্ঞা(strlen(*table)==strlen(row));

  অক্ষর **tt;
  জন্য (tt = table; tt && *tt; tt++) { }
  পূর্ণসংখ্যা n = (tt-table) + 2;

  table = static_cast<অক্ষর**>(realloc(table, n*আকার(অক্ষর*)));
  table[n-2] = row;
  table[n-1] = 0x0;

  ফেরত table;
}


/*
 * The parser uses this function to compile and link an executable
 * opcode. I do this by looking up the opcode in the opcode_table. The
 * table gives the operand structure that is acceptable, so I can
 * process the operands here as well.
 */
শূন্য compile_code(অক্ষর*label, অক্ষর*mnem, comp_operands_t opa)
{
	/* First, I can give the label a value that is the current
	   codespace pointer. Don't need the text of the label after
	   this is done. */
      যদি (label)
	    compile_codelabel(label);

	/* Lookup the opcode in the opcode table. */
      কাঠামো opcode_table_s*op = static_cast<কাঠামো opcode_table_s*>
                                            (bsearch(mnem, opcode_table, opcode_গুনতি,
                                                     আকার(কাঠামো opcode_table_s),
                                                     &opcode_compare));
      যদি (op == 0) {
	    yyerror("Invalid opcode");
	    compile_errors += 1;
	    ফেরত;
      }

      প্রতিজ্ঞা(op);

	/* Build up the code from the information about the opcode and
	   the information from the compiler. */
      vvp_code_t code = codespace_allocate();
      code->opcode = op->opcode;

      যদি (op->argc != (opa? opa->argc : 0)) {
	    yyerror("operand count");
	    compile_errors += 1;
	    ফেরত;
      }

	/* Pull the operands that the instruction expects from the
	   list that the parser supplied. */

      জন্য (unsigned idx = 0 ;  idx < op->argc ;  idx += 1) {

	    নির্বাচন (op->argt[idx]) {
		ক্ষেত্র OA_NONE:
		  বিরতি;

		ক্ষেত্র OA_ARR_PTR:
		  যদি (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			বিরতি;
		  }

		  compile_array_lookup(code, opa->argv[idx].symb.text);
		  বিরতি;

		ক্ষেত্র OA_BIT1:
		  যদি (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			বিরতি;
		  }
		  code->bit_idx[0] = opa->argv[idx].numb;
		  বিরতি;

		ক্ষেত্র OA_BIT2:
		  যদি (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			বিরতি;
		  }
		  code->bit_idx[1] = opa->argv[idx].numb;
		  বিরতি;

		ক্ষেত্র OA_CODE_PTR:
		ক্ষেত্র OA_CODE_PTR2:
		  যদি (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			বিরতি;
		  }

		  প্রতিজ্ঞা(opa->argv[idx].symb.idx == 0);
		  code_label_lookup(code, opa->argv[idx].symb.text,
				    op->argt[idx]==OA_CODE_PTR2);
		  বিরতি;

		ক্ষেত্র OA_FUNC_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  যদি (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			বিরতি;
		  }

		  functor_ref_lookup(&code->net, opa->argv[idx].symb.text);
		  বিরতি;

		ক্ষেত্র OA_FUNC_PTR2:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  যদি (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			বিরতি;
		  }

		  functor_ref_lookup(&code->net2, opa->argv[idx].symb.text);
		  বিরতি;

		ক্ষেত্র OA_সংখ্যা:
		  যদি (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			বিরতি;
		  }

		  code->সংখ্যা = opa->argv[idx].numb;
		  বিরতি;

		ক্ষেত্র OA_VPI_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  যদি (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			বিরতি;
		  }

		  compile_vpi_lookup(&code->handle, opa->argv[idx].symb.text);
		  বিরতি;

		ক্ষেত্র OA_STRING:
		  যদি (opa->argv[idx].ltype != L_STRING) {
			yyerror("operand format");
			বিরতি;
		  }

		  code->text = opa->argv[idx].text;
		  বিরতি;
	    }
      }

      মুক্ত(opa);

      মুক্ত(mnem);
}

শূন্য compile_codelabel(অক্ষর*label)
{
      প্রতীক_মান_t val;
      vvp_code_t ptr = codespace_পরবর্তী();

      val.ptr = ptr;
      sym_set_মান(sym_codespace, label, val);

      মুক্ত(label);
}

শূন্য compile_file_সারি(অক্ষর*label, long file_idx, long lineno,
                       অক্ষর*description)
{
      যদি (label) compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_code_t code = codespace_allocate();
      code->opcode = &of_FILE_সারি;

	/* Create a vpiHandle that contains the information. */
      code->handle = vpip_build_file_সারি(description, file_idx, lineno);
      প্রতিজ্ঞা(code->handle);

	/* Done with the lexor-allocated name string. */
      delete[] description;
}

শূন্য compile_vpi_call(অক্ষর*label, অক্ষর*name,
                      যৌক্তিক func_as_task_err, যৌক্তিক func_as_task_warn,
                      long file_idx, long lineno,
                      unsigned argc, vpiHandle*argv,
		      unsigned vec4_স্তূপ, unsigned real_স্তূপ, unsigned string_স্তূপ)
{
      যদি (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_code_t code = codespace_allocate();
      code->opcode = &of_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, 0, 0,
					 0, func_as_task_err, func_as_task_warn,
                                         argc, argv, vec4_স্তূপ, real_স্তূপ, string_স্তূপ,
					 file_idx, lineno);
      যদি (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      delete[] name;
}

শূন্য compile_vpi_func_call(অক্ষর*label, অক্ষর*name,
			   পূর্ণসংখ্যা val_type, unsigned val_wid,
			   long file_idx, long lineno,
			   unsigned argc, vpiHandle*argv,
			   unsigned vec4_স্তূপ,
			   unsigned real_স্তূপ,
			   unsigned string_স্তূপ)
{
      যদি (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_code_t code = codespace_allocate();
      code->opcode = &of_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, val_type, val_wid,
					 0, সত্য, মিথ্যা,
                                         argc, argv, vec4_স্তূপ, real_স্তূপ, string_স্তূপ,
					 file_idx, lineno);
      যদি (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      delete[] name;
}

/*
 * When the parser finds a thread statement, I create a new thread
 * with the start address referenced by the program symbol passed to
 * me.
 */
শূন্য compile_thread(অক্ষর*start_sym, অক্ষর*flag)
{
      যৌক্তিক ঠেলো_flag = মিথ্যা;

      প্রতীক_মান_t tmp = sym_get_মান(sym_codespace, start_sym);
      vvp_code_t pc = reinterpret_cast<vvp_code_t>(tmp.ptr);
      যদি (pc == 0) {
	    yyerror("unresolved address");
	    ফেরত;
      }

      যদি (flag && (strcmp(flag,"$push") == 0))
	    ঠেলো_flag = সত্য;

      vthread_t thr = vthread_new(pc, vpip_peek_current_scope());

      যদি (flag && (strcmp(flag,"$init") == 0))
	    schedule_init_vthread(thr);
      অন্যথা যদি (flag && (strcmp(flag,"$final") == 0))
	    schedule_final_vthread(thr);
      অন্যথা
	    schedule_vthread(thr, 0, ঠেলো_flag);

      মুক্ত(start_sym);
      মুক্ত(flag);
}

শূন্য compile_param_logic(অক্ষর*label, ধ্রুবক অক্ষর*name, অক্ষর*value, যৌক্তিক signed_flag,
                         যৌক্তিক local_flag,
                         long file_idx, long lineno)
{
      vvp_vector4_t value4 = c4string_to_vector4(value);
      vpiHandle obj = vpip_make_দ্বিমিক_param(name, value4, signed_flag,
                                             local_flag, file_idx, lineno);
      compile_vpi_প্রতীক(label, obj);
      vpip_attach_to_current_scope(obj);

      মুক্ত(label);
      মুক্ত(value);
}

শূন্য compile_param_string(অক্ষর*label, ধ্রুবক অক্ষর*name, অক্ষর*value,
                          যৌক্তিক local_flag,
                          long file_idx, long lineno)
{
	// name and value become owned by vpip_make_string_param
      vpiHandle obj = vpip_make_string_param(name, value, local_flag, file_idx, lineno);
      compile_vpi_প্রতীক(label, obj);
      vpip_attach_to_current_scope(obj);

      মুক্ত(label);
}

শূন্য compile_param_real(অক্ষর*label, ধ্রুবক অক্ষর*name, অক্ষর*value,
                        যৌক্তিক local_flag,
                        long file_idx, long lineno)
{
      দ্বিগুণ dvalue = crstring_to_double(value);
      vpiHandle obj = vpip_make_real_param(name, dvalue, local_flag, file_idx, lineno);
      compile_vpi_প্রতীক(label, obj);
      vpip_attach_to_current_scope(obj);

      মুক্ত(label);
      মুক্ত(value);
}

শূন্য compile_island(অক্ষর*label, অক্ষর*type)
{
      যদি (strcmp(type,"tran") == 0)
	    compile_island_tran(label);
      অন্যথা
	    প্রতিজ্ঞা(0);

      মুক্ত(type);
}
