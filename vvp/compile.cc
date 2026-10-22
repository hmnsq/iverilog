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
#अन्तर्भाव <windows.h>
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

गणना operand_e {
	/* Place holder for unused operand */
      OA_NONE,
	/* The operand is a number, an immediate unsigned integer */
      OA_संख्या,
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

संरचना opcode_table_s {
      ध्रुव वर्ण*mnemonic;
      vvp_code_fun opcode;

      unsigned argc;
      गणना operand_e argt[OPERAND_MAX];
};

स्थिर ध्रुव संरचना opcode_table_s opcode_table[] = {
      { "%abs/wr", of_ABS_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add",    of_ADD,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add/wr", of_ADD_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%addi",   of_ADDI,   3,  {OA_BIT1,     OA_BIT2,     OA_संख्या} },
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
      { "%cast/vec2/dar", of_CAST_VEC2_DAR, 1,  {OA_संख्या,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/dar", of_CAST_VEC4_DAR, 1,  {OA_संख्या,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/str", of_CAST_VEC4_STR, 1,  {OA_संख्या,   OA_NONE,     OA_NONE} },
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
      { "%cmpi/e",  of_CMPIE,  3,  {OA_BIT1,     OA_BIT2,     OA_संख्या} },
      { "%cmpi/ne", of_CMPINE, 3,  {OA_BIT1,     OA_BIT2,     OA_संख्या} },
      { "%cmpi/s",  of_CMPIS,  3,  {OA_BIT1,     OA_BIT2,     OA_संख्या} },
      { "%cmpi/u",  of_CMPIU,  3,  {OA_BIT1,     OA_BIT2,     OA_संख्या} },
      { "%cmpix/ltu", of_CMPIX_LTU, 2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%cmpix/slt0", of_CMPIX_SLT0, 1, {OA_संख्या, OA_NONE, OA_NONE} },
      { "%concat/str",  of_CONCAT_STR,  0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concat/vec4", of_CONCAT_VEC4, 0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concati/str", of_CONCATI_STR, 1,{OA_STRING,OA_NONE,  OA_NONE} },
      { "%concati/vec4",of_CONCATI_VEC4,3,{OA_BIT1,  OA_BIT2,  OA_संख्या} },
      { "%cvt/rv",   of_CVT_RV,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/rv/s", of_CVT_RV_S,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/sr", of_CVT_SR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/ur", of_CVT_UR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/vr", of_CVT_VR, 1,  {OA_संख्या,   OA_NONE,     OA_NONE} },
      { "%deassign",of_DEASSIGN,3,{OA_FUNC_PTR, OA_BIT1,     OA_BIT2} },
      { "%deassign/wr",of_DEASSIGN_WR,1,{OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%debug/thr",  of_DEBUG_THR,  1,{OA_STRING,   OA_NONE,     OA_NONE} },
      { "%delay",  of_DELAY,  2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%delayx", of_DELAYX, 1,  {OA_संख्या,   OA_NONE,     OA_NONE} },
      { "%delete/elem",of_DELETE_ELEM,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%delete/obj",of_DELETE_OBJ,1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%delete/prop/elem", of_DELETE_PROP_ELEM, 1, {OA_संख्या, OA_NONE, OA_NONE} },
      { "%delete/prop/obj",  of_DELETE_PROP_OBJ,  1, {OA_संख्या, OA_NONE, OA_NONE} },
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
      { "%flag_get/vec4", of_FLAG_GET_VEC4, 1, {OA_संख्या, OA_NONE, OA_NONE} },
      { "%flag_inv",      of_FLAG_INV,      1, {OA_BIT1,   OA_NONE, OA_NONE} },
      { "%flag_mov",      of_FLAG_MOV,      2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_or",       of_FLAG_OR,       2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_set/imm",  of_FLAG_SET_IMM,  2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%flag_set/vec4", of_FLAG_SET_VEC4, 1, {OA_संख्या, OA_NONE, OA_NONE} },
      { "%force/link",    of_FORCE_LINK,2,{OA_FUNC_PTR, OA_FUNC_PTR2, OA_NONE} },
      { "%force/vec4",    of_FORCE_VEC4,    1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%force/vec4/off",of_FORCE_VEC4_OFF,2,{OA_FUNC_PTR, OA_BIT1,      OA_NONE} },
      { "%force/vec4/off/d",of_FORCE_VEC4_OFF_D,3,{OA_FUNC_PTR, OA_BIT1,  OA_BIT2} },
      { "%force/wr",      of_FORCE_WR,      1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%fork",   of_FORK,   2,  {OA_CODE_PTR2,OA_VPI_PTR,  OA_NONE} },
      { "%free",   of_FREE,   1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%inv",    of_INV,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%ix/add", of_IX_ADD, 3,  {OA_संख्या,   OA_BIT1,     OA_BIT2} },
      { "%ix/getv",of_IX_GETV,2,  {OA_BIT1,     OA_FUNC_PTR, OA_NONE} },
      { "%ix/getv/s",of_IX_GETV_S,2, {OA_BIT1,   OA_FUNC_PTR, OA_NONE} },
      { "%ix/load",of_IX_LOAD,3,  {OA_संख्या,   OA_BIT1,     OA_BIT2} },
      { "%ix/mov", of_IX_MOV, 2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%ix/mul", of_IX_MUL, 3,  {OA_संख्या,   OA_BIT1,     OA_BIT2} },
      { "%ix/sub", of_IX_SUB, 3,  {OA_संख्या,   OA_BIT1,     OA_BIT2} },
      { "%ix/vec4",  of_IX_VEC4,  1,  {OA_संख्या,   OA_NONE, OA_NONE} },
      { "%ix/vec4/s",of_IX_VEC4_S,1,  {OA_संख्या,   OA_NONE, OA_NONE} },
      { "%jmp",    of_JMP,    1,  {OA_CODE_PTR, OA_NONE,     OA_NONE} },
      { "%jmp/0",  of_JMP0,   2,  {OA_CODE_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/0xz",of_JMP0XZ, 2,  {OA_CODE_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1",  of_JMP1,   2,  {OA_CODE_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1xz",of_JMP1XZ, 2,  {OA_CODE_PTR, OA_BIT1,     OA_NONE} },
      { "%join",   of_JOIN,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%join/detach",of_JOIN_DETACH,1,{OA_संख्या,OA_NONE,  OA_NONE} },
      { "%load/ar",of_LOAD_AR,2,  {OA_ARR_PTR,  OA_BIT1,     OA_NONE} },
      { "%load/dar/r",  of_LOAD_DAR_R,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE}},
      { "%load/dar/str",of_LOAD_DAR_STR,  1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/dar/vec4",of_LOAD_DAR_VEC4,1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/obj",   of_LOAD_OBJ,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/obja",  of_LOAD_OBJA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/prop/dar/vec4", of_LOAD_PROP_DAR_VEC4, 2, {OA_संख्या, OA_BIT1, OA_NONE} },
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
      { "%muli",   of_MULI,   3,  {OA_BIT1,     OA_BIT2,     OA_संख्या} },
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
      { "%pad/s",  of_PAD_S,  1,  {OA_संख्या,   OA_NONE,     OA_NONE} },
      { "%pad/u",  of_PAD_U,  1,  {OA_संख्या,   OA_NONE,     OA_NONE} },
      { "%part/s", of_PART_S, 1,  {OA_संख्या,   OA_NONE,     OA_NONE} },
      { "%part/u", of_PART_U, 1,  {OA_संख्या,   OA_NONE,     OA_NONE} },
      { "%parti/s",of_PARTI_S,3,  {OA_संख्या,   OA_BIT1,     OA_BIT2} },
      { "%parti/u",of_PARTI_U,3,  {OA_संख्या,   OA_BIT1,     OA_BIT2} },
      { "%pop/obj", of_काढा_OBJ, 2, {OA_BIT1,    OA_BIT2,     OA_NONE} },
      { "%pop/real",of_काढा_REAL,1, {OA_संख्या,  OA_NONE,     OA_NONE} },
      { "%pop/str", of_काढा_STR, 1, {OA_संख्या,  OA_NONE,     OA_NONE} },
      { "%pop/vec4",of_काढा_VEC4,1, {OA_संख्या,  OA_NONE,     OA_NONE} },
      { "%pow",     of_POW,     0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/s",   of_POW_S,   0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/wr",  of_POW_WR,  0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%prop/obj",of_PROP_OBJ,2,  {OA_संख्या,   OA_BIT1,     OA_NONE} },
      { "%prop/queue/size", of_PROP_QUEUE_SIZE,  1, {OA_संख्या, OA_NONE, OA_NONE} },
      { "%prop/r",  of_PROP_R,  1,  {OA_संख्या,   OA_NONE,     OA_NONE} },
      { "%prop/str",of_PROP_STR,1,  {OA_संख्या,   OA_NONE,     OA_NONE} },
      { "%prop/v",  of_PROP_V,  1,  {OA_संख्या,   OA_NONE,     OA_NONE} },
      { "%push/ix/vec4", of_ढकला_IX_VEC4, 3, {OA_संख्या, OA_BIT1, OA_BIT2} },
      { "%pushi/real",of_PUSHI_REAL,2,{OA_BIT1,   OA_BIT2,   OA_NONE} },
      { "%pushi/str", of_PUSHI_STR, 1,{OA_STRING, OA_NONE,   OA_NONE} },
      { "%pushi/vec4",of_PUSHI_VEC4,3,{OA_BIT1,   OA_BIT2,   OA_संख्या} },
      { "%pushv/str", of_PUSHV_STR, 0,{OA_NONE,   OA_NONE,   OA_NONE} },
      { "%putc/str/vec4",of_PUTC_STR_VEC4,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/prop/r",   of_QINSERT_PROP_R,   2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/str", of_QINSERT_PROP_STR, 2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/v",   of_QINSERT_PROP_V,   3, {OA_संख्या, OA_BIT1, OA_BIT2} },
      { "%qinsert/real",of_QINSERT_REAL,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/str", of_QINSERT_STR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/v",   of_QINSERT_V,   3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%qpop/b/real",of_QPOP_B_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/str", of_QPOP_B_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/v",   of_QPOP_B_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/f/real",of_QPOP_F_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/str", of_QPOP_F_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/v",   of_QPOP_F_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/prop/b/r",   of_QPOP_PROP_B_REAL, 1, {OA_संख्या, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/str", of_QPOP_PROP_B_STR, 1, {OA_संख्या, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/v",   of_QPOP_PROP_B_V,    2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%qpop/prop/f/r",   of_QPOP_PROP_F_REAL, 1, {OA_संख्या, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/str", of_QPOP_PROP_F_STR, 1, {OA_संख्या, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/v",   of_QPOP_PROP_F_V,    2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%queue/append_word/v", of_QUEUE_APPEND_WORD_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/find/index/prop/v", of_QUEUE_FIND_INDEX_PROP_V, 2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%queue/find/index/v", of_QUEUE_FIND_INDEX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find/prop/v", of_QUEUE_FIND_PROP_V, 2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%queue/find/v", of_QUEUE_FIND_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/prop/v", of_QUEUE_FIND_FIRST_INDEX_PROP_V, 2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/v", of_QUEUE_FIND_FIRST_INDEX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/prop/v", of_QUEUE_FIND_FIRST_PROP_V, 2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%queue/find_first/v", of_QUEUE_FIND_FIRST_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/prop/v", of_QUEUE_FIND_LAST_INDEX_PROP_V, 2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/v", of_QUEUE_FIND_LAST_INDEX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/prop/v", of_QUEUE_FIND_LAST_PROP_V, 2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%queue/find_last/v", of_QUEUE_FIND_LAST_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/max/obj/v", of_QUEUE_MAX_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/max/prop/v", of_QUEUE_MAX_PROP_V, 2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%queue/max/v", of_QUEUE_MAX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/min/obj/v", of_QUEUE_MIN_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/min/prop/v", of_QUEUE_MIN_PROP_V, 2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%queue/min/v", of_QUEUE_MIN_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/new_empty/v", of_QUEUE_NEW_EMPTY_V, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%queue/size/v", of_QUEUE_SIZE_V, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%queue/unique/index/obj/v", of_QUEUE_UNIQUE_INDEX_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/index/prop/v", of_QUEUE_UNIQUE_INDEX_PROP_V, 2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%queue/unique/index/v", of_QUEUE_UNIQUE_INDEX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/unique/obj/v", of_QUEUE_UNIQUE_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/prop/v", of_QUEUE_UNIQUE_PROP_V, 2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%queue/unique/v", of_QUEUE_UNIQUE_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/word/prop/v", of_QUEUE_WORD_PROP_V, 3, {OA_संख्या, OA_BIT1, OA_BIT2} },
      { "%queue/word/v", of_QUEUE_WORD_V, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%release/net",of_RELEASE_NET,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/reg",of_RELEASE_REG,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/wr", of_RELEASE_WR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%replicate", of_REPLICATE,   1,{OA_संख्या,  OA_NONE,OA_NONE} },
      { "%ret/real",  of_RET_REAL,    1,{OA_संख्या,  OA_NONE,OA_NONE} },
      { "%ret/str",   of_RET_STR,     1,{OA_संख्या,  OA_NONE,OA_NONE} },
      { "%ret/vec4",  of_RET_VEC4,    3,{OA_संख्या,  OA_BIT1,OA_BIT2} },
      { "%retload/real",of_RETLOAD_REAL,1,{OA_संख्या,  OA_NONE,OA_NONE} },
      { "%retload/str", of_RETLOAD_STR, 1,{OA_संख्या,  OA_NONE,OA_NONE} },
      { "%retload/vec4",of_RETLOAD_VEC4,1,{OA_संख्या,  OA_NONE,OA_NONE} },
      { "%reverse/obj", of_REVERSE_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%reverse/prop/obj", of_REVERSE_PROP_OBJ, 1, {OA_संख्या, OA_NONE, OA_NONE} },
      { "%rsort/obj", of_RSORT_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%rsort/prop/obj", of_RSORT_PROP_OBJ, 1, {OA_संख्या, OA_NONE, OA_NONE} },
      { "%scopy",  of_SCOPY,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%set/dar/obj/real",of_SET_DAR_OBJ_REAL,1,{OA_संख्या,OA_NONE,OA_NONE} },
      { "%set/dar/obj/str", of_SET_DAR_OBJ_STR, 1,{OA_संख्या,OA_NONE,OA_NONE} },
      { "%set/dar/obj/vec4",of_SET_DAR_OBJ_VEC4,1,{OA_संख्या,OA_NONE,OA_NONE} },
      { "%shiftl",   of_SHIFTL,   1, {OA_संख्या, OA_NONE,   OA_NONE} },
      { "%shiftr",   of_SHIFTR,   1, {OA_संख्या, OA_NONE,   OA_NONE} },
      { "%shiftr/s", of_SHIFTR_S, 1, {OA_संख्या, OA_NONE,   OA_NONE} },
      { "%shuffle/obj", of_SHUFFLE_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%shuffle/prop/obj", of_SHUFFLE_PROP_OBJ, 1, {OA_संख्या, OA_NONE, OA_NONE} },
      { "%sort/obj", of_SORT_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%sort/prop/obj", of_SORT_PROP_OBJ, 1, {OA_संख्या, OA_NONE, OA_NONE} },
      { "%split/vec4",    of_SPLIT_VEC4,    1,{OA_संख्या,   OA_NONE, OA_NONE} },
      { "%store/dar/r",   of_STORE_DAR_R,   1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/str", of_STORE_DAR_STR, 1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/vec4",of_STORE_DAR_VEC4,1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/obj",   of_STORE_OBJ,   1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/obja",  of_STORE_OBJA,  2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/prop/obj",of_STORE_PROP_OBJ,2, {OA_संख्या,  OA_BIT1, OA_NONE} },
      { "%store/prop/qb/r",   of_STORE_PROP_QB_R,   2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/str", of_STORE_PROP_QB_STR, 2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/v",   of_STORE_PROP_QB_V,   3, {OA_संख्या, OA_BIT1, OA_BIT2} },
      { "%store/prop/qf/r",   of_STORE_PROP_QF_R,   2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/str", of_STORE_PROP_QF_STR, 2, {OA_संख्या, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/v",   of_STORE_PROP_QF_V,   3, {OA_संख्या, OA_BIT1, OA_BIT2} },
      { "%store/prop/r",  of_STORE_PROP_R,  1, {OA_संख्या,  OA_NONE, OA_NONE} },
      { "%store/prop/str",of_STORE_PROP_STR,1, {OA_संख्या,  OA_NONE, OA_NONE} },
      { "%store/prop/v",  of_STORE_PROP_V,  2, {OA_संख्या,  OA_BIT1, OA_NONE} },
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
      { "%subi",   of_SUBI,   3,  {OA_BIT1,     OA_BIT2,     OA_संख्या} },
      { "%substr",     of_SUBSTR,     2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%substr/vec4",of_SUBSTR_VEC4,2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%test_nul",     of_TEST_NUL,     1,{OA_FUNC_PTR,OA_NONE,    OA_NONE} },
      { "%test_nul/a",   of_TEST_NUL_A,   2,{OA_ARR_PTR, OA_BIT1,    OA_NONE} },
      { "%test_nul/obj", of_TEST_NUL_OBJ, 0,{OA_NONE,    OA_NONE,    OA_NONE} },
      { "%test_nul/prop",of_TEST_NUL_PROP,2,{OA_संख्या,  OA_BIT1,    OA_NONE} },
      { "%uparr/and/v",          of_UPARR_AND_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/or/v",           of_UPARR_OR_V,           2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/product/obj/v",  of_UPARR_PRODUCT_OBJ_V,  1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/product/prop/v", of_UPARR_PRODUCT_PROP_V, 2, {OA_संख्या,   OA_BIT1, OA_NONE} },
      { "%uparr/product/r",      of_UPARR_PRODUCT_REAL,   1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/product/v",      of_UPARR_PRODUCT_V,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/sum/obj/v",      of_UPARR_SUM_OBJ_V,      1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/sum/prop/v",     of_UPARR_SUM_PROP_V,     2, {OA_संख्या,   OA_BIT1, OA_NONE} },
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

स्थिर ध्रुव unsigned opcode_मोजणी =
                    परिमाण(opcode_table)/परिमाण(*opcode_table) - 1;

स्थिर पूर्णाङ्क opcode_compare(ध्रुव शून्य*k, ध्रुव शून्य*r)
{
      ध्रुव वर्ण*kp = static_cast<ध्रुव वर्ण*>(k);
      ध्रुव संरचना opcode_table_s*rp = static_cast<ध्रुव संरचना opcode_table_s*>(r);
      प्रत्यागम strcmp(kp, rp->mnemonic);
}

/*
 * Keep a symbol table of addresses within code space. Labels on
 * executable opcodes are mapped to their address here.
 */
स्थिर चिन्ह_table_t sym_codespace = 0;

/*
 * Keep a symbol table of functors mentioned in the source. This table
 * is used to resolve references as they come.
 */
स्थिर चिन्ह_table_t sym_functors = 0;

/*
 * VPI objects are indexed during compile time so that they can be
 * linked together as they are created. This symbol table matches
 * labels to vpiHandles.
 */
स्थिर चिन्ह_table_t sym_vpi = 0;


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

शून्य define_functor_चिन्ह(ध्रुव वर्ण*label, vvp_net_t*net)
{
      चिन्ह_मूल्य_t val;
      val.net = net;
      sym_set_मूल्य(sym_functors, label, val);
}

स्थिर vvp_net_t*lookup_functor_चिन्ह(ध्रुव वर्ण*label)
{
      प्रतिज्ञा(sym_functors);
      चिन्ह_मूल्य_t val = sym_get_मूल्य(sym_functors, label);
      प्रत्यागम val.net;
}

vpiHandle vvp_lookup_handle(ध्रुव वर्ण*label)
{
      चिन्ह_मूल्य_t val = sym_get_मूल्य(sym_vpi, label);
      यदि (val.ptr) प्रत्यागम (vpiHandle) val.ptr;
      प्रत्यागम 0;
}

vvp_net_t* vvp_net_lookup(ध्रुव वर्ण*label)
{
      स्थिर तार्किक t0_trigger_generated = असत्य;

        /* First, look to see if the symbol is a vpi object of some
	   sort. If it is, then get the vvp_ipoint_t pointer out of
	   the vpiHandle. */
      चिन्ह_मूल्य_t val = sym_get_मूल्य(sym_vpi, label);
      यदि (val.ptr) {
	    vpiHandle vpi = static_cast<vpiHandle>(val.ptr);
	    विकल्प (vpi->get_type_code()) {
		पक्ष vpiNet:
		पक्ष vpiReg:
		पक्ष vpiBitVar:
		पक्ष vpiByteVar:
		पक्ष vpiShortIntVar:
		पक्ष vpiIntVar:
		पक्ष vpiLongIntVar:
		पक्ष vpiIntegerVar: {
		      ध्रुव __vpiSignal*sig = dynamic_cast<__vpiSignal*>(vpi);
		      प्रत्यागम sig->गाठ;
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
		          vpi->get_type_code());
		  प्रतिज्ञा(0);
	    }
      }


	/* Failing that, look for a general functor. */
      vvp_net_t*tmp = lookup_functor_चिन्ह(label);

	// This is a special label used to create a T0 trigger for the
	// always_comb/latch processes.
     यदि (! t0_trigger_generated && (strcmp(label, "E_0x0") == 0)) {
	      // This should never happen, but if it does then the E_0x0
	      // event generation may need to be explicitly generated in
	      // the compiler output instead of implicitly in this code.
	    प्रतिज्ञा(! vpip_peek_current_scope()->is_automatic());
	    t0_trigger_generated = सत्य;
	      // Create a local event with no name for the T0 trigger
	    compile_named_event(strdup(label), strcpy(नूतन वर्ण [1],""), सत्य);
	    tmp = vvp_net_lookup(label);
	    प्रतिज्ञा(tmp);
	      // Create a trigger for the T0 event.
	    vvp_net_ptr_t ptr (tmp, 0);
	    schedule_t0_trigger(ptr);
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
	    delete cur;
	    प्रत्यागम;
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
संरचना vvp_net_resolv_list_s: सार्वजनिक resolv_list_s {

      explicit vvp_net_resolv_list_s(वर्ण*l) : resolv_list_s(l) { }
	// port to be driven by the located node.
      vvp_net_ptr_t port;
      आभास तार्किक resolve(तार्किक mes) override;
};

तार्किक vvp_net_resolv_list_s::resolve(तार्किक mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      यदि (tmp) {
	      // Link the input port to the located output.
	    tmp->link(port);
	    प्रत्यागम सत्य;
      }

      यदि (mes)
	    fprintf(stderr, "unresolved vvp_net reference: %s\n", label());

      प्रत्यागम असत्य;
}

अन्तःस्थ स्थिर
शून्य postpone_functor_आदान(vvp_net_ptr_t port, वर्ण*lab)
{
      संरचना vvp_net_resolv_list_s*res = नूतन संरचना vvp_net_resolv_list_s(lab);
      res->port   = port;

      resolv_submit(res);
}


/*
 *  Generic functor reference lookup.
 */

संरचना functor_gen_resolv_list_s: सार्वजनिक resolv_list_s {
      explicit functor_gen_resolv_list_s(वर्ण*txt) : resolv_list_s(txt) {
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
      explicit vpi_handle_resolv_list_s(वर्ण*lab) : resolv_list_s(lab) {
	    handle = रिक्त;
      }
      आभास तार्किक resolve(तार्किक mes) override;
      vpiHandle *handle;
};

तार्किक vpi_handle_resolv_list_s::resolve(तार्किक mes)
{
      चिन्ह_मूल्य_t val = sym_get_मूल्य(sym_vpi, label());
      यदि (!val.ptr) {
	    // check for thread access symbols
	    unsigned base, wid;
	    size_t n = 0;
	    वर्ण ss[32];
	    यदि (2 == sscanf(label(), "W<%u,%31[r]>%zn", &base, ss, &n)
		       && n == strlen(label())) {

		  val.ptr = vpip_make_vthr_word(base, ss);
		  sym_set_मूल्य(sym_vpi, label(), val);

	    } अन्यथा यदि (1 == sscanf(label(), "S<%u,str>%zn", &base, &n)
		       && n == strlen(label())) {

		  val.ptr = vpip_make_vthr_str_चळत(base);
		  sym_set_मूल्य(sym_vpi, label(), val);

	    } अन्यथा यदि (3 == sscanf(label(), "S<%u,vec4,%31[su]%u>%zn", &base, ss, &wid, &n)
		       && n == strlen(label())) {

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
		  val.ptr = vpip_make_vthr_vec4_चळत(base, signed_flag, wid);
		  sym_set_मूल्य(sym_vpi, label(), val);
	    }

      }

      यदि (!val.ptr) {
	    // check for memory word  M<mem,base,wid>
      }

      यदि (strcmp(label(), "null") == 0) {
	    val.ptr = vpip_make_null_const();
	    sym_set_मूल्य(sym_vpi, label(), val);
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
      यदि (strcmp(label, "$time") == 0) {
	    *handle = vpip_sim_time(vpip_peek_current_scope(), असत्य);
	    मुक्त(label);
	    प्रत्यागम;
      }

      यदि (strcmp(label, "$stime") == 0) {
	    *handle = vpip_sim_time(vpip_peek_current_scope(), सत्य);
	    मुक्त(label);
	    प्रत्यागम;
      }

      यदि (strcmp(label, "$realtime") == 0) {
	    *handle = vpip_sim_realtime(vpip_peek_current_scope());
	    मुक्त(label);
	    प्रत्यागम;
      }

      यदि (strcmp(label, "$simtime") == 0) {
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

संरचना code_label_resolv_list_s: सार्वजनिक resolv_list_s {
      explicit code_label_resolv_list_s(वर्ण*lab, तार्किक cptr2) : resolv_list_s(lab) {
	    code = रिक्त;
	    cptr2_flag = cptr2;
      }
      संरचना vvp_code_s *code;
      तार्किक cptr2_flag;
      आभास तार्किक resolve(तार्किक mes) override;
};

तार्किक code_label_resolv_list_s::resolve(तार्किक mes)
{
      चिन्ह_मूल्य_t val = sym_get_मूल्य(sym_codespace, label());
      यदि (val.ptr) {
	    यदि (cptr2_flag)
		  code->cptr2 = reinterpret_cast<vvp_code_t>(val.ptr);
	    अन्यथा
		  code->cptr = reinterpret_cast<vvp_code_t>(val.ptr);
	    प्रत्यागम सत्य;
      }

      यदि (mes)
	    fprintf(stderr, "unresolved code label: %s\n", label());

      प्रत्यागम असत्य;
}

शून्य code_label_lookup(संरचना vvp_code_s *code, वर्ण *label, तार्किक cptr2)
{
      संरचना code_label_resolv_list_s *res
	    = नूतन संरचना code_label_resolv_list_s(label, cptr2);

      res->code  = code;

      resolv_submit(res);
}

संरचना code_array_resolv_list_s: सार्वजनिक resolv_list_s {
      explicit code_array_resolv_list_s(वर्ण*lab) : resolv_list_s(lab) {
	    code = रिक्त;
      }
      संरचना vvp_code_s *code;
      आभास तार्किक resolve(तार्किक mes) override;
};

तार्किक code_array_resolv_list_s::resolve(तार्किक mes)
{
      code->array = array_find(label());
      यदि (code->array != 0) {
	    प्रत्यागम सत्य;
      }

      यदि (mes)
	    fprintf(stderr, "Array unresolved: %s\n", label());

      प्रत्यागम असत्य;
}

स्थिर शून्य compile_array_lookup(संरचना vvp_code_s*code, वर्ण*label)
{
      संरचना code_array_resolv_list_s *res
	    = नूतन संरचना code_array_resolv_list_s(label);

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
      पूर्णाङ्क last;

      यदि (verbose_flag) {
	    fprintf(stderr, " ... Linking\n");
	    fflush(stderr);
      }

      कुरु {
	    resolv_list_s *res = resolv_list;
	    resolv_list = 0x0;
	    last = nerrs == lnerrs;
	    lnerrs = nerrs;
	    nerrs = 0;
	    यावत् (res) {
		  resolv_list_s *cur = res;
		  res = res->next;
		  यदि (cur->resolve(last))
			delete cur;
		  अन्यथा {
			nerrs++;
			cur->next = resolv_list;
			resolv_list = cur;
		  }
	    }
	    यदि (nerrs && last)
		  fprintf(stderr,
			  "compile_cleanup: %d unresolved items\n",
			  nerrs);
      } यावत् (nerrs && !last);

      compile_errors += nerrs;

      यदि (verbose_flag) {
	    fprintf(stderr, " ... Removing symbol tables\n");
	    fflush(stderr);
      }

	/* After compile is complete, the vpi symbol table is no
	   longer needed. VPI objects are located by following
	   scopes. */
      delete_चिन्ह_table(sym_vpi);
      sym_vpi = 0;

	/* Don't need the code labels. The instructions have numeric
	   pointers in them, the symbol table is no longer needed. */
      delete_चिन्ह_table(sym_codespace);
      sym_codespace = 0;

      delete_चिन्ह_table(sym_functors);
      sym_functors = 0;

      delete_udp_symbols();

      compile_island_cleanup();
      compile_array_cleanup();

      यदि (verbose_flag) {
	    fprintf(stderr, " ... Compiletf functions\n");
	    fflush(stderr);
      }

      प्रतिज्ञा(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_COMPILETF;

      यावत् (! scheduled_compiletf.empty()) {
	    संरचना __vpiSysTaskCall*obj = scheduled_compiletf.front();
	    scheduled_compiletf.pop_front();
	    vpip_cur_task = obj;
	    obj->defn->info.compiletf (obj->defn->info.user_data);
	    vpip_cur_task = 0;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

शून्य compile_vpi_चिन्ह(ध्रुव वर्ण*label, vpiHandle obj)
{
      चिन्ह_मूल्य_t val;
      val.ptr = obj;
      sym_set_मूल्य(sym_vpi, label, val);
}

/*
 * Initialize the compiler by allocation empty symbol tables and
 * initializing the various address spaces.
 */
शून्य compile_init(शून्य)
{
      sym_vpi = new_चिन्ह_table();

      sym_functors = new_चिन्ह_table();

      sym_codespace = new_चिन्ह_table();
      codespace_init();
}

शून्य compile_load_vpi_module(वर्ण*name)
{
      vpip_load_module(name);
      delete[] name;
}

शून्य compile_vpi_time_precision(long pre)
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
स्थिर तार्किक crstring_header_test(ध्रुव वर्ण*str)
{
      यदि ((str[0] != 'C') && (str[0] != 'c'))
	    प्रत्यागम असत्य;
      यदि ((str[1] != 'r') || (str[2] != '<'))
	    प्रत्यागम असत्य;

      प्रत्यागम सत्य;
}

तार्किक crstring_test(ध्रुव वर्ण*str)
{
      यदि (!crstring_header_test(str))
	    प्रत्यागम असत्य;
      ध्रुव वर्ण*tp = strchr(str, '>');
      यदि (tp == 0) प्रत्यागम असत्य;
      यदि (tp[1] != 0) प्रत्यागम असत्य;

      यदि ((strspn(str+3, "0123456789abcdefmg")+3) != (size_t)(tp - str))
	  प्रत्यागम असत्य;

      प्रत्यागम सत्य;
}

द्विगुण crstring_to_double(ध्रुव वर्ण*label)
{
      प्रतिज्ञा(crstring_header_test(label));

      ध्रुव वर्ण*cp = label+3;
      प्रतिज्ञा(*cp == 'm');
      cp += 1;
      वर्ण*ep;
      uint64_t mant = strtoull(cp, &ep, 16);
      cp = ep;
      प्रतिज्ञा(*cp == 'g');
      cp += 1;
      पूर्णाङ्क exp = strtoul(cp, 0, 16);

      द्विगुण tmp;
      यदि (mant == 0 && exp == 0x3fff) {
	    tmp = INFINITY;
      } अन्यथा यदि (mant == 0 && exp == 0x7fff) {
	    tmp = -INFINITY;
      } अन्यथा यदि (exp == 0x3fff) {
	    tmp = nan("");
      } अन्यथा {
	    द्विगुण sign = (exp & 0x4000)? -1.0 : 1.0;
	    exp &= 0x1fff;

	    tmp = sign * ldexp((द्विगुण)mant, exp - 0x1000);
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

शून्य आदान_connect(vvp_net_t*fdx, unsigned port, वर्ण*label)
{
      vvp_net_ptr_t ifdx = vvp_net_ptr_t(fdx, port);

	/* Is this a vvp_vector4_t constant value? */
      यदि (c4string_test(label)) {

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
	    यदि (label[0] == 'c')
		  schedule_init_vector(ifdx, tmp);
	    अन्यथा
		  schedule_set_vector(ifdx, tmp);

	    मुक्त(label);
	    प्रत्यागम;
      }

	/* Is this a vvp_vector8_t constant value? */
      यदि (c8string_test(label)) {

	    vvp_vector8_t tmp = c8string_to_vector8(label);
	    यदि (label[0] == 'c')
		  schedule_init_vector(ifdx, tmp);
	    अन्यथा
		  schedule_set_vector(ifdx, tmp);

	    मुक्त(label);
	    प्रत्यागम;
      }

	/* Handle the Cr<> constant driver, which is a real-value
	   driver. */
      यदि (crstring_test(label)) {

	    द्विगुण tmp = crstring_to_double(label);

	    यदि (label[0] == 'c')
		  schedule_init_vector(ifdx, tmp);
	    अन्यथा
		  schedule_set_vector(ifdx, tmp);
	    मुक्त(label);
	    प्रत्यागम;
      }

	/* Handle the general case that this is a label for a node in
	   the vvp net. This arranges for the label to be preserved in
	   a linker list, and linked when the symbol table is
	   complete. */
      postpone_functor_आदान(ifdx, label);
}

शून्य inputs_connect(vvp_net_t*fdx, unsigned argc, संरचना symb_s*argv)
{
      यदि (argc > 4) {
	    std::cerr << "XXXX argv[0] = " << argv[0].text << std::endl;
      }
      प्रतिज्ञा(argc <= 4);

      प्रति (unsigned idx = 0;  idx < argc;  idx += 1) {

	    आदान_connect(fdx, idx, argv[idx].text);
      }
}

शून्य wide_inputs_connect(vvp_wide_fun_core*core,
			 unsigned argc, संरचना symb_s*argv)
{
	/* Create input functors to receive values from the
	   network. These functors pass the data to the core. */
      unsigned आदान_functors = (argc+3) / 4;
      प्रति (unsigned idx = 0 ;  idx < आदान_functors ;  idx += 1) {
	    unsigned base = idx*4;
	    unsigned trans = 4;
	    यदि (base+trans > argc)
		  trans = argc - base;

	    vvp_wide_fun_t*cur = नूतन vvp_wide_fun_t(core, base);
	    vvp_net_t*ptr = नूतन vvp_net_t;
	    ptr->fun = cur;

	    inputs_connect(ptr, trans, argv+base);
      }
}

template <वर्ग T_> शून्य make_arith(T_ *arith, वर्ण*label,
				    unsigned argc, संरचना symb_s*argv)
{
      vvp_net_t* ptr = नूतन vvp_net_t;
      ptr->fun = arith;

      define_functor_चिन्ह(label, ptr);
      मुक्त(label);

      प्रतिज्ञा(argc == 2);
      inputs_connect(ptr, argc, argv);

      मुक्त(argv);
}

शून्य compile_arith_cast_int(वर्ण*label, long width,
                            unsigned argc, संरचना symb_s*argv)
{
      vvp_arith_cast_int*arith = नूतन vvp_arith_cast_int((unsigned) width);

      vvp_net_t* ptr = नूतन vvp_net_t;
      ptr->fun = arith;

      define_functor_चिन्ह(label, ptr);
      मुक्त(label);

      प्रतिज्ञा(argc == 1);
      inputs_connect(ptr, argc, argv);
      मुक्त(argv);
}

शून्य compile_arith_cast_vec2(वर्ण*label, long width,
			     unsigned argc, संरचना symb_s*argv)
{
      vvp_arith_cast_vec2*arith = नूतन vvp_arith_cast_vec2((unsigned) width);

      vvp_net_t* ptr = नूतन vvp_net_t;
      ptr->fun = arith;

      define_functor_चिन्ह(label, ptr);
      मुक्त(label);

      प्रतिज्ञा(argc == 1);
      inputs_connect(ptr, argc, argv);
      मुक्त(argv);
}

शून्य compile_arith_cast_real(वर्ण*label, तार्किक signed_flag,
                             unsigned argc, संरचना symb_s*argv)
{
      vvp_arith_cast_real*arith = नूतन vvp_arith_cast_real(signed_flag);

      vvp_net_t* ptr = नूतन vvp_net_t;
      ptr->fun = arith;

      define_functor_चिन्ह(label, ptr);
      मुक्त(label);

      प्रतिज्ञा(argc == 1);
      inputs_connect(ptr, argc, argv);
      मुक्त(argv);
}

शून्य compile_arith_abs(वर्ण*label, unsigned argc, संरचना symb_s*argv)
{
      vvp_arith_abs*arith = नूतन vvp_arith_abs;

      vvp_net_t* ptr = नूतन vvp_net_t;
      ptr->fun = arith;

      define_functor_चिन्ह(label, ptr);
      मुक्त(label);

      प्रतिज्ञा(argc == 1);
      inputs_connect(ptr, argc, argv);
      मुक्त(argv);
}

शून्य compile_arith_div(वर्ण*label, long wid, तार्किक signed_flag,
		       unsigned argc, संरचना symb_s*argv)
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

शून्य compile_arith_div_r(वर्ण*label, unsigned argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s; .arith/divr has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_arith_div_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_arith_mod(वर्ण*label, long wid, तार्किक signed_flag,
		       unsigned argc, संरचना symb_s*argv)
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

शून्य compile_arith_mod_r(वर्ण*label, unsigned argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .arith/mod.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_arith_mod_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_arith_mult(वर्ण*label, long wid,
			unsigned argc, संरचना symb_s*argv)
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

शून्य compile_arith_mult_r(वर्ण*label, unsigned argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .arith/mult.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_arith_mult_real;
      make_arith(arith, label, argc, argv);
}


शून्य compile_arith_pow(वर्ण*label, long wid, तार्किक signed_flag,
		       unsigned argc, संरचना symb_s*argv)
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

शून्य compile_arith_pow_r(वर्ण*label, unsigned argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .arith/pow.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_arith_pow_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_arith_sub(वर्ण*label, long wid, unsigned argc, संरचना symb_s*argv)
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

शून्य compile_arith_sub_r(वर्ण*label, unsigned argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s; .arith/sub.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_arith_sub_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_arith_sum(वर्ण*label, long wid, unsigned argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      यदि (argc != 2) {
	    fprintf(stderr, "%s .arith/sum has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_ *arith = नूतन vvp_arith_sum(wid);
      make_arith(arith, label, argc, argv);
}

शून्य compile_arith_sum_r(वर्ण*label, unsigned argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .arith/sum.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_arith_sum_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_eeq(वर्ण*label, long wid,
		     unsigned argc, संरचना symb_s*argv)
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

शून्य compile_cmp_nee(वर्ण*label, long wid,
		     unsigned argc, संरचना symb_s*argv)
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

शून्य compile_cmp_eq(वर्ण*label, long wid, unsigned argc, संरचना symb_s*argv)
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

शून्य compile_cmp_eqx(वर्ण*label, long wid, unsigned argc, संरचना symb_s*argv)
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

शून्य compile_cmp_eqz(वर्ण*label, long wid, unsigned argc, संरचना symb_s*argv)
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

शून्य compile_cmp_eq_r(वर्ण*label, unsigned argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_cmp_eq_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_ne(वर्ण*label, long wid, unsigned argc, संरचना symb_s*argv)
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

शून्य compile_cmp_ne_r(वर्ण*label, unsigned argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_cmp_ne_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_ge(वर्ण*label, long wid, तार्किक signed_flag,
		    unsigned argc, संरचना symb_s*argv)
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

शून्य compile_cmp_ge_r(वर्ण*label, unsigned argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_cmp_ge_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_gt(वर्ण*label, long wid, तार्किक signed_flag,
		    unsigned argc, संरचना symb_s*argv)
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

शून्य compile_cmp_gt_r(वर्ण*label, unsigned argc, संरचना symb_s*argv)
{
      यदि (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    प्रत्यागम;
      }

      vvp_arith_real_ *arith = नूतन vvp_cmp_gt_real;
      make_arith(arith, label, argc, argv);
}

शून्य compile_cmp_weq(वर्ण*label, long wid,
		     unsigned argc, संरचना symb_s*argv)
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

शून्य compile_cmp_wne(वर्ण*label, long wid,
		     unsigned argc, संरचना symb_s*argv)
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


शून्य compile_delay(वर्ण*label, unsigned width,
                   vvp_delay_t*delay, संरचना symb_s arg)
{
      vvp_net_t*net = नूतन vvp_net_t;
      vvp_fun_delay*obj = नूतन vvp_fun_delay(net, width, *delay);
      net->fun = obj;

      delete delay;

      आदान_connect(net, 0, arg.text);

      define_functor_चिन्ह(label, net);
      मुक्त(label);
}

शून्य compile_delay(वर्ण*label, unsigned width,
                   unsigned argc, संरचना symb_s*argv, तार्किक ignore_decay)
{
      vvp_delay_t stub (0, 0, 0);
      यदि (ignore_decay) stub.set_ignore_decay();
      vvp_net_t*net = नूतन vvp_net_t;
      vvp_fun_delay*obj = नूतन vvp_fun_delay(net, width, stub);
      net->fun = obj;

      inputs_connect(net, argc, argv);
      मुक्त(argv);

      define_functor_चिन्ह(label, net);
      मुक्त(label);
}

/*
 * Extend nodes.
 */
शून्य compile_extend_signed(वर्ण*label, long wid, संरचना symb_s arg)
{
      प्रतिज्ञा(wid >= 0);

      vvp_fun_extend_signed*fun = नूतन vvp_fun_extend_signed(wid);
      vvp_net_t*ptr = नूतन vvp_net_t;
      ptr->fun = fun;

      define_functor_चिन्ह(label, ptr);
      मुक्त(label);

      आदान_connect(ptr, 0, arg.text);
}

संरचना __vpiModPath* compile_modpath(वर्ण*label, unsigned width,
                                     संरचना symb_s drv, संरचना symb_s dest)
{
      vvp_net_t*net = नूतन vvp_net_t;
      vvp_fun_modpath*obj = नूतन vvp_fun_modpath(net, width);
      net->fun = obj;

      आदान_connect(net, 0, drv.text);

      define_functor_चिन्ह(label, net);

      __vpiModPath*modpath = vpip_make_modpath(net);

      compile_vpi_lookup(&modpath->path_term_out.अभिव्यक्ती, dest.text);

      मुक्त(label);

      modpath->modpath = obj;
      प्रत्यागम modpath;
}

स्थिर संरचना __vpiModPathSrc*make_modpath_src(संरचना __vpiModPath*path,
					       वर्ण edge_c,
					       ध्रुव संरचना symb_s&src,
					       संरचना numbv_s&vals,
					       तार्किक ifnone)
{
      vvp_fun_modpath*dst = path->modpath;

      vvp_time64_t use_delay[12];
      प्रतिज्ञा(vals.cnt == 12);
      प्रति (unsigned idx = 0 ; idx < vals.cnt ;  idx += 1) {
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
#endif
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
      संरचना __vpiModPathSrc* srcobj = vpip_make_modpath_src(path, net) ;
      vpip_attach_to_current_scope(srcobj);
      net->fun = obj;

	/* Save the vpiEdge directory into the input path term. */
      srcobj->path_term_in.edge = vpi_edge;
      आदान_connect(net, 0, src.text);
      dst->add_modpath_src(obj, ifnone);

      प्रत्यागम srcobj;
}

शून्य compile_modpath_src(संरचना __vpiModPath*dst, वर्ण edge_c,
			 ध्रुव संरचना symb_s&src,
			 संरचना numbv_s&vals,
			 ध्रुव संरचना symb_s&condit_src,
			 ध्रुव संरचना symb_s&path_term_in)
{
      संरचना __vpiModPathSrc*obj =
	    make_modpath_src(dst, edge_c, src, vals, असत्य);
      आदान_connect(obj->net, 1, condit_src.text);
      compile_vpi_lookup(&obj->path_term_in.अभिव्यक्ती, path_term_in.text);
}

शून्य compile_modpath_src(संरचना __vpiModPath*dst, वर्ण edge_c,
			 ध्रुव संरचना symb_s&src,
			 संरचना numbv_s&vals,
			 पूर्णाङ्क condit_src,
			 ध्रुव संरचना symb_s&path_term_in,
			 तार्किक ifnone)
{
      प्रतिज्ञा(condit_src == 0);
      संरचना __vpiModPathSrc*obj =
	    make_modpath_src(dst, edge_c, src, vals, ifnone);
      compile_vpi_lookup(&obj->path_term_in.अभिव्यक्ती, path_term_in.text);
}

/*
 * A .shift/l statement creates an array of functors for the
 * width. The 0 input is the data vector to be shifted and the 1 input
 * is the amount of the shift. An unconnected shift amount is set to 0.
 */
शून्य compile_shiftl(वर्ण*label, long wid, unsigned argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      vvp_arith_ *arith = नूतन vvp_shiftl(wid);
      make_arith(arith, label, argc, argv);
}

शून्य compile_shiftr(वर्ण*label, long wid, तार्किक signed_flag,
		    unsigned argc, संरचना symb_s*argv)
{
      प्रतिज्ञा( wid > 0 );

      vvp_arith_ *arith = नूतन vvp_shiftr(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

शून्य compile_resolver(वर्ण*label, वर्ण*type, unsigned argc, संरचना symb_s*argv)
{
      vvp_net_t*net = नूतन vvp_net_t;

      resolv_core*core = 0;

      यदि (strcmp(type,"tri") == 0) {
	    core = नूतन resolv_tri(argc, net, vvp_scalar_t(BIT4_Z, 0,0));

      } अन्यथा यदि (strcmp(type,"tri0") == 0) {
	    core = नूतन resolv_tri(argc, net, vvp_scalar_t(BIT4_0, 5,5));

      } अन्यथा यदि (strcmp(type,"tri1") == 0) {
	    core = नूतन resolv_tri(argc, net, vvp_scalar_t(BIT4_1, 5,5));

      } अन्यथा यदि (strcmp(type,"triand") == 0) {
	    core = नूतन resolv_triand(argc, net);

      } अन्यथा यदि (strcmp(type,"trior") == 0) {
	    core = नूतन resolv_trior(argc, net);

      } अन्यथा {
	    fprintf(stderr, "invalid resolver type: %s\n", type);
	    compile_errors += 1;
            delete net;
      }

      यदि (core) {
	    net->fun = core;
	    define_functor_चिन्ह(label, net);

            प्रति (unsigned base = 0 ;  base < argc ;  base += 4) {
	          unsigned nports = argc - base;
                  यदि (nports > 4)
                        nports = 4;

                  यदि (base > 0) {
                        net = नूतन vvp_net_t;
                        net->fun = नूतन resolv_extend(core, base);
                  }
                  inputs_connect(net, nports, argv+base);
            }
      }
      मुक्त(type);
      मुक्त(label);
      मुक्त(argv);
}

शून्य compile_udp_def(पूर्णाङ्क sequ, वर्ण *label, वर्ण *name,
		     unsigned nin, unsigned init, वर्ण **table)
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
	    u->compile_table(table);
      } अन्यथा {
	    vvp_udp_comb_s *u = नूतन vvp_udp_comb_s(label, name, nin);
	    u->compile_table(table);
      }
      मुक्त(label);
}

वर्ण **compile_udp_table(वर्ण **table, वर्ण *row)
{
  यदि (table)
    प्रतिज्ञा(strlen(*table)==strlen(row));

  वर्ण **tt;
  प्रति (tt = table; tt && *tt; tt++) { }
  पूर्णाङ्क n = (tt-table) + 2;

  table = static_cast<वर्ण**>(realloc(table, n*परिमाण(वर्ण*)));
  table[n-2] = row;
  table[n-1] = 0x0;

  प्रत्यागम table;
}


/*
 * The parser uses this function to compile and link an executable
 * opcode. I do this by looking up the opcode in the opcode_table. The
 * table gives the operand structure that is acceptable, so I can
 * process the operands here as well.
 */
शून्य compile_code(वर्ण*label, वर्ण*mnem, comp_operands_t opa)
{
	/* First, I can give the label a value that is the current
	   codespace pointer. Don't need the text of the label after
	   this is done. */
      यदि (label)
	    compile_codelabel(label);

	/* Lookup the opcode in the opcode table. */
      संरचना opcode_table_s*op = static_cast<संरचना opcode_table_s*>
                                            (bsearch(mnem, opcode_table, opcode_मोजणी,
                                                     परिमाण(संरचना opcode_table_s),
                                                     &opcode_compare));
      यदि (op == 0) {
	    yyerror("Invalid opcode");
	    compile_errors += 1;
	    प्रत्यागम;
      }

      प्रतिज्ञा(op);

	/* Build up the code from the information about the opcode and
	   the information from the compiler. */
      vvp_code_t code = codespace_allocate();
      code->opcode = op->opcode;

      यदि (op->argc != (opa? opa->argc : 0)) {
	    yyerror("operand count");
	    compile_errors += 1;
	    प्रत्यागम;
      }

	/* Pull the operands that the instruction expects from the
	   list that the parser supplied. */

      प्रति (unsigned idx = 0 ;  idx < op->argc ;  idx += 1) {

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

		पक्ष OA_CODE_PTR:
		पक्ष OA_CODE_PTR2:
		  यदि (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			भङ्ग;
		  }

		  प्रतिज्ञा(opa->argv[idx].symb.idx == 0);
		  code_label_lookup(code, opa->argv[idx].symb.text,
				    op->argt[idx]==OA_CODE_PTR2);
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

		पक्ष OA_संख्या:
		  यदि (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			भङ्ग;
		  }

		  code->संख्या = opa->argv[idx].numb;
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
      चिन्ह_मूल्य_t val;
      vvp_code_t ptr = codespace_next();

      val.ptr = ptr;
      sym_set_मूल्य(sym_codespace, label, val);

      मुक्त(label);
}

शून्य compile_file_ओळ(वर्ण*label, long file_idx, long lineno,
                       वर्ण*description)
{
      यदि (label) compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_code_t code = codespace_allocate();
      code->opcode = &of_FILE_ओळ;

	/* Create a vpiHandle that contains the information. */
      code->handle = vpip_build_file_ओळ(description, file_idx, lineno);
      प्रतिज्ञा(code->handle);

	/* Done with the lexor-allocated name string. */
      delete[] description;
}

शून्य compile_vpi_call(वर्ण*label, वर्ण*name,
                      तार्किक func_as_task_err, तार्किक func_as_task_warn,
                      long file_idx, long lineno,
                      unsigned argc, vpiHandle*argv,
		      unsigned vec4_चळत, unsigned real_चळत, unsigned string_चळत)
{
      यदि (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_code_t code = codespace_allocate();
      code->opcode = &of_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, 0, 0,
					 0, func_as_task_err, func_as_task_warn,
                                         argc, argv, vec4_चळत, real_चळत, string_चळत,
					 file_idx, lineno);
      यदि (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      delete[] name;
}

शून्य compile_vpi_func_call(वर्ण*label, वर्ण*name,
			   पूर्णाङ्क val_type, unsigned val_wid,
			   long file_idx, long lineno,
			   unsigned argc, vpiHandle*argv,
			   unsigned vec4_चळत,
			   unsigned real_चळत,
			   unsigned string_चळत)
{
      यदि (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_code_t code = codespace_allocate();
      code->opcode = &of_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, val_type, val_wid,
					 0, सत्य, असत्य,
                                         argc, argv, vec4_चळत, real_चळत, string_चळत,
					 file_idx, lineno);
      यदि (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      delete[] name;
}

/*
 * When the parser finds a thread statement, I create a new thread
 * with the start address referenced by the program symbol passed to
 * me.
 */
शून्य compile_thread(वर्ण*start_sym, वर्ण*flag)
{
      तार्किक ढकला_flag = असत्य;

      चिन्ह_मूल्य_t tmp = sym_get_मूल्य(sym_codespace, start_sym);
      vvp_code_t pc = reinterpret_cast<vvp_code_t>(tmp.ptr);
      यदि (pc == 0) {
	    yyerror("unresolved address");
	    प्रत्यागम;
      }

      यदि (flag && (strcmp(flag,"$push") == 0))
	    ढकला_flag = सत्य;

      vthread_t thr = vthread_new(pc, vpip_peek_current_scope());

      यदि (flag && (strcmp(flag,"$init") == 0))
	    schedule_init_vthread(thr);
      अन्यथा यदि (flag && (strcmp(flag,"$final") == 0))
	    schedule_final_vthread(thr);
      अन्यथा
	    schedule_vthread(thr, 0, ढकला_flag);

      मुक्त(start_sym);
      मुक्त(flag);
}

शून्य compile_param_logic(वर्ण*label, ध्रुव वर्ण*name, वर्ण*value, तार्किक signed_flag,
                         तार्किक local_flag,
                         long file_idx, long lineno)
{
      vvp_vector4_t value4 = c4string_to_vector4(value);
      vpiHandle obj = vpip_make_द्विमान_param(name, value4, signed_flag,
                                             local_flag, file_idx, lineno);
      compile_vpi_चिन्ह(label, obj);
      vpip_attach_to_current_scope(obj);

      मुक्त(label);
      मुक्त(value);
}

शून्य compile_param_string(वर्ण*label, ध्रुव वर्ण*name, वर्ण*value,
                          तार्किक local_flag,
                          long file_idx, long lineno)
{
	// name and value become owned by vpip_make_string_param
      vpiHandle obj = vpip_make_string_param(name, value, local_flag, file_idx, lineno);
      compile_vpi_चिन्ह(label, obj);
      vpip_attach_to_current_scope(obj);

      मुक्त(label);
}

शून्य compile_param_real(वर्ण*label, ध्रुव वर्ण*name, वर्ण*value,
                        तार्किक local_flag,
                        long file_idx, long lineno)
{
      द्विगुण dvalue = crstring_to_double(value);
      vpiHandle obj = vpip_make_real_param(name, dvalue, local_flag, file_idx, lineno);
      compile_vpi_चिन्ह(label, obj);
      vpip_attach_to_current_scope(obj);

      मुक्त(label);
      मुक्त(value);
}

शून्य compile_island(वर्ण*label, वर्ण*type)
{
      यदि (strcmp(type,"tran") == 0)
	    compile_island_tran(label);
      अन्यथा
	    प्रतिज्ञा(0);

      मुक्त(type);
}
