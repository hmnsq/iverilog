#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_vpi_priv_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_vpi_priv_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ  "delay.h"


class class_type;
class vvp_darray;
class vvp_fun_arrayport;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ __vpiArray* vvp_array_t;

/*
 * This header file contains the internal definitions that the vvp
 * program uses to implement the public interface in the vpi_user.h
 * header file elsewhere.
 */

#ਜੇ defined(__MINGW32__) || defined (__CYGWIN__)
ਬਾਹਰੀ vpip_routines_s vpi_routines;
#ਜੇ_ਅੰਤ

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vpi_File_ਸਤਰ    0x1000003
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _vpiDescription 0x1000004

ਬਾਹਰੀ bool show_file_ਸਤਰ;
ਬਾਹਰੀ bool ਕੋਡ_is_instrumented;

ਬਾਹਰੀ vpiHandle vpip_build_file_ਸਤਰ(ਅਚਲ ਅੱਖਰ*description,
                                      ਲੰਮਾ file_idx, ਲੰਮਾ lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ* vpi_type_as_string(PLI_INT32 ਕੋਡ);

/*
 * Return the port direction as a string.
 */
ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ* direction_as_string(ਪੂਰਨ dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#ਜੇ defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _vpiFromThr 0x1000001
#   define _vpiNoThr   0
#   define _vpiString  1
#   define _vpiVThr    2
#   define vpi_ਸ਼ਬਦ    3
#   define _vpi_at_PV  4
#   define _vpi_at_A   5
#   define _vpi_at_APV 6
#ਜੇ_ਅੰਤ


/*
 * The vpi_mode_flag contains the major mode for VPI use. This is used
 * to generate error messages when vpi functions are called
 * incorrectly.
 */
ਗਿਣਤੀ vpi_ਬਹੁਲਕ_t {
      VPI_ਬਹੁਲਕ_NONE =0,
	/* The compiler is calling a register function. */
      VPI_ਬਹੁਲਕ_ਰਜਿਸਟਰ,
	/* The compiler is calling a compiletf function. */
      VPI_ਬਹੁਲਕ_COMPILETF,
	/* The compiler is calling a calltf function. */
      VPI_ਬਹੁਲਕ_CALLTF,
	/* We are in the midst of a RWSync callback. */
      VPI_ਬਹੁਲਕ_RWSYNC,
	/* We are in a ROSync callback. */
      VPI_ਬਹੁਲਕ_ROSYNC
};
ਬਾਹਰੀ vpi_ਬਹੁਲਕ_t vpi_ਬਹੁਲਕ_flag;

/*
 * This structure is the very base of a vpiHandle. Every handle
 * structure is derived from this class so that the library can
 * internally pass the derived types as pointers to one of these.
 */
class __vpiHandle {
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ __vpiHandle() { }
	// The destructor is virtual so that dynamic types will work.
      ਆਭਾਸੀ ~__vpiHandle();

      ਆਭਾਸੀ ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ =0;
      ਆਭਾਸੀ ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ);
      ਆਭਾਸੀ int64_t vpi_get64(ਪੂਰਨ ਕੋਡ);
      ਆਭਾਸੀ ਅੱਖਰ* vpi_get_str(ਪੂਰਨ ਕੋਡ);

      ਆਭਾਸੀ ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val);
      ਆਭਾਸੀ vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags);
      ਆਭਾਸੀ vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ);
      ਆਭਾਸੀ vpiHandle vpi_iterate(ਪੂਰਨ ਕੋਡ);
      ਆਭਾਸੀ vpiHandle vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ idx);
      ਆਭਾਸੀ ਖਾਲੀ vpi_get_delays(p_vpi_delay del);
      ਆਭਾਸੀ ਖਾਲੀ vpi_put_delays(p_vpi_delay del);

	// Objects may have destroyer functions of their own. If so,
	// then this virtual method will return a POINTER to that
	// function. The pointer is used to "delete" the object, which
	// is why the function itself cannot be a method.
      ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*free_object_fun_t)(vpiHandle);
      ਆਭਾਸੀ free_object_fun_t free_object_fun(ਖਾਲੀ);
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
ਬਣਤਰ __vpiIterator : ਜਨਤਕ __vpiHandle {
      __vpiIterator();
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      free_object_fun_t free_object_fun(ਖਾਲੀ) override;

      vpiHandle *ਦਲੀਲਾਂ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ  nargs;
      ਬਿਨਾਨਿਸ਼ਾਨੀ  ਅਗਲਾ_2;
      bool free_ਦਲੀਲਾਂ_flag;
};

ਬਾਹਰੀ vpiHandle vpip_make_iterator(ਬਿਨਾਨਿਸ਼ਾਨੀ nargs, vpiHandle*ਦਲੀਲਾਂ,
				    bool free_ਦਲੀਲਾਂ_flag);

class __vpiDecConst : ਜਨਤਕ __vpiHandle {
    ਜਨਤਕ:
      explicit __vpiDecConst(ਪੂਰਨ val =0);
      __vpiDecConst(ਅਚਲ __vpiDecConst&that);
# ਜੇ __cplusplus >= 201103L
      __vpiDecConst& ਸੰਚਾਲਕ = (ਅਚਲ __vpiDecConst&that) = ਮੂਲ;
# endif
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;

    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ ਪੂਰਨ get_ਮੁੱਲ() ਅਚਲ { ਵਾਪਸ ਮੁੱਲ; }
      ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੈੱਟ_ਮੁੱਲ(ਪੂਰਨ val) { ਮੁੱਲ = val; }

    ਨਿਜੀ:
      ਪੂਰਨ ਮੁੱਲ;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
ਬਣਤਰ __vpiCallback : ਜਨਤਕ __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;

	// Used for listing callbacks.
      ਬਣਤਰ __vpiCallback*ਅਗਲਾ_2;

	// user supplied callback data
      ਬਣਤਰ t_cb_ਅੰਕੜੇ cb_ਅੰਕੜੇ;
};

class ਮੁੱਲ_callback : ਜਨਤਕ __vpiCallback {
    ਜਨਤਕ:
      explicit ਮੁੱਲ_callback(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ);
	// Return true if the callback really is ready to be called
      ਆਭਾਸੀ bool test_ਮੁੱਲ_callback_ready(ਖਾਲੀ);

    ਜਨਤਕ:
	// user supplied callback data
      ਬਣਤਰ t_vpi_ਸਮਾਂ cb_ਸਮਾਂ;
      ਬਣਤਰ t_vpi_ਮੁੱਲ cb_ਮੁੱਲ;
};

ਬਾਹਰੀ ਖਾਲੀ callback_execute(ਬਣਤਰ __vpiCallback*cur);

ਬਣਤਰ __vpiSystemTime : ਜਨਤਕ __vpiHandle {
      __vpiSystemTime();
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਅੱਖਰ*vpi_get_str(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
      vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ) override;

      __vpiScope*scope;
};

ਬਣਤਰ __vpiScopedTime : ਜਨਤਕ __vpiSystemTime {
      __vpiScopedTime();
      ਅੱਖਰ*vpi_get_str(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
};
ਬਣਤਰ __vpiScopedSTime : ਜਨਤਕ __vpiSystemTime {
      __vpiScopedSTime();
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਅੱਖਰ*vpi_get_str(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
};
ਬਣਤਰ __vpiScopedRealtime : ਜਨਤਕ __vpiSystemTime {
      __vpiScopedRealtime();
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਅੱਖਰ*vpi_get_str(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : ਜਨਤਕ __vpiHandle {

    ਜਨਤਕ:
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਅੱਖਰ* vpi_get_str(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_iterate(ਪੂਰਨ ਕੋਡ) override;

    ਜਨਤਕ:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ*scope_name() ਅਚਲ { ਵਾਪਸ name_; }

      ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ*scope_def_name() ਅਚਲ { ਵਾਪਸ tname_; }
	// TRUE if this is an automatic func/task/block
      ਸਤਰਅੰਦਰ bool is_automatic() ਅਚਲ { ਵਾਪਸ is_automatic_; }

    ਜਨਤਕ:
      __vpiScope *scope;
      ਬਿਨਾਨਿਸ਼ਾਨੀ file_idx;
      ਬਿਨਾਨਿਸ਼ਾਨੀ lineno;
      ਬਿਨਾਨਿਸ਼ਾਨੀ def_file_idx;
      ਬਿਨਾਨਿਸ਼ਾਨੀ def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_ਸਮਾਂ;
      ਬਣਤਰ __vpiScopedSTime scoped_stime;
      ਬਣਤਰ __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::ਸਦਿਸ਼<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      ਬਣਤਰ automatic_hooks_s**ਵਸਤੂ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ nitem;
        /* Keep a list of live contexts. */
      vvp_context_t ਜੀਵਤ_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::ਸੈੱਟ<vthread_t> ਧਾਗੇ;
      ਨਿਸ਼ਾਨੀਵਾਲਾ ਪੂਰਨ ਸਮਾਂ_units :8;
      ਨਿਸ਼ਾਨੀਵਾਲਾ ਪੂਰਨ ਸਮਾਂ_precision :8;

    ਸੁਰੱਖਿਅਤ:
      __vpiScope(ਅਚਲ ਅੱਖਰ*nam, ਅਚਲ ਅੱਖਰ*tnam, bool is_auto_flag =false);

    ਨਿਜੀ:
	/* The scope has a name. */
      ਅਚਲ ਅੱਖਰ*name_;
      ਅਚਲ ਅੱਖਰ*tname_;
	/* the scope may be "automatic" */
      bool is_automatic_;
};

class vpiScopeFunction  : ਜਨਤਕ __vpiScope {
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ vpiScopeFunction(ਅਚਲ ਅੱਖਰ*nam, ਅਚਲ ਅੱਖਰ*tnam,
			      bool auto_flag, ਪੂਰਨ func_type, ਬਿਨਾਨਿਸ਼ਾਨੀ func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiFunction; }
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override
      {
	    ਚੋਣ (ਕੋਡ) {
		ਹਾਲਤ vpiFuncType:
		  ਵਾਪਸ func_type_;
		ਮੂਲ:
		  ਵਾਪਸ __vpiScope::vpi_get(ਕੋਡ);
	    }
      }

    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ get_func_ਚੌੜਾਈ(ਖਾਲੀ) ਅਚਲ { ਵਾਪਸ func_wid_; }
      ਸਤਰਅੰਦਰ vvp_bit4_t get_func_init_val(ਖਾਲੀ) ਅਚਲ { ਵਾਪਸ func_init_val_; }

    ਨਿਜੀ:
      ਪੂਰਨ func_type_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ func_wid_;
      vvp_bit4_t func_init_val_;
};

ਬਾਹਰੀ __vpiScope* vpip_peek_current_scope(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ vpip_attach_ਮੰਜ਼ਿਲ_scope(__vpiScope*scope, vpiHandle obj);
ਬਾਹਰੀ ਖਾਲੀ vpip_attach_ਮੰਜ਼ਿਲ_current_scope(vpiHandle obj);
ਬਾਹਰੀ __vpiScope* vpip_peek_context_scope(ਖਾਲੀ);
ਬਾਹਰੀ ਬਿਨਾਨਿਸ਼ਾਨੀ vpip_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_context(automatic_hooks_s*ਵਸਤੂ,
                                         __vpiScope*scope);
ਬਾਹਰੀ vpiHandle vpip_make_ਜੜ੍ਹ_iterator(ਪੂਰਨ type_ਕੋਡ);
ਬਾਹਰੀ ਖਾਲੀ vpip_make_ਜੜ੍ਹ_iterator(class __vpiHandle**&ਸਾਰਣੀ,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
ਬਣਤਰ __vpiSignal : ਜਨਤਕ __vpiHandle {
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      int64_t vpi_get64(ਪੂਰਨ ਕੋਡ) override;
      ਅੱਖਰ* vpi_get_str(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
      vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags) override;
      vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_iterate(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ idx) override;

    ਜਨਤਕ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ() ਅਚਲ;
      vpiHandle get_ਸੂਚਕਾਂਕ(ਪੂਰਨ ਸੂਚਕਾਂਕ);
      ਖਾਲੀ get_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮੁੱਲ(ਅਚਲ ਬਣਤਰ __vpiBit*ਦੋਆਧਾਰੀ_ਅੰਕ, p_vpi_ਮੁੱਲ vp);
      vpiHandle put_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮੁੱਲ(ਬਣਤਰ __vpiBit*ਦੋਆਧਾਰੀ_ਅੰਕ, p_vpi_ਮੁੱਲ vp, ਪੂਰਨ flags);
      ਖਾਲੀ make_ਬਿੱਟ();

      ਬਣਤਰ __vpiBit*ਬਿੱਟ;

    ਜਨਤਕ:
      ਸੰਘ { // The scope or parent array that contains me.
	    vpiHandle ਮਾਪਾ;
	    __vpiScope* scope;
      } within;
      ਸੰਘ { // The name of this reg/net, or the index for array words.
            ਅਚਲ ਅੱਖਰ*name;
            vpiHandle ਸੂਚਕਾਂਕ;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      ਬਿਨਾਨਿਸ਼ਾਨੀ signed_flag  : 1;
      ਬਿਨਾਨਿਸ਼ਾਨੀ is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*ਗੰਢ;

    ਜਨਤਕ:
      ਸਥਿਰ ਖਾਲੀ*ਸੰਚਾਲਕ new(std::size_t ਮਾਪ);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ delete(ਖਾਲੀ*); // not implemented
    ਸੁਰੱਖਿਅਤ:
      ਸਤਰਅੰਦਰ __vpiSignal() : ਬਿੱਟ(ਨਿਰਾ) { }
    ਨਿਜੀ: // Not implemented
      ਸਥਿਰ ਖਾਲੀ*ਸੰਚਾਲਕ new[] (std::size_t ਮਾਪ);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ delete[](ਖਾਲੀ*);
};
ਬਾਹਰੀ ਬਿਨਾਨਿਸ਼ਾਨੀ vpip_ਮਾਪ(ਅਚਲ __vpiSignal *sig);
ਬਾਹਰੀ __vpiScope* vpip_scope(__vpiSignal*sig);

ਬਾਹਰੀ vpiHandle vpip_make_int2(ਅਚਲ ਅੱਖਰ*name, ਪੂਰਨ msb, ਪੂਰਨ lsb,
			       bool signed_flag, vvp_net_t*vec);
ਬਾਹਰੀ vpiHandle vpip_make_int4(ਅਚਲ ਅੱਖਰ*name, ਪੂਰਨ msb, ਪੂਰਨ lsb,
			       vvp_net_t*vec);
ਬਾਹਰੀ vpiHandle vpip_make_var4(ਅਚਲ ਅੱਖਰ*name, ਪੂਰਨ msb, ਪੂਰਨ lsb,
			       bool signed_flag, vvp_net_t*net);
ਬਾਹਰੀ vpiHandle vpip_make_net4(__vpiScope*scope,
				ਅਚਲ ਅੱਖਰ*name, ਪੂਰਨ msb, ਪੂਰਨ lsb,
				bool signed_flag, vvp_net_t*ਗੰਢ);

/*
 * This is used to represent a bit in a net/reg.
 */
ਬਣਤਰ __vpiBit {
      ਬਣਤਰ as_ਦੋਆਧਾਰੀ_ਅੰਕ_t : ਜਨਤਕ __vpiHandle {
	    ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ;
	    ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ);
	    ਅੱਖਰ* vpi_get_str(ਪੂਰਨ ਕੋਡ);
	    ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val);
	    vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags);
	    vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ);
      } as_ਦੋਆਧਾਰੀ_ਅੰਕ;

      vpiHandle ਸੂਚਕਾਂਕ;

      ਸੰਘ {
	    ਬਣਤਰ __vpiSignal*ਮਾਪਾ;
	    ਬਣਤਰ __vpiBit*bit0;
      };

      ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ get_norm_ਸੂਚਕਾਂਕ() ਅਚਲ { ਵਾਪਸ this - bit0; }
      ਸਤਰਅੰਦਰ ਬਣਤਰ __vpiSignal*get_ਮਾਪਾ() ਅਚਲ {ਵਾਪਸ (bit0 - 1)->ਮਾਪਾ; }
      ਪੂਰਨ get_ਸੂਚਕਾਂਕ(ਖਾਲੀ) ਅਚਲ;
};

class vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info;

class vpi_ਪੋਰਟ_Info  : ਜਨਤਕ __vpiHandle {
    ਜਨਤਕ:
      vpi_ਪੋਰਟ_Info( __vpiScope *ਮਾਪਾ,
                    ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ,
                    ਪੂਰਨ vpi_direction,
                    ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ,
                    ਅਚਲ ਅੱਖਰ *name,
                    ਅੱਖਰ* ਬਫਰ );
      ~vpi_ਪੋਰਟ_Info() override;

      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpi_ਪੋਰਟ; }
      ਪੂਰਨ get_direction(ਖਾਲੀ) { ਵਾਪਸ direction_; }
      ਬਿਨਾਨਿਸ਼ਾਨੀ get_ਸੂਚਕਾਂਕ(ਖਾਲੀ) { ਵਾਪਸ ਸੂਚਕਾਂਕ_2; }
      ਪੂਰਨ get_ਚੌੜਾਈ(ਖਾਲੀ) { ਵਾਪਸ ਚੌੜਾਈ_2; }
      ਖਾਲੀ ਜੋੜੋ_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info* ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ) { ਪੋਰਟ_ਬਿੱਟ.ਧੱਕੋ_ਪਿੱਠ(ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ); }

      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਅੱਖਰ* vpi_get_str(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ) override;
      vvp_net_t* get_ਪੋਰਟ(ਖਾਲੀ) ਅਚਲ { ਵਾਪਸ ref_; }
      vpiHandle vpi_iterate(ਪੂਰਨ ਕੋਡ) override;

      std::ਸਦਿਸ਼<vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info*> ਪੋਰਟ_ਬਿੱਟ;

    ਨਿਜੀ:
      __vpiScope *ਮਾਪਾ_2;
      ਬਿਨਾਨਿਸ਼ਾਨੀ  ਸੂਚਕਾਂਕ_2;
      ਪੂਰਨ       direction_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ  ਚੌੜਾਈ_2;
      ਅਚਲ ਅੱਖਰ *name_;
      vvp_net_t *ref_;
};

class vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info  : ਜਨਤਕ __vpiHandle {
    ਜਨਤਕ:
      vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info(vpi_ਪੋਰਟ_Info *ਮਾਪਾ,
                     ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ);
      ~vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info() override;

      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ; }
      ਬਿਨਾਨਿਸ਼ਾਨੀ get_ਦੋਆਧਾਰੀ_ਅੰਕ(ਖਾਲੀ) ਅਚਲ { ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_2; }

      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ) override;

    ਨਿਜੀ:
      vpi_ਪੋਰਟ_Info *ਮਾਪਾ_2;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ_2;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
ਬਣਤਰ __vpiPV : ਜਨਤਕ __vpiHandle {
      __vpiPV();
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਅੱਖਰ* vpi_get_str(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
      vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags) override;
      vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ) override;

      vpiHandle ਮਾਪਾ;
      vvp_net_t*net;
      vpiHandle sbase;
      ਪੂਰਨ tbase;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ;
};
ਬਾਹਰੀ vpiHandle vpip_make_PV(ਅੱਖਰ*name, ਪੂਰਨ ਅਧਾਰ, ਪੂਰਨ ਚੌੜਾਈ);
ਬਾਹਰੀ vpiHandle vpip_make_PV(ਅੱਖਰ*name, ਅੱਖਰ*ਚਿੰਨ੍ਹ, ਪੂਰਨ ਚੌੜਾਈ);
ਬਾਹਰੀ vpiHandle vpip_make_PV(ਅੱਖਰ*name, vpiHandle handle, ਪੂਰਨ ਚੌੜਾਈ);

ਬਣਤਰ __vpiModPathTerm : ਜਨਤਕ __vpiHandle {
      __vpiModPathTerm();
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ) override;

      vpiHandle ਵਿਅੰਜਕ;
	/* The value returned by vpi_get(vpiEdge, ...); */
      ਪੂਰਨ ਕਿਨਾਰਾ;
};

ਬਣਤਰ __vpiModPathSrc : ਜਨਤਕ __vpiHandle {
      __vpiModPathSrc();
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
      vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags) override;
      vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_iterate(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ idx) override;
      ਖਾਲੀ vpi_get_delays(p_vpi_delay del) override;
      ਖਾਲੀ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(ਖਾਲੀ) override;

      ਬਣਤਰ __vpiModPath *dest;
      ਪੂਰਨ   type;

	/* This is the input expression for this modpath. */
      ਬਣਤਰ __vpiModPathTerm ਰਸਤਾ_ਪਦ_ਵਿੱਚ;

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

ਬਣਤਰ __vpiModPath {
      __vpiScope   *scope ;

      class vvp_fun_modpath*modpath;

      ਬਣਤਰ __vpiModPathTerm ਰਸਤਾ_ਪਦ_ਬਾਹਰਲਾ;
      vvp_net_t *ਨਿਵੇਸ਼_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

ਬਾਹਰੀ ਬਣਤਰ __vpiModPathSrc* vpip_make_modpath_src(ਬਣਤਰ __vpiModPath*ਰਸਤਾ,
                                                     vvp_net_t *net) ;

ਬਾਹਰੀ ਬਣਤਰ __vpiModPath* vpip_make_modpath(vvp_net_t *net) ;

/*
 *
 * The vpiInterModPath vpiHandle will define
 * a vpiInterModPath of record .intermodpath as defined
 * in the IEEE 1364
 *
 */

ਬਣਤਰ __vpiInterModPath : ਜਨਤਕ __vpiHandle {
      __vpiInterModPath();
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
      vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags) override;
      vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_iterate(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_delays(p_vpi_delay del) override;
      ਖਾਲੀ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(ਖਾਲੀ) override;

      __vpiScope   *scope ;

      class vvp_fun_intermodpath*intermodpath;

      vvp_net_t *net;

      vpi_ਪੋਰਟ_Info* port1;
      vpi_ਪੋਰਟ_Info* port2;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiInterModPath
 */

ਬਾਹਰੀ ਬਣਤਰ __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpi_ਪੋਰਟ_Info* port1, vpi_ਪੋਰਟ_Info* port2);

/*
 * These methods support the vpi creation of events. The name string
 * passed in will be saved, so the caller must allocate it (or not
 * free it) after it is handed to this function.
 */
class __vpiNamedEvent : ਜਨਤਕ __vpiHandle {

    ਜਨਤਕ:
      __vpiNamedEvent(__vpiScope*scope, ਅਚਲ ਅੱਖਰ*name);
      ~__vpiNamedEvent() override;

      __vpiNamedEvent(ਅਚਲ __vpiNamedEvent&) = delete;
      __vpiNamedEvent& ਸੰਚਾਲਕ=(ਅਚਲ __vpiNamedEvent&) = delete;

      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      __vpiScope*get_scope(ਖਾਲੀ) ਅਚਲ { ਵਾਪਸ scope_; }
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਅੱਖਰ* vpi_get_str(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags) override;
      vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ) override;

      ਸਤਰਅੰਦਰ ਖਾਲੀ ਜੋੜੋ_vpi_callback(__vpiCallback*cb)
      { cb->ਅਗਲਾ_2 = callbacks_;
	callbacks_ = cb;
      }

      ਖਾਲੀ ਲੜੀਵਾਰ_vpi_callbacks(ਖਾਲੀ);

	/* The functor, used for %set operations. */
      vvp_net_t*funct;

    ਨਿਜੀ:
	/* base name of the event object */
      ਅਚਲ ਅੱਖਰ*name_;
	/* Parent scope of this object. */
      __vpiScope*scope_;
	/* List of callbacks interested in this event. */
      __vpiCallback*callbacks_;
};

ਬਾਹਰੀ vpiHandle vpip_make_named_event(ਅਚਲ ਅੱਖਰ*name, vvp_net_t*f);

/*
 * Memory is an array of bits that is accessible in N-bit chunks, with
 * N being the width of a word. The memory word handle just points
 * back to the memory and uses an index to identify its position in
 * the memory.
 */

ਬਾਹਰੀ bool is_net_array(vpiHandle obj);

/*
 * These are the various variable types.
 */
ਬਣਤਰ __vpiRealVar : ਜਨਤਕ __vpiHandle {
      __vpiRealVar();
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਅੱਖਰ* vpi_get_str(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
      vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags) override;
      vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_iterate(ਪੂਰਨ ਕੋਡ) override;

      ਸੰਘ { // The scope or parent array that contains me.
	    vpiHandle ਮਾਪਾ;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      ਸੰਘ {
            ਅਚਲ ਅੱਖਰ*name;
            vpiHandle ਸੂਚਕਾਂਕ;
      } id;
      ਬਿਨਾਨਿਸ਼ਾਨੀ is_netarray  : 1; // This is word of a net array
      ਬਿਨਾਨਿਸ਼ਾਨੀ is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

ਬਾਹਰੀ __vpiScope* vpip_scope(__vpiRealVar*sig);
ਬਾਹਰੀ vpiHandle vpip_make_real_var(ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net);
ਬਾਹਰੀ vpiHandle vpip_make_real_net(__vpiScope*scope,
				    ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net);

class __vpiBaseVar : ਜਨਤਕ __vpiHandle {

    ਜਨਤਕ:
      __vpiBaseVar(__vpiScope*scope, ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#ਜੇ_ਅੰਤ

      ਸਤਰਅੰਦਰ vvp_net_t* get_net() ਅਚਲ { ਵਾਪਸ net_; }

    ਸੁਰੱਖਿਅਤ:
      __vpiScope* scope_;
      ਅਚਲ ਅੱਖਰ*name_;

    ਨਿਜੀ:
      vvp_net_t*net_;
};

class __vpiStringVar : ਜਨਤਕ __vpiBaseVar {

    ਜਨਤਕ:
      __vpiStringVar(__vpiScope*scope, ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net);

      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
      vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags) override;
};

ਬਾਹਰੀ vpiHandle vpip_make_string_var(ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net);

ਬਣਤਰ __vpiArrayBase {
      __vpiArrayBase() : vals_ਸ਼ਬਦ(ਨਿਰਾ) {}
      ਆਭਾਸੀ ~__vpiArrayBase() {}

      ਆਭਾਸੀ ਬਿਨਾਨਿਸ਼ਾਨੀ get_ਮਾਪ(ਖਾਲੀ) ਅਚਲ = 0;
      ਆਭਾਸੀ vpiHandle get_ਖੱਬਾ_range() = 0;
      ਆਭਾਸੀ vpiHandle get_ਸੱਜਾ_range() = 0;
      ਆਭਾਸੀ __vpiScope*get_scope() ਅਚਲ = 0;

      ਆਭਾਸੀ ਪੂਰਨ get_ਸ਼ਬਦ_ਮਾਪ() ਅਚਲ = 0;
      ਆਭਾਸੀ ਅੱਖਰ*get_ਸ਼ਬਦ_str(ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2, ਪੂਰਨ ਕੋਡ) = 0;
      ਆਭਾਸੀ ਖਾਲੀ get_ਸ਼ਬਦ_ਮੁੱਲ(ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2, p_vpi_ਮੁੱਲ vp) = 0;
      ਆਭਾਸੀ ਖਾਲੀ put_ਸ਼ਬਦ_ਮੁੱਲ(ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2, p_vpi_ਮੁੱਲ vp,
                                    ਪੂਰਨ flags) = 0;
      ਆਭਾਸੀ vpiHandle get_iter_ਸੂਚਕਾਂਕ(ਬਣਤਰ __vpiArrayIterator*iter, ਪੂਰਨ idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_ਅਧਾਰ_iterate(ਪੂਰਨ ਕੋਡ);

      ਆਭਾਸੀ ਖਾਲੀ make_vals_ਸ਼ਬਦ();

      ਬਣਤਰ __vpiArrayWord*vals_ਸ਼ਬਦ;
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
ਬਣਤਰ __vpiArray : ਜਨਤਕ __vpiArrayBase, ਜਨਤਕ __vpiHandle {
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpi_ਸਮ੍ਰਿਤੀ; }
      ਬਿਨਾਨਿਸ਼ਾਨੀ get_ਮਾਪ() ਅਚਲ override { ਵਾਪਸ array_ਗਿਣਤੀ; }
      vpiHandle get_ਖੱਬਾ_range() override { ਪ੍ਰਤਿਜ੍ਞਾ(nets == 0); ਵਾਪਸ &msb; }
      vpiHandle get_ਸੱਜਾ_range() override { ਪ੍ਰਤਿਜ੍ਞਾ(nets == 0); ਵਾਪਸ &lsb; }
      __vpiScope*get_scope() ਅਚਲ override { ਵਾਪਸ scope; }

      ਪੂਰਨ get_ਸ਼ਬਦ_ਮਾਪ() ਅਚਲ override;
      ਅੱਖਰ*get_ਸ਼ਬਦ_str(ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2, ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ get_ਸ਼ਬਦ_ਮੁੱਲ(ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2, p_vpi_ਮੁੱਲ vp) override;
      ਖਾਲੀ put_ਸ਼ਬਦ_ਮੁੱਲ(ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2, p_vpi_ਮੁੱਲ vp, ਪੂਰਨ flags) override;

      vpiHandle get_iter_ਸੂਚਕਾਂਕ(ਬਣਤਰ __vpiArrayIterator*iter, ਪੂਰਨ idx) override;

      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਅੱਖਰ* vpi_get_str(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ) override;
      ਸਤਰਅੰਦਰ vpiHandle vpi_iterate(ਪੂਰਨ ਕੋਡ) override { ਵਾਪਸ vpi_array_ਅਧਾਰ_iterate(ਕੋਡ); }
      vpiHandle vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ idx) override;

      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਬਿਨਾਨਿਸ਼ਾਨੀ off, ਅਚਲ vvp_vector4_t&val);
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਦੁੱਗਣਾ val);
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅਚਲ std::string&val);
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅਚਲ vvp_object_t&val);

      vvp_vector4_t get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਤਾ);
      ਦੁੱਗਣਾ get_ਸ਼ਬਦ_r(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਤਾ);
      ਖਾਲੀ get_ਸ਼ਬਦ_obj(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਤਾ, vvp_object_t&val);
      std::string get_ਸ਼ਬਦ_str(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਤਾ);

      ਖਾਲੀ alias_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr, vpiHandle ਸ਼ਬਦ_2, ਪੂਰਨ msb, ਪੂਰਨ lsb);
      ਖਾਲੀ attach_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ addr, vpiHandle ਸ਼ਬਦ_2);
      ਖਾਲੀ ਸ਼ਬਦ_change(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr);

      ਅਚਲ ਅੱਖਰ*name; /* Permanently allocated string */
      __vpiDecConst ਪਹਿਲਾ_addr;
      __vpiDecConst ਆਖਰੀ_addr;
      __vpiDecConst msb;
      __vpiDecConst lsb;
      ਬਿਨਾਨਿਸ਼ਾਨੀ vals_ਚੌੜਾਈ;
	// If this is a net array, nets lists the handles.
      vpiHandle*nets;
	// If this is a var array, then these are used instead of nets.
      vvp_vector4array_t*vals4;
      vvp_darray        *vals;

      vvp_fun_arrayport*ports_;
      ਬਣਤਰ __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool ਅਦਲਾ_ਬਦਲੀ_addr;

ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ array_ਗਿਣਤੀ;
      __vpiScope*scope;

ਮਿੱਤਰ vpiHandle vpip_make_array(ਅਚਲ ਅੱਖਰ*label, ਅਚਲ ਅੱਖਰ*name,
                                 ਪੂਰਨ ਪਹਿਲਾ_addr, ਪੂਰਨ ਆਖਰੀ_addr,
                                 bool signed_flag);
ਮਿੱਤਰ ਖਾਲੀ compile_array_alias(ਅੱਖਰ*label, ਅੱਖਰ*name, ਅੱਖਰ*src);
};

class __vpiDarrayVar : ਜਨਤਕ __vpiBaseVar, ਜਨਤਕ __vpiArrayBase {
    ਜਨਤਕ:
      __vpiDarrayVar(__vpiScope*scope, ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net);

      ਪੂਰਨ get_type_ਕੋਡ() ਅਚਲ override { ਵਾਪਸ vpiArrayVar; }
      ਬਿਨਾਨਿਸ਼ਾਨੀ get_ਮਾਪ() ਅਚਲ override;
      vpiHandle get_ਖੱਬਾ_range() override;
      vpiHandle get_ਸੱਜਾ_range() override;
      __vpiScope*get_scope() ਅਚਲ override { ਵਾਪਸ scope_; }

      ਪੂਰਨ get_ਸ਼ਬਦ_ਮਾਪ() ਅਚਲ override;
      ਅੱਖਰ*get_ਸ਼ਬਦ_str(ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2, ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ get_ਸ਼ਬਦ_ਮੁੱਲ(ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2, p_vpi_ਮੁੱਲ vp) override;
      ਖਾਲੀ put_ਸ਼ਬਦ_ਮੁੱਲ(ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2, p_vpi_ਮੁੱਲ vp, ਪੂਰਨ flags) override;

      vpiHandle get_iter_ਸੂਚਕਾਂਕ(ਬਣਤਰ __vpiArrayIterator*iter, ਪੂਰਨ idx) override;
      ਸਤਰਅੰਦਰ vpiHandle vpi_iterate(ਪੂਰਨ ਕੋਡ) override { ਵਾਪਸ vpi_array_ਅਧਾਰ_iterate(ਕੋਡ); }

      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਅੱਖਰ* vpi_get_str(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ ਸੂਚਕਾਂਕ) override;

      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;

    ਸੁਰੱਖਿਅਤ:
      vvp_darray*get_vvp_darray() ਅਚਲ;
      __vpiDecConst ਖੱਬਾ_range, ਸੱਜਾ_range;
};

ਬਾਹਰੀ vpiHandle vpip_make_darray_var(ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net);

class __vpiQueueVar : ਜਨਤਕ __vpiDarrayVar {

    ਜਨਤਕ:
      __vpiQueueVar(__vpiScope*scope, ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net);

      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
};

ਬਾਹਰੀ vpiHandle vpip_make_ਕਤਾਰ_var(ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net);

class __vpiPropQueueRef : ਜਨਤਕ __vpiHandle {

    ਜਨਤਕ:
      explicit __vpiPropQueueRef(__vpiScope*scope, ਬਿਨਾਨਿਸ਼ਾਨੀ pidx, bool is_ਕਤਾਰ);

      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਅੱਖਰ* vpi_get_str(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;

      vvp_net_t* class_net_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ prop_idx_;
      bool is_ਕਤਾਰ_2;

    ਨਿਜੀ:
      __vpiScope* scope_;
};

ਬਾਹਰੀ vpiHandle vpip_make_prop_ਕਤਾਰ_ref(ਅੱਖਰ* class_label,
                                          ਬਿਨਾਨਿਸ਼ਾਨੀ prop_idx,
                                          ਬਿਨਾਨਿਸ਼ਾਨੀ is_ਕਤਾਰ_flag);
ਬਾਹਰੀ vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : ਜਨਤਕ __vpiBaseVar {

    ਜਨਤਕ:
      __vpiCobjectVar(__vpiScope*scope, ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net);

      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
};

ਬਾਹਰੀ vpiHandle vpip_make_cobject_var(ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net);

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
ਬਣਤਰ __vpiUserSystf : ਜਨਤਕ __vpiHandle {
      __vpiUserSystf();
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;

      s_vpi_systf_ਅੰਕੜੇ info;
      bool is_user_defn;
};

ਬਾਹਰੀ vpiHandle vpip_make_systf_iterator(ਖਾਲੀ);

ਬਾਹਰੀ ਬਣਤਰ __vpiUserSystf* vpip_ਲੱਭੋ_systf(ਅਚਲ ਅੱਖਰ*name);


ਬਣਤਰ __vpiSysTaskCall : ਜਨਤਕ __vpiHandle {

      __vpiScope* scope;
      ਬਣਤਰ __vpiUserSystf*defn;
      ਬਿਨਾਨਿਸ਼ਾਨੀ nargs;
      vpiHandle*ਦਲੀਲਾਂ;
	/* Stack consumed by this call */
      ਬਿਨਾਨਿਸ਼ਾਨੀ vec4_ਢੇਰ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ real_ਢੇਰ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ string_ਢੇਰ;
	/* Support for vpi_get_userdata. */
      ਖਾਲੀ*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      ਬਿਨਾਨਿਸ਼ਾਨੀ file_idx;
      ਬਿਨਾਨਿਸ਼ਾਨੀ lineno;
      bool put_ਮੁੱਲ;
    ਸੁਰੱਖਿਅਤ:
      ਸਤਰਅੰਦਰ __vpiSysTaskCall()
      {
	    vec4_ਢੇਰ = 0;
	    real_ਢੇਰ = 0;
	    string_ਢੇਰ = 0;
      }
};

ਬਾਹਰੀ ਬਣਤਰ __vpiSysTaskCall*vpip_cur_task;

/*
 * The persistent flag to vpip_make_string_const causes the created
 * handle to be persistent. This is necessary for cases where the
 * string handle may be reused, which is the normal case.
 *
 * When constructing with a string, the class takes possession of the
 * text value string, and will delete it in the constructor.
 */

vpiHandle vpip_make_string_const(ਅੱਖਰ*ਪਾਠ, bool persistent =true);
vpiHandle vpip_make_string_param(ਅਚਲ ਅੱਖਰ*name, ਅੱਖਰ*ਮੁੱਲ, bool local_flag,
                                 ਲੰਮਾ file_idx, ਲੰਮਾ lineno);

ਬਣਤਰ __vpiBinaryConst : ਜਨਤਕ __vpiHandle {
      __vpiBinaryConst();
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;

      vvp_vector4_t ਬਿੱਟ;
	/* TRUE if this constant is signed. */
      ਬਿਨਾਨਿਸ਼ਾਨੀ signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      ਬਿਨਾਨਿਸ਼ਾਨੀ sized_flag  :1;
};

vpiHandle vpip_make_ਦੁਆਧਾਰੀ_const(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਅਚਲ ਅੱਖਰ*ਬਿੱਟ);
vpiHandle vpip_make_ਦੁਆਧਾਰੀ_param(ਅਚਲ ਅੱਖਰ*name, ਅਚਲ vvp_vector4_t&ਬਿੱਟ,
				 bool signed_flag, bool local_flag,
				 ਲੰਮਾ file_idx, ਲੰਮਾ lineno);

class __vpiRealConst : ਜਨਤਕ __vpiHandle {
    ਜਨਤਕ:
      explicit __vpiRealConst(ਦੁੱਗਣਾ);
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;

      ਦੁੱਗਣਾ ਮੁੱਲ;
};

vpiHandle vpip_make_real_const(ਦੁੱਗਣਾ ਮੁੱਲ);
vpiHandle vpip_make_real_param(ਅਚਲ ਅੱਖਰ*name, ਦੁੱਗਣਾ ਮੁੱਲ, bool local_flag,
                               ਲੰਮਾ file_idx, ਲੰਮਾ lineno);

class __vpiNullConst : ਜਨਤਕ __vpiHandle {
    ਜਨਤਕ:
      explicit __vpiNullConst();
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਅਚਲ ਅੱਖਰ*type);
vpiHandle vpip_make_vthr_str_ਢੇਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ);
vpiHandle vpip_make_vthr_vec4_ਢੇਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਡੂੰਘਾਈ, bool signed_flag, ਬਿਨਾਨਿਸ਼ਾਨੀ wid);

vpiHandle vpip_make_vthr_A(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ);
vpiHandle vpip_make_vthr_A(ਅੱਖਰ*label, ਅੱਖਰ*ਚਿੰਨ੍ਹ);
vpiHandle vpip_make_vthr_A(ਅੱਖਰ*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
ਬਾਹਰੀ "C" ਖਾਲੀ vpip_ਲੱਦੋ_module(ਅਚਲ ਅੱਖਰ*name);

ਬਾਹਰੀ "C" ਖਾਲੀ vpip_clear_module_paths();
ਬਾਹਰੀ "C" ਖਾਲੀ vpip_ਜੋੜੋ_module_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);
ਬਾਹਰੀ "C" ਖਾਲੀ vpip_ਜੋੜੋ_env_and_default_module_paths();

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
ਬਾਹਰੀ vpiHandle vpip_build_vpi_call(ਅਚਲ ਅੱਖਰ*name,
				     ਪੂਰਨ val_type, ਬਿਨਾਨਿਸ਼ਾਨੀ return_ਚੌੜਾਈ,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     ਬਿਨਾਨਿਸ਼ਾਨੀ argc,
				     vpiHandle*argv,
				     ਬਿਨਾਨਿਸ਼ਾਨੀ vec4_ਢੇਰ,
				     ਬਿਨਾਨਿਸ਼ਾਨੀ real_ਢੇਰ,
				     ਬਿਨਾਨਿਸ਼ਾਨੀ string_ਢੇਰ,
				     ਲੰਮਾ file_idx,
				     ਲੰਮਾ lineno);

ਬਾਹਰੀ vthread_t vpip_current_vthread;

ਬਾਹਰੀ ਖਾਲੀ vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_ਸਮਾਂ(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

ਬਾਹਰੀ ਪੂਰਨ vpip_get_ਸਮਾਂ_precision(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ vpip_ਸੈੱਟ_ਸਮਾਂ_precision(ਪੂਰਨ pres);

ਬਾਹਰੀ __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

ਬਾਹਰੀ ਪੂਰਨ vpip_ਸਮਾਂ_units_from_handle(vpiHandle obj);
ਬਾਹਰੀ ਪੂਰਨ vpip_ਸਮਾਂ_precision_from_handle(vpiHandle obj);

ਬਾਹਰੀ ਦੁੱਗਣਾ vpip_scaled_ਸਮਾਂ_from_handle(vvp_time64_t ਸਮਾਂ, vpiHandle obj);

ਬਾਹਰੀ ਖਾਲੀ vpip_ਸਮਾਂ_ਮੰਜ਼ਿਲ_timestruct(ਬਣਤਰ t_vpi_ਸਮਾਂ*ts, vvp_time64_t ti);
ਬਾਹਰੀ vvp_time64_t vpip_timestruct_ਮੰਜ਼ਿਲ_ਸਮਾਂ(ਅਚਲ ਬਣਤਰ t_vpi_ਸਮਾਂ*ts);

ਬਾਹਰੀ ਦੁੱਗਣਾ vpip_ਸਮਾਂ_ਮੰਜ਼ਿਲ_scaled_real(vvp_time64_t ti, ਅਚਲ __vpiScope*sc);
ਬਾਹਰੀ vvp_time64_t vpip_scaled_real_ਮੰਜ਼ਿਲ_time64(ਦੁੱਗਣਾ val, ਅਚਲ __vpiScope*sc);

/*
 * These functions are used mostly as compile time to strings into
 * permallocated memory. The vpip_string function is the most general,
 * it allocates a fresh string no matter what. The vpip_name_string
 * allocates a string and keeps a pointer in the hash, and tries to
 * reuse it if it can. This us useful for handle names, which may be
 * reused in different scopes.
 */
ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ* vpip_string(ਅਚਲ ਅੱਖਰ*str);
ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ* vpip_name_string(ਅਚਲ ਅੱਖਰ*str);


/*
 * This function is used to make decimal string versions of various
 * vectors. The input format is a vvp_vector4_t, and the result is
 * written into buf, without overflowing nbuf.
 */
ਬਾਹਰੀ ਬਿਨਾਨਿਸ਼ਾਨੀ vpip_vec4_ਮੰਜ਼ਿਲ_dec_str(ਅਚਲ vvp_vector4_t&vec4,
				     ਅੱਖਰ *buf, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ nbuf,
				     ਪੂਰਨ signed_flag);


ਬਾਹਰੀ ਖਾਲੀ vpip_vec4_ਮੰਜ਼ਿਲ_hex_str(ਅਚਲ vvp_vector4_t&ਬਿੱਟ, ਅੱਖਰ*buf,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ nbuf);

ਬਾਹਰੀ ਖਾਲੀ vpip_vec4_ਮੰਜ਼ਿਲ_oct_str(ਅਚਲ vvp_vector4_t&ਬਿੱਟ, ਅੱਖਰ*buf,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ nbuf);

ਬਾਹਰੀ ਖਾਲੀ vpip_bin_str_ਮੰਜ਼ਿਲ_vec4(vvp_vector4_t&val, ਅਚਲ ਅੱਖਰ*buf);
ਬਾਹਰੀ ਖਾਲੀ vpip_oct_str_ਮੰਜ਼ਿਲ_vec4(vvp_vector4_t&val, ਅਚਲ ਅੱਖਰ*str);
ਬਾਹਰੀ ਖਾਲੀ vpip_dec_str_ਮੰਜ਼ਿਲ_vec4(vvp_vector4_t&val, ਅਚਲ ਅੱਖਰ*str);
ਬਾਹਰੀ ਖਾਲੀ vpip_hex_str_ਮੰਜ਼ਿਲ_vec4(vvp_vector4_t&val, ਅਚਲ ਅੱਖਰ*str);

ਬਾਹਰੀ vvp_vector4_t vec4_from_vpi_ਮੁੱਲ(s_vpi_ਮੁੱਲ*vp, ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
ਬਾਹਰੀ ਦੁੱਗਣਾ real_from_vpi_ਮੁੱਲ(s_vpi_ਮੁੱਲ*vp);

ਬਾਹਰੀ ਖਾਲੀ vpip_vec4_get_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&ਸ਼ਬਦ_val, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ,
				bool signed_flag, s_vpi_ਮੁੱਲ*vp);
ਬਾਹਰੀ ਖਾਲੀ vpip_vec2_get_ਮੁੱਲ(ਅਚਲ vvp_vector2_t&ਸ਼ਬਦ_val, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ,
				bool signed_flag, s_vpi_ਮੁੱਲ*vp);
ਬਾਹਰੀ ਖਾਲੀ vpip_real_get_ਮੁੱਲ(ਦੁੱਗਣਾ real, s_vpi_ਮੁੱਲ*vp);
ਬਾਹਰੀ ਖਾਲੀ vpip_string_get_ਮੁੱਲ(ਅਚਲ std::string&val, s_vpi_ਮੁੱਲ*vp);

/*
 * Function defined in vpi_signal.cc to manage vpi_get_* persistent
 * storage.
 */
ਗਿਣਤੀ vpi_rbuf_t {
      RBUF_VAL =0,
	/* Storage for *_get_value() */
      RBUF_STR,
	/* Storage for *_get_str() */
      RBUF_DEL
	/* Delete the storage for both buffers. */
};
ਬਾਹਰੀ ਖਾਲੀ *need_ਨਤੀਜਾ_buf(ਬਿਨਾਨਿਸ਼ਾਨੀ cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
ਬਾਹਰੀ ਅੱਖਰ *simple_ਸੈੱਟ_rbuf_str(ਅਚਲ ਅੱਖਰ *s1);
ਬਾਹਰੀ ਅੱਖਰ *generic_get_str(ਪੂਰਨ ਕੋਡ, vpiHandle ref, ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *ਸੂਚਕਾਂਕ);

/* A routine to find the enclosing module. */
ਬਾਹਰੀ vpiHandle vpip_module(__vpiScope*scope);

ਬਾਹਰੀ ਪੂਰਨ vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
ਬਾਹਰੀ bool vpip_mcd0_disable;

ਸਤਰਅੰਦਰ uint64_t vlg_ਗੇੜਾ_ਮੰਜ਼ਿਲ_u64(ਦੁੱਗਣਾ rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      ਵਾਪਸ static_cast<uint64_t>(static_cast<int64_t>(std::ਗੇੜਾ_2(rval)));
}

#ਜੇ_ਅੰਤ /* IVL_vpi_priv_H */
