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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

ಬಳಸಿ ನಾಮಾಕಾಶ std;

ಸಾಲೊಳಗೆ __vpiUserSystf::__vpiUserSystf()
{ }

ಪೂರ್ಣಾಂಕ __vpiUserSystf::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiUserSystf; }


ಸ್ಥಿರ vpiHandle systask_handle(ಪೂರ್ಣಾಂಕ type, vpiHandle ref)
{
      ರಚನೆ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ಆಯ್ಕೆ (type) {
	  ಸ್ಥಿತಿ vpiScope:
	    ಹಿಂತಿರುಗಿ rfp->scope;

	  ಸ್ಥಿತಿ vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    ಪ್ರತಿಜ್ಞಾ(rfp->defn->is_user_defn);
	    ಹಿಂತಿರುಗಿ rfp->defn;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      };
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ systask_get(ಪೂರ್ಣಾಂಕ type, vpiHandle ref)
{
      ರಚನೆ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ಆಯ್ಕೆ (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  ಸ್ಥಿತಿ vpi_ಸಮಯ_Unit:
	    ಹಿಂತಿರುಗಿ rfp->scope->ಸಮಯ_units;
	  ಸ್ಥಿತಿ vpi_ಸಮಯ_Precision:
	    ಹಿಂತಿರುಗಿ rfp->scope->ಸಮಯ_precision;

	  ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ rfp->lineno;

	  ಸ್ಥಿತಿ vpiUserDefn:
	    ಹಿಂತಿರುಗಿ rfp->defn->is_user_defn;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ vpiUndefined;
      }
}

// support getting vpiSize for a system function call
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sysfunc_get(ಪೂರ್ಣಾಂಕ type, vpiHandle ref)
{
      ರಚನೆ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ಆಯ್ಕೆ (type) {
	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಪ್ರತಿಜ್ಞಾ(0); // This should be handled by derived classes
	    ಹಿಂತಿರುಗಿ 0;

	  ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ rfp->lineno;

	  ಸ್ಥಿತಿ vpiUserDefn:
	    ಹಿಂತಿರುಗಿ rfp->defn->is_user_defn;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

ಸ್ಥಿರ ಅಕ್ಷರ *systask_get_str(ಪೂರ್ಣಾಂಕ type, vpiHandle ref)
{
      ಸ್ಥಿರಾಂಕ ರಚನೆ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ಆಯ್ಕೆ (type) {
          ಸ್ಥಿತಿ vpiFile:
            ಪ್ರತಿಜ್ಞಾ(rfp->file_idx < file_ಹೆಸರುಗಳು.size());
            ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[rfp->file_idx]);

          ಸ್ಥಿತಿ vpiName:
            ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(rfp->defn->info.tfname);
      }

      ಹಿಂತಿರುಗಿ 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
ಸ್ಥಿರ vpiHandle systask_iter(ಪೂರ್ಣಾಂಕ, vpiHandle ref)
{
      ರಚನೆ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ಆದರೆ (rfp->nargs == 0)
	    ಹಿಂತಿರುಗಿ 0;

      ಹಿಂತಿರುಗಿ vpip_make_iterator(rfp->nargs, rfp->ವಾದಗಳು, ಸುಳ್ಳು);
}

ರಚನೆ systask_def : ಸಾರ್ವಜನಿಕ __vpiSysTaskCall {
      ವಾಸ್ತವಿಕ ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ   override { ಹಿಂತಿರುಗಿ vpiSysTaskCall; }
      ವಾಸ್ತವಿಕ ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code)           override { ಹಿಂತಿರುಗಿ systask_get(code, ಇದು); }
      ವಾಸ್ತವಿಕ ಅಕ್ಷರ*vpi_get_str(ಪೂರ್ಣಾಂಕ code)      override { ಹಿಂತಿರುಗಿ systask_get_str(code, ಇದು); }
      ವಾಸ್ತವಿಕ vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ code)  override { ಹಿಂತಿರುಗಿ systask_handle(code, ಇದು); }
      ವಾಸ್ತವಿಕ vpiHandle vpi_iterate(ಪೂರ್ಣಾಂಕ code) override { ಹಿಂತಿರುಗಿ systask_iter(code, ಇದು); }
};

ರಚನೆ sysfunc_def : ಸಾರ್ವಜನಿಕ systask_def {
      ವಾಸ್ತವಿಕ ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiSysFuncCall; }
      ವಾಸ್ತವಿಕ ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override { ಹಿಂತಿರುಗಿ sysfunc_get(code, ಇದು); }
};

ರಚನೆ sysfunc_real : ಸಾರ್ವಜನಿಕ sysfunc_def {
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags) override;
      ಸಾಲೊಳಗೆ ದ್ವಿಗುಣ return_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ return_ಮೌಲ್ಯ_2; }

    ಖಾಸಗಿ:
      ದ್ವಿಗುಣ return_ಮೌಲ್ಯ_2;
};

vpiHandle sysfunc_real::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ)
{
      put_ಮೌಲ್ಯ = ನಿಜ;

      return_ಮೌಲ್ಯ_2 = 0.0;
      ಆಯ್ಕೆ (vp->format) {
	  ಸ್ಥಿತಿ vpiRealVal:
	    return_ಮೌಲ್ಯ_2 = vp->value.real;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported format %d.\n", (ಪೂರ್ಣಾಂಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      ಹಿಂತಿರುಗಿ 0;
}

ರಚನೆ sysfunc_str : ಸಾರ್ವಜನಿಕ sysfunc_def {
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags) override;
      ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ std::string& return_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ return_ಮೌಲ್ಯ_2; };

    ಖಾಸಗಿ:
      std::string return_ಮೌಲ್ಯ_2;
};

ಪೂರ್ಣಾಂಕ sysfunc_str::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ return_ಮೌಲ್ಯ_2.size();

	  ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ lineno;

	  ಸ್ಥಿತಿ vpiUserDefn:
	    ಹಿಂತಿರುಗಿ defn->is_user_defn;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ)
{
      put_ಮೌಲ್ಯ = ನಿಜ;

      ಆಯ್ಕೆ (vp->format) {
	  ಸ್ಥಿತಿ vpiStringVal:
	    return_ಮೌಲ್ಯ_2 = std::string(vp->value.str);
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported format %d.\n", (ಪೂರ್ಣಾಂಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      ಹಿಂತಿರುಗಿ 0;
}

ವರ್ಗ sysfunc_vec4 : ಸಾರ್ವಜನಿಕ sysfunc_def {
    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ ಸಾಲೊಳಗೆ sysfunc_vec4(ಚಿಹ್ನೆರಹಿತ wid): return_ಮೌಲ್ಯ_2(wid, BIT4_X) { }
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags) override;
      ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ vvp_vector4_t& return_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ return_ಮೌಲ್ಯ_2; }

    ಖಾಸಗಿ:
      vpiHandle put_ಮೌಲ್ಯ_scalar(p_vpi_ಮೌಲ್ಯ vp);
      vpiHandle put_ಮೌಲ್ಯ_int(p_vpi_ಮೌಲ್ಯ vp);
      vpiHandle put_ಮೌಲ್ಯ_string(p_vpi_ಮೌಲ್ಯ vp);
      vpiHandle put_ಮೌಲ್ಯ_ಸದಿಶ(p_vpi_ಮೌಲ್ಯ vp);
      vpiHandle put_ಮೌಲ್ಯ_ಸಮಯ(p_vpi_ಮೌಲ್ಯ vp);

      vvp_vector4_t return_ಮೌಲ್ಯ_2;
};

ಪೂರ್ಣಾಂಕ sysfunc_vec4::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ return_ಮೌಲ್ಯ_2.size();

	  ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ lineno;

	  ಸ್ಥಿತಿ vpiUserDefn:
	    ಹಿಂತಿರುಗಿ defn->is_user_defn;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_ಮೌಲ್ಯ_scalar(ಸ್ಥಿರಾಂಕ p_vpi_ಮೌಲ್ಯ vp)
{
      ಆಯ್ಕೆ (vp->value.scalar) {
	  ಸ್ಥಿತಿ vpi0:
	    return_ಮೌಲ್ಯ_2.ಗಣ_ಬಿಟ್(0, BIT4_0);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpi1:
	    return_ಮೌಲ್ಯ_2.ಗಣ_ಬಿಟ್(0, BIT4_1);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiX:
	    return_ಮೌಲ್ಯ_2.ಗಣ_ಬಿಟ್(0, BIT4_X);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiZ:
	    return_ಮೌಲ್ಯ_2.ಗಣ_ಬಿಟ್(0, BIT4_Z);
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported value %d.\n", (ಪೂರ್ಣಾಂಕ)vp->value.scalar);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
      ಹಿಂತಿರುಗಿ 0;
}

vpiHandle sysfunc_vec4::put_ಮೌಲ್ಯ_int(p_vpi_ಮೌಲ್ಯ vp)
{
      ದೀರ್ಘ tmp = vp->value.integer;
      ಚಿಹ್ನೆರಹಿತ width = return_ಮೌಲ್ಯ_2.size();
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < width ;  idx += 1) {
	    return_ಮೌಲ್ಯ_2.ಗಣ_ಬಿಟ್(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      ಹಿಂತಿರುಗಿ 0;
}

vpiHandle sysfunc_vec4::put_ಮೌಲ್ಯ_string(p_vpi_ಮೌಲ್ಯ vp)
{
      size_t slen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(vp->value.str);
      ಚಿಹ್ನೆರಹಿತ wid = return_ಮೌಲ್ಯ_2.size();

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < wid ; idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    ಆದರೆ (cidx >= slen) {
		  return_ಮೌಲ್ಯ_2.ಗಣ_ಬಿಟ್(idx, BIT4_0);
		  ಮುಂದುವರಿ;
	    }

	      // Put the end of the string in the LSB of the vector
	    ಅಕ್ಷರ use_char = vp->value.str[slen - 1 - cidx];

	    ಆದರೆ ((use_char >> (idx % 8)) & 1)
		  return_ಮೌಲ್ಯ_2.ಗಣ_ಬಿಟ್(idx, BIT4_1);
	    ಇಲ್ಲದಿದ್ದರೆ
		  return_ಮೌಲ್ಯ_2.ಗಣ_ಬಿಟ್(idx, BIT4_0);
      }
      ಹಿಂತಿರುಗಿ 0;
}

vpiHandle sysfunc_vec4::put_ಮೌಲ್ಯ_ಸದಿಶ(p_vpi_ಮೌಲ್ಯ vp)
{
      ಚಿಹ್ನೆರಹಿತ width = return_ಮೌಲ್ಯ_2.size();
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < width ; idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ = idx / 32;
	    ಚಿಹ್ನೆರಹಿತ bidx = idx % 32;

	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ aval = vp->value.vector[ಶಬ್ದ].aval >> bidx;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ bval = vp->value.vector[ಶಬ್ದ].bval >> bidx;

	    ಪೂರ್ಣಾಂಕ ಬಿಟ್ = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    ಆಯ್ಕೆ (ಬಿಟ್) {
		ಸ್ಥಿತಿ 0:
		  bit4 = BIT4_0;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 1:
		  bit4 = BIT4_1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 2:
		  bit4 = BIT4_Z;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 3:
		  bit4 = BIT4_X;
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  ಪ್ರತಿಜ್ಞಾ(0);
		  bit4 = BIT4_X;
		  ನಿಲ್ಲಿಸು;
	    }
	    return_ಮೌಲ್ಯ_2.ಗಣ_ಬಿಟ್(idx, bit4);
      }
      ಹಿಂತಿರುಗಿ 0;
}

vpiHandle sysfunc_vec4::put_ಮೌಲ್ಯ_ಸಮಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      ಚಿಹ್ನೆರಹಿತ width = return_ಮೌಲ್ಯ_2.size();
      ದೀರ್ಘ tmp = 0;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < width ; idx += 1) {
	    ಆದರೆ (idx == 0)
		  tmp = vp->value.time->ಕೆಳಗಿನ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == 32)
		  tmp = vp->value.time->ಮೇಲಿನ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == 64)
		  tmp = 0;

	    return_ಮೌಲ್ಯ_2.ಗಣ_ಬಿಟ್(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      ಹಿಂತಿರುಗಿ 0;
}

vpiHandle sysfunc_vec4::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ)
{
      put_ಮೌಲ್ಯ = ನಿಜ;

      ಆಯ್ಕೆ (vp->format) {
	  ಸ್ಥಿತಿ vpiScalarVal:
	    ಹಿಂತಿರುಗಿ put_ಮೌಲ್ಯ_scalar(vp);
	  ಸ್ಥಿತಿ vpiIntVal:
	    ಹಿಂತಿರುಗಿ put_ಮೌಲ್ಯ_int(vp);
	  ಸ್ಥಿತಿ vpiStringVal:
	    ಹಿಂತಿರುಗಿ put_ಮೌಲ್ಯ_string(vp);
	  ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:
	    ಹಿಂತಿರುಗಿ put_ಮೌಲ್ಯ_ಸದಿಶ(vp);
	  ಸ್ಥಿತಿ vpi_ಸಮಯ_Val:
	    ಹಿಂತಿರುಗಿ put_ಮೌಲ್ಯ_ಸಮಯ(vp);
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (ಪೂರ್ಣಾಂಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      ಹಿಂತಿರುಗಿ 0;
}

ರಚನೆ sysfunc_4net : ಸಾರ್ವಜನಿಕ sysfunc_def {
      ಸ್ಪಷ್ಟ ಸಾಲೊಳಗೆ sysfunc_4net(ಚಿಹ್ನೆರಹಿತ wid) : vwid_(wid) { }
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags) override;

    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ vwid_;
};

// support getting vpiSize for a system function call
ಪೂರ್ಣಾಂಕ sysfunc_4net::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ vwid_;

	  ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ lineno;

	  ಸ್ಥಿತಿ vpiUserDefn:
	    ಹಿಂತಿರುಗಿ defn->is_user_defn;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ)
{
      put_ಮೌಲ್ಯ = ನಿಜ;

      vvp_vector4_t val (vwid_);

      ಆಯ್ಕೆ (vp->format) {

          ಸ್ಥಿತಿ vpiScalarVal: {
	        ಆಯ್ಕೆ(vp->value.scalar) {
		      ಸ್ಥಿತಿ vpi0:
			val.ಗಣ_ಬಿಟ್(0, BIT4_0);
                        ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ vpi1:
                        val.ಗಣ_ಬಿಟ್(0, BIT4_1);
                        ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ vpiX:
                        val.ಗಣ_ಬಿಟ್(0, BIT4_X);
                        ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ vpiZ:
                        val.ಗಣ_ಬಿಟ್(0, BIT4_Z);
                        ನಿಲ್ಲಿಸು;
		      ಪೂರ್ವನಿಯೋಜಿತ:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (ಪೂರ್ಣಾಂಕ)vp->value.scalar);
                        ಪ್ರತಿಜ್ಞಾ(0);
                }
		ನಿಲ್ಲಿಸು;
          }

	  ಸ್ಥಿತಿ vpiIntVal: {
		ದೀರ್ಘ tmp = vp->value.integer;
		ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.ಗಣ_ಬಿಟ್(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		ನಿಲ್ಲಿಸು;
	  }

          ಸ್ಥಿತಿ vpi_ಸಮಯ_Val: {
                ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp = vp->value.time->ಕೆಳಗಿನ;
                ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.ಗಣ_ಬಿಟ್(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      ಆದರೆ (idx == 31)
                            tmp = vp->value.time->ಮೇಲಿನ;
                      ಇಲ್ಲದಿದ್ದರೆ
                            tmp >>= 1;
                }
                ನಿಲ್ಲಿಸು;
          }

	  ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ = wdx / 32;
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ aval = vp->value.vector[ಶಬ್ದ].aval;
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ bval = vp->value.vector[ಶಬ್ದ].bval;

		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			ಪೂರ್ಣಾಂಕ ಬಿಟ್ = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			ಆಯ್ಕೆ (ಬಿಟ್) {
			    ಸ್ಥಿತಿ 0:
			      bit4 = BIT4_0;
			      ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 1:
			      bit4 = BIT4_1;
			      ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 2:
			      bit4 = BIT4_Z;
			      ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 3:
			      bit4 = BIT4_X;
			      ನಿಲ್ಲಿಸು;
			    ಪೂರ್ವನಿಯೋಜಿತ:
			      bit4 = BIT4_X;
			      fprintf(stderr, "Unsupported bit value %d.\n",
			              ಬಿಟ್);
			      ಪ್ರತಿಜ್ಞಾ(0);
			}
			val.ಗಣ_ಬಿಟ್(wdx+idx, bit4);

			aval >>= 1;
			bval >>= 1;
		  }
	    }
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (ಪೂರ್ಣಾಂಕ)vp->format, vwid_);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      ಹಿಂತಿರುಗಿ 0;
}

ರಚನೆ sysfunc_rnet : ಸಾರ್ವಜನಿಕ sysfunc_def {
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ)
{
      put_ಮೌಲ್ಯ = ನಿಜ;

      ದ್ವಿಗುಣ value;
      ಆಯ್ಕೆ (vp->format) {

	  ಸ್ಥಿತಿ vpiRealVal:
	    value = vp->value.real;
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (ಪೂರ್ಣಾಂಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      ಹಿಂತಿರುಗಿ 0;
}

ರಚನೆ sysfunc_no : ಸಾರ್ವಜನಿಕ sysfunc_def {
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags) override;
};

vpiHandle sysfunc_no::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ)
{
      ಹಿಂತಿರುಗಿ 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
ಸ್ಥಿರ ರಚನೆ __vpiUserSystf**def_ಕೋಷ್ಟಕ = 0;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ def_ಎಣಿಕೆ = 0;

ಸ್ಥಿರ ರಚನೆ __vpiUserSystf* allocate_def(ಶೂನ್ಯ)
{
      ಆದರೆ (def_ಕೋಷ್ಟಕ == 0) {
	    def_ಕೋಷ್ಟಕ = static_cast<ರಚನೆ __vpiUserSystf**>
	                (ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ(ರಚನೆ __vpiUserSystf*)));

	    def_ಕೋಷ್ಟಕ[0] = ಹೊಸ __vpiUserSystf;

	    def_ಎಣಿಕೆ = 1;
	    ಹಿಂತಿರುಗಿ def_ಕೋಷ್ಟಕ[0];
      }

      def_ಕೋಷ್ಟಕ = static_cast<ರಚನೆ __vpiUserSystf**>
                  (realloc(def_ಕೋಷ್ಟಕ,
                   (def_ಎಣಿಕೆ+1)*ಗಾತ್ರ(ರಚನೆ __vpiUserSystf*)));

      def_ಕೋಷ್ಟಕ[def_ಎಣಿಕೆ] = ಹೊಸ __vpiUserSystf;

      ಹಿಂತಿರುಗಿ def_ಕೋಷ್ಟಕ[def_ಎಣಿಕೆ++];
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ def_ಕೋಷ್ಟಕ_delete(ಶೂನ್ಯ)
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < def_ಎಣಿಕೆ; idx += 1) {
	    ಬಿಡುಗಡೆ(const_cast<ಅಕ್ಷರ *>(def_ಕೋಷ್ಟಕ[idx]->info.tfname));
	    ಅಳಿಸು def_ಕೋಷ್ಟಕ[idx];
      }
      ಬಿಡುಗಡೆ(def_ಕೋಷ್ಟಕ);
      def_ಕೋಷ್ಟಕ = 0;
      def_ಎಣಿಕೆ = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ವರ್ಗ __vpiSystfIterator : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ __vpiSystfIterator(ಚಿಹ್ನೆರಹಿತ idx);
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      vpiHandle vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ idx) override;
      free_object_fun_t free_object_fun(ಶೂನ್ಯ) override;

      ಚಿಹ್ನೆರಹಿತ ಮುಂದಿನ_2;
};

ಸ್ಥಿರ vpiHandle systf_iterator_ಪರಿಶೀಲನೆ(vpiHandle ref, ಪೂರ್ಣಾಂಕ)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      ಆದರೆ (obj->ಮುಂದಿನ_2 >= def_ಎಣಿಕೆ) {
	    vpi_free_object(ref);
	    ಹಿಂತಿರುಗಿ 0;
      }

      ಚಿಹ್ನೆರಹಿತ use_ಸೂಚ್ಯಂಕ = obj->ಮುಂದಿನ_2;
      ಆಗ (!def_ಕೋಷ್ಟಕ[use_ಸೂಚ್ಯಂಕ]->is_user_defn) {
	    obj->ಮುಂದಿನ_2 += 1;
	    use_ಸೂಚ್ಯಂಕ = obj->ಮುಂದಿನ_2;
	    ಆದರೆ (obj->ಮುಂದಿನ_2 >= def_ಎಣಿಕೆ) {
		  vpi_free_object(ref);
		  ಹಿಂತಿರುಗಿ 0;
	    }
      }
      obj->ಮುಂದಿನ_2 += 1;
      ಹಿಂತಿರುಗಿ def_ಕೋಷ್ಟಕ[use_ಸೂಚ್ಯಂಕ];
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      ಅಳಿಸು obj;
      ಹಿಂತಿರುಗಿ 1;
}

ಸಾಲೊಳಗೆ __vpiSystfIterator::__vpiSystfIterator(ಚಿಹ್ನೆರಹಿತ idx)
{ ಮುಂದಿನ_2 = idx; }

ಪೂರ್ಣಾಂಕ __vpiSystfIterator::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ idx)
{ ಹಿಂತಿರುಗಿ systf_iterator_ಪರಿಶೀಲನೆ(ಇದು, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(ಶೂನ್ಯ)
{ ಹಿಂತಿರುಗಿ &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(ಶೂನ್ಯ)
{
	/* Check to see if there are any user defined functions. */
      ತಾರ್ಕಿಕ have_user_defn = ಸುಳ್ಳು;
      ಚಿಹ್ನೆರಹಿತ idx;
      ಗಾಗಿ (idx = 0; idx < def_ಎಣಿಕೆ; idx += 1) {
	    ಆದರೆ (def_ಕೋಷ್ಟಕ[idx]->is_user_defn) {
		  have_user_defn = ನಿಜ;
		  ನಿಲ್ಲಿಸು;
	    }
      }
      ಆದರೆ (!have_user_defn) ಹಿಂತಿರುಗಿ 0;

      __vpiSystfIterator*res = ಹೊಸ __vpiSystfIterator(idx);
      res->ಮುಂದಿನ_2 = idx;
      ಹಿಂತಿರುಗಿ res;
}

ರಚನೆ __vpiUserSystf* vpip_ಕಂಡುಹಿಡಿ_systf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name)
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < def_ಎಣಿಕೆ ;  idx += 1)
	    ಆದರೆ (strcmp(def_ಕೋಷ್ಟಕ[idx]->info.tfname, name) == 0)
		  ಹಿಂತಿರುಗಿ def_ಕೋಷ್ಟಕ[idx];

      ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ vpip_make_systf_system_defined(vpiHandle ref)
{
      ಪ್ರತಿಜ್ಞಾ(ref);
      ರಚನೆ __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      ಪ್ರತಿಜ್ಞಾ(obj);
      obj->is_user_defn = ಸುಳ್ಳು;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
ಎಣಿಕೆ vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ vpi_call_error {
      vpi_call_error_type type;
      ಅಕ್ಷರ *name;
      ದೀರ್ಘ file_idx;
      ದೀರ್ಘ lineno;
} vpi_call_error_s, *vpi_call_error_p;

ಸ್ಥಿರ vpi_call_error_p vpi_call_error_lst = ಖಾಲಿ;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ vpi_call_error_ಅಂಕೆ = 0;

ಸ್ಥಿರ ಶೂನ್ಯ ಕೂಡಿಸು_vpi_call_error(vpi_call_error_type type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                               ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (realloc(vpi_call_error_lst,
                                    (vpi_call_error_ಅಂಕೆ + 1) *
                                    ಗಾತ್ರ(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_ಅಂಕೆ].type = type;
      vpi_call_error_lst[vpi_call_error_ಅಂಕೆ].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_ಅಂಕೆ].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_ಅಂಕೆ].lineno = lineno;
      vpi_call_error_ಅಂಕೆ += 1;
}

ಶೂನ್ಯ print_vpi_call_errors()
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < vpi_call_error_ಅಂಕೆ; idx += 1) {
	    ಆಯ್ಕೆ (vpi_call_error_lst[idx].type) {
		ಸ್ಥಿತಿ VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_ಹೆಸರುಗಳು[vpi_call_error_lst[idx].file_idx],
		                  (ಪೂರ್ಣಾಂಕ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_ಹೆಸರುಗಳು[vpi_call_error_lst[idx].file_idx],
		                  (ಪೂರ್ಣಾಂಕ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_ಹೆಸರುಗಳು[vpi_call_error_lst[idx].file_idx],
		                  (ಪೂರ್ಣಾಂಕ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_ಹೆಸರುಗಳು[vpi_call_error_lst[idx].file_idx],
		                  (ಪೂರ್ಣಾಂಕ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_ಹೆಸರುಗಳು[vpi_call_error_lst[idx].file_idx],
                                 (ಪೂರ್ಣಾಂಕ)vpi_call_error_lst[idx].lineno);
		  ನಿಲ್ಲಿಸು;
	    }
	    ಬಿಡುಗಡೆ(vpi_call_error_lst[idx].name);
      }
      ಬಿಡುಗಡೆ(vpi_call_error_lst);
      fflush(stderr);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಸ್ಥಿರ ಶೂನ್ಯ cleanup_vpi_call_ವಾದಗಳು(ಚಿಹ್ನೆರಹಿತ argc, vpiHandle*argv)
{
      ಆದರೆ (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    ರಚನೆ __vpiSysTaskCall*obj = ಹೊಸ systask_def;
	    obj->nargs = argc;
	    obj->ವಾದಗಳು  = argv;
	    vpi_call_delete(obj);
      }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * A vpi_call is actually built up into a vpiSysTaskCall VPI object
 * that refers back to the vpiUserSystf VPI object that is the
 * definition. So this function is called by the compiler when a
 * %vpi_call statement is encountered. Create here a vpiHandle that
 * describes the call, and return it. The %vpi_call instruction will
 * store this handle for when it is executed.
 */
vpiHandle vpip_build_vpi_call(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ val_ಸಂಕೇತ, ಚಿಹ್ನೆರಹಿತ return_ಅಗಲ,
			      vvp_net_t*fnet,
			      ತಾರ್ಕಿಕ func_as_task_err, ತಾರ್ಕಿಕ func_as_task_warn,
			      ಚಿಹ್ನೆರಹಿತ argc, vpiHandle*argv,
			      ಚಿಹ್ನೆರಹಿತ vec4_ರಾಶಿ, ಚಿಹ್ನೆರಹಿತ real_ರಾಶಿ, ಚಿಹ್ನೆರಹಿತ string_ರಾಶಿ,
			      ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno)
{
      ಪ್ರತಿಜ್ಞಾ(!(func_as_task_err && func_as_task_warn));

      ರಚನೆ __vpiUserSystf*defn = vpip_ಕಂಡುಹಿಡಿ_systf(name);
      ಆದರೆ (defn == 0) {
	    ಕೂಡಿಸು_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
	    cleanup_vpi_call_ವಾದಗಳು(argc, argv);
#ಆದರೆ_ಅಂತ್ಯ
	    ಹಿಂತಿರುಗಿ 0;
      }

      ಆಯ್ಕೆ (defn->info.type) {
	  ಸ್ಥಿತಿ vpiSysTask:
	    ಆದರೆ (val_ಸಂಕೇತ != 0 || fnet != 0) {
		  ಕೂಡಿಸು_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
		  cleanup_vpi_call_ವಾದಗಳು(argc, argv);
#ಆದರೆ_ಅಂತ್ಯ
		  ಹಿಂತಿರುಗಿ 0;
	    }
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiSysFunc:
	    ಆದರೆ (val_ಸಂಕೇತ == 0 && fnet == 0) {
		  ಆದರೆ (func_as_task_err) {
			ಕೂಡಿಸು_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
			cleanup_vpi_call_ವಾದಗಳು(argc, argv);
#ಆದರೆ_ಅಂತ್ಯ
			ಹಿಂತಿರುಗಿ 0;
		  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (func_as_task_warn) {
			ಕೂಡಿಸು_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (ಪೂರ್ಣಾಂಕ)defn->info.type);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      ರಚನೆ __vpiSysTaskCall*obj = 0;

      ಆಯ್ಕೆ (defn->info.type) {
	  ಸ್ಥಿತಿ vpiSysTask:
	    obj = ಹೊಸ systask_def;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiSysFunc:
	    ಆದರೆ (fnet && val_ಸಂಕೇತ == -vpiRealVal) {
		  obj = ಹೊಸ sysfunc_rnet;

	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fnet && val_ಸಂಕೇತ == -vpi_ಸದಿಶ_Val) {
		  obj = ಹೊಸ sysfunc_4net(return_ಅಗಲ);

	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val_ಸಂಕೇತ == -vpiRealVal) {
		  obj = ಹೊಸ sysfunc_real;

	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val_ಸಂಕೇತ == -vpi_ಸದಿಶ_Val) {
		  obj = ಹೊಸ sysfunc_vec4(return_ಅಗಲ);

	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val_ಸಂಕೇತ == -vpiStringVal) {
		  obj = ಹೊಸ sysfunc_str;

           } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val_ಸಂಕೇತ == 0 && fnet == 0) {
		  obj = ಹೊಸ sysfunc_no;

	    } ಇಲ್ಲದಿದ್ದರೆ {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_ಸಂಕೇತ);
		  ಪ್ರತಿಜ್ಞಾ(0);
	    }
	    ನಿಲ್ಲಿಸು;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->ವಾದಗಳು  = argv;
      obj->vec4_ರಾಶಿ = vec4_ರಾಶಿ;
      obj->real_ರಾಶಿ = real_ರಾಶಿ;
      obj->string_ರಾಶಿ = string_ರಾಶಿ;
      obj->fnet  = fnet;
      obj->file_idx  = (ಚಿಹ್ನೆರಹಿತ) file_idx;
      obj->lineno   = (ಚಿಹ್ನೆರಹಿತ) lineno;
      obj->userdata  = 0;
      obj->put_ಮೌಲ್ಯ = ಸುಳ್ಳು;

      compile_compiletf(obj);

      ಹಿಂತಿರುಗಿ obj;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ vpi_call_delete(vpiHandle ಅಂಶ)
{
      ರಚನೆ __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(ಅಂಶ);
	/* The object can be NULL if there was an error. */
      ಆದರೆ (!obj) ಹಿಂತಿರುಗಿ;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ arg = 0; arg < obj->nargs; arg += 1) {
	    ಆಯ್ಕೆ (obj->ವಾದಗಳು[arg]->get_type_ಸಂಕೇತ()) {
		ಸ್ಥಿತಿ vpiConstant:
		  ಆಯ್ಕೆ (vpi_get(_vpiFromThr, obj->ವಾದಗಳು[arg])) {
		      ಸ್ಥಿತಿ _vpiNoThr:
			constant_delete(obj->ವಾದಗಳು[arg]);
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ _vpiString:
			thread_string_delete(obj->ವಾದಗಳು[arg]);
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ _vpiVThr:
			thread_vthr_delete(obj->ವಾದಗಳು[arg]);
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ vpi_ಶಬ್ದ:
			thread_ಶಬ್ದ_delete(obj->ವಾದಗಳು[arg]);
			ನಿಲ್ಲಿಸು;
		      ಪೂರ್ವನಿಯೋಜಿತ:
			ಪ್ರತಿಜ್ಞಾ(0);
		  }
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpi_ಸ್ಮರಣೆ_ಶಬ್ದ:
		  ಆಯ್ಕೆ (vpi_get(_vpiFromThr, obj->ವಾದಗಳು[arg])) {
		      ಸ್ಥಿತಿ _vpi_at_A:
			A_delete(obj->ವಾದಗಳು[arg]);
			ನಿಲ್ಲಿಸು;
		  }
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiPartSelect:
		  ಆಯ್ಕೆ (vpi_get(_vpiFromThr, obj->ವಾದಗಳು[arg])) {
		      ಸ್ಥಿತಿ _vpi_at_PV:
			PV_delete(obj->ವಾದಗಳು[arg]);
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ _vpi_at_APV:
			APV_delete(obj->ವಾದಗಳು[arg]);
			ನಿಲ್ಲಿಸು;
		  }
		  ನಿಲ್ಲಿಸು;
	    }
      }
      ಬಿಡುಗಡೆ(obj->ವಾದಗಳು);
      ಅಳಿಸು obj;
}
#ಆದರೆ_ಅಂತ್ಯ

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

      ಆದರೆ (vpip_cur_task->defn->info.calltf) {
	    ಪ್ರತಿಜ್ಞಾ(vpi_ಬಹುಲಕ_flag == VPI_ಬಹುಲಕ_NONE);
	    vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_CALLTF;
	    vpip_cur_task->put_ಮೌಲ್ಯ = ಸುಳ್ಳು;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_ದತ್ತಾಂಶ);
	    vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    ಆದರೆ (ref->get_type_ಸಂಕೇತ() == vpiSysFuncCall &&
	        !vpip_cur_task->put_ಮೌಲ್ಯ) {
		  s_vpi_ಮೌಲ್ಯ val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_ಮೌಲ್ಯ(ref, &val, 0, vpiNoDelay);
	    }
      }
      ಆದರೆ (vpip_cur_task->vec4_ರಾಶಿ > 0)
	    vthread_ತೆಗೆ_vec4(thr, vpip_cur_task->vec4_ರಾಶಿ);
      ಆದರೆ (vpip_cur_task->real_ರಾಶಿ > 0)
	    vthread_ತೆಗೆ_real(thr, vpip_cur_task->real_ರಾಶಿ);
      ಆದರೆ (vpip_cur_task->string_ರಾಶಿ > 0)
	    vthread_ತೆಗೆ_str(thr, vpip_cur_task->string_ರಾಶಿ);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      ಆದರೆ (ಸ್ಥಿರಾಂಕ sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_ತಳ್ಳು(thr, func_real->return_ಮೌಲ್ಯ());
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸ್ಥಿರಾಂಕ sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_ತಳ್ಳು(thr, func_string->return_ಮೌಲ್ಯ());
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸ್ಥಿರಾಂಕ sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
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
vpiHandle vpi_ರಿಜಿಸ್ಟರ್_systf(ಸ್ಥಿರಾಂಕ ರಚನೆ t_vpi_systf_ದತ್ತಾಂಶ*ss)
{
      ರಚನೆ __vpiUserSystf*cur = allocate_def();
      ಪ್ರತಿಜ್ಞಾ(ss);
      ಆಯ್ಕೆ (ss->type) {
	  ಸ್ಥಿತಿ vpiSysTask:
	  ಸ್ಥಿತಿ vpiSysFunc:
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported type %d.\n", (ಪೂರ್ಣಾಂಕ)ss->type);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = ನಿಜ;

      ಹಿಂತಿರುಗಿ cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, ಶೂನ್ಯ*ದತ್ತಾಂಶ)
{
      ರಚನೆ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      ಆದರೆ (rfp == 0)
	    ಹಿಂತಿರುಗಿ 0;

      rfp->userdata = ದತ್ತಾಂಶ;
      ಹಿಂತಿರುಗಿ 1;
}

ಶೂನ್ಯ* vpi_get_userdata(vpiHandle ref)
{
      ರಚನೆ __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಹಿಂತಿರುಗಿ rfp->userdata;
}
