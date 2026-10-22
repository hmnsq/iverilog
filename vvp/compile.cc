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

#اگر_تعریف_شده __MINGW32__
#ضمن <windows.h>
#پایان_اگر

# include  "ivl_alloc.h"

بیعلامت compile_errors = 0;

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

تعداد عملوند_e {
	/* Place holder for unused operand */
      OA_NONE,
	/* The operand is a number, an immediate unsigned integer */
      OA_عدد,
	/* The operand is a pointer to an array. */
      OA_ARR_PTR,
	/* The operand is a thread bit index or short integer */
      OA_BIT1,
      OA_BIT2,
	/* The operand is a pointer to code space */
      OA_کد_PTR,
      OA_کد_PTR2,
	/* The operand is a variable or net pointer */
      OA_FUNC_PTR,
	/* The operand is a second functor pointer */
      OA_FUNC_PTR2,
	/* The operand is a VPI handle */
      OA_VPI_PTR,
	/* String */
      OA_STRING
};

بنية opcode_جدول_s {
      ثابت حرف*mnemonic;
      vvp_کد_fun opcode;

      بیعلامت argc;
      تعداد عملوند_e argt[عملوند_MAX];
};

ساكن ثابت بنية opcode_جدول_s opcode_جدول[] = {
      { "%abs/wr", از_ABS_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add",    از_جمع_کن,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add/wr", از_جمع_کن_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%addi",   از_ADDI,   3,  {OA_BIT1,     OA_BIT2,     OA_عدد} },
      { "%alloc",  از_ALLOC,  1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%and",    از_AND,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%and/r",  از_ANDR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%assign/ar",از_تخصیص_بده_AR,2,{OA_ARR_PTR,OA_BIT1,     OA_NONE} },
      { "%assign/ar/d",از_تخصیص_بده_ARD,2,{OA_ARR_PTR,OA_BIT1,  OA_NONE} },
      { "%assign/ar/e",از_تخصیص_بده_ARE,1,{OA_ARR_PTR,OA_NONE,  OA_NONE} },
      { "%assign/vec4",      از_تخصیص_بده_VEC4,       2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/a/d",  از_تخصیص_بده_VEC4_A_D,   3, {OA_ARR_PTR,  OA_BIT1, OA_BIT2} },
      { "%assign/vec4/a/e",  از_تخصیص_بده_VEC4_A_E,   2, {OA_ARR_PTR,  OA_BIT1, OA_NONE} },
      { "%assign/vec4/d",    از_تخصیص_بده_VEC4_D,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/e",    از_تخصیص_بده_VEC4_E,      1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%assign/vec4/off/d",از_تخصیص_بده_VEC4_OFF_D, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%assign/vec4/off/e",از_تخصیص_بده_VEC4_OFF_E, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr",  از_تخصیص_بده_WR, 2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/d",از_تخصیص_بده_WRD,2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/e",از_تخصیص_بده_WRE,1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%blend",     از_BLEND,     0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/str", از_BLEND_STR, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/wr",  از_BLEND_WR,  0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%breakpoint", از_BREAKPOINT, 0,  {OA_NONE, OA_NONE, OA_NONE} },
      { "%callf/obj",       از_CALLF_OBJ,       2,{OA_کد_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/real",      از_CALLF_REAL,      2,{OA_کد_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/str",       از_CALLF_STR,       2,{OA_کد_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/vec4",      از_CALLF_VEC4,      2,{OA_کد_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/void",      از_CALLF_VOID,      2,{OA_کد_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%cassign/link",    از_CASSIGN_LINK,    2,{OA_FUNC_PTR,OA_FUNC_PTR2,OA_NONE} },
      { "%cassign/vec4",    از_CASSIGN_VEC4,    1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cassign/vec4/off",از_CASSIGN_VEC4_OFF,2,{OA_FUNC_PTR,OA_BIT1,     OA_NONE} },
      { "%cassign/wr",  از_CASSIGN_WR,  1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cast/vec2/dar", از_CAST_VEC2_DAR, 1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/dar", از_CAST_VEC4_DAR, 1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/str", از_CAST_VEC4_STR, 1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%cast2",   از_CAST2,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/e",   از_CMPE,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/ne",  از_CMPNE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/s",   از_CMPS,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/str", از_CMPSTR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/u",   از_CMPU,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/we",  از_CMPWE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wne", از_CMPWNE, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wr",  از_CMPWR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/x",   از_CMPX,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/z",   از_CMPZ,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmpi/e",  از_CMPIE,  3,  {OA_BIT1,     OA_BIT2,     OA_عدد} },
      { "%cmpi/ne", از_CMPINE, 3,  {OA_BIT1,     OA_BIT2,     OA_عدد} },
      { "%cmpi/s",  از_CMPIS,  3,  {OA_BIT1,     OA_BIT2,     OA_عدد} },
      { "%cmpi/u",  از_CMPIU,  3,  {OA_BIT1,     OA_BIT2,     OA_عدد} },
      { "%cmpix/ltu", از_CMPIX_LTU, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%cmpix/slt0", از_CMPIX_SLT0, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%concat/str",  از_CONCAT_STR,  0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concat/vec4", از_CONCAT_VEC4, 0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concati/str", از_CONCATI_STR, 1,{OA_STRING,OA_NONE,  OA_NONE} },
      { "%concati/vec4",از_CONCATI_VEC4,3,{OA_BIT1,  OA_BIT2,  OA_عدد} },
      { "%cvt/rv",   از_CVT_RV,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/rv/s", از_CVT_RV_S,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/sr", از_CVT_SR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/ur", از_CVT_UR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/vr", از_CVT_VR, 1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%deassign",از_DEASSIGN,3,{OA_FUNC_PTR, OA_BIT1,     OA_BIT2} },
      { "%deassign/wr",از_DEASSIGN_WR,1,{OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%debug/thr",  از_DEBUG_THR,  1,{OA_STRING,   OA_NONE,     OA_NONE} },
      { "%delay",  از_DELAY,  2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%delayx", از_DELAYX, 1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%delete/elem",از_DELETE_ELEM,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%delete/obj",از_DELETE_OBJ,1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%delete/prop/elem", از_DELETE_PROP_ELEM, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%delete/prop/obj",  از_DELETE_PROP_OBJ,  1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%delete/tail",از_DELETE_دم,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%disable",  از_DISABLE, 1, {OA_VPI_PTR,OA_NONE,     OA_NONE} },
      { "%disable/flow", از_DISABLE_FLOW, 1, {OA_VPI_PTR,OA_NONE, OA_NONE} },
      { "%disable/fork",از_DISABLE_FORK,0,{OA_NONE,OA_NONE,  OA_NONE} },
      { "%div",      از_DIV,     0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/s",    از_DIV_S,   0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/wr",   از_DIV_WR,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/obj",  از_DUP_OBJ, 0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/real", از_DUP_REAL,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/vec4", از_DUP_VEC4,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%end",    از_انتها,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl",  از_EVCTL,  2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/c",از_EVCTLC, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl/i",از_EVCTLI, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/s",از_EVCTLS, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%event",    از_EVENT,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%event/nb", از_EVENT_NB, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%flag_get/vec4", از_FLAG_GET_VEC4, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%flag_inv",      از_FLAG_INV,      1, {OA_BIT1,   OA_NONE, OA_NONE} },
      { "%flag_mov",      از_FLAG_MOV,      2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_or",       از_FLAG_OR,       2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_set/imm",  از_FLAG_مجموعه_IMM,  2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%flag_set/vec4", از_FLAG_مجموعه_VEC4, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%force/link",    از_FORCE_LINK,2,{OA_FUNC_PTR, OA_FUNC_PTR2, OA_NONE} },
      { "%force/vec4",    از_FORCE_VEC4,    1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%force/vec4/off",از_FORCE_VEC4_OFF,2,{OA_FUNC_PTR, OA_BIT1,      OA_NONE} },
      { "%force/vec4/off/d",از_FORCE_VEC4_OFF_D,3,{OA_FUNC_PTR, OA_BIT1,  OA_BIT2} },
      { "%force/wr",      از_FORCE_WR,      1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%fork",   از_FORK,   2,  {OA_کد_PTR2,OA_VPI_PTR,  OA_NONE} },
      { "%free",   از_FREE,   1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%inv",    از_INV,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%ix/add", از_IX_جمع_کن, 3,  {OA_عدد,   OA_BIT1,     OA_BIT2} },
      { "%ix/getv",از_IX_GETV,2,  {OA_BIT1,     OA_FUNC_PTR, OA_NONE} },
      { "%ix/getv/s",از_IX_GETV_S,2, {OA_BIT1,   OA_FUNC_PTR, OA_NONE} },
      { "%ix/load",از_IX_بارگیری,3,  {OA_عدد,   OA_BIT1,     OA_BIT2} },
      { "%ix/mov", از_IX_MOV, 2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%ix/mul", از_IX_MUL, 3,  {OA_عدد,   OA_BIT1,     OA_BIT2} },
      { "%ix/sub", از_IX_SUB, 3,  {OA_عدد,   OA_BIT1,     OA_BIT2} },
      { "%ix/vec4",  از_IX_VEC4,  1,  {OA_عدد,   OA_NONE, OA_NONE} },
      { "%ix/vec4/s",از_IX_VEC4_S,1,  {OA_عدد,   OA_NONE, OA_NONE} },
      { "%jmp",    از_JMP,    1,  {OA_کد_PTR, OA_NONE,     OA_NONE} },
      { "%jmp/0",  از_JMP0,   2,  {OA_کد_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/0xz",از_JMP0_XZ, 2,  {OA_کد_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1",  از_JMP1,   2,  {OA_کد_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1xz",از_JMP1_XZ, 2,  {OA_کد_PTR, OA_BIT1,     OA_NONE} },
      { "%join",   از_JOIN,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%join/detach",از_JOIN_DETACH,1,{OA_عدد,OA_NONE,  OA_NONE} },
      { "%load/ar",از_بارگیری_AR,2,  {OA_ARR_PTR,  OA_BIT1,     OA_NONE} },
      { "%load/dar/r",  از_بارگیری_DAR_R,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE}},
      { "%load/dar/str",از_بارگیری_DAR_STR,  1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/dar/vec4",از_بارگیری_DAR_VEC4,1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/obj",   از_بارگیری_OBJ,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/obja",  از_بارگیری_OBJA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/prop/dar/vec4", از_بارگیری_PROP_DAR_VEC4, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%load/real",  از_بارگیری_REAL, 1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%load/str",   از_بارگیری_STR,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/stra",  از_بارگیری_STRA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/vec4",  از_بارگیری_VEC4, 1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%load/vec4a", از_بارگیری_VEC4_A,2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%max/wr", از_MAX_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%min/wr", از_MIN_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod",    از_MOD,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/s",  از_MOD_S,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/wr", از_MOD_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul",    از_MUL,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul/wr", از_MUL_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%muli",   از_MULI,   3,  {OA_BIT1,     OA_BIT2,     OA_عدد} },
      { "%nand",   از_NAND,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nand/r", از_NANDR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%neg/wr", از_NEG_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%new/cobj",  از_NEW_COBJ,  1, {OA_VPI_PTR,OA_NONE,  OA_NONE} },
      { "%new/darray",از_NEW_DARRAY,2, {OA_BIT1,   OA_STRING,OA_NONE} },
      { "%noop",   از_NOOP,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor",    از_NOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor/r",  از_NORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%null",   از_NULL,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or",     از_OR,     0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or/r",   از_ORR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%pad/s",  از_PAD_S,  1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%pad/u",  از_PAD_U,  1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%part/s", از_PART_S, 1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%part/u", از_PART_U, 1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%parti/s",از_PARTI_S,3,  {OA_عدد,   OA_BIT1,     OA_BIT2} },
      { "%parti/u",از_PARTI_U,3,  {OA_عدد,   OA_BIT1,     OA_BIT2} },
      { "%pop/obj", از_برداشت_OBJ, 2, {OA_BIT1,    OA_BIT2,     OA_NONE} },
      { "%pop/real",از_برداشت_REAL,1, {OA_عدد,  OA_NONE,     OA_NONE} },
      { "%pop/str", از_برداشت_STR, 1, {OA_عدد,  OA_NONE,     OA_NONE} },
      { "%pop/vec4",از_برداشت_VEC4,1, {OA_عدد,  OA_NONE,     OA_NONE} },
      { "%pow",     از_POW,     0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/s",   از_POW_S,   0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/wr",  از_POW_WR,  0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%prop/obj",از_PROP_OBJ,2,  {OA_عدد,   OA_BIT1,     OA_NONE} },
      { "%prop/queue/size", از_PROP_صف_اندازه,  1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%prop/r",  از_PROP_R,  1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%prop/str",از_PROP_STR,1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%prop/v",  از_PROP_V,  1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%push/ix/vec4", از_افزودن_IX_VEC4, 3, {OA_عدد, OA_BIT1, OA_BIT2} },
      { "%pushi/real",از_PUSHI_REAL,2,{OA_BIT1,   OA_BIT2,   OA_NONE} },
      { "%pushi/str", از_PUSHI_STR, 1,{OA_STRING, OA_NONE,   OA_NONE} },
      { "%pushi/vec4",از_PUSHI_VEC4,3,{OA_BIT1,   OA_BIT2,   OA_عدد} },
      { "%pushv/str", از_PUSHV_STR, 0,{OA_NONE,   OA_NONE,   OA_NONE} },
      { "%putc/str/vec4",از_PUTC_STR_VEC4,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/prop/r",   از_QINSERT_PROP_R,   2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/str", از_QINSERT_PROP_STR, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/v",   از_QINSERT_PROP_V,   3, {OA_عدد, OA_BIT1, OA_BIT2} },
      { "%qinsert/real",از_QINSERT_REAL,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/str", از_QINSERT_STR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/v",   از_QINSERT_V,   3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%qpop/b/real",از_QPOP_B_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/str", از_QPOP_B_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/v",   از_QPOP_B_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/f/real",از_QPOP_F_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/str", از_QPOP_F_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/v",   از_QPOP_F_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/prop/b/r",   از_QPOP_PROP_B_REAL, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/str", از_QPOP_PROP_B_STR, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/v",   از_QPOP_PROP_B_V,    2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%qpop/prop/f/r",   از_QPOP_PROP_F_REAL, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/str", از_QPOP_PROP_F_STR, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/v",   از_QPOP_PROP_F_V,    2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/append_word/v", از_صف_APPEND_کلمه_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/find/index/prop/v", از_صف_پیدا_کن_شاخص_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/find/index/v", از_صف_پیدا_کن_شاخص_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find/prop/v", از_صف_پیدا_کن_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/find/v", از_صف_پیدا_کن_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/prop/v", از_صف_پیدا_کن_اولی_شاخص_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/v", از_صف_پیدا_کن_اولی_شاخص_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/prop/v", از_صف_پیدا_کن_اولی_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/find_first/v", از_صف_پیدا_کن_اولی_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/prop/v", از_صف_پیدا_کن_آخرین_شاخص_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/v", از_صف_پیدا_کن_آخرین_شاخص_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/prop/v", از_صف_پیدا_کن_آخرین_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/find_last/v", از_صف_پیدا_کن_آخرین_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/max/obj/v", از_صف_MAX_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/max/prop/v", از_صف_MAX_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/max/v", از_صف_MAX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/min/obj/v", از_صف_MIN_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/min/prop/v", از_صف_MIN_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/min/v", از_صف_MIN_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/new_empty/v", از_صف_NEW_خالی_V, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%queue/size/v", از_صف_اندازه_V, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%queue/unique/index/obj/v", از_صف_UNIQUE_شاخص_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/index/prop/v", از_صف_UNIQUE_شاخص_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/unique/index/v", از_صف_UNIQUE_شاخص_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/unique/obj/v", از_صف_UNIQUE_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/prop/v", از_صف_UNIQUE_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/unique/v", از_صف_UNIQUE_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/word/prop/v", از_صف_کلمه_PROP_V, 3, {OA_عدد, OA_BIT1, OA_BIT2} },
      { "%queue/word/v", از_صف_کلمه_V, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%release/net",از_RELEASE_NET,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/reg",از_RELEASE_REG,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/wr", از_RELEASE_WR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%replicate", از_REPLICATE,   1,{OA_عدد,  OA_NONE,OA_NONE} },
      { "%ret/real",  از_RET_REAL,    1,{OA_عدد,  OA_NONE,OA_NONE} },
      { "%ret/str",   از_RET_STR,     1,{OA_عدد,  OA_NONE,OA_NONE} },
      { "%ret/vec4",  از_RET_VEC4,    3,{OA_عدد,  OA_BIT1,OA_BIT2} },
      { "%retload/real",از_RETLOAD_REAL,1,{OA_عدد,  OA_NONE,OA_NONE} },
      { "%retload/str", از_RETLOAD_STR, 1,{OA_عدد,  OA_NONE,OA_NONE} },
      { "%retload/vec4",از_RETLOAD_VEC4,1,{OA_عدد,  OA_NONE,OA_NONE} },
      { "%reverse/obj", از_معکوس_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%reverse/prop/obj", از_معکوس_PROP_OBJ, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%rsort/obj", از_RSORT_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%rsort/prop/obj", از_RSORT_PROP_OBJ, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%scopy",  از_SCOPY,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%set/dar/obj/real",از_مجموعه_DAR_OBJ_REAL,1,{OA_عدد,OA_NONE,OA_NONE} },
      { "%set/dar/obj/str", از_مجموعه_DAR_OBJ_STR, 1,{OA_عدد,OA_NONE,OA_NONE} },
      { "%set/dar/obj/vec4",از_مجموعه_DAR_OBJ_VEC4,1,{OA_عدد,OA_NONE,OA_NONE} },
      { "%shiftl",   از_SHIFTL,   1, {OA_عدد, OA_NONE,   OA_NONE} },
      { "%shiftr",   از_SHIFTR,   1, {OA_عدد, OA_NONE,   OA_NONE} },
      { "%shiftr/s", از_SHIFTR_S, 1, {OA_عدد, OA_NONE,   OA_NONE} },
      { "%shuffle/obj", از_SHUFFLE_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%shuffle/prop/obj", از_SHUFFLE_PROP_OBJ, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%sort/obj", از_مرتب_سازی_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%sort/prop/obj", از_مرتب_سازی_PROP_OBJ, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%split/vec4",    از_تقسیم_VEC4,    1,{OA_عدد,   OA_NONE, OA_NONE} },
      { "%store/dar/r",   از_STORE_DAR_R,   1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/str", از_STORE_DAR_STR, 1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/vec4",از_STORE_DAR_VEC4,1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/obj",   از_STORE_OBJ,   1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/obja",  از_STORE_OBJA,  2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/prop/obj",از_STORE_PROP_OBJ,2, {OA_عدد,  OA_BIT1, OA_NONE} },
      { "%store/prop/qb/r",   از_STORE_PROP_QB_R,   2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/str", از_STORE_PROP_QB_STR, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/v",   از_STORE_PROP_QB_V,   3, {OA_عدد, OA_BIT1, OA_BIT2} },
      { "%store/prop/qf/r",   از_STORE_PROP_QF_R,   2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/str", از_STORE_PROP_QF_STR, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/v",   از_STORE_PROP_QF_V,   3, {OA_عدد, OA_BIT1, OA_BIT2} },
      { "%store/prop/r",  از_STORE_PROP_R,  1, {OA_عدد,  OA_NONE, OA_NONE} },
      { "%store/prop/str",از_STORE_PROP_STR,1, {OA_عدد,  OA_NONE, OA_NONE} },
      { "%store/prop/v",  از_STORE_PROP_V,  2, {OA_عدد,  OA_BIT1, OA_NONE} },
      { "%store/qb/r",   از_STORE_QB_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/str", از_STORE_QB_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/v",   از_STORE_QB_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qdar/r",  از_STORE_QDAR_R,  2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/str",از_STORE_QDAR_STR,2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/v",  از_STORE_QDAR_V,  3,{OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qf/r",   از_STORE_QF_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/str", از_STORE_QF_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/v",   از_STORE_QF_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qobj/r",  از_STORE_QOBJ_R,  2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/str",از_STORE_QOBJ_STR,2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/v",  از_STORE_QOBJ_V,  3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/real",    از_STORE_REAL,    1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/reala",   از_STORE_REALA,   2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/str",     از_STORE_STR,     1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/stra",    از_STORE_STRA,    2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/vec4",    از_STORE_VEC4,    3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/vec4a",   از_STORE_VEC4_A,   3, {OA_ARR_PTR, OA_BIT1, OA_BIT2} },
      { "%sub",    از_SUB,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%sub/wr", از_SUB_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%subi",   از_SUBI,   3,  {OA_BIT1,     OA_BIT2,     OA_عدد} },
      { "%substr",     از_SUBSTR,     2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%substr/vec4",از_SUBSTR_VEC4,2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%test_nul",     از_TEST_NUL,     1,{OA_FUNC_PTR,OA_NONE,    OA_NONE} },
      { "%test_nul/a",   از_TEST_NUL_A,   2,{OA_ARR_PTR, OA_BIT1,    OA_NONE} },
      { "%test_nul/obj", از_TEST_NUL_OBJ, 0,{OA_NONE,    OA_NONE,    OA_NONE} },
      { "%test_nul/prop",از_TEST_NUL_PROP,2,{OA_عدد,  OA_BIT1,    OA_NONE} },
      { "%uparr/and/v",          از_UPARR_AND_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/or/v",           از_UPARR_OR_V,           2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/product/obj/v",  از_UPARR_حاصلضرب_OBJ_V,  1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/product/prop/v", از_UPARR_حاصلضرب_PROP_V, 2, {OA_عدد,   OA_BIT1, OA_NONE} },
      { "%uparr/product/r",      از_UPARR_حاصلضرب_REAL,   1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/product/v",      از_UPARR_حاصلضرب_V,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/sum/obj/v",      از_UPARR_جمع_OBJ_V,      1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/sum/prop/v",     از_UPARR_جمع_PROP_V,     2, {OA_عدد,   OA_BIT1, OA_NONE} },
      { "%uparr/sum/r",          از_UPARR_جمع_REAL,       1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/sum/v",          از_UPARR_جمع_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/xor/v",          از_UPARR_XOR_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%wait",   از_WAIT,   1,  {OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%wait/fork",از_WAIT_FORK,0,{OA_NONE,   OA_NONE,     OA_NONE} },
      { "%xnor",   از_XNOR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xnor/r", از_XNORR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor",    از_XOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor/r",  از_XORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { 0, از_NOOP, 0, {OA_NONE, OA_NONE, OA_NONE} }
};

ساكن ثابت بیعلامت opcode_شمار =
                    حجم(opcode_جدول)/حجم(*opcode_جدول) - 1;

ساكن صحيح opcode_compare(ثابت فراغ*k, ثابت فراغ*r)
{
      ثابت حرف*kp = static_cast<ثابت حرف*>(k);
      ثابت بنية opcode_جدول_s*rp = static_cast<ثابت بنية opcode_جدول_s*>(r);
      أرجع strcmp(kp, rp->mnemonic);
}

/*
 * Keep a symbol table of addresses within code space. Labels on
 * executable opcodes are mapped to their address here.
 */
ساكن نماد_جدول_t sym_codespace = 0;

/*
 * Keep a symbol table of functors mentioned in the source. This table
 * is used to resolve references as they come.
 */
ساكن نماد_جدول_t sym_functors = 0;

/*
 * VPI objects are indexed during compile time so that they can be
 * linked together as they are created. This symbol table matches
 * labels to vpiHandles.
 */
ساكن نماد_جدول_t sym_vpi = 0;


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

فراغ define_functor_نماد(ثابت حرف*label, vvp_net_t*net)
{
      نماد_مقدار_t val;
      val.net = net;
      sym_مجموعه_مقدار(sym_functors, label, val);
}

ساكن vvp_net_t*lookup_functor_نماد(ثابت حرف*label)
{
      أكد(sym_functors);
      نماد_مقدار_t val = sym_get_مقدار(sym_functors, label);
      أرجع val.net;
}

vpiHandle vvp_lookup_handle(ثابت حرف*label)
{
      نماد_مقدار_t val = sym_get_مقدار(sym_vpi, label);
      إذا (val.ptr) أرجع (vpiHandle) val.ptr;
      أرجع 0;
}

vvp_net_t* vvp_net_lookup(ثابت حرف*label)
{
      ساكن منطقي t0_trigger_generated = خطأ;

        /* First, look to see if the symbol is a vpi object of some
	   sort. If it is, then get the vvp_ipoint_t pointer out of
	   the vpiHandle. */
      نماد_مقدار_t val = sym_get_مقدار(sym_vpi, label);
      إذا (val.ptr) {
	    vpiHandle vpi = static_cast<vpiHandle>(val.ptr);
	    بدل (vpi->get_type_کد()) {
		حالة vpiNet:
		حالة vpiReg:
		حالة vpi_بیت_Var:
		حالة vpiByteVar:
		حالة vpiShortIntVar:
		حالة vpiIntVar:
		حالة vpiLongIntVar:
		حالة vpiIntegerVar: {
		      ثابت __vpiSignal*sig = dynamic_cast<__vpiSignal*>(vpi);
		      أرجع sig->گره;
		}

		حالة vpiRealVar: {
		      ثابت __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(vpi);
		      أرجع sig->net;
		}

		حالة vpiStringVar:
		حالة vpiArrayVar:
		حالة vpiClassVar: {
		      ثابت __vpiBaseVar*sig = dynamic_cast<__vpiBaseVar*>(vpi);
		      أرجع sig->get_net();
		}

		حالة vpiNamedEvent: {
		      ثابت __vpiNamedEvent*tmp = dynamic_cast<__vpiNamedEvent*>(vpi);
		      أرجع tmp->funct;
		}

		افتراضي:
		  fprintf(stderr, "Unsupported type %d.\n",
		          vpi->get_type_کد());
		  أكد(0);
	    }
      }


	/* Failing that, look for a general functor. */
      vvp_net_t*tmp = lookup_functor_نماد(label);

	// This is a special label used to create a T0 trigger for the
	// always_comb/latch processes.
     إذا (! t0_trigger_generated && (strcmp(label, "E_0x0") == 0)) {
	      // This should never happen, but if it does then the E_0x0
	      // event generation may need to be explicitly generated in
	      // the compiler output instead of implicitly in this code.
	    أكد(! vpip_peek_current_scope()->is_automatic());
	    t0_trigger_generated = درست;
	      // Create a local event with no name for the T0 trigger
	    compile_named_event(strdup(label), strcpy(جديد حرف [1],""), درست);
	    tmp = vvp_net_lookup(label);
	    أكد(tmp);
	      // Create a trigger for the T0 event.
	    vvp_net_ptr_t ptr (tmp, 0);
	    زمانبندی_t0_trigger(ptr);
     }

      أرجع tmp;
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
ساكن resolv_list_s*resolv_list = 0;

resolv_list_s::~resolv_list_s()
{
      حرر(label_);
}

فراغ resolv_submit(resolv_list_s*cur)
{
      إذا (cur->resolve()) {
	    حذف cur;
	    أرجع;
      }

      cur->بعدی_2 = resolv_list;
      resolv_list = cur;
}


/*
 * Look up vvp_nets in the symbol table. The "source" is the label for
 * the net that I want to feed, and net->port[port] is the vvp_net
 * input that I want that node to feed into. When the name is found,
 * put net->port[port] into the fan-out list for that node.
 */
بنية vvp_net_resolv_list_s: عام resolv_list_s {

      صریح vvp_net_resolv_list_s(حرف*l) : resolv_list_s(l) { }
	// port to be driven by the located node.
      vvp_net_ptr_t پورت;
      مجازی منطقي resolve(منطقي mes) override;
};

منطقي vvp_net_resolv_list_s::resolve(منطقي mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      إذا (tmp) {
	      // Link the input port to the located output.
	    tmp->link(پورت);
	    أرجع درست;
      }

      إذا (mes)
	    fprintf(stderr, "unresolved vvp_net reference: %s\n", label());

      أرجع خطأ;
}

مضمن ساكن
فراغ postpone_functor_درون_داد(vvp_net_ptr_t پورت, حرف*lab)
{
      بنية vvp_net_resolv_list_s*res = جديد بنية vvp_net_resolv_list_s(lab);
      res->پورت   = پورت;

      resolv_submit(res);
}


/*
 *  Generic functor reference lookup.
 */

بنية functor_gen_resolv_list_s: عام resolv_list_s {
      صریح functor_gen_resolv_list_s(حرف*txt) : resolv_list_s(txt) {
	    ref = 0;
      }
      vvp_net_t**ref;
      مجازی منطقي resolve(منطقي mes) override;
};

منطقي functor_gen_resolv_list_s::resolve(منطقي mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      إذا (tmp) {
	    *ref = tmp;
	    أرجع درست;
      }

      إذا (mes)
	    fprintf(stderr, "unresolved functor reference: %s\n", label());

      أرجع خطأ;
}

فراغ functor_ref_lookup(vvp_net_t**ref, حرف*lab)
{
      بنية functor_gen_resolv_list_s*res =
	    جديد بنية functor_gen_resolv_list_s(lab);

      res->ref    = ref;

      resolv_submit(res);
}

/*
 *  vpiHandle lookup
 */

بنية vpi_handle_resolv_list_s: عام resolv_list_s {
      صریح vpi_handle_resolv_list_s(حرف*lab) : resolv_list_s(lab) {
	    handle = عدم;
      }
      مجازی منطقي resolve(منطقي mes) override;
      vpiHandle *handle;
};

منطقي vpi_handle_resolv_list_s::resolve(منطقي mes)
{
      نماد_مقدار_t val = sym_get_مقدار(sym_vpi, label());
      إذا (!val.ptr) {
	    // check for thread access symbols
	    بیعلامت پایه, wid;
	    size_t n = 0;
	    حرف ss[32];
	    إذا (2 == از_رشته_بخوان(label(), "W<%u,%31[r]>%zn", &پایه, ss, &n)
		       && n == طول_رشته(label())) {

		  val.ptr = vpip_make_vthr_کلمه(پایه, ss);
		  sym_مجموعه_مقدار(sym_vpi, label(), val);

	    } وإلا إذا (1 == از_رشته_بخوان(label(), "S<%u,str>%zn", &پایه, &n)
		       && n == طول_رشته(label())) {

		  val.ptr = vpip_make_vthr_str_پشته(پایه);
		  sym_مجموعه_مقدار(sym_vpi, label(), val);

	    } وإلا إذا (3 == از_رشته_بخوان(label(), "S<%u,vec4,%31[su]%u>%zn", &پایه, ss, &wid, &n)
		       && n == طول_رشته(label())) {

		  منطقي signed_flag = خطأ;
		  لكل (حرف*fp = ss ;  *fp ;  fp += 1) بدل (*fp) {
		      حالة 's':
			signed_flag = درست;
			اكسر;
		      حالة 'u':
			signed_flag = خطأ;
			اكسر;
		      افتراضي:
			اكسر;
		  }
		  val.ptr = vpip_make_vthr_vec4_پشته(پایه, signed_flag, wid);
		  sym_مجموعه_مقدار(sym_vpi, label(), val);
	    }

      }

      إذا (!val.ptr) {
	    // check for memory word  M<mem,base,wid>
      }

      إذا (strcmp(label(), "null") == 0) {
	    val.ptr = vpip_make_null_const();
	    sym_مجموعه_مقدار(sym_vpi, label(), val);
      }

      إذا (val.ptr) {
	    *handle = static_cast<vpiHandle>(val.ptr);
	    أرجع درست;
      }

      إذا (mes)
	    fprintf(stderr, "unresolved vpi name lookup: %s\n", label());

      أرجع خطأ;
}

فراغ compile_vpi_lookup(vpiHandle *handle, حرف*label)
{
      إذا (strcmp(label, "$time") == 0) {
	    *handle = vpip_sim_زمان(vpip_peek_current_scope(), خطأ);
	    حرر(label);
	    أرجع;
      }

      إذا (strcmp(label, "$stime") == 0) {
	    *handle = vpip_sim_زمان(vpip_peek_current_scope(), درست);
	    حرر(label);
	    أرجع;
      }

      إذا (strcmp(label, "$realtime") == 0) {
	    *handle = vpip_sim_realtime(vpip_peek_current_scope());
	    حرر(label);
	    أرجع;
      }

      إذا (strcmp(label, "$simtime") == 0) {
	    *handle = vpip_sim_زمان(0, خطأ);
	    حرر(label);
	    أرجع;
      }

      بنية vpi_handle_resolv_list_s*res
	    = جديد بنية vpi_handle_resolv_list_s(label);

      res->handle = handle;
      resolv_submit(res);
}

/*
 * Code Label lookup
 */

بنية کد_label_resolv_list_s: عام resolv_list_s {
      صریح کد_label_resolv_list_s(حرف*lab, منطقي cptr2) : resolv_list_s(lab) {
	    code = عدم;
	    cptr2_flag = cptr2;
      }
      بنية vvp_کد_s *code;
      منطقي cptr2_flag;
      مجازی منطقي resolve(منطقي mes) override;
};

منطقي کد_label_resolv_list_s::resolve(منطقي mes)
{
      نماد_مقدار_t val = sym_get_مقدار(sym_codespace, label());
      إذا (val.ptr) {
	    إذا (cptr2_flag)
		  code->cptr2 = reinterpret_cast<vvp_کد_t>(val.ptr);
	    وإلا
		  code->cptr = reinterpret_cast<vvp_کد_t>(val.ptr);
	    أرجع درست;
      }

      إذا (mes)
	    fprintf(stderr, "unresolved code label: %s\n", label());

      أرجع خطأ;
}

فراغ کد_label_lookup(بنية vvp_کد_s *code, حرف *label, منطقي cptr2)
{
      بنية کد_label_resolv_list_s *res
	    = جديد بنية کد_label_resolv_list_s(label, cptr2);

      res->code  = code;

      resolv_submit(res);
}

بنية کد_array_resolv_list_s: عام resolv_list_s {
      صریح کد_array_resolv_list_s(حرف*lab) : resolv_list_s(lab) {
	    code = عدم;
      }
      بنية vvp_کد_s *code;
      مجازی منطقي resolve(منطقي mes) override;
};

منطقي کد_array_resolv_list_s::resolve(منطقي mes)
{
      code->array = array_پیدا_کن(label());
      إذا (code->array != 0) {
	    أرجع درست;
      }

      إذا (mes)
	    fprintf(stderr, "Array unresolved: %s\n", label());

      أرجع خطأ;
}

ساكن فراغ compile_array_lookup(بنية vvp_کد_s*code, حرف*label)
{
      بنية کد_array_resolv_list_s *res
	    = جديد بنية کد_array_resolv_list_s(label);

      res->code  = code;

      resolv_submit(res);
}

ساكن std::list<بنية __vpiSysTaskCall*> scheduled_compiletf;

فراغ compile_compiletf(بنية __vpiSysTaskCall*obj)
{
      إذا (obj->defn->info.compiletf == 0)
	    أرجع;

      scheduled_compiletf.push_back(obj);
}

/*
 * When parsing is otherwise complete, this function is called to do
 * the final stuff. Clean up deferred linking here.
 */

فراغ compile_cleanup(فراغ)
{
      صحيح lnerrs = -1;
      صحيح nerrs = 0;
      صحيح آخرین;

      إذا (verbose_flag) {
	    fprintf(stderr, " ... Linking\n");
	    fflush(stderr);
      }

      افعل {
	    resolv_list_s *res = resolv_list;
	    resolv_list = 0x0;
	    آخرین = nerrs == lnerrs;
	    lnerrs = nerrs;
	    nerrs = 0;
	    طالما (res) {
		  resolv_list_s *cur = res;
		  res = res->بعدی_2;
		  إذا (cur->resolve(آخرین))
			حذف cur;
		  وإلا {
			nerrs++;
			cur->بعدی_2 = resolv_list;
			resolv_list = cur;
		  }
	    }
	    إذا (nerrs && آخرین)
		  fprintf(stderr,
			  "compile_cleanup: %d unresolved items\n",
			  nerrs);
      } طالما (nerrs && !آخرین);

      compile_errors += nerrs;

      إذا (verbose_flag) {
	    fprintf(stderr, " ... Removing symbol tables\n");
	    fflush(stderr);
      }

	/* After compile is complete, the vpi symbol table is no
	   longer needed. VPI objects are located by following
	   scopes. */
      delete_نماد_جدول(sym_vpi);
      sym_vpi = 0;

	/* Don't need the code labels. The instructions have numeric
	   pointers in them, the symbol table is no longer needed. */
      delete_نماد_جدول(sym_codespace);
      sym_codespace = 0;

      delete_نماد_جدول(sym_functors);
      sym_functors = 0;

      delete_udp_symbols();

      compile_island_cleanup();
      compile_array_cleanup();

      إذا (verbose_flag) {
	    fprintf(stderr, " ... Compiletf functions\n");
	    fflush(stderr);
      }

      أكد(vpi_نما_flag == VPI_نما_NONE);
      vpi_نما_flag = VPI_نما_COMPILETF;

      طالما (! scheduled_compiletf.empty()) {
	    بنية __vpiSysTaskCall*obj = scheduled_compiletf.front();
	    scheduled_compiletf.pop_front();
	    vpip_cur_task = obj;
	    obj->defn->info.compiletf (obj->defn->info.user_داده);
	    vpip_cur_task = 0;
      }

      vpi_نما_flag = VPI_نما_NONE;
}

فراغ compile_vpi_نماد(ثابت حرف*label, vpiHandle obj)
{
      نماد_مقدار_t val;
      val.ptr = obj;
      sym_مجموعه_مقدار(sym_vpi, label, val);
}

/*
 * Initialize the compiler by allocation empty symbol tables and
 * initializing the various address spaces.
 */
فراغ compile_init(فراغ)
{
      sym_vpi = new_نماد_جدول();

      sym_functors = new_نماد_جدول();

      sym_codespace = new_نماد_جدول();
      codespace_init();
}

فراغ compile_بارگیری_vpi_module(حرف*name)
{
      vpip_بارگیری_module(name);
      حذف[] name;
}

فراغ compile_vpi_زمان_precision(دراز pre)
{
      vpip_مجموعه_زمان_precision(pre);
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
ساكن منطقي crstring_سرآیند_test(ثابت حرف*str)
{
      إذا ((str[0] != 'C') && (str[0] != 'c'))
	    أرجع خطأ;
      إذا ((str[1] != 'r') || (str[2] != '<'))
	    أرجع خطأ;

      أرجع درست;
}

منطقي crstring_test(ثابت حرف*str)
{
      إذا (!crstring_سرآیند_test(str))
	    أرجع خطأ;
      ثابت حرف*tp = strchr(str, '>');
      إذا (tp == 0) أرجع خطأ;
      إذا (tp[1] != 0) أرجع خطأ;

      إذا ((strspn(str+3, "0123456789abcdefmg")+3) != (size_t)(tp - str))
	  أرجع خطأ;

      أرجع درست;
}

مزدوج crstring_مقصد_double(ثابت حرف*label)
{
      أكد(crstring_سرآیند_test(label));

      ثابت حرف*cp = label+3;
      أكد(*cp == 'm');
      cp += 1;
      حرف*ep;
      uint64_t mant = strtoull(cp, &ep, 16);
      cp = ep;
      أكد(*cp == 'g');
      cp += 1;
      صحيح نمایی_تابع = strtoul(cp, 0, 16);

      مزدوج tmp;
      إذا (mant == 0 && نمایی_تابع == 0x3fff) {
	    tmp = INFINITY;
      } وإلا إذا (mant == 0 && نمایی_تابع == 0x7fff) {
	    tmp = -INFINITY;
      } وإلا إذا (نمایی_تابع == 0x3fff) {
	    tmp = nan("");
      } وإلا {
	    مزدوج sign = (نمایی_تابع & 0x4000)? -1.0 : 1.0;
	    نمایی_تابع &= 0x1fff;

	    tmp = sign * ldexp((مزدوج)mant, نمایی_تابع - 0x1000);
      }

      أرجع tmp;
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

فراغ درون_داد_connect(vvp_net_t*fdx, بیعلامت پورت, حرف*label)
{
      vvp_net_ptr_t ifdx = vvp_net_ptr_t(fdx, پورت);

	/* Is this a vvp_vector4_t constant value? */
      إذا (c4string_test(label)) {

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
	    إذا (label[0] == 'c')
		  زمانبندی_init_وکتور(ifdx, tmp);
	    وإلا
		  زمانبندی_مجموعه_وکتور(ifdx, tmp);

	    حرر(label);
	    أرجع;
      }

	/* Is this a vvp_vector8_t constant value? */
      إذا (c8string_test(label)) {

	    vvp_vector8_t tmp = c8string_مقصد_vector8(label);
	    إذا (label[0] == 'c')
		  زمانبندی_init_وکتور(ifdx, tmp);
	    وإلا
		  زمانبندی_مجموعه_وکتور(ifdx, tmp);

	    حرر(label);
	    أرجع;
      }

	/* Handle the Cr<> constant driver, which is a real-value
	   driver. */
      إذا (crstring_test(label)) {

	    مزدوج tmp = crstring_مقصد_double(label);

	    إذا (label[0] == 'c')
		  زمانبندی_init_وکتور(ifdx, tmp);
	    وإلا
		  زمانبندی_مجموعه_وکتور(ifdx, tmp);
	    حرر(label);
	    أرجع;
      }

	/* Handle the general case that this is a label for a node in
	   the vvp net. This arranges for the label to be preserved in
	   a linker list, and linked when the symbol table is
	   complete. */
      postpone_functor_درون_داد(ifdx, label);
}

فراغ inputs_connect(vvp_net_t*fdx, بیعلامت argc, بنية symb_s*argv)
{
      إذا (argc > 4) {
	    std::cerr << "XXXX argv[0] = " << argv[0].متن << std::پایان_سطر;
      }
      أكد(argc <= 4);

      لكل (بیعلامت idx = 0;  idx < argc;  idx += 1) {

	    درون_داد_connect(fdx, idx, argv[idx].متن);
      }
}

فراغ wide_inputs_connect(vvp_wide_fun_core*core,
			 بیعلامت argc, بنية symb_s*argv)
{
	/* Create input functors to receive values from the
	   network. These functors pass the data to the core. */
      بیعلامت درون_داد_functors = (argc+3) / 4;
      لكل (بیعلامت idx = 0 ;  idx < درون_داد_functors ;  idx += 1) {
	    بیعلامت پایه = idx*4;
	    بیعلامت trans = 4;
	    إذا (پایه+trans > argc)
		  trans = argc - پایه;

	    vvp_wide_fun_t*cur = جديد vvp_wide_fun_t(core, پایه);
	    vvp_net_t*ptr = جديد vvp_net_t;
	    ptr->fun = cur;

	    inputs_connect(ptr, trans, argv+پایه);
      }
}

قالب <صنف T_> فراغ make_arith(T_ *arith, حرف*label,
				    بیعلامت argc, بنية symb_s*argv)
{
      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = arith;

      define_functor_نماد(label, ptr);
      حرر(label);

      أكد(argc == 2);
      inputs_connect(ptr, argc, argv);

      حرر(argv);
}

فراغ compile_arith_cast_int(حرف*label, دراز width,
                            بیعلامت argc, بنية symb_s*argv)
{
      vvp_arith_cast_int*arith = جديد vvp_arith_cast_int((بیعلامت) width);

      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = arith;

      define_functor_نماد(label, ptr);
      حرر(label);

      أكد(argc == 1);
      inputs_connect(ptr, argc, argv);
      حرر(argv);
}

فراغ compile_arith_cast_vec2(حرف*label, دراز width,
			     بیعلامت argc, بنية symb_s*argv)
{
      vvp_arith_cast_vec2*arith = جديد vvp_arith_cast_vec2((بیعلامت) width);

      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = arith;

      define_functor_نماد(label, ptr);
      حرر(label);

      أكد(argc == 1);
      inputs_connect(ptr, argc, argv);
      حرر(argv);
}

فراغ compile_arith_cast_real(حرف*label, منطقي signed_flag,
                             بیعلامت argc, بنية symb_s*argv)
{
      vvp_arith_cast_real*arith = جديد vvp_arith_cast_real(signed_flag);

      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = arith;

      define_functor_نماد(label, ptr);
      حرر(label);

      أكد(argc == 1);
      inputs_connect(ptr, argc, argv);
      حرر(argv);
}

فراغ compile_arith_abs(حرف*label, بیعلامت argc, بنية symb_s*argv)
{
      vvp_arith_abs*arith = جديد vvp_arith_abs;

      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = arith;

      define_functor_نماد(label, ptr);
      حرر(label);

      أكد(argc == 1);
      inputs_connect(ptr, argc, argv);
      حرر(argv);
}

فراغ compile_arith_div(حرف*label, دراز wid, منطقي signed_flag,
		       بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    ثابت حرف *suffix = "";
	    إذا (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s; .arith/div%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_arith_div(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

فراغ compile_arith_div_r(حرف*label, بیعلامت argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s; .arith/divr has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_arith_div_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_arith_mod(حرف*label, دراز wid, منطقي signed_flag,
		       بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    fprintf(stderr, "%s .arith/mod has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_arith_mod(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

فراغ compile_arith_mod_r(حرف*label, بیعلامت argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .arith/mod.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_arith_mod_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_arith_mult(حرف*label, دراز wid,
			بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    fprintf(stderr, "%s .arith/mult has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_arith_mult(wid);
      make_arith(arith, label, argc, argv);
}

فراغ compile_arith_mult_r(حرف*label, بیعلامت argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .arith/mult.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_arith_mult_real;
      make_arith(arith, label, argc, argv);
}


فراغ compile_arith_pow(حرف*label, دراز wid, منطقي signed_flag,
		       بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    ثابت حرف *suffix = "";
	    إذا (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s .arith/pow%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_arith_pow(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

فراغ compile_arith_pow_r(حرف*label, بیعلامت argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .arith/pow.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_arith_pow_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_arith_sub(حرف*label, دراز wid, بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    fprintf(stderr, "%s .arith/sub has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_arith_sub(wid);
      make_arith(arith, label, argc, argv);
}

فراغ compile_arith_sub_r(حرف*label, بیعلامت argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s; .arith/sub.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_arith_sub_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_arith_جمع(حرف*label, دراز wid, بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    fprintf(stderr, "%s .arith/sum has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_arith_جمع(wid);
      make_arith(arith, label, argc, argv);
}

فراغ compile_arith_جمع_r(حرف*label, بیعلامت argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .arith/sum.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_arith_جمع_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_eeq(حرف*label, دراز wid,
		     بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_cmp_eeq(wid);

      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_nee(حرف*label, دراز wid,
		     بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_cmp_nee(wid);

      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_eq(حرف*label, دراز wid, بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_cmp_eq(wid);
      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_eqx(حرف*label, دراز wid, بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqx has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_cmp_eqx(wid);
      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_eqz(حرف*label, دراز wid, بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqz has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_cmp_eqz(wid);
      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_eq_r(حرف*label, بیعلامت argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_cmp_eq_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_ne(حرف*label, دراز wid, بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_cmp_ne(wid);
      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_ne_r(حرف*label, بیعلامت argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_cmp_ne_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_ge(حرف*label, دراز wid, منطقي signed_flag,
		    بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_cmp_ge(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_ge_r(حرف*label, بیعلامت argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_cmp_ge_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_gt(حرف*label, دراز wid, منطقي signed_flag,
		    بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_cmp_gt(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_gt_r(حرف*label, بیعلامت argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_cmp_gt_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_weq(حرف*label, دراز wid,
		     بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/weq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_cmp_weq(wid);

      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_wne(حرف*label, دراز wid,
		     بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/wne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_cmp_wne(wid);

      make_arith(arith, label, argc, argv);
}


فراغ compile_delay(حرف*label, بیعلامت width,
                   vvp_delay_t*delay, بنية symb_s arg)
{
      vvp_net_t*net = جديد vvp_net_t;
      vvp_fun_delay*obj = جديد vvp_fun_delay(net, width, *delay);
      net->fun = obj;

      حذف delay;

      درون_داد_connect(net, 0, arg.متن);

      define_functor_نماد(label, net);
      حرر(label);
}

فراغ compile_delay(حرف*label, بیعلامت width,
                   بیعلامت argc, بنية symb_s*argv, منطقي ignore_decay)
{
      vvp_delay_t stub (0, 0, 0);
      إذا (ignore_decay) stub.مجموعه_ignore_decay();
      vvp_net_t*net = جديد vvp_net_t;
      vvp_fun_delay*obj = جديد vvp_fun_delay(net, width, stub);
      net->fun = obj;

      inputs_connect(net, argc, argv);
      حرر(argv);

      define_functor_نماد(label, net);
      حرر(label);
}

/*
 * Extend nodes.
 */
فراغ compile_extend_signed(حرف*label, دراز wid, بنية symb_s arg)
{
      أكد(wid >= 0);

      vvp_fun_extend_signed*fun = جديد vvp_fun_extend_signed(wid);
      vvp_net_t*ptr = جديد vvp_net_t;
      ptr->fun = fun;

      define_functor_نماد(label, ptr);
      حرر(label);

      درون_داد_connect(ptr, 0, arg.متن);
}

بنية __vpiModPath* compile_modpath(حرف*label, بیعلامت width,
                                     بنية symb_s drv, بنية symb_s dest)
{
      vvp_net_t*net = جديد vvp_net_t;
      vvp_fun_modpath*obj = جديد vvp_fun_modpath(net, width);
      net->fun = obj;

      درون_داد_connect(net, 0, drv.متن);

      define_functor_نماد(label, net);

      __vpiModPath*modpath = vpip_make_modpath(net);

      compile_vpi_lookup(&modpath->مسیر_جمله_بیرون.عبارت, dest.متن);

      حرر(label);

      modpath->modpath = obj;
      أرجع modpath;
}

ساكن بنية __vpiModPathSrc*make_modpath_src(بنية __vpiModPath*مسیر,
					       حرف یال_c,
					       ثابت بنية symb_s&src,
					       بنية numbv_s&vals,
					       منطقي ifnone)
{
      vvp_fun_modpath*dst = مسیر->modpath;

      vvp_time64_t use_delay[12];
      أكد(vals.cnt == 12);
      لكل (بیعلامت idx = 0 ; idx < vals.cnt ;  idx += 1) {
	    use_delay[idx] = vals.nvec[idx];
      }

      numbv_clear(&vals);

      vvp_fun_modpath_src*obj = 0;

      صحيح vpi_یال = vpi_No_یال;
      إذا (یال_c == 0) {
	    obj = جديد vvp_fun_modpath_src(use_delay);

      } وإلا {
	    منطقي posedge, negedge;
	    بدل (یال_c) {
		حالة '+':
		  vpi_یال = vpiPosedge;
		  posedge = درست;
		  negedge = خطأ;
		  اكسر;
		حالة '-':
		  vpi_یال = vpiNegedge;
		  posedge = خطأ;
		  negedge = درست;
		  اكسر;
#إذا 0
		حالة '*':
		  posedge = درست;
		  negedge = درست;
		  اكسر;
#پایان_اگر
		افتراضي:
		  posedge = خطأ;
		  negedge = خطأ;
		  fprintf(stderr, "Unknown edge identifier %c(%d).\n", یال_c,
		          یال_c);
		  أكد(0);
	    }
	    obj = جديد vvp_fun_modpath_یال(use_delay, posedge, negedge);
      }

      vvp_net_t*net = جديد vvp_net_t;
      بنية __vpiModPathSrc* srcobj = vpip_make_modpath_src(مسیر, net) ;
      vpip_attach_مقصد_current_scope(srcobj);
      net->fun = obj;

	/* Save the vpiEdge directory into the input path term. */
      srcobj->مسیر_جمله_درون.یال = vpi_یال;
      درون_داد_connect(net, 0, src.متن);
      dst->جمع_کن_modpath_src(obj, ifnone);

      أرجع srcobj;
}

فراغ compile_modpath_src(بنية __vpiModPath*dst, حرف یال_c,
			 ثابت بنية symb_s&src,
			 بنية numbv_s&vals,
			 ثابت بنية symb_s&condit_src,
			 ثابت بنية symb_s&مسیر_جمله_درون)
{
      بنية __vpiModPathSrc*obj =
	    make_modpath_src(dst, یال_c, src, vals, خطأ);
      درون_داد_connect(obj->net, 1, condit_src.متن);
      compile_vpi_lookup(&obj->مسیر_جمله_درون.عبارت, مسیر_جمله_درون.متن);
}

فراغ compile_modpath_src(بنية __vpiModPath*dst, حرف یال_c,
			 ثابت بنية symb_s&src,
			 بنية numbv_s&vals,
			 صحيح condit_src,
			 ثابت بنية symb_s&مسیر_جمله_درون,
			 منطقي ifnone)
{
      أكد(condit_src == 0);
      بنية __vpiModPathSrc*obj =
	    make_modpath_src(dst, یال_c, src, vals, ifnone);
      compile_vpi_lookup(&obj->مسیر_جمله_درون.عبارت, مسیر_جمله_درون.متن);
}

/*
 * A .shift/l statement creates an array of functors for the
 * width. The 0 input is the data vector to be shifted and the 1 input
 * is the amount of the shift. An unconnected shift amount is set to 0.
 */
فراغ compile_shiftl(حرف*label, دراز wid, بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      vvp_arith_ *arith = جديد vvp_shiftl(wid);
      make_arith(arith, label, argc, argv);
}

فراغ compile_shiftr(حرف*label, دراز wid, منطقي signed_flag,
		    بیعلامت argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      vvp_arith_ *arith = جديد vvp_shiftr(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

فراغ compile_resolver(حرف*label, حرف*type, بیعلامت argc, بنية symb_s*argv)
{
      vvp_net_t*net = جديد vvp_net_t;

      resolv_core*core = 0;

      إذا (strcmp(type,"tri") == 0) {
	    core = جديد resolv_tri(argc, net, vvp_scalar_t(BIT4_Z, 0,0));

      } وإلا إذا (strcmp(type,"tri0") == 0) {
	    core = جديد resolv_tri(argc, net, vvp_scalar_t(BIT4_0, 5,5));

      } وإلا إذا (strcmp(type,"tri1") == 0) {
	    core = جديد resolv_tri(argc, net, vvp_scalar_t(BIT4_1, 5,5));

      } وإلا إذا (strcmp(type,"triand") == 0) {
	    core = جديد resolv_triand(argc, net);

      } وإلا إذا (strcmp(type,"trior") == 0) {
	    core = جديد resolv_trior(argc, net);

      } وإلا {
	    fprintf(stderr, "invalid resolver type: %s\n", type);
	    compile_errors += 1;
            حذف net;
      }

      إذا (core) {
	    net->fun = core;
	    define_functor_نماد(label, net);

            لكل (بیعلامت پایه = 0 ;  پایه < argc ;  پایه += 4) {
	          بیعلامت nports = argc - پایه;
                  إذا (nports > 4)
                        nports = 4;

                  إذا (پایه > 0) {
                        net = جديد vvp_net_t;
                        net->fun = جديد resolv_extend(core, پایه);
                  }
                  inputs_connect(net, nports, argv+پایه);
            }
      }
      حرر(type);
      حرر(label);
      حرر(argv);
}

فراغ compile_udp_def(صحيح sequ, حرف *label, حرف *name,
		     بیعلامت nin, بیعلامت init, حرف **جدول)
{
      إذا (sequ) {
	    vvp_bit4_t init4;
	    إذا (init == 0)
		  init4 = BIT4_0;
	    وإلا إذا (init == 1)
		  init4 = BIT4_1;
	    وإلا
		  init4 = BIT4_X;

	    vvp_udp_seq_s *u = جديد vvp_udp_seq_s(label, name, nin, init4);
	    u->compile_جدول(جدول);
      } وإلا {
	    vvp_udp_comb_s *u = جديد vvp_udp_comb_s(label, name, nin);
	    u->compile_جدول(جدول);
      }
      حرر(label);
}

حرف **compile_udp_جدول(حرف **جدول, حرف *ردیف)
{
  إذا (جدول)
    أكد(طول_رشته(*جدول)==طول_رشته(ردیف));

  حرف **tt;
  لكل (tt = جدول; tt && *tt; tt++) { }
  صحيح n = (tt-جدول) + 2;

  جدول = static_cast<حرف**>(realloc(جدول, n*حجم(حرف*)));
  جدول[n-2] = ردیف;
  جدول[n-1] = 0x0;

  أرجع جدول;
}


/*
 * The parser uses this function to compile and link an executable
 * opcode. I do this by looking up the opcode in the opcode_table. The
 * table gives the operand structure that is acceptable, so I can
 * process the operands here as well.
 */
فراغ compile_کد(حرف*label, حرف*mnem, comp_operands_t opa)
{
	/* First, I can give the label a value that is the current
	   codespace pointer. Don't need the text of the label after
	   this is done. */
      إذا (label)
	    compile_codelabel(label);

	/* Lookup the opcode in the opcode table. */
      بنية opcode_جدول_s*op = static_cast<بنية opcode_جدول_s*>
                                            (bsearch(mnem, opcode_جدول, opcode_شمار,
                                                     حجم(بنية opcode_جدول_s),
                                                     &opcode_compare));
      إذا (op == 0) {
	    yyerror("Invalid opcode");
	    compile_errors += 1;
	    أرجع;
      }

      أكد(op);

	/* Build up the code from the information about the opcode and
	   the information from the compiler. */
      vvp_کد_t code = codespace_allocate();
      code->opcode = op->opcode;

      إذا (op->argc != (opa? opa->argc : 0)) {
	    yyerror("operand count");
	    compile_errors += 1;
	    أرجع;
      }

	/* Pull the operands that the instruction expects from the
	   list that the parser supplied. */

      لكل (بیعلامت idx = 0 ;  idx < op->argc ;  idx += 1) {

	    بدل (op->argt[idx]) {
		حالة OA_NONE:
		  اكسر;

		حالة OA_ARR_PTR:
		  إذا (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			اكسر;
		  }

		  compile_array_lookup(code, opa->argv[idx].symb.متن);
		  اكسر;

		حالة OA_BIT1:
		  إذا (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			اكسر;
		  }
		  code->بیت_idx[0] = opa->argv[idx].numb;
		  اكسر;

		حالة OA_BIT2:
		  إذا (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			اكسر;
		  }
		  code->بیت_idx[1] = opa->argv[idx].numb;
		  اكسر;

		حالة OA_کد_PTR:
		حالة OA_کد_PTR2:
		  إذا (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			اكسر;
		  }

		  أكد(opa->argv[idx].symb.idx == 0);
		  کد_label_lookup(code, opa->argv[idx].symb.متن,
				    op->argt[idx]==OA_کد_PTR2);
		  اكسر;

		حالة OA_FUNC_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  إذا (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			اكسر;
		  }

		  functor_ref_lookup(&code->net, opa->argv[idx].symb.متن);
		  اكسر;

		حالة OA_FUNC_PTR2:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  إذا (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			اكسر;
		  }

		  functor_ref_lookup(&code->net2, opa->argv[idx].symb.متن);
		  اكسر;

		حالة OA_عدد:
		  إذا (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			اكسر;
		  }

		  code->عدد = opa->argv[idx].numb;
		  اكسر;

		حالة OA_VPI_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  إذا (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			اكسر;
		  }

		  compile_vpi_lookup(&code->handle, opa->argv[idx].symb.متن);
		  اكسر;

		حالة OA_STRING:
		  إذا (opa->argv[idx].ltype != L_STRING) {
			yyerror("operand format");
			اكسر;
		  }

		  code->متن = opa->argv[idx].متن;
		  اكسر;
	    }
      }

      حرر(opa);

      حرر(mnem);
}

فراغ compile_codelabel(حرف*label)
{
      نماد_مقدار_t val;
      vvp_کد_t ptr = codespace_بعدی();

      val.ptr = ptr;
      sym_مجموعه_مقدار(sym_codespace, label, val);

      حرر(label);
}

فراغ compile_file_سطر(حرف*label, دراز file_idx, دراز lineno,
                       حرف*description)
{
      إذا (label) compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_کد_t code = codespace_allocate();
      code->opcode = &از_FILE_سطر;

	/* Create a vpiHandle that contains the information. */
      code->handle = vpip_build_file_سطر(description, file_idx, lineno);
      أكد(code->handle);

	/* Done with the lexor-allocated name string. */
      حذف[] description;
}

فراغ compile_vpi_call(حرف*label, حرف*name,
                      منطقي func_as_task_err, منطقي func_as_task_warn,
                      دراز file_idx, دراز lineno,
                      بیعلامت argc, vpiHandle*argv,
		      بیعلامت vec4_پشته, بیعلامت real_پشته, بیعلامت string_پشته)
{
      إذا (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_کد_t code = codespace_allocate();
      code->opcode = &از_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, 0, 0,
					 0, func_as_task_err, func_as_task_warn,
                                         argc, argv, vec4_پشته, real_پشته, string_پشته,
					 file_idx, lineno);
      إذا (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      حذف[] name;
}

فراغ compile_vpi_func_call(حرف*label, حرف*name,
			   صحيح val_type, بیعلامت val_wid,
			   دراز file_idx, دراز lineno,
			   بیعلامت argc, vpiHandle*argv,
			   بیعلامت vec4_پشته,
			   بیعلامت real_پشته,
			   بیعلامت string_پشته)
{
      إذا (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_کد_t code = codespace_allocate();
      code->opcode = &از_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, val_type, val_wid,
					 0, درست, خطأ,
                                         argc, argv, vec4_پشته, real_پشته, string_پشته,
					 file_idx, lineno);
      إذا (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      حذف[] name;
}

/*
 * When the parser finds a thread statement, I create a new thread
 * with the start address referenced by the program symbol passed to
 * me.
 */
فراغ compile_thread(حرف*شروع_sym, حرف*flag)
{
      منطقي افزودن_flag = خطأ;

      نماد_مقدار_t tmp = sym_get_مقدار(sym_codespace, شروع_sym);
      vvp_کد_t pc = reinterpret_cast<vvp_کد_t>(tmp.ptr);
      إذا (pc == 0) {
	    yyerror("unresolved address");
	    أرجع;
      }

      إذا (flag && (strcmp(flag,"$push") == 0))
	    افزودن_flag = درست;

      vthread_t thr = vthread_new(pc, vpip_peek_current_scope());

      إذا (flag && (strcmp(flag,"$init") == 0))
	    زمانبندی_init_vthread(thr);
      وإلا إذا (flag && (strcmp(flag,"$final") == 0))
	    زمانبندی_final_vthread(thr);
      وإلا
	    زمانبندی_vthread(thr, 0, افزودن_flag);

      حرر(شروع_sym);
      حرر(flag);
}

فراغ compile_param_logic(حرف*label, ثابت حرف*name, حرف*value, منطقي signed_flag,
                         منطقي local_flag,
                         دراز file_idx, دراز lineno)
{
      vvp_vector4_t value4 = c4string_مقصد_vector4(value);
      vpiHandle obj = vpip_make_دودویی_param(name, value4, signed_flag,
                                             local_flag, file_idx, lineno);
      compile_vpi_نماد(label, obj);
      vpip_attach_مقصد_current_scope(obj);

      حرر(label);
      حرر(value);
}

فراغ compile_param_string(حرف*label, ثابت حرف*name, حرف*value,
                          منطقي local_flag,
                          دراز file_idx, دراز lineno)
{
	// name and value become owned by vpip_make_string_param
      vpiHandle obj = vpip_make_string_param(name, value, local_flag, file_idx, lineno);
      compile_vpi_نماد(label, obj);
      vpip_attach_مقصد_current_scope(obj);

      حرر(label);
}

فراغ compile_param_real(حرف*label, ثابت حرف*name, حرف*value,
                        منطقي local_flag,
                        دراز file_idx, دراز lineno)
{
      مزدوج dvalue = crstring_مقصد_double(value);
      vpiHandle obj = vpip_make_real_param(name, dvalue, local_flag, file_idx, lineno);
      compile_vpi_نماد(label, obj);
      vpip_attach_مقصد_current_scope(obj);

      حرر(label);
      حرر(value);
}

فراغ compile_island(حرف*label, حرف*type)
{
      إذا (strcmp(type,"tran") == 0)
	    compile_island_tran(label);
      وإلا
	    أكد(0);

      حرر(type);
}
