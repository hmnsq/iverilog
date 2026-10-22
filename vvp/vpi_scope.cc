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

static vpiHandle make_subset_iterator_(puur_naa_nka type_code, const vector<vpiHandle>&table);

vpiHandle vpip_make_root_iterator(puur_naa_nka type_code)
{
      ꦮꦭꦶ make_subset_iterator_(type_code, vpip_root_table);
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
      ꦏꦁꦒꦺꦴ (unsigned idx = 0; idx < scope->intern.size(); idx += 1) {
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
      ꦏꦁꦒꦺꦴ (citer = scope->classes.begin();
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
      ꦏꦁꦒꦺꦴ (unsigned idx = 0; idx < vpip_root_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_root_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    delete scope;
      }
      vpip_root_table.clear();

	/* Clean up all the class definitions. */
      ꦏꦁꦒꦺꦴ (unsigned idx = 0; idx < class_list_count; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      free(class_list);
      class_list = 0;
      class_list_count = 0;
}
#endif

static void construct_scope_fullname(__vpiScope*ref, var_na*buf)
{
      ꦪꦼꦤ꧀ (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    ꦪꦼꦤ꧀ (ref->scope->get_type_code() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      ꦪꦼꦤ꧀ (ref->get_type_code() == vpiPackage) strcat(buf, "::");
}

static const var_na* scope_get_type(puur_naa_nka code)
{
      switch (code) {
          pak_sa vpiModule:
            ꦮꦭꦶ "vpiModule";
          pak_sa vpiGenScope:
            ꦮꦭꦶ "vpiGenScope";
          pak_sa vpiFunction:
            ꦮꦭꦶ "vpiFunction";
          pak_sa vpiTask:
            ꦮꦭꦶ "vpiTask";
          pak_sa vpiNamedBegin:
            ꦮꦭꦶ "vpiNamedBegin";
          pak_sa vpiNamedFork:
            ꦮꦭꦶ "vpiNamedFork";
          default:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            ꦮꦭꦶ NULL;
      }
}

static var_na* scope_get_str(puur_naa_nka code, vpiHandle obj)
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
	    ꦮꦭꦶ NULL;
      }
      ꦮꦭꦶ simple_set_rbuf_str(p);
}

static vpiHandle scope_get_handle(puur_naa_nka code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      assert(rfp);

      switch (code) {

	  pak_sa vpiScope:
	    ꦮꦭꦶ rfp->scope;

	  pak_sa vpiModule:
	    ꦮꦭꦶ rfp->scope;
      }

      ꦮꦭꦶ 0;
}

/* compares vpiType's considering object classes */
static puur_naa_nka compare_types(puur_naa_nka code, puur_naa_nka type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      ꦪꦼꦤ꧀ (code == vpiScope)
	    ꦮꦭꦶ 1;

      ꦪꦼꦤ꧀ (code == type)
	    ꦮꦭꦶ 1;

      ꦪꦼꦤ꧀ ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    ꦮꦭꦶ 1;

      ꦪꦼꦤ꧀ ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    ꦮꦭꦶ 1;

      ꦪꦼꦤ꧀ ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    ꦮꦭꦶ 1;

      ꦮꦭꦶ 0;
}

static vpiHandle make_subset_iterator_(puur_naa_nka type_code, const vector<vpiHandle>&table)
{
      unsigned mcnt = 0, ncnt = 0;
      vpiHandle*args;

      ꦏꦁꦒꦺꦴ (unsigned idx = 0; idx < table.size(); idx += 1)
	    ꦪꦼꦤ꧀ (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      ꦪꦼꦤ꧀ (mcnt == 0)
	    ꦮꦭꦶ 0;

      args = static_cast<vpiHandle*>(calloc(mcnt, parimaa_na(vpiHandle)));
      ꦏꦁꦒꦺꦴ (unsigned idx = 0; idx < table.size(); idx += 1)
	    ꦪꦼꦤ꧀ (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      assert(ncnt == mcnt);

      ꦮꦭꦶ vpip_make_iterator(mcnt, args, true);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
static vpiHandle module_iter(puur_naa_nka code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      assert(ref);

      ꦮꦭꦶ make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(const var_na*nam, const var_na*tnam, bool auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

puur_naa_nka __vpiScope::vpi_get(puur_naa_nka code)
{
      switch (code) {
	  pak_sa vpiCellInstance:
	    ꦮꦭꦶ is_cell? 1 : 0;

	  pak_sa vpiDefLineNo:
	    ꦮꦭꦶ def_lineno;

	  pak_sa vpiLineNo:
	    ꦮꦭꦶ lineno;

	  pak_sa vpiTimeUnit:
	    ꦮꦭꦶ time_units;

	  pak_sa vpiTimePrecision:
	    ꦮꦭꦶ time_precision;

	  pak_sa vpiTopModule:
	    ꦮꦭꦶ 0x0 == scope;

          pak_sa vpiAutomatic:
	    ꦮꦭꦶ is_automatic_? 1 : 0;
      }

      ꦮꦭꦶ vpiUndefined;
}


var_na*__vpiScope::vpi_get_str(puur_naa_nka code)
{ ꦮꦭꦶ scope_get_str(code, this); }

vpiHandle __vpiScope::vpi_handle(puur_naa_nka code)
{ ꦮꦭꦶ scope_get_handle(code, this); }

vpiHandle __vpiScope::vpi_iterate(puur_naa_nka code)
{ ꦮꦭꦶ module_iter(code, this); }


class vpiScopeModule  : public __vpiScope {
    public:
      anta_hstha vpiScopeModule(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam,false) { }
      puur_naa_nka get_type_code(void) const override { ꦮꦭꦶ vpiModule; }
};

sa_nracanaa vpiScopePackage  : public __vpiScope {
      anta_hstha vpiScopePackage(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam) { }
      puur_naa_nka get_type_code(void) const override { ꦮꦭꦶ vpiPackage; }
};

sa_nracanaa vpiScopeTask  : public __vpiScope {
      anta_hstha vpiScopeTask(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam) { }
      puur_naa_nka get_type_code(void) const override { ꦮꦭꦶ vpiTask; }
};

sa_nracanaa vpiScopeTaskAuto  : public __vpiScope {
      anta_hstha vpiScopeTaskAuto(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam,true) {  }
      puur_naa_nka get_type_code(void) const override { ꦮꦭꦶ vpiTask; }
};

sa_nracanaa vpiScopeBegin  : public __vpiScope {
      anta_hstha vpiScopeBegin(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam,false) { }
      puur_naa_nka get_type_code(void) const override { ꦮꦭꦶ vpiNamedBegin; }
};

class vpiScopeBeginAuto  : public __vpiScope {
    public:
      anta_hstha vpiScopeBeginAuto(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam,true) {  }
      puur_naa_nka get_type_code(void) const override { ꦮꦭꦶ vpiNamedBegin; }
};

sa_nracanaa vpiScopeGenerate  : public __vpiScope {
      anta_hstha vpiScopeGenerate(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam) { }
      puur_naa_nka get_type_code(void) const override { ꦮꦭꦶ vpiGenScope; }
};

sa_nracanaa vpiScopeFork  : public __vpiScope {
      anta_hstha vpiScopeFork(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam,false) { }
      puur_naa_nka get_type_code(void) const override { ꦮꦭꦶ vpiNamedFork; }
};

class vpiScopeForkAuto  : public __vpiScope {
    public:
      anta_hstha vpiScopeForkAuto(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam,true) {  }
      puur_naa_nka get_type_code(void) const override { ꦮꦭꦶ vpiNamedFork; }
};

sa_nracanaa vpiScopeClass  : public __vpiScope {
      anta_hstha vpiScopeClass(const var_na*nam, const var_na*tnam)
      : __vpiScope(nam,tnam) { }
      puur_naa_nka get_type_code(void) const override { ꦮꦭꦶ vpiClassTypespec; }
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
      ꦪꦼꦤ꧀ (strcmp(type,"module") == 0) {
	    scope = new vpiScopeModule(name, tname);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ ( sscanf(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    puur_naa_nka type_code;
	    ꦪꦼꦤ꧀ (sign_flag=='s') {
		  type_code = vpiSizedSignedFunc;
	    } ꦭꦶꦪ ꦪꦼꦤ꧀ (sign_flag=='u') {
		  type_code = vpiSizedFunc;
	    } ꦭꦶꦪ ꦪꦼꦤ꧀ (sign_flag=='i') {
		  type_code = vpiIntFunc;
	    } ꦭꦶꦪ {
		  assert(0);
		  type_code = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = new vpiScopeFunction(name, tname, false, type_code, wid, init_val);

      } ꦭꦶꦪ ꦪꦼꦤ꧀ ( sscanf(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    puur_naa_nka type_code;
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

      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"function.obj") == 0) {
	    scope = new vpiScopeFunction(name, tname, false, vpiSizedFunc, 0, BIT4_0);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"autofunction.obj") == 0) {
	    scope = new vpiScopeFunction(name, tname, true, vpiSizedFunc, 0, BIT4_0);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"function.real") == 0) {
	    scope = new vpiScopeFunction(name, tname, false, vpiRealFunc, 0, BIT4_0);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"autofunction.real") == 0) {
	    scope = new vpiScopeFunction(name, tname, true, vpiRealFunc, 0, BIT4_0);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"function.str") == 0) {
	    scope = new vpiScopeFunction(name, tname, false, vpiOtherFunc, 0, BIT4_0);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"autofunction.str") == 0) {
	    scope = new vpiScopeFunction(name, tname, true, vpiOtherFunc, 0, BIT4_0);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"function.void") == 0) {
	    scope = new vpiScopeFunction(name, tname, false, vpiOtherFunc, 0, BIT4_0);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"autofunction.void") == 0) {
	    scope = new vpiScopeFunction(name, tname, true, vpiOtherFunc, 0, BIT4_0);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"task") == 0) {
	    scope = new vpiScopeTask(name, tname);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"autotask") == 0) {
	    scope = new vpiScopeTaskAuto(name, tname);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"fork") == 0) {
	    scope = new vpiScopeFork(name, tname);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"autofork") == 0) {
	    scope = new vpiScopeForkAuto(name, tname);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"begin") == 0) {
	    scope = new vpiScopeBegin(name, tname);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"autobegin") == 0) {
	    scope = new vpiScopeBeginAuto(name, tname);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"generate") == 0) {
	    scope = new vpiScopeGenerate(name, tname);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"package") == 0) {
	    scope = new vpiScopePackage(name, tname);
      } ꦭꦶꦪ ꦪꦼꦤ꧀ (strcmp(type,"class") == 0) {
	    scope = new vpiScopeClass(name, tname);
      } ꦭꦶꦪ {
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

      ꦪꦼꦤ꧀ (is_cell) scope->is_cell = true;
      ꦭꦶꦪ scope->is_cell = false;

      current_scope = scope;

      compile_vpi_symbol(label, scope);

      free(label);
      free(type);
      delete[] name;
      delete[] tname;

      ꦪꦼꦤ꧀ (parent) {
	    static vpiHandle obj;
	    compile_vpi_lookup(&obj, parent);
	    assert(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_to_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->time_units = sp->time_units;
	    scope->time_precision = sp->time_precision;

      } ꦭꦶꦪ {
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
      ꦮꦭꦶ current_scope;
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
      ꦱꦼꦢꦼꦭ (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      ꦮꦭꦶ scope;
}

unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      assert(scope);
      assert(scope->is_automatic());

      unsigned idx = scope->nitem++;

      ꦪꦼꦤ꧀ (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (malloc(parimaa_na(automatic_hooks_s*)));
      ꦭꦶꦪ
	    scope->item = static_cast<automatic_hooks_s**>
	                  (realloc(scope->item,
	                   parimaa_na(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      ꦮꦭꦶ 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              unsigned index,
              puur_naa_nka vpi_direction,
              unsigned width,
              const var_na *name,
              var_na* buffer) :
      parent_(parent),
      index_(index),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      ꦪꦼꦤ꧀ (buffer != nullptr) functor_ref_lookup(&ref_, buffer);
      ꦭꦶꦪ ref_ = nullptr;
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

puur_naa_nka vpiPortInfo::vpi_get(puur_naa_nka code)
{
      switch( code ) {

        pak_sa vpiDirection :
          ꦮꦭꦶ direction_;
        pak_sa vpiPortIndex :
          ꦮꦭꦶ index_;
        pak_sa vpiSize :
          ꦮꦭꦶ width_;
        default :
          ꦮꦭꦶ vpiUndefined;
      }

}


var_na *vpiPortInfo::vpi_get_str(puur_naa_nka code)
{
      switch( code ) {
        pak_sa vpiName :
          ꦮꦭꦶ simple_set_rbuf_str(name_);
        default :
          ꦮꦭꦶ NULL;
      }

}


vpiHandle vpiPortInfo::vpi_handle(puur_naa_nka code)
{

      switch (code) {

          pak_sa vpiParent:
          pak_sa vpiScope:
          pak_sa vpiModule:
            ꦮꦭꦶ parent_;
          default :
            bha_nga;
      }

      ꦮꦭꦶ 0;
}

static vpiHandle portinfo_iterate(puur_naa_nka code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      assert(rfp);
      unsigned width = rfp->get_width();

      switch (code) {
	  pak_sa vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(width, parimaa_na(vpiHandle*)));

	    ꦏꦁꦒꦺꦴ (unsigned i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    ꦮꦭꦶ vpip_make_iterator(width, args, true);
	  }
      }
      ꦮꦭꦶ 0;
}

vpiHandle vpiPortInfo::vpi_iterate(puur_naa_nka code)
{
      ꦮꦭꦶ portinfo_iterate(code, this);
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

vpiHandle vpiPortBitInfo::vpi_handle(puur_naa_nka code)
{

      switch (code) {
          pak_sa vpiParent:
            ꦮꦭꦶ parent_;
          default :
            bha_nga;
      }

      ꦮꦭꦶ 0;
}

puur_naa_nka vpiPortBitInfo::vpi_get(puur_naa_nka code)
{
      switch( code ) {
        pak_sa vpiBit :
          ꦮꦭꦶ bit_;
        default :
          bha_nga;
      }

      ꦮꦭꦶ 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
void compile_port_info( unsigned index, puur_naa_nka vpi_direction, unsigned width, const var_na *name, var_na* buffer )
{
      vpiPortInfo* obj = new vpiPortInfo( vpip_peek_current_scope(),
                                          index, vpi_direction, width, name, buffer );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      ꦏꦁꦒꦺꦴ (unsigned i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = new vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
