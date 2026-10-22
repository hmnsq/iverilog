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
#شامل  "delay.h"


class class_type;
class vvp_darray;
class vvp_fun_arrayport;

تعریفنوع ساختار __vpiArray* vvp_array_t;

/*
 * This header file contains the internal definitions that the vvp
 * program uses to implement the public interface in the vpi_user.h
 * header file elsewhere.
 */

#اگر defined(__MINGW32__) || defined (__CYGWIN__)
بیرونی vpip_routines_s vpi_routines;
#endif

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#define vpi_File_سطر    0x1000003
#define _vpiDescription 0x1000004

بیرونی bool show_file_سطر;
بیرونی bool code_is_instrumented;

بیرونی vpiHandle vpip_build_file_سطر(ثابت نویسه*description,
                                      long file_idx, long lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
بیرونی ثابت نویسه* vpi_type_as_string(PLI_INT32 code);

/*
 * Return the port direction as a string.
 */
بیرونی ثابت نویسه* direction_as_string(صحیح dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#اگر defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
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
شمارش vpi_mode_t {
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
بیرونی vpi_mode_t vpi_mode_flag;

/*
 * This structure is the very base of a vpiHandle. Every handle
 * structure is derived from this class so that the library can
 * internally pass the derived types as pointers to one of these.
 */
class __vpiHandle {
    عمومی:
      درونخطی __vpiHandle() { }
	// The destructor is virtual so that dynamic types will work.
      مجازی ~__vpiHandle();

      مجازی صحیح get_type_code(تهی) ثابت =0;
      مجازی صحیح vpi_get(صحیح code);
      مجازی int64_t vpi_get64(صحیح code);
      مجازی نویسه* vpi_get_str(صحیح code);

      مجازی تهی vpi_get_مقدار(p_vpi_مقدار val);
      مجازی vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags);
      مجازی vpiHandle vpi_handle(صحیح code);
      مجازی vpiHandle vpi_iterate(صحیح code);
      مجازی vpiHandle vpi_index(صحیح idx);
      مجازی تهی vpi_get_delays(p_vpi_delay del);
      مجازی تهی vpi_put_delays(p_vpi_delay del);

	// Objects may have destroyer functions of their own. If so,
	// then this virtual method will return a POINTER to that
	// function. The pointer is used to "delete" the object, which
	// is why the function itself cannot be a method.
      تعریفنوع صحیح (*free_object_fun_t)(vpiHandle);
      مجازی free_object_fun_t free_object_fun(تهی);
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
ساختار __vpiIterator : عمومی __vpiHandle {
      __vpiIterator();
      صحیح get_type_code(تهی) ثابت override;
      free_object_fun_t free_object_fun(تهی) override;

      vpiHandle *args;
      unsigned  nargs;
      unsigned  next;
      bool free_args_flag;
};

بیرونی vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
				    bool free_args_flag);

class __vpiDecConst : عمومی __vpiHandle {
    عمومی:
      explicit __vpiDecConst(صحیح val =0);
      __vpiDecConst(ثابت __vpiDecConst&that);
# اگر __cplusplus >= 201103L
      __vpiDecConst& operator = (ثابت __vpiDecConst&that) = پیشفرض;
# endif
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;

    عمومی:
      درونخطی صحیح get_مقدار() ثابت { بازگشت مقدار; }
      درونخطی تهی set_مقدار(صحیح val) { مقدار = val; }

    خصوصی:
      صحیح مقدار;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
ساختار __vpiCallback : عمومی __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      صحیح get_type_code(تهی) ثابت override;

	// Used for listing callbacks.
      ساختار __vpiCallback*next;

	// user supplied callback data
      ساختار t_cb_data cb_data;
};

class مقدار_callback : عمومی __vpiCallback {
    عمومی:
      explicit مقدار_callback(p_cb_data data);
	// Return true if the callback really is ready to be called
      مجازی bool test_مقدار_callback_ready(تهی);

    عمومی:
	// user supplied callback data
      ساختار t_vpi_time cb_time;
      ساختار t_vpi_مقدار cb_مقدار;
};

بیرونی تهی callback_execute(ساختار __vpiCallback*cur);

ساختار __vpiSystemTime : عمومی __vpiHandle {
      __vpiSystemTime();
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      نویسه*vpi_get_str(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_handle(صحیح code) override;

      __vpiScope*scope;
};

ساختار __vpiScopedTime : عمومی __vpiSystemTime {
      __vpiScopedTime();
      نویسه*vpi_get_str(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
};
ساختار __vpiScopedSTime : عمومی __vpiSystemTime {
      __vpiScopedSTime();
      صحیح vpi_get(صحیح code) override;
      نویسه*vpi_get_str(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
};
ساختار __vpiScopedRealtime : عمومی __vpiSystemTime {
      __vpiScopedRealtime();
      صحیح vpi_get(صحیح code) override;
      نویسه*vpi_get_str(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : عمومی __vpiHandle {

    عمومی:
      صحیح vpi_get(صحیح code) override;
      نویسه* vpi_get_str(صحیح code) override;
      vpiHandle vpi_handle(صحیح code) override;
      vpiHandle vpi_iterate(صحیح code) override;

    عمومی:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      درونخطی ثابت نویسه*scope_name() ثابت { بازگشت name_; }

      درونخطی ثابت نویسه*scope_def_name() ثابت { بازگشت tname_; }
	// TRUE if this is an automatic func/task/block
      درونخطی bool is_automatic() ثابت { بازگشت is_automatic_; }

    عمومی:
      __vpiScope *scope;
      unsigned file_idx;
      unsigned lineno;
      unsigned def_file_idx;
      unsigned def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_time;
      ساختار __vpiScopedSTime scoped_stime;
      ساختار __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::vector<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      ساختار automatic_hooks_s**item;
      unsigned nitem;
        /* Keep a list of live contexts. */
      vvp_context_t live_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::set<vthread_t> threads;
      signed صحیح time_units :8;
      signed صحیح time_precision :8;

    محافظتشده:
      __vpiScope(ثابت نویسه*nam, ثابت نویسه*tnam, bool is_auto_flag =false);

    خصوصی:
	/* The scope has a name. */
      ثابت نویسه*name_;
      ثابت نویسه*tname_;
	/* the scope may be "automatic" */
      bool is_automatic_;
};

class vpiScopeFunction  : عمومی __vpiScope {
    عمومی:
      درونخطی vpiScopeFunction(ثابت نویسه*nam, ثابت نویسه*tnam,
			      bool auto_flag, صحیح func_type, unsigned func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      صحیح get_type_code(تهی) ثابت override { بازگشت vpiFunction; }
      صحیح vpi_get(صحیح code) override
      {
	    انتخاب (code) {
		حالت vpiFuncType:
		  بازگشت func_type_;
		پیشفرض:
		  بازگشت __vpiScope::vpi_get(code);
	    }
      }

    عمومی:
      درونخطی unsigned get_func_width(تهی) ثابت { بازگشت func_wid_; }
      درونخطی vvp_bit4_t get_func_init_val(تهی) ثابت { بازگشت func_init_val_; }

    خصوصی:
      صحیح func_type_;
      unsigned func_wid_;
      vvp_bit4_t func_init_val_;
};

بیرونی __vpiScope* vpip_peek_current_scope(تهی);
بیرونی تهی vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj);
بیرونی تهی vpip_attach_to_current_scope(vpiHandle obj);
بیرونی __vpiScope* vpip_peek_context_scope(تهی);
بیرونی unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                         __vpiScope*scope);
بیرونی vpiHandle vpip_make_ریشه_iterator(صحیح type_code);
بیرونی تهی vpip_make_ریشه_iterator(class __vpiHandle**&table,
				    unsigned&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
ساختار __vpiSignal : عمومی __vpiHandle {
      صحیح vpi_get(صحیح code) override;
      int64_t vpi_get64(صحیح code) override;
      نویسه* vpi_get_str(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags) override;
      vpiHandle vpi_handle(صحیح code) override;
      vpiHandle vpi_iterate(صحیح code) override;
      vpiHandle vpi_index(صحیح idx) override;

    عمومی:
      unsigned width() ثابت;
      vpiHandle get_index(صحیح index);
      تهی get_bit_مقدار(ثابت ساختار __vpiBit*bit, p_vpi_مقدار vp);
      vpiHandle put_bit_مقدار(ساختار __vpiBit*bit, p_vpi_مقدار vp, صحیح flags);
      تهی make_bits();

      ساختار __vpiBit*bits;

    عمومی:
      اجتماع { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
      اجتماع { // The name of this reg/net, or the index for array words.
            ثابت نویسه*name;
            vpiHandle index;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      unsigned signed_flag  : 1;
      unsigned is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*گره;

    عمومی:
      ایستا تهی*operator new(std::size_t size);
      ایستا تهی operator delete(تهی*); // not implemented
    محافظتشده:
      درونخطی __vpiSignal() : bits(پوچ) { }
    خصوصی: // Not implemented
      ایستا تهی*operator new[] (std::size_t size);
      ایستا تهی operator delete[](تهی*);
};
بیرونی unsigned vpip_size(ثابت __vpiSignal *sig);
بیرونی __vpiScope* vpip_scope(__vpiSignal*sig);

بیرونی vpiHandle vpip_make_int2(ثابت نویسه*name, صحیح msb, صحیح lsb,
			       bool signed_flag, vvp_net_t*vec);
بیرونی vpiHandle vpip_make_int4(ثابت نویسه*name, صحیح msb, صحیح lsb,
			       vvp_net_t*vec);
بیرونی vpiHandle vpip_make_var4(ثابت نویسه*name, صحیح msb, صحیح lsb,
			       bool signed_flag, vvp_net_t*net);
بیرونی vpiHandle vpip_make_net4(__vpiScope*scope,
				ثابت نویسه*name, صحیح msb, صحیح lsb,
				bool signed_flag, vvp_net_t*گره);

/*
 * This is used to represent a bit in a net/reg.
 */
ساختار __vpiBit {
      ساختار as_bit_t : عمومی __vpiHandle {
	    صحیح get_type_code(تهی) ثابت;
	    صحیح vpi_get(صحیح code);
	    نویسه* vpi_get_str(صحیح code);
	    تهی vpi_get_مقدار(p_vpi_مقدار val);
	    vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags);
	    vpiHandle vpi_handle(صحیح code);
      } as_bit;

      vpiHandle index;

      اجتماع {
	    ساختار __vpiSignal*parent;
	    ساختار __vpiBit*bit0;
      };

      درونخطی unsigned get_norm_index() ثابت { بازگشت this - bit0; }
      درونخطی ساختار __vpiSignal*get_parent() ثابت {بازگشت (bit0 - 1)->parent; }
      صحیح get_index(تهی) ثابت;
};

class vpiPortBitInfo;

class vpiPortInfo  : عمومی __vpiHandle {
    عمومی:
      vpiPortInfo( __vpiScope *parent,
                    unsigned index,
                    صحیح vpi_direction,
                    unsigned width,
                    ثابت نویسه *name,
                    نویسه* buffer );
      ~vpiPortInfo() override;

      صحیح get_type_code(تهی) ثابت override { بازگشت vpiPort; }
      صحیح get_direction(تهی) { بازگشت direction_; }
      unsigned get_index(تهی) { بازگشت index_; }
      صحیح get_width(تهی) { بازگشت width_; }
      تهی add_port_bit(vpiPortBitInfo* port_bit) { port_bits_.افزودن_back(port_bit); }

      صحیح vpi_get(صحیح code) override;
      نویسه* vpi_get_str(صحیح code) override;
      vpiHandle vpi_handle(صحیح code) override;
      vvp_net_t* get_port(تهی) ثابت { بازگشت ref_; }
      vpiHandle vpi_iterate(صحیح code) override;

      std::vector<vpiPortBitInfo*> port_bits_;

    خصوصی:
      __vpiScope *parent_;
      unsigned  index_;
      صحیح       direction_;
      unsigned  width_;
      ثابت نویسه *name_;
      vvp_net_t *ref_;
};

class vpiPortBitInfo  : عمومی __vpiHandle {
    عمومی:
      vpiPortBitInfo(vpiPortInfo *parent,
                     unsigned bit);
      ~vpiPortBitInfo() override;

      صحیح get_type_code(تهی) ثابت override { بازگشت vpiPortBit; }
      unsigned get_bit(تهی) ثابت { بازگشت bit_; }

      صحیح vpi_get(صحیح code) override;
      vpiHandle vpi_handle(صحیح code) override;

    خصوصی:
      vpiPortInfo *parent_;
      unsigned bit_;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
ساختار __vpiPV : عمومی __vpiHandle {
      __vpiPV();
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      نویسه* vpi_get_str(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags) override;
      vpiHandle vpi_handle(صحیح code) override;

      vpiHandle parent;
      vvp_net_t*net;
      vpiHandle sbase;
      صحیح tbase;
      unsigned width;
};
بیرونی vpiHandle vpip_make_PV(نویسه*name, صحیح base, صحیح width);
بیرونی vpiHandle vpip_make_PV(نویسه*name, نویسه*نماد, صحیح width);
بیرونی vpiHandle vpip_make_PV(نویسه*name, vpiHandle handle, صحیح width);

ساختار __vpiModPathTerm : عمومی __vpiHandle {
      __vpiModPathTerm();
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      vpiHandle vpi_handle(صحیح code) override;

      vpiHandle عبارت;
	/* The value returned by vpi_get(vpiEdge, ...); */
      صحیح edge;
};

ساختار __vpiModPathSrc : عمومی __vpiHandle {
      __vpiModPathSrc();
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags) override;
      vpiHandle vpi_handle(صحیح code) override;
      vpiHandle vpi_iterate(صحیح code) override;
      vpiHandle vpi_index(صحیح idx) override;
      تهی vpi_get_delays(p_vpi_delay del) override;
      تهی vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(تهی) override;

      ساختار __vpiModPath *dest;
      صحیح   type;

	/* This is the input expression for this modpath. */
      ساختار __vpiModPathTerm path_term_in;

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

ساختار __vpiModPath {
      __vpiScope   *scope ;

      class vvp_fun_modpath*modpath;

      ساختار __vpiModPathTerm path_term_out;
      vvp_net_t *درون_داد_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

بیرونی ساختار __vpiModPathSrc* vpip_make_modpath_src(ساختار __vpiModPath*path,
                                                     vvp_net_t *net) ;

بیرونی ساختار __vpiModPath* vpip_make_modpath(vvp_net_t *net) ;

/*
 *
 * The vpiInterModPath vpiHandle will define
 * a vpiInterModPath of record .intermodpath as defined
 * in the IEEE 1364
 *
 */

ساختار __vpiInterModPath : عمومی __vpiHandle {
      __vpiInterModPath();
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags) override;
      vpiHandle vpi_handle(صحیح code) override;
      vpiHandle vpi_iterate(صحیح code) override;
      تهی vpi_get_delays(p_vpi_delay del) override;
      تهی vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(تهی) override;

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

بیرونی ساختار __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2);

/*
 * These methods support the vpi creation of events. The name string
 * passed in will be saved, so the caller must allocate it (or not
 * free it) after it is handed to this function.
 */
class __vpiNamedEvent : عمومی __vpiHandle {

    عمومی:
      __vpiNamedEvent(__vpiScope*scope, ثابت نویسه*name);
      ~__vpiNamedEvent() override;

      __vpiNamedEvent(ثابت __vpiNamedEvent&) = delete;
      __vpiNamedEvent& operator=(ثابت __vpiNamedEvent&) = delete;

      صحیح get_type_code(تهی) ثابت override;
      __vpiScope*get_scope(تهی) ثابت { بازگشت scope_; }
      صحیح vpi_get(صحیح code) override;
      نویسه* vpi_get_str(صحیح code) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags) override;
      vpiHandle vpi_handle(صحیح code) override;

      درونخطی تهی add_vpi_callback(__vpiCallback*cb)
      { cb->next = callbacks_;
	callbacks_ = cb;
      }

      تهی run_vpi_callbacks(تهی);

	/* The functor, used for %set operations. */
      vvp_net_t*funct;

    خصوصی:
	/* base name of the event object */
      ثابت نویسه*name_;
	/* Parent scope of this object. */
      __vpiScope*scope_;
	/* List of callbacks interested in this event. */
      __vpiCallback*callbacks_;
};

بیرونی vpiHandle vpip_make_named_event(ثابت نویسه*name, vvp_net_t*f);

/*
 * Memory is an array of bits that is accessible in N-bit chunks, with
 * N being the width of a word. The memory word handle just points
 * back to the memory and uses an index to identify its position in
 * the memory.
 */

بیرونی bool is_net_array(vpiHandle obj);

/*
 * These are the various variable types.
 */
ساختار __vpiRealVar : عمومی __vpiHandle {
      __vpiRealVar();
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      نویسه* vpi_get_str(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags) override;
      vpiHandle vpi_handle(صحیح code) override;
      vpiHandle vpi_iterate(صحیح code) override;

      اجتماع { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      اجتماع {
            ثابت نویسه*name;
            vpiHandle index;
      } id;
      unsigned is_netarray  : 1; // This is word of a net array
      unsigned is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

بیرونی __vpiScope* vpip_scope(__vpiRealVar*sig);
بیرونی vpiHandle vpip_make_real_var(ثابت نویسه*name, vvp_net_t*net);
بیرونی vpiHandle vpip_make_real_net(__vpiScope*scope,
				    ثابت نویسه*name, vvp_net_t*net);

class __vpiBaseVar : عمومی __vpiHandle {

    عمومی:
      __vpiBaseVar(__vpiScope*scope, ثابت نویسه*name, vvp_net_t*net);
#ifdef CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#endif

      درونخطی vvp_net_t* get_net() ثابت { بازگشت net_; }

    محافظتشده:
      __vpiScope* scope_;
      ثابت نویسه*name_;

    خصوصی:
      vvp_net_t*net_;
};

class __vpiStringVar : عمومی __vpiBaseVar {

    عمومی:
      __vpiStringVar(__vpiScope*scope, ثابت نویسه*name, vvp_net_t*net);

      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags) override;
};

بیرونی vpiHandle vpip_make_string_var(ثابت نویسه*name, vvp_net_t*net);

ساختار __vpiArrayBase {
      __vpiArrayBase() : vals_واژگان(پوچ) {}
      مجازی ~__vpiArrayBase() {}

      مجازی unsigned get_size(تهی) ثابت = 0;
      مجازی vpiHandle get_چپ_range() = 0;
      مجازی vpiHandle get_راست_range() = 0;
      مجازی __vpiScope*get_scope() ثابت = 0;

      مجازی صحیح get_word_size() ثابت = 0;
      مجازی نویسه*get_word_str(ساختار __vpiArrayWord*word, صحیح code) = 0;
      مجازی تهی get_word_مقدار(ساختار __vpiArrayWord*word, p_vpi_مقدار vp) = 0;
      مجازی تهی put_word_مقدار(ساختار __vpiArrayWord*word, p_vpi_مقدار vp,
                                    صحیح flags) = 0;
      مجازی vpiHandle get_iter_index(ساختار __vpiArrayIterator*iter, صحیح idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_base_iterate(صحیح code);

      مجازی تهی make_vals_واژگان();

      ساختار __vpiArrayWord*vals_واژگان;
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
ساختار __vpiArray : عمومی __vpiArrayBase, عمومی __vpiHandle {
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiMemory; }
      unsigned get_size() ثابت override { بازگشت array_شمار; }
      vpiHandle get_چپ_range() override { أكد(nets == 0); بازگشت &msb; }
      vpiHandle get_راست_range() override { أكد(nets == 0); بازگشت &lsb; }
      __vpiScope*get_scope() ثابت override { بازگشت scope; }

      صحیح get_word_size() ثابت override;
      نویسه*get_word_str(ساختار __vpiArrayWord*word, صحیح code) override;
      تهی get_word_مقدار(ساختار __vpiArrayWord*word, p_vpi_مقدار vp) override;
      تهی put_word_مقدار(ساختار __vpiArrayWord*word, p_vpi_مقدار vp, صحیح flags) override;

      vpiHandle get_iter_index(ساختار __vpiArrayIterator*iter, صحیح idx) override;

      صحیح vpi_get(صحیح code) override;
      نویسه* vpi_get_str(صحیح code) override;
      vpiHandle vpi_handle(صحیح code) override;
      درونخطی vpiHandle vpi_iterate(صحیح code) override { بازگشت vpi_array_base_iterate(code); }
      vpiHandle vpi_index(صحیح idx) override;

      تهی set_word(unsigned idx, unsigned off, ثابت vvp_vector4_t&val);
      تهی set_word(unsigned idx, دوگانه val);
      تهی set_word(unsigned idx, ثابت std::string&val);
      تهی set_word(unsigned idx, ثابت vvp_object_t&val);

      vvp_vector4_t get_word(unsigned address);
      دوگانه get_word_r(unsigned address);
      تهی get_word_obj(unsigned address, vvp_object_t&val);
      std::string get_word_str(unsigned address);

      تهی alias_word(unsigned long addr, vpiHandle word, صحیح msb, صحیح lsb);
      تهی attach_word(unsigned addr, vpiHandle word);
      تهی word_change(unsigned long addr);

      ثابت نویسه*name; /* Permanently allocated string */
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
      ساختار __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool swap_addr;

خصوصی:
      unsigned array_شمار;
      __vpiScope*scope;

friend vpiHandle vpip_make_array(ثابت نویسه*label, ثابت نویسه*name,
                                 صحیح first_addr, صحیح last_addr,
                                 bool signed_flag);
friend تهی compile_array_alias(نویسه*label, نویسه*name, نویسه*src);
};

class __vpiDarrayVar : عمومی __vpiBaseVar, عمومی __vpiArrayBase {
    عمومی:
      __vpiDarrayVar(__vpiScope*scope, ثابت نویسه*name, vvp_net_t*net);

      صحیح get_type_code() ثابت override { بازگشت vpiArrayVar; }
      unsigned get_size() ثابت override;
      vpiHandle get_چپ_range() override;
      vpiHandle get_راست_range() override;
      __vpiScope*get_scope() ثابت override { بازگشت scope_; }

      صحیح get_word_size() ثابت override;
      نویسه*get_word_str(ساختار __vpiArrayWord*word, صحیح code) override;
      تهی get_word_مقدار(ساختار __vpiArrayWord*word, p_vpi_مقدار vp) override;
      تهی put_word_مقدار(ساختار __vpiArrayWord*word, p_vpi_مقدار vp, صحیح flags) override;

      vpiHandle get_iter_index(ساختار __vpiArrayIterator*iter, صحیح idx) override;
      درونخطی vpiHandle vpi_iterate(صحیح code) override { بازگشت vpi_array_base_iterate(code); }

      صحیح vpi_get(صحیح code) override;
      نویسه* vpi_get_str(صحیح code) override;
      vpiHandle vpi_handle(صحیح code) override;
      vpiHandle vpi_index(صحیح index) override;

      تهی vpi_get_مقدار(p_vpi_مقدار val) override;

    محافظتشده:
      vvp_darray*get_vvp_darray() ثابت;
      __vpiDecConst چپ_range, راست_range;
};

بیرونی vpiHandle vpip_make_darray_var(ثابت نویسه*name, vvp_net_t*net);

class __vpiQueueVar : عمومی __vpiDarrayVar {

    عمومی:
      __vpiQueueVar(__vpiScope*scope, ثابت نویسه*name, vvp_net_t*net);

      صحیح vpi_get(صحیح code) override;
};

بیرونی vpiHandle vpip_make_queue_var(ثابت نویسه*name, vvp_net_t*net);

class __vpiPropQueueRef : عمومی __vpiHandle {

    عمومی:
      explicit __vpiPropQueueRef(__vpiScope*scope, unsigned pidx, bool is_queue);

      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      نویسه* vpi_get_str(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;

      vvp_net_t* class_net_;
      unsigned prop_idx_;
      bool is_queue_;

    خصوصی:
      __vpiScope* scope_;
};

بیرونی vpiHandle vpip_make_prop_queue_ref(نویسه* class_label,
                                          unsigned prop_idx,
                                          unsigned is_queue_flag);
بیرونی vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : عمومی __vpiBaseVar {

    عمومی:
      __vpiCobjectVar(__vpiScope*scope, ثابت نویسه*name, vvp_net_t*net);

      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
};

بیرونی vpiHandle vpip_make_cobject_var(ثابت نویسه*name, vvp_net_t*net);

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
ساختار __vpiUserSystf : عمومی __vpiHandle {
      __vpiUserSystf();
      صحیح get_type_code(تهی) ثابت override;

      s_vpi_systf_data info;
      bool is_user_defn;
};

بیرونی vpiHandle vpip_make_systf_iterator(تهی);

بیرونی ساختار __vpiUserSystf* vpip_find_systf(ثابت نویسه*name);


ساختار __vpiSysTaskCall : عمومی __vpiHandle {

      __vpiScope* scope;
      ساختار __vpiUserSystf*defn;
      unsigned nargs;
      vpiHandle*args;
	/* Stack consumed by this call */
      unsigned vec4_پشته;
      unsigned real_پشته;
      unsigned string_پشته;
	/* Support for vpi_get_userdata. */
      تهی*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      unsigned file_idx;
      unsigned lineno;
      bool put_مقدار;
    محافظتشده:
      درونخطی __vpiSysTaskCall()
      {
	    vec4_پشته = 0;
	    real_پشته = 0;
	    string_پشته = 0;
      }
};

بیرونی ساختار __vpiSysTaskCall*vpip_cur_task;

/*
 * The persistent flag to vpip_make_string_const causes the created
 * handle to be persistent. This is necessary for cases where the
 * string handle may be reused, which is the normal case.
 *
 * When constructing with a string, the class takes possession of the
 * text value string, and will delete it in the constructor.
 */

vpiHandle vpip_make_string_const(نویسه*text, bool persistent =true);
vpiHandle vpip_make_string_param(ثابت نویسه*name, نویسه*مقدار, bool local_flag,
                                 long file_idx, long lineno);

ساختار __vpiBinaryConst : عمومی __vpiHandle {
      __vpiBinaryConst();
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;

      vvp_vector4_t bits;
	/* TRUE if this constant is signed. */
      unsigned signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      unsigned sized_flag  :1;
};

vpiHandle vpip_make_دودویی_const(unsigned wid, ثابت نویسه*bits);
vpiHandle vpip_make_دودویی_param(ثابت نویسه*name, ثابت vvp_vector4_t&bits,
				 bool signed_flag, bool local_flag,
				 long file_idx, long lineno);

class __vpiRealConst : عمومی __vpiHandle {
    عمومی:
      explicit __vpiRealConst(دوگانه);
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;

      دوگانه مقدار;
};

vpiHandle vpip_make_real_const(دوگانه مقدار);
vpiHandle vpip_make_real_param(ثابت نویسه*name, دوگانه مقدار, bool local_flag,
                               long file_idx, long lineno);

class __vpiNullConst : عمومی __vpiHandle {
    عمومی:
      explicit __vpiNullConst();
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_word(unsigned base, ثابت نویسه*type);
vpiHandle vpip_make_vthr_str_پشته(unsigned depth);
vpiHandle vpip_make_vthr_vec4_پشته(unsigned depth, bool signed_flag, unsigned wid);

vpiHandle vpip_make_vthr_A(نویسه*label, unsigned index);
vpiHandle vpip_make_vthr_A(نویسه*label, نویسه*نماد);
vpiHandle vpip_make_vthr_A(نویسه*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(نویسه*label, unsigned index, unsigned bit, unsigned wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
بیرونی "C" تهی vpip_load_module(ثابت نویسه*name);

بیرونی "C" تهی vpip_clear_module_paths();
بیرونی "C" تهی vpip_add_module_path(ثابت نویسه *path);
بیرونی "C" تهی vpip_add_env_and_default_module_paths();

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
بیرونی vpiHandle vpip_build_vpi_call(ثابت نویسه*name,
				     صحیح val_type, unsigned return_width,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     unsigned argc,
				     vpiHandle*argv,
				     unsigned vec4_پشته,
				     unsigned real_پشته,
				     unsigned string_پشته,
				     long file_idx,
				     long lineno);

بیرونی vthread_t vpip_current_vthread;

بیرونی تهی vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_time(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

بیرونی صحیح vpip_get_time_precision(تهی);
بیرونی تهی vpip_set_time_precision(صحیح pres);

بیرونی __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

بیرونی صحیح vpip_time_units_from_handle(vpiHandle obj);
بیرونی صحیح vpip_time_precision_from_handle(vpiHandle obj);

بیرونی دوگانه vpip_scaled_time_from_handle(vvp_time64_t time, vpiHandle obj);

بیرونی تهی vpip_time_to_timestruct(ساختار t_vpi_time*ts, vvp_time64_t ti);
بیرونی vvp_time64_t vpip_timestruct_to_time(ثابت ساختار t_vpi_time*ts);

بیرونی دوگانه vpip_time_to_scaled_real(vvp_time64_t ti, ثابت __vpiScope*sc);
بیرونی vvp_time64_t vpip_scaled_real_to_time64(دوگانه val, ثابت __vpiScope*sc);

/*
 * These functions are used mostly as compile time to strings into
 * permallocated memory. The vpip_string function is the most general,
 * it allocates a fresh string no matter what. The vpip_name_string
 * allocates a string and keeps a pointer in the hash, and tries to
 * reuse it if it can. This us useful for handle names, which may be
 * reused in different scopes.
 */
بیرونی ثابت نویسه* vpip_string(ثابت نویسه*str);
بیرونی ثابت نویسه* vpip_name_string(ثابت نویسه*str);


/*
 * This function is used to make decimal string versions of various
 * vectors. The input format is a vvp_vector4_t, and the result is
 * written into buf, without overflowing nbuf.
 */
بیرونی unsigned vpip_vec4_to_dec_str(ثابت vvp_vector4_t&vec4,
				     نویسه *buf, unsigned صحیح nbuf,
				     صحیح signed_flag);


بیرونی تهی vpip_vec4_to_hex_str(ثابت vvp_vector4_t&bits, نویسه*buf,
				 unsigned nbuf);

بیرونی تهی vpip_vec4_to_oct_str(ثابت vvp_vector4_t&bits, نویسه*buf,
				 unsigned nbuf);

بیرونی تهی vpip_bin_str_to_vec4(vvp_vector4_t&val, ثابت نویسه*buf);
بیرونی تهی vpip_oct_str_to_vec4(vvp_vector4_t&val, ثابت نویسه*str);
بیرونی تهی vpip_dec_str_to_vec4(vvp_vector4_t&val, ثابت نویسه*str);
بیرونی تهی vpip_hex_str_to_vec4(vvp_vector4_t&val, ثابت نویسه*str);

بیرونی vvp_vector4_t vec4_from_vpi_مقدار(s_vpi_مقدار*vp, unsigned wid);
بیرونی دوگانه real_from_vpi_مقدار(s_vpi_مقدار*vp);

بیرونی تهی vpip_vec4_get_مقدار(ثابت vvp_vector4_t&word_val, unsigned width,
				bool signed_flag, s_vpi_مقدار*vp);
بیرونی تهی vpip_vec2_get_مقدار(ثابت vvp_vector2_t&word_val, unsigned width,
				bool signed_flag, s_vpi_مقدار*vp);
بیرونی تهی vpip_real_get_مقدار(دوگانه real, s_vpi_مقدار*vp);
بیرونی تهی vpip_string_get_مقدار(ثابت std::string&val, s_vpi_مقدار*vp);

/*
 * Function defined in vpi_signal.cc to manage vpi_get_* persistent
 * storage.
 */
شمارش vpi_rbuf_t {
      RBUF_VAL =0,
	/* Storage for *_get_value() */
      RBUF_STR,
	/* Storage for *_get_str() */
      RBUF_DEL
	/* Delete the storage for both buffers. */
};
بیرونی تهی *need_result_buf(unsigned cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
بیرونی نویسه *simple_set_rbuf_str(ثابت نویسه *s1);
بیرونی نویسه *generic_get_str(صحیح code, vpiHandle ref, ثابت نویسه *name, ثابت نویسه *index);

/* A routine to find the enclosing module. */
بیرونی vpiHandle vpip_module(__vpiScope*scope);

بیرونی صحیح vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
بیرونی bool vpip_mcd0_disable;

درونخطی uint64_t vlg_round_to_u64(دوگانه rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      بازگشت static_cast<uint64_t>(static_cast<int64_t>(std::round(rval)));
}

#endif /* IVL_vpi_priv_H */
