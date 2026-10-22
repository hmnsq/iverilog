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

#ಅನ್ತರ್ಭಾವ "array_common.h"

vpiHandle __vpiArrayBase::vpi_array_base_iterate(ಪೂರ್ಣಾಙ್ಕ code)
{
    ವಿಕಲ್ಪ (code) {
        ಪಕ್ಷ vpiReg:
        ಪಕ್ಷ vpiMemoryWord: {
                ಸಂರಚನಾ __vpiArrayIterator*res;
                res = ನೂತನ __vpiArrayIterator;
                res->array = ಏತತ್;
                res->next = 0;
                ಪ್ರತ್ಯಾಗಮ res;
        }
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ __vpiArrayBase::make_vals_ಪದಗಳು()
{
    ಪ್ರತಿಜ್ಞಾ(vals_ಪದಗಳು == 0);
    vals_ಪದಗಳು = ನೂತನ ಸಂರಚನಾ __vpiArrayWord[get_size() + 1];

    // Make word[-1] point to the parent.
    vals_ಪದಗಳು->parent = ಏತತ್;
    // Now point to word-0
    vals_ಪದಗಳು += 1;

    ಸಂರಚನಾ __vpiArrayWord*ಪದಗಳು = vals_ಪದಗಳು;
    ಪ್ರತಿ (unsigned idx = 0 ; idx < get_size() ; idx += 1) {
            ಪದಗಳು[idx].word0 = ಪದಗಳು;
    }
}

vpiHandle __vpiArrayIterator::vpi_index(ಪೂರ್ಣಾಙ್ಕ)
{
      ಯದಿ (next >= array->get_size()) {
	    vpi_free_object(ಏತತ್);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }

      unsigned use_index = next;
      next += 1;

      ಪ್ರತ್ಯಾಗಮ array->get_iter_index(ಏತತ್, use_index);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ array_iterator_free_object(vpiHandle ref)
{
      ಸಂರಚನಾ __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      delete obj;
      ಪ್ರತ್ಯಾಗಮ 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(ಶೂನ್ಯ)
{ ಪ್ರತ್ಯಾಗಮ &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      ಯದಿ (code == vpiIndex) {
	    ಸಂರಚನಾ __vpiArrayIndex*res;
	    res = ನೂತನ __vpiArrayIndex;
	    res->index = index;         // see the comment above
	    res->done = 0;
	    ಪ್ರತ್ಯಾಗಮ res;
      }
      ಪ್ರತ್ಯಾಗಮ 0;
}

vpiHandle __vpiArrayIndex::vpi_index(ಪೂರ್ಣಾಙ್ಕ)
{
      ಯದಿ (done == 0) {
            done = 1;
            ಪ್ರತ್ಯಾಗಮ index;
      }

      vpi_free_object(ಏತತ್);
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ array_index_free_object(vpiHandle ref)
{
      ಸಂರಚನಾ __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      delete obj;
      ಪ್ರತ್ಯಾಗಮ 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(ಶೂನ್ಯ)
{ ಪ್ರತ್ಯಾಗಮ &array_index_free_object; }

ವರ್ಣ* __vpiArrayWord::as_word_t::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ಸಂರಚನಾ __vpiArrayWord*obj = array_var_word_from_handle(ಏತತ್);
      ಪ್ರತಿಜ್ಞಾ(obj);
      ಸಂರಚನಾ __vpiArrayBase*my_parent = obj->get_parent();
      ಪ್ರತ್ಯಾಗಮ my_parent->get_word_str(obj, code);
}

ಶೂನ್ಯ __vpiArrayWord::as_word_t::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      ಸಂರಚನಾ __vpiArrayWord*obj = array_var_word_from_handle(ಏತತ್);
      ಪ್ರತಿಜ್ಞಾ(obj);
      ಸಂರಚನಾ __vpiArrayBase*my_parent = obj->get_parent();
      ಪ್ರತ್ಯಾಗಮ my_parent->get_word_ಮೌಲ್ಯ(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ flags)
{
      ಸಂರಚನಾ __vpiArrayWord*obj = array_var_word_from_handle(ಏತತ್);
      ಪ್ರತಿಜ್ಞಾ(obj);
      ಸಂರಚನಾ __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_ಮೌಲ್ಯ(obj, vp, flags);
      ಪ್ರತ್ಯಾಗಮ ಏತತ್;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{
      ಸಂರಚನಾ __vpiArrayWord*obj = array_var_word_from_handle(ಏತತ್);
      ಪ್ರತಿಜ್ಞಾ(obj);
      ಸಂರಚನಾ __vpiArrayBase*my_parent = obj->get_parent();

      ವಿಕಲ್ಪ (code) {

	  ಪಕ್ಷ vpiIndex:
	    ಪ್ರತ್ಯಾಗಮ &(obj->as_index);

	  ಪಕ್ಷ vpi_ಎಡ_Range:
	    ಪ್ರತ್ಯಾಗಮ my_parent->get_ಎಡ_range();

	  ಪಕ್ಷ vpi_ಬಲ_Range:
	    ಪ್ರತ್ಯಾಗಮ my_parent->get_ಬಲ_range();

	  ಪಕ್ಷ vpiParent:
	    ಪ್ರತ್ಯಾಗಮ dynamic_cast<vpiHandle>(my_parent);

	  ಪಕ್ಷ vpiScope:
	    ಪ್ರತ್ಯಾಗಮ my_parent->get_scope();

	  ಪಕ್ಷ vpiModule:
	    ಪ್ರತ್ಯಾಗಮ vpip_module(my_parent->get_scope());
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ __vpiArrayWord::as_index_t::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      ಸಂರಚನಾ __vpiArrayWord*obj = array_var_index_from_handle(ಏತತ್);
      ಪ್ರತಿಜ್ಞಾ(obj);
      unsigned index = obj->get_index();

      ಪ್ರತಿಜ್ಞಾ(vp->format == vpiIntVal);
      vp->value.integer = index;
}

ಸಂರಚನಾ __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      ಯದಿ (ref == 0)
	    ಪ್ರತ್ಯಾಗಮ 0;
      ಧ್ರುವ __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      ಯದಿ (ptr == 0)
	    ಪ್ರತ್ಯಾಗಮ 0;

      ಪ್ರತ್ಯಾಗಮ (ಸಂರಚನಾ __vpiArrayWord*)(ref);
}

ಸಂರಚನಾ __vpiArrayWord* array_var_index_from_handle(vpiHandle ref)
{
      ಯದಿ (ref == 0)
	    ಪ್ರತ್ಯಾಗಮ 0;
      ಧ್ರುವ __vpiArrayWord::as_index_t*ptr = dynamic_cast<__vpiArrayWord::as_index_t*> (ref);
      ಯದಿ (ptr == 0)
	    ಪ್ರತ್ಯಾಗಮ 0;

      ಪ್ರತಿಜ್ಞಾ(ಪರಿಮಾಣ(__vpiHandle) == ಪರಿಮಾಣ(__vpiArrayWord::as_index_t));
      ಪ್ರತಿಜ್ಞಾ(ಪರಿಮಾಣ(__vpiHandle) == ಪರಿಮಾಣ(__vpiArrayWord::as_word_t));
      ಪ್ರತ್ಯಾಗಮ (ಸಂರಚನಾ __vpiArrayWord*)(ref-1);
}

