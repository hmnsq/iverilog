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

using ନାମାକାଶ std;

ସ୍ଥିର vector<vpiHandle> vpip_ମୂଳ_table;

ସ୍ଥିର vpiHandle make_subset_iterator_(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type_code, ଅଚଳ vector<vpiHandle>&table);

vpiHandle vpip_make_ମୂଳ_iterator(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type_code)
{
      ଫେରନ୍ତୁ make_subset_iterator_(type_code, vpip_ମୂଳ_table);
}

ଶୂନ୍ୟ vpip_make_ମୂଳ_iterator(__vpiHandle**&table, unsigned&ntable)
{
      table = &vpip_ମୂଳ_table[0];
      ntable = vpip_ମୂଳ_table.size();
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ port_delete(__vpiHandle*handle);
ଶୂନ୍ୟ port_bit_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
ସ୍ଥିର class_type **class_list = 0;
ସ୍ଥିର unsigned class_list_ଗଣନା = 0;

ସ୍ଥିର ଶୂନ୍ୟ delete_sub_scopes(__vpiScope *scope)
{
      ପାଇଁ (unsigned idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle item = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(item);
	    ବାଛ(item->get_type_code()) {
		ସ୍ଥିତି vpiFunction:
		ସ୍ଥିତି vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		ସ୍ଥିତି vpiModule:
		ସ୍ଥିତି vpiGenScope:
		ସ୍ଥିତି vpiNamedBegin:
		ସ୍ଥିତି vpiNamedFork:
		ସ୍ଥିତି vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  delete item;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiMemory:
		ସ୍ଥିତି vpiNetArray:
		  memory_delete(item);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiModPath:
		    /* The destination ModPath is cleaned up later. */
		  delete item;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiInterModPath:
		  delete item;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiNamedEvent:
		  named_event_delete(item);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiNet:
		ସ୍ଥିତି vpiReg:
		ସ୍ଥିତି vpiIntegerVar:
		ସ୍ଥିତି vpiLongIntVar:
		ସ୍ଥିତି vpiShortIntVar:
		ସ୍ଥିତି vpiIntVar:
		ସ୍ଥିତି vpiByteVar:
		ସ୍ଥିତି vpiBitVar:
		  signal_delete(item);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiParameter:
		  parameter_delete(item);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiRealVar:
		  real_delete(item);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiEnumTypespec:
		  enum_delete(item);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiPort:
		  port_delete(item);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiPortBit:
		  port_bit_delete(item);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiStringVar:
		  string_delete(item);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiClassVar:
		  class_delete(item);
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiArrayVar:
		  array_delete(item);
		  ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  fprintf(stderr, "Need support for type: %d\n",
		          item->get_type_code());
		  ପ୍ରତିଜ୍ଞା(0);
		  ଭାଙ୍ଗ;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      ପାଇଁ (citer = scope->classes.begin();
           citer != scope->classes.end(); ++ citer ) {
	    class_list_ଗଣନା += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_ଗଣନା*ଆକାର(class_type **)));
	    class_list[class_list_ଗଣନା-1] = citer->second;
      }
}

ଶୂନ୍ୟ ମୂଳ_table_delete(ଶୂନ୍ୟ)
{
      ପାଇଁ (unsigned idx = 0; idx < vpip_ମୂଳ_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_ମୂଳ_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    delete scope;
      }
      vpip_ମୂଳ_table.clear();

	/* Clean up all the class definitions. */
      ପାଇଁ (unsigned idx = 0; idx < class_list_ଗଣନା; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      ମୁକ୍ତ(class_list);
      class_list = 0;
      class_list_ଗଣନା = 0;
}
#endif

ସ୍ଥିର ଶୂନ୍ୟ construct_scope_fullname(__vpiScope*ref, ଅକ୍ଷର*buf)
{
      ଯଦି (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    ଯଦି (ref->scope->get_type_code() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      ଯଦି (ref->get_type_code() == vpiPackage) strcat(buf, "::");
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* scope_get_type(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
          ସ୍ଥିତି vpiModule:
            ଫେରନ୍ତୁ "vpiModule";
          ସ୍ଥିତି vpiGenScope:
            ଫେରନ୍ତୁ "vpiGenScope";
          ସ୍ଥିତି vpiFunction:
            ଫେରନ୍ତୁ "vpiFunction";
          ସ୍ଥିତି vpiTask:
            ଫେରନ୍ତୁ "vpiTask";
          ସ୍ଥିତି vpiNamedBegin:
            ଫେରନ୍ତୁ "vpiNamedBegin";
          ସ୍ଥିତି vpiNamedFork:
            ଫେରନ୍ତୁ "vpiNamedFork";
          ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            ଫେରନ୍ତୁ ଖାଲି;
      }
}

ସ୍ଥିର ଅକ୍ଷର* scope_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      ପ୍ରତିଜ୍ଞା(ref);

      ଅକ୍ଷର buf[4096];  // XXX is a fixed buffer size really reliable?
      ଅଚଳ ଅକ୍ଷର *p=0;
      ବାଛ (code) {
	  ସ୍ଥିତି vpiDefFile:
	    p = file_names[ref->def_file_idx];
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiFile:
	    p = file_names[ref->file_idx];
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiFullName:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiName:
	    p = ref->scope_name();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiDefName:
	    p = ref->scope_def_name();
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି vpiType:
	    p = scope_get_type(code);
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    ଫେରନ୍ତୁ ଖାଲି;
      }
      ଫେରନ୍ତୁ simple_set_rbuf_str(p);
}

ସ୍ଥିର vpiHandle scope_get_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      ପ୍ରତିଜ୍ଞା(rfp);

      ବାଛ (code) {

	  ସ୍ଥିତି vpiScope:
	    ଫେରନ୍ତୁ rfp->scope;

	  ସ୍ଥିତି vpiModule:
	    ଫେରନ୍ତୁ rfp->scope;
      }

      ଫେରନ୍ତୁ 0;
}

/* compares vpiType's considering object classes */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା compare_types(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      ଯଦି (code == vpiScope)
	    ଫେରନ୍ତୁ 1;

      ଯଦି (code == type)
	    ଫେରନ୍ତୁ 1;

      ଯଦି ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    ଫେରନ୍ତୁ 1;

      ଯଦି ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    ଫେରନ୍ତୁ 1;

      ଯଦି ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    ଫେରନ୍ତୁ 1;

      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର vpiHandle make_subset_iterator_(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type_code, ଅଚଳ vector<vpiHandle>&table)
{
      unsigned mcnt = 0, ncnt = 0;
      vpiHandle*args;

      ପାଇଁ (unsigned idx = 0; idx < table.size(); idx += 1)
	    ଯଦି (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      ଯଦି (mcnt == 0)
	    ଫେରନ୍ତୁ 0;

      args = static_cast<vpiHandle*>(calloc(mcnt, ଆକାର(vpiHandle)));
      ପାଇଁ (unsigned idx = 0; idx < table.size(); idx += 1)
	    ଯଦି (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      ପ୍ରତିଜ୍ଞା(ncnt == mcnt);

      ଫେରନ୍ତୁ vpip_make_iterator(mcnt, args, ସତ୍ୟ);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
ସ୍ଥିର vpiHandle module_iter(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      ପ୍ରତିଜ୍ଞା(ref);

      ଫେରନ୍ତୁ make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(ଅଚଳ ଅକ୍ଷର*nam, ଅଚଳ ଅକ୍ଷର*tnam, ତାର୍କିକ auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiScope::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiCellInstance:
	    ଫେରନ୍ତୁ is_cell? 1 : 0;

	  ସ୍ଥିତି vpi_Def_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ def_lineno;

	  ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ lineno;

	  ସ୍ଥିତି vpiTimeUnit:
	    ଫେରନ୍ତୁ time_units;

	  ସ୍ଥିତି vpiTimePrecision:
	    ଫେରନ୍ତୁ time_precision;

	  ସ୍ଥିତି vpi_ଶୀର୍ଷ_Module:
	    ଫେରନ୍ତୁ 0x0 == scope;

          ସ୍ଥିତି vpiAutomatic:
	    ଫେରନ୍ତୁ is_automatic_? 1 : 0;
      }

      ଫେରନ୍ତୁ vpiUndefined;
}


ଅକ୍ଷର*__vpiScope::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ scope_get_str(code, ଏହି); }

vpiHandle __vpiScope::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ scope_get_handle(code, ଏହି); }

vpiHandle __vpiScope::vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ module_iter(code, ଏହି); }


ଶ୍ରେଣୀ vpiScopeModule  : ସର୍ବସାଧାରଣ __vpiScope {
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ vpiScopeModule(ଅଚଳ ଅକ୍ଷର*nam, ଅଚଳ ଅକ୍ଷର*tnam)
      : __vpiScope(nam,tnam,ମିଥ୍ୟା) { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiModule; }
};

ସଂରଚନା vpiScopePackage  : ସର୍ବସାଧାରଣ __vpiScope {
      ଧାଡ଼ିଭିତରେ vpiScopePackage(ଅଚଳ ଅକ୍ଷର*nam, ଅଚଳ ଅକ୍ଷର*tnam)
      : __vpiScope(nam,tnam) { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiPackage; }
};

ସଂରଚନା vpiScopeTask  : ସର୍ବସାଧାରଣ __vpiScope {
      ଧାଡ଼ିଭିତରେ vpiScopeTask(ଅଚଳ ଅକ୍ଷର*nam, ଅଚଳ ଅକ୍ଷର*tnam)
      : __vpiScope(nam,tnam) { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiTask; }
};

ସଂରଚନା vpiScopeTaskAuto  : ସର୍ବସାଧାରଣ __vpiScope {
      ଧାଡ଼ିଭିତରେ vpiScopeTaskAuto(ଅଚଳ ଅକ୍ଷର*nam, ଅଚଳ ଅକ୍ଷର*tnam)
      : __vpiScope(nam,tnam,ସତ୍ୟ) {  }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiTask; }
};

ସଂରଚନା vpiScopeBegin  : ସର୍ବସାଧାରଣ __vpiScope {
      ଧାଡ଼ିଭିତରେ vpiScopeBegin(ଅଚଳ ଅକ୍ଷର*nam, ଅଚଳ ଅକ୍ଷର*tnam)
      : __vpiScope(nam,tnam,ମିଥ୍ୟା) { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiNamedBegin; }
};

ଶ୍ରେଣୀ vpiScopeBeginAuto  : ସର୍ବସାଧାରଣ __vpiScope {
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ vpiScopeBeginAuto(ଅଚଳ ଅକ୍ଷର*nam, ଅଚଳ ଅକ୍ଷର*tnam)
      : __vpiScope(nam,tnam,ସତ୍ୟ) {  }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiNamedBegin; }
};

ସଂରଚନା vpiScopeGenerate  : ସର୍ବସାଧାରଣ __vpiScope {
      ଧାଡ଼ିଭିତରେ vpiScopeGenerate(ଅଚଳ ଅକ୍ଷର*nam, ଅଚଳ ଅକ୍ଷର*tnam)
      : __vpiScope(nam,tnam) { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiGenScope; }
};

ସଂରଚନା vpiScopeFork  : ସର୍ବସାଧାରଣ __vpiScope {
      ଧାଡ଼ିଭିତରେ vpiScopeFork(ଅଚଳ ଅକ୍ଷର*nam, ଅଚଳ ଅକ୍ଷର*tnam)
      : __vpiScope(nam,tnam,ମିଥ୍ୟା) { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiNamedFork; }
};

ଶ୍ରେଣୀ vpiScopeForkAuto  : ସର୍ବସାଧାରଣ __vpiScope {
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ vpiScopeForkAuto(ଅଚଳ ଅକ୍ଷର*nam, ଅଚଳ ଅକ୍ଷର*tnam)
      : __vpiScope(nam,tnam,ସତ୍ୟ) {  }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiNamedFork; }
};

ସଂରଚନା vpiScopeClass  : ସର୍ବସାଧାରଣ __vpiScope {
      ଧାଡ଼ିଭିତରେ vpiScopeClass(ଅଚଳ ଅକ୍ଷର*nam, ଅଚଳ ଅକ୍ଷର*tnam)
      : __vpiScope(nam,tnam) { }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
ସ୍ଥିର __vpiScope*current_scope = 0;

ଶୂନ୍ୟ vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj)
{
      ପ୍ରତିଜ୍ଞା(scope);
      scope->intern.push_back(obj);
}

/*
 * When the compiler encounters a scope declaration, this function
 * creates and initializes a __vpiScope object with the requested name
 * and within the addressed parent. The label is used as a key in the
 * symbol table and the name is used to construct the actual object.
 */
ଶୂନ୍ୟ
compile_scope_decl(ଅକ୍ଷର*label, ଅକ୍ଷର*type, ଅକ୍ଷର*name, ଅକ୍ଷର*tname,
                   ଅକ୍ଷର*parent, long file_idx, long lineno,
                   long def_file_idx, long def_lineno, long is_cell)
{
      ଗଣନା_vpi_scopes += 1;
      ଅକ୍ଷର vec_type;
      ଅକ୍ଷର sign_flag;
      unsigned wid;

      __vpiScope*scope;
      ଯଦି (strcmp(type,"module") == 0) {
	    scope = ନୂତନ vpiScopeModule(name, tname);
      } ଅନ୍ୟଥା ଯଦି ( sscanf(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type_code;
	    ଯଦି (sign_flag=='s') {
		  type_code = vpiSizedSignedFunc;
	    } ଅନ୍ୟଥା ଯଦି (sign_flag=='u') {
		  type_code = vpiSizedFunc;
	    } ଅନ୍ୟଥା ଯଦି (sign_flag=='i') {
		  type_code = vpiIntFunc;
	    } ଅନ୍ୟଥା {
		  ପ୍ରତିଜ୍ଞା(0);
		  type_code = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = ନୂତନ vpiScopeFunction(name, tname, ମିଥ୍ୟା, type_code, wid, init_val);

      } ଅନ୍ୟଥା ଯଦି ( sscanf(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type_code;
	    ବାଛ (sign_flag) {
		ସ୍ଥିତି 's':
		  type_code = vpiSizedSignedFunc;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 'u':
		  type_code = vpiSizedFunc;
		  ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  ପ୍ରତିଜ୍ଞା(0);
		  type_code = vpiSizedFunc;
		  ଭାଙ୍ଗ;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = ନୂତନ vpiScopeFunction(name, tname, ସତ୍ୟ, type_code, wid, init_val);

      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"function.obj") == 0) {
	    scope = ନୂତନ vpiScopeFunction(name, tname, ମିଥ୍ୟା, vpiSizedFunc, 0, BIT4_0);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"autofunction.obj") == 0) {
	    scope = ନୂତନ vpiScopeFunction(name, tname, ସତ୍ୟ, vpiSizedFunc, 0, BIT4_0);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"function.real") == 0) {
	    scope = ନୂତନ vpiScopeFunction(name, tname, ମିଥ୍ୟା, vpiRealFunc, 0, BIT4_0);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"autofunction.real") == 0) {
	    scope = ନୂତନ vpiScopeFunction(name, tname, ସତ୍ୟ, vpiRealFunc, 0, BIT4_0);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"function.str") == 0) {
	    scope = ନୂତନ vpiScopeFunction(name, tname, ମିଥ୍ୟା, vpiOtherFunc, 0, BIT4_0);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"autofunction.str") == 0) {
	    scope = ନୂତନ vpiScopeFunction(name, tname, ସତ୍ୟ, vpiOtherFunc, 0, BIT4_0);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"function.void") == 0) {
	    scope = ନୂତନ vpiScopeFunction(name, tname, ମିଥ୍ୟା, vpiOtherFunc, 0, BIT4_0);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"autofunction.void") == 0) {
	    scope = ନୂତନ vpiScopeFunction(name, tname, ସତ୍ୟ, vpiOtherFunc, 0, BIT4_0);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"task") == 0) {
	    scope = ନୂତନ vpiScopeTask(name, tname);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"autotask") == 0) {
	    scope = ନୂତନ vpiScopeTaskAuto(name, tname);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"fork") == 0) {
	    scope = ନୂତନ vpiScopeFork(name, tname);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"autofork") == 0) {
	    scope = ନୂତନ vpiScopeForkAuto(name, tname);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"begin") == 0) {
	    scope = ନୂତନ vpiScopeBegin(name, tname);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"autobegin") == 0) {
	    scope = ନୂତନ vpiScopeBeginAuto(name, tname);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"generate") == 0) {
	    scope = ନୂତନ vpiScopeGenerate(name, tname);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"package") == 0) {
	    scope = ନୂତନ vpiScopePackage(name, tname);
      } ଅନ୍ୟଥା ଯଦି (strcmp(type,"class") == 0) {
	    scope = ନୂତନ vpiScopeClass(name, tname);
      } ଅନ୍ୟଥା {
	    scope = ନୂତନ vpiScopeModule(name, tname);
	    ପ୍ରତିଜ୍ଞା(0);
      }

      scope->file_idx = (unsigned) file_idx;
      scope->lineno  = (unsigned) lineno;
      scope->def_file_idx = (unsigned) def_file_idx;
      scope->def_lineno  = (unsigned) def_lineno;
      scope->item = 0;
      scope->nitem = 0;
      scope->live_contexts = 0;
      scope->free_contexts = 0;

      ଯଦି (is_cell) scope->is_cell = ସତ୍ୟ;
      ଅନ୍ୟଥା scope->is_cell = ମିଥ୍ୟା;

      current_scope = scope;

      compile_vpi_ପ୍ରତୀକ(label, scope);

      ମୁକ୍ତ(label);
      ମୁକ୍ତ(type);
      delete[] name;
      delete[] tname;

      ଯଦି (parent) {
	    ସ୍ଥିର vpiHandle obj;
	    compile_vpi_lookup(&obj, parent);
	    ପ୍ରତିଜ୍ଞା(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_to_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->time_units = sp->time_units;
	    scope->time_precision = sp->time_precision;

      } ଅନ୍ୟଥା {
	    scope->scope = 0x0;

	    vpip_ମୂଳ_table.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->time_units = vpip_get_time_precision();
	    scope->time_precision = vpip_get_time_precision();
      }
}

ଶୂନ୍ୟ compile_scope_recall(ଅକ୍ଷର*ପ୍ରତୀକ)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(ଶୂନ୍ୟ*)&current_scope, ପ୍ରତୀକ);
      ପ୍ରତିଜ୍ଞା(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
ଶୂନ୍ୟ compile_timescale(long units, long precision)
{
      ପ୍ରତିଜ୍ଞା(current_scope);
      current_scope->time_units = units;
      current_scope->time_precision = precision;
}

__vpiScope* vpip_peek_current_scope(ଶୂନ୍ୟ)
{
      ଫେରନ୍ତୁ current_scope;
}

ଶୂନ୍ୟ vpip_attach_to_current_scope(vpiHandle obj)
{
      vpip_attach_to_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(ଶୂନ୍ୟ)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      ଯେତେବେଳେ (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      ଫେରନ୍ତୁ scope;
}

unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      ପ୍ରତିଜ୍ଞା(scope);
      ପ୍ରତିଜ୍ଞା(scope->is_automatic());

      unsigned idx = scope->nitem++;

      ଯଦି (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (malloc(ଆକାର(automatic_hooks_s*)));
      ଅନ୍ୟଥା
	    scope->item = static_cast<automatic_hooks_s**>
	                  (realloc(scope->item,
	                   ଆକାର(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      ଫେରନ୍ତୁ 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              unsigned index,
              ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_direction,
              unsigned width,
              ଅଚଳ ଅକ୍ଷର *name,
              ଅକ୍ଷର* buffer) :
      parent_(parent),
      index_(index),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      ଯଦି (buffer != nullptr) functor_ref_lookup(&ref_, buffer);
      ଅନ୍ୟଥା ref_ = nullptr;
}

vpiPortInfo::~vpiPortInfo()
{
      delete[] name_;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ port_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortInfo *>(handle);
}
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpiPortInfo::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ( code ) {

        ସ୍ଥିତି vpiDirection :
          ଫେରନ୍ତୁ direction_;
        ସ୍ଥିତି vpiPortIndex :
          ଫେରନ୍ତୁ index_;
        ସ୍ଥିତି vpiSize :
          ଫେରନ୍ତୁ width_;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ :
          ଫେରନ୍ତୁ vpiUndefined;
      }

}


ଅକ୍ଷର *vpiPortInfo::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ( code ) {
        ସ୍ଥିତି vpiName :
          ଫେରନ୍ତୁ simple_set_rbuf_str(name_);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ :
          ଫେରନ୍ତୁ ଖାଲି;
      }

}


vpiHandle vpiPortInfo::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{

      ବାଛ (code) {

          ସ୍ଥିତି vpiParent:
          ସ୍ଥିତି vpiScope:
          ସ୍ଥିତି vpiModule:
            ଫେରନ୍ତୁ parent_;
          ପୂର୍ବନିର୍ଦ୍ଧାରିତ :
            ଭାଙ୍ଗ;
      }

      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର vpiHandle portinfo_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      ପ୍ରତିଜ୍ଞା(rfp);
      unsigned width = rfp->get_width();

      ବାଛ (code) {
	  ସ୍ଥିତି vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(width, ଆକାର(vpiHandle*)));

	    ପାଇଁ (unsigned i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    ଫେରନ୍ତୁ vpip_make_iterator(width, args, ସତ୍ୟ);
	  }
      }
      ଫେରନ୍ତୁ 0;
}

vpiHandle vpiPortInfo::vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ଫେରନ୍ତୁ portinfo_iterate(code, ଏହି);
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
ଶୂନ୍ୟ port_bit_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortBitInfo *>(handle);
}
#endif

vpiHandle vpiPortBitInfo::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{

      ବାଛ (code) {
          ସ୍ଥିତି vpiParent:
            ଫେରନ୍ତୁ parent_;
          ପୂର୍ବନିର୍ଦ୍ଧାରିତ :
            ଭାଙ୍ଗ;
      }

      ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpiPortBitInfo::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ( code ) {
        ସ୍ଥିତି vpiBit :
          ଫେରନ୍ତୁ bit_;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ :
          ଭାଙ୍ଗ;
      }

      ଫେରନ୍ତୁ 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
ଶୂନ୍ୟ compile_port_info( unsigned index, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_direction, unsigned width, ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର* buffer )
{
      vpiPortInfo* obj = ନୂତନ vpiPortInfo( vpip_peek_current_scope(),
                                          index, vpi_direction, width, name, buffer );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      ପାଇଁ (unsigned i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = ନୂତନ vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
