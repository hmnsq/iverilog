#اگر_تعریف_نشده IVL_codes_H
#تعریف_کن IVL_codes_H
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

تعريفنوع bool (*vvp_کد_fun)(vthread_t thr, vvp_کد_t کد);

/*
 * These functions are implementations of executable op-codes. The
 * implementation lives in the vthread.cc file so that they have
 * access to the thread context.
 */
خارجي bool از_ABS_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_جمع_کن(vthread_t thr, vvp_کد_t کد);
خارجي bool از_جمع_کن_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_ADDI(vthread_t thr, vvp_کد_t کد);
خارجي bool از_ALLOC(vthread_t thr, vvp_کد_t کد);
خارجي bool از_AND(vthread_t thr, vvp_کد_t کد);
خارجي bool از_ANDR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_تخصیص_بده_AR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_تخصیص_بده_ARD(vthread_t thr, vvp_کد_t کد);
خارجي bool از_تخصیص_بده_ARE(vthread_t thr, vvp_کد_t کد);
خارجي bool از_تخصیص_بده_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_تخصیص_بده_VEC4_D(vthread_t thr, vvp_کد_t کد);
خارجي bool از_تخصیص_بده_VEC4_E(vthread_t thr, vvp_کد_t کد);
خارجي bool از_تخصیص_بده_VEC4_A_D(vthread_t thr, vvp_کد_t کد);
خارجي bool از_تخصیص_بده_VEC4_A_E(vthread_t thr, vvp_کد_t کد);
خارجي bool از_تخصیص_بده_VEC4_OFF_D(vthread_t thr, vvp_کد_t کد);
خارجي bool از_تخصیص_بده_VEC4_OFF_E(vthread_t thr, vvp_کد_t کد);
خارجي bool از_تخصیص_بده_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_تخصیص_بده_WRD(vthread_t thr, vvp_کد_t کد);
خارجي bool از_تخصیص_بده_WRE(vthread_t thr, vvp_کد_t کد);
خارجي bool از_BLEND(vthread_t thr, vvp_کد_t کد);
خارجي bool از_BLEND_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_BLEND_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_BREAKPOINT(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CALLF_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CALLF_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CALLF_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CALLF_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CALLF_VOID(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CASSIGN_LINK(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CASSIGN_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CASSIGN_VEC4_OFF(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CASSIGN_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CAST2(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CAST_VEC2_DAR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CAST_VEC4_DAR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CAST_VEC4_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPE(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPIE(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPINE(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPNE(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPS(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPIS(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPSTR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPU(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPIU(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPWE(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPWNE(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPWR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPX(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPZ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CONCAT_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CONCATI_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CONCAT_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CONCATI_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CVT_RV(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CVT_RV_S(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CVT_SR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CVT_UR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CVT_VR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DEASSIGN(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DEASSIGN_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DEBUG_THR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DELAY(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DELAYX(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DELETE_ELEM(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DELETE_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DELETE_دم(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DISABLE(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DISABLE_FLOW(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DISABLE_FORK(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DIV(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DIV_S(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DIV_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DUP_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DUP_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DUP_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_انتها(vthread_t thr, vvp_کد_t کد);
خارجي bool از_EVENT(vthread_t thr, vvp_کد_t کد);
خارجي bool از_EVENT_NB(vthread_t thr, vvp_کد_t کد);
خارجي bool از_EVCTL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_EVCTLC(vthread_t thr, vvp_کد_t کد);
خارجي bool از_EVCTLI(vthread_t thr, vvp_کد_t کد);
خارجي bool از_EVCTLS(vthread_t thr, vvp_کد_t کد);
خارجي bool از_FILE_سطر(vthread_t thr, vvp_کد_t کد);
خارجي bool از_FLAG_GET_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_FLAG_INV(vthread_t thr, vvp_کد_t کد);
خارجي bool از_FLAG_MOV(vthread_t thr, vvp_کد_t کد);
خارجي bool از_FLAG_OR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_FLAG_مجموعه_IMM(vthread_t thr, vvp_کد_t کد);
خارجي bool از_FLAG_مجموعه_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_FORCE_LINK(vthread_t thr, vvp_کد_t کد);
خارجي bool از_FORCE_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_FORCE_VEC4_OFF(vthread_t thr, vvp_کد_t کد);
خارجي bool از_FORCE_VEC4_OFF_D(vthread_t thr, vvp_کد_t کد);
خارجي bool از_FORCE_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_FORK(vthread_t thr, vvp_کد_t کد);
خارجي bool از_FREE(vthread_t thr, vvp_کد_t کد);
خارجي bool از_INV(vthread_t thr, vvp_کد_t کد);
خارجي bool از_IX_جمع_کن(vthread_t thr, vvp_کد_t کد);
خارجي bool از_IX_GETV(vthread_t thr, vvp_کد_t کد);
خارجي bool از_IX_GETV_S(vthread_t thr, vvp_کد_t کد);
خارجي bool از_IX_بارگیری(vthread_t thr, vvp_کد_t کد);
خارجي bool از_IX_MOV(vthread_t thr, vvp_کد_t کد);
خارجي bool از_IX_MUL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_IX_SUB(vthread_t thr, vvp_کد_t کد);
خارجي bool از_IX_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_IX_VEC4_S(vthread_t thr, vvp_کد_t کد);
خارجي bool از_JMP(vthread_t thr, vvp_کد_t کد);
خارجي bool از_JMP0(vthread_t thr, vvp_کد_t کد);
خارجي bool از_JMP0_XZ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_JMP1(vthread_t thr, vvp_کد_t کد);
خارجي bool از_JMP1_XZ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_JOIN(vthread_t thr, vvp_کد_t کد);
خارجي bool از_JOIN_DETACH(vthread_t thr, vvp_کد_t کد);
خارجي bool از_بارگیری_AR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_بارگیری_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_بارگیری_DAR_R(vthread_t thr, vvp_کد_t کد);
خارجي bool از_بارگیری_DAR_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_بارگیری_DAR_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_بارگیری_PROP_DAR_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_بارگیری_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_بارگیری_OBJA(vthread_t thr, vvp_کد_t کد);
خارجي bool از_بارگیری_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_بارگیری_STRA(vthread_t thr, vvp_کد_t کد);
خارجي bool از_بارگیری_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_بارگیری_VEC4_A(vthread_t thr, vvp_کد_t کد);
خارجي bool از_MAX_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_MIN_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_MOD(vthread_t thr, vvp_کد_t کد);
خارجي bool از_MOD_S(vthread_t thr, vvp_کد_t کد);
خارجي bool از_MOD_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_MUL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_MULI(vthread_t thr, vvp_کد_t کد);
خارجي bool از_MUL_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_NEG_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_NAND(vthread_t thr, vvp_کد_t کد);
خارجي bool از_NANDR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_NEW_COBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_NEW_DARRAY(vthread_t thr, vvp_کد_t کد);
خارجي bool از_NOOP(vthread_t thr, vvp_کد_t کد);
خارجي bool از_NOR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_NORR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_NULL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_OR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_ORR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PAD_S(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PAD_U(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PART_S(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PART_U(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PARTI_S(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PARTI_U(vthread_t thr, vvp_کد_t کد);
خارجي bool از_برداشت_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_برداشت_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_برداشت_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_برداشت_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_POW(vthread_t thr, vvp_کد_t کد);
خارجي bool از_POW_S(vthread_t thr, vvp_کد_t کد);
خارجي bool از_POW_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QINSERT_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QINSERT_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QINSERT_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QPOP_B_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QPOP_B_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QPOP_B_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QPOP_F_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QPOP_F_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QPOP_F_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PROP_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PROP_R(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PROP_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PUSHI_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PUSHI_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PUSHI_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PUSHV_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PUTC_STR_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_RELEASE_NET(vthread_t thr, vvp_کد_t کد);
خارجي bool از_RELEASE_REG(vthread_t thr, vvp_کد_t کد);
خارجي bool از_RELEASE_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_REPLICATE(vthread_t thr, vvp_کد_t کد);
خارجي bool از_RET_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_RET_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_RET_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_معکوس_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_معکوس_PROP_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_RSORT_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_RSORT_PROP_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_SHUFFLE_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_مرتب_سازی_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_مرتب_سازی_PROP_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_RETLOAD_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_RETLOAD_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_RETLOAD_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_SCOPY(vthread_t thr, vvp_کد_t کد);
خارجي bool از_مجموعه_DAR_OBJ_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_مجموعه_DAR_OBJ_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_مجموعه_DAR_OBJ_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_SHIFTL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_SHIFTR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_SHIFTR_S(vthread_t thr, vvp_کد_t کد);
خارجي bool از_تقسیم_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_DAR_R(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_DAR_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_DAR_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_OBJA(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_PROP_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_PROP_R(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_PROP_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_PROP_QB_R(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_PROP_QB_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_PROP_QB_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_PROP_QF_R(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_PROP_QF_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_PROP_QF_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QINSERT_PROP_R(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QINSERT_PROP_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QINSERT_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QPOP_PROP_B_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QPOP_PROP_B_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QPOP_PROP_B_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QPOP_PROP_F_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QPOP_PROP_F_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_QPOP_PROP_F_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_PROP_صف_اندازه(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_APPEND_کلمه_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_NEW_خالی_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_اندازه_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_کلمه_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_کلمه_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPIX_LTU(vthread_t thr, vvp_کد_t کد);
خارجي bool از_CMPIX_SLT0(vthread_t thr, vvp_کد_t کد);
خارجي bool از_افزودن_IX_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_پیدا_کن_شاخص_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_پیدا_کن_شاخص_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_پیدا_کن_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_پیدا_کن_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_پیدا_کن_اولی_شاخص_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_پیدا_کن_اولی_شاخص_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_پیدا_کن_اولی_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_پیدا_کن_اولی_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_پیدا_کن_آخرین_شاخص_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_پیدا_کن_آخرین_شاخص_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_پیدا_کن_آخرین_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_پیدا_کن_آخرین_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_MIN_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_MIN_OBJ_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_MIN_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_MAX_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_MAX_OBJ_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_MAX_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_UNIQUE_OBJ_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_UNIQUE_شاخص_OBJ_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_UNIQUE_شاخص_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_UNIQUE_شاخص_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_UNIQUE_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_صف_UNIQUE_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DELETE_PROP_ELEM(vthread_t thr, vvp_کد_t کد);
خارجي bool از_DELETE_PROP_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_QB_R(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_QB_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_QB_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_QDAR_R(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_QDAR_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_QDAR_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_QF_R(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_QF_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_QF_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_QOBJ_R(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_QOBJ_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_QOBJ_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_REALA(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_STR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_STRA(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_STORE_VEC4_A(vthread_t thr, vvp_کد_t کد);
خارجي bool از_SUB(vthread_t thr, vvp_کد_t کد);
خارجي bool از_SUBI(vthread_t thr, vvp_کد_t کد);
خارجي bool از_SUB_WR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_SUBSTR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_SUBSTR_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_TEST_NUL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_TEST_NUL_A(vthread_t thr, vvp_کد_t کد);
خارجي bool از_TEST_NUL_OBJ(vthread_t thr, vvp_کد_t کد);
خارجي bool از_TEST_NUL_PROP(vthread_t thr, vvp_کد_t کد);
خارجي bool از_UPARR_AND_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_UPARR_OR_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_UPARR_حاصلضرب_OBJ_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_UPARR_حاصلضرب_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_UPARR_حاصلضرب_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_UPARR_حاصلضرب_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_UPARR_جمع_OBJ_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_UPARR_جمع_PROP_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_UPARR_جمع_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_UPARR_جمع_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_UPARR_XOR_V(vthread_t thr, vvp_کد_t کد);
خارجي bool از_VPI_CALL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_WAIT(vthread_t thr, vvp_کد_t کد);
خارجي bool از_WAIT_FORK(vthread_t thr, vvp_کد_t کد);
خارجي bool از_XNOR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_XNORR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_XOR(vthread_t thr, vvp_کد_t کد);
خارجي bool از_XORR(vthread_t thr, vvp_کد_t کد);

خارجي bool از_ZOMBIE(vthread_t thr, vvp_کد_t کد);

خارجي bool از_EXEC_UFUNC_REAL(vthread_t thr, vvp_کد_t کد);
خارجي bool از_EXEC_UFUNC_VEC4(vthread_t thr, vvp_کد_t کد);
خارجي bool از_REAP_UFUNC(vthread_t thr, vvp_کد_t کد);

خارجي bool از_CHUNK_LINK(vthread_t thr, vvp_کد_t کد);

/*
 * This is the format of a machine code instruction.
 */
بنية vvp_کد_s {
      vvp_کد_fun opcode;

      اتحاد {
	    بیعلامت دراز عدد;
	    vvp_net_t    *net;
	    vvp_کد_t   cptr;
	    vvp_array_t array;
	    class __vpiHandle*handle;
	    __vpiScope*scope;
	    ثابت حرف*متن;
      };

      اتحاد {
	    uint32_t    بیت_idx[2];
	    vvp_net_t   *net2;
	    vvp_کد_t   cptr2;
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
خارجي vvp_کد_t codespace_allocate(فراغ);
خارجي vvp_کد_t codespace_بعدی(فراغ);
خارجي vvp_کد_t codespace_null(فراغ);

#پایان_اگر /* IVL_codes_H */
