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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MINGW32__
#ಅನ್ತರ್ಭಾವ <windows.h>
#ಆದರೆ_ಅಂತ್ಯ

# include  "ivl_alloc.h"

ಚಿಹ್ನೆರಹಿತ compile_errors = 0;

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

ಎಣಿಕೆ ಕಾರ್ಯಾಂಶ_e {
	/* Place holder for unused operand */
      OA_NONE,
	/* The operand is a number, an immediate unsigned integer */
      OA_ಸಂಖ್ಯೆ,
	/* The operand is a pointer to an array. */
      OA_ARR_PTR,
	/* The operand is a thread bit index or short integer */
      OA_BIT1,
      OA_BIT2,
	/* The operand is a pointer to code space */
      OA_ಸಂಕೇತ_PTR,
      OA_ಸಂಕೇತ_PTR2,
	/* The operand is a variable or net pointer */
      OA_FUNC_PTR,
	/* The operand is a second functor pointer */
      OA_FUNC_PTR2,
	/* The operand is a VPI handle */
      OA_VPI_PTR,
	/* String */
      OA_STRING
};

ರಚನೆ opcode_ಕೋಷ್ಟಕ_s {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*mnemonic;
      vvp_ಸಂಕೇತ_fun opcode;

      ಚಿಹ್ನೆರಹಿತ argc;
      ಎಣಿಕೆ ಕಾರ್ಯಾಂಶ_e argt[ಕಾರ್ಯಾಂಶ_MAX];
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ opcode_ಕೋಷ್ಟಕ_s opcode_ಕೋಷ್ಟಕ[] = {
      { "%abs/wr", ದ_ABS_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add",    ದ_ಕೂಡಿಸು,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add/wr", ದ_ಕೂಡಿಸು_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%addi",   ದ_ADDI,   3,  {OA_BIT1,     OA_BIT2,     OA_ಸಂಖ್ಯೆ} },
      { "%alloc",  ದ_ALLOC,  1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%and",    ದ_AND,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%and/r",  ದ_ANDR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%assign/ar",ದ_ನಿಯೋಜಿಸು_AR,2,{OA_ARR_PTR,OA_BIT1,     OA_NONE} },
      { "%assign/ar/d",ದ_ನಿಯೋಜಿಸು_ARD,2,{OA_ARR_PTR,OA_BIT1,  OA_NONE} },
      { "%assign/ar/e",ದ_ನಿಯೋಜಿಸು_ARE,1,{OA_ARR_PTR,OA_NONE,  OA_NONE} },
      { "%assign/vec4",      ದ_ನಿಯೋಜಿಸು_VEC4,       2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/a/d",  ದ_ನಿಯೋಜಿಸು_VEC4_A_D,   3, {OA_ARR_PTR,  OA_BIT1, OA_BIT2} },
      { "%assign/vec4/a/e",  ದ_ನಿಯೋಜಿಸು_VEC4_A_E,   2, {OA_ARR_PTR,  OA_BIT1, OA_NONE} },
      { "%assign/vec4/d",    ದ_ನಿಯೋಜಿಸು_VEC4_D,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/e",    ದ_ನಿಯೋಜಿಸು_VEC4_E,      1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%assign/vec4/off/d",ದ_ನಿಯೋಜಿಸು_VEC4_OFF_D, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%assign/vec4/off/e",ದ_ನಿಯೋಜಿಸು_VEC4_OFF_E, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr",  ದ_ನಿಯೋಜಿಸು_WR, 2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/d",ದ_ನಿಯೋಜಿಸು_WRD,2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/e",ದ_ನಿಯೋಜಿಸು_WRE,1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%blend",     ದ_BLEND,     0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/str", ದ_BLEND_STR, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/wr",  ದ_BLEND_WR,  0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%breakpoint", ದ_BREAKPOINT, 0,  {OA_NONE, OA_NONE, OA_NONE} },
      { "%callf/obj",       ದ_CALLF_OBJ,       2,{OA_ಸಂಕೇತ_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/real",      ದ_CALLF_REAL,      2,{OA_ಸಂಕೇತ_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/str",       ದ_CALLF_STR,       2,{OA_ಸಂಕೇತ_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/vec4",      ದ_CALLF_VEC4,      2,{OA_ಸಂಕೇತ_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/void",      ದ_CALLF_VOID,      2,{OA_ಸಂಕೇತ_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%cassign/link",    ದ_CASSIGN_LINK,    2,{OA_FUNC_PTR,OA_FUNC_PTR2,OA_NONE} },
      { "%cassign/vec4",    ದ_CASSIGN_VEC4,    1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cassign/vec4/off",ದ_CASSIGN_VEC4_OFF,2,{OA_FUNC_PTR,OA_BIT1,     OA_NONE} },
      { "%cassign/wr",  ದ_CASSIGN_WR,  1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cast/vec2/dar", ದ_CAST_VEC2_DAR, 1,  {OA_ಸಂಖ್ಯೆ,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/dar", ದ_CAST_VEC4_DAR, 1,  {OA_ಸಂಖ್ಯೆ,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/str", ದ_CAST_VEC4_STR, 1,  {OA_ಸಂಖ್ಯೆ,   OA_NONE,     OA_NONE} },
      { "%cast2",   ದ_CAST2,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/e",   ದ_CMPE,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/ne",  ದ_CMPNE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/s",   ದ_CMPS,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/str", ದ_CMPSTR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/u",   ದ_CMPU,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/we",  ದ_CMPWE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wne", ದ_CMPWNE, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wr",  ದ_CMPWR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/x",   ದ_CMPX,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/z",   ದ_CMPZ,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmpi/e",  ದ_CMPIE,  3,  {OA_BIT1,     OA_BIT2,     OA_ಸಂಖ್ಯೆ} },
      { "%cmpi/ne", ದ_CMPINE, 3,  {OA_BIT1,     OA_BIT2,     OA_ಸಂಖ್ಯೆ} },
      { "%cmpi/s",  ದ_CMPIS,  3,  {OA_BIT1,     OA_BIT2,     OA_ಸಂಖ್ಯೆ} },
      { "%cmpi/u",  ದ_CMPIU,  3,  {OA_BIT1,     OA_BIT2,     OA_ಸಂಖ್ಯೆ} },
      { "%cmpix/ltu", ದ_CMPIX_LTU, 2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%cmpix/slt0", ದ_CMPIX_SLT0, 1, {OA_ಸಂಖ್ಯೆ, OA_NONE, OA_NONE} },
      { "%concat/str",  ದ_CONCAT_STR,  0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concat/vec4", ದ_CONCAT_VEC4, 0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concati/str", ದ_CONCATI_STR, 1,{OA_STRING,OA_NONE,  OA_NONE} },
      { "%concati/vec4",ದ_CONCATI_VEC4,3,{OA_BIT1,  OA_BIT2,  OA_ಸಂಖ್ಯೆ} },
      { "%cvt/rv",   ದ_CVT_RV,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/rv/s", ದ_CVT_RV_S,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/sr", ದ_CVT_SR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/ur", ದ_CVT_UR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/vr", ದ_CVT_VR, 1,  {OA_ಸಂಖ್ಯೆ,   OA_NONE,     OA_NONE} },
      { "%deassign",ದ_DEASSIGN,3,{OA_FUNC_PTR, OA_BIT1,     OA_BIT2} },
      { "%deassign/wr",ದ_DEASSIGN_WR,1,{OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%debug/thr",  ದ_DEBUG_THR,  1,{OA_STRING,   OA_NONE,     OA_NONE} },
      { "%delay",  ದ_DELAY,  2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%delayx", ದ_DELAYX, 1,  {OA_ಸಂಖ್ಯೆ,   OA_NONE,     OA_NONE} },
      { "%delete/elem",ದ_DELETE_ELEM,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%delete/obj",ದ_DELETE_OBJ,1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%delete/prop/elem", ದ_DELETE_PROP_ELEM, 1, {OA_ಸಂಖ್ಯೆ, OA_NONE, OA_NONE} },
      { "%delete/prop/obj",  ದ_DELETE_PROP_OBJ,  1, {OA_ಸಂಖ್ಯೆ, OA_NONE, OA_NONE} },
      { "%delete/tail",ದ_DELETE_ಬಾಲ,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%disable",  ದ_DISABLE, 1, {OA_VPI_PTR,OA_NONE,     OA_NONE} },
      { "%disable/flow", ದ_DISABLE_FLOW, 1, {OA_VPI_PTR,OA_NONE, OA_NONE} },
      { "%disable/fork",ದ_DISABLE_FORK,0,{OA_NONE,OA_NONE,  OA_NONE} },
      { "%div",      ದ_DIV,     0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/s",    ದ_DIV_S,   0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/wr",   ದ_DIV_WR,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/obj",  ದ_DUP_OBJ, 0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/real", ದ_DUP_REAL,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/vec4", ದ_DUP_VEC4,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%end",    ದ_ಕೊನೆ,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl",  ದ_EVCTL,  2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/c",ದ_EVCTLC, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl/i",ದ_EVCTLI, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/s",ದ_EVCTLS, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%event",    ದ_EVENT,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%event/nb", ದ_EVENT_NB, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%flag_get/vec4", ದ_FLAG_GET_VEC4, 1, {OA_ಸಂಖ್ಯೆ, OA_NONE, OA_NONE} },
      { "%flag_inv",      ದ_FLAG_INV,      1, {OA_BIT1,   OA_NONE, OA_NONE} },
      { "%flag_mov",      ದ_FLAG_MOV,      2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_or",       ದ_FLAG_OR,       2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_set/imm",  ದ_FLAG_ಗಣ_IMM,  2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%flag_set/vec4", ದ_FLAG_ಗಣ_VEC4, 1, {OA_ಸಂಖ್ಯೆ, OA_NONE, OA_NONE} },
      { "%force/link",    ದ_FORCE_LINK,2,{OA_FUNC_PTR, OA_FUNC_PTR2, OA_NONE} },
      { "%force/vec4",    ದ_FORCE_VEC4,    1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%force/vec4/off",ದ_FORCE_VEC4_OFF,2,{OA_FUNC_PTR, OA_BIT1,      OA_NONE} },
      { "%force/vec4/off/d",ದ_FORCE_VEC4_OFF_D,3,{OA_FUNC_PTR, OA_BIT1,  OA_BIT2} },
      { "%force/wr",      ದ_FORCE_WR,      1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%fork",   ದ_FORK,   2,  {OA_ಸಂಕೇತ_PTR2,OA_VPI_PTR,  OA_NONE} },
      { "%free",   ದ_FREE,   1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%inv",    ದ_INV,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%ix/add", ದ_IX_ಕೂಡಿಸು, 3,  {OA_ಸಂಖ್ಯೆ,   OA_BIT1,     OA_BIT2} },
      { "%ix/getv",ದ_IX_GETV,2,  {OA_BIT1,     OA_FUNC_PTR, OA_NONE} },
      { "%ix/getv/s",ದ_IX_GETV_S,2, {OA_BIT1,   OA_FUNC_PTR, OA_NONE} },
      { "%ix/load",ದ_IX_ತುಂಬು,3,  {OA_ಸಂಖ್ಯೆ,   OA_BIT1,     OA_BIT2} },
      { "%ix/mov", ದ_IX_MOV, 2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%ix/mul", ದ_IX_MUL, 3,  {OA_ಸಂಖ್ಯೆ,   OA_BIT1,     OA_BIT2} },
      { "%ix/sub", ದ_IX_SUB, 3,  {OA_ಸಂಖ್ಯೆ,   OA_BIT1,     OA_BIT2} },
      { "%ix/vec4",  ದ_IX_VEC4,  1,  {OA_ಸಂಖ್ಯೆ,   OA_NONE, OA_NONE} },
      { "%ix/vec4/s",ದ_IX_VEC4_S,1,  {OA_ಸಂಖ್ಯೆ,   OA_NONE, OA_NONE} },
      { "%jmp",    ದ_JMP,    1,  {OA_ಸಂಕೇತ_PTR, OA_NONE,     OA_NONE} },
      { "%jmp/0",  ದ_JMP0,   2,  {OA_ಸಂಕೇತ_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/0xz",ದ_JMP0_XZ, 2,  {OA_ಸಂಕೇತ_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1",  ದ_JMP1,   2,  {OA_ಸಂಕೇತ_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1xz",ದ_JMP1_XZ, 2,  {OA_ಸಂಕೇತ_PTR, OA_BIT1,     OA_NONE} },
      { "%join",   ದ_JOIN,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%join/detach",ದ_JOIN_DETACH,1,{OA_ಸಂಖ್ಯೆ,OA_NONE,  OA_NONE} },
      { "%load/ar",ದ_ತುಂಬು_AR,2,  {OA_ARR_PTR,  OA_BIT1,     OA_NONE} },
      { "%load/dar/r",  ದ_ತುಂಬು_DAR_R,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE}},
      { "%load/dar/str",ದ_ತುಂಬು_DAR_STR,  1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/dar/vec4",ದ_ತುಂಬು_DAR_VEC4,1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/obj",   ದ_ತುಂಬು_OBJ,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/obja",  ದ_ತುಂಬು_OBJA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/prop/dar/vec4", ದ_ತುಂಬು_PROP_DAR_VEC4, 2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%load/real",  ದ_ತುಂಬು_REAL, 1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%load/str",   ದ_ತುಂಬು_STR,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/stra",  ದ_ತುಂಬು_STRA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/vec4",  ದ_ತುಂಬು_VEC4, 1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%load/vec4a", ದ_ತುಂಬು_VEC4_A,2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%max/wr", ದ_MAX_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%min/wr", ದ_MIN_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod",    ದ_MOD,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/s",  ದ_MOD_S,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/wr", ದ_MOD_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul",    ದ_MUL,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul/wr", ದ_MUL_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%muli",   ದ_MULI,   3,  {OA_BIT1,     OA_BIT2,     OA_ಸಂಖ್ಯೆ} },
      { "%nand",   ದ_NAND,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nand/r", ದ_NANDR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%neg/wr", ದ_NEG_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%new/cobj",  ದ_NEW_COBJ,  1, {OA_VPI_PTR,OA_NONE,  OA_NONE} },
      { "%new/darray",ದ_NEW_DARRAY,2, {OA_BIT1,   OA_STRING,OA_NONE} },
      { "%noop",   ದ_NOOP,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor",    ದ_NOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor/r",  ದ_NORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%null",   ದ_NULL,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or",     ದ_OR,     0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or/r",   ದ_ORR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%pad/s",  ದ_PAD_S,  1,  {OA_ಸಂಖ್ಯೆ,   OA_NONE,     OA_NONE} },
      { "%pad/u",  ದ_PAD_U,  1,  {OA_ಸಂಖ್ಯೆ,   OA_NONE,     OA_NONE} },
      { "%part/s", ದ_PART_S, 1,  {OA_ಸಂಖ್ಯೆ,   OA_NONE,     OA_NONE} },
      { "%part/u", ದ_PART_U, 1,  {OA_ಸಂಖ್ಯೆ,   OA_NONE,     OA_NONE} },
      { "%parti/s",ದ_PARTI_S,3,  {OA_ಸಂಖ್ಯೆ,   OA_BIT1,     OA_BIT2} },
      { "%parti/u",ದ_PARTI_U,3,  {OA_ಸಂಖ್ಯೆ,   OA_BIT1,     OA_BIT2} },
      { "%pop/obj", ದ_ತೆಗೆ_OBJ, 2, {OA_BIT1,    OA_BIT2,     OA_NONE} },
      { "%pop/real",ದ_ತೆಗೆ_REAL,1, {OA_ಸಂಖ್ಯೆ,  OA_NONE,     OA_NONE} },
      { "%pop/str", ದ_ತೆಗೆ_STR, 1, {OA_ಸಂಖ್ಯೆ,  OA_NONE,     OA_NONE} },
      { "%pop/vec4",ದ_ತೆಗೆ_VEC4,1, {OA_ಸಂಖ್ಯೆ,  OA_NONE,     OA_NONE} },
      { "%pow",     ದ_POW,     0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/s",   ದ_POW_S,   0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/wr",  ದ_POW_WR,  0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%prop/obj",ದ_PROP_OBJ,2,  {OA_ಸಂಖ್ಯೆ,   OA_BIT1,     OA_NONE} },
      { "%prop/queue/size", ದ_PROP_ಸರತಿ_ಗಾತ್ರ,  1, {OA_ಸಂಖ್ಯೆ, OA_NONE, OA_NONE} },
      { "%prop/r",  ದ_PROP_R,  1,  {OA_ಸಂಖ್ಯೆ,   OA_NONE,     OA_NONE} },
      { "%prop/str",ದ_PROP_STR,1,  {OA_ಸಂಖ್ಯೆ,   OA_NONE,     OA_NONE} },
      { "%prop/v",  ದ_PROP_V,  1,  {OA_ಸಂಖ್ಯೆ,   OA_NONE,     OA_NONE} },
      { "%push/ix/vec4", ದ_ತಳ್ಳು_IX_VEC4, 3, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_BIT2} },
      { "%pushi/real",ದ_PUSHI_REAL,2,{OA_BIT1,   OA_BIT2,   OA_NONE} },
      { "%pushi/str", ದ_PUSHI_STR, 1,{OA_STRING, OA_NONE,   OA_NONE} },
      { "%pushi/vec4",ದ_PUSHI_VEC4,3,{OA_BIT1,   OA_BIT2,   OA_ಸಂಖ್ಯೆ} },
      { "%pushv/str", ದ_PUSHV_STR, 0,{OA_NONE,   OA_NONE,   OA_NONE} },
      { "%putc/str/vec4",ದ_PUTC_STR_VEC4,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/prop/r",   ದ_QINSERT_PROP_R,   2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/str", ದ_QINSERT_PROP_STR, 2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/v",   ದ_QINSERT_PROP_V,   3, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_BIT2} },
      { "%qinsert/real",ದ_QINSERT_REAL,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/str", ದ_QINSERT_STR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/v",   ದ_QINSERT_V,   3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%qpop/b/real",ದ_QPOP_B_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/str", ದ_QPOP_B_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/v",   ದ_QPOP_B_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/f/real",ದ_QPOP_F_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/str", ದ_QPOP_F_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/v",   ದ_QPOP_F_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/prop/b/r",   ದ_QPOP_PROP_B_REAL, 1, {OA_ಸಂಖ್ಯೆ, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/str", ದ_QPOP_PROP_B_STR, 1, {OA_ಸಂಖ್ಯೆ, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/v",   ದ_QPOP_PROP_B_V,    2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%qpop/prop/f/r",   ದ_QPOP_PROP_F_REAL, 1, {OA_ಸಂಖ್ಯೆ, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/str", ದ_QPOP_PROP_F_STR, 1, {OA_ಸಂಖ್ಯೆ, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/v",   ದ_QPOP_PROP_F_V,    2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%queue/append_word/v", ದ_ಸರತಿ_APPEND_ಶಬ್ದ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/find/index/prop/v", ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ_PROP_V, 2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%queue/find/index/v", ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find/prop/v", ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_PROP_V, 2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%queue/find/v", ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/prop/v", ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಸೂಚ್ಯಂಕ_PROP_V, 2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/v", ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಸೂಚ್ಯಂಕ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/prop/v", ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_PROP_V, 2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%queue/find_first/v", ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/prop/v", ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ_PROP_V, 2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/v", ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/prop/v", ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_PROP_V, 2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%queue/find_last/v", ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/max/obj/v", ದ_ಸರತಿ_MAX_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/max/prop/v", ದ_ಸರತಿ_MAX_PROP_V, 2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%queue/max/v", ದ_ಸರತಿ_MAX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/min/obj/v", ದ_ಸರತಿ_MIN_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/min/prop/v", ದ_ಸರತಿ_MIN_PROP_V, 2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%queue/min/v", ದ_ಸರತಿ_MIN_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/new_empty/v", ದ_ಸರತಿ_NEW_ಖಾಲಿಗಳು_V, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%queue/size/v", ದ_ಸರತಿ_ಗಾತ್ರ_V, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%queue/unique/index/obj/v", ದ_ಸರತಿ_UNIQUE_ಸೂಚ್ಯಂಕ_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/index/prop/v", ದ_ಸರತಿ_UNIQUE_ಸೂಚ್ಯಂಕ_PROP_V, 2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%queue/unique/index/v", ದ_ಸರತಿ_UNIQUE_ಸೂಚ್ಯಂಕ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/unique/obj/v", ದ_ಸರತಿ_UNIQUE_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/prop/v", ದ_ಸರತಿ_UNIQUE_PROP_V, 2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%queue/unique/v", ದ_ಸರತಿ_UNIQUE_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/word/prop/v", ದ_ಸರತಿ_ಶಬ್ದ_PROP_V, 3, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_BIT2} },
      { "%queue/word/v", ದ_ಸರತಿ_ಶಬ್ದ_V, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%release/net",ದ_RELEASE_NET,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/reg",ದ_RELEASE_REG,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/wr", ದ_RELEASE_WR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%replicate", ದ_REPLICATE,   1,{OA_ಸಂಖ್ಯೆ,  OA_NONE,OA_NONE} },
      { "%ret/real",  ದ_RET_REAL,    1,{OA_ಸಂಖ್ಯೆ,  OA_NONE,OA_NONE} },
      { "%ret/str",   ದ_RET_STR,     1,{OA_ಸಂಖ್ಯೆ,  OA_NONE,OA_NONE} },
      { "%ret/vec4",  ದ_RET_VEC4,    3,{OA_ಸಂಖ್ಯೆ,  OA_BIT1,OA_BIT2} },
      { "%retload/real",ದ_RETLOAD_REAL,1,{OA_ಸಂಖ್ಯೆ,  OA_NONE,OA_NONE} },
      { "%retload/str", ದ_RETLOAD_STR, 1,{OA_ಸಂಖ್ಯೆ,  OA_NONE,OA_NONE} },
      { "%retload/vec4",ದ_RETLOAD_VEC4,1,{OA_ಸಂಖ್ಯೆ,  OA_NONE,OA_NONE} },
      { "%reverse/obj", ದ_ಹಿಮ್ಮುಖ_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%reverse/prop/obj", ದ_ಹಿಮ್ಮುಖ_PROP_OBJ, 1, {OA_ಸಂಖ್ಯೆ, OA_NONE, OA_NONE} },
      { "%rsort/obj", ದ_RSORT_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%rsort/prop/obj", ದ_RSORT_PROP_OBJ, 1, {OA_ಸಂಖ್ಯೆ, OA_NONE, OA_NONE} },
      { "%scopy",  ದ_SCOPY,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%set/dar/obj/real",ದ_ಗಣ_DAR_OBJ_REAL,1,{OA_ಸಂಖ್ಯೆ,OA_NONE,OA_NONE} },
      { "%set/dar/obj/str", ದ_ಗಣ_DAR_OBJ_STR, 1,{OA_ಸಂಖ್ಯೆ,OA_NONE,OA_NONE} },
      { "%set/dar/obj/vec4",ದ_ಗಣ_DAR_OBJ_VEC4,1,{OA_ಸಂಖ್ಯೆ,OA_NONE,OA_NONE} },
      { "%shiftl",   ದ_SHIFTL,   1, {OA_ಸಂಖ್ಯೆ, OA_NONE,   OA_NONE} },
      { "%shiftr",   ದ_SHIFTR,   1, {OA_ಸಂಖ್ಯೆ, OA_NONE,   OA_NONE} },
      { "%shiftr/s", ದ_SHIFTR_S, 1, {OA_ಸಂಖ್ಯೆ, OA_NONE,   OA_NONE} },
      { "%shuffle/obj", ದ_SHUFFLE_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%shuffle/prop/obj", ದ_SHUFFLE_PROP_OBJ, 1, {OA_ಸಂಖ್ಯೆ, OA_NONE, OA_NONE} },
      { "%sort/obj", ದ_ವಿಂಗಡಿಸು_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%sort/prop/obj", ದ_ವಿಂಗಡಿಸು_PROP_OBJ, 1, {OA_ಸಂಖ್ಯೆ, OA_NONE, OA_NONE} },
      { "%split/vec4",    ದ_ವಿಭಜಿಸು_VEC4,    1,{OA_ಸಂಖ್ಯೆ,   OA_NONE, OA_NONE} },
      { "%store/dar/r",   ದ_STORE_DAR_R,   1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/str", ದ_STORE_DAR_STR, 1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/vec4",ದ_STORE_DAR_VEC4,1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/obj",   ದ_STORE_OBJ,   1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/obja",  ದ_STORE_OBJA,  2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/prop/obj",ದ_STORE_PROP_OBJ,2, {OA_ಸಂಖ್ಯೆ,  OA_BIT1, OA_NONE} },
      { "%store/prop/qb/r",   ದ_STORE_PROP_QB_R,   2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/str", ದ_STORE_PROP_QB_STR, 2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/v",   ದ_STORE_PROP_QB_V,   3, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_BIT2} },
      { "%store/prop/qf/r",   ದ_STORE_PROP_QF_R,   2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/str", ದ_STORE_PROP_QF_STR, 2, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/v",   ದ_STORE_PROP_QF_V,   3, {OA_ಸಂಖ್ಯೆ, OA_BIT1, OA_BIT2} },
      { "%store/prop/r",  ದ_STORE_PROP_R,  1, {OA_ಸಂಖ್ಯೆ,  OA_NONE, OA_NONE} },
      { "%store/prop/str",ದ_STORE_PROP_STR,1, {OA_ಸಂಖ್ಯೆ,  OA_NONE, OA_NONE} },
      { "%store/prop/v",  ದ_STORE_PROP_V,  2, {OA_ಸಂಖ್ಯೆ,  OA_BIT1, OA_NONE} },
      { "%store/qb/r",   ದ_STORE_QB_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/str", ದ_STORE_QB_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/v",   ದ_STORE_QB_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qdar/r",  ದ_STORE_QDAR_R,  2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/str",ದ_STORE_QDAR_STR,2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/v",  ದ_STORE_QDAR_V,  3,{OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qf/r",   ದ_STORE_QF_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/str", ದ_STORE_QF_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/v",   ದ_STORE_QF_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qobj/r",  ದ_STORE_QOBJ_R,  2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/str",ದ_STORE_QOBJ_STR,2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/v",  ದ_STORE_QOBJ_V,  3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/real",    ದ_STORE_REAL,    1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/reala",   ದ_STORE_REALA,   2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/str",     ದ_STORE_STR,     1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/stra",    ದ_STORE_STRA,    2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/vec4",    ದ_STORE_VEC4,    3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/vec4a",   ದ_STORE_VEC4_A,   3, {OA_ARR_PTR, OA_BIT1, OA_BIT2} },
      { "%sub",    ದ_SUB,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%sub/wr", ದ_SUB_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%subi",   ದ_SUBI,   3,  {OA_BIT1,     OA_BIT2,     OA_ಸಂಖ್ಯೆ} },
      { "%substr",     ದ_SUBSTR,     2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%substr/vec4",ದ_SUBSTR_VEC4,2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%test_nul",     ದ_TEST_NUL,     1,{OA_FUNC_PTR,OA_NONE,    OA_NONE} },
      { "%test_nul/a",   ದ_TEST_NUL_A,   2,{OA_ARR_PTR, OA_BIT1,    OA_NONE} },
      { "%test_nul/obj", ದ_TEST_NUL_OBJ, 0,{OA_NONE,    OA_NONE,    OA_NONE} },
      { "%test_nul/prop",ದ_TEST_NUL_PROP,2,{OA_ಸಂಖ್ಯೆ,  OA_BIT1,    OA_NONE} },
      { "%uparr/and/v",          ದ_UPARR_AND_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/or/v",           ದ_UPARR_OR_V,           2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/product/obj/v",  ದ_UPARR_ಗುಣಲಬ್ಧ_OBJ_V,  1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/product/prop/v", ದ_UPARR_ಗುಣಲಬ್ಧ_PROP_V, 2, {OA_ಸಂಖ್ಯೆ,   OA_BIT1, OA_NONE} },
      { "%uparr/product/r",      ದ_UPARR_ಗುಣಲಬ್ಧ_REAL,   1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/product/v",      ದ_UPARR_ಗುಣಲಬ್ಧ_V,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/sum/obj/v",      ದ_UPARR_ಮೊತ್ತ_OBJ_V,      1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/sum/prop/v",     ದ_UPARR_ಮೊತ್ತ_PROP_V,     2, {OA_ಸಂಖ್ಯೆ,   OA_BIT1, OA_NONE} },
      { "%uparr/sum/r",          ದ_UPARR_ಮೊತ್ತ_REAL,       1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/sum/v",          ದ_UPARR_ಮೊತ್ತ_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/xor/v",          ದ_UPARR_XOR_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%wait",   ದ_WAIT,   1,  {OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%wait/fork",ದ_WAIT_FORK,0,{OA_NONE,   OA_NONE,     OA_NONE} },
      { "%xnor",   ದ_XNOR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xnor/r", ದ_XNORR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor",    ದ_XOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor/r",  ದ_XORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { 0, ದ_NOOP, 0, {OA_NONE, OA_NONE, OA_NONE} }
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ opcode_ಎಣಿಕೆ =
                    ಗಾತ್ರ(opcode_ಕೋಷ್ಟಕ)/ಗಾತ್ರ(*opcode_ಕೋಷ್ಟಕ) - 1;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ opcode_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ*k, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ*r)
{
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*kp = static_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(k);
      ಸ್ಥಿರಾಂಕ ರಚನೆ opcode_ಕೋಷ್ಟಕ_s*rp = static_cast<ಸ್ಥಿರಾಂಕ ರಚನೆ opcode_ಕೋಷ್ಟಕ_s*>(r);
      ಹಿಂತಿರುಗಿ strcmp(kp, rp->mnemonic);
}

/*
 * Keep a symbol table of addresses within code space. Labels on
 * executable opcodes are mapped to their address here.
 */
ಸ್ಥಿರ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_t sym_codespace = 0;

/*
 * Keep a symbol table of functors mentioned in the source. This table
 * is used to resolve references as they come.
 */
ಸ್ಥಿರ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_t sym_functors = 0;

/*
 * VPI objects are indexed during compile time so that they can be
 * linked together as they are created. This symbol table matches
 * labels to vpiHandles.
 */
ಸ್ಥಿರ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_t sym_vpi = 0;


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

ಶೂನ್ಯ define_functor_ಚಿಹ್ನೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label, vvp_net_t*net)
{
      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val;
      val.net = net;
      sym_ಗಣ_ಮೌಲ್ಯ(sym_functors, label, val);
}

ಸ್ಥಿರ vvp_net_t*lookup_functor_ಚಿಹ್ನೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label)
{
      ಪ್ರತಿಜ್ಞಾ(sym_functors);
      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val = sym_get_ಮೌಲ್ಯ(sym_functors, label);
      ಹಿಂತಿರುಗಿ val.net;
}

vpiHandle vvp_lookup_handle(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label)
{
      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val = sym_get_ಮೌಲ್ಯ(sym_vpi, label);
      ಆದರೆ (val.ptr) ಹಿಂತಿರುಗಿ (vpiHandle) val.ptr;
      ಹಿಂತಿರುಗಿ 0;
}

vvp_net_t* vvp_net_lookup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label)
{
      ಸ್ಥಿರ ತಾರ್ಕಿಕ t0_trigger_generated = ಸುಳ್ಳು;

        /* First, look to see if the symbol is a vpi object of some
	   sort. If it is, then get the vvp_ipoint_t pointer out of
	   the vpiHandle. */
      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val = sym_get_ಮೌಲ್ಯ(sym_vpi, label);
      ಆದರೆ (val.ptr) {
	    vpiHandle vpi = static_cast<vpiHandle>(val.ptr);
	    ಆಯ್ಕೆ (vpi->get_type_ಸಂಕೇತ()) {
		ಸ್ಥಿತಿ vpiNet:
		ಸ್ಥಿತಿ vpiReg:
		ಸ್ಥಿತಿ vpi_ಬಿಟ್_Var:
		ಸ್ಥಿತಿ vpiByteVar:
		ಸ್ಥಿತಿ vpiShortIntVar:
		ಸ್ಥಿತಿ vpiIntVar:
		ಸ್ಥಿತಿ vpiLongIntVar:
		ಸ್ಥಿತಿ vpiIntegerVar: {
		      ಸ್ಥಿರಾಂಕ __vpiSignal*sig = dynamic_cast<__vpiSignal*>(vpi);
		      ಹಿಂತಿರುಗಿ sig->ಗಂಟು;
		}

		ಸ್ಥಿತಿ vpiRealVar: {
		      ಸ್ಥಿರಾಂಕ __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(vpi);
		      ಹಿಂತಿರುಗಿ sig->net;
		}

		ಸ್ಥಿತಿ vpiStringVar:
		ಸ್ಥಿತಿ vpiArrayVar:
		ಸ್ಥಿತಿ vpiClassVar: {
		      ಸ್ಥಿರಾಂಕ __vpiBaseVar*sig = dynamic_cast<__vpiBaseVar*>(vpi);
		      ಹಿಂತಿರುಗಿ sig->get_net();
		}

		ಸ್ಥಿತಿ vpiNamedEvent: {
		      ಸ್ಥಿರಾಂಕ __vpiNamedEvent*tmp = dynamic_cast<__vpiNamedEvent*>(vpi);
		      ಹಿಂತಿರುಗಿ tmp->funct;
		}

		ಪೂರ್ವನಿಯೋಜಿತ:
		  fprintf(stderr, "Unsupported type %d.\n",
		          vpi->get_type_ಸಂಕೇತ());
		  ಪ್ರತಿಜ್ಞಾ(0);
	    }
      }


	/* Failing that, look for a general functor. */
      vvp_net_t*tmp = lookup_functor_ಚಿಹ್ನೆ(label);

	// This is a special label used to create a T0 trigger for the
	// always_comb/latch processes.
     ಆದರೆ (! t0_trigger_generated && (strcmp(label, "E_0x0") == 0)) {
	      // This should never happen, but if it does then the E_0x0
	      // event generation may need to be explicitly generated in
	      // the compiler output instead of implicitly in this code.
	    ಪ್ರತಿಜ್ಞಾ(! vpip_peek_current_scope()->is_automatic());
	    t0_trigger_generated = ನಿಜ;
	      // Create a local event with no name for the T0 trigger
	    compile_named_event(strdup(label), strcpy(ಹೊಸ ಅಕ್ಷರ [1],""), ನಿಜ);
	    tmp = vvp_net_lookup(label);
	    ಪ್ರತಿಜ್ಞಾ(tmp);
	      // Create a trigger for the T0 event.
	    vvp_net_ptr_t ptr (tmp, 0);
	    ವೇಳಾಪಟ್ಟಿ_t0_trigger(ptr);
     }

      ಹಿಂತಿರುಗಿ tmp;
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
ಸ್ಥಿರ resolv_list_s*resolv_list = 0;

resolv_list_s::~resolv_list_s()
{
      ಬಿಡುಗಡೆ(label_);
}

ಶೂನ್ಯ resolv_submit(resolv_list_s*cur)
{
      ಆದರೆ (cur->resolve()) {
	    ಅಳಿಸು cur;
	    ಹಿಂತಿರುಗಿ;
      }

      cur->ಮುಂದಿನ_2 = resolv_list;
      resolv_list = cur;
}


/*
 * Look up vvp_nets in the symbol table. The "source" is the label for
 * the net that I want to feed, and net->port[port] is the vvp_net
 * input that I want that node to feed into. When the name is found,
 * put net->port[port] into the fan-out list for that node.
 */
ರಚನೆ vvp_net_resolv_list_s: ಸಾರ್ವಜನಿಕ resolv_list_s {

      ಸ್ಪಷ್ಟ vvp_net_resolv_list_s(ಅಕ್ಷರ*l) : resolv_list_s(l) { }
	// port to be driven by the located node.
      vvp_net_ptr_t ಪೋರ್ಟ್;
      ವಾಸ್ತವಿಕ ತಾರ್ಕಿಕ resolve(ತಾರ್ಕಿಕ mes) override;
};

ತಾರ್ಕಿಕ vvp_net_resolv_list_s::resolve(ತಾರ್ಕಿಕ mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      ಆದರೆ (tmp) {
	      // Link the input port to the located output.
	    tmp->link(ಪೋರ್ಟ್);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಆದರೆ (mes)
	    fprintf(stderr, "unresolved vvp_net reference: %s\n", label());

      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರ
ಶೂನ್ಯ postpone_functor_ಒಳಹರಿವು(vvp_net_ptr_t ಪೋರ್ಟ್, ಅಕ್ಷರ*lab)
{
      ರಚನೆ vvp_net_resolv_list_s*res = ಹೊಸ ರಚನೆ vvp_net_resolv_list_s(lab);
      res->ಪೋರ್ಟ್   = ಪೋರ್ಟ್;

      resolv_submit(res);
}


/*
 *  Generic functor reference lookup.
 */

ರಚನೆ functor_gen_resolv_list_s: ಸಾರ್ವಜನಿಕ resolv_list_s {
      ಸ್ಪಷ್ಟ functor_gen_resolv_list_s(ಅಕ್ಷರ*txt) : resolv_list_s(txt) {
	    ref = 0;
      }
      vvp_net_t**ref;
      ವಾಸ್ತವಿಕ ತಾರ್ಕಿಕ resolve(ತಾರ್ಕಿಕ mes) override;
};

ತಾರ್ಕಿಕ functor_gen_resolv_list_s::resolve(ತಾರ್ಕಿಕ mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      ಆದರೆ (tmp) {
	    *ref = tmp;
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಆದರೆ (mes)
	    fprintf(stderr, "unresolved functor reference: %s\n", label());

      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ functor_ref_lookup(vvp_net_t**ref, ಅಕ್ಷರ*lab)
{
      ರಚನೆ functor_gen_resolv_list_s*res =
	    ಹೊಸ ರಚನೆ functor_gen_resolv_list_s(lab);

      res->ref    = ref;

      resolv_submit(res);
}

/*
 *  vpiHandle lookup
 */

ರಚನೆ vpi_handle_resolv_list_s: ಸಾರ್ವಜನಿಕ resolv_list_s {
      ಸ್ಪಷ್ಟ vpi_handle_resolv_list_s(ಅಕ್ಷರ*lab) : resolv_list_s(lab) {
	    handle = ಖಾಲಿ;
      }
      ವಾಸ್ತವಿಕ ತಾರ್ಕಿಕ resolve(ತಾರ್ಕಿಕ mes) override;
      vpiHandle *handle;
};

ತಾರ್ಕಿಕ vpi_handle_resolv_list_s::resolve(ತಾರ್ಕಿಕ mes)
{
      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val = sym_get_ಮೌಲ್ಯ(sym_vpi, label());
      ಆದರೆ (!val.ptr) {
	    // check for thread access symbols
	    ಚಿಹ್ನೆರಹಿತ ತಳ, wid;
	    size_t n = 0;
	    ಅಕ್ಷರ ss[32];
	    ಆದರೆ (2 == ಅಕ್ಷರಸರಣಿ_ಓದು(label(), "W<%u,%31[r]>%zn", &ತಳ, ss, &n)
		       && n == ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(label())) {

		  val.ptr = vpip_make_vthr_ಶಬ್ದ(ತಳ, ss);
		  sym_ಗಣ_ಮೌಲ್ಯ(sym_vpi, label(), val);

	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (1 == ಅಕ್ಷರಸರಣಿ_ಓದು(label(), "S<%u,str>%zn", &ತಳ, &n)
		       && n == ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(label())) {

		  val.ptr = vpip_make_vthr_str_ರಾಶಿ(ತಳ);
		  sym_ಗಣ_ಮೌಲ್ಯ(sym_vpi, label(), val);

	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (3 == ಅಕ್ಷರಸರಣಿ_ಓದು(label(), "S<%u,vec4,%31[su]%u>%zn", &ತಳ, ss, &wid, &n)
		       && n == ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(label())) {

		  ತಾರ್ಕಿಕ signed_flag = ಸುಳ್ಳು;
		  ಗಾಗಿ (ಅಕ್ಷರ*fp = ss ;  *fp ;  fp += 1) ಆಯ್ಕೆ (*fp) {
		      ಸ್ಥಿತಿ 's':
			signed_flag = ನಿಜ;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ 'u':
			signed_flag = ಸುಳ್ಳು;
			ನಿಲ್ಲಿಸು;
		      ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		  }
		  val.ptr = vpip_make_vthr_vec4_ರಾಶಿ(ತಳ, signed_flag, wid);
		  sym_ಗಣ_ಮೌಲ್ಯ(sym_vpi, label(), val);
	    }

      }

      ಆದರೆ (!val.ptr) {
	    // check for memory word  M<mem,base,wid>
      }

      ಆದರೆ (strcmp(label(), "null") == 0) {
	    val.ptr = vpip_make_null_const();
	    sym_ಗಣ_ಮೌಲ್ಯ(sym_vpi, label(), val);
      }

      ಆದರೆ (val.ptr) {
	    *handle = static_cast<vpiHandle>(val.ptr);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಆದರೆ (mes)
	    fprintf(stderr, "unresolved vpi name lookup: %s\n", label());

      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ compile_vpi_lookup(vpiHandle *handle, ಅಕ್ಷರ*label)
{
      ಆದರೆ (strcmp(label, "$time") == 0) {
	    *handle = vpip_sim_ಸಮಯ(vpip_peek_current_scope(), ಸುಳ್ಳು);
	    ಬಿಡುಗಡೆ(label);
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (strcmp(label, "$stime") == 0) {
	    *handle = vpip_sim_ಸಮಯ(vpip_peek_current_scope(), ನಿಜ);
	    ಬಿಡುಗಡೆ(label);
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (strcmp(label, "$realtime") == 0) {
	    *handle = vpip_sim_realtime(vpip_peek_current_scope());
	    ಬಿಡುಗಡೆ(label);
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (strcmp(label, "$simtime") == 0) {
	    *handle = vpip_sim_ಸಮಯ(0, ಸುಳ್ಳು);
	    ಬಿಡುಗಡೆ(label);
	    ಹಿಂತಿರುಗಿ;
      }

      ರಚನೆ vpi_handle_resolv_list_s*res
	    = ಹೊಸ ರಚನೆ vpi_handle_resolv_list_s(label);

      res->handle = handle;
      resolv_submit(res);
}

/*
 * Code Label lookup
 */

ರಚನೆ ಸಂಕೇತ_label_resolv_list_s: ಸಾರ್ವಜನಿಕ resolv_list_s {
      ಸ್ಪಷ್ಟ ಸಂಕೇತ_label_resolv_list_s(ಅಕ್ಷರ*lab, ತಾರ್ಕಿಕ cptr2) : resolv_list_s(lab) {
	    code = ಖಾಲಿ;
	    cptr2_flag = cptr2;
      }
      ರಚನೆ vvp_ಸಂಕೇತ_s *code;
      ತಾರ್ಕಿಕ cptr2_flag;
      ವಾಸ್ತವಿಕ ತಾರ್ಕಿಕ resolve(ತಾರ್ಕಿಕ mes) override;
};

ತಾರ್ಕಿಕ ಸಂಕೇತ_label_resolv_list_s::resolve(ತಾರ್ಕಿಕ mes)
{
      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val = sym_get_ಮೌಲ್ಯ(sym_codespace, label());
      ಆದರೆ (val.ptr) {
	    ಆದರೆ (cptr2_flag)
		  code->cptr2 = reinterpret_cast<vvp_ಸಂಕೇತ_t>(val.ptr);
	    ಇಲ್ಲದಿದ್ದರೆ
		  code->cptr = reinterpret_cast<vvp_ಸಂಕೇತ_t>(val.ptr);
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಆದರೆ (mes)
	    fprintf(stderr, "unresolved code label: %s\n", label());

      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ ಸಂಕೇತ_label_lookup(ರಚನೆ vvp_ಸಂಕೇತ_s *code, ಅಕ್ಷರ *label, ತಾರ್ಕಿಕ cptr2)
{
      ರಚನೆ ಸಂಕೇತ_label_resolv_list_s *res
	    = ಹೊಸ ರಚನೆ ಸಂಕೇತ_label_resolv_list_s(label, cptr2);

      res->code  = code;

      resolv_submit(res);
}

ರಚನೆ ಸಂಕೇತ_array_resolv_list_s: ಸಾರ್ವಜನಿಕ resolv_list_s {
      ಸ್ಪಷ್ಟ ಸಂಕೇತ_array_resolv_list_s(ಅಕ್ಷರ*lab) : resolv_list_s(lab) {
	    code = ಖಾಲಿ;
      }
      ರಚನೆ vvp_ಸಂಕೇತ_s *code;
      ವಾಸ್ತವಿಕ ತಾರ್ಕಿಕ resolve(ತಾರ್ಕಿಕ mes) override;
};

ತಾರ್ಕಿಕ ಸಂಕೇತ_array_resolv_list_s::resolve(ತಾರ್ಕಿಕ mes)
{
      code->array = array_ಕಂಡುಹಿಡಿ(label());
      ಆದರೆ (code->array != 0) {
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಆದರೆ (mes)
	    fprintf(stderr, "Array unresolved: %s\n", label());

      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಸ್ಥಿರ ಶೂನ್ಯ compile_array_lookup(ರಚನೆ vvp_ಸಂಕೇತ_s*code, ಅಕ್ಷರ*label)
{
      ರಚನೆ ಸಂಕೇತ_array_resolv_list_s *res
	    = ಹೊಸ ರಚನೆ ಸಂಕೇತ_array_resolv_list_s(label);

      res->code  = code;

      resolv_submit(res);
}

ಸ್ಥಿರ std::list<ರಚನೆ __vpiSysTaskCall*> scheduled_compiletf;

ಶೂನ್ಯ compile_compiletf(ರಚನೆ __vpiSysTaskCall*obj)
{
      ಆದರೆ (obj->defn->info.compiletf == 0)
	    ಹಿಂತಿರುಗಿ;

      scheduled_compiletf.push_back(obj);
}

/*
 * When parsing is otherwise complete, this function is called to do
 * the final stuff. Clean up deferred linking here.
 */

ಶೂನ್ಯ compile_cleanup(ಶೂನ್ಯ)
{
      ಪೂರ್ಣಾಂಕ lnerrs = -1;
      ಪೂರ್ಣಾಂಕ nerrs = 0;
      ಪೂರ್ಣಾಂಕ ಕೊನೆಯ;

      ಆದರೆ (verbose_flag) {
	    fprintf(stderr, " ... Linking\n");
	    fflush(stderr);
      }

      ಮಾಡು {
	    resolv_list_s *res = resolv_list;
	    resolv_list = 0x0;
	    ಕೊನೆಯ = nerrs == lnerrs;
	    lnerrs = nerrs;
	    nerrs = 0;
	    ಆಗ (res) {
		  resolv_list_s *cur = res;
		  res = res->ಮುಂದಿನ_2;
		  ಆದರೆ (cur->resolve(ಕೊನೆಯ))
			ಅಳಿಸು cur;
		  ಇಲ್ಲದಿದ್ದರೆ {
			nerrs++;
			cur->ಮುಂದಿನ_2 = resolv_list;
			resolv_list = cur;
		  }
	    }
	    ಆದರೆ (nerrs && ಕೊನೆಯ)
		  fprintf(stderr,
			  "compile_cleanup: %d unresolved items\n",
			  nerrs);
      } ಆಗ (nerrs && !ಕೊನೆಯ);

      compile_errors += nerrs;

      ಆದರೆ (verbose_flag) {
	    fprintf(stderr, " ... Removing symbol tables\n");
	    fflush(stderr);
      }

	/* After compile is complete, the vpi symbol table is no
	   longer needed. VPI objects are located by following
	   scopes. */
      delete_ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ(sym_vpi);
      sym_vpi = 0;

	/* Don't need the code labels. The instructions have numeric
	   pointers in them, the symbol table is no longer needed. */
      delete_ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ(sym_codespace);
      sym_codespace = 0;

      delete_ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ(sym_functors);
      sym_functors = 0;

      delete_udp_symbols();

      compile_island_cleanup();
      compile_array_cleanup();

      ಆದರೆ (verbose_flag) {
	    fprintf(stderr, " ... Compiletf functions\n");
	    fflush(stderr);
      }

      ಪ್ರತಿಜ್ಞಾ(vpi_ಬಹುಲಕ_flag == VPI_ಬಹುಲಕ_NONE);
      vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_COMPILETF;

      ಆಗ (! scheduled_compiletf.empty()) {
	    ರಚನೆ __vpiSysTaskCall*obj = scheduled_compiletf.front();
	    scheduled_compiletf.pop_front();
	    vpip_cur_task = obj;
	    obj->defn->info.compiletf (obj->defn->info.user_ದತ್ತಾಂಶ);
	    vpip_cur_task = 0;
      }

      vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_NONE;
}

ಶೂನ್ಯ compile_vpi_ಚಿಹ್ನೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label, vpiHandle obj)
{
      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val;
      val.ptr = obj;
      sym_ಗಣ_ಮೌಲ್ಯ(sym_vpi, label, val);
}

/*
 * Initialize the compiler by allocation empty symbol tables and
 * initializing the various address spaces.
 */
ಶೂನ್ಯ compile_init(ಶೂನ್ಯ)
{
      sym_vpi = new_ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ();

      sym_functors = new_ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ();

      sym_codespace = new_ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ();
      codespace_init();
}

ಶೂನ್ಯ compile_ತುಂಬು_vpi_module(ಅಕ್ಷರ*name)
{
      vpip_ತುಂಬು_module(name);
      ಅಳಿಸು[] name;
}

ಶೂನ್ಯ compile_vpi_ಸಮಯ_precision(ದೀರ್ಘ pre)
{
      vpip_ಗಣ_ಸಮಯ_precision(pre);
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
ಸ್ಥಿರ ತಾರ್ಕಿಕ crstring_ಶೀರ್ಷಿಕೆ_test(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str)
{
      ಆದರೆ ((str[0] != 'C') && (str[0] != 'c'))
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      ಆದರೆ ((str[1] != 'r') || (str[2] != '<'))
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ crstring_test(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str)
{
      ಆದರೆ (!crstring_ಶೀರ್ಷಿಕೆ_test(str))
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tp = strchr(str, '>');
      ಆದರೆ (tp == 0) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      ಆದರೆ (tp[1] != 0) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

      ಆದರೆ ((strspn(str+3, "0123456789abcdefmg")+3) != (size_t)(tp - str))
	  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ದ್ವಿಗುಣ crstring_ಗಮ್ಯ_double(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label)
{
      ಪ್ರತಿಜ್ಞಾ(crstring_ಶೀರ್ಷಿಕೆ_test(label));

      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*cp = label+3;
      ಪ್ರತಿಜ್ಞಾ(*cp == 'm');
      cp += 1;
      ಅಕ್ಷರ*ep;
      uint64_t mant = strtoull(cp, &ep, 16);
      cp = ep;
      ಪ್ರತಿಜ್ಞಾ(*cp == 'g');
      cp += 1;
      ಪೂರ್ಣಾಂಕ ಘಾತಾಂಕ = strtoul(cp, 0, 16);

      ದ್ವಿಗುಣ tmp;
      ಆದರೆ (mant == 0 && ಘಾತಾಂಕ == 0x3fff) {
	    tmp = INFINITY;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mant == 0 && ಘಾತಾಂಕ == 0x7fff) {
	    tmp = -INFINITY;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಘಾತಾಂಕ == 0x3fff) {
	    tmp = nan("");
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ದ್ವಿಗುಣ sign = (ಘಾತಾಂಕ & 0x4000)? -1.0 : 1.0;
	    ಘಾತಾಂಕ &= 0x1fff;

	    tmp = sign * ldexp((ದ್ವಿಗುಣ)mant, ಘಾತಾಂಕ - 0x1000);
      }

      ಹಿಂತಿರುಗಿ tmp;
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

ಶೂನ್ಯ ಒಳಹರಿವು_connect(vvp_net_t*fdx, ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್, ಅಕ್ಷರ*label)
{
      vvp_net_ptr_t ifdx = vvp_net_ptr_t(fdx, ಪೋರ್ಟ್);

	/* Is this a vvp_vector4_t constant value? */
      ಆದರೆ (c4string_test(label)) {

	    vvp_vector4_t tmp = c4string_ಗಮ್ಯ_vector4(label);

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
	    ಆದರೆ (label[0] == 'c')
		  ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(ifdx, tmp);
	    ಇಲ್ಲದಿದ್ದರೆ
		  ವೇಳಾಪಟ್ಟಿ_ಗಣ_ಸದಿಶ(ifdx, tmp);

	    ಬಿಡುಗಡೆ(label);
	    ಹಿಂತಿರುಗಿ;
      }

	/* Is this a vvp_vector8_t constant value? */
      ಆದರೆ (c8string_test(label)) {

	    vvp_vector8_t tmp = c8string_ಗಮ್ಯ_vector8(label);
	    ಆದರೆ (label[0] == 'c')
		  ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(ifdx, tmp);
	    ಇಲ್ಲದಿದ್ದರೆ
		  ವೇಳಾಪಟ್ಟಿ_ಗಣ_ಸದಿಶ(ifdx, tmp);

	    ಬಿಡುಗಡೆ(label);
	    ಹಿಂತಿರುಗಿ;
      }

	/* Handle the Cr<> constant driver, which is a real-value
	   driver. */
      ಆದರೆ (crstring_test(label)) {

	    ದ್ವಿಗುಣ tmp = crstring_ಗಮ್ಯ_double(label);

	    ಆದರೆ (label[0] == 'c')
		  ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(ifdx, tmp);
	    ಇಲ್ಲದಿದ್ದರೆ
		  ವೇಳಾಪಟ್ಟಿ_ಗಣ_ಸದಿಶ(ifdx, tmp);
	    ಬಿಡುಗಡೆ(label);
	    ಹಿಂತಿರುಗಿ;
      }

	/* Handle the general case that this is a label for a node in
	   the vvp net. This arranges for the label to be preserved in
	   a linker list, and linked when the symbol table is
	   complete. */
      postpone_functor_ಒಳಹರಿವು(ifdx, label);
}

ಶೂನ್ಯ inputs_connect(vvp_net_t*fdx, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಆದರೆ (argc > 4) {
	    std::cerr << "XXXX argv[0] = " << argv[0].ಪಠ್ಯ << std::ಸಾಲು_ಅಂತ್ಯ;
      }
      ಪ್ರತಿಜ್ಞಾ(argc <= 4);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0;  idx < argc;  idx += 1) {

	    ಒಳಹರಿವು_connect(fdx, idx, argv[idx].ಪಠ್ಯ);
      }
}

ಶೂನ್ಯ wide_inputs_connect(vvp_wide_fun_core*core,
			 ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
	/* Create input functors to receive values from the
	   network. These functors pass the data to the core. */
      ಚಿಹ್ನೆರಹಿತ ಒಳಹರಿವು_functors = (argc+3) / 4;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಒಳಹರಿವು_functors ;  idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ ತಳ = idx*4;
	    ಚಿಹ್ನೆರಹಿತ trans = 4;
	    ಆದರೆ (ತಳ+trans > argc)
		  trans = argc - ತಳ;

	    vvp_wide_fun_t*cur = ಹೊಸ vvp_wide_fun_t(core, ತಳ);
	    vvp_net_t*ptr = ಹೊಸ vvp_net_t;
	    ptr->fun = cur;

	    inputs_connect(ptr, trans, argv+ತಳ);
      }
}

ಮಾದರಿ <ವರ್ಗ T_> ಶೂನ್ಯ make_arith(T_ *arith, ಅಕ್ಷರ*label,
				    ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      vvp_net_t* ptr = ಹೊಸ vvp_net_t;
      ptr->fun = arith;

      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಬಿಡುಗಡೆ(label);

      ಪ್ರತಿಜ್ಞಾ(argc == 2);
      inputs_connect(ptr, argc, argv);

      ಬಿಡುಗಡೆ(argv);
}

ಶೂನ್ಯ compile_arith_cast_int(ಅಕ್ಷರ*label, ದೀರ್ಘ width,
                            ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      vvp_arith_cast_int*arith = ಹೊಸ vvp_arith_cast_int((ಚಿಹ್ನೆರಹಿತ) width);

      vvp_net_t* ptr = ಹೊಸ vvp_net_t;
      ptr->fun = arith;

      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಬಿಡುಗಡೆ(label);

      ಪ್ರತಿಜ್ಞಾ(argc == 1);
      inputs_connect(ptr, argc, argv);
      ಬಿಡುಗಡೆ(argv);
}

ಶೂನ್ಯ compile_arith_cast_vec2(ಅಕ್ಷರ*label, ದೀರ್ಘ width,
			     ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      vvp_arith_cast_vec2*arith = ಹೊಸ vvp_arith_cast_vec2((ಚಿಹ್ನೆರಹಿತ) width);

      vvp_net_t* ptr = ಹೊಸ vvp_net_t;
      ptr->fun = arith;

      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಬಿಡುಗಡೆ(label);

      ಪ್ರತಿಜ್ಞಾ(argc == 1);
      inputs_connect(ptr, argc, argv);
      ಬಿಡುಗಡೆ(argv);
}

ಶೂನ್ಯ compile_arith_cast_real(ಅಕ್ಷರ*label, ತಾರ್ಕಿಕ signed_flag,
                             ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      vvp_arith_cast_real*arith = ಹೊಸ vvp_arith_cast_real(signed_flag);

      vvp_net_t* ptr = ಹೊಸ vvp_net_t;
      ptr->fun = arith;

      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಬಿಡುಗಡೆ(label);

      ಪ್ರತಿಜ್ಞಾ(argc == 1);
      inputs_connect(ptr, argc, argv);
      ಬಿಡುಗಡೆ(argv);
}

ಶೂನ್ಯ compile_arith_abs(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      vvp_arith_abs*arith = ಹೊಸ vvp_arith_abs;

      vvp_net_t* ptr = ಹೊಸ vvp_net_t;
      ptr->fun = arith;

      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಬಿಡುಗಡೆ(label);

      ಪ್ರತಿಜ್ಞಾ(argc == 1);
      inputs_connect(ptr, argc, argv);
      ಬಿಡುಗಡೆ(argv);
}

ಶೂನ್ಯ compile_arith_div(ಅಕ್ಷರ*label, ದೀರ್ಘ wid, ತಾರ್ಕಿಕ signed_flag,
		       ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix = "";
	    ಆದರೆ (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s; .arith/div%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_arith_div(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_arith_div_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s; .arith/divr has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_real_ *arith = ಹೊಸ vvp_arith_div_real;
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_arith_mod(ಅಕ್ಷರ*label, ದೀರ್ಘ wid, ತಾರ್ಕಿಕ signed_flag,
		       ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .arith/mod has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_arith_mod(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_arith_mod_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .arith/mod.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_real_ *arith = ಹೊಸ vvp_arith_mod_real;
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_arith_mult(ಅಕ್ಷರ*label, ದೀರ್ಘ wid,
			ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .arith/mult has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_arith_mult(wid);
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_arith_mult_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .arith/mult.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_real_ *arith = ಹೊಸ vvp_arith_mult_real;
      make_arith(arith, label, argc, argv);
}


ಶೂನ್ಯ compile_arith_pow(ಅಕ್ಷರ*label, ದೀರ್ಘ wid, ತಾರ್ಕಿಕ signed_flag,
		       ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix = "";
	    ಆದರೆ (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s .arith/pow%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_arith_pow(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_arith_pow_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .arith/pow.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_real_ *arith = ಹೊಸ vvp_arith_pow_real;
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_arith_sub(ಅಕ್ಷರ*label, ದೀರ್ಘ wid, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .arith/sub has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_arith_sub(wid);
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_arith_sub_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s; .arith/sub.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_real_ *arith = ಹೊಸ vvp_arith_sub_real;
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_arith_ಮೊತ್ತ(ಅಕ್ಷರ*label, ದೀರ್ಘ wid, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .arith/sum has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_arith_ಮೊತ್ತ(wid);
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_arith_ಮೊತ್ತ_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .arith/sum.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_real_ *arith = ಹೊಸ vvp_arith_ಮೊತ್ತ_real;
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_cmp_eeq(ಅಕ್ಷರ*label, ದೀರ್ಘ wid,
		     ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_cmp_eeq(wid);

      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_cmp_nee(ಅಕ್ಷರ*label, ದೀರ್ಘ wid,
		     ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_cmp_nee(wid);

      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_cmp_eq(ಅಕ್ಷರ*label, ದೀರ್ಘ wid, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_cmp_eq(wid);
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_cmp_eqx(ಅಕ್ಷರ*label, ದೀರ್ಘ wid, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqx has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_cmp_eqx(wid);
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_cmp_eqz(ಅಕ್ಷರ*label, ದೀರ್ಘ wid, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqz has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_cmp_eqz(wid);
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_cmp_eq_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_real_ *arith = ಹೊಸ vvp_cmp_eq_real;
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_cmp_ne(ಅಕ್ಷರ*label, ದೀರ್ಘ wid, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_cmp_ne(wid);
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_cmp_ne_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_real_ *arith = ಹೊಸ vvp_cmp_ne_real;
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_cmp_ge(ಅಕ್ಷರ*label, ದೀರ್ಘ wid, ತಾರ್ಕಿಕ signed_flag,
		    ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_cmp_ge(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_cmp_ge_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_real_ *arith = ಹೊಸ vvp_cmp_ge_real;
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_cmp_gt(ಅಕ್ಷರ*label, ದೀರ್ಘ wid, ತಾರ್ಕಿಕ signed_flag,
		    ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt has wrong number of symbols\n", label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_cmp_gt(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_cmp_gt_r(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_real_ *arith = ಹೊಸ vvp_cmp_gt_real;
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_cmp_weq(ಅಕ್ಷರ*label, ದೀರ್ಘ wid,
		     ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .cmp/weq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_cmp_weq(wid);

      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_cmp_wne(ಅಕ್ಷರ*label, ದೀರ್ಘ wid,
		     ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      ಆದರೆ (argc != 2) {
	    fprintf(stderr, "%s .cmp/wne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_arith_ *arith = ಹೊಸ vvp_cmp_wne(wid);

      make_arith(arith, label, argc, argv);
}


ಶೂನ್ಯ compile_delay(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ width,
                   vvp_delay_t*delay, ರಚನೆ symb_s arg)
{
      vvp_net_t*net = ಹೊಸ vvp_net_t;
      vvp_fun_delay*obj = ಹೊಸ vvp_fun_delay(net, width, *delay);
      net->fun = obj;

      ಅಳಿಸು delay;

      ಒಳಹರಿವು_connect(net, 0, arg.ಪಠ್ಯ);

      define_functor_ಚಿಹ್ನೆ(label, net);
      ಬಿಡುಗಡೆ(label);
}

ಶೂನ್ಯ compile_delay(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ width,
                   ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv, ತಾರ್ಕಿಕ ignore_decay)
{
      vvp_delay_t stub (0, 0, 0);
      ಆದರೆ (ignore_decay) stub.ಗಣ_ignore_decay();
      vvp_net_t*net = ಹೊಸ vvp_net_t;
      vvp_fun_delay*obj = ಹೊಸ vvp_fun_delay(net, width, stub);
      net->fun = obj;

      inputs_connect(net, argc, argv);
      ಬಿಡುಗಡೆ(argv);

      define_functor_ಚಿಹ್ನೆ(label, net);
      ಬಿಡುಗಡೆ(label);
}

/*
 * Extend nodes.
 */
ಶೂನ್ಯ compile_extend_signed(ಅಕ್ಷರ*label, ದೀರ್ಘ wid, ರಚನೆ symb_s arg)
{
      ಪ್ರತಿಜ್ಞಾ(wid >= 0);

      vvp_fun_extend_signed*fun = ಹೊಸ vvp_fun_extend_signed(wid);
      vvp_net_t*ptr = ಹೊಸ vvp_net_t;
      ptr->fun = fun;

      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಬಿಡುಗಡೆ(label);

      ಒಳಹರಿವು_connect(ptr, 0, arg.ಪಠ್ಯ);
}

ರಚನೆ __vpiModPath* compile_modpath(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ width,
                                     ರಚನೆ symb_s drv, ರಚನೆ symb_s dest)
{
      vvp_net_t*net = ಹೊಸ vvp_net_t;
      vvp_fun_modpath*obj = ಹೊಸ vvp_fun_modpath(net, width);
      net->fun = obj;

      ಒಳಹರಿವು_connect(net, 0, drv.ಪಠ್ಯ);

      define_functor_ಚಿಹ್ನೆ(label, net);

      __vpiModPath*modpath = vpip_make_modpath(net);

      compile_vpi_lookup(&modpath->ಮಾರ್ಗ_ಪದ_ಹೊರಹೋಗು.ಅಭಿವ್ಯಕ್ತಿ, dest.ಪಠ್ಯ);

      ಬಿಡುಗಡೆ(label);

      modpath->modpath = obj;
      ಹಿಂತಿರುಗಿ modpath;
}

ಸ್ಥಿರ ರಚನೆ __vpiModPathSrc*make_modpath_src(ರಚನೆ __vpiModPath*ಮಾರ್ಗ,
					       ಅಕ್ಷರ ಅಂಚು_c,
					       ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&src,
					       ರಚನೆ numbv_s&vals,
					       ತಾರ್ಕಿಕ ifnone)
{
      vvp_fun_modpath*dst = ಮಾರ್ಗ->modpath;

      vvp_time64_t use_delay[12];
      ಪ್ರತಿಜ್ಞಾ(vals.cnt == 12);
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < vals.cnt ;  idx += 1) {
	    use_delay[idx] = vals.nvec[idx];
      }

      numbv_clear(&vals);

      vvp_fun_modpath_src*obj = 0;

      ಪೂರ್ಣಾಂಕ vpi_ಅಂಚು = vpi_No_ಅಂಚು;
      ಆದರೆ (ಅಂಚು_c == 0) {
	    obj = ಹೊಸ vvp_fun_modpath_src(use_delay);

      } ಇಲ್ಲದಿದ್ದರೆ {
	    ತಾರ್ಕಿಕ posedge, negedge;
	    ಆಯ್ಕೆ (ಅಂಚು_c) {
		ಸ್ಥಿತಿ '+':
		  vpi_ಅಂಚು = vpiPosedge;
		  posedge = ನಿಜ;
		  negedge = ಸುಳ್ಳು;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '-':
		  vpi_ಅಂಚು = vpiNegedge;
		  posedge = ಸುಳ್ಳು;
		  negedge = ನಿಜ;
		  ನಿಲ್ಲಿಸು;
#ಆದರೆ 0
		ಸ್ಥಿತಿ '*':
		  posedge = ನಿಜ;
		  negedge = ನಿಜ;
		  ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
		ಪೂರ್ವನಿಯೋಜಿತ:
		  posedge = ಸುಳ್ಳು;
		  negedge = ಸುಳ್ಳು;
		  fprintf(stderr, "Unknown edge identifier %c(%d).\n", ಅಂಚು_c,
		          ಅಂಚು_c);
		  ಪ್ರತಿಜ್ಞಾ(0);
	    }
	    obj = ಹೊಸ vvp_fun_modpath_ಅಂಚು(use_delay, posedge, negedge);
      }

      vvp_net_t*net = ಹೊಸ vvp_net_t;
      ರಚನೆ __vpiModPathSrc* srcobj = vpip_make_modpath_src(ಮಾರ್ಗ, net) ;
      vpip_attach_ಗಮ್ಯ_current_scope(srcobj);
      net->fun = obj;

	/* Save the vpiEdge directory into the input path term. */
      srcobj->ಮಾರ್ಗ_ಪದ_ಒಳ.ಅಂಚು = vpi_ಅಂಚು;
      ಒಳಹರಿವು_connect(net, 0, src.ಪಠ್ಯ);
      dst->ಕೂಡಿಸು_modpath_src(obj, ifnone);

      ಹಿಂತಿರುಗಿ srcobj;
}

ಶೂನ್ಯ compile_modpath_src(ರಚನೆ __vpiModPath*dst, ಅಕ್ಷರ ಅಂಚು_c,
			 ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&src,
			 ರಚನೆ numbv_s&vals,
			 ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&condit_src,
			 ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&ಮಾರ್ಗ_ಪದ_ಒಳ)
{
      ರಚನೆ __vpiModPathSrc*obj =
	    make_modpath_src(dst, ಅಂಚು_c, src, vals, ಸುಳ್ಳು);
      ಒಳಹರಿವು_connect(obj->net, 1, condit_src.ಪಠ್ಯ);
      compile_vpi_lookup(&obj->ಮಾರ್ಗ_ಪದ_ಒಳ.ಅಭಿವ್ಯಕ್ತಿ, ಮಾರ್ಗ_ಪದ_ಒಳ.ಪಠ್ಯ);
}

ಶೂನ್ಯ compile_modpath_src(ರಚನೆ __vpiModPath*dst, ಅಕ್ಷರ ಅಂಚು_c,
			 ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&src,
			 ರಚನೆ numbv_s&vals,
			 ಪೂರ್ಣಾಂಕ condit_src,
			 ಸ್ಥಿರಾಂಕ ರಚನೆ symb_s&ಮಾರ್ಗ_ಪದ_ಒಳ,
			 ತಾರ್ಕಿಕ ifnone)
{
      ಪ್ರತಿಜ್ಞಾ(condit_src == 0);
      ರಚನೆ __vpiModPathSrc*obj =
	    make_modpath_src(dst, ಅಂಚು_c, src, vals, ifnone);
      compile_vpi_lookup(&obj->ಮಾರ್ಗ_ಪದ_ಒಳ.ಅಭಿವ್ಯಕ್ತಿ, ಮಾರ್ಗ_ಪದ_ಒಳ.ಪಠ್ಯ);
}

/*
 * A .shift/l statement creates an array of functors for the
 * width. The 0 input is the data vector to be shifted and the 1 input
 * is the amount of the shift. An unconnected shift amount is set to 0.
 */
ಶೂನ್ಯ compile_shiftl(ಅಕ್ಷರ*label, ದೀರ್ಘ wid, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      vvp_arith_ *arith = ಹೊಸ vvp_shiftl(wid);
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_shiftr(ಅಕ್ಷರ*label, ದೀರ್ಘ wid, ತಾರ್ಕಿಕ signed_flag,
		    ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ಪ್ರತಿಜ್ಞಾ( wid > 0 );

      vvp_arith_ *arith = ಹೊಸ vvp_shiftr(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

ಶೂನ್ಯ compile_resolver(ಅಕ್ಷರ*label, ಅಕ್ಷರ*type, ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      vvp_net_t*net = ಹೊಸ vvp_net_t;

      resolv_core*core = 0;

      ಆದರೆ (strcmp(type,"tri") == 0) {
	    core = ಹೊಸ resolv_tri(argc, net, vvp_scalar_t(BIT4_Z, 0,0));

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"tri0") == 0) {
	    core = ಹೊಸ resolv_tri(argc, net, vvp_scalar_t(BIT4_0, 5,5));

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"tri1") == 0) {
	    core = ಹೊಸ resolv_tri(argc, net, vvp_scalar_t(BIT4_1, 5,5));

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"triand") == 0) {
	    core = ಹೊಸ resolv_triand(argc, net);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"trior") == 0) {
	    core = ಹೊಸ resolv_trior(argc, net);

      } ಇಲ್ಲದಿದ್ದರೆ {
	    fprintf(stderr, "invalid resolver type: %s\n", type);
	    compile_errors += 1;
            ಅಳಿಸು net;
      }

      ಆದರೆ (core) {
	    net->fun = core;
	    define_functor_ಚಿಹ್ನೆ(label, net);

            ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ ತಳ = 0 ;  ತಳ < argc ;  ತಳ += 4) {
	          ಚಿಹ್ನೆರಹಿತ nports = argc - ತಳ;
                  ಆದರೆ (nports > 4)
                        nports = 4;

                  ಆದರೆ (ತಳ > 0) {
                        net = ಹೊಸ vvp_net_t;
                        net->fun = ಹೊಸ resolv_extend(core, ತಳ);
                  }
                  inputs_connect(net, nports, argv+ತಳ);
            }
      }
      ಬಿಡುಗಡೆ(type);
      ಬಿಡುಗಡೆ(label);
      ಬಿಡುಗಡೆ(argv);
}

ಶೂನ್ಯ compile_udp_def(ಪೂರ್ಣಾಂಕ sequ, ಅಕ್ಷರ *label, ಅಕ್ಷರ *name,
		     ಚಿಹ್ನೆರಹಿತ nin, ಚಿಹ್ನೆರಹಿತ init, ಅಕ್ಷರ **ಕೋಷ್ಟಕ)
{
      ಆದರೆ (sequ) {
	    vvp_bit4_t init4;
	    ಆದರೆ (init == 0)
		  init4 = BIT4_0;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (init == 1)
		  init4 = BIT4_1;
	    ಇಲ್ಲದಿದ್ದರೆ
		  init4 = BIT4_X;

	    vvp_udp_seq_s *u = ಹೊಸ vvp_udp_seq_s(label, name, nin, init4);
	    u->compile_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_udp_comb_s *u = ಹೊಸ vvp_udp_comb_s(label, name, nin);
	    u->compile_ಕೋಷ್ಟಕ(ಕೋಷ್ಟಕ);
      }
      ಬಿಡುಗಡೆ(label);
}

ಅಕ್ಷರ **compile_udp_ಕೋಷ್ಟಕ(ಅಕ್ಷರ **ಕೋಷ್ಟಕ, ಅಕ್ಷರ *ಅಡ್ಡಸಾಲು)
{
  ಆದರೆ (ಕೋಷ್ಟಕ)
    ಪ್ರತಿಜ್ಞಾ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*ಕೋಷ್ಟಕ)==ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಅಡ್ಡಸಾಲು));

  ಅಕ್ಷರ **tt;
  ಗಾಗಿ (tt = ಕೋಷ್ಟಕ; tt && *tt; tt++) { }
  ಪೂರ್ಣಾಂಕ n = (tt-ಕೋಷ್ಟಕ) + 2;

  ಕೋಷ್ಟಕ = static_cast<ಅಕ್ಷರ**>(realloc(ಕೋಷ್ಟಕ, n*ಗಾತ್ರ(ಅಕ್ಷರ*)));
  ಕೋಷ್ಟಕ[n-2] = ಅಡ್ಡಸಾಲು;
  ಕೋಷ್ಟಕ[n-1] = 0x0;

  ಹಿಂತಿರುಗಿ ಕೋಷ್ಟಕ;
}


/*
 * The parser uses this function to compile and link an executable
 * opcode. I do this by looking up the opcode in the opcode_table. The
 * table gives the operand structure that is acceptable, so I can
 * process the operands here as well.
 */
ಶೂನ್ಯ compile_ಸಂಕೇತ(ಅಕ್ಷರ*label, ಅಕ್ಷರ*mnem, comp_operands_t opa)
{
	/* First, I can give the label a value that is the current
	   codespace pointer. Don't need the text of the label after
	   this is done. */
      ಆದರೆ (label)
	    compile_codelabel(label);

	/* Lookup the opcode in the opcode table. */
      ರಚನೆ opcode_ಕೋಷ್ಟಕ_s*op = static_cast<ರಚನೆ opcode_ಕೋಷ್ಟಕ_s*>
                                            (bsearch(mnem, opcode_ಕೋಷ್ಟಕ, opcode_ಎಣಿಕೆ,
                                                     ಗಾತ್ರ(ರಚನೆ opcode_ಕೋಷ್ಟಕ_s),
                                                     &opcode_compare));
      ಆದರೆ (op == 0) {
	    yyerror("Invalid opcode");
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

      ಪ್ರತಿಜ್ಞಾ(op);

	/* Build up the code from the information about the opcode and
	   the information from the compiler. */
      vvp_ಸಂಕೇತ_t code = codespace_allocate();
      code->opcode = op->opcode;

      ಆದರೆ (op->argc != (opa? opa->argc : 0)) {
	    yyerror("operand count");
	    compile_errors += 1;
	    ಹಿಂತಿರುಗಿ;
      }

	/* Pull the operands that the instruction expects from the
	   list that the parser supplied. */

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < op->argc ;  idx += 1) {

	    ಆಯ್ಕೆ (op->argt[idx]) {
		ಸ್ಥಿತಿ OA_NONE:
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ OA_ARR_PTR:
		  ಆದರೆ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			ನಿಲ್ಲಿಸು;
		  }

		  compile_array_lookup(code, opa->argv[idx].symb.ಪಠ್ಯ);
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ OA_BIT1:
		  ಆದರೆ (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			ನಿಲ್ಲಿಸು;
		  }
		  code->ಬಿಟ್_idx[0] = opa->argv[idx].numb;
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ OA_BIT2:
		  ಆದರೆ (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			ನಿಲ್ಲಿಸು;
		  }
		  code->ಬಿಟ್_idx[1] = opa->argv[idx].numb;
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ OA_ಸಂಕೇತ_PTR:
		ಸ್ಥಿತಿ OA_ಸಂಕೇತ_PTR2:
		  ಆದರೆ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			ನಿಲ್ಲಿಸು;
		  }

		  ಪ್ರತಿಜ್ಞಾ(opa->argv[idx].symb.idx == 0);
		  ಸಂಕೇತ_label_lookup(code, opa->argv[idx].symb.ಪಠ್ಯ,
				    op->argt[idx]==OA_ಸಂಕೇತ_PTR2);
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ OA_FUNC_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  ಆದರೆ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			ನಿಲ್ಲಿಸು;
		  }

		  functor_ref_lookup(&code->net, opa->argv[idx].symb.ಪಠ್ಯ);
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ OA_FUNC_PTR2:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  ಆದರೆ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			ನಿಲ್ಲಿಸು;
		  }

		  functor_ref_lookup(&code->net2, opa->argv[idx].symb.ಪಠ್ಯ);
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ OA_ಸಂಖ್ಯೆ:
		  ಆದರೆ (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			ನಿಲ್ಲಿಸು;
		  }

		  code->ಸಂಖ್ಯೆ = opa->argv[idx].numb;
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ OA_VPI_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  ಆದರೆ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			ನಿಲ್ಲಿಸು;
		  }

		  compile_vpi_lookup(&code->handle, opa->argv[idx].symb.ಪಠ್ಯ);
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ OA_STRING:
		  ಆದರೆ (opa->argv[idx].ltype != L_STRING) {
			yyerror("operand format");
			ನಿಲ್ಲಿಸು;
		  }

		  code->ಪಠ್ಯ = opa->argv[idx].ಪಠ್ಯ;
		  ನಿಲ್ಲಿಸು;
	    }
      }

      ಬಿಡುಗಡೆ(opa);

      ಬಿಡುಗಡೆ(mnem);
}

ಶೂನ್ಯ compile_codelabel(ಅಕ್ಷರ*label)
{
      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val;
      vvp_ಸಂಕೇತ_t ptr = codespace_ಮುಂದಿನ();

      val.ptr = ptr;
      sym_ಗಣ_ಮೌಲ್ಯ(sym_codespace, label, val);

      ಬಿಡುಗಡೆ(label);
}

ಶೂನ್ಯ compile_file_ಸಾಲು(ಅಕ್ಷರ*label, ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno,
                       ಅಕ್ಷರ*description)
{
      ಆದರೆ (label) compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_ಸಂಕೇತ_t code = codespace_allocate();
      code->opcode = &ದ_FILE_ಸಾಲು;

	/* Create a vpiHandle that contains the information. */
      code->handle = vpip_build_file_ಸಾಲು(description, file_idx, lineno);
      ಪ್ರತಿಜ್ಞಾ(code->handle);

	/* Done with the lexor-allocated name string. */
      ಅಳಿಸು[] description;
}

ಶೂನ್ಯ compile_vpi_call(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,
                      ತಾರ್ಕಿಕ func_as_task_err, ತಾರ್ಕಿಕ func_as_task_warn,
                      ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno,
                      ಚಿಹ್ನೆರಹಿತ argc, vpiHandle*argv,
		      ಚಿಹ್ನೆರಹಿತ vec4_ರಾಶಿ, ಚಿಹ್ನೆರಹಿತ real_ರಾಶಿ, ಚಿಹ್ನೆರಹಿತ string_ರಾಶಿ)
{
      ಆದರೆ (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_ಸಂಕೇತ_t code = codespace_allocate();
      code->opcode = &ದ_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, 0, 0,
					 0, func_as_task_err, func_as_task_warn,
                                         argc, argv, vec4_ರಾಶಿ, real_ರಾಶಿ, string_ರಾಶಿ,
					 file_idx, lineno);
      ಆದರೆ (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      ಅಳಿಸು[] name;
}

ಶೂನ್ಯ compile_vpi_func_call(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name,
			   ಪೂರ್ಣಾಂಕ val_type, ಚಿಹ್ನೆರಹಿತ val_wid,
			   ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno,
			   ಚಿಹ್ನೆರಹಿತ argc, vpiHandle*argv,
			   ಚಿಹ್ನೆರಹಿತ vec4_ರಾಶಿ,
			   ಚಿಹ್ನೆರಹಿತ real_ರಾಶಿ,
			   ಚಿಹ್ನೆರಹಿತ string_ರಾಶಿ)
{
      ಆದರೆ (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_ಸಂಕೇತ_t code = codespace_allocate();
      code->opcode = &ದ_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, val_type, val_wid,
					 0, ನಿಜ, ಸುಳ್ಳು,
                                         argc, argv, vec4_ರಾಶಿ, real_ರಾಶಿ, string_ರಾಶಿ,
					 file_idx, lineno);
      ಆದರೆ (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      ಅಳಿಸು[] name;
}

/*
 * When the parser finds a thread statement, I create a new thread
 * with the start address referenced by the program symbol passed to
 * me.
 */
ಶೂನ್ಯ compile_thread(ಅಕ್ಷರ*ಆರಂಭ_sym, ಅಕ್ಷರ*flag)
{
      ತಾರ್ಕಿಕ ತಳ್ಳು_flag = ಸುಳ್ಳು;

      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t tmp = sym_get_ಮೌಲ್ಯ(sym_codespace, ಆರಂಭ_sym);
      vvp_ಸಂಕೇತ_t pc = reinterpret_cast<vvp_ಸಂಕೇತ_t>(tmp.ptr);
      ಆದರೆ (pc == 0) {
	    yyerror("unresolved address");
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (flag && (strcmp(flag,"$push") == 0))
	    ತಳ್ಳು_flag = ನಿಜ;

      vthread_t thr = vthread_new(pc, vpip_peek_current_scope());

      ಆದರೆ (flag && (strcmp(flag,"$init") == 0))
	    ವೇಳಾಪಟ್ಟಿ_init_vthread(thr);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flag && (strcmp(flag,"$final") == 0))
	    ವೇಳಾಪಟ್ಟಿ_final_vthread(thr);
      ಇಲ್ಲದಿದ್ದರೆ
	    ವೇಳಾಪಟ್ಟಿ_vthread(thr, 0, ತಳ್ಳು_flag);

      ಬಿಡುಗಡೆ(ಆರಂಭ_sym);
      ಬಿಡುಗಡೆ(flag);
}

ಶೂನ್ಯ compile_param_logic(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಅಕ್ಷರ*value, ತಾರ್ಕಿಕ signed_flag,
                         ತಾರ್ಕಿಕ local_flag,
                         ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno)
{
      vvp_vector4_t value4 = c4string_ಗಮ್ಯ_vector4(value);
      vpiHandle obj = vpip_make_ದ್ವಿಮಾನ_param(name, value4, signed_flag,
                                             local_flag, file_idx, lineno);
      compile_vpi_ಚಿಹ್ನೆ(label, obj);
      vpip_attach_ಗಮ್ಯ_current_scope(obj);

      ಬಿಡುಗಡೆ(label);
      ಬಿಡುಗಡೆ(value);
}

ಶೂನ್ಯ compile_param_string(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಅಕ್ಷರ*value,
                          ತಾರ್ಕಿಕ local_flag,
                          ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno)
{
	// name and value become owned by vpip_make_string_param
      vpiHandle obj = vpip_make_string_param(name, value, local_flag, file_idx, lineno);
      compile_vpi_ಚಿಹ್ನೆ(label, obj);
      vpip_attach_ಗಮ್ಯ_current_scope(obj);

      ಬಿಡುಗಡೆ(label);
}

ಶೂನ್ಯ compile_param_real(ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಅಕ್ಷರ*value,
                        ತಾರ್ಕಿಕ local_flag,
                        ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno)
{
      ದ್ವಿಗುಣ dvalue = crstring_ಗಮ್ಯ_double(value);
      vpiHandle obj = vpip_make_real_param(name, dvalue, local_flag, file_idx, lineno);
      compile_vpi_ಚಿಹ್ನೆ(label, obj);
      vpip_attach_ಗಮ್ಯ_current_scope(obj);

      ಬಿಡುಗಡೆ(label);
      ಬಿಡುಗಡೆ(value);
}

ಶೂನ್ಯ compile_island(ಅಕ್ಷರ*label, ಅಕ್ಷರ*type)
{
      ಆದರೆ (strcmp(type,"tran") == 0)
	    compile_island_tran(label);
      ಇಲ್ಲದಿದ್ದರೆ
	    ಪ್ರತಿಜ್ಞಾ(0);

      ಬಿಡುಗಡೆ(type);
}
