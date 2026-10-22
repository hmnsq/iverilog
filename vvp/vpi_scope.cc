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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <vector>
# include  <cstring>
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"

ಬಳಸಿ ನಾಮಾಕಾಶ std;

ಸ್ಥಿರ vector<vpiHandle> vpip_ಬೇರು_ಕೋಷ್ಟಕ;

ಸ್ಥಿರ vpiHandle make_subset_iterator_(ಪೂರ್ಣಾಂಕ type_ಸಂಕೇತ, ಸ್ಥಿರಾಂಕ vector<vpiHandle>&ಕೋಷ್ಟಕ);

vpiHandle vpip_make_ಬೇರು_iterator(ಪೂರ್ಣಾಂಕ type_ಸಂಕೇತ)
{
      ಹಿಂತಿರುಗಿ make_subset_iterator_(type_ಸಂಕೇತ, vpip_ಬೇರು_ಕೋಷ್ಟಕ);
}

ಶೂನ್ಯ vpip_make_ಬೇರು_iterator(__vpiHandle**&ಕೋಷ್ಟಕ, ಚಿಹ್ನೆರಹಿತ&ntable)
{
      ಕೋಷ್ಟಕ = &vpip_ಬೇರು_ಕೋಷ್ಟಕ[0];
      ntable = vpip_ಬೇರು_ಕೋಷ್ಟಕ.size();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ ಪೋರ್ಟ್_delete(__vpiHandle*handle);
ಶೂನ್ಯ ಪೋರ್ಟ್_ಬಿಟ್_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
ಸ್ಥಿರ class_type **class_list = 0;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ class_list_ಎಣಿಕೆ = 0;

ಸ್ಥಿರ ಶೂನ್ಯ delete_sub_scopes(__vpiScope *scope)
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle ಅಂಶ = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(ಅಂಶ);
	    ಆಯ್ಕೆ(ಅಂಶ->get_type_ಸಂಕೇತ()) {
		ಸ್ಥಿತಿ vpiFunction:
		ಸ್ಥಿತಿ vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		ಸ್ಥಿತಿ vpiModule:
		ಸ್ಥಿತಿ vpiGenScope:
		ಸ್ಥಿತಿ vpiNamedBegin:
		ಸ್ಥಿತಿ vpiNamedFork:
		ಸ್ಥಿತಿ vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  ಅಳಿಸು ಅಂಶ;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpi_ಸ್ಮರಣೆ:
		ಸ್ಥಿತಿ vpiNetArray:
		  ಸ್ಮರಣೆ_delete(ಅಂಶ);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpi_Mod_ಮಾರ್ಗ:
		    /* The destination ModPath is cleaned up later. */
		  ಅಳಿಸು ಅಂಶ;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpi_Inter_Mod_ಮಾರ್ಗ:
		  ಅಳಿಸು ಅಂಶ;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiNamedEvent:
		  named_event_delete(ಅಂಶ);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiNet:
		ಸ್ಥಿತಿ vpiReg:
		ಸ್ಥಿತಿ vpiIntegerVar:
		ಸ್ಥಿತಿ vpiLongIntVar:
		ಸ್ಥಿತಿ vpiShortIntVar:
		ಸ್ಥಿತಿ vpiIntVar:
		ಸ್ಥಿತಿ vpiByteVar:
		ಸ್ಥಿತಿ vpi_ಬಿಟ್_Var:
		  signal_delete(ಅಂಶ);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiParameter:
		  parameter_delete(ಅಂಶ);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiRealVar:
		  real_delete(ಅಂಶ);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiEnumTypespec:
		  enum_delete(ಅಂಶ);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpi_ಪೋರ್ಟ್:
		  ಪೋರ್ಟ್_delete(ಅಂಶ);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpi_ಪೋರ್ಟ್_ಬಿಟ್:
		  ಪೋರ್ಟ್_ಬಿಟ್_delete(ಅಂಶ);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiStringVar:
		  string_delete(ಅಂಶ);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiClassVar:
		  class_delete(ಅಂಶ);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiArrayVar:
		  array_delete(ಅಂಶ);
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  fprintf(stderr, "Need support for type: %d\n",
		          ಅಂಶ->get_type_ಸಂಕೇತ());
		  ಪ್ರತಿಜ್ಞಾ(0);
		  ನಿಲ್ಲಿಸು;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      ಗಾಗಿ (citer = scope->classes.begin();
           citer != scope->classes.ಕೊನೆ(); ++ citer ) {
	    class_list_ಎಣಿಕೆ += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_ಎಣಿಕೆ*ಗಾತ್ರ(class_type **)));
	    class_list[class_list_ಎಣಿಕೆ-1] = citer->second;
      }
}

ಶೂನ್ಯ ಬೇರು_ಕೋಷ್ಟಕ_delete(ಶೂನ್ಯ)
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < vpip_ಬೇರು_ಕೋಷ್ಟಕ.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_ಬೇರು_ಕೋಷ್ಟಕ[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    ಅಳಿಸು scope;
      }
      vpip_ಬೇರು_ಕೋಷ್ಟಕ.clear();

	/* Clean up all the class definitions. */
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < class_list_ಎಣಿಕೆ; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      ಬಿಡುಗಡೆ(class_list);
      class_list = 0;
      class_list_ಎಣಿಕೆ = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ construct_scope_fullname(__vpiScope*ref, ಅಕ್ಷರ*buf)
{
      ಆದರೆ (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    ಆದರೆ (ref->scope->get_type_ಸಂಕೇತ() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      ಆದರೆ (ref->get_type_ಸಂಕೇತ() == vpiPackage) strcat(buf, "::");
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* scope_get_type(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
          ಸ್ಥಿತಿ vpiModule:
            ಹಿಂತಿರುಗಿ "vpiModule";
          ಸ್ಥಿತಿ vpiGenScope:
            ಹಿಂತಿರುಗಿ "vpiGenScope";
          ಸ್ಥಿತಿ vpiFunction:
            ಹಿಂತಿರುಗಿ "vpiFunction";
          ಸ್ಥಿತಿ vpiTask:
            ಹಿಂತಿರುಗಿ "vpiTask";
          ಸ್ಥಿತಿ vpiNamedBegin:
            ಹಿಂತಿರುಗಿ "vpiNamedBegin";
          ಸ್ಥಿತಿ vpiNamedFork:
            ಹಿಂತಿರುಗಿ "vpiNamedFork";
          ಪೂರ್ವನಿಯೋಜಿತ:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            ಹಿಂತಿರುಗಿ ಖಾಲಿ;
      }
}

ಸ್ಥಿರ ಅಕ್ಷರ* scope_get_str(ಪೂರ್ಣಾಂಕ code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      ಪ್ರತಿಜ್ಞಾ(ref);

      ಅಕ್ಷರ buf[4096];  // XXX is a fixed buffer size really reliable?
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p=0;
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiDefFile:
	    p = file_ಹೆಸರುಗಳು[ref->def_file_idx];
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiFile:
	    p = file_ಹೆಸರುಗಳು[ref->file_idx];
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpi_ತುಂಬಿದ_Name:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiName:
	    p = ref->scope_name();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiDefName:
	    p = ref->scope_def_name();
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ vpiType:
	    p = scope_get_type(code);
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
      }
      ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(p);
}

ಸ್ಥಿರ vpiHandle scope_get_handle(ಪೂರ್ಣಾಂಕ code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆಯ್ಕೆ (code) {

	  ಸ್ಥಿತಿ vpiScope:
	    ಹಿಂತಿರುಗಿ rfp->scope;

	  ಸ್ಥಿತಿ vpiModule:
	    ಹಿಂತಿರುಗಿ rfp->scope;
      }

      ಹಿಂತಿರುಗಿ 0;
}

/* compares vpiType's considering object classes */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ compare_types(ಪೂರ್ಣಾಂಕ code, ಪೂರ್ಣಾಂಕ type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      ಆದರೆ (code == vpiScope)
	    ಹಿಂತಿರುಗಿ 1;

      ಆದರೆ (code == type)
	    ಹಿಂತಿರುಗಿ 1;

      ಆದರೆ ( code == vpi_ಆಂತರಿಕ_Scope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    ಹಿಂತಿರುಗಿ 1;

      ಆದರೆ ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpi_ಕಾರ್ಯಕ್ರಮ ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    ಹಿಂತಿರುಗಿ 1;

      ಆದರೆ ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpi_ಬಿಟ್_Var      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpi_ಸಮಯ_Var     ||
	      type == vpiRealVar))
	    ಹಿಂತಿರುಗಿ 1;

      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ vpiHandle make_subset_iterator_(ಪೂರ್ಣಾಂಕ type_ಸಂಕೇತ, ಸ್ಥಿರಾಂಕ vector<vpiHandle>&ಕೋಷ್ಟಕ)
{
      ಚಿಹ್ನೆರಹಿತ mcnt = 0, ncnt = 0;
      vpiHandle*ವಾದಗಳು;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < ಕೋಷ್ಟಕ.size(); idx += 1)
	    ಆದರೆ (compare_types(type_ಸಂಕೇತ, ಕೋಷ್ಟಕ[idx]->get_type_ಸಂಕೇತ()))
		  mcnt += 1;

      ಆದರೆ (mcnt == 0)
	    ಹಿಂತಿರುಗಿ 0;

      ವಾದಗಳು = static_cast<vpiHandle*>(ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(mcnt, ಗಾತ್ರ(vpiHandle)));
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < ಕೋಷ್ಟಕ.size(); idx += 1)
	    ಆದರೆ (compare_types(type_ಸಂಕೇತ, ಕೋಷ್ಟಕ[idx]->get_type_ಸಂಕೇತ()))
		  ವಾದಗಳು[ncnt++] = ಕೋಷ್ಟಕ[idx];

      ಪ್ರತಿಜ್ಞಾ(ncnt == mcnt);

      ಹಿಂತಿರುಗಿ vpip_make_iterator(mcnt, ವಾದಗಳು, ನಿಜ);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
ಸ್ಥಿರ vpiHandle module_iter(ಪೂರ್ಣಾಂಕ code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      ಪ್ರತಿಜ್ಞಾ(ref);

      ಹಿಂತಿರುಗಿ make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nam, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tnam, ತಾರ್ಕಿಕ auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

ಪೂರ್ಣಾಂಕ __vpiScope::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiCellInstance:
	    ಹಿಂತಿರುಗಿ is_cell? 1 : 0;

	  ಸ್ಥಿತಿ vpi_Def_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ def_lineno;

	  ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ lineno;

	  ಸ್ಥಿತಿ vpi_ಸಮಯ_Unit:
	    ಹಿಂತಿರುಗಿ ಸಮಯ_units;

	  ಸ್ಥಿತಿ vpi_ಸಮಯ_Precision:
	    ಹಿಂತಿರುಗಿ ಸಮಯ_precision;

	  ಸ್ಥಿತಿ vpi_ತುದಿ_Module:
	    ಹಿಂತಿರುಗಿ 0x0 == scope;

          ಸ್ಥಿತಿ vpiAutomatic:
	    ಹಿಂತಿರುಗಿ is_automatic_? 1 : 0;
      }

      ಹಿಂತಿರುಗಿ vpiUndefined;
}


ಅಕ್ಷರ*__vpiScope::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ scope_get_str(code, ಇದು); }

vpiHandle __vpiScope::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ scope_get_handle(code, ಇದು); }

vpiHandle __vpiScope::vpi_iterate(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ module_iter(code, ಇದು); }


ವರ್ಗ vpiScopeModule  : ಸಾರ್ವಜನಿಕ __vpiScope {
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ vpiScopeModule(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nam, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tnam)
      : __vpiScope(nam,tnam,ಸುಳ್ಳು) { }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiModule; }
};

ರಚನೆ vpiScopePackage  : ಸಾರ್ವಜನಿಕ __vpiScope {
      ಸಾಲೊಳಗೆ vpiScopePackage(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nam, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tnam)
      : __vpiScope(nam,tnam) { }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiPackage; }
};

ರಚನೆ vpiScopeTask  : ಸಾರ್ವಜನಿಕ __vpiScope {
      ಸಾಲೊಳಗೆ vpiScopeTask(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nam, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tnam)
      : __vpiScope(nam,tnam) { }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiTask; }
};

ರಚನೆ vpiScopeTaskAuto  : ಸಾರ್ವಜನಿಕ __vpiScope {
      ಸಾಲೊಳಗೆ vpiScopeTaskAuto(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nam, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tnam)
      : __vpiScope(nam,tnam,ನಿಜ) {  }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiTask; }
};

ರಚನೆ vpiScopeBegin  : ಸಾರ್ವಜನಿಕ __vpiScope {
      ಸಾಲೊಳಗೆ vpiScopeBegin(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nam, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tnam)
      : __vpiScope(nam,tnam,ಸುಳ್ಳು) { }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiNamedBegin; }
};

ವರ್ಗ vpiScopeBeginAuto  : ಸಾರ್ವಜನಿಕ __vpiScope {
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ vpiScopeBeginAuto(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nam, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tnam)
      : __vpiScope(nam,tnam,ನಿಜ) {  }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiNamedBegin; }
};

ರಚನೆ vpiScopeGenerate  : ಸಾರ್ವಜನಿಕ __vpiScope {
      ಸಾಲೊಳಗೆ vpiScopeGenerate(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nam, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tnam)
      : __vpiScope(nam,tnam) { }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiGenScope; }
};

ರಚನೆ vpiScopeFork  : ಸಾರ್ವಜನಿಕ __vpiScope {
      ಸಾಲೊಳಗೆ vpiScopeFork(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nam, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tnam)
      : __vpiScope(nam,tnam,ಸುಳ್ಳು) { }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiNamedFork; }
};

ವರ್ಗ vpiScopeForkAuto  : ಸಾರ್ವಜನಿಕ __vpiScope {
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ vpiScopeForkAuto(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nam, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tnam)
      : __vpiScope(nam,tnam,ನಿಜ) {  }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiNamedFork; }
};

ರಚನೆ vpiScopeClass  : ಸಾರ್ವಜನಿಕ __vpiScope {
      ಸಾಲೊಳಗೆ vpiScopeClass(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*nam, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*tnam)
      : __vpiScope(nam,tnam) { }
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
ಸ್ಥಿರ __vpiScope*current_scope = 0;

ಶೂನ್ಯ vpip_attach_ಗಮ್ಯ_scope(__vpiScope*scope, vpiHandle obj)
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
compile_scope_decl(ಅಕ್ಷರ*label, ಅಕ್ಷರ*type, ಅಕ್ಷರ*name, ಅಕ್ಷರ*tname,
                   ಅಕ್ಷರ*ಪೋಷಕ, ದೀರ್ಘ file_idx, ದೀರ್ಘ lineno,
                   ದೀರ್ಘ def_file_idx, ದೀರ್ಘ def_lineno, ದೀರ್ಘ is_cell)
{
      ಎಣಿಕೆ_vpi_scopes += 1;
      ಅಕ್ಷರ vec_type;
      ಅಕ್ಷರ sign_flag;
      ಚಿಹ್ನೆರಹಿತ wid;

      __vpiScope*scope;
      ಆದರೆ (strcmp(type,"module") == 0) {
	    scope = ಹೊಸ vpiScopeModule(name, tname);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ( ಅಕ್ಷರಸರಣಿ_ಓದು(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    ಪೂರ್ಣಾಂಕ type_ಸಂಕೇತ;
	    ಆದರೆ (sign_flag=='s') {
		  type_ಸಂಕೇತ = vpiSizedSignedFunc;
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sign_flag=='u') {
		  type_ಸಂಕೇತ = vpiSizedFunc;
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sign_flag=='i') {
		  type_ಸಂಕೇತ = vpiIntFunc;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಪ್ರತಿಜ್ಞಾ(0);
		  type_ಸಂಕೇತ = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = ಹೊಸ vpiScopeFunction(name, tname, ಸುಳ್ಳು, type_ಸಂಕೇತ, wid, init_val);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ( ಅಕ್ಷರಸರಣಿ_ಓದು(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    ಪೂರ್ಣಾಂಕ type_ಸಂಕೇತ;
	    ಆಯ್ಕೆ (sign_flag) {
		ಸ್ಥಿತಿ 's':
		  type_ಸಂಕೇತ = vpiSizedSignedFunc;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'u':
		  type_ಸಂಕೇತ = vpiSizedFunc;
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  ಪ್ರತಿಜ್ಞಾ(0);
		  type_ಸಂಕೇತ = vpiSizedFunc;
		  ನಿಲ್ಲಿಸು;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = ಹೊಸ vpiScopeFunction(name, tname, ನಿಜ, type_ಸಂಕೇತ, wid, init_val);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"function.obj") == 0) {
	    scope = ಹೊಸ vpiScopeFunction(name, tname, ಸುಳ್ಳು, vpiSizedFunc, 0, BIT4_0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"autofunction.obj") == 0) {
	    scope = ಹೊಸ vpiScopeFunction(name, tname, ನಿಜ, vpiSizedFunc, 0, BIT4_0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"function.real") == 0) {
	    scope = ಹೊಸ vpiScopeFunction(name, tname, ಸುಳ್ಳು, vpiRealFunc, 0, BIT4_0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"autofunction.real") == 0) {
	    scope = ಹೊಸ vpiScopeFunction(name, tname, ನಿಜ, vpiRealFunc, 0, BIT4_0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"function.str") == 0) {
	    scope = ಹೊಸ vpiScopeFunction(name, tname, ಸುಳ್ಳು, vpiOtherFunc, 0, BIT4_0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"autofunction.str") == 0) {
	    scope = ಹೊಸ vpiScopeFunction(name, tname, ನಿಜ, vpiOtherFunc, 0, BIT4_0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"function.void") == 0) {
	    scope = ಹೊಸ vpiScopeFunction(name, tname, ಸುಳ್ಳು, vpiOtherFunc, 0, BIT4_0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"autofunction.void") == 0) {
	    scope = ಹೊಸ vpiScopeFunction(name, tname, ನಿಜ, vpiOtherFunc, 0, BIT4_0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"task") == 0) {
	    scope = ಹೊಸ vpiScopeTask(name, tname);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"autotask") == 0) {
	    scope = ಹೊಸ vpiScopeTaskAuto(name, tname);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"fork") == 0) {
	    scope = ಹೊಸ vpiScopeFork(name, tname);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"autofork") == 0) {
	    scope = ಹೊಸ vpiScopeForkAuto(name, tname);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"begin") == 0) {
	    scope = ಹೊಸ vpiScopeBegin(name, tname);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"autobegin") == 0) {
	    scope = ಹೊಸ vpiScopeBeginAuto(name, tname);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"generate") == 0) {
	    scope = ಹೊಸ vpiScopeGenerate(name, tname);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"package") == 0) {
	    scope = ಹೊಸ vpiScopePackage(name, tname);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type,"class") == 0) {
	    scope = ಹೊಸ vpiScopeClass(name, tname);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    scope = ಹೊಸ vpiScopeModule(name, tname);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      scope->file_idx = (ಚಿಹ್ನೆರಹಿತ) file_idx;
      scope->lineno  = (ಚಿಹ್ನೆರಹಿತ) lineno;
      scope->def_file_idx = (ಚಿಹ್ನೆರಹಿತ) def_file_idx;
      scope->def_lineno  = (ಚಿಹ್ನೆರಹಿತ) def_lineno;
      scope->ಅಂಶ = 0;
      scope->nitem = 0;
      scope->ಜೀವಂತ_contexts = 0;
      scope->free_contexts = 0;

      ಆದರೆ (is_cell) scope->is_cell = ನಿಜ;
      ಇಲ್ಲದಿದ್ದರೆ scope->is_cell = ಸುಳ್ಳು;

      current_scope = scope;

      compile_vpi_ಚಿಹ್ನೆ(label, scope);

      ಬಿಡುಗಡೆ(label);
      ಬಿಡುಗಡೆ(type);
      ಅಳಿಸು[] name;
      ಅಳಿಸು[] tname;

      ಆದರೆ (ಪೋಷಕ) {
	    ಸ್ಥಿರ vpiHandle obj;
	    compile_vpi_lookup(&obj, ಪೋಷಕ);
	    ಪ್ರತಿಜ್ಞಾ(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_ಗಮ್ಯ_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->ಸಮಯ_units = sp->ಸಮಯ_units;
	    scope->ಸಮಯ_precision = sp->ಸಮಯ_precision;

      } ಇಲ್ಲದಿದ್ದರೆ {
	    scope->scope = 0x0;

	    vpip_ಬೇರು_ಕೋಷ್ಟಕ.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->ಸಮಯ_units = vpip_get_ಸಮಯ_precision();
	    scope->ಸಮಯ_precision = vpip_get_ಸಮಯ_precision();
      }
}

ಶೂನ್ಯ compile_scope_recall(ಅಕ್ಷರ*ಚಿಹ್ನೆ)
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
ಶೂನ್ಯ compile_timescale(ದೀರ್ಘ units, ದೀರ್ಘ precision)
{
      ಪ್ರತಿಜ್ಞಾ(current_scope);
      current_scope->ಸಮಯ_units = units;
      current_scope->ಸಮಯ_precision = precision;
}

__vpiScope* vpip_peek_current_scope(ಶೂನ್ಯ)
{
      ಹಿಂತಿರುಗಿ current_scope;
}

ಶೂನ್ಯ vpip_attach_ಗಮ್ಯ_current_scope(vpiHandle obj)
{
      vpip_attach_ಗಮ್ಯ_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(ಶೂನ್ಯ)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      ಆಗ (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      ಹಿಂತಿರುಗಿ scope;
}

ಚಿಹ್ನೆರಹಿತ vpip_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_context(automatic_hooks_s*ಅಂಶ,
                                  __vpiScope*scope)
{
      ಪ್ರತಿಜ್ಞಾ(scope);
      ಪ್ರತಿಜ್ಞಾ(scope->is_automatic());

      ಚಿಹ್ನೆರಹಿತ idx = scope->nitem++;

      ಆದರೆ (scope->ಅಂಶ == 0)
	    scope->ಅಂಶ = static_cast<automatic_hooks_s**>
	                  (ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ(automatic_hooks_s*)));
      ಇಲ್ಲದಿದ್ದರೆ
	    scope->ಅಂಶ = static_cast<automatic_hooks_s**>
	                  (realloc(scope->ಅಂಶ,
	                   ಗಾತ್ರ(automatic_hooks_s*)*scope->nitem));

      scope->ಅಂಶ[idx] = ಅಂಶ;

        /* Offset the context index by 2 to leave space for the list links. */
      ಹಿಂತಿರುಗಿ 2 + idx;
}


vpi_ಪೋರ್ಟ್_Info::vpi_ಪೋರ್ಟ್_Info( __vpiScope *ಪೋಷಕ,
              ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ,
              ಪೂರ್ಣಾಂಕ vpi_direction,
              ಚಿಹ್ನೆರಹಿತ width,
              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
              ಅಕ್ಷರ* ಬಫರ್) :
      ಪೋಷಕ_2(ಪೋಷಕ),
      ಸೂಚ್ಯಂಕ_2(ಸೂಚ್ಯಂಕ),
      direction_(vpi_direction),
      ಅಗಲ_2(width),
      name_(name)
{
      ಆದರೆ (ಬಫರ್ != ಶೂನ್ಯಸೂಚಕ) functor_ref_lookup(&ref_, ಬಫರ್);
      ಇಲ್ಲದಿದ್ದರೆ ref_ = ಶೂನ್ಯಸೂಚಕ;
}

vpi_ಪೋರ್ಟ್_Info::~vpi_ಪೋರ್ಟ್_Info()
{
      ಅಳಿಸು[] name_;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ ಪೋರ್ಟ್_delete(__vpiHandle *handle)
{
      ಅಳಿಸು dynamic_cast<vpi_ಪೋರ್ಟ್_Info *>(handle);
}
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ vpi_ಪೋರ್ಟ್_Info::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ( code ) {

        ಸ್ಥಿತಿ vpiDirection :
          ಹಿಂತಿರುಗಿ direction_;
        ಸ್ಥಿತಿ vpi_ಪೋರ್ಟ್_ಸೂಚ್ಯಂಕ :
          ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ_2;
        ಸ್ಥಿತಿ vpi_ಗಾತ್ರ :
          ಹಿಂತಿರುಗಿ ಅಗಲ_2;
        ಪೂರ್ವನಿಯೋಜಿತ :
          ಹಿಂತಿರುಗಿ vpiUndefined;
      }

}


ಅಕ್ಷರ *vpi_ಪೋರ್ಟ್_Info::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ( code ) {
        ಸ್ಥಿತಿ vpiName :
          ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(name_);
        ಪೂರ್ವನಿಯೋಜಿತ :
          ಹಿಂತಿರುಗಿ ಖಾಲಿ;
      }

}


vpiHandle vpi_ಪೋರ್ಟ್_Info::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{

      ಆಯ್ಕೆ (code) {

          ಸ್ಥಿತಿ vpi_ಪೋಷಕ:
          ಸ್ಥಿತಿ vpiScope:
          ಸ್ಥಿತಿ vpiModule:
            ಹಿಂತಿರುಗಿ ಪೋಷಕ_2;
          ಪೂರ್ವನಿಯೋಜಿತ :
            ನಿಲ್ಲಿಸು;
      }

      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ vpiHandle portinfo_iterate(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      vpi_ಪೋರ್ಟ್_Info*rfp = dynamic_cast<vpi_ಪೋರ್ಟ್_Info*>(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);
      ಚಿಹ್ನೆರಹಿತ width = rfp->get_ಅಗಲ();

      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಬಿಟ್: {
	    vpiHandle*ವಾದಗಳು = static_cast<vpiHandle*>(ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(width, ಗಾತ್ರ(vpiHandle*)));

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i<rfp->ಪೋರ್ಟ್_ಬಿಟ್ಟುಗಳು.size(); i++) {
		  ವಾದಗಳು[i] = rfp->ಪೋರ್ಟ್_ಬಿಟ್ಟುಗಳು[i];
	    }

	    ಹಿಂತಿರುಗಿ vpip_make_iterator(width, ವಾದಗಳು, ನಿಜ);
	  }
      }
      ಹಿಂತಿರುಗಿ 0;
}

vpiHandle vpi_ಪೋರ್ಟ್_Info::vpi_iterate(ಪೂರ್ಣಾಂಕ code)
{
      ಹಿಂತಿರುಗಿ portinfo_iterate(code, ಇದು);
}


vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info::vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info(vpi_ಪೋರ್ಟ್_Info *ಪೋಷಕ,
                               ಚಿಹ್ನೆರಹಿತ ಬಿಟ್) :
      ಪೋಷಕ_2(ಪೋಷಕ),
      ಬಿಟ್_2(ಬಿಟ್)
{
}

vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info::~vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info()
{
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ ಪೋರ್ಟ್_ಬಿಟ್_delete(__vpiHandle *handle)
{
      ಅಳಿಸು dynamic_cast<vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info *>(handle);
}
#ಆದರೆ_ಅಂತ್ಯ

vpiHandle vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{

      ಆಯ್ಕೆ (code) {
          ಸ್ಥಿತಿ vpi_ಪೋಷಕ:
            ಹಿಂತಿರುಗಿ ಪೋಷಕ_2;
          ಪೂರ್ವನಿಯೋಜಿತ :
            ನಿಲ್ಲಿಸು;
      }

      ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ( code ) {
        ಸ್ಥಿತಿ vpi_ಬಿಟ್ :
          ಹಿಂತಿರುಗಿ ಬಿಟ್_2;
        ಪೂರ್ವನಿಯೋಜಿತ :
          ನಿಲ್ಲಿಸು;
      }

      ಹಿಂತಿರುಗಿ 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
ಶೂನ್ಯ compile_ಪೋರ್ಟ್_info( ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ, ಪೂರ್ಣಾಂಕ vpi_direction, ಚಿಹ್ನೆರಹಿತ width, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ* ಬಫರ್ )
{
      vpi_ಪೋರ್ಟ್_Info* obj = ಹೊಸ vpi_ಪೋರ್ಟ್_Info( vpip_peek_current_scope(),
                                          ಸೂಚ್ಯಂಕ, vpi_direction, width, name, ಬಫರ್ );
      vpip_attach_ಗಮ್ಯ_current_scope(obj);

	// Create vpiPortBit objects
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i=0; i<width; i++) {
	    vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info* obj_ಬಿಟ್ = ಹೊಸ vpi_ಪೋರ್ಟ್_ಬಿಟ್_Info(obj, i);
	    obj->ಕೂಡಿಸು_ಪೋರ್ಟ್_ಬಿಟ್(obj_ಬಿಟ್);
	    vpip_attach_ಗಮ್ಯ_current_scope(obj_ಬಿಟ್);
      }
}
