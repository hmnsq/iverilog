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

#यदि_परिभाषितम् __MINGW32__
#अन्तर्भाव <windows.h>
#यदि_अन्तः

# include  "ivl_alloc.h"

अचिह्नित compile_errors = 0;

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

गणना सङ्कार्यम्_e {
	/* Place holder for unused operand */
      OA_NONE,
	/* The operand is a number, an immediate unsigned integer */
      OA_सङ्ख्या,
	/* The operand is a pointer to an array. */
      OA_ARR_PTR,
	/* The operand is a thread bit index or short integer */
      OA_BIT1,
      OA_BIT2,
	/* The operand is a pointer to code space */
      OA_संहिता_PTR,
      OA_संहिता_PTR2,
	/* The operand is a variable or net pointer */
      OA_FUNC_PTR,
	/* The operand is a second functor pointer */
      OA_FUNC_PTR2,
	/* The operand is a VPI handle */
      OA_VPI_PTR,
	/* String */
      OA_STRING
};

संरचना opcode_सारणी_s {
      ध्रुव वर्ण*mnemonic;
      vvp_संहिता_fun opcode;

      अचिह्नित argc;
      गणना सङ्कार्यम्_e argt[सङ्कार्यम्_MAX];
};

स्थिर ध्रुव संरचना opcode_सारणी_s opcode_सारणी[] = {
      { "%abs/wr", सम्बन्धि_ABS_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add",    सम्बन्धि_सङ्कलय,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add/wr", सम्बन्धि_सङ्कलय_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%addi",   सम्बन्धि_ADDI,   3,  {OA_BIT1,     OA_BIT2,     OA_सङ्ख्या} },
      { "%alloc",  सम्बन्धि_ALLOC,  1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%and",    सम्बन्धि_AND,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%and/r",  सम्बन्धि_ANDR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%assign/ar",सम्बन्धि_नियोजय_AR,2,{OA_ARR_PTR,OA_BIT1,     OA_NONE} },
      { "%assign/ar/d",सम्बन्धि_नियोजय_ARD,2,{OA_ARR_PTR,OA_BIT1,  OA_NONE} },
      { "%assign/ar/e",सम्बन्धि_नियोजय_ARE,1,{OA_ARR_PTR,OA_NONE,  OA_NONE} },
      { "%assign/vec4",      सम्बन्धि_नियोजय_VEC4,       2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/a/d",  सम्बन्धि_नियोजय_VEC4_A_D,   3, {OA_ARR_PTR,  OA_BIT1, OA_BIT2} },
      { "%assign/vec4/a/e",  सम्बन्धि_नियोजय_VEC4_A_E,   2, {OA_ARR_PTR,  OA_BIT1, OA_NONE} },
      { "%assign/vec4/d",    सम्बन्धि_नियोजय_VEC4_D,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/e",    सम्बन्धि_नियोजय_VEC4_E,      1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%assign/vec4/off/d",सम्बन्धि_नियोजय_VEC4_OFF_D, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%assign/vec4/off/e",सम्बन्धि_नियोजय_VEC4_OFF_E, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr",  सम्बन्धि_नियोजय_WR, 2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/d",सम्बन्धि_नियोजय_WRD,2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/e",सम्बन्धि_नियोजय_WRE,1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%blend",     सम्बन्धि_BLEND,     0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/str", सम्बन्धि_BLEND_STR, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/wr",  सम्बन्धि_BLEND_WR,  0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%breakpoint", सम्बन्धि_BREAKPOINT, 0,  {OA_NONE, OA_NONE, OA_NONE} },
      { "%callf/obj",       सम्बन्धि_CALLF_OBJ,       2,{OA_संहिता_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/real",      सम्बन्धि_CALLF_REAL,      2,{OA_संहिता_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/str",       सम्बन्धि_CALLF_STR,       2,{OA_संहिता_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/vec4",      सम्बन्धि_CALLF_VEC4,      2,{OA_संहिता_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/void",      सम्बन्धि_CALLF_VOID,      2,{OA_संहिता_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%cassign/link",    सम्बन्धि_CASSIGN_LINK,    2,{OA_FUNC_PTR,OA_FUNC_PTR2,OA_NONE} },
      { "%cassign/vec4",    सम्बन्धि_CASSIGN_VEC4,    1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cassign/vec4/off",सम्बन्धि_CASSIGN_VEC4_OFF,2,{OA_FUNC_PTR,OA_BIT1,     OA_NONE} },
      { "%cassign/wr",  सम्बन्धि_CASSIGN_WR,  1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cast/vec2/dar", सम्बन्धि_CAST_VEC2_DAR, 1,  {OA_सङ्ख्या,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/dar", सम्बन्धि_CAST_VEC4_DAR, 1,  {OA_सङ्ख्या,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/str", सम्बन्धि_CAST_VEC4_STR, 1,  {OA_सङ्ख्या,   OA_NONE,     OA_NONE} },
      { "%cast2",   सम्बन्धि_CAST2,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/e",   सम्बन्धि_CMPE,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/ne",  सम्बन्धि_CMPNE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/s",   सम्बन्धि_CMPS,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/str", सम्बन्धि_CMPSTR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/u",   सम्बन्धि_CMPU,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/we",  सम्बन्धि_CMPWE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wne", सम्बन्धि_CMPWNE, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wr",  सम्बन्धि_CMPWR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/x",   सम्बन्धि_CMPX,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/z",   सम्बन्धि_CMPZ,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmpi/e",  सम्बन्धि_CMPIE,  3,  {OA_BIT1,     OA_BIT2,     OA_सङ्ख्या} },
      { "%cmpi/ne", सम्बन्धि_CMPINE, 3,  {OA_BIT1,     OA_BIT2,     OA_सङ्ख्या} },
      { "%cmpi/s",  सम्बन्धि_CMPIS,  3,  {OA_BIT1,     OA_BIT2,     OA_सङ्ख्या} },
      { "%cmpi/u",  सम्बन्धि_CMPIU,  3,  {OA_BIT1,     OA_BIT2,     OA_सङ्ख्या} },
      { "%cmpix/ltu", सम्बन्धि_CMPIX_LTU, 2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%cmpix/slt0", सम्बन्धि_CMPIX_SLT0, 1, {OA_सङ्ख्या, OA_NONE, OA_NONE} },
      { "%concat/str",  सम्बन्धि_CONCAT_STR,  0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concat/vec4", सम्बन्धि_CONCAT_VEC4, 0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concati/str", सम्बन्धि_CONCATI_STR, 1,{OA_STRING,OA_NONE,  OA_NONE} },
      { "%concati/vec4",सम्बन्धि_CONCATI_VEC4,3,{OA_BIT1,  OA_BIT2,  OA_सङ्ख्या} },
      { "%cvt/rv",   सम्बन्धि_CVT_RV,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/rv/s", सम्बन्धि_CVT_RV_S,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/sr", सम्बन्धि_CVT_SR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/ur", सम्बन्धि_CVT_UR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/vr", सम्बन्धि_CVT_VR, 1,  {OA_सङ्ख्या,   OA_NONE,     OA_NONE} },
      { "%deassign",सम्बन्धि_DEASSIGN,3,{OA_FUNC_PTR, OA_BIT1,     OA_BIT2} },
      { "%deassign/wr",सम्बन्धि_DEASSIGN_WR,1,{OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%debug/thr",  सम्बन्धि_DEBUG_THR,  1,{OA_STRING,   OA_NONE,     OA_NONE} },
      { "%delay",  सम्बन्धि_DELAY,  2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%delayx", सम्बन्धि_DELAYX, 1,  {OA_सङ्ख्या,   OA_NONE,     OA_NONE} },
      { "%delete/elem",सम्बन्धि_DELETE_ELEM,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%delete/obj",सम्बन्धि_DELETE_OBJ,1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%delete/prop/elem", सम्बन्धि_DELETE_PROP_ELEM, 1, {OA_सङ्ख्या, OA_NONE, OA_NONE} },
      { "%delete/prop/obj",  सम्बन्धि_DELETE_PROP_OBJ,  1, {OA_सङ्ख्या, OA_NONE, OA_NONE} },
      { "%delete/tail",सम्बन्धि_DELETE_पुच्छ,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%disable",  सम्बन्धि_DISABLE, 1, {OA_VPI_PTR,OA_NONE,     OA_NONE} },
      { "%disable/flow", सम्बन्धि_DISABLE_FLOW, 1, {OA_VPI_PTR,OA_NONE, OA_NONE} },
      { "%disable/fork",सम्बन्धि_DISABLE_FORK,0,{OA_NONE,OA_NONE,  OA_NONE} },
      { "%div",      सम्बन्धि_DIV,     0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/s",    सम्बन्धि_DIV_S,   0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/wr",   सम्बन्धि_DIV_WR,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/obj",  सम्बन्धि_DUP_OBJ, 0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/real", सम्बन्धि_DUP_REAL,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/vec4", सम्बन्धि_DUP_VEC4,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%end",    सम्बन्धि_अन्तः,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl",  सम्बन्धि_EVCTL,  2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/c",सम्बन्धि_EVCTLC, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl/i",सम्बन्धि_EVCTLI, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/s",सम्बन्धि_EVCTLS, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%event",    सम्बन्धि_EVENT,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%event/nb", सम्बन्धि_EVENT_NB, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%flag_get/vec4", सम्बन्धि_FLAG_GET_VEC4, 1, {OA_सङ्ख्या, OA_NONE, OA_NONE} },
      { "%flag_inv",      सम्बन्धि_FLAG_INV,      1, {OA_BIT1,   OA_NONE, OA_NONE} },
      { "%flag_mov",      सम्बन्धि_FLAG_MOV,      2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_or",       सम्बन्धि_FLAG_OR,       2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_set/imm",  सम्बन्धि_FLAG_समुच्चयः_IMM,  2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%flag_set/vec4", सम्बन्धि_FLAG_समुच्चयः_VEC4, 1, {OA_सङ्ख्या, OA_NONE, OA_NONE} },
      { "%force/link",    सम्बन्धि_FORCE_LINK,2,{OA_FUNC_PTR, OA_FUNC_PTR2, OA_NONE} },
      { "%force/vec4",    सम्बन्धि_FORCE_VEC4,    1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%force/vec4/off",सम्बन्धि_FORCE_VEC4_OFF,2,{OA_FUNC_PTR, OA_BIT1,      OA_NONE} },
      { "%force/vec4/off/d",सम्बन्धि_FORCE_VEC4_OFF_D,3,{OA_FUNC_PTR, OA_BIT1,  OA_BIT2} },
      { "%force/wr",      सम्बन्धि_FORCE_WR,      1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%fork",   सम्बन्धि_FORK,   2,  {OA_संहिता_PTR2,OA_VPI_PTR,  OA_NONE} },
      { "%free",   सम्बन्धि_FREE,   1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%inv",    सम्बन्धि_INV,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%ix/add", सम्बन्धि_IX_सङ्कलय, 3,  {OA_सङ्ख्या,   OA_BIT1,     OA_BIT2} },
      { "%ix/getv",सम्बन्धि_IX_GETV,2,  {OA_BIT1,     OA_FUNC_PTR, OA_NONE} },
      { "%ix/getv/s",सम्बन्धि_IX_GETV_S,2, {OA_BIT1,   OA_FUNC_PTR, OA_NONE} },
      { "%ix/load",सम्बन्धि_IX_आरोपय,3,  {OA_सङ्ख्या,   OA_BIT1,     OA_BIT2} },
      { "%ix/mov", सम्बन्धि_IX_MOV, 2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%ix/mul", सम्बन्धि_IX_MUL, 3,  {OA_सङ्ख्या,   OA_BIT1,     OA_BIT2} },
      { "%ix/sub", सम्बन्धि_IX_SUB, 3,  {OA_सङ्ख्या,   OA_BIT1,     OA_BIT2} },
      { "%ix/vec4",  सम्बन्धि_IX_VEC4,  1,  {OA_सङ्ख्या,   OA_NONE, OA_NONE} },
      { "%ix/vec4/s",सम्बन्धि_IX_VEC4_S,1,  {OA_सङ्ख्या,   OA_NONE, OA_NONE} },
      { "%jmp",    सम्बन्धि_JMP,    1,  {OA_संहिता_PTR, OA_NONE,     OA_NONE} },
      { "%jmp/0",  सम्बन्धि_JMP0,   2,  {OA_संहिता_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/0xz",सम्बन्धि_JMP0_XZ, 2,  {OA_संहिता_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1",  सम्बन्धि_JMP1,   2,  {OA_संहिता_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1xz",सम्बन्धि_JMP1_XZ, 2,  {OA_संहिता_PTR, OA_BIT1,     OA_NONE} },
      { "%join",   सम्बन्धि_JOIN,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%join/detach",सम्बन्धि_JOIN_DETACH,1,{OA_सङ्ख्या,OA_NONE,  OA_NONE} },
      { "%load/ar",सम्बन्धि_आरोपय_AR,2,  {OA_ARR_PTR,  OA_BIT1,     OA_NONE} },
      { "%load/dar/r",  सम्बन्धि_आरोपय_DAR_R,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE}},
      { "%load/dar/str",सम्बन्धि_आरोपय_DAR_STR,  1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/dar/vec4",सम्बन्धि_आरोपय_DAR_VEC4,1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/obj",   सम्बन्धि_आरोपय_OBJ,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/obja",  सम्बन्धि_आरोपय_OBJA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/prop/dar/vec4", सम्बन्धि_आरोपय_PROP_DAR_VEC4, 2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%load/real",  सम्बन्धि_आरोपय_REAL, 1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%load/str",   सम्बन्धि_आरोपय_STR,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/stra",  सम्बन्धि_आरोपय_STRA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/vec4",  सम्बन्धि_आरोपय_VEC4, 1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%load/vec4a", सम्बन्धि_आरोपय_VEC4_A,2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%max/wr", सम्बन्धि_MAX_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%min/wr", सम्बन्धि_MIN_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod",    सम्बन्धि_MOD,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/s",  सम्बन्धि_MOD_S,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/wr", सम्बन्धि_MOD_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul",    सम्बन्धि_MUL,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul/wr", सम्बन्धि_MUL_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%muli",   सम्बन्धि_MULI,   3,  {OA_BIT1,     OA_BIT2,     OA_सङ्ख्या} },
      { "%nand",   सम्बन्धि_NAND,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nand/r", सम्बन्धि_NANDR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%neg/wr", सम्बन्धि_NEG_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%new/cobj",  सम्बन्धि_NEW_COBJ,  1, {OA_VPI_PTR,OA_NONE,  OA_NONE} },
      { "%new/darray",सम्बन्धि_NEW_DARRAY,2, {OA_BIT1,   OA_STRING,OA_NONE} },
      { "%noop",   सम्बन्धि_NOOP,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor",    सम्बन्धि_NOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor/r",  सम्बन्धि_NORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%null",   सम्बन्धि_NULL,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or",     सम्बन्धि_OR,     0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or/r",   सम्बन्धि_ORR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%pad/s",  सम्बन्धि_PAD_S,  1,  {OA_सङ्ख्या,   OA_NONE,     OA_NONE} },
      { "%pad/u",  सम्बन्धि_PAD_U,  1,  {OA_सङ्ख्या,   OA_NONE,     OA_NONE} },
      { "%part/s", सम्बन्धि_PART_S, 1,  {OA_सङ्ख्या,   OA_NONE,     OA_NONE} },
      { "%part/u", सम्बन्धि_PART_U, 1,  {OA_सङ्ख्या,   OA_NONE,     OA_NONE} },
      { "%parti/s",सम्बन्धि_PARTI_S,3,  {OA_सङ्ख्या,   OA_BIT1,     OA_BIT2} },
      { "%parti/u",सम्बन्धि_PARTI_U,3,  {OA_सङ्ख्या,   OA_BIT1,     OA_BIT2} },
      { "%pop/obj", सम्बन्धि_निष्कासय_OBJ, 2, {OA_BIT1,    OA_BIT2,     OA_NONE} },
      { "%pop/real",सम्बन्धि_निष्कासय_REAL,1, {OA_सङ्ख्या,  OA_NONE,     OA_NONE} },
      { "%pop/str", सम्बन्धि_निष्कासय_STR, 1, {OA_सङ्ख्या,  OA_NONE,     OA_NONE} },
      { "%pop/vec4",सम्बन्धि_निष्कासय_VEC4,1, {OA_सङ्ख्या,  OA_NONE,     OA_NONE} },
      { "%pow",     सम्बन्धि_POW,     0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/s",   सम्बन्धि_POW_S,   0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/wr",  सम्बन्धि_POW_WR,  0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%prop/obj",सम्बन्धि_PROP_OBJ,2,  {OA_सङ्ख्या,   OA_BIT1,     OA_NONE} },
      { "%prop/queue/size", सम्बन्धि_PROP_QUEUE_आकार,  1, {OA_सङ्ख्या, OA_NONE, OA_NONE} },
      { "%prop/r",  सम्बन्धि_PROP_R,  1,  {OA_सङ्ख्या,   OA_NONE,     OA_NONE} },
      { "%prop/str",सम्बन्धि_PROP_STR,1,  {OA_सङ्ख्या,   OA_NONE,     OA_NONE} },
      { "%prop/v",  सम्बन्धि_PROP_V,  1,  {OA_सङ्ख्या,   OA_NONE,     OA_NONE} },
      { "%push/ix/vec4", सम्बन्धि_प्रक्षिप_IX_VEC4, 3, {OA_सङ्ख्या, OA_BIT1, OA_BIT2} },
      { "%pushi/real",सम्बन्धि_PUSHI_REAL,2,{OA_BIT1,   OA_BIT2,   OA_NONE} },
      { "%pushi/str", सम्बन्धि_PUSHI_STR, 1,{OA_STRING, OA_NONE,   OA_NONE} },
      { "%pushi/vec4",सम्बन्धि_PUSHI_VEC4,3,{OA_BIT1,   OA_BIT2,   OA_सङ्ख्या} },
      { "%pushv/str", सम्बन्धि_PUSHV_STR, 0,{OA_NONE,   OA_NONE,   OA_NONE} },
      { "%putc/str/vec4",सम्बन्धि_PUTC_STR_VEC4,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/prop/r",   सम्बन्धि_QINSERT_PROP_R,   2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/str", सम्बन्धि_QINSERT_PROP_STR, 2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/v",   सम्बन्धि_QINSERT_PROP_V,   3, {OA_सङ्ख्या, OA_BIT1, OA_BIT2} },
      { "%qinsert/real",सम्बन्धि_QINSERT_REAL,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/str", सम्बन्धि_QINSERT_STR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/v",   सम्बन्धि_QINSERT_V,   3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%qpop/b/real",सम्बन्धि_QPOP_B_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/str", सम्बन्धि_QPOP_B_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/v",   सम्बन्धि_QPOP_B_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/f/real",सम्बन्धि_QPOP_F_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/str", सम्बन्धि_QPOP_F_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/v",   सम्बन्धि_QPOP_F_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/prop/b/r",   सम्बन्धि_QPOP_PROP_B_REAL, 1, {OA_सङ्ख्या, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/str", सम्बन्धि_QPOP_PROP_B_STR, 1, {OA_सङ्ख्या, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/v",   सम्बन्धि_QPOP_PROP_B_V,    2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%qpop/prop/f/r",   सम्बन्धि_QPOP_PROP_F_REAL, 1, {OA_सङ्ख्या, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/str", सम्बन्धि_QPOP_PROP_F_STR, 1, {OA_सङ्ख्या, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/v",   सम्बन्धि_QPOP_PROP_F_V,    2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%queue/append_word/v", सम्बन्धि_QUEUE_APPEND_WORD_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/find/index/prop/v", सम्बन्धि_QUEUE_FIND_सूचकांक_PROP_V, 2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%queue/find/index/v", सम्बन्धि_QUEUE_FIND_सूचकांक_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find/prop/v", सम्बन्धि_QUEUE_FIND_PROP_V, 2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%queue/find/v", सम्बन्धि_QUEUE_FIND_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/prop/v", सम्बन्धि_QUEUE_FIND_FIRST_सूचकांक_PROP_V, 2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/v", सम्बन्धि_QUEUE_FIND_FIRST_सूचकांक_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/prop/v", सम्बन्धि_QUEUE_FIND_FIRST_PROP_V, 2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%queue/find_first/v", सम्बन्धि_QUEUE_FIND_FIRST_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/prop/v", सम्बन्धि_QUEUE_FIND_चरमः_सूचकांक_PROP_V, 2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/v", सम्बन्धि_QUEUE_FIND_चरमः_सूचकांक_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/prop/v", सम्बन्धि_QUEUE_FIND_चरमः_PROP_V, 2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%queue/find_last/v", सम्बन्धि_QUEUE_FIND_चरमः_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/max/obj/v", सम्बन्धि_QUEUE_MAX_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/max/prop/v", सम्बन्धि_QUEUE_MAX_PROP_V, 2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%queue/max/v", सम्बन्धि_QUEUE_MAX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/min/obj/v", सम्बन्धि_QUEUE_MIN_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/min/prop/v", सम्बन्धि_QUEUE_MIN_PROP_V, 2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%queue/min/v", सम्बन्धि_QUEUE_MIN_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/new_empty/v", सम्बन्धि_QUEUE_NEW_रिक्तानि_V, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%queue/size/v", सम्बन्धि_QUEUE_आकार_V, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%queue/unique/index/obj/v", सम्बन्धि_QUEUE_UNIQUE_सूचकांक_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/index/prop/v", सम्बन्धि_QUEUE_UNIQUE_सूचकांक_PROP_V, 2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%queue/unique/index/v", सम्बन्धि_QUEUE_UNIQUE_सूचकांक_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/unique/obj/v", सम्बन्धि_QUEUE_UNIQUE_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/prop/v", सम्बन्धि_QUEUE_UNIQUE_PROP_V, 2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%queue/unique/v", सम्बन्धि_QUEUE_UNIQUE_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/word/prop/v", सम्बन्धि_QUEUE_WORD_PROP_V, 3, {OA_सङ्ख्या, OA_BIT1, OA_BIT2} },
      { "%queue/word/v", सम्बन्धि_QUEUE_WORD_V, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%release/net",सम्बन्धि_RELEASE_NET,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/reg",सम्बन्धि_RELEASE_REG,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/wr", सम्बन्धि_RELEASE_WR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%replicate", सम्बन्धि_REPLICATE,   1,{OA_सङ्ख्या,  OA_NONE,OA_NONE} },
      { "%ret/real",  सम्बन्धि_RET_REAL,    1,{OA_सङ्ख्या,  OA_NONE,OA_NONE} },
      { "%ret/str",   सम्बन्धि_RET_STR,     1,{OA_सङ्ख्या,  OA_NONE,OA_NONE} },
      { "%ret/vec4",  सम्बन्धि_RET_VEC4,    3,{OA_सङ्ख्या,  OA_BIT1,OA_BIT2} },
      { "%retload/real",सम्बन्धि_RETLOAD_REAL,1,{OA_सङ्ख्या,  OA_NONE,OA_NONE} },
      { "%retload/str", सम्बन्धि_RETLOAD_STR, 1,{OA_सङ्ख्या,  OA_NONE,OA_NONE} },
      { "%retload/vec4",सम्बन्धि_RETLOAD_VEC4,1,{OA_सङ्ख्या,  OA_NONE,OA_NONE} },
      { "%reverse/obj", सम्बन्धि_REVERSE_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%reverse/prop/obj", सम्बन्धि_REVERSE_PROP_OBJ, 1, {OA_सङ्ख्या, OA_NONE, OA_NONE} },
      { "%rsort/obj", सम्बन्धि_RSORT_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%rsort/prop/obj", सम्बन्धि_RSORT_PROP_OBJ, 1, {OA_सङ्ख्या, OA_NONE, OA_NONE} },
      { "%scopy",  सम्बन्धि_SCOPY,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%set/dar/obj/real",सम्बन्धि_समुच्चयः_DAR_OBJ_REAL,1,{OA_सङ्ख्या,OA_NONE,OA_NONE} },
      { "%set/dar/obj/str", सम्बन्धि_समुच्चयः_DAR_OBJ_STR, 1,{OA_सङ्ख्या,OA_NONE,OA_NONE} },
      { "%set/dar/obj/vec4",सम्बन्धि_समुच्चयः_DAR_OBJ_VEC4,1,{OA_सङ्ख्या,OA_NONE,OA_NONE} },
      { "%shiftl",   सम्बन्धि_SHIFTL,   1, {OA_सङ्ख्या, OA_NONE,   OA_NONE} },
      { "%shiftr",   सम्बन्धि_SHIFTR,   1, {OA_सङ्ख्या, OA_NONE,   OA_NONE} },
      { "%shiftr/s", सम्बन्धि_SHIFTR_S, 1, {OA_सङ्ख्या, OA_NONE,   OA_NONE} },
      { "%shuffle/obj", सम्बन्धि_SHUFFLE_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%shuffle/prop/obj", सम्बन्धि_SHUFFLE_PROP_OBJ, 1, {OA_सङ्ख्या, OA_NONE, OA_NONE} },
      { "%sort/obj", सम्बन्धि_क्रमय_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%sort/prop/obj", सम्बन्धि_क्रमय_PROP_OBJ, 1, {OA_सङ्ख्या, OA_NONE, OA_NONE} },
      { "%split/vec4",    सम्बन्धि_विभज_VEC4,    1,{OA_सङ्ख्या,   OA_NONE, OA_NONE} },
      { "%store/dar/r",   सम्बन्धि_STORE_DAR_R,   1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/str", सम्बन्धि_STORE_DAR_STR, 1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/vec4",सम्बन्धि_STORE_DAR_VEC4,1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/obj",   सम्बन्धि_STORE_OBJ,   1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/obja",  सम्बन्धि_STORE_OBJA,  2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/prop/obj",सम्बन्धि_STORE_PROP_OBJ,2, {OA_सङ्ख्या,  OA_BIT1, OA_NONE} },
      { "%store/prop/qb/r",   सम्बन्धि_STORE_PROP_QB_R,   2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/str", सम्बन्धि_STORE_PROP_QB_STR, 2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/v",   सम्बन्धि_STORE_PROP_QB_V,   3, {OA_सङ्ख्या, OA_BIT1, OA_BIT2} },
      { "%store/prop/qf/r",   सम्बन्धि_STORE_PROP_QF_R,   2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/str", सम्बन्धि_STORE_PROP_QF_STR, 2, {OA_सङ्ख्या, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/v",   सम्बन्धि_STORE_PROP_QF_V,   3, {OA_सङ्ख्या, OA_BIT1, OA_BIT2} },
      { "%store/prop/r",  सम्बन्धि_STORE_PROP_R,  1, {OA_सङ्ख्या,  OA_NONE, OA_NONE} },
      { "%store/prop/str",सम्बन्धि_STORE_PROP_STR,1, {OA_सङ्ख्या,  OA_NONE, OA_NONE} },
      { "%store/prop/v",  सम्बन्धि_STORE_PROP_V,  2, {OA_सङ्ख्या,  OA_BIT1, OA_NONE} },
      { "%store/qb/r",   सम्बन्धि_STORE_QB_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/str", सम्बन्धि_STORE_QB_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/v",   सम्बन्धि_STORE_QB_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qdar/r",  सम्बन्धि_STORE_QDAR_R,  2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/str",सम्बन्धि_STORE_QDAR_STR,2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/v",  सम्बन्धि_STORE_QDAR_V,  3,{OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qf/r",   सम्बन्धि_STORE_QF_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/str", सम्बन्धि_STORE_QF_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/v",   सम्बन्धि_STORE_QF_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qobj/r",  सम्बन्धि_STORE_QOBJ_R,  2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/str",सम्बन्धि_STORE_QOBJ_STR,2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/v",  सम्बन्धि_STORE_QOBJ_V,  3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/real",    सम्बन्धि_STORE_REAL,    1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/reala",   सम्बन्धि_STORE_REALA,   2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/str",     सम्बन्धि_STORE_STR,     1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/stra",    सम्बन्धि_STORE_STRA,    2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/vec4",    सम्बन्धि_STORE_VEC4,    3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/vec4a",   सम्बन्धि_STORE_VEC4_A,   3, {OA_ARR_PTR, OA_BIT1, OA_BIT2} },
      { "%sub",    सम्बन्धि_SUB,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%sub/wr", सम्बन्धि_SUB_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%subi",   सम्बन्धि_SUBI,   3,  {OA_BIT1,     OA_BIT2,     OA_सङ्ख्या} },
      { "%substr",     सम्बन्धि_SUBSTR,     2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%substr/vec4",सम्बन्धि_SUBSTR_VEC4,2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%test_nul",     सम्बन्धि_TEST_NUL,     1,{OA_FUNC_PTR,OA_NONE,    OA_NONE} },
      { "%test_nul/a",   सम्बन्धि_TEST_NUL_A,   2,{OA_ARR_PTR, OA_BIT1,    OA_NONE} },
      { "%test_nul/obj", सम्बन्धि_TEST_NUL_OBJ, 0,{OA_NONE,    OA_NONE,    OA_NONE} },
      { "%test_nul/prop",सम्बन्धि_TEST_NUL_PROP,2,{OA_सङ्ख्या,  OA_BIT1,    OA_NONE} },
      { "%uparr/and/v",          सम्बन्धि_UPARR_AND_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/or/v",           सम्बन्धि_UPARR_OR_V,           2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/product/obj/v",  सम्बन्धि_UPARR_गुणनफलम्_OBJ_V,  1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/product/prop/v", सम्बन्धि_UPARR_गुणनफलम्_PROP_V, 2, {OA_सङ्ख्या,   OA_BIT1, OA_NONE} },
      { "%uparr/product/r",      सम्बन्धि_UPARR_गुणनफलम्_REAL,   1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/product/v",      सम्बन्धि_UPARR_गुणनफलम्_V,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/sum/obj/v",      सम्बन्धि_UPARR_योग_OBJ_V,      1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/sum/prop/v",     सम्बन्धि_UPARR_योग_PROP_V,     2, {OA_सङ्ख्या,   OA_BIT1, OA_NONE} },
      { "%uparr/sum/r",          सम्बन्धि_UPARR_योग_REAL,       1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/sum/v",          सम्बन्धि_UPARR_योग_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/xor/v",          सम्बन्धि_UPARR_XOR_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%wait",   सम्बन्धि_WAIT,   1,  {OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%wait/fork",सम्बन्धि_WAIT_FORK,0,{OA_NONE,   OA_NONE,     OA_NONE} },
      { "%xnor",   सम्बन्धि_XNOR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xnor/r", सम्बन्धि_XNORR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor",    सम्बन्धि_XOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor/r",  सम्बन्धि_XORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { 0, सम्बन्धि_NOOP, 0, {OA_NONE, OA_NONE, OA_NONE} }
};

स्थिर ध्रुव अचिह्नित opcode_गणनम् =
                    परिमाण(opcode_सारणी)/परिमाण(*opcode_सारणी) - 1;

स्थिर पूर्णाङ्क opcode_compare(ध्रुव शून्य*k, ध्रुव शून्य*r)
{
      ध्रुव वर्ण*kp = static_cast<ध्रुव वर्ण*>(k);
      ध्रुव संरचना opcode_सारणी_s*rp = static_cast<ध्रुव संरचना opcode_सारणी_s*>(r);
      प्रत्यागम वाक्यं_तुलय(kp, rp->mnemonic);
}

/*
 * Keep a symbol table of addresses within code space. Labels on
 * executable opcodes are mapped to their address here.
 */
स्थिर चिह्नम्_सारणी_t sym_codespace = 0;

/*
 * Keep a symbol table of functors mentioned in the source. This table
 * is used to resolve references as they come.
 */
स्थिर चिह्नम्_सारणी_t sym_functors = 0;

/*
 * VPI objects are indexed during compile time so that they can be
 * linked together as they are created. This symbol table matches
 * labels to vpiHandles.
 */
स्थिर चिह्नम्_सारणी_t sym_vpi = 0;


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

शून्य define_functor_चिह्नम्(ध्रुव वर्ण*label, vvp_net_t*net)
{
      चिह्नम्_मानम्_t val;
      val.net = net;
      sym_समुच्चयः_मानम्(sym_functors, label, val);
}

स्थिर vvp_net_t*lookup_functor_चिह्नम्(ध्रुव वर्ण*label)
{
      प्रतिज्ञा(sym_functors);
      चिह्नम्_मानम्_t val = sym_get_मानम्(sym_functors, label);
      प्रत्यागम val.net;
}

vpiHandle vvp_lookup_handle(ध्रुव वर्ण*label)
{
      चिह्नम्_मानम्_t val = sym_get_मानम्(sym_vpi, label);
      यदि (val.ptr) प्रत्यागम (vpiHandle) val.ptr;
      प्रत्यागम 0;
}

vvp_net_t* vvp_net_lookup(ध्रुव वर्ण*label)
{
      स्थिर तार्किक t0_trigger_generated = असत्य;

        /* First, look to see if the symbol is a vpi object of some
	   sort. If it is, then get the vvp_ipoint_t pointer out of
	   the vpiHandle. */
      चिह्नम्_मानम्_t val = sym_get_मानम्(sym_vpi, label);
      यदि (val.ptr) {
	    vpiHandle vpi = static_cast<vpiHandle>(val.ptr);
	    विकल्प (vpi->get_type_संहिता()) {
		पक्ष vpiNet:
		पक्ष vpiReg:
		पक्ष vpiBitVar:
		पक्ष vpiByteVar:
		पक्ष vpiShortIntVar:
		पक्ष vpiIntVar:
		पक्ष vpiLongIntVar:
		पक्ष vpiIntegerVar: {
		      ध्रुव __vpiSignal*sig = dynamic_cast<__vpiSignal*>(vpi);
		      प्रत्यागम sig->ग्रन्थिः;
		}

		पक्ष vpiRealVar: {
		      ध्रुव __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(vpi);
		      प्रत्यागम sig->net;
		}

		पक्ष vpiStringVar:
		पक्ष vpiArrayVar:
		पक्ष vpiClassVar: {
		      ध्रुव __vpiBaseVar*sig = dynamic_cast<__vpiBaseVar*>(vpi);
		      प्रत्यागम sig->get_net();
		}

		पक्ष vpiNamedEvent: {
		      ध्रुव __vpiNamedEvent*tmp = dynamic_cast<__vpiNamedEvent*>(vpi);
		      प्रत्यागम tmp->funct;
		}

		स्वभाव:
		  fprintf(stderr, "Unsupported type %d.\n",
		          vpi->get_type_संहिता());
		  प्रतिज्ञा(0);
	    }
      }


	/* Failing that, look for a general functor. */
      vvp_net_t*tmp = lookup_functor_चिह्नम्(label);

	// This is a special label used to create a T0 trigger for the
	// always_comb/latch processes.
     यदि (! t0_trigger_generated && (वाक्यं_तुलय(label, "E_0x0") == 0)) {
	      // This should never happen, but if it does then the E_0x0
	      // event generation may need to be explicitly generated in
	      // the compiler output instead of implicitly in this code.
	    प्रतिज्ञा(! vpip_peek_current_scope()->is_automatic());
	    t0_trigger_generated = सत्य;
	      // Create a local event with no name for the T0 trigger
	    compile_named_event(strdup(label), वाक्यं_प्रतिलिख(नूतन वर्ण [1],""), सत्य);
	    tmp = vvp_net_lookup(label);
	    प्रतिज्ञा(tmp);
	      // Create a trigger for the T0 event.
	    vvp_net_ptr_t ptr (tmp, 0);
	    कालसारणी_t0_trigger(ptr);
     }

      प्रत्यागम tmp;
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
स्थिर resolv_list_s*resolv_list = 0;

resolv_list_s::~resolv_list_s()
{
      मुक्त(label_);
}

शून्य resolv_submit(resolv_list_s*cur)
{
      यदि (cur->resolve()) {
	    विलोपय cur;
	    प्रत्यागम;
      }

      cur->अग्र_2 = resolv_list;
      resolv_list = cur;
}


/*
 * Look up vvp_nets in the symbol table. The "source" is the label for
 * the net that I want to feed, and net->port[port] is the vvp_net
 * input that I want that node to feed into. When the name is found,
 * put net->port[port] into the fan-out list for that node.
 */
संरचना vvp_net_resolv_list_s: सार्वजनिक resolv_list_s {

      स्पष्ट vvp_net_resolv_list_s(वर्ण*l) : resolv_list_s(l) { }
	// port to be driven by the located node.
      vvp_net_ptr_t पत्तनम्;
      आभास तार्किक resolve(तार्किक mes) override;
};

तार्किक vvp_net_resolv_list_s::resolve(तार्किक mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      यदि (tmp) {
	      // Link the input port to the located output.
	    tmp->link(पत्तनम्);
	    प्रत्यागम सत्य;
      }

      यदि (mes)
	    fprintf(stderr, "unresolved vvp_net reference: %s\n", label());

      प्रत्यागम असत्य;
}

अन्तःस्थ स्थिर
शून्य postpone_functor_निवेशः(vvp_net_ptr_t पत्तनम्, वर्ण*lab)
{
      संरचना vvp_net_resolv_list_s*res = नूतन संरचना vvp_net_resolv_list_s(lab);
      res->पत्तनम्   = पत्तनम्;

      resolv_submit(res);
}


/*
 *  Generic functor reference lookup.
 */

संरचना functor_gen_resolv_list_s: सार्वजनिक resolv_list_s {
      स्पष्ट functor_gen_resolv_list_s(वर्ण*txt) : resolv_list_s(txt) {
	    ref = 0;
      }
      vvp_net_t**ref;
      आभास तार्किक resolve(तार्किक mes) override;
};

तार्किक functor_gen_resolv_list_s::resolve(तार्किक mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      यदि (tmp) {
	    *ref = tmp;
	    प्रत्यागम सत्य;
      }

      यदि (mes)
	    fprintf(stderr, "unresolved functor reference: %s\n", label());

      प्रत्यागम असत्य;
}

शून्य functor_ref_lookup(vvp_net_t**ref, वर्ण*lab)
{
      संरचना functor_gen_resolv_list_s*res =
	    नूतन संरचना functor_gen_resolv_list_s(lab);

      res->ref    = ref;

      resolv_submit(res);
}

/*
 *  vpiHandle lookup
 */

संरचना vpi_handle_resolv_list_s: सार्वजनिक resolv_list_s {
      स्पष्ट vpi_handle_resolv_list_s(वर्ण*lab) : resolv_list_s(lab) {
	    handle = रिक्त;
      }
      आभास तार्किक resolve(तार्किक mes) override;
      vpiHandle *handle;
};

तार्किक vpi_handle_resolv_list_s::resolve(तार्किक mes)
{
      चिह्नम्_मानम्_t val = sym_get_मानम्(sym_vpi, label());
      यदि (!val.ptr) {
	    // check for thread access symbols
	    अचिह्नित आधारः, wid;
	    size_t n = 0;
	    वर्ण ss[32];
	    यदि (2 == वाक्यात्_पठ(label(), "W<%u,%31[r]>%zn", &आधारः, ss, &n)
		       && n == वाक्यदैर्घ्यम्(label())) {

		  val.ptr = vpip_make_vthr_word(आधारः, ss);
		  sym_समुच्चयः_मानम्(sym_vpi, label(), val);

	    } अन्यथा यदि (1 == वाक्यात्_पठ(label(), "S<%u,str>%zn", &आधारः, &n)
		       && n == वाक्यदैर्घ्यम्(label())) {

		  val.ptr = vpip_make_vthr_str_राशिः(आधारः);
		  sym_समुच्चयः_मानम्(sym_vpi, label(), val);

	    } अन्यथा यदि (3 == वाक्यात्_पठ(label(), "S<%u,vec4,%31[su]%u>%zn", &आधारः, ss, &wid, &n)
		       && n == वाक्यदैर्घ्यम्(label())) {

		  तार्किक signed_flag = असत्य;
		  प्रति (वर्ण*fp = ss ;  *fp ;  fp += 1) विकल्प (*fp) {
		      पक्ष 's':
			signed_flag = सत्य;
			भङ्ग;
		      पक्ष 'u':
			signed_flag = असत्य;
			भङ्ग;
		      स्वभाव:
			भङ्ग;
		  }
		  val.ptr = vpip_make_vthr_vec4_राशिः(आधारः, signed_flag, wid);
		  sym_समुच्चयः_मानम्(sym_vpi, label(), val);
	    }

      }

      यदि (!val.ptr) {
	    // check for memory word  M<mem,base,wid>
      }

      यदि (वाक्यं_तुलय(label(), "null") == 0) {
	    val.ptr = vpip_make_null_const();
	    sym_समुच्चयः_मानम्(sym_vpi, label(), val);
      }

      यदि (val.ptr) {
	    *handle = static_cast<vpiHandle>(val.ptr);
	    प्रत्यागम सत्य;
      }

      यदि (mes)
	    fprintf(stderr, "unresolved vpi name lookup: %s\n", label());

      प्रत्यागम असत्य;
}

शून्य compile_vpi_lookup(vpiHandle *handle, वर्ण*label)
{
      यदि (वाक्यं_तुलय(label, "$time") == 0) {
	    *handle = vpip_sim_time(vpip_peek_current_scope(), असत्य);
	    मुक्त(label);
	    प्रत्यागम;
      }

      यदि (वाक्यं_तुलय(label, "$stime") == 0) {
	    *handle = vpip_sim_time(vpip_peek_current_scope(), सत्य);
	    मुक्त(label);
	    प्रत्यागम;
      }

      यदि (वाक्यं_तुलय(label, "$realtime") == 0) {
	    *handle = vpip_sim_realtime(vpip_peek_current_scope());
	    मुक्त(label);
	    प्रत्यागम;
      }

      यदि (वाक्यं_तुलय(label, "$simtime") == 0) {
	    *handle = vpip_sim_time(0, असत्य);
	    मुक्त(label);
	    प्रत्यागम;
      }

      संरचना vpi_handle_resolv_list_s*res
	    = नूतन संरचना vpi_handle_resolv_list_s(label);

      res->handle = handle;
      resolv_submit(res);
}

/*
 * Code Label lookup
 */

संरचना संहिता_label_resolv_list_s: सार्वजनिक resolv_list_s {
      स्पष्ट संहिता_label_resolv_list_s(वर्ण*lab, तार्किक cptr2) : resolv_list_s(lab) {
	    code = रिक्त;
	    cptr2_flag = cptr2;
      }
      संरचना vvp_संहिता_s *code;
      तार्किक cptr2_flag;
      आभास तार्किक resolve(तार्किक mes) override;
};

तार्किक संहिता_label_resolv_list_s::resolve(तार्किक mes)
{
      चिह्नम्_मानम्_t val = sym_get_मानम्(sym_codespace, label());
      यदि (val.ptr) {
	    यदि (cptr2_flag)
		  code->cptr2 = reinterpret_cast<vvp_संहिता_t>(val.ptr);
	    अन्यथा
		  code->cptr = reinterpret_cast<vvp_संहिता_t>(val.ptr);
	    प्रत्यागम सत्य;
      }

      यदि (mes)
	    fprintf(stderr, "unresolved code label: %s\n", label());

      प्रत्यागम असत्य;
}

शून्य संहिता_label_lookup(संरचना vvp_संहिता_s *code, वर्ण *label, तार्किक cptr2)
{
      संरचना संहिता_label_resolv_list_s *res
	    = नूतन संरचना संहिता_label_resolv_list_s(label, cptr2);

      res->code  = code;

      resolv_submit(res);
}

संरचना संहिता_array_resolv_list_s: सार्वजनिक resolv_list_s {
      स्पष्ट संहिता_array_resolv_list_s(वर्ण*lab) : resolv_list_s(lab) {
	    code = रिक्त;
      }
      संरचना vvp_संहिता_s *code;
      आभास तार्किक resolve(तार्किक mes) override;
};

तार्किक संहिता_array_resolv_list_s::resolve(तार्किक mes)
{
      code->array = array_find(label());
      यदि (code->array != 0) {
	    प्रत्यागम सत्य;
      }

      यदि (mes)
	    fprintf(stderr, "Array unresolved: %s\n", label());

      प्रत्यागम असत्य;
}

स्थिर शून्य compile_array_lookup(संरचना vvp_संहिता_s*code, वर्ण*label)
{
      संरचना संहिता_array_resolv_list_s *res
	    = नूतन संरचना संहिता_array_resolv_list_s(label);

      res->code  = code;

      resolv_submit(res);
}

स्थिर std::list<संरचना __vpiSysTaskCall*> scheduled_compiletf;

शून्य compile_compiletf(संरचना __vpiSysTaskCall*obj)
{
      यदि (obj->defn->info.compiletf == 0)
	    प्रत्यागम;

      scheduled_compiletf.push_back(obj);
}

/*
 * When parsing is otherwise complete, this function is called to do
 * the final stuff. Clean up deferred linking here.
 */

शून्य compile_cleanup(शून्य)
{
      पूर्णाङ्क lnerrs = -1;
      पूर्णाङ्क nerrs = 0;
      पूर्णाङ्क चरमः;

      यदि (verbose_flag) {
	    fprintf(stderr, " ... Linking\n");
	    fflush(stderr);
      }

      कुरु {
	    resolv_list_s *res = resolv_list;
	    resolv_list = 0x0;
	    चरमः = nerrs == lnerrs;
	    lnerrs = nerrs;
	    nerrs = 0;
	    यावत् (res) {
		  resolv_list_s *cur = res;
		  res = res->अग्र_2;
		  यदि (cur->resolve(चरमः))
			विलोपय cur;
		  अन्यथा {
			nerrs++;
			cur->अग्र_2 = resolv_list;
			resolv_list = cur;
		  }
	    }
	    यदि (nerrs && चरमः)
		  fprintf(stderr,
			  "compile_cleanup: %d unresolved items\n",
			  nerrs);
      } यावत् (nerrs && !चरमः);

      compile_errors += nerrs;

      यदि (verbose_flag) {
	    fprintf(stderr, " ... Removing symbol tables\n");
	    fflush(stderr);
      }

	/* After compile is complete, the vpi symbol table is no
	   longer needed. VPI objects are located by following
	   scopes. */
      delete_चिह्नम्_सारणी(sym_vpi);
      sym_vpi = 0;

	/* Don't need the code labels. The instructions have numeric
	   pointers in them, the symbol table is no longer needed. */
      delete_चिह्नम्_सारणी(sym_codespace);
      sym_codespace = 0;

      delete_चिह्नम्_सारणी(sym_functors);
      sym_functors = 0;

      delete_udp_symbols();

      compile_island_cleanup();
      compile_array_cleanup();

      यदि (verbose_flag) {
	    fprintf(stderr, " ... Compiletf functions\n");
	    fflush(stderr);
      }

      प्रतिज्ञा(vpi_बहुलकम्_flag == VPI_बहुलकम्_NONE);
      vpi_बहुलकम्_flag = VPI_बहुलकम्_COMPILETF;

      यावत् (! scheduled_compiletf.empty()) {
	    संरचना __vpiSysTaskCall*obj = scheduled_compiletf.front();
	    scheduled_compiletf.pop_front();
	    vpip_cur_task = obj;
	    obj->defn->info.compiletf (obj->defn->info.user_दत्त);
	    vpip_cur_task = 0;
      }

      vpi_बहुलकम्_flag = VPI_बहुलकम्_NONE;
}

शून्य compile_vpi_चिह्नम्(ध्रुव वर्ण*label, vpiHandle obj)
{
      चिह्नम्_मानम्_t val;
      val.ptr = obj;
      sym_समुच्चयः_मानम्(sym_vpi, label, val);
}

/*
 * Initialize the compiler by allocation empty symbol tables and
 * initializing the various address spaces.
 */
शून्य compile_init(शून्य)
{
      sym_vpi = new_चिह्नम्_सारणी();

      sym_functors = new_चिह्नम्_सारणी();

      sym_codespace = new_चिह्नम्_सारणी();
      codespace_init();
}

शून्य compile_आरोपय_vpi_module(वर्ण*name)
{
      vpip_आरोपय_module(name);
      विलोपय[] name;
}

शून्य compile_vpi_time_precision(दीर्घ pre)
{
      vpip_समुच्चयः_time_precision(pre);
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
स्थिर तार्किक crstring_शीर्षकम्_test(ध्रुव वर्ण*str)
{
      यदि ((str[0] != 'C') && (str[0] != 'c'))
	    प्रत्यागम असत्य;
      यदि ((str[1] != 'r') || (str[2] != '<'))
	    प्रत्यागम असत्य;

      प्रत्यागम सत्य;
}

तार्किक crstring_test(ध्रुव वर्ण*str)
{
      यदि (!crstring_शीर्षकम्_test(str))
	    प्रत्यागम असत्य;
      ध्रुव वर्ण*tp = strchr(str, '>');
      यदि (tp == 0) प्रत्यागम असत्य;
      यदि (tp[1] != 0) प्रत्यागम असत्य;

      यदि ((strspn(str+3, "0123456789abcdefmg")+3) != (size_t)(tp - str))
	  प्रत्यागम असत्य;

      प्रत्यागम सत्य;
}

द्विगुण crstring_गन्तव्यम्_double(ध्रुव वर्ण*label)
{
      प्रतिज्ञा(crstring_शीर्षकम्_test(label));

      ध्रुव वर्ण*cp = label+3;
      प्रतिज्ञा(*cp == 'm');
      cp += 1;
      वर्ण*ep;
      uint64_t mant = strtoull(cp, &ep, 16);
      cp = ep;
      प्रतिज्ञा(*cp == 'g');
      cp += 1;
      पूर्णाङ्क घातांकः = strtoul(cp, 0, 16);

      द्विगुण tmp;
      यदि (mant == 0 && घातांकः == 0x3fff) {
	    tmp = INFINITY;
      } अन्यथा यदि (mant == 0 && घातांकः == 0x7fff) {
	    tmp = -INFINITY;
      } अन्यथा यदि (घातांकः == 0x3fff) {
	    tmp = nan("");
      } अन्यथा {
	    द्विगुण sign = (घातांकः & 0x4000)? -1.0 : 1.0;
	    घातांकः &= 0x1fff;

	    tmp = sign * ldexp((द्विगुण)mant, घातांकः - 0x1000);
      }

      प्रत्यागम tmp;
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

शून्य निवेशः_connect(vvp_net_t*fdx, अचिह्नित पत्तनम्, वर्ण*label)
{
      vvp_net_ptr_t ifdx = vvp_net_ptr_t(fdx, पत्तनम्);

	/* Is this a vvp_vector4_t constant value? */
      यदि (c4string_test(label)) {

	    vvp_vector4_t tmp = c4string_गन्तव्यम्_vector4(label);

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
	    यदि (label[0] == 'c')
		  कालसारणी_init_सदिशम्(ifdx, tmp);
	    अन्यथा
		  कालसारणी_समुच्चयः_सदिशम्(ifdx, tmp);

	    मुक्त(label);
	    प्रत्यागम;
      }

	/* Is this a vvp_vector8_t constant value? */
      यदि (c8string_test(label)) {

	    vvp_vector8_t tmp = c8string_गन्तव्यम्_vector8(label);
	    यदि (label[0] == 'c')
		  कालसारणी_init_सदिशम्(ifdx, tmp);
	    अन्यथा
		  कालसारणी_समुच्चयः_सदिशम्(ifdx, tmp);

	    मुक्त(label);
	    प्रत्यागम;
      }

	/* Handle the Cr<> constant driver, which is a real-value
	   driver. */
      यदि (crstring_test(label)) {

	    द्विगुण tmp = crstring_गन्तव्यम्_double(label);

	    यदि (label[0] == 'c')
		  कालसारणी_init_सदिशम्(ifdx, tmp);
	    अन्यथा
		  कालसारणी_समुच्चयः_सदिशम्(ifdx, tmp);
	    मुक्त(label);
	    प्रत्यागम;
      }

	/* Handle the general case that this is a label for a node in
	   the vvp net. This arranges for the label to be preserved in
	   a linker list, and linked when the symbol table is
	   complete. */
      postpone_functor_निवेशः(ifdx, label);
}

शून्य inputs_connect(vvp_net_t*fdx, अचिह्नित argc, संरचना symb_s*argv)
{
      यदि (argc > 4) {
	    std::cerr << "XXXX argv[0] = " << argv[0].text << std::पङ्क्त्यन्तः;
      }
      प्रतिज्ञा(argc <= 4);

      प्रति (अचिह्नित idx = 0;  idx < argc;  idx += 1) {

	    निवेशः_connect(fdx, idx, argv[idx].text);
      }
}

शून्य wide_inputs_connect(vvp_wide_fun_core*core,
			 अचिह्नित argc, संरचना symb_s*argv)
{
	/* Create input functors to receive values from the
	   network. These functors pass the data to the core. */
      अचिह्नित निवेशः_functors = (argc+3) / 4;
      प्रति (अचिह्नित idx = 0 ;  idx < निवेशः_functors ;  idx += 1) {
	    अचिह्नित आधारः = idx*4;
	    अचिह्नित trans = 4;
	    यदि (आधारः+trans > argc)
		  trans = argc - आधारः;

	    vvp_wide_fun_t*cur = नूतन vvp_wide_fun_t(core, आधारः);
	    vvp_net_t*ptr = नूतन vvp_net_t;
	    ptr->fun = cur;

	    inputs_connect(ptr, trans, argv+आधारः);
      }
}

प्रतिरूपम् <वर्ग T_> शून्य make_arith(T_ *arith, वर्ण*label,
				    अचिह्नित argc, संरचना symb_s*argv)
{
      vvp_net_t* ptr = नूतन vvp_net_t;
      ptr->fun = arith;

      define_functor_चिह्नम्(label, ptr);
      मुक्त(label);

      प्रतिज्ञा(argc == 2);
      inputs_connect(ptr, argc, argv);

      मुक्त(argv);
}

शून्य compile_arith_cast_int(वर्ण*label, दीर्घ width,
                            अचिह्नित argc, संरचना symb_s*argv)
{
      vvp_arith_cast_int*arith = नूतन vvp_arith_cast_int((अचिह्नित) width);

      vvp_net_t* ptr = नूतन vvp_net_t;
      ptr->fun = arith;

      define_functor_चिह्नम्(label, ptr);
      मुक्त(label);

      प्रतिज्ञा(argc == 1);
      inputs_connect(ptr, argc, argv);
      मुक्त(argv);
}

शून्य compile_arith_cast_vec2(वर्ण*label, दीर्घ width,
			     अचिह्नित argc, संरचना symb_s*argv)
{
      vvp_arith_cast_vec2*arith = नूतन vvp_arith_cast_vec2((अचिह्नित) width);

      vvp_net_t* ptr = नूतन vvp_net_t;
      ptr->fun = arith;

      define_functor_चिह्नम्(label, ptr);
      मुक्त(label);

      प्रतिज्ञा(argc == 1);
      inputs_connect(ptr, argc, argv);
      मुक्त(argv);
}

शून्य compile_arith_cast_real(वर्ण*label, तार्किक signed_flag,
                             अचिह्नित argc, संरचना symb_s*argv)
{
      vvp_arith_cast_real*arith = नूतन vvp_arith_cast_real(signed_flag);

      vvp_net_t* ptr = नूतन vvp_net_t;
      ptr->fun = arith;

      define_functor_चिह्नम्(label, ptr);
      मुक्त(label);

      प्रतिज्ञा(argc == 1);
      inputs_connect(ptr, argc, argv);
      मुक्त(argv);
}

शून्य compile_arith_abs(वर्ण*label, अचिह्नित argc, संरचना symb_s*argv)
{
      vvp_arith_abs*arith = नूतन vvp_arith_abs;

      vvp_net_t* ptr = नूतन vvp_net_t;
      ptr->fun = arith;

      define_functor_चिह्नम्(label, ptr);
      मुक्त(label);

      प्रतिज्ञा(argc == 1);
      inputs_connect(ptr, argc, argv);
      मुक्त(argv);
}

शून्य compile_arith_div(वर्ण*label, दीर्घ wid, तार्किक signed_flag,
		       अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    ध्रुव वर्ण *suffix = "";
	    यदि (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s; .arith/div%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_arith_div(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

शून्य compile_arith_div_r(वर्ण*label, अचिह्नित argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s; .arith/divr has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_arith_div_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_arith_mod(वर्ण*label, दीर्घ wid, तार्किक signed_flag,
		       अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    fprintf(stderr, "%s .arith/mod has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_arith_mod(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

शून्य compile_arith_mod_r(वर्ण*label, अचिह्नित argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .arith/mod.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_arith_mod_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_arith_mult(वर्ण*label, दीर्घ wid,
			अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    fprintf(stderr, "%s .arith/mult has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_arith_mult(wid);
      make_arith(arith, label, argc, argv);
}

शून्य compile_arith_mult_r(वर्ण*label, अचिह्नित argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .arith/mult.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_arith_mult_real;
      make_arith(arith, label, argc, argv);
}


शून्य compile_arith_pow(वर्ण*label, दीर्घ wid, तार्किक signed_flag,
		       अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    ध्रुव वर्ण *suffix = "";
	    यदि (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s .arith/pow%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_arith_pow(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

शून्य compile_arith_pow_r(वर्ण*label, अचिह्नित argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .arith/pow.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_arith_pow_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_arith_sub(वर्ण*label, दीर्घ wid, अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    fprintf(stderr, "%s .arith/sub has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_arith_sub(wid);
      make_arith(arith, label, argc, argv);
}

शून्य compile_arith_sub_r(वर्ण*label, अचिह्नित argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s; .arith/sub.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_arith_sub_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_arith_योग(वर्ण*label, दीर्घ wid, अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    fprintf(stderr, "%s .arith/sum has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_arith_योग(wid);
      make_arith(arith, label, argc, argv);
}

शून्य compile_arith_योग_r(वर्ण*label, अचिह्नित argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .arith/sum.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_arith_योग_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_eeq(वर्ण*label, दीर्घ wid,
		     अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_cmp_eeq(wid);

      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_nee(वर्ण*label, दीर्घ wid,
		     अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_cmp_nee(wid);

      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_eq(वर्ण*label, दीर्घ wid, अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_cmp_eq(wid);
      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_eqx(वर्ण*label, दीर्घ wid, अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqx has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_cmp_eqx(wid);
      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_eqz(वर्ण*label, दीर्घ wid, अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqz has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_cmp_eqz(wid);
      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_eq_r(वर्ण*label, अचिह्नित argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_cmp_eq_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_ne(वर्ण*label, दीर्घ wid, अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_cmp_ne(wid);
      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_ne_r(वर्ण*label, अचिह्नित argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_cmp_ne_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_ge(वर्ण*label, दीर्घ wid, तार्किक signed_flag,
		    अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_cmp_ge(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_ge_r(वर्ण*label, अचिह्नित argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_cmp_ge_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_gt(वर्ण*label, दीर्घ wid, तार्किक signed_flag,
		    अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_cmp_gt(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_gt_r(वर्ण*label, अचिह्नित argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_cmp_gt_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_weq(वर्ण*label, दीर्घ wid,
		     अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/weq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_cmp_weq(wid);

      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_wne(वर्ण*label, दीर्घ wid,
		     अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/wne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_cmp_wne(wid);

      make_arith(arith, label, argc, argv);
}


शून्य compile_delay(वर्ण*label, अचिह्नित width,
                   vvp_delay_t*delay, संरचना symb_s arg)
{
      vvp_net_t*net = नूतन vvp_net_t;
      vvp_fun_delay*obj = नूतन vvp_fun_delay(net, width, *delay);
      net->fun = obj;

      विलोपय delay;

      निवेशः_connect(net, 0, arg.text);

      define_functor_चिह्नम्(label, net);
      मुक्त(label);
}

शून्य compile_delay(वर्ण*label, अचिह्नित width,
                   अचिह्नित argc, संरचना symb_s*argv, तार्किक ignore_decay)
{
      vvp_delay_t stub (0, 0, 0);
      यदि (ignore_decay) stub.समुच्चयः_ignore_decay();
      vvp_net_t*net = नूतन vvp_net_t;
      vvp_fun_delay*obj = नूतन vvp_fun_delay(net, width, stub);
      net->fun = obj;

      inputs_connect(net, argc, argv);
      मुक्त(argv);

      define_functor_चिह्नम्(label, net);
      मुक्त(label);
}

/*
 * Extend nodes.
 */
शून्य compile_extend_signed(वर्ण*label, दीर्घ wid, संरचना symb_s arg)
{
      प्रतिज्ञा(wid >= 0);

      vvp_fun_extend_signed*fun = नूतन vvp_fun_extend_signed(wid);
      vvp_net_t*ptr = नूतन vvp_net_t;
      ptr->fun = fun;

      define_functor_चिह्नम्(label, ptr);
      मुक्त(label);

      निवेशः_connect(ptr, 0, arg.text);
}

संरचना __vpiModPath* compile_modpath(वर्ण*label, अचिह्नित width,
                                     संरचना symb_s drv, संरचना symb_s dest)
{
      vvp_net_t*net = नूतन vvp_net_t;
      vvp_fun_modpath*obj = नूतन vvp_fun_modpath(net, width);
      net->fun = obj;

      निवेशः_connect(net, 0, drv.text);

      define_functor_चिह्नम्(label, net);

      __vpiModPath*modpath = vpip_make_modpath(net);

      compile_vpi_lookup(&modpath->मार्गः_पदांशः_बहिर्गमनम्.व्यञ्जकम्, dest.text);

      मुक्त(label);

      modpath->modpath = obj;
      प्रत्यागम modpath;
}

स्थिर संरचना __vpiModPathSrc*make_modpath_src(संरचना __vpiModPath*मार्गः,
					       वर्ण edge_c,
					       ध्रुव संरचना symb_s&src,
					       संरचना numbv_s&vals,
					       तार्किक ifnone)
{
      vvp_fun_modpath*dst = मार्गः->modpath;

      vvp_time64_t use_delay[12];
      प्रतिज्ञा(vals.cnt == 12);
      प्रति (अचिह्नित idx = 0 ; idx < vals.cnt ;  idx += 1) {
	    use_delay[idx] = vals.nvec[idx];
      }

      numbv_clear(&vals);

      vvp_fun_modpath_src*obj = 0;

      पूर्णाङ्क vpi_edge = vpiNoEdge;
      यदि (edge_c == 0) {
	    obj = नूतन vvp_fun_modpath_src(use_delay);

      } अन्यथा {
	    तार्किक posedge, negedge;
	    विकल्प (edge_c) {
		पक्ष '+':
		  vpi_edge = vpiPosedge;
		  posedge = सत्य;
		  negedge = असत्य;
		  भङ्ग;
		पक्ष '-':
		  vpi_edge = vpiNegedge;
		  posedge = असत्य;
		  negedge = सत्य;
		  भङ्ग;
#यदि 0
		पक्ष '*':
		  posedge = सत्य;
		  negedge = सत्य;
		  भङ्ग;
#यदि_अन्तः
		स्वभाव:
		  posedge = असत्य;
		  negedge = असत्य;
		  fprintf(stderr, "Unknown edge identifier %c(%d).\n", edge_c,
		          edge_c);
		  प्रतिज्ञा(0);
	    }
	    obj = नूतन vvp_fun_modpath_edge(use_delay, posedge, negedge);
      }

      vvp_net_t*net = नूतन vvp_net_t;
      संरचना __vpiModPathSrc* srcobj = vpip_make_modpath_src(मार्गः, net) ;
      vpip_attach_गन्तव्यम्_current_scope(srcobj);
      net->fun = obj;

	/* Save the vpiEdge directory into the input path term. */
      srcobj->मार्गः_पदांशः_अन्तर्.edge = vpi_edge;
      निवेशः_connect(net, 0, src.text);
      dst->सङ्कलय_modpath_src(obj, ifnone);

      प्रत्यागम srcobj;
}

शून्य compile_modpath_src(संरचना __vpiModPath*dst, वर्ण edge_c,
			 ध्रुव संरचना symb_s&src,
			 संरचना numbv_s&vals,
			 ध्रुव संरचना symb_s&condit_src,
			 ध्रुव संरचना symb_s&मार्गः_पदांशः_अन्तर्)
{
      संरचना __vpiModPathSrc*obj =
	    make_modpath_src(dst, edge_c, src, vals, असत्य);
      निवेशः_connect(obj->net, 1, condit_src.text);
      compile_vpi_lookup(&obj->मार्गः_पदांशः_अन्तर्.व्यञ्जकम्, मार्गः_पदांशः_अन्तर्.text);
}

शून्य compile_modpath_src(संरचना __vpiModPath*dst, वर्ण edge_c,
			 ध्रुव संरचना symb_s&src,
			 संरचना numbv_s&vals,
			 पूर्णाङ्क condit_src,
			 ध्रुव संरचना symb_s&मार्गः_पदांशः_अन्तर्,
			 तार्किक ifnone)
{
      प्रतिज्ञा(condit_src == 0);
      संरचना __vpiModPathSrc*obj =
	    make_modpath_src(dst, edge_c, src, vals, ifnone);
      compile_vpi_lookup(&obj->मार्गः_पदांशः_अन्तर्.व्यञ्जकम्, मार्गः_पदांशः_अन्तर्.text);
}

/*
 * A .shift/l statement creates an array of functors for the
 * width. The 0 input is the data vector to be shifted and the 1 input
 * is the amount of the shift. An unconnected shift amount is set to 0.
 */
शून्य compile_shiftl(वर्ण*label, दीर्घ wid, अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      vvp_arith_ *arith = नूतन vvp_shiftl(wid);
      make_arith(arith, label, argc, argv);
}

शून्य compile_shiftr(वर्ण*label, दीर्घ wid, तार्किक signed_flag,
		    अचिह्नित argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      vvp_arith_ *arith = नूतन vvp_shiftr(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

शून्य compile_resolver(वर्ण*label, वर्ण*type, अचिह्नित argc, संरचना symb_s*argv)
{
      vvp_net_t*net = नूतन vvp_net_t;

      resolv_core*core = 0;

      यदि (वाक्यं_तुलय(type,"tri") == 0) {
	    core = नूतन resolv_tri(argc, net, vvp_scalar_t(BIT4_Z, 0,0));

      } अन्यथा यदि (वाक्यं_तुलय(type,"tri0") == 0) {
	    core = नूतन resolv_tri(argc, net, vvp_scalar_t(BIT4_0, 5,5));

      } अन्यथा यदि (वाक्यं_तुलय(type,"tri1") == 0) {
	    core = नूतन resolv_tri(argc, net, vvp_scalar_t(BIT4_1, 5,5));

      } अन्यथा यदि (वाक्यं_तुलय(type,"triand") == 0) {
	    core = नूतन resolv_triand(argc, net);

      } अन्यथा यदि (वाक्यं_तुलय(type,"trior") == 0) {
	    core = नूतन resolv_trior(argc, net);

      } अन्यथा {
	    fprintf(stderr, "invalid resolver type: %s\n", type);
	    compile_errors += 1;
            विलोपय net;
      }

      यदि (core) {
	    net->fun = core;
	    define_functor_चिह्नम्(label, net);

            प्रति (अचिह्नित आधारः = 0 ;  आधारः < argc ;  आधारः += 4) {
	          अचिह्नित nports = argc - आधारः;
                  यदि (nports > 4)
                        nports = 4;

                  यदि (आधारः > 0) {
                        net = नूतन vvp_net_t;
                        net->fun = नूतन resolv_extend(core, आधारः);
                  }
                  inputs_connect(net, nports, argv+आधारः);
            }
      }
      मुक्त(type);
      मुक्त(label);
      मुक्त(argv);
}

शून्य compile_udp_def(पूर्णाङ्क sequ, वर्ण *label, वर्ण *name,
		     अचिह्नित nin, अचिह्नित init, वर्ण **सारणी)
{
      यदि (sequ) {
	    vvp_bit4_t init4;
	    यदि (init == 0)
		  init4 = BIT4_0;
	    अन्यथा यदि (init == 1)
		  init4 = BIT4_1;
	    अन्यथा
		  init4 = BIT4_X;

	    vvp_udp_seq_s *u = नूतन vvp_udp_seq_s(label, name, nin, init4);
	    u->compile_सारणी(सारणी);
      } अन्यथा {
	    vvp_udp_comb_s *u = नूतन vvp_udp_comb_s(label, name, nin);
	    u->compile_सारणी(सारणी);
      }
      मुक्त(label);
}

वर्ण **compile_udp_सारणी(वर्ण **सारणी, वर्ण *row)
{
  यदि (सारणी)
    प्रतिज्ञा(वाक्यदैर्घ्यम्(*सारणी)==वाक्यदैर्घ्यम्(row));

  वर्ण **tt;
  प्रति (tt = सारणी; tt && *tt; tt++) { }
  पूर्णाङ्क n = (tt-सारणी) + 2;

  सारणी = static_cast<वर्ण**>(realloc(सारणी, n*परिमाण(वर्ण*)));
  सारणी[n-2] = row;
  सारणी[n-1] = 0x0;

  प्रत्यागम सारणी;
}


/*
 * The parser uses this function to compile and link an executable
 * opcode. I do this by looking up the opcode in the opcode_table. The
 * table gives the operand structure that is acceptable, so I can
 * process the operands here as well.
 */
शून्य compile_संहिता(वर्ण*label, वर्ण*mnem, comp_operands_t opa)
{
	/* First, I can give the label a value that is the current
	   codespace pointer. Don't need the text of the label after
	   this is done. */
      यदि (label)
	    compile_codelabel(label);

	/* Lookup the opcode in the opcode table. */
      संरचना opcode_सारणी_s*op = static_cast<संरचना opcode_सारणी_s*>
                                            (bsearch(mnem, opcode_सारणी, opcode_गणनम्,
                                                     परिमाण(संरचना opcode_सारणी_s),
                                                     &opcode_compare));
      यदि (op == 0) {
	    yyerror("Invalid opcode");
	    compile_errors += 1;
	    प्रत्यागम;
      }

      प्रतिज्ञा(op);

	/* Build up the code from the information about the opcode and
	   the information from the compiler. */
      vvp_संहिता_t code = codespace_allocate();
      code->opcode = op->opcode;

      यदि (op->argc != (opa? opa->argc : 0)) {
	    yyerror("operand count");
	    compile_errors += 1;
	    प्रत्यागम;
      }

	/* Pull the operands that the instruction expects from the
	   list that the parser supplied. */

      प्रति (अचिह्नित idx = 0 ;  idx < op->argc ;  idx += 1) {

	    विकल्प (op->argt[idx]) {
		पक्ष OA_NONE:
		  भङ्ग;

		पक्ष OA_ARR_PTR:
		  यदि (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			भङ्ग;
		  }

		  compile_array_lookup(code, opa->argv[idx].symb.text);
		  भङ्ग;

		पक्ष OA_BIT1:
		  यदि (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			भङ्ग;
		  }
		  code->bit_idx[0] = opa->argv[idx].numb;
		  भङ्ग;

		पक्ष OA_BIT2:
		  यदि (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			भङ्ग;
		  }
		  code->bit_idx[1] = opa->argv[idx].numb;
		  भङ्ग;

		पक्ष OA_संहिता_PTR:
		पक्ष OA_संहिता_PTR2:
		  यदि (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			भङ्ग;
		  }

		  प्रतिज्ञा(opa->argv[idx].symb.idx == 0);
		  संहिता_label_lookup(code, opa->argv[idx].symb.text,
				    op->argt[idx]==OA_संहिता_PTR2);
		  भङ्ग;

		पक्ष OA_FUNC_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  यदि (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			भङ्ग;
		  }

		  functor_ref_lookup(&code->net, opa->argv[idx].symb.text);
		  भङ्ग;

		पक्ष OA_FUNC_PTR2:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  यदि (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			भङ्ग;
		  }

		  functor_ref_lookup(&code->net2, opa->argv[idx].symb.text);
		  भङ्ग;

		पक्ष OA_सङ्ख्या:
		  यदि (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			भङ्ग;
		  }

		  code->सङ्ख्या = opa->argv[idx].numb;
		  भङ्ग;

		पक्ष OA_VPI_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  यदि (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			भङ्ग;
		  }

		  compile_vpi_lookup(&code->handle, opa->argv[idx].symb.text);
		  भङ्ग;

		पक्ष OA_STRING:
		  यदि (opa->argv[idx].ltype != L_STRING) {
			yyerror("operand format");
			भङ्ग;
		  }

		  code->text = opa->argv[idx].text;
		  भङ्ग;
	    }
      }

      मुक्त(opa);

      मुक्त(mnem);
}

शून्य compile_codelabel(वर्ण*label)
{
      चिह्नम्_मानम्_t val;
      vvp_संहिता_t ptr = codespace_अग्र();

      val.ptr = ptr;
      sym_समुच्चयः_मानम्(sym_codespace, label, val);

      मुक्त(label);
}

शून्य compile_file_पङ्क्तिः(वर्ण*label, दीर्घ file_idx, दीर्घ lineno,
                       वर्ण*description)
{
      यदि (label) compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_संहिता_t code = codespace_allocate();
      code->opcode = &सम्बन्धि_FILE_पङ्क्तिः;

	/* Create a vpiHandle that contains the information. */
      code->handle = vpip_build_file_पङ्क्तिः(description, file_idx, lineno);
      प्रतिज्ञा(code->handle);

	/* Done with the lexor-allocated name string. */
      विलोपय[] description;
}

शून्य compile_vpi_call(वर्ण*label, वर्ण*name,
                      तार्किक func_as_task_err, तार्किक func_as_task_warn,
                      दीर्घ file_idx, दीर्घ lineno,
                      अचिह्नित argc, vpiHandle*argv,
		      अचिह्नित vec4_राशिः, अचिह्नित real_राशिः, अचिह्नित string_राशिः)
{
      यदि (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_संहिता_t code = codespace_allocate();
      code->opcode = &सम्बन्धि_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, 0, 0,
					 0, func_as_task_err, func_as_task_warn,
                                         argc, argv, vec4_राशिः, real_राशिः, string_राशिः,
					 file_idx, lineno);
      यदि (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      विलोपय[] name;
}

शून्य compile_vpi_func_call(वर्ण*label, वर्ण*name,
			   पूर्णाङ्क val_type, अचिह्नित val_wid,
			   दीर्घ file_idx, दीर्घ lineno,
			   अचिह्नित argc, vpiHandle*argv,
			   अचिह्नित vec4_राशिः,
			   अचिह्नित real_राशिः,
			   अचिह्नित string_राशिः)
{
      यदि (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_संहिता_t code = codespace_allocate();
      code->opcode = &सम्बन्धि_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, val_type, val_wid,
					 0, सत्य, असत्य,
                                         argc, argv, vec4_राशिः, real_राशिः, string_राशिः,
					 file_idx, lineno);
      यदि (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      विलोपय[] name;
}

/*
 * When the parser finds a thread statement, I create a new thread
 * with the start address referenced by the program symbol passed to
 * me.
 */
शून्य compile_thread(वर्ण*start_sym, वर्ण*flag)
{
      तार्किक प्रक्षिप_flag = असत्य;

      चिह्नम्_मानम्_t tmp = sym_get_मानम्(sym_codespace, start_sym);
      vvp_संहिता_t pc = reinterpret_cast<vvp_संहिता_t>(tmp.ptr);
      यदि (pc == 0) {
	    yyerror("unresolved address");
	    प्रत्यागम;
      }

      यदि (flag && (वाक्यं_तुलय(flag,"$push") == 0))
	    प्रक्षिप_flag = सत्य;

      vthread_t thr = vthread_new(pc, vpip_peek_current_scope());

      यदि (flag && (वाक्यं_तुलय(flag,"$init") == 0))
	    कालसारणी_init_vthread(thr);
      अन्यथा यदि (flag && (वाक्यं_तुलय(flag,"$final") == 0))
	    कालसारणी_final_vthread(thr);
      अन्यथा
	    कालसारणी_vthread(thr, 0, प्रक्षिप_flag);

      मुक्त(start_sym);
      मुक्त(flag);
}

शून्य compile_param_logic(वर्ण*label, ध्रुव वर्ण*name, वर्ण*value, तार्किक signed_flag,
                         तार्किक local_flag,
                         दीर्घ file_idx, दीर्घ lineno)
{
      vvp_vector4_t value4 = c4string_गन्तव्यम्_vector4(value);
      vpiHandle obj = vpip_make_द्विमान_param(name, value4, signed_flag,
                                             local_flag, file_idx, lineno);
      compile_vpi_चिह्नम्(label, obj);
      vpip_attach_गन्तव्यम्_current_scope(obj);

      मुक्त(label);
      मुक्त(value);
}

शून्य compile_param_string(वर्ण*label, ध्रुव वर्ण*name, वर्ण*value,
                          तार्किक local_flag,
                          दीर्घ file_idx, दीर्घ lineno)
{
	// name and value become owned by vpip_make_string_param
      vpiHandle obj = vpip_make_string_param(name, value, local_flag, file_idx, lineno);
      compile_vpi_चिह्नम्(label, obj);
      vpip_attach_गन्तव्यम्_current_scope(obj);

      मुक्त(label);
}

शून्य compile_param_real(वर्ण*label, ध्रुव वर्ण*name, वर्ण*value,
                        तार्किक local_flag,
                        दीर्घ file_idx, दीर्घ lineno)
{
      द्विगुण dvalue = crstring_गन्तव्यम्_double(value);
      vpiHandle obj = vpip_make_real_param(name, dvalue, local_flag, file_idx, lineno);
      compile_vpi_चिह्नम्(label, obj);
      vpip_attach_गन्तव्यम्_current_scope(obj);

      मुक्त(label);
      मुक्त(value);
}

शून्य compile_island(वर्ण*label, वर्ण*type)
{
      यदि (वाक्यं_तुलय(type,"tran") == 0)
	    compile_island_tran(label);
      अन्यथा
	    प्रतिज्ञा(0);

      मुक्त(type);
}
