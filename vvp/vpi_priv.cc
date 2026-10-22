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

स्थिरांक अक्षर* direction_as_string(पूर्णांक dir)
{
      निवड (dir) {
	  स्थिती vpi_आदान:
	    परत "input";
	  स्थिती vpiOutput:
	    परत "output";
	  स्थिती vpiInout:
	    परत "inout";
	  स्थिती vpiMixedIO:
	    परत "mixed io";
	  स्थिती vpiNoDirection:
	    परत "no direction";
	  पूर्वनिर्धारित:
	    परत "INVALID-DIRECTION";
      }
}

__vpiHandle::~__vpiHandle()
{ }

पूर्णांक __vpiHandle::vpi_get(पूर्णांक)
{ परत vpiUndefined; }

int64_t __vpiHandle::vpi_get64(पूर्णांक code)
{ परत (code == _vpiNexusId) ? 0 : vpiUndefined; }

अक्षर* __vpiHandle::vpi_get_str(पूर्णांक)
{ परत 0; }

शून्य __vpiHandle::vpi_get_मूल्य(p_vpi_मूल्य)
{ }

vpiHandle __vpiHandle::vpi_put_मूल्य(p_vpi_मूल्य, पूर्णांक)
{ परत 0; }

vpiHandle __vpiHandle::vpi_handle(पूर्णांक)
{ परत 0; }

vpiHandle __vpiHandle::vpi_iterate(पूर्णांक)
{ परत 0; }

vpiHandle __vpiHandle::vpi_index(पूर्णांक)
{ परत 0; }

शून्य __vpiHandle::vpi_get_delays(p_vpi_delay)
{ }

शून्य __vpiHandle::vpi_put_delays(p_vpi_delay)
{ }

__vpiBaseVar::__vpiBaseVar(__vpiScope*scope, स्थिरांक अक्षर*name, vvp_net_t*net)
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
स्थिर पूर्णांक suppress_free(vpiHandle)
{ परत 1; }
__vpiHandle::free_object_fun_t __vpiHandle::free_object_fun(शून्य)
{ परत &suppress_free; }

/*
 * The vpip_string function creates a constant string from the pass
 * input. This constant string is permanently allocated from an
 * efficient string buffer store.
 */
संरचना vpip_string_chunk {
      संरचना vpip_string_chunk*next;
      अक्षर data[64*1024 - आकार (संरचना vpip_string_chunk*)];
};

अचिन्हित vpip_size(स्थिरांक __vpiSignal *sig)
{
      परत असल(sig->msb.get_मूल्य() - sig->lsb.get_मूल्य()) + 1;
}

__vpiScope* vpip_scope(__vpiSignal*sig)
{
      जर (sig->is_netarray)
	    परत static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      अन्यथा
	    परत sig->within.scope;
}

__vpiScope* vpip_scope(__vpiRealVar*sig)
{
      जर (sig->is_netarray)
	    परत static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      अन्यथा
	    परत sig->within.scope;
}

vpiHandle vpip_module(__vpiScope*scope)
{
      पर्यंत(scope && scope->get_type_code() != vpiModule) {
	    scope = scope->scope;
      }
      प्रतिज्ञा(scope);
      परत scope;
}

स्थिरांक अक्षर *vpip_string(स्थिरांक अक्षर*str)
{
      स्थिर vpip_string_chunk first_chunk = {0, {0}};
      स्थिर vpip_string_chunk*chunk_list = &first_chunk;
      स्थिर अचिन्हित chunk_fill = 0;

      अचिन्हित len = म_माप(str);
      प्रतिज्ञा( (len+1) <= आकार chunk_list->data );

      जर ( (len+1) > (आकार chunk_list->data - chunk_fill) ) {
	    vpip_string_chunk*tmp = नवीन vpip_string_chunk;
	    tmp->next = chunk_list;
	    chunk_list = tmp;
	    chunk_fill = 0;
      }

      अक्षर*res = chunk_list->data + chunk_fill;
      chunk_fill += len + 1;

      म_नकल(res, str);
      परत res;
}

स्थिर अचिन्हित hash_string(स्थिरांक अक्षर*text)
{
      अचिन्हित h = 0;

      पर्यंत (*text) {
	    h = (h << 4) ^ (h >> 28) ^ (अचिन्हित)*text;
	    text += 1;
      }
      परत h;
}

स्थिरांक अक्षर* vpip_name_string(स्थिरांक अक्षर*text)
{
      स्थिरांक अचिन्हित HASH_SIZE = 4096;
      स्थिर स्थिरांक अक्षर*hash_table[HASH_SIZE] = {0};

      अचिन्हित hash_मूल्य = hash_string(text) % HASH_SIZE;

	/* If we easily find the string in the hash table, then return
	   that and be done. */
      जर (hash_table[hash_मूल्य]
	  && (म_भेद(hash_table[hash_मूल्य], text) == 0)) {
	    परत hash_table[hash_मूल्य];
      }

	/* The existing hash entry is not a match. Replace it with the
	   newly allocated value, and return the new pointer as the
	   result to the add. */
      स्थिरांक अक्षर*res = vpip_string(text);
      hash_table[hash_मूल्य] = res;

      परत res;
}

PLI_INT32 vpi_chk_error(p_vpi_error_info info)
{
      जर (vpip_last_error.state == 0)
	    परत 0;

      info->state = vpip_last_error.state;
      info->level = vpip_last_error.level;
      info->message = vpip_last_error.message;
      info->product = vpi_vlog_info.product;
      info->code = (अक्षर *)"";
      info->file = 0;
      info->ओळ = 0;

      परत info->level;
}

PLI_INT32 vpi_compare_objects(vpiHandle obj1, vpiHandle obj2)
{
      प्रतिज्ञा(obj1);
      प्रतिज्ञा(obj2);

	// Does this actually work for all cases?
      जर (obj1 != obj2) परत 0;
      अन्यथा परत 1;
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
      जर (rfp == 0) {
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
      पूर्णांक rtn;

      जर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_free_object(%p)", ref);
	    ख_साफ(vpi_trace);
      }

      प्रतिज्ञा(ref);
      __vpiHandle::free_object_fun_t fun = ref->free_object_fun();
      rtn = fun (ref);

      जर (vpi_trace)
	    ख_लिखो(vpi_trace, " --> %d\n", rtn);

      परत rtn;
}

PLI_INT32 vpi_release_handle(vpiHandle ref)
{
	// Since SystemVerilog vpi_free_object() has been
	// renamed vpi_release_handle(), and thus
	// vpi_free_object() has been deprecated.
      परत vpi_free_object(ref);
}

स्थिर पूर्णांक vpip_get_global(पूर्णांक property)
{
      निवड (property) {

	  स्थिती vpiTimeUnit:
	  स्थिती vpiTimePrecision:
	    परत vpip_get_time_precision();

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vpi error: bad global property: %d\n", property);
	    प्रतिज्ञा(0);
	    परत vpiUndefined;
      }
}

स्थिर स्थिरांक अक्षर* vpi_property_str(PLI_INT32 code)
{
      स्थिर अक्षर buf[32];
      निवड (code) {
	  स्थिती vpiConstType:
	    परत "vpiConstType";
	  स्थिती vpiName:
	    परत "vpiName";
	  स्थिती vpiFullName:
	    परत "vpiFullName";
	  स्थिती vpiTimeUnit:
	    परत "vpiTimeUnit";
	  स्थिती vpiTimePrecision:
	    परत "vpiTimePrecision";
          स्थिती vpiSize:
	    परत "vpiSize";
          स्थिती _vpiNexusId:
	    परत "_vpiNexusId";
	  पूर्वनिर्धारित:
	    snprintf(buf, आकार(buf), "%d", (पूर्णांक)code);
      }
      परत buf;
}

स्थिरांक अक्षर* vpi_type_as_string(PLI_INT32 code)
{
      स्थिर अक्षर buf[32];
      निवड (code) {
	  स्थिती vpiArrayType:
	    परत "vpiArrayType";
	  स्थिती vpiBitVar:
	    परत "vpiBitVar";
	  स्थिती vpiByteVar:
	    परत "vpiByteVar";
	  स्थिती vpiClassVar:
	    परत "vpiClassVar";
	  स्थिती vpiConstant:
	    परत "vpiConstant";
	  स्थिती vpiEnumTypespec:
	    परत "vpiEnumTypespec";
	  स्थिती vpiFunction:
	    परत "vpiFunction";
	  स्थिती vpiGenScope:
	    परत "vpiGenScope";
	  स्थिती vpiIntVar:
	    परत "vpiIntVar";
	  स्थिती vpiIntegerVar:
	    परत "vpiIntegerVar";
	  स्थिती vpiIterator:
	    परत "vpiIterator";
	  स्थिती vpiLongIntVar:
	    परत "vpiLongIntVar";
	  स्थिती vpiMemory:
	    परत "vpiMemory";
	  स्थिती vpiMemoryWord:
	    परत "vpiMemoryWord";
	  स्थिती vpiModule:
	    परत "vpiModule";
	  स्थिती vpiNamedBegin:
	    परत "vpiNamedBegin";
	  स्थिती vpiNamedEvent:
	    परत "vpiNamedEvent";
	  स्थिती vpiNamedFork:
	    परत "vpiNamedFork";
	  स्थिती vpiPackage:
	    परत "vpiPackage";
	  स्थिती vpiPathTerm:
	    परत "vpiPathTerm";
	  स्थिती vpiPort:
	    परत "vpiPort";
	  स्थिती vpiNet:
	    परत "vpiNet";
	  स्थिती vpiNetArray:
	    परत "vpiNetArray";
	  स्थिती vpiNetBit:
	    परत "vpiNetBit";
	  स्थिती vpiParameter:
	    परत "vpiParameter";
	  स्थिती vpiPartSelect:
	    परत "vpiPartSelect";
	  स्थिती vpiRealVar:
	    परत "vpiRealVar";
	  स्थिती vpiReg:
	    परत "vpiReg";
	  स्थिती vpiRegBit:
	    परत "vpiRegBit";
	  स्थिती vpiShortIntVar:
	    परत "vpiShortIntVar";
	  स्थिती vpiStringVar:
	    परत "vpiStringVar";
	  स्थिती vpiSysFuncCall:
	    परत "vpiSysFuncCall";
	  स्थिती vpiSysTaskCall:
	    परत "vpiSysTaskCall";
	  स्थिती vpiTask:
	    परत "vpiTask";
	  स्थिती vpiTimeVar:
	    परत "vpiTimeVar";
	  स्थिती vpiUserSystf:
	    परत "vpiUserSystf";
	  पूर्वनिर्धारित:
	    snprintf(buf, आकार(buf), "%d", (पूर्णांक)code);
      }
      परत buf;
}

PLI_INT32 vpi_get(पूर्णांक property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      जर (property == _vpiDelaySelection) परत vpip_delay_selection;

      जर (ref == 0)
	    परत vpip_get_global(property);

      जर (property == vpiType) {
	    जर (vpi_trace) {
		  ख_लिखो(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

	    जर (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  परत vpiNetArray;
	    अन्यथा
		  परत ref->get_type_code();
      }

      पूर्णांक res = ref->vpi_get(property);

      जर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_get(%s, %p) --> %d\n",
		    vpi_property_str(property), ref, res);
      }

      परत res;
}

PLI_INT64 vpi_get64(पूर्णांक property, vpiHandle ref)
{
	// For now we only support getting the nexus id.
      जर ((ref == 0) || (property != _vpiNexusId)) {
	    ख_लिखो(vpi_trace, "vpi_get64(%s, %p) is not currently supported.\n",
		    vpi_property_str(property), ref);
	    परत vpiUndefined;
      }

      int64_t res = ref->vpi_get64(property);

      जर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_get64(%s, %p) --> %" PRId64 "\n",
		    vpi_property_str(property), ref, res);
      }

      परत res;
}

अक्षर* vpi_get_str(PLI_INT32 property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      जर (property == _vpiDelaySelection) {
	    निवड (vpip_delay_selection) {
		स्थिती _vpiDelaySelMinimum:
		  परत simple_set_rbuf_str("MINIMUM");
		स्थिती _vpiDelaySelTypical:
		  परत simple_set_rbuf_str("TYPICAL");
		स्थिती _vpiDelaySelMaximum:
		  परत simple_set_rbuf_str("MAXIMUM");
		पूर्वनिर्धारित:
		  प्रतिज्ञा(0);
	    }
      }

      जर (ref == 0) {
	    ख_लिखो(मानक_त्रुटि, "vpi error: vpi_get_str(%s, 0) called "
		    "with null vpiHandle.\n", vpi_property_str(property));
	    परत 0;
      }

      जर (property == vpiType) {
	    जर (vpi_trace) {
		  ख_लिखो(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

            PLI_INT32 type;
	    जर (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  type = vpiNetArray;
	    अन्यथा
		  type = ref->get_type_code();
	    परत (अक्षर *)vpi_type_as_string(type);
      }

      अक्षर*res = ref->vpi_get_str(property);

      जर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_get_str(%s, %p) --> %s\n",
		    vpi_property_str(property), ref, res? res : "<NULL>");
      }

      परत res;
}

__vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj)
{
      स्थिरांक संरचना __vpiSysTaskCall*task;
      संरचना __vpiSignal*संकेत;
      संरचना __vpiRealVar*real;
      स्थिरांक __vpiNamedEvent*event;

      निवड (obj->get_type_code()) {
	  स्थिती vpiSysTaskCall:
	    task = dynamic_cast<__vpiSysTaskCall*>(obj);
	    परत task->scope;

	  स्थिती vpiModule:
	    परत dynamic_cast<__vpiScope*>(obj);

	  स्थिती vpiNet:
	  स्थिती vpiReg:
	  स्थिती vpiIntegerVar:
	  स्थिती vpiBitVar:
	  स्थिती vpiByteVar:
	  स्थिती vpiShortIntVar:
	  स्थिती vpiIntVar:
	  स्थिती vpiLongIntVar:
	    संकेत = dynamic_cast<__vpiSignal*>(obj);
	    परत vpip_scope(संकेत);

	  स्थिती vpiRealVar:
	    real = dynamic_cast<__vpiRealVar*>(obj);
	    परत vpip_scope(real);

	  स्थिती vpiNamedEvent:
	    event = dynamic_cast<__vpiNamedEvent*>(obj);
	    परत event->get_scope();

	  स्थिती vpiMemory:
	  स्थिती vpiMemoryWord:
	  स्थिती vpiPartSelect:
	    परत dynamic_cast<__vpiScope*>(obj->vpi_handle(vpiScope));

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "ERROR: vpip_scope_from_handle called with "
		    "object handle type=%d\n", obj->get_type_code());
	    प्रतिज्ञा(0);
	    परत 0;
      }
}

पूर्णांक vpip_time_units_from_handle(vpiHandle obj)
{
      जर (obj == 0)
	    परत vpip_get_time_precision();

      स्थिरांक __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      जर (scope == 0)
	    परत vpip_get_time_precision();

      परत scope->time_units;
}

पूर्णांक vpip_time_precision_from_handle(vpiHandle obj)
{
      जर (obj == 0)
	    परत vpip_get_time_precision();

      स्थिरांक __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      जर (scope == 0)
	    परत vpip_get_time_precision();

      परत scope->time_precision;
}

द्विगुण vpip_scaled_time_from_handle(vvp_time64_t समय, vpiHandle obj)
{
      पूर्णांक scale = vpip_get_time_precision() -
	          vpip_time_units_from_handle(obj);
      जर (scale >= 0)
	    परत (द्विगुण)समय * घात(10.0, scale);
      अन्यथा
	    परत (द्विगुण)समय / घात(10.0, -scale);
}

शून्य vpi_get_time(vpiHandle obj, s_vpi_time*vp)
{
      vvp_time64_t समय;

      प्रतिज्ञा(vp);

      समय = schedule_simtime();

      निवड (vp->type) {
          स्थिती vpiSimTime:
	    vp->वरचा = (समय >> 32) & 0xffffffff;
	    vp->खालचा = समय & 0xffffffff;
	    थांबा;

          स्थिती vpiScaledRealTime:
	    vp->real = vpip_scaled_time_from_handle(समय, obj);
	    थांबा;

          पूर्वनिर्धारित:
            ख_लिखो(मानक_त्रुटि, "vpi_get_time: unknown type: %d\n", (पूर्णांक)vp->type);
            प्रतिज्ञा(0);
	    थांबा;
      }
}

PLI_INT32 vpi_get_vlog_info(p_vpi_vlog_info vlog_info_p)
{
    जर (vlog_info_p != 0) {
	  *vlog_info_p = vpi_vlog_info;
	  परत 1;
    } अन्यथा {
	  परत 0;
    }
}

शून्य vpi_set_vlog_info(पूर्णांक argc, अक्षर** argv)
{
    स्थिर अक्षर icarus_product[] = "Icarus Verilog";
    स्थिर अक्षर icarus_version[] = VERSION;
    vpi_vlog_info.product = icarus_product;
    vpi_vlog_info.version = icarus_version;
    vpi_vlog_info.argc    = argc;
    vpi_vlog_info.argv    = argv;

    स्थिर अक्षर trace_buf[1024];
    जर (स्थिरांक अक्षर*path = दो_पर्या("VPI_TRACE"))  {
	  जर (!म_भेद(path,"-"))
		vpi_trace = मानक_निकास;
	  अन्यथा {
		vpi_trace = ख_खोलो(path, "w");
		जर (!vpi_trace) {
		      लिखो_त्रुटि(path);
		      निकास(1);
		}
		रखो_भबफ(vpi_trace, trace_buf, _IOLBF, आकार(trace_buf));
	  }
    }
}

स्थिर शून्य vec4_get_मूल्य_string(स्थिरांक vvp_vector4_t&word_val, अचिन्हित width,
				  s_vpi_मूल्य*vp)
{
      अचिन्हित nchar = width / 8;
      अचिन्हित tail = width % 8;

      अक्षर*rbuf = static_cast<अक्षर *>(need_result_buf(nchar + 1, RBUF_VAL));
      अक्षर*cp = rbuf;

      जर (tail > 0) {
	    अक्षर char_val = 0;
	    साठी (अचिन्हित idx = width-tail; idx < width ;  idx += 1) {
		  vvp_bit4_t val = word_val.value(idx);
		  जर (val == BIT4_1)
			char_val |= 1 << idx;
	    }

	    जर (char_val != 0)
		  *cp++ = char_val;
	    अन्यथा जर (cp != rbuf)
		  *cp++ = ' ';
      }

      साठी (अचिन्हित idx = 0 ;  idx < nchar ;  idx += 1) {
	    अचिन्हित bit = (nchar - idx - 1) * 8;
	    अक्षर char_val = 0;
	    साठी (अचिन्हित bdx = 0 ;  bdx < 8 ;  bdx += 1) {
		  vvp_bit4_t val = word_val.value(bit+bdx);
		  जर (val == BIT4_1)
			char_val |= 1 << bdx;
	    }
	    // Ignore leading null-bytes and replace other null-bytes with space.
	    // The LRM is not entirely clear on how null bytes should be handled.
	    // This is the implementation chosen for iverilog.
	    जर (char_val != 0)
		  *cp++ = char_val;
	    अन्यथा जर (cp != rbuf)
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
शून्य vpip_vec4_get_मूल्य(स्थिरांक vvp_vector4_t&word_val, अचिन्हित width,
			 तार्किक signed_flag, s_vpi_मूल्य*vp)
{
      अक्षर *rbuf = 0;

      निवड (vp->format) {
	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "sorry: Format %d not implemented for "
	                    "getting vector values.\n", (पूर्णांक)vp->format);
	    प्रतिज्ञा(0);

	  स्थिती vpiSuppressVal:
	    थांबा;

	  स्थिती vpiBinStrVal:
	    rbuf = static_cast<अक्षर *>(need_result_buf(width+1, RBUF_VAL));
	    साठी (अचिन्हित idx = 0 ;  idx < width ;  idx += 1) {
		  vvp_bit4_t bit = word_val.value(idx);
		  rbuf[width-idx-1] = vvp_bit4_to_ascii(bit);
	    }
	    rbuf[width] = 0;
	    vp->value.str = rbuf;
	    थांबा;

	  स्थिती vpiOctStrVal: {
		अचिन्हित hwid = ((width+2) / 3) + 1;
		rbuf = static_cast<अक्षर *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_oct_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		थांबा;
	  }

	  स्थिती vpiDecStrVal: {
// HERE need a better estimate.
		rbuf = static_cast<अक्षर *>(need_result_buf(width+1, RBUF_VAL));
		vpip_vec4_to_dec_str(word_val, rbuf, width+1, signed_flag);
		vp->value.str = rbuf;
		थांबा;
	  }

	  स्थिती vpiHexStrVal: {
		अचिन्हित  hwid = ((width + 3) / 4) + 1;
		rbuf = static_cast<अक्षर *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_hex_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		थांबा;
	  }

          स्थिती vpiScalarVal: {
	        // scalars should be of size 1
	        प्रतिज्ञा(width == 1);
	        निवड(word_val.value(0)) {
		    स्थिती BIT4_0:
		      vp->value.scalar = vpi0;
		      थांबा;
	            स्थिती BIT4_1:
                      vp->value.scalar = vpi1;
                      थांबा;
	            स्थिती BIT4_X:
                      vp->value.scalar = vpiX;
                      थांबा;
	            स्थिती BIT4_Z:
                      vp->value.scalar = vpiZ;
                      थांबा;
		}
                थांबा;
          }

	  स्थिती vpiIntVal: {
		दीर्घ val = 0;
		vvp_bit4_t pad = BIT4_0;
		जर (signed_flag && word_val.size() > 0)
		      pad = word_val.value(word_val.size()-1);

		साठी (अचिन्हित idx = 0 ; idx < 8*आकार(val) ;  idx += 1) {
		      vvp_bit4_t val4 = pad;
		      जर (idx < word_val.size())
			    val4 = word_val.value(idx);
		      जर (val4 == BIT4_1)
			    val |= 1L << idx;
		}

		vp->value.integer = val;
		थांबा;
	  }

	  स्थिती vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiVectorVal;
	    // fallthrough
	  स्थिती vpiVectorVal: {
		अचिन्हित hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * आकार(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		साठी (अचिन्हित idx = 0 ;  idx < width ;  idx += 1) {
		      निवड (word_val.value(idx)) {
			  स्थिती BIT4_0:
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    थांबा;
			  स्थिती BIT4_1:
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    थांबा;
			  स्थिती BIT4_X:
			    op->aval |= (1 << idx % 32);
			    op->bval |= (1 << idx % 32);
			    थांबा;
			  स्थिती BIT4_Z:
			    op->aval &= ~(1 << idx % 32);
			    op->bval |=  (1 << idx % 32);
			    थांबा;
		      }
		      जर (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		थांबा;
	  }

	  स्थिती vpiStringVal:
		vec4_get_मूल्य_string(word_val, width, vp);
		थांबा;

	  स्थिती vpiRealVal:
		vector4_to_मूल्य(word_val, vp->value.real, signed_flag);
		थांबा;
      }
}

शून्य vpip_vec2_get_मूल्य(स्थिरांक vvp_vector2_t&word_val, अचिन्हित width,
			 तार्किक signed_flag, s_vpi_मूल्य*vp)
{
      निवड (vp->format) {
	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "sorry: Format %d not implemented for "
	                    "getting vector2 values.\n", (पूर्णांक)vp->format);
	    प्रतिज्ञा(0);

	  स्थिती vpiSuppressVal:
	    थांबा;

	  स्थिती vpiIntVal:
	    vector2_to_मूल्य(word_val, vp->value.integer, signed_flag);
	    थांबा;

	  स्थिती vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  स्थिती vpiVectorVal: {
		अचिन्हित hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * आकार(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		साठी (अचिन्हित idx = 0 ;  idx < width ;  idx += 1) {
		      जर (word_val.value(idx)) {
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      } अन्यथा {
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      }
		      जर (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		थांबा;
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
      जर (real != real || (real && (real == 0.5*real))) {
	    परत 0;
      }

      rtn = std::round(real);

      परत (PLI_INT32) rtn;
}

/*
 * This is a generic function to convert a double value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer.
 */
शून्य vpip_real_get_मूल्य(द्विगुण real, s_vpi_मूल्य*vp)
{
      अक्षर *rbuf = 0;

      निवड (vp->format) {
	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "sorry: Format %d not implemented for "
	                    "getting real values.\n", (पूर्णांक)vp->format);
	    प्रतिज्ञा(0);

	  स्थिती vpiSuppressVal:
	    थांबा;

	  स्थिती vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiRealVal;
	    // fallthrough
	  स्थिती vpiRealVal:
	    vp->value.real = real;
	    थांबा;

	  स्थिती vpiIntVal:
	    vp->value.integer = get_real_as_int(real);
	    थांबा;

	  स्थिती vpiDecStrVal:
	    rbuf = static_cast<अक्षर *>(need_result_buf(1025, RBUF_VAL));
	    vpip_vec4_to_dec_str(vvp_vector4_t(1024, real), rbuf, 1025, सत्य);
	    vp->value.str = rbuf;
	    थांबा;
      }
}

द्विगुण real_from_vpi_मूल्य(s_vpi_मूल्य*vp)
{
      vvp_vector4_t vec4(1024);
      द्विगुण result;
      तार्किक is_signed = असत्य;

      निवड (vp->format) {
	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "sorry: Format %d not implemented for "
	                    "putting real values.\n", (पूर्णांक)vp->format);
	    प्रतिज्ञा(0);

	  स्थिती vpiRealVal:
	    result = vp->value.real;
	    थांबा;

	  स्थिती vpiIntVal:
	    result = (द्विगुण) vp->value.integer;
	    थांबा;

	  स्थिती vpiBinStrVal:
	    vpip_bin_str_to_vec4(vec4, vp->value.str);
	    जर (vp->value.str[0] == '-') is_signed = सत्य;
	    vector4_to_मूल्य(vec4, result, is_signed);
	    थांबा;

	  स्थिती vpiOctStrVal:
	    vpip_oct_str_to_vec4(vec4, vp->value.str);
	    जर (vp->value.str[0] == '-') is_signed = सत्य;
	    vector4_to_मूल्य(vec4, result, is_signed);
	    थांबा;

	  स्थिती vpiDecStrVal:
	    vpip_dec_str_to_vec4(vec4, vp->value.str);
	    जर (vp->value.str[0] == '-') is_signed = सत्य;
	    vector4_to_मूल्य(vec4, result, is_signed);
	    थांबा;

	  स्थिती vpiHexStrVal:
	    vpip_hex_str_to_vec4(vec4, vp->value.str);
	    जर (vp->value.str[0] == '-') is_signed = सत्य;
	    vector4_to_मूल्य(vec4, result, is_signed);
	    थांबा;

      }

      परत result;
}

शून्य vpip_string_get_मूल्य(स्थिरांक string&val, s_vpi_मूल्य*vp)
{
      अक्षर *rbuf = 0;

      निवड (vp->format) {
	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "sorry: Format %d not implemented for "
	                    "getting string values.\n", (पूर्णांक)vp->format);
	    प्रतिज्ञा(0);

	  स्थिती vpiSuppressVal:
	    थांबा;

	  स्थिती vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiStringVal;
	    // fallthrough
	  स्थिती vpiStringVal:
	    rbuf = static_cast<अक्षर *>(need_result_buf(val.size() + 1, RBUF_VAL));
	    म_नकल(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    थांबा;
      }
}


शून्य vpi_get_मूल्य(vpiHandle अभिव्यक्ती, s_vpi_मूल्य*vp)
{
      प्रतिज्ञा(अभिव्यक्ती);
      प्रतिज्ञा(vp);

	// Never bother with suppressed values. All the derived
	// classes can ignore this type.
      जर (vp->format == vpiSuppressVal)
	    परत;

      अभिव्यक्ती->vpi_get_मूल्य(vp);

      जर (vpi_trace) निवड (vp->format) {
		स्थिती vpiStringVal:
		  ख_लिखो(vpi_trace,"vpi_get_value(%p=<%d>) -> string=\"%s\"\n",
			  अभिव्यक्ती, अभिव्यक्ती->get_type_code(), vp->value.str);
		  थांबा;

		स्थिती vpiBinStrVal:
		  ख_लिखो(vpi_trace, "vpi_get_value(<%d>...) -> binstr=%s\n",
			  अभिव्यक्ती->get_type_code(), vp->value.str);
		  थांबा;

		स्थिती vpiIntVal:
		  ख_लिखो(vpi_trace, "vpi_get_value(<%d>...) -> int=%d\n",
			  अभिव्यक्ती->get_type_code(), (पूर्णांक)vp->value.integer);
		  थांबा;

		स्थिती vpiSuppressVal:
		  ख_लिखो(vpi_trace, "vpi_get_value(<%d>...) -> <suppress>\n",
			  अभिव्यक्ती->get_type_code());
		  थांबा;

		पूर्वनिर्धारित:
		  ख_लिखो(vpi_trace, "vpi_get_value(<%d>...) -> <%d>=?\n",
			  अभिव्यक्ती->get_type_code(), (पूर्णांक)vp->format);
	    }
}

संरचना vpip_put_मूल्य_event : vvp_gen_event_s {
      vpiHandle handle;
      s_vpi_मूल्य value;
      पूर्णांक flags;
      आभासी शून्य run_run() override;
      ~vpip_put_मूल्य_event() override { }
};

शून्य vpip_put_मूल्य_event::run_run()
{
      handle->vpi_put_मूल्य(&value, flags);
      निवड (value.format) {
	    /* Free the copied string. */
	  स्थिती vpiBinStrVal:
	  स्थिती vpiOctStrVal:
	  स्थिती vpiDecStrVal:
	  स्थिती vpiHexStrVal:
	  स्थिती vpiStringVal:
	    मुक्त(value.value.str);
	    थांबा;
	    /* Free the copied time structure. */
	  स्थिती vpiTimeVal:
	    मुक्त(value.value.समय);
	    थांबा;
	    /* Free the copied vector structure. */
	  स्थिती vpiVectorVal:
	    मुक्त(value.value.vector);
	    थांबा;
	    /* Free the copied strength structure. */
	  स्थिती vpiStrengthVal:
	    मुक्त(value.value.strength);
	    थांबा;
	    /* Everything else is static in the structure. */
	  पूर्वनिर्धारित:
	    थांबा;
      }
}

/* Make a copy of a pointer to a time structure. */
स्थिर t_vpi_time *timedup(स्थिरांक t_vpi_time *val)
{
      t_vpi_time *rtn;
      rtn = static_cast<t_vpi_time *> (दो_स्मृति(आकार(t_vpi_time)));
      *rtn = *val;
      परत rtn;
}

/* Make a copy of a pointer to a vector value structure. */
स्थिर t_vpi_vecval *vectordup(स्थिरांक t_vpi_vecval *val, PLI_INT32 size)
{
      अचिन्हित अंक_bytes;
      t_vpi_vecval *rtn;
      प्रतिज्ञा(size > 0);
      अंक_bytes = ((size + 31)/32)*आकार(t_vpi_vecval);
      rtn = static_cast<t_vpi_vecval *> (दो_स्मृति(अंक_bytes));
      स_नकल(rtn, val, अंक_bytes);
      परत rtn;
}

/* Make a copy of a pointer to a strength structure. */
स्थिर t_vpi_strengthval *strengthdup(स्थिरांक t_vpi_strengthval *val)
{
      t_vpi_strengthval *rtn;
      rtn = static_cast<t_vpi_strengthval *>
            (दो_स्मृति(आकार(t_vpi_strengthval)));
      *rtn = *val;
      परत rtn;
}

vpiHandle vpi_put_मूल्य(vpiHandle obj, s_vpi_मूल्य*vp,
			s_vpi_time*when, PLI_INT32 flags)
{
      प्रतिज्ञा(obj);

      flags &= ~vpiReturnEvent;

      जर (flags!=vpiNoDelay && flags!=vpiForceFlag && flags!=vpiReleaseFlag) {
	    vvp_time64_t dly;
	    पूर्णांक scale;

	    जर (vpi_get(vpiAutomatic, obj)) {
		  ख_लिखो(मानक_त्रुटि, "VPI error: cannot put a value with "
				  "a delay on automatically allocated "
				  "variable '%s'.\n",
				  vpi_get_str(vpiName, obj));
		  परत 0;
	    }

	    प्रतिज्ञा(when != 0);

	    निवड (when->type) {
		स्थिती vpiScaledRealTime:
		  scale = vpip_time_units_from_handle(obj) -
		          vpip_get_time_precision();
		  जर (scale >= 0) {
			dly = (vvp_time64_t)(when->real * घात(10.0, scale));
		  } अन्यथा {
			dly = (vvp_time64_t)(when->real / घात(10.0, -scale));
		  }
		  थांबा;
		स्थिती vpiSimTime:
		  dly = vpip_timestruct_to_time(when);
		  थांबा;
		पूर्वनिर्धारित:
		  dly = 0;
		  थांबा;
	    }

	    जर ((dly == 0) && schedule_at_rosync()) {
		  ख_लिखो(मानक_त्रुटि, "VPI error: attempted to put a value to "
				  "variable '%s' during a read-only synch "
				  "callback.\n", vpi_get_str(vpiName, obj));
		  परत 0;
	    }

	    vpip_put_मूल्य_event*put = नवीन vpip_put_मूल्य_event;
	    put->handle = obj;
	    जर (dynamic_cast<__vpiNamedEvent*>(obj)) {
		  put->value.format = vpiIntVal;
		  put->value.value.integer = 0;
	    } अन्यथा {
		  प्रतिज्ञा(vp);
		  put->value = *vp;
	    }
	      /* Since this is a scheduled put event we must copy any pointer
	       * data to keep it available until the event is actually run. */
	    निवड (put->value.format) {
		  /* Copy the string items. */
		स्थिती vpiBinStrVal:
		स्थिती vpiOctStrVal:
		स्थिती vpiDecStrVal:
		स्थिती vpiHexStrVal:
		स्थिती vpiStringVal:
		  put->value.value.str = strdup(put->value.value.str);
		  थांबा;
		  /* Copy a time pointer item. */
		स्थिती vpiTimeVal:
		  put->value.value.समय = timedup(put->value.value.समय);
		  थांबा;
		  /* Copy a vector pointer item. */
		स्थिती vpiVectorVal:
		  put->value.value.vector = vectordup(put->value.value.vector,
		                                      vpi_get(vpiSize, obj));
		  थांबा;
		  /* Copy a strength pointer item. */
		स्थिती vpiStrengthVal:
		  put->value.value.strength =
		      strengthdup(put->value.value.strength);
		  थांबा;
		  /* Everything thing else is already in the structure. */
		पूर्वनिर्धारित:
		  थांबा;
	    }
	    put->flags = flags;
	    schedule_generic(put, dly, असत्य, सत्य, सत्य);
	    परत 0;
      }

      जर (schedule_at_rosync()) {
            ख_लिखो(मानक_त्रुटि, "VPI error: attempted to put a value to "
			    "variable '%s' during a read-only synch "
			    "callback.\n", vpi_get_str(vpiName, obj));
            परत 0;
      }

      obj->vpi_put_मूल्य(vp, flags);

      परत 0;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle res = 0;

      जर (ref == 0) {
	      // A few types can apply to a nil handle. These are ways
	      // that the current function can get started finding things.
	    निवड (type) {

		स्थिती vpiScope:
		    // The IEEE1364-2005 doesn't seem to allow this,
		    // but some users seem to think it's handy, so
		    // return the scope that contains this SysTfCall.
		  प्रतिज्ञा(vpip_cur_task);
		  res = vpip_cur_task->vpi_handle(vpiScope);
		  थांबा;

		स्थिती vpiSysTfCall:
		    // This is how VPI users get a first handle into
		    // the system. This is the handle of the system
		    // task/function call currently being executed.
		  जर (vpi_trace) {
			ख_लिखो(vpi_trace, "vpi_handle(vpiSysTfCall, 0) "
				"-> %p (%s)\n", vpip_cur_task,
				vpip_cur_task->defn->info.tfname);
		  }
		  परत vpip_cur_task;

		पूर्वनिर्धारित:
		  ख_लिखो(मानक_त्रुटि, "VPI error: vpi_handle(type=%d, ref=0).\n",
			  (पूर्णांक)type);
		  res = 0;
		  थांबा;
	    }

      } अन्यथा {

	    जर (type == vpiSysTfCall) {
		  ख_लिखो(मानक_त्रुटि, "VPI error: vpi_handle(vpiSysTfCall, "
			  "ref!=0).\n");
		  परत 0;
	    }

	    res = ref->vpi_handle(type);
      }


      जर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_handle(vpiScope, ref=%p) "
		    "-> %p\n", vpip_cur_task, ref);
      }

      परत res;
}

स्थिर vpiHandle vpip_make_udp_iterator()
{
// HERE: Add support for iterating over UDP definitions.
//       See 26.6.16 (page 400 in 1364-2005).
      परत 0;
}

/*
 * This function asks the object to return an iterator for
 * the specified reference. It is up to the iterate_ method to
 * allocate a properly formed iterator.
 */
स्थिर vpiHandle vpi_iterate_global(पूर्णांक type)
{
      निवड (type) {
	  स्थिती vpiInstance:
	    // fallthrough
	  स्थिती vpiModule:
	    // fallthrough
	  स्थिती vpiProgram:
	    // fallthrough
	  स्थिती vpiInterface:
	    // fallthrough
	  स्थिती vpiPackage:
	    परत vpip_make_मूळ_iterator(type);

	  स्थिती vpiUdpDefn:
	    परत vpip_make_udp_iterator();

	  स्थिती vpiUserSystf:
	    परत vpip_make_systf_iterator();
      }

      परत 0;
}

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle rtn = 0;

      प्रतिज्ञा(vpi_mode_flag != VPI_MODE_NONE);
      जर (vpi_mode_flag == VPI_MODE_REGISTER) {
	    ख_लिखो(मानक_त्रुटि, "vpi error: vpi_iterate called during "
		    "vpi_register_systf. You can't do that!\n");
	    परत 0;
      }

      जर (ref == 0)
	    rtn = vpi_iterate_global(type);
      अन्यथा
	    rtn = ref->vpi_iterate(type);

      जर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_iterate(%d, %p) ->%s\n",
	    (पूर्णांक)type, ref, rtn ? "" : " (null)");
      }

      परत rtn;
}

vpiHandle vpi_handle_by_index(vpiHandle ref, PLI_INT32 idx)
{
      प्रतिज्ञा(ref);
      परत ref->vpi_index(idx);
}

स्थिर vpiHandle find_name(स्थिरांक अक्षर *name, vpiHandle handle)
{
      vpiHandle rtn = 0;
      __vpiScope*ref = dynamic_cast<__vpiScope*>(handle);

      /* check module names */
      जर (!म_भेद(name, vpi_get_str(vpiName, handle)))
	    rtn = handle;

      /* brute force search for the name in all objects in this scope */
      साठी (अचिन्हित i = 0 ;  i < ref->intern.size() ;  i += 1) {
	      /* The standard says that since a port does not have a full
	       * name it cannot be found by name. Because of this we need
	       * to skip ports here so the correct handle can be located. */
	    जर (vpi_get(vpiType, ref->intern[i]) == vpiPort) सुरूठेवा;
	    स्थिरांक अक्षर *nm = vpi_get_str(vpiName, ref->intern[i]);
	    जर (nm && !म_भेद(name, nm)) {
		  rtn = ref->intern[i];
		  थांबा;
	    } अन्यथा जर (vpi_get(vpiType, ref->intern[i]) == vpiMemory ||
	               vpi_get(vpiType, ref->intern[i]) == vpiNetArray) {
		  /* We need to iterate on the words */
		  vpiHandle word_i, word_h;
		  word_i = vpi_iterate(vpiMemoryWord, ref->intern[i]);
		  पर्यंत (word_i && (word_h = vpi_scan(word_i))) {
			nm = vpi_get_str(vpiName, word_h);
			जर (nm && !म_भेद(name, nm)) {
			      rtn = word_h;
			      vpi_free_object(word_i);
			      थांबा;
			}
		  }
	    }
	    /* found it yet? */
	    जर (rtn) थांबा;
      }

      परत rtn;
}

// Find the end of the escaped identifier or simple identifier
स्थिर अक्षर * find_rest(अक्षर *name)
{
      अक्षर *rest;
      जर (*name == '\\') {
	    rest = म_अक्षर(name, ' ');
	    जर (rest) {
		  *rest++ = 0; // The space is not part of the string
		    // There should be a '.' after the escaped ID if there is
		    // anything more. If it is missing add it to avoid a crash
		  जर ((*rest != '.') && (*rest != 0)) {
			*--rest = '.';
			ख_लिखो(मानक_त्रुटि, "ERROR: Malformed scope string: \"%s\"", name);
		  }
		  जर (*rest == 0) {
			rest = 0;
		  }
	    }
      } अन्यथा {
	    rest = म_अक्षर(name, '.');
      }
      परत rest;
}

स्थिर vpiHandle find_scope(स्थिरांक अक्षर *name, vpiHandle handle, पूर्णांक depth)
{

      vpiHandle iter = handle==0
	    ? vpi_iterate(vpiModule, रिक्त)
	    : vpi_iterate(vpiInternalScope, handle);

      vector<अक्षर> name_buf (म_माप(name)+1);
      म_नकल(&name_buf[0], name);
      अक्षर*nm_first = &name_buf[0];
      अक्षर*nm_rest = find_rest(nm_first);
      जर (*nm_first == '\\') ++nm_first;
      जर (nm_rest) {
	    *nm_rest++ = 0;
      }

      vpiHandle rtn = 0;
      vpiHandle hand;
      पर्यंत (iter && (hand = vpi_scan(iter))) {
	    स्थिरांक अक्षर *nm = vpi_get_str(vpiName, hand);

	    जर (म_भेद(nm_first,nm)==0) {
		  जर (nm_rest)
			rtn=find_scope(nm_rest, hand, depth+1);
		  अन्यथा
			rtn = hand;
	    }

	    /* found it yet ? */
	    जर (rtn) {
		  vpi_free_object(iter);
		  थांबा;
	    }
      }

      परत rtn;
}

// Find the end of the first escaped identifier or simple identifier
स्थिर अक्षर * find_next(अक्षर *name)
{
      अक्षर *next;
      जर (*name == '\\') {
	    next = म_अक्षर(name, ' ');
	    जर (next && *++next == 0) next = 0;
      } अन्यथा {
	    next = म_अक्षर(name, '.');
      }
      परत next;
}

vpiHandle vpi_handle_by_name(स्थिरांक अक्षर *name, vpiHandle scope)
{
      vpiHandle hand;

      जर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_handle_by_name(%s, %p) -->\n",
		    name, scope);
      }

	// Chop the name into path and base. For example, if the name
	// is "a.b.c", then nm_path becomes "a.b" and nm_base becomes
	// "c". If the name is "c" then nm_path is nil and nm_base is "c".
      vector<अक्षर> name_buf (म_माप(name)+1);
      म_नकल(&name_buf[0], name);
      अक्षर*nm_path = &name_buf[0];
      अक्षर*nm_base;

	// Check to see if we have an escaped identifier and if so search
	// the long way because a '.' could be in the escaped name.
      जर (म_अक्षर(nm_path, '\\')) {
	    अक्षर *next;
	    nm_base = nm_path;
	    पर्यंत ((next = find_next(nm_base))) {
		  nm_base = ++next;
	    }
	    जर (nm_path == nm_base) {
		  nm_path = 0;
	    } अन्यथा {
		  *(nm_base-1) = 0;
	    }

	// If there is no escaped identifier then just look for the last '.'
      } अन्यथा {
	    nm_base = म_खोजप(nm_path, '.');
	    जर (nm_base) {
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
      जर (scope) {
	    /* Some implementations support either a module or a scope. */
	    निवड (vpi_get(vpiType, scope)) {
		स्थिती vpiScope:
	          hand = vpi_handle(vpiModule, scope);
	          थांबा;
		स्थिती vpiModule:
	          hand = scope;
	          थांबा;
		पूर्वनिर्धारित:
		  जर (vpi_trace) {
			ख_लिखो(vpi_trace, "vpi_handle_by_name: "
				"Scope is not a vpiScope or vpiModule\n");
		  }
	          // Use vpi_chk_error() here when it is implemented.
	          परत 0;
	    }

      } अन्यथा जर (nm_path) {
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

      जर (hand == 0) {
	    जर (vpi_trace) {
		  ख_लिखो(vpi_trace, "vpi_handle_by_name: "
			  "Scope does not exist. Giving up.\n");
	    }

	    परत 0;
      }

	// If there is a path part, then use it to find the
	// scope. For example, if the full name is a.b.c, then
	// the nm_path string is a.b and we search for that
	// scope. If we find it, then set hand to that scope.
      जर (nm_path) {
	    vpiHandle tmp = find_scope(nm_path, hand, 0);
	    पर्यंत (tmp == 0 && hand != 0) {
		  hand = vpi_handle(vpiScope, hand);
		  tmp = find_scope(nm_path, hand, 0);
	    }
	    hand = tmp;
      }

	// find_name() expects escaped identifiers to be stripped
      जर (*nm_base == '\\') {
	    // Skip the \ at the beginning
	    ++nm_base;

	    // Drop the space at the end if it exists
	    अक्षर *next;
	    जर ((next = म_अक्षर(nm_base, ' '))) {
		  *next = 0;
	    }
      }

	// Now we have the correct scope, look for the item.
      vpiHandle out = find_name(nm_base, hand);

      जर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_handle_by_name: DONE\n");
      }

      परत out;
}

// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected
// x2: Ptr to previous node
// x3: True when net is connected to output of previous node
tuple<तार्किक, vvp_net_ptr_t, तार्किक> check_connected_to_concat8_and_part_sa(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent) {

      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      पर्यंत (cur.ptr()) {
	    जर (cur.ptr() == net2 && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
		    // Pass on the return value if found
		  परत {सत्य, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
	    } अन्यथा जर (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  स्वतः res = check_connected_to_concat8_and_part_sa(cur.ptr()->out_, net2, cur);
		  जर (get<0>(res)) {
			परत res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      परत {असत्य, vvp_net_ptr_t(nullptr, 0), असत्य};
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
      पर्यंत (cur.ptr()) {
	      // Check if it's a concat8
	    जर (cur.ptr() == net2) {
		  परत {सत्य, vvp_net_ptr_t(nullptr, 0), असत्य};
	    } अन्यथा जर (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  स्वतः res = check_connected_to_concat8(cur.ptr()->out_, net2, cur);
		  जर (get<0>(res)) {
			जर (!get<1>(res).ptr() && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
			      // `parent` is the last part_sa node in front of our destination
			      // We want to place the intermod node here
			      परत {सत्य, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
			}
			परत res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      परत {असत्य, vvp_net_ptr_t(nullptr, 0), असत्य};
}

// Used to get intermodpath for two ports
vpiHandle vpi_handle_multi(PLI_INT32 type,
                           vpiHandle ref1,
                           vpiHandle ref2)
{
      जर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_handle_multi(%d, %p, %p) -->\n",
	            type, ref1, ref2);
      }

      जर (type != vpiInterModPath) {
	    ख_लिखो(मानक_त्रुटि, "sorry: vpi_handle_multi currently supports"
	            "only vpiInterModPath\n");
	    परत nullptr;
      }

	// Indicates whether port1 refers to a single bit
      तार्किक port1_has_index = असत्य;
      पूर्णांक port1_bit_index = 0;
      vpiPortBitInfo* port1_bit = dynamic_cast<vpiPortBitInfo*>(ref1);

      जर (port1_bit) {
	      // Get the bit index
	    port1_has_index = सत्य;
	    port1_bit_index = vpi_get(vpiBit, port1_bit);
	      // Update the ref1 to point to the base port
	    ref1 = vpi_handle(vpiParent, port1_bit);
      }

	// Indicates whether port2 refers to a single bit
      तार्किक port2_has_index = असत्य;
      पूर्णांक port2_bit_index = 0;
      vpiPortBitInfo* port2_bit = dynamic_cast<vpiPortBitInfo*>(ref2);

      जर (port2_bit) {
	      // Get the bit index
	    port2_has_index = सत्य;
	    port2_bit_index = vpi_get(vpiBit, port2_bit);
	      // Update the ref1 to point to the base port
	    ref2 = vpi_handle(vpiParent, port2_bit);
      }

      vpiPortInfo* port1 = dynamic_cast<vpiPortInfo*>(ref1);

      जर (!port1) {
	    ख_लिखो(मानक_त्रुटि, "sorry: second argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    परत nullptr;
      }

      vpiPortInfo* port2 = dynamic_cast<vpiPortInfo*>(ref2);

      जर (!port2) {
	    ख_लिखो(मानक_त्रुटि, "sorry: third argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    परत nullptr;
      }

	// Get the names of both ports
      std::string port1_name(vpi_get_str(vpiName, ref1));
      std::string port2_name(vpi_get_str(vpiName, ref2));

	// If both ports are vpiOutput, we have to reassign the __vpiSignal from port1
	// to port2 because otherwise the non-delayed version of the signal is dumped
	// even tho the intermodpath is correctly inserted
      __vpiSignal* output_signal = nullptr;

      जर (port1->get_direction() == vpiOutput && port2->get_direction() == vpiOutput) {
	    vpiHandle scope_port2 = vpi_handle(vpiScope, ref2);
	    प्रतिज्ञा(scope_port2);

	      // Iterate over nets in the scope of port2
	    vpiHandle net_i = vpi_iterate(vpiNet, scope_port2) ;
	    vpiHandle net;

	    पर्यंत ((net = vpi_scan(net_i)) != रिक्त) {
		  std::string net_name(vpi_get_str(vpiName, net));

		    // Compare whether the net matches with the port name
		  जर (net_name == port2_name) {
			output_signal = dynamic_cast<__vpiSignal*>(net);
		  }
	    }
      }

      vvp_net_t* net1 = port1->get_port();
      vvp_net_t* net2 = port2->get_port();

      जर (net1 == nullptr || net2 == nullptr) {
	    ख_लिखो(मानक_त्रुटि, "Error: Could not find net. "
		    "Did you run iverilog with '-ginterconnect'?\n");
	    परत nullptr;
      }

      जर (net1 == net2) {
	    ख_लिखो(मानक_त्रुटि, "Error: Net for both ports is the same. "
		    "Did you pass the same port twice?\n");
	    परत nullptr;
      }

      जर (!dynamic_cast<vvp_fun_buft*>(net1->fun)) {
	    ख_लिखो(मानक_त्रुटि, "Error: functor of net1 must be"
		    "vvp_fun_buft\n");
	    परत nullptr;
      }

      जर (!dynamic_cast<vvp_fun_buft*>(net2->fun)) {
	    ख_लिखो(मानक_त्रुटि, "Error: functor of net2 must be"
		    "vvp_fun_buft\n");
	    परत nullptr;
      }

	// If port1 is actually a port bit, we have to get to the correct vvp_fun_part
	// after which we insert the intermodpath delay
      जर (port1_has_index) {
	    vvp_net_ptr_t* net1_ptr = &net1->out_;

	      // Search for part selects connected to port1
	    vvp_net_t* current_net = net1_ptr->ptr();

	    पर्यंत (current_net) {
		  स्थिरांक vvp_fun_part* part = dynamic_cast<vvp_fun_part*>(current_net->fun);

		    // Its a part select!
		  जर (part) {
			  // Is it the correct part select?
			जर (part->get_base() == (अचिन्हित)port1_bit_index) {
			      प्रतिज्ञा(part->get_wid() == 1);
			      net1 = current_net; // Replace net1 as this is our new start point
			      थांबा;
			}
		  }

		  current_net = current_net->port[0].ptr(); // BUFT has only one input, index 0
	    }
      }

	// Iterate over all nodes connected to port1
      vvp_net_ptr_t cur = net1->out_;
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      पर्यंत (cur.ptr()) {
	      // Either port2 is directly connected to port1
	      // Or in the second case port2 is indirectly connected
	      // to port1 through a net of concat8s
	    तार्किक is_connected, is_output;
	    vvp_net_ptr_t previous_गाठ;
	    tie(is_connected, previous_गाठ, is_output) = port2_has_index
		  ? check_connected_to_concat8(cur, net2, vvp_net_ptr_t(net1, 0))
		  : check_connected_to_concat8_and_part_sa(cur, net2, vvp_net_ptr_t(net1, 0));
	    जर ( (!port2_has_index && cur.ptr() == net2) || is_connected ) {
		  vvp_net_t*new_net = नवीन vvp_net_t;

		    // Create new node with intermodpath and connect port2 to it
		  पूर्णांक width = 1; // TODO
		  vvp_fun_intermodpath*obj = नवीन vvp_fun_intermodpath(new_net, width);

		  vvp_net_t* output_net = net1;
		  जर (is_connected) {
			जर (is_output) {
			      output_net = previous_गाठ.ptr(); // net2 is direct output of part_sa
			      cur = output_net->out_;
			      prev = vvp_net_ptr_t(nullptr, 0);
			} अन्यथा {
			      prev = previous_गाठ;
			      जर (prev.ptr()) {
				    cur = prev.ptr()->port[prev.port()];
			      }
			}
		  }

		  new_net->fun = obj;
		  new_net->out_ = cur;

		    // Port2 is in the middle of the list
		    // Insert intermodpath before port2 and keep everything else intact
		  जर (prev.ptr()) {
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
		  जर (output_signal && !port2_has_index) {
			net2->fil = net1->fil;
			net1->fil = nullptr;
			output_signal->गाठ = net2;
		  }

		    // Create the VPI intermodpath object
		  __vpiInterModPath* intermodpath = vpip_make_intermodpath(new_net, port1, port2);
		  intermodpath->intermodpath = obj;

		    // Finally done, return the intermodpath object
		  परत intermodpath;
	    }

	    prev = cur;
	    cur = cur.ptr()->port[cur.port()]; // Next net in linked list
      }

      ख_लिखो(मानक_त्रुटि, "VPI error: Could not insert intermodpath!\n");
      ख_लिखो(मानक_त्रुटि, "\tport1 = %s, port1_has_index = %d, port1_bit_index = %d\n", port1_name.c_str(), port1_has_index, port1_bit_index);
      ख_लिखो(मानक_त्रुटि, "\tport2 = %s, port2_has_index = %d, port2_bit_index = %d\n", port2_name.c_str(), port2_has_index, port2_bit_index);
      परत nullptr;
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



शून्य vpi_get_delays(vpiHandle अभिव्यक्ती, p_vpi_delay delays)
{
      प्रतिज्ञा(अभिव्यक्ती);
      प्रतिज्ञा(delays);

      अभिव्यक्ती->vpi_get_delays(delays);

      जर (vpi_trace) {
	    ख_लिखो(vpi_trace,
		    "vpi_get_delays(%p, %p) -->\n", अभिव्यक्ती, delays);
      }
}


शून्य vpi_put_delays(vpiHandle अभिव्यक्ती, p_vpi_delay delays)
{
      प्रतिज्ञा(अभिव्यक्ती  );
      प्रतिज्ञा(delays );

      अभिव्यक्ती->vpi_put_delays(delays);

      जर (vpi_trace) {
	    ख_लिखो(vpi_trace,
		    "vpi_put_delays(%p, %p) -->\n", अभिव्यक्ती, delays);
      }
}








बाह्य "C" PLI_INT32 vpi_vprintf(स्थिरांक अक्षर*fmt, बहु_सूची ap)
{
      परत vpi_mcd_vprintf(1, fmt, ap);
}

बाह्य "C" PLI_INT32 vpi_printf(स्थिरांक अक्षर *fmt, ...)
{
      बहु_सूची ap;
      बहु_शुरू(ap, fmt);
      पूर्णांक r = vpi_mcd_vprintf(1, fmt, ap);
      बहु_पूर्ण(ap);
      परत r;
}

बाह्य "C" PLI_INT32 vpi_flush(शून्य)
{
      परत vpi_mcd_flush(1);
}


बाह्य "C" PLI_INT32 vpi_sim_vcontrol(पूर्णांक operation, बहु_सूची ap)
{
      दीर्घ diag_संदेश;

      निवड (operation) {
	  स्थिती vpiFinish:
	  स्थिती __ivl_legacy_vpiFinish:
            diag_संदेश = बहु_तर्क(ap, PLI_INT32);
	    schedule_finish(diag_संदेश);
	    थांबा;

	  स्थिती vpiStop:
	  स्थिती __ivl_legacy_vpiStop:
            diag_संदेश = बहु_तर्क(ap, PLI_INT32);
	    schedule_stop(diag_संदेश);
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "Unsupported VPI control operation %d.\n", operation);
	    परत 0;
      }
      परत 1;
}

बाह्य "C" PLI_INT32 vpi_sim_control(PLI_INT32 operation, ...)
{
      बहु_सूची ap;
      बहु_शुरू(ap, operation);
      पूर्णांक r = vpi_sim_vcontrol(operation, ap);
      बहु_पूर्ण(ap);
      परत r;
}

बाह्य "C" PLI_INT32 vpi_control(PLI_INT32 operation, ...)
{
      बहु_सूची ap;
      बहु_शुरू(ap, operation);
      पूर्णांक r = vpi_sim_vcontrol(operation, ap);
      बहु_पूर्ण(ap);
      परत r;
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
      जर (val.format == vpiRealVal) {
	    vpi_get_मूल्य(arg, &val);
	      /* All double values can be represented in 1024 bits. */
	    vec4 = vvp_vector4_t(1024, val.value.real);
	    जर (val.value.real < 0) is_neg = सत्य;
      } अन्यथा {
	    val.format = vpiVectorVal;
	    vpi_get_मूल्य(arg, &val);
	    अचिन्हित wid = vpi_get(vpiSize, arg);
	    vec4 = vvp_vector4_t(wid, BIT4_0);
	    साठी (अचिन्हित idx=0; idx < wid; idx += 1) {
		PLI_INT32 aval = val.value.vector[idx/32].aval;
		PLI_INT32 bval = val.value.vector[idx/32].bval;
		aval >>= idx % 32;
		bval >>= idx % 32;
		पूर्णांक bitmask = (aval&1) | ((bval<<1)&2);
		vvp_bit4_t bit = scalar_to_bit4(bitmask);
		vec4.set_bit(idx, bit);
	    }
      }

      जर (vec4.has_xz()) {
	    rtn.aval = rtn.bval = 0xFFFFFFFFU;  /* Set to 'bx. */
	    परत rtn;
      }

      vvp_vector2_t vec2(vec4);

      जर (is_neg) vec2.trim_neg(); /* This is a special trim! */
      अन्यथा vec2.trim(); /* This makes less work shifting. */

	/* Calculate the clog2 result. */
      PLI_INT32 res = 0;
      जर (!vec2.is_zero()) {
	    vec2 -= vvp_vector2_t(1, vec2.size());
	    पर्यंत(!vec2.is_zero()) {
		  res += 1;
		  vec2 >>= 1;
	    }
      }

      rtn.aval = res;
      rtn.bval = 0;
      परत rtn;
}

/*
 * This routine provides the information needed to implement $countdrivers.
 * It is done here for performance reasons - interrogating the drivers
 * individually via the VPI interface would be much slower.
 */
बाह्य "C" शून्य vpip_मोजणी_drivers(vpiHandle ref, अचिन्हित idx,
                                   अचिन्हित counts[4])
{
      संरचना __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      प्रतिज्ञा(rfp);
      rfp->गाठ->मोजणी_drivers(idx, counts);
}

#जर defined(__MINGW32__) || defined (__CYGWIN__)
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
    .मोजणी_drivers              = vpip_मोजणी_drivers,
    .format_strength            = vpip_format_strength,
    .format_pretty              = vpip_format_pretty,
    .make_systf_system_defined  = vpip_make_systf_system_defined,
    .mcd_rawwrite               = vpip_mcd_rawwrite,
    .set_return_मूल्य           = vpip_set_return_मूल्य,
};
#endif
