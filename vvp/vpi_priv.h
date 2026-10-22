#اگر_غیر_تعریف_شدہ IVL_vpi_priv_H
#تعریف_کرو IVL_vpi_priv_H
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
#شامل  "delay.h"


class class_type;
class vvp_darray;
class vvp_fun_arrayport;

قسمتعریف ساخت __vpiArray* vvp_array_t;

/*
 * This header file contains the internal definitions that the vvp
 * program uses to implement the public interface in the vpi_user.h
 * header file elsewhere.
 */

#اگر defined(__MINGW32__) || defined (__CYGWIN__)
خارجی vpip_routines_s vpi_routines;
#اگر_ختم

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#تعریف_کرو vpi_File_سطر    0x1000003
#تعریف_کرو _vpiDescription 0x1000004

خارجی bool show_file_سطر;
خارجی bool رمز_is_instrumented;

خارجی vpiHandle vpip_build_file_سطر(ثابت حرف*description,
                                      طویل file_idx, طویل lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
خارجی ثابت حرف* vpi_type_as_string(PLI_INT32 رمز);

/*
 * Return the port direction as a string.
 */
خارجی ثابت حرف* direction_as_string(صحیح dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#اگر defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
#تعریف_کرو _vpiFromThr 0x1000001
#   define _vpiNoThr   0
#   define _vpiString  1
#   define _vpiVThr    2
#   define vpi_لفظ    3
#   define _vpi_at_PV  4
#   define _vpi_at_A   5
#   define _vpi_at_APV 6
#اگر_ختم


/*
 * The vpi_mode_flag contains the major mode for VPI use. This is used
 * to generate error messages when vpi functions are called
 * incorrectly.
 */
شمار vpi_منوال_t {
      VPI_منوال_NONE =0,
	/* The compiler is calling a register function. */
      VPI_منوال_رجسٹر,
	/* The compiler is calling a compiletf function. */
      VPI_منوال_COMPILETF,
	/* The compiler is calling a calltf function. */
      VPI_منوال_CALLTF,
	/* We are in the midst of a RWSync callback. */
      VPI_منوال_RWSYNC,
	/* We are in a ROSync callback. */
      VPI_منوال_ROSYNC
};
خارجی vpi_منوال_t vpi_منوال_flag;

/*
 * This structure is the very base of a vpiHandle. Every handle
 * structure is derived from this class so that the library can
 * internally pass the derived types as pointers to one of these.
 */
class __vpiHandle {
    عوامی:
      اندرونی __vpiHandle() { }
	// The destructor is virtual so that dynamic types will work.
      مجازی ~__vpiHandle();

      مجازی صحیح get_type_رمز(خالی) ثابت =0;
      مجازی صحیح vpi_get(صحیح رمز);
      مجازی int64_t vpi_get64(صحیح رمز);
      مجازی حرف* vpi_get_str(صحیح رمز);

      مجازی خالی vpi_get_قدر(p_vpi_قدر val);
      مجازی vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags);
      مجازی vpiHandle vpi_handle(صحیح رمز);
      مجازی vpiHandle vpi_iterate(صحیح رمز);
      مجازی vpiHandle vpi_اشاریہ(صحیح idx);
      مجازی خالی vpi_get_delays(p_vpi_delay del);
      مجازی خالی vpi_put_delays(p_vpi_delay del);

	// Objects may have destroyer functions of their own. If so,
	// then this virtual method will return a POINTER to that
	// function. The pointer is used to "delete" the object, which
	// is why the function itself cannot be a method.
      قسمتعریف صحیح (*free_object_fun_t)(vpiHandle);
      مجازی free_object_fun_t free_object_fun(خالی);
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
ساخت __vpiIterator : عوامی __vpiHandle {
      __vpiIterator();
      صحیح get_type_رمز(خالی) ثابت override;
      free_object_fun_t free_object_fun(خالی) override;

      vpiHandle *دلائل;
      غیرعلامتی  nargs;
      غیرعلامتی  اگلا_2;
      bool free_دلائل_flag;
};

خارجی vpiHandle vpip_make_iterator(غیرعلامتی nargs, vpiHandle*دلائل,
				    bool free_دلائل_flag);

class __vpiDecConst : عوامی __vpiHandle {
    عوامی:
      explicit __vpiDecConst(صحیح val =0);
      __vpiDecConst(ثابت __vpiDecConst&that);
# اگر __cplusplus >= 201103L
      __vpiDecConst& عامل = (ثابت __vpiDecConst&that) = طےشدہ;
# endif
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;

    عوامی:
      اندرونی صحیح get_قدر() ثابت { واپس قدر; }
      اندرونی خالی سیٹ_قدر(صحیح val) { قدر = val; }

    نجی:
      صحیح قدر;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
ساخت __vpiCallback : عوامی __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      صحیح get_type_رمز(خالی) ثابت override;

	// Used for listing callbacks.
      ساخت __vpiCallback*اگلا_2;

	// user supplied callback data
      ساخت t_cb_ڈیٹا cb_ڈیٹا;
};

class قدر_callback : عوامی __vpiCallback {
    عوامی:
      explicit قدر_callback(p_cb_ڈیٹا ڈیٹا);
	// Return true if the callback really is ready to be called
      مجازی bool test_قدر_callback_ready(خالی);

    عوامی:
	// user supplied callback data
      ساخت t_vpi_وقت cb_وقت;
      ساخت t_vpi_قدر cb_قدر;
};

خارجی خالی callback_execute(ساخت __vpiCallback*cur);

ساخت __vpiSystemTime : عوامی __vpiHandle {
      __vpiSystemTime();
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح رمز) override;
      حرف*vpi_get_str(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_handle(صحیح رمز) override;

      __vpiScope*scope;
};

ساخت __vpiScopedTime : عوامی __vpiSystemTime {
      __vpiScopedTime();
      حرف*vpi_get_str(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
};
ساخت __vpiScopedSTime : عوامی __vpiSystemTime {
      __vpiScopedSTime();
      صحیح vpi_get(صحیح رمز) override;
      حرف*vpi_get_str(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
};
ساخت __vpiScopedRealtime : عوامی __vpiSystemTime {
      __vpiScopedRealtime();
      صحیح vpi_get(صحیح رمز) override;
      حرف*vpi_get_str(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : عوامی __vpiHandle {

    عوامی:
      صحیح vpi_get(صحیح رمز) override;
      حرف* vpi_get_str(صحیح رمز) override;
      vpiHandle vpi_handle(صحیح رمز) override;
      vpiHandle vpi_iterate(صحیح رمز) override;

    عوامی:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      اندرونی ثابت حرف*scope_name() ثابت { واپس name_; }

      اندرونی ثابت حرف*scope_def_name() ثابت { واپس tname_; }
	// TRUE if this is an automatic func/task/block
      اندرونی bool is_automatic() ثابت { واپس is_automatic_; }

    عوامی:
      __vpiScope *scope;
      غیرعلامتی file_idx;
      غیرعلامتی lineno;
      غیرعلامتی def_file_idx;
      غیرعلامتی def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_وقت;
      ساخت __vpiScopedSTime scoped_stime;
      ساخت __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::سمتیہ<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      ساخت automatic_hooks_s**شے;
      غیرعلامتی nitem;
        /* Keep a list of live contexts. */
      vvp_context_t زندہ_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::سیٹ<vthread_t> دھاگے;
      علامتی صحیح وقت_units :8;
      علامتی صحیح وقت_precision :8;

    محفوظ:
      __vpiScope(ثابت حرف*nam, ثابت حرف*tnam, bool is_auto_flag =false);

    نجی:
	/* The scope has a name. */
      ثابت حرف*name_;
      ثابت حرف*tname_;
	/* the scope may be "automatic" */
      bool is_automatic_;
};

class vpiScopeFunction  : عوامی __vpiScope {
    عوامی:
      اندرونی vpiScopeFunction(ثابت حرف*nam, ثابت حرف*tnam,
			      bool auto_flag, صحیح func_type, غیرعلامتی func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      صحیح get_type_رمز(خالی) ثابت override { واپس vpiFunction; }
      صحیح vpi_get(صحیح رمز) override
      {
	    انتخاب (رمز) {
		حالت vpiFuncType:
		  واپس func_type_;
		طےشدہ:
		  واپس __vpiScope::vpi_get(رمز);
	    }
      }

    عوامی:
      اندرونی غیرعلامتی get_func_چوڑائی(خالی) ثابت { واپس func_wid_; }
      اندرونی vvp_bit4_t get_func_init_val(خالی) ثابت { واپس func_init_val_; }

    نجی:
      صحیح func_type_;
      غیرعلامتی func_wid_;
      vvp_bit4_t func_init_val_;
};

خارجی __vpiScope* vpip_peek_current_scope(خالی);
خارجی خالی vpip_attach_مقصد_scope(__vpiScope*scope, vpiHandle obj);
خارجی خالی vpip_attach_مقصد_current_scope(vpiHandle obj);
خارجی __vpiScope* vpip_peek_context_scope(خالی);
خارجی غیرعلامتی vpip_جوڑو_شے_مقصد_context(automatic_hooks_s*شے,
                                         __vpiScope*scope);
خارجی vpiHandle vpip_make_جڑ_iterator(صحیح type_رمز);
خارجی خالی vpip_make_جڑ_iterator(class __vpiHandle**&جدول,
				    غیرعلامتی&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
ساخت __vpiSignal : عوامی __vpiHandle {
      صحیح vpi_get(صحیح رمز) override;
      int64_t vpi_get64(صحیح رمز) override;
      حرف* vpi_get_str(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags) override;
      vpiHandle vpi_handle(صحیح رمز) override;
      vpiHandle vpi_iterate(صحیح رمز) override;
      vpiHandle vpi_اشاریہ(صحیح idx) override;

    عوامی:
      غیرعلامتی چوڑائی() ثابت;
      vpiHandle get_اشاریہ(صحیح اشاریہ);
      خالی get_ثنائی_ہندسہ_قدر(ثابت ساخت __vpiBit*ثنائی_ہندسہ, p_vpi_قدر vp);
      vpiHandle put_ثنائی_ہندسہ_قدر(ساخت __vpiBit*ثنائی_ہندسہ, p_vpi_قدر vp, صحیح flags);
      خالی make_بٹ();

      ساخت __vpiBit*بٹ;

    عوامی:
      اتحاد { // The scope or parent array that contains me.
	    vpiHandle والد;
	    __vpiScope* scope;
      } within;
      اتحاد { // The name of this reg/net, or the index for array words.
            ثابت حرف*name;
            vpiHandle اشاریہ;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      غیرعلامتی signed_flag  : 1;
      غیرعلامتی is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*گرہ;

    عوامی:
      ساکن خالی*عامل new(std::size_t سائز);
      ساکن خالی عامل delete(خالی*); // not implemented
    محفوظ:
      اندرونی __vpiSignal() : بٹ(تہی) { }
    نجی: // Not implemented
      ساکن خالی*عامل new[] (std::size_t سائز);
      ساکن خالی عامل delete[](خالی*);
};
خارجی غیرعلامتی vpip_سائز(ثابت __vpiSignal *sig);
خارجی __vpiScope* vpip_scope(__vpiSignal*sig);

خارجی vpiHandle vpip_make_int2(ثابت حرف*name, صحیح msb, صحیح lsb,
			       bool signed_flag, vvp_net_t*vec);
خارجی vpiHandle vpip_make_int4(ثابت حرف*name, صحیح msb, صحیح lsb,
			       vvp_net_t*vec);
خارجی vpiHandle vpip_make_var4(ثابت حرف*name, صحیح msb, صحیح lsb,
			       bool signed_flag, vvp_net_t*net);
خارجی vpiHandle vpip_make_net4(__vpiScope*scope,
				ثابت حرف*name, صحیح msb, صحیح lsb,
				bool signed_flag, vvp_net_t*گرہ);

/*
 * This is used to represent a bit in a net/reg.
 */
ساخت __vpiBit {
      ساخت as_ثنائی_ہندسہ_t : عوامی __vpiHandle {
	    صحیح get_type_رمز(خالی) ثابت;
	    صحیح vpi_get(صحیح رمز);
	    حرف* vpi_get_str(صحیح رمز);
	    خالی vpi_get_قدر(p_vpi_قدر val);
	    vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags);
	    vpiHandle vpi_handle(صحیح رمز);
      } as_ثنائی_ہندسہ;

      vpiHandle اشاریہ;

      اتحاد {
	    ساخت __vpiSignal*والد;
	    ساخت __vpiBit*bit0;
      };

      اندرونی غیرعلامتی get_norm_اشاریہ() ثابت { واپس this - bit0; }
      اندرونی ساخت __vpiSignal*get_والد() ثابت {واپس (bit0 - 1)->والد; }
      صحیح get_اشاریہ(خالی) ثابت;
};

class vpi_پورٹ_ثنائی_ہندسہ_Info;

class vpi_پورٹ_Info  : عوامی __vpiHandle {
    عوامی:
      vpi_پورٹ_Info( __vpiScope *والد,
                    غیرعلامتی اشاریہ,
                    صحیح vpi_direction,
                    غیرعلامتی چوڑائی,
                    ثابت حرف *name,
                    حرف* بفر );
      ~vpi_پورٹ_Info() override;

      صحیح get_type_رمز(خالی) ثابت override { واپس vpi_پورٹ; }
      صحیح get_direction(خالی) { واپس direction_; }
      غیرعلامتی get_اشاریہ(خالی) { واپس اشاریہ_2; }
      صحیح get_چوڑائی(خالی) { واپس چوڑائی_2; }
      خالی جوڑو_پورٹ_ثنائی_ہندسہ(vpi_پورٹ_ثنائی_ہندسہ_Info* پورٹ_ثنائی_ہندسہ) { پورٹ_بٹ.ڈالو_عقب(پورٹ_ثنائی_ہندسہ); }

      صحیح vpi_get(صحیح رمز) override;
      حرف* vpi_get_str(صحیح رمز) override;
      vpiHandle vpi_handle(صحیح رمز) override;
      vvp_net_t* get_پورٹ(خالی) ثابت { واپس ref_; }
      vpiHandle vpi_iterate(صحیح رمز) override;

      std::سمتیہ<vpi_پورٹ_ثنائی_ہندسہ_Info*> پورٹ_بٹ;

    نجی:
      __vpiScope *والد_2;
      غیرعلامتی  اشاریہ_2;
      صحیح       direction_;
      غیرعلامتی  چوڑائی_2;
      ثابت حرف *name_;
      vvp_net_t *ref_;
};

class vpi_پورٹ_ثنائی_ہندسہ_Info  : عوامی __vpiHandle {
    عوامی:
      vpi_پورٹ_ثنائی_ہندسہ_Info(vpi_پورٹ_Info *والد,
                     غیرعلامتی ثنائی_ہندسہ);
      ~vpi_پورٹ_ثنائی_ہندسہ_Info() override;

      صحیح get_type_رمز(خالی) ثابت override { واپس vpi_پورٹ_ثنائی_ہندسہ; }
      غیرعلامتی get_ثنائی_ہندسہ(خالی) ثابت { واپس ثنائی_ہندسہ_2; }

      صحیح vpi_get(صحیح رمز) override;
      vpiHandle vpi_handle(صحیح رمز) override;

    نجی:
      vpi_پورٹ_Info *والد_2;
      غیرعلامتی ثنائی_ہندسہ_2;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
ساخت __vpiPV : عوامی __vpiHandle {
      __vpiPV();
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح رمز) override;
      حرف* vpi_get_str(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags) override;
      vpiHandle vpi_handle(صحیح رمز) override;

      vpiHandle والد;
      vvp_net_t*net;
      vpiHandle sbase;
      صحیح tbase;
      غیرعلامتی چوڑائی;
};
خارجی vpiHandle vpip_make_PV(حرف*name, صحیح بنیاد, صحیح چوڑائی);
خارجی vpiHandle vpip_make_PV(حرف*name, حرف*علامت, صحیح چوڑائی);
خارجی vpiHandle vpip_make_PV(حرف*name, vpiHandle handle, صحیح چوڑائی);

ساخت __vpiModPathTerm : عوامی __vpiHandle {
      __vpiModPathTerm();
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح رمز) override;
      vpiHandle vpi_handle(صحیح رمز) override;

      vpiHandle اظہاریہ;
	/* The value returned by vpi_get(vpiEdge, ...); */
      صحیح کنارہ;
};

ساخت __vpiModPathSrc : عوامی __vpiHandle {
      __vpiModPathSrc();
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags) override;
      vpiHandle vpi_handle(صحیح رمز) override;
      vpiHandle vpi_iterate(صحیح رمز) override;
      vpiHandle vpi_اشاریہ(صحیح idx) override;
      خالی vpi_get_delays(p_vpi_delay del) override;
      خالی vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(خالی) override;

      ساخت __vpiModPath *dest;
      صحیح   type;

	/* This is the input expression for this modpath. */
      ساخت __vpiModPathTerm راستہ_رکن_اندر;

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

ساخت __vpiModPath {
      __vpiScope   *scope ;

      class vvp_fun_modpath*modpath;

      ساخت __vpiModPathTerm راستہ_رکن_برآمد;
      vvp_net_t *داخلہ_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

خارجی ساخت __vpiModPathSrc* vpip_make_modpath_src(ساخت __vpiModPath*راستہ,
                                                     vvp_net_t *net) ;

خارجی ساخت __vpiModPath* vpip_make_modpath(vvp_net_t *net) ;

/*
 *
 * The vpiInterModPath vpiHandle will define
 * a vpiInterModPath of record .intermodpath as defined
 * in the IEEE 1364
 *
 */

ساخت __vpiInterModPath : عوامی __vpiHandle {
      __vpiInterModPath();
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags) override;
      vpiHandle vpi_handle(صحیح رمز) override;
      vpiHandle vpi_iterate(صحیح رمز) override;
      خالی vpi_get_delays(p_vpi_delay del) override;
      خالی vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(خالی) override;

      __vpiScope   *scope ;

      class vvp_fun_intermodpath*intermodpath;

      vvp_net_t *net;

      vpi_پورٹ_Info* port1;
      vpi_پورٹ_Info* port2;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiInterModPath
 */

خارجی ساخت __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpi_پورٹ_Info* port1, vpi_پورٹ_Info* port2);

/*
 * These methods support the vpi creation of events. The name string
 * passed in will be saved, so the caller must allocate it (or not
 * free it) after it is handed to this function.
 */
class __vpiNamedEvent : عوامی __vpiHandle {

    عوامی:
      __vpiNamedEvent(__vpiScope*scope, ثابت حرف*name);
      ~__vpiNamedEvent() override;

      __vpiNamedEvent(ثابت __vpiNamedEvent&) = delete;
      __vpiNamedEvent& عامل=(ثابت __vpiNamedEvent&) = delete;

      صحیح get_type_رمز(خالی) ثابت override;
      __vpiScope*get_scope(خالی) ثابت { واپس scope_; }
      صحیح vpi_get(صحیح رمز) override;
      حرف* vpi_get_str(صحیح رمز) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags) override;
      vpiHandle vpi_handle(صحیح رمز) override;

      اندرونی خالی جوڑو_vpi_callback(__vpiCallback*cb)
      { cb->اگلا_2 = callbacks_;
	callbacks_ = cb;
      }

      خالی تسلسل_vpi_callbacks(خالی);

	/* The functor, used for %set operations. */
      vvp_net_t*funct;

    نجی:
	/* base name of the event object */
      ثابت حرف*name_;
	/* Parent scope of this object. */
      __vpiScope*scope_;
	/* List of callbacks interested in this event. */
      __vpiCallback*callbacks_;
};

خارجی vpiHandle vpip_make_named_event(ثابت حرف*name, vvp_net_t*f);

/*
 * Memory is an array of bits that is accessible in N-bit chunks, with
 * N being the width of a word. The memory word handle just points
 * back to the memory and uses an index to identify its position in
 * the memory.
 */

خارجی bool is_net_array(vpiHandle obj);

/*
 * These are the various variable types.
 */
ساخت __vpiRealVar : عوامی __vpiHandle {
      __vpiRealVar();
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح رمز) override;
      حرف* vpi_get_str(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags) override;
      vpiHandle vpi_handle(صحیح رمز) override;
      vpiHandle vpi_iterate(صحیح رمز) override;

      اتحاد { // The scope or parent array that contains me.
	    vpiHandle والد;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      اتحاد {
            ثابت حرف*name;
            vpiHandle اشاریہ;
      } id;
      غیرعلامتی is_netarray  : 1; // This is word of a net array
      غیرعلامتی is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

خارجی __vpiScope* vpip_scope(__vpiRealVar*sig);
خارجی vpiHandle vpip_make_real_var(ثابت حرف*name, vvp_net_t*net);
خارجی vpiHandle vpip_make_real_net(__vpiScope*scope,
				    ثابت حرف*name, vvp_net_t*net);

class __vpiBaseVar : عوامی __vpiHandle {

    عوامی:
      __vpiBaseVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#اگر_ختم

      اندرونی vvp_net_t* get_net() ثابت { واپس net_; }

    محفوظ:
      __vpiScope* scope_;
      ثابت حرف*name_;

    نجی:
      vvp_net_t*net_;
};

class __vpiStringVar : عوامی __vpiBaseVar {

    عوامی:
      __vpiStringVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags) override;
};

خارجی vpiHandle vpip_make_string_var(ثابت حرف*name, vvp_net_t*net);

ساخت __vpiArrayBase {
      __vpiArrayBase() : vals_الفاظ(تہی) {}
      مجازی ~__vpiArrayBase() {}

      مجازی غیرعلامتی get_سائز(خالی) ثابت = 0;
      مجازی vpiHandle get_بایاں_range() = 0;
      مجازی vpiHandle get_دایاں_range() = 0;
      مجازی __vpiScope*get_scope() ثابت = 0;

      مجازی صحیح get_لفظ_سائز() ثابت = 0;
      مجازی حرف*get_لفظ_str(ساخت __vpiArrayWord*لفظ, صحیح رمز) = 0;
      مجازی خالی get_لفظ_قدر(ساخت __vpiArrayWord*لفظ, p_vpi_قدر vp) = 0;
      مجازی خالی put_لفظ_قدر(ساخت __vpiArrayWord*لفظ, p_vpi_قدر vp,
                                    صحیح flags) = 0;
      مجازی vpiHandle get_iter_اشاریہ(ساخت __vpiArrayIterator*iter, صحیح idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_بنیاد_iterate(صحیح رمز);

      مجازی خالی make_vals_الفاظ();

      ساخت __vpiArrayWord*vals_الفاظ;
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
ساخت __vpiArray : عوامی __vpiArrayBase, عوامی __vpiHandle {
      صحیح get_type_رمز(خالی) ثابت override { واپس vpi_حافظہ; }
      غیرعلامتی get_سائز() ثابت override { واپس array_گنتی; }
      vpiHandle get_بایاں_range() override { أكد(nets == 0); واپس &msb; }
      vpiHandle get_دایاں_range() override { أكد(nets == 0); واپس &lsb; }
      __vpiScope*get_scope() ثابت override { واپس scope; }

      صحیح get_لفظ_سائز() ثابت override;
      حرف*get_لفظ_str(ساخت __vpiArrayWord*لفظ, صحیح رمز) override;
      خالی get_لفظ_قدر(ساخت __vpiArrayWord*لفظ, p_vpi_قدر vp) override;
      خالی put_لفظ_قدر(ساخت __vpiArrayWord*لفظ, p_vpi_قدر vp, صحیح flags) override;

      vpiHandle get_iter_اشاریہ(ساخت __vpiArrayIterator*iter, صحیح idx) override;

      صحیح vpi_get(صحیح رمز) override;
      حرف* vpi_get_str(صحیح رمز) override;
      vpiHandle vpi_handle(صحیح رمز) override;
      اندرونی vpiHandle vpi_iterate(صحیح رمز) override { واپس vpi_array_بنیاد_iterate(رمز); }
      vpiHandle vpi_اشاریہ(صحیح idx) override;

      خالی سیٹ_لفظ(غیرعلامتی idx, غیرعلامتی off, ثابت vvp_vector4_t&val);
      خالی سیٹ_لفظ(غیرعلامتی idx, دوہرا val);
      خالی سیٹ_لفظ(غیرعلامتی idx, ثابت std::string&val);
      خالی سیٹ_لفظ(غیرعلامتی idx, ثابت vvp_object_t&val);

      vvp_vector4_t get_لفظ(غیرعلامتی پتہ);
      دوہرا get_لفظ_r(غیرعلامتی پتہ);
      خالی get_لفظ_obj(غیرعلامتی پتہ, vvp_object_t&val);
      std::string get_لفظ_str(غیرعلامتی پتہ);

      خالی alias_لفظ(غیرعلامتی طویل addr, vpiHandle لفظ, صحیح msb, صحیح lsb);
      خالی attach_لفظ(غیرعلامتی addr, vpiHandle لفظ);
      خالی لفظ_change(غیرعلامتی طویل addr);

      ثابت حرف*name; /* Permanently allocated string */
      __vpiDecConst پہلا_addr;
      __vpiDecConst آخری_addr;
      __vpiDecConst msb;
      __vpiDecConst lsb;
      غیرعلامتی vals_چوڑائی;
	// If this is a net array, nets lists the handles.
      vpiHandle*nets;
	// If this is a var array, then these are used instead of nets.
      vvp_vector4array_t*vals4;
      vvp_darray        *vals;

      vvp_fun_arrayport*ports_;
      ساخت __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool تبادلہ_addr;

نجی:
      غیرعلامتی array_گنتی;
      __vpiScope*scope;

دوست vpiHandle vpip_make_array(ثابت حرف*label, ثابت حرف*name,
                                 صحیح پہلا_addr, صحیح آخری_addr,
                                 bool signed_flag);
دوست خالی compile_array_alias(حرف*label, حرف*name, حرف*src);
};

class __vpiDarrayVar : عوامی __vpiBaseVar, عوامی __vpiArrayBase {
    عوامی:
      __vpiDarrayVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحیح get_type_رمز() ثابت override { واپس vpiArrayVar; }
      غیرعلامتی get_سائز() ثابت override;
      vpiHandle get_بایاں_range() override;
      vpiHandle get_دایاں_range() override;
      __vpiScope*get_scope() ثابت override { واپس scope_; }

      صحیح get_لفظ_سائز() ثابت override;
      حرف*get_لفظ_str(ساخت __vpiArrayWord*لفظ, صحیح رمز) override;
      خالی get_لفظ_قدر(ساخت __vpiArrayWord*لفظ, p_vpi_قدر vp) override;
      خالی put_لفظ_قدر(ساخت __vpiArrayWord*لفظ, p_vpi_قدر vp, صحیح flags) override;

      vpiHandle get_iter_اشاریہ(ساخت __vpiArrayIterator*iter, صحیح idx) override;
      اندرونی vpiHandle vpi_iterate(صحیح رمز) override { واپس vpi_array_بنیاد_iterate(رمز); }

      صحیح vpi_get(صحیح رمز) override;
      حرف* vpi_get_str(صحیح رمز) override;
      vpiHandle vpi_handle(صحیح رمز) override;
      vpiHandle vpi_اشاریہ(صحیح اشاریہ) override;

      خالی vpi_get_قدر(p_vpi_قدر val) override;

    محفوظ:
      vvp_darray*get_vvp_darray() ثابت;
      __vpiDecConst بایاں_range, دایاں_range;
};

خارجی vpiHandle vpip_make_darray_var(ثابت حرف*name, vvp_net_t*net);

class __vpiQueueVar : عوامی __vpiDarrayVar {

    عوامی:
      __vpiQueueVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحیح vpi_get(صحیح رمز) override;
};

خارجی vpiHandle vpip_make_قطار_var(ثابت حرف*name, vvp_net_t*net);

class __vpiPropQueueRef : عوامی __vpiHandle {

    عوامی:
      explicit __vpiPropQueueRef(__vpiScope*scope, غیرعلامتی pidx, bool is_قطار);

      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح رمز) override;
      حرف* vpi_get_str(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;

      vvp_net_t* class_net_;
      غیرعلامتی prop_idx_;
      bool is_قطار_2;

    نجی:
      __vpiScope* scope_;
};

خارجی vpiHandle vpip_make_prop_قطار_ref(حرف* class_label,
                                          غیرعلامتی prop_idx,
                                          غیرعلامتی is_قطار_flag);
خارجی vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : عوامی __vpiBaseVar {

    عوامی:
      __vpiCobjectVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
};

خارجی vpiHandle vpip_make_cobject_var(ثابت حرف*name, vvp_net_t*net);

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
ساخت __vpiUserSystf : عوامی __vpiHandle {
      __vpiUserSystf();
      صحیح get_type_رمز(خالی) ثابت override;

      s_vpi_systf_ڈیٹا info;
      bool is_user_defn;
};

خارجی vpiHandle vpip_make_systf_iterator(خالی);

خارجی ساخت __vpiUserSystf* vpip_ڈھونڈو_systf(ثابت حرف*name);


ساخت __vpiSysTaskCall : عوامی __vpiHandle {

      __vpiScope* scope;
      ساخت __vpiUserSystf*defn;
      غیرعلامتی nargs;
      vpiHandle*دلائل;
	/* Stack consumed by this call */
      غیرعلامتی vec4_ڈھیر;
      غیرعلامتی real_ڈھیر;
      غیرعلامتی string_ڈھیر;
	/* Support for vpi_get_userdata. */
      خالی*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      غیرعلامتی file_idx;
      غیرعلامتی lineno;
      bool put_قدر;
    محفوظ:
      اندرونی __vpiSysTaskCall()
      {
	    vec4_ڈھیر = 0;
	    real_ڈھیر = 0;
	    string_ڈھیر = 0;
      }
};

خارجی ساخت __vpiSysTaskCall*vpip_cur_task;

/*
 * The persistent flag to vpip_make_string_const causes the created
 * handle to be persistent. This is necessary for cases where the
 * string handle may be reused, which is the normal case.
 *
 * When constructing with a string, the class takes possession of the
 * text value string, and will delete it in the constructor.
 */

vpiHandle vpip_make_string_const(حرف*متن, bool persistent =true);
vpiHandle vpip_make_string_param(ثابت حرف*name, حرف*قدر, bool local_flag,
                                 طویل file_idx, طویل lineno);

ساخت __vpiBinaryConst : عوامی __vpiHandle {
      __vpiBinaryConst();
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;

      vvp_vector4_t بٹ;
	/* TRUE if this constant is signed. */
      غیرعلامتی signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      غیرعلامتی sized_flag  :1;
};

vpiHandle vpip_make_ثنائی_const(غیرعلامتی wid, ثابت حرف*بٹ);
vpiHandle vpip_make_ثنائی_param(ثابت حرف*name, ثابت vvp_vector4_t&بٹ,
				 bool signed_flag, bool local_flag,
				 طویل file_idx, طویل lineno);

class __vpiRealConst : عوامی __vpiHandle {
    عوامی:
      explicit __vpiRealConst(دوہرا);
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;

      دوہرا قدر;
};

vpiHandle vpip_make_real_const(دوہرا قدر);
vpiHandle vpip_make_real_param(ثابت حرف*name, دوہرا قدر, bool local_flag,
                               طویل file_idx, طویل lineno);

class __vpiNullConst : عوامی __vpiHandle {
    عوامی:
      explicit __vpiNullConst();
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح رمز) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_لفظ(غیرعلامتی بنیاد, ثابت حرف*type);
vpiHandle vpip_make_vthr_str_ڈھیر(غیرعلامتی گہرائی);
vpiHandle vpip_make_vthr_vec4_ڈھیر(غیرعلامتی گہرائی, bool signed_flag, غیرعلامتی wid);

vpiHandle vpip_make_vthr_A(حرف*label, غیرعلامتی اشاریہ);
vpiHandle vpip_make_vthr_A(حرف*label, حرف*علامت);
vpiHandle vpip_make_vthr_A(حرف*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(حرف*label, غیرعلامتی اشاریہ, غیرعلامتی ثنائی_ہندسہ, غیرعلامتی wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
خارجی "C" خالی vpip_بھرو_module(ثابت حرف*name);

خارجی "C" خالی vpip_clear_module_paths();
خارجی "C" خالی vpip_جوڑو_module_راستہ(ثابت حرف *راستہ);
خارجی "C" خالی vpip_جوڑو_env_and_default_module_paths();

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
خارجی vpiHandle vpip_build_vpi_call(ثابت حرف*name,
				     صحیح val_type, غیرعلامتی return_چوڑائی,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     غیرعلامتی argc,
				     vpiHandle*argv,
				     غیرعلامتی vec4_ڈھیر,
				     غیرعلامتی real_ڈھیر,
				     غیرعلامتی string_ڈھیر,
				     طویل file_idx,
				     طویل lineno);

خارجی vthread_t vpip_current_vthread;

خارجی خالی vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_وقت(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

خارجی صحیح vpip_get_وقت_precision(خالی);
خارجی خالی vpip_سیٹ_وقت_precision(صحیح pres);

خارجی __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

خارجی صحیح vpip_وقت_units_from_handle(vpiHandle obj);
خارجی صحیح vpip_وقت_precision_from_handle(vpiHandle obj);

خارجی دوہرا vpip_scaled_وقت_from_handle(vvp_time64_t وقت, vpiHandle obj);

خارجی خالی vpip_وقت_مقصد_timestruct(ساخت t_vpi_وقت*ts, vvp_time64_t ti);
خارجی vvp_time64_t vpip_timestruct_مقصد_وقت(ثابت ساخت t_vpi_وقت*ts);

خارجی دوہرا vpip_وقت_مقصد_scaled_real(vvp_time64_t ti, ثابت __vpiScope*sc);
خارجی vvp_time64_t vpip_scaled_real_مقصد_time64(دوہرا val, ثابت __vpiScope*sc);

/*
 * These functions are used mostly as compile time to strings into
 * permallocated memory. The vpip_string function is the most general,
 * it allocates a fresh string no matter what. The vpip_name_string
 * allocates a string and keeps a pointer in the hash, and tries to
 * reuse it if it can. This us useful for handle names, which may be
 * reused in different scopes.
 */
خارجی ثابت حرف* vpip_string(ثابت حرف*str);
خارجی ثابت حرف* vpip_name_string(ثابت حرف*str);


/*
 * This function is used to make decimal string versions of various
 * vectors. The input format is a vvp_vector4_t, and the result is
 * written into buf, without overflowing nbuf.
 */
خارجی غیرعلامتی vpip_vec4_مقصد_dec_str(ثابت vvp_vector4_t&vec4,
				     حرف *buf, غیرعلامتی صحیح nbuf,
				     صحیح signed_flag);


خارجی خالی vpip_vec4_مقصد_hex_str(ثابت vvp_vector4_t&بٹ, حرف*buf,
				 غیرعلامتی nbuf);

خارجی خالی vpip_vec4_مقصد_oct_str(ثابت vvp_vector4_t&بٹ, حرف*buf,
				 غیرعلامتی nbuf);

خارجی خالی vpip_bin_str_مقصد_vec4(vvp_vector4_t&val, ثابت حرف*buf);
خارجی خالی vpip_oct_str_مقصد_vec4(vvp_vector4_t&val, ثابت حرف*str);
خارجی خالی vpip_dec_str_مقصد_vec4(vvp_vector4_t&val, ثابت حرف*str);
خارجی خالی vpip_hex_str_مقصد_vec4(vvp_vector4_t&val, ثابت حرف*str);

خارجی vvp_vector4_t vec4_from_vpi_قدر(s_vpi_قدر*vp, غیرعلامتی wid);
خارجی دوہرا real_from_vpi_قدر(s_vpi_قدر*vp);

خارجی خالی vpip_vec4_get_قدر(ثابت vvp_vector4_t&لفظ_val, غیرعلامتی چوڑائی,
				bool signed_flag, s_vpi_قدر*vp);
خارجی خالی vpip_vec2_get_قدر(ثابت vvp_vector2_t&لفظ_val, غیرعلامتی چوڑائی,
				bool signed_flag, s_vpi_قدر*vp);
خارجی خالی vpip_real_get_قدر(دوہرا real, s_vpi_قدر*vp);
خارجی خالی vpip_string_get_قدر(ثابت std::string&val, s_vpi_قدر*vp);

/*
 * Function defined in vpi_signal.cc to manage vpi_get_* persistent
 * storage.
 */
شمار vpi_rbuf_t {
      RBUF_VAL =0,
	/* Storage for *_get_value() */
      RBUF_STR,
	/* Storage for *_get_str() */
      RBUF_DEL
	/* Delete the storage for both buffers. */
};
خارجی خالی *need_نتیجہ_buf(غیرعلامتی cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
خارجی حرف *simple_سیٹ_rbuf_str(ثابت حرف *s1);
خارجی حرف *generic_get_str(صحیح رمز, vpiHandle ref, ثابت حرف *name, ثابت حرف *اشاریہ);

/* A routine to find the enclosing module. */
خارجی vpiHandle vpip_module(__vpiScope*scope);

خارجی صحیح vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
خارجی bool vpip_mcd0_disable;

اندرونی uint64_t vlg_باری_مقصد_u64(دوہرا rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      واپس static_cast<uint64_t>(static_cast<int64_t>(std::باری_2(rval)));
}

#اگر_ختم /* IVL_vpi_priv_H */
