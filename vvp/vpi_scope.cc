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

using নামাকাশ std;

স্থির vector<vpiHandle> vpip_মূল_table;

স্থির vpiHandle make_subset_iterator_(পূর্ণসংখ্যা type_code, ধ্রুবক vector<vpiHandle>&table);

vpiHandle vpip_make_মূল_iterator(পূর্ণসংখ্যা type_code)
{
      ফেরত make_subset_iterator_(type_code, vpip_মূল_table);
}

শূন্য vpip_make_মূল_iterator(__vpiHandle**&table, unsigned&ntable)
{
      table = &vpip_মূল_table[0];
      ntable = vpip_মূল_table.size();
}

#ifdef CHECK_WITH_VALGRIND
শূন্য port_delete(__vpiHandle*handle);
শূন্য port_bit_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
স্থির class_type **class_list = 0;
স্থির unsigned class_list_গুনতি = 0;

স্থির শূন্য delete_sub_scopes(__vpiScope *scope)
{
      জন্য (unsigned idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle item = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(item);
	    নির্বাচন(item->get_type_code()) {
		ক্ষেত্র vpiFunction:
		ক্ষেত্র vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		ক্ষেত্র vpiModule:
		ক্ষেত্র vpiGenScope:
		ক্ষেত্র vpiNamedBegin:
		ক্ষেত্র vpiNamedFork:
		ক্ষেত্র vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  delete item;
		  বিরতি;
		ক্ষেত্র vpiMemory:
		ক্ষেত্র vpiNetArray:
		  memory_delete(item);
		  বিরতি;
		ক্ষেত্র vpiModPath:
		    /* The destination ModPath is cleaned up later. */
		  delete item;
		  বিরতি;
		ক্ষেত্র vpiInterModPath:
		  delete item;
		  বিরতি;
		ক্ষেত্র vpiNamedEvent:
		  named_event_delete(item);
		  বিরতি;
		ক্ষেত্র vpiNet:
		ক্ষেত্র vpiReg:
		ক্ষেত্র vpiIntegerVar:
		ক্ষেত্র vpiLongIntVar:
		ক্ষেত্র vpiShortIntVar:
		ক্ষেত্র vpiIntVar:
		ক্ষেত্র vpiByteVar:
		ক্ষেত্র vpiBitVar:
		  signal_delete(item);
		  বিরতি;
		ক্ষেত্র vpiParameter:
		  parameter_delete(item);
		  বিরতি;
		ক্ষেত্র vpiRealVar:
		  real_delete(item);
		  বিরতি;
		ক্ষেত্র vpiEnumTypespec:
		  enum_delete(item);
		  বিরতি;
		ক্ষেত্র vpiPort:
		  port_delete(item);
		  বিরতি;
		ক্ষেত্র vpiPortBit:
		  port_bit_delete(item);
		  বিরতি;
		ক্ষেত্র vpiStringVar:
		  string_delete(item);
		  বিরতি;
		ক্ষেত্র vpiClassVar:
		  class_delete(item);
		  বিরতি;
		ক্ষেত্র vpiArrayVar:
		  array_delete(item);
		  বিরতি;
		পূর্বনির্ধারিত:
		  fprintf(stderr, "Need support for type: %d\n",
		          item->get_type_code());
		  প্রতিজ্ঞা(0);
		  বিরতি;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      জন্য (citer = scope->classes.begin();
           citer != scope->classes.end(); ++ citer ) {
	    class_list_গুনতি += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_গুনতি*আকার(class_type **)));
	    class_list[class_list_গুনতি-1] = citer->second;
      }
}

শূন্য মূল_table_delete(শূন্য)
{
      জন্য (unsigned idx = 0; idx < vpip_মূল_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_মূল_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    delete scope;
      }
      vpip_মূল_table.clear();

	/* Clean up all the class definitions. */
      জন্য (unsigned idx = 0; idx < class_list_গুনতি; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      মুক্ত(class_list);
      class_list = 0;
      class_list_গুনতি = 0;
}
#endif

স্থির শূন্য construct_scope_fullname(__vpiScope*ref, অক্ষর*buf)
{
      যদি (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    যদি (ref->scope->get_type_code() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      যদি (ref->get_type_code() == vpiPackage) strcat(buf, "::");
}

স্থির ধ্রুবক অক্ষর* scope_get_type(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
          ক্ষেত্র vpiModule:
            ফেরত "vpiModule";
          ক্ষেত্র vpiGenScope:
            ফেরত "vpiGenScope";
          ক্ষেত্র vpiFunction:
            ফেরত "vpiFunction";
          ক্ষেত্র vpiTask:
            ফেরত "vpiTask";
          ক্ষেত্র vpiNamedBegin:
            ফেরত "vpiNamedBegin";
          ক্ষেত্র vpiNamedFork:
            ফেরত "vpiNamedFork";
          পূর্বনির্ধারিত:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            ফেরত ফাঁকা;
      }
}

স্থির অক্ষর* scope_get_str(পূর্ণসংখ্যা code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      প্রতিজ্ঞা(ref);

      অক্ষর buf[4096];  // XXX is a fixed buffer size really reliable?
      ধ্রুবক অক্ষর *p=0;
      নির্বাচন (code) {
	  ক্ষেত্র vpiDefFile:
	    p = file_names[ref->def_file_idx];
	    বিরতি;

	  ক্ষেত্র vpiFile:
	    p = file_names[ref->file_idx];
	    বিরতি;

	  ক্ষেত্র vpiFullName:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    বিরতি;

	  ক্ষেত্র vpiName:
	    p = ref->scope_name();
	    বিরতি;

	  ক্ষেত্র vpiDefName:
	    p = ref->scope_def_name();
	    বিরতি;

	  ক্ষেত্র vpiType:
	    p = scope_get_type(code);
	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    ফেরত ফাঁকা;
      }
      ফেরত simple_set_rbuf_str(p);
}

স্থির vpiHandle scope_get_handle(পূর্ণসংখ্যা code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      প্রতিজ্ঞা(rfp);

      নির্বাচন (code) {

	  ক্ষেত্র vpiScope:
	    ফেরত rfp->scope;

	  ক্ষেত্র vpiModule:
	    ফেরত rfp->scope;
      }

      ফেরত 0;
}

/* compares vpiType's considering object classes */
স্থির পূর্ণসংখ্যা compare_types(পূর্ণসংখ্যা code, পূর্ণসংখ্যা type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      যদি (code == vpiScope)
	    ফেরত 1;

      যদি (code == type)
	    ফেরত 1;

      যদি ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    ফেরত 1;

      যদি ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    ফেরত 1;

      যদি ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    ফেরত 1;

      ফেরত 0;
}

স্থির vpiHandle make_subset_iterator_(পূর্ণসংখ্যা type_code, ধ্রুবক vector<vpiHandle>&table)
{
      unsigned mcnt = 0, ncnt = 0;
      vpiHandle*args;

      জন্য (unsigned idx = 0; idx < table.size(); idx += 1)
	    যদি (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      যদি (mcnt == 0)
	    ফেরত 0;

      args = static_cast<vpiHandle*>(calloc(mcnt, আকার(vpiHandle)));
      জন্য (unsigned idx = 0; idx < table.size(); idx += 1)
	    যদি (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      প্রতিজ্ঞা(ncnt == mcnt);

      ফেরত vpip_make_iterator(mcnt, args, সত্য);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
স্থির vpiHandle module_iter(পূর্ণসংখ্যা code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      প্রতিজ্ঞা(ref);

      ফেরত make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(ধ্রুবক অক্ষর*nam, ধ্রুবক অক্ষর*tnam, যৌক্তিক auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

পূর্ণসংখ্যা __vpiScope::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiCellInstance:
	    ফেরত is_cell? 1 : 0;

	  ক্ষেত্র vpi_Def_সারি_No:
	    ফেরত def_lineno;

	  ক্ষেত্র vpi_সারি_No:
	    ফেরত lineno;

	  ক্ষেত্র vpiTimeUnit:
	    ফেরত time_units;

	  ক্ষেত্র vpiTimePrecision:
	    ফেরত time_precision;

	  ক্ষেত্র vpi_শীর্ষ_Module:
	    ফেরত 0x0 == scope;

          ক্ষেত্র vpiAutomatic:
	    ফেরত is_automatic_? 1 : 0;
      }

      ফেরত vpiUndefined;
}


অক্ষর*__vpiScope::vpi_get_str(পূর্ণসংখ্যা code)
{ ফেরত scope_get_str(code, এই); }

vpiHandle __vpiScope::vpi_handle(পূর্ণসংখ্যা code)
{ ফেরত scope_get_handle(code, এই); }

vpiHandle __vpiScope::vpi_iterate(পূর্ণসংখ্যা code)
{ ফেরত module_iter(code, এই); }


শ্রেণী vpiScopeModule  : সর্বজনীন __vpiScope {
    সর্বজনীন:
      অন্তঃরেখ vpiScopeModule(ধ্রুবক অক্ষর*nam, ধ্রুবক অক্ষর*tnam)
      : __vpiScope(nam,tnam,মিথ্যা) { }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiModule; }
};

কাঠামো vpiScopePackage  : সর্বজনীন __vpiScope {
      অন্তঃরেখ vpiScopePackage(ধ্রুবক অক্ষর*nam, ধ্রুবক অক্ষর*tnam)
      : __vpiScope(nam,tnam) { }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiPackage; }
};

কাঠামো vpiScopeTask  : সর্বজনীন __vpiScope {
      অন্তঃরেখ vpiScopeTask(ধ্রুবক অক্ষর*nam, ধ্রুবক অক্ষর*tnam)
      : __vpiScope(nam,tnam) { }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiTask; }
};

কাঠামো vpiScopeTaskAuto  : সর্বজনীন __vpiScope {
      অন্তঃরেখ vpiScopeTaskAuto(ধ্রুবক অক্ষর*nam, ধ্রুবক অক্ষর*tnam)
      : __vpiScope(nam,tnam,সত্য) {  }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiTask; }
};

কাঠামো vpiScopeBegin  : সর্বজনীন __vpiScope {
      অন্তঃরেখ vpiScopeBegin(ধ্রুবক অক্ষর*nam, ধ্রুবক অক্ষর*tnam)
      : __vpiScope(nam,tnam,মিথ্যা) { }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiNamedBegin; }
};

শ্রেণী vpiScopeBeginAuto  : সর্বজনীন __vpiScope {
    সর্বজনীন:
      অন্তঃরেখ vpiScopeBeginAuto(ধ্রুবক অক্ষর*nam, ধ্রুবক অক্ষর*tnam)
      : __vpiScope(nam,tnam,সত্য) {  }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiNamedBegin; }
};

কাঠামো vpiScopeGenerate  : সর্বজনীন __vpiScope {
      অন্তঃরেখ vpiScopeGenerate(ধ্রুবক অক্ষর*nam, ধ্রুবক অক্ষর*tnam)
      : __vpiScope(nam,tnam) { }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiGenScope; }
};

কাঠামো vpiScopeFork  : সর্বজনীন __vpiScope {
      অন্তঃরেখ vpiScopeFork(ধ্রুবক অক্ষর*nam, ধ্রুবক অক্ষর*tnam)
      : __vpiScope(nam,tnam,মিথ্যা) { }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiNamedFork; }
};

শ্রেণী vpiScopeForkAuto  : সর্বজনীন __vpiScope {
    সর্বজনীন:
      অন্তঃরেখ vpiScopeForkAuto(ধ্রুবক অক্ষর*nam, ধ্রুবক অক্ষর*tnam)
      : __vpiScope(nam,tnam,সত্য) {  }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiNamedFork; }
};

কাঠামো vpiScopeClass  : সর্বজনীন __vpiScope {
      অন্তঃরেখ vpiScopeClass(ধ্রুবক অক্ষর*nam, ধ্রুবক অক্ষর*tnam)
      : __vpiScope(nam,tnam) { }
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
স্থির __vpiScope*current_scope = 0;

শূন্য vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj)
{
      প্রতিজ্ঞা(scope);
      scope->intern.push_back(obj);
}

/*
 * When the compiler encounters a scope declaration, this function
 * creates and initializes a __vpiScope object with the requested name
 * and within the addressed parent. The label is used as a key in the
 * symbol table and the name is used to construct the actual object.
 */
শূন্য
compile_scope_decl(অক্ষর*label, অক্ষর*type, অক্ষর*name, অক্ষর*tname,
                   অক্ষর*parent, long file_idx, long lineno,
                   long def_file_idx, long def_lineno, long is_cell)
{
      গুনতি_vpi_scopes += 1;
      অক্ষর vec_type;
      অক্ষর sign_flag;
      unsigned wid;

      __vpiScope*scope;
      যদি (strcmp(type,"module") == 0) {
	    scope = নতুন vpiScopeModule(name, tname);
      } অন্যথা যদি ( sscanf(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    পূর্ণসংখ্যা type_code;
	    যদি (sign_flag=='s') {
		  type_code = vpiSizedSignedFunc;
	    } অন্যথা যদি (sign_flag=='u') {
		  type_code = vpiSizedFunc;
	    } অন্যথা যদি (sign_flag=='i') {
		  type_code = vpiIntFunc;
	    } অন্যথা {
		  প্রতিজ্ঞা(0);
		  type_code = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = নতুন vpiScopeFunction(name, tname, মিথ্যা, type_code, wid, init_val);

      } অন্যথা যদি ( sscanf(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    পূর্ণসংখ্যা type_code;
	    নির্বাচন (sign_flag) {
		ক্ষেত্র 's':
		  type_code = vpiSizedSignedFunc;
		  বিরতি;
		ক্ষেত্র 'u':
		  type_code = vpiSizedFunc;
		  বিরতি;
		পূর্বনির্ধারিত:
		  প্রতিজ্ঞা(0);
		  type_code = vpiSizedFunc;
		  বিরতি;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = নতুন vpiScopeFunction(name, tname, সত্য, type_code, wid, init_val);

      } অন্যথা যদি (strcmp(type,"function.obj") == 0) {
	    scope = নতুন vpiScopeFunction(name, tname, মিথ্যা, vpiSizedFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"autofunction.obj") == 0) {
	    scope = নতুন vpiScopeFunction(name, tname, সত্য, vpiSizedFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"function.real") == 0) {
	    scope = নতুন vpiScopeFunction(name, tname, মিথ্যা, vpiRealFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"autofunction.real") == 0) {
	    scope = নতুন vpiScopeFunction(name, tname, সত্য, vpiRealFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"function.str") == 0) {
	    scope = নতুন vpiScopeFunction(name, tname, মিথ্যা, vpiOtherFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"autofunction.str") == 0) {
	    scope = নতুন vpiScopeFunction(name, tname, সত্য, vpiOtherFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"function.void") == 0) {
	    scope = নতুন vpiScopeFunction(name, tname, মিথ্যা, vpiOtherFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"autofunction.void") == 0) {
	    scope = নতুন vpiScopeFunction(name, tname, সত্য, vpiOtherFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"task") == 0) {
	    scope = নতুন vpiScopeTask(name, tname);
      } অন্যথা যদি (strcmp(type,"autotask") == 0) {
	    scope = নতুন vpiScopeTaskAuto(name, tname);
      } অন্যথা যদি (strcmp(type,"fork") == 0) {
	    scope = নতুন vpiScopeFork(name, tname);
      } অন্যথা যদি (strcmp(type,"autofork") == 0) {
	    scope = নতুন vpiScopeForkAuto(name, tname);
      } অন্যথা যদি (strcmp(type,"begin") == 0) {
	    scope = নতুন vpiScopeBegin(name, tname);
      } অন্যথা যদি (strcmp(type,"autobegin") == 0) {
	    scope = নতুন vpiScopeBeginAuto(name, tname);
      } অন্যথা যদি (strcmp(type,"generate") == 0) {
	    scope = নতুন vpiScopeGenerate(name, tname);
      } অন্যথা যদি (strcmp(type,"package") == 0) {
	    scope = নতুন vpiScopePackage(name, tname);
      } অন্যথা যদি (strcmp(type,"class") == 0) {
	    scope = নতুন vpiScopeClass(name, tname);
      } অন্যথা {
	    scope = নতুন vpiScopeModule(name, tname);
	    প্রতিজ্ঞা(0);
      }

      scope->file_idx = (unsigned) file_idx;
      scope->lineno  = (unsigned) lineno;
      scope->def_file_idx = (unsigned) def_file_idx;
      scope->def_lineno  = (unsigned) def_lineno;
      scope->item = 0;
      scope->nitem = 0;
      scope->live_contexts = 0;
      scope->free_contexts = 0;

      যদি (is_cell) scope->is_cell = সত্য;
      অন্যথা scope->is_cell = মিথ্যা;

      current_scope = scope;

      compile_vpi_প্রতীক(label, scope);

      মুক্ত(label);
      মুক্ত(type);
      delete[] name;
      delete[] tname;

      যদি (parent) {
	    স্থির vpiHandle obj;
	    compile_vpi_lookup(&obj, parent);
	    প্রতিজ্ঞা(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_to_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->time_units = sp->time_units;
	    scope->time_precision = sp->time_precision;

      } অন্যথা {
	    scope->scope = 0x0;

	    vpip_মূল_table.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->time_units = vpip_get_time_precision();
	    scope->time_precision = vpip_get_time_precision();
      }
}

শূন্য compile_scope_recall(অক্ষর*প্রতীক)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(শূন্য*)&current_scope, প্রতীক);
      প্রতিজ্ঞা(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
শূন্য compile_timescale(long units, long precision)
{
      প্রতিজ্ঞা(current_scope);
      current_scope->time_units = units;
      current_scope->time_precision = precision;
}

__vpiScope* vpip_peek_current_scope(শূন্য)
{
      ফেরত current_scope;
}

শূন্য vpip_attach_to_current_scope(vpiHandle obj)
{
      vpip_attach_to_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(শূন্য)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      যখন (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      ফেরত scope;
}

unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      প্রতিজ্ঞা(scope);
      প্রতিজ্ঞা(scope->is_automatic());

      unsigned idx = scope->nitem++;

      যদি (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (malloc(আকার(automatic_hooks_s*)));
      অন্যথা
	    scope->item = static_cast<automatic_hooks_s**>
	                  (realloc(scope->item,
	                   আকার(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      ফেরত 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              unsigned সূচক,
              পূর্ণসংখ্যা vpi_direction,
              unsigned width,
              ধ্রুবক অক্ষর *name,
              অক্ষর* বাফার) :
      parent_(parent),
      সূচক_2(সূচক),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      যদি (বাফার != nullptr) functor_ref_lookup(&ref_, বাফার);
      অন্যথা ref_ = nullptr;
}

vpiPortInfo::~vpiPortInfo()
{
      delete[] name_;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য port_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortInfo *>(handle);
}
#endif

পূর্ণসংখ্যা vpiPortInfo::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন( code ) {

        ক্ষেত্র vpiDirection :
          ফেরত direction_;
        ক্ষেত্র vpi_Port_সূচক :
          ফেরত সূচক_2;
        ক্ষেত্র vpi_আকার :
          ফেরত width_;
        পূর্বনির্ধারিত :
          ফেরত vpiUndefined;
      }

}


অক্ষর *vpiPortInfo::vpi_get_str(পূর্ণসংখ্যা code)
{
      নির্বাচন( code ) {
        ক্ষেত্র vpiName :
          ফেরত simple_set_rbuf_str(name_);
        পূর্বনির্ধারিত :
          ফেরত ফাঁকা;
      }

}


vpiHandle vpiPortInfo::vpi_handle(পূর্ণসংখ্যা code)
{

      নির্বাচন (code) {

          ক্ষেত্র vpiParent:
          ক্ষেত্র vpiScope:
          ক্ষেত্র vpiModule:
            ফেরত parent_;
          পূর্বনির্ধারিত :
            বিরতি;
      }

      ফেরত 0;
}

স্থির vpiHandle portinfo_iterate(পূর্ণসংখ্যা code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      প্রতিজ্ঞা(rfp);
      unsigned width = rfp->get_width();

      নির্বাচন (code) {
	  ক্ষেত্র vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(width, আকার(vpiHandle*)));

	    জন্য (unsigned i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    ফেরত vpip_make_iterator(width, args, সত্য);
	  }
      }
      ফেরত 0;
}

vpiHandle vpiPortInfo::vpi_iterate(পূর্ণসংখ্যা code)
{
      ফেরত portinfo_iterate(code, এই);
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
শূন্য port_bit_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortBitInfo *>(handle);
}
#endif

vpiHandle vpiPortBitInfo::vpi_handle(পূর্ণসংখ্যা code)
{

      নির্বাচন (code) {
          ক্ষেত্র vpiParent:
            ফেরত parent_;
          পূর্বনির্ধারিত :
            বিরতি;
      }

      ফেরত 0;
}

পূর্ণসংখ্যা vpiPortBitInfo::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন( code ) {
        ক্ষেত্র vpiBit :
          ফেরত bit_;
        পূর্বনির্ধারিত :
          বিরতি;
      }

      ফেরত 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
শূন্য compile_port_info( unsigned সূচক, পূর্ণসংখ্যা vpi_direction, unsigned width, ধ্রুবক অক্ষর *name, অক্ষর* বাফার )
{
      vpiPortInfo* obj = নতুন vpiPortInfo( vpip_peek_current_scope(),
                                          সূচক, vpi_direction, width, name, বাফার );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      জন্য (unsigned i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = নতুন vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
