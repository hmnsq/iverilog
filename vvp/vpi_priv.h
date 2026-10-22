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
#अन्तर्भाव  "delay.h"


class class_type;
class vvp_darray;
class vvp_fun_arrayport;

प्रकारनिर्देश संरचना __vpiArray* vvp_array_t;

/*
 * This header file contains the internal definitions that the vvp
 * program uses to implement the public interface in the vpi_user.h
 * header file elsewhere.
 */

#यदि defined(__MINGW32__) || defined (__CYGWIN__)
बाह्य vpip_routines_s vpi_routines;
#endif

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#define vpi_File_पंक्ति    0x1000003
#define _vpiDescription 0x1000004

बाह्य bool show_file_पंक्ति;
बाह्य bool code_is_instrumented;

बाह्य vpiHandle vpip_build_file_पंक्ति(ध्रुव वर्ण*description,
                                      दीर्घ file_idx, दीर्घ lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
बाह्य ध्रुव वर्ण* vpi_type_as_string(PLI_INT32 code);

/*
 * Return the port direction as a string.
 */
बाह्य ध्रुव वर्ण* direction_as_string(पूर्णाङ्क dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#यदि defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
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
गणना vpi_mode_t {
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
बाह्य vpi_mode_t vpi_mode_flag;

/*
 * This structure is the very base of a vpiHandle. Every handle
 * structure is derived from this class so that the library can
 * internally pass the derived types as pointers to one of these.
 */
class __vpiHandle {
    सार्वजनिक:
      अन्तःस्थ __vpiHandle() { }
	// The destructor is virtual so that dynamic types will work.
      आभास ~__vpiHandle();

      आभास पूर्णाङ्क get_type_code(शून्य) ध्रुव =0;
      आभास पूर्णाङ्क vpi_get(पूर्णाङ्क code);
      आभास int64_t vpi_get64(पूर्णाङ्क code);
      आभास वर्ण* vpi_get_str(पूर्णाङ्क code);

      आभास शून्य vpi_get_मूल्य(p_vpi_मूल्य val);
      आभास vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णाङ्क flags);
      आभास vpiHandle vpi_handle(पूर्णाङ्क code);
      आभास vpiHandle vpi_iterate(पूर्णाङ्क code);
      आभास vpiHandle vpi_index(पूर्णाङ्क idx);
      आभास शून्य vpi_get_delays(p_vpi_delay del);
      आभास शून्य vpi_put_delays(p_vpi_delay del);

	// Objects may have destroyer functions of their own. If so,
	// then this virtual method will return a POINTER to that
	// function. The pointer is used to "delete" the object, which
	// is why the function itself cannot be a method.
      प्रकारनिर्देश पूर्णाङ्क (*free_object_fun_t)(vpiHandle);
      आभास free_object_fun_t free_object_fun(शून्य);
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
संरचना __vpiIterator : सार्वजनिक __vpiHandle {
      __vpiIterator();
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      free_object_fun_t free_object_fun(शून्य) override;

      vpiHandle *args;
      अचिन्हित  nargs;
      अचिन्हित  next;
      bool free_args_flag;
};

बाह्य vpiHandle vpip_make_iterator(अचिन्हित nargs, vpiHandle*args,
				    bool free_args_flag);

class __vpiDecConst : सार्वजनिक __vpiHandle {
    सार्वजनिक:
      explicit __vpiDecConst(पूर्णाङ्क val =0);
      __vpiDecConst(ध्रुव __vpiDecConst&that);
# यदि __cplusplus >= 201103L
      __vpiDecConst& चालक = (ध्रुव __vpiDecConst&that) = स्वभाव;
# endif
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;

    सार्वजनिक:
      अन्तःस्थ पूर्णाङ्क get_मूल्य() ध्रुव { प्रत्यागम मूल्य; }
      अन्तःस्थ शून्य set_मूल्य(पूर्णाङ्क val) { मूल्य = val; }

    गुप्त:
      पूर्णाङ्क मूल्य;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
संरचना __vpiCallback : सार्वजनिक __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;

	// Used for listing callbacks.
      संरचना __vpiCallback*next;

	// user supplied callback data
      संरचना t_cb_data cb_data;
};

class मूल्य_callback : सार्वजनिक __vpiCallback {
    सार्वजनिक:
      explicit मूल्य_callback(p_cb_data data);
	// Return true if the callback really is ready to be called
      आभास bool test_मूल्य_callback_ready(शून्य);

    सार्वजनिक:
	// user supplied callback data
      संरचना t_vpi_time cb_time;
      संरचना t_vpi_मूल्य cb_मूल्य;
};

बाह्य शून्य callback_execute(संरचना __vpiCallback*cur);

संरचना __vpiSystemTime : सार्वजनिक __vpiHandle {
      __vpiSystemTime();
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण*vpi_get_str(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;

      __vpiScope*scope;
};

संरचना __vpiScopedTime : सार्वजनिक __vpiSystemTime {
      __vpiScopedTime();
      वर्ण*vpi_get_str(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;
};
संरचना __vpiScopedSTime : सार्वजनिक __vpiSystemTime {
      __vpiScopedSTime();
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण*vpi_get_str(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;
};
संरचना __vpiScopedRealtime : सार्वजनिक __vpiSystemTime {
      __vpiScopedRealtime();
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण*vpi_get_str(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : सार्वजनिक __vpiHandle {

    सार्वजनिक:
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;
      vpiHandle vpi_iterate(पूर्णाङ्क code) override;

    सार्वजनिक:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      अन्तःस्थ ध्रुव वर्ण*scope_name() ध्रुव { प्रत्यागम name_; }

      अन्तःस्थ ध्रुव वर्ण*scope_def_name() ध्रुव { प्रत्यागम tname_; }
	// TRUE if this is an automatic func/task/block
      अन्तःस्थ bool is_automatic() ध्रुव { प्रत्यागम is_automatic_; }

    सार्वजनिक:
      __vpiScope *scope;
      अचिन्हित file_idx;
      अचिन्हित lineno;
      अचिन्हित def_file_idx;
      अचिन्हित def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_time;
      संरचना __vpiScopedSTime scoped_stime;
      संरचना __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::vector<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      संरचना automatic_hooks_s**item;
      अचिन्हित nitem;
        /* Keep a list of live contexts. */
      vvp_context_t live_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::set<vthread_t> threads;
      चिन्हित पूर्णाङ्क time_units :8;
      चिन्हित पूर्णाङ्क time_precision :8;

    रक्षित:
      __vpiScope(ध्रुव वर्ण*nam, ध्रुव वर्ण*tnam, bool is_auto_flag =false);

    गुप्त:
	/* The scope has a name. */
      ध्रुव वर्ण*name_;
      ध्रुव वर्ण*tname_;
	/* the scope may be "automatic" */
      bool is_automatic_;
};

class vpiScopeFunction  : सार्वजनिक __vpiScope {
    सार्वजनिक:
      अन्तःस्थ vpiScopeFunction(ध्रुव वर्ण*nam, ध्रुव वर्ण*tnam,
			      bool auto_flag, पूर्णाङ्क func_type, अचिन्हित func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { प्रत्यागम vpiFunction; }
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override
      {
	    विकल्प (code) {
		पक्ष vpiFuncType:
		  प्रत्यागम func_type_;
		स्वभाव:
		  प्रत्यागम __vpiScope::vpi_get(code);
	    }
      }

    सार्वजनिक:
      अन्तःस्थ अचिन्हित get_func_width(शून्य) ध्रुव { प्रत्यागम func_wid_; }
      अन्तःस्थ vvp_bit4_t get_func_init_val(शून्य) ध्रुव { प्रत्यागम func_init_val_; }

    गुप्त:
      पूर्णाङ्क func_type_;
      अचिन्हित func_wid_;
      vvp_bit4_t func_init_val_;
};

बाह्य __vpiScope* vpip_peek_current_scope(शून्य);
बाह्य शून्य vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj);
बाह्य शून्य vpip_attach_to_current_scope(vpiHandle obj);
बाह्य __vpiScope* vpip_peek_context_scope(शून्य);
बाह्य अचिन्हित vpip_add_item_to_context(automatic_hooks_s*item,
                                         __vpiScope*scope);
बाह्य vpiHandle vpip_make_मूल_iterator(पूर्णाङ्क type_code);
बाह्य शून्य vpip_make_मूल_iterator(class __vpiHandle**&table,
				    अचिन्हित&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
संरचना __vpiSignal : सार्वजनिक __vpiHandle {
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      int64_t vpi_get64(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णाङ्क flags) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;
      vpiHandle vpi_iterate(पूर्णाङ्क code) override;
      vpiHandle vpi_index(पूर्णाङ्क idx) override;

    सार्वजनिक:
      अचिन्हित width() ध्रुव;
      vpiHandle get_index(पूर्णाङ्क index);
      शून्य get_bit_मूल्य(ध्रुव संरचना __vpiBit*bit, p_vpi_मूल्य vp);
      vpiHandle put_bit_मूल्य(संरचना __vpiBit*bit, p_vpi_मूल्य vp, पूर्णाङ्क flags);
      शून्य make_bits();

      संरचना __vpiBit*bits;

    सार्वजनिक:
      सङ्घ { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
      सङ्घ { // The name of this reg/net, or the index for array words.
            ध्रुव वर्ण*name;
            vpiHandle index;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      अचिन्हित signed_flag  : 1;
      अचिन्हित is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*गाँठ;

    सार्वजनिक:
      स्थिर शून्य*चालक new(std::माप_प्रकार size);
      स्थिर शून्य चालक delete(शून्य*); // not implemented
    रक्षित:
      अन्तःस्थ __vpiSignal() : bits(रिक्त) { }
    गुप्त: // Not implemented
      स्थिर शून्य*चालक new[] (std::माप_प्रकार size);
      स्थिर शून्य चालक delete[](शून्य*);
};
बाह्य अचिन्हित vpip_size(ध्रुव __vpiSignal *sig);
बाह्य __vpiScope* vpip_scope(__vpiSignal*sig);

बाह्य vpiHandle vpip_make_int2(ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb,
			       bool signed_flag, vvp_net_t*vec);
बाह्य vpiHandle vpip_make_int4(ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb,
			       vvp_net_t*vec);
बाह्य vpiHandle vpip_make_var4(ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb,
			       bool signed_flag, vvp_net_t*net);
बाह्य vpiHandle vpip_make_net4(__vpiScope*scope,
				ध्रुव वर्ण*name, पूर्णाङ्क msb, पूर्णाङ्क lsb,
				bool signed_flag, vvp_net_t*गाँठ);

/*
 * This is used to represent a bit in a net/reg.
 */
संरचना __vpiBit {
      संरचना as_bit_t : सार्वजनिक __vpiHandle {
	    पूर्णाङ्क get_type_code(शून्य) ध्रुव;
	    पूर्णाङ्क vpi_get(पूर्णाङ्क code);
	    वर्ण* vpi_get_str(पूर्णाङ्क code);
	    शून्य vpi_get_मूल्य(p_vpi_मूल्य val);
	    vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णाङ्क flags);
	    vpiHandle vpi_handle(पूर्णाङ्क code);
      } as_bit;

      vpiHandle index;

      सङ्घ {
	    संरचना __vpiSignal*parent;
	    संरचना __vpiBit*bit0;
      };

      अन्तःस्थ अचिन्हित get_norm_index() ध्रुव { प्रत्यागम this - bit0; }
      अन्तःस्थ संरचना __vpiSignal*get_parent() ध्रुव {प्रत्यागम (bit0 - 1)->parent; }
      पूर्णाङ्क get_index(शून्य) ध्रुव;
};

class vpiPortBitInfo;

class vpiPortInfo  : सार्वजनिक __vpiHandle {
    सार्वजनिक:
      vpiPortInfo( __vpiScope *parent,
                    अचिन्हित index,
                    पूर्णाङ्क vpi_direction,
                    अचिन्हित width,
                    ध्रुव वर्ण *name,
                    वर्ण* buffer );
      ~vpiPortInfo() override;

      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { प्रत्यागम vpiPort; }
      पूर्णाङ्क get_direction(शून्य) { प्रत्यागम direction_; }
      अचिन्हित get_index(शून्य) { प्रत्यागम index_; }
      पूर्णाङ्क get_width(शून्य) { प्रत्यागम width_; }
      शून्य add_port_bit(vpiPortBitInfo* port_bit) { port_bits_.धकेलो_back(port_bit); }

      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;
      vvp_net_t* get_port(शून्य) ध्रुव { प्रत्यागम ref_; }
      vpiHandle vpi_iterate(पूर्णाङ्क code) override;

      std::vector<vpiPortBitInfo*> port_bits_;

    गुप्त:
      __vpiScope *parent_;
      अचिन्हित  index_;
      पूर्णाङ्क       direction_;
      अचिन्हित  width_;
      ध्रुव वर्ण *name_;
      vvp_net_t *ref_;
};

class vpiPortBitInfo  : सार्वजनिक __vpiHandle {
    सार्वजनिक:
      vpiPortBitInfo(vpiPortInfo *parent,
                     अचिन्हित bit);
      ~vpiPortBitInfo() override;

      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { प्रत्यागम vpiPortBit; }
      अचिन्हित get_bit(शून्य) ध्रुव { प्रत्यागम bit_; }

      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;

    गुप्त:
      vpiPortInfo *parent_;
      अचिन्हित bit_;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
संरचना __vpiPV : सार्वजनिक __vpiHandle {
      __vpiPV();
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णाङ्क flags) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;

      vpiHandle parent;
      vvp_net_t*net;
      vpiHandle sbase;
      पूर्णाङ्क tbase;
      अचिन्हित width;
};
बाह्य vpiHandle vpip_make_PV(वर्ण*name, पूर्णाङ्क base, पूर्णाङ्क width);
बाह्य vpiHandle vpip_make_PV(वर्ण*name, वर्ण*प्रतीक, पूर्णाङ्क width);
बाह्य vpiHandle vpip_make_PV(वर्ण*name, vpiHandle handle, पूर्णाङ्क width);

संरचना __vpiModPathTerm : सार्वजनिक __vpiHandle {
      __vpiModPathTerm();
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;

      vpiHandle व्यंजक;
	/* The value returned by vpi_get(vpiEdge, ...); */
      पूर्णाङ्क edge;
};

संरचना __vpiModPathSrc : सार्वजनिक __vpiHandle {
      __vpiModPathSrc();
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णाङ्क flags) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;
      vpiHandle vpi_iterate(पूर्णाङ्क code) override;
      vpiHandle vpi_index(पूर्णाङ्क idx) override;
      शून्य vpi_get_delays(p_vpi_delay del) override;
      शून्य vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(शून्य) override;

      संरचना __vpiModPath *dest;
      पूर्णाङ्क   type;

	/* This is the input expression for this modpath. */
      संरचना __vpiModPathTerm path_term_in;

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

संरचना __vpiModPath {
      __vpiScope   *scope ;

      class vvp_fun_modpath*modpath;

      संरचना __vpiModPathTerm path_term_out;
      vvp_net_t *निवेश_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

बाह्य संरचना __vpiModPathSrc* vpip_make_modpath_src(संरचना __vpiModPath*path,
                                                     vvp_net_t *net) ;

बाह्य संरचना __vpiModPath* vpip_make_modpath(vvp_net_t *net) ;

/*
 *
 * The vpiInterModPath vpiHandle will define
 * a vpiInterModPath of record .intermodpath as defined
 * in the IEEE 1364
 *
 */

संरचना __vpiInterModPath : सार्वजनिक __vpiHandle {
      __vpiInterModPath();
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णाङ्क flags) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;
      vpiHandle vpi_iterate(पूर्णाङ्क code) override;
      शून्य vpi_get_delays(p_vpi_delay del) override;
      शून्य vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(शून्य) override;

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

बाह्य संरचना __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2);

/*
 * These methods support the vpi creation of events. The name string
 * passed in will be saved, so the caller must allocate it (or not
 * free it) after it is handed to this function.
 */
class __vpiNamedEvent : सार्वजनिक __vpiHandle {

    सार्वजनिक:
      __vpiNamedEvent(__vpiScope*scope, ध्रुव वर्ण*name);
      ~__vpiNamedEvent() override;

      __vpiNamedEvent(ध्रुव __vpiNamedEvent&) = delete;
      __vpiNamedEvent& चालक=(ध्रुव __vpiNamedEvent&) = delete;

      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      __vpiScope*get_scope(शून्य) ध्रुव { प्रत्यागम scope_; }
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णाङ्क flags) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;

      अन्तःस्थ शून्य add_vpi_callback(__vpiCallback*cb)
      { cb->next = callbacks_;
	callbacks_ = cb;
      }

      शून्य run_vpi_callbacks(शून्य);

	/* The functor, used for %set operations. */
      vvp_net_t*funct;

    गुप्त:
	/* base name of the event object */
      ध्रुव वर्ण*name_;
	/* Parent scope of this object. */
      __vpiScope*scope_;
	/* List of callbacks interested in this event. */
      __vpiCallback*callbacks_;
};

बाह्य vpiHandle vpip_make_named_event(ध्रुव वर्ण*name, vvp_net_t*f);

/*
 * Memory is an array of bits that is accessible in N-bit chunks, with
 * N being the width of a word. The memory word handle just points
 * back to the memory and uses an index to identify its position in
 * the memory.
 */

बाह्य bool is_net_array(vpiHandle obj);

/*
 * These are the various variable types.
 */
संरचना __vpiRealVar : सार्वजनिक __vpiHandle {
      __vpiRealVar();
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णाङ्क flags) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;
      vpiHandle vpi_iterate(पूर्णाङ्क code) override;

      सङ्घ { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      सङ्घ {
            ध्रुव वर्ण*name;
            vpiHandle index;
      } id;
      अचिन्हित is_netarray  : 1; // This is word of a net array
      अचिन्हित is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

बाह्य __vpiScope* vpip_scope(__vpiRealVar*sig);
बाह्य vpiHandle vpip_make_real_var(ध्रुव वर्ण*name, vvp_net_t*net);
बाह्य vpiHandle vpip_make_real_net(__vpiScope*scope,
				    ध्रुव वर्ण*name, vvp_net_t*net);

class __vpiBaseVar : सार्वजनिक __vpiHandle {

    सार्वजनिक:
      __vpiBaseVar(__vpiScope*scope, ध्रुव वर्ण*name, vvp_net_t*net);
#ifdef CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#endif

      अन्तःस्थ vvp_net_t* get_net() ध्रुव { प्रत्यागम net_; }

    रक्षित:
      __vpiScope* scope_;
      ध्रुव वर्ण*name_;

    गुप्त:
      vvp_net_t*net_;
};

class __vpiStringVar : सार्वजनिक __vpiBaseVar {

    सार्वजनिक:
      __vpiStringVar(__vpiScope*scope, ध्रुव वर्ण*name, vvp_net_t*net);

      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णाङ्क flags) override;
};

बाह्य vpiHandle vpip_make_string_var(ध्रुव वर्ण*name, vvp_net_t*net);

संरचना __vpiArrayBase {
      __vpiArrayBase() : vals_शब्दगण(रिक्त) {}
      आभास ~__vpiArrayBase() {}

      आभास अचिन्हित get_size(शून्य) ध्रुव = 0;
      आभास vpiHandle get_बायाँ_range() = 0;
      आभास vpiHandle get_दायाँ_range() = 0;
      आभास __vpiScope*get_scope() ध्रुव = 0;

      आभास पूर्णाङ्क get_word_size() ध्रुव = 0;
      आभास वर्ण*get_word_str(संरचना __vpiArrayWord*word, पूर्णाङ्क code) = 0;
      आभास शून्य get_word_मूल्य(संरचना __vpiArrayWord*word, p_vpi_मूल्य vp) = 0;
      आभास शून्य put_word_मूल्य(संरचना __vpiArrayWord*word, p_vpi_मूल्य vp,
                                    पूर्णाङ्क flags) = 0;
      आभास vpiHandle get_iter_index(संरचना __vpiArrayIterator*iter, पूर्णाङ्क idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_base_iterate(पूर्णाङ्क code);

      आभास शून्य make_vals_शब्दगण();

      संरचना __vpiArrayWord*vals_शब्दगण;
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
संरचना __vpiArray : सार्वजनिक __vpiArrayBase, सार्वजनिक __vpiHandle {
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { प्रत्यागम vpiMemory; }
      अचिन्हित get_size() ध्रुव override { प्रत्यागम array_गिनती; }
      vpiHandle get_बायाँ_range() override { प्रतिज्ञा(nets == 0); प्रत्यागम &msb; }
      vpiHandle get_दायाँ_range() override { प्रतिज्ञा(nets == 0); प्रत्यागम &lsb; }
      __vpiScope*get_scope() ध्रुव override { प्रत्यागम scope; }

      पूर्णाङ्क get_word_size() ध्रुव override;
      वर्ण*get_word_str(संरचना __vpiArrayWord*word, पूर्णाङ्क code) override;
      शून्य get_word_मूल्य(संरचना __vpiArrayWord*word, p_vpi_मूल्य vp) override;
      शून्य put_word_मूल्य(संरचना __vpiArrayWord*word, p_vpi_मूल्य vp, पूर्णाङ्क flags) override;

      vpiHandle get_iter_index(संरचना __vpiArrayIterator*iter, पूर्णाङ्क idx) override;

      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;
      अन्तःस्थ vpiHandle vpi_iterate(पूर्णाङ्क code) override { प्रत्यागम vpi_array_base_iterate(code); }
      vpiHandle vpi_index(पूर्णाङ्क idx) override;

      शून्य set_word(अचिन्हित idx, अचिन्हित off, ध्रुव vvp_vector4_t&val);
      शून्य set_word(अचिन्हित idx, द्विगुण val);
      शून्य set_word(अचिन्हित idx, ध्रुव std::string&val);
      शून्य set_word(अचिन्हित idx, ध्रुव vvp_object_t&val);

      vvp_vector4_t get_word(अचिन्हित address);
      द्विगुण get_word_r(अचिन्हित address);
      शून्य get_word_obj(अचिन्हित address, vvp_object_t&val);
      std::string get_word_str(अचिन्हित address);

      शून्य alias_word(अचिन्हित दीर्घ addr, vpiHandle word, पूर्णाङ्क msb, पूर्णाङ्क lsb);
      शून्य attach_word(अचिन्हित addr, vpiHandle word);
      शून्य word_change(अचिन्हित दीर्घ addr);

      ध्रुव वर्ण*name; /* Permanently allocated string */
      __vpiDecConst first_addr;
      __vpiDecConst last_addr;
      __vpiDecConst msb;
      __vpiDecConst lsb;
      अचिन्हित vals_width;
	// If this is a net array, nets lists the handles.
      vpiHandle*nets;
	// If this is a var array, then these are used instead of nets.
      vvp_vector4array_t*vals4;
      vvp_darray        *vals;

      vvp_fun_arrayport*ports_;
      संरचना __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool swap_addr;

गुप्त:
      अचिन्हित array_गिनती;
      __vpiScope*scope;

मित्र vpiHandle vpip_make_array(ध्रुव वर्ण*label, ध्रुव वर्ण*name,
                                 पूर्णाङ्क first_addr, पूर्णाङ्क last_addr,
                                 bool signed_flag);
मित्र शून्य compile_array_alias(वर्ण*label, वर्ण*name, वर्ण*src);
};

class __vpiDarrayVar : सार्वजनिक __vpiBaseVar, सार्वजनिक __vpiArrayBase {
    सार्वजनिक:
      __vpiDarrayVar(__vpiScope*scope, ध्रुव वर्ण*name, vvp_net_t*net);

      पूर्णाङ्क get_type_code() ध्रुव override { प्रत्यागम vpiArrayVar; }
      अचिन्हित get_size() ध्रुव override;
      vpiHandle get_बायाँ_range() override;
      vpiHandle get_दायाँ_range() override;
      __vpiScope*get_scope() ध्रुव override { प्रत्यागम scope_; }

      पूर्णाङ्क get_word_size() ध्रुव override;
      वर्ण*get_word_str(संरचना __vpiArrayWord*word, पूर्णाङ्क code) override;
      शून्य get_word_मूल्य(संरचना __vpiArrayWord*word, p_vpi_मूल्य vp) override;
      शून्य put_word_मूल्य(संरचना __vpiArrayWord*word, p_vpi_मूल्य vp, पूर्णाङ्क flags) override;

      vpiHandle get_iter_index(संरचना __vpiArrayIterator*iter, पूर्णाङ्क idx) override;
      अन्तःस्थ vpiHandle vpi_iterate(पूर्णाङ्क code) override { प्रत्यागम vpi_array_base_iterate(code); }

      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;
      vpiHandle vpi_index(पूर्णाङ्क index) override;

      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;

    रक्षित:
      vvp_darray*get_vvp_darray() ध्रुव;
      __vpiDecConst बायाँ_range, दायाँ_range;
};

बाह्य vpiHandle vpip_make_darray_var(ध्रुव वर्ण*name, vvp_net_t*net);

class __vpiQueueVar : सार्वजनिक __vpiDarrayVar {

    सार्वजनिक:
      __vpiQueueVar(__vpiScope*scope, ध्रुव वर्ण*name, vvp_net_t*net);

      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
};

बाह्य vpiHandle vpip_make_queue_var(ध्रुव वर्ण*name, vvp_net_t*net);

class __vpiPropQueueRef : सार्वजनिक __vpiHandle {

    सार्वजनिक:
      explicit __vpiPropQueueRef(__vpiScope*scope, अचिन्हित pidx, bool is_queue);

      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;

      vvp_net_t* class_net_;
      अचिन्हित prop_idx_;
      bool is_queue_;

    गुप्त:
      __vpiScope* scope_;
};

बाह्य vpiHandle vpip_make_prop_queue_ref(वर्ण* class_label,
                                          अचिन्हित prop_idx,
                                          अचिन्हित is_queue_flag);
बाह्य vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : सार्वजनिक __vpiBaseVar {

    सार्वजनिक:
      __vpiCobjectVar(__vpiScope*scope, ध्रुव वर्ण*name, vvp_net_t*net);

      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;
};

बाह्य vpiHandle vpip_make_cobject_var(ध्रुव वर्ण*name, vvp_net_t*net);

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
संरचना __vpiUserSystf : सार्वजनिक __vpiHandle {
      __vpiUserSystf();
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;

      s_vpi_systf_data info;
      bool is_user_defn;
};

बाह्य vpiHandle vpip_make_systf_iterator(शून्य);

बाह्य संरचना __vpiUserSystf* vpip_find_systf(ध्रुव वर्ण*name);


संरचना __vpiSysTaskCall : सार्वजनिक __vpiHandle {

      __vpiScope* scope;
      संरचना __vpiUserSystf*defn;
      अचिन्हित nargs;
      vpiHandle*args;
	/* Stack consumed by this call */
      अचिन्हित vec4_ढेर;
      अचिन्हित real_ढेर;
      अचिन्हित string_ढेर;
	/* Support for vpi_get_userdata. */
      शून्य*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      अचिन्हित file_idx;
      अचिन्हित lineno;
      bool put_मूल्य;
    रक्षित:
      अन्तःस्थ __vpiSysTaskCall()
      {
	    vec4_ढेर = 0;
	    real_ढेर = 0;
	    string_ढेर = 0;
      }
};

बाह्य संरचना __vpiSysTaskCall*vpip_cur_task;

/*
 * The persistent flag to vpip_make_string_const causes the created
 * handle to be persistent. This is necessary for cases where the
 * string handle may be reused, which is the normal case.
 *
 * When constructing with a string, the class takes possession of the
 * text value string, and will delete it in the constructor.
 */

vpiHandle vpip_make_string_const(वर्ण*text, bool persistent =true);
vpiHandle vpip_make_string_param(ध्रुव वर्ण*name, वर्ण*मूल्य, bool local_flag,
                                 दीर्घ file_idx, दीर्घ lineno);

संरचना __vpiBinaryConst : सार्वजनिक __vpiHandle {
      __vpiBinaryConst();
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;

      vvp_vector4_t bits;
	/* TRUE if this constant is signed. */
      अचिन्हित signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      अचिन्हित sized_flag  :1;
};

vpiHandle vpip_make_द्विआधारी_const(अचिन्हित wid, ध्रुव वर्ण*bits);
vpiHandle vpip_make_द्विआधारी_param(ध्रुव वर्ण*name, ध्रुव vvp_vector4_t&bits,
				 bool signed_flag, bool local_flag,
				 दीर्घ file_idx, दीर्घ lineno);

class __vpiRealConst : सार्वजनिक __vpiHandle {
    सार्वजनिक:
      explicit __vpiRealConst(द्विगुण);
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;

      द्विगुण मूल्य;
};

vpiHandle vpip_make_real_const(द्विगुण मूल्य);
vpiHandle vpip_make_real_param(ध्रुव वर्ण*name, द्विगुण मूल्य, bool local_flag,
                               दीर्घ file_idx, दीर्घ lineno);

class __vpiNullConst : सार्वजनिक __vpiHandle {
    सार्वजनिक:
      explicit __vpiNullConst();
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_word(अचिन्हित base, ध्रुव वर्ण*type);
vpiHandle vpip_make_vthr_str_ढेर(अचिन्हित depth);
vpiHandle vpip_make_vthr_vec4_ढेर(अचिन्हित depth, bool signed_flag, अचिन्हित wid);

vpiHandle vpip_make_vthr_A(वर्ण*label, अचिन्हित index);
vpiHandle vpip_make_vthr_A(वर्ण*label, वर्ण*प्रतीक);
vpiHandle vpip_make_vthr_A(वर्ण*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(वर्ण*label, अचिन्हित index, अचिन्हित bit, अचिन्हित wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
बाह्य "C" शून्य vpip_load_module(ध्रुव वर्ण*name);

बाह्य "C" शून्य vpip_clear_module_paths();
बाह्य "C" शून्य vpip_add_module_path(ध्रुव वर्ण *path);
बाह्य "C" शून्य vpip_add_env_and_default_module_paths();

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
बाह्य vpiHandle vpip_build_vpi_call(ध्रुव वर्ण*name,
				     पूर्णाङ्क val_type, अचिन्हित return_width,
				     class vvp_net_t*fnet,
				     bool func_as_task_err,
				     bool func_as_task_warn,
				     अचिन्हित argc,
				     vpiHandle*argv,
				     अचिन्हित vec4_ढेर,
				     अचिन्हित real_ढेर,
				     अचिन्हित string_ढेर,
				     दीर्घ file_idx,
				     दीर्घ lineno);

बाह्य vthread_t vpip_current_vthread;

बाह्य शून्य vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_time(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

बाह्य पूर्णाङ्क vpip_get_time_precision(शून्य);
बाह्य शून्य vpip_set_time_precision(पूर्णाङ्क pres);

बाह्य __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

बाह्य पूर्णाङ्क vpip_time_units_from_handle(vpiHandle obj);
बाह्य पूर्णाङ्क vpip_time_precision_from_handle(vpiHandle obj);

बाह्य द्विगुण vpip_scaled_time_from_handle(vvp_time64_t समय, vpiHandle obj);

बाह्य शून्य vpip_time_to_timestruct(संरचना t_vpi_time*ts, vvp_time64_t ti);
बाह्य vvp_time64_t vpip_timestruct_to_time(ध्रुव संरचना t_vpi_time*ts);

बाह्य द्विगुण vpip_time_to_scaled_real(vvp_time64_t ti, ध्रुव __vpiScope*sc);
बाह्य vvp_time64_t vpip_scaled_real_to_time64(द्विगुण val, ध्रुव __vpiScope*sc);

/*
 * These functions are used mostly as compile time to strings into
 * permallocated memory. The vpip_string function is the most general,
 * it allocates a fresh string no matter what. The vpip_name_string
 * allocates a string and keeps a pointer in the hash, and tries to
 * reuse it if it can. This us useful for handle names, which may be
 * reused in different scopes.
 */
बाह्य ध्रुव वर्ण* vpip_string(ध्रुव वर्ण*str);
बाह्य ध्रुव वर्ण* vpip_name_string(ध्रुव वर्ण*str);


/*
 * This function is used to make decimal string versions of various
 * vectors. The input format is a vvp_vector4_t, and the result is
 * written into buf, without overflowing nbuf.
 */
बाह्य अचिन्हित vpip_vec4_to_dec_str(ध्रुव vvp_vector4_t&vec4,
				     वर्ण *buf, अचिन्हित पूर्णाङ्क nbuf,
				     पूर्णाङ्क signed_flag);


बाह्य शून्य vpip_vec4_to_hex_str(ध्रुव vvp_vector4_t&bits, वर्ण*buf,
				 अचिन्हित nbuf);

बाह्य शून्य vpip_vec4_to_oct_str(ध्रुव vvp_vector4_t&bits, वर्ण*buf,
				 अचिन्हित nbuf);

बाह्य शून्य vpip_bin_str_to_vec4(vvp_vector4_t&val, ध्रुव वर्ण*buf);
बाह्य शून्य vpip_oct_str_to_vec4(vvp_vector4_t&val, ध्रुव वर्ण*str);
बाह्य शून्य vpip_dec_str_to_vec4(vvp_vector4_t&val, ध्रुव वर्ण*str);
बाह्य शून्य vpip_hex_str_to_vec4(vvp_vector4_t&val, ध्रुव वर्ण*str);

बाह्य vvp_vector4_t vec4_from_vpi_मूल्य(s_vpi_मूल्य*vp, अचिन्हित wid);
बाह्य द्विगुण real_from_vpi_मूल्य(s_vpi_मूल्य*vp);

बाह्य शून्य vpip_vec4_get_मूल्य(ध्रुव vvp_vector4_t&word_val, अचिन्हित width,
				bool signed_flag, s_vpi_मूल्य*vp);
बाह्य शून्य vpip_vec2_get_मूल्य(ध्रुव vvp_vector2_t&word_val, अचिन्हित width,
				bool signed_flag, s_vpi_मूल्य*vp);
बाह्य शून्य vpip_real_get_मूल्य(द्विगुण real, s_vpi_मूल्य*vp);
बाह्य शून्य vpip_string_get_मूल्य(ध्रुव std::string&val, s_vpi_मूल्य*vp);

/*
 * Function defined in vpi_signal.cc to manage vpi_get_* persistent
 * storage.
 */
गणना vpi_rbuf_t {
      RBUF_VAL =0,
	/* Storage for *_get_value() */
      RBUF_STR,
	/* Storage for *_get_str() */
      RBUF_DEL
	/* Delete the storage for both buffers. */
};
बाह्य शून्य *need_result_buf(अचिन्हित cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
बाह्य वर्ण *simple_set_rbuf_str(ध्रुव वर्ण *s1);
बाह्य वर्ण *generic_get_str(पूर्णाङ्क code, vpiHandle ref, ध्रुव वर्ण *name, ध्रुव वर्ण *index);

/* A routine to find the enclosing module. */
बाह्य vpiHandle vpip_module(__vpiScope*scope);

बाह्य पूर्णाङ्क vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
बाह्य bool vpip_mcd0_disable;

अन्तःस्थ uint64_t vlg_round_to_u64(द्विगुण rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      प्रत्यागम static_cast<uint64_t>(static_cast<int64_t>(std::round(rval)));
}

#endif /* IVL_vpi_priv_H */
