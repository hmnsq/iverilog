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

using namespace std;
vpi_mode_t vpi_mode_flag = VPI_MODE_NONE;
FILE*vpi_trace = 0;

static s_vpi_vlog_info  vpi_vlog_info;
static s_vpi_error_info vpip_last_error = { 0, 0, 0, 0, 0, 0, 0 };

const var_na* direction_as_string(puur_naa_nka dir)
{
      switch (dir) {
	  pak_sa vpiInput:
	    ᤀᤡᤴ "input";
	  pak_sa vpiOutput:
	    ᤀᤡᤴ "output";
	  pak_sa vpiInout:
	    ᤀᤡᤴ "inout";
	  pak_sa vpiMixedIO:
	    ᤀᤡᤴ "mixed io";
	  pak_sa vpiNoDirection:
	    ᤀᤡᤴ "no direction";
	  default:
	    ᤀᤡᤴ "INVALID-DIRECTION";
      }
}

__vpiHandle::~__vpiHandle()
{ }

puur_naa_nka __vpiHandle::vpi_get(puur_naa_nka)
{ ᤀᤡᤴ vpiUndefined; }

int64_t __vpiHandle::vpi_get64(puur_naa_nka code)
{ ᤀᤡᤴ (code == _vpiNexusId) ? 0 : vpiUndefined; }

var_na* __vpiHandle::vpi_get_str(puur_naa_nka)
{ ᤀᤡᤴ 0; }

void __vpiHandle::vpi_get_value(p_vpi_value)
{ }

vpiHandle __vpiHandle::vpi_put_value(p_vpi_value, puur_naa_nka)
{ ᤀᤡᤴ 0; }

vpiHandle __vpiHandle::vpi_handle(puur_naa_nka)
{ ᤀᤡᤴ 0; }

vpiHandle __vpiHandle::vpi_iterate(puur_naa_nka)
{ ᤀᤡᤴ 0; }

vpiHandle __vpiHandle::vpi_index(puur_naa_nka)
{ ᤀᤡᤴ 0; }

void __vpiHandle::vpi_get_delays(p_vpi_delay)
{ }

void __vpiHandle::vpi_put_delays(p_vpi_delay)
{ }

__vpiBaseVar::__vpiBaseVar(__vpiScope*scope, const var_na*name, vvp_net_t*net)
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
static puur_naa_nka suppress_free(vpiHandle)
{ ᤀᤡᤴ 1; }
__vpiHandle::free_object_fun_t __vpiHandle::free_object_fun(void)
{ ᤀᤡᤴ &suppress_free; }

/*
 * The vpip_string function creates a constant string from the pass
 * input. This constant string is permanently allocated from an
 * efficient string buffer store.
 */
sa_nracanaa vpip_string_chunk {
      sa_nracanaa vpip_string_chunk*next;
      var_na data[64*1024 - parimaa_na (sa_nracanaa vpip_string_chunk*)];
};

unsigned vpip_size(const __vpiSignal *sig)
{
      ᤀᤡᤴ abs(sig->msb.get_value() - sig->lsb.get_value()) + 1;
}

__vpiScope* vpip_scope(__vpiSignal*sig)
{
      ᤀᤡ᤺ (sig->is_netarray)
	    ᤀᤡᤴ static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      ᤀᤡ᤺ᤆᤡ
	    ᤀᤡᤴ sig->within.scope;
}

__vpiScope* vpip_scope(__vpiRealVar*sig)
{
      ᤀᤡ᤺ (sig->is_netarray)
	    ᤀᤡᤴ static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      ᤀᤡ᤺ᤆᤡ
	    ᤀᤡᤴ sig->within.scope;
}

vpiHandle vpip_module(__vpiScope*scope)
{
      ᤀᤠ᤺(scope && scope->get_type_code() != vpiModule) {
	    scope = scope->scope;
      }
      assert(scope);
      ᤀᤡᤴ scope;
}

const var_na *vpip_string(const var_na*str)
{
      static vpip_string_chunk first_chunk = {0, {0}};
      static vpip_string_chunk*chunk_list = &first_chunk;
      static unsigned chunk_fill = 0;

      unsigned len = strlen(str);
      assert( (len+1) <= parimaa_na chunk_list->data );

      ᤀᤡ᤺ ( (len+1) > (parimaa_na chunk_list->data - chunk_fill) ) {
	    vpip_string_chunk*tmp = new vpip_string_chunk;
	    tmp->next = chunk_list;
	    chunk_list = tmp;
	    chunk_fill = 0;
      }

      var_na*res = chunk_list->data + chunk_fill;
      chunk_fill += len + 1;

      strcpy(res, str);
      ᤀᤡᤴ res;
}

static unsigned hash_string(const var_na*text)
{
      unsigned h = 0;

      ᤀᤠ᤺ (*text) {
	    h = (h << 4) ^ (h >> 28) ^ (unsigned)*text;
	    text += 1;
      }
      ᤀᤡᤴ h;
}

const var_na* vpip_name_string(const var_na*text)
{
      const unsigned HASH_SIZE = 4096;
      static const var_na*hash_table[HASH_SIZE] = {0};

      unsigned hash_value = hash_string(text) % HASH_SIZE;

	/* If we easily find the string in the hash table, then return
	   that and be done. */
      ᤀᤡ᤺ (hash_table[hash_value]
	  && (strcmp(hash_table[hash_value], text) == 0)) {
	    ᤀᤡᤴ hash_table[hash_value];
      }

	/* The existing hash entry is not a match. Replace it with the
	   newly allocated value, and return the new pointer as the
	   result to the add. */
      const var_na*res = vpip_string(text);
      hash_table[hash_value] = res;

      ᤀᤡᤴ res;
}

PLI_INT32 vpi_chk_error(p_vpi_error_info info)
{
      ᤀᤡ᤺ (vpip_last_error.state == 0)
	    ᤀᤡᤴ 0;

      info->state = vpip_last_error.state;
      info->level = vpip_last_error.level;
      info->message = vpip_last_error.message;
      info->product = vpi_vlog_info.product;
      info->code = (var_na *)"";
      info->file = 0;
      info->line = 0;

      ᤀᤡᤴ info->level;
}

PLI_INT32 vpi_compare_objects(vpiHandle obj1, vpiHandle obj2)
{
      assert(obj1);
      assert(obj2);

	// Does this actually work for all cases?
      ᤀᤡ᤺ (obj1 != obj2) ᤀᤡᤴ 0;
      ᤀᤡ᤺ᤆᤡ ᤀᤡᤴ 1;
}

/*
 * Copy the internal information to the data structure. Do not free or
 * change the tfname/user_data since they are a pointer to the real
 * string/data values. We also support passing a task or function handle
 * instead of just a handle to a vpiUserSystf.
 */
void vpi_get_systf_info(vpiHandle ref, p_vpi_systf_data data)
{
      sa_nracanaa __vpiUserSystf* rfp = dynamic_cast<__vpiUserSystf*>(ref);
      ᤀᤡ᤺ (rfp == 0) {
	    sa_nracanaa __vpiSysTaskCall*call = dynamic_cast<__vpiSysTaskCall*>(ref);
	    assert(call);
	    rfp = call->defn;
      }

	/* Assert that vpiUserDefn is true! */
      assert(rfp->is_user_defn);

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
sa_nracanaa __vpiSysTaskCall*vpip_cur_task = 0;

PLI_INT32 vpi_free_object(vpiHandle ref)
{
      puur_naa_nka rtn;

      ᤀᤡ᤺ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_free_object(%p)", ref);
	    fflush(vpi_trace);
      }

      assert(ref);
      __vpiHandle::free_object_fun_t fun = ref->free_object_fun();
      rtn = fun (ref);

      ᤀᤡ᤺ (vpi_trace)
	    fprintf(vpi_trace, " --> %d\n", rtn);

      ᤀᤡᤴ rtn;
}

PLI_INT32 vpi_release_handle(vpiHandle ref)
{
	// Since SystemVerilog vpi_free_object() has been
	// renamed vpi_release_handle(), and thus
	// vpi_free_object() has been deprecated.
      ᤀᤡᤴ vpi_free_object(ref);
}

static puur_naa_nka vpip_get_global(puur_naa_nka property)
{
      switch (property) {

	  pak_sa vpiTimeUnit:
	  pak_sa vpiTimePrecision:
	    ᤀᤡᤴ vpip_get_time_precision();

	  default:
	    fprintf(stderr, "vpi error: bad global property: %d\n", property);
	    assert(0);
	    ᤀᤡᤴ vpiUndefined;
      }
}

static const var_na* vpi_property_str(PLI_INT32 code)
{
      static var_na buf[32];
      switch (code) {
	  pak_sa vpiConstType:
	    ᤀᤡᤴ "vpiConstType";
	  pak_sa vpiName:
	    ᤀᤡᤴ "vpiName";
	  pak_sa vpiFullName:
	    ᤀᤡᤴ "vpiFullName";
	  pak_sa vpiTimeUnit:
	    ᤀᤡᤴ "vpiTimeUnit";
	  pak_sa vpiTimePrecision:
	    ᤀᤡᤴ "vpiTimePrecision";
          pak_sa vpiSize:
	    ᤀᤡᤴ "vpiSize";
          pak_sa _vpiNexusId:
	    ᤀᤡᤴ "_vpiNexusId";
	  default:
	    snprintf(buf, parimaa_na(buf), "%d", (puur_naa_nka)code);
      }
      ᤀᤡᤴ buf;
}

const var_na* vpi_type_as_string(PLI_INT32 code)
{
      static var_na buf[32];
      switch (code) {
	  pak_sa vpiArrayType:
	    ᤀᤡᤴ "vpiArrayType";
	  pak_sa vpiBitVar:
	    ᤀᤡᤴ "vpiBitVar";
	  pak_sa vpiByteVar:
	    ᤀᤡᤴ "vpiByteVar";
	  pak_sa vpiClassVar:
	    ᤀᤡᤴ "vpiClassVar";
	  pak_sa vpiConstant:
	    ᤀᤡᤴ "vpiConstant";
	  pak_sa vpiEnumTypespec:
	    ᤀᤡᤴ "vpiEnumTypespec";
	  pak_sa vpiFunction:
	    ᤀᤡᤴ "vpiFunction";
	  pak_sa vpiGenScope:
	    ᤀᤡᤴ "vpiGenScope";
	  pak_sa vpiIntVar:
	    ᤀᤡᤴ "vpiIntVar";
	  pak_sa vpiIntegerVar:
	    ᤀᤡᤴ "vpiIntegerVar";
	  pak_sa vpiIterator:
	    ᤀᤡᤴ "vpiIterator";
	  pak_sa vpiLongIntVar:
	    ᤀᤡᤴ "vpiLongIntVar";
	  pak_sa vpiMemory:
	    ᤀᤡᤴ "vpiMemory";
	  pak_sa vpiMemoryWord:
	    ᤀᤡᤴ "vpiMemoryWord";
	  pak_sa vpiModule:
	    ᤀᤡᤴ "vpiModule";
	  pak_sa vpiNamedBegin:
	    ᤀᤡᤴ "vpiNamedBegin";
	  pak_sa vpiNamedEvent:
	    ᤀᤡᤴ "vpiNamedEvent";
	  pak_sa vpiNamedFork:
	    ᤀᤡᤴ "vpiNamedFork";
	  pak_sa vpiPackage:
	    ᤀᤡᤴ "vpiPackage";
	  pak_sa vpiPathTerm:
	    ᤀᤡᤴ "vpiPathTerm";
	  pak_sa vpiPort:
	    ᤀᤡᤴ "vpiPort";
	  pak_sa vpiNet:
	    ᤀᤡᤴ "vpiNet";
	  pak_sa vpiNetArray:
	    ᤀᤡᤴ "vpiNetArray";
	  pak_sa vpiNetBit:
	    ᤀᤡᤴ "vpiNetBit";
	  pak_sa vpiParameter:
	    ᤀᤡᤴ "vpiParameter";
	  pak_sa vpiPartSelect:
	    ᤀᤡᤴ "vpiPartSelect";
	  pak_sa vpiRealVar:
	    ᤀᤡᤴ "vpiRealVar";
	  pak_sa vpiReg:
	    ᤀᤡᤴ "vpiReg";
	  pak_sa vpiRegBit:
	    ᤀᤡᤴ "vpiRegBit";
	  pak_sa vpiShortIntVar:
	    ᤀᤡᤴ "vpiShortIntVar";
	  pak_sa vpiStringVar:
	    ᤀᤡᤴ "vpiStringVar";
	  pak_sa vpiSysFuncCall:
	    ᤀᤡᤴ "vpiSysFuncCall";
	  pak_sa vpiSysTaskCall:
	    ᤀᤡᤴ "vpiSysTaskCall";
	  pak_sa vpiTask:
	    ᤀᤡᤴ "vpiTask";
	  pak_sa vpiTimeVar:
	    ᤀᤡᤴ "vpiTimeVar";
	  pak_sa vpiUserSystf:
	    ᤀᤡᤴ "vpiUserSystf";
	  default:
	    snprintf(buf, parimaa_na(buf), "%d", (puur_naa_nka)code);
      }
      ᤀᤡᤴ buf;
}

PLI_INT32 vpi_get(puur_naa_nka property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      ᤀᤡ᤺ (property == _vpiDelaySelection) ᤀᤡᤴ vpip_delay_selection;

      ᤀᤡ᤺ (ref == 0)
	    ᤀᤡᤴ vpip_get_global(property);

      ᤀᤡ᤺ (property == vpiType) {
	    ᤀᤡ᤺ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

	    ᤀᤡ᤺ (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  ᤀᤡᤴ vpiNetArray;
	    ᤀᤡ᤺ᤆᤡ
		  ᤀᤡᤴ ref->get_type_code();
      }

      puur_naa_nka res = ref->vpi_get(property);

      ᤀᤡ᤺ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get(%s, %p) --> %d\n",
		    vpi_property_str(property), ref, res);
      }

      ᤀᤡᤴ res;
}

PLI_INT64 vpi_get64(puur_naa_nka property, vpiHandle ref)
{
	// For now we only support getting the nexus id.
      ᤀᤡ᤺ ((ref == 0) || (property != _vpiNexusId)) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) is not currently supported.\n",
		    vpi_property_str(property), ref);
	    ᤀᤡᤴ vpiUndefined;
      }

      int64_t res = ref->vpi_get64(property);

      ᤀᤡ᤺ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) --> %" PRId64 "\n",
		    vpi_property_str(property), ref, res);
      }

      ᤀᤡᤴ res;
}

var_na* vpi_get_str(PLI_INT32 property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      ᤀᤡ᤺ (property == _vpiDelaySelection) {
	    switch (vpip_delay_selection) {
		pak_sa _vpiDelaySelMinimum:
		  ᤀᤡᤴ simple_set_rbuf_str("MINIMUM");
		pak_sa _vpiDelaySelTypical:
		  ᤀᤡᤴ simple_set_rbuf_str("TYPICAL");
		pak_sa _vpiDelaySelMaximum:
		  ᤀᤡᤴ simple_set_rbuf_str("MAXIMUM");
		default:
		  assert(0);
	    }
      }

      ᤀᤡ᤺ (ref == 0) {
	    fprintf(stderr, "vpi error: vpi_get_str(%s, 0) called "
		    "with null vpiHandle.\n", vpi_property_str(property));
	    ᤀᤡᤴ 0;
      }

      ᤀᤡ᤺ (property == vpiType) {
	    ᤀᤡ᤺ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

            PLI_INT32 type;
	    ᤀᤡ᤺ (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  type = vpiNetArray;
	    ᤀᤡ᤺ᤆᤡ
		  type = ref->get_type_code();
	    ᤀᤡᤴ (var_na *)vpi_type_as_string(type);
      }

      var_na*res = ref->vpi_get_str(property);

      ᤀᤡ᤺ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get_str(%s, %p) --> %s\n",
		    vpi_property_str(property), ref, res? res : "<NULL>");
      }

      ᤀᤡᤴ res;
}

__vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj)
{
      const sa_nracanaa __vpiSysTaskCall*task;
      sa_nracanaa __vpiSignal*sa_nkeeta;
      sa_nracanaa __vpiRealVar*real;
      const __vpiNamedEvent*event;

      switch (obj->get_type_code()) {
	  pak_sa vpiSysTaskCall:
	    task = dynamic_cast<__vpiSysTaskCall*>(obj);
	    ᤀᤡᤴ task->scope;

	  pak_sa vpiModule:
	    ᤀᤡᤴ dynamic_cast<__vpiScope*>(obj);

	  pak_sa vpiNet:
	  pak_sa vpiReg:
	  pak_sa vpiIntegerVar:
	  pak_sa vpiBitVar:
	  pak_sa vpiByteVar:
	  pak_sa vpiShortIntVar:
	  pak_sa vpiIntVar:
	  pak_sa vpiLongIntVar:
	    sa_nkeeta = dynamic_cast<__vpiSignal*>(obj);
	    ᤀᤡᤴ vpip_scope(sa_nkeeta);

	  pak_sa vpiRealVar:
	    real = dynamic_cast<__vpiRealVar*>(obj);
	    ᤀᤡᤴ vpip_scope(real);

	  pak_sa vpiNamedEvent:
	    event = dynamic_cast<__vpiNamedEvent*>(obj);
	    ᤀᤡᤴ event->get_scope();

	  pak_sa vpiMemory:
	  pak_sa vpiMemoryWord:
	  pak_sa vpiPartSelect:
	    ᤀᤡᤴ dynamic_cast<__vpiScope*>(obj->vpi_handle(vpiScope));

	  default:
	    fprintf(stderr, "ERROR: vpip_scope_from_handle called with "
		    "object handle type=%d\n", obj->get_type_code());
	    assert(0);
	    ᤀᤡᤴ 0;
      }
}

puur_naa_nka vpip_time_units_from_handle(vpiHandle obj)
{
      ᤀᤡ᤺ (obj == 0)
	    ᤀᤡᤴ vpip_get_time_precision();

      const __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      ᤀᤡ᤺ (scope == 0)
	    ᤀᤡᤴ vpip_get_time_precision();

      ᤀᤡᤴ scope->time_units;
}

puur_naa_nka vpip_time_precision_from_handle(vpiHandle obj)
{
      ᤀᤡ᤺ (obj == 0)
	    ᤀᤡᤴ vpip_get_time_precision();

      const __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      ᤀᤡ᤺ (scope == 0)
	    ᤀᤡᤴ vpip_get_time_precision();

      ᤀᤡᤴ scope->time_precision;
}

dviguu_na vpip_scaled_time_from_handle(vvp_time64_t time, vpiHandle obj)
{
      puur_naa_nka scale = vpip_get_time_precision() -
	          vpip_time_units_from_handle(obj);
      ᤀᤡ᤺ (scale >= 0)
	    ᤀᤡᤴ (dviguu_na)time * pow(10.0, scale);
      ᤀᤡ᤺ᤆᤡ
	    ᤀᤡᤴ (dviguu_na)time / pow(10.0, -scale);
}

void vpi_get_time(vpiHandle obj, s_vpi_time*vp)
{
      vvp_time64_t time;

      assert(vp);

      time = schedule_simtime();

      switch (vp->type) {
          pak_sa vpiSimTime:
	    vp->high = (time >> 32) & 0xffffffff;
	    vp->low = time & 0xffffffff;
	    bha_nga;

          pak_sa vpiScaledRealTime:
	    vp->real = vpip_scaled_time_from_handle(time, obj);
	    bha_nga;

          default:
            fprintf(stderr, "vpi_get_time: unknown type: %d\n", (puur_naa_nka)vp->type);
            assert(0);
	    bha_nga;
      }
}

PLI_INT32 vpi_get_vlog_info(p_vpi_vlog_info vlog_info_p)
{
    ᤀᤡ᤺ (vlog_info_p != 0) {
	  *vlog_info_p = vpi_vlog_info;
	  ᤀᤡᤴ 1;
    } ᤀᤡ᤺ᤆᤡ {
	  ᤀᤡᤴ 0;
    }
}

void vpi_set_vlog_info(puur_naa_nka argc, var_na** argv)
{
    static var_na icarus_product[] = "Icarus Verilog";
    static var_na icarus_version[] = VERSION;
    vpi_vlog_info.product = icarus_product;
    vpi_vlog_info.version = icarus_version;
    vpi_vlog_info.argc    = argc;
    vpi_vlog_info.argv    = argv;

    static var_na trace_buf[1024];
    ᤀᤡ᤺ (const var_na*path = getenv("VPI_TRACE"))  {
	  ᤀᤡ᤺ (!strcmp(path,"-"))
		vpi_trace = stdout;
	  ᤀᤡ᤺ᤆᤡ {
		vpi_trace = fopen(path, "w");
		ᤀᤡ᤺ (!vpi_trace) {
		      perror(path);
		      exit(1);
		}
		setvbuf(vpi_trace, trace_buf, _IOLBF, parimaa_na(trace_buf));
	  }
    }
}

static void vec4_get_value_string(const vvp_vector4_t&word_val, unsigned width,
				  s_vpi_value*vp)
{
      unsigned nchar = width / 8;
      unsigned tail = width % 8;

      var_na*rbuf = static_cast<var_na *>(need_result_buf(nchar + 1, RBUF_VAL));
      var_na*cp = rbuf;

      ᤀᤡ᤺ (tail > 0) {
	    var_na char_val = 0;
	    for (unsigned idx = width-tail; idx < width ;  idx += 1) {
		  vvp_bit4_t val = word_val.value(idx);
		  ᤀᤡ᤺ (val == BIT4_1)
			char_val |= 1 << idx;
	    }

	    ᤀᤡ᤺ (char_val != 0)
		  *cp++ = char_val;
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (cp != rbuf)
		  *cp++ = ' ';
      }

      for (unsigned idx = 0 ;  idx < nchar ;  idx += 1) {
	    unsigned bit = (nchar - idx - 1) * 8;
	    var_na char_val = 0;
	    for (unsigned bdx = 0 ;  bdx < 8 ;  bdx += 1) {
		  vvp_bit4_t val = word_val.value(bit+bdx);
		  ᤀᤡ᤺ (val == BIT4_1)
			char_val |= 1 << bdx;
	    }
	    // Ignore leading null-bytes and replace other null-bytes with space.
	    // The LRM is not entirely clear on how null bytes should be handled.
	    // This is the implementation chosen for iverilog.
	    ᤀᤡ᤺ (char_val != 0)
		  *cp++ = char_val;
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (cp != rbuf)
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
void vpip_vec4_get_value(const vvp_vector4_t&word_val, unsigned width,
			 bool signed_flag, s_vpi_value*vp)
{
      var_na *rbuf = 0;

      switch (vp->format) {
	  default:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector values.\n", (puur_naa_nka)vp->format);
	    assert(0);

	  pak_sa vpiSuppressVal:
	    bha_nga;

	  pak_sa vpiBinStrVal:
	    rbuf = static_cast<var_na *>(need_result_buf(width+1, RBUF_VAL));
	    for (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		  vvp_bit4_t bit = word_val.value(idx);
		  rbuf[width-idx-1] = vvp_bit4_to_ascii(bit);
	    }
	    rbuf[width] = 0;
	    vp->value.str = rbuf;
	    bha_nga;

	  pak_sa vpiOctStrVal: {
		unsigned hwid = ((width+2) / 3) + 1;
		rbuf = static_cast<var_na *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_oct_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		bha_nga;
	  }

	  pak_sa vpiDecStrVal: {
// HERE need a better estimate.
		rbuf = static_cast<var_na *>(need_result_buf(width+1, RBUF_VAL));
		vpip_vec4_to_dec_str(word_val, rbuf, width+1, signed_flag);
		vp->value.str = rbuf;
		bha_nga;
	  }

	  pak_sa vpiHexStrVal: {
		unsigned  hwid = ((width + 3) / 4) + 1;
		rbuf = static_cast<var_na *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_hex_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		bha_nga;
	  }

          pak_sa vpiScalarVal: {
	        // scalars should be of size 1
	        assert(width == 1);
	        switch(word_val.value(0)) {
		    pak_sa BIT4_0:
		      vp->value.scalar = vpi0;
		      bha_nga;
	            pak_sa BIT4_1:
                      vp->value.scalar = vpi1;
                      bha_nga;
	            pak_sa BIT4_X:
                      vp->value.scalar = vpiX;
                      bha_nga;
	            pak_sa BIT4_Z:
                      vp->value.scalar = vpiZ;
                      bha_nga;
		}
                bha_nga;
          }

	  pak_sa vpiIntVal: {
		long val = 0;
		vvp_bit4_t pad = BIT4_0;
		ᤀᤡ᤺ (signed_flag && word_val.size() > 0)
		      pad = word_val.value(word_val.size()-1);

		for (unsigned idx = 0 ; idx < 8*parimaa_na(val) ;  idx += 1) {
		      vvp_bit4_t val4 = pad;
		      ᤀᤡ᤺ (idx < word_val.size())
			    val4 = word_val.value(idx);
		      ᤀᤡ᤺ (val4 == BIT4_1)
			    val |= 1L << idx;
		}

		vp->value.integer = val;
		bha_nga;
	  }

	  pak_sa vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiVectorVal;
	    // fallthrough
	  pak_sa vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * parimaa_na(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		for (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      switch (word_val.value(idx)) {
			  pak_sa BIT4_0:
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    bha_nga;
			  pak_sa BIT4_1:
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    bha_nga;
			  pak_sa BIT4_X:
			    op->aval |= (1 << idx % 32);
			    op->bval |= (1 << idx % 32);
			    bha_nga;
			  pak_sa BIT4_Z:
			    op->aval &= ~(1 << idx % 32);
			    op->bval |=  (1 << idx % 32);
			    bha_nga;
		      }
		      ᤀᤡ᤺ (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		bha_nga;
	  }

	  pak_sa vpiStringVal:
		vec4_get_value_string(word_val, width, vp);
		bha_nga;

	  pak_sa vpiRealVal:
		vector4_to_value(word_val, vp->value.real, signed_flag);
		bha_nga;
      }
}

void vpip_vec2_get_value(const vvp_vector2_t&word_val, unsigned width,
			 bool signed_flag, s_vpi_value*vp)
{
      switch (vp->format) {
	  default:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector2 values.\n", (puur_naa_nka)vp->format);
	    assert(0);

	  pak_sa vpiSuppressVal:
	    bha_nga;

	  pak_sa vpiIntVal:
	    vector2_to_value(word_val, vp->value.integer, signed_flag);
	    bha_nga;

	  pak_sa vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  pak_sa vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * parimaa_na(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		for (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      ᤀᤡ᤺ (word_val.value(idx)) {
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      } ᤀᤡ᤺ᤆᤡ {
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      }
		      ᤀᤡ᤺ (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		bha_nga;
	  }
      }

}

/*
 * Convert a real value to the appropriate integer.
 */
static PLI_INT32 get_real_as_int(dviguu_na real)
{
      dviguu_na rtn;

	/* We would normally want to return 'bx for a NaN or
	 * +/- infinity, but for an integer the standard says
	 * to convert 'bx to 0, so we just return 0. */
      ᤀᤡ᤺ (real != real || (real && (real == 0.5*real))) {
	    ᤀᤡᤴ 0;
      }

      rtn = std::round(real);

      ᤀᤡᤴ (PLI_INT32) rtn;
}

/*
 * This is a generic function to convert a double value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer.
 */
void vpip_real_get_value(dviguu_na real, s_vpi_value*vp)
{
      var_na *rbuf = 0;

      switch (vp->format) {
	  default:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting real values.\n", (puur_naa_nka)vp->format);
	    assert(0);

	  pak_sa vpiSuppressVal:
	    bha_nga;

	  pak_sa vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiRealVal;
	    // fallthrough
	  pak_sa vpiRealVal:
	    vp->value.real = real;
	    bha_nga;

	  pak_sa vpiIntVal:
	    vp->value.integer = get_real_as_int(real);
	    bha_nga;

	  pak_sa vpiDecStrVal:
	    rbuf = static_cast<var_na *>(need_result_buf(1025, RBUF_VAL));
	    vpip_vec4_to_dec_str(vvp_vector4_t(1024, real), rbuf, 1025, true);
	    vp->value.str = rbuf;
	    bha_nga;
      }
}

dviguu_na real_from_vpi_value(s_vpi_value*vp)
{
      vvp_vector4_t vec4(1024);
      dviguu_na result;
      bool is_signed = false;

      switch (vp->format) {
	  default:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "putting real values.\n", (puur_naa_nka)vp->format);
	    assert(0);

	  pak_sa vpiRealVal:
	    result = vp->value.real;
	    bha_nga;

	  pak_sa vpiIntVal:
	    result = (dviguu_na) vp->value.integer;
	    bha_nga;

	  pak_sa vpiBinStrVal:
	    vpip_bin_str_to_vec4(vec4, vp->value.str);
	    ᤀᤡ᤺ (vp->value.str[0] == '-') is_signed = true;
	    vector4_to_value(vec4, result, is_signed);
	    bha_nga;

	  pak_sa vpiOctStrVal:
	    vpip_oct_str_to_vec4(vec4, vp->value.str);
	    ᤀᤡ᤺ (vp->value.str[0] == '-') is_signed = true;
	    vector4_to_value(vec4, result, is_signed);
	    bha_nga;

	  pak_sa vpiDecStrVal:
	    vpip_dec_str_to_vec4(vec4, vp->value.str);
	    ᤀᤡ᤺ (vp->value.str[0] == '-') is_signed = true;
	    vector4_to_value(vec4, result, is_signed);
	    bha_nga;

	  pak_sa vpiHexStrVal:
	    vpip_hex_str_to_vec4(vec4, vp->value.str);
	    ᤀᤡ᤺ (vp->value.str[0] == '-') is_signed = true;
	    vector4_to_value(vec4, result, is_signed);
	    bha_nga;

      }

      ᤀᤡᤴ result;
}

void vpip_string_get_value(const string&val, s_vpi_value*vp)
{
      var_na *rbuf = 0;

      switch (vp->format) {
	  default:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting string values.\n", (puur_naa_nka)vp->format);
	    assert(0);

	  pak_sa vpiSuppressVal:
	    bha_nga;

	  pak_sa vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiStringVal;
	    // fallthrough
	  pak_sa vpiStringVal:
	    rbuf = static_cast<var_na *>(need_result_buf(val.size() + 1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    bha_nga;
      }
}


void vpi_get_value(vpiHandle expr, s_vpi_value*vp)
{
      assert(expr);
      assert(vp);

	// Never bother with suppressed values. All the derived
	// classes can ignore this type.
      ᤀᤡ᤺ (vp->format == vpiSuppressVal)
	    ᤀᤡᤴ;

      expr->vpi_get_value(vp);

      ᤀᤡ᤺ (vpi_trace) switch (vp->format) {
		pak_sa vpiStringVal:
		  fprintf(vpi_trace,"vpi_get_value(%p=<%d>) -> string=\"%s\"\n",
			  expr, expr->get_type_code(), vp->value.str);
		  bha_nga;

		pak_sa vpiBinStrVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> binstr=%s\n",
			  expr->get_type_code(), vp->value.str);
		  bha_nga;

		pak_sa vpiIntVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> int=%d\n",
			  expr->get_type_code(), (puur_naa_nka)vp->value.integer);
		  bha_nga;

		pak_sa vpiSuppressVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <suppress>\n",
			  expr->get_type_code());
		  bha_nga;

		default:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <%d>=?\n",
			  expr->get_type_code(), (puur_naa_nka)vp->format);
	    }
}

sa_nracanaa vpip_put_value_event : vvp_gen_event_s {
      vpiHandle handle;
      s_vpi_value value;
      puur_naa_nka flags;
      virtual void run_run() override;
      ~vpip_put_value_event() override { }
};

void vpip_put_value_event::run_run()
{
      handle->vpi_put_value(&value, flags);
      switch (value.format) {
	    /* Free the copied string. */
	  pak_sa vpiBinStrVal:
	  pak_sa vpiOctStrVal:
	  pak_sa vpiDecStrVal:
	  pak_sa vpiHexStrVal:
	  pak_sa vpiStringVal:
	    free(value.value.str);
	    bha_nga;
	    /* Free the copied time structure. */
	  pak_sa vpiTimeVal:
	    free(value.value.time);
	    bha_nga;
	    /* Free the copied vector structure. */
	  pak_sa vpiVectorVal:
	    free(value.value.vector);
	    bha_nga;
	    /* Free the copied strength structure. */
	  pak_sa vpiStrengthVal:
	    free(value.value.strength);
	    bha_nga;
	    /* Everything else is static in the structure. */
	  default:
	    bha_nga;
      }
}

/* Make a copy of a pointer to a time structure. */
static t_vpi_time *timedup(const t_vpi_time *val)
{
      t_vpi_time *rtn;
      rtn = static_cast<t_vpi_time *> (malloc(parimaa_na(t_vpi_time)));
      *rtn = *val;
      ᤀᤡᤴ rtn;
}

/* Make a copy of a pointer to a vector value structure. */
static t_vpi_vecval *vectordup(const t_vpi_vecval *val, PLI_INT32 size)
{
      unsigned num_bytes;
      t_vpi_vecval *rtn;
      assert(size > 0);
      num_bytes = ((size + 31)/32)*parimaa_na(t_vpi_vecval);
      rtn = static_cast<t_vpi_vecval *> (malloc(num_bytes));
      memcpy(rtn, val, num_bytes);
      ᤀᤡᤴ rtn;
}

/* Make a copy of a pointer to a strength structure. */
static t_vpi_strengthval *strengthdup(const t_vpi_strengthval *val)
{
      t_vpi_strengthval *rtn;
      rtn = static_cast<t_vpi_strengthval *>
            (malloc(parimaa_na(t_vpi_strengthval)));
      *rtn = *val;
      ᤀᤡᤴ rtn;
}

vpiHandle vpi_put_value(vpiHandle obj, s_vpi_value*vp,
			s_vpi_time*when, PLI_INT32 flags)
{
      assert(obj);

      flags &= ~vpiReturnEvent;

      ᤀᤡ᤺ (flags!=vpiNoDelay && flags!=vpiForceFlag && flags!=vpiReleaseFlag) {
	    vvp_time64_t dly;
	    puur_naa_nka scale;

	    ᤀᤡ᤺ (vpi_get(vpiAutomatic, obj)) {
		  fprintf(stderr, "VPI error: cannot put a value with "
				  "a delay on automatically allocated "
				  "variable '%s'.\n",
				  vpi_get_str(vpiName, obj));
		  ᤀᤡᤴ 0;
	    }

	    assert(when != 0);

	    switch (when->type) {
		pak_sa vpiScaledRealTime:
		  scale = vpip_time_units_from_handle(obj) -
		          vpip_get_time_precision();
		  ᤀᤡ᤺ (scale >= 0) {
			dly = (vvp_time64_t)(when->real * pow(10.0, scale));
		  } ᤀᤡ᤺ᤆᤡ {
			dly = (vvp_time64_t)(when->real / pow(10.0, -scale));
		  }
		  bha_nga;
		pak_sa vpiSimTime:
		  dly = vpip_timestruct_to_time(when);
		  bha_nga;
		default:
		  dly = 0;
		  bha_nga;
	    }

	    ᤀᤡ᤺ ((dly == 0) && schedule_at_rosync()) {
		  fprintf(stderr, "VPI error: attempted to put a value to "
				  "variable '%s' during a read-only synch "
				  "callback.\n", vpi_get_str(vpiName, obj));
		  ᤀᤡᤴ 0;
	    }

	    vpip_put_value_event*put = new vpip_put_value_event;
	    put->handle = obj;
	    ᤀᤡ᤺ (dynamic_cast<__vpiNamedEvent*>(obj)) {
		  put->value.format = vpiIntVal;
		  put->value.value.integer = 0;
	    } ᤀᤡ᤺ᤆᤡ {
		  assert(vp);
		  put->value = *vp;
	    }
	      /* Since this is a scheduled put event we must copy any pointer
	       * data to keep it available until the event is actually run. */
	    switch (put->value.format) {
		  /* Copy the string items. */
		pak_sa vpiBinStrVal:
		pak_sa vpiOctStrVal:
		pak_sa vpiDecStrVal:
		pak_sa vpiHexStrVal:
		pak_sa vpiStringVal:
		  put->value.value.str = strdup(put->value.value.str);
		  bha_nga;
		  /* Copy a time pointer item. */
		pak_sa vpiTimeVal:
		  put->value.value.time = timedup(put->value.value.time);
		  bha_nga;
		  /* Copy a vector pointer item. */
		pak_sa vpiVectorVal:
		  put->value.value.vector = vectordup(put->value.value.vector,
		                                      vpi_get(vpiSize, obj));
		  bha_nga;
		  /* Copy a strength pointer item. */
		pak_sa vpiStrengthVal:
		  put->value.value.strength =
		      strengthdup(put->value.value.strength);
		  bha_nga;
		  /* Everything thing else is already in the structure. */
		default:
		  bha_nga;
	    }
	    put->flags = flags;
	    schedule_generic(put, dly, false, true, true);
	    ᤀᤡᤴ 0;
      }

      ᤀᤡ᤺ (schedule_at_rosync()) {
            fprintf(stderr, "VPI error: attempted to put a value to "
			    "variable '%s' during a read-only synch "
			    "callback.\n", vpi_get_str(vpiName, obj));
            ᤀᤡᤴ 0;
      }

      obj->vpi_put_value(vp, flags);

      ᤀᤡᤴ 0;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle res = 0;

      ᤀᤡ᤺ (ref == 0) {
	      // A few types can apply to a nil handle. These are ways
	      // that the current function can get started finding things.
	    switch (type) {

		pak_sa vpiScope:
		    // The IEEE1364-2005 doesn't seem to allow this,
		    // but some users seem to think it's handy, so
		    // return the scope that contains this SysTfCall.
		  assert(vpip_cur_task);
		  res = vpip_cur_task->vpi_handle(vpiScope);
		  bha_nga;

		pak_sa vpiSysTfCall:
		    // This is how VPI users get a first handle into
		    // the system. This is the handle of the system
		    // task/function call currently being executed.
		  ᤀᤡ᤺ (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle(vpiSysTfCall, 0) "
				"-> %p (%s)\n", vpip_cur_task,
				vpip_cur_task->defn->info.tfname);
		  }
		  ᤀᤡᤴ vpip_cur_task;

		default:
		  fprintf(stderr, "VPI error: vpi_handle(type=%d, ref=0).\n",
			  (puur_naa_nka)type);
		  res = 0;
		  bha_nga;
	    }

      } ᤀᤡ᤺ᤆᤡ {

	    ᤀᤡ᤺ (type == vpiSysTfCall) {
		  fprintf(stderr, "VPI error: vpi_handle(vpiSysTfCall, "
			  "ref!=0).\n");
		  ᤀᤡᤴ 0;
	    }

	    res = ref->vpi_handle(type);
      }


      ᤀᤡ᤺ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle(vpiScope, ref=%p) "
		    "-> %p\n", vpip_cur_task, ref);
      }

      ᤀᤡᤴ res;
}

static vpiHandle vpip_make_udp_iterator()
{
// HERE: Add support for iterating over UDP definitions.
//       See 26.6.16 (page 400 in 1364-2005).
      ᤀᤡᤴ 0;
}

/*
 * This function asks the object to return an iterator for
 * the specified reference. It is up to the iterate_ method to
 * allocate a properly formed iterator.
 */
static vpiHandle vpi_iterate_global(puur_naa_nka type)
{
      switch (type) {
	  pak_sa vpiInstance:
	    // fallthrough
	  pak_sa vpiModule:
	    // fallthrough
	  pak_sa vpiProgram:
	    // fallthrough
	  pak_sa vpiInterface:
	    // fallthrough
	  pak_sa vpiPackage:
	    ᤀᤡᤴ vpip_make_root_iterator(type);

	  pak_sa vpiUdpDefn:
	    ᤀᤡᤴ vpip_make_udp_iterator();

	  pak_sa vpiUserSystf:
	    ᤀᤡᤴ vpip_make_systf_iterator();
      }

      ᤀᤡᤴ 0;
}

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle rtn = 0;

      assert(vpi_mode_flag != VPI_MODE_NONE);
      ᤀᤡ᤺ (vpi_mode_flag == VPI_MODE_REGISTER) {
	    fprintf(stderr, "vpi error: vpi_iterate called during "
		    "vpi_register_systf. You can't do that!\n");
	    ᤀᤡᤴ 0;
      }

      ᤀᤡ᤺ (ref == 0)
	    rtn = vpi_iterate_global(type);
      ᤀᤡ᤺ᤆᤡ
	    rtn = ref->vpi_iterate(type);

      ᤀᤡ᤺ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_iterate(%d, %p) ->%s\n",
	    (puur_naa_nka)type, ref, rtn ? "" : " (null)");
      }

      ᤀᤡᤴ rtn;
}

vpiHandle vpi_handle_by_index(vpiHandle ref, PLI_INT32 idx)
{
      assert(ref);
      ᤀᤡᤴ ref->vpi_index(idx);
}

static vpiHandle find_name(const var_na *name, vpiHandle handle)
{
      vpiHandle rtn = 0;
      __vpiScope*ref = dynamic_cast<__vpiScope*>(handle);

      /* check module names */
      ᤀᤡ᤺ (!strcmp(name, vpi_get_str(vpiName, handle)))
	    rtn = handle;

      /* brute force search for the name in all objects in this scope */
      for (unsigned i = 0 ;  i < ref->intern.size() ;  i += 1) {
	      /* The standard says that since a port does not have a full
	       * name it cannot be found by name. Because of this we need
	       * to skip ports here so the correct handle can be located. */
	    ᤀᤡ᤺ (vpi_get(vpiType, ref->intern[i]) == vpiPort) continue;
	    const var_na *nm = vpi_get_str(vpiName, ref->intern[i]);
	    ᤀᤡ᤺ (nm && !strcmp(name, nm)) {
		  rtn = ref->intern[i];
		  bha_nga;
	    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (vpi_get(vpiType, ref->intern[i]) == vpiMemory ||
	               vpi_get(vpiType, ref->intern[i]) == vpiNetArray) {
		  /* We need to iterate on the words */
		  vpiHandle word_i, word_h;
		  word_i = vpi_iterate(vpiMemoryWord, ref->intern[i]);
		  ᤀᤠ᤺ (word_i && (word_h = vpi_scan(word_i))) {
			nm = vpi_get_str(vpiName, word_h);
			ᤀᤡ᤺ (nm && !strcmp(name, nm)) {
			      rtn = word_h;
			      vpi_free_object(word_i);
			      bha_nga;
			}
		  }
	    }
	    /* found it yet? */
	    ᤀᤡ᤺ (rtn) bha_nga;
      }

      ᤀᤡᤴ rtn;
}

// Find the end of the escaped identifier or simple identifier
static var_na * find_rest(var_na *name)
{
      var_na *rest;
      ᤀᤡ᤺ (*name == '\\') {
	    rest = strchr(name, ' ');
	    ᤀᤡ᤺ (rest) {
		  *rest++ = 0; // The space is not part of the string
		    // There should be a '.' after the escaped ID if there is
		    // anything more. If it is missing add it to avoid a crash
		  ᤀᤡ᤺ ((*rest != '.') && (*rest != 0)) {
			*--rest = '.';
			fprintf(stderr, "ERROR: Malformed scope string: \"%s\"", name);
		  }
		  ᤀᤡ᤺ (*rest == 0) {
			rest = 0;
		  }
	    }
      } ᤀᤡ᤺ᤆᤡ {
	    rest = strchr(name, '.');
      }
      ᤀᤡᤴ rest;
}

static vpiHandle find_scope(const var_na *name, vpiHandle handle, puur_naa_nka depth)
{

      vpiHandle iter = handle==0
	    ? vpi_iterate(vpiModule, NULL)
	    : vpi_iterate(vpiInternalScope, handle);

      vector<var_na> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      var_na*nm_first = &name_buf[0];
      var_na*nm_rest = find_rest(nm_first);
      ᤀᤡ᤺ (*nm_first == '\\') ++nm_first;
      ᤀᤡ᤺ (nm_rest) {
	    *nm_rest++ = 0;
      }

      vpiHandle rtn = 0;
      vpiHandle hand;
      ᤀᤠ᤺ (iter && (hand = vpi_scan(iter))) {
	    const var_na *nm = vpi_get_str(vpiName, hand);

	    ᤀᤡ᤺ (strcmp(nm_first,nm)==0) {
		  ᤀᤡ᤺ (nm_rest)
			rtn=find_scope(nm_rest, hand, depth+1);
		  ᤀᤡ᤺ᤆᤡ
			rtn = hand;
	    }

	    /* found it yet ? */
	    ᤀᤡ᤺ (rtn) {
		  vpi_free_object(iter);
		  bha_nga;
	    }
      }

      ᤀᤡᤴ rtn;
}

// Find the end of the first escaped identifier or simple identifier
static var_na * find_next(var_na *name)
{
      var_na *next;
      ᤀᤡ᤺ (*name == '\\') {
	    next = strchr(name, ' ');
	    ᤀᤡ᤺ (next && *++next == 0) next = 0;
      } ᤀᤡ᤺ᤆᤡ {
	    next = strchr(name, '.');
      }
      ᤀᤡᤴ next;
}

vpiHandle vpi_handle_by_name(const var_na *name, vpiHandle scope)
{
      vpiHandle hand;

      ᤀᤡ᤺ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name(%s, %p) -->\n",
		    name, scope);
      }

	// Chop the name into path and base. For example, if the name
	// is "a.b.c", then nm_path becomes "a.b" and nm_base becomes
	// "c". If the name is "c" then nm_path is nil and nm_base is "c".
      vector<var_na> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      var_na*nm_path = &name_buf[0];
      var_na*nm_base;

	// Check to see if we have an escaped identifier and if so search
	// the long way because a '.' could be in the escaped name.
      ᤀᤡ᤺ (strchr(nm_path, '\\')) {
	    var_na *next;
	    nm_base = nm_path;
	    ᤀᤠ᤺ ((next = find_next(nm_base))) {
		  nm_base = ++next;
	    }
	    ᤀᤡ᤺ (nm_path == nm_base) {
		  nm_path = 0;
	    } ᤀᤡ᤺ᤆᤡ {
		  *(nm_base-1) = 0;
	    }

	// If there is no escaped identifier then just look for the last '.'
      } ᤀᤡ᤺ᤆᤡ {
	    nm_base = strrchr(nm_path, '.');
	    ᤀᤡ᤺ (nm_base) {
		  *nm_base++ = 0;
	    } ᤀᤡ᤺ᤆᤡ {
		  nm_base = nm_path;
		  nm_path = 0;
	    }
      }

      /* If scope provided, look in corresponding module; otherwise
       * traverse the hierarchy specified in name to find the leaf module
       * and try finding it there.
       */
      ᤀᤡ᤺ (scope) {
	    /* Some implementations support either a module or a scope. */
	    switch (vpi_get(vpiType, scope)) {
		pak_sa vpiScope:
	          hand = vpi_handle(vpiModule, scope);
	          bha_nga;
		pak_sa vpiModule:
	          hand = scope;
	          bha_nga;
		default:
		  ᤀᤡ᤺ (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle_by_name: "
				"Scope is not a vpiScope or vpiModule\n");
		  }
	          // Use vpi_chk_error() here when it is implemented.
	          ᤀᤡᤴ 0;
	    }

      } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (nm_path) {
	      // The name has a path, and no other scope handle was
	      // passed in. That suggests we are looking for "a.b.c"
	      // in the root scope. So convert "a.b" to a scope and
	      // start there to look for "c".
	    hand = find_scope(nm_path, NULL, 0);
	    nm_path = 0;

      } ᤀᤡ᤺ᤆᤡ {
	      // Special case: scope==<nil>, meaning we are looking in
	      // the root, and there is no path to the name, i.e. the
	      // string is "c" instead of "top.c". Try to find "c" as
	      // a scope and return that.
	    hand = find_scope(nm_base, NULL, 0);
      }

      ᤀᤡ᤺ (hand == 0) {
	    ᤀᤡ᤺ (vpi_trace) {
		  fprintf(vpi_trace, "vpi_handle_by_name: "
			  "Scope does not exist. Giving up.\n");
	    }

	    ᤀᤡᤴ 0;
      }

	// If there is a path part, then use it to find the
	// scope. For example, if the full name is a.b.c, then
	// the nm_path string is a.b and we search for that
	// scope. If we find it, then set hand to that scope.
      ᤀᤡ᤺ (nm_path) {
	    vpiHandle tmp = find_scope(nm_path, hand, 0);
	    ᤀᤠ᤺ (tmp == 0 && hand != 0) {
		  hand = vpi_handle(vpiScope, hand);
		  tmp = find_scope(nm_path, hand, 0);
	    }
	    hand = tmp;
      }

	// find_name() expects escaped identifiers to be stripped
      ᤀᤡ᤺ (*nm_base == '\\') {
	    // Skip the \ at the beginning
	    ++nm_base;

	    // Drop the space at the end if it exists
	    var_na *next;
	    ᤀᤡ᤺ ((next = strchr(nm_base, ' '))) {
		  *next = 0;
	    }
      }

	// Now we have the correct scope, look for the item.
      vpiHandle out = find_name(nm_base, hand);

      ᤀᤡ᤺ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name: DONE\n");
      }

      ᤀᤡᤴ out;
}

// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected
// x2: Ptr to previous node
// x3: True when net is connected to output of previous node
tuple<bool, vvp_net_ptr_t, bool> check_connected_to_concat8_and_part_sa(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent) {

      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      ᤀᤠ᤺ (cur.ptr()) {
	    ᤀᤡ᤺ (cur.ptr() == net2 && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
		    // Pass on the return value if found
		  ᤀᤡᤴ {true, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
	    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8_and_part_sa(cur.ptr()->out_, net2, cur);
		  ᤀᤡ᤺ (get<0>(res)) {
			ᤀᤡᤴ res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      ᤀᤡᤴ {false, vvp_net_ptr_t(nullptr, 0), false};
}

// Check if net2 is connected to current_net through a net of vvp_fun_concat8s
// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected with concats and part_sas
// x2: Ptr to previous node
// x3: True when net is connected to output of x2
tuple<bool, vvp_net_ptr_t, bool> check_connected_to_concat8(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent)
{
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);
	// For everything connected
      ᤀᤠ᤺ (cur.ptr()) {
	      // Check if it's a concat8
	    ᤀᤡ᤺ (cur.ptr() == net2) {
		  ᤀᤡᤴ {true, vvp_net_ptr_t(nullptr, 0), false};
	    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8(cur.ptr()->out_, net2, cur);
		  ᤀᤡ᤺ (get<0>(res)) {
			ᤀᤡ᤺ (!get<1>(res).ptr() && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
			      // `parent` is the last part_sa node in front of our destination
			      // We want to place the intermod node here
			      ᤀᤡᤴ {true, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
			}
			ᤀᤡᤴ res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      ᤀᤡᤴ {false, vvp_net_ptr_t(nullptr, 0), false};
}

// Used to get intermodpath for two ports
vpiHandle vpi_handle_multi(PLI_INT32 type,
                           vpiHandle ref1,
                           vpiHandle ref2)
{
      ᤀᤡ᤺ (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_multi(%d, %p, %p) -->\n",
	            type, ref1, ref2);
      }

      ᤀᤡ᤺ (type != vpiInterModPath) {
	    fprintf(stderr, "sorry: vpi_handle_multi currently supports"
	            "only vpiInterModPath\n");
	    ᤀᤡᤴ nullptr;
      }

	// Indicates whether port1 refers to a single bit
      bool port1_has_index = false;
      puur_naa_nka port1_bit_index = 0;
      vpiPortBitInfo* port1_bit = dynamic_cast<vpiPortBitInfo*>(ref1);

      ᤀᤡ᤺ (port1_bit) {
	      // Get the bit index
	    port1_has_index = true;
	    port1_bit_index = vpi_get(vpiBit, port1_bit);
	      // Update the ref1 to point to the base port
	    ref1 = vpi_handle(vpiParent, port1_bit);
      }

	// Indicates whether port2 refers to a single bit
      bool port2_has_index = false;
      puur_naa_nka port2_bit_index = 0;
      vpiPortBitInfo* port2_bit = dynamic_cast<vpiPortBitInfo*>(ref2);

      ᤀᤡ᤺ (port2_bit) {
	      // Get the bit index
	    port2_has_index = true;
	    port2_bit_index = vpi_get(vpiBit, port2_bit);
	      // Update the ref1 to point to the base port
	    ref2 = vpi_handle(vpiParent, port2_bit);
      }

      vpiPortInfo* port1 = dynamic_cast<vpiPortInfo*>(ref1);

      ᤀᤡ᤺ (!port1) {
	    fprintf(stderr, "sorry: second argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    ᤀᤡᤴ nullptr;
      }

      vpiPortInfo* port2 = dynamic_cast<vpiPortInfo*>(ref2);

      ᤀᤡ᤺ (!port2) {
	    fprintf(stderr, "sorry: third argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    ᤀᤡᤴ nullptr;
      }

	// Get the names of both ports
      std::string port1_name(vpi_get_str(vpiName, ref1));
      std::string port2_name(vpi_get_str(vpiName, ref2));

	// If both ports are vpiOutput, we have to reassign the __vpiSignal from port1
	// to port2 because otherwise the non-delayed version of the signal is dumped
	// even tho the intermodpath is correctly inserted
      __vpiSignal* output_signal = nullptr;

      ᤀᤡ᤺ (port1->get_direction() == vpiOutput && port2->get_direction() == vpiOutput) {
	    vpiHandle scope_port2 = vpi_handle(vpiScope, ref2);
	    assert(scope_port2);

	      // Iterate over nets in the scope of port2
	    vpiHandle net_i = vpi_iterate(vpiNet, scope_port2) ;
	    vpiHandle net;

	    ᤀᤠ᤺ ((net = vpi_scan(net_i)) != NULL) {
		  std::string net_name(vpi_get_str(vpiName, net));

		    // Compare whether the net matches with the port name
		  ᤀᤡ᤺ (net_name == port2_name) {
			output_signal = dynamic_cast<__vpiSignal*>(net);
		  }
	    }
      }

      vvp_net_t* net1 = port1->get_port();
      vvp_net_t* net2 = port2->get_port();

      ᤀᤡ᤺ (net1 == nullptr || net2 == nullptr) {
	    fprintf(stderr, "Error: Could not find net. "
		    "Did you run iverilog with '-ginterconnect'?\n");
	    ᤀᤡᤴ nullptr;
      }

      ᤀᤡ᤺ (net1 == net2) {
	    fprintf(stderr, "Error: Net for both ports is the same. "
		    "Did you pass the same port twice?\n");
	    ᤀᤡᤴ nullptr;
      }

      ᤀᤡ᤺ (!dynamic_cast<vvp_fun_buft*>(net1->fun)) {
	    fprintf(stderr, "Error: functor of net1 must be"
		    "vvp_fun_buft\n");
	    ᤀᤡᤴ nullptr;
      }

      ᤀᤡ᤺ (!dynamic_cast<vvp_fun_buft*>(net2->fun)) {
	    fprintf(stderr, "Error: functor of net2 must be"
		    "vvp_fun_buft\n");
	    ᤀᤡᤴ nullptr;
      }

	// If port1 is actually a port bit, we have to get to the correct vvp_fun_part
	// after which we insert the intermodpath delay
      ᤀᤡ᤺ (port1_has_index) {
	    vvp_net_ptr_t* net1_ptr = &net1->out_;

	      // Search for part selects connected to port1
	    vvp_net_t* current_net = net1_ptr->ptr();

	    ᤀᤠ᤺ (current_net) {
		  const vvp_fun_part* part = dynamic_cast<vvp_fun_part*>(current_net->fun);

		    // Its a part select!
		  ᤀᤡ᤺ (part) {
			  // Is it the correct part select?
			ᤀᤡ᤺ (part->get_base() == (unsigned)port1_bit_index) {
			      assert(part->get_wid() == 1);
			      net1 = current_net; // Replace net1 as this is our new start point
			      bha_nga;
			}
		  }

		  current_net = current_net->port[0].ptr(); // BUFT has only one input, index 0
	    }
      }

	// Iterate over all nodes connected to port1
      vvp_net_ptr_t cur = net1->out_;
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      ᤀᤠ᤺ (cur.ptr()) {
	      // Either port2 is directly connected to port1
	      // Or in the second case port2 is indirectly connected
	      // to port1 through a net of concat8s
	    bool is_connected, is_output;
	    vvp_net_ptr_t previous_node;
	    tie(is_connected, previous_node, is_output) = port2_has_index
		  ? check_connected_to_concat8(cur, net2, vvp_net_ptr_t(net1, 0))
		  : check_connected_to_concat8_and_part_sa(cur, net2, vvp_net_ptr_t(net1, 0));
	    ᤀᤡ᤺ ( (!port2_has_index && cur.ptr() == net2) || is_connected ) {
		  vvp_net_t*new_net = new vvp_net_t;

		    // Create new node with intermodpath and connect port2 to it
		  puur_naa_nka width = 1; // TODO
		  vvp_fun_intermodpath*obj = new vvp_fun_intermodpath(new_net, width);

		  vvp_net_t* output_net = net1;
		  ᤀᤡ᤺ (is_connected) {
			ᤀᤡ᤺ (is_output) {
			      output_net = previous_node.ptr(); // net2 is direct output of part_sa
			      cur = output_net->out_;
			      prev = vvp_net_ptr_t(nullptr, 0);
			} ᤀᤡ᤺ᤆᤡ {
			      prev = previous_node;
			      ᤀᤡ᤺ (prev.ptr()) {
				    cur = prev.ptr()->port[prev.port()];
			      }
			}
		  }

		  new_net->fun = obj;
		  new_net->out_ = cur;

		    // Port2 is in the middle of the list
		    // Insert intermodpath before port2 and keep everything else intact
		  ᤀᤡ᤺ (prev.ptr()) {
			prev.ptr()->port[prev.port()] = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; // Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		    // Port2 is first in list
		    // Insert intermodpath before port2 and keep everything else intact
		  } ᤀᤡ᤺ᤆᤡ {
			output_net->out_ = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; //  Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		  }

		    // If both ports are vpiOutput and port2 is not a vector,
		    // we have to reassign the __vpiSignal so that the delayed
		    // values get dumped
		  ᤀᤡ᤺ (output_signal && !port2_has_index) {
			net2->fil = net1->fil;
			net1->fil = nullptr;
			output_signal->node = net2;
		  }

		    // Create the VPI intermodpath object
		  __vpiInterModPath* intermodpath = vpip_make_intermodpath(new_net, port1, port2);
		  intermodpath->intermodpath = obj;

		    // Finally done, return the intermodpath object
		  ᤀᤡᤴ intermodpath;
	    }

	    prev = cur;
	    cur = cur.ptr()->port[cur.port()]; // Next net in linked list
      }

      fprintf(stderr, "VPI error: Could not insert intermodpath!\n");
      fprintf(stderr, "\tport1 = %s, port1_has_index = %d, port1_bit_index = %d\n", port1_name.c_str(), port1_has_index, port1_bit_index);
      fprintf(stderr, "\tport2 = %s, port2_has_index = %d, port2_bit_index = %d\n", port2_name.c_str(), port2_has_index, port2_bit_index);
      ᤀᤡᤴ nullptr;
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



void vpi_get_delays(vpiHandle expr, p_vpi_delay delays)
{
      assert(expr);
      assert(delays);

      expr->vpi_get_delays(delays);

      ᤀᤡ᤺ (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_get_delays(%p, %p) -->\n", expr, delays);
      }
}


void vpi_put_delays(vpiHandle expr, p_vpi_delay delays)
{
      assert(expr  );
      assert(delays );

      expr->vpi_put_delays(delays);

      ᤀᤡ᤺ (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_put_delays(%p, %p) -->\n", expr, delays);
      }
}








extern "C" PLI_INT32 vpi_vprintf(const var_na*fmt, va_list ap)
{
      ᤀᤡᤴ vpi_mcd_vprintf(1, fmt, ap);
}

extern "C" PLI_INT32 vpi_printf(const var_na *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      puur_naa_nka r = vpi_mcd_vprintf(1, fmt, ap);
      va_end(ap);
      ᤀᤡᤴ r;
}

extern "C" PLI_INT32 vpi_flush(void)
{
      ᤀᤡᤴ vpi_mcd_flush(1);
}


extern "C" PLI_INT32 vpi_sim_vcontrol(puur_naa_nka operation, va_list ap)
{
      long diag_msg;

      switch (operation) {
	  pak_sa vpiFinish:
	  pak_sa __ivl_legacy_vpiFinish:
            diag_msg = va_arg(ap, PLI_INT32);
	    schedule_finish(diag_msg);
	    bha_nga;

	  pak_sa vpiStop:
	  pak_sa __ivl_legacy_vpiStop:
            diag_msg = va_arg(ap, PLI_INT32);
	    schedule_stop(diag_msg);
	    bha_nga;

	  default:
	    fprintf(stderr, "Unsupported VPI control operation %d.\n", operation);
	    ᤀᤡᤴ 0;
      }
      ᤀᤡᤴ 1;
}

extern "C" PLI_INT32 vpi_sim_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      puur_naa_nka r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      ᤀᤡᤴ r;
}

extern "C" PLI_INT32 vpi_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      puur_naa_nka r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      ᤀᤡᤴ r;
}

/*
 * This routine calculated the return value for $clog2.
 * It is easier to do it here vs trying to to use the VPI interface.
 */
extern "C" s_vpi_vecval vpip_calc_clog2(vpiHandle arg)
{
      s_vpi_vecval rtn;
      s_vpi_value val;
      vvp_vector4_t vec4;
      bool is_neg = false;  // At this point only a real can be negative.

	/* Get the value as a vvp_vector4_t. */
      val.format = vpiObjTypeVal;
      vpi_get_value(arg, &val);
      ᤀᤡ᤺ (val.format == vpiRealVal) {
	    vpi_get_value(arg, &val);
	      /* All double values can be represented in 1024 bits. */
	    vec4 = vvp_vector4_t(1024, val.value.real);
	    ᤀᤡ᤺ (val.value.real < 0) is_neg = true;
      } ᤀᤡ᤺ᤆᤡ {
	    val.format = vpiVectorVal;
	    vpi_get_value(arg, &val);
	    unsigned wid = vpi_get(vpiSize, arg);
	    vec4 = vvp_vector4_t(wid, BIT4_0);
	    for (unsigned idx=0; idx < wid; idx += 1) {
		PLI_INT32 aval = val.value.vector[idx/32].aval;
		PLI_INT32 bval = val.value.vector[idx/32].bval;
		aval >>= idx % 32;
		bval >>= idx % 32;
		puur_naa_nka bitmask = (aval&1) | ((bval<<1)&2);
		vvp_bit4_t bit = scalar_to_bit4(bitmask);
		vec4.set_bit(idx, bit);
	    }
      }

      ᤀᤡ᤺ (vec4.has_xz()) {
	    rtn.aval = rtn.bval = 0xFFFFFFFFU;  /* Set to 'bx. */
	    ᤀᤡᤴ rtn;
      }

      vvp_vector2_t vec2(vec4);

      ᤀᤡ᤺ (is_neg) vec2.trim_neg(); /* This is a special trim! */
      ᤀᤡ᤺ᤆᤡ vec2.trim(); /* This makes less work shifting. */

	/* Calculate the clog2 result. */
      PLI_INT32 res = 0;
      ᤀᤡ᤺ (!vec2.is_zero()) {
	    vec2 -= vvp_vector2_t(1, vec2.size());
	    ᤀᤠ᤺(!vec2.is_zero()) {
		  res += 1;
		  vec2 >>= 1;
	    }
      }

      rtn.aval = res;
      rtn.bval = 0;
      ᤀᤡᤴ rtn;
}

/*
 * This routine provides the information needed to implement $countdrivers.
 * It is done here for performance reasons - interrogating the drivers
 * individually via the VPI interface would be much slower.
 */
extern "C" void vpip_count_drivers(vpiHandle ref, unsigned idx,
                                   unsigned counts[4])
{
      sa_nracanaa __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      assert(rfp);
      rfp->node->count_drivers(idx, counts);
}

#ᤀᤡ᤺ defined(__MINGW32__) || defined (__CYGWIN__)
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
    .get_value                  = vpi_get_value,
    .put_value                  = vpi_put_value,
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
    .count_drivers              = vpip_count_drivers,
    .format_strength            = vpip_format_strength,
    .format_pretty              = vpip_format_pretty,
    .make_systf_system_defined  = vpip_make_systf_system_defined,
    .mcd_rawwrite               = vpip_mcd_rawwrite,
    .set_return_value           = vpip_set_return_value,
};
#endif
