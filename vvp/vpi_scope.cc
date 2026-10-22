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

स्थिर vector<vpiHandle> vpip_मूलम्_table;

स्थिर vpiHandle make_subset_iterator_(पूर्णाङ्क type_code, ध्रुव vector<vpiHandle>&table);

vpiHandle vpip_make_मूलम्_iterator(पूर्णाङ्क type_code)
{
      पटिनिवत्त make_subset_iterator_(type_code, vpip_मूलम्_table);
}

शून्य vpip_make_मूलम्_iterator(__vpiHandle**&table, unsigned&ntable)
{
      table = &vpip_मूलम्_table[0];
      ntable = vpip_मूलम्_table.size();
}

#ifdef CHECK_WITH_VALGRIND
शून्य port_delete(__vpiHandle*handle);
शून्य port_bit_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
स्थिर class_type **class_list = 0;
स्थिर unsigned class_list_गणनम् = 0;

स्थिर शून्य delete_sub_scopes(__vpiScope *scope)
{
      अत्थाय (unsigned idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle item = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(item);
	    विकल्प(item->get_type_code()) {
		पक्ष vpiFunction:
		पक्ष vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		पक्ष vpiModule:
		पक्ष vpiGenScope:
		पक्ष vpiNamedBegin:
		पक्ष vpiNamedFork:
		पक्ष vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  delete item;
		  भङ्ग;
		पक्ष vpiMemory:
		पक्ष vpiNetArray:
		  memory_delete(item);
		  भङ्ग;
		पक्ष vpiModPath:
		    /* The destination ModPath is cleaned up later. */
		  delete item;
		  भङ्ग;
		पक्ष vpiInterModPath:
		  delete item;
		  भङ्ग;
		पक्ष vpiNamedEvent:
		  named_event_delete(item);
		  भङ्ग;
		पक्ष vpiNet:
		पक्ष vpiReg:
		पक्ष vpiIntegerVar:
		पक्ष vpiLongIntVar:
		पक्ष vpiShortIntVar:
		पक्ष vpiIntVar:
		पक्ष vpiByteVar:
		पक्ष vpiBitVar:
		  signal_delete(item);
		  भङ्ग;
		पक्ष vpiParameter:
		  parameter_delete(item);
		  भङ्ग;
		पक्ष vpiRealVar:
		  real_delete(item);
		  भङ्ग;
		पक्ष vpiEnumTypespec:
		  enum_delete(item);
		  भङ्ग;
		पक्ष vpiPort:
		  port_delete(item);
		  भङ्ग;
		पक्ष vpiPortBit:
		  port_bit_delete(item);
		  भङ्ग;
		पक्ष vpiStringVar:
		  string_delete(item);
		  भङ्ग;
		पक्ष vpiClassVar:
		  class_delete(item);
		  भङ्ग;
		पक्ष vpiArrayVar:
		  array_delete(item);
		  भङ्ग;
		स्वभाव:
		  fprintf(stderr, "Need support for type: %d\n",
		          item->get_type_code());
		  प्रतिज्ञा(0);
		  भङ्ग;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      अत्थाय (citer = scope->classes.begin();
           citer != scope->classes.end(); ++ citer ) {
	    class_list_गणनम् += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_गणनम्*परिमाण(class_type **)));
	    class_list[class_list_गणनम्-1] = citer->second;
      }
}

शून्य मूलम्_table_delete(शून्य)
{
      अत्थाय (unsigned idx = 0; idx < vpip_मूलम्_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_मूलम्_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    delete scope;
      }
      vpip_मूलम्_table.clear();

	/* Clean up all the class definitions. */
      अत्थाय (unsigned idx = 0; idx < class_list_गणनम्; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      मुक्त(class_list);
      class_list = 0;
      class_list_गणनम् = 0;
}
#endif

स्थिर शून्य construct_scope_fullname(__vpiScope*ref, वर्ण*buf)
{
      सचे (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    सचे (ref->scope->get_type_code() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      सचे (ref->get_type_code() == vpiPackage) strcat(buf, "::");
}

स्थिर ध्रुव वर्ण* scope_get_type(पूर्णाङ्क code)
{
      विकल्प (code) {
          पक्ष vpiModule:
            पटिनिवत्त "vpiModule";
          पक्ष vpiGenScope:
            पटिनिवत्त "vpiGenScope";
          पक्ष vpiFunction:
            पटिनिवत्त "vpiFunction";
          पक्ष vpiTask:
            पटिनिवत्त "vpiTask";
          पक्ष vpiNamedBegin:
            पटिनिवत्त "vpiNamedBegin";
          पक्ष vpiNamedFork:
            पटिनिवत्त "vpiNamedFork";
          स्वभाव:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            पटिनिवत्त रिक्त;
      }
}

स्थिर वर्ण* scope_get_str(पूर्णाङ्क code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      प्रतिज्ञा(ref);

      वर्ण buf[4096];  // XXX is a fixed buffer size really reliable?
      ध्रुव वर्ण *p=0;
      विकल्प (code) {
	  पक्ष vpiDefFile:
	    p = file_names[ref->def_file_idx];
	    भङ्ग;

	  पक्ष vpiFile:
	    p = file_names[ref->file_idx];
	    भङ्ग;

	  पक्ष vpiFullName:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    भङ्ग;

	  पक्ष vpiName:
	    p = ref->scope_name();
	    भङ्ग;

	  पक्ष vpiDefName:
	    p = ref->scope_def_name();
	    भङ्ग;

	  पक्ष vpiType:
	    p = scope_get_type(code);
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    पटिनिवत्त रिक्त;
      }
      पटिनिवत्त simple_set_rbuf_str(p);
}

स्थिर vpiHandle scope_get_handle(पूर्णाङ्क code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      प्रतिज्ञा(rfp);

      विकल्प (code) {

	  पक्ष vpiScope:
	    पटिनिवत्त rfp->scope;

	  पक्ष vpiModule:
	    पटिनिवत्त rfp->scope;
      }

      पटिनिवत्त 0;
}

/* compares vpiType's considering object classes */
स्थिर पूर्णाङ्क compare_types(पूर्णाङ्क code, पूर्णाङ्क type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      सचे (code == vpiScope)
	    पटिनिवत्त 1;

      सचे (code == type)
	    पटिनिवत्त 1;

      सचे ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    पटिनिवत्त 1;

      सचे ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    पटिनिवत्त 1;

      सचे ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    पटिनिवत्त 1;

      पटिनिवत्त 0;
}

स्थिर vpiHandle make_subset_iterator_(पूर्णाङ्क type_code, ध्रुव vector<vpiHandle>&table)
{
      unsigned mcnt = 0, ncnt = 0;
      vpiHandle*args;

      अत्थाय (unsigned idx = 0; idx < table.size(); idx += 1)
	    सचे (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      सचे (mcnt == 0)
	    पटिनिवत्त 0;

      args = static_cast<vpiHandle*>(calloc(mcnt, परिमाण(vpiHandle)));
      अत्थाय (unsigned idx = 0; idx < table.size(); idx += 1)
	    सचे (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      प्रतिज्ञा(ncnt == mcnt);

      पटिनिवत्त vpip_make_iterator(mcnt, args, सच्च);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
स्थिर vpiHandle module_iter(पूर्णाङ्क code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      प्रतिज्ञा(ref);

      पटिनिवत्त make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(ध्रुव वर्ण*nam, ध्रुव वर्ण*tnam, तार्किक auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

पूर्णाङ्क __vpiScope::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiCellInstance:
	    पटिनिवत्त is_cell? 1 : 0;

	  पक्ष vpi_Def_पङ्क्तिः_No:
	    पटिनिवत्त def_lineno;

	  पक्ष vpi_पङ्क्तिः_No:
	    पटिनिवत्त lineno;

	  पक्ष vpiTimeUnit:
	    पटिनिवत्त time_units;

	  पक्ष vpiTimePrecision:
	    पटिनिवत्त time_precision;

	  पक्ष vpi_शिखरम्_Module:
	    पटिनिवत्त 0x0 == scope;

          पक्ष vpiAutomatic:
	    पटिनिवत्त is_automatic_? 1 : 0;
      }

      पटिनिवत्त vpiUndefined;
}


वर्ण*__vpiScope::vpi_get_str(पूर्णाङ्क code)
{ पटिनिवत्त scope_get_str(code, एतत्); }

vpiHandle __vpiScope::vpi_handle(पूर्णाङ्क code)
{ पटिनिवत्त scope_get_handle(code, एतत्); }

vpiHandle __vpiScope::vpi_iterate(पूर्णाङ्क code)
{ पटिनिवत्त module_iter(code, एतत्); }


वग्ग vpiScopeModule  : सार्वजनिक __vpiScope {
    सार्वजनिक:
      अन्तःस्थ vpiScopeModule(ध्रुव वर्ण*nam, ध्रुव वर्ण*tnam)
      : __vpiScope(nam,tnam,मिच्छा) { }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiModule; }
};

संरचना vpiScopePackage  : सार्वजनिक __vpiScope {
      अन्तःस्थ vpiScopePackage(ध्रुव वर्ण*nam, ध्रुव वर्ण*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiPackage; }
};

संरचना vpiScopeTask  : सार्वजनिक __vpiScope {
      अन्तःस्थ vpiScopeTask(ध्रुव वर्ण*nam, ध्रुव वर्ण*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiTask; }
};

संरचना vpiScopeTaskAuto  : सार्वजनिक __vpiScope {
      अन्तःस्थ vpiScopeTaskAuto(ध्रुव वर्ण*nam, ध्रुव वर्ण*tnam)
      : __vpiScope(nam,tnam,सच्च) {  }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiTask; }
};

संरचना vpiScopeBegin  : सार्वजनिक __vpiScope {
      अन्तःस्थ vpiScopeBegin(ध्रुव वर्ण*nam, ध्रुव वर्ण*tnam)
      : __vpiScope(nam,tnam,मिच्छा) { }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiNamedBegin; }
};

वग्ग vpiScopeBeginAuto  : सार्वजनिक __vpiScope {
    सार्वजनिक:
      अन्तःस्थ vpiScopeBeginAuto(ध्रुव वर्ण*nam, ध्रुव वर्ण*tnam)
      : __vpiScope(nam,tnam,सच्च) {  }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiNamedBegin; }
};

संरचना vpiScopeGenerate  : सार्वजनिक __vpiScope {
      अन्तःस्थ vpiScopeGenerate(ध्रुव वर्ण*nam, ध्रुव वर्ण*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiGenScope; }
};

संरचना vpiScopeFork  : सार्वजनिक __vpiScope {
      अन्तःस्थ vpiScopeFork(ध्रुव वर्ण*nam, ध्रुव वर्ण*tnam)
      : __vpiScope(nam,tnam,मिच्छा) { }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiNamedFork; }
};

वग्ग vpiScopeForkAuto  : सार्वजनिक __vpiScope {
    सार्वजनिक:
      अन्तःस्थ vpiScopeForkAuto(ध्रुव वर्ण*nam, ध्रुव वर्ण*tnam)
      : __vpiScope(nam,tnam,सच्च) {  }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiNamedFork; }
};

संरचना vpiScopeClass  : सार्वजनिक __vpiScope {
      अन्तःस्थ vpiScopeClass(ध्रुव वर्ण*nam, ध्रुव वर्ण*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiClassTypespec; }
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
compile_scope_decl(वर्ण*label, वर्ण*type, वर्ण*name, वर्ण*tname,
                   वर्ण*parent, long file_idx, long lineno,
                   long def_file_idx, long def_lineno, long is_cell)
{
      गणनम्_vpi_scopes += 1;
      वर्ण vec_type;
      वर्ण sign_flag;
      unsigned wid;

      __vpiScope*scope;
      सचे (strcmp(type,"module") == 0) {
	    scope = नूतन vpiScopeModule(name, tname);
      } अञ्ञथा सचे ( sscanf(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    पूर्णाङ्क type_code;
	    सचे (sign_flag=='s') {
		  type_code = vpiSizedSignedFunc;
	    } अञ्ञथा सचे (sign_flag=='u') {
		  type_code = vpiSizedFunc;
	    } अञ्ञथा सचे (sign_flag=='i') {
		  type_code = vpiIntFunc;
	    } अञ्ञथा {
		  प्रतिज्ञा(0);
		  type_code = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = नूतन vpiScopeFunction(name, tname, मिच्छा, type_code, wid, init_val);

      } अञ्ञथा सचे ( sscanf(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    पूर्णाङ्क type_code;
	    विकल्प (sign_flag) {
		पक्ष 's':
		  type_code = vpiSizedSignedFunc;
		  भङ्ग;
		पक्ष 'u':
		  type_code = vpiSizedFunc;
		  भङ्ग;
		स्वभाव:
		  प्रतिज्ञा(0);
		  type_code = vpiSizedFunc;
		  भङ्ग;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = नूतन vpiScopeFunction(name, tname, सच्च, type_code, wid, init_val);

      } अञ्ञथा सचे (strcmp(type,"function.obj") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, मिच्छा, vpiSizedFunc, 0, BIT4_0);
      } अञ्ञथा सचे (strcmp(type,"autofunction.obj") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, सच्च, vpiSizedFunc, 0, BIT4_0);
      } अञ्ञथा सचे (strcmp(type,"function.real") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, मिच्छा, vpiRealFunc, 0, BIT4_0);
      } अञ्ञथा सचे (strcmp(type,"autofunction.real") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, सच्च, vpiRealFunc, 0, BIT4_0);
      } अञ्ञथा सचे (strcmp(type,"function.str") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, मिच्छा, vpiOtherFunc, 0, BIT4_0);
      } अञ्ञथा सचे (strcmp(type,"autofunction.str") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, सच्च, vpiOtherFunc, 0, BIT4_0);
      } अञ्ञथा सचे (strcmp(type,"function.void") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, मिच्छा, vpiOtherFunc, 0, BIT4_0);
      } अञ्ञथा सचे (strcmp(type,"autofunction.void") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, सच्च, vpiOtherFunc, 0, BIT4_0);
      } अञ्ञथा सचे (strcmp(type,"task") == 0) {
	    scope = नूतन vpiScopeTask(name, tname);
      } अञ्ञथा सचे (strcmp(type,"autotask") == 0) {
	    scope = नूतन vpiScopeTaskAuto(name, tname);
      } अञ्ञथा सचे (strcmp(type,"fork") == 0) {
	    scope = नूतन vpiScopeFork(name, tname);
      } अञ्ञथा सचे (strcmp(type,"autofork") == 0) {
	    scope = नूतन vpiScopeForkAuto(name, tname);
      } अञ्ञथा सचे (strcmp(type,"begin") == 0) {
	    scope = नूतन vpiScopeBegin(name, tname);
      } अञ्ञथा सचे (strcmp(type,"autobegin") == 0) {
	    scope = नूतन vpiScopeBeginAuto(name, tname);
      } अञ्ञथा सचे (strcmp(type,"generate") == 0) {
	    scope = नूतन vpiScopeGenerate(name, tname);
      } अञ्ञथा सचे (strcmp(type,"package") == 0) {
	    scope = नूतन vpiScopePackage(name, tname);
      } अञ्ञथा सचे (strcmp(type,"class") == 0) {
	    scope = नूतन vpiScopeClass(name, tname);
      } अञ्ञथा {
	    scope = नूतन vpiScopeModule(name, tname);
	    प्रतिज्ञा(0);
      }

      scope->file_idx = (unsigned) file_idx;
      scope->lineno  = (unsigned) lineno;
      scope->def_file_idx = (unsigned) def_file_idx;
      scope->def_lineno  = (unsigned) def_lineno;
      scope->item = 0;
      scope->nitem = 0;
      scope->live_contexts = 0;
      scope->free_contexts = 0;

      सचे (is_cell) scope->is_cell = सच्च;
      अञ्ञथा scope->is_cell = मिच्छा;

      current_scope = scope;

      compile_vpi_चिह्नम्(label, scope);

      मुक्त(label);
      मुक्त(type);
      delete[] name;
      delete[] tname;

      सचे (parent) {
	    स्थिर vpiHandle obj;
	    compile_vpi_lookup(&obj, parent);
	    प्रतिज्ञा(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_to_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->time_units = sp->time_units;
	    scope->time_precision = sp->time_precision;

      } अञ्ञथा {
	    scope->scope = 0x0;

	    vpip_मूलम्_table.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->time_units = vpip_get_time_precision();
	    scope->time_precision = vpip_get_time_precision();
      }
}

शून्य compile_scope_recall(वर्ण*चिह्नम्)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(शून्य*)&current_scope, चिह्नम्);
      प्रतिज्ञा(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
शून्य compile_timescale(long units, long precision)
{
      प्रतिज्ञा(current_scope);
      current_scope->time_units = units;
      current_scope->time_precision = precision;
}

__vpiScope* vpip_peek_current_scope(शून्य)
{
      पटिनिवत्त current_scope;
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
      याव (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      पटिनिवत्त scope;
}

unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      प्रतिज्ञा(scope);
      प्रतिज्ञा(scope->is_automatic());

      unsigned idx = scope->nitem++;

      सचे (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (malloc(परिमाण(automatic_hooks_s*)));
      अञ्ञथा
	    scope->item = static_cast<automatic_hooks_s**>
	                  (realloc(scope->item,
	                   परिमाण(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      पटिनिवत्त 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              unsigned index,
              पूर्णाङ्क vpi_direction,
              unsigned width,
              ध्रुव वर्ण *name,
              वर्ण* buffer) :
      parent_(parent),
      index_(index),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      सचे (buffer != nullptr) functor_ref_lookup(&ref_, buffer);
      अञ्ञथा ref_ = nullptr;
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

पूर्णाङ्क vpiPortInfo::vpi_get(पूर्णाङ्क code)
{
      विकल्प( code ) {

        पक्ष vpiDirection :
          पटिनिवत्त direction_;
        पक्ष vpiPortIndex :
          पटिनिवत्त index_;
        पक्ष vpiSize :
          पटिनिवत्त width_;
        स्वभाव :
          पटिनिवत्त vpiUndefined;
      }

}


वर्ण *vpiPortInfo::vpi_get_str(पूर्णाङ्क code)
{
      विकल्प( code ) {
        पक्ष vpiName :
          पटिनिवत्त simple_set_rbuf_str(name_);
        स्वभाव :
          पटिनिवत्त रिक्त;
      }

}


vpiHandle vpiPortInfo::vpi_handle(पूर्णाङ्क code)
{

      विकल्प (code) {

          पक्ष vpiParent:
          पक्ष vpiScope:
          पक्ष vpiModule:
            पटिनिवत्त parent_;
          स्वभाव :
            भङ्ग;
      }

      पटिनिवत्त 0;
}

स्थिर vpiHandle portinfo_iterate(पूर्णाङ्क code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      प्रतिज्ञा(rfp);
      unsigned width = rfp->get_width();

      विकल्प (code) {
	  पक्ष vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(width, परिमाण(vpiHandle*)));

	    अत्थाय (unsigned i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    पटिनिवत्त vpip_make_iterator(width, args, सच्च);
	  }
      }
      पटिनिवत्त 0;
}

vpiHandle vpiPortInfo::vpi_iterate(पूर्णाङ्क code)
{
      पटिनिवत्त portinfo_iterate(code, एतत्);
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
शून्य port_bit_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortBitInfo *>(handle);
}
#endif

vpiHandle vpiPortBitInfo::vpi_handle(पूर्णाङ्क code)
{

      विकल्प (code) {
          पक्ष vpiParent:
            पटिनिवत्त parent_;
          स्वभाव :
            भङ्ग;
      }

      पटिनिवत्त 0;
}

पूर्णाङ्क vpiPortBitInfo::vpi_get(पूर्णाङ्क code)
{
      विकल्प( code ) {
        पक्ष vpiBit :
          पटिनिवत्त bit_;
        स्वभाव :
          भङ्ग;
      }

      पटिनिवत्त 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
शून्य compile_port_info( unsigned index, पूर्णाङ्क vpi_direction, unsigned width, ध्रुव वर्ण *name, वर्ण* buffer )
{
      vpiPortInfo* obj = नूतन vpiPortInfo( vpip_peek_current_scope(),
                                          index, vpi_direction, width, name, buffer );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      अत्थाय (unsigned i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = नूतन vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
