#अगर_अघोषित IVL_vpi_priv_H
#घोषणा IVL_vpi_priv_H
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
#समावेश  "delay.h"


class class_type;
class vvp_darray;
class vvp_fun_arrayport;

प्रकार काष्ठा __vpiArray* vvp_array_t;

/*
 * This header file contains the internal definitions that the vvp
 * program uses to implement the public interface in the vpi_user.h
 * header file elsewhere.
 */

#अगर defined(__MINGW32__) || defined (__CYGWIN__)
बाह्य vpip_routines_s vpi_routines;
#पूर्ण_अगर

/*
 * Routines/definitions used to build the file/line number tracing object.
 */
#घोषणा vpi_File_पंक्ति    0x1000003
#घोषणा _vpiDescription 0x1000004

बाह्य bool show_file_पंक्ति;
बाह्य bool code_is_instrumented;

बाह्य vpiHandle vpip_build_file_पंक्ति(स्थिर अक्षर*description,
                                      दीर्घ file_idx, दीर्घ lineno);

/*
 * vpi_type_values returns the text form of the vpi type. If the type name
 * is unknown, then return the number stringified. Note that the return
 * value may return to a static buffer.
 */
बाह्य स्थिर अक्षर* vpi_type_as_string(PLI_INT32 code);

/*
 * Return the port direction as a string.
 */
बाह्य स्थिर अक्षर* direction_as_string(पूर्णांक dir);

/*
 * Private VPI properties that are only used in the cleanup code.
 */
#अगर defined(CHECK_WITH_VALGRIND) && !defined(BR916_STOPGAP_FIX)
#घोषणा _vpiFromThr 0x1000001
#   define _vpiNoThr   0
#   define _vpiString  1
#   define _vpiVThr    2
#   define _vpiWord    3
#   define _vpi_at_PV  4
#   define _vpi_at_A   5
#   define _vpi_at_APV 6
#पूर्ण_अगर


/*
 * The vpi_mode_flag contains the major mode for VPI use. This is used
 * to generate error messages when vpi functions are called
 * incorrectly.
 */
क्रमागत vpi_mode_t {
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
    खुला:
      अंतरभूत __vpiHandle() { }
	// The destructor is virtual so that dynamic types will work.
      भव ~__vpiHandle();

      भव पूर्णांक get_type_code(व्योम) स्थिर =0;
      भव पूर्णांक vpi_get(पूर्णांक code);
      भव int64_t vpi_get64(पूर्णांक code);
      भव अक्षर* vpi_get_str(पूर्णांक code);

      भव व्योम vpi_get_मूल्य(p_vpi_मूल्य val);
      भव vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags);
      भव vpiHandle vpi_handle(पूर्णांक code);
      भव vpiHandle vpi_iterate(पूर्णांक code);
      भव vpiHandle vpi_सूचकांक(पूर्णांक idx);
      भव व्योम vpi_get_delays(p_vpi_delay del);
      भव व्योम vpi_put_delays(p_vpi_delay del);

	// Objects may have destroyer functions of their own. If so,
	// then this virtual method will return a POINTER to that
	// function. The pointer is used to "delete" the object, which
	// is why the function itself cannot be a method.
      प्रकार पूर्णांक (*free_object_fun_t)(vpiHandle);
      भव free_object_fun_t free_object_fun(व्योम);
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
काष्ठा __vpiIterator : खुला __vpiHandle {
      __vpiIterator();
      पूर्णांक get_type_code(व्योम) स्थिर override;
      free_object_fun_t free_object_fun(व्योम) override;

      vpiHandle *args;
      अचिन्हित  nargs;
      अचिन्हित  अग्र_2;
      bool free_args_flag;
};

बाह्य vpiHandle vpip_make_iterator(अचिन्हित nargs, vpiHandle*args,
				    bool free_args_flag);

class __vpiDecConst : खुला __vpiHandle {
    खुला:
      explicit __vpiDecConst(पूर्णांक val =0);
      __vpiDecConst(स्थिर __vpiDecConst&that);
# अगर __cplusplus >= 201103L
      __vpiDecConst& चालक = (स्थिर __vpiDecConst&that) = शेष;
# endif
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;

    खुला:
      अंतरभूत पूर्णांक get_मूल्य() स्थिर { वापस मूल्य; }
      अंतरभूत व्योम set_मूल्य(पूर्णांक val) { मूल्य = val; }

    निजी:
      पूर्णांक मूल्य;
};

/*
 * This represents callback handles. There are some private types that
 * are defined and used in vpi_callback.cc. The __vpiCallback are
 * always used in association with vvp_vpi_callback objects.
 */
काष्ठा __vpiCallback : खुला __vpiHandle {
      __vpiCallback();
      ~__vpiCallback() override;
      पूर्णांक get_type_code(व्योम) स्थिर override;

	// Used for listing callbacks.
      काष्ठा __vpiCallback*अग्र_2;

	// user supplied callback data
      काष्ठा t_cb_तथ्य cb_तथ्य;
};

class मूल्य_callback : खुला __vpiCallback {
    खुला:
      explicit मूल्य_callback(p_cb_तथ्य तथ्य);
	// Return true if the callback really is ready to be called
      भव bool test_मूल्य_callback_ready(व्योम);

    खुला:
	// user supplied callback data
      काष्ठा t_vpi_time cb_time;
      काष्ठा t_vpi_मूल्य cb_मूल्य;
};

बाह्य व्योम callback_execute(काष्ठा __vpiCallback*cur);

काष्ठा __vpiSystemTime : खुला __vpiHandle {
      __vpiSystemTime();
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर*vpi_get_str(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_handle(पूर्णांक code) override;

      __vpiScope*scope;
};

काष्ठा __vpiScopedTime : खुला __vpiSystemTime {
      __vpiScopedTime();
      अक्षर*vpi_get_str(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
};
काष्ठा __vpiScopedSTime : खुला __vpiSystemTime {
      __vpiScopedSTime();
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर*vpi_get_str(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
};
काष्ठा __vpiScopedRealtime : खुला __vpiSystemTime {
      __vpiScopedRealtime();
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर*vpi_get_str(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
};

/*
 * Scopes are created by .scope statements in the source. These
 * objects hold the items and properties that are knowingly bound to a
 * scope.
 */
class __vpiScope : खुला __vpiHandle {

    खुला:
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;
      vpiHandle vpi_handle(पूर्णांक code) override;
      vpiHandle vpi_iterate(पूर्णांक code) override;

    खुला:
	// Return the BASE name of the scope. This does not include
	// any of the parent hierarchy.
      अंतरभूत स्थिर अक्षर*scope_name() स्थिर { वापस name_; }

      अंतरभूत स्थिर अक्षर*scope_def_name() स्थिर { वापस tname_; }
	// TRUE if this is an automatic func/task/block
      अंतरभूत bool is_automatic() स्थिर { वापस is_automatic_; }

    खुला:
      __vpiScope *scope;
      अचिन्हित file_idx;
      अचिन्हित lineno;
      अचिन्हित def_file_idx;
      अचिन्हित def_lineno;
      bool is_cell;
	/* The scope has a system time of its own. */
      __vpiScopedTime scoped_time;
      काष्ठा __vpiScopedSTime scoped_stime;
      काष्ठा __vpiScopedRealtime scoped_realtime;
	/* Keep an array of internal scope items. */
      std::vector<class __vpiHandle*> intern;
	/* Set of types */
      std::map<std::string,class_type*> classes;
        /* Keep an array of items to be automatically allocated */
      काष्ठा automatic_hooks_s**item;
      अचिन्हित nitem;
        /* Keep a list of live contexts. */
      vvp_context_t live_contexts;
        /* Keep a list of freed contexts. */
      vvp_context_t free_contexts;
	/* Keep a list of threads in the scope. */
      std::set<vthread_t> threads;
      चिन्हित पूर्णांक time_units :8;
      चिन्हित पूर्णांक time_precision :8;

    रक्षित:
      __vpiScope(स्थिर अक्षर*nam, स्थिर अक्षर*tnam, bool is_auto_flag =false);

    निजी:
	/* The scope has a name. */
      स्थिर अक्षर*name_;
      स्थिर अक्षर*tname_;
	/* the scope may be "automatic" */
      bool is_automatic_;
};

class vpiScopeFunction  : खुला __vpiScope {
    खुला:
      अंतरभूत vpiScopeFunction(स्थिर अक्षर*nam, स्थिर अक्षर*tnam,
			      bool auto_flag, पूर्णांक func_type, अचिन्हित func_wid, vvp_bit4_t func_init_val)
      : __vpiScope(nam,tnam, auto_flag), func_type_(func_type), func_wid_(func_wid), func_init_val_(func_init_val)
      { }

      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiFunction; }
      पूर्णांक vpi_get(पूर्णांक code) override
      {
	    चयन (code) {
		हाल vpiFuncType:
		  वापस func_type_;
		शेष:
		  वापस __vpiScope::vpi_get(code);
	    }
      }

    खुला:
      अंतरभूत अचिन्हित get_func_width(व्योम) स्थिर { वापस func_wid_; }
      अंतरभूत vvp_bit4_t get_func_init_val(व्योम) स्थिर { वापस func_init_val_; }

    निजी:
      पूर्णांक func_type_;
      अचिन्हित func_wid_;
      vvp_bit4_t func_init_val_;
};

बाह्य __vpiScope* vpip_peek_current_scope(व्योम);
बाह्य व्योम vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj);
बाह्य व्योम vpip_attach_to_current_scope(vpiHandle obj);
बाह्य __vpiScope* vpip_peek_context_scope(व्योम);
बाह्य अचिन्हित vpip_add_item_to_context(automatic_hooks_s*item,
                                         __vpiScope*scope);
बाह्य vpiHandle vpip_make_मूल_iterator(पूर्णांक type_code);
बाह्य व्योम vpip_make_मूल_iterator(class __vpiHandle**&table,
				    अचिन्हित&ntable);

/*
 * Signals include the variable types (reg, integer, time) and are
 * distinguished by the vpiType code. They also have a parent scope,
 * a declared name and declaration indices.
 */
काष्ठा __vpiSignal : खुला __vpiHandle {
      पूर्णांक vpi_get(पूर्णांक code) override;
      int64_t vpi_get64(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
      vpiHandle vpi_handle(पूर्णांक code) override;
      vpiHandle vpi_iterate(पूर्णांक code) override;
      vpiHandle vpi_सूचकांक(पूर्णांक idx) override;

    खुला:
      अचिन्हित width() स्थिर;
      vpiHandle get_सूचकांक(पूर्णांक सूचकांक);
      व्योम get_bit_मूल्य(स्थिर काष्ठा __vpiBit*bit, p_vpi_मूल्य vp);
      vpiHandle put_bit_मूल्य(काष्ठा __vpiBit*bit, p_vpi_मूल्य vp, पूर्णांक flags);
      व्योम make_bits();

      काष्ठा __vpiBit*bits;

    खुला:
      जोड़ { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
      जोड़ { // The name of this reg/net, or the index for array words.
            स्थिर अक्षर*name;
            vpiHandle सूचकांक;
      } id;
	/* The indices that define the width and access offset. */
      __vpiDecConst msb, lsb;
	/* Flags */
      अचिन्हित signed_flag  : 1;
      अचिन्हित is_netarray  : 1; // This is word of a net array
	/* The represented value is here. */
      vvp_net_t*गाँठ;

    खुला:
      अटल व्योम*चालक new(std::माप_प्रकार आकार);
      अटल व्योम चालक delete(व्योम*); // not implemented
    रक्षित:
      अंतरभूत __vpiSignal() : bits(शून्य) { }
    निजी: // Not implemented
      अटल व्योम*चालक new[] (std::माप_प्रकार आकार);
      अटल व्योम चालक delete[](व्योम*);
};
बाह्य अचिन्हित vpip_आकार(स्थिर __vpiSignal *sig);
बाह्य __vpiScope* vpip_scope(__vpiSignal*sig);

बाह्य vpiHandle vpip_make_int2(स्थिर अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
			       bool signed_flag, vvp_net_t*vec);
बाह्य vpiHandle vpip_make_int4(स्थिर अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
			       vvp_net_t*vec);
बाह्य vpiHandle vpip_make_var4(स्थिर अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
			       bool signed_flag, vvp_net_t*net);
बाह्य vpiHandle vpip_make_net4(__vpiScope*scope,
				स्थिर अक्षर*name, पूर्णांक msb, पूर्णांक lsb,
				bool signed_flag, vvp_net_t*गाँठ);

/*
 * This is used to represent a bit in a net/reg.
 */
काष्ठा __vpiBit {
      काष्ठा as_bit_t : खुला __vpiHandle {
	    पूर्णांक get_type_code(व्योम) स्थिर;
	    पूर्णांक vpi_get(पूर्णांक code);
	    अक्षर* vpi_get_str(पूर्णांक code);
	    व्योम vpi_get_मूल्य(p_vpi_मूल्य val);
	    vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags);
	    vpiHandle vpi_handle(पूर्णांक code);
      } as_bit;

      vpiHandle सूचकांक;

      जोड़ {
	    काष्ठा __vpiSignal*parent;
	    काष्ठा __vpiBit*bit0;
      };

      अंतरभूत अचिन्हित get_norm_सूचकांक() स्थिर { वापस this - bit0; }
      अंतरभूत काष्ठा __vpiSignal*get_parent() स्थिर {वापस (bit0 - 1)->parent; }
      पूर्णांक get_सूचकांक(व्योम) स्थिर;
};

class vpiPortBitInfo;

class vpiPortInfo  : खुला __vpiHandle {
    खुला:
      vpiPortInfo( __vpiScope *parent,
                    अचिन्हित सूचकांक,
                    पूर्णांक vpi_direction,
                    अचिन्हित width,
                    स्थिर अक्षर *name,
                    अक्षर* बफर );
      ~vpiPortInfo() override;

      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiPort; }
      पूर्णांक get_direction(व्योम) { वापस direction_; }
      अचिन्हित get_सूचकांक(व्योम) { वापस सूचकांक_2; }
      पूर्णांक get_width(व्योम) { वापस width_; }
      व्योम add_port_bit(vpiPortBitInfo* port_bit) { port_bits_.धकेलो_back(port_bit); }

      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;
      vpiHandle vpi_handle(पूर्णांक code) override;
      vvp_net_t* get_port(व्योम) स्थिर { वापस ref_; }
      vpiHandle vpi_iterate(पूर्णांक code) override;

      std::vector<vpiPortBitInfo*> port_bits_;

    निजी:
      __vpiScope *parent_;
      अचिन्हित  सूचकांक_2;
      पूर्णांक       direction_;
      अचिन्हित  width_;
      स्थिर अक्षर *name_;
      vvp_net_t *ref_;
};

class vpiPortBitInfo  : खुला __vpiHandle {
    खुला:
      vpiPortBitInfo(vpiPortInfo *parent,
                     अचिन्हित bit);
      ~vpiPortBitInfo() override;

      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiPortBit; }
      अचिन्हित get_bit(व्योम) स्थिर { वापस bit_; }

      पूर्णांक vpi_get(पूर्णांक code) override;
      vpiHandle vpi_handle(पूर्णांक code) override;

    निजी:
      vpiPortInfo *parent_;
      अचिन्हित bit_;
};

/*
 * This is used by system calls to represent a bit/part select of
 * a simple variable or constant array word.
 */
काष्ठा __vpiPV : खुला __vpiHandle {
      __vpiPV();
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
      vpiHandle vpi_handle(पूर्णांक code) override;

      vpiHandle parent;
      vvp_net_t*net;
      vpiHandle sbase;
      पूर्णांक tbase;
      अचिन्हित width;
};
बाह्य vpiHandle vpip_make_PV(अक्षर*name, पूर्णांक base, पूर्णांक width);
बाह्य vpiHandle vpip_make_PV(अक्षर*name, अक्षर*प्रतीक, पूर्णांक width);
बाह्य vpiHandle vpip_make_PV(अक्षर*name, vpiHandle handle, पूर्णांक width);

काष्ठा __vpiModPathTerm : खुला __vpiHandle {
      __vpiModPathTerm();
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      vpiHandle vpi_handle(पूर्णांक code) override;

      vpiHandle व्यंजक;
	/* The value returned by vpi_get(vpiEdge, ...); */
      पूर्णांक edge;
};

काष्ठा __vpiModPathSrc : खुला __vpiHandle {
      __vpiModPathSrc();
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
      vpiHandle vpi_handle(पूर्णांक code) override;
      vpiHandle vpi_iterate(पूर्णांक code) override;
      vpiHandle vpi_सूचकांक(पूर्णांक idx) override;
      व्योम vpi_get_delays(p_vpi_delay del) override;
      व्योम vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(व्योम) override;

      काष्ठा __vpiModPath *dest;
      पूर्णांक   type;

	/* This is the input expression for this modpath. */
      काष्ठा __vpiModPathTerm path_term_in;

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

काष्ठा __vpiModPath {
      __vpiScope   *scope ;

      class vvp_fun_modpath*modpath;

      काष्ठा __vpiModPathTerm path_term_out;
      vvp_net_t *निवेश_net  ;
};


/*
 * The Function is used to create the vpiHandle
 * for vpiModPath && vpiModPathIn objects
 */

बाह्य काष्ठा __vpiModPathSrc* vpip_make_modpath_src(काष्ठा __vpiModPath*path,
                                                     vvp_net_t *net) ;

बाह्य काष्ठा __vpiModPath* vpip_make_modpath(vvp_net_t *net) ;

/*
 *
 * The vpiInterModPath vpiHandle will define
 * a vpiInterModPath of record .intermodpath as defined
 * in the IEEE 1364
 *
 */

काष्ठा __vpiInterModPath : खुला __vpiHandle {
      __vpiInterModPath();
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
      vpiHandle vpi_handle(पूर्णांक code) override;
      vpiHandle vpi_iterate(पूर्णांक code) override;
      व्योम vpi_get_delays(p_vpi_delay del) override;
      व्योम vpi_put_delays(p_vpi_delay del) override;
      free_object_fun_t free_object_fun(व्योम) override;

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

बाह्य काष्ठा __vpiInterModPath* vpip_make_intermodpath(vvp_net_t *net, vpiPortInfo* port1, vpiPortInfo* port2);

/*
 * These methods support the vpi creation of events. The name string
 * passed in will be saved, so the caller must allocate it (or not
 * free it) after it is handed to this function.
 */
class __vpiNamedEvent : खुला __vpiHandle {

    खुला:
      __vpiNamedEvent(__vpiScope*scope, स्थिर अक्षर*name);
      ~__vpiNamedEvent() override;

      __vpiNamedEvent(स्थिर __vpiNamedEvent&) = delete;
      __vpiNamedEvent& चालक=(स्थिर __vpiNamedEvent&) = delete;

      पूर्णांक get_type_code(व्योम) स्थिर override;
      __vpiScope*get_scope(व्योम) स्थिर { वापस scope_; }
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
      vpiHandle vpi_handle(पूर्णांक code) override;

      अंतरभूत व्योम add_vpi_callback(__vpiCallback*cb)
      { cb->अग्र_2 = callbacks_;
	callbacks_ = cb;
      }

      व्योम run_vpi_callbacks(व्योम);

	/* The functor, used for %set operations. */
      vvp_net_t*funct;

    निजी:
	/* base name of the event object */
      स्थिर अक्षर*name_;
	/* Parent scope of this object. */
      __vpiScope*scope_;
	/* List of callbacks interested in this event. */
      __vpiCallback*callbacks_;
};

बाह्य vpiHandle vpip_make_named_event(स्थिर अक्षर*name, vvp_net_t*f);

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
काष्ठा __vpiRealVar : खुला __vpiHandle {
      __vpiRealVar();
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
      vpiHandle vpi_handle(पूर्णांक code) override;
      vpiHandle vpi_iterate(पूर्णांक code) override;

      जोड़ { // The scope or parent array that contains me.
	    vpiHandle parent;
	    __vpiScope* scope;
      } within;
	/* The name of this variable, or the index for array words. */
      जोड़ {
            स्थिर अक्षर*name;
            vpiHandle सूचकांक;
      } id;
      अचिन्हित is_netarray  : 1; // This is word of a net array
      अचिन्हित is_wire      : 1; // This is a wire, not a variable
      vvp_net_t*net;
};

बाह्य __vpiScope* vpip_scope(__vpiRealVar*sig);
बाह्य vpiHandle vpip_make_real_var(स्थिर अक्षर*name, vvp_net_t*net);
बाह्य vpiHandle vpip_make_real_net(__vpiScope*scope,
				    स्थिर अक्षर*name, vvp_net_t*net);

class __vpiBaseVar : खुला __vpiHandle {

    खुला:
      __vpiBaseVar(__vpiScope*scope, स्थिर अक्षर*name, vvp_net_t*net);
#अगर_घोषित CHECK_WITH_VALGRIND
      ~__vpiBaseVar() override;
#पूर्ण_अगर

      अंतरभूत vvp_net_t* get_net() स्थिर { वापस net_; }

    रक्षित:
      __vpiScope* scope_;
      स्थिर अक्षर*name_;

    निजी:
      vvp_net_t*net_;
};

class __vpiStringVar : खुला __vpiBaseVar {

    खुला:
      __vpiStringVar(__vpiScope*scope, स्थिर अक्षर*name, vvp_net_t*net);

      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
};

बाह्य vpiHandle vpip_make_string_var(स्थिर अक्षर*name, vvp_net_t*net);

काष्ठा __vpiArrayBase {
      __vpiArrayBase() : vals_शब्दगण(शून्य) {}
      भव ~__vpiArrayBase() {}

      भव अचिन्हित get_आकार(व्योम) स्थिर = 0;
      भव vpiHandle get_बायाँ_range() = 0;
      भव vpiHandle get_दायाँ_range() = 0;
      भव __vpiScope*get_scope() स्थिर = 0;

      भव पूर्णांक get_word_आकार() स्थिर = 0;
      भव अक्षर*get_word_str(काष्ठा __vpiArrayWord*word, पूर्णांक code) = 0;
      भव व्योम get_word_मूल्य(काष्ठा __vpiArrayWord*word, p_vpi_मूल्य vp) = 0;
      भव व्योम put_word_मूल्य(काष्ठा __vpiArrayWord*word, p_vpi_मूल्य vp,
                                    पूर्णांक flags) = 0;
      भव vpiHandle get_iter_सूचकांक(काष्ठा __vpiArrayIterator*iter, पूर्णांक idx) = 0;

    // vpi_iterate is already defined by vpiHandle, so to avoid problems with
    // classes inheriting from vpiHandle and vpiArrayBase just share the common
    // code in the following function
      vpiHandle vpi_array_base_iterate(पूर्णांक code);

      भव व्योम make_vals_शब्दगण();

      काष्ठा __vpiArrayWord*vals_शब्दगण;
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
काष्ठा __vpiArray : खुला __vpiArrayBase, खुला __vpiHandle {
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiMemory; }
      अचिन्हित get_आकार() स्थिर override { वापस array_गिनती; }
      vpiHandle get_बायाँ_range() override { निश्चित(nets == 0); वापस &msb; }
      vpiHandle get_दायाँ_range() override { निश्चित(nets == 0); वापस &lsb; }
      __vpiScope*get_scope() स्थिर override { वापस scope; }

      पूर्णांक get_word_आकार() स्थिर override;
      अक्षर*get_word_str(काष्ठा __vpiArrayWord*word, पूर्णांक code) override;
      व्योम get_word_मूल्य(काष्ठा __vpiArrayWord*word, p_vpi_मूल्य vp) override;
      व्योम put_word_मूल्य(काष्ठा __vpiArrayWord*word, p_vpi_मूल्य vp, पूर्णांक flags) override;

      vpiHandle get_iter_सूचकांक(काष्ठा __vpiArrayIterator*iter, पूर्णांक idx) override;

      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;
      vpiHandle vpi_handle(पूर्णांक code) override;
      अंतरभूत vpiHandle vpi_iterate(पूर्णांक code) override { वापस vpi_array_base_iterate(code); }
      vpiHandle vpi_सूचकांक(पूर्णांक idx) override;

      व्योम set_word(अचिन्हित idx, अचिन्हित off, स्थिर vvp_vector4_t&val);
      व्योम set_word(अचिन्हित idx, द्विगुन val);
      व्योम set_word(अचिन्हित idx, स्थिर std::string&val);
      व्योम set_word(अचिन्हित idx, स्थिर vvp_object_t&val);

      vvp_vector4_t get_word(अचिन्हित address);
      द्विगुन get_word_r(अचिन्हित address);
      व्योम get_word_obj(अचिन्हित address, vvp_object_t&val);
      std::string get_word_str(अचिन्हित address);

      व्योम alias_word(अचिन्हित दीर्घ addr, vpiHandle word, पूर्णांक msb, पूर्णांक lsb);
      व्योम attach_word(अचिन्हित addr, vpiHandle word);
      व्योम word_change(अचिन्हित दीर्घ addr);

      स्थिर अक्षर*name; /* Permanently allocated string */
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
      काष्ठा __vpiCallback *vpi_callbacks;
      bool signed_flag;
      bool swap_addr;

निजी:
      अचिन्हित array_गिनती;
      __vpiScope*scope;

मित्र vpiHandle vpip_make_array(स्थिर अक्षर*label, स्थिर अक्षर*name,
                                 पूर्णांक first_addr, पूर्णांक last_addr,
                                 bool signed_flag);
मित्र व्योम compile_array_alias(अक्षर*label, अक्षर*name, अक्षर*src);
};

class __vpiDarrayVar : खुला __vpiBaseVar, खुला __vpiArrayBase {
    खुला:
      __vpiDarrayVar(__vpiScope*scope, स्थिर अक्षर*name, vvp_net_t*net);

      पूर्णांक get_type_code() स्थिर override { वापस vpiArrayVar; }
      अचिन्हित get_आकार() स्थिर override;
      vpiHandle get_बायाँ_range() override;
      vpiHandle get_दायाँ_range() override;
      __vpiScope*get_scope() स्थिर override { वापस scope_; }

      पूर्णांक get_word_आकार() स्थिर override;
      अक्षर*get_word_str(काष्ठा __vpiArrayWord*word, पूर्णांक code) override;
      व्योम get_word_मूल्य(काष्ठा __vpiArrayWord*word, p_vpi_मूल्य vp) override;
      व्योम put_word_मूल्य(काष्ठा __vpiArrayWord*word, p_vpi_मूल्य vp, पूर्णांक flags) override;

      vpiHandle get_iter_सूचकांक(काष्ठा __vpiArrayIterator*iter, पूर्णांक idx) override;
      अंतरभूत vpiHandle vpi_iterate(पूर्णांक code) override { वापस vpi_array_base_iterate(code); }

      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;
      vpiHandle vpi_handle(पूर्णांक code) override;
      vpiHandle vpi_सूचकांक(पूर्णांक सूचकांक) override;

      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;

    रक्षित:
      vvp_darray*get_vvp_darray() स्थिर;
      __vpiDecConst बायाँ_range, दायाँ_range;
};

बाह्य vpiHandle vpip_make_darray_var(स्थिर अक्षर*name, vvp_net_t*net);

class __vpiQueueVar : खुला __vpiDarrayVar {

    खुला:
      __vpiQueueVar(__vpiScope*scope, स्थिर अक्षर*name, vvp_net_t*net);

      पूर्णांक vpi_get(पूर्णांक code) override;
};

बाह्य vpiHandle vpip_make_queue_var(स्थिर अक्षर*name, vvp_net_t*net);

class __vpiPropQueueRef : खुला __vpiHandle {

    खुला:
      explicit __vpiPropQueueRef(__vpiScope*scope, अचिन्हित pidx, bool is_queue);

      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;

      vvp_net_t* class_net_;
      अचिन्हित prop_idx_;
      bool is_queue_;

    निजी:
      __vpiScope* scope_;
};

बाह्य vpiHandle vpip_make_prop_queue_ref(अक्षर* class_label,
                                          अचिन्हित prop_idx,
                                          अचिन्हित is_queue_flag);
बाह्य vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref);

class __vpiCobjectVar : खुला __vpiBaseVar {

    खुला:
      __vpiCobjectVar(__vpiScope*scope, स्थिर अक्षर*name, vvp_net_t*net);

      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
};

बाह्य vpiHandle vpip_make_cobject_var(स्थिर अक्षर*name, vvp_net_t*net);

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
काष्ठा __vpiUserSystf : खुला __vpiHandle {
      __vpiUserSystf();
      पूर्णांक get_type_code(व्योम) स्थिर override;

      s_vpi_systf_तथ्य info;
      bool is_user_defn;
};

बाह्य vpiHandle vpip_make_systf_iterator(व्योम);

बाह्य काष्ठा __vpiUserSystf* vpip_find_systf(स्थिर अक्षर*name);


काष्ठा __vpiSysTaskCall : खुला __vpiHandle {

      __vpiScope* scope;
      काष्ठा __vpiUserSystf*defn;
      अचिन्हित nargs;
      vpiHandle*args;
	/* Stack consumed by this call */
      अचिन्हित vec4_ढेर;
      अचिन्हित real_ढेर;
      अचिन्हित string_ढेर;
	/* Support for vpi_get_userdata. */
      व्योम*userdata;
	// This is set if this is a structural call to a function
      class    vvp_net_t*fnet;
      अचिन्हित file_idx;
      अचिन्हित lineno;
      bool put_मूल्य;
    रक्षित:
      अंतरभूत __vpiSysTaskCall()
      {
	    vec4_ढेर = 0;
	    real_ढेर = 0;
	    string_ढेर = 0;
      }
};

बाह्य काष्ठा __vpiSysTaskCall*vpip_cur_task;

/*
 * The persistent flag to vpip_make_string_const causes the created
 * handle to be persistent. This is necessary for cases where the
 * string handle may be reused, which is the normal case.
 *
 * When constructing with a string, the class takes possession of the
 * text value string, and will delete it in the constructor.
 */

vpiHandle vpip_make_string_const(अक्षर*text, bool persistent =true);
vpiHandle vpip_make_string_param(स्थिर अक्षर*name, अक्षर*मूल्य, bool local_flag,
                                 दीर्घ file_idx, दीर्घ lineno);

काष्ठा __vpiBinaryConst : खुला __vpiHandle {
      __vpiBinaryConst();
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;

      vvp_vector4_t bits;
	/* TRUE if this constant is signed. */
      अचिन्हित signed_flag :1;
	/* TRUE if this constant has an explicit size (i.e. 19'h0 vs. 'h0) */
      अचिन्हित sized_flag  :1;
};

vpiHandle vpip_make_द्विआधारी_const(अचिन्हित wid, स्थिर अक्षर*bits);
vpiHandle vpip_make_द्विआधारी_param(स्थिर अक्षर*name, स्थिर vvp_vector4_t&bits,
				 bool signed_flag, bool local_flag,
				 दीर्घ file_idx, दीर्घ lineno);

class __vpiRealConst : खुला __vpiHandle {
    खुला:
      explicit __vpiRealConst(द्विगुन);
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;

      द्विगुन मूल्य;
};

vpiHandle vpip_make_real_const(द्विगुन मूल्य);
vpiHandle vpip_make_real_param(स्थिर अक्षर*name, द्विगुन मूल्य, bool local_flag,
                               दीर्घ file_idx, दीर्घ lineno);

class __vpiNullConst : खुला __vpiHandle {
    खुला:
      explicit __vpiNullConst();
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
};

vpiHandle vpip_make_null_const();

/*
 *  This one looks like a constant, but really is a vector in the current
 *  thread.
 */

vpiHandle vpip_make_vthr_word(अचिन्हित base, स्थिर अक्षर*type);
vpiHandle vpip_make_vthr_str_ढेर(अचिन्हित depth);
vpiHandle vpip_make_vthr_vec4_ढेर(अचिन्हित depth, bool signed_flag, अचिन्हित wid);

vpiHandle vpip_make_vthr_A(अक्षर*label, अचिन्हित सूचकांक);
vpiHandle vpip_make_vthr_A(अक्षर*label, अक्षर*प्रतीक);
vpiHandle vpip_make_vthr_A(अक्षर*label, vpiHandle handle);
vpiHandle vpip_make_vthr_APV(अक्षर*label, अचिन्हित सूचकांक, अचिन्हित bit, अचिन्हित wid);

/*
 * This function is called before any compilation to load VPI
 * modules. This gives the modules a chance to announce their
 * contained functions before compilation commences. It is called only
 * once per module.
 */
बाह्य "C" व्योम vpip_load_module(स्थिर अक्षर*name);

बाह्य "C" व्योम vpip_clear_module_paths();
बाह्य "C" व्योम vpip_add_module_path(स्थिर अक्षर *path);
बाह्य "C" व्योम vpip_add_env_and_default_module_paths();

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
बाह्य vpiHandle vpip_build_vpi_call(स्थिर अक्षर*name,
				     पूर्णांक val_type, अचिन्हित return_width,
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

बाह्य व्योम vpip_execute_vpi_call(vthread_t thr, vpiHandle obj);


/*
 * These are functions used by the compiler to prepare for compilation
 * and to finish compilation in preparation for execution.
 */

vpiHandle vpip_sim_time(__vpiScope*scope, bool is_stime);
vpiHandle vpip_sim_realtime(__vpiScope*scope);

बाह्य पूर्णांक vpip_get_time_precision(व्योम);
बाह्य व्योम vpip_set_time_precision(पूर्णांक pres);

बाह्य __vpiScope*vpip_timescale_scope_from_handle(vpiHandle obj);

बाह्य पूर्णांक vpip_time_units_from_handle(vpiHandle obj);
बाह्य पूर्णांक vpip_time_precision_from_handle(vpiHandle obj);

बाह्य द्विगुन vpip_scaled_time_from_handle(vvp_time64_t समय, vpiHandle obj);

बाह्य व्योम vpip_time_to_timestruct(काष्ठा t_vpi_time*ts, vvp_time64_t ti);
बाह्य vvp_time64_t vpip_timestruct_to_time(स्थिर काष्ठा t_vpi_time*ts);

बाह्य द्विगुन vpip_time_to_scaled_real(vvp_time64_t ti, स्थिर __vpiScope*sc);
बाह्य vvp_time64_t vpip_scaled_real_to_time64(द्विगुन val, स्थिर __vpiScope*sc);

/*
 * These functions are used mostly as compile time to strings into
 * permallocated memory. The vpip_string function is the most general,
 * it allocates a fresh string no matter what. The vpip_name_string
 * allocates a string and keeps a pointer in the hash, and tries to
 * reuse it if it can. This us useful for handle names, which may be
 * reused in different scopes.
 */
बाह्य स्थिर अक्षर* vpip_string(स्थिर अक्षर*str);
बाह्य स्थिर अक्षर* vpip_name_string(स्थिर अक्षर*str);


/*
 * This function is used to make decimal string versions of various
 * vectors. The input format is a vvp_vector4_t, and the result is
 * written into buf, without overflowing nbuf.
 */
बाह्य अचिन्हित vpip_vec4_to_dec_str(स्थिर vvp_vector4_t&vec4,
				     अक्षर *buf, अचिन्हित पूर्णांक nbuf,
				     पूर्णांक signed_flag);


बाह्य व्योम vpip_vec4_to_hex_str(स्थिर vvp_vector4_t&bits, अक्षर*buf,
				 अचिन्हित nbuf);

बाह्य व्योम vpip_vec4_to_oct_str(स्थिर vvp_vector4_t&bits, अक्षर*buf,
				 अचिन्हित nbuf);

बाह्य व्योम vpip_bin_str_to_vec4(vvp_vector4_t&val, स्थिर अक्षर*buf);
बाह्य व्योम vpip_oct_str_to_vec4(vvp_vector4_t&val, स्थिर अक्षर*str);
बाह्य व्योम vpip_dec_str_to_vec4(vvp_vector4_t&val, स्थिर अक्षर*str);
बाह्य व्योम vpip_hex_str_to_vec4(vvp_vector4_t&val, स्थिर अक्षर*str);

बाह्य vvp_vector4_t vec4_from_vpi_मूल्य(s_vpi_मूल्य*vp, अचिन्हित wid);
बाह्य द्विगुन real_from_vpi_मूल्य(s_vpi_मूल्य*vp);

बाह्य व्योम vpip_vec4_get_मूल्य(स्थिर vvp_vector4_t&word_val, अचिन्हित width,
				bool signed_flag, s_vpi_मूल्य*vp);
बाह्य व्योम vpip_vec2_get_मूल्य(स्थिर vvp_vector2_t&word_val, अचिन्हित width,
				bool signed_flag, s_vpi_मूल्य*vp);
बाह्य व्योम vpip_real_get_मूल्य(द्विगुन real, s_vpi_मूल्य*vp);
बाह्य व्योम vpip_string_get_मूल्य(स्थिर std::string&val, s_vpi_मूल्य*vp);

/*
 * Function defined in vpi_signal.cc to manage vpi_get_* persistent
 * storage.
 */
क्रमागत vpi_rbuf_t {
      RBUF_VAL =0,
	/* Storage for *_get_value() */
      RBUF_STR,
	/* Storage for *_get_str() */
      RBUF_DEL
	/* Delete the storage for both buffers. */
};
बाह्य व्योम *need_परिणाम_buf(अचिन्हित cnt, vpi_rbuf_t type);
/* following two routines use need_result_buf(, RBUF_STR) */
बाह्य अक्षर *simple_set_rbuf_str(स्थिर अक्षर *s1);
बाह्य अक्षर *generic_get_str(पूर्णांक code, vpiHandle ref, स्थिर अक्षर *name, स्थिर अक्षर *सूचकांक);

/* A routine to find the enclosing module. */
बाह्य vpiHandle vpip_module(__vpiScope*scope);

बाह्य पूर्णांक vpip_delay_selection;

/* A flag to disable output to MCD bit 0. */
बाह्य bool vpip_mcd0_disable;

अंतरभूत uint64_t vlg_round_to_u64(द्विगुन rval)
{
      // Directly casting a negative double to an unsigned integer types is
      // undefined behavior and behaves differently on different architectures.
      // Cast to signed integer first to get the behavior we want.
      वापस static_cast<uint64_t>(static_cast<int64_t>(std::round(rval)));
}

#पूर्ण_अगर /* IVL_vpi_priv_H */
