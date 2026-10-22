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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
# include  <vector>
# include  <cstdio>
# include  <cstdarg>
# include  <cstring>
# include  <cassert>
# include  <cstdlib>
# include  <cmath>
# include  <iostream>

استعمال نامخلا std;
vpi_منوال_t vpi_منوال_flag = VPI_منوال_NONE;
FILE*vpi_trace = 0;

ساکن s_vpi_vlog_info  vpi_vlog_info;
ساکن s_vpi_error_info vpip_آخری_error = { 0, 0, 0, 0, 0, 0, 0 };

ثابت حرف* direction_as_string(صحیح dir)
{
      انتخاب (dir) {
	  حالت vpi_داخلہ:
	    واپس "input";
	  حالت vpiOutput:
	    واپس "output";
	  حالت vpiInout:
	    واپس "inout";
	  حالت vpiMixedIO:
	    واپس "mixed io";
	  حالت vpiNoDirection:
	    واپس "no direction";
	  طےشدہ:
	    واپس "INVALID-DIRECTION";
      }
}

__vpiHandle::~__vpiHandle()
{ }

صحیح __vpiHandle::vpi_get(صحیح)
{ واپس vpiUndefined; }

int64_t __vpiHandle::vpi_get64(صحیح code)
{ واپس (code == _vpiNexusId) ? 0 : vpiUndefined; }

حرف* __vpiHandle::vpi_get_str(صحیح)
{ واپس 0; }

خالی __vpiHandle::vpi_get_قدر(p_vpi_قدر)
{ }

vpiHandle __vpiHandle::vpi_put_قدر(p_vpi_قدر, صحیح)
{ واپس 0; }

vpiHandle __vpiHandle::vpi_handle(صحیح)
{ واپس 0; }

vpiHandle __vpiHandle::vpi_iterate(صحیح)
{ واپس 0; }

vpiHandle __vpiHandle::vpi_اشاریہ(صحیح)
{ واپس 0; }

خالی __vpiHandle::vpi_get_delays(p_vpi_delay)
{ }

خالی __vpiHandle::vpi_put_delays(p_vpi_delay)
{ }

__vpiBaseVar::__vpiBaseVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net)
: scope_(scope), name_(name), net_(net)
{
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
__vpiBaseVar::~__vpiBaseVar()
{
      vvp_net_delete(net_);
}
#اگر_ختم

/*
 * The default behavior for the vpi_free_object to an object is to
 * suppress the actual operation. This is because handles are
 * generally allocated semi-permanently within vvp context. Dynamic
 * objects will override the free_object_fun method to return an
 * appropriately effective function.
 */
ساکن صحیح suppress_free(vpiHandle)
{ واپس 1; }
__vpiHandle::free_object_fun_t __vpiHandle::free_object_fun(خالی)
{ واپس &suppress_free; }

/*
 * The vpip_string function creates a constant string from the pass
 * input. This constant string is permanently allocated from an
 * efficient string buffer store.
 */
ساخت vpip_string_chunk {
      ساخت vpip_string_chunk*اگلا_2;
      حرف ڈیٹا[64*1024 - حجم (ساخت vpip_string_chunk*)];
};

غیرعلامتی vpip_سائز(ثابت __vpiSignal *sig)
{
      واپس مطلق_قدر(sig->msb.get_قدر() - sig->lsb.get_قدر()) + 1;
}

__vpiScope* vpip_scope(__vpiSignal*sig)
{
      اگر (sig->is_netarray)
	    واپس static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.والد));
      ورنہ
	    واپس sig->within.scope;
}

__vpiScope* vpip_scope(__vpiRealVar*sig)
{
      اگر (sig->is_netarray)
	    واپس static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.والد));
      ورنہ
	    واپس sig->within.scope;
}

vpiHandle vpip_module(__vpiScope*scope)
{
      جب(scope && scope->get_type_رمز() != vpiModule) {
	    scope = scope->scope;
      }
      أكد(scope);
      واپس scope;
}

ثابت حرف *vpip_string(ثابت حرف*str)
{
      ساکن vpip_string_chunk پہلا_chunk = {0, {0}};
      ساکن vpip_string_chunk*chunk_list = &پہلا_chunk;
      ساکن غیرعلامتی chunk_fill = 0;

      غیرعلامتی len = لڑی_لمبائی(str);
      أكد( (len+1) <= حجم chunk_list->ڈیٹا );

      اگر ( (len+1) > (حجم chunk_list->ڈیٹا - chunk_fill) ) {
	    vpip_string_chunk*tmp = نیا vpip_string_chunk;
	    tmp->اگلا_2 = chunk_list;
	    chunk_list = tmp;
	    chunk_fill = 0;
      }

      حرف*res = chunk_list->ڈیٹا + chunk_fill;
      chunk_fill += len + 1;

      strcpy(res, str);
      واپس res;
}

ساکن غیرعلامتی hash_string(ثابت حرف*متن)
{
      غیرعلامتی h = 0;

      جب (*متن) {
	    h = (h << 4) ^ (h >> 28) ^ (غیرعلامتی)*متن;
	    متن += 1;
      }
      واپس h;
}

ثابت حرف* vpip_name_string(ثابت حرف*متن)
{
      ثابت غیرعلامتی HASH_سائز = 4096;
      ساکن ثابت حرف*hash_جدول[HASH_سائز] = {0};

      غیرعلامتی hash_قدر = hash_string(متن) % HASH_سائز;

	/* If we easily find the string in the hash table, then return
	   that and be done. */
      اگر (hash_جدول[hash_قدر]
	  && (strcmp(hash_جدول[hash_قدر], متن) == 0)) {
	    واپس hash_جدول[hash_قدر];
      }

	/* The existing hash entry is not a match. Replace it with the
	   newly allocated value, and return the new pointer as the
	   result to the add. */
      ثابت حرف*res = vpip_string(متن);
      hash_جدول[hash_قدر] = res;

      واپس res;
}

PLI_INT32 vpi_chk_error(p_vpi_error_info info)
{
      اگر (vpip_آخری_error.state == 0)
	    واپس 0;

      info->state = vpip_آخری_error.state;
      info->level = vpip_آخری_error.level;
      info->message = vpip_آخری_error.message;
      info->حاصل_ضرب = vpi_vlog_info.حاصل_ضرب;
      info->code = (حرف *)"";
      info->file = 0;
      info->سطر = 0;

      واپس info->level;
}

PLI_INT32 vpi_compare_objects(vpiHandle obj1, vpiHandle obj2)
{
      أكد(obj1);
      أكد(obj2);

	// Does this actually work for all cases?
      اگر (obj1 != obj2) واپس 0;
      ورنہ واپس 1;
}

/*
 * Copy the internal information to the data structure. Do not free or
 * change the tfname/user_data since they are a pointer to the real
 * string/data values. We also support passing a task or function handle
 * instead of just a handle to a vpiUserSystf.
 */
خالی vpi_get_systf_info(vpiHandle ref, p_vpi_systf_ڈیٹا ڈیٹا)
{
      ساخت __vpiUserSystf* rfp = dynamic_cast<__vpiUserSystf*>(ref);
      اگر (rfp == 0) {
	    ساخت __vpiSysTaskCall*call = dynamic_cast<__vpiSysTaskCall*>(ref);
	    أكد(call);
	    rfp = call->defn;
      }

	/* Assert that vpiUserDefn is true! */
      أكد(rfp->is_user_defn);

      ڈیٹا->type = rfp->info.type;
      ڈیٹا->sysfunctype = rfp->info.sysfunctype;
      ڈیٹا->tfname = rfp->info.tfname;
      ڈیٹا->calltf = rfp->info.calltf;
      ڈیٹا->compiletf = rfp->info.compiletf;
      ڈیٹا->sizetf = rfp->info.sizetf;
      ڈیٹا->user_ڈیٹا = rfp->info.user_ڈیٹا;
}

/*
 * When a task is called, this value is set so that vpi_handle can
 * fathom the vpi_handle(vpiSysTfCall,0) function.
 */
ساخت __vpiSysTaskCall*vpip_cur_task = 0;

PLI_INT32 vpi_free_object(vpiHandle ref)
{
      صحیح rtn;

      اگر (vpi_trace) {
	    fprintf(vpi_trace, "vpi_free_object(%p)", ref);
	    fflush(vpi_trace);
      }

      أكد(ref);
      __vpiHandle::free_object_fun_t fun = ref->free_object_fun();
      rtn = fun (ref);

      اگر (vpi_trace)
	    fprintf(vpi_trace, " --> %d\n", rtn);

      واپس rtn;
}

PLI_INT32 vpi_release_handle(vpiHandle ref)
{
	// Since SystemVerilog vpi_free_object() has been
	// renamed vpi_release_handle(), and thus
	// vpi_free_object() has been deprecated.
      واپس vpi_free_object(ref);
}

ساکن صحیح vpip_get_global(صحیح property)
{
      انتخاب (property) {

	  حالت vpi_وقت_Unit:
	  حالت vpi_وقت_Precision:
	    واپس vpip_get_وقت_precision();

	  طےشدہ:
	    fprintf(stderr, "vpi error: bad global property: %d\n", property);
	    أكد(0);
	    واپس vpiUndefined;
      }
}

ساکن ثابت حرف* vpi_property_str(PLI_INT32 code)
{
      ساکن حرف buf[32];
      انتخاب (code) {
	  حالت vpiConstType:
	    واپس "vpiConstType";
	  حالت vpiName:
	    واپس "vpiName";
	  حالت vpi_بھرا_Name:
	    واپس "vpiFullName";
	  حالت vpi_وقت_Unit:
	    واپس "vpiTimeUnit";
	  حالت vpi_وقت_Precision:
	    واپس "vpiTimePrecision";
          حالت vpi_سائز:
	    واپس "vpiSize";
          حالت _vpiNexusId:
	    واپس "_vpiNexusId";
	  طےشدہ:
	    snprintf(buf, حجم(buf), "%d", (صحیح)code);
      }
      واپس buf;
}

ثابت حرف* vpi_type_as_string(PLI_INT32 code)
{
      ساکن حرف buf[32];
      انتخاب (code) {
	  حالت vpiArrayType:
	    واپس "vpiArrayType";
	  حالت vpi_ثنائی_ہندسہ_Var:
	    واپس "vpiBitVar";
	  حالت vpiByteVar:
	    واپس "vpiByteVar";
	  حالت vpiClassVar:
	    واپس "vpiClassVar";
	  حالت vpiConstant:
	    واپس "vpiConstant";
	  حالت vpiEnumTypespec:
	    واپس "vpiEnumTypespec";
	  حالت vpiFunction:
	    واپس "vpiFunction";
	  حالت vpiGenScope:
	    واپس "vpiGenScope";
	  حالت vpiIntVar:
	    واپس "vpiIntVar";
	  حالت vpiIntegerVar:
	    واپس "vpiIntegerVar";
	  حالت vpiIterator:
	    واپس "vpiIterator";
	  حالت vpiLongIntVar:
	    واپس "vpiLongIntVar";
	  حالت vpi_حافظہ:
	    واپس "vpiMemory";
	  حالت vpi_حافظہ_لفظ:
	    واپس "vpiMemoryWord";
	  حالت vpiModule:
	    واپس "vpiModule";
	  حالت vpiNamedBegin:
	    واپس "vpiNamedBegin";
	  حالت vpiNamedEvent:
	    واپس "vpiNamedEvent";
	  حالت vpiNamedFork:
	    واپس "vpiNamedFork";
	  حالت vpiPackage:
	    واپس "vpiPackage";
	  حالت vpi_راستہ_رکن:
	    واپس "vpiPathTerm";
	  حالت vpi_پورٹ:
	    واپس "vpiPort";
	  حالت vpiNet:
	    واپس "vpiNet";
	  حالت vpiNetArray:
	    واپس "vpiNetArray";
	  حالت vpi_Net_ثنائی_ہندسہ:
	    واپس "vpiNetBit";
	  حالت vpiParameter:
	    واپس "vpiParameter";
	  حالت vpiPartSelect:
	    واپس "vpiPartSelect";
	  حالت vpiRealVar:
	    واپس "vpiRealVar";
	  حالت vpiReg:
	    واپس "vpiReg";
	  حالت vpi_Reg_ثنائی_ہندسہ:
	    واپس "vpiRegBit";
	  حالت vpiShortIntVar:
	    واپس "vpiShortIntVar";
	  حالت vpiStringVar:
	    واپس "vpiStringVar";
	  حالت vpiSysFuncCall:
	    واپس "vpiSysFuncCall";
	  حالت vpiSysTaskCall:
	    واپس "vpiSysTaskCall";
	  حالت vpiTask:
	    واپس "vpiTask";
	  حالت vpi_وقت_Var:
	    واپس "vpiTimeVar";
	  حالت vpiUserSystf:
	    واپس "vpiUserSystf";
	  طےشدہ:
	    snprintf(buf, حجم(buf), "%d", (صحیح)code);
      }
      واپس buf;
}

PLI_INT32 vpi_get(صحیح property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      اگر (property == _vpiDelaySelection) واپس vpip_delay_selection;

      اگر (ref == 0)
	    واپس vpip_get_global(property);

      اگر (property == vpiType) {
	    اگر (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_رمز()));
	    }

	    اگر (ref->get_type_رمز() == vpi_حافظہ && is_net_array(ref))
		  واپس vpiNetArray;
	    ورنہ
		  واپس ref->get_type_رمز();
      }

      صحیح res = ref->vpi_get(property);

      اگر (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get(%s, %p) --> %d\n",
		    vpi_property_str(property), ref, res);
      }

      واپس res;
}

PLI_INT64 vpi_get64(صحیح property, vpiHandle ref)
{
	// For now we only support getting the nexus id.
      اگر ((ref == 0) || (property != _vpiNexusId)) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) is not currently supported.\n",
		    vpi_property_str(property), ref);
	    واپس vpiUndefined;
      }

      int64_t res = ref->vpi_get64(property);

      اگر (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get64(%s, %p) --> %" PRId64 "\n",
		    vpi_property_str(property), ref, res);
      }

      واپس res;
}

حرف* vpi_get_str(PLI_INT32 property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      اگر (property == _vpiDelaySelection) {
	    انتخاب (vpip_delay_selection) {
		حالت _vpiDelaySelMinimum:
		  واپس simple_سیٹ_rbuf_str("MINIMUM");
		حالت _vpiDelaySelTypical:
		  واپس simple_سیٹ_rbuf_str("TYPICAL");
		حالت vpi_Delay_Sel_زیادہ_سے_زیادہ:
		  واپس simple_سیٹ_rbuf_str("MAXIMUM");
		طےشدہ:
		  أكد(0);
	    }
      }

      اگر (ref == 0) {
	    fprintf(stderr, "vpi error: vpi_get_str(%s, 0) called "
		    "with null vpiHandle.\n", vpi_property_str(property));
	    واپس 0;
      }

      اگر (property == vpiType) {
	    اگر (vpi_trace) {
		  fprintf(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_رمز()));
	    }

            PLI_INT32 type;
	    اگر (ref->get_type_رمز() == vpi_حافظہ && is_net_array(ref))
		  type = vpiNetArray;
	    ورنہ
		  type = ref->get_type_رمز();
	    واپس (حرف *)vpi_type_as_string(type);
      }

      حرف*res = ref->vpi_get_str(property);

      اگر (vpi_trace) {
	    fprintf(vpi_trace, "vpi_get_str(%s, %p) --> %s\n",
		    vpi_property_str(property), ref, res? res : "<NULL>");
      }

      واپس res;
}

__vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj)
{
      ثابت ساخت __vpiSysTaskCall*task;
      ساخت __vpiSignal*اشارہ;
      ساخت __vpiRealVar*real;
      ثابت __vpiNamedEvent*event;

      انتخاب (obj->get_type_رمز()) {
	  حالت vpiSysTaskCall:
	    task = dynamic_cast<__vpiSysTaskCall*>(obj);
	    واپس task->scope;

	  حالت vpiModule:
	    واپس dynamic_cast<__vpiScope*>(obj);

	  حالت vpiNet:
	  حالت vpiReg:
	  حالت vpiIntegerVar:
	  حالت vpi_ثنائی_ہندسہ_Var:
	  حالت vpiByteVar:
	  حالت vpiShortIntVar:
	  حالت vpiIntVar:
	  حالت vpiLongIntVar:
	    اشارہ = dynamic_cast<__vpiSignal*>(obj);
	    واپس vpip_scope(اشارہ);

	  حالت vpiRealVar:
	    real = dynamic_cast<__vpiRealVar*>(obj);
	    واپس vpip_scope(real);

	  حالت vpiNamedEvent:
	    event = dynamic_cast<__vpiNamedEvent*>(obj);
	    واپس event->get_scope();

	  حالت vpi_حافظہ:
	  حالت vpi_حافظہ_لفظ:
	  حالت vpiPartSelect:
	    واپس dynamic_cast<__vpiScope*>(obj->vpi_handle(vpiScope));

	  طےشدہ:
	    fprintf(stderr, "ERROR: vpip_scope_from_handle called with "
		    "object handle type=%d\n", obj->get_type_رمز());
	    أكد(0);
	    واپس 0;
      }
}

صحیح vpip_وقت_units_from_handle(vpiHandle obj)
{
      اگر (obj == 0)
	    واپس vpip_get_وقت_precision();

      ثابت __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      اگر (scope == 0)
	    واپس vpip_get_وقت_precision();

      واپس scope->وقت_units;
}

صحیح vpip_وقت_precision_from_handle(vpiHandle obj)
{
      اگر (obj == 0)
	    واپس vpip_get_وقت_precision();

      ثابت __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      اگر (scope == 0)
	    واپس vpip_get_وقت_precision();

      واپس scope->وقت_precision;
}

دوہرا vpip_scaled_وقت_from_handle(vvp_time64_t time, vpiHandle obj)
{
      صحیح scale = vpip_get_وقت_precision() -
	          vpip_وقت_units_from_handle(obj);
      اگر (scale >= 0)
	    واپس (دوہرا)time * قوت(10.0, scale);
      ورنہ
	    واپس (دوہرا)time / قوت(10.0, -scale);
}

خالی vpi_get_وقت(vpiHandle obj, s_vpi_وقت*vp)
{
      vvp_time64_t time;

      أكد(vp);

      time = جدول_وقت_simtime();

      انتخاب (vp->type) {
          حالت vpi_Sim_وقت:
	    vp->بلند = (time >> 32) & 0xffffffff;
	    vp->کم = time & 0xffffffff;
	    توڑو;

          حالت vpi_Scaled_Real_وقت:
	    vp->real = vpip_scaled_وقت_from_handle(time, obj);
	    توڑو;

          طےشدہ:
            fprintf(stderr, "vpi_get_time: unknown type: %d\n", (صحیح)vp->type);
            أكد(0);
	    توڑو;
      }
}

PLI_INT32 vpi_get_vlog_info(p_vpi_vlog_info vlog_info_p)
{
    اگر (vlog_info_p != 0) {
	  *vlog_info_p = vpi_vlog_info;
	  واپس 1;
    } ورنہ {
	  واپس 0;
    }
}

خالی vpi_سیٹ_vlog_info(صحیح argc, حرف** argv)
{
    ساکن حرف icarus_حاصل_ضرب[] = "Icarus Verilog";
    ساکن حرف icarus_نسخہ[] = نسخہ;
    vpi_vlog_info.حاصل_ضرب = icarus_حاصل_ضرب;
    vpi_vlog_info.نسخہ_2 = icarus_نسخہ;
    vpi_vlog_info.argc    = argc;
    vpi_vlog_info.argv    = argv;

    ساکن حرف trace_buf[1024];
    اگر (ثابت حرف*راستہ = getenv("VPI_TRACE"))  {
	  اگر (!strcmp(راستہ,"-"))
		vpi_trace = stdout;
	  ورنہ {
		vpi_trace = fopen(راستہ, "w");
		اگر (!vpi_trace) {
		      perror(راستہ);
		      باہر_نکلو(1);
		}
		setvbuf(vpi_trace, trace_buf, _IOLBF, حجم(trace_buf));
	  }
    }
}

ساکن خالی vec4_get_قدر_string(ثابت vvp_vector4_t&لفظ_val, غیرعلامتی width,
				  s_vpi_قدر*vp)
{
      غیرعلامتی nchar = width / 8;
      غیرعلامتی دم = width % 8;

      حرف*rbuf = static_cast<حرف *>(need_نتیجہ_buf(nchar + 1, RBUF_VAL));
      حرف*cp = rbuf;

      اگر (دم > 0) {
	    حرف char_val = 0;
	    برائے (غیرعلامتی idx = width-دم; idx < width ;  idx += 1) {
		  vvp_bit4_t val = لفظ_val.value(idx);
		  اگر (val == BIT4_1)
			char_val |= 1 << idx;
	    }

	    اگر (char_val != 0)
		  *cp++ = char_val;
	    ورنہ اگر (cp != rbuf)
		  *cp++ = ' ';
      }

      برائے (غیرعلامتی idx = 0 ;  idx < nchar ;  idx += 1) {
	    غیرعلامتی ثنائی_ہندسہ = (nchar - idx - 1) * 8;
	    حرف char_val = 0;
	    برائے (غیرعلامتی bdx = 0 ;  bdx < 8 ;  bdx += 1) {
		  vvp_bit4_t val = لفظ_val.value(ثنائی_ہندسہ+bdx);
		  اگر (val == BIT4_1)
			char_val |= 1 << bdx;
	    }
	    // Ignore leading null-bytes and replace other null-bytes with space.
	    // The LRM is not entirely clear on how null bytes should be handled.
	    // This is the implementation chosen for iverilog.
	    اگر (char_val != 0)
		  *cp++ = char_val;
	    ورنہ اگر (cp != rbuf)
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
خالی vpip_vec4_get_قدر(ثابت vvp_vector4_t&لفظ_val, غیرعلامتی width,
			 منطقی signed_flag, s_vpi_قدر*vp)
{
      حرف *rbuf = 0;

      انتخاب (vp->format) {
	  طےشدہ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector values.\n", (صحیح)vp->format);
	    أكد(0);

	  حالت vpiSuppressVal:
	    توڑو;

	  حالت vpiBinStrVal:
	    rbuf = static_cast<حرف *>(need_نتیجہ_buf(width+1, RBUF_VAL));
	    برائے (غیرعلامتی idx = 0 ;  idx < width ;  idx += 1) {
		  vvp_bit4_t ثنائی_ہندسہ = لفظ_val.value(idx);
		  rbuf[width-idx-1] = vvp_bit4_مقصد_ascii(ثنائی_ہندسہ);
	    }
	    rbuf[width] = 0;
	    vp->value.str = rbuf;
	    توڑو;

	  حالت vpiOctStrVal: {
		غیرعلامتی hwid = ((width+2) / 3) + 1;
		rbuf = static_cast<حرف *>(need_نتیجہ_buf(hwid, RBUF_VAL));
		vpip_vec4_مقصد_oct_str(لفظ_val, rbuf, hwid);
		vp->value.str = rbuf;
		توڑو;
	  }

	  حالت vpiDecStrVal: {
// HERE need a better estimate.
		rbuf = static_cast<حرف *>(need_نتیجہ_buf(width+1, RBUF_VAL));
		vpip_vec4_مقصد_dec_str(لفظ_val, rbuf, width+1, signed_flag);
		vp->value.str = rbuf;
		توڑو;
	  }

	  حالت vpiHexStrVal: {
		غیرعلامتی  hwid = ((width + 3) / 4) + 1;
		rbuf = static_cast<حرف *>(need_نتیجہ_buf(hwid, RBUF_VAL));
		vpip_vec4_مقصد_hex_str(لفظ_val, rbuf, hwid);
		vp->value.str = rbuf;
		توڑو;
	  }

          حالت vpiScalarVal: {
	        // scalars should be of size 1
	        أكد(width == 1);
	        انتخاب(لفظ_val.value(0)) {
		    حالت BIT4_0:
		      vp->value.scalar = vpi0;
		      توڑو;
	            حالت BIT4_1:
                      vp->value.scalar = vpi1;
                      توڑو;
	            حالت BIT4_X:
                      vp->value.scalar = vpiX;
                      توڑو;
	            حالت BIT4_Z:
                      vp->value.scalar = vpiZ;
                      توڑو;
		}
                توڑو;
          }

	  حالت vpiIntVal: {
		طویل val = 0;
		vvp_bit4_t pad = BIT4_0;
		اگر (signed_flag && لفظ_val.size() > 0)
		      pad = لفظ_val.value(لفظ_val.size()-1);

		برائے (غیرعلامتی idx = 0 ; idx < 8*حجم(val) ;  idx += 1) {
		      vvp_bit4_t val4 = pad;
		      اگر (idx < لفظ_val.size())
			    val4 = لفظ_val.value(idx);
		      اگر (val4 == BIT4_1)
			    val |= 1L << idx;
		}

		vp->value.integer = val;
		توڑو;
	  }

	  حالت vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpi_سمتیہ_Val;
	    // fallthrough
	  حالت vpi_سمتیہ_Val: {
		غیرعلامتی hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_نتیجہ_buf(hwid * حجم(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		برائے (غیرعلامتی idx = 0 ;  idx < width ;  idx += 1) {
		      انتخاب (لفظ_val.value(idx)) {
			  حالت BIT4_0:
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    توڑو;
			  حالت BIT4_1:
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    توڑو;
			  حالت BIT4_X:
			    op->aval |= (1 << idx % 32);
			    op->bval |= (1 << idx % 32);
			    توڑو;
			  حالت BIT4_Z:
			    op->aval &= ~(1 << idx % 32);
			    op->bval |=  (1 << idx % 32);
			    توڑو;
		      }
		      اگر (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		توڑو;
	  }

	  حالت vpiStringVal:
		vec4_get_قدر_string(لفظ_val, width, vp);
		توڑو;

	  حالت vpiRealVal:
		vector4_مقصد_قدر(لفظ_val, vp->value.real, signed_flag);
		توڑو;
      }
}

خالی vpip_vec2_get_قدر(ثابت vvp_vector2_t&لفظ_val, غیرعلامتی width,
			 منطقی signed_flag, s_vpi_قدر*vp)
{
      انتخاب (vp->format) {
	  طےشدہ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting vector2 values.\n", (صحیح)vp->format);
	    أكد(0);

	  حالت vpiSuppressVal:
	    توڑو;

	  حالت vpiIntVal:
	    vector2_مقصد_قدر(لفظ_val, vp->value.integer, signed_flag);
	    توڑو;

	  حالت vpiObjTypeVal:
	    vp->format = vpi_سمتیہ_Val;
	    // fallthrough
	  حالت vpi_سمتیہ_Val: {
		غیرعلامتی hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_نتیجہ_buf(hwid * حجم(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		برائے (غیرعلامتی idx = 0 ;  idx < width ;  idx += 1) {
		      اگر (لفظ_val.value(idx)) {
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      } ورنہ {
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      }
		      اگر (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		توڑو;
	  }
      }

}

/*
 * Convert a real value to the appropriate integer.
 */
ساکن PLI_INT32 get_real_as_int(دوہرا real)
{
      دوہرا rtn;

	/* We would normally want to return 'bx for a NaN or
	 * +/- infinity, but for an integer the standard says
	 * to convert 'bx to 0, so we just return 0. */
      اگر (real != real || (real && (real == 0.5*real))) {
	    واپس 0;
      }

      rtn = std::باری_2(real);

      واپس (PLI_INT32) rtn;
}

/*
 * This is a generic function to convert a double value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer.
 */
خالی vpip_real_get_قدر(دوہرا real, s_vpi_قدر*vp)
{
      حرف *rbuf = 0;

      انتخاب (vp->format) {
	  طےشدہ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting real values.\n", (صحیح)vp->format);
	    أكد(0);

	  حالت vpiSuppressVal:
	    توڑو;

	  حالت vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiRealVal;
	    // fallthrough
	  حالت vpiRealVal:
	    vp->value.real = real;
	    توڑو;

	  حالت vpiIntVal:
	    vp->value.integer = get_real_as_int(real);
	    توڑو;

	  حالت vpiDecStrVal:
	    rbuf = static_cast<حرف *>(need_نتیجہ_buf(1025, RBUF_VAL));
	    vpip_vec4_مقصد_dec_str(vvp_vector4_t(1024, real), rbuf, 1025, سچ);
	    vp->value.str = rbuf;
	    توڑو;
      }
}

دوہرا real_from_vpi_قدر(s_vpi_قدر*vp)
{
      vvp_vector4_t vec4(1024);
      دوہرا نتیجہ;
      منطقی is_signed = جھوٹ;

      انتخاب (vp->format) {
	  طےشدہ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "putting real values.\n", (صحیح)vp->format);
	    أكد(0);

	  حالت vpiRealVal:
	    نتیجہ = vp->value.real;
	    توڑو;

	  حالت vpiIntVal:
	    نتیجہ = (دوہرا) vp->value.integer;
	    توڑو;

	  حالت vpiBinStrVal:
	    vpip_bin_str_مقصد_vec4(vec4, vp->value.str);
	    اگر (vp->value.str[0] == '-') is_signed = سچ;
	    vector4_مقصد_قدر(vec4, نتیجہ, is_signed);
	    توڑو;

	  حالت vpiOctStrVal:
	    vpip_oct_str_مقصد_vec4(vec4, vp->value.str);
	    اگر (vp->value.str[0] == '-') is_signed = سچ;
	    vector4_مقصد_قدر(vec4, نتیجہ, is_signed);
	    توڑو;

	  حالت vpiDecStrVal:
	    vpip_dec_str_مقصد_vec4(vec4, vp->value.str);
	    اگر (vp->value.str[0] == '-') is_signed = سچ;
	    vector4_مقصد_قدر(vec4, نتیجہ, is_signed);
	    توڑو;

	  حالت vpiHexStrVal:
	    vpip_hex_str_مقصد_vec4(vec4, vp->value.str);
	    اگر (vp->value.str[0] == '-') is_signed = سچ;
	    vector4_مقصد_قدر(vec4, نتیجہ, is_signed);
	    توڑو;

      }

      واپس نتیجہ;
}

خالی vpip_string_get_قدر(ثابت string&val, s_vpi_قدر*vp)
{
      حرف *rbuf = 0;

      انتخاب (vp->format) {
	  طےشدہ:
	    fprintf(stderr, "sorry: Format %d not implemented for "
	                    "getting string values.\n", (صحیح)vp->format);
	    أكد(0);

	  حالت vpiSuppressVal:
	    توڑو;

	  حالت vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiStringVal;
	    // fallthrough
	  حالت vpiStringVal:
	    rbuf = static_cast<حرف *>(need_نتیجہ_buf(val.size() + 1, RBUF_VAL));
	    strcpy(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    توڑو;
      }
}


خالی vpi_get_قدر(vpiHandle اظہاریہ, s_vpi_قدر*vp)
{
      أكد(اظہاریہ);
      أكد(vp);

	// Never bother with suppressed values. All the derived
	// classes can ignore this type.
      اگر (vp->format == vpiSuppressVal)
	    واپس;

      اظہاریہ->vpi_get_قدر(vp);

      اگر (vpi_trace) انتخاب (vp->format) {
		حالت vpiStringVal:
		  fprintf(vpi_trace,"vpi_get_value(%p=<%d>) -> string=\"%s\"\n",
			  اظہاریہ, اظہاریہ->get_type_رمز(), vp->value.str);
		  توڑو;

		حالت vpiBinStrVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> binstr=%s\n",
			  اظہاریہ->get_type_رمز(), vp->value.str);
		  توڑو;

		حالت vpiIntVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> int=%d\n",
			  اظہاریہ->get_type_رمز(), (صحیح)vp->value.integer);
		  توڑو;

		حالت vpiSuppressVal:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <suppress>\n",
			  اظہاریہ->get_type_رمز());
		  توڑو;

		طےشدہ:
		  fprintf(vpi_trace, "vpi_get_value(<%d>...) -> <%d>=?\n",
			  اظہاریہ->get_type_رمز(), (صحیح)vp->format);
	    }
}

ساخت vpip_put_قدر_event : vvp_gen_event_s {
      vpiHandle handle;
      s_vpi_قدر value;
      صحیح flags;
      مجازی خالی تسلسل_تسلسل() override;
      ~vpip_put_قدر_event() override { }
};

خالی vpip_put_قدر_event::تسلسل_تسلسل()
{
      handle->vpi_put_قدر(&value, flags);
      انتخاب (value.format) {
	    /* Free the copied string. */
	  حالت vpiBinStrVal:
	  حالت vpiOctStrVal:
	  حالت vpiDecStrVal:
	  حالت vpiHexStrVal:
	  حالت vpiStringVal:
	    آزاد(value.value.str);
	    توڑو;
	    /* Free the copied time structure. */
	  حالت vpi_وقت_Val:
	    آزاد(value.value.time);
	    توڑو;
	    /* Free the copied vector structure. */
	  حالت vpi_سمتیہ_Val:
	    آزاد(value.value.vector);
	    توڑو;
	    /* Free the copied strength structure. */
	  حالت vpiStrengthVal:
	    آزاد(value.value.strength);
	    توڑو;
	    /* Everything else is static in the structure. */
	  طےشدہ:
	    توڑو;
      }
}

/* Make a copy of a pointer to a time structure. */
ساکن t_vpi_وقت *timedup(ثابت t_vpi_وقت *val)
{
      t_vpi_وقت *rtn;
      rtn = static_cast<t_vpi_وقت *> (حافظہ_دو(حجم(t_vpi_وقت)));
      *rtn = *val;
      واپس rtn;
}

/* Make a copy of a pointer to a vector value structure. */
ساکن t_vpi_vecval *vectordup(ثابت t_vpi_vecval *val, PLI_INT32 size)
{
      غیرعلامتی عدد_بائٹیں;
      t_vpi_vecval *rtn;
      أكد(size > 0);
      عدد_بائٹیں = ((size + 31)/32)*حجم(t_vpi_vecval);
      rtn = static_cast<t_vpi_vecval *> (حافظہ_دو(عدد_بائٹیں));
      حافظہ_نقل(rtn, val, عدد_بائٹیں);
      واپس rtn;
}

/* Make a copy of a pointer to a strength structure. */
ساکن t_vpi_strengthval *strengthdup(ثابت t_vpi_strengthval *val)
{
      t_vpi_strengthval *rtn;
      rtn = static_cast<t_vpi_strengthval *>
            (حافظہ_دو(حجم(t_vpi_strengthval)));
      *rtn = *val;
      واپس rtn;
}

vpiHandle vpi_put_قدر(vpiHandle obj, s_vpi_قدر*vp,
			s_vpi_وقت*when, PLI_INT32 flags)
{
      أكد(obj);

      flags &= ~vpiReturnEvent;

      اگر (flags!=vpiNoDelay && flags!=vpiForceFlag && flags!=vpiReleaseFlag) {
	    vvp_time64_t dly;
	    صحیح scale;

	    اگر (vpi_get(vpiAutomatic, obj)) {
		  fprintf(stderr, "VPI error: cannot put a value with "
				  "a delay on automatically allocated "
				  "variable '%s'.\n",
				  vpi_get_str(vpiName, obj));
		  واپس 0;
	    }

	    أكد(when != 0);

	    انتخاب (when->type) {
		حالت vpi_Scaled_Real_وقت:
		  scale = vpip_وقت_units_from_handle(obj) -
		          vpip_get_وقت_precision();
		  اگر (scale >= 0) {
			dly = (vvp_time64_t)(when->real * قوت(10.0, scale));
		  } ورنہ {
			dly = (vvp_time64_t)(when->real / قوت(10.0, -scale));
		  }
		  توڑو;
		حالت vpi_Sim_وقت:
		  dly = vpip_timestruct_مقصد_وقت(when);
		  توڑو;
		طےشدہ:
		  dly = 0;
		  توڑو;
	    }

	    اگر ((dly == 0) && جدول_وقت_at_rosync()) {
		  fprintf(stderr, "VPI error: attempted to put a value to "
				  "variable '%s' during a read-only synch "
				  "callback.\n", vpi_get_str(vpiName, obj));
		  واپس 0;
	    }

	    vpip_put_قدر_event*put = نیا vpip_put_قدر_event;
	    put->handle = obj;
	    اگر (dynamic_cast<__vpiNamedEvent*>(obj)) {
		  put->value.format = vpiIntVal;
		  put->value.value.integer = 0;
	    } ورنہ {
		  أكد(vp);
		  put->value = *vp;
	    }
	      /* Since this is a scheduled put event we must copy any pointer
	       * data to keep it available until the event is actually run. */
	    انتخاب (put->value.format) {
		  /* Copy the string items. */
		حالت vpiBinStrVal:
		حالت vpiOctStrVal:
		حالت vpiDecStrVal:
		حالت vpiHexStrVal:
		حالت vpiStringVal:
		  put->value.value.str = strdup(put->value.value.str);
		  توڑو;
		  /* Copy a time pointer item. */
		حالت vpi_وقت_Val:
		  put->value.value.time = timedup(put->value.value.time);
		  توڑو;
		  /* Copy a vector pointer item. */
		حالت vpi_سمتیہ_Val:
		  put->value.value.vector = vectordup(put->value.value.vector,
		                                      vpi_get(vpi_سائز, obj));
		  توڑو;
		  /* Copy a strength pointer item. */
		حالت vpiStrengthVal:
		  put->value.value.strength =
		      strengthdup(put->value.value.strength);
		  توڑو;
		  /* Everything thing else is already in the structure. */
		طےشدہ:
		  توڑو;
	    }
	    put->flags = flags;
	    جدول_وقت_generic(put, dly, جھوٹ, سچ, سچ);
	    واپس 0;
      }

      اگر (جدول_وقت_at_rosync()) {
            fprintf(stderr, "VPI error: attempted to put a value to "
			    "variable '%s' during a read-only synch "
			    "callback.\n", vpi_get_str(vpiName, obj));
            واپس 0;
      }

      obj->vpi_put_قدر(vp, flags);

      واپس 0;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle res = 0;

      اگر (ref == 0) {
	      // A few types can apply to a nil handle. These are ways
	      // that the current function can get started finding things.
	    انتخاب (type) {

		حالت vpiScope:
		    // The IEEE1364-2005 doesn't seem to allow this,
		    // but some users seem to think it's handy, so
		    // return the scope that contains this SysTfCall.
		  أكد(vpip_cur_task);
		  res = vpip_cur_task->vpi_handle(vpiScope);
		  توڑو;

		حالت vpiSysTfCall:
		    // This is how VPI users get a first handle into
		    // the system. This is the handle of the system
		    // task/function call currently being executed.
		  اگر (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle(vpiSysTfCall, 0) "
				"-> %p (%s)\n", vpip_cur_task,
				vpip_cur_task->defn->info.tfname);
		  }
		  واپس vpip_cur_task;

		طےشدہ:
		  fprintf(stderr, "VPI error: vpi_handle(type=%d, ref=0).\n",
			  (صحیح)type);
		  res = 0;
		  توڑو;
	    }

      } ورنہ {

	    اگر (type == vpiSysTfCall) {
		  fprintf(stderr, "VPI error: vpi_handle(vpiSysTfCall, "
			  "ref!=0).\n");
		  واپس 0;
	    }

	    res = ref->vpi_handle(type);
      }


      اگر (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle(vpiScope, ref=%p) "
		    "-> %p\n", vpip_cur_task, ref);
      }

      واپس res;
}

ساکن vpiHandle vpip_make_udp_iterator()
{
// HERE: Add support for iterating over UDP definitions.
//       See 26.6.16 (page 400 in 1364-2005).
      واپس 0;
}

/*
 * This function asks the object to return an iterator for
 * the specified reference. It is up to the iterate_ method to
 * allocate a properly formed iterator.
 */
ساکن vpiHandle vpi_iterate_global(صحیح type)
{
      انتخاب (type) {
	  حالت vpiInstance:
	    // fallthrough
	  حالت vpiModule:
	    // fallthrough
	  حالت vpi_پروگرام:
	    // fallthrough
	  حالت vpiInterface:
	    // fallthrough
	  حالت vpiPackage:
	    واپس vpip_make_جڑ_iterator(type);

	  حالت vpiUdpDefn:
	    واپس vpip_make_udp_iterator();

	  حالت vpiUserSystf:
	    واپس vpip_make_systf_iterator();
      }

      واپس 0;
}

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle rtn = 0;

      أكد(vpi_منوال_flag != VPI_منوال_NONE);
      اگر (vpi_منوال_flag == VPI_منوال_رجسٹر) {
	    fprintf(stderr, "vpi error: vpi_iterate called during "
		    "vpi_register_systf. You can't do that!\n");
	    واپس 0;
      }

      اگر (ref == 0)
	    rtn = vpi_iterate_global(type);
      ورنہ
	    rtn = ref->vpi_iterate(type);

      اگر (vpi_trace) {
	    fprintf(vpi_trace, "vpi_iterate(%d, %p) ->%s\n",
	    (صحیح)type, ref, rtn ? "" : " (null)");
      }

      واپس rtn;
}

vpiHandle vpi_handle_by_اشاریہ(vpiHandle ref, PLI_INT32 idx)
{
      أكد(ref);
      واپس ref->vpi_اشاریہ(idx);
}

ساکن vpiHandle ڈھونڈو_name(ثابت حرف *name, vpiHandle handle)
{
      vpiHandle rtn = 0;
      __vpiScope*ref = dynamic_cast<__vpiScope*>(handle);

      /* check module names */
      اگر (!strcmp(name, vpi_get_str(vpiName, handle)))
	    rtn = handle;

      /* brute force search for the name in all objects in this scope */
      برائے (غیرعلامتی i = 0 ;  i < ref->intern.size() ;  i += 1) {
	      /* The standard says that since a port does not have a full
	       * name it cannot be found by name. Because of this we need
	       * to skip ports here so the correct handle can be located. */
	    اگر (vpi_get(vpiType, ref->intern[i]) == vpi_پورٹ) جاری;
	    ثابت حرف *nm = vpi_get_str(vpiName, ref->intern[i]);
	    اگر (nm && !strcmp(name, nm)) {
		  rtn = ref->intern[i];
		  توڑو;
	    } ورنہ اگر (vpi_get(vpiType, ref->intern[i]) == vpi_حافظہ ||
	               vpi_get(vpiType, ref->intern[i]) == vpiNetArray) {
		  /* We need to iterate on the words */
		  vpiHandle لفظ_i, لفظ_h;
		  لفظ_i = vpi_iterate(vpi_حافظہ_لفظ, ref->intern[i]);
		  جب (لفظ_i && (لفظ_h = vpi_جائزہ(لفظ_i))) {
			nm = vpi_get_str(vpiName, لفظ_h);
			اگر (nm && !strcmp(name, nm)) {
			      rtn = لفظ_h;
			      vpi_free_object(لفظ_i);
			      توڑو;
			}
		  }
	    }
	    /* found it yet? */
	    اگر (rtn) توڑو;
      }

      واپس rtn;
}

// Find the end of the escaped identifier or simple identifier
ساکن حرف * ڈھونڈو_rest(حرف *name)
{
      حرف *rest;
      اگر (*name == '\\') {
	    rest = strchr(name, ' ');
	    اگر (rest) {
		  *rest++ = 0; // The space is not part of the string
		    // There should be a '.' after the escaped ID if there is
		    // anything more. If it is missing add it to avoid a crash
		  اگر ((*rest != '.') && (*rest != 0)) {
			*--rest = '.';
			fprintf(stderr, "ERROR: Malformed scope string: \"%s\"", name);
		  }
		  اگر (*rest == 0) {
			rest = 0;
		  }
	    }
      } ورنہ {
	    rest = strchr(name, '.');
      }
      واپس rest;
}

ساکن vpiHandle ڈھونڈو_scope(ثابت حرف *name, vpiHandle handle, صحیح گہرائی)
{

      vpiHandle iter = handle==0
	    ? vpi_iterate(vpiModule, تہی)
	    : vpi_iterate(vpi_اندرونی_Scope, handle);

      vector<حرف> name_buf (لڑی_لمبائی(name)+1);
      strcpy(&name_buf[0], name);
      حرف*nm_پہلا = &name_buf[0];
      حرف*nm_rest = ڈھونڈو_rest(nm_پہلا);
      اگر (*nm_پہلا == '\\') ++nm_پہلا;
      اگر (nm_rest) {
	    *nm_rest++ = 0;
      }

      vpiHandle rtn = 0;
      vpiHandle hand;
      جب (iter && (hand = vpi_جائزہ(iter))) {
	    ثابت حرف *nm = vpi_get_str(vpiName, hand);

	    اگر (strcmp(nm_پہلا,nm)==0) {
		  اگر (nm_rest)
			rtn=ڈھونڈو_scope(nm_rest, hand, گہرائی+1);
		  ورنہ
			rtn = hand;
	    }

	    /* found it yet ? */
	    اگر (rtn) {
		  vpi_free_object(iter);
		  توڑو;
	    }
      }

      واپس rtn;
}

// Find the end of the first escaped identifier or simple identifier
ساکن حرف * ڈھونڈو_اگلا(حرف *name)
{
      حرف *اگلا_2;
      اگر (*name == '\\') {
	    اگلا_2 = strchr(name, ' ');
	    اگر (اگلا_2 && *++اگلا_2 == 0) اگلا_2 = 0;
      } ورنہ {
	    اگلا_2 = strchr(name, '.');
      }
      واپس اگلا_2;
}

vpiHandle vpi_handle_by_name(ثابت حرف *name, vpiHandle scope)
{
      vpiHandle hand;

      اگر (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name(%s, %p) -->\n",
		    name, scope);
      }

	// Chop the name into path and base. For example, if the name
	// is "a.b.c", then nm_path becomes "a.b" and nm_base becomes
	// "c". If the name is "c" then nm_path is nil and nm_base is "c".
      vector<حرف> name_buf (لڑی_لمبائی(name)+1);
      strcpy(&name_buf[0], name);
      حرف*nm_راستہ = &name_buf[0];
      حرف*nm_بنیاد;

	// Check to see if we have an escaped identifier and if so search
	// the long way because a '.' could be in the escaped name.
      اگر (strchr(nm_راستہ, '\\')) {
	    حرف *اگلا_2;
	    nm_بنیاد = nm_راستہ;
	    جب ((اگلا_2 = ڈھونڈو_اگلا(nm_بنیاد))) {
		  nm_بنیاد = ++اگلا_2;
	    }
	    اگر (nm_راستہ == nm_بنیاد) {
		  nm_راستہ = 0;
	    } ورنہ {
		  *(nm_بنیاد-1) = 0;
	    }

	// If there is no escaped identifier then just look for the last '.'
      } ورنہ {
	    nm_بنیاد = strrchr(nm_راستہ, '.');
	    اگر (nm_بنیاد) {
		  *nm_بنیاد++ = 0;
	    } ورنہ {
		  nm_بنیاد = nm_راستہ;
		  nm_راستہ = 0;
	    }
      }

      /* If scope provided, look in corresponding module; otherwise
       * traverse the hierarchy specified in name to find the leaf module
       * and try finding it there.
       */
      اگر (scope) {
	    /* Some implementations support either a module or a scope. */
	    انتخاب (vpi_get(vpiType, scope)) {
		حالت vpiScope:
	          hand = vpi_handle(vpiModule, scope);
	          توڑو;
		حالت vpiModule:
	          hand = scope;
	          توڑو;
		طےشدہ:
		  اگر (vpi_trace) {
			fprintf(vpi_trace, "vpi_handle_by_name: "
				"Scope is not a vpiScope or vpiModule\n");
		  }
	          // Use vpi_chk_error() here when it is implemented.
	          واپس 0;
	    }

      } ورنہ اگر (nm_راستہ) {
	      // The name has a path, and no other scope handle was
	      // passed in. That suggests we are looking for "a.b.c"
	      // in the root scope. So convert "a.b" to a scope and
	      // start there to look for "c".
	    hand = ڈھونڈو_scope(nm_راستہ, تہی, 0);
	    nm_راستہ = 0;

      } ورنہ {
	      // Special case: scope==<nil>, meaning we are looking in
	      // the root, and there is no path to the name, i.e. the
	      // string is "c" instead of "top.c". Try to find "c" as
	      // a scope and return that.
	    hand = ڈھونڈو_scope(nm_بنیاد, تہی, 0);
      }

      اگر (hand == 0) {
	    اگر (vpi_trace) {
		  fprintf(vpi_trace, "vpi_handle_by_name: "
			  "Scope does not exist. Giving up.\n");
	    }

	    واپس 0;
      }

	// If there is a path part, then use it to find the
	// scope. For example, if the full name is a.b.c, then
	// the nm_path string is a.b and we search for that
	// scope. If we find it, then set hand to that scope.
      اگر (nm_راستہ) {
	    vpiHandle tmp = ڈھونڈو_scope(nm_راستہ, hand, 0);
	    جب (tmp == 0 && hand != 0) {
		  hand = vpi_handle(vpiScope, hand);
		  tmp = ڈھونڈو_scope(nm_راستہ, hand, 0);
	    }
	    hand = tmp;
      }

	// find_name() expects escaped identifiers to be stripped
      اگر (*nm_بنیاد == '\\') {
	    // Skip the \ at the beginning
	    ++nm_بنیاد;

	    // Drop the space at the end if it exists
	    حرف *اگلا_2;
	    اگر ((اگلا_2 = strchr(nm_بنیاد, ' '))) {
		  *اگلا_2 = 0;
	    }
      }

	// Now we have the correct scope, look for the item.
      vpiHandle out = ڈھونڈو_name(nm_بنیاد, hand);

      اگر (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_by_name: DONE\n");
      }

      واپس out;
}

// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected
// x2: Ptr to previous node
// x3: True when net is connected to output of previous node
tuple<منطقی, vvp_net_ptr_t, منطقی> check_connected_مقصد_concat8_and_part_sa(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t والد) {

      vvp_net_ptr_t prev = vvp_net_ptr_t(خالی_اشاریہ, 0);

      جب (cur.ptr()) {
	    اگر (cur.ptr() == net2 && dynamic_cast<vvp_fun_part_sa*>(والد.ptr()->fun)) {
		    // Pass on the return value if found
		  واپس {سچ, prev.ptr() ? prev : والد, prev.ptr() == خالی_اشاریہ};
	    } ورنہ اگر (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  خودکار res = check_connected_مقصد_concat8_and_part_sa(cur.ptr()->برآمد_2, net2, cur);
		  اگر (get<0>(res)) {
			واپس res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->پورٹ[cur.پورٹ()];
      }

	// net2 is not connected to this concat8
      واپس {جھوٹ, vvp_net_ptr_t(خالی_اشاریہ, 0), جھوٹ};
}

// Check if net2 is connected to current_net through a net of vvp_fun_concat8s
// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected with concats and part_sas
// x2: Ptr to previous node
// x3: True when net is connected to output of x2
tuple<منطقی, vvp_net_ptr_t, منطقی> check_connected_مقصد_concat8(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t والد)
{
      vvp_net_ptr_t prev = vvp_net_ptr_t(خالی_اشاریہ, 0);
	// For everything connected
      جب (cur.ptr()) {
	      // Check if it's a concat8
	    اگر (cur.ptr() == net2) {
		  واپس {سچ, vvp_net_ptr_t(خالی_اشاریہ, 0), جھوٹ};
	    } ورنہ اگر (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  خودکار res = check_connected_مقصد_concat8(cur.ptr()->برآمد_2, net2, cur);
		  اگر (get<0>(res)) {
			اگر (!get<1>(res).ptr() && dynamic_cast<vvp_fun_part_sa*>(والد.ptr()->fun)) {
			      // `parent` is the last part_sa node in front of our destination
			      // We want to place the intermod node here
			      واپس {سچ, prev.ptr() ? prev : والد, prev.ptr() == خالی_اشاریہ};
			}
			واپس res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->پورٹ[cur.پورٹ()];
      }

	// net2 is not connected to this concat8
      واپس {جھوٹ, vvp_net_ptr_t(خالی_اشاریہ, 0), جھوٹ};
}

// Used to get intermodpath for two ports
vpiHandle vpi_handle_multi(PLI_INT32 type,
                           vpiHandle ref1,
                           vpiHandle ref2)
{
      اگر (vpi_trace) {
	    fprintf(vpi_trace, "vpi_handle_multi(%d, %p, %p) -->\n",
	            type, ref1, ref2);
      }

      اگر (type != vpi_Inter_Mod_راستہ) {
	    fprintf(stderr, "sorry: vpi_handle_multi currently supports"
	            "only vpiInterModPath\n");
	    واپس خالی_اشاریہ;
      }

	// Indicates whether port1 refers to a single bit
      منطقی port1_has_اشاریہ = جھوٹ;
      صحیح port1_ثنائی_ہندسہ_اشاریہ = 0;
      vpi_پورٹ_ثنائی_ہندسہ_Info* port1_ثنائی_ہندسہ = dynamic_cast<vpi_پورٹ_ثنائی_ہندسہ_Info*>(ref1);

      اگر (port1_ثنائی_ہندسہ) {
	      // Get the bit index
	    port1_has_اشاریہ = سچ;
	    port1_ثنائی_ہندسہ_اشاریہ = vpi_get(vpi_ثنائی_ہندسہ, port1_ثنائی_ہندسہ);
	      // Update the ref1 to point to the base port
	    ref1 = vpi_handle(vpi_والد, port1_ثنائی_ہندسہ);
      }

	// Indicates whether port2 refers to a single bit
      منطقی port2_has_اشاریہ = جھوٹ;
      صحیح port2_ثنائی_ہندسہ_اشاریہ = 0;
      vpi_پورٹ_ثنائی_ہندسہ_Info* port2_ثنائی_ہندسہ = dynamic_cast<vpi_پورٹ_ثنائی_ہندسہ_Info*>(ref2);

      اگر (port2_ثنائی_ہندسہ) {
	      // Get the bit index
	    port2_has_اشاریہ = سچ;
	    port2_ثنائی_ہندسہ_اشاریہ = vpi_get(vpi_ثنائی_ہندسہ, port2_ثنائی_ہندسہ);
	      // Update the ref1 to point to the base port
	    ref2 = vpi_handle(vpi_والد, port2_ثنائی_ہندسہ);
      }

      vpi_پورٹ_Info* port1 = dynamic_cast<vpi_پورٹ_Info*>(ref1);

      اگر (!port1) {
	    fprintf(stderr, "sorry: second argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    واپس خالی_اشاریہ;
      }

      vpi_پورٹ_Info* port2 = dynamic_cast<vpi_پورٹ_Info*>(ref2);

      اگر (!port2) {
	    fprintf(stderr, "sorry: third argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    واپس خالی_اشاریہ;
      }

	// Get the names of both ports
      std::string port1_name(vpi_get_str(vpiName, ref1));
      std::string port2_name(vpi_get_str(vpiName, ref2));

	// If both ports are vpiOutput, we have to reassign the __vpiSignal from port1
	// to port2 because otherwise the non-delayed version of the signal is dumped
	// even tho the intermodpath is correctly inserted
      __vpiSignal* output_signal = خالی_اشاریہ;

      اگر (port1->get_direction() == vpiOutput && port2->get_direction() == vpiOutput) {
	    vpiHandle scope_port2 = vpi_handle(vpiScope, ref2);
	    أكد(scope_port2);

	      // Iterate over nets in the scope of port2
	    vpiHandle net_i = vpi_iterate(vpiNet, scope_port2) ;
	    vpiHandle net;

	    جب ((net = vpi_جائزہ(net_i)) != تہی) {
		  std::string net_name(vpi_get_str(vpiName, net));

		    // Compare whether the net matches with the port name
		  اگر (net_name == port2_name) {
			output_signal = dynamic_cast<__vpiSignal*>(net);
		  }
	    }
      }

      vvp_net_t* net1 = port1->get_پورٹ();
      vvp_net_t* net2 = port2->get_پورٹ();

      اگر (net1 == خالی_اشاریہ || net2 == خالی_اشاریہ) {
	    fprintf(stderr, "Error: Could not find net. "
		    "Did you run iverilog with '-ginterconnect'?\n");
	    واپس خالی_اشاریہ;
      }

      اگر (net1 == net2) {
	    fprintf(stderr, "Error: Net for both ports is the same. "
		    "Did you pass the same port twice?\n");
	    واپس خالی_اشاریہ;
      }

      اگر (!dynamic_cast<vvp_fun_buft*>(net1->fun)) {
	    fprintf(stderr, "Error: functor of net1 must be"
		    "vvp_fun_buft\n");
	    واپس خالی_اشاریہ;
      }

      اگر (!dynamic_cast<vvp_fun_buft*>(net2->fun)) {
	    fprintf(stderr, "Error: functor of net2 must be"
		    "vvp_fun_buft\n");
	    واپس خالی_اشاریہ;
      }

	// If port1 is actually a port bit, we have to get to the correct vvp_fun_part
	// after which we insert the intermodpath delay
      اگر (port1_has_اشاریہ) {
	    vvp_net_ptr_t* net1_ptr = &net1->برآمد_2;

	      // Search for part selects connected to port1
	    vvp_net_t* current_net = net1_ptr->ptr();

	    جب (current_net) {
		  ثابت vvp_fun_part* part = dynamic_cast<vvp_fun_part*>(current_net->fun);

		    // Its a part select!
		  اگر (part) {
			  // Is it the correct part select?
			اگر (part->get_بنیاد() == (غیرعلامتی)port1_ثنائی_ہندسہ_اشاریہ) {
			      أكد(part->get_wid() == 1);
			      net1 = current_net; // Replace net1 as this is our new start point
			      توڑو;
			}
		  }

		  current_net = current_net->پورٹ[0].ptr(); // BUFT has only one input, index 0
	    }
      }

	// Iterate over all nodes connected to port1
      vvp_net_ptr_t cur = net1->برآمد_2;
      vvp_net_ptr_t prev = vvp_net_ptr_t(خالی_اشاریہ, 0);

      جب (cur.ptr()) {
	      // Either port2 is directly connected to port1
	      // Or in the second case port2 is indirectly connected
	      // to port1 through a net of concat8s
	    منطقی is_connected, is_output;
	    vvp_net_ptr_t پچھلا_گرہ;
	    tie(is_connected, پچھلا_گرہ, is_output) = port2_has_اشاریہ
		  ? check_connected_مقصد_concat8(cur, net2, vvp_net_ptr_t(net1, 0))
		  : check_connected_مقصد_concat8_and_part_sa(cur, net2, vvp_net_ptr_t(net1, 0));
	    اگر ( (!port2_has_اشاریہ && cur.ptr() == net2) || is_connected ) {
		  vvp_net_t*new_net = نیا vvp_net_t;

		    // Create new node with intermodpath and connect port2 to it
		  صحیح width = 1; // TODO
		  vvp_fun_intermodpath*obj = نیا vvp_fun_intermodpath(new_net, width);

		  vvp_net_t* output_net = net1;
		  اگر (is_connected) {
			اگر (is_output) {
			      output_net = پچھلا_گرہ.ptr(); // net2 is direct output of part_sa
			      cur = output_net->برآمد_2;
			      prev = vvp_net_ptr_t(خالی_اشاریہ, 0);
			} ورنہ {
			      prev = پچھلا_گرہ;
			      اگر (prev.ptr()) {
				    cur = prev.ptr()->پورٹ[prev.پورٹ()];
			      }
			}
		  }

		  new_net->fun = obj;
		  new_net->برآمد_2 = cur;

		    // Port2 is in the middle of the list
		    // Insert intermodpath before port2 and keep everything else intact
		  اگر (prev.ptr()) {
			prev.ptr()->پورٹ[prev.پورٹ()] = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->پورٹ[0] = cur.ptr()->پورٹ[cur.پورٹ()]; // Connect the next net in list
			cur.ptr()->پورٹ[cur.پورٹ()] = vvp_net_ptr_t(خالی_اشاریہ, 0); // Only port2 is connected to intermodpath
		    // Port2 is first in list
		    // Insert intermodpath before port2 and keep everything else intact
		  } ورنہ {
			output_net->برآمد_2 = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->پورٹ[0] = cur.ptr()->پورٹ[cur.پورٹ()]; //  Connect the next net in list
			cur.ptr()->پورٹ[cur.پورٹ()] = vvp_net_ptr_t(خالی_اشاریہ, 0); // Only port2 is connected to intermodpath
		  }

		    // If both ports are vpiOutput and port2 is not a vector,
		    // we have to reassign the __vpiSignal so that the delayed
		    // values get dumped
		  اگر (output_signal && !port2_has_اشاریہ) {
			net2->fil = net1->fil;
			net1->fil = خالی_اشاریہ;
			output_signal->گرہ = net2;
		  }

		    // Create the VPI intermodpath object
		  __vpiInterModPath* intermodpath = vpip_make_intermodpath(new_net, port1, port2);
		  intermodpath->intermodpath = obj;

		    // Finally done, return the intermodpath object
		  واپس intermodpath;
	    }

	    prev = cur;
	    cur = cur.ptr()->پورٹ[cur.پورٹ()]; // Next net in linked list
      }

      fprintf(stderr, "VPI error: Could not insert intermodpath!\n");
      fprintf(stderr, "\tport1 = %s, port1_has_index = %d, port1_bit_index = %d\n", port1_name.c_str(), port1_has_اشاریہ, port1_ثنائی_ہندسہ_اشاریہ);
      fprintf(stderr, "\tport2 = %s, port2_has_index = %d, port2_bit_index = %d\n", port2_name.c_str(), port2_has_اشاریہ, port2_ثنائی_ہندسہ_اشاریہ);
      واپس خالی_اشاریہ;
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



خالی vpi_get_delays(vpiHandle اظہاریہ, p_vpi_delay delays)
{
      أكد(اظہاریہ);
      أكد(delays);

      اظہاریہ->vpi_get_delays(delays);

      اگر (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_get_delays(%p, %p) -->\n", اظہاریہ, delays);
      }
}


خالی vpi_put_delays(vpiHandle اظہاریہ, p_vpi_delay delays)
{
      أكد(اظہاریہ  );
      أكد(delays );

      اظہاریہ->vpi_put_delays(delays);

      اگر (vpi_trace) {
	    fprintf(vpi_trace,
		    "vpi_put_delays(%p, %p) -->\n", اظہاریہ, delays);
      }
}








خارجی "C" PLI_INT32 vpi_vprintf(ثابت حرف*fmt, va_list ap)
{
      واپس vpi_mcd_vprintf(1, fmt, ap);
}

خارجی "C" PLI_INT32 vpi_printf(ثابت حرف *fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);
      صحیح r = vpi_mcd_vprintf(1, fmt, ap);
      va_end(ap);
      واپس r;
}

خارجی "C" PLI_INT32 vpi_flush(خالی)
{
      واپس vpi_mcd_flush(1);
}


خارجی "C" PLI_INT32 vpi_sim_vcontrol(صحیح operation, va_list ap)
{
      طویل diag_پیغام;

      انتخاب (operation) {
	  حالت vpi_اختتام:
	  حالت __ivl_legacy_vpiFinish:
            diag_پیغام = va_arg(ap, PLI_INT32);
	    جدول_وقت_اختتام(diag_پیغام);
	    توڑو;

	  حالت vpiStop:
	  حالت __ivl_legacy_vpiStop:
            diag_پیغام = va_arg(ap, PLI_INT32);
	    جدول_وقت_stop(diag_پیغام);
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "Unsupported VPI control operation %d.\n", operation);
	    واپس 0;
      }
      واپس 1;
}

خارجی "C" PLI_INT32 vpi_sim_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      صحیح r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      واپس r;
}

خارجی "C" PLI_INT32 vpi_control(PLI_INT32 operation, ...)
{
      va_list ap;
      va_start(ap, operation);
      صحیح r = vpi_sim_vcontrol(operation, ap);
      va_end(ap);
      واپس r;
}

/*
 * This routine calculated the return value for $clog2.
 * It is easier to do it here vs trying to to use the VPI interface.
 */
خارجی "C" s_vpi_vecval vpip_calc_clog2(vpiHandle arg)
{
      s_vpi_vecval rtn;
      s_vpi_قدر val;
      vvp_vector4_t vec4;
      منطقی is_neg = جھوٹ;  // At this point only a real can be negative.

	/* Get the value as a vvp_vector4_t. */
      val.format = vpiObjTypeVal;
      vpi_get_قدر(arg, &val);
      اگر (val.format == vpiRealVal) {
	    vpi_get_قدر(arg, &val);
	      /* All double values can be represented in 1024 bits. */
	    vec4 = vvp_vector4_t(1024, val.value.real);
	    اگر (val.value.real < 0) is_neg = سچ;
      } ورنہ {
	    val.format = vpi_سمتیہ_Val;
	    vpi_get_قدر(arg, &val);
	    غیرعلامتی wid = vpi_get(vpi_سائز, arg);
	    vec4 = vvp_vector4_t(wid, BIT4_0);
	    برائے (غیرعلامتی idx=0; idx < wid; idx += 1) {
		PLI_INT32 aval = val.value.vector[idx/32].aval;
		PLI_INT32 bval = val.value.vector[idx/32].bval;
		aval >>= idx % 32;
		bval >>= idx % 32;
		صحیح bitmask = (aval&1) | ((bval<<1)&2);
		vvp_bit4_t ثنائی_ہندسہ = scalar_مقصد_bit4(bitmask);
		vec4.سیٹ_ثنائی_ہندسہ(idx, ثنائی_ہندسہ);
	    }
      }

      اگر (vec4.has_xz()) {
	    rtn.aval = rtn.bval = 0xFFFFFFFFU;  /* Set to 'bx. */
	    واپس rtn;
      }

      vvp_vector2_t vec2(vec4);

      اگر (is_neg) vec2.trim_neg(); /* This is a special trim! */
      ورنہ vec2.trim(); /* This makes less work shifting. */

	/* Calculate the clog2 result. */
      PLI_INT32 res = 0;
      اگر (!vec2.is_zero()) {
	    vec2 -= vvp_vector2_t(1, vec2.size());
	    جب(!vec2.is_zero()) {
		  res += 1;
		  vec2 >>= 1;
	    }
      }

      rtn.aval = res;
      rtn.bval = 0;
      واپس rtn;
}

/*
 * This routine provides the information needed to implement $countdrivers.
 * It is done here for performance reasons - interrogating the drivers
 * individually via the VPI interface would be much slower.
 */
خارجی "C" خالی vpip_گنتی_drivers(vpiHandle ref, غیرعلامتی idx,
                                   غیرعلامتی counts[4])
{
      ساخت __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      أكد(rfp);
      rfp->گرہ->گنتی_drivers(idx, counts);
}

#اگر defined(__MINGW32__) || defined (__CYGWIN__)
vpip_routines_s vpi_routines = {
    .رجسٹر_cb                = vpi_رجسٹر_cb,
    .remove_cb                  = vpi_remove_cb,
    .رجسٹر_systf             = vpi_رجسٹر_systf,
    .get_systf_info             = vpi_get_systf_info,
    .handle_by_name             = vpi_handle_by_name,
    .handle_by_اشاریہ            = vpi_handle_by_اشاریہ,
    .handle_multi               = vpi_handle_multi,
    .handle                     = vpi_handle,
    .iterate                    = vpi_iterate,
    .جائزہ                       = vpi_جائزہ,
    .get                        = vpi_get,
    .get64                      = vpi_get64,
    .get_str                    = vpi_get_str,
    .get_delays                 = vpi_get_delays,
    .put_delays                 = vpi_put_delays,
    .get_قدر                  = vpi_get_قدر,
    .put_قدر                  = vpi_put_قدر,
    .get_وقت                   = vpi_get_وقت,
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
    .گنتی_drivers              = vpip_گنتی_drivers,
    .format_strength            = vpip_format_strength,
    .format_pretty              = vpip_format_pretty,
    .make_systf_system_defined  = vpip_make_systf_system_defined,
    .mcd_rawwrite               = vpip_mcd_rawwrite,
    .سیٹ_return_قدر           = vpip_سیٹ_return_قدر,
};
#اگر_ختم
