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
                أرجع res;
        }
    }

    أرجع 0;
}

فراغ __vpiArrayBase::make_vals_words()
{
    أكد(vals_words == 0);
    vals_words = جديد بنية __vpiArrayWord[get_size() + 1];

    // Make word[-1] point to the parent.
    vals_words->parent = هذا;
    // Now point to word-0
    vals_words += 1;

    بنية __vpiArrayWord*words = vals_words;
    لكل (unsigned idx = 0 ; idx < get_size() ; idx += 1) {
            words[idx].word0 = words;
    }
}

vpiHandle __vpiArrayIterator::vpi_index(صحيح)
{
      𐤀𐤌 (next >= array->get_size()) {
	    vpi_free_object(هذا);
	    أرجع 0;
      }

      unsigned use_index = next;
      next += 1;

      أرجع array->get_iter_index(هذا, use_index);
}

ساكن صحيح array_iterator_free_object(vpiHandle ref)
{
      بنية __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      delete obj;
      أرجع 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(فراغ)
{ أرجع &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(صحيح code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      𐤀𐤌 (code == vpiIndex) {
	    بنية __vpiArrayIndex*res;
	    res = جديد __vpiArrayIndex;
	    res->index = index;         // see the comment above
	    res->done = 0;
	    أرجع res;
      }
      أرجع 0;
}

vpiHandle __vpiArrayIndex::vpi_index(صحيح)
{
      𐤀𐤌 (done == 0) {
            done = 1;
            أرجع index;
      }

      vpi_free_object(هذا);
      أرجع 0;
}

ساكن صحيح array_index_free_object(vpiHandle ref)
{
      بنية __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      delete obj;
      أرجع 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(فراغ)
{ أرجع &array_index_free_object; }

حرف* __vpiArrayWord::as_word_t::vpi_get_str(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_word_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_parent = obj->get_parent();
      أرجع my_parent->get_word_str(obj, code);
}

فراغ __vpiArrayWord::as_word_t::vpi_get_value(p_vpi_value vp)
{
      بنية __vpiArrayWord*obj = array_var_word_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_parent = obj->get_parent();
      أرجع my_parent->get_word_value(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_value(p_vpi_value vp, صحيح flags)
{
      بنية __vpiArrayWord*obj = array_var_word_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_value(obj, vp, flags);
      أرجع هذا;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_word_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_parent = obj->get_parent();

      بدل (code) {

	  حالة vpiIndex:
	    أرجع &(obj->as_index);

	  حالة vpiLeftRange:
	    أرجع my_parent->get_left_range();

	  حالة vpiRightRange:
	    أرجع my_parent->get_right_range();

	  حالة vpiParent:
	    أرجع dynamic_cast<vpiHandle>(my_parent);

	  حالة vpiScope:
	    أرجع my_parent->get_scope();

	  حالة vpiModule:
	    أرجع vpip_module(my_parent->get_scope());
      }

      أرجع 0;
}

فراغ __vpiArrayWord::as_index_t::vpi_get_value(p_vpi_value vp)
{
      بنية __vpiArrayWord*obj = array_var_index_from_handle(هذا);
      أكد(obj);
      unsigned index = obj->get_index();

      أكد(vp->format == vpiIntVal);
      vp->value.integer = index;
}

بنية __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      𐤀𐤌 (ref == 0)
	    أرجع 0;
      ثابت __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      𐤀𐤌 (ptr == 0)
	    أرجع 0;

      أرجع (بنية __vpiArrayWord*)(ref);
}

بنية __vpiArrayWord* array_var_index_from_handle(vpiHandle ref)
{
      𐤀𐤌 (ref == 0)
	    أرجع 0;
      ثابت __vpiArrayWord::as_index_t*ptr = dynamic_cast<__vpiArrayWord::as_index_t*> (ref);
      𐤀𐤌 (ptr == 0)
	    أرجع 0;

      أكد(حجم(__vpiHandle) == حجم(__vpiArrayWord::as_index_t));
      أكد(حجم(__vpiHandle) == حجم(__vpiArrayWord::as_word_t));
      أرجع (بنية __vpiArrayWord*)(ref-1);
}

