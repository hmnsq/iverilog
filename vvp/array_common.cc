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

vpiHandle __vpiArrayBase::vpi_array_आधारः_iterate(पूर्णाङ्क code)
{
    विकल्प (code) {
        पक्ष vpiReg:
        पक्ष vpi_स्मृतिः_Word: {
                संरचना __vpiArrayIterator*res;
                res = नूतन __vpiArrayIterator;
                res->array = एतत्;
                res->अग्र_2 = 0;
                प्रत्यागम res;
        }
    }

    प्रत्यागम 0;
}

शून्य __vpiArrayBase::make_vals_शब्दाः()
{
    प्रतिज्ञा(vals_शब्दाः == 0);
    vals_शब्दाः = नूतन संरचना __vpiArrayWord[get_आकार() + 1];

    // Make word[-1] point to the parent.
    vals_शब्दाः->parent = एतत्;
    // Now point to word-0
    vals_शब्दाः += 1;

    संरचना __vpiArrayWord*शब्दाः = vals_शब्दाः;
    प्रति (अचिह्नित idx = 0 ; idx < get_आकार() ; idx += 1) {
            शब्दाः[idx].word0 = शब्दाः;
    }
}

vpiHandle __vpiArrayIterator::vpi_सूचकांक(पूर्णाङ्क)
{
      यदि (अग्र_2 >= array->get_आकार()) {
	    vpi_free_object(एतत्);
	    प्रत्यागम 0;
      }

      अचिह्नित use_सूचकांक = अग्र_2;
      अग्र_2 += 1;

      प्रत्यागम array->get_iter_सूचकांक(एतत्, use_सूचकांक);
}

स्थिर पूर्णाङ्क array_iterator_free_object(vpiHandle ref)
{
      संरचना __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      विलोपय obj;
      प्रत्यागम 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(शून्य)
{ प्रत्यागम &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(पूर्णाङ्क code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      यदि (code == vpi_सूचकांक_2) {
	    संरचना __vpiArrayIndex*res;
	    res = नूतन __vpiArrayIndex;
	    res->सूचकांक = सूचकांक;         // see the comment above
	    res->समाप्तम् = 0;
	    प्रत्यागम res;
      }
      प्रत्यागम 0;
}

vpiHandle __vpiArrayIndex::vpi_सूचकांक(पूर्णाङ्क)
{
      यदि (समाप्तम् == 0) {
            समाप्तम् = 1;
            प्रत्यागम सूचकांक;
      }

      vpi_free_object(एतत्);
      प्रत्यागम 0;
}

स्थिर पूर्णाङ्क array_सूचकांक_free_object(vpiHandle ref)
{
      संरचना __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      विलोपय obj;
      प्रत्यागम 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(शून्य)
{ प्रत्यागम &array_सूचकांक_free_object; }

वर्ण* __vpiArrayWord::as_word_t::vpi_get_str(पूर्णाङ्क code)
{
      संरचना __vpiArrayWord*obj = array_var_word_from_handle(एतत्);
      प्रतिज्ञा(obj);
      संरचना __vpiArrayBase*my_parent = obj->get_parent();
      प्रत्यागम my_parent->get_word_str(obj, code);
}

शून्य __vpiArrayWord::as_word_t::vpi_get_मानम्(p_vpi_मानम् vp)
{
      संरचना __vpiArrayWord*obj = array_var_word_from_handle(एतत्);
      प्रतिज्ञा(obj);
      संरचना __vpiArrayBase*my_parent = obj->get_parent();
      प्रत्यागम my_parent->get_word_मानम्(obj, vp);
}

vpiHandle __vpiArrayWord::as_word_t::vpi_put_मानम्(p_vpi_मानम् vp, पूर्णाङ्क flags)
{
      संरचना __vpiArrayWord*obj = array_var_word_from_handle(एतत्);
      प्रतिज्ञा(obj);
      संरचना __vpiArrayBase*my_parent = obj->get_parent();
      my_parent->put_word_मानम्(obj, vp, flags);
      प्रत्यागम एतत्;
}

vpiHandle __vpiArrayWord::as_word_t::vpi_handle(पूर्णाङ्क code)
{
      संरचना __vpiArrayWord*obj = array_var_word_from_handle(एतत्);
      प्रतिज्ञा(obj);
      संरचना __vpiArrayBase*my_parent = obj->get_parent();

      विकल्प (code) {

	  पक्ष vpi_सूचकांक_2:
	    प्रत्यागम &(obj->as_सूचकांक);

	  पक्ष vpi_वामः_Range:
	    प्रत्यागम my_parent->get_वामः_range();

	  पक्ष vpi_दक्षिणः_Range:
	    प्रत्यागम my_parent->get_दक्षिणः_range();

	  पक्ष vpiParent:
	    प्रत्यागम dynamic_cast<vpiHandle>(my_parent);

	  पक्ष vpiScope:
	    प्रत्यागम my_parent->get_scope();

	  पक्ष vpiModule:
	    प्रत्यागम vpip_module(my_parent->get_scope());
      }

      प्रत्यागम 0;
}

शून्य __vpiArrayWord::as_सूचकांक_t::vpi_get_मानम्(p_vpi_मानम् vp)
{
      संरचना __vpiArrayWord*obj = array_var_सूचकांक_from_handle(एतत्);
      प्रतिज्ञा(obj);
      अचिह्नित सूचकांक = obj->get_सूचकांक();

      प्रतिज्ञा(vp->format == vpiIntVal);
      vp->value.integer = सूचकांक;
}

संरचना __vpiArrayWord*array_var_word_from_handle(vpiHandle ref)
{
      यदि (ref == 0)
	    प्रत्यागम 0;
      ध्रुव __vpiArrayWord::as_word_t*ptr = dynamic_cast<__vpiArrayWord::as_word_t*> (ref);
      यदि (ptr == 0)
	    प्रत्यागम 0;

      प्रत्यागम (संरचना __vpiArrayWord*)(ref);
}

संरचना __vpiArrayWord* array_var_सूचकांक_from_handle(vpiHandle ref)
{
      यदि (ref == 0)
	    प्रत्यागम 0;
      ध्रुव __vpiArrayWord::as_सूचकांक_t*ptr = dynamic_cast<__vpiArrayWord::as_सूचकांक_t*> (ref);
      यदि (ptr == 0)
	    प्रत्यागम 0;

      प्रतिज्ञा(परिमाण(__vpiHandle) == परिमाण(__vpiArrayWord::as_सूचकांक_t));
      प्रतिज्ञा(परिमाण(__vpiHandle) == परिमाण(__vpiArrayWord::as_word_t));
      प्रत्यागम (संरचना __vpiArrayWord*)(ref-1);
}

