#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_codes_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_codes_H
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

ਕਿਸਮਪਰਿਭਾਸ਼ਾ bool (*vvp_ਕੋਡ_fun)(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);

/*
 * These functions are implementations of executable op-codes. The
 * implementation lives in the vthread.cc file so that they have
 * access to the thread context.
 */
ਬਾਹਰੀ bool ਦਾ_ABS_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਜੋੜੋ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਜੋੜੋ_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ADDI(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ALLOC(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_AND(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ANDR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੌਂਪੋ_AR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੌਂਪੋ_ARD(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੌਂਪੋ_ARE(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੌਂਪੋ_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੌਂਪੋ_VEC4_D(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੌਂਪੋ_VEC4_E(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੌਂਪੋ_VEC4_A_D(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੌਂਪੋ_VEC4_A_E(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੌਂਪੋ_VEC4_OFF_D(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੌਂਪੋ_VEC4_OFF_E(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੌਂਪੋ_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੌਂਪੋ_WRD(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੌਂਪੋ_WRE(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_BLEND(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_BLEND_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_BLEND_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_BREAKPOINT(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CALLF_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CALLF_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CALLF_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CALLF_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CALLF_VOID(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CASSIGN_LINK(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CASSIGN_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CASSIGN_VEC4_OFF(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CASSIGN_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CAST2(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CAST_VEC2_DAR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CAST_VEC4_DAR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CAST_VEC4_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPE(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPIE(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPINE(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPNE(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPS(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPIS(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPSTR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPU(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPIU(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPWE(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPWNE(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPWR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPX(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPZ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CONCAT_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CONCATI_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CONCAT_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CONCATI_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CVT_RV(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CVT_RV_S(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CVT_SR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CVT_UR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CVT_VR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DEASSIGN(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DEASSIGN_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DEBUG_THR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DELAY(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DELAYX(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DELETE_ELEM(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DELETE_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DELETE_ਪੂਛ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DISABLE(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DISABLE_FLOW(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DISABLE_FORK(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DIV(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DIV_S(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DIV_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DUP_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DUP_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DUP_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸਿਰਾ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_EVENT(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_EVENT_NB(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_EVCTL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_EVCTLC(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_EVCTLI(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_EVCTLS(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_FILE_ਸਤਰ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_FLAG_GET_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_FLAG_INV(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_FLAG_MOV(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_FLAG_OR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_FLAG_ਸੈੱਟ_IMM(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_FLAG_ਸੈੱਟ_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_FORCE_LINK(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_FORCE_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_FORCE_VEC4_OFF(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_FORCE_VEC4_OFF_D(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_FORCE_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_FORK(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_FREE(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_INV(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_IX_ਜੋੜੋ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_IX_GETV(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_IX_GETV_S(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_IX_ਲੱਦੋ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_IX_MOV(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_IX_MUL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_IX_SUB(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_IX_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_IX_VEC4_S(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_JMP(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_JMP0(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_JMP0_XZ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_JMP1(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_JMP1_XZ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_JOIN(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_JOIN_DETACH(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਲੱਦੋ_AR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਲੱਦੋ_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਲੱਦੋ_DAR_R(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਲੱਦੋ_DAR_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਲੱਦੋ_DAR_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਲੱਦੋ_PROP_DAR_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਲੱਦੋ_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਲੱਦੋ_OBJA(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਲੱਦੋ_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਲੱਦੋ_STRA(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਲੱਦੋ_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਲੱਦੋ_VEC4_A(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_MAX_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_MIN_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_MOD(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_MOD_S(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_MOD_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_MUL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_MULI(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_MUL_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_NEG_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_NAND(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_NANDR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_NEW_COBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_NEW_DARRAY(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_NOOP(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_NOR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_NORR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_NULL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_OR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ORR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PAD_S(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PAD_U(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PART_S(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PART_U(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PARTI_S(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PARTI_U(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕੱਢੋ_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕੱਢੋ_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕੱਢੋ_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕੱਢੋ_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_POW(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_POW_S(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_POW_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QINSERT_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QINSERT_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QINSERT_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QPOP_B_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QPOP_B_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QPOP_B_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QPOP_F_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QPOP_F_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QPOP_F_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PROP_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PROP_R(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PROP_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PUSHI_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PUSHI_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PUSHI_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PUSHV_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PUTC_STR_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_RELEASE_NET(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_RELEASE_REG(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_RELEASE_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_REPLICATE(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_RET_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_RET_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_RET_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਉਲਟ_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਉਲਟ_PROP_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_RSORT_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_RSORT_PROP_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_SHUFFLE_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਛਾਂਟੋ_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਛਾਂਟੋ_PROP_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_RETLOAD_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_RETLOAD_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_RETLOAD_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_SCOPY(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੈੱਟ_DAR_OBJ_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੈੱਟ_DAR_OBJ_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਸੈੱਟ_DAR_OBJ_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_SHIFTL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_SHIFTR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_SHIFTR_S(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਵੰਡੋ_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_DAR_R(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_DAR_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_DAR_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_OBJA(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_PROP_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_PROP_R(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_PROP_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_PROP_QB_R(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_PROP_QB_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_PROP_QB_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_PROP_QF_R(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_PROP_QF_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_PROP_QF_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QINSERT_PROP_R(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QINSERT_PROP_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QINSERT_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QPOP_PROP_B_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QPOP_PROP_B_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QPOP_PROP_B_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QPOP_PROP_F_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QPOP_PROP_F_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_QPOP_PROP_F_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_PROP_ਕਤਾਰ_ਮਾਪ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_APPEND_ਸ਼ਬਦ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_NEW_ਖਾਲੀ_ਥਾਵਾਂ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_ਮਾਪ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_ਸ਼ਬਦ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_ਸ਼ਬਦ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPIX_LTU(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_CMPIX_SLT0(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਧੱਕੋ_IX_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਸੂਚਕਾਂਕ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਸੂਚਕਾਂਕ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_ਲੱਭੋ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_ਲੱਭੋ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਪਹਿਲਾ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਆਖਰੀ_ਸੂਚਕਾਂਕ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਆਖਰੀ_ਸੂਚਕਾਂਕ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਆਖਰੀ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_ਲੱਭੋ_ਆਖਰੀ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_MIN_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_MIN_OBJ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_MIN_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_MAX_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_MAX_OBJ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_MAX_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_UNIQUE_OBJ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_UNIQUE_ਸੂਚਕਾਂਕ_OBJ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_UNIQUE_ਸੂਚਕਾਂਕ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_UNIQUE_ਸੂਚਕਾਂਕ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_UNIQUE_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_ਕਤਾਰ_UNIQUE_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DELETE_PROP_ELEM(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_DELETE_PROP_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_QB_R(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_QB_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_QB_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_QDAR_R(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_QDAR_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_QDAR_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_QF_R(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_QF_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_QF_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_QOBJ_R(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_QOBJ_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_QOBJ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_REALA(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_STR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_STRA(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_STORE_VEC4_A(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_SUB(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_SUBI(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_SUB_WR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_SUBSTR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_SUBSTR_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_TEST_NUL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_TEST_NUL_A(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_TEST_NUL_OBJ(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_TEST_NUL_PROP(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_UPARR_AND_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_UPARR_OR_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_UPARR_ਗੁਣਨਫਲ_OBJ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_UPARR_ਗੁਣਨਫਲ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_UPARR_ਗੁਣਨਫਲ_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_UPARR_ਗੁਣਨਫਲ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_UPARR_ਜੋੜ_OBJ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_UPARR_ਜੋੜ_PROP_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_UPARR_ਜੋੜ_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_UPARR_ਜੋੜ_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_UPARR_XOR_V(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_VPI_CALL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_WAIT(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_WAIT_FORK(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_XNOR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_XNORR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_XOR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_XORR(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);

ਬਾਹਰੀ bool ਦਾ_ZOMBIE(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);

ਬਾਹਰੀ bool ਦਾ_EXEC_UFUNC_REAL(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_EXEC_UFUNC_VEC4(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);
ਬਾਹਰੀ bool ਦਾ_REAP_UFUNC(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);

ਬਾਹਰੀ bool ਦਾ_CHUNK_LINK(vthread_t thr, vvp_ਕੋਡ_t ਕੋਡ);

/*
 * This is the format of a machine code instruction.
 */
ਬਣਤਰ vvp_ਕੋਡ_s {
      vvp_ਕੋਡ_fun opcode;

      ਸੰਘ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸੰਖਿਆ;
	    vvp_net_t    *net;
	    vvp_ਕੋਡ_t   cptr;
	    vvp_array_t array;
	    class __vpiHandle*handle;
	    __vpiScope*scope;
	    ਅਚਲ ਅੱਖਰ*ਪਾਠ;
      };

      ਸੰਘ {
	    uint32_t    ਦੋਆਧਾਰੀ_ਅੰਕ_idx[2];
	    vvp_net_t   *net2;
	    vvp_ਕੋਡ_t   cptr2;
	    class ufunc_core*ufunc_core_ptr;
      };
};

/*
 * This function clears the code space, ready for initialization. This
 * needs to be done exactly once before any instructions are created.
 */
ਬਾਹਰੀ ਖਾਲੀ codespace_init(ਖਾਲੀ);


/*
 * This function returns a pointer to the next free instruction in the
 * code address space. The codespace_next returns a pointer to the
 * next opcode that will be allocated. This is used by label
 * statements to get the address that will be attached to a label in
 * the code.
 */
ਬਾਹਰੀ vvp_ਕੋਡ_t codespace_allocate(ਖਾਲੀ);
ਬਾਹਰੀ vvp_ਕੋਡ_t codespace_ਅਗਲਾ(ਖਾਲੀ);
ਬਾਹਰੀ vvp_ਕੋਡ_t codespace_null(ਖਾਲੀ);

#ਜੇ_ਅੰਤ /* IVL_codes_H */
