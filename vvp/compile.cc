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

#اگر_تعریف_شدہ __MINGW32__
#شامل <windows.h>
#اگر_ختم

# include  "ivl_alloc.h"

غیرعلامتی compile_errors = 0;

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

شمار عملیہ_e {
	/* Place holder for unused operand */
      OA_NONE,
	/* The operand is a number, an immediate unsigned integer */
      OA_نمبر,
	/* The operand is a pointer to an array. */
      OA_ARR_PTR,
	/* The operand is a thread bit index or short integer */
      OA_BIT1,
      OA_BIT2,
	/* The operand is a pointer to code space */
      OA_رمز_PTR,
      OA_رمز_PTR2,
	/* The operand is a variable or net pointer */
      OA_FUNC_PTR,
	/* The operand is a second functor pointer */
      OA_FUNC_PTR2,
	/* The operand is a VPI handle */
      OA_VPI_PTR,
	/* String */
      OA_STRING
};

ساخت opcode_جدول_s {
      ثابت حرف*mnemonic;
      vvp_رمز_fun opcode;

      غیرعلامتی argc;
      شمار عملیہ_e argt[عملیہ_MAX];
};

ساکن ثابت ساخت opcode_جدول_s opcode_جدول[] = {
      { "%abs/wr", کا_ABS_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add",    کا_جوڑو,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add/wr", کا_جوڑو_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%addi",   کا_ADDI,   3,  {OA_BIT1,     OA_BIT2,     OA_نمبر} },
      { "%alloc",  کا_ALLOC,  1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%and",    کا_AND,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%and/r",  کا_ANDR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%assign/ar",کا_تفویض_AR,2,{OA_ARR_PTR,OA_BIT1,     OA_NONE} },
      { "%assign/ar/d",کا_تفویض_ARD,2,{OA_ARR_PTR,OA_BIT1,  OA_NONE} },
      { "%assign/ar/e",کا_تفویض_ARE,1,{OA_ARR_PTR,OA_NONE,  OA_NONE} },
      { "%assign/vec4",      کا_تفویض_VEC4,       2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/a/d",  کا_تفویض_VEC4_A_D,   3, {OA_ARR_PTR,  OA_BIT1, OA_BIT2} },
      { "%assign/vec4/a/e",  کا_تفویض_VEC4_A_E,   2, {OA_ARR_PTR,  OA_BIT1, OA_NONE} },
      { "%assign/vec4/d",    کا_تفویض_VEC4_D,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/e",    کا_تفویض_VEC4_E,      1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%assign/vec4/off/d",کا_تفویض_VEC4_OFF_D, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%assign/vec4/off/e",کا_تفویض_VEC4_OFF_E, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr",  کا_تفویض_WR, 2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/d",کا_تفویض_WRD,2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/e",کا_تفویض_WRE,1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%blend",     کا_BLEND,     0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/str", کا_BLEND_STR, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/wr",  کا_BLEND_WR,  0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%breakpoint", کا_BREAKPOINT, 0,  {OA_NONE, OA_NONE, OA_NONE} },
      { "%callf/obj",       کا_CALLF_OBJ,       2,{OA_رمز_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/real",      کا_CALLF_REAL,      2,{OA_رمز_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/str",       کا_CALLF_STR,       2,{OA_رمز_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/vec4",      کا_CALLF_VEC4,      2,{OA_رمز_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/void",      کا_CALLF_VOID,      2,{OA_رمز_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%cassign/link",    کا_CASSIGN_LINK,    2,{OA_FUNC_PTR,OA_FUNC_PTR2,OA_NONE} },
      { "%cassign/vec4",    کا_CASSIGN_VEC4,    1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cassign/vec4/off",کا_CASSIGN_VEC4_OFF,2,{OA_FUNC_PTR,OA_BIT1,     OA_NONE} },
      { "%cassign/wr",  کا_CASSIGN_WR,  1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cast/vec2/dar", کا_CAST_VEC2_DAR, 1,  {OA_نمبر,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/dar", کا_CAST_VEC4_DAR, 1,  {OA_نمبر,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/str", کا_CAST_VEC4_STR, 1,  {OA_نمبر,   OA_NONE,     OA_NONE} },
      { "%cast2",   کا_CAST2,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/e",   کا_CMPE,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/ne",  کا_CMPNE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/s",   کا_CMPS,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/str", کا_CMPSTR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/u",   کا_CMPU,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/we",  کا_CMPWE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wne", کا_CMPWNE, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wr",  کا_CMPWR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/x",   کا_CMPX,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/z",   کا_CMPZ,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmpi/e",  کا_CMPIE,  3,  {OA_BIT1,     OA_BIT2,     OA_نمبر} },
      { "%cmpi/ne", کا_CMPINE, 3,  {OA_BIT1,     OA_BIT2,     OA_نمبر} },
      { "%cmpi/s",  کا_CMPIS,  3,  {OA_BIT1,     OA_BIT2,     OA_نمبر} },
      { "%cmpi/u",  کا_CMPIU,  3,  {OA_BIT1,     OA_BIT2,     OA_نمبر} },
      { "%cmpix/ltu", کا_CMPIX_LTU, 2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%cmpix/slt0", کا_CMPIX_SLT0, 1, {OA_نمبر, OA_NONE, OA_NONE} },
      { "%concat/str",  کا_CONCAT_STR,  0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concat/vec4", کا_CONCAT_VEC4, 0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concati/str", کا_CONCATI_STR, 1,{OA_STRING,OA_NONE,  OA_NONE} },
      { "%concati/vec4",کا_CONCATI_VEC4,3,{OA_BIT1,  OA_BIT2,  OA_نمبر} },
      { "%cvt/rv",   کا_CVT_RV,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/rv/s", کا_CVT_RV_S,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/sr", کا_CVT_SR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/ur", کا_CVT_UR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/vr", کا_CVT_VR, 1,  {OA_نمبر,   OA_NONE,     OA_NONE} },
      { "%deassign",کا_DEASSIGN,3,{OA_FUNC_PTR, OA_BIT1,     OA_BIT2} },
      { "%deassign/wr",کا_DEASSIGN_WR,1,{OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%debug/thr",  کا_DEBUG_THR,  1,{OA_STRING,   OA_NONE,     OA_NONE} },
      { "%delay",  کا_DELAY,  2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%delayx", کا_DELAYX, 1,  {OA_نمبر,   OA_NONE,     OA_NONE} },
      { "%delete/elem",کا_DELETE_ELEM,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%delete/obj",کا_DELETE_OBJ,1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%delete/prop/elem", کا_DELETE_PROP_ELEM, 1, {OA_نمبر, OA_NONE, OA_NONE} },
      { "%delete/prop/obj",  کا_DELETE_PROP_OBJ,  1, {OA_نمبر, OA_NONE, OA_NONE} },
      { "%delete/tail",کا_DELETE_دم,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%disable",  کا_DISABLE, 1, {OA_VPI_PTR,OA_NONE,     OA_NONE} },
      { "%disable/flow", کا_DISABLE_FLOW, 1, {OA_VPI_PTR,OA_NONE, OA_NONE} },
      { "%disable/fork",کا_DISABLE_FORK,0,{OA_NONE,OA_NONE,  OA_NONE} },
      { "%div",      کا_DIV,     0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/s",    کا_DIV_S,   0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/wr",   کا_DIV_WR,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/obj",  کا_DUP_OBJ, 0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/real", کا_DUP_REAL,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/vec4", کا_DUP_VEC4,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%end",    کا_انتہا,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl",  کا_EVCTL,  2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/c",کا_EVCTLC, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl/i",کا_EVCTLI, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/s",کا_EVCTLS, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%event",    کا_EVENT,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%event/nb", کا_EVENT_NB, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%flag_get/vec4", کا_FLAG_GET_VEC4, 1, {OA_نمبر, OA_NONE, OA_NONE} },
      { "%flag_inv",      کا_FLAG_INV,      1, {OA_BIT1,   OA_NONE, OA_NONE} },
      { "%flag_mov",      کا_FLAG_MOV,      2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_or",       کا_FLAG_OR,       2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_set/imm",  کا_FLAG_سیٹ_IMM,  2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%flag_set/vec4", کا_FLAG_سیٹ_VEC4, 1, {OA_نمبر, OA_NONE, OA_NONE} },
      { "%force/link",    کا_FORCE_LINK,2,{OA_FUNC_PTR, OA_FUNC_PTR2, OA_NONE} },
      { "%force/vec4",    کا_FORCE_VEC4,    1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%force/vec4/off",کا_FORCE_VEC4_OFF,2,{OA_FUNC_PTR, OA_BIT1,      OA_NONE} },
      { "%force/vec4/off/d",کا_FORCE_VEC4_OFF_D,3,{OA_FUNC_PTR, OA_BIT1,  OA_BIT2} },
      { "%force/wr",      کا_FORCE_WR,      1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%fork",   کا_FORK,   2,  {OA_رمز_PTR2,OA_VPI_PTR,  OA_NONE} },
      { "%free",   کا_FREE,   1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%inv",    کا_INV,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%ix/add", کا_IX_جوڑو, 3,  {OA_نمبر,   OA_BIT1,     OA_BIT2} },
      { "%ix/getv",کا_IX_GETV,2,  {OA_BIT1,     OA_FUNC_PTR, OA_NONE} },
      { "%ix/getv/s",کا_IX_GETV_S,2, {OA_BIT1,   OA_FUNC_PTR, OA_NONE} },
      { "%ix/load",کا_IX_بھرو,3,  {OA_نمبر,   OA_BIT1,     OA_BIT2} },
      { "%ix/mov", کا_IX_MOV, 2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%ix/mul", کا_IX_MUL, 3,  {OA_نمبر,   OA_BIT1,     OA_BIT2} },
      { "%ix/sub", کا_IX_SUB, 3,  {OA_نمبر,   OA_BIT1,     OA_BIT2} },
      { "%ix/vec4",  کا_IX_VEC4,  1,  {OA_نمبر,   OA_NONE, OA_NONE} },
      { "%ix/vec4/s",کا_IX_VEC4_S,1,  {OA_نمبر,   OA_NONE, OA_NONE} },
      { "%jmp",    کا_JMP,    1,  {OA_رمز_PTR, OA_NONE,     OA_NONE} },
      { "%jmp/0",  کا_JMP0,   2,  {OA_رمز_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/0xz",کا_JMP0_XZ, 2,  {OA_رمز_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1",  کا_JMP1,   2,  {OA_رمز_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1xz",کا_JMP1_XZ, 2,  {OA_رمز_PTR, OA_BIT1,     OA_NONE} },
      { "%join",   کا_JOIN,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%join/detach",کا_JOIN_DETACH,1,{OA_نمبر,OA_NONE,  OA_NONE} },
      { "%load/ar",کا_بھرو_AR,2,  {OA_ARR_PTR,  OA_BIT1,     OA_NONE} },
      { "%load/dar/r",  کا_بھرو_DAR_R,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE}},
      { "%load/dar/str",کا_بھرو_DAR_STR,  1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/dar/vec4",کا_بھرو_DAR_VEC4,1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/obj",   کا_بھرو_OBJ,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/obja",  کا_بھرو_OBJA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/prop/dar/vec4", کا_بھرو_PROP_DAR_VEC4, 2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%load/real",  کا_بھرو_REAL, 1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%load/str",   کا_بھرو_STR,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/stra",  کا_بھرو_STRA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/vec4",  کا_بھرو_VEC4, 1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%load/vec4a", کا_بھرو_VEC4_A,2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%max/wr", کا_MAX_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%min/wr", کا_MIN_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod",    کا_MOD,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/s",  کا_MOD_S,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/wr", کا_MOD_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul",    کا_MUL,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul/wr", کا_MUL_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%muli",   کا_MULI,   3,  {OA_BIT1,     OA_BIT2,     OA_نمبر} },
      { "%nand",   کا_NAND,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nand/r", کا_NANDR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%neg/wr", کا_NEG_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%new/cobj",  کا_NEW_COBJ,  1, {OA_VPI_PTR,OA_NONE,  OA_NONE} },
      { "%new/darray",کا_NEW_DARRAY,2, {OA_BIT1,   OA_STRING,OA_NONE} },
      { "%noop",   کا_NOOP,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor",    کا_NOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor/r",  کا_NORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%null",   کا_NULL,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or",     کا_OR,     0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or/r",   کا_ORR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%pad/s",  کا_PAD_S,  1,  {OA_نمبر,   OA_NONE,     OA_NONE} },
      { "%pad/u",  کا_PAD_U,  1,  {OA_نمبر,   OA_NONE,     OA_NONE} },
      { "%part/s", کا_PART_S, 1,  {OA_نمبر,   OA_NONE,     OA_NONE} },
      { "%part/u", کا_PART_U, 1,  {OA_نمبر,   OA_NONE,     OA_NONE} },
      { "%parti/s",کا_PARTI_S,3,  {OA_نمبر,   OA_BIT1,     OA_BIT2} },
      { "%parti/u",کا_PARTI_U,3,  {OA_نمبر,   OA_BIT1,     OA_BIT2} },
      { "%pop/obj", کا_نکالو_OBJ, 2, {OA_BIT1,    OA_BIT2,     OA_NONE} },
      { "%pop/real",کا_نکالو_REAL,1, {OA_نمبر,  OA_NONE,     OA_NONE} },
      { "%pop/str", کا_نکالو_STR, 1, {OA_نمبر,  OA_NONE,     OA_NONE} },
      { "%pop/vec4",کا_نکالو_VEC4,1, {OA_نمبر,  OA_NONE,     OA_NONE} },
      { "%pow",     کا_POW,     0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/s",   کا_POW_S,   0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/wr",  کا_POW_WR,  0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%prop/obj",کا_PROP_OBJ,2,  {OA_نمبر,   OA_BIT1,     OA_NONE} },
      { "%prop/queue/size", کا_PROP_قطار_سائز,  1, {OA_نمبر, OA_NONE, OA_NONE} },
      { "%prop/r",  کا_PROP_R,  1,  {OA_نمبر,   OA_NONE,     OA_NONE} },
      { "%prop/str",کا_PROP_STR,1,  {OA_نمبر,   OA_NONE,     OA_NONE} },
      { "%prop/v",  کا_PROP_V,  1,  {OA_نمبر,   OA_NONE,     OA_NONE} },
      { "%push/ix/vec4", کا_ڈالو_IX_VEC4, 3, {OA_نمبر, OA_BIT1, OA_BIT2} },
      { "%pushi/real",کا_PUSHI_REAL,2,{OA_BIT1,   OA_BIT2,   OA_NONE} },
      { "%pushi/str", کا_PUSHI_STR, 1,{OA_STRING, OA_NONE,   OA_NONE} },
      { "%pushi/vec4",کا_PUSHI_VEC4,3,{OA_BIT1,   OA_BIT2,   OA_نمبر} },
      { "%pushv/str", کا_PUSHV_STR, 0,{OA_NONE,   OA_NONE,   OA_NONE} },
      { "%putc/str/vec4",کا_PUTC_STR_VEC4,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/prop/r",   کا_QINSERT_PROP_R,   2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/str", کا_QINSERT_PROP_STR, 2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/v",   کا_QINSERT_PROP_V,   3, {OA_نمبر, OA_BIT1, OA_BIT2} },
      { "%qinsert/real",کا_QINSERT_REAL,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/str", کا_QINSERT_STR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/v",   کا_QINSERT_V,   3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%qpop/b/real",کا_QPOP_B_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/str", کا_QPOP_B_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/v",   کا_QPOP_B_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/f/real",کا_QPOP_F_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/str", کا_QPOP_F_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/v",   کا_QPOP_F_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/prop/b/r",   کا_QPOP_PROP_B_REAL, 1, {OA_نمبر, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/str", کا_QPOP_PROP_B_STR, 1, {OA_نمبر, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/v",   کا_QPOP_PROP_B_V,    2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%qpop/prop/f/r",   کا_QPOP_PROP_F_REAL, 1, {OA_نمبر, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/str", کا_QPOP_PROP_F_STR, 1, {OA_نمبر, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/v",   کا_QPOP_PROP_F_V,    2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%queue/append_word/v", کا_قطار_APPEND_لفظ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/find/index/prop/v", کا_قطار_ڈھونڈو_اشاریہ_PROP_V, 2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%queue/find/index/v", کا_قطار_ڈھونڈو_اشاریہ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find/prop/v", کا_قطار_ڈھونڈو_PROP_V, 2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%queue/find/v", کا_قطار_ڈھونڈو_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/prop/v", کا_قطار_ڈھونڈو_پہلا_اشاریہ_PROP_V, 2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/v", کا_قطار_ڈھونڈو_پہلا_اشاریہ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/prop/v", کا_قطار_ڈھونڈو_پہلا_PROP_V, 2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%queue/find_first/v", کا_قطار_ڈھونڈو_پہلا_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/prop/v", کا_قطار_ڈھونڈو_آخری_اشاریہ_PROP_V, 2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/v", کا_قطار_ڈھونڈو_آخری_اشاریہ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/prop/v", کا_قطار_ڈھونڈو_آخری_PROP_V, 2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%queue/find_last/v", کا_قطار_ڈھونڈو_آخری_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/max/obj/v", کا_قطار_MAX_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/max/prop/v", کا_قطار_MAX_PROP_V, 2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%queue/max/v", کا_قطار_MAX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/min/obj/v", کا_قطار_MIN_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/min/prop/v", کا_قطار_MIN_PROP_V, 2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%queue/min/v", کا_قطار_MIN_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/new_empty/v", کا_قطار_NEW_خالی_خانے_V, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%queue/size/v", کا_قطار_سائز_V, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%queue/unique/index/obj/v", کا_قطار_UNIQUE_اشاریہ_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/index/prop/v", کا_قطار_UNIQUE_اشاریہ_PROP_V, 2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%queue/unique/index/v", کا_قطار_UNIQUE_اشاریہ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/unique/obj/v", کا_قطار_UNIQUE_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/prop/v", کا_قطار_UNIQUE_PROP_V, 2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%queue/unique/v", کا_قطار_UNIQUE_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/word/prop/v", کا_قطار_لفظ_PROP_V, 3, {OA_نمبر, OA_BIT1, OA_BIT2} },
      { "%queue/word/v", کا_قطار_لفظ_V, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%release/net",کا_RELEASE_NET,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/reg",کا_RELEASE_REG,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/wr", کا_RELEASE_WR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%replicate", کا_REPLICATE,   1,{OA_نمبر,  OA_NONE,OA_NONE} },
      { "%ret/real",  کا_RET_REAL,    1,{OA_نمبر,  OA_NONE,OA_NONE} },
      { "%ret/str",   کا_RET_STR,     1,{OA_نمبر,  OA_NONE,OA_NONE} },
      { "%ret/vec4",  کا_RET_VEC4,    3,{OA_نمبر,  OA_BIT1,OA_BIT2} },
      { "%retload/real",کا_RETLOAD_REAL,1,{OA_نمبر,  OA_NONE,OA_NONE} },
      { "%retload/str", کا_RETLOAD_STR, 1,{OA_نمبر,  OA_NONE,OA_NONE} },
      { "%retload/vec4",کا_RETLOAD_VEC4,1,{OA_نمبر,  OA_NONE,OA_NONE} },
      { "%reverse/obj", کا_الٹ_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%reverse/prop/obj", کا_الٹ_PROP_OBJ, 1, {OA_نمبر, OA_NONE, OA_NONE} },
      { "%rsort/obj", کا_RSORT_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%rsort/prop/obj", کا_RSORT_PROP_OBJ, 1, {OA_نمبر, OA_NONE, OA_NONE} },
      { "%scopy",  کا_SCOPY,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%set/dar/obj/real",کا_سیٹ_DAR_OBJ_REAL,1,{OA_نمبر,OA_NONE,OA_NONE} },
      { "%set/dar/obj/str", کا_سیٹ_DAR_OBJ_STR, 1,{OA_نمبر,OA_NONE,OA_NONE} },
      { "%set/dar/obj/vec4",کا_سیٹ_DAR_OBJ_VEC4,1,{OA_نمبر,OA_NONE,OA_NONE} },
      { "%shiftl",   کا_SHIFTL,   1, {OA_نمبر, OA_NONE,   OA_NONE} },
      { "%shiftr",   کا_SHIFTR,   1, {OA_نمبر, OA_NONE,   OA_NONE} },
      { "%shiftr/s", کا_SHIFTR_S, 1, {OA_نمبر, OA_NONE,   OA_NONE} },
      { "%shuffle/obj", کا_SHUFFLE_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%shuffle/prop/obj", کا_SHUFFLE_PROP_OBJ, 1, {OA_نمبر, OA_NONE, OA_NONE} },
      { "%sort/obj", کا_ترتیب_دو_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%sort/prop/obj", کا_ترتیب_دو_PROP_OBJ, 1, {OA_نمبر, OA_NONE, OA_NONE} },
      { "%split/vec4",    کا_بانٹو_VEC4,    1,{OA_نمبر,   OA_NONE, OA_NONE} },
      { "%store/dar/r",   کا_STORE_DAR_R,   1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/str", کا_STORE_DAR_STR, 1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/vec4",کا_STORE_DAR_VEC4,1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/obj",   کا_STORE_OBJ,   1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/obja",  کا_STORE_OBJA,  2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/prop/obj",کا_STORE_PROP_OBJ,2, {OA_نمبر,  OA_BIT1, OA_NONE} },
      { "%store/prop/qb/r",   کا_STORE_PROP_QB_R,   2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/str", کا_STORE_PROP_QB_STR, 2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/v",   کا_STORE_PROP_QB_V,   3, {OA_نمبر, OA_BIT1, OA_BIT2} },
      { "%store/prop/qf/r",   کا_STORE_PROP_QF_R,   2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/str", کا_STORE_PROP_QF_STR, 2, {OA_نمبر, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/v",   کا_STORE_PROP_QF_V,   3, {OA_نمبر, OA_BIT1, OA_BIT2} },
      { "%store/prop/r",  کا_STORE_PROP_R,  1, {OA_نمبر,  OA_NONE, OA_NONE} },
      { "%store/prop/str",کا_STORE_PROP_STR,1, {OA_نمبر,  OA_NONE, OA_NONE} },
      { "%store/prop/v",  کا_STORE_PROP_V,  2, {OA_نمبر,  OA_BIT1, OA_NONE} },
      { "%store/qb/r",   کا_STORE_QB_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/str", کا_STORE_QB_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/v",   کا_STORE_QB_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qdar/r",  کا_STORE_QDAR_R,  2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/str",کا_STORE_QDAR_STR,2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/v",  کا_STORE_QDAR_V,  3,{OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qf/r",   کا_STORE_QF_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/str", کا_STORE_QF_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/v",   کا_STORE_QF_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qobj/r",  کا_STORE_QOBJ_R,  2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/str",کا_STORE_QOBJ_STR,2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/v",  کا_STORE_QOBJ_V,  3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/real",    کا_STORE_REAL,    1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/reala",   کا_STORE_REALA,   2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/str",     کا_STORE_STR,     1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/stra",    کا_STORE_STRA,    2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/vec4",    کا_STORE_VEC4,    3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/vec4a",   کا_STORE_VEC4_A,   3, {OA_ARR_PTR, OA_BIT1, OA_BIT2} },
      { "%sub",    کا_SUB,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%sub/wr", کا_SUB_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%subi",   کا_SUBI,   3,  {OA_BIT1,     OA_BIT2,     OA_نمبر} },
      { "%substr",     کا_SUBSTR,     2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%substr/vec4",کا_SUBSTR_VEC4,2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%test_nul",     کا_TEST_NUL,     1,{OA_FUNC_PTR,OA_NONE,    OA_NONE} },
      { "%test_nul/a",   کا_TEST_NUL_A,   2,{OA_ARR_PTR, OA_BIT1,    OA_NONE} },
      { "%test_nul/obj", کا_TEST_NUL_OBJ, 0,{OA_NONE,    OA_NONE,    OA_NONE} },
      { "%test_nul/prop",کا_TEST_NUL_PROP,2,{OA_نمبر,  OA_BIT1,    OA_NONE} },
      { "%uparr/and/v",          کا_UPARR_AND_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/or/v",           کا_UPARR_OR_V,           2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/product/obj/v",  کا_UPARR_حاصل_ضرب_OBJ_V,  1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/product/prop/v", کا_UPARR_حاصل_ضرب_PROP_V, 2, {OA_نمبر,   OA_BIT1, OA_NONE} },
      { "%uparr/product/r",      کا_UPARR_حاصل_ضرب_REAL,   1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/product/v",      کا_UPARR_حاصل_ضرب_V,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/sum/obj/v",      کا_UPARR_مجموع_OBJ_V,      1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/sum/prop/v",     کا_UPARR_مجموع_PROP_V,     2, {OA_نمبر,   OA_BIT1, OA_NONE} },
      { "%uparr/sum/r",          کا_UPARR_مجموع_REAL,       1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/sum/v",          کا_UPARR_مجموع_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/xor/v",          کا_UPARR_XOR_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%wait",   کا_WAIT,   1,  {OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%wait/fork",کا_WAIT_FORK,0,{OA_NONE,   OA_NONE,     OA_NONE} },
      { "%xnor",   کا_XNOR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xnor/r", کا_XNORR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor",    کا_XOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor/r",  کا_XORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { 0, کا_NOOP, 0, {OA_NONE, OA_NONE, OA_NONE} }
};

ساکن ثابت غیرعلامتی opcode_گنتی =
                    حجم(opcode_جدول)/حجم(*opcode_جدول) - 1;

ساکن صحیح opcode_compare(ثابت خالی*k, ثابت خالی*r)
{
      ثابت حرف*kp = static_cast<ثابت حرف*>(k);
      ثابت ساخت opcode_جدول_s*rp = static_cast<ثابت ساخت opcode_جدول_s*>(r);
      واپس strcmp(kp, rp->mnemonic);
}

/*
 * Keep a symbol table of addresses within code space. Labels on
 * executable opcodes are mapped to their address here.
 */
ساکن علامت_جدول_t sym_codespace = 0;

/*
 * Keep a symbol table of functors mentioned in the source. This table
 * is used to resolve references as they come.
 */
ساکن علامت_جدول_t sym_functors = 0;

/*
 * VPI objects are indexed during compile time so that they can be
 * linked together as they are created. This symbol table matches
 * labels to vpiHandles.
 */
ساکن علامت_جدول_t sym_vpi = 0;


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

خالی define_functor_علامت(ثابت حرف*label, vvp_net_t*net)
{
      علامت_قدر_t val;
      val.net = net;
      sym_سیٹ_قدر(sym_functors, label, val);
}

ساکن vvp_net_t*lookup_functor_علامت(ثابت حرف*label)
{
      أكد(sym_functors);
      علامت_قدر_t val = sym_get_قدر(sym_functors, label);
      واپس val.net;
}

vpiHandle vvp_lookup_handle(ثابت حرف*label)
{
      علامت_قدر_t val = sym_get_قدر(sym_vpi, label);
      اگر (val.ptr) واپس (vpiHandle) val.ptr;
      واپس 0;
}

vvp_net_t* vvp_net_lookup(ثابت حرف*label)
{
      ساکن منطقی t0_trigger_generated = جھوٹ;

        /* First, look to see if the symbol is a vpi object of some
	   sort. If it is, then get the vvp_ipoint_t pointer out of
	   the vpiHandle. */
      علامت_قدر_t val = sym_get_قدر(sym_vpi, label);
      اگر (val.ptr) {
	    vpiHandle vpi = static_cast<vpiHandle>(val.ptr);
	    انتخاب (vpi->get_type_رمز()) {
		حالت vpiNet:
		حالت vpiReg:
		حالت vpi_ثنائی_ہندسہ_Var:
		حالت vpiByteVar:
		حالت vpiShortIntVar:
		حالت vpiIntVar:
		حالت vpiLongIntVar:
		حالت vpiIntegerVar: {
		      ثابت __vpiSignal*sig = dynamic_cast<__vpiSignal*>(vpi);
		      واپس sig->گرہ;
		}

		حالت vpiRealVar: {
		      ثابت __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(vpi);
		      واپس sig->net;
		}

		حالت vpiStringVar:
		حالت vpiArrayVar:
		حالت vpiClassVar: {
		      ثابت __vpiBaseVar*sig = dynamic_cast<__vpiBaseVar*>(vpi);
		      واپس sig->get_net();
		}

		حالت vpiNamedEvent: {
		      ثابت __vpiNamedEvent*tmp = dynamic_cast<__vpiNamedEvent*>(vpi);
		      واپس tmp->funct;
		}

		طےشدہ:
		  fprintf(stderr, "Unsupported type %d.\n",
		          vpi->get_type_رمز());
		  أكد(0);
	    }
      }


	/* Failing that, look for a general functor. */
      vvp_net_t*tmp = lookup_functor_علامت(label);

	// This is a special label used to create a T0 trigger for the
	// always_comb/latch processes.
     اگر (! t0_trigger_generated && (strcmp(label, "E_0x0") == 0)) {
	      // This should never happen, but if it does then the E_0x0
	      // event generation may need to be explicitly generated in
	      // the compiler output instead of implicitly in this code.
	    أكد(! vpip_peek_current_scope()->is_automatic());
	    t0_trigger_generated = سچ;
	      // Create a local event with no name for the T0 trigger
	    compile_named_event(strdup(label), strcpy(نیا حرف [1],""), سچ);
	    tmp = vvp_net_lookup(label);
	    أكد(tmp);
	      // Create a trigger for the T0 event.
	    vvp_net_ptr_t ptr (tmp, 0);
	    جدول_وقت_t0_trigger(ptr);
     }

      واپس tmp;
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
ساکن resolv_list_s*resolv_list = 0;

resolv_list_s::~resolv_list_s()
{
      آزاد(label_);
}

خالی resolv_submit(resolv_list_s*cur)
{
      اگر (cur->resolve()) {
	    حذف cur;
	    واپس;
      }

      cur->اگلا_2 = resolv_list;
      resolv_list = cur;
}


/*
 * Look up vvp_nets in the symbol table. The "source" is the label for
 * the net that I want to feed, and net->port[port] is the vvp_net
 * input that I want that node to feed into. When the name is found,
 * put net->port[port] into the fan-out list for that node.
 */
ساخت vvp_net_resolv_list_s: عوامی resolv_list_s {

      صریح vvp_net_resolv_list_s(حرف*l) : resolv_list_s(l) { }
	// port to be driven by the located node.
      vvp_net_ptr_t پورٹ;
      مجازی منطقی resolve(منطقی mes) override;
};

منطقی vvp_net_resolv_list_s::resolve(منطقی mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      اگر (tmp) {
	      // Link the input port to the located output.
	    tmp->link(پورٹ);
	    واپس سچ;
      }

      اگر (mes)
	    fprintf(stderr, "unresolved vvp_net reference: %s\n", label());

      واپس جھوٹ;
}

اندرونی ساکن
خالی postpone_functor_داخلہ(vvp_net_ptr_t پورٹ, حرف*lab)
{
      ساخت vvp_net_resolv_list_s*res = نیا ساخت vvp_net_resolv_list_s(lab);
      res->پورٹ   = پورٹ;

      resolv_submit(res);
}


/*
 *  Generic functor reference lookup.
 */

ساخت functor_gen_resolv_list_s: عوامی resolv_list_s {
      صریح functor_gen_resolv_list_s(حرف*txt) : resolv_list_s(txt) {
	    ref = 0;
      }
      vvp_net_t**ref;
      مجازی منطقی resolve(منطقی mes) override;
};

منطقی functor_gen_resolv_list_s::resolve(منطقی mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      اگر (tmp) {
	    *ref = tmp;
	    واپس سچ;
      }

      اگر (mes)
	    fprintf(stderr, "unresolved functor reference: %s\n", label());

      واپس جھوٹ;
}

خالی functor_ref_lookup(vvp_net_t**ref, حرف*lab)
{
      ساخت functor_gen_resolv_list_s*res =
	    نیا ساخت functor_gen_resolv_list_s(lab);

      res->ref    = ref;

      resolv_submit(res);
}

/*
 *  vpiHandle lookup
 */

ساخت vpi_handle_resolv_list_s: عوامی resolv_list_s {
      صریح vpi_handle_resolv_list_s(حرف*lab) : resolv_list_s(lab) {
	    handle = تہی;
      }
      مجازی منطقی resolve(منطقی mes) override;
      vpiHandle *handle;
};

منطقی vpi_handle_resolv_list_s::resolve(منطقی mes)
{
      علامت_قدر_t val = sym_get_قدر(sym_vpi, label());
      اگر (!val.ptr) {
	    // check for thread access symbols
	    غیرعلامتی بنیاد, wid;
	    size_t n = 0;
	    حرف ss[32];
	    اگر (2 == لڑی_سے_پڑھو(label(), "W<%u,%31[r]>%zn", &بنیاد, ss, &n)
		       && n == لڑی_لمبائی(label())) {

		  val.ptr = vpip_make_vthr_لفظ(بنیاد, ss);
		  sym_سیٹ_قدر(sym_vpi, label(), val);

	    } ورنہ اگر (1 == لڑی_سے_پڑھو(label(), "S<%u,str>%zn", &بنیاد, &n)
		       && n == لڑی_لمبائی(label())) {

		  val.ptr = vpip_make_vthr_str_ڈھیر(بنیاد);
		  sym_سیٹ_قدر(sym_vpi, label(), val);

	    } ورنہ اگر (3 == لڑی_سے_پڑھو(label(), "S<%u,vec4,%31[su]%u>%zn", &بنیاد, ss, &wid, &n)
		       && n == لڑی_لمبائی(label())) {

		  منطقی signed_flag = جھوٹ;
		  برائے (حرف*fp = ss ;  *fp ;  fp += 1) انتخاب (*fp) {
		      حالت 's':
			signed_flag = سچ;
			توڑو;
		      حالت 'u':
			signed_flag = جھوٹ;
			توڑو;
		      طےشدہ:
			توڑو;
		  }
		  val.ptr = vpip_make_vthr_vec4_ڈھیر(بنیاد, signed_flag, wid);
		  sym_سیٹ_قدر(sym_vpi, label(), val);
	    }

      }

      اگر (!val.ptr) {
	    // check for memory word  M<mem,base,wid>
      }

      اگر (strcmp(label(), "null") == 0) {
	    val.ptr = vpip_make_null_const();
	    sym_سیٹ_قدر(sym_vpi, label(), val);
      }

      اگر (val.ptr) {
	    *handle = static_cast<vpiHandle>(val.ptr);
	    واپس سچ;
      }

      اگر (mes)
	    fprintf(stderr, "unresolved vpi name lookup: %s\n", label());

      واپس جھوٹ;
}

خالی compile_vpi_lookup(vpiHandle *handle, حرف*label)
{
      اگر (strcmp(label, "$time") == 0) {
	    *handle = vpip_sim_وقت(vpip_peek_current_scope(), جھوٹ);
	    آزاد(label);
	    واپس;
      }

      اگر (strcmp(label, "$stime") == 0) {
	    *handle = vpip_sim_وقت(vpip_peek_current_scope(), سچ);
	    آزاد(label);
	    واپس;
      }

      اگر (strcmp(label, "$realtime") == 0) {
	    *handle = vpip_sim_realtime(vpip_peek_current_scope());
	    آزاد(label);
	    واپس;
      }

      اگر (strcmp(label, "$simtime") == 0) {
	    *handle = vpip_sim_وقت(0, جھوٹ);
	    آزاد(label);
	    واپس;
      }

      ساخت vpi_handle_resolv_list_s*res
	    = نیا ساخت vpi_handle_resolv_list_s(label);

      res->handle = handle;
      resolv_submit(res);
}

/*
 * Code Label lookup
 */

ساخت رمز_label_resolv_list_s: عوامی resolv_list_s {
      صریح رمز_label_resolv_list_s(حرف*lab, منطقی cptr2) : resolv_list_s(lab) {
	    code = تہی;
	    cptr2_flag = cptr2;
      }
      ساخت vvp_رمز_s *code;
      منطقی cptr2_flag;
      مجازی منطقی resolve(منطقی mes) override;
};

منطقی رمز_label_resolv_list_s::resolve(منطقی mes)
{
      علامت_قدر_t val = sym_get_قدر(sym_codespace, label());
      اگر (val.ptr) {
	    اگر (cptr2_flag)
		  code->cptr2 = reinterpret_cast<vvp_رمز_t>(val.ptr);
	    ورنہ
		  code->cptr = reinterpret_cast<vvp_رمز_t>(val.ptr);
	    واپس سچ;
      }

      اگر (mes)
	    fprintf(stderr, "unresolved code label: %s\n", label());

      واپس جھوٹ;
}

خالی رمز_label_lookup(ساخت vvp_رمز_s *code, حرف *label, منطقی cptr2)
{
      ساخت رمز_label_resolv_list_s *res
	    = نیا ساخت رمز_label_resolv_list_s(label, cptr2);

      res->code  = code;

      resolv_submit(res);
}

ساخت رمز_array_resolv_list_s: عوامی resolv_list_s {
      صریح رمز_array_resolv_list_s(حرف*lab) : resolv_list_s(lab) {
	    code = تہی;
      }
      ساخت vvp_رمز_s *code;
      مجازی منطقی resolve(منطقی mes) override;
};

منطقی رمز_array_resolv_list_s::resolve(منطقی mes)
{
      code->array = array_ڈھونڈو(label());
      اگر (code->array != 0) {
	    واپس سچ;
      }

      اگر (mes)
	    fprintf(stderr, "Array unresolved: %s\n", label());

      واپس جھوٹ;
}

ساکن خالی compile_array_lookup(ساخت vvp_رمز_s*code, حرف*label)
{
      ساخت رمز_array_resolv_list_s *res
	    = نیا ساخت رمز_array_resolv_list_s(label);

      res->code  = code;

      resolv_submit(res);
}

ساکن std::list<ساخت __vpiSysTaskCall*> scheduled_compiletf;

خالی compile_compiletf(ساخت __vpiSysTaskCall*obj)
{
      اگر (obj->defn->info.compiletf == 0)
	    واپس;

      scheduled_compiletf.push_back(obj);
}

/*
 * When parsing is otherwise complete, this function is called to do
 * the final stuff. Clean up deferred linking here.
 */

خالی compile_cleanup(خالی)
{
      صحیح lnerrs = -1;
      صحیح nerrs = 0;
      صحیح آخری;

      اگر (verbose_flag) {
	    fprintf(stderr, " ... Linking\n");
	    fflush(stderr);
      }

      کرو {
	    resolv_list_s *res = resolv_list;
	    resolv_list = 0x0;
	    آخری = nerrs == lnerrs;
	    lnerrs = nerrs;
	    nerrs = 0;
	    جب (res) {
		  resolv_list_s *cur = res;
		  res = res->اگلا_2;
		  اگر (cur->resolve(آخری))
			حذف cur;
		  ورنہ {
			nerrs++;
			cur->اگلا_2 = resolv_list;
			resolv_list = cur;
		  }
	    }
	    اگر (nerrs && آخری)
		  fprintf(stderr,
			  "compile_cleanup: %d unresolved items\n",
			  nerrs);
      } جب (nerrs && !آخری);

      compile_errors += nerrs;

      اگر (verbose_flag) {
	    fprintf(stderr, " ... Removing symbol tables\n");
	    fflush(stderr);
      }

	/* After compile is complete, the vpi symbol table is no
	   longer needed. VPI objects are located by following
	   scopes. */
      delete_علامت_جدول(sym_vpi);
      sym_vpi = 0;

	/* Don't need the code labels. The instructions have numeric
	   pointers in them, the symbol table is no longer needed. */
      delete_علامت_جدول(sym_codespace);
      sym_codespace = 0;

      delete_علامت_جدول(sym_functors);
      sym_functors = 0;

      delete_udp_symbols();

      compile_island_cleanup();
      compile_array_cleanup();

      اگر (verbose_flag) {
	    fprintf(stderr, " ... Compiletf functions\n");
	    fflush(stderr);
      }

      أكد(vpi_منوال_flag == VPI_منوال_NONE);
      vpi_منوال_flag = VPI_منوال_COMPILETF;

      جب (! scheduled_compiletf.empty()) {
	    ساخت __vpiSysTaskCall*obj = scheduled_compiletf.front();
	    scheduled_compiletf.pop_front();
	    vpip_cur_task = obj;
	    obj->defn->info.compiletf (obj->defn->info.user_ڈیٹا);
	    vpip_cur_task = 0;
      }

      vpi_منوال_flag = VPI_منوال_NONE;
}

خالی compile_vpi_علامت(ثابت حرف*label, vpiHandle obj)
{
      علامت_قدر_t val;
      val.ptr = obj;
      sym_سیٹ_قدر(sym_vpi, label, val);
}

/*
 * Initialize the compiler by allocation empty symbol tables and
 * initializing the various address spaces.
 */
خالی compile_init(خالی)
{
      sym_vpi = new_علامت_جدول();

      sym_functors = new_علامت_جدول();

      sym_codespace = new_علامت_جدول();
      codespace_init();
}

خالی compile_بھرو_vpi_module(حرف*name)
{
      vpip_بھرو_module(name);
      حذف[] name;
}

خالی compile_vpi_وقت_precision(طویل pre)
{
      vpip_سیٹ_وقت_precision(pre);
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
ساکن منطقی crstring_سرخی_test(ثابت حرف*str)
{
      اگر ((str[0] != 'C') && (str[0] != 'c'))
	    واپس جھوٹ;
      اگر ((str[1] != 'r') || (str[2] != '<'))
	    واپس جھوٹ;

      واپس سچ;
}

منطقی crstring_test(ثابت حرف*str)
{
      اگر (!crstring_سرخی_test(str))
	    واپس جھوٹ;
      ثابت حرف*tp = strchr(str, '>');
      اگر (tp == 0) واپس جھوٹ;
      اگر (tp[1] != 0) واپس جھوٹ;

      اگر ((strspn(str+3, "0123456789abcdefmg")+3) != (size_t)(tp - str))
	  واپس جھوٹ;

      واپس سچ;
}

دوہرا crstring_مقصد_double(ثابت حرف*label)
{
      أكد(crstring_سرخی_test(label));

      ثابت حرف*cp = label+3;
      أكد(*cp == 'm');
      cp += 1;
      حرف*ep;
      uint64_t mant = strtoull(cp, &ep, 16);
      cp = ep;
      أكد(*cp == 'g');
      cp += 1;
      صحیح قوت_نما = strtoul(cp, 0, 16);

      دوہرا tmp;
      اگر (mant == 0 && قوت_نما == 0x3fff) {
	    tmp = INFINITY;
      } ورنہ اگر (mant == 0 && قوت_نما == 0x7fff) {
	    tmp = -INFINITY;
      } ورنہ اگر (قوت_نما == 0x3fff) {
	    tmp = nan("");
      } ورنہ {
	    دوہرا sign = (قوت_نما & 0x4000)? -1.0 : 1.0;
	    قوت_نما &= 0x1fff;

	    tmp = sign * ldexp((دوہرا)mant, قوت_نما - 0x1000);
      }

      واپس tmp;
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

خالی داخلہ_connect(vvp_net_t*fdx, غیرعلامتی پورٹ, حرف*label)
{
      vvp_net_ptr_t ifdx = vvp_net_ptr_t(fdx, پورٹ);

	/* Is this a vvp_vector4_t constant value? */
      اگر (c4string_test(label)) {

	    vvp_vector4_t tmp = c4string_مقصد_vector4(label);

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
	    اگر (label[0] == 'c')
		  جدول_وقت_init_سمتیہ(ifdx, tmp);
	    ورنہ
		  جدول_وقت_سیٹ_سمتیہ(ifdx, tmp);

	    آزاد(label);
	    واپس;
      }

	/* Is this a vvp_vector8_t constant value? */
      اگر (c8string_test(label)) {

	    vvp_vector8_t tmp = c8string_مقصد_vector8(label);
	    اگر (label[0] == 'c')
		  جدول_وقت_init_سمتیہ(ifdx, tmp);
	    ورنہ
		  جدول_وقت_سیٹ_سمتیہ(ifdx, tmp);

	    آزاد(label);
	    واپس;
      }

	/* Handle the Cr<> constant driver, which is a real-value
	   driver. */
      اگر (crstring_test(label)) {

	    دوہرا tmp = crstring_مقصد_double(label);

	    اگر (label[0] == 'c')
		  جدول_وقت_init_سمتیہ(ifdx, tmp);
	    ورنہ
		  جدول_وقت_سیٹ_سمتیہ(ifdx, tmp);
	    آزاد(label);
	    واپس;
      }

	/* Handle the general case that this is a label for a node in
	   the vvp net. This arranges for the label to be preserved in
	   a linker list, and linked when the symbol table is
	   complete. */
      postpone_functor_داخلہ(ifdx, label);
}

خالی inputs_connect(vvp_net_t*fdx, غیرعلامتی argc, ساخت symb_s*argv)
{
      اگر (argc > 4) {
	    std::cerr << "XXXX argv[0] = " << argv[0].متن << std::سطر_ختم;
      }
      أكد(argc <= 4);

      برائے (غیرعلامتی idx = 0;  idx < argc;  idx += 1) {

	    داخلہ_connect(fdx, idx, argv[idx].متن);
      }
}

خالی wide_inputs_connect(vvp_wide_fun_core*core,
			 غیرعلامتی argc, ساخت symb_s*argv)
{
	/* Create input functors to receive values from the
	   network. These functors pass the data to the core. */
      غیرعلامتی داخلہ_functors = (argc+3) / 4;
      برائے (غیرعلامتی idx = 0 ;  idx < داخلہ_functors ;  idx += 1) {
	    غیرعلامتی بنیاد = idx*4;
	    غیرعلامتی trans = 4;
	    اگر (بنیاد+trans > argc)
		  trans = argc - بنیاد;

	    vvp_wide_fun_t*cur = نیا vvp_wide_fun_t(core, بنیاد);
	    vvp_net_t*ptr = نیا vvp_net_t;
	    ptr->fun = cur;

	    inputs_connect(ptr, trans, argv+بنیاد);
      }
}

سانچہ <جماعت T_> خالی make_arith(T_ *arith, حرف*label,
				    غیرعلامتی argc, ساخت symb_s*argv)
{
      vvp_net_t* ptr = نیا vvp_net_t;
      ptr->fun = arith;

      define_functor_علامت(label, ptr);
      آزاد(label);

      أكد(argc == 2);
      inputs_connect(ptr, argc, argv);

      آزاد(argv);
}

خالی compile_arith_cast_int(حرف*label, طویل width,
                            غیرعلامتی argc, ساخت symb_s*argv)
{
      vvp_arith_cast_int*arith = نیا vvp_arith_cast_int((غیرعلامتی) width);

      vvp_net_t* ptr = نیا vvp_net_t;
      ptr->fun = arith;

      define_functor_علامت(label, ptr);
      آزاد(label);

      أكد(argc == 1);
      inputs_connect(ptr, argc, argv);
      آزاد(argv);
}

خالی compile_arith_cast_vec2(حرف*label, طویل width,
			     غیرعلامتی argc, ساخت symb_s*argv)
{
      vvp_arith_cast_vec2*arith = نیا vvp_arith_cast_vec2((غیرعلامتی) width);

      vvp_net_t* ptr = نیا vvp_net_t;
      ptr->fun = arith;

      define_functor_علامت(label, ptr);
      آزاد(label);

      أكد(argc == 1);
      inputs_connect(ptr, argc, argv);
      آزاد(argv);
}

خالی compile_arith_cast_real(حرف*label, منطقی signed_flag,
                             غیرعلامتی argc, ساخت symb_s*argv)
{
      vvp_arith_cast_real*arith = نیا vvp_arith_cast_real(signed_flag);

      vvp_net_t* ptr = نیا vvp_net_t;
      ptr->fun = arith;

      define_functor_علامت(label, ptr);
      آزاد(label);

      أكد(argc == 1);
      inputs_connect(ptr, argc, argv);
      آزاد(argv);
}

خالی compile_arith_abs(حرف*label, غیرعلامتی argc, ساخت symb_s*argv)
{
      vvp_arith_abs*arith = نیا vvp_arith_abs;

      vvp_net_t* ptr = نیا vvp_net_t;
      ptr->fun = arith;

      define_functor_علامت(label, ptr);
      آزاد(label);

      أكد(argc == 1);
      inputs_connect(ptr, argc, argv);
      آزاد(argv);
}

خالی compile_arith_div(حرف*label, طویل wid, منطقی signed_flag,
		       غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    ثابت حرف *suffix = "";
	    اگر (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s; .arith/div%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_arith_div(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

خالی compile_arith_div_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv)
{
      اگر (argc != 2) {
	    fprintf(stderr, "%s; .arith/divr has wrong number of symbols\n", label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_real_ *arith = نیا vvp_arith_div_real;
      make_arith(arith, label, argc, argv);
}

خالی compile_arith_mod(حرف*label, طویل wid, منطقی signed_flag,
		       غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    fprintf(stderr, "%s .arith/mod has wrong number of symbols\n", label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_arith_mod(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

خالی compile_arith_mod_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv)
{
      اگر (argc != 2) {
	    fprintf(stderr, "%s .arith/mod.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_real_ *arith = نیا vvp_arith_mod_real;
      make_arith(arith, label, argc, argv);
}

خالی compile_arith_mult(حرف*label, طویل wid,
			غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    fprintf(stderr, "%s .arith/mult has wrong number of symbols\n", label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_arith_mult(wid);
      make_arith(arith, label, argc, argv);
}

خالی compile_arith_mult_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv)
{
      اگر (argc != 2) {
	    fprintf(stderr, "%s .arith/mult.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_real_ *arith = نیا vvp_arith_mult_real;
      make_arith(arith, label, argc, argv);
}


خالی compile_arith_pow(حرف*label, طویل wid, منطقی signed_flag,
		       غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    ثابت حرف *suffix = "";
	    اگر (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s .arith/pow%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_arith_pow(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

خالی compile_arith_pow_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv)
{
      اگر (argc != 2) {
	    fprintf(stderr, "%s .arith/pow.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_real_ *arith = نیا vvp_arith_pow_real;
      make_arith(arith, label, argc, argv);
}

خالی compile_arith_sub(حرف*label, طویل wid, غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    fprintf(stderr, "%s .arith/sub has wrong number of symbols\n", label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_arith_sub(wid);
      make_arith(arith, label, argc, argv);
}

خالی compile_arith_sub_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv)
{
      اگر (argc != 2) {
	    fprintf(stderr, "%s; .arith/sub.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_real_ *arith = نیا vvp_arith_sub_real;
      make_arith(arith, label, argc, argv);
}

خالی compile_arith_مجموع(حرف*label, طویل wid, غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    fprintf(stderr, "%s .arith/sum has wrong number of symbols\n", label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_arith_مجموع(wid);
      make_arith(arith, label, argc, argv);
}

خالی compile_arith_مجموع_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv)
{
      اگر (argc != 2) {
	    fprintf(stderr, "%s .arith/sum.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_real_ *arith = نیا vvp_arith_مجموع_real;
      make_arith(arith, label, argc, argv);
}

خالی compile_cmp_eeq(حرف*label, طویل wid,
		     غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_cmp_eeq(wid);

      make_arith(arith, label, argc, argv);
}

خالی compile_cmp_nee(حرف*label, طویل wid,
		     غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_cmp_nee(wid);

      make_arith(arith, label, argc, argv);
}

خالی compile_cmp_eq(حرف*label, طویل wid, غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_cmp_eq(wid);
      make_arith(arith, label, argc, argv);
}

خالی compile_cmp_eqx(حرف*label, طویل wid, غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqx has wrong number of symbols\n",label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_cmp_eqx(wid);
      make_arith(arith, label, argc, argv);
}

خالی compile_cmp_eqz(حرف*label, طویل wid, غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqz has wrong number of symbols\n",label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_cmp_eqz(wid);
      make_arith(arith, label, argc, argv);
}

خالی compile_cmp_eq_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv)
{
      اگر (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_real_ *arith = نیا vvp_cmp_eq_real;
      make_arith(arith, label, argc, argv);
}

خالی compile_cmp_ne(حرف*label, طویل wid, غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_cmp_ne(wid);
      make_arith(arith, label, argc, argv);
}

خالی compile_cmp_ne_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv)
{
      اگر (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_real_ *arith = نیا vvp_cmp_ne_real;
      make_arith(arith, label, argc, argv);
}

خالی compile_cmp_ge(حرف*label, طویل wid, منطقی signed_flag,
		    غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge has wrong number of symbols\n", label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_cmp_ge(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

خالی compile_cmp_ge_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv)
{
      اگر (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_real_ *arith = نیا vvp_cmp_ge_real;
      make_arith(arith, label, argc, argv);
}

خالی compile_cmp_gt(حرف*label, طویل wid, منطقی signed_flag,
		    غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt has wrong number of symbols\n", label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_cmp_gt(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

خالی compile_cmp_gt_r(حرف*label, غیرعلامتی argc, ساخت symb_s*argv)
{
      اگر (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_real_ *arith = نیا vvp_cmp_gt_real;
      make_arith(arith, label, argc, argv);
}

خالی compile_cmp_weq(حرف*label, طویل wid,
		     غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    fprintf(stderr, "%s .cmp/weq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_cmp_weq(wid);

      make_arith(arith, label, argc, argv);
}

خالی compile_cmp_wne(حرف*label, طویل wid,
		     غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      اگر (argc != 2) {
	    fprintf(stderr, "%s .cmp/wne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    واپس;
      }

      vvp_arith_ *arith = نیا vvp_cmp_wne(wid);

      make_arith(arith, label, argc, argv);
}


خالی compile_delay(حرف*label, غیرعلامتی width,
                   vvp_delay_t*delay, ساخت symb_s arg)
{
      vvp_net_t*net = نیا vvp_net_t;
      vvp_fun_delay*obj = نیا vvp_fun_delay(net, width, *delay);
      net->fun = obj;

      حذف delay;

      داخلہ_connect(net, 0, arg.متن);

      define_functor_علامت(label, net);
      آزاد(label);
}

خالی compile_delay(حرف*label, غیرعلامتی width,
                   غیرعلامتی argc, ساخت symb_s*argv, منطقی ignore_decay)
{
      vvp_delay_t stub (0, 0, 0);
      اگر (ignore_decay) stub.سیٹ_ignore_decay();
      vvp_net_t*net = نیا vvp_net_t;
      vvp_fun_delay*obj = نیا vvp_fun_delay(net, width, stub);
      net->fun = obj;

      inputs_connect(net, argc, argv);
      آزاد(argv);

      define_functor_علامت(label, net);
      آزاد(label);
}

/*
 * Extend nodes.
 */
خالی compile_extend_signed(حرف*label, طویل wid, ساخت symb_s arg)
{
      أكد(wid >= 0);

      vvp_fun_extend_signed*fun = نیا vvp_fun_extend_signed(wid);
      vvp_net_t*ptr = نیا vvp_net_t;
      ptr->fun = fun;

      define_functor_علامت(label, ptr);
      آزاد(label);

      داخلہ_connect(ptr, 0, arg.متن);
}

ساخت __vpiModPath* compile_modpath(حرف*label, غیرعلامتی width,
                                     ساخت symb_s drv, ساخت symb_s dest)
{
      vvp_net_t*net = نیا vvp_net_t;
      vvp_fun_modpath*obj = نیا vvp_fun_modpath(net, width);
      net->fun = obj;

      داخلہ_connect(net, 0, drv.متن);

      define_functor_علامت(label, net);

      __vpiModPath*modpath = vpip_make_modpath(net);

      compile_vpi_lookup(&modpath->راستہ_رکن_برآمد.اظہاریہ, dest.متن);

      آزاد(label);

      modpath->modpath = obj;
      واپس modpath;
}

ساکن ساخت __vpiModPathSrc*make_modpath_src(ساخت __vpiModPath*راستہ,
					       حرف کنارہ_c,
					       ثابت ساخت symb_s&src,
					       ساخت numbv_s&vals,
					       منطقی ifnone)
{
      vvp_fun_modpath*dst = راستہ->modpath;

      vvp_time64_t use_delay[12];
      أكد(vals.cnt == 12);
      برائے (غیرعلامتی idx = 0 ; idx < vals.cnt ;  idx += 1) {
	    use_delay[idx] = vals.nvec[idx];
      }

      numbv_clear(&vals);

      vvp_fun_modpath_src*obj = 0;

      صحیح vpi_کنارہ = vpi_No_کنارہ;
      اگر (کنارہ_c == 0) {
	    obj = نیا vvp_fun_modpath_src(use_delay);

      } ورنہ {
	    منطقی posedge, negedge;
	    انتخاب (کنارہ_c) {
		حالت '+':
		  vpi_کنارہ = vpiPosedge;
		  posedge = سچ;
		  negedge = جھوٹ;
		  توڑو;
		حالت '-':
		  vpi_کنارہ = vpiNegedge;
		  posedge = جھوٹ;
		  negedge = سچ;
		  توڑو;
#اگر 0
		حالت '*':
		  posedge = سچ;
		  negedge = سچ;
		  توڑو;
#اگر_ختم
		طےشدہ:
		  posedge = جھوٹ;
		  negedge = جھوٹ;
		  fprintf(stderr, "Unknown edge identifier %c(%d).\n", کنارہ_c,
		          کنارہ_c);
		  أكد(0);
	    }
	    obj = نیا vvp_fun_modpath_کنارہ(use_delay, posedge, negedge);
      }

      vvp_net_t*net = نیا vvp_net_t;
      ساخت __vpiModPathSrc* srcobj = vpip_make_modpath_src(راستہ, net) ;
      vpip_attach_مقصد_current_scope(srcobj);
      net->fun = obj;

	/* Save the vpiEdge directory into the input path term. */
      srcobj->راستہ_رکن_اندر.کنارہ = vpi_کنارہ;
      داخلہ_connect(net, 0, src.متن);
      dst->جوڑو_modpath_src(obj, ifnone);

      واپس srcobj;
}

خالی compile_modpath_src(ساخت __vpiModPath*dst, حرف کنارہ_c,
			 ثابت ساخت symb_s&src,
			 ساخت numbv_s&vals,
			 ثابت ساخت symb_s&condit_src,
			 ثابت ساخت symb_s&راستہ_رکن_اندر)
{
      ساخت __vpiModPathSrc*obj =
	    make_modpath_src(dst, کنارہ_c, src, vals, جھوٹ);
      داخلہ_connect(obj->net, 1, condit_src.متن);
      compile_vpi_lookup(&obj->راستہ_رکن_اندر.اظہاریہ, راستہ_رکن_اندر.متن);
}

خالی compile_modpath_src(ساخت __vpiModPath*dst, حرف کنارہ_c,
			 ثابت ساخت symb_s&src,
			 ساخت numbv_s&vals,
			 صحیح condit_src,
			 ثابت ساخت symb_s&راستہ_رکن_اندر,
			 منطقی ifnone)
{
      أكد(condit_src == 0);
      ساخت __vpiModPathSrc*obj =
	    make_modpath_src(dst, کنارہ_c, src, vals, ifnone);
      compile_vpi_lookup(&obj->راستہ_رکن_اندر.اظہاریہ, راستہ_رکن_اندر.متن);
}

/*
 * A .shift/l statement creates an array of functors for the
 * width. The 0 input is the data vector to be shifted and the 1 input
 * is the amount of the shift. An unconnected shift amount is set to 0.
 */
خالی compile_shiftl(حرف*label, طویل wid, غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      vvp_arith_ *arith = نیا vvp_shiftl(wid);
      make_arith(arith, label, argc, argv);
}

خالی compile_shiftr(حرف*label, طویل wid, منطقی signed_flag,
		    غیرعلامتی argc, ساخت symb_s*argv)
{
      أكد( wid > 0 );

      vvp_arith_ *arith = نیا vvp_shiftr(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

خالی compile_resolver(حرف*label, حرف*type, غیرعلامتی argc, ساخت symb_s*argv)
{
      vvp_net_t*net = نیا vvp_net_t;

      resolv_core*core = 0;

      اگر (strcmp(type,"tri") == 0) {
	    core = نیا resolv_tri(argc, net, vvp_scalar_t(BIT4_Z, 0,0));

      } ورنہ اگر (strcmp(type,"tri0") == 0) {
	    core = نیا resolv_tri(argc, net, vvp_scalar_t(BIT4_0, 5,5));

      } ورنہ اگر (strcmp(type,"tri1") == 0) {
	    core = نیا resolv_tri(argc, net, vvp_scalar_t(BIT4_1, 5,5));

      } ورنہ اگر (strcmp(type,"triand") == 0) {
	    core = نیا resolv_triand(argc, net);

      } ورنہ اگر (strcmp(type,"trior") == 0) {
	    core = نیا resolv_trior(argc, net);

      } ورنہ {
	    fprintf(stderr, "invalid resolver type: %s\n", type);
	    compile_errors += 1;
            حذف net;
      }

      اگر (core) {
	    net->fun = core;
	    define_functor_علامت(label, net);

            برائے (غیرعلامتی بنیاد = 0 ;  بنیاد < argc ;  بنیاد += 4) {
	          غیرعلامتی nports = argc - بنیاد;
                  اگر (nports > 4)
                        nports = 4;

                  اگر (بنیاد > 0) {
                        net = نیا vvp_net_t;
                        net->fun = نیا resolv_extend(core, بنیاد);
                  }
                  inputs_connect(net, nports, argv+بنیاد);
            }
      }
      آزاد(type);
      آزاد(label);
      آزاد(argv);
}

خالی compile_udp_def(صحیح sequ, حرف *label, حرف *name,
		     غیرعلامتی nin, غیرعلامتی init, حرف **جدول)
{
      اگر (sequ) {
	    vvp_bit4_t init4;
	    اگر (init == 0)
		  init4 = BIT4_0;
	    ورنہ اگر (init == 1)
		  init4 = BIT4_1;
	    ورنہ
		  init4 = BIT4_X;

	    vvp_udp_seq_s *u = نیا vvp_udp_seq_s(label, name, nin, init4);
	    u->compile_جدول(جدول);
      } ورنہ {
	    vvp_udp_comb_s *u = نیا vvp_udp_comb_s(label, name, nin);
	    u->compile_جدول(جدول);
      }
      آزاد(label);
}

حرف **compile_udp_جدول(حرف **جدول, حرف *صف)
{
  اگر (جدول)
    أكد(لڑی_لمبائی(*جدول)==لڑی_لمبائی(صف));

  حرف **tt;
  برائے (tt = جدول; tt && *tt; tt++) { }
  صحیح n = (tt-جدول) + 2;

  جدول = static_cast<حرف**>(realloc(جدول, n*حجم(حرف*)));
  جدول[n-2] = صف;
  جدول[n-1] = 0x0;

  واپس جدول;
}


/*
 * The parser uses this function to compile and link an executable
 * opcode. I do this by looking up the opcode in the opcode_table. The
 * table gives the operand structure that is acceptable, so I can
 * process the operands here as well.
 */
خالی compile_رمز(حرف*label, حرف*mnem, comp_operands_t opa)
{
	/* First, I can give the label a value that is the current
	   codespace pointer. Don't need the text of the label after
	   this is done. */
      اگر (label)
	    compile_codelabel(label);

	/* Lookup the opcode in the opcode table. */
      ساخت opcode_جدول_s*op = static_cast<ساخت opcode_جدول_s*>
                                            (bsearch(mnem, opcode_جدول, opcode_گنتی,
                                                     حجم(ساخت opcode_جدول_s),
                                                     &opcode_compare));
      اگر (op == 0) {
	    yyerror("Invalid opcode");
	    compile_errors += 1;
	    واپس;
      }

      أكد(op);

	/* Build up the code from the information about the opcode and
	   the information from the compiler. */
      vvp_رمز_t code = codespace_allocate();
      code->opcode = op->opcode;

      اگر (op->argc != (opa? opa->argc : 0)) {
	    yyerror("operand count");
	    compile_errors += 1;
	    واپس;
      }

	/* Pull the operands that the instruction expects from the
	   list that the parser supplied. */

      برائے (غیرعلامتی idx = 0 ;  idx < op->argc ;  idx += 1) {

	    انتخاب (op->argt[idx]) {
		حالت OA_NONE:
		  توڑو;

		حالت OA_ARR_PTR:
		  اگر (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			توڑو;
		  }

		  compile_array_lookup(code, opa->argv[idx].symb.متن);
		  توڑو;

		حالت OA_BIT1:
		  اگر (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			توڑو;
		  }
		  code->ثنائی_ہندسہ_idx[0] = opa->argv[idx].numb;
		  توڑو;

		حالت OA_BIT2:
		  اگر (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			توڑو;
		  }
		  code->ثنائی_ہندسہ_idx[1] = opa->argv[idx].numb;
		  توڑو;

		حالت OA_رمز_PTR:
		حالت OA_رمز_PTR2:
		  اگر (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			توڑو;
		  }

		  أكد(opa->argv[idx].symb.idx == 0);
		  رمز_label_lookup(code, opa->argv[idx].symb.متن,
				    op->argt[idx]==OA_رمز_PTR2);
		  توڑو;

		حالت OA_FUNC_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  اگر (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			توڑو;
		  }

		  functor_ref_lookup(&code->net, opa->argv[idx].symb.متن);
		  توڑو;

		حالت OA_FUNC_PTR2:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  اگر (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			توڑو;
		  }

		  functor_ref_lookup(&code->net2, opa->argv[idx].symb.متن);
		  توڑو;

		حالت OA_نمبر:
		  اگر (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			توڑو;
		  }

		  code->نمبر = opa->argv[idx].numb;
		  توڑو;

		حالت OA_VPI_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  اگر (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			توڑو;
		  }

		  compile_vpi_lookup(&code->handle, opa->argv[idx].symb.متن);
		  توڑو;

		حالت OA_STRING:
		  اگر (opa->argv[idx].ltype != L_STRING) {
			yyerror("operand format");
			توڑو;
		  }

		  code->متن = opa->argv[idx].متن;
		  توڑو;
	    }
      }

      آزاد(opa);

      آزاد(mnem);
}

خالی compile_codelabel(حرف*label)
{
      علامت_قدر_t val;
      vvp_رمز_t ptr = codespace_اگلا();

      val.ptr = ptr;
      sym_سیٹ_قدر(sym_codespace, label, val);

      آزاد(label);
}

خالی compile_file_سطر(حرف*label, طویل file_idx, طویل lineno,
                       حرف*description)
{
      اگر (label) compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_رمز_t code = codespace_allocate();
      code->opcode = &کا_FILE_سطر;

	/* Create a vpiHandle that contains the information. */
      code->handle = vpip_build_file_سطر(description, file_idx, lineno);
      أكد(code->handle);

	/* Done with the lexor-allocated name string. */
      حذف[] description;
}

خالی compile_vpi_call(حرف*label, حرف*name,
                      منطقی func_as_task_err, منطقی func_as_task_warn,
                      طویل file_idx, طویل lineno,
                      غیرعلامتی argc, vpiHandle*argv,
		      غیرعلامتی vec4_ڈھیر, غیرعلامتی real_ڈھیر, غیرعلامتی string_ڈھیر)
{
      اگر (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_رمز_t code = codespace_allocate();
      code->opcode = &کا_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, 0, 0,
					 0, func_as_task_err, func_as_task_warn,
                                         argc, argv, vec4_ڈھیر, real_ڈھیر, string_ڈھیر,
					 file_idx, lineno);
      اگر (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      حذف[] name;
}

خالی compile_vpi_func_call(حرف*label, حرف*name,
			   صحیح val_type, غیرعلامتی val_wid,
			   طویل file_idx, طویل lineno,
			   غیرعلامتی argc, vpiHandle*argv,
			   غیرعلامتی vec4_ڈھیر,
			   غیرعلامتی real_ڈھیر,
			   غیرعلامتی string_ڈھیر)
{
      اگر (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_رمز_t code = codespace_allocate();
      code->opcode = &کا_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, val_type, val_wid,
					 0, سچ, جھوٹ,
                                         argc, argv, vec4_ڈھیر, real_ڈھیر, string_ڈھیر,
					 file_idx, lineno);
      اگر (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      حذف[] name;
}

/*
 * When the parser finds a thread statement, I create a new thread
 * with the start address referenced by the program symbol passed to
 * me.
 */
خالی compile_thread(حرف*آغاز_sym, حرف*flag)
{
      منطقی ڈالو_flag = جھوٹ;

      علامت_قدر_t tmp = sym_get_قدر(sym_codespace, آغاز_sym);
      vvp_رمز_t pc = reinterpret_cast<vvp_رمز_t>(tmp.ptr);
      اگر (pc == 0) {
	    yyerror("unresolved address");
	    واپس;
      }

      اگر (flag && (strcmp(flag,"$push") == 0))
	    ڈالو_flag = سچ;

      vthread_t thr = vthread_new(pc, vpip_peek_current_scope());

      اگر (flag && (strcmp(flag,"$init") == 0))
	    جدول_وقت_init_vthread(thr);
      ورنہ اگر (flag && (strcmp(flag,"$final") == 0))
	    جدول_وقت_final_vthread(thr);
      ورنہ
	    جدول_وقت_vthread(thr, 0, ڈالو_flag);

      آزاد(آغاز_sym);
      آزاد(flag);
}

خالی compile_param_logic(حرف*label, ثابت حرف*name, حرف*value, منطقی signed_flag,
                         منطقی local_flag,
                         طویل file_idx, طویل lineno)
{
      vvp_vector4_t value4 = c4string_مقصد_vector4(value);
      vpiHandle obj = vpip_make_ثنائی_param(name, value4, signed_flag,
                                             local_flag, file_idx, lineno);
      compile_vpi_علامت(label, obj);
      vpip_attach_مقصد_current_scope(obj);

      آزاد(label);
      آزاد(value);
}

خالی compile_param_string(حرف*label, ثابت حرف*name, حرف*value,
                          منطقی local_flag,
                          طویل file_idx, طویل lineno)
{
	// name and value become owned by vpip_make_string_param
      vpiHandle obj = vpip_make_string_param(name, value, local_flag, file_idx, lineno);
      compile_vpi_علامت(label, obj);
      vpip_attach_مقصد_current_scope(obj);

      آزاد(label);
}

خالی compile_param_real(حرف*label, ثابت حرف*name, حرف*value,
                        منطقی local_flag,
                        طویل file_idx, طویل lineno)
{
      دوہرا dvalue = crstring_مقصد_double(value);
      vpiHandle obj = vpip_make_real_param(name, dvalue, local_flag, file_idx, lineno);
      compile_vpi_علامت(label, obj);
      vpip_attach_مقصد_current_scope(obj);

      آزاد(label);
      آزاد(value);
}

خالی compile_island(حرف*label, حرف*type)
{
      اگر (strcmp(type,"tran") == 0)
	    compile_island_tran(label);
      ورنہ
	    أكد(0);

      آزاد(type);
}
