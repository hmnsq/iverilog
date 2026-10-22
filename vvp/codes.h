#که_نه_تعریف_شوی IVL_codes_H
#تعریف_کړه IVL_codes_H
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

تعريفنوع bool (*vvp_کوډ_fun)(vthread_t thr, vvp_کوډ_t کوډ);

/*
 * These functions are implementations of executable op-codes. The
 * implementation lives in the vthread.cc file so that they have
 * access to the thread context.
 */
خارجي bool د_ABS_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_جمع_کړه(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_جمع_کړه_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ADDI(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ALLOC(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_AND(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ANDR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ټاکل_AR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ټاکل_ARD(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ټاکل_ARE(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ټاکل_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ټاکل_VEC4_D(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ټاکل_VEC4_E(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ټاکل_VEC4_A_D(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ټاکل_VEC4_A_E(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ټاکل_VEC4_OFF_D(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ټاکل_VEC4_OFF_E(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ټاکل_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ټاکل_WRD(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ټاکل_WRE(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_BLEND(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_BLEND_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_BLEND_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_BREAKPOINT(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CALLF_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CALLF_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CALLF_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CALLF_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CALLF_VOID(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CASSIGN_LINK(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CASSIGN_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CASSIGN_VEC4_OFF(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CASSIGN_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CAST2(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CAST_VEC2_DAR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CAST_VEC4_DAR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CAST_VEC4_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPE(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPIE(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPINE(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPNE(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPS(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPIS(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPSTR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPU(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPIU(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPWE(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPWNE(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPWR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPX(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPZ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CONCAT_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CONCATI_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CONCAT_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CONCATI_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CVT_RV(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CVT_RV_S(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CVT_SR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CVT_UR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CVT_VR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DEASSIGN(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DEASSIGN_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DEBUG_THR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DELAY(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DELAYX(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DELETE_ELEM(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DELETE_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DELETE_لکۍ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DISABLE(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DISABLE_FLOW(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DISABLE_FORK(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DIV(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DIV_S(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DIV_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DUP_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DUP_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DUP_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_پای_ټکی(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_EVENT(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_EVENT_NB(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_EVCTL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_EVCTLC(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_EVCTLI(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_EVCTLS(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_FILE_کرښه(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_FLAG_GET_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_FLAG_INV(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_FLAG_MOV(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_FLAG_OR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_FLAG_سېټ_IMM(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_FLAG_سېټ_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_FORCE_LINK(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_FORCE_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_FORCE_VEC4_OFF(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_FORCE_VEC4_OFF_D(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_FORCE_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_FORK(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_FREE(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_INV(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_IX_جمع_کړه(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_IX_GETV(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_IX_GETV_S(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_IX_بارول(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_IX_MOV(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_IX_MUL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_IX_SUB(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_IX_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_IX_VEC4_S(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_JMP(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_JMP0(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_JMP0_XZ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_JMP1(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_JMP1_XZ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_JOIN(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_JOIN_DETACH(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_بارول_AR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_بارول_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_بارول_DAR_R(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_بارول_DAR_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_بارول_DAR_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_بارول_PROP_DAR_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_بارول_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_بارول_OBJA(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_بارول_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_بارول_STRA(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_بارول_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_بارول_VEC4_A(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_MAX_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_MIN_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_MOD(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_MOD_S(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_MOD_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_MUL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_MULI(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_MUL_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_NEG_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_NAND(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_NANDR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_NEW_COBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_NEW_DARRAY(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_NOOP(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_NOR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_NORR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_NULL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_OR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ORR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PAD_S(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PAD_U(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PART_S(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PART_U(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PARTI_S(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PARTI_U(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_راوباسه_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_راوباسه_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_راوباسه_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_راوباسه_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_POW(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_POW_S(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_POW_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QINSERT_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QINSERT_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QINSERT_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QPOP_B_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QPOP_B_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QPOP_B_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QPOP_F_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QPOP_F_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QPOP_F_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PROP_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PROP_R(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PROP_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PUSHI_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PUSHI_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PUSHI_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PUSHV_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PUTC_STR_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_RELEASE_NET(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_RELEASE_REG(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_RELEASE_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_REPLICATE(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_RET_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_RET_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_RET_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_سرچپه_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_سرچپه_PROP_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_RSORT_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_RSORT_PROP_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_SHUFFLE_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ترتیبول_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ترتیبول_PROP_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_RETLOAD_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_RETLOAD_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_RETLOAD_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_SCOPY(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_سېټ_DAR_OBJ_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_سېټ_DAR_OBJ_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_سېټ_DAR_OBJ_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_SHIFTL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_SHIFTR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_SHIFTR_S(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_وېشل_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_DAR_R(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_DAR_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_DAR_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_OBJA(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_PROP_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_PROP_R(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_PROP_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_PROP_QB_R(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_PROP_QB_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_PROP_QB_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_PROP_QF_R(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_PROP_QF_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_PROP_QF_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QINSERT_PROP_R(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QINSERT_PROP_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QINSERT_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QPOP_PROP_B_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QPOP_PROP_B_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QPOP_PROP_B_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QPOP_PROP_F_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QPOP_PROP_F_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_QPOP_PROP_F_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_PROP_کتار_اندازه(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_APPEND_کلمه_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_NEW_تشه_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_اندازه_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_کلمه_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_کلمه_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPIX_LTU(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_CMPIX_SLT0(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_ټېل_IX_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_پیدا_کړه_شاخص_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_پیدا_کړه_شاخص_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_پیدا_کړه_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_پیدا_کړه_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_پیدا_کړه_لومړی_شاخص_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_پیدا_کړه_لومړی_شاخص_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_پیدا_کړه_لومړی_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_پیدا_کړه_لومړی_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_پیدا_کړه_وروستی_شاخص_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_پیدا_کړه_وروستی_شاخص_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_پیدا_کړه_وروستی_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_پیدا_کړه_وروستی_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_MIN_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_MIN_OBJ_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_MIN_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_MAX_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_MAX_OBJ_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_MAX_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_UNIQUE_OBJ_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_UNIQUE_شاخص_OBJ_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_UNIQUE_شاخص_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_UNIQUE_شاخص_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_UNIQUE_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_کتار_UNIQUE_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DELETE_PROP_ELEM(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_DELETE_PROP_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_QB_R(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_QB_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_QB_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_QDAR_R(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_QDAR_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_QDAR_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_QF_R(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_QF_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_QF_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_QOBJ_R(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_QOBJ_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_QOBJ_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_REALA(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_STR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_STRA(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_STORE_VEC4_A(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_SUB(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_SUBI(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_SUB_WR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_SUBSTR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_SUBSTR_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_TEST_NUL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_TEST_NUL_A(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_TEST_NUL_OBJ(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_TEST_NUL_PROP(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_UPARR_AND_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_UPARR_OR_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_UPARR_حاصل_ضرب_OBJ_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_UPARR_حاصل_ضرب_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_UPARR_حاصل_ضرب_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_UPARR_حاصل_ضرب_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_UPARR_مجموعه_OBJ_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_UPARR_مجموعه_PROP_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_UPARR_مجموعه_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_UPARR_مجموعه_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_UPARR_XOR_V(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_VPI_CALL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_WAIT(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_WAIT_FORK(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_XNOR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_XNORR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_XOR(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_XORR(vthread_t thr, vvp_کوډ_t کوډ);

خارجي bool د_ZOMBIE(vthread_t thr, vvp_کوډ_t کوډ);

خارجي bool د_EXEC_UFUNC_REAL(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_EXEC_UFUNC_VEC4(vthread_t thr, vvp_کوډ_t کوډ);
خارجي bool د_REAP_UFUNC(vthread_t thr, vvp_کوډ_t کوډ);

خارجي bool د_CHUNK_LINK(vthread_t thr, vvp_کوډ_t کوډ);

/*
 * This is the format of a machine code instruction.
 */
بنية vvp_کوډ_s {
      vvp_کوډ_fun opcode;

      اتحاد {
	    بینښه اوږد شمېره;
	    vvp_net_t    *net;
	    vvp_کوډ_t   cptr;
	    vvp_array_t array;
	    class __vpiHandle*handle;
	    __vpiScope*scope;
	    ثابت حرف*متن;
      };

      اتحاد {
	    uint32_t    بټ_idx[2];
	    vvp_net_t   *net2;
	    vvp_کوډ_t   cptr2;
	    class ufunc_core*ufunc_core_ptr;
      };
};

/*
 * This function clears the code space, ready for initialization. This
 * needs to be done exactly once before any instructions are created.
 */
خارجي فراغ codespace_init(فراغ);


/*
 * This function returns a pointer to the next free instruction in the
 * code address space. The codespace_next returns a pointer to the
 * next opcode that will be allocated. This is used by label
 * statements to get the address that will be attached to a label in
 * the code.
 */
خارجي vvp_کوډ_t codespace_allocate(فراغ);
خارجي vvp_کوډ_t codespace_بل(فراغ);
خارجي vvp_کوډ_t codespace_null(فراغ);

#د_که_پای /* IVL_codes_H */
