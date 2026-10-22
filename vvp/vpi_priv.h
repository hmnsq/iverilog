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
#ଅନ୍ତର୍ଭାଵ  "delay.h"


class class_type;
class vvp_darray;
class vvp_fun_arrayport;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା __vpiArray* vvp_array_t;

/*
 * This header file contains the internal definitions that the vvp
 * program uses to implement the public interface in the vpi_user.h
 * header file elsewhere.
 */

#ଯଦି defined(__MINGW32__) || defined (__CYGWIN__)
ବାହ୍ୟ vpip_routines_s vpi_routines;
#endif

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#define vpi_File_ଧାଡ଼ି    0x1000003
#define _vpiDescription 0x1000004

ବାହ୍ୟ bool show_file_ଧାଡ଼ି;
ବାହ୍ୟ bool code_is_instrumented;

ବାହ୍ୟ vpiHandle vpip_build_file_ଧାଡ଼ି(ଅଚଳ ଅକ୍ଷର*description,
                                      long file_idx, long lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର* vpi_type_as_string(PLI_INT32 code);

/*
 * Return the port direction as a string.
 */
ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର* direction_as_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#ଯଦି defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
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
ଗଣନା vpi_mode_t {
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
ବାହ୍ୟ vpi_mode_t vpi_mode_flag;

/*
 * This structure is the very base of a vpiHandle. Every handle
 * structure is derived from this class so that the library can
 * internally pass the derived types as pointers to one of these.
 */
class __vpiHandle {
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ __vpiHandle() { }
	// The destructor is virtual so that dynamic types will work.
      ଆଭାସୀ ~__vpiHandle();

      ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ =0;
      ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code);
      ଆଭାସୀ int64_t vpi_get64(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code);
      ଆଭାସୀ ଅକ୍ଷର* vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code);

      ଆଭାସୀ ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val);
      ଆଭାସୀ vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
      ଆଭାସୀ vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code);
      ଆଭାସୀ vpiHandle vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code);
      ଆଭାସୀ vpiHandle vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
      ଆଭାସୀ ଶୂନ୍ୟ vpi_get_delays(p_vpi_delay del);
      ଆଭାସୀ ଶୂନ୍ୟ vpi_put_delays(p_vpi_delay del);

	// Objects may have destroyer functions of their own. If so,
	// then this virtual method will return a POINTER to that
	// function. The pointer is used to "delete" the object, which
	// is why the function itself cannot be a method.
      ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*free_object_fun_t)(vpiHandle);
      ଆଭାସୀ free_object_fun_t free_object_fun(ଶୂନ୍ୟ);
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
ସଂରଚନା __vpiIterator : ସର୍ବସାଧାରଣ __vpiHandle {
      __vpiIterator();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      free_object_fun_t free_object_fun(ଶୂନ୍ୟ) override;

      vpiHandle *args;
      unsigned  nargs;
      unsigned  next;
      bool free_args_flag;
};

ବାହ୍ୟ vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
				    bool free_args_flag);

class __vpiDecConst : ସର୍ବସାଧାରଣ __vpiHandle {
    ସର୍ବସାଧାରଣ:
      explicit __vpiDecConst(ପୂର୍ଣ୍ଣସଂଖ୍ୟା val =0);
      __vpiDecConst(ଅଚଳ __vpiDecConst&that);
# ଯଦି __cplusplus >= 201103L
      __vpiDecConst& operator = (ଅଚଳ __vpiDecConst&that) = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
# endif
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;

    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_ମୂଲ୍ୟ() ଅଚଳ { ଫେରନ୍ତୁ ମୂଲ୍ୟ; }
      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ set_ମୂଲ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା val) { ମୂଲ୍ୟ = val; }

    ବ୍ୟକ୍ତିଗତ:
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
ସଂରଚନା __vpiCallback : ସର୍ବସାଧାରଣ __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;

	// Used for listing callbacks.
      ସଂରଚନା __vpiCallback*next;

	// user supplied callback data
      ସଂରଚନା t_cb_data cb_data;
};

class ମୂଲ୍ୟ_callback : ସର୍ବସାଧାରଣ __vpiCallback {
    ସର୍ବସାଧାରଣ:
      explicit ମୂଲ୍ୟ_callback(p_cb_data data);
	// Return true if the callback really is ready to be called
      ଆଭାସୀ bool test_ମୂଲ୍ୟ_callback_ready(ଶୂନ୍ୟ);

    ସର୍ବସାଧାରଣ:
	// user supplied callback data
      ସଂରଚନା t_vpi_time cb_time;
      ସଂରଚନା t_vpi_ମୂଲ୍ୟ cb_ମୂଲ୍ୟ;
};

ବାହ୍ୟ ଶୂନ୍ୟ callback_execute(ସଂରଚନା __vpiCallback*cur);

ସଂରଚନା __vpiSystemTime : ସର୍ବସାଧାରଣ __vpiHandle {
      __vpiSystemTime();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର*vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;

      __vpiScope*scope;
};

ସଂରଚନା __vpiScopedTime : ସର୍ବସାଧାରଣ __vpiSystemTime {
      __vpiScopedTime();
      ଅକ୍ଷର*vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
};
ସଂରଚନା __vpiScopedSTime : ସର୍ବସାଧାରଣ __vpiSystemTime {
      __vpiScopedSTime();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର*vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
};
ସଂରଚନା __vpiScopedRealtime : ସର୍ବସାଧାରଣ __vpiSystemTime {
      __vpiScopedRealtime();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର*vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : ସର୍ବସାଧାରଣ __vpiHandle {

    ସର୍ବସାଧାରଣ:
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର* vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;

    ସର୍ବସାଧାରଣ:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର*scope_name() ଅଚଳ { ଫେରନ୍ତୁ name_; }

      ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର*scope_def_name() ଅଚଳ { ଫେରନ୍ତୁ tname_; }
	// TRUE if this is an automatic func/task/block
      ଧାଡ଼ିଭିତରେ bool is_automatic() ଅଚଳ { ଫେରନ୍ତୁ is_automatic_; }

    ସର୍ବସାଧାରଣ:
      __vpiScope *scope;
      unsigned file_idx;
      unsigned lineno;
      unsigned def_file_idx;
      unsigned def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_time;
      ସଂରଚନା __vpiScopedSTime scoped_stime;
      ସଂରଚନା __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::vector<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      ସଂରଚନା automatic_hooks_s**item;
      unsigned nitem;
        /* Keep a list of live contexts. */
      vvp_context_t live_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::set<vthread_t> threads;
      signed ପୂର୍ଣ୍ଣସଂଖ୍ୟା time_units :8;
      signed ପୂର୍ଣ୍ଣସଂଖ୍ୟା time_precision :8;

    ସୁରକ୍ଷିତ:
      __vpiScope(ଅଚଳ ଅକ୍ଷର*nam, ଅଚଳ ଅକ୍ଷର*tnam, bool is_auto_flag =false);

    ବ୍ୟକ୍ତିଗତ:
	/* The scope has a name. */
      ଅଚଳ ଅକ୍ଷର*name_;
      ଅଚଳ ଅକ୍ଷର*tname_;
	/* the scope may be "automatic" */
      bool is_automatic_;
};

class vpiScopeFunction  : ସର୍ବସାଧାରଣ __vpiScope {
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ vpiScopeFunction(ଅଚଳ ଅକ୍ଷର*nam, ଅଚଳ ଅକ୍ଷର*tnam,
			      bool auto_flag, ପୂର୍ଣ୍ଣସଂଖ୍ୟା func_type, unsigned func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiFunction; }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override
      {
	    ବାଛ (code) {
		ସ୍ଥିତି vpiFuncType:
		  ଫେରନ୍ତୁ func_type_;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  ଫେରନ୍ତୁ __vpiScope::vpi_get(code);
	    }
      }

    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ unsigned get_func_width(ଶୂନ୍ୟ) ଅଚଳ { ଫେରନ୍ତୁ func_wid_; }
      ଧାଡ଼ିଭିତରେ vvp_bit4_t get_func_init_val(ଶୂନ୍ୟ) ଅଚଳ { ଫେରନ୍ତୁ func_init_val_; }

    ବ୍ୟକ୍ତିଗତ:
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା func_type_;
      unsigned func_wid_;
      vvp_bit4_t func_init_val_;
};

ବାହ୍ୟ __vpiScope* vpip_peek_current_scope(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj);
ବାହ୍ୟ ଶୂନ୍ୟ vpip_attach_to_current_scope(vpiHandle obj);
ବାହ୍ୟ __vpiScope* vpip_peek_context_scope(ଶୂନ୍ୟ);
ବାହ୍ୟ unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                         __vpiScope*scope);
ବାହ୍ୟ vpiHandle vpip_make_ମୂଳ_iterator(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type_code);
ବାହ୍ୟ ଶୂନ୍ୟ vpip_make_ମୂଳ_iterator(class __vpiHandle**&table,
				    unsigned&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
ସଂରଚନା __vpiSignal : ସର୍ବସାଧାରଣ __vpiHandle {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      int64_t vpi_get64(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର* vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
      vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx) override;

    ସର୍ବସାଧାରଣ:
      unsigned width() ଅଚଳ;
      vpiHandle get_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index);
      ଶୂନ୍ୟ get_bit_ମୂଲ୍ୟ(ଅଚଳ ସଂରଚନା __vpiBit*bit, p_vpi_ମୂଲ୍ୟ vp);
      vpiHandle put_bit_ମୂଲ୍ୟ(ସଂରଚନା __vpiBit*bit, p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
      ଶୂନ୍ୟ make_bits();

      ସଂରଚନା __vpiBit*bits;

    ସର୍ବସାଧାରଣ:
      ସଂଘ { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
      ସଂଘ { // The name of this reg/net, or the index for array words.
            ଅଚଳ ଅକ୍ଷର*name;
            vpiHandle index;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      unsigned signed_flag  : 1;
      unsigned is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*ଗଣ୍ଠି;

    ସର୍ବସାଧାରଣ:
      ସ୍ଥିର ଶୂନ୍ୟ*operator new(std::size_t size);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*); // not implemented
    ସୁରକ୍ଷିତ:
      ଧାଡ଼ିଭିତରେ __vpiSignal() : bits(ଖାଲି) { }
    ବ୍ୟକ୍ତିଗତ: // Not implemented
      ସ୍ଥିର ଶୂନ୍ୟ*operator new[] (std::size_t size);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete[](ଶୂନ୍ୟ*);
};
ବାହ୍ୟ unsigned vpip_size(ଅଚଳ __vpiSignal *sig);
ବାହ୍ୟ __vpiScope* vpip_scope(__vpiSignal*sig);

ବାହ୍ୟ vpiHandle vpip_make_int2(ଅଚଳ ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb,
			       bool signed_flag, vvp_net_t*vec);
ବାହ୍ୟ vpiHandle vpip_make_int4(ଅଚଳ ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb,
			       vvp_net_t*vec);
ବାହ୍ୟ vpiHandle vpip_make_var4(ଅଚଳ ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb,
			       bool signed_flag, vvp_net_t*net);
ବାହ୍ୟ vpiHandle vpip_make_net4(__vpiScope*scope,
				ଅଚଳ ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb,
				bool signed_flag, vvp_net_t*ଗଣ୍ଠି);

/*
 * This is used to represent a bit in a net/reg.
 */
ସଂରଚନା __vpiBit {
      ସଂରଚନା as_bit_t : ସର୍ବସାଧାରଣ __vpiHandle {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code);
	    ଅକ୍ଷର* vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code);
	    ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val);
	    vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
	    vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code);
      } as_bit;

      vpiHandle index;

      ସଂଘ {
	    ସଂରଚନା __vpiSignal*parent;
	    ସଂରଚନା __vpiBit*bit0;
      };

      ଧାଡ଼ିଭିତରେ unsigned get_norm_index() ଅଚଳ { ଫେରନ୍ତୁ this - bit0; }
      ଧାଡ଼ିଭିତରେ ସଂରଚନା __vpiSignal*get_parent() ଅଚଳ {ଫେରନ୍ତୁ (bit0 - 1)->parent; }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_index(ଶୂନ୍ୟ) ଅଚଳ;
};

class vpiPortBitInfo;

class vpiPortInfo  : ସର୍ବସାଧାରଣ __vpiHandle {
    ସର୍ବସାଧାରଣ:
      vpiPortInfo( __vpiScope *parent,
                    unsigned index,
                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_direction,
                    unsigned width,
                    ଅଚଳ ଅକ୍ଷର *name,
                    ଅକ୍ଷର* buffer );
      ~vpiPortInfo() override;

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiPort; }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_direction(ଶୂନ୍ୟ) { ଫେରନ୍ତୁ direction_; }
      unsigned get_index(ଶୂନ୍ୟ) { ଫେରନ୍ତୁ index_; }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_width(ଶୂନ୍ୟ) { ଫେରନ୍ତୁ width_; }
      ଶୂନ୍ୟ add_port_bit(vpiPortBitInfo* port_bit) { port_bits_.ଠେଲ_back(port_bit); }

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର* vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vvp_net_t* get_port(ଶୂନ୍ୟ) ଅଚଳ { ଫେରନ୍ତୁ ref_; }
      vpiHandle vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;

      std::vector<vpiPortBitInfo*> port_bits_;

    ବ୍ୟକ୍ତିଗତ:
      __vpiScope *parent_;
      unsigned  index_;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା       direction_;
      unsigned  width_;
      ଅଚଳ ଅକ୍ଷର *name_;
      vvp_net_t *ref_;
};

class vpiPortBitInfo  : ସର୍ବସାଧାରଣ __vpiHandle {
    ସର୍ବସାଧାରଣ:
      vpiPortBitInfo(vpiPortInfo *parent,
                     unsigned bit);
      ~vpiPortBitInfo() override;

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiPortBit; }
      unsigned get_bit(ଶୂନ୍ୟ) ଅଚଳ { ଫେରନ୍ତୁ bit_; }

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;

    ବ୍ୟକ୍ତିଗତ:
      vpiPortInfo *parent_;
      unsigned bit_;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
ସଂରଚନା __vpiPV : ସର୍ବସାଧାରଣ __vpiHandle {
      __vpiPV();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର* vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
      vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;

      vpiHandle parent;
      vvp_net_t*net;
      vpiHandle sbase;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା tbase;
      unsigned width;
};
ବାହ୍ୟ vpiHandle vpip_make_PV(ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width);
ବାହ୍ୟ vpiHandle vpip_make_PV(ଅକ୍ଷର*name, ଅକ୍ଷର*ପ୍ରତୀକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width);
ବାହ୍ୟ vpiHandle vpip_make_PV(ଅକ୍ଷର*name, vpiHandle handle, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width);

ସଂରଚନା __vpiModPathTerm : ସର୍ବସାଧାରଣ __vpiHandle {
      __vpiModPathTerm();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;

      vpiHandle ପଦସମଷ୍ଟି;
	/* The value returned by vpi_get(vpiEdge, ...); */
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା edge;
};

ସଂରଚନା __vpiModPathSrc : ସର୍ବସାଧାରଣ __vpiHandle {
      __vpiModPathSrc();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
      vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx) override;
      ଶୂନ୍ୟ vpi_get_delays(p_vpi_delay del) override;
      ଶୂନ୍ୟ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(ଶୂନ୍ୟ) override;

      ସଂରଚନା __vpiModPath *dest;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା   type;

	/* This is the input expression for this modpath. */
      ସଂରଚନା __vpiModPathTerm path_term_in;

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

ସଂରଚନା __vpiModPath {
      __vpiScope   *scope ;

      class vvp_fun_modpath*modpath;

      ସଂରଚନା __vpiModPathTerm path_term_out;
      vvp_net_t *ନିବେଶ_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

ବାହ୍ୟ ସଂରଚନା __vpiModPathSrc* vpip_make_modpath_src(ସଂରଚନା __vpiModPath*path,
                                                     vvp_net_t *net) ;

ବାହ୍ୟ ସଂରଚନା __vpiModPath* vpip_make_modpath(vvp_net_t *net) ;

/*
 *
 * The vpiInterModPath vpiHandle will define
 * a vpiInterModPath of record .intermodpath as defined
 * in the IEEE 1364
 *
 */

ସଂରଚନା __vpiInterModPath : ସର୍ବସାଧାରଣ __vpiHandle {
      __vpiInterModPath();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
      vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_delays(p_vpi_delay del) override;
      ଶୂନ୍ୟ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(ଶୂନ୍ୟ) override;

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

ବାହ୍ୟ ସଂରଚନା __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2);

/*
 * These methods support the vpi creation of events. The name string
 * passed in will be saved, so the caller must allocate it (or not
 * free it) after it is handed to this function.
 */
class __vpiNamedEvent : ସର୍ବସାଧାରଣ __vpiHandle {

    ସର୍ବସାଧାରଣ:
      __vpiNamedEvent(__vpiScope*scope, ଅଚଳ ଅକ୍ଷର*name);
      ~__vpiNamedEvent() override;

      __vpiNamedEvent(ଅଚଳ __vpiNamedEvent&) = delete;
      __vpiNamedEvent& operator=(ଅଚଳ __vpiNamedEvent&) = delete;

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      __vpiScope*get_scope(ଶୂନ୍ୟ) ଅଚଳ { ଫେରନ୍ତୁ scope_; }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର* vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;

      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ add_vpi_callback(__vpiCallback*cb)
      { cb->next = callbacks_;
	callbacks_ = cb;
      }

      ଶୂନ୍ୟ run_vpi_callbacks(ଶୂନ୍ୟ);

	/* The functor, used for %set operations. */
      vvp_net_t*funct;

    ବ୍ୟକ୍ତିଗତ:
	/* base name of the event object */
      ଅଚଳ ଅକ୍ଷର*name_;
	/* Parent scope of this object. */
      __vpiScope*scope_;
	/* List of callbacks interested in this event. */
      __vpiCallback*callbacks_;
};

ବାହ୍ୟ vpiHandle vpip_make_named_event(ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*f);

/*
 * Memory is an array of bits that is accessible in N-bit chunks, with
 * N being the width of a word. The memory word handle just points
 * back to the memory and uses an index to identify its position in
 * the memory.
 */

ବାହ୍ୟ bool is_net_array(vpiHandle obj);

/*
 * These are the various variable types.
 */
ସଂରଚନା __vpiRealVar : ସର୍ବସାଧାରଣ __vpiHandle {
      __vpiRealVar();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର* vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
      vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;

      ସଂଘ { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      ସଂଘ {
            ଅଚଳ ଅକ୍ଷର*name;
            vpiHandle index;
      } id;
      unsigned is_netarray  : 1; // This is word of a net array
      unsigned is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

ବାହ୍ୟ __vpiScope* vpip_scope(__vpiRealVar*sig);
ବାହ୍ୟ vpiHandle vpip_make_real_var(ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net);
ବାହ୍ୟ vpiHandle vpip_make_real_net(__vpiScope*scope,
				    ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net);

class __vpiBaseVar : ସର୍ବସାଧାରଣ __vpiHandle {

    ସର୍ବସାଧାରଣ:
      __vpiBaseVar(__vpiScope*scope, ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net);
#ifdef CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#endif

      ଧାଡ଼ିଭିତରେ vvp_net_t* get_net() ଅଚଳ { ଫେରନ୍ତୁ net_; }

    ସୁରକ୍ଷିତ:
      __vpiScope* scope_;
      ଅଚଳ ଅକ୍ଷର*name_;

    ବ୍ୟକ୍ତିଗତ:
      vvp_net_t*net_;
};

class __vpiStringVar : ସର୍ବସାଧାରଣ __vpiBaseVar {

    ସର୍ବସାଧାରଣ:
      __vpiStringVar(__vpiScope*scope, ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net);

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
      vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;
};

ବାହ୍ୟ vpiHandle vpip_make_string_var(ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net);

ସଂରଚନା __vpiArrayBase {
      __vpiArrayBase() : vals_ଶବ୍ଦଗୁଡ଼ିକ(ଖାଲି) {}
      ଆଭାସୀ ~__vpiArrayBase() {}

      ଆଭାସୀ unsigned get_size(ଶୂନ୍ୟ) ଅଚଳ = 0;
      ଆଭାସୀ vpiHandle get_ବାମ_range() = 0;
      ଆଭାସୀ vpiHandle get_ଡାହାଣ_range() = 0;
      ଆଭାସୀ __vpiScope*get_scope() ଅଚଳ = 0;

      ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_word_size() ଅଚଳ = 0;
      ଆଭାସୀ ଅକ୍ଷର*get_word_str(ସଂରଚନା __vpiArrayWord*word, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) = 0;
      ଆଭାସୀ ଶୂନ୍ୟ get_word_ମୂଲ୍ୟ(ସଂରଚନା __vpiArrayWord*word, p_vpi_ମୂଲ୍ୟ vp) = 0;
      ଆଭାସୀ ଶୂନ୍ୟ put_word_ମୂଲ୍ୟ(ସଂରଚନା __vpiArrayWord*word, p_vpi_ମୂଲ୍ୟ vp,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) = 0;
      ଆଭାସୀ vpiHandle get_iter_index(ସଂରଚନା __vpiArrayIterator*iter, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_base_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code);

      ଆଭାସୀ ଶୂନ୍ୟ make_vals_ଶବ୍ଦଗୁଡ଼ିକ();

      ସଂରଚନା __vpiArrayWord*vals_ଶବ୍ଦଗୁଡ଼ିକ;
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
ସଂରଚନା __vpiArray : ସର୍ବସାଧାରଣ __vpiArrayBase, ସର୍ବସାଧାରଣ __vpiHandle {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiMemory; }
      unsigned get_size() ଅଚଳ override { ଫେରନ୍ତୁ array_ଗଣନା; }
      vpiHandle get_ବାମ_range() override { ପ୍ରତିଜ୍ଞା(nets == 0); ଫେରନ୍ତୁ &msb; }
      vpiHandle get_ଡାହାଣ_range() override { ପ୍ରତିଜ୍ଞା(nets == 0); ଫେରନ୍ତୁ &lsb; }
      __vpiScope*get_scope() ଅଚଳ override { ଫେରନ୍ତୁ scope; }

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_word_size() ଅଚଳ override;
      ଅକ୍ଷର*get_word_str(ସଂରଚନା __vpiArrayWord*word, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ get_word_ମୂଲ୍ୟ(ସଂରଚନା __vpiArrayWord*word, p_vpi_ମୂଲ୍ୟ vp) override;
      ଶୂନ୍ୟ put_word_ମୂଲ୍ୟ(ସଂରଚନା __vpiArrayWord*word, p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;

      vpiHandle get_iter_index(ସଂରଚନା __vpiArrayIterator*iter, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx) override;

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର* vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଧାଡ଼ିଭିତରେ vpiHandle vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override { ଫେରନ୍ତୁ vpi_array_base_iterate(code); }
      vpiHandle vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx) override;

      ଶୂନ୍ୟ set_word(unsigned idx, unsigned off, ଅଚଳ vvp_vector4_t&val);
      ଶୂନ୍ୟ set_word(unsigned idx, ଦ୍ୱିଗୁଣ val);
      ଶୂନ୍ୟ set_word(unsigned idx, ଅଚଳ std::string&val);
      ଶୂନ୍ୟ set_word(unsigned idx, ଅଚଳ vvp_object_t&val);

      vvp_vector4_t get_word(unsigned address);
      ଦ୍ୱିଗୁଣ get_word_r(unsigned address);
      ଶୂନ୍ୟ get_word_obj(unsigned address, vvp_object_t&val);
      std::string get_word_str(unsigned address);

      ଶୂନ୍ୟ alias_word(unsigned long addr, vpiHandle word, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb);
      ଶୂନ୍ୟ attach_word(unsigned addr, vpiHandle word);
      ଶୂନ୍ୟ word_change(unsigned long addr);

      ଅଚଳ ଅକ୍ଷର*name; /* Permanently allocated string */
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
      ସଂରଚନା __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool swap_addr;

ବ୍ୟକ୍ତିଗତ:
      unsigned array_ଗଣନା;
      __vpiScope*scope;

friend vpiHandle vpip_make_array(ଅଚଳ ଅକ୍ଷର*label, ଅଚଳ ଅକ୍ଷର*name,
                                 ପୂର୍ଣ୍ଣସଂଖ୍ୟା first_addr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_addr,
                                 bool signed_flag);
friend ଶୂନ୍ୟ compile_array_alias(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ଅକ୍ଷର*src);
};

class __vpiDarrayVar : ସର୍ବସାଧାରଣ __vpiBaseVar, ସର୍ବସାଧାରଣ __vpiArrayBase {
    ସର୍ବସାଧାରଣ:
      __vpiDarrayVar(__vpiScope*scope, ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net);

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code() ଅଚଳ override { ଫେରନ୍ତୁ vpiArrayVar; }
      unsigned get_size() ଅଚଳ override;
      vpiHandle get_ବାମ_range() override;
      vpiHandle get_ଡାହାଣ_range() override;
      __vpiScope*get_scope() ଅଚଳ override { ଫେରନ୍ତୁ scope_; }

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_word_size() ଅଚଳ override;
      ଅକ୍ଷର*get_word_str(ସଂରଚନା __vpiArrayWord*word, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ get_word_ମୂଲ୍ୟ(ସଂରଚନା __vpiArrayWord*word, p_vpi_ମୂଲ୍ୟ vp) override;
      ଶୂନ୍ୟ put_word_ମୂଲ୍ୟ(ସଂରଚନା __vpiArrayWord*word, p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;

      vpiHandle get_iter_index(ସଂରଚନା __vpiArrayIterator*iter, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx) override;
      ଧାଡ଼ିଭିତରେ vpiHandle vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override { ଫେରନ୍ତୁ vpi_array_base_iterate(code); }

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର* vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) override;

      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;

    ସୁରକ୍ଷିତ:
      vvp_darray*get_vvp_darray() ଅଚଳ;
      __vpiDecConst ବାମ_range, ଡାହାଣ_range;
};

ବାହ୍ୟ vpiHandle vpip_make_darray_var(ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net);

class __vpiQueueVar : ସର୍ବସାଧାରଣ __vpiDarrayVar {

    ସର୍ବସାଧାରଣ:
      __vpiQueueVar(__vpiScope*scope, ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net);

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
};

ବାହ୍ୟ vpiHandle vpip_make_queue_var(ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net);

class __vpiPropQueueRef : ସର୍ବସାଧାରଣ __vpiHandle {

    ସର୍ବସାଧାରଣ:
      explicit __vpiPropQueueRef(__vpiScope*scope, unsigned pidx, bool is_queue);

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର* vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;

      vvp_net_t* class_net_;
      unsigned prop_idx_;
      bool is_queue_;

    ବ୍ୟକ୍ତିଗତ:
      __vpiScope* scope_;
};

ବାହ୍ୟ vpiHandle vpip_make_prop_queue_ref(ଅକ୍ଷର* class_label,
                                          unsigned prop_idx,
                                          unsigned is_queue_flag);
ବାହ୍ୟ vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : ସର୍ବସାଧାରଣ __vpiBaseVar {

    ସର୍ବସାଧାରଣ:
      __vpiCobjectVar(__vpiScope*scope, ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net);

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
};

ବାହ୍ୟ vpiHandle vpip_make_cobject_var(ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net);

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
ସଂରଚନା __vpiUserSystf : ସର୍ବସାଧାରଣ __vpiHandle {
      __vpiUserSystf();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;

      s_vpi_systf_data info;
      bool is_user_defn;
};

ବାହ୍ୟ vpiHandle vpip_make_systf_iterator(ଶୂନ୍ୟ);

ବାହ୍ୟ ସଂରଚନା __vpiUserSystf* vpip_find_systf(ଅଚଳ ଅକ୍ଷର*name);


ସଂରଚନା __vpiSysTaskCall : ସର୍ବସାଧାରଣ __vpiHandle {

      __vpiScope* scope;
      ସଂରଚନା __vpiUserSystf*defn;
      unsigned nargs;
      vpiHandle*args;
	/* Stack consumed by this call */
      unsigned vec4_ଗଦା;
      unsigned real_ଗଦା;
      unsigned string_ଗଦା;
	/* Support for vpi_get_userdata. */
      ଶୂନ୍ୟ*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      unsigned file_idx;
      unsigned lineno;
      bool put_ମୂଲ୍ୟ;
    ସୁରକ୍ଷିତ:
      ଧାଡ଼ିଭିତରେ __vpiSysTaskCall()
      {
	    vec4_ଗଦା = 0;
	    real_ଗଦା = 0;
	    string_ଗଦା = 0;
      }
};

ବାହ୍ୟ ସଂରଚନା __vpiSysTaskCall*vpip_cur_task;

/*
 * The persistent flag to vpip_make_string_const causes the created
 * handle to be persistent. This is necessary for cases where the
 * string handle may be reused, which is the normal case.
 *
 * When constructing with a string, the class takes possession of the
 * text value string, and will delete it in the constructor.
 */

vpiHandle vpip_make_string_const(ଅକ୍ଷର*text, bool persistent =true);
vpiHandle vpip_make_string_param(ଅଚଳ ଅକ୍ଷର*name, ଅକ୍ଷର*ମୂଲ୍ୟ, bool local_flag,
                                 long file_idx, long lineno);

ସଂରଚନା __vpiBinaryConst : ସର୍ବସାଧାରଣ __vpiHandle {
      __vpiBinaryConst();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;

      vvp_vector4_t bits;
	/* TRUE if this constant is signed. */
      unsigned signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      unsigned sized_flag  :1;
};

vpiHandle vpip_make_ଦ୍ୱିମିକ_const(unsigned wid, ଅଚଳ ଅକ୍ଷର*bits);
vpiHandle vpip_make_ଦ୍ୱିମିକ_param(ଅଚଳ ଅକ୍ଷର*name, ଅଚଳ vvp_vector4_t&bits,
				 bool signed_flag, bool local_flag,
				 long file_idx, long lineno);

class __vpiRealConst : ସର୍ବସାଧାରଣ __vpiHandle {
    ସର୍ବସାଧାରଣ:
      explicit __vpiRealConst(ଦ୍ୱିଗୁଣ);
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;

      ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ;
};

vpiHandle vpip_make_real_const(ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ);
vpiHandle vpip_make_real_param(ଅଚଳ ଅକ୍ଷର*name, ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ, bool local_flag,
                               long file_idx, long lineno);

class __vpiNullConst : ସର୍ବସାଧାରଣ __vpiHandle {
    ସର୍ବସାଧାରଣ:
      explicit __vpiNullConst();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_word(unsigned base, ଅଚଳ ଅକ୍ଷର*type);
vpiHandle vpip_make_vthr_str_ଗଦା(unsigned depth);
vpiHandle vpip_make_vthr_vec4_ଗଦା(unsigned depth, bool signed_flag, unsigned wid);

vpiHandle vpip_make_vthr_A(ଅକ୍ଷର*label, unsigned index);
vpiHandle vpip_make_vthr_A(ଅକ୍ଷର*label, ଅକ୍ଷର*ପ୍ରତୀକ);
vpiHandle vpip_make_vthr_A(ଅକ୍ଷର*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(ଅକ୍ଷର*label, unsigned index, unsigned bit, unsigned wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
ବାହ୍ୟ "C" ଶୂନ୍ୟ vpip_load_module(ଅଚଳ ଅକ୍ଷର*name);

ବାହ୍ୟ "C" ଶୂନ୍ୟ vpip_clear_module_paths();
ବାହ୍ୟ "C" ଶୂନ୍ୟ vpip_add_module_path(ଅଚଳ ଅକ୍ଷର *path);
ବାହ୍ୟ "C" ଶୂନ୍ୟ vpip_add_env_and_default_module_paths();

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
ବାହ୍ୟ vpiHandle vpip_build_vpi_call(ଅଚଳ ଅକ୍ଷର*name,
				     ପୂର୍ଣ୍ଣସଂଖ୍ୟା val_type, unsigned return_width,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     unsigned argc,
				     vpiHandle*argv,
				     unsigned vec4_ଗଦା,
				     unsigned real_ଗଦା,
				     unsigned string_ଗଦା,
				     long file_idx,
				     long lineno);

ବାହ୍ୟ vthread_t vpip_current_vthread;

ବାହ୍ୟ ଶୂନ୍ୟ vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_time(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpip_get_time_precision(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ vpip_set_time_precision(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pres);

ବାହ୍ୟ __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpip_time_units_from_handle(vpiHandle obj);
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpip_time_precision_from_handle(vpiHandle obj);

ବାହ୍ୟ ଦ୍ୱିଗୁଣ vpip_scaled_time_from_handle(vvp_time64_t time, vpiHandle obj);

ବାହ୍ୟ ଶୂନ୍ୟ vpip_time_to_timestruct(ସଂରଚନା t_vpi_time*ts, vvp_time64_t ti);
ବାହ୍ୟ vvp_time64_t vpip_timestruct_to_time(ଅଚଳ ସଂରଚନା t_vpi_time*ts);

ବାହ୍ୟ ଦ୍ୱିଗୁଣ vpip_time_to_scaled_real(vvp_time64_t ti, ଅଚଳ __vpiScope*sc);
ବାହ୍ୟ vvp_time64_t vpip_scaled_real_to_time64(ଦ୍ୱିଗୁଣ val, ଅଚଳ __vpiScope*sc);

/*
 * These functions are used mostly as compile time to strings into
 * permallocated memory. The vpip_string function is the most general,
 * it allocates a fresh string no matter what. The vpip_name_string
 * allocates a string and keeps a pointer in the hash, and tries to
 * reuse it if it can. This us useful for handle names, which may be
 * reused in different scopes.
 */
ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର* vpip_string(ଅଚଳ ଅକ୍ଷର*str);
ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର* vpip_name_string(ଅଚଳ ଅକ୍ଷର*str);


/*
 * This function is used to make decimal string versions of various
 * vectors. The input format is a vvp_vector4_t, and the result is
 * written into buf, without overflowing nbuf.
 */
ବାହ୍ୟ unsigned vpip_vec4_to_dec_str(ଅଚଳ vvp_vector4_t&vec4,
				     ଅକ୍ଷର *buf, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା nbuf,
				     ପୂର୍ଣ୍ଣସଂଖ୍ୟା signed_flag);


ବାହ୍ୟ ଶୂନ୍ୟ vpip_vec4_to_hex_str(ଅଚଳ vvp_vector4_t&bits, ଅକ୍ଷର*buf,
				 unsigned nbuf);

ବାହ୍ୟ ଶୂନ୍ୟ vpip_vec4_to_oct_str(ଅଚଳ vvp_vector4_t&bits, ଅକ୍ଷର*buf,
				 unsigned nbuf);

ବାହ୍ୟ ଶୂନ୍ୟ vpip_bin_str_to_vec4(vvp_vector4_t&val, ଅଚଳ ଅକ୍ଷର*buf);
ବାହ୍ୟ ଶୂନ୍ୟ vpip_oct_str_to_vec4(vvp_vector4_t&val, ଅଚଳ ଅକ୍ଷର*str);
ବାହ୍ୟ ଶୂନ୍ୟ vpip_dec_str_to_vec4(vvp_vector4_t&val, ଅଚଳ ଅକ୍ଷର*str);
ବାହ୍ୟ ଶୂନ୍ୟ vpip_hex_str_to_vec4(vvp_vector4_t&val, ଅଚଳ ଅକ୍ଷର*str);

ବାହ୍ୟ vvp_vector4_t vec4_from_vpi_ମୂଲ୍ୟ(s_vpi_ମୂଲ୍ୟ*vp, unsigned wid);
ବାହ୍ୟ ଦ୍ୱିଗୁଣ real_from_vpi_ମୂଲ୍ୟ(s_vpi_ମୂଲ୍ୟ*vp);

ବାହ୍ୟ ଶୂନ୍ୟ vpip_vec4_get_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&word_val, unsigned width,
				bool signed_flag, s_vpi_ମୂଲ୍ୟ*vp);
ବାହ୍ୟ ଶୂନ୍ୟ vpip_vec2_get_ମୂଲ୍ୟ(ଅଚଳ vvp_vector2_t&word_val, unsigned width,
				bool signed_flag, s_vpi_ମୂଲ୍ୟ*vp);
ବାହ୍ୟ ଶୂନ୍ୟ vpip_real_get_ମୂଲ୍ୟ(ଦ୍ୱିଗୁଣ real, s_vpi_ମୂଲ୍ୟ*vp);
ବାହ୍ୟ ଶୂନ୍ୟ vpip_string_get_ମୂଲ୍ୟ(ଅଚଳ std::string&val, s_vpi_ମୂଲ୍ୟ*vp);

/*
 * Function defined in vpi_signal.cc to manage vpi_get_* persistent
 * storage.
 */
ଗଣନା vpi_rbuf_t {
      RBUF_VAL =0,
	/* Storage for *_get_value() */
      RBUF_STR,
	/* Storage for *_get_str() */
      RBUF_DEL
	/* Delete the storage for both buffers. */
};
ବାହ୍ୟ ଶୂନ୍ୟ *need_result_buf(unsigned cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
ବାହ୍ୟ ଅକ୍ଷର *simple_set_rbuf_str(ଅଚଳ ଅକ୍ଷର *s1);
ବାହ୍ୟ ଅକ୍ଷର *generic_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref, ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *index);

/* A routine to find the enclosing module. */
ବାହ୍ୟ vpiHandle vpip_module(__vpiScope*scope);

ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
ବାହ୍ୟ bool vpip_mcd0_disable;

ଧାଡ଼ିଭିତରେ uint64_t vlg_round_to_u64(ଦ୍ୱିଗୁଣ rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      ଫେରନ୍ତୁ static_cast<uint64_t>(static_cast<int64_t>(std::round(rval)));
}

#endif /* IVL_vpi_priv_H */
