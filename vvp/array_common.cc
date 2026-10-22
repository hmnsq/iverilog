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

#ضمن "array_common.h"

vpiHandle __vpiArrayBase::vpi_array_بنياد_iterate(صحيح code)
{
    بدل (code) {
        حالة vpiReg:
        حالة vpi_يادگيري_لفظ: {
                بنية __vpiArrayIterator*res;
                res = جديد __vpiArrayIterator;
                res->array = هذا;
                res->ايندڙ_2 = 0;
                أرجع res;
        }
    }

    أرجع 0;
}

فراغ __vpiArrayBase::make_vals_لفظ()
{
    أكد(vals_لفظ == 0);
    vals_لفظ = جديد بنية __vpiArrayWord[get_سائيز() + 1];

    // Make word[-1] point to the parent.
    vals_لفظ->والد = هذا;
    // Now point to word-0
    vals_لفظ += 1;

    بنية __vpiArrayWord*لفظ = vals_لفظ;
    لكل (بنانشان idx = 0 ; idx < get_سائيز() ; idx += 1) {
            لفظ[idx].word0 = لفظ;
    }
}

vpiHandle __vpiArrayIterator::vpi_اشاريو(صحيح)
{
      إذا (ايندڙ_2 >= array->get_سائيز()) {
	    vpi_free_object(هذا);
	    أرجع 0;
      }

      بنانشان use_اشاريو = ايندڙ_2;
      ايندڙ_2 += 1;

      أرجع array->get_iter_اشاريو(هذا, use_اشاريو);
}

ساكن صحيح array_iterator_free_object(vpiHandle ref)
{
      بنية __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      ميٽيو obj;
      أرجع 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(فراغ)
{ أرجع &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(صحيح code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      إذا (code == vpi_اشاريو_2) {
	    بنية __vpiArrayIndex*res;
	    res = جديد __vpiArrayIndex;
	    res->اشاريو = اشاريو;         // see the comment above
	    res->پورو = 0;
	    أرجع res;
      }
      أرجع 0;
}

vpiHandle __vpiArrayIndex::vpi_اشاريو(صحيح)
{
      إذا (پورو == 0) {
            پورو = 1;
            أرجع اشاريو;
      }

      vpi_free_object(هذا);
      أرجع 0;
}

ساكن صحيح array_اشاريو_free_object(vpiHandle ref)
{
      بنية __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      ميٽيو obj;
      أرجع 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(فراغ)
{ أرجع &array_اشاريو_free_object; }

حرف* __vpiArrayWord::as_لفظ_t::vpi_get_str(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_لفظ_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_والد = obj->get_والد();
      أرجع my_والد->get_لفظ_str(obj, code);
}

فراغ __vpiArrayWord::as_لفظ_t::vpi_get_قدر(p_vpi_قدر vp)
{
      بنية __vpiArrayWord*obj = array_var_لفظ_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_والد = obj->get_والد();
      أرجع my_والد->get_لفظ_قدر(obj, vp);
}

vpiHandle __vpiArrayWord::as_لفظ_t::vpi_put_قدر(p_vpi_قدر vp, صحيح flags)
{
      بنية __vpiArrayWord*obj = array_var_لفظ_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_والد = obj->get_والد();
      my_والد->put_لفظ_قدر(obj, vp, flags);
      أرجع هذا;
}

vpiHandle __vpiArrayWord::as_لفظ_t::vpi_handle(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_لفظ_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_والد = obj->get_والد();

      بدل (code) {

	  حالة vpi_اشاريو_2:
	    أرجع &(obj->as_اشاريو);

	  حالة vpi_کاٻو_Range:
	    أرجع my_والد->get_کاٻو_range();

	  حالة vpi_ساڄو_Range:
	    أرجع my_والد->get_ساڄو_range();

	  حالة vpi_والد:
	    أرجع dynamic_cast<vpiHandle>(my_والد);

	  حالة vpiScope:
	    أرجع my_والد->get_scope();

	  حالة vpiModule:
	    أرجع vpip_module(my_والد->get_scope());
      }

      أرجع 0;
}

فراغ __vpiArrayWord::as_اشاريو_t::vpi_get_قدر(p_vpi_قدر vp)
{
      بنية __vpiArrayWord*obj = array_var_اشاريو_from_handle(هذا);
      أكد(obj);
      بنانشان اشاريو = obj->get_اشاريو();

      أكد(vp->format == vpiIntVal);
      vp->value.integer = اشاريو;
}

بنية __vpiArrayWord*array_var_لفظ_from_handle(vpiHandle ref)
{
      إذا (ref == 0)
	    أرجع 0;
      ثابت __vpiArrayWord::as_لفظ_t*ptr = dynamic_cast<__vpiArrayWord::as_لفظ_t*> (ref);
      إذا (ptr == 0)
	    أرجع 0;

      أرجع (بنية __vpiArrayWord*)(ref);
}

بنية __vpiArrayWord* array_var_اشاريو_from_handle(vpiHandle ref)
{
      إذا (ref == 0)
	    أرجع 0;
      ثابت __vpiArrayWord::as_اشاريو_t*ptr = dynamic_cast<__vpiArrayWord::as_اشاريو_t*> (ref);
      إذا (ptr == 0)
	    أرجع 0;

      أكد(حجم(__vpiHandle) == حجم(__vpiArrayWord::as_اشاريو_t));
      أكد(حجم(__vpiHandle) == حجم(__vpiArrayWord::as_لفظ_t));
      أرجع (بنية __vpiArrayWord*)(ref-1);
}

