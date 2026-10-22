#جے_بنا_تعریف IVL_vpi_priv_H
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
#جے_انت

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#تعریف_کرو vpi_File_سطر    0x1000003
#تعریف_کرو _vpiDescription 0x1000004

خارجي bool show_file_سطر;
خارجي bool کوڈ_is_instrumented;

خارجي vpiHandle vpip_build_file_سطر(ثابت حرف*description,
                                      لما file_idx, لما lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
خارجي ثابت حرف* vpi_type_as_string(PLI_INT32 کوڈ);

/*
 * Return the port direction as a string.
 */
خارجي ثابت حرف* direction_as_string(صحيح dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#إذا defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
#تعریف_کرو _vpiFromThr 0x1000001
#   define _vpiNoThr   0
#   define _vpiString  1
#   define _vpiVThr    2
#   define vpi_لفظ    3
#   define _vpi_at_PV  4
#   define _vpi_at_A   5
#   define _vpi_at_APV 6
#جے_انت


/*
 * The vpi_mode_flag contains the major mode for VPI use. This is used
 * to generate error messages when vpi functions are called
 * incorrectly.
 */
تعداد vpi_منوال_t {
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
خارجي vpi_منوال_t vpi_منوال_flag;

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

      مجازی صحيح get_type_کوڈ(فراغ) ثابت =0;
      مجازی صحيح vpi_get(صحيح کوڈ);
      مجازی int64_t vpi_get64(صحيح کوڈ);
      مجازی حرف* vpi_get_str(صحيح کوڈ);

      مجازی فراغ vpi_get_قدر(p_vpi_قدر val);
      مجازی vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags);
      مجازی vpiHandle vpi_handle(صحيح کوڈ);
      مجازی vpiHandle vpi_iterate(صحيح کوڈ);
      مجازی vpiHandle vpi_اشاریہ(صحيح idx);
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
      صحيح get_type_کوڈ(فراغ) ثابت override;
      free_object_fun_t free_object_fun(فراغ) override;

      vpiHandle *دلیلاں;
      بنانشانی  nargs;
      بنانشانی  اگلا_2;
      bool free_دلیلاں_flag;
};

خارجي vpiHandle vpip_make_iterator(بنانشانی nargs, vpiHandle*دلیلاں,
				    bool free_دلیلاں_flag);

class __vpiDecConst : عام __vpiHandle {
    عام:
      explicit __vpiDecConst(صحيح val =0);
      __vpiDecConst(ثابت __vpiDecConst&that);
# إذا __cplusplus >= 201103L
      __vpiDecConst& عامل = (ثابت __vpiDecConst&that) = افتراضي;
# endif
      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;

    عام:
      مضمن صحيح get_قدر() ثابت { أرجع قدر; }
      مضمن فراغ سیٹ_قدر(صحيح val) { قدر = val; }

    خاص:
      صحيح قدر;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
بنية __vpiCallback : عام __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      صحيح get_type_کوڈ(فراغ) ثابت override;

	// Used for listing callbacks.
      بنية __vpiCallback*اگلا_2;

	// user supplied callback data
      بنية t_cb_انکڑے cb_انکڑے;
};

class قدر_callback : عام __vpiCallback {
    عام:
      explicit قدر_callback(p_cb_انکڑے انکڑے);
	// Return true if the callback really is ready to be called
      مجازی bool test_قدر_callback_ready(فراغ);

    عام:
	// user supplied callback data
      بنية t_vpi_ویلا cb_ویلا;
      بنية t_vpi_قدر cb_قدر;
};

خارجي فراغ callback_execute(بنية __vpiCallback*cur);

بنية __vpiSystemTime : عام __vpiHandle {
      __vpiSystemTime();
      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوڈ) override;
      حرف*vpi_get_str(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_handle(صحيح کوڈ) override;

      __vpiScope*scope;
};

بنية __vpiScopedTime : عام __vpiSystemTime {
      __vpiScopedTime();
      حرف*vpi_get_str(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
};
بنية __vpiScopedSTime : عام __vpiSystemTime {
      __vpiScopedSTime();
      صحيح vpi_get(صحيح کوڈ) override;
      حرف*vpi_get_str(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
};
بنية __vpiScopedRealtime : عام __vpiSystemTime {
      __vpiScopedRealtime();
      صحيح vpi_get(صحيح کوڈ) override;
      حرف*vpi_get_str(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : عام __vpiHandle {

    عام:
      صحيح vpi_get(صحيح کوڈ) override;
      حرف* vpi_get_str(صحيح کوڈ) override;
      vpiHandle vpi_handle(صحيح کوڈ) override;
      vpiHandle vpi_iterate(صحيح کوڈ) override;

    عام:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      مضمن ثابت حرف*scope_name() ثابت { أرجع name_; }

      مضمن ثابت حرف*scope_def_name() ثابت { أرجع tname_; }
	// TRUE if this is an automatic func/task/block
      مضمن bool is_automatic() ثابت { أرجع is_automatic_; }

    عام:
      __vpiScope *scope;
      بنانشانی file_idx;
      بنانشانی lineno;
      بنانشانی def_file_idx;
      بنانشانی def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_ویلا;
      بنية __vpiScopedSTime scoped_stime;
      بنية __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::سمتیہ<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      بنية automatic_hooks_s**شے;
      بنانشانی nitem;
        /* Keep a list of live contexts. */
      vvp_context_t جیوندا_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::سیٹ<vthread_t> دھاگے;
      نشانیوالا صحيح ویلا_units :8;
      نشانیوالا صحيح ویلا_precision :8;

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
			      bool auto_flag, صحيح func_type, بنانشانی func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      صحيح get_type_کوڈ(فراغ) ثابت override { أرجع vpiFunction; }
      صحيح vpi_get(صحيح کوڈ) override
      {
	    بدل (کوڈ) {
		حالة vpiFuncType:
		  أرجع func_type_;
		افتراضي:
		  أرجع __vpiScope::vpi_get(کوڈ);
	    }
      }

    عام:
      مضمن بنانشانی get_func_چوڑائی(فراغ) ثابت { أرجع func_wid_; }
      مضمن vvp_bit4_t get_func_init_val(فراغ) ثابت { أرجع func_init_val_; }

    خاص:
      صحيح func_type_;
      بنانشانی func_wid_;
      vvp_bit4_t func_init_val_;
};

خارجي __vpiScope* vpip_peek_current_scope(فراغ);
خارجي فراغ vpip_attach_منزل_scope(__vpiScope*scope, vpiHandle obj);
خارجي فراغ vpip_attach_منزل_current_scope(vpiHandle obj);
خارجي __vpiScope* vpip_peek_context_scope(فراغ);
خارجي بنانشانی vpip_جوڑو_شے_منزل_context(automatic_hooks_s*شے,
                                         __vpiScope*scope);
خارجي vpiHandle vpip_make_جڑ_iterator(صحيح type_کوڈ);
خارجي فراغ vpip_make_جڑ_iterator(class __vpiHandle**&جدول,
				    بنانشانی&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
بنية __vpiSignal : عام __vpiHandle {
      صحيح vpi_get(صحيح کوڈ) override;
      int64_t vpi_get64(صحيح کوڈ) override;
      حرف* vpi_get_str(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کوڈ) override;
      vpiHandle vpi_iterate(صحيح کوڈ) override;
      vpiHandle vpi_اشاریہ(صحيح idx) override;

    عام:
      بنانشانی چوڑائی() ثابت;
      vpiHandle get_اشاریہ(صحيح اشاریہ);
      فراغ get_ثنائی_ہندسہ_قدر(ثابت بنية __vpiBit*ثنائی_ہندسہ, p_vpi_قدر vp);
      vpiHandle put_ثنائی_ہندسہ_قدر(بنية __vpiBit*ثنائی_ہندسہ, p_vpi_قدر vp, صحيح flags);
      فراغ make_بٹ();

      بنية __vpiBit*بٹ;

    عام:
      اتحاد { // The scope or parent array that contains me.
	    vpiHandle ماپا;
	    __vpiScope* scope;
      } within;
      اتحاد { // The name of this reg/net, or the index for array words.
            ثابت حرف*name;
            vpiHandle اشاریہ;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      بنانشانی signed_flag  : 1;
      بنانشانی is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*گرہ;

    عام:
      ساكن فراغ*عامل new(std::size_t ناپ);
      ساكن فراغ عامل delete(فراغ*); // not implemented
    محمي:
      مضمن __vpiSignal() : بٹ(عدم) { }
    خاص: // Not implemented
      ساكن فراغ*عامل new[] (std::size_t ناپ);
      ساكن فراغ عامل delete[](فراغ*);
};
خارجي بنانشانی vpip_ناپ(ثابت __vpiSignal *sig);
خارجي __vpiScope* vpip_scope(__vpiSignal*sig);

خارجي vpiHandle vpip_make_int2(ثابت حرف*name, صحيح msb, صحيح lsb,
			       bool signed_flag, vvp_net_t*vec);
خارجي vpiHandle vpip_make_int4(ثابت حرف*name, صحيح msb, صحيح lsb,
			       vvp_net_t*vec);
خارجي vpiHandle vpip_make_var4(ثابت حرف*name, صحيح msb, صحيح lsb,
			       bool signed_flag, vvp_net_t*net);
خارجي vpiHandle vpip_make_net4(__vpiScope*scope,
				ثابت حرف*name, صحيح msb, صحيح lsb,
				bool signed_flag, vvp_net_t*گرہ);

/*
 * This is used to represent a bit in a net/reg.
 */
بنية __vpiBit {
      بنية as_ثنائی_ہندسہ_t : عام __vpiHandle {
	    صحيح get_type_کوڈ(فراغ) ثابت;
	    صحيح vpi_get(صحيح کوڈ);
	    حرف* vpi_get_str(صحيح کوڈ);
	    فراغ vpi_get_قدر(p_vpi_قدر val);
	    vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags);
	    vpiHandle vpi_handle(صحيح کوڈ);
      } as_ثنائی_ہندسہ;

      vpiHandle اشاریہ;

      اتحاد {
	    بنية __vpiSignal*ماپا;
	    بنية __vpiBit*bit0;
      };

      مضمن بنانشانی get_norm_اشاریہ() ثابت { أرجع this - bit0; }
      مضمن بنية __vpiSignal*get_ماپا() ثابت {أرجع (bit0 - 1)->ماپا; }
      صحيح get_اشاریہ(فراغ) ثابت;
};

class vpi_پورٹ_ثنائی_ہندسہ_Info;

class vpi_پورٹ_Info  : عام __vpiHandle {
    عام:
      vpi_پورٹ_Info( __vpiScope *ماپا,
                    بنانشانی اشاریہ,
                    صحيح vpi_direction,
                    بنانشانی چوڑائی,
                    ثابت حرف *name,
                    حرف* بفر );
      ~vpi_پورٹ_Info() override;

      صحيح get_type_کوڈ(فراغ) ثابت override { أرجع vpi_پورٹ; }
      صحيح get_direction(فراغ) { أرجع direction_; }
      بنانشانی get_اشاریہ(فراغ) { أرجع اشاریہ_2; }
      صحيح get_چوڑائی(فراغ) { أرجع چوڑائی_2; }
      فراغ جوڑو_پورٹ_ثنائی_ہندسہ(vpi_پورٹ_ثنائی_ہندسہ_Info* پورٹ_ثنائی_ہندسہ) { پورٹ_بٹ.ڈالو_پٹھ(پورٹ_ثنائی_ہندسہ); }

      صحيح vpi_get(صحيح کوڈ) override;
      حرف* vpi_get_str(صحيح کوڈ) override;
      vpiHandle vpi_handle(صحيح کوڈ) override;
      vvp_net_t* get_پورٹ(فراغ) ثابت { أرجع ref_; }
      vpiHandle vpi_iterate(صحيح کوڈ) override;

      std::سمتیہ<vpi_پورٹ_ثنائی_ہندسہ_Info*> پورٹ_بٹ;

    خاص:
      __vpiScope *ماپا_2;
      بنانشانی  اشاریہ_2;
      صحيح       direction_;
      بنانشانی  چوڑائی_2;
      ثابت حرف *name_;
      vvp_net_t *ref_;
};

class vpi_پورٹ_ثنائی_ہندسہ_Info  : عام __vpiHandle {
    عام:
      vpi_پورٹ_ثنائی_ہندسہ_Info(vpi_پورٹ_Info *ماپا,
                     بنانشانی ثنائی_ہندسہ);
      ~vpi_پورٹ_ثنائی_ہندسہ_Info() override;

      صحيح get_type_کوڈ(فراغ) ثابت override { أرجع vpi_پورٹ_ثنائی_ہندسہ; }
      بنانشانی get_ثنائی_ہندسہ(فراغ) ثابت { أرجع ثنائی_ہندسہ_2; }

      صحيح vpi_get(صحيح کوڈ) override;
      vpiHandle vpi_handle(صحيح کوڈ) override;

    خاص:
      vpi_پورٹ_Info *ماپا_2;
      بنانشانی ثنائی_ہندسہ_2;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
بنية __vpiPV : عام __vpiHandle {
      __vpiPV();
      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوڈ) override;
      حرف* vpi_get_str(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کوڈ) override;

      vpiHandle ماپا;
      vvp_net_t*net;
      vpiHandle sbase;
      صحيح tbase;
      بنانشانی چوڑائی;
};
خارجي vpiHandle vpip_make_PV(حرف*name, صحيح ادھار, صحيح چوڑائی);
خارجي vpiHandle vpip_make_PV(حرف*name, حرف*علامت, صحيح چوڑائی);
خارجي vpiHandle vpip_make_PV(حرف*name, vpiHandle handle, صحيح چوڑائی);

بنية __vpiModPathTerm : عام __vpiHandle {
      __vpiModPathTerm();
      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوڈ) override;
      vpiHandle vpi_handle(صحيح کوڈ) override;

      vpiHandle اظہاریہ;
	/* The value returned by vpi_get(vpiEdge, ...); */
      صحيح کنارا;
};

بنية __vpiModPathSrc : عام __vpiHandle {
      __vpiModPathSrc();
      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کوڈ) override;
      vpiHandle vpi_iterate(صحيح کوڈ) override;
      vpiHandle vpi_اشاریہ(صحيح idx) override;
      فراغ vpi_get_delays(p_vpi_delay del) override;
      فراغ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(فراغ) override;

      بنية __vpiModPath *dest;
      صحيح   type;

	/* This is the input expression for this modpath. */
      بنية __vpiModPathTerm رستہ_رکن_وچ;

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

      بنية __vpiModPathTerm رستہ_رکن_باہرلا;
      vvp_net_t *داخلہ_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

خارجي بنية __vpiModPathSrc* vpip_make_modpath_src(بنية __vpiModPath*رستہ,
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
      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کوڈ) override;
      vpiHandle vpi_iterate(صحيح کوڈ) override;
      فراغ vpi_get_delays(p_vpi_delay del) override;
      فراغ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(فراغ) override;

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

خارجي بنية __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpi_پورٹ_Info* port1, vpi_پورٹ_Info* port2);

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
      __vpiNamedEvent& عامل=(ثابت __vpiNamedEvent&) = delete;

      صحيح get_type_کوڈ(فراغ) ثابت override;
      __vpiScope*get_scope(فراغ) ثابت { أرجع scope_; }
      صحيح vpi_get(صحيح کوڈ) override;
      حرف* vpi_get_str(صحيح کوڈ) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کوڈ) override;

      مضمن فراغ جوڑو_vpi_callback(__vpiCallback*cb)
      { cb->اگلا_2 = callbacks_;
	callbacks_ = cb;
      }

      فراغ لگاتار_vpi_callbacks(فراغ);

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
      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوڈ) override;
      حرف* vpi_get_str(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح کوڈ) override;
      vpiHandle vpi_iterate(صحيح کوڈ) override;

      اتحاد { // The scope or parent array that contains me.
	    vpiHandle ماپا;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      اتحاد {
            ثابت حرف*name;
            vpiHandle اشاریہ;
      } id;
      بنانشانی is_netarray  : 1; // This is word of a net array
      بنانشانی is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

خارجي __vpiScope* vpip_scope(__vpiRealVar*sig);
خارجي vpiHandle vpip_make_real_var(ثابت حرف*name, vvp_net_t*net);
خارجي vpiHandle vpip_make_real_net(__vpiScope*scope,
				    ثابت حرف*name, vvp_net_t*net);

class __vpiBaseVar : عام __vpiHandle {

    عام:
      __vpiBaseVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#جے_انت

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

      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
};

خارجي vpiHandle vpip_make_string_var(ثابت حرف*name, vvp_net_t*net);

بنية __vpiArrayBase {
      __vpiArrayBase() : vals_الفاظ(عدم) {}
      مجازی ~__vpiArrayBase() {}

      مجازی بنانشانی get_ناپ(فراغ) ثابت = 0;
      مجازی vpiHandle get_بایاں_range() = 0;
      مجازی vpiHandle get_دایاں_range() = 0;
      مجازی __vpiScope*get_scope() ثابت = 0;

      مجازی صحيح get_لفظ_ناپ() ثابت = 0;
      مجازی حرف*get_لفظ_str(بنية __vpiArrayWord*لفظ, صحيح کوڈ) = 0;
      مجازی فراغ get_لفظ_قدر(بنية __vpiArrayWord*لفظ, p_vpi_قدر vp) = 0;
      مجازی فراغ put_لفظ_قدر(بنية __vpiArrayWord*لفظ, p_vpi_قدر vp,
                                    صحيح flags) = 0;
      مجازی vpiHandle get_iter_اشاریہ(بنية __vpiArrayIterator*iter, صحيح idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_ادھار_iterate(صحيح کوڈ);

      مجازی فراغ make_vals_الفاظ();

      بنية __vpiArrayWord*vals_الفاظ;
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
      صحيح get_type_کوڈ(فراغ) ثابت override { أرجع vpi_حافظہ; }
      بنانشانی get_ناپ() ثابت override { أرجع array_گنتی; }
      vpiHandle get_بایاں_range() override { أكد(nets == 0); أرجع &msb; }
      vpiHandle get_دایاں_range() override { أكد(nets == 0); أرجع &lsb; }
      __vpiScope*get_scope() ثابت override { أرجع scope; }

      صحيح get_لفظ_ناپ() ثابت override;
      حرف*get_لفظ_str(بنية __vpiArrayWord*لفظ, صحيح کوڈ) override;
      فراغ get_لفظ_قدر(بنية __vpiArrayWord*لفظ, p_vpi_قدر vp) override;
      فراغ put_لفظ_قدر(بنية __vpiArrayWord*لفظ, p_vpi_قدر vp, صحيح flags) override;

      vpiHandle get_iter_اشاریہ(بنية __vpiArrayIterator*iter, صحيح idx) override;

      صحيح vpi_get(صحيح کوڈ) override;
      حرف* vpi_get_str(صحيح کوڈ) override;
      vpiHandle vpi_handle(صحيح کوڈ) override;
      مضمن vpiHandle vpi_iterate(صحيح کوڈ) override { أرجع vpi_array_ادھار_iterate(کوڈ); }
      vpiHandle vpi_اشاریہ(صحيح idx) override;

      فراغ سیٹ_لفظ(بنانشانی idx, بنانشانی off, ثابت vvp_vector4_t&val);
      فراغ سیٹ_لفظ(بنانشانی idx, مزدوج val);
      فراغ سیٹ_لفظ(بنانشانی idx, ثابت std::string&val);
      فراغ سیٹ_لفظ(بنانشانی idx, ثابت vvp_object_t&val);

      vvp_vector4_t get_لفظ(بنانشانی پتہ);
      مزدوج get_لفظ_r(بنانشانی پتہ);
      فراغ get_لفظ_obj(بنانشانی پتہ, vvp_object_t&val);
      std::string get_لفظ_str(بنانشانی پتہ);

      فراغ alias_لفظ(بنانشانی لما addr, vpiHandle لفظ, صحيح msb, صحيح lsb);
      فراغ attach_لفظ(بنانشانی addr, vpiHandle لفظ);
      فراغ لفظ_change(بنانشانی لما addr);

      ثابت حرف*name; /* Permanently allocated string */
      __vpiDecConst پہلا_addr;
      __vpiDecConst آخری_addr;
      __vpiDecConst msb;
      __vpiDecConst lsb;
      بنانشانی vals_چوڑائی;
	// If this is a net array, nets lists the handles.
      vpiHandle*nets;
	// If this is a var array, then these are used instead of nets.
      vvp_vector4array_t*vals4;
      vvp_darray        *vals;

      vvp_fun_arrayport*ports_;
      بنية __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool وٹاندرا_addr;

خاص:
      بنانشانی array_گنتی;
      __vpiScope*scope;

یار vpiHandle vpip_make_array(ثابت حرف*label, ثابت حرف*name,
                                 صحيح پہلا_addr, صحيح آخری_addr,
                                 bool signed_flag);
یار فراغ compile_array_alias(حرف*label, حرف*name, حرف*src);
};

class __vpiDarrayVar : عام __vpiBaseVar, عام __vpiArrayBase {
    عام:
      __vpiDarrayVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح get_type_کوڈ() ثابت override { أرجع vpiArrayVar; }
      بنانشانی get_ناپ() ثابت override;
      vpiHandle get_بایاں_range() override;
      vpiHandle get_دایاں_range() override;
      __vpiScope*get_scope() ثابت override { أرجع scope_; }

      صحيح get_لفظ_ناپ() ثابت override;
      حرف*get_لفظ_str(بنية __vpiArrayWord*لفظ, صحيح کوڈ) override;
      فراغ get_لفظ_قدر(بنية __vpiArrayWord*لفظ, p_vpi_قدر vp) override;
      فراغ put_لفظ_قدر(بنية __vpiArrayWord*لفظ, p_vpi_قدر vp, صحيح flags) override;

      vpiHandle get_iter_اشاریہ(بنية __vpiArrayIterator*iter, صحيح idx) override;
      مضمن vpiHandle vpi_iterate(صحيح کوڈ) override { أرجع vpi_array_ادھار_iterate(کوڈ); }

      صحيح vpi_get(صحيح کوڈ) override;
      حرف* vpi_get_str(صحيح کوڈ) override;
      vpiHandle vpi_handle(صحيح کوڈ) override;
      vpiHandle vpi_اشاریہ(صحيح اشاریہ) override;

      فراغ vpi_get_قدر(p_vpi_قدر val) override;

    محمي:
      vvp_darray*get_vvp_darray() ثابت;
      __vpiDecConst بایاں_range, دایاں_range;
};

خارجي vpiHandle vpip_make_darray_var(ثابت حرف*name, vvp_net_t*net);

class __vpiQueueVar : عام __vpiDarrayVar {

    عام:
      __vpiQueueVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح vpi_get(صحيح کوڈ) override;
};

خارجي vpiHandle vpip_make_قطار_var(ثابت حرف*name, vvp_net_t*net);

class __vpiPropQueueRef : عام __vpiHandle {

    عام:
      explicit __vpiPropQueueRef(__vpiScope*scope, بنانشانی pidx, bool is_قطار);

      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوڈ) override;
      حرف* vpi_get_str(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;

      vvp_net_t* class_net_;
      بنانشانی prop_idx_;
      bool is_قطار_2;

    خاص:
      __vpiScope* scope_;
};

خارجي vpiHandle vpip_make_prop_قطار_ref(حرف* class_label,
                                          بنانشانی prop_idx,
                                          بنانشانی is_قطار_flag);
خارجي vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : عام __vpiBaseVar {

    عام:
      __vpiCobjectVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
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
      صحيح get_type_کوڈ(فراغ) ثابت override;

      s_vpi_systf_انکڑے info;
      bool is_user_defn;
};

خارجي vpiHandle vpip_make_systf_iterator(فراغ);

خارجي بنية __vpiUserSystf* vpip_لبھو_systf(ثابت حرف*name);


بنية __vpiSysTaskCall : عام __vpiHandle {

      __vpiScope* scope;
      بنية __vpiUserSystf*defn;
      بنانشانی nargs;
      vpiHandle*دلیلاں;
	/* Stack consumed by this call */
      بنانشانی vec4_ڈھیر;
      بنانشانی real_ڈھیر;
      بنانشانی string_ڈھیر;
	/* Support for vpi_get_userdata. */
      فراغ*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      بنانشانی file_idx;
      بنانشانی lineno;
      bool put_قدر;
    محمي:
      مضمن __vpiSysTaskCall()
      {
	    vec4_ڈھیر = 0;
	    real_ڈھیر = 0;
	    string_ڈھیر = 0;
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
vpiHandle vpip_make_string_param(ثابت حرف*name, حرف*قدر, bool local_flag,
                                 لما file_idx, لما lineno);

بنية __vpiBinaryConst : عام __vpiHandle {
      __vpiBinaryConst();
      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;

      vvp_vector4_t بٹ;
	/* TRUE if this constant is signed. */
      بنانشانی signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      بنانشانی sized_flag  :1;
};

vpiHandle vpip_make_ثنائی_const(بنانشانی wid, ثابت حرف*بٹ);
vpiHandle vpip_make_ثنائی_param(ثابت حرف*name, ثابت vvp_vector4_t&بٹ,
				 bool signed_flag, bool local_flag,
				 لما file_idx, لما lineno);

class __vpiRealConst : عام __vpiHandle {
    عام:
      explicit __vpiRealConst(مزدوج);
      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;

      مزدوج قدر;
};

vpiHandle vpip_make_real_const(مزدوج قدر);
vpiHandle vpip_make_real_param(ثابت حرف*name, مزدوج قدر, bool local_flag,
                               لما file_idx, لما lineno);

class __vpiNullConst : عام __vpiHandle {
    عام:
      explicit __vpiNullConst();
      صحيح get_type_کوڈ(فراغ) ثابت override;
      صحيح vpi_get(صحيح کوڈ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_لفظ(بنانشانی ادھار, ثابت حرف*type);
vpiHandle vpip_make_vthr_str_ڈھیر(بنانشانی ڈونگھائی);
vpiHandle vpip_make_vthr_vec4_ڈھیر(بنانشانی ڈونگھائی, bool signed_flag, بنانشانی wid);

vpiHandle vpip_make_vthr_A(حرف*label, بنانشانی اشاریہ);
vpiHandle vpip_make_vthr_A(حرف*label, حرف*علامت);
vpiHandle vpip_make_vthr_A(حرف*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(حرف*label, بنانشانی اشاریہ, بنانشانی ثنائی_ہندسہ, بنانشانی wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
خارجي "C" فراغ vpip_لدو_module(ثابت حرف*name);

خارجي "C" فراغ vpip_clear_module_paths();
خارجي "C" فراغ vpip_جوڑو_module_رستہ(ثابت حرف *رستہ);
خارجي "C" فراغ vpip_جوڑو_env_and_default_module_paths();

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
				     صحيح val_type, بنانشانی return_چوڑائی,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     بنانشانی argc,
				     vpiHandle*argv,
				     بنانشانی vec4_ڈھیر,
				     بنانشانی real_ڈھیر,
				     بنانشانی string_ڈھیر,
				     لما file_idx,
				     لما lineno);

خارجي vthread_t vpip_current_vthread;

خارجي فراغ vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_ویلا(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

خارجي صحيح vpip_get_ویلا_precision(فراغ);
خارجي فراغ vpip_سیٹ_ویلا_precision(صحيح pres);

خارجي __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

خارجي صحيح vpip_ویلا_units_from_handle(vpiHandle obj);
خارجي صحيح vpip_ویلا_precision_from_handle(vpiHandle obj);

خارجي مزدوج vpip_scaled_ویلا_from_handle(vvp_time64_t ویلا, vpiHandle obj);

خارجي فراغ vpip_ویلا_منزل_timestruct(بنية t_vpi_ویلا*ts, vvp_time64_t ti);
خارجي vvp_time64_t vpip_timestruct_منزل_ویلا(ثابت بنية t_vpi_ویلا*ts);

خارجي مزدوج vpip_ویلا_منزل_scaled_real(vvp_time64_t ti, ثابت __vpiScope*sc);
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
خارجي بنانشانی vpip_vec4_منزل_dec_str(ثابت vvp_vector4_t&vec4,
				     حرف *buf, بنانشانی صحيح nbuf,
				     صحيح signed_flag);


خارجي فراغ vpip_vec4_منزل_hex_str(ثابت vvp_vector4_t&بٹ, حرف*buf,
				 بنانشانی nbuf);

خارجي فراغ vpip_vec4_منزل_oct_str(ثابت vvp_vector4_t&بٹ, حرف*buf,
				 بنانشانی nbuf);

خارجي فراغ vpip_bin_str_منزل_vec4(vvp_vector4_t&val, ثابت حرف*buf);
خارجي فراغ vpip_oct_str_منزل_vec4(vvp_vector4_t&val, ثابت حرف*str);
خارجي فراغ vpip_dec_str_منزل_vec4(vvp_vector4_t&val, ثابت حرف*str);
خارجي فراغ vpip_hex_str_منزل_vec4(vvp_vector4_t&val, ثابت حرف*str);

خارجي vvp_vector4_t vec4_from_vpi_قدر(s_vpi_قدر*vp, بنانشانی wid);
خارجي مزدوج real_from_vpi_قدر(s_vpi_قدر*vp);

خارجي فراغ vpip_vec4_get_قدر(ثابت vvp_vector4_t&لفظ_val, بنانشانی چوڑائی,
				bool signed_flag, s_vpi_قدر*vp);
خارجي فراغ vpip_vec2_get_قدر(ثابت vvp_vector2_t&لفظ_val, بنانشانی چوڑائی,
				bool signed_flag, s_vpi_قدر*vp);
خارجي فراغ vpip_real_get_قدر(مزدوج real, s_vpi_قدر*vp);
خارجي فراغ vpip_string_get_قدر(ثابت std::string&val, s_vpi_قدر*vp);

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
خارجي فراغ *need_نتیجہ_buf(بنانشانی cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
خارجي حرف *simple_سیٹ_rbuf_str(ثابت حرف *s1);
خارجي حرف *generic_get_str(صحيح کوڈ, vpiHandle ref, ثابت حرف *name, ثابت حرف *اشاریہ);

/* A routine to find the enclosing module. */
خارجي vpiHandle vpip_module(__vpiScope*scope);

خارجي صحيح vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
خارجي bool vpip_mcd0_disable;

مضمن uint64_t vlg_گیڑا_منزل_u64(مزدوج rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      أرجع static_cast<uint64_t>(static_cast<int64_t>(std::گیڑا_2(rval)));
}

#جے_انت /* IVL_vpi_priv_H */
