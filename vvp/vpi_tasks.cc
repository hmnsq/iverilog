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
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

using ನಾಮಾಕಾಶ std;

ಅನ್ತಃಸ್ಥ __vpiUserSystf::__vpiUserSystf()
{ }

ಪೂರ್ಣಾಙ್ಕ __vpiUserSystf::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiUserSystf; }


ಸ್ಥಿರ vpiHandle systask_handle(ಪೂರ್ಣಾಙ್ಕ type, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ವಿಕಲ್ಪ (type) {
	  ಪಕ್ಷ vpiScope:
	    ಪ್ರತ್ಯಾಗಮ rfp->scope;

	  ಪಕ್ಷ vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    ಪ್ರತಿಜ್ಞಾ(rfp->defn->is_user_defn);
	    ಪ್ರತ್ಯಾಗಮ rfp->defn;

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      };
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ systask_get(ಪೂರ್ಣಾಙ್ಕ type, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ವಿಕಲ್ಪ (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  ಪಕ್ಷ vpiTimeUnit:
	    ಪ್ರತ್ಯಾಗಮ rfp->scope->time_units;
	  ಪಕ್ಷ vpiTimePrecision:
	    ಪ್ರತ್ಯಾಗಮ rfp->scope->time_precision;

	  ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ rfp->lineno;

	  ಪಕ್ಷ vpiUserDefn:
	    ಪ್ರತ್ಯಾಗಮ rfp->defn->is_user_defn;

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ vpiUndefined;
      }
}

// support getting vpiSize for a system function call
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sysfunc_get(ಪೂರ್ಣಾಙ್ಕ type, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ವಿಕಲ್ಪ (type) {
	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತಿಜ್ಞಾ(0); // This should be handled by derived classes
	    ಪ್ರತ್ಯಾಗಮ 0;

	  ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ rfp->lineno;

	  ಪಕ್ಷ vpiUserDefn:
	    ಪ್ರತ್ಯಾಗಮ rfp->defn->is_user_defn;

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

ಸ್ಥಿರ ವರ್ಣ *systask_get_str(ಪೂರ್ಣಾಙ್ಕ type, vpiHandle ref)
{
      ಧ್ರುವ ಸಂರಚನಾ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ವಿಕಲ್ಪ (type) {
          ಪಕ್ಷ vpiFile:
            ಪ್ರತಿಜ್ಞಾ(rfp->file_idx < file_names.size());
            ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[rfp->file_idx]);

          ಪಕ್ಷ vpiName:
            ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(rfp->defn->info.tfname);
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
ಸ್ಥಿರ vpiHandle systask_iter(ಪೂರ್ಣಾಙ್ಕ, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ಯದಿ (rfp->nargs == 0)
	    ಪ್ರತ್ಯಾಗಮ 0;

      ಪ್ರತ್ಯಾಗಮ vpip_make_iterator(rfp->nargs, rfp->args, ಅಸತ್ಯ);
}

ಸಂರಚನಾ systask_def : ಸಾರ್ವಜನಿಕ __vpiSysTaskCall {
      ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ   override { ಪ್ರತ್ಯಾಗಮ vpiSysTaskCall; }
      ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code)           override { ಪ್ರತ್ಯಾಗಮ systask_get(code, ಏತತ್); }
      ಆಭಾಸ ವರ್ಣ*vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)      override { ಪ್ರತ್ಯಾಗಮ systask_get_str(code, ಏತತ್); }
      ಆಭಾಸ vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)  override { ಪ್ರತ್ಯಾಗಮ systask_handle(code, ಏತತ್); }
      ಆಭಾಸ vpiHandle vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code) override { ಪ್ರತ್ಯಾಗಮ systask_iter(code, ಏತತ್); }
};

ಸಂರಚನಾ sysfunc_def : ಸಾರ್ವಜನಿಕ systask_def {
      ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiSysFuncCall; }
      ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override { ಪ್ರತ್ಯಾಗಮ sysfunc_get(code, ಏತತ್); }
};

ಸಂರಚನಾ sysfunc_real : ಸಾರ್ವಜನಿಕ sysfunc_def {
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags) override;
      ಅನ್ತಃಸ್ಥ ದ್ವಿಗುಣ return_ಮೌಲ್ಯ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ return_ಮೌಲ್ಯ_2; }

    ಗುಪ್ತ:
      ದ್ವಿಗುಣ return_ಮೌಲ್ಯ_2;
};

vpiHandle sysfunc_real::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ)
{
      put_ಮೌಲ್ಯ = ಸತ್ಯ;

      return_ಮೌಲ್ಯ_2 = 0.0;
      ವಿಕಲ್ಪ (vp->format) {
	  ಪಕ್ಷ vpiRealVal:
	    return_ಮೌಲ್ಯ_2 = vp->value.real;
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Unsupported format %d.\n", (ಪೂರ್ಣಾಙ್ಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸಂರಚನಾ sysfunc_str : ಸಾರ್ವಜನಿಕ sysfunc_def {
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags) override;
      ಅನ್ತಃಸ್ಥ ಧ್ರುವ std::string& return_ಮೌಲ್ಯ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ return_ಮೌಲ್ಯ_2; };

    ಗುಪ್ತ:
      std::string return_ಮೌಲ್ಯ_2;
};

ಪೂರ್ಣಾಙ್ಕ sysfunc_str::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ return_ಮೌಲ್ಯ_2.size();

	  ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ lineno;

	  ಪಕ್ಷ vpiUserDefn:
	    ಪ್ರತ್ಯಾಗಮ defn->is_user_defn;

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ)
{
      put_ಮೌಲ್ಯ = ಸತ್ಯ;

      ವಿಕಲ್ಪ (vp->format) {
	  ಪಕ್ಷ vpiStringVal:
	    return_ಮೌಲ್ಯ_2 = std::string(vp->value.str);
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Unsupported format %d.\n", (ಪೂರ್ಣಾಙ್ಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಗ sysfunc_vec4 : ಸಾರ್ವಜನಿಕ sysfunc_def {
    ಸಾರ್ವಜನಿಕ:
      explicit ಅನ್ತಃಸ್ಥ sysfunc_vec4(unsigned wid): return_ಮೌಲ್ಯ_2(wid, BIT4_X) { }
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags) override;
      ಅನ್ತಃಸ್ಥ ಧ್ರುವ vvp_vector4_t& return_ಮೌಲ್ಯ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ return_ಮೌಲ್ಯ_2; }

    ಗುಪ್ತ:
      vpiHandle put_ಮೌಲ್ಯ_scalar(p_vpi_ಮೌಲ್ಯ vp);
      vpiHandle put_ಮೌಲ್ಯ_int(p_vpi_ಮೌಲ್ಯ vp);
      vpiHandle put_ಮೌಲ್ಯ_string(p_vpi_ಮೌಲ್ಯ vp);
      vpiHandle put_ಮೌಲ್ಯ_vector(p_vpi_ಮೌಲ್ಯ vp);
      vpiHandle put_ಮೌಲ್ಯ_time(p_vpi_ಮೌಲ್ಯ vp);

      vvp_vector4_t return_ಮೌಲ್ಯ_2;
};

ಪೂರ್ಣಾಙ್ಕ sysfunc_vec4::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ return_ಮೌಲ್ಯ_2.size();

	  ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ lineno;

	  ಪಕ್ಷ vpiUserDefn:
	    ಪ್ರತ್ಯಾಗಮ defn->is_user_defn;

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_ಮೌಲ್ಯ_scalar(ಧ್ರುವ p_vpi_ಮೌಲ್ಯ vp)
{
      ವಿಕಲ್ಪ (vp->value.scalar) {
	  ಪಕ್ಷ vpi0:
	    return_ಮೌಲ್ಯ_2.set_bit(0, BIT4_0);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpi1:
	    return_ಮೌಲ್ಯ_2.set_bit(0, BIT4_1);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiX:
	    return_ಮೌಲ್ಯ_2.set_bit(0, BIT4_X);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiZ:
	    return_ಮೌಲ್ಯ_2.set_bit(0, BIT4_Z);
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Unsupported value %d.\n", (ಪೂರ್ಣಾಙ್ಕ)vp->value.scalar);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
      ಪ್ರತ್ಯಾಗಮ 0;
}

vpiHandle sysfunc_vec4::put_ಮೌಲ್ಯ_int(p_vpi_ಮೌಲ್ಯ vp)
{
      long tmp = vp->value.integer;
      unsigned width = return_ಮೌಲ್ಯ_2.size();
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < width ;  idx += 1) {
	    return_ಮೌಲ್ಯ_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

vpiHandle sysfunc_vec4::put_ಮೌಲ್ಯ_string(p_vpi_ಮೌಲ್ಯ vp)
{
      size_t slen = strlen(vp->value.str);
      unsigned wid = return_ಮೌಲ್ಯ_2.size();

      ಪ್ರತಿ (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    unsigned cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    ಯದಿ (cidx >= slen) {
		  return_ಮೌಲ್ಯ_2.set_bit(idx, BIT4_0);
		  ಅನುವರ್ತ;
	    }

	      // Put the end of the string in the LSB of the vector
	    ವರ್ಣ use_char = vp->value.str[slen - 1 - cidx];

	    ಯದಿ ((use_char >> (idx % 8)) & 1)
		  return_ಮೌಲ್ಯ_2.set_bit(idx, BIT4_1);
	    ಅನ್ಯಥಾ
		  return_ಮೌಲ್ಯ_2.set_bit(idx, BIT4_0);
      }
      ಪ್ರತ್ಯಾಗಮ 0;
}

vpiHandle sysfunc_vec4::put_ಮೌಲ್ಯ_vector(p_vpi_ಮೌಲ್ಯ vp)
{
      unsigned width = return_ಮೌಲ್ಯ_2.size();
      ಪ್ರತಿ (unsigned idx = 0 ; idx < width ; idx += 1) {
	    unsigned word = idx / 32;
	    unsigned bidx = idx % 32;

	    unsigned long aval = vp->value.vector[word].aval >> bidx;
	    unsigned long bval = vp->value.vector[word].bval >> bidx;

	    ಪೂರ್ಣಾಙ್ಕ bit = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    ವಿಕಲ್ಪ (bit) {
		ಪಕ್ಷ 0:
		  bit4 = BIT4_0;
		  ಭಙ್ಗ;
		ಪಕ್ಷ 1:
		  bit4 = BIT4_1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ 2:
		  bit4 = BIT4_Z;
		  ಭಙ್ಗ;
		ಪಕ್ಷ 3:
		  bit4 = BIT4_X;
		  ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		  ಪ್ರತಿಜ್ಞಾ(0);
		  bit4 = BIT4_X;
		  ಭಙ್ಗ;
	    }
	    return_ಮೌಲ್ಯ_2.set_bit(idx, bit4);
      }
      ಪ್ರತ್ಯಾಗಮ 0;
}

vpiHandle sysfunc_vec4::put_ಮೌಲ್ಯ_time(p_vpi_ಮೌಲ್ಯ vp)
{
      unsigned width = return_ಮೌಲ್ಯ_2.size();
      long tmp = 0;
      ಪ್ರತಿ (unsigned idx = 0 ; idx < width ; idx += 1) {
	    ಯದಿ (idx == 0)
		  tmp = vp->value.time->ಕೆಳಗಿನ;
	    ಅನ್ಯಥಾ ಯದಿ (idx == 32)
		  tmp = vp->value.time->ಮೇಲಿನ;
	    ಅನ್ಯಥಾ ಯದಿ (idx == 64)
		  tmp = 0;

	    return_ಮೌಲ್ಯ_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      ಪ್ರತ್ಯಾಗಮ 0;
}

vpiHandle sysfunc_vec4::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ)
{
      put_ಮೌಲ್ಯ = ಸತ್ಯ;

      ವಿಕಲ್ಪ (vp->format) {
	  ಪಕ್ಷ vpiScalarVal:
	    ಪ್ರತ್ಯಾಗಮ put_ಮೌಲ್ಯ_scalar(vp);
	  ಪಕ್ಷ vpiIntVal:
	    ಪ್ರತ್ಯಾಗಮ put_ಮೌಲ್ಯ_int(vp);
	  ಪಕ್ಷ vpiStringVal:
	    ಪ್ರತ್ಯಾಗಮ put_ಮೌಲ್ಯ_string(vp);
	  ಪಕ್ಷ vpiVectorVal:
	    ಪ್ರತ್ಯಾಗಮ put_ಮೌಲ್ಯ_vector(vp);
	  ಪಕ್ಷ vpiTimeVal:
	    ಪ್ರತ್ಯಾಗಮ put_ಮೌಲ್ಯ_time(vp);
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (ಪೂರ್ಣಾಙ್ಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸಂರಚನಾ sysfunc_4net : ಸಾರ್ವಜನಿಕ sysfunc_def {
      explicit ಅನ್ತಃಸ್ಥ sysfunc_4net(unsigned wid) : vwid_(wid) { }
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags) override;

    ಗುಪ್ತ:
      unsigned vwid_;
};

// support getting vpiSize for a system function call
ಪೂರ್ಣಾಙ್ಕ sysfunc_4net::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ vwid_;

	  ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ lineno;

	  ಪಕ್ಷ vpiUserDefn:
	    ಪ್ರತ್ಯಾಗಮ defn->is_user_defn;

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ)
{
      put_ಮೌಲ್ಯ = ಸತ್ಯ;

      vvp_vector4_t val (vwid_);

      ವಿಕಲ್ಪ (vp->format) {

          ಪಕ್ಷ vpiScalarVal: {
	        ವಿಕಲ್ಪ(vp->value.scalar) {
		      ಪಕ್ಷ vpi0:
			val.set_bit(0, BIT4_0);
                        ಭಙ್ಗ;
		      ಪಕ್ಷ vpi1:
                        val.set_bit(0, BIT4_1);
                        ಭಙ್ಗ;
		      ಪಕ್ಷ vpiX:
                        val.set_bit(0, BIT4_X);
                        ಭಙ್ಗ;
		      ಪಕ್ಷ vpiZ:
                        val.set_bit(0, BIT4_Z);
                        ಭಙ್ಗ;
		      ಸ್ವಭಾವ:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (ಪೂರ್ಣಾಙ್ಕ)vp->value.scalar);
                        ಪ್ರತಿಜ್ಞಾ(0);
                }
		ಭಙ್ಗ;
          }

	  ಪಕ್ಷ vpiIntVal: {
		long tmp = vp->value.integer;
		ಪ್ರತಿ (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		ಭಙ್ಗ;
	  }

          ಪಕ್ಷ vpiTimeVal: {
                unsigned long tmp = vp->value.time->ಕೆಳಗಿನ;
                ಪ್ರತಿ (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      ಯದಿ (idx == 31)
                            tmp = vp->value.time->ಮೇಲಿನ;
                      ಅನ್ಯಥಾ
                            tmp >>= 1;
                }
                ಭಙ್ಗ;
          }

	  ಪಕ್ಷ vpiVectorVal:

	    ಪ್ರತಿ (unsigned wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  unsigned word = wdx / 32;
		  unsigned long aval = vp->value.vector[word].aval;
		  unsigned long bval = vp->value.vector[word].bval;

		  ಪ್ರತಿ (unsigned idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			ಪೂರ್ಣಾಙ್ಕ bit = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			ವಿಕಲ್ಪ (bit) {
			    ಪಕ್ಷ 0:
			      bit4 = BIT4_0;
			      ಭಙ್ಗ;
			    ಪಕ್ಷ 1:
			      bit4 = BIT4_1;
			      ಭಙ್ಗ;
			    ಪಕ್ಷ 2:
			      bit4 = BIT4_Z;
			      ಭಙ್ಗ;
			    ಪಕ್ಷ 3:
			      bit4 = BIT4_X;
			      ಭಙ್ಗ;
			    ಸ್ವಭಾವ:
			      bit4 = BIT4_X;
			      fprintf(stderr, "Unsupported bit value %d.\n",
			              bit);
			      ಪ್ರತಿಜ್ಞಾ(0);
			}
			val.set_bit(wdx+idx, bit4);

			aval >>= 1;
			bval >>= 1;
		  }
	    }
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (ಪೂರ್ಣಾಙ್ಕ)vp->format, vwid_);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸಂರಚನಾ sysfunc_rnet : ಸಾರ್ವಜನಿಕ sysfunc_def {
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ)
{
      put_ಮೌಲ್ಯ = ಸತ್ಯ;

      ದ್ವಿಗುಣ value;
      ವಿಕಲ್ಪ (vp->format) {

	  ಪಕ್ಷ vpiRealVal:
	    value = vp->value.real;
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (ಪೂರ್ಣಾಙ್ಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸಂರಚನಾ sysfunc_no : ಸಾರ್ವಜನಿಕ sysfunc_def {
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags) override;
};

vpiHandle sysfunc_no::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ)
{
      ಪ್ರತ್ಯಾಗಮ 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
ಸ್ಥಿರ ಸಂರಚನಾ __vpiUserSystf**def_table = 0;
ಸ್ಥಿರ unsigned def_ಎಣಿಕೆ = 0;

ಸ್ಥಿರ ಸಂರಚನಾ __vpiUserSystf* allocate_def(ಶೂನ್ಯ)
{
      ಯದಿ (def_table == 0) {
	    def_table = static_cast<ಸಂರಚನಾ __vpiUserSystf**>
	                (malloc(ಪರಿಮಾಣ(ಸಂರಚನಾ __vpiUserSystf*)));

	    def_table[0] = ನೂತನ __vpiUserSystf;

	    def_ಎಣಿಕೆ = 1;
	    ಪ್ರತ್ಯಾಗಮ def_table[0];
      }

      def_table = static_cast<ಸಂರಚನಾ __vpiUserSystf**>
                  (realloc(def_table,
                   (def_ಎಣಿಕೆ+1)*ಪರಿಮಾಣ(ಸಂರಚನಾ __vpiUserSystf*)));

      def_table[def_ಎಣಿಕೆ] = ನೂತನ __vpiUserSystf;

      ಪ್ರತ್ಯಾಗಮ def_table[def_ಎಣಿಕೆ++];
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ def_table_delete(ಶೂನ್ಯ)
{
      ಪ್ರತಿ (unsigned idx = 0; idx < def_ಎಣಿಕೆ; idx += 1) {
	    ಮುಕ್ತ(const_cast<ವರ್ಣ *>(def_table[idx]->info.tfname));
	    delete def_table[idx];
      }
      ಮುಕ್ತ(def_table);
      def_table = 0;
      def_ಎಣಿಕೆ = 0;
}
#endif

ವರ್ಗ __vpiSystfIterator : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      explicit __vpiSystfIterator(unsigned idx);
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      vpiHandle vpi_index(ಪೂರ್ಣಾಙ್ಕ idx) override;
      free_object_fun_t free_object_fun(ಶೂನ್ಯ) override;

      unsigned next;
};

ಸ್ಥಿರ vpiHandle systf_iterator_scan(vpiHandle ref, ಪೂರ್ಣಾಙ್ಕ)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      ಯದಿ (obj->next >= def_ಎಣಿಕೆ) {
	    vpi_free_object(ref);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }

      unsigned use_index = obj->next;
      ಯಾವತ್ (!def_table[use_index]->is_user_defn) {
	    obj->next += 1;
	    use_index = obj->next;
	    ಯದಿ (obj->next >= def_ಎಣಿಕೆ) {
		  vpi_free_object(ref);
		  ಪ್ರತ್ಯಾಗಮ 0;
	    }
      }
      obj->next += 1;
      ಪ್ರತ್ಯಾಗಮ def_table[use_index];
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      delete obj;
      ಪ್ರತ್ಯಾಗಮ 1;
}

ಅನ್ತಃಸ್ಥ __vpiSystfIterator::__vpiSystfIterator(unsigned idx)
{ next = idx; }

ಪೂರ್ಣಾಙ್ಕ __vpiSystfIterator::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_index(ಪೂರ್ಣಾಙ್ಕ idx)
{ ಪ್ರತ್ಯಾಗಮ systf_iterator_scan(ಏತತ್, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(ಶೂನ್ಯ)
{ ಪ್ರತ್ಯಾಗಮ &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(ಶೂನ್ಯ)
{
	/* Check to see if there are any user defined functions. */
      ತಾರ್ಕಿಕ have_user_defn = ಅಸತ್ಯ;
      unsigned idx;
      ಪ್ರತಿ (idx = 0; idx < def_ಎಣಿಕೆ; idx += 1) {
	    ಯದಿ (def_table[idx]->is_user_defn) {
		  have_user_defn = ಸತ್ಯ;
		  ಭಙ್ಗ;
	    }
      }
      ಯದಿ (!have_user_defn) ಪ್ರತ್ಯಾಗಮ 0;

      __vpiSystfIterator*res = ನೂತನ __vpiSystfIterator(idx);
      res->next = idx;
      ಪ್ರತ್ಯಾಗಮ res;
}

ಸಂರಚನಾ __vpiUserSystf* vpip_find_systf(ಧ್ರುವ ವರ್ಣ*name)
{
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < def_ಎಣಿಕೆ ;  idx += 1)
	    ಯದಿ (strcmp(def_table[idx]->info.tfname, name) == 0)
		  ಪ್ರತ್ಯಾಗಮ def_table[idx];

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ vpip_make_systf_system_defined(vpiHandle ref)
{
      ಪ್ರತಿಜ್ಞಾ(ref);
      ಸಂರಚನಾ __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      ಪ್ರತಿಜ್ಞಾ(obj);
      obj->is_user_defn = ಅಸತ್ಯ;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
ಗಣನಾ vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ vpi_call_error {
      vpi_call_error_type type;
      ವರ್ಣ *name;
      long file_idx;
      long lineno;
} vpi_call_error_s, *vpi_call_error_p;

ಸ್ಥಿರ vpi_call_error_p vpi_call_error_lst = ರಿಕ್ತ;
ಸ್ಥಿರ unsigned vpi_call_error_ಅಂಕೆ = 0;

ಸ್ಥಿರ ಶೂನ್ಯ add_vpi_call_error(vpi_call_error_type type, ಧ್ರುವ ವರ್ಣ *name,
                               long file_idx, long lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (realloc(vpi_call_error_lst,
                                    (vpi_call_error_ಅಂಕೆ + 1) *
                                    ಪರಿಮಾಣ(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_ಅಂಕೆ].type = type;
      vpi_call_error_lst[vpi_call_error_ಅಂಕೆ].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_ಅಂಕೆ].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_ಅಂಕೆ].lineno = lineno;
      vpi_call_error_ಅಂಕೆ += 1;
}

ಶೂನ್ಯ print_vpi_call_errors()
{
      ಪ್ರತಿ (unsigned idx = 0; idx < vpi_call_error_ಅಂಕೆ; idx += 1) {
	    ವಿಕಲ್ಪ (vpi_call_error_lst[idx].type) {
		ಪಕ್ಷ VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (ಪೂರ್ಣಾಙ್ಕ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ಭಙ್ಗ;
		ಪಕ್ಷ VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (ಪೂರ್ಣಾಙ್ಕ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ಭಙ್ಗ;
		ಪಕ್ಷ VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (ಪೂರ್ಣಾಙ್ಕ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ಭಙ್ಗ;
		ಪಕ್ಷ VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (ಪೂರ್ಣಾಙ್ಕ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_names[vpi_call_error_lst[idx].file_idx],
                                 (ಪೂರ್ಣಾಙ್ಕ)vpi_call_error_lst[idx].lineno);
		  ಭಙ್ಗ;
	    }
	    ಮುಕ್ತ(vpi_call_error_lst[idx].name);
      }
      ಮುಕ್ತ(vpi_call_error_lst);
      fflush(stderr);
}

#ifdef CHECK_WITH_VALGRIND
ಸ್ಥಿರ ಶೂನ್ಯ cleanup_vpi_call_args(unsigned argc, vpiHandle*argv)
{
      ಯದಿ (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    ಸಂರಚನಾ __vpiSysTaskCall*obj = ನೂತನ systask_def;
	    obj->nargs = argc;
	    obj->args  = argv;
	    vpi_call_delete(obj);
      }
}
#endif

/*
 * A vpi_call is actually built up into a vpiSysTaskCall VPI object
 * that refers back to the vpiUserSystf VPI object that is the
 * definition. So this function is called by the compiler when a
 * %vpi_call statement is encountered. Create here a vpiHandle that
 * describes the call, and return it. The %vpi_call instruction will
 * store this handle for when it is executed.
 */
vpiHandle vpip_build_vpi_call(ಧ್ರುವ ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ val_code, unsigned return_width,
			      vvp_net_t*fnet,
			      ತಾರ್ಕಿಕ func_as_task_err, ತಾರ್ಕಿಕ func_as_task_warn,
			      unsigned argc, vpiHandle*argv,
			      unsigned vec4_ರಾಶಿ, unsigned real_ರಾಶಿ, unsigned string_ರಾಶಿ,
			      long file_idx, long lineno)
{
      ಪ್ರತಿಜ್ಞಾ(!(func_as_task_err && func_as_task_warn));

      ಸಂರಚನಾ __vpiUserSystf*defn = vpip_find_systf(name);
      ಯದಿ (defn == 0) {
	    add_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
	    cleanup_vpi_call_args(argc, argv);
#endif
	    ಪ್ರತ್ಯಾಗಮ 0;
      }

      ವಿಕಲ್ಪ (defn->info.type) {
	  ಪಕ್ಷ vpiSysTask:
	    ಯದಿ (val_code != 0 || fnet != 0) {
		  add_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#ifdef CHECK_WITH_VALGRIND
		  cleanup_vpi_call_args(argc, argv);
#endif
		  ಪ್ರತ್ಯಾಗಮ 0;
	    }
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiSysFunc:
	    ಯದಿ (val_code == 0 && fnet == 0) {
		  ಯದಿ (func_as_task_err) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
			cleanup_vpi_call_args(argc, argv);
#endif
			ಪ್ರತ್ಯಾಗಮ 0;
		  } ಅನ್ಯಥಾ ಯದಿ (func_as_task_warn) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (ಪೂರ್ಣಾಙ್ಕ)defn->info.type);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      ಸಂರಚನಾ __vpiSysTaskCall*obj = 0;

      ವಿಕಲ್ಪ (defn->info.type) {
	  ಪಕ್ಷ vpiSysTask:
	    obj = ನೂತನ systask_def;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiSysFunc:
	    ಯದಿ (fnet && val_code == -vpiRealVal) {
		  obj = ನೂತನ sysfunc_rnet;

	    } ಅನ್ಯಥಾ ಯದಿ (fnet && val_code == -vpiVectorVal) {
		  obj = ನೂತನ sysfunc_4net(return_width);

	    } ಅನ್ಯಥಾ ಯದಿ (val_code == -vpiRealVal) {
		  obj = ನೂತನ sysfunc_real;

	    } ಅನ್ಯಥಾ ಯದಿ (val_code == -vpiVectorVal) {
		  obj = ನೂತನ sysfunc_vec4(return_width);

	    } ಅನ್ಯಥಾ ಯದಿ (val_code == -vpiStringVal) {
		  obj = ನೂತನ sysfunc_str;

           } ಅನ್ಯಥಾ ಯದಿ (val_code == 0 && fnet == 0) {
		  obj = ನೂತನ sysfunc_no;

	    } ಅನ್ಯಥಾ {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_code);
		  ಪ್ರತಿಜ್ಞಾ(0);
	    }
	    ಭಙ್ಗ;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->args  = argv;
      obj->vec4_ರಾಶಿ = vec4_ರಾಶಿ;
      obj->real_ರಾಶಿ = real_ರಾಶಿ;
      obj->string_ರಾಶಿ = string_ರಾಶಿ;
      obj->fnet  = fnet;
      obj->file_idx  = (unsigned) file_idx;
      obj->lineno   = (unsigned) lineno;
      obj->userdata  = 0;
      obj->put_ಮೌಲ್ಯ = ಅಸತ್ಯ;

      compile_compiletf(obj);

      ಪ್ರತ್ಯಾಗಮ obj;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ vpi_call_delete(vpiHandle item)
{
      ಸಂರಚನಾ __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(item);
	/* The object can be NULL if there was an error. */
      ಯದಿ (!obj) ಪ್ರತ್ಯಾಗಮ;
      ಪ್ರತಿ (unsigned arg = 0; arg < obj->nargs; arg += 1) {
	    ವಿಕಲ್ಪ (obj->args[arg]->get_type_code()) {
		ಪಕ್ಷ vpiConstant:
		  ವಿಕಲ್ಪ (vpi_get(_vpiFromThr, obj->args[arg])) {
		      ಪಕ್ಷ _vpiNoThr:
			constant_delete(obj->args[arg]);
			ಭಙ್ಗ;
		      ಪಕ್ಷ _vpiString:
			thread_string_delete(obj->args[arg]);
			ಭಙ್ಗ;
		      ಪಕ್ಷ _vpiVThr:
			thread_vthr_delete(obj->args[arg]);
			ಭಙ್ಗ;
		      ಪಕ್ಷ _vpiWord:
			thread_word_delete(obj->args[arg]);
			ಭಙ್ಗ;
		      ಸ್ವಭಾವ:
			ಪ್ರತಿಜ್ಞಾ(0);
		  }
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiMemoryWord:
		  ವಿಕಲ್ಪ (vpi_get(_vpiFromThr, obj->args[arg])) {
		      ಪಕ್ಷ _vpi_at_A:
			A_delete(obj->args[arg]);
			ಭಙ್ಗ;
		  }
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiPartSelect:
		  ವಿಕಲ್ಪ (vpi_get(_vpiFromThr, obj->args[arg])) {
		      ಪಕ್ಷ _vpi_at_PV:
			PV_delete(obj->args[arg]);
			ಭಙ್ಗ;
		      ಪಕ್ಷ _vpi_at_APV:
			APV_delete(obj->args[arg]);
			ಭಙ್ಗ;
		  }
		  ಭಙ್ಗ;
	    }
      }
      ಮುಕ್ತ(obj->args);
      delete obj;
}
#endif

/*
 * This function is used by the %vpi_call instruction to actually
 * place the call to the system task/function. For now, only support
 * calls to system tasks.
 */

vthread_t vpip_current_vthread;

ಶೂನ್ಯ vpip_execute_vpi_call(vthread_t thr, vpiHandle ref)
{
      vpip_current_vthread = thr;

      vpip_cur_task = dynamic_cast<__vpiSysTaskCall*>(ref);

      ಯದಿ (vpip_cur_task->defn->info.calltf) {
	    ಪ್ರತಿಜ್ಞಾ(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = VPI_MODE_CALLTF;
	    vpip_cur_task->put_ಮೌಲ್ಯ = ಅಸತ್ಯ;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_data);
	    vpi_mode_flag = VPI_MODE_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    ಯದಿ (ref->get_type_code() == vpiSysFuncCall &&
	        !vpip_cur_task->put_ಮೌಲ್ಯ) {
		  s_vpi_ಮೌಲ್ಯ val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_ಮೌಲ್ಯ(ref, &val, 0, vpiNoDelay);
	    }
      }
      ಯದಿ (vpip_cur_task->vec4_ರಾಶಿ > 0)
	    vthread_ತೆಗೆ_vec4(thr, vpip_cur_task->vec4_ರಾಶಿ);
      ಯದಿ (vpip_cur_task->real_ರಾಶಿ > 0)
	    vthread_ತೆಗೆ_real(thr, vpip_cur_task->real_ರಾಶಿ);
      ಯದಿ (vpip_cur_task->string_ರಾಶಿ > 0)
	    vthread_ತೆಗೆ_str(thr, vpip_cur_task->string_ರಾಶಿ);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      ಯದಿ (ಧ್ರುವ sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_ತಳ್ಳು(thr, func_real->return_ಮೌಲ್ಯ());
      }
      ಅನ್ಯಥಾ ಯದಿ (ಧ್ರುವ sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_ತಳ್ಳು(thr, func_string->return_ಮೌಲ್ಯ());
      }
      ಅನ್ಯಥಾ ಯದಿ (ಧ್ರುವ sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
	    vthread_ತಳ್ಳು(thr, func_vec4->return_ಮೌಲ್ಯ());
      }
      vpip_cur_task = 0;
}

/*
 * This is the entry function that a VPI module uses to hook a new
 * task/function into the simulator. The function creates a new
 * __vpi_userSystf to represent the definition for the calls that come
 * to pass later.
 */
vpiHandle vpi_register_systf(ಧ್ರುವ ಸಂರಚನಾ t_vpi_systf_data*ss)
{
      ಸಂರಚನಾ __vpiUserSystf*cur = allocate_def();
      ಪ್ರತಿಜ್ಞಾ(ss);
      ವಿಕಲ್ಪ (ss->type) {
	  ಪಕ್ಷ vpiSysTask:
	  ಪಕ್ಷ vpiSysFunc:
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Unsupported type %d.\n", (ಪೂರ್ಣಾಙ್ಕ)ss->type);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = ಸತ್ಯ;

      ಪ್ರತ್ಯಾಗಮ cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, ಶೂನ್ಯ*data)
{
      ಸಂರಚನಾ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      ಯದಿ (rfp == 0)
	    ಪ್ರತ್ಯಾಗಮ 0;

      rfp->userdata = data;
      ಪ್ರತ್ಯಾಗಮ 1;
}

ಶೂನ್ಯ* vpi_get_userdata(vpiHandle ref)
{
      ಸಂರಚನಾ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಪ್ರತ್ಯಾಗಮ rfp->userdata;
}
