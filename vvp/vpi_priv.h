#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_vpi_priv_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_vpi_priv_H
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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ __vpiArray* vvp_array_t;

/*
 * This header file contains the internal definitions that the vvp
 * program uses to implement the public interface in the vpi_user.h
 * header file elsewhere.
 */

#ಆದರೆ defined(__MINGW32__) || defined (__CYGWIN__)
ಬಾಹ್ಯ vpip_routines_s vpi_routines;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು vpi_File_ಸಾಲು    0x1000003
#ವ್ಯಾಖ್ಯಾನಿಸು _vpiDescription 0x1000004

ಬಾಹ್ಯ bool show_file_ಸಾಲು;
ಬಾಹ್ಯ bool ಸಂಕೇತ_is_instrumented;

ಬಾಹ್ಯ vpiHandle vpip_build_file_ಸಾಲು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*description,
                                      ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* vpi_type_as_string(PLI_INT32 ಸಂಕೇತ_2);

/*
 * Return the port direction as a string.
 */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* direction_as_string(ಪೂರ್ಣಾಂಕ dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#ಆದರೆ defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
#ವ್ಯಾಖ್ಯಾನಿಸು _vpiFromThr 0x1000001
#   define _vpiNoThr   0
#   define _vpiString  1
#   define _vpiVThr    2
#   define vpi_ಶಬ್ದ    3
#   define _vpi_at_PV  4
#   define _vpi_at_A   5
#   define _vpi_at_APV 6
#ಆದರೆ_ಅಂತ್ಯ


/*
 * The vpi_mode_flag contains the major mode for VPI use. This is used
 * to generate error messages when vpi functions are called
 * incorrectly.
 */
ಎಣಿಕೆ vpi_ಬಹುಲಕ_t {
      VPI_ಬಹುಲಕ_NONE =0,
	/* The compiler is calling a register function. */
      VPI_ಬಹುಲಕ_ರಿಜಿಸ್ಟರ್,
	/* The compiler is calling a compiletf function. */
      VPI_ಬಹುಲಕ_COMPILETF,
	/* The compiler is calling a calltf function. */
      VPI_ಬಹುಲಕ_CALLTF,
	/* We are in the midst of a RWSync callback. */
      VPI_ಬಹುಲಕ_RWSYNC,
	/* We are in a ROSync callback. */
      VPI_ಬಹುಲಕ_ROSYNC
};
ಬಾಹ್ಯ vpi_ಬಹುಲಕ_t vpi_ಬಹುಲಕ_flag;

/*
 * This structure is the very base of a vpiHandle. Every handle
 * structure is derived from this class so that the library can
 * internally pass the derived types as pointers to one of these.
 */
class __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ __vpiHandle() { }
	// The destructor is virtual so that dynamic types will work.
      ವಾಸ್ತವಿಕ ~__vpiHandle();

      ವಾಸ್ತವಿಕ ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ =0;
      ವಾಸ್ತವಿಕ ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2);
      ವಾಸ್ತವಿಕ int64_t vpi_get64(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2);
      ವಾಸ್ತವಿಕ ಅಕ್ಷರ* vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2);

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val);
      ವಾಸ್ತವಿಕ vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags);
      ವಾಸ್ತವಿಕ vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2);
      ವಾಸ್ತವಿಕ vpiHandle vpi_iterate(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2);
      ವಾಸ್ತವಿಕ vpiHandle vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ idx);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ vpi_get_delays(p_vpi_delay del);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ vpi_put_delays(p_vpi_delay del);

	// Objects may have destroyer functions of their own. If so,
	// then this virtual method will return a POINTER to that
	// function. The pointer is used to "delete" the object, which
	// is why the function itself cannot be a method.
      ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*free_object_fun_t)(vpiHandle);
      ವಾಸ್ತವಿಕ free_object_fun_t free_object_fun(ಶೂನ್ಯ);
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
ರಚನೆ __vpiIterator : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiIterator();
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      free_object_fun_t free_object_fun(ಶೂನ್ಯ) override;

      vpiHandle *ವಾದಗಳು;
      ಚಿಹ್ನೆರಹಿತ  nargs;
      ಚಿಹ್ನೆರಹಿತ  ಮುಂದಿನ_2;
      bool free_ವಾದಗಳು_flag;
};

ಬಾಹ್ಯ vpiHandle vpip_make_iterator(ಚಿಹ್ನೆರಹಿತ nargs, vpiHandle*ವಾದಗಳು,
				    bool free_ವಾದಗಳು_flag);

class __vpiDecConst : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      explicit __vpiDecConst(ಪೂರ್ಣಾಂಕ val =0);
      __vpiDecConst(ಸ್ಥಿರಾಂಕ __vpiDecConst&that);
# ಆದರೆ __cplusplus >= 201103L
      __vpiDecConst& ಕ್ರಿಯಾಕಾರಕ = (ಸ್ಥಿರಾಂಕ __vpiDecConst&that) = ಪೂರ್ವನಿಯೋಜಿತ;
# endif
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ get_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ; }
      ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಗಣ_ಮೌಲ್ಯ(ಪೂರ್ಣಾಂಕ val) { ಮೌಲ್ಯ = val; }

    ಖಾಸಗಿ:
      ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
ರಚನೆ __vpiCallback : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;

	// Used for listing callbacks.
      ರಚನೆ __vpiCallback*ಮುಂದಿನ_2;

	// user supplied callback data
      ರಚನೆ t_cb_ದತ್ತಾಂಶ cb_ದತ್ತಾಂಶ;
};

class ಮೌಲ್ಯ_callback : ಸಾರ್ವಜನಿಕ __vpiCallback {
    ಸಾರ್ವಜನಿಕ:
      explicit ಮೌಲ್ಯ_callback(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ);
	// Return true if the callback really is ready to be called
      ವಾಸ್ತವಿಕ bool test_ಮೌಲ್ಯ_callback_ready(ಶೂನ್ಯ);

    ಸಾರ್ವಜನಿಕ:
	// user supplied callback data
      ರಚನೆ t_vpi_ಸಮಯ cb_ಸಮಯ;
      ರಚನೆ t_vpi_ಮೌಲ್ಯ cb_ಮೌಲ್ಯ;
};

ಬಾಹ್ಯ ಶೂನ್ಯ callback_execute(ರಚನೆ __vpiCallback*cur);

ರಚನೆ __vpiSystemTime : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiSystemTime();
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಅಕ್ಷರ*vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;

      __vpiScope*scope;
};

ರಚನೆ __vpiScopedTime : ಸಾರ್ವಜನಿಕ __vpiSystemTime {
      __vpiScopedTime();
      ಅಕ್ಷರ*vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
};
ರಚನೆ __vpiScopedSTime : ಸಾರ್ವಜನಿಕ __vpiSystemTime {
      __vpiScopedSTime();
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಅಕ್ಷರ*vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
};
ರಚನೆ __vpiScopedRealtime : ಸಾರ್ವಜನಿಕ __vpiSystemTime {
      __vpiScopedRealtime();
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಅಕ್ಷರ*vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : ಸಾರ್ವಜನಿಕ __vpiHandle {

    ಸಾರ್ವಜನಿಕ:
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಅಕ್ಷರ* vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_iterate(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;

    ಸಾರ್ವಜನಿಕ:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*scope_name() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ name_; }

      ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*scope_def_name() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ tname_; }
	// TRUE if this is an automatic func/task/block
      ಸಾಲೊಳಗೆ bool is_automatic() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ is_automatic_; }

    ಸಾರ್ವಜನಿಕ:
      __vpiScope *scope;
      ಚಿಹ್ನೆರಹಿತ file_idx;
      ಚಿಹ್ನೆರಹಿತ lineno;
      ಚಿಹ್ನೆರಹಿತ def_file_idx;
      ಚಿಹ್ನೆರಹಿತ def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_ಸಮಯ;
      ರಚನೆ __vpiScopedSTime scoped_stime;
      ರಚನೆ __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::ಸದಿಶ<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      ರಚನೆ automatic_hooks_s**ಅಂಶ;
      ಚಿಹ್ನೆರಹಿತ nitem;
        /* Keep a list of live contexts. */
      vvp_context_t ಜೀವಂತ_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::ಗಣ<vthread_t> ಎಳೆಗಳು;
      ಚಿಹ್ನೆಯುಳ್ಳ ಪೂರ್ಣಾಂಕ ಸಮಯ_units :8;
      ಚಿಹ್ನೆಯುಳ್ಳ ಪೂರ್ಣಾಂಕ ಸಮಯ_precision :8;

    ಸಂರಕ್ಷಿತ:
      __vpiScope(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nam, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tnam, bool is_auto_flag =false);

    ಖಾಸಗಿ:
	/* The scope has a name. */
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name_;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tname_;
	/* the scope may be "automatic" */
      bool is_automatic_;
};

class vpiScopeFunction  : ಸಾರ್ವಜನಿಕ __vpiScope {
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ vpiScopeFunction(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nam, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tnam,
			      bool auto_flag, ಪೂರ್ಣಾಂಕ func_type, ಚಿಹ್ನೆರಹಿತ func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiFunction; }
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override
      {
	    ಆಯ್ಕೆ (ಸಂಕೇತ_2) {
		ಸ್ಥಿತಿ vpiFuncType:
		  ಹಿಂತಿರುಗಿ func_type_;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  ಹಿಂತಿರುಗಿ __vpiScope::vpi_get(ಸಂಕೇತ_2);
	    }
      }

    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ get_func_ಅಗಲ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ func_wid_; }
      ಸಾಲೊಳಗೆ vvp_bit4_t get_func_init_val(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ func_init_val_; }

    ಖಾಸಗಿ:
      ಪೂರ್ಣಾಂಕ func_type_;
      ಚಿಹ್ನೆರಹಿತ func_wid_;
      vvp_bit4_t func_init_val_;
};

ಬಾಹ್ಯ __vpiScope* vpip_peek_current_scope(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_attach_ಗಮ್ಯ_scope(__vpiScope*scope, vpiHandle obj);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_attach_ಗಮ್ಯ_current_scope(vpiHandle obj);
ಬಾಹ್ಯ __vpiScope* vpip_peek_context_scope(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ vpip_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_context(automatic_hooks_s*ಅಂಶ,
                                         __vpiScope*scope);
ಬಾಹ್ಯ vpiHandle vpip_make_ಬೇರು_iterator(ಪೂರ್ಣಾಂಕ type_ಸಂಕೇತ);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_make_ಬೇರು_iterator(class __vpiHandle**&ಕೋಷ್ಟಕ,
				    ಚಿಹ್ನೆರಹಿತ&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
ರಚನೆ __vpiSignal : ಸಾರ್ವಜನಿಕ __vpiHandle {
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      int64_t vpi_get64(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಅಕ್ಷರ* vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_iterate(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ idx) override;

    ಸಾರ್ವಜನಿಕ:
      ಚಿಹ್ನೆರಹಿತ ಅಗಲ() ಸ್ಥಿರಾಂಕ;
      vpiHandle get_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ);
      ಶೂನ್ಯ get_ಬಿಟ್_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ರಚನೆ __vpiBit*ಬಿಟ್, p_vpi_ಮೌಲ್ಯ vp);
      vpiHandle put_ಬಿಟ್_ಮೌಲ್ಯ(ರಚನೆ __vpiBit*ಬಿಟ್, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ flags);
      ಶೂನ್ಯ make_ಬಿಟ್ಟುಗಳು();

      ರಚನೆ __vpiBit*ಬಿಟ್ಟುಗಳು;

    ಸಾರ್ವಜನಿಕ:
      ಒಕ್ಕೂಟ { // The scope or parent array that contains me.
	    vpiHandle ಪೋಷಕ;
	    __vpiScope* scope;
      } within;
      ಒಕ್ಕೂಟ { // The name of this reg/net, or the index for array words.
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name;
            vpiHandle ಸೂಚ್ಯಂಕ;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      ಚಿಹ್ನೆರಹಿತ signed_flag  : 1;
      ಚಿಹ್ನೆರಹಿತ is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*ಗಂಟು;

    ಸಾರ್ವಜನಿಕ:
      ಸ್ಥಿರ ಶೂನ್ಯ*ಕ್ರಿಯಾಕಾರಕ new(std::size_t ಗಾತ್ರ_2);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ delete(ಶೂನ್ಯ*); // not implemented
    ಸಂರಕ್ಷಿತ:
      ಸಾಲೊಳಗೆ __vpiSignal() : ಬಿಟ್ಟುಗಳು(ಖಾಲಿ) { }
    ಖಾಸಗಿ: // Not implemented
      ಸ್ಥಿರ ಶೂನ್ಯ*ಕ್ರಿಯಾಕಾರಕ new[] (std::size_t ಗಾತ್ರ_2);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ delete[](ಶೂನ್ಯ*);
};
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ vpip_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ __vpiSignal *sig);
ಬಾಹ್ಯ __vpiScope* vpip_scope(__vpiSignal*sig);

ಬಾಹ್ಯ vpiHandle vpip_make_int2(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb,
			       bool signed_flag, vvp_net_t*vec);
ಬಾಹ್ಯ vpiHandle vpip_make_int4(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb,
			       vvp_net_t*vec);
ಬಾಹ್ಯ vpiHandle vpip_make_var4(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb,
			       bool signed_flag, vvp_net_t*net);
ಬಾಹ್ಯ vpiHandle vpip_make_net4(__vpiScope*scope,
				ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb,
				bool signed_flag, vvp_net_t*ಗಂಟು);

/*
 * This is used to represent a bit in a net/reg.
 */
ರಚನೆ __vpiBit {
      ರಚನೆ as_ಬಿಟ್_t : ಸಾರ್ವಜನಿಕ __vpiHandle {
	    ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ;
	    ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2);
	    ಅಕ್ಷರ* vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2);
	    ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val);
	    vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags);
	    vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2);
      } as_ಬಿಟ್;

      vpiHandle ಸೂಚ್ಯಂಕ;

      ಒಕ್ಕೂಟ {
	    ರಚನೆ __vpiSignal*ಪೋಷಕ;
	    ರಚನೆ __vpiBit*bit0;
      };

      ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ get_norm_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ this - bit0; }
      ಸಾಲೊಳಗೆ ರಚನೆ __vpiSignal*get_ಪೋಷಕ() ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ (bit0 - 1)->ಪೋಷಕ; }
      ಪೂರ್ಣಾಂಕ get_ಸೂಚ್ಯಂಕ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ;
};

class vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info;

class vpi_ಪೋರ್ಟ್_Info  : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      vpi_ಪೋರ್ಟ್_Info( __vpiScope *ಪೋಷಕ,
                    ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ,
                    ಪೂರ್ಣಾಂಕ vpi_direction,
                    ಚಿಹ್ನೆರಹಿತ ಅಗಲ,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                    ಅಕ್ಷರ* ಬಫರ್ );
      ~vpi_ಪೋರ್ಟ್_Info() override;

      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpi_ಪೋರ್ಟ್; }
      ಪೂರ್ಣಾಂಕ get_direction(ಶೂನ್ಯ) { ಹಿಂತಿರುಗಿ direction_; }
      ಚಿಹ್ನೆರಹಿತ get_ಸೂಚ್ಯಂಕ(ಶೂನ್ಯ) { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ_2; }
      ಪೂರ್ಣಾಂಕ get_ಅಗಲ(ಶೂನ್ಯ) { ಹಿಂತಿರುಗಿ ಅಗಲ_2; }
      ಶೂನ್ಯ ಕೂಡಿಸು_ಪೋರ್ಟ್_ಬಿಟ್(vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info* ಪೋರ್ಟ್_ಬಿಟ್) { ಪೋರ್ಟ್_ಬಿಟ್ಟುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(ಪೋರ್ಟ್_ಬಿಟ್); }

      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಅಕ್ಷರ* vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vvp_net_t* get_ಪೋರ್ಟ್(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ref_; }
      vpiHandle vpi_iterate(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;

      std::ಸದಿಶ<vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info*> ಪೋರ್ಟ್_ಬಿಟ್ಟುಗಳು;

    ಖಾಸಗಿ:
      __vpiScope *ಪೋಷಕ_2;
      ಚಿಹ್ನೆರಹಿತ  ಸೂಚ್ಯಂಕ_2;
      ಪೂರ್ಣಾಂಕ       direction_;
      ಚಿಹ್ನೆರಹಿತ  ಅಗಲ_2;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name_;
      vvp_net_t *ref_;
};

class vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info  : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info(vpi_ಪೋರ್ಟ್_Info *ಪೋಷಕ,
                     ಚಿಹ್ನೆರಹಿತ ಬಿಟ್);
      ~vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info() override;

      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpi_ಪೋರ್ಟ್_ಬಿಟ್; }
      ಚಿಹ್ನೆರಹಿತ get_ಬಿಟ್(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಬಿಟ್_2; }

      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;

    ಖಾಸಗಿ:
      vpi_ಪೋರ್ಟ್_Info *ಪೋಷಕ_2;
      ಚಿಹ್ನೆರಹಿತ ಬಿಟ್_2;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
ರಚನೆ __vpiPV : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiPV();
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಅಕ್ಷರ* vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;

      vpiHandle ಪೋಷಕ;
      vvp_net_t*net;
      vpiHandle sbase;
      ಪೂರ್ಣಾಂಕ tbase;
      ಚಿಹ್ನೆರಹಿತ ಅಗಲ;
};
ಬಾಹ್ಯ vpiHandle vpip_make_PV(ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ ತಳ, ಪೂರ್ಣಾಂಕ ಅಗಲ);
ಬಾಹ್ಯ vpiHandle vpip_make_PV(ಅಕ್ಷರ*name, ಅಕ್ಷರ*ಚಿಹ್ನೆ, ಪೂರ್ಣಾಂಕ ಅಗಲ);
ಬಾಹ್ಯ vpiHandle vpip_make_PV(ಅಕ್ಷರ*name, vpiHandle handle, ಪೂರ್ಣಾಂಕ ಅಗಲ);

ರಚನೆ __vpiModPathTerm : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiModPathTerm();
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;

      vpiHandle ಅಭಿವ್ಯಕ್ತಿ;
	/* The value returned by vpi_get(vpiEdge, ...); */
      ಪೂರ್ಣಾಂಕ ಅಂಚು;
};

ರಚನೆ __vpiModPathSrc : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiModPathSrc();
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_iterate(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ idx) override;
      ಶೂನ್ಯ vpi_get_delays(p_vpi_delay del) override;
      ಶೂನ್ಯ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(ಶೂನ್ಯ) override;

      ರಚನೆ __vpiModPath *dest;
      ಪೂರ್ಣಾಂಕ   type;

	/* This is the input expression for this modpath. */
      ರಚನೆ __vpiModPathTerm ಮಾರ್ಗ_ಪದ_ಒಳ;

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

ರಚನೆ __vpiModPath {
      __vpiScope   *scope ;

      class vvp_fun_modpath*modpath;

      ರಚನೆ __vpiModPathTerm ಮಾರ್ಗ_ಪದ_ಹೊರಹೋಗು;
      vvp_net_t *ಒಳಹರಿವು_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

ಬಾಹ್ಯ ರಚನೆ __vpiModPathSrc* vpip_make_modpath_src(ರಚನೆ __vpiModPath*ಮಾರ್ಗ,
                                                     vvp_net_t *net) ;

ಬಾಹ್ಯ ರಚನೆ __vpiModPath* vpip_make_modpath(vvp_net_t *net) ;

/*
 *
 * The vpiInterModPath vpiHandle will define
 * a vpiInterModPath of record .intermodpath as defined
 * in the IEEE 1364
 *
 */

ರಚನೆ __vpiInterModPath : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiInterModPath();
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_iterate(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_delays(p_vpi_delay del) override;
      ಶೂನ್ಯ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(ಶೂನ್ಯ) override;

      __vpiScope   *scope ;

      class vvp_fun_intermodpath*intermodpath;

      vvp_net_t *net;

      vpi_ಪೋರ್ಟ್_Info* port1;
      vpi_ಪೋರ್ಟ್_Info* port2;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiInterModPath
 */

ಬಾಹ್ಯ ರಚನೆ __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpi_ಪೋರ್ಟ್_Info* port1, vpi_ಪೋರ್ಟ್_Info* port2);

/*
 * These methods support the vpi creation of events. The name string
 * passed in will be saved, so the caller must allocate it (or not
 * free it) after it is handed to this function.
 */
class __vpiNamedEvent : ಸಾರ್ವಜನಿಕ __vpiHandle {

    ಸಾರ್ವಜನಿಕ:
      __vpiNamedEvent(__vpiScope*scope, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name);
      ~__vpiNamedEvent() override;

      __vpiNamedEvent(ಸ್ಥಿರಾಂಕ __vpiNamedEvent&) = delete;
      __vpiNamedEvent& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ __vpiNamedEvent&) = delete;

      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      __vpiScope*get_scope(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ scope_; }
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಅಕ್ಷರ* vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;

      ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಕೂಡಿಸು_vpi_callback(__vpiCallback*cb)
      { cb->ಮುಂದಿನ_2 = callbacks_;
	callbacks_ = cb;
      }

      ಶೂನ್ಯ ಸತತ_vpi_callbacks(ಶೂನ್ಯ);

	/* The functor, used for %set operations. */
      vvp_net_t*funct;

    ಖಾಸಗಿ:
	/* base name of the event object */
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name_;
	/* Parent scope of this object. */
      __vpiScope*scope_;
	/* List of callbacks interested in this event. */
      __vpiCallback*callbacks_;
};

ಬಾಹ್ಯ vpiHandle vpip_make_named_event(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*f);

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
ರಚನೆ __vpiRealVar : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiRealVar();
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಅಕ್ಷರ* vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_iterate(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;

      ಒಕ್ಕೂಟ { // The scope or parent array that contains me.
	    vpiHandle ಪೋಷಕ;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      ಒಕ್ಕೂಟ {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name;
            vpiHandle ಸೂಚ್ಯಂಕ;
      } id;
      ಚಿಹ್ನೆರಹಿತ is_netarray  : 1; // This is word of a net array
      ಚಿಹ್ನೆರಹಿತ is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

ಬಾಹ್ಯ __vpiScope* vpip_scope(__vpiRealVar*sig);
ಬಾಹ್ಯ vpiHandle vpip_make_real_var(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net);
ಬಾಹ್ಯ vpiHandle vpip_make_real_net(__vpiScope*scope,
				    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net);

class __vpiBaseVar : ಸಾರ್ವಜನಿಕ __vpiHandle {

    ಸಾರ್ವಜನಿಕ:
      __vpiBaseVar(__vpiScope*scope, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#ಆದರೆ_ಅಂತ್ಯ

      ಸಾಲೊಳಗೆ vvp_net_t* get_net() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ net_; }

    ಸಂರಕ್ಷಿತ:
      __vpiScope* scope_;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name_;

    ಖಾಸಗಿ:
      vvp_net_t*net_;
};

class __vpiStringVar : ಸಾರ್ವಜನಿಕ __vpiBaseVar {

    ಸಾರ್ವಜನಿಕ:
      __vpiStringVar(__vpiScope*scope, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net);

      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags) override;
};

ಬಾಹ್ಯ vpiHandle vpip_make_string_var(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net);

ರಚನೆ __vpiArrayBase {
      __vpiArrayBase() : vals_ಪದಗಳು(ಖಾಲಿ) {}
      ವಾಸ್ತವಿಕ ~__vpiArrayBase() {}

      ವಾಸ್ತವಿಕ ಚಿಹ್ನೆರಹಿತ get_ಗಾತ್ರ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ = 0;
      ವಾಸ್ತವಿಕ vpiHandle get_ಎಡ_range() = 0;
      ವಾಸ್ತವಿಕ vpiHandle get_ಬಲ_range() = 0;
      ವಾಸ್ತವಿಕ __vpiScope*get_scope() ಸ್ಥಿರಾಂಕ = 0;

      ವಾಸ್ತವಿಕ ಪೂರ್ಣಾಂಕ get_ಶಬ್ದ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ = 0;
      ವಾಸ್ತವಿಕ ಅಕ್ಷರ*get_ಶಬ್ದ_str(ರಚನೆ __vpiArrayWord*ಶಬ್ದ, ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) = 0;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ get_ಶಬ್ದ_ಮೌಲ್ಯ(ರಚನೆ __vpiArrayWord*ಶಬ್ದ, p_vpi_ಮೌಲ್ಯ vp) = 0;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ put_ಶಬ್ದ_ಮೌಲ್ಯ(ರಚನೆ __vpiArrayWord*ಶಬ್ದ, p_vpi_ಮೌಲ್ಯ vp,
                                    ಪೂರ್ಣಾಂಕ flags) = 0;
      ವಾಸ್ತವಿಕ vpiHandle get_iter_ಸೂಚ್ಯಂಕ(ರಚನೆ __vpiArrayIterator*iter, ಪೂರ್ಣಾಂಕ idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_ತಳ_iterate(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2);

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ make_vals_ಪದಗಳು();

      ರಚನೆ __vpiArrayWord*vals_ಪದಗಳು;
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
ರಚನೆ __vpiArray : ಸಾರ್ವಜನಿಕ __vpiArrayBase, ಸಾರ್ವಜನಿಕ __vpiHandle {
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpi_ಸ್ಮರಣೆ; }
      ಚಿಹ್ನೆರಹಿತ get_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ array_ಎಣಿಕೆ; }
      vpiHandle get_ಎಡ_range() override { ಪ್ರತಿಜ್ಞಾ(nets == 0); ಹಿಂತಿರುಗಿ &msb; }
      vpiHandle get_ಬಲ_range() override { ಪ್ರತಿಜ್ಞಾ(nets == 0); ಹಿಂತಿರುಗಿ &lsb; }
      __vpiScope*get_scope() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ scope; }

      ಪೂರ್ಣಾಂಕ get_ಶಬ್ದ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override;
      ಅಕ್ಷರ*get_ಶಬ್ದ_str(ರಚನೆ __vpiArrayWord*ಶಬ್ದ, ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ get_ಶಬ್ದ_ಮೌಲ್ಯ(ರಚನೆ __vpiArrayWord*ಶಬ್ದ, p_vpi_ಮೌಲ್ಯ vp) override;
      ಶೂನ್ಯ put_ಶಬ್ದ_ಮೌಲ್ಯ(ರಚನೆ __vpiArrayWord*ಶಬ್ದ, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ flags) override;

      vpiHandle get_iter_ಸೂಚ್ಯಂಕ(ರಚನೆ __vpiArrayIterator*iter, ಪೂರ್ಣಾಂಕ idx) override;

      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಅಕ್ಷರ* vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಸಾಲೊಳಗೆ vpiHandle vpi_iterate(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override { ಹಿಂತಿರುಗಿ vpi_array_ತಳ_iterate(ಸಂಕೇತ_2); }
      vpiHandle vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ idx) override;

      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ idx, ಚಿಹ್ನೆರಹಿತ off, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val);
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ idx, ದ್ವಿಗುಣ val);
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ idx, ಸ್ಥಿರಾಂಕ std::string&val);
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ idx, ಸ್ಥಿರಾಂಕ vvp_object_t&val);

      vvp_vector4_t get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ ವಿಳಾಸ);
      ದ್ವಿಗುಣ get_ಶಬ್ದ_r(ಚಿಹ್ನೆರಹಿತ ವಿಳಾಸ);
      ಶೂನ್ಯ get_ಶಬ್ದ_obj(ಚಿಹ್ನೆರಹಿತ ವಿಳಾಸ, vvp_object_t&val);
      std::string get_ಶಬ್ದ_str(ಚಿಹ್ನೆರಹಿತ ವಿಳಾಸ);

      ಶೂನ್ಯ alias_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr, vpiHandle ಶಬ್ದ, ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb);
      ಶೂನ್ಯ attach_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ addr, vpiHandle ಶಬ್ದ);
      ಶೂನ್ಯ ಶಬ್ದ_change(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr);

      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name; /* Permanently allocated string */
      __vpiDecConst ಮೊದಲ_addr;
      __vpiDecConst ಕೊನೆಯ_addr;
      __vpiDecConst msb;
      __vpiDecConst lsb;
      ಚಿಹ್ನೆರಹಿತ vals_ಅಗಲ;
	// If this is a net array, nets lists the handles.
      vpiHandle*nets;
	// If this is a var array, then these are used instead of nets.
      vvp_vector4array_t*vals4;
      vvp_darray        *vals;

      vvp_fun_arrayport*ports_;
      ರಚನೆ __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool ಅದಲುಬದಲು_addr;

ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ array_ಎಣಿಕೆ;
      __vpiScope*scope;

ಮಿತ್ರ vpiHandle vpip_make_array(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name,
                                 ಪೂರ್ಣಾಂಕ ಮೊದಲ_addr, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_addr,
                                 bool signed_flag);
ಮಿತ್ರ ಶೂನ್ಯ compile_array_alias(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಅಕ್ಷರ*src);
};

class __vpiDarrayVar : ಸಾರ್ವಜನಿಕ __vpiBaseVar, ಸಾರ್ವಜನಿಕ __vpiArrayBase {
    ಸಾರ್ವಜನಿಕ:
      __vpiDarrayVar(__vpiScope*scope, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net);

      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiArrayVar; }
      ಚಿಹ್ನೆರಹಿತ get_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override;
      vpiHandle get_ಎಡ_range() override;
      vpiHandle get_ಬಲ_range() override;
      __vpiScope*get_scope() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ scope_; }

      ಪೂರ್ಣಾಂಕ get_ಶಬ್ದ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override;
      ಅಕ್ಷರ*get_ಶಬ್ದ_str(ರಚನೆ __vpiArrayWord*ಶಬ್ದ, ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ get_ಶಬ್ದ_ಮೌಲ್ಯ(ರಚನೆ __vpiArrayWord*ಶಬ್ದ, p_vpi_ಮೌಲ್ಯ vp) override;
      ಶೂನ್ಯ put_ಶಬ್ದ_ಮೌಲ್ಯ(ರಚನೆ __vpiArrayWord*ಶಬ್ದ, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ flags) override;

      vpiHandle get_iter_ಸೂಚ್ಯಂಕ(ರಚನೆ __vpiArrayIterator*iter, ಪೂರ್ಣಾಂಕ idx) override;
      ಸಾಲೊಳಗೆ vpiHandle vpi_iterate(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override { ಹಿಂತಿರುಗಿ vpi_array_ತಳ_iterate(ಸಂಕೇತ_2); }

      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಅಕ್ಷರ* vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) override;

      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

    ಸಂರಕ್ಷಿತ:
      vvp_darray*get_vvp_darray() ಸ್ಥಿರಾಂಕ;
      __vpiDecConst ಎಡ_range, ಬಲ_range;
};

ಬಾಹ್ಯ vpiHandle vpip_make_darray_var(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net);

class __vpiQueueVar : ಸಾರ್ವಜನಿಕ __vpiDarrayVar {

    ಸಾರ್ವಜನಿಕ:
      __vpiQueueVar(__vpiScope*scope, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net);

      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
};

ಬಾಹ್ಯ vpiHandle vpip_make_ಸರತಿ_var(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net);

class __vpiPropQueueRef : ಸಾರ್ವಜನಿಕ __vpiHandle {

    ಸಾರ್ವಜನಿಕ:
      explicit __vpiPropQueueRef(__vpiScope*scope, ಚಿಹ್ನೆರಹಿತ pidx, bool is_ಸರತಿ);

      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಅಕ್ಷರ* vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

      vvp_net_t* class_net_;
      ಚಿಹ್ನೆರಹಿತ prop_idx_;
      bool is_ಸರತಿ_2;

    ಖಾಸಗಿ:
      __vpiScope* scope_;
};

ಬಾಹ್ಯ vpiHandle vpip_make_prop_ಸರತಿ_ref(ಅಕ್ಷರ* class_label,
                                          ಚಿಹ್ನೆರಹಿತ prop_idx,
                                          ಚಿಹ್ನೆರಹಿತ is_ಸರತಿ_flag);
ಬಾಹ್ಯ vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : ಸಾರ್ವಜನಿಕ __vpiBaseVar {

    ಸಾರ್ವಜನಿಕ:
      __vpiCobjectVar(__vpiScope*scope, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net);

      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
};

ಬಾಹ್ಯ vpiHandle vpip_make_cobject_var(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net);

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
ರಚನೆ __vpiUserSystf : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiUserSystf();
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;

      s_vpi_systf_ದತ್ತಾಂಶ info;
      bool is_user_defn;
};

ಬಾಹ್ಯ vpiHandle vpip_make_systf_iterator(ಶೂನ್ಯ);

ಬಾಹ್ಯ ರಚನೆ __vpiUserSystf* vpip_ಕಂಡುಹಿಡಿ_systf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name);


ರಚನೆ __vpiSysTaskCall : ಸಾರ್ವಜನಿಕ __vpiHandle {

      __vpiScope* scope;
      ರಚನೆ __vpiUserSystf*defn;
      ಚಿಹ್ನೆರಹಿತ nargs;
      vpiHandle*ವಾದಗಳು;
	/* Stack consumed by this call */
      ಚಿಹ್ನೆರಹಿತ vec4_ರಾಶಿ;
      ಚಿಹ್ನೆರಹಿತ real_ರಾಶಿ;
      ಚಿಹ್ನೆರಹಿತ string_ರಾಶಿ;
	/* Support for vpi_get_userdata. */
      ಶೂನ್ಯ*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      ಚಿಹ್ನೆರಹಿತ file_idx;
      ಚಿಹ್ನೆರಹಿತ lineno;
      bool put_ಮೌಲ್ಯ;
    ಸಂರಕ್ಷಿತ:
      ಸಾಲೊಳಗೆ __vpiSysTaskCall()
      {
	    vec4_ರಾಶಿ = 0;
	    real_ರಾಶಿ = 0;
	    string_ರಾಶಿ = 0;
      }
};

ಬಾಹ್ಯ ರಚನೆ __vpiSysTaskCall*vpip_cur_task;

/*
 * The persistent flag to vpip_make_string_const causes the created
 * handle to be persistent. This is necessary for cases where the
 * string handle may be reused, which is the normal case.
 *
 * When constructing with a string, the class takes possession of the
 * text value string, and will delete it in the constructor.
 */

vpiHandle vpip_make_string_const(ಅಕ್ಷರ*ಪಠ್ಯ, bool persistent =true);
vpiHandle vpip_make_string_param(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಅಕ್ಷರ*ಮೌಲ್ಯ, bool local_flag,
                                 ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno);

ರಚನೆ __vpiBinaryConst : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiBinaryConst();
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

      vvp_vector4_t ಬಿಟ್ಟುಗಳು;
	/* TRUE if this constant is signed. */
      ಚಿಹ್ನೆರಹಿತ signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      ಚಿಹ್ನೆರಹಿತ sized_flag  :1;
};

vpiHandle vpip_make_ದ್ವಿಮಾನ_const(ಚಿಹ್ನೆರಹಿತ wid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಬಿಟ್ಟುಗಳು);
vpiHandle vpip_make_ದ್ವಿಮಾನ_param(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್ಟುಗಳು,
				 bool signed_flag, bool local_flag,
				 ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno);

class __vpiRealConst : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      explicit __vpiRealConst(ದ್ವಿಗುಣ);
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

      ದ್ವಿಗುಣ ಮೌಲ್ಯ;
};

vpiHandle vpip_make_real_const(ದ್ವಿಗುಣ ಮೌಲ್ಯ);
vpiHandle vpip_make_real_param(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, ದ್ವಿಗುಣ ಮೌಲ್ಯ, bool local_flag,
                               ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno);

class __vpiNullConst : ಸಾರ್ವಜನಿಕ __vpiHandle {
    ಸಾರ್ವಜನಿಕ:
      explicit __vpiNullConst();
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ ತಳ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*type);
vpiHandle vpip_make_vthr_str_ರಾಶಿ(ಚಿಹ್ನೆರಹಿತ ಆಳ);
vpiHandle vpip_make_vthr_vec4_ರಾಶಿ(ಚಿಹ್ನೆರಹಿತ ಆಳ, bool signed_flag, ಚಿಹ್ನೆರಹಿತ wid);

vpiHandle vpip_make_vthr_A(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ);
vpiHandle vpip_make_vthr_A(ಅಕ್ಷರ*label, ಅಕ್ಷರ*ಚಿಹ್ನೆ);
vpiHandle vpip_make_vthr_A(ಅಕ್ಷರ*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ, ಚಿಹ್ನೆರಹಿತ ಬಿಟ್, ಚಿಹ್ನೆರಹಿತ wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
ಬಾಹ್ಯ "C" ಶೂನ್ಯ vpip_ತುಂಬು_module(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name);

ಬಾಹ್ಯ "C" ಶೂನ್ಯ vpip_clear_module_paths();
ಬಾಹ್ಯ "C" ಶೂನ್ಯ vpip_ಕೂಡಿಸು_module_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);
ಬಾಹ್ಯ "C" ಶೂನ್ಯ vpip_ಕೂಡಿಸು_env_and_default_module_paths();

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
ಬಾಹ್ಯ vpiHandle vpip_build_vpi_call(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name,
				     ಪೂರ್ಣಾಂಕ val_type, ಚಿಹ್ನೆರಹಿತ return_ಅಗಲ,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     ಚಿಹ್ನೆರಹಿತ argc,
				     vpiHandle*argv,
				     ಚಿಹ್ನೆರಹಿತ vec4_ರಾಶಿ,
				     ಚಿಹ್ನೆರಹಿತ real_ರಾಶಿ,
				     ಚಿಹ್ನೆರಹಿತ string_ರಾಶಿ,
				     ದೀರ್ಘ file_idx,
				     ದೀರ್ಘ lineno);

ಬಾಹ್ಯ vthread_t vpip_current_vthread;

ಬಾಹ್ಯ ಶೂನ್ಯ vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_ಸಮಯ(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ vpip_get_ಸಮಯ_precision(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_ಗಣ_ಸಮಯ_precision(ಪೂರ್ಣಾಂಕ pres);

ಬಾಹ್ಯ __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ vpip_ಸಮಯ_units_from_handle(vpiHandle obj);
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ vpip_ಸಮಯ_precision_from_handle(vpiHandle obj);

ಬಾಹ್ಯ ದ್ವಿಗುಣ vpip_scaled_ಸಮಯ_from_handle(vvp_time64_t ಸಮಯ, vpiHandle obj);

ಬಾಹ್ಯ ಶೂನ್ಯ vpip_ಸಮಯ_ಗಮ್ಯ_timestruct(ರಚನೆ t_vpi_ಸಮಯ*ts, vvp_time64_t ti);
ಬಾಹ್ಯ vvp_time64_t vpip_timestruct_ಗಮ್ಯ_ಸಮಯ(ಸ್ಥಿರಾಂಕ ರಚನೆ t_vpi_ಸಮಯ*ts);

ಬಾಹ್ಯ ದ್ವಿಗುಣ vpip_ಸಮಯ_ಗಮ್ಯ_scaled_real(vvp_time64_t ti, ಸ್ಥಿರಾಂಕ __vpiScope*sc);
ಬಾಹ್ಯ vvp_time64_t vpip_scaled_real_ಗಮ್ಯ_time64(ದ್ವಿಗುಣ val, ಸ್ಥಿರಾಂಕ __vpiScope*sc);

/*
 * These functions are used mostly as compile time to strings into
 * permallocated memory. The vpip_string function is the most general,
 * it allocates a fresh string no matter what. The vpip_name_string
 * allocates a string and keeps a pointer in the hash, and tries to
 * reuse it if it can. This us useful for handle names, which may be
 * reused in different scopes.
 */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* vpip_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str);
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* vpip_name_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str);


/*
 * This function is used to make decimal string versions of various
 * vectors. The input format is a vvp_vector4_t, and the result is
 * written into buf, without overflowing nbuf.
 */
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ vpip_vec4_ಗಮ್ಯ_dec_str(ಸ್ಥಿರಾಂಕ vvp_vector4_t&vec4,
				     ಅಕ್ಷರ *buf, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ nbuf,
				     ಪೂರ್ಣಾಂಕ signed_flag);


ಬಾಹ್ಯ ಶೂನ್ಯ vpip_vec4_ಗಮ್ಯ_hex_str(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್ಟುಗಳು, ಅಕ್ಷರ*buf,
				 ಚಿಹ್ನೆರಹಿತ nbuf);

ಬಾಹ್ಯ ಶೂನ್ಯ vpip_vec4_ಗಮ್ಯ_oct_str(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್ಟುಗಳು, ಅಕ್ಷರ*buf,
				 ಚಿಹ್ನೆರಹಿತ nbuf);

ಬಾಹ್ಯ ಶೂನ್ಯ vpip_bin_str_ಗಮ್ಯ_vec4(vvp_vector4_t&val, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*buf);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_oct_str_ಗಮ್ಯ_vec4(vvp_vector4_t&val, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_dec_str_ಗಮ್ಯ_vec4(vvp_vector4_t&val, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_hex_str_ಗಮ್ಯ_vec4(vvp_vector4_t&val, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str);

ಬಾಹ್ಯ vvp_vector4_t vec4_from_vpi_ಮೌಲ್ಯ(s_vpi_ಮೌಲ್ಯ*vp, ಚಿಹ್ನೆರಹಿತ wid);
ಬಾಹ್ಯ ದ್ವಿಗುಣ real_from_vpi_ಮೌಲ್ಯ(s_vpi_ಮೌಲ್ಯ*vp);

ಬಾಹ್ಯ ಶೂನ್ಯ vpip_vec4_get_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಶಬ್ದ_val, ಚಿಹ್ನೆರಹಿತ ಅಗಲ,
				bool signed_flag, s_vpi_ಮೌಲ್ಯ*vp);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_vec2_get_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಶಬ್ದ_val, ಚಿಹ್ನೆರಹಿತ ಅಗಲ,
				bool signed_flag, s_vpi_ಮೌಲ್ಯ*vp);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_real_get_ಮೌಲ್ಯ(ದ್ವಿಗುಣ real, s_vpi_ಮೌಲ್ಯ*vp);
ಬಾಹ್ಯ ಶೂನ್ಯ vpip_string_get_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ std::string&val, s_vpi_ಮೌಲ್ಯ*vp);

/*
 * Function defined in vpi_signal.cc to manage vpi_get_* persistent
 * storage.
 */
ಎಣಿಕೆ vpi_rbuf_t {
      RBUF_VAL =0,
	/* Storage for *_get_value() */
      RBUF_STR,
	/* Storage for *_get_str() */
      RBUF_DEL
	/* Delete the storage for both buffers. */
};
ಬಾಹ್ಯ ಶೂನ್ಯ *need_ಫಲಿತಾಂಶ_buf(ಚಿಹ್ನೆರಹಿತ cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
ಬಾಹ್ಯ ಅಕ್ಷರ *simple_ಗಣ_rbuf_str(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s1);
ಬಾಹ್ಯ ಅಕ್ಷರ *generic_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2, vpiHandle ref, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸೂಚ್ಯಂಕ);

/* A routine to find the enclosing module. */
ಬಾಹ್ಯ vpiHandle vpip_module(__vpiScope*scope);

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
ಬಾಹ್ಯ bool vpip_mcd0_disable;

ಸಾಲೊಳಗೆ uint64_t vlg_ಸುತ್ತು_ಗಮ್ಯ_u64(ದ್ವಿಗುಣ rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      ಹಿಂತಿರುಗಿ static_cast<uint64_t>(static_cast<int64_t>(std::ಸುತ್ತು_2(rval)));
}

#ಆದರೆ_ಅಂತ್ಯ /* IVL_vpi_priv_H */
