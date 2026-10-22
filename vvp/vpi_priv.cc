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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <vector>
# include  <cstdio>
# include  <cstdarg>
# include  <cstring>
# include  <cassert>
# include  <cstdlib>
# include  <cmath>
# include  <iostream>

ఉపయోగించి నామాకాశ std;
vpi_బహుళకం_t vpi_బహుళకం_flag = VPI_బహుళకం_NONE;
FILE*vpi_trace = 0;

స్థిర s_vpi_vlog_info  vpi_vlog_info;
స్థిర s_vpi_error_info vpip_చివరి_error = { 0, 0, 0, 0, 0, 0, 0 };

స్థిరరాశి అక్షరం* direction_as_string(పూర్ణాంకం dir)
{
      ఎంపిక (dir) {
	  స్థితి vpi_ఆదానం:
	    తిరిగి "input";
	  స్థితి vpiOutput:
	    తిరిగి "output";
	  స్థితి vpiInout:
	    తిరిగి "inout";
	  స్థితి vpiMixedIO:
	    తిరిగి "mixed io";
	  స్థితి vpiNoDirection:
	    తిరిగి "no direction";
	  అప్రమేయ:
	    తిరిగి "INVALID-DIRECTION";
      }
}

__vpiHandle::~__vpiHandle()
{ }

పూర్ణాంకం __vpiHandle::vpi_get(పూర్ణాంకం)
{ తిరిగి vpiUndefined; }

int64_t __vpiHandle::vpi_get64(పూర్ణాంకం code)
{ తిరిగి (code == _vpiNexusId) ? 0 : vpiUndefined; }

అక్షరం* __vpiHandle::vpi_get_str(పూర్ణాంకం)
{ తిరిగి 0; }

శూన్య __vpiHandle::vpi_get_విలువ(p_vpi_విలువ)
{ }

vpiHandle __vpiHandle::vpi_put_విలువ(p_vpi_విలువ, పూర్ణాంకం)
{ తిరిగి 0; }

vpiHandle __vpiHandle::vpi_handle(పూర్ణాంకం)
{ తిరిగి 0; }

vpiHandle __vpiHandle::vpi_iterate(పూర్ణాంకం)
{ తిరిగి 0; }

vpiHandle __vpiHandle::vpi_సూచీ(పూర్ణాంకం)
{ తిరిగి 0; }

శూన్య __vpiHandle::vpi_get_delays(p_vpi_delay)
{ }

శూన్య __vpiHandle::vpi_put_delays(p_vpi_delay)
{ }

__vpiBaseVar::__vpiBaseVar(__vpiScope*scope, స్థిరరాశి అక్షరం*name, vvp_net_t*net)
: scope_(scope), name_(name), net_(net)
{
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
__vpiBaseVar::~__vpiBaseVar()
{
      vvp_net_delete(net_);
}
#అయితే_ముగింపు

/*
 * The default behavior for the vpi_free_object to an object is to
 * suppress the actual operation. This is because handles are
 * generally allocated semi-permanently within vvp context. Dynamic
 * objects will override the free_object_fun method to return an
 * appropriately effective function.
 */
స్థిర పూర్ణాంకం suppress_free(vpiHandle)
{ తిరిగి 1; }
__vpiHandle::free_object_fun_t __vpiHandle::free_object_fun(శూన్య)
{ తిరిగి &suppress_free; }

/*
 * The vpip_string function creates a constant string from the pass
 * input. This constant string is permanently allocated from an
 * efficient string buffer store.
 */
నిర్మాణం vpip_string_chunk {
      నిర్మాణం vpip_string_chunk*తదుపరి_2;
      అక్షరం దత్తాంశం[64*1024 - పరిమాణం (నిర్మాణం vpip_string_chunk*)];
};

గుర్తులేని vpip_కొలత(స్థిరరాశి __vpiSignal *sig)
{
      తిరిగి నిరపేక్ష(sig->msb.get_విలువ() - sig->lsb.get_విలువ()) + 1;
}

__vpiScope* vpip_scope(__vpiSignal*sig)
{
      ఒకవేళ (sig->is_netarray)
	    తిరిగి static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.మాతృ));
      లేకపోతే
	    తిరిగి sig->within.scope;
}

__vpiScope* vpip_scope(__vpiRealVar*sig)
{
      ఒకవేళ (sig->is_netarray)
	    తిరిగి static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.మాతృ));
      లేకపోతే
	    తిరిగి sig->within.scope;
}

vpiHandle vpip_module(__vpiScope*scope)
{
      అయితే(scope && scope->get_type_సంకేతం() != vpiModule) {
	    scope = scope->scope;
      }
      ప్రతిజ్ఞా(scope);
      తిరిగి scope;
}

స్థిరరాశి అక్షరం *vpip_string(స్థిరరాశి అక్షరం*str)
{
      స్థిర vpip_string_chunk మొదటి_chunk = {0, {0}};
      స్థిర vpip_string_chunk*chunk_list = &మొదటి_chunk;
      స్థిర గుర్తులేని chunk_fill = 0;

      గుర్తులేని len = అక్షరశ్రేణి_పొడవు(str);
      ప్రతిజ్ఞా( (len+1) <= పరిమాణం chunk_list->దత్తాంశం );

      ఒకవేళ ( (len+1) > (పరిమాణం chunk_list->దత్తాంశం - chunk_fill) ) {
	    vpip_string_chunk*tmp = కొత్త vpip_string_chunk;
	    tmp->తదుపరి_2 = chunk_list;
	    chunk_list = tmp;
	    chunk_fill = 0;
      }

      అక్షరం*res = chunk_list->దత్తాంశం + chunk_fill;
      chunk_fill += len + 1;

      strcpy(res, str);
      తిరిగి res;
}

స్థిర గుర్తులేని hash_string(స్థిరరాశి అక్షరం*పాఠ్యం)
{
      గుర్తులేని h = 0;

      అయితే (*పాఠ్యం) {
	    h = (h << 4) ^ (h >> 28) ^ (గుర్తులేని)*పాఠ్యం;
	    పాఠ్యం += 1;
      }
      తిరిగి h;
}

స్థిరరాశి అక్షరం* vpip_name_string(స్థిరరాశి అక్షరం*పాఠ్యం)
{
      స్థిరరాశి గుర్తులేని HASH_కొలత = 4096;
      స్థిర స్థిరరాశి అక్షరం*hash_పట్టిక[HASH_కొలత] = {0};

      గుర్తులేని hash_విలువ = hash_string(పాఠ్యం) % HASH_కొలత;

	/* If we easily find the string in the hash table, then return
	   that and be done. */
      ఒకవేళ (hash_పట్టిక[hash_విలువ]
	  && (strcmp(hash_పట్టిక[hash_విలువ], పాఠ్యం) == 0)) {
	    తిరిగి hash_పట్టిక[hash_విలువ];
      }

	/* The existing hash entry is not a match. Replace it with the
	   newly allocated value, and return the new pointer as the
	   result to the add. */
      స్థిరరాశి అక్షరం*res = vpip_string(పాఠ్యం);
      hash_పట్టిక[hash_విలువ] = res;

      తిరిగి res;
}

PLI_INT32 vpi_chk_error(p_vpi_error_info info)
{
      ఒకవేళ (vpip_చివరి_error.state == 0)
	    తిరిగి 0;

      info->state = vpip_చివరి_error.state;
      info->level = vpip_చివరి_error.level;
      info->message = vpip_చివరి_error.message;
      info->లబ్ధం = vpi_vlog_info.లబ్ధం;
      info->code = (అక్షరం *)"";
      info->file = 0;
      info->పంక్తి = 0;

      తిరిగి info->level;
}

PLI_INT32 vpi_compare_objects(vpiHandle obj1, vpiHandle obj2)
{
      ప్రతిజ్ఞా(obj1);
      ప్రతిజ్ఞా(obj2);

	// Does this actually work for all cases?
      ఒకవేళ (obj1 != obj2) తిరిగి 0;
      లేకపోతే తిరిగి 1;
}

/*
 * Copy the internal information to the data structure. Do not free or
 * change the tfname/user_data since they are a pointer to the real
 * string/data values. We also support passing a task or function handle
 * instead of just a handle to a vpiUserSystf.
 */
శూన్య vpi_get_systf_info(vpiHandle ref, p_vpi_systf_దత్తాంశం దత్తాంశం)
{
      నిర్మాణం __vpiUserSystf* rfp = dynamic_cast<__vpiUserSystf*>(ref);
      ఒకవేళ (rfp == 0) {
	    నిర్మాణం __vpiSysTaskCall*call = dynamic_cast<__vpiSysTaskCall*>(ref);
	    ప్రతిజ్ఞా(call);
	    rfp = call->defn;
      }

	/* Assert that vpiUserDefn is true! */
      ప్రతిజ్ఞా(rfp->is_user_defn);

      దత్తాంశం->type = rfp->info.type;
      దత్తాంశం->sysfunctype = rfp->info.sysfunctype;
      దత్తాంశం->tfname = rfp->info.tfname;
      దత్తాంశం->calltf = rfp->info.calltf;
      దత్తాంశం->compiletf = rfp->info.compiletf;
      దత్తాంశం->sizetf = rfp->info.sizetf;
      దత్తాంశం->user_దత్తాంశం = rfp->info.user_దత్తాంశం;
}

/*
 * When a task is called, this value is set so that vpi_handle can
 * fathom the vpi_handle(vpiSysTfCall,0) function.
 */
నిర్మాణం __vpiSysTaskCall*vpip_cur_task = 0;

PLI_INT32 vpi_free_object(vpiHandle ref)
{
      పూర్ణాంకం rtn;

      ఒకవేళ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_free_object(%p)", ref);
	    fflush(vpi_trace);
      }

      ప్రతిజ్ఞా(ref);
      __vpiHandle::free_object_fun_t fun = ref->free_object_fun();
      rtn = fun (ref);

      ఒకవేళ (vpi_trace)
	    fprintf(vpi_trace, " --> %d\n", rtn);

      తిరిగి rtn;
}

PLI_INT32 vpi_release_handle(vpiHandle ref)
{
	// Since SystemVerilog vpi_free_object() has been
	// renamed vpi_release_handle(), and thus
	// vpi_free_object() has been deprecated.
      తిరిగి vpi_free_object(ref);
}

స్థిర పూర్ణాంకం vpip_get_global(పూర్ణాంకం property)
{
      ఎంపిక (property) {

	  స్థితి vpi_సమయం_Unit:
	  స్థితి vpi_సమయం_Precision:
	    తిరిగి vpip_get_సమయం_precision();

	  అప్రమేయ:
	    fprintf(stderr, "vpi error: bad global property: %d\n", property);
	    ప్రతిజ్ఞా(0);
	    తిరిగి vpiUndefined;
      }
}

స్థిర స్థిరరాశి అక్షరం* vpi_property_str(PLI_INT32 code)
{
      స్థిర అక్షరం buf[32];
      ఎంపిక (code) {
	  స్థితి vpiConstType:
	    తిరిగి "vpiConstType";
	  స్థితి vpiName:
	    తిరిగి "vpiName";
	  స్థితి vpi_నిండిన_Name:
	    తిరిగి "vpiFullName";
	  స్థితి vpi_సమయం_Unit:
	    తిరిగి "vpiTimeUnit";
	  స్థితి vpi_సమయం_Precision:
	    తిరిగి "vpiTimePrecision";
          స్థితి vpi_కొలత:
	    తిరిగి "vpiSize";
          స్థితి _vpiNexusId:
	    తిరిగి "_vpiNexusId";
	  అప్రమేయ:
	    snprintf(buf, పరిమాణం(buf), "%d", (పూర్ణాంకం)code);
      }
      తిరిగి buf;
}

స్థిరరాశి అక్షరం* vpi_type_as_string(PLI_INT32 code)
{
      స్థిర అక్షరం buf[32];
      ఎంపిక (code) {
	  స్థితి vpiArrayType:
	    తిరిగి "vpiArrayType";
	  స్థితి vpi_బిట్టు_Var:
	    తిరిగి "vpiBitVar";
	  స్థితి vpiByteVar:
	    తిరిగి "vpiByteVar";
	  స్థితి vpiClassVar:
	    తిరిగి "vpiClassVar";
	  స్థితి vpiConstant:
	    తిరిగి "vpiConstant";
	  స్థితి vpiEnumTypespec:
	    తిరిగి "vpiEnumTypespec";
	  స్థితి vpiFunction:
	    తిరిగి "vpiFunction";
	  స్థితి vpiGenScope:
	    తిరిగి "vpiGenScope";
	  స్థితి vpiIntVar:
	    తిరిగి "vpiIntVar";
	  స్థితి vpiIntegerVar:
	    తిరిగి "vpiIntegerVar";
	  స్థితి vpiIterator:
	    తిరిగి "vpiIterator";
	  స్థితి vpiLongIntVar:
	    తిరిగి "vpiLongIntVar";
	  స్థితి vpi_జ్ఞాపకం:
	    తిరిగి "vpiMemory";
	  స్థితి vpi_జ్ఞాపకం_మాట:
	    తిరిగి "vpiMemoryWord";
	  స్థితి vpiModule:
	    తిరిగి "vpiModule";
	  స్థితి vpiNamedBegin:
	    తిరిగి "vpiNamedBegin";
	  స్థితి vpiNamedEvent:
	    తిరిగి "vpiNamedEvent";
	  స్థితి vpiNamedFork:
	    తిరిగి "vpiNamedFork";
	  స్థితి vpiPackage:
	    తిరిగి "vpiPackage";
	  స్థితి vpi_మార్గం_పదం:
	    తిరిగి "vpiPathTerm";
	  స్థితి vpi_పోర్టు:
	    తిరిగి "vpiPort";
	  స్థితి vpiNet:
	    తిరిగి "vpiNet";
	  స్థితి vpiNetArray:
	    తిరిగి "vpiNetArray";
	  స్థితి vpi_Net_బిట్టు:
	    తిరిగి "vpiNetBit";
	  స్థితి vpiParameter:
	    తిరిగి "vpiParameter";
	  స్థితి vpiPartSelect:
	    తిరిగి "vpiPartSelect";
	  స్థితి vpiRealVar:
	    తిరిగి "vpiRealVar";
	  స్థితి vpiReg:
	    తిరిగి "vpiReg";
	  స్థితి vpi_Reg_బిట్టు:
	    తిరిగి "vpiRegBit";
	  స్థితి vpiShortIntVar:
	    తిరిగి "vpiShortIntVar";
	  స్థితి vpiStringVar:
	    తిరిగి "vpiStringVar";
	  స్థితి vpiSysFuncCall:
	    తిరిగి "vpiSysFuncCall";
	  స్థితి vpiSysTaskCall:
	    తిరిగి "vpiSysTaskCall";
	  స్థితి vpiTask:
	    తిరిగి "vpiTask";
	  స్థితి vpi_సమయం_Var:
	    తిరిగి "vpiTimeVar";
	  స్థితి vpiUserSystf:
	    తిరిగి "vpiUserSystf";
	  అప్రమేయ:
	    snprintf(buf, పరిమాణం(buf), "%d", (పూర్ణాంకం)code);
      }
      తిరిగి buf;
}

PLI_INT32 vpi_get(పూర్ణాంకం property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      ఒకవేళ (property == _vpiDelaySelection) తిరిగి vpip_delay_selection;

      ఒకవేళ (ref == 0)
	    తిరిగి vpip_get_global(property);

      ఒకవేళ (property == vpiType) {
	    ఒకవేళ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_సంకేతం()));
	    }

	    ఒకవేళ (ref->get_type_సంకేతం() == vpi_జ్ఞాపకం && is_net_array(ref))
		  తిరిగి vpiNetArray;
	    లేకపోతే
		  తిరిగి ref->get_type_సంకేతం();
      }

      పూర్ణాంకం res = ref->vpi_get(property);

      ఒకవేళ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get(%s, %p) --> %d\n",
		    vpi_property_str(property), ref, res);
      }

      తిరిగి res;
}

PLI_INT64 vpi_get64(పూర్ణాంకం property, vpiHandle ref)
{
	// For now we only support getting the nexus id.
      ఒకవేళ ((ref == 0) || (property != _vpiNexusId)) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) is not currently supported.\n",
		    vpi_property_str(property), ref);
	    తిరిగి vpiUndefined;
      }

      int64_t res = ref->vpi_get64(property);

      ఒకవేళ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) --> %" PRId64 "\n",
		    vpi_property_str(property), ref, res);
      }

      తిరిగి res;
}

అక్షరం* vpi_get_str(PLI_INT32 property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      ఒకవేళ (property == _vpiDelaySelection) {
	    ఎంపిక (vpip_delay_selection) {
		స్థితి _vpiDelaySelMinimum:
		  తిరిగి simple_సమితి_rbuf_str("MINIMUM");
		స్థితి _vpiDelaySelTypical:
		  తిరిగి simple_సమితి_rbuf_str("TYPICAL");
		స్థితి vpi_Delay_Sel_గరిష్ఠ:
		  తిరిగి simple_సమితి_rbuf_str("MAXIMUM");
		అప్రమేయ:
		  ప్రతిజ్ఞా(0);
	    }
      }

      ఒకవేళ (ref == 0) {
	    fprintf(stderr, "vpi error: vpi_get_str(%s, 0) called "
		    "with null vpiHandle.\n", vpi_property_str(property));
	    తిరిగి 0;
      }

      ఒకవేళ (property == vpiType) {
	    ఒకవేళ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_సంకేతం()));
	    }

            PLI_INT32 type;
	    ఒకవేళ (ref->get_type_సంకేతం() == vpi_జ్ఞాపకం && is_net_array(ref))
		  type = vpiNetArray;
	    లేకపోతే
		  type = ref->get_type_సంకేతం();
	    తిరిగి (అక్షరం *)vpi_type_as_string(type);
      }

      అక్షరం*res = ref->vpi_get_str(property);

      ఒకవేళ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get_str(%s, %p) --> %s\n",
		    vpi_property_str(property), ref, res? res : "<NULL>");
      }

      తిరిగి res;
}

__vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj)
{
      స్థిరరాశి నిర్మాణం __vpiSysTaskCall*task;
      నిర్మాణం __vpiSignal*సంకేతం;
      నిర్మాణం __vpiRealVar*real;
      స్థిరరాశి __vpiNamedEvent*event;

      ఎంపిక (obj->get_type_సంకేతం()) {
	  స్థితి vpiSysTaskCall:
	    task = dynamic_cast<__vpiSysTaskCall*>(obj);
	    తిరిగి task->scope;

	  స్థితి vpiModule:
	    తిరిగి dynamic_cast<__vpiScope*>(obj);

	  స్థితి vpiNet:
	  స్థితి vpiReg:
	  స్థితి vpiIntegerVar:
	  స్థితి vpi_బిట్టు_Var:
	  స్థితి vpiByteVar:
	  స్థితి vpiShortIntVar:
	  స్థితి vpiIntVar:
	  స్థితి vpiLongIntVar:
	    సంకేతం = dynamic_cast<__vpiSignal*>(obj);
	    తిరిగి vpip_scope(సంకేతం);

	  స్థితి vpiRealVar:
	    real = dynamic_cast<__vpiRealVar*>(obj);
	    తిరిగి vpip_scope(real);

	  స్థితి vpiNamedEvent:
	    event = dynamic_cast<__vpiNamedEvent*>(obj);
	    తిరిగి event->get_scope();

	  స్థితి vpi_జ్ఞాపకం:
	  స్థితి vpi_జ్ఞాపకం_మాట:
	  స్థితి vpiPartSelect:
	    తిరిగి dynamic_cast<__vpiScope*>(obj->vpi_handle(vpiScope));

	  అప్రమేయ:
	    fprintf(stderr, "ERROR: vpip_scope_from_handle called with "
		    "object handle type=%d\n", obj->get_type_సంకేతం());
	    ప్రతిజ్ఞా(0);
	    తిరిగి 0;
      }
}

పూర్ణాంకం vpip_సమయం_units_from_handle(vpiHandle obj)
{
      ఒకవేళ (obj == 0)
	    తిరిగి vpip_get_సమయం_precision();

      స్థిరరాశి __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      ఒకవేళ (scope == 0)
	    తిరిగి vpip_get_సమయం_precision();

      తిరిగి scope->సమయం_units;
}

పూర్ణాంకం vpip_సమయం_precision_from_handle(vpiHandle obj)
{
      ఒకవేళ (obj == 0)
	    తిరిగి vpip_get_సమయం_precision();

      స్థిరరాశి __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      ఒకవేళ (scope == 0)
	    తిరిగి vpip_get_సమయం_precision();

      తిరిగి scope->సమయం_precision;
}

ద్విగుణ vpip_scaled_సమయం_from_handle(vvp_time64_t time, vpiHandle obj)
{
      పూర్ణాంకం scale = vpip_get_సమయం_precision() -
	          vpip_సమయం_units_from_handle(obj);
      ఒకవేళ (scale >= 0)
	    తిరిగి (ద్విగుణ)time * ఘాతం(10.0, scale);
      లేకపోతే
	    తిరిగి (ద్విగుణ)time / ఘాతం(10.0, -scale);
}

శూన్య vpi_get_సమయం(vpiHandle obj, s_vpi_సమయం*vp)
{
      vvp_time64_t time;

      ప్రతిజ్ఞా(vp);

      time = కాలపట్టిక_simtime();

      ఎంపిక (vp->type) {
          స్థితి vpi_Sim_సమయం:
	    vp->ఎగువ = (time >> 32) & 0xffffffff;
	    vp->దిగువ = time & 0xffffffff;
	    ఆపు;

          స్థితి vpi_Scaled_Real_సమయం:
	    vp->real = vpip_scaled_సమయం_from_handle(time, obj);
	    ఆపు;

          అప్రమేయ:
            fprintf(stderr, "vpi_get_time: unknown type: %d\n", (పూర్ణాంకం)vp->type);
            ప్రతిజ్ఞా(0);
	    ఆపు;
      }
}

PLI_INT32 vpi_get_vlog_info(p_vpi_vlog_info vlog_info_p)
{
    ఒకవేళ (vlog_info_p != 0) {
	  *vlog_info_p = vpi_vlog_info;
	  తిరిగి 1;
    } లేకపోతే {
	  తిరిగి 0;
    }
}

శూన్య vpi_సమితి_vlog_info(పూర్ణాంకం argc, అక్షరం** argv)
{
    స్థిర అక్షరం icarus_లబ్ధం[] = "Icarus Verilog";
    స్థిర అక్షరం icarus_సంస్కరణ[] = సంస్కరణ;
    vpi_vlog_info.లబ్ధం = icarus_లబ్ధం;
    vpi_vlog_info.సంస్కరణ_2 = icarus_సంస్కరణ;
    vpi_vlog_info.argc    = argc;
    vpi_vlog_info.argv    = argv;

    స్థిర అక్షరం trace_buf[1024];
    ఒకవేళ (స్థిరరాశి అక్షరం*మార్గం = getenv("VPI_TRACE"))  {
	  ఒకవేళ (!strcmp(మార్గం,"-"))
		vpi_trace = stdout;
	  లేకపోతే {
		vpi_trace = fopen(మార్గం, "w");
		ఒకవేళ (!vpi_trace) {
		      perror(మార్గం);
		      నిష్క్రమించు(1);
		}
		setvbuf(vpi_trace, trace_buf, _IOLBF, పరిమాణం(trace_buf));
	  }
    }
}

స్థిర శూన్య vec4_get_విలువ_string(స్థిరరాశి vvp_vector4_t&మాట_val, గుర్తులేని width,
				  s_vpi_విలువ*vp)
{
      గుర్తులేని nchar = width / 8;
      గుర్తులేని తోక = width % 8;

      అక్షరం*rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(nchar + 1, RBUF_VAL));
      అక్షరం*cp = rbuf;

      ఒకవేళ (తోక > 0) {
	    అక్షరం char_val = 0;
	    కొరకు (గుర్తులేని idx = width-తోక; idx < width ;  idx += 1) {
		  vvp_bit4_t val = మాట_val.value(idx);
		  ఒకవేళ (val == BIT4_1)
			char_val |= 1 << idx;
	    }

	    ఒకవేళ (char_val != 0)
		  *cp++ = char_val;
	    లేకపోతే ఒకవేళ (cp != rbuf)
		  *cp++ = ' ';
      }

      కొరకు (గుర్తులేని idx = 0 ;  idx < nchar ;  idx += 1) {
	    గుర్తులేని బిట్టు = (nchar - idx - 1) * 8;
	    అక్షరం char_val = 0;
	    కొరకు (గుర్తులేని bdx = 0 ;  bdx < 8 ;  bdx += 1) {
		  vvp_bit4_t val = మాట_val.value(బిట్టు+bdx);
		  ఒకవేళ (val == BIT4_1)
			char_val |= 1 << bdx;
	    }
	    // Ignore leading null-bytes and replace other null-bytes with space.
	    // The LRM is not entirely clear on how null bytes should be handled.
	    // This is the implementation chosen for iverilog.
	    ఒకవేళ (char_val != 0)
		  *cp++ = char_val;
	    లేకపోతే ఒకవేళ (cp != rbuf)
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
శూన్య vpip_vec4_get_విలువ(స్థిరరాశి vvp_vector4_t&మాట_val, గుర్తులేని width,
			 తార్కిక signed_flag, s_vpi_విలువ*vp)
{
      అక్షరం *rbuf = 0;

      ఎంపిక (vp->format) {
	  అప్రమేయ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector values.\n", (పూర్ణాంకం)vp->format);
	    ప్రతిజ్ఞా(0);

	  స్థితి vpiSuppressVal:
	    ఆపు;

	  స్థితి vpiBinStrVal:
	    rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(width+1, RBUF_VAL));
	    కొరకు (గుర్తులేని idx = 0 ;  idx < width ;  idx += 1) {
		  vvp_bit4_t బిట్టు = మాట_val.value(idx);
		  rbuf[width-idx-1] = vvp_bit4_గమ్యం_ascii(బిట్టు);
	    }
	    rbuf[width] = 0;
	    vp->value.str = rbuf;
	    ఆపు;

	  స్థితి vpiOctStrVal: {
		గుర్తులేని hwid = ((width+2) / 3) + 1;
		rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(hwid, RBUF_VAL));
		vpip_vec4_గమ్యం_oct_str(మాట_val, rbuf, hwid);
		vp->value.str = rbuf;
		ఆపు;
	  }

	  స్థితి vpiDecStrVal: {
// HERE need a better estimate.
		rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(width+1, RBUF_VAL));
		vpip_vec4_గమ్యం_dec_str(మాట_val, rbuf, width+1, signed_flag);
		vp->value.str = rbuf;
		ఆపు;
	  }

	  స్థితి vpiHexStrVal: {
		గుర్తులేని  hwid = ((width + 3) / 4) + 1;
		rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(hwid, RBUF_VAL));
		vpip_vec4_గమ్యం_hex_str(మాట_val, rbuf, hwid);
		vp->value.str = rbuf;
		ఆపు;
	  }

          స్థితి vpiScalarVal: {
	        // scalars should be of size 1
	        ప్రతిజ్ఞా(width == 1);
	        ఎంపిక(మాట_val.value(0)) {
		    స్థితి BIT4_0:
		      vp->value.scalar = vpi0;
		      ఆపు;
	            స్థితి BIT4_1:
                      vp->value.scalar = vpi1;
                      ఆపు;
	            స్థితి BIT4_X:
                      vp->value.scalar = vpiX;
                      ఆపు;
	            స్థితి BIT4_Z:
                      vp->value.scalar = vpiZ;
                      ఆపు;
		}
                ఆపు;
          }

	  స్థితి vpiIntVal: {
		దీర్ఘ val = 0;
		vvp_bit4_t pad = BIT4_0;
		ఒకవేళ (signed_flag && మాట_val.size() > 0)
		      pad = మాట_val.value(మాట_val.size()-1);

		కొరకు (గుర్తులేని idx = 0 ; idx < 8*పరిమాణం(val) ;  idx += 1) {
		      vvp_bit4_t val4 = pad;
		      ఒకవేళ (idx < మాట_val.size())
			    val4 = మాట_val.value(idx);
		      ఒకవేళ (val4 == BIT4_1)
			    val |= 1L << idx;
		}

		vp->value.integer = val;
		ఆపు;
	  }

	  స్థితి vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpi_సదిశ_Val;
	    // fallthrough
	  స్థితి vpi_సదిశ_Val: {
		గుర్తులేని hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_ఫలితం_buf(hwid * పరిమాణం(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		కొరకు (గుర్తులేని idx = 0 ;  idx < width ;  idx += 1) {
		      ఎంపిక (మాట_val.value(idx)) {
			  స్థితి BIT4_0:
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    ఆపు;
			  స్థితి BIT4_1:
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    ఆపు;
			  స్థితి BIT4_X:
			    op->aval |= (1 << idx % 32);
			    op->bval |= (1 << idx % 32);
			    ఆపు;
			  స్థితి BIT4_Z:
			    op->aval &= ~(1 << idx % 32);
			    op->bval |=  (1 << idx % 32);
			    ఆపు;
		      }
		      ఒకవేళ (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		ఆపు;
	  }

	  స్థితి vpiStringVal:
		vec4_get_విలువ_string(మాట_val, width, vp);
		ఆపు;

	  స్థితి vpiRealVal:
		vector4_గమ్యం_విలువ(మాట_val, vp->value.real, signed_flag);
		ఆపు;
      }
}

శూన్య vpip_vec2_get_విలువ(స్థిరరాశి vvp_vector2_t&మాట_val, గుర్తులేని width,
			 తార్కిక signed_flag, s_vpi_విలువ*vp)
{
      ఎంపిక (vp->format) {
	  అప్రమేయ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector2 values.\n", (పూర్ణాంకం)vp->format);
	    ప్రతిజ్ఞా(0);

	  స్థితి vpiSuppressVal:
	    ఆపు;

	  స్థితి vpiIntVal:
	    vector2_గమ్యం_విలువ(మాట_val, vp->value.integer, signed_flag);
	    ఆపు;

	  స్థితి vpiObjTypeVal:
	    vp->format = vpi_సదిశ_Val;
	    // fallthrough
	  స్థితి vpi_సదిశ_Val: {
		గుర్తులేని hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_ఫలితం_buf(hwid * పరిమాణం(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		కొరకు (గుర్తులేని idx = 0 ;  idx < width ;  idx += 1) {
		      ఒకవేళ (మాట_val.value(idx)) {
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      } లేకపోతే {
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      }
		      ఒకవేళ (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		ఆపు;
	  }
      }

}

/*
 * Convert a real value to the appropriate integer.
 */
స్థిర PLI_INT32 get_real_as_int(ద్విగుణ real)
{
      ద్విగుణ rtn;

	/* We would normally want to return 'bx for a NaN or
	 * +/- infinity, but for an integer the standard says
	 * to convert 'bx to 0, so we just return 0. */
      ఒకవేళ (real != real || (real && (real == 0.5*real))) {
	    తిరిగి 0;
      }

      rtn = std::విడత_2(real);

      తిరిగి (PLI_INT32) rtn;
}

/*
 * This is a generic function to convert a double value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer.
 */
శూన్య vpip_real_get_విలువ(ద్విగుణ real, s_vpi_విలువ*vp)
{
      అక్షరం *rbuf = 0;

      ఎంపిక (vp->format) {
	  అప్రమేయ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting real values.\n", (పూర్ణాంకం)vp->format);
	    ప్రతిజ్ఞా(0);

	  స్థితి vpiSuppressVal:
	    ఆపు;

	  స్థితి vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiRealVal;
	    // fallthrough
	  స్థితి vpiRealVal:
	    vp->value.real = real;
	    ఆపు;

	  స్థితి vpiIntVal:
	    vp->value.integer = get_real_as_int(real);
	    ఆపు;

	  స్థితి vpiDecStrVal:
	    rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(1025, RBUF_VAL));
	    vpip_vec4_గమ్యం_dec_str(vvp_vector4_t(1024, real), rbuf, 1025, నిజం);
	    vp->value.str = rbuf;
	    ఆపు;
      }
}

ద్విగుణ real_from_vpi_విలువ(s_vpi_విలువ*vp)
{
      vvp_vector4_t vec4(1024);
      ద్విగుణ ఫలితం;
      తార్కిక is_signed = అబద్ధం;

      ఎంపిక (vp->format) {
	  అప్రమేయ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "putting real values.\n", (పూర్ణాంకం)vp->format);
	    ప్రతిజ్ఞా(0);

	  స్థితి vpiRealVal:
	    ఫలితం = vp->value.real;
	    ఆపు;

	  స్థితి vpiIntVal:
	    ఫలితం = (ద్విగుణ) vp->value.integer;
	    ఆపు;

	  స్థితి vpiBinStrVal:
	    vpip_bin_str_గమ్యం_vec4(vec4, vp->value.str);
	    ఒకవేళ (vp->value.str[0] == '-') is_signed = నిజం;
	    vector4_గమ్యం_విలువ(vec4, ఫలితం, is_signed);
	    ఆపు;

	  స్థితి vpiOctStrVal:
	    vpip_oct_str_గమ్యం_vec4(vec4, vp->value.str);
	    ఒకవేళ (vp->value.str[0] == '-') is_signed = నిజం;
	    vector4_గమ్యం_విలువ(vec4, ఫలితం, is_signed);
	    ఆపు;

	  స్థితి vpiDecStrVal:
	    vpip_dec_str_గమ్యం_vec4(vec4, vp->value.str);
	    ఒకవేళ (vp->value.str[0] == '-') is_signed = నిజం;
	    vector4_గమ్యం_విలువ(vec4, ఫలితం, is_signed);
	    ఆపు;

	  స్థితి vpiHexStrVal:
	    vpip_hex_str_గమ్యం_vec4(vec4, vp->value.str);
	    ఒకవేళ (vp->value.str[0] == '-') is_signed = నిజం;
	    vector4_గమ్యం_విలువ(vec4, ఫలితం, is_signed);
	    ఆపు;

      }

      తిరిగి ఫలితం;
}

శూన్య vpip_string_get_విలువ(స్థిరరాశి string&val, s_vpi_విలువ*vp)
{
      అక్షరం *rbuf = 0;

      ఎంపిక (vp->format) {
	  అప్రమేయ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting string values.\n", (పూర్ణాంకం)vp->format);
	    ప్రతిజ్ఞా(0);

	  స్థితి vpiSuppressVal:
	    ఆపు;

	  స్థితి vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiStringVal;
	    // fallthrough
	  స్థితి vpiStringVal:
	    rbuf = static_cast<అక్షరం *>(need_ఫలితం_buf(val.size() + 1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    ఆపు;
      }
}


శూన్య vpi_get_విలువ(vpiHandle సమాసం, s_vpi_విలువ*vp)
{
      ప్రతిజ్ఞా(సమాసం);
      ప్రతిజ్ఞా(vp);

	// Never bother with suppressed values. All the derived
	// classes can ignore this type.
      ఒకవేళ (vp->format == vpiSuppressVal)
	    తిరిగి;

      సమాసం->vpi_get_విలువ(vp);

      ఒకవేళ (vpi_trace) ఎంపిక (vp->format) {
		స్థితి vpiStringVal:
		  fprintf(vpi_trace,"vpi_get_value(%p=<%d>) -> string=\"%s\"\n",
			  సమాసం, సమాసం->get_type_సంకేతం(), vp->value.str);
		  ఆపు;

		స్థితి vpiBinStrVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> binstr=%s\n",
			  సమాసం->get_type_సంకేతం(), vp->value.str);
		  ఆపు;

		స్థితి vpiIntVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> int=%d\n",
			  సమాసం->get_type_సంకేతం(), (పూర్ణాంకం)vp->value.integer);
		  ఆపు;

		స్థితి vpiSuppressVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <suppress>\n",
			  సమాసం->get_type_సంకేతం());
		  ఆపు;

		అప్రమేయ:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <%d>=?\n",
			  సమాసం->get_type_సంకేతం(), (పూర్ణాంకం)vp->format);
	    }
}

నిర్మాణం vpip_put_విలువ_event : vvp_gen_event_s {
      vpiHandle handle;
      s_vpi_విలువ value;
      పూర్ణాంకం flags;
      మాయా శూన్య నిరంతరం_నిరంతరం() override;
      ~vpip_put_విలువ_event() override { }
};

శూన్య vpip_put_విలువ_event::నిరంతరం_నిరంతరం()
{
      handle->vpi_put_విలువ(&value, flags);
      ఎంపిక (value.format) {
	    /* Free the copied string. */
	  స్థితి vpiBinStrVal:
	  స్థితి vpiOctStrVal:
	  స్థితి vpiDecStrVal:
	  స్థితి vpiHexStrVal:
	  స్థితి vpiStringVal:
	    విడుదల(value.value.str);
	    ఆపు;
	    /* Free the copied time structure. */
	  స్థితి vpi_సమయం_Val:
	    విడుదల(value.value.time);
	    ఆపు;
	    /* Free the copied vector structure. */
	  స్థితి vpi_సదిశ_Val:
	    విడుదల(value.value.vector);
	    ఆపు;
	    /* Free the copied strength structure. */
	  స్థితి vpiStrengthVal:
	    విడుదల(value.value.strength);
	    ఆపు;
	    /* Everything else is static in the structure. */
	  అప్రమేయ:
	    ఆపు;
      }
}

/* Make a copy of a pointer to a time structure. */
స్థిర t_vpi_సమయం *timedup(స్థిరరాశి t_vpi_సమయం *val)
{
      t_vpi_సమయం *rtn;
      rtn = static_cast<t_vpi_సమయం *> (జ్ఞాపకం_ఇవ్వు(పరిమాణం(t_vpi_సమయం)));
      *rtn = *val;
      తిరిగి rtn;
}

/* Make a copy of a pointer to a vector value structure. */
స్థిర t_vpi_vecval *vectordup(స్థిరరాశి t_vpi_vecval *val, PLI_INT32 size)
{
      గుర్తులేని అంకె_బైట్లు;
      t_vpi_vecval *rtn;
      ప్రతిజ్ఞా(size > 0);
      అంకె_బైట్లు = ((size + 31)/32)*పరిమాణం(t_vpi_vecval);
      rtn = static_cast<t_vpi_vecval *> (జ్ఞాపకం_ఇవ్వు(అంకె_బైట్లు));
      జ్ఞాపకం_నకలు(rtn, val, అంకె_బైట్లు);
      తిరిగి rtn;
}

/* Make a copy of a pointer to a strength structure. */
స్థిర t_vpi_strengthval *strengthdup(స్థిరరాశి t_vpi_strengthval *val)
{
      t_vpi_strengthval *rtn;
      rtn = static_cast<t_vpi_strengthval *>
            (జ్ఞాపకం_ఇవ్వు(పరిమాణం(t_vpi_strengthval)));
      *rtn = *val;
      తిరిగి rtn;
}

vpiHandle vpi_put_విలువ(vpiHandle obj, s_vpi_విలువ*vp,
			s_vpi_సమయం*when, PLI_INT32 flags)
{
      ప్రతిజ్ఞా(obj);

      flags &= ~vpiReturnEvent;

      ఒకవేళ (flags!=vpiNoDelay && flags!=vpiForceFlag && flags!=vpiReleaseFlag) {
	    vvp_time64_t dly;
	    పూర్ణాంకం scale;

	    ఒకవేళ (vpi_get(vpiAutomatic, obj)) {
		  fprintf(stderr, "VPI error: cannot put a value with "
				  "a delay on automatically allocated "
				  "variable '%s'.\n",
				  vpi_get_str(vpiName, obj));
		  తిరిగి 0;
	    }

	    ప్రతిజ్ఞా(when != 0);

	    ఎంపిక (when->type) {
		స్థితి vpi_Scaled_Real_సమయం:
		  scale = vpip_సమయం_units_from_handle(obj) -
		          vpip_get_సమయం_precision();
		  ఒకవేళ (scale >= 0) {
			dly = (vvp_time64_t)(when->real * ఘాతం(10.0, scale));
		  } లేకపోతే {
			dly = (vvp_time64_t)(when->real / ఘాతం(10.0, -scale));
		  }
		  ఆపు;
		స్థితి vpi_Sim_సమయం:
		  dly = vpip_timestruct_గమ్యం_సమయం(when);
		  ఆపు;
		అప్రమేయ:
		  dly = 0;
		  ఆపు;
	    }

	    ఒకవేళ ((dly == 0) && కాలపట్టిక_at_rosync()) {
		  fprintf(stderr, "VPI error: attempted to put a value to "
				  "variable '%s' during a read-only synch "
				  "callback.\n", vpi_get_str(vpiName, obj));
		  తిరిగి 0;
	    }

	    vpip_put_విలువ_event*put = కొత్త vpip_put_విలువ_event;
	    put->handle = obj;
	    ఒకవేళ (dynamic_cast<__vpiNamedEvent*>(obj)) {
		  put->value.format = vpiIntVal;
		  put->value.value.integer = 0;
	    } లేకపోతే {
		  ప్రతిజ్ఞా(vp);
		  put->value = *vp;
	    }
	      /* Since this is a scheduled put event we must copy any pointer
	       * data to keep it available until the event is actually run. */
	    ఎంపిక (put->value.format) {
		  /* Copy the string items. */
		స్థితి vpiBinStrVal:
		స్థితి vpiOctStrVal:
		స్థితి vpiDecStrVal:
		స్థితి vpiHexStrVal:
		స్థితి vpiStringVal:
		  put->value.value.str = strdup(put->value.value.str);
		  ఆపు;
		  /* Copy a time pointer item. */
		స్థితి vpi_సమయం_Val:
		  put->value.value.time = timedup(put->value.value.time);
		  ఆపు;
		  /* Copy a vector pointer item. */
		స్థితి vpi_సదిశ_Val:
		  put->value.value.vector = vectordup(put->value.value.vector,
		                                      vpi_get(vpi_కొలత, obj));
		  ఆపు;
		  /* Copy a strength pointer item. */
		స్థితి vpiStrengthVal:
		  put->value.value.strength =
		      strengthdup(put->value.value.strength);
		  ఆపు;
		  /* Everything thing else is already in the structure. */
		అప్రమేయ:
		  ఆపు;
	    }
	    put->flags = flags;
	    కాలపట్టిక_generic(put, dly, అబద్ధం, నిజం, నిజం);
	    తిరిగి 0;
      }

      ఒకవేళ (కాలపట్టిక_at_rosync()) {
            fprintf(stderr, "VPI error: attempted to put a value to "
			    "variable '%s' during a read-only synch "
			    "callback.\n", vpi_get_str(vpiName, obj));
            తిరిగి 0;
      }

      obj->vpi_put_విలువ(vp, flags);

      తిరిగి 0;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle res = 0;

      ఒకవేళ (ref == 0) {
	      // A few types can apply to a nil handle. These are ways
	      // that the current function can get started finding things.
	    ఎంపిక (type) {

		స్థితి vpiScope:
		    // The IEEE1364-2005 doesn't seem to allow this,
		    // but some users seem to think it's handy, so
		    // return the scope that contains this SysTfCall.
		  ప్రతిజ్ఞా(vpip_cur_task);
		  res = vpip_cur_task->vpi_handle(vpiScope);
		  ఆపు;

		స్థితి vpiSysTfCall:
		    // This is how VPI users get a first handle into
		    // the system. This is the handle of the system
		    // task/function call currently being executed.
		  ఒకవేళ (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle(vpiSysTfCall, 0) "
				"-> %p (%s)\n", vpip_cur_task,
				vpip_cur_task->defn->info.tfname);
		  }
		  తిరిగి vpip_cur_task;

		అప్రమేయ:
		  fprintf(stderr, "VPI error: vpi_handle(type=%d, ref=0).\n",
			  (పూర్ణాంకం)type);
		  res = 0;
		  ఆపు;
	    }

      } లేకపోతే {

	    ఒకవేళ (type == vpiSysTfCall) {
		  fprintf(stderr, "VPI error: vpi_handle(vpiSysTfCall, "
			  "ref!=0).\n");
		  తిరిగి 0;
	    }

	    res = ref->vpi_handle(type);
      }


      ఒకవేళ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle(vpiScope, ref=%p) "
		    "-> %p\n", vpip_cur_task, ref);
      }

      తిరిగి res;
}

స్థిర vpiHandle vpip_make_udp_iterator()
{
// HERE: Add support for iterating over UDP definitions.
//       See 26.6.16 (page 400 in 1364-2005).
      తిరిగి 0;
}

/*
 * This function asks the object to return an iterator for
 * the specified reference. It is up to the iterate_ method to
 * allocate a properly formed iterator.
 */
స్థిర vpiHandle vpi_iterate_global(పూర్ణాంకం type)
{
      ఎంపిక (type) {
	  స్థితి vpiInstance:
	    // fallthrough
	  స్థితి vpiModule:
	    // fallthrough
	  స్థితి vpi_కార్యక్రమం:
	    // fallthrough
	  స్థితి vpiInterface:
	    // fallthrough
	  స్థితి vpiPackage:
	    తిరిగి vpip_make_మూలం_iterator(type);

	  స్థితి vpiUdpDefn:
	    తిరిగి vpip_make_udp_iterator();

	  స్థితి vpiUserSystf:
	    తిరిగి vpip_make_systf_iterator();
      }

      తిరిగి 0;
}

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle rtn = 0;

      ప్రతిజ్ఞా(vpi_బహుళకం_flag != VPI_బహుళకం_NONE);
      ఒకవేళ (vpi_బహుళకం_flag == VPI_బహుళకం_రిజిస్టరు) {
	    fprintf(stderr, "vpi error: vpi_iterate called during "
		    "vpi_register_systf. You can't do that!\n");
	    తిరిగి 0;
      }

      ఒకవేళ (ref == 0)
	    rtn = vpi_iterate_global(type);
      లేకపోతే
	    rtn = ref->vpi_iterate(type);

      ఒకవేళ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_iterate(%d, %p) ->%s\n",
	    (పూర్ణాంకం)type, ref, rtn ? "" : " (null)");
      }

      తిరిగి rtn;
}

vpiHandle vpi_handle_by_సూచీ(vpiHandle ref, PLI_INT32 idx)
{
      ప్రతిజ్ఞా(ref);
      తిరిగి ref->vpi_సూచీ(idx);
}

స్థిర vpiHandle కనుగొను_name(స్థిరరాశి అక్షరం *name, vpiHandle handle)
{
      vpiHandle rtn = 0;
      __vpiScope*ref = dynamic_cast<__vpiScope*>(handle);

      /* check module names */
      ఒకవేళ (!strcmp(name, vpi_get_str(vpiName, handle)))
	    rtn = handle;

      /* brute force search for the name in all objects in this scope */
      కొరకు (గుర్తులేని i = 0 ;  i < ref->intern.size() ;  i += 1) {
	      /* The standard says that since a port does not have a full
	       * name it cannot be found by name. Because of this we need
	       * to skip ports here so the correct handle can be located. */
	    ఒకవేళ (vpi_get(vpiType, ref->intern[i]) == vpi_పోర్టు) కొనసాగు;
	    స్థిరరాశి అక్షరం *nm = vpi_get_str(vpiName, ref->intern[i]);
	    ఒకవేళ (nm && !strcmp(name, nm)) {
		  rtn = ref->intern[i];
		  ఆపు;
	    } లేకపోతే ఒకవేళ (vpi_get(vpiType, ref->intern[i]) == vpi_జ్ఞాపకం ||
	               vpi_get(vpiType, ref->intern[i]) == vpiNetArray) {
		  /* We need to iterate on the words */
		  vpiHandle మాట_i, మాట_h;
		  మాట_i = vpi_iterate(vpi_జ్ఞాపకం_మాట, ref->intern[i]);
		  అయితే (మాట_i && (మాట_h = vpi_పరిశీలన(మాట_i))) {
			nm = vpi_get_str(vpiName, మాట_h);
			ఒకవేళ (nm && !strcmp(name, nm)) {
			      rtn = మాట_h;
			      vpi_free_object(మాట_i);
			      ఆపు;
			}
		  }
	    }
	    /* found it yet? */
	    ఒకవేళ (rtn) ఆపు;
      }

      తిరిగి rtn;
}

// Find the end of the escaped identifier or simple identifier
స్థిర అక్షరం * కనుగొను_rest(అక్షరం *name)
{
      అక్షరం *rest;
      ఒకవేళ (*name == '\\') {
	    rest = strchr(name, ' ');
	    ఒకవేళ (rest) {
		  *rest++ = 0; // The space is not part of the string
		    // There should be a '.' after the escaped ID if there is
		    // anything more. If it is missing add it to avoid a crash
		  ఒకవేళ ((*rest != '.') && (*rest != 0)) {
			*--rest = '.';
			fprintf(stderr, "ERROR: Malformed scope string: \"%s\"", name);
		  }
		  ఒకవేళ (*rest == 0) {
			rest = 0;
		  }
	    }
      } లేకపోతే {
	    rest = strchr(name, '.');
      }
      తిరిగి rest;
}

స్థిర vpiHandle కనుగొను_scope(స్థిరరాశి అక్షరం *name, vpiHandle handle, పూర్ణాంకం లోతు)
{

      vpiHandle iter = handle==0
	    ? vpi_iterate(vpiModule, ఖాళీ)
	    : vpi_iterate(vpi_అంతర్గత_Scope, handle);

      vector<అక్షరం> name_buf (అక్షరశ్రేణి_పొడవు(name)+1);
      strcpy(&name_buf[0], name);
      అక్షరం*nm_మొదటి = &name_buf[0];
      అక్షరం*nm_rest = కనుగొను_rest(nm_మొదటి);
      ఒకవేళ (*nm_మొదటి == '\\') ++nm_మొదటి;
      ఒకవేళ (nm_rest) {
	    *nm_rest++ = 0;
      }

      vpiHandle rtn = 0;
      vpiHandle hand;
      అయితే (iter && (hand = vpi_పరిశీలన(iter))) {
	    స్థిరరాశి అక్షరం *nm = vpi_get_str(vpiName, hand);

	    ఒకవేళ (strcmp(nm_మొదటి,nm)==0) {
		  ఒకవేళ (nm_rest)
			rtn=కనుగొను_scope(nm_rest, hand, లోతు+1);
		  లేకపోతే
			rtn = hand;
	    }

	    /* found it yet ? */
	    ఒకవేళ (rtn) {
		  vpi_free_object(iter);
		  ఆపు;
	    }
      }

      తిరిగి rtn;
}

// Find the end of the first escaped identifier or simple identifier
స్థిర అక్షరం * కనుగొను_తదుపరి(అక్షరం *name)
{
      అక్షరం *తదుపరి_2;
      ఒకవేళ (*name == '\\') {
	    తదుపరి_2 = strchr(name, ' ');
	    ఒకవేళ (తదుపరి_2 && *++తదుపరి_2 == 0) తదుపరి_2 = 0;
      } లేకపోతే {
	    తదుపరి_2 = strchr(name, '.');
      }
      తిరిగి తదుపరి_2;
}

vpiHandle vpi_handle_by_name(స్థిరరాశి అక్షరం *name, vpiHandle scope)
{
      vpiHandle hand;

      ఒకవేళ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name(%s, %p) -->\n",
		    name, scope);
      }

	// Chop the name into path and base. For example, if the name
	// is "a.b.c", then nm_path becomes "a.b" and nm_base becomes
	// "c". If the name is "c" then nm_path is nil and nm_base is "c".
      vector<అక్షరం> name_buf (అక్షరశ్రేణి_పొడవు(name)+1);
      strcpy(&name_buf[0], name);
      అక్షరం*nm_మార్గం = &name_buf[0];
      అక్షరం*nm_పునాది;

	// Check to see if we have an escaped identifier and if so search
	// the long way because a '.' could be in the escaped name.
      ఒకవేళ (strchr(nm_మార్గం, '\\')) {
	    అక్షరం *తదుపరి_2;
	    nm_పునాది = nm_మార్గం;
	    అయితే ((తదుపరి_2 = కనుగొను_తదుపరి(nm_పునాది))) {
		  nm_పునాది = ++తదుపరి_2;
	    }
	    ఒకవేళ (nm_మార్గం == nm_పునాది) {
		  nm_మార్గం = 0;
	    } లేకపోతే {
		  *(nm_పునాది-1) = 0;
	    }

	// If there is no escaped identifier then just look for the last '.'
      } లేకపోతే {
	    nm_పునాది = strrchr(nm_మార్గం, '.');
	    ఒకవేళ (nm_పునాది) {
		  *nm_పునాది++ = 0;
	    } లేకపోతే {
		  nm_పునాది = nm_మార్గం;
		  nm_మార్గం = 0;
	    }
      }

      /* If scope provided, look in corresponding module; otherwise
       * traverse the hierarchy specified in name to find the leaf module
       * and try finding it there.
       */
      ఒకవేళ (scope) {
	    /* Some implementations support either a module or a scope. */
	    ఎంపిక (vpi_get(vpiType, scope)) {
		స్థితి vpiScope:
	          hand = vpi_handle(vpiModule, scope);
	          ఆపు;
		స్థితి vpiModule:
	          hand = scope;
	          ఆపు;
		అప్రమేయ:
		  ఒకవేళ (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle_by_name: "
				"Scope is not a vpiScope or vpiModule\n");
		  }
	          // Use vpi_chk_error() here when it is implemented.
	          తిరిగి 0;
	    }

      } లేకపోతే ఒకవేళ (nm_మార్గం) {
	      // The name has a path, and no other scope handle was
	      // passed in. That suggests we are looking for "a.b.c"
	      // in the root scope. So convert "a.b" to a scope and
	      // start there to look for "c".
	    hand = కనుగొను_scope(nm_మార్గం, ఖాళీ, 0);
	    nm_మార్గం = 0;

      } లేకపోతే {
	      // Special case: scope==<nil>, meaning we are looking in
	      // the root, and there is no path to the name, i.e. the
	      // string is "c" instead of "top.c". Try to find "c" as
	      // a scope and return that.
	    hand = కనుగొను_scope(nm_పునాది, ఖాళీ, 0);
      }

      ఒకవేళ (hand == 0) {
	    ఒకవేళ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_handle_by_name: "
			  "Scope does not exist. Giving up.\n");
	    }

	    తిరిగి 0;
      }

	// If there is a path part, then use it to find the
	// scope. For example, if the full name is a.b.c, then
	// the nm_path string is a.b and we search for that
	// scope. If we find it, then set hand to that scope.
      ఒకవేళ (nm_మార్గం) {
	    vpiHandle tmp = కనుగొను_scope(nm_మార్గం, hand, 0);
	    అయితే (tmp == 0 && hand != 0) {
		  hand = vpi_handle(vpiScope, hand);
		  tmp = కనుగొను_scope(nm_మార్గం, hand, 0);
	    }
	    hand = tmp;
      }

	// find_name() expects escaped identifiers to be stripped
      ఒకవేళ (*nm_పునాది == '\\') {
	    // Skip the \ at the beginning
	    ++nm_పునాది;

	    // Drop the space at the end if it exists
	    అక్షరం *తదుపరి_2;
	    ఒకవేళ ((తదుపరి_2 = strchr(nm_పునాది, ' '))) {
		  *తదుపరి_2 = 0;
	    }
      }

	// Now we have the correct scope, look for the item.
      vpiHandle out = కనుగొను_name(nm_పునాది, hand);

      ఒకవేళ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name: DONE\n");
      }

      తిరిగి out;
}

// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected
// x2: Ptr to previous node
// x3: True when net is connected to output of previous node
tuple<తార్కిక, vvp_net_ptr_t, తార్కిక> check_connected_గమ్యం_concat8_and_part_sa(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t మాతృ) {

      vvp_net_ptr_t prev = vvp_net_ptr_t(శూన్యసూచిక, 0);

      అయితే (cur.ptr()) {
	    ఒకవేళ (cur.ptr() == net2 && dynamic_cast<vvp_fun_part_sa*>(మాతృ.ptr()->fun)) {
		    // Pass on the return value if found
		  తిరిగి {నిజం, prev.ptr() ? prev : మాతృ, prev.ptr() == శూన్యసూచిక};
	    } లేకపోతే ఒకవేళ (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  స్వయంచాలక res = check_connected_గమ్యం_concat8_and_part_sa(cur.ptr()->బహిర్గతం_2, net2, cur);
		  ఒకవేళ (get<0>(res)) {
			తిరిగి res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->పోర్టు[cur.పోర్టు()];
      }

	// net2 is not connected to this concat8
      తిరిగి {అబద్ధం, vvp_net_ptr_t(శూన్యసూచిక, 0), అబద్ధం};
}

// Check if net2 is connected to current_net through a net of vvp_fun_concat8s
// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected with concats and part_sas
// x2: Ptr to previous node
// x3: True when net is connected to output of x2
tuple<తార్కిక, vvp_net_ptr_t, తార్కిక> check_connected_గమ్యం_concat8(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t మాతృ)
{
      vvp_net_ptr_t prev = vvp_net_ptr_t(శూన్యసూచిక, 0);
	// For everything connected
      అయితే (cur.ptr()) {
	      // Check if it's a concat8
	    ఒకవేళ (cur.ptr() == net2) {
		  తిరిగి {నిజం, vvp_net_ptr_t(శూన్యసూచిక, 0), అబద్ధం};
	    } లేకపోతే ఒకవేళ (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  స్వయంచాలక res = check_connected_గమ్యం_concat8(cur.ptr()->బహిర్గతం_2, net2, cur);
		  ఒకవేళ (get<0>(res)) {
			ఒకవేళ (!get<1>(res).ptr() && dynamic_cast<vvp_fun_part_sa*>(మాతృ.ptr()->fun)) {
			      // `parent` is the last part_sa node in front of our destination
			      // We want to place the intermod node here
			      తిరిగి {నిజం, prev.ptr() ? prev : మాతృ, prev.ptr() == శూన్యసూచిక};
			}
			తిరిగి res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->పోర్టు[cur.పోర్టు()];
      }

	// net2 is not connected to this concat8
      తిరిగి {అబద్ధం, vvp_net_ptr_t(శూన్యసూచిక, 0), అబద్ధం};
}

// Used to get intermodpath for two ports
vpiHandle vpi_handle_multi(PLI_INT32 type,
                           vpiHandle ref1,
                           vpiHandle ref2)
{
      ఒకవేళ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_multi(%d, %p, %p) -->\n",
	            type, ref1, ref2);
      }

      ఒకవేళ (type != vpi_Inter_Mod_మార్గం) {
	    fprintf(stderr, "sorry: vpi_handle_multi currently supports"
	            "only vpiInterModPath\n");
	    తిరిగి శూన్యసూచిక;
      }

	// Indicates whether port1 refers to a single bit
      తార్కిక port1_has_సూచీ = అబద్ధం;
      పూర్ణాంకం port1_బిట్టు_సూచీ = 0;
      vpi_పోర్టు_బిట్టు_Info* port1_బిట్టు = dynamic_cast<vpi_పోర్టు_బిట్టు_Info*>(ref1);

      ఒకవేళ (port1_బిట్టు) {
	      // Get the bit index
	    port1_has_సూచీ = నిజం;
	    port1_బిట్టు_సూచీ = vpi_get(vpi_బిట్టు, port1_బిట్టు);
	      // Update the ref1 to point to the base port
	    ref1 = vpi_handle(vpi_మాతృ, port1_బిట్టు);
      }

	// Indicates whether port2 refers to a single bit
      తార్కిక port2_has_సూచీ = అబద్ధం;
      పూర్ణాంకం port2_బిట్టు_సూచీ = 0;
      vpi_పోర్టు_బిట్టు_Info* port2_బిట్టు = dynamic_cast<vpi_పోర్టు_బిట్టు_Info*>(ref2);

      ఒకవేళ (port2_బిట్టు) {
	      // Get the bit index
	    port2_has_సూచీ = నిజం;
	    port2_బిట్టు_సూచీ = vpi_get(vpi_బిట్టు, port2_బిట్టు);
	      // Update the ref1 to point to the base port
	    ref2 = vpi_handle(vpi_మాతృ, port2_బిట్టు);
      }

      vpi_పోర్టు_Info* port1 = dynamic_cast<vpi_పోర్టు_Info*>(ref1);

      ఒకవేళ (!port1) {
	    fprintf(stderr, "sorry: second argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    తిరిగి శూన్యసూచిక;
      }

      vpi_పోర్టు_Info* port2 = dynamic_cast<vpi_పోర్టు_Info*>(ref2);

      ఒకవేళ (!port2) {
	    fprintf(stderr, "sorry: third argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    తిరిగి శూన్యసూచిక;
      }

	// Get the names of both ports
      std::string port1_name(vpi_get_str(vpiName, ref1));
      std::string port2_name(vpi_get_str(vpiName, ref2));

	// If both ports are vpiOutput, we have to reassign the __vpiSignal from port1
	// to port2 because otherwise the non-delayed version of the signal is dumped
	// even tho the intermodpath is correctly inserted
      __vpiSignal* output_signal = శూన్యసూచిక;

      ఒకవేళ (port1->get_direction() == vpiOutput && port2->get_direction() == vpiOutput) {
	    vpiHandle scope_port2 = vpi_handle(vpiScope, ref2);
	    ప్రతిజ్ఞా(scope_port2);

	      // Iterate over nets in the scope of port2
	    vpiHandle net_i = vpi_iterate(vpiNet, scope_port2) ;
	    vpiHandle net;

	    అయితే ((net = vpi_పరిశీలన(net_i)) != ఖాళీ) {
		  std::string net_name(vpi_get_str(vpiName, net));

		    // Compare whether the net matches with the port name
		  ఒకవేళ (net_name == port2_name) {
			output_signal = dynamic_cast<__vpiSignal*>(net);
		  }
	    }
      }

      vvp_net_t* net1 = port1->get_పోర్టు();
      vvp_net_t* net2 = port2->get_పోర్టు();

      ఒకవేళ (net1 == శూన్యసూచిక || net2 == శూన్యసూచిక) {
	    fprintf(stderr, "Error: Could not find net. "
		    "Did you run iverilog with '-ginterconnect'?\n");
	    తిరిగి శూన్యసూచిక;
      }

      ఒకవేళ (net1 == net2) {
	    fprintf(stderr, "Error: Net for both ports is the same. "
		    "Did you pass the same port twice?\n");
	    తిరిగి శూన్యసూచిక;
      }

      ఒకవేళ (!dynamic_cast<vvp_fun_buft*>(net1->fun)) {
	    fprintf(stderr, "Error: functor of net1 must be"
		    "vvp_fun_buft\n");
	    తిరిగి శూన్యసూచిక;
      }

      ఒకవేళ (!dynamic_cast<vvp_fun_buft*>(net2->fun)) {
	    fprintf(stderr, "Error: functor of net2 must be"
		    "vvp_fun_buft\n");
	    తిరిగి శూన్యసూచిక;
      }

	// If port1 is actually a port bit, we have to get to the correct vvp_fun_part
	// after which we insert the intermodpath delay
      ఒకవేళ (port1_has_సూచీ) {
	    vvp_net_ptr_t* net1_ptr = &net1->బహిర్గతం_2;

	      // Search for part selects connected to port1
	    vvp_net_t* current_net = net1_ptr->ptr();

	    అయితే (current_net) {
		  స్థిరరాశి vvp_fun_part* part = dynamic_cast<vvp_fun_part*>(current_net->fun);

		    // Its a part select!
		  ఒకవేళ (part) {
			  // Is it the correct part select?
			ఒకవేళ (part->get_పునాది() == (గుర్తులేని)port1_బిట్టు_సూచీ) {
			      ప్రతిజ్ఞా(part->get_wid() == 1);
			      net1 = current_net; // Replace net1 as this is our new start point
			      ఆపు;
			}
		  }

		  current_net = current_net->పోర్టు[0].ptr(); // BUFT has only one input, index 0
	    }
      }

	// Iterate over all nodes connected to port1
      vvp_net_ptr_t cur = net1->బహిర్గతం_2;
      vvp_net_ptr_t prev = vvp_net_ptr_t(శూన్యసూచిక, 0);

      అయితే (cur.ptr()) {
	      // Either port2 is directly connected to port1
	      // Or in the second case port2 is indirectly connected
	      // to port1 through a net of concat8s
	    తార్కిక is_connected, is_output;
	    vvp_net_ptr_t మునుపటి_కణుపు;
	    tie(is_connected, మునుపటి_కణుపు, is_output) = port2_has_సూచీ
		  ? check_connected_గమ్యం_concat8(cur, net2, vvp_net_ptr_t(net1, 0))
		  : check_connected_గమ్యం_concat8_and_part_sa(cur, net2, vvp_net_ptr_t(net1, 0));
	    ఒకవేళ ( (!port2_has_సూచీ && cur.ptr() == net2) || is_connected ) {
		  vvp_net_t*new_net = కొత్త vvp_net_t;

		    // Create new node with intermodpath and connect port2 to it
		  పూర్ణాంకం width = 1; // TODO
		  vvp_fun_intermodpath*obj = కొత్త vvp_fun_intermodpath(new_net, width);

		  vvp_net_t* output_net = net1;
		  ఒకవేళ (is_connected) {
			ఒకవేళ (is_output) {
			      output_net = మునుపటి_కణుపు.ptr(); // net2 is direct output of part_sa
			      cur = output_net->బహిర్గతం_2;
			      prev = vvp_net_ptr_t(శూన్యసూచిక, 0);
			} లేకపోతే {
			      prev = మునుపటి_కణుపు;
			      ఒకవేళ (prev.ptr()) {
				    cur = prev.ptr()->పోర్టు[prev.పోర్టు()];
			      }
			}
		  }

		  new_net->fun = obj;
		  new_net->బహిర్గతం_2 = cur;

		    // Port2 is in the middle of the list
		    // Insert intermodpath before port2 and keep everything else intact
		  ఒకవేళ (prev.ptr()) {
			prev.ptr()->పోర్టు[prev.పోర్టు()] = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->పోర్టు[0] = cur.ptr()->పోర్టు[cur.పోర్టు()]; // Connect the next net in list
			cur.ptr()->పోర్టు[cur.పోర్టు()] = vvp_net_ptr_t(శూన్యసూచిక, 0); // Only port2 is connected to intermodpath
		    // Port2 is first in list
		    // Insert intermodpath before port2 and keep everything else intact
		  } లేకపోతే {
			output_net->బహిర్గతం_2 = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->పోర్టు[0] = cur.ptr()->పోర్టు[cur.పోర్టు()]; //  Connect the next net in list
			cur.ptr()->పోర్టు[cur.పోర్టు()] = vvp_net_ptr_t(శూన్యసూచిక, 0); // Only port2 is connected to intermodpath
		  }

		    // If both ports are vpiOutput and port2 is not a vector,
		    // we have to reassign the __vpiSignal so that the delayed
		    // values get dumped
		  ఒకవేళ (output_signal && !port2_has_సూచీ) {
			net2->fil = net1->fil;
			net1->fil = శూన్యసూచిక;
			output_signal->కణుపు = net2;
		  }

		    // Create the VPI intermodpath object
		  __vpiInterModPath* intermodpath = vpip_make_intermodpath(new_net, port1, port2);
		  intermodpath->intermodpath = obj;

		    // Finally done, return the intermodpath object
		  తిరిగి intermodpath;
	    }

	    prev = cur;
	    cur = cur.ptr()->పోర్టు[cur.పోర్టు()]; // Next net in linked list
      }

      fprintf(stderr, "VPI error: Could not insert intermodpath!\n");
      fprintf(stderr, "\tport1 = %s, port1_has_index = %d, port1_bit_index = %d\n", port1_name.c_str(), port1_has_సూచీ, port1_బిట్టు_సూచీ);
      fprintf(stderr, "\tport2 = %s, port2_has_index = %d, port2_bit_index = %d\n", port2_name.c_str(), port2_has_సూచీ, port2_బిట్టు_సూచీ);
      తిరిగి శూన్యసూచిక;
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



శూన్య vpi_get_delays(vpiHandle సమాసం, p_vpi_delay delays)
{
      ప్రతిజ్ఞా(సమాసం);
      ప్రతిజ్ఞా(delays);

      సమాసం->vpi_get_delays(delays);

      ఒకవేళ (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_get_delays(%p, %p) -->\n", సమాసం, delays);
      }
}


శూన్య vpi_put_delays(vpiHandle సమాసం, p_vpi_delay delays)
{
      ప్రతిజ్ఞా(సమాసం  );
      ప్రతిజ్ఞా(delays );

      సమాసం->vpi_put_delays(delays);

      ఒకవేళ (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_put_delays(%p, %p) -->\n", సమాసం, delays);
      }
}








బాహ్య "C" PLI_INT32 vpi_vprintf(స్థిరరాశి అక్షరం*fmt, va_list ap)
{
      తిరిగి vpi_mcd_vprintf(1, fmt, ap);
}

బాహ్య "C" PLI_INT32 vpi_printf(స్థిరరాశి అక్షరం *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      పూర్ణాంకం r = vpi_mcd_vprintf(1, fmt, ap);
      va_end(ap);
      తిరిగి r;
}

బాహ్య "C" PLI_INT32 vpi_flush(శూన్య)
{
      తిరిగి vpi_mcd_flush(1);
}


బాహ్య "C" PLI_INT32 vpi_sim_vcontrol(పూర్ణాంకం operation, va_list ap)
{
      దీర్ఘ diag_సందేశం;

      ఎంపిక (operation) {
	  స్థితి vpi_ముగింపు:
	  స్థితి __ivl_legacy_vpiFinish:
            diag_సందేశం = va_arg(ap, PLI_INT32);
	    కాలపట్టిక_ముగింపు(diag_సందేశం);
	    ఆపు;

	  స్థితి vpiStop:
	  స్థితి __ivl_legacy_vpiStop:
            diag_సందేశం = va_arg(ap, PLI_INT32);
	    కాలపట్టిక_stop(diag_సందేశం);
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "Unsupported VPI control operation %d.\n", operation);
	    తిరిగి 0;
      }
      తిరిగి 1;
}

బాహ్య "C" PLI_INT32 vpi_sim_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      పూర్ణాంకం r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      తిరిగి r;
}

బాహ్య "C" PLI_INT32 vpi_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      పూర్ణాంకం r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      తిరిగి r;
}

/*
 * This routine calculated the return value for $clog2.
 * It is easier to do it here vs trying to to use the VPI interface.
 */
బాహ్య "C" s_vpi_vecval vpip_calc_clog2(vpiHandle arg)
{
      s_vpi_vecval rtn;
      s_vpi_విలువ val;
      vvp_vector4_t vec4;
      తార్కిక is_neg = అబద్ధం;  // At this point only a real can be negative.

	/* Get the value as a vvp_vector4_t. */
      val.format = vpiObjTypeVal;
      vpi_get_విలువ(arg, &val);
      ఒకవేళ (val.format == vpiRealVal) {
	    vpi_get_విలువ(arg, &val);
	      /* All double values can be represented in 1024 bits. */
	    vec4 = vvp_vector4_t(1024, val.value.real);
	    ఒకవేళ (val.value.real < 0) is_neg = నిజం;
      } లేకపోతే {
	    val.format = vpi_సదిశ_Val;
	    vpi_get_విలువ(arg, &val);
	    గుర్తులేని wid = vpi_get(vpi_కొలత, arg);
	    vec4 = vvp_vector4_t(wid, BIT4_0);
	    కొరకు (గుర్తులేని idx=0; idx < wid; idx += 1) {
		PLI_INT32 aval = val.value.vector[idx/32].aval;
		PLI_INT32 bval = val.value.vector[idx/32].bval;
		aval >>= idx % 32;
		bval >>= idx % 32;
		పూర్ణాంకం bitmask = (aval&1) | ((bval<<1)&2);
		vvp_bit4_t బిట్టు = scalar_గమ్యం_bit4(bitmask);
		vec4.సమితి_బిట్టు(idx, బిట్టు);
	    }
      }

      ఒకవేళ (vec4.has_xz()) {
	    rtn.aval = rtn.bval = 0xFFFFFFFFU;  /* Set to 'bx. */
	    తిరిగి rtn;
      }

      vvp_vector2_t vec2(vec4);

      ఒకవేళ (is_neg) vec2.trim_neg(); /* This is a special trim! */
      లేకపోతే vec2.trim(); /* This makes less work shifting. */

	/* Calculate the clog2 result. */
      PLI_INT32 res = 0;
      ఒకవేళ (!vec2.is_zero()) {
	    vec2 -= vvp_vector2_t(1, vec2.size());
	    అయితే(!vec2.is_zero()) {
		  res += 1;
		  vec2 >>= 1;
	    }
      }

      rtn.aval = res;
      rtn.bval = 0;
      తిరిగి rtn;
}

/*
 * This routine provides the information needed to implement $countdrivers.
 * It is done here for performance reasons - interrogating the drivers
 * individually via the VPI interface would be much slower.
 */
బాహ్య "C" శూన్య vpip_లెక్క_drivers(vpiHandle ref, గుర్తులేని idx,
                                   గుర్తులేని counts[4])
{
      నిర్మాణం __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      ప్రతిజ్ఞా(rfp);
      rfp->కణుపు->లెక్క_drivers(idx, counts);
}

#ఒకవేళ defined(__MINGW32__) || defined (__CYGWIN__)
vpip_routines_s vpi_routines = {
    .రిజిస్టరు_cb                = vpi_రిజిస్టరు_cb,
    .remove_cb                  = vpi_remove_cb,
    .రిజిస్టరు_systf             = vpi_రిజిస్టరు_systf,
    .get_systf_info             = vpi_get_systf_info,
    .handle_by_name             = vpi_handle_by_name,
    .handle_by_సూచీ            = vpi_handle_by_సూచీ,
    .handle_multi               = vpi_handle_multi,
    .handle                     = vpi_handle,
    .iterate                    = vpi_iterate,
    .పరిశీలన                       = vpi_పరిశీలన,
    .get                        = vpi_get,
    .get64                      = vpi_get64,
    .get_str                    = vpi_get_str,
    .get_delays                 = vpi_get_delays,
    .put_delays                 = vpi_put_delays,
    .get_విలువ                  = vpi_get_విలువ,
    .put_విలువ                  = vpi_put_విలువ,
    .get_సమయం                   = vpi_get_సమయం,
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
    .లెక్క_drivers              = vpip_లెక్క_drivers,
    .format_strength            = vpip_format_strength,
    .format_pretty              = vpip_format_pretty,
    .make_systf_system_defined  = vpip_make_systf_system_defined,
    .mcd_rawwrite               = vpip_mcd_rawwrite,
    .సమితి_return_విలువ           = vpip_సమితి_return_విలువ,
};
#అయితే_ముగింపు
