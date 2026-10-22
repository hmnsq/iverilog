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
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <cmath>
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <climits>
# include  <cstring>
# include  <cassert>
#ifdef CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
#endif
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
ബാഹ്യ സ്ഥിരം അക്ഷരം hex_digits[256];
ബാഹ്യ സ്ഥിരം അക്ഷരം oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
ശൂന്യം *need_result_buf(unsigned cnt, vpi_rbuf_t type)
{
      സ്ഥിതം ശൂന്യം*result_buf[2] = {0, 0};
      സ്ഥിതം size_t result_buf_size[2] = {0, 0};

      എങ്കിൽ (type == RBUF_DEL) {
	    സ്വതന്ത്രം(result_buf[RBUF_VAL]);
	    result_buf[RBUF_VAL] = 0;
	    result_buf_size[RBUF_VAL] = 0;

	    സ്വതന്ത്രം(result_buf[RBUF_STR]);
	    result_buf[RBUF_STR] = 0;
	    result_buf_size[RBUF_STR] = 0;

	    തിരികെ 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      എങ്കിൽ (result_buf_size[type] == 0) {
	    result_buf[type] = malloc(cnt);
	    result_buf_size[type] = cnt;
      } അല്ലെങ്കിൽ എങ്കിൽ (result_buf_size[type] < cnt) {
	    result_buf[type] = realloc(result_buf[type], cnt);
	    result_buf_size[type] = cnt;
      }

      തിരികെ result_buf[type];
}

അക്ഷരം *simple_set_rbuf_str(സ്ഥിരം അക്ഷരം *s1)
{
      അക്ഷരം *res = static_cast<അക്ഷരം *>(need_result_buf(strlen(s1)+1, RBUF_STR));
      എങ്കിൽ (res) strcpy(res,s1);
      തിരികെ res;
}

അക്ഷരം *generic_get_str(പൂർണ്ണസംഖ്യ code, vpiHandle ref, സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *index)
{
      size_t len = strlen(name) + 1;  /* include space for null termination */
      അക്ഷരം *bn = ശൂന്യ;
      തർക്കം is_bn_a_pkg = അസത്യം;
      എങ്കിൽ (code == vpiFullName) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = strlen(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    എങ്കിൽ (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      എങ്കിൽ (index != ശൂന്യ) len += strlen(index) + 2;  /* include space for brackets */

      അക്ഷരം *res = static_cast<അക്ഷരം *>(need_result_buf(len, RBUF_STR));
      എങ്കിൽ (!res) {
	    സ്വതന്ത്രം(bn);
	    തിരികെ ശൂന്യ;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      എങ്കിൽ (bn != ശൂന്യ) {
	    strcat(res, bn);
	      // A package already has the "::" separator in the name
	    എങ്കിൽ (! is_bn_a_pkg) strcat(res, ".");
	    സ്വതന്ത്രം(bn);
      }
      strcat(res, name);
      എങ്കിൽ (index != ശൂന്യ) {
	    strcat(res, "[");
	    strcat(res, index);
	    strcat(res, "]");
      }
      തിരികെ res;
}

സ്ഥിതം vpiHandle fill_in_net4(ഘടന __vpiSignal*obj, __vpiScope*scope,
                              സ്ഥിരം അക്ഷരം*name, പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb,
                              തർക്കം signed_flag, vvp_net_t*കണ്ണി);

സ്ഥിതം vpiHandle fill_in_var4(ഘടന __vpiSignal*obj,
                              സ്ഥിരം അക്ഷരം*name, പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb,
                              തർക്കം signed_flag, vvp_net_t*കണ്ണി)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      തിരികെ fill_in_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, കണ്ണി);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

സ്ഥിതം ശൂന്യം format_vpiBinStrVal(സ്ഥിരം vvp_signal_മൂല്യം*sig, പൂർണ്ണസംഖ്യ base, unsigned wid,
                                s_vpi_മൂല്യം*vp)
{
      അക്ഷരം *rbuf = static_cast<അക്ഷരം *>(need_result_buf(wid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long offset = end - 1;
      long ssize = (signed)sig->മൂല്യം_size();

      വേണ്ടി (long idx = base ;  idx < end ;  idx += 1) {
	    എങ്കിൽ (idx < 0 || idx >= ssize) {
                  rbuf[offset-idx] = 'x';
	    } അല്ലെങ്കിൽ {
                  rbuf[offset-idx] = vvp_bit4_to_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

സ്ഥിതം ശൂന്യം format_vpiOctStrVal(സ്ഥിരം vvp_signal_മൂല്യം*sig, പൂർണ്ണസംഖ്യ base, unsigned wid,
                                s_vpi_മൂല്യം*vp)
{
      unsigned dwid = (wid + 2) / 3;
      അക്ഷരം *rbuf = static_cast<അക്ഷരം *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->മൂല്യം_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      വേണ്ടി (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    എങ്കിൽ (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } അല്ലെങ്കിൽ {
                  തിരഞ്ഞെടുപ്പ് (sig->value(idx)) {
		      അവസ്ഥ BIT4_0:
			bit = 0;
			നിർത്തുക;
		      അവസ്ഥ BIT4_1:
			bit = 1;
			നിർത്തുക;
		      അവസ്ഥ BIT4_X:
			bit = 2;
			നിർത്തുക;
		      അവസ്ഥ BIT4_Z:
			bit = 3;
			നിർത്തുക;
                  }
	    }
	    val |= bit << 2*((idx-base) % 3);

	    എങ്കിൽ ((idx-base) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      തിരഞ്ഞെടുപ്പ് (wid % 3) {
	  അവസ്ഥ 1:
	    എങ്കിൽ (val == 2) val = 42;
	    അല്ലെങ്കിൽ എങ്കിൽ (val == 3) val = 63;
	    നിർത്തുക;
	  അവസ്ഥ 2:
	    എങ്കിൽ (val == 10) val = 42;
	    അല്ലെങ്കിൽ എങ്കിൽ (val == 15) val = 63;
	    നിർത്തുക;
      }

      എങ്കിൽ (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

സ്ഥിതം ശൂന്യം format_vpiHexStrVal(സ്ഥിരം vvp_signal_മൂല്യം*sig, പൂർണ്ണസംഖ്യ base, unsigned wid,
                                s_vpi_മൂല്യം*vp)
{
      unsigned dwid = (wid + 3) / 4;
      അക്ഷരം *rbuf = static_cast<അക്ഷരം *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->മൂല്യം_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      വേണ്ടി (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    എങ്കിൽ (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } അല്ലെങ്കിൽ {
                  തിരഞ്ഞെടുപ്പ് (sig->value(idx)) {
		      അവസ്ഥ BIT4_0:
			bit = 0;
			നിർത്തുക;
		      അവസ്ഥ BIT4_1:
			bit = 1;
			നിർത്തുക;
		      അവസ്ഥ BIT4_X:
			bit = 2;
			നിർത്തുക;
		      അവസ്ഥ BIT4_Z:
			bit = 3;
			നിർത്തുക;
                  }
	    }
	    val |= bit << 2*((idx-base) % 4);

	    എങ്കിൽ ((idx-base) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      തിരഞ്ഞെടുപ്പ് (wid % 4) {
	  അവസ്ഥ 1:
	    എങ്കിൽ (val == 2) val = 170;
	    അല്ലെങ്കിൽ എങ്കിൽ (val == 3) val = 255;
	    നിർത്തുക;
	  അവസ്ഥ 2:
	    എങ്കിൽ (val == 10) val = 170;
	    അല്ലെങ്കിൽ എങ്കിൽ (val == 15) val = 255;
	    നിർത്തുക;
	  അവസ്ഥ 3:
	    എങ്കിൽ (val == 42) val = 170;
	    അല്ലെങ്കിൽ എങ്കിൽ (val == 63) val = 255;
	    നിർത്തുക;
      }

      എങ്കിൽ (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

സ്ഥിതം ശൂന്യം format_vpiDecStrVal(സ്ഥിരം vvp_signal_മൂല്യം*sig, പൂർണ്ണസംഖ്യ base, unsigned wid,
                                പൂർണ്ണസംഖ്യ signed_flag, s_vpi_മൂല്യം*vp)
{
      unsigned hwid = (sig->മൂല്യം_size()+2) / 3 + 1;
      അക്ഷരം *rbuf = static_cast<അക്ഷരം *>(need_result_buf(hwid, RBUF_VAL));
      long ssize = (signed)sig->മൂല്യം_size();
      long end = base + (signed)wid;

	/* Do we have an end outside of the real signal vector. */
      എങ്കിൽ (base < 0 || end > ssize) {
	    തർക്കം all_x = സത്യം;
	    എങ്കിൽ (end > ssize) end = ssize;
	    എങ്കിൽ (base < 0) base = 0;
	    വേണ്ടി (long idx = base ;  idx < end ;  idx += 1) {
		  എങ്കിൽ (sig->value(idx) != BIT4_X) {
			all_x = അസത്യം;
			നിർത്തുക;
		  }
	    }

	    എങ്കിൽ (all_x) {
		  rbuf[0] = 'x';
	    } അല്ലെങ്കിൽ {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    തിരികെ;
      }

      vvp_vector4_t vec4;
      എങ്കിൽ (base == 0 && end == ssize) {
	    sig->vec4_മൂല്യം(vec4);
      } അല്ലെങ്കിൽ {
	    vvp_vector4_t tmp;
	    sig->vec4_മൂല്യം(tmp);
	    vec4 = tmp.subvalue(base, wid);
      }

      vpip_vec4_to_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

സ്ഥിതം ശൂന്യം format_vpiIntVal(സ്ഥിരം vvp_signal_മൂല്യം*sig, പൂർണ്ണസംഖ്യ base, unsigned wid,
                             പൂർണ്ണസംഖ്യ signed_flag, s_vpi_മൂല്യം*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_മൂല്യം(tmp);
      vvp_vector4_t sub = tmp.subvalue(base, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      എങ്കിൽ (വലുപ്പം(vp->value.integer) == വലുപ്പം(int32_t)) {
	    int32_t val = 0;
	    vector4_to_മൂല്യം(sub, val, signed_flag, അസത്യം);
	    vp->value.integer = val;
      } അല്ലെങ്കിൽ {
	    പ്രതിജ്ഞാ(വലുപ്പം(vp->value.integer) == വലുപ്പം(int64_t));
	    int64_t val = 0;
	    vector4_to_മൂല്യം(sub, val, signed_flag, അസത്യം);
	    vp->value.integer = val;
      }
}

സ്ഥിതം ശൂന്യം format_vpiRealVal(സ്ഥിരം vvp_signal_മൂല്യം*sig, പൂർണ്ണസംഖ്യ base, unsigned wid,
                              പൂർണ്ണസംഖ്യ signed_flag, s_vpi_മൂല്യം*vp)
{
      vvp_vector4_t vec4(wid);
      long ssize = (signed)sig->മൂല്യം_size();
      long end = base + (signed)wid;
      എങ്കിൽ (end > ssize) end = ssize;

      വേണ്ടി (long idx = (base < 0) ? 0 : base ;  idx < end ;  idx += 1) {
	    vec4.set_bit(idx-base, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_to_മൂല്യം(vec4, vp->value.real, signed_flag);
}

സ്ഥിതം ശൂന്യം format_vpiStringVal(സ്ഥിരം vvp_signal_മൂല്യം*sig, പൂർണ്ണസംഖ്യ base, unsigned wid,
                                s_vpi_മൂല്യം*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      അക്ഷരം *rbuf = static_cast<അക്ഷരം *>(need_result_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      അക്ഷരം *cp = rbuf;

      അക്ഷരം tmp = 0;
      വേണ്ടി (long idx = base+(signed)wid-1; idx >= base; idx -= 1) {
	    tmp <<= 1;

	    എങ്കിൽ (idx >=0 && idx < (signed)sig->മൂല്യം_size() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    എങ്കിൽ (((idx-base)&7)==0){
		  /* Skip leading nulls. */
		  എങ്കിൽ (tmp == 0 && cp == rbuf)
			തുടരുക;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

സ്ഥിതം ശൂന്യം format_vpiScalarVal(സ്ഥിരം vvp_signal_മൂല്യം*sig, പൂർണ്ണസംഖ്യ base,
                                s_vpi_മൂല്യം*vp)
{
      എങ്കിൽ (base >= 0 && base < (signed)sig->മൂല്യം_size()) {
	    തിരഞ്ഞെടുപ്പ് (sig->value(base)) {
		അവസ്ഥ BIT4_0:
		  vp->value.scalar = vpi0;
		  നിർത്തുക;
		അവസ്ഥ BIT4_1:
		  vp->value.scalar = vpi1;
		  നിർത്തുക;
		അവസ്ഥ BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_മൂല്യം(base);
		  എങ്കിൽ (strn.strength0() == 1) vp->value.scalar = vpiH;
		  അല്ലെങ്കിൽ എങ്കിൽ (strn.strength1() == 1) vp->value.scalar = vpiL;
		  അല്ലെങ്കിൽ vp->value.scalar = vpiX;
		  നിർത്തുക;
		}
		അവസ്ഥ BIT4_Z:
		  vp->value.scalar = vpiZ;
		  നിർത്തുക;
	    }
      } അല്ലെങ്കിൽ {
	    vp->value.scalar = vpiX;
      }
}

സ്ഥിതം ശൂന്യം format_vpiStrengthVal(സ്ഥിരം vvp_signal_മൂല്യം*sig, പൂർണ്ണസംഖ്യ base,
                                  unsigned wid, s_vpi_മൂല്യം*vp)
{
      long end = base + (signed)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_result_buf(wid * വലുപ്പം(s_vpi_strengthval), RBUF_VAL));

      വേണ്ടി (long idx = base ;  idx < end ;  idx += 1) {
	    എങ്കിൽ (idx >=0 && idx < (signed)sig->മൂല്യം_size()) {
		  vvp_scalar_t val = sig->scalar_മൂല്യം(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  unsigned s0 = 1 << val.strength0();
		  unsigned s1 = 1 << val.strength1();

		  തിരഞ്ഞെടുപ്പ് (val.value()) {
		      അവസ്ഥ BIT4_0:
			op[idx-base].logic = vpi0;
			op[idx-base].s0 = s0|s1;
			op[idx-base].s1 = 0;
			നിർത്തുക;

		      അവസ്ഥ BIT4_1:
			op[idx-base].logic = vpi1;
			op[idx-base].s0 = 0;
			op[idx-base].s1 = s0|s1;
			നിർത്തുക;

		      അവസ്ഥ BIT4_X:
			op[idx-base].logic = vpiX;
			op[idx-base].s0 = s0;
			op[idx-base].s1 = s1;
			നിർത്തുക;

		      അവസ്ഥ BIT4_Z:
			op[idx-base].logic = vpiZ;
			op[idx-base].s0 = vpiHiZ;
			op[idx-base].s1 = vpiHiZ;
			നിർത്തുക;
		  }
	    } അല്ലെങ്കിൽ {
		  op[idx-base].logic = vpiX;
		  op[idx-base].s0 = vpiStrongDrive;
		  op[idx-base].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

സ്ഥിതം ശൂന്യം format_vpiVectorVal(സ്ഥിരം vvp_signal_മൂല്യം*sig, പൂർണ്ണസംഖ്യ base, unsigned wid,
                                s_vpi_മൂല്യം*vp)
{
      long end = base + (signed)wid;
      unsigned പൂർണ്ണസംഖ്യ obit = 0;
      unsigned hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_result_buf(hwid * വലുപ്പം(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      വേണ്ടി (long idx = base ;  idx < end ;  idx += 1) {
	    എങ്കിൽ (base >= 0 && base < (signed)sig->മൂല്യം_size()) {
		തിരഞ്ഞെടുപ്പ് (sig->value(idx)) {
		അവസ്ഥ BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  നിർത്തുക;
		അവസ്ഥ BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  നിർത്തുക;
		അവസ്ഥ BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  നിർത്തുക;
		അവസ്ഥ BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  നിർത്തുക;
		}
	    } അല്ലെങ്കിൽ {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    എങ്കിൽ (!(obit % 32)) {
		  op += 1;
		  എങ്കിൽ ((op - vp->value.vector) < (ptrdiff_t)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ signal_get(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      പ്രതിജ്ഞാ(rfp);

      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpi_വരി_No:
	    തിരികെ 0;  // Not implemented for now!

	  അവസ്ഥ vpiSigned:
	    തിരികെ rfp->signed_flag != 0;

	  അവസ്ഥ vpiArray:
	    തിരികെ rfp->is_netarray != 0;

	  അവസ്ഥ vpiIndex: // This only works while we have a single index.
	    എങ്കിൽ (rfp->is_netarray) {
		  s_vpi_മൂല്യം vp;
		  vp.format = vpiIntVal;
		  vpi_get_മൂല്യം(rfp->id.index, &vp);
		  തിരികെ vp.value.integer;
	    } അല്ലെങ്കിൽ {
		  തിരികെ vpiUndefined;
	    }

	  അവസ്ഥ vpiSize:
	    തിരികെ rfp->width();

	  അവസ്ഥ vpiNetType:
	    എങ്കിൽ (ref->get_type_code()==vpiNet)
		  തിരികെ vpiWire;
	    അല്ലെങ്കിൽ
		  തിരികെ vpiUndefined;

	  അവസ്ഥ vpi_ഇടത്_Range:
            തിരികെ rfp->msb.get_മൂല്യം();

	  അവസ്ഥ vpi_വലത്_Range:
            തിരികെ rfp->lsb.get_മൂല്യം();

	  അവസ്ഥ vpiScalar:
	    തിരികെ (rfp->msb.get_മൂല്യം() == 0 && rfp->lsb.get_മൂല്യം() == 0);
	  അവസ്ഥ vpiVector:
	    തിരികെ (rfp->msb.get_മൂല്യം() != rfp->lsb.get_മൂല്യം());

          അവസ്ഥ vpiAutomatic:
            തിരികെ vpip_scope(rfp)->is_automatic() ? 1  : 0;

#ifdef BR916_STOPGAP_FIX
          അവസ്ഥ _vpiFromThr:
            തിരികെ _vpiNoThr;
#endif

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    തിരികെ vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
സ്ഥിതം int64_t signal_get64(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      പ്രതിജ്ഞാ(rfp);

      തിരഞ്ഞെടുപ്പ് (code) {
	    // This private property must return zero when undefined.
	  അവസ്ഥ _vpiNexusId:
	    എങ്കിൽ (rfp->msb.get_മൂല്യം() == rfp->lsb.get_മൂല്യം())
		  തിരികെ reinterpret_cast<int64_t>(rfp->കണ്ണി);
	    തിരികെ 0;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    തിരികെ vpiUndefined;
      }
}

സ്ഥിതം അക്ഷരം* signal_get_str(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      പ്രതിജ്ഞാ(rfp);

      എങ്കിൽ (code == vpiFile) {  // Not implemented for now!
	    തിരികെ simple_set_rbuf_str(file_names[0]);
      }

      എങ്കിൽ ((code != vpiName) && (code != vpiFullName)) തിരികെ ശൂന്യ;

      അക്ഷരം *nm;
      സ്ഥിരം അക്ഷരം *ixs;
      എങ്കിൽ (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_മൂല്യം vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_മൂല്യം(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } അല്ലെങ്കിൽ {
	    nm = strdup(rfp->id.name);
	    ixs = ശൂന്യ;
      }

	/* The scope information is added here for vpiFullName. */
      അക്ഷരം *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      സ്വതന്ത്രം(nm);
      തിരികെ rbuf;
}

സ്ഥിതം vpiHandle signal_get_handle(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      പ്രതിജ്ഞാ(rfp);

      തിരഞ്ഞെടുപ്പ് (code) {

	  അവസ്ഥ vpiParent:
	    തിരികെ rfp->is_netarray? rfp->within.parent : ശൂന്യ;

	  അവസ്ഥ vpiIndex:
	    തിരികെ rfp->is_netarray? rfp->id.index : ശൂന്യ;

	  അവസ്ഥ vpi_ഇടത്_Range:
	    തിരികെ &rfp->msb;
	  അവസ്ഥ vpi_വലത്_Range:
	    തിരികെ &rfp->lsb;

	  അവസ്ഥ vpiScope:
	    തിരികെ vpip_scope(rfp);

	  അവസ്ഥ vpiModule:
	    തിരികെ vpip_module(vpip_scope(rfp));
      }

      തിരികെ 0;
}

സ്ഥിതം vpiHandle signal_iterate(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      പ്രതിജ്ഞാ(rfp);

      എങ്കിൽ (code == vpiIndex) {
	    തിരികെ rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : ശൂന്യ;
      }

      തിരികെ 0;
}

ശൂന്യം __vpiSignal::make_bits()
{
      പ്രതിജ്ഞാ(bits == ശൂന്യ);
      bits = പുതിയ ഘടന __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      bits->parent = ഇത്;
	// Now point to word[0].
      bits += 1;

	// Update each bit to point to the base
      ഘടന __vpiBit*base = bits;
      വേണ്ടി (unsigned idx = 0; idx < width(); idx += 1) {
	    base[idx].bit0 = base;
	    പൂർണ്ണസംഖ്യ real_idx;
	    എങ്കിൽ (msb.get_മൂല്യം() >= lsb.get_മൂല്യം()) {
		  real_idx = idx + lsb.get_മൂല്യം();
	    } അല്ലെങ്കിൽ {
		  real_idx = lsb.get_മൂല്യം() - idx;
	    }
	    base[idx].index = പുതിയ __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_index(പൂർണ്ണസംഖ്യ idx)
{
	/* Check to see if the index is in range. */
      എങ്കിൽ (msb.get_മൂല്യം() >= lsb.get_മൂല്യം()) {
	    എങ്കിൽ ((idx > msb.get_മൂല്യം()) || (idx < lsb.get_മൂല്യം())) തിരികെ 0;
      } അല്ലെങ്കിൽ {
	    എങ്കിൽ ((idx < msb.get_മൂല്യം()) || (idx > lsb.get_മൂല്യം())) തിരികെ 0;
      }

	/* Normalize the index */
      unsigned norm_idx;
      എങ്കിൽ (msb.get_മൂല്യം() >= lsb.get_മൂല്യം()) {
	    norm_idx = idx - lsb.get_മൂല്യം();
      } അല്ലെങ്കിൽ {
	    norm_idx = lsb.get_മൂല്യം() - idx;
      }

      എങ്കിൽ (bits == ശൂന്യ) make_bits();

      തിരികെ &(bits[norm_idx].as_bit);
}

ശൂന്യം __vpiSignal::get_bit_മൂല്യം(സ്ഥിരം ഘടന __vpiBit*bit, p_vpi_മൂല്യം vp)
{
      unsigned index = bit->get_norm_index();

      vvp_signal_മൂല്യം*vsig = dynamic_cast<vvp_signal_മൂല്യം*>(കണ്ണി->fil);
      പ്രതിജ്ഞാ(vsig);

      എങ്കിൽ (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      തിരഞ്ഞെടുപ്പ് (vp->format) {
	അവസ്ഥ vpiBinStrVal:
            format_vpiBinStrVal(vsig, index, 1, vp);
	    നിർത്തുക;

	അവസ്ഥ vpiOctStrVal:
            format_vpiOctStrVal(vsig, index, 1, vp);
	    നിർത്തുക;

	അവസ്ഥ vpiDecStrVal:
            format_vpiDecStrVal(vsig, index, 1, അസത്യം, vp);
	    നിർത്തുക;

	അവസ്ഥ vpiHexStrVal:
            format_vpiHexStrVal(vsig, index, 1, vp);
	    നിർത്തുക;

	അവസ്ഥ vpiStringVal:
            format_vpiStringVal(vsig, index, 1, vp);
	    നിർത്തുക;

	അവസ്ഥ vpiIntVal:
            format_vpiIntVal(vsig, index, 1, അസത്യം, vp);
	    നിർത്തുക;

	അവസ്ഥ vpiRealVal:
            format_vpiRealVal(vsig, index, 1, അസത്യം, vp);
	    നിർത്തുക;

	അവസ്ഥ vpiScalarVal:
            format_vpiScalarVal(vsig, index, vp);
	    നിർത്തുക;

        അവസ്ഥ vpiStrengthVal:
            format_vpiStrengthVal(vsig, index, 1, vp);
            നിർത്തുക;

	അവസ്ഥ vpiVectorVal:
            format_vpiVectorVal(vsig, index, 1, vp);
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (പൂർണ്ണസംഖ്യ)vp->format, vpi_get_str(vpiName),
		    bit->get_index(),
	            vpip_scope(ഇത്)->scope_name());
	    പ്രതിജ്ഞാ(0);
      };
}

vpiHandle __vpiSignal::put_bit_മൂല്യം(ഘടന __vpiBit*bit, p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ flags)
{
      unsigned index = bit->get_norm_index();
      vvp_net_ptr_t dest(കണ്ണി, 0);
      vvp_vector4_t val = vec4_from_vpi_മൂല്യം(vp, 1);

      എങ്കിൽ ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    bit->as_bit.vpi_get_str(vpiFullName));
	    തിരികെ ശൂന്യ;
      }

      എങ്കിൽ ((get_type_code() == vpiNet) &&
          !dynamic_cast<vvp_island_port*>(കണ്ണി->fun)) {
	    കണ്ണി->send_vec4_pv(val, index, width(),
	                            vthread_get_wt_context());
      } അല്ലെങ്കിൽ {
	    vvp_send_vec4_pv(dest, val, index, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      തിരികെ ശൂന്യ;
}

സ്ഥിതം vpiHandle signal_index(പൂർണ്ണസംഖ്യ idx, vpiHandle ref)
{
      ഘടന __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      പ്രതിജ്ഞാ(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      എങ്കിൽ ((type != vpiNet) && (type != vpiReg)) തിരികെ 0;

      തിരികെ rfp->get_index(idx);
}

unsigned __vpiSignal::width(ശൂന്യം) സ്ഥിരം
{
      unsigned wid = (msb.get_മൂല്യം() >= lsb.get_മൂല്യം())
	    ? (msb.get_മൂല്യം() - lsb.get_മൂല്യം() + 1)
	    : (lsb.get_മൂല്യം() - msb.get_മൂല്യം() + 1);

      തിരികെ wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
സ്ഥിതം ശൂന്യം signal_get_മൂല്യം(vpiHandle ref, s_vpi_മൂല്യം*vp)
{
      ഘടന __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      പ്രതിജ്ഞാ(rfp);

      unsigned wid = rfp->width();

      vvp_signal_മൂല്യം*vsig = dynamic_cast<vvp_signal_മൂല്യം*>(rfp->കണ്ണി->fil);
      പ്രതിജ്ഞാ(vsig);

      തിരഞ്ഞെടുപ്പ് (vp->format) {

	  അവസ്ഥ vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiVectorVal:
	    format_vpiVectorVal(vsig, 0, wid, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiObjTypeVal:
	    എങ്കിൽ (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } അല്ലെങ്കിൽ {
		  vp->format = vpiVectorVal;
		  format_vpiVectorVal(vsig, 0, wid, vp);
	    }
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (പൂർണ്ണസംഖ്യ)vp->format, vpi_get_str(vpiName, ref),
		    vpip_scope(rfp)->scope_name());
	    പ്രതിജ്ഞാ(0);
      }
}

/*
 * The put_value method writes the value into the vector, and returns
 * the affected ref. This operation works much like the %set or
 * %assign instructions and causes all the side-effects that the
 * equivalent instruction would cause.
 */

സ്ഥിതം vvp_vector4_t from_stringval(സ്ഥിരം അക്ഷരം*str, unsigned wid)
{
      unsigned idx;
      സ്ഥിരം അക്ഷരം*cp;

      cp = str + strlen(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      ആകുമ്പോൾ ((idx < wid) && (cp > str)) {
	    unsigned byte = *--cp;
	    പൂർണ്ണസംഖ്യ bit;

	    വേണ്ടി (bit = 0 ;  (bit < 8) && (idx < wid) ;  bit += 1) {
		  എങ്കിൽ (byte & 1)
			val.set_bit(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      തിരികെ val;
}

സ്ഥിതം vpiHandle signal_put_മൂല്യം(vpiHandle ref, s_vpi_മൂല്യം*vp, പൂർണ്ണസംഖ്യ flags)
{
      unsigned wid;
      ഘടന __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      പ്രതിജ്ഞാ(rfp);
      vvp_net_ptr_t dest(rfp->കണ്ണി, 0);

      തർക്കം net_flag = ref->get_type_code()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      എങ്കിൽ (flags == vpiReleaseFlag) {
	    പ്രതിജ്ഞാ(rfp->കണ്ണി->fil);
	    rfp->കണ്ണി->fil->force_unlink();
	    rfp->കണ്ണി->fil->release(dest, net_flag);
	    rfp->കണ്ണി->fun->force_flag(സത്യം);
	    signal_get_മൂല്യം(ref, vp);
	    തിരികെ ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_മൂല്യം() >= rfp->lsb.get_മൂല്യം())
	    ? (rfp->msb.get_മൂല്യം() - rfp->lsb.get_മൂല്യം() + 1)
	    : (rfp->lsb.get_മൂല്യം() - rfp->msb.get_മൂല്യം() + 1);

      vvp_vector4_t val = vec4_from_vpi_മൂല്യം(vp, wid);

      എങ്കിൽ (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, wid);
	    rfp->കണ്ണി->force_vec4(val, mask);
      } അല്ലെങ്കിൽ എങ്കിൽ (net_flag && !dynamic_cast<vvp_island_port*>(rfp->കണ്ണി->fun)) {
	    rfp->കണ്ണി->send_vec4(val, vthread_get_wt_context());
      } അല്ലെങ്കിൽ {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      തിരികെ ref;
}

vvp_vector4_t vec4_from_vpi_മൂല്യം(s_vpi_മൂല്യം*vp, unsigned wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      തിരഞ്ഞെടുപ്പ് (vp->format) {

	  അവസ്ഥ vpiIntVal: {
		long vpi_val = vp->value.integer;
		വേണ്ടി (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t bit = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.set_bit(idx, bit);
		      vpi_val >>= 1;
		}
		നിർത്തുക;
	  }

	  അവസ്ഥ vpiVectorVal:
	    വേണ്ടി (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long aval = vp->value.vector[idx/32].aval;
		  unsigned long bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  പൂർണ്ണസംഖ്യ bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t bit = scalar_to_bit4(bitmask);
		  val.set_bit(idx, bit);
	    }
	    നിർത്തുക;
	  അവസ്ഥ vpiBinStrVal:
	    vpip_bin_str_to_vec4(val, vp->value.str);
	    നിർത്തുക;
	  അവസ്ഥ vpiOctStrVal:
	    vpip_oct_str_to_vec4(val, vp->value.str);
	    നിർത്തുക;
	  അവസ്ഥ vpiDecStrVal:
	    vpip_dec_str_to_vec4(val, vp->value.str);
	    നിർത്തുക;
	  അവസ്ഥ vpiHexStrVal:
	    vpip_hex_str_to_vec4(val, vp->value.str);
	    നിർത്തുക;
	  അവസ്ഥ vpiScalarVal:
	    val.set_bit(0, scalar_to_bit4(vp->value.scalar));
	    നിർത്തുക;
	  അവസ്ഥ vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    നിർത്തുക;
	  അവസ്ഥ vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (പൂർണ്ണസംഖ്യ)vp->format);
	    പ്രതിജ്ഞാ(0);

      }

      തിരികെ val;
}

പൂർണ്ണസംഖ്യ __vpiSignal::vpi_get(പൂർണ്ണസംഖ്യ code)
{ തിരികെ signal_get(code, ഇത്); }

int64_t __vpiSignal::vpi_get64(പൂർണ്ണസംഖ്യ code)
{ തിരികെ signal_get64(code, ഇത്); }

അക്ഷരം* __vpiSignal::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{ തിരികെ signal_get_str(code, ഇത്); }

ശൂന്യം __vpiSignal::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{ signal_get_മൂല്യം(ഇത്, val); }

vpiHandle __vpiSignal::vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags)
{ തിരികെ signal_put_മൂല്യം(ഇത്, val, flags); }

vpiHandle __vpiSignal::vpi_handle(പൂർണ്ണസംഖ്യ code)
{ തിരികെ signal_get_handle(code, ഇത്); }

vpiHandle __vpiSignal::vpi_iterate(പൂർണ്ണസംഖ്യ code)
{ തിരികെ signal_iterate(code, ഇത്); }

vpiHandle __vpiSignal::vpi_index(പൂർണ്ണസംഖ്യ idx)
{ തിരികെ signal_index(idx, ഇത്); }

ഘടന signal_reg : പൊതു __vpiSignal {
      വരിയിൽ signal_reg() { }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiReg; }
};

ഘടന signal_integer : പൊതു __vpiSignal {
      വരിയിൽ signal_integer() { }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiIntegerVar; }
};

ഘടന signal_net : പൊതു __vpiSignal {
      വരിയിൽ signal_net() { }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiNet; }
};

ഘടന signal_byte : പൊതു __vpiSignal {
      വരിയിൽ signal_byte() { }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiByteVar; }
};

ഘടന signal_bitvar : പൊതു __vpiSignal {
      വരിയിൽ signal_bitvar() { }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiBitVar; }
};

ഘടന signal_shortint : പൊതു __vpiSignal {
      വരിയിൽ signal_shortint() { }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiShortIntVar; }
};

ഘടന signal_int : പൊതു __vpiSignal {
      വരിയിൽ signal_int() { }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiIntVar; }
};

ഘടന signal_longint : പൊതു __vpiSignal {
      വരിയിൽ signal_longint() { }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(സ്ഥിരം അക്ഷരം*name, പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = പുതിയ signal_integer;
      തിരികെ fill_in_var4(obj, name, msb, lsb, സത്യം, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(സ്ഥിരം അക്ഷരം*name, പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb, തർക്കം signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      എങ്കിൽ ((! signed_flag) || (lsb != 0) ) {
	    obj = പുതിയ signal_bitvar;
      } അല്ലെങ്കിൽ {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    തിരഞ്ഞെടുപ്പ് (msb) {
		അവസ്ഥ 7:
		  obj = പുതിയ signal_byte;
		  നിർത്തുക;
		അവസ്ഥ 15:
		  obj = പുതിയ signal_shortint;
		  നിർത്തുക;
		അവസ്ഥ 31:
		  obj = പുതിയ signal_int;
		  നിർത്തുക;
		അവസ്ഥ 63:
		  obj = പുതിയ signal_longint;
		  നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = പുതിയ signal_bitvar;
		  നിർത്തുക;
	    }
      }

      തിരികെ fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(സ്ഥിരം അക്ഷരം*name, പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb,
			തർക്കം signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = പുതിയ signal_reg;
      തിരികെ fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

#ifdef CHECK_WITH_VALGRIND
സ്ഥിതം ഘടന vpiSignal_plug **signal_pool = 0;
സ്ഥിതം unsigned signal_pool_എണ്ണം = 0;
സ്ഥിതം unsigned long signal_എണ്ണം = 0;
സ്ഥിതം unsigned long signal_dels = 0;
#endif

ഘടന vpiSignal_plug {
      unsigned അക്ഷരം space[വലുപ്പം (ഘടന __vpiSignal)];
#ifdef CHECK_WITH_VALGRIND
      ഘടന vpiSignal_plug *pool;
#endif
};

ശൂന്യം* __vpiSignal::operator പുതിയ(size_t siz)
{
#ifdef CHECK_WITH_VALGRIND
      പ്രതിജ്ഞാ(siz == വലുപ്പം(ഘടന vpiSignal_plug) - വലുപ്പം(ഘടന vpiSignal_plug *));
#അല്ലെങ്കിൽ
      പ്രതിജ്ഞാ(siz == വലുപ്പം(ഘടന vpiSignal_plug));
#endif
      സ്ഥിതം ഘടന vpiSignal_plug*alloc_array = 0;
      സ്ഥിതം unsigned alloc_index = 0;
      സ്ഥിരം unsigned alloc_എണ്ണം = 512;

      എങ്കിൽ ((alloc_array == 0) || (alloc_index == alloc_എണ്ണം)) {
	    alloc_array = static_cast<ഘടന vpiSignal_plug*>
	                  (calloc(alloc_എണ്ണം, വലുപ്പം(ഘടന vpiSignal_plug)));
	    alloc_index = 0;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_എണ്ണം *
	                                            വലുപ്പം(ഘടന vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_എണ്ണം += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_എണ്ണം*വലുപ്പം(vpiSignal_plug **)));
	    signal_pool[signal_pool_എണ്ണം-1] = alloc_array;
#endif
      }

      ഘടന vpiSignal_plug*cur = alloc_array + alloc_index;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, വലുപ്പം(ഘടന vpiSignal_plug));
      cur->pool = alloc_array;
      signal_എണ്ണം += 1;
#endif
      alloc_index += 1;
      തിരികെ cur;
}

ശൂന്യം __vpiSignal::operator delete(ശൂന്യം*)
{
      പ്രതിജ്ഞാ(0);
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം signal_delete(vpiHandle item)
{
      ഘടന __vpiSignal *obj = static_cast<__vpiSignal *> (item);
      പ്രതിജ്ഞാ(obj->കണ്ണി->fil);
      obj->കണ്ണി->fil->clear_all_callbacks();
      vvp_net_delete(obj->കണ്ണി);
      എങ്കിൽ (obj->bits) {
	    വേണ്ടി (unsigned idx=0; idx<obj->width(); idx+=1) {
		  delete obj->bits[idx].index;
	    }
	    obj->bits -= 1;
	    delete [] obj->bits;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

ശൂന്യം signal_pool_delete()
{
      എങ്കിൽ (RUNNING_ON_VALGRIND && (signal_എണ്ണം != signal_dels)) {
	    fflush(ശൂന്യ);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (long) signal_എണ്ണം - signal_dels, signal_എണ്ണം);
      }

      വേണ്ടി (unsigned idx = 0; idx < signal_pool_എണ്ണം; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    സ്വതന്ത്രം(signal_pool[idx]);
      }

      സ്വതന്ത്രം(signal_pool);
      signal_pool = 0;
      signal_pool_എണ്ണം = 0;
}
#endif

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
സ്ഥിതം vpiHandle fill_in_net4(ഘടന __vpiSignal*obj, __vpiScope*scope,
			      സ്ഥിരം അക്ഷരം*name, പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb,
			      തർക്കം signed_flag, vvp_net_t*കണ്ണി)
{
      obj->id.name = name? vpip_name_string(name) : ശൂന്യ;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->കണ്ണി = കണ്ണി;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      എണ്ണം_vpi_nets += 1;

      തിരികെ obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 സ്ഥിരം അക്ഷരം*name, പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb,
			 തർക്കം signed_flag, vvp_net_t*കണ്ണി)
{
      ഘടന __vpiSignal*obj = പുതിയ signal_net;
      തിരികെ fill_in_net4(obj, scope, name, msb, lsb, signed_flag, കണ്ണി);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ PV_get_base(ഘടന __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      എങ്കിൽ (rfp->sbase != 0) {
	    s_vpi_മൂല്യം val;
	      /* Check to see if the value is defined. */
	    val.format = vpiVectorVal;
	    vpi_get_മൂല്യം(rfp->sbase, &val);
	    പൂർണ്ണസംഖ്യ വാക്കുകൾ = (vpi_get(vpiSize, rfp->sbase)-1)/32 + 1;
	    വേണ്ടി(പൂർണ്ണസംഖ്യ idx = 0; idx < വാക്കുകൾ; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  എങ്കിൽ (val.value.vector[idx].bval != 0) തിരികെ INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_മൂല്യം(rfp->sbase, &val);
	    തിരികെ val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      തിരികെ rfp->tbase;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ PV_get(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      പ്രതിജ്ഞാ(rfp);

      പൂർണ്ണസംഖ്യ rval = 0;
      തിരഞ്ഞെടുപ്പ് (code) {
	അവസ്ഥ vpi_വരി_No:
	    തിരികെ 0;  // Not implemented for now!

	അവസ്ഥ vpiSigned:
	    തിരികെ 0;  // A part/bit select is always unsigned!

	അവസ്ഥ vpiSize:
	    തിരികെ rfp->width;

	  /* This is like the &A<> in array.cc. */
	അവസ്ഥ vpiConstantSelect:
	    തിരികെ rfp->sbase == 0;

	അവസ്ഥ vpi_ഇടത്_Range:
            rval += rfp->width - 1;
	    // fallthrough
	അവസ്ഥ vpi_വലത്_Range: {
	    പൂർണ്ണസംഖ്യ ഇടത് = vpi_get(vpi_ഇടത്_Range, rfp->parent);
	    പൂർണ്ണസംഖ്യ വലത് = vpi_get(vpi_വലത്_Range, rfp->parent);
	    rval += PV_get_base(rfp);
	    തിരികെ ഇടത് >= വലത് ? വലത് + rval : വലത് - rval;
	}

        അവസ്ഥ vpiAutomatic:
            തിരികെ vpi_get(vpiAutomatic, rfp->parent);

#എങ്കിൽ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        അവസ്ഥ _vpiFromThr:
            തിരികെ _vpi_at_PV;
#endif

	സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      തിരികെ 0;
}

സ്ഥിതം അക്ഷരം* PV_get_str(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      പ്രതിജ്ഞാ(rfp);

      തിരഞ്ഞെടുപ്പ് (code) {
	അവസ്ഥ vpiFile:  // Not implemented for now!
	    തിരികെ simple_set_rbuf_str(file_names[0]);

	അവസ്ഥ vpiName:
	അവസ്ഥ vpiFullName: {
	    സ്ഥിരം അക്ഷരം*nm = vpi_get_str(code, rfp->parent);
	    size_t len = 256+strlen(nm);
	    അക്ഷരം *full = static_cast<അക്ഷരം *>(malloc(len));
	    snprintf(full, len, "%s[%d:%d]", nm,
	                                     (പൂർണ്ണസംഖ്യ)vpi_get(vpi_ഇടത്_Range, ref),
	                                     (പൂർണ്ണസംഖ്യ)vpi_get(vpi_വലത്_Range, ref));
	    full[len-1] = 0;
	    അക്ഷരം *res = simple_set_rbuf_str(full);
	    സ്വതന്ത്രം(full);
	    തിരികെ res;
	}

	സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      തിരികെ 0;
}

സ്ഥിതം ശൂന്യം PV_get_മൂല്യം(vpiHandle ref, p_vpi_മൂല്യം vp)
{
      ഘടന __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      പ്രതിജ്ഞാ(rfp);

      vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*>(rfp->net->fil);
      പ്രതിജ്ഞാ(sig);

      തിരഞ്ഞെടുപ്പ് (vp->format) {

	  അവസ്ഥ vpiIntVal:
	    format_vpiIntVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiStringVal:
	    format_vpiStringVal(sig, PV_get_base(rfp), rfp->width, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_base(rfp), vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_base(rfp), rfp->width, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiVectorVal:
	    format_vpiVectorVal(sig, PV_get_base(rfp), rfp->width, vp);
	    നിർത്തുക;

	  അവസ്ഥ vpiRealVal:
	    format_vpiRealVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (പൂർണ്ണസംഖ്യ)vp->format, vpi_get_str(vpiFullName, rfp->parent));
	    പ്രതിജ്ഞാ(0);
      }
}

സ്ഥിതം vpiHandle PV_put_മൂല്യം(vpiHandle ref, p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ flags)
{
      ഘടന __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      പ്രതിജ്ഞാ(rfp);
      vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*>(rfp->net->fil);
      പ്രതിജ്ഞാ(sig);

      unsigned sig_size = sig->മൂല്യം_size();
      unsigned width = rfp->width;
      പൂർണ്ണസംഖ്യ base = PV_get_base(rfp);
      എങ്കിൽ (base >= (signed) sig_size) തിരികെ 0;
      എങ്കിൽ (base + (signed) width < 0) തിരികെ 0;

      vvp_vector4_t val;
      എങ്കിൽ (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_മൂല്യം(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      എങ്കിൽ (base < 0) {
	    width += base;
	    എങ്കിൽ (flags != vpiReleaseFlag) {
		  val = val.subvalue(-base, width);
	    }
	    base = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      എങ്കിൽ (base+width > sig_size) {
	    width = sig_size - base;
	    എങ്കിൽ (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      പ്രതിജ്ഞാ(rfp->parent);
      തർക്കം net_flag = rfp->parent->get_type_code()==vpiNet;
      തർക്കം full_sig = base == 0 && width == sig_size;

      vvp_net_ptr_t dest(rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value.  Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value.*/
      എങ്കിൽ (flags == vpiReleaseFlag) {
	    പ്രതിജ്ഞാ(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    എങ്കിൽ (full_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } അല്ലെങ്കിൽ {
		  rfp->net->fil->release_pv(dest, base, width, net_flag);
	    }
	    rfp->net->fun->force_flag(സത്യം);
	    PV_get_മൂല്യം(ref, vp);
	    തിരികെ ref;
      }

      എങ്കിൽ (flags == vpiForceFlag) {
	    എങ്കിൽ (full_sig) {
		  vvp_vector2_t mask (vvp_vector2_t::FILL1, sig_size);
		  rfp->net->force_vec4(val, mask);
	    } അല്ലെങ്കിൽ {
		  vvp_vector2_t mask (vvp_vector2_t::FILL0, sig_size);
		  വേണ്ടി (unsigned idx = 0 ; idx < width ; idx += 1)
		        mask.set_bit(base+idx, 1);

		  vvp_vector4_t tmp (sig_size, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_മൂല്യം(tmp);

		  tmp.set_vec(base, val);
		  rfp->net->force_vec4(tmp, mask);
	    }
      } അല്ലെങ്കിൽ എങ്കിൽ (net_flag && !dynamic_cast<vvp_island_port*>(rfp->net->fun)) {
	    എങ്കിൽ (full_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } അല്ലെങ്കിൽ {
		  rfp->net->send_vec4_pv(val, base, sig_size,
		                         vthread_get_wt_context());
	    }
      } അല്ലെങ്കിൽ {
	    എങ്കിൽ (full_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } അല്ലെങ്കിൽ {
		  vvp_send_vec4_pv(dest, val, base, sig_size,
	                           vthread_get_wt_context());
	    }
      }
      തിരികെ 0;
}

സ്ഥിതം vpiHandle PV_get_handle(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      പ്രതിജ്ഞാ(rfp);

      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiParent:
	    തിരികെ rfp->parent;

	  അവസ്ഥ vpiScope:
	    തിരികെ vpi_handle(vpiScope, rfp->parent);

	  അവസ്ഥ vpiModule:
	    തിരികെ vpi_handle(vpiModule, rfp->parent);
      }

      തിരികെ 0;
}

വരിയിൽ __vpiPV::__vpiPV()
{ }

പൂർണ്ണസംഖ്യ __vpiPV::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiPartSelect; }

പൂർണ്ണസംഖ്യ __vpiPV::vpi_get(പൂർണ്ണസംഖ്യ code)
{ തിരികെ PV_get(code, ഇത്); }

അക്ഷരം* __vpiPV::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{ തിരികെ PV_get_str(code, ഇത്); }

ശൂന്യം __vpiPV::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{ PV_get_മൂല്യം(ഇത്, val); }

vpiHandle __vpiPV::vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags)
{ തിരികെ PV_put_മൂല്യം(ഇത്, val, flags); }

vpiHandle __vpiPV::vpi_handle(പൂർണ്ണസംഖ്യ code)
{ തിരികെ PV_get_handle(code, ഇത്); }

vpiHandle vpip_make_PV(അക്ഷരം*var, പൂർണ്ണസംഖ്യ base, പൂർണ്ണസംഖ്യ width)
{
      ഘടന __vpiPV*obj = പുതിയ __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = 0;
      obj->tbase = base;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      തിരികെ obj;
}

vpiHandle vpip_make_PV(അക്ഷരം*var, അക്ഷരം*ചിഹ്നം, പൂർണ്ണസംഖ്യ width)
{
      ഘടന __vpiPV*obj = പുതിയ __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      compile_vpi_lookup(&obj->sbase, ചിഹ്നം);
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      തിരികെ obj;
}

vpiHandle vpip_make_PV(അക്ഷരം*var, vpiHandle handle, പൂർണ്ണസംഖ്യ width)
{
      ഘടന __vpiPV*obj = പുതിയ __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      തിരികെ obj;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം PV_delete(vpiHandle item)
{
      ഘടന __vpiPV *obj = dynamic_cast<__vpiPV*>(item);
      എങ്കിൽ (obj->sbase) {
	    തിരഞ്ഞെടുപ്പ് (obj->sbase->get_type_code()) {
		അവസ്ഥ vpiMemoryWord:
		  എങ്കിൽ (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  നിർത്തുക;
		അവസ്ഥ vpiPartSelect:
		  പ്രതിജ്ഞാ(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  നിർത്തുക;
	    }
      }
      പ്രതിജ്ഞാ(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      delete obj;
}
#endif
