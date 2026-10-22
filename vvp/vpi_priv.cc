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

using നാമാകാശ std;
vpi_mode_t vpi_mode_flag = VPI_MODE_NONE;
FILE*vpi_trace = 0;

സ്ഥിതം s_vpi_vlog_info  vpi_vlog_info;
സ്ഥിതം s_vpi_error_info vpip_last_error = { 0, 0, 0, 0, 0, 0, 0 };

സ്ഥിരം അക്ഷരം* direction_as_string(പൂർണ്ണസംഖ്യ dir)
{
      തിരഞ്ഞെടുപ്പ് (dir) {
	  അവസ്ഥ vpi_നിവേശം:
	    തിരികെ "input";
	  അവസ്ഥ vpiOutput:
	    തിരികെ "output";
	  അവസ്ഥ vpiInout:
	    തിരികെ "inout";
	  അവസ്ഥ vpiMixedIO:
	    തിരികെ "mixed io";
	  അവസ്ഥ vpiNoDirection:
	    തിരികെ "no direction";
	  സ്ഥിരസ്ഥിതി:
	    തിരികെ "INVALID-DIRECTION";
      }
}

__vpiHandle::~__vpiHandle()
{ }

പൂർണ്ണസംഖ്യ __vpiHandle::vpi_get(പൂർണ്ണസംഖ്യ)
{ തിരികെ vpiUndefined; }

int64_t __vpiHandle::vpi_get64(പൂർണ്ണസംഖ്യ code)
{ തിരികെ (code == _vpiNexusId) ? 0 : vpiUndefined; }

അക്ഷരം* __vpiHandle::vpi_get_str(പൂർണ്ണസംഖ്യ)
{ തിരികെ 0; }

ശൂന്യം __vpiHandle::vpi_get_മൂല്യം(p_vpi_മൂല്യം)
{ }

vpiHandle __vpiHandle::vpi_put_മൂല്യം(p_vpi_മൂല്യം, പൂർണ്ണസംഖ്യ)
{ തിരികെ 0; }

vpiHandle __vpiHandle::vpi_handle(പൂർണ്ണസംഖ്യ)
{ തിരികെ 0; }

vpiHandle __vpiHandle::vpi_iterate(പൂർണ്ണസംഖ്യ)
{ തിരികെ 0; }

vpiHandle __vpiHandle::vpi_index(പൂർണ്ണസംഖ്യ)
{ തിരികെ 0; }

ശൂന്യം __vpiHandle::vpi_get_delays(p_vpi_delay)
{ }

ശൂന്യം __vpiHandle::vpi_put_delays(p_vpi_delay)
{ }

__vpiBaseVar::__vpiBaseVar(__vpiScope*scope, സ്ഥിരം അക്ഷരം*name, vvp_net_t*net)
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
സ്ഥിതം പൂർണ്ണസംഖ്യ suppress_free(vpiHandle)
{ തിരികെ 1; }
__vpiHandle::free_object_fun_t __vpiHandle::free_object_fun(ശൂന്യം)
{ തിരികെ &suppress_free; }

/*
 * The vpip_string function creates a constant string from the pass
 * input. This constant string is permanently allocated from an
 * efficient string buffer store.
 */
ഘടന vpip_string_chunk {
      ഘടന vpip_string_chunk*next;
      അക്ഷരം data[64*1024 - വലുപ്പം (ഘടന vpip_string_chunk*)];
};

unsigned vpip_size(സ്ഥിരം __vpiSignal *sig)
{
      തിരികെ abs(sig->msb.get_മൂല്യം() - sig->lsb.get_മൂല്യം()) + 1;
}

__vpiScope* vpip_scope(__vpiSignal*sig)
{
      എങ്കിൽ (sig->is_netarray)
	    തിരികെ static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      അല്ലെങ്കിൽ
	    തിരികെ sig->within.scope;
}

__vpiScope* vpip_scope(__vpiRealVar*sig)
{
      എങ്കിൽ (sig->is_netarray)
	    തിരികെ static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      അല്ലെങ്കിൽ
	    തിരികെ sig->within.scope;
}

vpiHandle vpip_module(__vpiScope*scope)
{
      ആകുമ്പോൾ(scope && scope->get_type_code() != vpiModule) {
	    scope = scope->scope;
      }
      പ്രതിജ്ഞാ(scope);
      തിരികെ scope;
}

സ്ഥിരം അക്ഷരം *vpip_string(സ്ഥിരം അക്ഷരം*str)
{
      സ്ഥിതം vpip_string_chunk first_chunk = {0, {0}};
      സ്ഥിതം vpip_string_chunk*chunk_list = &first_chunk;
      സ്ഥിതം unsigned chunk_fill = 0;

      unsigned len = strlen(str);
      പ്രതിജ്ഞാ( (len+1) <= വലുപ്പം chunk_list->data );

      എങ്കിൽ ( (len+1) > (വലുപ്പം chunk_list->data - chunk_fill) ) {
	    vpip_string_chunk*tmp = പുതിയ vpip_string_chunk;
	    tmp->next = chunk_list;
	    chunk_list = tmp;
	    chunk_fill = 0;
      }

      അക്ഷരം*res = chunk_list->data + chunk_fill;
      chunk_fill += len + 1;

      strcpy(res, str);
      തിരികെ res;
}

സ്ഥിതം unsigned hash_string(സ്ഥിരം അക്ഷരം*text)
{
      unsigned h = 0;

      ആകുമ്പോൾ (*text) {
	    h = (h << 4) ^ (h >> 28) ^ (unsigned)*text;
	    text += 1;
      }
      തിരികെ h;
}

സ്ഥിരം അക്ഷരം* vpip_name_string(സ്ഥിരം അക്ഷരം*text)
{
      സ്ഥിരം unsigned HASH_SIZE = 4096;
      സ്ഥിതം സ്ഥിരം അക്ഷരം*hash_table[HASH_SIZE] = {0};

      unsigned hash_മൂല്യം = hash_string(text) % HASH_SIZE;

	/* If we easily find the string in the hash table, then return
	   that and be done. */
      എങ്കിൽ (hash_table[hash_മൂല്യം]
	  && (strcmp(hash_table[hash_മൂല്യം], text) == 0)) {
	    തിരികെ hash_table[hash_മൂല്യം];
      }

	/* The existing hash entry is not a match. Replace it with the
	   newly allocated value, and return the new pointer as the
	   result to the add. */
      സ്ഥിരം അക്ഷരം*res = vpip_string(text);
      hash_table[hash_മൂല്യം] = res;

      തിരികെ res;
}

PLI_INT32 vpi_chk_error(p_vpi_error_info info)
{
      എങ്കിൽ (vpip_last_error.state == 0)
	    തിരികെ 0;

      info->state = vpip_last_error.state;
      info->level = vpip_last_error.level;
      info->message = vpip_last_error.message;
      info->product = vpi_vlog_info.product;
      info->code = (അക്ഷരം *)"";
      info->file = 0;
      info->വരി = 0;

      തിരികെ info->level;
}

PLI_INT32 vpi_compare_objects(vpiHandle obj1, vpiHandle obj2)
{
      പ്രതിജ്ഞാ(obj1);
      പ്രതിജ്ഞാ(obj2);

	// Does this actually work for all cases?
      എങ്കിൽ (obj1 != obj2) തിരികെ 0;
      അല്ലെങ്കിൽ തിരികെ 1;
}

/*
 * Copy the internal information to the data structure. Do not free or
 * change the tfname/user_data since they are a pointer to the real
 * string/data values. We also support passing a task or function handle
 * instead of just a handle to a vpiUserSystf.
 */
ശൂന്യം vpi_get_systf_info(vpiHandle ref, p_vpi_systf_data data)
{
      ഘടന __vpiUserSystf* rfp = dynamic_cast<__vpiUserSystf*>(ref);
      എങ്കിൽ (rfp == 0) {
	    ഘടന __vpiSysTaskCall*call = dynamic_cast<__vpiSysTaskCall*>(ref);
	    പ്രതിജ്ഞാ(call);
	    rfp = call->defn;
      }

	/* Assert that vpiUserDefn is true! */
      പ്രതിജ്ഞാ(rfp->is_user_defn);

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
ഘടന __vpiSysTaskCall*vpip_cur_task = 0;

PLI_INT32 vpi_free_object(vpiHandle ref)
{
      പൂർണ്ണസംഖ്യ rtn;

      എങ്കിൽ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_free_object(%p)", ref);
	    fflush(vpi_trace);
      }

      പ്രതിജ്ഞാ(ref);
      __vpiHandle::free_object_fun_t fun = ref->free_object_fun();
      rtn = fun (ref);

      എങ്കിൽ (vpi_trace)
	    fprintf(vpi_trace, " --> %d\n", rtn);

      തിരികെ rtn;
}

PLI_INT32 vpi_release_handle(vpiHandle ref)
{
	// Since SystemVerilog vpi_free_object() has been
	// renamed vpi_release_handle(), and thus
	// vpi_free_object() has been deprecated.
      തിരികെ vpi_free_object(ref);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ vpip_get_global(പൂർണ്ണസംഖ്യ property)
{
      തിരഞ്ഞെടുപ്പ് (property) {

	  അവസ്ഥ vpiTimeUnit:
	  അവസ്ഥ vpiTimePrecision:
	    തിരികെ vpip_get_time_precision();

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vpi error: bad global property: %d\n", property);
	    പ്രതിജ്ഞാ(0);
	    തിരികെ vpiUndefined;
      }
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* vpi_property_str(PLI_INT32 code)
{
      സ്ഥിതം അക്ഷരം buf[32];
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiConstType:
	    തിരികെ "vpiConstType";
	  അവസ്ഥ vpiName:
	    തിരികെ "vpiName";
	  അവസ്ഥ vpiFullName:
	    തിരികെ "vpiFullName";
	  അവസ്ഥ vpiTimeUnit:
	    തിരികെ "vpiTimeUnit";
	  അവസ്ഥ vpiTimePrecision:
	    തിരികെ "vpiTimePrecision";
          അവസ്ഥ vpiSize:
	    തിരികെ "vpiSize";
          അവസ്ഥ _vpiNexusId:
	    തിരികെ "_vpiNexusId";
	  സ്ഥിരസ്ഥിതി:
	    snprintf(buf, വലുപ്പം(buf), "%d", (പൂർണ്ണസംഖ്യ)code);
      }
      തിരികെ buf;
}

സ്ഥിരം അക്ഷരം* vpi_type_as_string(PLI_INT32 code)
{
      സ്ഥിതം അക്ഷരം buf[32];
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiArrayType:
	    തിരികെ "vpiArrayType";
	  അവസ്ഥ vpiBitVar:
	    തിരികെ "vpiBitVar";
	  അവസ്ഥ vpiByteVar:
	    തിരികെ "vpiByteVar";
	  അവസ്ഥ vpiClassVar:
	    തിരികെ "vpiClassVar";
	  അവസ്ഥ vpiConstant:
	    തിരികെ "vpiConstant";
	  അവസ്ഥ vpiEnumTypespec:
	    തിരികെ "vpiEnumTypespec";
	  അവസ്ഥ vpiFunction:
	    തിരികെ "vpiFunction";
	  അവസ്ഥ vpiGenScope:
	    തിരികെ "vpiGenScope";
	  അവസ്ഥ vpiIntVar:
	    തിരികെ "vpiIntVar";
	  അവസ്ഥ vpiIntegerVar:
	    തിരികെ "vpiIntegerVar";
	  അവസ്ഥ vpiIterator:
	    തിരികെ "vpiIterator";
	  അവസ്ഥ vpiLongIntVar:
	    തിരികെ "vpiLongIntVar";
	  അവസ്ഥ vpiMemory:
	    തിരികെ "vpiMemory";
	  അവസ്ഥ vpiMemoryWord:
	    തിരികെ "vpiMemoryWord";
	  അവസ്ഥ vpiModule:
	    തിരികെ "vpiModule";
	  അവസ്ഥ vpiNamedBegin:
	    തിരികെ "vpiNamedBegin";
	  അവസ്ഥ vpiNamedEvent:
	    തിരികെ "vpiNamedEvent";
	  അവസ്ഥ vpiNamedFork:
	    തിരികെ "vpiNamedFork";
	  അവസ്ഥ vpiPackage:
	    തിരികെ "vpiPackage";
	  അവസ്ഥ vpiPathTerm:
	    തിരികെ "vpiPathTerm";
	  അവസ്ഥ vpiPort:
	    തിരികെ "vpiPort";
	  അവസ്ഥ vpiNet:
	    തിരികെ "vpiNet";
	  അവസ്ഥ vpiNetArray:
	    തിരികെ "vpiNetArray";
	  അവസ്ഥ vpiNetBit:
	    തിരികെ "vpiNetBit";
	  അവസ്ഥ vpiParameter:
	    തിരികെ "vpiParameter";
	  അവസ്ഥ vpiPartSelect:
	    തിരികെ "vpiPartSelect";
	  അവസ്ഥ vpiRealVar:
	    തിരികെ "vpiRealVar";
	  അവസ്ഥ vpiReg:
	    തിരികെ "vpiReg";
	  അവസ്ഥ vpiRegBit:
	    തിരികെ "vpiRegBit";
	  അവസ്ഥ vpiShortIntVar:
	    തിരികെ "vpiShortIntVar";
	  അവസ്ഥ vpiStringVar:
	    തിരികെ "vpiStringVar";
	  അവസ്ഥ vpiSysFuncCall:
	    തിരികെ "vpiSysFuncCall";
	  അവസ്ഥ vpiSysTaskCall:
	    തിരികെ "vpiSysTaskCall";
	  അവസ്ഥ vpiTask:
	    തിരികെ "vpiTask";
	  അവസ്ഥ vpiTimeVar:
	    തിരികെ "vpiTimeVar";
	  അവസ്ഥ vpiUserSystf:
	    തിരികെ "vpiUserSystf";
	  സ്ഥിരസ്ഥിതി:
	    snprintf(buf, വലുപ്പം(buf), "%d", (പൂർണ്ണസംഖ്യ)code);
      }
      തിരികെ buf;
}

PLI_INT32 vpi_get(പൂർണ്ണസംഖ്യ property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      എങ്കിൽ (property == _vpiDelaySelection) തിരികെ vpip_delay_selection;

      എങ്കിൽ (ref == 0)
	    തിരികെ vpip_get_global(property);

      എങ്കിൽ (property == vpiType) {
	    എങ്കിൽ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

	    എങ്കിൽ (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  തിരികെ vpiNetArray;
	    അല്ലെങ്കിൽ
		  തിരികെ ref->get_type_code();
      }

      പൂർണ്ണസംഖ്യ res = ref->vpi_get(property);

      എങ്കിൽ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get(%s, %p) --> %d\n",
		    vpi_property_str(property), ref, res);
      }

      തിരികെ res;
}

PLI_INT64 vpi_get64(പൂർണ്ണസംഖ്യ property, vpiHandle ref)
{
	// For now we only support getting the nexus id.
      എങ്കിൽ ((ref == 0) || (property != _vpiNexusId)) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) is not currently supported.\n",
		    vpi_property_str(property), ref);
	    തിരികെ vpiUndefined;
      }

      int64_t res = ref->vpi_get64(property);

      എങ്കിൽ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) --> %" PRId64 "\n",
		    vpi_property_str(property), ref, res);
      }

      തിരികെ res;
}

അക്ഷരം* vpi_get_str(PLI_INT32 property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      എങ്കിൽ (property == _vpiDelaySelection) {
	    തിരഞ്ഞെടുപ്പ് (vpip_delay_selection) {
		അവസ്ഥ _vpiDelaySelMinimum:
		  തിരികെ simple_set_rbuf_str("MINIMUM");
		അവസ്ഥ _vpiDelaySelTypical:
		  തിരികെ simple_set_rbuf_str("TYPICAL");
		അവസ്ഥ _vpiDelaySelMaximum:
		  തിരികെ simple_set_rbuf_str("MAXIMUM");
		സ്ഥിരസ്ഥിതി:
		  പ്രതിജ്ഞാ(0);
	    }
      }

      എങ്കിൽ (ref == 0) {
	    fprintf(stderr, "vpi error: vpi_get_str(%s, 0) called "
		    "with null vpiHandle.\n", vpi_property_str(property));
	    തിരികെ 0;
      }

      എങ്കിൽ (property == vpiType) {
	    എങ്കിൽ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

            PLI_INT32 type;
	    എങ്കിൽ (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  type = vpiNetArray;
	    അല്ലെങ്കിൽ
		  type = ref->get_type_code();
	    തിരികെ (അക്ഷരം *)vpi_type_as_string(type);
      }

      അക്ഷരം*res = ref->vpi_get_str(property);

      എങ്കിൽ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get_str(%s, %p) --> %s\n",
		    vpi_property_str(property), ref, res? res : "<NULL>");
      }

      തിരികെ res;
}

__vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj)
{
      സ്ഥിരം ഘടന __vpiSysTaskCall*task;
      ഘടന __vpiSignal*സൂചന;
      ഘടന __vpiRealVar*real;
      സ്ഥിരം __vpiNamedEvent*event;

      തിരഞ്ഞെടുപ്പ് (obj->get_type_code()) {
	  അവസ്ഥ vpiSysTaskCall:
	    task = dynamic_cast<__vpiSysTaskCall*>(obj);
	    തിരികെ task->scope;

	  അവസ്ഥ vpiModule:
	    തിരികെ dynamic_cast<__vpiScope*>(obj);

	  അവസ്ഥ vpiNet:
	  അവസ്ഥ vpiReg:
	  അവസ്ഥ vpiIntegerVar:
	  അവസ്ഥ vpiBitVar:
	  അവസ്ഥ vpiByteVar:
	  അവസ്ഥ vpiShortIntVar:
	  അവസ്ഥ vpiIntVar:
	  അവസ്ഥ vpiLongIntVar:
	    സൂചന = dynamic_cast<__vpiSignal*>(obj);
	    തിരികെ vpip_scope(സൂചന);

	  അവസ്ഥ vpiRealVar:
	    real = dynamic_cast<__vpiRealVar*>(obj);
	    തിരികെ vpip_scope(real);

	  അവസ്ഥ vpiNamedEvent:
	    event = dynamic_cast<__vpiNamedEvent*>(obj);
	    തിരികെ event->get_scope();

	  അവസ്ഥ vpiMemory:
	  അവസ്ഥ vpiMemoryWord:
	  അവസ്ഥ vpiPartSelect:
	    തിരികെ dynamic_cast<__vpiScope*>(obj->vpi_handle(vpiScope));

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "ERROR: vpip_scope_from_handle called with "
		    "object handle type=%d\n", obj->get_type_code());
	    പ്രതിജ്ഞാ(0);
	    തിരികെ 0;
      }
}

പൂർണ്ണസംഖ്യ vpip_time_units_from_handle(vpiHandle obj)
{
      എങ്കിൽ (obj == 0)
	    തിരികെ vpip_get_time_precision();

      സ്ഥിരം __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      എങ്കിൽ (scope == 0)
	    തിരികെ vpip_get_time_precision();

      തിരികെ scope->time_units;
}

പൂർണ്ണസംഖ്യ vpip_time_precision_from_handle(vpiHandle obj)
{
      എങ്കിൽ (obj == 0)
	    തിരികെ vpip_get_time_precision();

      സ്ഥിരം __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      എങ്കിൽ (scope == 0)
	    തിരികെ vpip_get_time_precision();

      തിരികെ scope->time_precision;
}

ഇരട്ട vpip_scaled_time_from_handle(vvp_time64_t time, vpiHandle obj)
{
      പൂർണ്ണസംഖ്യ scale = vpip_get_time_precision() -
	          vpip_time_units_from_handle(obj);
      എങ്കിൽ (scale >= 0)
	    തിരികെ (ഇരട്ട)time * pow(10.0, scale);
      അല്ലെങ്കിൽ
	    തിരികെ (ഇരട്ട)time / pow(10.0, -scale);
}

ശൂന്യം vpi_get_time(vpiHandle obj, s_vpi_time*vp)
{
      vvp_time64_t time;

      പ്രതിജ്ഞാ(vp);

      time = schedule_simtime();

      തിരഞ്ഞെടുപ്പ് (vp->type) {
          അവസ്ഥ vpiSimTime:
	    vp->മുകളിലെ = (time >> 32) & 0xffffffff;
	    vp->താഴെയുള്ള = time & 0xffffffff;
	    നിർത്തുക;

          അവസ്ഥ vpiScaledRealTime:
	    vp->real = vpip_scaled_time_from_handle(time, obj);
	    നിർത്തുക;

          സ്ഥിരസ്ഥിതി:
            fprintf(stderr, "vpi_get_time: unknown type: %d\n", (പൂർണ്ണസംഖ്യ)vp->type);
            പ്രതിജ്ഞാ(0);
	    നിർത്തുക;
      }
}

PLI_INT32 vpi_get_vlog_info(p_vpi_vlog_info vlog_info_p)
{
    എങ്കിൽ (vlog_info_p != 0) {
	  *vlog_info_p = vpi_vlog_info;
	  തിരികെ 1;
    } അല്ലെങ്കിൽ {
	  തിരികെ 0;
    }
}

ശൂന്യം vpi_set_vlog_info(പൂർണ്ണസംഖ്യ argc, അക്ഷരം** argv)
{
    സ്ഥിതം അക്ഷരം icarus_product[] = "Icarus Verilog";
    സ്ഥിതം അക്ഷരം icarus_version[] = VERSION;
    vpi_vlog_info.product = icarus_product;
    vpi_vlog_info.version = icarus_version;
    vpi_vlog_info.argc    = argc;
    vpi_vlog_info.argv    = argv;

    സ്ഥിതം അക്ഷരം trace_buf[1024];
    എങ്കിൽ (സ്ഥിരം അക്ഷരം*path = getenv("VPI_TRACE"))  {
	  എങ്കിൽ (!strcmp(path,"-"))
		vpi_trace = stdout;
	  അല്ലെങ്കിൽ {
		vpi_trace = fopen(path, "w");
		എങ്കിൽ (!vpi_trace) {
		      perror(path);
		      exit(1);
		}
		setvbuf(vpi_trace, trace_buf, _IOLBF, വലുപ്പം(trace_buf));
	  }
    }
}

സ്ഥിതം ശൂന്യം vec4_get_മൂല്യം_string(സ്ഥിരം vvp_vector4_t&word_val, unsigned width,
				  s_vpi_മൂല്യം*vp)
{
      unsigned nchar = width / 8;
      unsigned tail = width % 8;

      അക്ഷരം*rbuf = static_cast<അക്ഷരം *>(need_result_buf(nchar + 1, RBUF_VAL));
      അക്ഷരം*cp = rbuf;

      എങ്കിൽ (tail > 0) {
	    അക്ഷരം char_val = 0;
	    വേണ്ടി (unsigned idx = width-tail; idx < width ;  idx += 1) {
		  vvp_bit4_t val = word_val.value(idx);
		  എങ്കിൽ (val == BIT4_1)
			char_val |= 1 << idx;
	    }

	    എങ്കിൽ (char_val != 0)
		  *cp++ = char_val;
	    അല്ലെങ്കിൽ എങ്കിൽ (cp != rbuf)
		  *cp++ = ' ';
      }

      വേണ്ടി (unsigned idx = 0 ;  idx < nchar ;  idx += 1) {
	    unsigned bit = (nchar - idx - 1) * 8;
	    അക്ഷരം char_val = 0;
	    വേണ്ടി (unsigned bdx = 0 ;  bdx < 8 ;  bdx += 1) {
		  vvp_bit4_t val = word_val.value(bit+bdx);
		  എങ്കിൽ (val == BIT4_1)
			char_val |= 1 << bdx;
	    }
	    // Ignore leading null-bytes and replace other null-bytes with space.
	    // The LRM is not entirely clear on how null bytes should be handled.
	    // This is the implementation chosen for iverilog.
	    എങ്കിൽ (char_val != 0)
		  *cp++ = char_val;
	    അല്ലെങ്കിൽ എങ്കിൽ (cp != rbuf)
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
ശൂന്യം vpip_vec4_get_മൂല്യം(സ്ഥിരം vvp_vector4_t&word_val, unsigned width,
			 തർക്കം signed_flag, s_vpi_മൂല്യം*vp)
{
      അക്ഷരം *rbuf = 0;

      തിരഞ്ഞെടുപ്പ് (vp->format) {
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector values.\n", (പൂർണ്ണസംഖ്യ)vp->format);
	    പ്രതിജ്ഞാ(0);

	  അവസ്ഥ vpiSuppressVal:
	    നിർത്തുക;

	  അവസ്ഥ vpiBinStrVal:
	    rbuf = static_cast<അക്ഷരം *>(need_result_buf(width+1, RBUF_VAL));
	    വേണ്ടി (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		  vvp_bit4_t bit = word_val.value(idx);
		  rbuf[width-idx-1] = vvp_bit4_to_ascii(bit);
	    }
	    rbuf[width] = 0;
	    vp->value.str = rbuf;
	    നിർത്തുക;

	  അവസ്ഥ vpiOctStrVal: {
		unsigned hwid = ((width+2) / 3) + 1;
		rbuf = static_cast<അക്ഷരം *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_oct_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		നിർത്തുക;
	  }

	  അവസ്ഥ vpiDecStrVal: {
// HERE need a better estimate.
		rbuf = static_cast<അക്ഷരം *>(need_result_buf(width+1, RBUF_VAL));
		vpip_vec4_to_dec_str(word_val, rbuf, width+1, signed_flag);
		vp->value.str = rbuf;
		നിർത്തുക;
	  }

	  അവസ്ഥ vpiHexStrVal: {
		unsigned  hwid = ((width + 3) / 4) + 1;
		rbuf = static_cast<അക്ഷരം *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_hex_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		നിർത്തുക;
	  }

          അവസ്ഥ vpiScalarVal: {
	        // scalars should be of size 1
	        പ്രതിജ്ഞാ(width == 1);
	        തിരഞ്ഞെടുപ്പ്(word_val.value(0)) {
		    അവസ്ഥ BIT4_0:
		      vp->value.scalar = vpi0;
		      നിർത്തുക;
	            അവസ്ഥ BIT4_1:
                      vp->value.scalar = vpi1;
                      നിർത്തുക;
	            അവസ്ഥ BIT4_X:
                      vp->value.scalar = vpiX;
                      നിർത്തുക;
	            അവസ്ഥ BIT4_Z:
                      vp->value.scalar = vpiZ;
                      നിർത്തുക;
		}
                നിർത്തുക;
          }

	  അവസ്ഥ vpiIntVal: {
		long val = 0;
		vvp_bit4_t pad = BIT4_0;
		എങ്കിൽ (signed_flag && word_val.size() > 0)
		      pad = word_val.value(word_val.size()-1);

		വേണ്ടി (unsigned idx = 0 ; idx < 8*വലുപ്പം(val) ;  idx += 1) {
		      vvp_bit4_t val4 = pad;
		      എങ്കിൽ (idx < word_val.size())
			    val4 = word_val.value(idx);
		      എങ്കിൽ (val4 == BIT4_1)
			    val |= 1L << idx;
		}

		vp->value.integer = val;
		നിർത്തുക;
	  }

	  അവസ്ഥ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiVectorVal;
	    // fallthrough
	  അവസ്ഥ vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * വലുപ്പം(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		വേണ്ടി (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      തിരഞ്ഞെടുപ്പ് (word_val.value(idx)) {
			  അവസ്ഥ BIT4_0:
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    നിർത്തുക;
			  അവസ്ഥ BIT4_1:
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    നിർത്തുക;
			  അവസ്ഥ BIT4_X:
			    op->aval |= (1 << idx % 32);
			    op->bval |= (1 << idx % 32);
			    നിർത്തുക;
			  അവസ്ഥ BIT4_Z:
			    op->aval &= ~(1 << idx % 32);
			    op->bval |=  (1 << idx % 32);
			    നിർത്തുക;
		      }
		      എങ്കിൽ (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		നിർത്തുക;
	  }

	  അവസ്ഥ vpiStringVal:
		vec4_get_മൂല്യം_string(word_val, width, vp);
		നിർത്തുക;

	  അവസ്ഥ vpiRealVal:
		vector4_to_മൂല്യം(word_val, vp->value.real, signed_flag);
		നിർത്തുക;
      }
}

ശൂന്യം vpip_vec2_get_മൂല്യം(സ്ഥിരം vvp_vector2_t&word_val, unsigned width,
			 തർക്കം signed_flag, s_vpi_മൂല്യം*vp)
{
      തിരഞ്ഞെടുപ്പ് (vp->format) {
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector2 values.\n", (പൂർണ്ണസംഖ്യ)vp->format);
	    പ്രതിജ്ഞാ(0);

	  അവസ്ഥ vpiSuppressVal:
	    നിർത്തുക;

	  അവസ്ഥ vpiIntVal:
	    vector2_to_മൂല്യം(word_val, vp->value.integer, signed_flag);
	    നിർത്തുക;

	  അവസ്ഥ vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  അവസ്ഥ vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * വലുപ്പം(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		വേണ്ടി (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      എങ്കിൽ (word_val.value(idx)) {
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      } അല്ലെങ്കിൽ {
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      }
		      എങ്കിൽ (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		നിർത്തുക;
	  }
      }

}

/*
 * Convert a real value to the appropriate integer.
 */
സ്ഥിതം PLI_INT32 get_real_as_int(ഇരട്ട real)
{
      ഇരട്ട rtn;

	/* We would normally want to return 'bx for a NaN or
	 * +/- infinity, but for an integer the standard says
	 * to convert 'bx to 0, so we just return 0. */
      എങ്കിൽ (real != real || (real && (real == 0.5*real))) {
	    തിരികെ 0;
      }

      rtn = std::round(real);

      തിരികെ (PLI_INT32) rtn;
}

/*
 * This is a generic function to convert a double value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer.
 */
ശൂന്യം vpip_real_get_മൂല്യം(ഇരട്ട real, s_vpi_മൂല്യം*vp)
{
      അക്ഷരം *rbuf = 0;

      തിരഞ്ഞെടുപ്പ് (vp->format) {
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting real values.\n", (പൂർണ്ണസംഖ്യ)vp->format);
	    പ്രതിജ്ഞാ(0);

	  അവസ്ഥ vpiSuppressVal:
	    നിർത്തുക;

	  അവസ്ഥ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiRealVal;
	    // fallthrough
	  അവസ്ഥ vpiRealVal:
	    vp->value.real = real;
	    നിർത്തുക;

	  അവസ്ഥ vpiIntVal:
	    vp->value.integer = get_real_as_int(real);
	    നിർത്തുക;

	  അവസ്ഥ vpiDecStrVal:
	    rbuf = static_cast<അക്ഷരം *>(need_result_buf(1025, RBUF_VAL));
	    vpip_vec4_to_dec_str(vvp_vector4_t(1024, real), rbuf, 1025, സത്യം);
	    vp->value.str = rbuf;
	    നിർത്തുക;
      }
}

ഇരട്ട real_from_vpi_മൂല്യം(s_vpi_മൂല്യം*vp)
{
      vvp_vector4_t vec4(1024);
      ഇരട്ട result;
      തർക്കം is_signed = അസത്യം;

      തിരഞ്ഞെടുപ്പ് (vp->format) {
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "putting real values.\n", (പൂർണ്ണസംഖ്യ)vp->format);
	    പ്രതിജ്ഞാ(0);

	  അവസ്ഥ vpiRealVal:
	    result = vp->value.real;
	    നിർത്തുക;

	  അവസ്ഥ vpiIntVal:
	    result = (ഇരട്ട) vp->value.integer;
	    നിർത്തുക;

	  അവസ്ഥ vpiBinStrVal:
	    vpip_bin_str_to_vec4(vec4, vp->value.str);
	    എങ്കിൽ (vp->value.str[0] == '-') is_signed = സത്യം;
	    vector4_to_മൂല്യം(vec4, result, is_signed);
	    നിർത്തുക;

	  അവസ്ഥ vpiOctStrVal:
	    vpip_oct_str_to_vec4(vec4, vp->value.str);
	    എങ്കിൽ (vp->value.str[0] == '-') is_signed = സത്യം;
	    vector4_to_മൂല്യം(vec4, result, is_signed);
	    നിർത്തുക;

	  അവസ്ഥ vpiDecStrVal:
	    vpip_dec_str_to_vec4(vec4, vp->value.str);
	    എങ്കിൽ (vp->value.str[0] == '-') is_signed = സത്യം;
	    vector4_to_മൂല്യം(vec4, result, is_signed);
	    നിർത്തുക;

	  അവസ്ഥ vpiHexStrVal:
	    vpip_hex_str_to_vec4(vec4, vp->value.str);
	    എങ്കിൽ (vp->value.str[0] == '-') is_signed = സത്യം;
	    vector4_to_മൂല്യം(vec4, result, is_signed);
	    നിർത്തുക;

      }

      തിരികെ result;
}

ശൂന്യം vpip_string_get_മൂല്യം(സ്ഥിരം string&val, s_vpi_മൂല്യം*vp)
{
      അക്ഷരം *rbuf = 0;

      തിരഞ്ഞെടുപ്പ് (vp->format) {
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting string values.\n", (പൂർണ്ണസംഖ്യ)vp->format);
	    പ്രതിജ്ഞാ(0);

	  അവസ്ഥ vpiSuppressVal:
	    നിർത്തുക;

	  അവസ്ഥ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiStringVal;
	    // fallthrough
	  അവസ്ഥ vpiStringVal:
	    rbuf = static_cast<അക്ഷരം *>(need_result_buf(val.size() + 1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    നിർത്തുക;
      }
}


ശൂന്യം vpi_get_മൂല്യം(vpiHandle വ്യഞ്ജകം, s_vpi_മൂല്യം*vp)
{
      പ്രതിജ്ഞാ(വ്യഞ്ജകം);
      പ്രതിജ്ഞാ(vp);

	// Never bother with suppressed values. All the derived
	// classes can ignore this type.
      എങ്കിൽ (vp->format == vpiSuppressVal)
	    തിരികെ;

      വ്യഞ്ജകം->vpi_get_മൂല്യം(vp);

      എങ്കിൽ (vpi_trace) തിരഞ്ഞെടുപ്പ് (vp->format) {
		അവസ്ഥ vpiStringVal:
		  fprintf(vpi_trace,"vpi_get_value(%p=<%d>) -> string=\"%s\"\n",
			  വ്യഞ്ജകം, വ്യഞ്ജകം->get_type_code(), vp->value.str);
		  നിർത്തുക;

		അവസ്ഥ vpiBinStrVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> binstr=%s\n",
			  വ്യഞ്ജകം->get_type_code(), vp->value.str);
		  നിർത്തുക;

		അവസ്ഥ vpiIntVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> int=%d\n",
			  വ്യഞ്ജകം->get_type_code(), (പൂർണ്ണസംഖ്യ)vp->value.integer);
		  നിർത്തുക;

		അവസ്ഥ vpiSuppressVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <suppress>\n",
			  വ്യഞ്ജകം->get_type_code());
		  നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <%d>=?\n",
			  വ്യഞ്ജകം->get_type_code(), (പൂർണ്ണസംഖ്യ)vp->format);
	    }
}

ഘടന vpip_put_മൂല്യം_event : vvp_gen_event_s {
      vpiHandle handle;
      s_vpi_മൂല്യം value;
      പൂർണ്ണസംഖ്യ flags;
      മായാ ശൂന്യം run_run() override;
      ~vpip_put_മൂല്യം_event() override { }
};

ശൂന്യം vpip_put_മൂല്യം_event::run_run()
{
      handle->vpi_put_മൂല്യം(&value, flags);
      തിരഞ്ഞെടുപ്പ് (value.format) {
	    /* Free the copied string. */
	  അവസ്ഥ vpiBinStrVal:
	  അവസ്ഥ vpiOctStrVal:
	  അവസ്ഥ vpiDecStrVal:
	  അവസ്ഥ vpiHexStrVal:
	  അവസ്ഥ vpiStringVal:
	    സ്വതന്ത്രം(value.value.str);
	    നിർത്തുക;
	    /* Free the copied time structure. */
	  അവസ്ഥ vpiTimeVal:
	    സ്വതന്ത്രം(value.value.time);
	    നിർത്തുക;
	    /* Free the copied vector structure. */
	  അവസ്ഥ vpiVectorVal:
	    സ്വതന്ത്രം(value.value.vector);
	    നിർത്തുക;
	    /* Free the copied strength structure. */
	  അവസ്ഥ vpiStrengthVal:
	    സ്വതന്ത്രം(value.value.strength);
	    നിർത്തുക;
	    /* Everything else is static in the structure. */
	  സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
      }
}

/* Make a copy of a pointer to a time structure. */
സ്ഥിതം t_vpi_time *timedup(സ്ഥിരം t_vpi_time *val)
{
      t_vpi_time *rtn;
      rtn = static_cast<t_vpi_time *> (malloc(വലുപ്പം(t_vpi_time)));
      *rtn = *val;
      തിരികെ rtn;
}

/* Make a copy of a pointer to a vector value structure. */
സ്ഥിതം t_vpi_vecval *vectordup(സ്ഥിരം t_vpi_vecval *val, PLI_INT32 size)
{
      unsigned അക്കം_bytes;
      t_vpi_vecval *rtn;
      പ്രതിജ്ഞാ(size > 0);
      അക്കം_bytes = ((size + 31)/32)*വലുപ്പം(t_vpi_vecval);
      rtn = static_cast<t_vpi_vecval *> (malloc(അക്കം_bytes));
      memcpy(rtn, val, അക്കം_bytes);
      തിരികെ rtn;
}

/* Make a copy of a pointer to a strength structure. */
സ്ഥിതം t_vpi_strengthval *strengthdup(സ്ഥിരം t_vpi_strengthval *val)
{
      t_vpi_strengthval *rtn;
      rtn = static_cast<t_vpi_strengthval *>
            (malloc(വലുപ്പം(t_vpi_strengthval)));
      *rtn = *val;
      തിരികെ rtn;
}

vpiHandle vpi_put_മൂല്യം(vpiHandle obj, s_vpi_മൂല്യം*vp,
			s_vpi_time*when, PLI_INT32 flags)
{
      പ്രതിജ്ഞാ(obj);

      flags &= ~vpiReturnEvent;

      എങ്കിൽ (flags!=vpiNoDelay && flags!=vpiForceFlag && flags!=vpiReleaseFlag) {
	    vvp_time64_t dly;
	    പൂർണ്ണസംഖ്യ scale;

	    എങ്കിൽ (vpi_get(vpiAutomatic, obj)) {
		  fprintf(stderr, "VPI error: cannot put a value with "
				  "a delay on automatically allocated "
				  "variable '%s'.\n",
				  vpi_get_str(vpiName, obj));
		  തിരികെ 0;
	    }

	    പ്രതിജ്ഞാ(when != 0);

	    തിരഞ്ഞെടുപ്പ് (when->type) {
		അവസ്ഥ vpiScaledRealTime:
		  scale = vpip_time_units_from_handle(obj) -
		          vpip_get_time_precision();
		  എങ്കിൽ (scale >= 0) {
			dly = (vvp_time64_t)(when->real * pow(10.0, scale));
		  } അല്ലെങ്കിൽ {
			dly = (vvp_time64_t)(when->real / pow(10.0, -scale));
		  }
		  നിർത്തുക;
		അവസ്ഥ vpiSimTime:
		  dly = vpip_timestruct_to_time(when);
		  നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		  dly = 0;
		  നിർത്തുക;
	    }

	    എങ്കിൽ ((dly == 0) && schedule_at_rosync()) {
		  fprintf(stderr, "VPI error: attempted to put a value to "
				  "variable '%s' during a read-only synch "
				  "callback.\n", vpi_get_str(vpiName, obj));
		  തിരികെ 0;
	    }

	    vpip_put_മൂല്യം_event*put = പുതിയ vpip_put_മൂല്യം_event;
	    put->handle = obj;
	    എങ്കിൽ (dynamic_cast<__vpiNamedEvent*>(obj)) {
		  put->value.format = vpiIntVal;
		  put->value.value.integer = 0;
	    } അല്ലെങ്കിൽ {
		  പ്രതിജ്ഞാ(vp);
		  put->value = *vp;
	    }
	      /* Since this is a scheduled put event we must copy any pointer
	       * data to keep it available until the event is actually run. */
	    തിരഞ്ഞെടുപ്പ് (put->value.format) {
		  /* Copy the string items. */
		അവസ്ഥ vpiBinStrVal:
		അവസ്ഥ vpiOctStrVal:
		അവസ്ഥ vpiDecStrVal:
		അവസ്ഥ vpiHexStrVal:
		അവസ്ഥ vpiStringVal:
		  put->value.value.str = strdup(put->value.value.str);
		  നിർത്തുക;
		  /* Copy a time pointer item. */
		അവസ്ഥ vpiTimeVal:
		  put->value.value.time = timedup(put->value.value.time);
		  നിർത്തുക;
		  /* Copy a vector pointer item. */
		അവസ്ഥ vpiVectorVal:
		  put->value.value.vector = vectordup(put->value.value.vector,
		                                      vpi_get(vpiSize, obj));
		  നിർത്തുക;
		  /* Copy a strength pointer item. */
		അവസ്ഥ vpiStrengthVal:
		  put->value.value.strength =
		      strengthdup(put->value.value.strength);
		  നിർത്തുക;
		  /* Everything thing else is already in the structure. */
		സ്ഥിരസ്ഥിതി:
		  നിർത്തുക;
	    }
	    put->flags = flags;
	    schedule_generic(put, dly, അസത്യം, സത്യം, സത്യം);
	    തിരികെ 0;
      }

      എങ്കിൽ (schedule_at_rosync()) {
            fprintf(stderr, "VPI error: attempted to put a value to "
			    "variable '%s' during a read-only synch "
			    "callback.\n", vpi_get_str(vpiName, obj));
            തിരികെ 0;
      }

      obj->vpi_put_മൂല്യം(vp, flags);

      തിരികെ 0;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle res = 0;

      എങ്കിൽ (ref == 0) {
	      // A few types can apply to a nil handle. These are ways
	      // that the current function can get started finding things.
	    തിരഞ്ഞെടുപ്പ് (type) {

		അവസ്ഥ vpiScope:
		    // The IEEE1364-2005 doesn't seem to allow this,
		    // but some users seem to think it's handy, so
		    // return the scope that contains this SysTfCall.
		  പ്രതിജ്ഞാ(vpip_cur_task);
		  res = vpip_cur_task->vpi_handle(vpiScope);
		  നിർത്തുക;

		അവസ്ഥ vpiSysTfCall:
		    // This is how VPI users get a first handle into
		    // the system. This is the handle of the system
		    // task/function call currently being executed.
		  എങ്കിൽ (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle(vpiSysTfCall, 0) "
				"-> %p (%s)\n", vpip_cur_task,
				vpip_cur_task->defn->info.tfname);
		  }
		  തിരികെ vpip_cur_task;

		സ്ഥിരസ്ഥിതി:
		  fprintf(stderr, "VPI error: vpi_handle(type=%d, ref=0).\n",
			  (പൂർണ്ണസംഖ്യ)type);
		  res = 0;
		  നിർത്തുക;
	    }

      } അല്ലെങ്കിൽ {

	    എങ്കിൽ (type == vpiSysTfCall) {
		  fprintf(stderr, "VPI error: vpi_handle(vpiSysTfCall, "
			  "ref!=0).\n");
		  തിരികെ 0;
	    }

	    res = ref->vpi_handle(type);
      }


      എങ്കിൽ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle(vpiScope, ref=%p) "
		    "-> %p\n", vpip_cur_task, ref);
      }

      തിരികെ res;
}

സ്ഥിതം vpiHandle vpip_make_udp_iterator()
{
// HERE: Add support for iterating over UDP definitions.
//       See 26.6.16 (page 400 in 1364-2005).
      തിരികെ 0;
}

/*
 * This function asks the object to return an iterator for
 * the specified reference. It is up to the iterate_ method to
 * allocate a properly formed iterator.
 */
സ്ഥിതം vpiHandle vpi_iterate_global(പൂർണ്ണസംഖ്യ type)
{
      തിരഞ്ഞെടുപ്പ് (type) {
	  അവസ്ഥ vpiInstance:
	    // fallthrough
	  അവസ്ഥ vpiModule:
	    // fallthrough
	  അവസ്ഥ vpiProgram:
	    // fallthrough
	  അവസ്ഥ vpiInterface:
	    // fallthrough
	  അവസ്ഥ vpiPackage:
	    തിരികെ vpip_make_വേര്_iterator(type);

	  അവസ്ഥ vpiUdpDefn:
	    തിരികെ vpip_make_udp_iterator();

	  അവസ്ഥ vpiUserSystf:
	    തിരികെ vpip_make_systf_iterator();
      }

      തിരികെ 0;
}

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle rtn = 0;

      പ്രതിജ്ഞാ(vpi_mode_flag != VPI_MODE_NONE);
      എങ്കിൽ (vpi_mode_flag == VPI_MODE_REGISTER) {
	    fprintf(stderr, "vpi error: vpi_iterate called during "
		    "vpi_register_systf. You can't do that!\n");
	    തിരികെ 0;
      }

      എങ്കിൽ (ref == 0)
	    rtn = vpi_iterate_global(type);
      അല്ലെങ്കിൽ
	    rtn = ref->vpi_iterate(type);

      എങ്കിൽ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_iterate(%d, %p) ->%s\n",
	    (പൂർണ്ണസംഖ്യ)type, ref, rtn ? "" : " (null)");
      }

      തിരികെ rtn;
}

vpiHandle vpi_handle_by_index(vpiHandle ref, PLI_INT32 idx)
{
      പ്രതിജ്ഞാ(ref);
      തിരികെ ref->vpi_index(idx);
}

സ്ഥിതം vpiHandle find_name(സ്ഥിരം അക്ഷരം *name, vpiHandle handle)
{
      vpiHandle rtn = 0;
      __vpiScope*ref = dynamic_cast<__vpiScope*>(handle);

      /* check module names */
      എങ്കിൽ (!strcmp(name, vpi_get_str(vpiName, handle)))
	    rtn = handle;

      /* brute force search for the name in all objects in this scope */
      വേണ്ടി (unsigned i = 0 ;  i < ref->intern.size() ;  i += 1) {
	      /* The standard says that since a port does not have a full
	       * name it cannot be found by name. Because of this we need
	       * to skip ports here so the correct handle can be located. */
	    എങ്കിൽ (vpi_get(vpiType, ref->intern[i]) == vpiPort) തുടരുക;
	    സ്ഥിരം അക്ഷരം *nm = vpi_get_str(vpiName, ref->intern[i]);
	    എങ്കിൽ (nm && !strcmp(name, nm)) {
		  rtn = ref->intern[i];
		  നിർത്തുക;
	    } അല്ലെങ്കിൽ എങ്കിൽ (vpi_get(vpiType, ref->intern[i]) == vpiMemory ||
	               vpi_get(vpiType, ref->intern[i]) == vpiNetArray) {
		  /* We need to iterate on the words */
		  vpiHandle word_i, word_h;
		  word_i = vpi_iterate(vpiMemoryWord, ref->intern[i]);
		  ആകുമ്പോൾ (word_i && (word_h = vpi_scan(word_i))) {
			nm = vpi_get_str(vpiName, word_h);
			എങ്കിൽ (nm && !strcmp(name, nm)) {
			      rtn = word_h;
			      vpi_free_object(word_i);
			      നിർത്തുക;
			}
		  }
	    }
	    /* found it yet? */
	    എങ്കിൽ (rtn) നിർത്തുക;
      }

      തിരികെ rtn;
}

// Find the end of the escaped identifier or simple identifier
സ്ഥിതം അക്ഷരം * find_rest(അക്ഷരം *name)
{
      അക്ഷരം *rest;
      എങ്കിൽ (*name == '\\') {
	    rest = strchr(name, ' ');
	    എങ്കിൽ (rest) {
		  *rest++ = 0; // The space is not part of the string
		    // There should be a '.' after the escaped ID if there is
		    // anything more. If it is missing add it to avoid a crash
		  എങ്കിൽ ((*rest != '.') && (*rest != 0)) {
			*--rest = '.';
			fprintf(stderr, "ERROR: Malformed scope string: \"%s\"", name);
		  }
		  എങ്കിൽ (*rest == 0) {
			rest = 0;
		  }
	    }
      } അല്ലെങ്കിൽ {
	    rest = strchr(name, '.');
      }
      തിരികെ rest;
}

സ്ഥിതം vpiHandle find_scope(സ്ഥിരം അക്ഷരം *name, vpiHandle handle, പൂർണ്ണസംഖ്യ depth)
{

      vpiHandle iter = handle==0
	    ? vpi_iterate(vpiModule, ശൂന്യ)
	    : vpi_iterate(vpiInternalScope, handle);

      vector<അക്ഷരം> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      അക്ഷരം*nm_first = &name_buf[0];
      അക്ഷരം*nm_rest = find_rest(nm_first);
      എങ്കിൽ (*nm_first == '\\') ++nm_first;
      എങ്കിൽ (nm_rest) {
	    *nm_rest++ = 0;
      }

      vpiHandle rtn = 0;
      vpiHandle hand;
      ആകുമ്പോൾ (iter && (hand = vpi_scan(iter))) {
	    സ്ഥിരം അക്ഷരം *nm = vpi_get_str(vpiName, hand);

	    എങ്കിൽ (strcmp(nm_first,nm)==0) {
		  എങ്കിൽ (nm_rest)
			rtn=find_scope(nm_rest, hand, depth+1);
		  അല്ലെങ്കിൽ
			rtn = hand;
	    }

	    /* found it yet ? */
	    എങ്കിൽ (rtn) {
		  vpi_free_object(iter);
		  നിർത്തുക;
	    }
      }

      തിരികെ rtn;
}

// Find the end of the first escaped identifier or simple identifier
സ്ഥിതം അക്ഷരം * find_next(അക്ഷരം *name)
{
      അക്ഷരം *next;
      എങ്കിൽ (*name == '\\') {
	    next = strchr(name, ' ');
	    എങ്കിൽ (next && *++next == 0) next = 0;
      } അല്ലെങ്കിൽ {
	    next = strchr(name, '.');
      }
      തിരികെ next;
}

vpiHandle vpi_handle_by_name(സ്ഥിരം അക്ഷരം *name, vpiHandle scope)
{
      vpiHandle hand;

      എങ്കിൽ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name(%s, %p) -->\n",
		    name, scope);
      }

	// Chop the name into path and base. For example, if the name
	// is "a.b.c", then nm_path becomes "a.b" and nm_base becomes
	// "c". If the name is "c" then nm_path is nil and nm_base is "c".
      vector<അക്ഷരം> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      അക്ഷരം*nm_path = &name_buf[0];
      അക്ഷരം*nm_base;

	// Check to see if we have an escaped identifier and if so search
	// the long way because a '.' could be in the escaped name.
      എങ്കിൽ (strchr(nm_path, '\\')) {
	    അക്ഷരം *next;
	    nm_base = nm_path;
	    ആകുമ്പോൾ ((next = find_next(nm_base))) {
		  nm_base = ++next;
	    }
	    എങ്കിൽ (nm_path == nm_base) {
		  nm_path = 0;
	    } അല്ലെങ്കിൽ {
		  *(nm_base-1) = 0;
	    }

	// If there is no escaped identifier then just look for the last '.'
      } അല്ലെങ്കിൽ {
	    nm_base = strrchr(nm_path, '.');
	    എങ്കിൽ (nm_base) {
		  *nm_base++ = 0;
	    } അല്ലെങ്കിൽ {
		  nm_base = nm_path;
		  nm_path = 0;
	    }
      }

      /* If scope provided, look in corresponding module; otherwise
       * traverse the hierarchy specified in name to find the leaf module
       * and try finding it there.
       */
      എങ്കിൽ (scope) {
	    /* Some implementations support either a module or a scope. */
	    തിരഞ്ഞെടുപ്പ് (vpi_get(vpiType, scope)) {
		അവസ്ഥ vpiScope:
	          hand = vpi_handle(vpiModule, scope);
	          നിർത്തുക;
		അവസ്ഥ vpiModule:
	          hand = scope;
	          നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		  എങ്കിൽ (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle_by_name: "
				"Scope is not a vpiScope or vpiModule\n");
		  }
	          // Use vpi_chk_error() here when it is implemented.
	          തിരികെ 0;
	    }

      } അല്ലെങ്കിൽ എങ്കിൽ (nm_path) {
	      // The name has a path, and no other scope handle was
	      // passed in. That suggests we are looking for "a.b.c"
	      // in the root scope. So convert "a.b" to a scope and
	      // start there to look for "c".
	    hand = find_scope(nm_path, ശൂന്യ, 0);
	    nm_path = 0;

      } അല്ലെങ്കിൽ {
	      // Special case: scope==<nil>, meaning we are looking in
	      // the root, and there is no path to the name, i.e. the
	      // string is "c" instead of "top.c". Try to find "c" as
	      // a scope and return that.
	    hand = find_scope(nm_base, ശൂന്യ, 0);
      }

      എങ്കിൽ (hand == 0) {
	    എങ്കിൽ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_handle_by_name: "
			  "Scope does not exist. Giving up.\n");
	    }

	    തിരികെ 0;
      }

	// If there is a path part, then use it to find the
	// scope. For example, if the full name is a.b.c, then
	// the nm_path string is a.b and we search for that
	// scope. If we find it, then set hand to that scope.
      എങ്കിൽ (nm_path) {
	    vpiHandle tmp = find_scope(nm_path, hand, 0);
	    ആകുമ്പോൾ (tmp == 0 && hand != 0) {
		  hand = vpi_handle(vpiScope, hand);
		  tmp = find_scope(nm_path, hand, 0);
	    }
	    hand = tmp;
      }

	// find_name() expects escaped identifiers to be stripped
      എങ്കിൽ (*nm_base == '\\') {
	    // Skip the \ at the beginning
	    ++nm_base;

	    // Drop the space at the end if it exists
	    അക്ഷരം *next;
	    എങ്കിൽ ((next = strchr(nm_base, ' '))) {
		  *next = 0;
	    }
      }

	// Now we have the correct scope, look for the item.
      vpiHandle out = find_name(nm_base, hand);

      എങ്കിൽ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name: DONE\n");
      }

      തിരികെ out;
}

// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected
// x2: Ptr to previous node
// x3: True when net is connected to output of previous node
tuple<തർക്കം, vvp_net_ptr_t, തർക്കം> check_connected_to_concat8_and_part_sa(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent) {

      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      ആകുമ്പോൾ (cur.ptr()) {
	    എങ്കിൽ (cur.ptr() == net2 && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
		    // Pass on the return value if found
		  തിരികെ {സത്യം, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
	    } അല്ലെങ്കിൽ എങ്കിൽ (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8_and_part_sa(cur.ptr()->out_, net2, cur);
		  എങ്കിൽ (get<0>(res)) {
			തിരികെ res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      തിരികെ {അസത്യം, vvp_net_ptr_t(nullptr, 0), അസത്യം};
}

// Check if net2 is connected to current_net through a net of vvp_fun_concat8s
// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected with concats and part_sas
// x2: Ptr to previous node
// x3: True when net is connected to output of x2
tuple<തർക്കം, vvp_net_ptr_t, തർക്കം> check_connected_to_concat8(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent)
{
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);
	// For everything connected
      ആകുമ്പോൾ (cur.ptr()) {
	      // Check if it's a concat8
	    എങ്കിൽ (cur.ptr() == net2) {
		  തിരികെ {സത്യം, vvp_net_ptr_t(nullptr, 0), അസത്യം};
	    } അല്ലെങ്കിൽ എങ്കിൽ (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8(cur.ptr()->out_, net2, cur);
		  എങ്കിൽ (get<0>(res)) {
			എങ്കിൽ (!get<1>(res).ptr() && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
			      // `parent` is the last part_sa node in front of our destination
			      // We want to place the intermod node here
			      തിരികെ {സത്യം, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
			}
			തിരികെ res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      തിരികെ {അസത്യം, vvp_net_ptr_t(nullptr, 0), അസത്യം};
}

// Used to get intermodpath for two ports
vpiHandle vpi_handle_multi(PLI_INT32 type,
                           vpiHandle ref1,
                           vpiHandle ref2)
{
      എങ്കിൽ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_multi(%d, %p, %p) -->\n",
	            type, ref1, ref2);
      }

      എങ്കിൽ (type != vpiInterModPath) {
	    fprintf(stderr, "sorry: vpi_handle_multi currently supports"
	            "only vpiInterModPath\n");
	    തിരികെ nullptr;
      }

	// Indicates whether port1 refers to a single bit
      തർക്കം port1_has_index = അസത്യം;
      പൂർണ്ണസംഖ്യ port1_bit_index = 0;
      vpiPortBitInfo* port1_bit = dynamic_cast<vpiPortBitInfo*>(ref1);

      എങ്കിൽ (port1_bit) {
	      // Get the bit index
	    port1_has_index = സത്യം;
	    port1_bit_index = vpi_get(vpiBit, port1_bit);
	      // Update the ref1 to point to the base port
	    ref1 = vpi_handle(vpiParent, port1_bit);
      }

	// Indicates whether port2 refers to a single bit
      തർക്കം port2_has_index = അസത്യം;
      പൂർണ്ണസംഖ്യ port2_bit_index = 0;
      vpiPortBitInfo* port2_bit = dynamic_cast<vpiPortBitInfo*>(ref2);

      എങ്കിൽ (port2_bit) {
	      // Get the bit index
	    port2_has_index = സത്യം;
	    port2_bit_index = vpi_get(vpiBit, port2_bit);
	      // Update the ref1 to point to the base port
	    ref2 = vpi_handle(vpiParent, port2_bit);
      }

      vpiPortInfo* port1 = dynamic_cast<vpiPortInfo*>(ref1);

      എങ്കിൽ (!port1) {
	    fprintf(stderr, "sorry: second argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    തിരികെ nullptr;
      }

      vpiPortInfo* port2 = dynamic_cast<vpiPortInfo*>(ref2);

      എങ്കിൽ (!port2) {
	    fprintf(stderr, "sorry: third argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    തിരികെ nullptr;
      }

	// Get the names of both ports
      std::string port1_name(vpi_get_str(vpiName, ref1));
      std::string port2_name(vpi_get_str(vpiName, ref2));

	// If both ports are vpiOutput, we have to reassign the __vpiSignal from port1
	// to port2 because otherwise the non-delayed version of the signal is dumped
	// even tho the intermodpath is correctly inserted
      __vpiSignal* output_signal = nullptr;

      എങ്കിൽ (port1->get_direction() == vpiOutput && port2->get_direction() == vpiOutput) {
	    vpiHandle scope_port2 = vpi_handle(vpiScope, ref2);
	    പ്രതിജ്ഞാ(scope_port2);

	      // Iterate over nets in the scope of port2
	    vpiHandle net_i = vpi_iterate(vpiNet, scope_port2) ;
	    vpiHandle net;

	    ആകുമ്പോൾ ((net = vpi_scan(net_i)) != ശൂന്യ) {
		  std::string net_name(vpi_get_str(vpiName, net));

		    // Compare whether the net matches with the port name
		  എങ്കിൽ (net_name == port2_name) {
			output_signal = dynamic_cast<__vpiSignal*>(net);
		  }
	    }
      }

      vvp_net_t* net1 = port1->get_port();
      vvp_net_t* net2 = port2->get_port();

      എങ്കിൽ (net1 == nullptr || net2 == nullptr) {
	    fprintf(stderr, "Error: Could not find net. "
		    "Did you run iverilog with '-ginterconnect'?\n");
	    തിരികെ nullptr;
      }

      എങ്കിൽ (net1 == net2) {
	    fprintf(stderr, "Error: Net for both ports is the same. "
		    "Did you pass the same port twice?\n");
	    തിരികെ nullptr;
      }

      എങ്കിൽ (!dynamic_cast<vvp_fun_buft*>(net1->fun)) {
	    fprintf(stderr, "Error: functor of net1 must be"
		    "vvp_fun_buft\n");
	    തിരികെ nullptr;
      }

      എങ്കിൽ (!dynamic_cast<vvp_fun_buft*>(net2->fun)) {
	    fprintf(stderr, "Error: functor of net2 must be"
		    "vvp_fun_buft\n");
	    തിരികെ nullptr;
      }

	// If port1 is actually a port bit, we have to get to the correct vvp_fun_part
	// after which we insert the intermodpath delay
      എങ്കിൽ (port1_has_index) {
	    vvp_net_ptr_t* net1_ptr = &net1->out_;

	      // Search for part selects connected to port1
	    vvp_net_t* current_net = net1_ptr->ptr();

	    ആകുമ്പോൾ (current_net) {
		  സ്ഥിരം vvp_fun_part* part = dynamic_cast<vvp_fun_part*>(current_net->fun);

		    // Its a part select!
		  എങ്കിൽ (part) {
			  // Is it the correct part select?
			എങ്കിൽ (part->get_base() == (unsigned)port1_bit_index) {
			      പ്രതിജ്ഞാ(part->get_wid() == 1);
			      net1 = current_net; // Replace net1 as this is our new start point
			      നിർത്തുക;
			}
		  }

		  current_net = current_net->port[0].ptr(); // BUFT has only one input, index 0
	    }
      }

	// Iterate over all nodes connected to port1
      vvp_net_ptr_t cur = net1->out_;
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      ആകുമ്പോൾ (cur.ptr()) {
	      // Either port2 is directly connected to port1
	      // Or in the second case port2 is indirectly connected
	      // to port1 through a net of concat8s
	    തർക്കം is_connected, is_output;
	    vvp_net_ptr_t previous_കണ്ണി;
	    tie(is_connected, previous_കണ്ണി, is_output) = port2_has_index
		  ? check_connected_to_concat8(cur, net2, vvp_net_ptr_t(net1, 0))
		  : check_connected_to_concat8_and_part_sa(cur, net2, vvp_net_ptr_t(net1, 0));
	    എങ്കിൽ ( (!port2_has_index && cur.ptr() == net2) || is_connected ) {
		  vvp_net_t*new_net = പുതിയ vvp_net_t;

		    // Create new node with intermodpath and connect port2 to it
		  പൂർണ്ണസംഖ്യ width = 1; // TODO
		  vvp_fun_intermodpath*obj = പുതിയ vvp_fun_intermodpath(new_net, width);

		  vvp_net_t* output_net = net1;
		  എങ്കിൽ (is_connected) {
			എങ്കിൽ (is_output) {
			      output_net = previous_കണ്ണി.ptr(); // net2 is direct output of part_sa
			      cur = output_net->out_;
			      prev = vvp_net_ptr_t(nullptr, 0);
			} അല്ലെങ്കിൽ {
			      prev = previous_കണ്ണി;
			      എങ്കിൽ (prev.ptr()) {
				    cur = prev.ptr()->port[prev.port()];
			      }
			}
		  }

		  new_net->fun = obj;
		  new_net->out_ = cur;

		    // Port2 is in the middle of the list
		    // Insert intermodpath before port2 and keep everything else intact
		  എങ്കിൽ (prev.ptr()) {
			prev.ptr()->port[prev.port()] = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; // Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		    // Port2 is first in list
		    // Insert intermodpath before port2 and keep everything else intact
		  } അല്ലെങ്കിൽ {
			output_net->out_ = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; //  Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		  }

		    // If both ports are vpiOutput and port2 is not a vector,
		    // we have to reassign the __vpiSignal so that the delayed
		    // values get dumped
		  എങ്കിൽ (output_signal && !port2_has_index) {
			net2->fil = net1->fil;
			net1->fil = nullptr;
			output_signal->കണ്ണി = net2;
		  }

		    // Create the VPI intermodpath object
		  __vpiInterModPath* intermodpath = vpip_make_intermodpath(new_net, port1, port2);
		  intermodpath->intermodpath = obj;

		    // Finally done, return the intermodpath object
		  തിരികെ intermodpath;
	    }

	    prev = cur;
	    cur = cur.ptr()->port[cur.port()]; // Next net in linked list
      }

      fprintf(stderr, "VPI error: Could not insert intermodpath!\n");
      fprintf(stderr, "\tport1 = %s, port1_has_index = %d, port1_bit_index = %d\n", port1_name.c_str(), port1_has_index, port1_bit_index);
      fprintf(stderr, "\tport2 = %s, port2_has_index = %d, port2_bit_index = %d\n", port2_name.c_str(), port2_has_index, port2_bit_index);
      തിരികെ nullptr;
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



ശൂന്യം vpi_get_delays(vpiHandle വ്യഞ്ജകം, p_vpi_delay delays)
{
      പ്രതിജ്ഞാ(വ്യഞ്ജകം);
      പ്രതിജ്ഞാ(delays);

      വ്യഞ്ജകം->vpi_get_delays(delays);

      എങ്കിൽ (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_get_delays(%p, %p) -->\n", വ്യഞ്ജകം, delays);
      }
}


ശൂന്യം vpi_put_delays(vpiHandle വ്യഞ്ജകം, p_vpi_delay delays)
{
      പ്രതിജ്ഞാ(വ്യഞ്ജകം  );
      പ്രതിജ്ഞാ(delays );

      വ്യഞ്ജകം->vpi_put_delays(delays);

      എങ്കിൽ (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_put_delays(%p, %p) -->\n", വ്യഞ്ജകം, delays);
      }
}








ബാഹ്യ "C" PLI_INT32 vpi_vprintf(സ്ഥിരം അക്ഷരം*fmt, va_list ap)
{
      തിരികെ vpi_mcd_vprintf(1, fmt, ap);
}

ബാഹ്യ "C" PLI_INT32 vpi_printf(സ്ഥിരം അക്ഷരം *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      പൂർണ്ണസംഖ്യ r = vpi_mcd_vprintf(1, fmt, ap);
      va_end(ap);
      തിരികെ r;
}

ബാഹ്യ "C" PLI_INT32 vpi_flush(ശൂന്യം)
{
      തിരികെ vpi_mcd_flush(1);
}


ബാഹ്യ "C" PLI_INT32 vpi_sim_vcontrol(പൂർണ്ണസംഖ്യ operation, va_list ap)
{
      long diag_സന്ദേശം;

      തിരഞ്ഞെടുപ്പ് (operation) {
	  അവസ്ഥ vpiFinish:
	  അവസ്ഥ __ivl_legacy_vpiFinish:
            diag_സന്ദേശം = va_arg(ap, PLI_INT32);
	    schedule_finish(diag_സന്ദേശം);
	    നിർത്തുക;

	  അവസ്ഥ vpiStop:
	  അവസ്ഥ __ivl_legacy_vpiStop:
            diag_സന്ദേശം = va_arg(ap, PLI_INT32);
	    schedule_stop(diag_സന്ദേശം);
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Unsupported VPI control operation %d.\n", operation);
	    തിരികെ 0;
      }
      തിരികെ 1;
}

ബാഹ്യ "C" PLI_INT32 vpi_sim_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      പൂർണ്ണസംഖ്യ r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      തിരികെ r;
}

ബാഹ്യ "C" PLI_INT32 vpi_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      പൂർണ്ണസംഖ്യ r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      തിരികെ r;
}

/*
 * This routine calculated the return value for $clog2.
 * It is easier to do it here vs trying to to use the VPI interface.
 */
ബാഹ്യ "C" s_vpi_vecval vpip_calc_clog2(vpiHandle arg)
{
      s_vpi_vecval rtn;
      s_vpi_മൂല്യം val;
      vvp_vector4_t vec4;
      തർക്കം is_neg = അസത്യം;  // At this point only a real can be negative.

	/* Get the value as a vvp_vector4_t. */
      val.format = vpiObjTypeVal;
      vpi_get_മൂല്യം(arg, &val);
      എങ്കിൽ (val.format == vpiRealVal) {
	    vpi_get_മൂല്യം(arg, &val);
	      /* All double values can be represented in 1024 bits. */
	    vec4 = vvp_vector4_t(1024, val.value.real);
	    എങ്കിൽ (val.value.real < 0) is_neg = സത്യം;
      } അല്ലെങ്കിൽ {
	    val.format = vpiVectorVal;
	    vpi_get_മൂല്യം(arg, &val);
	    unsigned wid = vpi_get(vpiSize, arg);
	    vec4 = vvp_vector4_t(wid, BIT4_0);
	    വേണ്ടി (unsigned idx=0; idx < wid; idx += 1) {
		PLI_INT32 aval = val.value.vector[idx/32].aval;
		PLI_INT32 bval = val.value.vector[idx/32].bval;
		aval >>= idx % 32;
		bval >>= idx % 32;
		പൂർണ്ണസംഖ്യ bitmask = (aval&1) | ((bval<<1)&2);
		vvp_bit4_t bit = scalar_to_bit4(bitmask);
		vec4.set_bit(idx, bit);
	    }
      }

      എങ്കിൽ (vec4.has_xz()) {
	    rtn.aval = rtn.bval = 0xFFFFFFFFU;  /* Set to 'bx. */
	    തിരികെ rtn;
      }

      vvp_vector2_t vec2(vec4);

      എങ്കിൽ (is_neg) vec2.trim_neg(); /* This is a special trim! */
      അല്ലെങ്കിൽ vec2.trim(); /* This makes less work shifting. */

	/* Calculate the clog2 result. */
      PLI_INT32 res = 0;
      എങ്കിൽ (!vec2.is_zero()) {
	    vec2 -= vvp_vector2_t(1, vec2.size());
	    ആകുമ്പോൾ(!vec2.is_zero()) {
		  res += 1;
		  vec2 >>= 1;
	    }
      }

      rtn.aval = res;
      rtn.bval = 0;
      തിരികെ rtn;
}

/*
 * This routine provides the information needed to implement $countdrivers.
 * It is done here for performance reasons - interrogating the drivers
 * individually via the VPI interface would be much slower.
 */
ബാഹ്യ "C" ശൂന്യം vpip_എണ്ണം_drivers(vpiHandle ref, unsigned idx,
                                   unsigned counts[4])
{
      ഘടന __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      പ്രതിജ്ഞാ(rfp);
      rfp->കണ്ണി->എണ്ണം_drivers(idx, counts);
}

#എങ്കിൽ defined(__MINGW32__) || defined (__CYGWIN__)
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
    .get_മൂല്യം                  = vpi_get_മൂല്യം,
    .put_മൂല്യം                  = vpi_put_മൂല്യം,
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
    .എണ്ണം_drivers              = vpip_എണ്ണം_drivers,
    .format_strength            = vpip_format_strength,
    .format_pretty              = vpip_format_pretty,
    .make_systf_system_defined  = vpip_make_systf_system_defined,
    .mcd_rawwrite               = vpip_mcd_rawwrite,
    .set_return_മൂല്യം           = vpip_set_return_മൂല്യം,
};
#endif
