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

vpiHandle __vpiArrayBase::vpi_array_ತಳ_iterate(ಪೂರ್ಣಾಂಕ code)
{
    ಆಯ್ಕೆ (code) {
        ಸ್ಥಿತಿ vpiReg:
        ಸ್ಥಿತಿ vpi_ಸ್ಮರಣೆ_ಶಬ್ದ: {
                ರಚನೆ __vpiArrayIterator*res;
                res = ಹೊಸ __vpiArrayIterator;
                res->array = ಇದು;
                res->ಮುಂದಿನ_2 = 0;
                ಹಿಂತಿರುಗಿ res;
        }
    }

    ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ __vpiArrayBase::make_vals_ಪದಗಳು()
{
    ಪ್ರತಿಜ್ಞಾ(vals_ಪದಗಳು == 0);
    vals_ಪದಗಳು = ಹೊಸ ರಚನೆ __vpiArrayWord[get_ಗಾತ್ರ() + 1];

    // Make word[-1] point to the parent.
    vals_ಪದಗಳು->ಪೋಷಕ = ಇದು;
    // Now point to word-0
    vals_ಪದಗಳು += 1;

    ರಚನೆ __vpiArrayWord*ಪದಗಳು = vals_ಪದಗಳು;
    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < get_ಗಾತ್ರ() ; idx += 1) {
            ಪದಗಳು[idx].word0 = ಪದಗಳು;
    }
}

vpiHandle __vpiArrayIterator::vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ)
{
      ಆದರೆ (ಮುಂದಿನ_2 >= array->get_ಗಾತ್ರ()) {
	    vpi_free_object(ಇದು);
	    ಹಿಂತಿರುಗಿ 0;
      }

      ಚಿಹ್ನೆರಹಿತ use_ಸೂಚ್ಯಂಕ = ಮುಂದಿನ_2;
      ಮುಂದಿನ_2 += 1;

      ಹಿಂತಿರುಗಿ array->get_iter_ಸೂಚ್ಯಂಕ(ಇದು, use_ಸೂಚ್ಯಂಕ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ array_iterator_free_object(vpiHandle ref)
{
      ರಚನೆ __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      ಅಳಿಸು obj;
      ಹಿಂತಿರುಗಿ 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(ಶೂನ್ಯ)
{ ಹಿಂತಿರುಗಿ &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(ಪೂರ್ಣಾಂಕ code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      ಆದರೆ (code == vpi_ಸೂಚ್ಯಂಕ_2) {
	    ರಚನೆ __vpiArrayIndex*res;
	    res = ಹೊಸ __vpiArrayIndex;
	    res->ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;         // see the comment above
	    res->ಮುಗಿದ = 0;
	    ಹಿಂತಿರುಗಿ res;
      }
      ಹಿಂತಿರುಗಿ 0;
}

vpiHandle __vpiArrayIndex::vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ)
{
      ಆದರೆ (ಮುಗಿದ == 0) {
            ಮುಗಿದ = 1;
            ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
      }

      vpi_free_object(ಇದು);
      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ array_ಸೂಚ್ಯಂಕ_free_object(vpiHandle ref)
{
      ರಚನೆ __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      ಅಳಿಸು obj;
      ಹಿಂತಿರುಗಿ 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(ಶೂನ್ಯ)
{ ಹಿಂತಿರುಗಿ &array_ಸೂಚ್ಯಂಕ_free_object; }

ಅಕ್ಷರ* __vpiArrayWord::as_ಶಬ್ದ_t::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ರಚನೆ __vpiArrayWord*obj = array_var_ಶಬ್ದ_from_handle(ಇದು);
      ಪ್ರತಿಜ್ಞಾ(obj);
      ರಚನೆ __vpiArrayBase*my_ಪೋಷಕ = obj->get_ಪೋಷಕ();
      ಹಿಂತಿರುಗಿ my_ಪೋಷಕ->get_ಶಬ್ದ_str(obj, code);
}

ಶೂನ್ಯ __vpiArrayWord::as_ಶಬ್ದ_t::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      ರಚನೆ __vpiArrayWord*obj = array_var_ಶಬ್ದ_from_handle(ಇದು);
      ಪ್ರತಿಜ್ಞಾ(obj);
      ರಚನೆ __vpiArrayBase*my_ಪೋಷಕ = obj->get_ಪೋಷಕ();
      ಹಿಂತಿರುಗಿ my_ಪೋಷಕ->get_ಶಬ್ದ_ಮೌಲ್ಯ(obj, vp);
}

vpiHandle __vpiArrayWord::as_ಶಬ್ದ_t::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ flags)
{
      ರಚನೆ __vpiArrayWord*obj = array_var_ಶಬ್ದ_from_handle(ಇದು);
      ಪ್ರತಿಜ್ಞಾ(obj);
      ರಚನೆ __vpiArrayBase*my_ಪೋಷಕ = obj->get_ಪೋಷಕ();
      my_ಪೋಷಕ->put_ಶಬ್ದ_ಮೌಲ್ಯ(obj, vp, flags);
      ಹಿಂತಿರುಗಿ ಇದು;
}

vpiHandle __vpiArrayWord::as_ಶಬ್ದ_t::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{
      ರಚನೆ __vpiArrayWord*obj = array_var_ಶಬ್ದ_from_handle(ಇದು);
      ಪ್ರತಿಜ್ಞಾ(obj);
      ರಚನೆ __vpiArrayBase*my_ಪೋಷಕ = obj->get_ಪೋಷಕ();

      ಆಯ್ಕೆ (code) {

	  ಸ್ಥಿತಿ vpi_ಸೂಚ್ಯಂಕ_2:
	    ಹಿಂತಿರುಗಿ &(obj->as_ಸೂಚ್ಯಂಕ);

	  ಸ್ಥಿತಿ vpi_ಎಡ_Range:
	    ಹಿಂತಿರುಗಿ my_ಪೋಷಕ->get_ಎಡ_range();

	  ಸ್ಥಿತಿ vpi_ಬಲ_Range:
	    ಹಿಂತಿರುಗಿ my_ಪೋಷಕ->get_ಬಲ_range();

	  ಸ್ಥಿತಿ vpi_ಪೋಷಕ:
	    ಹಿಂತಿರುಗಿ dynamic_cast<vpiHandle>(my_ಪೋಷಕ);

	  ಸ್ಥಿತಿ vpiScope:
	    ಹಿಂತಿರುಗಿ my_ಪೋಷಕ->get_scope();

	  ಸ್ಥಿತಿ vpiModule:
	    ಹಿಂತಿರುಗಿ vpip_module(my_ಪೋಷಕ->get_scope());
      }

      ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ __vpiArrayWord::as_ಸೂಚ್ಯಂಕ_t::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      ರಚನೆ __vpiArrayWord*obj = array_var_ಸೂಚ್ಯಂಕ_from_handle(ಇದು);
      ಪ್ರತಿಜ್ಞಾ(obj);
      ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ = obj->get_ಸೂಚ್ಯಂಕ();

      ಪ್ರತಿಜ್ಞಾ(vp->format == vpiIntVal);
      vp->value.integer = ಸೂಚ್ಯಂಕ;
}

ರಚನೆ __vpiArrayWord*array_var_ಶಬ್ದ_from_handle(vpiHandle ref)
{
      ಆದರೆ (ref == 0)
	    ಹಿಂತಿರುಗಿ 0;
      ಸ್ಥಿರಾಂಕ __vpiArrayWord::as_ಶಬ್ದ_t*ptr = dynamic_cast<__vpiArrayWord::as_ಶಬ್ದ_t*> (ref);
      ಆದರೆ (ptr == 0)
	    ಹಿಂತಿರುಗಿ 0;

      ಹಿಂತಿರುಗಿ (ರಚನೆ __vpiArrayWord*)(ref);
}

ರಚನೆ __vpiArrayWord* array_var_ಸೂಚ್ಯಂಕ_from_handle(vpiHandle ref)
{
      ಆದರೆ (ref == 0)
	    ಹಿಂತಿರುಗಿ 0;
      ಸ್ಥಿರಾಂಕ __vpiArrayWord::as_ಸೂಚ್ಯಂಕ_t*ptr = dynamic_cast<__vpiArrayWord::as_ಸೂಚ್ಯಂಕ_t*> (ref);
      ಆದರೆ (ptr == 0)
	    ಹಿಂತಿರುಗಿ 0;

      ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ(__vpiHandle) == ಗಾತ್ರ(__vpiArrayWord::as_ಸೂಚ್ಯಂಕ_t));
      ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ(__vpiHandle) == ಗಾತ್ರ(__vpiArrayWord::as_ಶಬ್ದ_t));
      ಹಿಂತಿರುಗಿ (ರಚನೆ __vpiArrayWord*)(ref-1);
}

