#నిర్వచితం_కాకపోతే IVL_codes_H
#నిర్వచించు IVL_codes_H
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
# include  "vvp_net.h"
# include  "array.h"
# include  "vthread.h"

రకనిర్వచనం bool (*vvp_సంకేతం_fun)(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);

/*
 * These functions are implementations of executable op-codes. The
 * implementation lives in the vthread.cc file so that they have
 * access to the thread context.
 */
బాహ్య bool యొక్క_ABS_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_కూడు(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_కూడు_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ADDI(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ALLOC(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_AND(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ANDR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_కేటాయించు_AR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_కేటాయించు_ARD(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_కేటాయించు_ARE(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_కేటాయించు_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_కేటాయించు_VEC4_D(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_కేటాయించు_VEC4_E(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_కేటాయించు_VEC4_A_D(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_కేటాయించు_VEC4_A_E(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_కేటాయించు_VEC4_OFF_D(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_కేటాయించు_VEC4_OFF_E(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_కేటాయించు_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_కేటాయించు_WRD(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_కేటాయించు_WRE(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_BLEND(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_BLEND_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_BLEND_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_BREAKPOINT(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CALLF_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CALLF_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CALLF_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CALLF_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CALLF_VOID(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CASSIGN_LINK(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CASSIGN_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CASSIGN_VEC4_OFF(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CASSIGN_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CAST2(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CAST_VEC2_DAR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CAST_VEC4_DAR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CAST_VEC4_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPE(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPIE(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPINE(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPNE(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPS(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPIS(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPSTR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPU(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPIU(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPWE(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPWNE(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPWR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPX(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPZ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CONCAT_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CONCATI_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CONCAT_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CONCATI_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CVT_RV(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CVT_RV_S(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CVT_SR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CVT_UR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CVT_VR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DEASSIGN(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DEASSIGN_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DEBUG_THR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DELAY(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DELAYX(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DELETE_ELEM(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DELETE_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DELETE_తోక(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DISABLE(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DISABLE_FLOW(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DISABLE_FORK(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DIV(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DIV_S(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DIV_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DUP_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DUP_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DUP_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_చివర(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_EVENT(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_EVENT_NB(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_EVCTL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_EVCTLC(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_EVCTLI(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_EVCTLS(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_FILE_పంక్తి(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_FLAG_GET_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_FLAG_INV(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_FLAG_MOV(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_FLAG_OR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_FLAG_సమితి_IMM(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_FLAG_సమితి_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_FORCE_LINK(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_FORCE_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_FORCE_VEC4_OFF(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_FORCE_VEC4_OFF_D(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_FORCE_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_FORK(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_FREE(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_INV(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_IX_కూడు(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_IX_GETV(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_IX_GETV_S(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_IX_ఎక్కించు(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_IX_MOV(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_IX_MUL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_IX_SUB(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_IX_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_IX_VEC4_S(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_JMP(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_JMP0(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_JMP0_XZ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_JMP1(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_JMP1_XZ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_JOIN(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_JOIN_DETACH(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ఎక్కించు_AR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ఎక్కించు_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ఎక్కించు_DAR_R(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ఎక్కించు_DAR_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ఎక్కించు_DAR_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ఎక్కించు_PROP_DAR_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ఎక్కించు_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ఎక్కించు_OBJA(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ఎక్కించు_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ఎక్కించు_STRA(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ఎక్కించు_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ఎక్కించు_VEC4_A(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_MAX_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_MIN_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_MOD(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_MOD_S(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_MOD_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_MUL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_MULI(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_MUL_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_NEG_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_NAND(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_NANDR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_NEW_COBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_NEW_DARRAY(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_NOOP(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_NOR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_NORR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_NULL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_OR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_ORR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PAD_S(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PAD_U(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PART_S(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PART_U(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PARTI_S(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PARTI_U(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_తీయి_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_తీయి_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_తీయి_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_తీయి_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_POW(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_POW_S(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_POW_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QINSERT_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QINSERT_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QINSERT_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QPOP_B_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QPOP_B_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QPOP_B_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QPOP_F_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QPOP_F_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QPOP_F_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PROP_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PROP_R(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PROP_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PUSHI_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PUSHI_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PUSHI_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PUSHV_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PUTC_STR_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_RELEASE_NET(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_RELEASE_REG(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_RELEASE_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_REPLICATE(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_RET_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_RET_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_RET_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_తిరగేయి_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_తిరగేయి_PROP_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_RSORT_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_RSORT_PROP_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_SHUFFLE_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_క్రమీకరించు_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_క్రమీకరించు_PROP_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_RETLOAD_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_RETLOAD_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_RETLOAD_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_SCOPY(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_సమితి_DAR_OBJ_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_సమితి_DAR_OBJ_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_సమితి_DAR_OBJ_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_SHIFTL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_SHIFTR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_SHIFTR_S(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_విభజించు_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_DAR_R(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_DAR_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_DAR_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_OBJA(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_PROP_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_PROP_R(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_PROP_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_PROP_QB_R(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_PROP_QB_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_PROP_QB_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_PROP_QF_R(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_PROP_QF_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_PROP_QF_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QINSERT_PROP_R(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QINSERT_PROP_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QINSERT_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QPOP_PROP_B_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QPOP_PROP_B_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QPOP_PROP_B_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QPOP_PROP_F_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QPOP_PROP_F_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_QPOP_PROP_F_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_PROP_వరుస_కొలత(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_APPEND_మాట_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_NEW_ఖాళీలు_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_కొలత_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_మాట_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_మాట_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPIX_LTU(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_CMPIX_SLT0(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_నెట్టు_IX_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_కనుగొను_సూచీ_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_కనుగొను_సూచీ_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_కనుగొను_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_కనుగొను_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_కనుగొను_మొదటి_సూచీ_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_కనుగొను_మొదటి_సూచీ_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_కనుగొను_మొదటి_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_కనుగొను_మొదటి_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_కనుగొను_చివరి_సూచీ_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_కనుగొను_చివరి_సూచీ_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_కనుగొను_చివరి_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_కనుగొను_చివరి_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_MIN_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_MIN_OBJ_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_MIN_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_MAX_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_MAX_OBJ_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_MAX_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_UNIQUE_OBJ_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_UNIQUE_సూచీ_OBJ_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_UNIQUE_సూచీ_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_UNIQUE_సూచీ_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_UNIQUE_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_వరుస_UNIQUE_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DELETE_PROP_ELEM(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_DELETE_PROP_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_QB_R(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_QB_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_QB_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_QDAR_R(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_QDAR_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_QDAR_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_QF_R(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_QF_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_QF_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_QOBJ_R(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_QOBJ_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_QOBJ_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_REALA(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_STR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_STRA(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_STORE_VEC4_A(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_SUB(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_SUBI(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_SUB_WR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_SUBSTR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_SUBSTR_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_TEST_NUL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_TEST_NUL_A(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_TEST_NUL_OBJ(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_TEST_NUL_PROP(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_UPARR_AND_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_UPARR_OR_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_UPARR_లబ్ధం_OBJ_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_UPARR_లబ్ధం_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_UPARR_లబ్ధం_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_UPARR_లబ్ధం_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_UPARR_కూడిక_OBJ_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_UPARR_కూడిక_PROP_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_UPARR_కూడిక_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_UPARR_కూడిక_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_UPARR_XOR_V(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_VPI_CALL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_WAIT(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_WAIT_FORK(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_XNOR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_XNORR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_XOR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_XORR(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);

బాహ్య bool యొక్క_ZOMBIE(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);

బాహ్య bool యొక్క_EXEC_UFUNC_REAL(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_EXEC_UFUNC_VEC4(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);
బాహ్య bool యొక్క_REAP_UFUNC(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);

బాహ్య bool యొక్క_CHUNK_LINK(vthread_t thr, vvp_సంకేతం_t సంకేతం_2);

/*
 * This is the format of a machine code instruction.
 */
నిర్మాణం vvp_సంకేతం_s {
      vvp_సంకేతం_fun opcode;

      సంఘం {
	    గుర్తులేని దీర్ఘ సంఖ్య;
	    vvp_net_t    *net;
	    vvp_సంకేతం_t   cptr;
	    vvp_array_t array;
	    class __vpiHandle*handle;
	    __vpiScope*scope;
	    స్థిరరాశి అక్షరం*పాఠ్యం;
      };

      సంఘం {
	    uint32_t    బిట్టు_idx[2];
	    vvp_net_t   *net2;
	    vvp_సంకేతం_t   cptr2;
	    class ufunc_core*ufunc_core_ptr;
      };
};

/*
 * This function clears the code space, ready for initialization. This
 * needs to be done exactly once before any instructions are created.
 */
బాహ్య శూన్య codespace_init(శూన్య);


/*
 * This function returns a pointer to the next free instruction in the
 * code address space. The codespace_next returns a pointer to the
 * next opcode that will be allocated. This is used by label
 * statements to get the address that will be attached to a label in
 * the code.
 */
బాహ్య vvp_సంకేతం_t codespace_allocate(శూన్య);
బాహ్య vvp_సంకేతం_t codespace_తదుపరి(శూన్య);
బాహ్య vvp_సంకేతం_t codespace_null(శూన్య);

#అయితే_ముగింపు /* IVL_codes_H */
