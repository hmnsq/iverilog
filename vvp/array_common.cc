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

#ਅਨ੍ਤਰ੍ਭਾਵ "array_common.h"

vpiHandle __vpiArrayBase::vpi_array_ਅਧਾਰ_iterate(ਪੂਰਨ code)
{
    ਚੋਣ (code) {
        ਹਾਲਤ vpiReg:
        ਹਾਲਤ vpi_ਸਮ੍ਰਿਤੀ_ਸ਼ਬਦ: {
                ਬਣਤਰ __vpiArrayIterator*res;
                res = ਨਵਾਂ __vpiArrayIterator;
                res->array = ਇਹ;
                res->ਅਗਲਾ_2 = 0;
                ਵਾਪਸ res;
        }
    }

    ਵਾਪਸ 0;
}

ਖਾਲੀ __vpiArrayBase::make_vals_ਸ਼ਬਦ()
{
    ਪ੍ਰਤਿਜ੍ਞਾ(vals_ਸ਼ਬਦ == 0);
    vals_ਸ਼ਬਦ = ਨਵਾਂ ਬਣਤਰ __vpiArrayWord[get_ਮਾਪ() + 1];

    // Make word[-1] point to the parent.
    vals_ਸ਼ਬਦ->ਮਾਪਾ = ਇਹ;
    // Now point to word-0
    vals_ਸ਼ਬਦ += 1;

    ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ = vals_ਸ਼ਬਦ;
    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < get_ਮਾਪ() ; idx += 1) {
            ਸ਼ਬਦ[idx].word0 = ਸ਼ਬਦ;
    }
}

vpiHandle __vpiArrayIterator::vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ)
{
      ਜੇ (ਅਗਲਾ_2 >= array->get_ਮਾਪ()) {
	    vpi_free_object(ਇਹ);
	    ਵਾਪਸ 0;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ use_ਸੂਚਕਾਂਕ = ਅਗਲਾ_2;
      ਅਗਲਾ_2 += 1;

      ਵਾਪਸ array->get_iter_ਸੂਚਕਾਂਕ(ਇਹ, use_ਸੂਚਕਾਂਕ);
}

ਸਥਿਰ ਪੂਰਨ array_iterator_free_object(vpiHandle ref)
{
      ਬਣਤਰ __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      ਮਿਟਾਓ obj;
      ਵਾਪਸ 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(ਖਾਲੀ)
{ ਵਾਪਸ &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(ਪੂਰਨ code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      ਜੇ (code == vpi_ਸੂਚਕਾਂਕ_2) {
	    ਬਣਤਰ __vpiArrayIndex*res;
	    res = ਨਵਾਂ __vpiArrayIndex;
	    res->ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;         // see the comment above
	    res->ਪੂਰਾ = 0;
	    ਵਾਪਸ res;
      }
      ਵਾਪਸ 0;
}

vpiHandle __vpiArrayIndex::vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ)
{
      ਜੇ (ਪੂਰਾ == 0) {
            ਪੂਰਾ = 1;
            ਵਾਪਸ ਸੂਚਕਾਂਕ;
      }

      vpi_free_object(ਇਹ);
      ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ array_ਸੂਚਕਾਂਕ_free_object(vpiHandle ref)
{
      ਬਣਤਰ __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      ਮਿਟਾਓ obj;
      ਵਾਪਸ 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(ਖਾਲੀ)
{ ਵਾਪਸ &array_ਸੂਚਕਾਂਕ_free_object; }

ਅੱਖਰ* __vpiArrayWord::as_ਸ਼ਬਦ_t::vpi_get_str(ਪੂਰਨ code)
{
      ਬਣਤਰ __vpiArrayWord*obj = array_var_ਸ਼ਬਦ_from_handle(ਇਹ);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);
      ਬਣਤਰ __vpiArrayBase*my_ਮਾਪਾ = obj->get_ਮਾਪਾ();
      ਵਾਪਸ my_ਮਾਪਾ->get_ਸ਼ਬਦ_str(obj, code);
}

ਖਾਲੀ __vpiArrayWord::as_ਸ਼ਬਦ_t::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp)
{
      ਬਣਤਰ __vpiArrayWord*obj = array_var_ਸ਼ਬਦ_from_handle(ਇਹ);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);
      ਬਣਤਰ __vpiArrayBase*my_ਮਾਪਾ = obj->get_ਮਾਪਾ();
      ਵਾਪਸ my_ਮਾਪਾ->get_ਸ਼ਬਦ_ਮੁੱਲ(obj, vp);
}

vpiHandle __vpiArrayWord::as_ਸ਼ਬਦ_t::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp, ਪੂਰਨ flags)
{
      ਬਣਤਰ __vpiArrayWord*obj = array_var_ਸ਼ਬਦ_from_handle(ਇਹ);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);
      ਬਣਤਰ __vpiArrayBase*my_ਮਾਪਾ = obj->get_ਮਾਪਾ();
      my_ਮਾਪਾ->put_ਸ਼ਬਦ_ਮੁੱਲ(obj, vp, flags);
      ਵਾਪਸ ਇਹ;
}

vpiHandle __vpiArrayWord::as_ਸ਼ਬਦ_t::vpi_handle(ਪੂਰਨ code)
{
      ਬਣਤਰ __vpiArrayWord*obj = array_var_ਸ਼ਬਦ_from_handle(ਇਹ);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);
      ਬਣਤਰ __vpiArrayBase*my_ਮਾਪਾ = obj->get_ਮਾਪਾ();

      ਚੋਣ (code) {

	  ਹਾਲਤ vpi_ਸੂਚਕਾਂਕ_2:
	    ਵਾਪਸ &(obj->as_ਸੂਚਕਾਂਕ);

	  ਹਾਲਤ vpi_ਖੱਬਾ_Range:
	    ਵਾਪਸ my_ਮਾਪਾ->get_ਖੱਬਾ_range();

	  ਹਾਲਤ vpi_ਸੱਜਾ_Range:
	    ਵਾਪਸ my_ਮਾਪਾ->get_ਸੱਜਾ_range();

	  ਹਾਲਤ vpi_ਮਾਪਾ:
	    ਵਾਪਸ dynamic_cast<vpiHandle>(my_ਮਾਪਾ);

	  ਹਾਲਤ vpiScope:
	    ਵਾਪਸ my_ਮਾਪਾ->get_scope();

	  ਹਾਲਤ vpiModule:
	    ਵਾਪਸ vpip_module(my_ਮਾਪਾ->get_scope());
      }

      ਵਾਪਸ 0;
}

ਖਾਲੀ __vpiArrayWord::as_ਸੂਚਕਾਂਕ_t::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp)
{
      ਬਣਤਰ __vpiArrayWord*obj = array_var_ਸੂਚਕਾਂਕ_from_handle(ਇਹ);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ = obj->get_ਸੂਚਕਾਂਕ();

      ਪ੍ਰਤਿਜ੍ਞਾ(vp->format == vpiIntVal);
      vp->value.integer = ਸੂਚਕਾਂਕ;
}

ਬਣਤਰ __vpiArrayWord*array_var_ਸ਼ਬਦ_from_handle(vpiHandle ref)
{
      ਜੇ (ref == 0)
	    ਵਾਪਸ 0;
      ਅਚਲ __vpiArrayWord::as_ਸ਼ਬਦ_t*ptr = dynamic_cast<__vpiArrayWord::as_ਸ਼ਬਦ_t*> (ref);
      ਜੇ (ptr == 0)
	    ਵਾਪਸ 0;

      ਵਾਪਸ (ਬਣਤਰ __vpiArrayWord*)(ref);
}

ਬਣਤਰ __vpiArrayWord* array_var_ਸੂਚਕਾਂਕ_from_handle(vpiHandle ref)
{
      ਜੇ (ref == 0)
	    ਵਾਪਸ 0;
      ਅਚਲ __vpiArrayWord::as_ਸੂਚਕਾਂਕ_t*ptr = dynamic_cast<__vpiArrayWord::as_ਸੂਚਕਾਂਕ_t*> (ref);
      ਜੇ (ptr == 0)
	    ਵਾਪਸ 0;

      ਪ੍ਰਤਿਜ੍ਞਾ(ਆਕਾਰ(__vpiHandle) == ਆਕਾਰ(__vpiArrayWord::as_ਸੂਚਕਾਂਕ_t));
      ਪ੍ਰਤਿਜ੍ਞਾ(ਆਕਾਰ(__vpiHandle) == ਆਕਾਰ(__vpiArrayWord::as_ਸ਼ਬਦ_t));
      ਵਾਪਸ (ਬਣਤਰ __vpiArrayWord*)(ref-1);
}

