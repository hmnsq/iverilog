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

using নামাকাশ std;
vpi_mode_t vpi_mode_flag = VPI_MODE_NONE;
FILE*vpi_trace = 0;

স্থির s_vpi_vlog_info  vpi_vlog_info;
স্থির s_vpi_error_info vpip_last_error = { 0, 0, 0, 0, 0, 0, 0 };

ধ্রুৱ ৱর্ণ* direction_as_string(পূর্ণাঙ্ক dir)
{
      ৱিকল্প (dir) {
	  পক্ষ vpi_প্ৰবেশ:
	    ঘূৰাই "input";
	  পক্ষ vpiOutput:
	    ঘূৰাই "output";
	  পক্ষ vpiInout:
	    ঘূৰাই "inout";
	  পক্ষ vpiMixedIO:
	    ঘূৰাই "mixed io";
	  পক্ষ vpiNoDirection:
	    ঘূৰাই "no direction";
	  স্ৱভাৱ:
	    ঘূৰাই "INVALID-DIRECTION";
      }
}

__vpiHandle::~__vpiHandle()
{ }

পূর্ণাঙ্ক __vpiHandle::vpi_get(পূর্ণাঙ্ক)
{ ঘূৰাই vpiUndefined; }

int64_t __vpiHandle::vpi_get64(পূর্ণাঙ্ক code)
{ ঘূৰাই (code == _vpiNexusId) ? 0 : vpiUndefined; }

ৱর্ণ* __vpiHandle::vpi_get_str(পূর্ণাঙ্ক)
{ ঘূৰাই 0; }

শূন্য __vpiHandle::vpi_get_মান(p_vpi_মান)
{ }

vpiHandle __vpiHandle::vpi_put_মান(p_vpi_মান, পূর্ণাঙ্ক)
{ ঘূৰাই 0; }

vpiHandle __vpiHandle::vpi_handle(পূর্ণাঙ্ক)
{ ঘূৰাই 0; }

vpiHandle __vpiHandle::vpi_iterate(পূর্ণাঙ্ক)
{ ঘূৰাই 0; }

vpiHandle __vpiHandle::vpi_index(পূর্ণাঙ্ক)
{ ঘূৰাই 0; }

শূন্য __vpiHandle::vpi_get_delays(p_vpi_delay)
{ }

শূন্য __vpiHandle::vpi_put_delays(p_vpi_delay)
{ }

__vpiBaseVar::__vpiBaseVar(__vpiScope*scope, ধ্রুৱ ৱর্ণ*name, vvp_net_t*net)
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
স্থির পূর্ণাঙ্ক suppress_free(vpiHandle)
{ ঘূৰাই 1; }
__vpiHandle::free_object_fun_t __vpiHandle::free_object_fun(শূন্য)
{ ঘূৰাই &suppress_free; }

/*
 * The vpip_string function creates a constant string from the pass
 * input. This constant string is permanently allocated from an
 * efficient string buffer store.
 */
সংরচনা vpip_string_chunk {
      সংরচনা vpip_string_chunk*next;
      ৱর্ণ data[64*1024 - পরিমাণ (সংরচনা vpip_string_chunk*)];
};

unsigned vpip_size(ধ্রুৱ __vpiSignal *sig)
{
      ঘূৰাই abs(sig->msb.get_মান() - sig->lsb.get_মান()) + 1;
}

__vpiScope* vpip_scope(__vpiSignal*sig)
{
      যদি (sig->is_netarray)
	    ঘূৰাই static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      অন্যথা
	    ঘূৰাই sig->within.scope;
}

__vpiScope* vpip_scope(__vpiRealVar*sig)
{
      যদি (sig->is_netarray)
	    ঘূৰাই static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      অন্যথা
	    ঘূৰাই sig->within.scope;
}

vpiHandle vpip_module(__vpiScope*scope)
{
      যেতিয়ালৈ(scope && scope->get_type_code() != vpiModule) {
	    scope = scope->scope;
      }
      প্রতিজ্ঞা(scope);
      ঘূৰাই scope;
}

ধ্রুৱ ৱর্ণ *vpip_string(ধ্রুৱ ৱর্ণ*str)
{
      স্থির vpip_string_chunk first_chunk = {0, {0}};
      স্থির vpip_string_chunk*chunk_list = &first_chunk;
      স্থির unsigned chunk_fill = 0;

      unsigned len = strlen(str);
      প্রতিজ্ঞা( (len+1) <= পরিমাণ chunk_list->data );

      যদি ( (len+1) > (পরিমাণ chunk_list->data - chunk_fill) ) {
	    vpip_string_chunk*tmp = নূতন vpip_string_chunk;
	    tmp->next = chunk_list;
	    chunk_list = tmp;
	    chunk_fill = 0;
      }

      ৱর্ণ*res = chunk_list->data + chunk_fill;
      chunk_fill += len + 1;

      strcpy(res, str);
      ঘূৰাই res;
}

স্থির unsigned hash_string(ধ্রুৱ ৱর্ণ*text)
{
      unsigned h = 0;

      যেতিয়ালৈ (*text) {
	    h = (h << 4) ^ (h >> 28) ^ (unsigned)*text;
	    text += 1;
      }
      ঘূৰাই h;
}

ধ্রুৱ ৱর্ণ* vpip_name_string(ধ্রুৱ ৱর্ণ*text)
{
      ধ্রুৱ unsigned HASH_SIZE = 4096;
      স্থির ধ্রুৱ ৱর্ণ*hash_table[HASH_SIZE] = {0};

      unsigned hash_মান = hash_string(text) % HASH_SIZE;

	/* If we easily find the string in the hash table, then return
	   that and be done. */
      যদি (hash_table[hash_মান]
	  && (strcmp(hash_table[hash_মান], text) == 0)) {
	    ঘূৰাই hash_table[hash_মান];
      }

	/* The existing hash entry is not a match. Replace it with the
	   newly allocated value, and return the new pointer as the
	   result to the add. */
      ধ্রুৱ ৱর্ণ*res = vpip_string(text);
      hash_table[hash_মান] = res;

      ঘূৰাই res;
}

PLI_INT32 vpi_chk_error(p_vpi_error_info info)
{
      যদি (vpip_last_error.state == 0)
	    ঘূৰাই 0;

      info->state = vpip_last_error.state;
      info->level = vpip_last_error.level;
      info->message = vpip_last_error.message;
      info->product = vpi_vlog_info.product;
      info->code = (ৱর্ণ *)"";
      info->file = 0;
      info->সাৰি = 0;

      ঘূৰাই info->level;
}

PLI_INT32 vpi_compare_objects(vpiHandle obj1, vpiHandle obj2)
{
      প্রতিজ্ঞা(obj1);
      প্রতিজ্ঞা(obj2);

	// Does this actually work for all cases?
      যদি (obj1 != obj2) ঘূৰাই 0;
      অন্যথা ঘূৰাই 1;
}

/*
 * Copy the internal information to the data structure. Do not free or
 * change the tfname/user_data since they are a pointer to the real
 * string/data values. We also support passing a task or function handle
 * instead of just a handle to a vpiUserSystf.
 */
শূন্য vpi_get_systf_info(vpiHandle ref, p_vpi_systf_data data)
{
      সংরচনা __vpiUserSystf* rfp = dynamic_cast<__vpiUserSystf*>(ref);
      যদি (rfp == 0) {
	    সংরচনা __vpiSysTaskCall*call = dynamic_cast<__vpiSysTaskCall*>(ref);
	    প্রতিজ্ঞা(call);
	    rfp = call->defn;
      }

	/* Assert that vpiUserDefn is true! */
      প্রতিজ্ঞা(rfp->is_user_defn);

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
সংরচনা __vpiSysTaskCall*vpip_cur_task = 0;

PLI_INT32 vpi_free_object(vpiHandle ref)
{
      পূর্ণাঙ্ক rtn;

      যদি (vpi_trace) {
	    fprintf(vpi_trace, "vpi_free_object(%p)", ref);
	    fflush(vpi_trace);
      }

      প্রতিজ্ঞা(ref);
      __vpiHandle::free_object_fun_t fun = ref->free_object_fun();
      rtn = fun (ref);

      যদি (vpi_trace)
	    fprintf(vpi_trace, " --> %d\n", rtn);

      ঘূৰাই rtn;
}

PLI_INT32 vpi_release_handle(vpiHandle ref)
{
	// Since SystemVerilog vpi_free_object() has been
	// renamed vpi_release_handle(), and thus
	// vpi_free_object() has been deprecated.
      ঘূৰাই vpi_free_object(ref);
}

স্থির পূর্ণাঙ্ক vpip_get_global(পূর্ণাঙ্ক property)
{
      ৱিকল্প (property) {

	  পক্ষ vpiTimeUnit:
	  পক্ষ vpiTimePrecision:
	    ঘূৰাই vpip_get_time_precision();

	  স্ৱভাৱ:
	    fprintf(stderr, "vpi error: bad global property: %d\n", property);
	    প্রতিজ্ঞা(0);
	    ঘূৰাই vpiUndefined;
      }
}

স্থির ধ্রুৱ ৱর্ণ* vpi_property_str(PLI_INT32 code)
{
      স্থির ৱর্ণ buf[32];
      ৱিকল্প (code) {
	  পক্ষ vpiConstType:
	    ঘূৰাই "vpiConstType";
	  পক্ষ vpiName:
	    ঘূৰাই "vpiName";
	  পক্ষ vpiFullName:
	    ঘূৰাই "vpiFullName";
	  পক্ষ vpiTimeUnit:
	    ঘূৰাই "vpiTimeUnit";
	  পক্ষ vpiTimePrecision:
	    ঘূৰাই "vpiTimePrecision";
          পক্ষ vpiSize:
	    ঘূৰাই "vpiSize";
          পক্ষ _vpiNexusId:
	    ঘূৰাই "_vpiNexusId";
	  স্ৱভাৱ:
	    snprintf(buf, পরিমাণ(buf), "%d", (পূর্ণাঙ্ক)code);
      }
      ঘূৰাই buf;
}

ধ্রুৱ ৱর্ণ* vpi_type_as_string(PLI_INT32 code)
{
      স্থির ৱর্ণ buf[32];
      ৱিকল্প (code) {
	  পক্ষ vpiArrayType:
	    ঘূৰাই "vpiArrayType";
	  পক্ষ vpiBitVar:
	    ঘূৰাই "vpiBitVar";
	  পক্ষ vpiByteVar:
	    ঘূৰাই "vpiByteVar";
	  পক্ষ vpiClassVar:
	    ঘূৰাই "vpiClassVar";
	  পক্ষ vpiConstant:
	    ঘূৰাই "vpiConstant";
	  পক্ষ vpiEnumTypespec:
	    ঘূৰাই "vpiEnumTypespec";
	  পক্ষ vpiFunction:
	    ঘূৰাই "vpiFunction";
	  পক্ষ vpiGenScope:
	    ঘূৰাই "vpiGenScope";
	  পক্ষ vpiIntVar:
	    ঘূৰাই "vpiIntVar";
	  পক্ষ vpiIntegerVar:
	    ঘূৰাই "vpiIntegerVar";
	  পক্ষ vpiIterator:
	    ঘূৰাই "vpiIterator";
	  পক্ষ vpiLongIntVar:
	    ঘূৰাই "vpiLongIntVar";
	  পক্ষ vpiMemory:
	    ঘূৰাই "vpiMemory";
	  পক্ষ vpiMemoryWord:
	    ঘূৰাই "vpiMemoryWord";
	  পক্ষ vpiModule:
	    ঘূৰাই "vpiModule";
	  পক্ষ vpiNamedBegin:
	    ঘূৰাই "vpiNamedBegin";
	  পক্ষ vpiNamedEvent:
	    ঘূৰাই "vpiNamedEvent";
	  পক্ষ vpiNamedFork:
	    ঘূৰাই "vpiNamedFork";
	  পক্ষ vpiPackage:
	    ঘূৰাই "vpiPackage";
	  পক্ষ vpiPathTerm:
	    ঘূৰাই "vpiPathTerm";
	  পক্ষ vpiPort:
	    ঘূৰাই "vpiPort";
	  পক্ষ vpiNet:
	    ঘূৰাই "vpiNet";
	  পক্ষ vpiNetArray:
	    ঘূৰাই "vpiNetArray";
	  পক্ষ vpiNetBit:
	    ঘূৰাই "vpiNetBit";
	  পক্ষ vpiParameter:
	    ঘূৰাই "vpiParameter";
	  পক্ষ vpiPartSelect:
	    ঘূৰাই "vpiPartSelect";
	  পক্ষ vpiRealVar:
	    ঘূৰাই "vpiRealVar";
	  পক্ষ vpiReg:
	    ঘূৰাই "vpiReg";
	  পক্ষ vpiRegBit:
	    ঘূৰাই "vpiRegBit";
	  পক্ষ vpiShortIntVar:
	    ঘূৰাই "vpiShortIntVar";
	  পক্ষ vpiStringVar:
	    ঘূৰাই "vpiStringVar";
	  পক্ষ vpiSysFuncCall:
	    ঘূৰাই "vpiSysFuncCall";
	  পক্ষ vpiSysTaskCall:
	    ঘূৰাই "vpiSysTaskCall";
	  পক্ষ vpiTask:
	    ঘূৰাই "vpiTask";
	  পক্ষ vpiTimeVar:
	    ঘূৰাই "vpiTimeVar";
	  পক্ষ vpiUserSystf:
	    ঘূৰাই "vpiUserSystf";
	  স্ৱভাৱ:
	    snprintf(buf, পরিমাণ(buf), "%d", (পূর্ণাঙ্ক)code);
      }
      ঘূৰাই buf;
}

PLI_INT32 vpi_get(পূর্ণাঙ্ক property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      যদি (property == _vpiDelaySelection) ঘূৰাই vpip_delay_selection;

      যদি (ref == 0)
	    ঘূৰাই vpip_get_global(property);

      যদি (property == vpiType) {
	    যদি (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

	    যদি (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  ঘূৰাই vpiNetArray;
	    অন্যথা
		  ঘূৰাই ref->get_type_code();
      }

      পূর্ণাঙ্ক res = ref->vpi_get(property);

      যদি (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get(%s, %p) --> %d\n",
		    vpi_property_str(property), ref, res);
      }

      ঘূৰাই res;
}

PLI_INT64 vpi_get64(পূর্ণাঙ্ক property, vpiHandle ref)
{
	// For now we only support getting the nexus id.
      যদি ((ref == 0) || (property != _vpiNexusId)) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) is not currently supported.\n",
		    vpi_property_str(property), ref);
	    ঘূৰাই vpiUndefined;
      }

      int64_t res = ref->vpi_get64(property);

      যদি (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) --> %" PRId64 "\n",
		    vpi_property_str(property), ref, res);
      }

      ঘূৰাই res;
}

ৱর্ণ* vpi_get_str(PLI_INT32 property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      যদি (property == _vpiDelaySelection) {
	    ৱিকল্প (vpip_delay_selection) {
		পক্ষ _vpiDelaySelMinimum:
		  ঘূৰাই simple_set_rbuf_str("MINIMUM");
		পক্ষ _vpiDelaySelTypical:
		  ঘূৰাই simple_set_rbuf_str("TYPICAL");
		পক্ষ _vpiDelaySelMaximum:
		  ঘূৰাই simple_set_rbuf_str("MAXIMUM");
		স্ৱভাৱ:
		  প্রতিজ্ঞা(0);
	    }
      }

      যদি (ref == 0) {
	    fprintf(stderr, "vpi error: vpi_get_str(%s, 0) called "
		    "with null vpiHandle.\n", vpi_property_str(property));
	    ঘূৰাই 0;
      }

      যদি (property == vpiType) {
	    যদি (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

            PLI_INT32 type;
	    যদি (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  type = vpiNetArray;
	    অন্যথা
		  type = ref->get_type_code();
	    ঘূৰাই (ৱর্ণ *)vpi_type_as_string(type);
      }

      ৱর্ণ*res = ref->vpi_get_str(property);

      যদি (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get_str(%s, %p) --> %s\n",
		    vpi_property_str(property), ref, res? res : "<NULL>");
      }

      ঘূৰাই res;
}

__vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj)
{
      ধ্রুৱ সংরচনা __vpiSysTaskCall*task;
      সংরচনা __vpiSignal*সঙ্কেত;
      সংরচনা __vpiRealVar*real;
      ধ্রুৱ __vpiNamedEvent*event;

      ৱিকল্প (obj->get_type_code()) {
	  পক্ষ vpiSysTaskCall:
	    task = dynamic_cast<__vpiSysTaskCall*>(obj);
	    ঘূৰাই task->scope;

	  পক্ষ vpiModule:
	    ঘূৰাই dynamic_cast<__vpiScope*>(obj);

	  পক্ষ vpiNet:
	  পক্ষ vpiReg:
	  পক্ষ vpiIntegerVar:
	  পক্ষ vpiBitVar:
	  পক্ষ vpiByteVar:
	  পক্ষ vpiShortIntVar:
	  পক্ষ vpiIntVar:
	  পক্ষ vpiLongIntVar:
	    সঙ্কেত = dynamic_cast<__vpiSignal*>(obj);
	    ঘূৰাই vpip_scope(সঙ্কেত);

	  পক্ষ vpiRealVar:
	    real = dynamic_cast<__vpiRealVar*>(obj);
	    ঘূৰাই vpip_scope(real);

	  পক্ষ vpiNamedEvent:
	    event = dynamic_cast<__vpiNamedEvent*>(obj);
	    ঘূৰাই event->get_scope();

	  পক্ষ vpiMemory:
	  পক্ষ vpiMemoryWord:
	  পক্ষ vpiPartSelect:
	    ঘূৰাই dynamic_cast<__vpiScope*>(obj->vpi_handle(vpiScope));

	  স্ৱভাৱ:
	    fprintf(stderr, "ERROR: vpip_scope_from_handle called with "
		    "object handle type=%d\n", obj->get_type_code());
	    প্রতিজ্ঞা(0);
	    ঘূৰাই 0;
      }
}

পূর্ণাঙ্ক vpip_time_units_from_handle(vpiHandle obj)
{
      যদি (obj == 0)
	    ঘূৰাই vpip_get_time_precision();

      ধ্রুৱ __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      যদি (scope == 0)
	    ঘূৰাই vpip_get_time_precision();

      ঘূৰাই scope->time_units;
}

পূর্ণাঙ্ক vpip_time_precision_from_handle(vpiHandle obj)
{
      যদি (obj == 0)
	    ঘূৰাই vpip_get_time_precision();

      ধ্রুৱ __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      যদি (scope == 0)
	    ঘূৰাই vpip_get_time_precision();

      ঘূৰাই scope->time_precision;
}

দ্ৱিগুণ vpip_scaled_time_from_handle(vvp_time64_t time, vpiHandle obj)
{
      পূর্ণাঙ্ক scale = vpip_get_time_precision() -
	          vpip_time_units_from_handle(obj);
      যদি (scale >= 0)
	    ঘূৰাই (দ্ৱিগুণ)time * pow(10.0, scale);
      অন্যথা
	    ঘূৰাই (দ্ৱিগুণ)time / pow(10.0, -scale);
}

শূন্য vpi_get_time(vpiHandle obj, s_vpi_time*vp)
{
      vvp_time64_t time;

      প্রতিজ্ঞা(vp);

      time = schedule_simtime();

      ৱিকল্প (vp->type) {
          পক্ষ vpiSimTime:
	    vp->উপৰেৰ = (time >> 32) & 0xffffffff;
	    vp->নিচেৰ = time & 0xffffffff;
	    ভঙ্গ;

          পক্ষ vpiScaledRealTime:
	    vp->real = vpip_scaled_time_from_handle(time, obj);
	    ভঙ্গ;

          স্ৱভাৱ:
            fprintf(stderr, "vpi_get_time: unknown type: %d\n", (পূর্ণাঙ্ক)vp->type);
            প্রতিজ্ঞা(0);
	    ভঙ্গ;
      }
}

PLI_INT32 vpi_get_vlog_info(p_vpi_vlog_info vlog_info_p)
{
    যদি (vlog_info_p != 0) {
	  *vlog_info_p = vpi_vlog_info;
	  ঘূৰাই 1;
    } অন্যথা {
	  ঘূৰাই 0;
    }
}

শূন্য vpi_set_vlog_info(পূর্ণাঙ্ক argc, ৱর্ণ** argv)
{
    স্থির ৱর্ণ icarus_product[] = "Icarus Verilog";
    স্থির ৱর্ণ icarus_version[] = VERSION;
    vpi_vlog_info.product = icarus_product;
    vpi_vlog_info.version = icarus_version;
    vpi_vlog_info.argc    = argc;
    vpi_vlog_info.argv    = argv;

    স্থির ৱর্ণ trace_buf[1024];
    যদি (ধ্রুৱ ৱর্ণ*path = getenv("VPI_TRACE"))  {
	  যদি (!strcmp(path,"-"))
		vpi_trace = stdout;
	  অন্যথা {
		vpi_trace = fopen(path, "w");
		যদি (!vpi_trace) {
		      perror(path);
		      exit(1);
		}
		setvbuf(vpi_trace, trace_buf, _IOLBF, পরিমাণ(trace_buf));
	  }
    }
}

স্থির শূন্য vec4_get_মান_string(ধ্রুৱ vvp_vector4_t&word_val, unsigned width,
				  s_vpi_মান*vp)
{
      unsigned nchar = width / 8;
      unsigned tail = width % 8;

      ৱর্ণ*rbuf = static_cast<ৱর্ণ *>(need_result_buf(nchar + 1, RBUF_VAL));
      ৱর্ণ*cp = rbuf;

      যদি (tail > 0) {
	    ৱর্ণ char_val = 0;
	    বাবে (unsigned idx = width-tail; idx < width ;  idx += 1) {
		  vvp_bit4_t val = word_val.value(idx);
		  যদি (val == BIT4_1)
			char_val |= 1 << idx;
	    }

	    যদি (char_val != 0)
		  *cp++ = char_val;
	    অন্যথা যদি (cp != rbuf)
		  *cp++ = ' ';
      }

      বাবে (unsigned idx = 0 ;  idx < nchar ;  idx += 1) {
	    unsigned bit = (nchar - idx - 1) * 8;
	    ৱর্ণ char_val = 0;
	    বাবে (unsigned bdx = 0 ;  bdx < 8 ;  bdx += 1) {
		  vvp_bit4_t val = word_val.value(bit+bdx);
		  যদি (val == BIT4_1)
			char_val |= 1 << bdx;
	    }
	    // Ignore leading null-bytes and replace other null-bytes with space.
	    // The LRM is not entirely clear on how null bytes should be handled.
	    // This is the implementation chosen for iverilog.
	    যদি (char_val != 0)
		  *cp++ = char_val;
	    অন্যথা যদি (cp != rbuf)
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
শূন্য vpip_vec4_get_মান(ধ্রুৱ vvp_vector4_t&word_val, unsigned width,
			 তার্কিক signed_flag, s_vpi_মান*vp)
{
      ৱর্ণ *rbuf = 0;

      ৱিকল্প (vp->format) {
	  স্ৱভাৱ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector values.\n", (পূর্ণাঙ্ক)vp->format);
	    প্রতিজ্ঞা(0);

	  পক্ষ vpiSuppressVal:
	    ভঙ্গ;

	  পক্ষ vpiBinStrVal:
	    rbuf = static_cast<ৱর্ণ *>(need_result_buf(width+1, RBUF_VAL));
	    বাবে (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		  vvp_bit4_t bit = word_val.value(idx);
		  rbuf[width-idx-1] = vvp_bit4_to_ascii(bit);
	    }
	    rbuf[width] = 0;
	    vp->value.str = rbuf;
	    ভঙ্গ;

	  পক্ষ vpiOctStrVal: {
		unsigned hwid = ((width+2) / 3) + 1;
		rbuf = static_cast<ৱর্ণ *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_oct_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		ভঙ্গ;
	  }

	  পক্ষ vpiDecStrVal: {
// HERE need a better estimate.
		rbuf = static_cast<ৱর্ণ *>(need_result_buf(width+1, RBUF_VAL));
		vpip_vec4_to_dec_str(word_val, rbuf, width+1, signed_flag);
		vp->value.str = rbuf;
		ভঙ্গ;
	  }

	  পক্ষ vpiHexStrVal: {
		unsigned  hwid = ((width + 3) / 4) + 1;
		rbuf = static_cast<ৱর্ণ *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_hex_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		ভঙ্গ;
	  }

          পক্ষ vpiScalarVal: {
	        // scalars should be of size 1
	        প্রতিজ্ঞা(width == 1);
	        ৱিকল্প(word_val.value(0)) {
		    পক্ষ BIT4_0:
		      vp->value.scalar = vpi0;
		      ভঙ্গ;
	            পক্ষ BIT4_1:
                      vp->value.scalar = vpi1;
                      ভঙ্গ;
	            পক্ষ BIT4_X:
                      vp->value.scalar = vpiX;
                      ভঙ্গ;
	            পক্ষ BIT4_Z:
                      vp->value.scalar = vpiZ;
                      ভঙ্গ;
		}
                ভঙ্গ;
          }

	  পক্ষ vpiIntVal: {
		long val = 0;
		vvp_bit4_t pad = BIT4_0;
		যদি (signed_flag && word_val.size() > 0)
		      pad = word_val.value(word_val.size()-1);

		বাবে (unsigned idx = 0 ; idx < 8*পরিমাণ(val) ;  idx += 1) {
		      vvp_bit4_t val4 = pad;
		      যদি (idx < word_val.size())
			    val4 = word_val.value(idx);
		      যদি (val4 == BIT4_1)
			    val |= 1L << idx;
		}

		vp->value.integer = val;
		ভঙ্গ;
	  }

	  পক্ষ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiVectorVal;
	    // fallthrough
	  পক্ষ vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * পরিমাণ(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		বাবে (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      ৱিকল্প (word_val.value(idx)) {
			  পক্ষ BIT4_0:
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    ভঙ্গ;
			  পক্ষ BIT4_1:
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    ভঙ্গ;
			  পক্ষ BIT4_X:
			    op->aval |= (1 << idx % 32);
			    op->bval |= (1 << idx % 32);
			    ভঙ্গ;
			  পক্ষ BIT4_Z:
			    op->aval &= ~(1 << idx % 32);
			    op->bval |=  (1 << idx % 32);
			    ভঙ্গ;
		      }
		      যদি (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		ভঙ্গ;
	  }

	  পক্ষ vpiStringVal:
		vec4_get_মান_string(word_val, width, vp);
		ভঙ্গ;

	  পক্ষ vpiRealVal:
		vector4_to_মান(word_val, vp->value.real, signed_flag);
		ভঙ্গ;
      }
}

শূন্য vpip_vec2_get_মান(ধ্রুৱ vvp_vector2_t&word_val, unsigned width,
			 তার্কিক signed_flag, s_vpi_মান*vp)
{
      ৱিকল্প (vp->format) {
	  স্ৱভাৱ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector2 values.\n", (পূর্ণাঙ্ক)vp->format);
	    প্রতিজ্ঞা(0);

	  পক্ষ vpiSuppressVal:
	    ভঙ্গ;

	  পক্ষ vpiIntVal:
	    vector2_to_মান(word_val, vp->value.integer, signed_flag);
	    ভঙ্গ;

	  পক্ষ vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  পক্ষ vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * পরিমাণ(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		বাবে (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      যদি (word_val.value(idx)) {
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      } অন্যথা {
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      }
		      যদি (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		ভঙ্গ;
	  }
      }

}

/*
 * Convert a real value to the appropriate integer.
 */
স্থির PLI_INT32 get_real_as_int(দ্ৱিগুণ real)
{
      দ্ৱিগুণ rtn;

	/* We would normally want to return 'bx for a NaN or
	 * +/- infinity, but for an integer the standard says
	 * to convert 'bx to 0, so we just return 0. */
      যদি (real != real || (real && (real == 0.5*real))) {
	    ঘূৰাই 0;
      }

      rtn = std::round(real);

      ঘূৰাই (PLI_INT32) rtn;
}

/*
 * This is a generic function to convert a double value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer.
 */
শূন্য vpip_real_get_মান(দ্ৱিগুণ real, s_vpi_মান*vp)
{
      ৱর্ণ *rbuf = 0;

      ৱিকল্প (vp->format) {
	  স্ৱভাৱ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting real values.\n", (পূর্ণাঙ্ক)vp->format);
	    প্রতিজ্ঞা(0);

	  পক্ষ vpiSuppressVal:
	    ভঙ্গ;

	  পক্ষ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiRealVal;
	    // fallthrough
	  পক্ষ vpiRealVal:
	    vp->value.real = real;
	    ভঙ্গ;

	  পক্ষ vpiIntVal:
	    vp->value.integer = get_real_as_int(real);
	    ভঙ্গ;

	  পক্ষ vpiDecStrVal:
	    rbuf = static_cast<ৱর্ণ *>(need_result_buf(1025, RBUF_VAL));
	    vpip_vec4_to_dec_str(vvp_vector4_t(1024, real), rbuf, 1025, সঁচা);
	    vp->value.str = rbuf;
	    ভঙ্গ;
      }
}

দ্ৱিগুণ real_from_vpi_মান(s_vpi_মান*vp)
{
      vvp_vector4_t vec4(1024);
      দ্ৱিগুণ result;
      তার্কিক is_signed = মিছা;

      ৱিকল্প (vp->format) {
	  স্ৱভাৱ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "putting real values.\n", (পূর্ণাঙ্ক)vp->format);
	    প্রতিজ্ঞা(0);

	  পক্ষ vpiRealVal:
	    result = vp->value.real;
	    ভঙ্গ;

	  পক্ষ vpiIntVal:
	    result = (দ্ৱিগুণ) vp->value.integer;
	    ভঙ্গ;

	  পক্ষ vpiBinStrVal:
	    vpip_bin_str_to_vec4(vec4, vp->value.str);
	    যদি (vp->value.str[0] == '-') is_signed = সঁচা;
	    vector4_to_মান(vec4, result, is_signed);
	    ভঙ্গ;

	  পক্ষ vpiOctStrVal:
	    vpip_oct_str_to_vec4(vec4, vp->value.str);
	    যদি (vp->value.str[0] == '-') is_signed = সঁচা;
	    vector4_to_মান(vec4, result, is_signed);
	    ভঙ্গ;

	  পক্ষ vpiDecStrVal:
	    vpip_dec_str_to_vec4(vec4, vp->value.str);
	    যদি (vp->value.str[0] == '-') is_signed = সঁচা;
	    vector4_to_মান(vec4, result, is_signed);
	    ভঙ্গ;

	  পক্ষ vpiHexStrVal:
	    vpip_hex_str_to_vec4(vec4, vp->value.str);
	    যদি (vp->value.str[0] == '-') is_signed = সঁচা;
	    vector4_to_মান(vec4, result, is_signed);
	    ভঙ্গ;

      }

      ঘূৰাই result;
}

শূন্য vpip_string_get_মান(ধ্রুৱ string&val, s_vpi_মান*vp)
{
      ৱর্ণ *rbuf = 0;

      ৱিকল্প (vp->format) {
	  স্ৱভাৱ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting string values.\n", (পূর্ণাঙ্ক)vp->format);
	    প্রতিজ্ঞা(0);

	  পক্ষ vpiSuppressVal:
	    ভঙ্গ;

	  পক্ষ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiStringVal;
	    // fallthrough
	  পক্ষ vpiStringVal:
	    rbuf = static_cast<ৱর্ণ *>(need_result_buf(val.size() + 1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    ভঙ্গ;
      }
}


শূন্য vpi_get_মান(vpiHandle ৰাশিমালা, s_vpi_মান*vp)
{
      প্রতিজ্ঞা(ৰাশিমালা);
      প্রতিজ্ঞা(vp);

	// Never bother with suppressed values. All the derived
	// classes can ignore this type.
      যদি (vp->format == vpiSuppressVal)
	    ঘূৰাই;

      ৰাশিমালা->vpi_get_মান(vp);

      যদি (vpi_trace) ৱিকল্প (vp->format) {
		পক্ষ vpiStringVal:
		  fprintf(vpi_trace,"vpi_get_value(%p=<%d>) -> string=\"%s\"\n",
			  ৰাশিমালা, ৰাশিমালা->get_type_code(), vp->value.str);
		  ভঙ্গ;

		পক্ষ vpiBinStrVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> binstr=%s\n",
			  ৰাশিমালা->get_type_code(), vp->value.str);
		  ভঙ্গ;

		পক্ষ vpiIntVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> int=%d\n",
			  ৰাশিমালা->get_type_code(), (পূর্ণাঙ্ক)vp->value.integer);
		  ভঙ্গ;

		পক্ষ vpiSuppressVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <suppress>\n",
			  ৰাশিমালা->get_type_code());
		  ভঙ্গ;

		স্ৱভাৱ:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <%d>=?\n",
			  ৰাশিমালা->get_type_code(), (পূর্ণাঙ্ক)vp->format);
	    }
}

সংরচনা vpip_put_মান_event : vvp_gen_event_s {
      vpiHandle handle;
      s_vpi_মান value;
      পূর্ণাঙ্ক flags;
      আভাস শূন্য run_run() override;
      ~vpip_put_মান_event() override { }
};

শূন্য vpip_put_মান_event::run_run()
{
      handle->vpi_put_মান(&value, flags);
      ৱিকল্প (value.format) {
	    /* Free the copied string. */
	  পক্ষ vpiBinStrVal:
	  পক্ষ vpiOctStrVal:
	  পক্ষ vpiDecStrVal:
	  পক্ষ vpiHexStrVal:
	  পক্ষ vpiStringVal:
	    মুক্ত(value.value.str);
	    ভঙ্গ;
	    /* Free the copied time structure. */
	  পক্ষ vpiTimeVal:
	    মুক্ত(value.value.time);
	    ভঙ্গ;
	    /* Free the copied vector structure. */
	  পক্ষ vpiVectorVal:
	    মুক্ত(value.value.vector);
	    ভঙ্গ;
	    /* Free the copied strength structure. */
	  পক্ষ vpiStrengthVal:
	    মুক্ত(value.value.strength);
	    ভঙ্গ;
	    /* Everything else is static in the structure. */
	  স্ৱভাৱ:
	    ভঙ্গ;
      }
}

/* Make a copy of a pointer to a time structure. */
স্থির t_vpi_time *timedup(ধ্রুৱ t_vpi_time *val)
{
      t_vpi_time *rtn;
      rtn = static_cast<t_vpi_time *> (malloc(পরিমাণ(t_vpi_time)));
      *rtn = *val;
      ঘূৰাই rtn;
}

/* Make a copy of a pointer to a vector value structure. */
স্থির t_vpi_vecval *vectordup(ধ্রুৱ t_vpi_vecval *val, PLI_INT32 size)
{
      unsigned অঙ্ক_bytes;
      t_vpi_vecval *rtn;
      প্রতিজ্ঞা(size > 0);
      অঙ্ক_bytes = ((size + 31)/32)*পরিমাণ(t_vpi_vecval);
      rtn = static_cast<t_vpi_vecval *> (malloc(অঙ্ক_bytes));
      memcpy(rtn, val, অঙ্ক_bytes);
      ঘূৰাই rtn;
}

/* Make a copy of a pointer to a strength structure. */
স্থির t_vpi_strengthval *strengthdup(ধ্রুৱ t_vpi_strengthval *val)
{
      t_vpi_strengthval *rtn;
      rtn = static_cast<t_vpi_strengthval *>
            (malloc(পরিমাণ(t_vpi_strengthval)));
      *rtn = *val;
      ঘূৰাই rtn;
}

vpiHandle vpi_put_মান(vpiHandle obj, s_vpi_মান*vp,
			s_vpi_time*when, PLI_INT32 flags)
{
      প্রতিজ্ঞা(obj);

      flags &= ~vpiReturnEvent;

      যদি (flags!=vpiNoDelay && flags!=vpiForceFlag && flags!=vpiReleaseFlag) {
	    vvp_time64_t dly;
	    পূর্ণাঙ্ক scale;

	    যদি (vpi_get(vpiAutomatic, obj)) {
		  fprintf(stderr, "VPI error: cannot put a value with "
				  "a delay on automatically allocated "
				  "variable '%s'.\n",
				  vpi_get_str(vpiName, obj));
		  ঘূৰাই 0;
	    }

	    প্রতিজ্ঞা(when != 0);

	    ৱিকল্প (when->type) {
		পক্ষ vpiScaledRealTime:
		  scale = vpip_time_units_from_handle(obj) -
		          vpip_get_time_precision();
		  যদি (scale >= 0) {
			dly = (vvp_time64_t)(when->real * pow(10.0, scale));
		  } অন্যথা {
			dly = (vvp_time64_t)(when->real / pow(10.0, -scale));
		  }
		  ভঙ্গ;
		পক্ষ vpiSimTime:
		  dly = vpip_timestruct_to_time(when);
		  ভঙ্গ;
		স্ৱভাৱ:
		  dly = 0;
		  ভঙ্গ;
	    }

	    যদি ((dly == 0) && schedule_at_rosync()) {
		  fprintf(stderr, "VPI error: attempted to put a value to "
				  "variable '%s' during a read-only synch "
				  "callback.\n", vpi_get_str(vpiName, obj));
		  ঘূৰাই 0;
	    }

	    vpip_put_মান_event*put = নূতন vpip_put_মান_event;
	    put->handle = obj;
	    যদি (dynamic_cast<__vpiNamedEvent*>(obj)) {
		  put->value.format = vpiIntVal;
		  put->value.value.integer = 0;
	    } অন্যথা {
		  প্রতিজ্ঞা(vp);
		  put->value = *vp;
	    }
	      /* Since this is a scheduled put event we must copy any pointer
	       * data to keep it available until the event is actually run. */
	    ৱিকল্প (put->value.format) {
		  /* Copy the string items. */
		পক্ষ vpiBinStrVal:
		পক্ষ vpiOctStrVal:
		পক্ষ vpiDecStrVal:
		পক্ষ vpiHexStrVal:
		পক্ষ vpiStringVal:
		  put->value.value.str = strdup(put->value.value.str);
		  ভঙ্গ;
		  /* Copy a time pointer item. */
		পক্ষ vpiTimeVal:
		  put->value.value.time = timedup(put->value.value.time);
		  ভঙ্গ;
		  /* Copy a vector pointer item. */
		পক্ষ vpiVectorVal:
		  put->value.value.vector = vectordup(put->value.value.vector,
		                                      vpi_get(vpiSize, obj));
		  ভঙ্গ;
		  /* Copy a strength pointer item. */
		পক্ষ vpiStrengthVal:
		  put->value.value.strength =
		      strengthdup(put->value.value.strength);
		  ভঙ্গ;
		  /* Everything thing else is already in the structure. */
		স্ৱভাৱ:
		  ভঙ্গ;
	    }
	    put->flags = flags;
	    schedule_generic(put, dly, মিছা, সঁচা, সঁচা);
	    ঘূৰাই 0;
      }

      যদি (schedule_at_rosync()) {
            fprintf(stderr, "VPI error: attempted to put a value to "
			    "variable '%s' during a read-only synch "
			    "callback.\n", vpi_get_str(vpiName, obj));
            ঘূৰাই 0;
      }

      obj->vpi_put_মান(vp, flags);

      ঘূৰাই 0;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle res = 0;

      যদি (ref == 0) {
	      // A few types can apply to a nil handle. These are ways
	      // that the current function can get started finding things.
	    ৱিকল্প (type) {

		পক্ষ vpiScope:
		    // The IEEE1364-2005 doesn't seem to allow this,
		    // but some users seem to think it's handy, so
		    // return the scope that contains this SysTfCall.
		  প্রতিজ্ঞা(vpip_cur_task);
		  res = vpip_cur_task->vpi_handle(vpiScope);
		  ভঙ্গ;

		পক্ষ vpiSysTfCall:
		    // This is how VPI users get a first handle into
		    // the system. This is the handle of the system
		    // task/function call currently being executed.
		  যদি (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle(vpiSysTfCall, 0) "
				"-> %p (%s)\n", vpip_cur_task,
				vpip_cur_task->defn->info.tfname);
		  }
		  ঘূৰাই vpip_cur_task;

		স্ৱভাৱ:
		  fprintf(stderr, "VPI error: vpi_handle(type=%d, ref=0).\n",
			  (পূর্ণাঙ্ক)type);
		  res = 0;
		  ভঙ্গ;
	    }

      } অন্যথা {

	    যদি (type == vpiSysTfCall) {
		  fprintf(stderr, "VPI error: vpi_handle(vpiSysTfCall, "
			  "ref!=0).\n");
		  ঘূৰাই 0;
	    }

	    res = ref->vpi_handle(type);
      }


      যদি (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle(vpiScope, ref=%p) "
		    "-> %p\n", vpip_cur_task, ref);
      }

      ঘূৰাই res;
}

স্থির vpiHandle vpip_make_udp_iterator()
{
// HERE: Add support for iterating over UDP definitions.
//       See 26.6.16 (page 400 in 1364-2005).
      ঘূৰাই 0;
}

/*
 * This function asks the object to return an iterator for
 * the specified reference. It is up to the iterate_ method to
 * allocate a properly formed iterator.
 */
স্থির vpiHandle vpi_iterate_global(পূর্ণাঙ্ক type)
{
      ৱিকল্প (type) {
	  পক্ষ vpiInstance:
	    // fallthrough
	  পক্ষ vpiModule:
	    // fallthrough
	  পক্ষ vpiProgram:
	    // fallthrough
	  পক্ষ vpiInterface:
	    // fallthrough
	  পক্ষ vpiPackage:
	    ঘূৰাই vpip_make_মূল_iterator(type);

	  পক্ষ vpiUdpDefn:
	    ঘূৰাই vpip_make_udp_iterator();

	  পক্ষ vpiUserSystf:
	    ঘূৰাই vpip_make_systf_iterator();
      }

      ঘূৰাই 0;
}

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle rtn = 0;

      প্রতিজ্ঞা(vpi_mode_flag != VPI_MODE_NONE);
      যদি (vpi_mode_flag == VPI_MODE_REGISTER) {
	    fprintf(stderr, "vpi error: vpi_iterate called during "
		    "vpi_register_systf. You can't do that!\n");
	    ঘূৰাই 0;
      }

      যদি (ref == 0)
	    rtn = vpi_iterate_global(type);
      অন্যথা
	    rtn = ref->vpi_iterate(type);

      যদি (vpi_trace) {
	    fprintf(vpi_trace, "vpi_iterate(%d, %p) ->%s\n",
	    (পূর্ণাঙ্ক)type, ref, rtn ? "" : " (null)");
      }

      ঘূৰাই rtn;
}

vpiHandle vpi_handle_by_index(vpiHandle ref, PLI_INT32 idx)
{
      প্রতিজ্ঞা(ref);
      ঘূৰাই ref->vpi_index(idx);
}

স্থির vpiHandle find_name(ধ্রুৱ ৱর্ণ *name, vpiHandle handle)
{
      vpiHandle rtn = 0;
      __vpiScope*ref = dynamic_cast<__vpiScope*>(handle);

      /* check module names */
      যদি (!strcmp(name, vpi_get_str(vpiName, handle)))
	    rtn = handle;

      /* brute force search for the name in all objects in this scope */
      বাবে (unsigned i = 0 ;  i < ref->intern.size() ;  i += 1) {
	      /* The standard says that since a port does not have a full
	       * name it cannot be found by name. Because of this we need
	       * to skip ports here so the correct handle can be located. */
	    যদি (vpi_get(vpiType, ref->intern[i]) == vpiPort) অনুৱর্ত;
	    ধ্রুৱ ৱর্ণ *nm = vpi_get_str(vpiName, ref->intern[i]);
	    যদি (nm && !strcmp(name, nm)) {
		  rtn = ref->intern[i];
		  ভঙ্গ;
	    } অন্যথা যদি (vpi_get(vpiType, ref->intern[i]) == vpiMemory ||
	               vpi_get(vpiType, ref->intern[i]) == vpiNetArray) {
		  /* We need to iterate on the words */
		  vpiHandle word_i, word_h;
		  word_i = vpi_iterate(vpiMemoryWord, ref->intern[i]);
		  যেতিয়ালৈ (word_i && (word_h = vpi_scan(word_i))) {
			nm = vpi_get_str(vpiName, word_h);
			যদি (nm && !strcmp(name, nm)) {
			      rtn = word_h;
			      vpi_free_object(word_i);
			      ভঙ্গ;
			}
		  }
	    }
	    /* found it yet? */
	    যদি (rtn) ভঙ্গ;
      }

      ঘূৰাই rtn;
}

// Find the end of the escaped identifier or simple identifier
স্থির ৱর্ণ * find_rest(ৱর্ণ *name)
{
      ৱর্ণ *rest;
      যদি (*name == '\\') {
	    rest = strchr(name, ' ');
	    যদি (rest) {
		  *rest++ = 0; // The space is not part of the string
		    // There should be a '.' after the escaped ID if there is
		    // anything more. If it is missing add it to avoid a crash
		  যদি ((*rest != '.') && (*rest != 0)) {
			*--rest = '.';
			fprintf(stderr, "ERROR: Malformed scope string: \"%s\"", name);
		  }
		  যদি (*rest == 0) {
			rest = 0;
		  }
	    }
      } অন্যথা {
	    rest = strchr(name, '.');
      }
      ঘূৰাই rest;
}

স্থির vpiHandle find_scope(ধ্রুৱ ৱর্ণ *name, vpiHandle handle, পূর্ণাঙ্ক depth)
{

      vpiHandle iter = handle==0
	    ? vpi_iterate(vpiModule, রিক্ত)
	    : vpi_iterate(vpiInternalScope, handle);

      vector<ৱর্ণ> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      ৱর্ণ*nm_first = &name_buf[0];
      ৱর্ণ*nm_rest = find_rest(nm_first);
      যদি (*nm_first == '\\') ++nm_first;
      যদি (nm_rest) {
	    *nm_rest++ = 0;
      }

      vpiHandle rtn = 0;
      vpiHandle hand;
      যেতিয়ালৈ (iter && (hand = vpi_scan(iter))) {
	    ধ্রুৱ ৱর্ণ *nm = vpi_get_str(vpiName, hand);

	    যদি (strcmp(nm_first,nm)==0) {
		  যদি (nm_rest)
			rtn=find_scope(nm_rest, hand, depth+1);
		  অন্যথা
			rtn = hand;
	    }

	    /* found it yet ? */
	    যদি (rtn) {
		  vpi_free_object(iter);
		  ভঙ্গ;
	    }
      }

      ঘূৰাই rtn;
}

// Find the end of the first escaped identifier or simple identifier
স্থির ৱর্ণ * find_next(ৱর্ণ *name)
{
      ৱর্ণ *next;
      যদি (*name == '\\') {
	    next = strchr(name, ' ');
	    যদি (next && *++next == 0) next = 0;
      } অন্যথা {
	    next = strchr(name, '.');
      }
      ঘূৰাই next;
}

vpiHandle vpi_handle_by_name(ধ্রুৱ ৱর্ণ *name, vpiHandle scope)
{
      vpiHandle hand;

      যদি (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name(%s, %p) -->\n",
		    name, scope);
      }

	// Chop the name into path and base. For example, if the name
	// is "a.b.c", then nm_path becomes "a.b" and nm_base becomes
	// "c". If the name is "c" then nm_path is nil and nm_base is "c".
      vector<ৱর্ণ> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      ৱর্ণ*nm_path = &name_buf[0];
      ৱর্ণ*nm_base;

	// Check to see if we have an escaped identifier and if so search
	// the long way because a '.' could be in the escaped name.
      যদি (strchr(nm_path, '\\')) {
	    ৱর্ণ *next;
	    nm_base = nm_path;
	    যেতিয়ালৈ ((next = find_next(nm_base))) {
		  nm_base = ++next;
	    }
	    যদি (nm_path == nm_base) {
		  nm_path = 0;
	    } অন্যথা {
		  *(nm_base-1) = 0;
	    }

	// If there is no escaped identifier then just look for the last '.'
      } অন্যথা {
	    nm_base = strrchr(nm_path, '.');
	    যদি (nm_base) {
		  *nm_base++ = 0;
	    } অন্যথা {
		  nm_base = nm_path;
		  nm_path = 0;
	    }
      }

      /* If scope provided, look in corresponding module; otherwise
       * traverse the hierarchy specified in name to find the leaf module
       * and try finding it there.
       */
      যদি (scope) {
	    /* Some implementations support either a module or a scope. */
	    ৱিকল্প (vpi_get(vpiType, scope)) {
		পক্ষ vpiScope:
	          hand = vpi_handle(vpiModule, scope);
	          ভঙ্গ;
		পক্ষ vpiModule:
	          hand = scope;
	          ভঙ্গ;
		স্ৱভাৱ:
		  যদি (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle_by_name: "
				"Scope is not a vpiScope or vpiModule\n");
		  }
	          // Use vpi_chk_error() here when it is implemented.
	          ঘূৰাই 0;
	    }

      } অন্যথা যদি (nm_path) {
	      // The name has a path, and no other scope handle was
	      // passed in. That suggests we are looking for "a.b.c"
	      // in the root scope. So convert "a.b" to a scope and
	      // start there to look for "c".
	    hand = find_scope(nm_path, রিক্ত, 0);
	    nm_path = 0;

      } অন্যথা {
	      // Special case: scope==<nil>, meaning we are looking in
	      // the root, and there is no path to the name, i.e. the
	      // string is "c" instead of "top.c". Try to find "c" as
	      // a scope and return that.
	    hand = find_scope(nm_base, রিক্ত, 0);
      }

      যদি (hand == 0) {
	    যদি (vpi_trace) {
		  fprintf(vpi_trace, "vpi_handle_by_name: "
			  "Scope does not exist. Giving up.\n");
	    }

	    ঘূৰাই 0;
      }

	// If there is a path part, then use it to find the
	// scope. For example, if the full name is a.b.c, then
	// the nm_path string is a.b and we search for that
	// scope. If we find it, then set hand to that scope.
      যদি (nm_path) {
	    vpiHandle tmp = find_scope(nm_path, hand, 0);
	    যেতিয়ালৈ (tmp == 0 && hand != 0) {
		  hand = vpi_handle(vpiScope, hand);
		  tmp = find_scope(nm_path, hand, 0);
	    }
	    hand = tmp;
      }

	// find_name() expects escaped identifiers to be stripped
      যদি (*nm_base == '\\') {
	    // Skip the \ at the beginning
	    ++nm_base;

	    // Drop the space at the end if it exists
	    ৱর্ণ *next;
	    যদি ((next = strchr(nm_base, ' '))) {
		  *next = 0;
	    }
      }

	// Now we have the correct scope, look for the item.
      vpiHandle out = find_name(nm_base, hand);

      যদি (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name: DONE\n");
      }

      ঘূৰাই out;
}

// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected
// x2: Ptr to previous node
// x3: True when net is connected to output of previous node
tuple<তার্কিক, vvp_net_ptr_t, তার্কিক> check_connected_to_concat8_and_part_sa(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent) {

      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      যেতিয়ালৈ (cur.ptr()) {
	    যদি (cur.ptr() == net2 && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
		    // Pass on the return value if found
		  ঘূৰাই {সঁচা, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
	    } অন্যথা যদি (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8_and_part_sa(cur.ptr()->out_, net2, cur);
		  যদি (get<0>(res)) {
			ঘূৰাই res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      ঘূৰাই {মিছা, vvp_net_ptr_t(nullptr, 0), মিছা};
}

// Check if net2 is connected to current_net through a net of vvp_fun_concat8s
// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected with concats and part_sas
// x2: Ptr to previous node
// x3: True when net is connected to output of x2
tuple<তার্কিক, vvp_net_ptr_t, তার্কিক> check_connected_to_concat8(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent)
{
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);
	// For everything connected
      যেতিয়ালৈ (cur.ptr()) {
	      // Check if it's a concat8
	    যদি (cur.ptr() == net2) {
		  ঘূৰাই {সঁচা, vvp_net_ptr_t(nullptr, 0), মিছা};
	    } অন্যথা যদি (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8(cur.ptr()->out_, net2, cur);
		  যদি (get<0>(res)) {
			যদি (!get<1>(res).ptr() && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
			      // `parent` is the last part_sa node in front of our destination
			      // We want to place the intermod node here
			      ঘূৰাই {সঁচা, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
			}
			ঘূৰাই res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      ঘূৰাই {মিছা, vvp_net_ptr_t(nullptr, 0), মিছা};
}

// Used to get intermodpath for two ports
vpiHandle vpi_handle_multi(PLI_INT32 type,
                           vpiHandle ref1,
                           vpiHandle ref2)
{
      যদি (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_multi(%d, %p, %p) -->\n",
	            type, ref1, ref2);
      }

      যদি (type != vpiInterModPath) {
	    fprintf(stderr, "sorry: vpi_handle_multi currently supports"
	            "only vpiInterModPath\n");
	    ঘূৰাই nullptr;
      }

	// Indicates whether port1 refers to a single bit
      তার্কিক port1_has_index = মিছা;
      পূর্ণাঙ্ক port1_bit_index = 0;
      vpiPortBitInfo* port1_bit = dynamic_cast<vpiPortBitInfo*>(ref1);

      যদি (port1_bit) {
	      // Get the bit index
	    port1_has_index = সঁচা;
	    port1_bit_index = vpi_get(vpiBit, port1_bit);
	      // Update the ref1 to point to the base port
	    ref1 = vpi_handle(vpiParent, port1_bit);
      }

	// Indicates whether port2 refers to a single bit
      তার্কিক port2_has_index = মিছা;
      পূর্ণাঙ্ক port2_bit_index = 0;
      vpiPortBitInfo* port2_bit = dynamic_cast<vpiPortBitInfo*>(ref2);

      যদি (port2_bit) {
	      // Get the bit index
	    port2_has_index = সঁচা;
	    port2_bit_index = vpi_get(vpiBit, port2_bit);
	      // Update the ref1 to point to the base port
	    ref2 = vpi_handle(vpiParent, port2_bit);
      }

      vpiPortInfo* port1 = dynamic_cast<vpiPortInfo*>(ref1);

      যদি (!port1) {
	    fprintf(stderr, "sorry: second argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    ঘূৰাই nullptr;
      }

      vpiPortInfo* port2 = dynamic_cast<vpiPortInfo*>(ref2);

      যদি (!port2) {
	    fprintf(stderr, "sorry: third argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    ঘূৰাই nullptr;
      }

	// Get the names of both ports
      std::string port1_name(vpi_get_str(vpiName, ref1));
      std::string port2_name(vpi_get_str(vpiName, ref2));

	// If both ports are vpiOutput, we have to reassign the __vpiSignal from port1
	// to port2 because otherwise the non-delayed version of the signal is dumped
	// even tho the intermodpath is correctly inserted
      __vpiSignal* output_signal = nullptr;

      যদি (port1->get_direction() == vpiOutput && port2->get_direction() == vpiOutput) {
	    vpiHandle scope_port2 = vpi_handle(vpiScope, ref2);
	    প্রতিজ্ঞা(scope_port2);

	      // Iterate over nets in the scope of port2
	    vpiHandle net_i = vpi_iterate(vpiNet, scope_port2) ;
	    vpiHandle net;

	    যেতিয়ালৈ ((net = vpi_scan(net_i)) != রিক্ত) {
		  std::string net_name(vpi_get_str(vpiName, net));

		    // Compare whether the net matches with the port name
		  যদি (net_name == port2_name) {
			output_signal = dynamic_cast<__vpiSignal*>(net);
		  }
	    }
      }

      vvp_net_t* net1 = port1->get_port();
      vvp_net_t* net2 = port2->get_port();

      যদি (net1 == nullptr || net2 == nullptr) {
	    fprintf(stderr, "Error: Could not find net. "
		    "Did you run iverilog with '-ginterconnect'?\n");
	    ঘূৰাই nullptr;
      }

      যদি (net1 == net2) {
	    fprintf(stderr, "Error: Net for both ports is the same. "
		    "Did you pass the same port twice?\n");
	    ঘূৰাই nullptr;
      }

      যদি (!dynamic_cast<vvp_fun_buft*>(net1->fun)) {
	    fprintf(stderr, "Error: functor of net1 must be"
		    "vvp_fun_buft\n");
	    ঘূৰাই nullptr;
      }

      যদি (!dynamic_cast<vvp_fun_buft*>(net2->fun)) {
	    fprintf(stderr, "Error: functor of net2 must be"
		    "vvp_fun_buft\n");
	    ঘূৰাই nullptr;
      }

	// If port1 is actually a port bit, we have to get to the correct vvp_fun_part
	// after which we insert the intermodpath delay
      যদি (port1_has_index) {
	    vvp_net_ptr_t* net1_ptr = &net1->out_;

	      // Search for part selects connected to port1
	    vvp_net_t* current_net = net1_ptr->ptr();

	    যেতিয়ালৈ (current_net) {
		  ধ্রুৱ vvp_fun_part* part = dynamic_cast<vvp_fun_part*>(current_net->fun);

		    // Its a part select!
		  যদি (part) {
			  // Is it the correct part select?
			যদি (part->get_base() == (unsigned)port1_bit_index) {
			      প্রতিজ্ঞা(part->get_wid() == 1);
			      net1 = current_net; // Replace net1 as this is our new start point
			      ভঙ্গ;
			}
		  }

		  current_net = current_net->port[0].ptr(); // BUFT has only one input, index 0
	    }
      }

	// Iterate over all nodes connected to port1
      vvp_net_ptr_t cur = net1->out_;
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      যেতিয়ালৈ (cur.ptr()) {
	      // Either port2 is directly connected to port1
	      // Or in the second case port2 is indirectly connected
	      // to port1 through a net of concat8s
	    তার্কিক is_connected, is_output;
	    vvp_net_ptr_t previous_গ্ৰন্থি;
	    tie(is_connected, previous_গ্ৰন্থি, is_output) = port2_has_index
		  ? check_connected_to_concat8(cur, net2, vvp_net_ptr_t(net1, 0))
		  : check_connected_to_concat8_and_part_sa(cur, net2, vvp_net_ptr_t(net1, 0));
	    যদি ( (!port2_has_index && cur.ptr() == net2) || is_connected ) {
		  vvp_net_t*new_net = নূতন vvp_net_t;

		    // Create new node with intermodpath and connect port2 to it
		  পূর্ণাঙ্ক width = 1; // TODO
		  vvp_fun_intermodpath*obj = নূতন vvp_fun_intermodpath(new_net, width);

		  vvp_net_t* output_net = net1;
		  যদি (is_connected) {
			যদি (is_output) {
			      output_net = previous_গ্ৰন্থি.ptr(); // net2 is direct output of part_sa
			      cur = output_net->out_;
			      prev = vvp_net_ptr_t(nullptr, 0);
			} অন্যথা {
			      prev = previous_গ্ৰন্থি;
			      যদি (prev.ptr()) {
				    cur = prev.ptr()->port[prev.port()];
			      }
			}
		  }

		  new_net->fun = obj;
		  new_net->out_ = cur;

		    // Port2 is in the middle of the list
		    // Insert intermodpath before port2 and keep everything else intact
		  যদি (prev.ptr()) {
			prev.ptr()->port[prev.port()] = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; // Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		    // Port2 is first in list
		    // Insert intermodpath before port2 and keep everything else intact
		  } অন্যথা {
			output_net->out_ = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; //  Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		  }

		    // If both ports are vpiOutput and port2 is not a vector,
		    // we have to reassign the __vpiSignal so that the delayed
		    // values get dumped
		  যদি (output_signal && !port2_has_index) {
			net2->fil = net1->fil;
			net1->fil = nullptr;
			output_signal->গ্ৰন্থি = net2;
		  }

		    // Create the VPI intermodpath object
		  __vpiInterModPath* intermodpath = vpip_make_intermodpath(new_net, port1, port2);
		  intermodpath->intermodpath = obj;

		    // Finally done, return the intermodpath object
		  ঘূৰাই intermodpath;
	    }

	    prev = cur;
	    cur = cur.ptr()->port[cur.port()]; // Next net in linked list
      }

      fprintf(stderr, "VPI error: Could not insert intermodpath!\n");
      fprintf(stderr, "\tport1 = %s, port1_has_index = %d, port1_bit_index = %d\n", port1_name.c_str(), port1_has_index, port1_bit_index);
      fprintf(stderr, "\tport2 = %s, port2_has_index = %d, port2_bit_index = %d\n", port2_name.c_str(), port2_has_index, port2_bit_index);
      ঘূৰাই nullptr;
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



শূন্য vpi_get_delays(vpiHandle ৰাশিমালা, p_vpi_delay delays)
{
      প্রতিজ্ঞা(ৰাশিমালা);
      প্রতিজ্ঞা(delays);

      ৰাশিমালা->vpi_get_delays(delays);

      যদি (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_get_delays(%p, %p) -->\n", ৰাশিমালা, delays);
      }
}


শূন্য vpi_put_delays(vpiHandle ৰাশিমালা, p_vpi_delay delays)
{
      প্রতিজ্ঞা(ৰাশিমালা  );
      প্রতিজ্ঞা(delays );

      ৰাশিমালা->vpi_put_delays(delays);

      যদি (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_put_delays(%p, %p) -->\n", ৰাশিমালা, delays);
      }
}








বাহ্য "C" PLI_INT32 vpi_vprintf(ধ্রুৱ ৱর্ণ*fmt, va_list ap)
{
      ঘূৰাই vpi_mcd_vprintf(1, fmt, ap);
}

বাহ্য "C" PLI_INT32 vpi_printf(ধ্রুৱ ৱর্ণ *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      পূর্ণাঙ্ক r = vpi_mcd_vprintf(1, fmt, ap);
      va_end(ap);
      ঘূৰাই r;
}

বাহ্য "C" PLI_INT32 vpi_flush(শূন্য)
{
      ঘূৰাই vpi_mcd_flush(1);
}


বাহ্য "C" PLI_INT32 vpi_sim_vcontrol(পূর্ণাঙ্ক operation, va_list ap)
{
      long diag_বাৰ্তা;

      ৱিকল্প (operation) {
	  পক্ষ vpiFinish:
	  পক্ষ __ivl_legacy_vpiFinish:
            diag_বাৰ্তা = va_arg(ap, PLI_INT32);
	    schedule_finish(diag_বাৰ্তা);
	    ভঙ্গ;

	  পক্ষ vpiStop:
	  পক্ষ __ivl_legacy_vpiStop:
            diag_বাৰ্তা = va_arg(ap, PLI_INT32);
	    schedule_stop(diag_বাৰ্তা);
	    ভঙ্গ;

	  স্ৱভাৱ:
	    fprintf(stderr, "Unsupported VPI control operation %d.\n", operation);
	    ঘূৰাই 0;
      }
      ঘূৰাই 1;
}

বাহ্য "C" PLI_INT32 vpi_sim_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      পূর্ণাঙ্ক r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      ঘূৰাই r;
}

বাহ্য "C" PLI_INT32 vpi_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      পূর্ণাঙ্ক r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      ঘূৰাই r;
}

/*
 * This routine calculated the return value for $clog2.
 * It is easier to do it here vs trying to to use the VPI interface.
 */
বাহ্য "C" s_vpi_vecval vpip_calc_clog2(vpiHandle arg)
{
      s_vpi_vecval rtn;
      s_vpi_মান val;
      vvp_vector4_t vec4;
      তার্কিক is_neg = মিছা;  // At this point only a real can be negative.

	/* Get the value as a vvp_vector4_t. */
      val.format = vpiObjTypeVal;
      vpi_get_মান(arg, &val);
      যদি (val.format == vpiRealVal) {
	    vpi_get_মান(arg, &val);
	      /* All double values can be represented in 1024 bits. */
	    vec4 = vvp_vector4_t(1024, val.value.real);
	    যদি (val.value.real < 0) is_neg = সঁচা;
      } অন্যথা {
	    val.format = vpiVectorVal;
	    vpi_get_মান(arg, &val);
	    unsigned wid = vpi_get(vpiSize, arg);
	    vec4 = vvp_vector4_t(wid, BIT4_0);
	    বাবে (unsigned idx=0; idx < wid; idx += 1) {
		PLI_INT32 aval = val.value.vector[idx/32].aval;
		PLI_INT32 bval = val.value.vector[idx/32].bval;
		aval >>= idx % 32;
		bval >>= idx % 32;
		পূর্ণাঙ্ক bitmask = (aval&1) | ((bval<<1)&2);
		vvp_bit4_t bit = scalar_to_bit4(bitmask);
		vec4.set_bit(idx, bit);
	    }
      }

      যদি (vec4.has_xz()) {
	    rtn.aval = rtn.bval = 0xFFFFFFFFU;  /* Set to 'bx. */
	    ঘূৰাই rtn;
      }

      vvp_vector2_t vec2(vec4);

      যদি (is_neg) vec2.trim_neg(); /* This is a special trim! */
      অন্যথা vec2.trim(); /* This makes less work shifting. */

	/* Calculate the clog2 result. */
      PLI_INT32 res = 0;
      যদি (!vec2.is_zero()) {
	    vec2 -= vvp_vector2_t(1, vec2.size());
	    যেতিয়ালৈ(!vec2.is_zero()) {
		  res += 1;
		  vec2 >>= 1;
	    }
      }

      rtn.aval = res;
      rtn.bval = 0;
      ঘূৰাই rtn;
}

/*
 * This routine provides the information needed to implement $countdrivers.
 * It is done here for performance reasons - interrogating the drivers
 * individually via the VPI interface would be much slower.
 */
বাহ্য "C" শূন্য vpip_গুনতি_drivers(vpiHandle ref, unsigned idx,
                                   unsigned counts[4])
{
      সংরচনা __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      প্রতিজ্ঞা(rfp);
      rfp->গ্ৰন্থি->গুনতি_drivers(idx, counts);
}

#যদি defined(__MINGW32__) || defined (__CYGWIN__)
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
    .get_মান                  = vpi_get_মান,
    .put_মান                  = vpi_put_মান,
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
    .গুনতি_drivers              = vpip_গুনতি_drivers,
    .format_strength            = vpip_format_strength,
    .format_pretty              = vpip_format_pretty,
    .make_systf_system_defined  = vpip_make_systf_system_defined,
    .mcd_rawwrite               = vpip_mcd_rawwrite,
    .set_return_মান           = vpip_set_return_মান,
};
#endif
