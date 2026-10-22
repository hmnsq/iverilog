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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
# include  <iostream>
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

استعمال نامخلا std;

اندرونی __vpiUserSystf::__vpiUserSystf()
{ }

صحیح __vpiUserSystf::get_type_رمز(خالی) ثابت
{ واپس vpiUserSystf; }


ساکن vpiHandle systask_handle(صحیح type, vpiHandle ref)
{
      ساخت __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      انتخاب (type) {
	  حالت vpiScope:
	    واپس rfp->scope;

	  حالت vpiUserSystf:
	      /* Assert that vpiUserDefn is true! */
	    أكد(rfp->defn->is_user_defn);
	    واپس rfp->defn;

	  طےشدہ:
	    واپس 0;
      };
}

ساکن صحیح systask_get(صحیح type, vpiHandle ref)
{
      ساخت __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      انتخاب (type) {
	    /* This is not the correct way to get this information, but
	     * some of the code that implements the acc and tf routines
	     * use this method so we will keep it in for now. */
	  حالت vpi_وقت_Unit:
	    واپس rfp->scope->وقت_units;
	  حالت vpi_وقت_Precision:
	    واپس rfp->scope->وقت_precision;

	  حالت vpi_سطر_No:
	    واپس rfp->lineno;

	  حالت vpiUserDefn:
	    واپس rfp->defn->is_user_defn;

	  طےشدہ:
	    واپس vpiUndefined;
      }
}

// support getting vpiSize for a system function call
ساکن صحیح sysfunc_get(صحیح type, vpiHandle ref)
{
      ساخت __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      انتخاب (type) {
	  حالت vpi_سائز:
	    أكد(0); // This should be handled by derived classes
	    واپس 0;

	  حالت vpi_سطر_No:
	    واپس rfp->lineno;

	  حالت vpiUserDefn:
	    واپس rfp->defn->is_user_defn;

	  طےشدہ:
	    واپس vpiUndefined;
      }
}

/*
 * the get_str function only needs to support vpiName
 */

ساکن حرف *systask_get_str(صحیح type, vpiHandle ref)
{
      ثابت ساخت __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      انتخاب (type) {
          حالت vpiFile:
            أكد(rfp->file_idx < file_نام.size());
            واپس simple_سیٹ_rbuf_str(file_نام[rfp->file_idx]);

          حالت vpiName:
            واپس simple_سیٹ_rbuf_str(rfp->defn->info.tfname);
      }

      واپس 0;
}

/*
 * the iter function only supports getting an iterator of the
 * arguments. This works equally well for tasks and functions.
 */
ساکن vpiHandle systask_iter(صحیح, vpiHandle ref)
{
      ساخت __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);

      اگر (rfp->nargs == 0)
	    واپس 0;

      واپس vpip_make_iterator(rfp->nargs, rfp->دلائل, جھوٹ);
}

ساخت systask_def : عوامی __vpiSysTaskCall {
      مجازی صحیح get_type_رمز(خالی) ثابت   override { واپس vpiSysTaskCall; }
      مجازی صحیح vpi_get(صحیح code)           override { واپس systask_get(code, یہ); }
      مجازی حرف*vpi_get_str(صحیح code)      override { واپس systask_get_str(code, یہ); }
      مجازی vpiHandle vpi_handle(صحیح code)  override { واپس systask_handle(code, یہ); }
      مجازی vpiHandle vpi_iterate(صحیح code) override { واپس systask_iter(code, یہ); }
};

ساخت sysfunc_def : عوامی systask_def {
      مجازی صحیح get_type_رمز(خالی) ثابت override { واپس vpiSysFuncCall; }
      مجازی صحیح vpi_get(صحیح code) override { واپس sysfunc_get(code, یہ); }
};

ساخت sysfunc_real : عوامی sysfunc_def {
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags) override;
      اندرونی دوہرا return_قدر() ثابت { واپس return_قدر_2; }

    نجی:
      دوہرا return_قدر_2;
};

vpiHandle sysfunc_real::vpi_put_قدر(p_vpi_قدر vp, صحیح)
{
      put_قدر = سچ;

      return_قدر_2 = 0.0;
      انتخاب (vp->format) {
	  حالت vpiRealVal:
	    return_قدر_2 = vp->value.real;
	    توڑو;
	  طےشدہ:
	    fprintf(stderr, "Unsupported format %d.\n", (صحیح)vp->format);
	    أكد(0);
      }

      واپس 0;
}

ساخت sysfunc_str : عوامی sysfunc_def {
      صحیح vpi_get(صحیح code) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags) override;
      اندرونی ثابت std::string& return_قدر() ثابت { واپس return_قدر_2; };

    نجی:
      std::string return_قدر_2;
};

صحیح sysfunc_str::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سائز:
	    واپس return_قدر_2.size();

	  حالت vpi_سطر_No:
	    واپس lineno;

	  حالت vpiUserDefn:
	    واپس defn->is_user_defn;

	  طےشدہ:
	    واپس vpiUndefined;
      }
}

vpiHandle sysfunc_str::vpi_put_قدر(p_vpi_قدر vp, صحیح)
{
      put_قدر = سچ;

      انتخاب (vp->format) {
	  حالت vpiStringVal:
	    return_قدر_2 = std::string(vp->value.str);
	    توڑو;
	  طےشدہ:
	    fprintf(stderr, "Unsupported format %d.\n", (صحیح)vp->format);
	    أكد(0);
      }

      واپس 0;
}

جماعت sysfunc_vec4 : عوامی sysfunc_def {
    عوامی:
      صریح اندرونی sysfunc_vec4(غیرعلامتی wid): return_قدر_2(wid, BIT4_X) { }
      صحیح vpi_get(صحیح code) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags) override;
      اندرونی ثابت vvp_vector4_t& return_قدر() ثابت { واپس return_قدر_2; }

    نجی:
      vpiHandle put_قدر_scalar(p_vpi_قدر vp);
      vpiHandle put_قدر_int(p_vpi_قدر vp);
      vpiHandle put_قدر_string(p_vpi_قدر vp);
      vpiHandle put_قدر_سمتیہ(p_vpi_قدر vp);
      vpiHandle put_قدر_وقت(p_vpi_قدر vp);

      vvp_vector4_t return_قدر_2;
};

صحیح sysfunc_vec4::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سائز:
	    واپس return_قدر_2.size();

	  حالت vpi_سطر_No:
	    واپس lineno;

	  حالت vpiUserDefn:
	    واپس defn->is_user_defn;

	  طےشدہ:
	    واپس vpiUndefined;
      }
}

vpiHandle sysfunc_vec4::put_قدر_scalar(ثابت p_vpi_قدر vp)
{
      انتخاب (vp->value.scalar) {
	  حالت vpi0:
	    return_قدر_2.سیٹ_ثنائی_ہندسہ(0, BIT4_0);
	    توڑو;
	  حالت vpi1:
	    return_قدر_2.سیٹ_ثنائی_ہندسہ(0, BIT4_1);
	    توڑو;
	  حالت vpiX:
	    return_قدر_2.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
	    توڑو;
	  حالت vpiZ:
	    return_قدر_2.سیٹ_ثنائی_ہندسہ(0, BIT4_Z);
	    توڑو;
	  طےشدہ:
	    fprintf(stderr, "Unsupported value %d.\n", (صحیح)vp->value.scalar);
	    أكد(0);
      }
      واپس 0;
}

vpiHandle sysfunc_vec4::put_قدر_int(p_vpi_قدر vp)
{
      طویل tmp = vp->value.integer;
      غیرعلامتی width = return_قدر_2.size();
      برائے (غیرعلامتی idx = 0 ;  idx < width ;  idx += 1) {
	    return_قدر_2.سیٹ_ثنائی_ہندسہ(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }

      واپس 0;
}

vpiHandle sysfunc_vec4::put_قدر_string(p_vpi_قدر vp)
{
      size_t slen = لڑی_لمبائی(vp->value.str);
      غیرعلامتی wid = return_قدر_2.size();

      برائے (غیرعلامتی idx = 0 ; idx < wid ; idx += 1) {
	    غیرعلامتی cidx = idx / 8;
	      // If wider then the string, then pad with zero.
	    اگر (cidx >= slen) {
		  return_قدر_2.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);
		  جاری;
	    }

	      // Put the end of the string in the LSB of the vector
	    حرف use_char = vp->value.str[slen - 1 - cidx];

	    اگر ((use_char >> (idx % 8)) & 1)
		  return_قدر_2.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
	    ورنہ
		  return_قدر_2.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);
      }
      واپس 0;
}

vpiHandle sysfunc_vec4::put_قدر_سمتیہ(p_vpi_قدر vp)
{
      غیرعلامتی width = return_قدر_2.size();
      برائے (غیرعلامتی idx = 0 ; idx < width ; idx += 1) {
	    غیرعلامتی لفظ = idx / 32;
	    غیرعلامتی bidx = idx % 32;

	    غیرعلامتی طویل aval = vp->value.vector[لفظ].aval >> bidx;
	    غیرعلامتی طویل bval = vp->value.vector[لفظ].bval >> bidx;

	    صحیح ثنائی_ہندسہ = (aval&1) | ((bval<<1)&2);
	    vvp_bit4_t bit4;

	    انتخاب (ثنائی_ہندسہ) {
		حالت 0:
		  bit4 = BIT4_0;
		  توڑو;
		حالت 1:
		  bit4 = BIT4_1;
		  توڑو;
		حالت 2:
		  bit4 = BIT4_Z;
		  توڑو;
		حالت 3:
		  bit4 = BIT4_X;
		  توڑو;
		طےشدہ:
		  أكد(0);
		  bit4 = BIT4_X;
		  توڑو;
	    }
	    return_قدر_2.سیٹ_ثنائی_ہندسہ(idx, bit4);
      }
      واپس 0;
}

vpiHandle sysfunc_vec4::put_قدر_وقت(p_vpi_قدر vp)
{
      غیرعلامتی width = return_قدر_2.size();
      طویل tmp = 0;
      برائے (غیرعلامتی idx = 0 ; idx < width ; idx += 1) {
	    اگر (idx == 0)
		  tmp = vp->value.time->کم;
	    ورنہ اگر (idx == 32)
		  tmp = vp->value.time->بلند;
	    ورنہ اگر (idx == 64)
		  tmp = 0;

	    return_قدر_2.سیٹ_ثنائی_ہندسہ(idx, (tmp&1)? BIT4_1 : BIT4_0);
	    tmp >>= 1;
      }
      واپس 0;
}

vpiHandle sysfunc_vec4::vpi_put_قدر(p_vpi_قدر vp, صحیح)
{
      put_قدر = سچ;

      انتخاب (vp->format) {
	  حالت vpiScalarVal:
	    واپس put_قدر_scalar(vp);
	  حالت vpiIntVal:
	    واپس put_قدر_int(vp);
	  حالت vpiStringVal:
	    واپس put_قدر_string(vp);
	  حالت vpi_سمتیہ_Val:
	    واپس put_قدر_سمتیہ(vp);
	  حالت vpi_وقت_Val:
	    واپس put_قدر_وقت(vp);
	  طےشدہ:
	    fprintf(stderr, "Unsupported format %d setting sysfunc vec4 value.\n", (صحیح)vp->format);
	    أكد(0);
      }

      واپس 0;
}

ساخت sysfunc_4net : عوامی sysfunc_def {
      صریح اندرونی sysfunc_4net(غیرعلامتی wid) : vwid_(wid) { }
      صحیح vpi_get(صحیح code) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags) override;

    نجی:
      غیرعلامتی vwid_;
};

// support getting vpiSize for a system function call
صحیح sysfunc_4net::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سائز:
	    واپس vwid_;

	  حالت vpi_سطر_No:
	    واپس lineno;

	  حالت vpiUserDefn:
	    واپس defn->is_user_defn;

	  طےشدہ:
	    واپس vpiUndefined;
      }
}

vpiHandle sysfunc_4net::vpi_put_قدر(p_vpi_قدر vp, صحیح)
{
      put_قدر = سچ;

      vvp_vector4_t val (vwid_);

      انتخاب (vp->format) {

          حالت vpiScalarVal: {
	        انتخاب(vp->value.scalar) {
		      حالت vpi0:
			val.سیٹ_ثنائی_ہندسہ(0, BIT4_0);
                        توڑو;
		      حالت vpi1:
                        val.سیٹ_ثنائی_ہندسہ(0, BIT4_1);
                        توڑو;
		      حالت vpiX:
                        val.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
                        توڑو;
		      حالت vpiZ:
                        val.سیٹ_ثنائی_ہندسہ(0, BIT4_Z);
                        توڑو;
		      طےشدہ:
                        fprintf(stderr, "Unsupported bit value %d.\n",
                                (صحیح)vp->value.scalar);
                        أكد(0);
                }
		توڑو;
          }

	  حالت vpiIntVal: {
		طویل tmp = vp->value.integer;
		برائے (غیرعلامتی idx = 0 ;  idx < vwid_ ;  idx += 1) {
		      val.سیٹ_ثنائی_ہندسہ(idx, (tmp&1)? BIT4_1 : BIT4_0);
		      tmp >>= 1;
		}
		توڑو;
	  }

          حالت vpi_وقت_Val: {
                غیرعلامتی طویل tmp = vp->value.time->کم;
                برائے (غیرعلامتی idx = 0 ;  idx < vwid_ ;  idx += 1) {
                      val.سیٹ_ثنائی_ہندسہ(idx, (tmp&1)? BIT4_1 : BIT4_0);

                      اگر (idx == 31)
                            tmp = vp->value.time->بلند;
                      ورنہ
                            tmp >>= 1;
                }
                توڑو;
          }

	  حالت vpi_سمتیہ_Val:

	    برائے (غیرعلامتی wdx = 0 ;  wdx < vwid_ ;  wdx += 32) {
		  غیرعلامتی لفظ = wdx / 32;
		  غیرعلامتی طویل aval = vp->value.vector[لفظ].aval;
		  غیرعلامتی طویل bval = vp->value.vector[لفظ].bval;

		  برائے (غیرعلامتی idx = 0 ;  (wdx+idx) < vwid_ && idx < 32;
		       idx += 1) {
			صحیح ثنائی_ہندسہ = (aval&1) | ((bval<<1)&2);
			vvp_bit4_t bit4;

			انتخاب (ثنائی_ہندسہ) {
			    حالت 0:
			      bit4 = BIT4_0;
			      توڑو;
			    حالت 1:
			      bit4 = BIT4_1;
			      توڑو;
			    حالت 2:
			      bit4 = BIT4_Z;
			      توڑو;
			    حالت 3:
			      bit4 = BIT4_X;
			      توڑو;
			    طےشدہ:
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
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "XXXX format=%d, vwid_=%u\n", (صحیح)vp->format, vwid_);
	    أكد(0);
      }

      fnet->send_vec4(val, vthread_get_wt_context());
      واپس 0;
}

ساخت sysfunc_rnet : عوامی sysfunc_def {
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags) override;
};

vpiHandle sysfunc_rnet::vpi_put_قدر(p_vpi_قدر vp, صحیح)
{
      put_قدر = سچ;

      دوہرا value;
      انتخاب (vp->format) {

	  حالت vpiRealVal:
	    value = vp->value.real;
	    توڑو;

	  طےشدہ:
	    value = 0.0;
	    fprintf(stderr, "Unsupported format %d.\n", (صحیح)vp->format);
	    أكد(0);
      }

      fnet->send_real(value, vthread_get_wt_context());

      واپس 0;
}

ساخت sysfunc_no : عوامی sysfunc_def {
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags) override;
};

vpiHandle sysfunc_no::vpi_put_قدر(p_vpi_قدر, صحیح)
{
      واپس 0;
}


  /* **** Manipulate the internal data structures. **** */

/*
 * We keep a table of all the __vpiUserSystf objects that are created
 * so that the user can iterate over them. The def_table is an array
 * of pointers to __vpiUserSystf objects. This table can be searched
 * by name using the vpi_find_systf function, and they can be
 * collected into an iterator using the vpip_make_systf_iterator function.
 */
ساکن ساخت __vpiUserSystf**def_جدول = 0;
ساکن غیرعلامتی def_گنتی = 0;

ساکن ساخت __vpiUserSystf* allocate_def(خالی)
{
      اگر (def_جدول == 0) {
	    def_جدول = static_cast<ساخت __vpiUserSystf**>
	                (حافظہ_دو(حجم(ساخت __vpiUserSystf*)));

	    def_جدول[0] = نیا __vpiUserSystf;

	    def_گنتی = 1;
	    واپس def_جدول[0];
      }

      def_جدول = static_cast<ساخت __vpiUserSystf**>
                  (realloc(def_جدول,
                   (def_گنتی+1)*حجم(ساخت __vpiUserSystf*)));

      def_جدول[def_گنتی] = نیا __vpiUserSystf;

      واپس def_جدول[def_گنتی++];
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی def_جدول_delete(خالی)
{
      برائے (غیرعلامتی idx = 0; idx < def_گنتی; idx += 1) {
	    آزاد(const_cast<حرف *>(def_جدول[idx]->info.tfname));
	    حذف def_جدول[idx];
      }
      آزاد(def_جدول);
      def_جدول = 0;
      def_گنتی = 0;
}
#اگر_ختم

جماعت __vpiSystfIterator : عوامی __vpiHandle {
    عوامی:
      صریح __vpiSystfIterator(غیرعلامتی idx);
      صحیح get_type_رمز(خالی) ثابت override;
      vpiHandle vpi_اشاریہ(صحیح idx) override;
      free_object_fun_t free_object_fun(خالی) override;

      غیرعلامتی اگلا_2;
};

ساکن vpiHandle systf_iterator_جائزہ(vpiHandle ref, صحیح)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);

      اگر (obj->اگلا_2 >= def_گنتی) {
	    vpi_free_object(ref);
	    واپس 0;
      }

      غیرعلامتی use_اشاریہ = obj->اگلا_2;
      جب (!def_جدول[use_اشاریہ]->is_user_defn) {
	    obj->اگلا_2 += 1;
	    use_اشاریہ = obj->اگلا_2;
	    اگر (obj->اگلا_2 >= def_گنتی) {
		  vpi_free_object(ref);
		  واپس 0;
	    }
      }
      obj->اگلا_2 += 1;
      واپس def_جدول[use_اشاریہ];
}

ساکن صحیح systf_iterator_free_object(vpiHandle ref)
{
      __vpiSystfIterator*obj = dynamic_cast<__vpiSystfIterator*>(ref);
      حذف obj;
      واپس 1;
}

اندرونی __vpiSystfIterator::__vpiSystfIterator(غیرعلامتی idx)
{ اگلا_2 = idx; }

صحیح __vpiSystfIterator::get_type_رمز(خالی) ثابت
{ واپس vpiIterator; }

vpiHandle __vpiSystfIterator::vpi_اشاریہ(صحیح idx)
{ واپس systf_iterator_جائزہ(یہ, idx); }

__vpiHandle::free_object_fun_t __vpiSystfIterator::free_object_fun(خالی)
{ واپس &systf_iterator_free_object; }

vpiHandle vpip_make_systf_iterator(خالی)
{
	/* Check to see if there are any user defined functions. */
      منطقی have_user_defn = جھوٹ;
      غیرعلامتی idx;
      برائے (idx = 0; idx < def_گنتی; idx += 1) {
	    اگر (def_جدول[idx]->is_user_defn) {
		  have_user_defn = سچ;
		  توڑو;
	    }
      }
      اگر (!have_user_defn) واپس 0;

      __vpiSystfIterator*res = نیا __vpiSystfIterator(idx);
      res->اگلا_2 = idx;
      واپس res;
}

ساخت __vpiUserSystf* vpip_ڈھونڈو_systf(ثابت حرف*name)
{
      برائے (غیرعلامتی idx = 0 ;  idx < def_گنتی ;  idx += 1)
	    اگر (strcmp(def_جدول[idx]->info.tfname, name) == 0)
		  واپس def_جدول[idx];

      واپس 0;
}

خالی vpip_make_systf_system_defined(vpiHandle ref)
{
      أكد(ref);
      ساخت __vpiUserSystf*obj = dynamic_cast<__vpiUserSystf*>(ref);
      أكد(obj);
      obj->is_user_defn = جھوٹ;
}

/*
 * To get better error message we need to cache the vpi_call fail
 * information so that we can print the file name.
 */
شمار vpi_call_error_type {VPI_CALL_NO_DEF, VPI_CALL_TASK_AS_FUNC,
                          VPI_CALL_FUNC_AS_TASK, VPI_CALL_FUNC_AS_TASK_WARN};
قسمتعریف ساخت vpi_call_error {
      vpi_call_error_type type;
      حرف *name;
      طویل file_idx;
      طویل lineno;
} vpi_call_error_s, *vpi_call_error_p;

ساکن vpi_call_error_p vpi_call_error_lst = تہی;
ساکن غیرعلامتی vpi_call_error_عدد = 0;

ساکن خالی جوڑو_vpi_call_error(vpi_call_error_type type, ثابت حرف *name,
                               طویل file_idx, طویل lineno)
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

خالی print_vpi_call_errors()
{
      برائے (غیرعلامتی idx = 0; idx < vpi_call_error_عدد; idx += 1) {
	    انتخاب (vpi_call_error_lst[idx].type) {
		حالت VPI_CALL_NO_DEF:
		  fprintf(stderr, "%s:%d: Error: System task/function %s() is "
		                  "not defined by any module.\n",
		                  file_نام[vpi_call_error_lst[idx].file_idx],
		                  (صحیح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  توڑو;
		حالت VPI_CALL_TASK_AS_FUNC:
		  fprintf(stderr, "%s:%d: Error: %s() is a system task, it "
			          "cannot be called as a function.\n",
		                  file_نام[vpi_call_error_lst[idx].file_idx],
		                  (صحیح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  توڑو;
		حالت VPI_CALL_FUNC_AS_TASK:
		  fprintf(stderr, "%s:%d: Error: %s() is a system function, it "
			          "cannot be called as a task.\n",
		                  file_نام[vpi_call_error_lst[idx].file_idx],
		                  (صحیح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
		  توڑو;
		حالت VPI_CALL_FUNC_AS_TASK_WARN:
                 fprintf(stderr, "%s:%d: Warning: Calling system function "
                                 "%s() as a task.\n",
		                  file_نام[vpi_call_error_lst[idx].file_idx],
		                  (صحیح)vpi_call_error_lst[idx].lineno,
		                  vpi_call_error_lst[idx].name);
                 fprintf(stderr, "%s:%d:          The functions return "
                                 "value will be ignored.\n",
                                 file_نام[vpi_call_error_lst[idx].file_idx],
                                 (صحیح)vpi_call_error_lst[idx].lineno);
		  توڑو;
	    }
	    آزاد(vpi_call_error_lst[idx].name);
      }
      آزاد(vpi_call_error_lst);
      fflush(stderr);
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
ساکن خالی cleanup_vpi_call_دلائل(غیرعلامتی argc, vpiHandle*argv)
{
      اگر (argc) {
	      /* Since this is just being used to cleanup the arguments a
	       * system task definition can be used. */
	    ساخت __vpiSysTaskCall*obj = نیا systask_def;
	    obj->nargs = argc;
	    obj->دلائل  = argv;
	    vpi_call_delete(obj);
      }
}
#اگر_ختم

/*
 * A vpi_call is actually built up into a vpiSysTaskCall VPI object
 * that refers back to the vpiUserSystf VPI object that is the
 * definition. So this function is called by the compiler when a
 * %vpi_call statement is encountered. Create here a vpiHandle that
 * describes the call, and return it. The %vpi_call instruction will
 * store this handle for when it is executed.
 */
vpiHandle vpip_build_vpi_call(ثابت حرف*name, صحیح val_رمز, غیرعلامتی return_چوڑائی,
			      vvp_net_t*fnet,
			      منطقی func_as_task_err, منطقی func_as_task_warn,
			      غیرعلامتی argc, vpiHandle*argv,
			      غیرعلامتی vec4_ڈھیر, غیرعلامتی real_ڈھیر, غیرعلامتی string_ڈھیر,
			      طویل file_idx, طویل lineno)
{
      أكد(!(func_as_task_err && func_as_task_warn));

      ساخت __vpiUserSystf*defn = vpip_ڈھونڈو_systf(name);
      اگر (defn == 0) {
	    جوڑو_vpi_call_error(VPI_CALL_NO_DEF, name, file_idx, lineno);
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
	    cleanup_vpi_call_دلائل(argc, argv);
#اگر_ختم
	    واپس 0;
      }

      انتخاب (defn->info.type) {
	  حالت vpiSysTask:
	    اگر (val_رمز != 0 || fnet != 0) {
		  جوڑو_vpi_call_error(VPI_CALL_TASK_AS_FUNC, name, file_idx,
		                     lineno);
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
		  cleanup_vpi_call_دلائل(argc, argv);
#اگر_ختم
		  واپس 0;
	    }
	    توڑو;

	  حالت vpiSysFunc:
	    اگر (val_رمز == 0 && fnet == 0) {
		  اگر (func_as_task_err) {
			جوڑو_vpi_call_error(VPI_CALL_FUNC_AS_TASK,
			                   name, file_idx, lineno);
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
			cleanup_vpi_call_دلائل(argc, argv);
#اگر_ختم
			واپس 0;
		  } ورنہ اگر (func_as_task_warn) {
			جوڑو_vpi_call_error(VPI_CALL_FUNC_AS_TASK_WARN,
			                   name, file_idx, lineno);
		  }
	    }
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "Unsupported vpi_call type %d.\n",
	                    (صحیح)defn->info.type);
	    أكد(0);
      }

      ساخت __vpiSysTaskCall*obj = 0;

      انتخاب (defn->info.type) {
	  حالت vpiSysTask:
	    obj = نیا systask_def;
	    توڑو;

	  حالت vpiSysFunc:
	    اگر (fnet && val_رمز == -vpiRealVal) {
		  obj = نیا sysfunc_rnet;

	    } ورنہ اگر (fnet && val_رمز == -vpi_سمتیہ_Val) {
		  obj = نیا sysfunc_4net(return_چوڑائی);

	    } ورنہ اگر (val_رمز == -vpiRealVal) {
		  obj = نیا sysfunc_real;

	    } ورنہ اگر (val_رمز == -vpi_سمتیہ_Val) {
		  obj = نیا sysfunc_vec4(return_چوڑائی);

	    } ورنہ اگر (val_رمز == -vpiStringVal) {
		  obj = نیا sysfunc_str;

           } ورنہ اگر (val_رمز == 0 && fnet == 0) {
		  obj = نیا sysfunc_no;

	    } ورنہ {
		  fprintf(stderr, "XXXX fnet=%p, val_code=%d\n", fnet, val_رمز);
		  أكد(0);
	    }
	    توڑو;
      }

      obj->scope = vpip_peek_current_scope();
      obj->defn  = defn;
      obj->nargs = argc;
      obj->دلائل  = argv;
      obj->vec4_ڈھیر = vec4_ڈھیر;
      obj->real_ڈھیر = real_ڈھیر;
      obj->string_ڈھیر = string_ڈھیر;
      obj->fnet  = fnet;
      obj->file_idx  = (غیرعلامتی) file_idx;
      obj->lineno   = (غیرعلامتی) lineno;
      obj->userdata  = 0;
      obj->put_قدر = جھوٹ;

      compile_compiletf(obj);

      واپس obj;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی vpi_call_delete(vpiHandle شے)
{
      ساخت __vpiSysTaskCall*obj = dynamic_cast<__vpiSysTaskCall*>(شے);
	/* The object can be NULL if there was an error. */
      اگر (!obj) واپس;
      برائے (غیرعلامتی arg = 0; arg < obj->nargs; arg += 1) {
	    انتخاب (obj->دلائل[arg]->get_type_رمز()) {
		حالت vpiConstant:
		  انتخاب (vpi_get(_vpiFromThr, obj->دلائل[arg])) {
		      حالت _vpiNoThr:
			constant_delete(obj->دلائل[arg]);
			توڑو;
		      حالت _vpiString:
			thread_string_delete(obj->دلائل[arg]);
			توڑو;
		      حالت _vpiVThr:
			thread_vthr_delete(obj->دلائل[arg]);
			توڑو;
		      حالت vpi_لفظ:
			thread_لفظ_delete(obj->دلائل[arg]);
			توڑو;
		      طےشدہ:
			أكد(0);
		  }
		  توڑو;
		حالت vpi_حافظہ_لفظ:
		  انتخاب (vpi_get(_vpiFromThr, obj->دلائل[arg])) {
		      حالت _vpi_at_A:
			A_delete(obj->دلائل[arg]);
			توڑو;
		  }
		  توڑو;
		حالت vpiPartSelect:
		  انتخاب (vpi_get(_vpiFromThr, obj->دلائل[arg])) {
		      حالت _vpi_at_PV:
			PV_delete(obj->دلائل[arg]);
			توڑو;
		      حالت _vpi_at_APV:
			APV_delete(obj->دلائل[arg]);
			توڑو;
		  }
		  توڑو;
	    }
      }
      آزاد(obj->دلائل);
      حذف obj;
}
#اگر_ختم

/*
 * This function is used by the %vpi_call instruction to actually
 * place the call to the system task/function. For now, only support
 * calls to system tasks.
 */

vthread_t vpip_current_vthread;

خالی vpip_execute_vpi_call(vthread_t thr, vpiHandle ref)
{
      vpip_current_vthread = thr;

      vpip_cur_task = dynamic_cast<__vpiSysTaskCall*>(ref);

      اگر (vpip_cur_task->defn->info.calltf) {
	    أكد(vpi_منوال_flag == VPI_منوال_NONE);
	    vpi_منوال_flag = VPI_منوال_CALLTF;
	    vpip_cur_task->put_قدر = جھوٹ;
	    vpip_cur_task->defn->info.calltf(vpip_cur_task->defn->info.user_ڈیٹا);
	    vpi_منوال_flag = VPI_منوال_NONE;
	      /* If the function call did not set a value then put a
	       * default value (0). */
	    اگر (ref->get_type_رمز() == vpiSysFuncCall &&
	        !vpip_cur_task->put_قدر) {
		  s_vpi_قدر val;
		  val.format = vpiIntVal;
		  val.value.integer = 0;
		  vpi_put_قدر(ref, &val, 0, vpiNoDelay);
	    }
      }
      اگر (vpip_cur_task->vec4_ڈھیر > 0)
	    vthread_نکالو_vec4(thr, vpip_cur_task->vec4_ڈھیر);
      اگر (vpip_cur_task->real_ڈھیر > 0)
	    vthread_نکالو_real(thr, vpip_cur_task->real_ڈھیر);
      اگر (vpip_cur_task->string_ڈھیر > 0)
	    vthread_نکالو_str(thr, vpip_cur_task->string_ڈھیر);

	/* If the function returns a value, then push the value
	   to the appropriate thread stack. */
      اگر (ثابت sysfunc_real*func_real = dynamic_cast<sysfunc_real*>(ref)) {
	    vthread_ڈالو(thr, func_real->return_قدر());
      }
      ورنہ اگر (ثابت sysfunc_str*func_string = dynamic_cast<sysfunc_str*>(ref)) {
	    vthread_ڈالو(thr, func_string->return_قدر());
      }
      ورنہ اگر (ثابت sysfunc_vec4*func_vec4 = dynamic_cast<sysfunc_vec4*>(ref)) {
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
vpiHandle vpi_رجسٹر_systf(ثابت ساخت t_vpi_systf_ڈیٹا*ss)
{
      ساخت __vpiUserSystf*cur = allocate_def();
      أكد(ss);
      انتخاب (ss->type) {
	  حالت vpiSysTask:
	  حالت vpiSysFunc:
	    توڑو;
	  طےشدہ:
	    fprintf(stderr, "Unsupported type %d.\n", (صحیح)ss->type);
	    أكد(0);
      }

      cur->info = *ss;
      cur->info.tfname = strdup(ss->tfname);
      cur->is_user_defn = سچ;

      واپس cur;
}

PLI_INT32 vpi_put_userdata(vpiHandle ref, خالی*ڈیٹا)
{
      ساخت __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      اگر (rfp == 0)
	    واپس 0;

      rfp->userdata = ڈیٹا;
      واپس 1;
}

خالی* vpi_get_userdata(vpiHandle ref)
{
      ساخت __vpiSysTaskCall*rfp = dynamic_cast<__vpiSysTaskCall*>(ref);
      أكد(rfp);

      واپس rfp->userdata;
}
