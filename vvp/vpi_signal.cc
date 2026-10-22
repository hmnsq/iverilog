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

/*
 * vpiReg handles are handled here. These objects represent vectors of
 * .var objects that can be manipulated by the VPI module.
 */

# include  "compile.h"
# include  "vpi_priv.h"
# include  "vvp_net_sig.h"
# include  "vvp_island.h"
# include  "schedule.h"
# include  "statistics.h"
# include  "config.h"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <cmath>
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <climits>
# include  <cstring>
# include  <cassert>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
#ಆದರೆ_ಅಂತ್ಯ
# include  "ivl_alloc.h"

/*
 * Hex digits that represent 4-value bits of Verilog are not as
 * trivially obvious to display as if the bits were the usual 2-value
 * bits. So, although it is possible to write a function that
 * generates a correct character for 4*4-value bits, it is easier to
 * just perform the lookup in a table. This only takes 256 bytes,
 * which is not many executable instructions:-)
 *
 * The table is calculated at compile time, therefore, by the
 * draw_tt.c program.
 */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ hex_digits[256];
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
ಶೂನ್ಯ *need_ಫಲಿತಾಂಶ_buf(ಚಿಹ್ನೆರಹಿತ cnt, vpi_rbuf_t type)
{
      ಸ್ಥಿರ ಶೂನ್ಯ*ಫಲಿತಾಂಶ_buf[2] = {0, 0};
      ಸ್ಥಿರ size_t ಫಲಿತಾಂಶ_buf_ಗಾತ್ರ[2] = {0, 0};

      ಆದರೆ (type == RBUF_DEL) {
	    ಬಿಡುಗಡೆ(ಫಲಿತಾಂಶ_buf[RBUF_VAL]);
	    ಫಲಿತಾಂಶ_buf[RBUF_VAL] = 0;
	    ಫಲಿತಾಂಶ_buf_ಗಾತ್ರ[RBUF_VAL] = 0;

	    ಬಿಡುಗಡೆ(ಫಲಿತಾಂಶ_buf[RBUF_STR]);
	    ಫಲಿತಾಂಶ_buf[RBUF_STR] = 0;
	    ಫಲಿತಾಂಶ_buf_ಗಾತ್ರ[RBUF_STR] = 0;

	    ಹಿಂತಿರುಗಿ 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      ಆದರೆ (ಫಲಿತಾಂಶ_buf_ಗಾತ್ರ[type] == 0) {
	    ಫಲಿತಾಂಶ_buf[type] = ಸ್ಮರಣೆ_ಕೊಡು(cnt);
	    ಫಲಿತಾಂಶ_buf_ಗಾತ್ರ[type] = cnt;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಫಲಿತಾಂಶ_buf_ಗಾತ್ರ[type] < cnt) {
	    ಫಲಿತಾಂಶ_buf[type] = realloc(ಫಲಿತಾಂಶ_buf[type], cnt);
	    ಫಲಿತಾಂಶ_buf_ಗಾತ್ರ[type] = cnt;
      }

      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_buf[type];
}

ಅಕ್ಷರ *simple_ಗಣ_rbuf_str(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s1)
{
      ಅಕ್ಷರ *res = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s1)+1, RBUF_STR));
      ಆದರೆ (res) strcpy(res,s1);
      ಹಿಂತಿರುಗಿ res;
}

ಅಕ್ಷರ *generic_get_str(ಪೂರ್ಣಾಂಕ code, vpiHandle ref, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸೂಚ್ಯಂಕ)
{
      size_t len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name) + 1;  /* include space for null termination */
      ಅಕ್ಷರ *bn = ಖಾಲಿ;
      ತಾರ್ಕಿಕ is_bn_a_pkg = ಸುಳ್ಳು;
      ಆದರೆ (code == vpi_ತುಂಬಿದ_Name) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    ಆದರೆ (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      ಆದರೆ (ಸೂಚ್ಯಂಕ != ಖಾಲಿ) len += ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಸೂಚ್ಯಂಕ) + 2;  /* include space for brackets */

      ಅಕ್ಷರ *res = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(len, RBUF_STR));
      ಆದರೆ (!res) {
	    ಬಿಡುಗಡೆ(bn);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      ಆದರೆ (bn != ಖಾಲಿ) {
	    strcat(res, bn);
	      // A package already has the "::" separator in the name
	    ಆದರೆ (! is_bn_a_pkg) strcat(res, ".");
	    ಬಿಡುಗಡೆ(bn);
      }
      strcat(res, name);
      ಆದರೆ (ಸೂಚ್ಯಂಕ != ಖಾಲಿ) {
	    strcat(res, "[");
	    strcat(res, ಸೂಚ್ಯಂಕ);
	    strcat(res, "]");
      }
      ಹಿಂತಿರುಗಿ res;
}

ಸ್ಥಿರ vpiHandle fill_ಒಳ_net4(ರಚನೆ __vpiSignal*obj, __vpiScope*scope,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb,
                              ತಾರ್ಕಿಕ signed_flag, vvp_net_t*ಗಂಟು);

ಸ್ಥಿರ vpiHandle fill_ಒಳ_var4(ರಚನೆ __vpiSignal*obj,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb,
                              ತಾರ್ಕಿಕ signed_flag, vvp_net_t*ಗಂಟು)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      ಹಿಂತಿರುಗಿ fill_ಒಳ_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, ಗಂಟು);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiBinStrVal(ಸ್ಥಿರಾಂಕ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಂಕ ತಳ, ಚಿಹ್ನೆರಹಿತ wid,
                                s_vpi_ಮೌಲ್ಯ*vp)
{
      ಅಕ್ಷರ *rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(wid+1, RBUF_VAL));
      ದೀರ್ಘ ಕೊನೆ = ತಳ + (ಚಿಹ್ನೆಯುಳ್ಳ)wid;
      ದೀರ್ಘ ಸ್ಥಾನಾಂತರ_2 = ಕೊನೆ - 1;
      ದೀರ್ಘ ssize = (ಚಿಹ್ನೆಯುಳ್ಳ)sig->ಮೌಲ್ಯ_ಗಾತ್ರ();

      ಗಾಗಿ (ದೀರ್ಘ idx = ತಳ ;  idx < ಕೊನೆ ;  idx += 1) {
	    ಆದರೆ (idx < 0 || idx >= ssize) {
                  rbuf[ಸ್ಥಾನಾಂತರ_2-idx] = 'x';
	    } ಇಲ್ಲದಿದ್ದರೆ {
                  rbuf[ಸ್ಥಾನಾಂತರ_2-idx] = vvp_bit4_ಗಮ್ಯ_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiOctStrVal(ಸ್ಥಿರಾಂಕ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಂಕ ತಳ, ಚಿಹ್ನೆರಹಿತ wid,
                                s_vpi_ಮೌಲ್ಯ*vp)
{
      ಚಿಹ್ನೆರಹಿತ dwid = (wid + 2) / 3;
      ಅಕ್ಷರ *rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(dwid+1, RBUF_VAL));
      ದೀರ್ಘ ಕೊನೆ = ತಳ + (ಚಿಹ್ನೆಯುಳ್ಳ)wid;
      ದೀರ್ಘ ssize = (ಚಿಹ್ನೆಯುಳ್ಳ)sig->ಮೌಲ್ಯ_ಗಾತ್ರ();
      ಚಿಹ್ನೆರಹಿತ val = 0;

      rbuf[dwid] = 0;
      ಗಾಗಿ (ದೀರ್ಘ idx = ತಳ ;  idx < ಕೊನೆ ;  idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ = 0;
	    ಆದರೆ (idx < 0 || idx >= ssize) {
                  ಬಿಟ್ = 2; // BIT4_X
	    } ಇಲ್ಲದಿದ್ದರೆ {
                  ಆಯ್ಕೆ (sig->value(idx)) {
		      ಸ್ಥಿತಿ BIT4_0:
			ಬಿಟ್ = 0;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ BIT4_1:
			ಬಿಟ್ = 1;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ BIT4_X:
			ಬಿಟ್ = 2;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ BIT4_Z:
			ಬಿಟ್ = 3;
			ನಿಲ್ಲಿಸು;
                  }
	    }
	    val |= ಬಿಟ್ << 2*((idx-ತಳ) % 3);

	    ಆದರೆ ((idx-ತಳ) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ಆಯ್ಕೆ (wid % 3) {
	  ಸ್ಥಿತಿ 1:
	    ಆದರೆ (val == 2) val = 42;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val == 3) val = 63;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 2:
	    ಆದರೆ (val == 10) val = 42;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val == 15) val = 63;
	    ನಿಲ್ಲಿಸು;
      }

      ಆದರೆ (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiHexStrVal(ಸ್ಥಿರಾಂಕ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಂಕ ತಳ, ಚಿಹ್ನೆರಹಿತ wid,
                                s_vpi_ಮೌಲ್ಯ*vp)
{
      ಚಿಹ್ನೆರಹಿತ dwid = (wid + 3) / 4;
      ಅಕ್ಷರ *rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(dwid+1, RBUF_VAL));
      ದೀರ್ಘ ಕೊನೆ = ತಳ + (ಚಿಹ್ನೆಯುಳ್ಳ)wid;
      ದೀರ್ಘ ssize = (ಚಿಹ್ನೆಯುಳ್ಳ)sig->ಮೌಲ್ಯ_ಗಾತ್ರ();
      ಚಿಹ್ನೆರಹಿತ val = 0;

      rbuf[dwid] = 0;
      ಗಾಗಿ (ದೀರ್ಘ idx = ತಳ ;  idx < ಕೊನೆ ;  idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ = 0;
	    ಆದರೆ (idx < 0 || idx >= ssize) {
                  ಬಿಟ್ = 2; // BIT4_X
	    } ಇಲ್ಲದಿದ್ದರೆ {
                  ಆಯ್ಕೆ (sig->value(idx)) {
		      ಸ್ಥಿತಿ BIT4_0:
			ಬಿಟ್ = 0;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ BIT4_1:
			ಬಿಟ್ = 1;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ BIT4_X:
			ಬಿಟ್ = 2;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ BIT4_Z:
			ಬಿಟ್ = 3;
			ನಿಲ್ಲಿಸು;
                  }
	    }
	    val |= ಬಿಟ್ << 2*((idx-ತಳ) % 4);

	    ಆದರೆ ((idx-ತಳ) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ಆಯ್ಕೆ (wid % 4) {
	  ಸ್ಥಿತಿ 1:
	    ಆದರೆ (val == 2) val = 170;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val == 3) val = 255;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 2:
	    ಆದರೆ (val == 10) val = 170;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val == 15) val = 255;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 3:
	    ಆದರೆ (val == 42) val = 170;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val == 63) val = 255;
	    ನಿಲ್ಲಿಸು;
      }

      ಆದರೆ (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiDecStrVal(ಸ್ಥಿರಾಂಕ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಂಕ ತಳ, ಚಿಹ್ನೆರಹಿತ wid,
                                ಪೂರ್ಣಾಂಕ signed_flag, s_vpi_ಮೌಲ್ಯ*vp)
{
      ಚಿಹ್ನೆರಹಿತ hwid = (sig->ಮೌಲ್ಯ_ಗಾತ್ರ()+2) / 3 + 1;
      ಅಕ್ಷರ *rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(hwid, RBUF_VAL));
      ದೀರ್ಘ ssize = (ಚಿಹ್ನೆಯುಳ್ಳ)sig->ಮೌಲ್ಯ_ಗಾತ್ರ();
      ದೀರ್ಘ ಕೊನೆ = ತಳ + (ಚಿಹ್ನೆಯುಳ್ಳ)wid;

	/* Do we have an end outside of the real signal vector. */
      ಆದರೆ (ತಳ < 0 || ಕೊನೆ > ssize) {
	    ತಾರ್ಕಿಕ all_x = ನಿಜ;
	    ಆದರೆ (ಕೊನೆ > ssize) ಕೊನೆ = ssize;
	    ಆದರೆ (ತಳ < 0) ತಳ = 0;
	    ಗಾಗಿ (ದೀರ್ಘ idx = ತಳ ;  idx < ಕೊನೆ ;  idx += 1) {
		  ಆದರೆ (sig->value(idx) != BIT4_X) {
			all_x = ಸುಳ್ಳು;
			ನಿಲ್ಲಿಸು;
		  }
	    }

	    ಆದರೆ (all_x) {
		  rbuf[0] = 'x';
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_vector4_t vec4;
      ಆದರೆ (ತಳ == 0 && ಕೊನೆ == ssize) {
	    sig->vec4_ಮೌಲ್ಯ(vec4);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_vector4_t tmp;
	    sig->vec4_ಮೌಲ್ಯ(tmp);
	    vec4 = tmp.subvalue(ತಳ, wid);
      }

      vpip_vec4_ಗಮ್ಯ_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiIntVal(ಸ್ಥಿರಾಂಕ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಂಕ ತಳ, ಚಿಹ್ನೆರಹಿತ wid,
                             ಪೂರ್ಣಾಂಕ signed_flag, s_vpi_ಮೌಲ್ಯ*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_ಮೌಲ್ಯ(tmp);
      vvp_vector4_t sub = tmp.subvalue(ತಳ, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      ಆದರೆ (ಗಾತ್ರ(vp->value.integer) == ಗಾತ್ರ(int32_t)) {
	    int32_t val = 0;
	    vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(sub, val, signed_flag, ಸುಳ್ಳು);
	    vp->value.integer = val;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ(vp->value.integer) == ಗಾತ್ರ(int64_t));
	    int64_t val = 0;
	    vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(sub, val, signed_flag, ಸುಳ್ಳು);
	    vp->value.integer = val;
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiRealVal(ಸ್ಥಿರಾಂಕ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಂಕ ತಳ, ಚಿಹ್ನೆರಹಿತ wid,
                              ಪೂರ್ಣಾಂಕ signed_flag, s_vpi_ಮೌಲ್ಯ*vp)
{
      vvp_vector4_t vec4(wid);
      ದೀರ್ಘ ssize = (ಚಿಹ್ನೆಯುಳ್ಳ)sig->ಮೌಲ್ಯ_ಗಾತ್ರ();
      ದೀರ್ಘ ಕೊನೆ = ತಳ + (ಚಿಹ್ನೆಯುಳ್ಳ)wid;
      ಆದರೆ (ಕೊನೆ > ssize) ಕೊನೆ = ssize;

      ಗಾಗಿ (ದೀರ್ಘ idx = (ತಳ < 0) ? 0 : ತಳ ;  idx < ಕೊನೆ ;  idx += 1) {
	    vec4.ಗಣ_ಬಿಟ್(idx-ತಳ, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(vec4, vp->value.real, signed_flag);
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiStringVal(ಸ್ಥಿರಾಂಕ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಂಕ ತಳ, ಚಿಹ್ನೆರಹಿತ wid,
                                s_vpi_ಮೌಲ್ಯ*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      ಅಕ್ಷರ *rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      ಅಕ್ಷರ *cp = rbuf;

      ಅಕ್ಷರ tmp = 0;
      ಗಾಗಿ (ದೀರ್ಘ idx = ತಳ+(ಚಿಹ್ನೆಯುಳ್ಳ)wid-1; idx >= ತಳ; idx -= 1) {
	    tmp <<= 1;

	    ಆದರೆ (idx >=0 && idx < (ಚಿಹ್ನೆಯುಳ್ಳ)sig->ಮೌಲ್ಯ_ಗಾತ್ರ() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    ಆದರೆ (((idx-ತಳ)&7)==0){
		  /* Skip leading nulls. */
		  ಆದರೆ (tmp == 0 && cp == rbuf)
			ಮುಂದುವರಿ;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiScalarVal(ಸ್ಥಿರಾಂಕ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಂಕ ತಳ,
                                s_vpi_ಮೌಲ್ಯ*vp)
{
      ಆದರೆ (ತಳ >= 0 && ತಳ < (ಚಿಹ್ನೆಯುಳ್ಳ)sig->ಮೌಲ್ಯ_ಗಾತ್ರ()) {
	    ಆಯ್ಕೆ (sig->value(ತಳ)) {
		ಸ್ಥಿತಿ BIT4_0:
		  vp->value.scalar = vpi0;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  vp->value.scalar = vpi1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_ಮೌಲ್ಯ(ತಳ);
		  ಆದರೆ (strn.strength0() == 1) vp->value.scalar = vpiH;
		  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strn.strength1() == 1) vp->value.scalar = vpiL;
		  ಇಲ್ಲದಿದ್ದರೆ vp->value.scalar = vpiX;
		  ನಿಲ್ಲಿಸು;
		}
		ಸ್ಥಿತಿ BIT4_Z:
		  vp->value.scalar = vpiZ;
		  ನಿಲ್ಲಿಸು;
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
	    vp->value.scalar = vpiX;
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiStrengthVal(ಸ್ಥಿರಾಂಕ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಂಕ ತಳ,
                                  ಚಿಹ್ನೆರಹಿತ wid, s_vpi_ಮೌಲ್ಯ*vp)
{
      ದೀರ್ಘ ಕೊನೆ = ತಳ + (ಚಿಹ್ನೆಯುಳ್ಳ)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_ಫಲಿತಾಂಶ_buf(wid * ಗಾತ್ರ(s_vpi_strengthval), RBUF_VAL));

      ಗಾಗಿ (ದೀರ್ಘ idx = ತಳ ;  idx < ಕೊನೆ ;  idx += 1) {
	    ಆದರೆ (idx >=0 && idx < (ಚಿಹ್ನೆಯುಳ್ಳ)sig->ಮೌಲ್ಯ_ಗಾತ್ರ()) {
		  vvp_scalar_t val = sig->scalar_ಮೌಲ್ಯ(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  ಚಿಹ್ನೆರಹಿತ s0 = 1 << val.strength0();
		  ಚಿಹ್ನೆರಹಿತ s1 = 1 << val.strength1();

		  ಆಯ್ಕೆ (val.value()) {
		      ಸ್ಥಿತಿ BIT4_0:
			op[idx-ತಳ].logic = vpi0;
			op[idx-ತಳ].s0 = s0|s1;
			op[idx-ತಳ].s1 = 0;
			ನಿಲ್ಲಿಸು;

		      ಸ್ಥಿತಿ BIT4_1:
			op[idx-ತಳ].logic = vpi1;
			op[idx-ತಳ].s0 = 0;
			op[idx-ತಳ].s1 = s0|s1;
			ನಿಲ್ಲಿಸು;

		      ಸ್ಥಿತಿ BIT4_X:
			op[idx-ತಳ].logic = vpiX;
			op[idx-ತಳ].s0 = s0;
			op[idx-ತಳ].s1 = s1;
			ನಿಲ್ಲಿಸು;

		      ಸ್ಥಿತಿ BIT4_Z:
			op[idx-ತಳ].logic = vpiZ;
			op[idx-ತಳ].s0 = vpiHiZ;
			op[idx-ತಳ].s1 = vpiHiZ;
			ನಿಲ್ಲಿಸು;
		  }
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  op[idx-ತಳ].logic = vpiX;
		  op[idx-ತಳ].s0 = vpiStrongDrive;
		  op[idx-ತಳ].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpi_ಸದಿಶ_Val(ಸ್ಥಿರಾಂಕ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಂಕ ತಳ, ಚಿಹ್ನೆರಹಿತ wid,
                                s_vpi_ಮೌಲ್ಯ*vp)
{
      ದೀರ್ಘ ಕೊನೆ = ತಳ + (ಚಿಹ್ನೆಯುಳ್ಳ)wid;
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ obit = 0;
      ಚಿಹ್ನೆರಹಿತ hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_ಫಲಿತಾಂಶ_buf(hwid * ಗಾತ್ರ(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      ಗಾಗಿ (ದೀರ್ಘ idx = ತಳ ;  idx < ಕೊನೆ ;  idx += 1) {
	    ಆದರೆ (ತಳ >= 0 && ತಳ < (ಚಿಹ್ನೆಯುಳ್ಳ)sig->ಮೌಲ್ಯ_ಗಾತ್ರ()) {
		ಆಯ್ಕೆ (sig->value(idx)) {
		ಸ್ಥಿತಿ BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  ನಿಲ್ಲಿಸು;
		}
	    } ಇಲ್ಲದಿದ್ದರೆ {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    ಆದರೆ (!(obit % 32)) {
		  op += 1;
		  ಆದರೆ ((op - vp->value.vector) < (ptrdiff_t)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ signal_get(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ 0;  // Not implemented for now!

	  ಸ್ಥಿತಿ vpiSigned:
	    ಹಿಂತಿರುಗಿ rfp->signed_flag != 0;

	  ಸ್ಥಿತಿ vpiArray:
	    ಹಿಂತಿರುಗಿ rfp->is_netarray != 0;

	  ಸ್ಥಿತಿ vpi_ಸೂಚ್ಯಂಕ_2: // This only works while we have a single index.
	    ಆದರೆ (rfp->is_netarray) {
		  s_vpi_ಮೌಲ್ಯ vp;
		  vp.format = vpiIntVal;
		  vpi_get_ಮೌಲ್ಯ(rfp->id.ಸೂಚ್ಯಂಕ, &vp);
		  ಹಿಂತಿರುಗಿ vp.value.integer;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಹಿಂತಿರುಗಿ vpiUndefined;
	    }

	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ rfp->width();

	  ಸ್ಥಿತಿ vpiNetType:
	    ಆದರೆ (ref->get_type_ಸಂಕೇತ()==vpiNet)
		  ಹಿಂತಿರುಗಿ vpiWire;
	    ಇಲ್ಲದಿದ್ದರೆ
		  ಹಿಂತಿರುಗಿ vpiUndefined;

	  ಸ್ಥಿತಿ vpi_ಎಡ_Range:
            ಹಿಂತಿರುಗಿ rfp->msb.get_ಮೌಲ್ಯ();

	  ಸ್ಥಿತಿ vpi_ಬಲ_Range:
            ಹಿಂತಿರುಗಿ rfp->lsb.get_ಮೌಲ್ಯ();

	  ಸ್ಥಿತಿ vpiScalar:
	    ಹಿಂತಿರುಗಿ (rfp->msb.get_ಮೌಲ್ಯ() == 0 && rfp->lsb.get_ಮೌಲ್ಯ() == 0);
	  ಸ್ಥಿತಿ vpi_ಸದಿಶ:
	    ಹಿಂತಿರುಗಿ (rfp->msb.get_ಮೌಲ್ಯ() != rfp->lsb.get_ಮೌಲ್ಯ());

          ಸ್ಥಿತಿ vpiAutomatic:
            ಹಿಂತಿರುಗಿ vpip_scope(rfp)->is_automatic() ? 1  : 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BR916_STOPGAP_FIX
          ಸ್ಥಿತಿ _vpiFromThr:
            ಹಿಂತಿರುಗಿ _vpiNoThr;
#ಆದರೆ_ಅಂತ್ಯ

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    ಹಿಂತಿರುಗಿ vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
ಸ್ಥಿರ int64_t signal_get64(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆಯ್ಕೆ (code) {
	    // This private property must return zero when undefined.
	  ಸ್ಥಿತಿ _vpiNexusId:
	    ಆದರೆ (rfp->msb.get_ಮೌಲ್ಯ() == rfp->lsb.get_ಮೌಲ್ಯ())
		  ಹಿಂತಿರುಗಿ reinterpret_cast<int64_t>(rfp->ಗಂಟು);
	    ಹಿಂತಿರುಗಿ 0;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    ಹಿಂತಿರುಗಿ vpiUndefined;
      }
}

ಸ್ಥಿರ ಅಕ್ಷರ* signal_get_str(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆದರೆ (code == vpiFile) {  // Not implemented for now!
	    ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[0]);
      }

      ಆದರೆ ((code != vpiName) && (code != vpi_ತುಂಬಿದ_Name)) ಹಿಂತಿರುಗಿ ಖಾಲಿ;

      ಅಕ್ಷರ *nm;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ixs;
      ಆದರೆ (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.ಪೋಷಕ));
	    s_vpi_ಮೌಲ್ಯ vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_ಮೌಲ್ಯ(rfp->id.ಸೂಚ್ಯಂಕ, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } ಇಲ್ಲದಿದ್ದರೆ {
	    nm = strdup(rfp->id.name);
	    ixs = ಖಾಲಿ;
      }

	/* The scope information is added here for vpiFullName. */
      ಅಕ್ಷರ *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      ಬಿಡುಗಡೆ(nm);
      ಹಿಂತಿರುಗಿ rbuf;
}

ಸ್ಥಿರ vpiHandle signal_get_handle(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆಯ್ಕೆ (code) {

	  ಸ್ಥಿತಿ vpi_ಪೋಷಕ:
	    ಹಿಂತಿರುಗಿ rfp->is_netarray? rfp->within.ಪೋಷಕ : ಖಾಲಿ;

	  ಸ್ಥಿತಿ vpi_ಸೂಚ್ಯಂಕ_2:
	    ಹಿಂತಿರುಗಿ rfp->is_netarray? rfp->id.ಸೂಚ್ಯಂಕ : ಖಾಲಿ;

	  ಸ್ಥಿತಿ vpi_ಎಡ_Range:
	    ಹಿಂತಿರುಗಿ &rfp->msb;
	  ಸ್ಥಿತಿ vpi_ಬಲ_Range:
	    ಹಿಂತಿರುಗಿ &rfp->lsb;

	  ಸ್ಥಿತಿ vpiScope:
	    ಹಿಂತಿರುಗಿ vpip_scope(rfp);

	  ಸ್ಥಿತಿ vpiModule:
	    ಹಿಂತಿರುಗಿ vpip_module(vpip_scope(rfp));
      }

      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ vpiHandle signal_iterate(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆದರೆ (code == vpi_ಸೂಚ್ಯಂಕ_2) {
	    ಹಿಂತಿರುಗಿ rfp->is_netarray ? rfp->id.ಸೂಚ್ಯಂಕ->vpi_iterate(code) : ಖಾಲಿ;
      }

      ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ __vpiSignal::make_ಬಿಟ್ಟುಗಳು()
{
      ಪ್ರತಿಜ್ಞಾ(ಬಿಟ್ಟುಗಳು == ಖಾಲಿ);
      ಬಿಟ್ಟುಗಳು = ಹೊಸ ರಚನೆ __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      ಬಿಟ್ಟುಗಳು->ಪೋಷಕ = ಇದು;
	// Now point to word[0].
      ಬಿಟ್ಟುಗಳು += 1;

	// Update each bit to point to the base
      ರಚನೆ __vpiBit*ತಳ = ಬಿಟ್ಟುಗಳು;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < width(); idx += 1) {
	    ತಳ[idx].bit0 = ತಳ;
	    ಪೂರ್ಣಾಂಕ real_idx;
	    ಆದರೆ (msb.get_ಮೌಲ್ಯ() >= lsb.get_ಮೌಲ್ಯ()) {
		  real_idx = idx + lsb.get_ಮೌಲ್ಯ();
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  real_idx = lsb.get_ಮೌಲ್ಯ() - idx;
	    }
	    ತಳ[idx].ಸೂಚ್ಯಂಕ = ಹೊಸ __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ idx)
{
	/* Check to see if the index is in range. */
      ಆದರೆ (msb.get_ಮೌಲ್ಯ() >= lsb.get_ಮೌಲ್ಯ()) {
	    ಆದರೆ ((idx > msb.get_ಮೌಲ್ಯ()) || (idx < lsb.get_ಮೌಲ್ಯ())) ಹಿಂತಿರುಗಿ 0;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಆದರೆ ((idx < msb.get_ಮೌಲ್ಯ()) || (idx > lsb.get_ಮೌಲ್ಯ())) ಹಿಂತಿರುಗಿ 0;
      }

	/* Normalize the index */
      ಚಿಹ್ನೆರಹಿತ norm_idx;
      ಆದರೆ (msb.get_ಮೌಲ್ಯ() >= lsb.get_ಮೌಲ್ಯ()) {
	    norm_idx = idx - lsb.get_ಮೌಲ್ಯ();
      } ಇಲ್ಲದಿದ್ದರೆ {
	    norm_idx = lsb.get_ಮೌಲ್ಯ() - idx;
      }

      ಆದರೆ (ಬಿಟ್ಟುಗಳು == ಖಾಲಿ) make_ಬಿಟ್ಟುಗಳು();

      ಹಿಂತಿರುಗಿ &(ಬಿಟ್ಟುಗಳು[norm_idx].as_ಬಿಟ್);
}

ಶೂನ್ಯ __vpiSignal::get_ಬಿಟ್_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ರಚನೆ __vpiBit*ಬಿಟ್, p_vpi_ಮೌಲ್ಯ vp)
{
      ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ = ಬಿಟ್->get_norm_ಸೂಚ್ಯಂಕ();

      vvp_signal_ಮೌಲ್ಯ*vsig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(ಗಂಟು->fil);
      ಪ್ರತಿಜ್ಞಾ(vsig);

      ಆದರೆ (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      ಆಯ್ಕೆ (vp->format) {
	ಸ್ಥಿತಿ vpiBinStrVal:
            format_vpiBinStrVal(vsig, ಸೂಚ್ಯಂಕ, 1, vp);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ vpiOctStrVal:
            format_vpiOctStrVal(vsig, ಸೂಚ್ಯಂಕ, 1, vp);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ vpiDecStrVal:
            format_vpiDecStrVal(vsig, ಸೂಚ್ಯಂಕ, 1, ಸುಳ್ಳು, vp);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ vpiHexStrVal:
            format_vpiHexStrVal(vsig, ಸೂಚ್ಯಂಕ, 1, vp);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ vpiStringVal:
            format_vpiStringVal(vsig, ಸೂಚ್ಯಂಕ, 1, vp);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ vpiIntVal:
            format_vpiIntVal(vsig, ಸೂಚ್ಯಂಕ, 1, ಸುಳ್ಳು, vp);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ vpiRealVal:
            format_vpiRealVal(vsig, ಸೂಚ್ಯಂಕ, 1, ಸುಳ್ಳು, vp);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ vpiScalarVal:
            format_vpiScalarVal(vsig, ಸೂಚ್ಯಂಕ, vp);
	    ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ vpiStrengthVal:
            format_vpiStrengthVal(vsig, ಸೂಚ್ಯಂಕ, 1, vp);
            ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:
            format_vpi_ಸದಿಶ_Val(vsig, ಸೂಚ್ಯಂಕ, 1, vp);
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (ಪೂರ್ಣಾಂಕ)vp->format, vpi_get_str(vpiName),
		    ಬಿಟ್->get_ಸೂಚ್ಯಂಕ(),
	            vpip_scope(ಇದು)->scope_name());
	    ಪ್ರತಿಜ್ಞಾ(0);
      };
}

vpiHandle __vpiSignal::put_ಬಿಟ್_ಮೌಲ್ಯ(ರಚನೆ __vpiBit*ಬಿಟ್, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ flags)
{
      ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ = ಬಿಟ್->get_norm_ಸೂಚ್ಯಂಕ();
      vvp_net_ptr_t dest(ಗಂಟು, 0);
      vvp_vector4_t val = vec4_from_vpi_ಮೌಲ್ಯ(vp, 1);

      ಆದರೆ ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    ಬಿಟ್->as_ಬಿಟ್.vpi_get_str(vpi_ತುಂಬಿದ_Name));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
      }

      ಆದರೆ ((get_type_ಸಂಕೇತ() == vpiNet) &&
          !dynamic_cast<vvp_island_ಪೋರ್ಟ್*>(ಗಂಟು->fun)) {
	    ಗಂಟು->send_vec4_pv(val, ಸೂಚ್ಯಂಕ, width(),
	                            vthread_get_wt_context());
      } ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_send_vec4_pv(dest, val, ಸೂಚ್ಯಂಕ, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ vpiHandle signal_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ idx, vpiHandle ref)
{
      ರಚನೆ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      ಆದರೆ ((type != vpiNet) && (type != vpiReg)) ಹಿಂತಿರುಗಿ 0;

      ಹಿಂತಿರುಗಿ rfp->get_ಸೂಚ್ಯಂಕ(idx);
}

ಚಿಹ್ನೆರಹಿತ __vpiSignal::width(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{
      ಚಿಹ್ನೆರಹಿತ wid = (msb.get_ಮೌಲ್ಯ() >= lsb.get_ಮೌಲ್ಯ())
	    ? (msb.get_ಮೌಲ್ಯ() - lsb.get_ಮೌಲ್ಯ() + 1)
	    : (lsb.get_ಮೌಲ್ಯ() - msb.get_ಮೌಲ್ಯ() + 1);

      ಹಿಂತಿರುಗಿ wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
ಸ್ಥಿರ ಶೂನ್ಯ signal_get_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp)
{
      ರಚನೆ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಚಿಹ್ನೆರಹಿತ wid = rfp->width();

      vvp_signal_ಮೌಲ್ಯ*vsig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(rfp->ಗಂಟು->fil);
      ಪ್ರತಿಜ್ಞಾ(vsig);

      ಆಯ್ಕೆ (vp->format) {

	  ಸ್ಥಿತಿ vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:
	    format_vpi_ಸದಿಶ_Val(vsig, 0, wid, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiObjTypeVal:
	    ಆದರೆ (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  vp->format = vpi_ಸದಿಶ_Val;
		  format_vpi_ಸದಿಶ_Val(vsig, 0, wid, vp);
	    }
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (ಪೂರ್ಣಾಂಕ)vp->format, vpi_get_str(vpiName, ref),
		    vpip_scope(rfp)->scope_name());
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

/*
 * The put_value method writes the value into the vector, and returns
 * the affected ref. This operation works much like the %set or
 * %assign instructions and causes all the side-effects that the
 * equivalent instruction would cause.
 */

ಸ್ಥಿರ vvp_vector4_t from_stringval(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str, ಚಿಹ್ನೆರಹಿತ wid)
{
      ಚಿಹ್ನೆರಹಿತ idx;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*cp;

      cp = str + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      ಆಗ ((idx < wid) && (cp > str)) {
	    ಚಿಹ್ನೆರಹಿತ byte = *--cp;
	    ಪೂರ್ಣಾಂಕ ಬಿಟ್;

	    ಗಾಗಿ (ಬಿಟ್ = 0 ;  (ಬಿಟ್ < 8) && (idx < wid) ;  ಬಿಟ್ += 1) {
		  ಆದರೆ (byte & 1)
			val.ಗಣ_ಬಿಟ್(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      ಹಿಂತಿರುಗಿ val;
}

ಸ್ಥಿರ vpiHandle signal_put_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp, ಪೂರ್ಣಾಂಕ flags)
{
      ಚಿಹ್ನೆರಹಿತ wid;
      ರಚನೆ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);
      vvp_net_ptr_t dest(rfp->ಗಂಟು, 0);

      ತಾರ್ಕಿಕ net_flag = ref->get_type_ಸಂಕೇತ()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      ಆದರೆ (flags == vpiReleaseFlag) {
	    ಪ್ರತಿಜ್ಞಾ(rfp->ಗಂಟು->fil);
	    rfp->ಗಂಟು->fil->force_unlink();
	    rfp->ಗಂಟು->fil->release(dest, net_flag);
	    rfp->ಗಂಟು->fun->force_flag(ನಿಜ);
	    signal_get_ಮೌಲ್ಯ(ref, vp);
	    ಹಿಂತಿರುಗಿ ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_ಮೌಲ್ಯ() >= rfp->lsb.get_ಮೌಲ್ಯ())
	    ? (rfp->msb.get_ಮೌಲ್ಯ() - rfp->lsb.get_ಮೌಲ್ಯ() + 1)
	    : (rfp->lsb.get_ಮೌಲ್ಯ() - rfp->msb.get_ಮೌಲ್ಯ() + 1);

      vvp_vector4_t val = vec4_from_vpi_ಮೌಲ್ಯ(vp, wid);

      ಆದರೆ (flags == vpiForceFlag) {
	    vvp_vector2_t ಮುಖವಾಡ (vvp_vector2_t::FILL1, wid);
	    rfp->ಗಂಟು->force_vec4(val, ಮುಖವಾಡ);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (net_flag && !dynamic_cast<vvp_island_ಪೋರ್ಟ್*>(rfp->ಗಂಟು->fun)) {
	    rfp->ಗಂಟು->send_vec4(val, vthread_get_wt_context());
      } ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      ಹಿಂತಿರುಗಿ ref;
}

vvp_vector4_t vec4_from_vpi_ಮೌಲ್ಯ(s_vpi_ಮೌಲ್ಯ*vp, ಚಿಹ್ನೆರಹಿತ wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      ಆಯ್ಕೆ (vp->format) {

	  ಸ್ಥಿತಿ vpiIntVal: {
		ದೀರ್ಘ vpi_val = vp->value.integer;
		ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t ಬಿಟ್ = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.ಗಣ_ಬಿಟ್(idx, ಬಿಟ್);
		      vpi_val >>= 1;
		}
		ನಿಲ್ಲಿಸು;
	  }

	  ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid ;  idx += 1) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ aval = vp->value.vector[idx/32].aval;
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  ಪೂರ್ಣಾಂಕ bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t ಬಿಟ್ = scalar_ಗಮ್ಯ_bit4(bitmask);
		  val.ಗಣ_ಬಿಟ್(idx, ಬಿಟ್);
	    }
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiBinStrVal:
	    vpip_bin_str_ಗಮ್ಯ_vec4(val, vp->value.str);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiOctStrVal:
	    vpip_oct_str_ಗಮ್ಯ_vec4(val, vp->value.str);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiDecStrVal:
	    vpip_dec_str_ಗಮ್ಯ_vec4(val, vp->value.str);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiHexStrVal:
	    vpip_hex_str_ಗಮ್ಯ_vec4(val, vp->value.str);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiScalarVal:
	    val.ಗಣ_ಬಿಟ್(0, scalar_ಗಮ್ಯ_bit4(vp->value.scalar));
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (ಪೂರ್ಣಾಂಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);

      }

      ಹಿಂತಿರುಗಿ val;
}

ಪೂರ್ಣಾಂಕ __vpiSignal::vpi_get(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ signal_get(code, ಇದು); }

int64_t __vpiSignal::vpi_get64(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ signal_get64(code, ಇದು); }

ಅಕ್ಷರ* __vpiSignal::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ signal_get_str(code, ಇದು); }

ಶೂನ್ಯ __vpiSignal::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ signal_get_ಮೌಲ್ಯ(ಇದು, val); }

vpiHandle __vpiSignal::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags)
{ ಹಿಂತಿರುಗಿ signal_put_ಮೌಲ್ಯ(ಇದು, val, flags); }

vpiHandle __vpiSignal::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ signal_get_handle(code, ಇದು); }

vpiHandle __vpiSignal::vpi_iterate(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ signal_iterate(code, ಇದು); }

vpiHandle __vpiSignal::vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ idx)
{ ಹಿಂತಿರುಗಿ signal_ಸೂಚ್ಯಂಕ(idx, ಇದು); }

ರಚನೆ signal_reg : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಸಾಲೊಳಗೆ signal_reg() { }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiReg; }
};

ರಚನೆ signal_integer : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಸಾಲೊಳಗೆ signal_integer() { }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiIntegerVar; }
};

ರಚನೆ signal_net : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಸಾಲೊಳಗೆ signal_net() { }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiNet; }
};

ರಚನೆ signal_byte : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಸಾಲೊಳಗೆ signal_byte() { }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiByteVar; }
};

ರಚನೆ signal_bitvar : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಸಾಲೊಳಗೆ signal_bitvar() { }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpi_ಬಿಟ್_Var; }
};

ರಚನೆ signal_shortint : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಸಾಲೊಳಗೆ signal_shortint() { }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiShortIntVar; }
};

ರಚನೆ signal_int : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಸಾಲೊಳಗೆ signal_int() { }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiIntVar; }
};

ರಚನೆ signal_longint : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಸಾಲೊಳಗೆ signal_longint() { }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = ಹೊಸ signal_integer;
      ಹಿಂತಿರುಗಿ fill_ಒಳ_var4(obj, name, msb, lsb, ನಿಜ, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb, ತಾರ್ಕಿಕ signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      ಆದರೆ ((! signed_flag) || (lsb != 0) ) {
	    obj = ಹೊಸ signal_bitvar;
      } ಇಲ್ಲದಿದ್ದರೆ {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    ಆಯ್ಕೆ (msb) {
		ಸ್ಥಿತಿ 7:
		  obj = ಹೊಸ signal_byte;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 15:
		  obj = ಹೊಸ signal_shortint;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 31:
		  obj = ಹೊಸ signal_int;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 63:
		  obj = ಹೊಸ signal_longint;
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = ಹೊಸ signal_bitvar;
		  ನಿಲ್ಲಿಸು;
	    }
      }

      ಹಿಂತಿರುಗಿ fill_ಒಳ_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb,
			ತಾರ್ಕಿಕ signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = ಹೊಸ signal_reg;
      ಹಿಂತಿರುಗಿ fill_ಒಳ_var4(obj, name, msb, lsb, signed_flag, vec);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಸ್ಥಿರ ರಚನೆ vpiSignal_plug **signal_pool = 0;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ signal_pool_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ signal_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ signal_dels = 0;
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ vpiSignal_plug {
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ space[ಗಾತ್ರ (ರಚನೆ __vpiSignal)];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ರಚನೆ vpiSignal_plug *pool;
#ಆದರೆ_ಅಂತ್ಯ
};

ಶೂನ್ಯ* __vpiSignal::ಕ್ರಿಯಾಕಾರಕ ಹೊಸ(size_t siz)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಪ್ರತಿಜ್ಞಾ(siz == ಗಾತ್ರ(ರಚನೆ vpiSignal_plug) - ಗಾತ್ರ(ರಚನೆ vpiSignal_plug *));
#ಇಲ್ಲದಿದ್ದರೆ
      ಪ್ರತಿಜ್ಞಾ(siz == ಗಾತ್ರ(ರಚನೆ vpiSignal_plug));
#ಆದರೆ_ಅಂತ್ಯ
      ಸ್ಥಿರ ರಚನೆ vpiSignal_plug*alloc_array = 0;
      ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ alloc_ಸೂಚ್ಯಂಕ = 0;
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ alloc_ಎಣಿಕೆ = 512;

      ಆದರೆ ((alloc_array == 0) || (alloc_ಸೂಚ್ಯಂಕ == alloc_ಎಣಿಕೆ)) {
	    alloc_array = static_cast<ರಚನೆ vpiSignal_plug*>
	                  (ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(alloc_ಎಣಿಕೆ, ಗಾತ್ರ(ರಚನೆ vpiSignal_plug)));
	    alloc_ಸೂಚ್ಯಂಕ = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_ಎಣಿಕೆ *
	                                            ಗಾತ್ರ(ರಚನೆ vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_ಎಣಿಕೆ += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_ಎಣಿಕೆ*ಗಾತ್ರ(vpiSignal_plug **)));
	    signal_pool[signal_pool_ಎಣಿಕೆ-1] = alloc_array;
#ಆದರೆ_ಅಂತ್ಯ
      }

      ರಚನೆ vpiSignal_plug*cur = alloc_array + alloc_ಸೂಚ್ಯಂಕ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, ಗಾತ್ರ(ರಚನೆ vpiSignal_plug));
      cur->pool = alloc_array;
      signal_ಎಣಿಕೆ += 1;
#ಆದರೆ_ಅಂತ್ಯ
      alloc_ಸೂಚ್ಯಂಕ += 1;
      ಹಿಂತಿರುಗಿ cur;
}

ಶೂನ್ಯ __vpiSignal::ಕ್ರಿಯಾಕಾರಕ ಅಳಿಸು(ಶೂನ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ signal_delete(vpiHandle ಅಂಶ)
{
      ರಚನೆ __vpiSignal *obj = static_cast<__vpiSignal *> (ಅಂಶ);
      ಪ್ರತಿಜ್ಞಾ(obj->ಗಂಟು->fil);
      obj->ಗಂಟು->fil->clear_all_callbacks();
      vvp_net_delete(obj->ಗಂಟು);
      ಆದರೆ (obj->ಬಿಟ್ಟುಗಳು) {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx=0; idx<obj->width(); idx+=1) {
		  ಅಳಿಸು obj->ಬಿಟ್ಟುಗಳು[idx].ಸೂಚ್ಯಂಕ;
	    }
	    obj->ಬಿಟ್ಟುಗಳು -= 1;
	    ಅಳಿಸು [] obj->ಬಿಟ್ಟುಗಳು;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

ಶೂನ್ಯ signal_pool_delete()
{
      ಆದರೆ (RUNNING_ON_VALGRIND && (signal_ಎಣಿಕೆ != signal_dels)) {
	    fflush(ಖಾಲಿ);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (ದೀರ್ಘ) signal_ಎಣಿಕೆ - signal_dels, signal_ಎಣಿಕೆ);
      }

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < signal_pool_ಎಣಿಕೆ; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    ಬಿಡುಗಡೆ(signal_pool[idx]);
      }

      ಬಿಡುಗಡೆ(signal_pool);
      signal_pool = 0;
      signal_pool_ಎಣಿಕೆ = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
ಸ್ಥಿರ vpiHandle fill_ಒಳ_net4(ರಚನೆ __vpiSignal*obj, __vpiScope*scope,
			      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb,
			      ತಾರ್ಕಿಕ signed_flag, vvp_net_t*ಗಂಟು)
{
      obj->id.name = name? vpip_name_string(name) : ಖಾಲಿ;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->ಗಂಟು = ಗಂಟು;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      ಎಣಿಕೆ_vpi_nets += 1;

      ಹಿಂತಿರುಗಿ obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb,
			 ತಾರ್ಕಿಕ signed_flag, vvp_net_t*ಗಂಟು)
{
      ರಚನೆ __vpiSignal*obj = ಹೊಸ signal_net;
      ಹಿಂತಿರುಗಿ fill_ಒಳ_net4(obj, scope, name, msb, lsb, signed_flag, ಗಂಟು);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ PV_get_ತಳ(ರಚನೆ __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      ಆದರೆ (rfp->sbase != 0) {
	    s_vpi_ಮೌಲ್ಯ val;
	      /* Check to see if the value is defined. */
	    val.format = vpi_ಸದಿಶ_Val;
	    vpi_get_ಮೌಲ್ಯ(rfp->sbase, &val);
	    ಪೂರ್ಣಾಂಕ ಪದಗಳು = (vpi_get(vpi_ಗಾತ್ರ, rfp->sbase)-1)/32 + 1;
	    ಗಾಗಿ(ಪೂರ್ಣಾಂಕ idx = 0; idx < ಪದಗಳು; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  ಆದರೆ (val.value.vector[idx].bval != 0) ಹಿಂತಿರುಗಿ INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_ಮೌಲ್ಯ(rfp->sbase, &val);
	    ಹಿಂತಿರುಗಿ val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      ಹಿಂತಿರುಗಿ rfp->tbase;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ PV_get(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಪೂರ್ಣಾಂಕ rval = 0;
      ಆಯ್ಕೆ (code) {
	ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ 0;  // Not implemented for now!

	ಸ್ಥಿತಿ vpiSigned:
	    ಹಿಂತಿರುಗಿ 0;  // A part/bit select is always unsigned!

	ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ rfp->width;

	  /* This is like the &A<> in array.cc. */
	ಸ್ಥಿತಿ vpiConstantSelect:
	    ಹಿಂತಿರುಗಿ rfp->sbase == 0;

	ಸ್ಥಿತಿ vpi_ಎಡ_Range:
            rval += rfp->width - 1;
	    // fallthrough
	ಸ್ಥಿತಿ vpi_ಬಲ_Range: {
	    ಪೂರ್ಣಾಂಕ ಎಡ = vpi_get(vpi_ಎಡ_Range, rfp->ಪೋಷಕ);
	    ಪೂರ್ಣಾಂಕ ಬಲ = vpi_get(vpi_ಬಲ_Range, rfp->ಪೋಷಕ);
	    rval += PV_get_ತಳ(rfp);
	    ಹಿಂತಿರುಗಿ ಎಡ >= ಬಲ ? ಬಲ + rval : ಬಲ - rval;
	}

        ಸ್ಥಿತಿ vpiAutomatic:
            ಹಿಂತಿರುಗಿ vpi_get(vpiAutomatic, rfp->ಪೋಷಕ);

#ಆದರೆ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        ಸ್ಥಿತಿ _vpiFromThr:
            ಹಿಂತಿರುಗಿ _vpi_at_PV;
#ಆದರೆ_ಅಂತ್ಯ

	ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಅಕ್ಷರ* PV_get_str(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆಯ್ಕೆ (code) {
	ಸ್ಥಿತಿ vpiFile:  // Not implemented for now!
	    ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[0]);

	ಸ್ಥಿತಿ vpiName:
	ಸ್ಥಿತಿ vpi_ತುಂಬಿದ_Name: {
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nm = vpi_get_str(code, rfp->ಪೋಷಕ);
	    size_t len = 256+ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(nm);
	    ಅಕ್ಷರ *ತುಂಬಿದ = static_cast<ಅಕ್ಷರ *>(ಸ್ಮರಣೆ_ಕೊಡು(len));
	    snprintf(ತುಂಬಿದ, len, "%s[%d:%d]", nm,
	                                     (ಪೂರ್ಣಾಂಕ)vpi_get(vpi_ಎಡ_Range, ref),
	                                     (ಪೂರ್ಣಾಂಕ)vpi_get(vpi_ಬಲ_Range, ref));
	    ತುಂಬಿದ[len-1] = 0;
	    ಅಕ್ಷರ *res = simple_ಗಣ_rbuf_str(ತುಂಬಿದ);
	    ಬಿಡುಗಡೆ(ತುಂಬಿದ);
	    ಹಿಂತಿರುಗಿ res;
	}

	ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ PV_get_ಮೌಲ್ಯ(vpiHandle ref, p_vpi_ಮೌಲ್ಯ vp)
{
      ರಚನೆ __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(rfp->net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);

      ಆಯ್ಕೆ (vp->format) {

	  ಸ್ಥಿತಿ vpiIntVal:
	    format_vpiIntVal(sig, PV_get_ತಳ(rfp), rfp->width, 0, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_ತಳ(rfp), rfp->width, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_ತಳ(rfp), rfp->width, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_ತಳ(rfp), rfp->width, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_ತಳ(rfp), rfp->width, 0, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiStringVal:
	    format_vpiStringVal(sig, PV_get_ತಳ(rfp), rfp->width, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_ತಳ(rfp), vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_ತಳ(rfp), rfp->width, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:
	    format_vpi_ಸದಿಶ_Val(sig, PV_get_ತಳ(rfp), rfp->width, vp);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiRealVal:
	    format_vpiRealVal(sig, PV_get_ತಳ(rfp), rfp->width, 0, vp);
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (ಪೂರ್ಣಾಂಕ)vp->format, vpi_get_str(vpi_ತುಂಬಿದ_Name, rfp->ಪೋಷಕ));
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

ಸ್ಥಿರ vpiHandle PV_put_ಮೌಲ್ಯ(vpiHandle ref, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ flags)
{
      ರಚನೆ __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);
      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(rfp->net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);

      ಚಿಹ್ನೆರಹಿತ sig_ಗಾತ್ರ = sig->ಮೌಲ್ಯ_ಗಾತ್ರ();
      ಚಿಹ್ನೆರಹಿತ width = rfp->width;
      ಪೂರ್ಣಾಂಕ ತಳ = PV_get_ತಳ(rfp);
      ಆದರೆ (ತಳ >= (ಚಿಹ್ನೆಯುಳ್ಳ) sig_ಗಾತ್ರ) ಹಿಂತಿರುಗಿ 0;
      ಆದರೆ (ತಳ + (ಚಿಹ್ನೆಯುಳ್ಳ) width < 0) ಹಿಂತಿರುಗಿ 0;

      vvp_vector4_t val;
      ಆದರೆ (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_ಮೌಲ್ಯ(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      ಆದರೆ (ತಳ < 0) {
	    width += ತಳ;
	    ಆದರೆ (flags != vpiReleaseFlag) {
		  val = val.subvalue(-ತಳ, width);
	    }
	    ತಳ = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      ಆದರೆ (ತಳ+width > sig_ಗಾತ್ರ) {
	    width = sig_ಗಾತ್ರ - ತಳ;
	    ಆದರೆ (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      ಪ್ರತಿಜ್ಞಾ(rfp->ಪೋಷಕ);
      ತಾರ್ಕಿಕ net_flag = rfp->ಪೋಷಕ->get_type_ಸಂಕೇತ()==vpiNet;
      ತಾರ್ಕಿಕ ತುಂಬಿದ_sig = ತಳ == 0 && width == sig_ಗಾತ್ರ;

      vvp_net_ptr_t dest(rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value.  Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value.*/
      ಆದರೆ (flags == vpiReleaseFlag) {
	    ಪ್ರತಿಜ್ಞಾ(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    ಆದರೆ (ತುಂಬಿದ_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  rfp->net->fil->release_pv(dest, ತಳ, width, net_flag);
	    }
	    rfp->net->fun->force_flag(ನಿಜ);
	    PV_get_ಮೌಲ್ಯ(ref, vp);
	    ಹಿಂತಿರುಗಿ ref;
      }

      ಆದರೆ (flags == vpiForceFlag) {
	    ಆದರೆ (ತುಂಬಿದ_sig) {
		  vvp_vector2_t ಮುಖವಾಡ (vvp_vector2_t::FILL1, sig_ಗಾತ್ರ);
		  rfp->net->force_vec4(val, ಮುಖವಾಡ);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  vvp_vector2_t ಮುಖವಾಡ (vvp_vector2_t::FILL0, sig_ಗಾತ್ರ);
		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < width ; idx += 1)
		        ಮುಖವಾಡ.ಗಣ_ಬಿಟ್(ತಳ+idx, 1);

		  vvp_vector4_t tmp (sig_ಗಾತ್ರ, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_ಮೌಲ್ಯ(tmp);

		  tmp.ಗಣ_vec(ತಳ, val);
		  rfp->net->force_vec4(tmp, ಮುಖವಾಡ);
	    }
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (net_flag && !dynamic_cast<vvp_island_ಪೋರ್ಟ್*>(rfp->net->fun)) {
	    ಆದರೆ (ತುಂಬಿದ_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  rfp->net->send_vec4_pv(val, ತಳ, sig_ಗಾತ್ರ,
		                         vthread_get_wt_context());
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಆದರೆ (ತುಂಬಿದ_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  vvp_send_vec4_pv(dest, val, ತಳ, sig_ಗಾತ್ರ,
	                           vthread_get_wt_context());
	    }
      }
      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ vpiHandle PV_get_handle(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಪೋಷಕ:
	    ಹಿಂತಿರುಗಿ rfp->ಪೋಷಕ;

	  ಸ್ಥಿತಿ vpiScope:
	    ಹಿಂತಿರುಗಿ vpi_handle(vpiScope, rfp->ಪೋಷಕ);

	  ಸ್ಥಿತಿ vpiModule:
	    ಹಿಂತಿರುಗಿ vpi_handle(vpiModule, rfp->ಪೋಷಕ);
      }

      ಹಿಂತಿರುಗಿ 0;
}

ಸಾಲೊಳಗೆ __vpiPV::__vpiPV()
{ }

ಪೂರ್ಣಾಂಕ __vpiPV::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiPartSelect; }

ಪೂರ್ಣಾಂಕ __vpiPV::vpi_get(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ PV_get(code, ಇದು); }

ಅಕ್ಷರ* __vpiPV::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ PV_get_str(code, ಇದು); }

ಶೂನ್ಯ __vpiPV::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ PV_get_ಮೌಲ್ಯ(ಇದು, val); }

vpiHandle __vpiPV::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags)
{ ಹಿಂತಿರುಗಿ PV_put_ಮೌಲ್ಯ(ಇದು, val, flags); }

vpiHandle __vpiPV::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ PV_get_handle(code, ಇದು); }

vpiHandle vpip_make_PV(ಅಕ್ಷರ*var, ಪೂರ್ಣಾಂಕ ತಳ, ಪೂರ್ಣಾಂಕ width)
{
      ರಚನೆ __vpiPV*obj = ಹೊಸ __vpiPV;
      compile_vpi_lookup(&obj->ಪೋಷಕ, strdup(var));
      obj->sbase = 0;
      obj->tbase = ತಳ;
      obj->width = (ಚಿಹ್ನೆರಹಿತ) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ಹಿಂತಿರುಗಿ obj;
}

vpiHandle vpip_make_PV(ಅಕ್ಷರ*var, ಅಕ್ಷರ*ಚಿಹ್ನೆ, ಪೂರ್ಣಾಂಕ width)
{
      ರಚನೆ __vpiPV*obj = ಹೊಸ __vpiPV;
      compile_vpi_lookup(&obj->ಪೋಷಕ, strdup(var));
      compile_vpi_lookup(&obj->sbase, ಚಿಹ್ನೆ);
      obj->tbase = 0;
      obj->width = (ಚಿಹ್ನೆರಹಿತ) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ಹಿಂತಿರುಗಿ obj;
}

vpiHandle vpip_make_PV(ಅಕ್ಷರ*var, vpiHandle handle, ಪೂರ್ಣಾಂಕ width)
{
      ರಚನೆ __vpiPV*obj = ಹೊಸ __vpiPV;
      compile_vpi_lookup(&obj->ಪೋಷಕ, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (ಚಿಹ್ನೆರಹಿತ) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ಹಿಂತಿರುಗಿ obj;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ PV_delete(vpiHandle ಅಂಶ)
{
      ರಚನೆ __vpiPV *obj = dynamic_cast<__vpiPV*>(ಅಂಶ);
      ಆದರೆ (obj->sbase) {
	    ಆಯ್ಕೆ (obj->sbase->get_type_ಸಂಕೇತ()) {
		ಸ್ಥಿತಿ vpi_ಸ್ಮರಣೆ_ಶಬ್ದ:
		  ಆದರೆ (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiPartSelect:
		  ಪ್ರತಿಜ್ಞಾ(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  ನಿಲ್ಲಿಸು;
	    }
      }
      ಪ್ರತಿಜ್ಞಾ(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      ಅಳಿಸು obj;
}
#ಆದರೆ_ಅಂತ್ಯ
