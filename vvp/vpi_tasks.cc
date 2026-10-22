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
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#جے_انت
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

ورتوں نطاق std;

مضمن __vpiUserSystf::__vpiUserSystf()
{ }

صحيح __vpiUserSystf::get_type_کوڈ(فراغ) ثابت
{ أرجع vpiUserSystf; }


ساكن vpiHandle systask_handle(صحيح type, vpiHandle ref)
{
      بنية __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      بدل (type) {
	  حالة vpiScope:
	    أرجع rfp->scope;

	  حالة vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    أكد(rfp->defn->is_user_defn);
	    أرجع rfp->defn;

	  افتراضي:
	    أرجع 0;
      };
}

ساكن صحيح systask_get(صحيح type, vpiHandle ref)
{
      بنية __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      بدل (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  حالة vpi_ویلا_Unit:
	    أرجع rfp->scope->ویلا_units;
	  حالة vpi_ویلا_Precision:
	    أرجع rfp->scope->ویلا_precision;

	  حالة vpi_سطر_No:
	    أرجع rfp->lineno;

	  حالة vpiUserDefn:
	    أرجع rfp->defn->is_user_defn;

	  افتراضي:
	    أرجع vpiUndefined;
      }
}

// support getting vpiSize for a system function call
ساكن صحيح sysfunc_get(صحيح type, vpiHandle ref)
{
      بنية __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      بدل (type) {
	  حالة vpi_ناپ:
	    أكد(0); // This should be handled by derived classes
	    أرجع 0;

	  حالة vpi_سطر_No:
	    أرجع rfp->lineno;

	  حالة vpiUserDefn:
	    أرجع rfp->defn->is_user_defn;

	  افتراضي:
	    أرجع vpiUndefined;
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
            أكد(rfp->file_idx < file_ناں.size());
            أرجع simple_سیٹ_rbuf_str(file_ناں[rfp->file_idx]);

          حالة vpiName:
            أرجع simple_سیٹ_rbuf_str(rfp->defn->info.tfname);
      }

      أرجع 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
ساكن vpiHandle systask_iter(صحيح, vpiHandle ref)
{
      بنية __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      إذا (rfp->nargs == 0)
	    أرجع 0;

      أرجع vpip_make_iterator(rfp->nargs, rfp->دلیلاں, خطأ);
}

بنية systask_def : عام __vpiSysTaskCall {
      مجازی صحيح get_type_کوڈ(فراغ) ثابت   override { أرجع vpiSysTaskCall; }
      مجازی صحيح vpi_get(صحيح code)           override { أرجع systask_get(code, هذا); }
      مجازی حرف*vpi_get_str(صحيح code)      override { أرجع systask_get_str(code, هذا); }
      مجازی vpiHandle vpi_handle(صحيح code)  override { أرجع systask_handle(code, هذا); }
      مجازی vpiHandle vpi_iterate(صحيح code) override { أرجع systask_iter(code, هذا); }
};

بنية sysfunc_def : عام systask_def {
      مجازی صحيح get_type_کوڈ(فراغ) ثابت override { أرجع vpiSysFuncCall; }
      مجازی صحيح vpi_get(صحيح code) override { أرجع sysfunc_get(code, هذا); }
};

بنية sysfunc_real : عام sysfunc_def {
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      مضمن مزدوج return_قدر() ثابت { أرجع return_قدر_2; }

    خاص:
      مزدوج return_قدر_2;
};

vpiHandle sysfunc_real::vpi_put_قدر(p_vpi_قدر vp, صحيح)
{
      put_قدر = سچ;

      return_قدر_2 = 0.0;
      بدل (vp->format) {
	  حالة vpiRealVal:
	    return_قدر_2 = vp->value.real;
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported format %d.\n", (صحيح)vp->format);
	    أكد(0);
      }

      أرجع 0;
}

بنية sysfunc_str : عام sysfunc_def {
      صحيح vpi_get(صحيح code) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      مضمن ثابت std::string& return_قدر() ثابت { أرجع return_قدر_2; };

    خاص:
      std::string return_قدر_2;
};

صحيح sysfunc_str::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_ناپ:
	    أرجع return_قدر_2.size();

	  حالة vpi_سطر_No:
	    أرجع lineno;

	  حالة vpiUserDefn:
	    أرجع defn->is_user_defn;

	  افتراضي:
	    أرجع vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_قدر(p_vpi_قدر vp, صحيح)
{
      put_قدر = سچ;

      بدل (vp->format) {
	  حالة vpiStringVal:
	    return_قدر_2 = std::string(vp->value.str);
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported format %d.\n", (صحيح)vp->format);
	    أكد(0);
      }

      أرجع 0;
}

صنف sysfunc_vec4 : عام sysfunc_def {
    عام:
      واضح مضمن sysfunc_vec4(بنانشانی wid): return_قدر_2(wid, BIT4_X) { }
      صحيح vpi_get(صحيح code) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      مضمن ثابت vvp_vector4_t& return_قدر() ثابت { أرجع return_قدر_2; }

    خاص:
      vpiHandle put_قدر_scalar(p_vpi_قدر vp);
      vpiHandle put_قدر_int(p_vpi_قدر vp);
      vpiHandle put_قدر_string(p_vpi_قدر vp);
      vpiHandle put_قدر_سمتیہ(p_vpi_قدر vp);
      vpiHandle put_قدر_ویلا(p_vpi_قدر vp);

      vvp_vector4_t return_قدر_2;
};

صحيح sysfunc_vec4::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_ناپ:
	    أرجع return_قدر_2.size();

	  حالة vpi_سطر_No:
	    أرجع lineno;

	  حالة vpiUserDefn:
	    أرجع defn->is_user_defn;

	  افتراضي:
	    أرجع vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_قدر_scalar(ثابت p_vpi_قدر vp)
{
      بدل (vp->value.scalar) {
	  حالة vpi0:
	    return_قدر_2.سیٹ_ثنائی_ہندسہ(0, BIT4_0);
	    اكسر;
	  حالة vpi1:
	    return_قدر_2.سیٹ_ثنائی_ہندسہ(0, BIT4_1);
	    اكسر;
	  حالة vpiX:
	    return_قدر_2.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
	    اكسر;
	  حالة vpiZ:
	    return_قدر_2.سیٹ_ثنائی_ہندسہ(0, BIT4_Z);
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported value %d.\n", (صحيح)vp->value.scalar);
	    أكد(0);
      }
      أرجع 0;
}

vpiHandle sysfunc_vec4::put_قدر_int(p_vpi_قدر vp)
{
      لما tmp = vp->value.integer;
      بنانشانی width = return_قدر_2.size();
      لكل (بنانشانی idx = 0 ;  idx < width ;  idx += 1) {
	    return_قدر_2.سیٹ_ثنائی_ہندسہ(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      أرجع 0;
}

vpiHandle sysfunc_vec4::put_قدر_string(p_vpi_قدر vp)
{
      size_t slen = لڑی_لمبائی(vp->value.str);
      بنانشانی wid = return_قدر_2.size();

      لكل (بنانشانی idx = 0 ; idx < wid ; idx += 1) {
	    بنانشانی cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    إذا (cidx >= slen) {
		  return_قدر_2.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);
		  استمر;
	    }

	      // Put the end of the string in the LSB of the vector
	    حرف use_char = vp->value.str[slen - 1 - cidx];

	    إذا ((use_char >> (idx % 8)) & 1)
		  return_قدر_2.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
	    وإلا
		  return_قدر_2.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);
      }
      أرجع 0;
}

vpiHandle sysfunc_vec4::put_قدر_سمتیہ(p_vpi_قدر vp)
{
      بنانشانی width = return_قدر_2.size();
      لكل (بنانشانی idx = 0 ; idx < width ; idx += 1) {
	    بنانشانی لفظ = idx / 32;
	    بنانشانی bidx = idx % 32;

	    بنانشانی لما aval = vp->value.vector[لفظ].aval >> bidx;
	    بنانشانی لما bval = vp->value.vector[لفظ].bval >> bidx;

	    صحيح ثنائی_ہندسہ = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    بدل (ثنائی_ہندسہ) {
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
	    return_قدر_2.سیٹ_ثنائی_ہندسہ(idx, bit4);
      }
      أرجع 0;
}

vpiHandle sysfunc_vec4::put_قدر_ویلا(p_vpi_قدر vp)
{
      بنانشانی width = return_قدر_2.size();
      لما tmp = 0;
      لكل (بنانشانی idx = 0 ; idx < width ; idx += 1) {
	    إذا (idx == 0)
		  tmp = vp->value.time->کم;
	    وإلا إذا (idx == 32)
		  tmp = vp->value.time->بلند;
	    وإلا إذا (idx == 64)
		  tmp = 0;

	    return_قدر_2.سیٹ_ثنائی_ہندسہ(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      أرجع 0;
}

vpiHandle sysfunc_vec4::vpi_put_قدر(p_vpi_قدر vp, صحيح)
{
      put_قدر = سچ;

      بدل (vp->format) {
	  حالة vpiScalarVal:
	    أرجع put_قدر_scalar(vp);
	  حالة vpiIntVal:
	    أرجع put_قدر_int(vp);
	  حالة vpiStringVal:
	    أرجع put_قدر_string(vp);
	  حالة vpi_سمتیہ_Val:
	    أرجع put_قدر_سمتیہ(vp);
	  حالة vpi_ویلا_Val:
	    أرجع put_قدر_ویلا(vp);
	  افتراضي:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (صحيح)vp->format);
	    أكد(0);
      }

      أرجع 0;
}

بنية sysfunc_4net : عام sysfunc_def {
      واضح مضمن sysfunc_4net(بنانشانی wid) : vwid_(wid) { }
      صحيح vpi_get(صحيح code) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;

    خاص:
      بنانشانی vwid_;
};

// support getting vpiSize for a system function call
صحيح sysfunc_4net::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_ناپ:
	    أرجع vwid_;

	  حالة vpi_سطر_No:
	    أرجع lineno;

	  حالة vpiUserDefn:
	    أرجع defn->is_user_defn;

	  افتراضي:
	    أرجع vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_قدر(p_vpi_قدر vp, صحيح)
{
      put_قدر = سچ;

      vvp_vector4_t val (vwid_);

      بدل (vp->format) {

          حالة vpiScalarVal: {
	        بدل(vp->value.scalar) {
		      حالة vpi0:
			val.سیٹ_ثنائی_ہندسہ(0, BIT4_0);
                        اكسر;
		      حالة vpi1:
                        val.سیٹ_ثنائی_ہندسہ(0, BIT4_1);
                        اكسر;
		      حالة vpiX:
                        val.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
                        اكسر;
		      حالة vpiZ:
                        val.سیٹ_ثنائی_ہندسہ(0, BIT4_Z);
                        اكسر;
		      افتراضي:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (صحيح)vp->value.scalar);
                        أكد(0);
                }
		اكسر;
          }

	  حالة vpiIntVal: {
		لما tmp = vp->value.integer;
		لكل (بنانشانی idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.سیٹ_ثنائی_ہندسہ(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		اكسر;
	  }

          حالة vpi_ویلا_Val: {
                بنانشانی لما tmp = vp->value.time->کم;
                لكل (بنانشانی idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.سیٹ_ثنائی_ہندسہ(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      إذا (idx == 31)
                            tmp = vp->value.time->بلند;
                      وإلا
                            tmp >>= 1;
                }
                اكسر;
          }

	  حالة vpi_سمتیہ_Val:

	    لكل (بنانشانی wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  بنانشانی لفظ = wdx / 32;
		  بنانشانی لما aval = vp->value.vector[لفظ].aval;
		  بنانشانی لما bval = vp->value.vector[لفظ].bval;

		  لكل (بنانشانی idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			صحيح ثنائی_ہندسہ = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			بدل (ثنائی_ہندسہ) {
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
			              ثنائی_ہندسہ);
			      أكد(0);
			}
			val.سیٹ_ثنائی_ہندسہ(wdx+idx, bit4);

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
      أرجع 0;
}

بنية sysfunc_rnet : عام sysfunc_def {
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_قدر(p_vpi_قدر vp, صحيح)
{
      put_قدر = سچ;

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

      أرجع 0;
}

بنية sysfunc_no : عام sysfunc_def {
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
};

vpiHandle sysfunc_no::vpi_put_قدر(p_vpi_قدر, صحيح)
{
      أرجع 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
ساكن بنية __vpiUserSystf**def_جدول = 0;
ساكن بنانشانی def_گنتی = 0;

ساكن بنية __vpiUserSystf* allocate_def(فراغ)
{
      إذا (def_جدول == 0) {
	    def_جدول = static_cast<بنية __vpiUserSystf**>
	                (حافظہ_دیو(حجم(بنية __vpiUserSystf*)));

	    def_جدول[0] = جديد __vpiUserSystf;

	    def_گنتی = 1;
	    أرجع def_جدول[0];
      }

      def_جدول = static_cast<بنية __vpiUserSystf**>
                  (realloc(def_جدول,
                   (def_گنتی+1)*حجم(بنية __vpiUserSystf*)));

      def_جدول[def_گنتی] = جديد __vpiUserSystf;

      أرجع def_جدول[def_گنتی++];
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ def_جدول_delete(فراغ)
{
      لكل (بنانشانی idx = 0; idx < def_گنتی; idx += 1) {
	    حرر(const_cast<حرف *>(def_جدول[idx]->info.tfname));
	    مٹاؤ def_جدول[idx];
      }
      حرر(def_جدول);
      def_جدول = 0;
      def_گنتی = 0;
}
#جے_انت

صنف __vpiSystfIterator : عام __vpiHandle {
    عام:
      واضح __vpiSystfIterator(بنانشانی idx);
      صحيح get_type_کوڈ(فراغ) ثابت override;
      vpiHandle vpi_اشاریہ(صحيح idx) override;
      free_object_fun_t free_object_fun(فراغ) override;

      بنانشانی اگلا_2;
};

ساكن vpiHandle systf_iterator_جانچ(vpiHandle ref, صحيح)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      إذا (obj->اگلا_2 >= def_گنتی) {
	    vpi_free_object(ref);
	    أرجع 0;
      }

      بنانشانی use_اشاریہ = obj->اگلا_2;
      طالما (!def_جدول[use_اشاریہ]->is_user_defn) {
	    obj->اگلا_2 += 1;
	    use_اشاریہ = obj->اگلا_2;
	    إذا (obj->اگلا_2 >= def_گنتی) {
		  vpi_free_object(ref);
		  أرجع 0;
	    }
      }
      obj->اگلا_2 += 1;
      أرجع def_جدول[use_اشاریہ];
}

ساكن صحيح systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      مٹاؤ obj;
      أرجع 1;
}

مضمن __vpiSystfIterator::__vpiSystfIterator(بنانشانی idx)
{ اگلا_2 = idx; }

صحيح __vpiSystfIterator::get_type_کوڈ(فراغ) ثابت
{ أرجع vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_اشاریہ(صحيح idx)
{ أرجع systf_iterator_جانچ(هذا, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(فراغ)
{ أرجع &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(فراغ)
{
	/* Check to see if there are any user defined functions. */
      منطقي have_user_defn = خطأ;
      بنانشانی idx;
      لكل (idx = 0; idx < def_گنتی; idx += 1) {
	    إذا (def_جدول[idx]->is_user_defn) {
		  have_user_defn = سچ;
		  اكسر;
	    }
      }
      إذا (!have_user_defn) أرجع 0;

      __vpiSystfIterator*res = جديد __vpiSystfIterator(idx);
      res->اگلا_2 = idx;
      أرجع res;
}

بنية __vpiUserSystf* vpip_لبھو_systf(ثابت حرف*name)
{
      لكل (بنانشانی idx = 0 ;  idx < def_گنتی ;  idx += 1)
	    إذا (strcmp(def_جدول[idx]->info.tfname, name) == 0)
		  أرجع def_جدول[idx];

      أرجع 0;
}

فراغ vpip_make_systf_system_defined(vpiHandle ref)
{
      أكد(ref);
      بنية __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      أكد(obj);
      obj->is_user_defn = خطأ;
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
      لما file_idx;
      لما lineno;
} vpi_call_error_s, *vpi_call_error_p;

ساكن vpi_call_error_p vpi_call_error_lst = عدم;
ساكن بنانشانی vpi_call_error_عدد = 0;

ساكن فراغ جوڑو_vpi_call_error(vpi_call_error_type type, ثابت حرف *name,
                               لما file_idx, لما lineno)
{
      vpi_call_error_lst = static_cast<vpi_call_error_p>
                           (realloc(vpi_call_error_lst,
                                    (vpi_call_error_عدد + 1) *
                                    حجم(vpi_call_error_s)));
      vpi_call_error_lst[vpi_call_error_عدد].type = type;
      vpi_call_error_lst[vpi_call_error_عدد].name = strdup(name);
      vpi_call_error_lst[vpi_call_error_عدد].file_idx = file_idx;
      vpi_call_error_lst[vpi_call_error_عدد].lineno = lineno;
      vpi_call_error_عدد += 1;
}

فراغ print_vpi_call_errors()
{
      لكل (بنانشانی idx = 0; idx < vpi_call_error_عدد; idx += 1) {
	    بدل (vpi_call_error_lst[idx].type) {
		حالة VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_ناں[vpi_call_error_lst[idx].file_idx],
		                  (صحيح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  اكسر;
		حالة VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_ناں[vpi_call_error_lst[idx].file_idx],
		                  (صحيح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  اكسر;
		حالة VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_ناں[vpi_call_error_lst[idx].file_idx],
		                  (صحيح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  اكسر;
		حالة VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_ناں[vpi_call_error_lst[idx].file_idx],
		                  (صحيح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_ناں[vpi_call_error_lst[idx].file_idx],
                                 (صحيح)vpi_call_error_lst[idx].lineno);
		  اكسر;
	    }
	    حرر(vpi_call_error_lst[idx].name);
      }
      حرر(vpi_call_error_lst);
      fflush(stderr);
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
ساكن فراغ cleanup_vpi_call_دلیلاں(بنانشانی argc, vpiHandle*argv)
{
      إذا (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    بنية __vpiSysTaskCall*obj = جديد systask_def;
	    obj->nargs = argc;
	    obj->دلیلاں  = argv;
	    vpi_call_delete(obj);
      }
}
#جے_انت

/*
 * A vpi_call is actually built up into a vpiSysTaskCall VPI object
 * that refers back to the vpiUserSystf VPI object that is the
 * definition. So this function is called by the compiler when a
 * %vpi_call statement is encountered. Create here a vpiHandle that
 * describes the call, and return it. The %vpi_call instruction will
 * store this handle for when it is executed.
 */
vpiHandle vpip_build_vpi_call(ثابت حرف*name, صحيح val_کوڈ, بنانشانی return_چوڑائی,
			      vvp_net_t*fnet,
			      منطقي func_as_task_err, منطقي func_as_task_warn,
			      بنانشانی argc, vpiHandle*argv,
			      بنانشانی vec4_ڈھیر, بنانشانی real_ڈھیر, بنانشانی string_ڈھیر,
			      لما file_idx, لما lineno)
{
      أكد(!(func_as_task_err && func_as_task_warn));

      بنية __vpiUserSystf*defn = vpip_لبھو_systf(name);
      إذا (defn == 0) {
	    جوڑو_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
	    cleanup_vpi_call_دلیلاں(argc, argv);
#جے_انت
	    أرجع 0;
      }

      بدل (defn->info.type) {
	  حالة vpiSysTask:
	    إذا (val_کوڈ != 0 || fnet != 0) {
		  جوڑو_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
		  cleanup_vpi_call_دلیلاں(argc, argv);
#جے_انت
		  أرجع 0;
	    }
	    اكسر;

	  حالة vpiSysFunc:
	    إذا (val_کوڈ == 0 && fnet == 0) {
		  إذا (func_as_task_err) {
			جوڑو_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
			cleanup_vpi_call_دلیلاں(argc, argv);
#جے_انت
			أرجع 0;
		  } وإلا إذا (func_as_task_warn) {
			جوڑو_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
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
	    إذا (fnet && val_کوڈ == -vpiRealVal) {
		  obj = جديد sysfunc_rnet;

	    } وإلا إذا (fnet && val_کوڈ == -vpi_سمتیہ_Val) {
		  obj = جديد sysfunc_4net(return_چوڑائی);

	    } وإلا إذا (val_کوڈ == -vpiRealVal) {
		  obj = جديد sysfunc_real;

	    } وإلا إذا (val_کوڈ == -vpi_سمتیہ_Val) {
		  obj = جديد sysfunc_vec4(return_چوڑائی);

	    } وإلا إذا (val_کوڈ == -vpiStringVal) {
		  obj = جديد sysfunc_str;

           } وإلا إذا (val_کوڈ == 0 && fnet == 0) {
		  obj = جديد sysfunc_no;

	    } وإلا {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_کوڈ);
		  أكد(0);
	    }
	    اكسر;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->دلیلاں  = argv;
      obj->vec4_ڈھیر = vec4_ڈھیر;
      obj->real_ڈھیر = real_ڈھیر;
      obj->string_ڈھیر = string_ڈھیر;
      obj->fnet  = fnet;
      obj->file_idx  = (بنانشانی) file_idx;
      obj->lineno   = (بنانشانی) lineno;
      obj->userdata  = 0;
      obj->put_قدر = خطأ;

      compile_compiletf(obj);

      أرجع obj;
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ vpi_call_delete(vpiHandle شے)
{
      بنية __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(شے);
	/* The object can be NULL if there was an error. */
      إذا (!obj) أرجع;
      لكل (بنانشانی arg = 0; arg < obj->nargs; arg += 1) {
	    بدل (obj->دلیلاں[arg]->get_type_کوڈ()) {
		حالة vpiConstant:
		  بدل (vpi_get(_vpiFromThr, obj->دلیلاں[arg])) {
		      حالة _vpiNoThr:
			constant_delete(obj->دلیلاں[arg]);
			اكسر;
		      حالة _vpiString:
			thread_string_delete(obj->دلیلاں[arg]);
			اكسر;
		      حالة _vpiVThr:
			thread_vthr_delete(obj->دلیلاں[arg]);
			اكسر;
		      حالة vpi_لفظ:
			thread_لفظ_delete(obj->دلیلاں[arg]);
			اكسر;
		      افتراضي:
			أكد(0);
		  }
		  اكسر;
		حالة vpi_حافظہ_لفظ:
		  بدل (vpi_get(_vpiFromThr, obj->دلیلاں[arg])) {
		      حالة _vpi_at_A:
			A_delete(obj->دلیلاں[arg]);
			اكسر;
		  }
		  اكسر;
		حالة vpiPartSelect:
		  بدل (vpi_get(_vpiFromThr, obj->دلیلاں[arg])) {
		      حالة _vpi_at_PV:
			PV_delete(obj->دلیلاں[arg]);
			اكسر;
		      حالة _vpi_at_APV:
			APV_delete(obj->دلیلاں[arg]);
			اكسر;
		  }
		  اكسر;
	    }
      }
      حرر(obj->دلیلاں);
      مٹاؤ obj;
}
#جے_انت

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

      إذا (vpip_cur_task->defn->info.calltf) {
	    أكد(vpi_منوال_flag == VPI_منوال_NONE);
	    vpi_منوال_flag = VPI_منوال_CALLTF;
	    vpip_cur_task->put_قدر = خطأ;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_انکڑے);
	    vpi_منوال_flag = VPI_منوال_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    إذا (ref->get_type_کوڈ() == vpiSysFuncCall &&
	        !vpip_cur_task->put_قدر) {
		  s_vpi_قدر val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_قدر(ref, &val, 0, vpiNoDelay);
	    }
      }
      إذا (vpip_cur_task->vec4_ڈھیر > 0)
	    vthread_نکالو_vec4(thr, vpip_cur_task->vec4_ڈھیر);
      إذا (vpip_cur_task->real_ڈھیر > 0)
	    vthread_نکالو_real(thr, vpip_cur_task->real_ڈھیر);
      إذا (vpip_cur_task->string_ڈھیر > 0)
	    vthread_نکالو_str(thr, vpip_cur_task->string_ڈھیر);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      إذا (ثابت sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_ڈالو(thr, func_real->return_قدر());
      }
      وإلا إذا (ثابت sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_ڈالو(thr, func_string->return_قدر());
      }
      وإلا إذا (ثابت sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
	    vthread_ڈالو(thr, func_vec4->return_قدر());
      }
      vpip_cur_task = 0;
}

/*
 * This is the entry function that a VPI module uses to hook a new
 * task/function into the simulator. The function creates a new
 * __vpi_userSystf to represent the definition for the calls that come
 * to pass later.
 */
vpiHandle vpi_رجسٹر_systf(ثابت بنية t_vpi_systf_انکڑے*ss)
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
      cur->is_user_defn = سچ;

      أرجع cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, فراغ*انکڑے)
{
      بنية __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      إذا (rfp == 0)
	    أرجع 0;

      rfp->userdata = انکڑے;
      أرجع 1;
}

فراغ* vpi_get_userdata(vpiHandle ref)
{
      بنية __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      أكد(rfp);

      أرجع rfp->userdata;
}
