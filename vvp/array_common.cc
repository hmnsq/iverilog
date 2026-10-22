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

#അന്തര്ഭാവ "array_common.h"

vpiHandle __vpiArrayBase::vpi_array_base_iterate(പൂർണ്ണസംഖ്യ code)
{
    തിരഞ്ഞെടുപ്പ് (code) {
        അവസ്ഥ vpiReg:
        അവസ്ഥ vpiMemoryWord: {
                ഘടന __vpiArrayIterator*res;
                res = പുതിയ __vpiArrayIterator;
                res->array = ഇത്;
                res->next = 0;
                തിരികെ res;
        }
    }

    തിരികെ 0;
}

ശൂന്യം __vpiArrayBase::make_vals_വാക്കുകൾ()
{
    പ്രതിജ്ഞാ(vals_വാക്കുകൾ == 0);
    vals_വാക്കുകൾ = പുതിയ ഘടന __vpiArrayWord[get_size() + 1];

    // Make word[-1] point to the parent.
    vals_വാക്കുകൾ->parent = ഇത്;
    // Now point to word-0
    vals_വാക്കുകൾ += 1;

    ഘടന __vpiArrayWord*വാക്കുകൾ = vals_വാക്കുകൾ;
    വേണ്ടി (unsigned idx = 0 ; idx < get_size() ; idx += 1) {
            വാക്കുകൾ[idx].word0 = വാക്കുകൾ;
    }
}

vpiHandle __vpiArrayIterator::vpi_index(പൂർണ്ണസംഖ്യ)
{
      എങ്കിൽ (next >= array->get_size()) {
	    vpi_free_object(ഇത്);
	    തിരികെ 0;
      }

      unsigned use_index = next;
      next += 1;

      തിരികെ array->get_iter_index(ഇത്, use_index);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ array_iterator_free_object(vpiHandle ref)
{
      ഘടന __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      delete obj;
      തിരികെ 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(ശൂന്യം)
{ തിരികെ &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(പൂർണ്ണസംഖ്യ code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      എങ്കിൽ (code == vpiIndex) {
	    ഘടന __vpiArrayIndex*res;
	    res = പുതിയ __vpiArrayIndex;
	    res->index = index;         // see the comment above
	    res->done = 0;
	    തിരികെ res;
      }
      തിരികെ 0;
}

vpiHandle __vpiArrayIndex::vpi_index(പൂർണ്ണസംഖ്യ)
{
      എങ്കിൽ (done == 0) {
            done = 1;
            തിരികെ index;
      }

      vpi_free_object(ഇത്);
      തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ array_index_free_object(vpiHandle ref)
{
      ഘടന __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      delete obj;
      തിരികെ 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(ശൂന്യം)
{ തിരികെ &array_index_free_object; }

അക്ഷരം* __vpiArrayWord::as_word_t::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      ഘടന __vpiArrayWord*obj = array_var_word_from_handle(ഇത്);
      പ്രതിജ്ഞാ(obj);
      ഘടന __vpiArrayBase*my_parent = obj->get_parent();
      തിരികെ my_parent->get_word_str(obj, code);
}

ശൂന്യം __vpiArrayWord::as_word_t::vpi_get_മൂല്യം(p_vpi_മൂല്യം vp)
{
      ഘടന __vpiArrayWord*obj = array_var_word_from_handle(ഇത്);
      പ്രതിജ്ഞാ(obj);
      ഘടന __vpiArrayBase*my_parent = obj->get_parent();
      തിരികെ my_parent->get_word_മൂല്യം(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_മൂല്യം(p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ flags)
{
      ഘടന __vpiArrayWord*obj = array_var_word_from_handle(ഇത്);
      പ്രതിജ്ഞാ(obj);
      ഘടന __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_മൂല്യം(obj, vp, flags);
      തിരികെ ഇത്;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(പൂർണ്ണസംഖ്യ code)
{
      ഘടന __vpiArrayWord*obj = array_var_word_from_handle(ഇത്);
      പ്രതിജ്ഞാ(obj);
      ഘടന __vpiArrayBase*my_parent = obj->get_parent();

      തിരഞ്ഞെടുപ്പ് (code) {

	  അവസ്ഥ vpiIndex:
	    തിരികെ &(obj->as_index);

	  അവസ്ഥ vpi_ഇടത്_Range:
	    തിരികെ my_parent->get_ഇടത്_range();

	  അവസ്ഥ vpi_വലത്_Range:
	    തിരികെ my_parent->get_വലത്_range();

	  അവസ്ഥ vpiParent:
	    തിരികെ dynamic_cast<vpiHandle>(my_parent);

	  അവസ്ഥ vpiScope:
	    തിരികെ my_parent->get_scope();

	  അവസ്ഥ vpiModule:
	    തിരികെ vpip_module(my_parent->get_scope());
      }

      തിരികെ 0;
}

ശൂന്യം __vpiArrayWord::as_index_t::vpi_get_മൂല്യം(p_vpi_മൂല്യം vp)
{
      ഘടന __vpiArrayWord*obj = array_var_index_from_handle(ഇത്);
      പ്രതിജ്ഞാ(obj);
      unsigned index = obj->get_index();

      പ്രതിജ്ഞാ(vp->format == vpiIntVal);
      vp->value.integer = index;
}

ഘടന __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      എങ്കിൽ (ref == 0)
	    തിരികെ 0;
      സ്ഥിരം __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      എങ്കിൽ (ptr == 0)
	    തിരികെ 0;

      തിരികെ (ഘടന __vpiArrayWord*)(ref);
}

ഘടന __vpiArrayWord* array_var_index_from_handle(vpiHandle ref)
{
      എങ്കിൽ (ref == 0)
	    തിരികെ 0;
      സ്ഥിരം __vpiArrayWord::as_index_t*ptr = dynamic_cast<__vpiArrayWord::as_index_t*> (ref);
      എങ്കിൽ (ptr == 0)
	    തിരികെ 0;

      പ്രതിജ്ഞാ(വലുപ്പം(__vpiHandle) == വലുപ്പം(__vpiArrayWord::as_index_t));
      പ്രതിജ്ഞാ(വലുപ്പം(__vpiHandle) == വലുപ്പം(__vpiArrayWord::as_word_t));
      തിരികെ (ഘടന __vpiArrayWord*)(ref-1);
}

