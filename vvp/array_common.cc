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

vpiHandle __vpiArrayBase::vpi_array_base_iterate(ཧྲིལ code)
{
    switch (code) {
        pak_sa vpiReg:
        pak_sa vpiMemoryWord: {
                sa_nracanaa __vpiArrayIterator*res;
                res = new __vpiArrayIterator;
                res->array = this;
                res->next = 0;
                ཕྱིར res;
        }
    }

    ཕྱིར 0;
}

void __vpiArrayBase::make_vals_words()
{
    assert(vals_words == 0);
    vals_words = new sa_nracanaa __vpiArrayWord[get_size() + 1];

    // Make word[-1] point to the parent.
    vals_words->parent = this;
    // Now point to word-0
    vals_words += 1;

    sa_nracanaa __vpiArrayWord*words = vals_words;
    ཆེད་དུ (unsigned idx = 0 ; idx < get_size() ; idx += 1) {
            words[idx].word0 = words;
    }
}

vpiHandle __vpiArrayIterator::vpi_index(ཧྲིལ)
{
      གལ་ཏེ (next >= array->get_size()) {
	    vpi_free_object(this);
	    ཕྱིར 0;
      }

      unsigned use_index = next;
      next += 1;

      ཕྱིར array->get_iter_index(this, use_index);
}

static ཧྲིལ array_iterator_free_object(vpiHandle ref)
{
      sa_nracanaa __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      delete obj;
      ཕྱིར 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(void)
{ ཕྱིར &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(ཧྲིལ code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      གལ་ཏེ (code == vpiIndex) {
	    sa_nracanaa __vpiArrayIndex*res;
	    res = new __vpiArrayIndex;
	    res->index = index;         // see the comment above
	    res->done = 0;
	    ཕྱིར res;
      }
      ཕྱིར 0;
}

vpiHandle __vpiArrayIndex::vpi_index(ཧྲིལ)
{
      གལ་ཏེ (done == 0) {
            done = 1;
            ཕྱིར index;
      }

      vpi_free_object(this);
      ཕྱིར 0;
}

static ཧྲིལ array_index_free_object(vpiHandle ref)
{
      sa_nracanaa __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      delete obj;
      ཕྱིར 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(void)
{ ཕྱིར &array_index_free_object; }

var_na* __vpiArrayWord::as_word_t::vpi_get_str(ཧྲིལ code)
{
      sa_nracanaa __vpiArrayWord*obj = array_var_word_from_handle(this);
      assert(obj);
      sa_nracanaa __vpiArrayBase*my_parent = obj->get_parent();
      ཕྱིར my_parent->get_word_str(obj, code);
}

void __vpiArrayWord::as_word_t::vpi_get_value(p_vpi_value vp)
{
      sa_nracanaa __vpiArrayWord*obj = array_var_word_from_handle(this);
      assert(obj);
      sa_nracanaa __vpiArrayBase*my_parent = obj->get_parent();
      ཕྱིར my_parent->get_word_value(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_value(p_vpi_value vp, ཧྲིལ flags)
{
      sa_nracanaa __vpiArrayWord*obj = array_var_word_from_handle(this);
      assert(obj);
      sa_nracanaa __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_value(obj, vp, flags);
      ཕྱིར this;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(ཧྲིལ code)
{
      sa_nracanaa __vpiArrayWord*obj = array_var_word_from_handle(this);
      assert(obj);
      sa_nracanaa __vpiArrayBase*my_parent = obj->get_parent();

      switch (code) {

	  pak_sa vpiIndex:
	    ཕྱིར &(obj->as_index);

	  pak_sa vpiLeftRange:
	    ཕྱིར my_parent->get_left_range();

	  pak_sa vpiRightRange:
	    ཕྱིར my_parent->get_right_range();

	  pak_sa vpiParent:
	    ཕྱིར dynamic_cast<vpiHandle>(my_parent);

	  pak_sa vpiScope:
	    ཕྱིར my_parent->get_scope();

	  pak_sa vpiModule:
	    ཕྱིར vpip_module(my_parent->get_scope());
      }

      ཕྱིར 0;
}

void __vpiArrayWord::as_index_t::vpi_get_value(p_vpi_value vp)
{
      sa_nracanaa __vpiArrayWord*obj = array_var_index_from_handle(this);
      assert(obj);
      unsigned index = obj->get_index();

      assert(vp->format == vpiIntVal);
      vp->value.integer = index;
}

sa_nracanaa __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      གལ་ཏེ (ref == 0)
	    ཕྱིར 0;
      const __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      གལ་ཏེ (ptr == 0)
	    ཕྱིར 0;

      ཕྱིར (sa_nracanaa __vpiArrayWord*)(ref);
}

sa_nracanaa __vpiArrayWord* array_var_index_from_handle(vpiHandle ref)
{
      གལ་ཏེ (ref == 0)
	    ཕྱིར 0;
      const __vpiArrayWord::as_index_t*ptr = dynamic_cast<__vpiArrayWord::as_index_t*> (ref);
      གལ་ཏེ (ptr == 0)
	    ཕྱིར 0;

      assert(parimaa_na(__vpiHandle) == parimaa_na(__vpiArrayWord::as_index_t));
      assert(parimaa_na(__vpiHandle) == parimaa_na(__vpiArrayWord::as_word_t));
      ཕྱིར (sa_nracanaa __vpiArrayWord*)(ref-1);
}

