/*
 * Copyright (c) 2001-2025 Stephen Williams (steve@icarus.com)
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
 * This file keeps the table of system/task definitions. This table is
 * built up before the input source file is parsed, and is used by the
 * compiler when %vpi_call statements are encountered.
 */
# include  "vpi_priv.h"
# include  "vthread.h"
# include  "compile.h"
# include  "config.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

ਸਤਰਅੰਦਰ __vpiUserSystf::__vpiUserSystf()
{ }

ਪੂਰਨ __vpiUserSystf::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiUserSystf; }


ਸਥਿਰ vpiHandle systask_handle(ਪੂਰਨ type, vpiHandle ref)
{
      ਬਣਤਰ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ਚੋਣ (type) {
	  ਹਾਲਤ vpiScope:
	    ਵਾਪਸ rfp->scope;

	  ਹਾਲਤ vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    ਪ੍ਰਤਿਜ੍ਞਾ(rfp->defn->is_user_defn);
	    ਵਾਪਸ rfp->defn;

	  ਮੂਲ:
	    ਵਾਪਸ 0;
      };
}

ਸਥਿਰ ਪੂਰਨ systask_get(ਪੂਰਨ type, vpiHandle ref)
{
      ਬਣਤਰ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ਚੋਣ (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  ਹਾਲਤ vpi_ਸਮਾਂ_Unit:
	    ਵਾਪਸ rfp->scope->ਸਮਾਂ_units;
	  ਹਾਲਤ vpi_ਸਮਾਂ_Precision:
	    ਵਾਪਸ rfp->scope->ਸਮਾਂ_precision;

	  ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ rfp->lineno;

	  ਹਾਲਤ vpiUserDefn:
	    ਵਾਪਸ rfp->defn->is_user_defn;

	  ਮੂਲ:
	    ਵਾਪਸ vpiUndefined;
      }
}

// support getting vpiSize for a system function call
ਸਥਿਰ ਪੂਰਨ sysfunc_get(ਪੂਰਨ type, vpiHandle ref)
{
      ਬਣਤਰ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ਚੋਣ (type) {
	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਪ੍ਰਤਿਜ੍ਞਾ(0); // This should be handled by derived classes
	    ਵਾਪਸ 0;

	  ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ rfp->lineno;

	  ਹਾਲਤ vpiUserDefn:
	    ਵਾਪਸ rfp->defn->is_user_defn;

	  ਮੂਲ:
	    ਵਾਪਸ vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

ਸਥਿਰ ਅੱਖਰ *systask_get_str(ਪੂਰਨ type, vpiHandle ref)
{
      ਅਚਲ ਬਣਤਰ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ਚੋਣ (type) {
          ਹਾਲਤ vpiFile:
            ਪ੍ਰਤਿਜ੍ਞਾ(rfp->file_idx < file_ਨਾਂ.size());
            ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[rfp->file_idx]);

          ਹਾਲਤ vpiName:
            ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(rfp->defn->info.tfname);
      }

      ਵਾਪਸ 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
ਸਥਿਰ vpiHandle systask_iter(ਪੂਰਨ, vpiHandle ref)
{
      ਬਣਤਰ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ਜੇ (rfp->nargs == 0)
	    ਵਾਪਸ 0;

      ਵਾਪਸ vpip_make_iterator(rfp->nargs, rfp->ਦਲੀਲਾਂ, ਝੂਠ);
}

ਬਣਤਰ systask_def : ਜਨਤਕ __vpiSysTaskCall {
      ਆਭਾਸੀ ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ   override { ਵਾਪਸ vpiSysTaskCall; }
      ਆਭਾਸੀ ਪੂਰਨ vpi_get(ਪੂਰਨ code)           override { ਵਾਪਸ systask_get(code, ਇਹ); }
      ਆਭਾਸੀ ਅੱਖਰ*vpi_get_str(ਪੂਰਨ code)      override { ਵਾਪਸ systask_get_str(code, ਇਹ); }
      ਆਭਾਸੀ vpiHandle vpi_handle(ਪੂਰਨ code)  override { ਵਾਪਸ systask_handle(code, ਇਹ); }
      ਆਭਾਸੀ vpiHandle vpi_iterate(ਪੂਰਨ code) override { ਵਾਪਸ systask_iter(code, ਇਹ); }
};

ਬਣਤਰ sysfunc_def : ਜਨਤਕ systask_def {
      ਆਭਾਸੀ ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiSysFuncCall; }
      ਆਭਾਸੀ ਪੂਰਨ vpi_get(ਪੂਰਨ code) override { ਵਾਪਸ sysfunc_get(code, ਇਹ); }
};

ਬਣਤਰ sysfunc_real : ਜਨਤਕ sysfunc_def {
      vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags) override;
      ਸਤਰਅੰਦਰ ਦੁੱਗਣਾ return_ਮੁੱਲ() ਅਚਲ { ਵਾਪਸ return_ਮੁੱਲ_2; }

    ਨਿਜੀ:
      ਦੁੱਗਣਾ return_ਮੁੱਲ_2;
};

vpiHandle sysfunc_real::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp, ਪੂਰਨ)
{
      put_ਮੁੱਲ = ਸੱਚ;

      return_ਮੁੱਲ_2 = 0.0;
      ਚੋਣ (vp->format) {
	  ਹਾਲਤ vpiRealVal:
	    return_ਮੁੱਲ_2 = vp->value.real;
	    ਰੋਕੋ;
	  ਮੂਲ:
	    fprintf(stderr, "Unsupported format %d.\n", (ਪੂਰਨ)vp->format);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

      ਵਾਪਸ 0;
}

ਬਣਤਰ sysfunc_str : ਜਨਤਕ sysfunc_def {
      ਪੂਰਨ vpi_get(ਪੂਰਨ code) override;
      vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags) override;
      ਸਤਰਅੰਦਰ ਅਚਲ std::string& return_ਮੁੱਲ() ਅਚਲ { ਵਾਪਸ return_ਮੁੱਲ_2; };

    ਨਿਜੀ:
      std::string return_ਮੁੱਲ_2;
};

ਪੂਰਨ sysfunc_str::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ return_ਮੁੱਲ_2.size();

	  ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ lineno;

	  ਹਾਲਤ vpiUserDefn:
	    ਵਾਪਸ defn->is_user_defn;

	  ਮੂਲ:
	    ਵਾਪਸ vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp, ਪੂਰਨ)
{
      put_ਮੁੱਲ = ਸੱਚ;

      ਚੋਣ (vp->format) {
	  ਹਾਲਤ vpiStringVal:
	    return_ਮੁੱਲ_2 = std::string(vp->value.str);
	    ਰੋਕੋ;
	  ਮੂਲ:
	    fprintf(stderr, "Unsupported format %d.\n", (ਪੂਰਨ)vp->format);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

      ਵਾਪਸ 0;
}

ਵਰਗ sysfunc_vec4 : ਜਨਤਕ sysfunc_def {
    ਜਨਤਕ:
      ਸਪਸ਼ਟ ਸਤਰਅੰਦਰ sysfunc_vec4(ਬਿਨਾਨਿਸ਼ਾਨੀ wid): return_ਮੁੱਲ_2(wid, BIT4_X) { }
      ਪੂਰਨ vpi_get(ਪੂਰਨ code) override;
      vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags) override;
      ਸਤਰਅੰਦਰ ਅਚਲ vvp_vector4_t& return_ਮੁੱਲ() ਅਚਲ { ਵਾਪਸ return_ਮੁੱਲ_2; }

    ਨਿਜੀ:
      vpiHandle put_ਮੁੱਲ_scalar(p_vpi_ਮੁੱਲ vp);
      vpiHandle put_ਮੁੱਲ_int(p_vpi_ਮੁੱਲ vp);
      vpiHandle put_ਮੁੱਲ_string(p_vpi_ਮੁੱਲ vp);
      vpiHandle put_ਮੁੱਲ_ਸਦਿਸ਼(p_vpi_ਮੁੱਲ vp);
      vpiHandle put_ਮੁੱਲ_ਸਮਾਂ(p_vpi_ਮੁੱਲ vp);

      vvp_vector4_t return_ਮੁੱਲ_2;
};

ਪੂਰਨ sysfunc_vec4::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ return_ਮੁੱਲ_2.size();

	  ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ lineno;

	  ਹਾਲਤ vpiUserDefn:
	    ਵਾਪਸ defn->is_user_defn;

	  ਮੂਲ:
	    ਵਾਪਸ vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_ਮੁੱਲ_scalar(ਅਚਲ p_vpi_ਮੁੱਲ vp)
{
      ਚੋਣ (vp->value.scalar) {
	  ਹਾਲਤ vpi0:
	    return_ਮੁੱਲ_2.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_0);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpi1:
	    return_ਮੁੱਲ_2.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_1);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiX:
	    return_ਮੁੱਲ_2.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_X);
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiZ:
	    return_ਮੁੱਲ_2.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_Z);
	    ਰੋਕੋ;
	  ਮੂਲ:
	    fprintf(stderr, "Unsupported value %d.\n", (ਪੂਰਨ)vp->value.scalar);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }
      ਵਾਪਸ 0;
}

vpiHandle sysfunc_vec4::put_ਮੁੱਲ_int(p_vpi_ਮੁੱਲ vp)
{
      ਲੰਮਾ tmp = vp->value.integer;
      ਬਿਨਾਨਿਸ਼ਾਨੀ width = return_ਮੁੱਲ_2.size();
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < width ;  idx += 1) {
	    return_ਮੁੱਲ_2.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      ਵਾਪਸ 0;
}

vpiHandle sysfunc_vec4::put_ਮੁੱਲ_string(p_vpi_ਮੁੱਲ vp)
{
      size_t slen = ਲੜੀ_ਲੰਬਾਈ(vp->value.str);
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid = return_ਮੁੱਲ_2.size();

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < wid ; idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    ਜੇ (cidx >= slen) {
		  return_ਮੁੱਲ_2.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_0);
		  ਜਾਰੀ;
	    }

	      // Put the end of the string in the LSB of the vector
	    ਅੱਖਰ use_char = vp->value.str[slen - 1 - cidx];

	    ਜੇ ((use_char >> (idx % 8)) & 1)
		  return_ਮੁੱਲ_2.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_1);
	    ਨਹੀਂਤਾਂ
		  return_ਮੁੱਲ_2.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_0);
      }
      ਵਾਪਸ 0;
}

vpiHandle sysfunc_vec4::put_ਮੁੱਲ_ਸਦਿਸ਼(p_vpi_ਮੁੱਲ vp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ width = return_ਮੁੱਲ_2.size();
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < width ; idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_2 = idx / 32;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ bidx = idx % 32;

	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ aval = vp->value.vector[ਸ਼ਬਦ_2].aval >> bidx;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ bval = vp->value.vector[ਸ਼ਬਦ_2].bval >> bidx;

	    ਪੂਰਨ ਦੋਆਧਾਰੀ_ਅੰਕ = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    ਚੋਣ (ਦੋਆਧਾਰੀ_ਅੰਕ) {
		ਹਾਲਤ 0:
		  bit4 = BIT4_0;
		  ਰੋਕੋ;
		ਹਾਲਤ 1:
		  bit4 = BIT4_1;
		  ਰੋਕੋ;
		ਹਾਲਤ 2:
		  bit4 = BIT4_Z;
		  ਰੋਕੋ;
		ਹਾਲਤ 3:
		  bit4 = BIT4_X;
		  ਰੋਕੋ;
		ਮੂਲ:
		  ਪ੍ਰਤਿਜ੍ਞਾ(0);
		  bit4 = BIT4_X;
		  ਰੋਕੋ;
	    }
	    return_ਮੁੱਲ_2.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, bit4);
      }
      ਵਾਪਸ 0;
}

vpiHandle sysfunc_vec4::put_ਮੁੱਲ_ਸਮਾਂ(p_vpi_ਮੁੱਲ vp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ width = return_ਮੁੱਲ_2.size();
      ਲੰਮਾ tmp = 0;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < width ; idx += 1) {
	    ਜੇ (idx == 0)
		  tmp = vp->value.time->ਨੀਵਾਂ;
	    ਨਹੀਂਤਾਂ ਜੇ (idx == 32)
		  tmp = vp->value.time->ਉੱਚਾ;
	    ਨਹੀਂਤਾਂ ਜੇ (idx == 64)
		  tmp = 0;

	    return_ਮੁੱਲ_2.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      ਵਾਪਸ 0;
}

vpiHandle sysfunc_vec4::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp, ਪੂਰਨ)
{
      put_ਮੁੱਲ = ਸੱਚ;

      ਚੋਣ (vp->format) {
	  ਹਾਲਤ vpiScalarVal:
	    ਵਾਪਸ put_ਮੁੱਲ_scalar(vp);
	  ਹਾਲਤ vpiIntVal:
	    ਵਾਪਸ put_ਮੁੱਲ_int(vp);
	  ਹਾਲਤ vpiStringVal:
	    ਵਾਪਸ put_ਮੁੱਲ_string(vp);
	  ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:
	    ਵਾਪਸ put_ਮੁੱਲ_ਸਦਿਸ਼(vp);
	  ਹਾਲਤ vpi_ਸਮਾਂ_Val:
	    ਵਾਪਸ put_ਮੁੱਲ_ਸਮਾਂ(vp);
	  ਮੂਲ:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (ਪੂਰਨ)vp->format);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

      ਵਾਪਸ 0;
}

ਬਣਤਰ sysfunc_4net : ਜਨਤਕ sysfunc_def {
      ਸਪਸ਼ਟ ਸਤਰਅੰਦਰ sysfunc_4net(ਬਿਨਾਨਿਸ਼ਾਨੀ wid) : vwid_(wid) { }
      ਪੂਰਨ vpi_get(ਪੂਰਨ code) override;
      vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags) override;

    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ vwid_;
};

// support getting vpiSize for a system function call
ਪੂਰਨ sysfunc_4net::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ vwid_;

	  ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ lineno;

	  ਹਾਲਤ vpiUserDefn:
	    ਵਾਪਸ defn->is_user_defn;

	  ਮੂਲ:
	    ਵਾਪਸ vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp, ਪੂਰਨ)
{
      put_ਮੁੱਲ = ਸੱਚ;

      vvp_vector4_t val (vwid_);

      ਚੋਣ (vp->format) {

          ਹਾਲਤ vpiScalarVal: {
	        ਚੋਣ(vp->value.scalar) {
		      ਹਾਲਤ vpi0:
			val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_0);
                        ਰੋਕੋ;
		      ਹਾਲਤ vpi1:
                        val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_1);
                        ਰੋਕੋ;
		      ਹਾਲਤ vpiX:
                        val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_X);
                        ਰੋਕੋ;
		      ਹਾਲਤ vpiZ:
                        val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_Z);
                        ਰੋਕੋ;
		      ਮੂਲ:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (ਪੂਰਨ)vp->value.scalar);
                        ਪ੍ਰਤਿਜ੍ਞਾ(0);
                }
		ਰੋਕੋ;
          }

	  ਹਾਲਤ vpiIntVal: {
		ਲੰਮਾ tmp = vp->value.integer;
		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		ਰੋਕੋ;
	  }

          ਹਾਲਤ vpi_ਸਮਾਂ_Val: {
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp = vp->value.time->ਨੀਵਾਂ;
                ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      ਜੇ (idx == 31)
                            tmp = vp->value.time->ਉੱਚਾ;
                      ਨਹੀਂਤਾਂ
                            tmp >>= 1;
                }
                ਰੋਕੋ;
          }

	  ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_2 = wdx / 32;
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ aval = vp->value.vector[ਸ਼ਬਦ_2].aval;
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ bval = vp->value.vector[ਸ਼ਬਦ_2].bval;

		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			ਪੂਰਨ ਦੋਆਧਾਰੀ_ਅੰਕ = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			ਚੋਣ (ਦੋਆਧਾਰੀ_ਅੰਕ) {
			    ਹਾਲਤ 0:
			      bit4 = BIT4_0;
			      ਰੋਕੋ;
			    ਹਾਲਤ 1:
			      bit4 = BIT4_1;
			      ਰੋਕੋ;
			    ਹਾਲਤ 2:
			      bit4 = BIT4_Z;
			      ਰੋਕੋ;
			    ਹਾਲਤ 3:
			      bit4 = BIT4_X;
			      ਰੋਕੋ;
			    ਮੂਲ:
			      bit4 = BIT4_X;
			      fprintf(stderr, "Unsupported bit value %d.\n",
			              ਦੋਆਧਾਰੀ_ਅੰਕ);
			      ਪ੍ਰਤਿਜ੍ਞਾ(0);
			}
			val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(wdx+idx, bit4);

			aval >>= 1;
			bval >>= 1;
		  }
	    }
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (ਪੂਰਨ)vp->format, vwid_);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      ਵਾਪਸ 0;
}

ਬਣਤਰ sysfunc_rnet : ਜਨਤਕ sysfunc_def {
      vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp, ਪੂਰਨ)
{
      put_ਮੁੱਲ = ਸੱਚ;

      ਦੁੱਗਣਾ value;
      ਚੋਣ (vp->format) {

	  ਹਾਲਤ vpiRealVal:
	    value = vp->value.real;
	    ਰੋਕੋ;

	  ਮੂਲ:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (ਪੂਰਨ)vp->format);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      ਵਾਪਸ 0;
}

ਬਣਤਰ sysfunc_no : ਜਨਤਕ sysfunc_def {
      vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags) override;
};

vpiHandle sysfunc_no::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ, ਪੂਰਨ)
{
      ਵਾਪਸ 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
ਸਥਿਰ ਬਣਤਰ __vpiUserSystf**def_ਸਾਰਣੀ = 0;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ def_ਗਿਣਤੀ = 0;

ਸਥਿਰ ਬਣਤਰ __vpiUserSystf* allocate_def(ਖਾਲੀ)
{
      ਜੇ (def_ਸਾਰਣੀ == 0) {
	    def_ਸਾਰਣੀ = static_cast<ਬਣਤਰ __vpiUserSystf**>
	                (ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਆਕਾਰ(ਬਣਤਰ __vpiUserSystf*)));

	    def_ਸਾਰਣੀ[0] = ਨਵਾਂ __vpiUserSystf;

	    def_ਗਿਣਤੀ = 1;
	    ਵਾਪਸ def_ਸਾਰਣੀ[0];
      }

      def_ਸਾਰਣੀ = static_cast<ਬਣਤਰ __vpiUserSystf**>
                  (realloc(def_ਸਾਰਣੀ,
                   (def_ਗਿਣਤੀ+1)*ਆਕਾਰ(ਬਣਤਰ __vpiUserSystf*)));

      def_ਸਾਰਣੀ[def_ਗਿਣਤੀ] = ਨਵਾਂ __vpiUserSystf;

      ਵਾਪਸ def_ਸਾਰਣੀ[def_ਗਿਣਤੀ++];
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ def_ਸਾਰਣੀ_delete(ਖਾਲੀ)
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < def_ਗਿਣਤੀ; idx += 1) {
	    ਮੁਕਤ(const_cast<ਅੱਖਰ *>(def_ਸਾਰਣੀ[idx]->info.tfname));
	    ਮਿਟਾਓ def_ਸਾਰਣੀ[idx];
      }
      ਮੁਕਤ(def_ਸਾਰਣੀ);
      def_ਸਾਰਣੀ = 0;
      def_ਗਿਣਤੀ = 0;
}
#ਜੇ_ਅੰਤ

ਵਰਗ __vpiSystfIterator : ਜਨਤਕ __vpiHandle {
    ਜਨਤਕ:
      ਸਪਸ਼ਟ __vpiSystfIterator(ਬਿਨਾਨਿਸ਼ਾਨੀ idx);
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      vpiHandle vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ idx) override;
      free_object_fun_t free_object_fun(ਖਾਲੀ) override;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਗਲਾ_2;
};

ਸਥਿਰ vpiHandle systf_iterator_ਜਾਂਚ(vpiHandle ref, ਪੂਰਨ)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      ਜੇ (obj->ਅਗਲਾ_2 >= def_ਗਿਣਤੀ) {
	    vpi_free_object(ref);
	    ਵਾਪਸ 0;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ use_ਸੂਚਕਾਂਕ = obj->ਅਗਲਾ_2;
      ਜਦੋਂ (!def_ਸਾਰਣੀ[use_ਸੂਚਕਾਂਕ]->is_user_defn) {
	    obj->ਅਗਲਾ_2 += 1;
	    use_ਸੂਚਕਾਂਕ = obj->ਅਗਲਾ_2;
	    ਜੇ (obj->ਅਗਲਾ_2 >= def_ਗਿਣਤੀ) {
		  vpi_free_object(ref);
		  ਵਾਪਸ 0;
	    }
      }
      obj->ਅਗਲਾ_2 += 1;
      ਵਾਪਸ def_ਸਾਰਣੀ[use_ਸੂਚਕਾਂਕ];
}

ਸਥਿਰ ਪੂਰਨ systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      ਮਿਟਾਓ obj;
      ਵਾਪਸ 1;
}

ਸਤਰਅੰਦਰ __vpiSystfIterator::__vpiSystfIterator(ਬਿਨਾਨਿਸ਼ਾਨੀ idx)
{ ਅਗਲਾ_2 = idx; }

ਪੂਰਨ __vpiSystfIterator::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ idx)
{ ਵਾਪਸ systf_iterator_ਜਾਂਚ(ਇਹ, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(ਖਾਲੀ)
{ ਵਾਪਸ &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(ਖਾਲੀ)
{
	/* Check to see if there are any user defined functions. */
      ਤਾਰਕਿਕ have_user_defn = ਝੂਠ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx;
      ਲਈ (idx = 0; idx < def_ਗਿਣਤੀ; idx += 1) {
	    ਜੇ (def_ਸਾਰਣੀ[idx]->is_user_defn) {
		  have_user_defn = ਸੱਚ;
		  ਰੋਕੋ;
	    }
      }
      ਜੇ (!have_user_defn) ਵਾਪਸ 0;

      __vpiSystfIterator*res = ਨਵਾਂ __vpiSystfIterator(idx);
      res->ਅਗਲਾ_2 = idx;
      ਵਾਪਸ res;
}

ਬਣਤਰ __vpiUserSystf* vpip_ਲੱਭੋ_systf(ਅਚਲ ਅੱਖਰ*name)
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < def_ਗਿਣਤੀ ;  idx += 1)
	    ਜੇ (strcmp(def_ਸਾਰਣੀ[idx]->info.tfname, name) == 0)
		  ਵਾਪਸ def_ਸਾਰਣੀ[idx];

      ਵਾਪਸ 0;
}

ਖਾਲੀ vpip_make_systf_system_defined(vpiHandle ref)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ref);
      ਬਣਤਰ __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);
      obj->is_user_defn = ਝੂਠ;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
ਗਿਣਤੀ vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ vpi_call_error {
      vpi_call_error_type type;
      ਅੱਖਰ *name;
      ਲੰਮਾ file_idx;
      ਲੰਮਾ lineno;
} vpi_call_error_s, *vpi_call_error_p;

ਸਥਿਰ vpi_call_error_p vpi_call_error_lst = ਨਿਰਾ;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ vpi_call_error_ਅੰਕ = 0;

ਸਥਿਰ ਖਾਲੀ ਜੋੜੋ_vpi_call_error(vpi_call_error_type type, ਅਚਲ ਅੱਖਰ *name,
                               ਲੰਮਾ file_idx, ਲੰਮਾ lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (realloc(vpi_call_error_lst,
                                    (vpi_call_error_ਅੰਕ + 1) *
                                    ਆਕਾਰ(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_ਅੰਕ].type = type;
      vpi_call_error_lst[vpi_call_error_ਅੰਕ].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_ਅੰਕ].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_ਅੰਕ].lineno = lineno;
      vpi_call_error_ਅੰਕ += 1;
}

ਖਾਲੀ print_vpi_call_errors()
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < vpi_call_error_ਅੰਕ; idx += 1) {
	    ਚੋਣ (vpi_call_error_lst[idx].type) {
		ਹਾਲਤ VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_ਨਾਂ[vpi_call_error_lst[idx].file_idx],
		                  (ਪੂਰਨ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ਰੋਕੋ;
		ਹਾਲਤ VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_ਨਾਂ[vpi_call_error_lst[idx].file_idx],
		                  (ਪੂਰਨ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ਰੋਕੋ;
		ਹਾਲਤ VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_ਨਾਂ[vpi_call_error_lst[idx].file_idx],
		                  (ਪੂਰਨ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ਰੋਕੋ;
		ਹਾਲਤ VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_ਨਾਂ[vpi_call_error_lst[idx].file_idx],
		                  (ਪੂਰਨ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_ਨਾਂ[vpi_call_error_lst[idx].file_idx],
                                 (ਪੂਰਨ)vpi_call_error_lst[idx].lineno);
		  ਰੋਕੋ;
	    }
	    ਮੁਕਤ(vpi_call_error_lst[idx].name);
      }
      ਮੁਕਤ(vpi_call_error_lst);
      fflush(stderr);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਸਥਿਰ ਖਾਲੀ cleanup_vpi_call_ਦਲੀਲਾਂ(ਬਿਨਾਨਿਸ਼ਾਨੀ argc, vpiHandle*argv)
{
      ਜੇ (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    ਬਣਤਰ __vpiSysTaskCall*obj = ਨਵਾਂ systask_def;
	    obj->nargs = argc;
	    obj->ਦਲੀਲਾਂ  = argv;
	    vpi_call_delete(obj);
      }
}
#ਜੇ_ਅੰਤ

/*
 * A vpi_call is actually built up into a vpiSysTaskCall VPI object
 * that refers back to the vpiUserSystf VPI object that is the
 * definition. So this function is called by the compiler when a
 * %vpi_call statement is encountered. Create here a vpiHandle that
 * describes the call, and return it. The %vpi_call instruction will
 * store this handle for when it is executed.
 */
vpiHandle vpip_build_vpi_call(ਅਚਲ ਅੱਖਰ*name, ਪੂਰਨ val_ਕੋਡ, ਬਿਨਾਨਿਸ਼ਾਨੀ return_ਚੌੜਾਈ,
			      vvp_net_t*fnet,
			      ਤਾਰਕਿਕ func_as_task_err, ਤਾਰਕਿਕ func_as_task_warn,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ argc, vpiHandle*argv,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ vec4_ਢੇਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ real_ਢੇਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ string_ਢੇਰ,
			      ਲੰਮਾ file_idx, ਲੰਮਾ lineno)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(!(func_as_task_err && func_as_task_warn));

      ਬਣਤਰ __vpiUserSystf*defn = vpip_ਲੱਭੋ_systf(name);
      ਜੇ (defn == 0) {
	    ਜੋੜੋ_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
	    cleanup_vpi_call_ਦਲੀਲਾਂ(argc, argv);
#ਜੇ_ਅੰਤ
	    ਵਾਪਸ 0;
      }

      ਚੋਣ (defn->info.type) {
	  ਹਾਲਤ vpiSysTask:
	    ਜੇ (val_ਕੋਡ != 0 || fnet != 0) {
		  ਜੋੜੋ_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
		  cleanup_vpi_call_ਦਲੀਲਾਂ(argc, argv);
#ਜੇ_ਅੰਤ
		  ਵਾਪਸ 0;
	    }
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiSysFunc:
	    ਜੇ (val_ਕੋਡ == 0 && fnet == 0) {
		  ਜੇ (func_as_task_err) {
			ਜੋੜੋ_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
			cleanup_vpi_call_ਦਲੀਲਾਂ(argc, argv);
#ਜੇ_ਅੰਤ
			ਵਾਪਸ 0;
		  } ਨਹੀਂਤਾਂ ਜੇ (func_as_task_warn) {
			ਜੋੜੋ_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (ਪੂਰਨ)defn->info.type);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

      ਬਣਤਰ __vpiSysTaskCall*obj = 0;

      ਚੋਣ (defn->info.type) {
	  ਹਾਲਤ vpiSysTask:
	    obj = ਨਵਾਂ systask_def;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiSysFunc:
	    ਜੇ (fnet && val_ਕੋਡ == -vpiRealVal) {
		  obj = ਨਵਾਂ sysfunc_rnet;

	    } ਨਹੀਂਤਾਂ ਜੇ (fnet && val_ਕੋਡ == -vpi_ਸਦਿਸ਼_Val) {
		  obj = ਨਵਾਂ sysfunc_4net(return_ਚੌੜਾਈ);

	    } ਨਹੀਂਤਾਂ ਜੇ (val_ਕੋਡ == -vpiRealVal) {
		  obj = ਨਵਾਂ sysfunc_real;

	    } ਨਹੀਂਤਾਂ ਜੇ (val_ਕੋਡ == -vpi_ਸਦਿਸ਼_Val) {
		  obj = ਨਵਾਂ sysfunc_vec4(return_ਚੌੜਾਈ);

	    } ਨਹੀਂਤਾਂ ਜੇ (val_ਕੋਡ == -vpiStringVal) {
		  obj = ਨਵਾਂ sysfunc_str;

           } ਨਹੀਂਤਾਂ ਜੇ (val_ਕੋਡ == 0 && fnet == 0) {
		  obj = ਨਵਾਂ sysfunc_no;

	    } ਨਹੀਂਤਾਂ {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_ਕੋਡ);
		  ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    }
	    ਰੋਕੋ;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->ਦਲੀਲਾਂ  = argv;
      obj->vec4_ਢੇਰ = vec4_ਢੇਰ;
      obj->real_ਢੇਰ = real_ਢੇਰ;
      obj->string_ਢੇਰ = string_ਢੇਰ;
      obj->fnet  = fnet;
      obj->file_idx  = (ਬਿਨਾਨਿਸ਼ਾਨੀ) file_idx;
      obj->lineno   = (ਬਿਨਾਨਿਸ਼ਾਨੀ) lineno;
      obj->userdata  = 0;
      obj->put_ਮੁੱਲ = ਝੂਠ;

      compile_compiletf(obj);

      ਵਾਪਸ obj;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ vpi_call_delete(vpiHandle ਵਸਤੂ)
{
      ਬਣਤਰ __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(ਵਸਤੂ);
	/* The object can be NULL if there was an error. */
      ਜੇ (!obj) ਵਾਪਸ;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ arg = 0; arg < obj->nargs; arg += 1) {
	    ਚੋਣ (obj->ਦਲੀਲਾਂ[arg]->get_type_ਕੋਡ()) {
		ਹਾਲਤ vpiConstant:
		  ਚੋਣ (vpi_get(_vpiFromThr, obj->ਦਲੀਲਾਂ[arg])) {
		      ਹਾਲਤ _vpiNoThr:
			constant_delete(obj->ਦਲੀਲਾਂ[arg]);
			ਰੋਕੋ;
		      ਹਾਲਤ _vpiString:
			thread_string_delete(obj->ਦਲੀਲਾਂ[arg]);
			ਰੋਕੋ;
		      ਹਾਲਤ _vpiVThr:
			thread_vthr_delete(obj->ਦਲੀਲਾਂ[arg]);
			ਰੋਕੋ;
		      ਹਾਲਤ vpi_ਸ਼ਬਦ:
			thread_ਸ਼ਬਦ_delete(obj->ਦਲੀਲਾਂ[arg]);
			ਰੋਕੋ;
		      ਮੂਲ:
			ਪ੍ਰਤਿਜ੍ਞਾ(0);
		  }
		  ਰੋਕੋ;
		ਹਾਲਤ vpi_ਸਮ੍ਰਿਤੀ_ਸ਼ਬਦ:
		  ਚੋਣ (vpi_get(_vpiFromThr, obj->ਦਲੀਲਾਂ[arg])) {
		      ਹਾਲਤ _vpi_at_A:
			A_delete(obj->ਦਲੀਲਾਂ[arg]);
			ਰੋਕੋ;
		  }
		  ਰੋਕੋ;
		ਹਾਲਤ vpiPartSelect:
		  ਚੋਣ (vpi_get(_vpiFromThr, obj->ਦਲੀਲਾਂ[arg])) {
		      ਹਾਲਤ _vpi_at_PV:
			PV_delete(obj->ਦਲੀਲਾਂ[arg]);
			ਰੋਕੋ;
		      ਹਾਲਤ _vpi_at_APV:
			APV_delete(obj->ਦਲੀਲਾਂ[arg]);
			ਰੋਕੋ;
		  }
		  ਰੋਕੋ;
	    }
      }
      ਮੁਕਤ(obj->ਦਲੀਲਾਂ);
      ਮਿਟਾਓ obj;
}
#ਜੇ_ਅੰਤ

/*
 * This function is used by the %vpi_call instruction to actually
 * place the call to the system task/function. For now, only support
 * calls to system tasks.
 */

vthread_t vpip_current_vthread;

ਖਾਲੀ vpip_execute_vpi_call(vthread_t thr, vpiHandle ref)
{
      vpip_current_vthread = thr;

      vpip_cur_task = dynamic_cast<__vpiSysTaskCall*>(ref);

      ਜੇ (vpip_cur_task->defn->info.calltf) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(vpi_ਬਹੁਲਕ_flag == VPI_ਬਹੁਲਕ_NONE);
	    vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_CALLTF;
	    vpip_cur_task->put_ਮੁੱਲ = ਝੂਠ;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_ਅੰਕੜੇ);
	    vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    ਜੇ (ref->get_type_ਕੋਡ() == vpiSysFuncCall &&
	        !vpip_cur_task->put_ਮੁੱਲ) {
		  s_vpi_ਮੁੱਲ val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_ਮੁੱਲ(ref, &val, 0, vpiNoDelay);
	    }
      }
      ਜੇ (vpip_cur_task->vec4_ਢੇਰ > 0)
	    vthread_ਕੱਢੋ_vec4(thr, vpip_cur_task->vec4_ਢੇਰ);
      ਜੇ (vpip_cur_task->real_ਢੇਰ > 0)
	    vthread_ਕੱਢੋ_real(thr, vpip_cur_task->real_ਢੇਰ);
      ਜੇ (vpip_cur_task->string_ਢੇਰ > 0)
	    vthread_ਕੱਢੋ_str(thr, vpip_cur_task->string_ਢੇਰ);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      ਜੇ (ਅਚਲ sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_ਧੱਕੋ(thr, func_real->return_ਮੁੱਲ());
      }
      ਨਹੀਂਤਾਂ ਜੇ (ਅਚਲ sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_ਧੱਕੋ(thr, func_string->return_ਮੁੱਲ());
      }
      ਨਹੀਂਤਾਂ ਜੇ (ਅਚਲ sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
	    vthread_ਧੱਕੋ(thr, func_vec4->return_ਮੁੱਲ());
      }
      vpip_cur_task = 0;
}

/*
 * This is the entry function that a VPI module uses to hook a new
 * task/function into the simulator. The function creates a new
 * __vpi_userSystf to represent the definition for the calls that come
 * to pass later.
 */
vpiHandle vpi_ਰਜਿਸਟਰ_systf(ਅਚਲ ਬਣਤਰ t_vpi_systf_ਅੰਕੜੇ*ss)
{
      ਬਣਤਰ __vpiUserSystf*cur = allocate_def();
      ਪ੍ਰਤਿਜ੍ਞਾ(ss);
      ਚੋਣ (ss->type) {
	  ਹਾਲਤ vpiSysTask:
	  ਹਾਲਤ vpiSysFunc:
	    ਰੋਕੋ;
	  ਮੂਲ:
	    fprintf(stderr, "Unsupported type %d.\n", (ਪੂਰਨ)ss->type);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = ਸੱਚ;

      ਵਾਪਸ cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, ਖਾਲੀ*ਅੰਕੜੇ)
{
      ਬਣਤਰ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      ਜੇ (rfp == 0)
	    ਵਾਪਸ 0;

      rfp->userdata = ਅੰਕੜੇ;
      ਵਾਪਸ 1;
}

ਖਾਲੀ* vpi_get_userdata(vpiHandle ref)
{
      ਬਣਤਰ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਵਾਪਸ rfp->userdata;
}
