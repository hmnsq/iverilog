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

vpiHandle __vpiArrayBase::vpi_array_base_iterate(पूर्णांक code)
{
    निवड (code) {
        स्थिती vpiReg:
        स्थिती vpiMemoryWord: {
                संरचना __vpiArrayIterator*res;
                res = नवीन __vpiArrayIterator;
                res->array = हे;
                res->next = 0;
                परत res;
        }
    }

    परत 0;
}

शून्य __vpiArrayBase::make_vals_शब्द()
{
    प्रतिज्ञा(vals_शब्द == 0);
    vals_शब्द = नवीन संरचना __vpiArrayWord[get_size() + 1];

    // Make word[-1] point to the parent.
    vals_शब्द->parent = हे;
    // Now point to word-0
    vals_शब्द += 1;

    संरचना __vpiArrayWord*शब्द = vals_शब्द;
    साठी (अचिन्हित idx = 0 ; idx < get_size() ; idx += 1) {
            शब्द[idx].word0 = शब्द;
    }
}

vpiHandle __vpiArrayIterator::vpi_index(पूर्णांक)
{
      जर (next >= array->get_size()) {
	    vpi_free_object(हे);
	    परत 0;
      }

      अचिन्हित use_index = next;
      next += 1;

      परत array->get_iter_index(हे, use_index);
}

स्थिर पूर्णांक array_iterator_free_object(vpiHandle ref)
{
      संरचना __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      delete obj;
      परत 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(शून्य)
{ परत &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(पूर्णांक code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      जर (code == vpiIndex) {
	    संरचना __vpiArrayIndex*res;
	    res = नवीन __vpiArrayIndex;
	    res->index = index;         // see the comment above
	    res->done = 0;
	    परत res;
      }
      परत 0;
}

vpiHandle __vpiArrayIndex::vpi_index(पूर्णांक)
{
      जर (done == 0) {
            done = 1;
            परत index;
      }

      vpi_free_object(हे);
      परत 0;
}

स्थिर पूर्णांक array_index_free_object(vpiHandle ref)
{
      संरचना __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      delete obj;
      परत 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(शून्य)
{ परत &array_index_free_object; }

अक्षर* __vpiArrayWord::as_word_t::vpi_get_str(पूर्णांक code)
{
      संरचना __vpiArrayWord*obj = array_var_word_from_handle(हे);
      प्रतिज्ञा(obj);
      संरचना __vpiArrayBase*my_parent = obj->get_parent();
      परत my_parent->get_word_str(obj, code);
}

शून्य __vpiArrayWord::as_word_t::vpi_get_मूल्य(p_vpi_मूल्य vp)
{
      संरचना __vpiArrayWord*obj = array_var_word_from_handle(हे);
      प्रतिज्ञा(obj);
      संरचना __vpiArrayBase*my_parent = obj->get_parent();
      परत my_parent->get_word_मूल्य(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक flags)
{
      संरचना __vpiArrayWord*obj = array_var_word_from_handle(हे);
      प्रतिज्ञा(obj);
      संरचना __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_मूल्य(obj, vp, flags);
      परत हे;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(पूर्णांक code)
{
      संरचना __vpiArrayWord*obj = array_var_word_from_handle(हे);
      प्रतिज्ञा(obj);
      संरचना __vpiArrayBase*my_parent = obj->get_parent();

      निवड (code) {

	  स्थिती vpiIndex:
	    परत &(obj->as_index);

	  स्थिती vpi_डावा_Range:
	    परत my_parent->get_डावा_range();

	  स्थिती vpi_उजवा_Range:
	    परत my_parent->get_उजवा_range();

	  स्थिती vpiParent:
	    परत dynamic_cast<vpiHandle>(my_parent);

	  स्थिती vpiScope:
	    परत my_parent->get_scope();

	  स्थिती vpiModule:
	    परत vpip_module(my_parent->get_scope());
      }

      परत 0;
}

शून्य __vpiArrayWord::as_index_t::vpi_get_मूल्य(p_vpi_मूल्य vp)
{
      संरचना __vpiArrayWord*obj = array_var_index_from_handle(हे);
      प्रतिज्ञा(obj);
      अचिन्हित index = obj->get_index();

      प्रतिज्ञा(vp->format == vpiIntVal);
      vp->value.integer = index;
}

संरचना __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      जर (ref == 0)
	    परत 0;
      स्थिरांक __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      जर (ptr == 0)
	    परत 0;

      परत (संरचना __vpiArrayWord*)(ref);
}

संरचना __vpiArrayWord* array_var_index_from_handle(vpiHandle ref)
{
      जर (ref == 0)
	    परत 0;
      स्थिरांक __vpiArrayWord::as_index_t*ptr = dynamic_cast<__vpiArrayWord::as_index_t*> (ref);
      जर (ptr == 0)
	    परत 0;

      प्रतिज्ञा(आकार(__vpiHandle) == आकार(__vpiArrayWord::as_index_t));
      प्रतिज्ञा(आकार(__vpiHandle) == आकार(__vpiArrayWord::as_word_t));
      परत (संरचना __vpiArrayWord*)(ref-1);
}

