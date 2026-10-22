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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#पूर्ण_अगर
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
खाता*vpi_trace = 0;

अटल s_vpi_vlog_info  vpi_vlog_info;
अटल s_vpi_error_info vpip_last_error = { 0, 0, 0, 0, 0, 0, 0 };

स्थिर अक्षर* direction_as_string(पूर्णांक dir)
{
      चयन (dir) {
	  हाल vpi_निवेश:
	    वापस "input";
	  हाल vpiOutput:
	    वापस "output";
	  हाल vpiInout:
	    वापस "inout";
	  हाल vpiMixedIO:
	    वापस "mixed io";
	  हाल vpiNoDirection:
	    वापस "no direction";
	  शेष:
	    वापस "INVALID-DIRECTION";
      }
}

__vpiHandle::~__vpiHandle()
{ }

पूर्णांक __vpiHandle::vpi_get(पूर्णांक)
{ वापस vpiUndefined; }

int64_t __vpiHandle::vpi_get64(पूर्णांक code)
{ वापस (code == _vpiNexusId) ? 0 : vpiUndefined; }

अक्षर* __vpiHandle::vpi_get_str(पूर्णांक)
{ वापस 0; }

व्योम __vpiHandle::vpi_get_मूल्य(p_vpi_मूल्य)
{ }

vpiHandle __vpiHandle::vpi_put_मूल्य(p_vpi_मूल्य, पूर्णांक)
{ वापस 0; }

vpiHandle __vpiHandle::vpi_handle(पूर्णांक)
{ वापस 0; }

vpiHandle __vpiHandle::vpi_iterate(पूर्णांक)
{ वापस 0; }

vpiHandle __vpiHandle::vpi_सूचकांक(पूर्णांक)
{ वापस 0; }

व्योम __vpiHandle::vpi_get_delays(p_vpi_delay)
{ }

व्योम __vpiHandle::vpi_put_delays(p_vpi_delay)
{ }

__vpiBaseVar::__vpiBaseVar(__vpiScope*scope, स्थिर अक्षर*name, vvp_net_t*net)
: scope_(scope), name_(name), net_(net)
{
}

#अगर_घोषित CHECK_WITH_VALGRIND
__vpiBaseVar::~__vpiBaseVar()
{
      vvp_net_delete(net_);
}
#पूर्ण_अगर

/*
 * The default behavior for the vpi_free_object to an object is to
 * suppress the actual operation. This is because handles are
 * generally allocated semi-permanently within vvp context. Dynamic
 * objects will override the free_object_fun method to return an
 * appropriately effective function.
 */
अटल पूर्णांक suppress_free(vpiHandle)
{ वापस 1; }
__vpiHandle::free_object_fun_t __vpiHandle::free_object_fun(व्योम)
{ वापस &suppress_free; }

/*
 * The vpip_string function creates a constant string from the pass
 * input. This constant string is permanently allocated from an
 * efficient string buffer store.
 */
काष्ठा vpip_string_chunk {
      काष्ठा vpip_string_chunk*अग्र_2;
      अक्षर तथ्य[64*1024 - माप (काष्ठा vpip_string_chunk*)];
};

अचिन्हित vpip_आकार(स्थिर __vpiSignal *sig)
{
      वापस असल(sig->msb.get_मूल्य() - sig->lsb.get_मूल्य()) + 1;
}

__vpiScope* vpip_scope(__vpiSignal*sig)
{
      अगर (sig->is_netarray)
	    वापस static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      अन्यथा
	    वापस sig->within.scope;
}

__vpiScope* vpip_scope(__vpiRealVar*sig)
{
      अगर (sig->is_netarray)
	    वापस static_cast<__vpiScope*>(vpi_handle(vpiScope,
	                                               sig->within.parent));
      अन्यथा
	    वापस sig->within.scope;
}

vpiHandle vpip_module(__vpiScope*scope)
{
      जबतक(scope && scope->get_type_code() != vpiModule) {
	    scope = scope->scope;
      }
      निश्चित(scope);
      वापस scope;
}

स्थिर अक्षर *vpip_string(स्थिर अक्षर*str)
{
      अटल vpip_string_chunk first_chunk = {0, {0}};
      अटल vpip_string_chunk*chunk_list = &first_chunk;
      अटल अचिन्हित chunk_fill = 0;

      अचिन्हित len = म_माप(str);
      निश्चित( (len+1) <= माप chunk_list->तथ्य );

      अगर ( (len+1) > (माप chunk_list->तथ्य - chunk_fill) ) {
	    vpip_string_chunk*tmp = नया vpip_string_chunk;
	    tmp->अग्र_2 = chunk_list;
	    chunk_list = tmp;
	    chunk_fill = 0;
      }

      अक्षर*res = chunk_list->तथ्य + chunk_fill;
      chunk_fill += len + 1;

      म_नकल(res, str);
      वापस res;
}

अटल अचिन्हित hash_string(स्थिर अक्षर*text)
{
      अचिन्हित h = 0;

      जबतक (*text) {
	    h = (h << 4) ^ (h >> 28) ^ (अचिन्हित)*text;
	    text += 1;
      }
      वापस h;
}

स्थिर अक्षर* vpip_name_string(स्थिर अक्षर*text)
{
      स्थिर अचिन्हित HASH_आकार = 4096;
      अटल स्थिर अक्षर*hash_table[HASH_आकार] = {0};

      अचिन्हित hash_मूल्य = hash_string(text) % HASH_आकार;

	/* If we easily find the string in the hash table, then return
	   that and be done. */
      अगर (hash_table[hash_मूल्य]
	  && (म_भेद(hash_table[hash_मूल्य], text) == 0)) {
	    वापस hash_table[hash_मूल्य];
      }

	/* The existing hash entry is not a match. Replace it with the
	   newly allocated value, and return the new pointer as the
	   result to the add. */
      स्थिर अक्षर*res = vpip_string(text);
      hash_table[hash_मूल्य] = res;

      वापस res;
}

PLI_INT32 vpi_chk_error(p_vpi_error_info info)
{
      अगर (vpip_last_error.state == 0)
	    वापस 0;

      info->state = vpip_last_error.state;
      info->level = vpip_last_error.level;
      info->message = vpip_last_error.message;
      info->product = vpi_vlog_info.product;
      info->code = (अक्षर *)"";
      info->file = 0;
      info->पंक्ति = 0;

      वापस info->level;
}

PLI_INT32 vpi_compare_objects(vpiHandle obj1, vpiHandle obj2)
{
      निश्चित(obj1);
      निश्चित(obj2);

	// Does this actually work for all cases?
      अगर (obj1 != obj2) वापस 0;
      अन्यथा वापस 1;
}

/*
 * Copy the internal information to the data structure. Do not free or
 * change the tfname/user_data since they are a pointer to the real
 * string/data values. We also support passing a task or function handle
 * instead of just a handle to a vpiUserSystf.
 */
व्योम vpi_get_systf_info(vpiHandle ref, p_vpi_systf_तथ्य तथ्य)
{
      काष्ठा __vpiUserSystf* rfp = dynamic_cast<__vpiUserSystf*>(ref);
      अगर (rfp == 0) {
	    काष्ठा __vpiSysTaskCall*call = dynamic_cast<__vpiSysTaskCall*>(ref);
	    निश्चित(call);
	    rfp = call->defn;
      }

	/* Assert that vpiUserDefn is true! */
      निश्चित(rfp->is_user_defn);

      तथ्य->type = rfp->info.type;
      तथ्य->sysfunctype = rfp->info.sysfunctype;
      तथ्य->tfname = rfp->info.tfname;
      तथ्य->calltf = rfp->info.calltf;
      तथ्य->compiletf = rfp->info.compiletf;
      तथ्य->sizetf = rfp->info.sizetf;
      तथ्य->user_तथ्य = rfp->info.user_तथ्य;
}

/*
 * When a task is called, this value is set so that vpi_handle can
 * fathom the vpi_handle(vpiSysTfCall,0) function.
 */
काष्ठा __vpiSysTaskCall*vpip_cur_task = 0;

PLI_INT32 vpi_free_object(vpiHandle ref)
{
      पूर्णांक rtn;

      अगर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_free_object(%p)", ref);
	    ख_साफ(vpi_trace);
      }

      निश्चित(ref);
      __vpiHandle::free_object_fun_t fun = ref->free_object_fun();
      rtn = fun (ref);

      अगर (vpi_trace)
	    ख_लिखो(vpi_trace, " --> %d\n", rtn);

      वापस rtn;
}

PLI_INT32 vpi_release_handle(vpiHandle ref)
{
	// Since SystemVerilog vpi_free_object() has been
	// renamed vpi_release_handle(), and thus
	// vpi_free_object() has been deprecated.
      वापस vpi_free_object(ref);
}

अटल पूर्णांक vpip_get_global(पूर्णांक property)
{
      चयन (property) {

	  हाल vpiTimeUnit:
	  हाल vpiTimePrecision:
	    वापस vpip_get_time_precision();

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vpi error: bad global property: %d\n", property);
	    निश्चित(0);
	    वापस vpiUndefined;
      }
}

अटल स्थिर अक्षर* vpi_property_str(PLI_INT32 code)
{
      अटल अक्षर buf[32];
      चयन (code) {
	  हाल vpiConstType:
	    वापस "vpiConstType";
	  हाल vpiName:
	    वापस "vpiName";
	  हाल vpiFullName:
	    वापस "vpiFullName";
	  हाल vpiTimeUnit:
	    वापस "vpiTimeUnit";
	  हाल vpiTimePrecision:
	    वापस "vpiTimePrecision";
          हाल vpi_आकार:
	    वापस "vpiSize";
          हाल _vpiNexusId:
	    वापस "_vpiNexusId";
	  शेष:
	    snprintf(buf, माप(buf), "%d", (पूर्णांक)code);
      }
      वापस buf;
}

स्थिर अक्षर* vpi_type_as_string(PLI_INT32 code)
{
      अटल अक्षर buf[32];
      चयन (code) {
	  हाल vpiArrayType:
	    वापस "vpiArrayType";
	  हाल vpiBitVar:
	    वापस "vpiBitVar";
	  हाल vpiByteVar:
	    वापस "vpiByteVar";
	  हाल vpiClassVar:
	    वापस "vpiClassVar";
	  हाल vpiConstant:
	    वापस "vpiConstant";
	  हाल vpiEnumTypespec:
	    वापस "vpiEnumTypespec";
	  हाल vpiFunction:
	    वापस "vpiFunction";
	  हाल vpiGenScope:
	    वापस "vpiGenScope";
	  हाल vpiIntVar:
	    वापस "vpiIntVar";
	  हाल vpiIntegerVar:
	    वापस "vpiIntegerVar";
	  हाल vpiIterator:
	    वापस "vpiIterator";
	  हाल vpiLongIntVar:
	    वापस "vpiLongIntVar";
	  हाल vpiMemory:
	    वापस "vpiMemory";
	  हाल vpiMemoryWord:
	    वापस "vpiMemoryWord";
	  हाल vpiModule:
	    वापस "vpiModule";
	  हाल vpiNamedBegin:
	    वापस "vpiNamedBegin";
	  हाल vpiNamedEvent:
	    वापस "vpiNamedEvent";
	  हाल vpiNamedFork:
	    वापस "vpiNamedFork";
	  हाल vpiPackage:
	    वापस "vpiPackage";
	  हाल vpiPathTerm:
	    वापस "vpiPathTerm";
	  हाल vpiPort:
	    वापस "vpiPort";
	  हाल vpiNet:
	    वापस "vpiNet";
	  हाल vpiNetArray:
	    वापस "vpiNetArray";
	  हाल vpiNetBit:
	    वापस "vpiNetBit";
	  हाल vpiParameter:
	    वापस "vpiParameter";
	  हाल vpiPartSelect:
	    वापस "vpiPartSelect";
	  हाल vpiRealVar:
	    वापस "vpiRealVar";
	  हाल vpiReg:
	    वापस "vpiReg";
	  हाल vpiRegBit:
	    वापस "vpiRegBit";
	  हाल vpiShortIntVar:
	    वापस "vpiShortIntVar";
	  हाल vpiStringVar:
	    वापस "vpiStringVar";
	  हाल vpiSysFuncCall:
	    वापस "vpiSysFuncCall";
	  हाल vpiSysTaskCall:
	    वापस "vpiSysTaskCall";
	  हाल vpiTask:
	    वापस "vpiTask";
	  हाल vpiTimeVar:
	    वापस "vpiTimeVar";
	  हाल vpiUserSystf:
	    वापस "vpiUserSystf";
	  शेष:
	    snprintf(buf, माप(buf), "%d", (पूर्णांक)code);
      }
      वापस buf;
}

PLI_INT32 vpi_get(पूर्णांक property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      अगर (property == _vpiDelaySelection) वापस vpip_delay_selection;

      अगर (ref == 0)
	    वापस vpip_get_global(property);

      अगर (property == vpiType) {
	    अगर (vpi_trace) {
		  ख_लिखो(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

	    अगर (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  वापस vpiNetArray;
	    अन्यथा
		  वापस ref->get_type_code();
      }

      पूर्णांक res = ref->vpi_get(property);

      अगर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_get(%s, %p) --> %d\n",
		    vpi_property_str(property), ref, res);
      }

      वापस res;
}

PLI_INT64 vpi_get64(पूर्णांक property, vpiHandle ref)
{
	// For now we only support getting the nexus id.
      अगर ((ref == 0) || (property != _vpiNexusId)) {
	    ख_लिखो(vpi_trace, "vpi_get64(%s, %p) is not currently supported.\n",
		    vpi_property_str(property), ref);
	    वापस vpiUndefined;
      }

      int64_t res = ref->vpi_get64(property);

      अगर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_get64(%s, %p) --> %" PRId64 "\n",
		    vpi_property_str(property), ref, res);
      }

      वापस res;
}

अक्षर* vpi_get_str(PLI_INT32 property, vpiHandle ref)
{
	/* We don't care what the ref is there is only one delay selection. */
      अगर (property == _vpiDelaySelection) {
	    चयन (vpip_delay_selection) {
		हाल _vpiDelaySelMinimum:
		  वापस simple_set_rbuf_str("MINIMUM");
		हाल _vpiDelaySelTypical:
		  वापस simple_set_rbuf_str("TYPICAL");
		हाल _vpiDelaySelMaximum:
		  वापस simple_set_rbuf_str("MAXIMUM");
		शेष:
		  निश्चित(0);
	    }
      }

      अगर (ref == 0) {
	    ख_लिखो(मानक_त्रुटि, "vpi error: vpi_get_str(%s, 0) called "
		    "with null vpiHandle.\n", vpi_property_str(property));
	    वापस 0;
      }

      अगर (property == vpiType) {
	    अगर (vpi_trace) {
		  ख_लिखो(vpi_trace, "vpi_get(vpiType, %p) --> %s\n",
			  ref, vpi_type_as_string(ref->get_type_code()));
	    }

            PLI_INT32 type;
	    अगर (ref->get_type_code() == vpiMemory && is_net_array(ref))
		  type = vpiNetArray;
	    अन्यथा
		  type = ref->get_type_code();
	    वापस (अक्षर *)vpi_type_as_string(type);
      }

      अक्षर*res = ref->vpi_get_str(property);

      अगर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_get_str(%s, %p) --> %s\n",
		    vpi_property_str(property), ref, res? res : "<NULL>");
      }

      वापस res;
}

__vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj)
{
      स्थिर काष्ठा __vpiSysTaskCall*task;
      काष्ठा __vpiSignal*संकेत;
      काष्ठा __vpiRealVar*real;
      स्थिर __vpiNamedEvent*event;

      चयन (obj->get_type_code()) {
	  हाल vpiSysTaskCall:
	    task = dynamic_cast<__vpiSysTaskCall*>(obj);
	    वापस task->scope;

	  हाल vpiModule:
	    वापस dynamic_cast<__vpiScope*>(obj);

	  हाल vpiNet:
	  हाल vpiReg:
	  हाल vpiIntegerVar:
	  हाल vpiBitVar:
	  हाल vpiByteVar:
	  हाल vpiShortIntVar:
	  हाल vpiIntVar:
	  हाल vpiLongIntVar:
	    संकेत = dynamic_cast<__vpiSignal*>(obj);
	    वापस vpip_scope(संकेत);

	  हाल vpiRealVar:
	    real = dynamic_cast<__vpiRealVar*>(obj);
	    वापस vpip_scope(real);

	  हाल vpiNamedEvent:
	    event = dynamic_cast<__vpiNamedEvent*>(obj);
	    वापस event->get_scope();

	  हाल vpiMemory:
	  हाल vpiMemoryWord:
	  हाल vpiPartSelect:
	    वापस dynamic_cast<__vpiScope*>(obj->vpi_handle(vpiScope));

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "ERROR: vpip_scope_from_handle called with "
		    "object handle type=%d\n", obj->get_type_code());
	    निश्चित(0);
	    वापस 0;
      }
}

पूर्णांक vpip_time_units_from_handle(vpiHandle obj)
{
      अगर (obj == 0)
	    वापस vpip_get_time_precision();

      स्थिर __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      अगर (scope == 0)
	    वापस vpip_get_time_precision();

      वापस scope->time_units;
}

पूर्णांक vpip_time_precision_from_handle(vpiHandle obj)
{
      अगर (obj == 0)
	    वापस vpip_get_time_precision();

      स्थिर __vpiScope*scope = vpip_timescale_scope_from_handle(obj);
      अगर (scope == 0)
	    वापस vpip_get_time_precision();

      वापस scope->time_precision;
}

द्विगुन vpip_scaled_time_from_handle(vvp_time64_t समय, vpiHandle obj)
{
      पूर्णांक scale = vpip_get_time_precision() -
	          vpip_time_units_from_handle(obj);
      अगर (scale >= 0)
	    वापस (द्विगुन)समय * घात(10.0, scale);
      अन्यथा
	    वापस (द्विगुन)समय / घात(10.0, -scale);
}

व्योम vpi_get_time(vpiHandle obj, s_vpi_time*vp)
{
      vvp_time64_t समय;

      निश्चित(vp);

      समय = schedule_simtime();

      चयन (vp->type) {
          हाल vpiSimTime:
	    vp->ऊपरी = (समय >> 32) & 0xffffffff;
	    vp->निचला = समय & 0xffffffff;
	    अवरोध;

          हाल vpiScaledRealTime:
	    vp->real = vpip_scaled_time_from_handle(समय, obj);
	    अवरोध;

          शेष:
            ख_लिखो(मानक_त्रुटि, "vpi_get_time: unknown type: %d\n", (पूर्णांक)vp->type);
            निश्चित(0);
	    अवरोध;
      }
}

PLI_INT32 vpi_get_vlog_info(p_vpi_vlog_info vlog_info_p)
{
    अगर (vlog_info_p != 0) {
	  *vlog_info_p = vpi_vlog_info;
	  वापस 1;
    } अन्यथा {
	  वापस 0;
    }
}

व्योम vpi_set_vlog_info(पूर्णांक argc, अक्षर** argv)
{
    अटल अक्षर icarus_product[] = "Icarus Verilog";
    अटल अक्षर icarus_version[] = VERSION;
    vpi_vlog_info.product = icarus_product;
    vpi_vlog_info.version = icarus_version;
    vpi_vlog_info.argc    = argc;
    vpi_vlog_info.argv    = argv;

    अटल अक्षर trace_buf[1024];
    अगर (स्थिर अक्षर*path = दो_पर्या("VPI_TRACE"))  {
	  अगर (!म_भेद(path,"-"))
		vpi_trace = मानक_निकास;
	  अन्यथा {
		vpi_trace = ख_खोलो(path, "w");
		अगर (!vpi_trace) {
		      लिखो_त्रुटि(path);
		      निकास(1);
		}
		रखो_भबफ(vpi_trace, trace_buf, _IOLBF, माप(trace_buf));
	  }
    }
}

अटल व्योम vec4_get_मूल्य_string(स्थिर vvp_vector4_t&word_val, अचिन्हित width,
				  s_vpi_मूल्य*vp)
{
      अचिन्हित nchar = width / 8;
      अचिन्हित पुच्छ = width % 8;

      अक्षर*rbuf = static_cast<अक्षर *>(need_परिणाम_buf(nchar + 1, RBUF_VAL));
      अक्षर*cp = rbuf;

      अगर (पुच्छ > 0) {
	    अक्षर char_val = 0;
	    क्रम (अचिन्हित idx = width-पुच्छ; idx < width ;  idx += 1) {
		  vvp_bit4_t val = word_val.value(idx);
		  अगर (val == BIT4_1)
			char_val |= 1 << idx;
	    }

	    अगर (char_val != 0)
		  *cp++ = char_val;
	    अन्यथा अगर (cp != rbuf)
		  *cp++ = ' ';
      }

      क्रम (अचिन्हित idx = 0 ;  idx < nchar ;  idx += 1) {
	    अचिन्हित bit = (nchar - idx - 1) * 8;
	    अक्षर char_val = 0;
	    क्रम (अचिन्हित bdx = 0 ;  bdx < 8 ;  bdx += 1) {
		  vvp_bit4_t val = word_val.value(bit+bdx);
		  अगर (val == BIT4_1)
			char_val |= 1 << bdx;
	    }
	    // Ignore leading null-bytes and replace other null-bytes with space.
	    // The LRM is not entirely clear on how null bytes should be handled.
	    // This is the implementation chosen for iverilog.
	    अगर (char_val != 0)
		  *cp++ = char_val;
	    अन्यथा अगर (cp != rbuf)
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
व्योम vpip_vec4_get_मूल्य(स्थिर vvp_vector4_t&word_val, अचिन्हित width,
			 bool signed_flag, s_vpi_मूल्य*vp)
{
      अक्षर *rbuf = 0;

      चयन (vp->format) {
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "sorry: Format %d not implemented for "
	                    "getting vector values.\n", (पूर्णांक)vp->format);
	    निश्चित(0);

	  हाल vpiSuppressVal:
	    अवरोध;

	  हाल vpiBinStrVal:
	    rbuf = static_cast<अक्षर *>(need_परिणाम_buf(width+1, RBUF_VAL));
	    क्रम (अचिन्हित idx = 0 ;  idx < width ;  idx += 1) {
		  vvp_bit4_t bit = word_val.value(idx);
		  rbuf[width-idx-1] = vvp_bit4_to_ascii(bit);
	    }
	    rbuf[width] = 0;
	    vp->value.str = rbuf;
	    अवरोध;

	  हाल vpiOctStrVal: {
		अचिन्हित hwid = ((width+2) / 3) + 1;
		rbuf = static_cast<अक्षर *>(need_परिणाम_buf(hwid, RBUF_VAL));
		vpip_vec4_to_oct_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		अवरोध;
	  }

	  हाल vpiDecStrVal: {
// HERE need a better estimate.
		rbuf = static_cast<अक्षर *>(need_परिणाम_buf(width+1, RBUF_VAL));
		vpip_vec4_to_dec_str(word_val, rbuf, width+1, signed_flag);
		vp->value.str = rbuf;
		अवरोध;
	  }

	  हाल vpiHexStrVal: {
		अचिन्हित  hwid = ((width + 3) / 4) + 1;
		rbuf = static_cast<अक्षर *>(need_परिणाम_buf(hwid, RBUF_VAL));
		vpip_vec4_to_hex_str(word_val, rbuf, hwid);
		vp->value.str = rbuf;
		अवरोध;
	  }

          हाल vpiScalarVal: {
	        // scalars should be of size 1
	        निश्चित(width == 1);
	        चयन(word_val.value(0)) {
		    हाल BIT4_0:
		      vp->value.scalar = vpi0;
		      अवरोध;
	            हाल BIT4_1:
                      vp->value.scalar = vpi1;
                      अवरोध;
	            हाल BIT4_X:
                      vp->value.scalar = vpiX;
                      अवरोध;
	            हाल BIT4_Z:
                      vp->value.scalar = vpiZ;
                      अवरोध;
		}
                अवरोध;
          }

	  हाल vpiIntVal: {
		दीर्घ val = 0;
		vvp_bit4_t pad = BIT4_0;
		अगर (signed_flag && word_val.size() > 0)
		      pad = word_val.value(word_val.size()-1);

		क्रम (अचिन्हित idx = 0 ; idx < 8*माप(val) ;  idx += 1) {
		      vvp_bit4_t val4 = pad;
		      अगर (idx < word_val.size())
			    val4 = word_val.value(idx);
		      अगर (val4 == BIT4_1)
			    val |= 1L << idx;
		}

		vp->value.integer = val;
		अवरोध;
	  }

	  हाल vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiVectorVal;
	    // fallthrough
	  हाल vpiVectorVal: {
		अचिन्हित hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_परिणाम_buf(hwid * माप(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		क्रम (अचिन्हित idx = 0 ;  idx < width ;  idx += 1) {
		      चयन (word_val.value(idx)) {
			  हाल BIT4_0:
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    अवरोध;
			  हाल BIT4_1:
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
			    अवरोध;
			  हाल BIT4_X:
			    op->aval |= (1 << idx % 32);
			    op->bval |= (1 << idx % 32);
			    अवरोध;
			  हाल BIT4_Z:
			    op->aval &= ~(1 << idx % 32);
			    op->bval |=  (1 << idx % 32);
			    अवरोध;
		      }
		      अगर (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		अवरोध;
	  }

	  हाल vpiStringVal:
		vec4_get_मूल्य_string(word_val, width, vp);
		अवरोध;

	  हाल vpiRealVal:
		vector4_to_मूल्य(word_val, vp->value.real, signed_flag);
		अवरोध;
      }
}

व्योम vpip_vec2_get_मूल्य(स्थिर vvp_vector2_t&word_val, अचिन्हित width,
			 bool signed_flag, s_vpi_मूल्य*vp)
{
      चयन (vp->format) {
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "sorry: Format %d not implemented for "
	                    "getting vector2 values.\n", (पूर्णांक)vp->format);
	    निश्चित(0);

	  हाल vpiSuppressVal:
	    अवरोध;

	  हाल vpiIntVal:
	    vector2_to_मूल्य(word_val, vp->value.integer, signed_flag);
	    अवरोध;

	  हाल vpiObjTypeVal:
	    vp->format = vpiVectorVal;
	    // fallthrough
	  हाल vpiVectorVal: {
		अचिन्हित hwid = (width + 31)/32;

		s_vpi_vecval *op = static_cast<p_vpi_vecval>
		                   (need_परिणाम_buf(hwid * माप(s_vpi_vecval), RBUF_VAL));
		vp->value.vector = op;

		op->aval = op->bval = 0;
		क्रम (अचिन्हित idx = 0 ;  idx < width ;  idx += 1) {
		      अगर (word_val.value(idx)) {
			    op->aval |=  (1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      } अन्यथा {
			    op->aval &= ~(1 << idx % 32);
			    op->bval &= ~(1 << idx % 32);
		      }
		      अगर (!((idx+1) % 32) && (idx+1 < width)) {
			    op++;
			    op->aval = op->bval = 0;
		      }
		}
		अवरोध;
	  }
      }

}

/*
 * Convert a real value to the appropriate integer.
 */
अटल PLI_INT32 get_real_as_int(द्विगुन real)
{
      द्विगुन rtn;

	/* We would normally want to return 'bx for a NaN or
	 * +/- infinity, but for an integer the standard says
	 * to convert 'bx to 0, so we just return 0. */
      अगर (real != real || (real && (real == 0.5*real))) {
	    वापस 0;
      }

      rtn = std::round(real);

      वापस (PLI_INT32) rtn;
}

/*
 * This is a generic function to convert a double value into a
 * vpi_value structure. The format is selected by the format of the
 * value pointer.
 */
व्योम vpip_real_get_मूल्य(द्विगुन real, s_vpi_मूल्य*vp)
{
      अक्षर *rbuf = 0;

      चयन (vp->format) {
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "sorry: Format %d not implemented for "
	                    "getting real values.\n", (पूर्णांक)vp->format);
	    निश्चित(0);

	  हाल vpiSuppressVal:
	    अवरोध;

	  हाल vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiRealVal;
	    // fallthrough
	  हाल vpiRealVal:
	    vp->value.real = real;
	    अवरोध;

	  हाल vpiIntVal:
	    vp->value.integer = get_real_as_int(real);
	    अवरोध;

	  हाल vpiDecStrVal:
	    rbuf = static_cast<अक्षर *>(need_परिणाम_buf(1025, RBUF_VAL));
	    vpip_vec4_to_dec_str(vvp_vector4_t(1024, real), rbuf, 1025, true);
	    vp->value.str = rbuf;
	    अवरोध;
      }
}

द्विगुन real_from_vpi_मूल्य(s_vpi_मूल्य*vp)
{
      vvp_vector4_t vec4(1024);
      द्विगुन परिणाम;
      bool is_signed = false;

      चयन (vp->format) {
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "sorry: Format %d not implemented for "
	                    "putting real values.\n", (पूर्णांक)vp->format);
	    निश्चित(0);

	  हाल vpiRealVal:
	    परिणाम = vp->value.real;
	    अवरोध;

	  हाल vpiIntVal:
	    परिणाम = (द्विगुन) vp->value.integer;
	    अवरोध;

	  हाल vpiBinStrVal:
	    vpip_bin_str_to_vec4(vec4, vp->value.str);
	    अगर (vp->value.str[0] == '-') is_signed = true;
	    vector4_to_मूल्य(vec4, परिणाम, is_signed);
	    अवरोध;

	  हाल vpiOctStrVal:
	    vpip_oct_str_to_vec4(vec4, vp->value.str);
	    अगर (vp->value.str[0] == '-') is_signed = true;
	    vector4_to_मूल्य(vec4, परिणाम, is_signed);
	    अवरोध;

	  हाल vpiDecStrVal:
	    vpip_dec_str_to_vec4(vec4, vp->value.str);
	    अगर (vp->value.str[0] == '-') is_signed = true;
	    vector4_to_मूल्य(vec4, परिणाम, is_signed);
	    अवरोध;

	  हाल vpiHexStrVal:
	    vpip_hex_str_to_vec4(vec4, vp->value.str);
	    अगर (vp->value.str[0] == '-') is_signed = true;
	    vector4_to_मूल्य(vec4, परिणाम, is_signed);
	    अवरोध;

      }

      वापस परिणाम;
}

व्योम vpip_string_get_मूल्य(स्थिर string&val, s_vpi_मूल्य*vp)
{
      अक्षर *rbuf = 0;

      चयन (vp->format) {
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "sorry: Format %d not implemented for "
	                    "getting string values.\n", (पूर्णांक)vp->format);
	    निश्चित(0);

	  हाल vpiSuppressVal:
	    अवरोध;

	  हाल vpiObjTypeVal:
	    // Use the following case to actually set the value!
	    vp->format = vpiStringVal;
	    // fallthrough
	  हाल vpiStringVal:
	    rbuf = static_cast<अक्षर *>(need_परिणाम_buf(val.size() + 1, RBUF_VAL));
	    म_नकल(rbuf, val.c_str());
	    vp->value.str = rbuf;
	    अवरोध;
      }
}


व्योम vpi_get_मूल्य(vpiHandle व्यंजक, s_vpi_मूल्य*vp)
{
      निश्चित(व्यंजक);
      निश्चित(vp);

	// Never bother with suppressed values. All the derived
	// classes can ignore this type.
      अगर (vp->format == vpiSuppressVal)
	    वापस;

      व्यंजक->vpi_get_मूल्य(vp);

      अगर (vpi_trace) चयन (vp->format) {
		हाल vpiStringVal:
		  ख_लिखो(vpi_trace,"vpi_get_value(%p=<%d>) -> string=\"%s\"\n",
			  व्यंजक, व्यंजक->get_type_code(), vp->value.str);
		  अवरोध;

		हाल vpiBinStrVal:
		  ख_लिखो(vpi_trace, "vpi_get_value(<%d>...) -> binstr=%s\n",
			  व्यंजक->get_type_code(), vp->value.str);
		  अवरोध;

		हाल vpiIntVal:
		  ख_लिखो(vpi_trace, "vpi_get_value(<%d>...) -> int=%d\n",
			  व्यंजक->get_type_code(), (पूर्णांक)vp->value.integer);
		  अवरोध;

		हाल vpiSuppressVal:
		  ख_लिखो(vpi_trace, "vpi_get_value(<%d>...) -> <suppress>\n",
			  व्यंजक->get_type_code());
		  अवरोध;

		शेष:
		  ख_लिखो(vpi_trace, "vpi_get_value(<%d>...) -> <%d>=?\n",
			  व्यंजक->get_type_code(), (पूर्णांक)vp->format);
	    }
}

काष्ठा vpip_put_मूल्य_event : vvp_gen_event_s {
      vpiHandle handle;
      s_vpi_मूल्य value;
      पूर्णांक flags;
      भव व्योम run_run() override;
      ~vpip_put_मूल्य_event() override { }
};

व्योम vpip_put_मूल्य_event::run_run()
{
      handle->vpi_put_मूल्य(&value, flags);
      चयन (value.format) {
	    /* Free the copied string. */
	  हाल vpiBinStrVal:
	  हाल vpiOctStrVal:
	  हाल vpiDecStrVal:
	  हाल vpiHexStrVal:
	  हाल vpiStringVal:
	    मुक्त(value.value.str);
	    अवरोध;
	    /* Free the copied time structure. */
	  हाल vpiTimeVal:
	    मुक्त(value.value.समय);
	    अवरोध;
	    /* Free the copied vector structure. */
	  हाल vpiVectorVal:
	    मुक्त(value.value.vector);
	    अवरोध;
	    /* Free the copied strength structure. */
	  हाल vpiStrengthVal:
	    मुक्त(value.value.strength);
	    अवरोध;
	    /* Everything else is static in the structure. */
	  शेष:
	    अवरोध;
      }
}

/* Make a copy of a pointer to a time structure. */
अटल t_vpi_time *timedup(स्थिर t_vpi_time *val)
{
      t_vpi_time *rtn;
      rtn = static_cast<t_vpi_time *> (दो_स्मृति(माप(t_vpi_time)));
      *rtn = *val;
      वापस rtn;
}

/* Make a copy of a pointer to a vector value structure. */
अटल t_vpi_vecval *vectordup(स्थिर t_vpi_vecval *val, PLI_INT32 size)
{
      अचिन्हित अंक_bytes;
      t_vpi_vecval *rtn;
      निश्चित(size > 0);
      अंक_bytes = ((size + 31)/32)*माप(t_vpi_vecval);
      rtn = static_cast<t_vpi_vecval *> (दो_स्मृति(अंक_bytes));
      स_नकल(rtn, val, अंक_bytes);
      वापस rtn;
}

/* Make a copy of a pointer to a strength structure. */
अटल t_vpi_strengthval *strengthdup(स्थिर t_vpi_strengthval *val)
{
      t_vpi_strengthval *rtn;
      rtn = static_cast<t_vpi_strengthval *>
            (दो_स्मृति(माप(t_vpi_strengthval)));
      *rtn = *val;
      वापस rtn;
}

vpiHandle vpi_put_मूल्य(vpiHandle obj, s_vpi_मूल्य*vp,
			s_vpi_time*when, PLI_INT32 flags)
{
      निश्चित(obj);

      flags &= ~vpiReturnEvent;

      अगर (flags!=vpiNoDelay && flags!=vpiForceFlag && flags!=vpiReleaseFlag) {
	    vvp_time64_t dly;
	    पूर्णांक scale;

	    अगर (vpi_get(vpiAutomatic, obj)) {
		  ख_लिखो(मानक_त्रुटि, "VPI error: cannot put a value with "
				  "a delay on automatically allocated "
				  "variable '%s'.\n",
				  vpi_get_str(vpiName, obj));
		  वापस 0;
	    }

	    निश्चित(when != 0);

	    चयन (when->type) {
		हाल vpiScaledRealTime:
		  scale = vpip_time_units_from_handle(obj) -
		          vpip_get_time_precision();
		  अगर (scale >= 0) {
			dly = (vvp_time64_t)(when->real * घात(10.0, scale));
		  } अन्यथा {
			dly = (vvp_time64_t)(when->real / घात(10.0, -scale));
		  }
		  अवरोध;
		हाल vpiSimTime:
		  dly = vpip_timestruct_to_time(when);
		  अवरोध;
		शेष:
		  dly = 0;
		  अवरोध;
	    }

	    अगर ((dly == 0) && schedule_at_rosync()) {
		  ख_लिखो(मानक_त्रुटि, "VPI error: attempted to put a value to "
				  "variable '%s' during a read-only synch "
				  "callback.\n", vpi_get_str(vpiName, obj));
		  वापस 0;
	    }

	    vpip_put_मूल्य_event*दो = नया vpip_put_मूल्य_event;
	    दो->handle = obj;
	    अगर (dynamic_cast<__vpiNamedEvent*>(obj)) {
		  दो->value.format = vpiIntVal;
		  दो->value.value.integer = 0;
	    } अन्यथा {
		  निश्चित(vp);
		  दो->value = *vp;
	    }
	      /* Since this is a scheduled put event we must copy any pointer
	       * data to keep it available until the event is actually run. */
	    चयन (दो->value.format) {
		  /* Copy the string items. */
		हाल vpiBinStrVal:
		हाल vpiOctStrVal:
		हाल vpiDecStrVal:
		हाल vpiHexStrVal:
		हाल vpiStringVal:
		  दो->value.value.str = strdup(दो->value.value.str);
		  अवरोध;
		  /* Copy a time pointer item. */
		हाल vpiTimeVal:
		  दो->value.value.समय = timedup(दो->value.value.समय);
		  अवरोध;
		  /* Copy a vector pointer item. */
		हाल vpiVectorVal:
		  दो->value.value.vector = vectordup(दो->value.value.vector,
		                                      vpi_get(vpi_आकार, obj));
		  अवरोध;
		  /* Copy a strength pointer item. */
		हाल vpiStrengthVal:
		  दो->value.value.strength =
		      strengthdup(दो->value.value.strength);
		  अवरोध;
		  /* Everything thing else is already in the structure. */
		शेष:
		  अवरोध;
	    }
	    दो->flags = flags;
	    schedule_generic(दो, dly, false, true, true);
	    वापस 0;
      }

      अगर (schedule_at_rosync()) {
            ख_लिखो(मानक_त्रुटि, "VPI error: attempted to put a value to "
			    "variable '%s' during a read-only synch "
			    "callback.\n", vpi_get_str(vpiName, obj));
            वापस 0;
      }

      obj->vpi_put_मूल्य(vp, flags);

      वापस 0;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle res = 0;

      अगर (ref == 0) {
	      // A few types can apply to a nil handle. These are ways
	      // that the current function can get started finding things.
	    चयन (type) {

		हाल vpiScope:
		    // The IEEE1364-2005 doesn't seem to allow this,
		    // but some users seem to think it's handy, so
		    // return the scope that contains this SysTfCall.
		  निश्चित(vpip_cur_task);
		  res = vpip_cur_task->vpi_handle(vpiScope);
		  अवरोध;

		हाल vpiSysTfCall:
		    // This is how VPI users get a first handle into
		    // the system. This is the handle of the system
		    // task/function call currently being executed.
		  अगर (vpi_trace) {
			ख_लिखो(vpi_trace, "vpi_handle(vpiSysTfCall, 0) "
				"-> %p (%s)\n", vpip_cur_task,
				vpip_cur_task->defn->info.tfname);
		  }
		  वापस vpip_cur_task;

		शेष:
		  ख_लिखो(मानक_त्रुटि, "VPI error: vpi_handle(type=%d, ref=0).\n",
			  (पूर्णांक)type);
		  res = 0;
		  अवरोध;
	    }

      } अन्यथा {

	    अगर (type == vpiSysTfCall) {
		  ख_लिखो(मानक_त्रुटि, "VPI error: vpi_handle(vpiSysTfCall, "
			  "ref!=0).\n");
		  वापस 0;
	    }

	    res = ref->vpi_handle(type);
      }


      अगर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_handle(vpiScope, ref=%p) "
		    "-> %p\n", vpip_cur_task, ref);
      }

      वापस res;
}

अटल vpiHandle vpip_make_udp_iterator()
{
// HERE: Add support for iterating over UDP definitions.
//       See 26.6.16 (page 400 in 1364-2005).
      वापस 0;
}

/*
 * This function asks the object to return an iterator for
 * the specified reference. It is up to the iterate_ method to
 * allocate a properly formed iterator.
 */
अटल vpiHandle vpi_iterate_global(पूर्णांक type)
{
      चयन (type) {
	  हाल vpiInstance:
	    // fallthrough
	  हाल vpiModule:
	    // fallthrough
	  हाल vpiProgram:
	    // fallthrough
	  हाल vpiInterface:
	    // fallthrough
	  हाल vpiPackage:
	    वापस vpip_make_मूल_iterator(type);

	  हाल vpiUdpDefn:
	    वापस vpip_make_udp_iterator();

	  हाल vpiUserSystf:
	    वापस vpip_make_systf_iterator();
      }

      वापस 0;
}

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle ref)
{
      vpiHandle rtn = 0;

      निश्चित(vpi_mode_flag != VPI_MODE_NONE);
      अगर (vpi_mode_flag == VPI_MODE_REGISTER) {
	    ख_लिखो(मानक_त्रुटि, "vpi error: vpi_iterate called during "
		    "vpi_register_systf. You can't do that!\n");
	    वापस 0;
      }

      अगर (ref == 0)
	    rtn = vpi_iterate_global(type);
      अन्यथा
	    rtn = ref->vpi_iterate(type);

      अगर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_iterate(%d, %p) ->%s\n",
	    (पूर्णांक)type, ref, rtn ? "" : " (null)");
      }

      वापस rtn;
}

vpiHandle vpi_handle_by_सूचकांक(vpiHandle ref, PLI_INT32 idx)
{
      निश्चित(ref);
      वापस ref->vpi_सूचकांक(idx);
}

अटल vpiHandle find_name(स्थिर अक्षर *name, vpiHandle handle)
{
      vpiHandle rtn = 0;
      __vpiScope*ref = dynamic_cast<__vpiScope*>(handle);

      /* check module names */
      अगर (!म_भेद(name, vpi_get_str(vpiName, handle)))
	    rtn = handle;

      /* brute force search for the name in all objects in this scope */
      क्रम (अचिन्हित i = 0 ;  i < ref->intern.size() ;  i += 1) {
	      /* The standard says that since a port does not have a full
	       * name it cannot be found by name. Because of this we need
	       * to skip ports here so the correct handle can be located. */
	    अगर (vpi_get(vpiType, ref->intern[i]) == vpiPort) जारी;
	    स्थिर अक्षर *nm = vpi_get_str(vpiName, ref->intern[i]);
	    अगर (nm && !म_भेद(name, nm)) {
		  rtn = ref->intern[i];
		  अवरोध;
	    } अन्यथा अगर (vpi_get(vpiType, ref->intern[i]) == vpiMemory ||
	               vpi_get(vpiType, ref->intern[i]) == vpiNetArray) {
		  /* We need to iterate on the words */
		  vpiHandle word_i, word_h;
		  word_i = vpi_iterate(vpiMemoryWord, ref->intern[i]);
		  जबतक (word_i && (word_h = vpi_scan(word_i))) {
			nm = vpi_get_str(vpiName, word_h);
			अगर (nm && !म_भेद(name, nm)) {
			      rtn = word_h;
			      vpi_free_object(word_i);
			      अवरोध;
			}
		  }
	    }
	    /* found it yet? */
	    अगर (rtn) अवरोध;
      }

      वापस rtn;
}

// Find the end of the escaped identifier or simple identifier
अटल अक्षर * find_rest(अक्षर *name)
{
      अक्षर *rest;
      अगर (*name == '\\') {
	    rest = म_अक्षर(name, ' ');
	    अगर (rest) {
		  *rest++ = 0; // The space is not part of the string
		    // There should be a '.' after the escaped ID if there is
		    // anything more. If it is missing add it to avoid a crash
		  अगर ((*rest != '.') && (*rest != 0)) {
			*--rest = '.';
			ख_लिखो(मानक_त्रुटि, "ERROR: Malformed scope string: \"%s\"", name);
		  }
		  अगर (*rest == 0) {
			rest = 0;
		  }
	    }
      } अन्यथा {
	    rest = म_अक्षर(name, '.');
      }
      वापस rest;
}

अटल vpiHandle find_scope(स्थिर अक्षर *name, vpiHandle handle, पूर्णांक depth)
{

      vpiHandle iter = handle==0
	    ? vpi_iterate(vpiModule, शून्य)
	    : vpi_iterate(vpiInternalScope, handle);

      vector<अक्षर> name_buf (म_माप(name)+1);
      म_नकल(&name_buf[0], name);
      अक्षर*nm_first = &name_buf[0];
      अक्षर*nm_rest = find_rest(nm_first);
      अगर (*nm_first == '\\') ++nm_first;
      अगर (nm_rest) {
	    *nm_rest++ = 0;
      }

      vpiHandle rtn = 0;
      vpiHandle hand;
      जबतक (iter && (hand = vpi_scan(iter))) {
	    स्थिर अक्षर *nm = vpi_get_str(vpiName, hand);

	    अगर (म_भेद(nm_first,nm)==0) {
		  अगर (nm_rest)
			rtn=find_scope(nm_rest, hand, depth+1);
		  अन्यथा
			rtn = hand;
	    }

	    /* found it yet ? */
	    अगर (rtn) {
		  vpi_free_object(iter);
		  अवरोध;
	    }
      }

      वापस rtn;
}

// Find the end of the first escaped identifier or simple identifier
अटल अक्षर * find_अग्र(अक्षर *name)
{
      अक्षर *अग्र_2;
      अगर (*name == '\\') {
	    अग्र_2 = म_अक्षर(name, ' ');
	    अगर (अग्र_2 && *++अग्र_2 == 0) अग्र_2 = 0;
      } अन्यथा {
	    अग्र_2 = म_अक्षर(name, '.');
      }
      वापस अग्र_2;
}

vpiHandle vpi_handle_by_name(स्थिर अक्षर *name, vpiHandle scope)
{
      vpiHandle hand;

      अगर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_handle_by_name(%s, %p) -->\n",
		    name, scope);
      }

	// Chop the name into path and base. For example, if the name
	// is "a.b.c", then nm_path becomes "a.b" and nm_base becomes
	// "c". If the name is "c" then nm_path is nil and nm_base is "c".
      vector<अक्षर> name_buf (म_माप(name)+1);
      म_नकल(&name_buf[0], name);
      अक्षर*nm_path = &name_buf[0];
      अक्षर*nm_base;

	// Check to see if we have an escaped identifier and if so search
	// the long way because a '.' could be in the escaped name.
      अगर (म_अक्षर(nm_path, '\\')) {
	    अक्षर *अग्र_2;
	    nm_base = nm_path;
	    जबतक ((अग्र_2 = find_अग्र(nm_base))) {
		  nm_base = ++अग्र_2;
	    }
	    अगर (nm_path == nm_base) {
		  nm_path = 0;
	    } अन्यथा {
		  *(nm_base-1) = 0;
	    }

	// If there is no escaped identifier then just look for the last '.'
      } अन्यथा {
	    nm_base = म_खोजप(nm_path, '.');
	    अगर (nm_base) {
		  *nm_base++ = 0;
	    } अन्यथा {
		  nm_base = nm_path;
		  nm_path = 0;
	    }
      }

      /* If scope provided, look in corresponding module; otherwise
       * traverse the hierarchy specified in name to find the leaf module
       * and try finding it there.
       */
      अगर (scope) {
	    /* Some implementations support either a module or a scope. */
	    चयन (vpi_get(vpiType, scope)) {
		हाल vpiScope:
	          hand = vpi_handle(vpiModule, scope);
	          अवरोध;
		हाल vpiModule:
	          hand = scope;
	          अवरोध;
		शेष:
		  अगर (vpi_trace) {
			ख_लिखो(vpi_trace, "vpi_handle_by_name: "
				"Scope is not a vpiScope or vpiModule\n");
		  }
	          // Use vpi_chk_error() here when it is implemented.
	          वापस 0;
	    }

      } अन्यथा अगर (nm_path) {
	      // The name has a path, and no other scope handle was
	      // passed in. That suggests we are looking for "a.b.c"
	      // in the root scope. So convert "a.b" to a scope and
	      // start there to look for "c".
	    hand = find_scope(nm_path, शून्य, 0);
	    nm_path = 0;

      } अन्यथा {
	      // Special case: scope==<nil>, meaning we are looking in
	      // the root, and there is no path to the name, i.e. the
	      // string is "c" instead of "top.c". Try to find "c" as
	      // a scope and return that.
	    hand = find_scope(nm_base, शून्य, 0);
      }

      अगर (hand == 0) {
	    अगर (vpi_trace) {
		  ख_लिखो(vpi_trace, "vpi_handle_by_name: "
			  "Scope does not exist. Giving up.\n");
	    }

	    वापस 0;
      }

	// If there is a path part, then use it to find the
	// scope. For example, if the full name is a.b.c, then
	// the nm_path string is a.b and we search for that
	// scope. If we find it, then set hand to that scope.
      अगर (nm_path) {
	    vpiHandle tmp = find_scope(nm_path, hand, 0);
	    जबतक (tmp == 0 && hand != 0) {
		  hand = vpi_handle(vpiScope, hand);
		  tmp = find_scope(nm_path, hand, 0);
	    }
	    hand = tmp;
      }

	// find_name() expects escaped identifiers to be stripped
      अगर (*nm_base == '\\') {
	    // Skip the \ at the beginning
	    ++nm_base;

	    // Drop the space at the end if it exists
	    अक्षर *अग्र_2;
	    अगर ((अग्र_2 = म_अक्षर(nm_base, ' '))) {
		  *अग्र_2 = 0;
	    }
      }

	// Now we have the correct scope, look for the item.
      vpiHandle out = find_name(nm_base, hand);

      अगर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_handle_by_name: DONE\n");
      }

      वापस out;
}

// Return tuple {x1, x2, x3} with:
// x1: True when cur and net2 are connected
// x2: Ptr to previous node
// x3: True when net is connected to output of previous node
tuple<bool, vvp_net_ptr_t, bool> check_connected_to_concat8_and_part_sa(vvp_net_ptr_t cur, vvp_net_t* net2, vvp_net_ptr_t parent) {

      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      जबतक (cur.ptr()) {
	    अगर (cur.ptr() == net2 && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
		    // Pass on the return value if found
		  वापस {true, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
	    } अन्यथा अगर (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  स्वतः res = check_connected_to_concat8_and_part_sa(cur.ptr()->out_, net2, cur);
		  अगर (लो<0>(res)) {
			वापस res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      वापस {false, vvp_net_ptr_t(nullptr, 0), false};
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
      जबतक (cur.ptr()) {
	      // Check if it's a concat8
	    अगर (cur.ptr() == net2) {
		  वापस {true, vvp_net_ptr_t(nullptr, 0), false};
	    } अन्यथा अगर (dynamic_cast<vvp_fun_concat8*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_part_sa*>(cur.ptr()->fun) || dynamic_cast<vvp_fun_concat*>(cur.ptr()->fun)) {
		  स्वतः res = check_connected_to_concat8(cur.ptr()->out_, net2, cur);
		  अगर (लो<0>(res)) {
			अगर (!लो<1>(res).ptr() && dynamic_cast<vvp_fun_part_sa*>(parent.ptr()->fun)) {
			      // `parent` is the last part_sa node in front of our destination
			      // We want to place the intermod node here
			      वापस {true, prev.ptr() ? prev : parent, prev.ptr() == nullptr};
			}
			वापस res;
		  }
	    }

	      // Next net in linked list
	    prev = cur;
	    cur = cur.ptr()->port[cur.port()];
      }

	// net2 is not connected to this concat8
      वापस {false, vvp_net_ptr_t(nullptr, 0), false};
}

// Used to get intermodpath for two ports
vpiHandle vpi_handle_multi(PLI_INT32 type,
                           vpiHandle ref1,
                           vpiHandle ref2)
{
      अगर (vpi_trace) {
	    ख_लिखो(vpi_trace, "vpi_handle_multi(%d, %p, %p) -->\n",
	            type, ref1, ref2);
      }

      अगर (type != vpiInterModPath) {
	    ख_लिखो(मानक_त्रुटि, "sorry: vpi_handle_multi currently supports"
	            "only vpiInterModPath\n");
	    वापस nullptr;
      }

	// Indicates whether port1 refers to a single bit
      bool port1_has_सूचकांक = false;
      पूर्णांक port1_bit_सूचकांक = 0;
      vpiPortBitInfo* port1_bit = dynamic_cast<vpiPortBitInfo*>(ref1);

      अगर (port1_bit) {
	      // Get the bit index
	    port1_has_सूचकांक = true;
	    port1_bit_सूचकांक = vpi_get(vpiBit, port1_bit);
	      // Update the ref1 to point to the base port
	    ref1 = vpi_handle(vpiParent, port1_bit);
      }

	// Indicates whether port2 refers to a single bit
      bool port2_has_सूचकांक = false;
      पूर्णांक port2_bit_सूचकांक = 0;
      vpiPortBitInfo* port2_bit = dynamic_cast<vpiPortBitInfo*>(ref2);

      अगर (port2_bit) {
	      // Get the bit index
	    port2_has_सूचकांक = true;
	    port2_bit_सूचकांक = vpi_get(vpiBit, port2_bit);
	      // Update the ref1 to point to the base port
	    ref2 = vpi_handle(vpiParent, port2_bit);
      }

      vpiPortInfo* port1 = dynamic_cast<vpiPortInfo*>(ref1);

      अगर (!port1) {
	    ख_लिखो(मानक_त्रुटि, "sorry: second argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    वापस nullptr;
      }

      vpiPortInfo* port2 = dynamic_cast<vpiPortInfo*>(ref2);

      अगर (!port2) {
	    ख_लिखो(मानक_त्रुटि, "sorry: third argument of vpi_handle_multi"
		    "must be a vpiPort\n");
	    वापस nullptr;
      }

	// Get the names of both ports
      std::string port1_name(vpi_get_str(vpiName, ref1));
      std::string port2_name(vpi_get_str(vpiName, ref2));

	// If both ports are vpiOutput, we have to reassign the __vpiSignal from port1
	// to port2 because otherwise the non-delayed version of the signal is dumped
	// even tho the intermodpath is correctly inserted
      __vpiSignal* output_signal = nullptr;

      अगर (port1->get_direction() == vpiOutput && port2->get_direction() == vpiOutput) {
	    vpiHandle scope_port2 = vpi_handle(vpiScope, ref2);
	    निश्चित(scope_port2);

	      // Iterate over nets in the scope of port2
	    vpiHandle net_i = vpi_iterate(vpiNet, scope_port2) ;
	    vpiHandle net;

	    जबतक ((net = vpi_scan(net_i)) != शून्य) {
		  std::string net_name(vpi_get_str(vpiName, net));

		    // Compare whether the net matches with the port name
		  अगर (net_name == port2_name) {
			output_signal = dynamic_cast<__vpiSignal*>(net);
		  }
	    }
      }

      vvp_net_t* net1 = port1->get_port();
      vvp_net_t* net2 = port2->get_port();

      अगर (net1 == nullptr || net2 == nullptr) {
	    ख_लिखो(मानक_त्रुटि, "Error: Could not find net. "
		    "Did you run iverilog with '-ginterconnect'?\n");
	    वापस nullptr;
      }

      अगर (net1 == net2) {
	    ख_लिखो(मानक_त्रुटि, "Error: Net for both ports is the same. "
		    "Did you pass the same port twice?\n");
	    वापस nullptr;
      }

      अगर (!dynamic_cast<vvp_fun_buft*>(net1->fun)) {
	    ख_लिखो(मानक_त्रुटि, "Error: functor of net1 must be"
		    "vvp_fun_buft\n");
	    वापस nullptr;
      }

      अगर (!dynamic_cast<vvp_fun_buft*>(net2->fun)) {
	    ख_लिखो(मानक_त्रुटि, "Error: functor of net2 must be"
		    "vvp_fun_buft\n");
	    वापस nullptr;
      }

	// If port1 is actually a port bit, we have to get to the correct vvp_fun_part
	// after which we insert the intermodpath delay
      अगर (port1_has_सूचकांक) {
	    vvp_net_ptr_t* net1_ptr = &net1->out_;

	      // Search for part selects connected to port1
	    vvp_net_t* current_net = net1_ptr->ptr();

	    जबतक (current_net) {
		  स्थिर vvp_fun_part* part = dynamic_cast<vvp_fun_part*>(current_net->fun);

		    // Its a part select!
		  अगर (part) {
			  // Is it the correct part select?
			अगर (part->get_base() == (अचिन्हित)port1_bit_सूचकांक) {
			      निश्चित(part->get_wid() == 1);
			      net1 = current_net; // Replace net1 as this is our new start point
			      अवरोध;
			}
		  }

		  current_net = current_net->port[0].ptr(); // BUFT has only one input, index 0
	    }
      }

	// Iterate over all nodes connected to port1
      vvp_net_ptr_t cur = net1->out_;
      vvp_net_ptr_t prev = vvp_net_ptr_t(nullptr, 0);

      जबतक (cur.ptr()) {
	      // Either port2 is directly connected to port1
	      // Or in the second case port2 is indirectly connected
	      // to port1 through a net of concat8s
	    bool is_connected, is_output;
	    vvp_net_ptr_t previous_गाँठ;
	    tie(is_connected, previous_गाँठ, is_output) = port2_has_सूचकांक
		  ? check_connected_to_concat8(cur, net2, vvp_net_ptr_t(net1, 0))
		  : check_connected_to_concat8_and_part_sa(cur, net2, vvp_net_ptr_t(net1, 0));
	    अगर ( (!port2_has_सूचकांक && cur.ptr() == net2) || is_connected ) {
		  vvp_net_t*new_net = नया vvp_net_t;

		    // Create new node with intermodpath and connect port2 to it
		  पूर्णांक width = 1; // TODO
		  vvp_fun_intermodpath*obj = नया vvp_fun_intermodpath(new_net, width);

		  vvp_net_t* output_net = net1;
		  अगर (is_connected) {
			अगर (is_output) {
			      output_net = previous_गाँठ.ptr(); // net2 is direct output of part_sa
			      cur = output_net->out_;
			      prev = vvp_net_ptr_t(nullptr, 0);
			} अन्यथा {
			      prev = previous_गाँठ;
			      अगर (prev.ptr()) {
				    cur = prev.ptr()->port[prev.port()];
			      }
			}
		  }

		  new_net->fun = obj;
		  new_net->out_ = cur;

		    // Port2 is in the middle of the list
		    // Insert intermodpath before port2 and keep everything else intact
		  अगर (prev.ptr()) {
			prev.ptr()->port[prev.port()] = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; // Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		    // Port2 is first in list
		    // Insert intermodpath before port2 and keep everything else intact
		  } अन्यथा {
			output_net->out_ = vvp_net_ptr_t(new_net, 0); // Point to port 0 of vvp_fun_intermodpath
			new_net->port[0] = cur.ptr()->port[cur.port()]; //  Connect the next net in list
			cur.ptr()->port[cur.port()] = vvp_net_ptr_t(nullptr, 0); // Only port2 is connected to intermodpath
		  }

		    // If both ports are vpiOutput and port2 is not a vector,
		    // we have to reassign the __vpiSignal so that the delayed
		    // values get dumped
		  अगर (output_signal && !port2_has_सूचकांक) {
			net2->fil = net1->fil;
			net1->fil = nullptr;
			output_signal->गाँठ = net2;
		  }

		    // Create the VPI intermodpath object
		  __vpiInterModPath* intermodpath = vpip_make_intermodpath(new_net, port1, port2);
		  intermodpath->intermodpath = obj;

		    // Finally done, return the intermodpath object
		  वापस intermodpath;
	    }

	    prev = cur;
	    cur = cur.ptr()->port[cur.port()]; // Next net in linked list
      }

      ख_लिखो(मानक_त्रुटि, "VPI error: Could not insert intermodpath!\n");
      ख_लिखो(मानक_त्रुटि, "\tport1 = %s, port1_has_index = %d, port1_bit_index = %d\n", port1_name.c_str(), port1_has_सूचकांक, port1_bit_सूचकांक);
      ख_लिखो(मानक_त्रुटि, "\tport2 = %s, port2_has_index = %d, port2_bit_index = %d\n", port2_name.c_str(), port2_has_सूचकांक, port2_bit_सूचकांक);
      वापस nullptr;
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



व्योम vpi_get_delays(vpiHandle व्यंजक, p_vpi_delay delays)
{
      निश्चित(व्यंजक);
      निश्चित(delays);

      व्यंजक->vpi_get_delays(delays);

      अगर (vpi_trace) {
	    ख_लिखो(vpi_trace,
		    "vpi_get_delays(%p, %p) -->\n", व्यंजक, delays);
      }
}


व्योम vpi_put_delays(vpiHandle व्यंजक, p_vpi_delay delays)
{
      निश्चित(व्यंजक  );
      निश्चित(delays );

      व्यंजक->vpi_put_delays(delays);

      अगर (vpi_trace) {
	    ख_लिखो(vpi_trace,
		    "vpi_put_delays(%p, %p) -->\n", व्यंजक, delays);
      }
}








बाह्य "C" PLI_INT32 vpi_vprintf(स्थिर अक्षर*fmt, बहु_सूची ap)
{
      वापस vpi_mcd_vprintf(1, fmt, ap);
}

बाह्य "C" PLI_INT32 vpi_printf(स्थिर अक्षर *fmt, ...)
{
      बहु_सूची ap;
      बहु_शुरू(ap, fmt);
      पूर्णांक r = vpi_mcd_vprintf(1, fmt, ap);
      बहु_पूर्ण(ap);
      वापस r;
}

बाह्य "C" PLI_INT32 vpi_flush(व्योम)
{
      वापस vpi_mcd_flush(1);
}


बाह्य "C" PLI_INT32 vpi_sim_vcontrol(पूर्णांक operation, बहु_सूची ap)
{
      दीर्घ diag_संदेश;

      चयन (operation) {
	  हाल vpiFinish:
	  हाल __ivl_legacy_vpiFinish:
            diag_संदेश = बहु_तर्क(ap, PLI_INT32);
	    schedule_finish(diag_संदेश);
	    अवरोध;

	  हाल vpiStop:
	  हाल __ivl_legacy_vpiStop:
            diag_संदेश = बहु_तर्क(ap, PLI_INT32);
	    schedule_stop(diag_संदेश);
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Unsupported VPI control operation %d.\n", operation);
	    वापस 0;
      }
      वापस 1;
}

बाह्य "C" PLI_INT32 vpi_sim_control(PLI_INT32 operation, ...)
{
      बहु_सूची ap;
      बहु_शुरू(ap, operation);
      पूर्णांक r = vpi_sim_vcontrol(operation, ap);
      बहु_पूर्ण(ap);
      वापस r;
}

बाह्य "C" PLI_INT32 vpi_control(PLI_INT32 operation, ...)
{
      बहु_सूची ap;
      बहु_शुरू(ap, operation);
      पूर्णांक r = vpi_sim_vcontrol(operation, ap);
      बहु_पूर्ण(ap);
      वापस r;
}

/*
 * This routine calculated the return value for $clog2.
 * It is easier to do it here vs trying to to use the VPI interface.
 */
बाह्य "C" s_vpi_vecval vpip_calc_clog2(vpiHandle arg)
{
      s_vpi_vecval rtn;
      s_vpi_मूल्य val;
      vvp_vector4_t vec4;
      bool is_neg = false;  // At this point only a real can be negative.

	/* Get the value as a vvp_vector4_t. */
      val.format = vpiObjTypeVal;
      vpi_get_मूल्य(arg, &val);
      अगर (val.format == vpiRealVal) {
	    vpi_get_मूल्य(arg, &val);
	      /* All double values can be represented in 1024 bits. */
	    vec4 = vvp_vector4_t(1024, val.value.real);
	    अगर (val.value.real < 0) is_neg = true;
      } अन्यथा {
	    val.format = vpiVectorVal;
	    vpi_get_मूल्य(arg, &val);
	    अचिन्हित wid = vpi_get(vpi_आकार, arg);
	    vec4 = vvp_vector4_t(wid, BIT4_0);
	    क्रम (अचिन्हित idx=0; idx < wid; idx += 1) {
		PLI_INT32 aval = val.value.vector[idx/32].aval;
		PLI_INT32 bval = val.value.vector[idx/32].bval;
		aval >>= idx % 32;
		bval >>= idx % 32;
		पूर्णांक bitmask = (aval&1) | ((bval<<1)&2);
		vvp_bit4_t bit = scalar_to_bit4(bitmask);
		vec4.set_bit(idx, bit);
	    }
      }

      अगर (vec4.has_xz()) {
	    rtn.aval = rtn.bval = 0xFFFFFFFFU;  /* Set to 'bx. */
	    वापस rtn;
      }

      vvp_vector2_t vec2(vec4);

      अगर (is_neg) vec2.trim_neg(); /* This is a special trim! */
      अन्यथा vec2.trim(); /* This makes less work shifting. */

	/* Calculate the clog2 result. */
      PLI_INT32 res = 0;
      अगर (!vec2.is_zero()) {
	    vec2 -= vvp_vector2_t(1, vec2.size());
	    जबतक(!vec2.is_zero()) {
		  res += 1;
		  vec2 >>= 1;
	    }
      }

      rtn.aval = res;
      rtn.bval = 0;
      वापस rtn;
}

/*
 * This routine provides the information needed to implement $countdrivers.
 * It is done here for performance reasons - interrogating the drivers
 * individually via the VPI interface would be much slower.
 */
बाह्य "C" व्योम vpip_गिनती_drivers(vpiHandle ref, अचिन्हित idx,
                                   अचिन्हित counts[4])
{
      काष्ठा __vpiSignal*rfp = dynamic_cast<__vpiSignal*>(ref);
      निश्चित(rfp);
      rfp->गाँठ->गिनती_drivers(idx, counts);
}

#अगर defined(__MINGW32__) || defined (__CYGWIN__)
vpip_routines_s vpi_routines = {
    .register_cb                = vpi_register_cb,
    .remove_cb                  = vpi_remove_cb,
    .register_systf             = vpi_register_systf,
    .get_systf_info             = vpi_get_systf_info,
    .handle_by_name             = vpi_handle_by_name,
    .handle_by_सूचकांक            = vpi_handle_by_सूचकांक,
    .handle_multi               = vpi_handle_multi,
    .handle                     = vpi_handle,
    .iterate                    = vpi_iterate,
    .scan                       = vpi_scan,
    .लो                        = vpi_get,
    .get64                      = vpi_get64,
    .get_str                    = vpi_get_str,
    .get_delays                 = vpi_get_delays,
    .put_delays                 = vpi_put_delays,
    .get_मूल्य                  = vpi_get_मूल्य,
    .put_मूल्य                  = vpi_put_मूल्य,
    .get_time                   = vpi_get_time,
    .get_userdata               = vpi_get_userdata,
    .put_userdata               = vpi_put_userdata,
    .mcd_open                   = vpi_mcd_open,
    .mcd_close                  = vpi_mcd_close,
    .mcd_flush                  = vpi_mcd_flush,
    .mcd_name                   = vpi_mcd_name,
    .mcd_vprintf                = vpi_mcd_vprintf,
    .सफाई                      = vpi_flush,
    .भ_लिखो                    = vpi_vprintf,
    .chk_error                  = vpi_chk_error,
    .compare_objects            = vpi_compare_objects,
    .free_object                = vpi_free_object,
    .release_handle             = vpi_release_handle,
    .get_vlog_info              = vpi_get_vlog_info,
    .vcontrol                   = vpi_sim_vcontrol,
    .ख_खोलो                      = vpi_fopen,
    .get_file                   = vpi_get_file,
    .calc_clog2                 = vpip_calc_clog2,
    .गिनती_drivers              = vpip_गिनती_drivers,
    .format_strength            = vpip_format_strength,
    .format_pretty              = vpip_format_pretty,
    .make_systf_system_defined  = vpip_make_systf_system_defined,
    .mcd_rawwrite               = vpip_mcd_rawwrite,
    .set_return_मूल्य           = vpip_set_return_मूल्य,
};
#पूर्ण_अगर
