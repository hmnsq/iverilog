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

using ନାମାକାଶ std;

ଧାଡ଼ିଭିତରେ __vpiUserSystf::__vpiUserSystf()
{ }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiUserSystf::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiUserSystf; }


ସ୍ଥିର vpiHandle systask_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, vpiHandle ref)
{
      ସଂରଚନା __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ବାଛ (type) {
	  ସ୍ଥିତି vpiScope:
	    ଫେରନ୍ତୁ rfp->scope;

	  ସ୍ଥିତି vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    ପ୍ରତିଜ୍ଞା(rfp->defn->is_user_defn);
	    ଫେରନ୍ତୁ rfp->defn;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      };
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା systask_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, vpiHandle ref)
{
      ସଂରଚନା __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ବାଛ (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  ସ୍ଥିତି vpiTimeUnit:
	    ଫେରନ୍ତୁ rfp->scope->time_units;
	  ସ୍ଥିତି vpiTimePrecision:
	    ଫେରନ୍ତୁ rfp->scope->time_precision;

	  ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ rfp->lineno;

	  ସ୍ଥିତି vpiUserDefn:
	    ଫେରନ୍ତୁ rfp->defn->is_user_defn;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ vpiUndefined;
      }
}

// support getting vpiSize for a system function call
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysfunc_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, vpiHandle ref)
{
      ସଂରଚନା __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ବାଛ (type) {
	  ସ୍ଥିତି vpiSize:
	    ପ୍ରତିଜ୍ଞା(0); // This should be handled by derived classes
	    ଫେରନ୍ତୁ 0;

	  ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ rfp->lineno;

	  ସ୍ଥିତି vpiUserDefn:
	    ଫେରନ୍ତୁ rfp->defn->is_user_defn;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

ସ୍ଥିର ଅକ୍ଷର *systask_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, vpiHandle ref)
{
      ଅଚଳ ସଂରଚନା __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ବାଛ (type) {
          ସ୍ଥିତି vpiFile:
            ପ୍ରତିଜ୍ଞା(rfp->file_idx < file_names.size());
            ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[rfp->file_idx]);

          ସ୍ଥିତି vpiName:
            ଫେରନ୍ତୁ simple_set_rbuf_str(rfp->defn->info.tfname);
      }

      ଫେରନ୍ତୁ 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
ସ୍ଥିର vpiHandle systask_iter(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, vpiHandle ref)
{
      ସଂରଚନା __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ଯଦି (rfp->nargs == 0)
	    ଫେରନ୍ତୁ 0;

      ଫେରନ୍ତୁ vpip_make_iterator(rfp->nargs, rfp->args, ମିଥ୍ୟା);
}

ସଂରଚନା systask_def : ସର୍ବସାଧାରଣ __vpiSysTaskCall {
      ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ   override { ଫେରନ୍ତୁ vpiSysTaskCall; }
      ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)           override { ଫେରନ୍ତୁ systask_get(code, ଏହି); }
      ଆଭାସୀ ଅକ୍ଷର*vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)      override { ଫେରନ୍ତୁ systask_get_str(code, ଏହି); }
      ଆଭାସୀ vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)  override { ଫେରନ୍ତୁ systask_handle(code, ଏହି); }
      ଆଭାସୀ vpiHandle vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override { ଫେରନ୍ତୁ systask_iter(code, ଏହି); }
};

ସଂରଚନା sysfunc_def : ସର୍ବସାଧାରଣ systask_def {
      ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiSysFuncCall; }
      ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override { ଫେରନ୍ତୁ sysfunc_get(code, ଏହି); }
};

ସଂରଚନା sysfunc_real : ସର୍ବସାଧାରଣ sysfunc_def {
      vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;
      ଧାଡ଼ିଭିତରେ ଦ୍ୱିଗୁଣ return_ମୂଲ୍ୟ() ଅଚଳ { ଫେରନ୍ତୁ return_ମୂଲ୍ୟ_2; }

    ବ୍ୟକ୍ତିଗତ:
      ଦ୍ୱିଗୁଣ return_ମୂଲ୍ୟ_2;
};

vpiHandle sysfunc_real::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      put_ମୂଲ୍ୟ = ସତ୍ୟ;

      return_ମୂଲ୍ୟ_2 = 0.0;
      ବାଛ (vp->format) {
	  ସ୍ଥିତି vpiRealVal:
	    return_ମୂଲ୍ୟ_2 = vp->value.real;
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported format %d.\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
	    ପ୍ରତିଜ୍ଞା(0);
      }

      ଫେରନ୍ତୁ 0;
}

ସଂରଚନା sysfunc_str : ସର୍ବସାଧାରଣ sysfunc_def {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;
      ଧାଡ଼ିଭିତରେ ଅଚଳ std::string& return_ମୂଲ୍ୟ() ଅଚଳ { ଫେରନ୍ତୁ return_ମୂଲ୍ୟ_2; };

    ବ୍ୟକ୍ତିଗତ:
      std::string return_ମୂଲ୍ୟ_2;
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysfunc_str::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ return_ମୂଲ୍ୟ_2.size();

	  ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ lineno;

	  ସ୍ଥିତି vpiUserDefn:
	    ଫେରନ୍ତୁ defn->is_user_defn;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      put_ମୂଲ୍ୟ = ସତ୍ୟ;

      ବାଛ (vp->format) {
	  ସ୍ଥିତି vpiStringVal:
	    return_ମୂଲ୍ୟ_2 = std::string(vp->value.str);
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported format %d.\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
	    ପ୍ରତିଜ୍ଞା(0);
      }

      ଫେରନ୍ତୁ 0;
}

ଶ୍ରେଣୀ sysfunc_vec4 : ସର୍ବସାଧାରଣ sysfunc_def {
    ସର୍ବସାଧାରଣ:
      explicit ଧାଡ଼ିଭିତରେ sysfunc_vec4(unsigned wid): return_ମୂଲ୍ୟ_2(wid, BIT4_X) { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;
      ଧାଡ଼ିଭିତରେ ଅଚଳ vvp_vector4_t& return_ମୂଲ୍ୟ() ଅଚଳ { ଫେରନ୍ତୁ return_ମୂଲ୍ୟ_2; }

    ବ୍ୟକ୍ତିଗତ:
      vpiHandle put_ମୂଲ୍ୟ_scalar(p_vpi_ମୂଲ୍ୟ vp);
      vpiHandle put_ମୂଲ୍ୟ_int(p_vpi_ମୂଲ୍ୟ vp);
      vpiHandle put_ମୂଲ୍ୟ_string(p_vpi_ମୂଲ୍ୟ vp);
      vpiHandle put_ମୂଲ୍ୟ_vector(p_vpi_ମୂଲ୍ୟ vp);
      vpiHandle put_ମୂଲ୍ୟ_time(p_vpi_ମୂଲ୍ୟ vp);

      vvp_vector4_t return_ମୂଲ୍ୟ_2;
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysfunc_vec4::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ return_ମୂଲ୍ୟ_2.size();

	  ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ lineno;

	  ସ୍ଥିତି vpiUserDefn:
	    ଫେରନ୍ତୁ defn->is_user_defn;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_ମୂଲ୍ୟ_scalar(ଅଚଳ p_vpi_ମୂଲ୍ୟ vp)
{
      ବାଛ (vp->value.scalar) {
	  ସ୍ଥିତି vpi0:
	    return_ମୂଲ୍ୟ_2.set_bit(0, BIT4_0);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpi1:
	    return_ମୂଲ୍ୟ_2.set_bit(0, BIT4_1);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiX:
	    return_ମୂଲ୍ୟ_2.set_bit(0, BIT4_X);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiZ:
	    return_ମୂଲ୍ୟ_2.set_bit(0, BIT4_Z);
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported value %d.\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->value.scalar);
	    ପ୍ରତିଜ୍ଞା(0);
      }
      ଫେରନ୍ତୁ 0;
}

vpiHandle sysfunc_vec4::put_ମୂଲ୍ୟ_int(p_vpi_ମୂଲ୍ୟ vp)
{
      long tmp = vp->value.integer;
      unsigned width = return_ମୂଲ୍ୟ_2.size();
      ପାଇଁ (unsigned idx = 0 ;  idx < width ;  idx += 1) {
	    return_ମୂଲ୍ୟ_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      ଫେରନ୍ତୁ 0;
}

vpiHandle sysfunc_vec4::put_ମୂଲ୍ୟ_string(p_vpi_ମୂଲ୍ୟ vp)
{
      size_t slen = strlen(vp->value.str);
      unsigned wid = return_ମୂଲ୍ୟ_2.size();

      ପାଇଁ (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    unsigned cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    ଯଦି (cidx >= slen) {
		  return_ମୂଲ୍ୟ_2.set_bit(idx, BIT4_0);
		  ଜାରି;
	    }

	      // Put the end of the string in the LSB of the vector
	    ଅକ୍ଷର use_char = vp->value.str[slen - 1 - cidx];

	    ଯଦି ((use_char >> (idx % 8)) & 1)
		  return_ମୂଲ୍ୟ_2.set_bit(idx, BIT4_1);
	    ଅନ୍ୟଥା
		  return_ମୂଲ୍ୟ_2.set_bit(idx, BIT4_0);
      }
      ଫେରନ୍ତୁ 0;
}

vpiHandle sysfunc_vec4::put_ମୂଲ୍ୟ_vector(p_vpi_ମୂଲ୍ୟ vp)
{
      unsigned width = return_ମୂଲ୍ୟ_2.size();
      ପାଇଁ (unsigned idx = 0 ; idx < width ; idx += 1) {
	    unsigned word = idx / 32;
	    unsigned bidx = idx % 32;

	    unsigned long aval = vp->value.vector[word].aval >> bidx;
	    unsigned long bval = vp->value.vector[word].bval >> bidx;

	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bit = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    ବାଛ (bit) {
		ସ୍ଥିତି 0:
		  bit4 = BIT4_0;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 1:
		  bit4 = BIT4_1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 2:
		  bit4 = BIT4_Z;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 3:
		  bit4 = BIT4_X;
		  ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  ପ୍ରତିଜ୍ଞା(0);
		  bit4 = BIT4_X;
		  ଭାଙ୍ଗ;
	    }
	    return_ମୂଲ୍ୟ_2.set_bit(idx, bit4);
      }
      ଫେରନ୍ତୁ 0;
}

vpiHandle sysfunc_vec4::put_ମୂଲ୍ୟ_time(p_vpi_ମୂଲ୍ୟ vp)
{
      unsigned width = return_ମୂଲ୍ୟ_2.size();
      long tmp = 0;
      ପାଇଁ (unsigned idx = 0 ; idx < width ; idx += 1) {
	    ଯଦି (idx == 0)
		  tmp = vp->value.time->ତଳ;
	    ଅନ୍ୟଥା ଯଦି (idx == 32)
		  tmp = vp->value.time->ଉପର;
	    ଅନ୍ୟଥା ଯଦି (idx == 64)
		  tmp = 0;

	    return_ମୂଲ୍ୟ_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      ଫେରନ୍ତୁ 0;
}

vpiHandle sysfunc_vec4::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      put_ମୂଲ୍ୟ = ସତ୍ୟ;

      ବାଛ (vp->format) {
	  ସ୍ଥିତି vpiScalarVal:
	    ଫେରନ୍ତୁ put_ମୂଲ୍ୟ_scalar(vp);
	  ସ୍ଥିତି vpiIntVal:
	    ଫେରନ୍ତୁ put_ମୂଲ୍ୟ_int(vp);
	  ସ୍ଥିତି vpiStringVal:
	    ଫେରନ୍ତୁ put_ମୂଲ୍ୟ_string(vp);
	  ସ୍ଥିତି vpiVectorVal:
	    ଫେରନ୍ତୁ put_ମୂଲ୍ୟ_vector(vp);
	  ସ୍ଥିତି vpiTimeVal:
	    ଫେରନ୍ତୁ put_ମୂଲ୍ୟ_time(vp);
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
	    ପ୍ରତିଜ୍ଞା(0);
      }

      ଫେରନ୍ତୁ 0;
}

ସଂରଚନା sysfunc_4net : ସର୍ବସାଧାରଣ sysfunc_def {
      explicit ଧାଡ଼ିଭିତରେ sysfunc_4net(unsigned wid) : vwid_(wid) { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;

    ବ୍ୟକ୍ତିଗତ:
      unsigned vwid_;
};

// support getting vpiSize for a system function call
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysfunc_4net::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ vwid_;

	  ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ lineno;

	  ସ୍ଥିତି vpiUserDefn:
	    ଫେରନ୍ତୁ defn->is_user_defn;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      put_ମୂଲ୍ୟ = ସତ୍ୟ;

      vvp_vector4_t val (vwid_);

      ବାଛ (vp->format) {

          ସ୍ଥିତି vpiScalarVal: {
	        ବାଛ(vp->value.scalar) {
		      ସ୍ଥିତି vpi0:
			val.set_bit(0, BIT4_0);
                        ଭାଙ୍ଗ;
		      ସ୍ଥିତି vpi1:
                        val.set_bit(0, BIT4_1);
                        ଭାଙ୍ଗ;
		      ସ୍ଥିତି vpiX:
                        val.set_bit(0, BIT4_X);
                        ଭାଙ୍ଗ;
		      ସ୍ଥିତି vpiZ:
                        val.set_bit(0, BIT4_Z);
                        ଭାଙ୍ଗ;
		      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->value.scalar);
                        ପ୍ରତିଜ୍ଞା(0);
                }
		ଭାଙ୍ଗ;
          }

	  ସ୍ଥିତି vpiIntVal: {
		long tmp = vp->value.integer;
		ପାଇଁ (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		ଭାଙ୍ଗ;
	  }

          ସ୍ଥିତି vpiTimeVal: {
                unsigned long tmp = vp->value.time->ତଳ;
                ପାଇଁ (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      ଯଦି (idx == 31)
                            tmp = vp->value.time->ଉପର;
                      ଅନ୍ୟଥା
                            tmp >>= 1;
                }
                ଭାଙ୍ଗ;
          }

	  ସ୍ଥିତି vpiVectorVal:

	    ପାଇଁ (unsigned wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  unsigned word = wdx / 32;
		  unsigned long aval = vp->value.vector[word].aval;
		  unsigned long bval = vp->value.vector[word].bval;

		  ପାଇଁ (unsigned idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା bit = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			ବାଛ (bit) {
			    ସ୍ଥିତି 0:
			      bit4 = BIT4_0;
			      ଭାଙ୍ଗ;
			    ସ୍ଥିତି 1:
			      bit4 = BIT4_1;
			      ଭାଙ୍ଗ;
			    ସ୍ଥିତି 2:
			      bit4 = BIT4_Z;
			      ଭାଙ୍ଗ;
			    ସ୍ଥିତି 3:
			      bit4 = BIT4_X;
			      ଭାଙ୍ଗ;
			    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			      bit4 = BIT4_X;
			      fprintf(stderr, "Unsupported bit value %d.\n",
			              bit);
			      ପ୍ରତିଜ୍ଞା(0);
			}
			val.set_bit(wdx+idx, bit4);

			aval >>= 1;
			bval >>= 1;
		  }
	    }
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format, vwid_);
	    ପ୍ରତିଜ୍ଞା(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      ଫେରନ୍ତୁ 0;
}

ସଂରଚନା sysfunc_rnet : ସର୍ବସାଧାରଣ sysfunc_def {
      vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      put_ମୂଲ୍ୟ = ସତ୍ୟ;

      ଦ୍ୱିଗୁଣ value;
      ବାଛ (vp->format) {

	  ସ୍ଥିତି vpiRealVal:
	    value = vp->value.real;
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
	    ପ୍ରତିଜ୍ଞା(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      ଫେରନ୍ତୁ 0;
}

ସଂରଚନା sysfunc_no : ସର୍ବସାଧାରଣ sysfunc_def {
      vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;
};

vpiHandle sysfunc_no::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      ଫେରନ୍ତୁ 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
ସ୍ଥିର ସଂରଚନା __vpiUserSystf**def_table = 0;
ସ୍ଥିର unsigned def_ଗଣନା = 0;

ସ୍ଥିର ସଂରଚନା __vpiUserSystf* allocate_def(ଶୂନ୍ୟ)
{
      ଯଦି (def_table == 0) {
	    def_table = static_cast<ସଂରଚନା __vpiUserSystf**>
	                (malloc(ଆକାର(ସଂରଚନା __vpiUserSystf*)));

	    def_table[0] = ନୂତନ __vpiUserSystf;

	    def_ଗଣନା = 1;
	    ଫେରନ୍ତୁ def_table[0];
      }

      def_table = static_cast<ସଂରଚନା __vpiUserSystf**>
                  (realloc(def_table,
                   (def_ଗଣନା+1)*ଆକାର(ସଂରଚନା __vpiUserSystf*)));

      def_table[def_ଗଣନା] = ନୂତନ __vpiUserSystf;

      ଫେରନ୍ତୁ def_table[def_ଗଣନା++];
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ def_table_delete(ଶୂନ୍ୟ)
{
      ପାଇଁ (unsigned idx = 0; idx < def_ଗଣନା; idx += 1) {
	    ମୁକ୍ତ(const_cast<ଅକ୍ଷର *>(def_table[idx]->info.tfname));
	    delete def_table[idx];
      }
      ମୁକ୍ତ(def_table);
      def_table = 0;
      def_ଗଣନା = 0;
}
#endif

ଶ୍ରେଣୀ __vpiSystfIterator : ସର୍ବସାଧାରଣ __vpiHandle {
    ସର୍ବସାଧାରଣ:
      explicit __vpiSystfIterator(unsigned idx);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      vpiHandle vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx) override;
      free_object_fun_t free_object_fun(ଶୂନ୍ୟ) override;

      unsigned next;
};

ସ୍ଥିର vpiHandle systf_iterator_scan(vpiHandle ref, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      ଯଦି (obj->next >= def_ଗଣନା) {
	    vpi_free_object(ref);
	    ଫେରନ୍ତୁ 0;
      }

      unsigned use_index = obj->next;
      ଯେତେବେଳେ (!def_table[use_index]->is_user_defn) {
	    obj->next += 1;
	    use_index = obj->next;
	    ଯଦି (obj->next >= def_ଗଣନା) {
		  vpi_free_object(ref);
		  ଫେରନ୍ତୁ 0;
	    }
      }
      obj->next += 1;
      ଫେରନ୍ତୁ def_table[use_index];
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      delete obj;
      ଫେରନ୍ତୁ 1;
}

ଧାଡ଼ିଭିତରେ __vpiSystfIterator::__vpiSystfIterator(unsigned idx)
{ next = idx; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiSystfIterator::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{ ଫେରନ୍ତୁ systf_iterator_scan(ଏହି, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(ଶୂନ୍ୟ)
{ ଫେରନ୍ତୁ &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(ଶୂନ୍ୟ)
{
	/* Check to see if there are any user defined functions. */
      ତାର୍କିକ have_user_defn = ମିଥ୍ୟା;
      unsigned idx;
      ପାଇଁ (idx = 0; idx < def_ଗଣନା; idx += 1) {
	    ଯଦି (def_table[idx]->is_user_defn) {
		  have_user_defn = ସତ୍ୟ;
		  ଭାଙ୍ଗ;
	    }
      }
      ଯଦି (!have_user_defn) ଫେରନ୍ତୁ 0;

      __vpiSystfIterator*res = ନୂତନ __vpiSystfIterator(idx);
      res->next = idx;
      ଫେରନ୍ତୁ res;
}

ସଂରଚନା __vpiUserSystf* vpip_find_systf(ଅଚଳ ଅକ୍ଷର*name)
{
      ପାଇଁ (unsigned idx = 0 ;  idx < def_ଗଣନା ;  idx += 1)
	    ଯଦି (strcmp(def_table[idx]->info.tfname, name) == 0)
		  ଫେରନ୍ତୁ def_table[idx];

      ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ vpip_make_systf_system_defined(vpiHandle ref)
{
      ପ୍ରତିଜ୍ଞା(ref);
      ସଂରଚନା __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      ପ୍ରତିଜ୍ଞା(obj);
      obj->is_user_defn = ମିଥ୍ୟା;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
ଗଣନା vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା vpi_call_error {
      vpi_call_error_type type;
      ଅକ୍ଷର *name;
      long file_idx;
      long lineno;
} vpi_call_error_s, *vpi_call_error_p;

ସ୍ଥିର vpi_call_error_p vpi_call_error_lst = ଖାଲି;
ସ୍ଥିର unsigned vpi_call_error_ଅଙ୍କ = 0;

ସ୍ଥିର ଶୂନ୍ୟ add_vpi_call_error(vpi_call_error_type type, ଅଚଳ ଅକ୍ଷର *name,
                               long file_idx, long lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (realloc(vpi_call_error_lst,
                                    (vpi_call_error_ଅଙ୍କ + 1) *
                                    ଆକାର(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_ଅଙ୍କ].type = type;
      vpi_call_error_lst[vpi_call_error_ଅଙ୍କ].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_ଅଙ୍କ].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_ଅଙ୍କ].lineno = lineno;
      vpi_call_error_ଅଙ୍କ += 1;
}

ଶୂନ୍ୟ print_vpi_call_errors()
{
      ପାଇଁ (unsigned idx = 0; idx < vpi_call_error_ଅଙ୍କ; idx += 1) {
	    ବାଛ (vpi_call_error_lst[idx].type) {
		ସ୍ଥିତି VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_names[vpi_call_error_lst[idx].file_idx],
                                 (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vpi_call_error_lst[idx].lineno);
		  ଭାଙ୍ଗ;
	    }
	    ମୁକ୍ତ(vpi_call_error_lst[idx].name);
      }
      ମୁକ୍ତ(vpi_call_error_lst);
      fflush(stderr);
}

#ifdef CHECK_WITH_VALGRIND
ସ୍ଥିର ଶୂନ୍ୟ cleanup_vpi_call_args(unsigned argc, vpiHandle*argv)
{
      ଯଦି (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    ସଂରଚନା __vpiSysTaskCall*obj = ନୂତନ systask_def;
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
vpiHandle vpip_build_vpi_call(ଅଚଳ ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା val_code, unsigned return_width,
			      vvp_net_t*fnet,
			      ତାର୍କିକ func_as_task_err, ତାର୍କିକ func_as_task_warn,
			      unsigned argc, vpiHandle*argv,
			      unsigned vec4_ଗଦା, unsigned real_ଗଦା, unsigned string_ଗଦା,
			      long file_idx, long lineno)
{
      ପ୍ରତିଜ୍ଞା(!(func_as_task_err && func_as_task_warn));

      ସଂରଚନା __vpiUserSystf*defn = vpip_find_systf(name);
      ଯଦି (defn == 0) {
	    add_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
	    cleanup_vpi_call_args(argc, argv);
#endif
	    ଫେରନ୍ତୁ 0;
      }

      ବାଛ (defn->info.type) {
	  ସ୍ଥିତି vpiSysTask:
	    ଯଦି (val_code != 0 || fnet != 0) {
		  add_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#ifdef CHECK_WITH_VALGRIND
		  cleanup_vpi_call_args(argc, argv);
#endif
		  ଫେରନ୍ତୁ 0;
	    }
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiSysFunc:
	    ଯଦି (val_code == 0 && fnet == 0) {
		  ଯଦି (func_as_task_err) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
			cleanup_vpi_call_args(argc, argv);
#endif
			ଫେରନ୍ତୁ 0;
		  } ଅନ୍ୟଥା ଯଦି (func_as_task_warn) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)defn->info.type);
	    ପ୍ରତିଜ୍ଞା(0);
      }

      ସଂରଚନା __vpiSysTaskCall*obj = 0;

      ବାଛ (defn->info.type) {
	  ସ୍ଥିତି vpiSysTask:
	    obj = ନୂତନ systask_def;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiSysFunc:
	    ଯଦି (fnet && val_code == -vpiRealVal) {
		  obj = ନୂତନ sysfunc_rnet;

	    } ଅନ୍ୟଥା ଯଦି (fnet && val_code == -vpiVectorVal) {
		  obj = ନୂତନ sysfunc_4net(return_width);

	    } ଅନ୍ୟଥା ଯଦି (val_code == -vpiRealVal) {
		  obj = ନୂତନ sysfunc_real;

	    } ଅନ୍ୟଥା ଯଦି (val_code == -vpiVectorVal) {
		  obj = ନୂତନ sysfunc_vec4(return_width);

	    } ଅନ୍ୟଥା ଯଦି (val_code == -vpiStringVal) {
		  obj = ନୂତନ sysfunc_str;

           } ଅନ୍ୟଥା ଯଦି (val_code == 0 && fnet == 0) {
		  obj = ନୂତନ sysfunc_no;

	    } ଅନ୍ୟଥା {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_code);
		  ପ୍ରତିଜ୍ଞା(0);
	    }
	    ଭାଙ୍ଗ;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->args  = argv;
      obj->vec4_ଗଦା = vec4_ଗଦା;
      obj->real_ଗଦା = real_ଗଦା;
      obj->string_ଗଦା = string_ଗଦା;
      obj->fnet  = fnet;
      obj->file_idx  = (unsigned) file_idx;
      obj->lineno   = (unsigned) lineno;
      obj->userdata  = 0;
      obj->put_ମୂଲ୍ୟ = ମିଥ୍ୟା;

      compile_compiletf(obj);

      ଫେରନ୍ତୁ obj;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ vpi_call_delete(vpiHandle item)
{
      ସଂରଚନା __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(item);
	/* The object can be NULL if there was an error. */
      ଯଦି (!obj) ଫେରନ୍ତୁ;
      ପାଇଁ (unsigned arg = 0; arg < obj->nargs; arg += 1) {
	    ବାଛ (obj->args[arg]->get_type_code()) {
		ସ୍ଥିତି vpiConstant:
		  ବାଛ (vpi_get(_vpiFromThr, obj->args[arg])) {
		      ସ୍ଥିତି _vpiNoThr:
			constant_delete(obj->args[arg]);
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି _vpiString:
			thread_string_delete(obj->args[arg]);
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି _vpiVThr:
			thread_vthr_delete(obj->args[arg]);
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି _vpiWord:
			thread_word_delete(obj->args[arg]);
			ଭାଙ୍ଗ;
		      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ପ୍ରତିଜ୍ଞା(0);
		  }
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiMemoryWord:
		  ବାଛ (vpi_get(_vpiFromThr, obj->args[arg])) {
		      ସ୍ଥିତି _vpi_at_A:
			A_delete(obj->args[arg]);
			ଭାଙ୍ଗ;
		  }
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiPartSelect:
		  ବାଛ (vpi_get(_vpiFromThr, obj->args[arg])) {
		      ସ୍ଥିତି _vpi_at_PV:
			PV_delete(obj->args[arg]);
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି _vpi_at_APV:
			APV_delete(obj->args[arg]);
			ଭାଙ୍ଗ;
		  }
		  ଭାଙ୍ଗ;
	    }
      }
      ମୁକ୍ତ(obj->args);
      delete obj;
}
#endif

/*
 * This function is used by the %vpi_call instruction to actually
 * place the call to the system task/function. For now, only support
 * calls to system tasks.
 */

vthread_t vpip_current_vthread;

ଶୂନ୍ୟ vpip_execute_vpi_call(vthread_t thr, vpiHandle ref)
{
      vpip_current_vthread = thr;

      vpip_cur_task = dynamic_cast<__vpiSysTaskCall*>(ref);

      ଯଦି (vpip_cur_task->defn->info.calltf) {
	    ପ୍ରତିଜ୍ଞା(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = VPI_MODE_CALLTF;
	    vpip_cur_task->put_ମୂଲ୍ୟ = ମିଥ୍ୟା;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_data);
	    vpi_mode_flag = VPI_MODE_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    ଯଦି (ref->get_type_code() == vpiSysFuncCall &&
	        !vpip_cur_task->put_ମୂଲ୍ୟ) {
		  s_vpi_ମୂଲ୍ୟ val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_ମୂଲ୍ୟ(ref, &val, 0, vpiNoDelay);
	    }
      }
      ଯଦି (vpip_cur_task->vec4_ଗଦା > 0)
	    vthread_କାଢ଼_vec4(thr, vpip_cur_task->vec4_ଗଦା);
      ଯଦି (vpip_cur_task->real_ଗଦା > 0)
	    vthread_କାଢ଼_real(thr, vpip_cur_task->real_ଗଦା);
      ଯଦି (vpip_cur_task->string_ଗଦା > 0)
	    vthread_କାଢ଼_str(thr, vpip_cur_task->string_ଗଦା);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      ଯଦି (ଅଚଳ sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_ଠେଲ(thr, func_real->return_ମୂଲ୍ୟ());
      }
      ଅନ୍ୟଥା ଯଦି (ଅଚଳ sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_ଠେଲ(thr, func_string->return_ମୂଲ୍ୟ());
      }
      ଅନ୍ୟଥା ଯଦି (ଅଚଳ sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
	    vthread_ଠେଲ(thr, func_vec4->return_ମୂଲ୍ୟ());
      }
      vpip_cur_task = 0;
}

/*
 * This is the entry function that a VPI module uses to hook a new
 * task/function into the simulator. The function creates a new
 * __vpi_userSystf to represent the definition for the calls that come
 * to pass later.
 */
vpiHandle vpi_register_systf(ଅଚଳ ସଂରଚନା t_vpi_systf_data*ss)
{
      ସଂରଚନା __vpiUserSystf*cur = allocate_def();
      ପ୍ରତିଜ୍ଞା(ss);
      ବାଛ (ss->type) {
	  ସ୍ଥିତି vpiSysTask:
	  ସ୍ଥିତି vpiSysFunc:
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported type %d.\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ss->type);
	    ପ୍ରତିଜ୍ଞା(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = ସତ୍ୟ;

      ଫେରନ୍ତୁ cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, ଶୂନ୍ୟ*data)
{
      ସଂରଚନା __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      ଯଦି (rfp == 0)
	    ଫେରନ୍ତୁ 0;

      rfp->userdata = data;
      ଫେରନ୍ତୁ 1;
}

ଶୂନ୍ୟ* vpi_get_userdata(vpiHandle ref)
{
      ସଂରଚନା __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ଫେରନ୍ତୁ rfp->userdata;
}
