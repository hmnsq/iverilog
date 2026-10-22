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

using નામાકાશ std;

સ્થિર vector<vpiHandle> vpip_મૂળ_table;

સ્થિર vpiHandle make_subset_iterator_(પૂર્ણાંક type_code, અચલ vector<vpiHandle>&table);

vpiHandle vpip_make_મૂળ_iterator(પૂર્ણાંક type_code)
{
      પાછા make_subset_iterator_(type_code, vpip_મૂળ_table);
}

શૂન્ય vpip_make_મૂળ_iterator(__vpiHandle**&table, unsigned&ntable)
{
      table = &vpip_મૂળ_table[0];
      ntable = vpip_મૂળ_table.size();
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય port_delete(__vpiHandle*handle);
શૂન્ય port_bit_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
સ્થિર class_type **class_list = 0;
સ્થિર unsigned class_list_ગણતરી = 0;

સ્થિર શૂન્ય delete_sub_scopes(__vpiScope *scope)
{
      માટે (unsigned idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle item = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(item);
	    પસંદગી(item->get_type_code()) {
		સ્થિતિ vpiFunction:
		સ્થિતિ vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		સ્થિતિ vpiModule:
		સ્થિતિ vpiGenScope:
		સ્થિતિ vpiNamedBegin:
		સ્થિતિ vpiNamedFork:
		સ્થિતિ vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  delete item;
		  તોડો;
		સ્થિતિ vpiMemory:
		સ્થિતિ vpiNetArray:
		  memory_delete(item);
		  તોડો;
		સ્થિતિ vpiModPath:
		    /* The destination ModPath is cleaned up later. */
		  delete item;
		  તોડો;
		સ્થિતિ vpiInterModPath:
		  delete item;
		  તોડો;
		સ્થિતિ vpiNamedEvent:
		  named_event_delete(item);
		  તોડો;
		સ્થિતિ vpiNet:
		સ્થિતિ vpiReg:
		સ્થિતિ vpiIntegerVar:
		સ્થિતિ vpiLongIntVar:
		સ્થિતિ vpiShortIntVar:
		સ્થિતિ vpiIntVar:
		સ્થિતિ vpiByteVar:
		સ્થિતિ vpiBitVar:
		  signal_delete(item);
		  તોડો;
		સ્થિતિ vpiParameter:
		  parameter_delete(item);
		  તોડો;
		સ્થિતિ vpiRealVar:
		  real_delete(item);
		  તોડો;
		સ્થિતિ vpiEnumTypespec:
		  enum_delete(item);
		  તોડો;
		સ્થિતિ vpiPort:
		  port_delete(item);
		  તોડો;
		સ્થિતિ vpiPortBit:
		  port_bit_delete(item);
		  તોડો;
		સ્થિતિ vpiStringVar:
		  string_delete(item);
		  તોડો;
		સ્થિતિ vpiClassVar:
		  class_delete(item);
		  તોડો;
		સ્થિતિ vpiArrayVar:
		  array_delete(item);
		  તોડો;
		મૂળભૂત:
		  fprintf(stderr, "Need support for type: %d\n",
		          item->get_type_code());
		  પ્રતિજ્ઞા(0);
		  તોડો;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      માટે (citer = scope->classes.begin();
           citer != scope->classes.end(); ++ citer ) {
	    class_list_ગણતરી += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_ગણતરી*કદ(class_type **)));
	    class_list[class_list_ગણતરી-1] = citer->second;
      }
}

શૂન્ય મૂળ_table_delete(શૂન્ય)
{
      માટે (unsigned idx = 0; idx < vpip_મૂળ_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_મૂળ_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    delete scope;
      }
      vpip_મૂળ_table.clear();

	/* Clean up all the class definitions. */
      માટે (unsigned idx = 0; idx < class_list_ગણતરી; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      મુક્ત(class_list);
      class_list = 0;
      class_list_ગણતરી = 0;
}
#endif

સ્થિર શૂન્ય construct_scope_fullname(__vpiScope*ref, અક્ષર*buf)
{
      જો (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    જો (ref->scope->get_type_code() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      જો (ref->get_type_code() == vpiPackage) strcat(buf, "::");
}

સ્થિર અચલ અક્ષર* scope_get_type(પૂર્ણાંક code)
{
      પસંદગી (code) {
          સ્થિતિ vpiModule:
            પાછા "vpiModule";
          સ્થિતિ vpiGenScope:
            પાછા "vpiGenScope";
          સ્થિતિ vpiFunction:
            પાછા "vpiFunction";
          સ્થિતિ vpiTask:
            પાછા "vpiTask";
          સ્થિતિ vpiNamedBegin:
            પાછા "vpiNamedBegin";
          સ્થિતિ vpiNamedFork:
            પાછા "vpiNamedFork";
          મૂળભૂત:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            પાછા ખાલી;
      }
}

સ્થિર અક્ષર* scope_get_str(પૂર્ણાંક code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      પ્રતિજ્ઞા(ref);

      અક્ષર buf[4096];  // XXX is a fixed buffer size really reliable?
      અચલ અક્ષર *p=0;
      પસંદગી (code) {
	  સ્થિતિ vpiDefFile:
	    p = file_names[ref->def_file_idx];
	    તોડો;

	  સ્થિતિ vpiFile:
	    p = file_names[ref->file_idx];
	    તોડો;

	  સ્થિતિ vpiFullName:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    તોડો;

	  સ્થિતિ vpiName:
	    p = ref->scope_name();
	    તોડો;

	  સ્થિતિ vpiDefName:
	    p = ref->scope_def_name();
	    તોડો;

	  સ્થિતિ vpiType:
	    p = scope_get_type(code);
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    પાછા ખાલી;
      }
      પાછા simple_set_rbuf_str(p);
}

સ્થિર vpiHandle scope_get_handle(પૂર્ણાંક code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      પ્રતિજ્ઞા(rfp);

      પસંદગી (code) {

	  સ્થિતિ vpiScope:
	    પાછા rfp->scope;

	  સ્થિતિ vpiModule:
	    પાછા rfp->scope;
      }

      પાછા 0;
}

/* compares vpiType's considering object classes */
સ્થિર પૂર્ણાંક compare_types(પૂર્ણાંક code, પૂર્ણાંક type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      જો (code == vpiScope)
	    પાછા 1;

      જો (code == type)
	    પાછા 1;

      જો ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    પાછા 1;

      જો ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    પાછા 1;

      જો ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    પાછા 1;

      પાછા 0;
}

સ્થિર vpiHandle make_subset_iterator_(પૂર્ણાંક type_code, અચલ vector<vpiHandle>&table)
{
      unsigned mcnt = 0, ncnt = 0;
      vpiHandle*args;

      માટે (unsigned idx = 0; idx < table.size(); idx += 1)
	    જો (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      જો (mcnt == 0)
	    પાછા 0;

      args = static_cast<vpiHandle*>(calloc(mcnt, કદ(vpiHandle)));
      માટે (unsigned idx = 0; idx < table.size(); idx += 1)
	    જો (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      પ્રતિજ્ઞા(ncnt == mcnt);

      પાછા vpip_make_iterator(mcnt, args, સાચું);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
સ્થિર vpiHandle module_iter(પૂર્ણાંક code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      પ્રતિજ્ઞા(ref);

      પાછા make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(અચલ અક્ષર*nam, અચલ અક્ષર*tnam, તાર્કિક auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

પૂર્ણાંક __vpiScope::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiCellInstance:
	    પાછા is_cell? 1 : 0;

	  સ્થિતિ vpi_Def_લીટી_No:
	    પાછા def_lineno;

	  સ્થિતિ vpi_લીટી_No:
	    પાછા lineno;

	  સ્થિતિ vpiTimeUnit:
	    પાછા time_units;

	  સ્થિતિ vpiTimePrecision:
	    પાછા time_precision;

	  સ્થિતિ vpi_ટોચ_Module:
	    પાછા 0x0 == scope;

          સ્થિતિ vpiAutomatic:
	    પાછા is_automatic_? 1 : 0;
      }

      પાછા vpiUndefined;
}


અક્ષર*__vpiScope::vpi_get_str(પૂર્ણાંક code)
{ પાછા scope_get_str(code, આ); }

vpiHandle __vpiScope::vpi_handle(પૂર્ણાંક code)
{ પાછા scope_get_handle(code, આ); }

vpiHandle __vpiScope::vpi_iterate(પૂર્ણાંક code)
{ પાછા module_iter(code, આ); }


વર્ગ vpiScopeModule  : જાહેર __vpiScope {
    જાહેર:
      અંતઃરેખ vpiScopeModule(અચલ અક્ષર*nam, અચલ અક્ષર*tnam)
      : __vpiScope(nam,tnam,ખોટું) { }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiModule; }
};

સંરચના vpiScopePackage  : જાહેર __vpiScope {
      અંતઃરેખ vpiScopePackage(અચલ અક્ષર*nam, અચલ અક્ષર*tnam)
      : __vpiScope(nam,tnam) { }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiPackage; }
};

સંરચના vpiScopeTask  : જાહેર __vpiScope {
      અંતઃરેખ vpiScopeTask(અચલ અક્ષર*nam, અચલ અક્ષર*tnam)
      : __vpiScope(nam,tnam) { }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiTask; }
};

સંરચના vpiScopeTaskAuto  : જાહેર __vpiScope {
      અંતઃરેખ vpiScopeTaskAuto(અચલ અક્ષર*nam, અચલ અક્ષર*tnam)
      : __vpiScope(nam,tnam,સાચું) {  }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiTask; }
};

સંરચના vpiScopeBegin  : જાહેર __vpiScope {
      અંતઃરેખ vpiScopeBegin(અચલ અક્ષર*nam, અચલ અક્ષર*tnam)
      : __vpiScope(nam,tnam,ખોટું) { }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiNamedBegin; }
};

વર્ગ vpiScopeBeginAuto  : જાહેર __vpiScope {
    જાહેર:
      અંતઃરેખ vpiScopeBeginAuto(અચલ અક્ષર*nam, અચલ અક્ષર*tnam)
      : __vpiScope(nam,tnam,સાચું) {  }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiNamedBegin; }
};

સંરચના vpiScopeGenerate  : જાહેર __vpiScope {
      અંતઃરેખ vpiScopeGenerate(અચલ અક્ષર*nam, અચલ અક્ષર*tnam)
      : __vpiScope(nam,tnam) { }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiGenScope; }
};

સંરચના vpiScopeFork  : જાહેર __vpiScope {
      અંતઃરેખ vpiScopeFork(અચલ અક્ષર*nam, અચલ અક્ષર*tnam)
      : __vpiScope(nam,tnam,ખોટું) { }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiNamedFork; }
};

વર્ગ vpiScopeForkAuto  : જાહેર __vpiScope {
    જાહેર:
      અંતઃરેખ vpiScopeForkAuto(અચલ અક્ષર*nam, અચલ અક્ષર*tnam)
      : __vpiScope(nam,tnam,સાચું) {  }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiNamedFork; }
};

સંરચના vpiScopeClass  : જાહેર __vpiScope {
      અંતઃરેખ vpiScopeClass(અચલ અક્ષર*nam, અચલ અક્ષર*tnam)
      : __vpiScope(nam,tnam) { }
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
સ્થિર __vpiScope*current_scope = 0;

શૂન્ય vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj)
{
      પ્રતિજ્ઞા(scope);
      scope->intern.push_back(obj);
}

/*
 * When the compiler encounters a scope declaration, this function
 * creates and initializes a __vpiScope object with the requested name
 * and within the addressed parent. The label is used as a key in the
 * symbol table and the name is used to construct the actual object.
 */
શૂન્ય
compile_scope_decl(અક્ષર*label, અક્ષર*type, અક્ષર*name, અક્ષર*tname,
                   અક્ષર*parent, long file_idx, long lineno,
                   long def_file_idx, long def_lineno, long is_cell)
{
      ગણતરી_vpi_scopes += 1;
      અક્ષર vec_type;
      અક્ષર sign_flag;
      unsigned wid;

      __vpiScope*scope;
      જો (strcmp(type,"module") == 0) {
	    scope = નવું vpiScopeModule(name, tname);
      } નહીંતર જો ( sscanf(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    પૂર્ણાંક type_code;
	    જો (sign_flag=='s') {
		  type_code = vpiSizedSignedFunc;
	    } નહીંતર જો (sign_flag=='u') {
		  type_code = vpiSizedFunc;
	    } નહીંતર જો (sign_flag=='i') {
		  type_code = vpiIntFunc;
	    } નહીંતર {
		  પ્રતિજ્ઞા(0);
		  type_code = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = નવું vpiScopeFunction(name, tname, ખોટું, type_code, wid, init_val);

      } નહીંતર જો ( sscanf(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    પૂર્ણાંક type_code;
	    પસંદગી (sign_flag) {
		સ્થિતિ 's':
		  type_code = vpiSizedSignedFunc;
		  તોડો;
		સ્થિતિ 'u':
		  type_code = vpiSizedFunc;
		  તોડો;
		મૂળભૂત:
		  પ્રતિજ્ઞા(0);
		  type_code = vpiSizedFunc;
		  તોડો;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = નવું vpiScopeFunction(name, tname, સાચું, type_code, wid, init_val);

      } નહીંતર જો (strcmp(type,"function.obj") == 0) {
	    scope = નવું vpiScopeFunction(name, tname, ખોટું, vpiSizedFunc, 0, BIT4_0);
      } નહીંતર જો (strcmp(type,"autofunction.obj") == 0) {
	    scope = નવું vpiScopeFunction(name, tname, સાચું, vpiSizedFunc, 0, BIT4_0);
      } નહીંતર જો (strcmp(type,"function.real") == 0) {
	    scope = નવું vpiScopeFunction(name, tname, ખોટું, vpiRealFunc, 0, BIT4_0);
      } નહીંતર જો (strcmp(type,"autofunction.real") == 0) {
	    scope = નવું vpiScopeFunction(name, tname, સાચું, vpiRealFunc, 0, BIT4_0);
      } નહીંતર જો (strcmp(type,"function.str") == 0) {
	    scope = નવું vpiScopeFunction(name, tname, ખોટું, vpiOtherFunc, 0, BIT4_0);
      } નહીંતર જો (strcmp(type,"autofunction.str") == 0) {
	    scope = નવું vpiScopeFunction(name, tname, સાચું, vpiOtherFunc, 0, BIT4_0);
      } નહીંતર જો (strcmp(type,"function.void") == 0) {
	    scope = નવું vpiScopeFunction(name, tname, ખોટું, vpiOtherFunc, 0, BIT4_0);
      } નહીંતર જો (strcmp(type,"autofunction.void") == 0) {
	    scope = નવું vpiScopeFunction(name, tname, સાચું, vpiOtherFunc, 0, BIT4_0);
      } નહીંતર જો (strcmp(type,"task") == 0) {
	    scope = નવું vpiScopeTask(name, tname);
      } નહીંતર જો (strcmp(type,"autotask") == 0) {
	    scope = નવું vpiScopeTaskAuto(name, tname);
      } નહીંતર જો (strcmp(type,"fork") == 0) {
	    scope = નવું vpiScopeFork(name, tname);
      } નહીંતર જો (strcmp(type,"autofork") == 0) {
	    scope = નવું vpiScopeForkAuto(name, tname);
      } નહીંતર જો (strcmp(type,"begin") == 0) {
	    scope = નવું vpiScopeBegin(name, tname);
      } નહીંતર જો (strcmp(type,"autobegin") == 0) {
	    scope = નવું vpiScopeBeginAuto(name, tname);
      } નહીંતર જો (strcmp(type,"generate") == 0) {
	    scope = નવું vpiScopeGenerate(name, tname);
      } નહીંતર જો (strcmp(type,"package") == 0) {
	    scope = નવું vpiScopePackage(name, tname);
      } નહીંતર જો (strcmp(type,"class") == 0) {
	    scope = નવું vpiScopeClass(name, tname);
      } નહીંતર {
	    scope = નવું vpiScopeModule(name, tname);
	    પ્રતિજ્ઞા(0);
      }

      scope->file_idx = (unsigned) file_idx;
      scope->lineno  = (unsigned) lineno;
      scope->def_file_idx = (unsigned) def_file_idx;
      scope->def_lineno  = (unsigned) def_lineno;
      scope->item = 0;
      scope->nitem = 0;
      scope->live_contexts = 0;
      scope->free_contexts = 0;

      જો (is_cell) scope->is_cell = સાચું;
      નહીંતર scope->is_cell = ખોટું;

      current_scope = scope;

      compile_vpi_પ્રતીક(label, scope);

      મુક્ત(label);
      મુક્ત(type);
      delete[] name;
      delete[] tname;

      જો (parent) {
	    સ્થિર vpiHandle obj;
	    compile_vpi_lookup(&obj, parent);
	    પ્રતિજ્ઞા(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_to_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->time_units = sp->time_units;
	    scope->time_precision = sp->time_precision;

      } નહીંતર {
	    scope->scope = 0x0;

	    vpip_મૂળ_table.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->time_units = vpip_get_time_precision();
	    scope->time_precision = vpip_get_time_precision();
      }
}

શૂન્ય compile_scope_recall(અક્ષર*પ્રતીક)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(શૂન્ય*)&current_scope, પ્રતીક);
      પ્રતિજ્ઞા(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
શૂન્ય compile_timescale(long units, long precision)
{
      પ્રતિજ્ઞા(current_scope);
      current_scope->time_units = units;
      current_scope->time_precision = precision;
}

__vpiScope* vpip_peek_current_scope(શૂન્ય)
{
      પાછા current_scope;
}

શૂન્ય vpip_attach_to_current_scope(vpiHandle obj)
{
      vpip_attach_to_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(શૂન્ય)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      જ્યારે (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      પાછા scope;
}

unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      પ્રતિજ્ઞા(scope);
      પ્રતિજ્ઞા(scope->is_automatic());

      unsigned idx = scope->nitem++;

      જો (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (malloc(કદ(automatic_hooks_s*)));
      નહીંતર
	    scope->item = static_cast<automatic_hooks_s**>
	                  (realloc(scope->item,
	                   કદ(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      પાછા 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              unsigned index,
              પૂર્ણાંક vpi_direction,
              unsigned width,
              અચલ અક્ષર *name,
              અક્ષર* buffer) :
      parent_(parent),
      index_(index),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      જો (buffer != nullptr) functor_ref_lookup(&ref_, buffer);
      નહીંતર ref_ = nullptr;
}

vpiPortInfo::~vpiPortInfo()
{
      delete[] name_;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય port_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortInfo *>(handle);
}
#endif

પૂર્ણાંક vpiPortInfo::vpi_get(પૂર્ણાંક code)
{
      પસંદગી( code ) {

        સ્થિતિ vpiDirection :
          પાછા direction_;
        સ્થિતિ vpiPortIndex :
          પાછા index_;
        સ્થિતિ vpiSize :
          પાછા width_;
        મૂળભૂત :
          પાછા vpiUndefined;
      }

}


અક્ષર *vpiPortInfo::vpi_get_str(પૂર્ણાંક code)
{
      પસંદગી( code ) {
        સ્થિતિ vpiName :
          પાછા simple_set_rbuf_str(name_);
        મૂળભૂત :
          પાછા ખાલી;
      }

}


vpiHandle vpiPortInfo::vpi_handle(પૂર્ણાંક code)
{

      પસંદગી (code) {

          સ્થિતિ vpiParent:
          સ્થિતિ vpiScope:
          સ્થિતિ vpiModule:
            પાછા parent_;
          મૂળભૂત :
            તોડો;
      }

      પાછા 0;
}

સ્થિર vpiHandle portinfo_iterate(પૂર્ણાંક code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      પ્રતિજ્ઞા(rfp);
      unsigned width = rfp->get_width();

      પસંદગી (code) {
	  સ્થિતિ vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(width, કદ(vpiHandle*)));

	    માટે (unsigned i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    પાછા vpip_make_iterator(width, args, સાચું);
	  }
      }
      પાછા 0;
}

vpiHandle vpiPortInfo::vpi_iterate(પૂર્ણાંક code)
{
      પાછા portinfo_iterate(code, આ);
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
શૂન્ય port_bit_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortBitInfo *>(handle);
}
#endif

vpiHandle vpiPortBitInfo::vpi_handle(પૂર્ણાંક code)
{

      પસંદગી (code) {
          સ્થિતિ vpiParent:
            પાછા parent_;
          મૂળભૂત :
            તોડો;
      }

      પાછા 0;
}

પૂર્ણાંક vpiPortBitInfo::vpi_get(પૂર્ણાંક code)
{
      પસંદગી( code ) {
        સ્થિતિ vpiBit :
          પાછા bit_;
        મૂળભૂત :
          તોડો;
      }

      પાછા 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
શૂન્ય compile_port_info( unsigned index, પૂર્ણાંક vpi_direction, unsigned width, અચલ અક્ષર *name, અક્ષર* buffer )
{
      vpiPortInfo* obj = નવું vpiPortInfo( vpip_peek_current_scope(),
                                          index, vpi_direction, width, name, buffer );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      માટે (unsigned i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = નવું vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
