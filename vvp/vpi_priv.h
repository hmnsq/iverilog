#నిర్వచితం_కాకపోతే IVL_vpi_priv_H
#నిర్వచించు IVL_vpi_priv_H
/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
 * Copyright (c) 2016 CERN Michele Castellana (michele.castellana@cern.ch)
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

# include  "sv_vpi_user.h"
# include  "vvp_net.h"
# include  "config.h"

# include  <map>
# include  <set>
# include  <string>
# include  <vector>

/*
 * Added to use some "vvp_fun_modpath_src"
 * and "vvp_fun_modpath" classes definitions
 */
#అన్తర్భావ  "delay.h"


class class_type;
class vvp_darray;
class vvp_fun_arrayport;

రకనిర్వచనం నిర్మాణం __vpiArray* vvp_array_t;

/*
 * This header file contains the internal definitions that the vvp
 * program uses to implement the public interface in the vpi_user.h
 * header file elsewhere.
 */

#ఒకవేళ defined(__MINGW32__) || defined (__CYGWIN__)
బాహ్య vpip_routines_s vpi_routines;
#అయితే_ముగింపు

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#నిర్వచించు vpi_File_పంక్తి    0x1000003
#నిర్వచించు _vpiDescription 0x1000004

బాహ్య bool show_file_పంక్తి;
బాహ్య bool సంకేతం_is_instrumented;

బాహ్య vpiHandle vpip_build_file_పంక్తి(స్థిరరాశి అక్షరం*description,
                                      దీర్ఘ file_idx, దీర్ఘ lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
బాహ్య స్థిరరాశి అక్షరం* vpi_type_as_string(PLI_INT32 సంకేతం_2);

/*
 * Return the port direction as a string.
 */
బాహ్య స్థిరరాశి అక్షరం* direction_as_string(పూర్ణాంకం dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#ఒకవేళ defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
#నిర్వచించు _vpiFromThr 0x1000001
#   define _vpiNoThr   0
#   define _vpiString  1
#   define _vpiVThr    2
#   define vpi_మాట    3
#   define _vpi_at_PV  4
#   define _vpi_at_A   5
#   define _vpi_at_APV 6
#అయితే_ముగింపు


/*
 * The vpi_mode_flag contains the major mode for VPI use. This is used
 * to generate error messages when vpi functions are called
 * incorrectly.
 */
గణన vpi_బహుళకం_t {
      VPI_బహుళకం_NONE =0,
	/* The compiler is calling a register function. */
      VPI_బహుళకం_రిజిస్టరు,
	/* The compiler is calling a compiletf function. */
      VPI_బహుళకం_COMPILETF,
	/* The compiler is calling a calltf function. */
      VPI_బహుళకం_CALLTF,
	/* We are in the midst of a RWSync callback. */
      VPI_బహుళకం_RWSYNC,
	/* We are in a ROSync callback. */
      VPI_బహుళకం_ROSYNC
};
బాహ్య vpi_బహుళకం_t vpi_బహుళకం_flag;

/*
 * This structure is the very base of a vpiHandle. Every handle
 * structure is derived from this class so that the library can
 * internally pass the derived types as pointers to one of these.
 */
class __vpiHandle {
    బహిరంగ:
      అంతఃవరుస __vpiHandle() { }
	// The destructor is virtual so that dynamic types will work.
      మాయా ~__vpiHandle();

      మాయా పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి =0;
      మాయా పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2);
      మాయా int64_t vpi_get64(పూర్ణాంకం సంకేతం_2);
      మాయా అక్షరం* vpi_get_str(పూర్ణాంకం సంకేతం_2);

      మాయా శూన్య vpi_get_విలువ(p_vpi_విలువ val);
      మాయా vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags);
      మాయా vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2);
      మాయా vpiHandle vpi_iterate(పూర్ణాంకం సంకేతం_2);
      మాయా vpiHandle vpi_సూచీ(పూర్ణాంకం idx);
      మాయా శూన్య vpi_get_delays(p_vpi_delay del);
      మాయా శూన్య vpi_put_delays(p_vpi_delay del);

	// Objects may have destroyer functions of their own. If so,
	// then this virtual method will return a POINTER to that
	// function. The pointer is used to "delete" the object, which
	// is why the function itself cannot be a method.
      రకనిర్వచనం పూర్ణాంకం (*free_object_fun_t)(vpiHandle);
      మాయా free_object_fun_t free_object_fun(శూన్య);
};


/*
 * The vpiHandle for an iterator has this structure. The definition of
 * the methods lives in vpi_iter.c
 *
 * The args and nargs members point to the array of vpiHandle objects
 * that are to be iterated over. The next member is the index of the
 * next item to be returned by a vpi_scan.
 *
 * The free_args_flag member is true if when this iterator object is
 * released it must also free the args array.
 */
నిర్మాణం __vpiIterator : బహిరంగ __vpiHandle {
      __vpiIterator();
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      free_object_fun_t free_object_fun(శూన్య) override;

      vpiHandle *వాదనలు;
      గుర్తులేని  nargs;
      గుర్తులేని  తదుపరి_2;
      bool free_వాదనలు_flag;
};

బాహ్య vpiHandle vpip_make_iterator(గుర్తులేని nargs, vpiHandle*వాదనలు,
				    bool free_వాదనలు_flag);

class __vpiDecConst : బహిరంగ __vpiHandle {
    బహిరంగ:
      explicit __vpiDecConst(పూర్ణాంకం val =0);
      __vpiDecConst(స్థిరరాశి __vpiDecConst&that);
# ఒకవేళ __cplusplus >= 201103L
      __vpiDecConst& క్రియాకారకం = (స్థిరరాశి __vpiDecConst&that) = అప్రమేయ;
# endif
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;

    బహిరంగ:
      అంతఃవరుస పూర్ణాంకం get_విలువ() స్థిరరాశి { తిరిగి విలువ; }
      అంతఃవరుస శూన్య సమితి_విలువ(పూర్ణాంకం val) { విలువ = val; }

    అంతరంగ:
      పూర్ణాంకం విలువ;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
నిర్మాణం __vpiCallback : బహిరంగ __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;

	// Used for listing callbacks.
      నిర్మాణం __vpiCallback*తదుపరి_2;

	// user supplied callback data
      నిర్మాణం t_cb_దత్తాంశం cb_దత్తాంశం;
};

class విలువ_callback : బహిరంగ __vpiCallback {
    బహిరంగ:
      explicit విలువ_callback(p_cb_దత్తాంశం దత్తాంశం);
	// Return true if the callback really is ready to be called
      మాయా bool test_విలువ_callback_ready(శూన్య);

    బహిరంగ:
	// user supplied callback data
      నిర్మాణం t_vpi_సమయం cb_సమయం;
      నిర్మాణం t_vpi_విలువ cb_విలువ;
};

బాహ్య శూన్య callback_execute(నిర్మాణం __vpiCallback*cur);

నిర్మాణం __vpiSystemTime : బహిరంగ __vpiHandle {
      __vpiSystemTime();
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      అక్షరం*vpi_get_str(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
      vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2) override;

      __vpiScope*scope;
};

నిర్మాణం __vpiScopedTime : బహిరంగ __vpiSystemTime {
      __vpiScopedTime();
      అక్షరం*vpi_get_str(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
};
నిర్మాణం __vpiScopedSTime : బహిరంగ __vpiSystemTime {
      __vpiScopedSTime();
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      అక్షరం*vpi_get_str(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
};
నిర్మాణం __vpiScopedRealtime : బహిరంగ __vpiSystemTime {
      __vpiScopedRealtime();
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      అక్షరం*vpi_get_str(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : బహిరంగ __vpiHandle {

    బహిరంగ:
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      అక్షరం* vpi_get_str(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_iterate(పూర్ణాంకం సంకేతం_2) override;

    బహిరంగ:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      అంతఃవరుస స్థిరరాశి అక్షరం*scope_name() స్థిరరాశి { తిరిగి name_; }

      అంతఃవరుస స్థిరరాశి అక్షరం*scope_def_name() స్థిరరాశి { తిరిగి tname_; }
	// TRUE if this is an automatic func/task/block
      అంతఃవరుస bool is_automatic() స్థిరరాశి { తిరిగి is_automatic_; }

    బహిరంగ:
      __vpiScope *scope;
      గుర్తులేని file_idx;
      గుర్తులేని lineno;
      గుర్తులేని def_file_idx;
      గుర్తులేని def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_సమయం;
      నిర్మాణం __vpiScopedSTime scoped_stime;
      నిర్మాణం __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::సదిశ<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      నిర్మాణం automatic_hooks_s**అంశం;
      గుర్తులేని nitem;
        /* Keep a list of live contexts. */
      vvp_context_t సజీవ_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::సమితి<vthread_t> దారాలు;
      గుర్తుగల పూర్ణాంకం సమయం_units :8;
      గుర్తుగల పూర్ణాంకం సమయం_precision :8;

    రక్షిత:
      __vpiScope(స్థిరరాశి అక్షరం*nam, స్థిరరాశి అక్షరం*tnam, bool is_auto_flag =false);

    అంతరంగ:
	/* The scope has a name. */
      స్థిరరాశి అక్షరం*name_;
      స్థిరరాశి అక్షరం*tname_;
	/* the scope may be "automatic" */
      bool is_automatic_;
};

class vpiScopeFunction  : బహిరంగ __vpiScope {
    బహిరంగ:
      అంతఃవరుస vpiScopeFunction(స్థిరరాశి అక్షరం*nam, స్థిరరాశి అక్షరం*tnam,
			      bool auto_flag, పూర్ణాంకం func_type, గుర్తులేని func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiFunction; }
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override
      {
	    ఎంపిక (సంకేతం_2) {
		స్థితి vpiFuncType:
		  తిరిగి func_type_;
		అప్రమేయ:
		  తిరిగి __vpiScope::vpi_get(సంకేతం_2);
	    }
      }

    బహిరంగ:
      అంతఃవరుస గుర్తులేని get_func_వెడల్పు(శూన్య) స్థిరరాశి { తిరిగి func_wid_; }
      అంతఃవరుస vvp_bit4_t get_func_init_val(శూన్య) స్థిరరాశి { తిరిగి func_init_val_; }

    అంతరంగ:
      పూర్ణాంకం func_type_;
      గుర్తులేని func_wid_;
      vvp_bit4_t func_init_val_;
};

బాహ్య __vpiScope* vpip_peek_current_scope(శూన్య);
బాహ్య శూన్య vpip_attach_గమ్యం_scope(__vpiScope*scope, vpiHandle obj);
బాహ్య శూన్య vpip_attach_గమ్యం_current_scope(vpiHandle obj);
బాహ్య __vpiScope* vpip_peek_context_scope(శూన్య);
బాహ్య గుర్తులేని vpip_కూడు_అంశం_గమ్యం_context(automatic_hooks_s*అంశం,
                                         __vpiScope*scope);
బాహ్య vpiHandle vpip_make_మూలం_iterator(పూర్ణాంకం type_సంకేతం);
బాహ్య శూన్య vpip_make_మూలం_iterator(class __vpiHandle**&పట్టిక,
				    గుర్తులేని&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
నిర్మాణం __vpiSignal : బహిరంగ __vpiHandle {
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      int64_t vpi_get64(పూర్ణాంకం సంకేతం_2) override;
      అక్షరం* vpi_get_str(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
      vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags) override;
      vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_iterate(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_సూచీ(పూర్ణాంకం idx) override;

    బహిరంగ:
      గుర్తులేని వెడల్పు() స్థిరరాశి;
      vpiHandle get_సూచీ(పూర్ణాంకం సూచీ);
      శూన్య get_బిట్టు_విలువ(స్థిరరాశి నిర్మాణం __vpiBit*బిట్టు, p_vpi_విలువ vp);
      vpiHandle put_బిట్టు_విలువ(నిర్మాణం __vpiBit*బిట్టు, p_vpi_విలువ vp, పూర్ణాంకం flags);
      శూన్య make_బిట్లు();

      నిర్మాణం __vpiBit*బిట్లు;

    బహిరంగ:
      సంఘం { // The scope or parent array that contains me.
	    vpiHandle మాతృ;
	    __vpiScope* scope;
      } within;
      సంఘం { // The name of this reg/net, or the index for array words.
            స్థిరరాశి అక్షరం*name;
            vpiHandle సూచీ;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      గుర్తులేని signed_flag  : 1;
      గుర్తులేని is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*కణుపు;

    బహిరంగ:
      స్థిర శూన్య*క్రియాకారకం new(std::size_t కొలత);
      స్థిర శూన్య క్రియాకారకం delete(శూన్య*); // not implemented
    రక్షిత:
      అంతఃవరుస __vpiSignal() : బిట్లు(ఖాళీ) { }
    అంతరంగ: // Not implemented
      స్థిర శూన్య*క్రియాకారకం new[] (std::size_t కొలత);
      స్థిర శూన్య క్రియాకారకం delete[](శూన్య*);
};
బాహ్య గుర్తులేని vpip_కొలత(స్థిరరాశి __vpiSignal *sig);
బాహ్య __vpiScope* vpip_scope(__vpiSignal*sig);

బాహ్య vpiHandle vpip_make_int2(స్థిరరాశి అక్షరం*name, పూర్ణాంకం msb, పూర్ణాంకం lsb,
			       bool signed_flag, vvp_net_t*vec);
బాహ్య vpiHandle vpip_make_int4(స్థిరరాశి అక్షరం*name, పూర్ణాంకం msb, పూర్ణాంకం lsb,
			       vvp_net_t*vec);
బాహ్య vpiHandle vpip_make_var4(స్థిరరాశి అక్షరం*name, పూర్ణాంకం msb, పూర్ణాంకం lsb,
			       bool signed_flag, vvp_net_t*net);
బాహ్య vpiHandle vpip_make_net4(__vpiScope*scope,
				స్థిరరాశి అక్షరం*name, పూర్ణాంకం msb, పూర్ణాంకం lsb,
				bool signed_flag, vvp_net_t*కణుపు);

/*
 * This is used to represent a bit in a net/reg.
 */
నిర్మాణం __vpiBit {
      నిర్మాణం as_బిట్టు_t : బహిరంగ __vpiHandle {
	    పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి;
	    పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2);
	    అక్షరం* vpi_get_str(పూర్ణాంకం సంకేతం_2);
	    శూన్య vpi_get_విలువ(p_vpi_విలువ val);
	    vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags);
	    vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2);
      } as_బిట్టు;

      vpiHandle సూచీ;

      సంఘం {
	    నిర్మాణం __vpiSignal*మాతృ;
	    నిర్మాణం __vpiBit*bit0;
      };

      అంతఃవరుస గుర్తులేని get_norm_సూచీ() స్థిరరాశి { తిరిగి this - bit0; }
      అంతఃవరుస నిర్మాణం __vpiSignal*get_మాతృ() స్థిరరాశి {తిరిగి (bit0 - 1)->మాతృ; }
      పూర్ణాంకం get_సూచీ(శూన్య) స్థిరరాశి;
};

class vpi_పోర్టు_బిట్టు_Info;

class vpi_పోర్టు_Info  : బహిరంగ __vpiHandle {
    బహిరంగ:
      vpi_పోర్టు_Info( __vpiScope *మాతృ,
                    గుర్తులేని సూచీ,
                    పూర్ణాంకం vpi_direction,
                    గుర్తులేని వెడల్పు,
                    స్థిరరాశి అక్షరం *name,
                    అక్షరం* బఫరు );
      ~vpi_పోర్టు_Info() override;

      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpi_పోర్టు; }
      పూర్ణాంకం get_direction(శూన్య) { తిరిగి direction_; }
      గుర్తులేని get_సూచీ(శూన్య) { తిరిగి సూచీ_2; }
      పూర్ణాంకం get_వెడల్పు(శూన్య) { తిరిగి వెడల్పు_2; }
      శూన్య కూడు_పోర్టు_బిట్టు(vpi_పోర్టు_బిట్టు_Info* పోర్టు_బిట్టు) { పోర్టు_బిట్లు.నెట్టు_వెనుక(పోర్టు_బిట్టు); }

      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      అక్షరం* vpi_get_str(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2) override;
      vvp_net_t* get_పోర్టు(శూన్య) స్థిరరాశి { తిరిగి ref_; }
      vpiHandle vpi_iterate(పూర్ణాంకం సంకేతం_2) override;

      std::సదిశ<vpi_పోర్టు_బిట్టు_Info*> పోర్టు_బిట్లు;

    అంతరంగ:
      __vpiScope *మాతృ_2;
      గుర్తులేని  సూచీ_2;
      పూర్ణాంకం       direction_;
      గుర్తులేని  వెడల్పు_2;
      స్థిరరాశి అక్షరం *name_;
      vvp_net_t *ref_;
};

class vpi_పోర్టు_బిట్టు_Info  : బహిరంగ __vpiHandle {
    బహిరంగ:
      vpi_పోర్టు_బిట్టు_Info(vpi_పోర్టు_Info *మాతృ,
                     గుర్తులేని బిట్టు);
      ~vpi_పోర్టు_బిట్టు_Info() override;

      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpi_పోర్టు_బిట్టు; }
      గుర్తులేని get_బిట్టు(శూన్య) స్థిరరాశి { తిరిగి బిట్టు_2; }

      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2) override;

    అంతరంగ:
      vpi_పోర్టు_Info *మాతృ_2;
      గుర్తులేని బిట్టు_2;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
నిర్మాణం __vpiPV : బహిరంగ __vpiHandle {
      __vpiPV();
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      అక్షరం* vpi_get_str(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
      vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags) override;
      vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2) override;

      vpiHandle మాతృ;
      vvp_net_t*net;
      vpiHandle sbase;
      పూర్ణాంకం tbase;
      గుర్తులేని వెడల్పు;
};
బాహ్య vpiHandle vpip_make_PV(అక్షరం*name, పూర్ణాంకం పునాది, పూర్ణాంకం వెడల్పు);
బాహ్య vpiHandle vpip_make_PV(అక్షరం*name, అక్షరం*చిహ్నం, పూర్ణాంకం వెడల్పు);
బాహ్య vpiHandle vpip_make_PV(అక్షరం*name, vpiHandle handle, పూర్ణాంకం వెడల్పు);

నిర్మాణం __vpiModPathTerm : బహిరంగ __vpiHandle {
      __vpiModPathTerm();
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2) override;

      vpiHandle సమాసం;
	/* The value returned by vpi_get(vpiEdge, ...); */
      పూర్ణాంకం అంచు;
};

నిర్మాణం __vpiModPathSrc : బహిరంగ __vpiHandle {
      __vpiModPathSrc();
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
      vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags) override;
      vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_iterate(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_సూచీ(పూర్ణాంకం idx) override;
      శూన్య vpi_get_delays(p_vpi_delay del) override;
      శూన్య vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(శూన్య) override;

      నిర్మాణం __vpiModPath *dest;
      పూర్ణాంకం   type;

	/* This is the input expression for this modpath. */
      నిర్మాణం __vpiModPathTerm మార్గం_పదం_లోపల;

	/* This is the input net for the modpath. signals on this net
	   are used to determine the modpath. They are *not* propagated
	   anywhere. */
      vvp_net_t *net;
} ;


/*
 *
 * The vpiModPath vpiHandle will define
 * a vpiModPath of record .modpath as defined
 * in the IEEE 1364
 *
 */

నిర్మాణం __vpiModPath {
      __vpiScope   *scope ;

      class vvp_fun_modpath*modpath;

      నిర్మాణం __vpiModPathTerm మార్గం_పదం_బహిర్గతం;
      vvp_net_t *ఆదానం_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

బాహ్య నిర్మాణం __vpiModPathSrc* vpip_make_modpath_src(నిర్మాణం __vpiModPath*మార్గం,
                                                     vvp_net_t *net) ;

బాహ్య నిర్మాణం __vpiModPath* vpip_make_modpath(vvp_net_t *net) ;

/*
 *
 * The vpiInterModPath vpiHandle will define
 * a vpiInterModPath of record .intermodpath as defined
 * in the IEEE 1364
 *
 */

నిర్మాణం __vpiInterModPath : బహిరంగ __vpiHandle {
      __vpiInterModPath();
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
      vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags) override;
      vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_iterate(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_delays(p_vpi_delay del) override;
      శూన్య vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(శూన్య) override;

      __vpiScope   *scope ;

      class vvp_fun_intermodpath*intermodpath;

      vvp_net_t *net;

      vpi_పోర్టు_Info* port1;
      vpi_పోర్టు_Info* port2;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiInterModPath
 */

బాహ్య నిర్మాణం __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpi_పోర్టు_Info* port1, vpi_పోర్టు_Info* port2);

/*
 * These methods support the vpi creation of events. The name string
 * passed in will be saved, so the caller must allocate it (or not
 * free it) after it is handed to this function.
 */
class __vpiNamedEvent : బహిరంగ __vpiHandle {

    బహిరంగ:
      __vpiNamedEvent(__vpiScope*scope, స్థిరరాశి అక్షరం*name);
      ~__vpiNamedEvent() override;

      __vpiNamedEvent(స్థిరరాశి __vpiNamedEvent&) = delete;
      __vpiNamedEvent& క్రియాకారకం=(స్థిరరాశి __vpiNamedEvent&) = delete;

      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      __vpiScope*get_scope(శూన్య) స్థిరరాశి { తిరిగి scope_; }
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      అక్షరం* vpi_get_str(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags) override;
      vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2) override;

      అంతఃవరుస శూన్య కూడు_vpi_callback(__vpiCallback*cb)
      { cb->తదుపరి_2 = callbacks_;
	callbacks_ = cb;
      }

      శూన్య నిరంతరం_vpi_callbacks(శూన్య);

	/* The functor, used for %set operations. */
      vvp_net_t*funct;

    అంతరంగ:
	/* base name of the event object */
      స్థిరరాశి అక్షరం*name_;
	/* Parent scope of this object. */
      __vpiScope*scope_;
	/* List of callbacks interested in this event. */
      __vpiCallback*callbacks_;
};

బాహ్య vpiHandle vpip_make_named_event(స్థిరరాశి అక్షరం*name, vvp_net_t*f);

/*
 * Memory is an array of bits that is accessible in N-bit chunks, with
 * N being the width of a word. The memory word handle just points
 * back to the memory and uses an index to identify its position in
 * the memory.
 */

బాహ్య bool is_net_array(vpiHandle obj);

/*
 * These are the various variable types.
 */
నిర్మాణం __vpiRealVar : బహిరంగ __vpiHandle {
      __vpiRealVar();
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      అక్షరం* vpi_get_str(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
      vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags) override;
      vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_iterate(పూర్ణాంకం సంకేతం_2) override;

      సంఘం { // The scope or parent array that contains me.
	    vpiHandle మాతృ;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      సంఘం {
            స్థిరరాశి అక్షరం*name;
            vpiHandle సూచీ;
      } id;
      గుర్తులేని is_netarray  : 1; // This is word of a net array
      గుర్తులేని is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

బాహ్య __vpiScope* vpip_scope(__vpiRealVar*sig);
బాహ్య vpiHandle vpip_make_real_var(స్థిరరాశి అక్షరం*name, vvp_net_t*net);
బాహ్య vpiHandle vpip_make_real_net(__vpiScope*scope,
				    స్థిరరాశి అక్షరం*name, vvp_net_t*net);

class __vpiBaseVar : బహిరంగ __vpiHandle {

    బహిరంగ:
      __vpiBaseVar(__vpiScope*scope, స్థిరరాశి అక్షరం*name, vvp_net_t*net);
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#అయితే_ముగింపు

      అంతఃవరుస vvp_net_t* get_net() స్థిరరాశి { తిరిగి net_; }

    రక్షిత:
      __vpiScope* scope_;
      స్థిరరాశి అక్షరం*name_;

    అంతరంగ:
      vvp_net_t*net_;
};

class __vpiStringVar : బహిరంగ __vpiBaseVar {

    బహిరంగ:
      __vpiStringVar(__vpiScope*scope, స్థిరరాశి అక్షరం*name, vvp_net_t*net);

      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
      vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags) override;
};

బాహ్య vpiHandle vpip_make_string_var(స్థిరరాశి అక్షరం*name, vvp_net_t*net);

నిర్మాణం __vpiArrayBase {
      __vpiArrayBase() : vals_పదాలు(ఖాళీ) {}
      మాయా ~__vpiArrayBase() {}

      మాయా గుర్తులేని get_కొలత(శూన్య) స్థిరరాశి = 0;
      మాయా vpiHandle get_ఎడమ_range() = 0;
      మాయా vpiHandle get_కుడి_range() = 0;
      మాయా __vpiScope*get_scope() స్థిరరాశి = 0;

      మాయా పూర్ణాంకం get_మాట_కొలత() స్థిరరాశి = 0;
      మాయా అక్షరం*get_మాట_str(నిర్మాణం __vpiArrayWord*మాట, పూర్ణాంకం సంకేతం_2) = 0;
      మాయా శూన్య get_మాట_విలువ(నిర్మాణం __vpiArrayWord*మాట, p_vpi_విలువ vp) = 0;
      మాయా శూన్య put_మాట_విలువ(నిర్మాణం __vpiArrayWord*మాట, p_vpi_విలువ vp,
                                    పూర్ణాంకం flags) = 0;
      మాయా vpiHandle get_iter_సూచీ(నిర్మాణం __vpiArrayIterator*iter, పూర్ణాంకం idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_పునాది_iterate(పూర్ణాంకం సంకేతం_2);

      మాయా శూన్య make_vals_పదాలు();

      నిర్మాణం __vpiArrayWord*vals_పదాలు;
};

/*
* The vpiArray object holds an array of vpi objects that themselves
* represent the words of the array. The vpi_array_t is a pointer to
* a struct __vpiArray.
*
* The details of the implementation depends on what this is an array
* of. The easiest case is if this is an array of nets.
*
* - Array of Nets:
* If this represents an array of nets, then the nets member points to
* an array of vpiHandle objects. Each vpiHandle is a word. This is
* done because typically each word of a net array is simultaneously
* driven and accessed by other means, so there is no advantage to
* compacting the array in any other way.
*
* - Array of vector4 words.
* In this case, the nets pointer is nil, and the vals4 member points
* to a vvl_vector4array_t object that is a compact representation of
* an array of vvp_vector4_t vectors.
*
* - Array of real variables
* The vals member points to a dynamic array objects that has an
* array of double variables. This is very much like the way the
* vector4 array works.
*/
నిర్మాణం __vpiArray : బహిరంగ __vpiArrayBase, బహిరంగ __vpiHandle {
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpi_జ్ఞాపకం; }
      గుర్తులేని get_కొలత() స్థిరరాశి override { తిరిగి array_లెక్క; }
      vpiHandle get_ఎడమ_range() override { ప్రతిజ్ఞా(nets == 0); తిరిగి &msb; }
      vpiHandle get_కుడి_range() override { ప్రతిజ్ఞా(nets == 0); తిరిగి &lsb; }
      __vpiScope*get_scope() స్థిరరాశి override { తిరిగి scope; }

      పూర్ణాంకం get_మాట_కొలత() స్థిరరాశి override;
      అక్షరం*get_మాట_str(నిర్మాణం __vpiArrayWord*మాట, పూర్ణాంకం సంకేతం_2) override;
      శూన్య get_మాట_విలువ(నిర్మాణం __vpiArrayWord*మాట, p_vpi_విలువ vp) override;
      శూన్య put_మాట_విలువ(నిర్మాణం __vpiArrayWord*మాట, p_vpi_విలువ vp, పూర్ణాంకం flags) override;

      vpiHandle get_iter_సూచీ(నిర్మాణం __vpiArrayIterator*iter, పూర్ణాంకం idx) override;

      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      అక్షరం* vpi_get_str(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2) override;
      అంతఃవరుస vpiHandle vpi_iterate(పూర్ణాంకం సంకేతం_2) override { తిరిగి vpi_array_పునాది_iterate(సంకేతం_2); }
      vpiHandle vpi_సూచీ(పూర్ణాంకం idx) override;

      శూన్య సమితి_మాట(గుర్తులేని idx, గుర్తులేని off, స్థిరరాశి vvp_vector4_t&val);
      శూన్య సమితి_మాట(గుర్తులేని idx, ద్విగుణ val);
      శూన్య సమితి_మాట(గుర్తులేని idx, స్థిరరాశి std::string&val);
      శూన్య సమితి_మాట(గుర్తులేని idx, స్థిరరాశి vvp_object_t&val);

      vvp_vector4_t get_మాట(గుర్తులేని చిరునామా);
      ద్విగుణ get_మాట_r(గుర్తులేని చిరునామా);
      శూన్య get_మాట_obj(గుర్తులేని చిరునామా, vvp_object_t&val);
      std::string get_మాట_str(గుర్తులేని చిరునామా);

      శూన్య alias_మాట(గుర్తులేని దీర్ఘ addr, vpiHandle మాట, పూర్ణాంకం msb, పూర్ణాంకం lsb);
      శూన్య attach_మాట(గుర్తులేని addr, vpiHandle మాట);
      శూన్య మాట_change(గుర్తులేని దీర్ఘ addr);

      స్థిరరాశి అక్షరం*name; /* Permanently allocated string */
      __vpiDecConst మొదటి_addr;
      __vpiDecConst చివరి_addr;
      __vpiDecConst msb;
      __vpiDecConst lsb;
      గుర్తులేని vals_వెడల్పు;
	// If this is a net array, nets lists the handles.
      vpiHandle*nets;
	// If this is a var array, then these are used instead of nets.
      vvp_vector4array_t*vals4;
      vvp_darray        *vals;

      vvp_fun_arrayport*ports_;
      నిర్మాణం __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool మార్పిడి_addr;

అంతరంగ:
      గుర్తులేని array_లెక్క;
      __vpiScope*scope;

మిత్ర vpiHandle vpip_make_array(స్థిరరాశి అక్షరం*label, స్థిరరాశి అక్షరం*name,
                                 పూర్ణాంకం మొదటి_addr, పూర్ణాంకం చివరి_addr,
                                 bool signed_flag);
మిత్ర శూన్య compile_array_alias(అక్షరం*label, అక్షరం*name, అక్షరం*src);
};

class __vpiDarrayVar : బహిరంగ __vpiBaseVar, బహిరంగ __vpiArrayBase {
    బహిరంగ:
      __vpiDarrayVar(__vpiScope*scope, స్థిరరాశి అక్షరం*name, vvp_net_t*net);

      పూర్ణాంకం get_type_సంకేతం() స్థిరరాశి override { తిరిగి vpiArrayVar; }
      గుర్తులేని get_కొలత() స్థిరరాశి override;
      vpiHandle get_ఎడమ_range() override;
      vpiHandle get_కుడి_range() override;
      __vpiScope*get_scope() స్థిరరాశి override { తిరిగి scope_; }

      పూర్ణాంకం get_మాట_కొలత() స్థిరరాశి override;
      అక్షరం*get_మాట_str(నిర్మాణం __vpiArrayWord*మాట, పూర్ణాంకం సంకేతం_2) override;
      శూన్య get_మాట_విలువ(నిర్మాణం __vpiArrayWord*మాట, p_vpi_విలువ vp) override;
      శూన్య put_మాట_విలువ(నిర్మాణం __vpiArrayWord*మాట, p_vpi_విలువ vp, పూర్ణాంకం flags) override;

      vpiHandle get_iter_సూచీ(నిర్మాణం __vpiArrayIterator*iter, పూర్ణాంకం idx) override;
      అంతఃవరుస vpiHandle vpi_iterate(పూర్ణాంకం సంకేతం_2) override { తిరిగి vpi_array_పునాది_iterate(సంకేతం_2); }

      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      అక్షరం* vpi_get_str(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_సూచీ(పూర్ణాంకం సూచీ) override;

      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;

    రక్షిత:
      vvp_darray*get_vvp_darray() స్థిరరాశి;
      __vpiDecConst ఎడమ_range, కుడి_range;
};

బాహ్య vpiHandle vpip_make_darray_var(స్థిరరాశి అక్షరం*name, vvp_net_t*net);

class __vpiQueueVar : బహిరంగ __vpiDarrayVar {

    బహిరంగ:
      __vpiQueueVar(__vpiScope*scope, స్థిరరాశి అక్షరం*name, vvp_net_t*net);

      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
};

బాహ్య vpiHandle vpip_make_వరుస_var(స్థిరరాశి అక్షరం*name, vvp_net_t*net);

class __vpiPropQueueRef : బహిరంగ __vpiHandle {

    బహిరంగ:
      explicit __vpiPropQueueRef(__vpiScope*scope, గుర్తులేని pidx, bool is_వరుస);

      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      అక్షరం* vpi_get_str(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;

      vvp_net_t* class_net_;
      గుర్తులేని prop_idx_;
      bool is_వరుస_2;

    అంతరంగ:
      __vpiScope* scope_;
};

బాహ్య vpiHandle vpip_make_prop_వరుస_ref(అక్షరం* class_label,
                                          గుర్తులేని prop_idx,
                                          గుర్తులేని is_వరుస_flag);
బాహ్య vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : బహిరంగ __vpiBaseVar {

    బహిరంగ:
      __vpiCobjectVar(__vpiScope*scope, స్థిరరాశి అక్షరం*name, vvp_net_t*net);

      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
};

బాహ్య vpiHandle vpip_make_cobject_var(స్థిరరాశి అక్షరం*name, vvp_net_t*net);

/*
 * When a loaded VPI module announces a system task/function, one
 * __vpiUserSystf object is created to hold the definition of that
 * task/function. The distinction between task and function is stored
 * in the vpi_systf_data structure data that was supplied by the
 * external module.
 *
 * When the compiler encounters a %vpi_call statement, it creates a
 * __vpiSysTaskCall to represent that particular call. The call refers
 * to the definition handle so that when the %vpi_call instruction is
 * encountered at run-time, the definition can be located and used.
 *
 * The vpiSysTaskCall handles both functions and tasks, as the two are
 * extremely similar. The different VPI type is reflected in a
 * different vpi_type pointer in the base structure. The only
 * additional part is the vbit/vwid that is used by the put of the
 * system function call to place the values in the vthread bit space.
 */
నిర్మాణం __vpiUserSystf : బహిరంగ __vpiHandle {
      __vpiUserSystf();
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;

      s_vpi_systf_దత్తాంశం info;
      bool is_user_defn;
};

బాహ్య vpiHandle vpip_make_systf_iterator(శూన్య);

బాహ్య నిర్మాణం __vpiUserSystf* vpip_కనుగొను_systf(స్థిరరాశి అక్షరం*name);


నిర్మాణం __vpiSysTaskCall : బహిరంగ __vpiHandle {

      __vpiScope* scope;
      నిర్మాణం __vpiUserSystf*defn;
      గుర్తులేని nargs;
      vpiHandle*వాదనలు;
	/* Stack consumed by this call */
      గుర్తులేని vec4_దొంతర;
      గుర్తులేని real_దొంతర;
      గుర్తులేని string_దొంతర;
	/* Support for vpi_get_userdata. */
      శూన్య*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      గుర్తులేని file_idx;
      గుర్తులేని lineno;
      bool put_విలువ;
    రక్షిత:
      అంతఃవరుస __vpiSysTaskCall()
      {
	    vec4_దొంతర = 0;
	    real_దొంతర = 0;
	    string_దొంతర = 0;
      }
};

బాహ్య నిర్మాణం __vpiSysTaskCall*vpip_cur_task;

/*
 * The persistent flag to vpip_make_string_const causes the created
 * handle to be persistent. This is necessary for cases where the
 * string handle may be reused, which is the normal case.
 *
 * When constructing with a string, the class takes possession of the
 * text value string, and will delete it in the constructor.
 */

vpiHandle vpip_make_string_const(అక్షరం*పాఠ్యం, bool persistent =true);
vpiHandle vpip_make_string_param(స్థిరరాశి అక్షరం*name, అక్షరం*విలువ, bool local_flag,
                                 దీర్ఘ file_idx, దీర్ఘ lineno);

నిర్మాణం __vpiBinaryConst : బహిరంగ __vpiHandle {
      __vpiBinaryConst();
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;

      vvp_vector4_t బిట్లు;
	/* TRUE if this constant is signed. */
      గుర్తులేని signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      గుర్తులేని sized_flag  :1;
};

vpiHandle vpip_make_ద్విమాన_const(గుర్తులేని wid, స్థిరరాశి అక్షరం*బిట్లు);
vpiHandle vpip_make_ద్విమాన_param(స్థిరరాశి అక్షరం*name, స్థిరరాశి vvp_vector4_t&బిట్లు,
				 bool signed_flag, bool local_flag,
				 దీర్ఘ file_idx, దీర్ఘ lineno);

class __vpiRealConst : బహిరంగ __vpiHandle {
    బహిరంగ:
      explicit __vpiRealConst(ద్విగుణ);
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;

      ద్విగుణ విలువ;
};

vpiHandle vpip_make_real_const(ద్విగుణ విలువ);
vpiHandle vpip_make_real_param(స్థిరరాశి అక్షరం*name, ద్విగుణ విలువ, bool local_flag,
                               దీర్ఘ file_idx, దీర్ఘ lineno);

class __vpiNullConst : బహిరంగ __vpiHandle {
    బహిరంగ:
      explicit __vpiNullConst();
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_మాట(గుర్తులేని పునాది, స్థిరరాశి అక్షరం*type);
vpiHandle vpip_make_vthr_str_దొంతర(గుర్తులేని లోతు);
vpiHandle vpip_make_vthr_vec4_దొంతర(గుర్తులేని లోతు, bool signed_flag, గుర్తులేని wid);

vpiHandle vpip_make_vthr_A(అక్షరం*label, గుర్తులేని సూచీ);
vpiHandle vpip_make_vthr_A(అక్షరం*label, అక్షరం*చిహ్నం);
vpiHandle vpip_make_vthr_A(అక్షరం*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(అక్షరం*label, గుర్తులేని సూచీ, గుర్తులేని బిట్టు, గుర్తులేని wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
బాహ్య "C" శూన్య vpip_ఎక్కించు_module(స్థిరరాశి అక్షరం*name);

బాహ్య "C" శూన్య vpip_clear_module_paths();
బాహ్య "C" శూన్య vpip_కూడు_module_మార్గం(స్థిరరాశి అక్షరం *మార్గం);
బాహ్య "C" శూన్య vpip_కూడు_env_and_default_module_paths();

/*
 * The vpip_build_vpi_call function creates a __vpiSysTaskCall object
 * and returns the handle. The compiler uses this function when it
 * encounters a %vpi_call or %vpi_func statement.
 *
 * The %vpi_call instruction has as its only parameter the handle that
 * is returned by the vpip_build_vpi_call. This includes all the
 * information needed by vpip_execute_vpi_call to actually execute the
 * call. However, the vpiSysTaskCall that is the returned handle,
 * holds a parameter argument list that is passed in here.
 *
 * The val_type and return_width fields are used if this turns out to
 * be a system function. In that case, the val_type encodes the return
 * type (-vpiRealVal, -vpiVectorVal) and if a vector the return_width
 * has the vector width.
 *
 * Note that the argv array is saved in the handle, and should should
 * not be released by the caller.
 */
బాహ్య vpiHandle vpip_build_vpi_call(స్థిరరాశి అక్షరం*name,
				     పూర్ణాంకం val_type, గుర్తులేని return_వెడల్పు,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     గుర్తులేని argc,
				     vpiHandle*argv,
				     గుర్తులేని vec4_దొంతర,
				     గుర్తులేని real_దొంతర,
				     గుర్తులేని string_దొంతర,
				     దీర్ఘ file_idx,
				     దీర్ఘ lineno);

బాహ్య vthread_t vpip_current_vthread;

బాహ్య శూన్య vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_సమయం(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

బాహ్య పూర్ణాంకం vpip_get_సమయం_precision(శూన్య);
బాహ్య శూన్య vpip_సమితి_సమయం_precision(పూర్ణాంకం pres);

బాహ్య __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

బాహ్య పూర్ణాంకం vpip_సమయం_units_from_handle(vpiHandle obj);
బాహ్య పూర్ణాంకం vpip_సమయం_precision_from_handle(vpiHandle obj);

బాహ్య ద్విగుణ vpip_scaled_సమయం_from_handle(vvp_time64_t సమయం, vpiHandle obj);

బాహ్య శూన్య vpip_సమయం_గమ్యం_timestruct(నిర్మాణం t_vpi_సమయం*ts, vvp_time64_t ti);
బాహ్య vvp_time64_t vpip_timestruct_గమ్యం_సమయం(స్థిరరాశి నిర్మాణం t_vpi_సమయం*ts);

బాహ్య ద్విగుణ vpip_సమయం_గమ్యం_scaled_real(vvp_time64_t ti, స్థిరరాశి __vpiScope*sc);
బాహ్య vvp_time64_t vpip_scaled_real_గమ్యం_time64(ద్విగుణ val, స్థిరరాశి __vpiScope*sc);

/*
 * These functions are used mostly as compile time to strings into
 * permallocated memory. The vpip_string function is the most general,
 * it allocates a fresh string no matter what. The vpip_name_string
 * allocates a string and keeps a pointer in the hash, and tries to
 * reuse it if it can. This us useful for handle names, which may be
 * reused in different scopes.
 */
బాహ్య స్థిరరాశి అక్షరం* vpip_string(స్థిరరాశి అక్షరం*str);
బాహ్య స్థిరరాశి అక్షరం* vpip_name_string(స్థిరరాశి అక్షరం*str);


/*
 * This function is used to make decimal string versions of various
 * vectors. The input format is a vvp_vector4_t, and the result is
 * written into buf, without overflowing nbuf.
 */
బాహ్య గుర్తులేని vpip_vec4_గమ్యం_dec_str(స్థిరరాశి vvp_vector4_t&vec4,
				     అక్షరం *buf, గుర్తులేని పూర్ణాంకం nbuf,
				     పూర్ణాంకం signed_flag);


బాహ్య శూన్య vpip_vec4_గమ్యం_hex_str(స్థిరరాశి vvp_vector4_t&బిట్లు, అక్షరం*buf,
				 గుర్తులేని nbuf);

బాహ్య శూన్య vpip_vec4_గమ్యం_oct_str(స్థిరరాశి vvp_vector4_t&బిట్లు, అక్షరం*buf,
				 గుర్తులేని nbuf);

బాహ్య శూన్య vpip_bin_str_గమ్యం_vec4(vvp_vector4_t&val, స్థిరరాశి అక్షరం*buf);
బాహ్య శూన్య vpip_oct_str_గమ్యం_vec4(vvp_vector4_t&val, స్థిరరాశి అక్షరం*str);
బాహ్య శూన్య vpip_dec_str_గమ్యం_vec4(vvp_vector4_t&val, స్థిరరాశి అక్షరం*str);
బాహ్య శూన్య vpip_hex_str_గమ్యం_vec4(vvp_vector4_t&val, స్థిరరాశి అక్షరం*str);

బాహ్య vvp_vector4_t vec4_from_vpi_విలువ(s_vpi_విలువ*vp, గుర్తులేని wid);
బాహ్య ద్విగుణ real_from_vpi_విలువ(s_vpi_విలువ*vp);

బాహ్య శూన్య vpip_vec4_get_విలువ(స్థిరరాశి vvp_vector4_t&మాట_val, గుర్తులేని వెడల్పు,
				bool signed_flag, s_vpi_విలువ*vp);
బాహ్య శూన్య vpip_vec2_get_విలువ(స్థిరరాశి vvp_vector2_t&మాట_val, గుర్తులేని వెడల్పు,
				bool signed_flag, s_vpi_విలువ*vp);
బాహ్య శూన్య vpip_real_get_విలువ(ద్విగుణ real, s_vpi_విలువ*vp);
బాహ్య శూన్య vpip_string_get_విలువ(స్థిరరాశి std::string&val, s_vpi_విలువ*vp);

/*
 * Function defined in vpi_signal.cc to manage vpi_get_* persistent
 * storage.
 */
గణన vpi_rbuf_t {
      RBUF_VAL =0,
	/* Storage for *_get_value() */
      RBUF_STR,
	/* Storage for *_get_str() */
      RBUF_DEL
	/* Delete the storage for both buffers. */
};
బాహ్య శూన్య *need_ఫలితం_buf(గుర్తులేని cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
బాహ్య అక్షరం *simple_సమితి_rbuf_str(స్థిరరాశి అక్షరం *s1);
బాహ్య అక్షరం *generic_get_str(పూర్ణాంకం సంకేతం_2, vpiHandle ref, స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *సూచీ);

/* A routine to find the enclosing module. */
బాహ్య vpiHandle vpip_module(__vpiScope*scope);

బాహ్య పూర్ణాంకం vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
బాహ్య bool vpip_mcd0_disable;

అంతఃవరుస uint64_t vlg_విడత_గమ్యం_u64(ద్విగుణ rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      తిరిగి static_cast<uint64_t>(static_cast<int64_t>(std::విడత_2(rval)));
}

#అయితే_ముగింపు /* IVL_vpi_priv_H */
