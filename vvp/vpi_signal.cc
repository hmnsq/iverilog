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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <cmath>
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <climits>
# include  <cstring>
# include  <cassert>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
#ਜੇ_ਅੰਤ
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
ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ hex_digits[256];
ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
ਖਾਲੀ *need_ਨਤੀਜਾ_buf(ਬਿਨਾਨਿਸ਼ਾਨੀ cnt, vpi_rbuf_t type)
{
      ਸਥਿਰ ਖਾਲੀ*ਨਤੀਜਾ_buf[2] = {0, 0};
      ਸਥਿਰ size_t ਨਤੀਜਾ_buf_ਮਾਪ[2] = {0, 0};

      ਜੇ (type == RBUF_DEL) {
	    ਮੁਕਤ(ਨਤੀਜਾ_buf[RBUF_VAL]);
	    ਨਤੀਜਾ_buf[RBUF_VAL] = 0;
	    ਨਤੀਜਾ_buf_ਮਾਪ[RBUF_VAL] = 0;

	    ਮੁਕਤ(ਨਤੀਜਾ_buf[RBUF_STR]);
	    ਨਤੀਜਾ_buf[RBUF_STR] = 0;
	    ਨਤੀਜਾ_buf_ਮਾਪ[RBUF_STR] = 0;

	    ਵਾਪਸ 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      ਜੇ (ਨਤੀਜਾ_buf_ਮਾਪ[type] == 0) {
	    ਨਤੀਜਾ_buf[type] = ਸਮ੍ਰਿਤੀ_ਦਿਓ(cnt);
	    ਨਤੀਜਾ_buf_ਮਾਪ[type] = cnt;
      } ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ_buf_ਮਾਪ[type] < cnt) {
	    ਨਤੀਜਾ_buf[type] = realloc(ਨਤੀਜਾ_buf[type], cnt);
	    ਨਤੀਜਾ_buf_ਮਾਪ[type] = cnt;
      }

      ਵਾਪਸ ਨਤੀਜਾ_buf[type];
}

ਅੱਖਰ *simple_ਸੈੱਟ_rbuf_str(ਅਚਲ ਅੱਖਰ *s1)
{
      ਅੱਖਰ *res = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(ਲੜੀ_ਲੰਬਾਈ(s1)+1, RBUF_STR));
      ਜੇ (res) strcpy(res,s1);
      ਵਾਪਸ res;
}

ਅੱਖਰ *generic_get_str(ਪੂਰਨ code, vpiHandle ref, ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *ਸੂਚਕਾਂਕ)
{
      size_t len = ਲੜੀ_ਲੰਬਾਈ(name) + 1;  /* include space for null termination */
      ਅੱਖਰ *bn = ਨਿਰਾ;
      ਤਾਰਕਿਕ is_bn_a_pkg = ਝੂਠ;
      ਜੇ (code == vpi_ਭਰਿਆ_Name) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = ਲੜੀ_ਲੰਬਾਈ(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    ਜੇ (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      ਜੇ (ਸੂਚਕਾਂਕ != ਨਿਰਾ) len += ਲੜੀ_ਲੰਬਾਈ(ਸੂਚਕਾਂਕ) + 2;  /* include space for brackets */

      ਅੱਖਰ *res = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(len, RBUF_STR));
      ਜੇ (!res) {
	    ਮੁਕਤ(bn);
	    ਵਾਪਸ ਨਿਰਾ;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      ਜੇ (bn != ਨਿਰਾ) {
	    strcat(res, bn);
	      // A package already has the "::" separator in the name
	    ਜੇ (! is_bn_a_pkg) strcat(res, ".");
	    ਮੁਕਤ(bn);
      }
      strcat(res, name);
      ਜੇ (ਸੂਚਕਾਂਕ != ਨਿਰਾ) {
	    strcat(res, "[");
	    strcat(res, ਸੂਚਕਾਂਕ);
	    strcat(res, "]");
      }
      ਵਾਪਸ res;
}

ਸਥਿਰ vpiHandle fill_ਵਿੱਚ_net4(ਬਣਤਰ __vpiSignal*obj, __vpiScope*scope,
                              ਅਚਲ ਅੱਖਰ*name, ਪੂਰਨ msb, ਪੂਰਨ lsb,
                              ਤਾਰਕਿਕ signed_flag, vvp_net_t*ਗੰਢ);

ਸਥਿਰ vpiHandle fill_ਵਿੱਚ_var4(ਬਣਤਰ __vpiSignal*obj,
                              ਅਚਲ ਅੱਖਰ*name, ਪੂਰਨ msb, ਪੂਰਨ lsb,
                              ਤਾਰਕਿਕ signed_flag, vvp_net_t*ਗੰਢ)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      ਵਾਪਸ fill_ਵਿੱਚ_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, ਗੰਢ);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

ਸਥਿਰ ਖਾਲੀ format_vpiBinStrVal(ਅਚਲ vvp_signal_ਮੁੱਲ*sig, ਪੂਰਨ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
                                s_vpi_ਮੁੱਲ*vp)
{
      ਅੱਖਰ *rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(wid+1, RBUF_VAL));
      ਲੰਮਾ ਸਿਰਾ = ਅਧਾਰ + (ਨਿਸ਼ਾਨੀਵਾਲਾ)wid;
      ਲੰਮਾ ਵਿੱਥ_2 = ਸਿਰਾ - 1;
      ਲੰਮਾ ssize = (ਨਿਸ਼ਾਨੀਵਾਲਾ)sig->ਮੁੱਲ_ਮਾਪ();

      ਲਈ (ਲੰਮਾ idx = ਅਧਾਰ ;  idx < ਸਿਰਾ ;  idx += 1) {
	    ਜੇ (idx < 0 || idx >= ssize) {
                  rbuf[ਵਿੱਥ_2-idx] = 'x';
	    } ਨਹੀਂਤਾਂ {
                  rbuf[ਵਿੱਥ_2-idx] = vvp_bit4_ਮੰਜ਼ਿਲ_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

ਸਥਿਰ ਖਾਲੀ format_vpiOctStrVal(ਅਚਲ vvp_signal_ਮੁੱਲ*sig, ਪੂਰਨ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
                                s_vpi_ਮੁੱਲ*vp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ dwid = (wid + 2) / 3;
      ਅੱਖਰ *rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(dwid+1, RBUF_VAL));
      ਲੰਮਾ ਸਿਰਾ = ਅਧਾਰ + (ਨਿਸ਼ਾਨੀਵਾਲਾ)wid;
      ਲੰਮਾ ssize = (ਨਿਸ਼ਾਨੀਵਾਲਾ)sig->ਮੁੱਲ_ਮਾਪ();
      ਬਿਨਾਨਿਸ਼ਾਨੀ val = 0;

      rbuf[dwid] = 0;
      ਲਈ (ਲੰਮਾ idx = ਅਧਾਰ ;  idx < ਸਿਰਾ ;  idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ = 0;
	    ਜੇ (idx < 0 || idx >= ssize) {
                  ਦੋਆਧਾਰੀ_ਅੰਕ = 2; // BIT4_X
	    } ਨਹੀਂਤਾਂ {
                  ਚੋਣ (sig->value(idx)) {
		      ਹਾਲਤ BIT4_0:
			ਦੋਆਧਾਰੀ_ਅੰਕ = 0;
			ਰੋਕੋ;
		      ਹਾਲਤ BIT4_1:
			ਦੋਆਧਾਰੀ_ਅੰਕ = 1;
			ਰੋਕੋ;
		      ਹਾਲਤ BIT4_X:
			ਦੋਆਧਾਰੀ_ਅੰਕ = 2;
			ਰੋਕੋ;
		      ਹਾਲਤ BIT4_Z:
			ਦੋਆਧਾਰੀ_ਅੰਕ = 3;
			ਰੋਕੋ;
                  }
	    }
	    val |= ਦੋਆਧਾਰੀ_ਅੰਕ << 2*((idx-ਅਧਾਰ) % 3);

	    ਜੇ ((idx-ਅਧਾਰ) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ਚੋਣ (wid % 3) {
	  ਹਾਲਤ 1:
	    ਜੇ (val == 2) val = 42;
	    ਨਹੀਂਤਾਂ ਜੇ (val == 3) val = 63;
	    ਰੋਕੋ;
	  ਹਾਲਤ 2:
	    ਜੇ (val == 10) val = 42;
	    ਨਹੀਂਤਾਂ ਜੇ (val == 15) val = 63;
	    ਰੋਕੋ;
      }

      ਜੇ (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

ਸਥਿਰ ਖਾਲੀ format_vpiHexStrVal(ਅਚਲ vvp_signal_ਮੁੱਲ*sig, ਪੂਰਨ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
                                s_vpi_ਮੁੱਲ*vp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ dwid = (wid + 3) / 4;
      ਅੱਖਰ *rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(dwid+1, RBUF_VAL));
      ਲੰਮਾ ਸਿਰਾ = ਅਧਾਰ + (ਨਿਸ਼ਾਨੀਵਾਲਾ)wid;
      ਲੰਮਾ ssize = (ਨਿਸ਼ਾਨੀਵਾਲਾ)sig->ਮੁੱਲ_ਮਾਪ();
      ਬਿਨਾਨਿਸ਼ਾਨੀ val = 0;

      rbuf[dwid] = 0;
      ਲਈ (ਲੰਮਾ idx = ਅਧਾਰ ;  idx < ਸਿਰਾ ;  idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ = 0;
	    ਜੇ (idx < 0 || idx >= ssize) {
                  ਦੋਆਧਾਰੀ_ਅੰਕ = 2; // BIT4_X
	    } ਨਹੀਂਤਾਂ {
                  ਚੋਣ (sig->value(idx)) {
		      ਹਾਲਤ BIT4_0:
			ਦੋਆਧਾਰੀ_ਅੰਕ = 0;
			ਰੋਕੋ;
		      ਹਾਲਤ BIT4_1:
			ਦੋਆਧਾਰੀ_ਅੰਕ = 1;
			ਰੋਕੋ;
		      ਹਾਲਤ BIT4_X:
			ਦੋਆਧਾਰੀ_ਅੰਕ = 2;
			ਰੋਕੋ;
		      ਹਾਲਤ BIT4_Z:
			ਦੋਆਧਾਰੀ_ਅੰਕ = 3;
			ਰੋਕੋ;
                  }
	    }
	    val |= ਦੋਆਧਾਰੀ_ਅੰਕ << 2*((idx-ਅਧਾਰ) % 4);

	    ਜੇ ((idx-ਅਧਾਰ) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ਚੋਣ (wid % 4) {
	  ਹਾਲਤ 1:
	    ਜੇ (val == 2) val = 170;
	    ਨਹੀਂਤਾਂ ਜੇ (val == 3) val = 255;
	    ਰੋਕੋ;
	  ਹਾਲਤ 2:
	    ਜੇ (val == 10) val = 170;
	    ਨਹੀਂਤਾਂ ਜੇ (val == 15) val = 255;
	    ਰੋਕੋ;
	  ਹਾਲਤ 3:
	    ਜੇ (val == 42) val = 170;
	    ਨਹੀਂਤਾਂ ਜੇ (val == 63) val = 255;
	    ਰੋਕੋ;
      }

      ਜੇ (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

ਸਥਿਰ ਖਾਲੀ format_vpiDecStrVal(ਅਚਲ vvp_signal_ਮੁੱਲ*sig, ਪੂਰਨ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
                                ਪੂਰਨ signed_flag, s_vpi_ਮੁੱਲ*vp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ hwid = (sig->ਮੁੱਲ_ਮਾਪ()+2) / 3 + 1;
      ਅੱਖਰ *rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(hwid, RBUF_VAL));
      ਲੰਮਾ ssize = (ਨਿਸ਼ਾਨੀਵਾਲਾ)sig->ਮੁੱਲ_ਮਾਪ();
      ਲੰਮਾ ਸਿਰਾ = ਅਧਾਰ + (ਨਿਸ਼ਾਨੀਵਾਲਾ)wid;

	/* Do we have an end outside of the real signal vector. */
      ਜੇ (ਅਧਾਰ < 0 || ਸਿਰਾ > ssize) {
	    ਤਾਰਕਿਕ all_x = ਸੱਚ;
	    ਜੇ (ਸਿਰਾ > ssize) ਸਿਰਾ = ssize;
	    ਜੇ (ਅਧਾਰ < 0) ਅਧਾਰ = 0;
	    ਲਈ (ਲੰਮਾ idx = ਅਧਾਰ ;  idx < ਸਿਰਾ ;  idx += 1) {
		  ਜੇ (sig->value(idx) != BIT4_X) {
			all_x = ਝੂਠ;
			ਰੋਕੋ;
		  }
	    }

	    ਜੇ (all_x) {
		  rbuf[0] = 'x';
	    } ਨਹੀਂਤਾਂ {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    ਵਾਪਸ;
      }

      vvp_vector4_t vec4;
      ਜੇ (ਅਧਾਰ == 0 && ਸਿਰਾ == ssize) {
	    sig->vec4_ਮੁੱਲ(vec4);
      } ਨਹੀਂਤਾਂ {
	    vvp_vector4_t tmp;
	    sig->vec4_ਮੁੱਲ(tmp);
	    vec4 = tmp.subvalue(ਅਧਾਰ, wid);
      }

      vpip_vec4_ਮੰਜ਼ਿਲ_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

ਸਥਿਰ ਖਾਲੀ format_vpiIntVal(ਅਚਲ vvp_signal_ਮੁੱਲ*sig, ਪੂਰਨ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
                             ਪੂਰਨ signed_flag, s_vpi_ਮੁੱਲ*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_ਮੁੱਲ(tmp);
      vvp_vector4_t sub = tmp.subvalue(ਅਧਾਰ, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      ਜੇ (ਆਕਾਰ(vp->value.integer) == ਆਕਾਰ(int32_t)) {
	    int32_t val = 0;
	    vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(sub, val, signed_flag, ਝੂਠ);
	    vp->value.integer = val;
      } ਨਹੀਂਤਾਂ {
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਆਕਾਰ(vp->value.integer) == ਆਕਾਰ(int64_t));
	    int64_t val = 0;
	    vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(sub, val, signed_flag, ਝੂਠ);
	    vp->value.integer = val;
      }
}

ਸਥਿਰ ਖਾਲੀ format_vpiRealVal(ਅਚਲ vvp_signal_ਮੁੱਲ*sig, ਪੂਰਨ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
                              ਪੂਰਨ signed_flag, s_vpi_ਮੁੱਲ*vp)
{
      vvp_vector4_t vec4(wid);
      ਲੰਮਾ ssize = (ਨਿਸ਼ਾਨੀਵਾਲਾ)sig->ਮੁੱਲ_ਮਾਪ();
      ਲੰਮਾ ਸਿਰਾ = ਅਧਾਰ + (ਨਿਸ਼ਾਨੀਵਾਲਾ)wid;
      ਜੇ (ਸਿਰਾ > ssize) ਸਿਰਾ = ssize;

      ਲਈ (ਲੰਮਾ idx = (ਅਧਾਰ < 0) ? 0 : ਅਧਾਰ ;  idx < ਸਿਰਾ ;  idx += 1) {
	    vec4.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx-ਅਧਾਰ, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(vec4, vp->value.real, signed_flag);
}

ਸਥਿਰ ਖਾਲੀ format_vpiStringVal(ਅਚਲ vvp_signal_ਮੁੱਲ*sig, ਪੂਰਨ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
                                s_vpi_ਮੁੱਲ*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      ਅੱਖਰ *rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      ਅੱਖਰ *cp = rbuf;

      ਅੱਖਰ tmp = 0;
      ਲਈ (ਲੰਮਾ idx = ਅਧਾਰ+(ਨਿਸ਼ਾਨੀਵਾਲਾ)wid-1; idx >= ਅਧਾਰ; idx -= 1) {
	    tmp <<= 1;

	    ਜੇ (idx >=0 && idx < (ਨਿਸ਼ਾਨੀਵਾਲਾ)sig->ਮੁੱਲ_ਮਾਪ() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    ਜੇ (((idx-ਅਧਾਰ)&7)==0){
		  /* Skip leading nulls. */
		  ਜੇ (tmp == 0 && cp == rbuf)
			ਜਾਰੀ;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

ਸਥਿਰ ਖਾਲੀ format_vpiScalarVal(ਅਚਲ vvp_signal_ਮੁੱਲ*sig, ਪੂਰਨ ਅਧਾਰ,
                                s_vpi_ਮੁੱਲ*vp)
{
      ਜੇ (ਅਧਾਰ >= 0 && ਅਧਾਰ < (ਨਿਸ਼ਾਨੀਵਾਲਾ)sig->ਮੁੱਲ_ਮਾਪ()) {
	    ਚੋਣ (sig->value(ਅਧਾਰ)) {
		ਹਾਲਤ BIT4_0:
		  vp->value.scalar = vpi0;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  vp->value.scalar = vpi1;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_ਮੁੱਲ(ਅਧਾਰ);
		  ਜੇ (strn.strength0() == 1) vp->value.scalar = vpiH;
		  ਨਹੀਂਤਾਂ ਜੇ (strn.strength1() == 1) vp->value.scalar = vpiL;
		  ਨਹੀਂਤਾਂ vp->value.scalar = vpiX;
		  ਰੋਕੋ;
		}
		ਹਾਲਤ BIT4_Z:
		  vp->value.scalar = vpiZ;
		  ਰੋਕੋ;
	    }
      } ਨਹੀਂਤਾਂ {
	    vp->value.scalar = vpiX;
      }
}

ਸਥਿਰ ਖਾਲੀ format_vpiStrengthVal(ਅਚਲ vvp_signal_ਮੁੱਲ*sig, ਪੂਰਨ ਅਧਾਰ,
                                  ਬਿਨਾਨਿਸ਼ਾਨੀ wid, s_vpi_ਮੁੱਲ*vp)
{
      ਲੰਮਾ ਸਿਰਾ = ਅਧਾਰ + (ਨਿਸ਼ਾਨੀਵਾਲਾ)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_ਨਤੀਜਾ_buf(wid * ਆਕਾਰ(s_vpi_strengthval), RBUF_VAL));

      ਲਈ (ਲੰਮਾ idx = ਅਧਾਰ ;  idx < ਸਿਰਾ ;  idx += 1) {
	    ਜੇ (idx >=0 && idx < (ਨਿਸ਼ਾਨੀਵਾਲਾ)sig->ਮੁੱਲ_ਮਾਪ()) {
		  vvp_scalar_t val = sig->scalar_ਮੁੱਲ(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  ਬਿਨਾਨਿਸ਼ਾਨੀ s0 = 1 << val.strength0();
		  ਬਿਨਾਨਿਸ਼ਾਨੀ s1 = 1 << val.strength1();

		  ਚੋਣ (val.value()) {
		      ਹਾਲਤ BIT4_0:
			op[idx-ਅਧਾਰ].logic = vpi0;
			op[idx-ਅਧਾਰ].s0 = s0|s1;
			op[idx-ਅਧਾਰ].s1 = 0;
			ਰੋਕੋ;

		      ਹਾਲਤ BIT4_1:
			op[idx-ਅਧਾਰ].logic = vpi1;
			op[idx-ਅਧਾਰ].s0 = 0;
			op[idx-ਅਧਾਰ].s1 = s0|s1;
			ਰੋਕੋ;

		      ਹਾਲਤ BIT4_X:
			op[idx-ਅਧਾਰ].logic = vpiX;
			op[idx-ਅਧਾਰ].s0 = s0;
			op[idx-ਅਧਾਰ].s1 = s1;
			ਰੋਕੋ;

		      ਹਾਲਤ BIT4_Z:
			op[idx-ਅਧਾਰ].logic = vpiZ;
			op[idx-ਅਧਾਰ].s0 = vpiHiZ;
			op[idx-ਅਧਾਰ].s1 = vpiHiZ;
			ਰੋਕੋ;
		  }
	    } ਨਹੀਂਤਾਂ {
		  op[idx-ਅਧਾਰ].logic = vpiX;
		  op[idx-ਅਧਾਰ].s0 = vpiStrongDrive;
		  op[idx-ਅਧਾਰ].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

ਸਥਿਰ ਖਾਲੀ format_vpi_ਸਦਿਸ਼_Val(ਅਚਲ vvp_signal_ਮੁੱਲ*sig, ਪੂਰਨ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid,
                                s_vpi_ਮੁੱਲ*vp)
{
      ਲੰਮਾ ਸਿਰਾ = ਅਧਾਰ + (ਨਿਸ਼ਾਨੀਵਾਲਾ)wid;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ obit = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_ਨਤੀਜਾ_buf(hwid * ਆਕਾਰ(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      ਲਈ (ਲੰਮਾ idx = ਅਧਾਰ ;  idx < ਸਿਰਾ ;  idx += 1) {
	    ਜੇ (ਅਧਾਰ >= 0 && ਅਧਾਰ < (ਨਿਸ਼ਾਨੀਵਾਲਾ)sig->ਮੁੱਲ_ਮਾਪ()) {
		ਚੋਣ (sig->value(idx)) {
		ਹਾਲਤ BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  ਰੋਕੋ;
		}
	    } ਨਹੀਂਤਾਂ {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    ਜੇ (!(obit % 32)) {
		  op += 1;
		  ਜੇ ((op - vp->value.vector) < (ptrdiff_t)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
ਸਥਿਰ ਪੂਰਨ signal_get(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ 0;  // Not implemented for now!

	  ਹਾਲਤ vpiSigned:
	    ਵਾਪਸ rfp->signed_flag != 0;

	  ਹਾਲਤ vpiArray:
	    ਵਾਪਸ rfp->is_netarray != 0;

	  ਹਾਲਤ vpi_ਸੂਚਕਾਂਕ_2: // This only works while we have a single index.
	    ਜੇ (rfp->is_netarray) {
		  s_vpi_ਮੁੱਲ vp;
		  vp.format = vpiIntVal;
		  vpi_get_ਮੁੱਲ(rfp->id.ਸੂਚਕਾਂਕ, &vp);
		  ਵਾਪਸ vp.value.integer;
	    } ਨਹੀਂਤਾਂ {
		  ਵਾਪਸ vpiUndefined;
	    }

	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ rfp->width();

	  ਹਾਲਤ vpiNetType:
	    ਜੇ (ref->get_type_ਕੋਡ()==vpiNet)
		  ਵਾਪਸ vpiWire;
	    ਨਹੀਂਤਾਂ
		  ਵਾਪਸ vpiUndefined;

	  ਹਾਲਤ vpi_ਖੱਬਾ_Range:
            ਵਾਪਸ rfp->msb.get_ਮੁੱਲ();

	  ਹਾਲਤ vpi_ਸੱਜਾ_Range:
            ਵਾਪਸ rfp->lsb.get_ਮੁੱਲ();

	  ਹਾਲਤ vpiScalar:
	    ਵਾਪਸ (rfp->msb.get_ਮੁੱਲ() == 0 && rfp->lsb.get_ਮੁੱਲ() == 0);
	  ਹਾਲਤ vpi_ਸਦਿਸ਼:
	    ਵਾਪਸ (rfp->msb.get_ਮੁੱਲ() != rfp->lsb.get_ਮੁੱਲ());

          ਹਾਲਤ vpiAutomatic:
            ਵਾਪਸ vpip_scope(rfp)->is_automatic() ? 1  : 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BR916_STOPGAP_FIX
          ਹਾਲਤ _vpiFromThr:
            ਵਾਪਸ _vpiNoThr;
#ਜੇ_ਅੰਤ

	  ਮੂਲ:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    ਵਾਪਸ vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
ਸਥਿਰ int64_t signal_get64(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਚੋਣ (code) {
	    // This private property must return zero when undefined.
	  ਹਾਲਤ _vpiNexusId:
	    ਜੇ (rfp->msb.get_ਮੁੱਲ() == rfp->lsb.get_ਮੁੱਲ())
		  ਵਾਪਸ reinterpret_cast<int64_t>(rfp->ਗੰਢ);
	    ਵਾਪਸ 0;

	  ਮੂਲ:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    ਵਾਪਸ vpiUndefined;
      }
}

ਸਥਿਰ ਅੱਖਰ* signal_get_str(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਜੇ (code == vpiFile) {  // Not implemented for now!
	    ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[0]);
      }

      ਜੇ ((code != vpiName) && (code != vpi_ਭਰਿਆ_Name)) ਵਾਪਸ ਨਿਰਾ;

      ਅੱਖਰ *nm;
      ਅਚਲ ਅੱਖਰ *ixs;
      ਜੇ (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.ਮਾਪਾ));
	    s_vpi_ਮੁੱਲ vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_ਮੁੱਲ(rfp->id.ਸੂਚਕਾਂਕ, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } ਨਹੀਂਤਾਂ {
	    nm = strdup(rfp->id.name);
	    ixs = ਨਿਰਾ;
      }

	/* The scope information is added here for vpiFullName. */
      ਅੱਖਰ *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      ਮੁਕਤ(nm);
      ਵਾਪਸ rbuf;
}

ਸਥਿਰ vpiHandle signal_get_handle(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਚੋਣ (code) {

	  ਹਾਲਤ vpi_ਮਾਪਾ:
	    ਵਾਪਸ rfp->is_netarray? rfp->within.ਮਾਪਾ : ਨਿਰਾ;

	  ਹਾਲਤ vpi_ਸੂਚਕਾਂਕ_2:
	    ਵਾਪਸ rfp->is_netarray? rfp->id.ਸੂਚਕਾਂਕ : ਨਿਰਾ;

	  ਹਾਲਤ vpi_ਖੱਬਾ_Range:
	    ਵਾਪਸ &rfp->msb;
	  ਹਾਲਤ vpi_ਸੱਜਾ_Range:
	    ਵਾਪਸ &rfp->lsb;

	  ਹਾਲਤ vpiScope:
	    ਵਾਪਸ vpip_scope(rfp);

	  ਹਾਲਤ vpiModule:
	    ਵਾਪਸ vpip_module(vpip_scope(rfp));
      }

      ਵਾਪਸ 0;
}

ਸਥਿਰ vpiHandle signal_iterate(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਜੇ (code == vpi_ਸੂਚਕਾਂਕ_2) {
	    ਵਾਪਸ rfp->is_netarray ? rfp->id.ਸੂਚਕਾਂਕ->vpi_iterate(code) : ਨਿਰਾ;
      }

      ਵਾਪਸ 0;
}

ਖਾਲੀ __vpiSignal::make_ਬਿੱਟ()
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਬਿੱਟ == ਨਿਰਾ);
      ਬਿੱਟ = ਨਵਾਂ ਬਣਤਰ __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      ਬਿੱਟ->ਮਾਪਾ = ਇਹ;
	// Now point to word[0].
      ਬਿੱਟ += 1;

	// Update each bit to point to the base
      ਬਣਤਰ __vpiBit*ਅਧਾਰ = ਬਿੱਟ;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < width(); idx += 1) {
	    ਅਧਾਰ[idx].bit0 = ਅਧਾਰ;
	    ਪੂਰਨ real_idx;
	    ਜੇ (msb.get_ਮੁੱਲ() >= lsb.get_ਮੁੱਲ()) {
		  real_idx = idx + lsb.get_ਮੁੱਲ();
	    } ਨਹੀਂਤਾਂ {
		  real_idx = lsb.get_ਮੁੱਲ() - idx;
	    }
	    ਅਧਾਰ[idx].ਸੂਚਕਾਂਕ = ਨਵਾਂ __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_ਸੂਚਕਾਂਕ(ਪੂਰਨ idx)
{
	/* Check to see if the index is in range. */
      ਜੇ (msb.get_ਮੁੱਲ() >= lsb.get_ਮੁੱਲ()) {
	    ਜੇ ((idx > msb.get_ਮੁੱਲ()) || (idx < lsb.get_ਮੁੱਲ())) ਵਾਪਸ 0;
      } ਨਹੀਂਤਾਂ {
	    ਜੇ ((idx < msb.get_ਮੁੱਲ()) || (idx > lsb.get_ਮੁੱਲ())) ਵਾਪਸ 0;
      }

	/* Normalize the index */
      ਬਿਨਾਨਿਸ਼ਾਨੀ norm_idx;
      ਜੇ (msb.get_ਮੁੱਲ() >= lsb.get_ਮੁੱਲ()) {
	    norm_idx = idx - lsb.get_ਮੁੱਲ();
      } ਨਹੀਂਤਾਂ {
	    norm_idx = lsb.get_ਮੁੱਲ() - idx;
      }

      ਜੇ (ਬਿੱਟ == ਨਿਰਾ) make_ਬਿੱਟ();

      ਵਾਪਸ &(ਬਿੱਟ[norm_idx].as_ਦੋਆਧਾਰੀ_ਅੰਕ);
}

ਖਾਲੀ __vpiSignal::get_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮੁੱਲ(ਅਚਲ ਬਣਤਰ __vpiBit*ਦੋਆਧਾਰੀ_ਅੰਕ, p_vpi_ਮੁੱਲ vp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ = ਦੋਆਧਾਰੀ_ਅੰਕ->get_norm_ਸੂਚਕਾਂਕ();

      vvp_signal_ਮੁੱਲ*vsig = dynamic_cast<vvp_signal_ਮੁੱਲ*>(ਗੰਢ->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(vsig);

      ਜੇ (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      ਚੋਣ (vp->format) {
	ਹਾਲਤ vpiBinStrVal:
            format_vpiBinStrVal(vsig, ਸੂਚਕਾਂਕ, 1, vp);
	    ਰੋਕੋ;

	ਹਾਲਤ vpiOctStrVal:
            format_vpiOctStrVal(vsig, ਸੂਚਕਾਂਕ, 1, vp);
	    ਰੋਕੋ;

	ਹਾਲਤ vpiDecStrVal:
            format_vpiDecStrVal(vsig, ਸੂਚਕਾਂਕ, 1, ਝੂਠ, vp);
	    ਰੋਕੋ;

	ਹਾਲਤ vpiHexStrVal:
            format_vpiHexStrVal(vsig, ਸੂਚਕਾਂਕ, 1, vp);
	    ਰੋਕੋ;

	ਹਾਲਤ vpiStringVal:
            format_vpiStringVal(vsig, ਸੂਚਕਾਂਕ, 1, vp);
	    ਰੋਕੋ;

	ਹਾਲਤ vpiIntVal:
            format_vpiIntVal(vsig, ਸੂਚਕਾਂਕ, 1, ਝੂਠ, vp);
	    ਰੋਕੋ;

	ਹਾਲਤ vpiRealVal:
            format_vpiRealVal(vsig, ਸੂਚਕਾਂਕ, 1, ਝੂਠ, vp);
	    ਰੋਕੋ;

	ਹਾਲਤ vpiScalarVal:
            format_vpiScalarVal(vsig, ਸੂਚਕਾਂਕ, vp);
	    ਰੋਕੋ;

        ਹਾਲਤ vpiStrengthVal:
            format_vpiStrengthVal(vsig, ਸੂਚਕਾਂਕ, 1, vp);
            ਰੋਕੋ;

	ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:
            format_vpi_ਸਦਿਸ਼_Val(vsig, ਸੂਚਕਾਂਕ, 1, vp);
	    ਰੋਕੋ;

	ਮੂਲ:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (ਪੂਰਨ)vp->format, vpi_get_str(vpiName),
		    ਦੋਆਧਾਰੀ_ਅੰਕ->get_ਸੂਚਕਾਂਕ(),
	            vpip_scope(ਇਹ)->scope_name());
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      };
}

vpiHandle __vpiSignal::put_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮੁੱਲ(ਬਣਤਰ __vpiBit*ਦੋਆਧਾਰੀ_ਅੰਕ, p_vpi_ਮੁੱਲ vp, ਪੂਰਨ flags)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ = ਦੋਆਧਾਰੀ_ਅੰਕ->get_norm_ਸੂਚਕਾਂਕ();
      vvp_net_ptr_t dest(ਗੰਢ, 0);
      vvp_vector4_t val = vec4_from_vpi_ਮੁੱਲ(vp, 1);

      ਜੇ ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    ਦੋਆਧਾਰੀ_ਅੰਕ->as_ਦੋਆਧਾਰੀ_ਅੰਕ.vpi_get_str(vpi_ਭਰਿਆ_Name));
	    ਵਾਪਸ ਨਿਰਾ;
      }

      ਜੇ ((get_type_ਕੋਡ() == vpiNet) &&
          !dynamic_cast<vvp_island_ਪੋਰਟ*>(ਗੰਢ->fun)) {
	    ਗੰਢ->send_vec4_pv(val, ਸੂਚਕਾਂਕ, width(),
	                            vthread_get_wt_context());
      } ਨਹੀਂਤਾਂ {
	    vvp_send_vec4_pv(dest, val, ਸੂਚਕਾਂਕ, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ vpiHandle signal_ਸੂਚਕਾਂਕ(ਪੂਰਨ idx, vpiHandle ref)
{
      ਬਣਤਰ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      ਜੇ ((type != vpiNet) && (type != vpiReg)) ਵਾਪਸ 0;

      ਵਾਪਸ rfp->get_ਸੂਚਕਾਂਕ(idx);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ __vpiSignal::width(ਖਾਲੀ) ਅਚਲ
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = (msb.get_ਮੁੱਲ() >= lsb.get_ਮੁੱਲ())
	    ? (msb.get_ਮੁੱਲ() - lsb.get_ਮੁੱਲ() + 1)
	    : (lsb.get_ਮੁੱਲ() - msb.get_ਮੁੱਲ() + 1);

      ਵਾਪਸ wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
ਸਥਿਰ ਖਾਲੀ signal_get_ਮੁੱਲ(vpiHandle ref, s_vpi_ਮੁੱਲ*vp)
{
      ਬਣਤਰ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = rfp->width();

      vvp_signal_ਮੁੱਲ*vsig = dynamic_cast<vvp_signal_ਮੁੱਲ*>(rfp->ਗੰਢ->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(vsig);

      ਚੋਣ (vp->format) {

	  ਹਾਲਤ vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:
	    format_vpi_ਸਦਿਸ਼_Val(vsig, 0, wid, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiObjTypeVal:
	    ਜੇ (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } ਨਹੀਂਤਾਂ {
		  vp->format = vpi_ਸਦਿਸ਼_Val;
		  format_vpi_ਸਦਿਸ਼_Val(vsig, 0, wid, vp);
	    }
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (ਪੂਰਨ)vp->format, vpi_get_str(vpiName, ref),
		    vpip_scope(rfp)->scope_name());
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }
}

/*
 * The put_value method writes the value into the vector, and returns
 * the affected ref. This operation works much like the %set or
 * %assign instructions and causes all the side-effects that the
 * equivalent instruction would cause.
 */

ਸਥਿਰ vvp_vector4_t from_stringval(ਅਚਲ ਅੱਖਰ*str, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx;
      ਅਚਲ ਅੱਖਰ*cp;

      cp = str + ਲੜੀ_ਲੰਬਾਈ(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      ਜਦੋਂ ((idx < wid) && (cp > str)) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ byte = *--cp;
	    ਪੂਰਨ ਦੋਆਧਾਰੀ_ਅੰਕ;

	    ਲਈ (ਦੋਆਧਾਰੀ_ਅੰਕ = 0 ;  (ਦੋਆਧਾਰੀ_ਅੰਕ < 8) && (idx < wid) ;  ਦੋਆਧਾਰੀ_ਅੰਕ += 1) {
		  ਜੇ (byte & 1)
			val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      ਵਾਪਸ val;
}

ਸਥਿਰ vpiHandle signal_put_ਮੁੱਲ(vpiHandle ref, s_vpi_ਮੁੱਲ*vp, ਪੂਰਨ flags)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid;
      ਬਣਤਰ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);
      vvp_net_ptr_t dest(rfp->ਗੰਢ, 0);

      ਤਾਰਕਿਕ net_flag = ref->get_type_ਕੋਡ()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      ਜੇ (flags == vpiReleaseFlag) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(rfp->ਗੰਢ->fil);
	    rfp->ਗੰਢ->fil->force_unlink();
	    rfp->ਗੰਢ->fil->release(dest, net_flag);
	    rfp->ਗੰਢ->fun->force_flag(ਸੱਚ);
	    signal_get_ਮੁੱਲ(ref, vp);
	    ਵਾਪਸ ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_ਮੁੱਲ() >= rfp->lsb.get_ਮੁੱਲ())
	    ? (rfp->msb.get_ਮੁੱਲ() - rfp->lsb.get_ਮੁੱਲ() + 1)
	    : (rfp->lsb.get_ਮੁੱਲ() - rfp->msb.get_ਮੁੱਲ() + 1);

      vvp_vector4_t val = vec4_from_vpi_ਮੁੱਲ(vp, wid);

      ਜੇ (flags == vpiForceFlag) {
	    vvp_vector2_t ਨਕਾਬ (vvp_vector2_t::FILL1, wid);
	    rfp->ਗੰਢ->force_vec4(val, ਨਕਾਬ);
      } ਨਹੀਂਤਾਂ ਜੇ (net_flag && !dynamic_cast<vvp_island_ਪੋਰਟ*>(rfp->ਗੰਢ->fun)) {
	    rfp->ਗੰਢ->send_vec4(val, vthread_get_wt_context());
      } ਨਹੀਂਤਾਂ {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      ਵਾਪਸ ref;
}

vvp_vector4_t vec4_from_vpi_ਮੁੱਲ(s_vpi_ਮੁੱਲ*vp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      ਚੋਣ (vp->format) {

	  ਹਾਲਤ vpiIntVal: {
		ਲੰਮਾ vpi_val = vp->value.integer;
		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t ਦੋਆਧਾਰੀ_ਅੰਕ = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, ਦੋਆਧਾਰੀ_ਅੰਕ);
		      vpi_val >>= 1;
		}
		ਰੋਕੋ;
	  }

	  ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid ;  idx += 1) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ aval = vp->value.vector[idx/32].aval;
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  ਪੂਰਨ bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t ਦੋਆਧਾਰੀ_ਅੰਕ = scalar_ਮੰਜ਼ਿਲ_bit4(bitmask);
		  val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, ਦੋਆਧਾਰੀ_ਅੰਕ);
	    }
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiBinStrVal:
	    vpip_bin_str_ਮੰਜ਼ਿਲ_vec4(val, vp->value.str);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiOctStrVal:
	    vpip_oct_str_ਮੰਜ਼ਿਲ_vec4(val, vp->value.str);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiDecStrVal:
	    vpip_dec_str_ਮੰਜ਼ਿਲ_vec4(val, vp->value.str);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiHexStrVal:
	    vpip_hex_str_ਮੰਜ਼ਿਲ_vec4(val, vp->value.str);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiScalarVal:
	    val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, scalar_ਮੰਜ਼ਿਲ_bit4(vp->value.scalar));
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (ਪੂਰਨ)vp->format);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);

      }

      ਵਾਪਸ val;
}

ਪੂਰਨ __vpiSignal::vpi_get(ਪੂਰਨ code)
{ ਵਾਪਸ signal_get(code, ਇਹ); }

int64_t __vpiSignal::vpi_get64(ਪੂਰਨ code)
{ ਵਾਪਸ signal_get64(code, ਇਹ); }

ਅੱਖਰ* __vpiSignal::vpi_get_str(ਪੂਰਨ code)
{ ਵਾਪਸ signal_get_str(code, ਇਹ); }

ਖਾਲੀ __vpiSignal::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{ signal_get_ਮੁੱਲ(ਇਹ, val); }

vpiHandle __vpiSignal::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags)
{ ਵਾਪਸ signal_put_ਮੁੱਲ(ਇਹ, val, flags); }

vpiHandle __vpiSignal::vpi_handle(ਪੂਰਨ code)
{ ਵਾਪਸ signal_get_handle(code, ਇਹ); }

vpiHandle __vpiSignal::vpi_iterate(ਪੂਰਨ code)
{ ਵਾਪਸ signal_iterate(code, ਇਹ); }

vpiHandle __vpiSignal::vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ idx)
{ ਵਾਪਸ signal_ਸੂਚਕਾਂਕ(idx, ਇਹ); }

ਬਣਤਰ signal_reg : ਜਨਤਕ __vpiSignal {
      ਸਤਰਅੰਦਰ signal_reg() { }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiReg; }
};

ਬਣਤਰ signal_integer : ਜਨਤਕ __vpiSignal {
      ਸਤਰਅੰਦਰ signal_integer() { }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiIntegerVar; }
};

ਬਣਤਰ signal_net : ਜਨਤਕ __vpiSignal {
      ਸਤਰਅੰਦਰ signal_net() { }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiNet; }
};

ਬਣਤਰ signal_byte : ਜਨਤਕ __vpiSignal {
      ਸਤਰਅੰਦਰ signal_byte() { }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiByteVar; }
};

ਬਣਤਰ signal_bitvar : ਜਨਤਕ __vpiSignal {
      ਸਤਰਅੰਦਰ signal_bitvar() { }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpi_ਦੋਆਧਾਰੀ_ਅੰਕ_Var; }
};

ਬਣਤਰ signal_shortint : ਜਨਤਕ __vpiSignal {
      ਸਤਰਅੰਦਰ signal_shortint() { }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiShortIntVar; }
};

ਬਣਤਰ signal_int : ਜਨਤਕ __vpiSignal {
      ਸਤਰਅੰਦਰ signal_int() { }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiIntVar; }
};

ਬਣਤਰ signal_longint : ਜਨਤਕ __vpiSignal {
      ਸਤਰਅੰਦਰ signal_longint() { }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(ਅਚਲ ਅੱਖਰ*name, ਪੂਰਨ msb, ਪੂਰਨ lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = ਨਵਾਂ signal_integer;
      ਵਾਪਸ fill_ਵਿੱਚ_var4(obj, name, msb, lsb, ਸੱਚ, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(ਅਚਲ ਅੱਖਰ*name, ਪੂਰਨ msb, ਪੂਰਨ lsb, ਤਾਰਕਿਕ signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      ਜੇ ((! signed_flag) || (lsb != 0) ) {
	    obj = ਨਵਾਂ signal_bitvar;
      } ਨਹੀਂਤਾਂ {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    ਚੋਣ (msb) {
		ਹਾਲਤ 7:
		  obj = ਨਵਾਂ signal_byte;
		  ਰੋਕੋ;
		ਹਾਲਤ 15:
		  obj = ਨਵਾਂ signal_shortint;
		  ਰੋਕੋ;
		ਹਾਲਤ 31:
		  obj = ਨਵਾਂ signal_int;
		  ਰੋਕੋ;
		ਹਾਲਤ 63:
		  obj = ਨਵਾਂ signal_longint;
		  ਰੋਕੋ;
		ਮੂਲ:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = ਨਵਾਂ signal_bitvar;
		  ਰੋਕੋ;
	    }
      }

      ਵਾਪਸ fill_ਵਿੱਚ_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(ਅਚਲ ਅੱਖਰ*name, ਪੂਰਨ msb, ਪੂਰਨ lsb,
			ਤਾਰਕਿਕ signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = ਨਵਾਂ signal_reg;
      ਵਾਪਸ fill_ਵਿੱਚ_var4(obj, name, msb, lsb, signed_flag, vec);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਸਥਿਰ ਬਣਤਰ vpiSignal_plug **signal_pool = 0;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ signal_pool_ਗਿਣਤੀ = 0;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ signal_ਗਿਣਤੀ = 0;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ signal_dels = 0;
#ਜੇ_ਅੰਤ

ਬਣਤਰ vpiSignal_plug {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ space[ਆਕਾਰ (ਬਣਤਰ __vpiSignal)];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਬਣਤਰ vpiSignal_plug *pool;
#ਜੇ_ਅੰਤ
};

ਖਾਲੀ* __vpiSignal::ਸੰਚਾਲਕ ਨਵਾਂ(size_t siz)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਪ੍ਰਤਿਜ੍ਞਾ(siz == ਆਕਾਰ(ਬਣਤਰ vpiSignal_plug) - ਆਕਾਰ(ਬਣਤਰ vpiSignal_plug *));
#ਨਹੀਂਤਾਂ
      ਪ੍ਰਤਿਜ੍ਞਾ(siz == ਆਕਾਰ(ਬਣਤਰ vpiSignal_plug));
#ਜੇ_ਅੰਤ
      ਸਥਿਰ ਬਣਤਰ vpiSignal_plug*alloc_array = 0;
      ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ alloc_ਸੂਚਕਾਂਕ = 0;
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ alloc_ਗਿਣਤੀ = 512;

      ਜੇ ((alloc_array == 0) || (alloc_ਸੂਚਕਾਂਕ == alloc_ਗਿਣਤੀ)) {
	    alloc_array = static_cast<ਬਣਤਰ vpiSignal_plug*>
	                  (ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(alloc_ਗਿਣਤੀ, ਆਕਾਰ(ਬਣਤਰ vpiSignal_plug)));
	    alloc_ਸੂਚਕਾਂਕ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_ਗਿਣਤੀ *
	                                            ਆਕਾਰ(ਬਣਤਰ vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_ਗਿਣਤੀ += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_ਗਿਣਤੀ*ਆਕਾਰ(vpiSignal_plug **)));
	    signal_pool[signal_pool_ਗਿਣਤੀ-1] = alloc_array;
#ਜੇ_ਅੰਤ
      }

      ਬਣਤਰ vpiSignal_plug*cur = alloc_array + alloc_ਸੂਚਕਾਂਕ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, ਆਕਾਰ(ਬਣਤਰ vpiSignal_plug));
      cur->pool = alloc_array;
      signal_ਗਿਣਤੀ += 1;
#ਜੇ_ਅੰਤ
      alloc_ਸੂਚਕਾਂਕ += 1;
      ਵਾਪਸ cur;
}

ਖਾਲੀ __vpiSignal::ਸੰਚਾਲਕ ਮਿਟਾਓ(ਖਾਲੀ*)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ signal_delete(vpiHandle ਵਸਤੂ)
{
      ਬਣਤਰ __vpiSignal *obj = static_cast<__vpiSignal *> (ਵਸਤੂ);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj->ਗੰਢ->fil);
      obj->ਗੰਢ->fil->clear_all_callbacks();
      vvp_net_delete(obj->ਗੰਢ);
      ਜੇ (obj->ਬਿੱਟ) {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx=0; idx<obj->width(); idx+=1) {
		  ਮਿਟਾਓ obj->ਬਿੱਟ[idx].ਸੂਚਕਾਂਕ;
	    }
	    obj->ਬਿੱਟ -= 1;
	    ਮਿਟਾਓ [] obj->ਬਿੱਟ;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

ਖਾਲੀ signal_pool_delete()
{
      ਜੇ (RUNNING_ON_VALGRIND && (signal_ਗਿਣਤੀ != signal_dels)) {
	    fflush(ਨਿਰਾ);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (ਲੰਮਾ) signal_ਗਿਣਤੀ - signal_dels, signal_ਗਿਣਤੀ);
      }

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < signal_pool_ਗਿਣਤੀ; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    ਮੁਕਤ(signal_pool[idx]);
      }

      ਮੁਕਤ(signal_pool);
      signal_pool = 0;
      signal_pool_ਗਿਣਤੀ = 0;
}
#ਜੇ_ਅੰਤ

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
ਸਥਿਰ vpiHandle fill_ਵਿੱਚ_net4(ਬਣਤਰ __vpiSignal*obj, __vpiScope*scope,
			      ਅਚਲ ਅੱਖਰ*name, ਪੂਰਨ msb, ਪੂਰਨ lsb,
			      ਤਾਰਕਿਕ signed_flag, vvp_net_t*ਗੰਢ)
{
      obj->id.name = name? vpip_name_string(name) : ਨਿਰਾ;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->ਗੰਢ = ਗੰਢ;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      ਗਿਣਤੀ_vpi_nets += 1;

      ਵਾਪਸ obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 ਅਚਲ ਅੱਖਰ*name, ਪੂਰਨ msb, ਪੂਰਨ lsb,
			 ਤਾਰਕਿਕ signed_flag, vvp_net_t*ਗੰਢ)
{
      ਬਣਤਰ __vpiSignal*obj = ਨਵਾਂ signal_net;
      ਵਾਪਸ fill_ਵਿੱਚ_net4(obj, scope, name, msb, lsb, signed_flag, ਗੰਢ);
}

ਸਥਿਰ ਪੂਰਨ PV_get_ਅਧਾਰ(ਬਣਤਰ __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      ਜੇ (rfp->sbase != 0) {
	    s_vpi_ਮੁੱਲ val;
	      /* Check to see if the value is defined. */
	    val.format = vpi_ਸਦਿਸ਼_Val;
	    vpi_get_ਮੁੱਲ(rfp->sbase, &val);
	    ਪੂਰਨ ਸ਼ਬਦ = (vpi_get(vpi_ਮਾਪ, rfp->sbase)-1)/32 + 1;
	    ਲਈ(ਪੂਰਨ idx = 0; idx < ਸ਼ਬਦ; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  ਜੇ (val.value.vector[idx].bval != 0) ਵਾਪਸ INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_ਮੁੱਲ(rfp->sbase, &val);
	    ਵਾਪਸ val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      ਵਾਪਸ rfp->tbase;
}

ਸਥਿਰ ਪੂਰਨ PV_get(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਪੂਰਨ rval = 0;
      ਚੋਣ (code) {
	ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ 0;  // Not implemented for now!

	ਹਾਲਤ vpiSigned:
	    ਵਾਪਸ 0;  // A part/bit select is always unsigned!

	ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ rfp->width;

	  /* This is like the &A<> in array.cc. */
	ਹਾਲਤ vpiConstantSelect:
	    ਵਾਪਸ rfp->sbase == 0;

	ਹਾਲਤ vpi_ਖੱਬਾ_Range:
            rval += rfp->width - 1;
	    // fallthrough
	ਹਾਲਤ vpi_ਸੱਜਾ_Range: {
	    ਪੂਰਨ ਖੱਬੇ = vpi_get(vpi_ਖੱਬਾ_Range, rfp->ਮਾਪਾ);
	    ਪੂਰਨ ਸੱਜੇ = vpi_get(vpi_ਸੱਜਾ_Range, rfp->ਮਾਪਾ);
	    rval += PV_get_ਅਧਾਰ(rfp);
	    ਵਾਪਸ ਖੱਬੇ >= ਸੱਜੇ ? ਸੱਜੇ + rval : ਸੱਜੇ - rval;
	}

        ਹਾਲਤ vpiAutomatic:
            ਵਾਪਸ vpi_get(vpiAutomatic, rfp->ਮਾਪਾ);

#ਜੇ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        ਹਾਲਤ _vpiFromThr:
            ਵਾਪਸ _vpi_at_PV;
#ਜੇ_ਅੰਤ

	ਮੂਲ:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      ਵਾਪਸ 0;
}

ਸਥਿਰ ਅੱਖਰ* PV_get_str(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਚੋਣ (code) {
	ਹਾਲਤ vpiFile:  // Not implemented for now!
	    ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[0]);

	ਹਾਲਤ vpiName:
	ਹਾਲਤ vpi_ਭਰਿਆ_Name: {
	    ਅਚਲ ਅੱਖਰ*nm = vpi_get_str(code, rfp->ਮਾਪਾ);
	    size_t len = 256+ਲੜੀ_ਲੰਬਾਈ(nm);
	    ਅੱਖਰ *ਭਰਿਆ = static_cast<ਅੱਖਰ *>(ਸਮ੍ਰਿਤੀ_ਦਿਓ(len));
	    snprintf(ਭਰਿਆ, len, "%s[%d:%d]", nm,
	                                     (ਪੂਰਨ)vpi_get(vpi_ਖੱਬਾ_Range, ref),
	                                     (ਪੂਰਨ)vpi_get(vpi_ਸੱਜਾ_Range, ref));
	    ਭਰਿਆ[len-1] = 0;
	    ਅੱਖਰ *res = simple_ਸੈੱਟ_rbuf_str(ਭਰਿਆ);
	    ਮੁਕਤ(ਭਰਿਆ);
	    ਵਾਪਸ res;
	}

	ਮੂਲ:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ PV_get_ਮੁੱਲ(vpiHandle ref, p_vpi_ਮੁੱਲ vp)
{
      ਬਣਤਰ __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*>(rfp->net->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);

      ਚੋਣ (vp->format) {

	  ਹਾਲਤ vpiIntVal:
	    format_vpiIntVal(sig, PV_get_ਅਧਾਰ(rfp), rfp->width, 0, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_ਅਧਾਰ(rfp), rfp->width, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_ਅਧਾਰ(rfp), rfp->width, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_ਅਧਾਰ(rfp), rfp->width, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_ਅਧਾਰ(rfp), rfp->width, 0, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiStringVal:
	    format_vpiStringVal(sig, PV_get_ਅਧਾਰ(rfp), rfp->width, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_ਅਧਾਰ(rfp), vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_ਅਧਾਰ(rfp), rfp->width, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:
	    format_vpi_ਸਦਿਸ਼_Val(sig, PV_get_ਅਧਾਰ(rfp), rfp->width, vp);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiRealVal:
	    format_vpiRealVal(sig, PV_get_ਅਧਾਰ(rfp), rfp->width, 0, vp);
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (ਪੂਰਨ)vp->format, vpi_get_str(vpi_ਭਰਿਆ_Name, rfp->ਮਾਪਾ));
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }
}

ਸਥਿਰ vpiHandle PV_put_ਮੁੱਲ(vpiHandle ref, p_vpi_ਮੁੱਲ vp, ਪੂਰਨ flags)
{
      ਬਣਤਰ __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);
      vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*>(rfp->net->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);

      ਬਿਨਾਨਿਸ਼ਾਨੀ sig_ਮਾਪ = sig->ਮੁੱਲ_ਮਾਪ();
      ਬਿਨਾਨਿਸ਼ਾਨੀ width = rfp->width;
      ਪੂਰਨ ਅਧਾਰ = PV_get_ਅਧਾਰ(rfp);
      ਜੇ (ਅਧਾਰ >= (ਨਿਸ਼ਾਨੀਵਾਲਾ) sig_ਮਾਪ) ਵਾਪਸ 0;
      ਜੇ (ਅਧਾਰ + (ਨਿਸ਼ਾਨੀਵਾਲਾ) width < 0) ਵਾਪਸ 0;

      vvp_vector4_t val;
      ਜੇ (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_ਮੁੱਲ(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      ਜੇ (ਅਧਾਰ < 0) {
	    width += ਅਧਾਰ;
	    ਜੇ (flags != vpiReleaseFlag) {
		  val = val.subvalue(-ਅਧਾਰ, width);
	    }
	    ਅਧਾਰ = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      ਜੇ (ਅਧਾਰ+width > sig_ਮਾਪ) {
	    width = sig_ਮਾਪ - ਅਧਾਰ;
	    ਜੇ (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(rfp->ਮਾਪਾ);
      ਤਾਰਕਿਕ net_flag = rfp->ਮਾਪਾ->get_type_ਕੋਡ()==vpiNet;
      ਤਾਰਕਿਕ ਭਰਿਆ_sig = ਅਧਾਰ == 0 && width == sig_ਮਾਪ;

      vvp_net_ptr_t dest(rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value.  Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value.*/
      ਜੇ (flags == vpiReleaseFlag) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    ਜੇ (ਭਰਿਆ_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } ਨਹੀਂਤਾਂ {
		  rfp->net->fil->release_pv(dest, ਅਧਾਰ, width, net_flag);
	    }
	    rfp->net->fun->force_flag(ਸੱਚ);
	    PV_get_ਮੁੱਲ(ref, vp);
	    ਵਾਪਸ ref;
      }

      ਜੇ (flags == vpiForceFlag) {
	    ਜੇ (ਭਰਿਆ_sig) {
		  vvp_vector2_t ਨਕਾਬ (vvp_vector2_t::FILL1, sig_ਮਾਪ);
		  rfp->net->force_vec4(val, ਨਕਾਬ);
	    } ਨਹੀਂਤਾਂ {
		  vvp_vector2_t ਨਕਾਬ (vvp_vector2_t::FILL0, sig_ਮਾਪ);
		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < width ; idx += 1)
		        ਨਕਾਬ.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਧਾਰ+idx, 1);

		  vvp_vector4_t tmp (sig_ਮਾਪ, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_ਮੁੱਲ(tmp);

		  tmp.ਸੈੱਟ_vec(ਅਧਾਰ, val);
		  rfp->net->force_vec4(tmp, ਨਕਾਬ);
	    }
      } ਨਹੀਂਤਾਂ ਜੇ (net_flag && !dynamic_cast<vvp_island_ਪੋਰਟ*>(rfp->net->fun)) {
	    ਜੇ (ਭਰਿਆ_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } ਨਹੀਂਤਾਂ {
		  rfp->net->send_vec4_pv(val, ਅਧਾਰ, sig_ਮਾਪ,
		                         vthread_get_wt_context());
	    }
      } ਨਹੀਂਤਾਂ {
	    ਜੇ (ਭਰਿਆ_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } ਨਹੀਂਤਾਂ {
		  vvp_send_vec4_pv(dest, val, ਅਧਾਰ, sig_ਮਾਪ,
	                           vthread_get_wt_context());
	    }
      }
      ਵਾਪਸ 0;
}

ਸਥਿਰ vpiHandle PV_get_handle(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਮਾਪਾ:
	    ਵਾਪਸ rfp->ਮਾਪਾ;

	  ਹਾਲਤ vpiScope:
	    ਵਾਪਸ vpi_handle(vpiScope, rfp->ਮਾਪਾ);

	  ਹਾਲਤ vpiModule:
	    ਵਾਪਸ vpi_handle(vpiModule, rfp->ਮਾਪਾ);
      }

      ਵਾਪਸ 0;
}

ਸਤਰਅੰਦਰ __vpiPV::__vpiPV()
{ }

ਪੂਰਨ __vpiPV::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiPartSelect; }

ਪੂਰਨ __vpiPV::vpi_get(ਪੂਰਨ code)
{ ਵਾਪਸ PV_get(code, ਇਹ); }

ਅੱਖਰ* __vpiPV::vpi_get_str(ਪੂਰਨ code)
{ ਵਾਪਸ PV_get_str(code, ਇਹ); }

ਖਾਲੀ __vpiPV::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{ PV_get_ਮੁੱਲ(ਇਹ, val); }

vpiHandle __vpiPV::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags)
{ ਵਾਪਸ PV_put_ਮੁੱਲ(ਇਹ, val, flags); }

vpiHandle __vpiPV::vpi_handle(ਪੂਰਨ code)
{ ਵਾਪਸ PV_get_handle(code, ਇਹ); }

vpiHandle vpip_make_PV(ਅੱਖਰ*var, ਪੂਰਨ ਅਧਾਰ, ਪੂਰਨ width)
{
      ਬਣਤਰ __vpiPV*obj = ਨਵਾਂ __vpiPV;
      compile_vpi_lookup(&obj->ਮਾਪਾ, strdup(var));
      obj->sbase = 0;
      obj->tbase = ਅਧਾਰ;
      obj->width = (ਬਿਨਾਨਿਸ਼ਾਨੀ) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ਵਾਪਸ obj;
}

vpiHandle vpip_make_PV(ਅੱਖਰ*var, ਅੱਖਰ*ਚਿੰਨ੍ਹ, ਪੂਰਨ width)
{
      ਬਣਤਰ __vpiPV*obj = ਨਵਾਂ __vpiPV;
      compile_vpi_lookup(&obj->ਮਾਪਾ, strdup(var));
      compile_vpi_lookup(&obj->sbase, ਚਿੰਨ੍ਹ);
      obj->tbase = 0;
      obj->width = (ਬਿਨਾਨਿਸ਼ਾਨੀ) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ਵਾਪਸ obj;
}

vpiHandle vpip_make_PV(ਅੱਖਰ*var, vpiHandle handle, ਪੂਰਨ width)
{
      ਬਣਤਰ __vpiPV*obj = ਨਵਾਂ __vpiPV;
      compile_vpi_lookup(&obj->ਮਾਪਾ, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (ਬਿਨਾਨਿਸ਼ਾਨੀ) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ਵਾਪਸ obj;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ PV_delete(vpiHandle ਵਸਤੂ)
{
      ਬਣਤਰ __vpiPV *obj = dynamic_cast<__vpiPV*>(ਵਸਤੂ);
      ਜੇ (obj->sbase) {
	    ਚੋਣ (obj->sbase->get_type_ਕੋਡ()) {
		ਹਾਲਤ vpi_ਸਮ੍ਰਿਤੀ_ਸ਼ਬਦ:
		  ਜੇ (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  ਰੋਕੋ;
		ਹਾਲਤ vpiPartSelect:
		  ਪ੍ਰਤਿਜ੍ਞਾ(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  ਰੋਕੋ;
	    }
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      ਮਿਟਾਓ obj;
}
#ਜੇ_ਅੰਤ
