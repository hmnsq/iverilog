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

vpiHandle __vpiArrayBase::vpi_array_base_iterate(صحيح code)
{
    بدل (code) {
        حالة vpiReg:
        حالة vpiMemoryWord: {
                بنية __vpiArrayIterator*res;
                res = جديد __vpiArrayIterator;
                res->array = هذا;
                res->next = 0;
                إرجاع res;
        }
    }

    إرجاع 0;
}

فراغ __vpiArrayBase::make_vals_كلمات()
{
    أكد(vals_كلمات == 0);
    vals_كلمات = جديد بنية __vpiArrayWord[get_size() + 1];

    // Make word[-1] point to the parent.
    vals_كلمات->parent = هذا;
    // Now point to word-0
    vals_كلمات += 1;

    بنية __vpiArrayWord*كلمات = vals_كلمات;
    لكل (unsigned idx = 0 ; idx < get_size() ; idx += 1) {
            كلمات[idx].word0 = كلمات;
    }
}

vpiHandle __vpiArrayIterator::vpi_index(صحيح)
{
      إذا (next >= array->get_size()) {
	    vpi_free_object(هذا);
	    إرجاع 0;
      }

      unsigned use_index = next;
      next += 1;

      إرجاع array->get_iter_index(هذا, use_index);
}

ساكن صحيح array_iterator_free_object(vpiHandle ref)
{
      بنية __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      delete obj;
      إرجاع 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(فراغ)
{ إرجاع &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(صحيح code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      إذا (code == vpiIndex) {
	    بنية __vpiArrayIndex*res;
	    res = جديد __vpiArrayIndex;
	    res->index = index;         // see the comment above
	    res->done = 0;
	    إرجاع res;
      }
      إرجاع 0;
}

vpiHandle __vpiArrayIndex::vpi_index(صحيح)
{
      إذا (done == 0) {
            done = 1;
            إرجاع index;
      }

      vpi_free_object(هذا);
      إرجاع 0;
}

ساكن صحيح array_index_free_object(vpiHandle ref)
{
      بنية __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      delete obj;
      إرجاع 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(فراغ)
{ إرجاع &array_index_free_object; }

حرف* __vpiArrayWord::as_word_t::vpi_get_str(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_word_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_parent = obj->get_parent();
      إرجاع my_parent->get_word_str(obj, code);
}

فراغ __vpiArrayWord::as_word_t::vpi_get_قيمة(p_vpi_قيمة vp)
{
      بنية __vpiArrayWord*obj = array_var_word_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_parent = obj->get_parent();
      إرجاع my_parent->get_word_قيمة(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_قيمة(p_vpi_قيمة vp, صحيح flags)
{
      بنية __vpiArrayWord*obj = array_var_word_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_قيمة(obj, vp, flags);
      إرجاع هذا;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_word_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_parent = obj->get_parent();

      بدل (code) {

	  حالة vpiIndex:
	    إرجاع &(obj->as_index);

	  حالة vpi_يسار_Range:
	    إرجاع my_parent->get_يسار_range();

	  حالة vpi_يمين_Range:
	    إرجاع my_parent->get_يمين_range();

	  حالة vpiParent:
	    إرجاع dynamic_cast<vpiHandle>(my_parent);

	  حالة vpiScope:
	    إرجاع my_parent->get_scope();

	  حالة vpiModule:
	    إرجاع vpip_module(my_parent->get_scope());
      }

      إرجاع 0;
}

فراغ __vpiArrayWord::as_index_t::vpi_get_قيمة(p_vpi_قيمة vp)
{
      بنية __vpiArrayWord*obj = array_var_index_from_handle(هذا);
      أكد(obj);
      unsigned index = obj->get_index();

      أكد(vp->format == vpiIntVal);
      vp->value.integer = index;
}

بنية __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      إذا (ref == 0)
	    إرجاع 0;
      ثابت __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      إذا (ptr == 0)
	    إرجاع 0;

      إرجاع (بنية __vpiArrayWord*)(ref);
}

بنية __vpiArrayWord* array_var_index_from_handle(vpiHandle ref)
{
      إذا (ref == 0)
	    إرجاع 0;
      ثابت __vpiArrayWord::as_index_t*ptr = dynamic_cast<__vpiArrayWord::as_index_t*> (ref);
      إذا (ptr == 0)
	    إرجاع 0;

      أكد(حجم(__vpiHandle) == حجم(__vpiArrayWord::as_index_t));
      أكد(حجم(__vpiHandle) == حجم(__vpiArrayWord::as_word_t));
      إرجاع (بنية __vpiArrayWord*)(ref-1);
}

