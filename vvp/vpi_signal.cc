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
ಬಾಹ್ಯ ಧ್ರುವ ವರ್ಣ hex_digits[256];
ಬಾಹ್ಯ ಧ್ರುವ ವರ್ಣ oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
ಶೂನ್ಯ *need_result_buf(unsigned cnt, vpi_rbuf_t type)
{
      ಸ್ಥಿರ ಶೂನ್ಯ*result_buf[2] = {0, 0};
      ಸ್ಥಿರ size_t result_buf_size[2] = {0, 0};

      ಯದಿ (type == RBUF_DEL) {
	    ಮುಕ್ತ(result_buf[RBUF_VAL]);
	    result_buf[RBUF_VAL] = 0;
	    result_buf_size[RBUF_VAL] = 0;

	    ಮುಕ್ತ(result_buf[RBUF_STR]);
	    result_buf[RBUF_STR] = 0;
	    result_buf_size[RBUF_STR] = 0;

	    ಪ್ರತ್ಯಾಗಮ 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      ಯದಿ (result_buf_size[type] == 0) {
	    result_buf[type] = malloc(cnt);
	    result_buf_size[type] = cnt;
      } ಅನ್ಯಥಾ ಯದಿ (result_buf_size[type] < cnt) {
	    result_buf[type] = realloc(result_buf[type], cnt);
	    result_buf_size[type] = cnt;
      }

      ಪ್ರತ್ಯಾಗಮ result_buf[type];
}

ವರ್ಣ *simple_set_rbuf_str(ಧ್ರುವ ವರ್ಣ *s1)
{
      ವರ್ಣ *res = static_cast<ವರ್ಣ *>(need_result_buf(strlen(s1)+1, RBUF_STR));
      ಯದಿ (res) strcpy(res,s1);
      ಪ್ರತ್ಯಾಗಮ res;
}

ವರ್ಣ *generic_get_str(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref, ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *index)
{
      size_t len = strlen(name) + 1;  /* include space for null termination */
      ವರ್ಣ *bn = ರಿಕ್ತ;
      ತಾರ್ಕಿಕ is_bn_a_pkg = ಅಸತ್ಯ;
      ಯದಿ (code == vpiFullName) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = strlen(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    ಯದಿ (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      ಯದಿ (index != ರಿಕ್ತ) len += strlen(index) + 2;  /* include space for brackets */

      ವರ್ಣ *res = static_cast<ವರ್ಣ *>(need_result_buf(len, RBUF_STR));
      ಯದಿ (!res) {
	    ಮುಕ್ತ(bn);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      ಯದಿ (bn != ರಿಕ್ತ) {
	    strcat(res, bn);
	      // A package already has the "::" separator in the name
	    ಯದಿ (! is_bn_a_pkg) strcat(res, ".");
	    ಮುಕ್ತ(bn);
      }
      strcat(res, name);
      ಯದಿ (index != ರಿಕ್ತ) {
	    strcat(res, "[");
	    strcat(res, index);
	    strcat(res, "]");
      }
      ಪ್ರತ್ಯಾಗಮ res;
}

ಸ್ಥಿರ vpiHandle fill_in_net4(ಸಂರಚನಾ __vpiSignal*obj, __vpiScope*scope,
                              ಧ್ರುವ ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb,
                              ತಾರ್ಕಿಕ signed_flag, vvp_net_t*ಗಂಟು);

ಸ್ಥಿರ vpiHandle fill_in_var4(ಸಂರಚನಾ __vpiSignal*obj,
                              ಧ್ರುವ ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb,
                              ತಾರ್ಕಿಕ signed_flag, vvp_net_t*ಗಂಟು)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      ಪ್ರತ್ಯಾಗಮ fill_in_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, ಗಂಟು);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiBinStrVal(ಧ್ರುವ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಙ್ಕ base, unsigned wid,
                                s_vpi_ಮೌಲ್ಯ*vp)
{
      ವರ್ಣ *rbuf = static_cast<ವರ್ಣ *>(need_result_buf(wid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long offset = end - 1;
      long ssize = (signed)sig->ಮೌಲ್ಯ_size();

      ಪ್ರತಿ (long idx = base ;  idx < end ;  idx += 1) {
	    ಯದಿ (idx < 0 || idx >= ssize) {
                  rbuf[offset-idx] = 'x';
	    } ಅನ್ಯಥಾ {
                  rbuf[offset-idx] = vvp_bit4_to_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiOctStrVal(ಧ್ರುವ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಙ್ಕ base, unsigned wid,
                                s_vpi_ಮೌಲ್ಯ*vp)
{
      unsigned dwid = (wid + 2) / 3;
      ವರ್ಣ *rbuf = static_cast<ವರ್ಣ *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->ಮೌಲ್ಯ_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      ಪ್ರತಿ (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    ಯದಿ (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } ಅನ್ಯಥಾ {
                  ವಿಕಲ್ಪ (sig->value(idx)) {
		      ಪಕ್ಷ BIT4_0:
			bit = 0;
			ಭಙ್ಗ;
		      ಪಕ್ಷ BIT4_1:
			bit = 1;
			ಭಙ್ಗ;
		      ಪಕ್ಷ BIT4_X:
			bit = 2;
			ಭಙ್ಗ;
		      ಪಕ್ಷ BIT4_Z:
			bit = 3;
			ಭಙ್ಗ;
                  }
	    }
	    val |= bit << 2*((idx-base) % 3);

	    ಯದಿ ((idx-base) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ವಿಕಲ್ಪ (wid % 3) {
	  ಪಕ್ಷ 1:
	    ಯದಿ (val == 2) val = 42;
	    ಅನ್ಯಥಾ ಯದಿ (val == 3) val = 63;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 2:
	    ಯದಿ (val == 10) val = 42;
	    ಅನ್ಯಥಾ ಯದಿ (val == 15) val = 63;
	    ಭಙ್ಗ;
      }

      ಯದಿ (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiHexStrVal(ಧ್ರುವ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಙ್ಕ base, unsigned wid,
                                s_vpi_ಮೌಲ್ಯ*vp)
{
      unsigned dwid = (wid + 3) / 4;
      ವರ್ಣ *rbuf = static_cast<ವರ್ಣ *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->ಮೌಲ್ಯ_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      ಪ್ರತಿ (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    ಯದಿ (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } ಅನ್ಯಥಾ {
                  ವಿಕಲ್ಪ (sig->value(idx)) {
		      ಪಕ್ಷ BIT4_0:
			bit = 0;
			ಭಙ್ಗ;
		      ಪಕ್ಷ BIT4_1:
			bit = 1;
			ಭಙ್ಗ;
		      ಪಕ್ಷ BIT4_X:
			bit = 2;
			ಭಙ್ಗ;
		      ಪಕ್ಷ BIT4_Z:
			bit = 3;
			ಭಙ್ಗ;
                  }
	    }
	    val |= bit << 2*((idx-base) % 4);

	    ಯದಿ ((idx-base) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ವಿಕಲ್ಪ (wid % 4) {
	  ಪಕ್ಷ 1:
	    ಯದಿ (val == 2) val = 170;
	    ಅನ್ಯಥಾ ಯದಿ (val == 3) val = 255;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 2:
	    ಯದಿ (val == 10) val = 170;
	    ಅನ್ಯಥಾ ಯದಿ (val == 15) val = 255;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 3:
	    ಯದಿ (val == 42) val = 170;
	    ಅನ್ಯಥಾ ಯದಿ (val == 63) val = 255;
	    ಭಙ್ಗ;
      }

      ಯದಿ (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiDecStrVal(ಧ್ರುವ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಙ್ಕ base, unsigned wid,
                                ಪೂರ್ಣಾಙ್ಕ signed_flag, s_vpi_ಮೌಲ್ಯ*vp)
{
      unsigned hwid = (sig->ಮೌಲ್ಯ_size()+2) / 3 + 1;
      ವರ್ಣ *rbuf = static_cast<ವರ್ಣ *>(need_result_buf(hwid, RBUF_VAL));
      long ssize = (signed)sig->ಮೌಲ್ಯ_size();
      long end = base + (signed)wid;

	/* Do we have an end outside of the real signal vector. */
      ಯದಿ (base < 0 || end > ssize) {
	    ತಾರ್ಕಿಕ all_x = ಸತ್ಯ;
	    ಯದಿ (end > ssize) end = ssize;
	    ಯದಿ (base < 0) base = 0;
	    ಪ್ರತಿ (long idx = base ;  idx < end ;  idx += 1) {
		  ಯದಿ (sig->value(idx) != BIT4_X) {
			all_x = ಅಸತ್ಯ;
			ಭಙ್ಗ;
		  }
	    }

	    ಯದಿ (all_x) {
		  rbuf[0] = 'x';
	    } ಅನ್ಯಥಾ {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    ಪ್ರತ್ಯಾಗಮ;
      }

      vvp_vector4_t vec4;
      ಯದಿ (base == 0 && end == ssize) {
	    sig->vec4_ಮೌಲ್ಯ(vec4);
      } ಅನ್ಯಥಾ {
	    vvp_vector4_t tmp;
	    sig->vec4_ಮೌಲ್ಯ(tmp);
	    vec4 = tmp.subvalue(base, wid);
      }

      vpip_vec4_to_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiIntVal(ಧ್ರುವ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಙ್ಕ base, unsigned wid,
                             ಪೂರ್ಣಾಙ್ಕ signed_flag, s_vpi_ಮೌಲ್ಯ*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_ಮೌಲ್ಯ(tmp);
      vvp_vector4_t sub = tmp.subvalue(base, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      ಯದಿ (ಪರಿಮಾಣ(vp->value.integer) == ಪರಿಮಾಣ(int32_t)) {
	    int32_t val = 0;
	    vector4_to_ಮೌಲ್ಯ(sub, val, signed_flag, ಅಸತ್ಯ);
	    vp->value.integer = val;
      } ಅನ್ಯಥಾ {
	    ಪ್ರತಿಜ್ಞಾ(ಪರಿಮಾಣ(vp->value.integer) == ಪರಿಮಾಣ(int64_t));
	    int64_t val = 0;
	    vector4_to_ಮೌಲ್ಯ(sub, val, signed_flag, ಅಸತ್ಯ);
	    vp->value.integer = val;
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiRealVal(ಧ್ರುವ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಙ್ಕ base, unsigned wid,
                              ಪೂರ್ಣಾಙ್ಕ signed_flag, s_vpi_ಮೌಲ್ಯ*vp)
{
      vvp_vector4_t vec4(wid);
      long ssize = (signed)sig->ಮೌಲ್ಯ_size();
      long end = base + (signed)wid;
      ಯದಿ (end > ssize) end = ssize;

      ಪ್ರತಿ (long idx = (base < 0) ? 0 : base ;  idx < end ;  idx += 1) {
	    vec4.set_bit(idx-base, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_to_ಮೌಲ್ಯ(vec4, vp->value.real, signed_flag);
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiStringVal(ಧ್ರುವ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಙ್ಕ base, unsigned wid,
                                s_vpi_ಮೌಲ್ಯ*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      ವರ್ಣ *rbuf = static_cast<ವರ್ಣ *>(need_result_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      ವರ್ಣ *cp = rbuf;

      ವರ್ಣ tmp = 0;
      ಪ್ರತಿ (long idx = base+(signed)wid-1; idx >= base; idx -= 1) {
	    tmp <<= 1;

	    ಯದಿ (idx >=0 && idx < (signed)sig->ಮೌಲ್ಯ_size() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    ಯದಿ (((idx-base)&7)==0){
		  /* Skip leading nulls. */
		  ಯದಿ (tmp == 0 && cp == rbuf)
			ಅನುವರ್ತ;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiScalarVal(ಧ್ರುವ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಙ್ಕ base,
                                s_vpi_ಮೌಲ್ಯ*vp)
{
      ಯದಿ (base >= 0 && base < (signed)sig->ಮೌಲ್ಯ_size()) {
	    ವಿಕಲ್ಪ (sig->value(base)) {
		ಪಕ್ಷ BIT4_0:
		  vp->value.scalar = vpi0;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_1:
		  vp->value.scalar = vpi1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_ಮೌಲ್ಯ(base);
		  ಯದಿ (strn.strength0() == 1) vp->value.scalar = vpiH;
		  ಅನ್ಯಥಾ ಯದಿ (strn.strength1() == 1) vp->value.scalar = vpiL;
		  ಅನ್ಯಥಾ vp->value.scalar = vpiX;
		  ಭಙ್ಗ;
		}
		ಪಕ್ಷ BIT4_Z:
		  vp->value.scalar = vpiZ;
		  ಭಙ್ಗ;
	    }
      } ಅನ್ಯಥಾ {
	    vp->value.scalar = vpiX;
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiStrengthVal(ಧ್ರುವ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಙ್ಕ base,
                                  unsigned wid, s_vpi_ಮೌಲ್ಯ*vp)
{
      long end = base + (signed)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_result_buf(wid * ಪರಿಮಾಣ(s_vpi_strengthval), RBUF_VAL));

      ಪ್ರತಿ (long idx = base ;  idx < end ;  idx += 1) {
	    ಯದಿ (idx >=0 && idx < (signed)sig->ಮೌಲ್ಯ_size()) {
		  vvp_scalar_t val = sig->scalar_ಮೌಲ್ಯ(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  unsigned s0 = 1 << val.strength0();
		  unsigned s1 = 1 << val.strength1();

		  ವಿಕಲ್ಪ (val.value()) {
		      ಪಕ್ಷ BIT4_0:
			op[idx-base].logic = vpi0;
			op[idx-base].s0 = s0|s1;
			op[idx-base].s1 = 0;
			ಭಙ್ಗ;

		      ಪಕ್ಷ BIT4_1:
			op[idx-base].logic = vpi1;
			op[idx-base].s0 = 0;
			op[idx-base].s1 = s0|s1;
			ಭಙ್ಗ;

		      ಪಕ್ಷ BIT4_X:
			op[idx-base].logic = vpiX;
			op[idx-base].s0 = s0;
			op[idx-base].s1 = s1;
			ಭಙ್ಗ;

		      ಪಕ್ಷ BIT4_Z:
			op[idx-base].logic = vpiZ;
			op[idx-base].s0 = vpiHiZ;
			op[idx-base].s1 = vpiHiZ;
			ಭಙ್ಗ;
		  }
	    } ಅನ್ಯಥಾ {
		  op[idx-base].logic = vpiX;
		  op[idx-base].s0 = vpiStrongDrive;
		  op[idx-base].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

ಸ್ಥಿರ ಶೂನ್ಯ format_vpiVectorVal(ಧ್ರುವ vvp_signal_ಮೌಲ್ಯ*sig, ಪೂರ್ಣಾಙ್ಕ base, unsigned wid,
                                s_vpi_ಮೌಲ್ಯ*vp)
{
      long end = base + (signed)wid;
      unsigned ಪೂರ್ಣಾಙ್ಕ obit = 0;
      unsigned hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_result_buf(hwid * ಪರಿಮಾಣ(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      ಪ್ರತಿ (long idx = base ;  idx < end ;  idx += 1) {
	    ಯದಿ (base >= 0 && base < (signed)sig->ಮೌಲ್ಯ_size()) {
		ವಿಕಲ್ಪ (sig->value(idx)) {
		ಪಕ್ಷ BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  ಭಙ್ಗ;
		}
	    } ಅನ್ಯಥಾ {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    ಯದಿ (!(obit % 32)) {
		  op += 1;
		  ಯದಿ ((op - vp->value.vector) < (ptrdiff_t)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ signal_get(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ 0;  // Not implemented for now!

	  ಪಕ್ಷ vpiSigned:
	    ಪ್ರತ್ಯಾಗಮ rfp->signed_flag != 0;

	  ಪಕ್ಷ vpiArray:
	    ಪ್ರತ್ಯಾಗಮ rfp->is_netarray != 0;

	  ಪಕ್ಷ vpiIndex: // This only works while we have a single index.
	    ಯದಿ (rfp->is_netarray) {
		  s_vpi_ಮೌಲ್ಯ vp;
		  vp.format = vpiIntVal;
		  vpi_get_ಮೌಲ್ಯ(rfp->id.index, &vp);
		  ಪ್ರತ್ಯಾಗಮ vp.value.integer;
	    } ಅನ್ಯಥಾ {
		  ಪ್ರತ್ಯಾಗಮ vpiUndefined;
	    }

	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ rfp->width();

	  ಪಕ್ಷ vpiNetType:
	    ಯದಿ (ref->get_type_code()==vpiNet)
		  ಪ್ರತ್ಯಾಗಮ vpiWire;
	    ಅನ್ಯಥಾ
		  ಪ್ರತ್ಯಾಗಮ vpiUndefined;

	  ಪಕ್ಷ vpi_ಎಡ_Range:
            ಪ್ರತ್ಯಾಗಮ rfp->msb.get_ಮೌಲ್ಯ();

	  ಪಕ್ಷ vpi_ಬಲ_Range:
            ಪ್ರತ್ಯಾಗಮ rfp->lsb.get_ಮೌಲ್ಯ();

	  ಪಕ್ಷ vpiScalar:
	    ಪ್ರತ್ಯಾಗಮ (rfp->msb.get_ಮೌಲ್ಯ() == 0 && rfp->lsb.get_ಮೌಲ್ಯ() == 0);
	  ಪಕ್ಷ vpiVector:
	    ಪ್ರತ್ಯಾಗಮ (rfp->msb.get_ಮೌಲ್ಯ() != rfp->lsb.get_ಮೌಲ್ಯ());

          ಪಕ್ಷ vpiAutomatic:
            ಪ್ರತ್ಯಾಗಮ vpip_scope(rfp)->is_automatic() ? 1  : 0;

#ifdef BR916_STOPGAP_FIX
          ಪಕ್ಷ _vpiFromThr:
            ಪ್ರತ್ಯಾಗಮ _vpiNoThr;
#endif

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    ಪ್ರತ್ಯಾಗಮ vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
ಸ್ಥಿರ int64_t signal_get64(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ವಿಕಲ್ಪ (code) {
	    // This private property must return zero when undefined.
	  ಪಕ್ಷ _vpiNexusId:
	    ಯದಿ (rfp->msb.get_ಮೌಲ್ಯ() == rfp->lsb.get_ಮೌಲ್ಯ())
		  ಪ್ರತ್ಯಾಗಮ reinterpret_cast<int64_t>(rfp->ಗಂಟು);
	    ಪ್ರತ್ಯಾಗಮ 0;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    ಪ್ರತ್ಯಾಗಮ vpiUndefined;
      }
}

ಸ್ಥಿರ ವರ್ಣ* signal_get_str(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಯದಿ (code == vpiFile) {  // Not implemented for now!
	    ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[0]);
      }

      ಯದಿ ((code != vpiName) && (code != vpiFullName)) ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

      ವರ್ಣ *nm;
      ಧ್ರುವ ವರ್ಣ *ixs;
      ಯದಿ (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_ಮೌಲ್ಯ vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_ಮೌಲ್ಯ(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } ಅನ್ಯಥಾ {
	    nm = strdup(rfp->id.name);
	    ixs = ರಿಕ್ತ;
      }

	/* The scope information is added here for vpiFullName. */
      ವರ್ಣ *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      ಮುಕ್ತ(nm);
      ಪ್ರತ್ಯಾಗಮ rbuf;
}

ಸ್ಥಿರ vpiHandle signal_get_handle(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ವಿಕಲ್ಪ (code) {

	  ಪಕ್ಷ vpiParent:
	    ಪ್ರತ್ಯಾಗಮ rfp->is_netarray? rfp->within.parent : ರಿಕ್ತ;

	  ಪಕ್ಷ vpiIndex:
	    ಪ್ರತ್ಯಾಗಮ rfp->is_netarray? rfp->id.index : ರಿಕ್ತ;

	  ಪಕ್ಷ vpi_ಎಡ_Range:
	    ಪ್ರತ್ಯಾಗಮ &rfp->msb;
	  ಪಕ್ಷ vpi_ಬಲ_Range:
	    ಪ್ರತ್ಯಾಗಮ &rfp->lsb;

	  ಪಕ್ಷ vpiScope:
	    ಪ್ರತ್ಯಾಗಮ vpip_scope(rfp);

	  ಪಕ್ಷ vpiModule:
	    ಪ್ರತ್ಯಾಗಮ vpip_module(vpip_scope(rfp));
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ vpiHandle signal_iterate(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಯದಿ (code == vpiIndex) {
	    ಪ್ರತ್ಯಾಗಮ rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : ರಿಕ್ತ;
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ __vpiSignal::make_bits()
{
      ಪ್ರತಿಜ್ಞಾ(bits == ರಿಕ್ತ);
      bits = ನೂತನ ಸಂರಚನಾ __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      bits->parent = ಏತತ್;
	// Now point to word[0].
      bits += 1;

	// Update each bit to point to the base
      ಸಂರಚನಾ __vpiBit*base = bits;
      ಪ್ರತಿ (unsigned idx = 0; idx < width(); idx += 1) {
	    base[idx].bit0 = base;
	    ಪೂರ್ಣಾಙ್ಕ real_idx;
	    ಯದಿ (msb.get_ಮೌಲ್ಯ() >= lsb.get_ಮೌಲ್ಯ()) {
		  real_idx = idx + lsb.get_ಮೌಲ್ಯ();
	    } ಅನ್ಯಥಾ {
		  real_idx = lsb.get_ಮೌಲ್ಯ() - idx;
	    }
	    base[idx].index = ನೂತನ __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_index(ಪೂರ್ಣಾಙ್ಕ idx)
{
	/* Check to see if the index is in range. */
      ಯದಿ (msb.get_ಮೌಲ್ಯ() >= lsb.get_ಮೌಲ್ಯ()) {
	    ಯದಿ ((idx > msb.get_ಮೌಲ್ಯ()) || (idx < lsb.get_ಮೌಲ್ಯ())) ಪ್ರತ್ಯಾಗಮ 0;
      } ಅನ್ಯಥಾ {
	    ಯದಿ ((idx < msb.get_ಮೌಲ್ಯ()) || (idx > lsb.get_ಮೌಲ್ಯ())) ಪ್ರತ್ಯಾಗಮ 0;
      }

	/* Normalize the index */
      unsigned norm_idx;
      ಯದಿ (msb.get_ಮೌಲ್ಯ() >= lsb.get_ಮೌಲ್ಯ()) {
	    norm_idx = idx - lsb.get_ಮೌಲ್ಯ();
      } ಅನ್ಯಥಾ {
	    norm_idx = lsb.get_ಮೌಲ್ಯ() - idx;
      }

      ಯದಿ (bits == ರಿಕ್ತ) make_bits();

      ಪ್ರತ್ಯಾಗಮ &(bits[norm_idx].as_bit);
}

ಶೂನ್ಯ __vpiSignal::get_bit_ಮೌಲ್ಯ(ಧ್ರುವ ಸಂರಚನಾ __vpiBit*bit, p_vpi_ಮೌಲ್ಯ vp)
{
      unsigned index = bit->get_norm_index();

      vvp_signal_ಮೌಲ್ಯ*vsig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(ಗಂಟು->fil);
      ಪ್ರತಿಜ್ಞಾ(vsig);

      ಯದಿ (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      ವಿಕಲ್ಪ (vp->format) {
	ಪಕ್ಷ vpiBinStrVal:
            format_vpiBinStrVal(vsig, index, 1, vp);
	    ಭಙ್ಗ;

	ಪಕ್ಷ vpiOctStrVal:
            format_vpiOctStrVal(vsig, index, 1, vp);
	    ಭಙ್ಗ;

	ಪಕ್ಷ vpiDecStrVal:
            format_vpiDecStrVal(vsig, index, 1, ಅಸತ್ಯ, vp);
	    ಭಙ್ಗ;

	ಪಕ್ಷ vpiHexStrVal:
            format_vpiHexStrVal(vsig, index, 1, vp);
	    ಭಙ್ಗ;

	ಪಕ್ಷ vpiStringVal:
            format_vpiStringVal(vsig, index, 1, vp);
	    ಭಙ್ಗ;

	ಪಕ್ಷ vpiIntVal:
            format_vpiIntVal(vsig, index, 1, ಅಸತ್ಯ, vp);
	    ಭಙ್ಗ;

	ಪಕ್ಷ vpiRealVal:
            format_vpiRealVal(vsig, index, 1, ಅಸತ್ಯ, vp);
	    ಭಙ್ಗ;

	ಪಕ್ಷ vpiScalarVal:
            format_vpiScalarVal(vsig, index, vp);
	    ಭಙ್ಗ;

        ಪಕ್ಷ vpiStrengthVal:
            format_vpiStrengthVal(vsig, index, 1, vp);
            ಭಙ್ಗ;

	ಪಕ್ಷ vpiVectorVal:
            format_vpiVectorVal(vsig, index, 1, vp);
	    ಭಙ್ಗ;

	ಸ್ವಭಾವ:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (ಪೂರ್ಣಾಙ್ಕ)vp->format, vpi_get_str(vpiName),
		    bit->get_index(),
	            vpip_scope(ಏತತ್)->scope_name());
	    ಪ್ರತಿಜ್ಞಾ(0);
      };
}

vpiHandle __vpiSignal::put_bit_ಮೌಲ್ಯ(ಸಂರಚನಾ __vpiBit*bit, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ flags)
{
      unsigned index = bit->get_norm_index();
      vvp_net_ptr_t dest(ಗಂಟು, 0);
      vvp_vector4_t val = vec4_from_vpi_ಮೌಲ್ಯ(vp, 1);

      ಯದಿ ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    bit->as_bit.vpi_get_str(vpiFullName));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
      }

      ಯದಿ ((get_type_code() == vpiNet) &&
          !dynamic_cast<vvp_island_port*>(ಗಂಟು->fun)) {
	    ಗಂಟು->send_vec4_pv(val, index, width(),
	                            vthread_get_wt_context());
      } ಅನ್ಯಥಾ {
	    vvp_send_vec4_pv(dest, val, index, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ vpiHandle signal_index(ಪೂರ್ಣಾಙ್ಕ idx, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      ಯದಿ ((type != vpiNet) && (type != vpiReg)) ಪ್ರತ್ಯಾಗಮ 0;

      ಪ್ರತ್ಯಾಗಮ rfp->get_index(idx);
}

unsigned __vpiSignal::width(ಶೂನ್ಯ) ಧ್ರುವ
{
      unsigned wid = (msb.get_ಮೌಲ್ಯ() >= lsb.get_ಮೌಲ್ಯ())
	    ? (msb.get_ಮೌಲ್ಯ() - lsb.get_ಮೌಲ್ಯ() + 1)
	    : (lsb.get_ಮೌಲ್ಯ() - msb.get_ಮೌಲ್ಯ() + 1);

      ಪ್ರತ್ಯಾಗಮ wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
ಸ್ಥಿರ ಶೂನ್ಯ signal_get_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp)
{
      ಸಂರಚನಾ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      unsigned wid = rfp->width();

      vvp_signal_ಮೌಲ್ಯ*vsig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(rfp->ಗಂಟು->fil);
      ಪ್ರತಿಜ್ಞಾ(vsig);

      ವಿಕಲ್ಪ (vp->format) {

	  ಪಕ್ಷ vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiVectorVal:
	    format_vpiVectorVal(vsig, 0, wid, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiObjTypeVal:
	    ಯದಿ (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } ಅನ್ಯಥಾ {
		  vp->format = vpiVectorVal;
		  format_vpiVectorVal(vsig, 0, wid, vp);
	    }
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (ಪೂರ್ಣಾಙ್ಕ)vp->format, vpi_get_str(vpiName, ref),
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

ಸ್ಥಿರ vvp_vector4_t from_stringval(ಧ್ರುವ ವರ್ಣ*str, unsigned wid)
{
      unsigned idx;
      ಧ್ರುವ ವರ್ಣ*cp;

      cp = str + strlen(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      ಯಾವತ್ ((idx < wid) && (cp > str)) {
	    unsigned byte = *--cp;
	    ಪೂರ್ಣಾಙ್ಕ bit;

	    ಪ್ರತಿ (bit = 0 ;  (bit < 8) && (idx < wid) ;  bit += 1) {
		  ಯದಿ (byte & 1)
			val.set_bit(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      ಪ್ರತ್ಯಾಗಮ val;
}

ಸ್ಥಿರ vpiHandle signal_put_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp, ಪೂರ್ಣಾಙ್ಕ flags)
{
      unsigned wid;
      ಸಂರಚನಾ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);
      vvp_net_ptr_t dest(rfp->ಗಂಟು, 0);

      ತಾರ್ಕಿಕ net_flag = ref->get_type_code()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      ಯದಿ (flags == vpiReleaseFlag) {
	    ಪ್ರತಿಜ್ಞಾ(rfp->ಗಂಟು->fil);
	    rfp->ಗಂಟು->fil->force_unlink();
	    rfp->ಗಂಟು->fil->release(dest, net_flag);
	    rfp->ಗಂಟು->fun->force_flag(ಸತ್ಯ);
	    signal_get_ಮೌಲ್ಯ(ref, vp);
	    ಪ್ರತ್ಯಾಗಮ ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_ಮೌಲ್ಯ() >= rfp->lsb.get_ಮೌಲ್ಯ())
	    ? (rfp->msb.get_ಮೌಲ್ಯ() - rfp->lsb.get_ಮೌಲ್ಯ() + 1)
	    : (rfp->lsb.get_ಮೌಲ್ಯ() - rfp->msb.get_ಮೌಲ್ಯ() + 1);

      vvp_vector4_t val = vec4_from_vpi_ಮೌಲ್ಯ(vp, wid);

      ಯದಿ (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, wid);
	    rfp->ಗಂಟು->force_vec4(val, mask);
      } ಅನ್ಯಥಾ ಯದಿ (net_flag && !dynamic_cast<vvp_island_port*>(rfp->ಗಂಟು->fun)) {
	    rfp->ಗಂಟು->send_vec4(val, vthread_get_wt_context());
      } ಅನ್ಯಥಾ {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      ಪ್ರತ್ಯಾಗಮ ref;
}

vvp_vector4_t vec4_from_vpi_ಮೌಲ್ಯ(s_vpi_ಮೌಲ್ಯ*vp, unsigned wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      ವಿಕಲ್ಪ (vp->format) {

	  ಪಕ್ಷ vpiIntVal: {
		long vpi_val = vp->value.integer;
		ಪ್ರತಿ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t bit = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.set_bit(idx, bit);
		      vpi_val >>= 1;
		}
		ಭಙ್ಗ;
	  }

	  ಪಕ್ಷ vpiVectorVal:
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long aval = vp->value.vector[idx/32].aval;
		  unsigned long bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  ಪೂರ್ಣಾಙ್ಕ bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t bit = scalar_to_bit4(bitmask);
		  val.set_bit(idx, bit);
	    }
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiBinStrVal:
	    vpip_bin_str_to_vec4(val, vp->value.str);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiOctStrVal:
	    vpip_oct_str_to_vec4(val, vp->value.str);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiDecStrVal:
	    vpip_dec_str_to_vec4(val, vp->value.str);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiHexStrVal:
	    vpip_hex_str_to_vec4(val, vp->value.str);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiScalarVal:
	    val.set_bit(0, scalar_to_bit4(vp->value.scalar));
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (ಪೂರ್ಣಾಙ್ಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);

      }

      ಪ್ರತ್ಯಾಗಮ val;
}

ಪೂರ್ಣಾಙ್ಕ __vpiSignal::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ signal_get(code, ಏತತ್); }

int64_t __vpiSignal::vpi_get64(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ signal_get64(code, ಏತತ್); }

ವರ್ಣ* __vpiSignal::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ signal_get_str(code, ಏತತ್); }

ಶೂನ್ಯ __vpiSignal::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ signal_get_ಮೌಲ್ಯ(ಏತತ್, val); }

vpiHandle __vpiSignal::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags)
{ ಪ್ರತ್ಯಾಗಮ signal_put_ಮೌಲ್ಯ(ಏತತ್, val, flags); }

vpiHandle __vpiSignal::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ signal_get_handle(code, ಏತತ್); }

vpiHandle __vpiSignal::vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ signal_iterate(code, ಏತತ್); }

vpiHandle __vpiSignal::vpi_index(ಪೂರ್ಣಾಙ್ಕ idx)
{ ಪ್ರತ್ಯಾಗಮ signal_index(idx, ಏತತ್); }

ಸಂರಚನಾ signal_reg : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಅನ್ತಃಸ್ಥ signal_reg() { }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiReg; }
};

ಸಂರಚನಾ signal_integer : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಅನ್ತಃಸ್ಥ signal_integer() { }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiIntegerVar; }
};

ಸಂರಚನಾ signal_net : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಅನ್ತಃಸ್ಥ signal_net() { }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiNet; }
};

ಸಂರಚನಾ signal_byte : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಅನ್ತಃಸ್ಥ signal_byte() { }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiByteVar; }
};

ಸಂರಚನಾ signal_bitvar : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಅನ್ತಃಸ್ಥ signal_bitvar() { }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiBitVar; }
};

ಸಂರಚನಾ signal_shortint : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಅನ್ತಃಸ್ಥ signal_shortint() { }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiShortIntVar; }
};

ಸಂರಚನಾ signal_int : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಅನ್ತಃಸ್ಥ signal_int() { }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiIntVar; }
};

ಸಂರಚನಾ signal_longint : ಸಾರ್ವಜನಿಕ __vpiSignal {
      ಅನ್ತಃಸ್ಥ signal_longint() { }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(ಧ್ರುವ ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = ನೂತನ signal_integer;
      ಪ್ರತ್ಯಾಗಮ fill_in_var4(obj, name, msb, lsb, ಸತ್ಯ, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(ಧ್ರುವ ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb, ತಾರ್ಕಿಕ signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      ಯದಿ ((! signed_flag) || (lsb != 0) ) {
	    obj = ನೂತನ signal_bitvar;
      } ಅನ್ಯಥಾ {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    ವಿಕಲ್ಪ (msb) {
		ಪಕ್ಷ 7:
		  obj = ನೂತನ signal_byte;
		  ಭಙ್ಗ;
		ಪಕ್ಷ 15:
		  obj = ನೂತನ signal_shortint;
		  ಭಙ್ಗ;
		ಪಕ್ಷ 31:
		  obj = ನೂತನ signal_int;
		  ಭಙ್ಗ;
		ಪಕ್ಷ 63:
		  obj = ನೂತನ signal_longint;
		  ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = ನೂತನ signal_bitvar;
		  ಭಙ್ಗ;
	    }
      }

      ಪ್ರತ್ಯಾಗಮ fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(ಧ್ರುವ ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb,
			ತಾರ್ಕಿಕ signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = ನೂತನ signal_reg;
      ಪ್ರತ್ಯಾಗಮ fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

#ifdef CHECK_WITH_VALGRIND
ಸ್ಥಿರ ಸಂರಚನಾ vpiSignal_plug **signal_pool = 0;
ಸ್ಥಿರ unsigned signal_pool_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ unsigned long signal_ಎಣಿಕೆ = 0;
ಸ್ಥಿರ unsigned long signal_dels = 0;
#endif

ಸಂರಚನಾ vpiSignal_plug {
      unsigned ವರ್ಣ space[ಪರಿಮಾಣ (ಸಂರಚನಾ __vpiSignal)];
#ifdef CHECK_WITH_VALGRIND
      ಸಂರಚನಾ vpiSignal_plug *pool;
#endif
};

ಶೂನ್ಯ* __vpiSignal::operator ನೂತನ(size_t siz)
{
#ifdef CHECK_WITH_VALGRIND
      ಪ್ರತಿಜ್ಞಾ(siz == ಪರಿಮಾಣ(ಸಂರಚನಾ vpiSignal_plug) - ಪರಿಮಾಣ(ಸಂರಚನಾ vpiSignal_plug *));
#ಅನ್ಯಥಾ
      ಪ್ರತಿಜ್ಞಾ(siz == ಪರಿಮಾಣ(ಸಂರಚನಾ vpiSignal_plug));
#endif
      ಸ್ಥಿರ ಸಂರಚನಾ vpiSignal_plug*alloc_array = 0;
      ಸ್ಥಿರ unsigned alloc_index = 0;
      ಧ್ರುವ unsigned alloc_ಎಣಿಕೆ = 512;

      ಯದಿ ((alloc_array == 0) || (alloc_index == alloc_ಎಣಿಕೆ)) {
	    alloc_array = static_cast<ಸಂರಚನಾ vpiSignal_plug*>
	                  (calloc(alloc_ಎಣಿಕೆ, ಪರಿಮಾಣ(ಸಂರಚನಾ vpiSignal_plug)));
	    alloc_index = 0;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_ಎಣಿಕೆ *
	                                            ಪರಿಮಾಣ(ಸಂರಚನಾ vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_ಎಣಿಕೆ += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_ಎಣಿಕೆ*ಪರಿಮಾಣ(vpiSignal_plug **)));
	    signal_pool[signal_pool_ಎಣಿಕೆ-1] = alloc_array;
#endif
      }

      ಸಂರಚನಾ vpiSignal_plug*cur = alloc_array + alloc_index;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, ಪರಿಮಾಣ(ಸಂರಚನಾ vpiSignal_plug));
      cur->pool = alloc_array;
      signal_ಎಣಿಕೆ += 1;
#endif
      alloc_index += 1;
      ಪ್ರತ್ಯಾಗಮ cur;
}

ಶೂನ್ಯ __vpiSignal::operator delete(ಶೂನ್ಯ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ signal_delete(vpiHandle item)
{
      ಸಂರಚನಾ __vpiSignal *obj = static_cast<__vpiSignal *> (item);
      ಪ್ರತಿಜ್ಞಾ(obj->ಗಂಟು->fil);
      obj->ಗಂಟು->fil->clear_all_callbacks();
      vvp_net_delete(obj->ಗಂಟು);
      ಯದಿ (obj->bits) {
	    ಪ್ರತಿ (unsigned idx=0; idx<obj->width(); idx+=1) {
		  delete obj->bits[idx].index;
	    }
	    obj->bits -= 1;
	    delete [] obj->bits;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

ಶೂನ್ಯ signal_pool_delete()
{
      ಯದಿ (RUNNING_ON_VALGRIND && (signal_ಎಣಿಕೆ != signal_dels)) {
	    fflush(ರಿಕ್ತ);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (long) signal_ಎಣಿಕೆ - signal_dels, signal_ಎಣಿಕೆ);
      }

      ಪ್ರತಿ (unsigned idx = 0; idx < signal_pool_ಎಣಿಕೆ; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    ಮುಕ್ತ(signal_pool[idx]);
      }

      ಮುಕ್ತ(signal_pool);
      signal_pool = 0;
      signal_pool_ಎಣಿಕೆ = 0;
}
#endif

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
ಸ್ಥಿರ vpiHandle fill_in_net4(ಸಂರಚನಾ __vpiSignal*obj, __vpiScope*scope,
			      ಧ್ರುವ ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb,
			      ತಾರ್ಕಿಕ signed_flag, vvp_net_t*ಗಂಟು)
{
      obj->id.name = name? vpip_name_string(name) : ರಿಕ್ತ;
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

      ಪ್ರತ್ಯಾಗಮ obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 ಧ್ರುವ ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb,
			 ತಾರ್ಕಿಕ signed_flag, vvp_net_t*ಗಂಟು)
{
      ಸಂರಚನಾ __vpiSignal*obj = ನೂತನ signal_net;
      ಪ್ರತ್ಯಾಗಮ fill_in_net4(obj, scope, name, msb, lsb, signed_flag, ಗಂಟು);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ PV_get_base(ಸಂರಚನಾ __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      ಯದಿ (rfp->sbase != 0) {
	    s_vpi_ಮೌಲ್ಯ val;
	      /* Check to see if the value is defined. */
	    val.format = vpiVectorVal;
	    vpi_get_ಮೌಲ್ಯ(rfp->sbase, &val);
	    ಪೂರ್ಣಾಙ್ಕ ಪದಗಳು = (vpi_get(vpiSize, rfp->sbase)-1)/32 + 1;
	    ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ idx = 0; idx < ಪದಗಳು; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  ಯದಿ (val.value.vector[idx].bval != 0) ಪ್ರತ್ಯಾಗಮ INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_ಮೌಲ್ಯ(rfp->sbase, &val);
	    ಪ್ರತ್ಯಾಗಮ val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      ಪ್ರತ್ಯಾಗಮ rfp->tbase;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ PV_get(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಪೂರ್ಣಾಙ್ಕ rval = 0;
      ವಿಕಲ್ಪ (code) {
	ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ 0;  // Not implemented for now!

	ಪಕ್ಷ vpiSigned:
	    ಪ್ರತ್ಯಾಗಮ 0;  // A part/bit select is always unsigned!

	ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ rfp->width;

	  /* This is like the &A<> in array.cc. */
	ಪಕ್ಷ vpiConstantSelect:
	    ಪ್ರತ್ಯಾಗಮ rfp->sbase == 0;

	ಪಕ್ಷ vpi_ಎಡ_Range:
            rval += rfp->width - 1;
	    // fallthrough
	ಪಕ್ಷ vpi_ಬಲ_Range: {
	    ಪೂರ್ಣಾಙ್ಕ ಎಡ = vpi_get(vpi_ಎಡ_Range, rfp->parent);
	    ಪೂರ್ಣಾಙ್ಕ ಬಲ = vpi_get(vpi_ಬಲ_Range, rfp->parent);
	    rval += PV_get_base(rfp);
	    ಪ್ರತ್ಯಾಗಮ ಎಡ >= ಬಲ ? ಬಲ + rval : ಬಲ - rval;
	}

        ಪಕ್ಷ vpiAutomatic:
            ಪ್ರತ್ಯಾಗಮ vpi_get(vpiAutomatic, rfp->parent);

#ಯದಿ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        ಪಕ್ಷ _vpiFromThr:
            ಪ್ರತ್ಯಾಗಮ _vpi_at_PV;
#endif

	ಸ್ವಭಾವ:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ವರ್ಣ* PV_get_str(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ವಿಕಲ್ಪ (code) {
	ಪಕ್ಷ vpiFile:  // Not implemented for now!
	    ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[0]);

	ಪಕ್ಷ vpiName:
	ಪಕ್ಷ vpiFullName: {
	    ಧ್ರುವ ವರ್ಣ*nm = vpi_get_str(code, rfp->parent);
	    size_t len = 256+strlen(nm);
	    ವರ್ಣ *full = static_cast<ವರ್ಣ *>(malloc(len));
	    snprintf(full, len, "%s[%d:%d]", nm,
	                                     (ಪೂರ್ಣಾಙ್ಕ)vpi_get(vpi_ಎಡ_Range, ref),
	                                     (ಪೂರ್ಣಾಙ್ಕ)vpi_get(vpi_ಬಲ_Range, ref));
	    full[len-1] = 0;
	    ವರ್ಣ *res = simple_set_rbuf_str(full);
	    ಮುಕ್ತ(full);
	    ಪ್ರತ್ಯಾಗಮ res;
	}

	ಸ್ವಭಾವ:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ PV_get_ಮೌಲ್ಯ(vpiHandle ref, p_vpi_ಮೌಲ್ಯ vp)
{
      ಸಂರಚನಾ __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(rfp->net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);

      ವಿಕಲ್ಪ (vp->format) {

	  ಪಕ್ಷ vpiIntVal:
	    format_vpiIntVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiStringVal:
	    format_vpiStringVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_base(rfp), vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiVectorVal:
	    format_vpiVectorVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiRealVal:
	    format_vpiRealVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (ಪೂರ್ಣಾಙ್ಕ)vp->format, vpi_get_str(vpiFullName, rfp->parent));
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

ಸ್ಥಿರ vpiHandle PV_put_ಮೌಲ್ಯ(vpiHandle ref, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ flags)
{
      ಸಂರಚನಾ __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);
      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*>(rfp->net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);

      unsigned sig_size = sig->ಮೌಲ್ಯ_size();
      unsigned width = rfp->width;
      ಪೂರ್ಣಾಙ್ಕ base = PV_get_base(rfp);
      ಯದಿ (base >= (signed) sig_size) ಪ್ರತ್ಯಾಗಮ 0;
      ಯದಿ (base + (signed) width < 0) ಪ್ರತ್ಯಾಗಮ 0;

      vvp_vector4_t val;
      ಯದಿ (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_ಮೌಲ್ಯ(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      ಯದಿ (base < 0) {
	    width += base;
	    ಯದಿ (flags != vpiReleaseFlag) {
		  val = val.subvalue(-base, width);
	    }
	    base = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      ಯದಿ (base+width > sig_size) {
	    width = sig_size - base;
	    ಯದಿ (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      ಪ್ರತಿಜ್ಞಾ(rfp->parent);
      ತಾರ್ಕಿಕ net_flag = rfp->parent->get_type_code()==vpiNet;
      ತಾರ್ಕಿಕ full_sig = base == 0 && width == sig_size;

      vvp_net_ptr_t dest(rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value.  Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value.*/
      ಯದಿ (flags == vpiReleaseFlag) {
	    ಪ್ರತಿಜ್ಞಾ(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    ಯದಿ (full_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } ಅನ್ಯಥಾ {
		  rfp->net->fil->release_pv(dest, base, width, net_flag);
	    }
	    rfp->net->fun->force_flag(ಸತ್ಯ);
	    PV_get_ಮೌಲ್ಯ(ref, vp);
	    ಪ್ರತ್ಯಾಗಮ ref;
      }

      ಯದಿ (flags == vpiForceFlag) {
	    ಯದಿ (full_sig) {
		  vvp_vector2_t mask (vvp_vector2_t::FILL1, sig_size);
		  rfp->net->force_vec4(val, mask);
	    } ಅನ್ಯಥಾ {
		  vvp_vector2_t mask (vvp_vector2_t::FILL0, sig_size);
		  ಪ್ರತಿ (unsigned idx = 0 ; idx < width ; idx += 1)
		        mask.set_bit(base+idx, 1);

		  vvp_vector4_t tmp (sig_size, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_ಮೌಲ್ಯ(tmp);

		  tmp.set_vec(base, val);
		  rfp->net->force_vec4(tmp, mask);
	    }
      } ಅನ್ಯಥಾ ಯದಿ (net_flag && !dynamic_cast<vvp_island_port*>(rfp->net->fun)) {
	    ಯದಿ (full_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } ಅನ್ಯಥಾ {
		  rfp->net->send_vec4_pv(val, base, sig_size,
		                         vthread_get_wt_context());
	    }
      } ಅನ್ಯಥಾ {
	    ಯದಿ (full_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } ಅನ್ಯಥಾ {
		  vvp_send_vec4_pv(dest, val, base, sig_size,
	                           vthread_get_wt_context());
	    }
      }
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ vpiHandle PV_get_handle(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiParent:
	    ಪ್ರತ್ಯಾಗಮ rfp->parent;

	  ಪಕ್ಷ vpiScope:
	    ಪ್ರತ್ಯಾಗಮ vpi_handle(vpiScope, rfp->parent);

	  ಪಕ್ಷ vpiModule:
	    ಪ್ರತ್ಯಾಗಮ vpi_handle(vpiModule, rfp->parent);
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಅನ್ತಃಸ್ಥ __vpiPV::__vpiPV()
{ }

ಪೂರ್ಣಾಙ್ಕ __vpiPV::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiPartSelect; }

ಪೂರ್ಣಾಙ್ಕ __vpiPV::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ PV_get(code, ಏತತ್); }

ವರ್ಣ* __vpiPV::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ PV_get_str(code, ಏತತ್); }

ಶೂನ್ಯ __vpiPV::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ PV_get_ಮೌಲ್ಯ(ಏತತ್, val); }

vpiHandle __vpiPV::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags)
{ ಪ್ರತ್ಯಾಗಮ PV_put_ಮೌಲ್ಯ(ಏತತ್, val, flags); }

vpiHandle __vpiPV::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ PV_get_handle(code, ಏತತ್); }

vpiHandle vpip_make_PV(ವರ್ಣ*var, ಪೂರ್ಣಾಙ್ಕ base, ಪೂರ್ಣಾಙ್ಕ width)
{
      ಸಂರಚನಾ __vpiPV*obj = ನೂತನ __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = 0;
      obj->tbase = base;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ಪ್ರತ್ಯಾಗಮ obj;
}

vpiHandle vpip_make_PV(ವರ್ಣ*var, ವರ್ಣ*ಚಿಹ್ನೆ, ಪೂರ್ಣಾಙ್ಕ width)
{
      ಸಂರಚನಾ __vpiPV*obj = ನೂತನ __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      compile_vpi_lookup(&obj->sbase, ಚಿಹ್ನೆ);
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ಪ್ರತ್ಯಾಗಮ obj;
}

vpiHandle vpip_make_PV(ವರ್ಣ*var, vpiHandle handle, ಪೂರ್ಣಾಙ್ಕ width)
{
      ಸಂರಚನಾ __vpiPV*obj = ನೂತನ __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ಪ್ರತ್ಯಾಗಮ obj;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ PV_delete(vpiHandle item)
{
      ಸಂರಚನಾ __vpiPV *obj = dynamic_cast<__vpiPV*>(item);
      ಯದಿ (obj->sbase) {
	    ವಿಕಲ್ಪ (obj->sbase->get_type_code()) {
		ಪಕ್ಷ vpiMemoryWord:
		  ಯದಿ (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiPartSelect:
		  ಪ್ರತಿಜ್ಞಾ(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  ಭಙ್ಗ;
	    }
      }
      ಪ್ರತಿಜ್ಞಾ(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      delete obj;
}
#endif
