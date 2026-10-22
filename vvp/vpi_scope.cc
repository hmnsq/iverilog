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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#جيڪڏهن_پورو
# include  <vector>
# include  <cstring>
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"

استعمال نطاق std;

ساكن vector<vpiHandle> vpip_پاڙ_جدول;

ساكن vpiHandle make_subset_iterator_(صحيح type_ڪوڊ, ثابت vector<vpiHandle>&جدول);

vpiHandle vpip_make_پاڙ_iterator(صحيح type_ڪوڊ)
{
      أرجع make_subset_iterator_(type_ڪوڊ, vpip_پاڙ_جدول);
}

فراغ vpip_make_پاڙ_iterator(__vpiHandle**&جدول, بنانشان&ntable)
{
      جدول = &vpip_پاڙ_جدول[0];
      ntable = vpip_پاڙ_جدول.size();
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ پورٽ_delete(__vpiHandle*handle);
فراغ پورٽ_ٻٽو_انگ_delete(__vpiHandle*handle);

/* Class definitions need to be cleaned up at the end. */
ساكن class_type **class_list = 0;
ساكن بنانشان class_list_ڳڻپ = 0;

ساكن فراغ delete_sub_scopes(__vpiScope *scope)
{
      لكل (بنانشان idx = 0; idx < scope->intern.size(); idx += 1) {
	    vpiHandle شي = (scope->intern)[idx];
	    __vpiScope*lscope = static_cast<__vpiScope*>(شي);
	    بدل(شي->get_type_ڪوڊ()) {
		حالة vpiFunction:
		حالة vpiTask:
		  contexts_delete(lscope);
		  // fallthrough
		حالة vpiModule:
		حالة vpiGenScope:
		حالة vpiNamedBegin:
		حالة vpiNamedFork:
		حالة vpiClassTypespec:
		  delete_sub_scopes(lscope);
		  vthreads_delete(lscope);
		  ميٽيو شي;
		  اكسر;
		حالة vpi_يادگيري:
		حالة vpiNetArray:
		  يادگيري_delete(شي);
		  اكسر;
		حالة vpi_Mod_رستو:
		    /* The destination ModPath is cleaned up later. */
		  ميٽيو شي;
		  اكسر;
		حالة vpi_Inter_Mod_رستو:
		  ميٽيو شي;
		  اكسر;
		حالة vpiNamedEvent:
		  named_event_delete(شي);
		  اكسر;
		حالة vpiNet:
		حالة vpiReg:
		حالة vpiIntegerVar:
		حالة vpiLongIntVar:
		حالة vpiShortIntVar:
		حالة vpiIntVar:
		حالة vpiByteVar:
		حالة vpi_ٻٽو_انگ_Var:
		  signal_delete(شي);
		  اكسر;
		حالة vpiParameter:
		  parameter_delete(شي);
		  اكسر;
		حالة vpiRealVar:
		  real_delete(شي);
		  اكسر;
		حالة vpiEnumTypespec:
		  enum_delete(شي);
		  اكسر;
		حالة vpi_پورٽ:
		  پورٽ_delete(شي);
		  اكسر;
		حالة vpi_پورٽ_ٻٽو_انگ:
		  پورٽ_ٻٽو_انگ_delete(شي);
		  اكسر;
		حالة vpiStringVar:
		  string_delete(شي);
		  اكسر;
		حالة vpiClassVar:
		  class_delete(شي);
		  اكسر;
		حالة vpiArrayVar:
		  array_delete(شي);
		  اكسر;
		افتراضي:
		  fprintf(stderr, "Need support for type: %d\n",
		          شي->get_type_ڪوڊ());
		  أكد(0);
		  اكسر;
	    }
      }
      scope->intern.clear();

	/* Save any class definitions to clean up later. */
      map<std::string, class_type*>::iterator citer;
      لكل (citer = scope->classes.begin();
           citer != scope->classes.پڇاڙي(); ++ citer ) {
	    class_list_ڳڻپ += 1;
	    class_list = static_cast<class_type **>
	                 (realloc(class_list,
	                  class_list_ڳڻپ*حجم(class_type **)));
	    class_list[class_list_ڳڻپ-1] = citer->second;
      }
}

فراغ پاڙ_جدول_delete(فراغ)
{
      لكل (بنانشان idx = 0; idx < vpip_پاڙ_جدول.size(); idx += 1) {
	    __vpiScope *scope = static_cast<__vpiScope *>
	          (vpip_پاڙ_جدول[idx]);
	    vthreads_delete(scope);
	    delete_sub_scopes(scope);
	    ميٽيو scope;
      }
      vpip_پاڙ_جدول.clear();

	/* Clean up all the class definitions. */
      لكل (بنانشان idx = 0; idx < class_list_ڳڻپ; idx += 1) {
            class_def_delete(class_list[idx]);
      }
      حرر(class_list);
      class_list = 0;
      class_list_ڳڻپ = 0;
}
#جيڪڏهن_پورو

ساكن فراغ construct_scope_fullname(__vpiScope*ref, حرف*buf)
{
      إذا (ref->scope) {
	    construct_scope_fullname(ref->scope, buf);
	      // Add a "." separator, unless this is for a package
	    إذا (ref->scope->get_type_ڪوڊ() != vpiPackage) {
		  strcat(buf, ".");
	    }
      }

      strcat(buf, ref->scope_name());
	// For a package, add a "::" to the end
      إذا (ref->get_type_ڪوڊ() == vpiPackage) strcat(buf, "::");
}

ساكن ثابت حرف* scope_get_type(صحيح code)
{
      بدل (code) {
          حالة vpiModule:
            أرجع "vpiModule";
          حالة vpiGenScope:
            أرجع "vpiGenScope";
          حالة vpiFunction:
            أرجع "vpiFunction";
          حالة vpiTask:
            أرجع "vpiTask";
          حالة vpiNamedBegin:
            أرجع "vpiNamedBegin";
          حالة vpiNamedFork:
            أرجع "vpiNamedFork";
          افتراضي:
            fprintf(stderr, "VPI error: invalid scope type code %d.\n", code);
            أرجع عدم;
      }
}

ساكن حرف* scope_get_str(صحيح code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      أكد(ref);

      حرف buf[4096];  // XXX is a fixed buffer size really reliable?
      ثابت حرف *p=0;
      بدل (code) {
	  حالة vpiDefFile:
	    p = file_نالا[ref->def_file_idx];
	    اكسر;

	  حالة vpiFile:
	    p = file_نالا[ref->file_idx];
	    اكسر;

	  حالة vpi_ڀريل_Name:
	    buf[0] = 0;
	    construct_scope_fullname(ref, buf);
	    p = buf;
	    اكسر;

	  حالة vpiName:
	    p = ref->scope_name();
	    اكسر;

	  حالة vpiDefName:
	    p = ref->scope_def_name();
	    اكسر;

	  حالة vpiType:
	    p = scope_get_type(code);
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "VPI error: invalid scope string code %d.\n", code);
	    أرجع عدم;
      }
      أرجع simple_سيٽ_rbuf_str(p);
}

ساكن vpiHandle scope_get_handle(صحيح code, vpiHandle obj)
{
      __vpiScope*rfp = dynamic_cast<__vpiScope*>(obj);
      أكد(rfp);

      بدل (code) {

	  حالة vpiScope:
	    أرجع rfp->scope;

	  حالة vpiModule:
	    أرجع rfp->scope;
      }

      أرجع 0;
}

/* compares vpiType's considering object classes */
ساكن صحيح compare_types(صحيح code, صحيح type)
{
	/* NOTE: The Verilog VPI does not for any object support
	   vpiScope as an iterator parameter, so it is used here as a
	   means to scan everything in the *current* scope. */
      إذا (code == vpiScope)
	    أرجع 1;

      إذا (code == type)
	    أرجع 1;

      إذا ( code == vpi_اندروني_Scope &&
	     (type == vpiModule ||
	     type == vpiGenScope ||
	     type == vpiFunction ||
	     type == vpiTask ||
	     type == vpiNamedBegin ||
	     type == vpiNamedFork) )
	    أرجع 1;

      إذا ( code == vpiInstance &&
	    (type == vpiModule ||
	     type == vpi_پروگرام ||
	     type == vpiInterface ||
	     type == vpiPackage) )
	    أرجع 1;

      إذا ( code == vpiVariables &&
	     (type == vpiIntegerVar  ||
	      type == vpi_ٻٽو_انگ_Var      ||
	      type == vpiByteVar     ||
	      type == vpiShortIntVar ||
	      type == vpiIntVar      ||
	      type == vpiLongIntVar  ||
	      type == vpi_وقت_Var     ||
	      type == vpiRealVar))
	    أرجع 1;

      أرجع 0;
}

ساكن vpiHandle make_subset_iterator_(صحيح type_ڪوڊ, ثابت vector<vpiHandle>&جدول)
{
      بنانشان mcnt = 0, ncnt = 0;
      vpiHandle*دليلون;

      لكل (بنانشان idx = 0; idx < جدول.size(); idx += 1)
	    إذا (compare_types(type_ڪوڊ, جدول[idx]->get_type_ڪوڊ()))
		  mcnt += 1;

      إذا (mcnt == 0)
	    أرجع 0;

      دليلون = static_cast<vpiHandle*>(صفر_يادگيري_ڏيو(mcnt, حجم(vpiHandle)));
      لكل (بنانشان idx = 0; idx < جدول.size(); idx += 1)
	    إذا (compare_types(type_ڪوڊ, جدول[idx]->get_type_ڪوڊ()))
		  دليلون[ncnt++] = جدول[idx];

      أكد(ncnt == mcnt);

      أرجع vpip_make_iterator(mcnt, دليلون, سچ);
}

/*
 * This function implements the vpi_iterate method for vpiModule and
 * similar objects. The vpi_iterate allows the user to iterate over
 * things that are contained in the scope object, by generating an
 * iterator for the requested set of items.
 */
ساكن vpiHandle module_iter(صحيح code, vpiHandle obj)
{
      __vpiScope*ref = dynamic_cast<__vpiScope*>(obj);
      أكد(ref);

      أرجع make_subset_iterator_(code, ref->intern);
}


__vpiScope::__vpiScope(ثابت حرف*nam, ثابت حرف*tnam, منطقي auto_flag)
: is_automatic_(auto_flag)
{
      name_ = vpip_name_string(nam);
      tname_ = vpip_name_string(tnam? tnam : "");
}

صحيح __vpiScope::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiCellInstance:
	    أرجع is_cell? 1 : 0;

	  حالة vpi_Def_سٽ_No:
	    أرجع def_lineno;

	  حالة vpi_سٽ_No:
	    أرجع lineno;

	  حالة vpi_وقت_Unit:
	    أرجع وقت_units;

	  حالة vpi_وقت_Precision:
	    أرجع وقت_precision;

	  حالة vpi_چوٽي_Module:
	    أرجع 0x0 == scope;

          حالة vpiAutomatic:
	    أرجع is_automatic_? 1 : 0;
      }

      أرجع vpiUndefined;
}


حرف*__vpiScope::vpi_get_str(صحيح code)
{ أرجع scope_get_str(code, هذا); }

vpiHandle __vpiScope::vpi_handle(صحيح code)
{ أرجع scope_get_handle(code, هذا); }

vpiHandle __vpiScope::vpi_iterate(صحيح code)
{ أرجع module_iter(code, هذا); }


صنف vpiScopeModule  : عام __vpiScope {
    عام:
      مضمن vpiScopeModule(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,خطأ) { }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiModule; }
};

بنية vpiScopePackage  : عام __vpiScope {
      مضمن vpiScopePackage(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam) { }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiPackage; }
};

بنية vpiScopeTask  : عام __vpiScope {
      مضمن vpiScopeTask(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam) { }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiTask; }
};

بنية vpiScopeTaskAuto  : عام __vpiScope {
      مضمن vpiScopeTaskAuto(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,سچ) {  }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiTask; }
};

بنية vpiScopeBegin  : عام __vpiScope {
      مضمن vpiScopeBegin(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,خطأ) { }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiNamedBegin; }
};

صنف vpiScopeBeginAuto  : عام __vpiScope {
    عام:
      مضمن vpiScopeBeginAuto(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,سچ) {  }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiNamedBegin; }
};

بنية vpiScopeGenerate  : عام __vpiScope {
      مضمن vpiScopeGenerate(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam) { }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiGenScope; }
};

بنية vpiScopeFork  : عام __vpiScope {
      مضمن vpiScopeFork(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,خطأ) { }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiNamedFork; }
};

صنف vpiScopeForkAuto  : عام __vpiScope {
    عام:
      مضمن vpiScopeForkAuto(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam,سچ) {  }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiNamedFork; }
};

بنية vpiScopeClass  : عام __vpiScope {
      مضمن vpiScopeClass(ثابت حرف*nam, ثابت حرف*tnam)
      : __vpiScope(nam,tnam) { }
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiClassTypespec; }
};

/*
 * The current_scope is a compile time concept. As the vvp source is
 * compiled, items that have scope are placed in the current
 * scope. The ".scope" directives select the scope that is current.
 */
ساكن __vpiScope*current_scope = 0;

فراغ vpip_attach_منزل_scope(__vpiScope*scope, vpiHandle obj)
{
      أكد(scope);
      scope->intern.push_back(obj);
}

/*
 * When the compiler encounters a scope declaration, this function
 * creates and initializes a __vpiScope object with the requested name
 * and within the addressed parent. The label is used as a key in the
 * symbol table and the name is used to construct the actual object.
 */
فراغ
compile_scope_decl(حرف*label, حرف*type, حرف*name, حرف*tname,
                   حرف*والد, ڊگهو file_idx, ڊگهو lineno,
                   ڊگهو def_file_idx, ڊگهو def_lineno, ڊگهو is_cell)
{
      ڳڻپ_vpi_scopes += 1;
      حرف vec_type;
      حرف sign_flag;
      بنانشان wid;

      __vpiScope*scope;
      إذا (strcmp(type,"module") == 0) {
	    scope = جديد vpiScopeModule(name, tname);
      } وإلا إذا ( لڙي_مان_پڙهو(type, "function.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    صحيح type_ڪوڊ;
	    إذا (sign_flag=='s') {
		  type_ڪوڊ = vpiSizedSignedFunc;
	    } وإلا إذا (sign_flag=='u') {
		  type_ڪوڊ = vpiSizedFunc;
	    } وإلا إذا (sign_flag=='i') {
		  type_ڪوڊ = vpiIntFunc;
	    } وإلا {
		  أكد(0);
		  type_ڪوڊ = vpiSizedFunc;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = جديد vpiScopeFunction(name, tname, خطأ, type_ڪوڊ, wid, init_val);

      } وإلا إذا ( لڙي_مان_پڙهو(type, "autofunction.vec%c.%c%u", &vec_type, &sign_flag, &wid) == 3 ) {
	    صحيح type_ڪوڊ;
	    بدل (sign_flag) {
		حالة 's':
		  type_ڪوڊ = vpiSizedSignedFunc;
		  اكسر;
		حالة 'u':
		  type_ڪوڊ = vpiSizedFunc;
		  اكسر;
		افتراضي:
		  أكد(0);
		  type_ڪوڊ = vpiSizedFunc;
		  اكسر;
	    }
	    vvp_bit4_t init_val = vec_type == '4' ? BIT4_X : BIT4_0;
	    scope = جديد vpiScopeFunction(name, tname, سچ, type_ڪوڊ, wid, init_val);

      } وإلا إذا (strcmp(type,"function.obj") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, خطأ, vpiSizedFunc, 0, BIT4_0);
      } وإلا إذا (strcmp(type,"autofunction.obj") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, سچ, vpiSizedFunc, 0, BIT4_0);
      } وإلا إذا (strcmp(type,"function.real") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, خطأ, vpiRealFunc, 0, BIT4_0);
      } وإلا إذا (strcmp(type,"autofunction.real") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, سچ, vpiRealFunc, 0, BIT4_0);
      } وإلا إذا (strcmp(type,"function.str") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, خطأ, vpiOtherFunc, 0, BIT4_0);
      } وإلا إذا (strcmp(type,"autofunction.str") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, سچ, vpiOtherFunc, 0, BIT4_0);
      } وإلا إذا (strcmp(type,"function.void") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, خطأ, vpiOtherFunc, 0, BIT4_0);
      } وإلا إذا (strcmp(type,"autofunction.void") == 0) {
	    scope = جديد vpiScopeFunction(name, tname, سچ, vpiOtherFunc, 0, BIT4_0);
      } وإلا إذا (strcmp(type,"task") == 0) {
	    scope = جديد vpiScopeTask(name, tname);
      } وإلا إذا (strcmp(type,"autotask") == 0) {
	    scope = جديد vpiScopeTaskAuto(name, tname);
      } وإلا إذا (strcmp(type,"fork") == 0) {
	    scope = جديد vpiScopeFork(name, tname);
      } وإلا إذا (strcmp(type,"autofork") == 0) {
	    scope = جديد vpiScopeForkAuto(name, tname);
      } وإلا إذا (strcmp(type,"begin") == 0) {
	    scope = جديد vpiScopeBegin(name, tname);
      } وإلا إذا (strcmp(type,"autobegin") == 0) {
	    scope = جديد vpiScopeBeginAuto(name, tname);
      } وإلا إذا (strcmp(type,"generate") == 0) {
	    scope = جديد vpiScopeGenerate(name, tname);
      } وإلا إذا (strcmp(type,"package") == 0) {
	    scope = جديد vpiScopePackage(name, tname);
      } وإلا إذا (strcmp(type,"class") == 0) {
	    scope = جديد vpiScopeClass(name, tname);
      } وإلا {
	    scope = جديد vpiScopeModule(name, tname);
	    أكد(0);
      }

      scope->file_idx = (بنانشان) file_idx;
      scope->lineno  = (بنانشان) lineno;
      scope->def_file_idx = (بنانشان) def_file_idx;
      scope->def_lineno  = (بنانشان) def_lineno;
      scope->شي = 0;
      scope->nitem = 0;
      scope->جيئرو_contexts = 0;
      scope->free_contexts = 0;

      إذا (is_cell) scope->is_cell = سچ;
      وإلا scope->is_cell = خطأ;

      current_scope = scope;

      compile_vpi_نشان(label, scope);

      حرر(label);
      حرر(type);
      ميٽيو[] name;
      ميٽيو[] tname;

      إذا (والد) {
	    ساكن vpiHandle obj;
	    compile_vpi_lookup(&obj, والد);
	    أكد(obj);
	    __vpiScope*sp = dynamic_cast<__vpiScope*>(obj);
	    vpip_attach_منزل_scope(sp, scope);
	    scope->scope = dynamic_cast<__vpiScope*>(obj);

	      /* Inherit time units and precision from the parent scope. */
	    scope->وقت_units = sp->وقت_units;
	    scope->وقت_precision = sp->وقت_precision;

      } وإلا {
	    scope->scope = 0x0;

	    vpip_پاڙ_جدول.push_back(scope);

	      /* Root scopes inherit time_units and precision from the
	         system precision. */
	    scope->وقت_units = vpip_get_وقت_precision();
	    scope->وقت_precision = vpip_get_وقت_precision();
      }
}

فراغ compile_scope_recall(حرف*نشان)
{
	/* A __vpiScope starts with a __vpiHandle structure so this is
	   a safe cast. We need the (void*) to avoid a dereferenced
	   type punned pointer warning from some gcc compilers. */
      compile_vpi_lookup((vpiHandle*)(فراغ*)&current_scope, نشان);
      أكد(current_scope);
}

/*
 * This function handles the ".timescale" directive in the vvp
 * source. It sets in the current scope the specified units value.
 */
فراغ compile_timescale(ڊگهو units, ڊگهو precision)
{
      أكد(current_scope);
      current_scope->وقت_units = units;
      current_scope->وقت_precision = precision;
}

__vpiScope* vpip_peek_current_scope(فراغ)
{
      أرجع current_scope;
}

فراغ vpip_attach_منزل_current_scope(vpiHandle obj)
{
      vpip_attach_منزل_scope(current_scope, obj);
}

__vpiScope* vpip_peek_context_scope(فراغ)
{
      __vpiScope*scope = current_scope;

        /* A context is allocated for each automatic task or function.
           Storage for nested scopes (named blocks) is allocated in
           the parent context. */
      طالما (scope->scope && scope->scope->is_automatic())
            scope = scope->scope;

      أرجع scope;
}

بنانشان vpip_جوڙيو_شي_منزل_context(automatic_hooks_s*شي,
                                  __vpiScope*scope)
{
      أكد(scope);
      أكد(scope->is_automatic());

      بنانشان idx = scope->nitem++;

      إذا (scope->شي == 0)
	    scope->شي = static_cast<automatic_hooks_s**>
	                  (يادگيري_ڏيو(حجم(automatic_hooks_s*)));
      وإلا
	    scope->شي = static_cast<automatic_hooks_s**>
	                  (realloc(scope->شي,
	                   حجم(automatic_hooks_s*)*scope->nitem));

      scope->شي[idx] = شي;

        /* Offset the context index by 2 to leave space for the list links. */
      أرجع 2 + idx;
}


vpi_پورٽ_Info::vpi_پورٽ_Info( __vpiScope *والد,
              بنانشان اشاريو,
              صحيح vpi_direction,
              بنانشان width,
              ثابت حرف *name,
              حرف* بفر) :
      والد_2(والد),
      اشاريو_2(اشاريو),
      direction_(vpi_direction),
      ويڪر_2(width),
      name_(name)
{
      إذا (بفر != خالي_اشارو) functor_ref_lookup(&ref_, بفر);
      وإلا ref_ = خالي_اشارو;
}

vpi_پورٽ_Info::~vpi_پورٽ_Info()
{
      ميٽيو[] name_;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ پورٽ_delete(__vpiHandle *handle)
{
      ميٽيو dynamic_cast<vpi_پورٽ_Info *>(handle);
}
#جيڪڏهن_پورو

صحيح vpi_پورٽ_Info::vpi_get(صحيح code)
{
      بدل( code ) {

        حالة vpiDirection :
          أرجع direction_;
        حالة vpi_پورٽ_اشاريو :
          أرجع اشاريو_2;
        حالة vpi_سائيز :
          أرجع ويڪر_2;
        افتراضي :
          أرجع vpiUndefined;
      }

}


حرف *vpi_پورٽ_Info::vpi_get_str(صحيح code)
{
      بدل( code ) {
        حالة vpiName :
          أرجع simple_سيٽ_rbuf_str(name_);
        افتراضي :
          أرجع عدم;
      }

}


vpiHandle vpi_پورٽ_Info::vpi_handle(صحيح code)
{

      بدل (code) {

          حالة vpi_والد:
          حالة vpiScope:
          حالة vpiModule:
            أرجع والد_2;
          افتراضي :
            اكسر;
      }

      أرجع 0;
}

ساكن vpiHandle portinfo_iterate(صحيح code, vpiHandle ref)
{
      vpi_پورٽ_Info*rfp = dynamic_cast<vpi_پورٽ_Info*>(ref);
      أكد(rfp);
      بنانشان width = rfp->get_ويڪر();

      بدل (code) {
	  حالة vpi_ٻٽو_انگ: {
	    vpiHandle*دليلون = static_cast<vpiHandle*>(صفر_يادگيري_ڏيو(width, حجم(vpiHandle*)));

	    لكل (بنانشان i = 0; i<rfp->پورٽ_بٽ.size(); i++) {
		  دليلون[i] = rfp->پورٽ_بٽ[i];
	    }

	    أرجع vpip_make_iterator(width, دليلون, سچ);
	  }
      }
      أرجع 0;
}

vpiHandle vpi_پورٽ_Info::vpi_iterate(صحيح code)
{
      أرجع portinfo_iterate(code, هذا);
}


vpi_پورٽ_ٻٽو_انگ_Info::vpi_پورٽ_ٻٽو_انگ_Info(vpi_پورٽ_Info *والد,
                               بنانشان ٻٽو_انگ) :
      والد_2(والد),
      ٻٽو_انگ_2(ٻٽو_انگ)
{
}

vpi_پورٽ_ٻٽو_انگ_Info::~vpi_پورٽ_ٻٽو_انگ_Info()
{
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ پورٽ_ٻٽو_انگ_delete(__vpiHandle *handle)
{
      ميٽيو dynamic_cast<vpi_پورٽ_ٻٽو_انگ_Info *>(handle);
}
#جيڪڏهن_پورو

vpiHandle vpi_پورٽ_ٻٽو_انگ_Info::vpi_handle(صحيح code)
{

      بدل (code) {
          حالة vpi_والد:
            أرجع والد_2;
          افتراضي :
            اكسر;
      }

      أرجع 0;
}

صحيح vpi_پورٽ_ٻٽو_انگ_Info::vpi_get(صحيح code)
{
      بدل( code ) {
        حالة vpi_ٻٽو_انگ :
          أرجع ٻٽو_انگ_2;
        افتراضي :
          اكسر;
      }

      أرجع 0;
}

/* Port info is meta-data to allow vpi queries of the port signature of modules for
 * code-generators etc.  There are no actual nets corresponding to instances of module ports
 * as elaboration directly connects nets connected through module ports.
 */
فراغ compile_پورٽ_info( بنانشان اشاريو, صحيح vpi_direction, بنانشان width, ثابت حرف *name, حرف* بفر )
{
      vpi_پورٽ_Info* obj = جديد vpi_پورٽ_Info( vpip_peek_current_scope(),
                                          اشاريو, vpi_direction, width, name, بفر );
      vpip_attach_منزل_current_scope(obj);

	// Create vpiPortBit objects
      لكل (بنانشان i=0; i<width; i++) {
	    vpi_پورٽ_ٻٽو_انگ_Info* obj_ٻٽو_انگ = جديد vpi_پورٽ_ٻٽو_انگ_Info(obj, i);
	    obj->جوڙيو_پورٽ_ٻٽو_انگ(obj_ٻٽو_انگ);
	    vpip_attach_منزل_current_scope(obj_ٻٽو_انگ);
      }
}
