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

using نطاق std;

مضمن __vpiUserSystf::__vpiUserSystf()
{ }

صحيح __vpiUserSystf::get_type_code(فراغ) ثابت
{ ܦܢܐ vpiUserSystf; }


ساكن vpiHandle systask_handle(صحيح type, vpiHandle ref)
{
      بنية __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      بدل (type) {
	  حالة vpiScope:
	    ܦܢܐ rfp->scope;

	  حالة vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    أكد(rfp->defn->is_user_defn);
	    ܦܢܐ rfp->defn;

	  افتراضي:
	    ܦܢܐ 0;
      };
}

ساكن صحيح systask_get(صحيح type, vpiHandle ref)
{
      بنية __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      بدل (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  حالة vpiTimeUnit:
	    ܦܢܐ rfp->scope->time_units;
	  حالة vpiTimePrecision:
	    ܦܢܐ rfp->scope->time_precision;

	  حالة vpiLineNo:
	    ܦܢܐ rfp->lineno;

	  حالة vpiUserDefn:
	    ܦܢܐ rfp->defn->is_user_defn;

	  افتراضي:
	    ܦܢܐ vpiUndefined;
      }
}

// support getting vpiSize for a system function call
ساكن صحيح sysfunc_get(صحيح type, vpiHandle ref)
{
      بنية __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      بدل (type) {
	  حالة vpiSize:
	    أكد(0); // This should be handled by derived classes
	    ܦܢܐ 0;

	  حالة vpiLineNo:
	    ܦܢܐ rfp->lineno;

	  حالة vpiUserDefn:
	    ܦܢܐ rfp->defn->is_user_defn;

	  افتراضي:
	    ܦܢܐ vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

ساكن حرف *systask_get_str(صحيح type, vpiHandle ref)
{
      ثابت بنية __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      بدل (type) {
          حالة vpiFile:
            أكد(rfp->file_idx < file_names.size());
            ܦܢܐ simple_set_rbuf_str(file_names[rfp->file_idx]);

          حالة vpiName:
            ܦܢܐ simple_set_rbuf_str(rfp->defn->info.tfname);
      }

      ܦܢܐ 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
ساكن vpiHandle systask_iter(صحيح, vpiHandle ref)
{
      بنية __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      ܐܢ (rfp->nargs == 0)
	    ܦܢܐ 0;

      ܦܢܐ vpip_make_iterator(rfp->nargs, rfp->args, ܕܓܠܐ);
}

بنية systask_def : عام __vpiSysTaskCall {
      virtual صحيح get_type_code(فراغ) ثابت   override { ܦܢܐ vpiSysTaskCall; }
      virtual صحيح vpi_get(صحيح code)           override { ܦܢܐ systask_get(code, هذا); }
      virtual حرف*vpi_get_str(صحيح code)      override { ܦܢܐ systask_get_str(code, هذا); }
      virtual vpiHandle vpi_handle(صحيح code)  override { ܦܢܐ systask_handle(code, هذا); }
      virtual vpiHandle vpi_iterate(صحيح code) override { ܦܢܐ systask_iter(code, هذا); }
};

بنية sysfunc_def : عام systask_def {
      virtual صحيح get_type_code(فراغ) ثابت override { ܦܢܐ vpiSysFuncCall; }
      virtual صحيح vpi_get(صحيح code) override { ܦܢܐ sysfunc_get(code, هذا); }
};

بنية sysfunc_real : عام sysfunc_def {
      vpiHandle vpi_put_value(p_vpi_value val, صحيح flags) override;
      مضمن مزدوج return_value() ثابت { ܦܢܐ return_value_; }

    خاص:
      مزدوج return_value_;
};

vpiHandle sysfunc_real::vpi_put_value(p_vpi_value vp, صحيح)
{
      put_value = ܫܪܪܐ;

      return_value_ = 0.0;
      بدل (vp->format) {
	  حالة vpiRealVal:
	    return_value_ = vp->value.real;
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported format %d.\n", (صحيح)vp->format);
	    أكد(0);
      }

      ܦܢܐ 0;
}

بنية sysfunc_str : عام sysfunc_def {
      صحيح vpi_get(صحيح code) override;
      vpiHandle vpi_put_value(p_vpi_value val, صحيح flags) override;
      مضمن ثابت std::string& return_value() ثابت { ܦܢܐ return_value_; };

    خاص:
      std::string return_value_;
};

صحيح sysfunc_str::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiSize:
	    ܦܢܐ return_value_.size();

	  حالة vpiLineNo:
	    ܦܢܐ lineno;

	  حالة vpiUserDefn:
	    ܦܢܐ defn->is_user_defn;

	  افتراضي:
	    ܦܢܐ vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_value(p_vpi_value vp, صحيح)
{
      put_value = ܫܪܪܐ;

      بدل (vp->format) {
	  حالة vpiStringVal:
	    return_value_ = std::string(vp->value.str);
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported format %d.\n", (صحيح)vp->format);
	    أكد(0);
      }

      ܦܢܐ 0;
}

صنف sysfunc_vec4 : عام sysfunc_def {
    عام:
      explicit مضمن sysfunc_vec4(unsigned wid): return_value_(wid, BIT4_X) { }
      صحيح vpi_get(صحيح code) override;
      vpiHandle vpi_put_value(p_vpi_value val, صحيح flags) override;
      مضمن ثابت vvp_vector4_t& return_value() ثابت { ܦܢܐ return_value_; }

    خاص:
      vpiHandle put_value_scalar_(p_vpi_value vp);
      vpiHandle put_value_int_(p_vpi_value vp);
      vpiHandle put_value_string_(p_vpi_value vp);
      vpiHandle put_value_vector_(p_vpi_value vp);
      vpiHandle put_value_time_(p_vpi_value vp);

      vvp_vector4_t return_value_;
};

صحيح sysfunc_vec4::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiSize:
	    ܦܢܐ return_value_.size();

	  حالة vpiLineNo:
	    ܦܢܐ lineno;

	  حالة vpiUserDefn:
	    ܦܢܐ defn->is_user_defn;

	  افتراضي:
	    ܦܢܐ vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_value_scalar_(ثابت p_vpi_value vp)
{
      بدل (vp->value.scalar) {
	  حالة vpi0:
	    return_value_.set_bit(0, BIT4_0);
	    اكسر;
	  حالة vpi1:
	    return_value_.set_bit(0, BIT4_1);
	    اكسر;
	  حالة vpiX:
	    return_value_.set_bit(0, BIT4_X);
	    اكسر;
	  حالة vpiZ:
	    return_value_.set_bit(0, BIT4_Z);
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported value %d.\n", (صحيح)vp->value.scalar);
	    أكد(0);
      }
      ܦܢܐ 0;
}

vpiHandle sysfunc_vec4::put_value_int_(p_vpi_value vp)
{
      long tmp = vp->value.integer;
      unsigned width = return_value_.size();
      لكل (unsigned idx = 0 ;  idx < width ;  idx += 1) {
	    return_value_.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      ܦܢܐ 0;
}

vpiHandle sysfunc_vec4::put_value_string_(p_vpi_value vp)
{
      size_t slen = strlen(vp->value.str);
      unsigned wid = return_value_.size();

      لكل (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    unsigned cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    ܐܢ (cidx >= slen) {
		  return_value_.set_bit(idx, BIT4_0);
		  استمر;
	    }

	      // Put the end of the string in the LSB of the vector
	    حرف use_char = vp->value.str[slen - 1 - cidx];

	    ܐܢ ((use_char >> (idx % 8)) & 1)
		  return_value_.set_bit(idx, BIT4_1);
	    ܐܚܪܢܐ
		  return_value_.set_bit(idx, BIT4_0);
      }
      ܦܢܐ 0;
}

vpiHandle sysfunc_vec4::put_value_vector_(p_vpi_value vp)
{
      unsigned width = return_value_.size();
      لكل (unsigned idx = 0 ; idx < width ; idx += 1) {
	    unsigned word = idx / 32;
	    unsigned bidx = idx % 32;

	    unsigned long aval = vp->value.vector[word].aval >> bidx;
	    unsigned long bval = vp->value.vector[word].bval >> bidx;

	    صحيح bit = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    بدل (bit) {
		حالة 0:
		  bit4 = BIT4_0;
		  اكسر;
		حالة 1:
		  bit4 = BIT4_1;
		  اكسر;
		حالة 2:
		  bit4 = BIT4_Z;
		  اكسر;
		حالة 3:
		  bit4 = BIT4_X;
		  اكسر;
		افتراضي:
		  أكد(0);
		  bit4 = BIT4_X;
		  اكسر;
	    }
	    return_value_.set_bit(idx, bit4);
      }
      ܦܢܐ 0;
}

vpiHandle sysfunc_vec4::put_value_time_(p_vpi_value vp)
{
      unsigned width = return_value_.size();
      long tmp = 0;
      لكل (unsigned idx = 0 ; idx < width ; idx += 1) {
	    ܐܢ (idx == 0)
		  tmp = vp->value.time->low;
	    ܐܚܪܢܐ ܐܢ (idx == 32)
		  tmp = vp->value.time->high;
	    ܐܚܪܢܐ ܐܢ (idx == 64)
		  tmp = 0;

	    return_value_.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      ܦܢܐ 0;
}

vpiHandle sysfunc_vec4::vpi_put_value(p_vpi_value vp, صحيح)
{
      put_value = ܫܪܪܐ;

      بدل (vp->format) {
	  حالة vpiScalarVal:
	    ܦܢܐ put_value_scalar_(vp);
	  حالة vpiIntVal:
	    ܦܢܐ put_value_int_(vp);
	  حالة vpiStringVal:
	    ܦܢܐ put_value_string_(vp);
	  حالة vpiVectorVal:
	    ܦܢܐ put_value_vector_(vp);
	  حالة vpiTimeVal:
	    ܦܢܐ put_value_time_(vp);
	  افتراضي:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (صحيح)vp->format);
	    أكد(0);
      }

      ܦܢܐ 0;
}

بنية sysfunc_4net : عام sysfunc_def {
      explicit مضمن sysfunc_4net(unsigned wid) : vwid_(wid) { }
      صحيح vpi_get(صحيح code) override;
      vpiHandle vpi_put_value(p_vpi_value val, صحيح flags) override;

    خاص:
      unsigned vwid_;
};

// support getting vpiSize for a system function call
صحيح sysfunc_4net::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiSize:
	    ܦܢܐ vwid_;

	  حالة vpiLineNo:
	    ܦܢܐ lineno;

	  حالة vpiUserDefn:
	    ܦܢܐ defn->is_user_defn;

	  افتراضي:
	    ܦܢܐ vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_value(p_vpi_value vp, صحيح)
{
      put_value = ܫܪܪܐ;

      vvp_vector4_t val (vwid_);

      بدل (vp->format) {

          حالة vpiScalarVal: {
	        بدل(vp->value.scalar) {
		      حالة vpi0:
			val.set_bit(0, BIT4_0);
                        اكسر;
		      حالة vpi1:
                        val.set_bit(0, BIT4_1);
                        اكسر;
		      حالة vpiX:
                        val.set_bit(0, BIT4_X);
                        اكسر;
		      حالة vpiZ:
                        val.set_bit(0, BIT4_Z);
                        اكسر;
		      افتراضي:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (صحيح)vp->value.scalar);
                        أكد(0);
                }
		اكسر;
          }

	  حالة vpiIntVal: {
		long tmp = vp->value.integer;
		لكل (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		اكسر;
	  }

          حالة vpiTimeVal: {
                unsigned long tmp = vp->value.time->low;
                لكل (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      ܐܢ (idx == 31)
                            tmp = vp->value.time->high;
                      ܐܚܪܢܐ
                            tmp >>= 1;
                }
                اكسر;
          }

	  حالة vpiVectorVal:

	    لكل (unsigned wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  unsigned word = wdx / 32;
		  unsigned long aval = vp->value.vector[word].aval;
		  unsigned long bval = vp->value.vector[word].bval;

		  لكل (unsigned idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			صحيح bit = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			بدل (bit) {
			    حالة 0:
			      bit4 = BIT4_0;
			      اكسر;
			    حالة 1:
			      bit4 = BIT4_1;
			      اكسر;
			    حالة 2:
			      bit4 = BIT4_Z;
			      اكسر;
			    حالة 3:
			      bit4 = BIT4_X;
			      اكسر;
			    افتراضي:
			      bit4 = BIT4_X;
			      fprintf(stderr, "Unsupported bit value %d.\n",
			              bit);
			      أكد(0);
			}
			val.set_bit(wdx+idx, bit4);

			aval >>= 1;
			bval >>= 1;
		  }
	    }
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (صحيح)vp->format, vwid_);
	    أكد(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      ܦܢܐ 0;
}

بنية sysfunc_rnet : عام sysfunc_def {
      vpiHandle vpi_put_value(p_vpi_value val, صحيح flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_value(p_vpi_value vp, صحيح)
{
      put_value = ܫܪܪܐ;

      مزدوج value;
      بدل (vp->format) {

	  حالة vpiRealVal:
	    value = vp->value.real;
	    اكسر;

	  افتراضي:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (صحيح)vp->format);
	    أكد(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      ܦܢܐ 0;
}

بنية sysfunc_no : عام sysfunc_def {
      vpiHandle vpi_put_value(p_vpi_value val, صحيح flags) override;
};

vpiHandle sysfunc_no::vpi_put_value(p_vpi_value, صحيح)
{
      ܦܢܐ 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
ساكن بنية __vpiUserSystf**def_table = 0;
ساكن unsigned def_count = 0;

ساكن بنية __vpiUserSystf* allocate_def(فراغ)
{
      ܐܢ (def_table == 0) {
	    def_table = static_cast<بنية __vpiUserSystf**>
	                (malloc(حجم(بنية __vpiUserSystf*)));

	    def_table[0] = جديد __vpiUserSystf;

	    def_count = 1;
	    ܦܢܐ def_table[0];
      }

      def_table = static_cast<بنية __vpiUserSystf**>
                  (realloc(def_table,
                   (def_count+1)*حجم(بنية __vpiUserSystf*)));

      def_table[def_count] = جديد __vpiUserSystf;

      ܦܢܐ def_table[def_count++];
}

#ifdef CHECK_WITH_VALGRIND
فراغ def_table_delete(فراغ)
{
      لكل (unsigned idx = 0; idx < def_count; idx += 1) {
	    حرر(const_cast<حرف *>(def_table[idx]->info.tfname));
	    delete def_table[idx];
      }
      حرر(def_table);
      def_table = 0;
      def_count = 0;
}
#endif

صنف __vpiSystfIterator : عام __vpiHandle {
    عام:
      explicit __vpiSystfIterator(unsigned idx);
      صحيح get_type_code(فراغ) ثابت override;
      vpiHandle vpi_index(صحيح idx) override;
      free_object_fun_t free_object_fun(فراغ) override;

      unsigned next;
};

ساكن vpiHandle systf_iterator_scan(vpiHandle ref, صحيح)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      ܐܢ (obj->next >= def_count) {
	    vpi_free_object(ref);
	    ܦܢܐ 0;
      }

      unsigned use_index = obj->next;
      ܟܡܐ (!def_table[use_index]->is_user_defn) {
	    obj->next += 1;
	    use_index = obj->next;
	    ܐܢ (obj->next >= def_count) {
		  vpi_free_object(ref);
		  ܦܢܐ 0;
	    }
      }
      obj->next += 1;
      ܦܢܐ def_table[use_index];
}

ساكن صحيح systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      delete obj;
      ܦܢܐ 1;
}

مضمن __vpiSystfIterator::__vpiSystfIterator(unsigned idx)
{ next = idx; }

صحيح __vpiSystfIterator::get_type_code(فراغ) ثابت
{ ܦܢܐ vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_index(صحيح idx)
{ ܦܢܐ systf_iterator_scan(هذا, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(فراغ)
{ ܦܢܐ &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(فراغ)
{
	/* Check to see if there are any user defined functions. */
      منطقي have_user_defn = ܕܓܠܐ;
      unsigned idx;
      لكل (idx = 0; idx < def_count; idx += 1) {
	    ܐܢ (def_table[idx]->is_user_defn) {
		  have_user_defn = ܫܪܪܐ;
		  اكسر;
	    }
      }
      ܐܢ (!have_user_defn) ܦܢܐ 0;

      __vpiSystfIterator*res = جديد __vpiSystfIterator(idx);
      res->next = idx;
      ܦܢܐ res;
}

بنية __vpiUserSystf* vpip_find_systf(ثابت حرف*name)
{
      لكل (unsigned idx = 0 ;  idx < def_count ;  idx += 1)
	    ܐܢ (strcmp(def_table[idx]->info.tfname, name) == 0)
		  ܦܢܐ def_table[idx];

      ܦܢܐ 0;
}

فراغ vpip_make_systf_system_defined(vpiHandle ref)
{
      أكد(ref);
      بنية __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      أكد(obj);
      obj->is_user_defn = ܕܓܠܐ;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
تعداد vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
تعريفنوع بنية vpi_call_error {
      vpi_call_error_type type;
      حرف *name;
      long file_idx;
      long lineno;
} vpi_call_error_s, *vpi_call_error_p;

ساكن vpi_call_error_p vpi_call_error_lst = عدم;
ساكن unsigned vpi_call_error_num = 0;

ساكن فراغ add_vpi_call_error(vpi_call_error_type type, ثابت حرف *name,
                               long file_idx, long lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (realloc(vpi_call_error_lst,
                                    (vpi_call_error_num + 1) *
                                    حجم(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_num].type = type;
      vpi_call_error_lst[vpi_call_error_num].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_num].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_num].lineno = lineno;
      vpi_call_error_num += 1;
}

فراغ print_vpi_call_errors()
{
      لكل (unsigned idx = 0; idx < vpi_call_error_num; idx += 1) {
	    بدل (vpi_call_error_lst[idx].type) {
		حالة VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (صحيح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  اكسر;
		حالة VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (صحيح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  اكسر;
		حالة VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (صحيح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  اكسر;
		حالة VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (صحيح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_names[vpi_call_error_lst[idx].file_idx],
                                 (صحيح)vpi_call_error_lst[idx].lineno);
		  اكسر;
	    }
	    حرر(vpi_call_error_lst[idx].name);
      }
      حرر(vpi_call_error_lst);
      fflush(stderr);
}

#ifdef CHECK_WITH_VALGRIND
ساكن فراغ cleanup_vpi_call_args(unsigned argc, vpiHandle*argv)
{
      ܐܢ (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    بنية __vpiSysTaskCall*obj = جديد systask_def;
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
vpiHandle vpip_build_vpi_call(ثابت حرف*name, صحيح val_code, unsigned return_width,
			      vvp_net_t*fnet,
			      منطقي func_as_task_err, منطقي func_as_task_warn,
			      unsigned argc, vpiHandle*argv,
			      unsigned vec4_stack, unsigned real_stack, unsigned string_stack,
			      long file_idx, long lineno)
{
      أكد(!(func_as_task_err && func_as_task_warn));

      بنية __vpiUserSystf*defn = vpip_find_systf(name);
      ܐܢ (defn == 0) {
	    add_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
	    cleanup_vpi_call_args(argc, argv);
#endif
	    ܦܢܐ 0;
      }

      بدل (defn->info.type) {
	  حالة vpiSysTask:
	    ܐܢ (val_code != 0 || fnet != 0) {
		  add_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#ifdef CHECK_WITH_VALGRIND
		  cleanup_vpi_call_args(argc, argv);
#endif
		  ܦܢܐ 0;
	    }
	    اكسر;

	  حالة vpiSysFunc:
	    ܐܢ (val_code == 0 && fnet == 0) {
		  ܐܢ (func_as_task_err) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
			cleanup_vpi_call_args(argc, argv);
#endif
			ܦܢܐ 0;
		  } ܐܚܪܢܐ ܐܢ (func_as_task_warn) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (صحيح)defn->info.type);
	    أكد(0);
      }

      بنية __vpiSysTaskCall*obj = 0;

      بدل (defn->info.type) {
	  حالة vpiSysTask:
	    obj = جديد systask_def;
	    اكسر;

	  حالة vpiSysFunc:
	    ܐܢ (fnet && val_code == -vpiRealVal) {
		  obj = جديد sysfunc_rnet;

	    } ܐܚܪܢܐ ܐܢ (fnet && val_code == -vpiVectorVal) {
		  obj = جديد sysfunc_4net(return_width);

	    } ܐܚܪܢܐ ܐܢ (val_code == -vpiRealVal) {
		  obj = جديد sysfunc_real;

	    } ܐܚܪܢܐ ܐܢ (val_code == -vpiVectorVal) {
		  obj = جديد sysfunc_vec4(return_width);

	    } ܐܚܪܢܐ ܐܢ (val_code == -vpiStringVal) {
		  obj = جديد sysfunc_str;

           } ܐܚܪܢܐ ܐܢ (val_code == 0 && fnet == 0) {
		  obj = جديد sysfunc_no;

	    } ܐܚܪܢܐ {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_code);
		  أكد(0);
	    }
	    اكسر;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->args  = argv;
      obj->vec4_stack = vec4_stack;
      obj->real_stack = real_stack;
      obj->string_stack = string_stack;
      obj->fnet  = fnet;
      obj->file_idx  = (unsigned) file_idx;
      obj->lineno   = (unsigned) lineno;
      obj->userdata  = 0;
      obj->put_value = ܕܓܠܐ;

      compile_compiletf(obj);

      ܦܢܐ obj;
}

#ifdef CHECK_WITH_VALGRIND
فراغ vpi_call_delete(vpiHandle item)
{
      بنية __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(item);
	/* The object can be NULL if there was an error. */
      ܐܢ (!obj) ܦܢܐ;
      لكل (unsigned arg = 0; arg < obj->nargs; arg += 1) {
	    بدل (obj->args[arg]->get_type_code()) {
		حالة vpiConstant:
		  بدل (vpi_get(_vpiFromThr, obj->args[arg])) {
		      حالة _vpiNoThr:
			constant_delete(obj->args[arg]);
			اكسر;
		      حالة _vpiString:
			thread_string_delete(obj->args[arg]);
			اكسر;
		      حالة _vpiVThr:
			thread_vthr_delete(obj->args[arg]);
			اكسر;
		      حالة _vpiWord:
			thread_word_delete(obj->args[arg]);
			اكسر;
		      افتراضي:
			أكد(0);
		  }
		  اكسر;
		حالة vpiMemoryWord:
		  بدل (vpi_get(_vpiFromThr, obj->args[arg])) {
		      حالة _vpi_at_A:
			A_delete(obj->args[arg]);
			اكسر;
		  }
		  اكسر;
		حالة vpiPartSelect:
		  بدل (vpi_get(_vpiFromThr, obj->args[arg])) {
		      حالة _vpi_at_PV:
			PV_delete(obj->args[arg]);
			اكسر;
		      حالة _vpi_at_APV:
			APV_delete(obj->args[arg]);
			اكسر;
		  }
		  اكسر;
	    }
      }
      حرر(obj->args);
      delete obj;
}
#endif

/*
 * This function is used by the %vpi_call instruction to actually
 * place the call to the system task/function. For now, only support
 * calls to system tasks.
 */

vthread_t vpip_current_vthread;

فراغ vpip_execute_vpi_call(vthread_t thr, vpiHandle ref)
{
      vpip_current_vthread = thr;

      vpip_cur_task = dynamic_cast<__vpiSysTaskCall*>(ref);

      ܐܢ (vpip_cur_task->defn->info.calltf) {
	    أكد(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = VPI_MODE_CALLTF;
	    vpip_cur_task->put_value = ܕܓܠܐ;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_data);
	    vpi_mode_flag = VPI_MODE_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    ܐܢ (ref->get_type_code() == vpiSysFuncCall &&
	        !vpip_cur_task->put_value) {
		  s_vpi_value val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_value(ref, &val, 0, vpiNoDelay);
	    }
      }
      ܐܢ (vpip_cur_task->vec4_stack > 0)
	    vthread_pop_vec4(thr, vpip_cur_task->vec4_stack);
      ܐܢ (vpip_cur_task->real_stack > 0)
	    vthread_pop_real(thr, vpip_cur_task->real_stack);
      ܐܢ (vpip_cur_task->string_stack > 0)
	    vthread_pop_str(thr, vpip_cur_task->string_stack);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      ܐܢ (ثابت sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_push(thr, func_real->return_value());
      }
      ܐܚܪܢܐ ܐܢ (ثابت sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_push(thr, func_string->return_value());
      }
      ܐܚܪܢܐ ܐܢ (ثابت sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
	    vthread_push(thr, func_vec4->return_value());
      }
      vpip_cur_task = 0;
}

/*
 * This is the entry function that a VPI module uses to hook a new
 * task/function into the simulator. The function creates a new
 * __vpi_userSystf to represent the definition for the calls that come
 * to pass later.
 */
vpiHandle vpi_register_systf(ثابت بنية t_vpi_systf_data*ss)
{
      بنية __vpiUserSystf*cur = allocate_def();
      أكد(ss);
      بدل (ss->type) {
	  حالة vpiSysTask:
	  حالة vpiSysFunc:
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported type %d.\n", (صحيح)ss->type);
	    أكد(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = ܫܪܪܐ;

      ܦܢܐ cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, فراغ*data)
{
      بنية __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      ܐܢ (rfp == 0)
	    ܦܢܐ 0;

      rfp->userdata = data;
      ܦܢܐ 1;
}

فراغ* vpi_get_userdata(vpiHandle ref)
{
      بنية __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      أكد(rfp);

      ܦܢܐ rfp->userdata;
}
