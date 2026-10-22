#اگر_غیر_تعریف_شدہ IVL_codes_H
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

قسمتعریف bool (*vvp_رمز_fun)(vthread_t thr, vvp_رمز_t رمز);

/*
 * These functions are implementations of executable op-codes. The
 * implementation lives in the vthread.cc file so that they have
 * access to the thread context.
 */
خارجی bool کا_ABS_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_جوڑو(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_جوڑو_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_ADDI(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_ALLOC(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_AND(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_ANDR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_تفویض_AR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_تفویض_ARD(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_تفویض_ARE(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_تفویض_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_تفویض_VEC4_D(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_تفویض_VEC4_E(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_تفویض_VEC4_A_D(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_تفویض_VEC4_A_E(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_تفویض_VEC4_OFF_D(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_تفویض_VEC4_OFF_E(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_تفویض_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_تفویض_WRD(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_تفویض_WRE(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_BLEND(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_BLEND_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_BLEND_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_BREAKPOINT(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CALLF_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CALLF_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CALLF_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CALLF_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CALLF_VOID(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CASSIGN_LINK(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CASSIGN_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CASSIGN_VEC4_OFF(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CASSIGN_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CAST2(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CAST_VEC2_DAR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CAST_VEC4_DAR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CAST_VEC4_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPE(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPIE(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPINE(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPNE(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPS(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPIS(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPSTR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPU(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPIU(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPWE(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPWNE(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPWR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPX(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPZ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CONCAT_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CONCATI_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CONCAT_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CONCATI_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CVT_RV(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CVT_RV_S(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CVT_SR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CVT_UR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CVT_VR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DEASSIGN(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DEASSIGN_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DEBUG_THR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DELAY(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DELAYX(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DELETE_ELEM(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DELETE_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DELETE_دم(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DISABLE(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DISABLE_FLOW(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DISABLE_FORK(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DIV(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DIV_S(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DIV_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DUP_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DUP_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DUP_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_انتہا(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_EVENT(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_EVENT_NB(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_EVCTL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_EVCTLC(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_EVCTLI(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_EVCTLS(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_FILE_سطر(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_FLAG_GET_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_FLAG_INV(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_FLAG_MOV(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_FLAG_OR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_FLAG_سیٹ_IMM(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_FLAG_سیٹ_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_FORCE_LINK(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_FORCE_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_FORCE_VEC4_OFF(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_FORCE_VEC4_OFF_D(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_FORCE_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_FORK(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_FREE(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_INV(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_IX_جوڑو(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_IX_GETV(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_IX_GETV_S(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_IX_بھرو(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_IX_MOV(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_IX_MUL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_IX_SUB(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_IX_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_IX_VEC4_S(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_JMP(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_JMP0(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_JMP0_XZ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_JMP1(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_JMP1_XZ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_JOIN(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_JOIN_DETACH(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_بھرو_AR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_بھرو_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_بھرو_DAR_R(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_بھرو_DAR_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_بھرو_DAR_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_بھرو_PROP_DAR_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_بھرو_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_بھرو_OBJA(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_بھرو_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_بھرو_STRA(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_بھرو_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_بھرو_VEC4_A(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_MAX_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_MIN_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_MOD(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_MOD_S(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_MOD_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_MUL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_MULI(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_MUL_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_NEG_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_NAND(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_NANDR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_NEW_COBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_NEW_DARRAY(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_NOOP(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_NOR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_NORR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_NULL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_OR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_ORR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PAD_S(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PAD_U(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PART_S(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PART_U(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PARTI_S(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PARTI_U(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_نکالو_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_نکالو_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_نکالو_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_نکالو_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_POW(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_POW_S(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_POW_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QINSERT_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QINSERT_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QINSERT_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QPOP_B_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QPOP_B_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QPOP_B_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QPOP_F_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QPOP_F_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QPOP_F_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PROP_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PROP_R(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PROP_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PUSHI_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PUSHI_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PUSHI_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PUSHV_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PUTC_STR_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_RELEASE_NET(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_RELEASE_REG(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_RELEASE_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_REPLICATE(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_RET_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_RET_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_RET_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_الٹ_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_الٹ_PROP_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_RSORT_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_RSORT_PROP_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_SHUFFLE_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_ترتیب_دو_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_ترتیب_دو_PROP_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_RETLOAD_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_RETLOAD_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_RETLOAD_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_SCOPY(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_سیٹ_DAR_OBJ_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_سیٹ_DAR_OBJ_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_سیٹ_DAR_OBJ_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_SHIFTL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_SHIFTR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_SHIFTR_S(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_بانٹو_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_DAR_R(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_DAR_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_DAR_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_OBJA(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_PROP_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_PROP_R(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_PROP_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_PROP_QB_R(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_PROP_QB_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_PROP_QB_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_PROP_QF_R(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_PROP_QF_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_PROP_QF_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QINSERT_PROP_R(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QINSERT_PROP_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QINSERT_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QPOP_PROP_B_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QPOP_PROP_B_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QPOP_PROP_B_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QPOP_PROP_F_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QPOP_PROP_F_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_QPOP_PROP_F_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_PROP_قطار_سائز(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_APPEND_لفظ_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_NEW_خالی_خانے_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_سائز_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_لفظ_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_لفظ_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPIX_LTU(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_CMPIX_SLT0(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_ڈالو_IX_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_ڈھونڈو_اشاریہ_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_ڈھونڈو_اشاریہ_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_ڈھونڈو_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_ڈھونڈو_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_ڈھونڈو_پہلا_اشاریہ_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_ڈھونڈو_پہلا_اشاریہ_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_ڈھونڈو_پہلا_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_ڈھونڈو_پہلا_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_ڈھونڈو_آخری_اشاریہ_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_ڈھونڈو_آخری_اشاریہ_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_ڈھونڈو_آخری_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_ڈھونڈو_آخری_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_MIN_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_MIN_OBJ_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_MIN_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_MAX_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_MAX_OBJ_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_MAX_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_UNIQUE_OBJ_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_UNIQUE_اشاریہ_OBJ_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_UNIQUE_اشاریہ_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_UNIQUE_اشاریہ_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_UNIQUE_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_قطار_UNIQUE_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DELETE_PROP_ELEM(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_DELETE_PROP_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_QB_R(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_QB_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_QB_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_QDAR_R(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_QDAR_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_QDAR_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_QF_R(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_QF_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_QF_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_QOBJ_R(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_QOBJ_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_QOBJ_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_REALA(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_STR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_STRA(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_STORE_VEC4_A(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_SUB(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_SUBI(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_SUB_WR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_SUBSTR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_SUBSTR_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_TEST_NUL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_TEST_NUL_A(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_TEST_NUL_OBJ(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_TEST_NUL_PROP(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_UPARR_AND_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_UPARR_OR_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_UPARR_حاصل_ضرب_OBJ_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_UPARR_حاصل_ضرب_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_UPARR_حاصل_ضرب_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_UPARR_حاصل_ضرب_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_UPARR_مجموع_OBJ_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_UPARR_مجموع_PROP_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_UPARR_مجموع_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_UPARR_مجموع_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_UPARR_XOR_V(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_VPI_CALL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_WAIT(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_WAIT_FORK(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_XNOR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_XNORR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_XOR(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_XORR(vthread_t thr, vvp_رمز_t رمز);

خارجی bool کا_ZOMBIE(vthread_t thr, vvp_رمز_t رمز);

خارجی bool کا_EXEC_UFUNC_REAL(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_EXEC_UFUNC_VEC4(vthread_t thr, vvp_رمز_t رمز);
خارجی bool کا_REAP_UFUNC(vthread_t thr, vvp_رمز_t رمز);

خارجی bool کا_CHUNK_LINK(vthread_t thr, vvp_رمز_t رمز);

/*
 * This is the format of a machine code instruction.
 */
ساخت vvp_رمز_s {
      vvp_رمز_fun opcode;

      اتحاد {
	    غیرعلامتی طویل نمبر;
	    vvp_net_t    *net;
	    vvp_رمز_t   cptr;
	    vvp_array_t array;
	    class __vpiHandle*handle;
	    __vpiScope*scope;
	    ثابت حرف*متن;
      };

      اتحاد {
	    uint32_t    ثنائی_ہندسہ_idx[2];
	    vvp_net_t   *net2;
	    vvp_رمز_t   cptr2;
	    class ufunc_core*ufunc_core_ptr;
      };
};

/*
 * This function clears the code space, ready for initialization. This
 * needs to be done exactly once before any instructions are created.
 */
خارجی خالی codespace_init(خالی);


/*
 * This function returns a pointer to the next free instruction in the
 * code address space. The codespace_next returns a pointer to the
 * next opcode that will be allocated. This is used by label
 * statements to get the address that will be attached to a label in
 * the code.
 */
خارجی vvp_رمز_t codespace_allocate(خالی);
خارجی vvp_رمز_t codespace_اگلا(خالی);
خارجی vvp_رمز_t codespace_null(خالی);

#اگر_ختم /* IVL_codes_H */
