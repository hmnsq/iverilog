/*
 * Copyright (c) 2008-2026 Stephen Williams (steve@icarus.com)
 * Copyright (c) 2023 Leo Moser (leo.moser@pm.me)
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

# include  "version_base.h"
# include  "vpi_priv.h"
# include  "schedule.h"
# include  "logic.h"
# include  "part.h"
# include  "concat.h"
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <vector>
# include  <cstdio>
# include  <cstdarg>
# include  <cstring>
# include  <cassert>
# include  <cstdlib>
# include  <cmath>
# include  <iostream>

using नामाकाश std;
vpi_mode_t vpi_mode_flag = VPI_MODE_NONE;
खाता*vpi_trace = 0;

स्थिर s_vpi_vlog_info  vpi_vlog_info;
स्थिर s_vpi_error_info vpip_last_error = { 0, 0, 0, 0, 0, 0, 0 };

ध्रुव वर्ण* direction_as_string(पूर्णाङ्क dir)
{
      विकल्प (dir) {
	  पक्ष vpi_निवेश:
	    प्रत्यागम "input";
	  पक्ष vpiOutput:
	    प्रत्यागम "output";
	  पक्ष vpiInout:
	    प्रत्यागम "inout";
	  पक्ष vpiMixedIO:
	    प्रत्यागम "mixed io";
	  पक्ष vpiNoDirection:
	    प्रत्यागम "no direction";
	  स्वभाव:
	    प्रत्यागम "INVALID-DIRECTION";
      }
}

__vpiHandle::~__vpiHandle()
{ }

पूर्णाङ्क __vpiHandle::vpi_get(पूर्णाङ्क)
{ प्रत्यागम vpiUndefined; }

int64_t __vpiHandle::vpi_get64(पूर्णाङ्क code)
{ प्रत्यागम (code == _vpiNexusId) ? 0 : vpiUndefined; }

वर्ण* __vpiHandle::vpi_get_str(पूर्णाङ्क)
{ प्रत्यागम 0; }

शून्य __vpiHandle::vpi_get_मूल्य(p_vpi_मूल्य)
{ }

vpiHandle __vpiHandle::vpi_put_मूल्य(p_vpi_मूल्य, पूर्णाङ्क)
{ प्रत्यागम 0; }

vpiHandle __vpiHandle::vpi_handle(पूर्णाङ्क)
{ प्रत्यागम 0; }

vpiHandle __vpiHandle::vpi_iterate(पूर्णाङ्क)
{ प्रत्यागम 0; }

vpiHandle __vpiHandle::vpi_index(पूर्णाङ्क)
{ प्रत्यागम 0; }

शून्य __vpiHandle::vpi_get_delays(p_vpi_delay)
{ }

शून्य __vpiHandle::vpi_put_delays(p_vpi_delay)
{ }

__vpiBaseVar::__vpiBaseVar(__vpiScope*scope, ध्रुव वर्ण*name, vvp_net_t*net)
: scope_(scope), name_(name), net_(net)
{
}

#ifdef CHECK_WITH_VALGRIND
__vpiBaseVar::~__vpiBaseVar()
{
      vvp_net_delete(net_);
}
#endif

/*
 * The default behavior for the vpi_free_object to an object is to
 * suppress the actual operation. This is because handles are
 * generally allocated semi-permanently within vvp context. Dynamic
 * objects will override the free_object_fun method to return an
 * appropriately effective function.
 */
स्थिर पूर्णाङ्क suppress_free(vpiHandle)
{ प्रत्यागम 1; }
__vpiHandle::free_object_fun_t __vpiHandle::free_object_fun(शून्य)
{ प्रत्यागम &suppress_free; }

/*
 * The vpip_string function creates a constant string from the pass
 * input. This constant string is permanently allocated from an
 * efficient string buffer store.
 */
संरचना vpip_string_chunk {
      संरचना vpip_string_chunk*next;
      वर्ण data[64*1024 - परिमाण (संरचना vpip_string_chunk*)];
};

अचिन्हित vpip_size(ध्रुव __vpiSignal *sig)
{
      प्रत्यागम असल(sig->msb.get_मूल्य() - sig->lsb.get_मूल्य()) + 1;
}

__vpiScope* vpip_scope(__vpiSignal*sig)
{
      यदि (sig->is_netarray)
	    प्रत्यागम static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      अन्यथा
	    प्रत्यागम sig->within.scope;
}

__vpiScope* vpip_scope(__vpiRealVar*sig)
{
      यदि (sig->is_netarray)
	    प्रत्यागम static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      अन्यथा
	    प्रत्यागम sig->within.scope;
}

vpiHandle vpip_module(__vpiScope*scope)
{
      यावत्(scope && scope->get_type_code() != vpiModule) {
	    scope = scope->scope;
      }
      प्रतिज्ञा(scope);
      प्रत्यागम scope;
}

ध्रुव वर्ण *vpip_string(ध्रुव वर्ण*str)
{
      स्थिर vpip_string_chunk first_chunk = {0, {0}};
      स्थिर vpip_string_chunk*chunk_list = &first_chunk;
      स्थिर अचिन्हित chunk_fill = 0;

      अचिन्हित len = म_माप(str);
      प्रतिज्ञा( (len+1) <= परिमाण chunk_list->data );

      यदि ( (len+1) > (परिमाण chunk_list->data - chunk_fill) ) {
	    vpip_string_chunk*tmp = नूतन vpip_string_chunk;
	    tmp->next = chunk_list;
	    chunk_list = tmp;
	    chunk_fill = 0;
      }

      वर्ण*res = chunk_list->data + chunk_fill;
      chunk_fill += len + 1;

      म_नकल(res, str);
      प्रत्यागम res;
}

स्थिर अचिन्हित hash_string(ध्रुव वर्ण*text)
{
      अचिन्हित h = 0;

      यावत् (*text) {
	    h = (h << 4) ^ (h >> 28) ^ (अचिन्हित)*text;
	    text += 1;
      }
      प्रत्यागम h;
}

ध्रुव वर्ण* vpip_name_string(ध्रुव वर्ण*text)
{
      ध्रुव अचिन्हित HASH_SIZE = 4096;
      स्थिर ध्रुव वर्ण*hash_table[HASH_SIZE] = {0};

      अचिन्हित hash_मूल्य = hash_string(text) % HASH_SIZE;

	/* If we easily find the string in the hash table, then return
	   that and be done. */
      यदि (hash_table[hash_मूल्य]
	  && (म_भेद(hash_table[hash_मूल्य], text) == 0)) {
	    प्रत्यागम hash_table[hash_मूल्य];
      }

	/* The existing hash entry is not a match. Replace it with the
	   newly allocated value, and return the new pointer as the
	   result to the add. */
      ध्रुव वर्ण*res = vpip_string(text);
      hash_table[hash_मूल्य] = res;

      प्रत्यागम res;
}

PLI_INT32 vpi_chk_error(p_vpi_error_info info)
{
      यदि (vpip_last_error.state == 0)
	    प्रत्यागम 0;

      info->state = vpip_last_error.state;
      info->level = vpip_last_error.level;
      info->message = vpip_last_error.message;
      info->product = vpi_vlog_info.product;
      info->code = (वर्ण *)"";
      info->file = 0;
      info->पंक्ति = 0;

      प्रत्यागम info->level;
}

PLI_INT32 vpi_compare_objects(vpiHandle obj1, vpiHandle obj2)
{
      प्रतिज्ञा(obj1);
      प्रतिज्ञा(obj2);

	// Does this actually work for all cases?
      यदि (obj1 != obj2) प्रत्यागम 0;
      अन्यथा प्रत्यागम 1;
}

/*
 * Copy the internal information to the data structure. Do not free or
 * change the tfname/user_data since they are a pointer to the real
 * string/data values. We also support passing a task or function handle
 * instead of just a handle to a vpiUserSystf.
 */
शून्य vpi_get_systf_info(vpiHandle ref, p_vpi_systf_data data)
{
      संरचना __vpiUserSystf* rfp = dynamic_cast<__vpiUserSystf*>(ref);
      यदि (rfp == 0) {
	    संरचना __vpiSysTaskCall*call = dynamic_cast<__vpiSysTaskCall*>(ref);
	    प्रतिज्ञा(call);
	    rfp = call->defn;
      }

	/* Assert that vpiUserDefn is true! */
      प्रतिज्ञा(rfp->is_user_defn);

      data->type = rfp->info.type;
      data->sysfunctype = rfp->info.sysfunctype;
      data->tfname = rfp->info.tfname;
      data->calltf = rfp->info.calltf;
      data->compiletf = rfp->info.compiletf;
      data->sizetf = rfp->info.sizetf;
      data->user_data = rfp->info.user_data;
}

/*
 * When a task is called, this value is set so that vpi_handle can
 * fathom the vpi_handle(vpiSysTfCall,0) function.
 */
संरचना __vpiSysTaskCall*vpip_cur_task = 0;

PLI_INT32 vpi_free_object(vpiHandle ref)
{
      पूर्णाङ्क rtn;

      यदि (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_free_object(%p)", ref);
	    ख_साफ(vpi_trace);
      }

      प्रतिज्ञा(ref);
      __vpiHandle::free_object_fun_t fun = ref->free_object_fun();
      rtn = fun (ref);

      यदि (vpi_trace)
	    ख_लिखो(vpi_trace, " --> %d\n", rtn);

      प्रत्यागम rtn;
}

PLI_INT32 vpi_release_handle(vpiHandle ref)
{
	// Since SystemVerilog vpi_free_object() has been
	// renamed vpi_release_handle(), and thus
	// vpi_free_object() has been deprecated.
      प्रत्यागम vpi_free_object(ref);
}

स्थिर पूर्णाङ्क vpip_get_global(पूर्णाङ्क property)
{
      विकल्प (property) {

	  पक्ष vpiTimeUnit:
	  पक्ष vpiTimePrecision:
	    प्रत्यागम vpip_get_time_precision();

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "vpi error: bad global property: %d\n", property);
	    प्रतिज्ञा(0);
	    प्रत्यागम vpiUndefined;
      }
}

स्थिर ध्रुव वर्ण* vpi_property_str(PLI_INT32 code)
{
      स्थिर वर्ण buf[32];
      विकल्प (code) {
	  पक्ष vpiConstType:
	    प्रत्यागम "vpiConstType";
	  पक्ष vpiName:
	    प्रत्यागम "vpiName";
	  पक्ष vpiFullName:
	    प्रत्यागम "vpiFullName";
	  पक्ष vpiTimeUnit:
	    प्रत्यागम "vpiTimeUnit";
	  पक्ष vpiTimePrecision:
	    प्रत्यागम "vpiTimePrecision";
          पक्ष vpiSize:
	    प्रत्यागम "vpiSize";
          पक्ष _vpiNexusId:
	    प्रत्यागम "_vpiNexusId";
	  स्वभाव:
	    snprintf(buf, परिमाण(buf), "%d", (पूर्णाङ्क)code);
      }
      प्रत्यागम buf;
}

ध्रुव वर्ण* vpi_type_as_string(PLI_INT32 code)
{
      स्थिर वर्ण buf[32];
      विकल्प (code) {
	  पक्ष vpiArrayType:
	    प्रत्यागम "vpiArrayType";
	  पक्ष vpiBitVar:
	    प्रत्यागम "vpiBitVar";
	  पक्ष vpiByteVar:
	    प्रत्यागम "vpiByteVar";
	  पक्ष vpiClassVar:
	    प्रत्यागम "vpiClassVar";
	  पक्ष vpiConstant:
	    प्रत्यागम "vpiConstant";
	  पक्ष vpiEnumTypespec:
	    प्रत्यागम "vpiEnumTypespec";
	  पक्ष vpiFunction:
	    प्रत्यागम "vpiFunction";
	  पक्ष vpiGenScope:
	    प्रत्यागम "vpiGenScope";
	  पक्ष vpiIntVar:
	    प्रत्यागम "vpiIntVar";
	  पक्ष vpiIntegerVar:
	    प्रत्यागम "vpiIntegerVar";
	  पक्ष vpiIterator:
	    प्रत्यागम "vpiIterator";
	  पक्ष vpiLongIntVar:
	    प्रत्यागम "vpiLongIntVar";
	  पक्ष vpiMemory:
	    प्रत्यागम "vpiMemory";
	  पक्ष vpiMemoryWord:
	    प्रत्यागम "vpiMemoryWord";
	  पक्ष vpiModule:
	    प्रत्यागम "vpiModule";
	  पक्ष vpiNamedBegin:
	    प्रत्यागम "vpiNamedBegin";
	  पक्ष vpiNamedEvent:
	    प्रत्यागम "vpiNamedEvent";
	  पक्ष vpiNamedFork:
	    प्रत्यागम "vpiNamedFork";
	  पक्ष vpiPackage:
	    प्रत्यागम "vpiPackage";
	  पक्ष vpiPathTerm:
	    प्रत्यागम "vpiPathTerm";
	  पक्ष vpiPort:
	    प्रत्यागम "vpiPort";
	  पक्ष vpiNet:
	    प्रत्यागम "vpiNet";
	  पक्ष vpiNetArray:
	    प्रत्यागम "vpiNetArray";
	  पक्ष vpiNetBit:
	    प्रत्यागम "vpiNetBit";
	  पक्ष vpiParameter:
	    प्रत्यागम "vpiParameter";
	  पक्ष vpiPartSelect:
	    प्रत्यागम "vpiPartSelect";
	  पक्ष vpiRealVar:
	    प्रत्यागम "vpiRealVar";
	  पक्ष vpiReg:
	    प्रत्यागम "vpiReg";
	  पक्ष vpiRegBit:
	    प्रत्यागम "vpiRegBit";
	  पक्ष vpiShortIntVar:
	    प्रत्यागम "vpiShortIntVar";
	  पक्ष vpiStringVar:
	    प्रत्यागम "vpiStringVar";
	  पक्ष vpiSysFuncCall:
	    प्रत्यागम "vpiSysFuncCall";
	  पक्ष vpiSysTaskCall:
	    प्रत्यागम "vpiSysTaskCall";
	  पक्ष vpiTask:
	    प्रत्यागम "vpiTask";
	  पक्ष vpiTimeVar:
	    प्रत्यागम "vpiTimeVar";
	  पक्ष vpiUserSystf:
	    प्रत्यागम "vpiUserSystf";
	  स्वभाव:
	    snprintf(buf, परिमाण(buf), "%d", (पूर्णाङ्क)code);
      }
      प्रत्यागम buf;
}

PLI_INT32 vpi_get(पूर्णाङ्क property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      यदि (property == _vpiDelaySelection) प्रत्यागम vpip_delay_selection;

      यदि (ref == 0)
	    प्रत्यागम vpip_get_global(property);

      यदि (property == vpiType) {
	    यदि (vpi_trace) {
		  ख_लिखो(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

	    यदि (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  प्रत्यागम vpiNetArray;
	    अन्यथा
		  प्रत्यागम ref->get_type_code();
      }

      पूर्णाङ्क res = ref->vpi_get(property);

      यदि (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_get(%s, %p) --> %d\n",
		    vpi_property_str(property), ref, res);
      }

      प्रत्यागम res;
}

PLI_INT64 vpi_get64(पूर्णाङ्क property, vpiHandle ref)
{
	// For now we only support getting the nexus id.
      यदि ((ref == 0) || (property != _vpiNexusId)) {
	    ख_लिखो(vpi_trace, "vpi_get64(%s, %p) is not currently supported.\n",
		    vpi_property_str(property), ref);
	    प्रत्यागम vpiUndefined;
      }

      int64_t res = ref->vpi_get64(property);

      यदि (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_get64(%s, %p) --> %" PRId64 "\n",
		    vpi_property_str(property), ref, res);
      }

      प्रत्यागम res;
}

वर्ण* vpi_get_str(PLI_INT32 property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      यदि (property == _vpiDelaySelection) {
	    विकल्प (vpip_delay_selection) {
		पक्ष _vpiDelaySelMinimum:
		  प्रत्यागम simple_set_rbuf_str("MINIMUM");
		पक्ष _vpiDelaySelTypical:
		  प्रत्यागम simple_set_rbuf_str("TYPICAL");
		पक्ष _vpiDelaySelMaximum:
		  प्रत्यागम simple_set_rbuf_str("MAXIMUM");
		स्वभाव:
		  प्रतिज्ञा(0);
	    }
      }

      यदि (ref == 0) {
	    ख_लिखो(मानक_त्रुटि, "vpi error: vpi_get_str(%s, 0) called "
		    "with null vpiHandle.\n", vpi_property_str(property));
	    प्रत्यागम 0;
      }

      यदि (property == vpiType) {
	    यदि (vpi_trace) {
		  ख_लिखो(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

            PLI_INT32 type;
	    यदि (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  type = vpiNetArray;
	    अन्यथा
		  type = ref->get_type_code();
	    प्रत्यागम (वर्ण *)vpi_type_as_string(type);
      }

      वर्ण*res = ref->vpi_get_str(property);

      यदि (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_get_str(%s, %p) --> %s\n",
		    vpi_property_str(property), ref, res? res : "<NULL>");
      }

      प्रत्यागम res;
}

__vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj)
{
      ध्रुव संरचना __vpiSysTaskCall*task;
      संरचना __vpiSignal*सङ्केत;
      संरचना __vpiRealVar*real;
      ध्रुव __vpiNamedEvent*event;

      विकल्प (obj->get_type_code()) {
	  पक्ष vpiSysTaskCall:
	    task = dynamic_cast<__vpiSysTaskCall*>(obj);
	    प्रत्यागम task->scope;

	  पक्ष vpiModule:
	    प्रत्यागम dynamic_cast<__vpiScope*>(obj);

	  पक्ष vpiNet:
	  पक्ष vpiReg:
	  पक्ष vpiIntegerVar:
	  पक्ष vpiBitVar:
	  पक्ष vpiByteVar:
	  पक्ष vpiShortIntVar:
	  पक्ष vpiIntVar:
	  पक्ष vpiLongIntVar:
	    सङ्केत = dynamic_cast<__vpiSignal*>(obj);
	    प्रत्यागम vpip_scope(सङ्केत);

	  पक्ष vpiRealVar:
	    real = dynamic_cast<__vpiRealVar*>(obj);
	    प्रत्यागम vpip_scope(real);

	  पक्ष vpiNamedEvent:
	    event = dynamic_cast<__vpiNamedEvent*>(obj);
	    प्रत्यागम event->get_scope();

	  पक्ष vpiMemory:
	  पक्ष vpiMemoryWord:
	  पक्ष vpiPartSelect:
	    प्रत्यागम dynamic_cast<__vpiScope*>(obj->vpi_handle(vpiScope));

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "ERROR: vpip_scope_from_handle called with "
		    "object handle type=%d\n", obj->get_type_code());
	    प्रतिज्ञा(0);
	    प्रत्यागम 0;
      }
}

पूर्णाङ्क vpip_time_units_from_handle(vpiHandle obj)
{
      यदि (obj == 0)
	    प्रत्यागम vpip_get_time_precision();

      ध्रुव __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      यदि (scope == 0)
	    प्रत्यागम vpip_get_time_precision();

      प्रत्यागम scope->time_units;
}

पूर्णाङ्क vpip_time_precision_from_handle(vpiHandle obj)
{
      यदि (obj == 0)
	    प्रत्यागम vpip_get_time_precision();

      ध्रुव __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      यदि (scope == 0)
	    प्रत्यागम vpip_get_time_precision();

      प्रत्यागम scope->time_precision;
}

द्विगुण vpip_scaled_time_from_handle(vvp_time64_t समय, vpiHandle obj)
{
      पूर्णाङ्क scale = vpip_get_time_precision() -
	          vpip_time_units_from_handle(obj);
      यदि (scale >= 0)
	    प्रत्यागम (द्विगुण)समय * घात(10.0, scale);
      अन्यथा
	    प्रत्यागम (द्विगुण)समय / घात(10.0, -scale);
}

शून्य vpi_get_time(vpiHandle obj, s_vpi_time*vp)
{
      vvp_time64_t समय;

      प्रतिज्ञा(vp);

      समय = schedule_simtime();

      विकल्प (vp->type) {
          पक्ष vpiSimTime:
	    vp->ऊपरी = (समय >> 32) & 0xffffffff;
	    vp->निचला = समय & 0xffffffff;
	    भङ्ग;

          पक्ष vpiScaledRealTime:
	    vp->real = vpip_scaled_time_from_handle(समय, obj);
	    भङ्ग;

          स्वभाव:
            ख_लिखो(मानक_त्रुटि, "vpi_get_time: unknown type: %d\n", (पूर्णाङ्क)vp->type);
            प्रतिज्ञा(0);
	    भङ्ग;
      }
}

PLI_INT32 vpi_get_vlog_info(p_vpi_vlog_info vlog_info_p)
{
    यदि (vlog_info_p != 0) {
	  *vlog_info_p = vpi_vlog_info;
	  प्रत्यागम 1;
    } अन्यथा {
	  प्रत्यागम 0;
    }
}

शून्य vpi_set_vlog_info(पूर्णाङ्क argc, वर्ण** argv)
{
    स्थिर वर्ण icarus_product[] = "Icarus Verilog";
    स्थिर वर्ण icarus_version[] = VERSION;
    vpi_vlog_info.product = icarus_product;
    vpi_vlog_info.version = icarus_version;
    vpi_vlog_info.argc    = argc;
    vpi_vlog_info.argv    = argv;

    स्थिर वर्ण trace_buf[1024];
    यदि (ध्रुव वर्ण*path = दो_पर्या("VPI_TRACE"))  {
	  यदि (!म_भेद(path,"-"))
		vpi_trace = मानक_निकास;
	  अन्यथा {
		vpi_trace = ख_खोलो(path, "w");
		यदि (!vpi_trace) {
		      लिखो_त्रुटि(path);
		      निकास(1);
		}
		रखो_भबफ(vpi_trace, trace_buf, _IOLBF, परिमाण(trace_buf));
	  }
    }
}

स्थिर शून्य vec4_get_मूल्य_string(ध्रुव vvp_vector4_t&word_val, अचिन्हित width,
				  s_vpi_मूल्य*vp)
{
      अचिन्हित nchar = width / 8;
      अचिन्हित tail = width % 8;

      वर्ण*rbuf = static_cast<वर्ण *>(need_result_buf(nchar + 1, RBUF_VAL));
      वर्ण*cp = rbuf;

      यदि (tail > 0) {
	    वर्ण char_val = 0;
	    प्रति (अचिन्हित idx = width-tail; idx < width ;  idx += 1) {
		  vvp_bit4_t val = word_val.value(idx);
		  यदि (val == BIT4_1)
			char_val |= 1 << idx;
	    }

	    यदि (char_val != 0)
		  *cp++ = char_val;
	    अन्यथा यदि (cp != rbuf)
		  *cp++ = ' ';
      }

      प्रति (अचिन्हित idx = 0 ;  idx < nchar ;  idx += 1) {
	    अचिन्हित bit = (nchar - idx - 1) * 8;
	    वर्ण char_val = 0;
	    प्रति (अचिन्हित bdx = 0 ;  bdx < 8 ;  bdx += 1) {
		  vvp_bit4_t val = word_val.value(bit+bdx);
		  यदि (val == BIT4_1)
			char_val |= 1 << bdx;
	    }
	    // Ignore leading null-bytes and replace other null-bytes with space.
	    // The LRM is not entirely clear on how null bytes should be handled.
	    // This is the implementation chosen for iverilog.
	    यदि (char_val != 0)
		  *cp++ = char_val;
	    अन्यथा यदि (cp != rbuf)
		  *cp++ = ' ';
      }

      *cp = 0;
      vp->value.str = rbuf;
}

/*
 * This is a generic function to convert a vvp_vector4_t value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer. The width is the real width of the word, in case the
 * word_val width is not accurate.
 */
शून्य vpip_vec4_get_मूल्य(ध्रुव vvp_vector4_t&word_val, अचिन्हित width,
			 तार्किक signed_flag, s_vpi_मूल्य*vp)
{
      वर्ण *rbuf = 0;

      विकल्प (vp->format) {
	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "sorry: Format %d not implemented for "
	                    "getting vector values.\n", (पूर्णाङ्क)vp->format);
	    प्रतिज्ञा(0);

	  पक्ष vpiSuppressVal:
	    भङ्ग;

	  पक्ष vpiBinStrVal:
	    rbuf = static_cast<वर्ण *>(need_result_buf(width+1, RBUF_VAL));
	    प्रति (अचिन्हित idx = 0 ;  idx < width ;  idx += 1) {
		  vvp_bit4_t bit = word_val.value(idx);
		  rbuf[width-idx-1] = vvp_bit4_to_ascii(bit);
	    }
	    rbuf[width] = 0;
	    vp->value.str = rbuf;
	    भङ्ग;

	  पक्ष vpiOctStrVal: {
		अचिन्हित hwid = ((width+2) / 3) + 1;
		rbuf = static_cast<वर्ण *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_oct_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		भङ्ग;
	  }

	  पक्ष vpiDecStrVal: {
// HERE need a better estimate.
		rbuf = static_cast<वर्ण *>(need_result_buf(width+1, RBUF_VAL));
		vpip_vec4_to_dec_str(word_val, rbuf, width+1, signed_flag);
		vp->value.str = rbuf;
		भङ्ग;
	  }

	  पक्ष vpiHexStrVal: {
		अचिन्हित  hwid = ((width + 3) / 4) + 1;
		rbuf = static_cast<वर्ण *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_hex_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		भङ्ग;
	  }

          पक्ष vpiScalarVal: {
	        // scalars should be of size 1
	        प्रतिज्ञा(width == 1);
	        विकल्प(word_val.value(0)) {
		    पक्ष BIT4_0:
		      vp->value.scalar = vpi0;
		      भङ्ग;
	            पक्ष BIT4_1:
                      vp->value.scalar = vpi1;
                      भङ्ग;
	            पक्ष BIT4_X:
                      vp->value.scalar = vpiX;
                      भङ्ग;
	            पक्ष BIT4_Z:
                      vp->value.scalar = vpiZ;
                      भङ्ग;
		}
                भङ्ग;
          }

	  पक्ष vpiIntVal: {
		दीर्घ val = 0;
		vvp_bit4_t pad = BIT4_0;
		यदि (signed_flag && word_val.size() > 0)
		      pad = word_val.value(word_val.size()-1);

		प्रति (अचिन्हित idx = 0 ; idx < 8*परिमाण(val) ;  idx += 1) {
		      vvp_bit4_t val4 = pad;
		      यदि (idx < word_val.size())
			    val4 = word_val.value(idx);
		      यदि (val4 == BIT4_1)
			    val |= 1L << idx;
		}

		vp->value.integer = val;
		भङ्ग;
	  }

	  पक्ष vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiVectorVal;
	    // fallthrough
	  पक्ष vpiVectorVal: {
		अचिन्हित hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * परिमाण(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		प्रति (अचिन्हित idx = 0 ;  idx < width ;  idx += 1) {
		      विकल्प (word_val.value(idx)) {
			  पक्ष BIT4_0:
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    भङ्ग;
			  पक्ष BIT4_1:
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    भङ्ग;
			  पक्ष BIT4_X:
			    op->aval |= (1 << idx % 32);
			    op->bval |= (1 << idx % 32);
			    भङ्ग;
			  पक्ष BIT4_Z:
			    op->aval &= ~(1 << idx % 32);
			    op->bval |=  (1 << idx % 32);
			    भङ्ग;
		      }
		      यदि (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		भङ्ग;
	  }

	  पक्ष vpiStringVal:
		vec4_get_मूल्य_string(word_val, width, vp);
		भङ्ग;

	  पक्ष vpiRealVal:
		vector4_to_मूल्य(word_val, vp->value.real, signed_flag);
		भङ्ग;
      }
}

शून्य vpip_vec2_get_मूल्य(ध्रुव vvp_vector2_t&word_val, अचिन्हित width,
			 तार्किक signed_flag, s_vpi_मूल्य*vp)
{
      विकल्प (vp->format) {
	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "sorry: Format %d not implemented for "
	                    "getting vector2 values.\n", (पूर्णाङ्क)vp->format);
	    प्रतिज्ञा(0);

	  पक्ष vpiSuppressVal:
	    भङ्ग;

	  पक्ष vpiIntVal:
	    vector2_to_मूल्य(word_val, vp->value.integer, signed_flag);
	    भङ्ग;

	  पक्ष vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  पक्ष vpiVectorVal: {
		अचिन्हित hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * परिमाण(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		प्रति (अचिन्हित idx = 0 ;  idx < width ;  idx += 1) {
		      यदि (word_val.value(idx)) {
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      } अन्यथा {
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      }
		      यदि (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		भङ्ग;
	  }
      }

}

/*
 * Convert a real value to the appropriate integer.
 */
स्थिर PLI_INT32 get_real_as_int(द्विगुण real)
{
      द्विगुण rtn;

	/* We would normally want to return 'bx for a NaN or
	 * +/- infinity, but for an integer the standard says
	 * to convert 'bx to 0, so we just return 0. */
      यदि (real != real || (real && (real == 0.5*real))) {
	    प्रत्यागम 0;
      }

      rtn = std::round(real);

      प्रत्यागम (PLI_INT32) rtn;
}

/*
 * This is a generic function to convert a double value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer.
 */
शून्य vpip_real_get_मूल्य(द्विगुण real, s_vpi_मूल्य*vp)
{
      वर्ण *rbuf = 0;

      विकल्प (vp->format) {
	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "sorry: Format %d not implemented for "
	                    "getting real values.\n", (पूर्णाङ्क)vp->format);
	    प्रतिज्ञा(0);

	  पक्ष vpiSuppressVal:
	    भङ्ग;

	  पक्ष vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiRealVal;
	    // fallthrough
	  पक्ष vpiRealVal:
	    vp->value.real = real;
	    भङ्ग;

	  पक्ष vpiIntVal:
	    vp->value.integer = get_real_as_int(real);
	    भङ्ग;

	  पक्ष vpiDecStrVal:
	    rbuf = static_cast<वर्ण *>(need_result_buf(1025, RBUF_VAL));
	    vpip_vec4_to_dec_str(vvp_vector4_t(1024, real), rbuf, 1025, सत्य);
	    vp->value.str = rbuf;
	    भङ्ग;
      }
}

द्विगुण real_from_vpi_मूल्य(s_vpi_मूल्य*vp)
{
      vvp_vector4_t vec4(1024);
      द्विगुण result;
      तार्किक is_signed = असत्य;

      विकल्प (vp->format) {
	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "sorry: Format %d not implemented for "
	                    "putting real values.\n", (पूर्णाङ्क)vp->format);
	    प्रतिज्ञा(0);

	  पक्ष vpiRealVal:
	    result = vp->value.real;
	    भङ्ग;

	  पक्ष vpiIntVal:
	    result = (द्विगुण) vp->value.integer;
	    भङ्ग;

	  पक्ष vpiBinStrVal:
	    vpip_bin_str_to_vec4(vec4, vp->value.str);
	    यदि (vp->value.str[0] == '-') is_signed = सत्य;
	    vector4_to_मूल्य(vec4, result, is_signed);
	    भङ्ग;

	  पक्ष vpiOctStrVal:
	    vpip_oct_str_to_vec4(vec4, vp->value.str);
	    यदि (vp->value.str[0] == '-') is_signed = सत्य;
	    vector4_to_मूल्य(vec4, result, is_signed);
	    भङ्ग;

	  पक्ष vpiDecStrVal:
	    vpip_dec_str_to_vec4(vec4, vp->value.str);
	    यदि (vp->value.str[0] == '-') is_signed = सत्य;
	    vector4_to_मूल्य(vec4, result, is_signed);
	    भङ्ग;

	  पक्ष vpiHexStrVal:
	    vpip_hex_str_to_vec4(vec4, vp->value.str);
	    यदि (vp->value.str[0] == '-') is_signed = सत्य;
	    vector4_to_मूल्य(vec4, result, is_signed);
	    भङ्ग;

      }

      प्रत्यागम result;
}

शून्य vpip_string_get_मूल्य(ध्रुव string&val, s_vpi_मूल्य*vp)
{
      वर्ण *rbuf = 0;

      विकल्प (vp->format) {
	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "sorry: Format %d not implemented for "
	                    "getting string values.\n", (पूर्णाङ्क)vp->format);
	    प्रतिज्ञा(0);

	  पक्ष vpiSuppressVal:
	    भङ्ग;

	  पक्ष vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiStringVal;
	    // fallthrough
	  पक्ष vpiStringVal:
	    rbuf = static_cast<वर्ण *>(need_result_buf(val.size() + 1, RBUF_VAL));
	    म_नकल(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    भङ्ग;
      }
}


शून्य vpi_get_मूल्य(vpiHandle व्यंजक, s_vpi_मूल्य*vp)
{
      प्रतिज्ञा(व्यंजक);
      प्रतिज्ञा(vp);

	// Never bother with suppressed values. All the derived
	// classes can ignore this type.
      यदि (vp->format == vpiSuppressVal)
	    प्रत्यागम;

      व्यंजक->vpi_get_मूल्य(vp);

      यदि (vpi_trace) विकल्प (vp->format) {
		पक्ष vpiStringVal:
		  ख_लिखो(vpi_trace,"vpi_get_value(%p=<%d>) -> string=\"%s\"\n",
			  व्यंजक, व्यंजक->get_type_code(), vp->value.str);
		  भङ्ग;

		पक्ष vpiBinStrVal:
		  ख_लिखो(vpi_trace, "vpi_get_value(<%d>...) -> binstr=%s\n",
			  व्यंजक->get_type_code(), vp->value.str);
		  भङ्ग;

		पक्ष vpiIntVal:
		  ख_लिखो(vpi_trace, "vpi_get_value(<%d>...) -> int=%d\n",
			  व्यंजक->get_type_code(), (पूर्णाङ्क)vp->value.integer);
		  भङ्ग;

		पक्ष vpiSuppressVal:
		  ख_लिखो(vpi_trace, "vpi_get_value(<%d>...) -> <suppress>\n",
			  व्यंजक->get_type_code());
		  भङ्ग;

		स्वभाव:
		  ख_लिखो(vpi_trace, "vpi_get_value(<%d>...) -> <%d>=?\n",
			  व्यंजक->get_type_code(), (पूर्णाङ्क)vp->format);
	    }
}

संरचना vpip_put_मूल्य_event : vvp_gen_event_s {
      vpiHandle handle;
      s_vpi_मूल्य value;
      पूर्णाङ्क flags;
      आभास शून्य run_run() override;
      ~vpip_put_मूल्य_event() override { }
};

शून्य vpip_put_मूल्य_event::run_run()
{
      handle->vpi_put_मूल्य(&value, flags);
      विकल्प (value.format) {
	    /* Free the copied string. */
	  पक्ष vpiBinStrVal:
	  पक्ष vpiOctStrVal:
	  पक्ष vpiDecStrVal:
	  पक्ष vpiHexStrVal:
	  पक्ष vpiStringVal:
	    मुक्त(value.value.str);
	    भङ्ग;
	    /* Free the copied time structure. */
	  पक्ष vpiTimeVal:
	    मुक्त(value.value.समय);
	    भङ्ग;
	    /* Free the copied vector structure. */
	  पक्ष vpiVectorVal:
	    मुक्त(value.value.vector);
	    भङ्ग;
	    /* Free the copied strength structure. */
	  पक्ष vpiStrengthVal:
	    मुक्त(value.value.strength);
	    भङ्ग;
	    /* Everything else is static in the structure. */
	  स्वभाव:
	    भङ्ग;
      }
}

/* Make a copy of a pointer to a time structure. */
स्थिर t_vpi_time *timedup(ध्रुव t_vpi_time *val)
{
      t_vpi_time *rtn;
      rtn = static_cast<t_vpi_time *> (दो_स्मृति(परिमाण(t_vpi_time)));
      *rtn = *val;
      प्रत्यागम rtn;
}

/* Make a copy of a pointer to a vector value structure. */
स्थिर t_vpi_vecval *vectordup(ध्रुव t_vpi_vecval *val, PLI_INT32 size)
{
      अचिन्हित अंक_bytes;
      t_vpi_vecval *rtn;
      प्रतिज्ञा(size > 0);
      अंक_bytes = ((size + 31)/32)*परिमाण(t_vpi_vecval);
      rtn = static_cast<t_vpi_vecval *> (दो_स्मृति(अंक_bytes));
      स_नकल(rtn, val, अंक_bytes);
      प्रत्यागम rtn;
}

/* Make a copy of a pointer to a strength structure. */
स्थिर t_vpi_strengthval *strengthdup(ध्रुव t_vpi_strengthval *val)
{
      t_vpi_strengthval *rtn;
      rtn = static_cast<t_vpi_strengthval *>
            (दो_स्मृति(परिमाण(t_vpi_strengthval)));
      *rtn = *val;
      प्रत्यागम rtn;
}

vpiHandle vpi_put_मूल्य(vpiHandle obj, s_vpi_मूल्य*vp,
			s_vpi_time*when, PLI_INT32 flags)
{
      प्रतिज्ञा(obj);

      flags &= ~vpiReturnEvent;

      यदि (flags!=vpiNoDelay && flags!=vpiForceFlag && flags!=vpiReleaseFlag) {
	    vvp_time64_t dly;
	    पूर्णाङ्क scale;

	    यदि (vpi_get(vpiAutomatic, obj)) {
		  ख_लिखो(मानक_त्रुटि, "VPI error: cannot put a value with "
				  "a delay on automatically allocated "
				  "variable '%s'.\n",
				  vpi_get_str(vpiName, obj));
		  प्रत्यागम 0;
	    }

	    प्रतिज्ञा(when != 0);

	    विकल्प (when->type) {
		पक्ष vpiScaledRealTime:
		  scale = vpip_time_units_from_handle(obj) -
		          vpip_get_time_precision();
		  यदि (scale >= 0) {
			dly = (vvp_time64_t)(when->real * घात(10.0, scale));
		  } अन्यथा {
			dly = (vvp_time64_t)(when->real / घात(10.0, -scale));
		  }
		  भङ्ग;
		पक्ष vpiSimTime:
		  dly = vpip_timestruct_to_time(when);
		  भङ्ग;
		स्वभाव:
		  dly = 0;
		  भङ्ग;
	    }

	    यदि ((dly == 0) && schedule_at_rosync()) {
		  ख_लिखो(मानक_त्रुटि, "VPI error: attempted to put a value to "
				  "variable '%s' during a read-only synch "
				  "callback.\n", vpi_get_str(vpiName, obj));
		  प्रत्यागम 0;
	    }

	    vpip_put_मूल्य_event*put = नूतन vpip_put_मूल्य_event;
	    put->handle = obj;
	    यदि (dynamic_cast<__vpiNamedEvent*>(obj)) {
		  put->value.format = vpiIntVal;
		  put->value.value.integer = 0;
	    } अन्यथा {
		  प्रतिज्ञा(vp);
		  put->value = *vp;
	    }
	      /* Since this is a scheduled put event we must copy any pointer
	       * data to keep it available until the event is actually run. */
	    विकल्प (put->value.format) {
		  /* Copy the string items. */
		पक्ष vpiBinStrVal:
		पक्ष vpiOctStrVal:
		पक्ष vpiDecStrVal:
		पक्ष vpiHexStrVal:
		पक्ष vpiStringVal:
		  put->value.value.str = strdup(put->value.value.str);
		  भङ्ग;
		  /* Copy a time pointer item. */
		पक्ष vpiTimeVal:
		  put->value.value.समय = timedup(put->value.value.समय);
		  भङ्ग;
		  /* Copy a vector pointer item. */
		पक्ष vpiVectorVal:
		  put->value.value.vector = vectordup(put->value.value.vector,
		                                      vpi_get(vpiSize, obj));
		  भङ्ग;
		  /* Copy a strength pointer item. */
		पक्ष vpiStrengthVal:
		  put->value.value.strength =
		      strengthdup(put->value.value.strength);
		  भङ्ग;
		  /* Everything thing else is already in the structure. */
		स्वभाव:
		  भङ्ग;
	    }
	    put->flags = flags;
	    schedule_generic(put, dly, असत्य, सत्य, सत्य);
	    प्रत्यागम 0;
      }

      यदि (schedule_at_rosync()) {
            ख_लिखो(मानक_त्रुटि, "VPI error: attempted to put a value to "
			    "variable '%s' during a read-only synch "
			    "callback.\n", vpi_get_str(vpiName, obj));
            प्रत्यागम 0;
      }

      obj->vpi_put_मूल्य(vp, flags);

      प्रत्यागम 0;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle res = 0;

      यदि (ref == 0) {
	      // A few types can apply to a nil handle. These are ways
	      // that the current function can get started finding things.
	    विकल्प (type) {

		पक्ष vpiScope:
		    // The IEEE1364-2005 doesn't seem to allow this,
		    // but some users seem to think it's handy, so
		    // return the scope that contains this SysTfCall.
		  प्रतिज्ञा(vpip_cur_task);
		  res = vpip_cur_task->vpi_handle(vpiScope);
		  भङ्ग;

		पक्ष vpiSysTfCall:
		    // This is how VPI users get a first handle into
		    // the system. This is the handle of the system
		    // task/function call currently being executed.
		  यदि (vpi_trace) {
			ख_लिखो(vpi_trace, "vpi_handle(vpiSysTfCall, 0) "
				"-> %p (%s)\n", vpip_cur_task,
				vpip_cur_task->defn->info.tfname);
		  }
		  प्रत्यागम vpip_cur_task;

		स्वभाव:
		  ख_लिखो(मानक_त्रुटि, "VPI error: vpi_handle(type=%d, ref=0).\n",
			  (पूर्णाङ्क)type);
		  res = 0;
		  भङ्ग;
	    }

      } अन्यथा {

	    यदि (type == vpiSysTfCall) {
		  ख_लिखो(मानक_त्रुटि, "VPI error: vpi_handle(vpiSysTfCall, "
			  "ref!=0).\n");
		  प्रत्यागम 0;
	    }

	    res = ref->vpi_handle(type);
      }


      यदि (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_handle(vpiScope, ref=%p) "
		    "-> %p\n", vpip_cur_task, ref);
      }

      प्रत्यागम res;
}

स्थिर vpiHandle vpip_make_udp_iterator()
{
// HERE: Add support for iterating over UDP definitions.
//       See 26.6.16 (page 400 in 1364-2005).
      प्रत्यागम 0;
}

/*
 * This function asks the object to return an iterator for
 * the specified reference. It is up to the iterate_ method to
 * allocate a properly formed iterator.
 */
स्थिर vpiHandle vpi_iterate_global(पूर्णाङ्क type)
{
      विकल्प (type) {
	  पक्ष vpiInstance:
	    // fallthrough
	  पक्ष vpiModule:
	    // fallthrough
	  पक्ष vpiProgram:
	    // fallthrough
	  पक्ष vpiInterface:
	    // fallthrough
	  पक्ष vpiPackage:
	    प्रत्यागम vpip_make_मूल_iterator(type);

	  पक्ष vpiUdpDefn:
	    प्रत्यागम vpip_make_udp_iterator();

	  पक्ष vpiUserSystf:
	    प्रत्यागम vpip_make_systf_iterator();
      }

      प्रत्यागम 0;
}

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle rtn = 0;

      प्रतिज्ञा(vpi_mode_flag != VPI_MODE_NONE);
      यदि (vpi_mode_flag == VPI_MODE_REGISTER) {
	    ख_लिखो(मानक_त्रुटि, "vpi error: vpi_iterate called during "
		    "vpi_register_systf. You can't do that!\n");
	    प्रत्यागम 0;
      }

      यदि (ref == 0)
	    rtn = vpi_iterate_global(type);
      अन्यथा
	    rtn = ref->vpi_iterate(type);

      यदि (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_iterate(%d, %p) ->%s\n",
	    (पूर्णाङ्क)type, ref, rtn ? "" : " (null)");
      }

      प्रत्यागम rtn;
}

vpiHandle vpi_handle_by_index(vpiHandle ref, PLI_INT32 idx)
{
      प्रतिज्ञा(ref);
      प्रत्यागम ref->vpi_index(idx);
}

स्थिर vpiHandle find_name(ध्रुव वर्ण *name, vpiHandle handle)
{
      vpiHandle rtn = 0;
      __vpiScope*ref = dynamic_cast<__vpiScope*>(handle);

      /* check module names */
      यदि (!म_भेद(name, vpi_get_str(vpiName, handle)))
	    rtn = handle;

      /* brute force search for the name in all objects in this scope */
      प्रति (अचिन्हित i = 0 ;  i < ref->intern.size() ;  i += 1) {
	      /* The standard says that since a port does not have a full
	       * name it cannot be found by name. Because of this we need
	       * to skip ports here so the correct handle can be located. */
	    यदि (vpi_get(vpiType, ref->intern[i]) == vpiPort) अनुवर्त;
	    ध्रुव वर्ण *nm = vpi_get_str(vpiName, ref->intern[i]);
	    यदि (nm && !म_भेद(name, nm)) {
		  rtn = ref->intern[i];
		  भङ्ग;
	    } अन्यथा यदि (vpi_get(vpiType, ref->intern[i]) == vpiMemory ||
	               vpi_get(vpiType, ref->intern[i]) == vpiNetArray) {
		  /* We need to iterate on the words */
		  vpiHandle word_i, word_h;
		  word_i = vpi_iterate(vpiMemoryWord, ref->intern[i]);
		  यावत् (word_i && (word_h = vpi_scan(word_i))) {
			nm = vpi_get_str(vpiName, word_h);
			यदि (nm && !म_भेद(name, nm)) {
			      rtn = word_h;
			      vpi_free_object(word_i);
			      भङ्ग;
			}
		  }
	    }
	    /* found it yet? */
	    यदि (rtn) भङ्ग;
      }

      प्रत्यागम rtn;
}

// Find the end of the escaped identifier or simple identifier
स्थिर वर्ण * find_rest(वर्ण *name)
{
      वर्ण *rest;
      यदि (*name == '\\') {
	    rest = म_अक्षर(name, ' ');
	    यदि (rest) {
		  *rest++ = 0; // The space is not part of the string
		    // There should be a '.' after the escaped ID if there is
		    // anything more. If it is missing add it to avoid a crash
		  यदि ((*rest != '.') && (*rest != 0)) {
			*--rest = '.';
			ख_लिखो(मानक_त्रुटि, "ERROR: Malformed scope string: \"%s\"", name);
		  }
		  यदि (*rest == 0) {
			rest = 0;
		  }
	    }
      } अन्यथा {
	    rest = म_अक्षर(name, '.');
      }
      प्रत्यागम rest;
}

स्थिर vpiHandle find_scope(ध्रुव वर्ण *name, vpiHandle handle, पूर्णाङ्क depth)
{

      vpiHandle iter = handle==0
	    ? vpi_iterate(vpiModule, रिक्त)
	    : vpi_iterate(vpiInternalScope, handle);

      vector<वर्ण> name_buf (म_माप(name)+1);
      म_नकल(&name_buf[0], name);
      वर्ण*nm_first = &name_buf[0];
      वर्ण*nm_rest = find_rest(nm_first);
      यदि (*nm_first == '\\') ++nm_first;
      यदि (nm_rest) {
	    *nm_rest++ = 0;
      }

      vpiHandle rtn = 0;
      vpiHandle hand;
      यावत् (iter && (hand = vpi_scan(iter))) {
	    ध्रुव वर्ण *nm = vpi_get_str(vpiName, hand);

	    यदि (म_भेद(nm_first,nm)==0) {
		  यदि (nm_rest)
			rtn=find_scope(nm_rest, hand, depth+1);
		  अन्यथा
			rtn = hand;
	    }

	    /* found it yet ? */
	    यदि (rtn) {
		  vpi_free_object(iter);
		  भङ्ग;
	    }
      }

      प्रत्यागम rtn;
}

// Find the end of the first escaped identifier or simple identifier
स्थिर वर्ण * find_next(वर्ण *name)
{
      वर्ण *next;
      यदि (*name == '\\') {
	    next = म_अक्षर(name, ' ');
	    यदि (next && *++next == 0) next = 0;
      } अन्यथा {
	    next = म_अक्षर(name, '.');
      }
      प्रत्यागम next;
}

vpiHandle vpi_handle_by_name(ध्रुव वर्ण *name, vpiHandle scope)
{
      vpiHandle hand;

      यदि (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_handle_by_name(%s, %p) -->\n",
		    name, scope);
      }

	// Chop the name into path and base. For example, if the name
	// is "a.b.c", then nm_path becomes "a.b" and nm_base becomes
	// "c". If the name is "c" then nm_path is nil and nm_base is "c".
      vector<वर्ण> name_buf (म_माप(name)+1);
      म_नकल(&name_buf[0], name);
      वर्ण*nm_path = &name_buf[0];
      वर्ण*nm_base;

	// Check to see if we have an escaped identifier and if so search
	// the long way because a '.' could be in the escaped name.
      यदि (म_अक्षर(nm_path, '\\')) {
	    वर्ण *next;
	    nm_base = nm_path;
	    यावत् ((next = find_next(nm_base))) {
		  nm_base = ++next;
	    }
	    यदि (nm_path == nm_base) {
		  nm_path = 0;
	    } अन्यथा {
		  *(nm_base-1) = 0;
	    }

	// If there is no escaped identifier then just look for the last '.'
      } अन्यथा {
	    nm_base = म_खोजप(nm_path, '.');
	    यदि (nm_base) {
		  *nm_base++ = 0;
	    } अन्यथा {
		  nm_base = nm_path;
		  nm_path = 0;
	    }
      }

      /* If scope provided, look in corresponding module; otherwise
       * traverse the hierarchy specified in name to find the leaf module
       * and try finding it there.
       */
      यदि (scope) {
	    /* Some implementations support either a module or a scope. */
	    विकल्प (vpi_get(vpiType, scope)) {
		पक्ष vpiScope:
	          hand = vpi_handle(vpiModule, scope);
	          भङ्ग;
		पक्ष vpiModule:
	          hand = scope;
	          भङ्ग;
		स्वभाव:
		  यदि (vpi_trace) {
			ख_लिखो(vpi_trace, "vpi_handle_by_name: "
				"Scope is not a vpiScope or vpiModule\n");
		  }
	          // Use vpi_chk_error() here when it is implemented.
	          प्रत्यागम 0;
	    }

      } अन्यथा यदि (nm_path) {
	      // The name has a path, and no other scope handle was
	      // passed in. That suggests we are looking for "a.b.c"
	      // in the root scope. So convert "a.b" to a scope and
	      // start there to look for "c".
	    hand = find_scope(nm_path, रिक्त, 0);
	    nm_path = 0;

      } अन्यथा {
	      // Special case: scope==<nil>, meaning we are looking in
	      // the root, and there is no path to the name, i.e. the
	      // string is "c" instead of "top.c". Try to find "c" as
	      // a scope and return that.
	    hand = find_scope(nm_base, रिक्त, 0);
      }

      यदि (hand == 0) {
	    यदि (vpi_trace) {
		  ख_लिखो(vpi_trace, "vpi_handle_by_name: "
			  "Scope does not exist. Giving up.\n");
	    }

	    प्रत्यागम 0;
      }

	// If there is a path part, then use it to find the
	// scope. For example, if the full name is a.b.c, then
	// the nm_path string is a.b and we search for that
	// scope. If we find it, then set hand to that scope.
      यदि (nm_path) {
	    vpiHandle tmp = find_scope(nm_path, hand, 0);
	    यावत् (tmp == 0 && hand != 0) {
		  hand = vpi_handle(vpiScope, hand);
		  tmp = find_scope(nm_path, hand, 0);
	    }
	    hand = tmp;
      }

	// find_name() expects escaped identifiers to be stripped
      यदि (*nm_base == '\\') {
	    // Skip the \ at the beginning
	    ++nm_base;

	    // Drop the space at the end if it exists
	    वर्ण *next;
	    यदि ((next = म_अक्षर(nm_base, ' '))) {
		  *next = 0;
	    }
      }

	// Now we have the correct scope, look for the item.
      vpiHandle out = find_name(nm_base, hand);

      यदि (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_handle_by_name: DONE\n");
      }

      प्रत्यागम out;
}

// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected
// x2: Ptr to previous node
// x3: True when net is connected to output of previous node
tuple<तार्किक, vvp_net_ptr_t, तार्किक> check_connected_to_concat8_and_part_sa(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent) {

      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      यावत् (cur.ptr()) {
	    यदि (cur.ptr() == net2 && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
		    // Pass on the return value if found
		  प्रत्यागम {सत्य, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
	    } अन्यथा यदि (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  स्वतः res = check_connected_to_concat8_and_part_sa(cur.ptr()->out_, net2, cur);
		  यदि (get<0>(res)) {
			प्रत्यागम res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      प्रत्यागम {असत्य, vvp_net_ptr_t(nullptr, 0), असत्य};
}

// Check if net2 is connected to current_net through a net of vvp_fun_concat8s
// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected with concats and part_sas
// x2: Ptr to previous node
// x3: True when net is connected to output of x2
tuple<तार्किक, vvp_net_ptr_t, तार्किक> check_connected_to_concat8(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent)
{
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);
	// For everything connected
      यावत् (cur.ptr()) {
	      // Check if it's a concat8
	    यदि (cur.ptr() == net2) {
		  प्रत्यागम {सत्य, vvp_net_ptr_t(nullptr, 0), असत्य};
	    } अन्यथा यदि (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  स्वतः res = check_connected_to_concat8(cur.ptr()->out_, net2, cur);
		  यदि (get<0>(res)) {
			यदि (!get<1>(res).ptr() && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
			      // `parent` is the last part_sa node in front of our destination
			      // We want to place the intermod node here
			      प्रत्यागम {सत्य, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
			}
			प्रत्यागम res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      प्रत्यागम {असत्य, vvp_net_ptr_t(nullptr, 0), असत्य};
}

// Used to get intermodpath for two ports
vpiHandle vpi_handle_multi(PLI_INT32 type,
                           vpiHandle ref1,
                           vpiHandle ref2)
{
      यदि (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_handle_multi(%d, %p, %p) -->\n",
	            type, ref1, ref2);
      }

      यदि (type != vpiInterModPath) {
	    ख_लिखो(मानक_त्रुटि, "sorry: vpi_handle_multi currently supports"
	            "only vpiInterModPath\n");
	    प्रत्यागम nullptr;
      }

	// Indicates whether port1 refers to a single bit
      तार्किक port1_has_index = असत्य;
      पूर्णाङ्क port1_bit_index = 0;
      vpiPortBitInfo* port1_bit = dynamic_cast<vpiPortBitInfo*>(ref1);

      यदि (port1_bit) {
	      // Get the bit index
	    port1_has_index = सत्य;
	    port1_bit_index = vpi_get(vpiBit, port1_bit);
	      // Update the ref1 to point to the base port
	    ref1 = vpi_handle(vpiParent, port1_bit);
      }

	// Indicates whether port2 refers to a single bit
      तार्किक port2_has_index = असत्य;
      पूर्णाङ्क port2_bit_index = 0;
      vpiPortBitInfo* port2_bit = dynamic_cast<vpiPortBitInfo*>(ref2);

      यदि (port2_bit) {
	      // Get the bit index
	    port2_has_index = सत्य;
	    port2_bit_index = vpi_get(vpiBit, port2_bit);
	      // Update the ref1 to point to the base port
	    ref2 = vpi_handle(vpiParent, port2_bit);
      }

      vpiPortInfo* port1 = dynamic_cast<vpiPortInfo*>(ref1);

      यदि (!port1) {
	    ख_लिखो(मानक_त्रुटि, "sorry: second argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    प्रत्यागम nullptr;
      }

      vpiPortInfo* port2 = dynamic_cast<vpiPortInfo*>(ref2);

      यदि (!port2) {
	    ख_लिखो(मानक_त्रुटि, "sorry: third argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    प्रत्यागम nullptr;
      }

	// Get the names of both ports
      std::string port1_name(vpi_get_str(vpiName, ref1));
      std::string port2_name(vpi_get_str(vpiName, ref2));

	// If both ports are vpiOutput, we have to reassign the __vpiSignal from port1
	// to port2 because otherwise the non-delayed version of the signal is dumped
	// even tho the intermodpath is correctly inserted
      __vpiSignal* output_signal = nullptr;

      यदि (port1->get_direction() == vpiOutput && port2->get_direction() == vpiOutput) {
	    vpiHandle scope_port2 = vpi_handle(vpiScope, ref2);
	    प्रतिज्ञा(scope_port2);

	      // Iterate over nets in the scope of port2
	    vpiHandle net_i = vpi_iterate(vpiNet, scope_port2) ;
	    vpiHandle net;

	    यावत् ((net = vpi_scan(net_i)) != रिक्त) {
		  std::string net_name(vpi_get_str(vpiName, net));

		    // Compare whether the net matches with the port name
		  यदि (net_name == port2_name) {
			output_signal = dynamic_cast<__vpiSignal*>(net);
		  }
	    }
      }

      vvp_net_t* net1 = port1->get_port();
      vvp_net_t* net2 = port2->get_port();

      यदि (net1 == nullptr || net2 == nullptr) {
	    ख_लिखो(मानक_त्रुटि, "Error: Could not find net. "
		    "Did you run iverilog with '-ginterconnect'?\n");
	    प्रत्यागम nullptr;
      }

      यदि (net1 == net2) {
	    ख_लिखो(मानक_त्रुटि, "Error: Net for both ports is the same. "
		    "Did you pass the same port twice?\n");
	    प्रत्यागम nullptr;
      }

      यदि (!dynamic_cast<vvp_fun_buft*>(net1->fun)) {
	    ख_लिखो(मानक_त्रुटि, "Error: functor of net1 must be"
		    "vvp_fun_buft\n");
	    प्रत्यागम nullptr;
      }

      यदि (!dynamic_cast<vvp_fun_buft*>(net2->fun)) {
	    ख_लिखो(मानक_त्रुटि, "Error: functor of net2 must be"
		    "vvp_fun_buft\n");
	    प्रत्यागम nullptr;
      }

	// If port1 is actually a port bit, we have to get to the correct vvp_fun_part
	// after which we insert the intermodpath delay
      यदि (port1_has_index) {
	    vvp_net_ptr_t* net1_ptr = &net1->out_;

	      // Search for part selects connected to port1
	    vvp_net_t* current_net = net1_ptr->ptr();

	    यावत् (current_net) {
		  ध्रुव vvp_fun_part* part = dynamic_cast<vvp_fun_part*>(current_net->fun);

		    // Its a part select!
		  यदि (part) {
			  // Is it the correct part select?
			यदि (part->get_base() == (अचिन्हित)port1_bit_index) {
			      प्रतिज्ञा(part->get_wid() == 1);
			      net1 = current_net; // Replace net1 as this is our new start point
			      भङ्ग;
			}
		  }

		  current_net = current_net->port[0].ptr(); // BUFT has only one input, index 0
	    }
      }

	// Iterate over all nodes connected to port1
      vvp_net_ptr_t cur = net1->out_;
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      यावत् (cur.ptr()) {
	      // Either port2 is directly connected to port1
	      // Or in the second case port2 is indirectly connected
	      // to port1 through a net of concat8s
	    तार्किक is_connected, is_output;
	    vvp_net_ptr_t previous_गाँठ;
	    tie(is_connected, previous_गाँठ, is_output) = port2_has_index
		  ? check_connected_to_concat8(cur, net2, vvp_net_ptr_t(net1, 0))
		  : check_connected_to_concat8_and_part_sa(cur, net2, vvp_net_ptr_t(net1, 0));
	    यदि ( (!port2_has_index && cur.ptr() == net2) || is_connected ) {
		  vvp_net_t*new_net = नूतन vvp_net_t;

		    // Create new node with intermodpath and connect port2 to it
		  पूर्णाङ्क width = 1; // TODO
		  vvp_fun_intermodpath*obj = नूतन vvp_fun_intermodpath(new_net, width);

		  vvp_net_t* output_net = net1;
		  यदि (is_connected) {
			यदि (is_output) {
			      output_net = previous_गाँठ.ptr(); // net2 is direct output of part_sa
			      cur = output_net->out_;
			      prev = vvp_net_ptr_t(nullptr, 0);
			} अन्यथा {
			      prev = previous_गाँठ;
			      यदि (prev.ptr()) {
				    cur = prev.ptr()->port[prev.port()];
			      }
			}
		  }

		  new_net->fun = obj;
		  new_net->out_ = cur;

		    // Port2 is in the middle of the list
		    // Insert intermodpath before port2 and keep everything else intact
		  यदि (prev.ptr()) {
			prev.ptr()->port[prev.port()] = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; // Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		    // Port2 is first in list
		    // Insert intermodpath before port2 and keep everything else intact
		  } अन्यथा {
			output_net->out_ = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; //  Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		  }

		    // If both ports are vpiOutput and port2 is not a vector,
		    // we have to reassign the __vpiSignal so that the delayed
		    // values get dumped
		  यदि (output_signal && !port2_has_index) {
			net2->fil = net1->fil;
			net1->fil = nullptr;
			output_signal->गाँठ = net2;
		  }

		    // Create the VPI intermodpath object
		  __vpiInterModPath* intermodpath = vpip_make_intermodpath(new_net, port1, port2);
		  intermodpath->intermodpath = obj;

		    // Finally done, return the intermodpath object
		  प्रत्यागम intermodpath;
	    }

	    prev = cur;
	    cur = cur.ptr()->port[cur.port()]; // Next net in linked list
      }

      ख_लिखो(मानक_त्रुटि, "VPI error: Could not insert intermodpath!\n");
      ख_लिखो(मानक_त्रुटि, "\tport1 = %s, port1_has_index = %d, port1_bit_index = %d\n", port1_name.c_str(), port1_has_index, port1_bit_index);
      ख_लिखो(मानक_त्रुटि, "\tport2 = %s, port2_has_index = %d, port2_bit_index = %d\n", port2_name.c_str(), port2_has_index, port2_bit_index);
      प्रत्यागम nullptr;
}

/*
  We increment the two vpi methods to enable the
  read/write of SDF delay values from/into
  the modpath vpiHandle


  basically, they will redirect the generic vpi_interface

  vpi_get_delay ( .. )
  vpi_put_delay ( .. )


  to the

  modpath_get_delay ( .. ) ;
  modpath_put_delay ( .. ) ;

*/



शून्य vpi_get_delays(vpiHandle व्यंजक, p_vpi_delay delays)
{
      प्रतिज्ञा(व्यंजक);
      प्रतिज्ञा(delays);

      व्यंजक->vpi_get_delays(delays);

      यदि (vpi_trace) {
	    ख_लिखो(vpi_trace,
		    "vpi_get_delays(%p, %p) -->\n", व्यंजक, delays);
      }
}


शून्य vpi_put_delays(vpiHandle व्यंजक, p_vpi_delay delays)
{
      प्रतिज्ञा(व्यंजक  );
      प्रतिज्ञा(delays );

      व्यंजक->vpi_put_delays(delays);

      यदि (vpi_trace) {
	    ख_लिखो(vpi_trace,
		    "vpi_put_delays(%p, %p) -->\n", व्यंजक, delays);
      }
}








बाह्य "C" PLI_INT32 vpi_vprintf(ध्रुव वर्ण*fmt, बहु_सूची ap)
{
      प्रत्यागम vpi_mcd_vprintf(1, fmt, ap);
}

बाह्य "C" PLI_INT32 vpi_printf(ध्रुव वर्ण *fmt, ...)
{
      बहु_सूची ap;
      बहु_शुरू(ap, fmt);
      पूर्णाङ्क r = vpi_mcd_vprintf(1, fmt, ap);
      बहु_पूर्ण(ap);
      प्रत्यागम r;
}

बाह्य "C" PLI_INT32 vpi_flush(शून्य)
{
      प्रत्यागम vpi_mcd_flush(1);
}


बाह्य "C" PLI_INT32 vpi_sim_vcontrol(पूर्णाङ्क operation, बहु_सूची ap)
{
      दीर्घ diag_संदेश;

      विकल्प (operation) {
	  पक्ष vpiFinish:
	  पक्ष __ivl_legacy_vpiFinish:
            diag_संदेश = बहु_तर्क(ap, PLI_INT32);
	    schedule_finish(diag_संदेश);
	    भङ्ग;

	  पक्ष vpiStop:
	  पक्ष __ivl_legacy_vpiStop:
            diag_संदेश = बहु_तर्क(ap, PLI_INT32);
	    schedule_stop(diag_संदेश);
	    भङ्ग;

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "Unsupported VPI control operation %d.\n", operation);
	    प्रत्यागम 0;
      }
      प्रत्यागम 1;
}

बाह्य "C" PLI_INT32 vpi_sim_control(PLI_INT32 operation, ...)
{
      बहु_सूची ap;
      बहु_शुरू(ap, operation);
      पूर्णाङ्क r = vpi_sim_vcontrol(operation, ap);
      बहु_पूर्ण(ap);
      प्रत्यागम r;
}

बाह्य "C" PLI_INT32 vpi_control(PLI_INT32 operation, ...)
{
      बहु_सूची ap;
      बहु_शुरू(ap, operation);
      पूर्णाङ्क r = vpi_sim_vcontrol(operation, ap);
      बहु_पूर्ण(ap);
      प्रत्यागम r;
}

/*
 * This routine calculated the return value for $clog2.
 * It is easier to do it here vs trying to to use the VPI interface.
 */
बाह्य "C" s_vpi_vecval vpip_calc_clog2(vpiHandle arg)
{
      s_vpi_vecval rtn;
      s_vpi_मूल्य val;
      vvp_vector4_t vec4;
      तार्किक is_neg = असत्य;  // At this point only a real can be negative.

	/* Get the value as a vvp_vector4_t. */
      val.format = vpiObjTypeVal;
      vpi_get_मूल्य(arg, &val);
      यदि (val.format == vpiRealVal) {
	    vpi_get_मूल्य(arg, &val);
	      /* All double values can be represented in 1024 bits. */
	    vec4 = vvp_vector4_t(1024, val.value.real);
	    यदि (val.value.real < 0) is_neg = सत्य;
      } अन्यथा {
	    val.format = vpiVectorVal;
	    vpi_get_मूल्य(arg, &val);
	    अचिन्हित wid = vpi_get(vpiSize, arg);
	    vec4 = vvp_vector4_t(wid, BIT4_0);
	    प्रति (अचिन्हित idx=0; idx < wid; idx += 1) {
		PLI_INT32 aval = val.value.vector[idx/32].aval;
		PLI_INT32 bval = val.value.vector[idx/32].bval;
		aval >>= idx % 32;
		bval >>= idx % 32;
		पूर्णाङ्क bitmask = (aval&1) | ((bval<<1)&2);
		vvp_bit4_t bit = scalar_to_bit4(bitmask);
		vec4.set_bit(idx, bit);
	    }
      }

      यदि (vec4.has_xz()) {
	    rtn.aval = rtn.bval = 0xFFFFFFFFU;  /* Set to 'bx. */
	    प्रत्यागम rtn;
      }

      vvp_vector2_t vec2(vec4);

      यदि (is_neg) vec2.trim_neg(); /* This is a special trim! */
      अन्यथा vec2.trim(); /* This makes less work shifting. */

	/* Calculate the clog2 result. */
      PLI_INT32 res = 0;
      यदि (!vec2.is_zero()) {
	    vec2 -= vvp_vector2_t(1, vec2.size());
	    यावत्(!vec2.is_zero()) {
		  res += 1;
		  vec2 >>= 1;
	    }
      }

      rtn.aval = res;
      rtn.bval = 0;
      प्रत्यागम rtn;
}

/*
 * This routine provides the information needed to implement $countdrivers.
 * It is done here for performance reasons - interrogating the drivers
 * individually via the VPI interface would be much slower.
 */
बाह्य "C" शून्य vpip_गिनती_drivers(vpiHandle ref, अचिन्हित idx,
                                   अचिन्हित counts[4])
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);
      rfp->गाँठ->गिनती_drivers(idx, counts);
}

#यदि defined(__MINGW32__) || defined (__CYGWIN__)
vpip_routines_s vpi_routines = {
    .register_cb                = vpi_register_cb,
    .remove_cb                  = vpi_remove_cb,
    .register_systf             = vpi_register_systf,
    .get_systf_info             = vpi_get_systf_info,
    .handle_by_name             = vpi_handle_by_name,
    .handle_by_index            = vpi_handle_by_index,
    .handle_multi               = vpi_handle_multi,
    .handle                     = vpi_handle,
    .iterate                    = vpi_iterate,
    .scan                       = vpi_scan,
    .get                        = vpi_get,
    .get64                      = vpi_get64,
    .get_str                    = vpi_get_str,
    .get_delays                 = vpi_get_delays,
    .put_delays                 = vpi_put_delays,
    .get_मूल्य                  = vpi_get_मूल्य,
    .put_मूल्य                  = vpi_put_मूल्य,
    .get_time                   = vpi_get_time,
    .get_userdata               = vpi_get_userdata,
    .put_userdata               = vpi_put_userdata,
    .mcd_open                   = vpi_mcd_open,
    .mcd_close                  = vpi_mcd_close,
    .mcd_flush                  = vpi_mcd_flush,
    .mcd_name                   = vpi_mcd_name,
    .mcd_vprintf                = vpi_mcd_vprintf,
    .flush                      = vpi_flush,
    .भ_लिखो                    = vpi_vprintf,
    .chk_error                  = vpi_chk_error,
    .compare_objects            = vpi_compare_objects,
    .free_object                = vpi_free_object,
    .release_handle             = vpi_release_handle,
    .get_vlog_info              = vpi_get_vlog_info,
    .vcontrol                   = vpi_sim_vcontrol,
    .ख_खोलो                      = vpi_fopen,
    .get_file                   = vpi_get_file,
    .calc_clog2                 = vpip_calc_clog2,
    .गिनती_drivers              = vpip_गिनती_drivers,
    .format_strength            = vpip_format_strength,
    .format_pretty              = vpip_format_pretty,
    .make_systf_system_defined  = vpip_make_systf_system_defined,
    .mcd_rawwrite               = vpip_mcd_rawwrite,
    .set_return_मूल्य           = vpip_set_return_मूल्य,
};
#endif
