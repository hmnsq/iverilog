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

using নামাকাশ std;

অন্তঃস্থ __vpiUserSystf::__vpiUserSystf()
{ }

পূর্ণাঙ্ক __vpiUserSystf::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiUserSystf; }


স্থির vpiHandle systask_handle(পূর্ণাঙ্ক type, vpiHandle ref)
{
      সংরচনা __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      বিকল্প (type) {
	  পক্ষ vpiScope:
	    প্রত্যাগম rfp->scope;

	  পক্ষ vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    প্রতিজ্ঞা(rfp->defn->is_user_defn);
	    প্রত্যাগম rfp->defn;

	  স্বভাব:
	    প্রত্যাগম 0;
      };
}

স্থির পূর্ণাঙ্ক systask_get(পূর্ণাঙ্ক type, vpiHandle ref)
{
      সংরচনা __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      বিকল্প (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  পক্ষ vpiTimeUnit:
	    প্রত্যাগম rfp->scope->time_units;
	  পক্ষ vpiTimePrecision:
	    প্রত্যাগম rfp->scope->time_precision;

	  পক্ষ vpi_সারি_No:
	    প্রত্যাগম rfp->lineno;

	  পক্ষ vpiUserDefn:
	    প্রত্যাগম rfp->defn->is_user_defn;

	  স্বভাব:
	    প্রত্যাগম vpiUndefined;
      }
}

// support getting vpiSize for a system function call
স্থির পূর্ণাঙ্ক sysfunc_get(পূর্ণাঙ্ক type, vpiHandle ref)
{
      সংরচনা __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      বিকল্প (type) {
	  পক্ষ vpiSize:
	    প্রতিজ্ঞা(0); // This should be handled by derived classes
	    প্রত্যাগম 0;

	  পক্ষ vpi_সারি_No:
	    প্রত্যাগম rfp->lineno;

	  পক্ষ vpiUserDefn:
	    প্রত্যাগম rfp->defn->is_user_defn;

	  স্বভাব:
	    প্রত্যাগম vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

স্থির বর্ণ *systask_get_str(পূর্ণাঙ্ক type, vpiHandle ref)
{
      ধ্রুব সংরচনা __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      বিকল্প (type) {
          পক্ষ vpiFile:
            প্রতিজ্ঞা(rfp->file_idx < file_names.size());
            প্রত্যাগম simple_set_rbuf_str(file_names[rfp->file_idx]);

          পক্ষ vpiName:
            প্রত্যাগম simple_set_rbuf_str(rfp->defn->info.tfname);
      }

      প্রত্যাগম 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
স্থির vpiHandle systask_iter(পূর্ণাঙ্ক, vpiHandle ref)
{
      সংরচনা __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      যদি (rfp->nargs == 0)
	    প্রত্যাগম 0;

      প্রত্যাগম vpip_make_iterator(rfp->nargs, rfp->args, অসত্য);
}

সংরচনা systask_def : সার্বজনিক __vpiSysTaskCall {
      আভাস পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব   override { প্রত্যাগম vpiSysTaskCall; }
      আভাস পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code)           override { প্রত্যাগম systask_get(code, এতত্); }
      আভাস বর্ণ*vpi_get_str(পূর্ণাঙ্ক code)      override { প্রত্যাগম systask_get_str(code, এতত্); }
      আভাস vpiHandle vpi_handle(পূর্ণাঙ্ক code)  override { প্রত্যাগম systask_handle(code, এতত্); }
      আভাস vpiHandle vpi_iterate(পূর্ণাঙ্ক code) override { প্রত্যাগম systask_iter(code, এতত্); }
};

সংরচনা sysfunc_def : সার্বজনিক systask_def {
      আভাস পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override { প্রত্যাগম vpiSysFuncCall; }
      আভাস পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override { প্রত্যাগম sysfunc_get(code, এতত্); }
};

সংরচনা sysfunc_real : সার্বজনিক sysfunc_def {
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags) override;
      অন্তঃস্থ দ্বিগুণ return_মান() ধ্রুব { প্রত্যাগম return_মান_2; }

    গুপ্ত:
      দ্বিগুণ return_মান_2;
};

vpiHandle sysfunc_real::vpi_put_মান(p_vpi_মান vp, পূর্ণাঙ্ক)
{
      put_মান = সত্য;

      return_মান_2 = 0.0;
      বিকল্প (vp->format) {
	  পক্ষ vpiRealVal:
	    return_মান_2 = vp->value.real;
	    ভঙ্গ;
	  স্বভাব:
	    fprintf(stderr, "Unsupported format %d.\n", (পূর্ণাঙ্ক)vp->format);
	    প্রতিজ্ঞা(0);
      }

      প্রত্যাগম 0;
}

সংরচনা sysfunc_str : সার্বজনিক sysfunc_def {
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags) override;
      অন্তঃস্থ ধ্রুব std::string& return_মান() ধ্রুব { প্রত্যাগম return_মান_2; };

    গুপ্ত:
      std::string return_মান_2;
};

পূর্ণাঙ্ক sysfunc_str::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpiSize:
	    প্রত্যাগম return_মান_2.size();

	  পক্ষ vpi_সারি_No:
	    প্রত্যাগম lineno;

	  পক্ষ vpiUserDefn:
	    প্রত্যাগম defn->is_user_defn;

	  স্বভাব:
	    প্রত্যাগম vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_মান(p_vpi_মান vp, পূর্ণাঙ্ক)
{
      put_মান = সত্য;

      বিকল্প (vp->format) {
	  পক্ষ vpiStringVal:
	    return_মান_2 = std::string(vp->value.str);
	    ভঙ্গ;
	  স্বভাব:
	    fprintf(stderr, "Unsupported format %d.\n", (পূর্ণাঙ্ক)vp->format);
	    প্রতিজ্ঞা(0);
      }

      প্রত্যাগম 0;
}

বর্গ sysfunc_vec4 : সার্বজনিক sysfunc_def {
    সার্বজনিক:
      explicit অন্তঃস্থ sysfunc_vec4(unsigned wid): return_মান_2(wid, BIT4_X) { }
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags) override;
      অন্তঃস্থ ধ্রুব vvp_vector4_t& return_মান() ধ্রুব { প্রত্যাগম return_মান_2; }

    গুপ্ত:
      vpiHandle put_মান_scalar(p_vpi_মান vp);
      vpiHandle put_মান_int(p_vpi_মান vp);
      vpiHandle put_মান_string(p_vpi_মান vp);
      vpiHandle put_মান_vector(p_vpi_মান vp);
      vpiHandle put_মান_time(p_vpi_মান vp);

      vvp_vector4_t return_মান_2;
};

পূর্ণাঙ্ক sysfunc_vec4::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpiSize:
	    প্রত্যাগম return_মান_2.size();

	  পক্ষ vpi_সারি_No:
	    প্রত্যাগম lineno;

	  পক্ষ vpiUserDefn:
	    প্রত্যাগম defn->is_user_defn;

	  স্বভাব:
	    প্রত্যাগম vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_মান_scalar(ধ্রুব p_vpi_মান vp)
{
      বিকল্প (vp->value.scalar) {
	  পক্ষ vpi0:
	    return_মান_2.set_bit(0, BIT4_0);
	    ভঙ্গ;
	  পক্ষ vpi1:
	    return_মান_2.set_bit(0, BIT4_1);
	    ভঙ্গ;
	  পক্ষ vpiX:
	    return_মান_2.set_bit(0, BIT4_X);
	    ভঙ্গ;
	  পক্ষ vpiZ:
	    return_মান_2.set_bit(0, BIT4_Z);
	    ভঙ্গ;
	  স্বভাব:
	    fprintf(stderr, "Unsupported value %d.\n", (পূর্ণাঙ্ক)vp->value.scalar);
	    প্রতিজ্ঞা(0);
      }
      প্রত্যাগম 0;
}

vpiHandle sysfunc_vec4::put_মান_int(p_vpi_মান vp)
{
      long tmp = vp->value.integer;
      unsigned width = return_মান_2.size();
      প্রতি (unsigned idx = 0 ;  idx < width ;  idx += 1) {
	    return_মান_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      প্রত্যাগম 0;
}

vpiHandle sysfunc_vec4::put_মান_string(p_vpi_মান vp)
{
      size_t slen = strlen(vp->value.str);
      unsigned wid = return_মান_2.size();

      প্রতি (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    unsigned cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    যদি (cidx >= slen) {
		  return_মান_2.set_bit(idx, BIT4_0);
		  অনুবর্ত;
	    }

	      // Put the end of the string in the LSB of the vector
	    বর্ণ use_char = vp->value.str[slen - 1 - cidx];

	    যদি ((use_char >> (idx % 8)) & 1)
		  return_মান_2.set_bit(idx, BIT4_1);
	    অন্যথা
		  return_মান_2.set_bit(idx, BIT4_0);
      }
      প্রত্যাগম 0;
}

vpiHandle sysfunc_vec4::put_মান_vector(p_vpi_মান vp)
{
      unsigned width = return_মান_2.size();
      প্রতি (unsigned idx = 0 ; idx < width ; idx += 1) {
	    unsigned word = idx / 32;
	    unsigned bidx = idx % 32;

	    unsigned long aval = vp->value.vector[word].aval >> bidx;
	    unsigned long bval = vp->value.vector[word].bval >> bidx;

	    পূর্ণাঙ্ক bit = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    বিকল্প (bit) {
		পক্ষ 0:
		  bit4 = BIT4_0;
		  ভঙ্গ;
		পক্ষ 1:
		  bit4 = BIT4_1;
		  ভঙ্গ;
		পক্ষ 2:
		  bit4 = BIT4_Z;
		  ভঙ্গ;
		পক্ষ 3:
		  bit4 = BIT4_X;
		  ভঙ্গ;
		স্বভাব:
		  প্রতিজ্ঞা(0);
		  bit4 = BIT4_X;
		  ভঙ্গ;
	    }
	    return_মান_2.set_bit(idx, bit4);
      }
      প্রত্যাগম 0;
}

vpiHandle sysfunc_vec4::put_মান_time(p_vpi_মান vp)
{
      unsigned width = return_মান_2.size();
      long tmp = 0;
      প্রতি (unsigned idx = 0 ; idx < width ; idx += 1) {
	    যদি (idx == 0)
		  tmp = vp->value.time->নিচের;
	    অন্যথা যদি (idx == 32)
		  tmp = vp->value.time->উপরের;
	    অন্যথা যদি (idx == 64)
		  tmp = 0;

	    return_মান_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      প্রত্যাগম 0;
}

vpiHandle sysfunc_vec4::vpi_put_মান(p_vpi_মান vp, পূর্ণাঙ্ক)
{
      put_মান = সত্য;

      বিকল্প (vp->format) {
	  পক্ষ vpiScalarVal:
	    প্রত্যাগম put_মান_scalar(vp);
	  পক্ষ vpiIntVal:
	    প্রত্যাগম put_মান_int(vp);
	  পক্ষ vpiStringVal:
	    প্রত্যাগম put_মান_string(vp);
	  পক্ষ vpiVectorVal:
	    প্রত্যাগম put_মান_vector(vp);
	  পক্ষ vpiTimeVal:
	    প্রত্যাগম put_মান_time(vp);
	  স্বভাব:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (পূর্ণাঙ্ক)vp->format);
	    প্রতিজ্ঞা(0);
      }

      প্রত্যাগম 0;
}

সংরচনা sysfunc_4net : সার্বজনিক sysfunc_def {
      explicit অন্তঃস্থ sysfunc_4net(unsigned wid) : vwid_(wid) { }
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags) override;

    গুপ্ত:
      unsigned vwid_;
};

// support getting vpiSize for a system function call
পূর্ণাঙ্ক sysfunc_4net::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpiSize:
	    প্রত্যাগম vwid_;

	  পক্ষ vpi_সারি_No:
	    প্রত্যাগম lineno;

	  পক্ষ vpiUserDefn:
	    প্রত্যাগম defn->is_user_defn;

	  স্বভাব:
	    প্রত্যাগম vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_মান(p_vpi_মান vp, পূর্ণাঙ্ক)
{
      put_মান = সত্য;

      vvp_vector4_t val (vwid_);

      বিকল্প (vp->format) {

          পক্ষ vpiScalarVal: {
	        বিকল্প(vp->value.scalar) {
		      পক্ষ vpi0:
			val.set_bit(0, BIT4_0);
                        ভঙ্গ;
		      পক্ষ vpi1:
                        val.set_bit(0, BIT4_1);
                        ভঙ্গ;
		      পক্ষ vpiX:
                        val.set_bit(0, BIT4_X);
                        ভঙ্গ;
		      পক্ষ vpiZ:
                        val.set_bit(0, BIT4_Z);
                        ভঙ্গ;
		      স্বভাব:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (পূর্ণাঙ্ক)vp->value.scalar);
                        প্রতিজ্ঞা(0);
                }
		ভঙ্গ;
          }

	  পক্ষ vpiIntVal: {
		long tmp = vp->value.integer;
		প্রতি (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		ভঙ্গ;
	  }

          পক্ষ vpiTimeVal: {
                unsigned long tmp = vp->value.time->নিচের;
                প্রতি (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      যদি (idx == 31)
                            tmp = vp->value.time->উপরের;
                      অন্যথা
                            tmp >>= 1;
                }
                ভঙ্গ;
          }

	  পক্ষ vpiVectorVal:

	    প্রতি (unsigned wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  unsigned word = wdx / 32;
		  unsigned long aval = vp->value.vector[word].aval;
		  unsigned long bval = vp->value.vector[word].bval;

		  প্রতি (unsigned idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			পূর্ণাঙ্ক bit = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			বিকল্প (bit) {
			    পক্ষ 0:
			      bit4 = BIT4_0;
			      ভঙ্গ;
			    পক্ষ 1:
			      bit4 = BIT4_1;
			      ভঙ্গ;
			    পক্ষ 2:
			      bit4 = BIT4_Z;
			      ভঙ্গ;
			    পক্ষ 3:
			      bit4 = BIT4_X;
			      ভঙ্গ;
			    স্বভাব:
			      bit4 = BIT4_X;
			      fprintf(stderr, "Unsupported bit value %d.\n",
			              bit);
			      প্রতিজ্ঞা(0);
			}
			val.set_bit(wdx+idx, bit4);

			aval >>= 1;
			bval >>= 1;
		  }
	    }
	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (পূর্ণাঙ্ক)vp->format, vwid_);
	    প্রতিজ্ঞা(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      প্রত্যাগম 0;
}

সংরচনা sysfunc_rnet : সার্বজনিক sysfunc_def {
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_মান(p_vpi_মান vp, পূর্ণাঙ্ক)
{
      put_মান = সত্য;

      দ্বিগুণ value;
      বিকল্প (vp->format) {

	  পক্ষ vpiRealVal:
	    value = vp->value.real;
	    ভঙ্গ;

	  স্বভাব:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (পূর্ণাঙ্ক)vp->format);
	    প্রতিজ্ঞা(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      প্রত্যাগম 0;
}

সংরচনা sysfunc_no : সার্বজনিক sysfunc_def {
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags) override;
};

vpiHandle sysfunc_no::vpi_put_মান(p_vpi_মান, পূর্ণাঙ্ক)
{
      প্রত্যাগম 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
স্থির সংরচনা __vpiUserSystf**def_table = 0;
স্থির unsigned def_গুনতি = 0;

স্থির সংরচনা __vpiUserSystf* allocate_def(শূন্য)
{
      যদি (def_table == 0) {
	    def_table = static_cast<সংরচনা __vpiUserSystf**>
	                (malloc(পরিমাণ(সংরচনা __vpiUserSystf*)));

	    def_table[0] = নূতন __vpiUserSystf;

	    def_গুনতি = 1;
	    প্রত্যাগম def_table[0];
      }

      def_table = static_cast<সংরচনা __vpiUserSystf**>
                  (realloc(def_table,
                   (def_গুনতি+1)*পরিমাণ(সংরচনা __vpiUserSystf*)));

      def_table[def_গুনতি] = নূতন __vpiUserSystf;

      প্রত্যাগম def_table[def_গুনতি++];
}

#ifdef CHECK_WITH_VALGRIND
শূন্য def_table_delete(শূন্য)
{
      প্রতি (unsigned idx = 0; idx < def_গুনতি; idx += 1) {
	    মুক্ত(const_cast<বর্ণ *>(def_table[idx]->info.tfname));
	    delete def_table[idx];
      }
      মুক্ত(def_table);
      def_table = 0;
      def_গুনতি = 0;
}
#endif

বর্গ __vpiSystfIterator : সার্বজনিক __vpiHandle {
    সার্বজনিক:
      explicit __vpiSystfIterator(unsigned idx);
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      vpiHandle vpi_index(পূর্ণাঙ্ক idx) override;
      free_object_fun_t free_object_fun(শূন্য) override;

      unsigned next;
};

স্থির vpiHandle systf_iterator_scan(vpiHandle ref, পূর্ণাঙ্ক)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      যদি (obj->next >= def_গুনতি) {
	    vpi_free_object(ref);
	    প্রত্যাগম 0;
      }

      unsigned use_index = obj->next;
      যাবত্ (!def_table[use_index]->is_user_defn) {
	    obj->next += 1;
	    use_index = obj->next;
	    যদি (obj->next >= def_গুনতি) {
		  vpi_free_object(ref);
		  প্রত্যাগম 0;
	    }
      }
      obj->next += 1;
      প্রত্যাগম def_table[use_index];
}

স্থির পূর্ণাঙ্ক systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      delete obj;
      প্রত্যাগম 1;
}

অন্তঃস্থ __vpiSystfIterator::__vpiSystfIterator(unsigned idx)
{ next = idx; }

পূর্ণাঙ্ক __vpiSystfIterator::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_index(পূর্ণাঙ্ক idx)
{ প্রত্যাগম systf_iterator_scan(এতত্, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(শূন্য)
{ প্রত্যাগম &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(শূন্য)
{
	/* Check to see if there are any user defined functions. */
      তার্কিক have_user_defn = অসত্য;
      unsigned idx;
      প্রতি (idx = 0; idx < def_গুনতি; idx += 1) {
	    যদি (def_table[idx]->is_user_defn) {
		  have_user_defn = সত্য;
		  ভঙ্গ;
	    }
      }
      যদি (!have_user_defn) প্রত্যাগম 0;

      __vpiSystfIterator*res = নূতন __vpiSystfIterator(idx);
      res->next = idx;
      প্রত্যাগম res;
}

সংরচনা __vpiUserSystf* vpip_find_systf(ধ্রুব বর্ণ*name)
{
      প্রতি (unsigned idx = 0 ;  idx < def_গুনতি ;  idx += 1)
	    যদি (strcmp(def_table[idx]->info.tfname, name) == 0)
		  প্রত্যাগম def_table[idx];

      প্রত্যাগম 0;
}

শূন্য vpip_make_systf_system_defined(vpiHandle ref)
{
      প্রতিজ্ঞা(ref);
      সংরচনা __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      প্রতিজ্ঞা(obj);
      obj->is_user_defn = অসত্য;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
গণনা vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
প্রকারনির্দেশ সংরচনা vpi_call_error {
      vpi_call_error_type type;
      বর্ণ *name;
      long file_idx;
      long lineno;
} vpi_call_error_s, *vpi_call_error_p;

স্থির vpi_call_error_p vpi_call_error_lst = রিক্ত;
স্থির unsigned vpi_call_error_অঙ্ক = 0;

স্থির শূন্য add_vpi_call_error(vpi_call_error_type type, ধ্রুব বর্ণ *name,
                               long file_idx, long lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (realloc(vpi_call_error_lst,
                                    (vpi_call_error_অঙ্ক + 1) *
                                    পরিমাণ(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_অঙ্ক].type = type;
      vpi_call_error_lst[vpi_call_error_অঙ্ক].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_অঙ্ক].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_অঙ্ক].lineno = lineno;
      vpi_call_error_অঙ্ক += 1;
}

শূন্য print_vpi_call_errors()
{
      প্রতি (unsigned idx = 0; idx < vpi_call_error_অঙ্ক; idx += 1) {
	    বিকল্প (vpi_call_error_lst[idx].type) {
		পক্ষ VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (পূর্ণাঙ্ক)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ভঙ্গ;
		পক্ষ VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (পূর্ণাঙ্ক)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ভঙ্গ;
		পক্ষ VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (পূর্ণাঙ্ক)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  ভঙ্গ;
		পক্ষ VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (পূর্ণাঙ্ক)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_names[vpi_call_error_lst[idx].file_idx],
                                 (পূর্ণাঙ্ক)vpi_call_error_lst[idx].lineno);
		  ভঙ্গ;
	    }
	    মুক্ত(vpi_call_error_lst[idx].name);
      }
      মুক্ত(vpi_call_error_lst);
      fflush(stderr);
}

#ifdef CHECK_WITH_VALGRIND
স্থির শূন্য cleanup_vpi_call_args(unsigned argc, vpiHandle*argv)
{
      যদি (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    সংরচনা __vpiSysTaskCall*obj = নূতন systask_def;
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
vpiHandle vpip_build_vpi_call(ধ্রুব বর্ণ*name, পূর্ণাঙ্ক val_code, unsigned return_width,
			      vvp_net_t*fnet,
			      তার্কিক func_as_task_err, তার্কিক func_as_task_warn,
			      unsigned argc, vpiHandle*argv,
			      unsigned vec4_স্তূপ, unsigned real_স্তূপ, unsigned string_স্তূপ,
			      long file_idx, long lineno)
{
      প্রতিজ্ঞা(!(func_as_task_err && func_as_task_warn));

      সংরচনা __vpiUserSystf*defn = vpip_find_systf(name);
      যদি (defn == 0) {
	    add_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
	    cleanup_vpi_call_args(argc, argv);
#endif
	    প্রত্যাগম 0;
      }

      বিকল্প (defn->info.type) {
	  পক্ষ vpiSysTask:
	    যদি (val_code != 0 || fnet != 0) {
		  add_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#ifdef CHECK_WITH_VALGRIND
		  cleanup_vpi_call_args(argc, argv);
#endif
		  প্রত্যাগম 0;
	    }
	    ভঙ্গ;

	  পক্ষ vpiSysFunc:
	    যদি (val_code == 0 && fnet == 0) {
		  যদি (func_as_task_err) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
			cleanup_vpi_call_args(argc, argv);
#endif
			প্রত্যাগম 0;
		  } অন্যথা যদি (func_as_task_warn) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (পূর্ণাঙ্ক)defn->info.type);
	    প্রতিজ্ঞা(0);
      }

      সংরচনা __vpiSysTaskCall*obj = 0;

      বিকল্প (defn->info.type) {
	  পক্ষ vpiSysTask:
	    obj = নূতন systask_def;
	    ভঙ্গ;

	  পক্ষ vpiSysFunc:
	    যদি (fnet && val_code == -vpiRealVal) {
		  obj = নূতন sysfunc_rnet;

	    } অন্যথা যদি (fnet && val_code == -vpiVectorVal) {
		  obj = নূতন sysfunc_4net(return_width);

	    } অন্যথা যদি (val_code == -vpiRealVal) {
		  obj = নূতন sysfunc_real;

	    } অন্যথা যদি (val_code == -vpiVectorVal) {
		  obj = নূতন sysfunc_vec4(return_width);

	    } অন্যথা যদি (val_code == -vpiStringVal) {
		  obj = নূতন sysfunc_str;

           } অন্যথা যদি (val_code == 0 && fnet == 0) {
		  obj = নূতন sysfunc_no;

	    } অন্যথা {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_code);
		  প্রতিজ্ঞা(0);
	    }
	    ভঙ্গ;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->args  = argv;
      obj->vec4_স্তূপ = vec4_স্তূপ;
      obj->real_স্তূপ = real_স্তূপ;
      obj->string_স্তূপ = string_স্তূপ;
      obj->fnet  = fnet;
      obj->file_idx  = (unsigned) file_idx;
      obj->lineno   = (unsigned) lineno;
      obj->userdata  = 0;
      obj->put_মান = অসত্য;

      compile_compiletf(obj);

      প্রত্যাগম obj;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য vpi_call_delete(vpiHandle item)
{
      সংরচনা __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(item);
	/* The object can be NULL if there was an error. */
      যদি (!obj) প্রত্যাগম;
      প্রতি (unsigned arg = 0; arg < obj->nargs; arg += 1) {
	    বিকল্প (obj->args[arg]->get_type_code()) {
		পক্ষ vpiConstant:
		  বিকল্প (vpi_get(_vpiFromThr, obj->args[arg])) {
		      পক্ষ _vpiNoThr:
			constant_delete(obj->args[arg]);
			ভঙ্গ;
		      পক্ষ _vpiString:
			thread_string_delete(obj->args[arg]);
			ভঙ্গ;
		      পক্ষ _vpiVThr:
			thread_vthr_delete(obj->args[arg]);
			ভঙ্গ;
		      পক্ষ _vpiWord:
			thread_word_delete(obj->args[arg]);
			ভঙ্গ;
		      স্বভাব:
			প্রতিজ্ঞা(0);
		  }
		  ভঙ্গ;
		পক্ষ vpiMemoryWord:
		  বিকল্প (vpi_get(_vpiFromThr, obj->args[arg])) {
		      পক্ষ _vpi_at_A:
			A_delete(obj->args[arg]);
			ভঙ্গ;
		  }
		  ভঙ্গ;
		পক্ষ vpiPartSelect:
		  বিকল্প (vpi_get(_vpiFromThr, obj->args[arg])) {
		      পক্ষ _vpi_at_PV:
			PV_delete(obj->args[arg]);
			ভঙ্গ;
		      পক্ষ _vpi_at_APV:
			APV_delete(obj->args[arg]);
			ভঙ্গ;
		  }
		  ভঙ্গ;
	    }
      }
      মুক্ত(obj->args);
      delete obj;
}
#endif

/*
 * This function is used by the %vpi_call instruction to actually
 * place the call to the system task/function. For now, only support
 * calls to system tasks.
 */

vthread_t vpip_current_vthread;

শূন্য vpip_execute_vpi_call(vthread_t thr, vpiHandle ref)
{
      vpip_current_vthread = thr;

      vpip_cur_task = dynamic_cast<__vpiSysTaskCall*>(ref);

      যদি (vpip_cur_task->defn->info.calltf) {
	    প্রতিজ্ঞা(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = VPI_MODE_CALLTF;
	    vpip_cur_task->put_মান = অসত্য;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_data);
	    vpi_mode_flag = VPI_MODE_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    যদি (ref->get_type_code() == vpiSysFuncCall &&
	        !vpip_cur_task->put_মান) {
		  s_vpi_মান val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_মান(ref, &val, 0, vpiNoDelay);
	    }
      }
      যদি (vpip_cur_task->vec4_স্তূপ > 0)
	    vthread_বের_করো_vec4(thr, vpip_cur_task->vec4_স্তূপ);
      যদি (vpip_cur_task->real_স্তূপ > 0)
	    vthread_বের_করো_real(thr, vpip_cur_task->real_স্তূপ);
      যদি (vpip_cur_task->string_স্তূপ > 0)
	    vthread_বের_করো_str(thr, vpip_cur_task->string_স্তূপ);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      যদি (ধ্রুব sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_ঠেলো(thr, func_real->return_মান());
      }
      অন্যথা যদি (ধ্রুব sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_ঠেলো(thr, func_string->return_মান());
      }
      অন্যথা যদি (ধ্রুব sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
	    vthread_ঠেলো(thr, func_vec4->return_মান());
      }
      vpip_cur_task = 0;
}

/*
 * This is the entry function that a VPI module uses to hook a new
 * task/function into the simulator. The function creates a new
 * __vpi_userSystf to represent the definition for the calls that come
 * to pass later.
 */
vpiHandle vpi_register_systf(ধ্রুব সংরচনা t_vpi_systf_data*ss)
{
      সংরচনা __vpiUserSystf*cur = allocate_def();
      প্রতিজ্ঞা(ss);
      বিকল্প (ss->type) {
	  পক্ষ vpiSysTask:
	  পক্ষ vpiSysFunc:
	    ভঙ্গ;
	  স্বভাব:
	    fprintf(stderr, "Unsupported type %d.\n", (পূর্ণাঙ্ক)ss->type);
	    প্রতিজ্ঞা(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = সত্য;

      প্রত্যাগম cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, শূন্য*data)
{
      সংরচনা __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      যদি (rfp == 0)
	    প্রত্যাগম 0;

      rfp->userdata = data;
      প্রত্যাগম 1;
}

শূন্য* vpi_get_userdata(vpiHandle ref)
{
      সংরচনা __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      প্রতিজ্ঞা(rfp);

      প্রত্যাগম rfp->userdata;
}
