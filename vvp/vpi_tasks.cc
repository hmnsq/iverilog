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

using فضاینام std;

درونخطی __vpiUserSystf::__vpiUserSystf()
{ }

صحیح __vpiUserSystf::get_type_code(تهی) ثابت
{ بازگشت vpiUserSystf; }


ایستا vpiHandle systask_handle(صحیح type, vpiHandle ref)
{
      ساختار __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      انتخاب (type) {
	  حالت vpiScope:
	    بازگشت rfp->scope;

	  حالت vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    أكد(rfp->defn->is_user_defn);
	    بازگشت rfp->defn;

	  پیشفرض:
	    بازگشت 0;
      };
}

ایستا صحیح systask_get(صحیح type, vpiHandle ref)
{
      ساختار __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      انتخاب (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  حالت vpiTimeUnit:
	    بازگشت rfp->scope->time_units;
	  حالت vpiTimePrecision:
	    بازگشت rfp->scope->time_precision;

	  حالت vpi_سطر_No:
	    بازگشت rfp->lineno;

	  حالت vpiUserDefn:
	    بازگشت rfp->defn->is_user_defn;

	  پیشفرض:
	    بازگشت vpiUndefined;
      }
}

// support getting vpiSize for a system function call
ایستا صحیح sysfunc_get(صحیح type, vpiHandle ref)
{
      ساختار __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      انتخاب (type) {
	  حالت vpiSize:
	    أكد(0); // This should be handled by derived classes
	    بازگشت 0;

	  حالت vpi_سطر_No:
	    بازگشت rfp->lineno;

	  حالت vpiUserDefn:
	    بازگشت rfp->defn->is_user_defn;

	  پیشفرض:
	    بازگشت vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

ایستا نویسه *systask_get_str(صحیح type, vpiHandle ref)
{
      ثابت ساختار __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      انتخاب (type) {
          حالت vpiFile:
            أكد(rfp->file_idx < file_names.size());
            بازگشت simple_set_rbuf_str(file_names[rfp->file_idx]);

          حالت vpiName:
            بازگشت simple_set_rbuf_str(rfp->defn->info.tfname);
      }

      بازگشت 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
ایستا vpiHandle systask_iter(صحیح, vpiHandle ref)
{
      ساختار __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      اگر (rfp->nargs == 0)
	    بازگشت 0;

      بازگشت vpip_make_iterator(rfp->nargs, rfp->args, نادرست);
}

ساختار systask_def : عمومی __vpiSysTaskCall {
      مجازی صحیح get_type_code(تهی) ثابت   override { بازگشت vpiSysTaskCall; }
      مجازی صحیح vpi_get(صحیح code)           override { بازگشت systask_get(code, این); }
      مجازی نویسه*vpi_get_str(صحیح code)      override { بازگشت systask_get_str(code, این); }
      مجازی vpiHandle vpi_handle(صحیح code)  override { بازگشت systask_handle(code, این); }
      مجازی vpiHandle vpi_iterate(صحیح code) override { بازگشت systask_iter(code, این); }
};

ساختار sysfunc_def : عمومی systask_def {
      مجازی صحیح get_type_code(تهی) ثابت override { بازگشت vpiSysFuncCall; }
      مجازی صحیح vpi_get(صحیح code) override { بازگشت sysfunc_get(code, این); }
};

ساختار sysfunc_real : عمومی sysfunc_def {
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags) override;
      درونخطی دوگانه return_مقدار() ثابت { بازگشت return_مقدار_2; }

    خصوصی:
      دوگانه return_مقدار_2;
};

vpiHandle sysfunc_real::vpi_put_مقدار(p_vpi_مقدار vp, صحیح)
{
      put_مقدار = درست;

      return_مقدار_2 = 0.0;
      انتخاب (vp->format) {
	  حالت vpiRealVal:
	    return_مقدار_2 = vp->value.real;
	    بشکن;
	  پیشفرض:
	    fprintf(stderr, "Unsupported format %d.\n", (صحیح)vp->format);
	    أكد(0);
      }

      بازگشت 0;
}

ساختار sysfunc_str : عمومی sysfunc_def {
      صحیح vpi_get(صحیح code) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags) override;
      درونخطی ثابت std::string& return_مقدار() ثابت { بازگشت return_مقدار_2; };

    خصوصی:
      std::string return_مقدار_2;
};

صحیح sysfunc_str::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiSize:
	    بازگشت return_مقدار_2.size();

	  حالت vpi_سطر_No:
	    بازگشت lineno;

	  حالت vpiUserDefn:
	    بازگشت defn->is_user_defn;

	  پیشفرض:
	    بازگشت vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_مقدار(p_vpi_مقدار vp, صحیح)
{
      put_مقدار = درست;

      انتخاب (vp->format) {
	  حالت vpiStringVal:
	    return_مقدار_2 = std::string(vp->value.str);
	    بشکن;
	  پیشفرض:
	    fprintf(stderr, "Unsupported format %d.\n", (صحیح)vp->format);
	    أكد(0);
      }

      بازگشت 0;
}

رده sysfunc_vec4 : عمومی sysfunc_def {
    عمومی:
      explicit درونخطی sysfunc_vec4(unsigned wid): return_مقدار_2(wid, BIT4_X) { }
      صحیح vpi_get(صحیح code) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags) override;
      درونخطی ثابت vvp_vector4_t& return_مقدار() ثابت { بازگشت return_مقدار_2; }

    خصوصی:
      vpiHandle put_مقدار_scalar(p_vpi_مقدار vp);
      vpiHandle put_مقدار_int(p_vpi_مقدار vp);
      vpiHandle put_مقدار_string(p_vpi_مقدار vp);
      vpiHandle put_مقدار_vector(p_vpi_مقدار vp);
      vpiHandle put_مقدار_time(p_vpi_مقدار vp);

      vvp_vector4_t return_مقدار_2;
};

صحیح sysfunc_vec4::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiSize:
	    بازگشت return_مقدار_2.size();

	  حالت vpi_سطر_No:
	    بازگشت lineno;

	  حالت vpiUserDefn:
	    بازگشت defn->is_user_defn;

	  پیشفرض:
	    بازگشت vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_مقدار_scalar(ثابت p_vpi_مقدار vp)
{
      انتخاب (vp->value.scalar) {
	  حالت vpi0:
	    return_مقدار_2.set_bit(0, BIT4_0);
	    بشکن;
	  حالت vpi1:
	    return_مقدار_2.set_bit(0, BIT4_1);
	    بشکن;
	  حالت vpiX:
	    return_مقدار_2.set_bit(0, BIT4_X);
	    بشکن;
	  حالت vpiZ:
	    return_مقدار_2.set_bit(0, BIT4_Z);
	    بشکن;
	  پیشفرض:
	    fprintf(stderr, "Unsupported value %d.\n", (صحیح)vp->value.scalar);
	    أكد(0);
      }
      بازگشت 0;
}

vpiHandle sysfunc_vec4::put_مقدار_int(p_vpi_مقدار vp)
{
      long tmp = vp->value.integer;
      unsigned width = return_مقدار_2.size();
      برای (unsigned idx = 0 ;  idx < width ;  idx += 1) {
	    return_مقدار_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      بازگشت 0;
}

vpiHandle sysfunc_vec4::put_مقدار_string(p_vpi_مقدار vp)
{
      size_t slen = strlen(vp->value.str);
      unsigned wid = return_مقدار_2.size();

      برای (unsigned idx = 0 ; idx < wid ; idx += 1) {
	    unsigned cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    اگر (cidx >= slen) {
		  return_مقدار_2.set_bit(idx, BIT4_0);
		  ادامه;
	    }

	      // Put the end of the string in the LSB of the vector
	    نویسه use_char = vp->value.str[slen - 1 - cidx];

	    اگر ((use_char >> (idx % 8)) & 1)
		  return_مقدار_2.set_bit(idx, BIT4_1);
	    وگرنه
		  return_مقدار_2.set_bit(idx, BIT4_0);
      }
      بازگشت 0;
}

vpiHandle sysfunc_vec4::put_مقدار_vector(p_vpi_مقدار vp)
{
      unsigned width = return_مقدار_2.size();
      برای (unsigned idx = 0 ; idx < width ; idx += 1) {
	    unsigned word = idx / 32;
	    unsigned bidx = idx % 32;

	    unsigned long aval = vp->value.vector[word].aval >> bidx;
	    unsigned long bval = vp->value.vector[word].bval >> bidx;

	    صحیح bit = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    انتخاب (bit) {
		حالت 0:
		  bit4 = BIT4_0;
		  بشکن;
		حالت 1:
		  bit4 = BIT4_1;
		  بشکن;
		حالت 2:
		  bit4 = BIT4_Z;
		  بشکن;
		حالت 3:
		  bit4 = BIT4_X;
		  بشکن;
		پیشفرض:
		  أكد(0);
		  bit4 = BIT4_X;
		  بشکن;
	    }
	    return_مقدار_2.set_bit(idx, bit4);
      }
      بازگشت 0;
}

vpiHandle sysfunc_vec4::put_مقدار_time(p_vpi_مقدار vp)
{
      unsigned width = return_مقدار_2.size();
      long tmp = 0;
      برای (unsigned idx = 0 ; idx < width ; idx += 1) {
	    اگر (idx == 0)
		  tmp = vp->value.time->کم;
	    وگرنه اگر (idx == 32)
		  tmp = vp->value.time->بالا;
	    وگرنه اگر (idx == 64)
		  tmp = 0;

	    return_مقدار_2.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      بازگشت 0;
}

vpiHandle sysfunc_vec4::vpi_put_مقدار(p_vpi_مقدار vp, صحیح)
{
      put_مقدار = درست;

      انتخاب (vp->format) {
	  حالت vpiScalarVal:
	    بازگشت put_مقدار_scalar(vp);
	  حالت vpiIntVal:
	    بازگشت put_مقدار_int(vp);
	  حالت vpiStringVal:
	    بازگشت put_مقدار_string(vp);
	  حالت vpiVectorVal:
	    بازگشت put_مقدار_vector(vp);
	  حالت vpiTimeVal:
	    بازگشت put_مقدار_time(vp);
	  پیشفرض:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (صحیح)vp->format);
	    أكد(0);
      }

      بازگشت 0;
}

ساختار sysfunc_4net : عمومی sysfunc_def {
      explicit درونخطی sysfunc_4net(unsigned wid) : vwid_(wid) { }
      صحیح vpi_get(صحیح code) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags) override;

    خصوصی:
      unsigned vwid_;
};

// support getting vpiSize for a system function call
صحیح sysfunc_4net::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiSize:
	    بازگشت vwid_;

	  حالت vpi_سطر_No:
	    بازگشت lineno;

	  حالت vpiUserDefn:
	    بازگشت defn->is_user_defn;

	  پیشفرض:
	    بازگشت vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_مقدار(p_vpi_مقدار vp, صحیح)
{
      put_مقدار = درست;

      vvp_vector4_t val (vwid_);

      انتخاب (vp->format) {

          حالت vpiScalarVal: {
	        انتخاب(vp->value.scalar) {
		      حالت vpi0:
			val.set_bit(0, BIT4_0);
                        بشکن;
		      حالت vpi1:
                        val.set_bit(0, BIT4_1);
                        بشکن;
		      حالت vpiX:
                        val.set_bit(0, BIT4_X);
                        بشکن;
		      حالت vpiZ:
                        val.set_bit(0, BIT4_Z);
                        بشکن;
		      پیشفرض:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (صحیح)vp->value.scalar);
                        أكد(0);
                }
		بشکن;
          }

	  حالت vpiIntVal: {
		long tmp = vp->value.integer;
		برای (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		بشکن;
	  }

          حالت vpiTimeVal: {
                unsigned long tmp = vp->value.time->کم;
                برای (unsigned idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.set_bit(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      اگر (idx == 31)
                            tmp = vp->value.time->بالا;
                      وگرنه
                            tmp >>= 1;
                }
                بشکن;
          }

	  حالت vpiVectorVal:

	    برای (unsigned wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  unsigned word = wdx / 32;
		  unsigned long aval = vp->value.vector[word].aval;
		  unsigned long bval = vp->value.vector[word].bval;

		  برای (unsigned idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			صحیح bit = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			انتخاب (bit) {
			    حالت 0:
			      bit4 = BIT4_0;
			      بشکن;
			    حالت 1:
			      bit4 = BIT4_1;
			      بشکن;
			    حالت 2:
			      bit4 = BIT4_Z;
			      بشکن;
			    حالت 3:
			      bit4 = BIT4_X;
			      بشکن;
			    پیشفرض:
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
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (صحیح)vp->format, vwid_);
	    أكد(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      بازگشت 0;
}

ساختار sysfunc_rnet : عمومی sysfunc_def {
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_مقدار(p_vpi_مقدار vp, صحیح)
{
      put_مقدار = درست;

      دوگانه value;
      انتخاب (vp->format) {

	  حالت vpiRealVal:
	    value = vp->value.real;
	    بشکن;

	  پیشفرض:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (صحیح)vp->format);
	    أكد(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      بازگشت 0;
}

ساختار sysfunc_no : عمومی sysfunc_def {
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags) override;
};

vpiHandle sysfunc_no::vpi_put_مقدار(p_vpi_مقدار, صحیح)
{
      بازگشت 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
ایستا ساختار __vpiUserSystf**def_table = 0;
ایستا unsigned def_شمار = 0;

ایستا ساختار __vpiUserSystf* allocate_def(تهی)
{
      اگر (def_table == 0) {
	    def_table = static_cast<ساختار __vpiUserSystf**>
	                (malloc(اندازه(ساختار __vpiUserSystf*)));

	    def_table[0] = جدید __vpiUserSystf;

	    def_شمار = 1;
	    بازگشت def_table[0];
      }

      def_table = static_cast<ساختار __vpiUserSystf**>
                  (realloc(def_table,
                   (def_شمار+1)*اندازه(ساختار __vpiUserSystf*)));

      def_table[def_شمار] = جدید __vpiUserSystf;

      بازگشت def_table[def_شمار++];
}

#ifdef CHECK_WITH_VALGRIND
تهی def_table_delete(تهی)
{
      برای (unsigned idx = 0; idx < def_شمار; idx += 1) {
	    آزاد(const_cast<نویسه *>(def_table[idx]->info.tfname));
	    delete def_table[idx];
      }
      آزاد(def_table);
      def_table = 0;
      def_شمار = 0;
}
#endif

رده __vpiSystfIterator : عمومی __vpiHandle {
    عمومی:
      explicit __vpiSystfIterator(unsigned idx);
      صحیح get_type_code(تهی) ثابت override;
      vpiHandle vpi_index(صحیح idx) override;
      free_object_fun_t free_object_fun(تهی) override;

      unsigned next;
};

ایستا vpiHandle systf_iterator_scan(vpiHandle ref, صحیح)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      اگر (obj->next >= def_شمار) {
	    vpi_free_object(ref);
	    بازگشت 0;
      }

      unsigned use_index = obj->next;
      وقتی (!def_table[use_index]->is_user_defn) {
	    obj->next += 1;
	    use_index = obj->next;
	    اگر (obj->next >= def_شمار) {
		  vpi_free_object(ref);
		  بازگشت 0;
	    }
      }
      obj->next += 1;
      بازگشت def_table[use_index];
}

ایستا صحیح systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      delete obj;
      بازگشت 1;
}

درونخطی __vpiSystfIterator::__vpiSystfIterator(unsigned idx)
{ next = idx; }

صحیح __vpiSystfIterator::get_type_code(تهی) ثابت
{ بازگشت vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_index(صحیح idx)
{ بازگشت systf_iterator_scan(این, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(تهی)
{ بازگشت &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(تهی)
{
	/* Check to see if there are any user defined functions. */
      منطقی have_user_defn = نادرست;
      unsigned idx;
      برای (idx = 0; idx < def_شمار; idx += 1) {
	    اگر (def_table[idx]->is_user_defn) {
		  have_user_defn = درست;
		  بشکن;
	    }
      }
      اگر (!have_user_defn) بازگشت 0;

      __vpiSystfIterator*res = جدید __vpiSystfIterator(idx);
      res->next = idx;
      بازگشت res;
}

ساختار __vpiUserSystf* vpip_find_systf(ثابت نویسه*name)
{
      برای (unsigned idx = 0 ;  idx < def_شمار ;  idx += 1)
	    اگر (strcmp(def_table[idx]->info.tfname, name) == 0)
		  بازگشت def_table[idx];

      بازگشت 0;
}

تهی vpip_make_systf_system_defined(vpiHandle ref)
{
      أكد(ref);
      ساختار __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      أكد(obj);
      obj->is_user_defn = نادرست;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
شمارش vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
تعریفنوع ساختار vpi_call_error {
      vpi_call_error_type type;
      نویسه *name;
      long file_idx;
      long lineno;
} vpi_call_error_s, *vpi_call_error_p;

ایستا vpi_call_error_p vpi_call_error_lst = پوچ;
ایستا unsigned vpi_call_error_رقم = 0;

ایستا تهی add_vpi_call_error(vpi_call_error_type type, ثابت نویسه *name,
                               long file_idx, long lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (realloc(vpi_call_error_lst,
                                    (vpi_call_error_رقم + 1) *
                                    اندازه(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_رقم].type = type;
      vpi_call_error_lst[vpi_call_error_رقم].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_رقم].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_رقم].lineno = lineno;
      vpi_call_error_رقم += 1;
}

تهی print_vpi_call_errors()
{
      برای (unsigned idx = 0; idx < vpi_call_error_رقم; idx += 1) {
	    انتخاب (vpi_call_error_lst[idx].type) {
		حالت VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (صحیح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  بشکن;
		حالت VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (صحیح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  بشکن;
		حالت VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (صحیح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  بشکن;
		حالت VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_names[vpi_call_error_lst[idx].file_idx],
		                  (صحیح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_names[vpi_call_error_lst[idx].file_idx],
                                 (صحیح)vpi_call_error_lst[idx].lineno);
		  بشکن;
	    }
	    آزاد(vpi_call_error_lst[idx].name);
      }
      آزاد(vpi_call_error_lst);
      fflush(stderr);
}

#ifdef CHECK_WITH_VALGRIND
ایستا تهی cleanup_vpi_call_args(unsigned argc, vpiHandle*argv)
{
      اگر (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    ساختار __vpiSysTaskCall*obj = جدید systask_def;
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
vpiHandle vpip_build_vpi_call(ثابت نویسه*name, صحیح val_code, unsigned return_width,
			      vvp_net_t*fnet,
			      منطقی func_as_task_err, منطقی func_as_task_warn,
			      unsigned argc, vpiHandle*argv,
			      unsigned vec4_پشته, unsigned real_پشته, unsigned string_پشته,
			      long file_idx, long lineno)
{
      أكد(!(func_as_task_err && func_as_task_warn));

      ساختار __vpiUserSystf*defn = vpip_find_systf(name);
      اگر (defn == 0) {
	    add_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
	    cleanup_vpi_call_args(argc, argv);
#endif
	    بازگشت 0;
      }

      انتخاب (defn->info.type) {
	  حالت vpiSysTask:
	    اگر (val_code != 0 || fnet != 0) {
		  add_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#ifdef CHECK_WITH_VALGRIND
		  cleanup_vpi_call_args(argc, argv);
#endif
		  بازگشت 0;
	    }
	    بشکن;

	  حالت vpiSysFunc:
	    اگر (val_code == 0 && fnet == 0) {
		  اگر (func_as_task_err) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#ifdef CHECK_WITH_VALGRIND
			cleanup_vpi_call_args(argc, argv);
#endif
			بازگشت 0;
		  } وگرنه اگر (func_as_task_warn) {
			add_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (صحیح)defn->info.type);
	    أكد(0);
      }

      ساختار __vpiSysTaskCall*obj = 0;

      انتخاب (defn->info.type) {
	  حالت vpiSysTask:
	    obj = جدید systask_def;
	    بشکن;

	  حالت vpiSysFunc:
	    اگر (fnet && val_code == -vpiRealVal) {
		  obj = جدید sysfunc_rnet;

	    } وگرنه اگر (fnet && val_code == -vpiVectorVal) {
		  obj = جدید sysfunc_4net(return_width);

	    } وگرنه اگر (val_code == -vpiRealVal) {
		  obj = جدید sysfunc_real;

	    } وگرنه اگر (val_code == -vpiVectorVal) {
		  obj = جدید sysfunc_vec4(return_width);

	    } وگرنه اگر (val_code == -vpiStringVal) {
		  obj = جدید sysfunc_str;

           } وگرنه اگر (val_code == 0 && fnet == 0) {
		  obj = جدید sysfunc_no;

	    } وگرنه {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_code);
		  أكد(0);
	    }
	    بشکن;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->args  = argv;
      obj->vec4_پشته = vec4_پشته;
      obj->real_پشته = real_پشته;
      obj->string_پشته = string_پشته;
      obj->fnet  = fnet;
      obj->file_idx  = (unsigned) file_idx;
      obj->lineno   = (unsigned) lineno;
      obj->userdata  = 0;
      obj->put_مقدار = نادرست;

      compile_compiletf(obj);

      بازگشت obj;
}

#ifdef CHECK_WITH_VALGRIND
تهی vpi_call_delete(vpiHandle item)
{
      ساختار __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(item);
	/* The object can be NULL if there was an error. */
      اگر (!obj) بازگشت;
      برای (unsigned arg = 0; arg < obj->nargs; arg += 1) {
	    انتخاب (obj->args[arg]->get_type_code()) {
		حالت vpiConstant:
		  انتخاب (vpi_get(_vpiFromThr, obj->args[arg])) {
		      حالت _vpiNoThr:
			constant_delete(obj->args[arg]);
			بشکن;
		      حالت _vpiString:
			thread_string_delete(obj->args[arg]);
			بشکن;
		      حالت _vpiVThr:
			thread_vthr_delete(obj->args[arg]);
			بشکن;
		      حالت _vpiWord:
			thread_word_delete(obj->args[arg]);
			بشکن;
		      پیشفرض:
			أكد(0);
		  }
		  بشکن;
		حالت vpiMemoryWord:
		  انتخاب (vpi_get(_vpiFromThr, obj->args[arg])) {
		      حالت _vpi_at_A:
			A_delete(obj->args[arg]);
			بشکن;
		  }
		  بشکن;
		حالت vpiPartSelect:
		  انتخاب (vpi_get(_vpiFromThr, obj->args[arg])) {
		      حالت _vpi_at_PV:
			PV_delete(obj->args[arg]);
			بشکن;
		      حالت _vpi_at_APV:
			APV_delete(obj->args[arg]);
			بشکن;
		  }
		  بشکن;
	    }
      }
      آزاد(obj->args);
      delete obj;
}
#endif

/*
 * This function is used by the %vpi_call instruction to actually
 * place the call to the system task/function. For now, only support
 * calls to system tasks.
 */

vthread_t vpip_current_vthread;

تهی vpip_execute_vpi_call(vthread_t thr, vpiHandle ref)
{
      vpip_current_vthread = thr;

      vpip_cur_task = dynamic_cast<__vpiSysTaskCall*>(ref);

      اگر (vpip_cur_task->defn->info.calltf) {
	    أكد(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = VPI_MODE_CALLTF;
	    vpip_cur_task->put_مقدار = نادرست;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_data);
	    vpi_mode_flag = VPI_MODE_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    اگر (ref->get_type_code() == vpiSysFuncCall &&
	        !vpip_cur_task->put_مقدار) {
		  s_vpi_مقدار val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_مقدار(ref, &val, 0, vpiNoDelay);
	    }
      }
      اگر (vpip_cur_task->vec4_پشته > 0)
	    vthread_برداشت_vec4(thr, vpip_cur_task->vec4_پشته);
      اگر (vpip_cur_task->real_پشته > 0)
	    vthread_برداشت_real(thr, vpip_cur_task->real_پشته);
      اگر (vpip_cur_task->string_پشته > 0)
	    vthread_برداشت_str(thr, vpip_cur_task->string_پشته);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      اگر (ثابت sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_افزودن(thr, func_real->return_مقدار());
      }
      وگرنه اگر (ثابت sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_افزودن(thr, func_string->return_مقدار());
      }
      وگرنه اگر (ثابت sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
	    vthread_افزودن(thr, func_vec4->return_مقدار());
      }
      vpip_cur_task = 0;
}

/*
 * This is the entry function that a VPI module uses to hook a new
 * task/function into the simulator. The function creates a new
 * __vpi_userSystf to represent the definition for the calls that come
 * to pass later.
 */
vpiHandle vpi_register_systf(ثابت ساختار t_vpi_systf_data*ss)
{
      ساختار __vpiUserSystf*cur = allocate_def();
      أكد(ss);
      انتخاب (ss->type) {
	  حالت vpiSysTask:
	  حالت vpiSysFunc:
	    بشکن;
	  پیشفرض:
	    fprintf(stderr, "Unsupported type %d.\n", (صحیح)ss->type);
	    أكد(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = درست;

      بازگشت cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, تهی*data)
{
      ساختار __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      اگر (rfp == 0)
	    بازگشت 0;

      rfp->userdata = data;
      بازگشت 1;
}

تهی* vpi_get_userdata(vpiHandle ref)
{
      ساختار __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      أكد(rfp);

      بازگشت rfp->userdata;
}
