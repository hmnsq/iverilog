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
#അന്തര്ഭാവ  "delay.h"


class class_type;
class vvp_darray;
class vvp_fun_arrayport;

തരനിർവചനം ഘടന __vpiArray* vvp_array_t;

/*
 * This header file contains the internal definitions that the vvp
 * program uses to implement the public interface in the vpi_user.h
 * header file elsewhere.
 */

#എങ്കിൽ defined(__MINGW32__) || defined (__CYGWIN__)
ബാഹ്യ vpip_routines_s vpi_routines;
#endif

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#define vpi_File_വരി    0x1000003
#define _vpiDescription 0x1000004

ബാഹ്യ bool show_file_വരി;
ബാഹ്യ bool code_is_instrumented;

ബാഹ്യ vpiHandle vpip_build_file_വരി(സ്ഥിരം അക്ഷരം*description,
                                      long file_idx, long lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
ബാഹ്യ സ്ഥിരം അക്ഷരം* vpi_type_as_string(PLI_INT32 code);

/*
 * Return the port direction as a string.
 */
ബാഹ്യ സ്ഥിരം അക്ഷരം* direction_as_string(പൂർണ്ണസംഖ്യ dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#എങ്കിൽ defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
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
എണ്ണൽ vpi_mode_t {
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
ബാഹ്യ vpi_mode_t vpi_mode_flag;

/*
 * This structure is the very base of a vpiHandle. Every handle
 * structure is derived from this class so that the library can
 * internally pass the derived types as pointers to one of these.
 */
class __vpiHandle {
    പൊതു:
      വരിയിൽ __vpiHandle() { }
	// The destructor is virtual so that dynamic types will work.
      മായാ ~__vpiHandle();

      മായാ പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം =0;
      മായാ പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code);
      മായാ int64_t vpi_get64(പൂർണ്ണസംഖ്യ code);
      മായാ അക്ഷരം* vpi_get_str(പൂർണ്ണസംഖ്യ code);

      മായാ ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val);
      മായാ vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags);
      മായാ vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code);
      മായാ vpiHandle vpi_iterate(പൂർണ്ണസംഖ്യ code);
      മായാ vpiHandle vpi_index(പൂർണ്ണസംഖ്യ idx);
      മായാ ശൂന്യം vpi_get_delays(p_vpi_delay del);
      മായാ ശൂന്യം vpi_put_delays(p_vpi_delay del);

	// Objects may have destroyer functions of their own. If so,
	// then this virtual method will return a POINTER to that
	// function. The pointer is used to "delete" the object, which
	// is why the function itself cannot be a method.
      തരനിർവചനം പൂർണ്ണസംഖ്യ (*free_object_fun_t)(vpiHandle);
      മായാ free_object_fun_t free_object_fun(ശൂന്യം);
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
ഘടന __vpiIterator : പൊതു __vpiHandle {
      __vpiIterator();
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      free_object_fun_t free_object_fun(ശൂന്യം) override;

      vpiHandle *args;
      unsigned  nargs;
      unsigned  next;
      bool free_args_flag;
};

ബാഹ്യ vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
				    bool free_args_flag);

class __vpiDecConst : പൊതു __vpiHandle {
    പൊതു:
      explicit __vpiDecConst(പൂർണ്ണസംഖ്യ val =0);
      __vpiDecConst(സ്ഥിരം __vpiDecConst&that);
# എങ്കിൽ __cplusplus >= 201103L
      __vpiDecConst& operator = (സ്ഥിരം __vpiDecConst&that) = സ്ഥിരസ്ഥിതി;
# endif
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;

    പൊതു:
      വരിയിൽ പൂർണ്ണസംഖ്യ get_മൂല്യം() സ്ഥിരം { തിരികെ മൂല്യം; }
      വരിയിൽ ശൂന്യം set_മൂല്യം(പൂർണ്ണസംഖ്യ val) { മൂല്യം = val; }

    സ്വകാര്യ:
      പൂർണ്ണസംഖ്യ മൂല്യം;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
ഘടന __vpiCallback : പൊതു __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;

	// Used for listing callbacks.
      ഘടന __vpiCallback*next;

	// user supplied callback data
      ഘടന t_cb_data cb_data;
};

class മൂല്യം_callback : പൊതു __vpiCallback {
    പൊതു:
      explicit മൂല്യം_callback(p_cb_data data);
	// Return true if the callback really is ready to be called
      മായാ bool test_മൂല്യം_callback_ready(ശൂന്യം);

    പൊതു:
	// user supplied callback data
      ഘടന t_vpi_time cb_time;
      ഘടന t_vpi_മൂല്യം cb_മൂല്യം;
};

ബാഹ്യ ശൂന്യം callback_execute(ഘടന __vpiCallback*cur);

ഘടന __vpiSystemTime : പൊതു __vpiHandle {
      __vpiSystemTime();
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം*vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;

      __vpiScope*scope;
};

ഘടന __vpiScopedTime : പൊതു __vpiSystemTime {
      __vpiScopedTime();
      അക്ഷരം*vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
};
ഘടന __vpiScopedSTime : പൊതു __vpiSystemTime {
      __vpiScopedSTime();
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം*vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
};
ഘടന __vpiScopedRealtime : പൊതു __vpiSystemTime {
      __vpiScopedRealtime();
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം*vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : പൊതു __vpiHandle {

    പൊതു:
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം* vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_iterate(പൂർണ്ണസംഖ്യ code) override;

    പൊതു:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      വരിയിൽ സ്ഥിരം അക്ഷരം*scope_name() സ്ഥിരം { തിരികെ name_; }

      വരിയിൽ സ്ഥിരം അക്ഷരം*scope_def_name() സ്ഥിരം { തിരികെ tname_; }
	// TRUE if this is an automatic func/task/block
      വരിയിൽ bool is_automatic() സ്ഥിരം { തിരികെ is_automatic_; }

    പൊതു:
      __vpiScope *scope;
      unsigned file_idx;
      unsigned lineno;
      unsigned def_file_idx;
      unsigned def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_time;
      ഘടന __vpiScopedSTime scoped_stime;
      ഘടന __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::vector<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      ഘടന automatic_hooks_s**item;
      unsigned nitem;
        /* Keep a list of live contexts. */
      vvp_context_t live_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::set<vthread_t> threads;
      signed പൂർണ്ണസംഖ്യ time_units :8;
      signed പൂർണ്ണസംഖ്യ time_precision :8;

    സംരക്ഷിത:
      __vpiScope(സ്ഥിരം അക്ഷരം*nam, സ്ഥിരം അക്ഷരം*tnam, bool is_auto_flag =false);

    സ്വകാര്യ:
	/* The scope has a name. */
      സ്ഥിരം അക്ഷരം*name_;
      സ്ഥിരം അക്ഷരം*tname_;
	/* the scope may be "automatic" */
      bool is_automatic_;
};

class vpiScopeFunction  : പൊതു __vpiScope {
    പൊതു:
      വരിയിൽ vpiScopeFunction(സ്ഥിരം അക്ഷരം*nam, സ്ഥിരം അക്ഷരം*tnam,
			      bool auto_flag, പൂർണ്ണസംഖ്യ func_type, unsigned func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiFunction; }
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override
      {
	    തിരഞ്ഞെടുപ്പ് (code) {
		അവസ്ഥ vpiFuncType:
		  തിരികെ func_type_;
		സ്ഥിരസ്ഥിതി:
		  തിരികെ __vpiScope::vpi_get(code);
	    }
      }

    പൊതു:
      വരിയിൽ unsigned get_func_width(ശൂന്യം) സ്ഥിരം { തിരികെ func_wid_; }
      വരിയിൽ vvp_bit4_t get_func_init_val(ശൂന്യം) സ്ഥിരം { തിരികെ func_init_val_; }

    സ്വകാര്യ:
      പൂർണ്ണസംഖ്യ func_type_;
      unsigned func_wid_;
      vvp_bit4_t func_init_val_;
};

ബാഹ്യ __vpiScope* vpip_peek_current_scope(ശൂന്യം);
ബാഹ്യ ശൂന്യം vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj);
ബാഹ്യ ശൂന്യം vpip_attach_to_current_scope(vpiHandle obj);
ബാഹ്യ __vpiScope* vpip_peek_context_scope(ശൂന്യം);
ബാഹ്യ unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                         __vpiScope*scope);
ബാഹ്യ vpiHandle vpip_make_വേര്_iterator(പൂർണ്ണസംഖ്യ type_code);
ബാഹ്യ ശൂന്യം vpip_make_വേര്_iterator(class __vpiHandle**&table,
				    unsigned&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
ഘടന __vpiSignal : പൊതു __vpiHandle {
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      int64_t vpi_get64(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം* vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
      vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_iterate(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_index(പൂർണ്ണസംഖ്യ idx) override;

    പൊതു:
      unsigned width() സ്ഥിരം;
      vpiHandle get_index(പൂർണ്ണസംഖ്യ index);
      ശൂന്യം get_bit_മൂല്യം(സ്ഥിരം ഘടന __vpiBit*bit, p_vpi_മൂല്യം vp);
      vpiHandle put_bit_മൂല്യം(ഘടന __vpiBit*bit, p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ flags);
      ശൂന്യം make_bits();

      ഘടന __vpiBit*bits;

    പൊതു:
      സംഘം { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
      സംഘം { // The name of this reg/net, or the index for array words.
            സ്ഥിരം അക്ഷരം*name;
            vpiHandle index;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      unsigned signed_flag  : 1;
      unsigned is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*കണ്ണി;

    പൊതു:
      സ്ഥിതം ശൂന്യം*operator new(std::size_t size);
      സ്ഥിതം ശൂന്യം operator delete(ശൂന്യം*); // not implemented
    സംരക്ഷിത:
      വരിയിൽ __vpiSignal() : bits(ശൂന്യ) { }
    സ്വകാര്യ: // Not implemented
      സ്ഥിതം ശൂന്യം*operator new[] (std::size_t size);
      സ്ഥിതം ശൂന്യം operator delete[](ശൂന്യം*);
};
ബാഹ്യ unsigned vpip_size(സ്ഥിരം __vpiSignal *sig);
ബാഹ്യ __vpiScope* vpip_scope(__vpiSignal*sig);

ബാഹ്യ vpiHandle vpip_make_int2(സ്ഥിരം അക്ഷരം*name, പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb,
			       bool signed_flag, vvp_net_t*vec);
ബാഹ്യ vpiHandle vpip_make_int4(സ്ഥിരം അക്ഷരം*name, പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb,
			       vvp_net_t*vec);
ബാഹ്യ vpiHandle vpip_make_var4(സ്ഥിരം അക്ഷരം*name, പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb,
			       bool signed_flag, vvp_net_t*net);
ബാഹ്യ vpiHandle vpip_make_net4(__vpiScope*scope,
				സ്ഥിരം അക്ഷരം*name, പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb,
				bool signed_flag, vvp_net_t*കണ്ണി);

/*
 * This is used to represent a bit in a net/reg.
 */
ഘടന __vpiBit {
      ഘടന as_bit_t : പൊതു __vpiHandle {
	    പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം;
	    പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code);
	    അക്ഷരം* vpi_get_str(പൂർണ്ണസംഖ്യ code);
	    ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val);
	    vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags);
	    vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code);
      } as_bit;

      vpiHandle index;

      സംഘം {
	    ഘടന __vpiSignal*parent;
	    ഘടന __vpiBit*bit0;
      };

      വരിയിൽ unsigned get_norm_index() സ്ഥിരം { തിരികെ this - bit0; }
      വരിയിൽ ഘടന __vpiSignal*get_parent() സ്ഥിരം {തിരികെ (bit0 - 1)->parent; }
      പൂർണ്ണസംഖ്യ get_index(ശൂന്യം) സ്ഥിരം;
};

class vpiPortBitInfo;

class vpiPortInfo  : പൊതു __vpiHandle {
    പൊതു:
      vpiPortInfo( __vpiScope *parent,
                    unsigned index,
                    പൂർണ്ണസംഖ്യ vpi_direction,
                    unsigned width,
                    സ്ഥിരം അക്ഷരം *name,
                    അക്ഷരം* buffer );
      ~vpiPortInfo() override;

      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiPort; }
      പൂർണ്ണസംഖ്യ get_direction(ശൂന്യം) { തിരികെ direction_; }
      unsigned get_index(ശൂന്യം) { തിരികെ index_; }
      പൂർണ്ണസംഖ്യ get_width(ശൂന്യം) { തിരികെ width_; }
      ശൂന്യം add_port_bit(vpiPortBitInfo* port_bit) { port_bits_.തള്ളുക_back(port_bit); }

      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം* vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;
      vvp_net_t* get_port(ശൂന്യം) സ്ഥിരം { തിരികെ ref_; }
      vpiHandle vpi_iterate(പൂർണ്ണസംഖ്യ code) override;

      std::vector<vpiPortBitInfo*> port_bits_;

    സ്വകാര്യ:
      __vpiScope *parent_;
      unsigned  index_;
      പൂർണ്ണസംഖ്യ       direction_;
      unsigned  width_;
      സ്ഥിരം അക്ഷരം *name_;
      vvp_net_t *ref_;
};

class vpiPortBitInfo  : പൊതു __vpiHandle {
    പൊതു:
      vpiPortBitInfo(vpiPortInfo *parent,
                     unsigned bit);
      ~vpiPortBitInfo() override;

      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiPortBit; }
      unsigned get_bit(ശൂന്യം) സ്ഥിരം { തിരികെ bit_; }

      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;

    സ്വകാര്യ:
      vpiPortInfo *parent_;
      unsigned bit_;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
ഘടന __vpiPV : പൊതു __vpiHandle {
      __vpiPV();
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം* vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
      vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;

      vpiHandle parent;
      vvp_net_t*net;
      vpiHandle sbase;
      പൂർണ്ണസംഖ്യ tbase;
      unsigned width;
};
ബാഹ്യ vpiHandle vpip_make_PV(അക്ഷരം*name, പൂർണ്ണസംഖ്യ base, പൂർണ്ണസംഖ്യ width);
ബാഹ്യ vpiHandle vpip_make_PV(അക്ഷരം*name, അക്ഷരം*ചിഹ്നം, പൂർണ്ണസംഖ്യ width);
ബാഹ്യ vpiHandle vpip_make_PV(അക്ഷരം*name, vpiHandle handle, പൂർണ്ണസംഖ്യ width);

ഘടന __vpiModPathTerm : പൊതു __vpiHandle {
      __vpiModPathTerm();
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;

      vpiHandle വ്യഞ്ജകം;
	/* The value returned by vpi_get(vpiEdge, ...); */
      പൂർണ്ണസംഖ്യ edge;
};

ഘടന __vpiModPathSrc : പൊതു __vpiHandle {
      __vpiModPathSrc();
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
      vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_iterate(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_index(പൂർണ്ണസംഖ്യ idx) override;
      ശൂന്യം vpi_get_delays(p_vpi_delay del) override;
      ശൂന്യം vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(ശൂന്യം) override;

      ഘടന __vpiModPath *dest;
      പൂർണ്ണസംഖ്യ   type;

	/* This is the input expression for this modpath. */
      ഘടന __vpiModPathTerm path_term_in;

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

ഘടന __vpiModPath {
      __vpiScope   *scope ;

      class vvp_fun_modpath*modpath;

      ഘടന __vpiModPathTerm path_term_out;
      vvp_net_t *നിവേശം_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

ബാഹ്യ ഘടന __vpiModPathSrc* vpip_make_modpath_src(ഘടന __vpiModPath*path,
                                                     vvp_net_t *net) ;

ബാഹ്യ ഘടന __vpiModPath* vpip_make_modpath(vvp_net_t *net) ;

/*
 *
 * The vpiInterModPath vpiHandle will define
 * a vpiInterModPath of record .intermodpath as defined
 * in the IEEE 1364
 *
 */

ഘടന __vpiInterModPath : പൊതു __vpiHandle {
      __vpiInterModPath();
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
      vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_iterate(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_delays(p_vpi_delay del) override;
      ശൂന്യം vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(ശൂന്യം) override;

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

ബാഹ്യ ഘടന __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2);

/*
 * These methods support the vpi creation of events. The name string
 * passed in will be saved, so the caller must allocate it (or not
 * free it) after it is handed to this function.
 */
class __vpiNamedEvent : പൊതു __vpiHandle {

    പൊതു:
      __vpiNamedEvent(__vpiScope*scope, സ്ഥിരം അക്ഷരം*name);
      ~__vpiNamedEvent() override;

      __vpiNamedEvent(സ്ഥിരം __vpiNamedEvent&) = delete;
      __vpiNamedEvent& operator=(സ്ഥിരം __vpiNamedEvent&) = delete;

      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      __vpiScope*get_scope(ശൂന്യം) സ്ഥിരം { തിരികെ scope_; }
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം* vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;

      വരിയിൽ ശൂന്യം add_vpi_callback(__vpiCallback*cb)
      { cb->next = callbacks_;
	callbacks_ = cb;
      }

      ശൂന്യം run_vpi_callbacks(ശൂന്യം);

	/* The functor, used for %set operations. */
      vvp_net_t*funct;

    സ്വകാര്യ:
	/* base name of the event object */
      സ്ഥിരം അക്ഷരം*name_;
	/* Parent scope of this object. */
      __vpiScope*scope_;
	/* List of callbacks interested in this event. */
      __vpiCallback*callbacks_;
};

ബാഹ്യ vpiHandle vpip_make_named_event(സ്ഥിരം അക്ഷരം*name, vvp_net_t*f);

/*
 * Memory is an array of bits that is accessible in N-bit chunks, with
 * N being the width of a word. The memory word handle just points
 * back to the memory and uses an index to identify its position in
 * the memory.
 */

ബാഹ്യ bool is_net_array(vpiHandle obj);

/*
 * These are the various variable types.
 */
ഘടന __vpiRealVar : പൊതു __vpiHandle {
      __vpiRealVar();
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം* vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
      vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_iterate(പൂർണ്ണസംഖ്യ code) override;

      സംഘം { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      സംഘം {
            സ്ഥിരം അക്ഷരം*name;
            vpiHandle index;
      } id;
      unsigned is_netarray  : 1; // This is word of a net array
      unsigned is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

ബാഹ്യ __vpiScope* vpip_scope(__vpiRealVar*sig);
ബാഹ്യ vpiHandle vpip_make_real_var(സ്ഥിരം അക്ഷരം*name, vvp_net_t*net);
ബാഹ്യ vpiHandle vpip_make_real_net(__vpiScope*scope,
				    സ്ഥിരം അക്ഷരം*name, vvp_net_t*net);

class __vpiBaseVar : പൊതു __vpiHandle {

    പൊതു:
      __vpiBaseVar(__vpiScope*scope, സ്ഥിരം അക്ഷരം*name, vvp_net_t*net);
#ifdef CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#endif

      വരിയിൽ vvp_net_t* get_net() സ്ഥിരം { തിരികെ net_; }

    സംരക്ഷിത:
      __vpiScope* scope_;
      സ്ഥിരം അക്ഷരം*name_;

    സ്വകാര്യ:
      vvp_net_t*net_;
};

class __vpiStringVar : പൊതു __vpiBaseVar {

    പൊതു:
      __vpiStringVar(__vpiScope*scope, സ്ഥിരം അക്ഷരം*name, vvp_net_t*net);

      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
      vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags) override;
};

ബാഹ്യ vpiHandle vpip_make_string_var(സ്ഥിരം അക്ഷരം*name, vvp_net_t*net);

ഘടന __vpiArrayBase {
      __vpiArrayBase() : vals_വാക്കുകൾ(ശൂന്യ) {}
      മായാ ~__vpiArrayBase() {}

      മായാ unsigned get_size(ശൂന്യം) സ്ഥിരം = 0;
      മായാ vpiHandle get_ഇടത്_range() = 0;
      മായാ vpiHandle get_വലത്_range() = 0;
      മായാ __vpiScope*get_scope() സ്ഥിരം = 0;

      മായാ പൂർണ്ണസംഖ്യ get_word_size() സ്ഥിരം = 0;
      മായാ അക്ഷരം*get_word_str(ഘടന __vpiArrayWord*word, പൂർണ്ണസംഖ്യ code) = 0;
      മായാ ശൂന്യം get_word_മൂല്യം(ഘടന __vpiArrayWord*word, p_vpi_മൂല്യം vp) = 0;
      മായാ ശൂന്യം put_word_മൂല്യം(ഘടന __vpiArrayWord*word, p_vpi_മൂല്യം vp,
                                    പൂർണ്ണസംഖ്യ flags) = 0;
      മായാ vpiHandle get_iter_index(ഘടന __vpiArrayIterator*iter, പൂർണ്ണസംഖ്യ idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_base_iterate(പൂർണ്ണസംഖ്യ code);

      മായാ ശൂന്യം make_vals_വാക്കുകൾ();

      ഘടന __vpiArrayWord*vals_വാക്കുകൾ;
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
ഘടന __vpiArray : പൊതു __vpiArrayBase, പൊതു __vpiHandle {
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiMemory; }
      unsigned get_size() സ്ഥിരം override { തിരികെ array_എണ്ണം; }
      vpiHandle get_ഇടത്_range() override { പ്രതിജ്ഞാ(nets == 0); തിരികെ &msb; }
      vpiHandle get_വലത്_range() override { പ്രതിജ്ഞാ(nets == 0); തിരികെ &lsb; }
      __vpiScope*get_scope() സ്ഥിരം override { തിരികെ scope; }

      പൂർണ്ണസംഖ്യ get_word_size() സ്ഥിരം override;
      അക്ഷരം*get_word_str(ഘടന __vpiArrayWord*word, പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം get_word_മൂല്യം(ഘടന __vpiArrayWord*word, p_vpi_മൂല്യം vp) override;
      ശൂന്യം put_word_മൂല്യം(ഘടന __vpiArrayWord*word, p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ flags) override;

      vpiHandle get_iter_index(ഘടന __vpiArrayIterator*iter, പൂർണ്ണസംഖ്യ idx) override;

      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം* vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;
      വരിയിൽ vpiHandle vpi_iterate(പൂർണ്ണസംഖ്യ code) override { തിരികെ vpi_array_base_iterate(code); }
      vpiHandle vpi_index(പൂർണ്ണസംഖ്യ idx) override;

      ശൂന്യം set_word(unsigned idx, unsigned off, സ്ഥിരം vvp_vector4_t&val);
      ശൂന്യം set_word(unsigned idx, ഇരട്ട val);
      ശൂന്യം set_word(unsigned idx, സ്ഥിരം std::string&val);
      ശൂന്യം set_word(unsigned idx, സ്ഥിരം vvp_object_t&val);

      vvp_vector4_t get_word(unsigned address);
      ഇരട്ട get_word_r(unsigned address);
      ശൂന്യം get_word_obj(unsigned address, vvp_object_t&val);
      std::string get_word_str(unsigned address);

      ശൂന്യം alias_word(unsigned long addr, vpiHandle word, പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb);
      ശൂന്യം attach_word(unsigned addr, vpiHandle word);
      ശൂന്യം word_change(unsigned long addr);

      സ്ഥിരം അക്ഷരം*name; /* Permanently allocated string */
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
      ഘടന __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool swap_addr;

സ്വകാര്യ:
      unsigned array_എണ്ണം;
      __vpiScope*scope;

friend vpiHandle vpip_make_array(സ്ഥിരം അക്ഷരം*label, സ്ഥിരം അക്ഷരം*name,
                                 പൂർണ്ണസംഖ്യ first_addr, പൂർണ്ണസംഖ്യ last_addr,
                                 bool signed_flag);
friend ശൂന്യം compile_array_alias(അക്ഷരം*label, അക്ഷരം*name, അക്ഷരം*src);
};

class __vpiDarrayVar : പൊതു __vpiBaseVar, പൊതു __vpiArrayBase {
    പൊതു:
      __vpiDarrayVar(__vpiScope*scope, സ്ഥിരം അക്ഷരം*name, vvp_net_t*net);

      പൂർണ്ണസംഖ്യ get_type_code() സ്ഥിരം override { തിരികെ vpiArrayVar; }
      unsigned get_size() സ്ഥിരം override;
      vpiHandle get_ഇടത്_range() override;
      vpiHandle get_വലത്_range() override;
      __vpiScope*get_scope() സ്ഥിരം override { തിരികെ scope_; }

      പൂർണ്ണസംഖ്യ get_word_size() സ്ഥിരം override;
      അക്ഷരം*get_word_str(ഘടന __vpiArrayWord*word, പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം get_word_മൂല്യം(ഘടന __vpiArrayWord*word, p_vpi_മൂല്യം vp) override;
      ശൂന്യം put_word_മൂല്യം(ഘടന __vpiArrayWord*word, p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ flags) override;

      vpiHandle get_iter_index(ഘടന __vpiArrayIterator*iter, പൂർണ്ണസംഖ്യ idx) override;
      വരിയിൽ vpiHandle vpi_iterate(പൂർണ്ണസംഖ്യ code) override { തിരികെ vpi_array_base_iterate(code); }

      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം* vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_index(പൂർണ്ണസംഖ്യ index) override;

      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;

    സംരക്ഷിത:
      vvp_darray*get_vvp_darray() സ്ഥിരം;
      __vpiDecConst ഇടത്_range, വലത്_range;
};

ബാഹ്യ vpiHandle vpip_make_darray_var(സ്ഥിരം അക്ഷരം*name, vvp_net_t*net);

class __vpiQueueVar : പൊതു __vpiDarrayVar {

    പൊതു:
      __vpiQueueVar(__vpiScope*scope, സ്ഥിരം അക്ഷരം*name, vvp_net_t*net);

      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
};

ബാഹ്യ vpiHandle vpip_make_queue_var(സ്ഥിരം അക്ഷരം*name, vvp_net_t*net);

class __vpiPropQueueRef : പൊതു __vpiHandle {

    പൊതു:
      explicit __vpiPropQueueRef(__vpiScope*scope, unsigned pidx, bool is_queue);

      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം* vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;

      vvp_net_t* class_net_;
      unsigned prop_idx_;
      bool is_queue_;

    സ്വകാര്യ:
      __vpiScope* scope_;
};

ബാഹ്യ vpiHandle vpip_make_prop_queue_ref(അക്ഷരം* class_label,
                                          unsigned prop_idx,
                                          unsigned is_queue_flag);
ബാഹ്യ vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : പൊതു __vpiBaseVar {

    പൊതു:
      __vpiCobjectVar(__vpiScope*scope, സ്ഥിരം അക്ഷരം*name, vvp_net_t*net);

      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
};

ബാഹ്യ vpiHandle vpip_make_cobject_var(സ്ഥിരം അക്ഷരം*name, vvp_net_t*net);

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
ഘടന __vpiUserSystf : പൊതു __vpiHandle {
      __vpiUserSystf();
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;

      s_vpi_systf_data info;
      bool is_user_defn;
};

ബാഹ്യ vpiHandle vpip_make_systf_iterator(ശൂന്യം);

ബാഹ്യ ഘടന __vpiUserSystf* vpip_find_systf(സ്ഥിരം അക്ഷരം*name);


ഘടന __vpiSysTaskCall : പൊതു __vpiHandle {

      __vpiScope* scope;
      ഘടന __vpiUserSystf*defn;
      unsigned nargs;
      vpiHandle*args;
	/* Stack consumed by this call */
      unsigned vec4_അട്ടി;
      unsigned real_അട്ടി;
      unsigned string_അട്ടി;
	/* Support for vpi_get_userdata. */
      ശൂന്യം*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      unsigned file_idx;
      unsigned lineno;
      bool put_മൂല്യം;
    സംരക്ഷിത:
      വരിയിൽ __vpiSysTaskCall()
      {
	    vec4_അട്ടി = 0;
	    real_അട്ടി = 0;
	    string_അട്ടി = 0;
      }
};

ബാഹ്യ ഘടന __vpiSysTaskCall*vpip_cur_task;

/*
 * The persistent flag to vpip_make_string_const causes the created
 * handle to be persistent. This is necessary for cases where the
 * string handle may be reused, which is the normal case.
 *
 * When constructing with a string, the class takes possession of the
 * text value string, and will delete it in the constructor.
 */

vpiHandle vpip_make_string_const(അക്ഷരം*text, bool persistent =true);
vpiHandle vpip_make_string_param(സ്ഥിരം അക്ഷരം*name, അക്ഷരം*മൂല്യം, bool local_flag,
                                 long file_idx, long lineno);

ഘടന __vpiBinaryConst : പൊതു __vpiHandle {
      __vpiBinaryConst();
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;

      vvp_vector4_t bits;
	/* TRUE if this constant is signed. */
      unsigned signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      unsigned sized_flag  :1;
};

vpiHandle vpip_make_ദ്വിമാന_const(unsigned wid, സ്ഥിരം അക്ഷരം*bits);
vpiHandle vpip_make_ദ്വിമാന_param(സ്ഥിരം അക്ഷരം*name, സ്ഥിരം vvp_vector4_t&bits,
				 bool signed_flag, bool local_flag,
				 long file_idx, long lineno);

class __vpiRealConst : പൊതു __vpiHandle {
    പൊതു:
      explicit __vpiRealConst(ഇരട്ട);
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;

      ഇരട്ട മൂല്യം;
};

vpiHandle vpip_make_real_const(ഇരട്ട മൂല്യം);
vpiHandle vpip_make_real_param(സ്ഥിരം അക്ഷരം*name, ഇരട്ട മൂല്യം, bool local_flag,
                               long file_idx, long lineno);

class __vpiNullConst : പൊതു __vpiHandle {
    പൊതു:
      explicit __vpiNullConst();
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_word(unsigned base, സ്ഥിരം അക്ഷരം*type);
vpiHandle vpip_make_vthr_str_അട്ടി(unsigned depth);
vpiHandle vpip_make_vthr_vec4_അട്ടി(unsigned depth, bool signed_flag, unsigned wid);

vpiHandle vpip_make_vthr_A(അക്ഷരം*label, unsigned index);
vpiHandle vpip_make_vthr_A(അക്ഷരം*label, അക്ഷരം*ചിഹ്നം);
vpiHandle vpip_make_vthr_A(അക്ഷരം*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(അക്ഷരം*label, unsigned index, unsigned bit, unsigned wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
ബാഹ്യ "C" ശൂന്യം vpip_load_module(സ്ഥിരം അക്ഷരം*name);

ബാഹ്യ "C" ശൂന്യം vpip_clear_module_paths();
ബാഹ്യ "C" ശൂന്യം vpip_add_module_path(സ്ഥിരം അക്ഷരം *path);
ബാഹ്യ "C" ശൂന്യം vpip_add_env_and_default_module_paths();

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
ബാഹ്യ vpiHandle vpip_build_vpi_call(സ്ഥിരം അക്ഷരം*name,
				     പൂർണ്ണസംഖ്യ val_type, unsigned return_width,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     unsigned argc,
				     vpiHandle*argv,
				     unsigned vec4_അട്ടി,
				     unsigned real_അട്ടി,
				     unsigned string_അട്ടി,
				     long file_idx,
				     long lineno);

ബാഹ്യ vthread_t vpip_current_vthread;

ബാഹ്യ ശൂന്യം vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_time(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

ബാഹ്യ പൂർണ്ണസംഖ്യ vpip_get_time_precision(ശൂന്യം);
ബാഹ്യ ശൂന്യം vpip_set_time_precision(പൂർണ്ണസംഖ്യ pres);

ബാഹ്യ __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

ബാഹ്യ പൂർണ്ണസംഖ്യ vpip_time_units_from_handle(vpiHandle obj);
ബാഹ്യ പൂർണ്ണസംഖ്യ vpip_time_precision_from_handle(vpiHandle obj);

ബാഹ്യ ഇരട്ട vpip_scaled_time_from_handle(vvp_time64_t time, vpiHandle obj);

ബാഹ്യ ശൂന്യം vpip_time_to_timestruct(ഘടന t_vpi_time*ts, vvp_time64_t ti);
ബാഹ്യ vvp_time64_t vpip_timestruct_to_time(സ്ഥിരം ഘടന t_vpi_time*ts);

ബാഹ്യ ഇരട്ട vpip_time_to_scaled_real(vvp_time64_t ti, സ്ഥിരം __vpiScope*sc);
ബാഹ്യ vvp_time64_t vpip_scaled_real_to_time64(ഇരട്ട val, സ്ഥിരം __vpiScope*sc);

/*
 * These functions are used mostly as compile time to strings into
 * permallocated memory. The vpip_string function is the most general,
 * it allocates a fresh string no matter what. The vpip_name_string
 * allocates a string and keeps a pointer in the hash, and tries to
 * reuse it if it can. This us useful for handle names, which may be
 * reused in different scopes.
 */
ബാഹ്യ സ്ഥിരം അക്ഷരം* vpip_string(സ്ഥിരം അക്ഷരം*str);
ബാഹ്യ സ്ഥിരം അക്ഷരം* vpip_name_string(സ്ഥിരം അക്ഷരം*str);


/*
 * This function is used to make decimal string versions of various
 * vectors. The input format is a vvp_vector4_t, and the result is
 * written into buf, without overflowing nbuf.
 */
ബാഹ്യ unsigned vpip_vec4_to_dec_str(സ്ഥിരം vvp_vector4_t&vec4,
				     അക്ഷരം *buf, unsigned പൂർണ്ണസംഖ്യ nbuf,
				     പൂർണ്ണസംഖ്യ signed_flag);


ബാഹ്യ ശൂന്യം vpip_vec4_to_hex_str(സ്ഥിരം vvp_vector4_t&bits, അക്ഷരം*buf,
				 unsigned nbuf);

ബാഹ്യ ശൂന്യം vpip_vec4_to_oct_str(സ്ഥിരം vvp_vector4_t&bits, അക്ഷരം*buf,
				 unsigned nbuf);

ബാഹ്യ ശൂന്യം vpip_bin_str_to_vec4(vvp_vector4_t&val, സ്ഥിരം അക്ഷരം*buf);
ബാഹ്യ ശൂന്യം vpip_oct_str_to_vec4(vvp_vector4_t&val, സ്ഥിരം അക്ഷരം*str);
ബാഹ്യ ശൂന്യം vpip_dec_str_to_vec4(vvp_vector4_t&val, സ്ഥിരം അക്ഷരം*str);
ബാഹ്യ ശൂന്യം vpip_hex_str_to_vec4(vvp_vector4_t&val, സ്ഥിരം അക്ഷരം*str);

ബാഹ്യ vvp_vector4_t vec4_from_vpi_മൂല്യം(s_vpi_മൂല്യം*vp, unsigned wid);
ബാഹ്യ ഇരട്ട real_from_vpi_മൂല്യം(s_vpi_മൂല്യം*vp);

ബാഹ്യ ശൂന്യം vpip_vec4_get_മൂല്യം(സ്ഥിരം vvp_vector4_t&word_val, unsigned width,
				bool signed_flag, s_vpi_മൂല്യം*vp);
ബാഹ്യ ശൂന്യം vpip_vec2_get_മൂല്യം(സ്ഥിരം vvp_vector2_t&word_val, unsigned width,
				bool signed_flag, s_vpi_മൂല്യം*vp);
ബാഹ്യ ശൂന്യം vpip_real_get_മൂല്യം(ഇരട്ട real, s_vpi_മൂല്യം*vp);
ബാഹ്യ ശൂന്യം vpip_string_get_മൂല്യം(സ്ഥിരം std::string&val, s_vpi_മൂല്യം*vp);

/*
 * Function defined in vpi_signal.cc to manage vpi_get_* persistent
 * storage.
 */
എണ്ണൽ vpi_rbuf_t {
      RBUF_VAL =0,
	/* Storage for *_get_value() */
      RBUF_STR,
	/* Storage for *_get_str() */
      RBUF_DEL
	/* Delete the storage for both buffers. */
};
ബാഹ്യ ശൂന്യം *need_result_buf(unsigned cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
ബാഹ്യ അക്ഷരം *simple_set_rbuf_str(സ്ഥിരം അക്ഷരം *s1);
ബാഹ്യ അക്ഷരം *generic_get_str(പൂർണ്ണസംഖ്യ code, vpiHandle ref, സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *index);

/* A routine to find the enclosing module. */
ബാഹ്യ vpiHandle vpip_module(__vpiScope*scope);

ബാഹ്യ പൂർണ്ണസംഖ്യ vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
ബാഹ്യ bool vpip_mcd0_disable;

വരിയിൽ uint64_t vlg_round_to_u64(ഇരട്ട rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      തിരികെ static_cast<uint64_t>(static_cast<int64_t>(std::round(rval)));
}

#endif /* IVL_vpi_priv_H */
