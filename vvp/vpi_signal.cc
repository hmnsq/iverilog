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
extern const char hex_digits[256];
extern const char oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
void *need_result_buf(unsigned cnt, vpi_rbuf_t type)
{
      static void*result_buf[2] = {0, 0};
      static size_t result_buf_size[2] = {0, 0};

      ако (type == RBUF_DEL) {
	    free(result_buf[RBUF_VAL]);
	    result_buf[RBUF_VAL] = 0;
	    result_buf_size[RBUF_VAL] = 0;

	    free(result_buf[RBUF_STR]);
	    result_buf[RBUF_STR] = 0;
	    result_buf_size[RBUF_STR] = 0;

	    врати 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      ако (result_buf_size[type] == 0) {
	    result_buf[type] = malloc(cnt);
	    result_buf_size[type] = cnt;
      } инако ако (result_buf_size[type] < cnt) {
	    result_buf[type] = realloc(result_buf[type], cnt);
	    result_buf_size[type] = cnt;
      }

      врати result_buf[type];
}

char *simple_set_rbuf_str(const char *s1)
{
      char *res = static_cast<char *>(need_result_buf(strlen(s1)+1, RBUF_STR));
      ако (res) strcpy(res,s1);
      врати res;
}

char *generic_get_str(цео code, vpiHandle ref, const char *name, const char *index)
{
      size_t len = strlen(name) + 1;  /* include space for null termination */
      char *bn = NULL;
      bool is_bn_a_pkg = false;
      ако (code == vpiFullName) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = strlen(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    ако (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      ако (index != NULL) len += strlen(index) + 2;  /* include space for brackets */

      char *res = static_cast<char *>(need_result_buf(len, RBUF_STR));
      ако (!res) {
	    free(bn);
	    врати NULL;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      ако (bn != NULL) {
	    strcat(res, bn);
	      // A package already has the "::" separator in the name
	    ако (! is_bn_a_pkg) strcat(res, ".");
	    free(bn);
      }
      strcat(res, name);
      ако (index != NULL) {
	    strcat(res, "[");
	    strcat(res, index);
	    strcat(res, "]");
      }
      врати res;
}

static vpiHandle fill_in_net4(struct __vpiSignal*obj, __vpiScope*scope,
                              const char*name, цео msb, цео lsb,
                              bool signed_flag, vvp_net_t*node);

static vpiHandle fill_in_var4(struct __vpiSignal*obj,
                              const char*name, цео msb, цео lsb,
                              bool signed_flag, vvp_net_t*node)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      врати fill_in_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, node);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

static void format_vpiBinStrVal(const vvp_signal_value*sig, цео base, unsigned wid,
                                s_vpi_value*vp)
{
      char *rbuf = static_cast<char *>(need_result_buf(wid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long offset = end - 1;
      long ssize = (signed)sig->value_size();

      за (long idx = base ;  idx < end ;  idx += 1) {
	    ако (idx < 0 || idx >= ssize) {
                  rbuf[offset-idx] = 'x';
	    } инако {
                  rbuf[offset-idx] = vvp_bit4_to_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

static void format_vpiOctStrVal(const vvp_signal_value*sig, цео base, unsigned wid,
                                s_vpi_value*vp)
{
      unsigned dwid = (wid + 2) / 3;
      char *rbuf = static_cast<char *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->value_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      за (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    ако (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } инако {
                  switch (sig->value(idx)) {
		      case BIT4_0:
			bit = 0;
			break;
		      case BIT4_1:
			bit = 1;
			break;
		      case BIT4_X:
			bit = 2;
			break;
		      case BIT4_Z:
			bit = 3;
			break;
                  }
	    }
	    val |= bit << 2*((idx-base) % 3);

	    ако ((idx-base) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      switch (wid % 3) {
	  case 1:
	    ако (val == 2) val = 42;
	    инако ако (val == 3) val = 63;
	    break;
	  case 2:
	    ако (val == 10) val = 42;
	    инако ако (val == 15) val = 63;
	    break;
      }

      ако (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

static void format_vpiHexStrVal(const vvp_signal_value*sig, цео base, unsigned wid,
                                s_vpi_value*vp)
{
      unsigned dwid = (wid + 3) / 4;
      char *rbuf = static_cast<char *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->value_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      за (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    ако (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } инако {
                  switch (sig->value(idx)) {
		      case BIT4_0:
			bit = 0;
			break;
		      case BIT4_1:
			bit = 1;
			break;
		      case BIT4_X:
			bit = 2;
			break;
		      case BIT4_Z:
			bit = 3;
			break;
                  }
	    }
	    val |= bit << 2*((idx-base) % 4);

	    ако ((idx-base) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      switch (wid % 4) {
	  case 1:
	    ако (val == 2) val = 170;
	    инако ако (val == 3) val = 255;
	    break;
	  case 2:
	    ако (val == 10) val = 170;
	    инако ако (val == 15) val = 255;
	    break;
	  case 3:
	    ако (val == 42) val = 170;
	    инако ако (val == 63) val = 255;
	    break;
      }

      ако (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

static void format_vpiDecStrVal(const vvp_signal_value*sig, цео base, unsigned wid,
                                цео signed_flag, s_vpi_value*vp)
{
      unsigned hwid = (sig->value_size()+2) / 3 + 1;
      char *rbuf = static_cast<char *>(need_result_buf(hwid, RBUF_VAL));
      long ssize = (signed)sig->value_size();
      long end = base + (signed)wid;

	/* Do we have an end outside of the real signal vector. */
      ако (base < 0 || end > ssize) {
	    bool all_x = true;
	    ако (end > ssize) end = ssize;
	    ако (base < 0) base = 0;
	    за (long idx = base ;  idx < end ;  idx += 1) {
		  ако (sig->value(idx) != BIT4_X) {
			all_x = false;
			break;
		  }
	    }

	    ако (all_x) {
		  rbuf[0] = 'x';
	    } инако {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    врати;
      }

      vvp_vector4_t vec4;
      ако (base == 0 && end == ssize) {
	    sig->vec4_value(vec4);
      } инако {
	    vvp_vector4_t tmp;
	    sig->vec4_value(tmp);
	    vec4 = tmp.subvalue(base, wid);
      }

      vpip_vec4_to_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

static void format_vpiIntVal(const vvp_signal_value*sig, цео base, unsigned wid,
                             цео signed_flag, s_vpi_value*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_value(tmp);
      vvp_vector4_t sub = tmp.subvalue(base, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      ако (sizeof(vp->value.integer) == sizeof(int32_t)) {
	    int32_t val = 0;
	    vector4_to_value(sub, val, signed_flag, false);
	    vp->value.integer = val;
      } инако {
	    assert(sizeof(vp->value.integer) == sizeof(int64_t));
	    int64_t val = 0;
	    vector4_to_value(sub, val, signed_flag, false);
	    vp->value.integer = val;
      }
}

static void format_vpiRealVal(const vvp_signal_value*sig, цео base, unsigned wid,
                              цео signed_flag, s_vpi_value*vp)
{
      vvp_vector4_t vec4(wid);
      long ssize = (signed)sig->value_size();
      long end = base + (signed)wid;
      ако (end > ssize) end = ssize;

      за (long idx = (base < 0) ? 0 : base ;  idx < end ;  idx += 1) {
	    vec4.set_bit(idx-base, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_to_value(vec4, vp->value.real, signed_flag);
}

static void format_vpiStringVal(const vvp_signal_value*sig, цео base, unsigned wid,
                                s_vpi_value*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      char *rbuf = static_cast<char *>(need_result_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      char *cp = rbuf;

      char tmp = 0;
      за (long idx = base+(signed)wid-1; idx >= base; idx -= 1) {
	    tmp <<= 1;

	    ако (idx >=0 && idx < (signed)sig->value_size() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    ако (((idx-base)&7)==0){
		  /* Skip leading nulls. */
		  ако (tmp == 0 && cp == rbuf)
			continue;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

static void format_vpiScalarVal(const vvp_signal_value*sig, цео base,
                                s_vpi_value*vp)
{
      ако (base >= 0 && base < (signed)sig->value_size()) {
	    switch (sig->value(base)) {
		case BIT4_0:
		  vp->value.scalar = vpi0;
		  break;
		case BIT4_1:
		  vp->value.scalar = vpi1;
		  break;
		case BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_value(base);
		  ако (strn.strength0() == 1) vp->value.scalar = vpiH;
		  инако ако (strn.strength1() == 1) vp->value.scalar = vpiL;
		  инако vp->value.scalar = vpiX;
		  break;
		}
		case BIT4_Z:
		  vp->value.scalar = vpiZ;
		  break;
	    }
      } инако {
	    vp->value.scalar = vpiX;
      }
}

static void format_vpiStrengthVal(const vvp_signal_value*sig, цео base,
                                  unsigned wid, s_vpi_value*vp)
{
      long end = base + (signed)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_result_buf(wid * sizeof(s_vpi_strengthval), RBUF_VAL));

      за (long idx = base ;  idx < end ;  idx += 1) {
	    ако (idx >=0 && idx < (signed)sig->value_size()) {
		  vvp_scalar_t val = sig->scalar_value(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  unsigned s0 = 1 << val.strength0();
		  unsigned s1 = 1 << val.strength1();

		  switch (val.value()) {
		      case BIT4_0:
			op[idx-base].logic = vpi0;
			op[idx-base].s0 = s0|s1;
			op[idx-base].s1 = 0;
			break;

		      case BIT4_1:
			op[idx-base].logic = vpi1;
			op[idx-base].s0 = 0;
			op[idx-base].s1 = s0|s1;
			break;

		      case BIT4_X:
			op[idx-base].logic = vpiX;
			op[idx-base].s0 = s0;
			op[idx-base].s1 = s1;
			break;

		      case BIT4_Z:
			op[idx-base].logic = vpiZ;
			op[idx-base].s0 = vpiHiZ;
			op[idx-base].s1 = vpiHiZ;
			break;
		  }
	    } инако {
		  op[idx-base].logic = vpiX;
		  op[idx-base].s0 = vpiStrongDrive;
		  op[idx-base].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

static void format_vpiVectorVal(const vvp_signal_value*sig, цео base, unsigned wid,
                                s_vpi_value*vp)
{
      long end = base + (signed)wid;
      unsigned цео obit = 0;
      unsigned hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_result_buf(hwid * sizeof(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      за (long idx = base ;  idx < end ;  idx += 1) {
	    ако (base >= 0 && base < (signed)sig->value_size()) {
		switch (sig->value(idx)) {
		case BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  break;
		case BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  break;
		case BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  break;
		case BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  break;
		}
	    } инако {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    ако (!(obit % 32)) {
		  op += 1;
		  ако ((op - vp->value.vector) < (ptrdiff_t)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
static цео signal_get(цео code, vpiHandle ref)
{
      struct __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);

      switch (code) {
	  case vpiLineNo:
	    врати 0;  // Not implemented for now!

	  case vpiSigned:
	    врати rfp->signed_flag != 0;

	  case vpiArray:
	    врати rfp->is_netarray != 0;

	  case vpiIndex: // This only works while we have a single index.
	    ако (rfp->is_netarray) {
		  s_vpi_value vp;
		  vp.format = vpiIntVal;
		  vpi_get_value(rfp->id.index, &vp);
		  врати vp.value.integer;
	    } инако {
		  врати vpiUndefined;
	    }

	  case vpiSize:
	    врати rfp->width();

	  case vpiNetType:
	    ако (ref->get_type_code()==vpiNet)
		  врати vpiWire;
	    инако
		  врати vpiUndefined;

	  case vpiLeftRange:
            врати rfp->msb.get_value();

	  case vpiRightRange:
            врати rfp->lsb.get_value();

	  case vpiScalar:
	    врати (rfp->msb.get_value() == 0 && rfp->lsb.get_value() == 0);
	  case vpiVector:
	    врати (rfp->msb.get_value() != rfp->lsb.get_value());

          case vpiAutomatic:
            врати vpip_scope(rfp)->is_automatic() ? 1  : 0;

#ifdef BR916_STOPGAP_FIX
          case _vpiFromThr:
            врати _vpiNoThr;
#endif

	  default:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    врати vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
static int64_t signal_get64(цео code, vpiHandle ref)
{
      struct __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);

      switch (code) {
	    // This private property must return zero when undefined.
	  case _vpiNexusId:
	    ако (rfp->msb.get_value() == rfp->lsb.get_value())
		  врати reinterpret_cast<int64_t>(rfp->node);
	    врати 0;

	  default:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    врати vpiUndefined;
      }
}

static char* signal_get_str(цео code, vpiHandle ref)
{
      struct __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);

      ако (code == vpiFile) {  // Not implemented for now!
	    врати simple_set_rbuf_str(file_names[0]);
      }

      ако ((code != vpiName) && (code != vpiFullName)) врати NULL;

      char *nm;
      const char *ixs;
      ако (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_value vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_value(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } инако {
	    nm = strdup(rfp->id.name);
	    ixs = NULL;
      }

	/* The scope information is added here for vpiFullName. */
      char *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      free(nm);
      врати rbuf;
}

static vpiHandle signal_get_handle(цео code, vpiHandle ref)
{
      struct __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);

      switch (code) {

	  case vpiParent:
	    врати rfp->is_netarray? rfp->within.parent : NULL;

	  case vpiIndex:
	    врати rfp->is_netarray? rfp->id.index : NULL;

	  case vpiLeftRange:
	    врати &rfp->msb;
	  case vpiRightRange:
	    врати &rfp->lsb;

	  case vpiScope:
	    врати vpip_scope(rfp);

	  case vpiModule:
	    врати vpip_module(vpip_scope(rfp));
      }

      врати 0;
}

static vpiHandle signal_iterate(цео code, vpiHandle ref)
{
      struct __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);

      ако (code == vpiIndex) {
	    врати rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : NULL;
      }

      врати 0;
}

void __vpiSignal::make_bits()
{
      assert(bits == NULL);
      bits = new struct __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      bits->parent = this;
	// Now point to word[0].
      bits += 1;

	// Update each bit to point to the base
      struct __vpiBit*base = bits;
      за (unsigned idx = 0; idx < width(); idx += 1) {
	    base[idx].bit0 = base;
	    цео real_idx;
	    ако (msb.get_value() >= lsb.get_value()) {
		  real_idx = idx + lsb.get_value();
	    } инако {
		  real_idx = lsb.get_value() - idx;
	    }
	    base[idx].index = new __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_index(цео idx)
{
	/* Check to see if the index is in range. */
      ако (msb.get_value() >= lsb.get_value()) {
	    ако ((idx > msb.get_value()) || (idx < lsb.get_value())) врати 0;
      } инако {
	    ако ((idx < msb.get_value()) || (idx > lsb.get_value())) врати 0;
      }

	/* Normalize the index */
      unsigned norm_idx;
      ако (msb.get_value() >= lsb.get_value()) {
	    norm_idx = idx - lsb.get_value();
      } инако {
	    norm_idx = lsb.get_value() - idx;
      }

      ако (bits == NULL) make_bits();

      врати &(bits[norm_idx].as_bit);
}

void __vpiSignal::get_bit_value(const struct __vpiBit*bit, p_vpi_value vp)
{
      unsigned index = bit->get_norm_index();

      vvp_signal_value*vsig = dynamic_cast<vvp_signal_value*>(node->fil);
      assert(vsig);

      ако (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      switch (vp->format) {
	case vpiBinStrVal:
            format_vpiBinStrVal(vsig, index, 1, vp);
	    break;

	case vpiOctStrVal:
            format_vpiOctStrVal(vsig, index, 1, vp);
	    break;

	case vpiDecStrVal:
            format_vpiDecStrVal(vsig, index, 1, false, vp);
	    break;

	case vpiHexStrVal:
            format_vpiHexStrVal(vsig, index, 1, vp);
	    break;

	case vpiStringVal:
            format_vpiStringVal(vsig, index, 1, vp);
	    break;

	case vpiIntVal:
            format_vpiIntVal(vsig, index, 1, false, vp);
	    break;

	case vpiRealVal:
            format_vpiRealVal(vsig, index, 1, false, vp);
	    break;

	case vpiScalarVal:
            format_vpiScalarVal(vsig, index, vp);
	    break;

        case vpiStrengthVal:
            format_vpiStrengthVal(vsig, index, 1, vp);
            break;

	case vpiVectorVal:
            format_vpiVectorVal(vsig, index, 1, vp);
	    break;

	default:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (цео)vp->format, vpi_get_str(vpiName),
		    bit->get_index(),
	            vpip_scope(this)->scope_name());
	    assert(0);
      };
}

vpiHandle __vpiSignal::put_bit_value(struct __vpiBit*bit, p_vpi_value vp, цео flags)
{
      unsigned index = bit->get_norm_index();
      vvp_net_ptr_t dest(node, 0);
      vvp_vector4_t val = vec4_from_vpi_value(vp, 1);

      ако ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    bit->as_bit.vpi_get_str(vpiFullName));
	    врати NULL;
      }

      ако ((get_type_code() == vpiNet) &&
          !dynamic_cast<vvp_island_port*>(node->fun)) {
	    node->send_vec4_pv(val, index, width(),
	                            vthread_get_wt_context());
      } инако {
	    vvp_send_vec4_pv(dest, val, index, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      врати NULL;
}

static vpiHandle signal_index(цео idx, vpiHandle ref)
{
      struct __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      ако ((type != vpiNet) && (type != vpiReg)) врати 0;

      врати rfp->get_index(idx);
}

unsigned __vpiSignal::width(void) const
{
      unsigned wid = (msb.get_value() >= lsb.get_value())
	    ? (msb.get_value() - lsb.get_value() + 1)
	    : (lsb.get_value() - msb.get_value() + 1);

      врати wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
static void signal_get_value(vpiHandle ref, s_vpi_value*vp)
{
      struct __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);

      unsigned wid = rfp->width();

      vvp_signal_value*vsig = dynamic_cast<vvp_signal_value*>(rfp->node->fil);
      assert(vsig);

      switch (vp->format) {

	  case vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    break;

	  case vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    break;

	  case vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    break;

	  case vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    break;

	  case vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    break;

	  case vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    break;

	  case vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    break;

	  case vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    break;

	  case vpiVectorVal:
	    format_vpiVectorVal(vsig, 0, wid, vp);
	    break;

	  case vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    break;

	  case vpiObjTypeVal:
	    ако (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } инако {
		  vp->format = vpiVectorVal;
		  format_vpiVectorVal(vsig, 0, wid, vp);
	    }
	    break;

	  default:
	    fprintf(stderr, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (цео)vp->format, vpi_get_str(vpiName, ref),
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

static vvp_vector4_t from_stringval(const char*str, unsigned wid)
{
      unsigned idx;
      const char*cp;

      cp = str + strlen(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      док ((idx < wid) && (cp > str)) {
	    unsigned byte = *--cp;
	    цео bit;

	    за (bit = 0 ;  (bit < 8) && (idx < wid) ;  bit += 1) {
		  ако (byte & 1)
			val.set_bit(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      врати val;
}

static vpiHandle signal_put_value(vpiHandle ref, s_vpi_value*vp, цео flags)
{
      unsigned wid;
      struct __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);
      vvp_net_ptr_t dest(rfp->node, 0);

      bool net_flag = ref->get_type_code()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      ако (flags == vpiReleaseFlag) {
	    assert(rfp->node->fil);
	    rfp->node->fil->force_unlink();
	    rfp->node->fil->release(dest, net_flag);
	    rfp->node->fun->force_flag(true);
	    signal_get_value(ref, vp);
	    врати ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_value() >= rfp->lsb.get_value())
	    ? (rfp->msb.get_value() - rfp->lsb.get_value() + 1)
	    : (rfp->lsb.get_value() - rfp->msb.get_value() + 1);

      vvp_vector4_t val = vec4_from_vpi_value(vp, wid);

      ако (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, wid);
	    rfp->node->force_vec4(val, mask);
      } инако ако (net_flag && !dynamic_cast<vvp_island_port*>(rfp->node->fun)) {
	    rfp->node->send_vec4(val, vthread_get_wt_context());
      } инако {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      врати ref;
}

vvp_vector4_t vec4_from_vpi_value(s_vpi_value*vp, unsigned wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      switch (vp->format) {

	  case vpiIntVal: {
		long vpi_val = vp->value.integer;
		за (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t bit = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.set_bit(idx, bit);
		      vpi_val >>= 1;
		}
		break;
	  }

	  case vpiVectorVal:
	    за (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long aval = vp->value.vector[idx/32].aval;
		  unsigned long bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  цео bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t bit = scalar_to_bit4(bitmask);
		  val.set_bit(idx, bit);
	    }
	    break;
	  case vpiBinStrVal:
	    vpip_bin_str_to_vec4(val, vp->value.str);
	    break;
	  case vpiOctStrVal:
	    vpip_oct_str_to_vec4(val, vp->value.str);
	    break;
	  case vpiDecStrVal:
	    vpip_dec_str_to_vec4(val, vp->value.str);
	    break;
	  case vpiHexStrVal:
	    vpip_hex_str_to_vec4(val, vp->value.str);
	    break;
	  case vpiScalarVal:
	    val.set_bit(0, scalar_to_bit4(vp->value.scalar));
	    break;
	  case vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    break;
	  case vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    break;

	  default:
	    fprintf(stderr, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (цео)vp->format);
	    assert(0);

      }

      врати val;
}

цео __vpiSignal::vpi_get(цео code)
{ врати signal_get(code, this); }

int64_t __vpiSignal::vpi_get64(цео code)
{ врати signal_get64(code, this); }

char* __vpiSignal::vpi_get_str(цео code)
{ врати signal_get_str(code, this); }

void __vpiSignal::vpi_get_value(p_vpi_value val)
{ signal_get_value(this, val); }

vpiHandle __vpiSignal::vpi_put_value(p_vpi_value val, цео flags)
{ врати signal_put_value(this, val, flags); }

vpiHandle __vpiSignal::vpi_handle(цео code)
{ врати signal_get_handle(code, this); }

vpiHandle __vpiSignal::vpi_iterate(цео code)
{ врати signal_iterate(code, this); }

vpiHandle __vpiSignal::vpi_index(цео idx)
{ врати signal_index(idx, this); }

struct signal_reg : public __vpiSignal {
      inline signal_reg() { }
      цео get_type_code(void) const override { врати vpiReg; }
};

struct signal_integer : public __vpiSignal {
      inline signal_integer() { }
      цео get_type_code(void) const override { врати vpiIntegerVar; }
};

struct signal_net : public __vpiSignal {
      inline signal_net() { }
      цео get_type_code(void) const override { врати vpiNet; }
};

struct signal_byte : public __vpiSignal {
      inline signal_byte() { }
      цео get_type_code(void) const override { врати vpiByteVar; }
};

struct signal_bitvar : public __vpiSignal {
      inline signal_bitvar() { }
      цео get_type_code(void) const override { врати vpiBitVar; }
};

struct signal_shortint : public __vpiSignal {
      inline signal_shortint() { }
      цео get_type_code(void) const override { врати vpiShortIntVar; }
};

struct signal_int : public __vpiSignal {
      inline signal_int() { }
      цео get_type_code(void) const override { врати vpiIntVar; }
};

struct signal_longint : public __vpiSignal {
      inline signal_longint() { }
      цео get_type_code(void) const override { врати vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(const char*name, цео msb, цео lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = new signal_integer;
      врати fill_in_var4(obj, name, msb, lsb, true, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(const char*name, цео msb, цео lsb, bool signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      ако ((! signed_flag) || (lsb != 0) ) {
	    obj = new signal_bitvar;
      } инако {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    switch (msb) {
		case 7:
		  obj = new signal_byte;
		  break;
		case 15:
		  obj = new signal_shortint;
		  break;
		case 31:
		  obj = new signal_int;
		  break;
		case 63:
		  obj = new signal_longint;
		  break;
		default:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = new signal_bitvar;
		  break;
	    }
      }

      врати fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(const char*name, цео msb, цео lsb,
			bool signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = new signal_reg;
      врати fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

#ifdef CHECK_WITH_VALGRIND
static struct vpiSignal_plug **signal_pool = 0;
static unsigned signal_pool_count = 0;
static unsigned long signal_count = 0;
static unsigned long signal_dels = 0;
#endif

struct vpiSignal_plug {
      unsigned char space[sizeof (struct __vpiSignal)];
#ifdef CHECK_WITH_VALGRIND
      struct vpiSignal_plug *pool;
#endif
};

void* __vpiSignal::operator new(size_t siz)
{
#ifdef CHECK_WITH_VALGRIND
      assert(siz == sizeof(struct vpiSignal_plug) - sizeof(struct vpiSignal_plug *));
#инако
      assert(siz == sizeof(struct vpiSignal_plug));
#endif
      static struct vpiSignal_plug*alloc_array = 0;
      static unsigned alloc_index = 0;
      const unsigned alloc_count = 512;

      ако ((alloc_array == 0) || (alloc_index == alloc_count)) {
	    alloc_array = static_cast<struct vpiSignal_plug*>
	                  (calloc(alloc_count, sizeof(struct vpiSignal_plug)));
	    alloc_index = 0;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_count *
	                                            sizeof(struct vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_count += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_count*sizeof(vpiSignal_plug **)));
	    signal_pool[signal_pool_count-1] = alloc_array;
#endif
      }

      struct vpiSignal_plug*cur = alloc_array + alloc_index;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, sizeof(struct vpiSignal_plug));
      cur->pool = alloc_array;
      signal_count += 1;
#endif
      alloc_index += 1;
      врати cur;
}

void __vpiSignal::operator delete(void*)
{
      assert(0);
}

#ifdef CHECK_WITH_VALGRIND
void signal_delete(vpiHandle item)
{
      struct __vpiSignal *obj = static_cast<__vpiSignal *> (item);
      assert(obj->node->fil);
      obj->node->fil->clear_all_callbacks();
      vvp_net_delete(obj->node);
      ако (obj->bits) {
	    за (unsigned idx=0; idx<obj->width(); idx+=1) {
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
      ако (RUNNING_ON_VALGRIND && (signal_count != signal_dels)) {
	    fflush(NULL);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (long) signal_count - signal_dels, signal_count);
      }

      за (unsigned idx = 0; idx < signal_pool_count; idx += 1) {
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
static vpiHandle fill_in_net4(struct __vpiSignal*obj, __vpiScope*scope,
			      const char*name, цео msb, цео lsb,
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

      врати obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 const char*name, цео msb, цео lsb,
			 bool signed_flag, vvp_net_t*node)
{
      struct __vpiSignal*obj = new signal_net;
      врати fill_in_net4(obj, scope, name, msb, lsb, signed_flag, node);
}

static цео PV_get_base(struct __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      ако (rfp->sbase != 0) {
	    s_vpi_value val;
	      /* Check to see if the value is defined. */
	    val.format = vpiVectorVal;
	    vpi_get_value(rfp->sbase, &val);
	    цео words = (vpi_get(vpiSize, rfp->sbase)-1)/32 + 1;
	    за(цео idx = 0; idx < words; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  ако (val.value.vector[idx].bval != 0) врати INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_value(rfp->sbase, &val);
	    врати val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      врати rfp->tbase;
}

static цео PV_get(цео code, vpiHandle ref)
{
      struct __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      assert(rfp);

      цео rval = 0;
      switch (code) {
	case vpiLineNo:
	    врати 0;  // Not implemented for now!

	case vpiSigned:
	    врати 0;  // A part/bit select is always unsigned!

	case vpiSize:
	    врати rfp->width;

	  /* This is like the &A<> in array.cc. */
	case vpiConstantSelect:
	    врати rfp->sbase == 0;

	case vpiLeftRange:
            rval += rfp->width - 1;
	    // fallthrough
	case vpiRightRange: {
	    цео left = vpi_get(vpiLeftRange, rfp->parent);
	    цео right = vpi_get(vpiRightRange, rfp->parent);
	    rval += PV_get_base(rfp);
	    врати left >= right ? right + rval : right - rval;
	}

        case vpiAutomatic:
            врати vpi_get(vpiAutomatic, rfp->parent);

#ако defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        case _vpiFromThr:
            врати _vpi_at_PV;
#endif

	default:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      врати 0;
}

static char* PV_get_str(цео code, vpiHandle ref)
{
      struct __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      assert(rfp);

      switch (code) {
	case vpiFile:  // Not implemented for now!
	    врати simple_set_rbuf_str(file_names[0]);

	case vpiName:
	case vpiFullName: {
	    const char*nm = vpi_get_str(code, rfp->parent);
	    size_t len = 256+strlen(nm);
	    char *full = static_cast<char *>(malloc(len));
	    snprintf(full, len, "%s[%d:%d]", nm,
	                                     (цео)vpi_get(vpiLeftRange, ref),
	                                     (цео)vpi_get(vpiRightRange, ref));
	    full[len-1] = 0;
	    char *res = simple_set_rbuf_str(full);
	    free(full);
	    врати res;
	}

	default:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      врати 0;
}

static void PV_get_value(vpiHandle ref, p_vpi_value vp)
{
      struct __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      assert(rfp);

      vvp_signal_value*sig = dynamic_cast<vvp_signal_value*>(rfp->net->fil);
      assert(sig);

      switch (vp->format) {

	  case vpiIntVal:
	    format_vpiIntVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    break;

	  case vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    break;

	  case vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    break;

	  case vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    break;

	  case vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    break;

	  case vpiStringVal:
	    format_vpiStringVal(sig, PV_get_base(rfp), rfp->width, vp);
	    break;

	  case vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_base(rfp), vp);
	    break;

	  case vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_base(rfp), rfp->width, vp);
	    break;

	  case vpiVectorVal:
	    format_vpiVectorVal(sig, PV_get_base(rfp), rfp->width, vp);
	    break;

	  case vpiRealVal:
	    format_vpiRealVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    break;

	  default:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (цео)vp->format, vpi_get_str(vpiFullName, rfp->parent));
	    assert(0);
      }
}

static vpiHandle PV_put_value(vpiHandle ref, p_vpi_value vp, цео flags)
{
      struct __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      assert(rfp);
      vvp_signal_value*sig = dynamic_cast<vvp_signal_value*>(rfp->net->fil);
      assert(sig);

      unsigned sig_size = sig->value_size();
      unsigned width = rfp->width;
      цео base = PV_get_base(rfp);
      ако (base >= (signed) sig_size) врати 0;
      ако (base + (signed) width < 0) врати 0;

      vvp_vector4_t val;
      ако (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_value(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      ако (base < 0) {
	    width += base;
	    ако (flags != vpiReleaseFlag) {
		  val = val.subvalue(-base, width);
	    }
	    base = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      ако (base+width > sig_size) {
	    width = sig_size - base;
	    ако (flags != vpiReleaseFlag) {
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
      ако (flags == vpiReleaseFlag) {
	    assert(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    ако (full_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } инако {
		  rfp->net->fil->release_pv(dest, base, width, net_flag);
	    }
	    rfp->net->fun->force_flag(true);
	    PV_get_value(ref, vp);
	    врати ref;
      }

      ако (flags == vpiForceFlag) {
	    ако (full_sig) {
		  vvp_vector2_t mask (vvp_vector2_t::FILL1, sig_size);
		  rfp->net->force_vec4(val, mask);
	    } инако {
		  vvp_vector2_t mask (vvp_vector2_t::FILL0, sig_size);
		  за (unsigned idx = 0 ; idx < width ; idx += 1)
		        mask.set_bit(base+idx, 1);

		  vvp_vector4_t tmp (sig_size, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_value(tmp);

		  tmp.set_vec(base, val);
		  rfp->net->force_vec4(tmp, mask);
	    }
      } инако ако (net_flag && !dynamic_cast<vvp_island_port*>(rfp->net->fun)) {
	    ако (full_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } инако {
		  rfp->net->send_vec4_pv(val, base, sig_size,
		                         vthread_get_wt_context());
	    }
      } инако {
	    ако (full_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } инако {
		  vvp_send_vec4_pv(dest, val, base, sig_size,
	                           vthread_get_wt_context());
	    }
      }
      врати 0;
}

static vpiHandle PV_get_handle(цео code, vpiHandle ref)
{
      struct __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      assert(rfp);

      switch (code) {
	  case vpiParent:
	    врати rfp->parent;

	  case vpiScope:
	    врати vpi_handle(vpiScope, rfp->parent);

	  case vpiModule:
	    врати vpi_handle(vpiModule, rfp->parent);
      }

      врати 0;
}

inline __vpiPV::__vpiPV()
{ }

цео __vpiPV::get_type_code(void) const
{ врати vpiPartSelect; }

цео __vpiPV::vpi_get(цео code)
{ врати PV_get(code, this); }

char* __vpiPV::vpi_get_str(цео code)
{ врати PV_get_str(code, this); }

void __vpiPV::vpi_get_value(p_vpi_value val)
{ PV_get_value(this, val); }

vpiHandle __vpiPV::vpi_put_value(p_vpi_value val, цео flags)
{ врати PV_put_value(this, val, flags); }

vpiHandle __vpiPV::vpi_handle(цео code)
{ врати PV_get_handle(code, this); }

vpiHandle vpip_make_PV(char*var, цео base, цео width)
{
      struct __vpiPV*obj = new __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = 0;
      obj->tbase = base;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      врати obj;
}

vpiHandle vpip_make_PV(char*var, char*symbol, цео width)
{
      struct __vpiPV*obj = new __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      compile_vpi_lookup(&obj->sbase, symbol);
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      врати obj;
}

vpiHandle vpip_make_PV(char*var, vpiHandle handle, цео width)
{
      struct __vpiPV*obj = new __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      врати obj;
}

#ifdef CHECK_WITH_VALGRIND
void PV_delete(vpiHandle item)
{
      struct __vpiPV *obj = dynamic_cast<__vpiPV*>(item);
      ако (obj->sbase) {
	    switch (obj->sbase->get_type_code()) {
		case vpiMemoryWord:
		  ако (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  break;
		case vpiPartSelect:
		  assert(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  break;
	    }
      }
      assert(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      delete obj;
}
#endif
