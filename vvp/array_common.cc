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

#समावेश "array_common.h"

vpiHandle __vpiArrayBase::vpi_array_base_iterate(पूर्णांक code)
{
    चयन (code) {
        हाल vpiReg:
        हाल vpiMemoryWord: {
                काष्ठा __vpiArrayIterator*res;
                res = नया __vpiArrayIterator;
                res->array = यह;
                res->अग्र_2 = 0;
                वापस res;
        }
    }

    वापस 0;
}

व्योम __vpiArrayBase::make_vals_शब्दगण()
{
    निश्चित(vals_शब्दगण == 0);
    vals_शब्दगण = नया काष्ठा __vpiArrayWord[get_आकार() + 1];

    // Make word[-1] point to the parent.
    vals_शब्दगण->parent = यह;
    // Now point to word-0
    vals_शब्दगण += 1;

    काष्ठा __vpiArrayWord*शब्दगण = vals_शब्दगण;
    क्रम (अचिन्हित idx = 0 ; idx < get_आकार() ; idx += 1) {
            शब्दगण[idx].word0 = शब्दगण;
    }
}

vpiHandle __vpiArrayIterator::vpi_सूचकांक(पूर्णांक)
{
      अगर (अग्र_2 >= array->get_आकार()) {
	    vpi_free_object(यह);
	    वापस 0;
      }

      अचिन्हित use_सूचकांक = अग्र_2;
      अग्र_2 += 1;

      वापस array->get_iter_सूचकांक(यह, use_सूचकांक);
}

अटल पूर्णांक array_iterator_free_object(vpiHandle ref)
{
      काष्ठा __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      मिटाओ obj;
      वापस 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(व्योम)
{ वापस &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(पूर्णांक code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      अगर (code == vpi_सूचकांक_2) {
	    काष्ठा __vpiArrayIndex*res;
	    res = नया __vpiArrayIndex;
	    res->सूचकांक = सूचकांक;         // see the comment above
	    res->done = 0;
	    वापस res;
      }
      वापस 0;
}

vpiHandle __vpiArrayIndex::vpi_सूचकांक(पूर्णांक)
{
      अगर (done == 0) {
            done = 1;
            वापस सूचकांक;
      }

      vpi_free_object(यह);
      वापस 0;
}

अटल पूर्णांक array_सूचकांक_free_object(vpiHandle ref)
{
      काष्ठा __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      मिटाओ obj;
      वापस 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(व्योम)
{ वापस &array_सूचकांक_free_object; }

अक्षर* __vpiArrayWord::as_word_t::vpi_get_str(पूर्णांक code)
{
      काष्ठा __vpiArrayWord*obj = array_var_word_from_handle(यह);
      निश्चित(obj);
      काष्ठा __vpiArrayBase*my_parent = obj->get_parent();
      वापस my_parent->get_word_str(obj, code);
}

व्योम __vpiArrayWord::as_word_t::vpi_get_मूल्य(p_vpi_मूल्य vp)
{
      काष्ठा __vpiArrayWord*obj = array_var_word_from_handle(यह);
      निश्चित(obj);
      काष्ठा __vpiArrayBase*my_parent = obj->get_parent();
      वापस my_parent->get_word_मूल्य(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक flags)
{
      काष्ठा __vpiArrayWord*obj = array_var_word_from_handle(यह);
      निश्चित(obj);
      काष्ठा __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_मूल्य(obj, vp, flags);
      वापस यह;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(पूर्णांक code)
{
      काष्ठा __vpiArrayWord*obj = array_var_word_from_handle(यह);
      निश्चित(obj);
      काष्ठा __vpiArrayBase*my_parent = obj->get_parent();

      चयन (code) {

	  हाल vpi_सूचकांक_2:
	    वापस &(obj->as_सूचकांक);

	  हाल vpi_बायाँ_Range:
	    वापस my_parent->get_बायाँ_range();

	  हाल vpi_दायाँ_Range:
	    वापस my_parent->get_दायाँ_range();

	  हाल vpiParent:
	    वापस dynamic_cast<vpiHandle>(my_parent);

	  हाल vpiScope:
	    वापस my_parent->get_scope();

	  हाल vpiModule:
	    वापस vpip_module(my_parent->get_scope());
      }

      वापस 0;
}

व्योम __vpiArrayWord::as_सूचकांक_t::vpi_get_मूल्य(p_vpi_मूल्य vp)
{
      काष्ठा __vpiArrayWord*obj = array_var_सूचकांक_from_handle(यह);
      निश्चित(obj);
      अचिन्हित सूचकांक = obj->get_सूचकांक();

      निश्चित(vp->format == vpiIntVal);
      vp->value.integer = सूचकांक;
}

काष्ठा __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      अगर (ref == 0)
	    वापस 0;
      स्थिर __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      अगर (ptr == 0)
	    वापस 0;

      वापस (काष्ठा __vpiArrayWord*)(ref);
}

काष्ठा __vpiArrayWord* array_var_सूचकांक_from_handle(vpiHandle ref)
{
      अगर (ref == 0)
	    वापस 0;
      स्थिर __vpiArrayWord::as_सूचकांक_t*ptr = dynamic_cast<__vpiArrayWord::as_सूचकांक_t*> (ref);
      अगर (ptr == 0)
	    वापस 0;

      निश्चित(माप(__vpiHandle) == माप(__vpiArrayWord::as_सूचकांक_t));
      निश्चित(माप(__vpiHandle) == माप(__vpiArrayWord::as_word_t));
      वापस (काष्ठा __vpiArrayWord*)(ref-1);
}

