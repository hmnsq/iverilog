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

using നാമാകാശ std;

സ്ഥിതം vector<vpiHandle> vpip_വേര്_table;

സ്ഥിതം vpiHandle make_subset_iterator_(പൂർണ്ണസംഖ്യ type_code, സ്ഥിരം vector<vpiHandle>&table);

vpiHandle vpip_make_വേര്_iterator(പൂർണ്ണസംഖ്യ type_code)
{
      തിരികെ make_subset_iterator_(type_code, vpip_വേര്_table);
}

ശൂന്യം vpip_make_വേര്_iterator(__vpiHandle**&table, unsigned&ntable)
{
      table = &vpip_വേര്_table[0];
      ntable = vpip_വേര്_table.size();
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം port_delete(__vpiHandle*handle);
ശൂന്യം port_bit_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
സ്ഥിതം class_type **class_list = 0;
സ്ഥിതം unsigned class_list_എണ്ണം = 0;

സ്ഥിതം ശൂന്യം delete_sub_scopes(__vpiScope *scope)
{
      വേണ്ടി (unsigned idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle item = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(item);
	    തിരഞ്ഞെടുപ്പ്(item->get_type_code()) {
		അവസ്ഥ vpiFunction:
		അവസ്ഥ vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		അവസ്ഥ vpiModule:
		അവസ്ഥ vpiGenScope:
		അവസ്ഥ vpiNamedBegin:
		അവസ്ഥ vpiNamedFork:
		അവസ്ഥ vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  delete item;
		  നിർത്തുക;
		അവസ്ഥ vpiMemory:
		അവസ്ഥ vpiNetArray:
		  memory_delete(item);
		  നിർത്തുക;
		അവസ്ഥ vpiModPath:
		    /* The destination ModPath is cleaned up later. */
		  delete item;
		  നിർത്തുക;
		അവസ്ഥ vpiInterModPath:
		  delete item;
		  നിർത്തുക;
		അവസ്ഥ vpiNamedEvent:
		  named_event_delete(item);
		  നിർത്തുക;
		അവസ്ഥ vpiNet:
		അവസ്ഥ vpiReg:
		അവസ്ഥ vpiIntegerVar:
		അവസ്ഥ vpiLongIntVar:
		അവസ്ഥ vpiShortIntVar:
		അവസ്ഥ vpiIntVar:
		അവസ്ഥ vpiByteVar:
		അവസ്ഥ vpiBitVar:
		  signal_delete(item);
		  നിർത്തുക;
		അവസ്ഥ vpiParameter:
		  parameter_delete(item);
		  നിർത്തുക;
		അവസ്ഥ vpiRealVar:
		  real_delete(item);
		  നിർത്തുക;
		അവസ്ഥ vpiEnumTypespec:
		  enum_delete(item);
		  നിർത്തുക;
		അവസ്ഥ vpiPort:
		  port_delete(item);
		  നിർത്തുക;
		അവസ്ഥ vpiPortBit:
		  port_bit_delete(item);
		  നിർത്തുക;
		അവസ്ഥ vpiStringVar:
		  string_delete(item);
		  നിർത്തുക;
		അവസ്ഥ vpiClassVar:
		  class_delete(item);
		  നിർത്തുക;
		അവസ്ഥ vpiArrayVar:
		  array_delete(item);
		  നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		  fprintf(stderr, "Need support for type: %d\n",
		          item->get_type_code());
		  പ്രതിജ്ഞാ(0);
		  നിർത്തുക;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      വേണ്ടി (citer = scope->classes.begin();
           citer != scope->classes.end(); ++ citer ) {
	    class_list_എണ്ണം += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_എണ്ണം*വലുപ്പം(class_type **)));
	    class_list[class_list_എണ്ണം-1] = citer->second;
      }
}

ശൂന്യം വേര്_table_delete(ശൂന്യം)
{
      വേണ്ടി (unsigned idx = 0; idx < vpip_വേര്_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_വേര്_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    delete scope;
      }
      vpip_വേര്_table.clear();

	/* Clean up all the class definitions. */
      വേണ്ടി (unsigned idx = 0; idx < class_list_എണ്ണം; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      സ്വതന്ത്രം(class_list);
      class_list = 0;
      class_list_എണ്ണം = 0;
}
#endif

സ്ഥിതം ശൂന്യം construct_scope_fullname(__vpiScope*ref, അക്ഷരം*buf)
{
      എങ്കിൽ (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    എങ്കിൽ (ref->scope->get_type_code() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      എങ്കിൽ (ref->get_type_code() == vpiPackage) strcat(buf, "::");
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* scope_get_type(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
          അവസ്ഥ vpiModule:
            തിരികെ "vpiModule";
          അവസ്ഥ vpiGenScope:
            തിരികെ "vpiGenScope";
          അവസ്ഥ vpiFunction:
            തിരികെ "vpiFunction";
          അവസ്ഥ vpiTask:
            തിരികെ "vpiTask";
          അവസ്ഥ vpiNamedBegin:
            തിരികെ "vpiNamedBegin";
          അവസ്ഥ vpiNamedFork:
            തിരികെ "vpiNamedFork";
          സ്ഥിരസ്ഥിതി:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            തിരികെ ശൂന്യ;
      }
}

സ്ഥിതം അക്ഷരം* scope_get_str(പൂർണ്ണസംഖ്യ code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      പ്രതിജ്ഞാ(ref);

      അക്ഷരം buf[4096];  // XXX is a fixed buffer size really reliable?
      സ്ഥിരം അക്ഷരം *p=0;
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiDefFile:
	    p = file_names[ref->def_file_idx];
	    നിർത്തുക;

	  അവസ്ഥ vpiFile:
	    p = file_names[ref->file_idx];
	    നിർത്തുക;

	  അവസ്ഥ vpiFullName:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    നിർത്തുക;

	  അവസ്ഥ vpiName:
	    p = ref->scope_name();
	    നിർത്തുക;

	  അവസ്ഥ vpiDefName:
	    p = ref->scope_def_name();
	    നിർത്തുക;

	  അവസ്ഥ vpiType:
	    p = scope_get_type(code);
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    തിരികെ ശൂന്യ;
      }
      തിരികെ simple_set_rbuf_str(p);
}

സ്ഥിതം vpiHandle scope_get_handle(പൂർണ്ണസംഖ്യ code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      പ്രതിജ്ഞാ(rfp);

      തിരഞ്ഞെടുപ്പ് (code) {

	  അവസ്ഥ vpiScope:
	    തിരികെ rfp->scope;

	  അവസ്ഥ vpiModule:
	    തിരികെ rfp->scope;
      }

      തിരികെ 0;
}

/* compares vpiType's considering object classes */
സ്ഥിതം പൂർണ്ണസംഖ്യ compare_types(പൂർണ്ണസംഖ്യ code, പൂർണ്ണസംഖ്യ type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      എങ്കിൽ (code == vpiScope)
	    തിരികെ 1;

      എങ്കിൽ (code == type)
	    തിരികെ 1;

      എങ്കിൽ ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    തിരികെ 1;

      എങ്കിൽ ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    തിരികെ 1;

      എങ്കിൽ ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    തിരികെ 1;

      തിരികെ 0;
}

സ്ഥിതം vpiHandle make_subset_iterator_(പൂർണ്ണസംഖ്യ type_code, സ്ഥിരം vector<vpiHandle>&table)
{
      unsigned mcnt = 0, ncnt = 0;
      vpiHandle*args;

      വേണ്ടി (unsigned idx = 0; idx < table.size(); idx += 1)
	    എങ്കിൽ (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      എങ്കിൽ (mcnt == 0)
	    തിരികെ 0;

      args = static_cast<vpiHandle*>(calloc(mcnt, വലുപ്പം(vpiHandle)));
      വേണ്ടി (unsigned idx = 0; idx < table.size(); idx += 1)
	    എങ്കിൽ (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      പ്രതിജ്ഞാ(ncnt == mcnt);

      തിരികെ vpip_make_iterator(mcnt, args, സത്യം);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
സ്ഥിതം vpiHandle module_iter(പൂർണ്ണസംഖ്യ code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      പ്രതിജ്ഞാ(ref);

      തിരികെ make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(സ്ഥിരം അക്ഷരം*nam, സ്ഥിരം അക്ഷരം*tnam, തർക്കം auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

പൂർണ്ണസംഖ്യ __vpiScope::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiCellInstance:
	    തിരികെ is_cell? 1 : 0;

	  അവസ്ഥ vpi_Def_വരി_No:
	    തിരികെ def_lineno;

	  അവസ്ഥ vpi_വരി_No:
	    തിരികെ lineno;

	  അവസ്ഥ vpiTimeUnit:
	    തിരികെ time_units;

	  അവസ്ഥ vpiTimePrecision:
	    തിരികെ time_precision;

	  അവസ്ഥ vpi_മുകൾ_Module:
	    തിരികെ 0x0 == scope;

          അവസ്ഥ vpiAutomatic:
	    തിരികെ is_automatic_? 1 : 0;
      }

      തിരികെ vpiUndefined;
}


അക്ഷരം*__vpiScope::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{ തിരികെ scope_get_str(code, ഇത്); }

vpiHandle __vpiScope::vpi_handle(പൂർണ്ണസംഖ്യ code)
{ തിരികെ scope_get_handle(code, ഇത്); }

vpiHandle __vpiScope::vpi_iterate(പൂർണ്ണസംഖ്യ code)
{ തിരികെ module_iter(code, ഇത്); }


വർഗ്ഗം vpiScopeModule  : പൊതു __vpiScope {
    പൊതു:
      വരിയിൽ vpiScopeModule(സ്ഥിരം അക്ഷരം*nam, സ്ഥിരം അക്ഷരം*tnam)
      : __vpiScope(nam,tnam,അസത്യം) { }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiModule; }
};

ഘടന vpiScopePackage  : പൊതു __vpiScope {
      വരിയിൽ vpiScopePackage(സ്ഥിരം അക്ഷരം*nam, സ്ഥിരം അക്ഷരം*tnam)
      : __vpiScope(nam,tnam) { }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiPackage; }
};

ഘടന vpiScopeTask  : പൊതു __vpiScope {
      വരിയിൽ vpiScopeTask(സ്ഥിരം അക്ഷരം*nam, സ്ഥിരം അക്ഷരം*tnam)
      : __vpiScope(nam,tnam) { }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiTask; }
};

ഘടന vpiScopeTaskAuto  : പൊതു __vpiScope {
      വരിയിൽ vpiScopeTaskAuto(സ്ഥിരം അക്ഷരം*nam, സ്ഥിരം അക്ഷരം*tnam)
      : __vpiScope(nam,tnam,സത്യം) {  }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiTask; }
};

ഘടന vpiScopeBegin  : പൊതു __vpiScope {
      വരിയിൽ vpiScopeBegin(സ്ഥിരം അക്ഷരം*nam, സ്ഥിരം അക്ഷരം*tnam)
      : __vpiScope(nam,tnam,അസത്യം) { }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiNamedBegin; }
};

വർഗ്ഗം vpiScopeBeginAuto  : പൊതു __vpiScope {
    പൊതു:
      വരിയിൽ vpiScopeBeginAuto(സ്ഥിരം അക്ഷരം*nam, സ്ഥിരം അക്ഷരം*tnam)
      : __vpiScope(nam,tnam,സത്യം) {  }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiNamedBegin; }
};

ഘടന vpiScopeGenerate  : പൊതു __vpiScope {
      വരിയിൽ vpiScopeGenerate(സ്ഥിരം അക്ഷരം*nam, സ്ഥിരം അക്ഷരം*tnam)
      : __vpiScope(nam,tnam) { }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiGenScope; }
};

ഘടന vpiScopeFork  : പൊതു __vpiScope {
      വരിയിൽ vpiScopeFork(സ്ഥിരം അക്ഷരം*nam, സ്ഥിരം അക്ഷരം*tnam)
      : __vpiScope(nam,tnam,അസത്യം) { }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiNamedFork; }
};

വർഗ്ഗം vpiScopeForkAuto  : പൊതു __vpiScope {
    പൊതു:
      വരിയിൽ vpiScopeForkAuto(സ്ഥിരം അക്ഷരം*nam, സ്ഥിരം അക്ഷരം*tnam)
      : __vpiScope(nam,tnam,സത്യം) {  }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiNamedFork; }
};

ഘടന vpiScopeClass  : പൊതു __vpiScope {
      വരിയിൽ vpiScopeClass(സ്ഥിരം അക്ഷരം*nam, സ്ഥിരം അക്ഷരം*tnam)
      : __vpiScope(nam,tnam) { }
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
സ്ഥിതം __vpiScope*current_scope = 0;

ശൂന്യം vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj)
{
      പ്രതിജ്ഞാ(scope);
      scope->intern.push_back(obj);
}

/*
 * When the compiler encounters a scope declaration, this function
 * creates and initializes a __vpiScope object with the requested name
 * and within the addressed parent. The label is used as a key in the
 * symbol table and the name is used to construct the actual object.
 */
ശൂന്യം
compile_scope_decl(അക്ഷരം*label, അക്ഷരം*type, അക്ഷരം*name, അക്ഷരം*tname,
                   അക്ഷരം*parent, long file_idx, long lineno,
                   long def_file_idx, long def_lineno, long is_cell)
{
      എണ്ണം_vpi_scopes += 1;
      അക്ഷരം vec_type;
      അക്ഷരം sign_flag;
      unsigned wid;

      __vpiScope*scope;
      എങ്കിൽ (strcmp(type,"module") == 0) {
	    scope = പുതിയ vpiScopeModule(name, tname);
      } അല്ലെങ്കിൽ എങ്കിൽ ( sscanf(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    പൂർണ്ണസംഖ്യ type_code;
	    എങ്കിൽ (sign_flag=='s') {
		  type_code = vpiSizedSignedFunc;
	    } അല്ലെങ്കിൽ എങ്കിൽ (sign_flag=='u') {
		  type_code = vpiSizedFunc;
	    } അല്ലെങ്കിൽ എങ്കിൽ (sign_flag=='i') {
		  type_code = vpiIntFunc;
	    } അല്ലെങ്കിൽ {
		  പ്രതിജ്ഞാ(0);
		  type_code = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = പുതിയ vpiScopeFunction(name, tname, അസത്യം, type_code, wid, init_val);

      } അല്ലെങ്കിൽ എങ്കിൽ ( sscanf(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    പൂർണ്ണസംഖ്യ type_code;
	    തിരഞ്ഞെടുപ്പ് (sign_flag) {
		അവസ്ഥ 's':
		  type_code = vpiSizedSignedFunc;
		  നിർത്തുക;
		അവസ്ഥ 'u':
		  type_code = vpiSizedFunc;
		  നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		  പ്രതിജ്ഞാ(0);
		  type_code = vpiSizedFunc;
		  നിർത്തുക;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = പുതിയ vpiScopeFunction(name, tname, സത്യം, type_code, wid, init_val);

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"function.obj") == 0) {
	    scope = പുതിയ vpiScopeFunction(name, tname, അസത്യം, vpiSizedFunc, 0, BIT4_0);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"autofunction.obj") == 0) {
	    scope = പുതിയ vpiScopeFunction(name, tname, സത്യം, vpiSizedFunc, 0, BIT4_0);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"function.real") == 0) {
	    scope = പുതിയ vpiScopeFunction(name, tname, അസത്യം, vpiRealFunc, 0, BIT4_0);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"autofunction.real") == 0) {
	    scope = പുതിയ vpiScopeFunction(name, tname, സത്യം, vpiRealFunc, 0, BIT4_0);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"function.str") == 0) {
	    scope = പുതിയ vpiScopeFunction(name, tname, അസത്യം, vpiOtherFunc, 0, BIT4_0);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"autofunction.str") == 0) {
	    scope = പുതിയ vpiScopeFunction(name, tname, സത്യം, vpiOtherFunc, 0, BIT4_0);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"function.void") == 0) {
	    scope = പുതിയ vpiScopeFunction(name, tname, അസത്യം, vpiOtherFunc, 0, BIT4_0);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"autofunction.void") == 0) {
	    scope = പുതിയ vpiScopeFunction(name, tname, സത്യം, vpiOtherFunc, 0, BIT4_0);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"task") == 0) {
	    scope = പുതിയ vpiScopeTask(name, tname);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"autotask") == 0) {
	    scope = പുതിയ vpiScopeTaskAuto(name, tname);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"fork") == 0) {
	    scope = പുതിയ vpiScopeFork(name, tname);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"autofork") == 0) {
	    scope = പുതിയ vpiScopeForkAuto(name, tname);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"begin") == 0) {
	    scope = പുതിയ vpiScopeBegin(name, tname);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"autobegin") == 0) {
	    scope = പുതിയ vpiScopeBeginAuto(name, tname);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"generate") == 0) {
	    scope = പുതിയ vpiScopeGenerate(name, tname);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"package") == 0) {
	    scope = പുതിയ vpiScopePackage(name, tname);
      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type,"class") == 0) {
	    scope = പുതിയ vpiScopeClass(name, tname);
      } അല്ലെങ്കിൽ {
	    scope = പുതിയ vpiScopeModule(name, tname);
	    പ്രതിജ്ഞാ(0);
      }

      scope->file_idx = (unsigned) file_idx;
      scope->lineno  = (unsigned) lineno;
      scope->def_file_idx = (unsigned) def_file_idx;
      scope->def_lineno  = (unsigned) def_lineno;
      scope->item = 0;
      scope->nitem = 0;
      scope->live_contexts = 0;
      scope->free_contexts = 0;

      എങ്കിൽ (is_cell) scope->is_cell = സത്യം;
      അല്ലെങ്കിൽ scope->is_cell = അസത്യം;

      current_scope = scope;

      compile_vpi_ചിഹ്നം(label, scope);

      സ്വതന്ത്രം(label);
      സ്വതന്ത്രം(type);
      delete[] name;
      delete[] tname;

      എങ്കിൽ (parent) {
	    സ്ഥിതം vpiHandle obj;
	    compile_vpi_lookup(&obj, parent);
	    പ്രതിജ്ഞാ(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_to_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->time_units = sp->time_units;
	    scope->time_precision = sp->time_precision;

      } അല്ലെങ്കിൽ {
	    scope->scope = 0x0;

	    vpip_വേര്_table.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->time_units = vpip_get_time_precision();
	    scope->time_precision = vpip_get_time_precision();
      }
}

ശൂന്യം compile_scope_recall(അക്ഷരം*ചിഹ്നം)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(ശൂന്യം*)&current_scope, ചിഹ്നം);
      പ്രതിജ്ഞാ(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
ശൂന്യം compile_timescale(long units, long precision)
{
      പ്രതിജ്ഞാ(current_scope);
      current_scope->time_units = units;
      current_scope->time_precision = precision;
}

__vpiScope* vpip_peek_current_scope(ശൂന്യം)
{
      തിരികെ current_scope;
}

ശൂന്യം vpip_attach_to_current_scope(vpiHandle obj)
{
      vpip_attach_to_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(ശൂന്യം)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      ആകുമ്പോൾ (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      തിരികെ scope;
}

unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      പ്രതിജ്ഞാ(scope);
      പ്രതിജ്ഞാ(scope->is_automatic());

      unsigned idx = scope->nitem++;

      എങ്കിൽ (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (malloc(വലുപ്പം(automatic_hooks_s*)));
      അല്ലെങ്കിൽ
	    scope->item = static_cast<automatic_hooks_s**>
	                  (realloc(scope->item,
	                   വലുപ്പം(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      തിരികെ 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              unsigned index,
              പൂർണ്ണസംഖ്യ vpi_direction,
              unsigned width,
              സ്ഥിരം അക്ഷരം *name,
              അക്ഷരം* buffer) :
      parent_(parent),
      index_(index),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      എങ്കിൽ (buffer != nullptr) functor_ref_lookup(&ref_, buffer);
      അല്ലെങ്കിൽ ref_ = nullptr;
}

vpiPortInfo::~vpiPortInfo()
{
      delete[] name_;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം port_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortInfo *>(handle);
}
#endif

പൂർണ്ണസംഖ്യ vpiPortInfo::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ്( code ) {

        അവസ്ഥ vpiDirection :
          തിരികെ direction_;
        അവസ്ഥ vpiPortIndex :
          തിരികെ index_;
        അവസ്ഥ vpiSize :
          തിരികെ width_;
        സ്ഥിരസ്ഥിതി :
          തിരികെ vpiUndefined;
      }

}


അക്ഷരം *vpiPortInfo::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ്( code ) {
        അവസ്ഥ vpiName :
          തിരികെ simple_set_rbuf_str(name_);
        സ്ഥിരസ്ഥിതി :
          തിരികെ ശൂന്യ;
      }

}


vpiHandle vpiPortInfo::vpi_handle(പൂർണ്ണസംഖ്യ code)
{

      തിരഞ്ഞെടുപ്പ് (code) {

          അവസ്ഥ vpiParent:
          അവസ്ഥ vpiScope:
          അവസ്ഥ vpiModule:
            തിരികെ parent_;
          സ്ഥിരസ്ഥിതി :
            നിർത്തുക;
      }

      തിരികെ 0;
}

സ്ഥിതം vpiHandle portinfo_iterate(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      പ്രതിജ്ഞാ(rfp);
      unsigned width = rfp->get_width();

      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(width, വലുപ്പം(vpiHandle*)));

	    വേണ്ടി (unsigned i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    തിരികെ vpip_make_iterator(width, args, സത്യം);
	  }
      }
      തിരികെ 0;
}

vpiHandle vpiPortInfo::vpi_iterate(പൂർണ്ണസംഖ്യ code)
{
      തിരികെ portinfo_iterate(code, ഇത്);
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
ശൂന്യം port_bit_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortBitInfo *>(handle);
}
#endif

vpiHandle vpiPortBitInfo::vpi_handle(പൂർണ്ണസംഖ്യ code)
{

      തിരഞ്ഞെടുപ്പ് (code) {
          അവസ്ഥ vpiParent:
            തിരികെ parent_;
          സ്ഥിരസ്ഥിതി :
            നിർത്തുക;
      }

      തിരികെ 0;
}

പൂർണ്ണസംഖ്യ vpiPortBitInfo::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ്( code ) {
        അവസ്ഥ vpiBit :
          തിരികെ bit_;
        സ്ഥിരസ്ഥിതി :
          നിർത്തുക;
      }

      തിരികെ 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
ശൂന്യം compile_port_info( unsigned index, പൂർണ്ണസംഖ്യ vpi_direction, unsigned width, സ്ഥിരം അക്ഷരം *name, അക്ഷരം* buffer )
{
      vpiPortInfo* obj = പുതിയ vpiPortInfo( vpip_peek_current_scope(),
                                          index, vpi_direction, width, name, buffer );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      വേണ്ടി (unsigned i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = പുതിയ vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
