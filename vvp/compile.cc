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

#جيڪڏهن_وصف_ٿيل __MINGW32__
#ضمن <windows.h>
#جيڪڏهن_پورو

# include  "ivl_alloc.h"

بنانشان compile_errors = 0;

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

تعداد عمليو_e {
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
      OA_ڪوڊ_PTR,
      OA_ڪوڊ_PTR2,
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
      vvp_ڪوڊ_fun opcode;

      بنانشان argc;
      تعداد عمليو_e argt[عمليو_MAX];
};

ساكن ثابت بنية opcode_جدول_s opcode_جدول[] = {
      { "%abs/wr", جو_ABS_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add",    جو_جوڙيو,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add/wr", جو_جوڙيو_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%addi",   جو_ADDI,   3,  {OA_BIT1,     OA_BIT2,     OA_عدد} },
      { "%alloc",  جو_ALLOC,  1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%and",    جو_AND,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%and/r",  جو_ANDR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%assign/ar",جو_سونپيو_AR,2,{OA_ARR_PTR,OA_BIT1,     OA_NONE} },
      { "%assign/ar/d",جو_سونپيو_ARD,2,{OA_ARR_PTR,OA_BIT1,  OA_NONE} },
      { "%assign/ar/e",جو_سونپيو_ARE,1,{OA_ARR_PTR,OA_NONE,  OA_NONE} },
      { "%assign/vec4",      جو_سونپيو_VEC4,       2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/a/d",  جو_سونپيو_VEC4_A_D,   3, {OA_ARR_PTR,  OA_BIT1, OA_BIT2} },
      { "%assign/vec4/a/e",  جو_سونپيو_VEC4_A_E,   2, {OA_ARR_PTR,  OA_BIT1, OA_NONE} },
      { "%assign/vec4/d",    جو_سونپيو_VEC4_D,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/e",    جو_سونپيو_VEC4_E,      1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%assign/vec4/off/d",جو_سونپيو_VEC4_OFF_D, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%assign/vec4/off/e",جو_سونپيو_VEC4_OFF_E, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr",  جو_سونپيو_WR, 2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/d",جو_سونپيو_WRD,2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/e",جو_سونپيو_WRE,1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%blend",     جو_BLEND,     0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/str", جو_BLEND_STR, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/wr",  جو_BLEND_WR,  0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%breakpoint", جو_BREAKPOINT, 0,  {OA_NONE, OA_NONE, OA_NONE} },
      { "%callf/obj",       جو_CALLF_OBJ,       2,{OA_ڪوڊ_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/real",      جو_CALLF_REAL,      2,{OA_ڪوڊ_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/str",       جو_CALLF_STR,       2,{OA_ڪوڊ_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/vec4",      جو_CALLF_VEC4,      2,{OA_ڪوڊ_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/void",      جو_CALLF_VOID,      2,{OA_ڪوڊ_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%cassign/link",    جو_CASSIGN_LINK,    2,{OA_FUNC_PTR,OA_FUNC_PTR2,OA_NONE} },
      { "%cassign/vec4",    جو_CASSIGN_VEC4,    1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cassign/vec4/off",جو_CASSIGN_VEC4_OFF,2,{OA_FUNC_PTR,OA_BIT1,     OA_NONE} },
      { "%cassign/wr",  جو_CASSIGN_WR,  1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cast/vec2/dar", جو_CAST_VEC2_DAR, 1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/dar", جو_CAST_VEC4_DAR, 1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/str", جو_CAST_VEC4_STR, 1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%cast2",   جو_CAST2,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/e",   جو_CMPE,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/ne",  جو_CMPNE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/s",   جو_CMPS,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/str", جو_CMPSTR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/u",   جو_CMPU,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/we",  جو_CMPWE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wne", جو_CMPWNE, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wr",  جو_CMPWR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/x",   جو_CMPX,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/z",   جو_CMPZ,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmpi/e",  جو_CMPIE,  3,  {OA_BIT1,     OA_BIT2,     OA_عدد} },
      { "%cmpi/ne", جو_CMPINE, 3,  {OA_BIT1,     OA_BIT2,     OA_عدد} },
      { "%cmpi/s",  جو_CMPIS,  3,  {OA_BIT1,     OA_BIT2,     OA_عدد} },
      { "%cmpi/u",  جو_CMPIU,  3,  {OA_BIT1,     OA_BIT2,     OA_عدد} },
      { "%cmpix/ltu", جو_CMPIX_LTU, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%cmpix/slt0", جو_CMPIX_SLT0, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%concat/str",  جو_CONCAT_STR,  0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concat/vec4", جو_CONCAT_VEC4, 0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concati/str", جو_CONCATI_STR, 1,{OA_STRING,OA_NONE,  OA_NONE} },
      { "%concati/vec4",جو_CONCATI_VEC4,3,{OA_BIT1,  OA_BIT2,  OA_عدد} },
      { "%cvt/rv",   جو_CVT_RV,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/rv/s", جو_CVT_RV_S,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/sr", جو_CVT_SR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/ur", جو_CVT_UR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/vr", جو_CVT_VR, 1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%deassign",جو_DEASSIGN,3,{OA_FUNC_PTR, OA_BIT1,     OA_BIT2} },
      { "%deassign/wr",جو_DEASSIGN_WR,1,{OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%debug/thr",  جو_DEBUG_THR,  1,{OA_STRING,   OA_NONE,     OA_NONE} },
      { "%delay",  جو_DELAY,  2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%delayx", جو_DELAYX, 1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%delete/elem",جو_DELETE_ELEM,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%delete/obj",جو_DELETE_OBJ,1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%delete/prop/elem", جو_DELETE_PROP_ELEM, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%delete/prop/obj",  جو_DELETE_PROP_OBJ,  1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%delete/tail",جو_DELETE_پڇ,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%disable",  جو_DISABLE, 1, {OA_VPI_PTR,OA_NONE,     OA_NONE} },
      { "%disable/flow", جو_DISABLE_FLOW, 1, {OA_VPI_PTR,OA_NONE, OA_NONE} },
      { "%disable/fork",جو_DISABLE_FORK,0,{OA_NONE,OA_NONE,  OA_NONE} },
      { "%div",      جو_DIV,     0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/s",    جو_DIV_S,   0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/wr",   جو_DIV_WR,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/obj",  جو_DUP_OBJ, 0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/real", جو_DUP_REAL,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/vec4", جو_DUP_VEC4,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%end",    جو_پڇاڙي,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl",  جو_EVCTL,  2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/c",جو_EVCTLC, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl/i",جو_EVCTLI, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/s",جو_EVCTLS, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%event",    جو_EVENT,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%event/nb", جو_EVENT_NB, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%flag_get/vec4", جو_FLAG_GET_VEC4, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%flag_inv",      جو_FLAG_INV,      1, {OA_BIT1,   OA_NONE, OA_NONE} },
      { "%flag_mov",      جو_FLAG_MOV,      2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_or",       جو_FLAG_OR,       2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_set/imm",  جو_FLAG_سيٽ_IMM,  2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%flag_set/vec4", جو_FLAG_سيٽ_VEC4, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%force/link",    جو_FORCE_LINK,2,{OA_FUNC_PTR, OA_FUNC_PTR2, OA_NONE} },
      { "%force/vec4",    جو_FORCE_VEC4,    1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%force/vec4/off",جو_FORCE_VEC4_OFF,2,{OA_FUNC_PTR, OA_BIT1,      OA_NONE} },
      { "%force/vec4/off/d",جو_FORCE_VEC4_OFF_D,3,{OA_FUNC_PTR, OA_BIT1,  OA_BIT2} },
      { "%force/wr",      جو_FORCE_WR,      1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%fork",   جو_FORK,   2,  {OA_ڪوڊ_PTR2,OA_VPI_PTR,  OA_NONE} },
      { "%free",   جو_FREE,   1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%inv",    جو_INV,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%ix/add", جو_IX_جوڙيو, 3,  {OA_عدد,   OA_BIT1,     OA_BIT2} },
      { "%ix/getv",جو_IX_GETV,2,  {OA_BIT1,     OA_FUNC_PTR, OA_NONE} },
      { "%ix/getv/s",جو_IX_GETV_S,2, {OA_BIT1,   OA_FUNC_PTR, OA_NONE} },
      { "%ix/load",جو_IX_ڀريو,3,  {OA_عدد,   OA_BIT1,     OA_BIT2} },
      { "%ix/mov", جو_IX_MOV, 2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%ix/mul", جو_IX_MUL, 3,  {OA_عدد,   OA_BIT1,     OA_BIT2} },
      { "%ix/sub", جو_IX_SUB, 3,  {OA_عدد,   OA_BIT1,     OA_BIT2} },
      { "%ix/vec4",  جو_IX_VEC4,  1,  {OA_عدد,   OA_NONE, OA_NONE} },
      { "%ix/vec4/s",جو_IX_VEC4_S,1,  {OA_عدد,   OA_NONE, OA_NONE} },
      { "%jmp",    جو_JMP,    1,  {OA_ڪوڊ_PTR, OA_NONE,     OA_NONE} },
      { "%jmp/0",  جو_JMP0,   2,  {OA_ڪوڊ_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/0xz",جو_JMP0_XZ, 2,  {OA_ڪوڊ_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1",  جو_JMP1,   2,  {OA_ڪوڊ_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1xz",جو_JMP1_XZ, 2,  {OA_ڪوڊ_PTR, OA_BIT1,     OA_NONE} },
      { "%join",   جو_JOIN,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%join/detach",جو_JOIN_DETACH,1,{OA_عدد,OA_NONE,  OA_NONE} },
      { "%load/ar",جو_ڀريو_AR,2,  {OA_ARR_PTR,  OA_BIT1,     OA_NONE} },
      { "%load/dar/r",  جو_ڀريو_DAR_R,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE}},
      { "%load/dar/str",جو_ڀريو_DAR_STR,  1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/dar/vec4",جو_ڀريو_DAR_VEC4,1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/obj",   جو_ڀريو_OBJ,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/obja",  جو_ڀريو_OBJA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/prop/dar/vec4", جو_ڀريو_PROP_DAR_VEC4, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%load/real",  جو_ڀريو_REAL, 1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%load/str",   جو_ڀريو_STR,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/stra",  جو_ڀريو_STRA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/vec4",  جو_ڀريو_VEC4, 1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%load/vec4a", جو_ڀريو_VEC4_A,2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%max/wr", جو_MAX_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%min/wr", جو_MIN_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod",    جو_MOD,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/s",  جو_MOD_S,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/wr", جو_MOD_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul",    جو_MUL,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul/wr", جو_MUL_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%muli",   جو_MULI,   3,  {OA_BIT1,     OA_BIT2,     OA_عدد} },
      { "%nand",   جو_NAND,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nand/r", جو_NANDR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%neg/wr", جو_NEG_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%new/cobj",  جو_NEW_COBJ,  1, {OA_VPI_PTR,OA_NONE,  OA_NONE} },
      { "%new/darray",جو_NEW_DARRAY,2, {OA_BIT1,   OA_STRING,OA_NONE} },
      { "%noop",   جو_NOOP,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor",    جو_NOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor/r",  جو_NORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%null",   جو_NULL,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or",     جو_OR,     0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or/r",   جو_ORR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%pad/s",  جو_PAD_S,  1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%pad/u",  جو_PAD_U,  1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%part/s", جو_PART_S, 1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%part/u", جو_PART_U, 1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%parti/s",جو_PARTI_S,3,  {OA_عدد,   OA_BIT1,     OA_BIT2} },
      { "%parti/u",جو_PARTI_U,3,  {OA_عدد,   OA_BIT1,     OA_BIT2} },
      { "%pop/obj", جو_ڪڍو_OBJ, 2, {OA_BIT1,    OA_BIT2,     OA_NONE} },
      { "%pop/real",جو_ڪڍو_REAL,1, {OA_عدد,  OA_NONE,     OA_NONE} },
      { "%pop/str", جو_ڪڍو_STR, 1, {OA_عدد,  OA_NONE,     OA_NONE} },
      { "%pop/vec4",جو_ڪڍو_VEC4,1, {OA_عدد,  OA_NONE,     OA_NONE} },
      { "%pow",     جو_POW,     0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/s",   جو_POW_S,   0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/wr",  جو_POW_WR,  0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%prop/obj",جو_PROP_OBJ,2,  {OA_عدد,   OA_BIT1,     OA_NONE} },
      { "%prop/queue/size", جو_PROP_قطار_سائيز,  1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%prop/r",  جو_PROP_R,  1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%prop/str",جو_PROP_STR,1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%prop/v",  جو_PROP_V,  1,  {OA_عدد,   OA_NONE,     OA_NONE} },
      { "%push/ix/vec4", جو_ڌڪو_IX_VEC4, 3, {OA_عدد, OA_BIT1, OA_BIT2} },
      { "%pushi/real",جو_PUSHI_REAL,2,{OA_BIT1,   OA_BIT2,   OA_NONE} },
      { "%pushi/str", جو_PUSHI_STR, 1,{OA_STRING, OA_NONE,   OA_NONE} },
      { "%pushi/vec4",جو_PUSHI_VEC4,3,{OA_BIT1,   OA_BIT2,   OA_عدد} },
      { "%pushv/str", جو_PUSHV_STR, 0,{OA_NONE,   OA_NONE,   OA_NONE} },
      { "%putc/str/vec4",جو_PUTC_STR_VEC4,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/prop/r",   جو_QINSERT_PROP_R,   2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/str", جو_QINSERT_PROP_STR, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/v",   جو_QINSERT_PROP_V,   3, {OA_عدد, OA_BIT1, OA_BIT2} },
      { "%qinsert/real",جو_QINSERT_REAL,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/str", جو_QINSERT_STR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/v",   جو_QINSERT_V,   3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%qpop/b/real",جو_QPOP_B_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/str", جو_QPOP_B_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/v",   جو_QPOP_B_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/f/real",جو_QPOP_F_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/str", جو_QPOP_F_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/v",   جو_QPOP_F_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/prop/b/r",   جو_QPOP_PROP_B_REAL, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/str", جو_QPOP_PROP_B_STR, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/v",   جو_QPOP_PROP_B_V,    2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%qpop/prop/f/r",   جو_QPOP_PROP_F_REAL, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/str", جو_QPOP_PROP_F_STR, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/v",   جو_QPOP_PROP_F_V,    2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/append_word/v", جو_قطار_APPEND_لفظ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/find/index/prop/v", جو_قطار_ڳوليو_اشاريو_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/find/index/v", جو_قطار_ڳوليو_اشاريو_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find/prop/v", جو_قطار_ڳوليو_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/find/v", جو_قطار_ڳوليو_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/prop/v", جو_قطار_ڳوليو_پهريون_اشاريو_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/v", جو_قطار_ڳوليو_پهريون_اشاريو_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/prop/v", جو_قطار_ڳوليو_پهريون_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/find_first/v", جو_قطار_ڳوليو_پهريون_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/prop/v", جو_قطار_ڳوليو_آخري_اشاريو_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/v", جو_قطار_ڳوليو_آخري_اشاريو_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/prop/v", جو_قطار_ڳوليو_آخري_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/find_last/v", جو_قطار_ڳوليو_آخري_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/max/obj/v", جو_قطار_MAX_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/max/prop/v", جو_قطار_MAX_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/max/v", جو_قطار_MAX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/min/obj/v", جو_قطار_MIN_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/min/prop/v", جو_قطار_MIN_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/min/v", جو_قطار_MIN_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/new_empty/v", جو_قطار_NEW_خالي_جايون_V, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%queue/size/v", جو_قطار_سائيز_V, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%queue/unique/index/obj/v", جو_قطار_UNIQUE_اشاريو_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/index/prop/v", جو_قطار_UNIQUE_اشاريو_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/unique/index/v", جو_قطار_UNIQUE_اشاريو_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/unique/obj/v", جو_قطار_UNIQUE_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/prop/v", جو_قطار_UNIQUE_PROP_V, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%queue/unique/v", جو_قطار_UNIQUE_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/word/prop/v", جو_قطار_لفظ_PROP_V, 3, {OA_عدد, OA_BIT1, OA_BIT2} },
      { "%queue/word/v", جو_قطار_لفظ_V, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%release/net",جو_RELEASE_NET,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/reg",جو_RELEASE_REG,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/wr", جو_RELEASE_WR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%replicate", جو_REPLICATE,   1,{OA_عدد,  OA_NONE,OA_NONE} },
      { "%ret/real",  جو_RET_REAL,    1,{OA_عدد,  OA_NONE,OA_NONE} },
      { "%ret/str",   جو_RET_STR,     1,{OA_عدد,  OA_NONE,OA_NONE} },
      { "%ret/vec4",  جو_RET_VEC4,    3,{OA_عدد,  OA_BIT1,OA_BIT2} },
      { "%retload/real",جو_RETLOAD_REAL,1,{OA_عدد,  OA_NONE,OA_NONE} },
      { "%retload/str", جو_RETLOAD_STR, 1,{OA_عدد,  OA_NONE,OA_NONE} },
      { "%retload/vec4",جو_RETLOAD_VEC4,1,{OA_عدد,  OA_NONE,OA_NONE} },
      { "%reverse/obj", جو_ابتو_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%reverse/prop/obj", جو_ابتو_PROP_OBJ, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%rsort/obj", جو_RSORT_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%rsort/prop/obj", جو_RSORT_PROP_OBJ, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%scopy",  جو_SCOPY,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%set/dar/obj/real",جو_سيٽ_DAR_OBJ_REAL,1,{OA_عدد,OA_NONE,OA_NONE} },
      { "%set/dar/obj/str", جو_سيٽ_DAR_OBJ_STR, 1,{OA_عدد,OA_NONE,OA_NONE} },
      { "%set/dar/obj/vec4",جو_سيٽ_DAR_OBJ_VEC4,1,{OA_عدد,OA_NONE,OA_NONE} },
      { "%shiftl",   جو_SHIFTL,   1, {OA_عدد, OA_NONE,   OA_NONE} },
      { "%shiftr",   جو_SHIFTR,   1, {OA_عدد, OA_NONE,   OA_NONE} },
      { "%shiftr/s", جو_SHIFTR_S, 1, {OA_عدد, OA_NONE,   OA_NONE} },
      { "%shuffle/obj", جو_SHUFFLE_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%shuffle/prop/obj", جو_SHUFFLE_PROP_OBJ, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%sort/obj", جو_ترتيب_ڏيو_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%sort/prop/obj", جو_ترتيب_ڏيو_PROP_OBJ, 1, {OA_عدد, OA_NONE, OA_NONE} },
      { "%split/vec4",    جو_ورهايو_VEC4,    1,{OA_عدد,   OA_NONE, OA_NONE} },
      { "%store/dar/r",   جو_STORE_DAR_R,   1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/str", جو_STORE_DAR_STR, 1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/vec4",جو_STORE_DAR_VEC4,1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/obj",   جو_STORE_OBJ,   1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/obja",  جو_STORE_OBJA,  2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/prop/obj",جو_STORE_PROP_OBJ,2, {OA_عدد,  OA_BIT1, OA_NONE} },
      { "%store/prop/qb/r",   جو_STORE_PROP_QB_R,   2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/str", جو_STORE_PROP_QB_STR, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/v",   جو_STORE_PROP_QB_V,   3, {OA_عدد, OA_BIT1, OA_BIT2} },
      { "%store/prop/qf/r",   جو_STORE_PROP_QF_R,   2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/str", جو_STORE_PROP_QF_STR, 2, {OA_عدد, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/v",   جو_STORE_PROP_QF_V,   3, {OA_عدد, OA_BIT1, OA_BIT2} },
      { "%store/prop/r",  جو_STORE_PROP_R,  1, {OA_عدد,  OA_NONE, OA_NONE} },
      { "%store/prop/str",جو_STORE_PROP_STR,1, {OA_عدد,  OA_NONE, OA_NONE} },
      { "%store/prop/v",  جو_STORE_PROP_V,  2, {OA_عدد,  OA_BIT1, OA_NONE} },
      { "%store/qb/r",   جو_STORE_QB_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/str", جو_STORE_QB_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/v",   جو_STORE_QB_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qdar/r",  جو_STORE_QDAR_R,  2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/str",جو_STORE_QDAR_STR,2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/v",  جو_STORE_QDAR_V,  3,{OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qf/r",   جو_STORE_QF_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/str", جو_STORE_QF_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/v",   جو_STORE_QF_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qobj/r",  جو_STORE_QOBJ_R,  2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/str",جو_STORE_QOBJ_STR,2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/v",  جو_STORE_QOBJ_V,  3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/real",    جو_STORE_REAL,    1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/reala",   جو_STORE_REALA,   2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/str",     جو_STORE_STR,     1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/stra",    جو_STORE_STRA,    2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/vec4",    جو_STORE_VEC4,    3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/vec4a",   جو_STORE_VEC4_A,   3, {OA_ARR_PTR, OA_BIT1, OA_BIT2} },
      { "%sub",    جو_SUB,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%sub/wr", جو_SUB_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%subi",   جو_SUBI,   3,  {OA_BIT1,     OA_BIT2,     OA_عدد} },
      { "%substr",     جو_SUBSTR,     2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%substr/vec4",جو_SUBSTR_VEC4,2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%test_nul",     جو_TEST_NUL,     1,{OA_FUNC_PTR,OA_NONE,    OA_NONE} },
      { "%test_nul/a",   جو_TEST_NUL_A,   2,{OA_ARR_PTR, OA_BIT1,    OA_NONE} },
      { "%test_nul/obj", جو_TEST_NUL_OBJ, 0,{OA_NONE,    OA_NONE,    OA_NONE} },
      { "%test_nul/prop",جو_TEST_NUL_PROP,2,{OA_عدد,  OA_BIT1,    OA_NONE} },
      { "%uparr/and/v",          جو_UPARR_AND_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/or/v",           جو_UPARR_OR_V,           2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/product/obj/v",  جو_UPARR_حاصل_ضرب_OBJ_V,  1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/product/prop/v", جو_UPARR_حاصل_ضرب_PROP_V, 2, {OA_عدد,   OA_BIT1, OA_NONE} },
      { "%uparr/product/r",      جو_UPARR_حاصل_ضرب_REAL,   1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/product/v",      جو_UPARR_حاصل_ضرب_V,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/sum/obj/v",      جو_UPARR_جوڙ_OBJ_V,      1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/sum/prop/v",     جو_UPARR_جوڙ_PROP_V,     2, {OA_عدد,   OA_BIT1, OA_NONE} },
      { "%uparr/sum/r",          جو_UPARR_جوڙ_REAL,       1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/sum/v",          جو_UPARR_جوڙ_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/xor/v",          جو_UPARR_XOR_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%wait",   جو_WAIT,   1,  {OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%wait/fork",جو_WAIT_FORK,0,{OA_NONE,   OA_NONE,     OA_NONE} },
      { "%xnor",   جو_XNOR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xnor/r", جو_XNORR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor",    جو_XOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor/r",  جو_XORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { 0, جو_NOOP, 0, {OA_NONE, OA_NONE, OA_NONE} }
};

ساكن ثابت بنانشان opcode_ڳڻپ =
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
ساكن نشان_جدول_t sym_codespace = 0;

/*
 * Keep a symbol table of functors mentioned in the source. This table
 * is used to resolve references as they come.
 */
ساكن نشان_جدول_t sym_functors = 0;

/*
 * VPI objects are indexed during compile time so that they can be
 * linked together as they are created. This symbol table matches
 * labels to vpiHandles.
 */
ساكن نشان_جدول_t sym_vpi = 0;


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

فراغ define_functor_نشان(ثابت حرف*label, vvp_net_t*net)
{
      نشان_قدر_t val;
      val.net = net;
      sym_سيٽ_قدر(sym_functors, label, val);
}

ساكن vvp_net_t*lookup_functor_نشان(ثابت حرف*label)
{
      أكد(sym_functors);
      نشان_قدر_t val = sym_get_قدر(sym_functors, label);
      أرجع val.net;
}

vpiHandle vvp_lookup_handle(ثابت حرف*label)
{
      نشان_قدر_t val = sym_get_قدر(sym_vpi, label);
      إذا (val.ptr) أرجع (vpiHandle) val.ptr;
      أرجع 0;
}

vvp_net_t* vvp_net_lookup(ثابت حرف*label)
{
      ساكن منطقي t0_trigger_generated = خطأ;

        /* First, look to see if the symbol is a vpi object of some
	   sort. If it is, then get the vvp_ipoint_t pointer out of
	   the vpiHandle. */
      نشان_قدر_t val = sym_get_قدر(sym_vpi, label);
      إذا (val.ptr) {
	    vpiHandle vpi = static_cast<vpiHandle>(val.ptr);
	    بدل (vpi->get_type_ڪوڊ()) {
		حالة vpiNet:
		حالة vpiReg:
		حالة vpi_ٻٽو_انگ_Var:
		حالة vpiByteVar:
		حالة vpiShortIntVar:
		حالة vpiIntVar:
		حالة vpiLongIntVar:
		حالة vpiIntegerVar: {
		      ثابت __vpiSignal*sig = dynamic_cast<__vpiSignal*>(vpi);
		      أرجع sig->ڳنڍ;
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
		          vpi->get_type_ڪوڊ());
		  أكد(0);
	    }
      }


	/* Failing that, look for a general functor. */
      vvp_net_t*tmp = lookup_functor_نشان(label);

	// This is a special label used to create a T0 trigger for the
	// always_comb/latch processes.
     إذا (! t0_trigger_generated && (strcmp(label, "E_0x0") == 0)) {
	      // This should never happen, but if it does then the E_0x0
	      // event generation may need to be explicitly generated in
	      // the compiler output instead of implicitly in this code.
	    أكد(! vpip_peek_current_scope()->is_automatic());
	    t0_trigger_generated = سچ;
	      // Create a local event with no name for the T0 trigger
	    compile_named_event(strdup(label), strcpy(جديد حرف [1],""), سچ);
	    tmp = vvp_net_lookup(label);
	    أكد(tmp);
	      // Create a trigger for the T0 event.
	    vvp_net_ptr_t ptr (tmp, 0);
	    وقت_جدول_t0_trigger(ptr);
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
	    ميٽيو cur;
	    أرجع;
      }

      cur->ايندڙ_2 = resolv_list;
      resolv_list = cur;
}


/*
 * Look up vvp_nets in the symbol table. The "source" is the label for
 * the net that I want to feed, and net->port[port] is the vvp_net
 * input that I want that node to feed into. When the name is found,
 * put net->port[port] into the fan-out list for that node.
 */
بنية vvp_net_resolv_list_s: عام resolv_list_s {

      واضح vvp_net_resolv_list_s(حرف*l) : resolv_list_s(l) { }
	// port to be driven by the located node.
      vvp_net_ptr_t پورٽ;
      مجازي منطقي resolve(منطقي mes) override;
};

منطقي vvp_net_resolv_list_s::resolve(منطقي mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      إذا (tmp) {
	      // Link the input port to the located output.
	    tmp->link(پورٽ);
	    أرجع سچ;
      }

      إذا (mes)
	    fprintf(stderr, "unresolved vvp_net reference: %s\n", label());

      أرجع خطأ;
}

مضمن ساكن
فراغ postpone_functor_داخلا(vvp_net_ptr_t پورٽ, حرف*lab)
{
      بنية vvp_net_resolv_list_s*res = جديد بنية vvp_net_resolv_list_s(lab);
      res->پورٽ   = پورٽ;

      resolv_submit(res);
}


/*
 *  Generic functor reference lookup.
 */

بنية functor_gen_resolv_list_s: عام resolv_list_s {
      واضح functor_gen_resolv_list_s(حرف*txt) : resolv_list_s(txt) {
	    ref = 0;
      }
      vvp_net_t**ref;
      مجازي منطقي resolve(منطقي mes) override;
};

منطقي functor_gen_resolv_list_s::resolve(منطقي mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      إذا (tmp) {
	    *ref = tmp;
	    أرجع سچ;
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
      واضح vpi_handle_resolv_list_s(حرف*lab) : resolv_list_s(lab) {
	    handle = عدم;
      }
      مجازي منطقي resolve(منطقي mes) override;
      vpiHandle *handle;
};

منطقي vpi_handle_resolv_list_s::resolve(منطقي mes)
{
      نشان_قدر_t val = sym_get_قدر(sym_vpi, label());
      إذا (!val.ptr) {
	    // check for thread access symbols
	    بنانشان بنياد, wid;
	    size_t n = 0;
	    حرف ss[32];
	    إذا (2 == لڙي_مان_پڙهو(label(), "W<%u,%31[r]>%zn", &بنياد, ss, &n)
		       && n == لڙي_ڊيگهه(label())) {

		  val.ptr = vpip_make_vthr_لفظ(بنياد, ss);
		  sym_سيٽ_قدر(sym_vpi, label(), val);

	    } وإلا إذا (1 == لڙي_مان_پڙهو(label(), "S<%u,str>%zn", &بنياد, &n)
		       && n == لڙي_ڊيگهه(label())) {

		  val.ptr = vpip_make_vthr_str_ڍير(بنياد);
		  sym_سيٽ_قدر(sym_vpi, label(), val);

	    } وإلا إذا (3 == لڙي_مان_پڙهو(label(), "S<%u,vec4,%31[su]%u>%zn", &بنياد, ss, &wid, &n)
		       && n == لڙي_ڊيگهه(label())) {

		  منطقي signed_flag = خطأ;
		  لكل (حرف*fp = ss ;  *fp ;  fp += 1) بدل (*fp) {
		      حالة 's':
			signed_flag = سچ;
			اكسر;
		      حالة 'u':
			signed_flag = خطأ;
			اكسر;
		      افتراضي:
			اكسر;
		  }
		  val.ptr = vpip_make_vthr_vec4_ڍير(بنياد, signed_flag, wid);
		  sym_سيٽ_قدر(sym_vpi, label(), val);
	    }

      }

      إذا (!val.ptr) {
	    // check for memory word  M<mem,base,wid>
      }

      إذا (strcmp(label(), "null") == 0) {
	    val.ptr = vpip_make_null_const();
	    sym_سيٽ_قدر(sym_vpi, label(), val);
      }

      إذا (val.ptr) {
	    *handle = static_cast<vpiHandle>(val.ptr);
	    أرجع سچ;
      }

      إذا (mes)
	    fprintf(stderr, "unresolved vpi name lookup: %s\n", label());

      أرجع خطأ;
}

فراغ compile_vpi_lookup(vpiHandle *handle, حرف*label)
{
      إذا (strcmp(label, "$time") == 0) {
	    *handle = vpip_sim_وقت(vpip_peek_current_scope(), خطأ);
	    حرر(label);
	    أرجع;
      }

      إذا (strcmp(label, "$stime") == 0) {
	    *handle = vpip_sim_وقت(vpip_peek_current_scope(), سچ);
	    حرر(label);
	    أرجع;
      }

      إذا (strcmp(label, "$realtime") == 0) {
	    *handle = vpip_sim_realtime(vpip_peek_current_scope());
	    حرر(label);
	    أرجع;
      }

      إذا (strcmp(label, "$simtime") == 0) {
	    *handle = vpip_sim_وقت(0, خطأ);
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

بنية ڪوڊ_label_resolv_list_s: عام resolv_list_s {
      واضح ڪوڊ_label_resolv_list_s(حرف*lab, منطقي cptr2) : resolv_list_s(lab) {
	    code = عدم;
	    cptr2_flag = cptr2;
      }
      بنية vvp_ڪوڊ_s *code;
      منطقي cptr2_flag;
      مجازي منطقي resolve(منطقي mes) override;
};

منطقي ڪوڊ_label_resolv_list_s::resolve(منطقي mes)
{
      نشان_قدر_t val = sym_get_قدر(sym_codespace, label());
      إذا (val.ptr) {
	    إذا (cptr2_flag)
		  code->cptr2 = reinterpret_cast<vvp_ڪوڊ_t>(val.ptr);
	    وإلا
		  code->cptr = reinterpret_cast<vvp_ڪوڊ_t>(val.ptr);
	    أرجع سچ;
      }

      إذا (mes)
	    fprintf(stderr, "unresolved code label: %s\n", label());

      أرجع خطأ;
}

فراغ ڪوڊ_label_lookup(بنية vvp_ڪوڊ_s *code, حرف *label, منطقي cptr2)
{
      بنية ڪوڊ_label_resolv_list_s *res
	    = جديد بنية ڪوڊ_label_resolv_list_s(label, cptr2);

      res->code  = code;

      resolv_submit(res);
}

بنية ڪوڊ_array_resolv_list_s: عام resolv_list_s {
      واضح ڪوڊ_array_resolv_list_s(حرف*lab) : resolv_list_s(lab) {
	    code = عدم;
      }
      بنية vvp_ڪوڊ_s *code;
      مجازي منطقي resolve(منطقي mes) override;
};

منطقي ڪوڊ_array_resolv_list_s::resolve(منطقي mes)
{
      code->array = array_ڳوليو(label());
      إذا (code->array != 0) {
	    أرجع سچ;
      }

      إذا (mes)
	    fprintf(stderr, "Array unresolved: %s\n", label());

      أرجع خطأ;
}

ساكن فراغ compile_array_lookup(بنية vvp_ڪوڊ_s*code, حرف*label)
{
      بنية ڪوڊ_array_resolv_list_s *res
	    = جديد بنية ڪوڊ_array_resolv_list_s(label);

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
      صحيح آخري;

      إذا (verbose_flag) {
	    fprintf(stderr, " ... Linking\n");
	    fflush(stderr);
      }

      افعل {
	    resolv_list_s *res = resolv_list;
	    resolv_list = 0x0;
	    آخري = nerrs == lnerrs;
	    lnerrs = nerrs;
	    nerrs = 0;
	    طالما (res) {
		  resolv_list_s *cur = res;
		  res = res->ايندڙ_2;
		  إذا (cur->resolve(آخري))
			ميٽيو cur;
		  وإلا {
			nerrs++;
			cur->ايندڙ_2 = resolv_list;
			resolv_list = cur;
		  }
	    }
	    إذا (nerrs && آخري)
		  fprintf(stderr,
			  "compile_cleanup: %d unresolved items\n",
			  nerrs);
      } طالما (nerrs && !آخري);

      compile_errors += nerrs;

      إذا (verbose_flag) {
	    fprintf(stderr, " ... Removing symbol tables\n");
	    fflush(stderr);
      }

	/* After compile is complete, the vpi symbol table is no
	   longer needed. VPI objects are located by following
	   scopes. */
      delete_نشان_جدول(sym_vpi);
      sym_vpi = 0;

	/* Don't need the code labels. The instructions have numeric
	   pointers in them, the symbol table is no longer needed. */
      delete_نشان_جدول(sym_codespace);
      sym_codespace = 0;

      delete_نشان_جدول(sym_functors);
      sym_functors = 0;

      delete_udp_symbols();

      compile_island_cleanup();
      compile_array_cleanup();

      إذا (verbose_flag) {
	    fprintf(stderr, " ... Compiletf functions\n");
	    fflush(stderr);
      }

      أكد(vpi_گهڻائي_flag == VPI_گهڻائي_NONE);
      vpi_گهڻائي_flag = VPI_گهڻائي_COMPILETF;

      طالما (! scheduled_compiletf.empty()) {
	    بنية __vpiSysTaskCall*obj = scheduled_compiletf.front();
	    scheduled_compiletf.pop_front();
	    vpip_cur_task = obj;
	    obj->defn->info.compiletf (obj->defn->info.user_معلومات);
	    vpip_cur_task = 0;
      }

      vpi_گهڻائي_flag = VPI_گهڻائي_NONE;
}

فراغ compile_vpi_نشان(ثابت حرف*label, vpiHandle obj)
{
      نشان_قدر_t val;
      val.ptr = obj;
      sym_سيٽ_قدر(sym_vpi, label, val);
}

/*
 * Initialize the compiler by allocation empty symbol tables and
 * initializing the various address spaces.
 */
فراغ compile_init(فراغ)
{
      sym_vpi = new_نشان_جدول();

      sym_functors = new_نشان_جدول();

      sym_codespace = new_نشان_جدول();
      codespace_init();
}

فراغ compile_ڀريو_vpi_module(حرف*name)
{
      vpip_ڀريو_module(name);
      ميٽيو[] name;
}

فراغ compile_vpi_وقت_precision(ڊگهو pre)
{
      vpip_سيٽ_وقت_precision(pre);
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
ساكن منطقي crstring_سرو_test(ثابت حرف*str)
{
      إذا ((str[0] != 'C') && (str[0] != 'c'))
	    أرجع خطأ;
      إذا ((str[1] != 'r') || (str[2] != '<'))
	    أرجع خطأ;

      أرجع سچ;
}

منطقي crstring_test(ثابت حرف*str)
{
      إذا (!crstring_سرو_test(str))
	    أرجع خطأ;
      ثابت حرف*tp = strchr(str, '>');
      إذا (tp == 0) أرجع خطأ;
      إذا (tp[1] != 0) أرجع خطأ;

      إذا ((strspn(str+3, "0123456789abcdefmg")+3) != (size_t)(tp - str))
	  أرجع خطأ;

      أرجع سچ;
}

مزدوج crstring_منزل_double(ثابت حرف*label)
{
      أكد(crstring_سرو_test(label));

      ثابت حرف*cp = label+3;
      أكد(*cp == 'm');
      cp += 1;
      حرف*ep;
      uint64_t mant = strtoull(cp, &ep, 16);
      cp = ep;
      أكد(*cp == 'g');
      cp += 1;
      صحيح قوت_نما = strtoul(cp, 0, 16);

      مزدوج tmp;
      إذا (mant == 0 && قوت_نما == 0x3fff) {
	    tmp = INFINITY;
      } وإلا إذا (mant == 0 && قوت_نما == 0x7fff) {
	    tmp = -INFINITY;
      } وإلا إذا (قوت_نما == 0x3fff) {
	    tmp = nan("");
      } وإلا {
	    مزدوج sign = (قوت_نما & 0x4000)? -1.0 : 1.0;
	    قوت_نما &= 0x1fff;

	    tmp = sign * ldexp((مزدوج)mant, قوت_نما - 0x1000);
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

فراغ داخلا_connect(vvp_net_t*fdx, بنانشان پورٽ, حرف*label)
{
      vvp_net_ptr_t ifdx = vvp_net_ptr_t(fdx, پورٽ);

	/* Is this a vvp_vector4_t constant value? */
      إذا (c4string_test(label)) {

	    vvp_vector4_t tmp = c4string_منزل_vector4(label);

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
		  وقت_جدول_init_ويڪٽر(ifdx, tmp);
	    وإلا
		  وقت_جدول_سيٽ_ويڪٽر(ifdx, tmp);

	    حرر(label);
	    أرجع;
      }

	/* Is this a vvp_vector8_t constant value? */
      إذا (c8string_test(label)) {

	    vvp_vector8_t tmp = c8string_منزل_vector8(label);
	    إذا (label[0] == 'c')
		  وقت_جدول_init_ويڪٽر(ifdx, tmp);
	    وإلا
		  وقت_جدول_سيٽ_ويڪٽر(ifdx, tmp);

	    حرر(label);
	    أرجع;
      }

	/* Handle the Cr<> constant driver, which is a real-value
	   driver. */
      إذا (crstring_test(label)) {

	    مزدوج tmp = crstring_منزل_double(label);

	    إذا (label[0] == 'c')
		  وقت_جدول_init_ويڪٽر(ifdx, tmp);
	    وإلا
		  وقت_جدول_سيٽ_ويڪٽر(ifdx, tmp);
	    حرر(label);
	    أرجع;
      }

	/* Handle the general case that this is a label for a node in
	   the vvp net. This arranges for the label to be preserved in
	   a linker list, and linked when the symbol table is
	   complete. */
      postpone_functor_داخلا(ifdx, label);
}

فراغ inputs_connect(vvp_net_t*fdx, بنانشان argc, بنية symb_s*argv)
{
      إذا (argc > 4) {
	    std::cerr << "XXXX argv[0] = " << argv[0].متن << std::سٽ_پڄاڻي;
      }
      أكد(argc <= 4);

      لكل (بنانشان idx = 0;  idx < argc;  idx += 1) {

	    داخلا_connect(fdx, idx, argv[idx].متن);
      }
}

فراغ wide_inputs_connect(vvp_wide_fun_core*core,
			 بنانشان argc, بنية symb_s*argv)
{
	/* Create input functors to receive values from the
	   network. These functors pass the data to the core. */
      بنانشان داخلا_functors = (argc+3) / 4;
      لكل (بنانشان idx = 0 ;  idx < داخلا_functors ;  idx += 1) {
	    بنانشان بنياد = idx*4;
	    بنانشان trans = 4;
	    إذا (بنياد+trans > argc)
		  trans = argc - بنياد;

	    vvp_wide_fun_t*cur = جديد vvp_wide_fun_t(core, بنياد);
	    vvp_net_t*ptr = جديد vvp_net_t;
	    ptr->fun = cur;

	    inputs_connect(ptr, trans, argv+بنياد);
      }
}

سانچو <صنف T_> فراغ make_arith(T_ *arith, حرف*label,
				    بنانشان argc, بنية symb_s*argv)
{
      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = arith;

      define_functor_نشان(label, ptr);
      حرر(label);

      أكد(argc == 2);
      inputs_connect(ptr, argc, argv);

      حرر(argv);
}

فراغ compile_arith_cast_int(حرف*label, ڊگهو width,
                            بنانشان argc, بنية symb_s*argv)
{
      vvp_arith_cast_int*arith = جديد vvp_arith_cast_int((بنانشان) width);

      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = arith;

      define_functor_نشان(label, ptr);
      حرر(label);

      أكد(argc == 1);
      inputs_connect(ptr, argc, argv);
      حرر(argv);
}

فراغ compile_arith_cast_vec2(حرف*label, ڊگهو width,
			     بنانشان argc, بنية symb_s*argv)
{
      vvp_arith_cast_vec2*arith = جديد vvp_arith_cast_vec2((بنانشان) width);

      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = arith;

      define_functor_نشان(label, ptr);
      حرر(label);

      أكد(argc == 1);
      inputs_connect(ptr, argc, argv);
      حرر(argv);
}

فراغ compile_arith_cast_real(حرف*label, منطقي signed_flag,
                             بنانشان argc, بنية symb_s*argv)
{
      vvp_arith_cast_real*arith = جديد vvp_arith_cast_real(signed_flag);

      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = arith;

      define_functor_نشان(label, ptr);
      حرر(label);

      أكد(argc == 1);
      inputs_connect(ptr, argc, argv);
      حرر(argv);
}

فراغ compile_arith_abs(حرف*label, بنانشان argc, بنية symb_s*argv)
{
      vvp_arith_abs*arith = جديد vvp_arith_abs;

      vvp_net_t* ptr = جديد vvp_net_t;
      ptr->fun = arith;

      define_functor_نشان(label, ptr);
      حرر(label);

      أكد(argc == 1);
      inputs_connect(ptr, argc, argv);
      حرر(argv);
}

فراغ compile_arith_div(حرف*label, ڊگهو wid, منطقي signed_flag,
		       بنانشان argc, بنية symb_s*argv)
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

فراغ compile_arith_div_r(حرف*label, بنانشان argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s; .arith/divr has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_arith_div_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_arith_mod(حرف*label, ڊگهو wid, منطقي signed_flag,
		       بنانشان argc, بنية symb_s*argv)
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

فراغ compile_arith_mod_r(حرف*label, بنانشان argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .arith/mod.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_arith_mod_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_arith_mult(حرف*label, ڊگهو wid,
			بنانشان argc, بنية symb_s*argv)
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

فراغ compile_arith_mult_r(حرف*label, بنانشان argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .arith/mult.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_arith_mult_real;
      make_arith(arith, label, argc, argv);
}


فراغ compile_arith_pow(حرف*label, ڊگهو wid, منطقي signed_flag,
		       بنانشان argc, بنية symb_s*argv)
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

فراغ compile_arith_pow_r(حرف*label, بنانشان argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .arith/pow.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_arith_pow_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_arith_sub(حرف*label, ڊگهو wid, بنانشان argc, بنية symb_s*argv)
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

فراغ compile_arith_sub_r(حرف*label, بنانشان argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s; .arith/sub.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_arith_sub_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_arith_جوڙ(حرف*label, ڊگهو wid, بنانشان argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      إذا (argc != 2) {
	    fprintf(stderr, "%s .arith/sum has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_ *arith = جديد vvp_arith_جوڙ(wid);
      make_arith(arith, label, argc, argv);
}

فراغ compile_arith_جوڙ_r(حرف*label, بنانشان argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .arith/sum.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_arith_جوڙ_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_eeq(حرف*label, ڊگهو wid,
		     بنانشان argc, بنية symb_s*argv)
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

فراغ compile_cmp_nee(حرف*label, ڊگهو wid,
		     بنانشان argc, بنية symb_s*argv)
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

فراغ compile_cmp_eq(حرف*label, ڊگهو wid, بنانشان argc, بنية symb_s*argv)
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

فراغ compile_cmp_eqx(حرف*label, ڊگهو wid, بنانشان argc, بنية symb_s*argv)
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

فراغ compile_cmp_eqz(حرف*label, ڊگهو wid, بنانشان argc, بنية symb_s*argv)
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

فراغ compile_cmp_eq_r(حرف*label, بنانشان argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_cmp_eq_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_ne(حرف*label, ڊگهو wid, بنانشان argc, بنية symb_s*argv)
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

فراغ compile_cmp_ne_r(حرف*label, بنانشان argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_cmp_ne_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_ge(حرف*label, ڊگهو wid, منطقي signed_flag,
		    بنانشان argc, بنية symb_s*argv)
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

فراغ compile_cmp_ge_r(حرف*label, بنانشان argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_cmp_ge_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_gt(حرف*label, ڊگهو wid, منطقي signed_flag,
		    بنانشان argc, بنية symb_s*argv)
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

فراغ compile_cmp_gt_r(حرف*label, بنانشان argc, بنية symb_s*argv)
{
      إذا (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    أرجع;
      }

      vvp_arith_real_ *arith = جديد vvp_cmp_gt_real;
      make_arith(arith, label, argc, argv);
}

فراغ compile_cmp_weq(حرف*label, ڊگهو wid,
		     بنانشان argc, بنية symb_s*argv)
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

فراغ compile_cmp_wne(حرف*label, ڊگهو wid,
		     بنانشان argc, بنية symb_s*argv)
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


فراغ compile_delay(حرف*label, بنانشان width,
                   vvp_delay_t*delay, بنية symb_s arg)
{
      vvp_net_t*net = جديد vvp_net_t;
      vvp_fun_delay*obj = جديد vvp_fun_delay(net, width, *delay);
      net->fun = obj;

      ميٽيو delay;

      داخلا_connect(net, 0, arg.متن);

      define_functor_نشان(label, net);
      حرر(label);
}

فراغ compile_delay(حرف*label, بنانشان width,
                   بنانشان argc, بنية symb_s*argv, منطقي ignore_decay)
{
      vvp_delay_t stub (0, 0, 0);
      إذا (ignore_decay) stub.سيٽ_ignore_decay();
      vvp_net_t*net = جديد vvp_net_t;
      vvp_fun_delay*obj = جديد vvp_fun_delay(net, width, stub);
      net->fun = obj;

      inputs_connect(net, argc, argv);
      حرر(argv);

      define_functor_نشان(label, net);
      حرر(label);
}

/*
 * Extend nodes.
 */
فراغ compile_extend_signed(حرف*label, ڊگهو wid, بنية symb_s arg)
{
      أكد(wid >= 0);

      vvp_fun_extend_signed*fun = جديد vvp_fun_extend_signed(wid);
      vvp_net_t*ptr = جديد vvp_net_t;
      ptr->fun = fun;

      define_functor_نشان(label, ptr);
      حرر(label);

      داخلا_connect(ptr, 0, arg.متن);
}

بنية __vpiModPath* compile_modpath(حرف*label, بنانشان width,
                                     بنية symb_s drv, بنية symb_s dest)
{
      vvp_net_t*net = جديد vvp_net_t;
      vvp_fun_modpath*obj = جديد vvp_fun_modpath(net, width);
      net->fun = obj;

      داخلا_connect(net, 0, drv.متن);

      define_functor_نشان(label, net);

      __vpiModPath*modpath = vpip_make_modpath(net);

      compile_vpi_lookup(&modpath->رستو_رڪن_ٻاهريون.اظهار, dest.متن);

      حرر(label);

      modpath->modpath = obj;
      أرجع modpath;
}

ساكن بنية __vpiModPathSrc*make_modpath_src(بنية __vpiModPath*رستو,
					       حرف ڪنارو_c,
					       ثابت بنية symb_s&src,
					       بنية numbv_s&vals,
					       منطقي ifnone)
{
      vvp_fun_modpath*dst = رستو->modpath;

      vvp_time64_t use_delay[12];
      أكد(vals.cnt == 12);
      لكل (بنانشان idx = 0 ; idx < vals.cnt ;  idx += 1) {
	    use_delay[idx] = vals.nvec[idx];
      }

      numbv_clear(&vals);

      vvp_fun_modpath_src*obj = 0;

      صحيح vpi_ڪنارو = vpi_No_ڪنارو;
      إذا (ڪنارو_c == 0) {
	    obj = جديد vvp_fun_modpath_src(use_delay);

      } وإلا {
	    منطقي posedge, negedge;
	    بدل (ڪنارو_c) {
		حالة '+':
		  vpi_ڪنارو = vpiPosedge;
		  posedge = سچ;
		  negedge = خطأ;
		  اكسر;
		حالة '-':
		  vpi_ڪنارو = vpiNegedge;
		  posedge = خطأ;
		  negedge = سچ;
		  اكسر;
#إذا 0
		حالة '*':
		  posedge = سچ;
		  negedge = سچ;
		  اكسر;
#جيڪڏهن_پورو
		افتراضي:
		  posedge = خطأ;
		  negedge = خطأ;
		  fprintf(stderr, "Unknown edge identifier %c(%d).\n", ڪنارو_c,
		          ڪنارو_c);
		  أكد(0);
	    }
	    obj = جديد vvp_fun_modpath_ڪنارو(use_delay, posedge, negedge);
      }

      vvp_net_t*net = جديد vvp_net_t;
      بنية __vpiModPathSrc* srcobj = vpip_make_modpath_src(رستو, net) ;
      vpip_attach_منزل_current_scope(srcobj);
      net->fun = obj;

	/* Save the vpiEdge directory into the input path term. */
      srcobj->رستو_رڪن_اندريون.ڪنارو = vpi_ڪنارو;
      داخلا_connect(net, 0, src.متن);
      dst->جوڙيو_modpath_src(obj, ifnone);

      أرجع srcobj;
}

فراغ compile_modpath_src(بنية __vpiModPath*dst, حرف ڪنارو_c,
			 ثابت بنية symb_s&src,
			 بنية numbv_s&vals,
			 ثابت بنية symb_s&condit_src,
			 ثابت بنية symb_s&رستو_رڪن_اندريون)
{
      بنية __vpiModPathSrc*obj =
	    make_modpath_src(dst, ڪنارو_c, src, vals, خطأ);
      داخلا_connect(obj->net, 1, condit_src.متن);
      compile_vpi_lookup(&obj->رستو_رڪن_اندريون.اظهار, رستو_رڪن_اندريون.متن);
}

فراغ compile_modpath_src(بنية __vpiModPath*dst, حرف ڪنارو_c,
			 ثابت بنية symb_s&src,
			 بنية numbv_s&vals,
			 صحيح condit_src,
			 ثابت بنية symb_s&رستو_رڪن_اندريون,
			 منطقي ifnone)
{
      أكد(condit_src == 0);
      بنية __vpiModPathSrc*obj =
	    make_modpath_src(dst, ڪنارو_c, src, vals, ifnone);
      compile_vpi_lookup(&obj->رستو_رڪن_اندريون.اظهار, رستو_رڪن_اندريون.متن);
}

/*
 * A .shift/l statement creates an array of functors for the
 * width. The 0 input is the data vector to be shifted and the 1 input
 * is the amount of the shift. An unconnected shift amount is set to 0.
 */
فراغ compile_shiftl(حرف*label, ڊگهو wid, بنانشان argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      vvp_arith_ *arith = جديد vvp_shiftl(wid);
      make_arith(arith, label, argc, argv);
}

فراغ compile_shiftr(حرف*label, ڊگهو wid, منطقي signed_flag,
		    بنانشان argc, بنية symb_s*argv)
{
      أكد( wid > 0 );

      vvp_arith_ *arith = جديد vvp_shiftr(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

فراغ compile_resolver(حرف*label, حرف*type, بنانشان argc, بنية symb_s*argv)
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
            ميٽيو net;
      }

      إذا (core) {
	    net->fun = core;
	    define_functor_نشان(label, net);

            لكل (بنانشان بنياد = 0 ;  بنياد < argc ;  بنياد += 4) {
	          بنانشان nports = argc - بنياد;
                  إذا (nports > 4)
                        nports = 4;

                  إذا (بنياد > 0) {
                        net = جديد vvp_net_t;
                        net->fun = جديد resolv_extend(core, بنياد);
                  }
                  inputs_connect(net, nports, argv+بنياد);
            }
      }
      حرر(type);
      حرر(label);
      حرر(argv);
}

فراغ compile_udp_def(صحيح sequ, حرف *label, حرف *name,
		     بنانشان nin, بنانشان init, حرف **جدول)
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

حرف **compile_udp_جدول(حرف **جدول, حرف *صف)
{
  إذا (جدول)
    أكد(لڙي_ڊيگهه(*جدول)==لڙي_ڊيگهه(صف));

  حرف **tt;
  لكل (tt = جدول; tt && *tt; tt++) { }
  صحيح n = (tt-جدول) + 2;

  جدول = static_cast<حرف**>(realloc(جدول, n*حجم(حرف*)));
  جدول[n-2] = صف;
  جدول[n-1] = 0x0;

  أرجع جدول;
}


/*
 * The parser uses this function to compile and link an executable
 * opcode. I do this by looking up the opcode in the opcode_table. The
 * table gives the operand structure that is acceptable, so I can
 * process the operands here as well.
 */
فراغ compile_ڪوڊ(حرف*label, حرف*mnem, comp_operands_t opa)
{
	/* First, I can give the label a value that is the current
	   codespace pointer. Don't need the text of the label after
	   this is done. */
      إذا (label)
	    compile_codelabel(label);

	/* Lookup the opcode in the opcode table. */
      بنية opcode_جدول_s*op = static_cast<بنية opcode_جدول_s*>
                                            (bsearch(mnem, opcode_جدول, opcode_ڳڻپ,
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
      vvp_ڪوڊ_t code = codespace_allocate();
      code->opcode = op->opcode;

      إذا (op->argc != (opa? opa->argc : 0)) {
	    yyerror("operand count");
	    compile_errors += 1;
	    أرجع;
      }

	/* Pull the operands that the instruction expects from the
	   list that the parser supplied. */

      لكل (بنانشان idx = 0 ;  idx < op->argc ;  idx += 1) {

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
		  code->ٻٽو_انگ_idx[0] = opa->argv[idx].numb;
		  اكسر;

		حالة OA_BIT2:
		  إذا (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			اكسر;
		  }
		  code->ٻٽو_انگ_idx[1] = opa->argv[idx].numb;
		  اكسر;

		حالة OA_ڪوڊ_PTR:
		حالة OA_ڪوڊ_PTR2:
		  إذا (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			اكسر;
		  }

		  أكد(opa->argv[idx].symb.idx == 0);
		  ڪوڊ_label_lookup(code, opa->argv[idx].symb.متن,
				    op->argt[idx]==OA_ڪوڊ_PTR2);
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
      نشان_قدر_t val;
      vvp_ڪوڊ_t ptr = codespace_ايندڙ();

      val.ptr = ptr;
      sym_سيٽ_قدر(sym_codespace, label, val);

      حرر(label);
}

فراغ compile_file_سٽ(حرف*label, ڊگهو file_idx, ڊگهو lineno,
                       حرف*description)
{
      إذا (label) compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_ڪوڊ_t code = codespace_allocate();
      code->opcode = &جو_FILE_سٽ;

	/* Create a vpiHandle that contains the information. */
      code->handle = vpip_build_file_سٽ(description, file_idx, lineno);
      أكد(code->handle);

	/* Done with the lexor-allocated name string. */
      ميٽيو[] description;
}

فراغ compile_vpi_call(حرف*label, حرف*name,
                      منطقي func_as_task_err, منطقي func_as_task_warn,
                      ڊگهو file_idx, ڊگهو lineno,
                      بنانشان argc, vpiHandle*argv,
		      بنانشان vec4_ڍير, بنانشان real_ڍير, بنانشان string_ڍير)
{
      إذا (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_ڪوڊ_t code = codespace_allocate();
      code->opcode = &جو_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, 0, 0,
					 0, func_as_task_err, func_as_task_warn,
                                         argc, argv, vec4_ڍير, real_ڍير, string_ڍير,
					 file_idx, lineno);
      إذا (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      ميٽيو[] name;
}

فراغ compile_vpi_func_call(حرف*label, حرف*name,
			   صحيح val_type, بنانشان val_wid,
			   ڊگهو file_idx, ڊگهو lineno,
			   بنانشان argc, vpiHandle*argv,
			   بنانشان vec4_ڍير,
			   بنانشان real_ڍير,
			   بنانشان string_ڍير)
{
      إذا (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_ڪوڊ_t code = codespace_allocate();
      code->opcode = &جو_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, val_type, val_wid,
					 0, سچ, خطأ,
                                         argc, argv, vec4_ڍير, real_ڍير, string_ڍير,
					 file_idx, lineno);
      إذا (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      ميٽيو[] name;
}

/*
 * When the parser finds a thread statement, I create a new thread
 * with the start address referenced by the program symbol passed to
 * me.
 */
فراغ compile_thread(حرف*شروعات_sym, حرف*flag)
{
      منطقي ڌڪو_flag = خطأ;

      نشان_قدر_t tmp = sym_get_قدر(sym_codespace, شروعات_sym);
      vvp_ڪوڊ_t pc = reinterpret_cast<vvp_ڪوڊ_t>(tmp.ptr);
      إذا (pc == 0) {
	    yyerror("unresolved address");
	    أرجع;
      }

      إذا (flag && (strcmp(flag,"$push") == 0))
	    ڌڪو_flag = سچ;

      vthread_t thr = vthread_new(pc, vpip_peek_current_scope());

      إذا (flag && (strcmp(flag,"$init") == 0))
	    وقت_جدول_init_vthread(thr);
      وإلا إذا (flag && (strcmp(flag,"$final") == 0))
	    وقت_جدول_final_vthread(thr);
      وإلا
	    وقت_جدول_vthread(thr, 0, ڌڪو_flag);

      حرر(شروعات_sym);
      حرر(flag);
}

فراغ compile_param_logic(حرف*label, ثابت حرف*name, حرف*value, منطقي signed_flag,
                         منطقي local_flag,
                         ڊگهو file_idx, ڊگهو lineno)
{
      vvp_vector4_t value4 = c4string_منزل_vector4(value);
      vpiHandle obj = vpip_make_ٻٽو_param(name, value4, signed_flag,
                                             local_flag, file_idx, lineno);
      compile_vpi_نشان(label, obj);
      vpip_attach_منزل_current_scope(obj);

      حرر(label);
      حرر(value);
}

فراغ compile_param_string(حرف*label, ثابت حرف*name, حرف*value,
                          منطقي local_flag,
                          ڊگهو file_idx, ڊگهو lineno)
{
	// name and value become owned by vpip_make_string_param
      vpiHandle obj = vpip_make_string_param(name, value, local_flag, file_idx, lineno);
      compile_vpi_نشان(label, obj);
      vpip_attach_منزل_current_scope(obj);

      حرر(label);
}

فراغ compile_param_real(حرف*label, ثابت حرف*name, حرف*value,
                        منطقي local_flag,
                        ڊگهو file_idx, ڊگهو lineno)
{
      مزدوج dvalue = crstring_منزل_double(value);
      vpiHandle obj = vpip_make_real_param(name, dvalue, local_flag, file_idx, lineno);
      compile_vpi_نشان(label, obj);
      vpip_attach_منزل_current_scope(obj);

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
