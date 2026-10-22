#جے_بنا_تعریف IVL_codes_H
#تعریف_کرو IVL_codes_H
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

تعريفنوع bool (*vvp_کوڈ_fun)(vthread_t thr, vvp_کوڈ_t کوڈ);

/*
 * These functions are implementations of executable op-codes. The
 * implementation lives in the vthread.cc file so that they have
 * access to the thread context.
 */
خارجي bool دا_ABS_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_جوڑو(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_جوڑو_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_ADDI(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_ALLOC(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_AND(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_ANDR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سونپو_AR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سونپو_ARD(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سونپو_ARE(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سونپو_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سونپو_VEC4_D(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سونپو_VEC4_E(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سونپو_VEC4_A_D(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سونپو_VEC4_A_E(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سونپو_VEC4_OFF_D(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سونپو_VEC4_OFF_E(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سونپو_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سونپو_WRD(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سونپو_WRE(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_BLEND(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_BLEND_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_BLEND_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_BREAKPOINT(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CALLF_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CALLF_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CALLF_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CALLF_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CALLF_VOID(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CASSIGN_LINK(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CASSIGN_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CASSIGN_VEC4_OFF(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CASSIGN_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CAST2(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CAST_VEC2_DAR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CAST_VEC4_DAR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CAST_VEC4_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPE(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPIE(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPINE(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPNE(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPS(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPIS(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPSTR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPU(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPIU(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPWE(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPWNE(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPWR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPX(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPZ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CONCAT_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CONCATI_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CONCAT_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CONCATI_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CVT_RV(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CVT_RV_S(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CVT_SR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CVT_UR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CVT_VR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DEASSIGN(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DEASSIGN_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DEBUG_THR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DELAY(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DELAYX(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DELETE_ELEM(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DELETE_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DELETE_پوچھ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DISABLE(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DISABLE_FLOW(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DISABLE_FORK(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DIV(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DIV_S(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DIV_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DUP_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DUP_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DUP_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سرا(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_EVENT(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_EVENT_NB(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_EVCTL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_EVCTLC(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_EVCTLI(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_EVCTLS(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_FILE_سطر(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_FLAG_GET_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_FLAG_INV(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_FLAG_MOV(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_FLAG_OR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_FLAG_سیٹ_IMM(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_FLAG_سیٹ_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_FORCE_LINK(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_FORCE_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_FORCE_VEC4_OFF(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_FORCE_VEC4_OFF_D(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_FORCE_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_FORK(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_FREE(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_INV(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_IX_جوڑو(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_IX_GETV(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_IX_GETV_S(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_IX_لدو(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_IX_MOV(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_IX_MUL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_IX_SUB(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_IX_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_IX_VEC4_S(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_JMP(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_JMP0(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_JMP0_XZ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_JMP1(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_JMP1_XZ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_JOIN(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_JOIN_DETACH(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_لدو_AR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_لدو_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_لدو_DAR_R(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_لدو_DAR_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_لدو_DAR_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_لدو_PROP_DAR_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_لدو_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_لدو_OBJA(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_لدو_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_لدو_STRA(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_لدو_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_لدو_VEC4_A(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_MAX_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_MIN_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_MOD(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_MOD_S(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_MOD_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_MUL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_MULI(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_MUL_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_NEG_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_NAND(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_NANDR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_NEW_COBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_NEW_DARRAY(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_NOOP(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_NOR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_NORR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_NULL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_OR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_ORR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PAD_S(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PAD_U(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PART_S(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PART_U(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PARTI_S(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PARTI_U(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_نکالو_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_نکالو_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_نکالو_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_نکالو_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_POW(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_POW_S(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_POW_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QINSERT_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QINSERT_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QINSERT_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QPOP_B_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QPOP_B_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QPOP_B_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QPOP_F_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QPOP_F_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QPOP_F_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PROP_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PROP_R(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PROP_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PUSHI_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PUSHI_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PUSHI_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PUSHV_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PUTC_STR_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_RELEASE_NET(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_RELEASE_REG(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_RELEASE_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_REPLICATE(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_RET_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_RET_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_RET_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_الٹ_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_الٹ_PROP_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_RSORT_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_RSORT_PROP_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_SHUFFLE_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_ترتیب_دیو_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_ترتیب_دیو_PROP_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_RETLOAD_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_RETLOAD_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_RETLOAD_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_SCOPY(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سیٹ_DAR_OBJ_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سیٹ_DAR_OBJ_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_سیٹ_DAR_OBJ_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_SHIFTL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_SHIFTR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_SHIFTR_S(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_ونڈو_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_DAR_R(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_DAR_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_DAR_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_OBJA(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_PROP_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_PROP_R(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_PROP_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_PROP_QB_R(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_PROP_QB_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_PROP_QB_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_PROP_QF_R(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_PROP_QF_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_PROP_QF_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QINSERT_PROP_R(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QINSERT_PROP_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QINSERT_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QPOP_PROP_B_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QPOP_PROP_B_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QPOP_PROP_B_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QPOP_PROP_F_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QPOP_PROP_F_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_QPOP_PROP_F_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_PROP_قطار_ناپ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_APPEND_لفظ_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_NEW_خالی_تھاواں_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_ناپ_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_لفظ_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_لفظ_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPIX_LTU(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_CMPIX_SLT0(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_ڈالو_IX_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_لبھو_اشاریہ_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_لبھو_اشاریہ_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_لبھو_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_لبھو_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_لبھو_پہلا_اشاریہ_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_لبھو_پہلا_اشاریہ_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_لبھو_پہلا_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_لبھو_پہلا_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_لبھو_آخری_اشاریہ_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_لبھو_آخری_اشاریہ_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_لبھو_آخری_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_لبھو_آخری_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_MIN_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_MIN_OBJ_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_MIN_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_MAX_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_MAX_OBJ_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_MAX_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_UNIQUE_OBJ_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_UNIQUE_اشاریہ_OBJ_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_UNIQUE_اشاریہ_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_UNIQUE_اشاریہ_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_UNIQUE_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_قطار_UNIQUE_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DELETE_PROP_ELEM(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_DELETE_PROP_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_QB_R(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_QB_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_QB_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_QDAR_R(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_QDAR_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_QDAR_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_QF_R(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_QF_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_QF_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_QOBJ_R(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_QOBJ_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_QOBJ_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_REALA(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_STR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_STRA(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_STORE_VEC4_A(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_SUB(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_SUBI(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_SUB_WR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_SUBSTR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_SUBSTR_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_TEST_NUL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_TEST_NUL_A(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_TEST_NUL_OBJ(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_TEST_NUL_PROP(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_UPARR_AND_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_UPARR_OR_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_UPARR_ضرب_حاصل_OBJ_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_UPARR_ضرب_حاصل_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_UPARR_ضرب_حاصل_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_UPARR_ضرب_حاصل_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_UPARR_جوڑ_OBJ_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_UPARR_جوڑ_PROP_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_UPARR_جوڑ_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_UPARR_جوڑ_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_UPARR_XOR_V(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_VPI_CALL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_WAIT(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_WAIT_FORK(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_XNOR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_XNORR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_XOR(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_XORR(vthread_t thr, vvp_کوڈ_t کوڈ);

خارجي bool دا_ZOMBIE(vthread_t thr, vvp_کوڈ_t کوڈ);

خارجي bool دا_EXEC_UFUNC_REAL(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_EXEC_UFUNC_VEC4(vthread_t thr, vvp_کوڈ_t کوڈ);
خارجي bool دا_REAP_UFUNC(vthread_t thr, vvp_کوڈ_t کوڈ);

خارجي bool دا_CHUNK_LINK(vthread_t thr, vvp_کوڈ_t کوڈ);

/*
 * This is the format of a machine code instruction.
 */
بنية vvp_کوڈ_s {
      vvp_کوڈ_fun opcode;

      اتحاد {
	    بنانشانی لما نمبر;
	    vvp_net_t    *net;
	    vvp_کوڈ_t   cptr;
	    vvp_array_t array;
	    class __vpiHandle*handle;
	    __vpiScope*scope;
	    ثابت حرف*متن;
      };

      اتحاد {
	    uint32_t    ثنائی_ہندسہ_idx[2];
	    vvp_net_t   *net2;
	    vvp_کوڈ_t   cptr2;
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
خارجي vvp_کوڈ_t codespace_allocate(فراغ);
خارجي vvp_کوڈ_t codespace_اگلا(فراغ);
خارجي vvp_کوڈ_t codespace_null(فراغ);

#جے_انت /* IVL_codes_H */
