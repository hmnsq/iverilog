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

#অন্তর্ভাব "array_common.h"

vpiHandle __vpiArrayBase::vpi_array_base_iterate(পূর্ণসংখ্যা code)
{
    নির্বাচন (code) {
        ক্ষেত্র vpiReg:
        ক্ষেত্র vpiMemoryWord: {
                কাঠামো __vpiArrayIterator*res;
                res = নতুন __vpiArrayIterator;
                res->array = এই;
                res->পরবর্তী_2 = 0;
                ফেরত res;
        }
    }

    ফেরত 0;
}

শূন্য __vpiArrayBase::make_vals_শব্দগুলি()
{
    প্রতিজ্ঞা(vals_শব্দগুলি == 0);
    vals_শব্দগুলি = নতুন কাঠামো __vpiArrayWord[get_আকার() + 1];

    // Make word[-1] point to the parent.
    vals_শব্দগুলি->parent = এই;
    // Now point to word-0
    vals_শব্দগুলি += 1;

    কাঠামো __vpiArrayWord*শব্দগুলি = vals_শব্দগুলি;
    জন্য (unsigned idx = 0 ; idx < get_আকার() ; idx += 1) {
            শব্দগুলি[idx].word0 = শব্দগুলি;
    }
}

vpiHandle __vpiArrayIterator::vpi_সূচক(পূর্ণসংখ্যা)
{
      যদি (পরবর্তী_2 >= array->get_আকার()) {
	    vpi_free_object(এই);
	    ফেরত 0;
      }

      unsigned use_সূচক = পরবর্তী_2;
      পরবর্তী_2 += 1;

      ফেরত array->get_iter_সূচক(এই, use_সূচক);
}

স্থির পূর্ণসংখ্যা array_iterator_free_object(vpiHandle ref)
{
      কাঠামো __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      delete obj;
      ফেরত 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(শূন্য)
{ ফেরত &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(পূর্ণসংখ্যা code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      যদি (code == vpi_সূচক_2) {
	    কাঠামো __vpiArrayIndex*res;
	    res = নতুন __vpiArrayIndex;
	    res->সূচক = সূচক;         // see the comment above
	    res->done = 0;
	    ফেরত res;
      }
      ফেরত 0;
}

vpiHandle __vpiArrayIndex::vpi_সূচক(পূর্ণসংখ্যা)
{
      যদি (done == 0) {
            done = 1;
            ফেরত সূচক;
      }

      vpi_free_object(এই);
      ফেরত 0;
}

স্থির পূর্ণসংখ্যা array_সূচক_free_object(vpiHandle ref)
{
      কাঠামো __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      delete obj;
      ফেরত 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(শূন্য)
{ ফেরত &array_সূচক_free_object; }

অক্ষর* __vpiArrayWord::as_word_t::vpi_get_str(পূর্ণসংখ্যা code)
{
      কাঠামো __vpiArrayWord*obj = array_var_word_from_handle(এই);
      প্রতিজ্ঞা(obj);
      কাঠামো __vpiArrayBase*my_parent = obj->get_parent();
      ফেরত my_parent->get_word_str(obj, code);
}

শূন্য __vpiArrayWord::as_word_t::vpi_get_মান(p_vpi_মান vp)
{
      কাঠামো __vpiArrayWord*obj = array_var_word_from_handle(এই);
      প্রতিজ্ঞা(obj);
      কাঠামো __vpiArrayBase*my_parent = obj->get_parent();
      ফেরত my_parent->get_word_মান(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_মান(p_vpi_মান vp, পূর্ণসংখ্যা flags)
{
      কাঠামো __vpiArrayWord*obj = array_var_word_from_handle(এই);
      প্রতিজ্ঞা(obj);
      কাঠামো __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_মান(obj, vp, flags);
      ফেরত এই;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(পূর্ণসংখ্যা code)
{
      কাঠামো __vpiArrayWord*obj = array_var_word_from_handle(এই);
      প্রতিজ্ঞা(obj);
      কাঠামো __vpiArrayBase*my_parent = obj->get_parent();

      নির্বাচন (code) {

	  ক্ষেত্র vpi_সূচক_2:
	    ফেরত &(obj->as_সূচক);

	  ক্ষেত্র vpi_বাম_Range:
	    ফেরত my_parent->get_বাম_range();

	  ক্ষেত্র vpi_ডান_Range:
	    ফেরত my_parent->get_ডান_range();

	  ক্ষেত্র vpiParent:
	    ফেরত dynamic_cast<vpiHandle>(my_parent);

	  ক্ষেত্র vpiScope:
	    ফেরত my_parent->get_scope();

	  ক্ষেত্র vpiModule:
	    ফেরত vpip_module(my_parent->get_scope());
      }

      ফেরত 0;
}

শূন্য __vpiArrayWord::as_সূচক_t::vpi_get_মান(p_vpi_মান vp)
{
      কাঠামো __vpiArrayWord*obj = array_var_সূচক_from_handle(এই);
      প্রতিজ্ঞা(obj);
      unsigned সূচক = obj->get_সূচক();

      প্রতিজ্ঞা(vp->format == vpiIntVal);
      vp->value.integer = সূচক;
}

কাঠামো __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      যদি (ref == 0)
	    ফেরত 0;
      ধ্রুবক __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      যদি (ptr == 0)
	    ফেরত 0;

      ফেরত (কাঠামো __vpiArrayWord*)(ref);
}

কাঠামো __vpiArrayWord* array_var_সূচক_from_handle(vpiHandle ref)
{
      যদি (ref == 0)
	    ফেরত 0;
      ধ্রুবক __vpiArrayWord::as_সূচক_t*ptr = dynamic_cast<__vpiArrayWord::as_সূচক_t*> (ref);
      যদি (ptr == 0)
	    ফেরত 0;

      প্রতিজ্ঞা(আকার(__vpiHandle) == আকার(__vpiArrayWord::as_সূচক_t));
      প্রতিজ্ঞা(আকার(__vpiHandle) == আকার(__vpiArrayWord::as_word_t));
      ফেরত (কাঠামো __vpiArrayWord*)(ref-1);
}

