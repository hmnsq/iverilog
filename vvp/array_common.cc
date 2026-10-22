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

vpiHandle __vpiArrayBase::vpi_array_base_iterate(정수 code)
{
    switch (code) {
        case vpiReg:
        case vpiMemoryWord: {
                struct __vpiArrayIterator*res;
                res = new __vpiArrayIterator;
                res->array = this;
                res->next = 0;
                반환 res;
        }
    }

    반환 0;
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
    위해 (unsigned idx = 0 ; idx < get_size() ; idx += 1) {
            words[idx].word0 = words;
    }
}

vpiHandle __vpiArrayIterator::vpi_index(정수)
{
      만약 (next >= array->get_size()) {
	    vpi_free_object(this);
	    반환 0;
      }

      unsigned use_index = next;
      next += 1;

      반환 array->get_iter_index(this, use_index);
}

static 정수 array_iterator_free_object(vpiHandle ref)
{
      struct __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      delete obj;
      반환 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(void)
{ 반환 &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(정수 code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      만약 (code == vpiIndex) {
	    struct __vpiArrayIndex*res;
	    res = new __vpiArrayIndex;
	    res->index = index;         // see the comment above
	    res->done = 0;
	    반환 res;
      }
      반환 0;
}

vpiHandle __vpiArrayIndex::vpi_index(정수)
{
      만약 (done == 0) {
            done = 1;
            반환 index;
      }

      vpi_free_object(this);
      반환 0;
}

static 정수 array_index_free_object(vpiHandle ref)
{
      struct __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      delete obj;
      반환 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(void)
{ 반환 &array_index_free_object; }

char* __vpiArrayWord::as_word_t::vpi_get_str(정수 code)
{
      struct __vpiArrayWord*obj = array_var_word_from_handle(this);
      assert(obj);
      struct __vpiArrayBase*my_parent = obj->get_parent();
      반환 my_parent->get_word_str(obj, code);
}

void __vpiArrayWord::as_word_t::vpi_get_value(p_vpi_value vp)
{
      struct __vpiArrayWord*obj = array_var_word_from_handle(this);
      assert(obj);
      struct __vpiArrayBase*my_parent = obj->get_parent();
      반환 my_parent->get_word_value(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_value(p_vpi_value vp, 정수 flags)
{
      struct __vpiArrayWord*obj = array_var_word_from_handle(this);
      assert(obj);
      struct __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_value(obj, vp, flags);
      반환 this;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(정수 code)
{
      struct __vpiArrayWord*obj = array_var_word_from_handle(this);
      assert(obj);
      struct __vpiArrayBase*my_parent = obj->get_parent();

      switch (code) {

	  case vpiIndex:
	    반환 &(obj->as_index);

	  case vpiLeftRange:
	    반환 my_parent->get_left_range();

	  case vpiRightRange:
	    반환 my_parent->get_right_range();

	  case vpiParent:
	    반환 dynamic_cast<vpiHandle>(my_parent);

	  case vpiScope:
	    반환 my_parent->get_scope();

	  case vpiModule:
	    반환 vpip_module(my_parent->get_scope());
      }

      반환 0;
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
      만약 (ref == 0)
	    반환 0;
      const __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      만약 (ptr == 0)
	    반환 0;

      반환 (struct __vpiArrayWord*)(ref);
}

struct __vpiArrayWord* array_var_index_from_handle(vpiHandle ref)
{
      만약 (ref == 0)
	    반환 0;
      const __vpiArrayWord::as_index_t*ptr = dynamic_cast<__vpiArrayWord::as_index_t*> (ref);
      만약 (ptr == 0)
	    반환 0;

      assert(sizeof(__vpiHandle) == sizeof(__vpiArrayWord::as_index_t));
      assert(sizeof(__vpiHandle) == sizeof(__vpiArrayWord::as_word_t));
      반환 (struct __vpiArrayWord*)(ref-1);
}

