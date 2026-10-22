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

using نطاق std;
vpi_mode_t vpi_mode_flag = VPI_MODE_NONE;
FILE*vpi_trace = 0;

ساكن s_vpi_vlog_info  vpi_vlog_info;
ساكن s_vpi_error_info vpip_last_error = { 0, 0, 0, 0, 0, 0, 0 };

ثابت حرف* direction_as_string(صحيح dir)
{
      بدل (dir) {
	  حالة vpi_شغرو:
	    أرجع "input";
	  حالة vpiOutput:
	    أرجع "output";
	  حالة vpiInout:
	    أرجع "inout";
	  حالة vpiMixedIO:
	    أرجع "mixed io";
	  حالة vpiNoDirection:
	    أرجع "no direction";
	  افتراضي:
	    أرجع "INVALID-DIRECTION";
      }
}

__vpiHandle::~__vpiHandle()
{ }

صحيح __vpiHandle::vpi_get(صحيح)
{ أرجع vpiUndefined; }

int64_t __vpiHandle::vpi_get64(صحيح code)
{ أرجع (code == _vpiNexusId) ? 0 : vpiUndefined; }

حرف* __vpiHandle::vpi_get_str(صحيح)
{ أرجع 0; }

فراغ __vpiHandle::vpi_get_درج(p_vpi_درج)
{ }

vpiHandle __vpiHandle::vpi_put_درج(p_vpi_درج, صحيح)
{ أرجع 0; }

vpiHandle __vpiHandle::vpi_handle(صحيح)
{ أرجع 0; }

vpiHandle __vpiHandle::vpi_iterate(صحيح)
{ أرجع 0; }

vpiHandle __vpiHandle::vpi_index(صحيح)
{ أرجع 0; }

فراغ __vpiHandle::vpi_get_delays(p_vpi_delay)
{ }

فراغ __vpiHandle::vpi_put_delays(p_vpi_delay)
{ }

__vpiBaseVar::__vpiBaseVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net)
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
ساكن صحيح suppress_free(vpiHandle)
{ أرجع 1; }
__vpiHandle::free_object_fun_t __vpiHandle::free_object_fun(فراغ)
{ أرجع &suppress_free; }

/*
 * The vpip_string function creates a constant string from the pass
 * input. This constant string is permanently allocated from an
 * efficient string buffer store.
 */
بنية vpip_string_chunk {
      بنية vpip_string_chunk*next;
      حرف data[64*1024 - حجم (بنية vpip_string_chunk*)];
};

unsigned vpip_size(ثابت __vpiSignal *sig)
{
      أرجع abs(sig->msb.get_درج() - sig->lsb.get_درج()) + 1;
}

__vpiScope* vpip_scope(__vpiSignal*sig)
{
      إذا (sig->is_netarray)
	    أرجع static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      وإلا
	    أرجع sig->within.scope;
}

__vpiScope* vpip_scope(__vpiRealVar*sig)
{
      إذا (sig->is_netarray)
	    أرجع static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      وإلا
	    أرجع sig->within.scope;
}

vpiHandle vpip_module(__vpiScope*scope)
{
      طالما(scope && scope->get_type_code() != vpiModule) {
	    scope = scope->scope;
      }
      أكد(scope);
      أرجع scope;
}

ثابت حرف *vpip_string(ثابت حرف*str)
{
      ساكن vpip_string_chunk first_chunk = {0, {0}};
      ساكن vpip_string_chunk*chunk_list = &first_chunk;
      ساكن unsigned chunk_fill = 0;

      unsigned len = strlen(str);
      أكد( (len+1) <= حجم chunk_list->data );

      إذا ( (len+1) > (حجم chunk_list->data - chunk_fill) ) {
	    vpip_string_chunk*tmp = جديد vpip_string_chunk;
	    tmp->next = chunk_list;
	    chunk_list = tmp;
	    chunk_fill = 0;
      }

      حرف*res = chunk_list->data + chunk_fill;
      chunk_fill += len + 1;

      strcpy(res, str);
      أرجع res;
}

ساكن unsigned hash_string(ثابت حرف*text)
{
      unsigned h = 0;

      طالما (*text) {
	    h = (h << 4) ^ (h >> 28) ^ (unsigned)*text;
	    text += 1;
      }
      أرجع h;
}

ثابت حرف* vpip_name_string(ثابت حرف*text)
{
      ثابت unsigned HASH_SIZE = 4096;
      ساكن ثابت حرف*hash_table[HASH_SIZE] = {0};

      unsigned hash_درج = hash_string(text) % HASH_SIZE;

	/* If we easily find the string in the hash table, then return
	   that and be done. */
      إذا (hash_table[hash_درج]
	  && (strcmp(hash_table[hash_درج], text) == 0)) {
	    أرجع hash_table[hash_درج];
      }

	/* The existing hash entry is not a match. Replace it with the
	   newly allocated value, and return the new pointer as the
	   result to the add. */
      ثابت حرف*res = vpip_string(text);
      hash_table[hash_درج] = res;

      أرجع res;
}

PLI_INT32 vpi_chk_error(p_vpi_error_info info)
{
      إذا (vpip_last_error.state == 0)
	    أرجع 0;

      info->state = vpip_last_error.state;
      info->level = vpip_last_error.level;
      info->message = vpip_last_error.message;
      info->product = vpi_vlog_info.product;
      info->code = (حرف *)"";
      info->file = 0;
      info->لي = 0;

      أرجع info->level;
}

PLI_INT32 vpi_compare_objects(vpiHandle obj1, vpiHandle obj2)
{
      أكد(obj1);
      أكد(obj2);

	// Does this actually work for all cases?
      إذا (obj1 != obj2) أرجع 0;
      وإلا أرجع 1;
}

/*
 * Copy the internal information to the data structure. Do not free or
 * change the tfname/user_data since they are a pointer to the real
 * string/data values. We also support passing a task or function handle
 * instead of just a handle to a vpiUserSystf.
 */
فراغ vpi_get_systf_info(vpiHandle ref, p_vpi_systf_data data)
{
      بنية __vpiUserSystf* rfp = dynamic_cast<__vpiUserSystf*>(ref);
      إذا (rfp == 0) {
	    بنية __vpiSysTaskCall*call = dynamic_cast<__vpiSysTaskCall*>(ref);
	    أكد(call);
	    rfp = call->defn;
      }

	/* Assert that vpiUserDefn is true! */
      أكد(rfp->is_user_defn);

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
بنية __vpiSysTaskCall*vpip_cur_task = 0;

PLI_INT32 vpi_free_object(vpiHandle ref)
{
      صحيح rtn;

      إذا (vpi_trace) {
	    fprintf(vpi_trace, "vpi_free_object(%p)", ref);
	    fflush(vpi_trace);
      }

      أكد(ref);
      __vpiHandle::free_object_fun_t fun = ref->free_object_fun();
      rtn = fun (ref);

      إذا (vpi_trace)
	    fprintf(vpi_trace, " --> %d\n", rtn);

      أرجع rtn;
}

PLI_INT32 vpi_release_handle(vpiHandle ref)
{
	// Since SystemVerilog vpi_free_object() has been
	// renamed vpi_release_handle(), and thus
	// vpi_free_object() has been deprecated.
      أرجع vpi_free_object(ref);
}

ساكن صحيح vpip_get_global(صحيح property)
{
      بدل (property) {

	  حالة vpiTimeUnit:
	  حالة vpiTimePrecision:
	    أرجع vpip_get_time_precision();

	  افتراضي:
	    fprintf(stderr, "vpi error: bad global property: %d\n", property);
	    أكد(0);
	    أرجع vpiUndefined;
      }
}

ساكن ثابت حرف* vpi_property_str(PLI_INT32 code)
{
      ساكن حرف buf[32];
      بدل (code) {
	  حالة vpiConstType:
	    أرجع "vpiConstType";
	  حالة vpiName:
	    أرجع "vpiName";
	  حالة vpiFullName:
	    أرجع "vpiFullName";
	  حالة vpiTimeUnit:
	    أرجع "vpiTimeUnit";
	  حالة vpiTimePrecision:
	    أرجع "vpiTimePrecision";
          حالة vpiSize:
	    أرجع "vpiSize";
          حالة _vpiNexusId:
	    أرجع "_vpiNexusId";
	  افتراضي:
	    snprintf(buf, حجم(buf), "%d", (صحيح)code);
      }
      أرجع buf;
}

ثابت حرف* vpi_type_as_string(PLI_INT32 code)
{
      ساكن حرف buf[32];
      بدل (code) {
	  حالة vpiArrayType:
	    أرجع "vpiArrayType";
	  حالة vpiBitVar:
	    أرجع "vpiBitVar";
	  حالة vpiByteVar:
	    أرجع "vpiByteVar";
	  حالة vpiClassVar:
	    أرجع "vpiClassVar";
	  حالة vpiConstant:
	    أرجع "vpiConstant";
	  حالة vpiEnumTypespec:
	    أرجع "vpiEnumTypespec";
	  حالة vpiFunction:
	    أرجع "vpiFunction";
	  حالة vpiGenScope:
	    أرجع "vpiGenScope";
	  حالة vpiIntVar:
	    أرجع "vpiIntVar";
	  حالة vpiIntegerVar:
	    أرجع "vpiIntegerVar";
	  حالة vpiIterator:
	    أرجع "vpiIterator";
	  حالة vpiLongIntVar:
	    أرجع "vpiLongIntVar";
	  حالة vpiMemory:
	    أرجع "vpiMemory";
	  حالة vpiMemoryWord:
	    أرجع "vpiMemoryWord";
	  حالة vpiModule:
	    أرجع "vpiModule";
	  حالة vpiNamedBegin:
	    أرجع "vpiNamedBegin";
	  حالة vpiNamedEvent:
	    أرجع "vpiNamedEvent";
	  حالة vpiNamedFork:
	    أرجع "vpiNamedFork";
	  حالة vpiPackage:
	    أرجع "vpiPackage";
	  حالة vpiPathTerm:
	    أرجع "vpiPathTerm";
	  حالة vpiPort:
	    أرجع "vpiPort";
	  حالة vpiNet:
	    أرجع "vpiNet";
	  حالة vpiNetArray:
	    أرجع "vpiNetArray";
	  حالة vpiNetBit:
	    أرجع "vpiNetBit";
	  حالة vpiParameter:
	    أرجع "vpiParameter";
	  حالة vpiPartSelect:
	    أرجع "vpiPartSelect";
	  حالة vpiRealVar:
	    أرجع "vpiRealVar";
	  حالة vpiReg:
	    أرجع "vpiReg";
	  حالة vpiRegBit:
	    أرجع "vpiRegBit";
	  حالة vpiShortIntVar:
	    أرجع "vpiShortIntVar";
	  حالة vpiStringVar:
	    أرجع "vpiStringVar";
	  حالة vpiSysFuncCall:
	    أرجع "vpiSysFuncCall";
	  حالة vpiSysTaskCall:
	    أرجع "vpiSysTaskCall";
	  حالة vpiTask:
	    أرجع "vpiTask";
	  حالة vpiTimeVar:
	    أرجع "vpiTimeVar";
	  حالة vpiUserSystf:
	    أرجع "vpiUserSystf";
	  افتراضي:
	    snprintf(buf, حجم(buf), "%d", (صحيح)code);
      }
      أرجع buf;
}

PLI_INT32 vpi_get(صحيح property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      إذا (property == _vpiDelaySelection) أرجع vpip_delay_selection;

      إذا (ref == 0)
	    أرجع vpip_get_global(property);

      إذا (property == vpiType) {
	    إذا (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

	    إذا (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  أرجع vpiNetArray;
	    وإلا
		  أرجع ref->get_type_code();
      }

      صحيح res = ref->vpi_get(property);

      إذا (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get(%s, %p) --> %d\n",
		    vpi_property_str(property), ref, res);
      }

      أرجع res;
}

PLI_INT64 vpi_get64(صحيح property, vpiHandle ref)
{
	// For now we only support getting the nexus id.
      إذا ((ref == 0) || (property != _vpiNexusId)) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) is not currently supported.\n",
		    vpi_property_str(property), ref);
	    أرجع vpiUndefined;
      }

      int64_t res = ref->vpi_get64(property);

      إذا (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) --> %" PRId64 "\n",
		    vpi_property_str(property), ref, res);
      }

      أرجع res;
}

حرف* vpi_get_str(PLI_INT32 property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      إذا (property == _vpiDelaySelection) {
	    بدل (vpip_delay_selection) {
		حالة _vpiDelaySelMinimum:
		  أرجع simple_set_rbuf_str("MINIMUM");
		حالة _vpiDelaySelTypical:
		  أرجع simple_set_rbuf_str("TYPICAL");
		حالة _vpiDelaySelMaximum:
		  أرجع simple_set_rbuf_str("MAXIMUM");
		افتراضي:
		  أكد(0);
	    }
      }

      إذا (ref == 0) {
	    fprintf(stderr, "vpi error: vpi_get_str(%s, 0) called "
		    "with null vpiHandle.\n", vpi_property_str(property));
	    أرجع 0;
      }

      إذا (property == vpiType) {
	    إذا (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

            PLI_INT32 type;
	    إذا (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  type = vpiNetArray;
	    وإلا
		  type = ref->get_type_code();
	    أرجع (حرف *)vpi_type_as_string(type);
      }

      حرف*res = ref->vpi_get_str(property);

      إذا (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get_str(%s, %p) --> %s\n",
		    vpi_property_str(property), ref, res? res : "<NULL>");
      }

      أرجع res;
}

__vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj)
{
      ثابت بنية __vpiSysTaskCall*task;
      بنية __vpiSignal*إشارة;
      بنية __vpiRealVar*real;
      ثابت __vpiNamedEvent*event;

      بدل (obj->get_type_code()) {
	  حالة vpiSysTaskCall:
	    task = dynamic_cast<__vpiSysTaskCall*>(obj);
	    أرجع task->scope;

	  حالة vpiModule:
	    أرجع dynamic_cast<__vpiScope*>(obj);

	  حالة vpiNet:
	  حالة vpiReg:
	  حالة vpiIntegerVar:
	  حالة vpiBitVar:
	  حالة vpiByteVar:
	  حالة vpiShortIntVar:
	  حالة vpiIntVar:
	  حالة vpiLongIntVar:
	    إشارة = dynamic_cast<__vpiSignal*>(obj);
	    أرجع vpip_scope(إشارة);

	  حالة vpiRealVar:
	    real = dynamic_cast<__vpiRealVar*>(obj);
	    أرجع vpip_scope(real);

	  حالة vpiNamedEvent:
	    event = dynamic_cast<__vpiNamedEvent*>(obj);
	    أرجع event->get_scope();

	  حالة vpiMemory:
	  حالة vpiMemoryWord:
	  حالة vpiPartSelect:
	    أرجع dynamic_cast<__vpiScope*>(obj->vpi_handle(vpiScope));

	  افتراضي:
	    fprintf(stderr, "ERROR: vpip_scope_from_handle called with "
		    "object handle type=%d\n", obj->get_type_code());
	    أكد(0);
	    أرجع 0;
      }
}

صحيح vpip_time_units_from_handle(vpiHandle obj)
{
      إذا (obj == 0)
	    أرجع vpip_get_time_precision();

      ثابت __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      إذا (scope == 0)
	    أرجع vpip_get_time_precision();

      أرجع scope->time_units;
}

صحيح vpip_time_precision_from_handle(vpiHandle obj)
{
      إذا (obj == 0)
	    أرجع vpip_get_time_precision();

      ثابت __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      إذا (scope == 0)
	    أرجع vpip_get_time_precision();

      أرجع scope->time_precision;
}

مزدوج vpip_scaled_time_from_handle(vvp_time64_t time, vpiHandle obj)
{
      صحيح scale = vpip_get_time_precision() -
	          vpip_time_units_from_handle(obj);
      إذا (scale >= 0)
	    أرجع (مزدوج)time * pow(10.0, scale);
      وإلا
	    أرجع (مزدوج)time / pow(10.0, -scale);
}

فراغ vpi_get_time(vpiHandle obj, s_vpi_time*vp)
{
      vvp_time64_t time;

      أكد(vp);

      time = schedule_simtime();

      بدل (vp->type) {
          حالة vpiSimTime:
	    vp->سم = (time >> 32) & 0xffffffff;
	    vp->كس = time & 0xffffffff;
	    اكسر;

          حالة vpiScaledRealTime:
	    vp->real = vpip_scaled_time_from_handle(time, obj);
	    اكسر;

          افتراضي:
            fprintf(stderr, "vpi_get_time: unknown type: %d\n", (صحيح)vp->type);
            أكد(0);
	    اكسر;
      }
}

PLI_INT32 vpi_get_vlog_info(p_vpi_vlog_info vlog_info_p)
{
    إذا (vlog_info_p != 0) {
	  *vlog_info_p = vpi_vlog_info;
	  أرجع 1;
    } وإلا {
	  أرجع 0;
    }
}

فراغ vpi_set_vlog_info(صحيح argc, حرف** argv)
{
    ساكن حرف icarus_product[] = "Icarus Verilog";
    ساكن حرف icarus_version[] = VERSION;
    vpi_vlog_info.product = icarus_product;
    vpi_vlog_info.version = icarus_version;
    vpi_vlog_info.argc    = argc;
    vpi_vlog_info.argv    = argv;

    ساكن حرف trace_buf[1024];
    إذا (ثابت حرف*path = getenv("VPI_TRACE"))  {
	  إذا (!strcmp(path,"-"))
		vpi_trace = stdout;
	  وإلا {
		vpi_trace = fopen(path, "w");
		إذا (!vpi_trace) {
		      perror(path);
		      exit(1);
		}
		setvbuf(vpi_trace, trace_buf, _IOLBF, حجم(trace_buf));
	  }
    }
}

ساكن فراغ vec4_get_درج_string(ثابت vvp_vector4_t&word_val, unsigned width,
				  s_vpi_درج*vp)
{
      unsigned nchar = width / 8;
      unsigned tail = width % 8;

      حرف*rbuf = static_cast<حرف *>(need_result_buf(nchar + 1, RBUF_VAL));
      حرف*cp = rbuf;

      إذا (tail > 0) {
	    حرف char_val = 0;
	    لكل (unsigned idx = width-tail; idx < width ;  idx += 1) {
		  vvp_bit4_t val = word_val.value(idx);
		  إذا (val == BIT4_1)
			char_val |= 1 << idx;
	    }

	    إذا (char_val != 0)
		  *cp++ = char_val;
	    وإلا إذا (cp != rbuf)
		  *cp++ = ' ';
      }

      لكل (unsigned idx = 0 ;  idx < nchar ;  idx += 1) {
	    unsigned bit = (nchar - idx - 1) * 8;
	    حرف char_val = 0;
	    لكل (unsigned bdx = 0 ;  bdx < 8 ;  bdx += 1) {
		  vvp_bit4_t val = word_val.value(bit+bdx);
		  إذا (val == BIT4_1)
			char_val |= 1 << bdx;
	    }
	    // Ignore leading null-bytes and replace other null-bytes with space.
	    // The LRM is not entirely clear on how null bytes should be handled.
	    // This is the implementation chosen for iverilog.
	    إذا (char_val != 0)
		  *cp++ = char_val;
	    وإلا إذا (cp != rbuf)
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
فراغ vpip_vec4_get_درج(ثابت vvp_vector4_t&word_val, unsigned width,
			 منطقي signed_flag, s_vpi_درج*vp)
{
      حرف *rbuf = 0;

      بدل (vp->format) {
	  افتراضي:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector values.\n", (صحيح)vp->format);
	    أكد(0);

	  حالة vpiSuppressVal:
	    اكسر;

	  حالة vpiBinStrVal:
	    rbuf = static_cast<حرف *>(need_result_buf(width+1, RBUF_VAL));
	    لكل (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		  vvp_bit4_t bit = word_val.value(idx);
		  rbuf[width-idx-1] = vvp_bit4_to_ascii(bit);
	    }
	    rbuf[width] = 0;
	    vp->value.str = rbuf;
	    اكسر;

	  حالة vpiOctStrVal: {
		unsigned hwid = ((width+2) / 3) + 1;
		rbuf = static_cast<حرف *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_oct_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		اكسر;
	  }

	  حالة vpiDecStrVal: {
// HERE need a better estimate.
		rbuf = static_cast<حرف *>(need_result_buf(width+1, RBUF_VAL));
		vpip_vec4_to_dec_str(word_val, rbuf, width+1, signed_flag);
		vp->value.str = rbuf;
		اكسر;
	  }

	  حالة vpiHexStrVal: {
		unsigned  hwid = ((width + 3) / 4) + 1;
		rbuf = static_cast<حرف *>(need_result_buf(hwid, RBUF_VAL));
		vpip_vec4_to_hex_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		اكسر;
	  }

          حالة vpiScalarVal: {
	        // scalars should be of size 1
	        أكد(width == 1);
	        بدل(word_val.value(0)) {
		    حالة BIT4_0:
		      vp->value.scalar = vpi0;
		      اكسر;
	            حالة BIT4_1:
                      vp->value.scalar = vpi1;
                      اكسر;
	            حالة BIT4_X:
                      vp->value.scalar = vpiX;
                      اكسر;
	            حالة BIT4_Z:
                      vp->value.scalar = vpiZ;
                      اكسر;
		}
                اكسر;
          }

	  حالة vpiIntVal: {
		long val = 0;
		vvp_bit4_t pad = BIT4_0;
		إذا (signed_flag && word_val.size() > 0)
		      pad = word_val.value(word_val.size()-1);

		لكل (unsigned idx = 0 ; idx < 8*حجم(val) ;  idx += 1) {
		      vvp_bit4_t val4 = pad;
		      إذا (idx < word_val.size())
			    val4 = word_val.value(idx);
		      إذا (val4 == BIT4_1)
			    val |= 1L << idx;
		}

		vp->value.integer = val;
		اكسر;
	  }

	  حالة vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiVectorVal;
	    // fallthrough
	  حالة vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * حجم(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		لكل (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      بدل (word_val.value(idx)) {
			  حالة BIT4_0:
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    اكسر;
			  حالة BIT4_1:
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    اكسر;
			  حالة BIT4_X:
			    op->aval |= (1 << idx % 32);
			    op->bval |= (1 << idx % 32);
			    اكسر;
			  حالة BIT4_Z:
			    op->aval &= ~(1 << idx % 32);
			    op->bval |=  (1 << idx % 32);
			    اكسر;
		      }
		      إذا (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		اكسر;
	  }

	  حالة vpiStringVal:
		vec4_get_درج_string(word_val, width, vp);
		اكسر;

	  حالة vpiRealVal:
		vector4_to_درج(word_val, vp->value.real, signed_flag);
		اكسر;
      }
}

فراغ vpip_vec2_get_درج(ثابت vvp_vector2_t&word_val, unsigned width,
			 منطقي signed_flag, s_vpi_درج*vp)
{
      بدل (vp->format) {
	  افتراضي:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector2 values.\n", (صحيح)vp->format);
	    أكد(0);

	  حالة vpiSuppressVal:
	    اكسر;

	  حالة vpiIntVal:
	    vector2_to_درج(word_val, vp->value.integer, signed_flag);
	    اكسر;

	  حالة vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  حالة vpiVectorVal: {
		unsigned hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_result_buf(hwid * حجم(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		لكل (unsigned idx = 0 ;  idx < width ;  idx += 1) {
		      إذا (word_val.value(idx)) {
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      } وإلا {
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      }
		      إذا (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		اكسر;
	  }
      }

}

/*
 * Convert a real value to the appropriate integer.
 */
ساكن PLI_INT32 get_real_as_int(مزدوج real)
{
      مزدوج rtn;

	/* We would normally want to return 'bx for a NaN or
	 * +/- infinity, but for an integer the standard says
	 * to convert 'bx to 0, so we just return 0. */
      إذا (real != real || (real && (real == 0.5*real))) {
	    أرجع 0;
      }

      rtn = std::round(real);

      أرجع (PLI_INT32) rtn;
}

/*
 * This is a generic function to convert a double value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer.
 */
فراغ vpip_real_get_درج(مزدوج real, s_vpi_درج*vp)
{
      حرف *rbuf = 0;

      بدل (vp->format) {
	  افتراضي:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting real values.\n", (صحيح)vp->format);
	    أكد(0);

	  حالة vpiSuppressVal:
	    اكسر;

	  حالة vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiRealVal;
	    // fallthrough
	  حالة vpiRealVal:
	    vp->value.real = real;
	    اكسر;

	  حالة vpiIntVal:
	    vp->value.integer = get_real_as_int(real);
	    اكسر;

	  حالة vpiDecStrVal:
	    rbuf = static_cast<حرف *>(need_result_buf(1025, RBUF_VAL));
	    vpip_vec4_to_dec_str(vvp_vector4_t(1024, real), rbuf, 1025, true);
	    vp->value.str = rbuf;
	    اكسر;
      }
}

مزدوج real_from_vpi_درج(s_vpi_درج*vp)
{
      vvp_vector4_t vec4(1024);
      مزدوج result;
      منطقي is_signed = خطأ;

      بدل (vp->format) {
	  افتراضي:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "putting real values.\n", (صحيح)vp->format);
	    أكد(0);

	  حالة vpiRealVal:
	    result = vp->value.real;
	    اكسر;

	  حالة vpiIntVal:
	    result = (مزدوج) vp->value.integer;
	    اكسر;

	  حالة vpiBinStrVal:
	    vpip_bin_str_to_vec4(vec4, vp->value.str);
	    إذا (vp->value.str[0] == '-') is_signed = true;
	    vector4_to_درج(vec4, result, is_signed);
	    اكسر;

	  حالة vpiOctStrVal:
	    vpip_oct_str_to_vec4(vec4, vp->value.str);
	    إذا (vp->value.str[0] == '-') is_signed = true;
	    vector4_to_درج(vec4, result, is_signed);
	    اكسر;

	  حالة vpiDecStrVal:
	    vpip_dec_str_to_vec4(vec4, vp->value.str);
	    إذا (vp->value.str[0] == '-') is_signed = true;
	    vector4_to_درج(vec4, result, is_signed);
	    اكسر;

	  حالة vpiHexStrVal:
	    vpip_hex_str_to_vec4(vec4, vp->value.str);
	    إذا (vp->value.str[0] == '-') is_signed = true;
	    vector4_to_درج(vec4, result, is_signed);
	    اكسر;

      }

      أرجع result;
}

فراغ vpip_string_get_درج(ثابت string&val, s_vpi_درج*vp)
{
      حرف *rbuf = 0;

      بدل (vp->format) {
	  افتراضي:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting string values.\n", (صحيح)vp->format);
	    أكد(0);

	  حالة vpiSuppressVal:
	    اكسر;

	  حالة vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiStringVal;
	    // fallthrough
	  حالة vpiStringVal:
	    rbuf = static_cast<حرف *>(need_result_buf(val.size() + 1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    اكسر;
      }
}


فراغ vpi_get_درج(vpiHandle فرچ, s_vpi_درج*vp)
{
      أكد(فرچ);
      أكد(vp);

	// Never bother with suppressed values. All the derived
	// classes can ignore this type.
      إذا (vp->format == vpiSuppressVal)
	    أرجع;

      فرچ->vpi_get_درج(vp);

      إذا (vpi_trace) بدل (vp->format) {
		حالة vpiStringVal:
		  fprintf(vpi_trace,"vpi_get_value(%p=<%d>) -> string=\"%s\"\n",
			  فرچ, فرچ->get_type_code(), vp->value.str);
		  اكسر;

		حالة vpiBinStrVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> binstr=%s\n",
			  فرچ->get_type_code(), vp->value.str);
		  اكسر;

		حالة vpiIntVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> int=%d\n",
			  فرچ->get_type_code(), (صحيح)vp->value.integer);
		  اكسر;

		حالة vpiSuppressVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <suppress>\n",
			  فرچ->get_type_code());
		  اكسر;

		افتراضي:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <%d>=?\n",
			  فرچ->get_type_code(), (صحيح)vp->format);
	    }
}

بنية vpip_put_درج_event : vvp_gen_event_s {
      vpiHandle handle;
      s_vpi_درج value;
      صحيح flags;
      virtual فراغ run_run() override;
      ~vpip_put_درج_event() override { }
};

فراغ vpip_put_درج_event::run_run()
{
      handle->vpi_put_درج(&value, flags);
      بدل (value.format) {
	    /* Free the copied string. */
	  حالة vpiBinStrVal:
	  حالة vpiOctStrVal:
	  حالة vpiDecStrVal:
	  حالة vpiHexStrVal:
	  حالة vpiStringVal:
	    حرر(value.value.str);
	    اكسر;
	    /* Free the copied time structure. */
	  حالة vpiTimeVal:
	    حرر(value.value.time);
	    اكسر;
	    /* Free the copied vector structure. */
	  حالة vpiVectorVal:
	    حرر(value.value.vector);
	    اكسر;
	    /* Free the copied strength structure. */
	  حالة vpiStrengthVal:
	    حرر(value.value.strength);
	    اكسر;
	    /* Everything else is static in the structure. */
	  افتراضي:
	    اكسر;
      }
}

/* Make a copy of a pointer to a time structure. */
ساكن t_vpi_time *timedup(ثابت t_vpi_time *val)
{
      t_vpi_time *rtn;
      rtn = static_cast<t_vpi_time *> (malloc(حجم(t_vpi_time)));
      *rtn = *val;
      أرجع rtn;
}

/* Make a copy of a pointer to a vector value structure. */
ساكن t_vpi_vecval *vectordup(ثابت t_vpi_vecval *val, PLI_INT32 size)
{
      unsigned لمب_bytes;
      t_vpi_vecval *rtn;
      أكد(size > 0);
      لمب_bytes = ((size + 31)/32)*حجم(t_vpi_vecval);
      rtn = static_cast<t_vpi_vecval *> (malloc(لمب_bytes));
      memcpy(rtn, val, لمب_bytes);
      أرجع rtn;
}

/* Make a copy of a pointer to a strength structure. */
ساكن t_vpi_strengthval *strengthdup(ثابت t_vpi_strengthval *val)
{
      t_vpi_strengthval *rtn;
      rtn = static_cast<t_vpi_strengthval *>
            (malloc(حجم(t_vpi_strengthval)));
      *rtn = *val;
      أرجع rtn;
}

vpiHandle vpi_put_درج(vpiHandle obj, s_vpi_درج*vp,
			s_vpi_time*when, PLI_INT32 flags)
{
      أكد(obj);

      flags &= ~vpiReturnEvent;

      إذا (flags!=vpiNoDelay && flags!=vpiForceFlag && flags!=vpiReleaseFlag) {
	    vvp_time64_t dly;
	    صحيح scale;

	    إذا (vpi_get(vpiAutomatic, obj)) {
		  fprintf(stderr, "VPI error: cannot put a value with "
				  "a delay on automatically allocated "
				  "variable '%s'.\n",
				  vpi_get_str(vpiName, obj));
		  أرجع 0;
	    }

	    أكد(when != 0);

	    بدل (when->type) {
		حالة vpiScaledRealTime:
		  scale = vpip_time_units_from_handle(obj) -
		          vpip_get_time_precision();
		  إذا (scale >= 0) {
			dly = (vvp_time64_t)(when->real * pow(10.0, scale));
		  } وإلا {
			dly = (vvp_time64_t)(when->real / pow(10.0, -scale));
		  }
		  اكسر;
		حالة vpiSimTime:
		  dly = vpip_timestruct_to_time(when);
		  اكسر;
		افتراضي:
		  dly = 0;
		  اكسر;
	    }

	    إذا ((dly == 0) && schedule_at_rosync()) {
		  fprintf(stderr, "VPI error: attempted to put a value to "
				  "variable '%s' during a read-only synch "
				  "callback.\n", vpi_get_str(vpiName, obj));
		  أرجع 0;
	    }

	    vpip_put_درج_event*put = جديد vpip_put_درج_event;
	    put->handle = obj;
	    إذا (dynamic_cast<__vpiNamedEvent*>(obj)) {
		  put->value.format = vpiIntVal;
		  put->value.value.integer = 0;
	    } وإلا {
		  أكد(vp);
		  put->value = *vp;
	    }
	      /* Since this is a scheduled put event we must copy any pointer
	       * data to keep it available until the event is actually run. */
	    بدل (put->value.format) {
		  /* Copy the string items. */
		حالة vpiBinStrVal:
		حالة vpiOctStrVal:
		حالة vpiDecStrVal:
		حالة vpiHexStrVal:
		حالة vpiStringVal:
		  put->value.value.str = strdup(put->value.value.str);
		  اكسر;
		  /* Copy a time pointer item. */
		حالة vpiTimeVal:
		  put->value.value.time = timedup(put->value.value.time);
		  اكسر;
		  /* Copy a vector pointer item. */
		حالة vpiVectorVal:
		  put->value.value.vector = vectordup(put->value.value.vector,
		                                      vpi_get(vpiSize, obj));
		  اكسر;
		  /* Copy a strength pointer item. */
		حالة vpiStrengthVal:
		  put->value.value.strength =
		      strengthdup(put->value.value.strength);
		  اكسر;
		  /* Everything thing else is already in the structure. */
		افتراضي:
		  اكسر;
	    }
	    put->flags = flags;
	    schedule_generic(put, dly, خطأ, true, true);
	    أرجع 0;
      }

      إذا (schedule_at_rosync()) {
            fprintf(stderr, "VPI error: attempted to put a value to "
			    "variable '%s' during a read-only synch "
			    "callback.\n", vpi_get_str(vpiName, obj));
            أرجع 0;
      }

      obj->vpi_put_درج(vp, flags);

      أرجع 0;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle res = 0;

      إذا (ref == 0) {
	      // A few types can apply to a nil handle. These are ways
	      // that the current function can get started finding things.
	    بدل (type) {

		حالة vpiScope:
		    // The IEEE1364-2005 doesn't seem to allow this,
		    // but some users seem to think it's handy, so
		    // return the scope that contains this SysTfCall.
		  أكد(vpip_cur_task);
		  res = vpip_cur_task->vpi_handle(vpiScope);
		  اكسر;

		حالة vpiSysTfCall:
		    // This is how VPI users get a first handle into
		    // the system. This is the handle of the system
		    // task/function call currently being executed.
		  إذا (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle(vpiSysTfCall, 0) "
				"-> %p (%s)\n", vpip_cur_task,
				vpip_cur_task->defn->info.tfname);
		  }
		  أرجع vpip_cur_task;

		افتراضي:
		  fprintf(stderr, "VPI error: vpi_handle(type=%d, ref=0).\n",
			  (صحيح)type);
		  res = 0;
		  اكسر;
	    }

      } وإلا {

	    إذا (type == vpiSysTfCall) {
		  fprintf(stderr, "VPI error: vpi_handle(vpiSysTfCall, "
			  "ref!=0).\n");
		  أرجع 0;
	    }

	    res = ref->vpi_handle(type);
      }


      إذا (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle(vpiScope, ref=%p) "
		    "-> %p\n", vpip_cur_task, ref);
      }

      أرجع res;
}

ساكن vpiHandle vpip_make_udp_iterator()
{
// HERE: Add support for iterating over UDP definitions.
//       See 26.6.16 (page 400 in 1364-2005).
      أرجع 0;
}

/*
 * This function asks the object to return an iterator for
 * the specified reference. It is up to the iterate_ method to
 * allocate a properly formed iterator.
 */
ساكن vpiHandle vpi_iterate_global(صحيح type)
{
      بدل (type) {
	  حالة vpiInstance:
	    // fallthrough
	  حالة vpiModule:
	    // fallthrough
	  حالة vpiProgram:
	    // fallthrough
	  حالة vpiInterface:
	    // fallthrough
	  حالة vpiPackage:
	    أرجع vpip_make_سيو_iterator(type);

	  حالة vpiUdpDefn:
	    أرجع vpip_make_udp_iterator();

	  حالة vpiUserSystf:
	    أرجع vpip_make_systf_iterator();
      }

      أرجع 0;
}

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle rtn = 0;

      أكد(vpi_mode_flag != VPI_MODE_NONE);
      إذا (vpi_mode_flag == VPI_MODE_REGISTER) {
	    fprintf(stderr, "vpi error: vpi_iterate called during "
		    "vpi_register_systf. You can't do that!\n");
	    أرجع 0;
      }

      إذا (ref == 0)
	    rtn = vpi_iterate_global(type);
      وإلا
	    rtn = ref->vpi_iterate(type);

      إذا (vpi_trace) {
	    fprintf(vpi_trace, "vpi_iterate(%d, %p) ->%s\n",
	    (صحيح)type, ref, rtn ? "" : " (null)");
      }

      أرجع rtn;
}

vpiHandle vpi_handle_by_index(vpiHandle ref, PLI_INT32 idx)
{
      أكد(ref);
      أرجع ref->vpi_index(idx);
}

ساكن vpiHandle find_name(ثابت حرف *name, vpiHandle handle)
{
      vpiHandle rtn = 0;
      __vpiScope*ref = dynamic_cast<__vpiScope*>(handle);

      /* check module names */
      إذا (!strcmp(name, vpi_get_str(vpiName, handle)))
	    rtn = handle;

      /* brute force search for the name in all objects in this scope */
      لكل (unsigned i = 0 ;  i < ref->intern.size() ;  i += 1) {
	      /* The standard says that since a port does not have a full
	       * name it cannot be found by name. Because of this we need
	       * to skip ports here so the correct handle can be located. */
	    إذا (vpi_get(vpiType, ref->intern[i]) == vpiPort) استمر;
	    ثابت حرف *nm = vpi_get_str(vpiName, ref->intern[i]);
	    إذا (nm && !strcmp(name, nm)) {
		  rtn = ref->intern[i];
		  اكسر;
	    } وإلا إذا (vpi_get(vpiType, ref->intern[i]) == vpiMemory ||
	               vpi_get(vpiType, ref->intern[i]) == vpiNetArray) {
		  /* We need to iterate on the words */
		  vpiHandle word_i, word_h;
		  word_i = vpi_iterate(vpiMemoryWord, ref->intern[i]);
		  طالما (word_i && (word_h = vpi_scan(word_i))) {
			nm = vpi_get_str(vpiName, word_h);
			إذا (nm && !strcmp(name, nm)) {
			      rtn = word_h;
			      vpi_free_object(word_i);
			      اكسر;
			}
		  }
	    }
	    /* found it yet? */
	    إذا (rtn) اكسر;
      }

      أرجع rtn;
}

// Find the end of the escaped identifier or simple identifier
ساكن حرف * find_rest(حرف *name)
{
      حرف *rest;
      إذا (*name == '\\') {
	    rest = strchr(name, ' ');
	    إذا (rest) {
		  *rest++ = 0; // The space is not part of the string
		    // There should be a '.' after the escaped ID if there is
		    // anything more. If it is missing add it to avoid a crash
		  إذا ((*rest != '.') && (*rest != 0)) {
			*--rest = '.';
			fprintf(stderr, "ERROR: Malformed scope string: \"%s\"", name);
		  }
		  إذا (*rest == 0) {
			rest = 0;
		  }
	    }
      } وإلا {
	    rest = strchr(name, '.');
      }
      أرجع rest;
}

ساكن vpiHandle find_scope(ثابت حرف *name, vpiHandle handle, صحيح depth)
{

      vpiHandle iter = handle==0
	    ? vpi_iterate(vpiModule, عدم)
	    : vpi_iterate(vpiInternalScope, handle);

      vector<حرف> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      حرف*nm_first = &name_buf[0];
      حرف*nm_rest = find_rest(nm_first);
      إذا (*nm_first == '\\') ++nm_first;
      إذا (nm_rest) {
	    *nm_rest++ = 0;
      }

      vpiHandle rtn = 0;
      vpiHandle hand;
      طالما (iter && (hand = vpi_scan(iter))) {
	    ثابت حرف *nm = vpi_get_str(vpiName, hand);

	    إذا (strcmp(nm_first,nm)==0) {
		  إذا (nm_rest)
			rtn=find_scope(nm_rest, hand, depth+1);
		  وإلا
			rtn = hand;
	    }

	    /* found it yet ? */
	    إذا (rtn) {
		  vpi_free_object(iter);
		  اكسر;
	    }
      }

      أرجع rtn;
}

// Find the end of the first escaped identifier or simple identifier
ساكن حرف * find_next(حرف *name)
{
      حرف *next;
      إذا (*name == '\\') {
	    next = strchr(name, ' ');
	    إذا (next && *++next == 0) next = 0;
      } وإلا {
	    next = strchr(name, '.');
      }
      أرجع next;
}

vpiHandle vpi_handle_by_name(ثابت حرف *name, vpiHandle scope)
{
      vpiHandle hand;

      إذا (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name(%s, %p) -->\n",
		    name, scope);
      }

	// Chop the name into path and base. For example, if the name
	// is "a.b.c", then nm_path becomes "a.b" and nm_base becomes
	// "c". If the name is "c" then nm_path is nil and nm_base is "c".
      vector<حرف> name_buf (strlen(name)+1);
      strcpy(&name_buf[0], name);
      حرف*nm_path = &name_buf[0];
      حرف*nm_base;

	// Check to see if we have an escaped identifier and if so search
	// the long way because a '.' could be in the escaped name.
      إذا (strchr(nm_path, '\\')) {
	    حرف *next;
	    nm_base = nm_path;
	    طالما ((next = find_next(nm_base))) {
		  nm_base = ++next;
	    }
	    إذا (nm_path == nm_base) {
		  nm_path = 0;
	    } وإلا {
		  *(nm_base-1) = 0;
	    }

	// If there is no escaped identifier then just look for the last '.'
      } وإلا {
	    nm_base = strrchr(nm_path, '.');
	    إذا (nm_base) {
		  *nm_base++ = 0;
	    } وإلا {
		  nm_base = nm_path;
		  nm_path = 0;
	    }
      }

      /* If scope provided, look in corresponding module; otherwise
       * traverse the hierarchy specified in name to find the leaf module
       * and try finding it there.
       */
      إذا (scope) {
	    /* Some implementations support either a module or a scope. */
	    بدل (vpi_get(vpiType, scope)) {
		حالة vpiScope:
	          hand = vpi_handle(vpiModule, scope);
	          اكسر;
		حالة vpiModule:
	          hand = scope;
	          اكسر;
		افتراضي:
		  إذا (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle_by_name: "
				"Scope is not a vpiScope or vpiModule\n");
		  }
	          // Use vpi_chk_error() here when it is implemented.
	          أرجع 0;
	    }

      } وإلا إذا (nm_path) {
	      // The name has a path, and no other scope handle was
	      // passed in. That suggests we are looking for "a.b.c"
	      // in the root scope. So convert "a.b" to a scope and
	      // start there to look for "c".
	    hand = find_scope(nm_path, عدم, 0);
	    nm_path = 0;

      } وإلا {
	      // Special case: scope==<nil>, meaning we are looking in
	      // the root, and there is no path to the name, i.e. the
	      // string is "c" instead of "top.c". Try to find "c" as
	      // a scope and return that.
	    hand = find_scope(nm_base, عدم, 0);
      }

      إذا (hand == 0) {
	    إذا (vpi_trace) {
		  fprintf(vpi_trace, "vpi_handle_by_name: "
			  "Scope does not exist. Giving up.\n");
	    }

	    أرجع 0;
      }

	// If there is a path part, then use it to find the
	// scope. For example, if the full name is a.b.c, then
	// the nm_path string is a.b and we search for that
	// scope. If we find it, then set hand to that scope.
      إذا (nm_path) {
	    vpiHandle tmp = find_scope(nm_path, hand, 0);
	    طالما (tmp == 0 && hand != 0) {
		  hand = vpi_handle(vpiScope, hand);
		  tmp = find_scope(nm_path, hand, 0);
	    }
	    hand = tmp;
      }

	// find_name() expects escaped identifiers to be stripped
      إذا (*nm_base == '\\') {
	    // Skip the \ at the beginning
	    ++nm_base;

	    // Drop the space at the end if it exists
	    حرف *next;
	    إذا ((next = strchr(nm_base, ' '))) {
		  *next = 0;
	    }
      }

	// Now we have the correct scope, look for the item.
      vpiHandle out = find_name(nm_base, hand);

      إذا (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name: DONE\n");
      }

      أرجع out;
}

// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected
// x2: Ptr to previous node
// x3: True when net is connected to output of previous node
tuple<منطقي, vvp_net_ptr_t, منطقي> check_connected_to_concat8_and_part_sa(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent) {

      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      طالما (cur.ptr()) {
	    إذا (cur.ptr() == net2 && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
		    // Pass on the return value if found
		  أرجع {true, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
	    } وإلا إذا (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8_and_part_sa(cur.ptr()->out_, net2, cur);
		  إذا (get<0>(res)) {
			أرجع res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      أرجع {خطأ, vvp_net_ptr_t(nullptr, 0), خطأ};
}

// Check if net2 is connected to current_net through a net of vvp_fun_concat8s
// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected with concats and part_sas
// x2: Ptr to previous node
// x3: True when net is connected to output of x2
tuple<منطقي, vvp_net_ptr_t, منطقي> check_connected_to_concat8(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent)
{
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);
	// For everything connected
      طالما (cur.ptr()) {
	      // Check if it's a concat8
	    إذا (cur.ptr() == net2) {
		  أرجع {true, vvp_net_ptr_t(nullptr, 0), خطأ};
	    } وإلا إذا (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  auto res = check_connected_to_concat8(cur.ptr()->out_, net2, cur);
		  إذا (get<0>(res)) {
			إذا (!get<1>(res).ptr() && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
			      // `parent` is the last part_sa node in front of our destination
			      // We want to place the intermod node here
			      أرجع {true, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
			}
			أرجع res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      أرجع {خطأ, vvp_net_ptr_t(nullptr, 0), خطأ};
}

// Used to get intermodpath for two ports
vpiHandle vpi_handle_multi(PLI_INT32 type,
                           vpiHandle ref1,
                           vpiHandle ref2)
{
      إذا (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_multi(%d, %p, %p) -->\n",
	            type, ref1, ref2);
      }

      إذا (type != vpiInterModPath) {
	    fprintf(stderr, "sorry: vpi_handle_multi currently supports"
	            "only vpiInterModPath\n");
	    أرجع nullptr;
      }

	// Indicates whether port1 refers to a single bit
      منطقي port1_has_index = خطأ;
      صحيح port1_bit_index = 0;
      vpiPortBitInfo* port1_bit = dynamic_cast<vpiPortBitInfo*>(ref1);

      إذا (port1_bit) {
	      // Get the bit index
	    port1_has_index = true;
	    port1_bit_index = vpi_get(vpiBit, port1_bit);
	      // Update the ref1 to point to the base port
	    ref1 = vpi_handle(vpiParent, port1_bit);
      }

	// Indicates whether port2 refers to a single bit
      منطقي port2_has_index = خطأ;
      صحيح port2_bit_index = 0;
      vpiPortBitInfo* port2_bit = dynamic_cast<vpiPortBitInfo*>(ref2);

      إذا (port2_bit) {
	      // Get the bit index
	    port2_has_index = true;
	    port2_bit_index = vpi_get(vpiBit, port2_bit);
	      // Update the ref1 to point to the base port
	    ref2 = vpi_handle(vpiParent, port2_bit);
      }

      vpiPortInfo* port1 = dynamic_cast<vpiPortInfo*>(ref1);

      إذا (!port1) {
	    fprintf(stderr, "sorry: second argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    أرجع nullptr;
      }

      vpiPortInfo* port2 = dynamic_cast<vpiPortInfo*>(ref2);

      إذا (!port2) {
	    fprintf(stderr, "sorry: third argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    أرجع nullptr;
      }

	// Get the names of both ports
      std::string port1_name(vpi_get_str(vpiName, ref1));
      std::string port2_name(vpi_get_str(vpiName, ref2));

	// If both ports are vpiOutput, we have to reassign the __vpiSignal from port1
	// to port2 because otherwise the non-delayed version of the signal is dumped
	// even tho the intermodpath is correctly inserted
      __vpiSignal* output_signal = nullptr;

      إذا (port1->get_direction() == vpiOutput && port2->get_direction() == vpiOutput) {
	    vpiHandle scope_port2 = vpi_handle(vpiScope, ref2);
	    أكد(scope_port2);

	      // Iterate over nets in the scope of port2
	    vpiHandle net_i = vpi_iterate(vpiNet, scope_port2) ;
	    vpiHandle net;

	    طالما ((net = vpi_scan(net_i)) != عدم) {
		  std::string net_name(vpi_get_str(vpiName, net));

		    // Compare whether the net matches with the port name
		  إذا (net_name == port2_name) {
			output_signal = dynamic_cast<__vpiSignal*>(net);
		  }
	    }
      }

      vvp_net_t* net1 = port1->get_port();
      vvp_net_t* net2 = port2->get_port();

      إذا (net1 == nullptr || net2 == nullptr) {
	    fprintf(stderr, "Error: Could not find net. "
		    "Did you run iverilog with '-ginterconnect'?\n");
	    أرجع nullptr;
      }

      إذا (net1 == net2) {
	    fprintf(stderr, "Error: Net for both ports is the same. "
		    "Did you pass the same port twice?\n");
	    أرجع nullptr;
      }

      إذا (!dynamic_cast<vvp_fun_buft*>(net1->fun)) {
	    fprintf(stderr, "Error: functor of net1 must be"
		    "vvp_fun_buft\n");
	    أرجع nullptr;
      }

      إذا (!dynamic_cast<vvp_fun_buft*>(net2->fun)) {
	    fprintf(stderr, "Error: functor of net2 must be"
		    "vvp_fun_buft\n");
	    أرجع nullptr;
      }

	// If port1 is actually a port bit, we have to get to the correct vvp_fun_part
	// after which we insert the intermodpath delay
      إذا (port1_has_index) {
	    vvp_net_ptr_t* net1_ptr = &net1->out_;

	      // Search for part selects connected to port1
	    vvp_net_t* current_net = net1_ptr->ptr();

	    طالما (current_net) {
		  ثابت vvp_fun_part* part = dynamic_cast<vvp_fun_part*>(current_net->fun);

		    // Its a part select!
		  إذا (part) {
			  // Is it the correct part select?
			إذا (part->get_base() == (unsigned)port1_bit_index) {
			      أكد(part->get_wid() == 1);
			      net1 = current_net; // Replace net1 as this is our new start point
			      اكسر;
			}
		  }

		  current_net = current_net->port[0].ptr(); // BUFT has only one input, index 0
	    }
      }

	// Iterate over all nodes connected to port1
      vvp_net_ptr_t cur = net1->out_;
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      طالما (cur.ptr()) {
	      // Either port2 is directly connected to port1
	      // Or in the second case port2 is indirectly connected
	      // to port1 through a net of concat8s
	    منطقي is_connected, is_output;
	    vvp_net_ptr_t previous_كلي;
	    tie(is_connected, previous_كلي, is_output) = port2_has_index
		  ? check_connected_to_concat8(cur, net2, vvp_net_ptr_t(net1, 0))
		  : check_connected_to_concat8_and_part_sa(cur, net2, vvp_net_ptr_t(net1, 0));
	    إذا ( (!port2_has_index && cur.ptr() == net2) || is_connected ) {
		  vvp_net_t*new_net = جديد vvp_net_t;

		    // Create new node with intermodpath and connect port2 to it
		  صحيح width = 1; // TODO
		  vvp_fun_intermodpath*obj = جديد vvp_fun_intermodpath(new_net, width);

		  vvp_net_t* output_net = net1;
		  إذا (is_connected) {
			إذا (is_output) {
			      output_net = previous_كلي.ptr(); // net2 is direct output of part_sa
			      cur = output_net->out_;
			      prev = vvp_net_ptr_t(nullptr, 0);
			} وإلا {
			      prev = previous_كلي;
			      إذا (prev.ptr()) {
				    cur = prev.ptr()->port[prev.port()];
			      }
			}
		  }

		  new_net->fun = obj;
		  new_net->out_ = cur;

		    // Port2 is in the middle of the list
		    // Insert intermodpath before port2 and keep everything else intact
		  إذا (prev.ptr()) {
			prev.ptr()->port[prev.port()] = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; // Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		    // Port2 is first in list
		    // Insert intermodpath before port2 and keep everything else intact
		  } وإلا {
			output_net->out_ = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; //  Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		  }

		    // If both ports are vpiOutput and port2 is not a vector,
		    // we have to reassign the __vpiSignal so that the delayed
		    // values get dumped
		  إذا (output_signal && !port2_has_index) {
			net2->fil = net1->fil;
			net1->fil = nullptr;
			output_signal->كلي = net2;
		  }

		    // Create the VPI intermodpath object
		  __vpiInterModPath* intermodpath = vpip_make_intermodpath(new_net, port1, port2);
		  intermodpath->intermodpath = obj;

		    // Finally done, return the intermodpath object
		  أرجع intermodpath;
	    }

	    prev = cur;
	    cur = cur.ptr()->port[cur.port()]; // Next net in linked list
      }

      fprintf(stderr, "VPI error: Could not insert intermodpath!\n");
      fprintf(stderr, "\tport1 = %s, port1_has_index = %d, port1_bit_index = %d\n", port1_name.c_str(), port1_has_index, port1_bit_index);
      fprintf(stderr, "\tport2 = %s, port2_has_index = %d, port2_bit_index = %d\n", port2_name.c_str(), port2_has_index, port2_bit_index);
      أرجع nullptr;
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



فراغ vpi_get_delays(vpiHandle فرچ, p_vpi_delay delays)
{
      أكد(فرچ);
      أكد(delays);

      فرچ->vpi_get_delays(delays);

      إذا (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_get_delays(%p, %p) -->\n", فرچ, delays);
      }
}


فراغ vpi_put_delays(vpiHandle فرچ, p_vpi_delay delays)
{
      أكد(فرچ  );
      أكد(delays );

      فرچ->vpi_put_delays(delays);

      إذا (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_put_delays(%p, %p) -->\n", فرچ, delays);
      }
}








خارجي "C" PLI_INT32 vpi_vprintf(ثابت حرف*fmt, va_list ap)
{
      أرجع vpi_mcd_vprintf(1, fmt, ap);
}

خارجي "C" PLI_INT32 vpi_printf(ثابت حرف *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      صحيح r = vpi_mcd_vprintf(1, fmt, ap);
      va_end(ap);
      أرجع r;
}

خارجي "C" PLI_INT32 vpi_flush(فراغ)
{
      أرجع vpi_mcd_flush(1);
}


خارجي "C" PLI_INT32 vpi_sim_vcontrol(صحيح operation, va_list ap)
{
      long diag_سكو;

      بدل (operation) {
	  حالة vpiFinish:
	  حالة __ivl_legacy_vpiFinish:
            diag_سكو = va_arg(ap, PLI_INT32);
	    schedule_finish(diag_سكو);
	    اكسر;

	  حالة vpiStop:
	  حالة __ivl_legacy_vpiStop:
            diag_سكو = va_arg(ap, PLI_INT32);
	    schedule_stop(diag_سكو);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported VPI control operation %d.\n", operation);
	    أرجع 0;
      }
      أرجع 1;
}

خارجي "C" PLI_INT32 vpi_sim_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      صحيح r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      أرجع r;
}

خارجي "C" PLI_INT32 vpi_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      صحيح r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      أرجع r;
}

/*
 * This routine calculated the return value for $clog2.
 * It is easier to do it here vs trying to to use the VPI interface.
 */
خارجي "C" s_vpi_vecval vpip_calc_clog2(vpiHandle arg)
{
      s_vpi_vecval rtn;
      s_vpi_درج val;
      vvp_vector4_t vec4;
      منطقي is_neg = خطأ;  // At this point only a real can be negative.

	/* Get the value as a vvp_vector4_t. */
      val.format = vpiObjTypeVal;
      vpi_get_درج(arg, &val);
      إذا (val.format == vpiRealVal) {
	    vpi_get_درج(arg, &val);
	      /* All double values can be represented in 1024 bits. */
	    vec4 = vvp_vector4_t(1024, val.value.real);
	    إذا (val.value.real < 0) is_neg = true;
      } وإلا {
	    val.format = vpiVectorVal;
	    vpi_get_درج(arg, &val);
	    unsigned wid = vpi_get(vpiSize, arg);
	    vec4 = vvp_vector4_t(wid, BIT4_0);
	    لكل (unsigned idx=0; idx < wid; idx += 1) {
		PLI_INT32 aval = val.value.vector[idx/32].aval;
		PLI_INT32 bval = val.value.vector[idx/32].bval;
		aval >>= idx % 32;
		bval >>= idx % 32;
		صحيح bitmask = (aval&1) | ((bval<<1)&2);
		vvp_bit4_t bit = scalar_to_bit4(bitmask);
		vec4.set_bit(idx, bit);
	    }
      }

      إذا (vec4.has_xz()) {
	    rtn.aval = rtn.bval = 0xFFFFFFFFU;  /* Set to 'bx. */
	    أرجع rtn;
      }

      vvp_vector2_t vec2(vec4);

      إذا (is_neg) vec2.trim_neg(); /* This is a special trim! */
      وإلا vec2.trim(); /* This makes less work shifting. */

	/* Calculate the clog2 result. */
      PLI_INT32 res = 0;
      إذا (!vec2.is_zero()) {
	    vec2 -= vvp_vector2_t(1, vec2.size());
	    طالما(!vec2.is_zero()) {
		  res += 1;
		  vec2 >>= 1;
	    }
      }

      rtn.aval = res;
      rtn.bval = 0;
      أرجع rtn;
}

/*
 * This routine provides the information needed to implement $countdrivers.
 * It is done here for performance reasons - interrogating the drivers
 * individually via the VPI interface would be much slower.
 */
خارجي "C" فراغ vpip_كدي_drivers(vpiHandle ref, unsigned idx,
                                   unsigned counts[4])
{
      بنية __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);
      rfp->كلي->كدي_drivers(idx, counts);
}

#إذا defined(__MINGW32__) || defined (__CYGWIN__)
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
    .get_درج                  = vpi_get_درج,
    .put_درج                  = vpi_put_درج,
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
    .كدي_drivers              = vpip_كدي_drivers,
    .format_strength            = vpip_format_strength,
    .format_pretty              = vpip_format_pretty,
    .make_systf_system_defined  = vpip_make_systf_system_defined,
    .mcd_rawwrite               = vpip_mcd_rawwrite,
    .set_return_درج           = vpip_set_return_درج,
};
#endif
