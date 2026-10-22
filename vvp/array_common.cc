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

#অন্তর্ভাৱ "array_common.h"

vpiHandle __vpiArrayBase::vpi_array_base_iterate(পূর্ণাঙ্ক code)
{
    ৱিকল্প (code) {
        পক্ষ vpiReg:
        পক্ষ vpiMemoryWord: {
                সংরচনা __vpiArrayIterator*res;
                res = নূতন __vpiArrayIterator;
                res->array = এতত্;
                res->next = 0;
                ঘূৰাই res;
        }
    }

    ঘূৰাই 0;
}

শূন্য __vpiArrayBase::make_vals_শব্দগুলি()
{
    প্রতিজ্ঞা(vals_শব্দগুলি == 0);
    vals_শব্দগুলি = নূতন সংরচনা __vpiArrayWord[get_size() + 1];

    // Make word[-1] point to the parent.
    vals_শব্দগুলি->parent = এতত্;
    // Now point to word-0
    vals_শব্দগুলি += 1;

    সংরচনা __vpiArrayWord*শব্দগুলি = vals_শব্দগুলি;
    বাবে (unsigned idx = 0 ; idx < get_size() ; idx += 1) {
            শব্দগুলি[idx].word0 = শব্দগুলি;
    }
}

vpiHandle __vpiArrayIterator::vpi_index(পূর্ণাঙ্ক)
{
      যদি (next >= array->get_size()) {
	    vpi_free_object(এতত্);
	    ঘূৰাই 0;
      }

      unsigned use_index = next;
      next += 1;

      ঘূৰাই array->get_iter_index(এতত্, use_index);
}

স্থির পূর্ণাঙ্ক array_iterator_free_object(vpiHandle ref)
{
      সংরচনা __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      delete obj;
      ঘূৰাই 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(শূন্য)
{ ঘূৰাই &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(পূর্ণাঙ্ক code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      যদি (code == vpiIndex) {
	    সংরচনা __vpiArrayIndex*res;
	    res = নূতন __vpiArrayIndex;
	    res->index = index;         // see the comment above
	    res->done = 0;
	    ঘূৰাই res;
      }
      ঘূৰাই 0;
}

vpiHandle __vpiArrayIndex::vpi_index(পূর্ণাঙ্ক)
{
      যদি (done == 0) {
            done = 1;
            ঘূৰাই index;
      }

      vpi_free_object(এতত্);
      ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক array_index_free_object(vpiHandle ref)
{
      সংরচনা __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      delete obj;
      ঘূৰাই 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(শূন্য)
{ ঘূৰাই &array_index_free_object; }

ৱর্ণ* __vpiArrayWord::as_word_t::vpi_get_str(পূর্ণাঙ্ক code)
{
      সংরচনা __vpiArrayWord*obj = array_var_word_from_handle(এতত্);
      প্রতিজ্ঞা(obj);
      সংরচনা __vpiArrayBase*my_parent = obj->get_parent();
      ঘূৰাই my_parent->get_word_str(obj, code);
}

শূন্য __vpiArrayWord::as_word_t::vpi_get_মান(p_vpi_মান vp)
{
      সংরচনা __vpiArrayWord*obj = array_var_word_from_handle(এতত্);
      প্রতিজ্ঞা(obj);
      সংরচনা __vpiArrayBase*my_parent = obj->get_parent();
      ঘূৰাই my_parent->get_word_মান(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_মান(p_vpi_মান vp, পূর্ণাঙ্ক flags)
{
      সংরচনা __vpiArrayWord*obj = array_var_word_from_handle(এতত্);
      প্রতিজ্ঞা(obj);
      সংরচনা __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_মান(obj, vp, flags);
      ঘূৰাই এতত্;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(পূর্ণাঙ্ক code)
{
      সংরচনা __vpiArrayWord*obj = array_var_word_from_handle(এতত্);
      প্রতিজ্ঞা(obj);
      সংরচনা __vpiArrayBase*my_parent = obj->get_parent();

      ৱিকল্প (code) {

	  পক্ষ vpiIndex:
	    ঘূৰাই &(obj->as_index);

	  পক্ষ vpi_বাম_Range:
	    ঘূৰাই my_parent->get_বাম_range();

	  পক্ষ vpi_ডান_Range:
	    ঘূৰাই my_parent->get_ডান_range();

	  পক্ষ vpiParent:
	    ঘূৰাই dynamic_cast<vpiHandle>(my_parent);

	  পক্ষ vpiScope:
	    ঘূৰাই my_parent->get_scope();

	  পক্ষ vpiModule:
	    ঘূৰাই vpip_module(my_parent->get_scope());
      }

      ঘূৰাই 0;
}

শূন্য __vpiArrayWord::as_index_t::vpi_get_মান(p_vpi_মান vp)
{
      সংরচনা __vpiArrayWord*obj = array_var_index_from_handle(এতত্);
      প্রতিজ্ঞা(obj);
      unsigned index = obj->get_index();

      প্রতিজ্ঞা(vp->format == vpiIntVal);
      vp->value.integer = index;
}

সংরচনা __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      যদি (ref == 0)
	    ঘূৰাই 0;
      ধ্রুৱ __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      যদি (ptr == 0)
	    ঘূৰাই 0;

      ঘূৰাই (সংরচনা __vpiArrayWord*)(ref);
}

সংরচনা __vpiArrayWord* array_var_index_from_handle(vpiHandle ref)
{
      যদি (ref == 0)
	    ঘূৰাই 0;
      ধ্রুৱ __vpiArrayWord::as_index_t*ptr = dynamic_cast<__vpiArrayWord::as_index_t*> (ref);
      যদি (ptr == 0)
	    ঘূৰাই 0;

      প্রতিজ্ঞা(পরিমাণ(__vpiHandle) == পরিমাণ(__vpiArrayWord::as_index_t));
      প্রতিজ্ঞা(পরিমাণ(__vpiHandle) == পরিমাণ(__vpiArrayWord::as_word_t));
      ঘূৰাই (সংরচনা __vpiArrayWord*)(ref-1);
}

