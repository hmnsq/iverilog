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

using ನಾಮಾಕಾಶ std;

ಸ್ಥಿರ vector<vpiHandle> vpip_ಬೇರು_table;

ಸ್ಥಿರ vpiHandle make_subset_iterator_(ಪೂರ್ಣಾಙ್ಕ type_code, ಧ್ರುವ vector<vpiHandle>&table);

vpiHandle vpip_make_ಬೇರು_iterator(ಪೂರ್ಣಾಙ್ಕ type_code)
{
      ಪ್ರತ್ಯಾಗಮ make_subset_iterator_(type_code, vpip_ಬೇರು_table);
}

ಶೂನ್ಯ vpip_make_ಬೇರು_iterator(__vpiHandle**&table, unsigned&ntable)
{
      table = &vpip_ಬೇರು_table[0];
      ntable = vpip_ಬೇರು_table.size();
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ port_delete(__vpiHandle*handle);
ಶೂನ್ಯ port_bit_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
ಸ್ಥಿರ class_type **class_list = 0;
ಸ್ಥಿರ unsigned class_list_ಎಣಿಕೆ = 0;

ಸ್ಥಿರ ಶೂನ್ಯ delete_sub_scopes(__vpiScope *scope)
{
      ಪ್ರತಿ (unsigned idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle item = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(item);
	    ವಿಕಲ್ಪ(item->get_type_code()) {
		ಪಕ್ಷ vpiFunction:
		ಪಕ್ಷ vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		ಪಕ್ಷ vpiModule:
		ಪಕ್ಷ vpiGenScope:
		ಪಕ್ಷ vpiNamedBegin:
		ಪಕ್ಷ vpiNamedFork:
		ಪಕ್ಷ vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  delete item;
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiMemory:
		ಪಕ್ಷ vpiNetArray:
		  memory_delete(item);
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiModPath:
		    /* The destination ModPath is cleaned up later. */
		  delete item;
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiInterModPath:
		  delete item;
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiNamedEvent:
		  named_event_delete(item);
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiNet:
		ಪಕ್ಷ vpiReg:
		ಪಕ್ಷ vpiIntegerVar:
		ಪಕ್ಷ vpiLongIntVar:
		ಪಕ್ಷ vpiShortIntVar:
		ಪಕ್ಷ vpiIntVar:
		ಪಕ್ಷ vpiByteVar:
		ಪಕ್ಷ vpiBitVar:
		  signal_delete(item);
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiParameter:
		  parameter_delete(item);
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiRealVar:
		  real_delete(item);
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiEnumTypespec:
		  enum_delete(item);
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiPort:
		  port_delete(item);
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiPortBit:
		  port_bit_delete(item);
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiStringVar:
		  string_delete(item);
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiClassVar:
		  class_delete(item);
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiArrayVar:
		  array_delete(item);
		  ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		  fprintf(stderr, "Need support for type: %d\n",
		          item->get_type_code());
		  ಪ್ರತಿಜ್ಞಾ(0);
		  ಭಙ್ಗ;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      ಪ್ರತಿ (citer = scope->classes.begin();
           citer != scope->classes.end(); ++ citer ) {
	    class_list_ಎಣಿಕೆ += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_ಎಣಿಕೆ*ಪರಿಮಾಣ(class_type **)));
	    class_list[class_list_ಎಣಿಕೆ-1] = citer->second;
      }
}

ಶೂನ್ಯ ಬೇರು_table_delete(ಶೂನ್ಯ)
{
      ಪ್ರತಿ (unsigned idx = 0; idx < vpip_ಬೇರು_table.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_ಬೇರು_table[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    delete scope;
      }
      vpip_ಬೇರು_table.clear();

	/* Clean up all the class definitions. */
      ಪ್ರತಿ (unsigned idx = 0; idx < class_list_ಎಣಿಕೆ; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      ಮುಕ್ತ(class_list);
      class_list = 0;
      class_list_ಎಣಿಕೆ = 0;
}
#endif

ಸ್ಥಿರ ಶೂನ್ಯ construct_scope_fullname(__vpiScope*ref, ವರ್ಣ*buf)
{
      ಯದಿ (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    ಯದಿ (ref->scope->get_type_code() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      ಯದಿ (ref->get_type_code() == vpiPackage) strcat(buf, "::");
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* scope_get_type(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
          ಪಕ್ಷ vpiModule:
            ಪ್ರತ್ಯಾಗಮ "vpiModule";
          ಪಕ್ಷ vpiGenScope:
            ಪ್ರತ್ಯಾಗಮ "vpiGenScope";
          ಪಕ್ಷ vpiFunction:
            ಪ್ರತ್ಯಾಗಮ "vpiFunction";
          ಪಕ್ಷ vpiTask:
            ಪ್ರತ್ಯಾಗಮ "vpiTask";
          ಪಕ್ಷ vpiNamedBegin:
            ಪ್ರತ್ಯಾಗಮ "vpiNamedBegin";
          ಪಕ್ಷ vpiNamedFork:
            ಪ್ರತ್ಯಾಗಮ "vpiNamedFork";
          ಸ್ವಭಾವ:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
      }
}

ಸ್ಥಿರ ವರ್ಣ* scope_get_str(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      ಪ್ರತಿಜ್ಞಾ(ref);

      ವರ್ಣ buf[4096];  // XXX is a fixed buffer size really reliable?
      ಧ್ರುವ ವರ್ಣ *p=0;
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiDefFile:
	    p = file_names[ref->def_file_idx];
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiFile:
	    p = file_names[ref->file_idx];
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiFullName:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiName:
	    p = ref->scope_name();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiDefName:
	    p = ref->scope_def_name();
	    ಭಙ್ಗ;

	  ಪಕ್ಷ vpiType:
	    p = scope_get_type(code);
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
      }
      ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(p);
}

ಸ್ಥಿರ vpiHandle scope_get_handle(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ವಿಕಲ್ಪ (code) {

	  ಪಕ್ಷ vpiScope:
	    ಪ್ರತ್ಯಾಗಮ rfp->scope;

	  ಪಕ್ಷ vpiModule:
	    ಪ್ರತ್ಯಾಗಮ rfp->scope;
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

/* compares vpiType's considering object classes */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ compare_types(ಪೂರ್ಣಾಙ್ಕ code, ಪೂರ್ಣಾಙ್ಕ type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      ಯದಿ (code == vpiScope)
	    ಪ್ರತ್ಯಾಗಮ 1;

      ಯದಿ (code == type)
	    ಪ್ರತ್ಯಾಗಮ 1;

      ಯದಿ ( code == vpiInternalScope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    ಪ್ರತ್ಯಾಗಮ 1;

      ಯದಿ ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpiProgram ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    ಪ್ರತ್ಯಾಗಮ 1;

      ಯದಿ ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpiBitVar      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpiTimeVar     ||
	      type == vpiRealVar))
	    ಪ್ರತ್ಯಾಗಮ 1;

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ vpiHandle make_subset_iterator_(ಪೂರ್ಣಾಙ್ಕ type_code, ಧ್ರುವ vector<vpiHandle>&table)
{
      unsigned mcnt = 0, ncnt = 0;
      vpiHandle*args;

      ಪ್ರತಿ (unsigned idx = 0; idx < table.size(); idx += 1)
	    ಯದಿ (compare_types(type_code, table[idx]->get_type_code()))
		  mcnt += 1;

      ಯದಿ (mcnt == 0)
	    ಪ್ರತ್ಯಾಗಮ 0;

      args = static_cast<vpiHandle*>(calloc(mcnt, ಪರಿಮಾಣ(vpiHandle)));
      ಪ್ರತಿ (unsigned idx = 0; idx < table.size(); idx += 1)
	    ಯದಿ (compare_types(type_code, table[idx]->get_type_code()))
		  args[ncnt++] = table[idx];

      ಪ್ರತಿಜ್ಞಾ(ncnt == mcnt);

      ಪ್ರತ್ಯಾಗಮ vpip_make_iterator(mcnt, args, ಸತ್ಯ);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
ಸ್ಥಿರ vpiHandle module_iter(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      ಪ್ರತಿಜ್ಞಾ(ref);

      ಪ್ರತ್ಯಾಗಮ make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(ಧ್ರುವ ವರ್ಣ*nam, ಧ್ರುವ ವರ್ಣ*tnam, ತಾರ್ಕಿಕ auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

ಪೂರ್ಣಾಙ್ಕ __vpiScope::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiCellInstance:
	    ಪ್ರತ್ಯಾಗಮ is_cell? 1 : 0;

	  ಪಕ್ಷ vpi_Def_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ def_lineno;

	  ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ lineno;

	  ಪಕ್ಷ vpiTimeUnit:
	    ಪ್ರತ್ಯಾಗಮ time_units;

	  ಪಕ್ಷ vpiTimePrecision:
	    ಪ್ರತ್ಯಾಗಮ time_precision;

	  ಪಕ್ಷ vpi_ತುದಿ_Module:
	    ಪ್ರತ್ಯಾಗಮ 0x0 == scope;

          ಪಕ್ಷ vpiAutomatic:
	    ಪ್ರತ್ಯಾಗಮ is_automatic_? 1 : 0;
      }

      ಪ್ರತ್ಯಾಗಮ vpiUndefined;
}


ವರ್ಣ*__vpiScope::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ scope_get_str(code, ಏತತ್); }

vpiHandle __vpiScope::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ scope_get_handle(code, ಏತತ್); }

vpiHandle __vpiScope::vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ module_iter(code, ಏತತ್); }


ವರ್ಗ vpiScopeModule  : ಸಾರ್ವಜನಿಕ __vpiScope {
    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ vpiScopeModule(ಧ್ರುವ ವರ್ಣ*nam, ಧ್ರುವ ವರ್ಣ*tnam)
      : __vpiScope(nam,tnam,ಅಸತ್ಯ) { }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiModule; }
};

ಸಂರಚನಾ vpiScopePackage  : ಸಾರ್ವಜನಿಕ __vpiScope {
      ಅನ್ತಃಸ್ಥ vpiScopePackage(ಧ್ರುವ ವರ್ಣ*nam, ಧ್ರುವ ವರ್ಣ*tnam)
      : __vpiScope(nam,tnam) { }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiPackage; }
};

ಸಂರಚನಾ vpiScopeTask  : ಸಾರ್ವಜನಿಕ __vpiScope {
      ಅನ್ತಃಸ್ಥ vpiScopeTask(ಧ್ರುವ ವರ್ಣ*nam, ಧ್ರುವ ವರ್ಣ*tnam)
      : __vpiScope(nam,tnam) { }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiTask; }
};

ಸಂರಚನಾ vpiScopeTaskAuto  : ಸಾರ್ವಜನಿಕ __vpiScope {
      ಅನ್ತಃಸ್ಥ vpiScopeTaskAuto(ಧ್ರುವ ವರ್ಣ*nam, ಧ್ರುವ ವರ್ಣ*tnam)
      : __vpiScope(nam,tnam,ಸತ್ಯ) {  }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiTask; }
};

ಸಂರಚನಾ vpiScopeBegin  : ಸಾರ್ವಜನಿಕ __vpiScope {
      ಅನ್ತಃಸ್ಥ vpiScopeBegin(ಧ್ರುವ ವರ್ಣ*nam, ಧ್ರುವ ವರ್ಣ*tnam)
      : __vpiScope(nam,tnam,ಅಸತ್ಯ) { }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiNamedBegin; }
};

ವರ್ಗ vpiScopeBeginAuto  : ಸಾರ್ವಜನಿಕ __vpiScope {
    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ vpiScopeBeginAuto(ಧ್ರುವ ವರ್ಣ*nam, ಧ್ರುವ ವರ್ಣ*tnam)
      : __vpiScope(nam,tnam,ಸತ್ಯ) {  }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiNamedBegin; }
};

ಸಂರಚನಾ vpiScopeGenerate  : ಸಾರ್ವಜನಿಕ __vpiScope {
      ಅನ್ತಃಸ್ಥ vpiScopeGenerate(ಧ್ರುವ ವರ್ಣ*nam, ಧ್ರುವ ವರ್ಣ*tnam)
      : __vpiScope(nam,tnam) { }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiGenScope; }
};

ಸಂರಚನಾ vpiScopeFork  : ಸಾರ್ವಜನಿಕ __vpiScope {
      ಅನ್ತಃಸ್ಥ vpiScopeFork(ಧ್ರುವ ವರ್ಣ*nam, ಧ್ರುವ ವರ್ಣ*tnam)
      : __vpiScope(nam,tnam,ಅಸತ್ಯ) { }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiNamedFork; }
};

ವರ್ಗ vpiScopeForkAuto  : ಸಾರ್ವಜನಿಕ __vpiScope {
    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ vpiScopeForkAuto(ಧ್ರುವ ವರ್ಣ*nam, ಧ್ರುವ ವರ್ಣ*tnam)
      : __vpiScope(nam,tnam,ಸತ್ಯ) {  }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiNamedFork; }
};

ಸಂರಚನಾ vpiScopeClass  : ಸಾರ್ವಜನಿಕ __vpiScope {
      ಅನ್ತಃಸ್ಥ vpiScopeClass(ಧ್ರುವ ವರ್ಣ*nam, ಧ್ರುವ ವರ್ಣ*tnam)
      : __vpiScope(nam,tnam) { }
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
ಸ್ಥಿರ __vpiScope*current_scope = 0;

ಶೂನ್ಯ vpip_attach_to_scope(__vpiScope*scope, vpiHandle obj)
{
      ಪ್ರತಿಜ್ಞಾ(scope);
      scope->intern.push_back(obj);
}

/*
 * When the compiler encounters a scope declaration, this function
 * creates and initializes a __vpiScope object with the requested name
 * and within the addressed parent. The label is used as a key in the
 * symbol table and the name is used to construct the actual object.
 */
ಶೂನ್ಯ
compile_scope_decl(ವರ್ಣ*label, ವರ್ಣ*type, ವರ್ಣ*name, ವರ್ಣ*tname,
                   ವರ್ಣ*parent, long file_idx, long lineno,
                   long def_file_idx, long def_lineno, long is_cell)
{
      ಎಣಿಕೆ_vpi_scopes += 1;
      ವರ್ಣ vec_type;
      ವರ್ಣ sign_flag;
      unsigned wid;

      __vpiScope*scope;
      ಯದಿ (strcmp(type,"module") == 0) {
	    scope = ನೂತನ vpiScopeModule(name, tname);
      } ಅನ್ಯಥಾ ಯದಿ ( sscanf(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    ಪೂರ್ಣಾಙ್ಕ type_code;
	    ಯದಿ (sign_flag=='s') {
		  type_code = vpiSizedSignedFunc;
	    } ಅನ್ಯಥಾ ಯದಿ (sign_flag=='u') {
		  type_code = vpiSizedFunc;
	    } ಅನ್ಯಥಾ ಯದಿ (sign_flag=='i') {
		  type_code = vpiIntFunc;
	    } ಅನ್ಯಥಾ {
		  ಪ್ರತಿಜ್ಞಾ(0);
		  type_code = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = ನೂತನ vpiScopeFunction(name, tname, ಅಸತ್ಯ, type_code, wid, init_val);

      } ಅನ್ಯಥಾ ಯದಿ ( sscanf(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    ಪೂರ್ಣಾಙ್ಕ type_code;
	    ವಿಕಲ್ಪ (sign_flag) {
		ಪಕ್ಷ 's':
		  type_code = vpiSizedSignedFunc;
		  ಭಙ್ಗ;
		ಪಕ್ಷ 'u':
		  type_code = vpiSizedFunc;
		  ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		  ಪ್ರತಿಜ್ಞಾ(0);
		  type_code = vpiSizedFunc;
		  ಭಙ್ಗ;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = ನೂತನ vpiScopeFunction(name, tname, ಸತ್ಯ, type_code, wid, init_val);

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"function.obj") == 0) {
	    scope = ನೂತನ vpiScopeFunction(name, tname, ಅಸತ್ಯ, vpiSizedFunc, 0, BIT4_0);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"autofunction.obj") == 0) {
	    scope = ನೂತನ vpiScopeFunction(name, tname, ಸತ್ಯ, vpiSizedFunc, 0, BIT4_0);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"function.real") == 0) {
	    scope = ನೂತನ vpiScopeFunction(name, tname, ಅಸತ್ಯ, vpiRealFunc, 0, BIT4_0);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"autofunction.real") == 0) {
	    scope = ನೂತನ vpiScopeFunction(name, tname, ಸತ್ಯ, vpiRealFunc, 0, BIT4_0);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"function.str") == 0) {
	    scope = ನೂತನ vpiScopeFunction(name, tname, ಅಸತ್ಯ, vpiOtherFunc, 0, BIT4_0);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"autofunction.str") == 0) {
	    scope = ನೂತನ vpiScopeFunction(name, tname, ಸತ್ಯ, vpiOtherFunc, 0, BIT4_0);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"function.void") == 0) {
	    scope = ನೂತನ vpiScopeFunction(name, tname, ಅಸತ್ಯ, vpiOtherFunc, 0, BIT4_0);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"autofunction.void") == 0) {
	    scope = ನೂತನ vpiScopeFunction(name, tname, ಸತ್ಯ, vpiOtherFunc, 0, BIT4_0);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"task") == 0) {
	    scope = ನೂತನ vpiScopeTask(name, tname);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"autotask") == 0) {
	    scope = ನೂತನ vpiScopeTaskAuto(name, tname);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"fork") == 0) {
	    scope = ನೂತನ vpiScopeFork(name, tname);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"autofork") == 0) {
	    scope = ನೂತನ vpiScopeForkAuto(name, tname);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"begin") == 0) {
	    scope = ನೂತನ vpiScopeBegin(name, tname);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"autobegin") == 0) {
	    scope = ನೂತನ vpiScopeBeginAuto(name, tname);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"generate") == 0) {
	    scope = ನೂತನ vpiScopeGenerate(name, tname);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"package") == 0) {
	    scope = ನೂತನ vpiScopePackage(name, tname);
      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type,"class") == 0) {
	    scope = ನೂತನ vpiScopeClass(name, tname);
      } ಅನ್ಯಥಾ {
	    scope = ನೂತನ vpiScopeModule(name, tname);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      scope->file_idx = (unsigned) file_idx;
      scope->lineno  = (unsigned) lineno;
      scope->def_file_idx = (unsigned) def_file_idx;
      scope->def_lineno  = (unsigned) def_lineno;
      scope->item = 0;
      scope->nitem = 0;
      scope->live_contexts = 0;
      scope->free_contexts = 0;

      ಯದಿ (is_cell) scope->is_cell = ಸತ್ಯ;
      ಅನ್ಯಥಾ scope->is_cell = ಅಸತ್ಯ;

      current_scope = scope;

      compile_vpi_ಚಿಹ್ನೆ(label, scope);

      ಮುಕ್ತ(label);
      ಮುಕ್ತ(type);
      delete[] name;
      delete[] tname;

      ಯದಿ (parent) {
	    ಸ್ಥಿರ vpiHandle obj;
	    compile_vpi_lookup(&obj, parent);
	    ಪ್ರತಿಜ್ಞಾ(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_to_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->time_units = sp->time_units;
	    scope->time_precision = sp->time_precision;

      } ಅನ್ಯಥಾ {
	    scope->scope = 0x0;

	    vpip_ಬೇರು_table.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->time_units = vpip_get_time_precision();
	    scope->time_precision = vpip_get_time_precision();
      }
}

ಶೂನ್ಯ compile_scope_recall(ವರ್ಣ*ಚಿಹ್ನೆ)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(ಶೂನ್ಯ*)&current_scope, ಚಿಹ್ನೆ);
      ಪ್ರತಿಜ್ಞಾ(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
ಶೂನ್ಯ compile_timescale(long units, long precision)
{
      ಪ್ರತಿಜ್ಞಾ(current_scope);
      current_scope->time_units = units;
      current_scope->time_precision = precision;
}

__vpiScope* vpip_peek_current_scope(ಶೂನ್ಯ)
{
      ಪ್ರತ್ಯಾಗಮ current_scope;
}

ಶೂನ್ಯ vpip_attach_to_current_scope(vpiHandle obj)
{
      vpip_attach_to_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(ಶೂನ್ಯ)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      ಯಾವತ್ (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      ಪ್ರತ್ಯಾಗಮ scope;
}

unsigned vpip_add_item_to_context(automatic_hooks_s*item,
                                  __vpiScope*scope)
{
      ಪ್ರತಿಜ್ಞಾ(scope);
      ಪ್ರತಿಜ್ಞಾ(scope->is_automatic());

      unsigned idx = scope->nitem++;

      ಯದಿ (scope->item == 0)
	    scope->item = static_cast<automatic_hooks_s**>
	                  (malloc(ಪರಿಮಾಣ(automatic_hooks_s*)));
      ಅನ್ಯಥಾ
	    scope->item = static_cast<automatic_hooks_s**>
	                  (realloc(scope->item,
	                   ಪರಿಮಾಣ(automatic_hooks_s*)*scope->nitem));

      scope->item[idx] = item;

        /* Offset the context index by 2 to leave space for the list links. */
      ಪ್ರತ್ಯಾಗಮ 2 + idx;
}


vpiPortInfo::vpiPortInfo( __vpiScope *parent,
              unsigned index,
              ಪೂರ್ಣಾಙ್ಕ vpi_direction,
              unsigned width,
              ಧ್ರುವ ವರ್ಣ *name,
              ವರ್ಣ* buffer) :
      parent_(parent),
      index_(index),
      direction_(vpi_direction),
      width_(width),
      name_(name)
{
      ಯದಿ (buffer != nullptr) functor_ref_lookup(&ref_, buffer);
      ಅನ್ಯಥಾ ref_ = nullptr;
}

vpiPortInfo::~vpiPortInfo()
{
      delete[] name_;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ port_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortInfo *>(handle);
}
#endif

ಪೂರ್ಣಾಙ್ಕ vpiPortInfo::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ( code ) {

        ಪಕ್ಷ vpiDirection :
          ಪ್ರತ್ಯಾಗಮ direction_;
        ಪಕ್ಷ vpiPortIndex :
          ಪ್ರತ್ಯಾಗಮ index_;
        ಪಕ್ಷ vpiSize :
          ಪ್ರತ್ಯಾಗಮ width_;
        ಸ್ವಭಾವ :
          ಪ್ರತ್ಯಾಗಮ vpiUndefined;
      }

}


ವರ್ಣ *vpiPortInfo::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ( code ) {
        ಪಕ್ಷ vpiName :
          ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(name_);
        ಸ್ವಭಾವ :
          ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
      }

}


vpiHandle vpiPortInfo::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{

      ವಿಕಲ್ಪ (code) {

          ಪಕ್ಷ vpiParent:
          ಪಕ್ಷ vpiScope:
          ಪಕ್ಷ vpiModule:
            ಪ್ರತ್ಯಾಗಮ parent_;
          ಸ್ವಭಾವ :
            ಭಙ್ಗ;
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ vpiHandle portinfo_iterate(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      vpiPortInfo*rfp = dynamic_cast<vpiPortInfo*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);
      unsigned width = rfp->get_width();

      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiBit: {
	    vpiHandle*args = static_cast<vpiHandle*>(calloc(width, ಪರಿಮಾಣ(vpiHandle*)));

	    ಪ್ರತಿ (unsigned i = 0; i<rfp->port_bits_.size(); i++) {
		  args[i] = rfp->port_bits_[i];
	    }

	    ಪ್ರತ್ಯಾಗಮ vpip_make_iterator(width, args, ಸತ್ಯ);
	  }
      }
      ಪ್ರತ್ಯಾಗಮ 0;
}

vpiHandle vpiPortInfo::vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code)
{
      ಪ್ರತ್ಯಾಗಮ portinfo_iterate(code, ಏತತ್);
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
ಶೂನ್ಯ port_bit_delete(__vpiHandle *handle)
{
      delete dynamic_cast<vpiPortBitInfo *>(handle);
}
#endif

vpiHandle vpiPortBitInfo::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{

      ವಿಕಲ್ಪ (code) {
          ಪಕ್ಷ vpiParent:
            ಪ್ರತ್ಯಾಗಮ parent_;
          ಸ್ವಭಾವ :
            ಭಙ್ಗ;
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ vpiPortBitInfo::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ( code ) {
        ಪಕ್ಷ vpiBit :
          ಪ್ರತ್ಯಾಗಮ bit_;
        ಸ್ವಭಾವ :
          ಭಙ್ಗ;
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
ಶೂನ್ಯ compile_port_info( unsigned index, ಪೂರ್ಣಾಙ್ಕ vpi_direction, unsigned width, ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ* buffer )
{
      vpiPortInfo* obj = ನೂತನ vpiPortInfo( vpip_peek_current_scope(),
                                          index, vpi_direction, width, name, buffer );
      vpip_attach_to_current_scope(obj);

	// Create vpiPortBit objects
      ಪ್ರತಿ (unsigned i=0; i<width; i++) {
	    vpiPortBitInfo* obj_bit = ನೂತನ vpiPortBitInfo(obj, i);
	    obj->add_port_bit(obj_bit);
	    vpip_attach_to_current_scope(obj_bit);
      }
}
