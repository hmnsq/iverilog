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

using ନାମାକାଶ std;
vpi_mode_t vpi_mode_flag = VPI_MODE_NONE;
FILE*vpi_trace = 0;

ସ୍ଥିର s_vpi_vlog_info  vpi_vlog_info;
ସ୍ଥିର s_vpi_error_info vpip_last_error = { 0, 0, 0, 0, 0, 0, 0 };

ଅଚଳ ଅକ୍ଷର* direction_as_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir)
{
      ବାଛ (dir) {
	  ସ୍ଥିତି vpi_ନିବେଶ:
	    ଫେରନ୍ତୁ "input";
	  ସ୍ଥିତି vpiOutput:
	    ଫେରନ୍ତୁ "output";
	  ସ୍ଥିତି vpiInout:
	    ଫେରନ୍ତୁ "inout";
	  ସ୍ଥିତି vpiMixedIO:
	    ଫେରନ୍ତୁ "mixed io";
	  ସ୍ଥିତି vpiNoDirection:
	    ଫେରନ୍ତୁ "no direction";
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ "INVALID-DIRECTION";
      }
}

__vpiHandle::~__vpiHandle()
{ }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiHandle::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{ ଫେରନ୍ତୁ vpiUndefined; }

int64_t __vpiHandle::vpi_get64(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ (code == _vpiNexusId) ? 0 : vpiUndefined; }

ଅକ୍ଷର* __vpiHandle::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{ ଫେରନ୍ତୁ 0; }

ଶୂନ୍ୟ __vpiHandle::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ)
{ }

vpiHandle __vpiHandle::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{ ଫେରନ୍ତୁ 0; }

vpiHandle __vpiHandle::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{ ଫେରନ୍ତୁ 0; }

vpiHandle __vpiHandle::vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{ ଫେରନ୍ତୁ 0; }

vpiHandle __vpiHandle::vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{ ଫେରନ୍ତୁ 0; }

ଶୂନ୍ୟ __vpiHandle::vpi_get_delays(p_vpi_delay)
{ }

ଶୂନ୍ୟ __vpiHandle::vpi_put_delays(p_vpi_delay)
{ }

__vpiBaseVar::__vpiBaseVar(__vpiScope*scope, ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net)
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା suppress_free(vpiHandle)
{ ଫେରନ୍ତୁ 1; }
__vpiHandle::free_object_fun_t __vpiHandle::free_object_fun(ଶୂନ୍ୟ)
{ ଫେରନ୍ତୁ &suppress_free; }

/*
 * The vpip_string function creates a constant string from the pass
 * input. This constant string is permanently allocated from an
 * efficient string buffer store.
 */
ସଂରଚନା vpip_string_chunk {
      ସଂରଚନା vpip_string_chunk*next;
      ଅକ୍ଷର data[64*1024 - ଆକାର (ସଂରଚନା vpip_string_chunk*)];
};

unsigned vpip_size(ଅଚଳ __vpiSignal *sig)
{
      ଫେରନ୍ତୁ abs(sig->msb.get_ମୂଲ୍ୟ() - sig->lsb.get_ମୂଲ୍ୟ()) + 1;
}

__vpiScope* vpip_scope(__vpiSignal*sig)
{
      ଯଦି (sig->is_netarray)
	    ଫେରନ୍ତୁ static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ sig->within.scope;
}

__vpiScope* vpip_scope(__vpiRealVar*sig)
{
      ଯଦି (sig->is_netarray)
	    ଫେରନ୍ତୁ static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ sig->within.scope;
}

vpiHandle vpip_module(__vpiScope*scope)
{
      ଯେତେବେଳେ(scope && scope->get_type_code() != vpiModule) {
	    scope = scope->scope;
      }
      ପ୍ରତିଜ୍ଞା(scope);
      ଫେରନ୍ତୁ scope;
}

ଅଚଳ ଅକ୍ଷର *vpip_string(ଅଚଳ ଅକ୍ଷର*str)
{
      ସ୍ଥିର vpip_string_chunk first_chunk = {0, {0}};
      ସ୍ଥିର vpip_string_chunk*chunk_list = &first_chunk;
      ସ୍ଥିର unsigned chunk_fill = 0;

      unsigned len = strlen(str);
      ପ୍ରତିଜ୍ଞା( (len+1) <= ଆକାର chunk_list->data );

      ଯଦି ( (len+1) > (ଆକାର chunk_list->data - chunk_fill) ) {
	    vpip_string_chunk*tmp = ନୂତନ vpip_string_chunk;
	    tmp->next = chunk_list;
	    chunk_list = tmp;
	    chunk_fill = 0;
      }

      ଅକ୍ଷର*res = chunk_list->data + chunk_fill;
      chunk_fill += len + 1;

      strcpy(res, str);
      ଫେରନ୍ତୁ res;
}

ସ୍ଥିର unsigned hash_string(ଅଚଳ ଅକ୍ଷର*text)
{
      unsigned h = 0;

      ଯେତେବେଳେ (*text) {
	    h = (h << 4) ^ (h >> 28) ^ (unsigned)*text;
	    text += 1;
      }
      ଫେରନ୍ତୁ h;
}

ଅଚଳ ଅକ୍ଷର* vpip_name_string(ଅଚଳ ଅକ୍ଷର*text)
{
      ଅଚଳ unsigned HASH_SIZE = 4096;
      ସ୍ଥିର ଅଚଳ ଅକ୍ଷର*hash_table[HASH_SIZE] = {0};

      unsigned hash_ମୂଲ୍ୟ = hash_string(text) % HASH_SIZE;

	/* If we easily find the string in the hash table, then return
	   that and be done. */
      ଯଦି (hash_table[hash_ମୂଲ୍ୟ]
	  && (strcmp(hash_table[hash_ମୂଲ୍ୟ], text) == 0)) {
	    ଫେରନ୍ତୁ hash_table[hash_ମୂଲ୍ୟ];
      }

	/* The existing hash entry is not a match. Replace it with the
	   newly allocated value, and return the new pointer as the
	   result to the add. */
      ଅଚଳ ଅକ୍ଷର*res = vpip_string(text);
      hash_table[hash_ମୂଲ୍ୟ] = res;

      ଫେରନ୍ତୁ res;
}

PLI_INT32 vpi_chk_error(p_vpi_error_info info)
{
      ଯଦି (vpip_last_error.state == 0)
	    ଫେରନ୍ତୁ 0;

      info->state = vpip_last_error.state;
      info->level = vpip_last_error.level;
      info->message = vpip_last_error.message;
      info->product = vpi_vlog_info.product;
      info->code = (ଅକ୍ଷର *)"";
      info->file = 0;
      info->ଧାଡ଼ି = 0;

      ଫେରନ୍ତୁ info->level;
}

PLI_INT32 vpi_compare_objects(vpiHandle obj1, vpiHandle obj2)
{
      ପ୍ରତିଜ୍ଞା(obj1);
      ପ୍ରତିଜ୍ଞା(obj2);

	// Does this actually work for all cases?
      ଯଦି (obj1 != obj2) ଫେରନ୍ତୁ 0;
      ଅନ୍ୟଥା ଫେରନ୍ତୁ 1;
}

/*
 * Copy the internal information to the data structure. Do not free or
 * change the tfname/user_data since they are a pointer to the real
 * string/data values. We also support passing a task or function handle
 * instead of just a handle to a vpiUserSystf.
 */
ଶୂନ୍ୟ vpi_get_systf_info(vpiHandle ref, p_vpi_systf_data data)
{
      ସଂରଚନା __vpiUserSystf* rfp = dynamic_cast<__vpiUserSystf*>(ref);
      ଯଦି (rfp == 0) {
	    ସଂରଚନା __vpiSysTaskCall*call = dynamic_cast<__vpiSysTaskCall*>(ref);
	    ପ୍ରତିଜ୍ଞା(call);
	    rfp = call->defn;
      }

	/* Assert that vpiUserDefn is true! */
      ପ୍ରତିଜ୍ଞା(rfp->is_user_defn);

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
ସଂରଚନା __vpiSysTaskCall*vpip_cur_task = 0;

PLI_INT32 vpi_free_object(vpiHandle ref)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rtn;

      ଯଦି (vpi_trace) {
	    fprintf(vpi_trace, "vpi_free_object(%p)", ref);
	    fflush(vpi_trace);
      }

      ପ୍ରତିଜ୍ଞା(ref);
      __vpiHandle::free_object_fun_t fun = ref->free_object_fun();
      rtn = fun (ref);

      ଯଦି (vpi_trace)
	    fprintf(vpi_trace, " --> %d\n", rtn);

      ଫେରନ୍ତୁ rtn;
}

PLI_INT32 vpi_release_handle(vpiHandle ref)
{
	// Since SystemVerilog vpi_free_object() has been
	// renamed vpi_release_handle(), and thus
	// vpi_free_object() has been deprecated.
      ଫେରନ୍ତୁ vpi_free_object(ref);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpip_get_global(ପୂର୍ଣ୍ଣସଂଖ୍ୟା property)
{
      ବାଛ (property) {

	  ସ୍ଥିତି vpiTimeUnit:
	  ସ୍ଥିତି vpiTimePrecision:
	    ଫେରନ୍ତୁ vpip_get_time_precision();

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vpi error: bad global property: %d\n", property);
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ vpiUndefined;
      }
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* vpi_property_str(PLI_INT32 code)
{
      ସ୍ଥିର ଅକ୍ଷର buf[32];
      ବାଛ (code) {
	  ସ୍ଥିତି vpiConstType:
	    ଫେରନ୍ତୁ "vpiConstType";
	  ସ୍ଥିତି vpiName:
	    ଫେରନ୍ତୁ "vpiName";
	  ସ୍ଥିତି vpiFullName:
	    ଫେରନ୍ତୁ "vpiFullName";
	  ସ୍ଥିତି vpiTimeUnit:
	    ଫେରନ୍ତୁ "vpiTimeUnit";
	  ସ୍ଥିତି vpiTimePrecision:
	    ଫେରନ୍ତୁ "vpiTimePrecision";
          ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ "vpiSize";
          ସ୍ଥିତି _vpiNexusId:
	    ଫେରନ୍ତୁ "_vpiNexusId";
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    snprintf(buf, ଆକାର(buf), "%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)code);
      }
      ଫେରନ୍ତୁ buf;
}

ଅଚଳ ଅକ୍ଷର* vpi_type_as_string(PLI_INT32 code)
{
      ସ୍ଥିର ଅକ୍ଷର buf[32];
      ବାଛ (code) {
	  ସ୍ଥିତି vpiArrayType:
	    ଫେରନ୍ତୁ "vpiArrayType";
	  ସ୍ଥିତି vpiBitVar:
	    ଫେରନ୍ତୁ "vpiBitVar";
	  ସ୍ଥିତି vpiByteVar:
	    ଫେରନ୍ତୁ "vpiByteVar";
	  ସ୍ଥିତି vpiClassVar:
	    ଫେରନ୍ତୁ "vpiClassVar";
	  ସ୍ଥିତି vpiConstant:
	    ଫେରନ୍ତୁ "vpiConstant";
	  ସ୍ଥିତି vpiEnumTypespec:
	    ଫେରନ୍ତୁ "vpiEnumTypespec";
	  ସ୍ଥିତି vpiFunction:
	    ଫେରନ୍ତୁ "vpiFunction";
	  ସ୍ଥିତି vpiGenScope:
	    ଫେରନ୍ତୁ "vpiGenScope";
	  ସ୍ଥିତି vpiIntVar:
	    ଫେରନ୍ତୁ "vpiIntVar";
	  ସ୍ଥିତି vpiIntegerVar:
	    ଫେରନ୍ତୁ "vpiIntegerVar";
	  ସ୍ଥିତି vpiIterator:
	    ଫେରନ୍ତୁ "vpiIterator";
	  ସ୍ଥିତି vpiLongIntVar:
	    ଫେରନ୍ତୁ "vpiLongIntVar";
	  ସ୍ଥିତି vpiMemory:
	    ଫେରନ୍ତୁ "vpiMemory";
	  ସ୍ଥିତି vpiMemoryWord:
	    ଫେରନ୍ତୁ "vpiMemoryWord";
	  ସ୍ଥିତି vpiModule:
	    ଫେରନ୍ତୁ "vpiModule";
	  ସ୍ଥିତି vpiNamedBegin:
	    ଫେରନ୍ତୁ "vpiNamedBegin";
	  ସ୍ଥିତି vpiNamedEvent:
	    ଫେରନ୍ତୁ "vpiNamedEvent";
	  ସ୍ଥିତି vpiNamedFork:
	    ଫେରନ୍ତୁ "vpiNamedFork";
	  ସ୍ଥିତି vpiPackage:
	    ଫେରନ୍ତୁ "vpiPackage";
	  ସ୍ଥିତି vpiPathTerm:
	    ଫେରନ୍ତୁ "vpiPathTerm";
	  ସ୍ଥିତି vpiPort:
	    ଫେରନ୍ତୁ "vpiPort";
	  ସ୍ଥିତି vpiNet:
	    ଫେରନ୍ତୁ "vpiNet";
	  ସ୍ଥିତି vpiNetArray:
	    ଫେରନ୍ତୁ "vpiNetArray";
	  ସ୍ଥିତି vpiNetBit:
	    ଫେରନ୍ତୁ "vpiNetBit";
	  ସ୍ଥିତି vpiParameter:
	    ଫେରନ୍ତୁ "vpiParameter";
	  ସ୍ଥିତି vpiPartSelect:
	    ଫେରନ୍ତୁ "vpiPartSelect";
	  ସ୍ଥିତି vpiRealVar:
	    ଫେରନ୍ତୁ "vpiRealVar";
	  ସ୍ଥିତି vpiReg:
	    ଫେରନ୍ତୁ "vpiReg";
	  ସ୍ଥିତି vpiRegBit:
	    ଫେରନ୍ତୁ "vpiRegBit";
	  ସ୍ଥିତି vpiShortIntVar:
	    ଫେରନ୍ତୁ "vpiShortIntVar";
	  ସ୍ଥିତି vpiStringVar:
	    ଫେରନ୍ତୁ "vpiStringVar";
	  ସ୍ଥିତି vpiSysFuncCall:
	    ଫେରନ୍ତୁ "vpiSysFuncCall";
	  ସ୍ଥିତି vpiSysTaskCall:
	    ଫେରନ୍ତୁ "vpiSysTaskCall";
	  ସ୍ଥିତି vpiTask:
	    ଫେରନ୍ତୁ "vpiTask";
	  ସ୍ଥିତି vpiTimeVar:
	    ଫେରନ୍ତୁ "vpiTimeVar";
	  ସ୍ଥିତି vpiUserSystf:
	    ଫେରନ୍ତୁ "vpiUserSystf";
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    snprintf(buf, ଆକାର(buf), "%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)code);
      }
      ଫେରନ୍ତୁ buf;
}

PLI_INT32 vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      ଯଦି (property == _vpiDelaySelection) ଫେରନ୍ତୁ vpip_delay_selection;

      ଯଦି (ref == 0)
	    ଫେରନ୍ତୁ vpip_get_global(property);

      ଯଦି (property == vpiType) {
	    ଯଦି (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

	    ଯଦି (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  ଫେରନ୍ତୁ vpiNetArray;
	    ଅନ୍ୟଥା
		  ଫେରନ୍ତୁ ref->get_type_code();
      }

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = ref->vpi_get(property);

      ଯଦି (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get(%s, %p) --> %d\n",
		    vpi_property_str(property), ref, res);
      }

      ଫେରନ୍ତୁ res;
}

PLI_INT64 vpi_get64(ପୂର୍ଣ୍ଣସଂଖ୍ୟା property, vpiHandle ref)
{
	// For now we only support getting the nexus id.
      ଯଦି ((ref == 0) || (property != _vpiNexusId)) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) is not currently supported.\n",
		    vpi_property_str(property), ref);
	    ଫେରନ୍ତୁ vpiUndefined;
      }

      int64_t res = ref->vpi_get64(property);

      ଯଦି (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) --> %" PRId64 "\n",
		    vpi_property_str(property), ref, res);
      }

      ଫେରନ୍ତୁ res;
}

ଅକ୍ଷର* vpi_get_str(PLI_INT32 property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      ଯଦି (property == _vpiDelaySelection) {
	    ବାଛ (vpip_delay_selection) {
		ସ୍ଥିତି _vpiDelaySelMinimum:
		  ଫେରନ୍ତୁ simple_set_rbuf_str("MINIMUM");
		ସ୍ଥିତି _vpiDelaySelTypical:
		  ଫେରନ୍ତୁ simple_set_rbuf_str("TYPICAL");
		ସ୍ଥିତି _vpiDelaySelMaximum:
		  ଫେରନ୍ତୁ simple_set_rbuf_str("MAXIMUM");
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  ପ୍ରତିଜ୍ଞା(0);
	    }
      }

      ଯଦି (ref == 0) {
	    fprintf(stderr, "vpi error: vpi_get_str(%s, 0) called "
		    "with null vpiHandle.\n", vpi_property_str(property));
	    ଫେରନ୍ତୁ 0;
      }

      ଯଦି (property == vpiType) {
	    ଯଦି (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

            PLI_INT32 type;
	    ଯଦି (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  type = vpiNetArray;
	    ଅନ୍ୟଥା
		  type = ref->get_type_code();
	    ଫେରନ୍ତୁ (ଅକ୍ଷର *)vpi_type_as_string(type);
      }

      ଅକ୍ଷର*res = ref->vpi_get_str(property);

      ଯଦି (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get_str(%s, %p) --> %s\n",
		    vpi_property_str(property), ref, res? res : "<NULL>");
      }

      ଫେରନ୍ତୁ res;
}

__vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj)
{
      ଅଚଳ ସଂରଚନା __vpiSysTaskCall*task;
      ସଂରଚନା __vpiSignal*ସଙ୍କେତ;
      ସଂରଚନା __vpiRealVar*real;
      ଅଚଳ __vpiNamedEvent*event;

      ବାଛ (obj->get_type_code()) {
	  ସ୍ଥିତି vpiSysTaskCall:
	    task = dynamic_cast<__vpiSysTaskCall*>(obj);
	    ଫେରନ୍ତୁ task->scope;

	  ସ୍ଥିତି vpiModule:
	    ଫେରନ୍ତୁ dynamic_cast<__vpiScope*>(obj);

	  ସ୍ଥିତି vpiNet:
	  ସ୍ଥିତି vpiReg:
	  ସ୍ଥିତି vpiIntegerVar:
	  ସ୍ଥିତି vpiBitVar:
	  ସ୍ଥିତି vpiByteVar:
	  ସ୍ଥିତି vpiShortIntVar:
	  ସ୍ଥିତି vpiIntVar:
	  ସ୍ଥିତି vpiLongIntVar:
	    ସଙ୍କେତ = dynamic_cast<__vpiSignal*>(obj);
	    ଫେରନ୍ତୁ vpip_scope(ସଙ୍କେତ);

	  ସ୍ଥିତି vpiRealVar:
	    real = dynamic_cast<__vpiRealVar*>(obj);
	    ଫେରନ୍ତୁ vpip_scope(real);

	  ସ୍ଥିତି vpiNamedEvent:
	    event = dynamic_cast<__vpiNamedEvent*>(obj);
	    ଫେରନ୍ତୁ event->get_scope();

	  ସ୍ଥିତି vpiMemory:
	  ସ୍ଥିତି vpiMemoryWord:
	  ସ୍ଥିତି vpiPartSelect:
	    ଫେରନ୍ତୁ dynamic_cast<__vpiScope*>(obj->vpi_handle(vpiScope));

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "ERROR: vpip_scope_from_handle called with "
		    "object handle type=%d\n", obj->get_type_code());
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ 0;
      }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpip_time_units_from_handle(vpiHandle obj)
{
      ଯଦି (obj == 0)
	    ଫେରନ୍ତୁ vpip_get_time_precision();

      ଅଚଳ __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      ଯଦି (scope == 0)
	    ଫେରନ୍ତୁ vpip_get_time_precision();

      ଫେରନ୍ତୁ scope->time_units;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpip_time_precision_from_handle(vpiHandle obj)
{
      ଯଦି (obj == 0)
	    ଫେରନ୍ତୁ vpip_get_time_precision();

      ଅଚଳ __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      ଯଦି (scope == 0)
	    ଫେରନ୍ତୁ vpip_get_time_precision();

      ଫେରନ୍ତୁ scope->time_precision;
}

ଦ୍ୱିଗୁଣ vpip_scaled_time_from_handle(vvp_time64_t time, vpiHandle obj)
{
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା scale = vpip_get_time_precision() -
	          vpip_time_units_from_handle(obj);
      ଯଦି (scale >= 0)
	    ଫେରନ୍ତୁ (ଦ୍ୱିଗୁଣ)time * pow(10.0, scale);
      ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ (ଦ୍ୱିଗୁଣ)time / pow(10.0, -scale);
}

ଶୂନ୍ୟ vpi_get_time(vpiHandle obj, s_vpi_time*vp)
{
      vvp_time64_t time;

      ପ୍ରତିଜ୍ଞା(vp);

      time = schedule_simtime();

      ବାଛ (vp->type) {
          ସ୍ଥିତି vpiSimTime:
	    vp->ଉପର = (time >> 32) & 0xffffffff;
	    vp->ତଳ = time & 0xffffffff;
	    ଭାଙ୍ଗ;

          ସ୍ଥିତି vpiScaledRealTime:
	    vp->real = vpip_scaled_time_from_handle(time, obj);
	    ଭାଙ୍ଗ;

          ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            fprintf(stderr, "vpi_get_time: unknown type: %d\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->type);
            ପ୍ରତିଜ୍ଞା(0);
	    ଭାଙ୍ଗ;
      }
}

PLI_INT32 vpi_get_vlog_info(p_vpi_vlog_info vlog_info_p)
{
    ଯଦି (vlog_info_p != 0) {
	  *vlog_info_p = vpi_vlog_info;
	  ଫେରନ୍ତୁ 1;
    } ଅନ୍ୟଥା {
	  ଫେରନ୍ତୁ 0;
    }
}

ଶୂନ୍ୟ vpi_set_vlog_info(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର** argv)
{
    ସ୍ଥିର ଅକ୍ଷର icarus_product[] = "Icarus Verilog";
    ସ୍ଥିର ଅକ୍ଷର icarus_version[] = VERSION;
    vpi_vlog_info.product = icarus_product;
    vpi_vlog_info.version = icarus_version;
    vpi_vlog_info.argc    = argc;
    vpi_vlog_info.argv    = argv;

    ସ୍ଥିର ଅକ୍ଷର trace_buf[1024];
    ଯଦି (ଅଚଳ ଅକ୍ଷର*path = getenv("VPI_TRACE"))  {
	  ଯଦି (!strcmp(path,"-"))
		vpi_trace = stdout;
	  ଅନ୍ୟଥା {
		vpi_trace = fopen(path, "w");
		ଯଦି (!vpi_trace) {
		      perror(path);
		      exit(1);
		}
		setvbuf(vpi_trace, trace_buf, _IOLBF, ଆକାର(trace_buf));
	  }
    }
}

ସ୍ଥିର ଶୂନ୍ୟ vec4_get_ମୂଲ୍ୟ_string(ଅଚଳ vvp_vector4_t&word_val, unsigned width,
				  s_vpi_ମୂଲ୍ୟ*vp)
{
      unsigned nchar = width / 8;
      unsigned tail = width % 8;

      ଅକ୍ଷର*rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(nchar + 1, RBUF_VAL));
      ଅକ୍ଷର*cp = rbuf;

      ଯଦି (tail > 0) {
	    ଅକ୍ଷର char_val = 0;
	    ପାଇଁ (unsigned idx = width-tail; idx < width ;  idx += 1) {
		  vvp_bit4_t val = word_val.value(idx);
		  ଯଦି (val == BIT4_1)
			char_val |= 1 << idx;
	    }

	    ଯଦି (char_val != 0)
		  *cp++ = char_val;
	    ଅନ୍ୟଥା ଯଦି (cp != rbuf)
		  *cp++ = ' ';
      }

      ପାଇଁ (unsigned idx = 0 ;  idx < nchar ;  idx += 1) {
	    unsigned bit = (nchar - idx - 1) * 8;
	    ଅକ୍ଷର char_val = 0;
	    ପାଇଁ (unsigned bdx = 0 ;  bdx < 8 ;  bdx += 1) {
		  vvp_bit4_t val = word_val.value(bit+bdx);
		  ଯଦି (val == BIT4_1)
			char_val |= 1 << bdx;
	    }
	    // Ignore leading null-bytes and replace other null-bytes with space.
	    // The LRM is not entirely clear on how null bytes should be handled.
	    // This is the implementation chosen for iverilog.
	    ଯଦି (char_val != 0)
		  *cp++ = char_val;
	    ଅନ୍ୟଥା ଯଦି (cp != rbuf)
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
ଶୂନ୍ୟ vpip_vec4_get_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&word_val, unsigned width,
			 ତାର୍କିକ signed_flag, s_vpi_ମୂଲ୍ୟ*vp)
{
      ଅକ୍ଷର *rbuf = 0;

      ବାଛ (vp->format) {
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector values.\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
	    ପ୍ରତିଜ୍ଞା(0);

	  ସ୍ଥିତି vpiSuppressVal:
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiBinStrVal:
	    rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(width+1, RBUF_VAL));
	    ପାଇଁ (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		  vvp_bit4_t bit = word_val.value(idx);
		  rbuf[width-idx-1] = vvp_bit4_to_ascii(bit);
	    }
	    rbuf[width] = 0;
	    vp->value.str = rbuf;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiOctStrVal: {
		unsigned hwid = ((width+2) / 3) + 1;
		rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_oct_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		ଭାଙ୍ଗ;
	  }

	  ସ୍ଥିତି vpiDecStrVal: {
// HERE need a better estimate.
		rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(width+1, RBUF_VAL));
		vpip_vec4_to_dec_str(word_val, rbuf, width+1, signed_flag);
		vp->value.str = rbuf;
		ଭାଙ୍ଗ;
	  }

	  ସ୍ଥିତି vpiHexStrVal: {
		unsigned  hwid = ((width + 3) / 4) + 1;
		rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_hex_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		ଭାଙ୍ଗ;
	  }

          ସ୍ଥିତି vpiScalarVal: {
	        // scalars should be of size 1
	        ପ୍ରତିଜ୍ଞା(width == 1);
	        ବାଛ(word_val.value(0)) {
		    ସ୍ଥିତି BIT4_0:
		      vp->value.scalar = vpi0;
		      ଭାଙ୍ଗ;
	            ସ୍ଥିତି BIT4_1:
                      vp->value.scalar = vpi1;
                      ଭାଙ୍ଗ;
	            ସ୍ଥିତି BIT4_X:
                      vp->value.scalar = vpiX;
                      ଭାଙ୍ଗ;
	            ସ୍ଥିତି BIT4_Z:
                      vp->value.scalar = vpiZ;
                      ଭାଙ୍ଗ;
		}
                ଭାଙ୍ଗ;
          }

	  ସ୍ଥିତି vpiIntVal: {
		long val = 0;
		vvp_bit4_t pad = BIT4_0;
		ଯଦି (signed_flag && word_val.size() > 0)
		      pad = word_val.value(word_val.size()-1);

		ପାଇଁ (unsigned idx = 0 ; idx < 8*ଆକାର(val) ;  idx += 1) {
		      vvp_bit4_t val4 = pad;
		      ଯଦି (idx < word_val.size())
			    val4 = word_val.value(idx);
		      ଯଦି (val4 == BIT4_1)
			    val |= 1L << idx;
		}

		vp->value.integer = val;
		ଭାଙ୍ଗ;
	  }

	  ସ୍ଥିତି vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiVectorVal;
	    // fallthrough
	  ସ୍ଥିତି vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * ଆକାର(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		ପାଇଁ (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      ବାଛ (word_val.value(idx)) {
			  ସ୍ଥିତି BIT4_0:
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    ଭାଙ୍ଗ;
			  ସ୍ଥିତି BIT4_1:
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    ଭାଙ୍ଗ;
			  ସ୍ଥିତି BIT4_X:
			    op->aval |= (1 << idx % 32);
			    op->bval |= (1 << idx % 32);
			    ଭାଙ୍ଗ;
			  ସ୍ଥିତି BIT4_Z:
			    op->aval &= ~(1 << idx % 32);
			    op->bval |=  (1 << idx % 32);
			    ଭାଙ୍ଗ;
		      }
		      ଯଦି (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		ଭାଙ୍ଗ;
	  }

	  ସ୍ଥିତି vpiStringVal:
		vec4_get_ମୂଲ୍ୟ_string(word_val, width, vp);
		ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiRealVal:
		vector4_to_ମୂଲ୍ୟ(word_val, vp->value.real, signed_flag);
		ଭାଙ୍ଗ;
      }
}

ଶୂନ୍ୟ vpip_vec2_get_ମୂଲ୍ୟ(ଅଚଳ vvp_vector2_t&word_val, unsigned width,
			 ତାର୍କିକ signed_flag, s_vpi_ମୂଲ୍ୟ*vp)
{
      ବାଛ (vp->format) {
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector2 values.\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
	    ପ୍ରତିଜ୍ଞା(0);

	  ସ୍ଥିତି vpiSuppressVal:
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiIntVal:
	    vector2_to_ମୂଲ୍ୟ(word_val, vp->value.integer, signed_flag);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  ସ୍ଥିତି vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * ଆକାର(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		ପାଇଁ (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      ଯଦି (word_val.value(idx)) {
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      } ଅନ୍ୟଥା {
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      }
		      ଯଦି (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		ଭାଙ୍ଗ;
	  }
      }

}

/*
 * Convert a real value to the appropriate integer.
 */
ସ୍ଥିର PLI_INT32 get_real_as_int(ଦ୍ୱିଗୁଣ real)
{
      ଦ୍ୱିଗୁଣ rtn;

	/* We would normally want to return 'bx for a NaN or
	 * +/- infinity, but for an integer the standard says
	 * to convert 'bx to 0, so we just return 0. */
      ଯଦି (real != real || (real && (real == 0.5*real))) {
	    ଫେରନ୍ତୁ 0;
      }

      rtn = std::round(real);

      ଫେରନ୍ତୁ (PLI_INT32) rtn;
}

/*
 * This is a generic function to convert a double value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer.
 */
ଶୂନ୍ୟ vpip_real_get_ମୂଲ୍ୟ(ଦ୍ୱିଗୁଣ real, s_vpi_ମୂଲ୍ୟ*vp)
{
      ଅକ୍ଷର *rbuf = 0;

      ବାଛ (vp->format) {
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting real values.\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
	    ପ୍ରତିଜ୍ଞା(0);

	  ସ୍ଥିତି vpiSuppressVal:
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiRealVal;
	    // fallthrough
	  ସ୍ଥିତି vpiRealVal:
	    vp->value.real = real;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiIntVal:
	    vp->value.integer = get_real_as_int(real);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiDecStrVal:
	    rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(1025, RBUF_VAL));
	    vpip_vec4_to_dec_str(vvp_vector4_t(1024, real), rbuf, 1025, ସତ୍ୟ);
	    vp->value.str = rbuf;
	    ଭାଙ୍ଗ;
      }
}

ଦ୍ୱିଗୁଣ real_from_vpi_ମୂଲ୍ୟ(s_vpi_ମୂଲ୍ୟ*vp)
{
      vvp_vector4_t vec4(1024);
      ଦ୍ୱିଗୁଣ result;
      ତାର୍କିକ is_signed = ମିଥ୍ୟା;

      ବାଛ (vp->format) {
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "putting real values.\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
	    ପ୍ରତିଜ୍ଞା(0);

	  ସ୍ଥିତି vpiRealVal:
	    result = vp->value.real;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiIntVal:
	    result = (ଦ୍ୱିଗୁଣ) vp->value.integer;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiBinStrVal:
	    vpip_bin_str_to_vec4(vec4, vp->value.str);
	    ଯଦି (vp->value.str[0] == '-') is_signed = ସତ୍ୟ;
	    vector4_to_ମୂଲ୍ୟ(vec4, result, is_signed);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiOctStrVal:
	    vpip_oct_str_to_vec4(vec4, vp->value.str);
	    ଯଦି (vp->value.str[0] == '-') is_signed = ସତ୍ୟ;
	    vector4_to_ମୂଲ୍ୟ(vec4, result, is_signed);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiDecStrVal:
	    vpip_dec_str_to_vec4(vec4, vp->value.str);
	    ଯଦି (vp->value.str[0] == '-') is_signed = ସତ୍ୟ;
	    vector4_to_ମୂଲ୍ୟ(vec4, result, is_signed);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiHexStrVal:
	    vpip_hex_str_to_vec4(vec4, vp->value.str);
	    ଯଦି (vp->value.str[0] == '-') is_signed = ସତ୍ୟ;
	    vector4_to_ମୂଲ୍ୟ(vec4, result, is_signed);
	    ଭାଙ୍ଗ;

      }

      ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ vpip_string_get_ମୂଲ୍ୟ(ଅଚଳ string&val, s_vpi_ମୂଲ୍ୟ*vp)
{
      ଅକ୍ଷର *rbuf = 0;

      ବାଛ (vp->format) {
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting string values.\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
	    ପ୍ରତିଜ୍ଞା(0);

	  ସ୍ଥିତି vpiSuppressVal:
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiStringVal;
	    // fallthrough
	  ସ୍ଥିତି vpiStringVal:
	    rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(val.size() + 1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    ଭାଙ୍ଗ;
      }
}


ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(vpiHandle ପଦସମଷ୍ଟି, s_vpi_ମୂଲ୍ୟ*vp)
{
      ପ୍ରତିଜ୍ଞା(ପଦସମଷ୍ଟି);
      ପ୍ରତିଜ୍ଞା(vp);

	// Never bother with suppressed values. All the derived
	// classes can ignore this type.
      ଯଦି (vp->format == vpiSuppressVal)
	    ଫେରନ୍ତୁ;

      ପଦସମଷ୍ଟି->vpi_get_ମୂଲ୍ୟ(vp);

      ଯଦି (vpi_trace) ବାଛ (vp->format) {
		ସ୍ଥିତି vpiStringVal:
		  fprintf(vpi_trace,"vpi_get_value(%p=<%d>) -> string=\"%s\"\n",
			  ପଦସମଷ୍ଟି, ପଦସମଷ୍ଟି->get_type_code(), vp->value.str);
		  ଭାଙ୍ଗ;

		ସ୍ଥିତି vpiBinStrVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> binstr=%s\n",
			  ପଦସମଷ୍ଟି->get_type_code(), vp->value.str);
		  ଭାଙ୍ଗ;

		ସ୍ଥିତି vpiIntVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> int=%d\n",
			  ପଦସମଷ୍ଟି->get_type_code(), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->value.integer);
		  ଭାଙ୍ଗ;

		ସ୍ଥିତି vpiSuppressVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <suppress>\n",
			  ପଦସମଷ୍ଟି->get_type_code());
		  ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <%d>=?\n",
			  ପଦସମଷ୍ଟି->get_type_code(), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vp->format);
	    }
}

ସଂରଚନା vpip_put_ମୂଲ୍ୟ_event : vvp_gen_event_s {
      vpiHandle handle;
      s_vpi_ମୂଲ୍ୟ value;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags;
      ଆଭାସୀ ଶୂନ୍ୟ run_run() override;
      ~vpip_put_ମୂଲ୍ୟ_event() override { }
};

ଶୂନ୍ୟ vpip_put_ମୂଲ୍ୟ_event::run_run()
{
      handle->vpi_put_ମୂଲ୍ୟ(&value, flags);
      ବାଛ (value.format) {
	    /* Free the copied string. */
	  ସ୍ଥିତି vpiBinStrVal:
	  ସ୍ଥିତି vpiOctStrVal:
	  ସ୍ଥିତି vpiDecStrVal:
	  ସ୍ଥିତି vpiHexStrVal:
	  ସ୍ଥିତି vpiStringVal:
	    ମୁକ୍ତ(value.value.str);
	    ଭାଙ୍ଗ;
	    /* Free the copied time structure. */
	  ସ୍ଥିତି vpiTimeVal:
	    ମୁକ୍ତ(value.value.time);
	    ଭାଙ୍ଗ;
	    /* Free the copied vector structure. */
	  ସ୍ଥିତି vpiVectorVal:
	    ମୁକ୍ତ(value.value.vector);
	    ଭାଙ୍ଗ;
	    /* Free the copied strength structure. */
	  ସ୍ଥିତି vpiStrengthVal:
	    ମୁକ୍ତ(value.value.strength);
	    ଭାଙ୍ଗ;
	    /* Everything else is static in the structure. */
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
      }
}

/* Make a copy of a pointer to a time structure. */
ସ୍ଥିର t_vpi_time *timedup(ଅଚଳ t_vpi_time *val)
{
      t_vpi_time *rtn;
      rtn = static_cast<t_vpi_time *> (malloc(ଆକାର(t_vpi_time)));
      *rtn = *val;
      ଫେରନ୍ତୁ rtn;
}

/* Make a copy of a pointer to a vector value structure. */
ସ୍ଥିର t_vpi_vecval *vectordup(ଅଚଳ t_vpi_vecval *val, PLI_INT32 size)
{
      unsigned ଅଙ୍କ_bytes;
      t_vpi_vecval *rtn;
      ପ୍ରତିଜ୍ଞା(size > 0);
      ଅଙ୍କ_bytes = ((size + 31)/32)*ଆକାର(t_vpi_vecval);
      rtn = static_cast<t_vpi_vecval *> (malloc(ଅଙ୍କ_bytes));
      memcpy(rtn, val, ଅଙ୍କ_bytes);
      ଫେରନ୍ତୁ rtn;
}

/* Make a copy of a pointer to a strength structure. */
ସ୍ଥିର t_vpi_strengthval *strengthdup(ଅଚଳ t_vpi_strengthval *val)
{
      t_vpi_strengthval *rtn;
      rtn = static_cast<t_vpi_strengthval *>
            (malloc(ଆକାର(t_vpi_strengthval)));
      *rtn = *val;
      ଫେରନ୍ତୁ rtn;
}

vpiHandle vpi_put_ମୂଲ୍ୟ(vpiHandle obj, s_vpi_ମୂଲ୍ୟ*vp,
			s_vpi_time*when, PLI_INT32 flags)
{
      ପ୍ରତିଜ୍ଞା(obj);

      flags &= ~vpiReturnEvent;

      ଯଦି (flags!=vpiNoDelay && flags!=vpiForceFlag && flags!=vpiReleaseFlag) {
	    vvp_time64_t dly;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା scale;

	    ଯଦି (vpi_get(vpiAutomatic, obj)) {
		  fprintf(stderr, "VPI error: cannot put a value with "
				  "a delay on automatically allocated "
				  "variable '%s'.\n",
				  vpi_get_str(vpiName, obj));
		  ଫେରନ୍ତୁ 0;
	    }

	    ପ୍ରତିଜ୍ଞା(when != 0);

	    ବାଛ (when->type) {
		ସ୍ଥିତି vpiScaledRealTime:
		  scale = vpip_time_units_from_handle(obj) -
		          vpip_get_time_precision();
		  ଯଦି (scale >= 0) {
			dly = (vvp_time64_t)(when->real * pow(10.0, scale));
		  } ଅନ୍ୟଥା {
			dly = (vvp_time64_t)(when->real / pow(10.0, -scale));
		  }
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiSimTime:
		  dly = vpip_timestruct_to_time(when);
		  ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  dly = 0;
		  ଭାଙ୍ଗ;
	    }

	    ଯଦି ((dly == 0) && schedule_at_rosync()) {
		  fprintf(stderr, "VPI error: attempted to put a value to "
				  "variable '%s' during a read-only synch "
				  "callback.\n", vpi_get_str(vpiName, obj));
		  ଫେରନ୍ତୁ 0;
	    }

	    vpip_put_ମୂଲ୍ୟ_event*put = ନୂତନ vpip_put_ମୂଲ୍ୟ_event;
	    put->handle = obj;
	    ଯଦି (dynamic_cast<__vpiNamedEvent*>(obj)) {
		  put->value.format = vpiIntVal;
		  put->value.value.integer = 0;
	    } ଅନ୍ୟଥା {
		  ପ୍ରତିଜ୍ଞା(vp);
		  put->value = *vp;
	    }
	      /* Since this is a scheduled put event we must copy any pointer
	       * data to keep it available until the event is actually run. */
	    ବାଛ (put->value.format) {
		  /* Copy the string items. */
		ସ୍ଥିତି vpiBinStrVal:
		ସ୍ଥିତି vpiOctStrVal:
		ସ୍ଥିତି vpiDecStrVal:
		ସ୍ଥିତି vpiHexStrVal:
		ସ୍ଥିତି vpiStringVal:
		  put->value.value.str = strdup(put->value.value.str);
		  ଭାଙ୍ଗ;
		  /* Copy a time pointer item. */
		ସ୍ଥିତି vpiTimeVal:
		  put->value.value.time = timedup(put->value.value.time);
		  ଭାଙ୍ଗ;
		  /* Copy a vector pointer item. */
		ସ୍ଥିତି vpiVectorVal:
		  put->value.value.vector = vectordup(put->value.value.vector,
		                                      vpi_get(vpiSize, obj));
		  ଭାଙ୍ଗ;
		  /* Copy a strength pointer item. */
		ସ୍ଥିତି vpiStrengthVal:
		  put->value.value.strength =
		      strengthdup(put->value.value.strength);
		  ଭାଙ୍ଗ;
		  /* Everything thing else is already in the structure. */
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  ଭାଙ୍ଗ;
	    }
	    put->flags = flags;
	    schedule_generic(put, dly, ମିଥ୍ୟା, ସତ୍ୟ, ସତ୍ୟ);
	    ଫେରନ୍ତୁ 0;
      }

      ଯଦି (schedule_at_rosync()) {
            fprintf(stderr, "VPI error: attempted to put a value to "
			    "variable '%s' during a read-only synch "
			    "callback.\n", vpi_get_str(vpiName, obj));
            ଫେରନ୍ତୁ 0;
      }

      obj->vpi_put_ମୂଲ୍ୟ(vp, flags);

      ଫେରନ୍ତୁ 0;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle res = 0;

      ଯଦି (ref == 0) {
	      // A few types can apply to a nil handle. These are ways
	      // that the current function can get started finding things.
	    ବାଛ (type) {

		ସ୍ଥିତି vpiScope:
		    // The IEEE1364-2005 doesn't seem to allow this,
		    // but some users seem to think it's handy, so
		    // return the scope that contains this SysTfCall.
		  ପ୍ରତିଜ୍ଞା(vpip_cur_task);
		  res = vpip_cur_task->vpi_handle(vpiScope);
		  ଭାଙ୍ଗ;

		ସ୍ଥିତି vpiSysTfCall:
		    // This is how VPI users get a first handle into
		    // the system. This is the handle of the system
		    // task/function call currently being executed.
		  ଯଦି (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle(vpiSysTfCall, 0) "
				"-> %p (%s)\n", vpip_cur_task,
				vpip_cur_task->defn->info.tfname);
		  }
		  ଫେରନ୍ତୁ vpip_cur_task;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  fprintf(stderr, "VPI error: vpi_handle(type=%d, ref=0).\n",
			  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)type);
		  res = 0;
		  ଭାଙ୍ଗ;
	    }

      } ଅନ୍ୟଥା {

	    ଯଦି (type == vpiSysTfCall) {
		  fprintf(stderr, "VPI error: vpi_handle(vpiSysTfCall, "
			  "ref!=0).\n");
		  ଫେରନ୍ତୁ 0;
	    }

	    res = ref->vpi_handle(type);
      }


      ଯଦି (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle(vpiScope, ref=%p) "
		    "-> %p\n", vpip_cur_task, ref);
      }

      ଫେରନ୍ତୁ res;
}

ସ୍ଥିର vpiHandle vpip_make_udp_iterator()
{
// HERE: Add support for iterating over UDP definitions.
//       See 26.6.16 (page 400 in 1364-2005).
      ଫେରନ୍ତୁ 0;
}

/*
 * This function asks the object to return an iterator for
 * the specified reference. It is up to the iterate_ method to
 * allocate a properly formed iterator.
 */
ସ୍ଥିର vpiHandle vpi_iterate_global(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
      ବାଛ (type) {
	  ସ୍ଥିତି vpiInstance:
	    // fallthrough
	  ସ୍ଥିତି vpiModule:
	    // fallthrough
	  ସ୍ଥିତି vpiProgram:
	    // fallthrough
	  ସ୍ଥିତି vpiInterface:
	    // fallthrough
	  ସ୍ଥିତି vpiPackage:
	    ଫେରନ୍ତୁ vpip_make_ମୂଳ_iterator(type);

	  ସ୍ଥିତି vpiUdpDefn:
	    ଫେରନ୍ତୁ vpip_make_udp_iterator();

	  ସ୍ଥିତି vpiUserSystf:
	    ଫେରନ୍ତୁ vpip_make_systf_iterator();
      }

      ଫେରନ୍ତୁ 0;
}

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle rtn = 0;

      ପ୍ରତିଜ୍ଞା(vpi_mode_flag != VPI_MODE_NONE);
      ଯଦି (vpi_mode_flag == VPI_MODE_REGISTER) {
	    fprintf(stderr, "vpi error: vpi_iterate called during "
		    "vpi_register_systf. You can't do that!\n");
	    ଫେରନ୍ତୁ 0;
      }

      ଯଦି (ref == 0)
	    rtn = vpi_iterate_global(type);
      ଅନ୍ୟଥା
	    rtn = ref->vpi_iterate(type);

      ଯଦି (vpi_trace) {
	    fprintf(vpi_trace, "vpi_iterate(%d, %p) ->%s\n",
	    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)type, ref, rtn ? "" : " (null)");
      }

      ଫେରନ୍ତୁ rtn;
}

vpiHandle vpi_handle_by_index(vpiHandle ref, PLI_INT32 idx)
{
      ପ୍ରତିଜ୍ଞା(ref);
      ଫେରନ୍ତୁ ref->vpi_index(idx);
}

ସ୍ଥିର vpiHandle find_name(ଅଚଳ ଅକ୍ଷର *name, vpiHandle handle)
{
      vpiHandle rtn = 0;
      __vpiScope*ref = dynamic_cast<__vpiScope*>(handle);

      /* check module names */
      ଯଦି (!strcmp(name, vpi_get_str(vpiName, handle)))
	    rtn = handle;

      /* brute force search for the name in all objects in this scope */
      ପାଇଁ (unsigned i = 0 ;  i < ref->intern.size() ;  i += 1) {
	      /* The standard says that since a port does not have a full
	       * name it cannot be found by name. Because of this we need
	       * to skip ports here so the correct handle can be located. */
	    ଯଦି (vpi_get(vpiType, ref->intern[i]) == vpiPort) ଜାରି;
	    ଅଚଳ ଅକ୍ଷର *nm = vpi_get_str(vpiName, ref->intern[i]);
	    ଯଦି (nm && !strcmp(name, nm)) {
		  rtn = ref->intern[i];
		  ଭାଙ୍ଗ;
	    } ଅନ୍ୟଥା ଯଦି (vpi_get(vpiType, ref->intern[i]) == vpiMemory ||
	               vpi_get(vpiType, ref->intern[i]) == vpiNetArray) {
		  /* We need to iterate on the words */
		  vpiHandle word_i, word_h;
		  word_i = vpi_iterate(vpiMemoryWord, ref->intern[i]);
		  ଯେତେବେଳେ (word_i && (word_h = vpi_scan(word_i))) {
			nm = vpi_get_str(vpiName, word_h);
			ଯଦି (nm && !strcmp(name, nm)) {
			      rtn = word_h;
			      vpi_free_object(word_i);
			      ଭାଙ୍ଗ;
			}
		  }
	    }
	    /* found it yet? */
	    ଯଦି (rtn) ଭାଙ୍ଗ;
      }

      ଫେରନ୍ତୁ rtn;
}

// Find the end of the escaped identifier or simple identifier
ସ୍ଥିର ଅକ୍ଷର * find_rest(ଅକ୍ଷର *name)
{
      ଅକ୍ଷର *rest;
      ଯଦି (*name == '\\') {
	    rest = strchr(name, ' ');
	    ଯଦି (rest) {
		  *rest++ = 0; // The space is not part of the string
		    // There should be a '.' after the escaped ID if there is
		    // anything more. If it is missing add it to avoid a crash
		  ଯଦି ((*rest != '.') && (*rest != 0)) {
			*--rest = '.';
			fprintf(stderr, "ERROR: Malformed scope string: \"%s\"", name);
		  }
		  ଯଦି (*rest == 0) {
			rest = 0;
		  }
	    }
      } ଅନ୍ୟଥା {
	    rest = strchr(name, '.');
      }
      ଫେରନ୍ତୁ rest;
}

ସ୍ଥିର vpiHandle find_scope(ଅଚଳ ଅକ୍ଷର *name, vpiHandle handle, ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth)
{

      vpiHandle iter = handle==0
	    ? vpi_iterate(vpiModule, ଖାଲି)
	    : vpi_iterate(vpiInternalScope, handle);

      vector<ଅକ୍ଷର> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      ଅକ୍ଷର*nm_first = &name_buf[0];
      ଅକ୍ଷର*nm_rest = find_rest(nm_first);
      ଯଦି (*nm_first == '\\') ++nm_first;
      ଯଦି (nm_rest) {
	    *nm_rest++ = 0;
      }

      vpiHandle rtn = 0;
      vpiHandle hand;
      ଯେତେବେଳେ (iter && (hand = vpi_scan(iter))) {
	    ଅଚଳ ଅକ୍ଷର *nm = vpi_get_str(vpiName, hand);

	    ଯଦି (strcmp(nm_first,nm)==0) {
		  ଯଦି (nm_rest)
			rtn=find_scope(nm_rest, hand, depth+1);
		  ଅନ୍ୟଥା
			rtn = hand;
	    }

	    /* found it yet ? */
	    ଯଦି (rtn) {
		  vpi_free_object(iter);
		  ଭାଙ୍ଗ;
	    }
      }

      ଫେରନ୍ତୁ rtn;
}

// Find the end of the first escaped identifier or simple identifier
ସ୍ଥିର ଅକ୍ଷର * find_next(ଅକ୍ଷର *name)
{
      ଅକ୍ଷର *next;
      ଯଦି (*name == '\\') {
	    next = strchr(name, ' ');
	    ଯଦି (next && *++next == 0) next = 0;
      } ଅନ୍ୟଥା {
	    next = strchr(name, '.');
      }
      ଫେରନ୍ତୁ next;
}

vpiHandle vpi_handle_by_name(ଅଚଳ ଅକ୍ଷର *name, vpiHandle scope)
{
      vpiHandle hand;

      ଯଦି (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name(%s, %p) -->\n",
		    name, scope);
      }

	// Chop the name into path and base. For example, if the name
	// is "a.b.c", then nm_path becomes "a.b" and nm_base becomes
	// "c". If the name is "c" then nm_path is nil and nm_base is "c".
      vector<ଅକ୍ଷର> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      ଅକ୍ଷର*nm_path = &name_buf[0];
      ଅକ୍ଷର*nm_base;

	// Check to see if we have an escaped identifier and if so search
	// the long way because a '.' could be in the escaped name.
      ଯଦି (strchr(nm_path, '\\')) {
	    ଅକ୍ଷର *next;
	    nm_base = nm_path;
	    ଯେତେବେଳେ ((next = find_next(nm_base))) {
		  nm_base = ++next;
	    }
	    ଯଦି (nm_path == nm_base) {
		  nm_path = 0;
	    } ଅନ୍ୟଥା {
		  *(nm_base-1) = 0;
	    }

	// If there is no escaped identifier then just look for the last '.'
      } ଅନ୍ୟଥା {
	    nm_base = strrchr(nm_path, '.');
	    ଯଦି (nm_base) {
		  *nm_base++ = 0;
	    } ଅନ୍ୟଥା {
		  nm_base = nm_path;
		  nm_path = 0;
	    }
      }

      /* If scope provided, look in corresponding module; otherwise
       * traverse the hierarchy specified in name to find the leaf module
       * and try finding it there.
       */
      ଯଦି (scope) {
	    /* Some implementations support either a module or a scope. */
	    ବାଛ (vpi_get(vpiType, scope)) {
		ସ୍ଥିତି vpiScope:
	          hand = vpi_handle(vpiModule, scope);
	          ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiModule:
	          hand = scope;
	          ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  ଯଦି (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle_by_name: "
				"Scope is not a vpiScope or vpiModule\n");
		  }
	          // Use vpi_chk_error() here when it is implemented.
	          ଫେରନ୍ତୁ 0;
	    }

      } ଅନ୍ୟଥା ଯଦି (nm_path) {
	      // The name has a path, and no other scope handle was
	      // passed in. That suggests we are looking for "a.b.c"
	      // in the root scope. So convert "a.b" to a scope and
	      // start there to look for "c".
	    hand = find_scope(nm_path, ଖାଲି, 0);
	    nm_path = 0;

      } ଅନ୍ୟଥା {
	      // Special case: scope==<nil>, meaning we are looking in
	      // the root, and there is no path to the name, i.e. the
	      // string is "c" instead of "top.c". Try to find "c" as
	      // a scope and return that.
	    hand = find_scope(nm_base, ଖାଲି, 0);
      }

      ଯଦି (hand == 0) {
	    ଯଦି (vpi_trace) {
		  fprintf(vpi_trace, "vpi_handle_by_name: "
			  "Scope does not exist. Giving up.\n");
	    }

	    ଫେରନ୍ତୁ 0;
      }

	// If there is a path part, then use it to find the
	// scope. For example, if the full name is a.b.c, then
	// the nm_path string is a.b and we search for that
	// scope. If we find it, then set hand to that scope.
      ଯଦି (nm_path) {
	    vpiHandle tmp = find_scope(nm_path, hand, 0);
	    ଯେତେବେଳେ (tmp == 0 && hand != 0) {
		  hand = vpi_handle(vpiScope, hand);
		  tmp = find_scope(nm_path, hand, 0);
	    }
	    hand = tmp;
      }

	// find_name() expects escaped identifiers to be stripped
      ଯଦି (*nm_base == '\\') {
	    // Skip the \ at the beginning
	    ++nm_base;

	    // Drop the space at the end if it exists
	    ଅକ୍ଷର *next;
	    ଯଦି ((next = strchr(nm_base, ' '))) {
		  *next = 0;
	    }
      }

	// Now we have the correct scope, look for the item.
      vpiHandle out = find_name(nm_base, hand);

      ଯଦି (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name: DONE\n");
      }

      ଫେରନ୍ତୁ out;
}

// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected
// x2: Ptr to previous node
// x3: True when net is connected to output of previous node
tuple<ତାର୍କିକ, vvp_net_ptr_t, ତାର୍କିକ> check_connected_to_concat8_and_part_sa(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent) {

      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      ଯେତେବେଳେ (cur.ptr()) {
	    ଯଦି (cur.ptr() == net2 && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
		    // Pass on the return value if found
		  ଫେରନ୍ତୁ {ସତ୍ୟ, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
	    } ଅନ୍ୟଥା ଯଦି (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8_and_part_sa(cur.ptr()->out_, net2, cur);
		  ଯଦି (get<0>(res)) {
			ଫେରନ୍ତୁ res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      ଫେରନ୍ତୁ {ମିଥ୍ୟା, vvp_net_ptr_t(nullptr, 0), ମିଥ୍ୟା};
}

// Check if net2 is connected to current_net through a net of vvp_fun_concat8s
// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected with concats and part_sas
// x2: Ptr to previous node
// x3: True when net is connected to output of x2
tuple<ତାର୍କିକ, vvp_net_ptr_t, ତାର୍କିକ> check_connected_to_concat8(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent)
{
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);
	// For everything connected
      ଯେତେବେଳେ (cur.ptr()) {
	      // Check if it's a concat8
	    ଯଦି (cur.ptr() == net2) {
		  ଫେରନ୍ତୁ {ସତ୍ୟ, vvp_net_ptr_t(nullptr, 0), ମିଥ୍ୟା};
	    } ଅନ୍ୟଥା ଯଦି (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8(cur.ptr()->out_, net2, cur);
		  ଯଦି (get<0>(res)) {
			ଯଦି (!get<1>(res).ptr() && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
			      // `parent` is the last part_sa node in front of our destination
			      // We want to place the intermod node here
			      ଫେରନ୍ତୁ {ସତ୍ୟ, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
			}
			ଫେରନ୍ତୁ res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      ଫେରନ୍ତୁ {ମିଥ୍ୟା, vvp_net_ptr_t(nullptr, 0), ମିଥ୍ୟା};
}

// Used to get intermodpath for two ports
vpiHandle vpi_handle_multi(PLI_INT32 type,
                           vpiHandle ref1,
                           vpiHandle ref2)
{
      ଯଦି (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_multi(%d, %p, %p) -->\n",
	            type, ref1, ref2);
      }

      ଯଦି (type != vpiInterModPath) {
	    fprintf(stderr, "sorry: vpi_handle_multi currently supports"
	            "only vpiInterModPath\n");
	    ଫେରନ୍ତୁ nullptr;
      }

	// Indicates whether port1 refers to a single bit
      ତାର୍କିକ port1_has_index = ମିଥ୍ୟା;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା port1_bit_index = 0;
      vpiPortBitInfo* port1_bit = dynamic_cast<vpiPortBitInfo*>(ref1);

      ଯଦି (port1_bit) {
	      // Get the bit index
	    port1_has_index = ସତ୍ୟ;
	    port1_bit_index = vpi_get(vpiBit, port1_bit);
	      // Update the ref1 to point to the base port
	    ref1 = vpi_handle(vpiParent, port1_bit);
      }

	// Indicates whether port2 refers to a single bit
      ତାର୍କିକ port2_has_index = ମିଥ୍ୟା;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା port2_bit_index = 0;
      vpiPortBitInfo* port2_bit = dynamic_cast<vpiPortBitInfo*>(ref2);

      ଯଦି (port2_bit) {
	      // Get the bit index
	    port2_has_index = ସତ୍ୟ;
	    port2_bit_index = vpi_get(vpiBit, port2_bit);
	      // Update the ref1 to point to the base port
	    ref2 = vpi_handle(vpiParent, port2_bit);
      }

      vpiPortInfo* port1 = dynamic_cast<vpiPortInfo*>(ref1);

      ଯଦି (!port1) {
	    fprintf(stderr, "sorry: second argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    ଫେରନ୍ତୁ nullptr;
      }

      vpiPortInfo* port2 = dynamic_cast<vpiPortInfo*>(ref2);

      ଯଦି (!port2) {
	    fprintf(stderr, "sorry: third argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    ଫେରନ୍ତୁ nullptr;
      }

	// Get the names of both ports
      std::string port1_name(vpi_get_str(vpiName, ref1));
      std::string port2_name(vpi_get_str(vpiName, ref2));

	// If both ports are vpiOutput, we have to reassign the __vpiSignal from port1
	// to port2 because otherwise the non-delayed version of the signal is dumped
	// even tho the intermodpath is correctly inserted
      __vpiSignal* output_signal = nullptr;

      ଯଦି (port1->get_direction() == vpiOutput && port2->get_direction() == vpiOutput) {
	    vpiHandle scope_port2 = vpi_handle(vpiScope, ref2);
	    ପ୍ରତିଜ୍ଞା(scope_port2);

	      // Iterate over nets in the scope of port2
	    vpiHandle net_i = vpi_iterate(vpiNet, scope_port2) ;
	    vpiHandle net;

	    ଯେତେବେଳେ ((net = vpi_scan(net_i)) != ଖାଲି) {
		  std::string net_name(vpi_get_str(vpiName, net));

		    // Compare whether the net matches with the port name
		  ଯଦି (net_name == port2_name) {
			output_signal = dynamic_cast<__vpiSignal*>(net);
		  }
	    }
      }

      vvp_net_t* net1 = port1->get_port();
      vvp_net_t* net2 = port2->get_port();

      ଯଦି (net1 == nullptr || net2 == nullptr) {
	    fprintf(stderr, "Error: Could not find net. "
		    "Did you run iverilog with '-ginterconnect'?\n");
	    ଫେରନ୍ତୁ nullptr;
      }

      ଯଦି (net1 == net2) {
	    fprintf(stderr, "Error: Net for both ports is the same. "
		    "Did you pass the same port twice?\n");
	    ଫେରନ୍ତୁ nullptr;
      }

      ଯଦି (!dynamic_cast<vvp_fun_buft*>(net1->fun)) {
	    fprintf(stderr, "Error: functor of net1 must be"
		    "vvp_fun_buft\n");
	    ଫେରନ୍ତୁ nullptr;
      }

      ଯଦି (!dynamic_cast<vvp_fun_buft*>(net2->fun)) {
	    fprintf(stderr, "Error: functor of net2 must be"
		    "vvp_fun_buft\n");
	    ଫେରନ୍ତୁ nullptr;
      }

	// If port1 is actually a port bit, we have to get to the correct vvp_fun_part
	// after which we insert the intermodpath delay
      ଯଦି (port1_has_index) {
	    vvp_net_ptr_t* net1_ptr = &net1->out_;

	      // Search for part selects connected to port1
	    vvp_net_t* current_net = net1_ptr->ptr();

	    ଯେତେବେଳେ (current_net) {
		  ଅଚଳ vvp_fun_part* part = dynamic_cast<vvp_fun_part*>(current_net->fun);

		    // Its a part select!
		  ଯଦି (part) {
			  // Is it the correct part select?
			ଯଦି (part->get_base() == (unsigned)port1_bit_index) {
			      ପ୍ରତିଜ୍ଞା(part->get_wid() == 1);
			      net1 = current_net; // Replace net1 as this is our new start point
			      ଭାଙ୍ଗ;
			}
		  }

		  current_net = current_net->port[0].ptr(); // BUFT has only one input, index 0
	    }
      }

	// Iterate over all nodes connected to port1
      vvp_net_ptr_t cur = net1->out_;
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      ଯେତେବେଳେ (cur.ptr()) {
	      // Either port2 is directly connected to port1
	      // Or in the second case port2 is indirectly connected
	      // to port1 through a net of concat8s
	    ତାର୍କିକ is_connected, is_output;
	    vvp_net_ptr_t previous_ଗଣ୍ଠି;
	    tie(is_connected, previous_ଗଣ୍ଠି, is_output) = port2_has_index
		  ? check_connected_to_concat8(cur, net2, vvp_net_ptr_t(net1, 0))
		  : check_connected_to_concat8_and_part_sa(cur, net2, vvp_net_ptr_t(net1, 0));
	    ଯଦି ( (!port2_has_index && cur.ptr() == net2) || is_connected ) {
		  vvp_net_t*new_net = ନୂତନ vvp_net_t;

		    // Create new node with intermodpath and connect port2 to it
		  ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 1; // TODO
		  vvp_fun_intermodpath*obj = ନୂତନ vvp_fun_intermodpath(new_net, width);

		  vvp_net_t* output_net = net1;
		  ଯଦି (is_connected) {
			ଯଦି (is_output) {
			      output_net = previous_ଗଣ୍ଠି.ptr(); // net2 is direct output of part_sa
			      cur = output_net->out_;
			      prev = vvp_net_ptr_t(nullptr, 0);
			} ଅନ୍ୟଥା {
			      prev = previous_ଗଣ୍ଠି;
			      ଯଦି (prev.ptr()) {
				    cur = prev.ptr()->port[prev.port()];
			      }
			}
		  }

		  new_net->fun = obj;
		  new_net->out_ = cur;

		    // Port2 is in the middle of the list
		    // Insert intermodpath before port2 and keep everything else intact
		  ଯଦି (prev.ptr()) {
			prev.ptr()->port[prev.port()] = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; // Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		    // Port2 is first in list
		    // Insert intermodpath before port2 and keep everything else intact
		  } ଅନ୍ୟଥା {
			output_net->out_ = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; //  Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		  }

		    // If both ports are vpiOutput and port2 is not a vector,
		    // we have to reassign the __vpiSignal so that the delayed
		    // values get dumped
		  ଯଦି (output_signal && !port2_has_index) {
			net2->fil = net1->fil;
			net1->fil = nullptr;
			output_signal->ଗଣ୍ଠି = net2;
		  }

		    // Create the VPI intermodpath object
		  __vpiInterModPath* intermodpath = vpip_make_intermodpath(new_net, port1, port2);
		  intermodpath->intermodpath = obj;

		    // Finally done, return the intermodpath object
		  ଫେରନ୍ତୁ intermodpath;
	    }

	    prev = cur;
	    cur = cur.ptr()->port[cur.port()]; // Next net in linked list
      }

      fprintf(stderr, "VPI error: Could not insert intermodpath!\n");
      fprintf(stderr, "\tport1 = %s, port1_has_index = %d, port1_bit_index = %d\n", port1_name.c_str(), port1_has_index, port1_bit_index);
      fprintf(stderr, "\tport2 = %s, port2_has_index = %d, port2_bit_index = %d\n", port2_name.c_str(), port2_has_index, port2_bit_index);
      ଫେରନ୍ତୁ nullptr;
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



ଶୂନ୍ୟ vpi_get_delays(vpiHandle ପଦସମଷ୍ଟି, p_vpi_delay delays)
{
      ପ୍ରତିଜ୍ଞା(ପଦସମଷ୍ଟି);
      ପ୍ରତିଜ୍ଞା(delays);

      ପଦସମଷ୍ଟି->vpi_get_delays(delays);

      ଯଦି (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_get_delays(%p, %p) -->\n", ପଦସମଷ୍ଟି, delays);
      }
}


ଶୂନ୍ୟ vpi_put_delays(vpiHandle ପଦସମଷ୍ଟି, p_vpi_delay delays)
{
      ପ୍ରତିଜ୍ଞା(ପଦସମଷ୍ଟି  );
      ପ୍ରତିଜ୍ଞା(delays );

      ପଦସମଷ୍ଟି->vpi_put_delays(delays);

      ଯଦି (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_put_delays(%p, %p) -->\n", ପଦସମଷ୍ଟି, delays);
      }
}








ବାହ୍ୟ "C" PLI_INT32 vpi_vprintf(ଅଚଳ ଅକ୍ଷର*fmt, va_list ap)
{
      ଫେରନ୍ତୁ vpi_mcd_vprintf(1, fmt, ap);
}

ବାହ୍ୟ "C" PLI_INT32 vpi_printf(ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = vpi_mcd_vprintf(1, fmt, ap);
      va_end(ap);
      ଫେରନ୍ତୁ r;
}

ବାହ୍ୟ "C" PLI_INT32 vpi_flush(ଶୂନ୍ୟ)
{
      ଫେରନ୍ତୁ vpi_mcd_flush(1);
}


ବାହ୍ୟ "C" PLI_INT32 vpi_sim_vcontrol(ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation, va_list ap)
{
      long diag_ସନ୍ଦେଶ;

      ବାଛ (operation) {
	  ସ୍ଥିତି vpiFinish:
	  ସ୍ଥିତି __ivl_legacy_vpiFinish:
            diag_ସନ୍ଦେଶ = va_arg(ap, PLI_INT32);
	    schedule_finish(diag_ସନ୍ଦେଶ);
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiStop:
	  ସ୍ଥିତି __ivl_legacy_vpiStop:
            diag_ସନ୍ଦେଶ = va_arg(ap, PLI_INT32);
	    schedule_stop(diag_ସନ୍ଦେଶ);
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported VPI control operation %d.\n", operation);
	    ଫେରନ୍ତୁ 0;
      }
      ଫେରନ୍ତୁ 1;
}

ବାହ୍ୟ "C" PLI_INT32 vpi_sim_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      ଫେରନ୍ତୁ r;
}

ବାହ୍ୟ "C" PLI_INT32 vpi_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      ଫେରନ୍ତୁ r;
}

/*
 * This routine calculated the return value for $clog2.
 * It is easier to do it here vs trying to to use the VPI interface.
 */
ବାହ୍ୟ "C" s_vpi_vecval vpip_calc_clog2(vpiHandle arg)
{
      s_vpi_vecval rtn;
      s_vpi_ମୂଲ୍ୟ val;
      vvp_vector4_t vec4;
      ତାର୍କିକ is_neg = ମିଥ୍ୟା;  // At this point only a real can be negative.

	/* Get the value as a vvp_vector4_t. */
      val.format = vpiObjTypeVal;
      vpi_get_ମୂଲ୍ୟ(arg, &val);
      ଯଦି (val.format == vpiRealVal) {
	    vpi_get_ମୂଲ୍ୟ(arg, &val);
	      /* All double values can be represented in 1024 bits. */
	    vec4 = vvp_vector4_t(1024, val.value.real);
	    ଯଦି (val.value.real < 0) is_neg = ସତ୍ୟ;
      } ଅନ୍ୟଥା {
	    val.format = vpiVectorVal;
	    vpi_get_ମୂଲ୍ୟ(arg, &val);
	    unsigned wid = vpi_get(vpiSize, arg);
	    vec4 = vvp_vector4_t(wid, BIT4_0);
	    ପାଇଁ (unsigned idx=0; idx < wid; idx += 1) {
		PLI_INT32 aval = val.value.vector[idx/32].aval;
		PLI_INT32 bval = val.value.vector[idx/32].bval;
		aval >>= idx % 32;
		bval >>= idx % 32;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା bitmask = (aval&1) | ((bval<<1)&2);
		vvp_bit4_t bit = scalar_to_bit4(bitmask);
		vec4.set_bit(idx, bit);
	    }
      }

      ଯଦି (vec4.has_xz()) {
	    rtn.aval = rtn.bval = 0xFFFFFFFFU;  /* Set to 'bx. */
	    ଫେରନ୍ତୁ rtn;
      }

      vvp_vector2_t vec2(vec4);

      ଯଦି (is_neg) vec2.trim_neg(); /* This is a special trim! */
      ଅନ୍ୟଥା vec2.trim(); /* This makes less work shifting. */

	/* Calculate the clog2 result. */
      PLI_INT32 res = 0;
      ଯଦି (!vec2.is_zero()) {
	    vec2 -= vvp_vector2_t(1, vec2.size());
	    ଯେତେବେଳେ(!vec2.is_zero()) {
		  res += 1;
		  vec2 >>= 1;
	    }
      }

      rtn.aval = res;
      rtn.bval = 0;
      ଫେରନ୍ତୁ rtn;
}

/*
 * This routine provides the information needed to implement $countdrivers.
 * It is done here for performance reasons - interrogating the drivers
 * individually via the VPI interface would be much slower.
 */
ବାହ୍ୟ "C" ଶୂନ୍ୟ vpip_ଗଣନା_drivers(vpiHandle ref, unsigned idx,
                                   unsigned counts[4])
{
      ସଂରଚନା __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);
      rfp->ଗଣ୍ଠି->ଗଣନା_drivers(idx, counts);
}

#ଯଦି defined(__MINGW32__) || defined (__CYGWIN__)
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
    .get_ମୂଲ୍ୟ                  = vpi_get_ମୂଲ୍ୟ,
    .put_ମୂଲ୍ୟ                  = vpi_put_ମୂଲ୍ୟ,
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
    .ଗଣନା_drivers              = vpip_ଗଣନା_drivers,
    .format_strength            = vpip_format_strength,
    .format_pretty              = vpip_format_pretty,
    .make_systf_system_defined  = vpip_make_systf_system_defined,
    .mcd_rawwrite               = vpip_mcd_rawwrite,
    .set_return_ମୂଲ୍ୟ           = vpip_set_return_ମୂଲ୍ୟ,
};
#endif
