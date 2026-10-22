#جيڪڏهن_وصف_نه_ٿيل IVL_vpi_priv_H
#وصف_ڪريو IVL_vpi_priv_H
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
#جيڪڏهن_پورو

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#وصف_ڪريو vpi_File_سٽ    0x1000003
#وصف_ڪريو _vpiDescription 0x1000004

خارجي bool show_file_سٽ;
خارجي bool ڪوڊ_is_instrumented;

خارجي vpiHandle vpip_build_file_سٽ(ثابت حرف*description,
                                      ڊگهو file_idx, ڊگهو lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
خارجي ثابت حرف* vpi_type_as_string(PLI_INT32 ڪوڊ);

/*
 * Return the port direction as a string.
 */
خارجي ثابت حرف* direction_as_string(صحيح dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#إذا defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
#وصف_ڪريو _vpiFromThr 0x1000001
#   define _vpiNoThr   0
#   define _vpiString  1
#   define _vpiVThr    2
#   define vpi_لفظ    3
#   define _vpi_at_PV  4
#   define _vpi_at_A   5
#   define _vpi_at_APV 6
#جيڪڏهن_پورو


/*
 * The vpi_mode_flag contains the major mode for VPI use. This is used
 * to generate error messages when vpi functions are called
 * incorrectly.
 */
تعداد vpi_گهڻائي_t {
      VPI_گهڻائي_NONE =0,
	/* The compiler is calling a register function. */
      VPI_گهڻائي_رجسٽر,
	/* The compiler is calling a compiletf function. */
      VPI_گهڻائي_COMPILETF,
	/* The compiler is calling a calltf function. */
      VPI_گهڻائي_CALLTF,
	/* We are in the midst of a RWSync callback. */
      VPI_گهڻائي_RWSYNC,
	/* We are in a ROSync callback. */
      VPI_گهڻائي_ROSYNC
};
خارجي vpi_گهڻائي_t vpi_گهڻائي_flag;

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

      مجازي صحيح get_type_ڪوڊ(فراغ) ثابت =0;
      مجازي صحيح vpi_get(صحيح ڪوڊ);
      مجازي int64_t vpi_get64(صحيح ڪوڊ);
      مجازي حرف* vpi_get_str(صحيح ڪوڊ);

      مجازي فراغ vpi_get_قدر(p_vpi_قدر val);
      مجازي vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags);
      مجازي vpiHandle vpi_handle(صحيح ڪوڊ);
      مجازي vpiHandle vpi_iterate(صحيح ڪوڊ);
      مجازي vpiHandle vpi_اشاريو(صحيح idx);
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
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      free_object_fun_t free_object_fun(فراغ) override;

      vpiHandle *دليلون;
      بنانشان  nargs;
      بنانشان  ايندڙ_2;
      bool free_دليلون_flag;
};

خارجي vpiHandle vpip_make_iterator(بنانشان nargs, vpiHandle*دليلون,
				    bool free_دليلون_flag);

class __vpiDecConst : عام __vpiHandle {
    عام:
      explicit __vpiDecConst(صحيح val =0);
      __vpiDecConst(ثابت __vpiDecConst&that);
# إذا __cplusplus >= 201103L
      __vpiDecConst& عامل = (ثابت __vpiDecConst&that) = افتراضي;
# endif
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح ڪوڊ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;

    عام:
      مضمن صحيح get_قدر() ثابت { أرجع قدر; }
      مضمن فراغ سيٽ_قدر(صحيح val) { قدر = val; }

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
      صحيح get_type_ڪوڊ(فراغ) ثابت override;

	// Used for listing callbacks.
      بنية __vpiCallback*ايندڙ_2;

	// user supplied callback data
      بنية t_cb_معلومات cb_معلومات;
};

class قدر_callback : عام __vpiCallback {
    عام:
      explicit قدر_callback(p_cb_معلومات معلومات);
	// Return true if the callback really is ready to be called
      مجازي bool test_قدر_callback_ready(فراغ);

    عام:
	// user supplied callback data
      بنية t_vpi_وقت cb_وقت;
      بنية t_vpi_قدر cb_قدر;
};

خارجي فراغ callback_execute(بنية __vpiCallback*cur);

بنية __vpiSystemTime : عام __vpiHandle {
      __vpiSystemTime();
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح ڪوڊ) override;
      حرف*vpi_get_str(صحيح ڪوڊ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_handle(صحيح ڪوڊ) override;

      __vpiScope*scope;
};

بنية __vpiScopedTime : عام __vpiSystemTime {
      __vpiScopedTime();
      حرف*vpi_get_str(صحيح ڪوڊ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
};
بنية __vpiScopedSTime : عام __vpiSystemTime {
      __vpiScopedSTime();
      صحيح vpi_get(صحيح ڪوڊ) override;
      حرف*vpi_get_str(صحيح ڪوڊ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
};
بنية __vpiScopedRealtime : عام __vpiSystemTime {
      __vpiScopedRealtime();
      صحيح vpi_get(صحيح ڪوڊ) override;
      حرف*vpi_get_str(صحيح ڪوڊ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : عام __vpiHandle {

    عام:
      صحيح vpi_get(صحيح ڪوڊ) override;
      حرف* vpi_get_str(صحيح ڪوڊ) override;
      vpiHandle vpi_handle(صحيح ڪوڊ) override;
      vpiHandle vpi_iterate(صحيح ڪوڊ) override;

    عام:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      مضمن ثابت حرف*scope_name() ثابت { أرجع name_; }

      مضمن ثابت حرف*scope_def_name() ثابت { أرجع tname_; }
	// TRUE if this is an automatic func/task/block
      مضمن bool is_automatic() ثابت { أرجع is_automatic_; }

    عام:
      __vpiScope *scope;
      بنانشان file_idx;
      بنانشان lineno;
      بنانشان def_file_idx;
      بنانشان def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_وقت;
      بنية __vpiScopedSTime scoped_stime;
      بنية __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::ويڪٽر<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      بنية automatic_hooks_s**شي;
      بنانشان nitem;
        /* Keep a list of live contexts. */
      vvp_context_t جيئرو_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::سيٽ<vthread_t> ڌاڳا;
      نشانوارو صحيح وقت_units :8;
      نشانوارو صحيح وقت_precision :8;

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
			      bool auto_flag, صحيح func_type, بنانشان func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiFunction; }
      صحيح vpi_get(صحيح ڪوڊ) override
      {
	    بدل (ڪوڊ) {
		حالة vpiFuncType:
		  أرجع func_type_;
		افتراضي:
		  أرجع __vpiScope::vpi_get(ڪوڊ);
	    }
      }

    عام:
      مضمن بنانشان get_func_ويڪر(فراغ) ثابت { أرجع func_wid_; }
      مضمن vvp_bit4_t get_func_init_val(فراغ) ثابت { أرجع func_init_val_; }

    خاص:
      صحيح func_type_;
      بنانشان func_wid_;
      vvp_bit4_t func_init_val_;
};

خارجي __vpiScope* vpip_peek_current_scope(فراغ);
خارجي فراغ vpip_attach_منزل_scope(__vpiScope*scope, vpiHandle obj);
خارجي فراغ vpip_attach_منزل_current_scope(vpiHandle obj);
خارجي __vpiScope* vpip_peek_context_scope(فراغ);
خارجي بنانشان vpip_جوڙيو_شي_منزل_context(automatic_hooks_s*شي,
                                         __vpiScope*scope);
خارجي vpiHandle vpip_make_پاڙ_iterator(صحيح type_ڪوڊ);
خارجي فراغ vpip_make_پاڙ_iterator(class __vpiHandle**&جدول,
				    بنانشان&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
بنية __vpiSignal : عام __vpiHandle {
      صحيح vpi_get(صحيح ڪوڊ) override;
      int64_t vpi_get64(صحيح ڪوڊ) override;
      حرف* vpi_get_str(صحيح ڪوڊ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح ڪوڊ) override;
      vpiHandle vpi_iterate(صحيح ڪوڊ) override;
      vpiHandle vpi_اشاريو(صحيح idx) override;

    عام:
      بنانشان ويڪر() ثابت;
      vpiHandle get_اشاريو(صحيح اشاريو);
      فراغ get_ٻٽو_انگ_قدر(ثابت بنية __vpiBit*ٻٽو_انگ, p_vpi_قدر vp);
      vpiHandle put_ٻٽو_انگ_قدر(بنية __vpiBit*ٻٽو_انگ, p_vpi_قدر vp, صحيح flags);
      فراغ make_بٽ();

      بنية __vpiBit*بٽ;

    عام:
      اتحاد { // The scope or parent array that contains me.
	    vpiHandle والد;
	    __vpiScope* scope;
      } within;
      اتحاد { // The name of this reg/net, or the index for array words.
            ثابت حرف*name;
            vpiHandle اشاريو;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      بنانشان signed_flag  : 1;
      بنانشان is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*ڳنڍ;

    عام:
      ساكن فراغ*عامل new(std::size_t سائيز);
      ساكن فراغ عامل delete(فراغ*); // not implemented
    محمي:
      مضمن __vpiSignal() : بٽ(عدم) { }
    خاص: // Not implemented
      ساكن فراغ*عامل new[] (std::size_t سائيز);
      ساكن فراغ عامل delete[](فراغ*);
};
خارجي بنانشان vpip_سائيز(ثابت __vpiSignal *sig);
خارجي __vpiScope* vpip_scope(__vpiSignal*sig);

خارجي vpiHandle vpip_make_int2(ثابت حرف*name, صحيح msb, صحيح lsb,
			       bool signed_flag, vvp_net_t*vec);
خارجي vpiHandle vpip_make_int4(ثابت حرف*name, صحيح msb, صحيح lsb,
			       vvp_net_t*vec);
خارجي vpiHandle vpip_make_var4(ثابت حرف*name, صحيح msb, صحيح lsb,
			       bool signed_flag, vvp_net_t*net);
خارجي vpiHandle vpip_make_net4(__vpiScope*scope,
				ثابت حرف*name, صحيح msb, صحيح lsb,
				bool signed_flag, vvp_net_t*ڳنڍ);

/*
 * This is used to represent a bit in a net/reg.
 */
بنية __vpiBit {
      بنية as_ٻٽو_انگ_t : عام __vpiHandle {
	    صحيح get_type_ڪوڊ(فراغ) ثابت;
	    صحيح vpi_get(صحيح ڪوڊ);
	    حرف* vpi_get_str(صحيح ڪوڊ);
	    فراغ vpi_get_قدر(p_vpi_قدر val);
	    vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags);
	    vpiHandle vpi_handle(صحيح ڪوڊ);
      } as_ٻٽو_انگ;

      vpiHandle اشاريو;

      اتحاد {
	    بنية __vpiSignal*والد;
	    بنية __vpiBit*bit0;
      };

      مضمن بنانشان get_norm_اشاريو() ثابت { أرجع this - bit0; }
      مضمن بنية __vpiSignal*get_والد() ثابت {أرجع (bit0 - 1)->والد; }
      صحيح get_اشاريو(فراغ) ثابت;
};

class vpi_پورٽ_ٻٽو_انگ_Info;

class vpi_پورٽ_Info  : عام __vpiHandle {
    عام:
      vpi_پورٽ_Info( __vpiScope *والد,
                    بنانشان اشاريو,
                    صحيح vpi_direction,
                    بنانشان ويڪر,
                    ثابت حرف *name,
                    حرف* بفر );
      ~vpi_پورٽ_Info() override;

      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpi_پورٽ; }
      صحيح get_direction(فراغ) { أرجع direction_; }
      بنانشان get_اشاريو(فراغ) { أرجع اشاريو_2; }
      صحيح get_ويڪر(فراغ) { أرجع ويڪر_2; }
      فراغ جوڙيو_پورٽ_ٻٽو_انگ(vpi_پورٽ_ٻٽو_انگ_Info* پورٽ_ٻٽو_انگ) { پورٽ_بٽ.ڌڪو_پٺ(پورٽ_ٻٽو_انگ); }

      صحيح vpi_get(صحيح ڪوڊ) override;
      حرف* vpi_get_str(صحيح ڪوڊ) override;
      vpiHandle vpi_handle(صحيح ڪوڊ) override;
      vvp_net_t* get_پورٽ(فراغ) ثابت { أرجع ref_; }
      vpiHandle vpi_iterate(صحيح ڪوڊ) override;

      std::ويڪٽر<vpi_پورٽ_ٻٽو_انگ_Info*> پورٽ_بٽ;

    خاص:
      __vpiScope *والد_2;
      بنانشان  اشاريو_2;
      صحيح       direction_;
      بنانشان  ويڪر_2;
      ثابت حرف *name_;
      vvp_net_t *ref_;
};

class vpi_پورٽ_ٻٽو_انگ_Info  : عام __vpiHandle {
    عام:
      vpi_پورٽ_ٻٽو_انگ_Info(vpi_پورٽ_Info *والد,
                     بنانشان ٻٽو_انگ);
      ~vpi_پورٽ_ٻٽو_انگ_Info() override;

      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpi_پورٽ_ٻٽو_انگ; }
      بنانشان get_ٻٽو_انگ(فراغ) ثابت { أرجع ٻٽو_انگ_2; }

      صحيح vpi_get(صحيح ڪوڊ) override;
      vpiHandle vpi_handle(صحيح ڪوڊ) override;

    خاص:
      vpi_پورٽ_Info *والد_2;
      بنانشان ٻٽو_انگ_2;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
بنية __vpiPV : عام __vpiHandle {
      __vpiPV();
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح ڪوڊ) override;
      حرف* vpi_get_str(صحيح ڪوڊ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح ڪوڊ) override;

      vpiHandle والد;
      vvp_net_t*net;
      vpiHandle sbase;
      صحيح tbase;
      بنانشان ويڪر;
};
خارجي vpiHandle vpip_make_PV(حرف*name, صحيح بنياد, صحيح ويڪر);
خارجي vpiHandle vpip_make_PV(حرف*name, حرف*نشان, صحيح ويڪر);
خارجي vpiHandle vpip_make_PV(حرف*name, vpiHandle handle, صحيح ويڪر);

بنية __vpiModPathTerm : عام __vpiHandle {
      __vpiModPathTerm();
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح ڪوڊ) override;
      vpiHandle vpi_handle(صحيح ڪوڊ) override;

      vpiHandle اظهار;
	/* The value returned by vpi_get(vpiEdge, ...); */
      صحيح ڪنارو;
};

بنية __vpiModPathSrc : عام __vpiHandle {
      __vpiModPathSrc();
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح ڪوڊ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح ڪوڊ) override;
      vpiHandle vpi_iterate(صحيح ڪوڊ) override;
      vpiHandle vpi_اشاريو(صحيح idx) override;
      فراغ vpi_get_delays(p_vpi_delay del) override;
      فراغ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(فراغ) override;

      بنية __vpiModPath *dest;
      صحيح   type;

	/* This is the input expression for this modpath. */
      بنية __vpiModPathTerm رستو_رڪن_اندريون;

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

      بنية __vpiModPathTerm رستو_رڪن_ٻاهريون;
      vvp_net_t *داخلا_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

خارجي بنية __vpiModPathSrc* vpip_make_modpath_src(بنية __vpiModPath*رستو,
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
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح ڪوڊ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح ڪوڊ) override;
      vpiHandle vpi_iterate(صحيح ڪوڊ) override;
      فراغ vpi_get_delays(p_vpi_delay del) override;
      فراغ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(فراغ) override;

      __vpiScope   *scope ;

      class vvp_fun_intermodpath*intermodpath;

      vvp_net_t *net;

      vpi_پورٽ_Info* port1;
      vpi_پورٽ_Info* port2;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiInterModPath
 */

خارجي بنية __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpi_پورٽ_Info* port1, vpi_پورٽ_Info* port2);

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

      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      __vpiScope*get_scope(فراغ) ثابت { أرجع scope_; }
      صحيح vpi_get(صحيح ڪوڊ) override;
      حرف* vpi_get_str(صحيح ڪوڊ) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح ڪوڊ) override;

      مضمن فراغ جوڙيو_vpi_callback(__vpiCallback*cb)
      { cb->ايندڙ_2 = callbacks_;
	callbacks_ = cb;
      }

      فراغ لاڳيتو_vpi_callbacks(فراغ);

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
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح ڪوڊ) override;
      حرف* vpi_get_str(صحيح ڪوڊ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح ڪوڊ) override;
      vpiHandle vpi_iterate(صحيح ڪوڊ) override;

      اتحاد { // The scope or parent array that contains me.
	    vpiHandle والد;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      اتحاد {
            ثابت حرف*name;
            vpiHandle اشاريو;
      } id;
      بنانشان is_netarray  : 1; // This is word of a net array
      بنانشان is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

خارجي __vpiScope* vpip_scope(__vpiRealVar*sig);
خارجي vpiHandle vpip_make_real_var(ثابت حرف*name, vvp_net_t*net);
خارجي vpiHandle vpip_make_real_net(__vpiScope*scope,
				    ثابت حرف*name, vvp_net_t*net);

class __vpiBaseVar : عام __vpiHandle {

    عام:
      __vpiBaseVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#جيڪڏهن_پورو

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

      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح ڪوڊ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
};

خارجي vpiHandle vpip_make_string_var(ثابت حرف*name, vvp_net_t*net);

بنية __vpiArrayBase {
      __vpiArrayBase() : vals_لفظ(عدم) {}
      مجازي ~__vpiArrayBase() {}

      مجازي بنانشان get_سائيز(فراغ) ثابت = 0;
      مجازي vpiHandle get_کاٻو_range() = 0;
      مجازي vpiHandle get_ساڄو_range() = 0;
      مجازي __vpiScope*get_scope() ثابت = 0;

      مجازي صحيح get_لفظ_سائيز() ثابت = 0;
      مجازي حرف*get_لفظ_str(بنية __vpiArrayWord*لفظ_2, صحيح ڪوڊ) = 0;
      مجازي فراغ get_لفظ_قدر(بنية __vpiArrayWord*لفظ_2, p_vpi_قدر vp) = 0;
      مجازي فراغ put_لفظ_قدر(بنية __vpiArrayWord*لفظ_2, p_vpi_قدر vp,
                                    صحيح flags) = 0;
      مجازي vpiHandle get_iter_اشاريو(بنية __vpiArrayIterator*iter, صحيح idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_بنياد_iterate(صحيح ڪوڊ);

      مجازي فراغ make_vals_لفظ();

      بنية __vpiArrayWord*vals_لفظ;
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
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpi_يادگيري; }
      بنانشان get_سائيز() ثابت override { أرجع array_ڳڻپ; }
      vpiHandle get_کاٻو_range() override { أكد(nets == 0); أرجع &msb; }
      vpiHandle get_ساڄو_range() override { أكد(nets == 0); أرجع &lsb; }
      __vpiScope*get_scope() ثابت override { أرجع scope; }

      صحيح get_لفظ_سائيز() ثابت override;
      حرف*get_لفظ_str(بنية __vpiArrayWord*لفظ_2, صحيح ڪوڊ) override;
      فراغ get_لفظ_قدر(بنية __vpiArrayWord*لفظ_2, p_vpi_قدر vp) override;
      فراغ put_لفظ_قدر(بنية __vpiArrayWord*لفظ_2, p_vpi_قدر vp, صحيح flags) override;

      vpiHandle get_iter_اشاريو(بنية __vpiArrayIterator*iter, صحيح idx) override;

      صحيح vpi_get(صحيح ڪوڊ) override;
      حرف* vpi_get_str(صحيح ڪوڊ) override;
      vpiHandle vpi_handle(صحيح ڪوڊ) override;
      مضمن vpiHandle vpi_iterate(صحيح ڪوڊ) override { أرجع vpi_array_بنياد_iterate(ڪوڊ); }
      vpiHandle vpi_اشاريو(صحيح idx) override;

      فراغ سيٽ_لفظ(بنانشان idx, بنانشان off, ثابت vvp_vector4_t&val);
      فراغ سيٽ_لفظ(بنانشان idx, مزدوج val);
      فراغ سيٽ_لفظ(بنانشان idx, ثابت std::string&val);
      فراغ سيٽ_لفظ(بنانشان idx, ثابت vvp_object_t&val);

      vvp_vector4_t get_لفظ(بنانشان پتو);
      مزدوج get_لفظ_r(بنانشان پتو);
      فراغ get_لفظ_obj(بنانشان پتو, vvp_object_t&val);
      std::string get_لفظ_str(بنانشان پتو);

      فراغ alias_لفظ(بنانشان ڊگهو addr, vpiHandle لفظ_2, صحيح msb, صحيح lsb);
      فراغ attach_لفظ(بنانشان addr, vpiHandle لفظ_2);
      فراغ لفظ_change(بنانشان ڊگهو addr);

      ثابت حرف*name; /* Permanently allocated string */
      __vpiDecConst پهريون_addr;
      __vpiDecConst آخري_addr;
      __vpiDecConst msb;
      __vpiDecConst lsb;
      بنانشان vals_ويڪر;
	// If this is a net array, nets lists the handles.
      vpiHandle*nets;
	// If this is a var array, then these are used instead of nets.
      vvp_vector4array_t*vals4;
      vvp_darray        *vals;

      vvp_fun_arrayport*ports_;
      بنية __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool مٽا_سٽا_addr;

خاص:
      بنانشان array_ڳڻپ;
      __vpiScope*scope;

دوست vpiHandle vpip_make_array(ثابت حرف*label, ثابت حرف*name,
                                 صحيح پهريون_addr, صحيح آخري_addr,
                                 bool signed_flag);
دوست فراغ compile_array_alias(حرف*label, حرف*name, حرف*src);
};

class __vpiDarrayVar : عام __vpiBaseVar, عام __vpiArrayBase {
    عام:
      __vpiDarrayVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح get_type_ڪوڊ() ثابت override { أرجع vpiArrayVar; }
      بنانشان get_سائيز() ثابت override;
      vpiHandle get_کاٻو_range() override;
      vpiHandle get_ساڄو_range() override;
      __vpiScope*get_scope() ثابت override { أرجع scope_; }

      صحيح get_لفظ_سائيز() ثابت override;
      حرف*get_لفظ_str(بنية __vpiArrayWord*لفظ_2, صحيح ڪوڊ) override;
      فراغ get_لفظ_قدر(بنية __vpiArrayWord*لفظ_2, p_vpi_قدر vp) override;
      فراغ put_لفظ_قدر(بنية __vpiArrayWord*لفظ_2, p_vpi_قدر vp, صحيح flags) override;

      vpiHandle get_iter_اشاريو(بنية __vpiArrayIterator*iter, صحيح idx) override;
      مضمن vpiHandle vpi_iterate(صحيح ڪوڊ) override { أرجع vpi_array_بنياد_iterate(ڪوڊ); }

      صحيح vpi_get(صحيح ڪوڊ) override;
      حرف* vpi_get_str(صحيح ڪوڊ) override;
      vpiHandle vpi_handle(صحيح ڪوڊ) override;
      vpiHandle vpi_اشاريو(صحيح اشاريو) override;

      فراغ vpi_get_قدر(p_vpi_قدر val) override;

    محمي:
      vvp_darray*get_vvp_darray() ثابت;
      __vpiDecConst کاٻو_range, ساڄو_range;
};

خارجي vpiHandle vpip_make_darray_var(ثابت حرف*name, vvp_net_t*net);

class __vpiQueueVar : عام __vpiDarrayVar {

    عام:
      __vpiQueueVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح vpi_get(صحيح ڪوڊ) override;
};

خارجي vpiHandle vpip_make_قطار_var(ثابت حرف*name, vvp_net_t*net);

class __vpiPropQueueRef : عام __vpiHandle {

    عام:
      explicit __vpiPropQueueRef(__vpiScope*scope, بنانشان pidx, bool is_قطار);

      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح ڪوڊ) override;
      حرف* vpi_get_str(صحيح ڪوڊ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;

      vvp_net_t* class_net_;
      بنانشان prop_idx_;
      bool is_قطار_2;

    خاص:
      __vpiScope* scope_;
};

خارجي vpiHandle vpip_make_prop_قطار_ref(حرف* class_label,
                                          بنانشان prop_idx,
                                          بنانشان is_قطار_flag);
خارجي vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : عام __vpiBaseVar {

    عام:
      __vpiCobjectVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح ڪوڊ) override;
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
      صحيح get_type_ڪوڊ(فراغ) ثابت override;

      s_vpi_systf_معلومات info;
      bool is_user_defn;
};

خارجي vpiHandle vpip_make_systf_iterator(فراغ);

خارجي بنية __vpiUserSystf* vpip_ڳوليو_systf(ثابت حرف*name);


بنية __vpiSysTaskCall : عام __vpiHandle {

      __vpiScope* scope;
      بنية __vpiUserSystf*defn;
      بنانشان nargs;
      vpiHandle*دليلون;
	/* Stack consumed by this call */
      بنانشان vec4_ڍير;
      بنانشان real_ڍير;
      بنانشان string_ڍير;
	/* Support for vpi_get_userdata. */
      فراغ*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      بنانشان file_idx;
      بنانشان lineno;
      bool put_قدر;
    محمي:
      مضمن __vpiSysTaskCall()
      {
	    vec4_ڍير = 0;
	    real_ڍير = 0;
	    string_ڍير = 0;
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
                                 ڊگهو file_idx, ڊگهو lineno);

بنية __vpiBinaryConst : عام __vpiHandle {
      __vpiBinaryConst();
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح ڪوڊ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;

      vvp_vector4_t بٽ;
	/* TRUE if this constant is signed. */
      بنانشان signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      بنانشان sized_flag  :1;
};

vpiHandle vpip_make_ٻٽو_const(بنانشان wid, ثابت حرف*بٽ);
vpiHandle vpip_make_ٻٽو_param(ثابت حرف*name, ثابت vvp_vector4_t&بٽ,
				 bool signed_flag, bool local_flag,
				 ڊگهو file_idx, ڊگهو lineno);

class __vpiRealConst : عام __vpiHandle {
    عام:
      explicit __vpiRealConst(مزدوج);
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح ڪوڊ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;

      مزدوج قدر;
};

vpiHandle vpip_make_real_const(مزدوج قدر);
vpiHandle vpip_make_real_param(ثابت حرف*name, مزدوج قدر, bool local_flag,
                               ڊگهو file_idx, ڊگهو lineno);

class __vpiNullConst : عام __vpiHandle {
    عام:
      explicit __vpiNullConst();
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح ڪوڊ) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_لفظ(بنانشان بنياد, ثابت حرف*type);
vpiHandle vpip_make_vthr_str_ڍير(بنانشان اونهائي);
vpiHandle vpip_make_vthr_vec4_ڍير(بنانشان اونهائي, bool signed_flag, بنانشان wid);

vpiHandle vpip_make_vthr_A(حرف*label, بنانشان اشاريو);
vpiHandle vpip_make_vthr_A(حرف*label, حرف*نشان);
vpiHandle vpip_make_vthr_A(حرف*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(حرف*label, بنانشان اشاريو, بنانشان ٻٽو_انگ, بنانشان wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
خارجي "C" فراغ vpip_ڀريو_module(ثابت حرف*name);

خارجي "C" فراغ vpip_clear_module_paths();
خارجي "C" فراغ vpip_جوڙيو_module_رستو(ثابت حرف *رستو);
خارجي "C" فراغ vpip_جوڙيو_env_and_default_module_paths();

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
				     صحيح val_type, بنانشان return_ويڪر,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     بنانشان argc,
				     vpiHandle*argv,
				     بنانشان vec4_ڍير,
				     بنانشان real_ڍير,
				     بنانشان string_ڍير,
				     ڊگهو file_idx,
				     ڊگهو lineno);

خارجي vthread_t vpip_current_vthread;

خارجي فراغ vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_وقت(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

خارجي صحيح vpip_get_وقت_precision(فراغ);
خارجي فراغ vpip_سيٽ_وقت_precision(صحيح pres);

خارجي __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

خارجي صحيح vpip_وقت_units_from_handle(vpiHandle obj);
خارجي صحيح vpip_وقت_precision_from_handle(vpiHandle obj);

خارجي مزدوج vpip_scaled_وقت_from_handle(vvp_time64_t وقت, vpiHandle obj);

خارجي فراغ vpip_وقت_منزل_timestruct(بنية t_vpi_وقت*ts, vvp_time64_t ti);
خارجي vvp_time64_t vpip_timestruct_منزل_وقت(ثابت بنية t_vpi_وقت*ts);

خارجي مزدوج vpip_وقت_منزل_scaled_real(vvp_time64_t ti, ثابت __vpiScope*sc);
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
خارجي بنانشان vpip_vec4_منزل_dec_str(ثابت vvp_vector4_t&vec4,
				     حرف *buf, بنانشان صحيح nbuf,
				     صحيح signed_flag);


خارجي فراغ vpip_vec4_منزل_hex_str(ثابت vvp_vector4_t&بٽ, حرف*buf,
				 بنانشان nbuf);

خارجي فراغ vpip_vec4_منزل_oct_str(ثابت vvp_vector4_t&بٽ, حرف*buf,
				 بنانشان nbuf);

خارجي فراغ vpip_bin_str_منزل_vec4(vvp_vector4_t&val, ثابت حرف*buf);
خارجي فراغ vpip_oct_str_منزل_vec4(vvp_vector4_t&val, ثابت حرف*str);
خارجي فراغ vpip_dec_str_منزل_vec4(vvp_vector4_t&val, ثابت حرف*str);
خارجي فراغ vpip_hex_str_منزل_vec4(vvp_vector4_t&val, ثابت حرف*str);

خارجي vvp_vector4_t vec4_from_vpi_قدر(s_vpi_قدر*vp, بنانشان wid);
خارجي مزدوج real_from_vpi_قدر(s_vpi_قدر*vp);

خارجي فراغ vpip_vec4_get_قدر(ثابت vvp_vector4_t&لفظ_val, بنانشان ويڪر,
				bool signed_flag, s_vpi_قدر*vp);
خارجي فراغ vpip_vec2_get_قدر(ثابت vvp_vector2_t&لفظ_val, بنانشان ويڪر,
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
خارجي فراغ *need_نتيجو_buf(بنانشان cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
خارجي حرف *simple_سيٽ_rbuf_str(ثابت حرف *s1);
خارجي حرف *generic_get_str(صحيح ڪوڊ, vpiHandle ref, ثابت حرف *name, ثابت حرف *اشاريو);

/* A routine to find the enclosing module. */
خارجي vpiHandle vpip_module(__vpiScope*scope);

خارجي صحيح vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
خارجي bool vpip_mcd0_disable;

مضمن uint64_t vlg_ڦيرو_منزل_u64(مزدوج rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      أرجع static_cast<uint64_t>(static_cast<int64_t>(std::ڦيرو_2(rval)));
}

#جيڪڏهن_پورو /* IVL_vpi_priv_H */
