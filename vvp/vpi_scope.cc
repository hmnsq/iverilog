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

static vpiHandle make_subset_iterator_(ཧྲིལ type_code, const vector<vpiHandle>&table);

vpiHandle vpip_make_root_iterator(ཧྲིལ type_code)
{
      ལོག make_subset_iterator_(type_code, vpip_root_table);
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
      དོན་ལུ (unsigned idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle item = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(item);
	    switch(item->get_type_code()) {
		pak_sa vpiFunction:
		pak_sa vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		pak_sa vpiModule:
		pak_sa vpiGenScope:
		pak_sa vpiNamedBegin:
		pak_sa vpiNamedFork:
		pak_sa vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  delete item;
		  bha_nga;
		pak_sa vpiMemory:
		pak_sa vpiNetArray:
		  memory_delete(item);
		  bha_nga;
		pak_sa vpiModPath:
		    /* The destination ModPath is cleaned up later. */
		  delete item;
		  bha_nga;
		pak_sa vpiInterModPath:
		  delete item;
		  bha_nga;
		pak_sa vpiNamedEvent:
		  named_event_delete(item);
		  bha_nga;
		pak_sa vpiNet:
		pak_sa vpiReg:
		pak_sa vpiIntegerVar:
		pak_sa vpiLongIntVar:
		pak_sa vpiShortIntVar:
		pak_sa vpiIntVar:
		pak_sa vpiByteVar:
		pak_sa vpiBitVar:
		  signal_delete(item);
		  bha_nga;
		pak_sa vpiParameter:
		  parameter_delete(item);
		  bha_nga;
		pak_sa vpiRealVar:
		  real_delete(item);
		  bha_nga;
		pak_sa vpiEnumTypespec:
		  enum_delete(item);
		  bha_nga;
		pak_sa vpiPort:
		  port_delete(item);
		  bha_nga;
		pak_sa vpiPortBit:
		  port_bit_delete(item);
		  bha_nga;
		pak_sa vpiStringVar:
		  string_delete(item);
		  bha_nga;
		pak_sa vpiClassVar:
		  class_delete(item);
		  bha_nga;
		pak_sa vpiArrayVar:
		  array_delete(item);
		  bha_nga;
		default:
		  fprintf(stderr, "Need support for type: %d\n",
		          item->get_type_code());
		  assert(0);
		  bha_nga;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      དོན་ལུ (citer = scope->classes.begin();
           citer != scope->classes.end(); ++ citer ) {
	    class_list_count += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_count*parimaa_na(class_type **)));
	    class_list[class_list_count-1] = citer->second;
      }
}

void root_table_delete(void)
{
      དོན་ལུ (unsigned idx = 0; idx < vpip_root_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_root_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    delete scope;
      }
      vpip_root_table.clear();

	/* Clean up all the class definitions. */
      དོན་ལུ (unsigned idx = 0; idx < class_list_count; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      free(class_list);
      class_list = 0;
      class_list_count = 0;
}
#endif

static void construct_scope_fullname(__vpiScope*ref, var_na*buf)
{
      གལ་སྲིད (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    གལ་སྲིད (ref->scope->get_type_code() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      གལ་སྲིད (ref->get_type_code() == vpiPackage) strcat(buf, "::");
}

static const var_na* scope_get_type(ཧྲིལ code)
{
      switch (code) {
          pak_sa vpiModule:
            ལོག "vpiModule";
          pak_sa vpiGenScope:
            ལོག "vpiGenScope";
          pak_sa vpiFunction:
            ལོག "vpiFunction";
          pak_sa vpiTask:
            ལོག "vpiTask";
          pak_sa vpiNamedBegin:
            ལོག "vpiNamedBegin";
          pak_sa vpiNamedFork:
            ལོག "vpiNamedFork";
          default:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            ལོག NULL;
      }
}

static var_na* scope_get_str(ཧྲིལ code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      assert(ref);

      var_na buf[4096];  // XXX is a fixed buffer size really reliable?
      const var_na *p=0;
      switch (code) {
	  pak_sa vpiDefFile:
	    p = file_names[ref->def_file_idx];
	    bha_nga;

	  pak_sa vpiFile:
	    p = file_names[ref->file_idx];
	    bha_nga;

	  pak_sa vpiFullName:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    bha_nga;

	  pak_sa vpiName:
	    p = ref->scope_name();
	    bha_nga;

	  pak_sa vpiDefName:
	    p = ref->scope_def_name();
	    bha_nga;

	  pak_sa vpiType:
	    p = scope_get_type(code);
	    bha_nga;

	  default:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    ལོག NULL;
      }
      ལོག simple_set_rbuf_str(p);
}

static vpiHandle scope_get_handle(ཧྲིལ code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      assert(rfp);

      switch (code) {

	  pak_sa vpiScope:
	    ལོག rfp->scope;

	  pak_sa vpiModule:
	    ལོག rfp->scope;
      }

      ལོག 0;
}

/* compares vpiType's considering object classes */
static ཧྲིལ compare_types(ཧྲིལ code, ཧྲིལ type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      གལ་སྲིད (code == vpiScope)
	    ལོག 1;

      གལ་སྲིད (code == type)
	    ལོག 1;

      གལ་སྲིད ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    ལོག 1;

      གལ་སྲིད ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    ལོག 1;

      གལ་སྲིད ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    ལོག 1;

      ལོག 0;
}

static vpiHandle make_subset_iterator_(ཧྲིལ type_code, const vector<vpiHandle>&table)
{
      unsigned mcnt = 0, ncnt = 0;
      vpiHandle*args;

      དོན་ལུ (unsigned idx = 0; idx < table.size(); idx += 1)
	    གལ་སྲིད (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      གལ་སྲིད (mcnt == 0)
	    ལོག 0;

      args = static_cast<vpiHandle*>(calloc(mcnt, parimaa_na(vpiHandle)));
      དོན་ལུ (unsigned idx = 0; idx < table.size(); idx += 1)
	    གལ་སྲིད (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      assert(ncnt == mcnt);

      ལོག vpip_make_iterator(mcnt, args, true);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
static vpiHandle module_iter(ཧྲིལ code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      assert(ref);

      ལོག make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(const var_na*nam, const var_na*tnam, bool auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

ཧྲིལ __vpiScope::vpi_get(ཧྲིལ code)
{
      switch (code) {
	  pak_sa vpiCellInstance:
	    ལོག is_cell? 1 : 0;

	  pak_sa vpiDefLineNo:
	    ལོག def_lineno;

	  pak_sa vpiLineNo:
	    ལོག lineno;

	  pak_sa vpiTimeUnit:
	    ལོག time_units;

	  pak_sa vpiTimePrecision:
	    ལོག time_precision;

	  pak_sa vpiTopModule:
	    ལོག 0x0 == scope;

          pak_sa vpiAutomatic:
	    ལོག is_automatic_? 1 : 0;
      }

      ལོག vpiUndefined;
}


var_na*__vpiScope::vpi_get_str(ཧྲིལ code)
{ ལོག scope_get_str(code, this); }

vpiHandle __vpiScope::vpi_handle(ཧྲིལ code)
{ ལོག scope_get_handle(code, this); }

vpiHandle __vpiScope::vpi_iterate(ཧྲིལ code)
{ ལོག module_iter(code, this); }


class vpiScopeModule  : public __vpiScope {
    public:
      anta_hstha vpiScopeModule(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam,false) { }
      ཧྲིལ get_type_code(void) const override { ལོག vpiModule; }
};

sa_nracanaa vpiScopePackage  : public __vpiScope {
      anta_hstha vpiScopePackage(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam) { }
      ཧྲིལ get_type_code(void) const override { ལོག vpiPackage; }
};

sa_nracanaa vpiScopeTask  : public __vpiScope {
      anta_hstha vpiScopeTask(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam) { }
      ཧྲིལ get_type_code(void) const override { ལོག vpiTask; }
};

sa_nracanaa vpiScopeTaskAuto  : public __vpiScope {
      anta_hstha vpiScopeTaskAuto(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam,true) {  }
      ཧྲིལ get_type_code(void) const override { ལོག vpiTask; }
};

sa_nracanaa vpiScopeBegin  : public __vpiScope {
      anta_hstha vpiScopeBegin(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam,false) { }
      ཧྲིལ get_type_code(void) const override { ལོག vpiNamedBegin; }
};

class vpiScopeBeginAuto  : public __vpiScope {
    public:
      anta_hstha vpiScopeBeginAuto(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam,true) {  }
      ཧྲིལ get_type_code(void) const override { ལོག vpiNamedBegin; }
};

sa_nracanaa vpiScopeGenerate  : public __vpiScope {
      anta_hstha vpiScopeGenerate(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam) { }
      ཧྲིལ get_type_code(void) const override { ལོག vpiGenScope; }
};

sa_nracanaa vpiScopeFork  : public __vpiScope {
      anta_hstha vpiScopeFork(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam,false) { }
      ཧྲིལ get_type_code(void) const override { ལོག vpiNamedFork; }
};

class vpiScopeForkAuto  : public __vpiScope {
    public:
      anta_hstha vpiScopeForkAuto(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam,true) {  }
      ཧྲིལ get_type_code(void) const override { ལོག vpiNamedFork; }
};

sa_nracanaa vpiScopeClass  : public __vpiScope {
      anta_hstha vpiScopeClass(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam) { }
      ཧྲིལ get_type_code(void) const override { ལོག vpiClassTypespec; }
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
compile_scope_decl(var_na*label, var_na*type, var_na*name, var_na*tname,
                   var_na*parent, long file_idx, long lineno,
                   long def_file_idx, long def_lineno, long is_cell)
{
      count_vpi_scopes += 1;
      var_na vec_type;
      var_na sign_flag;
      unsigned wid;

      __vpiScope*scope;
      གལ་སྲིད (strcmp(type,"module") == 0) {
	    scope = new vpiScopeModule(name, tname);
      } དེ་མིན གལ་སྲིད ( sscanf(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    ཧྲིལ type_code;
	    གལ་སྲིད (sign_flag=='s') {
		  type_code = vpiSizedSignedFunc;
	    } དེ་མིན གལ་སྲིད (sign_flag=='u') {
		  type_code = vpiSizedFunc;
	    } དེ་མིན གལ་སྲིད (sign_flag=='i') {
		  type_code = vpiIntFunc;
	    } དེ་མིན {
		  assert(0);
		  type_code = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = new vpiScopeFunction(name, tname, false, type_code, wid, init_val);

      } དེ་མིན གལ་སྲིད ( sscanf(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    ཧྲིལ type_code;
	    switch (sign_flag) {
		pak_sa 's':
		  type_code = vpiSizedSignedFunc;
		  bha_nga;
		pak_sa 'u':
		  type_code = vpiSizedFunc;
		  bha_nga;
		default:
		  assert(0);
		  type_code = vpiSizedFunc;
		  bha_nga;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = new vpiScopeFunction(name, tname, true, type_code, wid, init_val);

      } དེ་མིན གལ་སྲིད (strcmp(type,"function.obj") == 0) {
	    scope = new vpiScopeFunction(name, tname, false, vpiSizedFunc, 0, BIT4_0);
      } དེ་མིན གལ་སྲིད (strcmp(type,"autofunction.obj") == 0) {
	    scope = new vpiScopeFunction(name, tname, true, vpiSizedFunc, 0, BIT4_0);
      } དེ་མིན གལ་སྲིད (strcmp(type,"function.real") == 0) {
	    scope = new vpiScopeFunction(name, tname, false, vpiRealFunc, 0, BIT4_0);
      } དེ་མིན གལ་སྲིད (strcmp(type,"autofunction.real") == 0) {
	    scope = new vpiScopeFunction(name, tname, true, vpiRealFunc, 0, BIT4_0);
      } དེ་མིན གལ་སྲིད (strcmp(type,"function.str") == 0) {
	    scope = new vpiScopeFunction(name, tname, false, vpiOtherFunc, 0, BIT4_0);
      } དེ་མིན གལ་སྲིད (strcmp(type,"autofunction.str") == 0) {
	    scope = new vpiScopeFunction(name, tname, true, vpiOtherFunc, 0, BIT4_0);
      } དེ་མིན གལ་སྲིད (strcmp(type,"function.void") == 0) {
	    scope = new vpiScopeFunction(name, tname, false, vpiOtherFunc, 0, BIT4_0);
      } དེ་མིན གལ་སྲིད (strcmp(type,"autofunction.void") == 0) {
	    scope = new vpiScopeFunction(name, tname, true, vpiOtherFunc, 0, BIT4_0);
      } དེ་མིན གལ་སྲིད (strcmp(type,"task") == 0) {
	    scope = new vpiScopeTask(name, tname);
      } དེ་མིན གལ་སྲིད (strcmp(type,"autotask") == 0) {
	    scope = new vpiScopeTaskAuto(name, tname);
      } དེ་མིན གལ་སྲིད (strcmp(type,"fork") == 0) {
	    scope = new vpiScopeFork(name, tname);
      } དེ་མིན གལ་སྲིད (strcmp(type,"autofork") == 0) {
	    scope = new vpiScopeForkAuto(name, tname);
      } དེ་མིན གལ་སྲིད (strcmp(type,"begin") == 0) {
	    scope = new vpiScopeBegin(name, tname);
      } དེ་མིན གལ་སྲིད (strcmp(type,"autobegin") == 0) {
	    scope = new vpiScopeBeginAuto(name, tname);
      } དེ་མིན གལ་སྲིད (strcmp(type,"generate") == 0) {
	    scope = new vpiScopeGenerate(name, tname);
      } དེ་མིན གལ་སྲིད (strcmp(type,"package") == 0) {
	    scope = new vpiScopePackage(name, tname);
      } དེ་མིན གལ་སྲིད (strcmp(type,"class") == 0) {
	    scope = new vpiScopeClass(name, tname);
      } དེ་མིན {
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

      གལ་སྲིད (is_cell) scope->is_cell = true;
      དེ་མིན scope->is_cell = false;

      current_scope = scope;

      compile_vpi_symbol(label, scope);

      free(label);
      free(type);
      delete[] name;
      delete[] tname;

      གལ་སྲིད (parent) {
	    static vpiHandle obj;
	    compile_vpi_lookup(&obj, parent);
	    assert(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_to_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->time_units = sp->time_units;
	    scope->time_precision = sp->time_precision;

      } དེ་མིན {
	    scope->scope = 0x0;

	    vpip_root_table.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->time_units = vpip_get_time_precision();
	    scope->time_precision = vpip_get_time_precision();
      }
}

void compile_scope_recall(var_na*symbol)
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
      ལོག current_scope;
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
      སྐབས (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      ལོག scope;
}

unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      assert(scope);
      assert(scope->is_automatic());

      unsigned idx = scope->nitem++;

      གལ་སྲིད (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (malloc(parimaa_na(automatic_hooks_s*)));
      དེ་མིན
	    scope->item = static_cast<automatic_hooks_s**>
	                  (realloc(scope->item,
	                   parimaa_na(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      ལོག 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              unsigned index,
              ཧྲིལ vpi_direction,
              unsigned width,
              const var_na *name,
              var_na* buffer) :
      parent_(parent),
      index_(index),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      གལ་སྲིད (buffer != nullptr) functor_ref_lookup(&ref_, buffer);
      དེ་མིན ref_ = nullptr;
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

ཧྲིལ vpiPortInfo::vpi_get(ཧྲིལ code)
{
      switch( code ) {

        pak_sa vpiDirection :
          ལོག direction_;
        pak_sa vpiPortIndex :
          ལོག index_;
        pak_sa vpiSize :
          ལོག width_;
        default :
          ལོག vpiUndefined;
      }

}


var_na *vpiPortInfo::vpi_get_str(ཧྲིལ code)
{
      switch( code ) {
        pak_sa vpiName :
          ལོག simple_set_rbuf_str(name_);
        default :
          ལོག NULL;
      }

}


vpiHandle vpiPortInfo::vpi_handle(ཧྲིལ code)
{

      switch (code) {

          pak_sa vpiParent:
          pak_sa vpiScope:
          pak_sa vpiModule:
            ལོག parent_;
          default :
            bha_nga;
      }

      ལོག 0;
}

static vpiHandle portinfo_iterate(ཧྲིལ code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      assert(rfp);
      unsigned width = rfp->get_width();

      switch (code) {
	  pak_sa vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(width, parimaa_na(vpiHandle*)));

	    དོན་ལུ (unsigned i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    ལོག vpip_make_iterator(width, args, true);
	  }
      }
      ལོག 0;
}

vpiHandle vpiPortInfo::vpi_iterate(ཧྲིལ code)
{
      ལོག portinfo_iterate(code, this);
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

vpiHandle vpiPortBitInfo::vpi_handle(ཧྲིལ code)
{

      switch (code) {
          pak_sa vpiParent:
            ལོག parent_;
          default :
            bha_nga;
      }

      ལོག 0;
}

ཧྲིལ vpiPortBitInfo::vpi_get(ཧྲིལ code)
{
      switch( code ) {
        pak_sa vpiBit :
          ལོག bit_;
        default :
          bha_nga;
      }

      ལོག 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
void compile_port_info( unsigned index, ཧྲིལ vpi_direction, unsigned width, const var_na *name, var_na* buffer )
{
      vpiPortInfo* obj = new vpiPortInfo( vpip_peek_current_scope(),
                                          index, vpi_direction, width, name, buffer );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      དོན་ལུ (unsigned i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = new vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
