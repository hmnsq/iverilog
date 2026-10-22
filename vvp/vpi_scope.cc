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

using नामाकाश std;

स्थिर vector<vpiHandle> vpip_मूळ_table;

स्थिर vpiHandle make_subset_iterator_(पूर्णांक type_code, स्थिरांक vector<vpiHandle>&table);

vpiHandle vpip_make_मूळ_iterator(पूर्णांक type_code)
{
      परत make_subset_iterator_(type_code, vpip_मूळ_table);
}

शून्य vpip_make_मूळ_iterator(__vpiHandle**&table, अचिन्हित&ntable)
{
      table = &vpip_मूळ_table[0];
      ntable = vpip_मूळ_table.size();
}

#ifdef CHECK_WITH_VALGRIND
शून्य port_delete(__vpiHandle*handle);
शून्य port_bit_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
स्थिर class_type **class_list = 0;
स्थिर अचिन्हित class_list_मोजणी = 0;

स्थिर शून्य delete_sub_scopes(__vpiScope *scope)
{
      साठी (अचिन्हित idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle item = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(item);
	    निवड(item->get_type_code()) {
		स्थिती vpiFunction:
		स्थिती vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		स्थिती vpiModule:
		स्थिती vpiGenScope:
		स्थिती vpiNamedBegin:
		स्थिती vpiNamedFork:
		स्थिती vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  delete item;
		  थांबा;
		स्थिती vpiMemory:
		स्थिती vpiNetArray:
		  memory_delete(item);
		  थांबा;
		स्थिती vpiModPath:
		    /* The destination ModPath is cleaned up later. */
		  delete item;
		  थांबा;
		स्थिती vpiInterModPath:
		  delete item;
		  थांबा;
		स्थिती vpiNamedEvent:
		  named_event_delete(item);
		  थांबा;
		स्थिती vpiNet:
		स्थिती vpiReg:
		स्थिती vpiIntegerVar:
		स्थिती vpiLongIntVar:
		स्थिती vpiShortIntVar:
		स्थिती vpiIntVar:
		स्थिती vpiByteVar:
		स्थिती vpiBitVar:
		  signal_delete(item);
		  थांबा;
		स्थिती vpiParameter:
		  parameter_delete(item);
		  थांबा;
		स्थिती vpiRealVar:
		  real_delete(item);
		  थांबा;
		स्थिती vpiEnumTypespec:
		  enum_delete(item);
		  थांबा;
		स्थिती vpiPort:
		  port_delete(item);
		  थांबा;
		स्थिती vpiPortBit:
		  port_bit_delete(item);
		  थांबा;
		स्थिती vpiStringVar:
		  string_delete(item);
		  थांबा;
		स्थिती vpiClassVar:
		  class_delete(item);
		  थांबा;
		स्थिती vpiArrayVar:
		  array_delete(item);
		  थांबा;
		पूर्वनिर्धारित:
		  ख_लिखो(मानक_त्रुटि, "Need support for type: %d\n",
		          item->get_type_code());
		  प्रतिज्ञा(0);
		  थांबा;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      साठी (citer = scope->classes.begin();
           citer != scope->classes.end(); ++ citer ) {
	    class_list_मोजणी += 1;
	    class_list = static_cast<class_type **>
	                 (पुनः_स्मृति(class_list,
	                  class_list_मोजणी*आकार(class_type **)));
	    class_list[class_list_मोजणी-1] = citer->second;
      }
}

शून्य मूळ_table_delete(शून्य)
{
      साठी (अचिन्हित idx = 0; idx < vpip_मूळ_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_मूळ_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    delete scope;
      }
      vpip_मूळ_table.clear();

	/* Clean up all the class definitions. */
      साठी (अचिन्हित idx = 0; idx < class_list_मोजणी; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      मुक्त(class_list);
      class_list = 0;
      class_list_मोजणी = 0;
}
#endif

स्थिर शून्य construct_scope_fullname(__vpiScope*ref, अक्षर*buf)
{
      जर (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    जर (ref->scope->get_type_code() != vpiPackage) {
		  म_जोड़ो(buf, ".");
	    }
      }

      म_जोड़ो(buf, ref->scope_name());
	// For a package, add a "::" to the end
      जर (ref->get_type_code() == vpiPackage) म_जोड़ो(buf, "::");
}

स्थिर स्थिरांक अक्षर* scope_get_type(पूर्णांक code)
{
      निवड (code) {
          स्थिती vpiModule:
            परत "vpiModule";
          स्थिती vpiGenScope:
            परत "vpiGenScope";
          स्थिती vpiFunction:
            परत "vpiFunction";
          स्थिती vpiTask:
            परत "vpiTask";
          स्थिती vpiNamedBegin:
            परत "vpiNamedBegin";
          स्थिती vpiNamedFork:
            परत "vpiNamedFork";
          पूर्वनिर्धारित:
            ख_लिखो(मानक_त्रुटि, "VPI error: invalid scope type code %d.\n", code);
            परत रिक्त;
      }
}

स्थिर अक्षर* scope_get_str(पूर्णांक code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      प्रतिज्ञा(ref);

      अक्षर buf[4096];  // XXX is a fixed buffer size really reliable?
      स्थिरांक अक्षर *p=0;
      निवड (code) {
	  स्थिती vpiDefFile:
	    p = file_names[ref->def_file_idx];
	    थांबा;

	  स्थिती vpiFile:
	    p = file_names[ref->file_idx];
	    थांबा;

	  स्थिती vpiFullName:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    थांबा;

	  स्थिती vpiName:
	    p = ref->scope_name();
	    थांबा;

	  स्थिती vpiDefName:
	    p = ref->scope_def_name();
	    थांबा;

	  स्थिती vpiType:
	    p = scope_get_type(code);
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "VPI error: invalid scope string code %d.\n", code);
	    परत रिक्त;
      }
      परत simple_set_rbuf_str(p);
}

स्थिर vpiHandle scope_get_handle(पूर्णांक code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      प्रतिज्ञा(rfp);

      निवड (code) {

	  स्थिती vpiScope:
	    परत rfp->scope;

	  स्थिती vpiModule:
	    परत rfp->scope;
      }

      परत 0;
}

/* compares vpiType's considering object classes */
स्थिर पूर्णांक compare_types(पूर्णांक code, पूर्णांक type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      जर (code == vpiScope)
	    परत 1;

      जर (code == type)
	    परत 1;

      जर ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    परत 1;

      जर ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    परत 1;

      जर ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    परत 1;

      परत 0;
}

स्थिर vpiHandle make_subset_iterator_(पूर्णांक type_code, स्थिरांक vector<vpiHandle>&table)
{
      अचिन्हित mcnt = 0, ncnt = 0;
      vpiHandle*args;

      साठी (अचिन्हित idx = 0; idx < table.size(); idx += 1)
	    जर (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      जर (mcnt == 0)
	    परत 0;

      args = static_cast<vpiHandle*>(सुस्मृति(mcnt, आकार(vpiHandle)));
      साठी (अचिन्हित idx = 0; idx < table.size(); idx += 1)
	    जर (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      प्रतिज्ञा(ncnt == mcnt);

      परत vpip_make_iterator(mcnt, args, सत्य);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
स्थिर vpiHandle module_iter(पूर्णांक code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      प्रतिज्ञा(ref);

      परत make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(स्थिरांक अक्षर*nam, स्थिरांक अक्षर*tnam, तार्किक auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

पूर्णांक __vpiScope::vpi_get(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiCellInstance:
	    परत is_cell? 1 : 0;

	  स्थिती vpi_Def_ओळ_No:
	    परत def_lineno;

	  स्थिती vpi_ओळ_No:
	    परत lineno;

	  स्थिती vpiTimeUnit:
	    परत time_units;

	  स्थिती vpiTimePrecision:
	    परत time_precision;

	  स्थिती vpi_शीर्ष_Module:
	    परत 0x0 == scope;

          स्थिती vpiAutomatic:
	    परत is_automatic_? 1 : 0;
      }

      परत vpiUndefined;
}


अक्षर*__vpiScope::vpi_get_str(पूर्णांक code)
{ परत scope_get_str(code, हे); }

vpiHandle __vpiScope::vpi_handle(पूर्णांक code)
{ परत scope_get_handle(code, हे); }

vpiHandle __vpiScope::vpi_iterate(पूर्णांक code)
{ परत module_iter(code, हे); }


वर्ग vpiScopeModule  : सार्वजनिक __vpiScope {
    सार्वजनिक:
      ओळीत vpiScopeModule(स्थिरांक अक्षर*nam, स्थिरांक अक्षर*tnam)
      : __vpiScope(nam,tnam,असत्य) { }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiModule; }
};

संरचना vpiScopePackage  : सार्वजनिक __vpiScope {
      ओळीत vpiScopePackage(स्थिरांक अक्षर*nam, स्थिरांक अक्षर*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiPackage; }
};

संरचना vpiScopeTask  : सार्वजनिक __vpiScope {
      ओळीत vpiScopeTask(स्थिरांक अक्षर*nam, स्थिरांक अक्षर*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiTask; }
};

संरचना vpiScopeTaskAuto  : सार्वजनिक __vpiScope {
      ओळीत vpiScopeTaskAuto(स्थिरांक अक्षर*nam, स्थिरांक अक्षर*tnam)
      : __vpiScope(nam,tnam,सत्य) {  }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiTask; }
};

संरचना vpiScopeBegin  : सार्वजनिक __vpiScope {
      ओळीत vpiScopeBegin(स्थिरांक अक्षर*nam, स्थिरांक अक्षर*tnam)
      : __vpiScope(nam,tnam,असत्य) { }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiNamedBegin; }
};

वर्ग vpiScopeBeginAuto  : सार्वजनिक __vpiScope {
    सार्वजनिक:
      ओळीत vpiScopeBeginAuto(स्थिरांक अक्षर*nam, स्थिरांक अक्षर*tnam)
      : __vpiScope(nam,tnam,सत्य) {  }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiNamedBegin; }
};

संरचना vpiScopeGenerate  : सार्वजनिक __vpiScope {
      ओळीत vpiScopeGenerate(स्थिरांक अक्षर*nam, स्थिरांक अक्षर*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiGenScope; }
};

संरचना vpiScopeFork  : सार्वजनिक __vpiScope {
      ओळीत vpiScopeFork(स्थिरांक अक्षर*nam, स्थिरांक अक्षर*tnam)
      : __vpiScope(nam,tnam,असत्य) { }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiNamedFork; }
};

वर्ग vpiScopeForkAuto  : सार्वजनिक __vpiScope {
    सार्वजनिक:
      ओळीत vpiScopeForkAuto(स्थिरांक अक्षर*nam, स्थिरांक अक्षर*tnam)
      : __vpiScope(nam,tnam,सत्य) {  }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiNamedFork; }
};

संरचना vpiScopeClass  : सार्वजनिक __vpiScope {
      ओळीत vpiScopeClass(स्थिरांक अक्षर*nam, स्थिरांक अक्षर*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
स्थिर __vpiScope*current_scope = 0;

शून्य vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj)
{
      प्रतिज्ञा(scope);
      scope->intern.push_back(obj);
}

/*
 * When the compiler encounters a scope declaration, this function
 * creates and initializes a __vpiScope object with the requested name
 * and within the addressed parent. The label is used as a key in the
 * symbol table and the name is used to construct the actual object.
 */
शून्य
compile_scope_decl(अक्षर*label, अक्षर*type, अक्षर*name, अक्षर*tname,
                   अक्षर*parent, दीर्घ file_idx, दीर्घ lineno,
                   दीर्घ def_file_idx, दीर्घ def_lineno, दीर्घ is_cell)
{
      मोजणी_vpi_scopes += 1;
      अक्षर vec_type;
      अक्षर sign_flag;
      अचिन्हित wid;

      __vpiScope*scope;
      जर (म_भेद(type,"module") == 0) {
	    scope = नवीन vpiScopeModule(name, tname);
      } अन्यथा जर ( माला_पूछो(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    पूर्णांक type_code;
	    जर (sign_flag=='s') {
		  type_code = vpiSizedSignedFunc;
	    } अन्यथा जर (sign_flag=='u') {
		  type_code = vpiSizedFunc;
	    } अन्यथा जर (sign_flag=='i') {
		  type_code = vpiIntFunc;
	    } अन्यथा {
		  प्रतिज्ञा(0);
		  type_code = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = नवीन vpiScopeFunction(name, tname, असत्य, type_code, wid, init_val);

      } अन्यथा जर ( माला_पूछो(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    पूर्णांक type_code;
	    निवड (sign_flag) {
		स्थिती 's':
		  type_code = vpiSizedSignedFunc;
		  थांबा;
		स्थिती 'u':
		  type_code = vpiSizedFunc;
		  थांबा;
		पूर्वनिर्धारित:
		  प्रतिज्ञा(0);
		  type_code = vpiSizedFunc;
		  थांबा;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = नवीन vpiScopeFunction(name, tname, सत्य, type_code, wid, init_val);

      } अन्यथा जर (म_भेद(type,"function.obj") == 0) {
	    scope = नवीन vpiScopeFunction(name, tname, असत्य, vpiSizedFunc, 0, BIT4_0);
      } अन्यथा जर (म_भेद(type,"autofunction.obj") == 0) {
	    scope = नवीन vpiScopeFunction(name, tname, सत्य, vpiSizedFunc, 0, BIT4_0);
      } अन्यथा जर (म_भेद(type,"function.real") == 0) {
	    scope = नवीन vpiScopeFunction(name, tname, असत्य, vpiRealFunc, 0, BIT4_0);
      } अन्यथा जर (म_भेद(type,"autofunction.real") == 0) {
	    scope = नवीन vpiScopeFunction(name, tname, सत्य, vpiRealFunc, 0, BIT4_0);
      } अन्यथा जर (म_भेद(type,"function.str") == 0) {
	    scope = नवीन vpiScopeFunction(name, tname, असत्य, vpiOtherFunc, 0, BIT4_0);
      } अन्यथा जर (म_भेद(type,"autofunction.str") == 0) {
	    scope = नवीन vpiScopeFunction(name, tname, सत्य, vpiOtherFunc, 0, BIT4_0);
      } अन्यथा जर (म_भेद(type,"function.void") == 0) {
	    scope = नवीन vpiScopeFunction(name, tname, असत्य, vpiOtherFunc, 0, BIT4_0);
      } अन्यथा जर (म_भेद(type,"autofunction.void") == 0) {
	    scope = नवीन vpiScopeFunction(name, tname, सत्य, vpiOtherFunc, 0, BIT4_0);
      } अन्यथा जर (म_भेद(type,"task") == 0) {
	    scope = नवीन vpiScopeTask(name, tname);
      } अन्यथा जर (म_भेद(type,"autotask") == 0) {
	    scope = नवीन vpiScopeTaskAuto(name, tname);
      } अन्यथा जर (म_भेद(type,"fork") == 0) {
	    scope = नवीन vpiScopeFork(name, tname);
      } अन्यथा जर (म_भेद(type,"autofork") == 0) {
	    scope = नवीन vpiScopeForkAuto(name, tname);
      } अन्यथा जर (म_भेद(type,"begin") == 0) {
	    scope = नवीन vpiScopeBegin(name, tname);
      } अन्यथा जर (म_भेद(type,"autobegin") == 0) {
	    scope = नवीन vpiScopeBeginAuto(name, tname);
      } अन्यथा जर (म_भेद(type,"generate") == 0) {
	    scope = नवीन vpiScopeGenerate(name, tname);
      } अन्यथा जर (म_भेद(type,"package") == 0) {
	    scope = नवीन vpiScopePackage(name, tname);
      } अन्यथा जर (म_भेद(type,"class") == 0) {
	    scope = नवीन vpiScopeClass(name, tname);
      } अन्यथा {
	    scope = नवीन vpiScopeModule(name, tname);
	    प्रतिज्ञा(0);
      }

      scope->file_idx = (अचिन्हित) file_idx;
      scope->lineno  = (अचिन्हित) lineno;
      scope->def_file_idx = (अचिन्हित) def_file_idx;
      scope->def_lineno  = (अचिन्हित) def_lineno;
      scope->item = 0;
      scope->nitem = 0;
      scope->live_contexts = 0;
      scope->free_contexts = 0;

      जर (is_cell) scope->is_cell = सत्य;
      अन्यथा scope->is_cell = असत्य;

      current_scope = scope;

      compile_vpi_चिन्ह(label, scope);

      मुक्त(label);
      मुक्त(type);
      delete[] name;
      delete[] tname;

      जर (parent) {
	    स्थिर vpiHandle obj;
	    compile_vpi_lookup(&obj, parent);
	    प्रतिज्ञा(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_to_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->time_units = sp->time_units;
	    scope->time_precision = sp->time_precision;

      } अन्यथा {
	    scope->scope = 0x0;

	    vpip_मूळ_table.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->time_units = vpip_get_time_precision();
	    scope->time_precision = vpip_get_time_precision();
      }
}

शून्य compile_scope_recall(अक्षर*चिन्ह)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(शून्य*)&current_scope, चिन्ह);
      प्रतिज्ञा(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
शून्य compile_timescale(दीर्घ units, दीर्घ precision)
{
      प्रतिज्ञा(current_scope);
      current_scope->time_units = units;
      current_scope->time_precision = precision;
}

__vpiScope* vpip_peek_current_scope(शून्य)
{
      परत current_scope;
}

शून्य vpip_attach_to_current_scope(vpiHandle obj)
{
      vpip_attach_to_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(शून्य)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      पर्यंत (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      परत scope;
}

अचिन्हित vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      प्रतिज्ञा(scope);
      प्रतिज्ञा(scope->is_automatic());

      अचिन्हित idx = scope->nitem++;

      जर (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (दो_स्मृति(आकार(automatic_hooks_s*)));
      अन्यथा
	    scope->item = static_cast<automatic_hooks_s**>
	                  (पुनः_स्मृति(scope->item,
	                   आकार(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      परत 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              अचिन्हित index,
              पूर्णांक vpi_direction,
              अचिन्हित width,
              स्थिरांक अक्षर *name,
              अक्षर* buffer) :
      parent_(parent),
      index_(index),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      जर (buffer != nullptr) functor_ref_lookup(&ref_, buffer);
      अन्यथा ref_ = nullptr;
}

vpiPortInfo::~vpiPortInfo()
{
      delete[] name_;
}

#ifdef CHECK_WITH_VALGRIND
शून्य port_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortInfo *>(handle);
}
#endif

पूर्णांक vpiPortInfo::vpi_get(पूर्णांक code)
{
      निवड( code ) {

        स्थिती vpiDirection :
          परत direction_;
        स्थिती vpiPortIndex :
          परत index_;
        स्थिती vpiSize :
          परत width_;
        पूर्वनिर्धारित :
          परत vpiUndefined;
      }

}


अक्षर *vpiPortInfo::vpi_get_str(पूर्णांक code)
{
      निवड( code ) {
        स्थिती vpiName :
          परत simple_set_rbuf_str(name_);
        पूर्वनिर्धारित :
          परत रिक्त;
      }

}


vpiHandle vpiPortInfo::vpi_handle(पूर्णांक code)
{

      निवड (code) {

          स्थिती vpiParent:
          स्थिती vpiScope:
          स्थिती vpiModule:
            परत parent_;
          पूर्वनिर्धारित :
            थांबा;
      }

      परत 0;
}

स्थिर vpiHandle portinfo_iterate(पूर्णांक code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      प्रतिज्ञा(rfp);
      अचिन्हित width = rfp->get_width();

      निवड (code) {
	  स्थिती vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(सुस्मृति(width, आकार(vpiHandle*)));

	    साठी (अचिन्हित i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    परत vpip_make_iterator(width, args, सत्य);
	  }
      }
      परत 0;
}

vpiHandle vpiPortInfo::vpi_iterate(पूर्णांक code)
{
      परत portinfo_iterate(code, हे);
}


vpiPortBitInfo::vpiPortBitInfo(vpiPortInfo *parent,
                               अचिन्हित bit) :
      parent_(parent),
      bit_(bit)
{
}

vpiPortBitInfo::~vpiPortBitInfo()
{
}

#ifdef CHECK_WITH_VALGRIND
शून्य port_bit_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortBitInfo *>(handle);
}
#endif

vpiHandle vpiPortBitInfo::vpi_handle(पूर्णांक code)
{

      निवड (code) {
          स्थिती vpiParent:
            परत parent_;
          पूर्वनिर्धारित :
            थांबा;
      }

      परत 0;
}

पूर्णांक vpiPortBitInfo::vpi_get(पूर्णांक code)
{
      निवड( code ) {
        स्थिती vpiBit :
          परत bit_;
        पूर्वनिर्धारित :
          थांबा;
      }

      परत 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
शून्य compile_port_info( अचिन्हित index, पूर्णांक vpi_direction, अचिन्हित width, स्थिरांक अक्षर *name, अक्षर* buffer )
{
      vpiPortInfo* obj = नवीन vpiPortInfo( vpip_peek_current_scope(),
                                          index, vpi_direction, width, name, buffer );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      साठी (अचिन्हित i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = नवीन vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
