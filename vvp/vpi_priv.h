#اگر_تعریف_نشده IVL_vpi_priv_H
#تعریف_کن IVL_vpi_priv_H
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
#ضمن  "delay.h"


class class_type;
class vvp_darray;
class vvp_fun_arrayport;

تعريفنوع بنية __vpiArray* vvp_array_t;

/*
 * This header file contains the internal definitions that the vvp
 * program uses to implement the public interface in the vpi_user.h
 * header file elsewhere.
 */

#إذا defined(__MINGW32__) || defined (__CYGWIN__)
خارجي vpip_routines_s vpi_routines;
#پایان_اگر

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#تعریف_کن vpi_File_سطر    0x1000003
#تعریف_کن _vpiDescription 0x1000004

خارجي bool show_file_سطر;
خارجي bool کد_is_instrumented;

خارجي vpiHandle vpip_build_file_سطر(ثابت حرف*description,
                                      دراز file_idx, دراز lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
خارجي ثابت حرف* vpi_type_as_string(PLI_INT32 کد);

/*
 * Return the port direction as a string.
 */
خارجي ثابت حرف* direction_as_string(صحيح dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#إذا defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
#تعریف_کن _vpiFromThr 0x1000001
#   define _vpiNoThr   0
#   define _vpiString  1
#   define _vpiVThr    2
#   define vpi_کلمه    3
#   define _vpi_at_PV  4
#   define _vpi_at_A   5
#   define _vpi_at_APV 6
#پایان_اگر


/*
 * The vpi_mode_flag contains the major mode for VPI use. This is used
 * to generate error messages when vpi functions are called
 * incorrectly.
 */
تعداد vpi_نما_t {
      VPI_نما_NONE =0,
	/* The compiler is calling a register function. */
      VPI_نما_ثبات,
	/* The compiler is calling a compiletf function. */
      VPI_نما_COMPILETF,
	/* The compiler is calling a calltf function. */
      VPI_نما_CALLTF,
	/* We are in the midst of a RWSync callback. */
      VPI_نما_RWSYNC,
	/* We are in a ROSync callback. */
      VPI_نما_ROSYNC
};
خارجي vpi_نما_t vpi_نما_flag;

/*
 * This structure is the very base of a vpiHandle. Every handle
 * structure is derived from this class so that the library can
 * internally pass the derived types as pointers to one of these.
 */
class __vpiHandle {
    عام:
      مضمن __vpiHandle() { }
	// The destructor is virtual so that dynamic types will work.
      مجازی ~__vpiHandle();

      مجازی صحيح get_type_کد(فراغ) ثابت =0;
      مجازی صحيح vpi_get(صحيح کد);
      مجازی int64_t vpi_get64(صحيح کد);
      مجازی حرف* vpi_get_str(صحيح کد);

      مجازی فراغ vpi_get_مقدار(p_vpi_مقدار val);
      مجازی vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحيح flags);
      مجازی vpiHandle vpi_handle(صحيح کد);
      مجازی vpiHandle vpi_iterate(صحيح کد);
      مجازی vpiHandle vpi_شاخص(صحيح idx);
      مجازی فراغ vpi_get_delays(p_vpi_delay del);
      مجازی فراغ vpi_put_delays(p_vpi_delay del);

	// Objects may have destroyer functions of their own. If so,
	// then this virtual method will return a POINTER to that
	// function. The pointer is used to "delete" the object, which
	// is why the function itself cannot be a method.
      تعريفنوع صحيح (*free_object_fun_t)(vpiHandle);
      مجازی free_object_fun_t free_object_fun(فراغ);
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
بنية __vpiIterator : عام __vpiHandle {
      __vpiIterator();
      صحيح get_type_کد(فراغ) ثابت override;
      free_object_fun_t free_object_fun(فراغ) override;

      vpiHandle *آرگومانها;
      بیعلامت  nargs;
      بیعلامت  بعدی_2;
      bool free_آرگومانها_flag;
};

خارجي vpiHandle vpip_make_iterator(بیعلامت nargs, vpiHandle*آرگومانها,
				    bool free_آرگومانها_flag);

class __vpiDecConst : عام __vpiHandle {
    عام:
      explicit __vpiDecConst(صحيح val =0);
      __vpiDecConst(ثابت __vpiDecConst&that);
# إذا __cplusplus >= 201103L
      __vpiDecConst& عملگر = (ثابت __vpiDecConst&that) = افتراضي;
# endif
      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;

    عام:
      مضمن صحيح get_مقدار() ثابت { أرجع مقدار; }
      مضمن فراغ مجموعه_مقدار(صحيح val) { مقدار = val; }

    خاص:
      صحيح مقدار;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
بنية __vpiCallback : عام __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      صحيح get_type_کد(فراغ) ثابت override;

	// Used for listing callbacks.
      بنية __vpiCallback*بعدی_2;

	// user supplied callback data
      بنية t_cb_داده cb_داده;
};

class مقدار_callback : عام __vpiCallback {
    عام:
      explicit مقدار_callback(p_cb_داده داده);
	// Return true if the callback really is ready to be called
      مجازی bool test_مقدار_callback_ready(فراغ);

    عام:
	// user supplied callback data
      بنية t_vpi_زمان cb_زمان;
      بنية t_vpi_مقدار cb_مقدار;
};

خارجي فراغ callback_execute(بنية __vpiCallback*cur);

بنية __vpiSystemTime : عام __vpiHandle {
      __vpiSystemTime();
      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح کد) override;
      حرف*vpi_get_str(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_handle(صحيح کد) override;

      __vpiScope*scope;
};

بنية __vpiScopedTime : عام __vpiSystemTime {
      __vpiScopedTime();
      حرف*vpi_get_str(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;
};
بنية __vpiScopedSTime : عام __vpiSystemTime {
      __vpiScopedSTime();
      صحيح vpi_get(صحيح کد) override;
      حرف*vpi_get_str(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;
};
بنية __vpiScopedRealtime : عام __vpiSystemTime {
      __vpiScopedRealtime();
      صحيح vpi_get(صحيح کد) override;
      حرف*vpi_get_str(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : عام __vpiHandle {

    عام:
      صحيح vpi_get(صحيح کد) override;
      حرف* vpi_get_str(صحيح کد) override;
      vpiHandle vpi_handle(صحيح کد) override;
      vpiHandle vpi_iterate(صحيح کد) override;

    عام:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      مضمن ثابت حرف*scope_name() ثابت { أرجع name_; }

      مضمن ثابت حرف*scope_def_name() ثابت { أرجع tname_; }
	// TRUE if this is an automatic func/task/block
      مضمن bool is_automatic() ثابت { أرجع is_automatic_; }

    عام:
      __vpiScope *scope;
      بیعلامت file_idx;
      بیعلامت lineno;
      بیعلامت def_file_idx;
      بیعلامت def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_زمان;
      بنية __vpiScopedSTime scoped_stime;
      بنية __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::وکتور<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      بنية automatic_hooks_s**قلم;
      بیعلامت nitem;
        /* Keep a list of live contexts. */
      vvp_context_t زنده_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::مجموعه<vthread_t> نخها;
      علامتدار صحيح زمان_units :8;
      علامتدار صحيح زمان_precision :8;

    محمي:
      __vpiScope(ثابت حرف*nam, ثابت حرف*tnam, bool is_auto_flag =false);

    خاص:
	/* The scope has a name. */
      ثابت حرف*name_;
      ثابت حرف*tname_;
	/* the scope may be "automatic" */
      bool is_automatic_;
};

class vpiScopeFunction  : عام __vpiScope {
    عام:
      مضمن vpiScopeFunction(ثابت حرف*nam, ثابت حرف*tnam,
			      bool auto_flag, صحيح func_type, بیعلامت func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      صحيح get_type_کد(فراغ) ثابت override { أرجع vpiFunction; }
      صحيح vpi_get(صحيح کد) override
      {
	    بدل (کد) {
		حالة vpiFuncType:
		  أرجع func_type_;
		افتراضي:
		  أرجع __vpiScope::vpi_get(کد);
	    }
      }

    عام:
      مضمن بیعلامت get_func_عرض(فراغ) ثابت { أرجع func_wid_; }
      مضمن vvp_bit4_t get_func_init_val(فراغ) ثابت { أرجع func_init_val_; }

    خاص:
      صحيح func_type_;
      بیعلامت func_wid_;
      vvp_bit4_t func_init_val_;
};

خارجي __vpiScope* vpip_peek_current_scope(فراغ);
خارجي فراغ vpip_attach_مقصد_scope(__vpiScope*scope, vpiHandle obj);
خارجي فراغ vpip_attach_مقصد_current_scope(vpiHandle obj);
خارجي __vpiScope* vpip_peek_context_scope(فراغ);
خارجي بیعلامت vpip_جمع_کن_قلم_مقصد_context(automatic_hooks_s*قلم,
                                         __vpiScope*scope);
خارجي vpiHandle vpip_make_ریشه_iterator(صحيح type_کد);
خارجي فراغ vpip_make_ریشه_iterator(class __vpiHandle**&جدول,
				    بیعلامت&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
بنية __vpiSignal : عام __vpiHandle {
      صحيح vpi_get(صحيح کد) override;
      int64_t vpi_get64(صحيح کد) override;
      حرف* vpi_get_str(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کد) override;
      vpiHandle vpi_iterate(صحيح کد) override;
      vpiHandle vpi_شاخص(صحيح idx) override;

    عام:
      بیعلامت عرض() ثابت;
      vpiHandle get_شاخص(صحيح شاخص);
      فراغ get_بیت_مقدار(ثابت بنية __vpiBit*بیت, p_vpi_مقدار vp);
      vpiHandle put_بیت_مقدار(بنية __vpiBit*بیت, p_vpi_مقدار vp, صحيح flags);
      فراغ make_بیتها();

      بنية __vpiBit*بیتها;

    عام:
      اتحاد { // The scope or parent array that contains me.
	    vpiHandle والد;
	    __vpiScope* scope;
      } within;
      اتحاد { // The name of this reg/net, or the index for array words.
            ثابت حرف*name;
            vpiHandle شاخص;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      بیعلامت signed_flag  : 1;
      بیعلامت is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*گره;

    عام:
      ساكن فراغ*عملگر new(std::size_t اندازه);
      ساكن فراغ عملگر delete(فراغ*); // not implemented
    محمي:
      مضمن __vpiSignal() : بیتها(عدم) { }
    خاص: // Not implemented
      ساكن فراغ*عملگر new[] (std::size_t اندازه);
      ساكن فراغ عملگر delete[](فراغ*);
};
خارجي بیعلامت vpip_اندازه(ثابت __vpiSignal *sig);
خارجي __vpiScope* vpip_scope(__vpiSignal*sig);

خارجي vpiHandle vpip_make_int2(ثابت حرف*name, صحيح msb, صحيح lsb,
			       bool signed_flag, vvp_net_t*vec);
خارجي vpiHandle vpip_make_int4(ثابت حرف*name, صحيح msb, صحيح lsb,
			       vvp_net_t*vec);
خارجي vpiHandle vpip_make_var4(ثابت حرف*name, صحيح msb, صحيح lsb,
			       bool signed_flag, vvp_net_t*net);
خارجي vpiHandle vpip_make_net4(__vpiScope*scope,
				ثابت حرف*name, صحيح msb, صحيح lsb,
				bool signed_flag, vvp_net_t*گره);

/*
 * This is used to represent a bit in a net/reg.
 */
بنية __vpiBit {
      بنية as_بیت_t : عام __vpiHandle {
	    صحيح get_type_کد(فراغ) ثابت;
	    صحيح vpi_get(صحيح کد);
	    حرف* vpi_get_str(صحيح کد);
	    فراغ vpi_get_مقدار(p_vpi_مقدار val);
	    vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحيح flags);
	    vpiHandle vpi_handle(صحيح کد);
      } as_بیت;

      vpiHandle شاخص;

      اتحاد {
	    بنية __vpiSignal*والد;
	    بنية __vpiBit*bit0;
      };

      مضمن بیعلامت get_norm_شاخص() ثابت { أرجع this - bit0; }
      مضمن بنية __vpiSignal*get_والد() ثابت {أرجع (bit0 - 1)->والد; }
      صحيح get_شاخص(فراغ) ثابت;
};

class vpi_پورت_بیت_Info;

class vpi_پورت_Info  : عام __vpiHandle {
    عام:
      vpi_پورت_Info( __vpiScope *والد,
                    بیعلامت شاخص,
                    صحيح vpi_direction,
                    بیعلامت عرض,
                    ثابت حرف *name,
                    حرف* میانگیر );
      ~vpi_پورت_Info() override;

      صحيح get_type_کد(فراغ) ثابت override { أرجع vpi_پورت; }
      صحيح get_direction(فراغ) { أرجع direction_; }
      بیعلامت get_شاخص(فراغ) { أرجع شاخص_2; }
      صحيح get_عرض(فراغ) { أرجع عرض_2; }
      فراغ جمع_کن_پورت_بیت(vpi_پورت_بیت_Info* پورت_بیت) { پورت_بیتها.افزودن_پشت(پورت_بیت); }

      صحيح vpi_get(صحيح کد) override;
      حرف* vpi_get_str(صحيح کد) override;
      vpiHandle vpi_handle(صحيح کد) override;
      vvp_net_t* get_پورت(فراغ) ثابت { أرجع ref_; }
      vpiHandle vpi_iterate(صحيح کد) override;

      std::وکتور<vpi_پورت_بیت_Info*> پورت_بیتها;

    خاص:
      __vpiScope *والد_2;
      بیعلامت  شاخص_2;
      صحيح       direction_;
      بیعلامت  عرض_2;
      ثابت حرف *name_;
      vvp_net_t *ref_;
};

class vpi_پورت_بیت_Info  : عام __vpiHandle {
    عام:
      vpi_پورت_بیت_Info(vpi_پورت_Info *والد,
                     بیعلامت بیت);
      ~vpi_پورت_بیت_Info() override;

      صحيح get_type_کد(فراغ) ثابت override { أرجع vpi_پورت_بیت; }
      بیعلامت get_بیت(فراغ) ثابت { أرجع بیت_2; }

      صحيح vpi_get(صحيح کد) override;
      vpiHandle vpi_handle(صحيح کد) override;

    خاص:
      vpi_پورت_Info *والد_2;
      بیعلامت بیت_2;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
بنية __vpiPV : عام __vpiHandle {
      __vpiPV();
      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح کد) override;
      حرف* vpi_get_str(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کد) override;

      vpiHandle والد;
      vvp_net_t*net;
      vpiHandle sbase;
      صحيح tbase;
      بیعلامت عرض;
};
خارجي vpiHandle vpip_make_PV(حرف*name, صحيح پایه, صحيح عرض);
خارجي vpiHandle vpip_make_PV(حرف*name, حرف*نماد, صحيح عرض);
خارجي vpiHandle vpip_make_PV(حرف*name, vpiHandle handle, صحيح عرض);

بنية __vpiModPathTerm : عام __vpiHandle {
      __vpiModPathTerm();
      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح کد) override;
      vpiHandle vpi_handle(صحيح کد) override;

      vpiHandle عبارت;
	/* The value returned by vpi_get(vpiEdge, ...); */
      صحيح یال;
};

بنية __vpiModPathSrc : عام __vpiHandle {
      __vpiModPathSrc();
      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کد) override;
      vpiHandle vpi_iterate(صحيح کد) override;
      vpiHandle vpi_شاخص(صحيح idx) override;
      فراغ vpi_get_delays(p_vpi_delay del) override;
      فراغ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(فراغ) override;

      بنية __vpiModPath *dest;
      صحيح   type;

	/* This is the input expression for this modpath. */
      بنية __vpiModPathTerm مسیر_جمله_درون;

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

بنية __vpiModPath {
      __vpiScope   *scope ;

      class vvp_fun_modpath*modpath;

      بنية __vpiModPathTerm مسیر_جمله_بیرون;
      vvp_net_t *درون_داد_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

خارجي بنية __vpiModPathSrc* vpip_make_modpath_src(بنية __vpiModPath*مسیر,
                                                     vvp_net_t *net) ;

خارجي بنية __vpiModPath* vpip_make_modpath(vvp_net_t *net) ;

/*
 *
 * The vpiInterModPath vpiHandle will define
 * a vpiInterModPath of record .intermodpath as defined
 * in the IEEE 1364
 *
 */

بنية __vpiInterModPath : عام __vpiHandle {
      __vpiInterModPath();
      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کد) override;
      vpiHandle vpi_iterate(صحيح کد) override;
      فراغ vpi_get_delays(p_vpi_delay del) override;
      فراغ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(فراغ) override;

      __vpiScope   *scope ;

      class vvp_fun_intermodpath*intermodpath;

      vvp_net_t *net;

      vpi_پورت_Info* port1;
      vpi_پورت_Info* port2;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiInterModPath
 */

خارجي بنية __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpi_پورت_Info* port1, vpi_پورت_Info* port2);

/*
 * These methods support the vpi creation of events. The name string
 * passed in will be saved, so the caller must allocate it (or not
 * free it) after it is handed to this function.
 */
class __vpiNamedEvent : عام __vpiHandle {

    عام:
      __vpiNamedEvent(__vpiScope*scope, ثابت حرف*name);
      ~__vpiNamedEvent() override;

      __vpiNamedEvent(ثابت __vpiNamedEvent&) = delete;
      __vpiNamedEvent& عملگر=(ثابت __vpiNamedEvent&) = delete;

      صحيح get_type_کد(فراغ) ثابت override;
      __vpiScope*get_scope(فراغ) ثابت { أرجع scope_; }
      صحيح vpi_get(صحيح کد) override;
      حرف* vpi_get_str(صحيح کد) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کد) override;

      مضمن فراغ جمع_کن_vpi_callback(__vpiCallback*cb)
      { cb->بعدی_2 = callbacks_;
	callbacks_ = cb;
      }

      فراغ پیاپی_vpi_callbacks(فراغ);

	/* The functor, used for %set operations. */
      vvp_net_t*funct;

    خاص:
	/* base name of the event object */
      ثابت حرف*name_;
	/* Parent scope of this object. */
      __vpiScope*scope_;
	/* List of callbacks interested in this event. */
      __vpiCallback*callbacks_;
};

خارجي vpiHandle vpip_make_named_event(ثابت حرف*name, vvp_net_t*f);

/*
 * Memory is an array of bits that is accessible in N-bit chunks, with
 * N being the width of a word. The memory word handle just points
 * back to the memory and uses an index to identify its position in
 * the memory.
 */

خارجي bool is_net_array(vpiHandle obj);

/*
 * These are the various variable types.
 */
بنية __vpiRealVar : عام __vpiHandle {
      __vpiRealVar();
      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح کد) override;
      حرف* vpi_get_str(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کد) override;
      vpiHandle vpi_iterate(صحيح کد) override;

      اتحاد { // The scope or parent array that contains me.
	    vpiHandle والد;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      اتحاد {
            ثابت حرف*name;
            vpiHandle شاخص;
      } id;
      بیعلامت is_netarray  : 1; // This is word of a net array
      بیعلامت is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

خارجي __vpiScope* vpip_scope(__vpiRealVar*sig);
خارجي vpiHandle vpip_make_real_var(ثابت حرف*name, vvp_net_t*net);
خارجي vpiHandle vpip_make_real_net(__vpiScope*scope,
				    ثابت حرف*name, vvp_net_t*net);

class __vpiBaseVar : عام __vpiHandle {

    عام:
      __vpiBaseVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);
#اگر_تعریف_شده CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#پایان_اگر

      مضمن vvp_net_t* get_net() ثابت { أرجع net_; }

    محمي:
      __vpiScope* scope_;
      ثابت حرف*name_;

    خاص:
      vvp_net_t*net_;
};

class __vpiStringVar : عام __vpiBaseVar {

    عام:
      __vpiStringVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحيح flags) override;
};

خارجي vpiHandle vpip_make_string_var(ثابت حرف*name, vvp_net_t*net);

بنية __vpiArrayBase {
      __vpiArrayBase() : vals_واژگان(عدم) {}
      مجازی ~__vpiArrayBase() {}

      مجازی بیعلامت get_اندازه(فراغ) ثابت = 0;
      مجازی vpiHandle get_چپ_range() = 0;
      مجازی vpiHandle get_راست_range() = 0;
      مجازی __vpiScope*get_scope() ثابت = 0;

      مجازی صحيح get_کلمه_اندازه() ثابت = 0;
      مجازی حرف*get_کلمه_str(بنية __vpiArrayWord*کلمه, صحيح کد) = 0;
      مجازی فراغ get_کلمه_مقدار(بنية __vpiArrayWord*کلمه, p_vpi_مقدار vp) = 0;
      مجازی فراغ put_کلمه_مقدار(بنية __vpiArrayWord*کلمه, p_vpi_مقدار vp,
                                    صحيح flags) = 0;
      مجازی vpiHandle get_iter_شاخص(بنية __vpiArrayIterator*iter, صحيح idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_پایه_iterate(صحيح کد);

      مجازی فراغ make_vals_واژگان();

      بنية __vpiArrayWord*vals_واژگان;
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
بنية __vpiArray : عام __vpiArrayBase, عام __vpiHandle {
      صحيح get_type_کد(فراغ) ثابت override { أرجع vpi_حافظه; }
      بیعلامت get_اندازه() ثابت override { أرجع array_شمار; }
      vpiHandle get_چپ_range() override { أكد(nets == 0); أرجع &msb; }
      vpiHandle get_راست_range() override { أكد(nets == 0); أرجع &lsb; }
      __vpiScope*get_scope() ثابت override { أرجع scope; }

      صحيح get_کلمه_اندازه() ثابت override;
      حرف*get_کلمه_str(بنية __vpiArrayWord*کلمه, صحيح کد) override;
      فراغ get_کلمه_مقدار(بنية __vpiArrayWord*کلمه, p_vpi_مقدار vp) override;
      فراغ put_کلمه_مقدار(بنية __vpiArrayWord*کلمه, p_vpi_مقدار vp, صحيح flags) override;

      vpiHandle get_iter_شاخص(بنية __vpiArrayIterator*iter, صحيح idx) override;

      صحيح vpi_get(صحيح کد) override;
      حرف* vpi_get_str(صحيح کد) override;
      vpiHandle vpi_handle(صحيح کد) override;
      مضمن vpiHandle vpi_iterate(صحيح کد) override { أرجع vpi_array_پایه_iterate(کد); }
      vpiHandle vpi_شاخص(صحيح idx) override;

      فراغ مجموعه_کلمه(بیعلامت idx, بیعلامت off, ثابت vvp_vector4_t&val);
      فراغ مجموعه_کلمه(بیعلامت idx, مزدوج val);
      فراغ مجموعه_کلمه(بیعلامت idx, ثابت std::string&val);
      فراغ مجموعه_کلمه(بیعلامت idx, ثابت vvp_object_t&val);

      vvp_vector4_t get_کلمه(بیعلامت آدرس);
      مزدوج get_کلمه_r(بیعلامت آدرس);
      فراغ get_کلمه_obj(بیعلامت آدرس, vvp_object_t&val);
      std::string get_کلمه_str(بیعلامت آدرس);

      فراغ alias_کلمه(بیعلامت دراز addr, vpiHandle کلمه, صحيح msb, صحيح lsb);
      فراغ attach_کلمه(بیعلامت addr, vpiHandle کلمه);
      فراغ کلمه_change(بیعلامت دراز addr);

      ثابت حرف*name; /* Permanently allocated string */
      __vpiDecConst اولی_addr;
      __vpiDecConst آخرین_addr;
      __vpiDecConst msb;
      __vpiDecConst lsb;
      بیعلامت vals_عرض;
	// If this is a net array, nets lists the handles.
      vpiHandle*nets;
	// If this is a var array, then these are used instead of nets.
      vvp_vector4array_t*vals4;
      vvp_darray        *vals;

      vvp_fun_arrayport*ports_;
      بنية __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool جابجا_addr;

خاص:
      بیعلامت array_شمار;
      __vpiScope*scope;

دوست vpiHandle vpip_make_array(ثابت حرف*label, ثابت حرف*name,
                                 صحيح اولی_addr, صحيح آخرین_addr,
                                 bool signed_flag);
دوست فراغ compile_array_alias(حرف*label, حرف*name, حرف*src);
};

class __vpiDarrayVar : عام __vpiBaseVar, عام __vpiArrayBase {
    عام:
      __vpiDarrayVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح get_type_کد() ثابت override { أرجع vpiArrayVar; }
      بیعلامت get_اندازه() ثابت override;
      vpiHandle get_چپ_range() override;
      vpiHandle get_راست_range() override;
      __vpiScope*get_scope() ثابت override { أرجع scope_; }

      صحيح get_کلمه_اندازه() ثابت override;
      حرف*get_کلمه_str(بنية __vpiArrayWord*کلمه, صحيح کد) override;
      فراغ get_کلمه_مقدار(بنية __vpiArrayWord*کلمه, p_vpi_مقدار vp) override;
      فراغ put_کلمه_مقدار(بنية __vpiArrayWord*کلمه, p_vpi_مقدار vp, صحيح flags) override;

      vpiHandle get_iter_شاخص(بنية __vpiArrayIterator*iter, صحيح idx) override;
      مضمن vpiHandle vpi_iterate(صحيح کد) override { أرجع vpi_array_پایه_iterate(کد); }

      صحيح vpi_get(صحيح کد) override;
      حرف* vpi_get_str(صحيح کد) override;
      vpiHandle vpi_handle(صحيح کد) override;
      vpiHandle vpi_شاخص(صحيح شاخص) override;

      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;

    محمي:
      vvp_darray*get_vvp_darray() ثابت;
      __vpiDecConst چپ_range, راست_range;
};

خارجي vpiHandle vpip_make_darray_var(ثابت حرف*name, vvp_net_t*net);

class __vpiQueueVar : عام __vpiDarrayVar {

    عام:
      __vpiQueueVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح vpi_get(صحيح کد) override;
};

خارجي vpiHandle vpip_make_صف_var(ثابت حرف*name, vvp_net_t*net);

class __vpiPropQueueRef : عام __vpiHandle {

    عام:
      explicit __vpiPropQueueRef(__vpiScope*scope, بیعلامت pidx, bool is_صف);

      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح کد) override;
      حرف* vpi_get_str(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;

      vvp_net_t* class_net_;
      بیعلامت prop_idx_;
      bool is_صف_2;

    خاص:
      __vpiScope* scope_;
};

خارجي vpiHandle vpip_make_prop_صف_ref(حرف* class_label,
                                          بیعلامت prop_idx,
                                          بیعلامت is_صف_flag);
خارجي vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : عام __vpiBaseVar {

    عام:
      __vpiCobjectVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;
};

خارجي vpiHandle vpip_make_cobject_var(ثابت حرف*name, vvp_net_t*net);

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
بنية __vpiUserSystf : عام __vpiHandle {
      __vpiUserSystf();
      صحيح get_type_کد(فراغ) ثابت override;

      s_vpi_systf_داده info;
      bool is_user_defn;
};

خارجي vpiHandle vpip_make_systf_iterator(فراغ);

خارجي بنية __vpiUserSystf* vpip_پیدا_کن_systf(ثابت حرف*name);


بنية __vpiSysTaskCall : عام __vpiHandle {

      __vpiScope* scope;
      بنية __vpiUserSystf*defn;
      بیعلامت nargs;
      vpiHandle*آرگومانها;
	/* Stack consumed by this call */
      بیعلامت vec4_پشته;
      بیعلامت real_پشته;
      بیعلامت string_پشته;
	/* Support for vpi_get_userdata. */
      فراغ*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      بیعلامت file_idx;
      بیعلامت lineno;
      bool put_مقدار;
    محمي:
      مضمن __vpiSysTaskCall()
      {
	    vec4_پشته = 0;
	    real_پشته = 0;
	    string_پشته = 0;
      }
};

خارجي بنية __vpiSysTaskCall*vpip_cur_task;

/*
 * The persistent flag to vpip_make_string_const causes the created
 * handle to be persistent. This is necessary for cases where the
 * string handle may be reused, which is the normal case.
 *
 * When constructing with a string, the class takes possession of the
 * text value string, and will delete it in the constructor.
 */

vpiHandle vpip_make_string_const(حرف*متن, bool persistent =true);
vpiHandle vpip_make_string_param(ثابت حرف*name, حرف*مقدار, bool local_flag,
                                 دراز file_idx, دراز lineno);

بنية __vpiBinaryConst : عام __vpiHandle {
      __vpiBinaryConst();
      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;

      vvp_vector4_t بیتها;
	/* TRUE if this constant is signed. */
      بیعلامت signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      بیعلامت sized_flag  :1;
};

vpiHandle vpip_make_دودویی_const(بیعلامت wid, ثابت حرف*بیتها);
vpiHandle vpip_make_دودویی_param(ثابت حرف*name, ثابت vvp_vector4_t&بیتها,
				 bool signed_flag, bool local_flag,
				 دراز file_idx, دراز lineno);

class __vpiRealConst : عام __vpiHandle {
    عام:
      explicit __vpiRealConst(مزدوج);
      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;

      مزدوج مقدار;
};

vpiHandle vpip_make_real_const(مزدوج مقدار);
vpiHandle vpip_make_real_param(ثابت حرف*name, مزدوج مقدار, bool local_flag,
                               دراز file_idx, دراز lineno);

class __vpiNullConst : عام __vpiHandle {
    عام:
      explicit __vpiNullConst();
      صحيح get_type_کد(فراغ) ثابت override;
      صحيح vpi_get(صحيح کد) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_کلمه(بیعلامت پایه, ثابت حرف*type);
vpiHandle vpip_make_vthr_str_پشته(بیعلامت عمق);
vpiHandle vpip_make_vthr_vec4_پشته(بیعلامت عمق, bool signed_flag, بیعلامت wid);

vpiHandle vpip_make_vthr_A(حرف*label, بیعلامت شاخص);
vpiHandle vpip_make_vthr_A(حرف*label, حرف*نماد);
vpiHandle vpip_make_vthr_A(حرف*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(حرف*label, بیعلامت شاخص, بیعلامت بیت, بیعلامت wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
خارجي "C" فراغ vpip_بارگیری_module(ثابت حرف*name);

خارجي "C" فراغ vpip_clear_module_paths();
خارجي "C" فراغ vpip_جمع_کن_module_مسیر(ثابت حرف *مسیر);
خارجي "C" فراغ vpip_جمع_کن_env_and_default_module_paths();

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
خارجي vpiHandle vpip_build_vpi_call(ثابت حرف*name,
				     صحيح val_type, بیعلامت return_عرض,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     بیعلامت argc,
				     vpiHandle*argv,
				     بیعلامت vec4_پشته,
				     بیعلامت real_پشته,
				     بیعلامت string_پشته,
				     دراز file_idx,
				     دراز lineno);

خارجي vthread_t vpip_current_vthread;

خارجي فراغ vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_زمان(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

خارجي صحيح vpip_get_زمان_precision(فراغ);
خارجي فراغ vpip_مجموعه_زمان_precision(صحيح pres);

خارجي __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

خارجي صحيح vpip_زمان_units_from_handle(vpiHandle obj);
خارجي صحيح vpip_زمان_precision_from_handle(vpiHandle obj);

خارجي مزدوج vpip_scaled_زمان_from_handle(vvp_time64_t زمان, vpiHandle obj);

خارجي فراغ vpip_زمان_مقصد_timestruct(بنية t_vpi_زمان*ts, vvp_time64_t ti);
خارجي vvp_time64_t vpip_timestruct_مقصد_زمان(ثابت بنية t_vpi_زمان*ts);

خارجي مزدوج vpip_زمان_مقصد_scaled_real(vvp_time64_t ti, ثابت __vpiScope*sc);
خارجي vvp_time64_t vpip_scaled_real_مقصد_time64(مزدوج val, ثابت __vpiScope*sc);

/*
 * These functions are used mostly as compile time to strings into
 * permallocated memory. The vpip_string function is the most general,
 * it allocates a fresh string no matter what. The vpip_name_string
 * allocates a string and keeps a pointer in the hash, and tries to
 * reuse it if it can. This us useful for handle names, which may be
 * reused in different scopes.
 */
خارجي ثابت حرف* vpip_string(ثابت حرف*str);
خارجي ثابت حرف* vpip_name_string(ثابت حرف*str);


/*
 * This function is used to make decimal string versions of various
 * vectors. The input format is a vvp_vector4_t, and the result is
 * written into buf, without overflowing nbuf.
 */
خارجي بیعلامت vpip_vec4_مقصد_dec_str(ثابت vvp_vector4_t&vec4,
				     حرف *buf, بیعلامت صحيح nbuf,
				     صحيح signed_flag);


خارجي فراغ vpip_vec4_مقصد_hex_str(ثابت vvp_vector4_t&بیتها, حرف*buf,
				 بیعلامت nbuf);

خارجي فراغ vpip_vec4_مقصد_oct_str(ثابت vvp_vector4_t&بیتها, حرف*buf,
				 بیعلامت nbuf);

خارجي فراغ vpip_bin_str_مقصد_vec4(vvp_vector4_t&val, ثابت حرف*buf);
خارجي فراغ vpip_oct_str_مقصد_vec4(vvp_vector4_t&val, ثابت حرف*str);
خارجي فراغ vpip_dec_str_مقصد_vec4(vvp_vector4_t&val, ثابت حرف*str);
خارجي فراغ vpip_hex_str_مقصد_vec4(vvp_vector4_t&val, ثابت حرف*str);

خارجي vvp_vector4_t vec4_from_vpi_مقدار(s_vpi_مقدار*vp, بیعلامت wid);
خارجي مزدوج real_from_vpi_مقدار(s_vpi_مقدار*vp);

خارجي فراغ vpip_vec4_get_مقدار(ثابت vvp_vector4_t&کلمه_val, بیعلامت عرض,
				bool signed_flag, s_vpi_مقدار*vp);
خارجي فراغ vpip_vec2_get_مقدار(ثابت vvp_vector2_t&کلمه_val, بیعلامت عرض,
				bool signed_flag, s_vpi_مقدار*vp);
خارجي فراغ vpip_real_get_مقدار(مزدوج real, s_vpi_مقدار*vp);
خارجي فراغ vpip_string_get_مقدار(ثابت std::string&val, s_vpi_مقدار*vp);

/*
 * Function defined in vpi_signal.cc to manage vpi_get_* persistent
 * storage.
 */
تعداد vpi_rbuf_t {
      RBUF_VAL =0,
	/* Storage for *_get_value() */
      RBUF_STR,
	/* Storage for *_get_str() */
      RBUF_DEL
	/* Delete the storage for both buffers. */
};
خارجي فراغ *need_نتیجه_buf(بیعلامت cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
خارجي حرف *simple_مجموعه_rbuf_str(ثابت حرف *s1);
خارجي حرف *generic_get_str(صحيح کد, vpiHandle ref, ثابت حرف *name, ثابت حرف *شاخص);

/* A routine to find the enclosing module. */
خارجي vpiHandle vpip_module(__vpiScope*scope);

خارجي صحيح vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
خارجي bool vpip_mcd0_disable;

مضمن uint64_t vlg_دور_مقصد_u64(مزدوج rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      أرجع static_cast<uint64_t>(static_cast<int64_t>(std::دور_2(rval)));
}

#پایان_اگر /* IVL_vpi_priv_H */
