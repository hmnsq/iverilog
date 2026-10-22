/*
 * Copyright (c) 2014-2025 Stephen Williams (steve@icarus.com)
 * Copyright (c) 2014 CERN
 * @author Maciej Suminski <maciej.suminski@cern.ch>
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

#شامل "array_common.h"

vpiHandle __vpiArrayBase::vpi_array_بنیاد_iterate(صحیح code)
{
    انتخاب (code) {
        حالت vpiReg:
        حالت vpi_حافظہ_لفظ: {
                ساخت __vpiArrayIterator*res;
                res = نیا __vpiArrayIterator;
                res->array = یہ;
                res->اگلا_2 = 0;
                واپس res;
        }
    }

    واپس 0;
}

خالی __vpiArrayBase::make_vals_الفاظ()
{
    أكد(vals_الفاظ == 0);
    vals_الفاظ = نیا ساخت __vpiArrayWord[get_سائز() + 1];

    // Make word[-1] point to the parent.
    vals_الفاظ->والد = یہ;
    // Now point to word-0
    vals_الفاظ += 1;

    ساخت __vpiArrayWord*الفاظ = vals_الفاظ;
    برائے (غیرعلامتی idx = 0 ; idx < get_سائز() ; idx += 1) {
            الفاظ[idx].word0 = الفاظ;
    }
}

vpiHandle __vpiArrayIterator::vpi_اشاریہ(صحیح)
{
      اگر (اگلا_2 >= array->get_سائز()) {
	    vpi_free_object(یہ);
	    واپس 0;
      }

      غیرعلامتی use_اشاریہ = اگلا_2;
      اگلا_2 += 1;

      واپس array->get_iter_اشاریہ(یہ, use_اشاریہ);
}

ساکن صحیح array_iterator_free_object(vpiHandle ref)
{
      ساخت __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      حذف obj;
      واپس 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(خالی)
{ واپس &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(صحیح code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      اگر (code == vpi_اشاریہ_2) {
	    ساخت __vpiArrayIndex*res;
	    res = نیا __vpiArrayIndex;
	    res->اشاریہ = اشاریہ;         // see the comment above
	    res->مکمل = 0;
	    واپس res;
      }
      واپس 0;
}

vpiHandle __vpiArrayIndex::vpi_اشاریہ(صحیح)
{
      اگر (مکمل == 0) {
            مکمل = 1;
            واپس اشاریہ;
      }

      vpi_free_object(یہ);
      واپس 0;
}

ساکن صحیح array_اشاریہ_free_object(vpiHandle ref)
{
      ساخت __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      حذف obj;
      واپس 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(خالی)
{ واپس &array_اشاریہ_free_object; }

حرف* __vpiArrayWord::as_لفظ_t::vpi_get_str(صحیح code)
{
      ساخت __vpiArrayWord*obj = array_var_لفظ_from_handle(یہ);
      أكد(obj);
      ساخت __vpiArrayBase*my_والد = obj->get_والد();
      واپس my_والد->get_لفظ_str(obj, code);
}

خالی __vpiArrayWord::as_لفظ_t::vpi_get_قدر(p_vpi_قدر vp)
{
      ساخت __vpiArrayWord*obj = array_var_لفظ_from_handle(یہ);
      أكد(obj);
      ساخت __vpiArrayBase*my_والد = obj->get_والد();
      واپس my_والد->get_لفظ_قدر(obj, vp);
}

vpiHandle __vpiArrayWord::as_لفظ_t::vpi_put_قدر(p_vpi_قدر vp, صحیح flags)
{
      ساخت __vpiArrayWord*obj = array_var_لفظ_from_handle(یہ);
      أكد(obj);
      ساخت __vpiArrayBase*my_والد = obj->get_والد();
      my_والد->put_لفظ_قدر(obj, vp, flags);
      واپس یہ;
}

vpiHandle __vpiArrayWord::as_لفظ_t::vpi_handle(صحیح code)
{
      ساخت __vpiArrayWord*obj = array_var_لفظ_from_handle(یہ);
      أكد(obj);
      ساخت __vpiArrayBase*my_والد = obj->get_والد();

      انتخاب (code) {

	  حالت vpi_اشاریہ_2:
	    واپس &(obj->as_اشاریہ);

	  حالت vpi_بایاں_Range:
	    واپس my_والد->get_بایاں_range();

	  حالت vpi_دایاں_Range:
	    واپس my_والد->get_دایاں_range();

	  حالت vpi_والد:
	    واپس dynamic_cast<vpiHandle>(my_والد);

	  حالت vpiScope:
	    واپس my_والد->get_scope();

	  حالت vpiModule:
	    واپس vpip_module(my_والد->get_scope());
      }

      واپس 0;
}

خالی __vpiArrayWord::as_اشاریہ_t::vpi_get_قدر(p_vpi_قدر vp)
{
      ساخت __vpiArrayWord*obj = array_var_اشاریہ_from_handle(یہ);
      أكد(obj);
      غیرعلامتی اشاریہ = obj->get_اشاریہ();

      أكد(vp->format == vpiIntVal);
      vp->value.integer = اشاریہ;
}

ساخت __vpiArrayWord*array_var_لفظ_from_handle(vpiHandle ref)
{
      اگر (ref == 0)
	    واپس 0;
      ثابت __vpiArrayWord::as_لفظ_t*ptr = dynamic_cast<__vpiArrayWord::as_لفظ_t*> (ref);
      اگر (ptr == 0)
	    واپس 0;

      واپس (ساخت __vpiArrayWord*)(ref);
}

ساخت __vpiArrayWord* array_var_اشاریہ_from_handle(vpiHandle ref)
{
      اگر (ref == 0)
	    واپس 0;
      ثابت __vpiArrayWord::as_اشاریہ_t*ptr = dynamic_cast<__vpiArrayWord::as_اشاریہ_t*> (ref);
      اگر (ptr == 0)
	    واپس 0;

      أكد(حجم(__vpiHandle) == حجم(__vpiArrayWord::as_اشاریہ_t));
      أكد(حجم(__vpiHandle) == حجم(__vpiArrayWord::as_لفظ_t));
      واپس (ساخت __vpiArrayWord*)(ref-1);
}

