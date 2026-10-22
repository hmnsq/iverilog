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

#அந்தர்பாவ "array_common.h"

vpiHandle __vpiArrayBase::vpi_array_base_iterate(முழு code)
{
    தேர்வு (code) {
        நிலை vpiReg:
        நிலை vpiMemoryWord: {
                கட்டமைப்பு __vpiArrayIterator*res;
                res = புதிய __vpiArrayIterator;
                res->array = இது;
                res->next = 0;
                திரும்பு res;
        }
    }

    திரும்பு 0;
}

வெறுமை __vpiArrayBase::make_vals_சொற்கள்()
{
    ப்ரதிஜ்ஞா(vals_சொற்கள் == 0);
    vals_சொற்கள் = புதிய கட்டமைப்பு __vpiArrayWord[get_size() + 1];

    // Make word[-1] point to the parent.
    vals_சொற்கள்->parent = இது;
    // Now point to word-0
    vals_சொற்கள் += 1;

    கட்டமைப்பு __vpiArrayWord*சொற்கள் = vals_சொற்கள்;
    க்கு (unsigned idx = 0 ; idx < get_size() ; idx += 1) {
            சொற்கள்[idx].word0 = சொற்கள்;
    }
}

vpiHandle __vpiArrayIterator::vpi_index(முழு)
{
      எனில் (next >= array->get_size()) {
	    vpi_free_object(இது);
	    திரும்பு 0;
      }

      unsigned use_index = next;
      next += 1;

      திரும்பு array->get_iter_index(இது, use_index);
}

நிலையான முழு array_iterator_free_object(vpiHandle ref)
{
      கட்டமைப்பு __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      delete obj;
      திரும்பு 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(வெறுமை)
{ திரும்பு &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(முழு code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      எனில் (code == vpiIndex) {
	    கட்டமைப்பு __vpiArrayIndex*res;
	    res = புதிய __vpiArrayIndex;
	    res->index = index;         // see the comment above
	    res->done = 0;
	    திரும்பு res;
      }
      திரும்பு 0;
}

vpiHandle __vpiArrayIndex::vpi_index(முழு)
{
      எனில் (done == 0) {
            done = 1;
            திரும்பு index;
      }

      vpi_free_object(இது);
      திரும்பு 0;
}

நிலையான முழு array_index_free_object(vpiHandle ref)
{
      கட்டமைப்பு __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      delete obj;
      திரும்பு 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(வெறுமை)
{ திரும்பு &array_index_free_object; }

எழுத்து* __vpiArrayWord::as_word_t::vpi_get_str(முழு code)
{
      கட்டமைப்பு __vpiArrayWord*obj = array_var_word_from_handle(இது);
      ப்ரதிஜ்ஞா(obj);
      கட்டமைப்பு __vpiArrayBase*my_parent = obj->get_parent();
      திரும்பு my_parent->get_word_str(obj, code);
}

வெறுமை __vpiArrayWord::as_word_t::vpi_get_மதிப்பு(p_vpi_மதிப்பு vp)
{
      கட்டமைப்பு __vpiArrayWord*obj = array_var_word_from_handle(இது);
      ப்ரதிஜ்ஞா(obj);
      கட்டமைப்பு __vpiArrayBase*my_parent = obj->get_parent();
      திரும்பு my_parent->get_word_மதிப்பு(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_மதிப்பு(p_vpi_மதிப்பு vp, முழு flags)
{
      கட்டமைப்பு __vpiArrayWord*obj = array_var_word_from_handle(இது);
      ப்ரதிஜ்ஞா(obj);
      கட்டமைப்பு __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_மதிப்பு(obj, vp, flags);
      திரும்பு இது;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(முழு code)
{
      கட்டமைப்பு __vpiArrayWord*obj = array_var_word_from_handle(இது);
      ப்ரதிஜ்ஞா(obj);
      கட்டமைப்பு __vpiArrayBase*my_parent = obj->get_parent();

      தேர்வு (code) {

	  நிலை vpiIndex:
	    திரும்பு &(obj->as_index);

	  நிலை vpi_இடது_Range:
	    திரும்பு my_parent->get_இடது_range();

	  நிலை vpi_வலது_Range:
	    திரும்பு my_parent->get_வலது_range();

	  நிலை vpiParent:
	    திரும்பு dynamic_cast<vpiHandle>(my_parent);

	  நிலை vpiScope:
	    திரும்பு my_parent->get_scope();

	  நிலை vpiModule:
	    திரும்பு vpip_module(my_parent->get_scope());
      }

      திரும்பு 0;
}

வெறுமை __vpiArrayWord::as_index_t::vpi_get_மதிப்பு(p_vpi_மதிப்பு vp)
{
      கட்டமைப்பு __vpiArrayWord*obj = array_var_index_from_handle(இது);
      ப்ரதிஜ்ஞா(obj);
      unsigned index = obj->get_index();

      ப்ரதிஜ்ஞா(vp->format == vpiIntVal);
      vp->value.integer = index;
}

கட்டமைப்பு __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      எனில் (ref == 0)
	    திரும்பு 0;
      மாறிலி __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      எனில் (ptr == 0)
	    திரும்பு 0;

      திரும்பு (கட்டமைப்பு __vpiArrayWord*)(ref);
}

கட்டமைப்பு __vpiArrayWord* array_var_index_from_handle(vpiHandle ref)
{
      எனில் (ref == 0)
	    திரும்பு 0;
      மாறிலி __vpiArrayWord::as_index_t*ptr = dynamic_cast<__vpiArrayWord::as_index_t*> (ref);
      எனில் (ptr == 0)
	    திரும்பு 0;

      ப்ரதிஜ்ஞா(அளவு(__vpiHandle) == அளவு(__vpiArrayWord::as_index_t));
      ப்ரதிஜ்ஞா(அளவு(__vpiHandle) == அளவு(__vpiArrayWord::as_word_t));
      திரும்பு (கட்டமைப்பு __vpiArrayWord*)(ref-1);
}

