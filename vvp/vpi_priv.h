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
#অন্তর্ভাব  "delay.h"


class class_type;
class vvp_darray;
class vvp_fun_arrayport;

প্রকারনির্দেশ সংরচনা __vpiArray* vvp_array_t;

/*
 * This header file contains the internal definitions that the vvp
 * program uses to implement the public interface in the vpi_user.h
 * header file elsewhere.
 */

#যদি defined(__MINGW32__) || defined (__CYGWIN__)
বাহ্য vpip_routines_s vpi_routines;
#endif

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#define vpi_File_সারি    0x1000003
#define _vpiDescription 0x1000004

বাহ্য bool show_file_সারি;
বাহ্য bool code_is_instrumented;

বাহ্য vpiHandle vpip_build_file_সারি(ধ্রুব বর্ণ*description,
                                      long file_idx, long lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
বাহ্য ধ্রুব বর্ণ* vpi_type_as_string(PLI_INT32 code);

/*
 * Return the port direction as a string.
 */
বাহ্য ধ্রুব বর্ণ* direction_as_string(পূর্ণাঙ্ক dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#যদি defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
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
গণনা vpi_mode_t {
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
বাহ্য vpi_mode_t vpi_mode_flag;

/*
 * This structure is the very base of a vpiHandle. Every handle
 * structure is derived from this class so that the library can
 * internally pass the derived types as pointers to one of these.
 */
class __vpiHandle {
    সার্বজনিক:
      অন্তঃস্থ __vpiHandle() { }
	// The destructor is virtual so that dynamic types will work.
      আভাস ~__vpiHandle();

      আভাস পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব =0;
      আভাস পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code);
      আভাস int64_t vpi_get64(পূর্ণাঙ্ক code);
      আভাস বর্ণ* vpi_get_str(পূর্ণাঙ্ক code);

      আভাস শূন্য vpi_get_মান(p_vpi_মান val);
      আভাস vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags);
      আভাস vpiHandle vpi_handle(পূর্ণাঙ্ক code);
      আভাস vpiHandle vpi_iterate(পূর্ণাঙ্ক code);
      আভাস vpiHandle vpi_index(পূর্ণাঙ্ক idx);
      আভাস শূন্য vpi_get_delays(p_vpi_delay del);
      আভাস শূন্য vpi_put_delays(p_vpi_delay del);

	// Objects may have destroyer functions of their own. If so,
	// then this virtual method will return a POINTER to that
	// function. The pointer is used to "delete" the object, which
	// is why the function itself cannot be a method.
      প্রকারনির্দেশ পূর্ণাঙ্ক (*free_object_fun_t)(vpiHandle);
      আভাস free_object_fun_t free_object_fun(শূন্য);
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
সংরচনা __vpiIterator : সার্বজনিক __vpiHandle {
      __vpiIterator();
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      free_object_fun_t free_object_fun(শূন্য) override;

      vpiHandle *args;
      unsigned  nargs;
      unsigned  next;
      bool free_args_flag;
};

বাহ্য vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
				    bool free_args_flag);

class __vpiDecConst : সার্বজনিক __vpiHandle {
    সার্বজনিক:
      explicit __vpiDecConst(পূর্ণাঙ্ক val =0);
      __vpiDecConst(ধ্রুব __vpiDecConst&that);
# যদি __cplusplus >= 201103L
      __vpiDecConst& operator = (ধ্রুব __vpiDecConst&that) = স্বভাব;
# endif
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;

    সার্বজনিক:
      অন্তঃস্থ পূর্ণাঙ্ক get_মান() ধ্রুব { প্রত্যাগম মান; }
      অন্তঃস্থ শূন্য set_মান(পূর্ণাঙ্ক val) { মান = val; }

    গুপ্ত:
      পূর্ণাঙ্ক মান;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
সংরচনা __vpiCallback : সার্বজনিক __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;

	// Used for listing callbacks.
      সংরচনা __vpiCallback*next;

	// user supplied callback data
      সংরচনা t_cb_data cb_data;
};

class মান_callback : সার্বজনিক __vpiCallback {
    সার্বজনিক:
      explicit মান_callback(p_cb_data data);
	// Return true if the callback really is ready to be called
      আভাস bool test_মান_callback_ready(শূন্য);

    সার্বজনিক:
	// user supplied callback data
      সংরচনা t_vpi_time cb_time;
      সংরচনা t_vpi_মান cb_মান;
};

বাহ্য শূন্য callback_execute(সংরচনা __vpiCallback*cur);

সংরচনা __vpiSystemTime : সার্বজনিক __vpiHandle {
      __vpiSystemTime();
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ*vpi_get_str(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;

      __vpiScope*scope;
};

সংরচনা __vpiScopedTime : সার্বজনিক __vpiSystemTime {
      __vpiScopedTime();
      বর্ণ*vpi_get_str(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
};
সংরচনা __vpiScopedSTime : সার্বজনিক __vpiSystemTime {
      __vpiScopedSTime();
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ*vpi_get_str(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
};
সংরচনা __vpiScopedRealtime : সার্বজনিক __vpiSystemTime {
      __vpiScopedRealtime();
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ*vpi_get_str(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : সার্বজনিক __vpiHandle {

    সার্বজনিক:
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ* vpi_get_str(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_iterate(পূর্ণাঙ্ক code) override;

    সার্বজনিক:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      অন্তঃস্থ ধ্রুব বর্ণ*scope_name() ধ্রুব { প্রত্যাগম name_; }

      অন্তঃস্থ ধ্রুব বর্ণ*scope_def_name() ধ্রুব { প্রত্যাগম tname_; }
	// TRUE if this is an automatic func/task/block
      অন্তঃস্থ bool is_automatic() ধ্রুব { প্রত্যাগম is_automatic_; }

    সার্বজনিক:
      __vpiScope *scope;
      unsigned file_idx;
      unsigned lineno;
      unsigned def_file_idx;
      unsigned def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_time;
      সংরচনা __vpiScopedSTime scoped_stime;
      সংরচনা __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::vector<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      সংরচনা automatic_hooks_s**item;
      unsigned nitem;
        /* Keep a list of live contexts. */
      vvp_context_t live_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::set<vthread_t> threads;
      signed পূর্ণাঙ্ক time_units :8;
      signed পূর্ণাঙ্ক time_precision :8;

    রক্ষিত:
      __vpiScope(ধ্রুব বর্ণ*nam, ধ্রুব বর্ণ*tnam, bool is_auto_flag =false);

    গুপ্ত:
	/* The scope has a name. */
      ধ্রুব বর্ণ*name_;
      ধ্রুব বর্ণ*tname_;
	/* the scope may be "automatic" */
      bool is_automatic_;
};

class vpiScopeFunction  : সার্বজনিক __vpiScope {
    সার্বজনিক:
      অন্তঃস্থ vpiScopeFunction(ধ্রুব বর্ণ*nam, ধ্রুব বর্ণ*tnam,
			      bool auto_flag, পূর্ণাঙ্ক func_type, unsigned func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override { প্রত্যাগম vpiFunction; }
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override
      {
	    বিকল্প (code) {
		পক্ষ vpiFuncType:
		  প্রত্যাগম func_type_;
		স্বভাব:
		  প্রত্যাগম __vpiScope::vpi_get(code);
	    }
      }

    সার্বজনিক:
      অন্তঃস্থ unsigned get_func_width(শূন্য) ধ্রুব { প্রত্যাগম func_wid_; }
      অন্তঃস্থ vvp_bit4_t get_func_init_val(শূন্য) ধ্রুব { প্রত্যাগম func_init_val_; }

    গুপ্ত:
      পূর্ণাঙ্ক func_type_;
      unsigned func_wid_;
      vvp_bit4_t func_init_val_;
};

বাহ্য __vpiScope* vpip_peek_current_scope(শূন্য);
বাহ্য শূন্য vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj);
বাহ্য শূন্য vpip_attach_to_current_scope(vpiHandle obj);
বাহ্য __vpiScope* vpip_peek_context_scope(শূন্য);
বাহ্য unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                         __vpiScope*scope);
বাহ্য vpiHandle vpip_make_মূল_iterator(পূর্ণাঙ্ক type_code);
বাহ্য শূন্য vpip_make_মূল_iterator(class __vpiHandle**&table,
				    unsigned&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
সংরচনা __vpiSignal : সার্বজনিক __vpiHandle {
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      int64_t vpi_get64(পূর্ণাঙ্ক code) override;
      বর্ণ* vpi_get_str(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_iterate(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_index(পূর্ণাঙ্ক idx) override;

    সার্বজনিক:
      unsigned width() ধ্রুব;
      vpiHandle get_index(পূর্ণাঙ্ক index);
      শূন্য get_bit_মান(ধ্রুব সংরচনা __vpiBit*bit, p_vpi_মান vp);
      vpiHandle put_bit_মান(সংরচনা __vpiBit*bit, p_vpi_মান vp, পূর্ণাঙ্ক flags);
      শূন্য make_bits();

      সংরচনা __vpiBit*bits;

    সার্বজনিক:
      সঙ্ঘ { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
      সঙ্ঘ { // The name of this reg/net, or the index for array words.
            ধ্রুব বর্ণ*name;
            vpiHandle index;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      unsigned signed_flag  : 1;
      unsigned is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*গ্রন্থি;

    সার্বজনিক:
      স্থির শূন্য*operator new(std::size_t size);
      স্থির শূন্য operator delete(শূন্য*); // not implemented
    রক্ষিত:
      অন্তঃস্থ __vpiSignal() : bits(রিক্ত) { }
    গুপ্ত: // Not implemented
      স্থির শূন্য*operator new[] (std::size_t size);
      স্থির শূন্য operator delete[](শূন্য*);
};
বাহ্য unsigned vpip_size(ধ্রুব __vpiSignal *sig);
বাহ্য __vpiScope* vpip_scope(__vpiSignal*sig);

বাহ্য vpiHandle vpip_make_int2(ধ্রুব বর্ণ*name, পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb,
			       bool signed_flag, vvp_net_t*vec);
বাহ্য vpiHandle vpip_make_int4(ধ্রুব বর্ণ*name, পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb,
			       vvp_net_t*vec);
বাহ্য vpiHandle vpip_make_var4(ধ্রুব বর্ণ*name, পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb,
			       bool signed_flag, vvp_net_t*net);
বাহ্য vpiHandle vpip_make_net4(__vpiScope*scope,
				ধ্রুব বর্ণ*name, পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb,
				bool signed_flag, vvp_net_t*গ্রন্থি);

/*
 * This is used to represent a bit in a net/reg.
 */
সংরচনা __vpiBit {
      সংরচনা as_bit_t : সার্বজনিক __vpiHandle {
	    পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব;
	    পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code);
	    বর্ণ* vpi_get_str(পূর্ণাঙ্ক code);
	    শূন্য vpi_get_মান(p_vpi_মান val);
	    vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags);
	    vpiHandle vpi_handle(পূর্ণাঙ্ক code);
      } as_bit;

      vpiHandle index;

      সঙ্ঘ {
	    সংরচনা __vpiSignal*parent;
	    সংরচনা __vpiBit*bit0;
      };

      অন্তঃস্থ unsigned get_norm_index() ধ্রুব { প্রত্যাগম this - bit0; }
      অন্তঃস্থ সংরচনা __vpiSignal*get_parent() ধ্রুব {প্রত্যাগম (bit0 - 1)->parent; }
      পূর্ণাঙ্ক get_index(শূন্য) ধ্রুব;
};

class vpiPortBitInfo;

class vpiPortInfo  : সার্বজনিক __vpiHandle {
    সার্বজনিক:
      vpiPortInfo( __vpiScope *parent,
                    unsigned index,
                    পূর্ণাঙ্ক vpi_direction,
                    unsigned width,
                    ধ্রুব বর্ণ *name,
                    বর্ণ* buffer );
      ~vpiPortInfo() override;

      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override { প্রত্যাগম vpiPort; }
      পূর্ণাঙ্ক get_direction(শূন্য) { প্রত্যাগম direction_; }
      unsigned get_index(শূন্য) { প্রত্যাগম index_; }
      পূর্ণাঙ্ক get_width(শূন্য) { প্রত্যাগম width_; }
      শূন্য add_port_bit(vpiPortBitInfo* port_bit) { port_bits_.ঠেলো_back(port_bit); }

      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ* vpi_get_str(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;
      vvp_net_t* get_port(শূন্য) ধ্রুব { প্রত্যাগম ref_; }
      vpiHandle vpi_iterate(পূর্ণাঙ্ক code) override;

      std::vector<vpiPortBitInfo*> port_bits_;

    গুপ্ত:
      __vpiScope *parent_;
      unsigned  index_;
      পূর্ণাঙ্ক       direction_;
      unsigned  width_;
      ধ্রুব বর্ণ *name_;
      vvp_net_t *ref_;
};

class vpiPortBitInfo  : সার্বজনিক __vpiHandle {
    সার্বজনিক:
      vpiPortBitInfo(vpiPortInfo *parent,
                     unsigned bit);
      ~vpiPortBitInfo() override;

      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override { প্রত্যাগম vpiPortBit; }
      unsigned get_bit(শূন্য) ধ্রুব { প্রত্যাগম bit_; }

      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;

    গুপ্ত:
      vpiPortInfo *parent_;
      unsigned bit_;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
সংরচনা __vpiPV : সার্বজনিক __vpiHandle {
      __vpiPV();
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ* vpi_get_str(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;

      vpiHandle parent;
      vvp_net_t*net;
      vpiHandle sbase;
      পূর্ণাঙ্ক tbase;
      unsigned width;
};
বাহ্য vpiHandle vpip_make_PV(বর্ণ*name, পূর্ণাঙ্ক base, পূর্ণাঙ্ক width);
বাহ্য vpiHandle vpip_make_PV(বর্ণ*name, বর্ণ*প্রতীক, পূর্ণাঙ্ক width);
বাহ্য vpiHandle vpip_make_PV(বর্ণ*name, vpiHandle handle, পূর্ণাঙ্ক width);

সংরচনা __vpiModPathTerm : সার্বজনিক __vpiHandle {
      __vpiModPathTerm();
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;

      vpiHandle রাশিমালা;
	/* The value returned by vpi_get(vpiEdge, ...); */
      পূর্ণাঙ্ক edge;
};

সংরচনা __vpiModPathSrc : সার্বজনিক __vpiHandle {
      __vpiModPathSrc();
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_iterate(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_index(পূর্ণাঙ্ক idx) override;
      শূন্য vpi_get_delays(p_vpi_delay del) override;
      শূন্য vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(শূন্য) override;

      সংরচনা __vpiModPath *dest;
      পূর্ণাঙ্ক   type;

	/* This is the input expression for this modpath. */
      সংরচনা __vpiModPathTerm path_term_in;

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

সংরচনা __vpiModPath {
      __vpiScope   *scope ;

      class vvp_fun_modpath*modpath;

      সংরচনা __vpiModPathTerm path_term_out;
      vvp_net_t *প্রবেশ_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

বাহ্য সংরচনা __vpiModPathSrc* vpip_make_modpath_src(সংরচনা __vpiModPath*path,
                                                     vvp_net_t *net) ;

বাহ্য সংরচনা __vpiModPath* vpip_make_modpath(vvp_net_t *net) ;

/*
 *
 * The vpiInterModPath vpiHandle will define
 * a vpiInterModPath of record .intermodpath as defined
 * in the IEEE 1364
 *
 */

সংরচনা __vpiInterModPath : সার্বজনিক __vpiHandle {
      __vpiInterModPath();
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_iterate(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_delays(p_vpi_delay del) override;
      শূন্য vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(শূন্য) override;

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

বাহ্য সংরচনা __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2);

/*
 * These methods support the vpi creation of events. The name string
 * passed in will be saved, so the caller must allocate it (or not
 * free it) after it is handed to this function.
 */
class __vpiNamedEvent : সার্বজনিক __vpiHandle {

    সার্বজনিক:
      __vpiNamedEvent(__vpiScope*scope, ধ্রুব বর্ণ*name);
      ~__vpiNamedEvent() override;

      __vpiNamedEvent(ধ্রুব __vpiNamedEvent&) = delete;
      __vpiNamedEvent& operator=(ধ্রুব __vpiNamedEvent&) = delete;

      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      __vpiScope*get_scope(শূন্য) ধ্রুব { প্রত্যাগম scope_; }
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ* vpi_get_str(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;

      অন্তঃস্থ শূন্য add_vpi_callback(__vpiCallback*cb)
      { cb->next = callbacks_;
	callbacks_ = cb;
      }

      শূন্য run_vpi_callbacks(শূন্য);

	/* The functor, used for %set operations. */
      vvp_net_t*funct;

    গুপ্ত:
	/* base name of the event object */
      ধ্রুব বর্ণ*name_;
	/* Parent scope of this object. */
      __vpiScope*scope_;
	/* List of callbacks interested in this event. */
      __vpiCallback*callbacks_;
};

বাহ্য vpiHandle vpip_make_named_event(ধ্রুব বর্ণ*name, vvp_net_t*f);

/*
 * Memory is an array of bits that is accessible in N-bit chunks, with
 * N being the width of a word. The memory word handle just points
 * back to the memory and uses an index to identify its position in
 * the memory.
 */

বাহ্য bool is_net_array(vpiHandle obj);

/*
 * These are the various variable types.
 */
সংরচনা __vpiRealVar : সার্বজনিক __vpiHandle {
      __vpiRealVar();
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ* vpi_get_str(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_iterate(পূর্ণাঙ্ক code) override;

      সঙ্ঘ { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      সঙ্ঘ {
            ধ্রুব বর্ণ*name;
            vpiHandle index;
      } id;
      unsigned is_netarray  : 1; // This is word of a net array
      unsigned is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

বাহ্য __vpiScope* vpip_scope(__vpiRealVar*sig);
বাহ্য vpiHandle vpip_make_real_var(ধ্রুব বর্ণ*name, vvp_net_t*net);
বাহ্য vpiHandle vpip_make_real_net(__vpiScope*scope,
				    ধ্রুব বর্ণ*name, vvp_net_t*net);

class __vpiBaseVar : সার্বজনিক __vpiHandle {

    সার্বজনিক:
      __vpiBaseVar(__vpiScope*scope, ধ্রুব বর্ণ*name, vvp_net_t*net);
#ifdef CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#endif

      অন্তঃস্থ vvp_net_t* get_net() ধ্রুব { প্রত্যাগম net_; }

    রক্ষিত:
      __vpiScope* scope_;
      ধ্রুব বর্ণ*name_;

    গুপ্ত:
      vvp_net_t*net_;
};

class __vpiStringVar : সার্বজনিক __vpiBaseVar {

    সার্বজনিক:
      __vpiStringVar(__vpiScope*scope, ধ্রুব বর্ণ*name, vvp_net_t*net);

      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags) override;
};

বাহ্য vpiHandle vpip_make_string_var(ধ্রুব বর্ণ*name, vvp_net_t*net);

সংরচনা __vpiArrayBase {
      __vpiArrayBase() : vals_শব্দগুলি(রিক্ত) {}
      আভাস ~__vpiArrayBase() {}

      আভাস unsigned get_size(শূন্য) ধ্রুব = 0;
      আভাস vpiHandle get_বাম_range() = 0;
      আভাস vpiHandle get_ডান_range() = 0;
      আভাস __vpiScope*get_scope() ধ্রুব = 0;

      আভাস পূর্ণাঙ্ক get_word_size() ধ্রুব = 0;
      আভাস বর্ণ*get_word_str(সংরচনা __vpiArrayWord*word, পূর্ণাঙ্ক code) = 0;
      আভাস শূন্য get_word_মান(সংরচনা __vpiArrayWord*word, p_vpi_মান vp) = 0;
      আভাস শূন্য put_word_মান(সংরচনা __vpiArrayWord*word, p_vpi_মান vp,
                                    পূর্ণাঙ্ক flags) = 0;
      আভাস vpiHandle get_iter_index(সংরচনা __vpiArrayIterator*iter, পূর্ণাঙ্ক idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_base_iterate(পূর্ণাঙ্ক code);

      আভাস শূন্য make_vals_শব্দগুলি();

      সংরচনা __vpiArrayWord*vals_শব্দগুলি;
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
সংরচনা __vpiArray : সার্বজনিক __vpiArrayBase, সার্বজনিক __vpiHandle {
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override { প্রত্যাগম vpiMemory; }
      unsigned get_size() ধ্রুব override { প্রত্যাগম array_গুনতি; }
      vpiHandle get_বাম_range() override { প্রতিজ্ঞা(nets == 0); প্রত্যাগম &msb; }
      vpiHandle get_ডান_range() override { প্রতিজ্ঞা(nets == 0); প্রত্যাগম &lsb; }
      __vpiScope*get_scope() ধ্রুব override { প্রত্যাগম scope; }

      পূর্ণাঙ্ক get_word_size() ধ্রুব override;
      বর্ণ*get_word_str(সংরচনা __vpiArrayWord*word, পূর্ণাঙ্ক code) override;
      শূন্য get_word_মান(সংরচনা __vpiArrayWord*word, p_vpi_মান vp) override;
      শূন্য put_word_মান(সংরচনা __vpiArrayWord*word, p_vpi_মান vp, পূর্ণাঙ্ক flags) override;

      vpiHandle get_iter_index(সংরচনা __vpiArrayIterator*iter, পূর্ণাঙ্ক idx) override;

      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ* vpi_get_str(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;
      অন্তঃস্থ vpiHandle vpi_iterate(পূর্ণাঙ্ক code) override { প্রত্যাগম vpi_array_base_iterate(code); }
      vpiHandle vpi_index(পূর্ণাঙ্ক idx) override;

      শূন্য set_word(unsigned idx, unsigned off, ধ্রুব vvp_vector4_t&val);
      শূন্য set_word(unsigned idx, দ্বিগুণ val);
      শূন্য set_word(unsigned idx, ধ্রুব std::string&val);
      শূন্য set_word(unsigned idx, ধ্রুব vvp_object_t&val);

      vvp_vector4_t get_word(unsigned address);
      দ্বিগুণ get_word_r(unsigned address);
      শূন্য get_word_obj(unsigned address, vvp_object_t&val);
      std::string get_word_str(unsigned address);

      শূন্য alias_word(unsigned long addr, vpiHandle word, পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb);
      শূন্য attach_word(unsigned addr, vpiHandle word);
      শূন্য word_change(unsigned long addr);

      ধ্রুব বর্ণ*name; /* Permanently allocated string */
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
      সংরচনা __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool swap_addr;

গুপ্ত:
      unsigned array_গুনতি;
      __vpiScope*scope;

friend vpiHandle vpip_make_array(ধ্রুব বর্ণ*label, ধ্রুব বর্ণ*name,
                                 পূর্ণাঙ্ক first_addr, পূর্ণাঙ্ক last_addr,
                                 bool signed_flag);
friend শূন্য compile_array_alias(বর্ণ*label, বর্ণ*name, বর্ণ*src);
};

class __vpiDarrayVar : সার্বজনিক __vpiBaseVar, সার্বজনিক __vpiArrayBase {
    সার্বজনিক:
      __vpiDarrayVar(__vpiScope*scope, ধ্রুব বর্ণ*name, vvp_net_t*net);

      পূর্ণাঙ্ক get_type_code() ধ্রুব override { প্রত্যাগম vpiArrayVar; }
      unsigned get_size() ধ্রুব override;
      vpiHandle get_বাম_range() override;
      vpiHandle get_ডান_range() override;
      __vpiScope*get_scope() ধ্রুব override { প্রত্যাগম scope_; }

      পূর্ণাঙ্ক get_word_size() ধ্রুব override;
      বর্ণ*get_word_str(সংরচনা __vpiArrayWord*word, পূর্ণাঙ্ক code) override;
      শূন্য get_word_মান(সংরচনা __vpiArrayWord*word, p_vpi_মান vp) override;
      শূন্য put_word_মান(সংরচনা __vpiArrayWord*word, p_vpi_মান vp, পূর্ণাঙ্ক flags) override;

      vpiHandle get_iter_index(সংরচনা __vpiArrayIterator*iter, পূর্ণাঙ্ক idx) override;
      অন্তঃস্থ vpiHandle vpi_iterate(পূর্ণাঙ্ক code) override { প্রত্যাগম vpi_array_base_iterate(code); }

      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ* vpi_get_str(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_index(পূর্ণাঙ্ক index) override;

      শূন্য vpi_get_মান(p_vpi_মান val) override;

    রক্ষিত:
      vvp_darray*get_vvp_darray() ধ্রুব;
      __vpiDecConst বাম_range, ডান_range;
};

বাহ্য vpiHandle vpip_make_darray_var(ধ্রুব বর্ণ*name, vvp_net_t*net);

class __vpiQueueVar : সার্বজনিক __vpiDarrayVar {

    সার্বজনিক:
      __vpiQueueVar(__vpiScope*scope, ধ্রুব বর্ণ*name, vvp_net_t*net);

      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
};

বাহ্য vpiHandle vpip_make_queue_var(ধ্রুব বর্ণ*name, vvp_net_t*net);

class __vpiPropQueueRef : সার্বজনিক __vpiHandle {

    সার্বজনিক:
      explicit __vpiPropQueueRef(__vpiScope*scope, unsigned pidx, bool is_queue);

      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ* vpi_get_str(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;

      vvp_net_t* class_net_;
      unsigned prop_idx_;
      bool is_queue_;

    গুপ্ত:
      __vpiScope* scope_;
};

বাহ্য vpiHandle vpip_make_prop_queue_ref(বর্ণ* class_label,
                                          unsigned prop_idx,
                                          unsigned is_queue_flag);
বাহ্য vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : সার্বজনিক __vpiBaseVar {

    সার্বজনিক:
      __vpiCobjectVar(__vpiScope*scope, ধ্রুব বর্ণ*name, vvp_net_t*net);

      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
};

বাহ্য vpiHandle vpip_make_cobject_var(ধ্রুব বর্ণ*name, vvp_net_t*net);

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
সংরচনা __vpiUserSystf : সার্বজনিক __vpiHandle {
      __vpiUserSystf();
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;

      s_vpi_systf_data info;
      bool is_user_defn;
};

বাহ্য vpiHandle vpip_make_systf_iterator(শূন্য);

বাহ্য সংরচনা __vpiUserSystf* vpip_find_systf(ধ্রুব বর্ণ*name);


সংরচনা __vpiSysTaskCall : সার্বজনিক __vpiHandle {

      __vpiScope* scope;
      সংরচনা __vpiUserSystf*defn;
      unsigned nargs;
      vpiHandle*args;
	/* Stack consumed by this call */
      unsigned vec4_স্তূপ;
      unsigned real_স্তূপ;
      unsigned string_স্তূপ;
	/* Support for vpi_get_userdata. */
      শূন্য*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      unsigned file_idx;
      unsigned lineno;
      bool put_মান;
    রক্ষিত:
      অন্তঃস্থ __vpiSysTaskCall()
      {
	    vec4_স্তূপ = 0;
	    real_স্তূপ = 0;
	    string_স্তূপ = 0;
      }
};

বাহ্য সংরচনা __vpiSysTaskCall*vpip_cur_task;

/*
 * The persistent flag to vpip_make_string_const causes the created
 * handle to be persistent. This is necessary for cases where the
 * string handle may be reused, which is the normal case.
 *
 * When constructing with a string, the class takes possession of the
 * text value string, and will delete it in the constructor.
 */

vpiHandle vpip_make_string_const(বর্ণ*text, bool persistent =true);
vpiHandle vpip_make_string_param(ধ্রুব বর্ণ*name, বর্ণ*মান, bool local_flag,
                                 long file_idx, long lineno);

সংরচনা __vpiBinaryConst : সার্বজনিক __vpiHandle {
      __vpiBinaryConst();
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;

      vvp_vector4_t bits;
	/* TRUE if this constant is signed. */
      unsigned signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      unsigned sized_flag  :1;
};

vpiHandle vpip_make_দ্বিমিক_const(unsigned wid, ধ্রুব বর্ণ*bits);
vpiHandle vpip_make_দ্বিমিক_param(ধ্রুব বর্ণ*name, ধ্রুব vvp_vector4_t&bits,
				 bool signed_flag, bool local_flag,
				 long file_idx, long lineno);

class __vpiRealConst : সার্বজনিক __vpiHandle {
    সার্বজনিক:
      explicit __vpiRealConst(দ্বিগুণ);
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;

      দ্বিগুণ মান;
};

vpiHandle vpip_make_real_const(দ্বিগুণ মান);
vpiHandle vpip_make_real_param(ধ্রুব বর্ণ*name, দ্বিগুণ মান, bool local_flag,
                               long file_idx, long lineno);

class __vpiNullConst : সার্বজনিক __vpiHandle {
    সার্বজনিক:
      explicit __vpiNullConst();
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_word(unsigned base, ধ্রুব বর্ণ*type);
vpiHandle vpip_make_vthr_str_স্তূপ(unsigned depth);
vpiHandle vpip_make_vthr_vec4_স্তূপ(unsigned depth, bool signed_flag, unsigned wid);

vpiHandle vpip_make_vthr_A(বর্ণ*label, unsigned index);
vpiHandle vpip_make_vthr_A(বর্ণ*label, বর্ণ*প্রতীক);
vpiHandle vpip_make_vthr_A(বর্ণ*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(বর্ণ*label, unsigned index, unsigned bit, unsigned wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
বাহ্য "C" শূন্য vpip_load_module(ধ্রুব বর্ণ*name);

বাহ্য "C" শূন্য vpip_clear_module_paths();
বাহ্য "C" শূন্য vpip_add_module_path(ধ্রুব বর্ণ *path);
বাহ্য "C" শূন্য vpip_add_env_and_default_module_paths();

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
বাহ্য vpiHandle vpip_build_vpi_call(ধ্রুব বর্ণ*name,
				     পূর্ণাঙ্ক val_type, unsigned return_width,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     unsigned argc,
				     vpiHandle*argv,
				     unsigned vec4_স্তূপ,
				     unsigned real_স্তূপ,
				     unsigned string_স্তূপ,
				     long file_idx,
				     long lineno);

বাহ্য vthread_t vpip_current_vthread;

বাহ্য শূন্য vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_time(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

বাহ্য পূর্ণাঙ্ক vpip_get_time_precision(শূন্য);
বাহ্য শূন্য vpip_set_time_precision(পূর্ণাঙ্ক pres);

বাহ্য __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

বাহ্য পূর্ণাঙ্ক vpip_time_units_from_handle(vpiHandle obj);
বাহ্য পূর্ণাঙ্ক vpip_time_precision_from_handle(vpiHandle obj);

বাহ্য দ্বিগুণ vpip_scaled_time_from_handle(vvp_time64_t time, vpiHandle obj);

বাহ্য শূন্য vpip_time_to_timestruct(সংরচনা t_vpi_time*ts, vvp_time64_t ti);
বাহ্য vvp_time64_t vpip_timestruct_to_time(ধ্রুব সংরচনা t_vpi_time*ts);

বাহ্য দ্বিগুণ vpip_time_to_scaled_real(vvp_time64_t ti, ধ্রুব __vpiScope*sc);
বাহ্য vvp_time64_t vpip_scaled_real_to_time64(দ্বিগুণ val, ধ্রুব __vpiScope*sc);

/*
 * These functions are used mostly as compile time to strings into
 * permallocated memory. The vpip_string function is the most general,
 * it allocates a fresh string no matter what. The vpip_name_string
 * allocates a string and keeps a pointer in the hash, and tries to
 * reuse it if it can. This us useful for handle names, which may be
 * reused in different scopes.
 */
বাহ্য ধ্রুব বর্ণ* vpip_string(ধ্রুব বর্ণ*str);
বাহ্য ধ্রুব বর্ণ* vpip_name_string(ধ্রুব বর্ণ*str);


/*
 * This function is used to make decimal string versions of various
 * vectors. The input format is a vvp_vector4_t, and the result is
 * written into buf, without overflowing nbuf.
 */
বাহ্য unsigned vpip_vec4_to_dec_str(ধ্রুব vvp_vector4_t&vec4,
				     বর্ণ *buf, unsigned পূর্ণাঙ্ক nbuf,
				     পূর্ণাঙ্ক signed_flag);


বাহ্য শূন্য vpip_vec4_to_hex_str(ধ্রুব vvp_vector4_t&bits, বর্ণ*buf,
				 unsigned nbuf);

বাহ্য শূন্য vpip_vec4_to_oct_str(ধ্রুব vvp_vector4_t&bits, বর্ণ*buf,
				 unsigned nbuf);

বাহ্য শূন্য vpip_bin_str_to_vec4(vvp_vector4_t&val, ধ্রুব বর্ণ*buf);
বাহ্য শূন্য vpip_oct_str_to_vec4(vvp_vector4_t&val, ধ্রুব বর্ণ*str);
বাহ্য শূন্য vpip_dec_str_to_vec4(vvp_vector4_t&val, ধ্রুব বর্ণ*str);
বাহ্য শূন্য vpip_hex_str_to_vec4(vvp_vector4_t&val, ধ্রুব বর্ণ*str);

বাহ্য vvp_vector4_t vec4_from_vpi_মান(s_vpi_মান*vp, unsigned wid);
বাহ্য দ্বিগুণ real_from_vpi_মান(s_vpi_মান*vp);

বাহ্য শূন্য vpip_vec4_get_মান(ধ্রুব vvp_vector4_t&word_val, unsigned width,
				bool signed_flag, s_vpi_মান*vp);
বাহ্য শূন্য vpip_vec2_get_মান(ধ্রুব vvp_vector2_t&word_val, unsigned width,
				bool signed_flag, s_vpi_মান*vp);
বাহ্য শূন্য vpip_real_get_মান(দ্বিগুণ real, s_vpi_মান*vp);
বাহ্য শূন্য vpip_string_get_মান(ধ্রুব std::string&val, s_vpi_মান*vp);

/*
 * Function defined in vpi_signal.cc to manage vpi_get_* persistent
 * storage.
 */
গণনা vpi_rbuf_t {
      RBUF_VAL =0,
	/* Storage for *_get_value() */
      RBUF_STR,
	/* Storage for *_get_str() */
      RBUF_DEL
	/* Delete the storage for both buffers. */
};
বাহ্য শূন্য *need_result_buf(unsigned cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
বাহ্য বর্ণ *simple_set_rbuf_str(ধ্রুব বর্ণ *s1);
বাহ্য বর্ণ *generic_get_str(পূর্ণাঙ্ক code, vpiHandle ref, ধ্রুব বর্ণ *name, ধ্রুব বর্ণ *index);

/* A routine to find the enclosing module. */
বাহ্য vpiHandle vpip_module(__vpiScope*scope);

বাহ্য পূর্ণাঙ্ক vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
বাহ্য bool vpip_mcd0_disable;

অন্তঃস্থ uint64_t vlg_round_to_u64(দ্বিগুণ rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      প্রত্যাগম static_cast<uint64_t>(static_cast<int64_t>(std::round(rval)));
}

#endif /* IVL_vpi_priv_H */
