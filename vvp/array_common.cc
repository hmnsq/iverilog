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

#અન્તર્ભાવ "array_common.h"

vpiHandle __vpiArrayBase::vpi_array_base_iterate(પૂર્ણાંક code)
{
    પસંદગી (code) {
        સ્થિતિ vpiReg:
        સ્થિતિ vpiMemoryWord: {
                સંરચના __vpiArrayIterator*res;
                res = નવું __vpiArrayIterator;
                res->array = આ;
                res->next = 0;
                પાછા res;
        }
    }

    પાછા 0;
}

શૂન્ય __vpiArrayBase::make_vals_શબ્દો()
{
    પ્રતિજ્ઞા(vals_શબ્દો == 0);
    vals_શબ્દો = નવું સંરચના __vpiArrayWord[get_size() + 1];

    // Make word[-1] point to the parent.
    vals_શબ્દો->parent = આ;
    // Now point to word-0
    vals_શબ્દો += 1;

    સંરચના __vpiArrayWord*શબ્દો = vals_શબ્દો;
    માટે (unsigned idx = 0 ; idx < get_size() ; idx += 1) {
            શબ્દો[idx].word0 = શબ્દો;
    }
}

vpiHandle __vpiArrayIterator::vpi_index(પૂર્ણાંક)
{
      જો (next >= array->get_size()) {
	    vpi_free_object(આ);
	    પાછા 0;
      }

      unsigned use_index = next;
      next += 1;

      પાછા array->get_iter_index(આ, use_index);
}

સ્થિર પૂર્ણાંક array_iterator_free_object(vpiHandle ref)
{
      સંરચના __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      delete obj;
      પાછા 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(શૂન્ય)
{ પાછા &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(પૂર્ણાંક code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      જો (code == vpiIndex) {
	    સંરચના __vpiArrayIndex*res;
	    res = નવું __vpiArrayIndex;
	    res->index = index;         // see the comment above
	    res->done = 0;
	    પાછા res;
      }
      પાછા 0;
}

vpiHandle __vpiArrayIndex::vpi_index(પૂર્ણાંક)
{
      જો (done == 0) {
            done = 1;
            પાછા index;
      }

      vpi_free_object(આ);
      પાછા 0;
}

સ્થિર પૂર્ણાંક array_index_free_object(vpiHandle ref)
{
      સંરચના __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      delete obj;
      પાછા 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(શૂન્ય)
{ પાછા &array_index_free_object; }

અક્ષર* __vpiArrayWord::as_word_t::vpi_get_str(પૂર્ણાંક code)
{
      સંરચના __vpiArrayWord*obj = array_var_word_from_handle(આ);
      પ્રતિજ્ઞા(obj);
      સંરચના __vpiArrayBase*my_parent = obj->get_parent();
      પાછા my_parent->get_word_str(obj, code);
}

શૂન્ય __vpiArrayWord::as_word_t::vpi_get_કિંમત(p_vpi_કિંમત vp)
{
      સંરચના __vpiArrayWord*obj = array_var_word_from_handle(આ);
      પ્રતિજ્ઞા(obj);
      સંરચના __vpiArrayBase*my_parent = obj->get_parent();
      પાછા my_parent->get_word_કિંમત(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_કિંમત(p_vpi_કિંમત vp, પૂર્ણાંક flags)
{
      સંરચના __vpiArrayWord*obj = array_var_word_from_handle(આ);
      પ્રતિજ્ઞા(obj);
      સંરચના __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_કિંમત(obj, vp, flags);
      પાછા આ;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(પૂર્ણાંક code)
{
      સંરચના __vpiArrayWord*obj = array_var_word_from_handle(આ);
      પ્રતિજ્ઞા(obj);
      સંરચના __vpiArrayBase*my_parent = obj->get_parent();

      પસંદગી (code) {

	  સ્થિતિ vpiIndex:
	    પાછા &(obj->as_index);

	  સ્થિતિ vpi_ડાબું_Range:
	    પાછા my_parent->get_ડાબું_range();

	  સ્થિતિ vpi_જમણું_Range:
	    પાછા my_parent->get_જમણું_range();

	  સ્થિતિ vpiParent:
	    પાછા dynamic_cast<vpiHandle>(my_parent);

	  સ્થિતિ vpiScope:
	    પાછા my_parent->get_scope();

	  સ્થિતિ vpiModule:
	    પાછા vpip_module(my_parent->get_scope());
      }

      પાછા 0;
}

શૂન્ય __vpiArrayWord::as_index_t::vpi_get_કિંમત(p_vpi_કિંમત vp)
{
      સંરચના __vpiArrayWord*obj = array_var_index_from_handle(આ);
      પ્રતિજ્ઞા(obj);
      unsigned index = obj->get_index();

      પ્રતિજ્ઞા(vp->format == vpiIntVal);
      vp->value.integer = index;
}

સંરચના __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      જો (ref == 0)
	    પાછા 0;
      અચલ __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      જો (ptr == 0)
	    પાછા 0;

      પાછા (સંરચના __vpiArrayWord*)(ref);
}

સંરચના __vpiArrayWord* array_var_index_from_handle(vpiHandle ref)
{
      જો (ref == 0)
	    પાછા 0;
      અચલ __vpiArrayWord::as_index_t*ptr = dynamic_cast<__vpiArrayWord::as_index_t*> (ref);
      જો (ptr == 0)
	    પાછા 0;

      પ્રતિજ્ઞા(કદ(__vpiHandle) == કદ(__vpiArrayWord::as_index_t));
      પ્રતિજ્ઞા(કદ(__vpiHandle) == કદ(__vpiArrayWord::as_word_t));
      પાછા (સંરચના __vpiArrayWord*)(ref-1);
}

