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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <vector>
# include  <cstring>
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"

ఉపయోగించి నామాకాశ std;

స్థిర vector<vpiHandle> vpip_మూలం_పట్టిక;

స్థిర vpiHandle make_subset_iterator_(పూర్ణాంకం type_సంకేతం, స్థిరరాశి vector<vpiHandle>&పట్టిక);

vpiHandle vpip_make_మూలం_iterator(పూర్ణాంకం type_సంకేతం)
{
      తిరిగి make_subset_iterator_(type_సంకేతం, vpip_మూలం_పట్టిక);
}

శూన్య vpip_make_మూలం_iterator(__vpiHandle**&పట్టిక, గుర్తులేని&ntable)
{
      పట్టిక = &vpip_మూలం_పట్టిక[0];
      ntable = vpip_మూలం_పట్టిక.size();
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య పోర్టు_delete(__vpiHandle*handle);
శూన్య పోర్టు_బిట్టు_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
స్థిర class_type **class_list = 0;
స్థిర గుర్తులేని class_list_లెక్క = 0;

స్థిర శూన్య delete_sub_scopes(__vpiScope *scope)
{
      కొరకు (గుర్తులేని idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle అంశం = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(అంశం);
	    ఎంపిక(అంశం->get_type_సంకేతం()) {
		స్థితి vpiFunction:
		స్థితి vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		స్థితి vpiModule:
		స్థితి vpiGenScope:
		స్థితి vpiNamedBegin:
		స్థితి vpiNamedFork:
		స్థితి vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  తొలగించు అంశం;
		  ఆపు;
		స్థితి vpi_జ్ఞాపకం:
		స్థితి vpiNetArray:
		  జ్ఞాపకం_delete(అంశం);
		  ఆపు;
		స్థితి vpi_Mod_మార్గం:
		    /* The destination ModPath is cleaned up later. */
		  తొలగించు అంశం;
		  ఆపు;
		స్థితి vpi_Inter_Mod_మార్గం:
		  తొలగించు అంశం;
		  ఆపు;
		స్థితి vpiNamedEvent:
		  named_event_delete(అంశం);
		  ఆపు;
		స్థితి vpiNet:
		స్థితి vpiReg:
		స్థితి vpiIntegerVar:
		స్థితి vpiLongIntVar:
		స్థితి vpiShortIntVar:
		స్థితి vpiIntVar:
		స్థితి vpiByteVar:
		స్థితి vpi_బిట్టు_Var:
		  signal_delete(అంశం);
		  ఆపు;
		స్థితి vpiParameter:
		  parameter_delete(అంశం);
		  ఆపు;
		స్థితి vpiRealVar:
		  real_delete(అంశం);
		  ఆపు;
		స్థితి vpiEnumTypespec:
		  enum_delete(అంశం);
		  ఆపు;
		స్థితి vpi_పోర్టు:
		  పోర్టు_delete(అంశం);
		  ఆపు;
		స్థితి vpi_పోర్టు_బిట్టు:
		  పోర్టు_బిట్టు_delete(అంశం);
		  ఆపు;
		స్థితి vpiStringVar:
		  string_delete(అంశం);
		  ఆపు;
		స్థితి vpiClassVar:
		  class_delete(అంశం);
		  ఆపు;
		స్థితి vpiArrayVar:
		  array_delete(అంశం);
		  ఆపు;
		అప్రమేయ:
		  fprintf(stderr, "Need support for type: %d\n",
		          అంశం->get_type_సంకేతం());
		  ప్రతిజ్ఞా(0);
		  ఆపు;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      కొరకు (citer = scope->classes.begin();
           citer != scope->classes.చివర(); ++ citer ) {
	    class_list_లెక్క += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_లెక్క*పరిమాణం(class_type **)));
	    class_list[class_list_లెక్క-1] = citer->second;
      }
}

శూన్య మూలం_పట్టిక_delete(శూన్య)
{
      కొరకు (గుర్తులేని idx = 0; idx < vpip_మూలం_పట్టిక.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_మూలం_పట్టిక[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    తొలగించు scope;
      }
      vpip_మూలం_పట్టిక.clear();

	/* Clean up all the class definitions. */
      కొరకు (గుర్తులేని idx = 0; idx < class_list_లెక్క; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      విడుదల(class_list);
      class_list = 0;
      class_list_లెక్క = 0;
}
#అయితే_ముగింపు

స్థిర శూన్య construct_scope_fullname(__vpiScope*ref, అక్షరం*buf)
{
      ఒకవేళ (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    ఒకవేళ (ref->scope->get_type_సంకేతం() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      ఒకవేళ (ref->get_type_సంకేతం() == vpiPackage) strcat(buf, "::");
}

స్థిర స్థిరరాశి అక్షరం* scope_get_type(పూర్ణాంకం code)
{
      ఎంపిక (code) {
          స్థితి vpiModule:
            తిరిగి "vpiModule";
          స్థితి vpiGenScope:
            తిరిగి "vpiGenScope";
          స్థితి vpiFunction:
            తిరిగి "vpiFunction";
          స్థితి vpiTask:
            తిరిగి "vpiTask";
          స్థితి vpiNamedBegin:
            తిరిగి "vpiNamedBegin";
          స్థితి vpiNamedFork:
            తిరిగి "vpiNamedFork";
          అప్రమేయ:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            తిరిగి ఖాళీ;
      }
}

స్థిర అక్షరం* scope_get_str(పూర్ణాంకం code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      ప్రతిజ్ఞా(ref);

      అక్షరం buf[4096];  // XXX is a fixed buffer size really reliable?
      స్థిరరాశి అక్షరం *p=0;
      ఎంపిక (code) {
	  స్థితి vpiDefFile:
	    p = file_పేర్లు[ref->def_file_idx];
	    ఆపు;

	  స్థితి vpiFile:
	    p = file_పేర్లు[ref->file_idx];
	    ఆపు;

	  స్థితి vpi_నిండిన_Name:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    ఆపు;

	  స్థితి vpiName:
	    p = ref->scope_name();
	    ఆపు;

	  స్థితి vpiDefName:
	    p = ref->scope_def_name();
	    ఆపు;

	  స్థితి vpiType:
	    p = scope_get_type(code);
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    తిరిగి ఖాళీ;
      }
      తిరిగి simple_సమితి_rbuf_str(p);
}

స్థిర vpiHandle scope_get_handle(పూర్ణాంకం code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      ప్రతిజ్ఞా(rfp);

      ఎంపిక (code) {

	  స్థితి vpiScope:
	    తిరిగి rfp->scope;

	  స్థితి vpiModule:
	    తిరిగి rfp->scope;
      }

      తిరిగి 0;
}

/* compares vpiType's considering object classes */
స్థిర పూర్ణాంకం compare_types(పూర్ణాంకం code, పూర్ణాంకం type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      ఒకవేళ (code == vpiScope)
	    తిరిగి 1;

      ఒకవేళ (code == type)
	    తిరిగి 1;

      ఒకవేళ ( code == vpi_అంతర్గత_Scope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    తిరిగి 1;

      ఒకవేళ ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpi_కార్యక్రమం ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    తిరిగి 1;

      ఒకవేళ ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpi_బిట్టు_Var      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpi_సమయం_Var     ||
	      type == vpiRealVar))
	    తిరిగి 1;

      తిరిగి 0;
}

స్థిర vpiHandle make_subset_iterator_(పూర్ణాంకం type_సంకేతం, స్థిరరాశి vector<vpiHandle>&పట్టిక)
{
      గుర్తులేని mcnt = 0, ncnt = 0;
      vpiHandle*వాదనలు;

      కొరకు (గుర్తులేని idx = 0; idx < పట్టిక.size(); idx += 1)
	    ఒకవేళ (compare_types(type_సంకేతం, పట్టిక[idx]->get_type_సంకేతం()))
		  mcnt += 1;

      ఒకవేళ (mcnt == 0)
	    తిరిగి 0;

      వాదనలు = static_cast<vpiHandle*>(సున్నా_జ్ఞాపకం_ఇవ్వు(mcnt, పరిమాణం(vpiHandle)));
      కొరకు (గుర్తులేని idx = 0; idx < పట్టిక.size(); idx += 1)
	    ఒకవేళ (compare_types(type_సంకేతం, పట్టిక[idx]->get_type_సంకేతం()))
		  వాదనలు[ncnt++] = పట్టిక[idx];

      ప్రతిజ్ఞా(ncnt == mcnt);

      తిరిగి vpip_make_iterator(mcnt, వాదనలు, నిజం);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
స్థిర vpiHandle module_iter(పూర్ణాంకం code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      ప్రతిజ్ఞా(ref);

      తిరిగి make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(స్థిరరాశి అక్షరం*nam, స్థిరరాశి అక్షరం*tnam, తార్కిక auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

పూర్ణాంకం __vpiScope::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpiCellInstance:
	    తిరిగి is_cell? 1 : 0;

	  స్థితి vpi_Def_పంక్తి_No:
	    తిరిగి def_lineno;

	  స్థితి vpi_పంక్తి_No:
	    తిరిగి lineno;

	  స్థితి vpi_సమయం_Unit:
	    తిరిగి సమయం_units;

	  స్థితి vpi_సమయం_Precision:
	    తిరిగి సమయం_precision;

	  స్థితి vpi_శిఖరం_Module:
	    తిరిగి 0x0 == scope;

          స్థితి vpiAutomatic:
	    తిరిగి is_automatic_? 1 : 0;
      }

      తిరిగి vpiUndefined;
}


అక్షరం*__vpiScope::vpi_get_str(పూర్ణాంకం code)
{ తిరిగి scope_get_str(code, ఇది); }

vpiHandle __vpiScope::vpi_handle(పూర్ణాంకం code)
{ తిరిగి scope_get_handle(code, ఇది); }

vpiHandle __vpiScope::vpi_iterate(పూర్ణాంకం code)
{ తిరిగి module_iter(code, ఇది); }


తరగతి vpiScopeModule  : బహిరంగ __vpiScope {
    బహిరంగ:
      అంతఃవరుస vpiScopeModule(స్థిరరాశి అక్షరం*nam, స్థిరరాశి అక్షరం*tnam)
      : __vpiScope(nam,tnam,అబద్ధం) { }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiModule; }
};

నిర్మాణం vpiScopePackage  : బహిరంగ __vpiScope {
      అంతఃవరుస vpiScopePackage(స్థిరరాశి అక్షరం*nam, స్థిరరాశి అక్షరం*tnam)
      : __vpiScope(nam,tnam) { }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiPackage; }
};

నిర్మాణం vpiScopeTask  : బహిరంగ __vpiScope {
      అంతఃవరుస vpiScopeTask(స్థిరరాశి అక్షరం*nam, స్థిరరాశి అక్షరం*tnam)
      : __vpiScope(nam,tnam) { }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiTask; }
};

నిర్మాణం vpiScopeTaskAuto  : బహిరంగ __vpiScope {
      అంతఃవరుస vpiScopeTaskAuto(స్థిరరాశి అక్షరం*nam, స్థిరరాశి అక్షరం*tnam)
      : __vpiScope(nam,tnam,నిజం) {  }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiTask; }
};

నిర్మాణం vpiScopeBegin  : బహిరంగ __vpiScope {
      అంతఃవరుస vpiScopeBegin(స్థిరరాశి అక్షరం*nam, స్థిరరాశి అక్షరం*tnam)
      : __vpiScope(nam,tnam,అబద్ధం) { }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiNamedBegin; }
};

తరగతి vpiScopeBeginAuto  : బహిరంగ __vpiScope {
    బహిరంగ:
      అంతఃవరుస vpiScopeBeginAuto(స్థిరరాశి అక్షరం*nam, స్థిరరాశి అక్షరం*tnam)
      : __vpiScope(nam,tnam,నిజం) {  }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiNamedBegin; }
};

నిర్మాణం vpiScopeGenerate  : బహిరంగ __vpiScope {
      అంతఃవరుస vpiScopeGenerate(స్థిరరాశి అక్షరం*nam, స్థిరరాశి అక్షరం*tnam)
      : __vpiScope(nam,tnam) { }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiGenScope; }
};

నిర్మాణం vpiScopeFork  : బహిరంగ __vpiScope {
      అంతఃవరుస vpiScopeFork(స్థిరరాశి అక్షరం*nam, స్థిరరాశి అక్షరం*tnam)
      : __vpiScope(nam,tnam,అబద్ధం) { }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiNamedFork; }
};

తరగతి vpiScopeForkAuto  : బహిరంగ __vpiScope {
    బహిరంగ:
      అంతఃవరుస vpiScopeForkAuto(స్థిరరాశి అక్షరం*nam, స్థిరరాశి అక్షరం*tnam)
      : __vpiScope(nam,tnam,నిజం) {  }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiNamedFork; }
};

నిర్మాణం vpiScopeClass  : బహిరంగ __vpiScope {
      అంతఃవరుస vpiScopeClass(స్థిరరాశి అక్షరం*nam, స్థిరరాశి అక్షరం*tnam)
      : __vpiScope(nam,tnam) { }
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
స్థిర __vpiScope*current_scope = 0;

శూన్య vpip_attach_గమ్యం_scope(__vpiScope*scope, vpiHandle obj)
{
      ప్రతిజ్ఞా(scope);
      scope->intern.push_back(obj);
}

/*
 * When the compiler encounters a scope declaration, this function
 * creates and initializes a __vpiScope object with the requested name
 * and within the addressed parent. The label is used as a key in the
 * symbol table and the name is used to construct the actual object.
 */
శూన్య
compile_scope_decl(అక్షరం*label, అక్షరం*type, అక్షరం*name, అక్షరం*tname,
                   అక్షరం*మాతృ, దీర్ఘ file_idx, దీర్ఘ lineno,
                   దీర్ఘ def_file_idx, దీర్ఘ def_lineno, దీర్ఘ is_cell)
{
      లెక్క_vpi_scopes += 1;
      అక్షరం vec_type;
      అక్షరం sign_flag;
      గుర్తులేని wid;

      __vpiScope*scope;
      ఒకవేళ (strcmp(type,"module") == 0) {
	    scope = కొత్త vpiScopeModule(name, tname);
      } లేకపోతే ఒకవేళ ( అక్షరశ్రేణి_చదువు(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    పూర్ణాంకం type_సంకేతం;
	    ఒకవేళ (sign_flag=='s') {
		  type_సంకేతం = vpiSizedSignedFunc;
	    } లేకపోతే ఒకవేళ (sign_flag=='u') {
		  type_సంకేతం = vpiSizedFunc;
	    } లేకపోతే ఒకవేళ (sign_flag=='i') {
		  type_సంకేతం = vpiIntFunc;
	    } లేకపోతే {
		  ప్రతిజ్ఞా(0);
		  type_సంకేతం = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = కొత్త vpiScopeFunction(name, tname, అబద్ధం, type_సంకేతం, wid, init_val);

      } లేకపోతే ఒకవేళ ( అక్షరశ్రేణి_చదువు(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    పూర్ణాంకం type_సంకేతం;
	    ఎంపిక (sign_flag) {
		స్థితి 's':
		  type_సంకేతం = vpiSizedSignedFunc;
		  ఆపు;
		స్థితి 'u':
		  type_సంకేతం = vpiSizedFunc;
		  ఆపు;
		అప్రమేయ:
		  ప్రతిజ్ఞా(0);
		  type_సంకేతం = vpiSizedFunc;
		  ఆపు;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = కొత్త vpiScopeFunction(name, tname, నిజం, type_సంకేతం, wid, init_val);

      } లేకపోతే ఒకవేళ (strcmp(type,"function.obj") == 0) {
	    scope = కొత్త vpiScopeFunction(name, tname, అబద్ధం, vpiSizedFunc, 0, BIT4_0);
      } లేకపోతే ఒకవేళ (strcmp(type,"autofunction.obj") == 0) {
	    scope = కొత్త vpiScopeFunction(name, tname, నిజం, vpiSizedFunc, 0, BIT4_0);
      } లేకపోతే ఒకవేళ (strcmp(type,"function.real") == 0) {
	    scope = కొత్త vpiScopeFunction(name, tname, అబద్ధం, vpiRealFunc, 0, BIT4_0);
      } లేకపోతే ఒకవేళ (strcmp(type,"autofunction.real") == 0) {
	    scope = కొత్త vpiScopeFunction(name, tname, నిజం, vpiRealFunc, 0, BIT4_0);
      } లేకపోతే ఒకవేళ (strcmp(type,"function.str") == 0) {
	    scope = కొత్త vpiScopeFunction(name, tname, అబద్ధం, vpiOtherFunc, 0, BIT4_0);
      } లేకపోతే ఒకవేళ (strcmp(type,"autofunction.str") == 0) {
	    scope = కొత్త vpiScopeFunction(name, tname, నిజం, vpiOtherFunc, 0, BIT4_0);
      } లేకపోతే ఒకవేళ (strcmp(type,"function.void") == 0) {
	    scope = కొత్త vpiScopeFunction(name, tname, అబద్ధం, vpiOtherFunc, 0, BIT4_0);
      } లేకపోతే ఒకవేళ (strcmp(type,"autofunction.void") == 0) {
	    scope = కొత్త vpiScopeFunction(name, tname, నిజం, vpiOtherFunc, 0, BIT4_0);
      } లేకపోతే ఒకవేళ (strcmp(type,"task") == 0) {
	    scope = కొత్త vpiScopeTask(name, tname);
      } లేకపోతే ఒకవేళ (strcmp(type,"autotask") == 0) {
	    scope = కొత్త vpiScopeTaskAuto(name, tname);
      } లేకపోతే ఒకవేళ (strcmp(type,"fork") == 0) {
	    scope = కొత్త vpiScopeFork(name, tname);
      } లేకపోతే ఒకవేళ (strcmp(type,"autofork") == 0) {
	    scope = కొత్త vpiScopeForkAuto(name, tname);
      } లేకపోతే ఒకవేళ (strcmp(type,"begin") == 0) {
	    scope = కొత్త vpiScopeBegin(name, tname);
      } లేకపోతే ఒకవేళ (strcmp(type,"autobegin") == 0) {
	    scope = కొత్త vpiScopeBeginAuto(name, tname);
      } లేకపోతే ఒకవేళ (strcmp(type,"generate") == 0) {
	    scope = కొత్త vpiScopeGenerate(name, tname);
      } లేకపోతే ఒకవేళ (strcmp(type,"package") == 0) {
	    scope = కొత్త vpiScopePackage(name, tname);
      } లేకపోతే ఒకవేళ (strcmp(type,"class") == 0) {
	    scope = కొత్త vpiScopeClass(name, tname);
      } లేకపోతే {
	    scope = కొత్త vpiScopeModule(name, tname);
	    ప్రతిజ్ఞా(0);
      }

      scope->file_idx = (గుర్తులేని) file_idx;
      scope->lineno  = (గుర్తులేని) lineno;
      scope->def_file_idx = (గుర్తులేని) def_file_idx;
      scope->def_lineno  = (గుర్తులేని) def_lineno;
      scope->అంశం = 0;
      scope->nitem = 0;
      scope->సజీవ_contexts = 0;
      scope->free_contexts = 0;

      ఒకవేళ (is_cell) scope->is_cell = నిజం;
      లేకపోతే scope->is_cell = అబద్ధం;

      current_scope = scope;

      compile_vpi_చిహ్నం(label, scope);

      విడుదల(label);
      విడుదల(type);
      తొలగించు[] name;
      తొలగించు[] tname;

      ఒకవేళ (మాతృ) {
	    స్థిర vpiHandle obj;
	    compile_vpi_lookup(&obj, మాతృ);
	    ప్రతిజ్ఞా(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_గమ్యం_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->సమయం_units = sp->సమయం_units;
	    scope->సమయం_precision = sp->సమయం_precision;

      } లేకపోతే {
	    scope->scope = 0x0;

	    vpip_మూలం_పట్టిక.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->సమయం_units = vpip_get_సమయం_precision();
	    scope->సమయం_precision = vpip_get_సమయం_precision();
      }
}

శూన్య compile_scope_recall(అక్షరం*చిహ్నం)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(శూన్య*)&current_scope, చిహ్నం);
      ప్రతిజ్ఞా(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
శూన్య compile_timescale(దీర్ఘ units, దీర్ఘ precision)
{
      ప్రతిజ్ఞా(current_scope);
      current_scope->సమయం_units = units;
      current_scope->సమయం_precision = precision;
}

__vpiScope* vpip_peek_current_scope(శూన్య)
{
      తిరిగి current_scope;
}

శూన్య vpip_attach_గమ్యం_current_scope(vpiHandle obj)
{
      vpip_attach_గమ్యం_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(శూన్య)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      అయితే (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      తిరిగి scope;
}

గుర్తులేని vpip_కూడు_అంశం_గమ్యం_context(automatic_hooks_s*అంశం,
                                  __vpiScope*scope)
{
      ప్రతిజ్ఞా(scope);
      ప్రతిజ్ఞా(scope->is_automatic());

      గుర్తులేని idx = scope->nitem++;

      ఒకవేళ (scope->అంశం == 0)
	    scope->అంశం = static_cast<automatic_hooks_s**>
	                  (జ్ఞాపకం_ఇవ్వు(పరిమాణం(automatic_hooks_s*)));
      లేకపోతే
	    scope->అంశం = static_cast<automatic_hooks_s**>
	                  (realloc(scope->అంశం,
	                   పరిమాణం(automatic_hooks_s*)*scope->nitem));

      scope->అంశం[idx] = అంశం;

        /* Offset the context index by 2 to leave space for the list links. */
      తిరిగి 2 + idx;
}


vpi_పోర్టు_Info::vpi_పోర్టు_Info( __vpiScope *మాతృ,
              గుర్తులేని సూచీ,
              పూర్ణాంకం vpi_direction,
              గుర్తులేని width,
              స్థిరరాశి అక్షరం *name,
              అక్షరం* బఫరు) :
      మాతృ_2(మాతృ),
      సూచీ_2(సూచీ),
      direction_(vpi_direction),
      వెడల్పు_2(width),
      name_(name)
{
      ఒకవేళ (బఫరు != శూన్యసూచిక) functor_ref_lookup(&ref_, బఫరు);
      లేకపోతే ref_ = శూన్యసూచిక;
}

vpi_పోర్టు_Info::~vpi_పోర్టు_Info()
{
      తొలగించు[] name_;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య పోర్టు_delete(__vpiHandle *handle)
{
      తొలగించు dynamic_cast<vpi_పోర్టు_Info *>(handle);
}
#అయితే_ముగింపు

పూర్ణాంకం vpi_పోర్టు_Info::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక( code ) {

        స్థితి vpiDirection :
          తిరిగి direction_;
        స్థితి vpi_పోర్టు_సూచీ :
          తిరిగి సూచీ_2;
        స్థితి vpi_కొలత :
          తిరిగి వెడల్పు_2;
        అప్రమేయ :
          తిరిగి vpiUndefined;
      }

}


అక్షరం *vpi_పోర్టు_Info::vpi_get_str(పూర్ణాంకం code)
{
      ఎంపిక( code ) {
        స్థితి vpiName :
          తిరిగి simple_సమితి_rbuf_str(name_);
        అప్రమేయ :
          తిరిగి ఖాళీ;
      }

}


vpiHandle vpi_పోర్టు_Info::vpi_handle(పూర్ణాంకం code)
{

      ఎంపిక (code) {

          స్థితి vpi_మాతృ:
          స్థితి vpiScope:
          స్థితి vpiModule:
            తిరిగి మాతృ_2;
          అప్రమేయ :
            ఆపు;
      }

      తిరిగి 0;
}

స్థిర vpiHandle portinfo_iterate(పూర్ణాంకం code, vpiHandle ref)
{
      vpi_పోర్టు_Info*rfp = dynamic_cast<vpi_పోర్టు_Info*>(ref);
      ప్రతిజ్ఞా(rfp);
      గుర్తులేని width = rfp->get_వెడల్పు();

      ఎంపిక (code) {
	  స్థితి vpi_బిట్టు: {
	    vpiHandle*వాదనలు = static_cast<vpiHandle*>(సున్నా_జ్ఞాపకం_ఇవ్వు(width, పరిమాణం(vpiHandle*)));

	    కొరకు (గుర్తులేని i = 0; i<rfp->పోర్టు_బిట్లు.size(); i++) {
		  వాదనలు[i] = rfp->పోర్టు_బిట్లు[i];
	    }

	    తిరిగి vpip_make_iterator(width, వాదనలు, నిజం);
	  }
      }
      తిరిగి 0;
}

vpiHandle vpi_పోర్టు_Info::vpi_iterate(పూర్ణాంకం code)
{
      తిరిగి portinfo_iterate(code, ఇది);
}


vpi_పోర్టు_బిట్టు_Info::vpi_పోర్టు_బిట్టు_Info(vpi_పోర్టు_Info *మాతృ,
                               గుర్తులేని బిట్టు) :
      మాతృ_2(మాతృ),
      బిట్టు_2(బిట్టు)
{
}

vpi_పోర్టు_బిట్టు_Info::~vpi_పోర్టు_బిట్టు_Info()
{
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య పోర్టు_బిట్టు_delete(__vpiHandle *handle)
{
      తొలగించు dynamic_cast<vpi_పోర్టు_బిట్టు_Info *>(handle);
}
#అయితే_ముగింపు

vpiHandle vpi_పోర్టు_బిట్టు_Info::vpi_handle(పూర్ణాంకం code)
{

      ఎంపిక (code) {
          స్థితి vpi_మాతృ:
            తిరిగి మాతృ_2;
          అప్రమేయ :
            ఆపు;
      }

      తిరిగి 0;
}

పూర్ణాంకం vpi_పోర్టు_బిట్టు_Info::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక( code ) {
        స్థితి vpi_బిట్టు :
          తిరిగి బిట్టు_2;
        అప్రమేయ :
          ఆపు;
      }

      తిరిగి 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
శూన్య compile_పోర్టు_info( గుర్తులేని సూచీ, పూర్ణాంకం vpi_direction, గుర్తులేని width, స్థిరరాశి అక్షరం *name, అక్షరం* బఫరు )
{
      vpi_పోర్టు_Info* obj = కొత్త vpi_పోర్టు_Info( vpip_peek_current_scope(),
                                          సూచీ, vpi_direction, width, name, బఫరు );
      vpip_attach_గమ్యం_current_scope(obj);

	// Create vpiPortBit objects
      కొరకు (గుర్తులేని i=0; i<width; i++) {
	    vpi_పోర్టు_బిట్టు_Info* obj_బిట్టు = కొత్త vpi_పోర్టు_బిట్టు_Info(obj, i);
	    obj->కూడు_పోర్టు_బిట్టు(obj_బిట్టు);
	    vpip_attach_గమ్యం_current_scope(obj_బిట్టు);
      }
}
