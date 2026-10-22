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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <vector>
# include  <cstdio>
# include  <cstdarg>
# include  <cstring>
# include  <cassert>
# include  <cstdlib>
# include  <cmath>
# include  <iostream>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;
vpi_ਬਹੁਲਕ_t vpi_ਬਹੁਲਕ_flag = VPI_ਬਹੁਲਕ_NONE;
FILE*vpi_trace = 0;

ਸਥਿਰ s_vpi_vlog_info  vpi_vlog_info;
ਸਥਿਰ s_vpi_error_info vpip_ਆਖਰੀ_error = { 0, 0, 0, 0, 0, 0, 0 };

ਅਚਲ ਅੱਖਰ* direction_as_string(ਪੂਰਨ dir)
{
      ਚੋਣ (dir) {
	  ਹਾਲਤ vpi_ਨਿਵੇਸ਼:
	    ਵਾਪਸ "input";
	  ਹਾਲਤ vpiOutput:
	    ਵਾਪਸ "output";
	  ਹਾਲਤ vpiInout:
	    ਵਾਪਸ "inout";
	  ਹਾਲਤ vpiMixedIO:
	    ਵਾਪਸ "mixed io";
	  ਹਾਲਤ vpiNoDirection:
	    ਵਾਪਸ "no direction";
	  ਮੂਲ:
	    ਵਾਪਸ "INVALID-DIRECTION";
      }
}

__vpiHandle::~__vpiHandle()
{ }

ਪੂਰਨ __vpiHandle::vpi_get(ਪੂਰਨ)
{ ਵਾਪਸ vpiUndefined; }

int64_t __vpiHandle::vpi_get64(ਪੂਰਨ code)
{ ਵਾਪਸ (code == _vpiNexusId) ? 0 : vpiUndefined; }

ਅੱਖਰ* __vpiHandle::vpi_get_str(ਪੂਰਨ)
{ ਵਾਪਸ 0; }

ਖਾਲੀ __vpiHandle::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ)
{ }

vpiHandle __vpiHandle::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ, ਪੂਰਨ)
{ ਵਾਪਸ 0; }

vpiHandle __vpiHandle::vpi_handle(ਪੂਰਨ)
{ ਵਾਪਸ 0; }

vpiHandle __vpiHandle::vpi_iterate(ਪੂਰਨ)
{ ਵਾਪਸ 0; }

vpiHandle __vpiHandle::vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ)
{ ਵਾਪਸ 0; }

ਖਾਲੀ __vpiHandle::vpi_get_delays(p_vpi_delay)
{ }

ਖਾਲੀ __vpiHandle::vpi_put_delays(p_vpi_delay)
{ }

__vpiBaseVar::__vpiBaseVar(__vpiScope*scope, ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net)
: scope_(scope), name_(name), net_(net)
{
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
__vpiBaseVar::~__vpiBaseVar()
{
      vvp_net_delete(net_);
}
#ਜੇ_ਅੰਤ

/*
 * The default behavior for the vpi_free_object to an object is to
 * suppress the actual operation. This is because handles are
 * generally allocated semi-permanently within vvp context. Dynamic
 * objects will override the free_object_fun method to return an
 * appropriately effective function.
 */
ਸਥਿਰ ਪੂਰਨ suppress_free(vpiHandle)
{ ਵਾਪਸ 1; }
__vpiHandle::free_object_fun_t __vpiHandle::free_object_fun(ਖਾਲੀ)
{ ਵਾਪਸ &suppress_free; }

/*
 * The vpip_string function creates a constant string from the pass
 * input. This constant string is permanently allocated from an
 * efficient string buffer store.
 */
ਬਣਤਰ vpip_string_chunk {
      ਬਣਤਰ vpip_string_chunk*ਅਗਲਾ_2;
      ਅੱਖਰ ਅੰਕੜੇ[64*1024 - ਆਕਾਰ (ਬਣਤਰ vpip_string_chunk*)];
};

ਬਿਨਾਨਿਸ਼ਾਨੀ vpip_ਮਾਪ(ਅਚਲ __vpiSignal *sig)
{
      ਵਾਪਸ ਨਿਰਪੇਖ(sig->msb.get_ਮੁੱਲ() - sig->lsb.get_ਮੁੱਲ()) + 1;
}

__vpiScope* vpip_scope(__vpiSignal*sig)
{
      ਜੇ (sig->is_netarray)
	    ਵਾਪਸ static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.ਮਾਪਾ));
      ਨਹੀਂਤਾਂ
	    ਵਾਪਸ sig->within.scope;
}

__vpiScope* vpip_scope(__vpiRealVar*sig)
{
      ਜੇ (sig->is_netarray)
	    ਵਾਪਸ static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.ਮਾਪਾ));
      ਨਹੀਂਤਾਂ
	    ਵਾਪਸ sig->within.scope;
}

vpiHandle vpip_module(__vpiScope*scope)
{
      ਜਦੋਂ(scope && scope->get_type_ਕੋਡ() != vpiModule) {
	    scope = scope->scope;
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(scope);
      ਵਾਪਸ scope;
}

ਅਚਲ ਅੱਖਰ *vpip_string(ਅਚਲ ਅੱਖਰ*str)
{
      ਸਥਿਰ vpip_string_chunk ਪਹਿਲਾ_chunk = {0, {0}};
      ਸਥਿਰ vpip_string_chunk*chunk_list = &ਪਹਿਲਾ_chunk;
      ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ chunk_fill = 0;

      ਬਿਨਾਨਿਸ਼ਾਨੀ len = ਲੜੀ_ਲੰਬਾਈ(str);
      ਪ੍ਰਤਿਜ੍ਞਾ( (len+1) <= ਆਕਾਰ chunk_list->ਅੰਕੜੇ );

      ਜੇ ( (len+1) > (ਆਕਾਰ chunk_list->ਅੰਕੜੇ - chunk_fill) ) {
	    vpip_string_chunk*tmp = ਨਵਾਂ vpip_string_chunk;
	    tmp->ਅਗਲਾ_2 = chunk_list;
	    chunk_list = tmp;
	    chunk_fill = 0;
      }

      ਅੱਖਰ*res = chunk_list->ਅੰਕੜੇ + chunk_fill;
      chunk_fill += len + 1;

      strcpy(res, str);
      ਵਾਪਸ res;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ hash_string(ਅਚਲ ਅੱਖਰ*ਪਾਠ)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ h = 0;

      ਜਦੋਂ (*ਪਾਠ) {
	    h = (h << 4) ^ (h >> 28) ^ (ਬਿਨਾਨਿਸ਼ਾਨੀ)*ਪਾਠ;
	    ਪਾਠ += 1;
      }
      ਵਾਪਸ h;
}

ਅਚਲ ਅੱਖਰ* vpip_name_string(ਅਚਲ ਅੱਖਰ*ਪਾਠ)
{
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ HASH_ਮਾਪ = 4096;
      ਸਥਿਰ ਅਚਲ ਅੱਖਰ*hash_ਸਾਰਣੀ[HASH_ਮਾਪ] = {0};

      ਬਿਨਾਨਿਸ਼ਾਨੀ hash_ਮੁੱਲ = hash_string(ਪਾਠ) % HASH_ਮਾਪ;

	/* If we easily find the string in the hash table, then return
	   that and be done. */
      ਜੇ (hash_ਸਾਰਣੀ[hash_ਮੁੱਲ]
	  && (strcmp(hash_ਸਾਰਣੀ[hash_ਮੁੱਲ], ਪਾਠ) == 0)) {
	    ਵਾਪਸ hash_ਸਾਰਣੀ[hash_ਮੁੱਲ];
      }

	/* The existing hash entry is not a match. Replace it with the
	   newly allocated value, and return the new pointer as the
	   result to the add. */
      ਅਚਲ ਅੱਖਰ*res = vpip_string(ਪਾਠ);
      hash_ਸਾਰਣੀ[hash_ਮੁੱਲ] = res;

      ਵਾਪਸ res;
}

PLI_INT32 vpi_chk_error(p_vpi_error_info info)
{
      ਜੇ (vpip_ਆਖਰੀ_error.state == 0)
	    ਵਾਪਸ 0;

      info->state = vpip_ਆਖਰੀ_error.state;
      info->level = vpip_ਆਖਰੀ_error.level;
      info->message = vpip_ਆਖਰੀ_error.message;
      info->ਗੁਣਨਫਲ = vpi_vlog_info.ਗੁਣਨਫਲ;
      info->code = (ਅੱਖਰ *)"";
      info->file = 0;
      info->ਸਤਰ = 0;

      ਵਾਪਸ info->level;
}

PLI_INT32 vpi_compare_objects(vpiHandle obj1, vpiHandle obj2)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(obj1);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj2);

	// Does this actually work for all cases?
      ਜੇ (obj1 != obj2) ਵਾਪਸ 0;
      ਨਹੀਂਤਾਂ ਵਾਪਸ 1;
}

/*
 * Copy the internal information to the data structure. Do not free or
 * change the tfname/user_data since they are a pointer to the real
 * string/data values. We also support passing a task or function handle
 * instead of just a handle to a vpiUserSystf.
 */
ਖਾਲੀ vpi_get_systf_info(vpiHandle ref, p_vpi_systf_ਅੰਕੜੇ ਅੰਕੜੇ)
{
      ਬਣਤਰ __vpiUserSystf* rfp = dynamic_cast<__vpiUserSystf*>(ref);
      ਜੇ (rfp == 0) {
	    ਬਣਤਰ __vpiSysTaskCall*call = dynamic_cast<__vpiSysTaskCall*>(ref);
	    ਪ੍ਰਤਿਜ੍ਞਾ(call);
	    rfp = call->defn;
      }

	/* Assert that vpiUserDefn is true! */
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp->is_user_defn);

      ਅੰਕੜੇ->type = rfp->info.type;
      ਅੰਕੜੇ->sysfunctype = rfp->info.sysfunctype;
      ਅੰਕੜੇ->tfname = rfp->info.tfname;
      ਅੰਕੜੇ->calltf = rfp->info.calltf;
      ਅੰਕੜੇ->compiletf = rfp->info.compiletf;
      ਅੰਕੜੇ->sizetf = rfp->info.sizetf;
      ਅੰਕੜੇ->user_ਅੰਕੜੇ = rfp->info.user_ਅੰਕੜੇ;
}

/*
 * When a task is called, this value is set so that vpi_handle can
 * fathom the vpi_handle(vpiSysTfCall,0) function.
 */
ਬਣਤਰ __vpiSysTaskCall*vpip_cur_task = 0;

PLI_INT32 vpi_free_object(vpiHandle ref)
{
      ਪੂਰਨ rtn;

      ਜੇ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_free_object(%p)", ref);
	    fflush(vpi_trace);
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(ref);
      __vpiHandle::free_object_fun_t fun = ref->free_object_fun();
      rtn = fun (ref);

      ਜੇ (vpi_trace)
	    fprintf(vpi_trace, " --> %d\n", rtn);

      ਵਾਪਸ rtn;
}

PLI_INT32 vpi_release_handle(vpiHandle ref)
{
	// Since SystemVerilog vpi_free_object() has been
	// renamed vpi_release_handle(), and thus
	// vpi_free_object() has been deprecated.
      ਵਾਪਸ vpi_free_object(ref);
}

ਸਥਿਰ ਪੂਰਨ vpip_get_global(ਪੂਰਨ property)
{
      ਚੋਣ (property) {

	  ਹਾਲਤ vpi_ਸਮਾਂ_Unit:
	  ਹਾਲਤ vpi_ਸਮਾਂ_Precision:
	    ਵਾਪਸ vpip_get_ਸਮਾਂ_precision();

	  ਮੂਲ:
	    fprintf(stderr, "vpi error: bad global property: %d\n", property);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ vpiUndefined;
      }
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* vpi_property_str(PLI_INT32 code)
{
      ਸਥਿਰ ਅੱਖਰ buf[32];
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiConstType:
	    ਵਾਪਸ "vpiConstType";
	  ਹਾਲਤ vpiName:
	    ਵਾਪਸ "vpiName";
	  ਹਾਲਤ vpi_ਭਰਿਆ_Name:
	    ਵਾਪਸ "vpiFullName";
	  ਹਾਲਤ vpi_ਸਮਾਂ_Unit:
	    ਵਾਪਸ "vpiTimeUnit";
	  ਹਾਲਤ vpi_ਸਮਾਂ_Precision:
	    ਵਾਪਸ "vpiTimePrecision";
          ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ "vpiSize";
          ਹਾਲਤ _vpiNexusId:
	    ਵਾਪਸ "_vpiNexusId";
	  ਮੂਲ:
	    snprintf(buf, ਆਕਾਰ(buf), "%d", (ਪੂਰਨ)code);
      }
      ਵਾਪਸ buf;
}

ਅਚਲ ਅੱਖਰ* vpi_type_as_string(PLI_INT32 code)
{
      ਸਥਿਰ ਅੱਖਰ buf[32];
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiArrayType:
	    ਵਾਪਸ "vpiArrayType";
	  ਹਾਲਤ vpi_ਦੋਆਧਾਰੀ_ਅੰਕ_Var:
	    ਵਾਪਸ "vpiBitVar";
	  ਹਾਲਤ vpiByteVar:
	    ਵਾਪਸ "vpiByteVar";
	  ਹਾਲਤ vpiClassVar:
	    ਵਾਪਸ "vpiClassVar";
	  ਹਾਲਤ vpiConstant:
	    ਵਾਪਸ "vpiConstant";
	  ਹਾਲਤ vpiEnumTypespec:
	    ਵਾਪਸ "vpiEnumTypespec";
	  ਹਾਲਤ vpiFunction:
	    ਵਾਪਸ "vpiFunction";
	  ਹਾਲਤ vpiGenScope:
	    ਵਾਪਸ "vpiGenScope";
	  ਹਾਲਤ vpiIntVar:
	    ਵਾਪਸ "vpiIntVar";
	  ਹਾਲਤ vpiIntegerVar:
	    ਵਾਪਸ "vpiIntegerVar";
	  ਹਾਲਤ vpiIterator:
	    ਵਾਪਸ "vpiIterator";
	  ਹਾਲਤ vpiLongIntVar:
	    ਵਾਪਸ "vpiLongIntVar";
	  ਹਾਲਤ vpi_ਸਮ੍ਰਿਤੀ:
	    ਵਾਪਸ "vpiMemory";
	  ਹਾਲਤ vpi_ਸਮ੍ਰਿਤੀ_ਸ਼ਬਦ:
	    ਵਾਪਸ "vpiMemoryWord";
	  ਹਾਲਤ vpiModule:
	    ਵਾਪਸ "vpiModule";
	  ਹਾਲਤ vpiNamedBegin:
	    ਵਾਪਸ "vpiNamedBegin";
	  ਹਾਲਤ vpiNamedEvent:
	    ਵਾਪਸ "vpiNamedEvent";
	  ਹਾਲਤ vpiNamedFork:
	    ਵਾਪਸ "vpiNamedFork";
	  ਹਾਲਤ vpiPackage:
	    ਵਾਪਸ "vpiPackage";
	  ਹਾਲਤ vpi_ਰਸਤਾ_ਪਦ:
	    ਵਾਪਸ "vpiPathTerm";
	  ਹਾਲਤ vpi_ਪੋਰਟ:
	    ਵਾਪਸ "vpiPort";
	  ਹਾਲਤ vpiNet:
	    ਵਾਪਸ "vpiNet";
	  ਹਾਲਤ vpiNetArray:
	    ਵਾਪਸ "vpiNetArray";
	  ਹਾਲਤ vpi_Net_ਦੋਆਧਾਰੀ_ਅੰਕ:
	    ਵਾਪਸ "vpiNetBit";
	  ਹਾਲਤ vpiParameter:
	    ਵਾਪਸ "vpiParameter";
	  ਹਾਲਤ vpiPartSelect:
	    ਵਾਪਸ "vpiPartSelect";
	  ਹਾਲਤ vpiRealVar:
	    ਵਾਪਸ "vpiRealVar";
	  ਹਾਲਤ vpiReg:
	    ਵਾਪਸ "vpiReg";
	  ਹਾਲਤ vpi_Reg_ਦੋਆਧਾਰੀ_ਅੰਕ:
	    ਵਾਪਸ "vpiRegBit";
	  ਹਾਲਤ vpiShortIntVar:
	    ਵਾਪਸ "vpiShortIntVar";
	  ਹਾਲਤ vpiStringVar:
	    ਵਾਪਸ "vpiStringVar";
	  ਹਾਲਤ vpiSysFuncCall:
	    ਵਾਪਸ "vpiSysFuncCall";
	  ਹਾਲਤ vpiSysTaskCall:
	    ਵਾਪਸ "vpiSysTaskCall";
	  ਹਾਲਤ vpiTask:
	    ਵਾਪਸ "vpiTask";
	  ਹਾਲਤ vpi_ਸਮਾਂ_Var:
	    ਵਾਪਸ "vpiTimeVar";
	  ਹਾਲਤ vpiUserSystf:
	    ਵਾਪਸ "vpiUserSystf";
	  ਮੂਲ:
	    snprintf(buf, ਆਕਾਰ(buf), "%d", (ਪੂਰਨ)code);
      }
      ਵਾਪਸ buf;
}

PLI_INT32 vpi_get(ਪੂਰਨ property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      ਜੇ (property == _vpiDelaySelection) ਵਾਪਸ vpip_delay_selection;

      ਜੇ (ref == 0)
	    ਵਾਪਸ vpip_get_global(property);

      ਜੇ (property == vpiType) {
	    ਜੇ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_ਕੋਡ()));
	    }

	    ਜੇ (ref->get_type_ਕੋਡ() == vpi_ਸਮ੍ਰਿਤੀ && is_net_array(ref))
		  ਵਾਪਸ vpiNetArray;
	    ਨਹੀਂਤਾਂ
		  ਵਾਪਸ ref->get_type_ਕੋਡ();
      }

      ਪੂਰਨ res = ref->vpi_get(property);

      ਜੇ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get(%s, %p) --> %d\n",
		    vpi_property_str(property), ref, res);
      }

      ਵਾਪਸ res;
}

PLI_INT64 vpi_get64(ਪੂਰਨ property, vpiHandle ref)
{
	// For now we only support getting the nexus id.
      ਜੇ ((ref == 0) || (property != _vpiNexusId)) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) is not currently supported.\n",
		    vpi_property_str(property), ref);
	    ਵਾਪਸ vpiUndefined;
      }

      int64_t res = ref->vpi_get64(property);

      ਜੇ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) --> %" PRId64 "\n",
		    vpi_property_str(property), ref, res);
      }

      ਵਾਪਸ res;
}

ਅੱਖਰ* vpi_get_str(PLI_INT32 property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      ਜੇ (property == _vpiDelaySelection) {
	    ਚੋਣ (vpip_delay_selection) {
		ਹਾਲਤ _vpiDelaySelMinimum:
		  ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str("MINIMUM");
		ਹਾਲਤ _vpiDelaySelTypical:
		  ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str("TYPICAL");
		ਹਾਲਤ vpi_Delay_Sel_ਵੱਧ_ਤੋਂ_ਵੱਧ:
		  ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str("MAXIMUM");
		ਮੂਲ:
		  ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    }
      }

      ਜੇ (ref == 0) {
	    fprintf(stderr, "vpi error: vpi_get_str(%s, 0) called "
		    "with null vpiHandle.\n", vpi_property_str(property));
	    ਵਾਪਸ 0;
      }

      ਜੇ (property == vpiType) {
	    ਜੇ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_ਕੋਡ()));
	    }

            PLI_INT32 type;
	    ਜੇ (ref->get_type_ਕੋਡ() == vpi_ਸਮ੍ਰਿਤੀ && is_net_array(ref))
		  type = vpiNetArray;
	    ਨਹੀਂਤਾਂ
		  type = ref->get_type_ਕੋਡ();
	    ਵਾਪਸ (ਅੱਖਰ *)vpi_type_as_string(type);
      }

      ਅੱਖਰ*res = ref->vpi_get_str(property);

      ਜੇ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get_str(%s, %p) --> %s\n",
		    vpi_property_str(property), ref, res? res : "<NULL>");
      }

      ਵਾਪਸ res;
}

__vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj)
{
      ਅਚਲ ਬਣਤਰ __vpiSysTaskCall*task;
      ਬਣਤਰ __vpiSignal*ਸੰਕੇਤ;
      ਬਣਤਰ __vpiRealVar*real;
      ਅਚਲ __vpiNamedEvent*event;

      ਚੋਣ (obj->get_type_ਕੋਡ()) {
	  ਹਾਲਤ vpiSysTaskCall:
	    task = dynamic_cast<__vpiSysTaskCall*>(obj);
	    ਵਾਪਸ task->scope;

	  ਹਾਲਤ vpiModule:
	    ਵਾਪਸ dynamic_cast<__vpiScope*>(obj);

	  ਹਾਲਤ vpiNet:
	  ਹਾਲਤ vpiReg:
	  ਹਾਲਤ vpiIntegerVar:
	  ਹਾਲਤ vpi_ਦੋਆਧਾਰੀ_ਅੰਕ_Var:
	  ਹਾਲਤ vpiByteVar:
	  ਹਾਲਤ vpiShortIntVar:
	  ਹਾਲਤ vpiIntVar:
	  ਹਾਲਤ vpiLongIntVar:
	    ਸੰਕੇਤ = dynamic_cast<__vpiSignal*>(obj);
	    ਵਾਪਸ vpip_scope(ਸੰਕੇਤ);

	  ਹਾਲਤ vpiRealVar:
	    real = dynamic_cast<__vpiRealVar*>(obj);
	    ਵਾਪਸ vpip_scope(real);

	  ਹਾਲਤ vpiNamedEvent:
	    event = dynamic_cast<__vpiNamedEvent*>(obj);
	    ਵਾਪਸ event->get_scope();

	  ਹਾਲਤ vpi_ਸਮ੍ਰਿਤੀ:
	  ਹਾਲਤ vpi_ਸਮ੍ਰਿਤੀ_ਸ਼ਬਦ:
	  ਹਾਲਤ vpiPartSelect:
	    ਵਾਪਸ dynamic_cast<__vpiScope*>(obj->vpi_handle(vpiScope));

	  ਮੂਲ:
	    fprintf(stderr, "ERROR: vpip_scope_from_handle called with "
		    "object handle type=%d\n", obj->get_type_ਕੋਡ());
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ 0;
      }
}

ਪੂਰਨ vpip_ਸਮਾਂ_units_from_handle(vpiHandle obj)
{
      ਜੇ (obj == 0)
	    ਵਾਪਸ vpip_get_ਸਮਾਂ_precision();

      ਅਚਲ __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      ਜੇ (scope == 0)
	    ਵਾਪਸ vpip_get_ਸਮਾਂ_precision();

      ਵਾਪਸ scope->ਸਮਾਂ_units;
}

ਪੂਰਨ vpip_ਸਮਾਂ_precision_from_handle(vpiHandle obj)
{
      ਜੇ (obj == 0)
	    ਵਾਪਸ vpip_get_ਸਮਾਂ_precision();

      ਅਚਲ __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      ਜੇ (scope == 0)
	    ਵਾਪਸ vpip_get_ਸਮਾਂ_precision();

      ਵਾਪਸ scope->ਸਮਾਂ_precision;
}

ਦੁੱਗਣਾ vpip_scaled_ਸਮਾਂ_from_handle(vvp_time64_t time, vpiHandle obj)
{
      ਪੂਰਨ scale = vpip_get_ਸਮਾਂ_precision() -
	          vpip_ਸਮਾਂ_units_from_handle(obj);
      ਜੇ (scale >= 0)
	    ਵਾਪਸ (ਦੁੱਗਣਾ)time * ਘਾਤ(10.0, scale);
      ਨਹੀਂਤਾਂ
	    ਵਾਪਸ (ਦੁੱਗਣਾ)time / ਘਾਤ(10.0, -scale);
}

ਖਾਲੀ vpi_get_ਸਮਾਂ(vpiHandle obj, s_vpi_ਸਮਾਂ*vp)
{
      vvp_time64_t time;

      ਪ੍ਰਤਿਜ੍ਞਾ(vp);

      time = ਸਮਾਂ_ਸਾਰਣੀ_simtime();

      ਚੋਣ (vp->type) {
          ਹਾਲਤ vpi_Sim_ਸਮਾਂ:
	    vp->ਉੱਚਾ = (time >> 32) & 0xffffffff;
	    vp->ਨੀਵਾਂ = time & 0xffffffff;
	    ਰੋਕੋ;

          ਹਾਲਤ vpi_Scaled_Real_ਸਮਾਂ:
	    vp->real = vpip_scaled_ਸਮਾਂ_from_handle(time, obj);
	    ਰੋਕੋ;

          ਮੂਲ:
            fprintf(stderr, "vpi_get_time: unknown type: %d\n", (ਪੂਰਨ)vp->type);
            ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਰੋਕੋ;
      }
}

PLI_INT32 vpi_get_vlog_info(p_vpi_vlog_info vlog_info_p)
{
    ਜੇ (vlog_info_p != 0) {
	  *vlog_info_p = vpi_vlog_info;
	  ਵਾਪਸ 1;
    } ਨਹੀਂਤਾਂ {
	  ਵਾਪਸ 0;
    }
}

ਖਾਲੀ vpi_ਸੈੱਟ_vlog_info(ਪੂਰਨ argc, ਅੱਖਰ** argv)
{
    ਸਥਿਰ ਅੱਖਰ icarus_ਗੁਣਨਫਲ[] = "Icarus Verilog";
    ਸਥਿਰ ਅੱਖਰ icarus_ਸੰਸਕਰਣ[] = ਸੰਸਕਰਣ;
    vpi_vlog_info.ਗੁਣਨਫਲ = icarus_ਗੁਣਨਫਲ;
    vpi_vlog_info.ਸੰਸਕਰਣ_2 = icarus_ਸੰਸਕਰਣ;
    vpi_vlog_info.argc    = argc;
    vpi_vlog_info.argv    = argv;

    ਸਥਿਰ ਅੱਖਰ trace_buf[1024];
    ਜੇ (ਅਚਲ ਅੱਖਰ*ਰਸਤਾ = getenv("VPI_TRACE"))  {
	  ਜੇ (!strcmp(ਰਸਤਾ,"-"))
		vpi_trace = stdout;
	  ਨਹੀਂਤਾਂ {
		vpi_trace = fopen(ਰਸਤਾ, "w");
		ਜੇ (!vpi_trace) {
		      perror(ਰਸਤਾ);
		      ਬਾਹਰ_ਨਿਕਲੋ(1);
		}
		setvbuf(vpi_trace, trace_buf, _IOLBF, ਆਕਾਰ(trace_buf));
	  }
    }
}

ਸਥਿਰ ਖਾਲੀ vec4_get_ਮੁੱਲ_string(ਅਚਲ vvp_vector4_t&ਸ਼ਬਦ_val, ਬਿਨਾਨਿਸ਼ਾਨੀ width,
				  s_vpi_ਮੁੱਲ*vp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ nchar = width / 8;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਛ = width % 8;

      ਅੱਖਰ*rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(nchar + 1, RBUF_VAL));
      ਅੱਖਰ*cp = rbuf;

      ਜੇ (ਪੂਛ > 0) {
	    ਅੱਖਰ char_val = 0;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = width-ਪੂਛ; idx < width ;  idx += 1) {
		  vvp_bit4_t val = ਸ਼ਬਦ_val.value(idx);
		  ਜੇ (val == BIT4_1)
			char_val |= 1 << idx;
	    }

	    ਜੇ (char_val != 0)
		  *cp++ = char_val;
	    ਨਹੀਂਤਾਂ ਜੇ (cp != rbuf)
		  *cp++ = ' ';
      }

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < nchar ;  idx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ = (nchar - idx - 1) * 8;
	    ਅੱਖਰ char_val = 0;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ bdx = 0 ;  bdx < 8 ;  bdx += 1) {
		  vvp_bit4_t val = ਸ਼ਬਦ_val.value(ਦੋਆਧਾਰੀ_ਅੰਕ+bdx);
		  ਜੇ (val == BIT4_1)
			char_val |= 1 << bdx;
	    }
	    // Ignore leading null-bytes and replace other null-bytes with space.
	    // The LRM is not entirely clear on how null bytes should be handled.
	    // This is the implementation chosen for iverilog.
	    ਜੇ (char_val != 0)
		  *cp++ = char_val;
	    ਨਹੀਂਤਾਂ ਜੇ (cp != rbuf)
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
ਖਾਲੀ vpip_vec4_get_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&ਸ਼ਬਦ_val, ਬਿਨਾਨਿਸ਼ਾਨੀ width,
			 ਤਾਰਕਿਕ signed_flag, s_vpi_ਮੁੱਲ*vp)
{
      ਅੱਖਰ *rbuf = 0;

      ਚੋਣ (vp->format) {
	  ਮੂਲ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector values.\n", (ਪੂਰਨ)vp->format);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);

	  ਹਾਲਤ vpiSuppressVal:
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiBinStrVal:
	    rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(width+1, RBUF_VAL));
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < width ;  idx += 1) {
		  vvp_bit4_t ਦੋਆਧਾਰੀ_ਅੰਕ = ਸ਼ਬਦ_val.value(idx);
		  rbuf[width-idx-1] = vvp_bit4_ਮੰਜ਼ਿਲ_ascii(ਦੋਆਧਾਰੀ_ਅੰਕ);
	    }
	    rbuf[width] = 0;
	    vp->value.str = rbuf;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiOctStrVal: {
		ਬਿਨਾਨਿਸ਼ਾਨੀ hwid = ((width+2) / 3) + 1;
		rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(hwid, RBUF_VAL));
		vpip_vec4_ਮੰਜ਼ਿਲ_oct_str(ਸ਼ਬਦ_val, rbuf, hwid);
		vp->value.str = rbuf;
		ਰੋਕੋ;
	  }

	  ਹਾਲਤ vpiDecStrVal: {
// HERE need a better estimate.
		rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(width+1, RBUF_VAL));
		vpip_vec4_ਮੰਜ਼ਿਲ_dec_str(ਸ਼ਬਦ_val, rbuf, width+1, signed_flag);
		vp->value.str = rbuf;
		ਰੋਕੋ;
	  }

	  ਹਾਲਤ vpiHexStrVal: {
		ਬਿਨਾਨਿਸ਼ਾਨੀ  hwid = ((width + 3) / 4) + 1;
		rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(hwid, RBUF_VAL));
		vpip_vec4_ਮੰਜ਼ਿਲ_hex_str(ਸ਼ਬਦ_val, rbuf, hwid);
		vp->value.str = rbuf;
		ਰੋਕੋ;
	  }

          ਹਾਲਤ vpiScalarVal: {
	        // scalars should be of size 1
	        ਪ੍ਰਤਿਜ੍ਞਾ(width == 1);
	        ਚੋਣ(ਸ਼ਬਦ_val.value(0)) {
		    ਹਾਲਤ BIT4_0:
		      vp->value.scalar = vpi0;
		      ਰੋਕੋ;
	            ਹਾਲਤ BIT4_1:
                      vp->value.scalar = vpi1;
                      ਰੋਕੋ;
	            ਹਾਲਤ BIT4_X:
                      vp->value.scalar = vpiX;
                      ਰੋਕੋ;
	            ਹਾਲਤ BIT4_Z:
                      vp->value.scalar = vpiZ;
                      ਰੋਕੋ;
		}
                ਰੋਕੋ;
          }

	  ਹਾਲਤ vpiIntVal: {
		ਲੰਮਾ val = 0;
		vvp_bit4_t pad = BIT4_0;
		ਜੇ (signed_flag && ਸ਼ਬਦ_val.size() > 0)
		      pad = ਸ਼ਬਦ_val.value(ਸ਼ਬਦ_val.size()-1);

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < 8*ਆਕਾਰ(val) ;  idx += 1) {
		      vvp_bit4_t val4 = pad;
		      ਜੇ (idx < ਸ਼ਬਦ_val.size())
			    val4 = ਸ਼ਬਦ_val.value(idx);
		      ਜੇ (val4 == BIT4_1)
			    val |= 1L << idx;
		}

		vp->value.integer = val;
		ਰੋਕੋ;
	  }

	  ਹਾਲਤ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpi_ਸਦਿਸ਼_Val;
	    // fallthrough
	  ਹਾਲਤ vpi_ਸਦਿਸ਼_Val: {
		ਬਿਨਾਨਿਸ਼ਾਨੀ hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_ਨਤੀਜਾ_buf(hwid * ਆਕਾਰ(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < width ;  idx += 1) {
		      ਚੋਣ (ਸ਼ਬਦ_val.value(idx)) {
			  ਹਾਲਤ BIT4_0:
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    ਰੋਕੋ;
			  ਹਾਲਤ BIT4_1:
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    ਰੋਕੋ;
			  ਹਾਲਤ BIT4_X:
			    op->aval |= (1 << idx % 32);
			    op->bval |= (1 << idx % 32);
			    ਰੋਕੋ;
			  ਹਾਲਤ BIT4_Z:
			    op->aval &= ~(1 << idx % 32);
			    op->bval |=  (1 << idx % 32);
			    ਰੋਕੋ;
		      }
		      ਜੇ (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		ਰੋਕੋ;
	  }

	  ਹਾਲਤ vpiStringVal:
		vec4_get_ਮੁੱਲ_string(ਸ਼ਬਦ_val, width, vp);
		ਰੋਕੋ;

	  ਹਾਲਤ vpiRealVal:
		vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਸ਼ਬਦ_val, vp->value.real, signed_flag);
		ਰੋਕੋ;
      }
}

ਖਾਲੀ vpip_vec2_get_ਮੁੱਲ(ਅਚਲ vvp_vector2_t&ਸ਼ਬਦ_val, ਬਿਨਾਨਿਸ਼ਾਨੀ width,
			 ਤਾਰਕਿਕ signed_flag, s_vpi_ਮੁੱਲ*vp)
{
      ਚੋਣ (vp->format) {
	  ਮੂਲ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector2 values.\n", (ਪੂਰਨ)vp->format);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);

	  ਹਾਲਤ vpiSuppressVal:
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiIntVal:
	    vector2_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਸ਼ਬਦ_val, vp->value.integer, signed_flag);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiObjTypeVal:
	    vp->format = vpi_ਸਦਿਸ਼_Val;
	    // fallthrough
	  ਹਾਲਤ vpi_ਸਦਿਸ਼_Val: {
		ਬਿਨਾਨਿਸ਼ਾਨੀ hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_ਨਤੀਜਾ_buf(hwid * ਆਕਾਰ(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < width ;  idx += 1) {
		      ਜੇ (ਸ਼ਬਦ_val.value(idx)) {
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      } ਨਹੀਂਤਾਂ {
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      }
		      ਜੇ (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		ਰੋਕੋ;
	  }
      }

}

/*
 * Convert a real value to the appropriate integer.
 */
ਸਥਿਰ PLI_INT32 get_real_as_int(ਦੁੱਗਣਾ real)
{
      ਦੁੱਗਣਾ rtn;

	/* We would normally want to return 'bx for a NaN or
	 * +/- infinity, but for an integer the standard says
	 * to convert 'bx to 0, so we just return 0. */
      ਜੇ (real != real || (real && (real == 0.5*real))) {
	    ਵਾਪਸ 0;
      }

      rtn = std::ਗੇੜਾ_2(real);

      ਵਾਪਸ (PLI_INT32) rtn;
}

/*
 * This is a generic function to convert a double value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer.
 */
ਖਾਲੀ vpip_real_get_ਮੁੱਲ(ਦੁੱਗਣਾ real, s_vpi_ਮੁੱਲ*vp)
{
      ਅੱਖਰ *rbuf = 0;

      ਚੋਣ (vp->format) {
	  ਮੂਲ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting real values.\n", (ਪੂਰਨ)vp->format);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);

	  ਹਾਲਤ vpiSuppressVal:
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiRealVal;
	    // fallthrough
	  ਹਾਲਤ vpiRealVal:
	    vp->value.real = real;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiIntVal:
	    vp->value.integer = get_real_as_int(real);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiDecStrVal:
	    rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(1025, RBUF_VAL));
	    vpip_vec4_ਮੰਜ਼ਿਲ_dec_str(vvp_vector4_t(1024, real), rbuf, 1025, ਸੱਚ);
	    vp->value.str = rbuf;
	    ਰੋਕੋ;
      }
}

ਦੁੱਗਣਾ real_from_vpi_ਮੁੱਲ(s_vpi_ਮੁੱਲ*vp)
{
      vvp_vector4_t vec4(1024);
      ਦੁੱਗਣਾ ਨਤੀਜਾ;
      ਤਾਰਕਿਕ is_signed = ਝੂਠ;

      ਚੋਣ (vp->format) {
	  ਮੂਲ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "putting real values.\n", (ਪੂਰਨ)vp->format);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);

	  ਹਾਲਤ vpiRealVal:
	    ਨਤੀਜਾ = vp->value.real;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiIntVal:
	    ਨਤੀਜਾ = (ਦੁੱਗਣਾ) vp->value.integer;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiBinStrVal:
	    vpip_bin_str_ਮੰਜ਼ਿਲ_vec4(vec4, vp->value.str);
	    ਜੇ (vp->value.str[0] == '-') is_signed = ਸੱਚ;
	    vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(vec4, ਨਤੀਜਾ, is_signed);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiOctStrVal:
	    vpip_oct_str_ਮੰਜ਼ਿਲ_vec4(vec4, vp->value.str);
	    ਜੇ (vp->value.str[0] == '-') is_signed = ਸੱਚ;
	    vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(vec4, ਨਤੀਜਾ, is_signed);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiDecStrVal:
	    vpip_dec_str_ਮੰਜ਼ਿਲ_vec4(vec4, vp->value.str);
	    ਜੇ (vp->value.str[0] == '-') is_signed = ਸੱਚ;
	    vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(vec4, ਨਤੀਜਾ, is_signed);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiHexStrVal:
	    vpip_hex_str_ਮੰਜ਼ਿਲ_vec4(vec4, vp->value.str);
	    ਜੇ (vp->value.str[0] == '-') is_signed = ਸੱਚ;
	    vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(vec4, ਨਤੀਜਾ, is_signed);
	    ਰੋਕੋ;

      }

      ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ vpip_string_get_ਮੁੱਲ(ਅਚਲ string&val, s_vpi_ਮੁੱਲ*vp)
{
      ਅੱਖਰ *rbuf = 0;

      ਚੋਣ (vp->format) {
	  ਮੂਲ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting string values.\n", (ਪੂਰਨ)vp->format);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);

	  ਹਾਲਤ vpiSuppressVal:
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiStringVal;
	    // fallthrough
	  ਹਾਲਤ vpiStringVal:
	    rbuf = static_cast<ਅੱਖਰ *>(need_ਨਤੀਜਾ_buf(val.size() + 1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    ਰੋਕੋ;
      }
}


ਖਾਲੀ vpi_get_ਮੁੱਲ(vpiHandle ਵਿਅੰਜਕ, s_vpi_ਮੁੱਲ*vp)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿਅੰਜਕ);
      ਪ੍ਰਤਿਜ੍ਞਾ(vp);

	// Never bother with suppressed values. All the derived
	// classes can ignore this type.
      ਜੇ (vp->format == vpiSuppressVal)
	    ਵਾਪਸ;

      ਵਿਅੰਜਕ->vpi_get_ਮੁੱਲ(vp);

      ਜੇ (vpi_trace) ਚੋਣ (vp->format) {
		ਹਾਲਤ vpiStringVal:
		  fprintf(vpi_trace,"vpi_get_value(%p=<%d>) -> string=\"%s\"\n",
			  ਵਿਅੰਜਕ, ਵਿਅੰਜਕ->get_type_ਕੋਡ(), vp->value.str);
		  ਰੋਕੋ;

		ਹਾਲਤ vpiBinStrVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> binstr=%s\n",
			  ਵਿਅੰਜਕ->get_type_ਕੋਡ(), vp->value.str);
		  ਰੋਕੋ;

		ਹਾਲਤ vpiIntVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> int=%d\n",
			  ਵਿਅੰਜਕ->get_type_ਕੋਡ(), (ਪੂਰਨ)vp->value.integer);
		  ਰੋਕੋ;

		ਹਾਲਤ vpiSuppressVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <suppress>\n",
			  ਵਿਅੰਜਕ->get_type_ਕੋਡ());
		  ਰੋਕੋ;

		ਮੂਲ:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <%d>=?\n",
			  ਵਿਅੰਜਕ->get_type_ਕੋਡ(), (ਪੂਰਨ)vp->format);
	    }
}

ਬਣਤਰ vpip_put_ਮੁੱਲ_event : vvp_gen_event_s {
      vpiHandle handle;
      s_vpi_ਮੁੱਲ value;
      ਪੂਰਨ flags;
      ਆਭਾਸੀ ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;
      ~vpip_put_ਮੁੱਲ_event() override { }
};

ਖਾਲੀ vpip_put_ਮੁੱਲ_event::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      handle->vpi_put_ਮੁੱਲ(&value, flags);
      ਚੋਣ (value.format) {
	    /* Free the copied string. */
	  ਹਾਲਤ vpiBinStrVal:
	  ਹਾਲਤ vpiOctStrVal:
	  ਹਾਲਤ vpiDecStrVal:
	  ਹਾਲਤ vpiHexStrVal:
	  ਹਾਲਤ vpiStringVal:
	    ਮੁਕਤ(value.value.str);
	    ਰੋਕੋ;
	    /* Free the copied time structure. */
	  ਹਾਲਤ vpi_ਸਮਾਂ_Val:
	    ਮੁਕਤ(value.value.time);
	    ਰੋਕੋ;
	    /* Free the copied vector structure. */
	  ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:
	    ਮੁਕਤ(value.value.vector);
	    ਰੋਕੋ;
	    /* Free the copied strength structure. */
	  ਹਾਲਤ vpiStrengthVal:
	    ਮੁਕਤ(value.value.strength);
	    ਰੋਕੋ;
	    /* Everything else is static in the structure. */
	  ਮੂਲ:
	    ਰੋਕੋ;
      }
}

/* Make a copy of a pointer to a time structure. */
ਸਥਿਰ t_vpi_ਸਮਾਂ *timedup(ਅਚਲ t_vpi_ਸਮਾਂ *val)
{
      t_vpi_ਸਮਾਂ *rtn;
      rtn = static_cast<t_vpi_ਸਮਾਂ *> (ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਆਕਾਰ(t_vpi_ਸਮਾਂ)));
      *rtn = *val;
      ਵਾਪਸ rtn;
}

/* Make a copy of a pointer to a vector value structure. */
ਸਥਿਰ t_vpi_vecval *vectordup(ਅਚਲ t_vpi_vecval *val, PLI_INT32 size)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੰਕ_ਬਾਈਟਾਂ;
      t_vpi_vecval *rtn;
      ਪ੍ਰਤਿਜ੍ਞਾ(size > 0);
      ਅੰਕ_ਬਾਈਟਾਂ = ((size + 31)/32)*ਆਕਾਰ(t_vpi_vecval);
      rtn = static_cast<t_vpi_vecval *> (ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਅੰਕ_ਬਾਈਟਾਂ));
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(rtn, val, ਅੰਕ_ਬਾਈਟਾਂ);
      ਵਾਪਸ rtn;
}

/* Make a copy of a pointer to a strength structure. */
ਸਥਿਰ t_vpi_strengthval *strengthdup(ਅਚਲ t_vpi_strengthval *val)
{
      t_vpi_strengthval *rtn;
      rtn = static_cast<t_vpi_strengthval *>
            (ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਆਕਾਰ(t_vpi_strengthval)));
      *rtn = *val;
      ਵਾਪਸ rtn;
}

vpiHandle vpi_put_ਮੁੱਲ(vpiHandle obj, s_vpi_ਮੁੱਲ*vp,
			s_vpi_ਸਮਾਂ*when, PLI_INT32 flags)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);

      flags &= ~vpiReturnEvent;

      ਜੇ (flags!=vpiNoDelay && flags!=vpiForceFlag && flags!=vpiReleaseFlag) {
	    vvp_time64_t dly;
	    ਪੂਰਨ scale;

	    ਜੇ (vpi_get(vpiAutomatic, obj)) {
		  fprintf(stderr, "VPI error: cannot put a value with "
				  "a delay on automatically allocated "
				  "variable '%s'.\n",
				  vpi_get_str(vpiName, obj));
		  ਵਾਪਸ 0;
	    }

	    ਪ੍ਰਤਿਜ੍ਞਾ(when != 0);

	    ਚੋਣ (when->type) {
		ਹਾਲਤ vpi_Scaled_Real_ਸਮਾਂ:
		  scale = vpip_ਸਮਾਂ_units_from_handle(obj) -
		          vpip_get_ਸਮਾਂ_precision();
		  ਜੇ (scale >= 0) {
			dly = (vvp_time64_t)(when->real * ਘਾਤ(10.0, scale));
		  } ਨਹੀਂਤਾਂ {
			dly = (vvp_time64_t)(when->real / ਘਾਤ(10.0, -scale));
		  }
		  ਰੋਕੋ;
		ਹਾਲਤ vpi_Sim_ਸਮਾਂ:
		  dly = vpip_timestruct_ਮੰਜ਼ਿਲ_ਸਮਾਂ(when);
		  ਰੋਕੋ;
		ਮੂਲ:
		  dly = 0;
		  ਰੋਕੋ;
	    }

	    ਜੇ ((dly == 0) && ਸਮਾਂ_ਸਾਰਣੀ_at_rosync()) {
		  fprintf(stderr, "VPI error: attempted to put a value to "
				  "variable '%s' during a read-only synch "
				  "callback.\n", vpi_get_str(vpiName, obj));
		  ਵਾਪਸ 0;
	    }

	    vpip_put_ਮੁੱਲ_event*put = ਨਵਾਂ vpip_put_ਮੁੱਲ_event;
	    put->handle = obj;
	    ਜੇ (dynamic_cast<__vpiNamedEvent*>(obj)) {
		  put->value.format = vpiIntVal;
		  put->value.value.integer = 0;
	    } ਨਹੀਂਤਾਂ {
		  ਪ੍ਰਤਿਜ੍ਞਾ(vp);
		  put->value = *vp;
	    }
	      /* Since this is a scheduled put event we must copy any pointer
	       * data to keep it available until the event is actually run. */
	    ਚੋਣ (put->value.format) {
		  /* Copy the string items. */
		ਹਾਲਤ vpiBinStrVal:
		ਹਾਲਤ vpiOctStrVal:
		ਹਾਲਤ vpiDecStrVal:
		ਹਾਲਤ vpiHexStrVal:
		ਹਾਲਤ vpiStringVal:
		  put->value.value.str = strdup(put->value.value.str);
		  ਰੋਕੋ;
		  /* Copy a time pointer item. */
		ਹਾਲਤ vpi_ਸਮਾਂ_Val:
		  put->value.value.time = timedup(put->value.value.time);
		  ਰੋਕੋ;
		  /* Copy a vector pointer item. */
		ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:
		  put->value.value.vector = vectordup(put->value.value.vector,
		                                      vpi_get(vpi_ਮਾਪ, obj));
		  ਰੋਕੋ;
		  /* Copy a strength pointer item. */
		ਹਾਲਤ vpiStrengthVal:
		  put->value.value.strength =
		      strengthdup(put->value.value.strength);
		  ਰੋਕੋ;
		  /* Everything thing else is already in the structure. */
		ਮੂਲ:
		  ਰੋਕੋ;
	    }
	    put->flags = flags;
	    ਸਮਾਂ_ਸਾਰਣੀ_generic(put, dly, ਝੂਠ, ਸੱਚ, ਸੱਚ);
	    ਵਾਪਸ 0;
      }

      ਜੇ (ਸਮਾਂ_ਸਾਰਣੀ_at_rosync()) {
            fprintf(stderr, "VPI error: attempted to put a value to "
			    "variable '%s' during a read-only synch "
			    "callback.\n", vpi_get_str(vpiName, obj));
            ਵਾਪਸ 0;
      }

      obj->vpi_put_ਮੁੱਲ(vp, flags);

      ਵਾਪਸ 0;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle res = 0;

      ਜੇ (ref == 0) {
	      // A few types can apply to a nil handle. These are ways
	      // that the current function can get started finding things.
	    ਚੋਣ (type) {

		ਹਾਲਤ vpiScope:
		    // The IEEE1364-2005 doesn't seem to allow this,
		    // but some users seem to think it's handy, so
		    // return the scope that contains this SysTfCall.
		  ਪ੍ਰਤਿਜ੍ਞਾ(vpip_cur_task);
		  res = vpip_cur_task->vpi_handle(vpiScope);
		  ਰੋਕੋ;

		ਹਾਲਤ vpiSysTfCall:
		    // This is how VPI users get a first handle into
		    // the system. This is the handle of the system
		    // task/function call currently being executed.
		  ਜੇ (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle(vpiSysTfCall, 0) "
				"-> %p (%s)\n", vpip_cur_task,
				vpip_cur_task->defn->info.tfname);
		  }
		  ਵਾਪਸ vpip_cur_task;

		ਮੂਲ:
		  fprintf(stderr, "VPI error: vpi_handle(type=%d, ref=0).\n",
			  (ਪੂਰਨ)type);
		  res = 0;
		  ਰੋਕੋ;
	    }

      } ਨਹੀਂਤਾਂ {

	    ਜੇ (type == vpiSysTfCall) {
		  fprintf(stderr, "VPI error: vpi_handle(vpiSysTfCall, "
			  "ref!=0).\n");
		  ਵਾਪਸ 0;
	    }

	    res = ref->vpi_handle(type);
      }


      ਜੇ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle(vpiScope, ref=%p) "
		    "-> %p\n", vpip_cur_task, ref);
      }

      ਵਾਪਸ res;
}

ਸਥਿਰ vpiHandle vpip_make_udp_iterator()
{
// HERE: Add support for iterating over UDP definitions.
//       See 26.6.16 (page 400 in 1364-2005).
      ਵਾਪਸ 0;
}

/*
 * This function asks the object to return an iterator for
 * the specified reference. It is up to the iterate_ method to
 * allocate a properly formed iterator.
 */
ਸਥਿਰ vpiHandle vpi_iterate_global(ਪੂਰਨ type)
{
      ਚੋਣ (type) {
	  ਹਾਲਤ vpiInstance:
	    // fallthrough
	  ਹਾਲਤ vpiModule:
	    // fallthrough
	  ਹਾਲਤ vpi_ਪ੍ਰੋਗਰਾਮ:
	    // fallthrough
	  ਹਾਲਤ vpiInterface:
	    // fallthrough
	  ਹਾਲਤ vpiPackage:
	    ਵਾਪਸ vpip_make_ਜੜ੍ਹ_iterator(type);

	  ਹਾਲਤ vpiUdpDefn:
	    ਵਾਪਸ vpip_make_udp_iterator();

	  ਹਾਲਤ vpiUserSystf:
	    ਵਾਪਸ vpip_make_systf_iterator();
      }

      ਵਾਪਸ 0;
}

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle rtn = 0;

      ਪ੍ਰਤਿਜ੍ਞਾ(vpi_ਬਹੁਲਕ_flag != VPI_ਬਹੁਲਕ_NONE);
      ਜੇ (vpi_ਬਹੁਲਕ_flag == VPI_ਬਹੁਲਕ_ਰਜਿਸਟਰ) {
	    fprintf(stderr, "vpi error: vpi_iterate called during "
		    "vpi_register_systf. You can't do that!\n");
	    ਵਾਪਸ 0;
      }

      ਜੇ (ref == 0)
	    rtn = vpi_iterate_global(type);
      ਨਹੀਂਤਾਂ
	    rtn = ref->vpi_iterate(type);

      ਜੇ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_iterate(%d, %p) ->%s\n",
	    (ਪੂਰਨ)type, ref, rtn ? "" : " (null)");
      }

      ਵਾਪਸ rtn;
}

vpiHandle vpi_handle_by_ਸੂਚਕਾਂਕ(vpiHandle ref, PLI_INT32 idx)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ref);
      ਵਾਪਸ ref->vpi_ਸੂਚਕਾਂਕ(idx);
}

ਸਥਿਰ vpiHandle ਲੱਭੋ_name(ਅਚਲ ਅੱਖਰ *name, vpiHandle handle)
{
      vpiHandle rtn = 0;
      __vpiScope*ref = dynamic_cast<__vpiScope*>(handle);

      /* check module names */
      ਜੇ (!strcmp(name, vpi_get_str(vpiName, handle)))
	    rtn = handle;

      /* brute force search for the name in all objects in this scope */
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0 ;  i < ref->intern.size() ;  i += 1) {
	      /* The standard says that since a port does not have a full
	       * name it cannot be found by name. Because of this we need
	       * to skip ports here so the correct handle can be located. */
	    ਜੇ (vpi_get(vpiType, ref->intern[i]) == vpi_ਪੋਰਟ) ਜਾਰੀ;
	    ਅਚਲ ਅੱਖਰ *nm = vpi_get_str(vpiName, ref->intern[i]);
	    ਜੇ (nm && !strcmp(name, nm)) {
		  rtn = ref->intern[i];
		  ਰੋਕੋ;
	    } ਨਹੀਂਤਾਂ ਜੇ (vpi_get(vpiType, ref->intern[i]) == vpi_ਸਮ੍ਰਿਤੀ ||
	               vpi_get(vpiType, ref->intern[i]) == vpiNetArray) {
		  /* We need to iterate on the words */
		  vpiHandle ਸ਼ਬਦ_i, ਸ਼ਬਦ_h;
		  ਸ਼ਬਦ_i = vpi_iterate(vpi_ਸਮ੍ਰਿਤੀ_ਸ਼ਬਦ, ref->intern[i]);
		  ਜਦੋਂ (ਸ਼ਬਦ_i && (ਸ਼ਬਦ_h = vpi_ਜਾਂਚ(ਸ਼ਬਦ_i))) {
			nm = vpi_get_str(vpiName, ਸ਼ਬਦ_h);
			ਜੇ (nm && !strcmp(name, nm)) {
			      rtn = ਸ਼ਬਦ_h;
			      vpi_free_object(ਸ਼ਬਦ_i);
			      ਰੋਕੋ;
			}
		  }
	    }
	    /* found it yet? */
	    ਜੇ (rtn) ਰੋਕੋ;
      }

      ਵਾਪਸ rtn;
}

// Find the end of the escaped identifier or simple identifier
ਸਥਿਰ ਅੱਖਰ * ਲੱਭੋ_rest(ਅੱਖਰ *name)
{
      ਅੱਖਰ *rest;
      ਜੇ (*name == '\\') {
	    rest = strchr(name, ' ');
	    ਜੇ (rest) {
		  *rest++ = 0; // The space is not part of the string
		    // There should be a '.' after the escaped ID if there is
		    // anything more. If it is missing add it to avoid a crash
		  ਜੇ ((*rest != '.') && (*rest != 0)) {
			*--rest = '.';
			fprintf(stderr, "ERROR: Malformed scope string: \"%s\"", name);
		  }
		  ਜੇ (*rest == 0) {
			rest = 0;
		  }
	    }
      } ਨਹੀਂਤਾਂ {
	    rest = strchr(name, '.');
      }
      ਵਾਪਸ rest;
}

ਸਥਿਰ vpiHandle ਲੱਭੋ_scope(ਅਚਲ ਅੱਖਰ *name, vpiHandle handle, ਪੂਰਨ ਡੂੰਘਾਈ)
{

      vpiHandle iter = handle==0
	    ? vpi_iterate(vpiModule, ਨਿਰਾ)
	    : vpi_iterate(vpi_ਅੰਦਰੂਨੀ_Scope, handle);

      vector<ਅੱਖਰ> name_buf (ਲੜੀ_ਲੰਬਾਈ(name)+1);
      strcpy(&name_buf[0], name);
      ਅੱਖਰ*nm_ਪਹਿਲਾ = &name_buf[0];
      ਅੱਖਰ*nm_rest = ਲੱਭੋ_rest(nm_ਪਹਿਲਾ);
      ਜੇ (*nm_ਪਹਿਲਾ == '\\') ++nm_ਪਹਿਲਾ;
      ਜੇ (nm_rest) {
	    *nm_rest++ = 0;
      }

      vpiHandle rtn = 0;
      vpiHandle hand;
      ਜਦੋਂ (iter && (hand = vpi_ਜਾਂਚ(iter))) {
	    ਅਚਲ ਅੱਖਰ *nm = vpi_get_str(vpiName, hand);

	    ਜੇ (strcmp(nm_ਪਹਿਲਾ,nm)==0) {
		  ਜੇ (nm_rest)
			rtn=ਲੱਭੋ_scope(nm_rest, hand, ਡੂੰਘਾਈ+1);
		  ਨਹੀਂਤਾਂ
			rtn = hand;
	    }

	    /* found it yet ? */
	    ਜੇ (rtn) {
		  vpi_free_object(iter);
		  ਰੋਕੋ;
	    }
      }

      ਵਾਪਸ rtn;
}

// Find the end of the first escaped identifier or simple identifier
ਸਥਿਰ ਅੱਖਰ * ਲੱਭੋ_ਅਗਲਾ(ਅੱਖਰ *name)
{
      ਅੱਖਰ *ਅਗਲਾ_2;
      ਜੇ (*name == '\\') {
	    ਅਗਲਾ_2 = strchr(name, ' ');
	    ਜੇ (ਅਗਲਾ_2 && *++ਅਗਲਾ_2 == 0) ਅਗਲਾ_2 = 0;
      } ਨਹੀਂਤਾਂ {
	    ਅਗਲਾ_2 = strchr(name, '.');
      }
      ਵਾਪਸ ਅਗਲਾ_2;
}

vpiHandle vpi_handle_by_name(ਅਚਲ ਅੱਖਰ *name, vpiHandle scope)
{
      vpiHandle hand;

      ਜੇ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name(%s, %p) -->\n",
		    name, scope);
      }

	// Chop the name into path and base. For example, if the name
	// is "a.b.c", then nm_path becomes "a.b" and nm_base becomes
	// "c". If the name is "c" then nm_path is nil and nm_base is "c".
      vector<ਅੱਖਰ> name_buf (ਲੜੀ_ਲੰਬਾਈ(name)+1);
      strcpy(&name_buf[0], name);
      ਅੱਖਰ*nm_ਰਸਤਾ = &name_buf[0];
      ਅੱਖਰ*nm_ਅਧਾਰ;

	// Check to see if we have an escaped identifier and if so search
	// the long way because a '.' could be in the escaped name.
      ਜੇ (strchr(nm_ਰਸਤਾ, '\\')) {
	    ਅੱਖਰ *ਅਗਲਾ_2;
	    nm_ਅਧਾਰ = nm_ਰਸਤਾ;
	    ਜਦੋਂ ((ਅਗਲਾ_2 = ਲੱਭੋ_ਅਗਲਾ(nm_ਅਧਾਰ))) {
		  nm_ਅਧਾਰ = ++ਅਗਲਾ_2;
	    }
	    ਜੇ (nm_ਰਸਤਾ == nm_ਅਧਾਰ) {
		  nm_ਰਸਤਾ = 0;
	    } ਨਹੀਂਤਾਂ {
		  *(nm_ਅਧਾਰ-1) = 0;
	    }

	// If there is no escaped identifier then just look for the last '.'
      } ਨਹੀਂਤਾਂ {
	    nm_ਅਧਾਰ = strrchr(nm_ਰਸਤਾ, '.');
	    ਜੇ (nm_ਅਧਾਰ) {
		  *nm_ਅਧਾਰ++ = 0;
	    } ਨਹੀਂਤਾਂ {
		  nm_ਅਧਾਰ = nm_ਰਸਤਾ;
		  nm_ਰਸਤਾ = 0;
	    }
      }

      /* If scope provided, look in corresponding module; otherwise
       * traverse the hierarchy specified in name to find the leaf module
       * and try finding it there.
       */
      ਜੇ (scope) {
	    /* Some implementations support either a module or a scope. */
	    ਚੋਣ (vpi_get(vpiType, scope)) {
		ਹਾਲਤ vpiScope:
	          hand = vpi_handle(vpiModule, scope);
	          ਰੋਕੋ;
		ਹਾਲਤ vpiModule:
	          hand = scope;
	          ਰੋਕੋ;
		ਮੂਲ:
		  ਜੇ (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle_by_name: "
				"Scope is not a vpiScope or vpiModule\n");
		  }
	          // Use vpi_chk_error() here when it is implemented.
	          ਵਾਪਸ 0;
	    }

      } ਨਹੀਂਤਾਂ ਜੇ (nm_ਰਸਤਾ) {
	      // The name has a path, and no other scope handle was
	      // passed in. That suggests we are looking for "a.b.c"
	      // in the root scope. So convert "a.b" to a scope and
	      // start there to look for "c".
	    hand = ਲੱਭੋ_scope(nm_ਰਸਤਾ, ਨਿਰਾ, 0);
	    nm_ਰਸਤਾ = 0;

      } ਨਹੀਂਤਾਂ {
	      // Special case: scope==<nil>, meaning we are looking in
	      // the root, and there is no path to the name, i.e. the
	      // string is "c" instead of "top.c". Try to find "c" as
	      // a scope and return that.
	    hand = ਲੱਭੋ_scope(nm_ਅਧਾਰ, ਨਿਰਾ, 0);
      }

      ਜੇ (hand == 0) {
	    ਜੇ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_handle_by_name: "
			  "Scope does not exist. Giving up.\n");
	    }

	    ਵਾਪਸ 0;
      }

	// If there is a path part, then use it to find the
	// scope. For example, if the full name is a.b.c, then
	// the nm_path string is a.b and we search for that
	// scope. If we find it, then set hand to that scope.
      ਜੇ (nm_ਰਸਤਾ) {
	    vpiHandle tmp = ਲੱਭੋ_scope(nm_ਰਸਤਾ, hand, 0);
	    ਜਦੋਂ (tmp == 0 && hand != 0) {
		  hand = vpi_handle(vpiScope, hand);
		  tmp = ਲੱਭੋ_scope(nm_ਰਸਤਾ, hand, 0);
	    }
	    hand = tmp;
      }

	// find_name() expects escaped identifiers to be stripped
      ਜੇ (*nm_ਅਧਾਰ == '\\') {
	    // Skip the \ at the beginning
	    ++nm_ਅਧਾਰ;

	    // Drop the space at the end if it exists
	    ਅੱਖਰ *ਅਗਲਾ_2;
	    ਜੇ ((ਅਗਲਾ_2 = strchr(nm_ਅਧਾਰ, ' '))) {
		  *ਅਗਲਾ_2 = 0;
	    }
      }

	// Now we have the correct scope, look for the item.
      vpiHandle out = ਲੱਭੋ_name(nm_ਅਧਾਰ, hand);

      ਜੇ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name: DONE\n");
      }

      ਵਾਪਸ out;
}

// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected
// x2: Ptr to previous node
// x3: True when net is connected to output of previous node
tuple<ਤਾਰਕਿਕ, vvp_net_ptr_t, ਤਾਰਕਿਕ> check_connected_ਮੰਜ਼ਿਲ_concat8_and_part_sa(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t ਮਾਪਾ) {

      vvp_net_ptr_t prev = vvp_net_ptr_t(ਖਾਲੀ_ਸੂਚਕ, 0);

      ਜਦੋਂ (cur.ptr()) {
	    ਜੇ (cur.ptr() == net2 && dynamic_cast<vvp_fun_part_sa*>(ਮਾਪਾ.ptr()->fun)) {
		    // Pass on the return value if found
		  ਵਾਪਸ {ਸੱਚ, prev.ptr() ? prev : ਮਾਪਾ, prev.ptr() == ਖਾਲੀ_ਸੂਚਕ};
	    } ਨਹੀਂਤਾਂ ਜੇ (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  ਸਵੈਚਾਲਿਤ res = check_connected_ਮੰਜ਼ਿਲ_concat8_and_part_sa(cur.ptr()->ਬਾਹਰਲਾ_2, net2, cur);
		  ਜੇ (get<0>(res)) {
			ਵਾਪਸ res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->ਪੋਰਟ[cur.ਪੋਰਟ()];
      }

	// net2 is not connected to this concat8
      ਵਾਪਸ {ਝੂਠ, vvp_net_ptr_t(ਖਾਲੀ_ਸੂਚਕ, 0), ਝੂਠ};
}

// Check if net2 is connected to current_net through a net of vvp_fun_concat8s
// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected with concats and part_sas
// x2: Ptr to previous node
// x3: True when net is connected to output of x2
tuple<ਤਾਰਕਿਕ, vvp_net_ptr_t, ਤਾਰਕਿਕ> check_connected_ਮੰਜ਼ਿਲ_concat8(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t ਮਾਪਾ)
{
      vvp_net_ptr_t prev = vvp_net_ptr_t(ਖਾਲੀ_ਸੂਚਕ, 0);
	// For everything connected
      ਜਦੋਂ (cur.ptr()) {
	      // Check if it's a concat8
	    ਜੇ (cur.ptr() == net2) {
		  ਵਾਪਸ {ਸੱਚ, vvp_net_ptr_t(ਖਾਲੀ_ਸੂਚਕ, 0), ਝੂਠ};
	    } ਨਹੀਂਤਾਂ ਜੇ (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  ਸਵੈਚਾਲਿਤ res = check_connected_ਮੰਜ਼ਿਲ_concat8(cur.ptr()->ਬਾਹਰਲਾ_2, net2, cur);
		  ਜੇ (get<0>(res)) {
			ਜੇ (!get<1>(res).ptr() && dynamic_cast<vvp_fun_part_sa*>(ਮਾਪਾ.ptr()->fun)) {
			      // `parent` is the last part_sa node in front of our destination
			      // We want to place the intermod node here
			      ਵਾਪਸ {ਸੱਚ, prev.ptr() ? prev : ਮਾਪਾ, prev.ptr() == ਖਾਲੀ_ਸੂਚਕ};
			}
			ਵਾਪਸ res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->ਪੋਰਟ[cur.ਪੋਰਟ()];
      }

	// net2 is not connected to this concat8
      ਵਾਪਸ {ਝੂਠ, vvp_net_ptr_t(ਖਾਲੀ_ਸੂਚਕ, 0), ਝੂਠ};
}

// Used to get intermodpath for two ports
vpiHandle vpi_handle_multi(PLI_INT32 type,
                           vpiHandle ref1,
                           vpiHandle ref2)
{
      ਜੇ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_multi(%d, %p, %p) -->\n",
	            type, ref1, ref2);
      }

      ਜੇ (type != vpi_Inter_Mod_ਰਸਤਾ) {
	    fprintf(stderr, "sorry: vpi_handle_multi currently supports"
	            "only vpiInterModPath\n");
	    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
      }

	// Indicates whether port1 refers to a single bit
      ਤਾਰਕਿਕ port1_has_ਸੂਚਕਾਂਕ = ਝੂਠ;
      ਪੂਰਨ port1_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ = 0;
      vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info* port1_ਦੋਆਧਾਰੀ_ਅੰਕ = dynamic_cast<vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info*>(ref1);

      ਜੇ (port1_ਦੋਆਧਾਰੀ_ਅੰਕ) {
	      // Get the bit index
	    port1_has_ਸੂਚਕਾਂਕ = ਸੱਚ;
	    port1_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ = vpi_get(vpi_ਦੋਆਧਾਰੀ_ਅੰਕ, port1_ਦੋਆਧਾਰੀ_ਅੰਕ);
	      // Update the ref1 to point to the base port
	    ref1 = vpi_handle(vpi_ਮਾਪਾ, port1_ਦੋਆਧਾਰੀ_ਅੰਕ);
      }

	// Indicates whether port2 refers to a single bit
      ਤਾਰਕਿਕ port2_has_ਸੂਚਕਾਂਕ = ਝੂਠ;
      ਪੂਰਨ port2_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ = 0;
      vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info* port2_ਦੋਆਧਾਰੀ_ਅੰਕ = dynamic_cast<vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info*>(ref2);

      ਜੇ (port2_ਦੋਆਧਾਰੀ_ਅੰਕ) {
	      // Get the bit index
	    port2_has_ਸੂਚਕਾਂਕ = ਸੱਚ;
	    port2_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ = vpi_get(vpi_ਦੋਆਧਾਰੀ_ਅੰਕ, port2_ਦੋਆਧਾਰੀ_ਅੰਕ);
	      // Update the ref1 to point to the base port
	    ref2 = vpi_handle(vpi_ਮਾਪਾ, port2_ਦੋਆਧਾਰੀ_ਅੰਕ);
      }

      vpi_ਪੋਰਟ_Info* port1 = dynamic_cast<vpi_ਪੋਰਟ_Info*>(ref1);

      ਜੇ (!port1) {
	    fprintf(stderr, "sorry: second argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
      }

      vpi_ਪੋਰਟ_Info* port2 = dynamic_cast<vpi_ਪੋਰਟ_Info*>(ref2);

      ਜੇ (!port2) {
	    fprintf(stderr, "sorry: third argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
      }

	// Get the names of both ports
      std::string port1_name(vpi_get_str(vpiName, ref1));
      std::string port2_name(vpi_get_str(vpiName, ref2));

	// If both ports are vpiOutput, we have to reassign the __vpiSignal from port1
	// to port2 because otherwise the non-delayed version of the signal is dumped
	// even tho the intermodpath is correctly inserted
      __vpiSignal* output_signal = ਖਾਲੀ_ਸੂਚਕ;

      ਜੇ (port1->get_direction() == vpiOutput && port2->get_direction() == vpiOutput) {
	    vpiHandle scope_port2 = vpi_handle(vpiScope, ref2);
	    ਪ੍ਰਤਿਜ੍ਞਾ(scope_port2);

	      // Iterate over nets in the scope of port2
	    vpiHandle net_i = vpi_iterate(vpiNet, scope_port2) ;
	    vpiHandle net;

	    ਜਦੋਂ ((net = vpi_ਜਾਂਚ(net_i)) != ਨਿਰਾ) {
		  std::string net_name(vpi_get_str(vpiName, net));

		    // Compare whether the net matches with the port name
		  ਜੇ (net_name == port2_name) {
			output_signal = dynamic_cast<__vpiSignal*>(net);
		  }
	    }
      }

      vvp_net_t* net1 = port1->get_ਪੋਰਟ();
      vvp_net_t* net2 = port2->get_ਪੋਰਟ();

      ਜੇ (net1 == ਖਾਲੀ_ਸੂਚਕ || net2 == ਖਾਲੀ_ਸੂਚਕ) {
	    fprintf(stderr, "Error: Could not find net. "
		    "Did you run iverilog with '-ginterconnect'?\n");
	    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
      }

      ਜੇ (net1 == net2) {
	    fprintf(stderr, "Error: Net for both ports is the same. "
		    "Did you pass the same port twice?\n");
	    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
      }

      ਜੇ (!dynamic_cast<vvp_fun_buft*>(net1->fun)) {
	    fprintf(stderr, "Error: functor of net1 must be"
		    "vvp_fun_buft\n");
	    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
      }

      ਜੇ (!dynamic_cast<vvp_fun_buft*>(net2->fun)) {
	    fprintf(stderr, "Error: functor of net2 must be"
		    "vvp_fun_buft\n");
	    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
      }

	// If port1 is actually a port bit, we have to get to the correct vvp_fun_part
	// after which we insert the intermodpath delay
      ਜੇ (port1_has_ਸੂਚਕਾਂਕ) {
	    vvp_net_ptr_t* net1_ptr = &net1->ਬਾਹਰਲਾ_2;

	      // Search for part selects connected to port1
	    vvp_net_t* current_net = net1_ptr->ptr();

	    ਜਦੋਂ (current_net) {
		  ਅਚਲ vvp_fun_part* part = dynamic_cast<vvp_fun_part*>(current_net->fun);

		    // Its a part select!
		  ਜੇ (part) {
			  // Is it the correct part select?
			ਜੇ (part->get_ਅਧਾਰ() == (ਬਿਨਾਨਿਸ਼ਾਨੀ)port1_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ) {
			      ਪ੍ਰਤਿਜ੍ਞਾ(part->get_wid() == 1);
			      net1 = current_net; // Replace net1 as this is our new start point
			      ਰੋਕੋ;
			}
		  }

		  current_net = current_net->ਪੋਰਟ[0].ptr(); // BUFT has only one input, index 0
	    }
      }

	// Iterate over all nodes connected to port1
      vvp_net_ptr_t cur = net1->ਬਾਹਰਲਾ_2;
      vvp_net_ptr_t prev = vvp_net_ptr_t(ਖਾਲੀ_ਸੂਚਕ, 0);

      ਜਦੋਂ (cur.ptr()) {
	      // Either port2 is directly connected to port1
	      // Or in the second case port2 is indirectly connected
	      // to port1 through a net of concat8s
	    ਤਾਰਕਿਕ is_connected, is_output;
	    vvp_net_ptr_t ਪਿਛਲਾ_ਗੰਢ;
	    tie(is_connected, ਪਿਛਲਾ_ਗੰਢ, is_output) = port2_has_ਸੂਚਕਾਂਕ
		  ? check_connected_ਮੰਜ਼ਿਲ_concat8(cur, net2, vvp_net_ptr_t(net1, 0))
		  : check_connected_ਮੰਜ਼ਿਲ_concat8_and_part_sa(cur, net2, vvp_net_ptr_t(net1, 0));
	    ਜੇ ( (!port2_has_ਸੂਚਕਾਂਕ && cur.ptr() == net2) || is_connected ) {
		  vvp_net_t*new_net = ਨਵਾਂ vvp_net_t;

		    // Create new node with intermodpath and connect port2 to it
		  ਪੂਰਨ width = 1; // TODO
		  vvp_fun_intermodpath*obj = ਨਵਾਂ vvp_fun_intermodpath(new_net, width);

		  vvp_net_t* output_net = net1;
		  ਜੇ (is_connected) {
			ਜੇ (is_output) {
			      output_net = ਪਿਛਲਾ_ਗੰਢ.ptr(); // net2 is direct output of part_sa
			      cur = output_net->ਬਾਹਰਲਾ_2;
			      prev = vvp_net_ptr_t(ਖਾਲੀ_ਸੂਚਕ, 0);
			} ਨਹੀਂਤਾਂ {
			      prev = ਪਿਛਲਾ_ਗੰਢ;
			      ਜੇ (prev.ptr()) {
				    cur = prev.ptr()->ਪੋਰਟ[prev.ਪੋਰਟ()];
			      }
			}
		  }

		  new_net->fun = obj;
		  new_net->ਬਾਹਰਲਾ_2 = cur;

		    // Port2 is in the middle of the list
		    // Insert intermodpath before port2 and keep everything else intact
		  ਜੇ (prev.ptr()) {
			prev.ptr()->ਪੋਰਟ[prev.ਪੋਰਟ()] = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->ਪੋਰਟ[0] = cur.ptr()->ਪੋਰਟ[cur.ਪੋਰਟ()]; // Connect the next net in list
			cur.ptr()->ਪੋਰਟ[cur.ਪੋਰਟ()] = vvp_net_ptr_t(ਖਾਲੀ_ਸੂਚਕ, 0); // Only port2 is connected to intermodpath
		    // Port2 is first in list
		    // Insert intermodpath before port2 and keep everything else intact
		  } ਨਹੀਂਤਾਂ {
			output_net->ਬਾਹਰਲਾ_2 = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->ਪੋਰਟ[0] = cur.ptr()->ਪੋਰਟ[cur.ਪੋਰਟ()]; //  Connect the next net in list
			cur.ptr()->ਪੋਰਟ[cur.ਪੋਰਟ()] = vvp_net_ptr_t(ਖਾਲੀ_ਸੂਚਕ, 0); // Only port2 is connected to intermodpath
		  }

		    // If both ports are vpiOutput and port2 is not a vector,
		    // we have to reassign the __vpiSignal so that the delayed
		    // values get dumped
		  ਜੇ (output_signal && !port2_has_ਸੂਚਕਾਂਕ) {
			net2->fil = net1->fil;
			net1->fil = ਖਾਲੀ_ਸੂਚਕ;
			output_signal->ਗੰਢ = net2;
		  }

		    // Create the VPI intermodpath object
		  __vpiInterModPath* intermodpath = vpip_make_intermodpath(new_net, port1, port2);
		  intermodpath->intermodpath = obj;

		    // Finally done, return the intermodpath object
		  ਵਾਪਸ intermodpath;
	    }

	    prev = cur;
	    cur = cur.ptr()->ਪੋਰਟ[cur.ਪੋਰਟ()]; // Next net in linked list
      }

      fprintf(stderr, "VPI error: Could not insert intermodpath!\n");
      fprintf(stderr, "\tport1 = %s, port1_has_index = %d, port1_bit_index = %d\n", port1_name.c_str(), port1_has_ਸੂਚਕਾਂਕ, port1_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ);
      fprintf(stderr, "\tport2 = %s, port2_has_index = %d, port2_bit_index = %d\n", port2_name.c_str(), port2_has_ਸੂਚਕਾਂਕ, port2_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ);
      ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
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



ਖਾਲੀ vpi_get_delays(vpiHandle ਵਿਅੰਜਕ, p_vpi_delay delays)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿਅੰਜਕ);
      ਪ੍ਰਤਿਜ੍ਞਾ(delays);

      ਵਿਅੰਜਕ->vpi_get_delays(delays);

      ਜੇ (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_get_delays(%p, %p) -->\n", ਵਿਅੰਜਕ, delays);
      }
}


ਖਾਲੀ vpi_put_delays(vpiHandle ਵਿਅੰਜਕ, p_vpi_delay delays)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿਅੰਜਕ  );
      ਪ੍ਰਤਿਜ੍ਞਾ(delays );

      ਵਿਅੰਜਕ->vpi_put_delays(delays);

      ਜੇ (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_put_delays(%p, %p) -->\n", ਵਿਅੰਜਕ, delays);
      }
}








ਬਾਹਰੀ "C" PLI_INT32 vpi_vprintf(ਅਚਲ ਅੱਖਰ*fmt, va_list ap)
{
      ਵਾਪਸ vpi_mcd_vprintf(1, fmt, ap);
}

ਬਾਹਰੀ "C" PLI_INT32 vpi_printf(ਅਚਲ ਅੱਖਰ *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      ਪੂਰਨ r = vpi_mcd_vprintf(1, fmt, ap);
      va_end(ap);
      ਵਾਪਸ r;
}

ਬਾਹਰੀ "C" PLI_INT32 vpi_flush(ਖਾਲੀ)
{
      ਵਾਪਸ vpi_mcd_flush(1);
}


ਬਾਹਰੀ "C" PLI_INT32 vpi_sim_vcontrol(ਪੂਰਨ operation, va_list ap)
{
      ਲੰਮਾ diag_ਸੁਨੇਹਾ;

      ਚੋਣ (operation) {
	  ਹਾਲਤ vpi_ਅੰਤ:
	  ਹਾਲਤ __ivl_legacy_vpiFinish:
            diag_ਸੁਨੇਹਾ = va_arg(ap, PLI_INT32);
	    ਸਮਾਂ_ਸਾਰਣੀ_ਅੰਤ(diag_ਸੁਨੇਹਾ);
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiStop:
	  ਹਾਲਤ __ivl_legacy_vpiStop:
            diag_ਸੁਨੇਹਾ = va_arg(ap, PLI_INT32);
	    ਸਮਾਂ_ਸਾਰਣੀ_stop(diag_ਸੁਨੇਹਾ);
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "Unsupported VPI control operation %d.\n", operation);
	    ਵਾਪਸ 0;
      }
      ਵਾਪਸ 1;
}

ਬਾਹਰੀ "C" PLI_INT32 vpi_sim_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      ਪੂਰਨ r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      ਵਾਪਸ r;
}

ਬਾਹਰੀ "C" PLI_INT32 vpi_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      ਪੂਰਨ r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      ਵਾਪਸ r;
}

/*
 * This routine calculated the return value for $clog2.
 * It is easier to do it here vs trying to to use the VPI interface.
 */
ਬਾਹਰੀ "C" s_vpi_vecval vpip_calc_clog2(vpiHandle arg)
{
      s_vpi_vecval rtn;
      s_vpi_ਮੁੱਲ val;
      vvp_vector4_t vec4;
      ਤਾਰਕਿਕ is_neg = ਝੂਠ;  // At this point only a real can be negative.

	/* Get the value as a vvp_vector4_t. */
      val.format = vpiObjTypeVal;
      vpi_get_ਮੁੱਲ(arg, &val);
      ਜੇ (val.format == vpiRealVal) {
	    vpi_get_ਮੁੱਲ(arg, &val);
	      /* All double values can be represented in 1024 bits. */
	    vec4 = vvp_vector4_t(1024, val.value.real);
	    ਜੇ (val.value.real < 0) is_neg = ਸੱਚ;
      } ਨਹੀਂਤਾਂ {
	    val.format = vpi_ਸਦਿਸ਼_Val;
	    vpi_get_ਮੁੱਲ(arg, &val);
	    ਬਿਨਾਨਿਸ਼ਾਨੀ wid = vpi_get(vpi_ਮਾਪ, arg);
	    vec4 = vvp_vector4_t(wid, BIT4_0);
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx=0; idx < wid; idx += 1) {
		PLI_INT32 aval = val.value.vector[idx/32].aval;
		PLI_INT32 bval = val.value.vector[idx/32].bval;
		aval >>= idx % 32;
		bval >>= idx % 32;
		ਪੂਰਨ bitmask = (aval&1) | ((bval<<1)&2);
		vvp_bit4_t ਦੋਆਧਾਰੀ_ਅੰਕ = scalar_ਮੰਜ਼ਿਲ_bit4(bitmask);
		vec4.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, ਦੋਆਧਾਰੀ_ਅੰਕ);
	    }
      }

      ਜੇ (vec4.has_xz()) {
	    rtn.aval = rtn.bval = 0xFFFFFFFFU;  /* Set to 'bx. */
	    ਵਾਪਸ rtn;
      }

      vvp_vector2_t vec2(vec4);

      ਜੇ (is_neg) vec2.trim_neg(); /* This is a special trim! */
      ਨਹੀਂਤਾਂ vec2.trim(); /* This makes less work shifting. */

	/* Calculate the clog2 result. */
      PLI_INT32 res = 0;
      ਜੇ (!vec2.is_zero()) {
	    vec2 -= vvp_vector2_t(1, vec2.size());
	    ਜਦੋਂ(!vec2.is_zero()) {
		  res += 1;
		  vec2 >>= 1;
	    }
      }

      rtn.aval = res;
      rtn.bval = 0;
      ਵਾਪਸ rtn;
}

/*
 * This routine provides the information needed to implement $countdrivers.
 * It is done here for performance reasons - interrogating the drivers
 * individually via the VPI interface would be much slower.
 */
ਬਾਹਰੀ "C" ਖਾਲੀ vpip_ਗਿਣਤੀ_drivers(vpiHandle ref, ਬਿਨਾਨਿਸ਼ਾਨੀ idx,
                                   ਬਿਨਾਨਿਸ਼ਾਨੀ counts[4])
{
      ਬਣਤਰ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);
      rfp->ਗੰਢ->ਗਿਣਤੀ_drivers(idx, counts);
}

#ਜੇ defined(__MINGW32__) || defined (__CYGWIN__)
vpip_routines_s vpi_routines = {
    .ਰਜਿਸਟਰ_cb                = vpi_ਰਜਿਸਟਰ_cb,
    .remove_cb                  = vpi_remove_cb,
    .ਰਜਿਸਟਰ_systf             = vpi_ਰਜਿਸਟਰ_systf,
    .get_systf_info             = vpi_get_systf_info,
    .handle_by_name             = vpi_handle_by_name,
    .handle_by_ਸੂਚਕਾਂਕ            = vpi_handle_by_ਸੂਚਕਾਂਕ,
    .handle_multi               = vpi_handle_multi,
    .handle                     = vpi_handle,
    .iterate                    = vpi_iterate,
    .ਜਾਂਚ                       = vpi_ਜਾਂਚ,
    .get                        = vpi_get,
    .get64                      = vpi_get64,
    .get_str                    = vpi_get_str,
    .get_delays                 = vpi_get_delays,
    .put_delays                 = vpi_put_delays,
    .get_ਮੁੱਲ                  = vpi_get_ਮੁੱਲ,
    .put_ਮੁੱਲ                  = vpi_put_ਮੁੱਲ,
    .get_ਸਮਾਂ                   = vpi_get_ਸਮਾਂ,
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
    .ਗਿਣਤੀ_drivers              = vpip_ਗਿਣਤੀ_drivers,
    .format_strength            = vpip_format_strength,
    .format_pretty              = vpip_format_pretty,
    .make_systf_system_defined  = vpip_make_systf_system_defined,
    .mcd_rawwrite               = vpip_mcd_rawwrite,
    .ਸੈੱਟ_return_ਮੁੱਲ           = vpip_ਸੈੱਟ_return_ਮੁੱਲ,
};
#ਜੇ_ਅੰਤ
