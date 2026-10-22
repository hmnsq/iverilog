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
#அந்தர்பாவ  "delay.h"


class class_type;
class vvp_darray;
class vvp_fun_arrayport;

வகைவரை கட்டமைப்பு __vpiArray* vvp_array_t;

/*
 * This header file contains the internal definitions that the vvp
 * program uses to implement the public interface in the vpi_user.h
 * header file elsewhere.
 */

#எனில் defined(__MINGW32__) || defined (__CYGWIN__)
வெளி vpip_routines_s vpi_routines;
#endif

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#define vpi_File_வரி    0x1000003
#define _vpiDescription 0x1000004

வெளி bool show_file_வரி;
வெளி bool code_is_instrumented;

வெளி vpiHandle vpip_build_file_வரி(மாறிலி எழுத்து*description,
                                      long file_idx, long lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
வெளி மாறிலி எழுத்து* vpi_type_as_string(PLI_INT32 code);

/*
 * Return the port direction as a string.
 */
வெளி மாறிலி எழுத்து* direction_as_string(முழு dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#எனில் defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
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
எண்ணிக்கை vpi_mode_t {
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
வெளி vpi_mode_t vpi_mode_flag;

/*
 * This structure is the very base of a vpiHandle. Every handle
 * structure is derived from this class so that the library can
 * internally pass the derived types as pointers to one of these.
 */
class __vpiHandle {
    பொது:
      உள்வரி __vpiHandle() { }
	// The destructor is virtual so that dynamic types will work.
      மெய்நிகர் ~__vpiHandle();

      மெய்நிகர் முழு get_type_code(வெறுமை) மாறிலி =0;
      மெய்நிகர் முழு vpi_get(முழு code);
      மெய்நிகர் int64_t vpi_get64(முழு code);
      மெய்நிகர் எழுத்து* vpi_get_str(முழு code);

      மெய்நிகர் வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val);
      மெய்நிகர் vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags);
      மெய்நிகர் vpiHandle vpi_handle(முழு code);
      மெய்நிகர் vpiHandle vpi_iterate(முழு code);
      மெய்நிகர் vpiHandle vpi_index(முழு idx);
      மெய்நிகர் வெறுமை vpi_get_delays(p_vpi_delay del);
      மெய்நிகர் வெறுமை vpi_put_delays(p_vpi_delay del);

	// Objects may have destroyer functions of their own. If so,
	// then this virtual method will return a POINTER to that
	// function. The pointer is used to "delete" the object, which
	// is why the function itself cannot be a method.
      வகைவரை முழு (*free_object_fun_t)(vpiHandle);
      மெய்நிகர் free_object_fun_t free_object_fun(வெறுமை);
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
கட்டமைப்பு __vpiIterator : பொது __vpiHandle {
      __vpiIterator();
      முழு get_type_code(வெறுமை) மாறிலி override;
      free_object_fun_t free_object_fun(வெறுமை) override;

      vpiHandle *args;
      unsigned  nargs;
      unsigned  next;
      bool free_args_flag;
};

வெளி vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
				    bool free_args_flag);

class __vpiDecConst : பொது __vpiHandle {
    பொது:
      explicit __vpiDecConst(முழு val =0);
      __vpiDecConst(மாறிலி __vpiDecConst&that);
# எனில் __cplusplus >= 201103L
      __vpiDecConst& operator = (மாறிலி __vpiDecConst&that) = இயல்பு;
# endif
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;

    பொது:
      உள்வரி முழு get_மதிப்பு() மாறிலி { திரும்பு மதிப்பு; }
      உள்வரி வெறுமை set_மதிப்பு(முழு val) { மதிப்பு = val; }

    தனிப்பட்ட:
      முழு மதிப்பு;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
கட்டமைப்பு __vpiCallback : பொது __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      முழு get_type_code(வெறுமை) மாறிலி override;

	// Used for listing callbacks.
      கட்டமைப்பு __vpiCallback*next;

	// user supplied callback data
      கட்டமைப்பு t_cb_data cb_data;
};

class மதிப்பு_callback : பொது __vpiCallback {
    பொது:
      explicit மதிப்பு_callback(p_cb_data data);
	// Return true if the callback really is ready to be called
      மெய்நிகர் bool test_மதிப்பு_callback_ready(வெறுமை);

    பொது:
	// user supplied callback data
      கட்டமைப்பு t_vpi_time cb_time;
      கட்டமைப்பு t_vpi_மதிப்பு cb_மதிப்பு;
};

வெளி வெறுமை callback_execute(கட்டமைப்பு __vpiCallback*cur);

கட்டமைப்பு __vpiSystemTime : பொது __vpiHandle {
      __vpiSystemTime();
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      எழுத்து*vpi_get_str(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
      vpiHandle vpi_handle(முழு code) override;

      __vpiScope*scope;
};

கட்டமைப்பு __vpiScopedTime : பொது __vpiSystemTime {
      __vpiScopedTime();
      எழுத்து*vpi_get_str(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
};
கட்டமைப்பு __vpiScopedSTime : பொது __vpiSystemTime {
      __vpiScopedSTime();
      முழு vpi_get(முழு code) override;
      எழுத்து*vpi_get_str(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
};
கட்டமைப்பு __vpiScopedRealtime : பொது __vpiSystemTime {
      __vpiScopedRealtime();
      முழு vpi_get(முழு code) override;
      எழுத்து*vpi_get_str(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : பொது __vpiHandle {

    பொது:
      முழு vpi_get(முழு code) override;
      எழுத்து* vpi_get_str(முழு code) override;
      vpiHandle vpi_handle(முழு code) override;
      vpiHandle vpi_iterate(முழு code) override;

    பொது:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      உள்வரி மாறிலி எழுத்து*scope_name() மாறிலி { திரும்பு name_; }

      உள்வரி மாறிலி எழுத்து*scope_def_name() மாறிலி { திரும்பு tname_; }
	// TRUE if this is an automatic func/task/block
      உள்வரி bool is_automatic() மாறிலி { திரும்பு is_automatic_; }

    பொது:
      __vpiScope *scope;
      unsigned file_idx;
      unsigned lineno;
      unsigned def_file_idx;
      unsigned def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_time;
      கட்டமைப்பு __vpiScopedSTime scoped_stime;
      கட்டமைப்பு __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::vector<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      கட்டமைப்பு automatic_hooks_s**item;
      unsigned nitem;
        /* Keep a list of live contexts. */
      vvp_context_t live_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::set<vthread_t> threads;
      signed முழு time_units :8;
      signed முழு time_precision :8;

    பாதுகாக்கப்பட்ட:
      __vpiScope(மாறிலி எழுத்து*nam, மாறிலி எழுத்து*tnam, bool is_auto_flag =false);

    தனிப்பட்ட:
	/* The scope has a name. */
      மாறிலி எழுத்து*name_;
      மாறிலி எழுத்து*tname_;
	/* the scope may be "automatic" */
      bool is_automatic_;
};

class vpiScopeFunction  : பொது __vpiScope {
    பொது:
      உள்வரி vpiScopeFunction(மாறிலி எழுத்து*nam, மாறிலி எழுத்து*tnam,
			      bool auto_flag, முழு func_type, unsigned func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiFunction; }
      முழு vpi_get(முழு code) override
      {
	    தேர்வு (code) {
		நிலை vpiFuncType:
		  திரும்பு func_type_;
		இயல்பு:
		  திரும்பு __vpiScope::vpi_get(code);
	    }
      }

    பொது:
      உள்வரி unsigned get_func_width(வெறுமை) மாறிலி { திரும்பு func_wid_; }
      உள்வரி vvp_bit4_t get_func_init_val(வெறுமை) மாறிலி { திரும்பு func_init_val_; }

    தனிப்பட்ட:
      முழு func_type_;
      unsigned func_wid_;
      vvp_bit4_t func_init_val_;
};

வெளி __vpiScope* vpip_peek_current_scope(வெறுமை);
வெளி வெறுமை vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj);
வெளி வெறுமை vpip_attach_to_current_scope(vpiHandle obj);
வெளி __vpiScope* vpip_peek_context_scope(வெறுமை);
வெளி unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                         __vpiScope*scope);
வெளி vpiHandle vpip_make_வேர்_iterator(முழு type_code);
வெளி வெறுமை vpip_make_வேர்_iterator(class __vpiHandle**&table,
				    unsigned&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
கட்டமைப்பு __vpiSignal : பொது __vpiHandle {
      முழு vpi_get(முழு code) override;
      int64_t vpi_get64(முழு code) override;
      எழுத்து* vpi_get_str(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
      vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags) override;
      vpiHandle vpi_handle(முழு code) override;
      vpiHandle vpi_iterate(முழு code) override;
      vpiHandle vpi_index(முழு idx) override;

    பொது:
      unsigned width() மாறிலி;
      vpiHandle get_index(முழு index);
      வெறுமை get_bit_மதிப்பு(மாறிலி கட்டமைப்பு __vpiBit*bit, p_vpi_மதிப்பு vp);
      vpiHandle put_bit_மதிப்பு(கட்டமைப்பு __vpiBit*bit, p_vpi_மதிப்பு vp, முழு flags);
      வெறுமை make_bits();

      கட்டமைப்பு __vpiBit*bits;

    பொது:
      ஒன்றிப்பு { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
      ஒன்றிப்பு { // The name of this reg/net, or the index for array words.
            மாறிலி எழுத்து*name;
            vpiHandle index;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      unsigned signed_flag  : 1;
      unsigned is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*முனை;

    பொது:
      நிலையான வெறுமை*operator new(std::size_t size);
      நிலையான வெறுமை operator delete(வெறுமை*); // not implemented
    பாதுகாக்கப்பட்ட:
      உள்வரி __vpiSignal() : bits(வெற்று) { }
    தனிப்பட்ட: // Not implemented
      நிலையான வெறுமை*operator new[] (std::size_t size);
      நிலையான வெறுமை operator delete[](வெறுமை*);
};
வெளி unsigned vpip_size(மாறிலி __vpiSignal *sig);
வெளி __vpiScope* vpip_scope(__vpiSignal*sig);

வெளி vpiHandle vpip_make_int2(மாறிலி எழுத்து*name, முழு msb, முழு lsb,
			       bool signed_flag, vvp_net_t*vec);
வெளி vpiHandle vpip_make_int4(மாறிலி எழுத்து*name, முழு msb, முழு lsb,
			       vvp_net_t*vec);
வெளி vpiHandle vpip_make_var4(மாறிலி எழுத்து*name, முழு msb, முழு lsb,
			       bool signed_flag, vvp_net_t*net);
வெளி vpiHandle vpip_make_net4(__vpiScope*scope,
				மாறிலி எழுத்து*name, முழு msb, முழு lsb,
				bool signed_flag, vvp_net_t*முனை);

/*
 * This is used to represent a bit in a net/reg.
 */
கட்டமைப்பு __vpiBit {
      கட்டமைப்பு as_bit_t : பொது __vpiHandle {
	    முழு get_type_code(வெறுமை) மாறிலி;
	    முழு vpi_get(முழு code);
	    எழுத்து* vpi_get_str(முழு code);
	    வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val);
	    vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags);
	    vpiHandle vpi_handle(முழு code);
      } as_bit;

      vpiHandle index;

      ஒன்றிப்பு {
	    கட்டமைப்பு __vpiSignal*parent;
	    கட்டமைப்பு __vpiBit*bit0;
      };

      உள்வரி unsigned get_norm_index() மாறிலி { திரும்பு this - bit0; }
      உள்வரி கட்டமைப்பு __vpiSignal*get_parent() மாறிலி {திரும்பு (bit0 - 1)->parent; }
      முழு get_index(வெறுமை) மாறிலி;
};

class vpiPortBitInfo;

class vpiPortInfo  : பொது __vpiHandle {
    பொது:
      vpiPortInfo( __vpiScope *parent,
                    unsigned index,
                    முழு vpi_direction,
                    unsigned width,
                    மாறிலி எழுத்து *name,
                    எழுத்து* buffer );
      ~vpiPortInfo() override;

      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiPort; }
      முழு get_direction(வெறுமை) { திரும்பு direction_; }
      unsigned get_index(வெறுமை) { திரும்பு index_; }
      முழு get_width(வெறுமை) { திரும்பு width_; }
      வெறுமை add_port_bit(vpiPortBitInfo* port_bit) { port_bits_.தள்ளு_back(port_bit); }

      முழு vpi_get(முழு code) override;
      எழுத்து* vpi_get_str(முழு code) override;
      vpiHandle vpi_handle(முழு code) override;
      vvp_net_t* get_port(வெறுமை) மாறிலி { திரும்பு ref_; }
      vpiHandle vpi_iterate(முழு code) override;

      std::vector<vpiPortBitInfo*> port_bits_;

    தனிப்பட்ட:
      __vpiScope *parent_;
      unsigned  index_;
      முழு       direction_;
      unsigned  width_;
      மாறிலி எழுத்து *name_;
      vvp_net_t *ref_;
};

class vpiPortBitInfo  : பொது __vpiHandle {
    பொது:
      vpiPortBitInfo(vpiPortInfo *parent,
                     unsigned bit);
      ~vpiPortBitInfo() override;

      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiPortBit; }
      unsigned get_bit(வெறுமை) மாறிலி { திரும்பு bit_; }

      முழு vpi_get(முழு code) override;
      vpiHandle vpi_handle(முழு code) override;

    தனிப்பட்ட:
      vpiPortInfo *parent_;
      unsigned bit_;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
கட்டமைப்பு __vpiPV : பொது __vpiHandle {
      __vpiPV();
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      எழுத்து* vpi_get_str(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
      vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags) override;
      vpiHandle vpi_handle(முழு code) override;

      vpiHandle parent;
      vvp_net_t*net;
      vpiHandle sbase;
      முழு tbase;
      unsigned width;
};
வெளி vpiHandle vpip_make_PV(எழுத்து*name, முழு base, முழு width);
வெளி vpiHandle vpip_make_PV(எழுத்து*name, எழுத்து*குறியீடு, முழு width);
வெளி vpiHandle vpip_make_PV(எழுத்து*name, vpiHandle handle, முழு width);

கட்டமைப்பு __vpiModPathTerm : பொது __vpiHandle {
      __vpiModPathTerm();
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      vpiHandle vpi_handle(முழு code) override;

      vpiHandle கோவை;
	/* The value returned by vpi_get(vpiEdge, ...); */
      முழு edge;
};

கட்டமைப்பு __vpiModPathSrc : பொது __vpiHandle {
      __vpiModPathSrc();
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
      vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags) override;
      vpiHandle vpi_handle(முழு code) override;
      vpiHandle vpi_iterate(முழு code) override;
      vpiHandle vpi_index(முழு idx) override;
      வெறுமை vpi_get_delays(p_vpi_delay del) override;
      வெறுமை vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(வெறுமை) override;

      கட்டமைப்பு __vpiModPath *dest;
      முழு   type;

	/* This is the input expression for this modpath. */
      கட்டமைப்பு __vpiModPathTerm path_term_in;

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

கட்டமைப்பு __vpiModPath {
      __vpiScope   *scope ;

      class vvp_fun_modpath*modpath;

      கட்டமைப்பு __vpiModPathTerm path_term_out;
      vvp_net_t *உள்ளீடு_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

வெளி கட்டமைப்பு __vpiModPathSrc* vpip_make_modpath_src(கட்டமைப்பு __vpiModPath*path,
                                                     vvp_net_t *net) ;

வெளி கட்டமைப்பு __vpiModPath* vpip_make_modpath(vvp_net_t *net) ;

/*
 *
 * The vpiInterModPath vpiHandle will define
 * a vpiInterModPath of record .intermodpath as defined
 * in the IEEE 1364
 *
 */

கட்டமைப்பு __vpiInterModPath : பொது __vpiHandle {
      __vpiInterModPath();
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
      vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags) override;
      vpiHandle vpi_handle(முழு code) override;
      vpiHandle vpi_iterate(முழு code) override;
      வெறுமை vpi_get_delays(p_vpi_delay del) override;
      வெறுமை vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(வெறுமை) override;

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

வெளி கட்டமைப்பு __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2);

/*
 * These methods support the vpi creation of events. The name string
 * passed in will be saved, so the caller must allocate it (or not
 * free it) after it is handed to this function.
 */
class __vpiNamedEvent : பொது __vpiHandle {

    பொது:
      __vpiNamedEvent(__vpiScope*scope, மாறிலி எழுத்து*name);
      ~__vpiNamedEvent() override;

      __vpiNamedEvent(மாறிலி __vpiNamedEvent&) = delete;
      __vpiNamedEvent& operator=(மாறிலி __vpiNamedEvent&) = delete;

      முழு get_type_code(வெறுமை) மாறிலி override;
      __vpiScope*get_scope(வெறுமை) மாறிலி { திரும்பு scope_; }
      முழு vpi_get(முழு code) override;
      எழுத்து* vpi_get_str(முழு code) override;
      vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags) override;
      vpiHandle vpi_handle(முழு code) override;

      உள்வரி வெறுமை add_vpi_callback(__vpiCallback*cb)
      { cb->next = callbacks_;
	callbacks_ = cb;
      }

      வெறுமை run_vpi_callbacks(வெறுமை);

	/* The functor, used for %set operations. */
      vvp_net_t*funct;

    தனிப்பட்ட:
	/* base name of the event object */
      மாறிலி எழுத்து*name_;
	/* Parent scope of this object. */
      __vpiScope*scope_;
	/* List of callbacks interested in this event. */
      __vpiCallback*callbacks_;
};

வெளி vpiHandle vpip_make_named_event(மாறிலி எழுத்து*name, vvp_net_t*f);

/*
 * Memory is an array of bits that is accessible in N-bit chunks, with
 * N being the width of a word. The memory word handle just points
 * back to the memory and uses an index to identify its position in
 * the memory.
 */

வெளி bool is_net_array(vpiHandle obj);

/*
 * These are the various variable types.
 */
கட்டமைப்பு __vpiRealVar : பொது __vpiHandle {
      __vpiRealVar();
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      எழுத்து* vpi_get_str(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
      vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags) override;
      vpiHandle vpi_handle(முழு code) override;
      vpiHandle vpi_iterate(முழு code) override;

      ஒன்றிப்பு { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      ஒன்றிப்பு {
            மாறிலி எழுத்து*name;
            vpiHandle index;
      } id;
      unsigned is_netarray  : 1; // This is word of a net array
      unsigned is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

வெளி __vpiScope* vpip_scope(__vpiRealVar*sig);
வெளி vpiHandle vpip_make_real_var(மாறிலி எழுத்து*name, vvp_net_t*net);
வெளி vpiHandle vpip_make_real_net(__vpiScope*scope,
				    மாறிலி எழுத்து*name, vvp_net_t*net);

class __vpiBaseVar : பொது __vpiHandle {

    பொது:
      __vpiBaseVar(__vpiScope*scope, மாறிலி எழுத்து*name, vvp_net_t*net);
#ifdef CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#endif

      உள்வரி vvp_net_t* get_net() மாறிலி { திரும்பு net_; }

    பாதுகாக்கப்பட்ட:
      __vpiScope* scope_;
      மாறிலி எழுத்து*name_;

    தனிப்பட்ட:
      vvp_net_t*net_;
};

class __vpiStringVar : பொது __vpiBaseVar {

    பொது:
      __vpiStringVar(__vpiScope*scope, மாறிலி எழுத்து*name, vvp_net_t*net);

      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
      vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags) override;
};

வெளி vpiHandle vpip_make_string_var(மாறிலி எழுத்து*name, vvp_net_t*net);

கட்டமைப்பு __vpiArrayBase {
      __vpiArrayBase() : vals_சொற்கள்(வெற்று) {}
      மெய்நிகர் ~__vpiArrayBase() {}

      மெய்நிகர் unsigned get_size(வெறுமை) மாறிலி = 0;
      மெய்நிகர் vpiHandle get_இடது_range() = 0;
      மெய்நிகர் vpiHandle get_வலது_range() = 0;
      மெய்நிகர் __vpiScope*get_scope() மாறிலி = 0;

      மெய்நிகர் முழு get_word_size() மாறிலி = 0;
      மெய்நிகர் எழுத்து*get_word_str(கட்டமைப்பு __vpiArrayWord*word, முழு code) = 0;
      மெய்நிகர் வெறுமை get_word_மதிப்பு(கட்டமைப்பு __vpiArrayWord*word, p_vpi_மதிப்பு vp) = 0;
      மெய்நிகர் வெறுமை put_word_மதிப்பு(கட்டமைப்பு __vpiArrayWord*word, p_vpi_மதிப்பு vp,
                                    முழு flags) = 0;
      மெய்நிகர் vpiHandle get_iter_index(கட்டமைப்பு __vpiArrayIterator*iter, முழு idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_base_iterate(முழு code);

      மெய்நிகர் வெறுமை make_vals_சொற்கள்();

      கட்டமைப்பு __vpiArrayWord*vals_சொற்கள்;
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
கட்டமைப்பு __vpiArray : பொது __vpiArrayBase, பொது __vpiHandle {
      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiMemory; }
      unsigned get_size() மாறிலி override { திரும்பு array_மொத்தம்; }
      vpiHandle get_இடது_range() override { ப்ரதிஜ்ஞா(nets == 0); திரும்பு &msb; }
      vpiHandle get_வலது_range() override { ப்ரதிஜ்ஞா(nets == 0); திரும்பு &lsb; }
      __vpiScope*get_scope() மாறிலி override { திரும்பு scope; }

      முழு get_word_size() மாறிலி override;
      எழுத்து*get_word_str(கட்டமைப்பு __vpiArrayWord*word, முழு code) override;
      வெறுமை get_word_மதிப்பு(கட்டமைப்பு __vpiArrayWord*word, p_vpi_மதிப்பு vp) override;
      வெறுமை put_word_மதிப்பு(கட்டமைப்பு __vpiArrayWord*word, p_vpi_மதிப்பு vp, முழு flags) override;

      vpiHandle get_iter_index(கட்டமைப்பு __vpiArrayIterator*iter, முழு idx) override;

      முழு vpi_get(முழு code) override;
      எழுத்து* vpi_get_str(முழு code) override;
      vpiHandle vpi_handle(முழு code) override;
      உள்வரி vpiHandle vpi_iterate(முழு code) override { திரும்பு vpi_array_base_iterate(code); }
      vpiHandle vpi_index(முழு idx) override;

      வெறுமை set_word(unsigned idx, unsigned off, மாறிலி vvp_vector4_t&val);
      வெறுமை set_word(unsigned idx, இரட்டை val);
      வெறுமை set_word(unsigned idx, மாறிலி std::string&val);
      வெறுமை set_word(unsigned idx, மாறிலி vvp_object_t&val);

      vvp_vector4_t get_word(unsigned address);
      இரட்டை get_word_r(unsigned address);
      வெறுமை get_word_obj(unsigned address, vvp_object_t&val);
      std::string get_word_str(unsigned address);

      வெறுமை alias_word(unsigned long addr, vpiHandle word, முழு msb, முழு lsb);
      வெறுமை attach_word(unsigned addr, vpiHandle word);
      வெறுமை word_change(unsigned long addr);

      மாறிலி எழுத்து*name; /* Permanently allocated string */
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
      கட்டமைப்பு __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool swap_addr;

தனிப்பட்ட:
      unsigned array_மொத்தம்;
      __vpiScope*scope;

friend vpiHandle vpip_make_array(மாறிலி எழுத்து*label, மாறிலி எழுத்து*name,
                                 முழு first_addr, முழு last_addr,
                                 bool signed_flag);
friend வெறுமை compile_array_alias(எழுத்து*label, எழுத்து*name, எழுத்து*src);
};

class __vpiDarrayVar : பொது __vpiBaseVar, பொது __vpiArrayBase {
    பொது:
      __vpiDarrayVar(__vpiScope*scope, மாறிலி எழுத்து*name, vvp_net_t*net);

      முழு get_type_code() மாறிலி override { திரும்பு vpiArrayVar; }
      unsigned get_size() மாறிலி override;
      vpiHandle get_இடது_range() override;
      vpiHandle get_வலது_range() override;
      __vpiScope*get_scope() மாறிலி override { திரும்பு scope_; }

      முழு get_word_size() மாறிலி override;
      எழுத்து*get_word_str(கட்டமைப்பு __vpiArrayWord*word, முழு code) override;
      வெறுமை get_word_மதிப்பு(கட்டமைப்பு __vpiArrayWord*word, p_vpi_மதிப்பு vp) override;
      வெறுமை put_word_மதிப்பு(கட்டமைப்பு __vpiArrayWord*word, p_vpi_மதிப்பு vp, முழு flags) override;

      vpiHandle get_iter_index(கட்டமைப்பு __vpiArrayIterator*iter, முழு idx) override;
      உள்வரி vpiHandle vpi_iterate(முழு code) override { திரும்பு vpi_array_base_iterate(code); }

      முழு vpi_get(முழு code) override;
      எழுத்து* vpi_get_str(முழு code) override;
      vpiHandle vpi_handle(முழு code) override;
      vpiHandle vpi_index(முழு index) override;

      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;

    பாதுகாக்கப்பட்ட:
      vvp_darray*get_vvp_darray() மாறிலி;
      __vpiDecConst இடது_range, வலது_range;
};

வெளி vpiHandle vpip_make_darray_var(மாறிலி எழுத்து*name, vvp_net_t*net);

class __vpiQueueVar : பொது __vpiDarrayVar {

    பொது:
      __vpiQueueVar(__vpiScope*scope, மாறிலி எழுத்து*name, vvp_net_t*net);

      முழு vpi_get(முழு code) override;
};

வெளி vpiHandle vpip_make_queue_var(மாறிலி எழுத்து*name, vvp_net_t*net);

class __vpiPropQueueRef : பொது __vpiHandle {

    பொது:
      explicit __vpiPropQueueRef(__vpiScope*scope, unsigned pidx, bool is_queue);

      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      எழுத்து* vpi_get_str(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;

      vvp_net_t* class_net_;
      unsigned prop_idx_;
      bool is_queue_;

    தனிப்பட்ட:
      __vpiScope* scope_;
};

வெளி vpiHandle vpip_make_prop_queue_ref(எழுத்து* class_label,
                                          unsigned prop_idx,
                                          unsigned is_queue_flag);
வெளி vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : பொது __vpiBaseVar {

    பொது:
      __vpiCobjectVar(__vpiScope*scope, மாறிலி எழுத்து*name, vvp_net_t*net);

      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
};

வெளி vpiHandle vpip_make_cobject_var(மாறிலி எழுத்து*name, vvp_net_t*net);

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
கட்டமைப்பு __vpiUserSystf : பொது __vpiHandle {
      __vpiUserSystf();
      முழு get_type_code(வெறுமை) மாறிலி override;

      s_vpi_systf_data info;
      bool is_user_defn;
};

வெளி vpiHandle vpip_make_systf_iterator(வெறுமை);

வெளி கட்டமைப்பு __vpiUserSystf* vpip_find_systf(மாறிலி எழுத்து*name);


கட்டமைப்பு __vpiSysTaskCall : பொது __vpiHandle {

      __vpiScope* scope;
      கட்டமைப்பு __vpiUserSystf*defn;
      unsigned nargs;
      vpiHandle*args;
	/* Stack consumed by this call */
      unsigned vec4_அடுக்கு;
      unsigned real_அடுக்கு;
      unsigned string_அடுக்கு;
	/* Support for vpi_get_userdata. */
      வெறுமை*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      unsigned file_idx;
      unsigned lineno;
      bool put_மதிப்பு;
    பாதுகாக்கப்பட்ட:
      உள்வரி __vpiSysTaskCall()
      {
	    vec4_அடுக்கு = 0;
	    real_அடுக்கு = 0;
	    string_அடுக்கு = 0;
      }
};

வெளி கட்டமைப்பு __vpiSysTaskCall*vpip_cur_task;

/*
 * The persistent flag to vpip_make_string_const causes the created
 * handle to be persistent. This is necessary for cases where the
 * string handle may be reused, which is the normal case.
 *
 * When constructing with a string, the class takes possession of the
 * text value string, and will delete it in the constructor.
 */

vpiHandle vpip_make_string_const(எழுத்து*text, bool persistent =true);
vpiHandle vpip_make_string_param(மாறிலி எழுத்து*name, எழுத்து*மதிப்பு, bool local_flag,
                                 long file_idx, long lineno);

கட்டமைப்பு __vpiBinaryConst : பொது __vpiHandle {
      __vpiBinaryConst();
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;

      vvp_vector4_t bits;
	/* TRUE if this constant is signed. */
      unsigned signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      unsigned sized_flag  :1;
};

vpiHandle vpip_make_இருமம்_const(unsigned wid, மாறிலி எழுத்து*bits);
vpiHandle vpip_make_இருமம்_param(மாறிலி எழுத்து*name, மாறிலி vvp_vector4_t&bits,
				 bool signed_flag, bool local_flag,
				 long file_idx, long lineno);

class __vpiRealConst : பொது __vpiHandle {
    பொது:
      explicit __vpiRealConst(இரட்டை);
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;

      இரட்டை மதிப்பு;
};

vpiHandle vpip_make_real_const(இரட்டை மதிப்பு);
vpiHandle vpip_make_real_param(மாறிலி எழுத்து*name, இரட்டை மதிப்பு, bool local_flag,
                               long file_idx, long lineno);

class __vpiNullConst : பொது __vpiHandle {
    பொது:
      explicit __vpiNullConst();
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_word(unsigned base, மாறிலி எழுத்து*type);
vpiHandle vpip_make_vthr_str_அடுக்கு(unsigned depth);
vpiHandle vpip_make_vthr_vec4_அடுக்கு(unsigned depth, bool signed_flag, unsigned wid);

vpiHandle vpip_make_vthr_A(எழுத்து*label, unsigned index);
vpiHandle vpip_make_vthr_A(எழுத்து*label, எழுத்து*குறியீடு);
vpiHandle vpip_make_vthr_A(எழுத்து*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(எழுத்து*label, unsigned index, unsigned bit, unsigned wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
வெளி "C" வெறுமை vpip_load_module(மாறிலி எழுத்து*name);

வெளி "C" வெறுமை vpip_clear_module_paths();
வெளி "C" வெறுமை vpip_add_module_path(மாறிலி எழுத்து *path);
வெளி "C" வெறுமை vpip_add_env_and_default_module_paths();

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
வெளி vpiHandle vpip_build_vpi_call(மாறிலி எழுத்து*name,
				     முழு val_type, unsigned return_width,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     unsigned argc,
				     vpiHandle*argv,
				     unsigned vec4_அடுக்கு,
				     unsigned real_அடுக்கு,
				     unsigned string_அடுக்கு,
				     long file_idx,
				     long lineno);

வெளி vthread_t vpip_current_vthread;

வெளி வெறுமை vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_time(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

வெளி முழு vpip_get_time_precision(வெறுமை);
வெளி வெறுமை vpip_set_time_precision(முழு pres);

வெளி __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

வெளி முழு vpip_time_units_from_handle(vpiHandle obj);
வெளி முழு vpip_time_precision_from_handle(vpiHandle obj);

வெளி இரட்டை vpip_scaled_time_from_handle(vvp_time64_t time, vpiHandle obj);

வெளி வெறுமை vpip_time_to_timestruct(கட்டமைப்பு t_vpi_time*ts, vvp_time64_t ti);
வெளி vvp_time64_t vpip_timestruct_to_time(மாறிலி கட்டமைப்பு t_vpi_time*ts);

வெளி இரட்டை vpip_time_to_scaled_real(vvp_time64_t ti, மாறிலி __vpiScope*sc);
வெளி vvp_time64_t vpip_scaled_real_to_time64(இரட்டை val, மாறிலி __vpiScope*sc);

/*
 * These functions are used mostly as compile time to strings into
 * permallocated memory. The vpip_string function is the most general,
 * it allocates a fresh string no matter what. The vpip_name_string
 * allocates a string and keeps a pointer in the hash, and tries to
 * reuse it if it can. This us useful for handle names, which may be
 * reused in different scopes.
 */
வெளி மாறிலி எழுத்து* vpip_string(மாறிலி எழுத்து*str);
வெளி மாறிலி எழுத்து* vpip_name_string(மாறிலி எழுத்து*str);


/*
 * This function is used to make decimal string versions of various
 * vectors. The input format is a vvp_vector4_t, and the result is
 * written into buf, without overflowing nbuf.
 */
வெளி unsigned vpip_vec4_to_dec_str(மாறிலி vvp_vector4_t&vec4,
				     எழுத்து *buf, unsigned முழு nbuf,
				     முழு signed_flag);


வெளி வெறுமை vpip_vec4_to_hex_str(மாறிலி vvp_vector4_t&bits, எழுத்து*buf,
				 unsigned nbuf);

வெளி வெறுமை vpip_vec4_to_oct_str(மாறிலி vvp_vector4_t&bits, எழுத்து*buf,
				 unsigned nbuf);

வெளி வெறுமை vpip_bin_str_to_vec4(vvp_vector4_t&val, மாறிலி எழுத்து*buf);
வெளி வெறுமை vpip_oct_str_to_vec4(vvp_vector4_t&val, மாறிலி எழுத்து*str);
வெளி வெறுமை vpip_dec_str_to_vec4(vvp_vector4_t&val, மாறிலி எழுத்து*str);
வெளி வெறுமை vpip_hex_str_to_vec4(vvp_vector4_t&val, மாறிலி எழுத்து*str);

வெளி vvp_vector4_t vec4_from_vpi_மதிப்பு(s_vpi_மதிப்பு*vp, unsigned wid);
வெளி இரட்டை real_from_vpi_மதிப்பு(s_vpi_மதிப்பு*vp);

வெளி வெறுமை vpip_vec4_get_மதிப்பு(மாறிலி vvp_vector4_t&word_val, unsigned width,
				bool signed_flag, s_vpi_மதிப்பு*vp);
வெளி வெறுமை vpip_vec2_get_மதிப்பு(மாறிலி vvp_vector2_t&word_val, unsigned width,
				bool signed_flag, s_vpi_மதிப்பு*vp);
வெளி வெறுமை vpip_real_get_மதிப்பு(இரட்டை real, s_vpi_மதிப்பு*vp);
வெளி வெறுமை vpip_string_get_மதிப்பு(மாறிலி std::string&val, s_vpi_மதிப்பு*vp);

/*
 * Function defined in vpi_signal.cc to manage vpi_get_* persistent
 * storage.
 */
எண்ணிக்கை vpi_rbuf_t {
      RBUF_VAL =0,
	/* Storage for *_get_value() */
      RBUF_STR,
	/* Storage for *_get_str() */
      RBUF_DEL
	/* Delete the storage for both buffers. */
};
வெளி வெறுமை *need_result_buf(unsigned cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
வெளி எழுத்து *simple_set_rbuf_str(மாறிலி எழுத்து *s1);
வெளி எழுத்து *generic_get_str(முழு code, vpiHandle ref, மாறிலி எழுத்து *name, மாறிலி எழுத்து *index);

/* A routine to find the enclosing module. */
வெளி vpiHandle vpip_module(__vpiScope*scope);

வெளி முழு vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
வெளி bool vpip_mcd0_disable;

உள்வரி uint64_t vlg_round_to_u64(இரட்டை rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      திரும்பு static_cast<uint64_t>(static_cast<int64_t>(std::round(rval)));
}

#endif /* IVL_vpi_priv_H */
