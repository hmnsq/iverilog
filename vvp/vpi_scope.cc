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

using نطاق std;

ساكن vector<vpiHandle> vpip_root_table;

ساكن vpiHandle make_subset_iterator_(صحيح type_code, ثابت vector<vpiHandle>&table);

vpiHandle vpip_make_root_iterator(صحيح type_code)
{
      أرجع make_subset_iterator_(type_code, vpip_root_table);
}

فراغ vpip_make_root_iterator(__vpiHandle**&table, unsigned&ntable)
{
      table = &vpip_root_table[0];
      ntable = vpip_root_table.size();
}

#ifdef CHECK_WITH_VALGRIND
فراغ port_delete(__vpiHandle*handle);
فراغ port_bit_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
ساكن class_type **class_list = 0;
ساكن unsigned class_list_count = 0;

ساكن فراغ delete_sub_scopes(__vpiScope *scope)
{
      لكل (unsigned idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle item = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(item);
	    بدل(item->get_type_code()) {
		حالة vpiFunction:
		حالة vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		حالة vpiModule:
		حالة vpiGenScope:
		حالة vpiNamedBegin:
		حالة vpiNamedFork:
		حالة vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  delete item;
		  اكسر;
		حالة vpiMemory:
		حالة vpiNetArray:
		  memory_delete(item);
		  اكسر;
		حالة vpiModPath:
		    /* The destination ModPath is cleaned up later. */
		  delete item;
		  اكسر;
		حالة vpiInterModPath:
		  delete item;
		  اكسر;
		حالة vpiNamedEvent:
		  named_event_delete(item);
		  اكسر;
		حالة vpiNet:
		حالة vpiReg:
		حالة vpiIntegerVar:
		حالة vpiLongIntVar:
		حالة vpiShortIntVar:
		حالة vpiIntVar:
		حالة vpiByteVar:
		حالة vpiBitVar:
		  signal_delete(item);
		  اكسر;
		حالة vpiParameter:
		  parameter_delete(item);
		  اكسر;
		حالة vpiRealVar:
		  real_delete(item);
		  اكسر;
		حالة vpiEnumTypespec:
		  enum_delete(item);
		  اكسر;
		حالة vpiPort:
		  port_delete(item);
		  اكسر;
		حالة vpiPortBit:
		  port_bit_delete(item);
		  اكسر;
		حالة vpiStringVar:
		  string_delete(item);
		  اكسر;
		حالة vpiClassVar:
		  class_delete(item);
		  اكسر;
		حالة vpiArrayVar:
		  array_delete(item);
		  اكسر;
		افتراضي:
		  fprintf(stderr, "Need support for type: %d\n",
		          item->get_type_code());
		  أكد(0);
		  اكسر;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      لكل (citer = scope->classes.begin();
           citer != scope->classes.end(); ++ citer ) {
	    class_list_count += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_count*حجم(class_type **)));
	    class_list[class_list_count-1] = citer->second;
      }
}

فراغ root_table_delete(فراغ)
{
      لكل (unsigned idx = 0; idx < vpip_root_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_root_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    delete scope;
      }
      vpip_root_table.clear();

	/* Clean up all the class definitions. */
      لكل (unsigned idx = 0; idx < class_list_count; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      حرر(class_list);
      class_list = 0;
      class_list_count = 0;
}
#endif

ساكن فراغ construct_scope_fullname(__vpiScope*ref, حرف*buf)
{
      𐡄𐡍 (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    𐡄𐡍 (ref->scope->get_type_code() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      𐡄𐡍 (ref->get_type_code() == vpiPackage) strcat(buf, "::");
}

ساكن ثابت حرف* scope_get_type(صحيح code)
{
      بدل (code) {
          حالة vpiModule:
            أرجع "vpiModule";
          حالة vpiGenScope:
            أرجع "vpiGenScope";
          حالة vpiFunction:
            أرجع "vpiFunction";
          حالة vpiTask:
            أرجع "vpiTask";
          حالة vpiNamedBegin:
            أرجع "vpiNamedBegin";
          حالة vpiNamedFork:
            أرجع "vpiNamedFork";
          افتراضي:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            أرجع عدم;
      }
}

ساكن حرف* scope_get_str(صحيح code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      أكد(ref);

      حرف buf[4096];  // XXX is a fixed buffer size really reliable?
      ثابت حرف *p=0;
      بدل (code) {
	  حالة vpiDefFile:
	    p = file_names[ref->def_file_idx];
	    اكسر;

	  حالة vpiFile:
	    p = file_names[ref->file_idx];
	    اكسر;

	  حالة vpiFullName:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    اكسر;

	  حالة vpiName:
	    p = ref->scope_name();
	    اكسر;

	  حالة vpiDefName:
	    p = ref->scope_def_name();
	    اكسر;

	  حالة vpiType:
	    p = scope_get_type(code);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    أرجع عدم;
      }
      أرجع simple_set_rbuf_str(p);
}

ساكن vpiHandle scope_get_handle(صحيح code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      أكد(rfp);

      بدل (code) {

	  حالة vpiScope:
	    أرجع rfp->scope;

	  حالة vpiModule:
	    أرجع rfp->scope;
      }

      أرجع 0;
}

/* compares vpiType's considering object classes */
ساكن صحيح compare_types(صحيح code, صحيح type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      𐡄𐡍 (code == vpiScope)
	    أرجع 1;

      𐡄𐡍 (code == type)
	    أرجع 1;

      𐡄𐡍 ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    أرجع 1;

      𐡄𐡍 ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    أرجع 1;

      𐡄𐡍 ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    أرجع 1;

      أرجع 0;
}

ساكن vpiHandle make_subset_iterator_(صحيح type_code, ثابت vector<vpiHandle>&table)
{
      unsigned mcnt = 0, ncnt = 0;
      vpiHandle*args;

      لكل (unsigned idx = 0; idx < table.size(); idx += 1)
	    𐡄𐡍 (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      𐡄𐡍 (mcnt == 0)
	    أرجع 0;

      args = static_cast<vpiHandle*>(calloc(mcnt, حجم(vpiHandle)));
      لكل (unsigned idx = 0; idx < table.size(); idx += 1)
	    𐡄𐡍 (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      أكد(ncnt == mcnt);

      أرجع vpip_make_iterator(mcnt, args, 𐡒𐡔𐡈);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
ساكن vpiHandle module_iter(صحيح code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      أكد(ref);

      أرجع make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(ثابت حرف*nam, ثابت حرف*tnam, منطقي auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

صحيح __vpiScope::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiCellInstance:
	    أرجع is_cell? 1 : 0;

	  حالة vpiDefLineNo:
	    أرجع def_lineno;

	  حالة vpiLineNo:
	    أرجع lineno;

	  حالة vpiTimeUnit:
	    أرجع time_units;

	  حالة vpiTimePrecision:
	    أرجع time_precision;

	  حالة vpiTopModule:
	    أرجع 0x0 == scope;

          حالة vpiAutomatic:
	    أرجع is_automatic_? 1 : 0;
      }

      أرجع vpiUndefined;
}


حرف*__vpiScope::vpi_get_str(صحيح code)
{ أرجع scope_get_str(code, هذا); }

vpiHandle __vpiScope::vpi_handle(صحيح code)
{ أرجع scope_get_handle(code, هذا); }

vpiHandle __vpiScope::vpi_iterate(صحيح code)
{ أرجع module_iter(code, هذا); }


صنف vpiScopeModule  : عام __vpiScope {
    عام:
      مضمن vpiScopeModule(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,خطأ) { }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiModule; }
};

بنية vpiScopePackage  : عام __vpiScope {
      مضمن vpiScopePackage(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam) { }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiPackage; }
};

بنية vpiScopeTask  : عام __vpiScope {
      مضمن vpiScopeTask(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam) { }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiTask; }
};

بنية vpiScopeTaskAuto  : عام __vpiScope {
      مضمن vpiScopeTaskAuto(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,𐡒𐡔𐡈) {  }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiTask; }
};

بنية vpiScopeBegin  : عام __vpiScope {
      مضمن vpiScopeBegin(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,خطأ) { }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiNamedBegin; }
};

صنف vpiScopeBeginAuto  : عام __vpiScope {
    عام:
      مضمن vpiScopeBeginAuto(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,𐡒𐡔𐡈) {  }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiNamedBegin; }
};

بنية vpiScopeGenerate  : عام __vpiScope {
      مضمن vpiScopeGenerate(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam) { }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiGenScope; }
};

بنية vpiScopeFork  : عام __vpiScope {
      مضمن vpiScopeFork(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,خطأ) { }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiNamedFork; }
};

صنف vpiScopeForkAuto  : عام __vpiScope {
    عام:
      مضمن vpiScopeForkAuto(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,𐡒𐡔𐡈) {  }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiNamedFork; }
};

بنية vpiScopeClass  : عام __vpiScope {
      مضمن vpiScopeClass(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam) { }
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
ساكن __vpiScope*current_scope = 0;

فراغ vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj)
{
      أكد(scope);
      scope->intern.push_back(obj);
}

/*
 * When the compiler encounters a scope declaration, this function
 * creates and initializes a __vpiScope object with the requested name
 * and within the addressed parent. The label is used as a key in the
 * symbol table and the name is used to construct the actual object.
 */
فراغ
compile_scope_decl(حرف*label, حرف*type, حرف*name, حرف*tname,
                   حرف*parent, long file_idx, long lineno,
                   long def_file_idx, long def_lineno, long is_cell)
{
      count_vpi_scopes += 1;
      حرف vec_type;
      حرف sign_flag;
      unsigned wid;

      __vpiScope*scope;
      𐡄𐡍 (strcmp(type,"module") == 0) {
	    scope = جديد vpiScopeModule(name, tname);
      } وإلا 𐡄𐡍 ( sscanf(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    صحيح type_code;
	    𐡄𐡍 (sign_flag=='s') {
		  type_code = vpiSizedSignedFunc;
	    } وإلا 𐡄𐡍 (sign_flag=='u') {
		  type_code = vpiSizedFunc;
	    } وإلا 𐡄𐡍 (sign_flag=='i') {
		  type_code = vpiIntFunc;
	    } وإلا {
		  أكد(0);
		  type_code = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = جديد vpiScopeFunction(name, tname, خطأ, type_code, wid, init_val);

      } وإلا 𐡄𐡍 ( sscanf(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    صحيح type_code;
	    بدل (sign_flag) {
		حالة 's':
		  type_code = vpiSizedSignedFunc;
		  اكسر;
		حالة 'u':
		  type_code = vpiSizedFunc;
		  اكسر;
		افتراضي:
		  أكد(0);
		  type_code = vpiSizedFunc;
		  اكسر;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = جديد vpiScopeFunction(name, tname, 𐡒𐡔𐡈, type_code, wid, init_val);

      } وإلا 𐡄𐡍 (strcmp(type,"function.obj") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, خطأ, vpiSizedFunc, 0, BIT4_0);
      } وإلا 𐡄𐡍 (strcmp(type,"autofunction.obj") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, 𐡒𐡔𐡈, vpiSizedFunc, 0, BIT4_0);
      } وإلا 𐡄𐡍 (strcmp(type,"function.real") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, خطأ, vpiRealFunc, 0, BIT4_0);
      } وإلا 𐡄𐡍 (strcmp(type,"autofunction.real") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, 𐡒𐡔𐡈, vpiRealFunc, 0, BIT4_0);
      } وإلا 𐡄𐡍 (strcmp(type,"function.str") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, خطأ, vpiOtherFunc, 0, BIT4_0);
      } وإلا 𐡄𐡍 (strcmp(type,"autofunction.str") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, 𐡒𐡔𐡈, vpiOtherFunc, 0, BIT4_0);
      } وإلا 𐡄𐡍 (strcmp(type,"function.void") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, خطأ, vpiOtherFunc, 0, BIT4_0);
      } وإلا 𐡄𐡍 (strcmp(type,"autofunction.void") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, 𐡒𐡔𐡈, vpiOtherFunc, 0, BIT4_0);
      } وإلا 𐡄𐡍 (strcmp(type,"task") == 0) {
	    scope = جديد vpiScopeTask(name, tname);
      } وإلا 𐡄𐡍 (strcmp(type,"autotask") == 0) {
	    scope = جديد vpiScopeTaskAuto(name, tname);
      } وإلا 𐡄𐡍 (strcmp(type,"fork") == 0) {
	    scope = جديد vpiScopeFork(name, tname);
      } وإلا 𐡄𐡍 (strcmp(type,"autofork") == 0) {
	    scope = جديد vpiScopeForkAuto(name, tname);
      } وإلا 𐡄𐡍 (strcmp(type,"begin") == 0) {
	    scope = جديد vpiScopeBegin(name, tname);
      } وإلا 𐡄𐡍 (strcmp(type,"autobegin") == 0) {
	    scope = جديد vpiScopeBeginAuto(name, tname);
      } وإلا 𐡄𐡍 (strcmp(type,"generate") == 0) {
	    scope = جديد vpiScopeGenerate(name, tname);
      } وإلا 𐡄𐡍 (strcmp(type,"package") == 0) {
	    scope = جديد vpiScopePackage(name, tname);
      } وإلا 𐡄𐡍 (strcmp(type,"class") == 0) {
	    scope = جديد vpiScopeClass(name, tname);
      } وإلا {
	    scope = جديد vpiScopeModule(name, tname);
	    أكد(0);
      }

      scope->file_idx = (unsigned) file_idx;
      scope->lineno  = (unsigned) lineno;
      scope->def_file_idx = (unsigned) def_file_idx;
      scope->def_lineno  = (unsigned) def_lineno;
      scope->item = 0;
      scope->nitem = 0;
      scope->live_contexts = 0;
      scope->free_contexts = 0;

      𐡄𐡍 (is_cell) scope->is_cell = 𐡒𐡔𐡈;
      وإلا scope->is_cell = خطأ;

      current_scope = scope;

      compile_vpi_symbol(label, scope);

      حرر(label);
      حرر(type);
      delete[] name;
      delete[] tname;

      𐡄𐡍 (parent) {
	    ساكن vpiHandle obj;
	    compile_vpi_lookup(&obj, parent);
	    أكد(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_to_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->time_units = sp->time_units;
	    scope->time_precision = sp->time_precision;

      } وإلا {
	    scope->scope = 0x0;

	    vpip_root_table.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->time_units = vpip_get_time_precision();
	    scope->time_precision = vpip_get_time_precision();
      }
}

فراغ compile_scope_recall(حرف*symbol)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(فراغ*)&current_scope, symbol);
      أكد(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
فراغ compile_timescale(long units, long precision)
{
      أكد(current_scope);
      current_scope->time_units = units;
      current_scope->time_precision = precision;
}

__vpiScope* vpip_peek_current_scope(فراغ)
{
      أرجع current_scope;
}

فراغ vpip_attach_to_current_scope(vpiHandle obj)
{
      vpip_attach_to_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(فراغ)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      طالما (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      أرجع scope;
}

unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      أكد(scope);
      أكد(scope->is_automatic());

      unsigned idx = scope->nitem++;

      𐡄𐡍 (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (malloc(حجم(automatic_hooks_s*)));
      وإلا
	    scope->item = static_cast<automatic_hooks_s**>
	                  (realloc(scope->item,
	                   حجم(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      أرجع 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              unsigned index,
              صحيح vpi_direction,
              unsigned width,
              ثابت حرف *name,
              حرف* buffer) :
      parent_(parent),
      index_(index),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      𐡄𐡍 (buffer != nullptr) functor_ref_lookup(&ref_, buffer);
      وإلا ref_ = nullptr;
}

vpiPortInfo::~vpiPortInfo()
{
      delete[] name_;
}

#ifdef CHECK_WITH_VALGRIND
فراغ port_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortInfo *>(handle);
}
#endif

صحيح vpiPortInfo::vpi_get(صحيح code)
{
      بدل( code ) {

        حالة vpiDirection :
          أرجع direction_;
        حالة vpiPortIndex :
          أرجع index_;
        حالة vpiSize :
          أرجع width_;
        افتراضي :
          أرجع vpiUndefined;
      }

}


حرف *vpiPortInfo::vpi_get_str(صحيح code)
{
      بدل( code ) {
        حالة vpiName :
          أرجع simple_set_rbuf_str(name_);
        افتراضي :
          أرجع عدم;
      }

}


vpiHandle vpiPortInfo::vpi_handle(صحيح code)
{

      بدل (code) {

          حالة vpiParent:
          حالة vpiScope:
          حالة vpiModule:
            أرجع parent_;
          افتراضي :
            اكسر;
      }

      أرجع 0;
}

ساكن vpiHandle portinfo_iterate(صحيح code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      أكد(rfp);
      unsigned width = rfp->get_width();

      بدل (code) {
	  حالة vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(width, حجم(vpiHandle*)));

	    لكل (unsigned i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    أرجع vpip_make_iterator(width, args, 𐡒𐡔𐡈);
	  }
      }
      أرجع 0;
}

vpiHandle vpiPortInfo::vpi_iterate(صحيح code)
{
      أرجع portinfo_iterate(code, هذا);
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
فراغ port_bit_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortBitInfo *>(handle);
}
#endif

vpiHandle vpiPortBitInfo::vpi_handle(صحيح code)
{

      بدل (code) {
          حالة vpiParent:
            أرجع parent_;
          افتراضي :
            اكسر;
      }

      أرجع 0;
}

صحيح vpiPortBitInfo::vpi_get(صحيح code)
{
      بدل( code ) {
        حالة vpiBit :
          أرجع bit_;
        افتراضي :
          اكسر;
      }

      أرجع 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
فراغ compile_port_info( unsigned index, صحيح vpi_direction, unsigned width, ثابت حرف *name, حرف* buffer )
{
      vpiPortInfo* obj = جديد vpiPortInfo( vpip_peek_current_scope(),
                                          index, vpi_direction, width, name, buffer );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      لكل (unsigned i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = جديد vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
