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

অন্তঃরেখ __vpiUserSystf::__vpiUserSystf()
{ }

পূর্ণসংখ্যা __vpiUserSystf::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiUserSystf; }


স্থির vpiHandle systask_handle(পূর্ণসংখ্যা type, vpiHandle ref)
{
      কাঠামো __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      নির্বাচন (type) {
	  ক্ষেত্র vpiScope:
	    ফেরত rfp->scope;

	  ক্ষেত্র vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    প্রতিজ্ঞা(rfp->defn->is_user_defn);
	    ফেরত rfp->defn;

	  পূর্বনির্ধারিত:
	    ফেরত 0;
      };
}

স্থির পূর্ণসংখ্যা systask_get(পূর্ণসংখ্যা type, vpiHandle ref)
{
      কাঠামো __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      নির্বাচন (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  ক্ষেত্র vpiTimeUnit:
	    ফেরত rfp->scope->time_units;
	  ক্ষেত্র vpiTimePrecision:
	    ফেরত rfp->scope->time_precision;

	  ক্ষেত্র vpi_সারি_No:
	    ফেরত rfp->lineno;

	  ক্ষেত্র vpiUserDefn:
	    ফেরত rfp->defn->is_user_defn;

	  পূর্বনির্ধারিত:
	    ফেরত vpiUndefined;
      }
}

// support getting vpiSize for a system function call
স্থির পূর্ণসংখ্যা sysfunc_get(পূর্ণসংখ্যা type, vpiHandle ref)
{
      কাঠামো __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      নির্বাচন (type) {
	  ক্ষেত্র vpi_আকার:
	    প্রতিজ্ঞা(0); // This should be handled by derived classes
	    ফেরত 0;

	  ক্ষেত্র vpi_সারি_No:
	    ফেরত rfp->lineno;

	  ক্ষেত্র vpiUserDefn:
	    ফেরত rfp->defn->is_user_defn;

	  পূর্বনির্ধারিত:
	    ফেরত vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

স্থির অক্ষর *systask_get_str(পূর্ণসংখ্যা type, vpiHandle ref)
{
      ধ্রুবক কাঠামো __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      নির্বাচন (type) {
          ক্ষেত্র vpiFile:
            প্রতিজ্ঞা(rfp->file_idx < file_names.size());
            ফেরত simple_set_rbuf_str(file_names[rfp->file_idx]);

          ক্ষেত্র vpiName:
            ফেরত simple_set_rbuf_str(rfp->defn->info.tfname);
      }

      ফেরত 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
স্থির vpiHandle systask_iter(পূর্ণসংখ্যা, vpiHandle ref)
{
      কাঠামো __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      যদি (rfp->nargs == 0)
	    ফেরত 0;

      ফেরত vpip_make_iterator(rfp->nargs, rfp->args, মিথ্যা);
}

কাঠামো systask_def : সর্বজনীন __vpiSysTaskCall {
      আভাসী পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক   override { ফেরত vpiSysTaskCall; }
      আভাসী পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code)           override { ফেরত systask_get(code, এই); }
      আভাসী অক্ষর*vpi_get_str(পূর্ণসংখ্যা code)      override { ফেরত systask_get_str(code, এই); }
      আভাসী vpiHandle vpi_handle(পূর্ণসংখ্যা code)  override { ফেরত systask_handle(code, এই); }
      আভাসী vpiHandle vpi_iterate(পূর্ণসংখ্যা code) override { ফেরত systask_iter(code, এই); }
};

কাঠামো sysfunc_def : সর্বজনীন systask_def {
      আভাসী পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiSysFuncCall; }
      আভাসী পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override { ফেরত sysfunc_get(code, এই); }
};

কাঠামো sysfunc_real : সর্বজনীন sysfunc_def {
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণসংখ্যা flags) override;
      অন্তঃরেখ দ্বিগুণ return_মান() ধ্রুবক { ফেরত return_মান_2; }

    ব্যক্তিগত:
      দ্বিগুণ return_মান_2;
};

vpiHandle sysfunc_real::vpi_put_মান(p_vpi_মান vp, পূর্ণসংখ্যা)
{
      put_মান = সত্য;

      return_মান_2 = 0.0;
      নির্বাচন (vp->format) {
	  ক্ষেত্র vpiRealVal:
	    return_মান_2 = vp->value.real;
	    বিরতি;
	  পূর্বনির্ধারিত:
	    fprintf(stderr, "Unsupported format %d.\n", (পূর্ণসংখ্যা)vp->format);
	    প্রতিজ্ঞা(0);
      }

      ফেরত 0;
}

কাঠামো sysfunc_str : সর্বজনীন sysfunc_def {
      পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণসংখ্যা flags) override;
      অন্তঃরেখ ধ্রুবক std::string& return_মান() ধ্রুবক { ফেরত return_মান_2; };

    ব্যক্তিগত:
      std::string return_মান_2;
};

পূর্ণসংখ্যা sysfunc_str::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpi_আকার:
	    ফেরত return_মান_2.size();

	  ক্ষেত্র vpi_সারি_No:
	    ফেরত lineno;

	  ক্ষেত্র vpiUserDefn:
	    ফেরত defn->is_user_defn;

	  পূর্বনির্ধারিত:
	    ফেরত vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_মান(p_vpi_মান vp, পূর্ণসংখ্যা)
{
      put_মান = সত্য;

      নির্বাচন (vp->format) {
	  ক্ষেত্র vpiStringVal:
	    return_মান_2 = std::string(vp->value.str);
	    বিরতি;
	  পূর্বনির্ধারিত:
	    fprintf(stderr, "Unsupported format %d.\n", (পূর্ণসংখ্যা)vp->format);
	    প্রতিজ্ঞা(0);
      }

      ফেরত 0;
}

শ্রেণী sysfunc_vec4 : সর্বজনীন sysfunc_def {
    সর্বজনীন:
      explicit অন্তঃরেখ sysfunc_vec4(unsigned wid): return_মান_2(wid, BIT4_X) { }
      পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণসংখ্যা flags) override;
      অন্তঃরেখ ধ্রুবক vvp_vector4_t& return_মান() ধ্রুবক { ফেরত return_মান_2; }

    ব্যক্তিগত:
      vpiHandle put_মান_scalar(p_vpi_মান vp);
      vpiHandle put_মান_int(p_vpi_মান vp);
      vpiHandle put_মান_string(p_vpi_মান vp);
      vpiHandle put_মান_vector(p_vpi_মান vp);
      vpiHandle put_মান_time(p_vpi_মান vp);

      vvp_vector4_t return_মান_2;
};

পূর্ণসংখ্যা sysfunc_vec4::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpi_আকার:
	    ফেরত return_মান_2.size();

	  ক্ষেত্র vpi_সারি_No:
	    ফেরত lineno;

	  ক্ষেত্র vpiUserDefn:
	    ফেরত defn->is_user_defn;

	  পূর্বনির্ধারিত:
	    ফেরত vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_মান_scalar(ধ্রুবক p_vpi_মান vp)
{
      নির্বাচন (vp->value.scalar) {
	  ক্ষেত্র vpi0:
	    return_মান_2.set_bit(0, BIT4_0);
	    বিরতি;
	  ক্ষেত্র vpi1:
	    return_মান_2.set_bit(0, BIT4_1);
	    বিরতি;
	  ক্ষেত্র vpiX:
	    return_মান_2.set_bit(0, BIT4_X);
	    বিরতি;
	  ক্ষেত্র vpiZ:
	    return_মান_2.set_bit(0, BIT4_Z);
	    বিরতি;
	  পূর্বনির্ধারিত:
	    fprintf(stderr, "Unsupported value %d.\n", (পূর্ণসংখ্যা)vp->value.scalar);
	    প্রতিজ্ঞা(0);
      }
      ফেরত 0;
}

vpiHandle sysfunc_vec4::put_মান_int(p_vpi_মান vp)
{
      long tmp = vp->value.integer;
      unsigned width = return_মান_2.size();
      জন্য (unsigned idx = 0 ;  idx < width ;  idx += 1) {
	    return_মান_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      ফেরত 0;
}

vpiHandle sysfunc_vec4::put_মান_string(p_vpi_মান vp)
{
      size_t slen = strlen(vp->value.str);
      unsigned wid = return_মান_2.size();

      জন্য (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    unsigned cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    যদি (cidx >= slen) {
		  return_মান_2.set_bit(idx, BIT4_0);
		  চালু;
	    }

	      // Put the end of the string in the LSB of the vector
	    অক্ষর use_char = vp->value.str[slen - 1 - cidx];

	    যদি ((use_char >> (idx % 8)) & 1)
		  return_মান_2.set_bit(idx, BIT4_1);
	    অন্যথা
		  return_মান_2.set_bit(idx, BIT4_0);
      }
      ফেরত 0;
}

vpiHandle sysfunc_vec4::put_মান_vector(p_vpi_মান vp)
{
      unsigned width = return_মান_2.size();
      জন্য (unsigned idx = 0 ; idx < width ; idx += 1) {
	    unsigned word = idx / 32;
	    unsigned bidx = idx % 32;

	    unsigned long aval = vp->value.vector[word].aval >> bidx;
	    unsigned long bval = vp->value.vector[word].bval >> bidx;

	    পূর্ণসংখ্যা bit = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    নির্বাচন (bit) {
		ক্ষেত্র 0:
		  bit4 = BIT4_0;
		  বিরতি;
		ক্ষেত্র 1:
		  bit4 = BIT4_1;
		  বিরতি;
		ক্ষেত্র 2:
		  bit4 = BIT4_Z;
		  বিরতি;
		ক্ষেত্র 3:
		  bit4 = BIT4_X;
		  বিরতি;
		পূর্বনির্ধারিত:
		  প্রতিজ্ঞা(0);
		  bit4 = BIT4_X;
		  বিরতি;
	    }
	    return_মান_2.set_bit(idx, bit4);
      }
      ফেরত 0;
}

vpiHandle sysfunc_vec4::put_মান_time(p_vpi_মান vp)
{
      unsigned width = return_মান_2.size();
      long tmp = 0;
      জন্য (unsigned idx = 0 ; idx < width ; idx += 1) {
	    যদি (idx == 0)
		  tmp = vp->value.time->নিচের;
	    অন্যথা যদি (idx == 32)
		  tmp = vp->value.time->উপরের;
	    অন্যথা যদি (idx == 64)
		  tmp = 0;

	    return_মান_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      ফেরত 0;
}

vpiHandle sysfunc_vec4::vpi_put_মান(p_vpi_মান vp, পূর্ণসংখ্যা)
{
      put_মান = সত্য;

      নির্বাচন (vp->format) {
	  ক্ষেত্র vpiScalarVal:
	    ফেরত put_মান_scalar(vp);
	  ক্ষেত্র vpiIntVal:
	    ফেরত put_মান_int(vp);
	  ক্ষেত্র vpiStringVal:
	    ফেরত put_মান_string(vp);
	  ক্ষেত্র vpiVectorVal:
	    ফেরত put_মান_vector(vp);
	  ক্ষেত্র vpiTimeVal:
	    ফেরত put_মান_time(vp);
	  পূর্বনির্ধারিত:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (পূর্ণসংখ্যা)vp->format);
	    প্রতিজ্ঞা(0);
      }

      ফেরত 0;
}

কাঠামো sysfunc_4net : সর্বজনীন sysfunc_def {
      explicit অন্তঃরেখ sysfunc_4net(unsigned wid) : vwid_(wid) { }
      পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণসংখ্যা flags) override;

    ব্যক্তিগত:
      unsigned vwid_;
};

// support getting vpiSize for a system function call
পূর্ণসংখ্যা sysfunc_4net::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpi_আকার:
	    ফেরত vwid_;

	  ক্ষেত্র vpi_সারি_No:
	    ফেরত lineno;

	  ক্ষেত্র vpiUserDefn:
	    ফেরত defn->is_user_defn;

	  পূর্বনির্ধারিত:
	    ফেরত vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_মান(p_vpi_মান vp, পূর্ণসংখ্যা)
{
      put_মান = সত্য;

      vvp_vector4_t val (vwid_);

      নির্বাচন (vp->format) {

          ক্ষেত্র vpiScalarVal: {
	        নির্বাচন(vp->value.scalar) {
		      ক্ষেত্র vpi0:
			val.set_bit(0, BIT4_0);
                        বিরতি;
		      ক্ষেত্র vpi1:
                        val.set_bit(0, BIT4_1);
                        বিরতি;
		      ক্ষেত্র vpiX:
                        val.set_bit(0, BIT4_X);
                        বিরতি;
		      ক্ষেত্র vpiZ:
                        val.set_bit(0, BIT4_Z);
                        বিরতি;
		      পূর্বনির্ধারিত:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (পূর্ণসংখ্যা)vp->value.scalar);
                        প্রতিজ্ঞা(0);
                }
		বিরতি;
          }

	  ক্ষেত্র vpiIntVal: {
		long tmp = vp->value.integer;
		জন্য (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		বিরতি;
	  }

          ক্ষেত্র vpiTimeVal: {
                unsigned long tmp = vp->value.time->নিচের;
                জন্য (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      যদি (idx == 31)
                            tmp = vp->value.time->উপরের;
                      অন্যথা
                            tmp >>= 1;
                }
                বিরতি;
          }

	  ক্ষেত্র vpiVectorVal:

	    জন্য (unsigned wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  unsigned word = wdx / 32;
		  unsigned long aval = vp->value.vector[word].aval;
		  unsigned long bval = vp->value.vector[word].bval;

		  জন্য (unsigned idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			পূর্ণসংখ্যা bit = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			নির্বাচন (bit) {
			    ক্ষেত্র 0:
			      bit4 = BIT4_0;
			      বিরতি;
			    ক্ষেত্র 1:
			      bit4 = BIT4_1;
			      বিরতি;
			    ক্ষেত্র 2:
			      bit4 = BIT4_Z;
			      বিরতি;
			    ক্ষেত্র 3:
			      bit4 = BIT4_X;
			      বিরতি;
			    পূর্বনির্ধারিত:
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
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (পূর্ণসংখ্যা)vp->format, vwid_);
	    প্রতিজ্ঞা(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      ফেরত 0;
}

কাঠামো sysfunc_rnet : সর্বজনীন sysfunc_def {
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণসংখ্যা flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_মান(p_vpi_মান vp, পূর্ণসংখ্যা)
{
      put_মান = সত্য;

      দ্বিগুণ value;
      নির্বাচন (vp->format) {

	  ক্ষেত্র vpiRealVal:
	    value = vp->value.real;
	    বিরতি;

	  পূর্বনির্ধারিত:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (পূর্ণসংখ্যা)vp->format);
	    প্রতিজ্ঞা(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      ফেরত 0;
}

কাঠামো sysfunc_no : সর্বজনীন sysfunc_def {
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণসংখ্যা flags) override;
};

vpiHandle sysfunc_no::vpi_put_মান(p_vpi_মান, পূর্ণসংখ্যা)
{
      ফেরত 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
স্থির কাঠামো __vpiUserSystf**def_table = 0;
স্থির unsigned def_গুনতি = 0;

স্থির কাঠামো __vpiUserSystf* allocate_def(শূন্য)
{
      যদি (def_table == 0) {
	    def_table = static_cast<কাঠামো __vpiUserSystf**>
	                (malloc(আকার(কাঠামো __vpiUserSystf*)));

	    def_table[0] = নতুন __vpiUserSystf;

	    def_গুনতি = 1;
	    ফেরত def_table[0];
      }

      def_table = static_cast<কাঠামো __vpiUserSystf**>
                  (realloc(def_table,
                   (def_গুনতি+1)*আকার(কাঠামো __vpiUserSystf*)));

      def_table[def_গুনতি] = নতুন __vpiUserSystf;

      ফেরত def_table[def_গুনতি++];
}

#ifdef CHECK_WITH_VALGRIND
শূন্য def_table_delete(শূন্য)
{
      জন্য (unsigned idx = 0; idx < def_গুনতি; idx += 1) {
	    মুক্ত(const_cast<অক্ষর *>(def_table[idx]->info.tfname));
	    delete def_table[idx];
      }
      মুক্ত(def_table);
      def_table = 0;
      def_গুনতি = 0;
}
#endif

শ্রেণী __vpiSystfIterator : সর্বজনীন __vpiHandle {
    সর্বজনীন:
      explicit __vpiSystfIterator(unsigned idx);
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override;
      vpiHandle vpi_সূচক(পূর্ণসংখ্যা idx) override;
      free_object_fun_t free_object_fun(শূন্য) override;

      unsigned পরবর্তী_2;
};

স্থির vpiHandle systf_iterator_scan(vpiHandle ref, পূর্ণসংখ্যা)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      যদি (obj->পরবর্তী_2 >= def_গুনতি) {
	    vpi_free_object(ref);
	    ফেরত 0;
      }

      unsigned use_সূচক = obj->পরবর্তী_2;
      যখন (!def_table[use_সূচক]->is_user_defn) {
	    obj->পরবর্তী_2 += 1;
	    use_সূচক = obj->পরবর্তী_2;
	    যদি (obj->পরবর্তী_2 >= def_গুনতি) {
		  vpi_free_object(ref);
		  ফেরত 0;
	    }
      }
      obj->পরবর্তী_2 += 1;
      ফেরত def_table[use_সূচক];
}

স্থির পূর্ণসংখ্যা systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      delete obj;
      ফেরত 1;
}

অন্তঃরেখ __vpiSystfIterator::__vpiSystfIterator(unsigned idx)
{ পরবর্তী_2 = idx; }

পূর্ণসংখ্যা __vpiSystfIterator::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_সূচক(পূর্ণসংখ্যা idx)
{ ফেরত systf_iterator_scan(এই, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(শূন্য)
{ ফেরত &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(শূন্য)
{
	/* Check to see if there are any user defined functions. */
      যৌক্তিক have_user_defn = মিথ্যা;
      unsigned idx;
      জন্য (idx = 0; idx < def_গুনতি; idx += 1) {
	    যদি (def_table[idx]->is_user_defn) {
		  have_user_defn = সত্য;
		  বিরতি;
	    }
      }
      যদি (!have_user_defn) ফেরত 0;

      __vpiSystfIterator*res = নতুন __vpiSystfIterator(idx);
      res->পরবর্তী_2 = idx;
      ফেরত res;
}

কাঠামো __vpiUserSystf* vpip_find_systf(ধ্রুবক অক্ষর*name)
{
      জন্য (unsigned idx = 0 ;  idx < def_গুনতি ;  idx += 1)
	    যদি (strcmp(def_table[idx]->info.tfname, name) == 0)
		  ফেরত def_table[idx];

      ফেরত 0;
}

শূন্য vpip_make_systf_system_defined(vpiHandle ref)
{
      প্রতিজ্ঞা(ref);
      কাঠামো __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      প্রতিজ্ঞা(obj);
      obj->is_user_defn = মিথ্যা;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
গণনা vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
প্রকারলেখ কাঠামো vpi_call_error {
      vpi_call_error_type type;
      অক্ষর *name;
      long file_idx;
      long lineno;
} vpi_call_error_s, *vpi_call_error_p;

স্থির vpi_call_error_p vpi_call_error_lst = ফাঁকা;
স্থির unsigned vpi_call_error_অঙ্ক = 0;

স্থির শূন্য add_vpi_call_error(vpi_call_error_type type, ধ্রুবক অক্ষর *name,
                               long file_idx, long lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (realloc(vpi_call_error_lst,
                                    (vpi_call_error_অঙ্ক + 1) *
                                    আকার(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_অঙ্ক].type = type;
      vpi_call_error_lst[vpi_call_error_অঙ্ক].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_অঙ্ক].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_অঙ্ক].lineno = lineno;
      vpi_call_error_অঙ্ক += 1;
}

শূন্য print_vpi_call_errors()
{
      জন্য (unsigned idx = 0; idx < vpi_call_error_অঙ্ক; idx += 1) {
	    নির্বাচন (vpi_call_error_lst[idx].type) {
		ক্ষেত্র VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (পূর্ণসংখ্যা)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  বিরতি;
		ক্ষেত্র VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (পূর্ণসংখ্যা)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  বিরতি;
		ক্ষেত্র VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (পূর্ণসংখ্যা)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  বিরতি;
		ক্ষেত্র VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (পূর্ণসংখ্যা)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_names[vpi_call_error_lst[idx].file_idx],
                                 (পূর্ণসংখ্যা)vpi_call_error_lst[idx].lineno);
		  বিরতি;
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
	    কাঠামো __vpiSysTaskCall*obj = নতুন systask_def;
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
vpiHandle vpip_build_vpi_call(ধ্রুবক অক্ষর*name, পূর্ণসংখ্যা val_code, unsigned return_width,
			      vvp_net_t*fnet,
			      যৌক্তিক func_as_task_err, যৌক্তিক func_as_task_warn,
			      unsigned argc, vpiHandle*argv,
			      unsigned vec4_স্তূপ, unsigned real_স্তূপ, unsigned string_স্তূপ,
			      long file_idx, long lineno)
{
      প্রতিজ্ঞা(!(func_as_task_err && func_as_task_warn));

      কাঠামো __vpiUserSystf*defn = vpip_find_systf(name);
      যদি (defn == 0) {
	    add_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
	    cleanup_vpi_call_args(argc, argv);
#endif
	    ফেরত 0;
      }

      নির্বাচন (defn->info.type) {
	  ক্ষেত্র vpiSysTask:
	    যদি (val_code != 0 || fnet != 0) {
		  add_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#ifdef CHECK_WITH_VALGRIND
		  cleanup_vpi_call_args(argc, argv);
#endif
		  ফেরত 0;
	    }
	    বিরতি;

	  ক্ষেত্র vpiSysFunc:
	    যদি (val_code == 0 && fnet == 0) {
		  যদি (func_as_task_err) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
			cleanup_vpi_call_args(argc, argv);
#endif
			ফেরত 0;
		  } অন্যথা যদি (func_as_task_warn) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (পূর্ণসংখ্যা)defn->info.type);
	    প্রতিজ্ঞা(0);
      }

      কাঠামো __vpiSysTaskCall*obj = 0;

      নির্বাচন (defn->info.type) {
	  ক্ষেত্র vpiSysTask:
	    obj = নতুন systask_def;
	    বিরতি;

	  ক্ষেত্র vpiSysFunc:
	    যদি (fnet && val_code == -vpiRealVal) {
		  obj = নতুন sysfunc_rnet;

	    } অন্যথা যদি (fnet && val_code == -vpiVectorVal) {
		  obj = নতুন sysfunc_4net(return_width);

	    } অন্যথা যদি (val_code == -vpiRealVal) {
		  obj = নতুন sysfunc_real;

	    } অন্যথা যদি (val_code == -vpiVectorVal) {
		  obj = নতুন sysfunc_vec4(return_width);

	    } অন্যথা যদি (val_code == -vpiStringVal) {
		  obj = নতুন sysfunc_str;

           } অন্যথা যদি (val_code == 0 && fnet == 0) {
		  obj = নতুন sysfunc_no;

	    } অন্যথা {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_code);
		  প্রতিজ্ঞা(0);
	    }
	    বিরতি;
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
      obj->put_মান = মিথ্যা;

      compile_compiletf(obj);

      ফেরত obj;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য vpi_call_delete(vpiHandle item)
{
      কাঠামো __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(item);
	/* The object can be NULL if there was an error. */
      যদি (!obj) ফেরত;
      জন্য (unsigned arg = 0; arg < obj->nargs; arg += 1) {
	    নির্বাচন (obj->args[arg]->get_type_code()) {
		ক্ষেত্র vpiConstant:
		  নির্বাচন (vpi_get(_vpiFromThr, obj->args[arg])) {
		      ক্ষেত্র _vpiNoThr:
			constant_delete(obj->args[arg]);
			বিরতি;
		      ক্ষেত্র _vpiString:
			thread_string_delete(obj->args[arg]);
			বিরতি;
		      ক্ষেত্র _vpiVThr:
			thread_vthr_delete(obj->args[arg]);
			বিরতি;
		      ক্ষেত্র _vpiWord:
			thread_word_delete(obj->args[arg]);
			বিরতি;
		      পূর্বনির্ধারিত:
			প্রতিজ্ঞা(0);
		  }
		  বিরতি;
		ক্ষেত্র vpiMemoryWord:
		  নির্বাচন (vpi_get(_vpiFromThr, obj->args[arg])) {
		      ক্ষেত্র _vpi_at_A:
			A_delete(obj->args[arg]);
			বিরতি;
		  }
		  বিরতি;
		ক্ষেত্র vpiPartSelect:
		  নির্বাচন (vpi_get(_vpiFromThr, obj->args[arg])) {
		      ক্ষেত্র _vpi_at_PV:
			PV_delete(obj->args[arg]);
			বিরতি;
		      ক্ষেত্র _vpi_at_APV:
			APV_delete(obj->args[arg]);
			বিরতি;
		  }
		  বিরতি;
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
	    vpip_cur_task->put_মান = মিথ্যা;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_তথ্য);
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
      যদি (ধ্রুবক sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_ঠেলো(thr, func_real->return_মান());
      }
      অন্যথা যদি (ধ্রুবক sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_ঠেলো(thr, func_string->return_মান());
      }
      অন্যথা যদি (ধ্রুবক sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
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
vpiHandle vpi_register_systf(ধ্রুবক কাঠামো t_vpi_systf_তথ্য*ss)
{
      কাঠামো __vpiUserSystf*cur = allocate_def();
      প্রতিজ্ঞা(ss);
      নির্বাচন (ss->type) {
	  ক্ষেত্র vpiSysTask:
	  ক্ষেত্র vpiSysFunc:
	    বিরতি;
	  পূর্বনির্ধারিত:
	    fprintf(stderr, "Unsupported type %d.\n", (পূর্ণসংখ্যা)ss->type);
	    প্রতিজ্ঞা(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = সত্য;

      ফেরত cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, শূন্য*তথ্য)
{
      কাঠামো __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      যদি (rfp == 0)
	    ফেরত 0;

      rfp->userdata = তথ্য;
      ফেরত 1;
}

শূন্য* vpi_get_userdata(vpiHandle ref)
{
      কাঠামো __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      প্রতিজ্ঞা(rfp);

      ফেরত rfp->userdata;
}
