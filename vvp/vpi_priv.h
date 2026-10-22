#ifndef IVL_vpi_priv_H
#define IVL_vpi_priv_H
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
#ಅನ್ತರ್ಭಾವ  "delay.h"


class class_type;
class vvp_darray;
class vvp_fun_arrayport;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ __vpiArray* vvp_array_t;

/*
 * This header file contains the internal definitions that the vvp
 * program uses to implement the public interface in the vpi_user.h
 * header file elsewhere.
 */

#ಯದಿ defined(__MINGW32__) || defined (__CYGWIN__)
ಬಾಹ್ಯ vpip_routines_s vpi_routines;
#endif

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#define vpi_File_ಸಾಲು    0x1000003
#define _vpiDescription 0x1000004

ಬಾಹ್ಯ bool show_file_ಸಾಲು;
ಬಾಹ್ಯ bool code_is_instrumented;

ಬಾಹ್ಯ vpiHandle vpip_build_file_ಸಾಲು(ಧ್ರುವ ವರ್ಣ*description,
                                      long file_idx, long lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
ಬಾಹ್ಯ ಧ್ರುವ ವರ್ಣ* vpi_type_as_string(PLI_INT32 code);

/*
 * Return the port direction as a string.
 */
ಬಾಹ್ಯ ಧ್ರುವ ವರ್ಣ* direction_as_string(ಪೂರ್ಣಾಙ್ಕ dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#ಯದಿ defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
#define _vpiFromThr 0x1000001
#   define _vpiNoThr   0
#   define _vpiString  1
#   define _vpiVThr    2
#   define _vpiWord    3
#   define _vpi_at_PV  4
#   define _vpi_at_A   5
#   define _vpi_at_APV 6
#endif


/*
 * The vpi_mode_flag contains the major mode for VPI use. This is used
 * to generate error messages when vpi functions are called
 * incorrectly.
 */
ಗಣನಾ vpi_mode_t {
      VPI_MODE_NONE =0,
	/* The compiler is calling a register function. */
      VPI_MODE_REGISTER,
	/* The compiler is calling a compiletf function. */
      VPI_MODE_COMPILETF,
	/* The compiler is calling a calltf function. */
      VPI_MODE_CALLTF,
	/* We are in the midst of a RWSync callback. */
      VPI_MODE_RWSYNC,
	/* We are in a ROSync callback. */
      VPI_MODE_ROSYNC
};
ಬಾಹ್ಯ vpi_mode_t vpi_mode_flag;

/*
 * This structure is the very base of a vpiHandle. Every handle
 * structure is derived from this class so that the library can
 * internally pass the derived types as pointers to one of these.
 */
class __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ __vpiHandle() { }
	// The destructor is virtual so that dynamic types will work.
      ಆಭಾಸ ~__vpiHandle();

      ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ =0;
      ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code);
      ಆಭಾಸ int64_t vpi_get64(ಪೂರ್ಣಾಙ್ಕ code);
      ಆಭಾಸ ವರ್ಣ* vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code);

      ಆಭಾಸ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val);
      ಆಭಾಸ vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags);
      ಆಭಾಸ vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code);
      ಆಭಾಸ vpiHandle vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code);
      ಆಭಾಸ vpiHandle vpi_index(ಪೂರ್ಣಾಙ್ಕ idx);
      ಆಭಾಸ ಶೂನ್ಯ vpi_get_delays(p_vpi_delay del);
      ಆಭಾಸ ಶೂನ್ಯ vpi_put_delays(p_vpi_delay del);

	// Objects may have destroyer functions of their own. If so,
	// then this virtual method will return a POINTER to that
	// function. The pointer is used to "delete" the object, which
	// is why the function itself cannot be a method.
      ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*free_object_fun_t)(vpiHandle);
      ಆಭಾಸ free_object_fun_t free_object_fun(ಶೂನ್ಯ);
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
ಸಂರಚನಾ __vpiIterator : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiIterator();
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      free_object_fun_t free_object_fun(ಶೂನ್ಯ) override;

      vpiHandle *args;
      unsigned  nargs;
      unsigned  next;
      bool free_args_flag;
};

ಬಾಹ್ಯ vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
				    bool free_args_flag);

class __vpiDecConst : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      explicit __vpiDecConst(ಪೂರ್ಣಾಙ್ಕ val =0);
      __vpiDecConst(ಧ್ರುವ __vpiDecConst&that);
# ಯದಿ __cplusplus >= 201103L
      __vpiDecConst& operator = (ಧ್ರುವ __vpiDecConst&that) = ಸ್ವಭಾವ;
# endif
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ get_ಮೌಲ್ಯ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ; }
      ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ set_ಮೌಲ್ಯ(ಪೂರ್ಣಾಙ್ಕ val) { ಮೌಲ್ಯ = val; }

    ಗುಪ್ತ:
      ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
ಸಂರಚನಾ __vpiCallback : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;

	// Used for listing callbacks.
      ಸಂರಚನಾ __vpiCallback*next;

	// user supplied callback data
      ಸಂರಚನಾ t_cb_data cb_data;
};

class ಮೌಲ್ಯ_callback : ಸಾರ್ವಜನಿಕ __vpiCallback {
    ಸಾರ್ವಜನಿಕ:
      explicit ಮೌಲ್ಯ_callback(p_cb_data data);
	// Return true if the callback really is ready to be called
      ಆಭಾಸ bool test_ಮೌಲ್ಯ_callback_ready(ಶೂನ್ಯ);

    ಸಾರ್ವಜನಿಕ:
	// user supplied callback data
      ಸಂರಚನಾ t_vpi_time cb_time;
      ಸಂರಚನಾ t_vpi_ಮೌಲ್ಯ cb_ಮೌಲ್ಯ;
};

ಬಾಹ್ಯ ಶೂನ್ಯ callback_execute(ಸಂರಚನಾ __vpiCallback*cur);

ಸಂರಚನಾ __vpiSystemTime : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiSystemTime();
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ*vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;

      __vpiScope*scope;
};

ಸಂರಚನಾ __vpiScopedTime : ಸಾರ್ವಜನಿಕ __vpiSystemTime {
      __vpiScopedTime();
      ವರ್ಣ*vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
};
ಸಂರಚನಾ __vpiScopedSTime : ಸಾರ್ವಜನಿಕ __vpiSystemTime {
      __vpiScopedSTime();
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ*vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
};
ಸಂರಚನಾ __vpiScopedRealtime : ಸಾರ್ವಜನಿಕ __vpiSystemTime {
      __vpiScopedRealtime();
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ*vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : ಸಾರ್ವಜನಿಕ __vpiHandle {

    ಸಾರ್ವಜನಿಕ:
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ* vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code) override;

    ಸಾರ್ವಜನಿಕ:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ*scope_name() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ name_; }

      ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ*scope_def_name() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ tname_; }
	// TRUE if this is an automatic func/task/block
      ಅನ್ತಃಸ್ಥ bool is_automatic() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ is_automatic_; }

    ಸಾರ್ವಜನಿಕ:
      __vpiScope *scope;
      unsigned file_idx;
      unsigned lineno;
      unsigned def_file_idx;
      unsigned def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_time;
      ಸಂರಚನಾ __vpiScopedSTime scoped_stime;
      ಸಂರಚನಾ __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::vector<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      ಸಂರಚನಾ automatic_hooks_s**item;
      unsigned nitem;
        /* Keep a list of live contexts. */
      vvp_context_t live_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::set<vthread_t> threads;
      signed ಪೂರ್ಣಾಙ್ಕ time_units :8;
      signed ಪೂರ್ಣಾಙ್ಕ time_precision :8;

    ರಕ್ಷಿತ:
      __vpiScope(ಧ್ರುವ ವರ್ಣ*nam, ಧ್ರುವ ವರ್ಣ*tnam, bool is_auto_flag =false);

    ಗುಪ್ತ:
	/* The scope has a name. */
      ಧ್ರುವ ವರ್ಣ*name_;
      ಧ್ರುವ ವರ್ಣ*tname_;
	/* the scope may be "automatic" */
      bool is_automatic_;
};

class vpiScopeFunction  : ಸಾರ್ವಜನಿಕ __vpiScope {
    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ vpiScopeFunction(ಧ್ರುವ ವರ್ಣ*nam, ಧ್ರುವ ವರ್ಣ*tnam,
			      bool auto_flag, ಪೂರ್ಣಾಙ್ಕ func_type, unsigned func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiFunction; }
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override
      {
	    ವಿಕಲ್ಪ (code) {
		ಪಕ್ಷ vpiFuncType:
		  ಪ್ರತ್ಯಾಗಮ func_type_;
		ಸ್ವಭಾವ:
		  ಪ್ರತ್ಯಾಗಮ __vpiScope::vpi_get(code);
	    }
      }

    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ unsigned get_func_width(ಶೂನ್ಯ) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ func_wid_; }
      ಅನ್ತಃಸ್ಥ vvp_bit4_t get_func_init_val(ಶೂನ್ಯ) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ func_init_val_; }

    ಗುಪ್ತ:
      ಪೂರ್ಣಾಙ್ಕ func_type_;
      unsigned func_wid_;
      vvp_bit4_t func_init_val_;
};

ಬಾಹ್ಯ __vpiScope* vpip_peek_current_scope(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_attach_to_current_scope(vpiHandle obj);
ಬಾಹ್ಯ __vpiScope* vpip_peek_context_scope(ಶೂನ್ಯ);
ಬಾಹ್ಯ unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                         __vpiScope*scope);
ಬಾಹ್ಯ vpiHandle vpip_make_ಬೇರು_iterator(ಪೂರ್ಣಾಙ್ಕ type_code);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_make_ಬೇರು_iterator(class __vpiHandle**&table,
				    unsigned&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
ಸಂರಚನಾ __vpiSignal : ಸಾರ್ವಜನಿಕ __vpiHandle {
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      int64_t vpi_get64(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ* vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_index(ಪೂರ್ಣಾಙ್ಕ idx) override;

    ಸಾರ್ವಜನಿಕ:
      unsigned width() ಧ್ರುವ;
      vpiHandle get_index(ಪೂರ್ಣಾಙ್ಕ index);
      ಶೂನ್ಯ get_bit_ಮೌಲ್ಯ(ಧ್ರುವ ಸಂರಚನಾ __vpiBit*bit, p_vpi_ಮೌಲ್ಯ vp);
      vpiHandle put_bit_ಮೌಲ್ಯ(ಸಂರಚನಾ __vpiBit*bit, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ flags);
      ಶೂನ್ಯ make_bits();

      ಸಂರಚನಾ __vpiBit*bits;

    ಸಾರ್ವಜನಿಕ:
      ಸಙ್ಘ { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
      ಸಙ್ಘ { // The name of this reg/net, or the index for array words.
            ಧ್ರುವ ವರ್ಣ*name;
            vpiHandle index;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      unsigned signed_flag  : 1;
      unsigned is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*ಗಂಟು;

    ಸಾರ್ವಜನಿಕ:
      ಸ್ಥಿರ ಶೂನ್ಯ*operator new(std::size_t size);
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete(ಶೂನ್ಯ*); // not implemented
    ರಕ್ಷಿತ:
      ಅನ್ತಃಸ್ಥ __vpiSignal() : bits(ರಿಕ್ತ) { }
    ಗುಪ್ತ: // Not implemented
      ಸ್ಥಿರ ಶೂನ್ಯ*operator new[] (std::size_t size);
      ಸ್ಥಿರ ಶೂನ್ಯ operator delete[](ಶೂನ್ಯ*);
};
ಬಾಹ್ಯ unsigned vpip_size(ಧ್ರುವ __vpiSignal *sig);
ಬಾಹ್ಯ __vpiScope* vpip_scope(__vpiSignal*sig);

ಬಾಹ್ಯ vpiHandle vpip_make_int2(ಧ್ರುವ ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb,
			       bool signed_flag, vvp_net_t*vec);
ಬಾಹ್ಯ vpiHandle vpip_make_int4(ಧ್ರುವ ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb,
			       vvp_net_t*vec);
ಬಾಹ್ಯ vpiHandle vpip_make_var4(ಧ್ರುವ ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb,
			       bool signed_flag, vvp_net_t*net);
ಬಾಹ್ಯ vpiHandle vpip_make_net4(__vpiScope*scope,
				ಧ್ರುವ ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb,
				bool signed_flag, vvp_net_t*ಗಂಟು);

/*
 * This is used to represent a bit in a net/reg.
 */
ಸಂರಚನಾ __vpiBit {
      ಸಂರಚನಾ as_bit_t : ಸಾರ್ವಜನಿಕ __vpiHandle {
	    ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ;
	    ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code);
	    ವರ್ಣ* vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code);
	    ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val);
	    vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags);
	    vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code);
      } as_bit;

      vpiHandle index;

      ಸಙ್ಘ {
	    ಸಂರಚನಾ __vpiSignal*parent;
	    ಸಂರಚನಾ __vpiBit*bit0;
      };

      ಅನ್ತಃಸ್ಥ unsigned get_norm_index() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ this - bit0; }
      ಅನ್ತಃಸ್ಥ ಸಂರಚನಾ __vpiSignal*get_parent() ಧ್ರುವ {ಪ್ರತ್ಯಾಗಮ (bit0 - 1)->parent; }
      ಪೂರ್ಣಾಙ್ಕ get_index(ಶೂನ್ಯ) ಧ್ರುವ;
};

class vpiPortBitInfo;

class vpiPortInfo  : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      vpiPortInfo( __vpiScope *parent,
                    unsigned index,
                    ಪೂರ್ಣಾಙ್ಕ vpi_direction,
                    unsigned width,
                    ಧ್ರುವ ವರ್ಣ *name,
                    ವರ್ಣ* buffer );
      ~vpiPortInfo() override;

      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiPort; }
      ಪೂರ್ಣಾಙ್ಕ get_direction(ಶೂನ್ಯ) { ಪ್ರತ್ಯಾಗಮ direction_; }
      unsigned get_index(ಶೂನ್ಯ) { ಪ್ರತ್ಯಾಗಮ index_; }
      ಪೂರ್ಣಾಙ್ಕ get_width(ಶೂನ್ಯ) { ಪ್ರತ್ಯಾಗಮ width_; }
      ಶೂನ್ಯ add_port_bit(vpiPortBitInfo* port_bit) { port_bits_.ತಳ್ಳು_back(port_bit); }

      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ* vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;
      vvp_net_t* get_port(ಶೂನ್ಯ) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ref_; }
      vpiHandle vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code) override;

      std::vector<vpiPortBitInfo*> port_bits_;

    ಗುಪ್ತ:
      __vpiScope *parent_;
      unsigned  index_;
      ಪೂರ್ಣಾಙ್ಕ       direction_;
      unsigned  width_;
      ಧ್ರುವ ವರ್ಣ *name_;
      vvp_net_t *ref_;
};

class vpiPortBitInfo  : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      vpiPortBitInfo(vpiPortInfo *parent,
                     unsigned bit);
      ~vpiPortBitInfo() override;

      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiPortBit; }
      unsigned get_bit(ಶೂನ್ಯ) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ bit_; }

      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;

    ಗುಪ್ತ:
      vpiPortInfo *parent_;
      unsigned bit_;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
ಸಂರಚನಾ __vpiPV : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiPV();
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ* vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;

      vpiHandle parent;
      vvp_net_t*net;
      vpiHandle sbase;
      ಪೂರ್ಣಾಙ್ಕ tbase;
      unsigned width;
};
ಬಾಹ್ಯ vpiHandle vpip_make_PV(ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ base, ಪೂರ್ಣಾಙ್ಕ width);
ಬಾಹ್ಯ vpiHandle vpip_make_PV(ವರ್ಣ*name, ವರ್ಣ*ಚಿಹ್ನೆ, ಪೂರ್ಣಾಙ್ಕ width);
ಬಾಹ್ಯ vpiHandle vpip_make_PV(ವರ್ಣ*name, vpiHandle handle, ಪೂರ್ಣಾಙ್ಕ width);

ಸಂರಚನಾ __vpiModPathTerm : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiModPathTerm();
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;

      vpiHandle ಅಭಿವ್ಯಕ್ತಿ;
	/* The value returned by vpi_get(vpiEdge, ...); */
      ಪೂರ್ಣಾಙ್ಕ edge;
};

ಸಂರಚನಾ __vpiModPathSrc : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiModPathSrc();
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_index(ಪೂರ್ಣಾಙ್ಕ idx) override;
      ಶೂನ್ಯ vpi_get_delays(p_vpi_delay del) override;
      ಶೂನ್ಯ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(ಶೂನ್ಯ) override;

      ಸಂರಚನಾ __vpiModPath *dest;
      ಪೂರ್ಣಾಙ್ಕ   type;

	/* This is the input expression for this modpath. */
      ಸಂರಚನಾ __vpiModPathTerm path_term_in;

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

ಸಂರಚನಾ __vpiModPath {
      __vpiScope   *scope ;

      class vvp_fun_modpath*modpath;

      ಸಂರಚನಾ __vpiModPathTerm path_term_out;
      vvp_net_t *ಒಳಹರಿವು_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

ಬಾಹ್ಯ ಸಂರಚನಾ __vpiModPathSrc* vpip_make_modpath_src(ಸಂರಚನಾ __vpiModPath*path,
                                                     vvp_net_t *net) ;

ಬಾಹ್ಯ ಸಂರಚನಾ __vpiModPath* vpip_make_modpath(vvp_net_t *net) ;

/*
 *
 * The vpiInterModPath vpiHandle will define
 * a vpiInterModPath of record .intermodpath as defined
 * in the IEEE 1364
 *
 */

ಸಂರಚನಾ __vpiInterModPath : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiInterModPath();
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_delays(p_vpi_delay del) override;
      ಶೂನ್ಯ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(ಶೂನ್ಯ) override;

      __vpiScope   *scope ;

      class vvp_fun_intermodpath*intermodpath;

      vvp_net_t *net;

      vpiPortInfo* port1;
      vpiPortInfo* port2;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiInterModPath
 */

ಬಾಹ್ಯ ಸಂರಚನಾ __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2);

/*
 * These methods support the vpi creation of events. The name string
 * passed in will be saved, so the caller must allocate it (or not
 * free it) after it is handed to this function.
 */
class __vpiNamedEvent : ಸಾರ್ವಜನಿಕ __vpiHandle {

    ಸಾರ್ವಜನಿಕ:
      __vpiNamedEvent(__vpiScope*scope, ಧ್ರುವ ವರ್ಣ*name);
      ~__vpiNamedEvent() override;

      __vpiNamedEvent(ಧ್ರುವ __vpiNamedEvent&) = delete;
      __vpiNamedEvent& operator=(ಧ್ರುವ __vpiNamedEvent&) = delete;

      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      __vpiScope*get_scope(ಶೂನ್ಯ) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ scope_; }
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ* vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;

      ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ add_vpi_callback(__vpiCallback*cb)
      { cb->next = callbacks_;
	callbacks_ = cb;
      }

      ಶೂನ್ಯ run_vpi_callbacks(ಶೂನ್ಯ);

	/* The functor, used for %set operations. */
      vvp_net_t*funct;

    ಗುಪ್ತ:
	/* base name of the event object */
      ಧ್ರುವ ವರ್ಣ*name_;
	/* Parent scope of this object. */
      __vpiScope*scope_;
	/* List of callbacks interested in this event. */
      __vpiCallback*callbacks_;
};

ಬಾಹ್ಯ vpiHandle vpip_make_named_event(ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*f);

/*
 * Memory is an array of bits that is accessible in N-bit chunks, with
 * N being the width of a word. The memory word handle just points
 * back to the memory and uses an index to identify its position in
 * the memory.
 */

ಬಾಹ್ಯ bool is_net_array(vpiHandle obj);

/*
 * These are the various variable types.
 */
ಸಂರಚನಾ __vpiRealVar : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiRealVar();
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ* vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code) override;

      ಸಙ್ಘ { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      ಸಙ್ಘ {
            ಧ್ರುವ ವರ್ಣ*name;
            vpiHandle index;
      } id;
      unsigned is_netarray  : 1; // This is word of a net array
      unsigned is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

ಬಾಹ್ಯ __vpiScope* vpip_scope(__vpiRealVar*sig);
ಬಾಹ್ಯ vpiHandle vpip_make_real_var(ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net);
ಬಾಹ್ಯ vpiHandle vpip_make_real_net(__vpiScope*scope,
				    ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net);

class __vpiBaseVar : ಸಾರ್ವಜನಿಕ __vpiHandle {

    ಸಾರ್ವಜನಿಕ:
      __vpiBaseVar(__vpiScope*scope, ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net);
#ifdef CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#endif

      ಅನ್ತಃಸ್ಥ vvp_net_t* get_net() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ net_; }

    ರಕ್ಷಿತ:
      __vpiScope* scope_;
      ಧ್ರುವ ವರ್ಣ*name_;

    ಗುಪ್ತ:
      vvp_net_t*net_;
};

class __vpiStringVar : ಸಾರ್ವಜನಿಕ __vpiBaseVar {

    ಸಾರ್ವಜನಿಕ:
      __vpiStringVar(__vpiScope*scope, ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net);

      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags) override;
};

ಬಾಹ್ಯ vpiHandle vpip_make_string_var(ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net);

ಸಂರಚನಾ __vpiArrayBase {
      __vpiArrayBase() : vals_ಪದಗಳು(ರಿಕ್ತ) {}
      ಆಭಾಸ ~__vpiArrayBase() {}

      ಆಭಾಸ unsigned get_size(ಶೂನ್ಯ) ಧ್ರುವ = 0;
      ಆಭಾಸ vpiHandle get_ಎಡ_range() = 0;
      ಆಭಾಸ vpiHandle get_ಬಲ_range() = 0;
      ಆಭಾಸ __vpiScope*get_scope() ಧ್ರುವ = 0;

      ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ get_word_size() ಧ್ರುವ = 0;
      ಆಭಾಸ ವರ್ಣ*get_word_str(ಸಂರಚನಾ __vpiArrayWord*word, ಪೂರ್ಣಾಙ್ಕ code) = 0;
      ಆಭಾಸ ಶೂನ್ಯ get_word_ಮೌಲ್ಯ(ಸಂರಚನಾ __vpiArrayWord*word, p_vpi_ಮೌಲ್ಯ vp) = 0;
      ಆಭಾಸ ಶೂನ್ಯ put_word_ಮೌಲ್ಯ(ಸಂರಚನಾ __vpiArrayWord*word, p_vpi_ಮೌಲ್ಯ vp,
                                    ಪೂರ್ಣಾಙ್ಕ flags) = 0;
      ಆಭಾಸ vpiHandle get_iter_index(ಸಂರಚನಾ __vpiArrayIterator*iter, ಪೂರ್ಣಾಙ್ಕ idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_base_iterate(ಪೂರ್ಣಾಙ್ಕ code);

      ಆಭಾಸ ಶೂನ್ಯ make_vals_ಪದಗಳು();

      ಸಂರಚನಾ __vpiArrayWord*vals_ಪದಗಳು;
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
ಸಂರಚನಾ __vpiArray : ಸಾರ್ವಜನಿಕ __vpiArrayBase, ಸಾರ್ವಜನಿಕ __vpiHandle {
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiMemory; }
      unsigned get_size() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ array_ಎಣಿಕೆ; }
      vpiHandle get_ಎಡ_range() override { ಪ್ರತಿಜ್ಞಾ(nets == 0); ಪ್ರತ್ಯಾಗಮ &msb; }
      vpiHandle get_ಬಲ_range() override { ಪ್ರತಿಜ್ಞಾ(nets == 0); ಪ್ರತ್ಯಾಗಮ &lsb; }
      __vpiScope*get_scope() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ scope; }

      ಪೂರ್ಣಾಙ್ಕ get_word_size() ಧ್ರುವ override;
      ವರ್ಣ*get_word_str(ಸಂರಚನಾ __vpiArrayWord*word, ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ get_word_ಮೌಲ್ಯ(ಸಂರಚನಾ __vpiArrayWord*word, p_vpi_ಮೌಲ್ಯ vp) override;
      ಶೂನ್ಯ put_word_ಮೌಲ್ಯ(ಸಂರಚನಾ __vpiArrayWord*word, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ flags) override;

      vpiHandle get_iter_index(ಸಂರಚನಾ __vpiArrayIterator*iter, ಪೂರ್ಣಾಙ್ಕ idx) override;

      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ* vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಅನ್ತಃಸ್ಥ vpiHandle vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code) override { ಪ್ರತ್ಯಾಗಮ vpi_array_base_iterate(code); }
      vpiHandle vpi_index(ಪೂರ್ಣಾಙ್ಕ idx) override;

      ಶೂನ್ಯ set_word(unsigned idx, unsigned off, ಧ್ರುವ vvp_vector4_t&val);
      ಶೂನ್ಯ set_word(unsigned idx, ದ್ವಿಗುಣ val);
      ಶೂನ್ಯ set_word(unsigned idx, ಧ್ರುವ std::string&val);
      ಶೂನ್ಯ set_word(unsigned idx, ಧ್ರುವ vvp_object_t&val);

      vvp_vector4_t get_word(unsigned address);
      ದ್ವಿಗುಣ get_word_r(unsigned address);
      ಶೂನ್ಯ get_word_obj(unsigned address, vvp_object_t&val);
      std::string get_word_str(unsigned address);

      ಶೂನ್ಯ alias_word(unsigned long addr, vpiHandle word, ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb);
      ಶೂನ್ಯ attach_word(unsigned addr, vpiHandle word);
      ಶೂನ್ಯ word_change(unsigned long addr);

      ಧ್ರುವ ವರ್ಣ*name; /* Permanently allocated string */
      __vpiDecConst first_addr;
      __vpiDecConst last_addr;
      __vpiDecConst msb;
      __vpiDecConst lsb;
      unsigned vals_width;
	// If this is a net array, nets lists the handles.
      vpiHandle*nets;
	// If this is a var array, then these are used instead of nets.
      vvp_vector4array_t*vals4;
      vvp_darray        *vals;

      vvp_fun_arrayport*ports_;
      ಸಂರಚನಾ __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool swap_addr;

ಗುಪ್ತ:
      unsigned array_ಎಣಿಕೆ;
      __vpiScope*scope;

friend vpiHandle vpip_make_array(ಧ್ರುವ ವರ್ಣ*label, ಧ್ರುವ ವರ್ಣ*name,
                                 ಪೂರ್ಣಾಙ್ಕ first_addr, ಪೂರ್ಣಾಙ್ಕ last_addr,
                                 bool signed_flag);
friend ಶೂನ್ಯ compile_array_alias(ವರ್ಣ*label, ವರ್ಣ*name, ವರ್ಣ*src);
};

class __vpiDarrayVar : ಸಾರ್ವಜನಿಕ __vpiBaseVar, ಸಾರ್ವಜನಿಕ __vpiArrayBase {
    ಸಾರ್ವಜನಿಕ:
      __vpiDarrayVar(__vpiScope*scope, ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net);

      ಪೂರ್ಣಾಙ್ಕ get_type_code() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiArrayVar; }
      unsigned get_size() ಧ್ರುವ override;
      vpiHandle get_ಎಡ_range() override;
      vpiHandle get_ಬಲ_range() override;
      __vpiScope*get_scope() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ scope_; }

      ಪೂರ್ಣಾಙ್ಕ get_word_size() ಧ್ರುವ override;
      ವರ್ಣ*get_word_str(ಸಂರಚನಾ __vpiArrayWord*word, ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ get_word_ಮೌಲ್ಯ(ಸಂರಚನಾ __vpiArrayWord*word, p_vpi_ಮೌಲ್ಯ vp) override;
      ಶೂನ್ಯ put_word_ಮೌಲ್ಯ(ಸಂರಚನಾ __vpiArrayWord*word, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ flags) override;

      vpiHandle get_iter_index(ಸಂರಚನಾ __vpiArrayIterator*iter, ಪೂರ್ಣಾಙ್ಕ idx) override;
      ಅನ್ತಃಸ್ಥ vpiHandle vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code) override { ಪ್ರತ್ಯಾಗಮ vpi_array_base_iterate(code); }

      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ* vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_index(ಪೂರ್ಣಾಙ್ಕ index) override;

      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

    ರಕ್ಷಿತ:
      vvp_darray*get_vvp_darray() ಧ್ರುವ;
      __vpiDecConst ಎಡ_range, ಬಲ_range;
};

ಬಾಹ್ಯ vpiHandle vpip_make_darray_var(ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net);

class __vpiQueueVar : ಸಾರ್ವಜನಿಕ __vpiDarrayVar {

    ಸಾರ್ವಜನಿಕ:
      __vpiQueueVar(__vpiScope*scope, ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net);

      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
};

ಬಾಹ್ಯ vpiHandle vpip_make_queue_var(ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net);

class __vpiPropQueueRef : ಸಾರ್ವಜನಿಕ __vpiHandle {

    ಸಾರ್ವಜನಿಕ:
      explicit __vpiPropQueueRef(__vpiScope*scope, unsigned pidx, bool is_queue);

      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ* vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

      vvp_net_t* class_net_;
      unsigned prop_idx_;
      bool is_queue_;

    ಗುಪ್ತ:
      __vpiScope* scope_;
};

ಬಾಹ್ಯ vpiHandle vpip_make_prop_queue_ref(ವರ್ಣ* class_label,
                                          unsigned prop_idx,
                                          unsigned is_queue_flag);
ಬಾಹ್ಯ vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : ಸಾರ್ವಜನಿಕ __vpiBaseVar {

    ಸಾರ್ವಜನಿಕ:
      __vpiCobjectVar(__vpiScope*scope, ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net);

      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
};

ಬಾಹ್ಯ vpiHandle vpip_make_cobject_var(ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net);

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
ಸಂರಚನಾ __vpiUserSystf : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiUserSystf();
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;

      s_vpi_systf_data info;
      bool is_user_defn;
};

ಬಾಹ್ಯ vpiHandle vpip_make_systf_iterator(ಶೂನ್ಯ);

ಬಾಹ್ಯ ಸಂರಚನಾ __vpiUserSystf* vpip_find_systf(ಧ್ರುವ ವರ್ಣ*name);


ಸಂರಚನಾ __vpiSysTaskCall : ಸಾರ್ವಜನಿಕ __vpiHandle {

      __vpiScope* scope;
      ಸಂರಚನಾ __vpiUserSystf*defn;
      unsigned nargs;
      vpiHandle*args;
	/* Stack consumed by this call */
      unsigned vec4_ರಾಶಿ;
      unsigned real_ರಾಶಿ;
      unsigned string_ರಾಶಿ;
	/* Support for vpi_get_userdata. */
      ಶೂನ್ಯ*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      unsigned file_idx;
      unsigned lineno;
      bool put_ಮೌಲ್ಯ;
    ರಕ್ಷಿತ:
      ಅನ್ತಃಸ್ಥ __vpiSysTaskCall()
      {
	    vec4_ರಾಶಿ = 0;
	    real_ರಾಶಿ = 0;
	    string_ರಾಶಿ = 0;
      }
};

ಬಾಹ್ಯ ಸಂರಚನಾ __vpiSysTaskCall*vpip_cur_task;

/*
 * The persistent flag to vpip_make_string_const causes the created
 * handle to be persistent. This is necessary for cases where the
 * string handle may be reused, which is the normal case.
 *
 * When constructing with a string, the class takes possession of the
 * text value string, and will delete it in the constructor.
 */

vpiHandle vpip_make_string_const(ವರ್ಣ*text, bool persistent =true);
vpiHandle vpip_make_string_param(ಧ್ರುವ ವರ್ಣ*name, ವರ್ಣ*ಮೌಲ್ಯ, bool local_flag,
                                 long file_idx, long lineno);

ಸಂರಚನಾ __vpiBinaryConst : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiBinaryConst();
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

      vvp_vector4_t bits;
	/* TRUE if this constant is signed. */
      unsigned signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      unsigned sized_flag  :1;
};

vpiHandle vpip_make_ದ್ವಿಮಾನ_const(unsigned wid, ಧ್ರುವ ವರ್ಣ*bits);
vpiHandle vpip_make_ದ್ವಿಮಾನ_param(ಧ್ರುವ ವರ್ಣ*name, ಧ್ರುವ vvp_vector4_t&bits,
				 bool signed_flag, bool local_flag,
				 long file_idx, long lineno);

class __vpiRealConst : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      explicit __vpiRealConst(ದ್ವಿಗುಣ);
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

      ದ್ವಿಗುಣ ಮೌಲ್ಯ;
};

vpiHandle vpip_make_real_const(ದ್ವಿಗುಣ ಮೌಲ್ಯ);
vpiHandle vpip_make_real_param(ಧ್ರುವ ವರ್ಣ*name, ದ್ವಿಗುಣ ಮೌಲ್ಯ, bool local_flag,
                               long file_idx, long lineno);

class __vpiNullConst : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      explicit __vpiNullConst();
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_word(unsigned base, ಧ್ರುವ ವರ್ಣ*type);
vpiHandle vpip_make_vthr_str_ರಾಶಿ(unsigned depth);
vpiHandle vpip_make_vthr_vec4_ರಾಶಿ(unsigned depth, bool signed_flag, unsigned wid);

vpiHandle vpip_make_vthr_A(ವರ್ಣ*label, unsigned index);
vpiHandle vpip_make_vthr_A(ವರ್ಣ*label, ವರ್ಣ*ಚಿಹ್ನೆ);
vpiHandle vpip_make_vthr_A(ವರ್ಣ*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(ವರ್ಣ*label, unsigned index, unsigned bit, unsigned wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
ಬಾಹ್ಯ "C" ಶೂನ್ಯ vpip_load_module(ಧ್ರುವ ವರ್ಣ*name);

ಬಾಹ್ಯ "C" ಶೂನ್ಯ vpip_clear_module_paths();
ಬಾಹ್ಯ "C" ಶೂನ್ಯ vpip_add_module_path(ಧ್ರುವ ವರ್ಣ *path);
ಬಾಹ್ಯ "C" ಶೂನ್ಯ vpip_add_env_and_default_module_paths();

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
ಬಾಹ್ಯ vpiHandle vpip_build_vpi_call(ಧ್ರುವ ವರ್ಣ*name,
				     ಪೂರ್ಣಾಙ್ಕ val_type, unsigned return_width,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     unsigned argc,
				     vpiHandle*argv,
				     unsigned vec4_ರಾಶಿ,
				     unsigned real_ರಾಶಿ,
				     unsigned string_ರಾಶಿ,
				     long file_idx,
				     long lineno);

ಬಾಹ್ಯ vthread_t vpip_current_vthread;

ಬಾಹ್ಯ ಶೂನ್ಯ vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_time(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ vpip_get_time_precision(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_set_time_precision(ಪೂರ್ಣಾಙ್ಕ pres);

ಬಾಹ್ಯ __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ vpip_time_units_from_handle(vpiHandle obj);
ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ vpip_time_precision_from_handle(vpiHandle obj);

ಬಾಹ್ಯ ದ್ವಿಗುಣ vpip_scaled_time_from_handle(vvp_time64_t time, vpiHandle obj);

ಬಾಹ್ಯ ಶೂನ್ಯ vpip_time_to_timestruct(ಸಂರಚನಾ t_vpi_time*ts, vvp_time64_t ti);
ಬಾಹ್ಯ vvp_time64_t vpip_timestruct_to_time(ಧ್ರುವ ಸಂರಚನಾ t_vpi_time*ts);

ಬಾಹ್ಯ ದ್ವಿಗುಣ vpip_time_to_scaled_real(vvp_time64_t ti, ಧ್ರುವ __vpiScope*sc);
ಬಾಹ್ಯ vvp_time64_t vpip_scaled_real_to_time64(ದ್ವಿಗುಣ val, ಧ್ರುವ __vpiScope*sc);

/*
 * These functions are used mostly as compile time to strings into
 * permallocated memory. The vpip_string function is the most general,
 * it allocates a fresh string no matter what. The vpip_name_string
 * allocates a string and keeps a pointer in the hash, and tries to
 * reuse it if it can. This us useful for handle names, which may be
 * reused in different scopes.
 */
ಬಾಹ್ಯ ಧ್ರುವ ವರ್ಣ* vpip_string(ಧ್ರುವ ವರ್ಣ*str);
ಬಾಹ್ಯ ಧ್ರುವ ವರ್ಣ* vpip_name_string(ಧ್ರುವ ವರ್ಣ*str);


/*
 * This function is used to make decimal string versions of various
 * vectors. The input format is a vvp_vector4_t, and the result is
 * written into buf, without overflowing nbuf.
 */
ಬಾಹ್ಯ unsigned vpip_vec4_to_dec_str(ಧ್ರುವ vvp_vector4_t&vec4,
				     ವರ್ಣ *buf, unsigned ಪೂರ್ಣಾಙ್ಕ nbuf,
				     ಪೂರ್ಣಾಙ್ಕ signed_flag);


ಬಾಹ್ಯ ಶೂನ್ಯ vpip_vec4_to_hex_str(ಧ್ರುವ vvp_vector4_t&bits, ವರ್ಣ*buf,
				 unsigned nbuf);

ಬಾಹ್ಯ ಶೂನ್ಯ vpip_vec4_to_oct_str(ಧ್ರುವ vvp_vector4_t&bits, ವರ್ಣ*buf,
				 unsigned nbuf);

ಬಾಹ್ಯ ಶೂನ್ಯ vpip_bin_str_to_vec4(vvp_vector4_t&val, ಧ್ರುವ ವರ್ಣ*buf);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_oct_str_to_vec4(vvp_vector4_t&val, ಧ್ರುವ ವರ್ಣ*str);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_dec_str_to_vec4(vvp_vector4_t&val, ಧ್ರುವ ವರ್ಣ*str);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_hex_str_to_vec4(vvp_vector4_t&val, ಧ್ರುವ ವರ್ಣ*str);

ಬಾಹ್ಯ vvp_vector4_t vec4_from_vpi_ಮೌಲ್ಯ(s_vpi_ಮೌಲ್ಯ*vp, unsigned wid);
ಬಾಹ್ಯ ದ್ವಿಗುಣ real_from_vpi_ಮೌಲ್ಯ(s_vpi_ಮೌಲ್ಯ*vp);

ಬಾಹ್ಯ ಶೂನ್ಯ vpip_vec4_get_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector4_t&word_val, unsigned width,
				bool signed_flag, s_vpi_ಮೌಲ್ಯ*vp);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_vec2_get_ಮೌಲ್ಯ(ಧ್ರುವ vvp_vector2_t&word_val, unsigned width,
				bool signed_flag, s_vpi_ಮೌಲ್ಯ*vp);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_real_get_ಮೌಲ್ಯ(ದ್ವಿಗುಣ real, s_vpi_ಮೌಲ್ಯ*vp);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_string_get_ಮೌಲ್ಯ(ಧ್ರುವ std::string&val, s_vpi_ಮೌಲ್ಯ*vp);

/*
 * Function defined in vpi_signal.cc to manage vpi_get_* persistent
 * storage.
 */
ಗಣನಾ vpi_rbuf_t {
      RBUF_VAL =0,
	/* Storage for *_get_value() */
      RBUF_STR,
	/* Storage for *_get_str() */
      RBUF_DEL
	/* Delete the storage for both buffers. */
};
ಬಾಹ್ಯ ಶೂನ್ಯ *need_result_buf(unsigned cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
ಬಾಹ್ಯ ವರ್ಣ *simple_set_rbuf_str(ಧ್ರುವ ವರ್ಣ *s1);
ಬಾಹ್ಯ ವರ್ಣ *generic_get_str(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref, ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *index);

/* A routine to find the enclosing module. */
ಬಾಹ್ಯ vpiHandle vpip_module(__vpiScope*scope);

ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
ಬಾಹ್ಯ bool vpip_mcd0_disable;

ಅನ್ತಃಸ್ಥ uint64_t vlg_round_to_u64(ದ್ವಿಗುಣ rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      ಪ್ರತ್ಯಾಗಮ static_cast<uint64_t>(static_cast<int64_t>(std::round(rval)));
}

#endif /* IVL_vpi_priv_H */
