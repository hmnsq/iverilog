#جيڪڏهن_وصف_نه_ٿيل IVL_codes_H
#وصف_ڪريو IVL_codes_H
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

تعريفنوع bool (*vvp_ڪوڊ_fun)(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);

/*
 * These functions are implementations of executable op-codes. The
 * implementation lives in the vthread.cc file so that they have
 * access to the thread context.
 */
خارجي bool جو_ABS_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_جوڙيو(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_جوڙيو_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ADDI(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ALLOC(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_AND(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ANDR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سونپيو_AR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سونپيو_ARD(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سونپيو_ARE(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سونپيو_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سونپيو_VEC4_D(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سونپيو_VEC4_E(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سونپيو_VEC4_A_D(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سونپيو_VEC4_A_E(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سونپيو_VEC4_OFF_D(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سونپيو_VEC4_OFF_E(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سونپيو_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سونپيو_WRD(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سونپيو_WRE(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_BLEND(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_BLEND_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_BLEND_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_BREAKPOINT(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CALLF_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CALLF_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CALLF_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CALLF_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CALLF_VOID(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CASSIGN_LINK(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CASSIGN_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CASSIGN_VEC4_OFF(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CASSIGN_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CAST2(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CAST_VEC2_DAR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CAST_VEC4_DAR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CAST_VEC4_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPE(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPIE(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPINE(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPNE(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPS(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPIS(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPSTR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPU(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPIU(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPWE(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPWNE(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPWR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPX(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPZ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CONCAT_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CONCATI_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CONCAT_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CONCATI_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CVT_RV(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CVT_RV_S(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CVT_SR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CVT_UR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CVT_VR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DEASSIGN(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DEASSIGN_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DEBUG_THR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DELAY(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DELAYX(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DELETE_ELEM(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DELETE_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DELETE_پڇ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DISABLE(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DISABLE_FLOW(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DISABLE_FORK(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DIV(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DIV_S(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DIV_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DUP_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DUP_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DUP_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_پڇاڙي(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_EVENT(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_EVENT_NB(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_EVCTL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_EVCTLC(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_EVCTLI(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_EVCTLS(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_FILE_سٽ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_FLAG_GET_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_FLAG_INV(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_FLAG_MOV(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_FLAG_OR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_FLAG_سيٽ_IMM(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_FLAG_سيٽ_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_FORCE_LINK(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_FORCE_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_FORCE_VEC4_OFF(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_FORCE_VEC4_OFF_D(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_FORCE_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_FORK(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_FREE(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_INV(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_IX_جوڙيو(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_IX_GETV(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_IX_GETV_S(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_IX_ڀريو(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_IX_MOV(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_IX_MUL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_IX_SUB(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_IX_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_IX_VEC4_S(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_JMP(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_JMP0(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_JMP0_XZ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_JMP1(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_JMP1_XZ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_JOIN(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_JOIN_DETACH(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڀريو_AR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڀريو_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڀريو_DAR_R(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڀريو_DAR_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڀريو_DAR_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڀريو_PROP_DAR_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڀريو_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڀريو_OBJA(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڀريو_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڀريو_STRA(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڀريو_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڀريو_VEC4_A(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_MAX_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_MIN_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_MOD(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_MOD_S(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_MOD_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_MUL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_MULI(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_MUL_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_NEG_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_NAND(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_NANDR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_NEW_COBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_NEW_DARRAY(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_NOOP(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_NOR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_NORR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_NULL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_OR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ORR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PAD_S(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PAD_U(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PART_S(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PART_U(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PARTI_S(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PARTI_U(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڪڍو_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڪڍو_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڪڍو_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڪڍو_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_POW(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_POW_S(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_POW_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QINSERT_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QINSERT_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QINSERT_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QPOP_B_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QPOP_B_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QPOP_B_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QPOP_F_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QPOP_F_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QPOP_F_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PROP_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PROP_R(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PROP_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PUSHI_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PUSHI_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PUSHI_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PUSHV_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PUTC_STR_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_RELEASE_NET(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_RELEASE_REG(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_RELEASE_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_REPLICATE(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_RET_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_RET_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_RET_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ابتو_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ابتو_PROP_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_RSORT_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_RSORT_PROP_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_SHUFFLE_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ترتيب_ڏيو_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ترتيب_ڏيو_PROP_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_RETLOAD_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_RETLOAD_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_RETLOAD_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_SCOPY(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سيٽ_DAR_OBJ_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سيٽ_DAR_OBJ_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_سيٽ_DAR_OBJ_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_SHIFTL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_SHIFTR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_SHIFTR_S(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ورهايو_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_DAR_R(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_DAR_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_DAR_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_OBJA(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_PROP_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_PROP_R(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_PROP_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_PROP_QB_R(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_PROP_QB_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_PROP_QB_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_PROP_QF_R(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_PROP_QF_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_PROP_QF_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QINSERT_PROP_R(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QINSERT_PROP_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QINSERT_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QPOP_PROP_B_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QPOP_PROP_B_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QPOP_PROP_B_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QPOP_PROP_F_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QPOP_PROP_F_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_QPOP_PROP_F_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_PROP_قطار_سائيز(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_APPEND_لفظ_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_NEW_خالي_جايون_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_سائيز_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_لفظ_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_لفظ_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPIX_LTU(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_CMPIX_SLT0(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_ڌڪو_IX_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_ڳوليو_اشاريو_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_ڳوليو_اشاريو_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_ڳوليو_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_ڳوليو_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_ڳوليو_پهريون_اشاريو_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_ڳوليو_پهريون_اشاريو_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_ڳوليو_پهريون_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_ڳوليو_پهريون_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_ڳوليو_آخري_اشاريو_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_ڳوليو_آخري_اشاريو_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_ڳوليو_آخري_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_ڳوليو_آخري_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_MIN_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_MIN_OBJ_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_MIN_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_MAX_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_MAX_OBJ_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_MAX_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_UNIQUE_OBJ_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_UNIQUE_اشاريو_OBJ_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_UNIQUE_اشاريو_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_UNIQUE_اشاريو_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_UNIQUE_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_قطار_UNIQUE_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DELETE_PROP_ELEM(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_DELETE_PROP_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_QB_R(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_QB_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_QB_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_QDAR_R(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_QDAR_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_QDAR_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_QF_R(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_QF_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_QF_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_QOBJ_R(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_QOBJ_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_QOBJ_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_REALA(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_STR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_STRA(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_STORE_VEC4_A(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_SUB(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_SUBI(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_SUB_WR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_SUBSTR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_SUBSTR_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_TEST_NUL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_TEST_NUL_A(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_TEST_NUL_OBJ(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_TEST_NUL_PROP(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_UPARR_AND_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_UPARR_OR_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_UPARR_حاصل_ضرب_OBJ_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_UPARR_حاصل_ضرب_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_UPARR_حاصل_ضرب_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_UPARR_حاصل_ضرب_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_UPARR_جوڙ_OBJ_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_UPARR_جوڙ_PROP_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_UPARR_جوڙ_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_UPARR_جوڙ_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_UPARR_XOR_V(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_VPI_CALL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_WAIT(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_WAIT_FORK(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_XNOR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_XNORR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_XOR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_XORR(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);

خارجي bool جو_ZOMBIE(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);

خارجي bool جو_EXEC_UFUNC_REAL(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_EXEC_UFUNC_VEC4(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);
خارجي bool جو_REAP_UFUNC(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);

خارجي bool جو_CHUNK_LINK(vthread_t thr, vvp_ڪوڊ_t ڪوڊ);

/*
 * This is the format of a machine code instruction.
 */
بنية vvp_ڪوڊ_s {
      vvp_ڪوڊ_fun opcode;

      اتحاد {
	    بنانشان ڊگهو عدد;
	    vvp_net_t    *net;
	    vvp_ڪوڊ_t   cptr;
	    vvp_array_t array;
	    class __vpiHandle*handle;
	    __vpiScope*scope;
	    ثابت حرف*متن;
      };

      اتحاد {
	    uint32_t    ٻٽو_انگ_idx[2];
	    vvp_net_t   *net2;
	    vvp_ڪوڊ_t   cptr2;
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
خارجي vvp_ڪوڊ_t codespace_allocate(فراغ);
خارجي vvp_ڪوڊ_t codespace_ايندڙ(فراغ);
خارجي vvp_ڪوڊ_t codespace_null(فراغ);

#جيڪڏهن_پورو /* IVL_codes_H */
