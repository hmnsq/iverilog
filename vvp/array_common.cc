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

#include "array_common.h"

vpiHandle __vpiArrayBase::vpi_array_base_iterate(ціле code)
{
    switch (code) {
        case vpiReg:
        case vpiMemoryWord: {
                struct __vpiArrayIterator*res;
                res = new __vpiArrayIterator;
                res->array = this;
                res->next = 0;
                повернути res;
        }
    }

    повернути 0;
}

void __vpiArrayBase::make_vals_words()
{
    assert(vals_words == 0);
    vals_words = new struct __vpiArrayWord[get_size() + 1];

    // Make word[-1] point to the parent.
    vals_words->parent = this;
    // Now point to word-0
    vals_words += 1;

    struct __vpiArrayWord*words = vals_words;
    для (unsigned idx = 0 ; idx < get_size() ; idx += 1) {
            words[idx].word0 = words;
    }
}

vpiHandle __vpiArrayIterator::vpi_index(ціле)
{
      якщо (next >= array->get_size()) {
	    vpi_free_object(this);
	    повернути 0;
      }

      unsigned use_index = next;
      next += 1;

      повернути array->get_iter_index(this, use_index);
}

static ціле array_iterator_free_object(vpiHandle ref)
{
      struct __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      delete obj;
      повернути 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(void)
{ повернути &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(ціле code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      якщо (code == vpiIndex) {
	    struct __vpiArrayIndex*res;
	    res = new __vpiArrayIndex;
	    res->index = index;         // see the comment above
	    res->done = 0;
	    повернути res;
      }
      повернути 0;
}

vpiHandle __vpiArrayIndex::vpi_index(ціле)
{
      якщо (done == 0) {
            done = 1;
            повернути index;
      }

      vpi_free_object(this);
      повернути 0;
}

static ціле array_index_free_object(vpiHandle ref)
{
      struct __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      delete obj;
      повернути 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(void)
{ повернути &array_index_free_object; }

char* __vpiArrayWord::as_word_t::vpi_get_str(ціле code)
{
      struct __vpiArrayWord*obj = array_var_word_from_handle(this);
      assert(obj);
      struct __vpiArrayBase*my_parent = obj->get_parent();
      повернути my_parent->get_word_str(obj, code);
}

void __vpiArrayWord::as_word_t::vpi_get_value(p_vpi_value vp)
{
      struct __vpiArrayWord*obj = array_var_word_from_handle(this);
      assert(obj);
      struct __vpiArrayBase*my_parent = obj->get_parent();
      повернути my_parent->get_word_value(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_value(p_vpi_value vp, ціле flags)
{
      struct __vpiArrayWord*obj = array_var_word_from_handle(this);
      assert(obj);
      struct __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_value(obj, vp, flags);
      повернути this;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(ціле code)
{
      struct __vpiArrayWord*obj = array_var_word_from_handle(this);
      assert(obj);
      struct __vpiArrayBase*my_parent = obj->get_parent();

      switch (code) {

	  case vpiIndex:
	    повернути &(obj->as_index);

	  case vpiLeftRange:
	    повернути my_parent->get_left_range();

	  case vpiRightRange:
	    повернути my_parent->get_right_range();

	  case vpiParent:
	    повернути dynamic_cast<vpiHandle>(my_parent);

	  case vpiScope:
	    повернути my_parent->get_scope();

	  case vpiModule:
	    повернути vpip_module(my_parent->get_scope());
      }

      повернути 0;
}

void __vpiArrayWord::as_index_t::vpi_get_value(p_vpi_value vp)
{
      struct __vpiArrayWord*obj = array_var_index_from_handle(this);
      assert(obj);
      unsigned index = obj->get_index();

      assert(vp->format == vpiIntVal);
      vp->value.integer = index;
}

struct __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      якщо (ref == 0)
	    повернути 0;
      const __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      якщо (ptr == 0)
	    повернути 0;

      повернути (struct __vpiArrayWord*)(ref);
}

struct __vpiArrayWord* array_var_index_from_handle(vpiHandle ref)
{
      якщо (ref == 0)
	    повернути 0;
      const __vpiArrayWord::as_index_t*ptr = dynamic_cast<__vpiArrayWord::as_index_t*> (ref);
      якщо (ptr == 0)
	    повернути 0;

      assert(sizeof(__vpiHandle) == sizeof(__vpiArrayWord::as_index_t));
      assert(sizeof(__vpiHandle) == sizeof(__vpiArrayWord::as_word_t));
      повернути (struct __vpiArrayWord*)(ref-1);
}

