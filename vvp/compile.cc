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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MINGW32__
#ਅਨ੍ਤਰ੍ਭਾਵ <windows.h>
#ਜੇ_ਅੰਤ

# include  "ivl_alloc.h"

ਬਿਨਾਨਿਸ਼ਾਨੀ compile_errors = 0;

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

ਗਿਣਤੀ ਸੰਕਾਰਜ_e {
	/* Place holder for unused operand */
      OA_NONE,
	/* The operand is a number, an immediate unsigned integer */
      OA_ਸੰਖਿਆ,
	/* The operand is a pointer to an array. */
      OA_ARR_PTR,
	/* The operand is a thread bit index or short integer */
      OA_BIT1,
      OA_BIT2,
	/* The operand is a pointer to code space */
      OA_ਕੋਡ_PTR,
      OA_ਕੋਡ_PTR2,
	/* The operand is a variable or net pointer */
      OA_FUNC_PTR,
	/* The operand is a second functor pointer */
      OA_FUNC_PTR2,
	/* The operand is a VPI handle */
      OA_VPI_PTR,
	/* String */
      OA_STRING
};

ਬਣਤਰ opcode_ਸਾਰਣੀ_s {
      ਅਚਲ ਅੱਖਰ*mnemonic;
      vvp_ਕੋਡ_fun opcode;

      ਬਿਨਾਨਿਸ਼ਾਨੀ argc;
      ਗਿਣਤੀ ਸੰਕਾਰਜ_e argt[ਸੰਕਾਰਜ_MAX];
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ opcode_ਸਾਰਣੀ_s opcode_ਸਾਰਣੀ[] = {
      { "%abs/wr", ਦਾ_ABS_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add",    ਦਾ_ਜੋੜੋ,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add/wr", ਦਾ_ਜੋੜੋ_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%addi",   ਦਾ_ADDI,   3,  {OA_BIT1,     OA_BIT2,     OA_ਸੰਖਿਆ} },
      { "%alloc",  ਦਾ_ALLOC,  1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%and",    ਦਾ_AND,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%and/r",  ਦਾ_ANDR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%assign/ar",ਦਾ_ਸੌਂਪੋ_AR,2,{OA_ARR_PTR,OA_BIT1,     OA_NONE} },
      { "%assign/ar/d",ਦਾ_ਸੌਂਪੋ_ARD,2,{OA_ARR_PTR,OA_BIT1,  OA_NONE} },
      { "%assign/ar/e",ਦਾ_ਸੌਂਪੋ_ARE,1,{OA_ARR_PTR,OA_NONE,  OA_NONE} },
      { "%assign/vec4",      ਦਾ_ਸੌਂਪੋ_VEC4,       2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/a/d",  ਦਾ_ਸੌਂਪੋ_VEC4_A_D,   3, {OA_ARR_PTR,  OA_BIT1, OA_BIT2} },
      { "%assign/vec4/a/e",  ਦਾ_ਸੌਂਪੋ_VEC4_A_E,   2, {OA_ARR_PTR,  OA_BIT1, OA_NONE} },
      { "%assign/vec4/d",    ਦਾ_ਸੌਂਪੋ_VEC4_D,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/e",    ਦਾ_ਸੌਂਪੋ_VEC4_E,      1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%assign/vec4/off/d",ਦਾ_ਸੌਂਪੋ_VEC4_OFF_D, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%assign/vec4/off/e",ਦਾ_ਸੌਂਪੋ_VEC4_OFF_E, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr",  ਦਾ_ਸੌਂਪੋ_WR, 2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/d",ਦਾ_ਸੌਂਪੋ_WRD,2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/e",ਦਾ_ਸੌਂਪੋ_WRE,1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%blend",     ਦਾ_BLEND,     0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/str", ਦਾ_BLEND_STR, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/wr",  ਦਾ_BLEND_WR,  0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%breakpoint", ਦਾ_BREAKPOINT, 0,  {OA_NONE, OA_NONE, OA_NONE} },
      { "%callf/obj",       ਦਾ_CALLF_OBJ,       2,{OA_ਕੋਡ_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/real",      ਦਾ_CALLF_REAL,      2,{OA_ਕੋਡ_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/str",       ਦਾ_CALLF_STR,       2,{OA_ਕੋਡ_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/vec4",      ਦਾ_CALLF_VEC4,      2,{OA_ਕੋਡ_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/void",      ਦਾ_CALLF_VOID,      2,{OA_ਕੋਡ_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%cassign/link",    ਦਾ_CASSIGN_LINK,    2,{OA_FUNC_PTR,OA_FUNC_PTR2,OA_NONE} },
      { "%cassign/vec4",    ਦਾ_CASSIGN_VEC4,    1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cassign/vec4/off",ਦਾ_CASSIGN_VEC4_OFF,2,{OA_FUNC_PTR,OA_BIT1,     OA_NONE} },
      { "%cassign/wr",  ਦਾ_CASSIGN_WR,  1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cast/vec2/dar", ਦਾ_CAST_VEC2_DAR, 1,  {OA_ਸੰਖਿਆ,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/dar", ਦਾ_CAST_VEC4_DAR, 1,  {OA_ਸੰਖਿਆ,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/str", ਦਾ_CAST_VEC4_STR, 1,  {OA_ਸੰਖਿਆ,   OA_NONE,     OA_NONE} },
      { "%cast2",   ਦਾ_CAST2,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/e",   ਦਾ_CMPE,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/ne",  ਦਾ_CMPNE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/s",   ਦਾ_CMPS,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/str", ਦਾ_CMPSTR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/u",   ਦਾ_CMPU,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/we",  ਦਾ_CMPWE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wne", ਦਾ_CMPWNE, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wr",  ਦਾ_CMPWR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/x",   ਦਾ_CMPX,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/z",   ਦਾ_CMPZ,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmpi/e",  ਦਾ_CMPIE,  3,  {OA_BIT1,     OA_BIT2,     OA_ਸੰਖਿਆ} },
      { "%cmpi/ne", ਦਾ_CMPINE, 3,  {OA_BIT1,     OA_BIT2,     OA_ਸੰਖਿਆ} },
      { "%cmpi/s",  ਦਾ_CMPIS,  3,  {OA_BIT1,     OA_BIT2,     OA_ਸੰਖਿਆ} },
      { "%cmpi/u",  ਦਾ_CMPIU,  3,  {OA_BIT1,     OA_BIT2,     OA_ਸੰਖਿਆ} },
      { "%cmpix/ltu", ਦਾ_CMPIX_LTU, 2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%cmpix/slt0", ਦਾ_CMPIX_SLT0, 1, {OA_ਸੰਖਿਆ, OA_NONE, OA_NONE} },
      { "%concat/str",  ਦਾ_CONCAT_STR,  0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concat/vec4", ਦਾ_CONCAT_VEC4, 0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concati/str", ਦਾ_CONCATI_STR, 1,{OA_STRING,OA_NONE,  OA_NONE} },
      { "%concati/vec4",ਦਾ_CONCATI_VEC4,3,{OA_BIT1,  OA_BIT2,  OA_ਸੰਖਿਆ} },
      { "%cvt/rv",   ਦਾ_CVT_RV,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/rv/s", ਦਾ_CVT_RV_S,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/sr", ਦਾ_CVT_SR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/ur", ਦਾ_CVT_UR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/vr", ਦਾ_CVT_VR, 1,  {OA_ਸੰਖਿਆ,   OA_NONE,     OA_NONE} },
      { "%deassign",ਦਾ_DEASSIGN,3,{OA_FUNC_PTR, OA_BIT1,     OA_BIT2} },
      { "%deassign/wr",ਦਾ_DEASSIGN_WR,1,{OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%debug/thr",  ਦਾ_DEBUG_THR,  1,{OA_STRING,   OA_NONE,     OA_NONE} },
      { "%delay",  ਦਾ_DELAY,  2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%delayx", ਦਾ_DELAYX, 1,  {OA_ਸੰਖਿਆ,   OA_NONE,     OA_NONE} },
      { "%delete/elem",ਦਾ_DELETE_ELEM,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%delete/obj",ਦਾ_DELETE_OBJ,1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%delete/prop/elem", ਦਾ_DELETE_PROP_ELEM, 1, {OA_ਸੰਖਿਆ, OA_NONE, OA_NONE} },
      { "%delete/prop/obj",  ਦਾ_DELETE_PROP_OBJ,  1, {OA_ਸੰਖਿਆ, OA_NONE, OA_NONE} },
      { "%delete/tail",ਦਾ_DELETE_ਪੂਛ,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%disable",  ਦਾ_DISABLE, 1, {OA_VPI_PTR,OA_NONE,     OA_NONE} },
      { "%disable/flow", ਦਾ_DISABLE_FLOW, 1, {OA_VPI_PTR,OA_NONE, OA_NONE} },
      { "%disable/fork",ਦਾ_DISABLE_FORK,0,{OA_NONE,OA_NONE,  OA_NONE} },
      { "%div",      ਦਾ_DIV,     0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/s",    ਦਾ_DIV_S,   0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/wr",   ਦਾ_DIV_WR,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/obj",  ਦਾ_DUP_OBJ, 0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/real", ਦਾ_DUP_REAL,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/vec4", ਦਾ_DUP_VEC4,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%end",    ਦਾ_ਸਿਰਾ,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl",  ਦਾ_EVCTL,  2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/c",ਦਾ_EVCTLC, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl/i",ਦਾ_EVCTLI, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/s",ਦਾ_EVCTLS, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%event",    ਦਾ_EVENT,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%event/nb", ਦਾ_EVENT_NB, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%flag_get/vec4", ਦਾ_FLAG_GET_VEC4, 1, {OA_ਸੰਖਿਆ, OA_NONE, OA_NONE} },
      { "%flag_inv",      ਦਾ_FLAG_INV,      1, {OA_BIT1,   OA_NONE, OA_NONE} },
      { "%flag_mov",      ਦਾ_FLAG_MOV,      2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_or",       ਦਾ_FLAG_OR,       2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_set/imm",  ਦਾ_FLAG_ਸੈੱਟ_IMM,  2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%flag_set/vec4", ਦਾ_FLAG_ਸੈੱਟ_VEC4, 1, {OA_ਸੰਖਿਆ, OA_NONE, OA_NONE} },
      { "%force/link",    ਦਾ_FORCE_LINK,2,{OA_FUNC_PTR, OA_FUNC_PTR2, OA_NONE} },
      { "%force/vec4",    ਦਾ_FORCE_VEC4,    1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%force/vec4/off",ਦਾ_FORCE_VEC4_OFF,2,{OA_FUNC_PTR, OA_BIT1,      OA_NONE} },
      { "%force/vec4/off/d",ਦਾ_FORCE_VEC4_OFF_D,3,{OA_FUNC_PTR, OA_BIT1,  OA_BIT2} },
      { "%force/wr",      ਦਾ_FORCE_WR,      1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%fork",   ਦਾ_FORK,   2,  {OA_ਕੋਡ_PTR2,OA_VPI_PTR,  OA_NONE} },
      { "%free",   ਦਾ_FREE,   1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%inv",    ਦਾ_INV,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%ix/add", ਦਾ_IX_ਜੋੜੋ, 3,  {OA_ਸੰਖਿਆ,   OA_BIT1,     OA_BIT2} },
      { "%ix/getv",ਦਾ_IX_GETV,2,  {OA_BIT1,     OA_FUNC_PTR, OA_NONE} },
      { "%ix/getv/s",ਦਾ_IX_GETV_S,2, {OA_BIT1,   OA_FUNC_PTR, OA_NONE} },
      { "%ix/load",ਦਾ_IX_ਲੱਦੋ,3,  {OA_ਸੰਖਿਆ,   OA_BIT1,     OA_BIT2} },
      { "%ix/mov", ਦਾ_IX_MOV, 2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%ix/mul", ਦਾ_IX_MUL, 3,  {OA_ਸੰਖਿਆ,   OA_BIT1,     OA_BIT2} },
      { "%ix/sub", ਦਾ_IX_SUB, 3,  {OA_ਸੰਖਿਆ,   OA_BIT1,     OA_BIT2} },
      { "%ix/vec4",  ਦਾ_IX_VEC4,  1,  {OA_ਸੰਖਿਆ,   OA_NONE, OA_NONE} },
      { "%ix/vec4/s",ਦਾ_IX_VEC4_S,1,  {OA_ਸੰਖਿਆ,   OA_NONE, OA_NONE} },
      { "%jmp",    ਦਾ_JMP,    1,  {OA_ਕੋਡ_PTR, OA_NONE,     OA_NONE} },
      { "%jmp/0",  ਦਾ_JMP0,   2,  {OA_ਕੋਡ_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/0xz",ਦਾ_JMP0_XZ, 2,  {OA_ਕੋਡ_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1",  ਦਾ_JMP1,   2,  {OA_ਕੋਡ_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1xz",ਦਾ_JMP1_XZ, 2,  {OA_ਕੋਡ_PTR, OA_BIT1,     OA_NONE} },
      { "%join",   ਦਾ_JOIN,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%join/detach",ਦਾ_JOIN_DETACH,1,{OA_ਸੰਖਿਆ,OA_NONE,  OA_NONE} },
      { "%load/ar",ਦਾ_ਲੱਦੋ_AR,2,  {OA_ARR_PTR,  OA_BIT1,     OA_NONE} },
      { "%load/dar/r",  ਦਾ_ਲੱਦੋ_DAR_R,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE}},
      { "%load/dar/str",ਦਾ_ਲੱਦੋ_DAR_STR,  1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/dar/vec4",ਦਾ_ਲੱਦੋ_DAR_VEC4,1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/obj",   ਦਾ_ਲੱਦੋ_OBJ,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/obja",  ਦਾ_ਲੱਦੋ_OBJA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/prop/dar/vec4", ਦਾ_ਲੱਦੋ_PROP_DAR_VEC4, 2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%load/real",  ਦਾ_ਲੱਦੋ_REAL, 1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%load/str",   ਦਾ_ਲੱਦੋ_STR,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/stra",  ਦਾ_ਲੱਦੋ_STRA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/vec4",  ਦਾ_ਲੱਦੋ_VEC4, 1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%load/vec4a", ਦਾ_ਲੱਦੋ_VEC4_A,2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%max/wr", ਦਾ_MAX_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%min/wr", ਦਾ_MIN_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod",    ਦਾ_MOD,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/s",  ਦਾ_MOD_S,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/wr", ਦਾ_MOD_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul",    ਦਾ_MUL,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul/wr", ਦਾ_MUL_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%muli",   ਦਾ_MULI,   3,  {OA_BIT1,     OA_BIT2,     OA_ਸੰਖਿਆ} },
      { "%nand",   ਦਾ_NAND,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nand/r", ਦਾ_NANDR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%neg/wr", ਦਾ_NEG_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%new/cobj",  ਦਾ_NEW_COBJ,  1, {OA_VPI_PTR,OA_NONE,  OA_NONE} },
      { "%new/darray",ਦਾ_NEW_DARRAY,2, {OA_BIT1,   OA_STRING,OA_NONE} },
      { "%noop",   ਦਾ_NOOP,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor",    ਦਾ_NOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor/r",  ਦਾ_NORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%null",   ਦਾ_NULL,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or",     ਦਾ_OR,     0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or/r",   ਦਾ_ORR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%pad/s",  ਦਾ_PAD_S,  1,  {OA_ਸੰਖਿਆ,   OA_NONE,     OA_NONE} },
      { "%pad/u",  ਦਾ_PAD_U,  1,  {OA_ਸੰਖਿਆ,   OA_NONE,     OA_NONE} },
      { "%part/s", ਦਾ_PART_S, 1,  {OA_ਸੰਖਿਆ,   OA_NONE,     OA_NONE} },
      { "%part/u", ਦਾ_PART_U, 1,  {OA_ਸੰਖਿਆ,   OA_NONE,     OA_NONE} },
      { "%parti/s",ਦਾ_PARTI_S,3,  {OA_ਸੰਖਿਆ,   OA_BIT1,     OA_BIT2} },
      { "%parti/u",ਦਾ_PARTI_U,3,  {OA_ਸੰਖਿਆ,   OA_BIT1,     OA_BIT2} },
      { "%pop/obj", ਦਾ_ਕੱਢੋ_OBJ, 2, {OA_BIT1,    OA_BIT2,     OA_NONE} },
      { "%pop/real",ਦਾ_ਕੱਢੋ_REAL,1, {OA_ਸੰਖਿਆ,  OA_NONE,     OA_NONE} },
      { "%pop/str", ਦਾ_ਕੱਢੋ_STR, 1, {OA_ਸੰਖਿਆ,  OA_NONE,     OA_NONE} },
      { "%pop/vec4",ਦਾ_ਕੱਢੋ_VEC4,1, {OA_ਸੰਖਿਆ,  OA_NONE,     OA_NONE} },
      { "%pow",     ਦਾ_POW,     0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/s",   ਦਾ_POW_S,   0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/wr",  ਦਾ_POW_WR,  0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%prop/obj",ਦਾ_PROP_OBJ,2,  {OA_ਸੰਖਿਆ,   OA_BIT1,     OA_NONE} },
      { "%prop/queue/size", ਦਾ_PROP_ਕਤਾਰ_ਮਾਪ,  1, {OA_ਸੰਖਿਆ, OA_NONE, OA_NONE} },
      { "%prop/r",  ਦਾ_PROP_R,  1,  {OA_ਸੰਖਿਆ,   OA_NONE,     OA_NONE} },
      { "%prop/str",ਦਾ_PROP_STR,1,  {OA_ਸੰਖਿਆ,   OA_NONE,     OA_NONE} },
      { "%prop/v",  ਦਾ_PROP_V,  1,  {OA_ਸੰਖਿਆ,   OA_NONE,     OA_NONE} },
      { "%push/ix/vec4", ਦਾ_ਧੱਕੋ_IX_VEC4, 3, {OA_ਸੰਖਿਆ, OA_BIT1, OA_BIT2} },
      { "%pushi/real",ਦਾ_PUSHI_REAL,2,{OA_BIT1,   OA_BIT2,   OA_NONE} },
      { "%pushi/str", ਦਾ_PUSHI_STR, 1,{OA_STRING, OA_NONE,   OA_NONE} },
      { "%pushi/vec4",ਦਾ_PUSHI_VEC4,3,{OA_BIT1,   OA_BIT2,   OA_ਸੰਖਿਆ} },
      { "%pushv/str", ਦਾ_PUSHV_STR, 0,{OA_NONE,   OA_NONE,   OA_NONE} },
      { "%putc/str/vec4",ਦਾ_PUTC_STR_VEC4,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/prop/r",   ਦਾ_QINSERT_PROP_R,   2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/str", ਦਾ_QINSERT_PROP_STR, 2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/v",   ਦਾ_QINSERT_PROP_V,   3, {OA_ਸੰਖਿਆ, OA_BIT1, OA_BIT2} },
      { "%qinsert/real",ਦਾ_QINSERT_REAL,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/str", ਦਾ_QINSERT_STR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/v",   ਦਾ_QINSERT_V,   3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%qpop/b/real",ਦਾ_QPOP_B_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/str", ਦਾ_QPOP_B_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/v",   ਦਾ_QPOP_B_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/f/real",ਦਾ_QPOP_F_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/str", ਦਾ_QPOP_F_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/v",   ਦਾ_QPOP_F_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/prop/b/r",   ਦਾ_QPOP_PROP_B_REAL, 1, {OA_ਸੰਖਿਆ, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/str", ਦਾ_QPOP_PROP_B_STR, 1, {OA_ਸੰਖਿਆ, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/v",   ਦਾ_QPOP_PROP_B_V,    2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%qpop/prop/f/r",   ਦਾ_QPOP_PROP_F_REAL, 1, {OA_ਸੰਖਿਆ, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/str", ਦਾ_QPOP_PROP_F_STR, 1, {OA_ਸੰਖਿਆ, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/v",   ਦਾ_QPOP_PROP_F_V,    2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%queue/append_word/v", ਦਾ_ਕਤਾਰ_APPEND_ਸ਼ਬਦ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/find/index/prop/v", ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਸੂਚਕਾਂਕ_PROP_V, 2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%queue/find/index/v", ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਸੂਚਕਾਂਕ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find/prop/v", ਦਾ_ਕਤਾਰ_ਲੱਭੋ_PROP_V, 2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%queue/find/v", ਦਾ_ਕਤਾਰ_ਲੱਭੋ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/prop/v", ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ_PROP_V, 2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/v", ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/prop/v", ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_PROP_V, 2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%queue/find_first/v", ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/prop/v", ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਆਖਰੀ_ਸੂਚਕਾਂਕ_PROP_V, 2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/v", ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਆਖਰੀ_ਸੂਚਕਾਂਕ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/prop/v", ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਆਖਰੀ_PROP_V, 2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%queue/find_last/v", ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਆਖਰੀ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/max/obj/v", ਦਾ_ਕਤਾਰ_MAX_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/max/prop/v", ਦਾ_ਕਤਾਰ_MAX_PROP_V, 2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%queue/max/v", ਦਾ_ਕਤਾਰ_MAX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/min/obj/v", ਦਾ_ਕਤਾਰ_MIN_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/min/prop/v", ਦਾ_ਕਤਾਰ_MIN_PROP_V, 2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%queue/min/v", ਦਾ_ਕਤਾਰ_MIN_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/new_empty/v", ਦਾ_ਕਤਾਰ_NEW_ਖਾਲੀ_ਥਾਵਾਂ_V, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%queue/size/v", ਦਾ_ਕਤਾਰ_ਮਾਪ_V, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%queue/unique/index/obj/v", ਦਾ_ਕਤਾਰ_UNIQUE_ਸੂਚਕਾਂਕ_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/index/prop/v", ਦਾ_ਕਤਾਰ_UNIQUE_ਸੂਚਕਾਂਕ_PROP_V, 2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%queue/unique/index/v", ਦਾ_ਕਤਾਰ_UNIQUE_ਸੂਚਕਾਂਕ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/unique/obj/v", ਦਾ_ਕਤਾਰ_UNIQUE_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/prop/v", ਦਾ_ਕਤਾਰ_UNIQUE_PROP_V, 2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%queue/unique/v", ਦਾ_ਕਤਾਰ_UNIQUE_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/word/prop/v", ਦਾ_ਕਤਾਰ_ਸ਼ਬਦ_PROP_V, 3, {OA_ਸੰਖਿਆ, OA_BIT1, OA_BIT2} },
      { "%queue/word/v", ਦਾ_ਕਤਾਰ_ਸ਼ਬਦ_V, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%release/net",ਦਾ_RELEASE_NET,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/reg",ਦਾ_RELEASE_REG,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/wr", ਦਾ_RELEASE_WR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%replicate", ਦਾ_REPLICATE,   1,{OA_ਸੰਖਿਆ,  OA_NONE,OA_NONE} },
      { "%ret/real",  ਦਾ_RET_REAL,    1,{OA_ਸੰਖਿਆ,  OA_NONE,OA_NONE} },
      { "%ret/str",   ਦਾ_RET_STR,     1,{OA_ਸੰਖਿਆ,  OA_NONE,OA_NONE} },
      { "%ret/vec4",  ਦਾ_RET_VEC4,    3,{OA_ਸੰਖਿਆ,  OA_BIT1,OA_BIT2} },
      { "%retload/real",ਦਾ_RETLOAD_REAL,1,{OA_ਸੰਖਿਆ,  OA_NONE,OA_NONE} },
      { "%retload/str", ਦਾ_RETLOAD_STR, 1,{OA_ਸੰਖਿਆ,  OA_NONE,OA_NONE} },
      { "%retload/vec4",ਦਾ_RETLOAD_VEC4,1,{OA_ਸੰਖਿਆ,  OA_NONE,OA_NONE} },
      { "%reverse/obj", ਦਾ_ਉਲਟ_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%reverse/prop/obj", ਦਾ_ਉਲਟ_PROP_OBJ, 1, {OA_ਸੰਖਿਆ, OA_NONE, OA_NONE} },
      { "%rsort/obj", ਦਾ_RSORT_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%rsort/prop/obj", ਦਾ_RSORT_PROP_OBJ, 1, {OA_ਸੰਖਿਆ, OA_NONE, OA_NONE} },
      { "%scopy",  ਦਾ_SCOPY,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%set/dar/obj/real",ਦਾ_ਸੈੱਟ_DAR_OBJ_REAL,1,{OA_ਸੰਖਿਆ,OA_NONE,OA_NONE} },
      { "%set/dar/obj/str", ਦਾ_ਸੈੱਟ_DAR_OBJ_STR, 1,{OA_ਸੰਖਿਆ,OA_NONE,OA_NONE} },
      { "%set/dar/obj/vec4",ਦਾ_ਸੈੱਟ_DAR_OBJ_VEC4,1,{OA_ਸੰਖਿਆ,OA_NONE,OA_NONE} },
      { "%shiftl",   ਦਾ_SHIFTL,   1, {OA_ਸੰਖਿਆ, OA_NONE,   OA_NONE} },
      { "%shiftr",   ਦਾ_SHIFTR,   1, {OA_ਸੰਖਿਆ, OA_NONE,   OA_NONE} },
      { "%shiftr/s", ਦਾ_SHIFTR_S, 1, {OA_ਸੰਖਿਆ, OA_NONE,   OA_NONE} },
      { "%shuffle/obj", ਦਾ_SHUFFLE_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%shuffle/prop/obj", ਦਾ_SHUFFLE_PROP_OBJ, 1, {OA_ਸੰਖਿਆ, OA_NONE, OA_NONE} },
      { "%sort/obj", ਦਾ_ਛਾਂਟੋ_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%sort/prop/obj", ਦਾ_ਛਾਂਟੋ_PROP_OBJ, 1, {OA_ਸੰਖਿਆ, OA_NONE, OA_NONE} },
      { "%split/vec4",    ਦਾ_ਵੰਡੋ_VEC4,    1,{OA_ਸੰਖਿਆ,   OA_NONE, OA_NONE} },
      { "%store/dar/r",   ਦਾ_STORE_DAR_R,   1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/str", ਦਾ_STORE_DAR_STR, 1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/vec4",ਦਾ_STORE_DAR_VEC4,1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/obj",   ਦਾ_STORE_OBJ,   1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/obja",  ਦਾ_STORE_OBJA,  2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/prop/obj",ਦਾ_STORE_PROP_OBJ,2, {OA_ਸੰਖਿਆ,  OA_BIT1, OA_NONE} },
      { "%store/prop/qb/r",   ਦਾ_STORE_PROP_QB_R,   2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/str", ਦਾ_STORE_PROP_QB_STR, 2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/v",   ਦਾ_STORE_PROP_QB_V,   3, {OA_ਸੰਖਿਆ, OA_BIT1, OA_BIT2} },
      { "%store/prop/qf/r",   ਦਾ_STORE_PROP_QF_R,   2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/str", ਦਾ_STORE_PROP_QF_STR, 2, {OA_ਸੰਖਿਆ, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/v",   ਦਾ_STORE_PROP_QF_V,   3, {OA_ਸੰਖਿਆ, OA_BIT1, OA_BIT2} },
      { "%store/prop/r",  ਦਾ_STORE_PROP_R,  1, {OA_ਸੰਖਿਆ,  OA_NONE, OA_NONE} },
      { "%store/prop/str",ਦਾ_STORE_PROP_STR,1, {OA_ਸੰਖਿਆ,  OA_NONE, OA_NONE} },
      { "%store/prop/v",  ਦਾ_STORE_PROP_V,  2, {OA_ਸੰਖਿਆ,  OA_BIT1, OA_NONE} },
      { "%store/qb/r",   ਦਾ_STORE_QB_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/str", ਦਾ_STORE_QB_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/v",   ਦਾ_STORE_QB_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qdar/r",  ਦਾ_STORE_QDAR_R,  2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/str",ਦਾ_STORE_QDAR_STR,2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/v",  ਦਾ_STORE_QDAR_V,  3,{OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qf/r",   ਦਾ_STORE_QF_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/str", ਦਾ_STORE_QF_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/v",   ਦਾ_STORE_QF_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qobj/r",  ਦਾ_STORE_QOBJ_R,  2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/str",ਦਾ_STORE_QOBJ_STR,2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/v",  ਦਾ_STORE_QOBJ_V,  3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/real",    ਦਾ_STORE_REAL,    1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/reala",   ਦਾ_STORE_REALA,   2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/str",     ਦਾ_STORE_STR,     1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/stra",    ਦਾ_STORE_STRA,    2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/vec4",    ਦਾ_STORE_VEC4,    3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/vec4a",   ਦਾ_STORE_VEC4_A,   3, {OA_ARR_PTR, OA_BIT1, OA_BIT2} },
      { "%sub",    ਦਾ_SUB,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%sub/wr", ਦਾ_SUB_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%subi",   ਦਾ_SUBI,   3,  {OA_BIT1,     OA_BIT2,     OA_ਸੰਖਿਆ} },
      { "%substr",     ਦਾ_SUBSTR,     2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%substr/vec4",ਦਾ_SUBSTR_VEC4,2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%test_nul",     ਦਾ_TEST_NUL,     1,{OA_FUNC_PTR,OA_NONE,    OA_NONE} },
      { "%test_nul/a",   ਦਾ_TEST_NUL_A,   2,{OA_ARR_PTR, OA_BIT1,    OA_NONE} },
      { "%test_nul/obj", ਦਾ_TEST_NUL_OBJ, 0,{OA_NONE,    OA_NONE,    OA_NONE} },
      { "%test_nul/prop",ਦਾ_TEST_NUL_PROP,2,{OA_ਸੰਖਿਆ,  OA_BIT1,    OA_NONE} },
      { "%uparr/and/v",          ਦਾ_UPARR_AND_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/or/v",           ਦਾ_UPARR_OR_V,           2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/product/obj/v",  ਦਾ_UPARR_ਗੁਣਨਫਲ_OBJ_V,  1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/product/prop/v", ਦਾ_UPARR_ਗੁਣਨਫਲ_PROP_V, 2, {OA_ਸੰਖਿਆ,   OA_BIT1, OA_NONE} },
      { "%uparr/product/r",      ਦਾ_UPARR_ਗੁਣਨਫਲ_REAL,   1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/product/v",      ਦਾ_UPARR_ਗੁਣਨਫਲ_V,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/sum/obj/v",      ਦਾ_UPARR_ਜੋੜ_OBJ_V,      1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/sum/prop/v",     ਦਾ_UPARR_ਜੋੜ_PROP_V,     2, {OA_ਸੰਖਿਆ,   OA_BIT1, OA_NONE} },
      { "%uparr/sum/r",          ਦਾ_UPARR_ਜੋੜ_REAL,       1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/sum/v",          ਦਾ_UPARR_ਜੋੜ_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/xor/v",          ਦਾ_UPARR_XOR_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%wait",   ਦਾ_WAIT,   1,  {OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%wait/fork",ਦਾ_WAIT_FORK,0,{OA_NONE,   OA_NONE,     OA_NONE} },
      { "%xnor",   ਦਾ_XNOR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xnor/r", ਦਾ_XNORR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor",    ਦਾ_XOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor/r",  ਦਾ_XORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { 0, ਦਾ_NOOP, 0, {OA_NONE, OA_NONE, OA_NONE} }
};

ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ opcode_ਗਿਣਤੀ =
                    ਆਕਾਰ(opcode_ਸਾਰਣੀ)/ਆਕਾਰ(*opcode_ਸਾਰਣੀ) - 1;

ਸਥਿਰ ਪੂਰਨ opcode_compare(ਅਚਲ ਖਾਲੀ*k, ਅਚਲ ਖਾਲੀ*r)
{
      ਅਚਲ ਅੱਖਰ*kp = static_cast<ਅਚਲ ਅੱਖਰ*>(k);
      ਅਚਲ ਬਣਤਰ opcode_ਸਾਰਣੀ_s*rp = static_cast<ਅਚਲ ਬਣਤਰ opcode_ਸਾਰਣੀ_s*>(r);
      ਵਾਪਸ strcmp(kp, rp->mnemonic);
}

/*
 * Keep a symbol table of addresses within code space. Labels on
 * executable opcodes are mapped to their address here.
 */
ਸਥਿਰ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_t sym_codespace = 0;

/*
 * Keep a symbol table of functors mentioned in the source. This table
 * is used to resolve references as they come.
 */
ਸਥਿਰ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_t sym_functors = 0;

/*
 * VPI objects are indexed during compile time so that they can be
 * linked together as they are created. This symbol table matches
 * labels to vpiHandles.
 */
ਸਥਿਰ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_t sym_vpi = 0;


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

ਖਾਲੀ define_functor_ਚਿੰਨ੍ਹ(ਅਚਲ ਅੱਖਰ*label, vvp_net_t*net)
{
      ਚਿੰਨ੍ਹ_ਮੁੱਲ_t val;
      val.net = net;
      sym_ਸੈੱਟ_ਮੁੱਲ(sym_functors, label, val);
}

ਸਥਿਰ vvp_net_t*lookup_functor_ਚਿੰਨ੍ਹ(ਅਚਲ ਅੱਖਰ*label)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(sym_functors);
      ਚਿੰਨ੍ਹ_ਮੁੱਲ_t val = sym_get_ਮੁੱਲ(sym_functors, label);
      ਵਾਪਸ val.net;
}

vpiHandle vvp_lookup_handle(ਅਚਲ ਅੱਖਰ*label)
{
      ਚਿੰਨ੍ਹ_ਮੁੱਲ_t val = sym_get_ਮੁੱਲ(sym_vpi, label);
      ਜੇ (val.ptr) ਵਾਪਸ (vpiHandle) val.ptr;
      ਵਾਪਸ 0;
}

vvp_net_t* vvp_net_lookup(ਅਚਲ ਅੱਖਰ*label)
{
      ਸਥਿਰ ਤਾਰਕਿਕ t0_trigger_generated = ਝੂਠ;

        /* First, look to see if the symbol is a vpi object of some
	   sort. If it is, then get the vvp_ipoint_t pointer out of
	   the vpiHandle. */
      ਚਿੰਨ੍ਹ_ਮੁੱਲ_t val = sym_get_ਮੁੱਲ(sym_vpi, label);
      ਜੇ (val.ptr) {
	    vpiHandle vpi = static_cast<vpiHandle>(val.ptr);
	    ਚੋਣ (vpi->get_type_ਕੋਡ()) {
		ਹਾਲਤ vpiNet:
		ਹਾਲਤ vpiReg:
		ਹਾਲਤ vpi_ਦੋਆਧਾਰੀ_ਅੰਕ_Var:
		ਹਾਲਤ vpiByteVar:
		ਹਾਲਤ vpiShortIntVar:
		ਹਾਲਤ vpiIntVar:
		ਹਾਲਤ vpiLongIntVar:
		ਹਾਲਤ vpiIntegerVar: {
		      ਅਚਲ __vpiSignal*sig = dynamic_cast<__vpiSignal*>(vpi);
		      ਵਾਪਸ sig->ਗੰਢ;
		}

		ਹਾਲਤ vpiRealVar: {
		      ਅਚਲ __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(vpi);
		      ਵਾਪਸ sig->net;
		}

		ਹਾਲਤ vpiStringVar:
		ਹਾਲਤ vpiArrayVar:
		ਹਾਲਤ vpiClassVar: {
		      ਅਚਲ __vpiBaseVar*sig = dynamic_cast<__vpiBaseVar*>(vpi);
		      ਵਾਪਸ sig->get_net();
		}

		ਹਾਲਤ vpiNamedEvent: {
		      ਅਚਲ __vpiNamedEvent*tmp = dynamic_cast<__vpiNamedEvent*>(vpi);
		      ਵਾਪਸ tmp->funct;
		}

		ਮੂਲ:
		  fprintf(stderr, "Unsupported type %d.\n",
		          vpi->get_type_ਕੋਡ());
		  ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    }
      }


	/* Failing that, look for a general functor. */
      vvp_net_t*tmp = lookup_functor_ਚਿੰਨ੍ਹ(label);

	// This is a special label used to create a T0 trigger for the
	// always_comb/latch processes.
     ਜੇ (! t0_trigger_generated && (strcmp(label, "E_0x0") == 0)) {
	      // This should never happen, but if it does then the E_0x0
	      // event generation may need to be explicitly generated in
	      // the compiler output instead of implicitly in this code.
	    ਪ੍ਰਤਿਜ੍ਞਾ(! vpip_peek_current_scope()->is_automatic());
	    t0_trigger_generated = ਸੱਚ;
	      // Create a local event with no name for the T0 trigger
	    compile_named_event(strdup(label), strcpy(ਨਵਾਂ ਅੱਖਰ [1],""), ਸੱਚ);
	    tmp = vvp_net_lookup(label);
	    ਪ੍ਰਤਿਜ੍ਞਾ(tmp);
	      // Create a trigger for the T0 event.
	    vvp_net_ptr_t ptr (tmp, 0);
	    ਸਮਾਂ_ਸਾਰਣੀ_t0_trigger(ptr);
     }

      ਵਾਪਸ tmp;
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
ਸਥਿਰ resolv_list_s*resolv_list = 0;

resolv_list_s::~resolv_list_s()
{
      ਮੁਕਤ(label_);
}

ਖਾਲੀ resolv_submit(resolv_list_s*cur)
{
      ਜੇ (cur->resolve()) {
	    ਮਿਟਾਓ cur;
	    ਵਾਪਸ;
      }

      cur->ਅਗਲਾ_2 = resolv_list;
      resolv_list = cur;
}


/*
 * Look up vvp_nets in the symbol table. The "source" is the label for
 * the net that I want to feed, and net->port[port] is the vvp_net
 * input that I want that node to feed into. When the name is found,
 * put net->port[port] into the fan-out list for that node.
 */
ਬਣਤਰ vvp_net_resolv_list_s: ਜਨਤਕ resolv_list_s {

      ਸਪਸ਼ਟ vvp_net_resolv_list_s(ਅੱਖਰ*l) : resolv_list_s(l) { }
	// port to be driven by the located node.
      vvp_net_ptr_t ਪੋਰਟ;
      ਆਭਾਸੀ ਤਾਰਕਿਕ resolve(ਤਾਰਕਿਕ mes) override;
};

ਤਾਰਕਿਕ vvp_net_resolv_list_s::resolve(ਤਾਰਕਿਕ mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      ਜੇ (tmp) {
	      // Link the input port to the located output.
	    tmp->link(ਪੋਰਟ);
	    ਵਾਪਸ ਸੱਚ;
      }

      ਜੇ (mes)
	    fprintf(stderr, "unresolved vvp_net reference: %s\n", label());

      ਵਾਪਸ ਝੂਠ;
}

ਸਤਰਅੰਦਰ ਸਥਿਰ
ਖਾਲੀ postpone_functor_ਨਿਵੇਸ਼(vvp_net_ptr_t ਪੋਰਟ, ਅੱਖਰ*lab)
{
      ਬਣਤਰ vvp_net_resolv_list_s*res = ਨਵਾਂ ਬਣਤਰ vvp_net_resolv_list_s(lab);
      res->ਪੋਰਟ   = ਪੋਰਟ;

      resolv_submit(res);
}


/*
 *  Generic functor reference lookup.
 */

ਬਣਤਰ functor_gen_resolv_list_s: ਜਨਤਕ resolv_list_s {
      ਸਪਸ਼ਟ functor_gen_resolv_list_s(ਅੱਖਰ*txt) : resolv_list_s(txt) {
	    ref = 0;
      }
      vvp_net_t**ref;
      ਆਭਾਸੀ ਤਾਰਕਿਕ resolve(ਤਾਰਕਿਕ mes) override;
};

ਤਾਰਕਿਕ functor_gen_resolv_list_s::resolve(ਤਾਰਕਿਕ mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      ਜੇ (tmp) {
	    *ref = tmp;
	    ਵਾਪਸ ਸੱਚ;
      }

      ਜੇ (mes)
	    fprintf(stderr, "unresolved functor reference: %s\n", label());

      ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ functor_ref_lookup(vvp_net_t**ref, ਅੱਖਰ*lab)
{
      ਬਣਤਰ functor_gen_resolv_list_s*res =
	    ਨਵਾਂ ਬਣਤਰ functor_gen_resolv_list_s(lab);

      res->ref    = ref;

      resolv_submit(res);
}

/*
 *  vpiHandle lookup
 */

ਬਣਤਰ vpi_handle_resolv_list_s: ਜਨਤਕ resolv_list_s {
      ਸਪਸ਼ਟ vpi_handle_resolv_list_s(ਅੱਖਰ*lab) : resolv_list_s(lab) {
	    handle = ਨਿਰਾ;
      }
      ਆਭਾਸੀ ਤਾਰਕਿਕ resolve(ਤਾਰਕਿਕ mes) override;
      vpiHandle *handle;
};

ਤਾਰਕਿਕ vpi_handle_resolv_list_s::resolve(ਤਾਰਕਿਕ mes)
{
      ਚਿੰਨ੍ਹ_ਮੁੱਲ_t val = sym_get_ਮੁੱਲ(sym_vpi, label());
      ਜੇ (!val.ptr) {
	    // check for thread access symbols
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, wid;
	    size_t n = 0;
	    ਅੱਖਰ ss[32];
	    ਜੇ (2 == ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(label(), "W<%u,%31[r]>%zn", &ਅਧਾਰ, ss, &n)
		       && n == ਲੜੀ_ਲੰਬਾਈ(label())) {

		  val.ptr = vpip_make_vthr_ਸ਼ਬਦ(ਅਧਾਰ, ss);
		  sym_ਸੈੱਟ_ਮੁੱਲ(sym_vpi, label(), val);

	    } ਨਹੀਂਤਾਂ ਜੇ (1 == ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(label(), "S<%u,str>%zn", &ਅਧਾਰ, &n)
		       && n == ਲੜੀ_ਲੰਬਾਈ(label())) {

		  val.ptr = vpip_make_vthr_str_ਢੇਰ(ਅਧਾਰ);
		  sym_ਸੈੱਟ_ਮੁੱਲ(sym_vpi, label(), val);

	    } ਨਹੀਂਤਾਂ ਜੇ (3 == ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(label(), "S<%u,vec4,%31[su]%u>%zn", &ਅਧਾਰ, ss, &wid, &n)
		       && n == ਲੜੀ_ਲੰਬਾਈ(label())) {

		  ਤਾਰਕਿਕ signed_flag = ਝੂਠ;
		  ਲਈ (ਅੱਖਰ*fp = ss ;  *fp ;  fp += 1) ਚੋਣ (*fp) {
		      ਹਾਲਤ 's':
			signed_flag = ਸੱਚ;
			ਰੋਕੋ;
		      ਹਾਲਤ 'u':
			signed_flag = ਝੂਠ;
			ਰੋਕੋ;
		      ਮੂਲ:
			ਰੋਕੋ;
		  }
		  val.ptr = vpip_make_vthr_vec4_ਢੇਰ(ਅਧਾਰ, signed_flag, wid);
		  sym_ਸੈੱਟ_ਮੁੱਲ(sym_vpi, label(), val);
	    }

      }

      ਜੇ (!val.ptr) {
	    // check for memory word  M<mem,base,wid>
      }

      ਜੇ (strcmp(label(), "null") == 0) {
	    val.ptr = vpip_make_null_const();
	    sym_ਸੈੱਟ_ਮੁੱਲ(sym_vpi, label(), val);
      }

      ਜੇ (val.ptr) {
	    *handle = static_cast<vpiHandle>(val.ptr);
	    ਵਾਪਸ ਸੱਚ;
      }

      ਜੇ (mes)
	    fprintf(stderr, "unresolved vpi name lookup: %s\n", label());

      ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ compile_vpi_lookup(vpiHandle *handle, ਅੱਖਰ*label)
{
      ਜੇ (strcmp(label, "$time") == 0) {
	    *handle = vpip_sim_ਸਮਾਂ(vpip_peek_current_scope(), ਝੂਠ);
	    ਮੁਕਤ(label);
	    ਵਾਪਸ;
      }

      ਜੇ (strcmp(label, "$stime") == 0) {
	    *handle = vpip_sim_ਸਮਾਂ(vpip_peek_current_scope(), ਸੱਚ);
	    ਮੁਕਤ(label);
	    ਵਾਪਸ;
      }

      ਜੇ (strcmp(label, "$realtime") == 0) {
	    *handle = vpip_sim_realtime(vpip_peek_current_scope());
	    ਮੁਕਤ(label);
	    ਵਾਪਸ;
      }

      ਜੇ (strcmp(label, "$simtime") == 0) {
	    *handle = vpip_sim_ਸਮਾਂ(0, ਝੂਠ);
	    ਮੁਕਤ(label);
	    ਵਾਪਸ;
      }

      ਬਣਤਰ vpi_handle_resolv_list_s*res
	    = ਨਵਾਂ ਬਣਤਰ vpi_handle_resolv_list_s(label);

      res->handle = handle;
      resolv_submit(res);
}

/*
 * Code Label lookup
 */

ਬਣਤਰ ਕੋਡ_label_resolv_list_s: ਜਨਤਕ resolv_list_s {
      ਸਪਸ਼ਟ ਕੋਡ_label_resolv_list_s(ਅੱਖਰ*lab, ਤਾਰਕਿਕ cptr2) : resolv_list_s(lab) {
	    code = ਨਿਰਾ;
	    cptr2_flag = cptr2;
      }
      ਬਣਤਰ vvp_ਕੋਡ_s *code;
      ਤਾਰਕਿਕ cptr2_flag;
      ਆਭਾਸੀ ਤਾਰਕਿਕ resolve(ਤਾਰਕਿਕ mes) override;
};

ਤਾਰਕਿਕ ਕੋਡ_label_resolv_list_s::resolve(ਤਾਰਕਿਕ mes)
{
      ਚਿੰਨ੍ਹ_ਮੁੱਲ_t val = sym_get_ਮੁੱਲ(sym_codespace, label());
      ਜੇ (val.ptr) {
	    ਜੇ (cptr2_flag)
		  code->cptr2 = reinterpret_cast<vvp_ਕੋਡ_t>(val.ptr);
	    ਨਹੀਂਤਾਂ
		  code->cptr = reinterpret_cast<vvp_ਕੋਡ_t>(val.ptr);
	    ਵਾਪਸ ਸੱਚ;
      }

      ਜੇ (mes)
	    fprintf(stderr, "unresolved code label: %s\n", label());

      ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ ਕੋਡ_label_lookup(ਬਣਤਰ vvp_ਕੋਡ_s *code, ਅੱਖਰ *label, ਤਾਰਕਿਕ cptr2)
{
      ਬਣਤਰ ਕੋਡ_label_resolv_list_s *res
	    = ਨਵਾਂ ਬਣਤਰ ਕੋਡ_label_resolv_list_s(label, cptr2);

      res->code  = code;

      resolv_submit(res);
}

ਬਣਤਰ ਕੋਡ_array_resolv_list_s: ਜਨਤਕ resolv_list_s {
      ਸਪਸ਼ਟ ਕੋਡ_array_resolv_list_s(ਅੱਖਰ*lab) : resolv_list_s(lab) {
	    code = ਨਿਰਾ;
      }
      ਬਣਤਰ vvp_ਕੋਡ_s *code;
      ਆਭਾਸੀ ਤਾਰਕਿਕ resolve(ਤਾਰਕਿਕ mes) override;
};

ਤਾਰਕਿਕ ਕੋਡ_array_resolv_list_s::resolve(ਤਾਰਕਿਕ mes)
{
      code->array = array_ਲੱਭੋ(label());
      ਜੇ (code->array != 0) {
	    ਵਾਪਸ ਸੱਚ;
      }

      ਜੇ (mes)
	    fprintf(stderr, "Array unresolved: %s\n", label());

      ਵਾਪਸ ਝੂਠ;
}

ਸਥਿਰ ਖਾਲੀ compile_array_lookup(ਬਣਤਰ vvp_ਕੋਡ_s*code, ਅੱਖਰ*label)
{
      ਬਣਤਰ ਕੋਡ_array_resolv_list_s *res
	    = ਨਵਾਂ ਬਣਤਰ ਕੋਡ_array_resolv_list_s(label);

      res->code  = code;

      resolv_submit(res);
}

ਸਥਿਰ std::list<ਬਣਤਰ __vpiSysTaskCall*> scheduled_compiletf;

ਖਾਲੀ compile_compiletf(ਬਣਤਰ __vpiSysTaskCall*obj)
{
      ਜੇ (obj->defn->info.compiletf == 0)
	    ਵਾਪਸ;

      scheduled_compiletf.push_back(obj);
}

/*
 * When parsing is otherwise complete, this function is called to do
 * the final stuff. Clean up deferred linking here.
 */

ਖਾਲੀ compile_cleanup(ਖਾਲੀ)
{
      ਪੂਰਨ lnerrs = -1;
      ਪੂਰਨ nerrs = 0;
      ਪੂਰਨ ਆਖਰੀ;

      ਜੇ (verbose_flag) {
	    fprintf(stderr, " ... Linking\n");
	    fflush(stderr);
      }

      ਕਰੋ {
	    resolv_list_s *res = resolv_list;
	    resolv_list = 0x0;
	    ਆਖਰੀ = nerrs == lnerrs;
	    lnerrs = nerrs;
	    nerrs = 0;
	    ਜਦੋਂ (res) {
		  resolv_list_s *cur = res;
		  res = res->ਅਗਲਾ_2;
		  ਜੇ (cur->resolve(ਆਖਰੀ))
			ਮਿਟਾਓ cur;
		  ਨਹੀਂਤਾਂ {
			nerrs++;
			cur->ਅਗਲਾ_2 = resolv_list;
			resolv_list = cur;
		  }
	    }
	    ਜੇ (nerrs && ਆਖਰੀ)
		  fprintf(stderr,
			  "compile_cleanup: %d unresolved items\n",
			  nerrs);
      } ਜਦੋਂ (nerrs && !ਆਖਰੀ);

      compile_errors += nerrs;

      ਜੇ (verbose_flag) {
	    fprintf(stderr, " ... Removing symbol tables\n");
	    fflush(stderr);
      }

	/* After compile is complete, the vpi symbol table is no
	   longer needed. VPI objects are located by following
	   scopes. */
      delete_ਚਿੰਨ੍ਹ_ਸਾਰਣੀ(sym_vpi);
      sym_vpi = 0;

	/* Don't need the code labels. The instructions have numeric
	   pointers in them, the symbol table is no longer needed. */
      delete_ਚਿੰਨ੍ਹ_ਸਾਰਣੀ(sym_codespace);
      sym_codespace = 0;

      delete_ਚਿੰਨ੍ਹ_ਸਾਰਣੀ(sym_functors);
      sym_functors = 0;

      delete_udp_symbols();

      compile_island_cleanup();
      compile_array_cleanup();

      ਜੇ (verbose_flag) {
	    fprintf(stderr, " ... Compiletf functions\n");
	    fflush(stderr);
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(vpi_ਬਹੁਲਕ_flag == VPI_ਬਹੁਲਕ_NONE);
      vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_COMPILETF;

      ਜਦੋਂ (! scheduled_compiletf.empty()) {
	    ਬਣਤਰ __vpiSysTaskCall*obj = scheduled_compiletf.front();
	    scheduled_compiletf.pop_front();
	    vpip_cur_task = obj;
	    obj->defn->info.compiletf (obj->defn->info.user_ਅੰਕੜੇ);
	    vpip_cur_task = 0;
      }

      vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_NONE;
}

ਖਾਲੀ compile_vpi_ਚਿੰਨ੍ਹ(ਅਚਲ ਅੱਖਰ*label, vpiHandle obj)
{
      ਚਿੰਨ੍ਹ_ਮੁੱਲ_t val;
      val.ptr = obj;
      sym_ਸੈੱਟ_ਮੁੱਲ(sym_vpi, label, val);
}

/*
 * Initialize the compiler by allocation empty symbol tables and
 * initializing the various address spaces.
 */
ਖਾਲੀ compile_init(ਖਾਲੀ)
{
      sym_vpi = new_ਚਿੰਨ੍ਹ_ਸਾਰਣੀ();

      sym_functors = new_ਚਿੰਨ੍ਹ_ਸਾਰਣੀ();

      sym_codespace = new_ਚਿੰਨ੍ਹ_ਸਾਰਣੀ();
      codespace_init();
}

ਖਾਲੀ compile_ਲੱਦੋ_vpi_module(ਅੱਖਰ*name)
{
      vpip_ਲੱਦੋ_module(name);
      ਮਿਟਾਓ[] name;
}

ਖਾਲੀ compile_vpi_ਸਮਾਂ_precision(ਲੰਮਾ pre)
{
      vpip_ਸੈੱਟ_ਸਮਾਂ_precision(pre);
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
ਸਥਿਰ ਤਾਰਕਿਕ crstring_ਸਿਰਲੇਖ_test(ਅਚਲ ਅੱਖਰ*str)
{
      ਜੇ ((str[0] != 'C') && (str[0] != 'c'))
	    ਵਾਪਸ ਝੂਠ;
      ਜੇ ((str[1] != 'r') || (str[2] != '<'))
	    ਵਾਪਸ ਝੂਠ;

      ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ crstring_test(ਅਚਲ ਅੱਖਰ*str)
{
      ਜੇ (!crstring_ਸਿਰਲੇਖ_test(str))
	    ਵਾਪਸ ਝੂਠ;
      ਅਚਲ ਅੱਖਰ*tp = strchr(str, '>');
      ਜੇ (tp == 0) ਵਾਪਸ ਝੂਠ;
      ਜੇ (tp[1] != 0) ਵਾਪਸ ਝੂਠ;

      ਜੇ ((strspn(str+3, "0123456789abcdefmg")+3) != (size_t)(tp - str))
	  ਵਾਪਸ ਝੂਠ;

      ਵਾਪਸ ਸੱਚ;
}

ਦੁੱਗਣਾ crstring_ਮੰਜ਼ਿਲ_double(ਅਚਲ ਅੱਖਰ*label)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(crstring_ਸਿਰਲੇਖ_test(label));

      ਅਚਲ ਅੱਖਰ*cp = label+3;
      ਪ੍ਰਤਿਜ੍ਞਾ(*cp == 'm');
      cp += 1;
      ਅੱਖਰ*ep;
      uint64_t mant = strtoull(cp, &ep, 16);
      cp = ep;
      ਪ੍ਰਤਿਜ੍ਞਾ(*cp == 'g');
      cp += 1;
      ਪੂਰਨ ਘਾਤਾਂਕ = strtoul(cp, 0, 16);

      ਦੁੱਗਣਾ tmp;
      ਜੇ (mant == 0 && ਘਾਤਾਂਕ == 0x3fff) {
	    tmp = INFINITY;
      } ਨਹੀਂਤਾਂ ਜੇ (mant == 0 && ਘਾਤਾਂਕ == 0x7fff) {
	    tmp = -INFINITY;
      } ਨਹੀਂਤਾਂ ਜੇ (ਘਾਤਾਂਕ == 0x3fff) {
	    tmp = nan("");
      } ਨਹੀਂਤਾਂ {
	    ਦੁੱਗਣਾ sign = (ਘਾਤਾਂਕ & 0x4000)? -1.0 : 1.0;
	    ਘਾਤਾਂਕ &= 0x1fff;

	    tmp = sign * ldexp((ਦੁੱਗਣਾ)mant, ਘਾਤਾਂਕ - 0x1000);
      }

      ਵਾਪਸ tmp;
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

ਖਾਲੀ ਨਿਵੇਸ਼_connect(vvp_net_t*fdx, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅੱਖਰ*label)
{
      vvp_net_ptr_t ifdx = vvp_net_ptr_t(fdx, ਪੋਰਟ);

	/* Is this a vvp_vector4_t constant value? */
      ਜੇ (c4string_test(label)) {

	    vvp_vector4_t tmp = c4string_ਮੰਜ਼ਿਲ_vector4(label);

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
	    ਜੇ (label[0] == 'c')
		  ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(ifdx, tmp);
	    ਨਹੀਂਤਾਂ
		  ਸਮਾਂ_ਸਾਰਣੀ_ਸੈੱਟ_ਸਦਿਸ਼(ifdx, tmp);

	    ਮੁਕਤ(label);
	    ਵਾਪਸ;
      }

	/* Is this a vvp_vector8_t constant value? */
      ਜੇ (c8string_test(label)) {

	    vvp_vector8_t tmp = c8string_ਮੰਜ਼ਿਲ_vector8(label);
	    ਜੇ (label[0] == 'c')
		  ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(ifdx, tmp);
	    ਨਹੀਂਤਾਂ
		  ਸਮਾਂ_ਸਾਰਣੀ_ਸੈੱਟ_ਸਦਿਸ਼(ifdx, tmp);

	    ਮੁਕਤ(label);
	    ਵਾਪਸ;
      }

	/* Handle the Cr<> constant driver, which is a real-value
	   driver. */
      ਜੇ (crstring_test(label)) {

	    ਦੁੱਗਣਾ tmp = crstring_ਮੰਜ਼ਿਲ_double(label);

	    ਜੇ (label[0] == 'c')
		  ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(ifdx, tmp);
	    ਨਹੀਂਤਾਂ
		  ਸਮਾਂ_ਸਾਰਣੀ_ਸੈੱਟ_ਸਦਿਸ਼(ifdx, tmp);
	    ਮੁਕਤ(label);
	    ਵਾਪਸ;
      }

	/* Handle the general case that this is a label for a node in
	   the vvp net. This arranges for the label to be preserved in
	   a linker list, and linked when the symbol table is
	   complete. */
      postpone_functor_ਨਿਵੇਸ਼(ifdx, label);
}

ਖਾਲੀ inputs_connect(vvp_net_t*fdx, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਜੇ (argc > 4) {
	    std::cerr << "XXXX argv[0] = " << argv[0].ਪਾਠ << std::ਸਤਰ_ਅੰਤ;
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(argc <= 4);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0;  idx < argc;  idx += 1) {

	    ਨਿਵੇਸ਼_connect(fdx, idx, argv[idx].ਪਾਠ);
      }
}

ਖਾਲੀ wide_inputs_connect(vvp_wide_fun_core*core,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
	/* Create input functors to receive values from the
	   network. These functors pass the data to the core. */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਨਿਵੇਸ਼_functors = (argc+3) / 4;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਨਿਵੇਸ਼_functors ;  idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ = idx*4;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ trans = 4;
	    ਜੇ (ਅਧਾਰ+trans > argc)
		  trans = argc - ਅਧਾਰ;

	    vvp_wide_fun_t*cur = ਨਵਾਂ vvp_wide_fun_t(core, ਅਧਾਰ);
	    vvp_net_t*ptr = ਨਵਾਂ vvp_net_t;
	    ptr->fun = cur;

	    inputs_connect(ptr, trans, argv+ਅਧਾਰ);
      }
}

ਸਾਂਚਾ <ਵਰਗ T_> ਖਾਲੀ make_arith(T_ *arith, ਅੱਖਰ*label,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      vvp_net_t* ptr = ਨਵਾਂ vvp_net_t;
      ptr->fun = arith;

      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);

      ਪ੍ਰਤਿਜ੍ਞਾ(argc == 2);
      inputs_connect(ptr, argc, argv);

      ਮੁਕਤ(argv);
}

ਖਾਲੀ compile_arith_cast_int(ਅੱਖਰ*label, ਲੰਮਾ width,
                            ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      vvp_arith_cast_int*arith = ਨਵਾਂ vvp_arith_cast_int((ਬਿਨਾਨਿਸ਼ਾਨੀ) width);

      vvp_net_t* ptr = ਨਵਾਂ vvp_net_t;
      ptr->fun = arith;

      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);

      ਪ੍ਰਤਿਜ੍ਞਾ(argc == 1);
      inputs_connect(ptr, argc, argv);
      ਮੁਕਤ(argv);
}

ਖਾਲੀ compile_arith_cast_vec2(ਅੱਖਰ*label, ਲੰਮਾ width,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      vvp_arith_cast_vec2*arith = ਨਵਾਂ vvp_arith_cast_vec2((ਬਿਨਾਨਿਸ਼ਾਨੀ) width);

      vvp_net_t* ptr = ਨਵਾਂ vvp_net_t;
      ptr->fun = arith;

      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);

      ਪ੍ਰਤਿਜ੍ਞਾ(argc == 1);
      inputs_connect(ptr, argc, argv);
      ਮੁਕਤ(argv);
}

ਖਾਲੀ compile_arith_cast_real(ਅੱਖਰ*label, ਤਾਰਕਿਕ signed_flag,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      vvp_arith_cast_real*arith = ਨਵਾਂ vvp_arith_cast_real(signed_flag);

      vvp_net_t* ptr = ਨਵਾਂ vvp_net_t;
      ptr->fun = arith;

      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);

      ਪ੍ਰਤਿਜ੍ਞਾ(argc == 1);
      inputs_connect(ptr, argc, argv);
      ਮੁਕਤ(argv);
}

ਖਾਲੀ compile_arith_abs(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      vvp_arith_abs*arith = ਨਵਾਂ vvp_arith_abs;

      vvp_net_t* ptr = ਨਵਾਂ vvp_net_t;
      ptr->fun = arith;

      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);

      ਪ੍ਰਤਿਜ੍ਞਾ(argc == 1);
      inputs_connect(ptr, argc, argv);
      ਮੁਕਤ(argv);
}

ਖਾਲੀ compile_arith_div(ਅੱਖਰ*label, ਲੰਮਾ wid, ਤਾਰਕਿਕ signed_flag,
		       ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    ਅਚਲ ਅੱਖਰ *suffix = "";
	    ਜੇ (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s; .arith/div%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_arith_div(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_arith_div_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s; .arith/divr has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_real_ *arith = ਨਵਾਂ vvp_arith_div_real;
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_arith_mod(ਅੱਖਰ*label, ਲੰਮਾ wid, ਤਾਰਕਿਕ signed_flag,
		       ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .arith/mod has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_arith_mod(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_arith_mod_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .arith/mod.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_real_ *arith = ਨਵਾਂ vvp_arith_mod_real;
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_arith_mult(ਅੱਖਰ*label, ਲੰਮਾ wid,
			ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .arith/mult has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_arith_mult(wid);
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_arith_mult_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .arith/mult.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_real_ *arith = ਨਵਾਂ vvp_arith_mult_real;
      make_arith(arith, label, argc, argv);
}


ਖਾਲੀ compile_arith_pow(ਅੱਖਰ*label, ਲੰਮਾ wid, ਤਾਰਕਿਕ signed_flag,
		       ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    ਅਚਲ ਅੱਖਰ *suffix = "";
	    ਜੇ (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s .arith/pow%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_arith_pow(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_arith_pow_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .arith/pow.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_real_ *arith = ਨਵਾਂ vvp_arith_pow_real;
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_arith_sub(ਅੱਖਰ*label, ਲੰਮਾ wid, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .arith/sub has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_arith_sub(wid);
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_arith_sub_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s; .arith/sub.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_real_ *arith = ਨਵਾਂ vvp_arith_sub_real;
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_arith_ਜੋੜ(ਅੱਖਰ*label, ਲੰਮਾ wid, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .arith/sum has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_arith_ਜੋੜ(wid);
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_arith_ਜੋੜ_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .arith/sum.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_real_ *arith = ਨਵਾਂ vvp_arith_ਜੋੜ_real;
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_cmp_eeq(ਅੱਖਰ*label, ਲੰਮਾ wid,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_cmp_eeq(wid);

      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_cmp_nee(ਅੱਖਰ*label, ਲੰਮਾ wid,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_cmp_nee(wid);

      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_cmp_eq(ਅੱਖਰ*label, ਲੰਮਾ wid, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_cmp_eq(wid);
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_cmp_eqx(ਅੱਖਰ*label, ਲੰਮਾ wid, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqx has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_cmp_eqx(wid);
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_cmp_eqz(ਅੱਖਰ*label, ਲੰਮਾ wid, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqz has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_cmp_eqz(wid);
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_cmp_eq_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_real_ *arith = ਨਵਾਂ vvp_cmp_eq_real;
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_cmp_ne(ਅੱਖਰ*label, ਲੰਮਾ wid, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_cmp_ne(wid);
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_cmp_ne_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_real_ *arith = ਨਵਾਂ vvp_cmp_ne_real;
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_cmp_ge(ਅੱਖਰ*label, ਲੰਮਾ wid, ਤਾਰਕਿਕ signed_flag,
		    ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_cmp_ge(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_cmp_ge_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_real_ *arith = ਨਵਾਂ vvp_cmp_ge_real;
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_cmp_gt(ਅੱਖਰ*label, ਲੰਮਾ wid, ਤਾਰਕਿਕ signed_flag,
		    ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_cmp_gt(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_cmp_gt_r(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_real_ *arith = ਨਵਾਂ vvp_cmp_gt_real;
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_cmp_weq(ਅੱਖਰ*label, ਲੰਮਾ wid,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .cmp/weq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_cmp_weq(wid);

      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_cmp_wne(ਅੱਖਰ*label, ਲੰਮਾ wid,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      ਜੇ (argc != 2) {
	    fprintf(stderr, "%s .cmp/wne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      vvp_arith_ *arith = ਨਵਾਂ vvp_cmp_wne(wid);

      make_arith(arith, label, argc, argv);
}


ਖਾਲੀ compile_delay(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ width,
                   vvp_delay_t*delay, ਬਣਤਰ symb_s arg)
{
      vvp_net_t*net = ਨਵਾਂ vvp_net_t;
      vvp_fun_delay*obj = ਨਵਾਂ vvp_fun_delay(net, width, *delay);
      net->fun = obj;

      ਮਿਟਾਓ delay;

      ਨਿਵੇਸ਼_connect(net, 0, arg.ਪਾਠ);

      define_functor_ਚਿੰਨ੍ਹ(label, net);
      ਮੁਕਤ(label);
}

ਖਾਲੀ compile_delay(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ width,
                   ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv, ਤਾਰਕਿਕ ignore_decay)
{
      vvp_delay_t stub (0, 0, 0);
      ਜੇ (ignore_decay) stub.ਸੈੱਟ_ignore_decay();
      vvp_net_t*net = ਨਵਾਂ vvp_net_t;
      vvp_fun_delay*obj = ਨਵਾਂ vvp_fun_delay(net, width, stub);
      net->fun = obj;

      inputs_connect(net, argc, argv);
      ਮੁਕਤ(argv);

      define_functor_ਚਿੰਨ੍ਹ(label, net);
      ਮੁਕਤ(label);
}

/*
 * Extend nodes.
 */
ਖਾਲੀ compile_extend_signed(ਅੱਖਰ*label, ਲੰਮਾ wid, ਬਣਤਰ symb_s arg)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(wid >= 0);

      vvp_fun_extend_signed*fun = ਨਵਾਂ vvp_fun_extend_signed(wid);
      vvp_net_t*ptr = ਨਵਾਂ vvp_net_t;
      ptr->fun = fun;

      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);

      ਨਿਵੇਸ਼_connect(ptr, 0, arg.ਪਾਠ);
}

ਬਣਤਰ __vpiModPath* compile_modpath(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ width,
                                     ਬਣਤਰ symb_s drv, ਬਣਤਰ symb_s dest)
{
      vvp_net_t*net = ਨਵਾਂ vvp_net_t;
      vvp_fun_modpath*obj = ਨਵਾਂ vvp_fun_modpath(net, width);
      net->fun = obj;

      ਨਿਵੇਸ਼_connect(net, 0, drv.ਪਾਠ);

      define_functor_ਚਿੰਨ੍ਹ(label, net);

      __vpiModPath*modpath = vpip_make_modpath(net);

      compile_vpi_lookup(&modpath->ਰਸਤਾ_ਪਦ_ਬਾਹਰਲਾ.ਵਿਅੰਜਕ, dest.ਪਾਠ);

      ਮੁਕਤ(label);

      modpath->modpath = obj;
      ਵਾਪਸ modpath;
}

ਸਥਿਰ ਬਣਤਰ __vpiModPathSrc*make_modpath_src(ਬਣਤਰ __vpiModPath*ਰਸਤਾ,
					       ਅੱਖਰ ਕਿਨਾਰਾ_c,
					       ਅਚਲ ਬਣਤਰ symb_s&src,
					       ਬਣਤਰ numbv_s&vals,
					       ਤਾਰਕਿਕ ifnone)
{
      vvp_fun_modpath*dst = ਰਸਤਾ->modpath;

      vvp_time64_t use_delay[12];
      ਪ੍ਰਤਿਜ੍ਞਾ(vals.cnt == 12);
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < vals.cnt ;  idx += 1) {
	    use_delay[idx] = vals.nvec[idx];
      }

      numbv_clear(&vals);

      vvp_fun_modpath_src*obj = 0;

      ਪੂਰਨ vpi_ਕਿਨਾਰਾ = vpi_No_ਕਿਨਾਰਾ;
      ਜੇ (ਕਿਨਾਰਾ_c == 0) {
	    obj = ਨਵਾਂ vvp_fun_modpath_src(use_delay);

      } ਨਹੀਂਤਾਂ {
	    ਤਾਰਕਿਕ posedge, negedge;
	    ਚੋਣ (ਕਿਨਾਰਾ_c) {
		ਹਾਲਤ '+':
		  vpi_ਕਿਨਾਰਾ = vpiPosedge;
		  posedge = ਸੱਚ;
		  negedge = ਝੂਠ;
		  ਰੋਕੋ;
		ਹਾਲਤ '-':
		  vpi_ਕਿਨਾਰਾ = vpiNegedge;
		  posedge = ਝੂਠ;
		  negedge = ਸੱਚ;
		  ਰੋਕੋ;
#ਜੇ 0
		ਹਾਲਤ '*':
		  posedge = ਸੱਚ;
		  negedge = ਸੱਚ;
		  ਰੋਕੋ;
#ਜੇ_ਅੰਤ
		ਮੂਲ:
		  posedge = ਝੂਠ;
		  negedge = ਝੂਠ;
		  fprintf(stderr, "Unknown edge identifier %c(%d).\n", ਕਿਨਾਰਾ_c,
		          ਕਿਨਾਰਾ_c);
		  ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    }
	    obj = ਨਵਾਂ vvp_fun_modpath_ਕਿਨਾਰਾ(use_delay, posedge, negedge);
      }

      vvp_net_t*net = ਨਵਾਂ vvp_net_t;
      ਬਣਤਰ __vpiModPathSrc* srcobj = vpip_make_modpath_src(ਰਸਤਾ, net) ;
      vpip_attach_ਮੰਜ਼ਿਲ_current_scope(srcobj);
      net->fun = obj;

	/* Save the vpiEdge directory into the input path term. */
      srcobj->ਰਸਤਾ_ਪਦ_ਵਿੱਚ.ਕਿਨਾਰਾ = vpi_ਕਿਨਾਰਾ;
      ਨਿਵੇਸ਼_connect(net, 0, src.ਪਾਠ);
      dst->ਜੋੜੋ_modpath_src(obj, ifnone);

      ਵਾਪਸ srcobj;
}

ਖਾਲੀ compile_modpath_src(ਬਣਤਰ __vpiModPath*dst, ਅੱਖਰ ਕਿਨਾਰਾ_c,
			 ਅਚਲ ਬਣਤਰ symb_s&src,
			 ਬਣਤਰ numbv_s&vals,
			 ਅਚਲ ਬਣਤਰ symb_s&condit_src,
			 ਅਚਲ ਬਣਤਰ symb_s&ਰਸਤਾ_ਪਦ_ਵਿੱਚ)
{
      ਬਣਤਰ __vpiModPathSrc*obj =
	    make_modpath_src(dst, ਕਿਨਾਰਾ_c, src, vals, ਝੂਠ);
      ਨਿਵੇਸ਼_connect(obj->net, 1, condit_src.ਪਾਠ);
      compile_vpi_lookup(&obj->ਰਸਤਾ_ਪਦ_ਵਿੱਚ.ਵਿਅੰਜਕ, ਰਸਤਾ_ਪਦ_ਵਿੱਚ.ਪਾਠ);
}

ਖਾਲੀ compile_modpath_src(ਬਣਤਰ __vpiModPath*dst, ਅੱਖਰ ਕਿਨਾਰਾ_c,
			 ਅਚਲ ਬਣਤਰ symb_s&src,
			 ਬਣਤਰ numbv_s&vals,
			 ਪੂਰਨ condit_src,
			 ਅਚਲ ਬਣਤਰ symb_s&ਰਸਤਾ_ਪਦ_ਵਿੱਚ,
			 ਤਾਰਕਿਕ ifnone)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(condit_src == 0);
      ਬਣਤਰ __vpiModPathSrc*obj =
	    make_modpath_src(dst, ਕਿਨਾਰਾ_c, src, vals, ifnone);
      compile_vpi_lookup(&obj->ਰਸਤਾ_ਪਦ_ਵਿੱਚ.ਵਿਅੰਜਕ, ਰਸਤਾ_ਪਦ_ਵਿੱਚ.ਪਾਠ);
}

/*
 * A .shift/l statement creates an array of functors for the
 * width. The 0 input is the data vector to be shifted and the 1 input
 * is the amount of the shift. An unconnected shift amount is set to 0.
 */
ਖਾਲੀ compile_shiftl(ਅੱਖਰ*label, ਲੰਮਾ wid, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      vvp_arith_ *arith = ਨਵਾਂ vvp_shiftl(wid);
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_shiftr(ਅੱਖਰ*label, ਲੰਮਾ wid, ਤਾਰਕਿਕ signed_flag,
		    ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਪ੍ਰਤਿਜ੍ਞਾ( wid > 0 );

      vvp_arith_ *arith = ਨਵਾਂ vvp_shiftr(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

ਖਾਲੀ compile_resolver(ਅੱਖਰ*label, ਅੱਖਰ*type, ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      vvp_net_t*net = ਨਵਾਂ vvp_net_t;

      resolv_core*core = 0;

      ਜੇ (strcmp(type,"tri") == 0) {
	    core = ਨਵਾਂ resolv_tri(argc, net, vvp_scalar_t(BIT4_Z, 0,0));

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"tri0") == 0) {
	    core = ਨਵਾਂ resolv_tri(argc, net, vvp_scalar_t(BIT4_0, 5,5));

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"tri1") == 0) {
	    core = ਨਵਾਂ resolv_tri(argc, net, vvp_scalar_t(BIT4_1, 5,5));

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"triand") == 0) {
	    core = ਨਵਾਂ resolv_triand(argc, net);

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"trior") == 0) {
	    core = ਨਵਾਂ resolv_trior(argc, net);

      } ਨਹੀਂਤਾਂ {
	    fprintf(stderr, "invalid resolver type: %s\n", type);
	    compile_errors += 1;
            ਮਿਟਾਓ net;
      }

      ਜੇ (core) {
	    net->fun = core;
	    define_functor_ਚਿੰਨ੍ਹ(label, net);

            ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ = 0 ;  ਅਧਾਰ < argc ;  ਅਧਾਰ += 4) {
	          ਬਿਨਾਨਿਸ਼ਾਨੀ nports = argc - ਅਧਾਰ;
                  ਜੇ (nports > 4)
                        nports = 4;

                  ਜੇ (ਅਧਾਰ > 0) {
                        net = ਨਵਾਂ vvp_net_t;
                        net->fun = ਨਵਾਂ resolv_extend(core, ਅਧਾਰ);
                  }
                  inputs_connect(net, nports, argv+ਅਧਾਰ);
            }
      }
      ਮੁਕਤ(type);
      ਮੁਕਤ(label);
      ਮੁਕਤ(argv);
}

ਖਾਲੀ compile_udp_def(ਪੂਰਨ sequ, ਅੱਖਰ *label, ਅੱਖਰ *name,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ nin, ਬਿਨਾਨਿਸ਼ਾਨੀ init, ਅੱਖਰ **ਸਾਰਣੀ)
{
      ਜੇ (sequ) {
	    vvp_bit4_t init4;
	    ਜੇ (init == 0)
		  init4 = BIT4_0;
	    ਨਹੀਂਤਾਂ ਜੇ (init == 1)
		  init4 = BIT4_1;
	    ਨਹੀਂਤਾਂ
		  init4 = BIT4_X;

	    vvp_udp_seq_s *u = ਨਵਾਂ vvp_udp_seq_s(label, name, nin, init4);
	    u->compile_ਸਾਰਣੀ(ਸਾਰਣੀ);
      } ਨਹੀਂਤਾਂ {
	    vvp_udp_comb_s *u = ਨਵਾਂ vvp_udp_comb_s(label, name, nin);
	    u->compile_ਸਾਰਣੀ(ਸਾਰਣੀ);
      }
      ਮੁਕਤ(label);
}

ਅੱਖਰ **compile_udp_ਸਾਰਣੀ(ਅੱਖਰ **ਸਾਰਣੀ, ਅੱਖਰ *ਪੰਕਤੀ)
{
  ਜੇ (ਸਾਰਣੀ)
    ਪ੍ਰਤਿਜ੍ਞਾ(ਲੜੀ_ਲੰਬਾਈ(*ਸਾਰਣੀ)==ਲੜੀ_ਲੰਬਾਈ(ਪੰਕਤੀ));

  ਅੱਖਰ **tt;
  ਲਈ (tt = ਸਾਰਣੀ; tt && *tt; tt++) { }
  ਪੂਰਨ n = (tt-ਸਾਰਣੀ) + 2;

  ਸਾਰਣੀ = static_cast<ਅੱਖਰ**>(realloc(ਸਾਰਣੀ, n*ਆਕਾਰ(ਅੱਖਰ*)));
  ਸਾਰਣੀ[n-2] = ਪੰਕਤੀ;
  ਸਾਰਣੀ[n-1] = 0x0;

  ਵਾਪਸ ਸਾਰਣੀ;
}


/*
 * The parser uses this function to compile and link an executable
 * opcode. I do this by looking up the opcode in the opcode_table. The
 * table gives the operand structure that is acceptable, so I can
 * process the operands here as well.
 */
ਖਾਲੀ compile_ਕੋਡ(ਅੱਖਰ*label, ਅੱਖਰ*mnem, comp_operands_t opa)
{
	/* First, I can give the label a value that is the current
	   codespace pointer. Don't need the text of the label after
	   this is done. */
      ਜੇ (label)
	    compile_codelabel(label);

	/* Lookup the opcode in the opcode table. */
      ਬਣਤਰ opcode_ਸਾਰਣੀ_s*op = static_cast<ਬਣਤਰ opcode_ਸਾਰਣੀ_s*>
                                            (bsearch(mnem, opcode_ਸਾਰਣੀ, opcode_ਗਿਣਤੀ,
                                                     ਆਕਾਰ(ਬਣਤਰ opcode_ਸਾਰਣੀ_s),
                                                     &opcode_compare));
      ਜੇ (op == 0) {
	    yyerror("Invalid opcode");
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(op);

	/* Build up the code from the information about the opcode and
	   the information from the compiler. */
      vvp_ਕੋਡ_t code = codespace_allocate();
      code->opcode = op->opcode;

      ਜੇ (op->argc != (opa? opa->argc : 0)) {
	    yyerror("operand count");
	    compile_errors += 1;
	    ਵਾਪਸ;
      }

	/* Pull the operands that the instruction expects from the
	   list that the parser supplied. */

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < op->argc ;  idx += 1) {

	    ਚੋਣ (op->argt[idx]) {
		ਹਾਲਤ OA_NONE:
		  ਰੋਕੋ;

		ਹਾਲਤ OA_ARR_PTR:
		  ਜੇ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			ਰੋਕੋ;
		  }

		  compile_array_lookup(code, opa->argv[idx].symb.ਪਾਠ);
		  ਰੋਕੋ;

		ਹਾਲਤ OA_BIT1:
		  ਜੇ (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			ਰੋਕੋ;
		  }
		  code->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[0] = opa->argv[idx].numb;
		  ਰੋਕੋ;

		ਹਾਲਤ OA_BIT2:
		  ਜੇ (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			ਰੋਕੋ;
		  }
		  code->ਦੋਆਧਾਰੀ_ਅੰਕ_idx[1] = opa->argv[idx].numb;
		  ਰੋਕੋ;

		ਹਾਲਤ OA_ਕੋਡ_PTR:
		ਹਾਲਤ OA_ਕੋਡ_PTR2:
		  ਜੇ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			ਰੋਕੋ;
		  }

		  ਪ੍ਰਤਿਜ੍ਞਾ(opa->argv[idx].symb.idx == 0);
		  ਕੋਡ_label_lookup(code, opa->argv[idx].symb.ਪਾਠ,
				    op->argt[idx]==OA_ਕੋਡ_PTR2);
		  ਰੋਕੋ;

		ਹਾਲਤ OA_FUNC_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  ਜੇ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			ਰੋਕੋ;
		  }

		  functor_ref_lookup(&code->net, opa->argv[idx].symb.ਪਾਠ);
		  ਰੋਕੋ;

		ਹਾਲਤ OA_FUNC_PTR2:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  ਜੇ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			ਰੋਕੋ;
		  }

		  functor_ref_lookup(&code->net2, opa->argv[idx].symb.ਪਾਠ);
		  ਰੋਕੋ;

		ਹਾਲਤ OA_ਸੰਖਿਆ:
		  ਜੇ (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			ਰੋਕੋ;
		  }

		  code->ਸੰਖਿਆ = opa->argv[idx].numb;
		  ਰੋਕੋ;

		ਹਾਲਤ OA_VPI_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  ਜੇ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			ਰੋਕੋ;
		  }

		  compile_vpi_lookup(&code->handle, opa->argv[idx].symb.ਪਾਠ);
		  ਰੋਕੋ;

		ਹਾਲਤ OA_STRING:
		  ਜੇ (opa->argv[idx].ltype != L_STRING) {
			yyerror("operand format");
			ਰੋਕੋ;
		  }

		  code->ਪਾਠ = opa->argv[idx].ਪਾਠ;
		  ਰੋਕੋ;
	    }
      }

      ਮੁਕਤ(opa);

      ਮੁਕਤ(mnem);
}

ਖਾਲੀ compile_codelabel(ਅੱਖਰ*label)
{
      ਚਿੰਨ੍ਹ_ਮੁੱਲ_t val;
      vvp_ਕੋਡ_t ptr = codespace_ਅਗਲਾ();

      val.ptr = ptr;
      sym_ਸੈੱਟ_ਮੁੱਲ(sym_codespace, label, val);

      ਮੁਕਤ(label);
}

ਖਾਲੀ compile_file_ਸਤਰ(ਅੱਖਰ*label, ਲੰਮਾ file_idx, ਲੰਮਾ lineno,
                       ਅੱਖਰ*description)
{
      ਜੇ (label) compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_ਕੋਡ_t code = codespace_allocate();
      code->opcode = &ਦਾ_FILE_ਸਤਰ;

	/* Create a vpiHandle that contains the information. */
      code->handle = vpip_build_file_ਸਤਰ(description, file_idx, lineno);
      ਪ੍ਰਤਿਜ੍ਞਾ(code->handle);

	/* Done with the lexor-allocated name string. */
      ਮਿਟਾਓ[] description;
}

ਖਾਲੀ compile_vpi_call(ਅੱਖਰ*label, ਅੱਖਰ*name,
                      ਤਾਰਕਿਕ func_as_task_err, ਤਾਰਕਿਕ func_as_task_warn,
                      ਲੰਮਾ file_idx, ਲੰਮਾ lineno,
                      ਬਿਨਾਨਿਸ਼ਾਨੀ argc, vpiHandle*argv,
		      ਬਿਨਾਨਿਸ਼ਾਨੀ vec4_ਢੇਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ real_ਢੇਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ string_ਢੇਰ)
{
      ਜੇ (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_ਕੋਡ_t code = codespace_allocate();
      code->opcode = &ਦਾ_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, 0, 0,
					 0, func_as_task_err, func_as_task_warn,
                                         argc, argv, vec4_ਢੇਰ, real_ਢੇਰ, string_ਢੇਰ,
					 file_idx, lineno);
      ਜੇ (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      ਮਿਟਾਓ[] name;
}

ਖਾਲੀ compile_vpi_func_call(ਅੱਖਰ*label, ਅੱਖਰ*name,
			   ਪੂਰਨ val_type, ਬਿਨਾਨਿਸ਼ਾਨੀ val_wid,
			   ਲੰਮਾ file_idx, ਲੰਮਾ lineno,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ argc, vpiHandle*argv,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ vec4_ਢੇਰ,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ real_ਢੇਰ,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ string_ਢੇਰ)
{
      ਜੇ (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_ਕੋਡ_t code = codespace_allocate();
      code->opcode = &ਦਾ_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, val_type, val_wid,
					 0, ਸੱਚ, ਝੂਠ,
                                         argc, argv, vec4_ਢੇਰ, real_ਢੇਰ, string_ਢੇਰ,
					 file_idx, lineno);
      ਜੇ (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      ਮਿਟਾਓ[] name;
}

/*
 * When the parser finds a thread statement, I create a new thread
 * with the start address referenced by the program symbol passed to
 * me.
 */
ਖਾਲੀ compile_thread(ਅੱਖਰ*ਸ਼ੁਰੂ_sym, ਅੱਖਰ*flag)
{
      ਤਾਰਕਿਕ ਧੱਕੋ_flag = ਝੂਠ;

      ਚਿੰਨ੍ਹ_ਮੁੱਲ_t tmp = sym_get_ਮੁੱਲ(sym_codespace, ਸ਼ੁਰੂ_sym);
      vvp_ਕੋਡ_t pc = reinterpret_cast<vvp_ਕੋਡ_t>(tmp.ptr);
      ਜੇ (pc == 0) {
	    yyerror("unresolved address");
	    ਵਾਪਸ;
      }

      ਜੇ (flag && (strcmp(flag,"$push") == 0))
	    ਧੱਕੋ_flag = ਸੱਚ;

      vthread_t thr = vthread_new(pc, vpip_peek_current_scope());

      ਜੇ (flag && (strcmp(flag,"$init") == 0))
	    ਸਮਾਂ_ਸਾਰਣੀ_init_vthread(thr);
      ਨਹੀਂਤਾਂ ਜੇ (flag && (strcmp(flag,"$final") == 0))
	    ਸਮਾਂ_ਸਾਰਣੀ_final_vthread(thr);
      ਨਹੀਂਤਾਂ
	    ਸਮਾਂ_ਸਾਰਣੀ_vthread(thr, 0, ਧੱਕੋ_flag);

      ਮੁਕਤ(ਸ਼ੁਰੂ_sym);
      ਮੁਕਤ(flag);
}

ਖਾਲੀ compile_param_logic(ਅੱਖਰ*label, ਅਚਲ ਅੱਖਰ*name, ਅੱਖਰ*value, ਤਾਰਕਿਕ signed_flag,
                         ਤਾਰਕਿਕ local_flag,
                         ਲੰਮਾ file_idx, ਲੰਮਾ lineno)
{
      vvp_vector4_t value4 = c4string_ਮੰਜ਼ਿਲ_vector4(value);
      vpiHandle obj = vpip_make_ਦੁਆਧਾਰੀ_param(name, value4, signed_flag,
                                             local_flag, file_idx, lineno);
      compile_vpi_ਚਿੰਨ੍ਹ(label, obj);
      vpip_attach_ਮੰਜ਼ਿਲ_current_scope(obj);

      ਮੁਕਤ(label);
      ਮੁਕਤ(value);
}

ਖਾਲੀ compile_param_string(ਅੱਖਰ*label, ਅਚਲ ਅੱਖਰ*name, ਅੱਖਰ*value,
                          ਤਾਰਕਿਕ local_flag,
                          ਲੰਮਾ file_idx, ਲੰਮਾ lineno)
{
	// name and value become owned by vpip_make_string_param
      vpiHandle obj = vpip_make_string_param(name, value, local_flag, file_idx, lineno);
      compile_vpi_ਚਿੰਨ੍ਹ(label, obj);
      vpip_attach_ਮੰਜ਼ਿਲ_current_scope(obj);

      ਮੁਕਤ(label);
}

ਖਾਲੀ compile_param_real(ਅੱਖਰ*label, ਅਚਲ ਅੱਖਰ*name, ਅੱਖਰ*value,
                        ਤਾਰਕਿਕ local_flag,
                        ਲੰਮਾ file_idx, ਲੰਮਾ lineno)
{
      ਦੁੱਗਣਾ dvalue = crstring_ਮੰਜ਼ਿਲ_double(value);
      vpiHandle obj = vpip_make_real_param(name, dvalue, local_flag, file_idx, lineno);
      compile_vpi_ਚਿੰਨ੍ਹ(label, obj);
      vpip_attach_ਮੰਜ਼ਿਲ_current_scope(obj);

      ਮੁਕਤ(label);
      ਮੁਕਤ(value);
}

ਖਾਲੀ compile_island(ਅੱਖਰ*label, ਅੱਖਰ*type)
{
      ਜੇ (strcmp(type,"tran") == 0)
	    compile_island_tran(label);
      ਨਹੀਂਤਾਂ
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);

      ਮੁਕਤ(type);
}
