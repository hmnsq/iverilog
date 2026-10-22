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

using நாமாகாஶ std;

நிலையான vector<vpiHandle> vpip_வேர்_table;

நிலையான vpiHandle make_subset_iterator_(முழு type_code, மாறிலி vector<vpiHandle>&table);

vpiHandle vpip_make_வேர்_iterator(முழு type_code)
{
      திரும்பு make_subset_iterator_(type_code, vpip_வேர்_table);
}

வெறுமை vpip_make_வேர்_iterator(__vpiHandle**&table, unsigned&ntable)
{
      table = &vpip_வேர்_table[0];
      ntable = vpip_வேர்_table.size();
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை port_delete(__vpiHandle*handle);
வெறுமை port_bit_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
நிலையான class_type **class_list = 0;
நிலையான unsigned class_list_மொத்தம் = 0;

நிலையான வெறுமை delete_sub_scopes(__vpiScope *scope)
{
      க்கு (unsigned idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle item = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(item);
	    தேர்வு(item->get_type_code()) {
		நிலை vpiFunction:
		நிலை vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		நிலை vpiModule:
		நிலை vpiGenScope:
		நிலை vpiNamedBegin:
		நிலை vpiNamedFork:
		நிலை vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  delete item;
		  நிறுத்து;
		நிலை vpiMemory:
		நிலை vpiNetArray:
		  memory_delete(item);
		  நிறுத்து;
		நிலை vpiModPath:
		    /* The destination ModPath is cleaned up later. */
		  delete item;
		  நிறுத்து;
		நிலை vpiInterModPath:
		  delete item;
		  நிறுத்து;
		நிலை vpiNamedEvent:
		  named_event_delete(item);
		  நிறுத்து;
		நிலை vpiNet:
		நிலை vpiReg:
		நிலை vpiIntegerVar:
		நிலை vpiLongIntVar:
		நிலை vpiShortIntVar:
		நிலை vpiIntVar:
		நிலை vpiByteVar:
		நிலை vpiBitVar:
		  signal_delete(item);
		  நிறுத்து;
		நிலை vpiParameter:
		  parameter_delete(item);
		  நிறுத்து;
		நிலை vpiRealVar:
		  real_delete(item);
		  நிறுத்து;
		நிலை vpiEnumTypespec:
		  enum_delete(item);
		  நிறுத்து;
		நிலை vpiPort:
		  port_delete(item);
		  நிறுத்து;
		நிலை vpiPortBit:
		  port_bit_delete(item);
		  நிறுத்து;
		நிலை vpiStringVar:
		  string_delete(item);
		  நிறுத்து;
		நிலை vpiClassVar:
		  class_delete(item);
		  நிறுத்து;
		நிலை vpiArrayVar:
		  array_delete(item);
		  நிறுத்து;
		இயல்பு:
		  fprintf(stderr, "Need support for type: %d\n",
		          item->get_type_code());
		  ப்ரதிஜ்ஞா(0);
		  நிறுத்து;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      க்கு (citer = scope->classes.begin();
           citer != scope->classes.end(); ++ citer ) {
	    class_list_மொத்தம் += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_மொத்தம்*அளவு(class_type **)));
	    class_list[class_list_மொத்தம்-1] = citer->second;
      }
}

வெறுமை வேர்_table_delete(வெறுமை)
{
      க்கு (unsigned idx = 0; idx < vpip_வேர்_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_வேர்_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    delete scope;
      }
      vpip_வேர்_table.clear();

	/* Clean up all the class definitions. */
      க்கு (unsigned idx = 0; idx < class_list_மொத்தம்; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      விடுவி(class_list);
      class_list = 0;
      class_list_மொத்தம் = 0;
}
#endif

நிலையான வெறுமை construct_scope_fullname(__vpiScope*ref, எழுத்து*buf)
{
      எனில் (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    எனில் (ref->scope->get_type_code() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      எனில் (ref->get_type_code() == vpiPackage) strcat(buf, "::");
}

நிலையான மாறிலி எழுத்து* scope_get_type(முழு code)
{
      தேர்வு (code) {
          நிலை vpiModule:
            திரும்பு "vpiModule";
          நிலை vpiGenScope:
            திரும்பு "vpiGenScope";
          நிலை vpiFunction:
            திரும்பு "vpiFunction";
          நிலை vpiTask:
            திரும்பு "vpiTask";
          நிலை vpiNamedBegin:
            திரும்பு "vpiNamedBegin";
          நிலை vpiNamedFork:
            திரும்பு "vpiNamedFork";
          இயல்பு:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            திரும்பு வெற்று;
      }
}

நிலையான எழுத்து* scope_get_str(முழு code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      ப்ரதிஜ்ஞா(ref);

      எழுத்து buf[4096];  // XXX is a fixed buffer size really reliable?
      மாறிலி எழுத்து *p=0;
      தேர்வு (code) {
	  நிலை vpiDefFile:
	    p = file_names[ref->def_file_idx];
	    நிறுத்து;

	  நிலை vpiFile:
	    p = file_names[ref->file_idx];
	    நிறுத்து;

	  நிலை vpiFullName:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    நிறுத்து;

	  நிலை vpiName:
	    p = ref->scope_name();
	    நிறுத்து;

	  நிலை vpiDefName:
	    p = ref->scope_def_name();
	    நிறுத்து;

	  நிலை vpiType:
	    p = scope_get_type(code);
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    திரும்பு வெற்று;
      }
      திரும்பு simple_set_rbuf_str(p);
}

நிலையான vpiHandle scope_get_handle(முழு code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      ப்ரதிஜ்ஞா(rfp);

      தேர்வு (code) {

	  நிலை vpiScope:
	    திரும்பு rfp->scope;

	  நிலை vpiModule:
	    திரும்பு rfp->scope;
      }

      திரும்பு 0;
}

/* compares vpiType's considering object classes */
நிலையான முழு compare_types(முழு code, முழு type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      எனில் (code == vpiScope)
	    திரும்பு 1;

      எனில் (code == type)
	    திரும்பு 1;

      எனில் ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    திரும்பு 1;

      எனில் ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    திரும்பு 1;

      எனில் ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    திரும்பு 1;

      திரும்பு 0;
}

நிலையான vpiHandle make_subset_iterator_(முழு type_code, மாறிலி vector<vpiHandle>&table)
{
      unsigned mcnt = 0, ncnt = 0;
      vpiHandle*args;

      க்கு (unsigned idx = 0; idx < table.size(); idx += 1)
	    எனில் (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      எனில் (mcnt == 0)
	    திரும்பு 0;

      args = static_cast<vpiHandle*>(calloc(mcnt, அளவு(vpiHandle)));
      க்கு (unsigned idx = 0; idx < table.size(); idx += 1)
	    எனில் (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      ப்ரதிஜ்ஞா(ncnt == mcnt);

      திரும்பு vpip_make_iterator(mcnt, args, உண்மை);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
நிலையான vpiHandle module_iter(முழு code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      ப்ரதிஜ்ஞா(ref);

      திரும்பு make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(மாறிலி எழுத்து*nam, மாறிலி எழுத்து*tnam, தருக்க auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

முழு __vpiScope::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiCellInstance:
	    திரும்பு is_cell? 1 : 0;

	  நிலை vpi_Def_வரி_No:
	    திரும்பு def_lineno;

	  நிலை vpi_வரி_No:
	    திரும்பு lineno;

	  நிலை vpiTimeUnit:
	    திரும்பு time_units;

	  நிலை vpiTimePrecision:
	    திரும்பு time_precision;

	  நிலை vpi_உச்சி_Module:
	    திரும்பு 0x0 == scope;

          நிலை vpiAutomatic:
	    திரும்பு is_automatic_? 1 : 0;
      }

      திரும்பு vpiUndefined;
}


எழுத்து*__vpiScope::vpi_get_str(முழு code)
{ திரும்பு scope_get_str(code, இது); }

vpiHandle __vpiScope::vpi_handle(முழு code)
{ திரும்பு scope_get_handle(code, இது); }

vpiHandle __vpiScope::vpi_iterate(முழு code)
{ திரும்பு module_iter(code, இது); }


வகுப்பு vpiScopeModule  : பொது __vpiScope {
    பொது:
      உள்வரி vpiScopeModule(மாறிலி எழுத்து*nam, மாறிலி எழுத்து*tnam)
      : __vpiScope(nam,tnam,பொய்) { }
      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiModule; }
};

கட்டமைப்பு vpiScopePackage  : பொது __vpiScope {
      உள்வரி vpiScopePackage(மாறிலி எழுத்து*nam, மாறிலி எழுத்து*tnam)
      : __vpiScope(nam,tnam) { }
      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiPackage; }
};

கட்டமைப்பு vpiScopeTask  : பொது __vpiScope {
      உள்வரி vpiScopeTask(மாறிலி எழுத்து*nam, மாறிலி எழுத்து*tnam)
      : __vpiScope(nam,tnam) { }
      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiTask; }
};

கட்டமைப்பு vpiScopeTaskAuto  : பொது __vpiScope {
      உள்வரி vpiScopeTaskAuto(மாறிலி எழுத்து*nam, மாறிலி எழுத்து*tnam)
      : __vpiScope(nam,tnam,உண்மை) {  }
      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiTask; }
};

கட்டமைப்பு vpiScopeBegin  : பொது __vpiScope {
      உள்வரி vpiScopeBegin(மாறிலி எழுத்து*nam, மாறிலி எழுத்து*tnam)
      : __vpiScope(nam,tnam,பொய்) { }
      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiNamedBegin; }
};

வகுப்பு vpiScopeBeginAuto  : பொது __vpiScope {
    பொது:
      உள்வரி vpiScopeBeginAuto(மாறிலி எழுத்து*nam, மாறிலி எழுத்து*tnam)
      : __vpiScope(nam,tnam,உண்மை) {  }
      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiNamedBegin; }
};

கட்டமைப்பு vpiScopeGenerate  : பொது __vpiScope {
      உள்வரி vpiScopeGenerate(மாறிலி எழுத்து*nam, மாறிலி எழுத்து*tnam)
      : __vpiScope(nam,tnam) { }
      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiGenScope; }
};

கட்டமைப்பு vpiScopeFork  : பொது __vpiScope {
      உள்வரி vpiScopeFork(மாறிலி எழுத்து*nam, மாறிலி எழுத்து*tnam)
      : __vpiScope(nam,tnam,பொய்) { }
      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiNamedFork; }
};

வகுப்பு vpiScopeForkAuto  : பொது __vpiScope {
    பொது:
      உள்வரி vpiScopeForkAuto(மாறிலி எழுத்து*nam, மாறிலி எழுத்து*tnam)
      : __vpiScope(nam,tnam,உண்மை) {  }
      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiNamedFork; }
};

கட்டமைப்பு vpiScopeClass  : பொது __vpiScope {
      உள்வரி vpiScopeClass(மாறிலி எழுத்து*nam, மாறிலி எழுத்து*tnam)
      : __vpiScope(nam,tnam) { }
      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
நிலையான __vpiScope*current_scope = 0;

வெறுமை vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj)
{
      ப்ரதிஜ்ஞா(scope);
      scope->intern.push_back(obj);
}

/*
 * When the compiler encounters a scope declaration, this function
 * creates and initializes a __vpiScope object with the requested name
 * and within the addressed parent. The label is used as a key in the
 * symbol table and the name is used to construct the actual object.
 */
வெறுமை
compile_scope_decl(எழுத்து*label, எழுத்து*type, எழுத்து*name, எழுத்து*tname,
                   எழுத்து*parent, long file_idx, long lineno,
                   long def_file_idx, long def_lineno, long is_cell)
{
      மொத்தம்_vpi_scopes += 1;
      எழுத்து vec_type;
      எழுத்து sign_flag;
      unsigned wid;

      __vpiScope*scope;
      எனில் (strcmp(type,"module") == 0) {
	    scope = புதிய vpiScopeModule(name, tname);
      } இல்லையெனில் எனில் ( sscanf(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    முழு type_code;
	    எனில் (sign_flag=='s') {
		  type_code = vpiSizedSignedFunc;
	    } இல்லையெனில் எனில் (sign_flag=='u') {
		  type_code = vpiSizedFunc;
	    } இல்லையெனில் எனில் (sign_flag=='i') {
		  type_code = vpiIntFunc;
	    } இல்லையெனில் {
		  ப்ரதிஜ்ஞா(0);
		  type_code = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = புதிய vpiScopeFunction(name, tname, பொய், type_code, wid, init_val);

      } இல்லையெனில் எனில் ( sscanf(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    முழு type_code;
	    தேர்வு (sign_flag) {
		நிலை 's':
		  type_code = vpiSizedSignedFunc;
		  நிறுத்து;
		நிலை 'u':
		  type_code = vpiSizedFunc;
		  நிறுத்து;
		இயல்பு:
		  ப்ரதிஜ்ஞா(0);
		  type_code = vpiSizedFunc;
		  நிறுத்து;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = புதிய vpiScopeFunction(name, tname, உண்மை, type_code, wid, init_val);

      } இல்லையெனில் எனில் (strcmp(type,"function.obj") == 0) {
	    scope = புதிய vpiScopeFunction(name, tname, பொய், vpiSizedFunc, 0, BIT4_0);
      } இல்லையெனில் எனில் (strcmp(type,"autofunction.obj") == 0) {
	    scope = புதிய vpiScopeFunction(name, tname, உண்மை, vpiSizedFunc, 0, BIT4_0);
      } இல்லையெனில் எனில் (strcmp(type,"function.real") == 0) {
	    scope = புதிய vpiScopeFunction(name, tname, பொய், vpiRealFunc, 0, BIT4_0);
      } இல்லையெனில் எனில் (strcmp(type,"autofunction.real") == 0) {
	    scope = புதிய vpiScopeFunction(name, tname, உண்மை, vpiRealFunc, 0, BIT4_0);
      } இல்லையெனில் எனில் (strcmp(type,"function.str") == 0) {
	    scope = புதிய vpiScopeFunction(name, tname, பொய், vpiOtherFunc, 0, BIT4_0);
      } இல்லையெனில் எனில் (strcmp(type,"autofunction.str") == 0) {
	    scope = புதிய vpiScopeFunction(name, tname, உண்மை, vpiOtherFunc, 0, BIT4_0);
      } இல்லையெனில் எனில் (strcmp(type,"function.void") == 0) {
	    scope = புதிய vpiScopeFunction(name, tname, பொய், vpiOtherFunc, 0, BIT4_0);
      } இல்லையெனில் எனில் (strcmp(type,"autofunction.void") == 0) {
	    scope = புதிய vpiScopeFunction(name, tname, உண்மை, vpiOtherFunc, 0, BIT4_0);
      } இல்லையெனில் எனில் (strcmp(type,"task") == 0) {
	    scope = புதிய vpiScopeTask(name, tname);
      } இல்லையெனில் எனில் (strcmp(type,"autotask") == 0) {
	    scope = புதிய vpiScopeTaskAuto(name, tname);
      } இல்லையெனில் எனில் (strcmp(type,"fork") == 0) {
	    scope = புதிய vpiScopeFork(name, tname);
      } இல்லையெனில் எனில் (strcmp(type,"autofork") == 0) {
	    scope = புதிய vpiScopeForkAuto(name, tname);
      } இல்லையெனில் எனில் (strcmp(type,"begin") == 0) {
	    scope = புதிய vpiScopeBegin(name, tname);
      } இல்லையெனில் எனில் (strcmp(type,"autobegin") == 0) {
	    scope = புதிய vpiScopeBeginAuto(name, tname);
      } இல்லையெனில் எனில் (strcmp(type,"generate") == 0) {
	    scope = புதிய vpiScopeGenerate(name, tname);
      } இல்லையெனில் எனில் (strcmp(type,"package") == 0) {
	    scope = புதிய vpiScopePackage(name, tname);
      } இல்லையெனில் எனில் (strcmp(type,"class") == 0) {
	    scope = புதிய vpiScopeClass(name, tname);
      } இல்லையெனில் {
	    scope = புதிய vpiScopeModule(name, tname);
	    ப்ரதிஜ்ஞா(0);
      }

      scope->file_idx = (unsigned) file_idx;
      scope->lineno  = (unsigned) lineno;
      scope->def_file_idx = (unsigned) def_file_idx;
      scope->def_lineno  = (unsigned) def_lineno;
      scope->item = 0;
      scope->nitem = 0;
      scope->live_contexts = 0;
      scope->free_contexts = 0;

      எனில் (is_cell) scope->is_cell = உண்மை;
      இல்லையெனில் scope->is_cell = பொய்;

      current_scope = scope;

      compile_vpi_குறியீடு(label, scope);

      விடுவி(label);
      விடுவி(type);
      delete[] name;
      delete[] tname;

      எனில் (parent) {
	    நிலையான vpiHandle obj;
	    compile_vpi_lookup(&obj, parent);
	    ப்ரதிஜ்ஞா(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_to_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->time_units = sp->time_units;
	    scope->time_precision = sp->time_precision;

      } இல்லையெனில் {
	    scope->scope = 0x0;

	    vpip_வேர்_table.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->time_units = vpip_get_time_precision();
	    scope->time_precision = vpip_get_time_precision();
      }
}

வெறுமை compile_scope_recall(எழுத்து*குறியீடு)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(வெறுமை*)&current_scope, குறியீடு);
      ப்ரதிஜ்ஞா(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
வெறுமை compile_timescale(long units, long precision)
{
      ப்ரதிஜ்ஞா(current_scope);
      current_scope->time_units = units;
      current_scope->time_precision = precision;
}

__vpiScope* vpip_peek_current_scope(வெறுமை)
{
      திரும்பு current_scope;
}

வெறுமை vpip_attach_to_current_scope(vpiHandle obj)
{
      vpip_attach_to_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(வெறுமை)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      போது (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      திரும்பு scope;
}

unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      ப்ரதிஜ்ஞா(scope);
      ப்ரதிஜ்ஞா(scope->is_automatic());

      unsigned idx = scope->nitem++;

      எனில் (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (malloc(அளவு(automatic_hooks_s*)));
      இல்லையெனில்
	    scope->item = static_cast<automatic_hooks_s**>
	                  (realloc(scope->item,
	                   அளவு(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      திரும்பு 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              unsigned index,
              முழு vpi_direction,
              unsigned width,
              மாறிலி எழுத்து *name,
              எழுத்து* buffer) :
      parent_(parent),
      index_(index),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      எனில் (buffer != nullptr) functor_ref_lookup(&ref_, buffer);
      இல்லையெனில் ref_ = nullptr;
}

vpiPortInfo::~vpiPortInfo()
{
      delete[] name_;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை port_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortInfo *>(handle);
}
#endif

முழு vpiPortInfo::vpi_get(முழு code)
{
      தேர்வு( code ) {

        நிலை vpiDirection :
          திரும்பு direction_;
        நிலை vpiPortIndex :
          திரும்பு index_;
        நிலை vpiSize :
          திரும்பு width_;
        இயல்பு :
          திரும்பு vpiUndefined;
      }

}


எழுத்து *vpiPortInfo::vpi_get_str(முழு code)
{
      தேர்வு( code ) {
        நிலை vpiName :
          திரும்பு simple_set_rbuf_str(name_);
        இயல்பு :
          திரும்பு வெற்று;
      }

}


vpiHandle vpiPortInfo::vpi_handle(முழு code)
{

      தேர்வு (code) {

          நிலை vpiParent:
          நிலை vpiScope:
          நிலை vpiModule:
            திரும்பு parent_;
          இயல்பு :
            நிறுத்து;
      }

      திரும்பு 0;
}

நிலையான vpiHandle portinfo_iterate(முழு code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      ப்ரதிஜ்ஞா(rfp);
      unsigned width = rfp->get_width();

      தேர்வு (code) {
	  நிலை vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(width, அளவு(vpiHandle*)));

	    க்கு (unsigned i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    திரும்பு vpip_make_iterator(width, args, உண்மை);
	  }
      }
      திரும்பு 0;
}

vpiHandle vpiPortInfo::vpi_iterate(முழு code)
{
      திரும்பு portinfo_iterate(code, இது);
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
வெறுமை port_bit_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortBitInfo *>(handle);
}
#endif

vpiHandle vpiPortBitInfo::vpi_handle(முழு code)
{

      தேர்வு (code) {
          நிலை vpiParent:
            திரும்பு parent_;
          இயல்பு :
            நிறுத்து;
      }

      திரும்பு 0;
}

முழு vpiPortBitInfo::vpi_get(முழு code)
{
      தேர்வு( code ) {
        நிலை vpiBit :
          திரும்பு bit_;
        இயல்பு :
          நிறுத்து;
      }

      திரும்பு 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
வெறுமை compile_port_info( unsigned index, முழு vpi_direction, unsigned width, மாறிலி எழுத்து *name, எழுத்து* buffer )
{
      vpiPortInfo* obj = புதிய vpiPortInfo( vpip_peek_current_scope(),
                                          index, vpi_direction, width, name, buffer );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      க்கு (unsigned i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = புதிய vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
