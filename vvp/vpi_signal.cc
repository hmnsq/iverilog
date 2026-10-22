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
બાહ્ય અચલ અક્ષર hex_digits[256];
બાહ્ય અચલ અક્ષર oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
શૂન્ય *need_result_buf(unsigned cnt, vpi_rbuf_t type)
{
      સ્થિર શૂન્ય*result_buf[2] = {0, 0};
      સ્થિર size_t result_buf_size[2] = {0, 0};

      જો (type == RBUF_DEL) {
	    મુક્ત(result_buf[RBUF_VAL]);
	    result_buf[RBUF_VAL] = 0;
	    result_buf_size[RBUF_VAL] = 0;

	    મુક્ત(result_buf[RBUF_STR]);
	    result_buf[RBUF_STR] = 0;
	    result_buf_size[RBUF_STR] = 0;

	    પાછા 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      જો (result_buf_size[type] == 0) {
	    result_buf[type] = malloc(cnt);
	    result_buf_size[type] = cnt;
      } નહીંતર જો (result_buf_size[type] < cnt) {
	    result_buf[type] = realloc(result_buf[type], cnt);
	    result_buf_size[type] = cnt;
      }

      પાછા result_buf[type];
}

અક્ષર *simple_set_rbuf_str(અચલ અક્ષર *s1)
{
      અક્ષર *res = static_cast<અક્ષર *>(need_result_buf(strlen(s1)+1, RBUF_STR));
      જો (res) strcpy(res,s1);
      પાછા res;
}

અક્ષર *generic_get_str(પૂર્ણાંક code, vpiHandle ref, અચલ અક્ષર *name, અચલ અક્ષર *index)
{
      size_t len = strlen(name) + 1;  /* include space for null termination */
      અક્ષર *bn = ખાલી;
      તાર્કિક is_bn_a_pkg = ખોટું;
      જો (code == vpiFullName) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = strlen(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    જો (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      જો (index != ખાલી) len += strlen(index) + 2;  /* include space for brackets */

      અક્ષર *res = static_cast<અક્ષર *>(need_result_buf(len, RBUF_STR));
      જો (!res) {
	    મુક્ત(bn);
	    પાછા ખાલી;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      જો (bn != ખાલી) {
	    strcat(res, bn);
	      // A package already has the "::" separator in the name
	    જો (! is_bn_a_pkg) strcat(res, ".");
	    મુક્ત(bn);
      }
      strcat(res, name);
      જો (index != ખાલી) {
	    strcat(res, "[");
	    strcat(res, index);
	    strcat(res, "]");
      }
      પાછા res;
}

સ્થિર vpiHandle fill_in_net4(સંરચના __vpiSignal*obj, __vpiScope*scope,
                              અચલ અક્ષર*name, પૂર્ણાંક msb, પૂર્ણાંક lsb,
                              તાર્કિક signed_flag, vvp_net_t*ગાંઠ);

સ્થિર vpiHandle fill_in_var4(સંરચના __vpiSignal*obj,
                              અચલ અક્ષર*name, પૂર્ણાંક msb, પૂર્ણાંક lsb,
                              તાર્કિક signed_flag, vvp_net_t*ગાંઠ)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      પાછા fill_in_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, ગાંઠ);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

સ્થિર શૂન્ય format_vpiBinStrVal(અચલ vvp_signal_કિંમત*sig, પૂર્ણાંક base, unsigned wid,
                                s_vpi_કિંમત*vp)
{
      અક્ષર *rbuf = static_cast<અક્ષર *>(need_result_buf(wid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long offset = end - 1;
      long ssize = (signed)sig->કિંમત_size();

      માટે (long idx = base ;  idx < end ;  idx += 1) {
	    જો (idx < 0 || idx >= ssize) {
                  rbuf[offset-idx] = 'x';
	    } નહીંતર {
                  rbuf[offset-idx] = vvp_bit4_to_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

સ્થિર શૂન્ય format_vpiOctStrVal(અચલ vvp_signal_કિંમત*sig, પૂર્ણાંક base, unsigned wid,
                                s_vpi_કિંમત*vp)
{
      unsigned dwid = (wid + 2) / 3;
      અક્ષર *rbuf = static_cast<અક્ષર *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->કિંમત_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      માટે (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    જો (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } નહીંતર {
                  પસંદગી (sig->value(idx)) {
		      સ્થિતિ BIT4_0:
			bit = 0;
			તોડો;
		      સ્થિતિ BIT4_1:
			bit = 1;
			તોડો;
		      સ્થિતિ BIT4_X:
			bit = 2;
			તોડો;
		      સ્થિતિ BIT4_Z:
			bit = 3;
			તોડો;
                  }
	    }
	    val |= bit << 2*((idx-base) % 3);

	    જો ((idx-base) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      પસંદગી (wid % 3) {
	  સ્થિતિ 1:
	    જો (val == 2) val = 42;
	    નહીંતર જો (val == 3) val = 63;
	    તોડો;
	  સ્થિતિ 2:
	    જો (val == 10) val = 42;
	    નહીંતર જો (val == 15) val = 63;
	    તોડો;
      }

      જો (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

સ્થિર શૂન્ય format_vpiHexStrVal(અચલ vvp_signal_કિંમત*sig, પૂર્ણાંક base, unsigned wid,
                                s_vpi_કિંમત*vp)
{
      unsigned dwid = (wid + 3) / 4;
      અક્ષર *rbuf = static_cast<અક્ષર *>(need_result_buf(dwid+1, RBUF_VAL));
      long end = base + (signed)wid;
      long ssize = (signed)sig->કિંમત_size();
      unsigned val = 0;

      rbuf[dwid] = 0;
      માટે (long idx = base ;  idx < end ;  idx += 1) {
	    unsigned bit = 0;
	    જો (idx < 0 || idx >= ssize) {
                  bit = 2; // BIT4_X
	    } નહીંતર {
                  પસંદગી (sig->value(idx)) {
		      સ્થિતિ BIT4_0:
			bit = 0;
			તોડો;
		      સ્થિતિ BIT4_1:
			bit = 1;
			તોડો;
		      સ્થિતિ BIT4_X:
			bit = 2;
			તોડો;
		      સ્થિતિ BIT4_Z:
			bit = 3;
			તોડો;
                  }
	    }
	    val |= bit << 2*((idx-base) % 4);

	    જો ((idx-base) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      પસંદગી (wid % 4) {
	  સ્થિતિ 1:
	    જો (val == 2) val = 170;
	    નહીંતર જો (val == 3) val = 255;
	    તોડો;
	  સ્થિતિ 2:
	    જો (val == 10) val = 170;
	    નહીંતર જો (val == 15) val = 255;
	    તોડો;
	  સ્થિતિ 3:
	    જો (val == 42) val = 170;
	    નહીંતર જો (val == 63) val = 255;
	    તોડો;
      }

      જો (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

સ્થિર શૂન્ય format_vpiDecStrVal(અચલ vvp_signal_કિંમત*sig, પૂર્ણાંક base, unsigned wid,
                                પૂર્ણાંક signed_flag, s_vpi_કિંમત*vp)
{
      unsigned hwid = (sig->કિંમત_size()+2) / 3 + 1;
      અક્ષર *rbuf = static_cast<અક્ષર *>(need_result_buf(hwid, RBUF_VAL));
      long ssize = (signed)sig->કિંમત_size();
      long end = base + (signed)wid;

	/* Do we have an end outside of the real signal vector. */
      જો (base < 0 || end > ssize) {
	    તાર્કિક all_x = સાચું;
	    જો (end > ssize) end = ssize;
	    જો (base < 0) base = 0;
	    માટે (long idx = base ;  idx < end ;  idx += 1) {
		  જો (sig->value(idx) != BIT4_X) {
			all_x = ખોટું;
			તોડો;
		  }
	    }

	    જો (all_x) {
		  rbuf[0] = 'x';
	    } નહીંતર {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    પાછા;
      }

      vvp_vector4_t vec4;
      જો (base == 0 && end == ssize) {
	    sig->vec4_કિંમત(vec4);
      } નહીંતર {
	    vvp_vector4_t tmp;
	    sig->vec4_કિંમત(tmp);
	    vec4 = tmp.subvalue(base, wid);
      }

      vpip_vec4_to_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

સ્થિર શૂન્ય format_vpiIntVal(અચલ vvp_signal_કિંમત*sig, પૂર્ણાંક base, unsigned wid,
                             પૂર્ણાંક signed_flag, s_vpi_કિંમત*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_કિંમત(tmp);
      vvp_vector4_t sub = tmp.subvalue(base, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      જો (કદ(vp->value.integer) == કદ(int32_t)) {
	    int32_t val = 0;
	    vector4_to_કિંમત(sub, val, signed_flag, ખોટું);
	    vp->value.integer = val;
      } નહીંતર {
	    પ્રતિજ્ઞા(કદ(vp->value.integer) == કદ(int64_t));
	    int64_t val = 0;
	    vector4_to_કિંમત(sub, val, signed_flag, ખોટું);
	    vp->value.integer = val;
      }
}

સ્થિર શૂન્ય format_vpiRealVal(અચલ vvp_signal_કિંમત*sig, પૂર્ણાંક base, unsigned wid,
                              પૂર્ણાંક signed_flag, s_vpi_કિંમત*vp)
{
      vvp_vector4_t vec4(wid);
      long ssize = (signed)sig->કિંમત_size();
      long end = base + (signed)wid;
      જો (end > ssize) end = ssize;

      માટે (long idx = (base < 0) ? 0 : base ;  idx < end ;  idx += 1) {
	    vec4.set_bit(idx-base, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_to_કિંમત(vec4, vp->value.real, signed_flag);
}

સ્થિર શૂન્ય format_vpiStringVal(અચલ vvp_signal_કિંમત*sig, પૂર્ણાંક base, unsigned wid,
                                s_vpi_કિંમત*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      અક્ષર *rbuf = static_cast<અક્ષર *>(need_result_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      અક્ષર *cp = rbuf;

      અક્ષર tmp = 0;
      માટે (long idx = base+(signed)wid-1; idx >= base; idx -= 1) {
	    tmp <<= 1;

	    જો (idx >=0 && idx < (signed)sig->કિંમત_size() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    જો (((idx-base)&7)==0){
		  /* Skip leading nulls. */
		  જો (tmp == 0 && cp == rbuf)
			ચાલુ;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

સ્થિર શૂન્ય format_vpiScalarVal(અચલ vvp_signal_કિંમત*sig, પૂર્ણાંક base,
                                s_vpi_કિંમત*vp)
{
      જો (base >= 0 && base < (signed)sig->કિંમત_size()) {
	    પસંદગી (sig->value(base)) {
		સ્થિતિ BIT4_0:
		  vp->value.scalar = vpi0;
		  તોડો;
		સ્થિતિ BIT4_1:
		  vp->value.scalar = vpi1;
		  તોડો;
		સ્થિતિ BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_કિંમત(base);
		  જો (strn.strength0() == 1) vp->value.scalar = vpiH;
		  નહીંતર જો (strn.strength1() == 1) vp->value.scalar = vpiL;
		  નહીંતર vp->value.scalar = vpiX;
		  તોડો;
		}
		સ્થિતિ BIT4_Z:
		  vp->value.scalar = vpiZ;
		  તોડો;
	    }
      } નહીંતર {
	    vp->value.scalar = vpiX;
      }
}

સ્થિર શૂન્ય format_vpiStrengthVal(અચલ vvp_signal_કિંમત*sig, પૂર્ણાંક base,
                                  unsigned wid, s_vpi_કિંમત*vp)
{
      long end = base + (signed)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_result_buf(wid * કદ(s_vpi_strengthval), RBUF_VAL));

      માટે (long idx = base ;  idx < end ;  idx += 1) {
	    જો (idx >=0 && idx < (signed)sig->કિંમત_size()) {
		  vvp_scalar_t val = sig->scalar_કિંમત(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  unsigned s0 = 1 << val.strength0();
		  unsigned s1 = 1 << val.strength1();

		  પસંદગી (val.value()) {
		      સ્થિતિ BIT4_0:
			op[idx-base].logic = vpi0;
			op[idx-base].s0 = s0|s1;
			op[idx-base].s1 = 0;
			તોડો;

		      સ્થિતિ BIT4_1:
			op[idx-base].logic = vpi1;
			op[idx-base].s0 = 0;
			op[idx-base].s1 = s0|s1;
			તોડો;

		      સ્થિતિ BIT4_X:
			op[idx-base].logic = vpiX;
			op[idx-base].s0 = s0;
			op[idx-base].s1 = s1;
			તોડો;

		      સ્થિતિ BIT4_Z:
			op[idx-base].logic = vpiZ;
			op[idx-base].s0 = vpiHiZ;
			op[idx-base].s1 = vpiHiZ;
			તોડો;
		  }
	    } નહીંતર {
		  op[idx-base].logic = vpiX;
		  op[idx-base].s0 = vpiStrongDrive;
		  op[idx-base].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

સ્થિર શૂન્ય format_vpiVectorVal(અચલ vvp_signal_કિંમત*sig, પૂર્ણાંક base, unsigned wid,
                                s_vpi_કિંમત*vp)
{
      long end = base + (signed)wid;
      unsigned પૂર્ણાંક obit = 0;
      unsigned hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_result_buf(hwid * કદ(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      માટે (long idx = base ;  idx < end ;  idx += 1) {
	    જો (base >= 0 && base < (signed)sig->કિંમત_size()) {
		પસંદગી (sig->value(idx)) {
		સ્થિતિ BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  તોડો;
		સ્થિતિ BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  તોડો;
		સ્થિતિ BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  તોડો;
		સ્થિતિ BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  તોડો;
		}
	    } નહીંતર {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    જો (!(obit % 32)) {
		  op += 1;
		  જો ((op - vp->value.vector) < (ptrdiff_t)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
સ્થિર પૂર્ણાંક signal_get(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      પ્રતિજ્ઞા(rfp);

      પસંદગી (code) {
	  સ્થિતિ vpi_લીટી_No:
	    પાછા 0;  // Not implemented for now!

	  સ્થિતિ vpiSigned:
	    પાછા rfp->signed_flag != 0;

	  સ્થિતિ vpiArray:
	    પાછા rfp->is_netarray != 0;

	  સ્થિતિ vpiIndex: // This only works while we have a single index.
	    જો (rfp->is_netarray) {
		  s_vpi_કિંમત vp;
		  vp.format = vpiIntVal;
		  vpi_get_કિંમત(rfp->id.index, &vp);
		  પાછા vp.value.integer;
	    } નહીંતર {
		  પાછા vpiUndefined;
	    }

	  સ્થિતિ vpiSize:
	    પાછા rfp->width();

	  સ્થિતિ vpiNetType:
	    જો (ref->get_type_code()==vpiNet)
		  પાછા vpiWire;
	    નહીંતર
		  પાછા vpiUndefined;

	  સ્થિતિ vpi_ડાબું_Range:
            પાછા rfp->msb.get_કિંમત();

	  સ્થિતિ vpi_જમણું_Range:
            પાછા rfp->lsb.get_કિંમત();

	  સ્થિતિ vpiScalar:
	    પાછા (rfp->msb.get_કિંમત() == 0 && rfp->lsb.get_કિંમત() == 0);
	  સ્થિતિ vpiVector:
	    પાછા (rfp->msb.get_કિંમત() != rfp->lsb.get_કિંમત());

          સ્થિતિ vpiAutomatic:
            પાછા vpip_scope(rfp)->is_automatic() ? 1  : 0;

#ifdef BR916_STOPGAP_FIX
          સ્થિતિ _vpiFromThr:
            પાછા _vpiNoThr;
#endif

	  મૂળભૂત:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    પાછા vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
સ્થિર int64_t signal_get64(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      પ્રતિજ્ઞા(rfp);

      પસંદગી (code) {
	    // This private property must return zero when undefined.
	  સ્થિતિ _vpiNexusId:
	    જો (rfp->msb.get_કિંમત() == rfp->lsb.get_કિંમત())
		  પાછા reinterpret_cast<int64_t>(rfp->ગાંઠ);
	    પાછા 0;

	  મૂળભૂત:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    પાછા vpiUndefined;
      }
}

સ્થિર અક્ષર* signal_get_str(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      પ્રતિજ્ઞા(rfp);

      જો (code == vpiFile) {  // Not implemented for now!
	    પાછા simple_set_rbuf_str(file_names[0]);
      }

      જો ((code != vpiName) && (code != vpiFullName)) પાછા ખાલી;

      અક્ષર *nm;
      અચલ અક્ષર *ixs;
      જો (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.parent));
	    s_vpi_કિંમત vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_કિંમત(rfp->id.index, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } નહીંતર {
	    nm = strdup(rfp->id.name);
	    ixs = ખાલી;
      }

	/* The scope information is added here for vpiFullName. */
      અક્ષર *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      મુક્ત(nm);
      પાછા rbuf;
}

સ્થિર vpiHandle signal_get_handle(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      પ્રતિજ્ઞા(rfp);

      પસંદગી (code) {

	  સ્થિતિ vpiParent:
	    પાછા rfp->is_netarray? rfp->within.parent : ખાલી;

	  સ્થિતિ vpiIndex:
	    પાછા rfp->is_netarray? rfp->id.index : ખાલી;

	  સ્થિતિ vpi_ડાબું_Range:
	    પાછા &rfp->msb;
	  સ્થિતિ vpi_જમણું_Range:
	    પાછા &rfp->lsb;

	  સ્થિતિ vpiScope:
	    પાછા vpip_scope(rfp);

	  સ્થિતિ vpiModule:
	    પાછા vpip_module(vpip_scope(rfp));
      }

      પાછા 0;
}

સ્થિર vpiHandle signal_iterate(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      પ્રતિજ્ઞા(rfp);

      જો (code == vpiIndex) {
	    પાછા rfp->is_netarray ? rfp->id.index->vpi_iterate(code) : ખાલી;
      }

      પાછા 0;
}

શૂન્ય __vpiSignal::make_bits()
{
      પ્રતિજ્ઞા(bits == ખાલી);
      bits = નવું સંરચના __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      bits->parent = આ;
	// Now point to word[0].
      bits += 1;

	// Update each bit to point to the base
      સંરચના __vpiBit*base = bits;
      માટે (unsigned idx = 0; idx < width(); idx += 1) {
	    base[idx].bit0 = base;
	    પૂર્ણાંક real_idx;
	    જો (msb.get_કિંમત() >= lsb.get_કિંમત()) {
		  real_idx = idx + lsb.get_કિંમત();
	    } નહીંતર {
		  real_idx = lsb.get_કિંમત() - idx;
	    }
	    base[idx].index = નવું __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_index(પૂર્ણાંક idx)
{
	/* Check to see if the index is in range. */
      જો (msb.get_કિંમત() >= lsb.get_કિંમત()) {
	    જો ((idx > msb.get_કિંમત()) || (idx < lsb.get_કિંમત())) પાછા 0;
      } નહીંતર {
	    જો ((idx < msb.get_કિંમત()) || (idx > lsb.get_કિંમત())) પાછા 0;
      }

	/* Normalize the index */
      unsigned norm_idx;
      જો (msb.get_કિંમત() >= lsb.get_કિંમત()) {
	    norm_idx = idx - lsb.get_કિંમત();
      } નહીંતર {
	    norm_idx = lsb.get_કિંમત() - idx;
      }

      જો (bits == ખાલી) make_bits();

      પાછા &(bits[norm_idx].as_bit);
}

શૂન્ય __vpiSignal::get_bit_કિંમત(અચલ સંરચના __vpiBit*bit, p_vpi_કિંમત vp)
{
      unsigned index = bit->get_norm_index();

      vvp_signal_કિંમત*vsig = dynamic_cast<vvp_signal_કિંમત*>(ગાંઠ->fil);
      પ્રતિજ્ઞા(vsig);

      જો (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      પસંદગી (vp->format) {
	સ્થિતિ vpiBinStrVal:
            format_vpiBinStrVal(vsig, index, 1, vp);
	    તોડો;

	સ્થિતિ vpiOctStrVal:
            format_vpiOctStrVal(vsig, index, 1, vp);
	    તોડો;

	સ્થિતિ vpiDecStrVal:
            format_vpiDecStrVal(vsig, index, 1, ખોટું, vp);
	    તોડો;

	સ્થિતિ vpiHexStrVal:
            format_vpiHexStrVal(vsig, index, 1, vp);
	    તોડો;

	સ્થિતિ vpiStringVal:
            format_vpiStringVal(vsig, index, 1, vp);
	    તોડો;

	સ્થિતિ vpiIntVal:
            format_vpiIntVal(vsig, index, 1, ખોટું, vp);
	    તોડો;

	સ્થિતિ vpiRealVal:
            format_vpiRealVal(vsig, index, 1, ખોટું, vp);
	    તોડો;

	સ્થિતિ vpiScalarVal:
            format_vpiScalarVal(vsig, index, vp);
	    તોડો;

        સ્થિતિ vpiStrengthVal:
            format_vpiStrengthVal(vsig, index, 1, vp);
            તોડો;

	સ્થિતિ vpiVectorVal:
            format_vpiVectorVal(vsig, index, 1, vp);
	    તોડો;

	મૂળભૂત:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (પૂર્ણાંક)vp->format, vpi_get_str(vpiName),
		    bit->get_index(),
	            vpip_scope(આ)->scope_name());
	    પ્રતિજ્ઞા(0);
      };
}

vpiHandle __vpiSignal::put_bit_કિંમત(સંરચના __vpiBit*bit, p_vpi_કિંમત vp, પૂર્ણાંક flags)
{
      unsigned index = bit->get_norm_index();
      vvp_net_ptr_t dest(ગાંઠ, 0);
      vvp_vector4_t val = vec4_from_vpi_કિંમત(vp, 1);

      જો ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    bit->as_bit.vpi_get_str(vpiFullName));
	    પાછા ખાલી;
      }

      જો ((get_type_code() == vpiNet) &&
          !dynamic_cast<vvp_island_port*>(ગાંઠ->fun)) {
	    ગાંઠ->send_vec4_pv(val, index, width(),
	                            vthread_get_wt_context());
      } નહીંતર {
	    vvp_send_vec4_pv(dest, val, index, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      પાછા ખાલી;
}

સ્થિર vpiHandle signal_index(પૂર્ણાંક idx, vpiHandle ref)
{
      સંરચના __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      પ્રતિજ્ઞા(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      જો ((type != vpiNet) && (type != vpiReg)) પાછા 0;

      પાછા rfp->get_index(idx);
}

unsigned __vpiSignal::width(શૂન્ય) અચલ
{
      unsigned wid = (msb.get_કિંમત() >= lsb.get_કિંમત())
	    ? (msb.get_કિંમત() - lsb.get_કિંમત() + 1)
	    : (lsb.get_કિંમત() - msb.get_કિંમત() + 1);

      પાછા wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
સ્થિર શૂન્ય signal_get_કિંમત(vpiHandle ref, s_vpi_કિંમત*vp)
{
      સંરચના __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      પ્રતિજ્ઞા(rfp);

      unsigned wid = rfp->width();

      vvp_signal_કિંમત*vsig = dynamic_cast<vvp_signal_કિંમત*>(rfp->ગાંઠ->fil);
      પ્રતિજ્ઞા(vsig);

      પસંદગી (vp->format) {

	  સ્થિતિ vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    તોડો;

	  સ્થિતિ vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    તોડો;

	  સ્થિતિ vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    તોડો;

	  સ્થિતિ vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    તોડો;

	  સ્થિતિ vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    તોડો;

	  સ્થિતિ vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    તોડો;

	  સ્થિતિ vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    તોડો;

	  સ્થિતિ vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    તોડો;

	  સ્થિતિ vpiVectorVal:
	    format_vpiVectorVal(vsig, 0, wid, vp);
	    તોડો;

	  સ્થિતિ vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    તોડો;

	  સ્થિતિ vpiObjTypeVal:
	    જો (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } નહીંતર {
		  vp->format = vpiVectorVal;
		  format_vpiVectorVal(vsig, 0, wid, vp);
	    }
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (પૂર્ણાંક)vp->format, vpi_get_str(vpiName, ref),
		    vpip_scope(rfp)->scope_name());
	    પ્રતિજ્ઞા(0);
      }
}

/*
 * The put_value method writes the value into the vector, and returns
 * the affected ref. This operation works much like the %set or
 * %assign instructions and causes all the side-effects that the
 * equivalent instruction would cause.
 */

સ્થિર vvp_vector4_t from_stringval(અચલ અક્ષર*str, unsigned wid)
{
      unsigned idx;
      અચલ અક્ષર*cp;

      cp = str + strlen(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      જ્યારે ((idx < wid) && (cp > str)) {
	    unsigned byte = *--cp;
	    પૂર્ણાંક bit;

	    માટે (bit = 0 ;  (bit < 8) && (idx < wid) ;  bit += 1) {
		  જો (byte & 1)
			val.set_bit(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      પાછા val;
}

સ્થિર vpiHandle signal_put_કિંમત(vpiHandle ref, s_vpi_કિંમત*vp, પૂર્ણાંક flags)
{
      unsigned wid;
      સંરચના __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      પ્રતિજ્ઞા(rfp);
      vvp_net_ptr_t dest(rfp->ગાંઠ, 0);

      તાર્કિક net_flag = ref->get_type_code()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      જો (flags == vpiReleaseFlag) {
	    પ્રતિજ્ઞા(rfp->ગાંઠ->fil);
	    rfp->ગાંઠ->fil->force_unlink();
	    rfp->ગાંઠ->fil->release(dest, net_flag);
	    rfp->ગાંઠ->fun->force_flag(સાચું);
	    signal_get_કિંમત(ref, vp);
	    પાછા ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_કિંમત() >= rfp->lsb.get_કિંમત())
	    ? (rfp->msb.get_કિંમત() - rfp->lsb.get_કિંમત() + 1)
	    : (rfp->lsb.get_કિંમત() - rfp->msb.get_કિંમત() + 1);

      vvp_vector4_t val = vec4_from_vpi_કિંમત(vp, wid);

      જો (flags == vpiForceFlag) {
	    vvp_vector2_t mask (vvp_vector2_t::FILL1, wid);
	    rfp->ગાંઠ->force_vec4(val, mask);
      } નહીંતર જો (net_flag && !dynamic_cast<vvp_island_port*>(rfp->ગાંઠ->fun)) {
	    rfp->ગાંઠ->send_vec4(val, vthread_get_wt_context());
      } નહીંતર {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      પાછા ref;
}

vvp_vector4_t vec4_from_vpi_કિંમત(s_vpi_કિંમત*vp, unsigned wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      પસંદગી (vp->format) {

	  સ્થિતિ vpiIntVal: {
		long vpi_val = vp->value.integer;
		માટે (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t bit = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.set_bit(idx, bit);
		      vpi_val >>= 1;
		}
		તોડો;
	  }

	  સ્થિતિ vpiVectorVal:
	    માટે (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
		  unsigned long aval = vp->value.vector[idx/32].aval;
		  unsigned long bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  પૂર્ણાંક bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t bit = scalar_to_bit4(bitmask);
		  val.set_bit(idx, bit);
	    }
	    તોડો;
	  સ્થિતિ vpiBinStrVal:
	    vpip_bin_str_to_vec4(val, vp->value.str);
	    તોડો;
	  સ્થિતિ vpiOctStrVal:
	    vpip_oct_str_to_vec4(val, vp->value.str);
	    તોડો;
	  સ્થિતિ vpiDecStrVal:
	    vpip_dec_str_to_vec4(val, vp->value.str);
	    તોડો;
	  સ્થિતિ vpiHexStrVal:
	    vpip_hex_str_to_vec4(val, vp->value.str);
	    તોડો;
	  સ્થિતિ vpiScalarVal:
	    val.set_bit(0, scalar_to_bit4(vp->value.scalar));
	    તોડો;
	  સ્થિતિ vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    તોડો;
	  સ્થિતિ vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (પૂર્ણાંક)vp->format);
	    પ્રતિજ્ઞા(0);

      }

      પાછા val;
}

પૂર્ણાંક __vpiSignal::vpi_get(પૂર્ણાંક code)
{ પાછા signal_get(code, આ); }

int64_t __vpiSignal::vpi_get64(પૂર્ણાંક code)
{ પાછા signal_get64(code, આ); }

અક્ષર* __vpiSignal::vpi_get_str(પૂર્ણાંક code)
{ પાછા signal_get_str(code, આ); }

શૂન્ય __vpiSignal::vpi_get_કિંમત(p_vpi_કિંમત val)
{ signal_get_કિંમત(આ, val); }

vpiHandle __vpiSignal::vpi_put_કિંમત(p_vpi_કિંમત val, પૂર્ણાંક flags)
{ પાછા signal_put_કિંમત(આ, val, flags); }

vpiHandle __vpiSignal::vpi_handle(પૂર્ણાંક code)
{ પાછા signal_get_handle(code, આ); }

vpiHandle __vpiSignal::vpi_iterate(પૂર્ણાંક code)
{ પાછા signal_iterate(code, આ); }

vpiHandle __vpiSignal::vpi_index(પૂર્ણાંક idx)
{ પાછા signal_index(idx, આ); }

સંરચના signal_reg : જાહેર __vpiSignal {
      અંતઃરેખ signal_reg() { }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiReg; }
};

સંરચના signal_integer : જાહેર __vpiSignal {
      અંતઃરેખ signal_integer() { }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiIntegerVar; }
};

સંરચના signal_net : જાહેર __vpiSignal {
      અંતઃરેખ signal_net() { }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiNet; }
};

સંરચના signal_byte : જાહેર __vpiSignal {
      અંતઃરેખ signal_byte() { }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiByteVar; }
};

સંરચના signal_bitvar : જાહેર __vpiSignal {
      અંતઃરેખ signal_bitvar() { }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiBitVar; }
};

સંરચના signal_shortint : જાહેર __vpiSignal {
      અંતઃરેખ signal_shortint() { }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiShortIntVar; }
};

સંરચના signal_int : જાહેર __vpiSignal {
      અંતઃરેખ signal_int() { }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiIntVar; }
};

સંરચના signal_longint : જાહેર __vpiSignal {
      અંતઃરેખ signal_longint() { }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(અચલ અક્ષર*name, પૂર્ણાંક msb, પૂર્ણાંક lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = નવું signal_integer;
      પાછા fill_in_var4(obj, name, msb, lsb, સાચું, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(અચલ અક્ષર*name, પૂર્ણાંક msb, પૂર્ણાંક lsb, તાર્કિક signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      જો ((! signed_flag) || (lsb != 0) ) {
	    obj = નવું signal_bitvar;
      } નહીંતર {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    પસંદગી (msb) {
		સ્થિતિ 7:
		  obj = નવું signal_byte;
		  તોડો;
		સ્થિતિ 15:
		  obj = નવું signal_shortint;
		  તોડો;
		સ્થિતિ 31:
		  obj = નવું signal_int;
		  તોડો;
		સ્થિતિ 63:
		  obj = નવું signal_longint;
		  તોડો;
		મૂળભૂત:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = નવું signal_bitvar;
		  તોડો;
	    }
      }

      પાછા fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(અચલ અક્ષર*name, પૂર્ણાંક msb, પૂર્ણાંક lsb,
			તાર્કિક signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = નવું signal_reg;
      પાછા fill_in_var4(obj, name, msb, lsb, signed_flag, vec);
}

#ifdef CHECK_WITH_VALGRIND
સ્થિર સંરચના vpiSignal_plug **signal_pool = 0;
સ્થિર unsigned signal_pool_ગણતરી = 0;
સ્થિર unsigned long signal_ગણતરી = 0;
સ્થિર unsigned long signal_dels = 0;
#endif

સંરચના vpiSignal_plug {
      unsigned અક્ષર space[કદ (સંરચના __vpiSignal)];
#ifdef CHECK_WITH_VALGRIND
      સંરચના vpiSignal_plug *pool;
#endif
};

શૂન્ય* __vpiSignal::operator નવું(size_t siz)
{
#ifdef CHECK_WITH_VALGRIND
      પ્રતિજ્ઞા(siz == કદ(સંરચના vpiSignal_plug) - કદ(સંરચના vpiSignal_plug *));
#નહીંતર
      પ્રતિજ્ઞા(siz == કદ(સંરચના vpiSignal_plug));
#endif
      સ્થિર સંરચના vpiSignal_plug*alloc_array = 0;
      સ્થિર unsigned alloc_index = 0;
      અચલ unsigned alloc_ગણતરી = 512;

      જો ((alloc_array == 0) || (alloc_index == alloc_ગણતરી)) {
	    alloc_array = static_cast<સંરચના vpiSignal_plug*>
	                  (calloc(alloc_ગણતરી, કદ(સંરચના vpiSignal_plug)));
	    alloc_index = 0;
#ifdef CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_ગણતરી *
	                                            કદ(સંરચના vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_ગણતરી += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_ગણતરી*કદ(vpiSignal_plug **)));
	    signal_pool[signal_pool_ગણતરી-1] = alloc_array;
#endif
      }

      સંરચના vpiSignal_plug*cur = alloc_array + alloc_index;
#ifdef CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, કદ(સંરચના vpiSignal_plug));
      cur->pool = alloc_array;
      signal_ગણતરી += 1;
#endif
      alloc_index += 1;
      પાછા cur;
}

શૂન્ય __vpiSignal::operator delete(શૂન્ય*)
{
      પ્રતિજ્ઞા(0);
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય signal_delete(vpiHandle item)
{
      સંરચના __vpiSignal *obj = static_cast<__vpiSignal *> (item);
      પ્રતિજ્ઞા(obj->ગાંઠ->fil);
      obj->ગાંઠ->fil->clear_all_callbacks();
      vvp_net_delete(obj->ગાંઠ);
      જો (obj->bits) {
	    માટે (unsigned idx=0; idx<obj->width(); idx+=1) {
		  delete obj->bits[idx].index;
	    }
	    obj->bits -= 1;
	    delete [] obj->bits;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

શૂન્ય signal_pool_delete()
{
      જો (RUNNING_ON_VALGRIND && (signal_ગણતરી != signal_dels)) {
	    fflush(ખાલી);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (long) signal_ગણતરી - signal_dels, signal_ગણતરી);
      }

      માટે (unsigned idx = 0; idx < signal_pool_ગણતરી; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    મુક્ત(signal_pool[idx]);
      }

      મુક્ત(signal_pool);
      signal_pool = 0;
      signal_pool_ગણતરી = 0;
}
#endif

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
સ્થિર vpiHandle fill_in_net4(સંરચના __vpiSignal*obj, __vpiScope*scope,
			      અચલ અક્ષર*name, પૂર્ણાંક msb, પૂર્ણાંક lsb,
			      તાર્કિક signed_flag, vvp_net_t*ગાંઠ)
{
      obj->id.name = name? vpip_name_string(name) : ખાલી;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->ગાંઠ = ગાંઠ;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      ગણતરી_vpi_nets += 1;

      પાછા obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 અચલ અક્ષર*name, પૂર્ણાંક msb, પૂર્ણાંક lsb,
			 તાર્કિક signed_flag, vvp_net_t*ગાંઠ)
{
      સંરચના __vpiSignal*obj = નવું signal_net;
      પાછા fill_in_net4(obj, scope, name, msb, lsb, signed_flag, ગાંઠ);
}

સ્થિર પૂર્ણાંક PV_get_base(સંરચના __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      જો (rfp->sbase != 0) {
	    s_vpi_કિંમત val;
	      /* Check to see if the value is defined. */
	    val.format = vpiVectorVal;
	    vpi_get_કિંમત(rfp->sbase, &val);
	    પૂર્ણાંક શબ્દો = (vpi_get(vpiSize, rfp->sbase)-1)/32 + 1;
	    માટે(પૂર્ણાંક idx = 0; idx < શબ્દો; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  જો (val.value.vector[idx].bval != 0) પાછા INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_કિંમત(rfp->sbase, &val);
	    પાછા val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      પાછા rfp->tbase;
}

સ્થિર પૂર્ણાંક PV_get(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      પ્રતિજ્ઞા(rfp);

      પૂર્ણાંક rval = 0;
      પસંદગી (code) {
	સ્થિતિ vpi_લીટી_No:
	    પાછા 0;  // Not implemented for now!

	સ્થિતિ vpiSigned:
	    પાછા 0;  // A part/bit select is always unsigned!

	સ્થિતિ vpiSize:
	    પાછા rfp->width;

	  /* This is like the &A<> in array.cc. */
	સ્થિતિ vpiConstantSelect:
	    પાછા rfp->sbase == 0;

	સ્થિતિ vpi_ડાબું_Range:
            rval += rfp->width - 1;
	    // fallthrough
	સ્થિતિ vpi_જમણું_Range: {
	    પૂર્ણાંક ડાબું = vpi_get(vpi_ડાબું_Range, rfp->parent);
	    પૂર્ણાંક જમણું = vpi_get(vpi_જમણું_Range, rfp->parent);
	    rval += PV_get_base(rfp);
	    પાછા ડાબું >= જમણું ? જમણું + rval : જમણું - rval;
	}

        સ્થિતિ vpiAutomatic:
            પાછા vpi_get(vpiAutomatic, rfp->parent);

#જો defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        સ્થિતિ _vpiFromThr:
            પાછા _vpi_at_PV;
#endif

	મૂળભૂત:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      પાછા 0;
}

સ્થિર અક્ષર* PV_get_str(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      પ્રતિજ્ઞા(rfp);

      પસંદગી (code) {
	સ્થિતિ vpiFile:  // Not implemented for now!
	    પાછા simple_set_rbuf_str(file_names[0]);

	સ્થિતિ vpiName:
	સ્થિતિ vpiFullName: {
	    અચલ અક્ષર*nm = vpi_get_str(code, rfp->parent);
	    size_t len = 256+strlen(nm);
	    અક્ષર *full = static_cast<અક્ષર *>(malloc(len));
	    snprintf(full, len, "%s[%d:%d]", nm,
	                                     (પૂર્ણાંક)vpi_get(vpi_ડાબું_Range, ref),
	                                     (પૂર્ણાંક)vpi_get(vpi_જમણું_Range, ref));
	    full[len-1] = 0;
	    અક્ષર *res = simple_set_rbuf_str(full);
	    મુક્ત(full);
	    પાછા res;
	}

	મૂળભૂત:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      પાછા 0;
}

સ્થિર શૂન્ય PV_get_કિંમત(vpiHandle ref, p_vpi_કિંમત vp)
{
      સંરચના __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      પ્રતિજ્ઞા(rfp);

      vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*>(rfp->net->fil);
      પ્રતિજ્ઞા(sig);

      પસંદગી (vp->format) {

	  સ્થિતિ vpiIntVal:
	    format_vpiIntVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    તોડો;

	  સ્થિતિ vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    તોડો;

	  સ્થિતિ vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    તોડો;

	  સ્થિતિ vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_base(rfp), rfp->width, vp);
	    તોડો;

	  સ્થિતિ vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    તોડો;

	  સ્થિતિ vpiStringVal:
	    format_vpiStringVal(sig, PV_get_base(rfp), rfp->width, vp);
	    તોડો;

	  સ્થિતિ vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_base(rfp), vp);
	    તોડો;

	  સ્થિતિ vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_base(rfp), rfp->width, vp);
	    તોડો;

	  સ્થિતિ vpiVectorVal:
	    format_vpiVectorVal(sig, PV_get_base(rfp), rfp->width, vp);
	    તોડો;

	  સ્થિતિ vpiRealVal:
	    format_vpiRealVal(sig, PV_get_base(rfp), rfp->width, 0, vp);
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (પૂર્ણાંક)vp->format, vpi_get_str(vpiFullName, rfp->parent));
	    પ્રતિજ્ઞા(0);
      }
}

સ્થિર vpiHandle PV_put_કિંમત(vpiHandle ref, p_vpi_કિંમત vp, પૂર્ણાંક flags)
{
      સંરચના __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      પ્રતિજ્ઞા(rfp);
      vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*>(rfp->net->fil);
      પ્રતિજ્ઞા(sig);

      unsigned sig_size = sig->કિંમત_size();
      unsigned width = rfp->width;
      પૂર્ણાંક base = PV_get_base(rfp);
      જો (base >= (signed) sig_size) પાછા 0;
      જો (base + (signed) width < 0) પાછા 0;

      vvp_vector4_t val;
      જો (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_કિંમત(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      જો (base < 0) {
	    width += base;
	    જો (flags != vpiReleaseFlag) {
		  val = val.subvalue(-base, width);
	    }
	    base = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      જો (base+width > sig_size) {
	    width = sig_size - base;
	    જો (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      પ્રતિજ્ઞા(rfp->parent);
      તાર્કિક net_flag = rfp->parent->get_type_code()==vpiNet;
      તાર્કિક full_sig = base == 0 && width == sig_size;

      vvp_net_ptr_t dest(rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value.  Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value.*/
      જો (flags == vpiReleaseFlag) {
	    પ્રતિજ્ઞા(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    જો (full_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } નહીંતર {
		  rfp->net->fil->release_pv(dest, base, width, net_flag);
	    }
	    rfp->net->fun->force_flag(સાચું);
	    PV_get_કિંમત(ref, vp);
	    પાછા ref;
      }

      જો (flags == vpiForceFlag) {
	    જો (full_sig) {
		  vvp_vector2_t mask (vvp_vector2_t::FILL1, sig_size);
		  rfp->net->force_vec4(val, mask);
	    } નહીંતર {
		  vvp_vector2_t mask (vvp_vector2_t::FILL0, sig_size);
		  માટે (unsigned idx = 0 ; idx < width ; idx += 1)
		        mask.set_bit(base+idx, 1);

		  vvp_vector4_t tmp (sig_size, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_કિંમત(tmp);

		  tmp.set_vec(base, val);
		  rfp->net->force_vec4(tmp, mask);
	    }
      } નહીંતર જો (net_flag && !dynamic_cast<vvp_island_port*>(rfp->net->fun)) {
	    જો (full_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } નહીંતર {
		  rfp->net->send_vec4_pv(val, base, sig_size,
		                         vthread_get_wt_context());
	    }
      } નહીંતર {
	    જો (full_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } નહીંતર {
		  vvp_send_vec4_pv(dest, val, base, sig_size,
	                           vthread_get_wt_context());
	    }
      }
      પાછા 0;
}

સ્થિર vpiHandle PV_get_handle(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      પ્રતિજ્ઞા(rfp);

      પસંદગી (code) {
	  સ્થિતિ vpiParent:
	    પાછા rfp->parent;

	  સ્થિતિ vpiScope:
	    પાછા vpi_handle(vpiScope, rfp->parent);

	  સ્થિતિ vpiModule:
	    પાછા vpi_handle(vpiModule, rfp->parent);
      }

      પાછા 0;
}

અંતઃરેખ __vpiPV::__vpiPV()
{ }

પૂર્ણાંક __vpiPV::get_type_code(શૂન્ય) અચલ
{ પાછા vpiPartSelect; }

પૂર્ણાંક __vpiPV::vpi_get(પૂર્ણાંક code)
{ પાછા PV_get(code, આ); }

અક્ષર* __vpiPV::vpi_get_str(પૂર્ણાંક code)
{ પાછા PV_get_str(code, આ); }

શૂન્ય __vpiPV::vpi_get_કિંમત(p_vpi_કિંમત val)
{ PV_get_કિંમત(આ, val); }

vpiHandle __vpiPV::vpi_put_કિંમત(p_vpi_કિંમત val, પૂર્ણાંક flags)
{ પાછા PV_put_કિંમત(આ, val, flags); }

vpiHandle __vpiPV::vpi_handle(પૂર્ણાંક code)
{ પાછા PV_get_handle(code, આ); }

vpiHandle vpip_make_PV(અક્ષર*var, પૂર્ણાંક base, પૂર્ણાંક width)
{
      સંરચના __vpiPV*obj = નવું __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = 0;
      obj->tbase = base;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      પાછા obj;
}

vpiHandle vpip_make_PV(અક્ષર*var, અક્ષર*પ્રતીક, પૂર્ણાંક width)
{
      સંરચના __vpiPV*obj = નવું __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      compile_vpi_lookup(&obj->sbase, પ્રતીક);
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      પાછા obj;
}

vpiHandle vpip_make_PV(અક્ષર*var, vpiHandle handle, પૂર્ણાંક width)
{
      સંરચના __vpiPV*obj = નવું __vpiPV;
      compile_vpi_lookup(&obj->parent, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (unsigned) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      પાછા obj;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય PV_delete(vpiHandle item)
{
      સંરચના __vpiPV *obj = dynamic_cast<__vpiPV*>(item);
      જો (obj->sbase) {
	    પસંદગી (obj->sbase->get_type_code()) {
		સ્થિતિ vpiMemoryWord:
		  જો (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  તોડો;
		સ્થિતિ vpiPartSelect:
		  પ્રતિજ્ઞા(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  તોડો;
	    }
      }
      પ્રતિજ્ઞા(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      delete obj;
}
#endif
