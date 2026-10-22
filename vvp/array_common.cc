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

#అన్తర్భావ "array_common.h"

vpiHandle __vpiArrayBase::vpi_array_పునాది_iterate(పూర్ణాంకం code)
{
    ఎంపిక (code) {
        స్థితి vpiReg:
        స్థితి vpi_జ్ఞాపకం_మాట: {
                నిర్మాణం __vpiArrayIterator*res;
                res = కొత్త __vpiArrayIterator;
                res->array = ఇది;
                res->తదుపరి_2 = 0;
                తిరిగి res;
        }
    }

    తిరిగి 0;
}

శూన్య __vpiArrayBase::make_vals_పదాలు()
{
    ప్రతిజ్ఞా(vals_పదాలు == 0);
    vals_పదాలు = కొత్త నిర్మాణం __vpiArrayWord[get_కొలత() + 1];

    // Make word[-1] point to the parent.
    vals_పదాలు->మాతృ = ఇది;
    // Now point to word-0
    vals_పదాలు += 1;

    నిర్మాణం __vpiArrayWord*పదాలు = vals_పదాలు;
    కొరకు (గుర్తులేని idx = 0 ; idx < get_కొలత() ; idx += 1) {
            పదాలు[idx].word0 = పదాలు;
    }
}

vpiHandle __vpiArrayIterator::vpi_సూచీ(పూర్ణాంకం)
{
      ఒకవేళ (తదుపరి_2 >= array->get_కొలత()) {
	    vpi_free_object(ఇది);
	    తిరిగి 0;
      }

      గుర్తులేని use_సూచీ = తదుపరి_2;
      తదుపరి_2 += 1;

      తిరిగి array->get_iter_సూచీ(ఇది, use_సూచీ);
}

స్థిర పూర్ణాంకం array_iterator_free_object(vpiHandle ref)
{
      నిర్మాణం __vpiArrayIterator*obj = dynamic_cast<__vpiArrayIterator*>(ref);
      తొలగించు obj;
      తిరిగి 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIterator::free_object_fun(శూన్య)
{ తిరిగి &array_iterator_free_object; }

vpiHandle __vpiArrayIndex::vpi_iterate(పూర్ణాంకం code)
{
    // TODO originally, __vpiArrayIndex is casted to __vpiDecConst and assigned
    // to res->index - seems like a bug to me

      ఒకవేళ (code == vpi_సూచీ_2) {
	    నిర్మాణం __vpiArrayIndex*res;
	    res = కొత్త __vpiArrayIndex;
	    res->సూచీ = సూచీ;         // see the comment above
	    res->పూర్తి = 0;
	    తిరిగి res;
      }
      తిరిగి 0;
}

vpiHandle __vpiArrayIndex::vpi_సూచీ(పూర్ణాంకం)
{
      ఒకవేళ (పూర్తి == 0) {
            పూర్తి = 1;
            తిరిగి సూచీ;
      }

      vpi_free_object(ఇది);
      తిరిగి 0;
}

స్థిర పూర్ణాంకం array_సూచీ_free_object(vpiHandle ref)
{
      నిర్మాణం __vpiArrayIndex*obj = dynamic_cast<__vpiArrayIndex*>(ref);
      తొలగించు obj;
      తిరిగి 1;
}

__vpiHandle::free_object_fun_t __vpiArrayIndex::free_object_fun(శూన్య)
{ తిరిగి &array_సూచీ_free_object; }

అక్షరం* __vpiArrayWord::as_మాట_t::vpi_get_str(పూర్ణాంకం code)
{
      నిర్మాణం __vpiArrayWord*obj = array_var_మాట_from_handle(ఇది);
      ప్రతిజ్ఞా(obj);
      నిర్మాణం __vpiArrayBase*my_మాతృ = obj->get_మాతృ();
      తిరిగి my_మాతృ->get_మాట_str(obj, code);
}

శూన్య __vpiArrayWord::as_మాట_t::vpi_get_విలువ(p_vpi_విలువ vp)
{
      నిర్మాణం __vpiArrayWord*obj = array_var_మాట_from_handle(ఇది);
      ప్రతిజ్ఞా(obj);
      నిర్మాణం __vpiArrayBase*my_మాతృ = obj->get_మాతృ();
      తిరిగి my_మాతృ->get_మాట_విలువ(obj, vp);
}

vpiHandle __vpiArrayWord::as_మాట_t::vpi_put_విలువ(p_vpi_విలువ vp, పూర్ణాంకం flags)
{
      నిర్మాణం __vpiArrayWord*obj = array_var_మాట_from_handle(ఇది);
      ప్రతిజ్ఞా(obj);
      నిర్మాణం __vpiArrayBase*my_మాతృ = obj->get_మాతృ();
      my_మాతృ->put_మాట_విలువ(obj, vp, flags);
      తిరిగి ఇది;
}

vpiHandle __vpiArrayWord::as_మాట_t::vpi_handle(పూర్ణాంకం code)
{
      నిర్మాణం __vpiArrayWord*obj = array_var_మాట_from_handle(ఇది);
      ప్రతిజ్ఞా(obj);
      నిర్మాణం __vpiArrayBase*my_మాతృ = obj->get_మాతృ();

      ఎంపిక (code) {

	  స్థితి vpi_సూచీ_2:
	    తిరిగి &(obj->as_సూచీ);

	  స్థితి vpi_ఎడమ_Range:
	    తిరిగి my_మాతృ->get_ఎడమ_range();

	  స్థితి vpi_కుడి_Range:
	    తిరిగి my_మాతృ->get_కుడి_range();

	  స్థితి vpi_మాతృ:
	    తిరిగి dynamic_cast<vpiHandle>(my_మాతృ);

	  స్థితి vpiScope:
	    తిరిగి my_మాతృ->get_scope();

	  స్థితి vpiModule:
	    తిరిగి vpip_module(my_మాతృ->get_scope());
      }

      తిరిగి 0;
}

శూన్య __vpiArrayWord::as_సూచీ_t::vpi_get_విలువ(p_vpi_విలువ vp)
{
      నిర్మాణం __vpiArrayWord*obj = array_var_సూచీ_from_handle(ఇది);
      ప్రతిజ్ఞా(obj);
      గుర్తులేని సూచీ = obj->get_సూచీ();

      ప్రతిజ్ఞా(vp->format == vpiIntVal);
      vp->value.integer = సూచీ;
}

నిర్మాణం __vpiArrayWord*array_var_మాట_from_handle(vpiHandle ref)
{
      ఒకవేళ (ref == 0)
	    తిరిగి 0;
      స్థిరరాశి __vpiArrayWord::as_మాట_t*ptr = dynamic_cast<__vpiArrayWord::as_మాట_t*> (ref);
      ఒకవేళ (ptr == 0)
	    తిరిగి 0;

      తిరిగి (నిర్మాణం __vpiArrayWord*)(ref);
}

నిర్మాణం __vpiArrayWord* array_var_సూచీ_from_handle(vpiHandle ref)
{
      ఒకవేళ (ref == 0)
	    తిరిగి 0;
      స్థిరరాశి __vpiArrayWord::as_సూచీ_t*ptr = dynamic_cast<__vpiArrayWord::as_సూచీ_t*> (ref);
      ఒకవేళ (ptr == 0)
	    తిరిగి 0;

      ప్రతిజ్ఞా(పరిమాణం(__vpiHandle) == పరిమాణం(__vpiArrayWord::as_సూచీ_t));
      ప్రతిజ్ఞా(పరిమాణం(__vpiHandle) == పరిమాణం(__vpiArrayWord::as_మాట_t));
      తిరిగి (నిర్మాణం __vpiArrayWord*)(ref-1);
}

