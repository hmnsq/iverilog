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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <vector>
# include  <cstring>
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

ਸਥਿਰ vector<vpiHandle> vpip_ਜੜ੍ਹ_ਸਾਰਣੀ;

ਸਥਿਰ vpiHandle make_subset_iterator_(ਪੂਰਨ type_ਕੋਡ, ਅਚਲ vector<vpiHandle>&ਸਾਰਣੀ);

vpiHandle vpip_make_ਜੜ੍ਹ_iterator(ਪੂਰਨ type_ਕੋਡ)
{
      ਵਾਪਸ make_subset_iterator_(type_ਕੋਡ, vpip_ਜੜ੍ਹ_ਸਾਰਣੀ);
}

ਖਾਲੀ vpip_make_ਜੜ੍ਹ_iterator(__vpiHandle**&ਸਾਰਣੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ&ntable)
{
      ਸਾਰਣੀ = &vpip_ਜੜ੍ਹ_ਸਾਰਣੀ[0];
      ntable = vpip_ਜੜ੍ਹ_ਸਾਰਣੀ.size();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ ਪੋਰਟ_delete(__vpiHandle*handle);
ਖਾਲੀ ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
ਸਥਿਰ class_type **class_list = 0;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ class_list_ਗਿਣਤੀ = 0;

ਸਥਿਰ ਖਾਲੀ delete_sub_scopes(__vpiScope *scope)
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle ਵਸਤੂ = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(ਵਸਤੂ);
	    ਚੋਣ(ਵਸਤੂ->get_type_ਕੋਡ()) {
		ਹਾਲਤ vpiFunction:
		ਹਾਲਤ vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		ਹਾਲਤ vpiModule:
		ਹਾਲਤ vpiGenScope:
		ਹਾਲਤ vpiNamedBegin:
		ਹਾਲਤ vpiNamedFork:
		ਹਾਲਤ vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  ਮਿਟਾਓ ਵਸਤੂ;
		  ਰੋਕੋ;
		ਹਾਲਤ vpi_ਸਮ੍ਰਿਤੀ:
		ਹਾਲਤ vpiNetArray:
		  ਸਮ੍ਰਿਤੀ_delete(ਵਸਤੂ);
		  ਰੋਕੋ;
		ਹਾਲਤ vpi_Mod_ਰਸਤਾ:
		    /* The destination ModPath is cleaned up later. */
		  ਮਿਟਾਓ ਵਸਤੂ;
		  ਰੋਕੋ;
		ਹਾਲਤ vpi_Inter_Mod_ਰਸਤਾ:
		  ਮਿਟਾਓ ਵਸਤੂ;
		  ਰੋਕੋ;
		ਹਾਲਤ vpiNamedEvent:
		  named_event_delete(ਵਸਤੂ);
		  ਰੋਕੋ;
		ਹਾਲਤ vpiNet:
		ਹਾਲਤ vpiReg:
		ਹਾਲਤ vpiIntegerVar:
		ਹਾਲਤ vpiLongIntVar:
		ਹਾਲਤ vpiShortIntVar:
		ਹਾਲਤ vpiIntVar:
		ਹਾਲਤ vpiByteVar:
		ਹਾਲਤ vpi_ਦੋਆਧਾਰੀ_ਅੰਕ_Var:
		  signal_delete(ਵਸਤੂ);
		  ਰੋਕੋ;
		ਹਾਲਤ vpiParameter:
		  parameter_delete(ਵਸਤੂ);
		  ਰੋਕੋ;
		ਹਾਲਤ vpiRealVar:
		  real_delete(ਵਸਤੂ);
		  ਰੋਕੋ;
		ਹਾਲਤ vpiEnumTypespec:
		  enum_delete(ਵਸਤੂ);
		  ਰੋਕੋ;
		ਹਾਲਤ vpi_ਪੋਰਟ:
		  ਪੋਰਟ_delete(ਵਸਤੂ);
		  ਰੋਕੋ;
		ਹਾਲਤ vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ:
		  ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_delete(ਵਸਤੂ);
		  ਰੋਕੋ;
		ਹਾਲਤ vpiStringVar:
		  string_delete(ਵਸਤੂ);
		  ਰੋਕੋ;
		ਹਾਲਤ vpiClassVar:
		  class_delete(ਵਸਤੂ);
		  ਰੋਕੋ;
		ਹਾਲਤ vpiArrayVar:
		  array_delete(ਵਸਤੂ);
		  ਰੋਕੋ;
		ਮੂਲ:
		  fprintf(stderr, "Need support for type: %d\n",
		          ਵਸਤੂ->get_type_ਕੋਡ());
		  ਪ੍ਰਤਿਜ੍ਞਾ(0);
		  ਰੋਕੋ;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      ਲਈ (citer = scope->classes.begin();
           citer != scope->classes.ਸਿਰਾ(); ++ citer ) {
	    class_list_ਗਿਣਤੀ += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_ਗਿਣਤੀ*ਆਕਾਰ(class_type **)));
	    class_list[class_list_ਗਿਣਤੀ-1] = citer->second;
      }
}

ਖਾਲੀ ਜੜ੍ਹ_ਸਾਰਣੀ_delete(ਖਾਲੀ)
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < vpip_ਜੜ੍ਹ_ਸਾਰਣੀ.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_ਜੜ੍ਹ_ਸਾਰਣੀ[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    ਮਿਟਾਓ scope;
      }
      vpip_ਜੜ੍ਹ_ਸਾਰਣੀ.clear();

	/* Clean up all the class definitions. */
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < class_list_ਗਿਣਤੀ; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      ਮੁਕਤ(class_list);
      class_list = 0;
      class_list_ਗਿਣਤੀ = 0;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ construct_scope_fullname(__vpiScope*ref, ਅੱਖਰ*buf)
{
      ਜੇ (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    ਜੇ (ref->scope->get_type_ਕੋਡ() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      ਜੇ (ref->get_type_ਕੋਡ() == vpiPackage) strcat(buf, "::");
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* scope_get_type(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
          ਹਾਲਤ vpiModule:
            ਵਾਪਸ "vpiModule";
          ਹਾਲਤ vpiGenScope:
            ਵਾਪਸ "vpiGenScope";
          ਹਾਲਤ vpiFunction:
            ਵਾਪਸ "vpiFunction";
          ਹਾਲਤ vpiTask:
            ਵਾਪਸ "vpiTask";
          ਹਾਲਤ vpiNamedBegin:
            ਵਾਪਸ "vpiNamedBegin";
          ਹਾਲਤ vpiNamedFork:
            ਵਾਪਸ "vpiNamedFork";
          ਮੂਲ:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            ਵਾਪਸ ਨਿਰਾ;
      }
}

ਸਥਿਰ ਅੱਖਰ* scope_get_str(ਪੂਰਨ code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(ref);

      ਅੱਖਰ buf[4096];  // XXX is a fixed buffer size really reliable?
      ਅਚਲ ਅੱਖਰ *p=0;
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiDefFile:
	    p = file_ਨਾਂ[ref->def_file_idx];
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiFile:
	    p = file_ਨਾਂ[ref->file_idx];
	    ਰੋਕੋ;

	  ਹਾਲਤ vpi_ਭਰਿਆ_Name:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiName:
	    p = ref->scope_name();
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiDefName:
	    p = ref->scope_def_name();
	    ਰੋਕੋ;

	  ਹਾਲਤ vpiType:
	    p = scope_get_type(code);
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    ਵਾਪਸ ਨਿਰਾ;
      }
      ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(p);
}

ਸਥਿਰ vpiHandle scope_get_handle(ਪੂਰਨ code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਚੋਣ (code) {

	  ਹਾਲਤ vpiScope:
	    ਵਾਪਸ rfp->scope;

	  ਹਾਲਤ vpiModule:
	    ਵਾਪਸ rfp->scope;
      }

      ਵਾਪਸ 0;
}

/* compares vpiType's considering object classes */
ਸਥਿਰ ਪੂਰਨ compare_types(ਪੂਰਨ code, ਪੂਰਨ type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      ਜੇ (code == vpiScope)
	    ਵਾਪਸ 1;

      ਜੇ (code == type)
	    ਵਾਪਸ 1;

      ਜੇ ( code == vpi_ਅੰਦਰੂਨੀ_Scope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    ਵਾਪਸ 1;

      ਜੇ ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpi_ਪ੍ਰੋਗਰਾਮ ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    ਵਾਪਸ 1;

      ਜੇ ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpi_ਦੋਆਧਾਰੀ_ਅੰਕ_Var      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpi_ਸਮਾਂ_Var     ||
	      type == vpiRealVar))
	    ਵਾਪਸ 1;

      ਵਾਪਸ 0;
}

ਸਥਿਰ vpiHandle make_subset_iterator_(ਪੂਰਨ type_ਕੋਡ, ਅਚਲ vector<vpiHandle>&ਸਾਰਣੀ)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ mcnt = 0, ncnt = 0;
      vpiHandle*ਦਲੀਲਾਂ;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < ਸਾਰਣੀ.size(); idx += 1)
	    ਜੇ (compare_types(type_ਕੋਡ, ਸਾਰਣੀ[idx]->get_type_ਕੋਡ()))
		  mcnt += 1;

      ਜੇ (mcnt == 0)
	    ਵਾਪਸ 0;

      ਦਲੀਲਾਂ = static_cast<vpiHandle*>(ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(mcnt, ਆਕਾਰ(vpiHandle)));
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < ਸਾਰਣੀ.size(); idx += 1)
	    ਜੇ (compare_types(type_ਕੋਡ, ਸਾਰਣੀ[idx]->get_type_ਕੋਡ()))
		  ਦਲੀਲਾਂ[ncnt++] = ਸਾਰਣੀ[idx];

      ਪ੍ਰਤਿਜ੍ਞਾ(ncnt == mcnt);

      ਵਾਪਸ vpip_make_iterator(mcnt, ਦਲੀਲਾਂ, ਸੱਚ);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
ਸਥਿਰ vpiHandle module_iter(ਪੂਰਨ code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(ref);

      ਵਾਪਸ make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(ਅਚਲ ਅੱਖਰ*nam, ਅਚਲ ਅੱਖਰ*tnam, ਤਾਰਕਿਕ auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

ਪੂਰਨ __vpiScope::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiCellInstance:
	    ਵਾਪਸ is_cell? 1 : 0;

	  ਹਾਲਤ vpi_Def_ਸਤਰ_No:
	    ਵਾਪਸ def_lineno;

	  ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ lineno;

	  ਹਾਲਤ vpi_ਸਮਾਂ_Unit:
	    ਵਾਪਸ ਸਮਾਂ_units;

	  ਹਾਲਤ vpi_ਸਮਾਂ_Precision:
	    ਵਾਪਸ ਸਮਾਂ_precision;

	  ਹਾਲਤ vpi_ਸਿਖਰ_Module:
	    ਵਾਪਸ 0x0 == scope;

          ਹਾਲਤ vpiAutomatic:
	    ਵਾਪਸ is_automatic_? 1 : 0;
      }

      ਵਾਪਸ vpiUndefined;
}


ਅੱਖਰ*__vpiScope::vpi_get_str(ਪੂਰਨ code)
{ ਵਾਪਸ scope_get_str(code, ਇਹ); }

vpiHandle __vpiScope::vpi_handle(ਪੂਰਨ code)
{ ਵਾਪਸ scope_get_handle(code, ਇਹ); }

vpiHandle __vpiScope::vpi_iterate(ਪੂਰਨ code)
{ ਵਾਪਸ module_iter(code, ਇਹ); }


ਵਰਗ vpiScopeModule  : ਜਨਤਕ __vpiScope {
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ vpiScopeModule(ਅਚਲ ਅੱਖਰ*nam, ਅਚਲ ਅੱਖਰ*tnam)
      : __vpiScope(nam,tnam,ਝੂਠ) { }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiModule; }
};

ਬਣਤਰ vpiScopePackage  : ਜਨਤਕ __vpiScope {
      ਸਤਰਅੰਦਰ vpiScopePackage(ਅਚਲ ਅੱਖਰ*nam, ਅਚਲ ਅੱਖਰ*tnam)
      : __vpiScope(nam,tnam) { }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiPackage; }
};

ਬਣਤਰ vpiScopeTask  : ਜਨਤਕ __vpiScope {
      ਸਤਰਅੰਦਰ vpiScopeTask(ਅਚਲ ਅੱਖਰ*nam, ਅਚਲ ਅੱਖਰ*tnam)
      : __vpiScope(nam,tnam) { }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiTask; }
};

ਬਣਤਰ vpiScopeTaskAuto  : ਜਨਤਕ __vpiScope {
      ਸਤਰਅੰਦਰ vpiScopeTaskAuto(ਅਚਲ ਅੱਖਰ*nam, ਅਚਲ ਅੱਖਰ*tnam)
      : __vpiScope(nam,tnam,ਸੱਚ) {  }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiTask; }
};

ਬਣਤਰ vpiScopeBegin  : ਜਨਤਕ __vpiScope {
      ਸਤਰਅੰਦਰ vpiScopeBegin(ਅਚਲ ਅੱਖਰ*nam, ਅਚਲ ਅੱਖਰ*tnam)
      : __vpiScope(nam,tnam,ਝੂਠ) { }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiNamedBegin; }
};

ਵਰਗ vpiScopeBeginAuto  : ਜਨਤਕ __vpiScope {
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ vpiScopeBeginAuto(ਅਚਲ ਅੱਖਰ*nam, ਅਚਲ ਅੱਖਰ*tnam)
      : __vpiScope(nam,tnam,ਸੱਚ) {  }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiNamedBegin; }
};

ਬਣਤਰ vpiScopeGenerate  : ਜਨਤਕ __vpiScope {
      ਸਤਰਅੰਦਰ vpiScopeGenerate(ਅਚਲ ਅੱਖਰ*nam, ਅਚਲ ਅੱਖਰ*tnam)
      : __vpiScope(nam,tnam) { }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiGenScope; }
};

ਬਣਤਰ vpiScopeFork  : ਜਨਤਕ __vpiScope {
      ਸਤਰਅੰਦਰ vpiScopeFork(ਅਚਲ ਅੱਖਰ*nam, ਅਚਲ ਅੱਖਰ*tnam)
      : __vpiScope(nam,tnam,ਝੂਠ) { }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiNamedFork; }
};

ਵਰਗ vpiScopeForkAuto  : ਜਨਤਕ __vpiScope {
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ vpiScopeForkAuto(ਅਚਲ ਅੱਖਰ*nam, ਅਚਲ ਅੱਖਰ*tnam)
      : __vpiScope(nam,tnam,ਸੱਚ) {  }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiNamedFork; }
};

ਬਣਤਰ vpiScopeClass  : ਜਨਤਕ __vpiScope {
      ਸਤਰਅੰਦਰ vpiScopeClass(ਅਚਲ ਅੱਖਰ*nam, ਅਚਲ ਅੱਖਰ*tnam)
      : __vpiScope(nam,tnam) { }
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
ਸਥਿਰ __vpiScope*current_scope = 0;

ਖਾਲੀ vpip_attach_ਮੰਜ਼ਿਲ_scope(__vpiScope*scope, vpiHandle obj)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(scope);
      scope->intern.push_back(obj);
}

/*
 * When the compiler encounters a scope declaration, this function
 * creates and initializes a __vpiScope object with the requested name
 * and within the addressed parent. The label is used as a key in the
 * symbol table and the name is used to construct the actual object.
 */
ਖਾਲੀ
compile_scope_decl(ਅੱਖਰ*label, ਅੱਖਰ*type, ਅੱਖਰ*name, ਅੱਖਰ*tname,
                   ਅੱਖਰ*ਮਾਪਾ, ਲੰਮਾ file_idx, ਲੰਮਾ lineno,
                   ਲੰਮਾ def_file_idx, ਲੰਮਾ def_lineno, ਲੰਮਾ is_cell)
{
      ਗਿਣਤੀ_vpi_scopes += 1;
      ਅੱਖਰ vec_type;
      ਅੱਖਰ sign_flag;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid;

      __vpiScope*scope;
      ਜੇ (strcmp(type,"module") == 0) {
	    scope = ਨਵਾਂ vpiScopeModule(name, tname);
      } ਨਹੀਂਤਾਂ ਜੇ ( ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    ਪੂਰਨ type_ਕੋਡ;
	    ਜੇ (sign_flag=='s') {
		  type_ਕੋਡ = vpiSizedSignedFunc;
	    } ਨਹੀਂਤਾਂ ਜੇ (sign_flag=='u') {
		  type_ਕੋਡ = vpiSizedFunc;
	    } ਨਹੀਂਤਾਂ ਜੇ (sign_flag=='i') {
		  type_ਕੋਡ = vpiIntFunc;
	    } ਨਹੀਂਤਾਂ {
		  ਪ੍ਰਤਿਜ੍ਞਾ(0);
		  type_ਕੋਡ = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = ਨਵਾਂ vpiScopeFunction(name, tname, ਝੂਠ, type_ਕੋਡ, wid, init_val);

      } ਨਹੀਂਤਾਂ ਜੇ ( ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    ਪੂਰਨ type_ਕੋਡ;
	    ਚੋਣ (sign_flag) {
		ਹਾਲਤ 's':
		  type_ਕੋਡ = vpiSizedSignedFunc;
		  ਰੋਕੋ;
		ਹਾਲਤ 'u':
		  type_ਕੋਡ = vpiSizedFunc;
		  ਰੋਕੋ;
		ਮੂਲ:
		  ਪ੍ਰਤਿਜ੍ਞਾ(0);
		  type_ਕੋਡ = vpiSizedFunc;
		  ਰੋਕੋ;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = ਨਵਾਂ vpiScopeFunction(name, tname, ਸੱਚ, type_ਕੋਡ, wid, init_val);

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"function.obj") == 0) {
	    scope = ਨਵਾਂ vpiScopeFunction(name, tname, ਝੂਠ, vpiSizedFunc, 0, BIT4_0);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"autofunction.obj") == 0) {
	    scope = ਨਵਾਂ vpiScopeFunction(name, tname, ਸੱਚ, vpiSizedFunc, 0, BIT4_0);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"function.real") == 0) {
	    scope = ਨਵਾਂ vpiScopeFunction(name, tname, ਝੂਠ, vpiRealFunc, 0, BIT4_0);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"autofunction.real") == 0) {
	    scope = ਨਵਾਂ vpiScopeFunction(name, tname, ਸੱਚ, vpiRealFunc, 0, BIT4_0);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"function.str") == 0) {
	    scope = ਨਵਾਂ vpiScopeFunction(name, tname, ਝੂਠ, vpiOtherFunc, 0, BIT4_0);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"autofunction.str") == 0) {
	    scope = ਨਵਾਂ vpiScopeFunction(name, tname, ਸੱਚ, vpiOtherFunc, 0, BIT4_0);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"function.void") == 0) {
	    scope = ਨਵਾਂ vpiScopeFunction(name, tname, ਝੂਠ, vpiOtherFunc, 0, BIT4_0);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"autofunction.void") == 0) {
	    scope = ਨਵਾਂ vpiScopeFunction(name, tname, ਸੱਚ, vpiOtherFunc, 0, BIT4_0);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"task") == 0) {
	    scope = ਨਵਾਂ vpiScopeTask(name, tname);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"autotask") == 0) {
	    scope = ਨਵਾਂ vpiScopeTaskAuto(name, tname);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"fork") == 0) {
	    scope = ਨਵਾਂ vpiScopeFork(name, tname);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"autofork") == 0) {
	    scope = ਨਵਾਂ vpiScopeForkAuto(name, tname);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"begin") == 0) {
	    scope = ਨਵਾਂ vpiScopeBegin(name, tname);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"autobegin") == 0) {
	    scope = ਨਵਾਂ vpiScopeBeginAuto(name, tname);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"generate") == 0) {
	    scope = ਨਵਾਂ vpiScopeGenerate(name, tname);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"package") == 0) {
	    scope = ਨਵਾਂ vpiScopePackage(name, tname);
      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type,"class") == 0) {
	    scope = ਨਵਾਂ vpiScopeClass(name, tname);
      } ਨਹੀਂਤਾਂ {
	    scope = ਨਵਾਂ vpiScopeModule(name, tname);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

      scope->file_idx = (ਬਿਨਾਨਿਸ਼ਾਨੀ) file_idx;
      scope->lineno  = (ਬਿਨਾਨਿਸ਼ਾਨੀ) lineno;
      scope->def_file_idx = (ਬਿਨਾਨਿਸ਼ਾਨੀ) def_file_idx;
      scope->def_lineno  = (ਬਿਨਾਨਿਸ਼ਾਨੀ) def_lineno;
      scope->ਵਸਤੂ = 0;
      scope->nitem = 0;
      scope->ਜੀਵਤ_contexts = 0;
      scope->free_contexts = 0;

      ਜੇ (is_cell) scope->is_cell = ਸੱਚ;
      ਨਹੀਂਤਾਂ scope->is_cell = ਝੂਠ;

      current_scope = scope;

      compile_vpi_ਚਿੰਨ੍ਹ(label, scope);

      ਮੁਕਤ(label);
      ਮੁਕਤ(type);
      ਮਿਟਾਓ[] name;
      ਮਿਟਾਓ[] tname;

      ਜੇ (ਮਾਪਾ) {
	    ਸਥਿਰ vpiHandle obj;
	    compile_vpi_lookup(&obj, ਮਾਪਾ);
	    ਪ੍ਰਤਿਜ੍ਞਾ(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_ਮੰਜ਼ਿਲ_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->ਸਮਾਂ_units = sp->ਸਮਾਂ_units;
	    scope->ਸਮਾਂ_precision = sp->ਸਮਾਂ_precision;

      } ਨਹੀਂਤਾਂ {
	    scope->scope = 0x0;

	    vpip_ਜੜ੍ਹ_ਸਾਰਣੀ.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->ਸਮਾਂ_units = vpip_get_ਸਮਾਂ_precision();
	    scope->ਸਮਾਂ_precision = vpip_get_ਸਮਾਂ_precision();
      }
}

ਖਾਲੀ compile_scope_recall(ਅੱਖਰ*ਚਿੰਨ੍ਹ)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(ਖਾਲੀ*)&current_scope, ਚਿੰਨ੍ਹ);
      ਪ੍ਰਤਿਜ੍ਞਾ(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
ਖਾਲੀ compile_timescale(ਲੰਮਾ units, ਲੰਮਾ precision)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(current_scope);
      current_scope->ਸਮਾਂ_units = units;
      current_scope->ਸਮਾਂ_precision = precision;
}

__vpiScope* vpip_peek_current_scope(ਖਾਲੀ)
{
      ਵਾਪਸ current_scope;
}

ਖਾਲੀ vpip_attach_ਮੰਜ਼ਿਲ_current_scope(vpiHandle obj)
{
      vpip_attach_ਮੰਜ਼ਿਲ_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(ਖਾਲੀ)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      ਜਦੋਂ (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      ਵਾਪਸ scope;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vpip_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_context(automatic_hooks_s*ਵਸਤੂ,
                                  __vpiScope*scope)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(scope);
      ਪ੍ਰਤਿਜ੍ਞਾ(scope->is_automatic());

      ਬਿਨਾਨਿਸ਼ਾਨੀ idx = scope->nitem++;

      ਜੇ (scope->ਵਸਤੂ == 0)
	    scope->ਵਸਤੂ = static_cast<automatic_hooks_s**>
	                  (ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਆਕਾਰ(automatic_hooks_s*)));
      ਨਹੀਂਤਾਂ
	    scope->ਵਸਤੂ = static_cast<automatic_hooks_s**>
	                  (realloc(scope->ਵਸਤੂ,
	                   ਆਕਾਰ(automatic_hooks_s*)*scope->nitem));

      scope->ਵਸਤੂ[idx] = ਵਸਤੂ;

        /* Offset the context index by 2 to leave space for the list links. */
      ਵਾਪਸ 2 + idx;
}


vpi_ਪੋਰਟ_Info::vpi_ਪੋਰਟ_Info( __vpiScope *ਮਾਪਾ,
              ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ,
              ਪੂਰਨ vpi_direction,
              ਬਿਨਾਨਿਸ਼ਾਨੀ width,
              ਅਚਲ ਅੱਖਰ *name,
              ਅੱਖਰ* ਬਫਰ) :
      ਮਾਪਾ_2(ਮਾਪਾ),
      ਸੂਚਕਾਂਕ_2(ਸੂਚਕਾਂਕ),
      direction_(vpi_direction),
      ਚੌੜਾਈ_2(width),
      name_(name)
{
      ਜੇ (ਬਫਰ != ਖਾਲੀ_ਸੂਚਕ) functor_ref_lookup(&ref_, ਬਫਰ);
      ਨਹੀਂਤਾਂ ref_ = ਖਾਲੀ_ਸੂਚਕ;
}

vpi_ਪੋਰਟ_Info::~vpi_ਪੋਰਟ_Info()
{
      ਮਿਟਾਓ[] name_;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ ਪੋਰਟ_delete(__vpiHandle *handle)
{
      ਮਿਟਾਓ dynamic_cast<vpi_ਪੋਰਟ_Info *>(handle);
}
#ਜੇ_ਅੰਤ

ਪੂਰਨ vpi_ਪੋਰਟ_Info::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ( code ) {

        ਹਾਲਤ vpiDirection :
          ਵਾਪਸ direction_;
        ਹਾਲਤ vpi_ਪੋਰਟ_ਸੂਚਕਾਂਕ :
          ਵਾਪਸ ਸੂਚਕਾਂਕ_2;
        ਹਾਲਤ vpi_ਮਾਪ :
          ਵਾਪਸ ਚੌੜਾਈ_2;
        ਮੂਲ :
          ਵਾਪਸ vpiUndefined;
      }

}


ਅੱਖਰ *vpi_ਪੋਰਟ_Info::vpi_get_str(ਪੂਰਨ code)
{
      ਚੋਣ( code ) {
        ਹਾਲਤ vpiName :
          ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(name_);
        ਮੂਲ :
          ਵਾਪਸ ਨਿਰਾ;
      }

}


vpiHandle vpi_ਪੋਰਟ_Info::vpi_handle(ਪੂਰਨ code)
{

      ਚੋਣ (code) {

          ਹਾਲਤ vpi_ਮਾਪਾ:
          ਹਾਲਤ vpiScope:
          ਹਾਲਤ vpiModule:
            ਵਾਪਸ ਮਾਪਾ_2;
          ਮੂਲ :
            ਰੋਕੋ;
      }

      ਵਾਪਸ 0;
}

ਸਥਿਰ vpiHandle portinfo_iterate(ਪੂਰਨ code, vpiHandle ref)
{
      vpi_ਪੋਰਟ_Info*rfp = dynamic_cast<vpi_ਪੋਰਟ_Info*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);
      ਬਿਨਾਨਿਸ਼ਾਨੀ width = rfp->get_ਚੌੜਾਈ();

      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਦੋਆਧਾਰੀ_ਅੰਕ: {
	    vpiHandle*ਦਲੀਲਾਂ = static_cast<vpiHandle*>(ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(width, ਆਕਾਰ(vpiHandle*)));

	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i<rfp->ਪੋਰਟ_ਬਿੱਟ.size(); i++) {
		  ਦਲੀਲਾਂ[i] = rfp->ਪੋਰਟ_ਬਿੱਟ[i];
	    }

	    ਵਾਪਸ vpip_make_iterator(width, ਦਲੀਲਾਂ, ਸੱਚ);
	  }
      }
      ਵਾਪਸ 0;
}

vpiHandle vpi_ਪੋਰਟ_Info::vpi_iterate(ਪੂਰਨ code)
{
      ਵਾਪਸ portinfo_iterate(code, ਇਹ);
}


vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info::vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info(vpi_ਪੋਰਟ_Info *ਮਾਪਾ,
                               ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ) :
      ਮਾਪਾ_2(ਮਾਪਾ),
      ਦੋਆਧਾਰੀ_ਅੰਕ_2(ਦੋਆਧਾਰੀ_ਅੰਕ)
{
}

vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info::~vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info()
{
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_delete(__vpiHandle *handle)
{
      ਮਿਟਾਓ dynamic_cast<vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info *>(handle);
}
#ਜੇ_ਅੰਤ

vpiHandle vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info::vpi_handle(ਪੂਰਨ code)
{

      ਚੋਣ (code) {
          ਹਾਲਤ vpi_ਮਾਪਾ:
            ਵਾਪਸ ਮਾਪਾ_2;
          ਮੂਲ :
            ਰੋਕੋ;
      }

      ਵਾਪਸ 0;
}

ਪੂਰਨ vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ( code ) {
        ਹਾਲਤ vpi_ਦੋਆਧਾਰੀ_ਅੰਕ :
          ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_2;
        ਮੂਲ :
          ਰੋਕੋ;
      }

      ਵਾਪਸ 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
ਖਾਲੀ compile_ਪੋਰਟ_info( ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ, ਪੂਰਨ vpi_direction, ਬਿਨਾਨਿਸ਼ਾਨੀ width, ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ* ਬਫਰ )
{
      vpi_ਪੋਰਟ_Info* obj = ਨਵਾਂ vpi_ਪੋਰਟ_Info( vpip_peek_current_scope(),
                                          ਸੂਚਕਾਂਕ, vpi_direction, width, name, ਬਫਰ );
      vpip_attach_ਮੰਜ਼ਿਲ_current_scope(obj);

	// Create vpiPortBit objects
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i=0; i<width; i++) {
	    vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info* obj_ਦੋਆਧਾਰੀ_ਅੰਕ = ਨਵਾਂ vpi_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_Info(obj, i);
	    obj->ਜੋੜੋ_ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(obj_ਦੋਆਧਾਰੀ_ਅੰਕ);
	    vpip_attach_ਮੰਜ਼ਿਲ_current_scope(obj_ਦੋਆਧਾਰੀ_ਅੰਕ);
      }
}
