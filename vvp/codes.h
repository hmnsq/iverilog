#यदि_अपरिभाषितम् IVL_codes_H
#परिभाषय IVL_codes_H
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

प्रकारनिर्देश bool (*vvp_संहिता_fun)(vthread_t thr, vvp_संहिता_t संहिता);

/*
 * These functions are implementations of executable op-codes. The
 * implementation lives in the vthread.cc file so that they have
 * access to the thread context.
 */
बाह्य bool सम्बन्धि_ABS_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_सङ्कलय(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_सङ्कलय_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_ADDI(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_ALLOC(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_AND(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_ANDR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_नियोजय_AR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_नियोजय_ARD(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_नियोजय_ARE(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_नियोजय_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_नियोजय_VEC4_D(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_नियोजय_VEC4_E(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_नियोजय_VEC4_A_D(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_नियोजय_VEC4_A_E(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_नियोजय_VEC4_OFF_D(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_नियोजय_VEC4_OFF_E(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_नियोजय_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_नियोजय_WRD(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_नियोजय_WRE(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_BLEND(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_BLEND_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_BLEND_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_BREAKPOINT(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CALLF_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CALLF_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CALLF_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CALLF_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CALLF_VOID(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CASSIGN_LINK(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CASSIGN_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CASSIGN_VEC4_OFF(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CASSIGN_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CAST2(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CAST_VEC2_DAR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CAST_VEC4_DAR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CAST_VEC4_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPE(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPIE(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPINE(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPNE(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPS(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPIS(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPSTR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPU(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPIU(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPWE(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPWNE(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPWR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPX(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPZ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CONCAT_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CONCATI_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CONCAT_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CONCATI_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CVT_RV(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CVT_RV_S(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CVT_SR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CVT_UR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CVT_VR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DEASSIGN(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DEASSIGN_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DEBUG_THR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DELAY(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DELAYX(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DELETE_ELEM(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DELETE_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DELETE_पुच्छ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DISABLE(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DISABLE_FLOW(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DISABLE_FORK(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DIV(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DIV_S(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DIV_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DUP_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DUP_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DUP_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_अन्तः(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_EVENT(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_EVENT_NB(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_EVCTL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_EVCTLC(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_EVCTLI(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_EVCTLS(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_FILE_पङ्क्तिः(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_FLAG_GET_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_FLAG_INV(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_FLAG_MOV(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_FLAG_OR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_FLAG_समुच्चयः_IMM(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_FLAG_समुच्चयः_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_FORCE_LINK(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_FORCE_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_FORCE_VEC4_OFF(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_FORCE_VEC4_OFF_D(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_FORCE_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_FORK(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_FREE(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_INV(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_IX_सङ्कलय(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_IX_GETV(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_IX_GETV_S(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_IX_आरोपय(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_IX_MOV(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_IX_MUL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_IX_SUB(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_IX_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_IX_VEC4_S(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_JMP(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_JMP0(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_JMP0_XZ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_JMP1(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_JMP1_XZ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_JOIN(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_JOIN_DETACH(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_आरोपय_AR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_आरोपय_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_आरोपय_DAR_R(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_आरोपय_DAR_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_आरोपय_DAR_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_आरोपय_PROP_DAR_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_आरोपय_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_आरोपय_OBJA(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_आरोपय_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_आरोपय_STRA(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_आरोपय_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_आरोपय_VEC4_A(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_MAX_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_MIN_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_MOD(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_MOD_S(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_MOD_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_MUL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_MULI(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_MUL_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_NEG_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_NAND(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_NANDR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_NEW_COBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_NEW_DARRAY(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_NOOP(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_NOR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_NORR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_NULL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_OR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_ORR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PAD_S(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PAD_U(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PART_S(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PART_U(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PARTI_S(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PARTI_U(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_निष्कासय_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_निष्कासय_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_निष्कासय_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_निष्कासय_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_POW(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_POW_S(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_POW_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QINSERT_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QINSERT_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QINSERT_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QPOP_B_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QPOP_B_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QPOP_B_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QPOP_F_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QPOP_F_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QPOP_F_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PROP_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PROP_R(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PROP_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PUSHI_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PUSHI_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PUSHI_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PUSHV_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PUTC_STR_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_RELEASE_NET(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_RELEASE_REG(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_RELEASE_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_REPLICATE(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_RET_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_RET_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_RET_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_REVERSE_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_REVERSE_PROP_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_RSORT_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_RSORT_PROP_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_SHUFFLE_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_क्रमय_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_क्रमय_PROP_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_RETLOAD_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_RETLOAD_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_RETLOAD_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_SCOPY(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_समुच्चयः_DAR_OBJ_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_समुच्चयः_DAR_OBJ_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_समुच्चयः_DAR_OBJ_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_SHIFTL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_SHIFTR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_SHIFTR_S(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_विभज_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_DAR_R(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_DAR_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_DAR_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_OBJA(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_PROP_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_PROP_R(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_PROP_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_PROP_QB_R(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_PROP_QB_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_PROP_QB_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_PROP_QF_R(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_PROP_QF_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_PROP_QF_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QINSERT_PROP_R(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QINSERT_PROP_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QINSERT_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QPOP_PROP_B_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QPOP_PROP_B_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QPOP_PROP_B_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QPOP_PROP_F_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QPOP_PROP_F_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QPOP_PROP_F_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_PROP_QUEUE_आकार(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_APPEND_WORD_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_NEW_रिक्तानि_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_आकार_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_WORD_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_WORD_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPIX_LTU(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_CMPIX_SLT0(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_प्रक्षिप_IX_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_FIND_सूचकांक_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_FIND_सूचकांक_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_FIND_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_FIND_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_FIND_FIRST_सूचकांक_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_FIND_FIRST_सूचकांक_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_FIND_FIRST_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_FIND_FIRST_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_FIND_चरमः_सूचकांक_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_FIND_चरमः_सूचकांक_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_FIND_चरमः_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_FIND_चरमः_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_MIN_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_MIN_OBJ_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_MIN_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_MAX_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_MAX_OBJ_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_MAX_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_UNIQUE_OBJ_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_UNIQUE_सूचकांक_OBJ_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_UNIQUE_सूचकांक_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_UNIQUE_सूचकांक_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_UNIQUE_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_QUEUE_UNIQUE_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DELETE_PROP_ELEM(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_DELETE_PROP_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_QB_R(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_QB_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_QB_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_QDAR_R(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_QDAR_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_QDAR_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_QF_R(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_QF_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_QF_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_QOBJ_R(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_QOBJ_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_QOBJ_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_REALA(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_STR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_STRA(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_STORE_VEC4_A(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_SUB(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_SUBI(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_SUB_WR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_SUBSTR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_SUBSTR_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_TEST_NUL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_TEST_NUL_A(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_TEST_NUL_OBJ(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_TEST_NUL_PROP(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_UPARR_AND_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_UPARR_OR_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_UPARR_गुणनफलम्_OBJ_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_UPARR_गुणनफलम्_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_UPARR_गुणनफलम्_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_UPARR_गुणनफलम्_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_UPARR_योग_OBJ_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_UPARR_योग_PROP_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_UPARR_योग_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_UPARR_योग_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_UPARR_XOR_V(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_VPI_CALL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_WAIT(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_WAIT_FORK(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_XNOR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_XNORR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_XOR(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_XORR(vthread_t thr, vvp_संहिता_t संहिता);

बाह्य bool सम्बन्धि_ZOMBIE(vthread_t thr, vvp_संहिता_t संहिता);

बाह्य bool सम्बन्धि_EXEC_UFUNC_REAL(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_EXEC_UFUNC_VEC4(vthread_t thr, vvp_संहिता_t संहिता);
बाह्य bool सम्बन्धि_REAP_UFUNC(vthread_t thr, vvp_संहिता_t संहिता);

बाह्य bool सम्बन्धि_CHUNK_LINK(vthread_t thr, vvp_संहिता_t संहिता);

/*
 * This is the format of a machine code instruction.
 */
संरचना vvp_संहिता_s {
      vvp_संहिता_fun opcode;

      सङ्घ {
	    अचिह्नित दीर्घ सङ्ख्या;
	    vvp_net_t    *net;
	    vvp_संहिता_t   cptr;
	    vvp_array_t array;
	    class __vpiHandle*handle;
	    __vpiScope*scope;
	    ध्रुव वर्ण*text;
      };

      सङ्घ {
	    uint32_t    bit_idx[2];
	    vvp_net_t   *net2;
	    vvp_संहिता_t   cptr2;
	    class ufunc_core*ufunc_core_ptr;
      };
};

/*
 * This function clears the code space, ready for initialization. This
 * needs to be done exactly once before any instructions are created.
 */
बाह्य शून्य codespace_init(शून्य);


/*
 * This function returns a pointer to the next free instruction in the
 * code address space. The codespace_next returns a pointer to the
 * next opcode that will be allocated. This is used by label
 * statements to get the address that will be attached to a label in
 * the code.
 */
बाह्य vvp_संहिता_t codespace_allocate(शून्य);
बाह्य vvp_संहिता_t codespace_अग्र(शून्य);
बाह्य vvp_संहिता_t codespace_null(शून्य);

#यदि_अन्तः /* IVL_codes_H */
