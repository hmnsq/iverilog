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

using ನಾಮಾಕಾಶ std;
vpi_mode_t vpi_mode_flag = VPI_MODE_NONE;
FILE*vpi_trace = 0;

ಸ್ಥಿರ s_vpi_vlog_info  vpi_vlog_info;
ಸ್ಥಿರ s_vpi_error_info vpip_last_error = { 0, 0, 0, 0, 0, 0, 0 };

ಧ್ರುವ ವರ್ಣ* direction_as_string(ಪೂರ್ಣಾಙ್ಕ dir)
{
      ವಿಕಲ್ಪ (dir) {
	  ಪಕ್ಷ vpi_ಒಳಹರಿವು:
	    ಪ್ರತ್ಯಾಗಮ "input";
	  ಪಕ್ಷ vpiOutput:
	    ಪ್ರತ್ಯಾಗಮ "output";
	  ಪಕ್ಷ vpiInout:
	    ಪ್ರತ್ಯಾಗಮ "inout";
	  ಪಕ್ಷ vpiMixedIO:
	    ಪ್ರತ್ಯಾಗಮ "mixed io";
	  ಪಕ್ಷ vpiNoDirection:
	    ಪ್ರತ್ಯಾಗಮ "no direction";
	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ "INVALID-DIRECTION";
      }
}

__vpiHandle::~__vpiHandle()
{ }

ಪೂರ್ಣಾಙ್ಕ __vpiHandle::vpi_get(ಪೂರ್ಣಾಙ್ಕ)
{ ಪ್ರತ್ಯಾಗಮ vpiUndefined; }

int64_t __vpiHandle::vpi_get64(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ (code == _vpiNexusId) ? 0 : vpiUndefined; }

ವರ್ಣ* __vpiHandle::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ)
{ ಪ್ರತ್ಯಾಗಮ 0; }

ಶೂನ್ಯ __vpiHandle::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ)
{ }

vpiHandle __vpiHandle::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ)
{ ಪ್ರತ್ಯಾಗಮ 0; }

vpiHandle __vpiHandle::vpi_handle(ಪೂರ್ಣಾಙ್ಕ)
{ ಪ್ರತ್ಯಾಗಮ 0; }

vpiHandle __vpiHandle::vpi_iterate(ಪೂರ್ಣಾಙ್ಕ)
{ ಪ್ರತ್ಯಾಗಮ 0; }

vpiHandle __vpiHandle::vpi_index(ಪೂರ್ಣಾಙ್ಕ)
{ ಪ್ರತ್ಯಾಗಮ 0; }

ಶೂನ್ಯ __vpiHandle::vpi_get_delays(p_vpi_delay)
{ }

ಶೂನ್ಯ __vpiHandle::vpi_put_delays(p_vpi_delay)
{ }

__vpiBaseVar::__vpiBaseVar(__vpiScope*scope, ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net)
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ suppress_free(vpiHandle)
{ ಪ್ರತ್ಯಾಗಮ 1; }
__vpiHandle::free_object_fun_t __vpiHandle::free_object_fun(ಶೂನ್ಯ)
{ ಪ್ರತ್ಯಾಗಮ &suppress_free; }

/*
 * The vpip_string function creates a constant string from the pass
 * input. This constant string is permanently allocated from an
 * efficient string buffer store.
 */
ಸಂರಚನಾ vpip_string_chunk {
      ಸಂರಚನಾ vpip_string_chunk*next;
      ವರ್ಣ data[64*1024 - ಪರಿಮಾಣ (ಸಂರಚನಾ vpip_string_chunk*)];
};

unsigned vpip_size(ಧ್ರುವ __vpiSignal *sig)
{
      ಪ್ರತ್ಯಾಗಮ abs(sig->msb.get_ಮೌಲ್ಯ() - sig->lsb.get_ಮೌಲ್ಯ()) + 1;
}

__vpiScope* vpip_scope(__vpiSignal*sig)
{
      ಯದಿ (sig->is_netarray)
	    ಪ್ರತ್ಯಾಗಮ static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ sig->within.scope;
}

__vpiScope* vpip_scope(__vpiRealVar*sig)
{
      ಯದಿ (sig->is_netarray)
	    ಪ್ರತ್ಯಾಗಮ static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ sig->within.scope;
}

vpiHandle vpip_module(__vpiScope*scope)
{
      ಯಾವತ್(scope && scope->get_type_code() != vpiModule) {
	    scope = scope->scope;
      }
      ಪ್ರತಿಜ್ಞಾ(scope);
      ಪ್ರತ್ಯಾಗಮ scope;
}

ಧ್ರುವ ವರ್ಣ *vpip_string(ಧ್ರುವ ವರ್ಣ*str)
{
      ಸ್ಥಿರ vpip_string_chunk first_chunk = {0, {0}};
      ಸ್ಥಿರ vpip_string_chunk*chunk_list = &first_chunk;
      ಸ್ಥಿರ unsigned chunk_fill = 0;

      unsigned len = strlen(str);
      ಪ್ರತಿಜ್ಞಾ( (len+1) <= ಪರಿಮಾಣ chunk_list->data );

      ಯದಿ ( (len+1) > (ಪರಿಮಾಣ chunk_list->data - chunk_fill) ) {
	    vpip_string_chunk*tmp = ನೂತನ vpip_string_chunk;
	    tmp->next = chunk_list;
	    chunk_list = tmp;
	    chunk_fill = 0;
      }

      ವರ್ಣ*res = chunk_list->data + chunk_fill;
      chunk_fill += len + 1;

      strcpy(res, str);
      ಪ್ರತ್ಯಾಗಮ res;
}

ಸ್ಥಿರ unsigned hash_string(ಧ್ರುವ ವರ್ಣ*text)
{
      unsigned h = 0;

      ಯಾವತ್ (*text) {
	    h = (h << 4) ^ (h >> 28) ^ (unsigned)*text;
	    text += 1;
      }
      ಪ್ರತ್ಯಾಗಮ h;
}

ಧ್ರುವ ವರ್ಣ* vpip_name_string(ಧ್ರುವ ವರ್ಣ*text)
{
      ಧ್ರುವ unsigned HASH_SIZE = 4096;
      ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ*hash_table[HASH_SIZE] = {0};

      unsigned hash_ಮೌಲ್ಯ = hash_string(text) % HASH_SIZE;

	/* If we easily find the string in the hash table, then return
	   that and be done. */
      ಯದಿ (hash_table[hash_ಮೌಲ್ಯ]
	  && (strcmp(hash_table[hash_ಮೌಲ್ಯ], text) == 0)) {
	    ಪ್ರತ್ಯಾಗಮ hash_table[hash_ಮೌಲ್ಯ];
      }

	/* The existing hash entry is not a match. Replace it with the
	   newly allocated value, and return the new pointer as the
	   result to the add. */
      ಧ್ರುವ ವರ್ಣ*res = vpip_string(text);
      hash_table[hash_ಮೌಲ್ಯ] = res;

      ಪ್ರತ್ಯಾಗಮ res;
}

PLI_INT32 vpi_chk_error(p_vpi_error_info info)
{
      ಯದಿ (vpip_last_error.state == 0)
	    ಪ್ರತ್ಯಾಗಮ 0;

      info->state = vpip_last_error.state;
      info->level = vpip_last_error.level;
      info->message = vpip_last_error.message;
      info->product = vpi_vlog_info.product;
      info->code = (ವರ್ಣ *)"";
      info->file = 0;
      info->ಸಾಲು = 0;

      ಪ್ರತ್ಯಾಗಮ info->level;
}

PLI_INT32 vpi_compare_objects(vpiHandle obj1, vpiHandle obj2)
{
      ಪ್ರತಿಜ್ಞಾ(obj1);
      ಪ್ರತಿಜ್ಞಾ(obj2);

	// Does this actually work for all cases?
      ಯದಿ (obj1 != obj2) ಪ್ರತ್ಯಾಗಮ 0;
      ಅನ್ಯಥಾ ಪ್ರತ್ಯಾಗಮ 1;
}

/*
 * Copy the internal information to the data structure. Do not free or
 * change the tfname/user_data since they are a pointer to the real
 * string/data values. We also support passing a task or function handle
 * instead of just a handle to a vpiUserSystf.
 */
ಶೂನ್ಯ vpi_get_systf_info(vpiHandle ref, p_vpi_systf_data data)
{
      ಸಂರಚನಾ __vpiUserSystf* rfp = dynamic_cast<__vpiUserSystf*>(ref);
      ಯದಿ (rfp == 0) {
	    ಸಂರಚನಾ __vpiSysTaskCall*call = dynamic_cast<__vpiSysTaskCall*>(ref);
	    ಪ್ರತಿಜ್ಞಾ(call);
	    rfp = call->defn;
      }

	/* Assert that vpiUserDefn is true! */
      ಪ್ರತಿಜ್ಞಾ(rfp->is_user_defn);

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
ಸಂರಚನಾ __vpiSysTaskCall*vpip_cur_task = 0;

PLI_INT32 vpi_free_object(vpiHandle ref)
{
      ಪೂರ್ಣಾಙ್ಕ rtn;

      ಯದಿ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_free_object(%p)", ref);
	    fflush(vpi_trace);
      }

      ಪ್ರತಿಜ್ಞಾ(ref);
      __vpiHandle::free_object_fun_t fun = ref->free_object_fun();
      rtn = fun (ref);

      ಯದಿ (vpi_trace)
	    fprintf(vpi_trace, " --> %d\n", rtn);

      ಪ್ರತ್ಯಾಗಮ rtn;
}

PLI_INT32 vpi_release_handle(vpiHandle ref)
{
	// Since SystemVerilog vpi_free_object() has been
	// renamed vpi_release_handle(), and thus
	// vpi_free_object() has been deprecated.
      ಪ್ರತ್ಯಾಗಮ vpi_free_object(ref);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vpip_get_global(ಪೂರ್ಣಾಙ್ಕ property)
{
      ವಿಕಲ್ಪ (property) {

	  ಪಕ್ಷ vpiTimeUnit:
	  ಪಕ್ಷ vpiTimePrecision:
	    ಪ್ರತ್ಯಾಗಮ vpip_get_time_precision();

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vpi error: bad global property: %d\n", property);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ vpiUndefined;
      }
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* vpi_property_str(PLI_INT32 code)
{
      ಸ್ಥಿರ ವರ್ಣ buf[32];
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiConstType:
	    ಪ್ರತ್ಯಾಗಮ "vpiConstType";
	  ಪಕ್ಷ vpiName:
	    ಪ್ರತ್ಯಾಗಮ "vpiName";
	  ಪಕ್ಷ vpiFullName:
	    ಪ್ರತ್ಯಾಗಮ "vpiFullName";
	  ಪಕ್ಷ vpiTimeUnit:
	    ಪ್ರತ್ಯಾಗಮ "vpiTimeUnit";
	  ಪಕ್ಷ vpiTimePrecision:
	    ಪ್ರತ್ಯಾಗಮ "vpiTimePrecision";
          ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ "vpiSize";
          ಪಕ್ಷ _vpiNexusId:
	    ಪ್ರತ್ಯಾಗಮ "_vpiNexusId";
	  ಸ್ವಭಾವ:
	    snprintf(buf, ಪರಿಮಾಣ(buf), "%d", (ಪೂರ್ಣಾಙ್ಕ)code);
      }
      ಪ್ರತ್ಯಾಗಮ buf;
}

ಧ್ರುವ ವರ್ಣ* vpi_type_as_string(PLI_INT32 code)
{
      ಸ್ಥಿರ ವರ್ಣ buf[32];
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiArrayType:
	    ಪ್ರತ್ಯಾಗಮ "vpiArrayType";
	  ಪಕ್ಷ vpiBitVar:
	    ಪ್ರತ್ಯಾಗಮ "vpiBitVar";
	  ಪಕ್ಷ vpiByteVar:
	    ಪ್ರತ್ಯಾಗಮ "vpiByteVar";
	  ಪಕ್ಷ vpiClassVar:
	    ಪ್ರತ್ಯಾಗಮ "vpiClassVar";
	  ಪಕ್ಷ vpiConstant:
	    ಪ್ರತ್ಯಾಗಮ "vpiConstant";
	  ಪಕ್ಷ vpiEnumTypespec:
	    ಪ್ರತ್ಯಾಗಮ "vpiEnumTypespec";
	  ಪಕ್ಷ vpiFunction:
	    ಪ್ರತ್ಯಾಗಮ "vpiFunction";
	  ಪಕ್ಷ vpiGenScope:
	    ಪ್ರತ್ಯಾಗಮ "vpiGenScope";
	  ಪಕ್ಷ vpiIntVar:
	    ಪ್ರತ್ಯಾಗಮ "vpiIntVar";
	  ಪಕ್ಷ vpiIntegerVar:
	    ಪ್ರತ್ಯಾಗಮ "vpiIntegerVar";
	  ಪಕ್ಷ vpiIterator:
	    ಪ್ರತ್ಯಾಗಮ "vpiIterator";
	  ಪಕ್ಷ vpiLongIntVar:
	    ಪ್ರತ್ಯಾಗಮ "vpiLongIntVar";
	  ಪಕ್ಷ vpiMemory:
	    ಪ್ರತ್ಯಾಗಮ "vpiMemory";
	  ಪಕ್ಷ vpiMemoryWord:
	    ಪ್ರತ್ಯಾಗಮ "vpiMemoryWord";
	  ಪಕ್ಷ vpiModule:
	    ಪ್ರತ್ಯಾಗಮ "vpiModule";
	  ಪಕ್ಷ vpiNamedBegin:
	    ಪ್ರತ್ಯಾಗಮ "vpiNamedBegin";
	  ಪಕ್ಷ vpiNamedEvent:
	    ಪ್ರತ್ಯಾಗಮ "vpiNamedEvent";
	  ಪಕ್ಷ vpiNamedFork:
	    ಪ್ರತ್ಯಾಗಮ "vpiNamedFork";
	  ಪಕ್ಷ vpiPackage:
	    ಪ್ರತ್ಯಾಗಮ "vpiPackage";
	  ಪಕ್ಷ vpiPathTerm:
	    ಪ್ರತ್ಯಾಗಮ "vpiPathTerm";
	  ಪಕ್ಷ vpiPort:
	    ಪ್ರತ್ಯಾಗಮ "vpiPort";
	  ಪಕ್ಷ vpiNet:
	    ಪ್ರತ್ಯಾಗಮ "vpiNet";
	  ಪಕ್ಷ vpiNetArray:
	    ಪ್ರತ್ಯಾಗಮ "vpiNetArray";
	  ಪಕ್ಷ vpiNetBit:
	    ಪ್ರತ್ಯಾಗಮ "vpiNetBit";
	  ಪಕ್ಷ vpiParameter:
	    ಪ್ರತ್ಯಾಗಮ "vpiParameter";
	  ಪಕ್ಷ vpiPartSelect:
	    ಪ್ರತ್ಯಾಗಮ "vpiPartSelect";
	  ಪಕ್ಷ vpiRealVar:
	    ಪ್ರತ್ಯಾಗಮ "vpiRealVar";
	  ಪಕ್ಷ vpiReg:
	    ಪ್ರತ್ಯಾಗಮ "vpiReg";
	  ಪಕ್ಷ vpiRegBit:
	    ಪ್ರತ್ಯಾಗಮ "vpiRegBit";
	  ಪಕ್ಷ vpiShortIntVar:
	    ಪ್ರತ್ಯಾಗಮ "vpiShortIntVar";
	  ಪಕ್ಷ vpiStringVar:
	    ಪ್ರತ್ಯಾಗಮ "vpiStringVar";
	  ಪಕ್ಷ vpiSysFuncCall:
	    ಪ್ರತ್ಯಾಗಮ "vpiSysFuncCall";
	  ಪಕ್ಷ vpiSysTaskCall:
	    ಪ್ರತ್ಯಾಗಮ "vpiSysTaskCall";
	  ಪಕ್ಷ vpiTask:
	    ಪ್ರತ್ಯಾಗಮ "vpiTask";
	  ಪಕ್ಷ vpiTimeVar:
	    ಪ್ರತ್ಯಾಗಮ "vpiTimeVar";
	  ಪಕ್ಷ vpiUserSystf:
	    ಪ್ರತ್ಯಾಗಮ "vpiUserSystf";
	  ಸ್ವಭಾವ:
	    snprintf(buf, ಪರಿಮಾಣ(buf), "%d", (ಪೂರ್ಣಾಙ್ಕ)code);
      }
      ಪ್ರತ್ಯಾಗಮ buf;
}

PLI_INT32 vpi_get(ಪೂರ್ಣಾಙ್ಕ property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      ಯದಿ (property == _vpiDelaySelection) ಪ್ರತ್ಯಾಗಮ vpip_delay_selection;

      ಯದಿ (ref == 0)
	    ಪ್ರತ್ಯಾಗಮ vpip_get_global(property);

      ಯದಿ (property == vpiType) {
	    ಯದಿ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

	    ಯದಿ (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  ಪ್ರತ್ಯಾಗಮ vpiNetArray;
	    ಅನ್ಯಥಾ
		  ಪ್ರತ್ಯಾಗಮ ref->get_type_code();
      }

      ಪೂರ್ಣಾಙ್ಕ res = ref->vpi_get(property);

      ಯದಿ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get(%s, %p) --> %d\n",
		    vpi_property_str(property), ref, res);
      }

      ಪ್ರತ್ಯಾಗಮ res;
}

PLI_INT64 vpi_get64(ಪೂರ್ಣಾಙ್ಕ property, vpiHandle ref)
{
	// For now we only support getting the nexus id.
      ಯದಿ ((ref == 0) || (property != _vpiNexusId)) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) is not currently supported.\n",
		    vpi_property_str(property), ref);
	    ಪ್ರತ್ಯಾಗಮ vpiUndefined;
      }

      int64_t res = ref->vpi_get64(property);

      ಯದಿ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) --> %" PRId64 "\n",
		    vpi_property_str(property), ref, res);
      }

      ಪ್ರತ್ಯಾಗಮ res;
}

ವರ್ಣ* vpi_get_str(PLI_INT32 property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      ಯದಿ (property == _vpiDelaySelection) {
	    ವಿಕಲ್ಪ (vpip_delay_selection) {
		ಪಕ್ಷ _vpiDelaySelMinimum:
		  ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str("MINIMUM");
		ಪಕ್ಷ _vpiDelaySelTypical:
		  ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str("TYPICAL");
		ಪಕ್ಷ _vpiDelaySelMaximum:
		  ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str("MAXIMUM");
		ಸ್ವಭಾವ:
		  ಪ್ರತಿಜ್ಞಾ(0);
	    }
      }

      ಯದಿ (ref == 0) {
	    fprintf(stderr, "vpi error: vpi_get_str(%s, 0) called "
		    "with null vpiHandle.\n", vpi_property_str(property));
	    ಪ್ರತ್ಯಾಗಮ 0;
      }

      ಯದಿ (property == vpiType) {
	    ಯದಿ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

            PLI_INT32 type;
	    ಯದಿ (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  type = vpiNetArray;
	    ಅನ್ಯಥಾ
		  type = ref->get_type_code();
	    ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ *)vpi_type_as_string(type);
      }

      ವರ್ಣ*res = ref->vpi_get_str(property);

      ಯದಿ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get_str(%s, %p) --> %s\n",
		    vpi_property_str(property), ref, res? res : "<NULL>");
      }

      ಪ್ರತ್ಯಾಗಮ res;
}

__vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj)
{
      ಧ್ರುವ ಸಂರಚನಾ __vpiSysTaskCall*task;
      ಸಂರಚನಾ __vpiSignal*ಸಙ್ಕೇತ;
      ಸಂರಚನಾ __vpiRealVar*real;
      ಧ್ರುವ __vpiNamedEvent*event;

      ವಿಕಲ್ಪ (obj->get_type_code()) {
	  ಪಕ್ಷ vpiSysTaskCall:
	    task = dynamic_cast<__vpiSysTaskCall*>(obj);
	    ಪ್ರತ್ಯಾಗಮ task->scope;

	  ಪಕ್ಷ vpiModule:
	    ಪ್ರತ್ಯಾಗಮ dynamic_cast<__vpiScope*>(obj);

	  ಪಕ್ಷ vpiNet:
	  ಪಕ್ಷ vpiReg:
	  ಪಕ್ಷ vpiIntegerVar:
	  ಪಕ್ಷ vpiBitVar:
	  ಪಕ್ಷ vpiByteVar:
	  ಪಕ್ಷ vpiShortIntVar:
	  ಪಕ್ಷ vpiIntVar:
	  ಪಕ್ಷ vpiLongIntVar:
	    ಸಙ್ಕೇತ = dynamic_cast<__vpiSignal*>(obj);
	    ಪ್ರತ್ಯಾಗಮ vpip_scope(ಸಙ್ಕೇತ);

	  ಪಕ್ಷ vpiRealVar:
	    real = dynamic_cast<__vpiRealVar*>(obj);
	    ಪ್ರತ್ಯಾಗಮ vpip_scope(real);

	  ಪಕ್ಷ vpiNamedEvent:
	    event = dynamic_cast<__vpiNamedEvent*>(obj);
	    ಪ್ರತ್ಯಾಗಮ event->get_scope();

	  ಪಕ್ಷ vpiMemory:
	  ಪಕ್ಷ vpiMemoryWord:
	  ಪಕ್ಷ vpiPartSelect:
	    ಪ್ರತ್ಯಾಗಮ dynamic_cast<__vpiScope*>(obj->vpi_handle(vpiScope));

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "ERROR: vpip_scope_from_handle called with "
		    "object handle type=%d\n", obj->get_type_code());
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ಪೂರ್ಣಾಙ್ಕ vpip_time_units_from_handle(vpiHandle obj)
{
      ಯದಿ (obj == 0)
	    ಪ್ರತ್ಯಾಗಮ vpip_get_time_precision();

      ಧ್ರುವ __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      ಯದಿ (scope == 0)
	    ಪ್ರತ್ಯಾಗಮ vpip_get_time_precision();

      ಪ್ರತ್ಯಾಗಮ scope->time_units;
}

ಪೂರ್ಣಾಙ್ಕ vpip_time_precision_from_handle(vpiHandle obj)
{
      ಯದಿ (obj == 0)
	    ಪ್ರತ್ಯಾಗಮ vpip_get_time_precision();

      ಧ್ರುವ __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      ಯದಿ (scope == 0)
	    ಪ್ರತ್ಯಾಗಮ vpip_get_time_precision();

      ಪ್ರತ್ಯಾಗಮ scope->time_precision;
}

ದ್ವಿಗುಣ vpip_scaled_time_from_handle(vvp_time64_t time, vpiHandle obj)
{
      ಪೂರ್ಣಾಙ್ಕ scale = vpip_get_time_precision() -
	          vpip_time_units_from_handle(obj);
      ಯದಿ (scale >= 0)
	    ಪ್ರತ್ಯಾಗಮ (ದ್ವಿಗುಣ)time * pow(10.0, scale);
      ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ (ದ್ವಿಗುಣ)time / pow(10.0, -scale);
}

ಶೂನ್ಯ vpi_get_time(vpiHandle obj, s_vpi_time*vp)
{
      vvp_time64_t time;

      ಪ್ರತಿಜ್ಞಾ(vp);

      time = schedule_simtime();

      ವಿಕಲ್ಪ (vp->type) {
          ಪಕ್ಷ vpiSimTime:
	    vp->ಮೇಲಿನ = (time >> 32) & 0xffffffff;
	    vp->ಕೆಳಗಿನ = time & 0xffffffff;
	    ಭಙ್ಗ;

          ಪಕ್ಷ vpiScaledRealTime:
	    vp->real = vpip_scaled_time_from_handle(time, obj);
	    ಭಙ್ಗ;

          ಸ್ವಭಾವ:
            fprintf(stderr, "vpi_get_time: unknown type: %d\n", (ಪೂರ್ಣಾಙ್ಕ)vp->type);
            ಪ್ರತಿಜ್ಞಾ(0);
	    ಭಙ್ಗ;
      }
}

PLI_INT32 vpi_get_vlog_info(p_vpi_vlog_info vlog_info_p)
{
    ಯದಿ (vlog_info_p != 0) {
	  *vlog_info_p = vpi_vlog_info;
	  ಪ್ರತ್ಯಾಗಮ 1;
    } ಅನ್ಯಥಾ {
	  ಪ್ರತ್ಯಾಗಮ 0;
    }
}

ಶೂನ್ಯ vpi_set_vlog_info(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ** argv)
{
    ಸ್ಥಿರ ವರ್ಣ icarus_product[] = "Icarus Verilog";
    ಸ್ಥಿರ ವರ್ಣ icarus_version[] = VERSION;
    vpi_vlog_info.product = icarus_product;
    vpi_vlog_info.version = icarus_version;
    vpi_vlog_info.argc    = argc;
    vpi_vlog_info.argv    = argv;

    ಸ್ಥಿರ ವರ್ಣ trace_buf[1024];
    ಯದಿ (ಧ್ರುವ ವರ್ಣ*path = getenv("VPI_TRACE"))  {
	  ಯದಿ (!strcmp(path,"-"))
		vpi_trace = stdout;
	  ಅನ್ಯಥಾ {
		vpi_trace = fopen(path, "w");
		ಯದಿ (!vpi_trace) {
		      perror(path);
		      exit(1);
		}
		setvbuf(vpi_trace, trace_buf, _IOLBF, ಪರಿಮಾಣ(trace_buf));
	  }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ vec4_get_ಮೌಲ್ಯ_string(ಧ್ರುವ vvp_vector4_t&word_val, unsigned width,
				  s_vpi_ಮೌಲ್ಯ*vp)
{
      unsigned nchar = width / 8;
      unsigned tail = width % 8;

      ವರ್ಣ*rbuf = static_cast<ವರ್ಣ *>(need_result_buf(nchar + 1, RBUF_VAL));
      ವರ್ಣ*cp = rbuf;

      ಯದಿ (tail > 0) {
	    ವರ್ಣ char_val = 0;
	    ಪ್ರತಿ (unsigned idx = width-tail; idx < width ;  idx += 1) {
		  vvp_bit4_t val = word_val.value(idx);
		  ಯದಿ (val == BIT4_1)
			char_val |= 1 << idx;
	    }

	    ಯದಿ (char_val != 0)
		  *cp++ = char_val;
	    ಅನ್ಯಥಾ ಯದಿ (cp != rbuf)
		  *cp++ = ' ';
      }

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < nchar ;  idx += 1) {
	    unsigned bit = (nchar - idx - 1) * 8;
	    ವರ್ಣ char_val = 0;
	    ಪ್ರತಿ (unsigned bdx = 0 ;  bdx < 8 ;  bdx += 1) {
		  vvp_bit4_t val = word_val.value(bit+bdx);
		  ಯದಿ (val == BIT4_1)
			char_val |= 1 << bdx;
	    }
	    // Ignore leading null-bytes and replace other null-bytes with space.
	    // The LRM is not entirely clear on how null bytes should be handled.
	    // This is the implementation chosen for iverilog.
	    ಯದಿ (char_val != 0)
		  *cp++ = char_val;
	    ಅನ್ಯಥಾ ಯದಿ (cp != rbuf)
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
ಶೂನ್ಯ vpip_vec4_get_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector4_t&word_val, unsigned width,
			 ತಾರ್ಕಿಕ signed_flag, s_vpi_ಮೌಲ್ಯ*vp)
{
      ವರ್ಣ *rbuf = 0;

      ವಿಕಲ್ಪ (vp->format) {
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector values.\n", (ಪೂರ್ಣಾಙ್ಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);

	  ಪಕ್ಷ vpiSuppressVal:
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiBinStrVal:
	    rbuf = static_cast<ವರ್ಣ *>(need_result_buf(width+1, RBUF_VAL));
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		  vvp_bit4_t bit = word_val.value(idx);
		  rbuf[width-idx-1] = vvp_bit4_to_ascii(bit);
	    }
	    rbuf[width] = 0;
	    vp->value.str = rbuf;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiOctStrVal: {
		unsigned hwid = ((width+2) / 3) + 1;
		rbuf = static_cast<ವರ್ಣ *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_oct_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		ಭಙ್ಗ;
	  }

	  ಪಕ್ಷ vpiDecStrVal: {
// HERE need a better estimate.
		rbuf = static_cast<ವರ್ಣ *>(need_result_buf(width+1, RBUF_VAL));
		vpip_vec4_to_dec_str(word_val, rbuf, width+1, signed_flag);
		vp->value.str = rbuf;
		ಭಙ್ಗ;
	  }

	  ಪಕ್ಷ vpiHexStrVal: {
		unsigned  hwid = ((width + 3) / 4) + 1;
		rbuf = static_cast<ವರ್ಣ *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_hex_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		ಭಙ್ಗ;
	  }

          ಪಕ್ಷ vpiScalarVal: {
	        // scalars should be of size 1
	        ಪ್ರತಿಜ್ಞಾ(width == 1);
	        ವಿಕಲ್ಪ(word_val.value(0)) {
		    ಪಕ್ಷ BIT4_0:
		      vp->value.scalar = vpi0;
		      ಭಙ್ಗ;
	            ಪಕ್ಷ BIT4_1:
                      vp->value.scalar = vpi1;
                      ಭಙ್ಗ;
	            ಪಕ್ಷ BIT4_X:
                      vp->value.scalar = vpiX;
                      ಭಙ್ಗ;
	            ಪಕ್ಷ BIT4_Z:
                      vp->value.scalar = vpiZ;
                      ಭಙ್ಗ;
		}
                ಭಙ್ಗ;
          }

	  ಪಕ್ಷ vpiIntVal: {
		long val = 0;
		vvp_bit4_t pad = BIT4_0;
		ಯದಿ (signed_flag && word_val.size() > 0)
		      pad = word_val.value(word_val.size()-1);

		ಪ್ರತಿ (unsigned idx = 0 ; idx < 8*ಪರಿಮಾಣ(val) ;  idx += 1) {
		      vvp_bit4_t val4 = pad;
		      ಯದಿ (idx < word_val.size())
			    val4 = word_val.value(idx);
		      ಯದಿ (val4 == BIT4_1)
			    val |= 1L << idx;
		}

		vp->value.integer = val;
		ಭಙ್ಗ;
	  }

	  ಪಕ್ಷ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiVectorVal;
	    // fallthrough
	  ಪಕ್ಷ vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * ಪರಿಮಾಣ(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		ಪ್ರತಿ (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      ವಿಕಲ್ಪ (word_val.value(idx)) {
			  ಪಕ್ಷ BIT4_0:
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    ಭಙ್ಗ;
			  ಪಕ್ಷ BIT4_1:
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    ಭಙ್ಗ;
			  ಪಕ್ಷ BIT4_X:
			    op->aval |= (1 << idx % 32);
			    op->bval |= (1 << idx % 32);
			    ಭಙ್ಗ;
			  ಪಕ್ಷ BIT4_Z:
			    op->aval &= ~(1 << idx % 32);
			    op->bval |=  (1 << idx % 32);
			    ಭಙ್ಗ;
		      }
		      ಯದಿ (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		ಭಙ್ಗ;
	  }

	  ಪಕ್ಷ vpiStringVal:
		vec4_get_ಮೌಲ್ಯ_string(word_val, width, vp);
		ಭಙ್ಗ;

	  ಪಕ್ಷ vpiRealVal:
		vector4_to_ಮೌಲ್ಯ(word_val, vp->value.real, signed_flag);
		ಭಙ್ಗ;
      }
}

ಶೂನ್ಯ vpip_vec2_get_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector2_t&word_val, unsigned width,
			 ತಾರ್ಕಿಕ signed_flag, s_vpi_ಮೌಲ್ಯ*vp)
{
      ವಿಕಲ್ಪ (vp->format) {
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector2 values.\n", (ಪೂರ್ಣಾಙ್ಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);

	  ಪಕ್ಷ vpiSuppressVal:
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiIntVal:
	    vector2_to_ಮೌಲ್ಯ(word_val, vp->value.integer, signed_flag);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  ಪಕ್ಷ vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * ಪರಿಮಾಣ(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		ಪ್ರತಿ (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      ಯದಿ (word_val.value(idx)) {
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      } ಅನ್ಯಥಾ {
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      }
		      ಯದಿ (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		ಭಙ್ಗ;
	  }
      }

}

/*
 * Convert a real value to the appropriate integer.
 */
ಸ್ಥಿರ PLI_INT32 get_real_as_int(ದ್ವಿಗುಣ real)
{
      ದ್ವಿಗುಣ rtn;

	/* We would normally want to return 'bx for a NaN or
	 * +/- infinity, but for an integer the standard says
	 * to convert 'bx to 0, so we just return 0. */
      ಯದಿ (real != real || (real && (real == 0.5*real))) {
	    ಪ್ರತ್ಯಾಗಮ 0;
      }

      rtn = std::round(real);

      ಪ್ರತ್ಯಾಗಮ (PLI_INT32) rtn;
}

/*
 * This is a generic function to convert a double value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer.
 */
ಶೂನ್ಯ vpip_real_get_ಮೌಲ್ಯ(ದ್ವಿಗುಣ real, s_vpi_ಮೌಲ್ಯ*vp)
{
      ವರ್ಣ *rbuf = 0;

      ವಿಕಲ್ಪ (vp->format) {
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting real values.\n", (ಪೂರ್ಣಾಙ್ಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);

	  ಪಕ್ಷ vpiSuppressVal:
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiRealVal;
	    // fallthrough
	  ಪಕ್ಷ vpiRealVal:
	    vp->value.real = real;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiIntVal:
	    vp->value.integer = get_real_as_int(real);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiDecStrVal:
	    rbuf = static_cast<ವರ್ಣ *>(need_result_buf(1025, RBUF_VAL));
	    vpip_vec4_to_dec_str(vvp_vector4_t(1024, real), rbuf, 1025, ಸತ್ಯ);
	    vp->value.str = rbuf;
	    ಭಙ್ಗ;
      }
}

ದ್ವಿಗುಣ real_from_vpi_ಮೌಲ್ಯ(s_vpi_ಮೌಲ್ಯ*vp)
{
      vvp_vector4_t vec4(1024);
      ದ್ವಿಗುಣ result;
      ತಾರ್ಕಿಕ is_signed = ಅಸತ್ಯ;

      ವಿಕಲ್ಪ (vp->format) {
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "putting real values.\n", (ಪೂರ್ಣಾಙ್ಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);

	  ಪಕ್ಷ vpiRealVal:
	    result = vp->value.real;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiIntVal:
	    result = (ದ್ವಿಗುಣ) vp->value.integer;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiBinStrVal:
	    vpip_bin_str_to_vec4(vec4, vp->value.str);
	    ಯದಿ (vp->value.str[0] == '-') is_signed = ಸತ್ಯ;
	    vector4_to_ಮೌಲ್ಯ(vec4, result, is_signed);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiOctStrVal:
	    vpip_oct_str_to_vec4(vec4, vp->value.str);
	    ಯದಿ (vp->value.str[0] == '-') is_signed = ಸತ್ಯ;
	    vector4_to_ಮೌಲ್ಯ(vec4, result, is_signed);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiDecStrVal:
	    vpip_dec_str_to_vec4(vec4, vp->value.str);
	    ಯದಿ (vp->value.str[0] == '-') is_signed = ಸತ್ಯ;
	    vector4_to_ಮೌಲ್ಯ(vec4, result, is_signed);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiHexStrVal:
	    vpip_hex_str_to_vec4(vec4, vp->value.str);
	    ಯದಿ (vp->value.str[0] == '-') is_signed = ಸತ್ಯ;
	    vector4_to_ಮೌಲ್ಯ(vec4, result, is_signed);
	    ಭಙ್ಗ;

      }

      ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ vpip_string_get_ಮೌಲ್ಯ(ಧ್ರುವ string&val, s_vpi_ಮೌಲ್ಯ*vp)
{
      ವರ್ಣ *rbuf = 0;

      ವಿಕಲ್ಪ (vp->format) {
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting string values.\n", (ಪೂರ್ಣಾಙ್ಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);

	  ಪಕ್ಷ vpiSuppressVal:
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiStringVal;
	    // fallthrough
	  ಪಕ್ಷ vpiStringVal:
	    rbuf = static_cast<ವರ್ಣ *>(need_result_buf(val.size() + 1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    ಭಙ್ಗ;
      }
}


ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(vpiHandle ಅಭಿವ್ಯಕ್ತಿ, s_vpi_ಮೌಲ್ಯ*vp)
{
      ಪ್ರತಿಜ್ಞಾ(ಅಭಿವ್ಯಕ್ತಿ);
      ಪ್ರತಿಜ್ಞಾ(vp);

	// Never bother with suppressed values. All the derived
	// classes can ignore this type.
      ಯದಿ (vp->format == vpiSuppressVal)
	    ಪ್ರತ್ಯಾಗಮ;

      ಅಭಿವ್ಯಕ್ತಿ->vpi_get_ಮೌಲ್ಯ(vp);

      ಯದಿ (vpi_trace) ವಿಕಲ್ಪ (vp->format) {
		ಪಕ್ಷ vpiStringVal:
		  fprintf(vpi_trace,"vpi_get_value(%p=<%d>) -> string=\"%s\"\n",
			  ಅಭಿವ್ಯಕ್ತಿ, ಅಭಿವ್ಯಕ್ತಿ->get_type_code(), vp->value.str);
		  ಭಙ್ಗ;

		ಪಕ್ಷ vpiBinStrVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> binstr=%s\n",
			  ಅಭಿವ್ಯಕ್ತಿ->get_type_code(), vp->value.str);
		  ಭಙ್ಗ;

		ಪಕ್ಷ vpiIntVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> int=%d\n",
			  ಅಭಿವ್ಯಕ್ತಿ->get_type_code(), (ಪೂರ್ಣಾಙ್ಕ)vp->value.integer);
		  ಭಙ್ಗ;

		ಪಕ್ಷ vpiSuppressVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <suppress>\n",
			  ಅಭಿವ್ಯಕ್ತಿ->get_type_code());
		  ಭಙ್ಗ;

		ಸ್ವಭಾವ:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <%d>=?\n",
			  ಅಭಿವ್ಯಕ್ತಿ->get_type_code(), (ಪೂರ್ಣಾಙ್ಕ)vp->format);
	    }
}

ಸಂರಚನಾ vpip_put_ಮೌಲ್ಯ_event : vvp_gen_event_s {
      vpiHandle handle;
      s_vpi_ಮೌಲ್ಯ value;
      ಪೂರ್ಣಾಙ್ಕ flags;
      ಆಭಾಸ ಶೂನ್ಯ run_run() override;
      ~vpip_put_ಮೌಲ್ಯ_event() override { }
};

ಶೂನ್ಯ vpip_put_ಮೌಲ್ಯ_event::run_run()
{
      handle->vpi_put_ಮೌಲ್ಯ(&value, flags);
      ವಿಕಲ್ಪ (value.format) {
	    /* Free the copied string. */
	  ಪಕ್ಷ vpiBinStrVal:
	  ಪಕ್ಷ vpiOctStrVal:
	  ಪಕ್ಷ vpiDecStrVal:
	  ಪಕ್ಷ vpiHexStrVal:
	  ಪಕ್ಷ vpiStringVal:
	    ಮುಕ್ತ(value.value.str);
	    ಭಙ್ಗ;
	    /* Free the copied time structure. */
	  ಪಕ್ಷ vpiTimeVal:
	    ಮುಕ್ತ(value.value.time);
	    ಭಙ್ಗ;
	    /* Free the copied vector structure. */
	  ಪಕ್ಷ vpiVectorVal:
	    ಮುಕ್ತ(value.value.vector);
	    ಭಙ್ಗ;
	    /* Free the copied strength structure. */
	  ಪಕ್ಷ vpiStrengthVal:
	    ಮುಕ್ತ(value.value.strength);
	    ಭಙ್ಗ;
	    /* Everything else is static in the structure. */
	  ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
      }
}

/* Make a copy of a pointer to a time structure. */
ಸ್ಥಿರ t_vpi_time *timedup(ಧ್ರುವ t_vpi_time *val)
{
      t_vpi_time *rtn;
      rtn = static_cast<t_vpi_time *> (malloc(ಪರಿಮಾಣ(t_vpi_time)));
      *rtn = *val;
      ಪ್ರತ್ಯಾಗಮ rtn;
}

/* Make a copy of a pointer to a vector value structure. */
ಸ್ಥಿರ t_vpi_vecval *vectordup(ಧ್ರುವ t_vpi_vecval *val, PLI_INT32 size)
{
      unsigned ಅಂಕೆ_bytes;
      t_vpi_vecval *rtn;
      ಪ್ರತಿಜ್ಞಾ(size > 0);
      ಅಂಕೆ_bytes = ((size + 31)/32)*ಪರಿಮಾಣ(t_vpi_vecval);
      rtn = static_cast<t_vpi_vecval *> (malloc(ಅಂಕೆ_bytes));
      memcpy(rtn, val, ಅಂಕೆ_bytes);
      ಪ್ರತ್ಯಾಗಮ rtn;
}

/* Make a copy of a pointer to a strength structure. */
ಸ್ಥಿರ t_vpi_strengthval *strengthdup(ಧ್ರುವ t_vpi_strengthval *val)
{
      t_vpi_strengthval *rtn;
      rtn = static_cast<t_vpi_strengthval *>
            (malloc(ಪರಿಮಾಣ(t_vpi_strengthval)));
      *rtn = *val;
      ಪ್ರತ್ಯಾಗಮ rtn;
}

vpiHandle vpi_put_ಮೌಲ್ಯ(vpiHandle obj, s_vpi_ಮೌಲ್ಯ*vp,
			s_vpi_time*when, PLI_INT32 flags)
{
      ಪ್ರತಿಜ್ಞಾ(obj);

      flags &= ~vpiReturnEvent;

      ಯದಿ (flags!=vpiNoDelay && flags!=vpiForceFlag && flags!=vpiReleaseFlag) {
	    vvp_time64_t dly;
	    ಪೂರ್ಣಾಙ್ಕ scale;

	    ಯದಿ (vpi_get(vpiAutomatic, obj)) {
		  fprintf(stderr, "VPI error: cannot put a value with "
				  "a delay on automatically allocated "
				  "variable '%s'.\n",
				  vpi_get_str(vpiName, obj));
		  ಪ್ರತ್ಯಾಗಮ 0;
	    }

	    ಪ್ರತಿಜ್ಞಾ(when != 0);

	    ವಿಕಲ್ಪ (when->type) {
		ಪಕ್ಷ vpiScaledRealTime:
		  scale = vpip_time_units_from_handle(obj) -
		          vpip_get_time_precision();
		  ಯದಿ (scale >= 0) {
			dly = (vvp_time64_t)(when->real * pow(10.0, scale));
		  } ಅನ್ಯಥಾ {
			dly = (vvp_time64_t)(when->real / pow(10.0, -scale));
		  }
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiSimTime:
		  dly = vpip_timestruct_to_time(when);
		  ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		  dly = 0;
		  ಭಙ್ಗ;
	    }

	    ಯದಿ ((dly == 0) && schedule_at_rosync()) {
		  fprintf(stderr, "VPI error: attempted to put a value to "
				  "variable '%s' during a read-only synch "
				  "callback.\n", vpi_get_str(vpiName, obj));
		  ಪ್ರತ್ಯಾಗಮ 0;
	    }

	    vpip_put_ಮೌಲ್ಯ_event*put = ನೂತನ vpip_put_ಮೌಲ್ಯ_event;
	    put->handle = obj;
	    ಯದಿ (dynamic_cast<__vpiNamedEvent*>(obj)) {
		  put->value.format = vpiIntVal;
		  put->value.value.integer = 0;
	    } ಅನ್ಯಥಾ {
		  ಪ್ರತಿಜ್ಞಾ(vp);
		  put->value = *vp;
	    }
	      /* Since this is a scheduled put event we must copy any pointer
	       * data to keep it available until the event is actually run. */
	    ವಿಕಲ್ಪ (put->value.format) {
		  /* Copy the string items. */
		ಪಕ್ಷ vpiBinStrVal:
		ಪಕ್ಷ vpiOctStrVal:
		ಪಕ್ಷ vpiDecStrVal:
		ಪಕ್ಷ vpiHexStrVal:
		ಪಕ್ಷ vpiStringVal:
		  put->value.value.str = strdup(put->value.value.str);
		  ಭಙ್ಗ;
		  /* Copy a time pointer item. */
		ಪಕ್ಷ vpiTimeVal:
		  put->value.value.time = timedup(put->value.value.time);
		  ಭಙ್ಗ;
		  /* Copy a vector pointer item. */
		ಪಕ್ಷ vpiVectorVal:
		  put->value.value.vector = vectordup(put->value.value.vector,
		                                      vpi_get(vpiSize, obj));
		  ಭಙ್ಗ;
		  /* Copy a strength pointer item. */
		ಪಕ್ಷ vpiStrengthVal:
		  put->value.value.strength =
		      strengthdup(put->value.value.strength);
		  ಭಙ್ಗ;
		  /* Everything thing else is already in the structure. */
		ಸ್ವಭಾವ:
		  ಭಙ್ಗ;
	    }
	    put->flags = flags;
	    schedule_generic(put, dly, ಅಸತ್ಯ, ಸತ್ಯ, ಸತ್ಯ);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }

      ಯದಿ (schedule_at_rosync()) {
            fprintf(stderr, "VPI error: attempted to put a value to "
			    "variable '%s' during a read-only synch "
			    "callback.\n", vpi_get_str(vpiName, obj));
            ಪ್ರತ್ಯಾಗಮ 0;
      }

      obj->vpi_put_ಮೌಲ್ಯ(vp, flags);

      ಪ್ರತ್ಯಾಗಮ 0;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle res = 0;

      ಯದಿ (ref == 0) {
	      // A few types can apply to a nil handle. These are ways
	      // that the current function can get started finding things.
	    ವಿಕಲ್ಪ (type) {

		ಪಕ್ಷ vpiScope:
		    // The IEEE1364-2005 doesn't seem to allow this,
		    // but some users seem to think it's handy, so
		    // return the scope that contains this SysTfCall.
		  ಪ್ರತಿಜ್ಞಾ(vpip_cur_task);
		  res = vpip_cur_task->vpi_handle(vpiScope);
		  ಭಙ್ಗ;

		ಪಕ್ಷ vpiSysTfCall:
		    // This is how VPI users get a first handle into
		    // the system. This is the handle of the system
		    // task/function call currently being executed.
		  ಯದಿ (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle(vpiSysTfCall, 0) "
				"-> %p (%s)\n", vpip_cur_task,
				vpip_cur_task->defn->info.tfname);
		  }
		  ಪ್ರತ್ಯಾಗಮ vpip_cur_task;

		ಸ್ವಭಾವ:
		  fprintf(stderr, "VPI error: vpi_handle(type=%d, ref=0).\n",
			  (ಪೂರ್ಣಾಙ್ಕ)type);
		  res = 0;
		  ಭಙ್ಗ;
	    }

      } ಅನ್ಯಥಾ {

	    ಯದಿ (type == vpiSysTfCall) {
		  fprintf(stderr, "VPI error: vpi_handle(vpiSysTfCall, "
			  "ref!=0).\n");
		  ಪ್ರತ್ಯಾಗಮ 0;
	    }

	    res = ref->vpi_handle(type);
      }


      ಯದಿ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle(vpiScope, ref=%p) "
		    "-> %p\n", vpip_cur_task, ref);
      }

      ಪ್ರತ್ಯಾಗಮ res;
}

ಸ್ಥಿರ vpiHandle vpip_make_udp_iterator()
{
// HERE: Add support for iterating over UDP definitions.
//       See 26.6.16 (page 400 in 1364-2005).
      ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * This function asks the object to return an iterator for
 * the specified reference. It is up to the iterate_ method to
 * allocate a properly formed iterator.
 */
ಸ್ಥಿರ vpiHandle vpi_iterate_global(ಪೂರ್ಣಾಙ್ಕ type)
{
      ವಿಕಲ್ಪ (type) {
	  ಪಕ್ಷ vpiInstance:
	    // fallthrough
	  ಪಕ್ಷ vpiModule:
	    // fallthrough
	  ಪಕ್ಷ vpiProgram:
	    // fallthrough
	  ಪಕ್ಷ vpiInterface:
	    // fallthrough
	  ಪಕ್ಷ vpiPackage:
	    ಪ್ರತ್ಯಾಗಮ vpip_make_ಬೇರು_iterator(type);

	  ಪಕ್ಷ vpiUdpDefn:
	    ಪ್ರತ್ಯಾಗಮ vpip_make_udp_iterator();

	  ಪಕ್ಷ vpiUserSystf:
	    ಪ್ರತ್ಯಾಗಮ vpip_make_systf_iterator();
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle rtn = 0;

      ಪ್ರತಿಜ್ಞಾ(vpi_mode_flag != VPI_MODE_NONE);
      ಯದಿ (vpi_mode_flag == VPI_MODE_REGISTER) {
	    fprintf(stderr, "vpi error: vpi_iterate called during "
		    "vpi_register_systf. You can't do that!\n");
	    ಪ್ರತ್ಯಾಗಮ 0;
      }

      ಯದಿ (ref == 0)
	    rtn = vpi_iterate_global(type);
      ಅನ್ಯಥಾ
	    rtn = ref->vpi_iterate(type);

      ಯದಿ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_iterate(%d, %p) ->%s\n",
	    (ಪೂರ್ಣಾಙ್ಕ)type, ref, rtn ? "" : " (null)");
      }

      ಪ್ರತ್ಯಾಗಮ rtn;
}

vpiHandle vpi_handle_by_index(vpiHandle ref, PLI_INT32 idx)
{
      ಪ್ರತಿಜ್ಞಾ(ref);
      ಪ್ರತ್ಯಾಗಮ ref->vpi_index(idx);
}

ಸ್ಥಿರ vpiHandle find_name(ಧ್ರುವ ವರ್ಣ *name, vpiHandle handle)
{
      vpiHandle rtn = 0;
      __vpiScope*ref = dynamic_cast<__vpiScope*>(handle);

      /* check module names */
      ಯದಿ (!strcmp(name, vpi_get_str(vpiName, handle)))
	    rtn = handle;

      /* brute force search for the name in all objects in this scope */
      ಪ್ರತಿ (unsigned i = 0 ;  i < ref->intern.size() ;  i += 1) {
	      /* The standard says that since a port does not have a full
	       * name it cannot be found by name. Because of this we need
	       * to skip ports here so the correct handle can be located. */
	    ಯದಿ (vpi_get(vpiType, ref->intern[i]) == vpiPort) ಅನುವರ್ತ;
	    ಧ್ರುವ ವರ್ಣ *nm = vpi_get_str(vpiName, ref->intern[i]);
	    ಯದಿ (nm && !strcmp(name, nm)) {
		  rtn = ref->intern[i];
		  ಭಙ್ಗ;
	    } ಅನ್ಯಥಾ ಯದಿ (vpi_get(vpiType, ref->intern[i]) == vpiMemory ||
	               vpi_get(vpiType, ref->intern[i]) == vpiNetArray) {
		  /* We need to iterate on the words */
		  vpiHandle word_i, word_h;
		  word_i = vpi_iterate(vpiMemoryWord, ref->intern[i]);
		  ಯಾವತ್ (word_i && (word_h = vpi_scan(word_i))) {
			nm = vpi_get_str(vpiName, word_h);
			ಯದಿ (nm && !strcmp(name, nm)) {
			      rtn = word_h;
			      vpi_free_object(word_i);
			      ಭಙ್ಗ;
			}
		  }
	    }
	    /* found it yet? */
	    ಯದಿ (rtn) ಭಙ್ಗ;
      }

      ಪ್ರತ್ಯಾಗಮ rtn;
}

// Find the end of the escaped identifier or simple identifier
ಸ್ಥಿರ ವರ್ಣ * find_rest(ವರ್ಣ *name)
{
      ವರ್ಣ *rest;
      ಯದಿ (*name == '\\') {
	    rest = strchr(name, ' ');
	    ಯದಿ (rest) {
		  *rest++ = 0; // The space is not part of the string
		    // There should be a '.' after the escaped ID if there is
		    // anything more. If it is missing add it to avoid a crash
		  ಯದಿ ((*rest != '.') && (*rest != 0)) {
			*--rest = '.';
			fprintf(stderr, "ERROR: Malformed scope string: \"%s\"", name);
		  }
		  ಯದಿ (*rest == 0) {
			rest = 0;
		  }
	    }
      } ಅನ್ಯಥಾ {
	    rest = strchr(name, '.');
      }
      ಪ್ರತ್ಯಾಗಮ rest;
}

ಸ್ಥಿರ vpiHandle find_scope(ಧ್ರುವ ವರ್ಣ *name, vpiHandle handle, ಪೂರ್ಣಾಙ್ಕ depth)
{

      vpiHandle iter = handle==0
	    ? vpi_iterate(vpiModule, ರಿಕ್ತ)
	    : vpi_iterate(vpiInternalScope, handle);

      vector<ವರ್ಣ> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      ವರ್ಣ*nm_first = &name_buf[0];
      ವರ್ಣ*nm_rest = find_rest(nm_first);
      ಯದಿ (*nm_first == '\\') ++nm_first;
      ಯದಿ (nm_rest) {
	    *nm_rest++ = 0;
      }

      vpiHandle rtn = 0;
      vpiHandle hand;
      ಯಾವತ್ (iter && (hand = vpi_scan(iter))) {
	    ಧ್ರುವ ವರ್ಣ *nm = vpi_get_str(vpiName, hand);

	    ಯದಿ (strcmp(nm_first,nm)==0) {
		  ಯದಿ (nm_rest)
			rtn=find_scope(nm_rest, hand, depth+1);
		  ಅನ್ಯಥಾ
			rtn = hand;
	    }

	    /* found it yet ? */
	    ಯದಿ (rtn) {
		  vpi_free_object(iter);
		  ಭಙ್ಗ;
	    }
      }

      ಪ್ರತ್ಯಾಗಮ rtn;
}

// Find the end of the first escaped identifier or simple identifier
ಸ್ಥಿರ ವರ್ಣ * find_next(ವರ್ಣ *name)
{
      ವರ್ಣ *next;
      ಯದಿ (*name == '\\') {
	    next = strchr(name, ' ');
	    ಯದಿ (next && *++next == 0) next = 0;
      } ಅನ್ಯಥಾ {
	    next = strchr(name, '.');
      }
      ಪ್ರತ್ಯಾಗಮ next;
}

vpiHandle vpi_handle_by_name(ಧ್ರುವ ವರ್ಣ *name, vpiHandle scope)
{
      vpiHandle hand;

      ಯದಿ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name(%s, %p) -->\n",
		    name, scope);
      }

	// Chop the name into path and base. For example, if the name
	// is "a.b.c", then nm_path becomes "a.b" and nm_base becomes
	// "c". If the name is "c" then nm_path is nil and nm_base is "c".
      vector<ವರ್ಣ> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      ವರ್ಣ*nm_path = &name_buf[0];
      ವರ್ಣ*nm_base;

	// Check to see if we have an escaped identifier and if so search
	// the long way because a '.' could be in the escaped name.
      ಯದಿ (strchr(nm_path, '\\')) {
	    ವರ್ಣ *next;
	    nm_base = nm_path;
	    ಯಾವತ್ ((next = find_next(nm_base))) {
		  nm_base = ++next;
	    }
	    ಯದಿ (nm_path == nm_base) {
		  nm_path = 0;
	    } ಅನ್ಯಥಾ {
		  *(nm_base-1) = 0;
	    }

	// If there is no escaped identifier then just look for the last '.'
      } ಅನ್ಯಥಾ {
	    nm_base = strrchr(nm_path, '.');
	    ಯದಿ (nm_base) {
		  *nm_base++ = 0;
	    } ಅನ್ಯಥಾ {
		  nm_base = nm_path;
		  nm_path = 0;
	    }
      }

      /* If scope provided, look in corresponding module; otherwise
       * traverse the hierarchy specified in name to find the leaf module
       * and try finding it there.
       */
      ಯದಿ (scope) {
	    /* Some implementations support either a module or a scope. */
	    ವಿಕಲ್ಪ (vpi_get(vpiType, scope)) {
		ಪಕ್ಷ vpiScope:
	          hand = vpi_handle(vpiModule, scope);
	          ಭಙ್ಗ;
		ಪಕ್ಷ vpiModule:
	          hand = scope;
	          ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		  ಯದಿ (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle_by_name: "
				"Scope is not a vpiScope or vpiModule\n");
		  }
	          // Use vpi_chk_error() here when it is implemented.
	          ಪ್ರತ್ಯಾಗಮ 0;
	    }

      } ಅನ್ಯಥಾ ಯದಿ (nm_path) {
	      // The name has a path, and no other scope handle was
	      // passed in. That suggests we are looking for "a.b.c"
	      // in the root scope. So convert "a.b" to a scope and
	      // start there to look for "c".
	    hand = find_scope(nm_path, ರಿಕ್ತ, 0);
	    nm_path = 0;

      } ಅನ್ಯಥಾ {
	      // Special case: scope==<nil>, meaning we are looking in
	      // the root, and there is no path to the name, i.e. the
	      // string is "c" instead of "top.c". Try to find "c" as
	      // a scope and return that.
	    hand = find_scope(nm_base, ರಿಕ್ತ, 0);
      }

      ಯದಿ (hand == 0) {
	    ಯದಿ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_handle_by_name: "
			  "Scope does not exist. Giving up.\n");
	    }

	    ಪ್ರತ್ಯಾಗಮ 0;
      }

	// If there is a path part, then use it to find the
	// scope. For example, if the full name is a.b.c, then
	// the nm_path string is a.b and we search for that
	// scope. If we find it, then set hand to that scope.
      ಯದಿ (nm_path) {
	    vpiHandle tmp = find_scope(nm_path, hand, 0);
	    ಯಾವತ್ (tmp == 0 && hand != 0) {
		  hand = vpi_handle(vpiScope, hand);
		  tmp = find_scope(nm_path, hand, 0);
	    }
	    hand = tmp;
      }

	// find_name() expects escaped identifiers to be stripped
      ಯದಿ (*nm_base == '\\') {
	    // Skip the \ at the beginning
	    ++nm_base;

	    // Drop the space at the end if it exists
	    ವರ್ಣ *next;
	    ಯದಿ ((next = strchr(nm_base, ' '))) {
		  *next = 0;
	    }
      }

	// Now we have the correct scope, look for the item.
      vpiHandle out = find_name(nm_base, hand);

      ಯದಿ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name: DONE\n");
      }

      ಪ್ರತ್ಯಾಗಮ out;
}

// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected
// x2: Ptr to previous node
// x3: True when net is connected to output of previous node
tuple<ತಾರ್ಕಿಕ, vvp_net_ptr_t, ತಾರ್ಕಿಕ> check_connected_to_concat8_and_part_sa(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent) {

      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      ಯಾವತ್ (cur.ptr()) {
	    ಯದಿ (cur.ptr() == net2 && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
		    // Pass on the return value if found
		  ಪ್ರತ್ಯಾಗಮ {ಸತ್ಯ, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
	    } ಅನ್ಯಥಾ ಯದಿ (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8_and_part_sa(cur.ptr()->out_, net2, cur);
		  ಯದಿ (get<0>(res)) {
			ಪ್ರತ್ಯಾಗಮ res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      ಪ್ರತ್ಯಾಗಮ {ಅಸತ್ಯ, vvp_net_ptr_t(nullptr, 0), ಅಸತ್ಯ};
}

// Check if net2 is connected to current_net through a net of vvp_fun_concat8s
// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected with concats and part_sas
// x2: Ptr to previous node
// x3: True when net is connected to output of x2
tuple<ತಾರ್ಕಿಕ, vvp_net_ptr_t, ತಾರ್ಕಿಕ> check_connected_to_concat8(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent)
{
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);
	// For everything connected
      ಯಾವತ್ (cur.ptr()) {
	      // Check if it's a concat8
	    ಯದಿ (cur.ptr() == net2) {
		  ಪ್ರತ್ಯಾಗಮ {ಸತ್ಯ, vvp_net_ptr_t(nullptr, 0), ಅಸತ್ಯ};
	    } ಅನ್ಯಥಾ ಯದಿ (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8(cur.ptr()->out_, net2, cur);
		  ಯದಿ (get<0>(res)) {
			ಯದಿ (!get<1>(res).ptr() && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
			      // `parent` is the last part_sa node in front of our destination
			      // We want to place the intermod node here
			      ಪ್ರತ್ಯಾಗಮ {ಸತ್ಯ, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
			}
			ಪ್ರತ್ಯಾಗಮ res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      ಪ್ರತ್ಯಾಗಮ {ಅಸತ್ಯ, vvp_net_ptr_t(nullptr, 0), ಅಸತ್ಯ};
}

// Used to get intermodpath for two ports
vpiHandle vpi_handle_multi(PLI_INT32 type,
                           vpiHandle ref1,
                           vpiHandle ref2)
{
      ಯದಿ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_multi(%d, %p, %p) -->\n",
	            type, ref1, ref2);
      }

      ಯದಿ (type != vpiInterModPath) {
	    fprintf(stderr, "sorry: vpi_handle_multi currently supports"
	            "only vpiInterModPath\n");
	    ಪ್ರತ್ಯಾಗಮ nullptr;
      }

	// Indicates whether port1 refers to a single bit
      ತಾರ್ಕಿಕ port1_has_index = ಅಸತ್ಯ;
      ಪೂರ್ಣಾಙ್ಕ port1_bit_index = 0;
      vpiPortBitInfo* port1_bit = dynamic_cast<vpiPortBitInfo*>(ref1);

      ಯದಿ (port1_bit) {
	      // Get the bit index
	    port1_has_index = ಸತ್ಯ;
	    port1_bit_index = vpi_get(vpiBit, port1_bit);
	      // Update the ref1 to point to the base port
	    ref1 = vpi_handle(vpiParent, port1_bit);
      }

	// Indicates whether port2 refers to a single bit
      ತಾರ್ಕಿಕ port2_has_index = ಅಸತ್ಯ;
      ಪೂರ್ಣಾಙ್ಕ port2_bit_index = 0;
      vpiPortBitInfo* port2_bit = dynamic_cast<vpiPortBitInfo*>(ref2);

      ಯದಿ (port2_bit) {
	      // Get the bit index
	    port2_has_index = ಸತ್ಯ;
	    port2_bit_index = vpi_get(vpiBit, port2_bit);
	      // Update the ref1 to point to the base port
	    ref2 = vpi_handle(vpiParent, port2_bit);
      }

      vpiPortInfo* port1 = dynamic_cast<vpiPortInfo*>(ref1);

      ಯದಿ (!port1) {
	    fprintf(stderr, "sorry: second argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    ಪ್ರತ್ಯಾಗಮ nullptr;
      }

      vpiPortInfo* port2 = dynamic_cast<vpiPortInfo*>(ref2);

      ಯದಿ (!port2) {
	    fprintf(stderr, "sorry: third argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    ಪ್ರತ್ಯಾಗಮ nullptr;
      }

	// Get the names of both ports
      std::string port1_name(vpi_get_str(vpiName, ref1));
      std::string port2_name(vpi_get_str(vpiName, ref2));

	// If both ports are vpiOutput, we have to reassign the __vpiSignal from port1
	// to port2 because otherwise the non-delayed version of the signal is dumped
	// even tho the intermodpath is correctly inserted
      __vpiSignal* output_signal = nullptr;

      ಯದಿ (port1->get_direction() == vpiOutput && port2->get_direction() == vpiOutput) {
	    vpiHandle scope_port2 = vpi_handle(vpiScope, ref2);
	    ಪ್ರತಿಜ್ಞಾ(scope_port2);

	      // Iterate over nets in the scope of port2
	    vpiHandle net_i = vpi_iterate(vpiNet, scope_port2) ;
	    vpiHandle net;

	    ಯಾವತ್ ((net = vpi_scan(net_i)) != ರಿಕ್ತ) {
		  std::string net_name(vpi_get_str(vpiName, net));

		    // Compare whether the net matches with the port name
		  ಯದಿ (net_name == port2_name) {
			output_signal = dynamic_cast<__vpiSignal*>(net);
		  }
	    }
      }

      vvp_net_t* net1 = port1->get_port();
      vvp_net_t* net2 = port2->get_port();

      ಯದಿ (net1 == nullptr || net2 == nullptr) {
	    fprintf(stderr, "Error: Could not find net. "
		    "Did you run iverilog with '-ginterconnect'?\n");
	    ಪ್ರತ್ಯಾಗಮ nullptr;
      }

      ಯದಿ (net1 == net2) {
	    fprintf(stderr, "Error: Net for both ports is the same. "
		    "Did you pass the same port twice?\n");
	    ಪ್ರತ್ಯಾಗಮ nullptr;
      }

      ಯದಿ (!dynamic_cast<vvp_fun_buft*>(net1->fun)) {
	    fprintf(stderr, "Error: functor of net1 must be"
		    "vvp_fun_buft\n");
	    ಪ್ರತ್ಯಾಗಮ nullptr;
      }

      ಯದಿ (!dynamic_cast<vvp_fun_buft*>(net2->fun)) {
	    fprintf(stderr, "Error: functor of net2 must be"
		    "vvp_fun_buft\n");
	    ಪ್ರತ್ಯಾಗಮ nullptr;
      }

	// If port1 is actually a port bit, we have to get to the correct vvp_fun_part
	// after which we insert the intermodpath delay
      ಯದಿ (port1_has_index) {
	    vvp_net_ptr_t* net1_ptr = &net1->out_;

	      // Search for part selects connected to port1
	    vvp_net_t* current_net = net1_ptr->ptr();

	    ಯಾವತ್ (current_net) {
		  ಧ್ರುವ vvp_fun_part* part = dynamic_cast<vvp_fun_part*>(current_net->fun);

		    // Its a part select!
		  ಯದಿ (part) {
			  // Is it the correct part select?
			ಯದಿ (part->get_base() == (unsigned)port1_bit_index) {
			      ಪ್ರತಿಜ್ಞಾ(part->get_wid() == 1);
			      net1 = current_net; // Replace net1 as this is our new start point
			      ಭಙ್ಗ;
			}
		  }

		  current_net = current_net->port[0].ptr(); // BUFT has only one input, index 0
	    }
      }

	// Iterate over all nodes connected to port1
      vvp_net_ptr_t cur = net1->out_;
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      ಯಾವತ್ (cur.ptr()) {
	      // Either port2 is directly connected to port1
	      // Or in the second case port2 is indirectly connected
	      // to port1 through a net of concat8s
	    ತಾರ್ಕಿಕ is_connected, is_output;
	    vvp_net_ptr_t previous_ಗಂಟು;
	    tie(is_connected, previous_ಗಂಟು, is_output) = port2_has_index
		  ? check_connected_to_concat8(cur, net2, vvp_net_ptr_t(net1, 0))
		  : check_connected_to_concat8_and_part_sa(cur, net2, vvp_net_ptr_t(net1, 0));
	    ಯದಿ ( (!port2_has_index && cur.ptr() == net2) || is_connected ) {
		  vvp_net_t*new_net = ನೂತನ vvp_net_t;

		    // Create new node with intermodpath and connect port2 to it
		  ಪೂರ್ಣಾಙ್ಕ width = 1; // TODO
		  vvp_fun_intermodpath*obj = ನೂತನ vvp_fun_intermodpath(new_net, width);

		  vvp_net_t* output_net = net1;
		  ಯದಿ (is_connected) {
			ಯದಿ (is_output) {
			      output_net = previous_ಗಂಟು.ptr(); // net2 is direct output of part_sa
			      cur = output_net->out_;
			      prev = vvp_net_ptr_t(nullptr, 0);
			} ಅನ್ಯಥಾ {
			      prev = previous_ಗಂಟು;
			      ಯದಿ (prev.ptr()) {
				    cur = prev.ptr()->port[prev.port()];
			      }
			}
		  }

		  new_net->fun = obj;
		  new_net->out_ = cur;

		    // Port2 is in the middle of the list
		    // Insert intermodpath before port2 and keep everything else intact
		  ಯದಿ (prev.ptr()) {
			prev.ptr()->port[prev.port()] = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; // Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		    // Port2 is first in list
		    // Insert intermodpath before port2 and keep everything else intact
		  } ಅನ್ಯಥಾ {
			output_net->out_ = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; //  Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		  }

		    // If both ports are vpiOutput and port2 is not a vector,
		    // we have to reassign the __vpiSignal so that the delayed
		    // values get dumped
		  ಯದಿ (output_signal && !port2_has_index) {
			net2->fil = net1->fil;
			net1->fil = nullptr;
			output_signal->ಗಂಟು = net2;
		  }

		    // Create the VPI intermodpath object
		  __vpiInterModPath* intermodpath = vpip_make_intermodpath(new_net, port1, port2);
		  intermodpath->intermodpath = obj;

		    // Finally done, return the intermodpath object
		  ಪ್ರತ್ಯಾಗಮ intermodpath;
	    }

	    prev = cur;
	    cur = cur.ptr()->port[cur.port()]; // Next net in linked list
      }

      fprintf(stderr, "VPI error: Could not insert intermodpath!\n");
      fprintf(stderr, "\tport1 = %s, port1_has_index = %d, port1_bit_index = %d\n", port1_name.c_str(), port1_has_index, port1_bit_index);
      fprintf(stderr, "\tport2 = %s, port2_has_index = %d, port2_bit_index = %d\n", port2_name.c_str(), port2_has_index, port2_bit_index);
      ಪ್ರತ್ಯಾಗಮ nullptr;
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



ಶೂನ್ಯ vpi_get_delays(vpiHandle ಅಭಿವ್ಯಕ್ತಿ, p_vpi_delay delays)
{
      ಪ್ರತಿಜ್ಞಾ(ಅಭಿವ್ಯಕ್ತಿ);
      ಪ್ರತಿಜ್ಞಾ(delays);

      ಅಭಿವ್ಯಕ್ತಿ->vpi_get_delays(delays);

      ಯದಿ (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_get_delays(%p, %p) -->\n", ಅಭಿವ್ಯಕ್ತಿ, delays);
      }
}


ಶೂನ್ಯ vpi_put_delays(vpiHandle ಅಭಿವ್ಯಕ್ತಿ, p_vpi_delay delays)
{
      ಪ್ರತಿಜ್ಞಾ(ಅಭಿವ್ಯಕ್ತಿ  );
      ಪ್ರತಿಜ್ಞಾ(delays );

      ಅಭಿವ್ಯಕ್ತಿ->vpi_put_delays(delays);

      ಯದಿ (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_put_delays(%p, %p) -->\n", ಅಭಿವ್ಯಕ್ತಿ, delays);
      }
}








ಬಾಹ್ಯ "C" PLI_INT32 vpi_vprintf(ಧ್ರುವ ವರ್ಣ*fmt, va_list ap)
{
      ಪ್ರತ್ಯಾಗಮ vpi_mcd_vprintf(1, fmt, ap);
}

ಬಾಹ್ಯ "C" PLI_INT32 vpi_printf(ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      ಪೂರ್ಣಾಙ್ಕ r = vpi_mcd_vprintf(1, fmt, ap);
      va_end(ap);
      ಪ್ರತ್ಯಾಗಮ r;
}

ಬಾಹ್ಯ "C" PLI_INT32 vpi_flush(ಶೂನ್ಯ)
{
      ಪ್ರತ್ಯಾಗಮ vpi_mcd_flush(1);
}


ಬಾಹ್ಯ "C" PLI_INT32 vpi_sim_vcontrol(ಪೂರ್ಣಾಙ್ಕ operation, va_list ap)
{
      long diag_ಸಂದೇಶ;

      ವಿಕಲ್ಪ (operation) {
	  ಪಕ್ಷ vpiFinish:
	  ಪಕ್ಷ __ivl_legacy_vpiFinish:
            diag_ಸಂದೇಶ = va_arg(ap, PLI_INT32);
	    schedule_finish(diag_ಸಂದೇಶ);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiStop:
	  ಪಕ್ಷ __ivl_legacy_vpiStop:
            diag_ಸಂದೇಶ = va_arg(ap, PLI_INT32);
	    schedule_stop(diag_ಸಂದೇಶ);
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Unsupported VPI control operation %d.\n", operation);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
      ಪ್ರತ್ಯಾಗಮ 1;
}

ಬಾಹ್ಯ "C" PLI_INT32 vpi_sim_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      ಪೂರ್ಣಾಙ್ಕ r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      ಪ್ರತ್ಯಾಗಮ r;
}

ಬಾಹ್ಯ "C" PLI_INT32 vpi_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      ಪೂರ್ಣಾಙ್ಕ r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * This routine calculated the return value for $clog2.
 * It is easier to do it here vs trying to to use the VPI interface.
 */
ಬಾಹ್ಯ "C" s_vpi_vecval vpip_calc_clog2(vpiHandle arg)
{
      s_vpi_vecval rtn;
      s_vpi_ಮೌಲ್ಯ val;
      vvp_vector4_t vec4;
      ತಾರ್ಕಿಕ is_neg = ಅಸತ್ಯ;  // At this point only a real can be negative.

	/* Get the value as a vvp_vector4_t. */
      val.format = vpiObjTypeVal;
      vpi_get_ಮೌಲ್ಯ(arg, &val);
      ಯದಿ (val.format == vpiRealVal) {
	    vpi_get_ಮೌಲ್ಯ(arg, &val);
	      /* All double values can be represented in 1024 bits. */
	    vec4 = vvp_vector4_t(1024, val.value.real);
	    ಯದಿ (val.value.real < 0) is_neg = ಸತ್ಯ;
      } ಅನ್ಯಥಾ {
	    val.format = vpiVectorVal;
	    vpi_get_ಮೌಲ್ಯ(arg, &val);
	    unsigned wid = vpi_get(vpiSize, arg);
	    vec4 = vvp_vector4_t(wid, BIT4_0);
	    ಪ್ರತಿ (unsigned idx=0; idx < wid; idx += 1) {
		PLI_INT32 aval = val.value.vector[idx/32].aval;
		PLI_INT32 bval = val.value.vector[idx/32].bval;
		aval >>= idx % 32;
		bval >>= idx % 32;
		ಪೂರ್ಣಾಙ್ಕ bitmask = (aval&1) | ((bval<<1)&2);
		vvp_bit4_t bit = scalar_to_bit4(bitmask);
		vec4.set_bit(idx, bit);
	    }
      }

      ಯದಿ (vec4.has_xz()) {
	    rtn.aval = rtn.bval = 0xFFFFFFFFU;  /* Set to 'bx. */
	    ಪ್ರತ್ಯಾಗಮ rtn;
      }

      vvp_vector2_t vec2(vec4);

      ಯದಿ (is_neg) vec2.trim_neg(); /* This is a special trim! */
      ಅನ್ಯಥಾ vec2.trim(); /* This makes less work shifting. */

	/* Calculate the clog2 result. */
      PLI_INT32 res = 0;
      ಯದಿ (!vec2.is_zero()) {
	    vec2 -= vvp_vector2_t(1, vec2.size());
	    ಯಾವತ್(!vec2.is_zero()) {
		  res += 1;
		  vec2 >>= 1;
	    }
      }

      rtn.aval = res;
      rtn.bval = 0;
      ಪ್ರತ್ಯಾಗಮ rtn;
}

/*
 * This routine provides the information needed to implement $countdrivers.
 * It is done here for performance reasons - interrogating the drivers
 * individually via the VPI interface would be much slower.
 */
ಬಾಹ್ಯ "C" ಶೂನ್ಯ vpip_ಎಣಿಕೆ_drivers(vpiHandle ref, unsigned idx,
                                   unsigned counts[4])
{
      ಸಂರಚನಾ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);
      rfp->ಗಂಟು->ಎಣಿಕೆ_drivers(idx, counts);
}

#ಯದಿ defined(__MINGW32__) || defined (__CYGWIN__)
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
    .get_ಮೌಲ್ಯ                  = vpi_get_ಮೌಲ್ಯ,
    .put_ಮೌಲ್ಯ                  = vpi_put_ಮೌಲ್ಯ,
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
    .ಎಣಿಕೆ_drivers              = vpip_ಎಣಿಕೆ_drivers,
    .format_strength            = vpip_format_strength,
    .format_pretty              = vpip_format_pretty,
    .make_systf_system_defined  = vpip_make_systf_system_defined,
    .mcd_rawwrite               = vpip_mcd_rawwrite,
    .set_return_ಮೌಲ್ಯ           = vpip_set_return_ಮೌಲ್ಯ,
};
#endif
