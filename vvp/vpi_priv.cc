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

using நாமாகாஶ std;
vpi_mode_t vpi_mode_flag = VPI_MODE_NONE;
FILE*vpi_trace = 0;

நிலையான s_vpi_vlog_info  vpi_vlog_info;
நிலையான s_vpi_error_info vpip_last_error = { 0, 0, 0, 0, 0, 0, 0 };

மாறிலி எழுத்து* direction_as_string(முழு dir)
{
      தேர்வு (dir) {
	  நிலை vpi_உள்ளீடு:
	    திரும்பு "input";
	  நிலை vpiOutput:
	    திரும்பு "output";
	  நிலை vpiInout:
	    திரும்பு "inout";
	  நிலை vpiMixedIO:
	    திரும்பு "mixed io";
	  நிலை vpiNoDirection:
	    திரும்பு "no direction";
	  இயல்பு:
	    திரும்பு "INVALID-DIRECTION";
      }
}

__vpiHandle::~__vpiHandle()
{ }

முழு __vpiHandle::vpi_get(முழு)
{ திரும்பு vpiUndefined; }

int64_t __vpiHandle::vpi_get64(முழு code)
{ திரும்பு (code == _vpiNexusId) ? 0 : vpiUndefined; }

எழுத்து* __vpiHandle::vpi_get_str(முழு)
{ திரும்பு 0; }

வெறுமை __vpiHandle::vpi_get_மதிப்பு(p_vpi_மதிப்பு)
{ }

vpiHandle __vpiHandle::vpi_put_மதிப்பு(p_vpi_மதிப்பு, முழு)
{ திரும்பு 0; }

vpiHandle __vpiHandle::vpi_handle(முழு)
{ திரும்பு 0; }

vpiHandle __vpiHandle::vpi_iterate(முழு)
{ திரும்பு 0; }

vpiHandle __vpiHandle::vpi_index(முழு)
{ திரும்பு 0; }

வெறுமை __vpiHandle::vpi_get_delays(p_vpi_delay)
{ }

வெறுமை __vpiHandle::vpi_put_delays(p_vpi_delay)
{ }

__vpiBaseVar::__vpiBaseVar(__vpiScope*scope, மாறிலி எழுத்து*name, vvp_net_t*net)
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
நிலையான முழு suppress_free(vpiHandle)
{ திரும்பு 1; }
__vpiHandle::free_object_fun_t __vpiHandle::free_object_fun(வெறுமை)
{ திரும்பு &suppress_free; }

/*
 * The vpip_string function creates a constant string from the pass
 * input. This constant string is permanently allocated from an
 * efficient string buffer store.
 */
கட்டமைப்பு vpip_string_chunk {
      கட்டமைப்பு vpip_string_chunk*next;
      எழுத்து data[64*1024 - அளவு (கட்டமைப்பு vpip_string_chunk*)];
};

unsigned vpip_size(மாறிலி __vpiSignal *sig)
{
      திரும்பு abs(sig->msb.get_மதிப்பு() - sig->lsb.get_மதிப்பு()) + 1;
}

__vpiScope* vpip_scope(__vpiSignal*sig)
{
      எனில் (sig->is_netarray)
	    திரும்பு static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      இல்லையெனில்
	    திரும்பு sig->within.scope;
}

__vpiScope* vpip_scope(__vpiRealVar*sig)
{
      எனில் (sig->is_netarray)
	    திரும்பு static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      இல்லையெனில்
	    திரும்பு sig->within.scope;
}

vpiHandle vpip_module(__vpiScope*scope)
{
      போது(scope && scope->get_type_code() != vpiModule) {
	    scope = scope->scope;
      }
      ப்ரதிஜ்ஞா(scope);
      திரும்பு scope;
}

மாறிலி எழுத்து *vpip_string(மாறிலி எழுத்து*str)
{
      நிலையான vpip_string_chunk first_chunk = {0, {0}};
      நிலையான vpip_string_chunk*chunk_list = &first_chunk;
      நிலையான unsigned chunk_fill = 0;

      unsigned len = strlen(str);
      ப்ரதிஜ்ஞா( (len+1) <= அளவு chunk_list->data );

      எனில் ( (len+1) > (அளவு chunk_list->data - chunk_fill) ) {
	    vpip_string_chunk*tmp = புதிய vpip_string_chunk;
	    tmp->next = chunk_list;
	    chunk_list = tmp;
	    chunk_fill = 0;
      }

      எழுத்து*res = chunk_list->data + chunk_fill;
      chunk_fill += len + 1;

      strcpy(res, str);
      திரும்பு res;
}

நிலையான unsigned hash_string(மாறிலி எழுத்து*text)
{
      unsigned h = 0;

      போது (*text) {
	    h = (h << 4) ^ (h >> 28) ^ (unsigned)*text;
	    text += 1;
      }
      திரும்பு h;
}

மாறிலி எழுத்து* vpip_name_string(மாறிலி எழுத்து*text)
{
      மாறிலி unsigned HASH_SIZE = 4096;
      நிலையான மாறிலி எழுத்து*hash_table[HASH_SIZE] = {0};

      unsigned hash_மதிப்பு = hash_string(text) % HASH_SIZE;

	/* If we easily find the string in the hash table, then return
	   that and be done. */
      எனில் (hash_table[hash_மதிப்பு]
	  && (strcmp(hash_table[hash_மதிப்பு], text) == 0)) {
	    திரும்பு hash_table[hash_மதிப்பு];
      }

	/* The existing hash entry is not a match. Replace it with the
	   newly allocated value, and return the new pointer as the
	   result to the add. */
      மாறிலி எழுத்து*res = vpip_string(text);
      hash_table[hash_மதிப்பு] = res;

      திரும்பு res;
}

PLI_INT32 vpi_chk_error(p_vpi_error_info info)
{
      எனில் (vpip_last_error.state == 0)
	    திரும்பு 0;

      info->state = vpip_last_error.state;
      info->level = vpip_last_error.level;
      info->message = vpip_last_error.message;
      info->product = vpi_vlog_info.product;
      info->code = (எழுத்து *)"";
      info->file = 0;
      info->வரி = 0;

      திரும்பு info->level;
}

PLI_INT32 vpi_compare_objects(vpiHandle obj1, vpiHandle obj2)
{
      ப்ரதிஜ்ஞா(obj1);
      ப்ரதிஜ்ஞா(obj2);

	// Does this actually work for all cases?
      எனில் (obj1 != obj2) திரும்பு 0;
      இல்லையெனில் திரும்பு 1;
}

/*
 * Copy the internal information to the data structure. Do not free or
 * change the tfname/user_data since they are a pointer to the real
 * string/data values. We also support passing a task or function handle
 * instead of just a handle to a vpiUserSystf.
 */
வெறுமை vpi_get_systf_info(vpiHandle ref, p_vpi_systf_data data)
{
      கட்டமைப்பு __vpiUserSystf* rfp = dynamic_cast<__vpiUserSystf*>(ref);
      எனில் (rfp == 0) {
	    கட்டமைப்பு __vpiSysTaskCall*call = dynamic_cast<__vpiSysTaskCall*>(ref);
	    ப்ரதிஜ்ஞா(call);
	    rfp = call->defn;
      }

	/* Assert that vpiUserDefn is true! */
      ப்ரதிஜ்ஞா(rfp->is_user_defn);

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
கட்டமைப்பு __vpiSysTaskCall*vpip_cur_task = 0;

PLI_INT32 vpi_free_object(vpiHandle ref)
{
      முழு rtn;

      எனில் (vpi_trace) {
	    fprintf(vpi_trace, "vpi_free_object(%p)", ref);
	    fflush(vpi_trace);
      }

      ப்ரதிஜ்ஞா(ref);
      __vpiHandle::free_object_fun_t fun = ref->free_object_fun();
      rtn = fun (ref);

      எனில் (vpi_trace)
	    fprintf(vpi_trace, " --> %d\n", rtn);

      திரும்பு rtn;
}

PLI_INT32 vpi_release_handle(vpiHandle ref)
{
	// Since SystemVerilog vpi_free_object() has been
	// renamed vpi_release_handle(), and thus
	// vpi_free_object() has been deprecated.
      திரும்பு vpi_free_object(ref);
}

நிலையான முழு vpip_get_global(முழு property)
{
      தேர்வு (property) {

	  நிலை vpiTimeUnit:
	  நிலை vpiTimePrecision:
	    திரும்பு vpip_get_time_precision();

	  இயல்பு:
	    fprintf(stderr, "vpi error: bad global property: %d\n", property);
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு vpiUndefined;
      }
}

நிலையான மாறிலி எழுத்து* vpi_property_str(PLI_INT32 code)
{
      நிலையான எழுத்து buf[32];
      தேர்வு (code) {
	  நிலை vpiConstType:
	    திரும்பு "vpiConstType";
	  நிலை vpiName:
	    திரும்பு "vpiName";
	  நிலை vpiFullName:
	    திரும்பு "vpiFullName";
	  நிலை vpiTimeUnit:
	    திரும்பு "vpiTimeUnit";
	  நிலை vpiTimePrecision:
	    திரும்பு "vpiTimePrecision";
          நிலை vpiSize:
	    திரும்பு "vpiSize";
          நிலை _vpiNexusId:
	    திரும்பு "_vpiNexusId";
	  இயல்பு:
	    snprintf(buf, அளவு(buf), "%d", (முழு)code);
      }
      திரும்பு buf;
}

மாறிலி எழுத்து* vpi_type_as_string(PLI_INT32 code)
{
      நிலையான எழுத்து buf[32];
      தேர்வு (code) {
	  நிலை vpiArrayType:
	    திரும்பு "vpiArrayType";
	  நிலை vpiBitVar:
	    திரும்பு "vpiBitVar";
	  நிலை vpiByteVar:
	    திரும்பு "vpiByteVar";
	  நிலை vpiClassVar:
	    திரும்பு "vpiClassVar";
	  நிலை vpiConstant:
	    திரும்பு "vpiConstant";
	  நிலை vpiEnumTypespec:
	    திரும்பு "vpiEnumTypespec";
	  நிலை vpiFunction:
	    திரும்பு "vpiFunction";
	  நிலை vpiGenScope:
	    திரும்பு "vpiGenScope";
	  நிலை vpiIntVar:
	    திரும்பு "vpiIntVar";
	  நிலை vpiIntegerVar:
	    திரும்பு "vpiIntegerVar";
	  நிலை vpiIterator:
	    திரும்பு "vpiIterator";
	  நிலை vpiLongIntVar:
	    திரும்பு "vpiLongIntVar";
	  நிலை vpiMemory:
	    திரும்பு "vpiMemory";
	  நிலை vpiMemoryWord:
	    திரும்பு "vpiMemoryWord";
	  நிலை vpiModule:
	    திரும்பு "vpiModule";
	  நிலை vpiNamedBegin:
	    திரும்பு "vpiNamedBegin";
	  நிலை vpiNamedEvent:
	    திரும்பு "vpiNamedEvent";
	  நிலை vpiNamedFork:
	    திரும்பு "vpiNamedFork";
	  நிலை vpiPackage:
	    திரும்பு "vpiPackage";
	  நிலை vpiPathTerm:
	    திரும்பு "vpiPathTerm";
	  நிலை vpiPort:
	    திரும்பு "vpiPort";
	  நிலை vpiNet:
	    திரும்பு "vpiNet";
	  நிலை vpiNetArray:
	    திரும்பு "vpiNetArray";
	  நிலை vpiNetBit:
	    திரும்பு "vpiNetBit";
	  நிலை vpiParameter:
	    திரும்பு "vpiParameter";
	  நிலை vpiPartSelect:
	    திரும்பு "vpiPartSelect";
	  நிலை vpiRealVar:
	    திரும்பு "vpiRealVar";
	  நிலை vpiReg:
	    திரும்பு "vpiReg";
	  நிலை vpiRegBit:
	    திரும்பு "vpiRegBit";
	  நிலை vpiShortIntVar:
	    திரும்பு "vpiShortIntVar";
	  நிலை vpiStringVar:
	    திரும்பு "vpiStringVar";
	  நிலை vpiSysFuncCall:
	    திரும்பு "vpiSysFuncCall";
	  நிலை vpiSysTaskCall:
	    திரும்பு "vpiSysTaskCall";
	  நிலை vpiTask:
	    திரும்பு "vpiTask";
	  நிலை vpiTimeVar:
	    திரும்பு "vpiTimeVar";
	  நிலை vpiUserSystf:
	    திரும்பு "vpiUserSystf";
	  இயல்பு:
	    snprintf(buf, அளவு(buf), "%d", (முழு)code);
      }
      திரும்பு buf;
}

PLI_INT32 vpi_get(முழு property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      எனில் (property == _vpiDelaySelection) திரும்பு vpip_delay_selection;

      எனில் (ref == 0)
	    திரும்பு vpip_get_global(property);

      எனில் (property == vpiType) {
	    எனில் (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

	    எனில் (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  திரும்பு vpiNetArray;
	    இல்லையெனில்
		  திரும்பு ref->get_type_code();
      }

      முழு res = ref->vpi_get(property);

      எனில் (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get(%s, %p) --> %d\n",
		    vpi_property_str(property), ref, res);
      }

      திரும்பு res;
}

PLI_INT64 vpi_get64(முழு property, vpiHandle ref)
{
	// For now we only support getting the nexus id.
      எனில் ((ref == 0) || (property != _vpiNexusId)) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) is not currently supported.\n",
		    vpi_property_str(property), ref);
	    திரும்பு vpiUndefined;
      }

      int64_t res = ref->vpi_get64(property);

      எனில் (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) --> %" PRId64 "\n",
		    vpi_property_str(property), ref, res);
      }

      திரும்பு res;
}

எழுத்து* vpi_get_str(PLI_INT32 property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      எனில் (property == _vpiDelaySelection) {
	    தேர்வு (vpip_delay_selection) {
		நிலை _vpiDelaySelMinimum:
		  திரும்பு simple_set_rbuf_str("MINIMUM");
		நிலை _vpiDelaySelTypical:
		  திரும்பு simple_set_rbuf_str("TYPICAL");
		நிலை _vpiDelaySelMaximum:
		  திரும்பு simple_set_rbuf_str("MAXIMUM");
		இயல்பு:
		  ப்ரதிஜ்ஞா(0);
	    }
      }

      எனில் (ref == 0) {
	    fprintf(stderr, "vpi error: vpi_get_str(%s, 0) called "
		    "with null vpiHandle.\n", vpi_property_str(property));
	    திரும்பு 0;
      }

      எனில் (property == vpiType) {
	    எனில் (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

            PLI_INT32 type;
	    எனில் (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  type = vpiNetArray;
	    இல்லையெனில்
		  type = ref->get_type_code();
	    திரும்பு (எழுத்து *)vpi_type_as_string(type);
      }

      எழுத்து*res = ref->vpi_get_str(property);

      எனில் (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get_str(%s, %p) --> %s\n",
		    vpi_property_str(property), ref, res? res : "<NULL>");
      }

      திரும்பு res;
}

__vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj)
{
      மாறிலி கட்டமைப்பு __vpiSysTaskCall*task;
      கட்டமைப்பு __vpiSignal*சமிக்ஞை;
      கட்டமைப்பு __vpiRealVar*real;
      மாறிலி __vpiNamedEvent*event;

      தேர்வு (obj->get_type_code()) {
	  நிலை vpiSysTaskCall:
	    task = dynamic_cast<__vpiSysTaskCall*>(obj);
	    திரும்பு task->scope;

	  நிலை vpiModule:
	    திரும்பு dynamic_cast<__vpiScope*>(obj);

	  நிலை vpiNet:
	  நிலை vpiReg:
	  நிலை vpiIntegerVar:
	  நிலை vpiBitVar:
	  நிலை vpiByteVar:
	  நிலை vpiShortIntVar:
	  நிலை vpiIntVar:
	  நிலை vpiLongIntVar:
	    சமிக்ஞை = dynamic_cast<__vpiSignal*>(obj);
	    திரும்பு vpip_scope(சமிக்ஞை);

	  நிலை vpiRealVar:
	    real = dynamic_cast<__vpiRealVar*>(obj);
	    திரும்பு vpip_scope(real);

	  நிலை vpiNamedEvent:
	    event = dynamic_cast<__vpiNamedEvent*>(obj);
	    திரும்பு event->get_scope();

	  நிலை vpiMemory:
	  நிலை vpiMemoryWord:
	  நிலை vpiPartSelect:
	    திரும்பு dynamic_cast<__vpiScope*>(obj->vpi_handle(vpiScope));

	  இயல்பு:
	    fprintf(stderr, "ERROR: vpip_scope_from_handle called with "
		    "object handle type=%d\n", obj->get_type_code());
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு 0;
      }
}

முழு vpip_time_units_from_handle(vpiHandle obj)
{
      எனில் (obj == 0)
	    திரும்பு vpip_get_time_precision();

      மாறிலி __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      எனில் (scope == 0)
	    திரும்பு vpip_get_time_precision();

      திரும்பு scope->time_units;
}

முழு vpip_time_precision_from_handle(vpiHandle obj)
{
      எனில் (obj == 0)
	    திரும்பு vpip_get_time_precision();

      மாறிலி __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      எனில் (scope == 0)
	    திரும்பு vpip_get_time_precision();

      திரும்பு scope->time_precision;
}

இரட்டை vpip_scaled_time_from_handle(vvp_time64_t time, vpiHandle obj)
{
      முழு scale = vpip_get_time_precision() -
	          vpip_time_units_from_handle(obj);
      எனில் (scale >= 0)
	    திரும்பு (இரட்டை)time * pow(10.0, scale);
      இல்லையெனில்
	    திரும்பு (இரட்டை)time / pow(10.0, -scale);
}

வெறுமை vpi_get_time(vpiHandle obj, s_vpi_time*vp)
{
      vvp_time64_t time;

      ப்ரதிஜ்ஞா(vp);

      time = schedule_simtime();

      தேர்வு (vp->type) {
          நிலை vpiSimTime:
	    vp->உயர்வு = (time >> 32) & 0xffffffff;
	    vp->கீழ் = time & 0xffffffff;
	    நிறுத்து;

          நிலை vpiScaledRealTime:
	    vp->real = vpip_scaled_time_from_handle(time, obj);
	    நிறுத்து;

          இயல்பு:
            fprintf(stderr, "vpi_get_time: unknown type: %d\n", (முழு)vp->type);
            ப்ரதிஜ்ஞா(0);
	    நிறுத்து;
      }
}

PLI_INT32 vpi_get_vlog_info(p_vpi_vlog_info vlog_info_p)
{
    எனில் (vlog_info_p != 0) {
	  *vlog_info_p = vpi_vlog_info;
	  திரும்பு 1;
    } இல்லையெனில் {
	  திரும்பு 0;
    }
}

வெறுமை vpi_set_vlog_info(முழு argc, எழுத்து** argv)
{
    நிலையான எழுத்து icarus_product[] = "Icarus Verilog";
    நிலையான எழுத்து icarus_version[] = VERSION;
    vpi_vlog_info.product = icarus_product;
    vpi_vlog_info.version = icarus_version;
    vpi_vlog_info.argc    = argc;
    vpi_vlog_info.argv    = argv;

    நிலையான எழுத்து trace_buf[1024];
    எனில் (மாறிலி எழுத்து*path = getenv("VPI_TRACE"))  {
	  எனில் (!strcmp(path,"-"))
		vpi_trace = stdout;
	  இல்லையெனில் {
		vpi_trace = fopen(path, "w");
		எனில் (!vpi_trace) {
		      perror(path);
		      exit(1);
		}
		setvbuf(vpi_trace, trace_buf, _IOLBF, அளவு(trace_buf));
	  }
    }
}

நிலையான வெறுமை vec4_get_மதிப்பு_string(மாறிலி vvp_vector4_t&word_val, unsigned width,
				  s_vpi_மதிப்பு*vp)
{
      unsigned nchar = width / 8;
      unsigned tail = width % 8;

      எழுத்து*rbuf = static_cast<எழுத்து *>(need_result_buf(nchar + 1, RBUF_VAL));
      எழுத்து*cp = rbuf;

      எனில் (tail > 0) {
	    எழுத்து char_val = 0;
	    க்கு (unsigned idx = width-tail; idx < width ;  idx += 1) {
		  vvp_bit4_t val = word_val.value(idx);
		  எனில் (val == BIT4_1)
			char_val |= 1 << idx;
	    }

	    எனில் (char_val != 0)
		  *cp++ = char_val;
	    இல்லையெனில் எனில் (cp != rbuf)
		  *cp++ = ' ';
      }

      க்கு (unsigned idx = 0 ;  idx < nchar ;  idx += 1) {
	    unsigned bit = (nchar - idx - 1) * 8;
	    எழுத்து char_val = 0;
	    க்கு (unsigned bdx = 0 ;  bdx < 8 ;  bdx += 1) {
		  vvp_bit4_t val = word_val.value(bit+bdx);
		  எனில் (val == BIT4_1)
			char_val |= 1 << bdx;
	    }
	    // Ignore leading null-bytes and replace other null-bytes with space.
	    // The LRM is not entirely clear on how null bytes should be handled.
	    // This is the implementation chosen for iverilog.
	    எனில் (char_val != 0)
		  *cp++ = char_val;
	    இல்லையெனில் எனில் (cp != rbuf)
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
வெறுமை vpip_vec4_get_மதிப்பு(மாறிலி vvp_vector4_t&word_val, unsigned width,
			 தருக்க signed_flag, s_vpi_மதிப்பு*vp)
{
      எழுத்து *rbuf = 0;

      தேர்வு (vp->format) {
	  இயல்பு:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector values.\n", (முழு)vp->format);
	    ப்ரதிஜ்ஞா(0);

	  நிலை vpiSuppressVal:
	    நிறுத்து;

	  நிலை vpiBinStrVal:
	    rbuf = static_cast<எழுத்து *>(need_result_buf(width+1, RBUF_VAL));
	    க்கு (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		  vvp_bit4_t bit = word_val.value(idx);
		  rbuf[width-idx-1] = vvp_bit4_to_ascii(bit);
	    }
	    rbuf[width] = 0;
	    vp->value.str = rbuf;
	    நிறுத்து;

	  நிலை vpiOctStrVal: {
		unsigned hwid = ((width+2) / 3) + 1;
		rbuf = static_cast<எழுத்து *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_oct_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		நிறுத்து;
	  }

	  நிலை vpiDecStrVal: {
// HERE need a better estimate.
		rbuf = static_cast<எழுத்து *>(need_result_buf(width+1, RBUF_VAL));
		vpip_vec4_to_dec_str(word_val, rbuf, width+1, signed_flag);
		vp->value.str = rbuf;
		நிறுத்து;
	  }

	  நிலை vpiHexStrVal: {
		unsigned  hwid = ((width + 3) / 4) + 1;
		rbuf = static_cast<எழுத்து *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_hex_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		நிறுத்து;
	  }

          நிலை vpiScalarVal: {
	        // scalars should be of size 1
	        ப்ரதிஜ்ஞா(width == 1);
	        தேர்வு(word_val.value(0)) {
		    நிலை BIT4_0:
		      vp->value.scalar = vpi0;
		      நிறுத்து;
	            நிலை BIT4_1:
                      vp->value.scalar = vpi1;
                      நிறுத்து;
	            நிலை BIT4_X:
                      vp->value.scalar = vpiX;
                      நிறுத்து;
	            நிலை BIT4_Z:
                      vp->value.scalar = vpiZ;
                      நிறுத்து;
		}
                நிறுத்து;
          }

	  நிலை vpiIntVal: {
		long val = 0;
		vvp_bit4_t pad = BIT4_0;
		எனில் (signed_flag && word_val.size() > 0)
		      pad = word_val.value(word_val.size()-1);

		க்கு (unsigned idx = 0 ; idx < 8*அளவு(val) ;  idx += 1) {
		      vvp_bit4_t val4 = pad;
		      எனில் (idx < word_val.size())
			    val4 = word_val.value(idx);
		      எனில் (val4 == BIT4_1)
			    val |= 1L << idx;
		}

		vp->value.integer = val;
		நிறுத்து;
	  }

	  நிலை vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiVectorVal;
	    // fallthrough
	  நிலை vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * அளவு(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		க்கு (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      தேர்வு (word_val.value(idx)) {
			  நிலை BIT4_0:
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    நிறுத்து;
			  நிலை BIT4_1:
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    நிறுத்து;
			  நிலை BIT4_X:
			    op->aval |= (1 << idx % 32);
			    op->bval |= (1 << idx % 32);
			    நிறுத்து;
			  நிலை BIT4_Z:
			    op->aval &= ~(1 << idx % 32);
			    op->bval |=  (1 << idx % 32);
			    நிறுத்து;
		      }
		      எனில் (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		நிறுத்து;
	  }

	  நிலை vpiStringVal:
		vec4_get_மதிப்பு_string(word_val, width, vp);
		நிறுத்து;

	  நிலை vpiRealVal:
		vector4_to_மதிப்பு(word_val, vp->value.real, signed_flag);
		நிறுத்து;
      }
}

வெறுமை vpip_vec2_get_மதிப்பு(மாறிலி vvp_vector2_t&word_val, unsigned width,
			 தருக்க signed_flag, s_vpi_மதிப்பு*vp)
{
      தேர்வு (vp->format) {
	  இயல்பு:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector2 values.\n", (முழு)vp->format);
	    ப்ரதிஜ்ஞா(0);

	  நிலை vpiSuppressVal:
	    நிறுத்து;

	  நிலை vpiIntVal:
	    vector2_to_மதிப்பு(word_val, vp->value.integer, signed_flag);
	    நிறுத்து;

	  நிலை vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  நிலை vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * அளவு(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		க்கு (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      எனில் (word_val.value(idx)) {
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      } இல்லையெனில் {
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      }
		      எனில் (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		நிறுத்து;
	  }
      }

}

/*
 * Convert a real value to the appropriate integer.
 */
நிலையான PLI_INT32 get_real_as_int(இரட்டை real)
{
      இரட்டை rtn;

	/* We would normally want to return 'bx for a NaN or
	 * +/- infinity, but for an integer the standard says
	 * to convert 'bx to 0, so we just return 0. */
      எனில் (real != real || (real && (real == 0.5*real))) {
	    திரும்பு 0;
      }

      rtn = std::round(real);

      திரும்பு (PLI_INT32) rtn;
}

/*
 * This is a generic function to convert a double value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer.
 */
வெறுமை vpip_real_get_மதிப்பு(இரட்டை real, s_vpi_மதிப்பு*vp)
{
      எழுத்து *rbuf = 0;

      தேர்வு (vp->format) {
	  இயல்பு:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting real values.\n", (முழு)vp->format);
	    ப்ரதிஜ்ஞா(0);

	  நிலை vpiSuppressVal:
	    நிறுத்து;

	  நிலை vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiRealVal;
	    // fallthrough
	  நிலை vpiRealVal:
	    vp->value.real = real;
	    நிறுத்து;

	  நிலை vpiIntVal:
	    vp->value.integer = get_real_as_int(real);
	    நிறுத்து;

	  நிலை vpiDecStrVal:
	    rbuf = static_cast<எழுத்து *>(need_result_buf(1025, RBUF_VAL));
	    vpip_vec4_to_dec_str(vvp_vector4_t(1024, real), rbuf, 1025, உண்மை);
	    vp->value.str = rbuf;
	    நிறுத்து;
      }
}

இரட்டை real_from_vpi_மதிப்பு(s_vpi_மதிப்பு*vp)
{
      vvp_vector4_t vec4(1024);
      இரட்டை result;
      தருக்க is_signed = பொய்;

      தேர்வு (vp->format) {
	  இயல்பு:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "putting real values.\n", (முழு)vp->format);
	    ப்ரதிஜ்ஞா(0);

	  நிலை vpiRealVal:
	    result = vp->value.real;
	    நிறுத்து;

	  நிலை vpiIntVal:
	    result = (இரட்டை) vp->value.integer;
	    நிறுத்து;

	  நிலை vpiBinStrVal:
	    vpip_bin_str_to_vec4(vec4, vp->value.str);
	    எனில் (vp->value.str[0] == '-') is_signed = உண்மை;
	    vector4_to_மதிப்பு(vec4, result, is_signed);
	    நிறுத்து;

	  நிலை vpiOctStrVal:
	    vpip_oct_str_to_vec4(vec4, vp->value.str);
	    எனில் (vp->value.str[0] == '-') is_signed = உண்மை;
	    vector4_to_மதிப்பு(vec4, result, is_signed);
	    நிறுத்து;

	  நிலை vpiDecStrVal:
	    vpip_dec_str_to_vec4(vec4, vp->value.str);
	    எனில் (vp->value.str[0] == '-') is_signed = உண்மை;
	    vector4_to_மதிப்பு(vec4, result, is_signed);
	    நிறுத்து;

	  நிலை vpiHexStrVal:
	    vpip_hex_str_to_vec4(vec4, vp->value.str);
	    எனில் (vp->value.str[0] == '-') is_signed = உண்மை;
	    vector4_to_மதிப்பு(vec4, result, is_signed);
	    நிறுத்து;

      }

      திரும்பு result;
}

வெறுமை vpip_string_get_மதிப்பு(மாறிலி string&val, s_vpi_மதிப்பு*vp)
{
      எழுத்து *rbuf = 0;

      தேர்வு (vp->format) {
	  இயல்பு:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting string values.\n", (முழு)vp->format);
	    ப்ரதிஜ்ஞா(0);

	  நிலை vpiSuppressVal:
	    நிறுத்து;

	  நிலை vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiStringVal;
	    // fallthrough
	  நிலை vpiStringVal:
	    rbuf = static_cast<எழுத்து *>(need_result_buf(val.size() + 1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    நிறுத்து;
      }
}


வெறுமை vpi_get_மதிப்பு(vpiHandle கோவை, s_vpi_மதிப்பு*vp)
{
      ப்ரதிஜ்ஞா(கோவை);
      ப்ரதிஜ்ஞா(vp);

	// Never bother with suppressed values. All the derived
	// classes can ignore this type.
      எனில் (vp->format == vpiSuppressVal)
	    திரும்பு;

      கோவை->vpi_get_மதிப்பு(vp);

      எனில் (vpi_trace) தேர்வு (vp->format) {
		நிலை vpiStringVal:
		  fprintf(vpi_trace,"vpi_get_value(%p=<%d>) -> string=\"%s\"\n",
			  கோவை, கோவை->get_type_code(), vp->value.str);
		  நிறுத்து;

		நிலை vpiBinStrVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> binstr=%s\n",
			  கோவை->get_type_code(), vp->value.str);
		  நிறுத்து;

		நிலை vpiIntVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> int=%d\n",
			  கோவை->get_type_code(), (முழு)vp->value.integer);
		  நிறுத்து;

		நிலை vpiSuppressVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <suppress>\n",
			  கோவை->get_type_code());
		  நிறுத்து;

		இயல்பு:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <%d>=?\n",
			  கோவை->get_type_code(), (முழு)vp->format);
	    }
}

கட்டமைப்பு vpip_put_மதிப்பு_event : vvp_gen_event_s {
      vpiHandle handle;
      s_vpi_மதிப்பு value;
      முழு flags;
      மெய்நிகர் வெறுமை run_run() override;
      ~vpip_put_மதிப்பு_event() override { }
};

வெறுமை vpip_put_மதிப்பு_event::run_run()
{
      handle->vpi_put_மதிப்பு(&value, flags);
      தேர்வு (value.format) {
	    /* Free the copied string. */
	  நிலை vpiBinStrVal:
	  நிலை vpiOctStrVal:
	  நிலை vpiDecStrVal:
	  நிலை vpiHexStrVal:
	  நிலை vpiStringVal:
	    விடுவி(value.value.str);
	    நிறுத்து;
	    /* Free the copied time structure. */
	  நிலை vpiTimeVal:
	    விடுவி(value.value.time);
	    நிறுத்து;
	    /* Free the copied vector structure. */
	  நிலை vpiVectorVal:
	    விடுவி(value.value.vector);
	    நிறுத்து;
	    /* Free the copied strength structure. */
	  நிலை vpiStrengthVal:
	    விடுவி(value.value.strength);
	    நிறுத்து;
	    /* Everything else is static in the structure. */
	  இயல்பு:
	    நிறுத்து;
      }
}

/* Make a copy of a pointer to a time structure. */
நிலையான t_vpi_time *timedup(மாறிலி t_vpi_time *val)
{
      t_vpi_time *rtn;
      rtn = static_cast<t_vpi_time *> (malloc(அளவு(t_vpi_time)));
      *rtn = *val;
      திரும்பு rtn;
}

/* Make a copy of a pointer to a vector value structure. */
நிலையான t_vpi_vecval *vectordup(மாறிலி t_vpi_vecval *val, PLI_INT32 size)
{
      unsigned இலக்கம்_bytes;
      t_vpi_vecval *rtn;
      ப்ரதிஜ்ஞா(size > 0);
      இலக்கம்_bytes = ((size + 31)/32)*அளவு(t_vpi_vecval);
      rtn = static_cast<t_vpi_vecval *> (malloc(இலக்கம்_bytes));
      memcpy(rtn, val, இலக்கம்_bytes);
      திரும்பு rtn;
}

/* Make a copy of a pointer to a strength structure. */
நிலையான t_vpi_strengthval *strengthdup(மாறிலி t_vpi_strengthval *val)
{
      t_vpi_strengthval *rtn;
      rtn = static_cast<t_vpi_strengthval *>
            (malloc(அளவு(t_vpi_strengthval)));
      *rtn = *val;
      திரும்பு rtn;
}

vpiHandle vpi_put_மதிப்பு(vpiHandle obj, s_vpi_மதிப்பு*vp,
			s_vpi_time*when, PLI_INT32 flags)
{
      ப்ரதிஜ்ஞா(obj);

      flags &= ~vpiReturnEvent;

      எனில் (flags!=vpiNoDelay && flags!=vpiForceFlag && flags!=vpiReleaseFlag) {
	    vvp_time64_t dly;
	    முழு scale;

	    எனில் (vpi_get(vpiAutomatic, obj)) {
		  fprintf(stderr, "VPI error: cannot put a value with "
				  "a delay on automatically allocated "
				  "variable '%s'.\n",
				  vpi_get_str(vpiName, obj));
		  திரும்பு 0;
	    }

	    ப்ரதிஜ்ஞா(when != 0);

	    தேர்வு (when->type) {
		நிலை vpiScaledRealTime:
		  scale = vpip_time_units_from_handle(obj) -
		          vpip_get_time_precision();
		  எனில் (scale >= 0) {
			dly = (vvp_time64_t)(when->real * pow(10.0, scale));
		  } இல்லையெனில் {
			dly = (vvp_time64_t)(when->real / pow(10.0, -scale));
		  }
		  நிறுத்து;
		நிலை vpiSimTime:
		  dly = vpip_timestruct_to_time(when);
		  நிறுத்து;
		இயல்பு:
		  dly = 0;
		  நிறுத்து;
	    }

	    எனில் ((dly == 0) && schedule_at_rosync()) {
		  fprintf(stderr, "VPI error: attempted to put a value to "
				  "variable '%s' during a read-only synch "
				  "callback.\n", vpi_get_str(vpiName, obj));
		  திரும்பு 0;
	    }

	    vpip_put_மதிப்பு_event*put = புதிய vpip_put_மதிப்பு_event;
	    put->handle = obj;
	    எனில் (dynamic_cast<__vpiNamedEvent*>(obj)) {
		  put->value.format = vpiIntVal;
		  put->value.value.integer = 0;
	    } இல்லையெனில் {
		  ப்ரதிஜ்ஞா(vp);
		  put->value = *vp;
	    }
	      /* Since this is a scheduled put event we must copy any pointer
	       * data to keep it available until the event is actually run. */
	    தேர்வு (put->value.format) {
		  /* Copy the string items. */
		நிலை vpiBinStrVal:
		நிலை vpiOctStrVal:
		நிலை vpiDecStrVal:
		நிலை vpiHexStrVal:
		நிலை vpiStringVal:
		  put->value.value.str = strdup(put->value.value.str);
		  நிறுத்து;
		  /* Copy a time pointer item. */
		நிலை vpiTimeVal:
		  put->value.value.time = timedup(put->value.value.time);
		  நிறுத்து;
		  /* Copy a vector pointer item. */
		நிலை vpiVectorVal:
		  put->value.value.vector = vectordup(put->value.value.vector,
		                                      vpi_get(vpiSize, obj));
		  நிறுத்து;
		  /* Copy a strength pointer item. */
		நிலை vpiStrengthVal:
		  put->value.value.strength =
		      strengthdup(put->value.value.strength);
		  நிறுத்து;
		  /* Everything thing else is already in the structure. */
		இயல்பு:
		  நிறுத்து;
	    }
	    put->flags = flags;
	    schedule_generic(put, dly, பொய், உண்மை, உண்மை);
	    திரும்பு 0;
      }

      எனில் (schedule_at_rosync()) {
            fprintf(stderr, "VPI error: attempted to put a value to "
			    "variable '%s' during a read-only synch "
			    "callback.\n", vpi_get_str(vpiName, obj));
            திரும்பு 0;
      }

      obj->vpi_put_மதிப்பு(vp, flags);

      திரும்பு 0;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle res = 0;

      எனில் (ref == 0) {
	      // A few types can apply to a nil handle. These are ways
	      // that the current function can get started finding things.
	    தேர்வு (type) {

		நிலை vpiScope:
		    // The IEEE1364-2005 doesn't seem to allow this,
		    // but some users seem to think it's handy, so
		    // return the scope that contains this SysTfCall.
		  ப்ரதிஜ்ஞா(vpip_cur_task);
		  res = vpip_cur_task->vpi_handle(vpiScope);
		  நிறுத்து;

		நிலை vpiSysTfCall:
		    // This is how VPI users get a first handle into
		    // the system. This is the handle of the system
		    // task/function call currently being executed.
		  எனில் (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle(vpiSysTfCall, 0) "
				"-> %p (%s)\n", vpip_cur_task,
				vpip_cur_task->defn->info.tfname);
		  }
		  திரும்பு vpip_cur_task;

		இயல்பு:
		  fprintf(stderr, "VPI error: vpi_handle(type=%d, ref=0).\n",
			  (முழு)type);
		  res = 0;
		  நிறுத்து;
	    }

      } இல்லையெனில் {

	    எனில் (type == vpiSysTfCall) {
		  fprintf(stderr, "VPI error: vpi_handle(vpiSysTfCall, "
			  "ref!=0).\n");
		  திரும்பு 0;
	    }

	    res = ref->vpi_handle(type);
      }


      எனில் (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle(vpiScope, ref=%p) "
		    "-> %p\n", vpip_cur_task, ref);
      }

      திரும்பு res;
}

நிலையான vpiHandle vpip_make_udp_iterator()
{
// HERE: Add support for iterating over UDP definitions.
//       See 26.6.16 (page 400 in 1364-2005).
      திரும்பு 0;
}

/*
 * This function asks the object to return an iterator for
 * the specified reference. It is up to the iterate_ method to
 * allocate a properly formed iterator.
 */
நிலையான vpiHandle vpi_iterate_global(முழு type)
{
      தேர்வு (type) {
	  நிலை vpiInstance:
	    // fallthrough
	  நிலை vpiModule:
	    // fallthrough
	  நிலை vpiProgram:
	    // fallthrough
	  நிலை vpiInterface:
	    // fallthrough
	  நிலை vpiPackage:
	    திரும்பு vpip_make_வேர்_iterator(type);

	  நிலை vpiUdpDefn:
	    திரும்பு vpip_make_udp_iterator();

	  நிலை vpiUserSystf:
	    திரும்பு vpip_make_systf_iterator();
      }

      திரும்பு 0;
}

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle rtn = 0;

      ப்ரதிஜ்ஞா(vpi_mode_flag != VPI_MODE_NONE);
      எனில் (vpi_mode_flag == VPI_MODE_REGISTER) {
	    fprintf(stderr, "vpi error: vpi_iterate called during "
		    "vpi_register_systf. You can't do that!\n");
	    திரும்பு 0;
      }

      எனில் (ref == 0)
	    rtn = vpi_iterate_global(type);
      இல்லையெனில்
	    rtn = ref->vpi_iterate(type);

      எனில் (vpi_trace) {
	    fprintf(vpi_trace, "vpi_iterate(%d, %p) ->%s\n",
	    (முழு)type, ref, rtn ? "" : " (null)");
      }

      திரும்பு rtn;
}

vpiHandle vpi_handle_by_index(vpiHandle ref, PLI_INT32 idx)
{
      ப்ரதிஜ்ஞா(ref);
      திரும்பு ref->vpi_index(idx);
}

நிலையான vpiHandle find_name(மாறிலி எழுத்து *name, vpiHandle handle)
{
      vpiHandle rtn = 0;
      __vpiScope*ref = dynamic_cast<__vpiScope*>(handle);

      /* check module names */
      எனில் (!strcmp(name, vpi_get_str(vpiName, handle)))
	    rtn = handle;

      /* brute force search for the name in all objects in this scope */
      க்கு (unsigned i = 0 ;  i < ref->intern.size() ;  i += 1) {
	      /* The standard says that since a port does not have a full
	       * name it cannot be found by name. Because of this we need
	       * to skip ports here so the correct handle can be located. */
	    எனில் (vpi_get(vpiType, ref->intern[i]) == vpiPort) தொடர்;
	    மாறிலி எழுத்து *nm = vpi_get_str(vpiName, ref->intern[i]);
	    எனில் (nm && !strcmp(name, nm)) {
		  rtn = ref->intern[i];
		  நிறுத்து;
	    } இல்லையெனில் எனில் (vpi_get(vpiType, ref->intern[i]) == vpiMemory ||
	               vpi_get(vpiType, ref->intern[i]) == vpiNetArray) {
		  /* We need to iterate on the words */
		  vpiHandle word_i, word_h;
		  word_i = vpi_iterate(vpiMemoryWord, ref->intern[i]);
		  போது (word_i && (word_h = vpi_scan(word_i))) {
			nm = vpi_get_str(vpiName, word_h);
			எனில் (nm && !strcmp(name, nm)) {
			      rtn = word_h;
			      vpi_free_object(word_i);
			      நிறுத்து;
			}
		  }
	    }
	    /* found it yet? */
	    எனில் (rtn) நிறுத்து;
      }

      திரும்பு rtn;
}

// Find the end of the escaped identifier or simple identifier
நிலையான எழுத்து * find_rest(எழுத்து *name)
{
      எழுத்து *rest;
      எனில் (*name == '\\') {
	    rest = strchr(name, ' ');
	    எனில் (rest) {
		  *rest++ = 0; // The space is not part of the string
		    // There should be a '.' after the escaped ID if there is
		    // anything more. If it is missing add it to avoid a crash
		  எனில் ((*rest != '.') && (*rest != 0)) {
			*--rest = '.';
			fprintf(stderr, "ERROR: Malformed scope string: \"%s\"", name);
		  }
		  எனில் (*rest == 0) {
			rest = 0;
		  }
	    }
      } இல்லையெனில் {
	    rest = strchr(name, '.');
      }
      திரும்பு rest;
}

நிலையான vpiHandle find_scope(மாறிலி எழுத்து *name, vpiHandle handle, முழு depth)
{

      vpiHandle iter = handle==0
	    ? vpi_iterate(vpiModule, வெற்று)
	    : vpi_iterate(vpiInternalScope, handle);

      vector<எழுத்து> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      எழுத்து*nm_first = &name_buf[0];
      எழுத்து*nm_rest = find_rest(nm_first);
      எனில் (*nm_first == '\\') ++nm_first;
      எனில் (nm_rest) {
	    *nm_rest++ = 0;
      }

      vpiHandle rtn = 0;
      vpiHandle hand;
      போது (iter && (hand = vpi_scan(iter))) {
	    மாறிலி எழுத்து *nm = vpi_get_str(vpiName, hand);

	    எனில் (strcmp(nm_first,nm)==0) {
		  எனில் (nm_rest)
			rtn=find_scope(nm_rest, hand, depth+1);
		  இல்லையெனில்
			rtn = hand;
	    }

	    /* found it yet ? */
	    எனில் (rtn) {
		  vpi_free_object(iter);
		  நிறுத்து;
	    }
      }

      திரும்பு rtn;
}

// Find the end of the first escaped identifier or simple identifier
நிலையான எழுத்து * find_next(எழுத்து *name)
{
      எழுத்து *next;
      எனில் (*name == '\\') {
	    next = strchr(name, ' ');
	    எனில் (next && *++next == 0) next = 0;
      } இல்லையெனில் {
	    next = strchr(name, '.');
      }
      திரும்பு next;
}

vpiHandle vpi_handle_by_name(மாறிலி எழுத்து *name, vpiHandle scope)
{
      vpiHandle hand;

      எனில் (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name(%s, %p) -->\n",
		    name, scope);
      }

	// Chop the name into path and base. For example, if the name
	// is "a.b.c", then nm_path becomes "a.b" and nm_base becomes
	// "c". If the name is "c" then nm_path is nil and nm_base is "c".
      vector<எழுத்து> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      எழுத்து*nm_path = &name_buf[0];
      எழுத்து*nm_base;

	// Check to see if we have an escaped identifier and if so search
	// the long way because a '.' could be in the escaped name.
      எனில் (strchr(nm_path, '\\')) {
	    எழுத்து *next;
	    nm_base = nm_path;
	    போது ((next = find_next(nm_base))) {
		  nm_base = ++next;
	    }
	    எனில் (nm_path == nm_base) {
		  nm_path = 0;
	    } இல்லையெனில் {
		  *(nm_base-1) = 0;
	    }

	// If there is no escaped identifier then just look for the last '.'
      } இல்லையெனில் {
	    nm_base = strrchr(nm_path, '.');
	    எனில் (nm_base) {
		  *nm_base++ = 0;
	    } இல்லையெனில் {
		  nm_base = nm_path;
		  nm_path = 0;
	    }
      }

      /* If scope provided, look in corresponding module; otherwise
       * traverse the hierarchy specified in name to find the leaf module
       * and try finding it there.
       */
      எனில் (scope) {
	    /* Some implementations support either a module or a scope. */
	    தேர்வு (vpi_get(vpiType, scope)) {
		நிலை vpiScope:
	          hand = vpi_handle(vpiModule, scope);
	          நிறுத்து;
		நிலை vpiModule:
	          hand = scope;
	          நிறுத்து;
		இயல்பு:
		  எனில் (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle_by_name: "
				"Scope is not a vpiScope or vpiModule\n");
		  }
	          // Use vpi_chk_error() here when it is implemented.
	          திரும்பு 0;
	    }

      } இல்லையெனில் எனில் (nm_path) {
	      // The name has a path, and no other scope handle was
	      // passed in. That suggests we are looking for "a.b.c"
	      // in the root scope. So convert "a.b" to a scope and
	      // start there to look for "c".
	    hand = find_scope(nm_path, வெற்று, 0);
	    nm_path = 0;

      } இல்லையெனில் {
	      // Special case: scope==<nil>, meaning we are looking in
	      // the root, and there is no path to the name, i.e. the
	      // string is "c" instead of "top.c". Try to find "c" as
	      // a scope and return that.
	    hand = find_scope(nm_base, வெற்று, 0);
      }

      எனில் (hand == 0) {
	    எனில் (vpi_trace) {
		  fprintf(vpi_trace, "vpi_handle_by_name: "
			  "Scope does not exist. Giving up.\n");
	    }

	    திரும்பு 0;
      }

	// If there is a path part, then use it to find the
	// scope. For example, if the full name is a.b.c, then
	// the nm_path string is a.b and we search for that
	// scope. If we find it, then set hand to that scope.
      எனில் (nm_path) {
	    vpiHandle tmp = find_scope(nm_path, hand, 0);
	    போது (tmp == 0 && hand != 0) {
		  hand = vpi_handle(vpiScope, hand);
		  tmp = find_scope(nm_path, hand, 0);
	    }
	    hand = tmp;
      }

	// find_name() expects escaped identifiers to be stripped
      எனில் (*nm_base == '\\') {
	    // Skip the \ at the beginning
	    ++nm_base;

	    // Drop the space at the end if it exists
	    எழுத்து *next;
	    எனில் ((next = strchr(nm_base, ' '))) {
		  *next = 0;
	    }
      }

	// Now we have the correct scope, look for the item.
      vpiHandle out = find_name(nm_base, hand);

      எனில் (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name: DONE\n");
      }

      திரும்பு out;
}

// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected
// x2: Ptr to previous node
// x3: True when net is connected to output of previous node
tuple<தருக்க, vvp_net_ptr_t, தருக்க> check_connected_to_concat8_and_part_sa(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent) {

      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      போது (cur.ptr()) {
	    எனில் (cur.ptr() == net2 && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
		    // Pass on the return value if found
		  திரும்பு {உண்மை, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
	    } இல்லையெனில் எனில் (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8_and_part_sa(cur.ptr()->out_, net2, cur);
		  எனில் (get<0>(res)) {
			திரும்பு res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      திரும்பு {பொய், vvp_net_ptr_t(nullptr, 0), பொய்};
}

// Check if net2 is connected to current_net through a net of vvp_fun_concat8s
// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected with concats and part_sas
// x2: Ptr to previous node
// x3: True when net is connected to output of x2
tuple<தருக்க, vvp_net_ptr_t, தருக்க> check_connected_to_concat8(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent)
{
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);
	// For everything connected
      போது (cur.ptr()) {
	      // Check if it's a concat8
	    எனில் (cur.ptr() == net2) {
		  திரும்பு {உண்மை, vvp_net_ptr_t(nullptr, 0), பொய்};
	    } இல்லையெனில் எனில் (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8(cur.ptr()->out_, net2, cur);
		  எனில் (get<0>(res)) {
			எனில் (!get<1>(res).ptr() && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
			      // `parent` is the last part_sa node in front of our destination
			      // We want to place the intermod node here
			      திரும்பு {உண்மை, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
			}
			திரும்பு res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      திரும்பு {பொய், vvp_net_ptr_t(nullptr, 0), பொய்};
}

// Used to get intermodpath for two ports
vpiHandle vpi_handle_multi(PLI_INT32 type,
                           vpiHandle ref1,
                           vpiHandle ref2)
{
      எனில் (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_multi(%d, %p, %p) -->\n",
	            type, ref1, ref2);
      }

      எனில் (type != vpiInterModPath) {
	    fprintf(stderr, "sorry: vpi_handle_multi currently supports"
	            "only vpiInterModPath\n");
	    திரும்பு nullptr;
      }

	// Indicates whether port1 refers to a single bit
      தருக்க port1_has_index = பொய்;
      முழு port1_bit_index = 0;
      vpiPortBitInfo* port1_bit = dynamic_cast<vpiPortBitInfo*>(ref1);

      எனில் (port1_bit) {
	      // Get the bit index
	    port1_has_index = உண்மை;
	    port1_bit_index = vpi_get(vpiBit, port1_bit);
	      // Update the ref1 to point to the base port
	    ref1 = vpi_handle(vpiParent, port1_bit);
      }

	// Indicates whether port2 refers to a single bit
      தருக்க port2_has_index = பொய்;
      முழு port2_bit_index = 0;
      vpiPortBitInfo* port2_bit = dynamic_cast<vpiPortBitInfo*>(ref2);

      எனில் (port2_bit) {
	      // Get the bit index
	    port2_has_index = உண்மை;
	    port2_bit_index = vpi_get(vpiBit, port2_bit);
	      // Update the ref1 to point to the base port
	    ref2 = vpi_handle(vpiParent, port2_bit);
      }

      vpiPortInfo* port1 = dynamic_cast<vpiPortInfo*>(ref1);

      எனில் (!port1) {
	    fprintf(stderr, "sorry: second argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    திரும்பு nullptr;
      }

      vpiPortInfo* port2 = dynamic_cast<vpiPortInfo*>(ref2);

      எனில் (!port2) {
	    fprintf(stderr, "sorry: third argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    திரும்பு nullptr;
      }

	// Get the names of both ports
      std::string port1_name(vpi_get_str(vpiName, ref1));
      std::string port2_name(vpi_get_str(vpiName, ref2));

	// If both ports are vpiOutput, we have to reassign the __vpiSignal from port1
	// to port2 because otherwise the non-delayed version of the signal is dumped
	// even tho the intermodpath is correctly inserted
      __vpiSignal* output_signal = nullptr;

      எனில் (port1->get_direction() == vpiOutput && port2->get_direction() == vpiOutput) {
	    vpiHandle scope_port2 = vpi_handle(vpiScope, ref2);
	    ப்ரதிஜ்ஞா(scope_port2);

	      // Iterate over nets in the scope of port2
	    vpiHandle net_i = vpi_iterate(vpiNet, scope_port2) ;
	    vpiHandle net;

	    போது ((net = vpi_scan(net_i)) != வெற்று) {
		  std::string net_name(vpi_get_str(vpiName, net));

		    // Compare whether the net matches with the port name
		  எனில் (net_name == port2_name) {
			output_signal = dynamic_cast<__vpiSignal*>(net);
		  }
	    }
      }

      vvp_net_t* net1 = port1->get_port();
      vvp_net_t* net2 = port2->get_port();

      எனில் (net1 == nullptr || net2 == nullptr) {
	    fprintf(stderr, "Error: Could not find net. "
		    "Did you run iverilog with '-ginterconnect'?\n");
	    திரும்பு nullptr;
      }

      எனில் (net1 == net2) {
	    fprintf(stderr, "Error: Net for both ports is the same. "
		    "Did you pass the same port twice?\n");
	    திரும்பு nullptr;
      }

      எனில் (!dynamic_cast<vvp_fun_buft*>(net1->fun)) {
	    fprintf(stderr, "Error: functor of net1 must be"
		    "vvp_fun_buft\n");
	    திரும்பு nullptr;
      }

      எனில் (!dynamic_cast<vvp_fun_buft*>(net2->fun)) {
	    fprintf(stderr, "Error: functor of net2 must be"
		    "vvp_fun_buft\n");
	    திரும்பு nullptr;
      }

	// If port1 is actually a port bit, we have to get to the correct vvp_fun_part
	// after which we insert the intermodpath delay
      எனில் (port1_has_index) {
	    vvp_net_ptr_t* net1_ptr = &net1->out_;

	      // Search for part selects connected to port1
	    vvp_net_t* current_net = net1_ptr->ptr();

	    போது (current_net) {
		  மாறிலி vvp_fun_part* part = dynamic_cast<vvp_fun_part*>(current_net->fun);

		    // Its a part select!
		  எனில் (part) {
			  // Is it the correct part select?
			எனில் (part->get_base() == (unsigned)port1_bit_index) {
			      ப்ரதிஜ்ஞா(part->get_wid() == 1);
			      net1 = current_net; // Replace net1 as this is our new start point
			      நிறுத்து;
			}
		  }

		  current_net = current_net->port[0].ptr(); // BUFT has only one input, index 0
	    }
      }

	// Iterate over all nodes connected to port1
      vvp_net_ptr_t cur = net1->out_;
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      போது (cur.ptr()) {
	      // Either port2 is directly connected to port1
	      // Or in the second case port2 is indirectly connected
	      // to port1 through a net of concat8s
	    தருக்க is_connected, is_output;
	    vvp_net_ptr_t previous_முனை;
	    tie(is_connected, previous_முனை, is_output) = port2_has_index
		  ? check_connected_to_concat8(cur, net2, vvp_net_ptr_t(net1, 0))
		  : check_connected_to_concat8_and_part_sa(cur, net2, vvp_net_ptr_t(net1, 0));
	    எனில் ( (!port2_has_index && cur.ptr() == net2) || is_connected ) {
		  vvp_net_t*new_net = புதிய vvp_net_t;

		    // Create new node with intermodpath and connect port2 to it
		  முழு width = 1; // TODO
		  vvp_fun_intermodpath*obj = புதிய vvp_fun_intermodpath(new_net, width);

		  vvp_net_t* output_net = net1;
		  எனில் (is_connected) {
			எனில் (is_output) {
			      output_net = previous_முனை.ptr(); // net2 is direct output of part_sa
			      cur = output_net->out_;
			      prev = vvp_net_ptr_t(nullptr, 0);
			} இல்லையெனில் {
			      prev = previous_முனை;
			      எனில் (prev.ptr()) {
				    cur = prev.ptr()->port[prev.port()];
			      }
			}
		  }

		  new_net->fun = obj;
		  new_net->out_ = cur;

		    // Port2 is in the middle of the list
		    // Insert intermodpath before port2 and keep everything else intact
		  எனில் (prev.ptr()) {
			prev.ptr()->port[prev.port()] = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; // Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		    // Port2 is first in list
		    // Insert intermodpath before port2 and keep everything else intact
		  } இல்லையெனில் {
			output_net->out_ = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; //  Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		  }

		    // If both ports are vpiOutput and port2 is not a vector,
		    // we have to reassign the __vpiSignal so that the delayed
		    // values get dumped
		  எனில் (output_signal && !port2_has_index) {
			net2->fil = net1->fil;
			net1->fil = nullptr;
			output_signal->முனை = net2;
		  }

		    // Create the VPI intermodpath object
		  __vpiInterModPath* intermodpath = vpip_make_intermodpath(new_net, port1, port2);
		  intermodpath->intermodpath = obj;

		    // Finally done, return the intermodpath object
		  திரும்பு intermodpath;
	    }

	    prev = cur;
	    cur = cur.ptr()->port[cur.port()]; // Next net in linked list
      }

      fprintf(stderr, "VPI error: Could not insert intermodpath!\n");
      fprintf(stderr, "\tport1 = %s, port1_has_index = %d, port1_bit_index = %d\n", port1_name.c_str(), port1_has_index, port1_bit_index);
      fprintf(stderr, "\tport2 = %s, port2_has_index = %d, port2_bit_index = %d\n", port2_name.c_str(), port2_has_index, port2_bit_index);
      திரும்பு nullptr;
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



வெறுமை vpi_get_delays(vpiHandle கோவை, p_vpi_delay delays)
{
      ப்ரதிஜ்ஞா(கோவை);
      ப்ரதிஜ்ஞா(delays);

      கோவை->vpi_get_delays(delays);

      எனில் (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_get_delays(%p, %p) -->\n", கோவை, delays);
      }
}


வெறுமை vpi_put_delays(vpiHandle கோவை, p_vpi_delay delays)
{
      ப்ரதிஜ்ஞா(கோவை  );
      ப்ரதிஜ்ஞா(delays );

      கோவை->vpi_put_delays(delays);

      எனில் (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_put_delays(%p, %p) -->\n", கோவை, delays);
      }
}








வெளி "C" PLI_INT32 vpi_vprintf(மாறிலி எழுத்து*fmt, va_list ap)
{
      திரும்பு vpi_mcd_vprintf(1, fmt, ap);
}

வெளி "C" PLI_INT32 vpi_printf(மாறிலி எழுத்து *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      முழு r = vpi_mcd_vprintf(1, fmt, ap);
      va_end(ap);
      திரும்பு r;
}

வெளி "C" PLI_INT32 vpi_flush(வெறுமை)
{
      திரும்பு vpi_mcd_flush(1);
}


வெளி "C" PLI_INT32 vpi_sim_vcontrol(முழு operation, va_list ap)
{
      long diag_செய்தி;

      தேர்வு (operation) {
	  நிலை vpiFinish:
	  நிலை __ivl_legacy_vpiFinish:
            diag_செய்தி = va_arg(ap, PLI_INT32);
	    schedule_finish(diag_செய்தி);
	    நிறுத்து;

	  நிலை vpiStop:
	  நிலை __ivl_legacy_vpiStop:
            diag_செய்தி = va_arg(ap, PLI_INT32);
	    schedule_stop(diag_செய்தி);
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "Unsupported VPI control operation %d.\n", operation);
	    திரும்பு 0;
      }
      திரும்பு 1;
}

வெளி "C" PLI_INT32 vpi_sim_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      முழு r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      திரும்பு r;
}

வெளி "C" PLI_INT32 vpi_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      முழு r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      திரும்பு r;
}

/*
 * This routine calculated the return value for $clog2.
 * It is easier to do it here vs trying to to use the VPI interface.
 */
வெளி "C" s_vpi_vecval vpip_calc_clog2(vpiHandle arg)
{
      s_vpi_vecval rtn;
      s_vpi_மதிப்பு val;
      vvp_vector4_t vec4;
      தருக்க is_neg = பொய்;  // At this point only a real can be negative.

	/* Get the value as a vvp_vector4_t. */
      val.format = vpiObjTypeVal;
      vpi_get_மதிப்பு(arg, &val);
      எனில் (val.format == vpiRealVal) {
	    vpi_get_மதிப்பு(arg, &val);
	      /* All double values can be represented in 1024 bits. */
	    vec4 = vvp_vector4_t(1024, val.value.real);
	    எனில் (val.value.real < 0) is_neg = உண்மை;
      } இல்லையெனில் {
	    val.format = vpiVectorVal;
	    vpi_get_மதிப்பு(arg, &val);
	    unsigned wid = vpi_get(vpiSize, arg);
	    vec4 = vvp_vector4_t(wid, BIT4_0);
	    க்கு (unsigned idx=0; idx < wid; idx += 1) {
		PLI_INT32 aval = val.value.vector[idx/32].aval;
		PLI_INT32 bval = val.value.vector[idx/32].bval;
		aval >>= idx % 32;
		bval >>= idx % 32;
		முழு bitmask = (aval&1) | ((bval<<1)&2);
		vvp_bit4_t bit = scalar_to_bit4(bitmask);
		vec4.set_bit(idx, bit);
	    }
      }

      எனில் (vec4.has_xz()) {
	    rtn.aval = rtn.bval = 0xFFFFFFFFU;  /* Set to 'bx. */
	    திரும்பு rtn;
      }

      vvp_vector2_t vec2(vec4);

      எனில் (is_neg) vec2.trim_neg(); /* This is a special trim! */
      இல்லையெனில் vec2.trim(); /* This makes less work shifting. */

	/* Calculate the clog2 result. */
      PLI_INT32 res = 0;
      எனில் (!vec2.is_zero()) {
	    vec2 -= vvp_vector2_t(1, vec2.size());
	    போது(!vec2.is_zero()) {
		  res += 1;
		  vec2 >>= 1;
	    }
      }

      rtn.aval = res;
      rtn.bval = 0;
      திரும்பு rtn;
}

/*
 * This routine provides the information needed to implement $countdrivers.
 * It is done here for performance reasons - interrogating the drivers
 * individually via the VPI interface would be much slower.
 */
வெளி "C" வெறுமை vpip_மொத்தம்_drivers(vpiHandle ref, unsigned idx,
                                   unsigned counts[4])
{
      கட்டமைப்பு __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ப்ரதிஜ்ஞா(rfp);
      rfp->முனை->மொத்தம்_drivers(idx, counts);
}

#எனில் defined(__MINGW32__) || defined (__CYGWIN__)
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
    .get_மதிப்பு                  = vpi_get_மதிப்பு,
    .put_மதிப்பு                  = vpi_put_மதிப்பு,
    .get_time                   = vpi_get_time,
    .get_userdata               = vpi_get_userdata,
    .put_userdata               = vpi_put_userdata,
    .mcd_open                   = vpi_mcd_open,
    .mcd_close                  = vpi_mcd_close,
    .mcd_flush                  = vpi_mcd_flush,
    .mcd_name                   = vpi_mcd_name,
    .mcd_vprintf                = vpi_mcd_vprintf,
    .flush                      = vpi_flush,
    .vprintf                    = vpi_vprintf,
    .chk_error                  = vpi_chk_error,
    .compare_objects            = vpi_compare_objects,
    .free_object                = vpi_free_object,
    .release_handle             = vpi_release_handle,
    .get_vlog_info              = vpi_get_vlog_info,
    .vcontrol                   = vpi_sim_vcontrol,
    .fopen                      = vpi_fopen,
    .get_file                   = vpi_get_file,
    .calc_clog2                 = vpip_calc_clog2,
    .மொத்தம்_drivers              = vpip_மொத்தம்_drivers,
    .format_strength            = vpip_format_strength,
    .format_pretty              = vpip_format_pretty,
    .make_systf_system_defined  = vpip_make_systf_system_defined,
    .mcd_rawwrite               = vpip_mcd_rawwrite,
    .set_return_மதிப்பு           = vpip_set_return_மதிப்பு,
};
#endif
