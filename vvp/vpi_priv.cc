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

using નામાકાશ std;
vpi_mode_t vpi_mode_flag = VPI_MODE_NONE;
FILE*vpi_trace = 0;

સ્થિર s_vpi_vlog_info  vpi_vlog_info;
સ્થિર s_vpi_error_info vpip_last_error = { 0, 0, 0, 0, 0, 0, 0 };

અચલ અક્ષર* direction_as_string(પૂર્ણાંક dir)
{
      પસંદગી (dir) {
	  સ્થિતિ vpi_આગત:
	    પાછા "input";
	  સ્થિતિ vpiOutput:
	    પાછા "output";
	  સ્થિતિ vpiInout:
	    પાછા "inout";
	  સ્થિતિ vpiMixedIO:
	    પાછા "mixed io";
	  સ્થિતિ vpiNoDirection:
	    પાછા "no direction";
	  મૂળભૂત:
	    પાછા "INVALID-DIRECTION";
      }
}

__vpiHandle::~__vpiHandle()
{ }

પૂર્ણાંક __vpiHandle::vpi_get(પૂર્ણાંક)
{ પાછા vpiUndefined; }

int64_t __vpiHandle::vpi_get64(પૂર્ણાંક code)
{ પાછા (code == _vpiNexusId) ? 0 : vpiUndefined; }

અક્ષર* __vpiHandle::vpi_get_str(પૂર્ણાંક)
{ પાછા 0; }

શૂન્ય __vpiHandle::vpi_get_કિંમત(p_vpi_કિંમત)
{ }

vpiHandle __vpiHandle::vpi_put_કિંમત(p_vpi_કિંમત, પૂર્ણાંક)
{ પાછા 0; }

vpiHandle __vpiHandle::vpi_handle(પૂર્ણાંક)
{ પાછા 0; }

vpiHandle __vpiHandle::vpi_iterate(પૂર્ણાંક)
{ પાછા 0; }

vpiHandle __vpiHandle::vpi_index(પૂર્ણાંક)
{ પાછા 0; }

શૂન્ય __vpiHandle::vpi_get_delays(p_vpi_delay)
{ }

શૂન્ય __vpiHandle::vpi_put_delays(p_vpi_delay)
{ }

__vpiBaseVar::__vpiBaseVar(__vpiScope*scope, અચલ અક્ષર*name, vvp_net_t*net)
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
સ્થિર પૂર્ણાંક suppress_free(vpiHandle)
{ પાછા 1; }
__vpiHandle::free_object_fun_t __vpiHandle::free_object_fun(શૂન્ય)
{ પાછા &suppress_free; }

/*
 * The vpip_string function creates a constant string from the pass
 * input. This constant string is permanently allocated from an
 * efficient string buffer store.
 */
સંરચના vpip_string_chunk {
      સંરચના vpip_string_chunk*next;
      અક્ષર data[64*1024 - કદ (સંરચના vpip_string_chunk*)];
};

unsigned vpip_size(અચલ __vpiSignal *sig)
{
      પાછા abs(sig->msb.get_કિંમત() - sig->lsb.get_કિંમત()) + 1;
}

__vpiScope* vpip_scope(__vpiSignal*sig)
{
      જો (sig->is_netarray)
	    પાછા static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      નહીંતર
	    પાછા sig->within.scope;
}

__vpiScope* vpip_scope(__vpiRealVar*sig)
{
      જો (sig->is_netarray)
	    પાછા static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      નહીંતર
	    પાછા sig->within.scope;
}

vpiHandle vpip_module(__vpiScope*scope)
{
      જ્યારે(scope && scope->get_type_code() != vpiModule) {
	    scope = scope->scope;
      }
      પ્રતિજ્ઞા(scope);
      પાછા scope;
}

અચલ અક્ષર *vpip_string(અચલ અક્ષર*str)
{
      સ્થિર vpip_string_chunk first_chunk = {0, {0}};
      સ્થિર vpip_string_chunk*chunk_list = &first_chunk;
      સ્થિર unsigned chunk_fill = 0;

      unsigned len = strlen(str);
      પ્રતિજ્ઞા( (len+1) <= કદ chunk_list->data );

      જો ( (len+1) > (કદ chunk_list->data - chunk_fill) ) {
	    vpip_string_chunk*tmp = નવું vpip_string_chunk;
	    tmp->next = chunk_list;
	    chunk_list = tmp;
	    chunk_fill = 0;
      }

      અક્ષર*res = chunk_list->data + chunk_fill;
      chunk_fill += len + 1;

      strcpy(res, str);
      પાછા res;
}

સ્થિર unsigned hash_string(અચલ અક્ષર*text)
{
      unsigned h = 0;

      જ્યારે (*text) {
	    h = (h << 4) ^ (h >> 28) ^ (unsigned)*text;
	    text += 1;
      }
      પાછા h;
}

અચલ અક્ષર* vpip_name_string(અચલ અક્ષર*text)
{
      અચલ unsigned HASH_SIZE = 4096;
      સ્થિર અચલ અક્ષર*hash_table[HASH_SIZE] = {0};

      unsigned hash_કિંમત = hash_string(text) % HASH_SIZE;

	/* If we easily find the string in the hash table, then return
	   that and be done. */
      જો (hash_table[hash_કિંમત]
	  && (strcmp(hash_table[hash_કિંમત], text) == 0)) {
	    પાછા hash_table[hash_કિંમત];
      }

	/* The existing hash entry is not a match. Replace it with the
	   newly allocated value, and return the new pointer as the
	   result to the add. */
      અચલ અક્ષર*res = vpip_string(text);
      hash_table[hash_કિંમત] = res;

      પાછા res;
}

PLI_INT32 vpi_chk_error(p_vpi_error_info info)
{
      જો (vpip_last_error.state == 0)
	    પાછા 0;

      info->state = vpip_last_error.state;
      info->level = vpip_last_error.level;
      info->message = vpip_last_error.message;
      info->product = vpi_vlog_info.product;
      info->code = (અક્ષર *)"";
      info->file = 0;
      info->લીટી = 0;

      પાછા info->level;
}

PLI_INT32 vpi_compare_objects(vpiHandle obj1, vpiHandle obj2)
{
      પ્રતિજ્ઞા(obj1);
      પ્રતિજ્ઞા(obj2);

	// Does this actually work for all cases?
      જો (obj1 != obj2) પાછા 0;
      નહીંતર પાછા 1;
}

/*
 * Copy the internal information to the data structure. Do not free or
 * change the tfname/user_data since they are a pointer to the real
 * string/data values. We also support passing a task or function handle
 * instead of just a handle to a vpiUserSystf.
 */
શૂન્ય vpi_get_systf_info(vpiHandle ref, p_vpi_systf_data data)
{
      સંરચના __vpiUserSystf* rfp = dynamic_cast<__vpiUserSystf*>(ref);
      જો (rfp == 0) {
	    સંરચના __vpiSysTaskCall*call = dynamic_cast<__vpiSysTaskCall*>(ref);
	    પ્રતિજ્ઞા(call);
	    rfp = call->defn;
      }

	/* Assert that vpiUserDefn is true! */
      પ્રતિજ્ઞા(rfp->is_user_defn);

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
સંરચના __vpiSysTaskCall*vpip_cur_task = 0;

PLI_INT32 vpi_free_object(vpiHandle ref)
{
      પૂર્ણાંક rtn;

      જો (vpi_trace) {
	    fprintf(vpi_trace, "vpi_free_object(%p)", ref);
	    fflush(vpi_trace);
      }

      પ્રતિજ્ઞા(ref);
      __vpiHandle::free_object_fun_t fun = ref->free_object_fun();
      rtn = fun (ref);

      જો (vpi_trace)
	    fprintf(vpi_trace, " --> %d\n", rtn);

      પાછા rtn;
}

PLI_INT32 vpi_release_handle(vpiHandle ref)
{
	// Since SystemVerilog vpi_free_object() has been
	// renamed vpi_release_handle(), and thus
	// vpi_free_object() has been deprecated.
      પાછા vpi_free_object(ref);
}

સ્થિર પૂર્ણાંક vpip_get_global(પૂર્ણાંક property)
{
      પસંદગી (property) {

	  સ્થિતિ vpiTimeUnit:
	  સ્થિતિ vpiTimePrecision:
	    પાછા vpip_get_time_precision();

	  મૂળભૂત:
	    fprintf(stderr, "vpi error: bad global property: %d\n", property);
	    પ્રતિજ્ઞા(0);
	    પાછા vpiUndefined;
      }
}

સ્થિર અચલ અક્ષર* vpi_property_str(PLI_INT32 code)
{
      સ્થિર અક્ષર buf[32];
      પસંદગી (code) {
	  સ્થિતિ vpiConstType:
	    પાછા "vpiConstType";
	  સ્થિતિ vpiName:
	    પાછા "vpiName";
	  સ્થિતિ vpiFullName:
	    પાછા "vpiFullName";
	  સ્થિતિ vpiTimeUnit:
	    પાછા "vpiTimeUnit";
	  સ્થિતિ vpiTimePrecision:
	    પાછા "vpiTimePrecision";
          સ્થિતિ vpiSize:
	    પાછા "vpiSize";
          સ્થિતિ _vpiNexusId:
	    પાછા "_vpiNexusId";
	  મૂળભૂત:
	    snprintf(buf, કદ(buf), "%d", (પૂર્ણાંક)code);
      }
      પાછા buf;
}

અચલ અક્ષર* vpi_type_as_string(PLI_INT32 code)
{
      સ્થિર અક્ષર buf[32];
      પસંદગી (code) {
	  સ્થિતિ vpiArrayType:
	    પાછા "vpiArrayType";
	  સ્થિતિ vpiBitVar:
	    પાછા "vpiBitVar";
	  સ્થિતિ vpiByteVar:
	    પાછા "vpiByteVar";
	  સ્થિતિ vpiClassVar:
	    પાછા "vpiClassVar";
	  સ્થિતિ vpiConstant:
	    પાછા "vpiConstant";
	  સ્થિતિ vpiEnumTypespec:
	    પાછા "vpiEnumTypespec";
	  સ્થિતિ vpiFunction:
	    પાછા "vpiFunction";
	  સ્થિતિ vpiGenScope:
	    પાછા "vpiGenScope";
	  સ્થિતિ vpiIntVar:
	    પાછા "vpiIntVar";
	  સ્થિતિ vpiIntegerVar:
	    પાછા "vpiIntegerVar";
	  સ્થિતિ vpiIterator:
	    પાછા "vpiIterator";
	  સ્થિતિ vpiLongIntVar:
	    પાછા "vpiLongIntVar";
	  સ્થિતિ vpiMemory:
	    પાછા "vpiMemory";
	  સ્થિતિ vpiMemoryWord:
	    પાછા "vpiMemoryWord";
	  સ્થિતિ vpiModule:
	    પાછા "vpiModule";
	  સ્થિતિ vpiNamedBegin:
	    પાછા "vpiNamedBegin";
	  સ્થિતિ vpiNamedEvent:
	    પાછા "vpiNamedEvent";
	  સ્થિતિ vpiNamedFork:
	    પાછા "vpiNamedFork";
	  સ્થિતિ vpiPackage:
	    પાછા "vpiPackage";
	  સ્થિતિ vpiPathTerm:
	    પાછા "vpiPathTerm";
	  સ્થિતિ vpiPort:
	    પાછા "vpiPort";
	  સ્થિતિ vpiNet:
	    પાછા "vpiNet";
	  સ્થિતિ vpiNetArray:
	    પાછા "vpiNetArray";
	  સ્થિતિ vpiNetBit:
	    પાછા "vpiNetBit";
	  સ્થિતિ vpiParameter:
	    પાછા "vpiParameter";
	  સ્થિતિ vpiPartSelect:
	    પાછા "vpiPartSelect";
	  સ્થિતિ vpiRealVar:
	    પાછા "vpiRealVar";
	  સ્થિતિ vpiReg:
	    પાછા "vpiReg";
	  સ્થિતિ vpiRegBit:
	    પાછા "vpiRegBit";
	  સ્થિતિ vpiShortIntVar:
	    પાછા "vpiShortIntVar";
	  સ્થિતિ vpiStringVar:
	    પાછા "vpiStringVar";
	  સ્થિતિ vpiSysFuncCall:
	    પાછા "vpiSysFuncCall";
	  સ્થિતિ vpiSysTaskCall:
	    પાછા "vpiSysTaskCall";
	  સ્થિતિ vpiTask:
	    પાછા "vpiTask";
	  સ્થિતિ vpiTimeVar:
	    પાછા "vpiTimeVar";
	  સ્થિતિ vpiUserSystf:
	    પાછા "vpiUserSystf";
	  મૂળભૂત:
	    snprintf(buf, કદ(buf), "%d", (પૂર્ણાંક)code);
      }
      પાછા buf;
}

PLI_INT32 vpi_get(પૂર્ણાંક property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      જો (property == _vpiDelaySelection) પાછા vpip_delay_selection;

      જો (ref == 0)
	    પાછા vpip_get_global(property);

      જો (property == vpiType) {
	    જો (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

	    જો (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  પાછા vpiNetArray;
	    નહીંતર
		  પાછા ref->get_type_code();
      }

      પૂર્ણાંક res = ref->vpi_get(property);

      જો (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get(%s, %p) --> %d\n",
		    vpi_property_str(property), ref, res);
      }

      પાછા res;
}

PLI_INT64 vpi_get64(પૂર્ણાંક property, vpiHandle ref)
{
	// For now we only support getting the nexus id.
      જો ((ref == 0) || (property != _vpiNexusId)) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) is not currently supported.\n",
		    vpi_property_str(property), ref);
	    પાછા vpiUndefined;
      }

      int64_t res = ref->vpi_get64(property);

      જો (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) --> %" PRId64 "\n",
		    vpi_property_str(property), ref, res);
      }

      પાછા res;
}

અક્ષર* vpi_get_str(PLI_INT32 property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      જો (property == _vpiDelaySelection) {
	    પસંદગી (vpip_delay_selection) {
		સ્થિતિ _vpiDelaySelMinimum:
		  પાછા simple_set_rbuf_str("MINIMUM");
		સ્થિતિ _vpiDelaySelTypical:
		  પાછા simple_set_rbuf_str("TYPICAL");
		સ્થિતિ _vpiDelaySelMaximum:
		  પાછા simple_set_rbuf_str("MAXIMUM");
		મૂળભૂત:
		  પ્રતિજ્ઞા(0);
	    }
      }

      જો (ref == 0) {
	    fprintf(stderr, "vpi error: vpi_get_str(%s, 0) called "
		    "with null vpiHandle.\n", vpi_property_str(property));
	    પાછા 0;
      }

      જો (property == vpiType) {
	    જો (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

            PLI_INT32 type;
	    જો (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  type = vpiNetArray;
	    નહીંતર
		  type = ref->get_type_code();
	    પાછા (અક્ષર *)vpi_type_as_string(type);
      }

      અક્ષર*res = ref->vpi_get_str(property);

      જો (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get_str(%s, %p) --> %s\n",
		    vpi_property_str(property), ref, res? res : "<NULL>");
      }

      પાછા res;
}

__vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj)
{
      અચલ સંરચના __vpiSysTaskCall*task;
      સંરચના __vpiSignal*સંકેત;
      સંરચના __vpiRealVar*real;
      અચલ __vpiNamedEvent*event;

      પસંદગી (obj->get_type_code()) {
	  સ્થિતિ vpiSysTaskCall:
	    task = dynamic_cast<__vpiSysTaskCall*>(obj);
	    પાછા task->scope;

	  સ્થિતિ vpiModule:
	    પાછા dynamic_cast<__vpiScope*>(obj);

	  સ્થિતિ vpiNet:
	  સ્થિતિ vpiReg:
	  સ્થિતિ vpiIntegerVar:
	  સ્થિતિ vpiBitVar:
	  સ્થિતિ vpiByteVar:
	  સ્થિતિ vpiShortIntVar:
	  સ્થિતિ vpiIntVar:
	  સ્થિતિ vpiLongIntVar:
	    સંકેત = dynamic_cast<__vpiSignal*>(obj);
	    પાછા vpip_scope(સંકેત);

	  સ્થિતિ vpiRealVar:
	    real = dynamic_cast<__vpiRealVar*>(obj);
	    પાછા vpip_scope(real);

	  સ્થિતિ vpiNamedEvent:
	    event = dynamic_cast<__vpiNamedEvent*>(obj);
	    પાછા event->get_scope();

	  સ્થિતિ vpiMemory:
	  સ્થિતિ vpiMemoryWord:
	  સ્થિતિ vpiPartSelect:
	    પાછા dynamic_cast<__vpiScope*>(obj->vpi_handle(vpiScope));

	  મૂળભૂત:
	    fprintf(stderr, "ERROR: vpip_scope_from_handle called with "
		    "object handle type=%d\n", obj->get_type_code());
	    પ્રતિજ્ઞા(0);
	    પાછા 0;
      }
}

પૂર્ણાંક vpip_time_units_from_handle(vpiHandle obj)
{
      જો (obj == 0)
	    પાછા vpip_get_time_precision();

      અચલ __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      જો (scope == 0)
	    પાછા vpip_get_time_precision();

      પાછા scope->time_units;
}

પૂર્ણાંક vpip_time_precision_from_handle(vpiHandle obj)
{
      જો (obj == 0)
	    પાછા vpip_get_time_precision();

      અચલ __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      જો (scope == 0)
	    પાછા vpip_get_time_precision();

      પાછા scope->time_precision;
}

દ્વિગુણ vpip_scaled_time_from_handle(vvp_time64_t time, vpiHandle obj)
{
      પૂર્ણાંક scale = vpip_get_time_precision() -
	          vpip_time_units_from_handle(obj);
      જો (scale >= 0)
	    પાછા (દ્વિગુણ)time * pow(10.0, scale);
      નહીંતર
	    પાછા (દ્વિગુણ)time / pow(10.0, -scale);
}

શૂન્ય vpi_get_time(vpiHandle obj, s_vpi_time*vp)
{
      vvp_time64_t time;

      પ્રતિજ્ઞા(vp);

      time = schedule_simtime();

      પસંદગી (vp->type) {
          સ્થિતિ vpiSimTime:
	    vp->ઉપલું = (time >> 32) & 0xffffffff;
	    vp->નીચલું = time & 0xffffffff;
	    તોડો;

          સ્થિતિ vpiScaledRealTime:
	    vp->real = vpip_scaled_time_from_handle(time, obj);
	    તોડો;

          મૂળભૂત:
            fprintf(stderr, "vpi_get_time: unknown type: %d\n", (પૂર્ણાંક)vp->type);
            પ્રતિજ્ઞા(0);
	    તોડો;
      }
}

PLI_INT32 vpi_get_vlog_info(p_vpi_vlog_info vlog_info_p)
{
    જો (vlog_info_p != 0) {
	  *vlog_info_p = vpi_vlog_info;
	  પાછા 1;
    } નહીંતર {
	  પાછા 0;
    }
}

શૂન્ય vpi_set_vlog_info(પૂર્ણાંક argc, અક્ષર** argv)
{
    સ્થિર અક્ષર icarus_product[] = "Icarus Verilog";
    સ્થિર અક્ષર icarus_version[] = VERSION;
    vpi_vlog_info.product = icarus_product;
    vpi_vlog_info.version = icarus_version;
    vpi_vlog_info.argc    = argc;
    vpi_vlog_info.argv    = argv;

    સ્થિર અક્ષર trace_buf[1024];
    જો (અચલ અક્ષર*path = getenv("VPI_TRACE"))  {
	  જો (!strcmp(path,"-"))
		vpi_trace = stdout;
	  નહીંતર {
		vpi_trace = fopen(path, "w");
		જો (!vpi_trace) {
		      perror(path);
		      exit(1);
		}
		setvbuf(vpi_trace, trace_buf, _IOLBF, કદ(trace_buf));
	  }
    }
}

સ્થિર શૂન્ય vec4_get_કિંમત_string(અચલ vvp_vector4_t&word_val, unsigned width,
				  s_vpi_કિંમત*vp)
{
      unsigned nchar = width / 8;
      unsigned tail = width % 8;

      અક્ષર*rbuf = static_cast<અક્ષર *>(need_result_buf(nchar + 1, RBUF_VAL));
      અક્ષર*cp = rbuf;

      જો (tail > 0) {
	    અક્ષર char_val = 0;
	    માટે (unsigned idx = width-tail; idx < width ;  idx += 1) {
		  vvp_bit4_t val = word_val.value(idx);
		  જો (val == BIT4_1)
			char_val |= 1 << idx;
	    }

	    જો (char_val != 0)
		  *cp++ = char_val;
	    નહીંતર જો (cp != rbuf)
		  *cp++ = ' ';
      }

      માટે (unsigned idx = 0 ;  idx < nchar ;  idx += 1) {
	    unsigned bit = (nchar - idx - 1) * 8;
	    અક્ષર char_val = 0;
	    માટે (unsigned bdx = 0 ;  bdx < 8 ;  bdx += 1) {
		  vvp_bit4_t val = word_val.value(bit+bdx);
		  જો (val == BIT4_1)
			char_val |= 1 << bdx;
	    }
	    // Ignore leading null-bytes and replace other null-bytes with space.
	    // The LRM is not entirely clear on how null bytes should be handled.
	    // This is the implementation chosen for iverilog.
	    જો (char_val != 0)
		  *cp++ = char_val;
	    નહીંતર જો (cp != rbuf)
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
શૂન્ય vpip_vec4_get_કિંમત(અચલ vvp_vector4_t&word_val, unsigned width,
			 તાર્કિક signed_flag, s_vpi_કિંમત*vp)
{
      અક્ષર *rbuf = 0;

      પસંદગી (vp->format) {
	  મૂળભૂત:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector values.\n", (પૂર્ણાંક)vp->format);
	    પ્રતિજ્ઞા(0);

	  સ્થિતિ vpiSuppressVal:
	    તોડો;

	  સ્થિતિ vpiBinStrVal:
	    rbuf = static_cast<અક્ષર *>(need_result_buf(width+1, RBUF_VAL));
	    માટે (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		  vvp_bit4_t bit = word_val.value(idx);
		  rbuf[width-idx-1] = vvp_bit4_to_ascii(bit);
	    }
	    rbuf[width] = 0;
	    vp->value.str = rbuf;
	    તોડો;

	  સ્થિતિ vpiOctStrVal: {
		unsigned hwid = ((width+2) / 3) + 1;
		rbuf = static_cast<અક્ષર *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_oct_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		તોડો;
	  }

	  સ્થિતિ vpiDecStrVal: {
// HERE need a better estimate.
		rbuf = static_cast<અક્ષર *>(need_result_buf(width+1, RBUF_VAL));
		vpip_vec4_to_dec_str(word_val, rbuf, width+1, signed_flag);
		vp->value.str = rbuf;
		તોડો;
	  }

	  સ્થિતિ vpiHexStrVal: {
		unsigned  hwid = ((width + 3) / 4) + 1;
		rbuf = static_cast<અક્ષર *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_hex_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		તોડો;
	  }

          સ્થિતિ vpiScalarVal: {
	        // scalars should be of size 1
	        પ્રતિજ્ઞા(width == 1);
	        પસંદગી(word_val.value(0)) {
		    સ્થિતિ BIT4_0:
		      vp->value.scalar = vpi0;
		      તોડો;
	            સ્થિતિ BIT4_1:
                      vp->value.scalar = vpi1;
                      તોડો;
	            સ્થિતિ BIT4_X:
                      vp->value.scalar = vpiX;
                      તોડો;
	            સ્થિતિ BIT4_Z:
                      vp->value.scalar = vpiZ;
                      તોડો;
		}
                તોડો;
          }

	  સ્થિતિ vpiIntVal: {
		long val = 0;
		vvp_bit4_t pad = BIT4_0;
		જો (signed_flag && word_val.size() > 0)
		      pad = word_val.value(word_val.size()-1);

		માટે (unsigned idx = 0 ; idx < 8*કદ(val) ;  idx += 1) {
		      vvp_bit4_t val4 = pad;
		      જો (idx < word_val.size())
			    val4 = word_val.value(idx);
		      જો (val4 == BIT4_1)
			    val |= 1L << idx;
		}

		vp->value.integer = val;
		તોડો;
	  }

	  સ્થિતિ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiVectorVal;
	    // fallthrough
	  સ્થિતિ vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * કદ(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		માટે (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      પસંદગી (word_val.value(idx)) {
			  સ્થિતિ BIT4_0:
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    તોડો;
			  સ્થિતિ BIT4_1:
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    તોડો;
			  સ્થિતિ BIT4_X:
			    op->aval |= (1 << idx % 32);
			    op->bval |= (1 << idx % 32);
			    તોડો;
			  સ્થિતિ BIT4_Z:
			    op->aval &= ~(1 << idx % 32);
			    op->bval |=  (1 << idx % 32);
			    તોડો;
		      }
		      જો (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		તોડો;
	  }

	  સ્થિતિ vpiStringVal:
		vec4_get_કિંમત_string(word_val, width, vp);
		તોડો;

	  સ્થિતિ vpiRealVal:
		vector4_to_કિંમત(word_val, vp->value.real, signed_flag);
		તોડો;
      }
}

શૂન્ય vpip_vec2_get_કિંમત(અચલ vvp_vector2_t&word_val, unsigned width,
			 તાર્કિક signed_flag, s_vpi_કિંમત*vp)
{
      પસંદગી (vp->format) {
	  મૂળભૂત:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector2 values.\n", (પૂર્ણાંક)vp->format);
	    પ્રતિજ્ઞા(0);

	  સ્થિતિ vpiSuppressVal:
	    તોડો;

	  સ્થિતિ vpiIntVal:
	    vector2_to_કિંમત(word_val, vp->value.integer, signed_flag);
	    તોડો;

	  સ્થિતિ vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  સ્થિતિ vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * કદ(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		માટે (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      જો (word_val.value(idx)) {
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      } નહીંતર {
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      }
		      જો (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		તોડો;
	  }
      }

}

/*
 * Convert a real value to the appropriate integer.
 */
સ્થિર PLI_INT32 get_real_as_int(દ્વિગુણ real)
{
      દ્વિગુણ rtn;

	/* We would normally want to return 'bx for a NaN or
	 * +/- infinity, but for an integer the standard says
	 * to convert 'bx to 0, so we just return 0. */
      જો (real != real || (real && (real == 0.5*real))) {
	    પાછા 0;
      }

      rtn = std::round(real);

      પાછા (PLI_INT32) rtn;
}

/*
 * This is a generic function to convert a double value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer.
 */
શૂન્ય vpip_real_get_કિંમત(દ્વિગુણ real, s_vpi_કિંમત*vp)
{
      અક્ષર *rbuf = 0;

      પસંદગી (vp->format) {
	  મૂળભૂત:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting real values.\n", (પૂર્ણાંક)vp->format);
	    પ્રતિજ્ઞા(0);

	  સ્થિતિ vpiSuppressVal:
	    તોડો;

	  સ્થિતિ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiRealVal;
	    // fallthrough
	  સ્થિતિ vpiRealVal:
	    vp->value.real = real;
	    તોડો;

	  સ્થિતિ vpiIntVal:
	    vp->value.integer = get_real_as_int(real);
	    તોડો;

	  સ્થિતિ vpiDecStrVal:
	    rbuf = static_cast<અક્ષર *>(need_result_buf(1025, RBUF_VAL));
	    vpip_vec4_to_dec_str(vvp_vector4_t(1024, real), rbuf, 1025, સાચું);
	    vp->value.str = rbuf;
	    તોડો;
      }
}

દ્વિગુણ real_from_vpi_કિંમત(s_vpi_કિંમત*vp)
{
      vvp_vector4_t vec4(1024);
      દ્વિગુણ result;
      તાર્કિક is_signed = ખોટું;

      પસંદગી (vp->format) {
	  મૂળભૂત:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "putting real values.\n", (પૂર્ણાંક)vp->format);
	    પ્રતિજ્ઞા(0);

	  સ્થિતિ vpiRealVal:
	    result = vp->value.real;
	    તોડો;

	  સ્થિતિ vpiIntVal:
	    result = (દ્વિગુણ) vp->value.integer;
	    તોડો;

	  સ્થિતિ vpiBinStrVal:
	    vpip_bin_str_to_vec4(vec4, vp->value.str);
	    જો (vp->value.str[0] == '-') is_signed = સાચું;
	    vector4_to_કિંમત(vec4, result, is_signed);
	    તોડો;

	  સ્થિતિ vpiOctStrVal:
	    vpip_oct_str_to_vec4(vec4, vp->value.str);
	    જો (vp->value.str[0] == '-') is_signed = સાચું;
	    vector4_to_કિંમત(vec4, result, is_signed);
	    તોડો;

	  સ્થિતિ vpiDecStrVal:
	    vpip_dec_str_to_vec4(vec4, vp->value.str);
	    જો (vp->value.str[0] == '-') is_signed = સાચું;
	    vector4_to_કિંમત(vec4, result, is_signed);
	    તોડો;

	  સ્થિતિ vpiHexStrVal:
	    vpip_hex_str_to_vec4(vec4, vp->value.str);
	    જો (vp->value.str[0] == '-') is_signed = સાચું;
	    vector4_to_કિંમત(vec4, result, is_signed);
	    તોડો;

      }

      પાછા result;
}

શૂન્ય vpip_string_get_કિંમત(અચલ string&val, s_vpi_કિંમત*vp)
{
      અક્ષર *rbuf = 0;

      પસંદગી (vp->format) {
	  મૂળભૂત:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting string values.\n", (પૂર્ણાંક)vp->format);
	    પ્રતિજ્ઞા(0);

	  સ્થિતિ vpiSuppressVal:
	    તોડો;

	  સ્થિતિ vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiStringVal;
	    // fallthrough
	  સ્થિતિ vpiStringVal:
	    rbuf = static_cast<અક્ષર *>(need_result_buf(val.size() + 1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    તોડો;
      }
}


શૂન્ય vpi_get_કિંમત(vpiHandle પદાવલિ, s_vpi_કિંમત*vp)
{
      પ્રતિજ્ઞા(પદાવલિ);
      પ્રતિજ્ઞા(vp);

	// Never bother with suppressed values. All the derived
	// classes can ignore this type.
      જો (vp->format == vpiSuppressVal)
	    પાછા;

      પદાવલિ->vpi_get_કિંમત(vp);

      જો (vpi_trace) પસંદગી (vp->format) {
		સ્થિતિ vpiStringVal:
		  fprintf(vpi_trace,"vpi_get_value(%p=<%d>) -> string=\"%s\"\n",
			  પદાવલિ, પદાવલિ->get_type_code(), vp->value.str);
		  તોડો;

		સ્થિતિ vpiBinStrVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> binstr=%s\n",
			  પદાવલિ->get_type_code(), vp->value.str);
		  તોડો;

		સ્થિતિ vpiIntVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> int=%d\n",
			  પદાવલિ->get_type_code(), (પૂર્ણાંક)vp->value.integer);
		  તોડો;

		સ્થિતિ vpiSuppressVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <suppress>\n",
			  પદાવલિ->get_type_code());
		  તોડો;

		મૂળભૂત:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <%d>=?\n",
			  પદાવલિ->get_type_code(), (પૂર્ણાંક)vp->format);
	    }
}

સંરચના vpip_put_કિંમત_event : vvp_gen_event_s {
      vpiHandle handle;
      s_vpi_કિંમત value;
      પૂર્ણાંક flags;
      આભાસી શૂન્ય run_run() override;
      ~vpip_put_કિંમત_event() override { }
};

શૂન્ય vpip_put_કિંમત_event::run_run()
{
      handle->vpi_put_કિંમત(&value, flags);
      પસંદગી (value.format) {
	    /* Free the copied string. */
	  સ્થિતિ vpiBinStrVal:
	  સ્થિતિ vpiOctStrVal:
	  સ્થિતિ vpiDecStrVal:
	  સ્થિતિ vpiHexStrVal:
	  સ્થિતિ vpiStringVal:
	    મુક્ત(value.value.str);
	    તોડો;
	    /* Free the copied time structure. */
	  સ્થિતિ vpiTimeVal:
	    મુક્ત(value.value.time);
	    તોડો;
	    /* Free the copied vector structure. */
	  સ્થિતિ vpiVectorVal:
	    મુક્ત(value.value.vector);
	    તોડો;
	    /* Free the copied strength structure. */
	  સ્થિતિ vpiStrengthVal:
	    મુક્ત(value.value.strength);
	    તોડો;
	    /* Everything else is static in the structure. */
	  મૂળભૂત:
	    તોડો;
      }
}

/* Make a copy of a pointer to a time structure. */
સ્થિર t_vpi_time *timedup(અચલ t_vpi_time *val)
{
      t_vpi_time *rtn;
      rtn = static_cast<t_vpi_time *> (malloc(કદ(t_vpi_time)));
      *rtn = *val;
      પાછા rtn;
}

/* Make a copy of a pointer to a vector value structure. */
સ્થિર t_vpi_vecval *vectordup(અચલ t_vpi_vecval *val, PLI_INT32 size)
{
      unsigned અંક_bytes;
      t_vpi_vecval *rtn;
      પ્રતિજ્ઞા(size > 0);
      અંક_bytes = ((size + 31)/32)*કદ(t_vpi_vecval);
      rtn = static_cast<t_vpi_vecval *> (malloc(અંક_bytes));
      memcpy(rtn, val, અંક_bytes);
      પાછા rtn;
}

/* Make a copy of a pointer to a strength structure. */
સ્થિર t_vpi_strengthval *strengthdup(અચલ t_vpi_strengthval *val)
{
      t_vpi_strengthval *rtn;
      rtn = static_cast<t_vpi_strengthval *>
            (malloc(કદ(t_vpi_strengthval)));
      *rtn = *val;
      પાછા rtn;
}

vpiHandle vpi_put_કિંમત(vpiHandle obj, s_vpi_કિંમત*vp,
			s_vpi_time*when, PLI_INT32 flags)
{
      પ્રતિજ્ઞા(obj);

      flags &= ~vpiReturnEvent;

      જો (flags!=vpiNoDelay && flags!=vpiForceFlag && flags!=vpiReleaseFlag) {
	    vvp_time64_t dly;
	    પૂર્ણાંક scale;

	    જો (vpi_get(vpiAutomatic, obj)) {
		  fprintf(stderr, "VPI error: cannot put a value with "
				  "a delay on automatically allocated "
				  "variable '%s'.\n",
				  vpi_get_str(vpiName, obj));
		  પાછા 0;
	    }

	    પ્રતિજ્ઞા(when != 0);

	    પસંદગી (when->type) {
		સ્થિતિ vpiScaledRealTime:
		  scale = vpip_time_units_from_handle(obj) -
		          vpip_get_time_precision();
		  જો (scale >= 0) {
			dly = (vvp_time64_t)(when->real * pow(10.0, scale));
		  } નહીંતર {
			dly = (vvp_time64_t)(when->real / pow(10.0, -scale));
		  }
		  તોડો;
		સ્થિતિ vpiSimTime:
		  dly = vpip_timestruct_to_time(when);
		  તોડો;
		મૂળભૂત:
		  dly = 0;
		  તોડો;
	    }

	    જો ((dly == 0) && schedule_at_rosync()) {
		  fprintf(stderr, "VPI error: attempted to put a value to "
				  "variable '%s' during a read-only synch "
				  "callback.\n", vpi_get_str(vpiName, obj));
		  પાછા 0;
	    }

	    vpip_put_કિંમત_event*put = નવું vpip_put_કિંમત_event;
	    put->handle = obj;
	    જો (dynamic_cast<__vpiNamedEvent*>(obj)) {
		  put->value.format = vpiIntVal;
		  put->value.value.integer = 0;
	    } નહીંતર {
		  પ્રતિજ્ઞા(vp);
		  put->value = *vp;
	    }
	      /* Since this is a scheduled put event we must copy any pointer
	       * data to keep it available until the event is actually run. */
	    પસંદગી (put->value.format) {
		  /* Copy the string items. */
		સ્થિતિ vpiBinStrVal:
		સ્થિતિ vpiOctStrVal:
		સ્થિતિ vpiDecStrVal:
		સ્થિતિ vpiHexStrVal:
		સ્થિતિ vpiStringVal:
		  put->value.value.str = strdup(put->value.value.str);
		  તોડો;
		  /* Copy a time pointer item. */
		સ્થિતિ vpiTimeVal:
		  put->value.value.time = timedup(put->value.value.time);
		  તોડો;
		  /* Copy a vector pointer item. */
		સ્થિતિ vpiVectorVal:
		  put->value.value.vector = vectordup(put->value.value.vector,
		                                      vpi_get(vpiSize, obj));
		  તોડો;
		  /* Copy a strength pointer item. */
		સ્થિતિ vpiStrengthVal:
		  put->value.value.strength =
		      strengthdup(put->value.value.strength);
		  તોડો;
		  /* Everything thing else is already in the structure. */
		મૂળભૂત:
		  તોડો;
	    }
	    put->flags = flags;
	    schedule_generic(put, dly, ખોટું, સાચું, સાચું);
	    પાછા 0;
      }

      જો (schedule_at_rosync()) {
            fprintf(stderr, "VPI error: attempted to put a value to "
			    "variable '%s' during a read-only synch "
			    "callback.\n", vpi_get_str(vpiName, obj));
            પાછા 0;
      }

      obj->vpi_put_કિંમત(vp, flags);

      પાછા 0;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle res = 0;

      જો (ref == 0) {
	      // A few types can apply to a nil handle. These are ways
	      // that the current function can get started finding things.
	    પસંદગી (type) {

		સ્થિતિ vpiScope:
		    // The IEEE1364-2005 doesn't seem to allow this,
		    // but some users seem to think it's handy, so
		    // return the scope that contains this SysTfCall.
		  પ્રતિજ્ઞા(vpip_cur_task);
		  res = vpip_cur_task->vpi_handle(vpiScope);
		  તોડો;

		સ્થિતિ vpiSysTfCall:
		    // This is how VPI users get a first handle into
		    // the system. This is the handle of the system
		    // task/function call currently being executed.
		  જો (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle(vpiSysTfCall, 0) "
				"-> %p (%s)\n", vpip_cur_task,
				vpip_cur_task->defn->info.tfname);
		  }
		  પાછા vpip_cur_task;

		મૂળભૂત:
		  fprintf(stderr, "VPI error: vpi_handle(type=%d, ref=0).\n",
			  (પૂર્ણાંક)type);
		  res = 0;
		  તોડો;
	    }

      } નહીંતર {

	    જો (type == vpiSysTfCall) {
		  fprintf(stderr, "VPI error: vpi_handle(vpiSysTfCall, "
			  "ref!=0).\n");
		  પાછા 0;
	    }

	    res = ref->vpi_handle(type);
      }


      જો (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle(vpiScope, ref=%p) "
		    "-> %p\n", vpip_cur_task, ref);
      }

      પાછા res;
}

સ્થિર vpiHandle vpip_make_udp_iterator()
{
// HERE: Add support for iterating over UDP definitions.
//       See 26.6.16 (page 400 in 1364-2005).
      પાછા 0;
}

/*
 * This function asks the object to return an iterator for
 * the specified reference. It is up to the iterate_ method to
 * allocate a properly formed iterator.
 */
સ્થિર vpiHandle vpi_iterate_global(પૂર્ણાંક type)
{
      પસંદગી (type) {
	  સ્થિતિ vpiInstance:
	    // fallthrough
	  સ્થિતિ vpiModule:
	    // fallthrough
	  સ્થિતિ vpiProgram:
	    // fallthrough
	  સ્થિતિ vpiInterface:
	    // fallthrough
	  સ્થિતિ vpiPackage:
	    પાછા vpip_make_મૂળ_iterator(type);

	  સ્થિતિ vpiUdpDefn:
	    પાછા vpip_make_udp_iterator();

	  સ્થિતિ vpiUserSystf:
	    પાછા vpip_make_systf_iterator();
      }

      પાછા 0;
}

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle rtn = 0;

      પ્રતિજ્ઞા(vpi_mode_flag != VPI_MODE_NONE);
      જો (vpi_mode_flag == VPI_MODE_REGISTER) {
	    fprintf(stderr, "vpi error: vpi_iterate called during "
		    "vpi_register_systf. You can't do that!\n");
	    પાછા 0;
      }

      જો (ref == 0)
	    rtn = vpi_iterate_global(type);
      નહીંતર
	    rtn = ref->vpi_iterate(type);

      જો (vpi_trace) {
	    fprintf(vpi_trace, "vpi_iterate(%d, %p) ->%s\n",
	    (પૂર્ણાંક)type, ref, rtn ? "" : " (null)");
      }

      પાછા rtn;
}

vpiHandle vpi_handle_by_index(vpiHandle ref, PLI_INT32 idx)
{
      પ્રતિજ્ઞા(ref);
      પાછા ref->vpi_index(idx);
}

સ્થિર vpiHandle find_name(અચલ અક્ષર *name, vpiHandle handle)
{
      vpiHandle rtn = 0;
      __vpiScope*ref = dynamic_cast<__vpiScope*>(handle);

      /* check module names */
      જો (!strcmp(name, vpi_get_str(vpiName, handle)))
	    rtn = handle;

      /* brute force search for the name in all objects in this scope */
      માટે (unsigned i = 0 ;  i < ref->intern.size() ;  i += 1) {
	      /* The standard says that since a port does not have a full
	       * name it cannot be found by name. Because of this we need
	       * to skip ports here so the correct handle can be located. */
	    જો (vpi_get(vpiType, ref->intern[i]) == vpiPort) ચાલુ;
	    અચલ અક્ષર *nm = vpi_get_str(vpiName, ref->intern[i]);
	    જો (nm && !strcmp(name, nm)) {
		  rtn = ref->intern[i];
		  તોડો;
	    } નહીંતર જો (vpi_get(vpiType, ref->intern[i]) == vpiMemory ||
	               vpi_get(vpiType, ref->intern[i]) == vpiNetArray) {
		  /* We need to iterate on the words */
		  vpiHandle word_i, word_h;
		  word_i = vpi_iterate(vpiMemoryWord, ref->intern[i]);
		  જ્યારે (word_i && (word_h = vpi_scan(word_i))) {
			nm = vpi_get_str(vpiName, word_h);
			જો (nm && !strcmp(name, nm)) {
			      rtn = word_h;
			      vpi_free_object(word_i);
			      તોડો;
			}
		  }
	    }
	    /* found it yet? */
	    જો (rtn) તોડો;
      }

      પાછા rtn;
}

// Find the end of the escaped identifier or simple identifier
સ્થિર અક્ષર * find_rest(અક્ષર *name)
{
      અક્ષર *rest;
      જો (*name == '\\') {
	    rest = strchr(name, ' ');
	    જો (rest) {
		  *rest++ = 0; // The space is not part of the string
		    // There should be a '.' after the escaped ID if there is
		    // anything more. If it is missing add it to avoid a crash
		  જો ((*rest != '.') && (*rest != 0)) {
			*--rest = '.';
			fprintf(stderr, "ERROR: Malformed scope string: \"%s\"", name);
		  }
		  જો (*rest == 0) {
			rest = 0;
		  }
	    }
      } નહીંતર {
	    rest = strchr(name, '.');
      }
      પાછા rest;
}

સ્થિર vpiHandle find_scope(અચલ અક્ષર *name, vpiHandle handle, પૂર્ણાંક depth)
{

      vpiHandle iter = handle==0
	    ? vpi_iterate(vpiModule, ખાલી)
	    : vpi_iterate(vpiInternalScope, handle);

      vector<અક્ષર> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      અક્ષર*nm_first = &name_buf[0];
      અક્ષર*nm_rest = find_rest(nm_first);
      જો (*nm_first == '\\') ++nm_first;
      જો (nm_rest) {
	    *nm_rest++ = 0;
      }

      vpiHandle rtn = 0;
      vpiHandle hand;
      જ્યારે (iter && (hand = vpi_scan(iter))) {
	    અચલ અક્ષર *nm = vpi_get_str(vpiName, hand);

	    જો (strcmp(nm_first,nm)==0) {
		  જો (nm_rest)
			rtn=find_scope(nm_rest, hand, depth+1);
		  નહીંતર
			rtn = hand;
	    }

	    /* found it yet ? */
	    જો (rtn) {
		  vpi_free_object(iter);
		  તોડો;
	    }
      }

      પાછા rtn;
}

// Find the end of the first escaped identifier or simple identifier
સ્થિર અક્ષર * find_next(અક્ષર *name)
{
      અક્ષર *next;
      જો (*name == '\\') {
	    next = strchr(name, ' ');
	    જો (next && *++next == 0) next = 0;
      } નહીંતર {
	    next = strchr(name, '.');
      }
      પાછા next;
}

vpiHandle vpi_handle_by_name(અચલ અક્ષર *name, vpiHandle scope)
{
      vpiHandle hand;

      જો (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name(%s, %p) -->\n",
		    name, scope);
      }

	// Chop the name into path and base. For example, if the name
	// is "a.b.c", then nm_path becomes "a.b" and nm_base becomes
	// "c". If the name is "c" then nm_path is nil and nm_base is "c".
      vector<અક્ષર> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      અક્ષર*nm_path = &name_buf[0];
      અક્ષર*nm_base;

	// Check to see if we have an escaped identifier and if so search
	// the long way because a '.' could be in the escaped name.
      જો (strchr(nm_path, '\\')) {
	    અક્ષર *next;
	    nm_base = nm_path;
	    જ્યારે ((next = find_next(nm_base))) {
		  nm_base = ++next;
	    }
	    જો (nm_path == nm_base) {
		  nm_path = 0;
	    } નહીંતર {
		  *(nm_base-1) = 0;
	    }

	// If there is no escaped identifier then just look for the last '.'
      } નહીંતર {
	    nm_base = strrchr(nm_path, '.');
	    જો (nm_base) {
		  *nm_base++ = 0;
	    } નહીંતર {
		  nm_base = nm_path;
		  nm_path = 0;
	    }
      }

      /* If scope provided, look in corresponding module; otherwise
       * traverse the hierarchy specified in name to find the leaf module
       * and try finding it there.
       */
      જો (scope) {
	    /* Some implementations support either a module or a scope. */
	    પસંદગી (vpi_get(vpiType, scope)) {
		સ્થિતિ vpiScope:
	          hand = vpi_handle(vpiModule, scope);
	          તોડો;
		સ્થિતિ vpiModule:
	          hand = scope;
	          તોડો;
		મૂળભૂત:
		  જો (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle_by_name: "
				"Scope is not a vpiScope or vpiModule\n");
		  }
	          // Use vpi_chk_error() here when it is implemented.
	          પાછા 0;
	    }

      } નહીંતર જો (nm_path) {
	      // The name has a path, and no other scope handle was
	      // passed in. That suggests we are looking for "a.b.c"
	      // in the root scope. So convert "a.b" to a scope and
	      // start there to look for "c".
	    hand = find_scope(nm_path, ખાલી, 0);
	    nm_path = 0;

      } નહીંતર {
	      // Special case: scope==<nil>, meaning we are looking in
	      // the root, and there is no path to the name, i.e. the
	      // string is "c" instead of "top.c". Try to find "c" as
	      // a scope and return that.
	    hand = find_scope(nm_base, ખાલી, 0);
      }

      જો (hand == 0) {
	    જો (vpi_trace) {
		  fprintf(vpi_trace, "vpi_handle_by_name: "
			  "Scope does not exist. Giving up.\n");
	    }

	    પાછા 0;
      }

	// If there is a path part, then use it to find the
	// scope. For example, if the full name is a.b.c, then
	// the nm_path string is a.b and we search for that
	// scope. If we find it, then set hand to that scope.
      જો (nm_path) {
	    vpiHandle tmp = find_scope(nm_path, hand, 0);
	    જ્યારે (tmp == 0 && hand != 0) {
		  hand = vpi_handle(vpiScope, hand);
		  tmp = find_scope(nm_path, hand, 0);
	    }
	    hand = tmp;
      }

	// find_name() expects escaped identifiers to be stripped
      જો (*nm_base == '\\') {
	    // Skip the \ at the beginning
	    ++nm_base;

	    // Drop the space at the end if it exists
	    અક્ષર *next;
	    જો ((next = strchr(nm_base, ' '))) {
		  *next = 0;
	    }
      }

	// Now we have the correct scope, look for the item.
      vpiHandle out = find_name(nm_base, hand);

      જો (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name: DONE\n");
      }

      પાછા out;
}

// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected
// x2: Ptr to previous node
// x3: True when net is connected to output of previous node
tuple<તાર્કિક, vvp_net_ptr_t, તાર્કિક> check_connected_to_concat8_and_part_sa(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent) {

      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      જ્યારે (cur.ptr()) {
	    જો (cur.ptr() == net2 && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
		    // Pass on the return value if found
		  પાછા {સાચું, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
	    } નહીંતર જો (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8_and_part_sa(cur.ptr()->out_, net2, cur);
		  જો (get<0>(res)) {
			પાછા res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      પાછા {ખોટું, vvp_net_ptr_t(nullptr, 0), ખોટું};
}

// Check if net2 is connected to current_net through a net of vvp_fun_concat8s
// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected with concats and part_sas
// x2: Ptr to previous node
// x3: True when net is connected to output of x2
tuple<તાર્કિક, vvp_net_ptr_t, તાર્કિક> check_connected_to_concat8(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent)
{
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);
	// For everything connected
      જ્યારે (cur.ptr()) {
	      // Check if it's a concat8
	    જો (cur.ptr() == net2) {
		  પાછા {સાચું, vvp_net_ptr_t(nullptr, 0), ખોટું};
	    } નહીંતર જો (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8(cur.ptr()->out_, net2, cur);
		  જો (get<0>(res)) {
			જો (!get<1>(res).ptr() && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
			      // `parent` is the last part_sa node in front of our destination
			      // We want to place the intermod node here
			      પાછા {સાચું, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
			}
			પાછા res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      પાછા {ખોટું, vvp_net_ptr_t(nullptr, 0), ખોટું};
}

// Used to get intermodpath for two ports
vpiHandle vpi_handle_multi(PLI_INT32 type,
                           vpiHandle ref1,
                           vpiHandle ref2)
{
      જો (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_multi(%d, %p, %p) -->\n",
	            type, ref1, ref2);
      }

      જો (type != vpiInterModPath) {
	    fprintf(stderr, "sorry: vpi_handle_multi currently supports"
	            "only vpiInterModPath\n");
	    પાછા nullptr;
      }

	// Indicates whether port1 refers to a single bit
      તાર્કિક port1_has_index = ખોટું;
      પૂર્ણાંક port1_bit_index = 0;
      vpiPortBitInfo* port1_bit = dynamic_cast<vpiPortBitInfo*>(ref1);

      જો (port1_bit) {
	      // Get the bit index
	    port1_has_index = સાચું;
	    port1_bit_index = vpi_get(vpiBit, port1_bit);
	      // Update the ref1 to point to the base port
	    ref1 = vpi_handle(vpiParent, port1_bit);
      }

	// Indicates whether port2 refers to a single bit
      તાર્કિક port2_has_index = ખોટું;
      પૂર્ણાંક port2_bit_index = 0;
      vpiPortBitInfo* port2_bit = dynamic_cast<vpiPortBitInfo*>(ref2);

      જો (port2_bit) {
	      // Get the bit index
	    port2_has_index = સાચું;
	    port2_bit_index = vpi_get(vpiBit, port2_bit);
	      // Update the ref1 to point to the base port
	    ref2 = vpi_handle(vpiParent, port2_bit);
      }

      vpiPortInfo* port1 = dynamic_cast<vpiPortInfo*>(ref1);

      જો (!port1) {
	    fprintf(stderr, "sorry: second argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    પાછા nullptr;
      }

      vpiPortInfo* port2 = dynamic_cast<vpiPortInfo*>(ref2);

      જો (!port2) {
	    fprintf(stderr, "sorry: third argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    પાછા nullptr;
      }

	// Get the names of both ports
      std::string port1_name(vpi_get_str(vpiName, ref1));
      std::string port2_name(vpi_get_str(vpiName, ref2));

	// If both ports are vpiOutput, we have to reassign the __vpiSignal from port1
	// to port2 because otherwise the non-delayed version of the signal is dumped
	// even tho the intermodpath is correctly inserted
      __vpiSignal* output_signal = nullptr;

      જો (port1->get_direction() == vpiOutput && port2->get_direction() == vpiOutput) {
	    vpiHandle scope_port2 = vpi_handle(vpiScope, ref2);
	    પ્રતિજ્ઞા(scope_port2);

	      // Iterate over nets in the scope of port2
	    vpiHandle net_i = vpi_iterate(vpiNet, scope_port2) ;
	    vpiHandle net;

	    જ્યારે ((net = vpi_scan(net_i)) != ખાલી) {
		  std::string net_name(vpi_get_str(vpiName, net));

		    // Compare whether the net matches with the port name
		  જો (net_name == port2_name) {
			output_signal = dynamic_cast<__vpiSignal*>(net);
		  }
	    }
      }

      vvp_net_t* net1 = port1->get_port();
      vvp_net_t* net2 = port2->get_port();

      જો (net1 == nullptr || net2 == nullptr) {
	    fprintf(stderr, "Error: Could not find net. "
		    "Did you run iverilog with '-ginterconnect'?\n");
	    પાછા nullptr;
      }

      જો (net1 == net2) {
	    fprintf(stderr, "Error: Net for both ports is the same. "
		    "Did you pass the same port twice?\n");
	    પાછા nullptr;
      }

      જો (!dynamic_cast<vvp_fun_buft*>(net1->fun)) {
	    fprintf(stderr, "Error: functor of net1 must be"
		    "vvp_fun_buft\n");
	    પાછા nullptr;
      }

      જો (!dynamic_cast<vvp_fun_buft*>(net2->fun)) {
	    fprintf(stderr, "Error: functor of net2 must be"
		    "vvp_fun_buft\n");
	    પાછા nullptr;
      }

	// If port1 is actually a port bit, we have to get to the correct vvp_fun_part
	// after which we insert the intermodpath delay
      જો (port1_has_index) {
	    vvp_net_ptr_t* net1_ptr = &net1->out_;

	      // Search for part selects connected to port1
	    vvp_net_t* current_net = net1_ptr->ptr();

	    જ્યારે (current_net) {
		  અચલ vvp_fun_part* part = dynamic_cast<vvp_fun_part*>(current_net->fun);

		    // Its a part select!
		  જો (part) {
			  // Is it the correct part select?
			જો (part->get_base() == (unsigned)port1_bit_index) {
			      પ્રતિજ્ઞા(part->get_wid() == 1);
			      net1 = current_net; // Replace net1 as this is our new start point
			      તોડો;
			}
		  }

		  current_net = current_net->port[0].ptr(); // BUFT has only one input, index 0
	    }
      }

	// Iterate over all nodes connected to port1
      vvp_net_ptr_t cur = net1->out_;
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      જ્યારે (cur.ptr()) {
	      // Either port2 is directly connected to port1
	      // Or in the second case port2 is indirectly connected
	      // to port1 through a net of concat8s
	    તાર્કિક is_connected, is_output;
	    vvp_net_ptr_t previous_ગાંઠ;
	    tie(is_connected, previous_ગાંઠ, is_output) = port2_has_index
		  ? check_connected_to_concat8(cur, net2, vvp_net_ptr_t(net1, 0))
		  : check_connected_to_concat8_and_part_sa(cur, net2, vvp_net_ptr_t(net1, 0));
	    જો ( (!port2_has_index && cur.ptr() == net2) || is_connected ) {
		  vvp_net_t*new_net = નવું vvp_net_t;

		    // Create new node with intermodpath and connect port2 to it
		  પૂર્ણાંક width = 1; // TODO
		  vvp_fun_intermodpath*obj = નવું vvp_fun_intermodpath(new_net, width);

		  vvp_net_t* output_net = net1;
		  જો (is_connected) {
			જો (is_output) {
			      output_net = previous_ગાંઠ.ptr(); // net2 is direct output of part_sa
			      cur = output_net->out_;
			      prev = vvp_net_ptr_t(nullptr, 0);
			} નહીંતર {
			      prev = previous_ગાંઠ;
			      જો (prev.ptr()) {
				    cur = prev.ptr()->port[prev.port()];
			      }
			}
		  }

		  new_net->fun = obj;
		  new_net->out_ = cur;

		    // Port2 is in the middle of the list
		    // Insert intermodpath before port2 and keep everything else intact
		  જો (prev.ptr()) {
			prev.ptr()->port[prev.port()] = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; // Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		    // Port2 is first in list
		    // Insert intermodpath before port2 and keep everything else intact
		  } નહીંતર {
			output_net->out_ = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; //  Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		  }

		    // If both ports are vpiOutput and port2 is not a vector,
		    // we have to reassign the __vpiSignal so that the delayed
		    // values get dumped
		  જો (output_signal && !port2_has_index) {
			net2->fil = net1->fil;
			net1->fil = nullptr;
			output_signal->ગાંઠ = net2;
		  }

		    // Create the VPI intermodpath object
		  __vpiInterModPath* intermodpath = vpip_make_intermodpath(new_net, port1, port2);
		  intermodpath->intermodpath = obj;

		    // Finally done, return the intermodpath object
		  પાછા intermodpath;
	    }

	    prev = cur;
	    cur = cur.ptr()->port[cur.port()]; // Next net in linked list
      }

      fprintf(stderr, "VPI error: Could not insert intermodpath!\n");
      fprintf(stderr, "\tport1 = %s, port1_has_index = %d, port1_bit_index = %d\n", port1_name.c_str(), port1_has_index, port1_bit_index);
      fprintf(stderr, "\tport2 = %s, port2_has_index = %d, port2_bit_index = %d\n", port2_name.c_str(), port2_has_index, port2_bit_index);
      પાછા nullptr;
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



શૂન્ય vpi_get_delays(vpiHandle પદાવલિ, p_vpi_delay delays)
{
      પ્રતિજ્ઞા(પદાવલિ);
      પ્રતિજ્ઞા(delays);

      પદાવલિ->vpi_get_delays(delays);

      જો (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_get_delays(%p, %p) -->\n", પદાવલિ, delays);
      }
}


શૂન્ય vpi_put_delays(vpiHandle પદાવલિ, p_vpi_delay delays)
{
      પ્રતિજ્ઞા(પદાવલિ  );
      પ્રતિજ્ઞા(delays );

      પદાવલિ->vpi_put_delays(delays);

      જો (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_put_delays(%p, %p) -->\n", પદાવલિ, delays);
      }
}








બાહ્ય "C" PLI_INT32 vpi_vprintf(અચલ અક્ષર*fmt, va_list ap)
{
      પાછા vpi_mcd_vprintf(1, fmt, ap);
}

બાહ્ય "C" PLI_INT32 vpi_printf(અચલ અક્ષર *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      પૂર્ણાંક r = vpi_mcd_vprintf(1, fmt, ap);
      va_end(ap);
      પાછા r;
}

બાહ્ય "C" PLI_INT32 vpi_flush(શૂન્ય)
{
      પાછા vpi_mcd_flush(1);
}


બાહ્ય "C" PLI_INT32 vpi_sim_vcontrol(પૂર્ણાંક operation, va_list ap)
{
      long diag_સંદેશ;

      પસંદગી (operation) {
	  સ્થિતિ vpiFinish:
	  સ્થિતિ __ivl_legacy_vpiFinish:
            diag_સંદેશ = va_arg(ap, PLI_INT32);
	    schedule_finish(diag_સંદેશ);
	    તોડો;

	  સ્થિતિ vpiStop:
	  સ્થિતિ __ivl_legacy_vpiStop:
            diag_સંદેશ = va_arg(ap, PLI_INT32);
	    schedule_stop(diag_સંદેશ);
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "Unsupported VPI control operation %d.\n", operation);
	    પાછા 0;
      }
      પાછા 1;
}

બાહ્ય "C" PLI_INT32 vpi_sim_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      પૂર્ણાંક r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      પાછા r;
}

બાહ્ય "C" PLI_INT32 vpi_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      પૂર્ણાંક r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      પાછા r;
}

/*
 * This routine calculated the return value for $clog2.
 * It is easier to do it here vs trying to to use the VPI interface.
 */
બાહ્ય "C" s_vpi_vecval vpip_calc_clog2(vpiHandle arg)
{
      s_vpi_vecval rtn;
      s_vpi_કિંમત val;
      vvp_vector4_t vec4;
      તાર્કિક is_neg = ખોટું;  // At this point only a real can be negative.

	/* Get the value as a vvp_vector4_t. */
      val.format = vpiObjTypeVal;
      vpi_get_કિંમત(arg, &val);
      જો (val.format == vpiRealVal) {
	    vpi_get_કિંમત(arg, &val);
	      /* All double values can be represented in 1024 bits. */
	    vec4 = vvp_vector4_t(1024, val.value.real);
	    જો (val.value.real < 0) is_neg = સાચું;
      } નહીંતર {
	    val.format = vpiVectorVal;
	    vpi_get_કિંમત(arg, &val);
	    unsigned wid = vpi_get(vpiSize, arg);
	    vec4 = vvp_vector4_t(wid, BIT4_0);
	    માટે (unsigned idx=0; idx < wid; idx += 1) {
		PLI_INT32 aval = val.value.vector[idx/32].aval;
		PLI_INT32 bval = val.value.vector[idx/32].bval;
		aval >>= idx % 32;
		bval >>= idx % 32;
		પૂર્ણાંક bitmask = (aval&1) | ((bval<<1)&2);
		vvp_bit4_t bit = scalar_to_bit4(bitmask);
		vec4.set_bit(idx, bit);
	    }
      }

      જો (vec4.has_xz()) {
	    rtn.aval = rtn.bval = 0xFFFFFFFFU;  /* Set to 'bx. */
	    પાછા rtn;
      }

      vvp_vector2_t vec2(vec4);

      જો (is_neg) vec2.trim_neg(); /* This is a special trim! */
      નહીંતર vec2.trim(); /* This makes less work shifting. */

	/* Calculate the clog2 result. */
      PLI_INT32 res = 0;
      જો (!vec2.is_zero()) {
	    vec2 -= vvp_vector2_t(1, vec2.size());
	    જ્યારે(!vec2.is_zero()) {
		  res += 1;
		  vec2 >>= 1;
	    }
      }

      rtn.aval = res;
      rtn.bval = 0;
      પાછા rtn;
}

/*
 * This routine provides the information needed to implement $countdrivers.
 * It is done here for performance reasons - interrogating the drivers
 * individually via the VPI interface would be much slower.
 */
બાહ્ય "C" શૂન્ય vpip_ગણતરી_drivers(vpiHandle ref, unsigned idx,
                                   unsigned counts[4])
{
      સંરચના __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      પ્રતિજ્ઞા(rfp);
      rfp->ગાંઠ->ગણતરી_drivers(idx, counts);
}

#જો defined(__MINGW32__) || defined (__CYGWIN__)
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
    .get_કિંમત                  = vpi_get_કિંમત,
    .put_કિંમત                  = vpi_put_કિંમત,
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
    .ગણતરી_drivers              = vpip_ગણતરી_drivers,
    .format_strength            = vpip_format_strength,
    .format_pretty              = vpip_format_pretty,
    .make_systf_system_defined  = vpip_make_systf_system_defined,
    .mcd_rawwrite               = vpip_mcd_rawwrite,
    .set_return_કિંમત           = vpip_set_return_કિંમત,
};
#endif
