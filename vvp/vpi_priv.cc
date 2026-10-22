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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <vector>
# include  <cstdio>
# include  <cstdarg>
# include  <cstring>
# include  <cassert>
# include  <cstdlib>
# include  <cmath>
# include  <iostream>

ಬಳಸಿ ನಾಮಾಕಾಶ std;
vpi_ಬಹುಲಕ_t vpi_ಬಹುಲಕ_flag = VPI_ಬಹುಲಕ_NONE;
FILE*vpi_trace = 0;

ಸ್ಥಿರ s_vpi_vlog_info  vpi_vlog_info;
ಸ್ಥಿರ s_vpi_error_info vpip_ಕೊನೆಯ_error = { 0, 0, 0, 0, 0, 0, 0 };

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* direction_as_string(ಪೂರ್ಣಾಂಕ dir)
{
      ಆಯ್ಕೆ (dir) {
	  ಸ್ಥಿತಿ vpi_ಒಳಹರಿವು:
	    ಹಿಂತಿರುಗಿ "input";
	  ಸ್ಥಿತಿ vpiOutput:
	    ಹಿಂತಿರುಗಿ "output";
	  ಸ್ಥಿತಿ vpiInout:
	    ಹಿಂತಿರುಗಿ "inout";
	  ಸ್ಥಿತಿ vpiMixedIO:
	    ಹಿಂತಿರುಗಿ "mixed io";
	  ಸ್ಥಿತಿ vpiNoDirection:
	    ಹಿಂತಿರುಗಿ "no direction";
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ "INVALID-DIRECTION";
      }
}

__vpiHandle::~__vpiHandle()
{ }

ಪೂರ್ಣಾಂಕ __vpiHandle::vpi_get(ಪೂರ್ಣಾಂಕ)
{ ಹಿಂತಿರುಗಿ vpiUndefined; }

int64_t __vpiHandle::vpi_get64(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ (code == _vpiNexusId) ? 0 : vpiUndefined; }

ಅಕ್ಷರ* __vpiHandle::vpi_get_str(ಪೂರ್ಣಾಂಕ)
{ ಹಿಂತಿರುಗಿ 0; }

ಶೂನ್ಯ __vpiHandle::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ)
{ }

vpiHandle __vpiHandle::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ)
{ ಹಿಂತಿರುಗಿ 0; }

vpiHandle __vpiHandle::vpi_handle(ಪೂರ್ಣಾಂಕ)
{ ಹಿಂತಿರುಗಿ 0; }

vpiHandle __vpiHandle::vpi_iterate(ಪೂರ್ಣಾಂಕ)
{ ಹಿಂತಿರುಗಿ 0; }

vpiHandle __vpiHandle::vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ)
{ ಹಿಂತಿರುಗಿ 0; }

ಶೂನ್ಯ __vpiHandle::vpi_get_delays(p_vpi_delay)
{ }

ಶೂನ್ಯ __vpiHandle::vpi_put_delays(p_vpi_delay)
{ }

__vpiBaseVar::__vpiBaseVar(__vpiScope*scope, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net)
: scope_(scope), name_(name), net_(net)
{
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
__vpiBaseVar::~__vpiBaseVar()
{
      vvp_net_delete(net_);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The default behavior for the vpi_free_object to an object is to
 * suppress the actual operation. This is because handles are
 * generally allocated semi-permanently within vvp context. Dynamic
 * objects will override the free_object_fun method to return an
 * appropriately effective function.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ suppress_free(vpiHandle)
{ ಹಿಂತಿರುಗಿ 1; }
__vpiHandle::free_object_fun_t __vpiHandle::free_object_fun(ಶೂನ್ಯ)
{ ಹಿಂತಿರುಗಿ &suppress_free; }

/*
 * The vpip_string function creates a constant string from the pass
 * input. This constant string is permanently allocated from an
 * efficient string buffer store.
 */
ರಚನೆ vpip_string_chunk {
      ರಚನೆ vpip_string_chunk*ಮುಂದಿನ_2;
      ಅಕ್ಷರ ದತ್ತಾಂಶ[64*1024 - ಗಾತ್ರ (ರಚನೆ vpip_string_chunk*)];
};

ಚಿಹ್ನೆರಹಿತ vpip_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ __vpiSignal *sig)
{
      ಹಿಂತಿರುಗಿ ನಿರಪೇಕ್ಷ(sig->msb.get_ಮೌಲ್ಯ() - sig->lsb.get_ಮೌಲ್ಯ()) + 1;
}

__vpiScope* vpip_scope(__vpiSignal*sig)
{
      ಆದರೆ (sig->is_netarray)
	    ಹಿಂತಿರುಗಿ static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.ಪೋಷಕ));
      ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ sig->within.scope;
}

__vpiScope* vpip_scope(__vpiRealVar*sig)
{
      ಆದರೆ (sig->is_netarray)
	    ಹಿಂತಿರುಗಿ static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.ಪೋಷಕ));
      ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ sig->within.scope;
}

vpiHandle vpip_module(__vpiScope*scope)
{
      ಆಗ(scope && scope->get_type_ಸಂಕೇತ() != vpiModule) {
	    scope = scope->scope;
      }
      ಪ್ರತಿಜ್ಞಾ(scope);
      ಹಿಂತಿರುಗಿ scope;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *vpip_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str)
{
      ಸ್ಥಿರ vpip_string_chunk ಮೊದಲ_chunk = {0, {0}};
      ಸ್ಥಿರ vpip_string_chunk*chunk_list = &ಮೊದಲ_chunk;
      ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ chunk_fill = 0;

      ಚಿಹ್ನೆರಹಿತ len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str);
      ಪ್ರತಿಜ್ಞಾ( (len+1) <= ಗಾತ್ರ chunk_list->ದತ್ತಾಂಶ );

      ಆದರೆ ( (len+1) > (ಗಾತ್ರ chunk_list->ದತ್ತಾಂಶ - chunk_fill) ) {
	    vpip_string_chunk*tmp = ಹೊಸ vpip_string_chunk;
	    tmp->ಮುಂದಿನ_2 = chunk_list;
	    chunk_list = tmp;
	    chunk_fill = 0;
      }

      ಅಕ್ಷರ*res = chunk_list->ದತ್ತಾಂಶ + chunk_fill;
      chunk_fill += len + 1;

      strcpy(res, str);
      ಹಿಂತಿರುಗಿ res;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ hash_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಪಠ್ಯ)
{
      ಚಿಹ್ನೆರಹಿತ h = 0;

      ಆಗ (*ಪಠ್ಯ) {
	    h = (h << 4) ^ (h >> 28) ^ (ಚಿಹ್ನೆರಹಿತ)*ಪಠ್ಯ;
	    ಪಠ್ಯ += 1;
      }
      ಹಿಂತಿರುಗಿ h;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* vpip_name_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಪಠ್ಯ)
{
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ HASH_ಗಾತ್ರ = 4096;
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*hash_ಕೋಷ್ಟಕ[HASH_ಗಾತ್ರ] = {0};

      ಚಿಹ್ನೆರಹಿತ hash_ಮೌಲ್ಯ = hash_string(ಪಠ್ಯ) % HASH_ಗಾತ್ರ;

	/* If we easily find the string in the hash table, then return
	   that and be done. */
      ಆದರೆ (hash_ಕೋಷ್ಟಕ[hash_ಮೌಲ್ಯ]
	  && (strcmp(hash_ಕೋಷ್ಟಕ[hash_ಮೌಲ್ಯ], ಪಠ್ಯ) == 0)) {
	    ಹಿಂತಿರುಗಿ hash_ಕೋಷ್ಟಕ[hash_ಮೌಲ್ಯ];
      }

	/* The existing hash entry is not a match. Replace it with the
	   newly allocated value, and return the new pointer as the
	   result to the add. */
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*res = vpip_string(ಪಠ್ಯ);
      hash_ಕೋಷ್ಟಕ[hash_ಮೌಲ್ಯ] = res;

      ಹಿಂತಿರುಗಿ res;
}

PLI_INT32 vpi_chk_error(p_vpi_error_info info)
{
      ಆದರೆ (vpip_ಕೊನೆಯ_error.state == 0)
	    ಹಿಂತಿರುಗಿ 0;

      info->state = vpip_ಕೊನೆಯ_error.state;
      info->level = vpip_ಕೊನೆಯ_error.level;
      info->message = vpip_ಕೊನೆಯ_error.message;
      info->ಗುಣಲಬ್ಧ = vpi_vlog_info.ಗುಣಲಬ್ಧ;
      info->code = (ಅಕ್ಷರ *)"";
      info->file = 0;
      info->ಸಾಲು = 0;

      ಹಿಂತಿರುಗಿ info->level;
}

PLI_INT32 vpi_compare_objects(vpiHandle obj1, vpiHandle obj2)
{
      ಪ್ರತಿಜ್ಞಾ(obj1);
      ಪ್ರತಿಜ್ಞಾ(obj2);

	// Does this actually work for all cases?
      ಆದರೆ (obj1 != obj2) ಹಿಂತಿರುಗಿ 0;
      ಇಲ್ಲದಿದ್ದರೆ ಹಿಂತಿರುಗಿ 1;
}

/*
 * Copy the internal information to the data structure. Do not free or
 * change the tfname/user_data since they are a pointer to the real
 * string/data values. We also support passing a task or function handle
 * instead of just a handle to a vpiUserSystf.
 */
ಶೂನ್ಯ vpi_get_systf_info(vpiHandle ref, p_vpi_systf_ದತ್ತಾಂಶ ದತ್ತಾಂಶ)
{
      ರಚನೆ __vpiUserSystf* rfp = dynamic_cast<__vpiUserSystf*>(ref);
      ಆದರೆ (rfp == 0) {
	    ರಚನೆ __vpiSysTaskCall*call = dynamic_cast<__vpiSysTaskCall*>(ref);
	    ಪ್ರತಿಜ್ಞಾ(call);
	    rfp = call->defn;
      }

	/* Assert that vpiUserDefn is true! */
      ಪ್ರತಿಜ್ಞಾ(rfp->is_user_defn);

      ದತ್ತಾಂಶ->type = rfp->info.type;
      ದತ್ತಾಂಶ->sysfunctype = rfp->info.sysfunctype;
      ದತ್ತಾಂಶ->tfname = rfp->info.tfname;
      ದತ್ತಾಂಶ->calltf = rfp->info.calltf;
      ದತ್ತಾಂಶ->compiletf = rfp->info.compiletf;
      ದತ್ತಾಂಶ->sizetf = rfp->info.sizetf;
      ದತ್ತಾಂಶ->user_ದತ್ತಾಂಶ = rfp->info.user_ದತ್ತಾಂಶ;
}

/*
 * When a task is called, this value is set so that vpi_handle can
 * fathom the vpi_handle(vpiSysTfCall,0) function.
 */
ರಚನೆ __vpiSysTaskCall*vpip_cur_task = 0;

PLI_INT32 vpi_free_object(vpiHandle ref)
{
      ಪೂರ್ಣಾಂಕ rtn;

      ಆದರೆ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_free_object(%p)", ref);
	    fflush(vpi_trace);
      }

      ಪ್ರತಿಜ್ಞಾ(ref);
      __vpiHandle::free_object_fun_t fun = ref->free_object_fun();
      rtn = fun (ref);

      ಆದರೆ (vpi_trace)
	    fprintf(vpi_trace, " --> %d\n", rtn);

      ಹಿಂತಿರುಗಿ rtn;
}

PLI_INT32 vpi_release_handle(vpiHandle ref)
{
	// Since SystemVerilog vpi_free_object() has been
	// renamed vpi_release_handle(), and thus
	// vpi_free_object() has been deprecated.
      ಹಿಂತಿರುಗಿ vpi_free_object(ref);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vpip_get_global(ಪೂರ್ಣಾಂಕ property)
{
      ಆಯ್ಕೆ (property) {

	  ಸ್ಥಿತಿ vpi_ಸಮಯ_Unit:
	  ಸ್ಥಿತಿ vpi_ಸಮಯ_Precision:
	    ಹಿಂತಿರುಗಿ vpip_get_ಸಮಯ_precision();

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vpi error: bad global property: %d\n", property);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ vpiUndefined;
      }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* vpi_property_str(PLI_INT32 code)
{
      ಸ್ಥಿರ ಅಕ್ಷರ buf[32];
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiConstType:
	    ಹಿಂತಿರುಗಿ "vpiConstType";
	  ಸ್ಥಿತಿ vpiName:
	    ಹಿಂತಿರುಗಿ "vpiName";
	  ಸ್ಥಿತಿ vpi_ತುಂಬಿದ_Name:
	    ಹಿಂತಿರುಗಿ "vpiFullName";
	  ಸ್ಥಿತಿ vpi_ಸಮಯ_Unit:
	    ಹಿಂತಿರುಗಿ "vpiTimeUnit";
	  ಸ್ಥಿತಿ vpi_ಸಮಯ_Precision:
	    ಹಿಂತಿರುಗಿ "vpiTimePrecision";
          ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ "vpiSize";
          ಸ್ಥಿತಿ _vpiNexusId:
	    ಹಿಂತಿರುಗಿ "_vpiNexusId";
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    snprintf(buf, ಗಾತ್ರ(buf), "%d", (ಪೂರ್ಣಾಂಕ)code);
      }
      ಹಿಂತಿರುಗಿ buf;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* vpi_type_as_string(PLI_INT32 code)
{
      ಸ್ಥಿರ ಅಕ್ಷರ buf[32];
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiArrayType:
	    ಹಿಂತಿರುಗಿ "vpiArrayType";
	  ಸ್ಥಿತಿ vpi_ಬಿಟ್_Var:
	    ಹಿಂತಿರುಗಿ "vpiBitVar";
	  ಸ್ಥಿತಿ vpiByteVar:
	    ಹಿಂತಿರುಗಿ "vpiByteVar";
	  ಸ್ಥಿತಿ vpiClassVar:
	    ಹಿಂತಿರುಗಿ "vpiClassVar";
	  ಸ್ಥಿತಿ vpiConstant:
	    ಹಿಂತಿರುಗಿ "vpiConstant";
	  ಸ್ಥಿತಿ vpiEnumTypespec:
	    ಹಿಂತಿರುಗಿ "vpiEnumTypespec";
	  ಸ್ಥಿತಿ vpiFunction:
	    ಹಿಂತಿರುಗಿ "vpiFunction";
	  ಸ್ಥಿತಿ vpiGenScope:
	    ಹಿಂತಿರುಗಿ "vpiGenScope";
	  ಸ್ಥಿತಿ vpiIntVar:
	    ಹಿಂತಿರುಗಿ "vpiIntVar";
	  ಸ್ಥಿತಿ vpiIntegerVar:
	    ಹಿಂತಿರುಗಿ "vpiIntegerVar";
	  ಸ್ಥಿತಿ vpiIterator:
	    ಹಿಂತಿರುಗಿ "vpiIterator";
	  ಸ್ಥಿತಿ vpiLongIntVar:
	    ಹಿಂತಿರುಗಿ "vpiLongIntVar";
	  ಸ್ಥಿತಿ vpi_ಸ್ಮರಣೆ:
	    ಹಿಂತಿರುಗಿ "vpiMemory";
	  ಸ್ಥಿತಿ vpi_ಸ್ಮರಣೆ_ಶಬ್ದ:
	    ಹಿಂತಿರುಗಿ "vpiMemoryWord";
	  ಸ್ಥಿತಿ vpiModule:
	    ಹಿಂತಿರುಗಿ "vpiModule";
	  ಸ್ಥಿತಿ vpiNamedBegin:
	    ಹಿಂತಿರುಗಿ "vpiNamedBegin";
	  ಸ್ಥಿತಿ vpiNamedEvent:
	    ಹಿಂತಿರುಗಿ "vpiNamedEvent";
	  ಸ್ಥಿತಿ vpiNamedFork:
	    ಹಿಂತಿರುಗಿ "vpiNamedFork";
	  ಸ್ಥಿತಿ vpiPackage:
	    ಹಿಂತಿರುಗಿ "vpiPackage";
	  ಸ್ಥಿತಿ vpi_ಮಾರ್ಗ_ಪದ:
	    ಹಿಂತಿರುಗಿ "vpiPathTerm";
	  ಸ್ಥಿತಿ vpi_ಪೋರ್ಟ್:
	    ಹಿಂತಿರುಗಿ "vpiPort";
	  ಸ್ಥಿತಿ vpiNet:
	    ಹಿಂತಿರುಗಿ "vpiNet";
	  ಸ್ಥಿತಿ vpiNetArray:
	    ಹಿಂತಿರುಗಿ "vpiNetArray";
	  ಸ್ಥಿತಿ vpi_Net_ಬಿಟ್:
	    ಹಿಂತಿರುಗಿ "vpiNetBit";
	  ಸ್ಥಿತಿ vpiParameter:
	    ಹಿಂತಿರುಗಿ "vpiParameter";
	  ಸ್ಥಿತಿ vpiPartSelect:
	    ಹಿಂತಿರುಗಿ "vpiPartSelect";
	  ಸ್ಥಿತಿ vpiRealVar:
	    ಹಿಂತಿರುಗಿ "vpiRealVar";
	  ಸ್ಥಿತಿ vpiReg:
	    ಹಿಂತಿರುಗಿ "vpiReg";
	  ಸ್ಥಿತಿ vpi_Reg_ಬಿಟ್:
	    ಹಿಂತಿರುಗಿ "vpiRegBit";
	  ಸ್ಥಿತಿ vpiShortIntVar:
	    ಹಿಂತಿರುಗಿ "vpiShortIntVar";
	  ಸ್ಥಿತಿ vpiStringVar:
	    ಹಿಂತಿರುಗಿ "vpiStringVar";
	  ಸ್ಥಿತಿ vpiSysFuncCall:
	    ಹಿಂತಿರುಗಿ "vpiSysFuncCall";
	  ಸ್ಥಿತಿ vpiSysTaskCall:
	    ಹಿಂತಿರುಗಿ "vpiSysTaskCall";
	  ಸ್ಥಿತಿ vpiTask:
	    ಹಿಂತಿರುಗಿ "vpiTask";
	  ಸ್ಥಿತಿ vpi_ಸಮಯ_Var:
	    ಹಿಂತಿರುಗಿ "vpiTimeVar";
	  ಸ್ಥಿತಿ vpiUserSystf:
	    ಹಿಂತಿರುಗಿ "vpiUserSystf";
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    snprintf(buf, ಗಾತ್ರ(buf), "%d", (ಪೂರ್ಣಾಂಕ)code);
      }
      ಹಿಂತಿರುಗಿ buf;
}

PLI_INT32 vpi_get(ಪೂರ್ಣಾಂಕ property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      ಆದರೆ (property == _vpiDelaySelection) ಹಿಂತಿರುಗಿ vpip_delay_selection;

      ಆದರೆ (ref == 0)
	    ಹಿಂತಿರುಗಿ vpip_get_global(property);

      ಆದರೆ (property == vpiType) {
	    ಆದರೆ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_ಸಂಕೇತ()));
	    }

	    ಆದರೆ (ref->get_type_ಸಂಕೇತ() == vpi_ಸ್ಮರಣೆ && is_net_array(ref))
		  ಹಿಂತಿರುಗಿ vpiNetArray;
	    ಇಲ್ಲದಿದ್ದರೆ
		  ಹಿಂತಿರುಗಿ ref->get_type_ಸಂಕೇತ();
      }

      ಪೂರ್ಣಾಂಕ res = ref->vpi_get(property);

      ಆದರೆ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get(%s, %p) --> %d\n",
		    vpi_property_str(property), ref, res);
      }

      ಹಿಂತಿರುಗಿ res;
}

PLI_INT64 vpi_get64(ಪೂರ್ಣಾಂಕ property, vpiHandle ref)
{
	// For now we only support getting the nexus id.
      ಆದರೆ ((ref == 0) || (property != _vpiNexusId)) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) is not currently supported.\n",
		    vpi_property_str(property), ref);
	    ಹಿಂತಿರುಗಿ vpiUndefined;
      }

      int64_t res = ref->vpi_get64(property);

      ಆದರೆ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) --> %" PRId64 "\n",
		    vpi_property_str(property), ref, res);
      }

      ಹಿಂತಿರುಗಿ res;
}

ಅಕ್ಷರ* vpi_get_str(PLI_INT32 property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      ಆದರೆ (property == _vpiDelaySelection) {
	    ಆಯ್ಕೆ (vpip_delay_selection) {
		ಸ್ಥಿತಿ _vpiDelaySelMinimum:
		  ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str("MINIMUM");
		ಸ್ಥಿತಿ _vpiDelaySelTypical:
		  ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str("TYPICAL");
		ಸ್ಥಿತಿ vpi_Delay_Sel_ಗರಿಷ್ಠ:
		  ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str("MAXIMUM");
		ಪೂರ್ವನಿಯೋಜಿತ:
		  ಪ್ರತಿಜ್ಞಾ(0);
	    }
      }

      ಆದರೆ (ref == 0) {
	    fprintf(stderr, "vpi error: vpi_get_str(%s, 0) called "
		    "with null vpiHandle.\n", vpi_property_str(property));
	    ಹಿಂತಿರುಗಿ 0;
      }

      ಆದರೆ (property == vpiType) {
	    ಆದರೆ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_ಸಂಕೇತ()));
	    }

            PLI_INT32 type;
	    ಆದರೆ (ref->get_type_ಸಂಕೇತ() == vpi_ಸ್ಮರಣೆ && is_net_array(ref))
		  type = vpiNetArray;
	    ಇಲ್ಲದಿದ್ದರೆ
		  type = ref->get_type_ಸಂಕೇತ();
	    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ *)vpi_type_as_string(type);
      }

      ಅಕ್ಷರ*res = ref->vpi_get_str(property);

      ಆದರೆ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get_str(%s, %p) --> %s\n",
		    vpi_property_str(property), ref, res? res : "<NULL>");
      }

      ಹಿಂತಿರುಗಿ res;
}

__vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj)
{
      ಸ್ಥಿರಾಂಕ ರಚನೆ __vpiSysTaskCall*task;
      ರಚನೆ __vpiSignal*ಸಂಕೇತ;
      ರಚನೆ __vpiRealVar*real;
      ಸ್ಥಿರಾಂಕ __vpiNamedEvent*event;

      ಆಯ್ಕೆ (obj->get_type_ಸಂಕೇತ()) {
	  ಸ್ಥಿತಿ vpiSysTaskCall:
	    task = dynamic_cast<__vpiSysTaskCall*>(obj);
	    ಹಿಂತಿರುಗಿ task->scope;

	  ಸ್ಥಿತಿ vpiModule:
	    ಹಿಂತಿರುಗಿ dynamic_cast<__vpiScope*>(obj);

	  ಸ್ಥಿತಿ vpiNet:
	  ಸ್ಥಿತಿ vpiReg:
	  ಸ್ಥಿತಿ vpiIntegerVar:
	  ಸ್ಥಿತಿ vpi_ಬಿಟ್_Var:
	  ಸ್ಥಿತಿ vpiByteVar:
	  ಸ್ಥಿತಿ vpiShortIntVar:
	  ಸ್ಥಿತಿ vpiIntVar:
	  ಸ್ಥಿತಿ vpiLongIntVar:
	    ಸಂಕೇತ = dynamic_cast<__vpiSignal*>(obj);
	    ಹಿಂತಿರುಗಿ vpip_scope(ಸಂಕೇತ);

	  ಸ್ಥಿತಿ vpiRealVar:
	    real = dynamic_cast<__vpiRealVar*>(obj);
	    ಹಿಂತಿರುಗಿ vpip_scope(real);

	  ಸ್ಥಿತಿ vpiNamedEvent:
	    event = dynamic_cast<__vpiNamedEvent*>(obj);
	    ಹಿಂತಿರುಗಿ event->get_scope();

	  ಸ್ಥಿತಿ vpi_ಸ್ಮರಣೆ:
	  ಸ್ಥಿತಿ vpi_ಸ್ಮರಣೆ_ಶಬ್ದ:
	  ಸ್ಥಿತಿ vpiPartSelect:
	    ಹಿಂತಿರುಗಿ dynamic_cast<__vpiScope*>(obj->vpi_handle(vpiScope));

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "ERROR: vpip_scope_from_handle called with "
		    "object handle type=%d\n", obj->get_type_ಸಂಕೇತ());
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ 0;
      }
}

ಪೂರ್ಣಾಂಕ vpip_ಸಮಯ_units_from_handle(vpiHandle obj)
{
      ಆದರೆ (obj == 0)
	    ಹಿಂತಿರುಗಿ vpip_get_ಸಮಯ_precision();

      ಸ್ಥಿರಾಂಕ __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      ಆದರೆ (scope == 0)
	    ಹಿಂತಿರುಗಿ vpip_get_ಸಮಯ_precision();

      ಹಿಂತಿರುಗಿ scope->ಸಮಯ_units;
}

ಪೂರ್ಣಾಂಕ vpip_ಸಮಯ_precision_from_handle(vpiHandle obj)
{
      ಆದರೆ (obj == 0)
	    ಹಿಂತಿರುಗಿ vpip_get_ಸಮಯ_precision();

      ಸ್ಥಿರಾಂಕ __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      ಆದರೆ (scope == 0)
	    ಹಿಂತಿರುಗಿ vpip_get_ಸಮಯ_precision();

      ಹಿಂತಿರುಗಿ scope->ಸಮಯ_precision;
}

ದ್ವಿಗುಣ vpip_scaled_ಸಮಯ_from_handle(vvp_time64_t time, vpiHandle obj)
{
      ಪೂರ್ಣಾಂಕ scale = vpip_get_ಸಮಯ_precision() -
	          vpip_ಸಮಯ_units_from_handle(obj);
      ಆದರೆ (scale >= 0)
	    ಹಿಂತಿರುಗಿ (ದ್ವಿಗುಣ)time * ಘಾತ(10.0, scale);
      ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ (ದ್ವಿಗುಣ)time / ಘಾತ(10.0, -scale);
}

ಶೂನ್ಯ vpi_get_ಸಮಯ(vpiHandle obj, s_vpi_ಸಮಯ*vp)
{
      vvp_time64_t time;

      ಪ್ರತಿಜ್ಞಾ(vp);

      time = ವೇಳಾಪಟ್ಟಿ_simtime();

      ಆಯ್ಕೆ (vp->type) {
          ಸ್ಥಿತಿ vpi_Sim_ಸಮಯ:
	    vp->ಮೇಲಿನ = (time >> 32) & 0xffffffff;
	    vp->ಕೆಳಗಿನ = time & 0xffffffff;
	    ನಿಲ್ಲಿಸು;

          ಸ್ಥಿತಿ vpi_Scaled_Real_ಸಮಯ:
	    vp->real = vpip_scaled_ಸಮಯ_from_handle(time, obj);
	    ನಿಲ್ಲಿಸು;

          ಪೂರ್ವನಿಯೋಜಿತ:
            fprintf(stderr, "vpi_get_time: unknown type: %d\n", (ಪೂರ್ಣಾಂಕ)vp->type);
            ಪ್ರತಿಜ್ಞಾ(0);
	    ನಿಲ್ಲಿಸು;
      }
}

PLI_INT32 vpi_get_vlog_info(p_vpi_vlog_info vlog_info_p)
{
    ಆದರೆ (vlog_info_p != 0) {
	  *vlog_info_p = vpi_vlog_info;
	  ಹಿಂತಿರುಗಿ 1;
    } ಇಲ್ಲದಿದ್ದರೆ {
	  ಹಿಂತಿರುಗಿ 0;
    }
}

ಶೂನ್ಯ vpi_ಗಣ_vlog_info(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ** argv)
{
    ಸ್ಥಿರ ಅಕ್ಷರ icarus_ಗುಣಲಬ್ಧ[] = "Icarus Verilog";
    ಸ್ಥಿರ ಅಕ್ಷರ icarus_ಆವೃತ್ತಿ[] = ಆವೃತ್ತಿ;
    vpi_vlog_info.ಗುಣಲಬ್ಧ = icarus_ಗುಣಲಬ್ಧ;
    vpi_vlog_info.ಆವೃತ್ತಿ_2 = icarus_ಆವೃತ್ತಿ;
    vpi_vlog_info.argc    = argc;
    vpi_vlog_info.argv    = argv;

    ಸ್ಥಿರ ಅಕ್ಷರ trace_buf[1024];
    ಆದರೆ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಮಾರ್ಗ = getenv("VPI_TRACE"))  {
	  ಆದರೆ (!strcmp(ಮಾರ್ಗ,"-"))
		vpi_trace = stdout;
	  ಇಲ್ಲದಿದ್ದರೆ {
		vpi_trace = fopen(ಮಾರ್ಗ, "w");
		ಆದರೆ (!vpi_trace) {
		      perror(ಮಾರ್ಗ);
		      ನಿರ್ಗಮಿಸು(1);
		}
		setvbuf(vpi_trace, trace_buf, _IOLBF, ಗಾತ್ರ(trace_buf));
	  }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ vec4_get_ಮೌಲ್ಯ_string(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಶಬ್ದ_val, ಚಿಹ್ನೆರಹಿತ width,
				  s_vpi_ಮೌಲ್ಯ*vp)
{
      ಚಿಹ್ನೆರಹಿತ nchar = width / 8;
      ಚಿಹ್ನೆರಹಿತ ಬಾಲ = width % 8;

      ಅಕ್ಷರ*rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(nchar + 1, RBUF_VAL));
      ಅಕ್ಷರ*cp = rbuf;

      ಆದರೆ (ಬಾಲ > 0) {
	    ಅಕ್ಷರ char_val = 0;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = width-ಬಾಲ; idx < width ;  idx += 1) {
		  vvp_bit4_t val = ಶಬ್ದ_val.value(idx);
		  ಆದರೆ (val == BIT4_1)
			char_val |= 1 << idx;
	    }

	    ಆದರೆ (char_val != 0)
		  *cp++ = char_val;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cp != rbuf)
		  *cp++ = ' ';
      }

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < nchar ;  idx += 1) {
	    ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ = (nchar - idx - 1) * 8;
	    ಅಕ್ಷರ char_val = 0;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ bdx = 0 ;  bdx < 8 ;  bdx += 1) {
		  vvp_bit4_t val = ಶಬ್ದ_val.value(ಬಿಟ್+bdx);
		  ಆದರೆ (val == BIT4_1)
			char_val |= 1 << bdx;
	    }
	    // Ignore leading null-bytes and replace other null-bytes with space.
	    // The LRM is not entirely clear on how null bytes should be handled.
	    // This is the implementation chosen for iverilog.
	    ಆದರೆ (char_val != 0)
		  *cp++ = char_val;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cp != rbuf)
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
ಶೂನ್ಯ vpip_vec4_get_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಶಬ್ದ_val, ಚಿಹ್ನೆರಹಿತ width,
			 ತಾರ್ಕಿಕ signed_flag, s_vpi_ಮೌಲ್ಯ*vp)
{
      ಅಕ್ಷರ *rbuf = 0;

      ಆಯ್ಕೆ (vp->format) {
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector values.\n", (ಪೂರ್ಣಾಂಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);

	  ಸ್ಥಿತಿ vpiSuppressVal:
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiBinStrVal:
	    rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(width+1, RBUF_VAL));
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < width ;  idx += 1) {
		  vvp_bit4_t ಬಿಟ್ = ಶಬ್ದ_val.value(idx);
		  rbuf[width-idx-1] = vvp_bit4_ಗಮ್ಯ_ascii(ಬಿಟ್);
	    }
	    rbuf[width] = 0;
	    vp->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiOctStrVal: {
		ಚಿಹ್ನೆರಹಿತ hwid = ((width+2) / 3) + 1;
		rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(hwid, RBUF_VAL));
		vpip_vec4_ಗಮ್ಯ_oct_str(ಶಬ್ದ_val, rbuf, hwid);
		vp->value.str = rbuf;
		ನಿಲ್ಲಿಸು;
	  }

	  ಸ್ಥಿತಿ vpiDecStrVal: {
// HERE need a better estimate.
		rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(width+1, RBUF_VAL));
		vpip_vec4_ಗಮ್ಯ_dec_str(ಶಬ್ದ_val, rbuf, width+1, signed_flag);
		vp->value.str = rbuf;
		ನಿಲ್ಲಿಸು;
	  }

	  ಸ್ಥಿತಿ vpiHexStrVal: {
		ಚಿಹ್ನೆರಹಿತ  hwid = ((width + 3) / 4) + 1;
		rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(hwid, RBUF_VAL));
		vpip_vec4_ಗಮ್ಯ_hex_str(ಶಬ್ದ_val, rbuf, hwid);
		vp->value.str = rbuf;
		ನಿಲ್ಲಿಸು;
	  }

          ಸ್ಥಿತಿ vpiScalarVal: {
	        // scalars should be of size 1
	        ಪ್ರತಿಜ್ಞಾ(width == 1);
	        ಆಯ್ಕೆ(ಶಬ್ದ_val.value(0)) {
		    ಸ್ಥಿತಿ BIT4_0:
		      vp->value.scalar = vpi0;
		      ನಿಲ್ಲಿಸು;
	            ಸ್ಥಿತಿ BIT4_1:
                      vp->value.scalar = vpi1;
                      ನಿಲ್ಲಿಸು;
	            ಸ್ಥಿತಿ BIT4_X:
                      vp->value.scalar = vpiX;
                      ನಿಲ್ಲಿಸು;
	            ಸ್ಥಿತಿ BIT4_Z:
                      vp->value.scalar = vpiZ;
                      ನಿಲ್ಲಿಸು;
		}
                ನಿಲ್ಲಿಸು;
          }

	  ಸ್ಥಿತಿ vpiIntVal: {
		ದೀರ್ಘ val = 0;
		vvp_bit4_t pad = BIT4_0;
		ಆದರೆ (signed_flag && ಶಬ್ದ_val.size() > 0)
		      pad = ಶಬ್ದ_val.value(ಶಬ್ದ_val.size()-1);

		ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < 8*ಗಾತ್ರ(val) ;  idx += 1) {
		      vvp_bit4_t val4 = pad;
		      ಆದರೆ (idx < ಶಬ್ದ_val.size())
			    val4 = ಶಬ್ದ_val.value(idx);
		      ಆದರೆ (val4 == BIT4_1)
			    val |= 1L << idx;
		}

		vp->value.integer = val;
		ನಿಲ್ಲಿಸು;
	  }

	  ಸ್ಥಿತಿ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpi_ಸದಿಶ_Val;
	    // fallthrough
	  ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val: {
		ಚಿಹ್ನೆರಹಿತ hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_ಫಲಿತಾಂಶ_buf(hwid * ಗಾತ್ರ(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < width ;  idx += 1) {
		      ಆಯ್ಕೆ (ಶಬ್ದ_val.value(idx)) {
			  ಸ್ಥಿತಿ BIT4_0:
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    ನಿಲ್ಲಿಸು;
			  ಸ್ಥಿತಿ BIT4_1:
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    ನಿಲ್ಲಿಸು;
			  ಸ್ಥಿತಿ BIT4_X:
			    op->aval |= (1 << idx % 32);
			    op->bval |= (1 << idx % 32);
			    ನಿಲ್ಲಿಸು;
			  ಸ್ಥಿತಿ BIT4_Z:
			    op->aval &= ~(1 << idx % 32);
			    op->bval |=  (1 << idx % 32);
			    ನಿಲ್ಲಿಸು;
		      }
		      ಆದರೆ (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		ನಿಲ್ಲಿಸು;
	  }

	  ಸ್ಥಿತಿ vpiStringVal:
		vec4_get_ಮೌಲ್ಯ_string(ಶಬ್ದ_val, width, vp);
		ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiRealVal:
		vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಶಬ್ದ_val, vp->value.real, signed_flag);
		ನಿಲ್ಲಿಸು;
      }
}

ಶೂನ್ಯ vpip_vec2_get_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಶಬ್ದ_val, ಚಿಹ್ನೆರಹಿತ width,
			 ತಾರ್ಕಿಕ signed_flag, s_vpi_ಮೌಲ್ಯ*vp)
{
      ಆಯ್ಕೆ (vp->format) {
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector2 values.\n", (ಪೂರ್ಣಾಂಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);

	  ಸ್ಥಿತಿ vpiSuppressVal:
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiIntVal:
	    vector2_ಗಮ್ಯ_ಮೌಲ್ಯ(ಶಬ್ದ_val, vp->value.integer, signed_flag);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiObjTypeVal:
	    vp->format = vpi_ಸದಿಶ_Val;
	    // fallthrough
	  ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val: {
		ಚಿಹ್ನೆರಹಿತ hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_ಫಲಿತಾಂಶ_buf(hwid * ಗಾತ್ರ(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < width ;  idx += 1) {
		      ಆದರೆ (ಶಬ್ದ_val.value(idx)) {
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      } ಇಲ್ಲದಿದ್ದರೆ {
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      }
		      ಆದರೆ (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		ನಿಲ್ಲಿಸು;
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
      ಆದರೆ (real != real || (real && (real == 0.5*real))) {
	    ಹಿಂತಿರುಗಿ 0;
      }

      rtn = std::ಸುತ್ತು_2(real);

      ಹಿಂತಿರುಗಿ (PLI_INT32) rtn;
}

/*
 * This is a generic function to convert a double value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer.
 */
ಶೂನ್ಯ vpip_real_get_ಮೌಲ್ಯ(ದ್ವಿಗುಣ real, s_vpi_ಮೌಲ್ಯ*vp)
{
      ಅಕ್ಷರ *rbuf = 0;

      ಆಯ್ಕೆ (vp->format) {
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting real values.\n", (ಪೂರ್ಣಾಂಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);

	  ಸ್ಥಿತಿ vpiSuppressVal:
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiRealVal;
	    // fallthrough
	  ಸ್ಥಿತಿ vpiRealVal:
	    vp->value.real = real;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiIntVal:
	    vp->value.integer = get_real_as_int(real);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiDecStrVal:
	    rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(1025, RBUF_VAL));
	    vpip_vec4_ಗಮ್ಯ_dec_str(vvp_vector4_t(1024, real), rbuf, 1025, ನಿಜ);
	    vp->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;
      }
}

ದ್ವಿಗುಣ real_from_vpi_ಮೌಲ್ಯ(s_vpi_ಮೌಲ್ಯ*vp)
{
      vvp_vector4_t vec4(1024);
      ದ್ವಿಗುಣ ಫಲಿತಾಂಶ;
      ತಾರ್ಕಿಕ is_signed = ಸುಳ್ಳು;

      ಆಯ್ಕೆ (vp->format) {
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "putting real values.\n", (ಪೂರ್ಣಾಂಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);

	  ಸ್ಥಿತಿ vpiRealVal:
	    ಫಲಿತಾಂಶ = vp->value.real;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiIntVal:
	    ಫಲಿತಾಂಶ = (ದ್ವಿಗುಣ) vp->value.integer;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiBinStrVal:
	    vpip_bin_str_ಗಮ್ಯ_vec4(vec4, vp->value.str);
	    ಆದರೆ (vp->value.str[0] == '-') is_signed = ನಿಜ;
	    vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(vec4, ಫಲಿತಾಂಶ, is_signed);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiOctStrVal:
	    vpip_oct_str_ಗಮ್ಯ_vec4(vec4, vp->value.str);
	    ಆದರೆ (vp->value.str[0] == '-') is_signed = ನಿಜ;
	    vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(vec4, ಫಲಿತಾಂಶ, is_signed);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiDecStrVal:
	    vpip_dec_str_ಗಮ್ಯ_vec4(vec4, vp->value.str);
	    ಆದರೆ (vp->value.str[0] == '-') is_signed = ನಿಜ;
	    vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(vec4, ಫಲಿತಾಂಶ, is_signed);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiHexStrVal:
	    vpip_hex_str_ಗಮ್ಯ_vec4(vec4, vp->value.str);
	    ಆದರೆ (vp->value.str[0] == '-') is_signed = ನಿಜ;
	    vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(vec4, ಫಲಿತಾಂಶ, is_signed);
	    ನಿಲ್ಲಿಸು;

      }

      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ vpip_string_get_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ string&val, s_vpi_ಮೌಲ್ಯ*vp)
{
      ಅಕ್ಷರ *rbuf = 0;

      ಆಯ್ಕೆ (vp->format) {
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting string values.\n", (ಪೂರ್ಣಾಂಕ)vp->format);
	    ಪ್ರತಿಜ್ಞಾ(0);

	  ಸ್ಥಿತಿ vpiSuppressVal:
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiStringVal;
	    // fallthrough
	  ಸ್ಥಿತಿ vpiStringVal:
	    rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(val.size() + 1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;
      }
}


ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(vpiHandle ಅಭಿವ್ಯಕ್ತಿ, s_vpi_ಮೌಲ್ಯ*vp)
{
      ಪ್ರತಿಜ್ಞಾ(ಅಭಿವ್ಯಕ್ತಿ);
      ಪ್ರತಿಜ್ಞಾ(vp);

	// Never bother with suppressed values. All the derived
	// classes can ignore this type.
      ಆದರೆ (vp->format == vpiSuppressVal)
	    ಹಿಂತಿರುಗಿ;

      ಅಭಿವ್ಯಕ್ತಿ->vpi_get_ಮೌಲ್ಯ(vp);

      ಆದರೆ (vpi_trace) ಆಯ್ಕೆ (vp->format) {
		ಸ್ಥಿತಿ vpiStringVal:
		  fprintf(vpi_trace,"vpi_get_value(%p=<%d>) -> string=\"%s\"\n",
			  ಅಭಿವ್ಯಕ್ತಿ, ಅಭಿವ್ಯಕ್ತಿ->get_type_ಸಂಕೇತ(), vp->value.str);
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ vpiBinStrVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> binstr=%s\n",
			  ಅಭಿವ್ಯಕ್ತಿ->get_type_ಸಂಕೇತ(), vp->value.str);
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ vpiIntVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> int=%d\n",
			  ಅಭಿವ್ಯಕ್ತಿ->get_type_ಸಂಕೇತ(), (ಪೂರ್ಣಾಂಕ)vp->value.integer);
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ vpiSuppressVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <suppress>\n",
			  ಅಭಿವ್ಯಕ್ತಿ->get_type_ಸಂಕೇತ());
		  ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <%d>=?\n",
			  ಅಭಿವ್ಯಕ್ತಿ->get_type_ಸಂಕೇತ(), (ಪೂರ್ಣಾಂಕ)vp->format);
	    }
}

ರಚನೆ vpip_put_ಮೌಲ್ಯ_event : vvp_gen_event_s {
      vpiHandle handle;
      s_vpi_ಮೌಲ್ಯ value;
      ಪೂರ್ಣಾಂಕ flags;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸತತ_ಸತತ() override;
      ~vpip_put_ಮೌಲ್ಯ_event() override { }
};

ಶೂನ್ಯ vpip_put_ಮೌಲ್ಯ_event::ಸತತ_ಸತತ()
{
      handle->vpi_put_ಮೌಲ್ಯ(&value, flags);
      ಆಯ್ಕೆ (value.format) {
	    /* Free the copied string. */
	  ಸ್ಥಿತಿ vpiBinStrVal:
	  ಸ್ಥಿತಿ vpiOctStrVal:
	  ಸ್ಥಿತಿ vpiDecStrVal:
	  ಸ್ಥಿತಿ vpiHexStrVal:
	  ಸ್ಥಿತಿ vpiStringVal:
	    ಬಿಡುಗಡೆ(value.value.str);
	    ನಿಲ್ಲಿಸು;
	    /* Free the copied time structure. */
	  ಸ್ಥಿತಿ vpi_ಸಮಯ_Val:
	    ಬಿಡುಗಡೆ(value.value.time);
	    ನಿಲ್ಲಿಸು;
	    /* Free the copied vector structure. */
	  ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:
	    ಬಿಡುಗಡೆ(value.value.vector);
	    ನಿಲ್ಲಿಸು;
	    /* Free the copied strength structure. */
	  ಸ್ಥಿತಿ vpiStrengthVal:
	    ಬಿಡುಗಡೆ(value.value.strength);
	    ನಿಲ್ಲಿಸು;
	    /* Everything else is static in the structure. */
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
      }
}

/* Make a copy of a pointer to a time structure. */
ಸ್ಥಿರ t_vpi_ಸಮಯ *timedup(ಸ್ಥಿರಾಂಕ t_vpi_ಸಮಯ *val)
{
      t_vpi_ಸಮಯ *rtn;
      rtn = static_cast<t_vpi_ಸಮಯ *> (ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ(t_vpi_ಸಮಯ)));
      *rtn = *val;
      ಹಿಂತಿರುಗಿ rtn;
}

/* Make a copy of a pointer to a vector value structure. */
ಸ್ಥಿರ t_vpi_vecval *vectordup(ಸ್ಥಿರಾಂಕ t_vpi_vecval *val, PLI_INT32 size)
{
      ಚಿಹ್ನೆರಹಿತ ಅಂಕೆ_ಬೈಟುಗಳು;
      t_vpi_vecval *rtn;
      ಪ್ರತಿಜ್ಞಾ(size > 0);
      ಅಂಕೆ_ಬೈಟುಗಳು = ((size + 31)/32)*ಗಾತ್ರ(t_vpi_vecval);
      rtn = static_cast<t_vpi_vecval *> (ಸ್ಮರಣೆ_ಕೊಡು(ಅಂಕೆ_ಬೈಟುಗಳು));
      ಸ್ಮರಣೆ_ನಕಲು(rtn, val, ಅಂಕೆ_ಬೈಟುಗಳು);
      ಹಿಂತಿರುಗಿ rtn;
}

/* Make a copy of a pointer to a strength structure. */
ಸ್ಥಿರ t_vpi_strengthval *strengthdup(ಸ್ಥಿರಾಂಕ t_vpi_strengthval *val)
{
      t_vpi_strengthval *rtn;
      rtn = static_cast<t_vpi_strengthval *>
            (ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ(t_vpi_strengthval)));
      *rtn = *val;
      ಹಿಂತಿರುಗಿ rtn;
}

vpiHandle vpi_put_ಮೌಲ್ಯ(vpiHandle obj, s_vpi_ಮೌಲ್ಯ*vp,
			s_vpi_ಸಮಯ*when, PLI_INT32 flags)
{
      ಪ್ರತಿಜ್ಞಾ(obj);

      flags &= ~vpiReturnEvent;

      ಆದರೆ (flags!=vpiNoDelay && flags!=vpiForceFlag && flags!=vpiReleaseFlag) {
	    vvp_time64_t dly;
	    ಪೂರ್ಣಾಂಕ scale;

	    ಆದರೆ (vpi_get(vpiAutomatic, obj)) {
		  fprintf(stderr, "VPI error: cannot put a value with "
				  "a delay on automatically allocated "
				  "variable '%s'.\n",
				  vpi_get_str(vpiName, obj));
		  ಹಿಂತಿರುಗಿ 0;
	    }

	    ಪ್ರತಿಜ್ಞಾ(when != 0);

	    ಆಯ್ಕೆ (when->type) {
		ಸ್ಥಿತಿ vpi_Scaled_Real_ಸಮಯ:
		  scale = vpip_ಸಮಯ_units_from_handle(obj) -
		          vpip_get_ಸಮಯ_precision();
		  ಆದರೆ (scale >= 0) {
			dly = (vvp_time64_t)(when->real * ಘಾತ(10.0, scale));
		  } ಇಲ್ಲದಿದ್ದರೆ {
			dly = (vvp_time64_t)(when->real / ಘಾತ(10.0, -scale));
		  }
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpi_Sim_ಸಮಯ:
		  dly = vpip_timestruct_ಗಮ್ಯ_ಸಮಯ(when);
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  dly = 0;
		  ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ ((dly == 0) && ವೇಳಾಪಟ್ಟಿ_at_rosync()) {
		  fprintf(stderr, "VPI error: attempted to put a value to "
				  "variable '%s' during a read-only synch "
				  "callback.\n", vpi_get_str(vpiName, obj));
		  ಹಿಂತಿರುಗಿ 0;
	    }

	    vpip_put_ಮೌಲ್ಯ_event*put = ಹೊಸ vpip_put_ಮೌಲ್ಯ_event;
	    put->handle = obj;
	    ಆದರೆ (dynamic_cast<__vpiNamedEvent*>(obj)) {
		  put->value.format = vpiIntVal;
		  put->value.value.integer = 0;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಪ್ರತಿಜ್ಞಾ(vp);
		  put->value = *vp;
	    }
	      /* Since this is a scheduled put event we must copy any pointer
	       * data to keep it available until the event is actually run. */
	    ಆಯ್ಕೆ (put->value.format) {
		  /* Copy the string items. */
		ಸ್ಥಿತಿ vpiBinStrVal:
		ಸ್ಥಿತಿ vpiOctStrVal:
		ಸ್ಥಿತಿ vpiDecStrVal:
		ಸ್ಥಿತಿ vpiHexStrVal:
		ಸ್ಥಿತಿ vpiStringVal:
		  put->value.value.str = strdup(put->value.value.str);
		  ನಿಲ್ಲಿಸು;
		  /* Copy a time pointer item. */
		ಸ್ಥಿತಿ vpi_ಸಮಯ_Val:
		  put->value.value.time = timedup(put->value.value.time);
		  ನಿಲ್ಲಿಸು;
		  /* Copy a vector pointer item. */
		ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:
		  put->value.value.vector = vectordup(put->value.value.vector,
		                                      vpi_get(vpi_ಗಾತ್ರ, obj));
		  ನಿಲ್ಲಿಸು;
		  /* Copy a strength pointer item. */
		ಸ್ಥಿತಿ vpiStrengthVal:
		  put->value.value.strength =
		      strengthdup(put->value.value.strength);
		  ನಿಲ್ಲಿಸು;
		  /* Everything thing else is already in the structure. */
		ಪೂರ್ವನಿಯೋಜಿತ:
		  ನಿಲ್ಲಿಸು;
	    }
	    put->flags = flags;
	    ವೇಳಾಪಟ್ಟಿ_generic(put, dly, ಸುಳ್ಳು, ನಿಜ, ನಿಜ);
	    ಹಿಂತಿರುಗಿ 0;
      }

      ಆದರೆ (ವೇಳಾಪಟ್ಟಿ_at_rosync()) {
            fprintf(stderr, "VPI error: attempted to put a value to "
			    "variable '%s' during a read-only synch "
			    "callback.\n", vpi_get_str(vpiName, obj));
            ಹಿಂತಿರುಗಿ 0;
      }

      obj->vpi_put_ಮೌಲ್ಯ(vp, flags);

      ಹಿಂತಿರುಗಿ 0;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle res = 0;

      ಆದರೆ (ref == 0) {
	      // A few types can apply to a nil handle. These are ways
	      // that the current function can get started finding things.
	    ಆಯ್ಕೆ (type) {

		ಸ್ಥಿತಿ vpiScope:
		    // The IEEE1364-2005 doesn't seem to allow this,
		    // but some users seem to think it's handy, so
		    // return the scope that contains this SysTfCall.
		  ಪ್ರತಿಜ್ಞಾ(vpip_cur_task);
		  res = vpip_cur_task->vpi_handle(vpiScope);
		  ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ vpiSysTfCall:
		    // This is how VPI users get a first handle into
		    // the system. This is the handle of the system
		    // task/function call currently being executed.
		  ಆದರೆ (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle(vpiSysTfCall, 0) "
				"-> %p (%s)\n", vpip_cur_task,
				vpip_cur_task->defn->info.tfname);
		  }
		  ಹಿಂತಿರುಗಿ vpip_cur_task;

		ಪೂರ್ವನಿಯೋಜಿತ:
		  fprintf(stderr, "VPI error: vpi_handle(type=%d, ref=0).\n",
			  (ಪೂರ್ಣಾಂಕ)type);
		  res = 0;
		  ನಿಲ್ಲಿಸು;
	    }

      } ಇಲ್ಲದಿದ್ದರೆ {

	    ಆದರೆ (type == vpiSysTfCall) {
		  fprintf(stderr, "VPI error: vpi_handle(vpiSysTfCall, "
			  "ref!=0).\n");
		  ಹಿಂತಿರುಗಿ 0;
	    }

	    res = ref->vpi_handle(type);
      }


      ಆದರೆ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle(vpiScope, ref=%p) "
		    "-> %p\n", vpip_cur_task, ref);
      }

      ಹಿಂತಿರುಗಿ res;
}

ಸ್ಥಿರ vpiHandle vpip_make_udp_iterator()
{
// HERE: Add support for iterating over UDP definitions.
//       See 26.6.16 (page 400 in 1364-2005).
      ಹಿಂತಿರುಗಿ 0;
}

/*
 * This function asks the object to return an iterator for
 * the specified reference. It is up to the iterate_ method to
 * allocate a properly formed iterator.
 */
ಸ್ಥಿರ vpiHandle vpi_iterate_global(ಪೂರ್ಣಾಂಕ type)
{
      ಆಯ್ಕೆ (type) {
	  ಸ್ಥಿತಿ vpiInstance:
	    // fallthrough
	  ಸ್ಥಿತಿ vpiModule:
	    // fallthrough
	  ಸ್ಥಿತಿ vpi_ಕಾರ್ಯಕ್ರಮ:
	    // fallthrough
	  ಸ್ಥಿತಿ vpiInterface:
	    // fallthrough
	  ಸ್ಥಿತಿ vpiPackage:
	    ಹಿಂತಿರುಗಿ vpip_make_ಬೇರು_iterator(type);

	  ಸ್ಥಿತಿ vpiUdpDefn:
	    ಹಿಂತಿರುಗಿ vpip_make_udp_iterator();

	  ಸ್ಥಿತಿ vpiUserSystf:
	    ಹಿಂತಿರುಗಿ vpip_make_systf_iterator();
      }

      ಹಿಂತಿರುಗಿ 0;
}

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle rtn = 0;

      ಪ್ರತಿಜ್ಞಾ(vpi_ಬಹುಲಕ_flag != VPI_ಬಹುಲಕ_NONE);
      ಆದರೆ (vpi_ಬಹುಲಕ_flag == VPI_ಬಹುಲಕ_ರಿಜಿಸ್ಟರ್) {
	    fprintf(stderr, "vpi error: vpi_iterate called during "
		    "vpi_register_systf. You can't do that!\n");
	    ಹಿಂತಿರುಗಿ 0;
      }

      ಆದರೆ (ref == 0)
	    rtn = vpi_iterate_global(type);
      ಇಲ್ಲದಿದ್ದರೆ
	    rtn = ref->vpi_iterate(type);

      ಆದರೆ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_iterate(%d, %p) ->%s\n",
	    (ಪೂರ್ಣಾಂಕ)type, ref, rtn ? "" : " (null)");
      }

      ಹಿಂತಿರುಗಿ rtn;
}

vpiHandle vpi_handle_by_ಸೂಚ್ಯಂಕ(vpiHandle ref, PLI_INT32 idx)
{
      ಪ್ರತಿಜ್ಞಾ(ref);
      ಹಿಂತಿರುಗಿ ref->vpi_ಸೂಚ್ಯಂಕ(idx);
}

ಸ್ಥಿರ vpiHandle ಕಂಡುಹಿಡಿ_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, vpiHandle handle)
{
      vpiHandle rtn = 0;
      __vpiScope*ref = dynamic_cast<__vpiScope*>(handle);

      /* check module names */
      ಆದರೆ (!strcmp(name, vpi_get_str(vpiName, handle)))
	    rtn = handle;

      /* brute force search for the name in all objects in this scope */
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0 ;  i < ref->intern.size() ;  i += 1) {
	      /* The standard says that since a port does not have a full
	       * name it cannot be found by name. Because of this we need
	       * to skip ports here so the correct handle can be located. */
	    ಆದರೆ (vpi_get(vpiType, ref->intern[i]) == vpi_ಪೋರ್ಟ್) ಮುಂದುವರಿ;
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nm = vpi_get_str(vpiName, ref->intern[i]);
	    ಆದರೆ (nm && !strcmp(name, nm)) {
		  rtn = ref->intern[i];
		  ನಿಲ್ಲಿಸು;
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vpi_get(vpiType, ref->intern[i]) == vpi_ಸ್ಮರಣೆ ||
	               vpi_get(vpiType, ref->intern[i]) == vpiNetArray) {
		  /* We need to iterate on the words */
		  vpiHandle ಶಬ್ದ_i, ಶಬ್ದ_h;
		  ಶಬ್ದ_i = vpi_iterate(vpi_ಸ್ಮರಣೆ_ಶಬ್ದ, ref->intern[i]);
		  ಆಗ (ಶಬ್ದ_i && (ಶಬ್ದ_h = vpi_ಪರಿಶೀಲನೆ(ಶಬ್ದ_i))) {
			nm = vpi_get_str(vpiName, ಶಬ್ದ_h);
			ಆದರೆ (nm && !strcmp(name, nm)) {
			      rtn = ಶಬ್ದ_h;
			      vpi_free_object(ಶಬ್ದ_i);
			      ನಿಲ್ಲಿಸು;
			}
		  }
	    }
	    /* found it yet? */
	    ಆದರೆ (rtn) ನಿಲ್ಲಿಸು;
      }

      ಹಿಂತಿರುಗಿ rtn;
}

// Find the end of the escaped identifier or simple identifier
ಸ್ಥಿರ ಅಕ್ಷರ * ಕಂಡುಹಿಡಿ_rest(ಅಕ್ಷರ *name)
{
      ಅಕ್ಷರ *rest;
      ಆದರೆ (*name == '\\') {
	    rest = strchr(name, ' ');
	    ಆದರೆ (rest) {
		  *rest++ = 0; // The space is not part of the string
		    // There should be a '.' after the escaped ID if there is
		    // anything more. If it is missing add it to avoid a crash
		  ಆದರೆ ((*rest != '.') && (*rest != 0)) {
			*--rest = '.';
			fprintf(stderr, "ERROR: Malformed scope string: \"%s\"", name);
		  }
		  ಆದರೆ (*rest == 0) {
			rest = 0;
		  }
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
	    rest = strchr(name, '.');
      }
      ಹಿಂತಿರುಗಿ rest;
}

ಸ್ಥಿರ vpiHandle ಕಂಡುಹಿಡಿ_scope(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, vpiHandle handle, ಪೂರ್ಣಾಂಕ ಆಳ)
{

      vpiHandle iter = handle==0
	    ? vpi_iterate(vpiModule, ಖಾಲಿ)
	    : vpi_iterate(vpi_ಆಂತರಿಕ_Scope, handle);

      vector<ಅಕ್ಷರ> name_buf (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name)+1);
      strcpy(&name_buf[0], name);
      ಅಕ್ಷರ*nm_ಮೊದಲ = &name_buf[0];
      ಅಕ್ಷರ*nm_rest = ಕಂಡುಹಿಡಿ_rest(nm_ಮೊದಲ);
      ಆದರೆ (*nm_ಮೊದಲ == '\\') ++nm_ಮೊದಲ;
      ಆದರೆ (nm_rest) {
	    *nm_rest++ = 0;
      }

      vpiHandle rtn = 0;
      vpiHandle hand;
      ಆಗ (iter && (hand = vpi_ಪರಿಶೀಲನೆ(iter))) {
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nm = vpi_get_str(vpiName, hand);

	    ಆದರೆ (strcmp(nm_ಮೊದಲ,nm)==0) {
		  ಆದರೆ (nm_rest)
			rtn=ಕಂಡುಹಿಡಿ_scope(nm_rest, hand, ಆಳ+1);
		  ಇಲ್ಲದಿದ್ದರೆ
			rtn = hand;
	    }

	    /* found it yet ? */
	    ಆದರೆ (rtn) {
		  vpi_free_object(iter);
		  ನಿಲ್ಲಿಸು;
	    }
      }

      ಹಿಂತಿರುಗಿ rtn;
}

// Find the end of the first escaped identifier or simple identifier
ಸ್ಥಿರ ಅಕ್ಷರ * ಕಂಡುಹಿಡಿ_ಮುಂದಿನ(ಅಕ್ಷರ *name)
{
      ಅಕ್ಷರ *ಮುಂದಿನ_2;
      ಆದರೆ (*name == '\\') {
	    ಮುಂದಿನ_2 = strchr(name, ' ');
	    ಆದರೆ (ಮುಂದಿನ_2 && *++ಮುಂದಿನ_2 == 0) ಮುಂದಿನ_2 = 0;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಮುಂದಿನ_2 = strchr(name, '.');
      }
      ಹಿಂತಿರುಗಿ ಮುಂದಿನ_2;
}

vpiHandle vpi_handle_by_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, vpiHandle scope)
{
      vpiHandle hand;

      ಆದರೆ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name(%s, %p) -->\n",
		    name, scope);
      }

	// Chop the name into path and base. For example, if the name
	// is "a.b.c", then nm_path becomes "a.b" and nm_base becomes
	// "c". If the name is "c" then nm_path is nil and nm_base is "c".
      vector<ಅಕ್ಷರ> name_buf (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name)+1);
      strcpy(&name_buf[0], name);
      ಅಕ್ಷರ*nm_ಮಾರ್ಗ = &name_buf[0];
      ಅಕ್ಷರ*nm_ತಳ;

	// Check to see if we have an escaped identifier and if so search
	// the long way because a '.' could be in the escaped name.
      ಆದರೆ (strchr(nm_ಮಾರ್ಗ, '\\')) {
	    ಅಕ್ಷರ *ಮುಂದಿನ_2;
	    nm_ತಳ = nm_ಮಾರ್ಗ;
	    ಆಗ ((ಮುಂದಿನ_2 = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ(nm_ತಳ))) {
		  nm_ತಳ = ++ಮುಂದಿನ_2;
	    }
	    ಆದರೆ (nm_ಮಾರ್ಗ == nm_ತಳ) {
		  nm_ಮಾರ್ಗ = 0;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  *(nm_ತಳ-1) = 0;
	    }

	// If there is no escaped identifier then just look for the last '.'
      } ಇಲ್ಲದಿದ್ದರೆ {
	    nm_ತಳ = strrchr(nm_ಮಾರ್ಗ, '.');
	    ಆದರೆ (nm_ತಳ) {
		  *nm_ತಳ++ = 0;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  nm_ತಳ = nm_ಮಾರ್ಗ;
		  nm_ಮಾರ್ಗ = 0;
	    }
      }

      /* If scope provided, look in corresponding module; otherwise
       * traverse the hierarchy specified in name to find the leaf module
       * and try finding it there.
       */
      ಆದರೆ (scope) {
	    /* Some implementations support either a module or a scope. */
	    ಆಯ್ಕೆ (vpi_get(vpiType, scope)) {
		ಸ್ಥಿತಿ vpiScope:
	          hand = vpi_handle(vpiModule, scope);
	          ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiModule:
	          hand = scope;
	          ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  ಆದರೆ (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle_by_name: "
				"Scope is not a vpiScope or vpiModule\n");
		  }
	          // Use vpi_chk_error() here when it is implemented.
	          ಹಿಂತಿರುಗಿ 0;
	    }

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nm_ಮಾರ್ಗ) {
	      // The name has a path, and no other scope handle was
	      // passed in. That suggests we are looking for "a.b.c"
	      // in the root scope. So convert "a.b" to a scope and
	      // start there to look for "c".
	    hand = ಕಂಡುಹಿಡಿ_scope(nm_ಮಾರ್ಗ, ಖಾಲಿ, 0);
	    nm_ಮಾರ್ಗ = 0;

      } ಇಲ್ಲದಿದ್ದರೆ {
	      // Special case: scope==<nil>, meaning we are looking in
	      // the root, and there is no path to the name, i.e. the
	      // string is "c" instead of "top.c". Try to find "c" as
	      // a scope and return that.
	    hand = ಕಂಡುಹಿಡಿ_scope(nm_ತಳ, ಖಾಲಿ, 0);
      }

      ಆದರೆ (hand == 0) {
	    ಆದರೆ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_handle_by_name: "
			  "Scope does not exist. Giving up.\n");
	    }

	    ಹಿಂತಿರುಗಿ 0;
      }

	// If there is a path part, then use it to find the
	// scope. For example, if the full name is a.b.c, then
	// the nm_path string is a.b and we search for that
	// scope. If we find it, then set hand to that scope.
      ಆದರೆ (nm_ಮಾರ್ಗ) {
	    vpiHandle tmp = ಕಂಡುಹಿಡಿ_scope(nm_ಮಾರ್ಗ, hand, 0);
	    ಆಗ (tmp == 0 && hand != 0) {
		  hand = vpi_handle(vpiScope, hand);
		  tmp = ಕಂಡುಹಿಡಿ_scope(nm_ಮಾರ್ಗ, hand, 0);
	    }
	    hand = tmp;
      }

	// find_name() expects escaped identifiers to be stripped
      ಆದರೆ (*nm_ತಳ == '\\') {
	    // Skip the \ at the beginning
	    ++nm_ತಳ;

	    // Drop the space at the end if it exists
	    ಅಕ್ಷರ *ಮುಂದಿನ_2;
	    ಆದರೆ ((ಮುಂದಿನ_2 = strchr(nm_ತಳ, ' '))) {
		  *ಮುಂದಿನ_2 = 0;
	    }
      }

	// Now we have the correct scope, look for the item.
      vpiHandle out = ಕಂಡುಹಿಡಿ_name(nm_ತಳ, hand);

      ಆದರೆ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name: DONE\n");
      }

      ಹಿಂತಿರುಗಿ out;
}

// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected
// x2: Ptr to previous node
// x3: True when net is connected to output of previous node
tuple<ತಾರ್ಕಿಕ, vvp_net_ptr_t, ತಾರ್ಕಿಕ> check_connected_ಗಮ್ಯ_concat8_and_part_sa(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t ಪೋಷಕ) {

      vvp_net_ptr_t prev = vvp_net_ptr_t(ಶೂನ್ಯಸೂಚಕ, 0);

      ಆಗ (cur.ptr()) {
	    ಆದರೆ (cur.ptr() == net2 && dynamic_cast<vvp_fun_part_sa*>(ಪೋಷಕ.ptr()->fun)) {
		    // Pass on the return value if found
		  ಹಿಂತಿರುಗಿ {ನಿಜ, prev.ptr() ? prev : ಪೋಷಕ, prev.ptr() == ಶೂನ್ಯಸೂಚಕ};
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  ಸ್ವಯಂಚಾಲಿತ res = check_connected_ಗಮ್ಯ_concat8_and_part_sa(cur.ptr()->ಹೊರಹೋಗು_2, net2, cur);
		  ಆದರೆ (get<0>(res)) {
			ಹಿಂತಿರುಗಿ res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->ಪೋರ್ಟ್[cur.ಪೋರ್ಟ್()];
      }

	// net2 is not connected to this concat8
      ಹಿಂತಿರುಗಿ {ಸುಳ್ಳು, vvp_net_ptr_t(ಶೂನ್ಯಸೂಚಕ, 0), ಸುಳ್ಳು};
}

// Check if net2 is connected to current_net through a net of vvp_fun_concat8s
// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected with concats and part_sas
// x2: Ptr to previous node
// x3: True when net is connected to output of x2
tuple<ತಾರ್ಕಿಕ, vvp_net_ptr_t, ತಾರ್ಕಿಕ> check_connected_ಗಮ್ಯ_concat8(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t ಪೋಷಕ)
{
      vvp_net_ptr_t prev = vvp_net_ptr_t(ಶೂನ್ಯಸೂಚಕ, 0);
	// For everything connected
      ಆಗ (cur.ptr()) {
	      // Check if it's a concat8
	    ಆದರೆ (cur.ptr() == net2) {
		  ಹಿಂತಿರುಗಿ {ನಿಜ, vvp_net_ptr_t(ಶೂನ್ಯಸೂಚಕ, 0), ಸುಳ್ಳು};
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  ಸ್ವಯಂಚಾಲಿತ res = check_connected_ಗಮ್ಯ_concat8(cur.ptr()->ಹೊರಹೋಗು_2, net2, cur);
		  ಆದರೆ (get<0>(res)) {
			ಆದರೆ (!get<1>(res).ptr() && dynamic_cast<vvp_fun_part_sa*>(ಪೋಷಕ.ptr()->fun)) {
			      // `parent` is the last part_sa node in front of our destination
			      // We want to place the intermod node here
			      ಹಿಂತಿರುಗಿ {ನಿಜ, prev.ptr() ? prev : ಪೋಷಕ, prev.ptr() == ಶೂನ್ಯಸೂಚಕ};
			}
			ಹಿಂತಿರುಗಿ res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->ಪೋರ್ಟ್[cur.ಪೋರ್ಟ್()];
      }

	// net2 is not connected to this concat8
      ಹಿಂತಿರುಗಿ {ಸುಳ್ಳು, vvp_net_ptr_t(ಶೂನ್ಯಸೂಚಕ, 0), ಸುಳ್ಳು};
}

// Used to get intermodpath for two ports
vpiHandle vpi_handle_multi(PLI_INT32 type,
                           vpiHandle ref1,
                           vpiHandle ref2)
{
      ಆದರೆ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_multi(%d, %p, %p) -->\n",
	            type, ref1, ref2);
      }

      ಆದರೆ (type != vpi_Inter_Mod_ಮಾರ್ಗ) {
	    fprintf(stderr, "sorry: vpi_handle_multi currently supports"
	            "only vpiInterModPath\n");
	    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
      }

	// Indicates whether port1 refers to a single bit
      ತಾರ್ಕಿಕ port1_has_ಸೂಚ್ಯಂಕ = ಸುಳ್ಳು;
      ಪೂರ್ಣಾಂಕ port1_ಬಿಟ್_ಸೂಚ್ಯಂಕ = 0;
      vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info* port1_ಬಿಟ್ = dynamic_cast<vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info*>(ref1);

      ಆದರೆ (port1_ಬಿಟ್) {
	      // Get the bit index
	    port1_has_ಸೂಚ್ಯಂಕ = ನಿಜ;
	    port1_ಬಿಟ್_ಸೂಚ್ಯಂಕ = vpi_get(vpi_ಬಿಟ್, port1_ಬಿಟ್);
	      // Update the ref1 to point to the base port
	    ref1 = vpi_handle(vpi_ಪೋಷಕ, port1_ಬಿಟ್);
      }

	// Indicates whether port2 refers to a single bit
      ತಾರ್ಕಿಕ port2_has_ಸೂಚ್ಯಂಕ = ಸುಳ್ಳು;
      ಪೂರ್ಣಾಂಕ port2_ಬಿಟ್_ಸೂಚ್ಯಂಕ = 0;
      vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info* port2_ಬಿಟ್ = dynamic_cast<vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info*>(ref2);

      ಆದರೆ (port2_ಬಿಟ್) {
	      // Get the bit index
	    port2_has_ಸೂಚ್ಯಂಕ = ನಿಜ;
	    port2_ಬಿಟ್_ಸೂಚ್ಯಂಕ = vpi_get(vpi_ಬಿಟ್, port2_ಬಿಟ್);
	      // Update the ref1 to point to the base port
	    ref2 = vpi_handle(vpi_ಪೋಷಕ, port2_ಬಿಟ್);
      }

      vpi_ಪೋರ್ಟ್_Info* port1 = dynamic_cast<vpi_ಪೋರ್ಟ್_Info*>(ref1);

      ಆದರೆ (!port1) {
	    fprintf(stderr, "sorry: second argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
      }

      vpi_ಪೋರ್ಟ್_Info* port2 = dynamic_cast<vpi_ಪೋರ್ಟ್_Info*>(ref2);

      ಆದರೆ (!port2) {
	    fprintf(stderr, "sorry: third argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
      }

	// Get the names of both ports
      std::string port1_name(vpi_get_str(vpiName, ref1));
      std::string port2_name(vpi_get_str(vpiName, ref2));

	// If both ports are vpiOutput, we have to reassign the __vpiSignal from port1
	// to port2 because otherwise the non-delayed version of the signal is dumped
	// even tho the intermodpath is correctly inserted
      __vpiSignal* output_signal = ಶೂನ್ಯಸೂಚಕ;

      ಆದರೆ (port1->get_direction() == vpiOutput && port2->get_direction() == vpiOutput) {
	    vpiHandle scope_port2 = vpi_handle(vpiScope, ref2);
	    ಪ್ರತಿಜ್ಞಾ(scope_port2);

	      // Iterate over nets in the scope of port2
	    vpiHandle net_i = vpi_iterate(vpiNet, scope_port2) ;
	    vpiHandle net;

	    ಆಗ ((net = vpi_ಪರಿಶೀಲನೆ(net_i)) != ಖಾಲಿ) {
		  std::string net_name(vpi_get_str(vpiName, net));

		    // Compare whether the net matches with the port name
		  ಆದರೆ (net_name == port2_name) {
			output_signal = dynamic_cast<__vpiSignal*>(net);
		  }
	    }
      }

      vvp_net_t* net1 = port1->get_ಪೋರ್ಟ್();
      vvp_net_t* net2 = port2->get_ಪೋರ್ಟ್();

      ಆದರೆ (net1 == ಶೂನ್ಯಸೂಚಕ || net2 == ಶೂನ್ಯಸೂಚಕ) {
	    fprintf(stderr, "Error: Could not find net. "
		    "Did you run iverilog with '-ginterconnect'?\n");
	    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
      }

      ಆದರೆ (net1 == net2) {
	    fprintf(stderr, "Error: Net for both ports is the same. "
		    "Did you pass the same port twice?\n");
	    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
      }

      ಆದರೆ (!dynamic_cast<vvp_fun_buft*>(net1->fun)) {
	    fprintf(stderr, "Error: functor of net1 must be"
		    "vvp_fun_buft\n");
	    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
      }

      ಆದರೆ (!dynamic_cast<vvp_fun_buft*>(net2->fun)) {
	    fprintf(stderr, "Error: functor of net2 must be"
		    "vvp_fun_buft\n");
	    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
      }

	// If port1 is actually a port bit, we have to get to the correct vvp_fun_part
	// after which we insert the intermodpath delay
      ಆದರೆ (port1_has_ಸೂಚ್ಯಂಕ) {
	    vvp_net_ptr_t* net1_ptr = &net1->ಹೊರಹೋಗು_2;

	      // Search for part selects connected to port1
	    vvp_net_t* current_net = net1_ptr->ptr();

	    ಆಗ (current_net) {
		  ಸ್ಥಿರಾಂಕ vvp_fun_part* part = dynamic_cast<vvp_fun_part*>(current_net->fun);

		    // Its a part select!
		  ಆದರೆ (part) {
			  // Is it the correct part select?
			ಆದರೆ (part->get_ತಳ() == (ಚಿಹ್ನೆರಹಿತ)port1_ಬಿಟ್_ಸೂಚ್ಯಂಕ) {
			      ಪ್ರತಿಜ್ಞಾ(part->get_wid() == 1);
			      net1 = current_net; // Replace net1 as this is our new start point
			      ನಿಲ್ಲಿಸು;
			}
		  }

		  current_net = current_net->ಪೋರ್ಟ್[0].ptr(); // BUFT has only one input, index 0
	    }
      }

	// Iterate over all nodes connected to port1
      vvp_net_ptr_t cur = net1->ಹೊರಹೋಗು_2;
      vvp_net_ptr_t prev = vvp_net_ptr_t(ಶೂನ್ಯಸೂಚಕ, 0);

      ಆಗ (cur.ptr()) {
	      // Either port2 is directly connected to port1
	      // Or in the second case port2 is indirectly connected
	      // to port1 through a net of concat8s
	    ತಾರ್ಕಿಕ is_connected, is_output;
	    vvp_net_ptr_t ಹಿಂದಿನ_ಗಂಟು;
	    tie(is_connected, ಹಿಂದಿನ_ಗಂಟು, is_output) = port2_has_ಸೂಚ್ಯಂಕ
		  ? check_connected_ಗಮ್ಯ_concat8(cur, net2, vvp_net_ptr_t(net1, 0))
		  : check_connected_ಗಮ್ಯ_concat8_and_part_sa(cur, net2, vvp_net_ptr_t(net1, 0));
	    ಆದರೆ ( (!port2_has_ಸೂಚ್ಯಂಕ && cur.ptr() == net2) || is_connected ) {
		  vvp_net_t*new_net = ಹೊಸ vvp_net_t;

		    // Create new node with intermodpath and connect port2 to it
		  ಪೂರ್ಣಾಂಕ width = 1; // TODO
		  vvp_fun_intermodpath*obj = ಹೊಸ vvp_fun_intermodpath(new_net, width);

		  vvp_net_t* output_net = net1;
		  ಆದರೆ (is_connected) {
			ಆದರೆ (is_output) {
			      output_net = ಹಿಂದಿನ_ಗಂಟು.ptr(); // net2 is direct output of part_sa
			      cur = output_net->ಹೊರಹೋಗು_2;
			      prev = vvp_net_ptr_t(ಶೂನ್ಯಸೂಚಕ, 0);
			} ಇಲ್ಲದಿದ್ದರೆ {
			      prev = ಹಿಂದಿನ_ಗಂಟು;
			      ಆದರೆ (prev.ptr()) {
				    cur = prev.ptr()->ಪೋರ್ಟ್[prev.ಪೋರ್ಟ್()];
			      }
			}
		  }

		  new_net->fun = obj;
		  new_net->ಹೊರಹೋಗು_2 = cur;

		    // Port2 is in the middle of the list
		    // Insert intermodpath before port2 and keep everything else intact
		  ಆದರೆ (prev.ptr()) {
			prev.ptr()->ಪೋರ್ಟ್[prev.ಪೋರ್ಟ್()] = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->ಪೋರ್ಟ್[0] = cur.ptr()->ಪೋರ್ಟ್[cur.ಪೋರ್ಟ್()]; // Connect the next net in list
			cur.ptr()->ಪೋರ್ಟ್[cur.ಪೋರ್ಟ್()] = vvp_net_ptr_t(ಶೂನ್ಯಸೂಚಕ, 0); // Only port2 is connected to intermodpath
		    // Port2 is first in list
		    // Insert intermodpath before port2 and keep everything else intact
		  } ಇಲ್ಲದಿದ್ದರೆ {
			output_net->ಹೊರಹೋಗು_2 = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->ಪೋರ್ಟ್[0] = cur.ptr()->ಪೋರ್ಟ್[cur.ಪೋರ್ಟ್()]; //  Connect the next net in list
			cur.ptr()->ಪೋರ್ಟ್[cur.ಪೋರ್ಟ್()] = vvp_net_ptr_t(ಶೂನ್ಯಸೂಚಕ, 0); // Only port2 is connected to intermodpath
		  }

		    // If both ports are vpiOutput and port2 is not a vector,
		    // we have to reassign the __vpiSignal so that the delayed
		    // values get dumped
		  ಆದರೆ (output_signal && !port2_has_ಸೂಚ್ಯಂಕ) {
			net2->fil = net1->fil;
			net1->fil = ಶೂನ್ಯಸೂಚಕ;
			output_signal->ಗಂಟು = net2;
		  }

		    // Create the VPI intermodpath object
		  __vpiInterModPath* intermodpath = vpip_make_intermodpath(new_net, port1, port2);
		  intermodpath->intermodpath = obj;

		    // Finally done, return the intermodpath object
		  ಹಿಂತಿರುಗಿ intermodpath;
	    }

	    prev = cur;
	    cur = cur.ptr()->ಪೋರ್ಟ್[cur.ಪೋರ್ಟ್()]; // Next net in linked list
      }

      fprintf(stderr, "VPI error: Could not insert intermodpath!\n");
      fprintf(stderr, "\tport1 = %s, port1_has_index = %d, port1_bit_index = %d\n", port1_name.c_str(), port1_has_ಸೂಚ್ಯಂಕ, port1_ಬಿಟ್_ಸೂಚ್ಯಂಕ);
      fprintf(stderr, "\tport2 = %s, port2_has_index = %d, port2_bit_index = %d\n", port2_name.c_str(), port2_has_ಸೂಚ್ಯಂಕ, port2_ಬಿಟ್_ಸೂಚ್ಯಂಕ);
      ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
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

      ಆದರೆ (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_get_delays(%p, %p) -->\n", ಅಭಿವ್ಯಕ್ತಿ, delays);
      }
}


ಶೂನ್ಯ vpi_put_delays(vpiHandle ಅಭಿವ್ಯಕ್ತಿ, p_vpi_delay delays)
{
      ಪ್ರತಿಜ್ಞಾ(ಅಭಿವ್ಯಕ್ತಿ  );
      ಪ್ರತಿಜ್ಞಾ(delays );

      ಅಭಿವ್ಯಕ್ತಿ->vpi_put_delays(delays);

      ಆದರೆ (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_put_delays(%p, %p) -->\n", ಅಭಿವ್ಯಕ್ತಿ, delays);
      }
}








ಬಾಹ್ಯ "C" PLI_INT32 vpi_vprintf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*fmt, va_list ap)
{
      ಹಿಂತಿರುಗಿ vpi_mcd_vprintf(1, fmt, ap);
}

ಬಾಹ್ಯ "C" PLI_INT32 vpi_printf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      ಪೂರ್ಣಾಂಕ r = vpi_mcd_vprintf(1, fmt, ap);
      va_end(ap);
      ಹಿಂತಿರುಗಿ r;
}

ಬಾಹ್ಯ "C" PLI_INT32 vpi_flush(ಶೂನ್ಯ)
{
      ಹಿಂತಿರುಗಿ vpi_mcd_flush(1);
}


ಬಾಹ್ಯ "C" PLI_INT32 vpi_sim_vcontrol(ಪೂರ್ಣಾಂಕ operation, va_list ap)
{
      ದೀರ್ಘ diag_ಸಂದೇಶ;

      ಆಯ್ಕೆ (operation) {
	  ಸ್ಥಿತಿ vpi_ಮುಕ್ತಾಯ:
	  ಸ್ಥಿತಿ __ivl_legacy_vpiFinish:
            diag_ಸಂದೇಶ = va_arg(ap, PLI_INT32);
	    ವೇಳಾಪಟ್ಟಿ_ಮುಕ್ತಾಯ(diag_ಸಂದೇಶ);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiStop:
	  ಸ್ಥಿತಿ __ivl_legacy_vpiStop:
            diag_ಸಂದೇಶ = va_arg(ap, PLI_INT32);
	    ವೇಳಾಪಟ್ಟಿ_stop(diag_ಸಂದೇಶ);
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported VPI control operation %d.\n", operation);
	    ಹಿಂತಿರುಗಿ 0;
      }
      ಹಿಂತಿರುಗಿ 1;
}

ಬಾಹ್ಯ "C" PLI_INT32 vpi_sim_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      ಪೂರ್ಣಾಂಕ r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      ಹಿಂತಿರುಗಿ r;
}

ಬಾಹ್ಯ "C" PLI_INT32 vpi_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      ಪೂರ್ಣಾಂಕ r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      ಹಿಂತಿರುಗಿ r;
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
      ತಾರ್ಕಿಕ is_neg = ಸುಳ್ಳು;  // At this point only a real can be negative.

	/* Get the value as a vvp_vector4_t. */
      val.format = vpiObjTypeVal;
      vpi_get_ಮೌಲ್ಯ(arg, &val);
      ಆದರೆ (val.format == vpiRealVal) {
	    vpi_get_ಮೌಲ್ಯ(arg, &val);
	      /* All double values can be represented in 1024 bits. */
	    vec4 = vvp_vector4_t(1024, val.value.real);
	    ಆದರೆ (val.value.real < 0) is_neg = ನಿಜ;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    val.format = vpi_ಸದಿಶ_Val;
	    vpi_get_ಮೌಲ್ಯ(arg, &val);
	    ಚಿಹ್ನೆರಹಿತ wid = vpi_get(vpi_ಗಾತ್ರ, arg);
	    vec4 = vvp_vector4_t(wid, BIT4_0);
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx=0; idx < wid; idx += 1) {
		PLI_INT32 aval = val.value.vector[idx/32].aval;
		PLI_INT32 bval = val.value.vector[idx/32].bval;
		aval >>= idx % 32;
		bval >>= idx % 32;
		ಪೂರ್ಣಾಂಕ bitmask = (aval&1) | ((bval<<1)&2);
		vvp_bit4_t ಬಿಟ್ = scalar_ಗಮ್ಯ_bit4(bitmask);
		vec4.ಗಣ_ಬಿಟ್(idx, ಬಿಟ್);
	    }
      }

      ಆದರೆ (vec4.has_xz()) {
	    rtn.aval = rtn.bval = 0xFFFFFFFFU;  /* Set to 'bx. */
	    ಹಿಂತಿರುಗಿ rtn;
      }

      vvp_vector2_t vec2(vec4);

      ಆದರೆ (is_neg) vec2.trim_neg(); /* This is a special trim! */
      ಇಲ್ಲದಿದ್ದರೆ vec2.trim(); /* This makes less work shifting. */

	/* Calculate the clog2 result. */
      PLI_INT32 res = 0;
      ಆದರೆ (!vec2.is_zero()) {
	    vec2 -= vvp_vector2_t(1, vec2.size());
	    ಆಗ(!vec2.is_zero()) {
		  res += 1;
		  vec2 >>= 1;
	    }
      }

      rtn.aval = res;
      rtn.bval = 0;
      ಹಿಂತಿರುಗಿ rtn;
}

/*
 * This routine provides the information needed to implement $countdrivers.
 * It is done here for performance reasons - interrogating the drivers
 * individually via the VPI interface would be much slower.
 */
ಬಾಹ್ಯ "C" ಶೂನ್ಯ vpip_ಎಣಿಕೆ_drivers(vpiHandle ref, ಚಿಹ್ನೆರಹಿತ idx,
                                   ಚಿಹ್ನೆರಹಿತ counts[4])
{
      ರಚನೆ __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);
      rfp->ಗಂಟು->ಎಣಿಕೆ_drivers(idx, counts);
}

#ಆದರೆ defined(__MINGW32__) || defined (__CYGWIN__)
vpip_routines_s vpi_routines = {
    .ರಿಜಿಸ್ಟರ್_cb                = vpi_ರಿಜಿಸ್ಟರ್_cb,
    .remove_cb                  = vpi_remove_cb,
    .ರಿಜಿಸ್ಟರ್_systf             = vpi_ರಿಜಿಸ್ಟರ್_systf,
    .get_systf_info             = vpi_get_systf_info,
    .handle_by_name             = vpi_handle_by_name,
    .handle_by_ಸೂಚ್ಯಂಕ            = vpi_handle_by_ಸೂಚ್ಯಂಕ,
    .handle_multi               = vpi_handle_multi,
    .handle                     = vpi_handle,
    .iterate                    = vpi_iterate,
    .ಪರಿಶೀಲನೆ                       = vpi_ಪರಿಶೀಲನೆ,
    .get                        = vpi_get,
    .get64                      = vpi_get64,
    .get_str                    = vpi_get_str,
    .get_delays                 = vpi_get_delays,
    .put_delays                 = vpi_put_delays,
    .get_ಮೌಲ್ಯ                  = vpi_get_ಮೌಲ್ಯ,
    .put_ಮೌಲ್ಯ                  = vpi_put_ಮೌಲ್ಯ,
    .get_ಸಮಯ                   = vpi_get_ಸಮಯ,
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
    .ಗಣ_return_ಮೌಲ್ಯ           = vpip_ಗಣ_return_ಮೌಲ್ಯ,
};
#ಆದರೆ_ಅಂತ್ಯ
