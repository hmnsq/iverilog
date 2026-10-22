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

vpiHandle __vpiArrayBase::vpi_array_پایه_iterate(صحيح code)
{
    بدل (code) {
        حالة vpiReg:
        حالة vpi_حافظه_کلمه: {
                بنية __vpiArrayIterator*res;
                res = جديد __vpiArrayIterator;
                res->array = هذا;
                res->بعدی_2 = 0;
                أرجع res;
        }
    }

    أرجع 0;
}

فراغ __vpiArrayBase::make_vals_واژگان()
{
    أكد(vals_واژگان == 0);
    vals_واژگان = جديد بنية __vpiArrayWord[get_اندازه() + 1];

    // Make word[-1] point to the parent.
    vals_واژگان->والد = هذا;
    // Now point to word-0
    vals_واژگان += 1;

    بنية __vpiArrayWord*واژگان = vals_واژگان;
    لكل (بیعلامت idx = 0 ; idx < get_اندازه() ; idx += 1) {
            واژگان[idx].word0 = واژگان;
    }
}

vpiHandle __vpiArrayIterator::vpi_شاخص(صحيح)
{
      إذا (بعدی_2 >= array->get_اندازه()) {
	    vpi_free_object(هذا);
	    أرجع 0;
      }

      بیعلامت use_شاخص = بعدی_2;
      بعدی_2 += 1;

      أرجع array->get_iter_شاخص(هذا, use_شاخص);
}

ساكن صحيح array_iterator_free_object(vpiHandle ref)
{
      بنية __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      حذف obj;
      أرجع 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(فراغ)
{ أرجع &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(صحيح code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      إذا (code == vpi_شاخص_2) {
	    بنية __vpiArrayIndex*res;
	    res = جديد __vpiArrayIndex;
	    res->شاخص = شاخص;         // see the comment above
	    res->انجامشده = 0;
	    أرجع res;
      }
      أرجع 0;
}

vpiHandle __vpiArrayIndex::vpi_شاخص(صحيح)
{
      إذا (انجامشده == 0) {
            انجامشده = 1;
            أرجع شاخص;
      }

      vpi_free_object(هذا);
      أرجع 0;
}

ساكن صحيح array_شاخص_free_object(vpiHandle ref)
{
      بنية __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      حذف obj;
      أرجع 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(فراغ)
{ أرجع &array_شاخص_free_object; }

حرف* __vpiArrayWord::as_کلمه_t::vpi_get_str(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_کلمه_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_والد = obj->get_والد();
      أرجع my_والد->get_کلمه_str(obj, code);
}

فراغ __vpiArrayWord::as_کلمه_t::vpi_get_مقدار(p_vpi_مقدار vp)
{
      بنية __vpiArrayWord*obj = array_var_کلمه_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_والد = obj->get_والد();
      أرجع my_والد->get_کلمه_مقدار(obj, vp);
}

vpiHandle __vpiArrayWord::as_کلمه_t::vpi_put_مقدار(p_vpi_مقدار vp, صحيح flags)
{
      بنية __vpiArrayWord*obj = array_var_کلمه_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_والد = obj->get_والد();
      my_والد->put_کلمه_مقدار(obj, vp, flags);
      أرجع هذا;
}

vpiHandle __vpiArrayWord::as_کلمه_t::vpi_handle(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_کلمه_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_والد = obj->get_والد();

      بدل (code) {

	  حالة vpi_شاخص_2:
	    أرجع &(obj->as_شاخص);

	  حالة vpi_چپ_Range:
	    أرجع my_والد->get_چپ_range();

	  حالة vpi_راست_Range:
	    أرجع my_والد->get_راست_range();

	  حالة vpi_والد:
	    أرجع dynamic_cast<vpiHandle>(my_والد);

	  حالة vpiScope:
	    أرجع my_والد->get_scope();

	  حالة vpiModule:
	    أرجع vpip_module(my_والد->get_scope());
      }

      أرجع 0;
}

فراغ __vpiArrayWord::as_شاخص_t::vpi_get_مقدار(p_vpi_مقدار vp)
{
      بنية __vpiArrayWord*obj = array_var_شاخص_from_handle(هذا);
      أكد(obj);
      بیعلامت شاخص = obj->get_شاخص();

      أكد(vp->format == vpiIntVal);
      vp->value.integer = شاخص;
}

بنية __vpiArrayWord*array_var_کلمه_from_handle(vpiHandle ref)
{
      إذا (ref == 0)
	    أرجع 0;
      ثابت __vpiArrayWord::as_کلمه_t*ptr = dynamic_cast<__vpiArrayWord::as_کلمه_t*> (ref);
      إذا (ptr == 0)
	    أرجع 0;

      أرجع (بنية __vpiArrayWord*)(ref);
}

بنية __vpiArrayWord* array_var_شاخص_from_handle(vpiHandle ref)
{
      إذا (ref == 0)
	    أرجع 0;
      ثابت __vpiArrayWord::as_شاخص_t*ptr = dynamic_cast<__vpiArrayWord::as_شاخص_t*> (ref);
      إذا (ptr == 0)
	    أرجع 0;

      أكد(حجم(__vpiHandle) == حجم(__vpiArrayWord::as_شاخص_t));
      أكد(حجم(__vpiHandle) == حجم(__vpiArrayWord::as_کلمه_t));
      أرجع (بنية __vpiArrayWord*)(ref-1);
}

