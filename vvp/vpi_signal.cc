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
extern const var_na hex_digits[256];
extern const var_na oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
void *need_result_buf(unsigned cnt, vpi_rbuf_t type)
{
      static void*result_buf[2] = {0, 0};
      static size_t result_buf_size[2] = {0, 0};

      အကယ်၍ (type == RBUF_DEL) {
	    free(result_buf[RBUF_VAL]);
	    result_buf[RBUF_VAL] = 0;
	    result_buf_size[RBUF_VAL] = 0;

	    free(result_buf[RBUF_STR]);
	    result_buf[RBUF_STR] = 0;
	    result_buf_size[RBUF_STR] = 0;

	    ပြန်ပေး 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      အကယ်၍ (result_buf_size[type] == 0) {
	    result_buf[type] = malloc(cnt);
	    result_buf_size[type] = cnt;
      } မဟုတ်ပါက အကယ်၍ (result_buf_size[type] < cnt) {
	    result_buf[type] = realloc(result_buf[type], cnt);
	    result_buf_size[type] = cnt;
      }

      ပြန်ပေး result_buf[type];
}

var_na *simple_set_rbuf_str(const var_na *s1)
{
      var_na *res = static_cast<var_na *>(need_result_buf(strlen(s1)+1, RBUF_STR));
      အကယ်၍ (res) strcpy(res,s1);
      ပြန်ပေး res;
}

var_na *generic_get_str(ကိန်းပြည့် code, vpiHandle ref, const var_na *name, const var_na *index)
{
      size_t len = strlen(name) + 1;  /* include space for null termination */
      var_na *bn = NULL;
      bool is_bn_a_pkg = false;
      အကယ်၍ (code == vpiFullName) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = strlen(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    အကယ်၍ (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      အကယ်၍ (index != NULL) len += strlen(index) + 2;  /* include space for brackets */

      var_na *res = static_cast<var_na *>(need_result_buf(len, RBUF_STR));
      အကယ်၍ (!res) {
	    free(bn);
	    ပြန်ပေး NULL;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      အကယ်၍ (bn != NULL) {
	    strcat(res, bn);
	      // A package already has the "::" separator in the name
	    အကယ်၍ (! is_bn_a_pkg) strcat(res, ".");
	    free(bn);
      }
      strcat(res, name);
      အကယ်၍ (index != NULL) {
	    strcat(res, "[");
	    strcat(res, index);
	    strcat(res, "]");
      }
      ပြန်ပေး res;
}

static vpiHandle fill_in_net4(sa_nracanaa __vpiSignal*obj, __vpiScope*scope,
                              const var_na*name, ကိန်းပြည့် msb, ကိန်းပြည့် lsb,
                              bool signed_flag, vvp_net_t*node);

static vpiHandle fill_in_var4(sa_nracanaa __vpiSignal*obj,
                              const var_na*name, ကိန်းပြည့် msb, ကိန်းပြည့် lsb,
                              bool signed_flag, vvp_net_t*node)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      ပြန်ပေး fill_in_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, node);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

static void format_vpiBinStrVal(const vvp_signal_value*sig, ကိန်းပြည့် base, unsigned wid,
                                s_vpi_value*vp)
{
      var_na *rbuf = static_cast<var_na *>(need_result_buf(wid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long offset = end - 1;
      long ssize = (signed)sig->value_size();

      အတွက် (long idx = base ;  idx < end ;  idx += 1) {
	    အကယ်၍ (idx < 0 || idx >= ssize) {
                  rbuf[offset-idx] = 'x';
	    } မဟုတ်ပါက {
                  rbuf[offset-idx] = vvp_bit4_to_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

static void format_vpiOctStrVal(const vvp_signal_value*sig, ကိန်းပြည့် base, unsigned wid,
                                s_vpi_value*vp)
{
      unsigned dwid = (wid + 2) / 3;
      var_na *rbuf = static_cast<var_na *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->value_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      အတွက် (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    အကယ်၍ (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } မဟုတ်ပါက {
                  switch (sig->value(idx)) {
		      pak_sa BIT4_0:
			bit = 0;
			bha_nga;
		      pak_sa BIT4_1:
			bit = 1;
			bha_nga;
		      pak_sa BIT4_X:
			bit = 2;
			bha_nga;
		      pak_sa BIT4_Z:
			bit = 3;
			bha_nga;
                  }
	    }
	    val |= bit << 2*((idx-base) % 3);

	    အကယ်၍ ((idx-base) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      switch (wid % 3) {
	  pak_sa 1:
	    အကယ်၍ (val == 2) val = 42;
	    မဟုတ်ပါက အကယ်၍ (val == 3) val = 63;
	    bha_nga;
	  pak_sa 2:
	    အကယ်၍ (val == 10) val = 42;
	    မဟုတ်ပါက အကယ်၍ (val == 15) val = 63;
	    bha_nga;
      }

      အကယ်၍ (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

static void format_vpiHexStrVal(const vvp_signal_value*sig, ကိန်းပြည့် base, unsigned wid,
                                s_vpi_value*vp)
{
      unsigned dwid = (wid + 3) / 4;
      var_na *rbuf = static_cast<var_na *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->value_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      အတွက် (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    အကယ်၍ (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } မဟုတ်ပါက {
                  switch (sig->value(idx)) {
		      pak_sa BIT4_0:
			bit = 0;
			bha_nga;
		      pak_sa BIT4_1:
			bit = 1;
			bha_nga;
		      pak_sa BIT4_X:
			bit = 2;
			bha_nga;
		      pak_sa BIT4_Z:
			bit = 3;
			bha_nga;
                  }
	    }
	    val |= bit << 2*((idx-base) % 4);

	    အကယ်၍ ((idx-base) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      switch (wid % 4) {
	  pak_sa 1:
	    အကယ်၍ (val == 2) val = 170;
	    မဟုတ်ပါက အကယ်၍ (val == 3) val = 255;
	    bha_nga;
	  pak_sa 2:
	    အကယ်၍ (val == 10) val = 170;
	    မဟုတ်ပါက အကယ်၍ (val == 15) val = 255;
	    bha_nga;
	  pak_sa 3:
	    အကယ်၍ (val == 42) val = 170;
	    မဟုတ်ပါက အကယ်၍ (val == 63) val = 255;
	    bha_nga;
      }

      အကယ်၍ (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

static void format_vpiDecStrVal(const vvp_signal_value*sig, ကိန်းပြည့် base, unsigned wid,
                                ကိန်းပြည့် signed_flag, s_vpi_value*vp)
{
      unsigned hwid = (sig->value_size()+2) / 3 + 1;
      var_na *rbuf = static_cast<var_na *>(need_result_buf(hwid, RBUF_VAL));
      long ssize = (signed)sig->value_size();
      long end = base + (signed)wid;

	/* Do we have an end outside of the real signal vector. */
      အကယ်၍ (base < 0 || end > ssize) {
	    bool all_x = true;
	    အကယ်၍ (end > ssize) end = ssize;
	    အကယ်၍ (base < 0) base = 0;
	    အတွက် (long idx = base ;  idx < end ;  idx += 1) {
		  အကယ်၍ (sig->value(idx) != BIT4_X) {
			all_x = false;
			bha_nga;
		  }
	    }

	    အကယ်၍ (all_x) {
		  rbuf[0] = 'x';
	    } မဟုတ်ပါက {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    ပြန်ပေး;
      }

      vvp_vector4_t vec4;
      အကယ်၍ (base == 0 && end == ssize) {
	    sig->vec4_value(vec4);
      } မဟုတ်ပါက {
	    vvp_vector4_t tmp;
	    sig->vec4_value(tmp);
	    vec4 = tmp.subvalue(base, wid);
      }

      vpip_vec4_to_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

static void format_vpiIntVal(const vvp_signal_value*sig, ကိန်းပြည့် base, unsigned wid,
                             ကိန်းပြည့် signed_flag, s_vpi_value*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_value(tmp);
      vvp_vector4_t sub = tmp.subvalue(base, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      အကယ်၍ (parimaa_na(vp->value.integer) == parimaa_na(int32_t)) {
	    int32_t val = 0;
	    vector4_to_value(sub, val, signed_flag, false);
	    vp->value.integer = val;
      } မဟုတ်ပါက {
	    assert(parimaa_na(vp->value.integer) == parimaa_na(int64_t));
	    int64_t val = 0;
	    vector4_to_value(sub, val, signed_flag, false);
	    vp->value.integer = val;
      }
}

static void format_vpiRealVal(const vvp_signal_value*sig, ကိန်းပြည့် base, unsigned wid,
                              ကိန်းပြည့် signed_flag, s_vpi_value*vp)
{
      vvp_vector4_t vec4(wid);
      long ssize = (signed)sig->value_size();
      long end = base + (signed)wid;
      အကယ်၍ (end > ssize) end = ssize;

      အတွက် (long idx = (base < 0) ? 0 : base ;  idx < end ;  idx += 1) {
	    vec4.set_bit(idx-base, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_to_value(vec4, vp->value.real, signed_flag);
}

static void format_vpiStringVal(const vvp_signal_value*sig, ကိန်းပြည့် base, unsigned wid,
                                s_vpi_value*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      var_na *rbuf = static_cast<var_na *>(need_result_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      var_na *cp = rbuf;

      var_na tmp = 0;
      အတွက် (long idx = base+(signed)wid-1; idx >= base; idx -= 1) {
	    tmp <<= 1;

	    အကယ်၍ (idx >=0 && idx < (signed)sig->value_size() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    အကယ်၍ (((idx-base)&7)==0){
		  /* Skip leading nulls. */
		  အကယ်၍ (tmp == 0 && cp == rbuf)
			continue;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

static void format_vpiScalarVal(const vvp_signal_value*sig, ကိန်းပြည့် base,
                                s_vpi_value*vp)
{
      အကယ်၍ (base >= 0 && base < (signed)sig->value_size()) {
	    switch (sig->value(base)) {
		pak_sa BIT4_0:
		  vp->value.scalar = vpi0;
		  bha_nga;
		pak_sa BIT4_1:
		  vp->value.scalar = vpi1;
		  bha_nga;
		pak_sa BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_value(base);
		  အကယ်၍ (strn.strength0() == 1) vp->value.scalar = vpiH;
		  မဟုတ်ပါက အကယ်၍ (strn.strength1() == 1) vp->value.scalar = vpiL;
		  မဟုတ်ပါက vp->value.scalar = vpiX;
		  bha_nga;
		}
		pak_sa BIT4_Z:
		  vp->value.scalar = vpiZ;
		  bha_nga;
	    }
      } မဟုတ်ပါက {
	    vp->value.scalar = vpiX;
      }
}

static void format_vpiStrengthVal(const vvp_signal_value*sig, ကိန်းပြည့် base,
                                  unsigned wid, s_vpi_value*vp)
{
      long end = base + (signed)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_result_buf(wid * parimaa_na(s_vpi_strengthval), RBUF_VAL));

      အတွက် (long idx = base ;  idx < end ;  idx += 1) {
	    အကယ်၍ (idx >=0 && idx < (signed)sig->value_size()) {
		  vvp_scalar_t val = sig->scalar_value(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  unsigned s0 = 1 << val.strength0();
		  unsigned s1 = 1 << val.strength1();

		  switch (val.value()) {
		      pak_sa BIT4_0:
			op[idx-base].logic = vpi0;
			op[idx-base].s0 = s0|s1;
			op[idx-base].s1 = 0;
			bha_nga;

		      pak_sa BIT4_1:
			op[idx-base].logic = vpi1;
			op[idx-base].s0 = 0;
			op[idx-base].s1 = s0|s1;
			bha_nga;

		      pak_sa BIT4_X:
			op[idx-base].logic = vpiX;
			op[idx-base].s0 = s0;
			op[idx-base].s1 = s1;
			bha_nga;

		      pak_sa BIT4_Z:
			op[idx-base].logic = vpiZ;
			op[idx-base].s0 = vpiHiZ;
			op[idx-base].s1 = vpiHiZ;
			bha_nga;
		  }
	    } မဟုတ်ပါက {
		  op[idx-base].logic = vpiX;
		  op[idx-base].s0 = vpiStrongDrive;
		  op[idx-base].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

static void format_vpiVectorVal(const vvp_signal_value*sig, ကိန်းပြည့် base, unsigned wid,
                                s_vpi_value*vp)
{
      long end = base + (signed)wid;
      unsigned ကိန်းပြည့် obit = 0;
      unsigned hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_result_buf(hwid * parimaa_na(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      အတွက် (long idx = base ;  idx < end ;  idx += 1) {
	    အကယ်၍ (base >= 0 && base < (signed)sig->value_size()) {
		switch (sig->value(idx)) {
		pak_sa BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  bha_nga;
		pak_sa BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  bha_nga;
		pak_sa BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  bha_nga;
		pak_sa BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  bha_nga;
		}
	    } မဟုတ်ပါက {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    အကယ်၍ (!(obit % 32)) {
		  op += 1;
		  အကယ်၍ ((op - vp->value.vector) < (ptrdiff_t)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
static ကိန်းပြည့် signal_get(ကိန်းပြည့် code, vpiHandle ref)
{
      sa_nracanaa __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);

      switch (code) {
	  pak_sa vpiLineNo:
	    ပြန်ပေး 0;  // Not implemented for now!

	  pak_sa vpiSigned:
	    ပြန်ပေး rfp->signed_flag != 0;

	  pak_sa vpiArray:
	    ပြန်ပေး rfp->is_netarray != 0;

	  pak_sa vpiIndex: // This only works while we have a single index.
	    အကယ်၍ (rfp->is_netarray) {
		  s_vpi_value vp;
		  vp.format = vpiIntVal;
		  vpi_get_value(rfp->id.index, &vp);
		  ပြန်ပေး vp.value.integer;
	    } မဟုတ်ပါက {
		  ပြန်ပေး vpiUndefined;
	    }

	  pak_sa vpiSize:
	    ပြန်ပေး rfp->width();

	  pak_sa vpiNetType:
	    အကယ်၍ (ref->get_type_code()==vpiNet)
		  ပြန်ပေး vpiWire;
	    မဟုတ်ပါက
		  ပြန်ပေး vpiUndefined;

	  pak_sa vpiLeftRange:
            ပြန်ပေး rfp->msb.get_value();

	  pak_sa vpiRightRange:
            ပြန်ပေး rfp->lsb.get_value();

	  pak_sa vpiScalar:
	    ပြန်ပေး (rfp->msb.get_value() == 0 && rfp->lsb.get_value() == 0);
	  pak_sa vpiVector:
	    ပြန်ပေး (rfp->msb.get_value() != rfp->lsb.get_value());

          pak_sa vpiAutomatic:
            ပြန်ပေး vpip_scope(rfp)->is_automatic() ? 1  : 0;

#ifdef BR916_STOPGAP_FIX
          pak_sa _vpiFromThr:
            ပြန်ပေး _vpiNoThr;
#endif

	  default:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    ပြန်ပေး vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
static int64_t signal_get64(ကိန်းပြည့် code, vpiHandle ref)
{
      sa_nracanaa __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);

      switch (code) {
	    // This private property must return zero when undefined.
	  pak_sa _vpiNexusId:
	    အကယ်၍ (rfp->msb.get_value() == rfp->lsb.get_value())
		  ပြန်ပေး reinterpret_cast<int64_t>(rfp->node);
	    ပြန်ပေး 0;

	  default:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    ပြန်ပေး vpiUndefined;
      }
}

static var_na* signal_get_str(ကိန်းပြည့် code, vpiHandle ref)
{
      sa_nracanaa __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);

      အကယ်၍ (code == vpiFile) {  // Not implemented for now!
	    ပြန်ပေး simple_set_rbuf_str(file_names[0]);
      }

      အကယ်၍ ((code != vpiName) && (code != vpiFullName)) ပြန်ပေး NULL;

      var_na *nm;
      const var_na *ixs;
      အကယ်၍ (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_value vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_value(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } မဟုတ်ပါက {
	    nm = strdup(rfp->id.name);
	    ixs = NULL;
      }

	/* The scope information is added here for vpiFullName. */
      var_na *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      free(nm);
      ပြန်ပေး rbuf;
}

static vpiHandle signal_get_handle(ကိန်းပြည့် code, vpiHandle ref)
{
      sa_nracanaa __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);

      switch (code) {

	  pak_sa vpiParent:
	    ပြန်ပေး rfp->is_netarray? rfp->within.parent : NULL;

	  pak_sa vpiIndex:
	    ပြန်ပေး rfp->is_netarray? rfp->id.index : NULL;

	  pak_sa vpiLeftRange:
	    ပြန်ပေး &rfp->msb;
	  pak_sa vpiRightRange:
	    ပြန်ပေး &rfp->lsb;

	  pak_sa vpiScope:
	    ပြန်ပေး vpip_scope(rfp);

	  pak_sa vpiModule:
	    ပြန်ပေး vpip_module(vpip_scope(rfp));
      }

      ပြန်ပေး 0;
}

static vpiHandle signal_iterate(ကိန်းပြည့် code, vpiHandle ref)
{
      sa_nracanaa __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);

      အကယ်၍ (code == vpiIndex) {
	    ပြန်ပေး rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : NULL;
      }

      ပြန်ပေး 0;
}

void __vpiSignal::make_bits()
{
      assert(bits == NULL);
      bits = new sa_nracanaa __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      bits->parent = this;
	// Now point to word[0].
      bits += 1;

	// Update each bit to point to the base
      sa_nracanaa __vpiBit*base = bits;
      အတွက် (unsigned idx = 0; idx < width(); idx += 1) {
	    base[idx].bit0 = base;
	    ကိန်းပြည့် real_idx;
	    အကယ်၍ (msb.get_value() >= lsb.get_value()) {
		  real_idx = idx + lsb.get_value();
	    } မဟုတ်ပါက {
		  real_idx = lsb.get_value() - idx;
	    }
	    base[idx].index = new __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_index(ကိန်းပြည့် idx)
{
	/* Check to see if the index is in range. */
      အကယ်၍ (msb.get_value() >= lsb.get_value()) {
	    အကယ်၍ ((idx > msb.get_value()) || (idx < lsb.get_value())) ပြန်ပေး 0;
      } မဟုတ်ပါက {
	    အကယ်၍ ((idx < msb.get_value()) || (idx > lsb.get_value())) ပြန်ပေး 0;
      }

	/* Normalize the index */
      unsigned norm_idx;
      အကယ်၍ (msb.get_value() >= lsb.get_value()) {
	    norm_idx = idx - lsb.get_value();
      } မဟုတ်ပါက {
	    norm_idx = lsb.get_value() - idx;
      }

      အကယ်၍ (bits == NULL) make_bits();

      ပြန်ပေး &(bits[norm_idx].as_bit);
}

void __vpiSignal::get_bit_value(const sa_nracanaa __vpiBit*bit, p_vpi_value vp)
{
      unsigned index = bit->get_norm_index();

      vvp_signal_value*vsig = dynamic_cast<vvp_signal_value*>(node->fil);
      assert(vsig);

      အကယ်၍ (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      switch (vp->format) {
	pak_sa vpiBinStrVal:
            format_vpiBinStrVal(vsig, index, 1, vp);
	    bha_nga;

	pak_sa vpiOctStrVal:
            format_vpiOctStrVal(vsig, index, 1, vp);
	    bha_nga;

	pak_sa vpiDecStrVal:
            format_vpiDecStrVal(vsig, index, 1, false, vp);
	    bha_nga;

	pak_sa vpiHexStrVal:
            format_vpiHexStrVal(vsig, index, 1, vp);
	    bha_nga;

	pak_sa vpiStringVal:
            format_vpiStringVal(vsig, index, 1, vp);
	    bha_nga;

	pak_sa vpiIntVal:
            format_vpiIntVal(vsig, index, 1, false, vp);
	    bha_nga;

	pak_sa vpiRealVal:
            format_vpiRealVal(vsig, index, 1, false, vp);
	    bha_nga;

	pak_sa vpiScalarVal:
            format_vpiScalarVal(vsig, index, vp);
	    bha_nga;

        pak_sa vpiStrengthVal:
            format_vpiStrengthVal(vsig, index, 1, vp);
            bha_nga;

	pak_sa vpiVectorVal:
            format_vpiVectorVal(vsig, index, 1, vp);
	    bha_nga;

	default:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (ကိန်းပြည့်)vp->format, vpi_get_str(vpiName),
		    bit->get_index(),
	            vpip_scope(this)->scope_name());
	    assert(0);
      };
}

vpiHandle __vpiSignal::put_bit_value(sa_nracanaa __vpiBit*bit, p_vpi_value vp, ကိန်းပြည့် flags)
{
      unsigned index = bit->get_norm_index();
      vvp_net_ptr_t dest(node, 0);
      vvp_vector4_t val = vec4_from_vpi_value(vp, 1);

      အကယ်၍ ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    bit->as_bit.vpi_get_str(vpiFullName));
	    ပြန်ပေး NULL;
      }

      အကယ်၍ ((get_type_code() == vpiNet) &&
          !dynamic_cast<vvp_island_port*>(node->fun)) {
	    node->send_vec4_pv(val, index, width(),
	                            vthread_get_wt_context());
      } မဟုတ်ပါက {
	    vvp_send_vec4_pv(dest, val, index, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      ပြန်ပေး NULL;
}

static vpiHandle signal_index(ကိန်းပြည့် idx, vpiHandle ref)
{
      sa_nracanaa __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      အကယ်၍ ((type != vpiNet) && (type != vpiReg)) ပြန်ပေး 0;

      ပြန်ပေး rfp->get_index(idx);
}

unsigned __vpiSignal::width(void) const
{
      unsigned wid = (msb.get_value() >= lsb.get_value())
	    ? (msb.get_value() - lsb.get_value() + 1)
	    : (lsb.get_value() - msb.get_value() + 1);

      ပြန်ပေး wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
static void signal_get_value(vpiHandle ref, s_vpi_value*vp)
{
      sa_nracanaa __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);

      unsigned wid = rfp->width();

      vvp_signal_value*vsig = dynamic_cast<vvp_signal_value*>(rfp->node->fil);
      assert(vsig);

      switch (vp->format) {

	  pak_sa vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    bha_nga;

	  pak_sa vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    bha_nga;

	  pak_sa vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    bha_nga;

	  pak_sa vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    bha_nga;

	  pak_sa vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    bha_nga;

	  pak_sa vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    bha_nga;

	  pak_sa vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    bha_nga;

	  pak_sa vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    bha_nga;

	  pak_sa vpiVectorVal:
	    format_vpiVectorVal(vsig, 0, wid, vp);
	    bha_nga;

	  pak_sa vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    bha_nga;

	  pak_sa vpiObjTypeVal:
	    အကယ်၍ (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } မဟုတ်ပါက {
		  vp->format = vpiVectorVal;
		  format_vpiVectorVal(vsig, 0, wid, vp);
	    }
	    bha_nga;

	  default:
	    fprintf(stderr, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (ကိန်းပြည့်)vp->format, vpi_get_str(vpiName, ref),
		    vpip_scope(rfp)->scope_name());
	    assert(0);
      }
}

/*
 * The put_value method writes the value into the vector, and returns
 * the affected ref. This operation works much like the %set or
 * %assign instructions and causes all the side-effects that the
 * equivalent instruction would cause.
 */

static vvp_vector4_t from_stringval(const var_na*str, unsigned wid)
{
      unsigned idx;
      const var_na*cp;

      cp = str + strlen(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      နေစဉ် ((idx < wid) && (cp > str)) {
	    unsigned byte = *--cp;
	    ကိန်းပြည့် bit;

	    အတွက် (bit = 0 ;  (bit < 8) && (idx < wid) ;  bit += 1) {
		  အကယ်၍ (byte & 1)
			val.set_bit(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      ပြန်ပေး val;
}

static vpiHandle signal_put_value(vpiHandle ref, s_vpi_value*vp, ကိန်းပြည့် flags)
{
      unsigned wid;
      sa_nracanaa __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);
      vvp_net_ptr_t dest(rfp->node, 0);

      bool net_flag = ref->get_type_code()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      အကယ်၍ (flags == vpiReleaseFlag) {
	    assert(rfp->node->fil);
	    rfp->node->fil->force_unlink();
	    rfp->node->fil->release(dest, net_flag);
	    rfp->node->fun->force_flag(true);
	    signal_get_value(ref, vp);
	    ပြန်ပေး ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_value() >= rfp->lsb.get_value())
	    ? (rfp->msb.get_value() - rfp->lsb.get_value() + 1)
	    : (rfp->lsb.get_value() - rfp->msb.get_value() + 1);

      vvp_vector4_t val = vec4_from_vpi_value(vp, wid);

      အကယ်၍ (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, wid);
	    rfp->node->force_vec4(val, mask);
      } မဟုတ်ပါက အကယ်၍ (net_flag && !dynamic_cast<vvp_island_port*>(rfp->node->fun)) {
	    rfp->node->send_vec4(val, vthread_get_wt_context());
      } မဟုတ်ပါက {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      ပြန်ပေး ref;
}

vvp_vector4_t vec4_from_vpi_value(s_vpi_value*vp, unsigned wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      switch (vp->format) {

	  pak_sa vpiIntVal: {
		long vpi_val = vp->value.integer;
		အတွက် (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t bit = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.set_bit(idx, bit);
		      vpi_val >>= 1;
		}
		bha_nga;
	  }

	  pak_sa vpiVectorVal:
	    အတွက် (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long aval = vp->value.vector[idx/32].aval;
		  unsigned long bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  ကိန်းပြည့် bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t bit = scalar_to_bit4(bitmask);
		  val.set_bit(idx, bit);
	    }
	    bha_nga;
	  pak_sa vpiBinStrVal:
	    vpip_bin_str_to_vec4(val, vp->value.str);
	    bha_nga;
	  pak_sa vpiOctStrVal:
	    vpip_oct_str_to_vec4(val, vp->value.str);
	    bha_nga;
	  pak_sa vpiDecStrVal:
	    vpip_dec_str_to_vec4(val, vp->value.str);
	    bha_nga;
	  pak_sa vpiHexStrVal:
	    vpip_hex_str_to_vec4(val, vp->value.str);
	    bha_nga;
	  pak_sa vpiScalarVal:
	    val.set_bit(0, scalar_to_bit4(vp->value.scalar));
	    bha_nga;
	  pak_sa vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    bha_nga;
	  pak_sa vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    bha_nga;

	  default:
	    fprintf(stderr, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (ကိန်းပြည့်)vp->format);
	    assert(0);

      }

      ပြန်ပေး val;
}

ကိန်းပြည့် __vpiSignal::vpi_get(ကိန်းပြည့် code)
{ ပြန်ပေး signal_get(code, this); }

int64_t __vpiSignal::vpi_get64(ကိန်းပြည့် code)
{ ပြန်ပေး signal_get64(code, this); }

var_na* __vpiSignal::vpi_get_str(ကိန်းပြည့် code)
{ ပြန်ပေး signal_get_str(code, this); }

void __vpiSignal::vpi_get_value(p_vpi_value val)
{ signal_get_value(this, val); }

vpiHandle __vpiSignal::vpi_put_value(p_vpi_value val, ကိန်းပြည့် flags)
{ ပြန်ပေး signal_put_value(this, val, flags); }

vpiHandle __vpiSignal::vpi_handle(ကိန်းပြည့် code)
{ ပြန်ပေး signal_get_handle(code, this); }

vpiHandle __vpiSignal::vpi_iterate(ကိန်းပြည့် code)
{ ပြန်ပေး signal_iterate(code, this); }

vpiHandle __vpiSignal::vpi_index(ကိန်းပြည့် idx)
{ ပြန်ပေး signal_index(idx, this); }

sa_nracanaa signal_reg : public __vpiSignal {
      anta_hstha signal_reg() { }
      ကိန်းပြည့် get_type_code(void) const override { ပြန်ပေး vpiReg; }
};

sa_nracanaa signal_integer : public __vpiSignal {
      anta_hstha signal_integer() { }
      ကိန်းပြည့် get_type_code(void) const override { ပြန်ပေး vpiIntegerVar; }
};

sa_nracanaa signal_net : public __vpiSignal {
      anta_hstha signal_net() { }
      ကိန်းပြည့် get_type_code(void) const override { ပြန်ပေး vpiNet; }
};

sa_nracanaa signal_byte : public __vpiSignal {
      anta_hstha signal_byte() { }
      ကိန်းပြည့် get_type_code(void) const override { ပြန်ပေး vpiByteVar; }
};

sa_nracanaa signal_bitvar : public __vpiSignal {
      anta_hstha signal_bitvar() { }
      ကိန်းပြည့် get_type_code(void) const override { ပြန်ပေး vpiBitVar; }
};

sa_nracanaa signal_shortint : public __vpiSignal {
      anta_hstha signal_shortint() { }
      ကိန်းပြည့် get_type_code(void) const override { ပြန်ပေး vpiShortIntVar; }
};

sa_nracanaa signal_int : public __vpiSignal {
      anta_hstha signal_int() { }
      ကိန်းပြည့် get_type_code(void) const override { ပြန်ပေး vpiIntVar; }
};

sa_nracanaa signal_longint : public __vpiSignal {
      anta_hstha signal_longint() { }
      ကိန်းပြည့် get_type_code(void) const override { ပြန်ပေး vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(const var_na*name, ကိန်းပြည့် msb, ကိန်းပြည့် lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = new signal_integer;
      ပြန်ပေး fill_in_var4(obj, name, msb, lsb, true, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(const var_na*name, ကိန်းပြည့် msb, ကိန်းပြည့် lsb, bool signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      အကယ်၍ ((! signed_flag) || (lsb != 0) ) {
	    obj = new signal_bitvar;
      } မဟုတ်ပါက {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    switch (msb) {
		pak_sa 7:
		  obj = new signal_byte;
		  bha_nga;
		pak_sa 15:
		  obj = new signal_shortint;
		  bha_nga;
		pak_sa 31:
		  obj = new signal_int;
		  bha_nga;
		pak_sa 63:
		  obj = new signal_longint;
		  bha_nga;
		default:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = new signal_bitvar;
		  bha_nga;
	    }
      }

      ပြန်ပေး fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(const var_na*name, ကိန်းပြည့် msb, ကိန်းပြည့် lsb,
			bool signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = new signal_reg;
      ပြန်ပေး fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

#ifdef CHECK_WITH_VALGRIND
static sa_nracanaa vpiSignal_plug **signal_pool = 0;
static unsigned signal_pool_count = 0;
static unsigned long signal_count = 0;
static unsigned long signal_dels = 0;
#endif

sa_nracanaa vpiSignal_plug {
      unsigned var_na space[parimaa_na (sa_nracanaa __vpiSignal)];
#ifdef CHECK_WITH_VALGRIND
      sa_nracanaa vpiSignal_plug *pool;
#endif
};

void* __vpiSignal::operator new(size_t siz)
{
#ifdef CHECK_WITH_VALGRIND
      assert(siz == parimaa_na(sa_nracanaa vpiSignal_plug) - parimaa_na(sa_nracanaa vpiSignal_plug *));
#မဟုတ်ပါက
      assert(siz == parimaa_na(sa_nracanaa vpiSignal_plug));
#endif
      static sa_nracanaa vpiSignal_plug*alloc_array = 0;
      static unsigned alloc_index = 0;
      const unsigned alloc_count = 512;

      အကယ်၍ ((alloc_array == 0) || (alloc_index == alloc_count)) {
	    alloc_array = static_cast<sa_nracanaa vpiSignal_plug*>
	                  (calloc(alloc_count, parimaa_na(sa_nracanaa vpiSignal_plug)));
	    alloc_index = 0;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_count *
	                                            parimaa_na(sa_nracanaa vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_count += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_count*parimaa_na(vpiSignal_plug **)));
	    signal_pool[signal_pool_count-1] = alloc_array;
#endif
      }

      sa_nracanaa vpiSignal_plug*cur = alloc_array + alloc_index;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, parimaa_na(sa_nracanaa vpiSignal_plug));
      cur->pool = alloc_array;
      signal_count += 1;
#endif
      alloc_index += 1;
      ပြန်ပေး cur;
}

void __vpiSignal::operator delete(void*)
{
      assert(0);
}

#ifdef CHECK_WITH_VALGRIND
void signal_delete(vpiHandle item)
{
      sa_nracanaa __vpiSignal *obj = static_cast<__vpiSignal *> (item);
      assert(obj->node->fil);
      obj->node->fil->clear_all_callbacks();
      vvp_net_delete(obj->node);
      အကယ်၍ (obj->bits) {
	    အတွက် (unsigned idx=0; idx<obj->width(); idx+=1) {
		  delete obj->bits[idx].index;
	    }
	    obj->bits -= 1;
	    delete [] obj->bits;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

void signal_pool_delete()
{
      အကယ်၍ (RUNNING_ON_VALGRIND && (signal_count != signal_dels)) {
	    fflush(NULL);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (long) signal_count - signal_dels, signal_count);
      }

      အတွက် (unsigned idx = 0; idx < signal_pool_count; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    free(signal_pool[idx]);
      }

      free(signal_pool);
      signal_pool = 0;
      signal_pool_count = 0;
}
#endif

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
static vpiHandle fill_in_net4(sa_nracanaa __vpiSignal*obj, __vpiScope*scope,
			      const var_na*name, ကိန်းပြည့် msb, ကိန်းပြည့် lsb,
			      bool signed_flag, vvp_net_t*node)
{
      obj->id.name = name? vpip_name_string(name) : NULL;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->node = node;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      count_vpi_nets += 1;

      ပြန်ပေး obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 const var_na*name, ကိန်းပြည့် msb, ကိန်းပြည့် lsb,
			 bool signed_flag, vvp_net_t*node)
{
      sa_nracanaa __vpiSignal*obj = new signal_net;
      ပြန်ပေး fill_in_net4(obj, scope, name, msb, lsb, signed_flag, node);
}

static ကိန်းပြည့် PV_get_base(sa_nracanaa __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      အကယ်၍ (rfp->sbase != 0) {
	    s_vpi_value val;
	      /* Check to see if the value is defined. */
	    val.format = vpiVectorVal;
	    vpi_get_value(rfp->sbase, &val);
	    ကိန်းပြည့် words = (vpi_get(vpiSize, rfp->sbase)-1)/32 + 1;
	    အတွက်(ကိန်းပြည့် idx = 0; idx < words; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  အကယ်၍ (val.value.vector[idx].bval != 0) ပြန်ပေး INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_value(rfp->sbase, &val);
	    ပြန်ပေး val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      ပြန်ပေး rfp->tbase;
}

static ကိန်းပြည့် PV_get(ကိန်းပြည့် code, vpiHandle ref)
{
      sa_nracanaa __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      assert(rfp);

      ကိန်းပြည့် rval = 0;
      switch (code) {
	pak_sa vpiLineNo:
	    ပြန်ပေး 0;  // Not implemented for now!

	pak_sa vpiSigned:
	    ပြန်ပေး 0;  // A part/bit select is always unsigned!

	pak_sa vpiSize:
	    ပြန်ပေး rfp->width;

	  /* This is like the &A<> in array.cc. */
	pak_sa vpiConstantSelect:
	    ပြန်ပေး rfp->sbase == 0;

	pak_sa vpiLeftRange:
            rval += rfp->width - 1;
	    // fallthrough
	pak_sa vpiRightRange: {
	    ကိန်းပြည့် left = vpi_get(vpiLeftRange, rfp->parent);
	    ကိန်းပြည့် right = vpi_get(vpiRightRange, rfp->parent);
	    rval += PV_get_base(rfp);
	    ပြန်ပေး left >= right ? right + rval : right - rval;
	}

        pak_sa vpiAutomatic:
            ပြန်ပေး vpi_get(vpiAutomatic, rfp->parent);

#အကယ်၍ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        pak_sa _vpiFromThr:
            ပြန်ပေး _vpi_at_PV;
#endif

	default:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      ပြန်ပေး 0;
}

static var_na* PV_get_str(ကိန်းပြည့် code, vpiHandle ref)
{
      sa_nracanaa __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      assert(rfp);

      switch (code) {
	pak_sa vpiFile:  // Not implemented for now!
	    ပြန်ပေး simple_set_rbuf_str(file_names[0]);

	pak_sa vpiName:
	pak_sa vpiFullName: {
	    const var_na*nm = vpi_get_str(code, rfp->parent);
	    size_t len = 256+strlen(nm);
	    var_na *full = static_cast<var_na *>(malloc(len));
	    snprintf(full, len, "%s[%d:%d]", nm,
	                                     (ကိန်းပြည့်)vpi_get(vpiLeftRange, ref),
	                                     (ကိန်းပြည့်)vpi_get(vpiRightRange, ref));
	    full[len-1] = 0;
	    var_na *res = simple_set_rbuf_str(full);
	    free(full);
	    ပြန်ပေး res;
	}

	default:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      ပြန်ပေး 0;
}

static void PV_get_value(vpiHandle ref, p_vpi_value vp)
{
      sa_nracanaa __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      assert(rfp);

      vvp_signal_value*sig = dynamic_cast<vvp_signal_value*>(rfp->net->fil);
      assert(sig);

      switch (vp->format) {

	  pak_sa vpiIntVal:
	    format_vpiIntVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    bha_nga;

	  pak_sa vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    bha_nga;

	  pak_sa vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    bha_nga;

	  pak_sa vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    bha_nga;

	  pak_sa vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    bha_nga;

	  pak_sa vpiStringVal:
	    format_vpiStringVal(sig, PV_get_base(rfp), rfp->width, vp);
	    bha_nga;

	  pak_sa vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_base(rfp), vp);
	    bha_nga;

	  pak_sa vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_base(rfp), rfp->width, vp);
	    bha_nga;

	  pak_sa vpiVectorVal:
	    format_vpiVectorVal(sig, PV_get_base(rfp), rfp->width, vp);
	    bha_nga;

	  pak_sa vpiRealVal:
	    format_vpiRealVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    bha_nga;

	  default:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (ကိန်းပြည့်)vp->format, vpi_get_str(vpiFullName, rfp->parent));
	    assert(0);
      }
}

static vpiHandle PV_put_value(vpiHandle ref, p_vpi_value vp, ကိန်းပြည့် flags)
{
      sa_nracanaa __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      assert(rfp);
      vvp_signal_value*sig = dynamic_cast<vvp_signal_value*>(rfp->net->fil);
      assert(sig);

      unsigned sig_size = sig->value_size();
      unsigned width = rfp->width;
      ကိန်းပြည့် base = PV_get_base(rfp);
      အကယ်၍ (base >= (signed) sig_size) ပြန်ပေး 0;
      အကယ်၍ (base + (signed) width < 0) ပြန်ပေး 0;

      vvp_vector4_t val;
      အကယ်၍ (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_value(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      အကယ်၍ (base < 0) {
	    width += base;
	    အကယ်၍ (flags != vpiReleaseFlag) {
		  val = val.subvalue(-base, width);
	    }
	    base = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      အကယ်၍ (base+width > sig_size) {
	    width = sig_size - base;
	    အကယ်၍ (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      assert(rfp->parent);
      bool net_flag = rfp->parent->get_type_code()==vpiNet;
      bool full_sig = base == 0 && width == sig_size;

      vvp_net_ptr_t dest(rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value.  Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value.*/
      အကယ်၍ (flags == vpiReleaseFlag) {
	    assert(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    အကယ်၍ (full_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } မဟုတ်ပါက {
		  rfp->net->fil->release_pv(dest, base, width, net_flag);
	    }
	    rfp->net->fun->force_flag(true);
	    PV_get_value(ref, vp);
	    ပြန်ပေး ref;
      }

      အကယ်၍ (flags == vpiForceFlag) {
	    အကယ်၍ (full_sig) {
		  vvp_vector2_t mask (vvp_vector2_t::FILL1, sig_size);
		  rfp->net->force_vec4(val, mask);
	    } မဟုတ်ပါက {
		  vvp_vector2_t mask (vvp_vector2_t::FILL0, sig_size);
		  အတွက် (unsigned idx = 0 ; idx < width ; idx += 1)
		        mask.set_bit(base+idx, 1);

		  vvp_vector4_t tmp (sig_size, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_value(tmp);

		  tmp.set_vec(base, val);
		  rfp->net->force_vec4(tmp, mask);
	    }
      } မဟုတ်ပါက အကယ်၍ (net_flag && !dynamic_cast<vvp_island_port*>(rfp->net->fun)) {
	    အကယ်၍ (full_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } မဟုတ်ပါက {
		  rfp->net->send_vec4_pv(val, base, sig_size,
		                         vthread_get_wt_context());
	    }
      } မဟုတ်ပါက {
	    အကယ်၍ (full_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } မဟုတ်ပါက {
		  vvp_send_vec4_pv(dest, val, base, sig_size,
	                           vthread_get_wt_context());
	    }
      }
      ပြန်ပေး 0;
}

static vpiHandle PV_get_handle(ကိန်းပြည့် code, vpiHandle ref)
{
      sa_nracanaa __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      assert(rfp);

      switch (code) {
	  pak_sa vpiParent:
	    ပြန်ပေး rfp->parent;

	  pak_sa vpiScope:
	    ပြန်ပေး vpi_handle(vpiScope, rfp->parent);

	  pak_sa vpiModule:
	    ပြန်ပေး vpi_handle(vpiModule, rfp->parent);
      }

      ပြန်ပေး 0;
}

anta_hstha __vpiPV::__vpiPV()
{ }

ကိန်းပြည့် __vpiPV::get_type_code(void) const
{ ပြန်ပေး vpiPartSelect; }

ကိန်းပြည့် __vpiPV::vpi_get(ကိန်းပြည့် code)
{ ပြန်ပေး PV_get(code, this); }

var_na* __vpiPV::vpi_get_str(ကိန်းပြည့် code)
{ ပြန်ပေး PV_get_str(code, this); }

void __vpiPV::vpi_get_value(p_vpi_value val)
{ PV_get_value(this, val); }

vpiHandle __vpiPV::vpi_put_value(p_vpi_value val, ကိန်းပြည့် flags)
{ ပြန်ပေး PV_put_value(this, val, flags); }

vpiHandle __vpiPV::vpi_handle(ကိန်းပြည့် code)
{ ပြန်ပေး PV_get_handle(code, this); }

vpiHandle vpip_make_PV(var_na*var, ကိန်းပြည့် base, ကိန်းပြည့် width)
{
      sa_nracanaa __vpiPV*obj = new __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = 0;
      obj->tbase = base;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ပြန်ပေး obj;
}

vpiHandle vpip_make_PV(var_na*var, var_na*symbol, ကိန်းပြည့် width)
{
      sa_nracanaa __vpiPV*obj = new __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      compile_vpi_lookup(&obj->sbase, symbol);
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ပြန်ပေး obj;
}

vpiHandle vpip_make_PV(var_na*var, vpiHandle handle, ကိန်းပြည့် width)
{
      sa_nracanaa __vpiPV*obj = new __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      ပြန်ပေး obj;
}

#ifdef CHECK_WITH_VALGRIND
void PV_delete(vpiHandle item)
{
      sa_nracanaa __vpiPV *obj = dynamic_cast<__vpiPV*>(item);
      အကယ်၍ (obj->sbase) {
	    switch (obj->sbase->get_type_code()) {
		pak_sa vpiMemoryWord:
		  အကယ်၍ (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  bha_nga;
		pak_sa vpiPartSelect:
		  assert(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  bha_nga;
	    }
      }
      assert(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      delete obj;
}
#endif
