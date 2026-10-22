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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#पूर्ण_अगर
# include  <vector>
# include  <cstring>
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"

using namespace std;

अटल vector<vpiHandle> vpip_मूल_table;

अटल vpiHandle make_subset_iterator_(पूर्णांक type_code, स्थिर vector<vpiHandle>&table);

vpiHandle vpip_make_मूल_iterator(पूर्णांक type_code)
{
      वापस make_subset_iterator_(type_code, vpip_मूल_table);
}

व्योम vpip_make_मूल_iterator(__vpiHandle**&table, अचिन्हित&ntable)
{
      table = &vpip_मूल_table[0];
      ntable = vpip_मूल_table.size();
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम port_delete(__vpiHandle*handle);
व्योम port_bit_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
अटल class_type **class_list = 0;
अटल अचिन्हित class_list_गिनती = 0;

अटल व्योम delete_sub_scopes(__vpiScope *scope)
{
      क्रम (अचिन्हित idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle item = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(item);
	    चयन(item->get_type_code()) {
		हाल vpiFunction:
		हाल vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		हाल vpiModule:
		हाल vpiGenScope:
		हाल vpiNamedBegin:
		हाल vpiNamedFork:
		हाल vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  मिटाओ item;
		  अवरोध;
		हाल vpiMemory:
		हाल vpiNetArray:
		  memory_delete(item);
		  अवरोध;
		हाल vpiModPath:
		    /* The destination ModPath is cleaned up later. */
		  मिटाओ item;
		  अवरोध;
		हाल vpiInterModPath:
		  मिटाओ item;
		  अवरोध;
		हाल vpiNamedEvent:
		  named_event_delete(item);
		  अवरोध;
		हाल vpiNet:
		हाल vpiReg:
		हाल vpiIntegerVar:
		हाल vpiLongIntVar:
		हाल vpiShortIntVar:
		हाल vpiIntVar:
		हाल vpiByteVar:
		हाल vpiBitVar:
		  signal_delete(item);
		  अवरोध;
		हाल vpiParameter:
		  parameter_delete(item);
		  अवरोध;
		हाल vpiRealVar:
		  real_delete(item);
		  अवरोध;
		हाल vpiEnumTypespec:
		  enum_delete(item);
		  अवरोध;
		हाल vpiPort:
		  port_delete(item);
		  अवरोध;
		हाल vpiPortBit:
		  port_bit_delete(item);
		  अवरोध;
		हाल vpiStringVar:
		  string_delete(item);
		  अवरोध;
		हाल vpiClassVar:
		  class_delete(item);
		  अवरोध;
		हाल vpiArrayVar:
		  array_delete(item);
		  अवरोध;
		शेष:
		  ख_लिखो(मानक_त्रुटि, "Need support for type: %d\n",
		          item->get_type_code());
		  निश्चित(0);
		  अवरोध;
	    }
      }
      scope->intern.सफा();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      क्रम (citer = scope->classes.begin();
           citer != scope->classes.end(); ++ citer ) {
	    class_list_गिनती += 1;
	    class_list = static_cast<class_type **>
	                 (पुनः_स्मृति(class_list,
	                  class_list_गिनती*माप(class_type **)));
	    class_list[class_list_गिनती-1] = citer->second;
      }
}

व्योम मूल_table_delete(व्योम)
{
      क्रम (अचिन्हित idx = 0; idx < vpip_मूल_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_मूल_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    मिटाओ scope;
      }
      vpip_मूल_table.सफा();

	/* Clean up all the class definitions. */
      क्रम (अचिन्हित idx = 0; idx < class_list_गिनती; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      मुक्त(class_list);
      class_list = 0;
      class_list_गिनती = 0;
}
#पूर्ण_अगर

अटल व्योम construct_scope_fullname(__vpiScope*ref, अक्षर*buf)
{
      अगर (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    अगर (ref->scope->get_type_code() != vpiPackage) {
		  म_जोड़ो(buf, ".");
	    }
      }

      म_जोड़ो(buf, ref->scope_name());
	// For a package, add a "::" to the end
      अगर (ref->get_type_code() == vpiPackage) म_जोड़ो(buf, "::");
}

अटल स्थिर अक्षर* scope_get_type(पूर्णांक code)
{
      चयन (code) {
          हाल vpiModule:
            वापस "vpiModule";
          हाल vpiGenScope:
            वापस "vpiGenScope";
          हाल vpiFunction:
            वापस "vpiFunction";
          हाल vpiTask:
            वापस "vpiTask";
          हाल vpiNamedBegin:
            वापस "vpiNamedBegin";
          हाल vpiNamedFork:
            वापस "vpiNamedFork";
          शेष:
            ख_लिखो(मानक_त्रुटि, "VPI error: invalid scope type code %d.\n", code);
            वापस शून्य;
      }
}

अटल अक्षर* scope_get_str(पूर्णांक code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      निश्चित(ref);

      अक्षर buf[4096];  // XXX is a fixed buffer size really reliable?
      स्थिर अक्षर *p=0;
      चयन (code) {
	  हाल vpiDefFile:
	    p = file_names[ref->def_file_idx];
	    अवरोध;

	  हाल vpiFile:
	    p = file_names[ref->file_idx];
	    अवरोध;

	  हाल vpiFullName:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    अवरोध;

	  हाल vpiName:
	    p = ref->scope_name();
	    अवरोध;

	  हाल vpiDefName:
	    p = ref->scope_def_name();
	    अवरोध;

	  हाल vpiType:
	    p = scope_get_type(code);
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "VPI error: invalid scope string code %d.\n", code);
	    वापस शून्य;
      }
      वापस simple_set_rbuf_str(p);
}

अटल vpiHandle scope_get_handle(पूर्णांक code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      निश्चित(rfp);

      चयन (code) {

	  हाल vpiScope:
	    वापस rfp->scope;

	  हाल vpiModule:
	    वापस rfp->scope;
      }

      वापस 0;
}

/* compares vpiType's considering object classes */
अटल पूर्णांक compare_types(पूर्णांक code, पूर्णांक type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      अगर (code == vpiScope)
	    वापस 1;

      अगर (code == type)
	    वापस 1;

      अगर ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    वापस 1;

      अगर ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    वापस 1;

      अगर ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    वापस 1;

      वापस 0;
}

अटल vpiHandle make_subset_iterator_(पूर्णांक type_code, स्थिर vector<vpiHandle>&table)
{
      अचिन्हित mcnt = 0, ncnt = 0;
      vpiHandle*args;

      क्रम (अचिन्हित idx = 0; idx < table.size(); idx += 1)
	    अगर (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      अगर (mcnt == 0)
	    वापस 0;

      args = static_cast<vpiHandle*>(सुस्मृति(mcnt, माप(vpiHandle)));
      क्रम (अचिन्हित idx = 0; idx < table.size(); idx += 1)
	    अगर (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      निश्चित(ncnt == mcnt);

      वापस vpip_make_iterator(mcnt, args, true);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
अटल vpiHandle module_iter(पूर्णांक code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      निश्चित(ref);

      वापस make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(स्थिर अक्षर*nam, स्थिर अक्षर*tnam, bool auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

पूर्णांक __vpiScope::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiCellInstance:
	    वापस is_cell? 1 : 0;

	  हाल vpi_Def_पंक्ति_No:
	    वापस def_lineno;

	  हाल vpi_पंक्ति_No:
	    वापस lineno;

	  हाल vpiTimeUnit:
	    वापस time_units;

	  हाल vpiTimePrecision:
	    वापस time_precision;

	  हाल vpi_शीर्ष_Module:
	    वापस 0x0 == scope;

          हाल vpiAutomatic:
	    वापस is_automatic_? 1 : 0;
      }

      वापस vpiUndefined;
}


अक्षर*__vpiScope::vpi_get_str(पूर्णांक code)
{ वापस scope_get_str(code, यह); }

vpiHandle __vpiScope::vpi_handle(पूर्णांक code)
{ वापस scope_get_handle(code, यह); }

vpiHandle __vpiScope::vpi_iterate(पूर्णांक code)
{ वापस module_iter(code, यह); }


श्रेणी vpiScopeModule  : खुला __vpiScope {
    खुला:
      अंतरभूत vpiScopeModule(स्थिर अक्षर*nam, स्थिर अक्षर*tnam)
      : __vpiScope(nam,tnam,false) { }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiModule; }
};

काष्ठा vpiScopePackage  : खुला __vpiScope {
      अंतरभूत vpiScopePackage(स्थिर अक्षर*nam, स्थिर अक्षर*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiPackage; }
};

काष्ठा vpiScopeTask  : खुला __vpiScope {
      अंतरभूत vpiScopeTask(स्थिर अक्षर*nam, स्थिर अक्षर*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiTask; }
};

काष्ठा vpiScopeTaskAuto  : खुला __vpiScope {
      अंतरभूत vpiScopeTaskAuto(स्थिर अक्षर*nam, स्थिर अक्षर*tnam)
      : __vpiScope(nam,tnam,true) {  }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiTask; }
};

काष्ठा vpiScopeBegin  : खुला __vpiScope {
      अंतरभूत vpiScopeBegin(स्थिर अक्षर*nam, स्थिर अक्षर*tnam)
      : __vpiScope(nam,tnam,false) { }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiNamedBegin; }
};

श्रेणी vpiScopeBeginAuto  : खुला __vpiScope {
    खुला:
      अंतरभूत vpiScopeBeginAuto(स्थिर अक्षर*nam, स्थिर अक्षर*tnam)
      : __vpiScope(nam,tnam,true) {  }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiNamedBegin; }
};

काष्ठा vpiScopeGenerate  : खुला __vpiScope {
      अंतरभूत vpiScopeGenerate(स्थिर अक्षर*nam, स्थिर अक्षर*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiGenScope; }
};

काष्ठा vpiScopeFork  : खुला __vpiScope {
      अंतरभूत vpiScopeFork(स्थिर अक्षर*nam, स्थिर अक्षर*tnam)
      : __vpiScope(nam,tnam,false) { }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiNamedFork; }
};

श्रेणी vpiScopeForkAuto  : खुला __vpiScope {
    खुला:
      अंतरभूत vpiScopeForkAuto(स्थिर अक्षर*nam, स्थिर अक्षर*tnam)
      : __vpiScope(nam,tnam,true) {  }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiNamedFork; }
};

काष्ठा vpiScopeClass  : खुला __vpiScope {
      अंतरभूत vpiScopeClass(स्थिर अक्षर*nam, स्थिर अक्षर*tnam)
      : __vpiScope(nam,tnam) { }
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
अटल __vpiScope*current_scope = 0;

व्योम vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj)
{
      निश्चित(scope);
      scope->intern.push_back(obj);
}

/*
 * When the compiler encounters a scope declaration, this function
 * creates and initializes a __vpiScope object with the requested name
 * and within the addressed parent. The label is used as a key in the
 * symbol table and the name is used to construct the actual object.
 */
व्योम
compile_scope_decl(अक्षर*label, अक्षर*type, अक्षर*name, अक्षर*tname,
                   अक्षर*parent, दीर्घ file_idx, दीर्घ lineno,
                   दीर्घ def_file_idx, दीर्घ def_lineno, दीर्घ is_cell)
{
      गिनती_vpi_scopes += 1;
      अक्षर vec_type;
      अक्षर sign_flag;
      अचिन्हित wid;

      __vpiScope*scope;
      अगर (म_भेद(type,"module") == 0) {
	    scope = नया vpiScopeModule(name, tname);
      } अन्यथा अगर ( माला_पूछो(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    पूर्णांक type_code;
	    अगर (sign_flag=='s') {
		  type_code = vpiSizedSignedFunc;
	    } अन्यथा अगर (sign_flag=='u') {
		  type_code = vpiSizedFunc;
	    } अन्यथा अगर (sign_flag=='i') {
		  type_code = vpiIntFunc;
	    } अन्यथा {
		  निश्चित(0);
		  type_code = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = नया vpiScopeFunction(name, tname, false, type_code, wid, init_val);

      } अन्यथा अगर ( माला_पूछो(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    पूर्णांक type_code;
	    चयन (sign_flag) {
		हाल 's':
		  type_code = vpiSizedSignedFunc;
		  अवरोध;
		हाल 'u':
		  type_code = vpiSizedFunc;
		  अवरोध;
		शेष:
		  निश्चित(0);
		  type_code = vpiSizedFunc;
		  अवरोध;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = नया vpiScopeFunction(name, tname, true, type_code, wid, init_val);

      } अन्यथा अगर (म_भेद(type,"function.obj") == 0) {
	    scope = नया vpiScopeFunction(name, tname, false, vpiSizedFunc, 0, BIT4_0);
      } अन्यथा अगर (म_भेद(type,"autofunction.obj") == 0) {
	    scope = नया vpiScopeFunction(name, tname, true, vpiSizedFunc, 0, BIT4_0);
      } अन्यथा अगर (म_भेद(type,"function.real") == 0) {
	    scope = नया vpiScopeFunction(name, tname, false, vpiRealFunc, 0, BIT4_0);
      } अन्यथा अगर (म_भेद(type,"autofunction.real") == 0) {
	    scope = नया vpiScopeFunction(name, tname, true, vpiRealFunc, 0, BIT4_0);
      } अन्यथा अगर (म_भेद(type,"function.str") == 0) {
	    scope = नया vpiScopeFunction(name, tname, false, vpiOtherFunc, 0, BIT4_0);
      } अन्यथा अगर (म_भेद(type,"autofunction.str") == 0) {
	    scope = नया vpiScopeFunction(name, tname, true, vpiOtherFunc, 0, BIT4_0);
      } अन्यथा अगर (म_भेद(type,"function.void") == 0) {
	    scope = नया vpiScopeFunction(name, tname, false, vpiOtherFunc, 0, BIT4_0);
      } अन्यथा अगर (म_भेद(type,"autofunction.void") == 0) {
	    scope = नया vpiScopeFunction(name, tname, true, vpiOtherFunc, 0, BIT4_0);
      } अन्यथा अगर (म_भेद(type,"task") == 0) {
	    scope = नया vpiScopeTask(name, tname);
      } अन्यथा अगर (म_भेद(type,"autotask") == 0) {
	    scope = नया vpiScopeTaskAuto(name, tname);
      } अन्यथा अगर (म_भेद(type,"fork") == 0) {
	    scope = नया vpiScopeFork(name, tname);
      } अन्यथा अगर (म_भेद(type,"autofork") == 0) {
	    scope = नया vpiScopeForkAuto(name, tname);
      } अन्यथा अगर (म_भेद(type,"begin") == 0) {
	    scope = नया vpiScopeBegin(name, tname);
      } अन्यथा अगर (म_भेद(type,"autobegin") == 0) {
	    scope = नया vpiScopeBeginAuto(name, tname);
      } अन्यथा अगर (म_भेद(type,"generate") == 0) {
	    scope = नया vpiScopeGenerate(name, tname);
      } अन्यथा अगर (म_भेद(type,"package") == 0) {
	    scope = नया vpiScopePackage(name, tname);
      } अन्यथा अगर (म_भेद(type,"class") == 0) {
	    scope = नया vpiScopeClass(name, tname);
      } अन्यथा {
	    scope = नया vpiScopeModule(name, tname);
	    निश्चित(0);
      }

      scope->file_idx = (अचिन्हित) file_idx;
      scope->lineno  = (अचिन्हित) lineno;
      scope->def_file_idx = (अचिन्हित) def_file_idx;
      scope->def_lineno  = (अचिन्हित) def_lineno;
      scope->item = 0;
      scope->nitem = 0;
      scope->live_contexts = 0;
      scope->free_contexts = 0;

      अगर (is_cell) scope->is_cell = true;
      अन्यथा scope->is_cell = false;

      current_scope = scope;

      compile_vpi_प्रतीक(label, scope);

      मुक्त(label);
      मुक्त(type);
      मिटाओ[] name;
      मिटाओ[] tname;

      अगर (parent) {
	    अटल vpiHandle obj;
	    compile_vpi_lookup(&obj, parent);
	    निश्चित(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_to_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->time_units = sp->time_units;
	    scope->time_precision = sp->time_precision;

      } अन्यथा {
	    scope->scope = 0x0;

	    vpip_मूल_table.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->time_units = vpip_get_time_precision();
	    scope->time_precision = vpip_get_time_precision();
      }
}

व्योम compile_scope_recall(अक्षर*प्रतीक)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(व्योम*)&current_scope, प्रतीक);
      निश्चित(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
व्योम compile_timescale(दीर्घ units, दीर्घ precision)
{
      निश्चित(current_scope);
      current_scope->time_units = units;
      current_scope->time_precision = precision;
}

__vpiScope* vpip_peek_current_scope(व्योम)
{
      वापस current_scope;
}

व्योम vpip_attach_to_current_scope(vpiHandle obj)
{
      vpip_attach_to_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(व्योम)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      जबतक (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      वापस scope;
}

अचिन्हित vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      निश्चित(scope);
      निश्चित(scope->is_automatic());

      अचिन्हित idx = scope->nitem++;

      अगर (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (दो_स्मृति(माप(automatic_hooks_s*)));
      अन्यथा
	    scope->item = static_cast<automatic_hooks_s**>
	                  (पुनः_स्मृति(scope->item,
	                   माप(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      वापस 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              अचिन्हित सूचकांक,
              पूर्णांक vpi_direction,
              अचिन्हित width,
              स्थिर अक्षर *name,
              अक्षर* बफर) :
      parent_(parent),
      सूचकांक_2(सूचकांक),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      अगर (बफर != nullptr) functor_ref_lookup(&ref_, बफर);
      अन्यथा ref_ = nullptr;
}

vpiPortInfo::~vpiPortInfo()
{
      मिटाओ[] name_;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम port_delete(__vpiHandle *handle)
{
      मिटाओ dynamic_cast<vpiPortInfo *>(handle);
}
#पूर्ण_अगर

पूर्णांक vpiPortInfo::vpi_get(पूर्णांक code)
{
      चयन( code ) {

        हाल vpiDirection :
          वापस direction_;
        हाल vpi_Port_सूचकांक :
          वापस सूचकांक_2;
        हाल vpi_आकार :
          वापस width_;
        शेष :
          वापस vpiUndefined;
      }

}


अक्षर *vpiPortInfo::vpi_get_str(पूर्णांक code)
{
      चयन( code ) {
        हाल vpiName :
          वापस simple_set_rbuf_str(name_);
        शेष :
          वापस शून्य;
      }

}


vpiHandle vpiPortInfo::vpi_handle(पूर्णांक code)
{

      चयन (code) {

          हाल vpiParent:
          हाल vpiScope:
          हाल vpiModule:
            वापस parent_;
          शेष :
            अवरोध;
      }

      वापस 0;
}

अटल vpiHandle portinfo_iterate(पूर्णांक code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      निश्चित(rfp);
      अचिन्हित width = rfp->get_width();

      चयन (code) {
	  हाल vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(सुस्मृति(width, माप(vpiHandle*)));

	    क्रम (अचिन्हित i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    वापस vpip_make_iterator(width, args, true);
	  }
      }
      वापस 0;
}

vpiHandle vpiPortInfo::vpi_iterate(पूर्णांक code)
{
      वापस portinfo_iterate(code, यह);
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

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम port_bit_delete(__vpiHandle *handle)
{
      मिटाओ dynamic_cast<vpiPortBitInfo *>(handle);
}
#पूर्ण_अगर

vpiHandle vpiPortBitInfo::vpi_handle(पूर्णांक code)
{

      चयन (code) {
          हाल vpiParent:
            वापस parent_;
          शेष :
            अवरोध;
      }

      वापस 0;
}

पूर्णांक vpiPortBitInfo::vpi_get(पूर्णांक code)
{
      चयन( code ) {
        हाल vpiBit :
          वापस bit_;
        शेष :
          अवरोध;
      }

      वापस 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
व्योम compile_port_info( अचिन्हित सूचकांक, पूर्णांक vpi_direction, अचिन्हित width, स्थिर अक्षर *name, अक्षर* बफर )
{
      vpiPortInfo* obj = नया vpiPortInfo( vpip_peek_current_scope(),
                                          सूचकांक, vpi_direction, width, name, बफर );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      क्रम (अचिन्हित i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = नया vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
