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

#నిర్వచితమైతే __MINGW32__
#అన్తర్భావ <windows.h>
#అయితే_ముగింపు

# include  "ivl_alloc.h"

గుర్తులేని compile_errors = 0;

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

గణన నిర్వాహ్యం_e {
	/* Place holder for unused operand */
      OA_NONE,
	/* The operand is a number, an immediate unsigned integer */
      OA_సంఖ్య,
	/* The operand is a pointer to an array. */
      OA_ARR_PTR,
	/* The operand is a thread bit index or short integer */
      OA_BIT1,
      OA_BIT2,
	/* The operand is a pointer to code space */
      OA_సంకేతం_PTR,
      OA_సంకేతం_PTR2,
	/* The operand is a variable or net pointer */
      OA_FUNC_PTR,
	/* The operand is a second functor pointer */
      OA_FUNC_PTR2,
	/* The operand is a VPI handle */
      OA_VPI_PTR,
	/* String */
      OA_STRING
};

నిర్మాణం opcode_పట్టిక_s {
      స్థిరరాశి అక్షరం*mnemonic;
      vvp_సంకేతం_fun opcode;

      గుర్తులేని argc;
      గణన నిర్వాహ్యం_e argt[నిర్వాహ్యం_MAX];
};

స్థిర స్థిరరాశి నిర్మాణం opcode_పట్టిక_s opcode_పట్టిక[] = {
      { "%abs/wr", యొక్క_ABS_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add",    యొక్క_కూడు,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%add/wr", యొక్క_కూడు_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%addi",   యొక్క_ADDI,   3,  {OA_BIT1,     OA_BIT2,     OA_సంఖ్య} },
      { "%alloc",  యొక్క_ALLOC,  1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%and",    యొక్క_AND,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%and/r",  యొక్క_ANDR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%assign/ar",యొక్క_కేటాయించు_AR,2,{OA_ARR_PTR,OA_BIT1,     OA_NONE} },
      { "%assign/ar/d",యొక్క_కేటాయించు_ARD,2,{OA_ARR_PTR,OA_BIT1,  OA_NONE} },
      { "%assign/ar/e",యొక్క_కేటాయించు_ARE,1,{OA_ARR_PTR,OA_NONE,  OA_NONE} },
      { "%assign/vec4",      యొక్క_కేటాయించు_VEC4,       2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/a/d",  యొక్క_కేటాయించు_VEC4_A_D,   3, {OA_ARR_PTR,  OA_BIT1, OA_BIT2} },
      { "%assign/vec4/a/e",  యొక్క_కేటాయించు_VEC4_A_E,   2, {OA_ARR_PTR,  OA_BIT1, OA_NONE} },
      { "%assign/vec4/d",    యొక్క_కేటాయించు_VEC4_D,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/vec4/e",    యొక్క_కేటాయించు_VEC4_E,      1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%assign/vec4/off/d",యొక్క_కేటాయించు_VEC4_OFF_D, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%assign/vec4/off/e",యొక్క_కేటాయించు_VEC4_OFF_E, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr",  యొక్క_కేటాయించు_WR, 2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/d",యొక్క_కేటాయించు_WRD,2,{OA_VPI_PTR, OA_BIT1, OA_NONE} },
      { "%assign/wr/e",యొక్క_కేటాయించు_WRE,1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%blend",     యొక్క_BLEND,     0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/str", యొక్క_BLEND_STR, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%blend/wr",  యొక్క_BLEND_WR,  0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%breakpoint", యొక్క_BREAKPOINT, 0,  {OA_NONE, OA_NONE, OA_NONE} },
      { "%callf/obj",       యొక్క_CALLF_OBJ,       2,{OA_సంకేతం_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/real",      యొక్క_CALLF_REAL,      2,{OA_సంకేతం_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/str",       యొక్క_CALLF_STR,       2,{OA_సంకేతం_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/vec4",      యొక్క_CALLF_VEC4,      2,{OA_సంకేతం_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%callf/void",      యొక్క_CALLF_VOID,      2,{OA_సంకేతం_PTR2,OA_VPI_PTR, OA_NONE} },
      { "%cassign/link",    యొక్క_CASSIGN_LINK,    2,{OA_FUNC_PTR,OA_FUNC_PTR2,OA_NONE} },
      { "%cassign/vec4",    యొక్క_CASSIGN_VEC4,    1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cassign/vec4/off",యొక్క_CASSIGN_VEC4_OFF,2,{OA_FUNC_PTR,OA_BIT1,     OA_NONE} },
      { "%cassign/wr",  యొక్క_CASSIGN_WR,  1,{OA_FUNC_PTR,OA_NONE,     OA_NONE} },
      { "%cast/vec2/dar", యొక్క_CAST_VEC2_DAR, 1,  {OA_సంఖ్య,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/dar", యొక్క_CAST_VEC4_DAR, 1,  {OA_సంఖ్య,   OA_NONE,     OA_NONE} },
      { "%cast/vec4/str", యొక్క_CAST_VEC4_STR, 1,  {OA_సంఖ్య,   OA_NONE,     OA_NONE} },
      { "%cast2",   యొక్క_CAST2,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/e",   యొక్క_CMPE,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/ne",  యొక్క_CMPNE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/s",   యొక్క_CMPS,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/str", యొక్క_CMPSTR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/u",   యొక్క_CMPU,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/we",  యొక్క_CMPWE,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wne", యొక్క_CMPWNE, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/wr",  యొక్క_CMPWR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/x",   యొక్క_CMPX,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmp/z",   యొక్క_CMPZ,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%cmpi/e",  యొక్క_CMPIE,  3,  {OA_BIT1,     OA_BIT2,     OA_సంఖ్య} },
      { "%cmpi/ne", యొక్క_CMPINE, 3,  {OA_BIT1,     OA_BIT2,     OA_సంఖ్య} },
      { "%cmpi/s",  యొక్క_CMPIS,  3,  {OA_BIT1,     OA_BIT2,     OA_సంఖ్య} },
      { "%cmpi/u",  యొక్క_CMPIU,  3,  {OA_BIT1,     OA_BIT2,     OA_సంఖ్య} },
      { "%cmpix/ltu", యొక్క_CMPIX_LTU, 2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%cmpix/slt0", యొక్క_CMPIX_SLT0, 1, {OA_సంఖ్య, OA_NONE, OA_NONE} },
      { "%concat/str",  యొక్క_CONCAT_STR,  0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concat/vec4", యొక్క_CONCAT_VEC4, 0,{OA_NONE,  OA_NONE,  OA_NONE} },
      { "%concati/str", యొక్క_CONCATI_STR, 1,{OA_STRING,OA_NONE,  OA_NONE} },
      { "%concati/vec4",యొక్క_CONCATI_VEC4,3,{OA_BIT1,  OA_BIT2,  OA_సంఖ్య} },
      { "%cvt/rv",   యొక్క_CVT_RV,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/rv/s", యొక్క_CVT_RV_S,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%cvt/sr", యొక్క_CVT_SR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/ur", యొక్క_CVT_UR, 1,  {OA_BIT1,     OA_NONE,     OA_NONE} },
      { "%cvt/vr", యొక్క_CVT_VR, 1,  {OA_సంఖ్య,   OA_NONE,     OA_NONE} },
      { "%deassign",యొక్క_DEASSIGN,3,{OA_FUNC_PTR, OA_BIT1,     OA_BIT2} },
      { "%deassign/wr",యొక్క_DEASSIGN_WR,1,{OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%debug/thr",  యొక్క_DEBUG_THR,  1,{OA_STRING,   OA_NONE,     OA_NONE} },
      { "%delay",  యొక్క_DELAY,  2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%delayx", యొక్క_DELAYX, 1,  {OA_సంఖ్య,   OA_NONE,     OA_NONE} },
      { "%delete/elem",యొక్క_DELETE_ELEM,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%delete/obj",యొక్క_DELETE_OBJ,1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%delete/prop/elem", యొక్క_DELETE_PROP_ELEM, 1, {OA_సంఖ్య, OA_NONE, OA_NONE} },
      { "%delete/prop/obj",  యొక్క_DELETE_PROP_OBJ,  1, {OA_సంఖ్య, OA_NONE, OA_NONE} },
      { "%delete/tail",యొక్క_DELETE_తోక,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%disable",  యొక్క_DISABLE, 1, {OA_VPI_PTR,OA_NONE,     OA_NONE} },
      { "%disable/flow", యొక్క_DISABLE_FLOW, 1, {OA_VPI_PTR,OA_NONE, OA_NONE} },
      { "%disable/fork",యొక్క_DISABLE_FORK,0,{OA_NONE,OA_NONE,  OA_NONE} },
      { "%div",      యొక్క_DIV,     0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/s",    యొక్క_DIV_S,   0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%div/wr",   యొక్క_DIV_WR,  0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/obj",  యొక్క_DUP_OBJ, 0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/real", యొక్క_DUP_REAL,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%dup/vec4", యొక్క_DUP_VEC4,0, {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%end",    యొక్క_చివర,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl",  యొక్క_EVCTL,  2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/c",యొక్క_EVCTLC, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%evctl/i",యొక్క_EVCTLI, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%evctl/s",యొక్క_EVCTLS, 2,  {OA_FUNC_PTR, OA_BIT1,     OA_NONE} },
      { "%event",    యొక్క_EVENT,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%event/nb", యొక్క_EVENT_NB, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%flag_get/vec4", యొక్క_FLAG_GET_VEC4, 1, {OA_సంఖ్య, OA_NONE, OA_NONE} },
      { "%flag_inv",      యొక్క_FLAG_INV,      1, {OA_BIT1,   OA_NONE, OA_NONE} },
      { "%flag_mov",      యొక్క_FLAG_MOV,      2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_or",       యొక్క_FLAG_OR,       2, {OA_BIT1,   OA_BIT2, OA_NONE} },
      { "%flag_set/imm",  యొక్క_FLAG_సమితి_IMM,  2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%flag_set/vec4", యొక్క_FLAG_సమితి_VEC4, 1, {OA_సంఖ్య, OA_NONE, OA_NONE} },
      { "%force/link",    యొక్క_FORCE_LINK,2,{OA_FUNC_PTR, OA_FUNC_PTR2, OA_NONE} },
      { "%force/vec4",    యొక్క_FORCE_VEC4,    1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%force/vec4/off",యొక్క_FORCE_VEC4_OFF,2,{OA_FUNC_PTR, OA_BIT1,      OA_NONE} },
      { "%force/vec4/off/d",యొక్క_FORCE_VEC4_OFF_D,3,{OA_FUNC_PTR, OA_BIT1,  OA_BIT2} },
      { "%force/wr",      యొక్క_FORCE_WR,      1,{OA_FUNC_PTR, OA_NONE,      OA_NONE} },
      { "%fork",   యొక్క_FORK,   2,  {OA_సంకేతం_PTR2,OA_VPI_PTR,  OA_NONE} },
      { "%free",   యొక్క_FREE,   1,  {OA_VPI_PTR,  OA_NONE,     OA_NONE} },
      { "%inv",    యొక్క_INV,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%ix/add", యొక్క_IX_కూడు, 3,  {OA_సంఖ్య,   OA_BIT1,     OA_BIT2} },
      { "%ix/getv",యొక్క_IX_GETV,2,  {OA_BIT1,     OA_FUNC_PTR, OA_NONE} },
      { "%ix/getv/s",యొక్క_IX_GETV_S,2, {OA_BIT1,   OA_FUNC_PTR, OA_NONE} },
      { "%ix/load",యొక్క_IX_ఎక్కించు,3,  {OA_సంఖ్య,   OA_BIT1,     OA_BIT2} },
      { "%ix/mov", యొక్క_IX_MOV, 2,  {OA_BIT1,     OA_BIT2,     OA_NONE} },
      { "%ix/mul", యొక్క_IX_MUL, 3,  {OA_సంఖ్య,   OA_BIT1,     OA_BIT2} },
      { "%ix/sub", యొక్క_IX_SUB, 3,  {OA_సంఖ్య,   OA_BIT1,     OA_BIT2} },
      { "%ix/vec4",  యొక్క_IX_VEC4,  1,  {OA_సంఖ్య,   OA_NONE, OA_NONE} },
      { "%ix/vec4/s",యొక్క_IX_VEC4_S,1,  {OA_సంఖ్య,   OA_NONE, OA_NONE} },
      { "%jmp",    యొక్క_JMP,    1,  {OA_సంకేతం_PTR, OA_NONE,     OA_NONE} },
      { "%jmp/0",  యొక్క_JMP0,   2,  {OA_సంకేతం_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/0xz",యొక్క_JMP0_XZ, 2,  {OA_సంకేతం_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1",  యొక్క_JMP1,   2,  {OA_సంకేతం_PTR, OA_BIT1,     OA_NONE} },
      { "%jmp/1xz",యొక్క_JMP1_XZ, 2,  {OA_సంకేతం_PTR, OA_BIT1,     OA_NONE} },
      { "%join",   యొక్క_JOIN,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%join/detach",యొక్క_JOIN_DETACH,1,{OA_సంఖ్య,OA_NONE,  OA_NONE} },
      { "%load/ar",యొక్క_ఎక్కించు_AR,2,  {OA_ARR_PTR,  OA_BIT1,     OA_NONE} },
      { "%load/dar/r",  యొక్క_ఎక్కించు_DAR_R,    1, {OA_FUNC_PTR, OA_NONE, OA_NONE}},
      { "%load/dar/str",యొక్క_ఎక్కించు_DAR_STR,  1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/dar/vec4",యొక్క_ఎక్కించు_DAR_VEC4,1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%load/obj",   యొక్క_ఎక్కించు_OBJ,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/obja",  యొక్క_ఎక్కించు_OBJA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/prop/dar/vec4", యొక్క_ఎక్కించు_PROP_DAR_VEC4, 2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%load/real",  యొక్క_ఎక్కించు_REAL, 1,{OA_VPI_PTR, OA_NONE, OA_NONE} },
      { "%load/str",   యొక్క_ఎక్కించు_STR,  1,{OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%load/stra",  యొక్క_ఎక్కించు_STRA, 2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%load/vec4",  యొక్క_ఎక్కించు_VEC4, 1,{OA_FUNC_PTR,OA_NONE,  OA_NONE} },
      { "%load/vec4a", యొక్క_ఎక్కించు_VEC4_A,2,{OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%max/wr", యొక్క_MAX_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%min/wr", యొక్క_MIN_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod",    యొక్క_MOD,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/s",  యొక్క_MOD_S,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mod/wr", యొక్క_MOD_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul",    యొక్క_MUL,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%mul/wr", యొక్క_MUL_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%muli",   యొక్క_MULI,   3,  {OA_BIT1,     OA_BIT2,     OA_సంఖ్య} },
      { "%nand",   యొక్క_NAND,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nand/r", యొక్క_NANDR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%neg/wr", యొక్క_NEG_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%new/cobj",  యొక్క_NEW_COBJ,  1, {OA_VPI_PTR,OA_NONE,  OA_NONE} },
      { "%new/darray",యొక్క_NEW_DARRAY,2, {OA_BIT1,   OA_STRING,OA_NONE} },
      { "%noop",   యొక్క_NOOP,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor",    యొక్క_NOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%nor/r",  యొక్క_NORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%null",   యొక్క_NULL,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or",     యొక్క_OR,     0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%or/r",   యొక్క_ORR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%pad/s",  యొక్క_PAD_S,  1,  {OA_సంఖ్య,   OA_NONE,     OA_NONE} },
      { "%pad/u",  యొక్క_PAD_U,  1,  {OA_సంఖ్య,   OA_NONE,     OA_NONE} },
      { "%part/s", యొక్క_PART_S, 1,  {OA_సంఖ్య,   OA_NONE,     OA_NONE} },
      { "%part/u", యొక్క_PART_U, 1,  {OA_సంఖ్య,   OA_NONE,     OA_NONE} },
      { "%parti/s",యొక్క_PARTI_S,3,  {OA_సంఖ్య,   OA_BIT1,     OA_BIT2} },
      { "%parti/u",యొక్క_PARTI_U,3,  {OA_సంఖ్య,   OA_BIT1,     OA_BIT2} },
      { "%pop/obj", యొక్క_తీయి_OBJ, 2, {OA_BIT1,    OA_BIT2,     OA_NONE} },
      { "%pop/real",యొక్క_తీయి_REAL,1, {OA_సంఖ్య,  OA_NONE,     OA_NONE} },
      { "%pop/str", యొక్క_తీయి_STR, 1, {OA_సంఖ్య,  OA_NONE,     OA_NONE} },
      { "%pop/vec4",యొక్క_తీయి_VEC4,1, {OA_సంఖ్య,  OA_NONE,     OA_NONE} },
      { "%pow",     యొక్క_POW,     0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/s",   యొక్క_POW_S,   0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%pow/wr",  యొక్క_POW_WR,  0,  {OA_NONE,   OA_NONE,     OA_NONE} },
      { "%prop/obj",యొక్క_PROP_OBJ,2,  {OA_సంఖ్య,   OA_BIT1,     OA_NONE} },
      { "%prop/queue/size", యొక్క_PROP_వరుస_కొలత,  1, {OA_సంఖ్య, OA_NONE, OA_NONE} },
      { "%prop/r",  యొక్క_PROP_R,  1,  {OA_సంఖ్య,   OA_NONE,     OA_NONE} },
      { "%prop/str",యొక్క_PROP_STR,1,  {OA_సంఖ్య,   OA_NONE,     OA_NONE} },
      { "%prop/v",  యొక్క_PROP_V,  1,  {OA_సంఖ్య,   OA_NONE,     OA_NONE} },
      { "%push/ix/vec4", యొక్క_నెట్టు_IX_VEC4, 3, {OA_సంఖ్య, OA_BIT1, OA_BIT2} },
      { "%pushi/real",యొక్క_PUSHI_REAL,2,{OA_BIT1,   OA_BIT2,   OA_NONE} },
      { "%pushi/str", యొక్క_PUSHI_STR, 1,{OA_STRING, OA_NONE,   OA_NONE} },
      { "%pushi/vec4",యొక్క_PUSHI_VEC4,3,{OA_BIT1,   OA_BIT2,   OA_సంఖ్య} },
      { "%pushv/str", యొక్క_PUSHV_STR, 0,{OA_NONE,   OA_NONE,   OA_NONE} },
      { "%putc/str/vec4",యొక్క_PUTC_STR_VEC4,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/prop/r",   యొక్క_QINSERT_PROP_R,   2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/str", యొక్క_QINSERT_PROP_STR, 2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%qinsert/prop/v",   యొక్క_QINSERT_PROP_V,   3, {OA_సంఖ్య, OA_BIT1, OA_BIT2} },
      { "%qinsert/real",యొక్క_QINSERT_REAL,2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/str", యొక్క_QINSERT_STR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qinsert/v",   యొక్క_QINSERT_V,   3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%qpop/b/real",యొక్క_QPOP_B_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/str", యొక్క_QPOP_B_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/b/v",   యొక్క_QPOP_B_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/f/real",యొక్క_QPOP_F_REAL,1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/str", యొక్క_QPOP_F_STR, 1,{OA_FUNC_PTR,OA_NONE,OA_NONE} },
      { "%qpop/f/v",   యొక్క_QPOP_F_V,   2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%qpop/prop/b/r",   యొక్క_QPOP_PROP_B_REAL, 1, {OA_సంఖ్య, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/str", యొక్క_QPOP_PROP_B_STR, 1, {OA_సంఖ్య, OA_NONE, OA_NONE} },
      { "%qpop/prop/b/v",   యొక్క_QPOP_PROP_B_V,    2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%qpop/prop/f/r",   యొక్క_QPOP_PROP_F_REAL, 1, {OA_సంఖ్య, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/str", యొక్క_QPOP_PROP_F_STR, 1, {OA_సంఖ్య, OA_NONE, OA_NONE} },
      { "%qpop/prop/f/v",   యొక్క_QPOP_PROP_F_V,    2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%queue/append_word/v", యొక్క_వరుస_APPEND_మాట_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/find/index/prop/v", యొక్క_వరుస_కనుగొను_సూచీ_PROP_V, 2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%queue/find/index/v", యొక్క_వరుస_కనుగొను_సూచీ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find/prop/v", యొక్క_వరుస_కనుగొను_PROP_V, 2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%queue/find/v", యొక్క_వరుస_కనుగొను_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/prop/v", యొక్క_వరుస_కనుగొను_మొదటి_సూచీ_PROP_V, 2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%queue/find_first/index/v", యొక్క_వరుస_కనుగొను_మొదటి_సూచీ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_first/prop/v", యొక్క_వరుస_కనుగొను_మొదటి_PROP_V, 2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%queue/find_first/v", యొక్క_వరుస_కనుగొను_మొదటి_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/prop/v", యొక్క_వరుస_కనుగొను_చివరి_సూచీ_PROP_V, 2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%queue/find_last/index/v", యొక్క_వరుస_కనుగొను_చివరి_సూచీ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/find_last/prop/v", యొక్క_వరుస_కనుగొను_చివరి_PROP_V, 2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%queue/find_last/v", యొక్క_వరుస_కనుగొను_చివరి_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/max/obj/v", యొక్క_వరుస_MAX_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/max/prop/v", యొక్క_వరుస_MAX_PROP_V, 2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%queue/max/v", యొక్క_వరుస_MAX_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/min/obj/v", యొక్క_వరుస_MIN_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/min/prop/v", యొక్క_వరుస_MIN_PROP_V, 2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%queue/min/v", యొక్క_వరుస_MIN_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/new_empty/v", యొక్క_వరుస_NEW_ఖాళీలు_V, 0, {OA_NONE, OA_NONE, OA_NONE} },
      { "%queue/size/v", యొక్క_వరుస_కొలత_V, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%queue/unique/index/obj/v", యొక్క_వరుస_UNIQUE_సూచీ_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/index/prop/v", యొక్క_వరుస_UNIQUE_సూచీ_PROP_V, 2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%queue/unique/index/v", యొక్క_వరుస_UNIQUE_సూచీ_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/unique/obj/v", యొక్క_వరుస_UNIQUE_OBJ_V, 1, {OA_BIT1, OA_NONE, OA_NONE} },
      { "%queue/unique/prop/v", యొక్క_వరుస_UNIQUE_PROP_V, 2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%queue/unique/v", యొక్క_వరుస_UNIQUE_V, 2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%queue/word/prop/v", యొక్క_వరుస_మాట_PROP_V, 3, {OA_సంఖ్య, OA_BIT1, OA_BIT2} },
      { "%queue/word/v", యొక్క_వరుస_మాట_V, 3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%release/net",యొక్క_RELEASE_NET,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/reg",యొక్క_RELEASE_REG,3,{OA_FUNC_PTR,OA_BIT1,OA_BIT2} },
      { "%release/wr", యొక్క_RELEASE_WR, 2,{OA_FUNC_PTR,OA_BIT1,OA_NONE} },
      { "%replicate", యొక్క_REPLICATE,   1,{OA_సంఖ్య,  OA_NONE,OA_NONE} },
      { "%ret/real",  యొక్క_RET_REAL,    1,{OA_సంఖ్య,  OA_NONE,OA_NONE} },
      { "%ret/str",   యొక్క_RET_STR,     1,{OA_సంఖ్య,  OA_NONE,OA_NONE} },
      { "%ret/vec4",  యొక్క_RET_VEC4,    3,{OA_సంఖ్య,  OA_BIT1,OA_BIT2} },
      { "%retload/real",యొక్క_RETLOAD_REAL,1,{OA_సంఖ్య,  OA_NONE,OA_NONE} },
      { "%retload/str", యొక్క_RETLOAD_STR, 1,{OA_సంఖ్య,  OA_NONE,OA_NONE} },
      { "%retload/vec4",యొక్క_RETLOAD_VEC4,1,{OA_సంఖ్య,  OA_NONE,OA_NONE} },
      { "%reverse/obj", యొక్క_తిరగేయి_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%reverse/prop/obj", యొక్క_తిరగేయి_PROP_OBJ, 1, {OA_సంఖ్య, OA_NONE, OA_NONE} },
      { "%rsort/obj", యొక్క_RSORT_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%rsort/prop/obj", యొక్క_RSORT_PROP_OBJ, 1, {OA_సంఖ్య, OA_NONE, OA_NONE} },
      { "%scopy",  యొక్క_SCOPY,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%set/dar/obj/real",యొక్క_సమితి_DAR_OBJ_REAL,1,{OA_సంఖ్య,OA_NONE,OA_NONE} },
      { "%set/dar/obj/str", యొక్క_సమితి_DAR_OBJ_STR, 1,{OA_సంఖ్య,OA_NONE,OA_NONE} },
      { "%set/dar/obj/vec4",యొక్క_సమితి_DAR_OBJ_VEC4,1,{OA_సంఖ్య,OA_NONE,OA_NONE} },
      { "%shiftl",   యొక్క_SHIFTL,   1, {OA_సంఖ్య, OA_NONE,   OA_NONE} },
      { "%shiftr",   యొక్క_SHIFTR,   1, {OA_సంఖ్య, OA_NONE,   OA_NONE} },
      { "%shiftr/s", యొక్క_SHIFTR_S, 1, {OA_సంఖ్య, OA_NONE,   OA_NONE} },
      { "%shuffle/obj", యొక్క_SHUFFLE_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%shuffle/prop/obj", యొక్క_SHUFFLE_PROP_OBJ, 1, {OA_సంఖ్య, OA_NONE, OA_NONE} },
      { "%sort/obj", యొక్క_క్రమీకరించు_OBJ, 1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%sort/prop/obj", యొక్క_క్రమీకరించు_PROP_OBJ, 1, {OA_సంఖ్య, OA_NONE, OA_NONE} },
      { "%split/vec4",    యొక్క_విభజించు_VEC4,    1,{OA_సంఖ్య,   OA_NONE, OA_NONE} },
      { "%store/dar/r",   యొక్క_STORE_DAR_R,   1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/str", యొక్క_STORE_DAR_STR, 1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/dar/vec4",యొక్క_STORE_DAR_VEC4,1,{OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%store/obj",   యొక్క_STORE_OBJ,   1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/obja",  యొక్క_STORE_OBJA,  2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/prop/obj",యొక్క_STORE_PROP_OBJ,2, {OA_సంఖ్య,  OA_BIT1, OA_NONE} },
      { "%store/prop/qb/r",   యొక్క_STORE_PROP_QB_R,   2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/str", యొక్క_STORE_PROP_QB_STR, 2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%store/prop/qb/v",   యొక్క_STORE_PROP_QB_V,   3, {OA_సంఖ్య, OA_BIT1, OA_BIT2} },
      { "%store/prop/qf/r",   యొక్క_STORE_PROP_QF_R,   2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/str", యొక్క_STORE_PROP_QF_STR, 2, {OA_సంఖ్య, OA_BIT1, OA_NONE} },
      { "%store/prop/qf/v",   యొక్క_STORE_PROP_QF_V,   3, {OA_సంఖ్య, OA_BIT1, OA_BIT2} },
      { "%store/prop/r",  యొక్క_STORE_PROP_R,  1, {OA_సంఖ్య,  OA_NONE, OA_NONE} },
      { "%store/prop/str",యొక్క_STORE_PROP_STR,1, {OA_సంఖ్య,  OA_NONE, OA_NONE} },
      { "%store/prop/v",  యొక్క_STORE_PROP_V,  2, {OA_సంఖ్య,  OA_BIT1, OA_NONE} },
      { "%store/qb/r",   యొక్క_STORE_QB_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/str", యొక్క_STORE_QB_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qb/v",   యొక్క_STORE_QB_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qdar/r",  యొక్క_STORE_QDAR_R,  2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/str",యొక్క_STORE_QDAR_STR,2,{OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qdar/v",  యొక్క_STORE_QDAR_V,  3,{OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qf/r",   యొక్క_STORE_QF_R,    2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/str", యొక్క_STORE_QF_STR,  2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%store/qf/v",   యొక్క_STORE_QF_V,    3, {OA_FUNC_PTR, OA_BIT1, OA_BIT2} },
      { "%store/qobj/r",  యొక్క_STORE_QOBJ_R,  2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/str",యొక్క_STORE_QOBJ_STR,2, {OA_FUNC_PTR,OA_BIT1, OA_NONE} },
      { "%store/qobj/v",  యొక్క_STORE_QOBJ_V,  3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/real",    యొక్క_STORE_REAL,    1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/reala",   యొక్క_STORE_REALA,   2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/str",     యొక్క_STORE_STR,     1, {OA_FUNC_PTR,OA_NONE, OA_NONE} },
      { "%store/stra",    యొక్క_STORE_STRA,    2, {OA_ARR_PTR, OA_BIT1, OA_NONE} },
      { "%store/vec4",    యొక్క_STORE_VEC4,    3, {OA_FUNC_PTR,OA_BIT1, OA_BIT2} },
      { "%store/vec4a",   యొక్క_STORE_VEC4_A,   3, {OA_ARR_PTR, OA_BIT1, OA_BIT2} },
      { "%sub",    యొక్క_SUB,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%sub/wr", యొక్క_SUB_WR, 0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%subi",   యొక్క_SUBI,   3,  {OA_BIT1,     OA_BIT2,     OA_సంఖ్య} },
      { "%substr",     యొక్క_SUBSTR,     2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%substr/vec4",యొక్క_SUBSTR_VEC4,2,{OA_BIT1,    OA_BIT2, OA_NONE} },
      { "%test_nul",     యొక్క_TEST_NUL,     1,{OA_FUNC_PTR,OA_NONE,    OA_NONE} },
      { "%test_nul/a",   యొక్క_TEST_NUL_A,   2,{OA_ARR_PTR, OA_BIT1,    OA_NONE} },
      { "%test_nul/obj", యొక్క_TEST_NUL_OBJ, 0,{OA_NONE,    OA_NONE,    OA_NONE} },
      { "%test_nul/prop",యొక్క_TEST_NUL_PROP,2,{OA_సంఖ్య,  OA_BIT1,    OA_NONE} },
      { "%uparr/and/v",          యొక్క_UPARR_AND_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/or/v",           యొక్క_UPARR_OR_V,           2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/product/obj/v",  యొక్క_UPARR_లబ్ధం_OBJ_V,  1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/product/prop/v", యొక్క_UPARR_లబ్ధం_PROP_V, 2, {OA_సంఖ్య,   OA_BIT1, OA_NONE} },
      { "%uparr/product/r",      యొక్క_UPARR_లబ్ధం_REAL,   1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/product/v",      యొక్క_UPARR_లబ్ధం_V,      2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/sum/obj/v",      యొక్క_UPARR_కూడిక_OBJ_V,      1, {OA_BIT1,     OA_NONE, OA_NONE} },
      { "%uparr/sum/prop/v",     యొక్క_UPARR_కూడిక_PROP_V,     2, {OA_సంఖ్య,   OA_BIT1, OA_NONE} },
      { "%uparr/sum/r",          యొక్క_UPARR_కూడిక_REAL,       1, {OA_FUNC_PTR, OA_NONE, OA_NONE} },
      { "%uparr/sum/v",          యొక్క_UPARR_కూడిక_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%uparr/xor/v",          యొక్క_UPARR_XOR_V,          2, {OA_FUNC_PTR, OA_BIT1, OA_NONE} },
      { "%wait",   యొక్క_WAIT,   1,  {OA_FUNC_PTR, OA_NONE,     OA_NONE} },
      { "%wait/fork",యొక్క_WAIT_FORK,0,{OA_NONE,   OA_NONE,     OA_NONE} },
      { "%xnor",   యొక్క_XNOR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xnor/r", యొక్క_XNORR,  0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor",    యొక్క_XOR,    0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { "%xor/r",  యొక్క_XORR,   0,  {OA_NONE,     OA_NONE,     OA_NONE} },
      { 0, యొక్క_NOOP, 0, {OA_NONE, OA_NONE, OA_NONE} }
};

స్థిర స్థిరరాశి గుర్తులేని opcode_లెక్క =
                    పరిమాణం(opcode_పట్టిక)/పరిమాణం(*opcode_పట్టిక) - 1;

స్థిర పూర్ణాంకం opcode_compare(స్థిరరాశి శూన్య*k, స్థిరరాశి శూన్య*r)
{
      స్థిరరాశి అక్షరం*kp = static_cast<స్థిరరాశి అక్షరం*>(k);
      స్థిరరాశి నిర్మాణం opcode_పట్టిక_s*rp = static_cast<స్థిరరాశి నిర్మాణం opcode_పట్టిక_s*>(r);
      తిరిగి strcmp(kp, rp->mnemonic);
}

/*
 * Keep a symbol table of addresses within code space. Labels on
 * executable opcodes are mapped to their address here.
 */
స్థిర చిహ్నం_పట్టిక_t sym_codespace = 0;

/*
 * Keep a symbol table of functors mentioned in the source. This table
 * is used to resolve references as they come.
 */
స్థిర చిహ్నం_పట్టిక_t sym_functors = 0;

/*
 * VPI objects are indexed during compile time so that they can be
 * linked together as they are created. This symbol table matches
 * labels to vpiHandles.
 */
స్థిర చిహ్నం_పట్టిక_t sym_vpi = 0;


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

శూన్య define_functor_చిహ్నం(స్థిరరాశి అక్షరం*label, vvp_net_t*net)
{
      చిహ్నం_విలువ_t val;
      val.net = net;
      sym_సమితి_విలువ(sym_functors, label, val);
}

స్థిర vvp_net_t*lookup_functor_చిహ్నం(స్థిరరాశి అక్షరం*label)
{
      ప్రతిజ్ఞా(sym_functors);
      చిహ్నం_విలువ_t val = sym_get_విలువ(sym_functors, label);
      తిరిగి val.net;
}

vpiHandle vvp_lookup_handle(స్థిరరాశి అక్షరం*label)
{
      చిహ్నం_విలువ_t val = sym_get_విలువ(sym_vpi, label);
      ఒకవేళ (val.ptr) తిరిగి (vpiHandle) val.ptr;
      తిరిగి 0;
}

vvp_net_t* vvp_net_lookup(స్థిరరాశి అక్షరం*label)
{
      స్థిర తార్కిక t0_trigger_generated = అబద్ధం;

        /* First, look to see if the symbol is a vpi object of some
	   sort. If it is, then get the vvp_ipoint_t pointer out of
	   the vpiHandle. */
      చిహ్నం_విలువ_t val = sym_get_విలువ(sym_vpi, label);
      ఒకవేళ (val.ptr) {
	    vpiHandle vpi = static_cast<vpiHandle>(val.ptr);
	    ఎంపిక (vpi->get_type_సంకేతం()) {
		స్థితి vpiNet:
		స్థితి vpiReg:
		స్థితి vpi_బిట్టు_Var:
		స్థితి vpiByteVar:
		స్థితి vpiShortIntVar:
		స్థితి vpiIntVar:
		స్థితి vpiLongIntVar:
		స్థితి vpiIntegerVar: {
		      స్థిరరాశి __vpiSignal*sig = dynamic_cast<__vpiSignal*>(vpi);
		      తిరిగి sig->కణుపు;
		}

		స్థితి vpiRealVar: {
		      స్థిరరాశి __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(vpi);
		      తిరిగి sig->net;
		}

		స్థితి vpiStringVar:
		స్థితి vpiArrayVar:
		స్థితి vpiClassVar: {
		      స్థిరరాశి __vpiBaseVar*sig = dynamic_cast<__vpiBaseVar*>(vpi);
		      తిరిగి sig->get_net();
		}

		స్థితి vpiNamedEvent: {
		      స్థిరరాశి __vpiNamedEvent*tmp = dynamic_cast<__vpiNamedEvent*>(vpi);
		      తిరిగి tmp->funct;
		}

		అప్రమేయ:
		  fprintf(stderr, "Unsupported type %d.\n",
		          vpi->get_type_సంకేతం());
		  ప్రతిజ్ఞా(0);
	    }
      }


	/* Failing that, look for a general functor. */
      vvp_net_t*tmp = lookup_functor_చిహ్నం(label);

	// This is a special label used to create a T0 trigger for the
	// always_comb/latch processes.
     ఒకవేళ (! t0_trigger_generated && (strcmp(label, "E_0x0") == 0)) {
	      // This should never happen, but if it does then the E_0x0
	      // event generation may need to be explicitly generated in
	      // the compiler output instead of implicitly in this code.
	    ప్రతిజ్ఞా(! vpip_peek_current_scope()->is_automatic());
	    t0_trigger_generated = నిజం;
	      // Create a local event with no name for the T0 trigger
	    compile_named_event(strdup(label), strcpy(కొత్త అక్షరం [1],""), నిజం);
	    tmp = vvp_net_lookup(label);
	    ప్రతిజ్ఞా(tmp);
	      // Create a trigger for the T0 event.
	    vvp_net_ptr_t ptr (tmp, 0);
	    కాలపట్టిక_t0_trigger(ptr);
     }

      తిరిగి tmp;
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
స్థిర resolv_list_s*resolv_list = 0;

resolv_list_s::~resolv_list_s()
{
      విడుదల(label_);
}

శూన్య resolv_submit(resolv_list_s*cur)
{
      ఒకవేళ (cur->resolve()) {
	    తొలగించు cur;
	    తిరిగి;
      }

      cur->తదుపరి_2 = resolv_list;
      resolv_list = cur;
}


/*
 * Look up vvp_nets in the symbol table. The "source" is the label for
 * the net that I want to feed, and net->port[port] is the vvp_net
 * input that I want that node to feed into. When the name is found,
 * put net->port[port] into the fan-out list for that node.
 */
నిర్మాణం vvp_net_resolv_list_s: బహిరంగ resolv_list_s {

      స్పష్టమైన vvp_net_resolv_list_s(అక్షరం*l) : resolv_list_s(l) { }
	// port to be driven by the located node.
      vvp_net_ptr_t పోర్టు;
      మాయా తార్కిక resolve(తార్కిక mes) override;
};

తార్కిక vvp_net_resolv_list_s::resolve(తార్కిక mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      ఒకవేళ (tmp) {
	      // Link the input port to the located output.
	    tmp->link(పోర్టు);
	    తిరిగి నిజం;
      }

      ఒకవేళ (mes)
	    fprintf(stderr, "unresolved vvp_net reference: %s\n", label());

      తిరిగి అబద్ధం;
}

అంతఃవరుస స్థిర
శూన్య postpone_functor_ఆదానం(vvp_net_ptr_t పోర్టు, అక్షరం*lab)
{
      నిర్మాణం vvp_net_resolv_list_s*res = కొత్త నిర్మాణం vvp_net_resolv_list_s(lab);
      res->పోర్టు   = పోర్టు;

      resolv_submit(res);
}


/*
 *  Generic functor reference lookup.
 */

నిర్మాణం functor_gen_resolv_list_s: బహిరంగ resolv_list_s {
      స్పష్టమైన functor_gen_resolv_list_s(అక్షరం*txt) : resolv_list_s(txt) {
	    ref = 0;
      }
      vvp_net_t**ref;
      మాయా తార్కిక resolve(తార్కిక mes) override;
};

తార్కిక functor_gen_resolv_list_s::resolve(తార్కిక mes)
{
      vvp_net_t*tmp = vvp_net_lookup(label());

      ఒకవేళ (tmp) {
	    *ref = tmp;
	    తిరిగి నిజం;
      }

      ఒకవేళ (mes)
	    fprintf(stderr, "unresolved functor reference: %s\n", label());

      తిరిగి అబద్ధం;
}

శూన్య functor_ref_lookup(vvp_net_t**ref, అక్షరం*lab)
{
      నిర్మాణం functor_gen_resolv_list_s*res =
	    కొత్త నిర్మాణం functor_gen_resolv_list_s(lab);

      res->ref    = ref;

      resolv_submit(res);
}

/*
 *  vpiHandle lookup
 */

నిర్మాణం vpi_handle_resolv_list_s: బహిరంగ resolv_list_s {
      స్పష్టమైన vpi_handle_resolv_list_s(అక్షరం*lab) : resolv_list_s(lab) {
	    handle = ఖాళీ;
      }
      మాయా తార్కిక resolve(తార్కిక mes) override;
      vpiHandle *handle;
};

తార్కిక vpi_handle_resolv_list_s::resolve(తార్కిక mes)
{
      చిహ్నం_విలువ_t val = sym_get_విలువ(sym_vpi, label());
      ఒకవేళ (!val.ptr) {
	    // check for thread access symbols
	    గుర్తులేని పునాది, wid;
	    size_t n = 0;
	    అక్షరం ss[32];
	    ఒకవేళ (2 == అక్షరశ్రేణి_చదువు(label(), "W<%u,%31[r]>%zn", &పునాది, ss, &n)
		       && n == అక్షరశ్రేణి_పొడవు(label())) {

		  val.ptr = vpip_make_vthr_మాట(పునాది, ss);
		  sym_సమితి_విలువ(sym_vpi, label(), val);

	    } లేకపోతే ఒకవేళ (1 == అక్షరశ్రేణి_చదువు(label(), "S<%u,str>%zn", &పునాది, &n)
		       && n == అక్షరశ్రేణి_పొడవు(label())) {

		  val.ptr = vpip_make_vthr_str_దొంతర(పునాది);
		  sym_సమితి_విలువ(sym_vpi, label(), val);

	    } లేకపోతే ఒకవేళ (3 == అక్షరశ్రేణి_చదువు(label(), "S<%u,vec4,%31[su]%u>%zn", &పునాది, ss, &wid, &n)
		       && n == అక్షరశ్రేణి_పొడవు(label())) {

		  తార్కిక signed_flag = అబద్ధం;
		  కొరకు (అక్షరం*fp = ss ;  *fp ;  fp += 1) ఎంపిక (*fp) {
		      స్థితి 's':
			signed_flag = నిజం;
			ఆపు;
		      స్థితి 'u':
			signed_flag = అబద్ధం;
			ఆపు;
		      అప్రమేయ:
			ఆపు;
		  }
		  val.ptr = vpip_make_vthr_vec4_దొంతర(పునాది, signed_flag, wid);
		  sym_సమితి_విలువ(sym_vpi, label(), val);
	    }

      }

      ఒకవేళ (!val.ptr) {
	    // check for memory word  M<mem,base,wid>
      }

      ఒకవేళ (strcmp(label(), "null") == 0) {
	    val.ptr = vpip_make_null_const();
	    sym_సమితి_విలువ(sym_vpi, label(), val);
      }

      ఒకవేళ (val.ptr) {
	    *handle = static_cast<vpiHandle>(val.ptr);
	    తిరిగి నిజం;
      }

      ఒకవేళ (mes)
	    fprintf(stderr, "unresolved vpi name lookup: %s\n", label());

      తిరిగి అబద్ధం;
}

శూన్య compile_vpi_lookup(vpiHandle *handle, అక్షరం*label)
{
      ఒకవేళ (strcmp(label, "$time") == 0) {
	    *handle = vpip_sim_సమయం(vpip_peek_current_scope(), అబద్ధం);
	    విడుదల(label);
	    తిరిగి;
      }

      ఒకవేళ (strcmp(label, "$stime") == 0) {
	    *handle = vpip_sim_సమయం(vpip_peek_current_scope(), నిజం);
	    విడుదల(label);
	    తిరిగి;
      }

      ఒకవేళ (strcmp(label, "$realtime") == 0) {
	    *handle = vpip_sim_realtime(vpip_peek_current_scope());
	    విడుదల(label);
	    తిరిగి;
      }

      ఒకవేళ (strcmp(label, "$simtime") == 0) {
	    *handle = vpip_sim_సమయం(0, అబద్ధం);
	    విడుదల(label);
	    తిరిగి;
      }

      నిర్మాణం vpi_handle_resolv_list_s*res
	    = కొత్త నిర్మాణం vpi_handle_resolv_list_s(label);

      res->handle = handle;
      resolv_submit(res);
}

/*
 * Code Label lookup
 */

నిర్మాణం సంకేతం_label_resolv_list_s: బహిరంగ resolv_list_s {
      స్పష్టమైన సంకేతం_label_resolv_list_s(అక్షరం*lab, తార్కిక cptr2) : resolv_list_s(lab) {
	    code = ఖాళీ;
	    cptr2_flag = cptr2;
      }
      నిర్మాణం vvp_సంకేతం_s *code;
      తార్కిక cptr2_flag;
      మాయా తార్కిక resolve(తార్కిక mes) override;
};

తార్కిక సంకేతం_label_resolv_list_s::resolve(తార్కిక mes)
{
      చిహ్నం_విలువ_t val = sym_get_విలువ(sym_codespace, label());
      ఒకవేళ (val.ptr) {
	    ఒకవేళ (cptr2_flag)
		  code->cptr2 = reinterpret_cast<vvp_సంకేతం_t>(val.ptr);
	    లేకపోతే
		  code->cptr = reinterpret_cast<vvp_సంకేతం_t>(val.ptr);
	    తిరిగి నిజం;
      }

      ఒకవేళ (mes)
	    fprintf(stderr, "unresolved code label: %s\n", label());

      తిరిగి అబద్ధం;
}

శూన్య సంకేతం_label_lookup(నిర్మాణం vvp_సంకేతం_s *code, అక్షరం *label, తార్కిక cptr2)
{
      నిర్మాణం సంకేతం_label_resolv_list_s *res
	    = కొత్త నిర్మాణం సంకేతం_label_resolv_list_s(label, cptr2);

      res->code  = code;

      resolv_submit(res);
}

నిర్మాణం సంకేతం_array_resolv_list_s: బహిరంగ resolv_list_s {
      స్పష్టమైన సంకేతం_array_resolv_list_s(అక్షరం*lab) : resolv_list_s(lab) {
	    code = ఖాళీ;
      }
      నిర్మాణం vvp_సంకేతం_s *code;
      మాయా తార్కిక resolve(తార్కిక mes) override;
};

తార్కిక సంకేతం_array_resolv_list_s::resolve(తార్కిక mes)
{
      code->array = array_కనుగొను(label());
      ఒకవేళ (code->array != 0) {
	    తిరిగి నిజం;
      }

      ఒకవేళ (mes)
	    fprintf(stderr, "Array unresolved: %s\n", label());

      తిరిగి అబద్ధం;
}

స్థిర శూన్య compile_array_lookup(నిర్మాణం vvp_సంకేతం_s*code, అక్షరం*label)
{
      నిర్మాణం సంకేతం_array_resolv_list_s *res
	    = కొత్త నిర్మాణం సంకేతం_array_resolv_list_s(label);

      res->code  = code;

      resolv_submit(res);
}

స్థిర std::list<నిర్మాణం __vpiSysTaskCall*> scheduled_compiletf;

శూన్య compile_compiletf(నిర్మాణం __vpiSysTaskCall*obj)
{
      ఒకవేళ (obj->defn->info.compiletf == 0)
	    తిరిగి;

      scheduled_compiletf.push_back(obj);
}

/*
 * When parsing is otherwise complete, this function is called to do
 * the final stuff. Clean up deferred linking here.
 */

శూన్య compile_cleanup(శూన్య)
{
      పూర్ణాంకం lnerrs = -1;
      పూర్ణాంకం nerrs = 0;
      పూర్ణాంకం చివరి;

      ఒకవేళ (verbose_flag) {
	    fprintf(stderr, " ... Linking\n");
	    fflush(stderr);
      }

      చేయి {
	    resolv_list_s *res = resolv_list;
	    resolv_list = 0x0;
	    చివరి = nerrs == lnerrs;
	    lnerrs = nerrs;
	    nerrs = 0;
	    అయితే (res) {
		  resolv_list_s *cur = res;
		  res = res->తదుపరి_2;
		  ఒకవేళ (cur->resolve(చివరి))
			తొలగించు cur;
		  లేకపోతే {
			nerrs++;
			cur->తదుపరి_2 = resolv_list;
			resolv_list = cur;
		  }
	    }
	    ఒకవేళ (nerrs && చివరి)
		  fprintf(stderr,
			  "compile_cleanup: %d unresolved items\n",
			  nerrs);
      } అయితే (nerrs && !చివరి);

      compile_errors += nerrs;

      ఒకవేళ (verbose_flag) {
	    fprintf(stderr, " ... Removing symbol tables\n");
	    fflush(stderr);
      }

	/* After compile is complete, the vpi symbol table is no
	   longer needed. VPI objects are located by following
	   scopes. */
      delete_చిహ్నం_పట్టిక(sym_vpi);
      sym_vpi = 0;

	/* Don't need the code labels. The instructions have numeric
	   pointers in them, the symbol table is no longer needed. */
      delete_చిహ్నం_పట్టిక(sym_codespace);
      sym_codespace = 0;

      delete_చిహ్నం_పట్టిక(sym_functors);
      sym_functors = 0;

      delete_udp_symbols();

      compile_island_cleanup();
      compile_array_cleanup();

      ఒకవేళ (verbose_flag) {
	    fprintf(stderr, " ... Compiletf functions\n");
	    fflush(stderr);
      }

      ప్రతిజ్ఞా(vpi_బహుళకం_flag == VPI_బహుళకం_NONE);
      vpi_బహుళకం_flag = VPI_బహుళకం_COMPILETF;

      అయితే (! scheduled_compiletf.empty()) {
	    నిర్మాణం __vpiSysTaskCall*obj = scheduled_compiletf.front();
	    scheduled_compiletf.pop_front();
	    vpip_cur_task = obj;
	    obj->defn->info.compiletf (obj->defn->info.user_దత్తాంశం);
	    vpip_cur_task = 0;
      }

      vpi_బహుళకం_flag = VPI_బహుళకం_NONE;
}

శూన్య compile_vpi_చిహ్నం(స్థిరరాశి అక్షరం*label, vpiHandle obj)
{
      చిహ్నం_విలువ_t val;
      val.ptr = obj;
      sym_సమితి_విలువ(sym_vpi, label, val);
}

/*
 * Initialize the compiler by allocation empty symbol tables and
 * initializing the various address spaces.
 */
శూన్య compile_init(శూన్య)
{
      sym_vpi = new_చిహ్నం_పట్టిక();

      sym_functors = new_చిహ్నం_పట్టిక();

      sym_codespace = new_చిహ్నం_పట్టిక();
      codespace_init();
}

శూన్య compile_ఎక్కించు_vpi_module(అక్షరం*name)
{
      vpip_ఎక్కించు_module(name);
      తొలగించు[] name;
}

శూన్య compile_vpi_సమయం_precision(దీర్ఘ pre)
{
      vpip_సమితి_సమయం_precision(pre);
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
స్థిర తార్కిక crstring_శీర్షిక_test(స్థిరరాశి అక్షరం*str)
{
      ఒకవేళ ((str[0] != 'C') && (str[0] != 'c'))
	    తిరిగి అబద్ధం;
      ఒకవేళ ((str[1] != 'r') || (str[2] != '<'))
	    తిరిగి అబద్ధం;

      తిరిగి నిజం;
}

తార్కిక crstring_test(స్థిరరాశి అక్షరం*str)
{
      ఒకవేళ (!crstring_శీర్షిక_test(str))
	    తిరిగి అబద్ధం;
      స్థిరరాశి అక్షరం*tp = strchr(str, '>');
      ఒకవేళ (tp == 0) తిరిగి అబద్ధం;
      ఒకవేళ (tp[1] != 0) తిరిగి అబద్ధం;

      ఒకవేళ ((strspn(str+3, "0123456789abcdefmg")+3) != (size_t)(tp - str))
	  తిరిగి అబద్ధం;

      తిరిగి నిజం;
}

ద్విగుణ crstring_గమ్యం_double(స్థిరరాశి అక్షరం*label)
{
      ప్రతిజ్ఞా(crstring_శీర్షిక_test(label));

      స్థిరరాశి అక్షరం*cp = label+3;
      ప్రతిజ్ఞా(*cp == 'm');
      cp += 1;
      అక్షరం*ep;
      uint64_t mant = strtoull(cp, &ep, 16);
      cp = ep;
      ప్రతిజ్ఞా(*cp == 'g');
      cp += 1;
      పూర్ణాంకం ఘాతాంకం = strtoul(cp, 0, 16);

      ద్విగుణ tmp;
      ఒకవేళ (mant == 0 && ఘాతాంకం == 0x3fff) {
	    tmp = INFINITY;
      } లేకపోతే ఒకవేళ (mant == 0 && ఘాతాంకం == 0x7fff) {
	    tmp = -INFINITY;
      } లేకపోతే ఒకవేళ (ఘాతాంకం == 0x3fff) {
	    tmp = nan("");
      } లేకపోతే {
	    ద్విగుణ sign = (ఘాతాంకం & 0x4000)? -1.0 : 1.0;
	    ఘాతాంకం &= 0x1fff;

	    tmp = sign * ldexp((ద్విగుణ)mant, ఘాతాంకం - 0x1000);
      }

      తిరిగి tmp;
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

శూన్య ఆదానం_connect(vvp_net_t*fdx, గుర్తులేని పోర్టు, అక్షరం*label)
{
      vvp_net_ptr_t ifdx = vvp_net_ptr_t(fdx, పోర్టు);

	/* Is this a vvp_vector4_t constant value? */
      ఒకవేళ (c4string_test(label)) {

	    vvp_vector4_t tmp = c4string_గమ్యం_vector4(label);

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
	    ఒకవేళ (label[0] == 'c')
		  కాలపట్టిక_init_సదిశ(ifdx, tmp);
	    లేకపోతే
		  కాలపట్టిక_సమితి_సదిశ(ifdx, tmp);

	    విడుదల(label);
	    తిరిగి;
      }

	/* Is this a vvp_vector8_t constant value? */
      ఒకవేళ (c8string_test(label)) {

	    vvp_vector8_t tmp = c8string_గమ్యం_vector8(label);
	    ఒకవేళ (label[0] == 'c')
		  కాలపట్టిక_init_సదిశ(ifdx, tmp);
	    లేకపోతే
		  కాలపట్టిక_సమితి_సదిశ(ifdx, tmp);

	    విడుదల(label);
	    తిరిగి;
      }

	/* Handle the Cr<> constant driver, which is a real-value
	   driver. */
      ఒకవేళ (crstring_test(label)) {

	    ద్విగుణ tmp = crstring_గమ్యం_double(label);

	    ఒకవేళ (label[0] == 'c')
		  కాలపట్టిక_init_సదిశ(ifdx, tmp);
	    లేకపోతే
		  కాలపట్టిక_సమితి_సదిశ(ifdx, tmp);
	    విడుదల(label);
	    తిరిగి;
      }

	/* Handle the general case that this is a label for a node in
	   the vvp net. This arranges for the label to be preserved in
	   a linker list, and linked when the symbol table is
	   complete. */
      postpone_functor_ఆదానం(ifdx, label);
}

శూన్య inputs_connect(vvp_net_t*fdx, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ఒకవేళ (argc > 4) {
	    std::cerr << "XXXX argv[0] = " << argv[0].పాఠ్యం << std::పంక్తి_ముగింపు;
      }
      ప్రతిజ్ఞా(argc <= 4);

      కొరకు (గుర్తులేని idx = 0;  idx < argc;  idx += 1) {

	    ఆదానం_connect(fdx, idx, argv[idx].పాఠ్యం);
      }
}

శూన్య wide_inputs_connect(vvp_wide_fun_core*core,
			 గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
	/* Create input functors to receive values from the
	   network. These functors pass the data to the core. */
      గుర్తులేని ఆదానం_functors = (argc+3) / 4;
      కొరకు (గుర్తులేని idx = 0 ;  idx < ఆదానం_functors ;  idx += 1) {
	    గుర్తులేని పునాది = idx*4;
	    గుర్తులేని trans = 4;
	    ఒకవేళ (పునాది+trans > argc)
		  trans = argc - పునాది;

	    vvp_wide_fun_t*cur = కొత్త vvp_wide_fun_t(core, పునాది);
	    vvp_net_t*ptr = కొత్త vvp_net_t;
	    ptr->fun = cur;

	    inputs_connect(ptr, trans, argv+పునాది);
      }
}

మూస <తరగతి T_> శూన్య make_arith(T_ *arith, అక్షరం*label,
				    గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      vvp_net_t* ptr = కొత్త vvp_net_t;
      ptr->fun = arith;

      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);

      ప్రతిజ్ఞా(argc == 2);
      inputs_connect(ptr, argc, argv);

      విడుదల(argv);
}

శూన్య compile_arith_cast_int(అక్షరం*label, దీర్ఘ width,
                            గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      vvp_arith_cast_int*arith = కొత్త vvp_arith_cast_int((గుర్తులేని) width);

      vvp_net_t* ptr = కొత్త vvp_net_t;
      ptr->fun = arith;

      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);

      ప్రతిజ్ఞా(argc == 1);
      inputs_connect(ptr, argc, argv);
      విడుదల(argv);
}

శూన్య compile_arith_cast_vec2(అక్షరం*label, దీర్ఘ width,
			     గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      vvp_arith_cast_vec2*arith = కొత్త vvp_arith_cast_vec2((గుర్తులేని) width);

      vvp_net_t* ptr = కొత్త vvp_net_t;
      ptr->fun = arith;

      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);

      ప్రతిజ్ఞా(argc == 1);
      inputs_connect(ptr, argc, argv);
      విడుదల(argv);
}

శూన్య compile_arith_cast_real(అక్షరం*label, తార్కిక signed_flag,
                             గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      vvp_arith_cast_real*arith = కొత్త vvp_arith_cast_real(signed_flag);

      vvp_net_t* ptr = కొత్త vvp_net_t;
      ptr->fun = arith;

      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);

      ప్రతిజ్ఞా(argc == 1);
      inputs_connect(ptr, argc, argv);
      విడుదల(argv);
}

శూన్య compile_arith_abs(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      vvp_arith_abs*arith = కొత్త vvp_arith_abs;

      vvp_net_t* ptr = కొత్త vvp_net_t;
      ptr->fun = arith;

      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);

      ప్రతిజ్ఞా(argc == 1);
      inputs_connect(ptr, argc, argv);
      విడుదల(argv);
}

శూన్య compile_arith_div(అక్షరం*label, దీర్ఘ wid, తార్కిక signed_flag,
		       గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    స్థిరరాశి అక్షరం *suffix = "";
	    ఒకవేళ (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s; .arith/div%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_arith_div(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

శూన్య compile_arith_div_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s; .arith/divr has wrong number of symbols\n", label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_real_ *arith = కొత్త vvp_arith_div_real;
      make_arith(arith, label, argc, argv);
}

శూన్య compile_arith_mod(అక్షరం*label, దీర్ఘ wid, తార్కిక signed_flag,
		       గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .arith/mod has wrong number of symbols\n", label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_arith_mod(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

శూన్య compile_arith_mod_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .arith/mod.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_real_ *arith = కొత్త vvp_arith_mod_real;
      make_arith(arith, label, argc, argv);
}

శూన్య compile_arith_mult(అక్షరం*label, దీర్ఘ wid,
			గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .arith/mult has wrong number of symbols\n", label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_arith_mult(wid);
      make_arith(arith, label, argc, argv);
}

శూన్య compile_arith_mult_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .arith/mult.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_real_ *arith = కొత్త vvp_arith_mult_real;
      make_arith(arith, label, argc, argv);
}


శూన్య compile_arith_pow(అక్షరం*label, దీర్ఘ wid, తార్కిక signed_flag,
		       గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    స్థిరరాశి అక్షరం *suffix = "";
	    ఒకవేళ (signed_flag) suffix = ".s";
	    fprintf(stderr, "%s .arith/pow%s has wrong number of "
	                    "symbols\n", label, suffix);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_arith_pow(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

శూన్య compile_arith_pow_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .arith/pow.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_real_ *arith = కొత్త vvp_arith_pow_real;
      make_arith(arith, label, argc, argv);
}

శూన్య compile_arith_sub(అక్షరం*label, దీర్ఘ wid, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .arith/sub has wrong number of symbols\n", label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_arith_sub(wid);
      make_arith(arith, label, argc, argv);
}

శూన్య compile_arith_sub_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s; .arith/sub.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_real_ *arith = కొత్త vvp_arith_sub_real;
      make_arith(arith, label, argc, argv);
}

శూన్య compile_arith_కూడిక(అక్షరం*label, దీర్ఘ wid, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .arith/sum has wrong number of symbols\n", label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_arith_కూడిక(wid);
      make_arith(arith, label, argc, argv);
}

శూన్య compile_arith_కూడిక_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .arith/sum.r has wrong number of symbols\n", label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_real_ *arith = కొత్త vvp_arith_కూడిక_real;
      make_arith(arith, label, argc, argv);
}

శూన్య compile_cmp_eeq(అక్షరం*label, దీర్ఘ wid,
		     గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_cmp_eeq(wid);

      make_arith(arith, label, argc, argv);
}

శూన్య compile_cmp_nee(అక్షరం*label, దీర్ఘ wid,
		     గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eeq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_cmp_nee(wid);

      make_arith(arith, label, argc, argv);
}

శూన్య compile_cmp_eq(అక్షరం*label, దీర్ఘ wid, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_cmp_eq(wid);
      make_arith(arith, label, argc, argv);
}

శూన్య compile_cmp_eqx(అక్షరం*label, దీర్ఘ wid, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqx has wrong number of symbols\n",label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_cmp_eqx(wid);
      make_arith(arith, label, argc, argv);
}

శూన్య compile_cmp_eqz(అక్షరం*label, దీర్ఘ wid, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eqz has wrong number of symbols\n",label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_cmp_eqz(wid);
      make_arith(arith, label, argc, argv);
}

శూన్య compile_cmp_eq_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .cmp/eq.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_real_ *arith = కొత్త vvp_cmp_eq_real;
      make_arith(arith, label, argc, argv);
}

శూన్య compile_cmp_ne(అక్షరం*label, దీర్ఘ wid, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_cmp_ne(wid);
      make_arith(arith, label, argc, argv);
}

శూన్య compile_cmp_ne_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ne.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_real_ *arith = కొత్త vvp_cmp_ne_real;
      make_arith(arith, label, argc, argv);
}

శూన్య compile_cmp_ge(అక్షరం*label, దీర్ఘ wid, తార్కిక signed_flag,
		    గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge has wrong number of symbols\n", label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_cmp_ge(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

శూన్య compile_cmp_ge_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .cmp/ge.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_real_ *arith = కొత్త vvp_cmp_ge_real;
      make_arith(arith, label, argc, argv);
}

శూన్య compile_cmp_gt(అక్షరం*label, దీర్ఘ wid, తార్కిక signed_flag,
		    గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt has wrong number of symbols\n", label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_cmp_gt(wid, signed_flag);

      make_arith(arith, label, argc, argv);
}

శూన్య compile_cmp_gt_r(అక్షరం*label, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .cmp/gt.r has wrong number of symbols\n",label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_real_ *arith = కొత్త vvp_cmp_gt_real;
      make_arith(arith, label, argc, argv);
}

శూన్య compile_cmp_weq(అక్షరం*label, దీర్ఘ wid,
		     గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .cmp/weq has wrong number of symbols\n",label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_cmp_weq(wid);

      make_arith(arith, label, argc, argv);
}

శూన్య compile_cmp_wne(అక్షరం*label, దీర్ఘ wid,
		     గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      ఒకవేళ (argc != 2) {
	    fprintf(stderr, "%s .cmp/wne has wrong number of symbols\n",label);
	    compile_errors += 1;
	    తిరిగి;
      }

      vvp_arith_ *arith = కొత్త vvp_cmp_wne(wid);

      make_arith(arith, label, argc, argv);
}


శూన్య compile_delay(అక్షరం*label, గుర్తులేని width,
                   vvp_delay_t*delay, నిర్మాణం symb_s arg)
{
      vvp_net_t*net = కొత్త vvp_net_t;
      vvp_fun_delay*obj = కొత్త vvp_fun_delay(net, width, *delay);
      net->fun = obj;

      తొలగించు delay;

      ఆదానం_connect(net, 0, arg.పాఠ్యం);

      define_functor_చిహ్నం(label, net);
      విడుదల(label);
}

శూన్య compile_delay(అక్షరం*label, గుర్తులేని width,
                   గుర్తులేని argc, నిర్మాణం symb_s*argv, తార్కిక ignore_decay)
{
      vvp_delay_t stub (0, 0, 0);
      ఒకవేళ (ignore_decay) stub.సమితి_ignore_decay();
      vvp_net_t*net = కొత్త vvp_net_t;
      vvp_fun_delay*obj = కొత్త vvp_fun_delay(net, width, stub);
      net->fun = obj;

      inputs_connect(net, argc, argv);
      విడుదల(argv);

      define_functor_చిహ్నం(label, net);
      విడుదల(label);
}

/*
 * Extend nodes.
 */
శూన్య compile_extend_signed(అక్షరం*label, దీర్ఘ wid, నిర్మాణం symb_s arg)
{
      ప్రతిజ్ఞా(wid >= 0);

      vvp_fun_extend_signed*fun = కొత్త vvp_fun_extend_signed(wid);
      vvp_net_t*ptr = కొత్త vvp_net_t;
      ptr->fun = fun;

      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);

      ఆదానం_connect(ptr, 0, arg.పాఠ్యం);
}

నిర్మాణం __vpiModPath* compile_modpath(అక్షరం*label, గుర్తులేని width,
                                     నిర్మాణం symb_s drv, నిర్మాణం symb_s dest)
{
      vvp_net_t*net = కొత్త vvp_net_t;
      vvp_fun_modpath*obj = కొత్త vvp_fun_modpath(net, width);
      net->fun = obj;

      ఆదానం_connect(net, 0, drv.పాఠ్యం);

      define_functor_చిహ్నం(label, net);

      __vpiModPath*modpath = vpip_make_modpath(net);

      compile_vpi_lookup(&modpath->మార్గం_పదం_బహిర్గతం.సమాసం, dest.పాఠ్యం);

      విడుదల(label);

      modpath->modpath = obj;
      తిరిగి modpath;
}

స్థిర నిర్మాణం __vpiModPathSrc*make_modpath_src(నిర్మాణం __vpiModPath*మార్గం,
					       అక్షరం అంచు_c,
					       స్థిరరాశి నిర్మాణం symb_s&src,
					       నిర్మాణం numbv_s&vals,
					       తార్కిక ifnone)
{
      vvp_fun_modpath*dst = మార్గం->modpath;

      vvp_time64_t use_delay[12];
      ప్రతిజ్ఞా(vals.cnt == 12);
      కొరకు (గుర్తులేని idx = 0 ; idx < vals.cnt ;  idx += 1) {
	    use_delay[idx] = vals.nvec[idx];
      }

      numbv_clear(&vals);

      vvp_fun_modpath_src*obj = 0;

      పూర్ణాంకం vpi_అంచు = vpi_No_అంచు;
      ఒకవేళ (అంచు_c == 0) {
	    obj = కొత్త vvp_fun_modpath_src(use_delay);

      } లేకపోతే {
	    తార్కిక posedge, negedge;
	    ఎంపిక (అంచు_c) {
		స్థితి '+':
		  vpi_అంచు = vpiPosedge;
		  posedge = నిజం;
		  negedge = అబద్ధం;
		  ఆపు;
		స్థితి '-':
		  vpi_అంచు = vpiNegedge;
		  posedge = అబద్ధం;
		  negedge = నిజం;
		  ఆపు;
#ఒకవేళ 0
		స్థితి '*':
		  posedge = నిజం;
		  negedge = నిజం;
		  ఆపు;
#అయితే_ముగింపు
		అప్రమేయ:
		  posedge = అబద్ధం;
		  negedge = అబద్ధం;
		  fprintf(stderr, "Unknown edge identifier %c(%d).\n", అంచు_c,
		          అంచు_c);
		  ప్రతిజ్ఞా(0);
	    }
	    obj = కొత్త vvp_fun_modpath_అంచు(use_delay, posedge, negedge);
      }

      vvp_net_t*net = కొత్త vvp_net_t;
      నిర్మాణం __vpiModPathSrc* srcobj = vpip_make_modpath_src(మార్గం, net) ;
      vpip_attach_గమ్యం_current_scope(srcobj);
      net->fun = obj;

	/* Save the vpiEdge directory into the input path term. */
      srcobj->మార్గం_పదం_లోపల.అంచు = vpi_అంచు;
      ఆదానం_connect(net, 0, src.పాఠ్యం);
      dst->కూడు_modpath_src(obj, ifnone);

      తిరిగి srcobj;
}

శూన్య compile_modpath_src(నిర్మాణం __vpiModPath*dst, అక్షరం అంచు_c,
			 స్థిరరాశి నిర్మాణం symb_s&src,
			 నిర్మాణం numbv_s&vals,
			 స్థిరరాశి నిర్మాణం symb_s&condit_src,
			 స్థిరరాశి నిర్మాణం symb_s&మార్గం_పదం_లోపల)
{
      నిర్మాణం __vpiModPathSrc*obj =
	    make_modpath_src(dst, అంచు_c, src, vals, అబద్ధం);
      ఆదానం_connect(obj->net, 1, condit_src.పాఠ్యం);
      compile_vpi_lookup(&obj->మార్గం_పదం_లోపల.సమాసం, మార్గం_పదం_లోపల.పాఠ్యం);
}

శూన్య compile_modpath_src(నిర్మాణం __vpiModPath*dst, అక్షరం అంచు_c,
			 స్థిరరాశి నిర్మాణం symb_s&src,
			 నిర్మాణం numbv_s&vals,
			 పూర్ణాంకం condit_src,
			 స్థిరరాశి నిర్మాణం symb_s&మార్గం_పదం_లోపల,
			 తార్కిక ifnone)
{
      ప్రతిజ్ఞా(condit_src == 0);
      నిర్మాణం __vpiModPathSrc*obj =
	    make_modpath_src(dst, అంచు_c, src, vals, ifnone);
      compile_vpi_lookup(&obj->మార్గం_పదం_లోపల.సమాసం, మార్గం_పదం_లోపల.పాఠ్యం);
}

/*
 * A .shift/l statement creates an array of functors for the
 * width. The 0 input is the data vector to be shifted and the 1 input
 * is the amount of the shift. An unconnected shift amount is set to 0.
 */
శూన్య compile_shiftl(అక్షరం*label, దీర్ఘ wid, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      vvp_arith_ *arith = కొత్త vvp_shiftl(wid);
      make_arith(arith, label, argc, argv);
}

శూన్య compile_shiftr(అక్షరం*label, దీర్ఘ wid, తార్కిక signed_flag,
		    గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      ప్రతిజ్ఞా( wid > 0 );

      vvp_arith_ *arith = కొత్త vvp_shiftr(wid, signed_flag);
      make_arith(arith, label, argc, argv);
}

శూన్య compile_resolver(అక్షరం*label, అక్షరం*type, గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      vvp_net_t*net = కొత్త vvp_net_t;

      resolv_core*core = 0;

      ఒకవేళ (strcmp(type,"tri") == 0) {
	    core = కొత్త resolv_tri(argc, net, vvp_scalar_t(BIT4_Z, 0,0));

      } లేకపోతే ఒకవేళ (strcmp(type,"tri0") == 0) {
	    core = కొత్త resolv_tri(argc, net, vvp_scalar_t(BIT4_0, 5,5));

      } లేకపోతే ఒకవేళ (strcmp(type,"tri1") == 0) {
	    core = కొత్త resolv_tri(argc, net, vvp_scalar_t(BIT4_1, 5,5));

      } లేకపోతే ఒకవేళ (strcmp(type,"triand") == 0) {
	    core = కొత్త resolv_triand(argc, net);

      } లేకపోతే ఒకవేళ (strcmp(type,"trior") == 0) {
	    core = కొత్త resolv_trior(argc, net);

      } లేకపోతే {
	    fprintf(stderr, "invalid resolver type: %s\n", type);
	    compile_errors += 1;
            తొలగించు net;
      }

      ఒకవేళ (core) {
	    net->fun = core;
	    define_functor_చిహ్నం(label, net);

            కొరకు (గుర్తులేని పునాది = 0 ;  పునాది < argc ;  పునాది += 4) {
	          గుర్తులేని nports = argc - పునాది;
                  ఒకవేళ (nports > 4)
                        nports = 4;

                  ఒకవేళ (పునాది > 0) {
                        net = కొత్త vvp_net_t;
                        net->fun = కొత్త resolv_extend(core, పునాది);
                  }
                  inputs_connect(net, nports, argv+పునాది);
            }
      }
      విడుదల(type);
      విడుదల(label);
      విడుదల(argv);
}

శూన్య compile_udp_def(పూర్ణాంకం sequ, అక్షరం *label, అక్షరం *name,
		     గుర్తులేని nin, గుర్తులేని init, అక్షరం **పట్టిక)
{
      ఒకవేళ (sequ) {
	    vvp_bit4_t init4;
	    ఒకవేళ (init == 0)
		  init4 = BIT4_0;
	    లేకపోతే ఒకవేళ (init == 1)
		  init4 = BIT4_1;
	    లేకపోతే
		  init4 = BIT4_X;

	    vvp_udp_seq_s *u = కొత్త vvp_udp_seq_s(label, name, nin, init4);
	    u->compile_పట్టిక(పట్టిక);
      } లేకపోతే {
	    vvp_udp_comb_s *u = కొత్త vvp_udp_comb_s(label, name, nin);
	    u->compile_పట్టిక(పట్టిక);
      }
      విడుదల(label);
}

అక్షరం **compile_udp_పట్టిక(అక్షరం **పట్టిక, అక్షరం *అడ్డువరుస)
{
  ఒకవేళ (పట్టిక)
    ప్రతిజ్ఞా(అక్షరశ్రేణి_పొడవు(*పట్టిక)==అక్షరశ్రేణి_పొడవు(అడ్డువరుస));

  అక్షరం **tt;
  కొరకు (tt = పట్టిక; tt && *tt; tt++) { }
  పూర్ణాంకం n = (tt-పట్టిక) + 2;

  పట్టిక = static_cast<అక్షరం**>(realloc(పట్టిక, n*పరిమాణం(అక్షరం*)));
  పట్టిక[n-2] = అడ్డువరుస;
  పట్టిక[n-1] = 0x0;

  తిరిగి పట్టిక;
}


/*
 * The parser uses this function to compile and link an executable
 * opcode. I do this by looking up the opcode in the opcode_table. The
 * table gives the operand structure that is acceptable, so I can
 * process the operands here as well.
 */
శూన్య compile_సంకేతం(అక్షరం*label, అక్షరం*mnem, comp_operands_t opa)
{
	/* First, I can give the label a value that is the current
	   codespace pointer. Don't need the text of the label after
	   this is done. */
      ఒకవేళ (label)
	    compile_codelabel(label);

	/* Lookup the opcode in the opcode table. */
      నిర్మాణం opcode_పట్టిక_s*op = static_cast<నిర్మాణం opcode_పట్టిక_s*>
                                            (bsearch(mnem, opcode_పట్టిక, opcode_లెక్క,
                                                     పరిమాణం(నిర్మాణం opcode_పట్టిక_s),
                                                     &opcode_compare));
      ఒకవేళ (op == 0) {
	    yyerror("Invalid opcode");
	    compile_errors += 1;
	    తిరిగి;
      }

      ప్రతిజ్ఞా(op);

	/* Build up the code from the information about the opcode and
	   the information from the compiler. */
      vvp_సంకేతం_t code = codespace_allocate();
      code->opcode = op->opcode;

      ఒకవేళ (op->argc != (opa? opa->argc : 0)) {
	    yyerror("operand count");
	    compile_errors += 1;
	    తిరిగి;
      }

	/* Pull the operands that the instruction expects from the
	   list that the parser supplied. */

      కొరకు (గుర్తులేని idx = 0 ;  idx < op->argc ;  idx += 1) {

	    ఎంపిక (op->argt[idx]) {
		స్థితి OA_NONE:
		  ఆపు;

		స్థితి OA_ARR_PTR:
		  ఒకవేళ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			ఆపు;
		  }

		  compile_array_lookup(code, opa->argv[idx].symb.పాఠ్యం);
		  ఆపు;

		స్థితి OA_BIT1:
		  ఒకవేళ (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			ఆపు;
		  }
		  code->బిట్టు_idx[0] = opa->argv[idx].numb;
		  ఆపు;

		స్థితి OA_BIT2:
		  ఒకవేళ (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			ఆపు;
		  }
		  code->బిట్టు_idx[1] = opa->argv[idx].numb;
		  ఆపు;

		స్థితి OA_సంకేతం_PTR:
		స్థితి OA_సంకేతం_PTR2:
		  ఒకవేళ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			ఆపు;
		  }

		  ప్రతిజ్ఞా(opa->argv[idx].symb.idx == 0);
		  సంకేతం_label_lookup(code, opa->argv[idx].symb.పాఠ్యం,
				    op->argt[idx]==OA_సంకేతం_PTR2);
		  ఆపు;

		స్థితి OA_FUNC_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  ఒకవేళ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			ఆపు;
		  }

		  functor_ref_lookup(&code->net, opa->argv[idx].symb.పాఠ్యం);
		  ఆపు;

		స్థితి OA_FUNC_PTR2:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  ఒకవేళ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			ఆపు;
		  }

		  functor_ref_lookup(&code->net2, opa->argv[idx].symb.పాఠ్యం);
		  ఆపు;

		స్థితి OA_సంఖ్య:
		  ఒకవేళ (opa->argv[idx].ltype != L_NUMB) {
			yyerror("operand format");
			ఆపు;
		  }

		  code->సంఖ్య = opa->argv[idx].numb;
		  ఆపు;

		స్థితి OA_VPI_PTR:
		    /* The operand is a functor. Resolve the label to
		       a functor pointer, or postpone the resolution
		       if it is not defined yet. */
		  ఒకవేళ (opa->argv[idx].ltype != L_SYMB) {
			yyerror("operand format");
			ఆపు;
		  }

		  compile_vpi_lookup(&code->handle, opa->argv[idx].symb.పాఠ్యం);
		  ఆపు;

		స్థితి OA_STRING:
		  ఒకవేళ (opa->argv[idx].ltype != L_STRING) {
			yyerror("operand format");
			ఆపు;
		  }

		  code->పాఠ్యం = opa->argv[idx].పాఠ్యం;
		  ఆపు;
	    }
      }

      విడుదల(opa);

      విడుదల(mnem);
}

శూన్య compile_codelabel(అక్షరం*label)
{
      చిహ్నం_విలువ_t val;
      vvp_సంకేతం_t ptr = codespace_తదుపరి();

      val.ptr = ptr;
      sym_సమితి_విలువ(sym_codespace, label, val);

      విడుదల(label);
}

శూన్య compile_file_పంక్తి(అక్షరం*label, దీర్ఘ file_idx, దీర్ఘ lineno,
                       అక్షరం*description)
{
      ఒకవేళ (label) compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_సంకేతం_t code = codespace_allocate();
      code->opcode = &యొక్క_FILE_పంక్తి;

	/* Create a vpiHandle that contains the information. */
      code->handle = vpip_build_file_పంక్తి(description, file_idx, lineno);
      ప్రతిజ్ఞా(code->handle);

	/* Done with the lexor-allocated name string. */
      తొలగించు[] description;
}

శూన్య compile_vpi_call(అక్షరం*label, అక్షరం*name,
                      తార్కిక func_as_task_err, తార్కిక func_as_task_warn,
                      దీర్ఘ file_idx, దీర్ఘ lineno,
                      గుర్తులేని argc, vpiHandle*argv,
		      గుర్తులేని vec4_దొంతర, గుర్తులేని real_దొంతర, గుర్తులేని string_దొంతర)
{
      ఒకవేళ (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_సంకేతం_t code = codespace_allocate();
      code->opcode = &యొక్క_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, 0, 0,
					 0, func_as_task_err, func_as_task_warn,
                                         argc, argv, vec4_దొంతర, real_దొంతర, string_దొంతర,
					 file_idx, lineno);
      ఒకవేళ (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      తొలగించు[] name;
}

శూన్య compile_vpi_func_call(అక్షరం*label, అక్షరం*name,
			   పూర్ణాంకం val_type, గుర్తులేని val_wid,
			   దీర్ఘ file_idx, దీర్ఘ lineno,
			   గుర్తులేని argc, vpiHandle*argv,
			   గుర్తులేని vec4_దొంతర,
			   గుర్తులేని real_దొంతర,
			   గుర్తులేని string_దొంతర)
{
      ఒకవేళ (label)
	    compile_codelabel(label);

	/* Create an instruction in the code space. */
      vvp_సంకేతం_t code = codespace_allocate();
      code->opcode = &యొక్క_VPI_CALL;

	/* Create a vpiHandle that bundles the call information, and
	   store that handle in the instruction. */
      code->handle = vpip_build_vpi_call(name, val_type, val_wid,
					 0, నిజం, అబద్ధం,
                                         argc, argv, vec4_దొంతర, real_దొంతర, string_దొంతర,
					 file_idx, lineno);
      ఒకవేళ (code->handle == 0)
	    compile_errors += 1;

	/* Done with the lexor-allocated name string. */
      తొలగించు[] name;
}

/*
 * When the parser finds a thread statement, I create a new thread
 * with the start address referenced by the program symbol passed to
 * me.
 */
శూన్య compile_thread(అక్షరం*ప్రారంభం_sym, అక్షరం*flag)
{
      తార్కిక నెట్టు_flag = అబద్ధం;

      చిహ్నం_విలువ_t tmp = sym_get_విలువ(sym_codespace, ప్రారంభం_sym);
      vvp_సంకేతం_t pc = reinterpret_cast<vvp_సంకేతం_t>(tmp.ptr);
      ఒకవేళ (pc == 0) {
	    yyerror("unresolved address");
	    తిరిగి;
      }

      ఒకవేళ (flag && (strcmp(flag,"$push") == 0))
	    నెట్టు_flag = నిజం;

      vthread_t thr = vthread_new(pc, vpip_peek_current_scope());

      ఒకవేళ (flag && (strcmp(flag,"$init") == 0))
	    కాలపట్టిక_init_vthread(thr);
      లేకపోతే ఒకవేళ (flag && (strcmp(flag,"$final") == 0))
	    కాలపట్టిక_final_vthread(thr);
      లేకపోతే
	    కాలపట్టిక_vthread(thr, 0, నెట్టు_flag);

      విడుదల(ప్రారంభం_sym);
      విడుదల(flag);
}

శూన్య compile_param_logic(అక్షరం*label, స్థిరరాశి అక్షరం*name, అక్షరం*value, తార్కిక signed_flag,
                         తార్కిక local_flag,
                         దీర్ఘ file_idx, దీర్ఘ lineno)
{
      vvp_vector4_t value4 = c4string_గమ్యం_vector4(value);
      vpiHandle obj = vpip_make_ద్విమాన_param(name, value4, signed_flag,
                                             local_flag, file_idx, lineno);
      compile_vpi_చిహ్నం(label, obj);
      vpip_attach_గమ్యం_current_scope(obj);

      విడుదల(label);
      విడుదల(value);
}

శూన్య compile_param_string(అక్షరం*label, స్థిరరాశి అక్షరం*name, అక్షరం*value,
                          తార్కిక local_flag,
                          దీర్ఘ file_idx, దీర్ఘ lineno)
{
	// name and value become owned by vpip_make_string_param
      vpiHandle obj = vpip_make_string_param(name, value, local_flag, file_idx, lineno);
      compile_vpi_చిహ్నం(label, obj);
      vpip_attach_గమ్యం_current_scope(obj);

      విడుదల(label);
}

శూన్య compile_param_real(అక్షరం*label, స్థిరరాశి అక్షరం*name, అక్షరం*value,
                        తార్కిక local_flag,
                        దీర్ఘ file_idx, దీర్ఘ lineno)
{
      ద్విగుణ dvalue = crstring_గమ్యం_double(value);
      vpiHandle obj = vpip_make_real_param(name, dvalue, local_flag, file_idx, lineno);
      compile_vpi_చిహ్నం(label, obj);
      vpip_attach_గమ్యం_current_scope(obj);

      విడుదల(label);
      విడుదల(value);
}

శూన్య compile_island(అక్షరం*label, అక్షరం*type)
{
      ఒకవేళ (strcmp(type,"tran") == 0)
	    compile_island_tran(label);
      లేకపోతే
	    ప్రతిజ్ఞా(0);

      విడుదల(type);
}
