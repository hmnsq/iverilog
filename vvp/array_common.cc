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

vpiHandle __vpiArrayBase::vpi_array_بنسټ_iterate(صحيح code)
{
    بدل (code) {
        حالة vpiReg:
        حالة vpi_حافظه_کلمه: {
                بنية __vpiArrayIterator*res;
                res = جديد __vpiArrayIterator;
                res->array = هذا;
                res->بل_2 = 0;
                أرجع res;
        }
    }

    أرجع 0;
}

فراغ __vpiArrayBase::make_vals_ټکي()
{
    أكد(vals_ټکي == 0);
    vals_ټکي = جديد بنية __vpiArrayWord[get_اندازه() + 1];

    // Make word[-1] point to the parent.
    vals_ټکي->پلار = هذا;
    // Now point to word-0
    vals_ټکي += 1;

    بنية __vpiArrayWord*ټکي = vals_ټکي;
    لكل (بینښه idx = 0 ; idx < get_اندازه() ; idx += 1) {
            ټکي[idx].word0 = ټکي;
    }
}

vpiHandle __vpiArrayIterator::vpi_شاخص(صحيح)
{
      إذا (بل_2 >= array->get_اندازه()) {
	    vpi_free_object(هذا);
	    أرجع 0;
      }

      بینښه use_شاخص = بل_2;
      بل_2 += 1;

      أرجع array->get_iter_شاخص(هذا, use_شاخص);
}

ساكن صحيح array_iterator_free_object(vpiHandle ref)
{
      بنية __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      ړنګول obj;
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
	    res->بشپړ = 0;
	    أرجع res;
      }
      أرجع 0;
}

vpiHandle __vpiArrayIndex::vpi_شاخص(صحيح)
{
      إذا (بشپړ == 0) {
            بشپړ = 1;
            أرجع شاخص;
      }

      vpi_free_object(هذا);
      أرجع 0;
}

ساكن صحيح array_شاخص_free_object(vpiHandle ref)
{
      بنية __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      ړنګول obj;
      أرجع 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(فراغ)
{ أرجع &array_شاخص_free_object; }

حرف* __vpiArrayWord::as_کلمه_t::vpi_get_str(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_کلمه_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_پلار = obj->get_پلار();
      أرجع my_پلار->get_کلمه_str(obj, code);
}

فراغ __vpiArrayWord::as_کلمه_t::vpi_get_ارزښت(p_vpi_ارزښت vp)
{
      بنية __vpiArrayWord*obj = array_var_کلمه_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_پلار = obj->get_پلار();
      أرجع my_پلار->get_کلمه_ارزښت(obj, vp);
}

vpiHandle __vpiArrayWord::as_کلمه_t::vpi_put_ارزښت(p_vpi_ارزښت vp, صحيح flags)
{
      بنية __vpiArrayWord*obj = array_var_کلمه_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_پلار = obj->get_پلار();
      my_پلار->put_کلمه_ارزښت(obj, vp, flags);
      أرجع هذا;
}

vpiHandle __vpiArrayWord::as_کلمه_t::vpi_handle(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_کلمه_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_پلار = obj->get_پلار();

      بدل (code) {

	  حالة vpi_شاخص_2:
	    أرجع &(obj->as_شاخص);

	  حالة vpi_کیڼ_Range:
	    أرجع my_پلار->get_کیڼ_range();

	  حالة vpi_ښی_Range:
	    أرجع my_پلار->get_ښی_range();

	  حالة vpi_پلار:
	    أرجع dynamic_cast<vpiHandle>(my_پلار);

	  حالة vpiScope:
	    أرجع my_پلار->get_scope();

	  حالة vpiModule:
	    أرجع vpip_module(my_پلار->get_scope());
      }

      أرجع 0;
}

فراغ __vpiArrayWord::as_شاخص_t::vpi_get_ارزښت(p_vpi_ارزښت vp)
{
      بنية __vpiArrayWord*obj = array_var_شاخص_from_handle(هذا);
      أكد(obj);
      بینښه شاخص = obj->get_شاخص();

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

