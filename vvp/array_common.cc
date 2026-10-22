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

vpiHandle __vpiArrayBase::vpi_array_base_iterate(صحیح code)
{
    انتخاب (code) {
        حالت vpiReg:
        حالت vpiMemoryWord: {
                ساختار __vpiArrayIterator*res;
                res = جدید __vpiArrayIterator;
                res->array = این;
                res->next = 0;
                بازگشت res;
        }
    }

    بازگشت 0;
}

تهی __vpiArrayBase::make_vals_واژگان()
{
    أكد(vals_واژگان == 0);
    vals_واژگان = جدید ساختار __vpiArrayWord[get_size() + 1];

    // Make word[-1] point to the parent.
    vals_واژگان->parent = این;
    // Now point to word-0
    vals_واژگان += 1;

    ساختار __vpiArrayWord*واژگان = vals_واژگان;
    برای (unsigned idx = 0 ; idx < get_size() ; idx += 1) {
            واژگان[idx].word0 = واژگان;
    }
}

vpiHandle __vpiArrayIterator::vpi_index(صحیح)
{
      اگر (next >= array->get_size()) {
	    vpi_free_object(این);
	    بازگشت 0;
      }

      unsigned use_index = next;
      next += 1;

      بازگشت array->get_iter_index(این, use_index);
}

ایستا صحیح array_iterator_free_object(vpiHandle ref)
{
      ساختار __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      delete obj;
      بازگشت 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(تهی)
{ بازگشت &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(صحیح code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      اگر (code == vpiIndex) {
	    ساختار __vpiArrayIndex*res;
	    res = جدید __vpiArrayIndex;
	    res->index = index;         // see the comment above
	    res->done = 0;
	    بازگشت res;
      }
      بازگشت 0;
}

vpiHandle __vpiArrayIndex::vpi_index(صحیح)
{
      اگر (done == 0) {
            done = 1;
            بازگشت index;
      }

      vpi_free_object(این);
      بازگشت 0;
}

ایستا صحیح array_index_free_object(vpiHandle ref)
{
      ساختار __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      delete obj;
      بازگشت 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(تهی)
{ بازگشت &array_index_free_object; }

نویسه* __vpiArrayWord::as_word_t::vpi_get_str(صحیح code)
{
      ساختار __vpiArrayWord*obj = array_var_word_from_handle(این);
      أكد(obj);
      ساختار __vpiArrayBase*my_parent = obj->get_parent();
      بازگشت my_parent->get_word_str(obj, code);
}

تهی __vpiArrayWord::as_word_t::vpi_get_مقدار(p_vpi_مقدار vp)
{
      ساختار __vpiArrayWord*obj = array_var_word_from_handle(این);
      أكد(obj);
      ساختار __vpiArrayBase*my_parent = obj->get_parent();
      بازگشت my_parent->get_word_مقدار(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_مقدار(p_vpi_مقدار vp, صحیح flags)
{
      ساختار __vpiArrayWord*obj = array_var_word_from_handle(این);
      أكد(obj);
      ساختار __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_مقدار(obj, vp, flags);
      بازگشت این;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(صحیح code)
{
      ساختار __vpiArrayWord*obj = array_var_word_from_handle(این);
      أكد(obj);
      ساختار __vpiArrayBase*my_parent = obj->get_parent();

      انتخاب (code) {

	  حالت vpiIndex:
	    بازگشت &(obj->as_index);

	  حالت vpi_چپ_Range:
	    بازگشت my_parent->get_چپ_range();

	  حالت vpi_راست_Range:
	    بازگشت my_parent->get_راست_range();

	  حالت vpiParent:
	    بازگشت dynamic_cast<vpiHandle>(my_parent);

	  حالت vpiScope:
	    بازگشت my_parent->get_scope();

	  حالت vpiModule:
	    بازگشت vpip_module(my_parent->get_scope());
      }

      بازگشت 0;
}

تهی __vpiArrayWord::as_index_t::vpi_get_مقدار(p_vpi_مقدار vp)
{
      ساختار __vpiArrayWord*obj = array_var_index_from_handle(این);
      أكد(obj);
      unsigned index = obj->get_index();

      أكد(vp->format == vpiIntVal);
      vp->value.integer = index;
}

ساختار __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      اگر (ref == 0)
	    بازگشت 0;
      ثابت __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      اگر (ptr == 0)
	    بازگشت 0;

      بازگشت (ساختار __vpiArrayWord*)(ref);
}

ساختار __vpiArrayWord* array_var_index_from_handle(vpiHandle ref)
{
      اگر (ref == 0)
	    بازگشت 0;
      ثابت __vpiArrayWord::as_index_t*ptr = dynamic_cast<__vpiArrayWord::as_index_t*> (ref);
      اگر (ptr == 0)
	    بازگشت 0;

      أكد(اندازه(__vpiHandle) == اندازه(__vpiArrayWord::as_index_t));
      أكد(اندازه(__vpiHandle) == اندازه(__vpiArrayWord::as_word_t));
      بازگشت (ساختار __vpiArrayWord*)(ref-1);
}

