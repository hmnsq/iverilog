#که_نه_تعریف_شوی IVL_vpi_priv_H
#تعریف_کړه IVL_vpi_priv_H
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
#د_که_پای

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#تعریف_کړه vpi_File_کرښه    0x1000003
#تعریف_کړه _vpiDescription 0x1000004

خارجي bool show_file_کرښه;
خارجي bool کوډ_is_instrumented;

خارجي vpiHandle vpip_build_file_کرښه(ثابت حرف*description,
                                      اوږد file_idx, اوږد lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
خارجي ثابت حرف* vpi_type_as_string(PLI_INT32 کوډ);

/*
 * Return the port direction as a string.
 */
خارجي ثابت حرف* direction_as_string(صحيح dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#إذا defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
#تعریف_کړه _vpiFromThr 0x1000001
#   define _vpiNoThr   0
#   define _vpiString  1
#   define _vpiVThr    2
#   define vpi_کلمه    3
#   define _vpi_at_PV  4
#   define _vpi_at_A   5
#   define _vpi_at_APV 6
#د_که_پای


/*
 * The vpi_mode_flag contains the major mode for VPI use. This is used
 * to generate error messages when vpi functions are called
 * incorrectly.
 */
تعداد vpi_ډېرپېښ_t {
      VPI_ډېرپېښ_NONE =0,
	/* The compiler is calling a register function. */
      VPI_ډېرپېښ_ثبتوونکی,
	/* The compiler is calling a compiletf function. */
      VPI_ډېرپېښ_COMPILETF,
	/* The compiler is calling a calltf function. */
      VPI_ډېرپېښ_CALLTF,
	/* We are in the midst of a RWSync callback. */
      VPI_ډېرپېښ_RWSYNC,
	/* We are in a ROSync callback. */
      VPI_ډېرپېښ_ROSYNC
};
خارجي vpi_ډېرپېښ_t vpi_ډېرپېښ_flag;

/*
 * This structure is the very base of a vpiHandle. Every handle
 * structure is derived from this class so that the library can
 * internally pass the derived types as pointers to one of these.
 */
class __vpiHandle {
    عام:
      مضمن __vpiHandle() { }
	// The destructor is virtual so that dynamic types will work.
      مجازي ~__vpiHandle();

      مجازي صحيح get_type_کوډ(فراغ) ثابت =0;
      مجازي صحيح vpi_get(صحيح کوډ);
      مجازي int64_t vpi_get64(صحيح کوډ);
      مجازي حرف* vpi_get_str(صحيح کوډ);

      مجازي فراغ vpi_get_ارزښت(p_vpi_ارزښت val);
      مجازي vpiHandle vpi_put_ارزښت(p_vpi_ارزښت val, صحيح flags);
      مجازي vpiHandle vpi_handle(صحيح کوډ);
      مجازي vpiHandle vpi_iterate(صحيح کوډ);
      مجازي vpiHandle vpi_شاخص(صحيح idx);
      مجازي فراغ vpi_get_delays(p_vpi_delay del);
      مجازي فراغ vpi_put_delays(p_vpi_delay del);

	// Objects may have destroyer functions of their own. If so,
	// then this virtual method will return a POINTER to that
	// function. The pointer is used to "delete" the object, which
	// is why the function itself cannot be a method.
      تعريفنوع صحيح (*free_object_fun_t)(vpiHandle);
      مجازي free_object_fun_t free_object_fun(فراغ);
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
      صحيح get_type_کوډ(فراغ) ثابت override;
      free_object_fun_t free_object_fun(فراغ) override;

      vpiHandle *دلیلونه;
      بینښه  nargs;
      بینښه  بل_2;
      bool free_دلیلونه_flag;
};

خارجي vpiHandle vpip_make_iterator(بینښه nargs, vpiHandle*دلیلونه,
				    bool free_دلیلونه_flag);

class __vpiDecConst : عام __vpiHandle {
    عام:
      explicit __vpiDecConst(صحيح val =0);
      __vpiDecConst(ثابت __vpiDecConst&that);
# إذا __cplusplus >= 201103L
      __vpiDecConst& چلوونکی = (ثابت __vpiDecConst&that) = افتراضي;
# endif
      صحيح get_type_کوډ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;

    عام:
      مضمن صحيح get_ارزښت() ثابت { أرجع ارزښت; }
      مضمن فراغ سېټ_ارزښت(صحيح val) { ارزښت = val; }

    خاص:
      صحيح ارزښت;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
بنية __vpiCallback : عام __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      صحيح get_type_کوډ(فراغ) ثابت override;

	// Used for listing callbacks.
      بنية __vpiCallback*بل_2;

	// user supplied callback data
      بنية t_cb_معلومات cb_معلومات;
};

class ارزښت_callback : عام __vpiCallback {
    عام:
      explicit ارزښت_callback(p_cb_معلومات معلومات);
	// Return true if the callback really is ready to be called
      مجازي bool test_ارزښت_callback_ready(فراغ);

    عام:
	// user supplied callback data
      بنية t_vpi_وخت cb_وخت;
      بنية t_vpi_ارزښت cb_ارزښت;
};

خارجي فراغ callback_execute(بنية __vpiCallback*cur);

بنية __vpiSystemTime : عام __vpiHandle {
      __vpiSystemTime();
      صحيح get_type_کوډ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوډ) override;
      حرف*vpi_get_str(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;
      vpiHandle vpi_handle(صحيح کوډ) override;

      __vpiScope*scope;
};

بنية __vpiScopedTime : عام __vpiSystemTime {
      __vpiScopedTime();
      حرف*vpi_get_str(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;
};
بنية __vpiScopedSTime : عام __vpiSystemTime {
      __vpiScopedSTime();
      صحيح vpi_get(صحيح کوډ) override;
      حرف*vpi_get_str(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;
};
بنية __vpiScopedRealtime : عام __vpiSystemTime {
      __vpiScopedRealtime();
      صحيح vpi_get(صحيح کوډ) override;
      حرف*vpi_get_str(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : عام __vpiHandle {

    عام:
      صحيح vpi_get(صحيح کوډ) override;
      حرف* vpi_get_str(صحيح کوډ) override;
      vpiHandle vpi_handle(صحيح کوډ) override;
      vpiHandle vpi_iterate(صحيح کوډ) override;

    عام:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      مضمن ثابت حرف*scope_name() ثابت { أرجع name_; }

      مضمن ثابت حرف*scope_def_name() ثابت { أرجع tname_; }
	// TRUE if this is an automatic func/task/block
      مضمن bool is_automatic() ثابت { أرجع is_automatic_; }

    عام:
      __vpiScope *scope;
      بینښه file_idx;
      بینښه lineno;
      بینښه def_file_idx;
      بینښه def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_وخت;
      بنية __vpiScopedSTime scoped_stime;
      بنية __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::ویکټور<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      بنية automatic_hooks_s**توکی;
      بینښه nitem;
        /* Keep a list of live contexts. */
      vvp_context_t ژوندی_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::سېټ<vthread_t> تارونه;
      نښهدار صحيح وخت_units :8;
      نښهدار صحيح وخت_precision :8;

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
			      bool auto_flag, صحيح func_type, بینښه func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      صحيح get_type_کوډ(فراغ) ثابت override { أرجع vpiFunction; }
      صحيح vpi_get(صحيح کوډ) override
      {
	    بدل (کوډ) {
		حالة vpiFuncType:
		  أرجع func_type_;
		افتراضي:
		  أرجع __vpiScope::vpi_get(کوډ);
	    }
      }

    عام:
      مضمن بینښه get_func_پلنوالی(فراغ) ثابت { أرجع func_wid_; }
      مضمن vvp_bit4_t get_func_init_val(فراغ) ثابت { أرجع func_init_val_; }

    خاص:
      صحيح func_type_;
      بینښه func_wid_;
      vvp_bit4_t func_init_val_;
};

خارجي __vpiScope* vpip_peek_current_scope(فراغ);
خارجي فراغ vpip_attach_منزل_scope(__vpiScope*scope, vpiHandle obj);
خارجي فراغ vpip_attach_منزل_current_scope(vpiHandle obj);
خارجي __vpiScope* vpip_peek_context_scope(فراغ);
خارجي بینښه vpip_جمع_کړه_توکی_منزل_context(automatic_hooks_s*توکی,
                                         __vpiScope*scope);
خارجي vpiHandle vpip_make_ریښه_iterator(صحيح type_کوډ);
خارجي فراغ vpip_make_ریښه_iterator(class __vpiHandle**&جدول,
				    بینښه&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
بنية __vpiSignal : عام __vpiHandle {
      صحيح vpi_get(صحيح کوډ) override;
      int64_t vpi_get64(صحيح کوډ) override;
      حرف* vpi_get_str(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;
      vpiHandle vpi_put_ارزښت(p_vpi_ارزښت val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کوډ) override;
      vpiHandle vpi_iterate(صحيح کوډ) override;
      vpiHandle vpi_شاخص(صحيح idx) override;

    عام:
      بینښه پلنوالی() ثابت;
      vpiHandle get_شاخص(صحيح شاخص);
      فراغ get_بټ_ارزښت(ثابت بنية __vpiBit*بټ, p_vpi_ارزښت vp);
      vpiHandle put_بټ_ارزښت(بنية __vpiBit*بټ, p_vpi_ارزښت vp, صحيح flags);
      فراغ make_بټونه();

      بنية __vpiBit*بټونه;

    عام:
      اتحاد { // The scope or parent array that contains me.
	    vpiHandle پلار;
	    __vpiScope* scope;
      } within;
      اتحاد { // The name of this reg/net, or the index for array words.
            ثابت حرف*name;
            vpiHandle شاخص;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      بینښه signed_flag  : 1;
      بینښه is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*غوټه;

    عام:
      ساكن فراغ*چلوونکی new(std::size_t اندازه);
      ساكن فراغ چلوونکی delete(فراغ*); // not implemented
    محمي:
      مضمن __vpiSignal() : بټونه(عدم) { }
    خاص: // Not implemented
      ساكن فراغ*چلوونکی new[] (std::size_t اندازه);
      ساكن فراغ چلوونکی delete[](فراغ*);
};
خارجي بینښه vpip_اندازه(ثابت __vpiSignal *sig);
خارجي __vpiScope* vpip_scope(__vpiSignal*sig);

خارجي vpiHandle vpip_make_int2(ثابت حرف*name, صحيح msb, صحيح lsb,
			       bool signed_flag, vvp_net_t*vec);
خارجي vpiHandle vpip_make_int4(ثابت حرف*name, صحيح msb, صحيح lsb,
			       vvp_net_t*vec);
خارجي vpiHandle vpip_make_var4(ثابت حرف*name, صحيح msb, صحيح lsb,
			       bool signed_flag, vvp_net_t*net);
خارجي vpiHandle vpip_make_net4(__vpiScope*scope,
				ثابت حرف*name, صحيح msb, صحيح lsb,
				bool signed_flag, vvp_net_t*غوټه);

/*
 * This is used to represent a bit in a net/reg.
 */
بنية __vpiBit {
      بنية as_بټ_t : عام __vpiHandle {
	    صحيح get_type_کوډ(فراغ) ثابت;
	    صحيح vpi_get(صحيح کوډ);
	    حرف* vpi_get_str(صحيح کوډ);
	    فراغ vpi_get_ارزښت(p_vpi_ارزښت val);
	    vpiHandle vpi_put_ارزښت(p_vpi_ارزښت val, صحيح flags);
	    vpiHandle vpi_handle(صحيح کوډ);
      } as_بټ;

      vpiHandle شاخص;

      اتحاد {
	    بنية __vpiSignal*پلار;
	    بنية __vpiBit*bit0;
      };

      مضمن بینښه get_norm_شاخص() ثابت { أرجع this - bit0; }
      مضمن بنية __vpiSignal*get_پلار() ثابت {أرجع (bit0 - 1)->پلار; }
      صحيح get_شاخص(فراغ) ثابت;
};

class vpi_پورټ_بټ_Info;

class vpi_پورټ_Info  : عام __vpiHandle {
    عام:
      vpi_پورټ_Info( __vpiScope *پلار,
                    بینښه شاخص,
                    صحيح vpi_direction,
                    بینښه پلنوالی,
                    ثابت حرف *name,
                    حرف* بفر );
      ~vpi_پورټ_Info() override;

      صحيح get_type_کوډ(فراغ) ثابت override { أرجع vpi_پورټ; }
      صحيح get_direction(فراغ) { أرجع direction_; }
      بینښه get_شاخص(فراغ) { أرجع شاخص_2; }
      صحيح get_پلنوالی(فراغ) { أرجع پلنوالی_2; }
      فراغ جمع_کړه_پورټ_بټ(vpi_پورټ_بټ_Info* پورټ_بټ) { پورټ_بټونه.ټېل_شا(پورټ_بټ); }

      صحيح vpi_get(صحيح کوډ) override;
      حرف* vpi_get_str(صحيح کوډ) override;
      vpiHandle vpi_handle(صحيح کوډ) override;
      vvp_net_t* get_پورټ(فراغ) ثابت { أرجع ref_; }
      vpiHandle vpi_iterate(صحيح کوډ) override;

      std::ویکټور<vpi_پورټ_بټ_Info*> پورټ_بټونه;

    خاص:
      __vpiScope *پلار_2;
      بینښه  شاخص_2;
      صحيح       direction_;
      بینښه  پلنوالی_2;
      ثابت حرف *name_;
      vvp_net_t *ref_;
};

class vpi_پورټ_بټ_Info  : عام __vpiHandle {
    عام:
      vpi_پورټ_بټ_Info(vpi_پورټ_Info *پلار,
                     بینښه بټ);
      ~vpi_پورټ_بټ_Info() override;

      صحيح get_type_کوډ(فراغ) ثابت override { أرجع vpi_پورټ_بټ; }
      بینښه get_بټ(فراغ) ثابت { أرجع بټ_2; }

      صحيح vpi_get(صحيح کوډ) override;
      vpiHandle vpi_handle(صحيح کوډ) override;

    خاص:
      vpi_پورټ_Info *پلار_2;
      بینښه بټ_2;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
بنية __vpiPV : عام __vpiHandle {
      __vpiPV();
      صحيح get_type_کوډ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوډ) override;
      حرف* vpi_get_str(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;
      vpiHandle vpi_put_ارزښت(p_vpi_ارزښت val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کوډ) override;

      vpiHandle پلار;
      vvp_net_t*net;
      vpiHandle sbase;
      صحيح tbase;
      بینښه پلنوالی;
};
خارجي vpiHandle vpip_make_PV(حرف*name, صحيح بنسټ, صحيح پلنوالی);
خارجي vpiHandle vpip_make_PV(حرف*name, حرف*نښه, صحيح پلنوالی);
خارجي vpiHandle vpip_make_PV(حرف*name, vpiHandle handle, صحيح پلنوالی);

بنية __vpiModPathTerm : عام __vpiHandle {
      __vpiModPathTerm();
      صحيح get_type_کوډ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوډ) override;
      vpiHandle vpi_handle(صحيح کوډ) override;

      vpiHandle بیان;
	/* The value returned by vpi_get(vpiEdge, ...); */
      صحيح څنډه;
};

بنية __vpiModPathSrc : عام __vpiHandle {
      __vpiModPathSrc();
      صحيح get_type_کوډ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;
      vpiHandle vpi_put_ارزښت(p_vpi_ارزښت val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کوډ) override;
      vpiHandle vpi_iterate(صحيح کوډ) override;
      vpiHandle vpi_شاخص(صحيح idx) override;
      فراغ vpi_get_delays(p_vpi_delay del) override;
      فراغ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(فراغ) override;

      بنية __vpiModPath *dest;
      صحيح   type;

	/* This is the input expression for this modpath. */
      بنية __vpiModPathTerm لار_غړی_دننه;

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

      بنية __vpiModPathTerm لار_غړی_بهر;
      vvp_net_t *ننوت_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

خارجي بنية __vpiModPathSrc* vpip_make_modpath_src(بنية __vpiModPath*لار,
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
      صحيح get_type_کوډ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;
      vpiHandle vpi_put_ارزښت(p_vpi_ارزښت val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کوډ) override;
      vpiHandle vpi_iterate(صحيح کوډ) override;
      فراغ vpi_get_delays(p_vpi_delay del) override;
      فراغ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(فراغ) override;

      __vpiScope   *scope ;

      class vvp_fun_intermodpath*intermodpath;

      vvp_net_t *net;

      vpi_پورټ_Info* port1;
      vpi_پورټ_Info* port2;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiInterModPath
 */

خارجي بنية __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpi_پورټ_Info* port1, vpi_پورټ_Info* port2);

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
      __vpiNamedEvent& چلوونکی=(ثابت __vpiNamedEvent&) = delete;

      صحيح get_type_کوډ(فراغ) ثابت override;
      __vpiScope*get_scope(فراغ) ثابت { أرجع scope_; }
      صحيح vpi_get(صحيح کوډ) override;
      حرف* vpi_get_str(صحيح کوډ) override;
      vpiHandle vpi_put_ارزښت(p_vpi_ارزښت val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کوډ) override;

      مضمن فراغ جمع_کړه_vpi_callback(__vpiCallback*cb)
      { cb->بل_2 = callbacks_;
	callbacks_ = cb;
      }

      فراغ پرلپسې_vpi_callbacks(فراغ);

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
      صحيح get_type_کوډ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوډ) override;
      حرف* vpi_get_str(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;
      vpiHandle vpi_put_ارزښت(p_vpi_ارزښت val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کوډ) override;
      vpiHandle vpi_iterate(صحيح کوډ) override;

      اتحاد { // The scope or parent array that contains me.
	    vpiHandle پلار;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      اتحاد {
            ثابت حرف*name;
            vpiHandle شاخص;
      } id;
      بینښه is_netarray  : 1; // This is word of a net array
      بینښه is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

خارجي __vpiScope* vpip_scope(__vpiRealVar*sig);
خارجي vpiHandle vpip_make_real_var(ثابت حرف*name, vvp_net_t*net);
خارجي vpiHandle vpip_make_real_net(__vpiScope*scope,
				    ثابت حرف*name, vvp_net_t*net);

class __vpiBaseVar : عام __vpiHandle {

    عام:
      __vpiBaseVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);
#که_تعریف_شوی CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#د_که_پای

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

      صحيح get_type_کوډ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;
      vpiHandle vpi_put_ارزښت(p_vpi_ارزښت val, صحيح flags) override;
};

خارجي vpiHandle vpip_make_string_var(ثابت حرف*name, vvp_net_t*net);

بنية __vpiArrayBase {
      __vpiArrayBase() : vals_ټکي(عدم) {}
      مجازي ~__vpiArrayBase() {}

      مجازي بینښه get_اندازه(فراغ) ثابت = 0;
      مجازي vpiHandle get_کیڼ_range() = 0;
      مجازي vpiHandle get_ښی_range() = 0;
      مجازي __vpiScope*get_scope() ثابت = 0;

      مجازي صحيح get_کلمه_اندازه() ثابت = 0;
      مجازي حرف*get_کلمه_str(بنية __vpiArrayWord*کلمه, صحيح کوډ) = 0;
      مجازي فراغ get_کلمه_ارزښت(بنية __vpiArrayWord*کلمه, p_vpi_ارزښت vp) = 0;
      مجازي فراغ put_کلمه_ارزښت(بنية __vpiArrayWord*کلمه, p_vpi_ارزښت vp,
                                    صحيح flags) = 0;
      مجازي vpiHandle get_iter_شاخص(بنية __vpiArrayIterator*iter, صحيح idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_بنسټ_iterate(صحيح کوډ);

      مجازي فراغ make_vals_ټکي();

      بنية __vpiArrayWord*vals_ټکي;
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
      صحيح get_type_کوډ(فراغ) ثابت override { أرجع vpi_حافظه; }
      بینښه get_اندازه() ثابت override { أرجع array_شمېر; }
      vpiHandle get_کیڼ_range() override { أكد(nets == 0); أرجع &msb; }
      vpiHandle get_ښی_range() override { أكد(nets == 0); أرجع &lsb; }
      __vpiScope*get_scope() ثابت override { أرجع scope; }

      صحيح get_کلمه_اندازه() ثابت override;
      حرف*get_کلمه_str(بنية __vpiArrayWord*کلمه, صحيح کوډ) override;
      فراغ get_کلمه_ارزښت(بنية __vpiArrayWord*کلمه, p_vpi_ارزښت vp) override;
      فراغ put_کلمه_ارزښت(بنية __vpiArrayWord*کلمه, p_vpi_ارزښت vp, صحيح flags) override;

      vpiHandle get_iter_شاخص(بنية __vpiArrayIterator*iter, صحيح idx) override;

      صحيح vpi_get(صحيح کوډ) override;
      حرف* vpi_get_str(صحيح کوډ) override;
      vpiHandle vpi_handle(صحيح کوډ) override;
      مضمن vpiHandle vpi_iterate(صحيح کوډ) override { أرجع vpi_array_بنسټ_iterate(کوډ); }
      vpiHandle vpi_شاخص(صحيح idx) override;

      فراغ سېټ_کلمه(بینښه idx, بینښه off, ثابت vvp_vector4_t&val);
      فراغ سېټ_کلمه(بینښه idx, مزدوج val);
      فراغ سېټ_کلمه(بینښه idx, ثابت std::string&val);
      فراغ سېټ_کلمه(بینښه idx, ثابت vvp_object_t&val);

      vvp_vector4_t get_کلمه(بینښه پته);
      مزدوج get_کلمه_r(بینښه پته);
      فراغ get_کلمه_obj(بینښه پته, vvp_object_t&val);
      std::string get_کلمه_str(بینښه پته);

      فراغ alias_کلمه(بینښه اوږد addr, vpiHandle کلمه, صحيح msb, صحيح lsb);
      فراغ attach_کلمه(بینښه addr, vpiHandle کلمه);
      فراغ کلمه_change(بینښه اوږد addr);

      ثابت حرف*name; /* Permanently allocated string */
      __vpiDecConst لومړی_addr;
      __vpiDecConst وروستی_addr;
      __vpiDecConst msb;
      __vpiDecConst lsb;
      بینښه vals_پلنوالی;
	// If this is a net array, nets lists the handles.
      vpiHandle*nets;
	// If this is a var array, then these are used instead of nets.
      vvp_vector4array_t*vals4;
      vvp_darray        *vals;

      vvp_fun_arrayport*ports_;
      بنية __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool بدلون_addr;

خاص:
      بینښه array_شمېر;
      __vpiScope*scope;

ملګری vpiHandle vpip_make_array(ثابت حرف*label, ثابت حرف*name,
                                 صحيح لومړی_addr, صحيح وروستی_addr,
                                 bool signed_flag);
ملګری فراغ compile_array_alias(حرف*label, حرف*name, حرف*src);
};

class __vpiDarrayVar : عام __vpiBaseVar, عام __vpiArrayBase {
    عام:
      __vpiDarrayVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح get_type_کوډ() ثابت override { أرجع vpiArrayVar; }
      بینښه get_اندازه() ثابت override;
      vpiHandle get_کیڼ_range() override;
      vpiHandle get_ښی_range() override;
      __vpiScope*get_scope() ثابت override { أرجع scope_; }

      صحيح get_کلمه_اندازه() ثابت override;
      حرف*get_کلمه_str(بنية __vpiArrayWord*کلمه, صحيح کوډ) override;
      فراغ get_کلمه_ارزښت(بنية __vpiArrayWord*کلمه, p_vpi_ارزښت vp) override;
      فراغ put_کلمه_ارزښت(بنية __vpiArrayWord*کلمه, p_vpi_ارزښت vp, صحيح flags) override;

      vpiHandle get_iter_شاخص(بنية __vpiArrayIterator*iter, صحيح idx) override;
      مضمن vpiHandle vpi_iterate(صحيح کوډ) override { أرجع vpi_array_بنسټ_iterate(کوډ); }

      صحيح vpi_get(صحيح کوډ) override;
      حرف* vpi_get_str(صحيح کوډ) override;
      vpiHandle vpi_handle(صحيح کوډ) override;
      vpiHandle vpi_شاخص(صحيح شاخص) override;

      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;

    محمي:
      vvp_darray*get_vvp_darray() ثابت;
      __vpiDecConst کیڼ_range, ښی_range;
};

خارجي vpiHandle vpip_make_darray_var(ثابت حرف*name, vvp_net_t*net);

class __vpiQueueVar : عام __vpiDarrayVar {

    عام:
      __vpiQueueVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح vpi_get(صحيح کوډ) override;
};

خارجي vpiHandle vpip_make_کتار_var(ثابت حرف*name, vvp_net_t*net);

class __vpiPropQueueRef : عام __vpiHandle {

    عام:
      explicit __vpiPropQueueRef(__vpiScope*scope, بینښه pidx, bool is_کتار);

      صحيح get_type_کوډ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوډ) override;
      حرف* vpi_get_str(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;

      vvp_net_t* class_net_;
      بینښه prop_idx_;
      bool is_کتار_2;

    خاص:
      __vpiScope* scope_;
};

خارجي vpiHandle vpip_make_prop_کتار_ref(حرف* class_label,
                                          بینښه prop_idx,
                                          بینښه is_کتار_flag);
خارجي vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : عام __vpiBaseVar {

    عام:
      __vpiCobjectVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح get_type_کوډ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;
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
      صحيح get_type_کوډ(فراغ) ثابت override;

      s_vpi_systf_معلومات info;
      bool is_user_defn;
};

خارجي vpiHandle vpip_make_systf_iterator(فراغ);

خارجي بنية __vpiUserSystf* vpip_پیدا_کړه_systf(ثابت حرف*name);


بنية __vpiSysTaskCall : عام __vpiHandle {

      __vpiScope* scope;
      بنية __vpiUserSystf*defn;
      بینښه nargs;
      vpiHandle*دلیلونه;
	/* Stack consumed by this call */
      بینښه vec4_ډېری;
      بینښه real_ډېری;
      بینښه string_ډېری;
	/* Support for vpi_get_userdata. */
      فراغ*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      بینښه file_idx;
      بینښه lineno;
      bool put_ارزښت;
    محمي:
      مضمن __vpiSysTaskCall()
      {
	    vec4_ډېری = 0;
	    real_ډېری = 0;
	    string_ډېری = 0;
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
vpiHandle vpip_make_string_param(ثابت حرف*name, حرف*ارزښت, bool local_flag,
                                 اوږد file_idx, اوږد lineno);

بنية __vpiBinaryConst : عام __vpiHandle {
      __vpiBinaryConst();
      صحيح get_type_کوډ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;

      vvp_vector4_t بټونه;
	/* TRUE if this constant is signed. */
      بینښه signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      بینښه sized_flag  :1;
};

vpiHandle vpip_make_دوګونی_const(بینښه wid, ثابت حرف*بټونه);
vpiHandle vpip_make_دوګونی_param(ثابت حرف*name, ثابت vvp_vector4_t&بټونه,
				 bool signed_flag, bool local_flag,
				 اوږد file_idx, اوږد lineno);

class __vpiRealConst : عام __vpiHandle {
    عام:
      explicit __vpiRealConst(مزدوج);
      صحيح get_type_کوډ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;

      مزدوج ارزښت;
};

vpiHandle vpip_make_real_const(مزدوج ارزښت);
vpiHandle vpip_make_real_param(ثابت حرف*name, مزدوج ارزښت, bool local_flag,
                               اوږد file_idx, اوږد lineno);

class __vpiNullConst : عام __vpiHandle {
    عام:
      explicit __vpiNullConst();
      صحيح get_type_کوډ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوډ) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_کلمه(بینښه بنسټ, ثابت حرف*type);
vpiHandle vpip_make_vthr_str_ډېری(بینښه ژوروالی);
vpiHandle vpip_make_vthr_vec4_ډېری(بینښه ژوروالی, bool signed_flag, بینښه wid);

vpiHandle vpip_make_vthr_A(حرف*label, بینښه شاخص);
vpiHandle vpip_make_vthr_A(حرف*label, حرف*نښه);
vpiHandle vpip_make_vthr_A(حرف*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(حرف*label, بینښه شاخص, بینښه بټ, بینښه wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
خارجي "C" فراغ vpip_بارول_module(ثابت حرف*name);

خارجي "C" فراغ vpip_clear_module_paths();
خارجي "C" فراغ vpip_جمع_کړه_module_لار(ثابت حرف *لار);
خارجي "C" فراغ vpip_جمع_کړه_env_and_default_module_paths();

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
				     صحيح val_type, بینښه return_پلنوالی,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     بینښه argc,
				     vpiHandle*argv,
				     بینښه vec4_ډېری,
				     بینښه real_ډېری,
				     بینښه string_ډېری,
				     اوږد file_idx,
				     اوږد lineno);

خارجي vthread_t vpip_current_vthread;

خارجي فراغ vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_وخت(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

خارجي صحيح vpip_get_وخت_precision(فراغ);
خارجي فراغ vpip_سېټ_وخت_precision(صحيح pres);

خارجي __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

خارجي صحيح vpip_وخت_units_from_handle(vpiHandle obj);
خارجي صحيح vpip_وخت_precision_from_handle(vpiHandle obj);

خارجي مزدوج vpip_scaled_وخت_from_handle(vvp_time64_t وخت, vpiHandle obj);

خارجي فراغ vpip_وخت_منزل_timestruct(بنية t_vpi_وخت*ts, vvp_time64_t ti);
خارجي vvp_time64_t vpip_timestruct_منزل_وخت(ثابت بنية t_vpi_وخت*ts);

خارجي مزدوج vpip_وخت_منزل_scaled_real(vvp_time64_t ti, ثابت __vpiScope*sc);
خارجي vvp_time64_t vpip_scaled_real_منزل_time64(مزدوج val, ثابت __vpiScope*sc);

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
خارجي بینښه vpip_vec4_منزل_dec_str(ثابت vvp_vector4_t&vec4,
				     حرف *buf, بینښه صحيح nbuf,
				     صحيح signed_flag);


خارجي فراغ vpip_vec4_منزل_hex_str(ثابت vvp_vector4_t&بټونه, حرف*buf,
				 بینښه nbuf);

خارجي فراغ vpip_vec4_منزل_oct_str(ثابت vvp_vector4_t&بټونه, حرف*buf,
				 بینښه nbuf);

خارجي فراغ vpip_bin_str_منزل_vec4(vvp_vector4_t&val, ثابت حرف*buf);
خارجي فراغ vpip_oct_str_منزل_vec4(vvp_vector4_t&val, ثابت حرف*str);
خارجي فراغ vpip_dec_str_منزل_vec4(vvp_vector4_t&val, ثابت حرف*str);
خارجي فراغ vpip_hex_str_منزل_vec4(vvp_vector4_t&val, ثابت حرف*str);

خارجي vvp_vector4_t vec4_from_vpi_ارزښت(s_vpi_ارزښت*vp, بینښه wid);
خارجي مزدوج real_from_vpi_ارزښت(s_vpi_ارزښت*vp);

خارجي فراغ vpip_vec4_get_ارزښت(ثابت vvp_vector4_t&کلمه_val, بینښه پلنوالی,
				bool signed_flag, s_vpi_ارزښت*vp);
خارجي فراغ vpip_vec2_get_ارزښت(ثابت vvp_vector2_t&کلمه_val, بینښه پلنوالی,
				bool signed_flag, s_vpi_ارزښت*vp);
خارجي فراغ vpip_real_get_ارزښت(مزدوج real, s_vpi_ارزښت*vp);
خارجي فراغ vpip_string_get_ارزښت(ثابت std::string&val, s_vpi_ارزښت*vp);

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
خارجي فراغ *need_پایله_buf(بینښه cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
خارجي حرف *simple_سېټ_rbuf_str(ثابت حرف *s1);
خارجي حرف *generic_get_str(صحيح کوډ, vpiHandle ref, ثابت حرف *name, ثابت حرف *شاخص);

/* A routine to find the enclosing module. */
خارجي vpiHandle vpip_module(__vpiScope*scope);

خارجي صحيح vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
خارجي bool vpip_mcd0_disable;

مضمن uint64_t vlg_پړاو_منزل_u64(مزدوج rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      أرجع static_cast<uint64_t>(static_cast<int64_t>(std::پړاو_2(rval)));
}

#د_که_پای /* IVL_vpi_priv_H */
