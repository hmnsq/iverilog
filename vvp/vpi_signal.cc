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
বাহ্য ধ্রুব বর্ণ hex_digits[256];
বাহ্য ধ্রুব বর্ণ oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
শূন্য *need_result_buf(unsigned cnt, vpi_rbuf_t type)
{
      স্থির শূন্য*result_buf[2] = {0, 0};
      স্থির size_t result_buf_size[2] = {0, 0};

      যদি (type == RBUF_DEL) {
	    মুক্ত(result_buf[RBUF_VAL]);
	    result_buf[RBUF_VAL] = 0;
	    result_buf_size[RBUF_VAL] = 0;

	    মুক্ত(result_buf[RBUF_STR]);
	    result_buf[RBUF_STR] = 0;
	    result_buf_size[RBUF_STR] = 0;

	    প্রত্যাগম 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      যদি (result_buf_size[type] == 0) {
	    result_buf[type] = malloc(cnt);
	    result_buf_size[type] = cnt;
      } অন্যথা যদি (result_buf_size[type] < cnt) {
	    result_buf[type] = realloc(result_buf[type], cnt);
	    result_buf_size[type] = cnt;
      }

      প্রত্যাগম result_buf[type];
}

বর্ণ *simple_set_rbuf_str(ধ্রুব বর্ণ *s1)
{
      বর্ণ *res = static_cast<বর্ণ *>(need_result_buf(strlen(s1)+1, RBUF_STR));
      যদি (res) strcpy(res,s1);
      প্রত্যাগম res;
}

বর্ণ *generic_get_str(পূর্ণাঙ্ক code, vpiHandle ref, ধ্রুব বর্ণ *name, ধ্রুব বর্ণ *index)
{
      size_t len = strlen(name) + 1;  /* include space for null termination */
      বর্ণ *bn = রিক্ত;
      তার্কিক is_bn_a_pkg = অসত্য;
      যদি (code == vpiFullName) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = strlen(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    যদি (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      যদি (index != রিক্ত) len += strlen(index) + 2;  /* include space for brackets */

      বর্ণ *res = static_cast<বর্ণ *>(need_result_buf(len, RBUF_STR));
      যদি (!res) {
	    মুক্ত(bn);
	    প্রত্যাগম রিক্ত;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      যদি (bn != রিক্ত) {
	    strcat(res, bn);
	      // A package already has the "::" separator in the name
	    যদি (! is_bn_a_pkg) strcat(res, ".");
	    মুক্ত(bn);
      }
      strcat(res, name);
      যদি (index != রিক্ত) {
	    strcat(res, "[");
	    strcat(res, index);
	    strcat(res, "]");
      }
      প্রত্যাগম res;
}

স্থির vpiHandle fill_in_net4(সংরচনা __vpiSignal*obj, __vpiScope*scope,
                              ধ্রুব বর্ণ*name, পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb,
                              তার্কিক signed_flag, vvp_net_t*গ্রন্থি);

স্থির vpiHandle fill_in_var4(সংরচনা __vpiSignal*obj,
                              ধ্রুব বর্ণ*name, পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb,
                              তার্কিক signed_flag, vvp_net_t*গ্রন্থি)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      প্রত্যাগম fill_in_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, গ্রন্থি);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

স্থির শূন্য format_vpiBinStrVal(ধ্রুব vvp_signal_মান*sig, পূর্ণাঙ্ক base, unsigned wid,
                                s_vpi_মান*vp)
{
      বর্ণ *rbuf = static_cast<বর্ণ *>(need_result_buf(wid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long offset = end - 1;
      long ssize = (signed)sig->মান_size();

      প্রতি (long idx = base ;  idx < end ;  idx += 1) {
	    যদি (idx < 0 || idx >= ssize) {
                  rbuf[offset-idx] = 'x';
	    } অন্যথা {
                  rbuf[offset-idx] = vvp_bit4_to_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

স্থির শূন্য format_vpiOctStrVal(ধ্রুব vvp_signal_মান*sig, পূর্ণাঙ্ক base, unsigned wid,
                                s_vpi_মান*vp)
{
      unsigned dwid = (wid + 2) / 3;
      বর্ণ *rbuf = static_cast<বর্ণ *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->মান_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      প্রতি (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    যদি (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } অন্যথা {
                  বিকল্প (sig->value(idx)) {
		      পক্ষ BIT4_0:
			bit = 0;
			ভঙ্গ;
		      পক্ষ BIT4_1:
			bit = 1;
			ভঙ্গ;
		      পক্ষ BIT4_X:
			bit = 2;
			ভঙ্গ;
		      পক্ষ BIT4_Z:
			bit = 3;
			ভঙ্গ;
                  }
	    }
	    val |= bit << 2*((idx-base) % 3);

	    যদি ((idx-base) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      বিকল্প (wid % 3) {
	  পক্ষ 1:
	    যদি (val == 2) val = 42;
	    অন্যথা যদি (val == 3) val = 63;
	    ভঙ্গ;
	  পক্ষ 2:
	    যদি (val == 10) val = 42;
	    অন্যথা যদি (val == 15) val = 63;
	    ভঙ্গ;
      }

      যদি (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

স্থির শূন্য format_vpiHexStrVal(ধ্রুব vvp_signal_মান*sig, পূর্ণাঙ্ক base, unsigned wid,
                                s_vpi_মান*vp)
{
      unsigned dwid = (wid + 3) / 4;
      বর্ণ *rbuf = static_cast<বর্ণ *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->মান_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      প্রতি (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    যদি (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } অন্যথা {
                  বিকল্প (sig->value(idx)) {
		      পক্ষ BIT4_0:
			bit = 0;
			ভঙ্গ;
		      পক্ষ BIT4_1:
			bit = 1;
			ভঙ্গ;
		      পক্ষ BIT4_X:
			bit = 2;
			ভঙ্গ;
		      পক্ষ BIT4_Z:
			bit = 3;
			ভঙ্গ;
                  }
	    }
	    val |= bit << 2*((idx-base) % 4);

	    যদি ((idx-base) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      বিকল্প (wid % 4) {
	  পক্ষ 1:
	    যদি (val == 2) val = 170;
	    অন্যথা যদি (val == 3) val = 255;
	    ভঙ্গ;
	  পক্ষ 2:
	    যদি (val == 10) val = 170;
	    অন্যথা যদি (val == 15) val = 255;
	    ভঙ্গ;
	  পক্ষ 3:
	    যদি (val == 42) val = 170;
	    অন্যথা যদি (val == 63) val = 255;
	    ভঙ্গ;
      }

      যদি (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

স্থির শূন্য format_vpiDecStrVal(ধ্রুব vvp_signal_মান*sig, পূর্ণাঙ্ক base, unsigned wid,
                                পূর্ণাঙ্ক signed_flag, s_vpi_মান*vp)
{
      unsigned hwid = (sig->মান_size()+2) / 3 + 1;
      বর্ণ *rbuf = static_cast<বর্ণ *>(need_result_buf(hwid, RBUF_VAL));
      long ssize = (signed)sig->মান_size();
      long end = base + (signed)wid;

	/* Do we have an end outside of the real signal vector. */
      যদি (base < 0 || end > ssize) {
	    তার্কিক all_x = সত্য;
	    যদি (end > ssize) end = ssize;
	    যদি (base < 0) base = 0;
	    প্রতি (long idx = base ;  idx < end ;  idx += 1) {
		  যদি (sig->value(idx) != BIT4_X) {
			all_x = অসত্য;
			ভঙ্গ;
		  }
	    }

	    যদি (all_x) {
		  rbuf[0] = 'x';
	    } অন্যথা {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    প্রত্যাগম;
      }

      vvp_vector4_t vec4;
      যদি (base == 0 && end == ssize) {
	    sig->vec4_মান(vec4);
      } অন্যথা {
	    vvp_vector4_t tmp;
	    sig->vec4_মান(tmp);
	    vec4 = tmp.subvalue(base, wid);
      }

      vpip_vec4_to_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

স্থির শূন্য format_vpiIntVal(ধ্রুব vvp_signal_মান*sig, পূর্ণাঙ্ক base, unsigned wid,
                             পূর্ণাঙ্ক signed_flag, s_vpi_মান*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_মান(tmp);
      vvp_vector4_t sub = tmp.subvalue(base, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      যদি (পরিমাণ(vp->value.integer) == পরিমাণ(int32_t)) {
	    int32_t val = 0;
	    vector4_to_মান(sub, val, signed_flag, অসত্য);
	    vp->value.integer = val;
      } অন্যথা {
	    প্রতিজ্ঞা(পরিমাণ(vp->value.integer) == পরিমাণ(int64_t));
	    int64_t val = 0;
	    vector4_to_মান(sub, val, signed_flag, অসত্য);
	    vp->value.integer = val;
      }
}

স্থির শূন্য format_vpiRealVal(ধ্রুব vvp_signal_মান*sig, পূর্ণাঙ্ক base, unsigned wid,
                              পূর্ণাঙ্ক signed_flag, s_vpi_মান*vp)
{
      vvp_vector4_t vec4(wid);
      long ssize = (signed)sig->মান_size();
      long end = base + (signed)wid;
      যদি (end > ssize) end = ssize;

      প্রতি (long idx = (base < 0) ? 0 : base ;  idx < end ;  idx += 1) {
	    vec4.set_bit(idx-base, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_to_মান(vec4, vp->value.real, signed_flag);
}

স্থির শূন্য format_vpiStringVal(ধ্রুব vvp_signal_মান*sig, পূর্ণাঙ্ক base, unsigned wid,
                                s_vpi_মান*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      বর্ণ *rbuf = static_cast<বর্ণ *>(need_result_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      বর্ণ *cp = rbuf;

      বর্ণ tmp = 0;
      প্রতি (long idx = base+(signed)wid-1; idx >= base; idx -= 1) {
	    tmp <<= 1;

	    যদি (idx >=0 && idx < (signed)sig->মান_size() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    যদি (((idx-base)&7)==0){
		  /* Skip leading nulls. */
		  যদি (tmp == 0 && cp == rbuf)
			অনুবর্ত;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

স্থির শূন্য format_vpiScalarVal(ধ্রুব vvp_signal_মান*sig, পূর্ণাঙ্ক base,
                                s_vpi_মান*vp)
{
      যদি (base >= 0 && base < (signed)sig->মান_size()) {
	    বিকল্প (sig->value(base)) {
		পক্ষ BIT4_0:
		  vp->value.scalar = vpi0;
		  ভঙ্গ;
		পক্ষ BIT4_1:
		  vp->value.scalar = vpi1;
		  ভঙ্গ;
		পক্ষ BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_মান(base);
		  যদি (strn.strength0() == 1) vp->value.scalar = vpiH;
		  অন্যথা যদি (strn.strength1() == 1) vp->value.scalar = vpiL;
		  অন্যথা vp->value.scalar = vpiX;
		  ভঙ্গ;
		}
		পক্ষ BIT4_Z:
		  vp->value.scalar = vpiZ;
		  ভঙ্গ;
	    }
      } অন্যথা {
	    vp->value.scalar = vpiX;
      }
}

স্থির শূন্য format_vpiStrengthVal(ধ্রুব vvp_signal_মান*sig, পূর্ণাঙ্ক base,
                                  unsigned wid, s_vpi_মান*vp)
{
      long end = base + (signed)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_result_buf(wid * পরিমাণ(s_vpi_strengthval), RBUF_VAL));

      প্রতি (long idx = base ;  idx < end ;  idx += 1) {
	    যদি (idx >=0 && idx < (signed)sig->মান_size()) {
		  vvp_scalar_t val = sig->scalar_মান(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  unsigned s0 = 1 << val.strength0();
		  unsigned s1 = 1 << val.strength1();

		  বিকল্প (val.value()) {
		      পক্ষ BIT4_0:
			op[idx-base].logic = vpi0;
			op[idx-base].s0 = s0|s1;
			op[idx-base].s1 = 0;
			ভঙ্গ;

		      পক্ষ BIT4_1:
			op[idx-base].logic = vpi1;
			op[idx-base].s0 = 0;
			op[idx-base].s1 = s0|s1;
			ভঙ্গ;

		      পক্ষ BIT4_X:
			op[idx-base].logic = vpiX;
			op[idx-base].s0 = s0;
			op[idx-base].s1 = s1;
			ভঙ্গ;

		      পক্ষ BIT4_Z:
			op[idx-base].logic = vpiZ;
			op[idx-base].s0 = vpiHiZ;
			op[idx-base].s1 = vpiHiZ;
			ভঙ্গ;
		  }
	    } অন্যথা {
		  op[idx-base].logic = vpiX;
		  op[idx-base].s0 = vpiStrongDrive;
		  op[idx-base].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

স্থির শূন্য format_vpiVectorVal(ধ্রুব vvp_signal_মান*sig, পূর্ণাঙ্ক base, unsigned wid,
                                s_vpi_মান*vp)
{
      long end = base + (signed)wid;
      unsigned পূর্ণাঙ্ক obit = 0;
      unsigned hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_result_buf(hwid * পরিমাণ(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      প্রতি (long idx = base ;  idx < end ;  idx += 1) {
	    যদি (base >= 0 && base < (signed)sig->মান_size()) {
		বিকল্প (sig->value(idx)) {
		পক্ষ BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  ভঙ্গ;
		পক্ষ BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  ভঙ্গ;
		পক্ষ BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  ভঙ্গ;
		পক্ষ BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  ভঙ্গ;
		}
	    } অন্যথা {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    যদি (!(obit % 32)) {
		  op += 1;
		  যদি ((op - vp->value.vector) < (ptrdiff_t)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
স্থির পূর্ণাঙ্ক signal_get(পূর্ণাঙ্ক code, vpiHandle ref)
{
      সংরচনা __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);

      বিকল্প (code) {
	  পক্ষ vpi_সারি_No:
	    প্রত্যাগম 0;  // Not implemented for now!

	  পক্ষ vpiSigned:
	    প্রত্যাগম rfp->signed_flag != 0;

	  পক্ষ vpiArray:
	    প্রত্যাগম rfp->is_netarray != 0;

	  পক্ষ vpiIndex: // This only works while we have a single index.
	    যদি (rfp->is_netarray) {
		  s_vpi_মান vp;
		  vp.format = vpiIntVal;
		  vpi_get_মান(rfp->id.index, &vp);
		  প্রত্যাগম vp.value.integer;
	    } অন্যথা {
		  প্রত্যাগম vpiUndefined;
	    }

	  পক্ষ vpiSize:
	    প্রত্যাগম rfp->width();

	  পক্ষ vpiNetType:
	    যদি (ref->get_type_code()==vpiNet)
		  প্রত্যাগম vpiWire;
	    অন্যথা
		  প্রত্যাগম vpiUndefined;

	  পক্ষ vpi_বাম_Range:
            প্রত্যাগম rfp->msb.get_মান();

	  পক্ষ vpi_ডান_Range:
            প্রত্যাগম rfp->lsb.get_মান();

	  পক্ষ vpiScalar:
	    প্রত্যাগম (rfp->msb.get_মান() == 0 && rfp->lsb.get_মান() == 0);
	  পক্ষ vpiVector:
	    প্রত্যাগম (rfp->msb.get_মান() != rfp->lsb.get_মান());

          পক্ষ vpiAutomatic:
            প্রত্যাগম vpip_scope(rfp)->is_automatic() ? 1  : 0;

#ifdef BR916_STOPGAP_FIX
          পক্ষ _vpiFromThr:
            প্রত্যাগম _vpiNoThr;
#endif

	  স্বভাব:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    প্রত্যাগম vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
স্থির int64_t signal_get64(পূর্ণাঙ্ক code, vpiHandle ref)
{
      সংরচনা __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);

      বিকল্প (code) {
	    // This private property must return zero when undefined.
	  পক্ষ _vpiNexusId:
	    যদি (rfp->msb.get_মান() == rfp->lsb.get_মান())
		  প্রত্যাগম reinterpret_cast<int64_t>(rfp->গ্রন্থি);
	    প্রত্যাগম 0;

	  স্বভাব:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    প্রত্যাগম vpiUndefined;
      }
}

স্থির বর্ণ* signal_get_str(পূর্ণাঙ্ক code, vpiHandle ref)
{
      সংরচনা __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);

      যদি (code == vpiFile) {  // Not implemented for now!
	    প্রত্যাগম simple_set_rbuf_str(file_names[0]);
      }

      যদি ((code != vpiName) && (code != vpiFullName)) প্রত্যাগম রিক্ত;

      বর্ণ *nm;
      ধ্রুব বর্ণ *ixs;
      যদি (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_মান vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_মান(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } অন্যথা {
	    nm = strdup(rfp->id.name);
	    ixs = রিক্ত;
      }

	/* The scope information is added here for vpiFullName. */
      বর্ণ *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      মুক্ত(nm);
      প্রত্যাগম rbuf;
}

স্থির vpiHandle signal_get_handle(পূর্ণাঙ্ক code, vpiHandle ref)
{
      সংরচনা __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);

      বিকল্প (code) {

	  পক্ষ vpiParent:
	    প্রত্যাগম rfp->is_netarray? rfp->within.parent : রিক্ত;

	  পক্ষ vpiIndex:
	    প্রত্যাগম rfp->is_netarray? rfp->id.index : রিক্ত;

	  পক্ষ vpi_বাম_Range:
	    প্রত্যাগম &rfp->msb;
	  পক্ষ vpi_ডান_Range:
	    প্রত্যাগম &rfp->lsb;

	  পক্ষ vpiScope:
	    প্রত্যাগম vpip_scope(rfp);

	  পক্ষ vpiModule:
	    প্রত্যাগম vpip_module(vpip_scope(rfp));
      }

      প্রত্যাগম 0;
}

স্থির vpiHandle signal_iterate(পূর্ণাঙ্ক code, vpiHandle ref)
{
      সংরচনা __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);

      যদি (code == vpiIndex) {
	    প্রত্যাগম rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : রিক্ত;
      }

      প্রত্যাগম 0;
}

শূন্য __vpiSignal::make_bits()
{
      প্রতিজ্ঞা(bits == রিক্ত);
      bits = নূতন সংরচনা __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      bits->parent = এতত্;
	// Now point to word[0].
      bits += 1;

	// Update each bit to point to the base
      সংরচনা __vpiBit*base = bits;
      প্রতি (unsigned idx = 0; idx < width(); idx += 1) {
	    base[idx].bit0 = base;
	    পূর্ণাঙ্ক real_idx;
	    যদি (msb.get_মান() >= lsb.get_মান()) {
		  real_idx = idx + lsb.get_মান();
	    } অন্যথা {
		  real_idx = lsb.get_মান() - idx;
	    }
	    base[idx].index = নূতন __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_index(পূর্ণাঙ্ক idx)
{
	/* Check to see if the index is in range. */
      যদি (msb.get_মান() >= lsb.get_মান()) {
	    যদি ((idx > msb.get_মান()) || (idx < lsb.get_মান())) প্রত্যাগম 0;
      } অন্যথা {
	    যদি ((idx < msb.get_মান()) || (idx > lsb.get_মান())) প্রত্যাগম 0;
      }

	/* Normalize the index */
      unsigned norm_idx;
      যদি (msb.get_মান() >= lsb.get_মান()) {
	    norm_idx = idx - lsb.get_মান();
      } অন্যথা {
	    norm_idx = lsb.get_মান() - idx;
      }

      যদি (bits == রিক্ত) make_bits();

      প্রত্যাগম &(bits[norm_idx].as_bit);
}

শূন্য __vpiSignal::get_bit_মান(ধ্রুব সংরচনা __vpiBit*bit, p_vpi_মান vp)
{
      unsigned index = bit->get_norm_index();

      vvp_signal_মান*vsig = dynamic_cast<vvp_signal_মান*>(গ্রন্থি->fil);
      প্রতিজ্ঞা(vsig);

      যদি (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      বিকল্প (vp->format) {
	পক্ষ vpiBinStrVal:
            format_vpiBinStrVal(vsig, index, 1, vp);
	    ভঙ্গ;

	পক্ষ vpiOctStrVal:
            format_vpiOctStrVal(vsig, index, 1, vp);
	    ভঙ্গ;

	পক্ষ vpiDecStrVal:
            format_vpiDecStrVal(vsig, index, 1, অসত্য, vp);
	    ভঙ্গ;

	পক্ষ vpiHexStrVal:
            format_vpiHexStrVal(vsig, index, 1, vp);
	    ভঙ্গ;

	পক্ষ vpiStringVal:
            format_vpiStringVal(vsig, index, 1, vp);
	    ভঙ্গ;

	পক্ষ vpiIntVal:
            format_vpiIntVal(vsig, index, 1, অসত্য, vp);
	    ভঙ্গ;

	পক্ষ vpiRealVal:
            format_vpiRealVal(vsig, index, 1, অসত্য, vp);
	    ভঙ্গ;

	পক্ষ vpiScalarVal:
            format_vpiScalarVal(vsig, index, vp);
	    ভঙ্গ;

        পক্ষ vpiStrengthVal:
            format_vpiStrengthVal(vsig, index, 1, vp);
            ভঙ্গ;

	পক্ষ vpiVectorVal:
            format_vpiVectorVal(vsig, index, 1, vp);
	    ভঙ্গ;

	স্বভাব:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (পূর্ণাঙ্ক)vp->format, vpi_get_str(vpiName),
		    bit->get_index(),
	            vpip_scope(এতত্)->scope_name());
	    প্রতিজ্ঞা(0);
      };
}

vpiHandle __vpiSignal::put_bit_মান(সংরচনা __vpiBit*bit, p_vpi_মান vp, পূর্ণাঙ্ক flags)
{
      unsigned index = bit->get_norm_index();
      vvp_net_ptr_t dest(গ্রন্থি, 0);
      vvp_vector4_t val = vec4_from_vpi_মান(vp, 1);

      যদি ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    bit->as_bit.vpi_get_str(vpiFullName));
	    প্রত্যাগম রিক্ত;
      }

      যদি ((get_type_code() == vpiNet) &&
          !dynamic_cast<vvp_island_port*>(গ্রন্থি->fun)) {
	    গ্রন্থি->send_vec4_pv(val, index, width(),
	                            vthread_get_wt_context());
      } অন্যথা {
	    vvp_send_vec4_pv(dest, val, index, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      প্রত্যাগম রিক্ত;
}

স্থির vpiHandle signal_index(পূর্ণাঙ্ক idx, vpiHandle ref)
{
      সংরচনা __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      যদি ((type != vpiNet) && (type != vpiReg)) প্রত্যাগম 0;

      প্রত্যাগম rfp->get_index(idx);
}

unsigned __vpiSignal::width(শূন্য) ধ্রুব
{
      unsigned wid = (msb.get_মান() >= lsb.get_মান())
	    ? (msb.get_মান() - lsb.get_মান() + 1)
	    : (lsb.get_মান() - msb.get_মান() + 1);

      প্রত্যাগম wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
স্থির শূন্য signal_get_মান(vpiHandle ref, s_vpi_মান*vp)
{
      সংরচনা __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);

      unsigned wid = rfp->width();

      vvp_signal_মান*vsig = dynamic_cast<vvp_signal_মান*>(rfp->গ্রন্থি->fil);
      প্রতিজ্ঞা(vsig);

      বিকল্প (vp->format) {

	  পক্ষ vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ভঙ্গ;

	  পক্ষ vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    ভঙ্গ;

	  পক্ষ vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    ভঙ্গ;

	  পক্ষ vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    ভঙ্গ;

	  পক্ষ vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    ভঙ্গ;

	  পক্ষ vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    ভঙ্গ;

	  পক্ষ vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ভঙ্গ;

	  পক্ষ vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    ভঙ্গ;

	  পক্ষ vpiVectorVal:
	    format_vpiVectorVal(vsig, 0, wid, vp);
	    ভঙ্গ;

	  পক্ষ vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ভঙ্গ;

	  পক্ষ vpiObjTypeVal:
	    যদি (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } অন্যথা {
		  vp->format = vpiVectorVal;
		  format_vpiVectorVal(vsig, 0, wid, vp);
	    }
	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (পূর্ণাঙ্ক)vp->format, vpi_get_str(vpiName, ref),
		    vpip_scope(rfp)->scope_name());
	    প্রতিজ্ঞা(0);
      }
}

/*
 * The put_value method writes the value into the vector, and returns
 * the affected ref. This operation works much like the %set or
 * %assign instructions and causes all the side-effects that the
 * equivalent instruction would cause.
 */

স্থির vvp_vector4_t from_stringval(ধ্রুব বর্ণ*str, unsigned wid)
{
      unsigned idx;
      ধ্রুব বর্ণ*cp;

      cp = str + strlen(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      যাবত্ ((idx < wid) && (cp > str)) {
	    unsigned byte = *--cp;
	    পূর্ণাঙ্ক bit;

	    প্রতি (bit = 0 ;  (bit < 8) && (idx < wid) ;  bit += 1) {
		  যদি (byte & 1)
			val.set_bit(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      প্রত্যাগম val;
}

স্থির vpiHandle signal_put_মান(vpiHandle ref, s_vpi_মান*vp, পূর্ণাঙ্ক flags)
{
      unsigned wid;
      সংরচনা __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);
      vvp_net_ptr_t dest(rfp->গ্রন্থি, 0);

      তার্কিক net_flag = ref->get_type_code()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      যদি (flags == vpiReleaseFlag) {
	    প্রতিজ্ঞা(rfp->গ্রন্থি->fil);
	    rfp->গ্রন্থি->fil->force_unlink();
	    rfp->গ্রন্থি->fil->release(dest, net_flag);
	    rfp->গ্রন্থি->fun->force_flag(সত্য);
	    signal_get_মান(ref, vp);
	    প্রত্যাগম ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_মান() >= rfp->lsb.get_মান())
	    ? (rfp->msb.get_মান() - rfp->lsb.get_মান() + 1)
	    : (rfp->lsb.get_মান() - rfp->msb.get_মান() + 1);

      vvp_vector4_t val = vec4_from_vpi_মান(vp, wid);

      যদি (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, wid);
	    rfp->গ্রন্থি->force_vec4(val, mask);
      } অন্যথা যদি (net_flag && !dynamic_cast<vvp_island_port*>(rfp->গ্রন্থি->fun)) {
	    rfp->গ্রন্থি->send_vec4(val, vthread_get_wt_context());
      } অন্যথা {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      প্রত্যাগম ref;
}

vvp_vector4_t vec4_from_vpi_মান(s_vpi_মান*vp, unsigned wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      বিকল্প (vp->format) {

	  পক্ষ vpiIntVal: {
		long vpi_val = vp->value.integer;
		প্রতি (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t bit = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.set_bit(idx, bit);
		      vpi_val >>= 1;
		}
		ভঙ্গ;
	  }

	  পক্ষ vpiVectorVal:
	    প্রতি (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long aval = vp->value.vector[idx/32].aval;
		  unsigned long bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  পূর্ণাঙ্ক bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t bit = scalar_to_bit4(bitmask);
		  val.set_bit(idx, bit);
	    }
	    ভঙ্গ;
	  পক্ষ vpiBinStrVal:
	    vpip_bin_str_to_vec4(val, vp->value.str);
	    ভঙ্গ;
	  পক্ষ vpiOctStrVal:
	    vpip_oct_str_to_vec4(val, vp->value.str);
	    ভঙ্গ;
	  পক্ষ vpiDecStrVal:
	    vpip_dec_str_to_vec4(val, vp->value.str);
	    ভঙ্গ;
	  পক্ষ vpiHexStrVal:
	    vpip_hex_str_to_vec4(val, vp->value.str);
	    ভঙ্গ;
	  পক্ষ vpiScalarVal:
	    val.set_bit(0, scalar_to_bit4(vp->value.scalar));
	    ভঙ্গ;
	  পক্ষ vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    ভঙ্গ;
	  পক্ষ vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (পূর্ণাঙ্ক)vp->format);
	    প্রতিজ্ঞা(0);

      }

      প্রত্যাগম val;
}

পূর্ণাঙ্ক __vpiSignal::vpi_get(পূর্ণাঙ্ক code)
{ প্রত্যাগম signal_get(code, এতত্); }

int64_t __vpiSignal::vpi_get64(পূর্ণাঙ্ক code)
{ প্রত্যাগম signal_get64(code, এতত্); }

বর্ণ* __vpiSignal::vpi_get_str(পূর্ণাঙ্ক code)
{ প্রত্যাগম signal_get_str(code, এতত্); }

শূন্য __vpiSignal::vpi_get_মান(p_vpi_মান val)
{ signal_get_মান(এতত্, val); }

vpiHandle __vpiSignal::vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags)
{ প্রত্যাগম signal_put_মান(এতত্, val, flags); }

vpiHandle __vpiSignal::vpi_handle(পূর্ণাঙ্ক code)
{ প্রত্যাগম signal_get_handle(code, এতত্); }

vpiHandle __vpiSignal::vpi_iterate(পূর্ণাঙ্ক code)
{ প্রত্যাগম signal_iterate(code, এতত্); }

vpiHandle __vpiSignal::vpi_index(পূর্ণাঙ্ক idx)
{ প্রত্যাগম signal_index(idx, এতত্); }

সংরচনা signal_reg : সার্বজনিক __vpiSignal {
      অন্তঃস্থ signal_reg() { }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override { প্রত্যাগম vpiReg; }
};

সংরচনা signal_integer : সার্বজনিক __vpiSignal {
      অন্তঃস্থ signal_integer() { }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override { প্রত্যাগম vpiIntegerVar; }
};

সংরচনা signal_net : সার্বজনিক __vpiSignal {
      অন্তঃস্থ signal_net() { }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override { প্রত্যাগম vpiNet; }
};

সংরচনা signal_byte : সার্বজনিক __vpiSignal {
      অন্তঃস্থ signal_byte() { }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override { প্রত্যাগম vpiByteVar; }
};

সংরচনা signal_bitvar : সার্বজনিক __vpiSignal {
      অন্তঃস্থ signal_bitvar() { }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override { প্রত্যাগম vpiBitVar; }
};

সংরচনা signal_shortint : সার্বজনিক __vpiSignal {
      অন্তঃস্থ signal_shortint() { }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override { প্রত্যাগম vpiShortIntVar; }
};

সংরচনা signal_int : সার্বজনিক __vpiSignal {
      অন্তঃস্থ signal_int() { }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override { প্রত্যাগম vpiIntVar; }
};

সংরচনা signal_longint : সার্বজনিক __vpiSignal {
      অন্তঃস্থ signal_longint() { }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override { প্রত্যাগম vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(ধ্রুব বর্ণ*name, পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = নূতন signal_integer;
      প্রত্যাগম fill_in_var4(obj, name, msb, lsb, সত্য, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(ধ্রুব বর্ণ*name, পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb, তার্কিক signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      যদি ((! signed_flag) || (lsb != 0) ) {
	    obj = নূতন signal_bitvar;
      } অন্যথা {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    বিকল্প (msb) {
		পক্ষ 7:
		  obj = নূতন signal_byte;
		  ভঙ্গ;
		পক্ষ 15:
		  obj = নূতন signal_shortint;
		  ভঙ্গ;
		পক্ষ 31:
		  obj = নূতন signal_int;
		  ভঙ্গ;
		পক্ষ 63:
		  obj = নূতন signal_longint;
		  ভঙ্গ;
		স্বভাব:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = নূতন signal_bitvar;
		  ভঙ্গ;
	    }
      }

      প্রত্যাগম fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(ধ্রুব বর্ণ*name, পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb,
			তার্কিক signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = নূতন signal_reg;
      প্রত্যাগম fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

#ifdef CHECK_WITH_VALGRIND
স্থির সংরচনা vpiSignal_plug **signal_pool = 0;
স্থির unsigned signal_pool_গুনতি = 0;
স্থির unsigned long signal_গুনতি = 0;
স্থির unsigned long signal_dels = 0;
#endif

সংরচনা vpiSignal_plug {
      unsigned বর্ণ space[পরিমাণ (সংরচনা __vpiSignal)];
#ifdef CHECK_WITH_VALGRIND
      সংরচনা vpiSignal_plug *pool;
#endif
};

শূন্য* __vpiSignal::operator নূতন(size_t siz)
{
#ifdef CHECK_WITH_VALGRIND
      প্রতিজ্ঞা(siz == পরিমাণ(সংরচনা vpiSignal_plug) - পরিমাণ(সংরচনা vpiSignal_plug *));
#অন্যথা
      প্রতিজ্ঞা(siz == পরিমাণ(সংরচনা vpiSignal_plug));
#endif
      স্থির সংরচনা vpiSignal_plug*alloc_array = 0;
      স্থির unsigned alloc_index = 0;
      ধ্রুব unsigned alloc_গুনতি = 512;

      যদি ((alloc_array == 0) || (alloc_index == alloc_গুনতি)) {
	    alloc_array = static_cast<সংরচনা vpiSignal_plug*>
	                  (calloc(alloc_গুনতি, পরিমাণ(সংরচনা vpiSignal_plug)));
	    alloc_index = 0;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_গুনতি *
	                                            পরিমাণ(সংরচনা vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_গুনতি += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_গুনতি*পরিমাণ(vpiSignal_plug **)));
	    signal_pool[signal_pool_গুনতি-1] = alloc_array;
#endif
      }

      সংরচনা vpiSignal_plug*cur = alloc_array + alloc_index;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, পরিমাণ(সংরচনা vpiSignal_plug));
      cur->pool = alloc_array;
      signal_গুনতি += 1;
#endif
      alloc_index += 1;
      প্রত্যাগম cur;
}

শূন্য __vpiSignal::operator delete(শূন্য*)
{
      প্রতিজ্ঞা(0);
}

#ifdef CHECK_WITH_VALGRIND
শূন্য signal_delete(vpiHandle item)
{
      সংরচনা __vpiSignal *obj = static_cast<__vpiSignal *> (item);
      প্রতিজ্ঞা(obj->গ্রন্থি->fil);
      obj->গ্রন্থি->fil->clear_all_callbacks();
      vvp_net_delete(obj->গ্রন্থি);
      যদি (obj->bits) {
	    প্রতি (unsigned idx=0; idx<obj->width(); idx+=1) {
		  delete obj->bits[idx].index;
	    }
	    obj->bits -= 1;
	    delete [] obj->bits;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

শূন্য signal_pool_delete()
{
      যদি (RUNNING_ON_VALGRIND && (signal_গুনতি != signal_dels)) {
	    fflush(রিক্ত);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (long) signal_গুনতি - signal_dels, signal_গুনতি);
      }

      প্রতি (unsigned idx = 0; idx < signal_pool_গুনতি; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    মুক্ত(signal_pool[idx]);
      }

      মুক্ত(signal_pool);
      signal_pool = 0;
      signal_pool_গুনতি = 0;
}
#endif

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
স্থির vpiHandle fill_in_net4(সংরচনা __vpiSignal*obj, __vpiScope*scope,
			      ধ্রুব বর্ণ*name, পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb,
			      তার্কিক signed_flag, vvp_net_t*গ্রন্থি)
{
      obj->id.name = name? vpip_name_string(name) : রিক্ত;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->গ্রন্থি = গ্রন্থি;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      গুনতি_vpi_nets += 1;

      প্রত্যাগম obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 ধ্রুব বর্ণ*name, পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb,
			 তার্কিক signed_flag, vvp_net_t*গ্রন্থি)
{
      সংরচনা __vpiSignal*obj = নূতন signal_net;
      প্রত্যাগম fill_in_net4(obj, scope, name, msb, lsb, signed_flag, গ্রন্থি);
}

স্থির পূর্ণাঙ্ক PV_get_base(সংরচনা __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      যদি (rfp->sbase != 0) {
	    s_vpi_মান val;
	      /* Check to see if the value is defined. */
	    val.format = vpiVectorVal;
	    vpi_get_মান(rfp->sbase, &val);
	    পূর্ণাঙ্ক শব্দগুলি = (vpi_get(vpiSize, rfp->sbase)-1)/32 + 1;
	    প্রতি(পূর্ণাঙ্ক idx = 0; idx < শব্দগুলি; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  যদি (val.value.vector[idx].bval != 0) প্রত্যাগম INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_মান(rfp->sbase, &val);
	    প্রত্যাগম val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      প্রত্যাগম rfp->tbase;
}

স্থির পূর্ণাঙ্ক PV_get(পূর্ণাঙ্ক code, vpiHandle ref)
{
      সংরচনা __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      প্রতিজ্ঞা(rfp);

      পূর্ণাঙ্ক rval = 0;
      বিকল্প (code) {
	পক্ষ vpi_সারি_No:
	    প্রত্যাগম 0;  // Not implemented for now!

	পক্ষ vpiSigned:
	    প্রত্যাগম 0;  // A part/bit select is always unsigned!

	পক্ষ vpiSize:
	    প্রত্যাগম rfp->width;

	  /* This is like the &A<> in array.cc. */
	পক্ষ vpiConstantSelect:
	    প্রত্যাগম rfp->sbase == 0;

	পক্ষ vpi_বাম_Range:
            rval += rfp->width - 1;
	    // fallthrough
	পক্ষ vpi_ডান_Range: {
	    পূর্ণাঙ্ক বাম = vpi_get(vpi_বাম_Range, rfp->parent);
	    পূর্ণাঙ্ক ডান = vpi_get(vpi_ডান_Range, rfp->parent);
	    rval += PV_get_base(rfp);
	    প্রত্যাগম বাম >= ডান ? ডান + rval : ডান - rval;
	}

        পক্ষ vpiAutomatic:
            প্রত্যাগম vpi_get(vpiAutomatic, rfp->parent);

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        পক্ষ _vpiFromThr:
            প্রত্যাগম _vpi_at_PV;
#endif

	স্বভাব:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      প্রত্যাগম 0;
}

স্থির বর্ণ* PV_get_str(পূর্ণাঙ্ক code, vpiHandle ref)
{
      সংরচনা __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      প্রতিজ্ঞা(rfp);

      বিকল্প (code) {
	পক্ষ vpiFile:  // Not implemented for now!
	    প্রত্যাগম simple_set_rbuf_str(file_names[0]);

	পক্ষ vpiName:
	পক্ষ vpiFullName: {
	    ধ্রুব বর্ণ*nm = vpi_get_str(code, rfp->parent);
	    size_t len = 256+strlen(nm);
	    বর্ণ *full = static_cast<বর্ণ *>(malloc(len));
	    snprintf(full, len, "%s[%d:%d]", nm,
	                                     (পূর্ণাঙ্ক)vpi_get(vpi_বাম_Range, ref),
	                                     (পূর্ণাঙ্ক)vpi_get(vpi_ডান_Range, ref));
	    full[len-1] = 0;
	    বর্ণ *res = simple_set_rbuf_str(full);
	    মুক্ত(full);
	    প্রত্যাগম res;
	}

	স্বভাব:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      প্রত্যাগম 0;
}

স্থির শূন্য PV_get_মান(vpiHandle ref, p_vpi_মান vp)
{
      সংরচনা __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      প্রতিজ্ঞা(rfp);

      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*>(rfp->net->fil);
      প্রতিজ্ঞা(sig);

      বিকল্প (vp->format) {

	  পক্ষ vpiIntVal:
	    format_vpiIntVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    ভঙ্গ;

	  পক্ষ vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ভঙ্গ;

	  পক্ষ vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ভঙ্গ;

	  পক্ষ vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ভঙ্গ;

	  পক্ষ vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    ভঙ্গ;

	  পক্ষ vpiStringVal:
	    format_vpiStringVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ভঙ্গ;

	  পক্ষ vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_base(rfp), vp);
	    ভঙ্গ;

	  পক্ষ vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ভঙ্গ;

	  পক্ষ vpiVectorVal:
	    format_vpiVectorVal(sig, PV_get_base(rfp), rfp->width, vp);
	    ভঙ্গ;

	  পক্ষ vpiRealVal:
	    format_vpiRealVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (পূর্ণাঙ্ক)vp->format, vpi_get_str(vpiFullName, rfp->parent));
	    প্রতিজ্ঞা(0);
      }
}

স্থির vpiHandle PV_put_মান(vpiHandle ref, p_vpi_মান vp, পূর্ণাঙ্ক flags)
{
      সংরচনা __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      প্রতিজ্ঞা(rfp);
      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*>(rfp->net->fil);
      প্রতিজ্ঞা(sig);

      unsigned sig_size = sig->মান_size();
      unsigned width = rfp->width;
      পূর্ণাঙ্ক base = PV_get_base(rfp);
      যদি (base >= (signed) sig_size) প্রত্যাগম 0;
      যদি (base + (signed) width < 0) প্রত্যাগম 0;

      vvp_vector4_t val;
      যদি (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_মান(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      যদি (base < 0) {
	    width += base;
	    যদি (flags != vpiReleaseFlag) {
		  val = val.subvalue(-base, width);
	    }
	    base = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      যদি (base+width > sig_size) {
	    width = sig_size - base;
	    যদি (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      প্রতিজ্ঞা(rfp->parent);
      তার্কিক net_flag = rfp->parent->get_type_code()==vpiNet;
      তার্কিক full_sig = base == 0 && width == sig_size;

      vvp_net_ptr_t dest(rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value.  Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value.*/
      যদি (flags == vpiReleaseFlag) {
	    প্রতিজ্ঞা(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    যদি (full_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } অন্যথা {
		  rfp->net->fil->release_pv(dest, base, width, net_flag);
	    }
	    rfp->net->fun->force_flag(সত্য);
	    PV_get_মান(ref, vp);
	    প্রত্যাগম ref;
      }

      যদি (flags == vpiForceFlag) {
	    যদি (full_sig) {
		  vvp_vector2_t mask (vvp_vector2_t::FILL1, sig_size);
		  rfp->net->force_vec4(val, mask);
	    } অন্যথা {
		  vvp_vector2_t mask (vvp_vector2_t::FILL0, sig_size);
		  প্রতি (unsigned idx = 0 ; idx < width ; idx += 1)
		        mask.set_bit(base+idx, 1);

		  vvp_vector4_t tmp (sig_size, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_মান(tmp);

		  tmp.set_vec(base, val);
		  rfp->net->force_vec4(tmp, mask);
	    }
      } অন্যথা যদি (net_flag && !dynamic_cast<vvp_island_port*>(rfp->net->fun)) {
	    যদি (full_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } অন্যথা {
		  rfp->net->send_vec4_pv(val, base, sig_size,
		                         vthread_get_wt_context());
	    }
      } অন্যথা {
	    যদি (full_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } অন্যথা {
		  vvp_send_vec4_pv(dest, val, base, sig_size,
	                           vthread_get_wt_context());
	    }
      }
      প্রত্যাগম 0;
}

স্থির vpiHandle PV_get_handle(পূর্ণাঙ্ক code, vpiHandle ref)
{
      সংরচনা __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      প্রতিজ্ঞা(rfp);

      বিকল্প (code) {
	  পক্ষ vpiParent:
	    প্রত্যাগম rfp->parent;

	  পক্ষ vpiScope:
	    প্রত্যাগম vpi_handle(vpiScope, rfp->parent);

	  পক্ষ vpiModule:
	    প্রত্যাগম vpi_handle(vpiModule, rfp->parent);
      }

      প্রত্যাগম 0;
}

অন্তঃস্থ __vpiPV::__vpiPV()
{ }

পূর্ণাঙ্ক __vpiPV::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiPartSelect; }

পূর্ণাঙ্ক __vpiPV::vpi_get(পূর্ণাঙ্ক code)
{ প্রত্যাগম PV_get(code, এতত্); }

বর্ণ* __vpiPV::vpi_get_str(পূর্ণাঙ্ক code)
{ প্রত্যাগম PV_get_str(code, এতত্); }

শূন্য __vpiPV::vpi_get_মান(p_vpi_মান val)
{ PV_get_মান(এতত্, val); }

vpiHandle __vpiPV::vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags)
{ প্রত্যাগম PV_put_মান(এতত্, val, flags); }

vpiHandle __vpiPV::vpi_handle(পূর্ণাঙ্ক code)
{ প্রত্যাগম PV_get_handle(code, এতত্); }

vpiHandle vpip_make_PV(বর্ণ*var, পূর্ণাঙ্ক base, পূর্ণাঙ্ক width)
{
      সংরচনা __vpiPV*obj = নূতন __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = 0;
      obj->tbase = base;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      প্রত্যাগম obj;
}

vpiHandle vpip_make_PV(বর্ণ*var, বর্ণ*প্রতীক, পূর্ণাঙ্ক width)
{
      সংরচনা __vpiPV*obj = নূতন __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      compile_vpi_lookup(&obj->sbase, প্রতীক);
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      প্রত্যাগম obj;
}

vpiHandle vpip_make_PV(বর্ণ*var, vpiHandle handle, পূর্ণাঙ্ক width)
{
      সংরচনা __vpiPV*obj = নূতন __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      প্রত্যাগম obj;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য PV_delete(vpiHandle item)
{
      সংরচনা __vpiPV *obj = dynamic_cast<__vpiPV*>(item);
      যদি (obj->sbase) {
	    বিকল্প (obj->sbase->get_type_code()) {
		পক্ষ vpiMemoryWord:
		  যদি (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  ভঙ্গ;
		পক্ষ vpiPartSelect:
		  প্রতিজ্ঞা(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  ভঙ্গ;
	    }
      }
      প্রতিজ্ঞা(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      delete obj;
}
#endif
