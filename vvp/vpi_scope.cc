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

using فضاینام std;

ایستا vector<vpiHandle> vpip_ریشه_table;

ایستا vpiHandle make_subset_iterator_(صحیح type_code, ثابت vector<vpiHandle>&table);

vpiHandle vpip_make_ریشه_iterator(صحیح type_code)
{
      بازگشت make_subset_iterator_(type_code, vpip_ریشه_table);
}

تهی vpip_make_ریشه_iterator(__vpiHandle**&table, unsigned&ntable)
{
      table = &vpip_ریشه_table[0];
      ntable = vpip_ریشه_table.size();
}

#ifdef CHECK_WITH_VALGRIND
تهی port_delete(__vpiHandle*handle);
تهی port_bit_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
ایستا class_type **class_list = 0;
ایستا unsigned class_list_شمار = 0;

ایستا تهی delete_sub_scopes(__vpiScope *scope)
{
      برای (unsigned idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle item = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(item);
	    انتخاب(item->get_type_code()) {
		حالت vpiFunction:
		حالت vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		حالت vpiModule:
		حالت vpiGenScope:
		حالت vpiNamedBegin:
		حالت vpiNamedFork:
		حالت vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  delete item;
		  بشکن;
		حالت vpiMemory:
		حالت vpiNetArray:
		  memory_delete(item);
		  بشکن;
		حالت vpiModPath:
		    /* The destination ModPath is cleaned up later. */
		  delete item;
		  بشکن;
		حالت vpiInterModPath:
		  delete item;
		  بشکن;
		حالت vpiNamedEvent:
		  named_event_delete(item);
		  بشکن;
		حالت vpiNet:
		حالت vpiReg:
		حالت vpiIntegerVar:
		حالت vpiLongIntVar:
		حالت vpiShortIntVar:
		حالت vpiIntVar:
		حالت vpiByteVar:
		حالت vpiBitVar:
		  signal_delete(item);
		  بشکن;
		حالت vpiParameter:
		  parameter_delete(item);
		  بشکن;
		حالت vpiRealVar:
		  real_delete(item);
		  بشکن;
		حالت vpiEnumTypespec:
		  enum_delete(item);
		  بشکن;
		حالت vpiPort:
		  port_delete(item);
		  بشکن;
		حالت vpiPortBit:
		  port_bit_delete(item);
		  بشکن;
		حالت vpiStringVar:
		  string_delete(item);
		  بشکن;
		حالت vpiClassVar:
		  class_delete(item);
		  بشکن;
		حالت vpiArrayVar:
		  array_delete(item);
		  بشکن;
		پیشفرض:
		  fprintf(stderr, "Need support for type: %d\n",
		          item->get_type_code());
		  أكد(0);
		  بشکن;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      برای (citer = scope->classes.begin();
           citer != scope->classes.end(); ++ citer ) {
	    class_list_شمار += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_شمار*اندازه(class_type **)));
	    class_list[class_list_شمار-1] = citer->second;
      }
}

تهی ریشه_table_delete(تهی)
{
      برای (unsigned idx = 0; idx < vpip_ریشه_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_ریشه_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    delete scope;
      }
      vpip_ریشه_table.clear();

	/* Clean up all the class definitions. */
      برای (unsigned idx = 0; idx < class_list_شمار; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      آزاد(class_list);
      class_list = 0;
      class_list_شمار = 0;
}
#endif

ایستا تهی construct_scope_fullname(__vpiScope*ref, نویسه*buf)
{
      اگر (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    اگر (ref->scope->get_type_code() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      اگر (ref->get_type_code() == vpiPackage) strcat(buf, "::");
}

ایستا ثابت نویسه* scope_get_type(صحیح code)
{
      انتخاب (code) {
          حالت vpiModule:
            بازگشت "vpiModule";
          حالت vpiGenScope:
            بازگشت "vpiGenScope";
          حالت vpiFunction:
            بازگشت "vpiFunction";
          حالت vpiTask:
            بازگشت "vpiTask";
          حالت vpiNamedBegin:
            بازگشت "vpiNamedBegin";
          حالت vpiNamedFork:
            بازگشت "vpiNamedFork";
          پیشفرض:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            بازگشت پوچ;
      }
}

ایستا نویسه* scope_get_str(صحیح code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      أكد(ref);

      نویسه buf[4096];  // XXX is a fixed buffer size really reliable?
      ثابت نویسه *p=0;
      انتخاب (code) {
	  حالت vpiDefFile:
	    p = file_names[ref->def_file_idx];
	    بشکن;

	  حالت vpiFile:
	    p = file_names[ref->file_idx];
	    بشکن;

	  حالت vpiFullName:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    بشکن;

	  حالت vpiName:
	    p = ref->scope_name();
	    بشکن;

	  حالت vpiDefName:
	    p = ref->scope_def_name();
	    بشکن;

	  حالت vpiType:
	    p = scope_get_type(code);
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    بازگشت پوچ;
      }
      بازگشت simple_set_rbuf_str(p);
}

ایستا vpiHandle scope_get_handle(صحیح code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      أكد(rfp);

      انتخاب (code) {

	  حالت vpiScope:
	    بازگشت rfp->scope;

	  حالت vpiModule:
	    بازگشت rfp->scope;
      }

      بازگشت 0;
}

/* compares vpiType's considering object classes */
ایستا صحیح compare_types(صحیح code, صحیح type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      اگر (code == vpiScope)
	    بازگشت 1;

      اگر (code == type)
	    بازگشت 1;

      اگر ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    بازگشت 1;

      اگر ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    بازگشت 1;

      اگر ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    بازگشت 1;

      بازگشت 0;
}

ایستا vpiHandle make_subset_iterator_(صحیح type_code, ثابت vector<vpiHandle>&table)
{
      unsigned mcnt = 0, ncnt = 0;
      vpiHandle*args;

      برای (unsigned idx = 0; idx < table.size(); idx += 1)
	    اگر (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      اگر (mcnt == 0)
	    بازگشت 0;

      args = static_cast<vpiHandle*>(calloc(mcnt, اندازه(vpiHandle)));
      برای (unsigned idx = 0; idx < table.size(); idx += 1)
	    اگر (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      أكد(ncnt == mcnt);

      بازگشت vpip_make_iterator(mcnt, args, درست);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
ایستا vpiHandle module_iter(صحیح code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      أكد(ref);

      بازگشت make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(ثابت نویسه*nam, ثابت نویسه*tnam, منطقی auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

صحیح __vpiScope::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiCellInstance:
	    بازگشت is_cell? 1 : 0;

	  حالت vpi_Def_سطر_No:
	    بازگشت def_lineno;

	  حالت vpi_سطر_No:
	    بازگشت lineno;

	  حالت vpiTimeUnit:
	    بازگشت time_units;

	  حالت vpiTimePrecision:
	    بازگشت time_precision;

	  حالت vpi_بالاترین_Module:
	    بازگشت 0x0 == scope;

          حالت vpiAutomatic:
	    بازگشت is_automatic_? 1 : 0;
      }

      بازگشت vpiUndefined;
}


نویسه*__vpiScope::vpi_get_str(صحیح code)
{ بازگشت scope_get_str(code, این); }

vpiHandle __vpiScope::vpi_handle(صحیح code)
{ بازگشت scope_get_handle(code, این); }

vpiHandle __vpiScope::vpi_iterate(صحیح code)
{ بازگشت module_iter(code, این); }


رده vpiScopeModule  : عمومی __vpiScope {
    عمومی:
      درونخطی vpiScopeModule(ثابت نویسه*nam, ثابت نویسه*tnam)
      : __vpiScope(nam,tnam,نادرست) { }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiModule; }
};

ساختار vpiScopePackage  : عمومی __vpiScope {
      درونخطی vpiScopePackage(ثابت نویسه*nam, ثابت نویسه*tnam)
      : __vpiScope(nam,tnam) { }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiPackage; }
};

ساختار vpiScopeTask  : عمومی __vpiScope {
      درونخطی vpiScopeTask(ثابت نویسه*nam, ثابت نویسه*tnam)
      : __vpiScope(nam,tnam) { }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiTask; }
};

ساختار vpiScopeTaskAuto  : عمومی __vpiScope {
      درونخطی vpiScopeTaskAuto(ثابت نویسه*nam, ثابت نویسه*tnam)
      : __vpiScope(nam,tnam,درست) {  }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiTask; }
};

ساختار vpiScopeBegin  : عمومی __vpiScope {
      درونخطی vpiScopeBegin(ثابت نویسه*nam, ثابت نویسه*tnam)
      : __vpiScope(nam,tnam,نادرست) { }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiNamedBegin; }
};

رده vpiScopeBeginAuto  : عمومی __vpiScope {
    عمومی:
      درونخطی vpiScopeBeginAuto(ثابت نویسه*nam, ثابت نویسه*tnam)
      : __vpiScope(nam,tnam,درست) {  }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiNamedBegin; }
};

ساختار vpiScopeGenerate  : عمومی __vpiScope {
      درونخطی vpiScopeGenerate(ثابت نویسه*nam, ثابت نویسه*tnam)
      : __vpiScope(nam,tnam) { }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiGenScope; }
};

ساختار vpiScopeFork  : عمومی __vpiScope {
      درونخطی vpiScopeFork(ثابت نویسه*nam, ثابت نویسه*tnam)
      : __vpiScope(nam,tnam,نادرست) { }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiNamedFork; }
};

رده vpiScopeForkAuto  : عمومی __vpiScope {
    عمومی:
      درونخطی vpiScopeForkAuto(ثابت نویسه*nam, ثابت نویسه*tnam)
      : __vpiScope(nam,tnam,درست) {  }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiNamedFork; }
};

ساختار vpiScopeClass  : عمومی __vpiScope {
      درونخطی vpiScopeClass(ثابت نویسه*nam, ثابت نویسه*tnam)
      : __vpiScope(nam,tnam) { }
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
ایستا __vpiScope*current_scope = 0;

تهی vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj)
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
تهی
compile_scope_decl(نویسه*label, نویسه*type, نویسه*name, نویسه*tname,
                   نویسه*parent, long file_idx, long lineno,
                   long def_file_idx, long def_lineno, long is_cell)
{
      شمار_vpi_scopes += 1;
      نویسه vec_type;
      نویسه sign_flag;
      unsigned wid;

      __vpiScope*scope;
      اگر (strcmp(type,"module") == 0) {
	    scope = جدید vpiScopeModule(name, tname);
      } وگرنه اگر ( sscanf(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    صحیح type_code;
	    اگر (sign_flag=='s') {
		  type_code = vpiSizedSignedFunc;
	    } وگرنه اگر (sign_flag=='u') {
		  type_code = vpiSizedFunc;
	    } وگرنه اگر (sign_flag=='i') {
		  type_code = vpiIntFunc;
	    } وگرنه {
		  أكد(0);
		  type_code = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = جدید vpiScopeFunction(name, tname, نادرست, type_code, wid, init_val);

      } وگرنه اگر ( sscanf(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    صحیح type_code;
	    انتخاب (sign_flag) {
		حالت 's':
		  type_code = vpiSizedSignedFunc;
		  بشکن;
		حالت 'u':
		  type_code = vpiSizedFunc;
		  بشکن;
		پیشفرض:
		  أكد(0);
		  type_code = vpiSizedFunc;
		  بشکن;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = جدید vpiScopeFunction(name, tname, درست, type_code, wid, init_val);

      } وگرنه اگر (strcmp(type,"function.obj") == 0) {
	    scope = جدید vpiScopeFunction(name, tname, نادرست, vpiSizedFunc, 0, BIT4_0);
      } وگرنه اگر (strcmp(type,"autofunction.obj") == 0) {
	    scope = جدید vpiScopeFunction(name, tname, درست, vpiSizedFunc, 0, BIT4_0);
      } وگرنه اگر (strcmp(type,"function.real") == 0) {
	    scope = جدید vpiScopeFunction(name, tname, نادرست, vpiRealFunc, 0, BIT4_0);
      } وگرنه اگر (strcmp(type,"autofunction.real") == 0) {
	    scope = جدید vpiScopeFunction(name, tname, درست, vpiRealFunc, 0, BIT4_0);
      } وگرنه اگر (strcmp(type,"function.str") == 0) {
	    scope = جدید vpiScopeFunction(name, tname, نادرست, vpiOtherFunc, 0, BIT4_0);
      } وگرنه اگر (strcmp(type,"autofunction.str") == 0) {
	    scope = جدید vpiScopeFunction(name, tname, درست, vpiOtherFunc, 0, BIT4_0);
      } وگرنه اگر (strcmp(type,"function.void") == 0) {
	    scope = جدید vpiScopeFunction(name, tname, نادرست, vpiOtherFunc, 0, BIT4_0);
      } وگرنه اگر (strcmp(type,"autofunction.void") == 0) {
	    scope = جدید vpiScopeFunction(name, tname, درست, vpiOtherFunc, 0, BIT4_0);
      } وگرنه اگر (strcmp(type,"task") == 0) {
	    scope = جدید vpiScopeTask(name, tname);
      } وگرنه اگر (strcmp(type,"autotask") == 0) {
	    scope = جدید vpiScopeTaskAuto(name, tname);
      } وگرنه اگر (strcmp(type,"fork") == 0) {
	    scope = جدید vpiScopeFork(name, tname);
      } وگرنه اگر (strcmp(type,"autofork") == 0) {
	    scope = جدید vpiScopeForkAuto(name, tname);
      } وگرنه اگر (strcmp(type,"begin") == 0) {
	    scope = جدید vpiScopeBegin(name, tname);
      } وگرنه اگر (strcmp(type,"autobegin") == 0) {
	    scope = جدید vpiScopeBeginAuto(name, tname);
      } وگرنه اگر (strcmp(type,"generate") == 0) {
	    scope = جدید vpiScopeGenerate(name, tname);
      } وگرنه اگر (strcmp(type,"package") == 0) {
	    scope = جدید vpiScopePackage(name, tname);
      } وگرنه اگر (strcmp(type,"class") == 0) {
	    scope = جدید vpiScopeClass(name, tname);
      } وگرنه {
	    scope = جدید vpiScopeModule(name, tname);
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

      اگر (is_cell) scope->is_cell = درست;
      وگرنه scope->is_cell = نادرست;

      current_scope = scope;

      compile_vpi_نماد(label, scope);

      آزاد(label);
      آزاد(type);
      delete[] name;
      delete[] tname;

      اگر (parent) {
	    ایستا vpiHandle obj;
	    compile_vpi_lookup(&obj, parent);
	    أكد(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_to_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->time_units = sp->time_units;
	    scope->time_precision = sp->time_precision;

      } وگرنه {
	    scope->scope = 0x0;

	    vpip_ریشه_table.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->time_units = vpip_get_time_precision();
	    scope->time_precision = vpip_get_time_precision();
      }
}

تهی compile_scope_recall(نویسه*نماد)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(تهی*)&current_scope, نماد);
      أكد(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
تهی compile_timescale(long units, long precision)
{
      أكد(current_scope);
      current_scope->time_units = units;
      current_scope->time_precision = precision;
}

__vpiScope* vpip_peek_current_scope(تهی)
{
      بازگشت current_scope;
}

تهی vpip_attach_to_current_scope(vpiHandle obj)
{
      vpip_attach_to_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(تهی)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      وقتی (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      بازگشت scope;
}

unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      أكد(scope);
      أكد(scope->is_automatic());

      unsigned idx = scope->nitem++;

      اگر (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (malloc(اندازه(automatic_hooks_s*)));
      وگرنه
	    scope->item = static_cast<automatic_hooks_s**>
	                  (realloc(scope->item,
	                   اندازه(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      بازگشت 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              unsigned index,
              صحیح vpi_direction,
              unsigned width,
              ثابت نویسه *name,
              نویسه* buffer) :
      parent_(parent),
      index_(index),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      اگر (buffer != nullptr) functor_ref_lookup(&ref_, buffer);
      وگرنه ref_ = nullptr;
}

vpiPortInfo::~vpiPortInfo()
{
      delete[] name_;
}

#ifdef CHECK_WITH_VALGRIND
تهی port_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortInfo *>(handle);
}
#endif

صحیح vpiPortInfo::vpi_get(صحیح code)
{
      انتخاب( code ) {

        حالت vpiDirection :
          بازگشت direction_;
        حالت vpiPortIndex :
          بازگشت index_;
        حالت vpiSize :
          بازگشت width_;
        پیشفرض :
          بازگشت vpiUndefined;
      }

}


نویسه *vpiPortInfo::vpi_get_str(صحیح code)
{
      انتخاب( code ) {
        حالت vpiName :
          بازگشت simple_set_rbuf_str(name_);
        پیشفرض :
          بازگشت پوچ;
      }

}


vpiHandle vpiPortInfo::vpi_handle(صحیح code)
{

      انتخاب (code) {

          حالت vpiParent:
          حالت vpiScope:
          حالت vpiModule:
            بازگشت parent_;
          پیشفرض :
            بشکن;
      }

      بازگشت 0;
}

ایستا vpiHandle portinfo_iterate(صحیح code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      أكد(rfp);
      unsigned width = rfp->get_width();

      انتخاب (code) {
	  حالت vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(width, اندازه(vpiHandle*)));

	    برای (unsigned i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    بازگشت vpip_make_iterator(width, args, درست);
	  }
      }
      بازگشت 0;
}

vpiHandle vpiPortInfo::vpi_iterate(صحیح code)
{
      بازگشت portinfo_iterate(code, این);
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
تهی port_bit_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortBitInfo *>(handle);
}
#endif

vpiHandle vpiPortBitInfo::vpi_handle(صحیح code)
{

      انتخاب (code) {
          حالت vpiParent:
            بازگشت parent_;
          پیشفرض :
            بشکن;
      }

      بازگشت 0;
}

صحیح vpiPortBitInfo::vpi_get(صحیح code)
{
      انتخاب( code ) {
        حالت vpiBit :
          بازگشت bit_;
        پیشفرض :
          بشکن;
      }

      بازگشت 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
تهی compile_port_info( unsigned index, صحیح vpi_direction, unsigned width, ثابت نویسه *name, نویسه* buffer )
{
      vpiPortInfo* obj = جدید vpiPortInfo( vpip_peek_current_scope(),
                                          index, vpi_direction, width, name, buffer );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      برای (unsigned i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = جدید vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
