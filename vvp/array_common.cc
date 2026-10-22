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

#अन्तर्भाव "array_common.h"

vpiHandle __vpiArrayBase::vpi_array_base_iterate(पूर्णाङ्क code)
{
    विकल्प (code) {
        पक्ष vpiReg:
        पक्ष vpiMemoryWord: {
                संरचना __vpiArrayIterator*res;
                res = नूतन __vpiArrayIterator;
                res->array = एतत्;
                res->next = 0;
                पटिनिवत्त res;
        }
    }

    पटिनिवत्त 0;
}

शून्य __vpiArrayBase::make_vals_शब्दाः()
{
    प्रतिज्ञा(vals_शब्दाः == 0);
    vals_शब्दाः = नूतन संरचना __vpiArrayWord[get_size() + 1];

    // Make word[-1] point to the parent.
    vals_शब्दाः->parent = एतत्;
    // Now point to word-0
    vals_शब्दाः += 1;

    संरचना __vpiArrayWord*शब्दाः = vals_शब्दाः;
    अत्थाय (unsigned idx = 0 ; idx < get_size() ; idx += 1) {
            शब्दाः[idx].word0 = शब्दाः;
    }
}

vpiHandle __vpiArrayIterator::vpi_index(पूर्णाङ्क)
{
      सचे (next >= array->get_size()) {
	    vpi_free_object(एतत्);
	    पटिनिवत्त 0;
      }

      unsigned use_index = next;
      next += 1;

      पटिनिवत्त array->get_iter_index(एतत्, use_index);
}

स्थिर पूर्णाङ्क array_iterator_free_object(vpiHandle ref)
{
      संरचना __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      delete obj;
      पटिनिवत्त 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(शून्य)
{ पटिनिवत्त &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(पूर्णाङ्क code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      सचे (code == vpiIndex) {
	    संरचना __vpiArrayIndex*res;
	    res = नूतन __vpiArrayIndex;
	    res->index = index;         // see the comment above
	    res->done = 0;
	    पटिनिवत्त res;
      }
      पटिनिवत्त 0;
}

vpiHandle __vpiArrayIndex::vpi_index(पूर्णाङ्क)
{
      सचे (done == 0) {
            done = 1;
            पटिनिवत्त index;
      }

      vpi_free_object(एतत्);
      पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क array_index_free_object(vpiHandle ref)
{
      संरचना __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      delete obj;
      पटिनिवत्त 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(शून्य)
{ पटिनिवत्त &array_index_free_object; }

वर्ण* __vpiArrayWord::as_word_t::vpi_get_str(पूर्णाङ्क code)
{
      संरचना __vpiArrayWord*obj = array_var_word_from_handle(एतत्);
      प्रतिज्ञा(obj);
      संरचना __vpiArrayBase*my_parent = obj->get_parent();
      पटिनिवत्त my_parent->get_word_str(obj, code);
}

शून्य __vpiArrayWord::as_word_t::vpi_get_मानम्(p_vpi_मानम् vp)
{
      संरचना __vpiArrayWord*obj = array_var_word_from_handle(एतत्);
      प्रतिज्ञा(obj);
      संरचना __vpiArrayBase*my_parent = obj->get_parent();
      पटिनिवत्त my_parent->get_word_मानम्(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_मानम्(p_vpi_मानम् vp, पूर्णाङ्क flags)
{
      संरचना __vpiArrayWord*obj = array_var_word_from_handle(एतत्);
      प्रतिज्ञा(obj);
      संरचना __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_मानम्(obj, vp, flags);
      पटिनिवत्त एतत्;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(पूर्णाङ्क code)
{
      संरचना __vpiArrayWord*obj = array_var_word_from_handle(एतत्);
      प्रतिज्ञा(obj);
      संरचना __vpiArrayBase*my_parent = obj->get_parent();

      विकल्प (code) {

	  पक्ष vpiIndex:
	    पटिनिवत्त &(obj->as_index);

	  पक्ष vpi_वामः_Range:
	    पटिनिवत्त my_parent->get_वामः_range();

	  पक्ष vpi_दक्षिणः_Range:
	    पटिनिवत्त my_parent->get_दक्षिणः_range();

	  पक्ष vpiParent:
	    पटिनिवत्त dynamic_cast<vpiHandle>(my_parent);

	  पक्ष vpiScope:
	    पटिनिवत्त my_parent->get_scope();

	  पक्ष vpiModule:
	    पटिनिवत्त vpip_module(my_parent->get_scope());
      }

      पटिनिवत्त 0;
}

शून्य __vpiArrayWord::as_index_t::vpi_get_मानम्(p_vpi_मानम् vp)
{
      संरचना __vpiArrayWord*obj = array_var_index_from_handle(एतत्);
      प्रतिज्ञा(obj);
      unsigned index = obj->get_index();

      प्रतिज्ञा(vp->format == vpiIntVal);
      vp->value.integer = index;
}

संरचना __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      सचे (ref == 0)
	    पटिनिवत्त 0;
      ध्रुव __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      सचे (ptr == 0)
	    पटिनिवत्त 0;

      पटिनिवत्त (संरचना __vpiArrayWord*)(ref);
}

संरचना __vpiArrayWord* array_var_index_from_handle(vpiHandle ref)
{
      सचे (ref == 0)
	    पटिनिवत्त 0;
      ध्रुव __vpiArrayWord::as_index_t*ptr = dynamic_cast<__vpiArrayWord::as_index_t*> (ref);
      सचे (ptr == 0)
	    पटिनिवत्त 0;

      प्रतिज्ञा(परिमाण(__vpiHandle) == परिमाण(__vpiArrayWord::as_index_t));
      प्रतिज्ञा(परिमाण(__vpiHandle) == परिमाण(__vpiArrayWord::as_word_t));
      पटिनिवत्त (संरचना __vpiArrayWord*)(ref-1);
}

