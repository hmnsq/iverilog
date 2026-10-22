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

#אם defined(__MINGW32__) || defined (__CYGWIN__)
خارجي vpip_routines_s vpi_routines;
#endif

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#define _vpiFileLine    0x1000003
#define _vpiDescription 0x1000004

خارجي bool show_file_line;
خارجي bool code_is_instrumented;

خارجي vpiHandle vpip_build_file_line(ثابت حرف*description,
                                      long file_idx, long lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
خارجي ثابت حرف* vpi_type_as_string(PLI_INT32 code);

/*
 * Return the port direction as a string.
 */
خارجي ثابت حرف* direction_as_string(صحيح dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#אם defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
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
تعداد vpi_mode_t {
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
خارجي vpi_mode_t vpi_mode_flag;

/*
 * This structure is the very base of a vpiHandle. Every handle
 * structure is derived from this class so that the library can
 * internally pass the derived types as pointers to one of these.
 */
class __vpiHandle {
    عام:
      مضمن __vpiHandle() { }
	// The destructor is virtual so that dynamic types will work.
      virtual ~__vpiHandle();

      virtual صحيح get_type_code(فراغ) ثابت =0;
      virtual صحيح vpi_get(صحيح code);
      virtual int64_t vpi_get64(صحيح code);
      virtual حرف* vpi_get_str(صحيح code);

      virtual فراغ vpi_get_value(p_vpi_value val);
      virtual vpiHandle vpi_put_value(p_vpi_value val, صحيح flags);
      virtual vpiHandle vpi_handle(صحيح code);
      virtual vpiHandle vpi_iterate(صحيح code);
      virtual vpiHandle vpi_index(صحيح idx);
      virtual فراغ vpi_get_delays(p_vpi_delay del);
      virtual فراغ vpi_put_delays(p_vpi_delay del);

	// Objects may have destroyer functions of their own. If so,
	// then this virtual method will return a POINTER to that
	// function. The pointer is used to "delete" the object, which
	// is why the function itself cannot be a method.
      تعريفنوع صحيح (*free_object_fun_t)(vpiHandle);
      virtual free_object_fun_t free_object_fun(فراغ);
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
      صحيح get_type_code(فراغ) ثابت override;
      free_object_fun_t free_object_fun(فراغ) override;

      vpiHandle *args;
      unsigned  nargs;
      unsigned  next;
      bool free_args_flag;
};

خارجي vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
				    bool free_args_flag);

class __vpiDecConst : عام __vpiHandle {
    عام:
      explicit __vpiDecConst(صحيح val =0);
      __vpiDecConst(ثابت __vpiDecConst&that);
# אם __cplusplus >= 201103L
      __vpiDecConst& operator = (ثابت __vpiDecConst&that) = افتراضي;
# endif
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;

    عام:
      مضمن صحيح get_value() ثابت { החזר value; }
      مضمن فراغ set_value(صحيح val) { value = val; }

    خاص:
      صحيح value;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
بنية __vpiCallback : عام __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      صحيح get_type_code(فراغ) ثابت override;

	// Used for listing callbacks.
      بنية __vpiCallback*next;

	// user supplied callback data
      بنية t_cb_data cb_data;
};

class value_callback : عام __vpiCallback {
    عام:
      explicit value_callback(p_cb_data data);
	// Return true if the callback really is ready to be called
      virtual bool test_value_callback_ready(فراغ);

    عام:
	// user supplied callback data
      بنية t_vpi_time cb_time;
      بنية t_vpi_value cb_value;
};

خارجي فراغ callback_execute(بنية __vpiCallback*cur);

بنية __vpiSystemTime : عام __vpiHandle {
      __vpiSystemTime();
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;
      vpiHandle vpi_handle(صحيح code) override;

      __vpiScope*scope;
};

بنية __vpiScopedTime : عام __vpiSystemTime {
      __vpiScopedTime();
      حرف*vpi_get_str(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;
};
بنية __vpiScopedSTime : عام __vpiSystemTime {
      __vpiScopedSTime();
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;
};
بنية __vpiScopedRealtime : عام __vpiSystemTime {
      __vpiScopedRealtime();
      صحيح vpi_get(صحيح code) override;
      حرف*vpi_get_str(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : عام __vpiHandle {

    عام:
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;
      vpiHandle vpi_iterate(صحيح code) override;

    عام:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      مضمن ثابت حرف*scope_name() ثابت { החזר name_; }

      مضمن ثابت حرف*scope_def_name() ثابت { החזר tname_; }
	// TRUE if this is an automatic func/task/block
      مضمن bool is_automatic() ثابت { החזר is_automatic_; }

    عام:
      __vpiScope *scope;
      unsigned file_idx;
      unsigned lineno;
      unsigned def_file_idx;
      unsigned def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_time;
      بنية __vpiScopedSTime scoped_stime;
      بنية __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::vector<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      بنية automatic_hooks_s**item;
      unsigned nitem;
        /* Keep a list of live contexts. */
      vvp_context_t live_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::set<vthread_t> threads;
      signed صحيح time_units :8;
      signed صحيح time_precision :8;

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
			      bool auto_flag, صحيح func_type, unsigned func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      صحيح get_type_code(فراغ) ثابت override { החזר vpiFunction; }
      صحيح vpi_get(صحيح code) override
      {
	    بدل (code) {
		حالة vpiFuncType:
		  החזר func_type_;
		افتراضي:
		  החזר __vpiScope::vpi_get(code);
	    }
      }

    عام:
      مضمن unsigned get_func_width(فراغ) ثابت { החזר func_wid_; }
      مضمن vvp_bit4_t get_func_init_val(فراغ) ثابت { החזר func_init_val_; }

    خاص:
      صحيح func_type_;
      unsigned func_wid_;
      vvp_bit4_t func_init_val_;
};

خارجي __vpiScope* vpip_peek_current_scope(فراغ);
خارجي فراغ vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj);
خارجي فراغ vpip_attach_to_current_scope(vpiHandle obj);
خارجي __vpiScope* vpip_peek_context_scope(فراغ);
خارجي unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                         __vpiScope*scope);
خارجي vpiHandle vpip_make_root_iterator(صحيح type_code);
خارجي فراغ vpip_make_root_iterator(class __vpiHandle**&table,
				    unsigned&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
بنية __vpiSignal : عام __vpiHandle {
      صحيح vpi_get(صحيح code) override;
      int64_t vpi_get64(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;
      vpiHandle vpi_put_value(p_vpi_value val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح code) override;
      vpiHandle vpi_iterate(صحيح code) override;
      vpiHandle vpi_index(صحيح idx) override;

    عام:
      unsigned width() ثابت;
      vpiHandle get_index(صحيح index);
      فراغ get_bit_value(ثابت بنية __vpiBit*bit, p_vpi_value vp);
      vpiHandle put_bit_value(بنية __vpiBit*bit, p_vpi_value vp, صحيح flags);
      فراغ make_bits();

      بنية __vpiBit*bits;

    عام:
      اتحاد { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
      اتحاد { // The name of this reg/net, or the index for array words.
            ثابت حرف*name;
            vpiHandle index;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      unsigned signed_flag  : 1;
      unsigned is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*node;

    عام:
      ساكن فراغ*operator new(std::size_t size);
      ساكن فراغ operator delete(فراغ*); // not implemented
    محمي:
      مضمن __vpiSignal() : bits(عدم) { }
    خاص: // Not implemented
      ساكن فراغ*operator new[] (std::size_t size);
      ساكن فراغ operator delete[](فراغ*);
};
خارجي unsigned vpip_size(ثابت __vpiSignal *sig);
خارجي __vpiScope* vpip_scope(__vpiSignal*sig);

خارجي vpiHandle vpip_make_int2(ثابت حرف*name, صحيح msb, صحيح lsb,
			       bool signed_flag, vvp_net_t*vec);
خارجي vpiHandle vpip_make_int4(ثابت حرف*name, صحيح msb, صحيح lsb,
			       vvp_net_t*vec);
خارجي vpiHandle vpip_make_var4(ثابت حرف*name, صحيح msb, صحيح lsb,
			       bool signed_flag, vvp_net_t*net);
خارجي vpiHandle vpip_make_net4(__vpiScope*scope,
				ثابت حرف*name, صحيح msb, صحيح lsb,
				bool signed_flag, vvp_net_t*node);

/*
 * This is used to represent a bit in a net/reg.
 */
بنية __vpiBit {
      بنية as_bit_t : عام __vpiHandle {
	    صحيح get_type_code(فراغ) ثابت;
	    صحيح vpi_get(صحيح code);
	    حرف* vpi_get_str(صحيح code);
	    فراغ vpi_get_value(p_vpi_value val);
	    vpiHandle vpi_put_value(p_vpi_value val, صحيح flags);
	    vpiHandle vpi_handle(صحيح code);
      } as_bit;

      vpiHandle index;

      اتحاد {
	    بنية __vpiSignal*parent;
	    بنية __vpiBit*bit0;
      };

      مضمن unsigned get_norm_index() ثابت { החזר this - bit0; }
      مضمن بنية __vpiSignal*get_parent() ثابت {החזר (bit0 - 1)->parent; }
      صحيح get_index(فراغ) ثابت;
};

class vpiPortBitInfo;

class vpiPortInfo  : عام __vpiHandle {
    عام:
      vpiPortInfo( __vpiScope *parent,
                    unsigned index,
                    صحيح vpi_direction,
                    unsigned width,
                    ثابت حرف *name,
                    حرف* buffer );
      ~vpiPortInfo() override;

      صحيح get_type_code(فراغ) ثابت override { החזר vpiPort; }
      صحيح get_direction(فراغ) { החזר direction_; }
      unsigned get_index(فراغ) { החזר index_; }
      صحيح get_width(فراغ) { החזר width_; }
      فراغ add_port_bit(vpiPortBitInfo* port_bit) { port_bits_.push_back(port_bit); }

      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;
      vvp_net_t* get_port(فراغ) ثابت { החזר ref_; }
      vpiHandle vpi_iterate(صحيح code) override;

      std::vector<vpiPortBitInfo*> port_bits_;

    خاص:
      __vpiScope *parent_;
      unsigned  index_;
      صحيح       direction_;
      unsigned  width_;
      ثابت حرف *name_;
      vvp_net_t *ref_;
};

class vpiPortBitInfo  : عام __vpiHandle {
    عام:
      vpiPortBitInfo(vpiPortInfo *parent,
                     unsigned bit);
      ~vpiPortBitInfo() override;

      صحيح get_type_code(فراغ) ثابت override { החזר vpiPortBit; }
      unsigned get_bit(فراغ) ثابت { החזר bit_; }

      صحيح vpi_get(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;

    خاص:
      vpiPortInfo *parent_;
      unsigned bit_;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
بنية __vpiPV : عام __vpiHandle {
      __vpiPV();
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;
      vpiHandle vpi_put_value(p_vpi_value val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح code) override;

      vpiHandle parent;
      vvp_net_t*net;
      vpiHandle sbase;
      صحيح tbase;
      unsigned width;
};
خارجي vpiHandle vpip_make_PV(حرف*name, صحيح base, صحيح width);
خارجي vpiHandle vpip_make_PV(حرف*name, حرف*symbol, صحيح width);
خارجي vpiHandle vpip_make_PV(حرف*name, vpiHandle handle, صحيح width);

بنية __vpiModPathTerm : عام __vpiHandle {
      __vpiModPathTerm();
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;

      vpiHandle expr;
	/* The value returned by vpi_get(vpiEdge, ...); */
      صحيح edge;
};

بنية __vpiModPathSrc : عام __vpiHandle {
      __vpiModPathSrc();
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;
      vpiHandle vpi_put_value(p_vpi_value val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح code) override;
      vpiHandle vpi_iterate(صحيح code) override;
      vpiHandle vpi_index(صحيح idx) override;
      فراغ vpi_get_delays(p_vpi_delay del) override;
      فراغ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(فراغ) override;

      بنية __vpiModPath *dest;
      صحيح   type;

	/* This is the input expression for this modpath. */
      بنية __vpiModPathTerm path_term_in;

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

      بنية __vpiModPathTerm path_term_out;
      vvp_net_t *input_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

خارجي بنية __vpiModPathSrc* vpip_make_modpath_src(بنية __vpiModPath*path,
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
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;
      vpiHandle vpi_put_value(p_vpi_value val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح code) override;
      vpiHandle vpi_iterate(صحيح code) override;
      فراغ vpi_get_delays(p_vpi_delay del) override;
      فراغ vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(فراغ) override;

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

خارجي بنية __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2);

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
      __vpiNamedEvent& operator=(ثابت __vpiNamedEvent&) = delete;

      صحيح get_type_code(فراغ) ثابت override;
      __vpiScope*get_scope(فراغ) ثابت { החזר scope_; }
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      vpiHandle vpi_put_value(p_vpi_value val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح code) override;

      مضمن فراغ add_vpi_callback(__vpiCallback*cb)
      { cb->next = callbacks_;
	callbacks_ = cb;
      }

      فراغ run_vpi_callbacks(فراغ);

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
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;
      vpiHandle vpi_put_value(p_vpi_value val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح code) override;
      vpiHandle vpi_iterate(صحيح code) override;

      اتحاد { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      اتحاد {
            ثابت حرف*name;
            vpiHandle index;
      } id;
      unsigned is_netarray  : 1; // This is word of a net array
      unsigned is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

خارجي __vpiScope* vpip_scope(__vpiRealVar*sig);
خارجي vpiHandle vpip_make_real_var(ثابت حرف*name, vvp_net_t*net);
خارجي vpiHandle vpip_make_real_net(__vpiScope*scope,
				    ثابت حرف*name, vvp_net_t*net);

class __vpiBaseVar : عام __vpiHandle {

    عام:
      __vpiBaseVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);
#ifdef CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#endif

      مضمن vvp_net_t* get_net() ثابت { החזר net_; }

    محمي:
      __vpiScope* scope_;
      ثابت حرف*name_;

    خاص:
      vvp_net_t*net_;
};

class __vpiStringVar : عام __vpiBaseVar {

    عام:
      __vpiStringVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;
      vpiHandle vpi_put_value(p_vpi_value val, صحيح flags) override;
};

خارجي vpiHandle vpip_make_string_var(ثابت حرف*name, vvp_net_t*net);

بنية __vpiArrayBase {
      __vpiArrayBase() : vals_words(عدم) {}
      virtual ~__vpiArrayBase() {}

      virtual unsigned get_size(فراغ) ثابت = 0;
      virtual vpiHandle get_left_range() = 0;
      virtual vpiHandle get_right_range() = 0;
      virtual __vpiScope*get_scope() ثابت = 0;

      virtual صحيح get_word_size() ثابت = 0;
      virtual حرف*get_word_str(بنية __vpiArrayWord*word, صحيح code) = 0;
      virtual فراغ get_word_value(بنية __vpiArrayWord*word, p_vpi_value vp) = 0;
      virtual فراغ put_word_value(بنية __vpiArrayWord*word, p_vpi_value vp,
                                    صحيح flags) = 0;
      virtual vpiHandle get_iter_index(بنية __vpiArrayIterator*iter, صحيح idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_base_iterate(صحيح code);

      virtual فراغ make_vals_words();

      بنية __vpiArrayWord*vals_words;
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
      صحيح get_type_code(فراغ) ثابت override { החזר vpiMemory; }
      unsigned get_size() ثابت override { החזר array_count; }
      vpiHandle get_left_range() override { أكد(nets == 0); החזר &msb; }
      vpiHandle get_right_range() override { أكد(nets == 0); החזר &lsb; }
      __vpiScope*get_scope() ثابت override { החזר scope; }

      صحيح get_word_size() ثابت override;
      حرف*get_word_str(بنية __vpiArrayWord*word, صحيح code) override;
      فراغ get_word_value(بنية __vpiArrayWord*word, p_vpi_value vp) override;
      فراغ put_word_value(بنية __vpiArrayWord*word, p_vpi_value vp, صحيح flags) override;

      vpiHandle get_iter_index(بنية __vpiArrayIterator*iter, صحيح idx) override;

      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;
      مضمن vpiHandle vpi_iterate(صحيح code) override { החזר vpi_array_base_iterate(code); }
      vpiHandle vpi_index(صحيح idx) override;

      فراغ set_word(unsigned idx, unsigned off, ثابت vvp_vector4_t&val);
      فراغ set_word(unsigned idx, مزدوج val);
      فراغ set_word(unsigned idx, ثابت std::string&val);
      فراغ set_word(unsigned idx, ثابت vvp_object_t&val);

      vvp_vector4_t get_word(unsigned address);
      مزدوج get_word_r(unsigned address);
      فراغ get_word_obj(unsigned address, vvp_object_t&val);
      std::string get_word_str(unsigned address);

      فراغ alias_word(unsigned long addr, vpiHandle word, صحيح msb, صحيح lsb);
      فراغ attach_word(unsigned addr, vpiHandle word);
      فراغ word_change(unsigned long addr);

      ثابت حرف*name; /* Permanently allocated string */
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
      بنية __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool swap_addr;

خاص:
      unsigned array_count;
      __vpiScope*scope;

friend vpiHandle vpip_make_array(ثابت حرف*label, ثابت حرف*name,
                                 صحيح first_addr, صحيح last_addr,
                                 bool signed_flag);
friend فراغ compile_array_alias(حرف*label, حرف*name, حرف*src);
};

class __vpiDarrayVar : عام __vpiBaseVar, عام __vpiArrayBase {
    عام:
      __vpiDarrayVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح get_type_code() ثابت override { החזר vpiArrayVar; }
      unsigned get_size() ثابت override;
      vpiHandle get_left_range() override;
      vpiHandle get_right_range() override;
      __vpiScope*get_scope() ثابت override { החזר scope_; }

      صحيح get_word_size() ثابت override;
      حرف*get_word_str(بنية __vpiArrayWord*word, صحيح code) override;
      فراغ get_word_value(بنية __vpiArrayWord*word, p_vpi_value vp) override;
      فراغ put_word_value(بنية __vpiArrayWord*word, p_vpi_value vp, صحيح flags) override;

      vpiHandle get_iter_index(بنية __vpiArrayIterator*iter, صحيح idx) override;
      مضمن vpiHandle vpi_iterate(صحيح code) override { החזר vpi_array_base_iterate(code); }

      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      vpiHandle vpi_handle(صحيح code) override;
      vpiHandle vpi_index(صحيح index) override;

      فراغ vpi_get_value(p_vpi_value val) override;

    محمي:
      vvp_darray*get_vvp_darray() ثابت;
      __vpiDecConst left_range_, right_range_;
};

خارجي vpiHandle vpip_make_darray_var(ثابت حرف*name, vvp_net_t*net);

class __vpiQueueVar : عام __vpiDarrayVar {

    عام:
      __vpiQueueVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح vpi_get(صحيح code) override;
};

خارجي vpiHandle vpip_make_queue_var(ثابت حرف*name, vvp_net_t*net);

class __vpiPropQueueRef : عام __vpiHandle {

    عام:
      explicit __vpiPropQueueRef(__vpiScope*scope, unsigned pidx, bool is_queue);

      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;

      vvp_net_t* class_net_;
      unsigned prop_idx_;
      bool is_queue_;

    خاص:
      __vpiScope* scope_;
};

خارجي vpiHandle vpip_make_prop_queue_ref(حرف* class_label,
                                          unsigned prop_idx,
                                          unsigned is_queue_flag);
خارجي vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : عام __vpiBaseVar {

    عام:
      __vpiCobjectVar(__vpiScope*scope, ثابت حرف*name, vvp_net_t*net);

      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;
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
      صحيح get_type_code(فراغ) ثابت override;

      s_vpi_systf_data info;
      bool is_user_defn;
};

خارجي vpiHandle vpip_make_systf_iterator(فراغ);

خارجي بنية __vpiUserSystf* vpip_find_systf(ثابت حرف*name);


بنية __vpiSysTaskCall : عام __vpiHandle {

      __vpiScope* scope;
      بنية __vpiUserSystf*defn;
      unsigned nargs;
      vpiHandle*args;
	/* Stack consumed by this call */
      unsigned vec4_stack;
      unsigned real_stack;
      unsigned string_stack;
	/* Support for vpi_get_userdata. */
      فراغ*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      unsigned file_idx;
      unsigned lineno;
      bool put_value;
    محمي:
      مضمن __vpiSysTaskCall()
      {
	    vec4_stack = 0;
	    real_stack = 0;
	    string_stack = 0;
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

vpiHandle vpip_make_string_const(حرف*text, bool persistent =true);
vpiHandle vpip_make_string_param(ثابت حرف*name, حرف*value, bool local_flag,
                                 long file_idx, long lineno);

بنية __vpiBinaryConst : عام __vpiHandle {
      __vpiBinaryConst();
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;

      vvp_vector4_t bits;
	/* TRUE if this constant is signed. */
      unsigned signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      unsigned sized_flag  :1;
};

vpiHandle vpip_make_binary_const(unsigned wid, ثابت حرف*bits);
vpiHandle vpip_make_binary_param(ثابت حرف*name, ثابت vvp_vector4_t&bits,
				 bool signed_flag, bool local_flag,
				 long file_idx, long lineno);

class __vpiRealConst : عام __vpiHandle {
    عام:
      explicit __vpiRealConst(مزدوج);
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;

      مزدوج value;
};

vpiHandle vpip_make_real_const(مزدوج value);
vpiHandle vpip_make_real_param(ثابت حرف*name, مزدوج value, bool local_flag,
                               long file_idx, long lineno);

class __vpiNullConst : عام __vpiHandle {
    عام:
      explicit __vpiNullConst();
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_word(unsigned base, ثابت حرف*type);
vpiHandle vpip_make_vthr_str_stack(unsigned depth);
vpiHandle vpip_make_vthr_vec4_stack(unsigned depth, bool signed_flag, unsigned wid);

vpiHandle vpip_make_vthr_A(حرف*label, unsigned index);
vpiHandle vpip_make_vthr_A(حرف*label, حرف*symbol);
vpiHandle vpip_make_vthr_A(حرف*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(حرف*label, unsigned index, unsigned bit, unsigned wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
خارجي "C" فراغ vpip_load_module(ثابت حرف*name);

خارجي "C" فراغ vpip_clear_module_paths();
خارجي "C" فراغ vpip_add_module_path(ثابت حرف *path);
خارجي "C" فراغ vpip_add_env_and_default_module_paths();

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
				     صحيح val_type, unsigned return_width,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     unsigned argc,
				     vpiHandle*argv,
				     unsigned vec4_stack,
				     unsigned real_stack,
				     unsigned string_stack,
				     long file_idx,
				     long lineno);

خارجي vthread_t vpip_current_vthread;

خارجي فراغ vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_time(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

خارجي صحيح vpip_get_time_precision(فراغ);
خارجي فراغ vpip_set_time_precision(صحيح pres);

خارجي __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

خارجي صحيح vpip_time_units_from_handle(vpiHandle obj);
خارجي صحيح vpip_time_precision_from_handle(vpiHandle obj);

خارجي مزدوج vpip_scaled_time_from_handle(vvp_time64_t time, vpiHandle obj);

خارجي فراغ vpip_time_to_timestruct(بنية t_vpi_time*ts, vvp_time64_t ti);
خارجي vvp_time64_t vpip_timestruct_to_time(ثابت بنية t_vpi_time*ts);

خارجي مزدوج vpip_time_to_scaled_real(vvp_time64_t ti, ثابت __vpiScope*sc);
خارجي vvp_time64_t vpip_scaled_real_to_time64(مزدوج val, ثابت __vpiScope*sc);

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
خارجي unsigned vpip_vec4_to_dec_str(ثابت vvp_vector4_t&vec4,
				     حرف *buf, unsigned صحيح nbuf,
				     صحيح signed_flag);


خارجي فراغ vpip_vec4_to_hex_str(ثابت vvp_vector4_t&bits, حرف*buf,
				 unsigned nbuf);

خارجي فراغ vpip_vec4_to_oct_str(ثابت vvp_vector4_t&bits, حرف*buf,
				 unsigned nbuf);

خارجي فراغ vpip_bin_str_to_vec4(vvp_vector4_t&val, ثابت حرف*buf);
خارجي فراغ vpip_oct_str_to_vec4(vvp_vector4_t&val, ثابت حرف*str);
خارجي فراغ vpip_dec_str_to_vec4(vvp_vector4_t&val, ثابت حرف*str);
خارجي فراغ vpip_hex_str_to_vec4(vvp_vector4_t&val, ثابت حرف*str);

خارجي vvp_vector4_t vec4_from_vpi_value(s_vpi_value*vp, unsigned wid);
خارجي مزدوج real_from_vpi_value(s_vpi_value*vp);

خارجي فراغ vpip_vec4_get_value(ثابت vvp_vector4_t&word_val, unsigned width,
				bool signed_flag, s_vpi_value*vp);
خارجي فراغ vpip_vec2_get_value(ثابت vvp_vector2_t&word_val, unsigned width,
				bool signed_flag, s_vpi_value*vp);
خارجي فراغ vpip_real_get_value(مزدوج real, s_vpi_value*vp);
خارجي فراغ vpip_string_get_value(ثابت std::string&val, s_vpi_value*vp);

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
خارجي فراغ *need_result_buf(unsigned cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
خارجي حرف *simple_set_rbuf_str(ثابت حرف *s1);
خارجي حرف *generic_get_str(صحيح code, vpiHandle ref, ثابت حرف *name, ثابت حرف *index);

/* A routine to find the enclosing module. */
خارجي vpiHandle vpip_module(__vpiScope*scope);

خارجي صحيح vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
خارجي bool vpip_mcd0_disable;

مضمن uint64_t vlg_round_to_u64(مزدوج rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      החזר static_cast<uint64_t>(static_cast<int64_t>(std::round(rval)));
}

#endif /* IVL_vpi_priv_H */
