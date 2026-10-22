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
ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର hex_digits[256];
ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
ଶୂନ୍ୟ *need_result_buf(unsigned cnt, vpi_rbuf_t type)
{
      ସ୍ଥିର ଶୂନ୍ୟ*result_buf[2] = {0, 0};
      ସ୍ଥିର size_t result_buf_size[2] = {0, 0};

      ଯଦି (type == RBUF_DEL) {
	    ମୁକ୍ତ(result_buf[RBUF_VAL]);
	    result_buf[RBUF_VAL] = 0;
	    result_buf_size[RBUF_VAL] = 0;

	    ମୁକ୍ତ(result_buf[RBUF_STR]);
	    result_buf[RBUF_STR] = 0;
	    result_buf_size[RBUF_STR] = 0;

	    ଫେରନ୍ତୁ 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      ଯଦି (result_buf_size[type] == 0) {
	    result_buf[type] = malloc(cnt);
	    result_buf_size[type] = cnt;
      } ଅନ୍ୟଥା ଯଦି (result_buf_size[type] < cnt) {
	    result_buf[type] = realloc(result_buf[type], cnt);
	    result_buf_size[type] = cnt;
      }

      ଫେରନ୍ତୁ result_buf[type];
}

ଅକ୍ଷର *simple_set_rbuf_str(ଅଚଳ ଅକ୍ଷର *s1)
{
      ଅକ୍ଷର *res = static_cast<ଅକ୍ଷର *>(need_result_buf(strlen(s1)+1, RBUF_STR));
      ଯଦି (res) strcpy(res,s1);
      ଫେରନ୍ତୁ res;
}

ଅକ୍ଷର *generic_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref, ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *index)
{
      size_t len = strlen(name) + 1;  /* include space for null termination */
      ଅକ୍ଷର *bn = ଖାଲି;
      ତାର୍କିକ is_bn_a_pkg = ମିଥ୍ୟା;
      ଯଦି (code == vpiFullName) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = strlen(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    ଯଦି (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      ଯଦି (index != ଖାଲି) len += strlen(index) + 2;  /* include space for brackets */

      ଅକ୍ଷର *res = static_cast<ଅକ୍ଷର *>(need_result_buf(len, RBUF_STR));
      ଯଦି (!res) {
	    ମୁକ୍ତ(bn);
	    ଫେରନ୍ତୁ ଖାଲି;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      ଯଦି (bn != ଖାଲି) {
	    strcat(res, bn);
	      // A package already has the "::" separator in the name
	    ଯଦି (! is_bn_a_pkg) strcat(res, ".");
	    ମୁକ୍ତ(bn);
      }
      strcat(res, name);
      ଯଦି (index != ଖାଲି) {
	    strcat(res, "[");
	    strcat(res, index);
	    strcat(res, "]");
      }
      ଫେରନ୍ତୁ res;
}

ସ୍ଥିର vpiHandle fill_in_net4(ସଂରଚନା __vpiSignal*obj, __vpiScope*scope,
                              ଅଚଳ ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb,
                              ତାର୍କିକ signed_flag, vvp_net_t*ଗଣ୍ଠି);

ସ୍ଥିର vpiHandle fill_in_var4(ସଂରଚନା __vpiSignal*obj,
                              ଅଚଳ ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb,
                              ତାର୍କିକ signed_flag, vvp_net_t*ଗଣ୍ଠି)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      ଫେରନ୍ତୁ fill_in_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, ଗଣ୍ଠି);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

ସ୍ଥିର ଶୂନ୍ୟ format_vpiBinStrVal(ଅଚଳ vvp_signal_ମୂଲ୍ୟ*sig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base, unsigned wid,
                                s_vpi_ମୂଲ୍ୟ*vp)
{
      ଅକ୍ଷର *rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(wid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long offset = end - 1;
      long ssize = (signed)sig->ମୂଲ୍ୟ_size();

      ପାଇଁ (long idx = base ;  idx < end ;  idx += 1) {
	    ଯଦି (idx < 0 || idx >= ssize) {
                  rbuf[offset-idx] = 'x';
	    } ଅନ୍ୟଥା {
                  rbuf[offset-idx] = vvp_bit4_to_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

ସ୍ଥିର ଶୂନ୍ୟ format_vpiOctStrVal(ଅଚଳ vvp_signal_ମୂଲ୍ୟ*sig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base, unsigned wid,
                                s_vpi_ମୂଲ୍ୟ*vp)
{
      unsigned dwid = (wid + 2) / 3;
      ଅକ୍ଷର *rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->ମୂଲ୍ୟ_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      ପାଇଁ (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    ଯଦି (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } ଅନ୍ୟଥା {
                  ବାଛ (sig->value(idx)) {
		      ସ୍ଥିତି BIT4_0:
			bit = 0;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି BIT4_1:
			bit = 1;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି BIT4_X:
			bit = 2;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି BIT4_Z:
			bit = 3;
			ଭାଙ୍ଗ;
                  }
	    }
	    val |= bit << 2*((idx-base) % 3);

	    ଯଦି ((idx-base) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ବାଛ (wid % 3) {
	  ସ୍ଥିତି 1:
	    ଯଦି (val == 2) val = 42;
	    ଅନ୍ୟଥା ଯଦି (val == 3) val = 63;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 2:
	    ଯଦି (val == 10) val = 42;
	    ଅନ୍ୟଥା ଯଦି (val == 15) val = 63;
	    ଭାଙ୍ଗ;
      }

      ଯଦି (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

ସ୍ଥିର ଶୂନ୍ୟ format_vpiHexStrVal(ଅଚଳ vvp_signal_ମୂଲ୍ୟ*sig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base, unsigned wid,
                                s_vpi_ମୂଲ୍ୟ*vp)
{
      unsigned dwid = (wid + 3) / 4;
      ଅକ୍ଷର *rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->ମୂଲ୍ୟ_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      ପାଇଁ (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    ଯଦି (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } ଅନ୍ୟଥା {
                  ବାଛ (sig->value(idx)) {
		      ସ୍ଥିତି BIT4_0:
			bit = 0;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି BIT4_1:
			bit = 1;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି BIT4_X:
			bit = 2;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି BIT4_Z:
			bit = 3;
			ଭାଙ୍ଗ;
                  }
	    }
	    val |= bit << 2*((idx-base) % 4);

	    ଯଦି ((idx-base) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ବାଛ (wid % 4) {
	  ସ୍ଥିତି 1:
	    ଯଦି (val == 2) val = 170;
	    ଅନ୍ୟଥା ଯଦି (val == 3) val = 255;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 2:
	    ଯଦି (val == 10) val = 170;
	    ଅନ୍ୟଥା ଯଦି (val == 15) val = 255;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 3:
	    ଯଦି (val == 42) val = 170;
	    ଅନ୍ୟଥା ଯଦି (val == 63) val = 255;
	    ଭାଙ୍ଗ;
      }

      ଯଦି (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

ସ୍ଥିର ଶୂନ୍ୟ format_vpiDecStrVal(ଅଚଳ vvp_signal_ମୂଲ୍ୟ*sig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base, unsigned wid,
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା signed_flag, s_vpi_ମୂଲ୍ୟ*vp)
{
      unsigned hwid = (sig->ମୂଲ୍ୟ_size()+2) / 3 + 1;
      ଅକ୍ଷର *rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(hwid, RBUF_VAL));
      long ssize = (signed)sig->ମୂଲ୍ୟ_size();
      long end = base + (signed)wid;

	/* Do we have an end outside of the real signal vector. */
      ଯଦି (base < 0 || end > ssize) {
	    ତାର୍କିକ all_x = ସତ୍ୟ;
	    ଯଦି (end > ssize) end = ssize;
	    ଯଦି (base < 0) base = 0;
	    ପାଇଁ (long idx = base ;  idx < end ;  idx += 1) {
		  ଯଦି (sig->value(idx) != BIT4_X) {
			all_x = ମିଥ୍ୟା;
			ଭାଙ୍ଗ;
		  }
	    }

	    ଯଦି (all_x) {
		  rbuf[0] = 'x';
	    } ଅନ୍ୟଥା {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    ଫେରନ୍ତୁ;
      }

      vvp_vector4_t vec4;
      ଯଦି (base == 0 && end == ssize) {
	    sig->vec4_ମୂଲ୍ୟ(vec4);
      } ଅନ୍ୟଥା {
	    vvp_vector4_t tmp;
	    sig->vec4_ମୂଲ୍ୟ(tmp);
	    vec4 = tmp.subvalue(base, wid);
      }

      vpip_vec4_to_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

ସ୍ଥିର ଶୂନ୍ୟ format_vpiIntVal(ଅଚଳ vvp_signal_ମୂଲ୍ୟ*sig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base, unsigned wid,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା signed_flag, s_vpi_ମୂଲ୍ୟ*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_ମୂଲ୍ୟ(tmp);
      vvp_vector4_t sub = tmp.subvalue(base, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      ଯଦି (ଆକାର(vp->value.integer) == ଆକାର(int32_t)) {
	    int32_t val = 0;
	    vector4_to_ମୂଲ୍ୟ(sub, val, signed_flag, ମିଥ୍ୟା);
	    vp->value.integer = val;
      } ଅନ୍ୟଥା {
	    ପ୍ରତିଜ୍ଞା(ଆକାର(vp->value.integer) == ଆକାର(int64_t));
	    int64_t val = 0;
	    vector4_to_ମୂଲ୍ୟ(sub, val, signed_flag, ମିଥ୍ୟା);
	    vp->value.integer = val;
      }
}

ସ୍ଥିର ଶୂନ୍ୟ format_vpiRealVal(ଅଚଳ vvp_signal_ମୂଲ୍ୟ*sig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base, unsigned wid,
                              ପୂର୍ଣ୍ଣସଂଖ୍ୟା signed_flag, s_vpi_ମୂଲ୍ୟ*vp)
{
      vvp_vector4_t vec4(wid);
      long ssize = (signed)sig->ମୂଲ୍ୟ_size();
      long end = base + (signed)wid;
      ଯଦି (end > ssize) end = ssize;

      ପାଇଁ (long idx = (base < 0) ? 0 : base ;  idx < end ;  idx += 1) {
	    vec4.set_bit(idx-base, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_to_ମୂଲ୍ୟ(vec4, vp->value.real, signed_flag);
}

ସ୍ଥିର ଶୂନ୍ୟ format_vpiStringVal(ଅଚଳ vvp_signal_ମୂଲ୍ୟ*sig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base, unsigned wid,
                                s_vpi_ମୂଲ୍ୟ*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      ଅକ୍ଷର *rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      ଅକ୍ଷର *cp = rbuf;

      ଅକ୍ଷର tmp = 0;
      ପାଇଁ (long idx = base+(signed)wid-1; idx >= base; idx -= 1) {
	    tmp <<= 1;

	    ଯଦି (idx >=0 && idx < (signed)sig->ମୂଲ୍ୟ_size() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    ଯଦି (((idx-base)&7)==0){
		  /* Skip leading nulls. */
		  ଯଦି (tmp == 0 && cp == rbuf)
			ଜାରି;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

ସ୍ଥିର ଶୂନ୍ୟ format_vpiScalarVal(ଅଚଳ vvp_signal_ମୂଲ୍ୟ*sig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base,
                                s_vpi_ମୂଲ୍ୟ*vp)
{
      ଯଦି (base >= 0 && base < (signed)sig->ମୂଲ୍ୟ_size()) {
	    ବାଛ (sig->value(base)) {
		ସ୍ଥିତି BIT4_0:
		  vp->value.scalar = vpi0;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_1:
		  vp->value.scalar = vpi1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_ମୂଲ୍ୟ(base);
		  ଯଦି (strn.strength0() == 1) vp->value.scalar = vpiH;
		  ଅନ୍ୟଥା ଯଦି (strn.strength1() == 1) vp->value.scalar = vpiL;
		  ଅନ୍ୟଥା vp->value.scalar = vpiX;
		  ଭାଙ୍ଗ;
		}
		ସ୍ଥିତି BIT4_Z:
		  vp->value.scalar = vpiZ;
		  ଭାଙ୍ଗ;
	    }
      } ଅନ୍ୟଥା {
	    vp->value.scalar = vpiX;
      }
}

ସ୍ଥିର ଶୂନ୍ୟ format_vpiStrengthVal(ଅଚଳ vvp_signal_ମୂଲ୍ୟ*sig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base,
                                  unsigned wid, s_vpi_ମୂଲ୍ୟ*vp)
{
      long end = base + (signed)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_result_buf(wid * ଆକାର(s_vpi_strengthval), RBUF_VAL));

      ପାଇଁ (long idx = base ;  idx < end ;  idx += 1) {
	    ଯଦି (idx >=0 && idx < (signed)sig->ମୂଲ୍ୟ_size()) {
		  vvp_scalar_t val = sig->scalar_ମୂଲ୍ୟ(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  unsigned s0 = 1 << val.strength0();
		  unsigned s1 = 1 << val.strength1();

		  ବାଛ (val.value()) {
		      ସ୍ଥିତି BIT4_0:
			op[idx-base].logic = vpi0;
			op[idx-base].s0 = s0|s1;
			op[idx-base].s1 = 0;
			ଭାଙ୍ଗ;

		      ସ୍ଥିତି BIT4_1:
			op[idx-base].logic = vpi1;
			op[idx-base].s0 = 0;
			op[idx-base].s1 = s0|s1;
			ଭାଙ୍ଗ;

		      ସ୍ଥିତି BIT4_X:
			op[idx-base].logic = vpiX;
			op[idx-base].s0 = s0;
			op[idx-base].s1 = s1;
			ଭାଙ୍ଗ;

		      ସ୍ଥିତି BIT4_Z:
			op[idx-base].logic = vpiZ;
			op[idx-base].s0 = vpiHiZ;
			op[idx-base].s1 = vpiHiZ;
			ଭାଙ୍ଗ;
		  }
	    } ଅନ୍ୟଥା {
		  op[idx-base].logic = vpiX;
		  op[idx-base].s0 = vpiStrongDrive;
		  op[idx-base].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

ସ୍ଥିର ଶୂନ୍ୟ format_vpiVectorVal(ଅଚଳ vvp_signal_ମୂଲ୍ୟ*sig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base, unsigned wid,
                                s_vpi_ମୂଲ୍ୟ*vp)
{
      long end = base + (signed)wid;
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା obit = 0;
      unsigned hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_result_buf(hwid * ଆକାର(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      ପାଇଁ (long idx = base ;  idx < end ;  idx += 1) {
	    ଯଦି (base >= 0 && base < (signed)sig->ମୂଲ୍ୟ_size()) {
		ବାଛ (sig->value(idx)) {
		ସ୍ଥିତି BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  ଭାଙ୍ଗ;
		}
	    } ଅନ୍ୟଥା {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    ଯଦି (!(obit % 32)) {
		  op += 1;
		  ଯଦି ((op - vp->value.vector) < (ptrdiff_t)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା signal_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ବାଛ (code) {
	  ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ 0;  // Not implemented for now!

	  ସ୍ଥିତି vpiSigned:
	    ଫେରନ୍ତୁ rfp->signed_flag != 0;

	  ସ୍ଥିତି vpiArray:
	    ଫେରନ୍ତୁ rfp->is_netarray != 0;

	  ସ୍ଥିତି vpiIndex: // This only works while we have a single index.
	    ଯଦି (rfp->is_netarray) {
		  s_vpi_ମୂଲ୍ୟ vp;
		  vp.format = vpiIntVal;
		  vpi_get_ମୂଲ୍ୟ(rfp->id.index, &vp);
		  ଫେରନ୍ତୁ vp.value.integer;
	    } ଅନ୍ୟଥା {
		  ଫେରନ୍ତୁ vpiUndefined;
	    }

	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ rfp->width();

	  ସ୍ଥିତି vpiNetType:
	    ଯଦି (ref->get_type_code()==vpiNet)
		  ଫେରନ୍ତୁ vpiWire;
	    ଅନ୍ୟଥା
		  ଫେରନ୍ତୁ vpiUndefined;

	  ସ୍ଥିତି vpi_ବାମ_Range:
            ଫେରନ୍ତୁ rfp->msb.get_ମୂଲ୍ୟ();

	  ସ୍ଥିତି vpi_ଡାହାଣ_Range:
            ଫେରନ୍ତୁ rfp->lsb.get_ମୂଲ୍ୟ();

	  ସ୍ଥିତି vpiScalar:
	    ଫେରନ୍ତୁ (rfp->msb.get_ମୂଲ୍ୟ() == 0 && rfp->lsb.get_ମୂଲ୍ୟ() == 0);
	  ସ୍ଥିତି vpiVector:
	    ଫେରନ୍ତୁ (rfp->msb.get_ମୂଲ୍ୟ() != rfp->lsb.get_ମୂଲ୍ୟ());

          ସ୍ଥିତି vpiAutomatic:
            ଫେରନ୍ତୁ vpip_scope(rfp)->is_automatic() ? 1  : 0;

#ifdef BR916_STOPGAP_FIX
          ସ୍ଥିତି _vpiFromThr:
            ଫେରନ୍ତୁ _vpiNoThr;
#endif

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    ଫେରନ୍ତୁ vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
ସ୍ଥିର int64_t signal_get64(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ବାଛ (code) {
	    // This private property must return zero when undefined.
	  ସ୍ଥିତି _vpiNexusId:
	    ଯଦି (rfp->msb.get_ମୂଲ୍ୟ() == rfp->lsb.get_ମୂଲ୍ୟ())
		  ଫେରନ୍ତୁ reinterpret_cast<int64_t>(rfp->ଗଣ୍ଠି);
	    ଫେରନ୍ତୁ 0;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    ଫେରନ୍ତୁ vpiUndefined;
      }
}

ସ୍ଥିର ଅକ୍ଷର* signal_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ଯଦି (code == vpiFile) {  // Not implemented for now!
	    ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[0]);
      }

      ଯଦି ((code != vpiName) && (code != vpiFullName)) ଫେରନ୍ତୁ ଖାଲି;

      ଅକ୍ଷର *nm;
      ଅଚଳ ଅକ୍ଷର *ixs;
      ଯଦି (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_ମୂଲ୍ୟ vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_ମୂଲ୍ୟ(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } ଅନ୍ୟଥା {
	    nm = strdup(rfp->id.name);
	    ixs = ଖାଲି;
      }

	/* The scope information is added here for vpiFullName. */
      ଅକ୍ଷର *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      ମୁକ୍ତ(nm);
      ଫେରନ୍ତୁ rbuf;
}

ସ୍ଥିର vpiHandle signal_get_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ବାଛ (code) {

	  ସ୍ଥିତି vpiParent:
	    ଫେରନ୍ତୁ rfp->is_netarray? rfp->within.parent : ଖାଲି;

	  ସ୍ଥିତି vpiIndex:
	    ଫେରନ୍ତୁ rfp->is_netarray? rfp->id.index : ଖାଲି;

	  ସ୍ଥିତି vpi_ବାମ_Range:
	    ଫେରନ୍ତୁ &rfp->msb;
	  ସ୍ଥିତି vpi_ଡାହାଣ_Range:
	    ଫେରନ୍ତୁ &rfp->lsb;

	  ସ୍ଥିତି vpiScope:
	    ଫେରନ୍ତୁ vpip_scope(rfp);

	  ସ୍ଥିତି vpiModule:
	    ଫେରନ୍ତୁ vpip_module(vpip_scope(rfp));
      }

      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର vpiHandle signal_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ଯଦି (code == vpiIndex) {
	    ଫେରନ୍ତୁ rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : ଖାଲି;
      }

      ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ __vpiSignal::make_bits()
{
      ପ୍ରତିଜ୍ଞା(bits == ଖାଲି);
      bits = ନୂତନ ସଂରଚନା __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      bits->parent = ଏହି;
	// Now point to word[0].
      bits += 1;

	// Update each bit to point to the base
      ସଂରଚନା __vpiBit*base = bits;
      ପାଇଁ (unsigned idx = 0; idx < width(); idx += 1) {
	    base[idx].bit0 = base;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା real_idx;
	    ଯଦି (msb.get_ମୂଲ୍ୟ() >= lsb.get_ମୂଲ୍ୟ()) {
		  real_idx = idx + lsb.get_ମୂଲ୍ୟ();
	    } ଅନ୍ୟଥା {
		  real_idx = lsb.get_ମୂଲ୍ୟ() - idx;
	    }
	    base[idx].index = ନୂତନ __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
	/* Check to see if the index is in range. */
      ଯଦି (msb.get_ମୂଲ୍ୟ() >= lsb.get_ମୂଲ୍ୟ()) {
	    ଯଦି ((idx > msb.get_ମୂଲ୍ୟ()) || (idx < lsb.get_ମୂଲ୍ୟ())) ଫେରନ୍ତୁ 0;
      } ଅନ୍ୟଥା {
	    ଯଦି ((idx < msb.get_ମୂଲ୍ୟ()) || (idx > lsb.get_ମୂଲ୍ୟ())) ଫେରନ୍ତୁ 0;
      }

	/* Normalize the index */
      unsigned norm_idx;
      ଯଦି (msb.get_ମୂଲ୍ୟ() >= lsb.get_ମୂଲ୍ୟ()) {
	    norm_idx = idx - lsb.get_ମୂଲ୍ୟ();
      } ଅନ୍ୟଥା {
	    norm_idx = lsb.get_ମୂଲ୍ୟ() - idx;
      }

      ଯଦି (bits == ଖାଲି) make_bits();

      ଫେରନ୍ତୁ &(bits[norm_idx].as_bit);
}

ଶୂନ୍ୟ __vpiSignal::get_bit_ମୂଲ୍ୟ(ଅଚଳ ସଂରଚନା __vpiBit*bit, p_vpi_ମୂଲ୍ୟ vp)
{
      unsigned index = bit->get_norm_index();

      vvp_signal_ମୂଲ୍ୟ*vsig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*>(ଗଣ୍ଠି->fil);
      ପ୍ରତିଜ୍ଞା(vsig);

      ଯଦି (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      ବାଛ (vp->format) {
	ସ୍ଥିତି vpiBinStrVal:
            format_vpiBinStrVal(vsig, index, 1, vp);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି vpiOctStrVal:
            format_vpiOctStrVal(vsig, index, 1, vp);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି vpiDecStrVal:
            format_vpiDecStrVal(vsig, index, 1, ମିଥ୍ୟା, vp);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି vpiHexStrVal:
            format_vpiHexStrVal(vsig, index, 1, vp);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି vpiStringVal:
            format_vpiStringVal(vsig, index, 1, vp);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି vpiIntVal:
            format_vpiIntVal(vsig, index, 1, ମିଥ୍ୟା, vp);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି vpiRealVal:
            format_vpiRealVal(vsig, index, 1, ମିଥ୍ୟା, vp);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି vpiScalarVal:
            format_vpiScalarVal(vsig, index, vp);
	    ଭାଙ୍ଗ;

        ସ୍ଥିତି vpiStrengthVal:
            format_vpiStrengthVal(vsig, index, 1, vp);
            ଭାଙ୍ଗ;

	ସ୍ଥିତି vpiVectorVal:
            format_vpiVectorVal(vsig, index, 1, vp);
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format, vpi_get_str(vpiName),
		    bit->get_index(),
	            vpip_scope(ଏହି)->scope_name());
	    ପ୍ରତିଜ୍ଞା(0);
      };
}

vpiHandle __vpiSignal::put_bit_ମୂଲ୍ୟ(ସଂରଚନା __vpiBit*bit, p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
      unsigned index = bit->get_norm_index();
      vvp_net_ptr_t dest(ଗଣ୍ଠି, 0);
      vvp_vector4_t val = vec4_from_vpi_ମୂଲ୍ୟ(vp, 1);

      ଯଦି ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    bit->as_bit.vpi_get_str(vpiFullName));
	    ଫେରନ୍ତୁ ଖାଲି;
      }

      ଯଦି ((get_type_code() == vpiNet) &&
          !dynamic_cast<vvp_island_port*>(ଗଣ୍ଠି->fun)) {
	    ଗଣ୍ଠି->send_vec4_pv(val, index, width(),
	                            vthread_get_wt_context());
      } ଅନ୍ୟଥା {
	    vvp_send_vec4_pv(dest, val, index, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର vpiHandle signal_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, vpiHandle ref)
{
      ସଂରଚନା __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      ଯଦି ((type != vpiNet) && (type != vpiReg)) ଫେରନ୍ତୁ 0;

      ଫେରନ୍ତୁ rfp->get_index(idx);
}

unsigned __vpiSignal::width(ଶୂନ୍ୟ) ଅଚଳ
{
      unsigned wid = (msb.get_ମୂଲ୍ୟ() >= lsb.get_ମୂଲ୍ୟ())
	    ? (msb.get_ମୂଲ୍ୟ() - lsb.get_ମୂଲ୍ୟ() + 1)
	    : (lsb.get_ମୂଲ୍ୟ() - msb.get_ମୂଲ୍ୟ() + 1);

      ଫେରନ୍ତୁ wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
ସ୍ଥିର ଶୂନ୍ୟ signal_get_ମୂଲ୍ୟ(vpiHandle ref, s_vpi_ମୂଲ୍ୟ*vp)
{
      ସଂରଚନା __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      unsigned wid = rfp->width();

      vvp_signal_ମୂଲ୍ୟ*vsig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*>(rfp->ଗଣ୍ଠି->fil);
      ପ୍ରତିଜ୍ଞା(vsig);

      ବାଛ (vp->format) {

	  ସ୍ଥିତି vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiVectorVal:
	    format_vpiVectorVal(vsig, 0, wid, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiObjTypeVal:
	    ଯଦି (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } ଅନ୍ୟଥା {
		  vp->format = vpiVectorVal;
		  format_vpiVectorVal(vsig, 0, wid, vp);
	    }
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format, vpi_get_str(vpiName, ref),
		    vpip_scope(rfp)->scope_name());
	    ପ୍ରତିଜ୍ଞା(0);
      }
}

/*
 * The put_value method writes the value into the vector, and returns
 * the affected ref. This operation works much like the %set or
 * %assign instructions and causes all the side-effects that the
 * equivalent instruction would cause.
 */

ସ୍ଥିର vvp_vector4_t from_stringval(ଅଚଳ ଅକ୍ଷର*str, unsigned wid)
{
      unsigned idx;
      ଅଚଳ ଅକ୍ଷର*cp;

      cp = str + strlen(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      ଯେତେବେଳେ ((idx < wid) && (cp > str)) {
	    unsigned byte = *--cp;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bit;

	    ପାଇଁ (bit = 0 ;  (bit < 8) && (idx < wid) ;  bit += 1) {
		  ଯଦି (byte & 1)
			val.set_bit(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      ଫେରନ୍ତୁ val;
}

ସ୍ଥିର vpiHandle signal_put_ମୂଲ୍ୟ(vpiHandle ref, s_vpi_ମୂଲ୍ୟ*vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
      unsigned wid;
      ସଂରଚନା __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);
      vvp_net_ptr_t dest(rfp->ଗଣ୍ଠି, 0);

      ତାର୍କିକ net_flag = ref->get_type_code()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      ଯଦି (flags == vpiReleaseFlag) {
	    ପ୍ରତିଜ୍ଞା(rfp->ଗଣ୍ଠି->fil);
	    rfp->ଗଣ୍ଠି->fil->force_unlink();
	    rfp->ଗଣ୍ଠି->fil->release(dest, net_flag);
	    rfp->ଗଣ୍ଠି->fun->force_flag(ସତ୍ୟ);
	    signal_get_ମୂଲ୍ୟ(ref, vp);
	    ଫେରନ୍ତୁ ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_ମୂଲ୍ୟ() >= rfp->lsb.get_ମୂଲ୍ୟ())
	    ? (rfp->msb.get_ମୂଲ୍ୟ() - rfp->lsb.get_ମୂଲ୍ୟ() + 1)
	    : (rfp->lsb.get_ମୂଲ୍ୟ() - rfp->msb.get_ମୂଲ୍ୟ() + 1);

      vvp_vector4_t val = vec4_from_vpi_ମୂଲ୍ୟ(vp, wid);

      ଯଦି (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, wid);
	    rfp->ଗଣ୍ଠି->force_vec4(val, mask);
      } ଅନ୍ୟଥା ଯଦି (net_flag && !dynamic_cast<vvp_island_port*>(rfp->ଗଣ୍ଠି->fun)) {
	    rfp->ଗଣ୍ଠି->send_vec4(val, vthread_get_wt_context());
      } ଅନ୍ୟଥା {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      ଫେରନ୍ତୁ ref;
}

vvp_vector4_t vec4_from_vpi_ମୂଲ୍ୟ(s_vpi_ମୂଲ୍ୟ*vp, unsigned wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      ବାଛ (vp->format) {

	  ସ୍ଥିତି vpiIntVal: {
		long vpi_val = vp->value.integer;
		ପାଇଁ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t bit = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.set_bit(idx, bit);
		      vpi_val >>= 1;
		}
		ଭାଙ୍ଗ;
	  }

	  ସ୍ଥିତି vpiVectorVal:
	    ପାଇଁ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long aval = vp->value.vector[idx/32].aval;
		  unsigned long bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  ପୂର୍ଣ୍ଣସଂଖ୍ୟା bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t bit = scalar_to_bit4(bitmask);
		  val.set_bit(idx, bit);
	    }
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiBinStrVal:
	    vpip_bin_str_to_vec4(val, vp->value.str);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiOctStrVal:
	    vpip_oct_str_to_vec4(val, vp->value.str);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiDecStrVal:
	    vpip_dec_str_to_vec4(val, vp->value.str);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiHexStrVal:
	    vpip_hex_str_to_vec4(val, vp->value.str);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiScalarVal:
	    val.set_bit(0, scalar_to_bit4(vp->value.scalar));
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
	    ପ୍ରତିଜ୍ଞା(0);

      }

      ଫେରନ୍ତୁ val;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiSignal::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ signal_get(code, ଏହି); }

int64_t __vpiSignal::vpi_get64(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ signal_get64(code, ଏହି); }

ଅକ୍ଷର* __vpiSignal::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ signal_get_str(code, ଏହି); }

ଶୂନ୍ୟ __vpiSignal::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{ signal_get_ମୂଲ୍ୟ(ଏହି, val); }

vpiHandle __vpiSignal::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{ ଫେରନ୍ତୁ signal_put_ମୂଲ୍ୟ(ଏହି, val, flags); }

vpiHandle __vpiSignal::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ signal_get_handle(code, ଏହି); }

vpiHandle __vpiSignal::vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ signal_iterate(code, ଏହି); }

vpiHandle __vpiSignal::vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{ ଫେରନ୍ତୁ signal_index(idx, ଏହି); }

ସଂରଚନା signal_reg : ସର୍ବସାଧାରଣ __vpiSignal {
      ଧାଡ଼ିଭିତରେ signal_reg() { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiReg; }
};

ସଂରଚନା signal_integer : ସର୍ବସାଧାରଣ __vpiSignal {
      ଧାଡ଼ିଭିତରେ signal_integer() { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiIntegerVar; }
};

ସଂରଚନା signal_net : ସର୍ବସାଧାରଣ __vpiSignal {
      ଧାଡ଼ିଭିତରେ signal_net() { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiNet; }
};

ସଂରଚନା signal_byte : ସର୍ବସାଧାରଣ __vpiSignal {
      ଧାଡ଼ିଭିତରେ signal_byte() { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiByteVar; }
};

ସଂରଚନା signal_bitvar : ସର୍ବସାଧାରଣ __vpiSignal {
      ଧାଡ଼ିଭିତରେ signal_bitvar() { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiBitVar; }
};

ସଂରଚନା signal_shortint : ସର୍ବସାଧାରଣ __vpiSignal {
      ଧାଡ଼ିଭିତରେ signal_shortint() { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiShortIntVar; }
};

ସଂରଚନା signal_int : ସର୍ବସାଧାରଣ __vpiSignal {
      ଧାଡ଼ିଭିତରେ signal_int() { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiIntVar; }
};

ସଂରଚନା signal_longint : ସର୍ବସାଧାରଣ __vpiSignal {
      ଧାଡ଼ିଭିତରେ signal_longint() { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(ଅଚଳ ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = ନୂତନ signal_integer;
      ଫେରନ୍ତୁ fill_in_var4(obj, name, msb, lsb, ସତ୍ୟ, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(ଅଚଳ ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb, ତାର୍କିକ signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      ଯଦି ((! signed_flag) || (lsb != 0) ) {
	    obj = ନୂତନ signal_bitvar;
      } ଅନ୍ୟଥା {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    ବାଛ (msb) {
		ସ୍ଥିତି 7:
		  obj = ନୂତନ signal_byte;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 15:
		  obj = ନୂତନ signal_shortint;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 31:
		  obj = ନୂତନ signal_int;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 63:
		  obj = ନୂତନ signal_longint;
		  ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = ନୂତନ signal_bitvar;
		  ଭାଙ୍ଗ;
	    }
      }

      ଫେରନ୍ତୁ fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(ଅଚଳ ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb,
			ତାର୍କିକ signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = ନୂତନ signal_reg;
      ଫେରନ୍ତୁ fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

#ifdef CHECK_WITH_VALGRIND
ସ୍ଥିର ସଂରଚନା vpiSignal_plug **signal_pool = 0;
ସ୍ଥିର unsigned signal_pool_ଗଣନା = 0;
ସ୍ଥିର unsigned long signal_ଗଣନା = 0;
ସ୍ଥିର unsigned long signal_dels = 0;
#endif

ସଂରଚନା vpiSignal_plug {
      unsigned ଅକ୍ଷର space[ଆକାର (ସଂରଚନା __vpiSignal)];
#ifdef CHECK_WITH_VALGRIND
      ସଂରଚନା vpiSignal_plug *pool;
#endif
};

ଶୂନ୍ୟ* __vpiSignal::operator ନୂତନ(size_t siz)
{
#ifdef CHECK_WITH_VALGRIND
      ପ୍ରତିଜ୍ଞା(siz == ଆକାର(ସଂରଚନା vpiSignal_plug) - ଆକାର(ସଂରଚନା vpiSignal_plug *));
#ଅନ୍ୟଥା
      ପ୍ରତିଜ୍ଞା(siz == ଆକାର(ସଂରଚନା vpiSignal_plug));
#endif
      ସ୍ଥିର ସଂରଚନା vpiSignal_plug*alloc_array = 0;
      ସ୍ଥିର unsigned alloc_index = 0;
      ଅଚଳ unsigned alloc_ଗଣନା = 512;

      ଯଦି ((alloc_array == 0) || (alloc_index == alloc_ଗଣନା)) {
	    alloc_array = static_cast<ସଂରଚନା vpiSignal_plug*>
	                  (calloc(alloc_ଗଣନା, ଆକାର(ସଂରଚନା vpiSignal_plug)));
	    alloc_index = 0;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_ଗଣନା *
	                                            ଆକାର(ସଂରଚନା vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_ଗଣନା += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_ଗଣନା*ଆକାର(vpiSignal_plug **)));
	    signal_pool[signal_pool_ଗଣନା-1] = alloc_array;
#endif
      }

      ସଂରଚନା vpiSignal_plug*cur = alloc_array + alloc_index;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, ଆକାର(ସଂରଚନା vpiSignal_plug));
      cur->pool = alloc_array;
      signal_ଗଣନା += 1;
#endif
      alloc_index += 1;
      ଫେରନ୍ତୁ cur;
}

ଶୂନ୍ୟ __vpiSignal::operator delete(ଶୂନ୍ୟ*)
{
      ପ୍ରତିଜ୍ଞା(0);
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ signal_delete(vpiHandle item)
{
      ସଂରଚନା __vpiSignal *obj = static_cast<__vpiSignal *> (item);
      ପ୍ରତିଜ୍ଞା(obj->ଗଣ୍ଠି->fil);
      obj->ଗଣ୍ଠି->fil->clear_all_callbacks();
      vvp_net_delete(obj->ଗଣ୍ଠି);
      ଯଦି (obj->bits) {
	    ପାଇଁ (unsigned idx=0; idx<obj->width(); idx+=1) {
		  delete obj->bits[idx].index;
	    }
	    obj->bits -= 1;
	    delete [] obj->bits;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

ଶୂନ୍ୟ signal_pool_delete()
{
      ଯଦି (RUNNING_ON_VALGRIND && (signal_ଗଣନା != signal_dels)) {
	    fflush(ଖାଲି);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (long) signal_ଗଣନା - signal_dels, signal_ଗଣନା);
      }

      ପାଇଁ (unsigned idx = 0; idx < signal_pool_ଗଣନା; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    ମୁକ୍ତ(signal_pool[idx]);
      }

      ମୁକ୍ତ(signal_pool);
      signal_pool = 0;
      signal_pool_ଗଣନା = 0;
}
#endif

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
ସ୍ଥିର vpiHandle fill_in_net4(ସଂରଚନା __vpiSignal*obj, __vpiScope*scope,
			      ଅଚଳ ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb,
			      ତାର୍କିକ signed_flag, vvp_net_t*ଗଣ୍ଠି)
{
      obj->id.name = name? vpip_name_string(name) : ଖାଲି;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->ଗଣ୍ଠି = ଗଣ୍ଠି;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      ଗଣନା_vpi_nets += 1;

      ଫେରନ୍ତୁ obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 ଅଚଳ ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb,
			 ତାର୍କିକ signed_flag, vvp_net_t*ଗଣ୍ଠି)
{
      ସଂରଚନା __vpiSignal*obj = ନୂତନ signal_net;
      ଫେରନ୍ତୁ fill_in_net4(obj, scope, name, msb, lsb, signed_flag, ଗଣ୍ଠି);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା PV_get_base(ସଂରଚନା __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      ଯଦି (rfp->sbase != 0) {
	    s_vpi_ମୂଲ୍ୟ val;
	      /* Check to see if the value is defined. */
	    val.format = vpiVectorVal;
	    vpi_get_ମୂଲ୍ୟ(rfp->sbase, &val);
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶବ୍ଦଗୁଡ଼ିକ = (vpi_get(vpiSize, rfp->sbase)-1)/32 + 1;
	    ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0; idx < ଶବ୍ଦଗୁଡ଼ିକ; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  ଯଦି (val.value.vector[idx].bval != 0) ଫେରନ୍ତୁ INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_ମୂଲ୍ୟ(rfp->sbase, &val);
	    ଫେରନ୍ତୁ val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      ଫେରନ୍ତୁ rfp->tbase;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା PV_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rval = 0;
      ବାଛ (code) {
	ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ 0;  // Not implemented for now!

	ସ୍ଥିତି vpiSigned:
	    ଫେରନ୍ତୁ 0;  // A part/bit select is always unsigned!

	ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ rfp->width;

	  /* This is like the &A<> in array.cc. */
	ସ୍ଥିତି vpiConstantSelect:
	    ଫେରନ୍ତୁ rfp->sbase == 0;

	ସ୍ଥିତି vpi_ବାମ_Range:
            rval += rfp->width - 1;
	    // fallthrough
	ସ୍ଥିତି vpi_ଡାହାଣ_Range: {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ = vpi_get(vpi_ବାମ_Range, rfp->parent);
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଡାହାଣ = vpi_get(vpi_ଡାହାଣ_Range, rfp->parent);
	    rval += PV_get_base(rfp);
	    ଫେରନ୍ତୁ ବାମ >= ଡାହାଣ ? ଡାହାଣ + rval : ଡାହାଣ - rval;
	}

        ସ୍ଥିତି vpiAutomatic:
            ଫେରନ୍ତୁ vpi_get(vpiAutomatic, rfp->parent);

#ଯଦି defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        ସ୍ଥିତି _vpiFromThr:
            ଫେରନ୍ତୁ _vpi_at_PV;
#endif

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅକ୍ଷର* PV_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ବାଛ (code) {
	ସ୍ଥିତି vpiFile:  // Not implemented for now!
	    ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[0]);

	ସ୍ଥିତି vpiName:
	ସ୍ଥିତି vpiFullName: {
	    ଅଚଳ ଅକ୍ଷର*nm = vpi_get_str(code, rfp->parent);
	    size_t len = 256+strlen(nm);
	    ଅକ୍ଷର *full = static_cast<ଅକ୍ଷର *>(malloc(len));
	    snprintf(full, len, "%s[%d:%d]", nm,
	                                     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vpi_get(vpi_ବାମ_Range, ref),
	                                     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vpi_get(vpi_ଡାହାଣ_Range, ref));
	    full[len-1] = 0;
	    ଅକ୍ଷର *res = simple_set_rbuf_str(full);
	    ମୁକ୍ତ(full);
	    ଫେରନ୍ତୁ res;
	}

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ PV_get_ମୂଲ୍ୟ(vpiHandle ref, p_vpi_ମୂଲ୍ୟ vp)
{
      ସଂରଚନା __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*>(rfp->net->fil);
      ପ୍ରତିଜ୍ଞା(sig);

      ବାଛ (vp->format) {

	  ସ୍ଥିତି vpiIntVal:
	    format_vpiIntVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiStringVal:
	    format_vpiStringVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_base(rfp), vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiVectorVal:
	    format_vpiVectorVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiRealVal:
	    format_vpiRealVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format, vpi_get_str(vpiFullName, rfp->parent));
	    ପ୍ରତିଜ୍ଞା(0);
      }
}

ସ୍ଥିର vpiHandle PV_put_ମୂଲ୍ୟ(vpiHandle ref, p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
      ସଂରଚନା __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);
      vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*>(rfp->net->fil);
      ପ୍ରତିଜ୍ଞା(sig);

      unsigned sig_size = sig->ମୂଲ୍ୟ_size();
      unsigned width = rfp->width;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା base = PV_get_base(rfp);
      ଯଦି (base >= (signed) sig_size) ଫେରନ୍ତୁ 0;
      ଯଦି (base + (signed) width < 0) ଫେରନ୍ତୁ 0;

      vvp_vector4_t val;
      ଯଦି (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_ମୂଲ୍ୟ(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      ଯଦି (base < 0) {
	    width += base;
	    ଯଦି (flags != vpiReleaseFlag) {
		  val = val.subvalue(-base, width);
	    }
	    base = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      ଯଦି (base+width > sig_size) {
	    width = sig_size - base;
	    ଯଦି (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      ପ୍ରତିଜ୍ଞା(rfp->parent);
      ତାର୍କିକ net_flag = rfp->parent->get_type_code()==vpiNet;
      ତାର୍କିକ full_sig = base == 0 && width == sig_size;

      vvp_net_ptr_t dest(rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value.  Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value.*/
      ଯଦି (flags == vpiReleaseFlag) {
	    ପ୍ରତିଜ୍ଞା(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    ଯଦି (full_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } ଅନ୍ୟଥା {
		  rfp->net->fil->release_pv(dest, base, width, net_flag);
	    }
	    rfp->net->fun->force_flag(ସତ୍ୟ);
	    PV_get_ମୂଲ୍ୟ(ref, vp);
	    ଫେରନ୍ତୁ ref;
      }

      ଯଦି (flags == vpiForceFlag) {
	    ଯଦି (full_sig) {
		  vvp_vector2_t mask (vvp_vector2_t::FILL1, sig_size);
		  rfp->net->force_vec4(val, mask);
	    } ଅନ୍ୟଥା {
		  vvp_vector2_t mask (vvp_vector2_t::FILL0, sig_size);
		  ପାଇଁ (unsigned idx = 0 ; idx < width ; idx += 1)
		        mask.set_bit(base+idx, 1);

		  vvp_vector4_t tmp (sig_size, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_ମୂଲ୍ୟ(tmp);

		  tmp.set_vec(base, val);
		  rfp->net->force_vec4(tmp, mask);
	    }
      } ଅନ୍ୟଥା ଯଦି (net_flag && !dynamic_cast<vvp_island_port*>(rfp->net->fun)) {
	    ଯଦି (full_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } ଅନ୍ୟଥା {
		  rfp->net->send_vec4_pv(val, base, sig_size,
		                         vthread_get_wt_context());
	    }
      } ଅନ୍ୟଥା {
	    ଯଦି (full_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } ଅନ୍ୟଥା {
		  vvp_send_vec4_pv(dest, val, base, sig_size,
	                           vthread_get_wt_context());
	    }
      }
      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର vpiHandle PV_get_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ବାଛ (code) {
	  ସ୍ଥିତି vpiParent:
	    ଫେରନ୍ତୁ rfp->parent;

	  ସ୍ଥିତି vpiScope:
	    ଫେରନ୍ତୁ vpi_handle(vpiScope, rfp->parent);

	  ସ୍ଥିତି vpiModule:
	    ଫେରନ୍ତୁ vpi_handle(vpiModule, rfp->parent);
      }

      ଫେରନ୍ତୁ 0;
}

ଧାଡ଼ିଭିତରେ __vpiPV::__vpiPV()
{ }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiPV::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiPartSelect; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiPV::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ PV_get(code, ଏହି); }

ଅକ୍ଷର* __vpiPV::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ PV_get_str(code, ଏହି); }

ଶୂନ୍ୟ __vpiPV::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{ PV_get_ମୂଲ୍ୟ(ଏହି, val); }

vpiHandle __vpiPV::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{ ଫେରନ୍ତୁ PV_put_ମୂଲ୍ୟ(ଏହି, val, flags); }

vpiHandle __vpiPV::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ PV_get_handle(code, ଏହି); }

vpiHandle vpip_make_PV(ଅକ୍ଷର*var, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
      ସଂରଚନା __vpiPV*obj = ନୂତନ __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = 0;
      obj->tbase = base;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ଫେରନ୍ତୁ obj;
}

vpiHandle vpip_make_PV(ଅକ୍ଷର*var, ଅକ୍ଷର*ପ୍ରତୀକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
      ସଂରଚନା __vpiPV*obj = ନୂତନ __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      compile_vpi_lookup(&obj->sbase, ପ୍ରତୀକ);
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ଫେରନ୍ତୁ obj;
}

vpiHandle vpip_make_PV(ଅକ୍ଷର*var, vpiHandle handle, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
      ସଂରଚନା __vpiPV*obj = ନୂତନ __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ଫେରନ୍ତୁ obj;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ PV_delete(vpiHandle item)
{
      ସଂରଚନା __vpiPV *obj = dynamic_cast<__vpiPV*>(item);
      ଯଦି (obj->sbase) {
	    ବାଛ (obj->sbase->get_type_code()) {
		ସ୍ଥିତି vpiMemoryWord:
		  ଯଦି (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiPartSelect:
		  ପ୍ରତିଜ୍ଞା(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  ଭାଙ୍ଗ;
	    }
      }
      ପ୍ରତିଜ୍ଞା(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      delete obj;
}
#endif
