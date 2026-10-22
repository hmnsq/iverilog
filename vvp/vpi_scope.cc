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

স্থির vpiHandle make_subset_iterator_(পূর্ণাঙ্ক type_code, ধ্রুৱ vector<vpiHandle>&table);

vpiHandle vpip_make_মূল_iterator(পূর্ণাঙ্ক type_code)
{
      ঘূৰাই make_subset_iterator_(type_code, vpip_মূল_table);
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
      বাবে (unsigned idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle item = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(item);
	    ৱিকল্প(item->get_type_code()) {
		পক্ষ vpiFunction:
		পক্ষ vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		পক্ষ vpiModule:
		পক্ষ vpiGenScope:
		পক্ষ vpiNamedBegin:
		পক্ষ vpiNamedFork:
		পক্ষ vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  delete item;
		  ভঙ্গ;
		পক্ষ vpiMemory:
		পক্ষ vpiNetArray:
		  memory_delete(item);
		  ভঙ্গ;
		পক্ষ vpiModPath:
		    /* The destination ModPath is cleaned up later. */
		  delete item;
		  ভঙ্গ;
		পক্ষ vpiInterModPath:
		  delete item;
		  ভঙ্গ;
		পক্ষ vpiNamedEvent:
		  named_event_delete(item);
		  ভঙ্গ;
		পক্ষ vpiNet:
		পক্ষ vpiReg:
		পক্ষ vpiIntegerVar:
		পক্ষ vpiLongIntVar:
		পক্ষ vpiShortIntVar:
		পক্ষ vpiIntVar:
		পক্ষ vpiByteVar:
		পক্ষ vpiBitVar:
		  signal_delete(item);
		  ভঙ্গ;
		পক্ষ vpiParameter:
		  parameter_delete(item);
		  ভঙ্গ;
		পক্ষ vpiRealVar:
		  real_delete(item);
		  ভঙ্গ;
		পক্ষ vpiEnumTypespec:
		  enum_delete(item);
		  ভঙ্গ;
		পক্ষ vpiPort:
		  port_delete(item);
		  ভঙ্গ;
		পক্ষ vpiPortBit:
		  port_bit_delete(item);
		  ভঙ্গ;
		পক্ষ vpiStringVar:
		  string_delete(item);
		  ভঙ্গ;
		পক্ষ vpiClassVar:
		  class_delete(item);
		  ভঙ্গ;
		পক্ষ vpiArrayVar:
		  array_delete(item);
		  ভঙ্গ;
		স্ৱভাৱ:
		  fprintf(stderr, "Need support for type: %d\n",
		          item->get_type_code());
		  প্রতিজ্ঞা(0);
		  ভঙ্গ;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      বাবে (citer = scope->classes.begin();
           citer != scope->classes.end(); ++ citer ) {
	    class_list_গুনতি += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_গুনতি*পরিমাণ(class_type **)));
	    class_list[class_list_গুনতি-1] = citer->second;
      }
}

শূন্য মূল_table_delete(শূন্য)
{
      বাবে (unsigned idx = 0; idx < vpip_মূল_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_মূল_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    delete scope;
      }
      vpip_মূল_table.clear();

	/* Clean up all the class definitions. */
      বাবে (unsigned idx = 0; idx < class_list_গুনতি; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      মুক্ত(class_list);
      class_list = 0;
      class_list_গুনতি = 0;
}
#endif

স্থির শূন্য construct_scope_fullname(__vpiScope*ref, ৱর্ণ*buf)
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

স্থির ধ্রুৱ ৱর্ণ* scope_get_type(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
          পক্ষ vpiModule:
            ঘূৰাই "vpiModule";
          পক্ষ vpiGenScope:
            ঘূৰাই "vpiGenScope";
          পক্ষ vpiFunction:
            ঘূৰাই "vpiFunction";
          পক্ষ vpiTask:
            ঘূৰাই "vpiTask";
          পক্ষ vpiNamedBegin:
            ঘূৰাই "vpiNamedBegin";
          পক্ষ vpiNamedFork:
            ঘূৰাই "vpiNamedFork";
          স্ৱভাৱ:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            ঘূৰাই রিক্ত;
      }
}

স্থির ৱর্ণ* scope_get_str(পূর্ণাঙ্ক code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      প্রতিজ্ঞা(ref);

      ৱর্ণ buf[4096];  // XXX is a fixed buffer size really reliable?
      ধ্রুৱ ৱর্ণ *p=0;
      ৱিকল্প (code) {
	  পক্ষ vpiDefFile:
	    p = file_names[ref->def_file_idx];
	    ভঙ্গ;

	  পক্ষ vpiFile:
	    p = file_names[ref->file_idx];
	    ভঙ্গ;

	  পক্ষ vpiFullName:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    ভঙ্গ;

	  পক্ষ vpiName:
	    p = ref->scope_name();
	    ভঙ্গ;

	  পক্ষ vpiDefName:
	    p = ref->scope_def_name();
	    ভঙ্গ;

	  পক্ষ vpiType:
	    p = scope_get_type(code);
	    ভঙ্গ;

	  স্ৱভাৱ:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    ঘূৰাই রিক্ত;
      }
      ঘূৰাই simple_set_rbuf_str(p);
}

স্থির vpiHandle scope_get_handle(পূর্ণাঙ্ক code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      প্রতিজ্ঞা(rfp);

      ৱিকল্প (code) {

	  পক্ষ vpiScope:
	    ঘূৰাই rfp->scope;

	  পক্ষ vpiModule:
	    ঘূৰাই rfp->scope;
      }

      ঘূৰাই 0;
}

/* compares vpiType's considering object classes */
স্থির পূর্ণাঙ্ক compare_types(পূর্ণাঙ্ক code, পূর্ণাঙ্ক type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      যদি (code == vpiScope)
	    ঘূৰাই 1;

      যদি (code == type)
	    ঘূৰাই 1;

      যদি ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    ঘূৰাই 1;

      যদি ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    ঘূৰাই 1;

      যদি ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    ঘূৰাই 1;

      ঘূৰাই 0;
}

স্থির vpiHandle make_subset_iterator_(পূর্ণাঙ্ক type_code, ধ্রুৱ vector<vpiHandle>&table)
{
      unsigned mcnt = 0, ncnt = 0;
      vpiHandle*args;

      বাবে (unsigned idx = 0; idx < table.size(); idx += 1)
	    যদি (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      যদি (mcnt == 0)
	    ঘূৰাই 0;

      args = static_cast<vpiHandle*>(calloc(mcnt, পরিমাণ(vpiHandle)));
      বাবে (unsigned idx = 0; idx < table.size(); idx += 1)
	    যদি (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      প্রতিজ্ঞা(ncnt == mcnt);

      ঘূৰাই vpip_make_iterator(mcnt, args, সঁচা);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
স্থির vpiHandle module_iter(পূর্ণাঙ্ক code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      প্রতিজ্ঞা(ref);

      ঘূৰাই make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(ধ্রুৱ ৱর্ণ*nam, ধ্রুৱ ৱর্ণ*tnam, তার্কিক auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

পূর্ণাঙ্ক __vpiScope::vpi_get(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
	  পক্ষ vpiCellInstance:
	    ঘূৰাই is_cell? 1 : 0;

	  পক্ষ vpi_Def_সাৰি_No:
	    ঘূৰাই def_lineno;

	  পক্ষ vpi_সাৰি_No:
	    ঘূৰাই lineno;

	  পক্ষ vpiTimeUnit:
	    ঘূৰাই time_units;

	  পক্ষ vpiTimePrecision:
	    ঘূৰাই time_precision;

	  পক্ষ vpi_শীৰ্ষ_Module:
	    ঘূৰাই 0x0 == scope;

          পক্ষ vpiAutomatic:
	    ঘূৰাই is_automatic_? 1 : 0;
      }

      ঘূৰাই vpiUndefined;
}


ৱর্ণ*__vpiScope::vpi_get_str(পূর্ণাঙ্ক code)
{ ঘূৰাই scope_get_str(code, এতত্); }

vpiHandle __vpiScope::vpi_handle(পূর্ণাঙ্ক code)
{ ঘূৰাই scope_get_handle(code, এতত্); }

vpiHandle __vpiScope::vpi_iterate(পূর্ণাঙ্ক code)
{ ঘূৰাই module_iter(code, এতত্); }


শ্ৰেণী vpiScopeModule  : সার্ৱজনিক __vpiScope {
    সার্ৱজনিক:
      অন্তঃস্থ vpiScopeModule(ধ্রুৱ ৱর্ণ*nam, ধ্রুৱ ৱর্ণ*tnam)
      : __vpiScope(nam,tnam,মিছা) { }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override { ঘূৰাই vpiModule; }
};

সংরচনা vpiScopePackage  : সার্ৱজনিক __vpiScope {
      অন্তঃস্থ vpiScopePackage(ধ্রুৱ ৱর্ণ*nam, ধ্রুৱ ৱর্ণ*tnam)
      : __vpiScope(nam,tnam) { }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override { ঘূৰাই vpiPackage; }
};

সংরচনা vpiScopeTask  : সার্ৱজনিক __vpiScope {
      অন্তঃস্থ vpiScopeTask(ধ্রুৱ ৱর্ণ*nam, ধ্রুৱ ৱর্ণ*tnam)
      : __vpiScope(nam,tnam) { }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override { ঘূৰাই vpiTask; }
};

সংরচনা vpiScopeTaskAuto  : সার্ৱজনিক __vpiScope {
      অন্তঃস্থ vpiScopeTaskAuto(ধ্রুৱ ৱর্ণ*nam, ধ্রুৱ ৱর্ণ*tnam)
      : __vpiScope(nam,tnam,সঁচা) {  }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override { ঘূৰাই vpiTask; }
};

সংরচনা vpiScopeBegin  : সার্ৱজনিক __vpiScope {
      অন্তঃস্থ vpiScopeBegin(ধ্রুৱ ৱর্ণ*nam, ধ্রুৱ ৱর্ণ*tnam)
      : __vpiScope(nam,tnam,মিছা) { }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override { ঘূৰাই vpiNamedBegin; }
};

শ্ৰেণী vpiScopeBeginAuto  : সার্ৱজনিক __vpiScope {
    সার্ৱজনিক:
      অন্তঃস্থ vpiScopeBeginAuto(ধ্রুৱ ৱর্ণ*nam, ধ্রুৱ ৱর্ণ*tnam)
      : __vpiScope(nam,tnam,সঁচা) {  }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override { ঘূৰাই vpiNamedBegin; }
};

সংরচনা vpiScopeGenerate  : সার্ৱজনিক __vpiScope {
      অন্তঃস্থ vpiScopeGenerate(ধ্রুৱ ৱর্ণ*nam, ধ্রুৱ ৱর্ণ*tnam)
      : __vpiScope(nam,tnam) { }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override { ঘূৰাই vpiGenScope; }
};

সংরচনা vpiScopeFork  : সার্ৱজনিক __vpiScope {
      অন্তঃস্থ vpiScopeFork(ধ্রুৱ ৱর্ণ*nam, ধ্রুৱ ৱর্ণ*tnam)
      : __vpiScope(nam,tnam,মিছা) { }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override { ঘূৰাই vpiNamedFork; }
};

শ্ৰেণী vpiScopeForkAuto  : সার্ৱজনিক __vpiScope {
    সার্ৱজনিক:
      অন্তঃস্থ vpiScopeForkAuto(ধ্রুৱ ৱর্ণ*nam, ধ্রুৱ ৱর্ণ*tnam)
      : __vpiScope(nam,tnam,সঁচা) {  }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override { ঘূৰাই vpiNamedFork; }
};

সংরচনা vpiScopeClass  : সার্ৱজনিক __vpiScope {
      অন্তঃস্থ vpiScopeClass(ধ্রুৱ ৱর্ণ*nam, ধ্রুৱ ৱর্ণ*tnam)
      : __vpiScope(nam,tnam) { }
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override { ঘূৰাই vpiClassTypespec; }
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
compile_scope_decl(ৱর্ণ*label, ৱর্ণ*type, ৱর্ণ*name, ৱর্ণ*tname,
                   ৱর্ণ*parent, long file_idx, long lineno,
                   long def_file_idx, long def_lineno, long is_cell)
{
      গুনতি_vpi_scopes += 1;
      ৱর্ণ vec_type;
      ৱর্ণ sign_flag;
      unsigned wid;

      __vpiScope*scope;
      যদি (strcmp(type,"module") == 0) {
	    scope = নূতন vpiScopeModule(name, tname);
      } অন্যথা যদি ( sscanf(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    পূর্ণাঙ্ক type_code;
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
	    scope = নূতন vpiScopeFunction(name, tname, মিছা, type_code, wid, init_val);

      } অন্যথা যদি ( sscanf(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    পূর্ণাঙ্ক type_code;
	    ৱিকল্প (sign_flag) {
		পক্ষ 's':
		  type_code = vpiSizedSignedFunc;
		  ভঙ্গ;
		পক্ষ 'u':
		  type_code = vpiSizedFunc;
		  ভঙ্গ;
		স্ৱভাৱ:
		  প্রতিজ্ঞা(0);
		  type_code = vpiSizedFunc;
		  ভঙ্গ;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = নূতন vpiScopeFunction(name, tname, সঁচা, type_code, wid, init_val);

      } অন্যথা যদি (strcmp(type,"function.obj") == 0) {
	    scope = নূতন vpiScopeFunction(name, tname, মিছা, vpiSizedFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"autofunction.obj") == 0) {
	    scope = নূতন vpiScopeFunction(name, tname, সঁচা, vpiSizedFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"function.real") == 0) {
	    scope = নূতন vpiScopeFunction(name, tname, মিছা, vpiRealFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"autofunction.real") == 0) {
	    scope = নূতন vpiScopeFunction(name, tname, সঁচা, vpiRealFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"function.str") == 0) {
	    scope = নূতন vpiScopeFunction(name, tname, মিছা, vpiOtherFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"autofunction.str") == 0) {
	    scope = নূতন vpiScopeFunction(name, tname, সঁচা, vpiOtherFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"function.void") == 0) {
	    scope = নূতন vpiScopeFunction(name, tname, মিছা, vpiOtherFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"autofunction.void") == 0) {
	    scope = নূতন vpiScopeFunction(name, tname, সঁচা, vpiOtherFunc, 0, BIT4_0);
      } অন্যথা যদি (strcmp(type,"task") == 0) {
	    scope = নূতন vpiScopeTask(name, tname);
      } অন্যথা যদি (strcmp(type,"autotask") == 0) {
	    scope = নূতন vpiScopeTaskAuto(name, tname);
      } অন্যথা যদি (strcmp(type,"fork") == 0) {
	    scope = নূতন vpiScopeFork(name, tname);
      } অন্যথা যদি (strcmp(type,"autofork") == 0) {
	    scope = নূতন vpiScopeForkAuto(name, tname);
      } অন্যথা যদি (strcmp(type,"begin") == 0) {
	    scope = নূতন vpiScopeBegin(name, tname);
      } অন্যথা যদি (strcmp(type,"autobegin") == 0) {
	    scope = নূতন vpiScopeBeginAuto(name, tname);
      } অন্যথা যদি (strcmp(type,"generate") == 0) {
	    scope = নূতন vpiScopeGenerate(name, tname);
      } অন্যথা যদি (strcmp(type,"package") == 0) {
	    scope = নূতন vpiScopePackage(name, tname);
      } অন্যথা যদি (strcmp(type,"class") == 0) {
	    scope = নূতন vpiScopeClass(name, tname);
      } অন্যথা {
	    scope = নূতন vpiScopeModule(name, tname);
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

      যদি (is_cell) scope->is_cell = সঁচা;
      অন্যথা scope->is_cell = মিছা;

      current_scope = scope;

      compile_vpi_প্ৰতীক(label, scope);

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

শূন্য compile_scope_recall(ৱর্ণ*প্ৰতীক)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(শূন্য*)&current_scope, প্ৰতীক);
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
      ঘূৰাই current_scope;
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
      যেতিয়ালৈ (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      ঘূৰাই scope;
}

unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      প্রতিজ্ঞা(scope);
      প্রতিজ্ঞা(scope->is_automatic());

      unsigned idx = scope->nitem++;

      যদি (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (malloc(পরিমাণ(automatic_hooks_s*)));
      অন্যথা
	    scope->item = static_cast<automatic_hooks_s**>
	                  (realloc(scope->item,
	                   পরিমাণ(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      ঘূৰাই 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              unsigned index,
              পূর্ণাঙ্ক vpi_direction,
              unsigned width,
              ধ্রুৱ ৱর্ণ *name,
              ৱর্ণ* buffer) :
      parent_(parent),
      index_(index),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      যদি (buffer != nullptr) functor_ref_lookup(&ref_, buffer);
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

পূর্ণাঙ্ক vpiPortInfo::vpi_get(পূর্ণাঙ্ক code)
{
      ৱিকল্প( code ) {

        পক্ষ vpiDirection :
          ঘূৰাই direction_;
        পক্ষ vpiPortIndex :
          ঘূৰাই index_;
        পক্ষ vpiSize :
          ঘূৰাই width_;
        স্ৱভাৱ :
          ঘূৰাই vpiUndefined;
      }

}


ৱর্ণ *vpiPortInfo::vpi_get_str(পূর্ণাঙ্ক code)
{
      ৱিকল্প( code ) {
        পক্ষ vpiName :
          ঘূৰাই simple_set_rbuf_str(name_);
        স্ৱভাৱ :
          ঘূৰাই রিক্ত;
      }

}


vpiHandle vpiPortInfo::vpi_handle(পূর্ণাঙ্ক code)
{

      ৱিকল্প (code) {

          পক্ষ vpiParent:
          পক্ষ vpiScope:
          পক্ষ vpiModule:
            ঘূৰাই parent_;
          স্ৱভাৱ :
            ভঙ্গ;
      }

      ঘূৰাই 0;
}

স্থির vpiHandle portinfo_iterate(পূর্ণাঙ্ক code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      প্রতিজ্ঞা(rfp);
      unsigned width = rfp->get_width();

      ৱিকল্প (code) {
	  পক্ষ vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(width, পরিমাণ(vpiHandle*)));

	    বাবে (unsigned i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    ঘূৰাই vpip_make_iterator(width, args, সঁচা);
	  }
      }
      ঘূৰাই 0;
}

vpiHandle vpiPortInfo::vpi_iterate(পূর্ণাঙ্ক code)
{
      ঘূৰাই portinfo_iterate(code, এতত্);
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

vpiHandle vpiPortBitInfo::vpi_handle(পূর্ণাঙ্ক code)
{

      ৱিকল্প (code) {
          পক্ষ vpiParent:
            ঘূৰাই parent_;
          স্ৱভাৱ :
            ভঙ্গ;
      }

      ঘূৰাই 0;
}

পূর্ণাঙ্ক vpiPortBitInfo::vpi_get(পূর্ণাঙ্ক code)
{
      ৱিকল্প( code ) {
        পক্ষ vpiBit :
          ঘূৰাই bit_;
        স্ৱভাৱ :
          ভঙ্গ;
      }

      ঘূৰাই 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
শূন্য compile_port_info( unsigned index, পূর্ণাঙ্ক vpi_direction, unsigned width, ধ্রুৱ ৱর্ণ *name, ৱর্ণ* buffer )
{
      vpiPortInfo* obj = নূতন vpiPortInfo( vpip_peek_current_scope(),
                                          index, vpi_direction, width, name, buffer );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      বাবে (unsigned i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = নূতন vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
