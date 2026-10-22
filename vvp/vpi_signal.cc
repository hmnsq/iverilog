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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <cmath>
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <climits>
# include  <cstring>
# include  <cassert>
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
#అయితే_ముగింపు
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
బాహ్య స్థిరరాశి అక్షరం hex_digits[256];
బాహ్య స్థిరరాశి అక్షరం oct_digits[64];

/*
 * The string values need a result buf to hold the results. This
 * buffer can be reused for that purpose. Whenever I have a need, the
 * need_result_buf function makes sure that need can be met.
 */
శూన్య *need_ఫలితం_buf(గుర్తులేని cnt, vpi_rbuf_t type)
{
      స్థిర శూన్య*ఫలితం_buf[2] = {0, 0};
      స్థిర size_t ఫలితం_buf_కొలత[2] = {0, 0};

      ఒకవేళ (type == RBUF_DEL) {
	    విడుదల(ఫలితం_buf[RBUF_VAL]);
	    ఫలితం_buf[RBUF_VAL] = 0;
	    ఫలితం_buf_కొలత[RBUF_VAL] = 0;

	    విడుదల(ఫలితం_buf[RBUF_STR]);
	    ఫలితం_buf[RBUF_STR] = 0;
	    ఫలితం_buf_కొలత[RBUF_STR] = 0;

	    తిరిగి 0;
      }

      cnt = (cnt + 0x0fff) & ~0x0fff;

      ఒకవేళ (ఫలితం_buf_కొలత[type] == 0) {
	    ఫలితం_buf[type] = జ్ఞాపకం_ఇవ్వు(cnt);
	    ఫలితం_buf_కొలత[type] = cnt;
      } లేకపోతే ఒకవేళ (ఫలితం_buf_కొలత[type] < cnt) {
	    ఫలితం_buf[type] = realloc(ఫలితం_buf[type], cnt);
	    ఫలితం_buf_కొలత[type] = cnt;
      }

      తిరిగి ఫలితం_buf[type];
}

అక్షరం *simple_సమితి_rbuf_str(స్థిరరాశి అక్షరం *s1)
{
      అక్షరం *res = static_cast<అక్షరం *>(need_ఫలితం_buf(అక్షరశ్రేణి_పొడవు(s1)+1, RBUF_STR));
      ఒకవేళ (res) strcpy(res,s1);
      తిరిగి res;
}

అక్షరం *generic_get_str(పూర్ణాంకం code, vpiHandle ref, స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *సూచీ)
{
      size_t len = అక్షరశ్రేణి_పొడవు(name) + 1;  /* include space for null termination */
      అక్షరం *bn = ఖాళీ;
      తార్కిక is_bn_a_pkg = అబద్ధం;
      ఒకవేళ (code == vpi_నిండిన_Name) {
	    bn = strdup(vpi_get_str(code,ref));
	    size_t bn_len = అక్షరశ్రేణి_పొడవు(bn);
	    is_bn_a_pkg = (bn_len >= 3) && (bn[bn_len-1] == ':') && (bn[bn_len-2] == ':');
	    len += bn_len;
	    ఒకవేళ (! is_bn_a_pkg) len += 1; // include space for "." separator
      }
      ఒకవేళ (సూచీ != ఖాళీ) len += అక్షరశ్రేణి_పొడవు(సూచీ) + 2;  /* include space for brackets */

      అక్షరం *res = static_cast<అక్షరం *>(need_ఫలితం_buf(len, RBUF_STR));
      ఒకవేళ (!res) {
	    విడుదల(bn);
	    తిరిగి ఖాళీ;
      }
      *res=0;  /* start with nothing */

	/* if this works, I can make it more efficient later */
      ఒకవేళ (bn != ఖాళీ) {
	    strcat(res, bn);
	      // A package already has the "::" separator in the name
	    ఒకవేళ (! is_bn_a_pkg) strcat(res, ".");
	    విడుదల(bn);
      }
      strcat(res, name);
      ఒకవేళ (సూచీ != ఖాళీ) {
	    strcat(res, "[");
	    strcat(res, సూచీ);
	    strcat(res, "]");
      }
      తిరిగి res;
}

స్థిర vpiHandle fill_లోపల_net4(నిర్మాణం __vpiSignal*obj, __vpiScope*scope,
                              స్థిరరాశి అక్షరం*name, పూర్ణాంకం msb, పూర్ణాంకం lsb,
                              తార్కిక signed_flag, vvp_net_t*కణుపు);

స్థిర vpiHandle fill_లోపల_var4(నిర్మాణం __vpiSignal*obj,
                              స్థిరరాశి అక్షరం*name, పూర్ణాంకం msb, పూర్ణాంకం lsb,
                              తార్కిక signed_flag, vvp_net_t*కణుపు)
{
	// Variable declarations are always resolved immediately,
	// so we can assume they belong in the current scope.
      తిరిగి fill_లోపల_net4(obj, vpip_peek_current_scope(),
			  name, msb, lsb, signed_flag, కణుపు);
}

/*
 * The standard formatting/conversion routines.
 * They work with full or partial signals.
 */

స్థిర శూన్య format_vpiBinStrVal(స్థిరరాశి vvp_signal_విలువ*sig, పూర్ణాంకం పునాది, గుర్తులేని wid,
                                s_vpi_విలువ*vp)
{
      అక్షరం *rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(wid+1, RBUF_VAL));
      దీర్ఘ చివర = పునాది + (గుర్తుగల)wid;
      దీర్ఘ స్థానభ్రంశం_2 = చివర - 1;
      దీర్ఘ ssize = (గుర్తుగల)sig->విలువ_కొలత();

      కొరకు (దీర్ఘ idx = పునాది ;  idx < చివర ;  idx += 1) {
	    ఒకవేళ (idx < 0 || idx >= ssize) {
                  rbuf[స్థానభ్రంశం_2-idx] = 'x';
	    } లేకపోతే {
                  rbuf[స్థానభ్రంశం_2-idx] = vvp_bit4_గమ్యం_ascii(sig->value(idx));
	    }
      }
      rbuf[wid] = 0;

      vp->value.str = rbuf;
}

స్థిర శూన్య format_vpiOctStrVal(స్థిరరాశి vvp_signal_విలువ*sig, పూర్ణాంకం పునాది, గుర్తులేని wid,
                                s_vpi_విలువ*vp)
{
      గుర్తులేని dwid = (wid + 2) / 3;
      అక్షరం *rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(dwid+1, RBUF_VAL));
      దీర్ఘ చివర = పునాది + (గుర్తుగల)wid;
      దీర్ఘ ssize = (గుర్తుగల)sig->విలువ_కొలత();
      గుర్తులేని val = 0;

      rbuf[dwid] = 0;
      కొరకు (దీర్ఘ idx = పునాది ;  idx < చివర ;  idx += 1) {
	    గుర్తులేని బిట్టు = 0;
	    ఒకవేళ (idx < 0 || idx >= ssize) {
                  బిట్టు = 2; // BIT4_X
	    } లేకపోతే {
                  ఎంపిక (sig->value(idx)) {
		      స్థితి BIT4_0:
			బిట్టు = 0;
			ఆపు;
		      స్థితి BIT4_1:
			బిట్టు = 1;
			ఆపు;
		      స్థితి BIT4_X:
			బిట్టు = 2;
			ఆపు;
		      స్థితి BIT4_Z:
			బిట్టు = 3;
			ఆపు;
                  }
	    }
	    val |= బిట్టు << 2*((idx-పునాది) % 3);

	    ఒకవేళ ((idx-పునాది) % 3 == 2) {
		dwid -= 1;
		rbuf[dwid] = oct_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ఎంపిక (wid % 3) {
	  స్థితి 1:
	    ఒకవేళ (val == 2) val = 42;
	    లేకపోతే ఒకవేళ (val == 3) val = 63;
	    ఆపు;
	  స్థితి 2:
	    ఒకవేళ (val == 10) val = 42;
	    లేకపోతే ఒకవేళ (val == 15) val = 63;
	    ఆపు;
      }

      ఒకవేళ (dwid > 0) rbuf[0] = oct_digits[val];

      vp->value.str = rbuf;
}

స్థిర శూన్య format_vpiHexStrVal(స్థిరరాశి vvp_signal_విలువ*sig, పూర్ణాంకం పునాది, గుర్తులేని wid,
                                s_vpi_విలువ*vp)
{
      గుర్తులేని dwid = (wid + 3) / 4;
      అక్షరం *rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(dwid+1, RBUF_VAL));
      దీర్ఘ చివర = పునాది + (గుర్తుగల)wid;
      దీర్ఘ ssize = (గుర్తుగల)sig->విలువ_కొలత();
      గుర్తులేని val = 0;

      rbuf[dwid] = 0;
      కొరకు (దీర్ఘ idx = పునాది ;  idx < చివర ;  idx += 1) {
	    గుర్తులేని బిట్టు = 0;
	    ఒకవేళ (idx < 0 || idx >= ssize) {
                  బిట్టు = 2; // BIT4_X
	    } లేకపోతే {
                  ఎంపిక (sig->value(idx)) {
		      స్థితి BIT4_0:
			బిట్టు = 0;
			ఆపు;
		      స్థితి BIT4_1:
			బిట్టు = 1;
			ఆపు;
		      స్థితి BIT4_X:
			బిట్టు = 2;
			ఆపు;
		      స్థితి BIT4_Z:
			బిట్టు = 3;
			ఆపు;
                  }
	    }
	    val |= బిట్టు << 2*((idx-పునాది) % 4);

	    ఒకవేళ ((idx-పునాది) % 4 == 3) {
		dwid -= 1;
		rbuf[dwid] = hex_digits[val];
		val = 0;
	    }
      }

	/* Fill in X or Z if they are the only thing in the value. */
      ఎంపిక (wid % 4) {
	  స్థితి 1:
	    ఒకవేళ (val == 2) val = 170;
	    లేకపోతే ఒకవేళ (val == 3) val = 255;
	    ఆపు;
	  స్థితి 2:
	    ఒకవేళ (val == 10) val = 170;
	    లేకపోతే ఒకవేళ (val == 15) val = 255;
	    ఆపు;
	  స్థితి 3:
	    ఒకవేళ (val == 42) val = 170;
	    లేకపోతే ఒకవేళ (val == 63) val = 255;
	    ఆపు;
      }

      ఒకవేళ (dwid > 0) rbuf[0] = hex_digits[val];

      vp->value.str = rbuf;
}

స్థిర శూన్య format_vpiDecStrVal(స్థిరరాశి vvp_signal_విలువ*sig, పూర్ణాంకం పునాది, గుర్తులేని wid,
                                పూర్ణాంకం signed_flag, s_vpi_విలువ*vp)
{
      గుర్తులేని hwid = (sig->విలువ_కొలత()+2) / 3 + 1;
      అక్షరం *rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(hwid, RBUF_VAL));
      దీర్ఘ ssize = (గుర్తుగల)sig->విలువ_కొలత();
      దీర్ఘ చివర = పునాది + (గుర్తుగల)wid;

	/* Do we have an end outside of the real signal vector. */
      ఒకవేళ (పునాది < 0 || చివర > ssize) {
	    తార్కిక all_x = నిజం;
	    ఒకవేళ (చివర > ssize) చివర = ssize;
	    ఒకవేళ (పునాది < 0) పునాది = 0;
	    కొరకు (దీర్ఘ idx = పునాది ;  idx < చివర ;  idx += 1) {
		  ఒకవేళ (sig->value(idx) != BIT4_X) {
			all_x = అబద్ధం;
			ఆపు;
		  }
	    }

	    ఒకవేళ (all_x) {
		  rbuf[0] = 'x';
	    } లేకపోతే {
		  rbuf[0] = 'X';
	    }
	    rbuf[1] = 0;

	    vp->value.str = rbuf;
	    తిరిగి;
      }

      vvp_vector4_t vec4;
      ఒకవేళ (పునాది == 0 && చివర == ssize) {
	    sig->vec4_విలువ(vec4);
      } లేకపోతే {
	    vvp_vector4_t tmp;
	    sig->vec4_విలువ(tmp);
	    vec4 = tmp.subvalue(పునాది, wid);
      }

      vpip_vec4_గమ్యం_dec_str(vec4, rbuf, hwid, signed_flag);

      vp->value.str = rbuf;
}

స్థిర శూన్య format_vpiIntVal(స్థిరరాశి vvp_signal_విలువ*sig, పూర్ణాంకం పునాది, గుర్తులేని wid,
                             పూర్ణాంకం signed_flag, s_vpi_విలువ*vp)
{
      vvp_vector4_t tmp;
      sig->vec4_విలువ(tmp);
      vvp_vector4_t sub = tmp.subvalue(పునాది, wid);

	// Normally, we'd be OK with just using long in the call to
	// vector4_to_value, but some compilers seem to take long as
	// distinct from int32_t AND int64_t. Since the condition is
	// constant, the compiler should eliminate the dead code.
      ఒకవేళ (పరిమాణం(vp->value.integer) == పరిమాణం(int32_t)) {
	    int32_t val = 0;
	    vector4_గమ్యం_విలువ(sub, val, signed_flag, అబద్ధం);
	    vp->value.integer = val;
      } లేకపోతే {
	    ప్రతిజ్ఞా(పరిమాణం(vp->value.integer) == పరిమాణం(int64_t));
	    int64_t val = 0;
	    vector4_గమ్యం_విలువ(sub, val, signed_flag, అబద్ధం);
	    vp->value.integer = val;
      }
}

స్థిర శూన్య format_vpiRealVal(స్థిరరాశి vvp_signal_విలువ*sig, పూర్ణాంకం పునాది, గుర్తులేని wid,
                              పూర్ణాంకం signed_flag, s_vpi_విలువ*vp)
{
      vvp_vector4_t vec4(wid);
      దీర్ఘ ssize = (గుర్తుగల)sig->విలువ_కొలత();
      దీర్ఘ చివర = పునాది + (గుర్తుగల)wid;
      ఒకవేళ (చివర > ssize) చివర = ssize;

      కొరకు (దీర్ఘ idx = (పునాది < 0) ? 0 : పునాది ;  idx < చివర ;  idx += 1) {
	    vec4.సమితి_బిట్టు(idx-పునాది, sig->value(idx));
      }

      vp->value.real = 0.0;
      vector4_గమ్యం_విలువ(vec4, vp->value.real, signed_flag);
}

స్థిర శూన్య format_vpiStringVal(స్థిరరాశి vvp_signal_విలువ*sig, పూర్ణాంకం పునాది, గుర్తులేని wid,
                                s_vpi_విలువ*vp)
{
      /* The result will use a character for each 8 bits of the
	 vector. Add one extra character for the highest bits that
	 don't form an 8 bit group. */
      అక్షరం *rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(wid/8 + ((wid&7)!=0) + 1, RBUF_VAL));
      అక్షరం *cp = rbuf;

      అక్షరం tmp = 0;
      కొరకు (దీర్ఘ idx = పునాది+(గుర్తుగల)wid-1; idx >= పునాది; idx -= 1) {
	    tmp <<= 1;

	    ఒకవేళ (idx >=0 && idx < (గుర్తుగల)sig->విలువ_కొలత() &&
	        sig->value(idx) == BIT4_1) {
		   tmp |= 1;
	    }

	    ఒకవేళ (((idx-పునాది)&7)==0){
		  /* Skip leading nulls. */
		  ఒకవేళ (tmp == 0 && cp == rbuf)
			కొనసాగు;

		  /* Nulls in the middle get turned into spaces. */
		  *cp++ = tmp ? tmp : ' ';
		  tmp = 0;
	    }
      }
      *cp++ = 0;

      vp->value.str = rbuf;
}

స్థిర శూన్య format_vpiScalarVal(స్థిరరాశి vvp_signal_విలువ*sig, పూర్ణాంకం పునాది,
                                s_vpi_విలువ*vp)
{
      ఒకవేళ (పునాది >= 0 && పునాది < (గుర్తుగల)sig->విలువ_కొలత()) {
	    ఎంపిక (sig->value(పునాది)) {
		స్థితి BIT4_0:
		  vp->value.scalar = vpi0;
		  ఆపు;
		స్థితి BIT4_1:
		  vp->value.scalar = vpi1;
		  ఆపు;
		స్థితి BIT4_X: {
		  vvp_scalar_t strn = sig->scalar_విలువ(పునాది);
		  ఒకవేళ (strn.strength0() == 1) vp->value.scalar = vpiH;
		  లేకపోతే ఒకవేళ (strn.strength1() == 1) vp->value.scalar = vpiL;
		  లేకపోతే vp->value.scalar = vpiX;
		  ఆపు;
		}
		స్థితి BIT4_Z:
		  vp->value.scalar = vpiZ;
		  ఆపు;
	    }
      } లేకపోతే {
	    vp->value.scalar = vpiX;
      }
}

స్థిర శూన్య format_vpiStrengthVal(స్థిరరాశి vvp_signal_విలువ*sig, పూర్ణాంకం పునాది,
                                  గుర్తులేని wid, s_vpi_విలువ*vp)
{
      దీర్ఘ చివర = పునాది + (గుర్తుగల)wid;
      s_vpi_strengthval*op;

      op = static_cast<s_vpi_strengthval*>
           (need_ఫలితం_buf(wid * పరిమాణం(s_vpi_strengthval), RBUF_VAL));

      కొరకు (దీర్ఘ idx = పునాది ;  idx < చివర ;  idx += 1) {
	    ఒకవేళ (idx >=0 && idx < (గుర్తుగల)sig->విలువ_కొలత()) {
		  vvp_scalar_t val = sig->scalar_విలువ(idx);

		  /* vvp_scalar_t strengths are 0-7, but the vpi strength
		     is bit0-bit7. This gets the vpi form of the strengths
		     from the vvp_scalar_t strengths. */
		  గుర్తులేని s0 = 1 << val.strength0();
		  గుర్తులేని s1 = 1 << val.strength1();

		  ఎంపిక (val.value()) {
		      స్థితి BIT4_0:
			op[idx-పునాది].logic = vpi0;
			op[idx-పునాది].s0 = s0|s1;
			op[idx-పునాది].s1 = 0;
			ఆపు;

		      స్థితి BIT4_1:
			op[idx-పునాది].logic = vpi1;
			op[idx-పునాది].s0 = 0;
			op[idx-పునాది].s1 = s0|s1;
			ఆపు;

		      స్థితి BIT4_X:
			op[idx-పునాది].logic = vpiX;
			op[idx-పునాది].s0 = s0;
			op[idx-పునాది].s1 = s1;
			ఆపు;

		      స్థితి BIT4_Z:
			op[idx-పునాది].logic = vpiZ;
			op[idx-పునాది].s0 = vpiHiZ;
			op[idx-పునాది].s1 = vpiHiZ;
			ఆపు;
		  }
	    } లేకపోతే {
		  op[idx-పునాది].logic = vpiX;
		  op[idx-పునాది].s0 = vpiStrongDrive;
		  op[idx-పునాది].s1 = vpiStrongDrive;
	    }
      }

      vp->value.strength = op;
}

స్థిర శూన్య format_vpi_సదిశ_Val(స్థిరరాశి vvp_signal_విలువ*sig, పూర్ణాంకం పునాది, గుర్తులేని wid,
                                s_vpi_విలువ*vp)
{
      దీర్ఘ చివర = పునాది + (గుర్తుగల)wid;
      గుర్తులేని పూర్ణాంకం obit = 0;
      గుర్తులేని hwid = (wid + 31)/32;

      s_vpi_vecval *op = static_cast<p_vpi_vecval>
                         (need_ఫలితం_buf(hwid * పరిమాణం(s_vpi_vecval), RBUF_VAL));
      vp->value.vector = op;

      op->aval = op->bval = 0;
      కొరకు (దీర్ఘ idx = పునాది ;  idx < చివర ;  idx += 1) {
	    ఒకవేళ (పునాది >= 0 && పునాది < (గుర్తుగల)sig->విలువ_కొలత()) {
		ఎంపిక (sig->value(idx)) {
		స్థితి BIT4_0:
		  op->aval &= ~(1 << obit);
		  op->bval &= ~(1 << obit);
		  ఆపు;
		స్థితి BIT4_1:
		  op->aval |= (1 << obit);
		  op->bval &= ~(1 << obit);
		  ఆపు;
		స్థితి BIT4_X:
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
		  ఆపు;
		స్థితి BIT4_Z:
		  op->aval &= ~(1 << obit);
		  op->bval |= (1 << obit);
		  ఆపు;
		}
	    } లేకపోతే {  /* BIT4_X */
		  op->aval |= (1 << obit);
		  op->bval |= (1 << obit);
	    }

	    obit++;
	    ఒకవేళ (!(obit % 32)) {
		  op += 1;
		  ఒకవేళ ((op - vp->value.vector) < (ptrdiff_t)hwid)
			op->aval = op->bval = 0;
		  obit = 0;
	    }
      }
}

/*
 * implement vpi_get for vpiReg objects.
 */
స్థిర పూర్ణాంకం signal_get(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ప్రతిజ్ఞా(rfp);

      ఎంపిక (code) {
	  స్థితి vpi_పంక్తి_No:
	    తిరిగి 0;  // Not implemented for now!

	  స్థితి vpiSigned:
	    తిరిగి rfp->signed_flag != 0;

	  స్థితి vpiArray:
	    తిరిగి rfp->is_netarray != 0;

	  స్థితి vpi_సూచీ_2: // This only works while we have a single index.
	    ఒకవేళ (rfp->is_netarray) {
		  s_vpi_విలువ vp;
		  vp.format = vpiIntVal;
		  vpi_get_విలువ(rfp->id.సూచీ, &vp);
		  తిరిగి vp.value.integer;
	    } లేకపోతే {
		  తిరిగి vpiUndefined;
	    }

	  స్థితి vpi_కొలత:
	    తిరిగి rfp->width();

	  స్థితి vpiNetType:
	    ఒకవేళ (ref->get_type_సంకేతం()==vpiNet)
		  తిరిగి vpiWire;
	    లేకపోతే
		  తిరిగి vpiUndefined;

	  స్థితి vpi_ఎడమ_Range:
            తిరిగి rfp->msb.get_విలువ();

	  స్థితి vpi_కుడి_Range:
            తిరిగి rfp->lsb.get_విలువ();

	  స్థితి vpiScalar:
	    తిరిగి (rfp->msb.get_విలువ() == 0 && rfp->lsb.get_విలువ() == 0);
	  స్థితి vpi_సదిశ:
	    తిరిగి (rfp->msb.get_విలువ() != rfp->lsb.get_విలువ());

          స్థితి vpiAutomatic:
            తిరిగి vpip_scope(rfp)->is_automatic() ? 1  : 0;

#నిర్వచితమైతే BR916_STOPGAP_FIX
          స్థితి _vpiFromThr:
            తిరిగి _vpiNoThr;
#అయితే_ముగింపు

	  అప్రమేయ:
	    fprintf(stderr, "VPI error: unknown signal_get property %d.\n",
	            code);
	    తిరిగి vpiUndefined;
      }
}

/*
 * implement vpi_get64 for vpiReg objects.
 */
స్థిర int64_t signal_get64(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ప్రతిజ్ఞా(rfp);

      ఎంపిక (code) {
	    // This private property must return zero when undefined.
	  స్థితి _vpiNexusId:
	    ఒకవేళ (rfp->msb.get_విలువ() == rfp->lsb.get_విలువ())
		  తిరిగి reinterpret_cast<int64_t>(rfp->కణుపు);
	    తిరిగి 0;

	  అప్రమేయ:
	    fprintf(stderr, "VPI error: unknown signal_get64 property %d.\n",
	            code);
	    తిరిగి vpiUndefined;
      }
}

స్థిర అక్షరం* signal_get_str(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ప్రతిజ్ఞా(rfp);

      ఒకవేళ (code == vpiFile) {  // Not implemented for now!
	    తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[0]);
      }

      ఒకవేళ ((code != vpiName) && (code != vpi_నిండిన_Name)) తిరిగి ఖాళీ;

      అక్షరం *nm;
      స్థిరరాశి అక్షరం *ixs;
      ఒకవేళ (rfp->is_netarray) {
	    nm = strdup(vpi_get_str(vpiName, rfp->within.మాతృ));
	    s_vpi_విలువ vp;
	    vp.format = vpiDecStrVal;
	    vpi_get_విలువ(rfp->id.సూచీ, &vp);
	    ixs = vp.value.str;  /* do I need to strdup() this? */
      } లేకపోతే {
	    nm = strdup(rfp->id.name);
	    ixs = ఖాళీ;
      }

	/* The scope information is added here for vpiFullName. */
      అక్షరం *rbuf = generic_get_str(code, vpip_scope(rfp), nm, ixs);
      విడుదల(nm);
      తిరిగి rbuf;
}

స్థిర vpiHandle signal_get_handle(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ప్రతిజ్ఞా(rfp);

      ఎంపిక (code) {

	  స్థితి vpi_మాతృ:
	    తిరిగి rfp->is_netarray? rfp->within.మాతృ : ఖాళీ;

	  స్థితి vpi_సూచీ_2:
	    తిరిగి rfp->is_netarray? rfp->id.సూచీ : ఖాళీ;

	  స్థితి vpi_ఎడమ_Range:
	    తిరిగి &rfp->msb;
	  స్థితి vpi_కుడి_Range:
	    తిరిగి &rfp->lsb;

	  స్థితి vpiScope:
	    తిరిగి vpip_scope(rfp);

	  స్థితి vpiModule:
	    తిరిగి vpip_module(vpip_scope(rfp));
      }

      తిరిగి 0;
}

స్థిర vpiHandle signal_iterate(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ప్రతిజ్ఞా(rfp);

      ఒకవేళ (code == vpi_సూచీ_2) {
	    తిరిగి rfp->is_netarray ? rfp->id.సూచీ->vpi_iterate(code) : ఖాళీ;
      }

      తిరిగి 0;
}

శూన్య __vpiSignal::make_బిట్లు()
{
      ప్రతిజ్ఞా(బిట్లు == ఖాళీ);
      బిట్లు = కొత్త నిర్మాణం __vpiBit[width() + 1];

	// Make word[-1] point to the parent.
      బిట్లు->మాతృ = ఇది;
	// Now point to word[0].
      బిట్లు += 1;

	// Update each bit to point to the base
      నిర్మాణం __vpiBit*పునాది = బిట్లు;
      కొరకు (గుర్తులేని idx = 0; idx < width(); idx += 1) {
	    పునాది[idx].bit0 = పునాది;
	    పూర్ణాంకం real_idx;
	    ఒకవేళ (msb.get_విలువ() >= lsb.get_విలువ()) {
		  real_idx = idx + lsb.get_విలువ();
	    } లేకపోతే {
		  real_idx = lsb.get_విలువ() - idx;
	    }
	    పునాది[idx].సూచీ = కొత్త __vpiDecConst(real_idx);
      }
}

vpiHandle __vpiSignal::get_సూచీ(పూర్ణాంకం idx)
{
	/* Check to see if the index is in range. */
      ఒకవేళ (msb.get_విలువ() >= lsb.get_విలువ()) {
	    ఒకవేళ ((idx > msb.get_విలువ()) || (idx < lsb.get_విలువ())) తిరిగి 0;
      } లేకపోతే {
	    ఒకవేళ ((idx < msb.get_విలువ()) || (idx > lsb.get_విలువ())) తిరిగి 0;
      }

	/* Normalize the index */
      గుర్తులేని norm_idx;
      ఒకవేళ (msb.get_విలువ() >= lsb.get_విలువ()) {
	    norm_idx = idx - lsb.get_విలువ();
      } లేకపోతే {
	    norm_idx = lsb.get_విలువ() - idx;
      }

      ఒకవేళ (బిట్లు == ఖాళీ) make_బిట్లు();

      తిరిగి &(బిట్లు[norm_idx].as_బిట్టు);
}

శూన్య __vpiSignal::get_బిట్టు_విలువ(స్థిరరాశి నిర్మాణం __vpiBit*బిట్టు, p_vpi_విలువ vp)
{
      గుర్తులేని సూచీ = బిట్టు->get_norm_సూచీ();

      vvp_signal_విలువ*vsig = dynamic_cast<vvp_signal_విలువ*>(కణుపు->fil);
      ప్రతిజ్ఞా(vsig);

      ఒకవేళ (vp->format == vpiObjTypeVal) {
	    vp->format = vpiIntVal;
      }

      ఎంపిక (vp->format) {
	స్థితి vpiBinStrVal:
            format_vpiBinStrVal(vsig, సూచీ, 1, vp);
	    ఆపు;

	స్థితి vpiOctStrVal:
            format_vpiOctStrVal(vsig, సూచీ, 1, vp);
	    ఆపు;

	స్థితి vpiDecStrVal:
            format_vpiDecStrVal(vsig, సూచీ, 1, అబద్ధం, vp);
	    ఆపు;

	స్థితి vpiHexStrVal:
            format_vpiHexStrVal(vsig, సూచీ, 1, vp);
	    ఆపు;

	స్థితి vpiStringVal:
            format_vpiStringVal(vsig, సూచీ, 1, vp);
	    ఆపు;

	స్థితి vpiIntVal:
            format_vpiIntVal(vsig, సూచీ, 1, అబద్ధం, vp);
	    ఆపు;

	స్థితి vpiRealVal:
            format_vpiRealVal(vsig, సూచీ, 1, అబద్ధం, vp);
	    ఆపు;

	స్థితి vpiScalarVal:
            format_vpiScalarVal(vsig, సూచీ, vp);
	    ఆపు;

        స్థితి vpiStrengthVal:
            format_vpiStrengthVal(vsig, సూచీ, 1, vp);
            ఆపు;

	స్థితి vpi_సదిశ_Val:
            format_vpi_సదిశ_Val(vsig, సూచీ, 1, vp);
	    ఆపు;

	అప్రమేయ:
	    fprintf(stderr, "internal error: get_value() "
		    "type %d is not implemented for bit "
		    "select %s[%d] in scope %s.\n",
		    (పూర్ణాంకం)vp->format, vpi_get_str(vpiName),
		    బిట్టు->get_సూచీ(),
	            vpip_scope(ఇది)->scope_name());
	    ప్రతిజ్ఞా(0);
      };
}

vpiHandle __vpiSignal::put_బిట్టు_విలువ(నిర్మాణం __vpiBit*బిట్టు, p_vpi_విలువ vp, పూర్ణాంకం flags)
{
      గుర్తులేని సూచీ = బిట్టు->get_norm_సూచీ();
      vvp_net_ptr_t dest(కణుపు, 0);
      vvp_vector4_t val = vec4_from_vpi_విలువ(vp, 1);

      ఒకవేళ ((flags == vpiForceFlag) || (flags == vpiReleaseFlag)) {
	    fprintf(stderr, "Sorry: vpi_put_value() for %s does not "
	                    "currently support force/release.\n",
	                    బిట్టు->as_బిట్టు.vpi_get_str(vpi_నిండిన_Name));
	    తిరిగి ఖాళీ;
      }

      ఒకవేళ ((get_type_సంకేతం() == vpiNet) &&
          !dynamic_cast<vvp_island_పోర్టు*>(కణుపు->fun)) {
	    కణుపు->send_vec4_pv(val, సూచీ, width(),
	                            vthread_get_wt_context());
      } లేకపోతే {
	    vvp_send_vec4_pv(dest, val, సూచీ, width(),
	                     vthread_get_wt_context());
      }

	// This is not a scheduled event so there is no event to return
      తిరిగి ఖాళీ;
}

స్థిర vpiHandle signal_సూచీ(పూర్ణాంకం idx, vpiHandle ref)
{
      నిర్మాణం __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ప్రతిజ్ఞా(rfp);

	/* We can only get the bit for a net or reg. */
      PLI_INT32 type = vpi_get(vpiType, ref);
      ఒకవేళ ((type != vpiNet) && (type != vpiReg)) తిరిగి 0;

      తిరిగి rfp->get_సూచీ(idx);
}

గుర్తులేని __vpiSignal::width(శూన్య) స్థిరరాశి
{
      గుర్తులేని wid = (msb.get_విలువ() >= lsb.get_విలువ())
	    ? (msb.get_విలువ() - lsb.get_విలువ() + 1)
	    : (lsb.get_విలువ() - msb.get_విలువ() + 1);

      తిరిగి wid;
}

/*
 * The get_value method reads the values of the functors and returns
 * the vector to the caller. This causes no side-effect, and reads the
 * variables like a %load would.
 */
స్థిర శూన్య signal_get_విలువ(vpiHandle ref, s_vpi_విలువ*vp)
{
      నిర్మాణం __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ప్రతిజ్ఞా(rfp);

      గుర్తులేని wid = rfp->width();

      vvp_signal_విలువ*vsig = dynamic_cast<vvp_signal_విలువ*>(rfp->కణుపు->fil);
      ప్రతిజ్ఞా(vsig);

      ఎంపిక (vp->format) {

	  స్థితి vpiIntVal:
	    format_vpiIntVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ఆపు;

	  స్థితి vpiScalarVal:
	    format_vpiScalarVal(vsig, 0, vp);
	    ఆపు;

	  స్థితి vpiStrengthVal:
	    format_vpiStrengthVal(vsig, 0, wid, vp);
	    ఆపు;

	  స్థితి vpiBinStrVal:
	    format_vpiBinStrVal(vsig, 0, wid, vp);
	    ఆపు;

	  స్థితి vpiHexStrVal:
	    format_vpiHexStrVal(vsig, 0, wid, vp);
	    ఆపు;

	  స్థితి vpiOctStrVal:
	    format_vpiOctStrVal(vsig, 0, wid, vp);
	    ఆపు;

	  స్థితి vpiDecStrVal:
	    format_vpiDecStrVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ఆపు;

	  స్థితి vpiStringVal:
	    format_vpiStringVal(vsig, 0, wid, vp);
	    ఆపు;

	  స్థితి vpi_సదిశ_Val:
	    format_vpi_సదిశ_Val(vsig, 0, wid, vp);
	    ఆపు;

	  స్థితి vpiRealVal:
	    format_vpiRealVal(vsig, 0, wid, rfp->signed_flag, vp);
	    ఆపు;

	  స్థితి vpiObjTypeVal:
	    ఒకవేళ (wid == 1) {
		  vp->format = vpiScalarVal;
		  format_vpiScalarVal(vsig, 0, vp);
	    } లేకపోతే {
		  vp->format = vpi_సదిశ_Val;
		  format_vpi_సదిశ_Val(vsig, 0, wid, vp);
	    }
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "vvp internal error: get_value: "
		    "value type %d not implemented."
		    " Signal is %s in scope %s\n",
		    (పూర్ణాంకం)vp->format, vpi_get_str(vpiName, ref),
		    vpip_scope(rfp)->scope_name());
	    ప్రతిజ్ఞా(0);
      }
}

/*
 * The put_value method writes the value into the vector, and returns
 * the affected ref. This operation works much like the %set or
 * %assign instructions and causes all the side-effects that the
 * equivalent instruction would cause.
 */

స్థిర vvp_vector4_t from_stringval(స్థిరరాశి అక్షరం*str, గుర్తులేని wid)
{
      గుర్తులేని idx;
      స్థిరరాశి అక్షరం*cp;

      cp = str + అక్షరశ్రేణి_పొడవు(str);
      idx = 0;

      vvp_vector4_t val(wid, BIT4_0);

      అయితే ((idx < wid) && (cp > str)) {
	    గుర్తులేని byte = *--cp;
	    పూర్ణాంకం బిట్టు;

	    కొరకు (బిట్టు = 0 ;  (బిట్టు < 8) && (idx < wid) ;  బిట్టు += 1) {
		  ఒకవేళ (byte & 1)
			val.సమితి_బిట్టు(idx, BIT4_1);

		  byte >>= 1;
		  idx += 1;
	    }
      }

      తిరిగి val;
}

స్థిర vpiHandle signal_put_విలువ(vpiHandle ref, s_vpi_విలువ*vp, పూర్ణాంకం flags)
{
      గుర్తులేని wid;
      నిర్మాణం __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ప్రతిజ్ఞా(rfp);
      vvp_net_ptr_t dest(rfp->కణుపు, 0);

      తార్కిక net_flag = ref->get_type_సంకేతం()==vpiNet;

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value. Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value. */
      ఒకవేళ (flags == vpiReleaseFlag) {
	    ప్రతిజ్ఞా(rfp->కణుపు->fil);
	    rfp->కణుపు->fil->force_unlink();
	    rfp->కణుపు->fil->release(dest, net_flag);
	    rfp->కణుపు->fun->force_flag(నిజం);
	    signal_get_విలువ(ref, vp);
	    తిరిగి ref;
      }

	/* Make a vvp_vector4_t vector to receive the translated value
	   that we are going to poke. This will get populated
	   differently depending on the format. */
      wid = (rfp->msb.get_విలువ() >= rfp->lsb.get_విలువ())
	    ? (rfp->msb.get_విలువ() - rfp->lsb.get_విలువ() + 1)
	    : (rfp->lsb.get_విలువ() - rfp->msb.get_విలువ() + 1);

      vvp_vector4_t val = vec4_from_vpi_విలువ(vp, wid);

      ఒకవేళ (flags == vpiForceFlag) {
	    vvp_vector2_t ముసుగు (vvp_vector2_t::FILL1, wid);
	    rfp->కణుపు->force_vec4(val, ముసుగు);
      } లేకపోతే ఒకవేళ (net_flag && !dynamic_cast<vvp_island_పోర్టు*>(rfp->కణుపు->fun)) {
	    rfp->కణుపు->send_vec4(val, vthread_get_wt_context());
      } లేకపోతే {
	    vvp_send_vec4(dest, val, vthread_get_wt_context());
      }
      తిరిగి ref;
}

vvp_vector4_t vec4_from_vpi_విలువ(s_vpi_విలువ*vp, గుర్తులేని wid)
{
      vvp_vector4_t val (wid, BIT4_0);

      ఎంపిక (vp->format) {

	  స్థితి vpiIntVal: {
		దీర్ఘ vpi_val = vp->value.integer;
		కొరకు (గుర్తులేని idx = 0 ;  idx < wid ;  idx += 1) {
		      vvp_bit4_t బిట్టు = (vpi_val & 1) ? BIT4_1 : BIT4_0;
		      val.సమితి_బిట్టు(idx, బిట్టు);
		      vpi_val >>= 1;
		}
		ఆపు;
	  }

	  స్థితి vpi_సదిశ_Val:
	    కొరకు (గుర్తులేని idx = 0 ;  idx < wid ;  idx += 1) {
		  గుర్తులేని దీర్ఘ aval = vp->value.vector[idx/32].aval;
		  గుర్తులేని దీర్ఘ bval = vp->value.vector[idx/32].bval;
		  aval >>= idx%32;
		  bval >>= idx%32;
		  పూర్ణాంకం bitmask = (aval&1) | ((bval<<1)&2);
		  vvp_bit4_t బిట్టు = scalar_గమ్యం_bit4(bitmask);
		  val.సమితి_బిట్టు(idx, బిట్టు);
	    }
	    ఆపు;
	  స్థితి vpiBinStrVal:
	    vpip_bin_str_గమ్యం_vec4(val, vp->value.str);
	    ఆపు;
	  స్థితి vpiOctStrVal:
	    vpip_oct_str_గమ్యం_vec4(val, vp->value.str);
	    ఆపు;
	  స్థితి vpiDecStrVal:
	    vpip_dec_str_గమ్యం_vec4(val, vp->value.str);
	    ఆపు;
	  స్థితి vpiHexStrVal:
	    vpip_hex_str_గమ్యం_vec4(val, vp->value.str);
	    ఆపు;
	  స్థితి vpiScalarVal:
	    val.సమితి_బిట్టు(0, scalar_గమ్యం_bit4(vp->value.scalar));
	    ఆపు;
	  స్థితి vpiStringVal:
	    val = from_stringval(vp->value.str, wid);
	    ఆపు;
	  స్థితి vpiRealVal:
	    val = vvp_vector4_t(wid, vp->value.real);
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "vvp internal error: put_value: "
		    "value type %d not implemented here.\n",
		    (పూర్ణాంకం)vp->format);
	    ప్రతిజ్ఞా(0);

      }

      తిరిగి val;
}

పూర్ణాంకం __vpiSignal::vpi_get(పూర్ణాంకం code)
{ తిరిగి signal_get(code, ఇది); }

int64_t __vpiSignal::vpi_get64(పూర్ణాంకం code)
{ తిరిగి signal_get64(code, ఇది); }

అక్షరం* __vpiSignal::vpi_get_str(పూర్ణాంకం code)
{ తిరిగి signal_get_str(code, ఇది); }

శూన్య __vpiSignal::vpi_get_విలువ(p_vpi_విలువ val)
{ signal_get_విలువ(ఇది, val); }

vpiHandle __vpiSignal::vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags)
{ తిరిగి signal_put_విలువ(ఇది, val, flags); }

vpiHandle __vpiSignal::vpi_handle(పూర్ణాంకం code)
{ తిరిగి signal_get_handle(code, ఇది); }

vpiHandle __vpiSignal::vpi_iterate(పూర్ణాంకం code)
{ తిరిగి signal_iterate(code, ఇది); }

vpiHandle __vpiSignal::vpi_సూచీ(పూర్ణాంకం idx)
{ తిరిగి signal_సూచీ(idx, ఇది); }

నిర్మాణం signal_reg : బహిరంగ __vpiSignal {
      అంతఃవరుస signal_reg() { }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiReg; }
};

నిర్మాణం signal_integer : బహిరంగ __vpiSignal {
      అంతఃవరుస signal_integer() { }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiIntegerVar; }
};

నిర్మాణం signal_net : బహిరంగ __vpiSignal {
      అంతఃవరుస signal_net() { }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiNet; }
};

నిర్మాణం signal_byte : బహిరంగ __vpiSignal {
      అంతఃవరుస signal_byte() { }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiByteVar; }
};

నిర్మాణం signal_bitvar : బహిరంగ __vpiSignal {
      అంతఃవరుస signal_bitvar() { }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpi_బిట్టు_Var; }
};

నిర్మాణం signal_shortint : బహిరంగ __vpiSignal {
      అంతఃవరుస signal_shortint() { }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiShortIntVar; }
};

నిర్మాణం signal_int : బహిరంగ __vpiSignal {
      అంతఃవరుస signal_int() { }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiIntVar; }
};

నిర్మాణం signal_longint : బహిరంగ __vpiSignal {
      అంతఃవరుస signal_longint() { }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiLongIntVar; }
};


/*
 * Construct a vpiIntegerVar object. Indicate the type using a flag
 * to minimize the code modifications. Icarus implements integers
 * as 'reg signed [31:0]'.
 */
vpiHandle vpip_make_int4(స్థిరరాశి అక్షరం*name, పూర్ణాంకం msb, పూర్ణాంకం lsb, vvp_net_t*vec)
{
      __vpiSignal*obj = కొత్త signal_integer;
      తిరిగి fill_లోపల_var4(obj, name, msb, lsb, నిజం, vec);
}

/*
 * Construct the two-state SystemVerilog variables.
 */
vpiHandle vpip_make_int2(స్థిరరాశి అక్షరం*name, పూర్ణాంకం msb, పూర్ణాంకం lsb, తార్కిక signed_flag,
                         vvp_net_t*vec)
{
      __vpiSignal*obj;

	// All unsigned 2-state variables are a vpiBitVar. All 2-state
	// variables with a non-zero lsb are also a vpiBitVar.
      ఒకవేళ ((! signed_flag) || (lsb != 0) ) {
	    obj = కొత్త signal_bitvar;
      } లేకపోతే {
	      // These could also be bit declarations with matching
	      // information, but for now they get the apparent type.
	    ఎంపిక (msb) {
		స్థితి 7:
		  obj = కొత్త signal_byte;
		  ఆపు;
		స్థితి 15:
		  obj = కొత్త signal_shortint;
		  ఆపు;
		స్థితి 31:
		  obj = కొత్త signal_int;
		  ఆపు;
		స్థితి 63:
		  obj = కొత్త signal_longint;
		  ఆపు;
		అప్రమేయ:
		    // Every other type of bit vector is a vpiBitVar with
		    // array dimensions.
		  obj = కొత్త signal_bitvar;
		  ఆపు;
	    }
      }

      తిరిగి fill_లోపల_var4(obj, name, msb, lsb, signed_flag, vec);
}

/*
 * Construct a vpiReg/vpiLogicVar object. It's like a net, except for the type.
 */
vpiHandle vpip_make_var4(స్థిరరాశి అక్షరం*name, పూర్ణాంకం msb, పూర్ణాంకం lsb,
			తార్కిక signed_flag, vvp_net_t*vec)
{
      __vpiSignal*obj = కొత్త signal_reg;
      తిరిగి fill_లోపల_var4(obj, name, msb, lsb, signed_flag, vec);
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
స్థిర నిర్మాణం vpiSignal_plug **signal_pool = 0;
స్థిర గుర్తులేని signal_pool_లెక్క = 0;
స్థిర గుర్తులేని దీర్ఘ signal_లెక్క = 0;
స్థిర గుర్తులేని దీర్ఘ signal_dels = 0;
#అయితే_ముగింపు

నిర్మాణం vpiSignal_plug {
      గుర్తులేని అక్షరం space[పరిమాణం (నిర్మాణం __vpiSignal)];
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      నిర్మాణం vpiSignal_plug *pool;
#అయితే_ముగింపు
};

శూన్య* __vpiSignal::క్రియాకారకం కొత్త(size_t siz)
{
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      ప్రతిజ్ఞా(siz == పరిమాణం(నిర్మాణం vpiSignal_plug) - పరిమాణం(నిర్మాణం vpiSignal_plug *));
#లేకపోతే
      ప్రతిజ్ఞా(siz == పరిమాణం(నిర్మాణం vpiSignal_plug));
#అయితే_ముగింపు
      స్థిర నిర్మాణం vpiSignal_plug*alloc_array = 0;
      స్థిర గుర్తులేని alloc_సూచీ = 0;
      స్థిరరాశి గుర్తులేని alloc_లెక్క = 512;

      ఒకవేళ ((alloc_array == 0) || (alloc_సూచీ == alloc_లెక్క)) {
	    alloc_array = static_cast<నిర్మాణం vpiSignal_plug*>
	                  (సున్నా_జ్ఞాపకం_ఇవ్వు(alloc_లెక్క, పరిమాణం(నిర్మాణం vpiSignal_plug)));
	    alloc_సూచీ = 0;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(alloc_array, alloc_లెక్క *
	                                            పరిమాణం(నిర్మాణం vpiSignal_plug));
	    VALGRIND_CREATE_MEMPOOL(alloc_array, 0, 1);
	    signal_pool_లెక్క += 1;
	    signal_pool = static_cast<vpiSignal_plug **>
	                  (realloc(signal_pool,
	                   signal_pool_లెక్క*పరిమాణం(vpiSignal_plug **)));
	    signal_pool[signal_pool_లెక్క-1] = alloc_array;
#అయితే_ముగింపు
      }

      నిర్మాణం vpiSignal_plug*cur = alloc_array + alloc_సూచీ;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(alloc_array, cur, పరిమాణం(నిర్మాణం vpiSignal_plug));
      cur->pool = alloc_array;
      signal_లెక్క += 1;
#అయితే_ముగింపు
      alloc_సూచీ += 1;
      తిరిగి cur;
}

శూన్య __vpiSignal::క్రియాకారకం తొలగించు(శూన్య*)
{
      ప్రతిజ్ఞా(0);
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య signal_delete(vpiHandle అంశం)
{
      నిర్మాణం __vpiSignal *obj = static_cast<__vpiSignal *> (అంశం);
      ప్రతిజ్ఞా(obj->కణుపు->fil);
      obj->కణుపు->fil->clear_all_callbacks();
      vvp_net_delete(obj->కణుపు);
      ఒకవేళ (obj->బిట్లు) {
	    కొరకు (గుర్తులేని idx=0; idx<obj->width(); idx+=1) {
		  తొలగించు obj->బిట్లు[idx].సూచీ;
	    }
	    obj->బిట్లు -= 1;
	    తొలగించు [] obj->బిట్లు;
      }
      signal_dels += 1;
      VALGRIND_MEMPOOL_FREE(reinterpret_cast<vpiSignal_plug *>(obj)->pool, obj);
}

శూన్య signal_pool_delete()
{
      ఒకవేళ (RUNNING_ON_VALGRIND && (signal_లెక్క != signal_dels)) {
	    fflush(ఖాళీ);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu signal(s).",
	                    (దీర్ఘ) signal_లెక్క - signal_dels, signal_లెక్క);
      }

      కొరకు (గుర్తులేని idx = 0; idx < signal_pool_లెక్క; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(signal_pool[idx]);
	    విడుదల(signal_pool[idx]);
      }

      విడుదల(signal_pool);
      signal_pool = 0;
      signal_pool_లెక్క = 0;
}
#అయితే_ముగింపు

/*
 * Construct a vpiNet object. Give the object specified dimensions,
 * and point to the specified functor for the lsb.
 *
 * The name is the PLI name for the object. If it is an array it is
 * <name>[<index>].
 */
స్థిర vpiHandle fill_లోపల_net4(నిర్మాణం __vpiSignal*obj, __vpiScope*scope,
			      స్థిరరాశి అక్షరం*name, పూర్ణాంకం msb, పూర్ణాంకం lsb,
			      తార్కిక signed_flag, vvp_net_t*కణుపు)
{
      obj->id.name = name? vpip_name_string(name) : ఖాళీ;
      obj->msb = __vpiDecConst(msb);
      obj->lsb = __vpiDecConst(lsb);
      obj->signed_flag = signed_flag? 1 : 0;
      obj->is_netarray = 0;
      obj->కణుపు = కణుపు;

	// Place this object within a scope. If this object is
	// attached to an array, then this value will be replaced with
	// the handle to the parent.
      obj->within.scope = scope;

      లెక్క_vpi_nets += 1;

      తిరిగి obj;
}

vpiHandle vpip_make_net4(__vpiScope*scope,
			 స్థిరరాశి అక్షరం*name, పూర్ణాంకం msb, పూర్ణాంకం lsb,
			 తార్కిక signed_flag, vvp_net_t*కణుపు)
{
      నిర్మాణం __vpiSignal*obj = కొత్త signal_net;
      తిరిగి fill_లోపల_net4(obj, scope, name, msb, lsb, signed_flag, కణుపు);
}

స్థిర పూర్ణాంకం PV_get_పునాది(నిర్మాణం __vpiPV*rfp)
{
	/* We return from the symbol base if it is defined. */
      ఒకవేళ (rfp->sbase != 0) {
	    s_vpi_విలువ val;
	      /* Check to see if the value is defined. */
	    val.format = vpi_సదిశ_Val;
	    vpi_get_విలువ(rfp->sbase, &val);
	    పూర్ణాంకం పదాలు = (vpi_get(vpi_కొలత, rfp->sbase)-1)/32 + 1;
	    కొరకు(పూర్ణాంకం idx = 0; idx < పదాలు; idx += 1) {
		    /* Return INT_MIN to indicate an X base. */
		  ఒకవేళ (val.value.vector[idx].bval != 0) తిరిగి INT_MIN;
	    }
	      /* The value is defined so get and return it. */
	    val.format = vpiIntVal;
	    vpi_get_విలువ(rfp->sbase, &val);
	    తిరిగి val.value.integer;
      }

	/* If the width is zero then tbase is the constant. */
      తిరిగి rfp->tbase;
}

స్థిర పూర్ణాంకం PV_get(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ప్రతిజ్ఞా(rfp);

      పూర్ణాంకం rval = 0;
      ఎంపిక (code) {
	స్థితి vpi_పంక్తి_No:
	    తిరిగి 0;  // Not implemented for now!

	స్థితి vpiSigned:
	    తిరిగి 0;  // A part/bit select is always unsigned!

	స్థితి vpi_కొలత:
	    తిరిగి rfp->width;

	  /* This is like the &A<> in array.cc. */
	స్థితి vpiConstantSelect:
	    తిరిగి rfp->sbase == 0;

	స్థితి vpi_ఎడమ_Range:
            rval += rfp->width - 1;
	    // fallthrough
	స్థితి vpi_కుడి_Range: {
	    పూర్ణాంకం ఎడమ = vpi_get(vpi_ఎడమ_Range, rfp->మాతృ);
	    పూర్ణాంకం కుడి = vpi_get(vpi_కుడి_Range, rfp->మాతృ);
	    rval += PV_get_పునాది(rfp);
	    తిరిగి ఎడమ >= కుడి ? కుడి + rval : కుడి - rval;
	}

        స్థితి vpiAutomatic:
            తిరిగి vpi_get(vpiAutomatic, rfp->మాతృ);

#ఒకవేళ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
        స్థితి _vpiFromThr:
            తిరిగి _vpi_at_PV;
#అయితే_ముగింపు

	అప్రమేయ:
	    fprintf(stderr, "PV_get: property %d is unknown\n", code);
      }

      తిరిగి 0;
}

స్థిర అక్షరం* PV_get_str(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ప్రతిజ్ఞా(rfp);

      ఎంపిక (code) {
	స్థితి vpiFile:  // Not implemented for now!
	    తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[0]);

	స్థితి vpiName:
	స్థితి vpi_నిండిన_Name: {
	    స్థిరరాశి అక్షరం*nm = vpi_get_str(code, rfp->మాతృ);
	    size_t len = 256+అక్షరశ్రేణి_పొడవు(nm);
	    అక్షరం *నిండిన = static_cast<అక్షరం *>(జ్ఞాపకం_ఇవ్వు(len));
	    snprintf(నిండిన, len, "%s[%d:%d]", nm,
	                                     (పూర్ణాంకం)vpi_get(vpi_ఎడమ_Range, ref),
	                                     (పూర్ణాంకం)vpi_get(vpi_కుడి_Range, ref));
	    నిండిన[len-1] = 0;
	    అక్షరం *res = simple_సమితి_rbuf_str(నిండిన);
	    విడుదల(నిండిన);
	    తిరిగి res;
	}

	అప్రమేయ:
	    fprintf(stderr, "PV_get_str: property %d is unknown.\n", code);
      }

      తిరిగి 0;
}

స్థిర శూన్య PV_get_విలువ(vpiHandle ref, p_vpi_విలువ vp)
{
      నిర్మాణం __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ప్రతిజ్ఞా(rfp);

      vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*>(rfp->net->fil);
      ప్రతిజ్ఞా(sig);

      ఎంపిక (vp->format) {

	  స్థితి vpiIntVal:
	    format_vpiIntVal(sig, PV_get_పునాది(rfp), rfp->width, 0, vp);
	    ఆపు;

	  స్థితి vpiBinStrVal:
	    format_vpiBinStrVal(sig, PV_get_పునాది(rfp), rfp->width, vp);
	    ఆపు;

	  స్థితి vpiOctStrVal:
	    format_vpiOctStrVal(sig, PV_get_పునాది(rfp), rfp->width, vp);
	    ఆపు;

	  స్థితి vpiHexStrVal:
	    format_vpiHexStrVal(sig, PV_get_పునాది(rfp), rfp->width, vp);
	    ఆపు;

	  స్థితి vpiDecStrVal:
	    format_vpiDecStrVal(sig, PV_get_పునాది(rfp), rfp->width, 0, vp);
	    ఆపు;

	  స్థితి vpiStringVal:
	    format_vpiStringVal(sig, PV_get_పునాది(rfp), rfp->width, vp);
	    ఆపు;

	  స్థితి vpiScalarVal:
	    format_vpiScalarVal(sig, PV_get_పునాది(rfp), vp);
	    ఆపు;

	  స్థితి vpiStrengthVal:
	    format_vpiStrengthVal(sig, PV_get_పునాది(rfp), rfp->width, vp);
	    ఆపు;

	  స్థితి vpi_సదిశ_Val:
	    format_vpi_సదిశ_Val(sig, PV_get_పునాది(rfp), rfp->width, vp);
	    ఆపు;

	  స్థితి vpiRealVal:
	    format_vpiRealVal(sig, PV_get_పునాది(rfp), rfp->width, 0, vp);
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "vvp internal error: PV_get_value: "
		    "value type %d not implemented. Signal is %s.\n",
		    (పూర్ణాంకం)vp->format, vpi_get_str(vpi_నిండిన_Name, rfp->మాతృ));
	    ప్రతిజ్ఞా(0);
      }
}

స్థిర vpiHandle PV_put_విలువ(vpiHandle ref, p_vpi_విలువ vp, పూర్ణాంకం flags)
{
      నిర్మాణం __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ప్రతిజ్ఞా(rfp);
      vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*>(rfp->net->fil);
      ప్రతిజ్ఞా(sig);

      గుర్తులేని sig_కొలత = sig->విలువ_కొలత();
      గుర్తులేని width = rfp->width;
      పూర్ణాంకం పునాది = PV_get_పునాది(rfp);
      ఒకవేళ (పునాది >= (గుర్తుగల) sig_కొలత) తిరిగి 0;
      ఒకవేళ (పునాది + (గుర్తుగల) width < 0) తిరిగి 0;

      vvp_vector4_t val;
      ఒకవేళ (flags != vpiReleaseFlag) {
	    val = vec4_from_vpi_విలువ(vp, width);
      }

	/*
	 * If the base is less than zero then trim off any unneeded
	 * lower bits.
	 */
      ఒకవేళ (పునాది < 0) {
	    width += పునాది;
	    ఒకవేళ (flags != vpiReleaseFlag) {
		  val = val.subvalue(-పునాది, width);
	    }
	    పునాది = 0;
      }

	/*
	 * If the value is wider than the signal then trim off any
	 * unneeded upper bits.
	 */
      ఒకవేళ (పునాది+width > sig_కొలత) {
	    width = sig_కొలత - పునాది;
	    ఒకవేళ (flags != vpiReleaseFlag) {
		  val = val.subvalue(0, width);
	    }
      }

      ప్రతిజ్ఞా(rfp->మాతృ);
      తార్కిక net_flag = rfp->మాతృ->get_type_సంకేతం()==vpiNet;
      తార్కిక నిండిన_sig = పునాది == 0 && width == sig_కొలత;

      vvp_net_ptr_t dest(rfp->net, 0);

	/* If this is a release, then we are not really putting a
	   value. Instead, issue a release "command" to the signal
	   node to cause it to release a forced value.  Note that
	   if this net is attached to an island, we need to rerun
	   the calculations immediately so we can return the
	   released value.*/
      ఒకవేళ (flags == vpiReleaseFlag) {
	    ప్రతిజ్ఞా(rfp->net->fil);
	      // XXXX Can't really do this if this is a partial release?
	    rfp->net->fil->force_unlink();
	    ఒకవేళ (నిండిన_sig) {
		  rfp->net->fil->release(dest, net_flag);
	    } లేకపోతే {
		  rfp->net->fil->release_pv(dest, పునాది, width, net_flag);
	    }
	    rfp->net->fun->force_flag(నిజం);
	    PV_get_విలువ(ref, vp);
	    తిరిగి ref;
      }

      ఒకవేళ (flags == vpiForceFlag) {
	    ఒకవేళ (నిండిన_sig) {
		  vvp_vector2_t ముసుగు (vvp_vector2_t::FILL1, sig_కొలత);
		  rfp->net->force_vec4(val, ముసుగు);
	    } లేకపోతే {
		  vvp_vector2_t ముసుగు (vvp_vector2_t::FILL0, sig_కొలత);
		  కొరకు (గుర్తులేని idx = 0 ; idx < width ; idx += 1)
		        ముసుగు.సమితి_బిట్టు(పునాది+idx, 1);

		  vvp_vector4_t tmp (sig_కొలత, BIT4_Z);

		    // vvp_net_t::force_vec4 propagates all the bits of the
		    // forced vector value, regardless of the mask. This
		    // ensures the unforced bits retain their current value.
		  sig->vec4_విలువ(tmp);

		  tmp.సమితి_vec(పునాది, val);
		  rfp->net->force_vec4(tmp, ముసుగు);
	    }
      } లేకపోతే ఒకవేళ (net_flag && !dynamic_cast<vvp_island_పోర్టు*>(rfp->net->fun)) {
	    ఒకవేళ (నిండిన_sig) {
		  rfp->net->send_vec4(val, vthread_get_wt_context());
	    } లేకపోతే {
		  rfp->net->send_vec4_pv(val, పునాది, sig_కొలత,
		                         vthread_get_wt_context());
	    }
      } లేకపోతే {
	    ఒకవేళ (నిండిన_sig) {
		  vvp_send_vec4(dest, val, vthread_get_wt_context());
	    } లేకపోతే {
		  vvp_send_vec4_pv(dest, val, పునాది, sig_కొలత,
	                           vthread_get_wt_context());
	    }
      }
      తిరిగి 0;
}

స్థిర vpiHandle PV_get_handle(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiPV*rfp = dynamic_cast<__vpiPV*>(ref);
      ప్రతిజ్ఞా(rfp);

      ఎంపిక (code) {
	  స్థితి vpi_మాతృ:
	    తిరిగి rfp->మాతృ;

	  స్థితి vpiScope:
	    తిరిగి vpi_handle(vpiScope, rfp->మాతృ);

	  స్థితి vpiModule:
	    తిరిగి vpi_handle(vpiModule, rfp->మాతృ);
      }

      తిరిగి 0;
}

అంతఃవరుస __vpiPV::__vpiPV()
{ }

పూర్ణాంకం __vpiPV::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiPartSelect; }

పూర్ణాంకం __vpiPV::vpi_get(పూర్ణాంకం code)
{ తిరిగి PV_get(code, ఇది); }

అక్షరం* __vpiPV::vpi_get_str(పూర్ణాంకం code)
{ తిరిగి PV_get_str(code, ఇది); }

శూన్య __vpiPV::vpi_get_విలువ(p_vpi_విలువ val)
{ PV_get_విలువ(ఇది, val); }

vpiHandle __vpiPV::vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags)
{ తిరిగి PV_put_విలువ(ఇది, val, flags); }

vpiHandle __vpiPV::vpi_handle(పూర్ణాంకం code)
{ తిరిగి PV_get_handle(code, ఇది); }

vpiHandle vpip_make_PV(అక్షరం*var, పూర్ణాంకం పునాది, పూర్ణాంకం width)
{
      నిర్మాణం __vpiPV*obj = కొత్త __vpiPV;
      compile_vpi_lookup(&obj->మాతృ, strdup(var));
      obj->sbase = 0;
      obj->tbase = పునాది;
      obj->width = (గుర్తులేని) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      తిరిగి obj;
}

vpiHandle vpip_make_PV(అక్షరం*var, అక్షరం*చిహ్నం, పూర్ణాంకం width)
{
      నిర్మాణం __vpiPV*obj = కొత్త __vpiPV;
      compile_vpi_lookup(&obj->మాతృ, strdup(var));
      compile_vpi_lookup(&obj->sbase, చిహ్నం);
      obj->tbase = 0;
      obj->width = (గుర్తులేని) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      తిరిగి obj;
}

vpiHandle vpip_make_PV(అక్షరం*var, vpiHandle handle, పూర్ణాంకం width)
{
      నిర్మాణం __vpiPV*obj = కొత్త __vpiPV;
      compile_vpi_lookup(&obj->మాతృ, strdup(var));
      obj->sbase = handle;
      obj->tbase = 0;
      obj->width = (గుర్తులేని) width;
      obj->net = 0;
      functor_ref_lookup(&obj->net, var);

      తిరిగి obj;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య PV_delete(vpiHandle అంశం)
{
      నిర్మాణం __vpiPV *obj = dynamic_cast<__vpiPV*>(అంశం);
      ఒకవేళ (obj->sbase) {
	    ఎంపిక (obj->sbase->get_type_సంకేతం()) {
		స్థితి vpi_జ్ఞాపకం_మాట:
		  ఒకవేళ (vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_A) {
			A_delete(obj->sbase);
		  }
		  ఆపు;
		స్థితి vpiPartSelect:
		  ప్రతిజ్ఞా(vpi_get(_vpiFromThr, obj->sbase) == _vpi_at_PV);
		  PV_delete(obj->sbase);
		  ఆపు;
	    }
      }
      ప్రతిజ్ఞా(obj->net->fil);
      obj->net->fil->clear_all_callbacks();
      తొలగించు obj;
}
#అయితే_ముగింపు
