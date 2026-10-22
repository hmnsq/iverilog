/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
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

# include  "compile.h"
# include  "vpi_priv.h"
# include  "symbols.h"
# include  "statistics.h"
# include  "config.h"
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <vector>
# include  <cstring>
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"

using namespace std;

static vector<vpiHandle> vpip_root_table;

static vpiHandle make_subset_iterator_(цео type_code, const vector<vpiHandle>&table);

vpiHandle vpip_make_root_iterator(цео type_code)
{
      врати make_subset_iterator_(type_code, vpip_root_table);
}

void vpip_make_root_iterator(__vpiHandle**&table, unsigned&ntable)
{
      table = &vpip_root_table[0];
      ntable = vpip_root_table.size();
}

#ifdef CHECK_WITH_VALGRIND
void port_delete(__vpiHandle*handle);
void port_bit_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
static class_type **class_list = 0;
static unsigned class_list_count = 0;

static void delete_sub_scopes(__vpiScope *scope)
{
      за (unsigned idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle item = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(item);
	    switch(item->get_type_code()) {
		case vpiFunction:
		case vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		case vpiModule:
		case vpiGenScope:
		case vpiNamedBegin:
		case vpiNamedFork:
		case vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  delete item;
		  break;
		case vpiMemory:
		case vpiNetArray:
		  memory_delete(item);
		  break;
		case vpiModPath:
		    /* The destination ModPath is cleaned up later. */
		  delete item;
		  break;
		case vpiInterModPath:
		  delete item;
		  break;
		case vpiNamedEvent:
		  named_event_delete(item);
		  break;
		case vpiNet:
		case vpiReg:
		case vpiIntegerVar:
		case vpiLongIntVar:
		case vpiShortIntVar:
		case vpiIntVar:
		case vpiByteVar:
		case vpiBitVar:
		  signal_delete(item);
		  break;
		case vpiParameter:
		  parameter_delete(item);
		  break;
		case vpiRealVar:
		  real_delete(item);
		  break;
		case vpiEnumTypespec:
		  enum_delete(item);
		  break;
		case vpiPort:
		  port_delete(item);
		  break;
		case vpiPortBit:
		  port_bit_delete(item);
		  break;
		case vpiStringVar:
		  string_delete(item);
		  break;
		case vpiClassVar:
		  class_delete(item);
		  break;
		case vpiArrayVar:
		  array_delete(item);
		  break;
		default:
		  fprintf(stderr, "Need support for type: %d\n",
		          item->get_type_code());
		  assert(0);
		  break;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      за (citer = scope->classes.begin();
           citer != scope->classes.end(); ++ citer ) {
	    class_list_count += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_count*sizeof(class_type **)));
	    class_list[class_list_count-1] = citer->second;
      }
}

void root_table_delete(void)
{
      за (unsigned idx = 0; idx < vpip_root_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_root_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    delete scope;
      }
      vpip_root_table.clear();

	/* Clean up all the class definitions. */
      за (unsigned idx = 0; idx < class_list_count; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      free(class_list);
      class_list = 0;
      class_list_count = 0;
}
#endif

static void construct_scope_fullname(__vpiScope*ref, char*buf)
{
      ако (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    ако (ref->scope->get_type_code() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      ако (ref->get_type_code() == vpiPackage) strcat(buf, "::");
}

static const char* scope_get_type(цео code)
{
      switch (code) {
          case vpiModule:
            врати "vpiModule";
          case vpiGenScope:
            врати "vpiGenScope";
          case vpiFunction:
            врати "vpiFunction";
          case vpiTask:
            врати "vpiTask";
          case vpiNamedBegin:
            врати "vpiNamedBegin";
          case vpiNamedFork:
            врати "vpiNamedFork";
          default:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            врати NULL;
      }
}

static char* scope_get_str(цео code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      assert(ref);

      char buf[4096];  // XXX is a fixed buffer size really reliable?
      const char *p=0;
      switch (code) {
	  case vpiDefFile:
	    p = file_names[ref->def_file_idx];
	    break;

	  case vpiFile:
	    p = file_names[ref->file_idx];
	    break;

	  case vpiFullName:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    break;

	  case vpiName:
	    p = ref->scope_name();
	    break;

	  case vpiDefName:
	    p = ref->scope_def_name();
	    break;

	  case vpiType:
	    p = scope_get_type(code);
	    break;

	  default:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    врати NULL;
      }
      врати simple_set_rbuf_str(p);
}

static vpiHandle scope_get_handle(цео code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      assert(rfp);

      switch (code) {

	  case vpiScope:
	    врати rfp->scope;

	  case vpiModule:
	    врати rfp->scope;
      }

      врати 0;
}

/* compares vpiType's considering object classes */
static цео compare_types(цео code, цео type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      ако (code == vpiScope)
	    врати 1;

      ако (code == type)
	    врати 1;

      ако ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    врати 1;

      ако ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    врати 1;

      ако ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    врати 1;

      врати 0;
}

static vpiHandle make_subset_iterator_(цео type_code, const vector<vpiHandle>&table)
{
      unsigned mcnt = 0, ncnt = 0;
      vpiHandle*args;

      за (unsigned idx = 0; idx < table.size(); idx += 1)
	    ако (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      ако (mcnt == 0)
	    врати 0;

      args = static_cast<vpiHandle*>(calloc(mcnt, sizeof(vpiHandle)));
      за (unsigned idx = 0; idx < table.size(); idx += 1)
	    ако (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      assert(ncnt == mcnt);

      врати vpip_make_iterator(mcnt, args, true);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
static vpiHandle module_iter(цео code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      assert(ref);

      врати make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(const char*nam, const char*tnam, bool auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

цео __vpiScope::vpi_get(цео code)
{
      switch (code) {
	  case vpiCellInstance:
	    врати is_cell? 1 : 0;

	  case vpiDefLineNo:
	    врати def_lineno;

	  case vpiLineNo:
	    врати lineno;

	  case vpiTimeUnit:
	    врати time_units;

	  case vpiTimePrecision:
	    врати time_precision;

	  case vpiTopModule:
	    врати 0x0 == scope;

          case vpiAutomatic:
	    врати is_automatic_? 1 : 0;
      }

      врати vpiUndefined;
}


char*__vpiScope::vpi_get_str(цео code)
{ врати scope_get_str(code, this); }

vpiHandle __vpiScope::vpi_handle(цео code)
{ врати scope_get_handle(code, this); }

vpiHandle __vpiScope::vpi_iterate(цео code)
{ врати module_iter(code, this); }


class vpiScopeModule  : public __vpiScope {
    public:
      inline vpiScopeModule(const char*nam, const char*tnam)
      : __vpiScope(nam,tnam,false) { }
      цео get_type_code(void) const override { врати vpiModule; }
};

struct vpiScopePackage  : public __vpiScope {
      inline vpiScopePackage(const char*nam, const char*tnam)
      : __vpiScope(nam,tnam) { }
      цео get_type_code(void) const override { врати vpiPackage; }
};

struct vpiScopeTask  : public __vpiScope {
      inline vpiScopeTask(const char*nam, const char*tnam)
      : __vpiScope(nam,tnam) { }
      цео get_type_code(void) const override { врати vpiTask; }
};

struct vpiScopeTaskAuto  : public __vpiScope {
      inline vpiScopeTaskAuto(const char*nam, const char*tnam)
      : __vpiScope(nam,tnam,true) {  }
      цео get_type_code(void) const override { врати vpiTask; }
};

struct vpiScopeBegin  : public __vpiScope {
      inline vpiScopeBegin(const char*nam, const char*tnam)
      : __vpiScope(nam,tnam,false) { }
      цео get_type_code(void) const override { врати vpiNamedBegin; }
};

class vpiScopeBeginAuto  : public __vpiScope {
    public:
      inline vpiScopeBeginAuto(const char*nam, const char*tnam)
      : __vpiScope(nam,tnam,true) {  }
      цео get_type_code(void) const override { врати vpiNamedBegin; }
};

struct vpiScopeGenerate  : public __vpiScope {
      inline vpiScopeGenerate(const char*nam, const char*tnam)
      : __vpiScope(nam,tnam) { }
      цео get_type_code(void) const override { врати vpiGenScope; }
};

struct vpiScopeFork  : public __vpiScope {
      inline vpiScopeFork(const char*nam, const char*tnam)
      : __vpiScope(nam,tnam,false) { }
      цео get_type_code(void) const override { врати vpiNamedFork; }
};

class vpiScopeForkAuto  : public __vpiScope {
    public:
      inline vpiScopeForkAuto(const char*nam, const char*tnam)
      : __vpiScope(nam,tnam,true) {  }
      цео get_type_code(void) const override { врати vpiNamedFork; }
};

struct vpiScopeClass  : public __vpiScope {
      inline vpiScopeClass(const char*nam, const char*tnam)
      : __vpiScope(nam,tnam) { }
      цео get_type_code(void) const override { врати vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
static __vpiScope*current_scope = 0;

void vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj)
{
      assert(scope);
      scope->intern.push_back(obj);
}

/*
 * When the compiler encounters a scope declaration, this function
 * creates and initializes a __vpiScope object with the requested name
 * and within the addressed parent. The label is used as a key in the
 * symbol table and the name is used to construct the actual object.
 */
void
compile_scope_decl(char*label, char*type, char*name, char*tname,
                   char*parent, long file_idx, long lineno,
                   long def_file_idx, long def_lineno, long is_cell)
{
      count_vpi_scopes += 1;
      char vec_type;
      char sign_flag;
      unsigned wid;

      __vpiScope*scope;
      ако (strcmp(type,"module") == 0) {
	    scope = new vpiScopeModule(name, tname);
      } инако ако ( sscanf(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    цео type_code;
	    ако (sign_flag=='s') {
		  type_code = vpiSizedSignedFunc;
	    } инако ако (sign_flag=='u') {
		  type_code = vpiSizedFunc;
	    } инако ако (sign_flag=='i') {
		  type_code = vpiIntFunc;
	    } инако {
		  assert(0);
		  type_code = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = new vpiScopeFunction(name, tname, false, type_code, wid, init_val);

      } инако ако ( sscanf(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    цео type_code;
	    switch (sign_flag) {
		case 's':
		  type_code = vpiSizedSignedFunc;
		  break;
		case 'u':
		  type_code = vpiSizedFunc;
		  break;
		default:
		  assert(0);
		  type_code = vpiSizedFunc;
		  break;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = new vpiScopeFunction(name, tname, true, type_code, wid, init_val);

      } инако ако (strcmp(type,"function.obj") == 0) {
	    scope = new vpiScopeFunction(name, tname, false, vpiSizedFunc, 0, BIT4_0);
      } инако ако (strcmp(type,"autofunction.obj") == 0) {
	    scope = new vpiScopeFunction(name, tname, true, vpiSizedFunc, 0, BIT4_0);
      } инако ако (strcmp(type,"function.real") == 0) {
	    scope = new vpiScopeFunction(name, tname, false, vpiRealFunc, 0, BIT4_0);
      } инако ако (strcmp(type,"autofunction.real") == 0) {
	    scope = new vpiScopeFunction(name, tname, true, vpiRealFunc, 0, BIT4_0);
      } инако ако (strcmp(type,"function.str") == 0) {
	    scope = new vpiScopeFunction(name, tname, false, vpiOtherFunc, 0, BIT4_0);
      } инако ако (strcmp(type,"autofunction.str") == 0) {
	    scope = new vpiScopeFunction(name, tname, true, vpiOtherFunc, 0, BIT4_0);
      } инако ако (strcmp(type,"function.void") == 0) {
	    scope = new vpiScopeFunction(name, tname, false, vpiOtherFunc, 0, BIT4_0);
      } инако ако (strcmp(type,"autofunction.void") == 0) {
	    scope = new vpiScopeFunction(name, tname, true, vpiOtherFunc, 0, BIT4_0);
      } инако ако (strcmp(type,"task") == 0) {
	    scope = new vpiScopeTask(name, tname);
      } инако ако (strcmp(type,"autotask") == 0) {
	    scope = new vpiScopeTaskAuto(name, tname);
      } инако ако (strcmp(type,"fork") == 0) {
	    scope = new vpiScopeFork(name, tname);
      } инако ако (strcmp(type,"autofork") == 0) {
	    scope = new vpiScopeForkAuto(name, tname);
      } инако ако (strcmp(type,"begin") == 0) {
	    scope = new vpiScopeBegin(name, tname);
      } инако ако (strcmp(type,"autobegin") == 0) {
	    scope = new vpiScopeBeginAuto(name, tname);
      } инако ако (strcmp(type,"generate") == 0) {
	    scope = new vpiScopeGenerate(name, tname);
      } инако ако (strcmp(type,"package") == 0) {
	    scope = new vpiScopePackage(name, tname);
      } инако ако (strcmp(type,"class") == 0) {
	    scope = new vpiScopeClass(name, tname);
      } инако {
	    scope = new vpiScopeModule(name, tname);
	    assert(0);
      }

      scope->file_idx = (unsigned) file_idx;
      scope->lineno  = (unsigned) lineno;
      scope->def_file_idx = (unsigned) def_file_idx;
      scope->def_lineno  = (unsigned) def_lineno;
      scope->item = 0;
      scope->nitem = 0;
      scope->live_contexts = 0;
      scope->free_contexts = 0;

      ако (is_cell) scope->is_cell = true;
      инако scope->is_cell = false;

      current_scope = scope;

      compile_vpi_symbol(label, scope);

      free(label);
      free(type);
      delete[] name;
      delete[] tname;

      ако (parent) {
	    static vpiHandle obj;
	    compile_vpi_lookup(&obj, parent);
	    assert(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_to_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->time_units = sp->time_units;
	    scope->time_precision = sp->time_precision;

      } инако {
	    scope->scope = 0x0;

	    vpip_root_table.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->time_units = vpip_get_time_precision();
	    scope->time_precision = vpip_get_time_precision();
      }
}

void compile_scope_recall(char*symbol)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(void*)&current_scope, symbol);
      assert(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
void compile_timescale(long units, long precision)
{
      assert(current_scope);
      current_scope->time_units = units;
      current_scope->time_precision = precision;
}

__vpiScope* vpip_peek_current_scope(void)
{
      врати current_scope;
}

void vpip_attach_to_current_scope(vpiHandle obj)
{
      vpip_attach_to_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(void)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      док (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      врати scope;
}

unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      assert(scope);
      assert(scope->is_automatic());

      unsigned idx = scope->nitem++;

      ако (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (malloc(sizeof(automatic_hooks_s*)));
      инако
	    scope->item = static_cast<automatic_hooks_s**>
	                  (realloc(scope->item,
	                   sizeof(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      врати 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              unsigned index,
              цео vpi_direction,
              unsigned width,
              const char *name,
              char* buffer) :
      parent_(parent),
      index_(index),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      ако (buffer != nullptr) functor_ref_lookup(&ref_, buffer);
      инако ref_ = nullptr;
}

vpiPortInfo::~vpiPortInfo()
{
      delete[] name_;
}

#ifdef CHECK_WITH_VALGRIND
void port_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortInfo *>(handle);
}
#endif

цео vpiPortInfo::vpi_get(цео code)
{
      switch( code ) {

        case vpiDirection :
          врати direction_;
        case vpiPortIndex :
          врати index_;
        case vpiSize :
          врати width_;
        default :
          врати vpiUndefined;
      }

}


char *vpiPortInfo::vpi_get_str(цео code)
{
      switch( code ) {
        case vpiName :
          врати simple_set_rbuf_str(name_);
        default :
          врати NULL;
      }

}


vpiHandle vpiPortInfo::vpi_handle(цео code)
{

      switch (code) {

          case vpiParent:
          case vpiScope:
          case vpiModule:
            врати parent_;
          default :
            break;
      }

      врати 0;
}

static vpiHandle portinfo_iterate(цео code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      assert(rfp);
      unsigned width = rfp->get_width();

      switch (code) {
	  case vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(width, sizeof(vpiHandle*)));

	    за (unsigned i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    врати vpip_make_iterator(width, args, true);
	  }
      }
      врати 0;
}

vpiHandle vpiPortInfo::vpi_iterate(цео code)
{
      врати portinfo_iterate(code, this);
}


vpiPortBitInfo::vpiPortBitInfo(vpiPortInfo *parent,
                               unsigned bit) :
      parent_(parent),
      bit_(bit)
{
}

vpiPortBitInfo::~vpiPortBitInfo()
{
}

#ifdef CHECK_WITH_VALGRIND
void port_bit_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortBitInfo *>(handle);
}
#endif

vpiHandle vpiPortBitInfo::vpi_handle(цео code)
{

      switch (code) {
          case vpiParent:
            врати parent_;
          default :
            break;
      }

      врати 0;
}

цео vpiPortBitInfo::vpi_get(цео code)
{
      switch( code ) {
        case vpiBit :
          врати bit_;
        default :
          break;
      }

      врати 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
void compile_port_info( unsigned index, цео vpi_direction, unsigned width, const char *name, char* buffer )
{
      vpiPortInfo* obj = new vpiPortInfo( vpip_peek_current_scope(),
                                          index, vpi_direction, width, name, buffer );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      за (unsigned i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = new vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
