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

#ଅନ୍ତର୍ଭାଵ "array_common.h"

vpiHandle __vpiArrayBase::vpi_array_base_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
    ବାଛ (code) {
        ସ୍ଥିତି vpiReg:
        ସ୍ଥିତି vpiMemoryWord: {
                ସଂରଚନା __vpiArrayIterator*res;
                res = ନୂତନ __vpiArrayIterator;
                res->array = ଏହି;
                res->next = 0;
                ଫେରନ୍ତୁ res;
        }
    }

    ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ __vpiArrayBase::make_vals_ଶବ୍ଦଗୁଡ଼ିକ()
{
    ପ୍ରତିଜ୍ଞା(vals_ଶବ୍ଦଗୁଡ଼ିକ == 0);
    vals_ଶବ୍ଦଗୁଡ଼ିକ = ନୂତନ ସଂରଚନା __vpiArrayWord[get_size() + 1];

    // Make word[-1] point to the parent.
    vals_ଶବ୍ଦଗୁଡ଼ିକ->parent = ଏହି;
    // Now point to word-0
    vals_ଶବ୍ଦଗୁଡ଼ିକ += 1;

    ସଂରଚନା __vpiArrayWord*ଶବ୍ଦଗୁଡ଼ିକ = vals_ଶବ୍ଦଗୁଡ଼ିକ;
    ପାଇଁ (unsigned idx = 0 ; idx < get_size() ; idx += 1) {
            ଶବ୍ଦଗୁଡ଼ିକ[idx].word0 = ଶବ୍ଦଗୁଡ଼ିକ;
    }
}

vpiHandle __vpiArrayIterator::vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      ଯଦି (next >= array->get_size()) {
	    vpi_free_object(ଏହି);
	    ଫେରନ୍ତୁ 0;
      }

      unsigned use_index = next;
      next += 1;

      ଫେରନ୍ତୁ array->get_iter_index(ଏହି, use_index);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା array_iterator_free_object(vpiHandle ref)
{
      ସଂରଚନା __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      delete obj;
      ଫେରନ୍ତୁ 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(ଶୂନ୍ୟ)
{ ଫେରନ୍ତୁ &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      ଯଦି (code == vpiIndex) {
	    ସଂରଚନା __vpiArrayIndex*res;
	    res = ନୂତନ __vpiArrayIndex;
	    res->index = index;         // see the comment above
	    res->done = 0;
	    ଫେରନ୍ତୁ res;
      }
      ଫେରନ୍ତୁ 0;
}

vpiHandle __vpiArrayIndex::vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      ଯଦି (done == 0) {
            done = 1;
            ଫେରନ୍ତୁ index;
      }

      vpi_free_object(ଏହି);
      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା array_index_free_object(vpiHandle ref)
{
      ସଂରଚନା __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      delete obj;
      ଫେରନ୍ତୁ 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(ଶୂନ୍ୟ)
{ ଫେରନ୍ତୁ &array_index_free_object; }

ଅକ୍ଷର* __vpiArrayWord::as_word_t::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ସଂରଚନା __vpiArrayWord*obj = array_var_word_from_handle(ଏହି);
      ପ୍ରତିଜ୍ଞା(obj);
      ସଂରଚନା __vpiArrayBase*my_parent = obj->get_parent();
      ଫେରନ୍ତୁ my_parent->get_word_str(obj, code);
}

ଶୂନ୍ୟ __vpiArrayWord::as_word_t::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp)
{
      ସଂରଚନା __vpiArrayWord*obj = array_var_word_from_handle(ଏହି);
      ପ୍ରତିଜ୍ଞା(obj);
      ସଂରଚନା __vpiArrayBase*my_parent = obj->get_parent();
      ଫେରନ୍ତୁ my_parent->get_word_ମୂଲ୍ୟ(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
      ସଂରଚନା __vpiArrayWord*obj = array_var_word_from_handle(ଏହି);
      ପ୍ରତିଜ୍ଞା(obj);
      ସଂରଚନା __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_ମୂଲ୍ୟ(obj, vp, flags);
      ଫେରନ୍ତୁ ଏହି;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ସଂରଚନା __vpiArrayWord*obj = array_var_word_from_handle(ଏହି);
      ପ୍ରତିଜ୍ଞା(obj);
      ସଂରଚନା __vpiArrayBase*my_parent = obj->get_parent();

      ବାଛ (code) {

	  ସ୍ଥିତି vpiIndex:
	    ଫେରନ୍ତୁ &(obj->as_index);

	  ସ୍ଥିତି vpi_ବାମ_Range:
	    ଫେରନ୍ତୁ my_parent->get_ବାମ_range();

	  ସ୍ଥିତି vpi_ଡାହାଣ_Range:
	    ଫେରନ୍ତୁ my_parent->get_ଡାହାଣ_range();

	  ସ୍ଥିତି vpiParent:
	    ଫେରନ୍ତୁ dynamic_cast<vpiHandle>(my_parent);

	  ସ୍ଥିତି vpiScope:
	    ଫେରନ୍ତୁ my_parent->get_scope();

	  ସ୍ଥିତି vpiModule:
	    ଫେରନ୍ତୁ vpip_module(my_parent->get_scope());
      }

      ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ __vpiArrayWord::as_index_t::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp)
{
      ସଂରଚନା __vpiArrayWord*obj = array_var_index_from_handle(ଏହି);
      ପ୍ରତିଜ୍ଞା(obj);
      unsigned index = obj->get_index();

      ପ୍ରତିଜ୍ଞା(vp->format == vpiIntVal);
      vp->value.integer = index;
}

ସଂରଚନା __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      ଯଦି (ref == 0)
	    ଫେରନ୍ତୁ 0;
      ଅଚଳ __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      ଯଦି (ptr == 0)
	    ଫେରନ୍ତୁ 0;

      ଫେରନ୍ତୁ (ସଂରଚନା __vpiArrayWord*)(ref);
}

ସଂରଚନା __vpiArrayWord* array_var_index_from_handle(vpiHandle ref)
{
      ଯଦି (ref == 0)
	    ଫେରନ୍ତୁ 0;
      ଅଚଳ __vpiArrayWord::as_index_t*ptr = dynamic_cast<__vpiArrayWord::as_index_t*> (ref);
      ଯଦି (ptr == 0)
	    ଫେରନ୍ତୁ 0;

      ପ୍ରତିଜ୍ଞା(ଆକାର(__vpiHandle) == ଆକାର(__vpiArrayWord::as_index_t));
      ପ୍ରତିଜ୍ଞା(ଆକାର(__vpiHandle) == ଆକାର(__vpiArrayWord::as_word_t));
      ଫେରନ୍ତୁ (ସଂରଚନା __vpiArrayWord*)(ref-1);
}

