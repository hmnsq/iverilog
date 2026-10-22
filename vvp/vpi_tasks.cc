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

using നാമാകാശ std;

വരിയിൽ __vpiUserSystf::__vpiUserSystf()
{ }

പൂർണ്ണസംഖ്യ __vpiUserSystf::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiUserSystf; }


സ്ഥിതം vpiHandle systask_handle(പൂർണ്ണസംഖ്യ type, vpiHandle ref)
{
      ഘടന __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      തിരഞ്ഞെടുപ്പ് (type) {
	  അവസ്ഥ vpiScope:
	    തിരികെ rfp->scope;

	  അവസ്ഥ vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    പ്രതിജ്ഞാ(rfp->defn->is_user_defn);
	    തിരികെ rfp->defn;

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      };
}

സ്ഥിതം പൂർണ്ണസംഖ്യ systask_get(പൂർണ്ണസംഖ്യ type, vpiHandle ref)
{
      ഘടന __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      തിരഞ്ഞെടുപ്പ് (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  അവസ്ഥ vpiTimeUnit:
	    തിരികെ rfp->scope->time_units;
	  അവസ്ഥ vpiTimePrecision:
	    തിരികെ rfp->scope->time_precision;

	  അവസ്ഥ vpi_വരി_No:
	    തിരികെ rfp->lineno;

	  അവസ്ഥ vpiUserDefn:
	    തിരികെ rfp->defn->is_user_defn;

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ vpiUndefined;
      }
}

// support getting vpiSize for a system function call
സ്ഥിതം പൂർണ്ണസംഖ്യ sysfunc_get(പൂർണ്ണസംഖ്യ type, vpiHandle ref)
{
      ഘടന __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      തിരഞ്ഞെടുപ്പ് (type) {
	  അവസ്ഥ vpiSize:
	    പ്രതിജ്ഞാ(0); // This should be handled by derived classes
	    തിരികെ 0;

	  അവസ്ഥ vpi_വരി_No:
	    തിരികെ rfp->lineno;

	  അവസ്ഥ vpiUserDefn:
	    തിരികെ rfp->defn->is_user_defn;

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

സ്ഥിതം അക്ഷരം *systask_get_str(പൂർണ്ണസംഖ്യ type, vpiHandle ref)
{
      സ്ഥിരം ഘടന __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      തിരഞ്ഞെടുപ്പ് (type) {
          അവസ്ഥ vpiFile:
            പ്രതിജ്ഞാ(rfp->file_idx < file_names.size());
            തിരികെ simple_set_rbuf_str(file_names[rfp->file_idx]);

          അവസ്ഥ vpiName:
            തിരികെ simple_set_rbuf_str(rfp->defn->info.tfname);
      }

      തിരികെ 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
സ്ഥിതം vpiHandle systask_iter(പൂർണ്ണസംഖ്യ, vpiHandle ref)
{
      ഘടന __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      എങ്കിൽ (rfp->nargs == 0)
	    തിരികെ 0;

      തിരികെ vpip_make_iterator(rfp->nargs, rfp->args, അസത്യം);
}

ഘടന systask_def : പൊതു __vpiSysTaskCall {
      മായാ പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം   override { തിരികെ vpiSysTaskCall; }
      മായാ പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code)           override { തിരികെ systask_get(code, ഇത്); }
      മായാ അക്ഷരം*vpi_get_str(പൂർണ്ണസംഖ്യ code)      override { തിരികെ systask_get_str(code, ഇത്); }
      മായാ vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code)  override { തിരികെ systask_handle(code, ഇത്); }
      മായാ vpiHandle vpi_iterate(പൂർണ്ണസംഖ്യ code) override { തിരികെ systask_iter(code, ഇത്); }
};

ഘടന sysfunc_def : പൊതു systask_def {
      മായാ പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiSysFuncCall; }
      മായാ പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override { തിരികെ sysfunc_get(code, ഇത്); }
};

ഘടന sysfunc_real : പൊതു sysfunc_def {
      vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags) override;
      വരിയിൽ ഇരട്ട return_മൂല്യം() സ്ഥിരം { തിരികെ return_മൂല്യം_2; }

    സ്വകാര്യ:
      ഇരട്ട return_മൂല്യം_2;
};

vpiHandle sysfunc_real::vpi_put_മൂല്യം(p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ)
{
      put_മൂല്യം = സത്യം;

      return_മൂല്യം_2 = 0.0;
      തിരഞ്ഞെടുപ്പ് (vp->format) {
	  അവസ്ഥ vpiRealVal:
	    return_മൂല്യം_2 = vp->value.real;
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Unsupported format %d.\n", (പൂർണ്ണസംഖ്യ)vp->format);
	    പ്രതിജ്ഞാ(0);
      }

      തിരികെ 0;
}

ഘടന sysfunc_str : പൊതു sysfunc_def {
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags) override;
      വരിയിൽ സ്ഥിരം std::string& return_മൂല്യം() സ്ഥിരം { തിരികെ return_മൂല്യം_2; };

    സ്വകാര്യ:
      std::string return_മൂല്യം_2;
};

പൂർണ്ണസംഖ്യ sysfunc_str::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiSize:
	    തിരികെ return_മൂല്യം_2.size();

	  അവസ്ഥ vpi_വരി_No:
	    തിരികെ lineno;

	  അവസ്ഥ vpiUserDefn:
	    തിരികെ defn->is_user_defn;

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_മൂല്യം(p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ)
{
      put_മൂല്യം = സത്യം;

      തിരഞ്ഞെടുപ്പ് (vp->format) {
	  അവസ്ഥ vpiStringVal:
	    return_മൂല്യം_2 = std::string(vp->value.str);
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Unsupported format %d.\n", (പൂർണ്ണസംഖ്യ)vp->format);
	    പ്രതിജ്ഞാ(0);
      }

      തിരികെ 0;
}

വർഗ്ഗം sysfunc_vec4 : പൊതു sysfunc_def {
    പൊതു:
      explicit വരിയിൽ sysfunc_vec4(unsigned wid): return_മൂല്യം_2(wid, BIT4_X) { }
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags) override;
      വരിയിൽ സ്ഥിരം vvp_vector4_t& return_മൂല്യം() സ്ഥിരം { തിരികെ return_മൂല്യം_2; }

    സ്വകാര്യ:
      vpiHandle put_മൂല്യം_scalar(p_vpi_മൂല്യം vp);
      vpiHandle put_മൂല്യം_int(p_vpi_മൂല്യം vp);
      vpiHandle put_മൂല്യം_string(p_vpi_മൂല്യം vp);
      vpiHandle put_മൂല്യം_vector(p_vpi_മൂല്യം vp);
      vpiHandle put_മൂല്യം_time(p_vpi_മൂല്യം vp);

      vvp_vector4_t return_മൂല്യം_2;
};

പൂർണ്ണസംഖ്യ sysfunc_vec4::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiSize:
	    തിരികെ return_മൂല്യം_2.size();

	  അവസ്ഥ vpi_വരി_No:
	    തിരികെ lineno;

	  അവസ്ഥ vpiUserDefn:
	    തിരികെ defn->is_user_defn;

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_മൂല്യം_scalar(സ്ഥിരം p_vpi_മൂല്യം vp)
{
      തിരഞ്ഞെടുപ്പ് (vp->value.scalar) {
	  അവസ്ഥ vpi0:
	    return_മൂല്യം_2.set_bit(0, BIT4_0);
	    നിർത്തുക;
	  അവസ്ഥ vpi1:
	    return_മൂല്യം_2.set_bit(0, BIT4_1);
	    നിർത്തുക;
	  അവസ്ഥ vpiX:
	    return_മൂല്യം_2.set_bit(0, BIT4_X);
	    നിർത്തുക;
	  അവസ്ഥ vpiZ:
	    return_മൂല്യം_2.set_bit(0, BIT4_Z);
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Unsupported value %d.\n", (പൂർണ്ണസംഖ്യ)vp->value.scalar);
	    പ്രതിജ്ഞാ(0);
      }
      തിരികെ 0;
}

vpiHandle sysfunc_vec4::put_മൂല്യം_int(p_vpi_മൂല്യം vp)
{
      long tmp = vp->value.integer;
      unsigned width = return_മൂല്യം_2.size();
      വേണ്ടി (unsigned idx = 0 ;  idx < width ;  idx += 1) {
	    return_മൂല്യം_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      തിരികെ 0;
}

vpiHandle sysfunc_vec4::put_മൂല്യം_string(p_vpi_മൂല്യം vp)
{
      size_t slen = strlen(vp->value.str);
      unsigned wid = return_മൂല്യം_2.size();

      വേണ്ടി (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    unsigned cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    എങ്കിൽ (cidx >= slen) {
		  return_മൂല്യം_2.set_bit(idx, BIT4_0);
		  തുടരുക;
	    }

	      // Put the end of the string in the LSB of the vector
	    അക്ഷരം use_char = vp->value.str[slen - 1 - cidx];

	    എങ്കിൽ ((use_char >> (idx % 8)) & 1)
		  return_മൂല്യം_2.set_bit(idx, BIT4_1);
	    അല്ലെങ്കിൽ
		  return_മൂല്യം_2.set_bit(idx, BIT4_0);
      }
      തിരികെ 0;
}

vpiHandle sysfunc_vec4::put_മൂല്യം_vector(p_vpi_മൂല്യം vp)
{
      unsigned width = return_മൂല്യം_2.size();
      വേണ്ടി (unsigned idx = 0 ; idx < width ; idx += 1) {
	    unsigned word = idx / 32;
	    unsigned bidx = idx % 32;

	    unsigned long aval = vp->value.vector[word].aval >> bidx;
	    unsigned long bval = vp->value.vector[word].bval >> bidx;

	    പൂർണ്ണസംഖ്യ bit = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    തിരഞ്ഞെടുപ്പ് (bit) {
		അവസ്ഥ 0:
		  bit4 = BIT4_0;
		  നിർത്തുക;
		അവസ്ഥ 1:
		  bit4 = BIT4_1;
		  നിർത്തുക;
		അവസ്ഥ 2:
		  bit4 = BIT4_Z;
		  നിർത്തുക;
		അവസ്ഥ 3:
		  bit4 = BIT4_X;
		  നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		  പ്രതിജ്ഞാ(0);
		  bit4 = BIT4_X;
		  നിർത്തുക;
	    }
	    return_മൂല്യം_2.set_bit(idx, bit4);
      }
      തിരികെ 0;
}

vpiHandle sysfunc_vec4::put_മൂല്യം_time(p_vpi_മൂല്യം vp)
{
      unsigned width = return_മൂല്യം_2.size();
      long tmp = 0;
      വേണ്ടി (unsigned idx = 0 ; idx < width ; idx += 1) {
	    എങ്കിൽ (idx == 0)
		  tmp = vp->value.time->താഴെയുള്ള;
	    അല്ലെങ്കിൽ എങ്കിൽ (idx == 32)
		  tmp = vp->value.time->മുകളിലെ;
	    അല്ലെങ്കിൽ എങ്കിൽ (idx == 64)
		  tmp = 0;

	    return_മൂല്യം_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      തിരികെ 0;
}

vpiHandle sysfunc_vec4::vpi_put_മൂല്യം(p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ)
{
      put_മൂല്യം = സത്യം;

      തിരഞ്ഞെടുപ്പ് (vp->format) {
	  അവസ്ഥ vpiScalarVal:
	    തിരികെ put_മൂല്യം_scalar(vp);
	  അവസ്ഥ vpiIntVal:
	    തിരികെ put_മൂല്യം_int(vp);
	  അവസ്ഥ vpiStringVal:
	    തിരികെ put_മൂല്യം_string(vp);
	  അവസ്ഥ vpiVectorVal:
	    തിരികെ put_മൂല്യം_vector(vp);
	  അവസ്ഥ vpiTimeVal:
	    തിരികെ put_മൂല്യം_time(vp);
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (പൂർണ്ണസംഖ്യ)vp->format);
	    പ്രതിജ്ഞാ(0);
      }

      തിരികെ 0;
}

ഘടന sysfunc_4net : പൊതു sysfunc_def {
      explicit വരിയിൽ sysfunc_4net(unsigned wid) : vwid_(wid) { }
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags) override;

    സ്വകാര്യ:
      unsigned vwid_;
};

// support getting vpiSize for a system function call
പൂർണ്ണസംഖ്യ sysfunc_4net::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiSize:
	    തിരികെ vwid_;

	  അവസ്ഥ vpi_വരി_No:
	    തിരികെ lineno;

	  അവസ്ഥ vpiUserDefn:
	    തിരികെ defn->is_user_defn;

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_മൂല്യം(p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ)
{
      put_മൂല്യം = സത്യം;

      vvp_vector4_t val (vwid_);

      തിരഞ്ഞെടുപ്പ് (vp->format) {

          അവസ്ഥ vpiScalarVal: {
	        തിരഞ്ഞെടുപ്പ്(vp->value.scalar) {
		      അവസ്ഥ vpi0:
			val.set_bit(0, BIT4_0);
                        നിർത്തുക;
		      അവസ്ഥ vpi1:
                        val.set_bit(0, BIT4_1);
                        നിർത്തുക;
		      അവസ്ഥ vpiX:
                        val.set_bit(0, BIT4_X);
                        നിർത്തുക;
		      അവസ്ഥ vpiZ:
                        val.set_bit(0, BIT4_Z);
                        നിർത്തുക;
		      സ്ഥിരസ്ഥിതി:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (പൂർണ്ണസംഖ്യ)vp->value.scalar);
                        പ്രതിജ്ഞാ(0);
                }
		നിർത്തുക;
          }

	  അവസ്ഥ vpiIntVal: {
		long tmp = vp->value.integer;
		വേണ്ടി (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		നിർത്തുക;
	  }

          അവസ്ഥ vpiTimeVal: {
                unsigned long tmp = vp->value.time->താഴെയുള്ള;
                വേണ്ടി (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      എങ്കിൽ (idx == 31)
                            tmp = vp->value.time->മുകളിലെ;
                      അല്ലെങ്കിൽ
                            tmp >>= 1;
                }
                നിർത്തുക;
          }

	  അവസ്ഥ vpiVectorVal:

	    വേണ്ടി (unsigned wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  unsigned word = wdx / 32;
		  unsigned long aval = vp->value.vector[word].aval;
		  unsigned long bval = vp->value.vector[word].bval;

		  വേണ്ടി (unsigned idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			പൂർണ്ണസംഖ്യ bit = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			തിരഞ്ഞെടുപ്പ് (bit) {
			    അവസ്ഥ 0:
			      bit4 = BIT4_0;
			      നിർത്തുക;
			    അവസ്ഥ 1:
			      bit4 = BIT4_1;
			      നിർത്തുക;
			    അവസ്ഥ 2:
			      bit4 = BIT4_Z;
			      നിർത്തുക;
			    അവസ്ഥ 3:
			      bit4 = BIT4_X;
			      നിർത്തുക;
			    സ്ഥിരസ്ഥിതി:
			      bit4 = BIT4_X;
			      fprintf(stderr, "Unsupported bit value %d.\n",
			              bit);
			      പ്രതിജ്ഞാ(0);
			}
			val.set_bit(wdx+idx, bit4);

			aval >>= 1;
			bval >>= 1;
		  }
	    }
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (പൂർണ്ണസംഖ്യ)vp->format, vwid_);
	    പ്രതിജ്ഞാ(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      തിരികെ 0;
}

ഘടന sysfunc_rnet : പൊതു sysfunc_def {
      vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_മൂല്യം(p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ)
{
      put_മൂല്യം = സത്യം;

      ഇരട്ട value;
      തിരഞ്ഞെടുപ്പ് (vp->format) {

	  അവസ്ഥ vpiRealVal:
	    value = vp->value.real;
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (പൂർണ്ണസംഖ്യ)vp->format);
	    പ്രതിജ്ഞാ(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      തിരികെ 0;
}

ഘടന sysfunc_no : പൊതു sysfunc_def {
      vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags) override;
};

vpiHandle sysfunc_no::vpi_put_മൂല്യം(p_vpi_മൂല്യം, പൂർണ്ണസംഖ്യ)
{
      തിരികെ 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
സ്ഥിതം ഘടന __vpiUserSystf**def_table = 0;
സ്ഥിതം unsigned def_എണ്ണം = 0;

സ്ഥിതം ഘടന __vpiUserSystf* allocate_def(ശൂന്യം)
{
      എങ്കിൽ (def_table == 0) {
	    def_table = static_cast<ഘടന __vpiUserSystf**>
	                (malloc(വലുപ്പം(ഘടന __vpiUserSystf*)));

	    def_table[0] = പുതിയ __vpiUserSystf;

	    def_എണ്ണം = 1;
	    തിരികെ def_table[0];
      }

      def_table = static_cast<ഘടന __vpiUserSystf**>
                  (realloc(def_table,
                   (def_എണ്ണം+1)*വലുപ്പം(ഘടന __vpiUserSystf*)));

      def_table[def_എണ്ണം] = പുതിയ __vpiUserSystf;

      തിരികെ def_table[def_എണ്ണം++];
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം def_table_delete(ശൂന്യം)
{
      വേണ്ടി (unsigned idx = 0; idx < def_എണ്ണം; idx += 1) {
	    സ്വതന്ത്രം(const_cast<അക്ഷരം *>(def_table[idx]->info.tfname));
	    delete def_table[idx];
      }
      സ്വതന്ത്രം(def_table);
      def_table = 0;
      def_എണ്ണം = 0;
}
#endif

വർഗ്ഗം __vpiSystfIterator : പൊതു __vpiHandle {
    പൊതു:
      explicit __vpiSystfIterator(unsigned idx);
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      vpiHandle vpi_index(പൂർണ്ണസംഖ്യ idx) override;
      free_object_fun_t free_object_fun(ശൂന്യം) override;

      unsigned next;
};

സ്ഥിതം vpiHandle systf_iterator_scan(vpiHandle ref, പൂർണ്ണസംഖ്യ)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      എങ്കിൽ (obj->next >= def_എണ്ണം) {
	    vpi_free_object(ref);
	    തിരികെ 0;
      }

      unsigned use_index = obj->next;
      ആകുമ്പോൾ (!def_table[use_index]->is_user_defn) {
	    obj->next += 1;
	    use_index = obj->next;
	    എങ്കിൽ (obj->next >= def_എണ്ണം) {
		  vpi_free_object(ref);
		  തിരികെ 0;
	    }
      }
      obj->next += 1;
      തിരികെ def_table[use_index];
}

സ്ഥിതം പൂർണ്ണസംഖ്യ systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      delete obj;
      തിരികെ 1;
}

വരിയിൽ __vpiSystfIterator::__vpiSystfIterator(unsigned idx)
{ next = idx; }

പൂർണ്ണസംഖ്യ __vpiSystfIterator::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_index(പൂർണ്ണസംഖ്യ idx)
{ തിരികെ systf_iterator_scan(ഇത്, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(ശൂന്യം)
{ തിരികെ &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(ശൂന്യം)
{
	/* Check to see if there are any user defined functions. */
      തർക്കം have_user_defn = അസത്യം;
      unsigned idx;
      വേണ്ടി (idx = 0; idx < def_എണ്ണം; idx += 1) {
	    എങ്കിൽ (def_table[idx]->is_user_defn) {
		  have_user_defn = സത്യം;
		  നിർത്തുക;
	    }
      }
      എങ്കിൽ (!have_user_defn) തിരികെ 0;

      __vpiSystfIterator*res = പുതിയ __vpiSystfIterator(idx);
      res->next = idx;
      തിരികെ res;
}

ഘടന __vpiUserSystf* vpip_find_systf(സ്ഥിരം അക്ഷരം*name)
{
      വേണ്ടി (unsigned idx = 0 ;  idx < def_എണ്ണം ;  idx += 1)
	    എങ്കിൽ (strcmp(def_table[idx]->info.tfname, name) == 0)
		  തിരികെ def_table[idx];

      തിരികെ 0;
}

ശൂന്യം vpip_make_systf_system_defined(vpiHandle ref)
{
      പ്രതിജ്ഞാ(ref);
      ഘടന __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      പ്രതിജ്ഞാ(obj);
      obj->is_user_defn = അസത്യം;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
എണ്ണൽ vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
തരനിർവചനം ഘടന vpi_call_error {
      vpi_call_error_type type;
      അക്ഷരം *name;
      long file_idx;
      long lineno;
} vpi_call_error_s, *vpi_call_error_p;

സ്ഥിതം vpi_call_error_p vpi_call_error_lst = ശൂന്യ;
സ്ഥിതം unsigned vpi_call_error_അക്കം = 0;

സ്ഥിതം ശൂന്യം add_vpi_call_error(vpi_call_error_type type, സ്ഥിരം അക്ഷരം *name,
                               long file_idx, long lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (realloc(vpi_call_error_lst,
                                    (vpi_call_error_അക്കം + 1) *
                                    വലുപ്പം(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_അക്കം].type = type;
      vpi_call_error_lst[vpi_call_error_അക്കം].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_അക്കം].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_അക്കം].lineno = lineno;
      vpi_call_error_അക്കം += 1;
}

ശൂന്യം print_vpi_call_errors()
{
      വേണ്ടി (unsigned idx = 0; idx < vpi_call_error_അക്കം; idx += 1) {
	    തിരഞ്ഞെടുപ്പ് (vpi_call_error_lst[idx].type) {
		അവസ്ഥ VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (പൂർണ്ണസംഖ്യ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  നിർത്തുക;
		അവസ്ഥ VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (പൂർണ്ണസംഖ്യ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  നിർത്തുക;
		അവസ്ഥ VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (പൂർണ്ണസംഖ്യ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  നിർത്തുക;
		അവസ്ഥ VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (പൂർണ്ണസംഖ്യ)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_names[vpi_call_error_lst[idx].file_idx],
                                 (പൂർണ്ണസംഖ്യ)vpi_call_error_lst[idx].lineno);
		  നിർത്തുക;
	    }
	    സ്വതന്ത്രം(vpi_call_error_lst[idx].name);
      }
      സ്വതന്ത്രം(vpi_call_error_lst);
      fflush(stderr);
}

#ifdef CHECK_WITH_VALGRIND
സ്ഥിതം ശൂന്യം cleanup_vpi_call_args(unsigned argc, vpiHandle*argv)
{
      എങ്കിൽ (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    ഘടന __vpiSysTaskCall*obj = പുതിയ systask_def;
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
vpiHandle vpip_build_vpi_call(സ്ഥിരം അക്ഷരം*name, പൂർണ്ണസംഖ്യ val_code, unsigned return_width,
			      vvp_net_t*fnet,
			      തർക്കം func_as_task_err, തർക്കം func_as_task_warn,
			      unsigned argc, vpiHandle*argv,
			      unsigned vec4_അട്ടി, unsigned real_അട്ടി, unsigned string_അട്ടി,
			      long file_idx, long lineno)
{
      പ്രതിജ്ഞാ(!(func_as_task_err && func_as_task_warn));

      ഘടന __vpiUserSystf*defn = vpip_find_systf(name);
      എങ്കിൽ (defn == 0) {
	    add_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
	    cleanup_vpi_call_args(argc, argv);
#endif
	    തിരികെ 0;
      }

      തിരഞ്ഞെടുപ്പ് (defn->info.type) {
	  അവസ്ഥ vpiSysTask:
	    എങ്കിൽ (val_code != 0 || fnet != 0) {
		  add_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#ifdef CHECK_WITH_VALGRIND
		  cleanup_vpi_call_args(argc, argv);
#endif
		  തിരികെ 0;
	    }
	    നിർത്തുക;

	  അവസ്ഥ vpiSysFunc:
	    എങ്കിൽ (val_code == 0 && fnet == 0) {
		  എങ്കിൽ (func_as_task_err) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
			cleanup_vpi_call_args(argc, argv);
#endif
			തിരികെ 0;
		  } അല്ലെങ്കിൽ എങ്കിൽ (func_as_task_warn) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (പൂർണ്ണസംഖ്യ)defn->info.type);
	    പ്രതിജ്ഞാ(0);
      }

      ഘടന __vpiSysTaskCall*obj = 0;

      തിരഞ്ഞെടുപ്പ് (defn->info.type) {
	  അവസ്ഥ vpiSysTask:
	    obj = പുതിയ systask_def;
	    നിർത്തുക;

	  അവസ്ഥ vpiSysFunc:
	    എങ്കിൽ (fnet && val_code == -vpiRealVal) {
		  obj = പുതിയ sysfunc_rnet;

	    } അല്ലെങ്കിൽ എങ്കിൽ (fnet && val_code == -vpiVectorVal) {
		  obj = പുതിയ sysfunc_4net(return_width);

	    } അല്ലെങ്കിൽ എങ്കിൽ (val_code == -vpiRealVal) {
		  obj = പുതിയ sysfunc_real;

	    } അല്ലെങ്കിൽ എങ്കിൽ (val_code == -vpiVectorVal) {
		  obj = പുതിയ sysfunc_vec4(return_width);

	    } അല്ലെങ്കിൽ എങ്കിൽ (val_code == -vpiStringVal) {
		  obj = പുതിയ sysfunc_str;

           } അല്ലെങ്കിൽ എങ്കിൽ (val_code == 0 && fnet == 0) {
		  obj = പുതിയ sysfunc_no;

	    } അല്ലെങ്കിൽ {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_code);
		  പ്രതിജ്ഞാ(0);
	    }
	    നിർത്തുക;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->args  = argv;
      obj->vec4_അട്ടി = vec4_അട്ടി;
      obj->real_അട്ടി = real_അട്ടി;
      obj->string_അട്ടി = string_അട്ടി;
      obj->fnet  = fnet;
      obj->file_idx  = (unsigned) file_idx;
      obj->lineno   = (unsigned) lineno;
      obj->userdata  = 0;
      obj->put_മൂല്യം = അസത്യം;

      compile_compiletf(obj);

      തിരികെ obj;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം vpi_call_delete(vpiHandle item)
{
      ഘടന __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(item);
	/* The object can be NULL if there was an error. */
      എങ്കിൽ (!obj) തിരികെ;
      വേണ്ടി (unsigned arg = 0; arg < obj->nargs; arg += 1) {
	    തിരഞ്ഞെടുപ്പ് (obj->args[arg]->get_type_code()) {
		അവസ്ഥ vpiConstant:
		  തിരഞ്ഞെടുപ്പ് (vpi_get(_vpiFromThr, obj->args[arg])) {
		      അവസ്ഥ _vpiNoThr:
			constant_delete(obj->args[arg]);
			നിർത്തുക;
		      അവസ്ഥ _vpiString:
			thread_string_delete(obj->args[arg]);
			നിർത്തുക;
		      അവസ്ഥ _vpiVThr:
			thread_vthr_delete(obj->args[arg]);
			നിർത്തുക;
		      അവസ്ഥ _vpiWord:
			thread_word_delete(obj->args[arg]);
			നിർത്തുക;
		      സ്ഥിരസ്ഥിതി:
			പ്രതിജ്ഞാ(0);
		  }
		  നിർത്തുക;
		അവസ്ഥ vpiMemoryWord:
		  തിരഞ്ഞെടുപ്പ് (vpi_get(_vpiFromThr, obj->args[arg])) {
		      അവസ്ഥ _vpi_at_A:
			A_delete(obj->args[arg]);
			നിർത്തുക;
		  }
		  നിർത്തുക;
		അവസ്ഥ vpiPartSelect:
		  തിരഞ്ഞെടുപ്പ് (vpi_get(_vpiFromThr, obj->args[arg])) {
		      അവസ്ഥ _vpi_at_PV:
			PV_delete(obj->args[arg]);
			നിർത്തുക;
		      അവസ്ഥ _vpi_at_APV:
			APV_delete(obj->args[arg]);
			നിർത്തുക;
		  }
		  നിർത്തുക;
	    }
      }
      സ്വതന്ത്രം(obj->args);
      delete obj;
}
#endif

/*
 * This function is used by the %vpi_call instruction to actually
 * place the call to the system task/function. For now, only support
 * calls to system tasks.
 */

vthread_t vpip_current_vthread;

ശൂന്യം vpip_execute_vpi_call(vthread_t thr, vpiHandle ref)
{
      vpip_current_vthread = thr;

      vpip_cur_task = dynamic_cast<__vpiSysTaskCall*>(ref);

      എങ്കിൽ (vpip_cur_task->defn->info.calltf) {
	    പ്രതിജ്ഞാ(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = VPI_MODE_CALLTF;
	    vpip_cur_task->put_മൂല്യം = അസത്യം;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_data);
	    vpi_mode_flag = VPI_MODE_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    എങ്കിൽ (ref->get_type_code() == vpiSysFuncCall &&
	        !vpip_cur_task->put_മൂല്യം) {
		  s_vpi_മൂല്യം val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_മൂല്യം(ref, &val, 0, vpiNoDelay);
	    }
      }
      എങ്കിൽ (vpip_cur_task->vec4_അട്ടി > 0)
	    vthread_എടുക്കുക_vec4(thr, vpip_cur_task->vec4_അട്ടി);
      എങ്കിൽ (vpip_cur_task->real_അട്ടി > 0)
	    vthread_എടുക്കുക_real(thr, vpip_cur_task->real_അട്ടി);
      എങ്കിൽ (vpip_cur_task->string_അട്ടി > 0)
	    vthread_എടുക്കുക_str(thr, vpip_cur_task->string_അട്ടി);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      എങ്കിൽ (സ്ഥിരം sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_തള്ളുക(thr, func_real->return_മൂല്യം());
      }
      അല്ലെങ്കിൽ എങ്കിൽ (സ്ഥിരം sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_തള്ളുക(thr, func_string->return_മൂല്യം());
      }
      അല്ലെങ്കിൽ എങ്കിൽ (സ്ഥിരം sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
	    vthread_തള്ളുക(thr, func_vec4->return_മൂല്യം());
      }
      vpip_cur_task = 0;
}

/*
 * This is the entry function that a VPI module uses to hook a new
 * task/function into the simulator. The function creates a new
 * __vpi_userSystf to represent the definition for the calls that come
 * to pass later.
 */
vpiHandle vpi_register_systf(സ്ഥിരം ഘടന t_vpi_systf_data*ss)
{
      ഘടന __vpiUserSystf*cur = allocate_def();
      പ്രതിജ്ഞാ(ss);
      തിരഞ്ഞെടുപ്പ് (ss->type) {
	  അവസ്ഥ vpiSysTask:
	  അവസ്ഥ vpiSysFunc:
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Unsupported type %d.\n", (പൂർണ്ണസംഖ്യ)ss->type);
	    പ്രതിജ്ഞാ(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = സത്യം;

      തിരികെ cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, ശൂന്യം*data)
{
      ഘടന __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      എങ്കിൽ (rfp == 0)
	    തിരികെ 0;

      rfp->userdata = data;
      തിരികെ 1;
}

ശൂന്യം* vpi_get_userdata(vpiHandle ref)
{
      ഘടന __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      പ്രതിജ്ഞാ(rfp);

      തിരികെ rfp->userdata;
}
