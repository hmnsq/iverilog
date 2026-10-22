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

स्थिर vector<vpiHandle> vpip_जरा_table;

स्थिर vpiHandle make_subset_iterator_(पूर्णाङ्क type_code, अचर vector<vpiHandle>&table);

vpiHandle vpip_make_जरा_iterator(पूर्णाङ्क type_code)
{
      फर्काऊ make_subset_iterator_(type_code, vpip_जरा_table);
}

शून्य vpip_make_जरा_iterator(__vpiHandle**&table, अचिन्हित&ntable)
{
      table = &vpip_जरा_table[0];
      ntable = vpip_जरा_table.size();
}

#ifdef CHECK_WITH_VALGRIND
शून्य port_delete(__vpiHandle*handle);
शून्य port_bit_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
स्थिर class_type **class_list = 0;
स्थिर अचिन्हित class_list_गन्ती = 0;

स्थिर शून्य delete_sub_scopes(__vpiScope *scope)
{
      लागि (अचिन्हित idx = 0; idx < scope->intern.size(); idx += 1) {
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
		  रोक;
		पक्ष vpiMemory:
		पक्ष vpiNetArray:
		  memory_delete(item);
		  रोक;
		पक्ष vpiModPath:
		    /* The destination ModPath is cleaned up later. */
		  delete item;
		  रोक;
		पक्ष vpiInterModPath:
		  delete item;
		  रोक;
		पक्ष vpiNamedEvent:
		  named_event_delete(item);
		  रोक;
		पक्ष vpiNet:
		पक्ष vpiReg:
		पक्ष vpiIntegerVar:
		पक्ष vpiLongIntVar:
		पक्ष vpiShortIntVar:
		पक्ष vpiIntVar:
		पक्ष vpiByteVar:
		पक्ष vpiBitVar:
		  signal_delete(item);
		  रोक;
		पक्ष vpiParameter:
		  parameter_delete(item);
		  रोक;
		पक्ष vpiRealVar:
		  real_delete(item);
		  रोक;
		पक्ष vpiEnumTypespec:
		  enum_delete(item);
		  रोक;
		पक्ष vpiPort:
		  port_delete(item);
		  रोक;
		पक्ष vpiPortBit:
		  port_bit_delete(item);
		  रोक;
		पक्ष vpiStringVar:
		  string_delete(item);
		  रोक;
		पक्ष vpiClassVar:
		  class_delete(item);
		  रोक;
		पक्ष vpiArrayVar:
		  array_delete(item);
		  रोक;
		स्वभाव:
		  ख_लिखो(मानक_त्रुटि, "Need support for type: %d\n",
		          item->get_type_code());
		  प्रतिज्ञा(0);
		  रोक;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      लागि (citer = scope->classes.begin();
           citer != scope->classes.end(); ++ citer ) {
	    class_list_गन्ती += 1;
	    class_list = static_cast<class_type **>
	                 (पुनः_स्मृति(class_list,
	                  class_list_गन्ती*परिमाण(class_type **)));
	    class_list[class_list_गन्ती-1] = citer->second;
      }
}

शून्य जरा_table_delete(शून्य)
{
      लागि (अचिन्हित idx = 0; idx < vpip_जरा_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_जरा_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    delete scope;
      }
      vpip_जरा_table.clear();

	/* Clean up all the class definitions. */
      लागि (अचिन्हित idx = 0; idx < class_list_गन्ती; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      मुक्त(class_list);
      class_list = 0;
      class_list_गन्ती = 0;
}
#endif

स्थिर शून्य construct_scope_fullname(__vpiScope*ref, वर्ण*buf)
{
      यदि (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    यदि (ref->scope->get_type_code() != vpiPackage) {
		  म_जोड़ो(buf, ".");
	    }
      }

      म_जोड़ो(buf, ref->scope_name());
	// For a package, add a "::" to the end
      यदि (ref->get_type_code() == vpiPackage) म_जोड़ो(buf, "::");
}

स्थिर अचर वर्ण* scope_get_type(पूर्णाङ्क code)
{
      विकल्प (code) {
          पक्ष vpiModule:
            फर्काऊ "vpiModule";
          पक्ष vpiGenScope:
            फर्काऊ "vpiGenScope";
          पक्ष vpiFunction:
            फर्काऊ "vpiFunction";
          पक्ष vpiTask:
            फर्काऊ "vpiTask";
          पक्ष vpiNamedBegin:
            फर्काऊ "vpiNamedBegin";
          पक्ष vpiNamedFork:
            फर्काऊ "vpiNamedFork";
          स्वभाव:
            ख_लिखो(मानक_त्रुटि, "VPI error: invalid scope type code %d.\n", code);
            फर्काऊ रिक्त;
      }
}

स्थिर वर्ण* scope_get_str(पूर्णाङ्क code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      प्रतिज्ञा(ref);

      वर्ण buf[4096];  // XXX is a fixed buffer size really reliable?
      अचर वर्ण *p=0;
      विकल्प (code) {
	  पक्ष vpiDefFile:
	    p = file_names[ref->def_file_idx];
	    रोक;

	  पक्ष vpiFile:
	    p = file_names[ref->file_idx];
	    रोक;

	  पक्ष vpiFullName:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    रोक;

	  पक्ष vpiName:
	    p = ref->scope_name();
	    रोक;

	  पक्ष vpiDefName:
	    p = ref->scope_def_name();
	    रोक;

	  पक्ष vpiType:
	    p = scope_get_type(code);
	    रोक;

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "VPI error: invalid scope string code %d.\n", code);
	    फर्काऊ रिक्त;
      }
      फर्काऊ simple_set_rbuf_str(p);
}

स्थिर vpiHandle scope_get_handle(पूर्णाङ्क code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      प्रतिज्ञा(rfp);

      विकल्प (code) {

	  पक्ष vpiScope:
	    फर्काऊ rfp->scope;

	  पक्ष vpiModule:
	    फर्काऊ rfp->scope;
      }

      फर्काऊ 0;
}

/* compares vpiType's considering object classes */
स्थिर पूर्णाङ्क compare_types(पूर्णाङ्क code, पूर्णाङ्क type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      यदि (code == vpiScope)
	    फर्काऊ 1;

      यदि (code == type)
	    फर्काऊ 1;

      यदि ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    फर्काऊ 1;

      यदि ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    फर्काऊ 1;

      यदि ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    फर्काऊ 1;

      फर्काऊ 0;
}

स्थिर vpiHandle make_subset_iterator_(पूर्णाङ्क type_code, अचर vector<vpiHandle>&table)
{
      अचिन्हित mcnt = 0, ncnt = 0;
      vpiHandle*args;

      लागि (अचिन्हित idx = 0; idx < table.size(); idx += 1)
	    यदि (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      यदि (mcnt == 0)
	    फर्काऊ 0;

      args = static_cast<vpiHandle*>(सुस्मृति(mcnt, परिमाण(vpiHandle)));
      लागि (अचिन्हित idx = 0; idx < table.size(); idx += 1)
	    यदि (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      प्रतिज्ञा(ncnt == mcnt);

      फर्काऊ vpip_make_iterator(mcnt, args, सत्य);
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

      फर्काऊ make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(अचर वर्ण*nam, अचर वर्ण*tnam, तार्किक auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

पूर्णाङ्क __vpiScope::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiCellInstance:
	    फर्काऊ is_cell? 1 : 0;

	  पक्ष vpi_Def_पङ्क्ति_No:
	    फर्काऊ def_lineno;

	  पक्ष vpi_पङ्क्ति_No:
	    फर्काऊ lineno;

	  पक्ष vpiTimeUnit:
	    फर्काऊ time_units;

	  पक्ष vpiTimePrecision:
	    फर्काऊ time_precision;

	  पक्ष vpi_शीर्ष_Module:
	    फर्काऊ 0x0 == scope;

          पक्ष vpiAutomatic:
	    फर्काऊ is_automatic_? 1 : 0;
      }

      फर्काऊ vpiUndefined;
}


वर्ण*__vpiScope::vpi_get_str(पूर्णाङ्क code)
{ फर्काऊ scope_get_str(code, एतत्); }

vpiHandle __vpiScope::vpi_handle(पूर्णाङ्क code)
{ फर्काऊ scope_get_handle(code, एतत्); }

vpiHandle __vpiScope::vpi_iterate(पूर्णाङ्क code)
{ फर्काऊ module_iter(code, एतत्); }


वर्ग vpiScopeModule  : सार्वजनिक __vpiScope {
    सार्वजनिक:
      अन्तःस्थ vpiScopeModule(अचर वर्ण*nam, अचर वर्ण*tnam)
      : __vpiScope(nam,tnam,असत्य) { }
      पूर्णाङ्क get_type_code(शून्य) अचर override { फर्काऊ vpiModule; }
};

संरचना vpiScopePackage  : सार्वजनिक __vpiScope {
      अन्तःस्थ vpiScopePackage(अचर वर्ण*nam, अचर वर्ण*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णाङ्क get_type_code(शून्य) अचर override { फर्काऊ vpiPackage; }
};

संरचना vpiScopeTask  : सार्वजनिक __vpiScope {
      अन्तःस्थ vpiScopeTask(अचर वर्ण*nam, अचर वर्ण*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णाङ्क get_type_code(शून्य) अचर override { फर्काऊ vpiTask; }
};

संरचना vpiScopeTaskAuto  : सार्वजनिक __vpiScope {
      अन्तःस्थ vpiScopeTaskAuto(अचर वर्ण*nam, अचर वर्ण*tnam)
      : __vpiScope(nam,tnam,सत्य) {  }
      पूर्णाङ्क get_type_code(शून्य) अचर override { फर्काऊ vpiTask; }
};

संरचना vpiScopeBegin  : सार्वजनिक __vpiScope {
      अन्तःस्थ vpiScopeBegin(अचर वर्ण*nam, अचर वर्ण*tnam)
      : __vpiScope(nam,tnam,असत्य) { }
      पूर्णाङ्क get_type_code(शून्य) अचर override { फर्काऊ vpiNamedBegin; }
};

वर्ग vpiScopeBeginAuto  : सार्वजनिक __vpiScope {
    सार्वजनिक:
      अन्तःस्थ vpiScopeBeginAuto(अचर वर्ण*nam, अचर वर्ण*tnam)
      : __vpiScope(nam,tnam,सत्य) {  }
      पूर्णाङ्क get_type_code(शून्य) अचर override { फर्काऊ vpiNamedBegin; }
};

संरचना vpiScopeGenerate  : सार्वजनिक __vpiScope {
      अन्तःस्थ vpiScopeGenerate(अचर वर्ण*nam, अचर वर्ण*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णाङ्क get_type_code(शून्य) अचर override { फर्काऊ vpiGenScope; }
};

संरचना vpiScopeFork  : सार्वजनिक __vpiScope {
      अन्तःस्थ vpiScopeFork(अचर वर्ण*nam, अचर वर्ण*tnam)
      : __vpiScope(nam,tnam,असत्य) { }
      पूर्णाङ्क get_type_code(शून्य) अचर override { फर्काऊ vpiNamedFork; }
};

वर्ग vpiScopeForkAuto  : सार्वजनिक __vpiScope {
    सार्वजनिक:
      अन्तःस्थ vpiScopeForkAuto(अचर वर्ण*nam, अचर वर्ण*tnam)
      : __vpiScope(nam,tnam,सत्य) {  }
      पूर्णाङ्क get_type_code(शून्य) अचर override { फर्काऊ vpiNamedFork; }
};

संरचना vpiScopeClass  : सार्वजनिक __vpiScope {
      अन्तःस्थ vpiScopeClass(अचर वर्ण*nam, अचर वर्ण*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णाङ्क get_type_code(शून्य) अचर override { फर्काऊ vpiClassTypespec; }
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
                   वर्ण*parent, दीर्घ file_idx, दीर्घ lineno,
                   दीर्घ def_file_idx, दीर्घ def_lineno, दीर्घ is_cell)
{
      गन्ती_vpi_scopes += 1;
      वर्ण vec_type;
      वर्ण sign_flag;
      अचिन्हित wid;

      __vpiScope*scope;
      यदि (म_भेद(type,"module") == 0) {
	    scope = नूतन vpiScopeModule(name, tname);
      } अन्यथा यदि ( माला_पूछो(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    पूर्णाङ्क type_code;
	    यदि (sign_flag=='s') {
		  type_code = vpiSizedSignedFunc;
	    } अन्यथा यदि (sign_flag=='u') {
		  type_code = vpiSizedFunc;
	    } अन्यथा यदि (sign_flag=='i') {
		  type_code = vpiIntFunc;
	    } अन्यथा {
		  प्रतिज्ञा(0);
		  type_code = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = नूतन vpiScopeFunction(name, tname, असत्य, type_code, wid, init_val);

      } अन्यथा यदि ( माला_पूछो(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    पूर्णाङ्क type_code;
	    विकल्प (sign_flag) {
		पक्ष 's':
		  type_code = vpiSizedSignedFunc;
		  रोक;
		पक्ष 'u':
		  type_code = vpiSizedFunc;
		  रोक;
		स्वभाव:
		  प्रतिज्ञा(0);
		  type_code = vpiSizedFunc;
		  रोक;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = नूतन vpiScopeFunction(name, tname, सत्य, type_code, wid, init_val);

      } अन्यथा यदि (म_भेद(type,"function.obj") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, असत्य, vpiSizedFunc, 0, BIT4_0);
      } अन्यथा यदि (म_भेद(type,"autofunction.obj") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, सत्य, vpiSizedFunc, 0, BIT4_0);
      } अन्यथा यदि (म_भेद(type,"function.real") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, असत्य, vpiRealFunc, 0, BIT4_0);
      } अन्यथा यदि (म_भेद(type,"autofunction.real") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, सत्य, vpiRealFunc, 0, BIT4_0);
      } अन्यथा यदि (म_भेद(type,"function.str") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, असत्य, vpiOtherFunc, 0, BIT4_0);
      } अन्यथा यदि (म_भेद(type,"autofunction.str") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, सत्य, vpiOtherFunc, 0, BIT4_0);
      } अन्यथा यदि (म_भेद(type,"function.void") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, असत्य, vpiOtherFunc, 0, BIT4_0);
      } अन्यथा यदि (म_भेद(type,"autofunction.void") == 0) {
	    scope = नूतन vpiScopeFunction(name, tname, सत्य, vpiOtherFunc, 0, BIT4_0);
      } अन्यथा यदि (म_भेद(type,"task") == 0) {
	    scope = नूतन vpiScopeTask(name, tname);
      } अन्यथा यदि (म_भेद(type,"autotask") == 0) {
	    scope = नूतन vpiScopeTaskAuto(name, tname);
      } अन्यथा यदि (म_भेद(type,"fork") == 0) {
	    scope = नूतन vpiScopeFork(name, tname);
      } अन्यथा यदि (म_भेद(type,"autofork") == 0) {
	    scope = नूतन vpiScopeForkAuto(name, tname);
      } अन्यथा यदि (म_भेद(type,"begin") == 0) {
	    scope = नूतन vpiScopeBegin(name, tname);
      } अन्यथा यदि (म_भेद(type,"autobegin") == 0) {
	    scope = नूतन vpiScopeBeginAuto(name, tname);
      } अन्यथा यदि (म_भेद(type,"generate") == 0) {
	    scope = नूतन vpiScopeGenerate(name, tname);
      } अन्यथा यदि (म_भेद(type,"package") == 0) {
	    scope = नूतन vpiScopePackage(name, tname);
      } अन्यथा यदि (म_भेद(type,"class") == 0) {
	    scope = नूतन vpiScopeClass(name, tname);
      } अन्यथा {
	    scope = नूतन vpiScopeModule(name, tname);
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

      यदि (is_cell) scope->is_cell = सत्य;
      अन्यथा scope->is_cell = असत्य;

      current_scope = scope;

      compile_vpi_चिह्न(label, scope);

      मुक्त(label);
      मुक्त(type);
      delete[] name;
      delete[] tname;

      यदि (parent) {
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

	    vpip_जरा_table.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->time_units = vpip_get_time_precision();
	    scope->time_precision = vpip_get_time_precision();
      }
}

शून्य compile_scope_recall(वर्ण*चिह्न)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(शून्य*)&current_scope, चिह्न);
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
      फर्काऊ current_scope;
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
      जबसम्म (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      फर्काऊ scope;
}

अचिन्हित vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      प्रतिज्ञा(scope);
      प्रतिज्ञा(scope->is_automatic());

      अचिन्हित idx = scope->nitem++;

      यदि (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (दो_स्मृति(परिमाण(automatic_hooks_s*)));
      अन्यथा
	    scope->item = static_cast<automatic_hooks_s**>
	                  (पुनः_स्मृति(scope->item,
	                   परिमाण(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      फर्काऊ 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              अचिन्हित index,
              पूर्णाङ्क vpi_direction,
              अचिन्हित width,
              अचर वर्ण *name,
              वर्ण* buffer) :
      parent_(parent),
      index_(index),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      यदि (buffer != nullptr) functor_ref_lookup(&ref_, buffer);
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

पूर्णाङ्क vpiPortInfo::vpi_get(पूर्णाङ्क code)
{
      विकल्प( code ) {

        पक्ष vpiDirection :
          फर्काऊ direction_;
        पक्ष vpiPortIndex :
          फर्काऊ index_;
        पक्ष vpiSize :
          फर्काऊ width_;
        स्वभाव :
          फर्काऊ vpiUndefined;
      }

}


वर्ण *vpiPortInfo::vpi_get_str(पूर्णाङ्क code)
{
      विकल्प( code ) {
        पक्ष vpiName :
          फर्काऊ simple_set_rbuf_str(name_);
        स्वभाव :
          फर्काऊ रिक्त;
      }

}


vpiHandle vpiPortInfo::vpi_handle(पूर्णाङ्क code)
{

      विकल्प (code) {

          पक्ष vpiParent:
          पक्ष vpiScope:
          पक्ष vpiModule:
            फर्काऊ parent_;
          स्वभाव :
            रोक;
      }

      फर्काऊ 0;
}

स्थिर vpiHandle portinfo_iterate(पूर्णाङ्क code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      प्रतिज्ञा(rfp);
      अचिन्हित width = rfp->get_width();

      विकल्प (code) {
	  पक्ष vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(सुस्मृति(width, परिमाण(vpiHandle*)));

	    लागि (अचिन्हित i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    फर्काऊ vpip_make_iterator(width, args, सत्य);
	  }
      }
      फर्काऊ 0;
}

vpiHandle vpiPortInfo::vpi_iterate(पूर्णाङ्क code)
{
      फर्काऊ portinfo_iterate(code, एतत्);
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

vpiHandle vpiPortBitInfo::vpi_handle(पूर्णाङ्क code)
{

      विकल्प (code) {
          पक्ष vpiParent:
            फर्काऊ parent_;
          स्वभाव :
            रोक;
      }

      फर्काऊ 0;
}

पूर्णाङ्क vpiPortBitInfo::vpi_get(पूर्णाङ्क code)
{
      विकल्प( code ) {
        पक्ष vpiBit :
          फर्काऊ bit_;
        स्वभाव :
          रोक;
      }

      फर्काऊ 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
शून्य compile_port_info( अचिन्हित index, पूर्णाङ्क vpi_direction, अचिन्हित width, अचर वर्ण *name, वर्ण* buffer )
{
      vpiPortInfo* obj = नूतन vpiPortInfo( vpip_peek_current_scope(),
                                          index, vpi_direction, width, name, buffer );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      लागि (अचिन्हित i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = नूतन vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
