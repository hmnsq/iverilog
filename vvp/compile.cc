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
#അന്തര്ഭാവ <windows.h>
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

എണ്ണൽ operand_e {
	/* Place holder for unused operand */
      OA_NONE,
	/* The operand is a number, an immediate unsigned integer */
      OA_സംഖ്യ,
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

ഘടന opcode_table_s {
      സ്ഥിരം അക്ഷരം*mnemonic;
      vvp_code_fun opcode;

      unsigned argc;
      എണ്ണൽ operand_e argt[OPERAND_MAX];
};

സ്ഥിതം സ്ഥിരം ഘടന opcode_table_s opcode_table[] = {
      { "%abs/wr", of_ABS_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add",    of_ADD,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add/wr", of_ADD_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%addi",   of_ADDI,   3,  {OA_BIT1,     OA_BIT2,     OA_സംഖ്യ} },
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
      { "%cast/vec2/dar", of_CAST_VEC2_DAR, 1,  {OA_സംഖ്യ,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/dar", of_CAST_VEC4_DAR, 1,  {OA_സംഖ്യ,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/str", of_CAST_VEC4_STR, 1,  {OA_സംഖ്യ,   OA_NONE,     OA_NONE} },
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
      { "%cmpi/e",  of_CMPIE,  3,  {OA_BIT1,     OA_BIT2,     OA_സംഖ്യ} },
      { "%cmpi/ne", of_CMPINE, 3,  {OA_BIT1,     OA_BIT2,     OA_സംഖ്യ} },
      { "%cmpi/s",  of_CMPIS,  3,  {OA_BIT1,     OA_BIT2,     OA_സംഖ്യ} },
      { "%cmpi/u",  of_CMPIU,  3,  {OA_BIT1,     OA_BIT2,     OA_സംഖ്യ} },
      { "%cmpix/ltu", of_CMPIX_LTU, 2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%cmpix/slt0", of_CMPIX_SLT0, 1, {OA_സംഖ്യ, OA_NONE, OA_NONE} },
      { "%concat/str",  of_CONCAT_STR,  0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concat/vec4", of_CONCAT_VEC4, 0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concati/str", of_CONCATI_STR, 1,{OA_STRING,OA_NONE,  OA_NONE} },
      { "%concati/vec4",of_CONCATI_VEC4,3,{OA_BIT1,  OA_BIT2,  OA_സംഖ്യ} },
      { "%cvt/rv",   of_CVT_RV,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/rv/s", of_CVT_RV_S,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/sr", of_CVT_SR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/ur", of_CVT_UR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/vr", of_CVT_VR, 1,  {OA_സംഖ്യ,   OA_NONE,     OA_NONE} },
      { "%deassign",of_DEASSIGN,3,{OA_FUNC_PTR, OA_BIT1,     OA_BIT2} },
      { "%deassign/wr",of_DEASSIGN_WR,1,{OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%debug/thr",  of_DEBUG_THR,  1,{OA_STRING,   OA_NONE,     OA_NONE} },
      { "%delay",  of_DELAY,  2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%delayx", of_DELAYX, 1,  {OA_സംഖ്യ,   OA_NONE,     OA_NONE} },
      { "%delete/elem",of_DELETE_ELEM,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%delete/obj",of_DELETE_OBJ,1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%delete/prop/elem", of_DELETE_PROP_ELEM, 1, {OA_സംഖ്യ, OA_NONE, OA_NONE} },
      { "%delete/prop/obj",  of_DELETE_PROP_OBJ,  1, {OA_സംഖ്യ, OA_NONE, OA_NONE} },
      { "%delete/tail",of_DELETE_TAIL,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
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
      { "%flag_get/vec4", of_FLAG_GET_VEC4, 1, {OA_സംഖ്യ, OA_NONE, OA_NONE} },
      { "%flag_inv",      of_FLAG_INV,      1, {OA_BIT1,   OA_NONE, OA_NONE} },
      { "%flag_mov",      of_FLAG_MOV,      2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_or",       of_FLAG_OR,       2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_set/imm",  of_FLAG_SET_IMM,  2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%flag_set/vec4", of_FLAG_SET_VEC4, 1, {OA_സംഖ്യ, OA_NONE, OA_NONE} },
      { "%force/link",    of_FORCE_LINK,2,{OA_FUNC_PTR, OA_FUNC_PTR2, OA_NONE} },
      { "%force/vec4",    of_FORCE_VEC4,    1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%force/vec4/off",of_FORCE_VEC4_OFF,2,{OA_FUNC_PTR, OA_BIT1,      OA_NONE} },
      { "%force/vec4/off/d",of_FORCE_VEC4_OFF_D,3,{OA_FUNC_PTR, OA_BIT1,  OA_BIT2} },
      { "%force/wr",      of_FORCE_WR,      1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%fork",   of_FORK,   2,  {OA_CODE_PTR2,OA_VPI_PTR,  OA_NONE} },
      { "%free",   of_FREE,   1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%inv",    of_INV,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%ix/add", of_IX_ADD, 3,  {OA_സംഖ്യ,   OA_BIT1,     OA_BIT2} },
      { "%ix/getv",of_IX_GETV,2,  {OA_BIT1,     OA_FUNC_PTR, OA_NONE} },
      { "%ix/getv/s",of_IX_GETV_S,2, {OA_BIT1,   OA_FUNC_PTR, OA_NONE} },
      { "%ix/load",of_IX_LOAD,3,  {OA_സംഖ്യ,   OA_BIT1,     OA_BIT2} },
      { "%ix/mov", of_IX_MOV, 2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%ix/mul", of_IX_MUL, 3,  {OA_സംഖ്യ,   OA_BIT1,     OA_BIT2} },
      { "%ix/sub", of_IX_SUB, 3,  {OA_സംഖ്യ,   OA_BIT1,     OA_BIT2} },
      { "%ix/vec4",  of_IX_VEC4,  1,  {OA_സംഖ്യ,   OA_NONE, OA_NONE} },
      { "%ix/vec4/s",of_IX_VEC4_S,1,  {OA_സംഖ്യ,   OA_NONE, OA_NONE} },
      { "%jmp",    of_JMP,    1,  {OA_CODE_PTR, OA_NONE,     OA_NONE} },
      { "%jmp/0",  of_JMP0,   2,  {OA_CODE_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/0xz",of_JMP0XZ, 2,  {OA_CODE_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1",  of_JMP1,   2,  {OA_CODE_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1xz",of_JMP1XZ, 2,  {OA_CODE_PTR, OA_BIT1,     OA_NONE} },
      { "%join",   of_JOIN,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%join/detach",of_JOIN_DETACH,1,{OA_സംഖ്യ,OA_NONE,  OA_NONE} },
      { "%load/ar",of_LOAD_AR,2,  {OA_ARR_PTR,  OA_BIT1,     OA_NONE} },
      { "%load/dar/r",  of_LOAD_DAR_R,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE}},
      { "%load/dar/str",of_LOAD_DAR_STR,  1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/dar/vec4",of_LOAD_DAR_VEC4,1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/obj",   of_LOAD_OBJ,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/obja",  of_LOAD_OBJA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/prop/dar/vec4", of_LOAD_PROP_DAR_VEC4, 2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
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
      { "%muli",   of_MULI,   3,  {OA_BIT1,     OA_BIT2,     OA_സംഖ്യ} },
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
      { "%pad/s",  of_PAD_S,  1,  {OA_സംഖ്യ,   OA_NONE,     OA_NONE} },
      { "%pad/u",  of_PAD_U,  1,  {OA_സംഖ്യ,   OA_NONE,     OA_NONE} },
      { "%part/s", of_PART_S, 1,  {OA_സംഖ്യ,   OA_NONE,     OA_NONE} },
      { "%part/u", of_PART_U, 1,  {OA_സംഖ്യ,   OA_NONE,     OA_NONE} },
      { "%parti/s",of_PARTI_S,3,  {OA_സംഖ്യ,   OA_BIT1,     OA_BIT2} },
      { "%parti/u",of_PARTI_U,3,  {OA_സംഖ്യ,   OA_BIT1,     OA_BIT2} },
      { "%pop/obj", of_എടുക്കുക_OBJ, 2, {OA_BIT1,    OA_BIT2,     OA_NONE} },
      { "%pop/real",of_എടുക്കുക_REAL,1, {OA_സംഖ്യ,  OA_NONE,     OA_NONE} },
      { "%pop/str", of_എടുക്കുക_STR, 1, {OA_സംഖ്യ,  OA_NONE,     OA_NONE} },
      { "%pop/vec4",of_എടുക്കുക_VEC4,1, {OA_സംഖ്യ,  OA_NONE,     OA_NONE} },
      { "%pow",     of_POW,     0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/s",   of_POW_S,   0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/wr",  of_POW_WR,  0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%prop/obj",of_PROP_OBJ,2,  {OA_സംഖ്യ,   OA_BIT1,     OA_NONE} },
      { "%prop/queue/size", of_PROP_QUEUE_SIZE,  1, {OA_സംഖ്യ, OA_NONE, OA_NONE} },
      { "%prop/r",  of_PROP_R,  1,  {OA_സംഖ്യ,   OA_NONE,     OA_NONE} },
      { "%prop/str",of_PROP_STR,1,  {OA_സംഖ്യ,   OA_NONE,     OA_NONE} },
      { "%prop/v",  of_PROP_V,  1,  {OA_സംഖ്യ,   OA_NONE,     OA_NONE} },
      { "%push/ix/vec4", of_തള്ളുക_IX_VEC4, 3, {OA_സംഖ്യ, OA_BIT1, OA_BIT2} },
      { "%pushi/real",of_PUSHI_REAL,2,{OA_BIT1,   OA_BIT2,   OA_NONE} },
      { "%pushi/str", of_PUSHI_STR, 1,{OA_STRING, OA_NONE,   OA_NONE} },
      { "%pushi/vec4",of_PUSHI_VEC4,3,{OA_BIT1,   OA_BIT2,   OA_സംഖ്യ} },
      { "%pushv/str", of_PUSHV_STR, 0,{OA_NONE,   OA_NONE,   OA_NONE} },
      { "%putc/str/vec4",of_PUTC_STR_VEC4,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/prop/r",   of_QINSERT_PROP_R,   2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/str", of_QINSERT_PROP_STR, 2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/v",   of_QINSERT_PROP_V,   3, {OA_സംഖ്യ, OA_BIT1, OA_BIT2} },
      { "%qinsert/real",of_QINSERT_REAL,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/str", of_QINSERT_STR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/v",   of_QINSERT_V,   3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%qpop/b/real",of_QPOP_B_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/str", of_QPOP_B_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/v",   of_QPOP_B_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/f/real",of_QPOP_F_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/str", of_QPOP_F_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/v",   of_QPOP_F_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/prop/b/r",   of_QPOP_PROP_B_REAL, 1, {OA_സംഖ്യ, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/str", of_QPOP_PROP_B_STR, 1, {OA_സംഖ്യ, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/v",   of_QPOP_PROP_B_V,    2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%qpop/prop/f/r",   of_QPOP_PROP_F_REAL, 1, {OA_സംഖ്യ, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/str", of_QPOP_PROP_F_STR, 1, {OA_സംഖ്യ, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/v",   of_QPOP_PROP_F_V,    2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%queue/append_word/v", of_QUEUE_APPEND_WORD_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/find/index/prop/v", of_QUEUE_FIND_INDEX_PROP_V, 2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%queue/find/index/v", of_QUEUE_FIND_INDEX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find/prop/v", of_QUEUE_FIND_PROP_V, 2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%queue/find/v", of_QUEUE_FIND_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/prop/v", of_QUEUE_FIND_FIRST_INDEX_PROP_V, 2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/v", of_QUEUE_FIND_FIRST_INDEX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/prop/v", of_QUEUE_FIND_FIRST_PROP_V, 2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%queue/find_first/v", of_QUEUE_FIND_FIRST_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/prop/v", of_QUEUE_FIND_LAST_INDEX_PROP_V, 2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/v", of_QUEUE_FIND_LAST_INDEX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/prop/v", of_QUEUE_FIND_LAST_PROP_V, 2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%queue/find_last/v", of_QUEUE_FIND_LAST_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/max/obj/v", of_QUEUE_MAX_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/max/prop/v", of_QUEUE_MAX_PROP_V, 2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%queue/max/v", of_QUEUE_MAX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/min/obj/v", of_QUEUE_MIN_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/min/prop/v", of_QUEUE_MIN_PROP_V, 2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%queue/min/v", of_QUEUE_MIN_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/new_empty/v", of_QUEUE_NEW_EMPTY_V, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%queue/size/v", of_QUEUE_SIZE_V, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%queue/unique/index/obj/v", of_QUEUE_UNIQUE_INDEX_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/index/prop/v", of_QUEUE_UNIQUE_INDEX_PROP_V, 2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%queue/unique/index/v", of_QUEUE_UNIQUE_INDEX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/unique/obj/v", of_QUEUE_UNIQUE_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/prop/v", of_QUEUE_UNIQUE_PROP_V, 2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%queue/unique/v", of_QUEUE_UNIQUE_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/word/prop/v", of_QUEUE_WORD_PROP_V, 3, {OA_സംഖ്യ, OA_BIT1, OA_BIT2} },
      { "%queue/word/v", of_QUEUE_WORD_V, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%release/net",of_RELEASE_NET,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/reg",of_RELEASE_REG,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/wr", of_RELEASE_WR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%replicate", of_REPLICATE,   1,{OA_സംഖ്യ,  OA_NONE,OA_NONE} },
      { "%ret/real",  of_RET_REAL,    1,{OA_സംഖ്യ,  OA_NONE,OA_NONE} },
      { "%ret/str",   of_RET_STR,     1,{OA_സംഖ്യ,  OA_NONE,OA_NONE} },
      { "%ret/vec4",  of_RET_VEC4,    3,{OA_സംഖ്യ,  OA_BIT1,OA_BIT2} },
      { "%retload/real",of_RETLOAD_REAL,1,{OA_സംഖ്യ,  OA_NONE,OA_NONE} },
      { "%retload/str", of_RETLOAD_STR, 1,{OA_സംഖ്യ,  OA_NONE,OA_NONE} },
      { "%retload/vec4",of_RETLOAD_VEC4,1,{OA_സംഖ്യ,  OA_NONE,OA_NONE} },
      { "%reverse/obj", of_REVERSE_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%reverse/prop/obj", of_REVERSE_PROP_OBJ, 1, {OA_സംഖ്യ, OA_NONE, OA_NONE} },
      { "%rsort/obj", of_RSORT_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%rsort/prop/obj", of_RSORT_PROP_OBJ, 1, {OA_സംഖ്യ, OA_NONE, OA_NONE} },
      { "%scopy",  of_SCOPY,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%set/dar/obj/real",of_SET_DAR_OBJ_REAL,1,{OA_സംഖ്യ,OA_NONE,OA_NONE} },
      { "%set/dar/obj/str", of_SET_DAR_OBJ_STR, 1,{OA_സംഖ്യ,OA_NONE,OA_NONE} },
      { "%set/dar/obj/vec4",of_SET_DAR_OBJ_VEC4,1,{OA_സംഖ്യ,OA_NONE,OA_NONE} },
      { "%shiftl",   of_SHIFTL,   1, {OA_സംഖ്യ, OA_NONE,   OA_NONE} },
      { "%shiftr",   of_SHIFTR,   1, {OA_സംഖ്യ, OA_NONE,   OA_NONE} },
      { "%shiftr/s", of_SHIFTR_S, 1, {OA_സംഖ്യ, OA_NONE,   OA_NONE} },
      { "%shuffle/obj", of_SHUFFLE_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%shuffle/prop/obj", of_SHUFFLE_PROP_OBJ, 1, {OA_സംഖ്യ, OA_NONE, OA_NONE} },
      { "%sort/obj", of_SORT_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%sort/prop/obj", of_SORT_PROP_OBJ, 1, {OA_സംഖ്യ, OA_NONE, OA_NONE} },
      { "%split/vec4",    of_SPLIT_VEC4,    1,{OA_സംഖ്യ,   OA_NONE, OA_NONE} },
      { "%store/dar/r",   of_STORE_DAR_R,   1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/str", of_STORE_DAR_STR, 1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/vec4",of_STORE_DAR_VEC4,1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/obj",   of_STORE_OBJ,   1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/obja",  of_STORE_OBJA,  2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/prop/obj",of_STORE_PROP_OBJ,2, {OA_സംഖ്യ,  OA_BIT1, OA_NONE} },
      { "%store/prop/qb/r",   of_STORE_PROP_QB_R,   2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/str", of_STORE_PROP_QB_STR, 2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/v",   of_STORE_PROP_QB_V,   3, {OA_സംഖ്യ, OA_BIT1, OA_BIT2} },
      { "%store/prop/qf/r",   of_STORE_PROP_QF_R,   2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/str", of_STORE_PROP_QF_STR, 2, {OA_സംഖ്യ, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/v",   of_STORE_PROP_QF_V,   3, {OA_സംഖ്യ, OA_BIT1, OA_BIT2} },
      { "%store/prop/r",  of_STORE_PROP_R,  1, {OA_സംഖ്യ,  OA_NONE, OA_NONE} },
      { "%store/prop/str",of_STORE_PROP_STR,1, {OA_സംഖ്യ,  OA_NONE, OA_NONE} },
      { "%store/prop/v",  of_STORE_PROP_V,  2, {OA_സംഖ്യ,  OA_BIT1, OA_NONE} },
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
      { "%subi",   of_SUBI,   3,  {OA_BIT1,     OA_BIT2,     OA_സംഖ്യ} },
      { "%substr",     of_SUBSTR,     2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%substr/vec4",of_SUBSTR_VEC4,2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%test_nul",     of_TEST_NUL,     1,{OA_FUNC_PTR,OA_NONE,    OA_NONE} },
      { "%test_nul/a",   of_TEST_NUL_A,   2,{OA_ARR_PTR, OA_BIT1,    OA_NONE} },
      { "%test_nul/obj", of_TEST_NUL_OBJ, 0,{OA_NONE,    OA_NONE,    OA_NONE} },
      { "%test_nul/prop",of_TEST_NUL_PROP,2,{OA_സംഖ്യ,  OA_BIT1,    OA_NONE} },
      { "%uparr/and/v",          of_UPARR_AND_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/or/v",           of_UPARR_OR_V,           2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/product/obj/v",  of_UPARR_PRODUCT_OBJ_V,  1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/product/prop/v", of_UPARR_PRODUCT_PROP_V, 2, {OA_സംഖ്യ,   OA_BIT1, OA_NONE} },
      { "%uparr/product/r",      of_UPARR_PRODUCT_REAL,   1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/product/v",      of_UPARR_PRODUCT_V,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/sum/obj/v",      of_UPARR_SUM_OBJ_V,      1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/sum/prop/v",     of_UPARR_SUM_PROP_V,     2, {OA_സംഖ്യ,   OA_BIT1, OA_NONE} },
      { "%uparr/sum/r",          of_UPARR_SUM_REAL,       1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/sum/v",          of_UPARR_SUM_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/xor/v",          of_UPARR_XOR_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%wait",   of_WAIT,   1,  {OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%wait/fork",of_WAIT_FORK,0,{OA_NONE,   OA_NONE,     OA_NONE} },
      { "%xnor",   of_XNOR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xnor/r", of_XNORR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor",    of_XOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor/r",  of_XORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { 0, of_NOOP, 0, {OA_NONE, OA_NONE, OA_NONE} }
};

സ്ഥിതം സ്ഥിരം unsigned opcode_എണ്ണം =
                    വലുപ്പം(opcode_table)/വലുപ്പം(*opcode_table) - 1;

സ്ഥിതം പൂർണ്ണസംഖ്യ opcode_compare(സ്ഥിരം ശൂന്യം*k, സ്ഥിരം ശൂന്യം*r)
{
      സ്ഥിരം അക്ഷരം*kp = static_cast<സ്ഥിരം അക്ഷരം*>(k);
      സ്ഥിരം ഘടന opcode_table_s*rp = static_cast<സ്ഥിരം ഘടന opcode_table_s*>(r);
      തിരികെ strcmp(kp, rp->mnemonic);
}

/*
 * Keep a symbol table of addresses within code space. Labels on
 * executable opcodes are mapped to their address here.
 */
സ്ഥിതം ചിഹ്നം_table_t sym_codespace = 0;

/*
 * Keep a symbol table of functors mentioned in the source. This table
 * is used to resolve references as they come.
 */
സ്ഥിതം ചിഹ്നം_table_t sym_functors = 0;

/*
 * VPI objects are indexed during compile time so that they can be
 * linked together as they are created. This symbol table matches
 * labels to vpiHandles.
 */
സ്ഥിതം ചിഹ്നം_table_t sym_vpi = 0;


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

ശൂന്യം define_functor_ചിഹ്നം(സ്ഥിരം അക്ഷരം*label, vvp_net_t*net)
{
      ചിഹ്നം_മൂല്യം_t val;
      val.net = net;
      sym_set_മൂല്യം(sym_functors, label, val);
}

സ്ഥിതം vvp_net_t*lookup_functor_ചിഹ്നം(സ്ഥിരം അക്ഷരം*label)
{
      പ്രതിജ്ഞാ(sym_functors);
      ചിഹ്നം_മൂല്യം_t val = sym_get_മൂല്യം(sym_functors, label);
      തിരികെ val.net;
}

vpiHandle vvp_lookup_handle(സ്ഥിരം അക്ഷരം*label)
{
      ചിഹ്നം_മൂല്യം_t val = sym_get_മൂല്യം(sym_vpi, label);
      എങ്കിൽ (val.ptr) തിരികെ (vpiHandle) val.ptr;
      തിരികെ 0;
}

vvp_net_t* vvp_net_lookup(സ്ഥിരം അക്ഷരം*label)
{
      സ്ഥിതം തർക്കം t0_trigger_generated = അസത്യം;

        /* First, look to see if the symbol is a vpi object of some
	   sort. If it is, then get the vvp_ipoint_t pointer out of
	   the vpiHandle. */
      ചിഹ്നം_മൂല്യം_t val = sym_get_മൂല്യം(sym_vpi, label);
      എങ്കിൽ (val.ptr) {
	    vpiHandle vpi = static_cast<vpiHandle>(val.ptr);
	    തിരഞ്ഞെടുപ്പ് (vpi->get_type_code()) {
		അവസ്ഥ vpiNet:
		അവസ്ഥ vpiReg:
		അവസ്ഥ vpiBitVar:
		അവസ്ഥ vpiByteVar:
		അവസ്ഥ vpiShortIntVar:
		അവസ്ഥ vpiIntVar:
		അവസ്ഥ vpiLongIntVar:
		അവസ്ഥ vpiIntegerVar: {
		      സ്ഥിരം __vpiSignal*sig = dynamic_cast<__vpiSignal*>(vpi);
		      തിരികെ sig->കണ്ണി;
		}

		അവസ്ഥ vpiRealVar: {
		      സ്ഥിരം __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(vpi);
		      തിരികെ sig->net;
		}

		അവസ്ഥ vpiStringVar:
		അവസ്ഥ vpiArrayVar:
		അവസ്ഥ vpiClassVar: {
		      സ്ഥിരം __vpiBaseVar*sig = dynamic_cast<__vpiBaseVar*>(vpi);
		      തിരികെ sig->get_net();
		}

		അവസ്ഥ vpiNamedEvent: {
		      സ്ഥിരം __vpiNamedEvent*tmp = dynamic_cast<__vpiNamedEvent*>(vpi);
		      തിരികെ tmp->funct;
		}

		സ്ഥിരസ്ഥിതി:
		  fprintf(stderr, "Unsupported type %d.\n",
		          vpi->get_type_code());
		  പ്രതിജ്ഞാ(0);
	    }
      }


	/* Failing that, look for a general functor. */
      vvp_net_t*tmp = lookup_functor_ചിഹ്നം(label);

	// This is a special label used to create a T0 trigger for the
	// always_comb/latch processes.
     എങ്കിൽ (! t0_trigger_generated && (strcmp(label, "E_0x0") == 0)) {
	      // This should never happen, but if it does then the E_0x0
	      // event generation may need to be explicitly generated in
	      // the compiler output instead of implicitly in this code.
	    പ്രതിജ്ഞാ(! vpip_peek_current_scope()->is_automatic());
	    t0_trigger_generated = സത്യം;
	      // Create a local event with no name for the T0 trigger
	    compile_named_event(strdup(label), strcpy(പുതിയ അക്ഷരം [1],""), സത്യം);
	    tmp = vvp_net_lookup(label);
	    പ്രതിജ്ഞാ(tmp);
	      // Create a trigger for the T0 event.
	    vvp_net_ptr_t ptr (tmp, 0);
	    schedule_t0_trigger(ptr);
     }

      തിരികെ tmp;
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
സ്ഥിതം resolv_list_s*resolv_list = 0;

resolv_list_s::~resolv_list_s()
{
      സ്വതന്ത്രം(label_);
}

ശൂന്യം resolv_submit(resolv_list_s*cur)
{
      എങ്കിൽ (cur->resolve()) {
	    delete cur;
	    തിരികെ;
      }

      cur->next = resolv_list;
      resolv_list = cur;
}


/*
 * Look up vvp_nets in the symbol table. The "source" is the label for
 * the net that I want to feed, and net->port[port] is the vvp_net
 * input that I want that node to feed into. When the name is found,
 * put net->port[port] into the fan-out list for that node.
 */
ഘടന vvp_net_resolv_list_s: പൊതു resolv_list_s {

      explicit vvp_net_resolv_list_s(അക്ഷരം*l) : resolv_list_s(l) { }
	// port to be driven by the located node.
      vvp_net_ptr_t port;
      മായാ തർക്കം resolve(തർക്കം mes) override;
};

തർക്കം vvp_net_resolv_list_s::resolve(തർക്കം mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      എങ്കിൽ (tmp) {
	      // Link the input port to the located output.
	    tmp->link(port);
	    തിരികെ സത്യം;
      }

      എങ്കിൽ (mes)
	    fprintf(stderr, "unresolved vvp_net reference: %s\n", label());

      തിരികെ അസത്യം;
}

വരിയിൽ സ്ഥിതം
ശൂന്യം postpone_functor_നിവേശം(vvp_net_ptr_t port, അക്ഷരം*lab)
{
      ഘടന vvp_net_resolv_list_s*res = പുതിയ ഘടന vvp_net_resolv_list_s(lab);
      res->port   = port;

      resolv_submit(res);
}


/*
 *  Generic functor reference lookup.
 */

ഘടന functor_gen_resolv_list_s: പൊതു resolv_list_s {
      explicit functor_gen_resolv_list_s(അക്ഷരം*txt) : resolv_list_s(txt) {
	    ref = 0;
      }
      vvp_net_t**ref;
      മായാ തർക്കം resolve(തർക്കം mes) override;
};

തർക്കം functor_gen_resolv_list_s::resolve(തർക്കം mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      എങ്കിൽ (tmp) {
	    *ref = tmp;
	    തിരികെ സത്യം;
      }

      എങ്കിൽ (mes)
	    fprintf(stderr, "unresolved functor reference: %s\n", label());

      തിരികെ അസത്യം;
}

ശൂന്യം functor_ref_lookup(vvp_net_t**ref, അക്ഷരം*lab)
{
      ഘടന functor_gen_resolv_list_s*res =
	    പുതിയ ഘടന functor_gen_resolv_list_s(lab);

      res->ref    = ref;

      resolv_submit(res);
}

/*
 *  vpiHandle lookup
 */

ഘടന vpi_handle_resolv_list_s: പൊതു resolv_list_s {
      explicit vpi_handle_resolv_list_s(അക്ഷരം*lab) : resolv_list_s(lab) {
	    handle = ശൂന്യ;
      }
      മായാ തർക്കം resolve(തർക്കം mes) override;
      vpiHandle *handle;
};

തർക്കം vpi_handle_resolv_list_s::resolve(തർക്കം mes)
{
      ചിഹ്നം_മൂല്യം_t val = sym_get_മൂല്യം(sym_vpi, label());
      എങ്കിൽ (!val.ptr) {
	    // check for thread access symbols
	    unsigned base, wid;
	    size_t n = 0;
	    അക്ഷരം ss[32];
	    എങ്കിൽ (2 == sscanf(label(), "W<%u,%31[r]>%zn", &base, ss, &n)
		       && n == strlen(label())) {

		  val.ptr = vpip_make_vthr_word(base, ss);
		  sym_set_മൂല്യം(sym_vpi, label(), val);

	    } അല്ലെങ്കിൽ എങ്കിൽ (1 == sscanf(label(), "S<%u,str>%zn", &base, &n)
		       && n == strlen(label())) {

		  val.ptr = vpip_make_vthr_str_അട്ടി(base);
		  sym_set_മൂല്യം(sym_vpi, label(), val);

	    } അല്ലെങ്കിൽ എങ്കിൽ (3 == sscanf(label(), "S<%u,vec4,%31[su]%u>%zn", &base, ss, &wid, &n)
		       && n == strlen(label())) {

		  തർക്കം signed_flag = അസത്യം;
		  വേണ്ടി (അക്ഷരം*fp = ss ;  *fp ;  fp += 1) തിരഞ്ഞെടുപ്പ് (*fp) {
		      അവസ്ഥ 's':
			signed_flag = സത്യം;
			നിർത്തുക;
		      അവസ്ഥ 'u':
			signed_flag = അസത്യം;
			നിർത്തുക;
		      സ്ഥിരസ്ഥിതി:
			നിർത്തുക;
		  }
		  val.ptr = vpip_make_vthr_vec4_അട്ടി(base, signed_flag, wid);
		  sym_set_മൂല്യം(sym_vpi, label(), val);
	    }

      }

      എങ്കിൽ (!val.ptr) {
	    // check for memory word  M<mem,base,wid>
      }

      എങ്കിൽ (strcmp(label(), "null") == 0) {
	    val.ptr = vpip_make_null_const();
	    sym_set_മൂല്യം(sym_vpi, label(), val);
      }

      എങ്കിൽ (val.ptr) {
	    *handle = static_cast<vpiHandle>(val.ptr);
	    തിരികെ സത്യം;
      }

      എങ്കിൽ (mes)
	    fprintf(stderr, "unresolved vpi name lookup: %s\n", label());

      തിരികെ അസത്യം;
}

ശൂന്യം compile_vpi_lookup(vpiHandle *handle, അക്ഷരം*label)
{
      എങ്കിൽ (strcmp(label, "$time") == 0) {
	    *handle = vpip_sim_time(vpip_peek_current_scope(), അസത്യം);
	    സ്വതന്ത്രം(label);
	    തിരികെ;
      }

      എങ്കിൽ (strcmp(label, "$stime") == 0) {
	    *handle = vpip_sim_time(vpip_peek_current_scope(), സത്യം);
	    സ്വതന്ത്രം(label);
	    തിരികെ;
      }

      എങ്കിൽ (strcmp(label, "$realtime") == 0) {
	    *handle = vpip_sim_realtime(vpip_peek_current_scope());
	    സ്വതന്ത്രം(label);
	    തിരികെ;
      }

      എങ്കിൽ (strcmp(label, "$simtime") == 0) {
	    *handle = vpip_sim_time(0, അസത്യം);
	    സ്വതന്ത്രം(label);
	    തിരികെ;
      }

      ഘടന vpi_handle_resolv_list_s*res
	    = പുതിയ ഘടന vpi_handle_resolv_list_s(label);

      res->handle = handle;
      resolv_submit(res);
}

/*
 * Code Label lookup
 */

ഘടന code_label_resolv_list_s: പൊതു resolv_list_s {
      explicit code_label_resolv_list_s(അക്ഷരം*lab, തർക്കം cptr2) : resolv_list_s(lab) {
	    code = ശൂന്യ;
	    cptr2_flag = cptr2;
      }
      ഘടന vvp_code_s *code;
      തർക്കം cptr2_flag;
      മായാ തർക്കം resolve(തർക്കം mes) override;
};

തർക്കം code_label_resolv_list_s::resolve(തർക്കം mes)
{
      ചിഹ്നം_മൂല്യം_t val = sym_get_മൂല്യം(sym_codespace, label());
      എങ്കിൽ (val.ptr) {
	    എങ്കിൽ (cptr2_flag)
		  code->cptr2 = reinterpret_cast<vvp_code_t>(val.ptr);
	    അല്ലെങ്കിൽ
		  code->cptr = reinterpret_cast<vvp_code_t>(val.ptr);
	    തിരികെ സത്യം;
      }

      എങ്കിൽ (mes)
	    fprintf(stderr, "unresolved code label: %s\n", label());

      തിരികെ അസത്യം;
}

ശൂന്യം code_label_lookup(ഘടന vvp_code_s *code, അക്ഷരം *label, തർക്കം cptr2)
{
      ഘടന code_label_resolv_list_s *res
	    = പുതിയ ഘടന code_label_resolv_list_s(label, cptr2);

      res->code  = code;

      resolv_submit(res);
}

ഘടന code_array_resolv_list_s: പൊതു resolv_list_s {
      explicit code_array_resolv_list_s(അക്ഷരം*lab) : resolv_list_s(lab) {
	    code = ശൂന്യ;
      }
      ഘടന vvp_code_s *code;
      മായാ തർക്കം resolve(തർക്കം mes) override;
};

തർക്കം code_array_resolv_list_s::resolve(തർക്കം mes)
{
      code->array = array_find(label());
      എങ്കിൽ (code->array != 0) {
	    തിരികെ സത്യം;
      }

      എങ്കിൽ (mes)
	    fprintf(stderr, "Array unresolved: %s\n", label());

      തിരികെ അസത്യം;
}

സ്ഥിതം ശൂന്യം compile_array_lookup(ഘടന vvp_code_s*code, അക്ഷരം*label)
{
      ഘടന code_array_resolv_list_s *res
	    = പുതിയ ഘടന code_array_resolv_list_s(label);

      res->code  = code;

      resolv_submit(res);
}

സ്ഥിതം std::list<ഘടന __vpiSysTaskCall*> scheduled_compiletf;

ശൂന്യം compile_compiletf(ഘടന __vpiSysTaskCall*obj)
{
      എങ്കിൽ (obj->defn->info.compiletf == 0)
	    തിരികെ;

      scheduled_compiletf.push_back(obj);
}

/*
 * When parsing is otherwise complete, this function is called to do
 * the final stuff. Clean up deferred linking here.
 */

ശൂന്യം compile_cleanup(ശൂന്യം)
{
      പൂർണ്ണസംഖ്യ lnerrs = -1;
      പൂർണ്ണസംഖ്യ nerrs = 0;
      പൂർണ്ണസംഖ്യ last;

      എങ്കിൽ (verbose_flag) {
	    fprintf(stderr, " ... Linking\n");
	    fflush(stderr);
      }

      ചെയ്യുക {
	    resolv_list_s *res = resolv_list;
	    resolv_list = 0x0;
	    last = nerrs == lnerrs;
	    lnerrs = nerrs;
	    nerrs = 0;
	    ആകുമ്പോൾ (res) {
		  resolv_list_s *cur = res;
		  res = res->next;
		  എങ്കിൽ (cur->resolve(last))
			delete cur;
		  അല്ലെങ്കിൽ {
			nerrs++;
			cur->next = resolv_list;
			resolv_list = cur;
		  }
	    }
	    എങ്കിൽ (nerrs && last)
		  fprintf(stderr,
			  "compile_cleanup: %d unresolved items\n",
			  nerrs);
      } ആകുമ്പോൾ (nerrs && !last);

      compile_errors += nerrs;

      എങ്കിൽ (verbose_flag) {
	    fprintf(stderr, " ... Removing symbol tables\n");
	    fflush(stderr);
      }

	/* After compile is complete, the vpi symbol table is no
	   longer needed. VPI objects are located by following
	   scopes. */
      delete_ചിഹ്നം_table(sym_vpi);
      sym_vpi = 0;

	/* Don't need the code labels. The instructions have numeric
	   pointers in them, the symbol table is no longer needed. */
      delete_ചിഹ്നം_table(sym_codespace);
      sym_codespace = 0;

      delete_ചിഹ്നം_table(sym_functors);
      sym_functors = 0;

      delete_udp_symbols();

      compile_island_cleanup();
      compile_array_cleanup();

      എങ്കിൽ (verbose_flag) {
	    fprintf(stderr, " ... Compiletf functions\n");
	    fflush(stderr);
      }

      പ്രതിജ്ഞാ(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_COMPILETF;

      ആകുമ്പോൾ (! scheduled_compiletf.empty()) {
	    ഘടന __vpiSysTaskCall*obj = scheduled_compiletf.front();
	    scheduled_compiletf.pop_front();
	    vpip_cur_task = obj;
	    obj->defn->info.compiletf (obj->defn->info.user_data);
	    vpip_cur_task = 0;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

ശൂന്യം compile_vpi_ചിഹ്നം(സ്ഥിരം അക്ഷരം*label, vpiHandle obj)
{
      ചിഹ്നം_മൂല്യം_t val;
      val.ptr = obj;
      sym_set_മൂല്യം(sym_vpi, label, val);
}

/*
 * Initialize the compiler by allocation empty symbol tables and
 * initializing the various address spaces.
 */
ശൂന്യം compile_init(ശൂന്യം)
{
      sym_vpi = new_ചിഹ്നം_table();

      sym_functors = new_ചിഹ്നം_table();

      sym_codespace = new_ചിഹ്നം_table();
      codespace_init();
}

ശൂന്യം compile_load_vpi_module(അക്ഷരം*name)
{
      vpip_load_module(name);
      delete[] name;
}

ശൂന്യം compile_vpi_time_precision(long pre)
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
സ്ഥിതം തർക്കം crstring_header_test(സ്ഥിരം അക്ഷരം*str)
{
      എങ്കിൽ ((str[0] != 'C') && (str[0] != 'c'))
	    തിരികെ അസത്യം;
      എങ്കിൽ ((str[1] != 'r') || (str[2] != '<'))
	    തിരികെ അസത്യം;

      തിരികെ സത്യം;
}

തർക്കം crstring_test(സ്ഥിരം അക്ഷരം*str)
{
      എങ്കിൽ (!crstring_header_test(str))
	    തിരികെ അസത്യം;
      സ്ഥിരം അക്ഷരം*tp = strchr(str, '>');
      എങ്കിൽ (tp == 0) തിരികെ അസത്യം;
      എങ്കിൽ (tp[1] != 0) തിരികെ അസത്യം;

      എങ്കിൽ ((strspn(str+3, "0123456789abcdefmg")+3) != (size_t)(tp - str))
	  തിരികെ അസത്യം;

      തിരികെ സത്യം;
}

ഇരട്ട crstring_to_double(സ്ഥിരം അക്ഷരം*label)
{
      പ്രതിജ്ഞാ(crstring_header_test(label));

      സ്ഥിരം അക്ഷരം*cp = label+3;
      പ്രതിജ്ഞാ(*cp == 'm');
      cp += 1;
      അക്ഷരം*ep;
      uint64_t mant = strtoull(cp, &ep, 16);
      cp = ep;
      പ്രതിജ്ഞാ(*cp == 'g');
      cp += 1;
      പൂർണ്ണസംഖ്യ exp = strtoul(cp, 0, 16);

      ഇരട്ട tmp;
      എങ്കിൽ (mant == 0 && exp == 0x3fff) {
	    tmp = INFINITY;
      } അല്ലെങ്കിൽ എങ്കിൽ (mant == 0 && exp == 0x7fff) {
	    tmp = -INFINITY;
      } അല്ലെങ്കിൽ എങ്കിൽ (exp == 0x3fff) {
	    tmp = nan("");
      } അല്ലെങ്കിൽ {
	    ഇരട്ട sign = (exp & 0x4000)? -1.0 : 1.0;
	    exp &= 0x1fff;

	    tmp = sign * ldexp((ഇരട്ട)mant, exp - 0x1000);
      }

      തിരികെ tmp;
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

ശൂന്യം നിവേശം_connect(vvp_net_t*fdx, unsigned port, അക്ഷരം*label)
{
      vvp_net_ptr_t ifdx = vvp_net_ptr_t(fdx, port);

	/* Is this a vvp_vector4_t constant value? */
      എങ്കിൽ (c4string_test(label)) {

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
	    എങ്കിൽ (label[0] == 'c')
		  schedule_init_vector(ifdx, tmp);
	    അല്ലെങ്കിൽ
		  schedule_set_vector(ifdx, tmp);

	    സ്വതന്ത്രം(label);
	    തിരികെ;
      }

	/* Is this a vvp_vector8_t constant value? */
      എങ്കിൽ (c8string_test(label)) {

	    vvp_vector8_t tmp = c8string_to_vector8(label);
	    എങ്കിൽ (label[0] == 'c')
		  schedule_init_vector(ifdx, tmp);
	    അല്ലെങ്കിൽ
		  schedule_set_vector(ifdx, tmp);

	    സ്വതന്ത്രം(label);
	    തിരികെ;
      }

	/* Handle the Cr<> constant driver, which is a real-value
	   driver. */
      എങ്കിൽ (crstring_test(label)) {

	    ഇരട്ട tmp = crstring_to_double(label);

	    എങ്കിൽ (label[0] == 'c')
		  schedule_init_vector(ifdx, tmp);
	    അല്ലെങ്കിൽ
		  schedule_set_vector(ifdx, tmp);
	    സ്വതന്ത്രം(label);
	    തിരികെ;
      }

	/* Handle the general case that this is a label for a node in
	   the vvp net. This arranges for the label to be preserved in
	   a linker list, and linked when the symbol table is
	   complete. */
      postpone_functor_നിവേശം(ifdx, label);
}

ശൂന്യം inputs_connect(vvp_net_t*fdx, unsigned argc, ഘടന symb_s*argv)
{
      എങ്കിൽ (argc > 4) {
	    std::cerr << "XXXX argv[0] = " << argv[0].text << std::endl;
      }
      പ്രതിജ്ഞാ(argc <= 4);

      വേണ്ടി (unsigned idx = 0;  idx < argc;  idx += 1) {

	    നിവേശം_connect(fdx, idx, argv[idx].text);
      }
}

ശൂന്യം wide_inputs_connect(vvp_wide_fun_core*core,
			 unsigned argc, ഘടന symb_s*argv)
{
	/* Create input functors to receive values from the
	   network. These functors pass the data to the core. */
      unsigned നിവേശം_functors = (argc+3) / 4;
      വേണ്ടി (unsigned idx = 0 ;  idx < നിവേശം_functors ;  idx += 1) {
	    unsigned base = idx*4;
	    unsigned trans = 4;
	    എങ്കിൽ (base+trans > argc)
		  trans = argc - base;

	    vvp_wide_fun_t*cur = പുതിയ vvp_wide_fun_t(core, base);
	    vvp_net_t*ptr = പുതിയ vvp_net_t;
	    ptr->fun = cur;

	    inputs_connect(ptr, trans, argv+base);
      }
}

template <വർഗ്ഗം T_> ശൂന്യം make_arith(T_ *arith, അക്ഷരം*label,
				    unsigned argc, ഘടന symb_s*argv)
{
      vvp_net_t* ptr = പുതിയ vvp_net_t;
      ptr->fun = arith;

      define_functor_ചിഹ്നം(label, ptr);
      സ്വതന്ത്രം(label);

      പ്രതിജ്ഞാ(argc == 2);
      inputs_connect(ptr, argc, argv);

      സ്വതന്ത്രം(argv);
}

ശൂന്യം compile_arith_cast_int(അക്ഷരം*label, long width,
                            unsigned argc, ഘടന symb_s*argv)
{
      vvp_arith_cast_int*arith = പുതിയ vvp_arith_cast_int((unsigned) width);

      vvp_net_t* ptr = പുതിയ vvp_net_t;
      ptr->fun = arith;

      define_functor_ചിഹ്നം(label, ptr);
      സ്വതന്ത്രം(label);

      പ്രതിജ്ഞാ(argc == 1);
      inputs_connect(ptr, argc, argv);
      സ്വതന്ത്രം(argv);
}

ശൂന്യം compile_arith_cast_vec2(അക്ഷരം*label, long width,
			     unsigned argc, ഘടന symb_s*argv)
{
      vvp_arith_cast_vec2*arith = പുതിയ vvp_arith_cast_vec2((unsigned) width);

      vvp_net_t* ptr = പുതിയ vvp_net_t;
      ptr->fun = arith;

      define_functor_ചിഹ്നം(label, ptr);
      സ്വതന്ത്രം(label);

      പ്രതിജ്ഞാ(argc == 1);
      inputs_connect(ptr, argc, argv);
      സ്വതന്ത്രം(argv);
}

ശൂന്യം compile_arith_cast_real(അക്ഷരം*label, തർക്കം signed_flag,
                             unsigned argc, ഘടന symb_s*argv)
{
      vvp_arith_cast_real*arith = പുതിയ vvp_arith_cast_real(signed_flag);

      vvp_net_t* ptr = പുതിയ vvp_net_t;
      ptr->fun = arith;

      define_functor_ചിഹ്നം(label, ptr);
      സ്വതന്ത്രം(label);

      പ്രതിജ്ഞാ(argc == 1);
      inputs_connect(ptr, argc, argv);
      സ്വതന്ത്രം(argv);
}

ശൂന്യം compile_arith_abs(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv)
{
      vvp_arith_abs*arith = പുതിയ vvp_arith_abs;

      vvp_net_t* ptr = പുതിയ vvp_net_t;
      ptr->fun = arith;

      define_functor_ചിഹ്നം(label, ptr);
      സ്വതന്ത്രം(label);

      പ്രതിജ്ഞാ(argc == 1);
      inputs_connect(ptr, argc, argv);
      സ്വതന്ത്രം(argv);
}

ശൂന്യം compile_arith_div(അക്ഷരം*label, long wid, തർക്കം signed_flag,
		       unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    സ്ഥിരം അക്ഷരം *suffix = "";
	    എങ്കിൽ (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s; .arith/div%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_arith_div(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_arith_div_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv)
{
      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s; .arith/divr has wrong number of symbols\n", label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_real_ *arith = പുതിയ vvp_arith_div_real;
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_arith_mod(അക്ഷരം*label, long wid, തർക്കം signed_flag,
		       unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .arith/mod has wrong number of symbols\n", label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_arith_mod(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_arith_mod_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv)
{
      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .arith/mod.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_real_ *arith = പുതിയ vvp_arith_mod_real;
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_arith_mult(അക്ഷരം*label, long wid,
			unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .arith/mult has wrong number of symbols\n", label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_arith_mult(wid);
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_arith_mult_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv)
{
      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .arith/mult.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_real_ *arith = പുതിയ vvp_arith_mult_real;
      make_arith(arith, label, argc, argv);
}


ശൂന്യം compile_arith_pow(അക്ഷരം*label, long wid, തർക്കം signed_flag,
		       unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    സ്ഥിരം അക്ഷരം *suffix = "";
	    എങ്കിൽ (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s .arith/pow%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_arith_pow(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_arith_pow_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv)
{
      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .arith/pow.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_real_ *arith = പുതിയ vvp_arith_pow_real;
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_arith_sub(അക്ഷരം*label, long wid, unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .arith/sub has wrong number of symbols\n", label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_arith_sub(wid);
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_arith_sub_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv)
{
      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s; .arith/sub.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_real_ *arith = പുതിയ vvp_arith_sub_real;
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_arith_sum(അക്ഷരം*label, long wid, unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .arith/sum has wrong number of symbols\n", label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_arith_sum(wid);
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_arith_sum_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv)
{
      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .arith/sum.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_real_ *arith = പുതിയ vvp_arith_sum_real;
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_cmp_eeq(അക്ഷരം*label, long wid,
		     unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_cmp_eeq(wid);

      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_cmp_nee(അക്ഷരം*label, long wid,
		     unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_cmp_nee(wid);

      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_cmp_eq(അക്ഷരം*label, long wid, unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_cmp_eq(wid);
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_cmp_eqx(അക്ഷരം*label, long wid, unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqx has wrong number of symbols\n",label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_cmp_eqx(wid);
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_cmp_eqz(അക്ഷരം*label, long wid, unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqz has wrong number of symbols\n",label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_cmp_eqz(wid);
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_cmp_eq_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv)
{
      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_real_ *arith = പുതിയ vvp_cmp_eq_real;
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_cmp_ne(അക്ഷരം*label, long wid, unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_cmp_ne(wid);
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_cmp_ne_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv)
{
      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_real_ *arith = പുതിയ vvp_cmp_ne_real;
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_cmp_ge(അക്ഷരം*label, long wid, തർക്കം signed_flag,
		    unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge has wrong number of symbols\n", label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_cmp_ge(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_cmp_ge_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv)
{
      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_real_ *arith = പുതിയ vvp_cmp_ge_real;
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_cmp_gt(അക്ഷരം*label, long wid, തർക്കം signed_flag,
		    unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt has wrong number of symbols\n", label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_cmp_gt(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_cmp_gt_r(അക്ഷരം*label, unsigned argc, ഘടന symb_s*argv)
{
      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_real_ *arith = പുതിയ vvp_cmp_gt_real;
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_cmp_weq(അക്ഷരം*label, long wid,
		     unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .cmp/weq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_cmp_weq(wid);

      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_cmp_wne(അക്ഷരം*label, long wid,
		     unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      എങ്കിൽ (argc != 2) {
	    fprintf(stderr, "%s .cmp/wne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    തിരികെ;
      }

      vvp_arith_ *arith = പുതിയ vvp_cmp_wne(wid);

      make_arith(arith, label, argc, argv);
}


ശൂന്യം compile_delay(അക്ഷരം*label, unsigned width,
                   vvp_delay_t*delay, ഘടന symb_s arg)
{
      vvp_net_t*net = പുതിയ vvp_net_t;
      vvp_fun_delay*obj = പുതിയ vvp_fun_delay(net, width, *delay);
      net->fun = obj;

      delete delay;

      നിവേശം_connect(net, 0, arg.text);

      define_functor_ചിഹ്നം(label, net);
      സ്വതന്ത്രം(label);
}

ശൂന്യം compile_delay(അക്ഷരം*label, unsigned width,
                   unsigned argc, ഘടന symb_s*argv, തർക്കം ignore_decay)
{
      vvp_delay_t stub (0, 0, 0);
      എങ്കിൽ (ignore_decay) stub.set_ignore_decay();
      vvp_net_t*net = പുതിയ vvp_net_t;
      vvp_fun_delay*obj = പുതിയ vvp_fun_delay(net, width, stub);
      net->fun = obj;

      inputs_connect(net, argc, argv);
      സ്വതന്ത്രം(argv);

      define_functor_ചിഹ്നം(label, net);
      സ്വതന്ത്രം(label);
}

/*
 * Extend nodes.
 */
ശൂന്യം compile_extend_signed(അക്ഷരം*label, long wid, ഘടന symb_s arg)
{
      പ്രതിജ്ഞാ(wid >= 0);

      vvp_fun_extend_signed*fun = പുതിയ vvp_fun_extend_signed(wid);
      vvp_net_t*ptr = പുതിയ vvp_net_t;
      ptr->fun = fun;

      define_functor_ചിഹ്നം(label, ptr);
      സ്വതന്ത്രം(label);

      നിവേശം_connect(ptr, 0, arg.text);
}

ഘടന __vpiModPath* compile_modpath(അക്ഷരം*label, unsigned width,
                                     ഘടന symb_s drv, ഘടന symb_s dest)
{
      vvp_net_t*net = പുതിയ vvp_net_t;
      vvp_fun_modpath*obj = പുതിയ vvp_fun_modpath(net, width);
      net->fun = obj;

      നിവേശം_connect(net, 0, drv.text);

      define_functor_ചിഹ്നം(label, net);

      __vpiModPath*modpath = vpip_make_modpath(net);

      compile_vpi_lookup(&modpath->path_term_out.വ്യഞ്ജകം, dest.text);

      സ്വതന്ത്രം(label);

      modpath->modpath = obj;
      തിരികെ modpath;
}

സ്ഥിതം ഘടന __vpiModPathSrc*make_modpath_src(ഘടന __vpiModPath*path,
					       അക്ഷരം edge_c,
					       സ്ഥിരം ഘടന symb_s&src,
					       ഘടന numbv_s&vals,
					       തർക്കം ifnone)
{
      vvp_fun_modpath*dst = path->modpath;

      vvp_time64_t use_delay[12];
      പ്രതിജ്ഞാ(vals.cnt == 12);
      വേണ്ടി (unsigned idx = 0 ; idx < vals.cnt ;  idx += 1) {
	    use_delay[idx] = vals.nvec[idx];
      }

      numbv_clear(&vals);

      vvp_fun_modpath_src*obj = 0;

      പൂർണ്ണസംഖ്യ vpi_edge = vpiNoEdge;
      എങ്കിൽ (edge_c == 0) {
	    obj = പുതിയ vvp_fun_modpath_src(use_delay);

      } അല്ലെങ്കിൽ {
	    തർക്കം posedge, negedge;
	    തിരഞ്ഞെടുപ്പ് (edge_c) {
		അവസ്ഥ '+':
		  vpi_edge = vpiPosedge;
		  posedge = സത്യം;
		  negedge = അസത്യം;
		  നിർത്തുക;
		അവസ്ഥ '-':
		  vpi_edge = vpiNegedge;
		  posedge = അസത്യം;
		  negedge = സത്യം;
		  നിർത്തുക;
#എങ്കിൽ 0
		അവസ്ഥ '*':
		  posedge = സത്യം;
		  negedge = സത്യം;
		  നിർത്തുക;
#endif
		സ്ഥിരസ്ഥിതി:
		  posedge = അസത്യം;
		  negedge = അസത്യം;
		  fprintf(stderr, "Unknown edge identifier %c(%d).\n", edge_c,
		          edge_c);
		  പ്രതിജ്ഞാ(0);
	    }
	    obj = പുതിയ vvp_fun_modpath_edge(use_delay, posedge, negedge);
      }

      vvp_net_t*net = പുതിയ vvp_net_t;
      ഘടന __vpiModPathSrc* srcobj = vpip_make_modpath_src(path, net) ;
      vpip_attach_to_current_scope(srcobj);
      net->fun = obj;

	/* Save the vpiEdge directory into the input path term. */
      srcobj->path_term_in.edge = vpi_edge;
      നിവേശം_connect(net, 0, src.text);
      dst->add_modpath_src(obj, ifnone);

      തിരികെ srcobj;
}

ശൂന്യം compile_modpath_src(ഘടന __vpiModPath*dst, അക്ഷരം edge_c,
			 സ്ഥിരം ഘടന symb_s&src,
			 ഘടന numbv_s&vals,
			 സ്ഥിരം ഘടന symb_s&condit_src,
			 സ്ഥിരം ഘടന symb_s&path_term_in)
{
      ഘടന __vpiModPathSrc*obj =
	    make_modpath_src(dst, edge_c, src, vals, അസത്യം);
      നിവേശം_connect(obj->net, 1, condit_src.text);
      compile_vpi_lookup(&obj->path_term_in.വ്യഞ്ജകം, path_term_in.text);
}

ശൂന്യം compile_modpath_src(ഘടന __vpiModPath*dst, അക്ഷരം edge_c,
			 സ്ഥിരം ഘടന symb_s&src,
			 ഘടന numbv_s&vals,
			 പൂർണ്ണസംഖ്യ condit_src,
			 സ്ഥിരം ഘടന symb_s&path_term_in,
			 തർക്കം ifnone)
{
      പ്രതിജ്ഞാ(condit_src == 0);
      ഘടന __vpiModPathSrc*obj =
	    make_modpath_src(dst, edge_c, src, vals, ifnone);
      compile_vpi_lookup(&obj->path_term_in.വ്യഞ്ജകം, path_term_in.text);
}

/*
 * A .shift/l statement creates an array of functors for the
 * width. The 0 input is the data vector to be shifted and the 1 input
 * is the amount of the shift. An unconnected shift amount is set to 0.
 */
ശൂന്യം compile_shiftl(അക്ഷരം*label, long wid, unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      vvp_arith_ *arith = പുതിയ vvp_shiftl(wid);
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_shiftr(അക്ഷരം*label, long wid, തർക്കം signed_flag,
		    unsigned argc, ഘടന symb_s*argv)
{
      പ്രതിജ്ഞാ( wid > 0 );

      vvp_arith_ *arith = പുതിയ vvp_shiftr(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

ശൂന്യം compile_resolver(അക്ഷരം*label, അക്ഷരം*type, unsigned argc, ഘടന symb_s*argv)
{
      vvp_net_t*net = പുതിയ vvp_net_t;

      resolv_core*core = 0;

      എങ്കിൽ (strcmp(type,"tri") == 0) {
	    core = പുതിയ resolv_tri(argc, net, vvp_scalar_t(BIT4_Z, 0,0));

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"tri0") == 0) {
	    core = പുതിയ resolv_tri(argc, net, vvp_scalar_t(BIT4_0, 5,5));

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"tri1") == 0) {
	    core = പുതിയ resolv_tri(argc, net, vvp_scalar_t(BIT4_1, 5,5));

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"triand") == 0) {
	    core = പുതിയ resolv_triand(argc, net);

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"trior") == 0) {
	    core = പുതിയ resolv_trior(argc, net);

      } അല്ലെങ്കിൽ {
	    fprintf(stderr, "invalid resolver type: %s\n", type);
	    compile_errors += 1;
            delete net;
      }

      എങ്കിൽ (core) {
	    net->fun = core;
	    define_functor_ചിഹ്നം(label, net);

            വേണ്ടി (unsigned base = 0 ;  base < argc ;  base += 4) {
	          unsigned nports = argc - base;
                  എങ്കിൽ (nports > 4)
                        nports = 4;

                  എങ്കിൽ (base > 0) {
                        net = പുതിയ vvp_net_t;
                        net->fun = പുതിയ resolv_extend(core, base);
                  }
                  inputs_connect(net, nports, argv+base);
            }
      }
      സ്വതന്ത്രം(type);
      സ്വതന്ത്രം(label);
      സ്വതന്ത്രം(argv);
}

ശൂന്യം compile_udp_def(പൂർണ്ണസംഖ്യ sequ, അക്ഷരം *label, അക്ഷരം *name,
		     unsigned nin, unsigned init, അക്ഷരം **table)
{
      എങ്കിൽ (sequ) {
	    vvp_bit4_t init4;
	    എങ്കിൽ (init == 0)
		  init4 = BIT4_0;
	    അല്ലെങ്കിൽ എങ്കിൽ (init == 1)
		  init4 = BIT4_1;
	    അല്ലെങ്കിൽ
		  init4 = BIT4_X;

	    vvp_udp_seq_s *u = പുതിയ vvp_udp_seq_s(label, name, nin, init4);
	    u->compile_table(table);
      } അല്ലെങ്കിൽ {
	    vvp_udp_comb_s *u = പുതിയ vvp_udp_comb_s(label, name, nin);
	    u->compile_table(table);
      }
      സ്വതന്ത്രം(label);
}

അക്ഷരം **compile_udp_table(അക്ഷരം **table, അക്ഷരം *row)
{
  എങ്കിൽ (table)
    പ്രതിജ്ഞാ(strlen(*table)==strlen(row));

  അക്ഷരം **tt;
  വേണ്ടി (tt = table; tt && *tt; tt++) { }
  പൂർണ്ണസംഖ്യ n = (tt-table) + 2;

  table = static_cast<അക്ഷരം**>(realloc(table, n*വലുപ്പം(അക്ഷരം*)));
  table[n-2] = row;
  table[n-1] = 0x0;

  തിരികെ table;
}


/*
 * The parser uses this function to compile and link an executable
 * opcode. I do this by looking up the opcode in the opcode_table. The
 * table gives the operand structure that is acceptable, so I can
 * process the operands here as well.
 */
ശൂന്യം compile_code(അക്ഷരം*label, അക്ഷരം*mnem, comp_operands_t opa)
{
	/* First, I can give the label a value that is the current
	   codespace pointer. Don't need the text of the label after
	   this is done. */
      എങ്കിൽ (label)
	    compile_codelabel(label);

	/* Lookup the opcode in the opcode table. */
      ഘടന opcode_table_s*op = static_cast<ഘടന opcode_table_s*>
                                            (bsearch(mnem, opcode_table, opcode_എണ്ണം,
                                                     വലുപ്പം(ഘടന opcode_table_s),
                                                     &opcode_compare));
      എങ്കിൽ (op == 0) {
	    yyerror("Invalid opcode");
	    compile_errors += 1;
	    തിരികെ;
      }

      പ്രതിജ്ഞാ(op);

	/* Build up the code from the information about the opcode and
	   the information from the compiler. */
      vvp_code_t code = codespace_allocate();
      code->opcode = op->opcode;

      എങ്കിൽ (op->argc != (opa? opa->argc : 0)) {
	    yyerror("operand count");
	    compile_errors += 1;
	    തിരികെ;
      }

	/* Pull the operands that the instruction expects from the
	   list that the parser supplied. */

      വേണ്ടി (unsigned idx = 0 ;  idx < op->argc ;  idx += 1) {

	    തിരഞ്ഞെടുപ്പ് (op->argt[idx]) {
		അവസ്ഥ OA_NONE:
		  നിർത്തുക;

		അവസ്ഥ OA_ARR_PTR:
		  എങ്കിൽ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			നിർത്തുക;
		  }

		  compile_array_lookup(code, opa->argv[idx].symb.text);
		  നിർത്തുക;

		അവസ്ഥ OA_BIT1:
		  എങ്കിൽ (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			നിർത്തുക;
		  }
		  code->bit_idx[0] = opa->argv[idx].numb;
		  നിർത്തുക;

		അവസ്ഥ OA_BIT2:
		  എങ്കിൽ (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			നിർത്തുക;
		  }
		  code->bit_idx[1] = opa->argv[idx].numb;
		  നിർത്തുക;

		അവസ്ഥ OA_CODE_PTR:
		അവസ്ഥ OA_CODE_PTR2:
		  എങ്കിൽ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			നിർത്തുക;
		  }

		  പ്രതിജ്ഞാ(opa->argv[idx].symb.idx == 0);
		  code_label_lookup(code, opa->argv[idx].symb.text,
				    op->argt[idx]==OA_CODE_PTR2);
		  നിർത്തുക;

		അവസ്ഥ OA_FUNC_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  എങ്കിൽ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			നിർത്തുക;
		  }

		  functor_ref_lookup(&code->net, opa->argv[idx].symb.text);
		  നിർത്തുക;

		അവസ്ഥ OA_FUNC_PTR2:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  എങ്കിൽ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			നിർത്തുക;
		  }

		  functor_ref_lookup(&code->net2, opa->argv[idx].symb.text);
		  നിർത്തുക;

		അവസ്ഥ OA_സംഖ്യ:
		  എങ്കിൽ (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			നിർത്തുക;
		  }

		  code->സംഖ്യ = opa->argv[idx].numb;
		  നിർത്തുക;

		അവസ്ഥ OA_VPI_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  എങ്കിൽ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			നിർത്തുക;
		  }

		  compile_vpi_lookup(&code->handle, opa->argv[idx].symb.text);
		  നിർത്തുക;

		അവസ്ഥ OA_STRING:
		  എങ്കിൽ (opa->argv[idx].ltype != L_STRING) {
			yyerror("operand format");
			നിർത്തുക;
		  }

		  code->text = opa->argv[idx].text;
		  നിർത്തുക;
	    }
      }

      സ്വതന്ത്രം(opa);

      സ്വതന്ത്രം(mnem);
}

ശൂന്യം compile_codelabel(അക്ഷരം*label)
{
      ചിഹ്നം_മൂല്യം_t val;
      vvp_code_t ptr = codespace_next();

      val.ptr = ptr;
      sym_set_മൂല്യം(sym_codespace, label, val);

      സ്വതന്ത്രം(label);
}

ശൂന്യം compile_file_വരി(അക്ഷരം*label, long file_idx, long lineno,
                       അക്ഷരം*description)
{
      എങ്കിൽ (label) compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_code_t code = codespace_allocate();
      code->opcode = &of_FILE_വരി;

	/* Create a vpiHandle that contains the information. */
      code->handle = vpip_build_file_വരി(description, file_idx, lineno);
      പ്രതിജ്ഞാ(code->handle);

	/* Done with the lexor-allocated name string. */
      delete[] description;
}

ശൂന്യം compile_vpi_call(അക്ഷരം*label, അക്ഷരം*name,
                      തർക്കം func_as_task_err, തർക്കം func_as_task_warn,
                      long file_idx, long lineno,
                      unsigned argc, vpiHandle*argv,
		      unsigned vec4_അട്ടി, unsigned real_അട്ടി, unsigned string_അട്ടി)
{
      എങ്കിൽ (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_code_t code = codespace_allocate();
      code->opcode = &of_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, 0, 0,
					 0, func_as_task_err, func_as_task_warn,
                                         argc, argv, vec4_അട്ടി, real_അട്ടി, string_അട്ടി,
					 file_idx, lineno);
      എങ്കിൽ (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      delete[] name;
}

ശൂന്യം compile_vpi_func_call(അക്ഷരം*label, അക്ഷരം*name,
			   പൂർണ്ണസംഖ്യ val_type, unsigned val_wid,
			   long file_idx, long lineno,
			   unsigned argc, vpiHandle*argv,
			   unsigned vec4_അട്ടി,
			   unsigned real_അട്ടി,
			   unsigned string_അട്ടി)
{
      എങ്കിൽ (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_code_t code = codespace_allocate();
      code->opcode = &of_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, val_type, val_wid,
					 0, സത്യം, അസത്യം,
                                         argc, argv, vec4_അട്ടി, real_അട്ടി, string_അട്ടി,
					 file_idx, lineno);
      എങ്കിൽ (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      delete[] name;
}

/*
 * When the parser finds a thread statement, I create a new thread
 * with the start address referenced by the program symbol passed to
 * me.
 */
ശൂന്യം compile_thread(അക്ഷരം*start_sym, അക്ഷരം*flag)
{
      തർക്കം തള്ളുക_flag = അസത്യം;

      ചിഹ്നം_മൂല്യം_t tmp = sym_get_മൂല്യം(sym_codespace, start_sym);
      vvp_code_t pc = reinterpret_cast<vvp_code_t>(tmp.ptr);
      എങ്കിൽ (pc == 0) {
	    yyerror("unresolved address");
	    തിരികെ;
      }

      എങ്കിൽ (flag && (strcmp(flag,"$push") == 0))
	    തള്ളുക_flag = സത്യം;

      vthread_t thr = vthread_new(pc, vpip_peek_current_scope());

      എങ്കിൽ (flag && (strcmp(flag,"$init") == 0))
	    schedule_init_vthread(thr);
      അല്ലെങ്കിൽ എങ്കിൽ (flag && (strcmp(flag,"$final") == 0))
	    schedule_final_vthread(thr);
      അല്ലെങ്കിൽ
	    schedule_vthread(thr, 0, തള്ളുക_flag);

      സ്വതന്ത്രം(start_sym);
      സ്വതന്ത്രം(flag);
}

ശൂന്യം compile_param_logic(അക്ഷരം*label, സ്ഥിരം അക്ഷരം*name, അക്ഷരം*value, തർക്കം signed_flag,
                         തർക്കം local_flag,
                         long file_idx, long lineno)
{
      vvp_vector4_t value4 = c4string_to_vector4(value);
      vpiHandle obj = vpip_make_ദ്വിമാന_param(name, value4, signed_flag,
                                             local_flag, file_idx, lineno);
      compile_vpi_ചിഹ്നം(label, obj);
      vpip_attach_to_current_scope(obj);

      സ്വതന്ത്രം(label);
      സ്വതന്ത്രം(value);
}

ശൂന്യം compile_param_string(അക്ഷരം*label, സ്ഥിരം അക്ഷരം*name, അക്ഷരം*value,
                          തർക്കം local_flag,
                          long file_idx, long lineno)
{
	// name and value become owned by vpip_make_string_param
      vpiHandle obj = vpip_make_string_param(name, value, local_flag, file_idx, lineno);
      compile_vpi_ചിഹ്നം(label, obj);
      vpip_attach_to_current_scope(obj);

      സ്വതന്ത്രം(label);
}

ശൂന്യം compile_param_real(അക്ഷരം*label, സ്ഥിരം അക്ഷരം*name, അക്ഷരം*value,
                        തർക്കം local_flag,
                        long file_idx, long lineno)
{
      ഇരട്ട dvalue = crstring_to_double(value);
      vpiHandle obj = vpip_make_real_param(name, dvalue, local_flag, file_idx, lineno);
      compile_vpi_ചിഹ്നം(label, obj);
      vpip_attach_to_current_scope(obj);

      സ്വതന്ത്രം(label);
      സ്വതന്ത്രം(value);
}

ശൂന്യം compile_island(അക്ഷരം*label, അക്ഷരം*type)
{
      എങ്കിൽ (strcmp(type,"tran") == 0)
	    compile_island_tran(label);
      അല്ലെങ്കിൽ
	    പ്രതിജ്ഞാ(0);

      സ്വതന്ത്രം(type);
}
