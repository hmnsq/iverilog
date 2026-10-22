#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_codes_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_codes_H
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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ bool (*vvp_ಸಂಕೇತ_fun)(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);

/*
 * These functions are implementations of executable op-codes. The
 * implementation lives in the vthread.cc file so that they have
 * access to the thread context.
 */
ಬಾಹ್ಯ bool ದ_ABS_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಕೂಡಿಸು(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಕೂಡಿಸು_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ADDI(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ALLOC(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_AND(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ANDR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ನಿಯೋಜಿಸು_AR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ನಿಯೋಜಿಸು_ARD(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ನಿಯೋಜಿಸು_ARE(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ನಿಯೋಜಿಸು_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ನಿಯೋಜಿಸು_VEC4_D(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ನಿಯೋಜಿಸು_VEC4_E(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ನಿಯೋಜಿಸು_VEC4_A_D(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ನಿಯೋಜಿಸು_VEC4_A_E(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ನಿಯೋಜಿಸು_VEC4_OFF_D(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ನಿಯೋಜಿಸು_VEC4_OFF_E(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ನಿಯೋಜಿಸು_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ನಿಯೋಜಿಸು_WRD(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ನಿಯೋಜಿಸು_WRE(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_BLEND(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_BLEND_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_BLEND_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_BREAKPOINT(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CALLF_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CALLF_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CALLF_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CALLF_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CALLF_VOID(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CASSIGN_LINK(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CASSIGN_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CASSIGN_VEC4_OFF(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CASSIGN_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CAST2(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CAST_VEC2_DAR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CAST_VEC4_DAR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CAST_VEC4_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPE(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPIE(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPINE(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPNE(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPS(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPIS(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPSTR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPU(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPIU(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPWE(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPWNE(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPWR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPX(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPZ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CONCAT_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CONCATI_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CONCAT_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CONCATI_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CVT_RV(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CVT_RV_S(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CVT_SR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CVT_UR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CVT_VR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DEASSIGN(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DEASSIGN_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DEBUG_THR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DELAY(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DELAYX(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DELETE_ELEM(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DELETE_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DELETE_ಬಾಲ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DISABLE(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DISABLE_FLOW(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DISABLE_FORK(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DIV(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DIV_S(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DIV_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DUP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DUP_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DUP_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಕೊನೆ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_EVENT(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_EVENT_NB(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_EVCTL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_EVCTLC(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_EVCTLI(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_EVCTLS(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_FILE_ಸಾಲು(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_FLAG_GET_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_FLAG_INV(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_FLAG_MOV(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_FLAG_OR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_FLAG_ಗಣ_IMM(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_FLAG_ಗಣ_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_FORCE_LINK(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_FORCE_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_FORCE_VEC4_OFF(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_FORCE_VEC4_OFF_D(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_FORCE_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_FORK(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_FREE(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_INV(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_IX_ಕೂಡಿಸು(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_IX_GETV(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_IX_GETV_S(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_IX_ತುಂಬು(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_IX_MOV(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_IX_MUL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_IX_SUB(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_IX_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_IX_VEC4_S(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_JMP(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_JMP0(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_JMP0_XZ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_JMP1(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_JMP1_XZ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_JOIN(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_JOIN_DETACH(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತುಂಬು_AR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತುಂಬು_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತುಂಬು_DAR_R(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತುಂಬು_DAR_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತುಂಬು_DAR_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತುಂಬು_PROP_DAR_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತುಂಬು_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತುಂಬು_OBJA(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತುಂಬು_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತುಂಬು_STRA(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತುಂಬು_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತುಂಬು_VEC4_A(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_MAX_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_MIN_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_MOD(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_MOD_S(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_MOD_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_MUL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_MULI(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_MUL_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_NEG_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_NAND(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_NANDR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_NEW_COBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_NEW_DARRAY(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_NOOP(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_NOR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_NORR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_NULL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_OR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ORR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PAD_S(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PAD_U(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PART_S(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PART_U(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PARTI_S(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PARTI_U(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತೆಗೆ_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತೆಗೆ_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತೆಗೆ_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತೆಗೆ_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_POW(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_POW_S(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_POW_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QINSERT_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QINSERT_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QINSERT_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QPOP_B_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QPOP_B_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QPOP_B_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QPOP_F_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QPOP_F_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QPOP_F_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PROP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PROP_R(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PROP_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PUSHI_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PUSHI_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PUSHI_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PUSHV_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PUTC_STR_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_RELEASE_NET(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_RELEASE_REG(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_RELEASE_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_REPLICATE(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_RET_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_RET_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_RET_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಹಿಮ್ಮುಖ_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಹಿಮ್ಮುಖ_PROP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_RSORT_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_RSORT_PROP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_SHUFFLE_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_SHUFFLE_PROP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ವಿಂಗಡಿಸು_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ವಿಂಗಡಿಸು_PROP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_RETLOAD_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_RETLOAD_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_RETLOAD_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_SCOPY(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಗಣ_DAR_OBJ_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಗಣ_DAR_OBJ_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಗಣ_DAR_OBJ_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_SHIFTL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_SHIFTR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_SHIFTR_S(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ವಿಭಜಿಸು_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_DAR_R(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_DAR_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_DAR_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_OBJA(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_PROP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_PROP_R(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_PROP_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_PROP_QB_R(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_PROP_QB_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_PROP_QB_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_PROP_QF_R(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_PROP_QF_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_PROP_QF_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QINSERT_PROP_R(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QINSERT_PROP_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QINSERT_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QPOP_PROP_B_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QPOP_PROP_B_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QPOP_PROP_B_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QPOP_PROP_F_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QPOP_PROP_F_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_QPOP_PROP_F_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_PROP_ಸರತಿ_ಗಾತ್ರ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_APPEND_ಶಬ್ದ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_NEW_ಖಾಲಿಗಳು_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_ಗಾತ್ರ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_ಶಬ್ದ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_ಶಬ್ದ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPIX_LTU(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_CMPIX_SLT0(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ತಳ್ಳು_IX_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಸೂಚ್ಯಂಕ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಸೂಚ್ಯಂಕ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_MIN_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_MIN_OBJ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_MIN_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_MAX_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_MAX_OBJ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_MAX_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_UNIQUE_OBJ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_UNIQUE_ಸೂಚ್ಯಂಕ_OBJ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_UNIQUE_ಸೂಚ್ಯಂಕ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_UNIQUE_ಸೂಚ್ಯಂಕ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_UNIQUE_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_ಸರತಿ_UNIQUE_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DELETE_PROP_ELEM(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_DELETE_PROP_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_QB_R(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_QB_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_QB_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_QDAR_R(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_QDAR_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_QDAR_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_QF_R(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_QF_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_QF_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_QOBJ_R(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_QOBJ_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_QOBJ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_REALA(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_STR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_STRA(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_STORE_VEC4_A(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_SUB(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_SUBI(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_SUB_WR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_SUBSTR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_SUBSTR_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_TEST_NUL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_TEST_NUL_A(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_TEST_NUL_OBJ(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_TEST_NUL_PROP(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_UPARR_AND_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_UPARR_OR_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_UPARR_ಗುಣಲಬ್ಧ_OBJ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_UPARR_ಗುಣಲಬ್ಧ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_UPARR_ಗುಣಲಬ್ಧ_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_UPARR_ಗುಣಲಬ್ಧ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_UPARR_ಮೊತ್ತ_OBJ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_UPARR_ಮೊತ್ತ_PROP_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_UPARR_ಮೊತ್ತ_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_UPARR_ಮೊತ್ತ_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_UPARR_XOR_V(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_VPI_CALL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_WAIT(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_WAIT_FORK(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_XNOR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_XNORR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_XOR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_XORR(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);

ಬಾಹ್ಯ bool ದ_ZOMBIE(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);

ಬಾಹ್ಯ bool ದ_EXEC_UFUNC_REAL(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_EXEC_UFUNC_VEC4(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);
ಬಾಹ್ಯ bool ದ_REAP_UFUNC(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);

ಬಾಹ್ಯ bool ದ_CHUNK_LINK(vthread_t thr, vvp_ಸಂಕೇತ_t ಸಂಕೇತ_2);

/*
 * This is the format of a machine code instruction.
 */
ರಚನೆ vvp_ಸಂಕೇತ_s {
      vvp_ಸಂಕೇತ_fun opcode;

      ಒಕ್ಕೂಟ {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಸಂಖ್ಯೆ;
	    vvp_net_t    *net;
	    vvp_ಸಂಕೇತ_t   cptr;
	    vvp_array_t array;
	    class __vpiHandle*handle;
	    __vpiScope*scope;
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಪಠ್ಯ;
      };

      ಒಕ್ಕೂಟ {
	    uint32_t    ಬಿಟ್_idx[2];
	    vvp_net_t   *net2;
	    vvp_ಸಂಕೇತ_t   cptr2;
	    class ufunc_core*ufunc_core_ptr;
      };
};

/*
 * This function clears the code space, ready for initialization. This
 * needs to be done exactly once before any instructions are created.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ codespace_init(ಶೂನ್ಯ);


/*
 * This function returns a pointer to the next free instruction in the
 * code address space. The codespace_next returns a pointer to the
 * next opcode that will be allocated. This is used by label
 * statements to get the address that will be attached to a label in
 * the code.
 */
ಬಾಹ್ಯ vvp_ಸಂಕೇತ_t codespace_allocate(ಶೂನ್ಯ);
ಬಾಹ್ಯ vvp_ಸಂಕೇತ_t codespace_ಮುಂದಿನ(ಶೂನ್ಯ);
ಬಾಹ್ಯ vvp_ಸಂಕೇತ_t codespace_null(ಶೂನ್ಯ);

#ಆದರೆ_ಅಂತ್ಯ /* IVL_codes_H */
